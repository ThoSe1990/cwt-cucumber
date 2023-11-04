
#ifndef cwt_examples_box_step_definition_h
#define cwt_examples_box_step_definition_h

#include "cwt/cucumber.h"

void step_init(int arg_count, cuke_value* args);

void step_open(int arg_count, cuke_value* args);
void step_close(int arg_count, cuke_value* args);
void step_water(int arg_count, cuke_value* args);

void step_is_open(int arg_count, cuke_value* args);
void step_is_closed(int arg_count, cuke_value* args);

void step_front(int arg_count, cuke_value* args);
void step_side(int arg_count, cuke_value* args);
void step_top(int arg_count, cuke_value* args);
void step_volume(int arg_count, cuke_value* args);
void step_weight(int arg_count, cuke_value* args);

#endif