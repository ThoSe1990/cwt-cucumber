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

static void emit_constant(value v)
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

static void emit_byte(uint8_t byte)
{
  write_chunk(current_chunk(), byte, parser.previous.line);
}

static void print_current()
{
  for (int i = 0 ; i < parser.current.length ; i++)
    printf("%c",parser.current.start[i]);
  printf(" ");
}

static void skip_linebreaks()
{
  while (match(TOKEN_LINEBREAK)) { }
}

// TODO End of file!
static void text_until(token_type t)
{
  while(!check(t))
  {
    if (!check(TOKEN_LINEBREAK))
      print_current();
    advance();
    if (check(TOKEN_EOF)) break;
  }
  printf("\n");
}

static void language()
{
// TODO
}

static void steps() 
{
  skip_linebreaks();
  while (match(TOKEN_STEP))
  {
    text_until(TOKEN_LINEBREAK);
    advance();
  }
}

static void scenarios()
{
  skip_linebreaks();
  consume(TOKEN_SCENARIO, "Expect 'Scenario:'.");
  printf("Scenario name: \n");
  text_until(TOKEN_LINEBREAK); 
  printf("Scenario description: \n");
  text_until(TOKEN_STEP); 
  steps();
}
static void feature()
{
  skip_linebreaks();
  consume(TOKEN_FEATURE, "Expect 'Feature'.");
  printf("Feature name: \n");
  text_until(TOKEN_LINEBREAK); 
  printf("Feature description: \n");
  text_until(TOKEN_SCENARIO); 
}




bool compile(const char* source, chunk* c)
{
  init_scanner(source);
  compiling_chunk = c;

  parser.had_error = false;
  
  advance();
  // language(); 
  feature();

  while (!parser.had_error && !match(TOKEN_EOF))
  {
    scenarios();
  }

  end_compiler();
  return !parser.had_error;
}
