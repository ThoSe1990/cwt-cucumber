#ifndef cwt_examples_box_h
#define cwt_examples_box_h

#include <stdbool.h>

void box_init(int w, int h, int d);

void set_box_weight(double weight);
double box_weight();

void set_name(const char* name);
const char* name();

void set_description(const char* description);
const char* description();

int box_front();
int box_side();
int box_top();
int box_volume();

void box_open();
void box_close();

bool box_is_open();

#endif