#include "../bass/bass.h"

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
	BOOL is_initialized;
} REPLAY_GAIN_CONTEXT;

BOOL scanner_init_context(REPLAY_GAIN_CONTEXT* context, DWORD channel_count, DWORD sample_rate);

BOOL scanner_process_samples(REPLAY_GAIN_CONTEXT* context, DWORD sample_count);

BOOL scanner_calc_replaygain(REPLAY_GAIN_CONTEXT* context);