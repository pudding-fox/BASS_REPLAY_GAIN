#include "scanner.h"

BOOL processor_process_channel(REPLAY_GAIN_CONTEXT* contexts, DWORD context_count, HSTREAM handle);

BOOL processor_init_contexts(REPLAY_GAIN_CONTEXT* contexts, DWORD context_count, DWORD channel_count, DWORD sample_rate);

BOOL processor_process_samples(REPLAY_GAIN_CONTEXT* contexts, DWORD context_count, PFLOAT samples_input, PFLOAT samples_output, DWORD sample_count);

BOOL processor_calc_replaygain(REPLAY_GAIN_CONTEXT* contexts, DWORD context_count);