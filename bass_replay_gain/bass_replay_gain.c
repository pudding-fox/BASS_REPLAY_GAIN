#ifdef _DEBUG
#include <stdio.h>
#endif

#include "bass_replay_gain.h"
#include "processor.h"

//2.4.0.0
#define BASS_VERSION 0x02040000

//I have no idea how to prevent linking against this routine in msvcrt.
//It doesn't exist on Windows XP.
//Hopefully it doesn't do anything important.
int _except_handler4_common() {
	return 0;
}

static const BASS_PLUGININFO plugin_info = { BASS_VERSION, 0, NULL };

BOOL BASSDEF(DllMain)(HANDLE dll, DWORD reason, LPVOID reserved) {
	switch (reason) {
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls((HMODULE)dll);
		if (HIWORD(BASS_GetVersion()) != BASSVERSION || !GetBassFunc()) {
			MessageBoxA(0, "Incorrect BASS.DLL version (" BASSVERSIONTEXT " is required)", "BASS", MB_ICONERROR | MB_OK);
			return FALSE;
		}
		break;
	}
	return TRUE;
}

const VOID* BASSDEF(BASSplugin)(DWORD face) {
	switch (face) {
	case BASSPLUGIN_INFO:
		return (void*)&plugin_info;
	}
	return NULL;
}

BOOL BASSDEF(BASS_REPLAY_GAIN_Process)(HSTREAM Handle, REPLAY_GAIN_INFO* Result) {
	REPLAY_GAIN_CONTEXT contexts[1] = { 0 };

	//Make sure no garbage is returned to the caller.
	Result->handle = 0;
	Result->peak = 0;
	Result->gain = 0;

	//Attempt to process the channel.
	if (!processor_process_channel(contexts, 1, Handle)) {
#if _DEBUG
		printf("Failed to process channel %d.\n", Handle);
#endif
		return FALSE;
	}

	//Success.
	Result->handle = Handle;
	Result->peak = contexts[0].peak;
	Result->gain = contexts[0].gain;

	return TRUE;
}

BOOL BASSDEF(BASS_REPLAY_GAIN_ProcessBatch)(HSTREAM Handles[BATCH_SLOTS], DWORD Length, REPLAY_GAIN_BATCH_INFO* Result) {
	DWORD position = 0;
	//Create two contexts, the first is the batch context and the second is the channel context.
	REPLAY_GAIN_CONTEXT contexts[2] = { 0 };

	//Make sure no garbage is returned to the caller.
	for (position = 0; position < BATCH_SLOTS; position++) {
		memset(&Result->items[position], 0, sizeof(REPLAY_GAIN_INFO));
	}
	Result->peak = 0;
	Result->gain = 0;

	if (Length > BATCH_SLOTS) {
#if _DEBUG
		printf("Batch size is too large.\n");
#endif
		return FALSE;
	}

	//For each channel handle in the batch...
	for (position = 0; position < Length; position++) {

		//Zero the channel context, it will be re-initialized.
		memset(&contexts[1], 0, sizeof(REPLAY_GAIN_CONTEXT));

		//Attempt to process the channel.
		if (!processor_process_channel(contexts, 2, Handles[position])) {
#if _DEBUG
			printf("Failed to process channel %d.\n", Handles[position]);
#endif
			return FALSE;
		}

		//Success.
		Result->items[position].handle = Handles[position];
		Result->items[position].peak = contexts[1].peak;
		Result->items[position].gain = contexts[1].gain;
	}

	//All channels were successful, these are the batch values for peak and gain.
	Result->peak = contexts[0].peak;
	Result->gain = contexts[0].gain;

	return TRUE;
}