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
  bool panic_mode;
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

parser_t parser; 
chunk* compiling_chunk;

static chunk* current_chunk()
{
  return compiling_chunk;
}

static void error_at(token* t, const char* msg)
{
  
  fprintf(stderr, "[line %d] Error", t->line);
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

static void advance()
{
  parser.previous = parser.current;

  for (;;) 
  {
    parser.current = scan_token(false);
    if (parser.current.type != TOKEN_ERROR) 
    {
      break;
    }
    error_at_current(parser.current.start);
  }
}

static bool match(token_type type)
{
  if (!check(type)) 
  { 
    return false; 
  }
  else 
  {
    advance();
    return true;
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

static void emit_constant(value_t v)
{
  printf("emitting constant now ... ");
}


static void end_compiler()
{
#ifdef DEBUG_PRINT_CODE
  if (!parser.had_error)
  {
    disassemble_chunk(current_chunk(), "code");
  }
#endif
}

static void scenarios()
{
  
  if (match(TOKEN_SCENARIO))
  {
    match(TOKEN_STRING);
    match(TOKEN_STRING);
    while (match(TOKEN_STEP))
    {
      match(TOKEN_STRING);
    }
  }
}


bool compile(const char* source, chunk* c)
{
  init_scanner(source);
  compiling_chunk = c;

  parser.had_error = false;

  advance();

  if (match(TOKEN_LANGUAGE)) 
  {
    match(TOKEN_STRING);
  }
  consume(TOKEN_FEATURE, "Expect 'Feature'.");
  // printf("Feature begin:\n");
  if (match(TOKEN_STRING)) printf("  name ...");
  if (match(TOKEN_STRING)) printf("  description ...");  
  

  while (!parser.had_error && !match(TOKEN_EOF))
  {
    scenarios();
  }

  end_compiler();
  return !parser.had_error;
}
