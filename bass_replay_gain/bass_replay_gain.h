#ifndef BASS_REPLAY_GAIN_H
#define BASS_REPLAY_GAIN_H

#include "../bass/bass.h"
#include "../bass/bass_addon.h"

//The maximum number of channel handles that can be submitted as a batch.
#define BATCH_SLOTS 256

typedef struct REPLAY_GAIN_INFO {
	HSTREAM handle;
	FLOAT peak;
	FLOAT gain;
} REPLAY_GAIN_INFO;

typedef struct REPLAY_GAIN_BATCH_INFO {
	FLOAT peak;
	FLOAT gain;
	REPLAY_GAIN_INFO items[BATCH_SLOTS];
} REPLAY_GAIN_BATCH_INFO;

#define HISTOGRAM_SLOTS 12000

typedef struct REPLAY_GAIN_CONTEXT {
	const double* yule_coeff_a;
	const double* yule_coeff_b;
	const double* butter_coeff_a;
	const double* butter_coeff_b;
	DWORD channel_count;
	DWORD sample_rate;
	UINT32 histogram[HISTOGRAM_SLOTS];
	FLOAT yule_hist_a[256];
	FLOAT yule_hist_b[256];
	WORD yule_hist_position;
	FLOAT butter_hist_a[256];
	FLOAT butter_hist_b[256];
	WORD butter_hist_position;
	FLOAT peak;
	FLOAT gain;
	PFLOAT samples_input;
	PFLOAT samples_output;
	DWORD sample_count;
	BOOL is_initialized;
} REPLAY_GAIN_CONTEXT;

BOOL BASSDEF(DllMain)(HANDLE dll, DWORD reason, LPVOID reserved);

const VOID* BASSDEF(BASSplugin)(DWORD face);

BOOL BASSDEF(BASS_REPLAY_GAIN_Process)(HSTREAM Handle, REPLAY_GAIN_INFO* Result);

BOOL BASSDEF(BASS_REPLAY_GAIN_ProcessBatch)(HSTREAM Handles[BATCH_SLOTS], DWORD Length, REPLAY_GAIN_BATCH_INFO* Result);

BOOL BASSDEF(BASS_REPLAY_GAIN_CreateContext)(DWORD Channels, DWORD Rate, REPLAY_GAIN_CONTEXT** Context);

BOOL BASSDEF(BASS_REPLAY_GAIN_PrepareSamples)(REPLAY_GAIN_CONTEXT* Context, PFLOAT* Samples, DWORD Count);

BOOL BASSDEF(BASS_REPLAY_GAIN_ProcessSamples)(REPLAY_GAIN_CONTEXT* Context, DWORD Count);

BOOL BASSDEF(BASS_REPLAY_GAIN_GetResult)(REPLAY_GAIN_CONTEXT* Context, REPLAY_GAIN_INFO* Result);

BOOL BASSDEF(BASS_REPLAY_GAIN_DestroyContext)(REPLAY_GAIN_CONTEXT* Context);

#endif