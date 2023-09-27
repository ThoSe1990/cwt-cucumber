#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "compiler.h"
#include "scanner.h"

#ifdef DEBUG_PRINT_CODE
  #include "debug.h"
#endif

typedef struct {
  token current;
  token previous; 
  bool had_error;
} cwtc_parser;

typedef enum {
  PREC_NONE,
  PREC_CALL,
} precedence_type;

typedef void (*parse_fn)();

typedef struct {
  parse_fn prefix;
  parse_fn infix;
  precedence_type precedence;
} parse_rule;

typedef struct {
  token name; 
  int depth;
} local;

typedef enum {
  TYPE_FUNCTION,
  TYPE_SCRIPT
} function_type;

typedef struct cwtc_compiler {
  struct cwtc_compiler* enclosing;
  obj_function* function;
  function_type type;
  local locals[UINT8_COUNT];
  int local_count;
  int scope_depth;
} cwtc_compiler;


cwtc_parser parser; 
cwtc_compiler* current = NULL;

static chunk* current_chunk()
{
  return &current->function->chunk;
}

static void error_at(token* t, const char* msg)
{
  fprintf(stderr, "[line %i] Error", t->line);
  if (t->type == TOKEN_EOF)
  {
    fprintf(stderr, " at end");
  }
  else if (t->type == TOKEN_ERROR)
  {
    // nothing
  }
  else 
  {
    fprintf(stderr, " at '%.*s'", t->length, t->start);
  }

  fprintf(stderr, ": %s\n", msg);
  parser.had_error = true;
}

static void error(const char* msg) 
{
  error_at(&parser.previous, msg);
}

static void error_at_current(const char* msg) 
{
  error_at(&parser.current, msg);
}

static bool check(token_type type)
{
  return parser.current.type == type;
}

static token_type current_token()
{
  return parser.current.type;
}

static void advance()
{
  parser.previous = parser.current;

  for (;;) 
  {
    parser.current = scan_token();
    if (parser.current.type != TOKEN_ERROR) 
    {
      break;
    }
    error_at_current(parser.current.start);
  }
}


static bool match(token_type type)
{
  if (check(type)) 
  { 
    advance();
    return true;
  }
  else 
  {
    return false; 
  }
}

static void consume(token_type type, const char* msg)
{
  if (parser.current.type == type) 
  {
    advance();
    return;
  }
  error_at_current(msg);
}


static uint8_t make_constant(cwtc_value value)
{
  int constant = add_constant(current_chunk(), value);
  if (constant > UINT8_MAX)
  {
    error("Too many constants in one chunk");
    return 0;
  }
  return (uint8_t)constant;
}
static void emit_byte(uint8_t byte)
{
  write_chunk(current_chunk(), byte, parser.previous.line);
}

static void emit_bytes(uint8_t byte1, uint8_t byte2)
{
  emit_byte(byte1);
  emit_byte(byte2);
}
static void emit_bytes_at(uint8_t byte1, uint8_t byte2, int line)
{
  write_chunk(current_chunk(), byte1, line);
  write_chunk(current_chunk(), byte2, line);
}
static void emit_constant(cwtc_value value)
{
  emit_bytes(OP_CONSTANT, make_constant(value));
}
static int emit_jump(uint8_t instruction)
{
  emit_byte(instruction);
  emit_byte(0xff);
  emit_byte(0xff);
  return current_chunk()->count - 2;
}
static void patch_jump(int offset)
{
  int jump = current_chunk()->count - offset - 2;
  if (jump > UINT16_MAX)
  {
    error("Too much code to jump over.");
  }
  current_chunk()->code[offset] = (jump >> 8) & 0xff;
  current_chunk()->code[offset+1] = jump & 0xff;
}
static void emit_step(const char* name, int length)
{
  emit_bytes(OP_STEP, make_constant(OBJ_VAL(copy_string(name , length))));
}

static int step_name_length(const char* str)
{ 
  const char* begin = str; 
  while (*str != '\n' && *str != '\r' && *str != '\0') { str++; }
  return str-begin;
}

static void emit_scenario(int line)
{
  char buffer[32];
  sprintf(buffer, "%d", line);
  emit_bytes(OP_SCENARIO, make_constant(OBJ_VAL(copy_string(buffer , strlen(buffer)))));
}

