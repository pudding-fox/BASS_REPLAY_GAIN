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

BOOL BASSDEF(DllMain)(HANDLE dll, DWORD reason, LPVOID reserved);

const VOID* BASSDEF(BASSplugin)(DWORD face);

BOOL BASSDEF(BASS_REPLAY_GAIN_Process)(HSTREAM Handle, REPLAY_GAIN_INFO* Result);

BOOL BASSDEF(BASS_REPLAY_GAIN_ProcessBatch)(HSTREAM Handles[BATCH_SLOTS], DWORD Length, REPLAY_GAIN_BATCH_INFO* Result);