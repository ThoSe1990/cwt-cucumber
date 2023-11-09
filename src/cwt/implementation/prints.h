#ifndef cwt_cucumber_prints_h
#define cwt_cucumber_prints_h

void print_red(const char* format, ...);
void print_yellow(const char* format, ...);
void print_blue(const char* format, ...);
void print_green(const char* format, ...);
void print_black(const char* format, ...);
void print_white(const char* format, ...);

void start_black();
void end_black();
void start_red_on_stderr();
void end_red_on_stderr();

#endif