static void emit_name(cwtc_value value)
{
  emit_bytes(OP_NAME, make_constant(value));
}
static void emit_description(cwtc_value value)
{
  emit_bytes(OP_DESCRIPTION, make_constant(value));
}

static void emit_int()
{
  long long long_value = strtold(parser.current.start, NULL);
  if (parser.previous.type == TOKEN_MINUS) 
  {
    long_value *= -1;
  }
  emit_constant(LONG_VAL(long_value));
}
static void emit_double()
{
  double double_value = strtod(parser.current.start, NULL);
  if (parser.previous.type == TOKEN_MINUS) 
  {
    double_value *= -1;
  }
  emit_constant(DOUBLE_VAL(double_value));
}

static void emit_string()
{
  emit_constant(OBJ_VAL(copy_string(parser.current.start , parser.current.length)));
}

static void emit_return()
{
  emit_byte(OP_RETURN);
}

static void init_compiler(cwtc_compiler* compiler, function_type type)
{
  compiler->enclosing = current;
  compiler->function = NULL;
  compiler->type = type;
  compiler->local_count = 0;
  compiler->scope_depth = 0; 
  compiler->function = new_function();
  current = compiler;

  if (type != TYPE_SCRIPT)
  {
    char buffer[512];
    sprintf(buffer, "%s:%d", filename(), parser.previous.line);
    current->function->name = copy_string(buffer, strlen(buffer));
  }

  local* l = &current->locals[current->local_count++];
  l->depth = 0;
  l->name.start = "";
  l->name.length = 0;
}
static uint8_t identifier_constant(token* name)
{
  return make_constant(OBJ_VAL(copy_string(name->start, name->length)));
}

// TODO later
static bool identifiers_equal(token* a, token* b)
{
  if (a->length != b->length)
  {
    return false;
  }
  return memcmp(a->start, b->start, a->length) == 0;
}

static int resolve_step(cwtc_compiler* compiler, token* name)
{
  for (int i = compiler->local_count-1 ; i >= 0 ; i--)
  {
    local* l = &compiler->locals[i];
    if (name->line == l->name.line)
    {
      return i;
    }
  }
  return -1;
}

static int resolve_local(cwtc_compiler* compiler, token* name)
{
  for (int i = compiler->local_count-1 ; i >= 0 ; i--)
  {
    local* l = &compiler->locals[i];
    if (identifiers_equal(name, &l->name)) 
    {
      if (l->depth == -1) 
      {
        error("Can't read local variable in its own initializer.");
      }
      return i;
    }
  }
  return -1;
}


// TODO I'll need this later, once there are variables in scenario outlines
static void named_variable(token name, bool can_assign)
{
  uint8_t get_op, set_op;
  int arg = resolve_local(current, &name);
  if (arg != -1) 
  {
    get_op = OP_GET_LOCAL;
    set_op = OP_SET_LOCAL;
  }
  else 
  {
    arg = identifier_constant(&name);
    get_op = OP_GET_GLOBAL;
    set_op = OP_SET_GLOBAL;
  }
  
  if (can_assign)
  {
    emit_bytes(set_op, (uint8_t)arg);
  }
  else 
  {
    emit_bytes(get_op, (uint8_t)arg);
  }
}
static void variable(bool can_assign)
{
  named_variable(parser.previous, can_assign);
}



static obj_function* end_compiler()
{
  emit_return();
  obj_function* func = current->function;

#ifdef DEBUG_PRINT_CODE
  if (!parser.had_error)
  {
    disassemble_chunk(current_chunk(), 
      func->name != NULL ? func->name->chars : "<script>");
  }
#endif
  
  current = current->enclosing;
  return func;
}

static void mark_initialized()
{
  if (current->scope_depth == 0)
  {
    return ;
  }
  else 
  {
    current->locals[current->local_count-1].depth = current->scope_depth;
  }
}


static void define_variable(uint8_t global)
{
  if (current->scope_depth > 0) 
  {
    mark_initialized();
    return ;
  }
  emit_bytes(OP_DEFINE_GLOBAL, global);
}

static void begin_scope()
{
  current->scope_depth++;
}
static void end_scope()
{
  current->scope_depth--;
  while (current->local_count > 0 
    && current->locals[current->local_count-1].depth > current->scope_depth)
  {
    emit_byte(OP_POP);
    current->local_count--;
  }
}

static void skip_linebreaks()
{
  while (match(TOKEN_LINEBREAK)) {}
}

static void language()
{
// TODO parsing #language: en/es/de/etc.. 
}

