#ifndef cwt_cucumber_table_h
#define cwt_cucumber_table_h

#include "common.h"
#include "value.h"

typedef struct {
  obj_string* key;
  cuke_value value;
} entry;

typedef struct {
  int count;
  int capacity;
  entry* entries; 
} table;

void init_table(table* t);
void free_table(table* t);
bool table_set(table* t, obj_string* key, cuke_value value);
bool table_get(table* t, obj_string* key, cuke_value* value);
bool table_set_step(table* t, obj_string* key, cuke_value value);
bool table_get_step(table* t, obj_string* key, cuke_value* value, obj_string* step_definition);
bool table_get_hook(table* t, obj_string* key, cuke_value* v);
bool table_delete(table* t, obj_string* key);
void table_add_all(table* from, table* to);
obj_string* table_find_string(table* t, const char* chars, int length, uint32_t hash);

#endif 
