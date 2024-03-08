#include "bass_replay_gain.h"

BOOL scanner_init_context(REPLAY_GAIN_CONTEXT* context, DWORD channel_count, DWORD sample_rate);

BOOL scanner_process_samples(REPLAY_GAIN_CONTEXT* context, DWORD sample_count);

BOOL scanner_calc_replaygain(REPLAY_GAIN_CONTEXT* context);