static void process_step()
{
  const char* step_name = parser.current.start;
  const int length = step_name_length(parser.current.start);

  emit_step(step_name, length);
  int after_step = emit_jump(OP_JUMP_IF_FAILED);
  uint8_t arg_count = 0;
  while(!check(TOKEN_LINEBREAK) && !check(TOKEN_EOF))
  {
    switch (current_token())
    {
      case TOKEN_STRING: emit_string(); arg_count++;
      break; case TOKEN_INT: emit_int(); arg_count++;
      break; case TOKEN_DOUBLE: emit_double(); arg_count++;
    }
    advance();
  }
  emit_bytes(OP_CALL, arg_count);
  patch_jump(after_step);
  emit_bytes(OP_PRINT_RESULT, make_constant(OBJ_VAL(copy_string(step_name , length))));  
}

static void description(token_type end)
{
  const char* begin = parser.current.start;
  const int line = parser.current.line;
  while(!check(end))
  {
    advance();
    if (check(TOKEN_EOF)) break;
  }
  // const char* end = parser.previous.start + parser.previous.length;
  // TODO later we'll need names and descriptions ... 
  // emit_description(OBJ_VAL(copy_string(begin , end-begin)))
}

static void name() 
{
  const char* begin = parser.previous.start;
  const int line = parser.previous.line;
  while(!check(TOKEN_LINEBREAK))
  {
    advance();
    if (check(TOKEN_EOF)) break;
  }
  const char* end = parser.previous.start + parser.previous.length;
  emit_name(OBJ_VAL(copy_string(begin , end-begin)));
}

static void step() 
{
  skip_linebreaks();
  process_step();
  skip_linebreaks();
  if (match(TOKEN_STEP))
  {
    step();
  }
  return ;
}

static void add_local(token name)
{
  if (current->local_count == UINT8_COUNT)
  {
    error("Too many local variables in function.");
  }
  else 
  {
    local* l = &current->locals[current->local_count++];
    l->name = name;
    l->depth = -1;
  }
}

static void scenario()
{
  emit_byte(OP_SCENARIO);
  begin_scope();

  cwtc_compiler compiler;
  init_compiler(&compiler, TYPE_FUNCTION);
  begin_scope();

  name();
  if (!check(TOKEN_STEP))
  {
    description(TOKEN_STEP);
  }
  match(TOKEN_STEP);
  step();

  obj_function* func = end_compiler();
  emit_bytes(OP_CONSTANT, make_constant(OBJ_VAL(func)));
  add_local(parser.current); 
  mark_initialized();
  
  int arg = resolve_step(current, &parser.current);
  emit_bytes(OP_GET_LOCAL, arg);
  emit_bytes(OP_CALL, 0);

  end_scope();
}


static void parse_all_scenarios()
{
  for (;;)
  {
    if (match(TOKEN_EOF)) 
    { 
      break;
    }
    else if (match(TOKEN_SCENARIO))
    {
      scenario();
    }
    else 
    {
      error_at_current("Expect StepLine or Scenario.");
      break;
    }
  }
}


static void feature()
{
  skip_linebreaks();
  consume(TOKEN_FEATURE, "Expect FeatureLine.");

  cwtc_compiler compiler;
  init_compiler(&compiler, TYPE_FUNCTION);
  begin_scope();

  name();
  emit_byte(OP_PRINT_LINEBREAK);
  advance();
  if (!check(TOKEN_SCENARIO)) // TODO add Tags here 
  {
    description(TOKEN_SCENARIO);
  }
  parse_all_scenarios();
  
  obj_function* func = end_compiler();
  emit_bytes(OP_CONSTANT, make_constant(OBJ_VAL(func)));
  define_variable(make_constant(OBJ_VAL(copy_string(func->name->chars, strlen(func->name->chars)))));

  emit_bytes(OP_GET_GLOBAL, make_constant(OBJ_VAL(copy_string(func->name->chars, strlen(func->name->chars)))));
  emit_bytes(OP_CALL, 0);
  emit_byte(OP_POP);
}



obj_function* compile(const char* source, const char* filename)
{
  init_scanner(source, filename);
  cwtc_compiler compiler; 
  init_compiler(&compiler, TYPE_SCRIPT);

  parser.had_error = false;

  advance();
  feature();

  obj_function* func = end_compiler();
  return parser.had_error ? NULL : func;
}
