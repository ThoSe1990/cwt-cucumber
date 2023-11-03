#ifndef cwt_examples_box_h
#define cwt_examples_box_h


static int s_width;
static int s_height;
static int s_depth;
static double s_weight;


void box_init(int w, int h, int d);
int box_front();
int box_side();
int box_top();
int box_volume();

#endif