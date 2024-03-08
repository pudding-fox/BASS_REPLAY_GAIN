#include "processor.h"

BOOL processor_process_channel(REPLAY_GAIN_CONTEXT* contexts, DWORD context_count, HSTREAM handle) {
	DWORD length;
	DWORD count;
	PFLOAT samples_input;
	PFLOAT samples_output;
	DWORD sample_count;
	BASS_CHANNELINFO info;
	BOOL result;

	//Retrieve the channel info.
	if (!BASS_ChannelGetInfo(handle, &info)) {
#if _DEBUG
		printf("BASS_ChannelGetInfo(%d) failed.\n", handle);
#endif
		return FALSE;
	}

	//Initialize any contexts to the channel specification.
	if (!processor_init_contexts(contexts, context_count, info.chans, info.freq)) {
#if _DEBUG
		printf("Failed to initialize contexts %d/%d.\n", info.chans, info.freq);
#endif
		return FALSE;
	}

	//This value is important for some reason, we need to analyze samples for a certain window.
	length = info.freq / 10;
	samples_input = calloc(length, sizeof(FLOAT));
	if (!samples_input) {
#if _DEBUG
		printf("Out of memory.\n");
#endif
		return FALSE;
	}
	samples_output = calloc(length, sizeof(FLOAT));
	if (!samples_output) {
#if _DEBUG
		printf("Out of memory.\n");
#endif
		return FALSE;
	}

	//We want float samples. If BASS_SAMPLE_FLOAT was provided we will be more accurate.
	length = (length * sizeof(FLOAT)) | BASS_DATA_FLOAT;

	result = TRUE;
	do {
		//Check if there's any more data.
		if (BASS_ChannelIsActive(handle) == BASS_ACTIVE_STOPPED) {
			//There isn't.
			break;
		}

		count = BASS_ChannelGetData(handle, samples_input, length);

		if (count > 0) {

			if (count == BASS_STREAMPROC_END) {
				//Reached the end of a "user" stream.
#if _DEBUG
				printf("BASS_ChannelGetData returned BASS_STREAMPROC_END.\n");
#endif
				break;
			}
			if (count == BASS_ERROR_UNKNOWN) {
				//Something went wrong. We don't know what else might have gone wrong.
#if _DEBUG
				printf("BASS_ChannelGetData returned BASS_ERROR_UNKNOWN.\n");
#endif
				break;
			}

			//BASS_ChannelGetData returns count in bytes, convert to count in samples.
			sample_count = count / sizeof(FLOAT);
			//Attempt to process the samples.
			if (!processor_process_samples(contexts, context_count, samples_input, samples_output, sample_count)) {
				//This can't currently happen but we reserve the right to fail.
#if _DEBUG
				printf("Failed to process samples.\n");
#endif
				result = FALSE;
				break;
			}
		}
	} while (result);

	free(samples_input);
	free(samples_output);

	if (!result) {
		//There was a problem earlier.
		return FALSE;
	}

	//Attempt to calculate replay gain.
	if (!processor_calc_replaygain(contexts, context_count)) {
		//This can't currently happen but we reserve the right to fail.
#if _DEBUG
		printf("Failed to calculate replay gain.\n");
#endif
		return FALSE;
	}

	return TRUE;
}

BOOL processor_init_contexts(REPLAY_GAIN_CONTEXT* contexts, DWORD context_count, DWORD channel_count, DWORD sample_rate) {
	DWORD position;
	for (position = 0; position < context_count; position++) {
		//If a context is already initialized and the channel count and sample rate match we don't need to do anything.
		if (contexts[position].is_initialized) {
			if (contexts[position].channel_count == channel_count && contexts[position].sample_rate == sample_rate) {
				continue;
			}
		}
		//Attempt to initialize the context.
		if (!scanner_init_context(&contexts[position], channel_count, sample_rate)) {
			//Likely an unsupported channel/rate.
#if _DEBUG
			printf("Failed to initialize context %d\n", position);
#endif
			return FALSE;
		}
	}
	return TRUE;
}

BOOL processor_process_samples(REPLAY_GAIN_CONTEXT* contexts, DWORD context_count, PFLOAT samples_input, PFLOAT samples_output, DWORD sample_count) {
	DWORD position;
	BOOL result;
	for (position = 0; position < context_count; position++) {
		//Assign the current buffers to the context.
		contexts[position].samples_input = samples_input;
		contexts[position].samples_output = samples_output;
		contexts[position].sample_count = sample_count;
		//Process them.
		result = scanner_process_samples(&contexts[position], sample_count);
		//Unassign the buffers as we will free them.
		contexts[position].samples_input = NULL;
		contexts[position].samples_output = NULL;
		contexts[position].sample_count = 0;
		if (!result) {
			//This can't currently happen but we reserve the right to fail.
#if _DEBUG
			printf("Failed to process samples for context %d.\n", position);
#endif
			return FALSE;
		}
	}
	return TRUE;
}

BOOL processor_calc_replaygain(REPLAY_GAIN_CONTEXT* contexts, DWORD context_count) {
	DWORD position;
	for (position = 0; position < context_count; position++) {
		//Attempt to calculate replay gain.
		if (!scanner_calc_replaygain(&contexts[position])) {
			//This can't currently happen but we reserve the right to fail.
#if _DEBUG
			printf("Failed to calculate replay gain for context %d.\n", position);
#endif
			return FALSE;
		}
	}
	return TRUE;
}