#ifndef cwt_cucumber_program_options_h
#define cwt_cucumber_program_options_h

#define MAX_LINES 32

void option_lines(int argc, const char* argv[], int current_idx, int out[MAX_LINES], int* count);
void global_options(int argc, const char* argv[]);

#endif