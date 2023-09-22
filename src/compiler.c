#include <stdio.h>
#include <stdlib.h>

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
} parser_t;

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

typedef struct compiler {
  struct compiler* enclosing;
  obj_function* function;
  function_type type;
  local locals[UINT8_COUNT];
  int local_count;
  int scope_depth;
} compiler;


parser_t parser; 
compiler* current = NULL;

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


static uint8_t make_constant(value v)
{
  int constant = add_constant(current_chunk(), v);
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
static void emit_constant(value v)
{
  emit_bytes(OP_CONSTANT, make_constant(v));
}
// static void emit_step(value v)
static void emit_step(const char* str)
{
  const char* begin = str; 
  while (*str != '\n' && *str != '\r' && *str != '\0') { str++; }
  emit_bytes(OP_STEP, make_constant(OBJ_VAL(copy_string(begin , str-begin))));
}
static void emit_scenario(const char* str)
{
  const char* begin = str; 
  while (*str != '\n' && *str != '\r' && *str != '\0') { str++; }
  // emit_bytes(OP_SCENARIO, make_constant(OBJ_VAL(copy_string(begin , str-begin))));
  emit_bytes(OP_SCENARIO, make_constant(OBJ_VAL(copy_string("my scenario" , 11))));
}
static void emit_name(value v)
{
  emit_bytes(OP_NAME, make_constant(v));
}
static void emit_description(value v)
{
  emit_bytes(OP_DESCRIPTION, make_constant(v));
}

static void emit_int()
{
  double number = strtod(parser.current.start, NULL);
  emit_constant(NUMBER_VAL(number));
}

static void emit_string()
{
  emit_constant(OBJ_VAL(copy_string(parser.current.start , parser.current.length)));
}

static void emit_return()
{
  emit_byte(OP_NIL);
  emit_byte(OP_RETURN);
}

static int compiler_count = 0;
static void init_compiler(compiler* c, function_type type)
{
  compiler_count++;
  printf("compiler count: %d\n", compiler_count);
  c->enclosing = current;
  c->function = NULL;
  c->type = type;
  c->local_count = 0;
  c->scope_depth = 0; 
  c->function = new_function();
  current = c;

  if (type != TYPE_SCRIPT)
  {
    // TODO unique naming for scenario, eg: filename + line
    // current->function->name = copy_string(parser.previous.start, parser.previous.length);
    char buffer[512];
    sprintf(buffer, "%s:%d", filename(), parser.current.line);
    current->function->name = copy_string(buffer, strlen(buffer));
  }

  local* l = &current->locals[current->local_count++];
  l->depth = 0;
  l->name.start = "";
  l->name.length = 0;
}

static obj_function* end_compiler()
{
  compiler_count--;
  printf("compiler count: %d\n", compiler_count);
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
// TODO
}
static void scenario();

static void process_step()
{
  const char* begin = parser.current.start;
  emit_step(parser.current.start);
  uint8_t arg_count = 0;
  while(!check(TOKEN_LINEBREAK) && !check(TOKEN_EOF))
  {
    switch (current_token())
    {
      case TOKEN_STRING: emit_string(); arg_count++;
      break; case TOKEN_INT: emit_int(); arg_count++;
    }
    advance();
  }
  emit_bytes(OP_CALL, arg_count);
  const char* end = parser.previous.start + parser.previous.length;
}

static void create_op_until(op_code code, token_type type)
{
  const char* begin = parser.current.start;
  const int line = parser.current.line;
  while(!check(type))
  {
    advance();
    if (check(TOKEN_EOF)) break;
  }
  const char* end = parser.previous.start + parser.previous.length;
  emit_bytes_at(code, make_constant(OBJ_VAL(copy_string(begin , end-begin))), line);
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


static void scenario()
{
  for (;;)
  {
    if (match(TOKEN_EOF)) 
    { 
      return;
    }
    else if (match(TOKEN_SCENARIO))
    {
      compiler c;
      init_compiler(&c, TYPE_FUNCTION);
      begin_scope();
      // scenario header: push scenario, name, description
      emit_scenario(parser.current.start);
      create_op_until(OP_NAME, TOKEN_LINEBREAK);
      if (!check(TOKEN_STEP))
      {
        create_op_until(OP_DESCRIPTION, TOKEN_STEP);
      }
      match(TOKEN_STEP);
      // once we reach steps we add all steps
      step();
      // end_scope();
      obj_function* func = end_compiler();
      emit_bytes(OP_CONSTANT, make_constant(OBJ_VAL(func)));
      // TODO proof of concept
      define_variable(make_constant(OBJ_VAL(copy_string(func->name->chars, strlen(func->name->chars)))));
      emit_bytes(OP_GET_GLOBAL, make_constant(OBJ_VAL(copy_string(func->name->chars, strlen(func->name->chars)))));
      emit_bytes(OP_CALL, 0);
    }
    else 
    {
      error_at_current("Expect StepLine or Scenario");
      return ;
    }
  }
}

static void feature()
{
  skip_linebreaks();
  consume(TOKEN_FEATURE, "Expect 'Feature:'.");
  emit_byte(OP_FEATURE);
  create_op_until(OP_NAME, TOKEN_LINEBREAK);
  advance();
  if (!match(TOKEN_SCENARIO)) // TODO add Tags here 
  {
    create_op_until(OP_DESCRIPTION, TOKEN_SCENARIO);
  }
  scenario();
}



obj_function* compile(const char* source, const char* filename)
{
  init_scanner(source, filename);
  compiler cmplr; 
  init_compiler(&cmplr, TYPE_SCRIPT);

  parser.had_error = false;

  advance();
  feature();

  obj_function* func = end_compiler();
  return parser.had_error ? NULL : func;
}
