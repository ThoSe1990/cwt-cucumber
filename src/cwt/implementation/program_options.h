#ifndef cwt_cucumber_program_options_h
#define cwt_cucumber_program_options_h

#define MAX_SCENARIOS 32

int get_scenario_lines(int argc, const char* args[], int out[MAX_SCENARIOS]);
void program_options(int argc, const char* argv[]);

#endif