#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tag_scanner.h"
#include "object.h"

typedef struct {
  tag_token_type type;
  const char* start;
  int length;
} tag_token;

typedef struct {
  const char* start;
  const char* current;
} tag_scanner_t;

tag_scanner_t tag_scanner; 


static void init_tag_scanner(const char* tags)
{
  tag_scanner.current = tags;
  tag_scanner.start = tags;
}

static bool is_lower_case_alpha(char c)
{
  return  (c >= 'a' && c <= 'z');
}

static bool is_at_end()
{
  return *tag_scanner.current == '\0';
}

static char peek_next()
{
  if (is_at_end()) 
  {
    return '\0';
  }
  else 
  {
    return tag_scanner.current[1];
  }
}

static bool right_paren()
{
  return *tag_scanner.current == ')'; 
}
static bool is_whitespace()
{
  return *tag_scanner.current == ' ';
}
static bool is_at_sign() 
{
  return *tag_scanner.current == '@';
}
static char advance() 
{
  tag_scanner.current++;
  return tag_scanner.current[-1];
}
static char peek() 
{
  return *tag_scanner.current;
}
static tag_token make_token(tag_token_type type)
{
  tag_token t; 
  t.type = type;
  t.start = tag_scanner.start;
  t.length = (int)(tag_scanner.current - tag_scanner.start);
  return t;
}

static void skip_whitespace()
{
  for(;;)
  {
    char c = peek();
    switch (c)
    {
      case ' ':
      case '\r':
      case '\t':
      case '\n':
        advance();
        break;
      default:
        return;
    }
  }
}
static bool is_alpha(char c)
{
  return  (c >= 'a' && c <= 'z');
}


static tag_token_type check_keyword(int start, int length, const char* rest, tag_token_type type)
{
  if (  tag_scanner.current - tag_scanner.start == start + length && 
        memcmp(tag_scanner.start + start, rest, length) == 0 )
  {
    return  type;
  } 
  else 
  {
    return TAG_TOKEN_ERROR;
  }
}

static tag_token_type identifier_type()
{
  switch (tag_scanner.start[0])
  {
    case 'a': return check_keyword(1,2, "nd", TAG_TOKEN_AND);
    case 'o': return check_keyword(1,1, "r", TAG_TOKEN_OR);
    case 'x': return check_keyword(1,2, "or", TAG_TOKEN_XOR);
    case 'n': return check_keyword(1,2, "ot", TAG_TOKEN_NOT);
    default: 
    {
      return TAG_TOKEN_ERROR;
    }
  }
}

static tag_token identifier()
{
  while (!is_whitespace() && !is_at_end() && !is_at_sign()) 
  {
    advance();
  }
  return make_token(identifier_type());
}
static tag_token make_tag()
{
  while (!is_whitespace() && !right_paren() && !is_at_end())
  {
    advance();
  }
  return make_token(TAG_TOKEN_TAG);
}
static tag_token scan_tag_token()
{
  skip_whitespace();

  tag_scanner.start = tag_scanner.current;

  if (is_at_end())
  {
    return make_token(TAG_TOKEN_EOL);
  }

  char c = advance();

  if (is_alpha(c))
  {
    return identifier();
  }

  switch (c)
  {
    case '(' : return make_token(TAG_TOKEN_LEFT_PAREN);
    case ')' : return make_token(TAG_TOKEN_RIGHT_PAREN);
    case '@' : return make_tag();
  }

  return make_token(TAG_TOKEN_ERROR);
}

#include "value.h"

typedef struct {
  tag_token current;
  tag_token previous; 
  bool had_error;
} tag_parser_t;

tag_parser_t tag_parser; 

static void error(const char* msg, tag_token* t)
{
  fprintf(stderr, "\x1b[31m");
  fprintf(stderr, "Tag-Error at: '%.*s': ",t->length, t->start);
  fprintf(stderr,"%s\n", msg);
  fprintf(stderr,"\x1b[0m");
  tag_parser.had_error = true;
}

static void error_at_current(const char* msg)
{
  error(msg, &tag_parser.current);
}

static void next_token()
{
  tag_parser.previous = tag_parser.current;

  for (;;) 
  {
    tag_parser.current = scan_tag_token();
    if (tag_parser.current.type != TAG_TOKEN_ERROR) 
    {
      break;
    }
    else 
    {
      error_at_current("Unexpected identifier");
      return ;
    }
  }
}

static bool check(tag_token_type type)
{
  return tag_parser.current.type == type;
}

static bool match(tag_token_type type)
{
  if (check(type)) 
  { 
    next_token();
    return true;
  }
  else 
  {
    return false; 
  }
}


typedef struct 
{
  cuke_value out[TAGS_RPN_MAX];
  cuke_value* out_top;
  cuke_value operators[TAGS_RPN_MAX];
  cuke_value* operators_top;
} tag_values_t;

tag_values_t tag_values;

void push_operator(cuke_value value)
{
  *tag_values.operators_top = value;
  tag_values.operators_top++;
}
cuke_value pop_operator()
{
  tag_values.operators_top--;
  return *tag_values.operators_top;
}

cuke_value token_to_cuke_value()
{
  switch (tag_parser.previous.type)
  {
    case TAG_TOKEN_TAG: return OBJ_VAL(copy_string(tag_parser.previous.start, tag_parser.previous.length));
    case TAG_TOKEN_AND: return LONG_VAL(TAG_TOKEN_AND);
    case TAG_TOKEN_OR: return LONG_VAL(TAG_TOKEN_OR);
    case TAG_TOKEN_XOR: return LONG_VAL(TAG_TOKEN_XOR);
    case TAG_TOKEN_NOT: return LONG_VAL(TAG_TOKEN_NOT);
    case TAG_TOKEN_LEFT_PAREN: return LONG_VAL(TAG_TOKEN_LEFT_PAREN);
    case TAG_TOKEN_RIGHT_PAREN: return LONG_VAL(TAG_TOKEN_RIGHT_PAREN);
    default: return NIL_VAL;
  }
}

void push_out(cuke_value value)
{
  *tag_values.out_top = value;
  tag_values.out_top++;
}

static void expression();

static int last_operator()
{
  if (tag_values.operators == tag_values.operators_top)
  {
    return 0;
  }
  return (int)AS_LONG(*(tag_values.operators_top-1));
}

static bool last_operator_is_and_or_xor()
{
  int last_op = last_operator();
  if (last_op == 0) 
  {
    return false;
  }
  return last_op == TAG_TOKEN_AND || last_op == TAG_TOKEN_OR || last_op == TAG_TOKEN_XOR;
}

static bool last_operator_is_not()
{
  int last_op = last_operator();
  if (last_op == 0) 
  {
    return false;
  }
  return last_op == TAG_TOKEN_NOT;
}

static void left_association()
{
  if (last_operator_is_and_or_xor())
  {
    push_out(pop_operator());
  }
}

static void and_or_xor()
{
  left_association();
  push_operator(token_to_cuke_value());
  expression();
}

static void close_grouping()
{
  for(;;)
  {
    cuke_value operator = pop_operator();
    if (IS_INT(operator) && AS_LONG(operator) == TAG_TOKEN_LEFT_PAREN)
    {
      break;
    }
    push_out(operator);
  }

  if (match(TAG_TOKEN_AND) || match(TAG_TOKEN_OR) || match(TAG_TOKEN_XOR))
  {
    and_or_xor();
  }
  else if (match(TAG_TOKEN_RIGHT_PAREN))
  {
    close_grouping();
  }
  else if (match(TAG_TOKEN_EOL))
  {
    return;
  }
  else 
  {
    error_at_current("Expect operator or end of tags after ')': ");
  }
}

static void grouping()
{
  push_operator(token_to_cuke_value());
  expression();
}

static void tag()
{
  push_out(token_to_cuke_value());

  if (last_operator_is_not())
  {
    push_out(pop_operator());
  }

  if (match(TAG_TOKEN_AND) || match(TAG_TOKEN_OR) || match(TAG_TOKEN_XOR))
  {
    and_or_xor();
  }
  else if (match(TAG_TOKEN_RIGHT_PAREN))
  {
    close_grouping();
  }
  else if (match(TAG_TOKEN_EOL))
  {
    return ;
  }
  else 
  {
    error_at_current("Expect 'and' or 'or' after tag.");
  }
}

static void expression()
{
  if (match(TAG_TOKEN_TAG))
  {
    tag();
  }
  else if (match(TAG_TOKEN_LEFT_PAREN))
  {
    grouping();  
  }
  else if (match(TAG_TOKEN_NOT))
  {
    push_operator(token_to_cuke_value());
    expression();
  }
  else if (match(TAG_TOKEN_EOL))
  {
    return ;
  }
  else 
  {
    error_at_current("Expect '@tag', '(' or 'not'.");
  }
}

static void reset_tag_values()
{
  tag_values.operators_top = tag_values.operators;
  tag_values.out_top = tag_values.out;
}

int compile_tag_expression(const char* tags, cuke_value* result)
{
  init_tag_scanner(tags);

  reset_tag_values();

  tag_parser.had_error = false;
  next_token();
  expression();

  if (tag_parser.had_error) 
  {
    return 0;
  }

  while (tag_values.operators_top != tag_values.operators)
  {
    cuke_value operator = pop_operator();
    if (IS_INT(operator) && AS_LONG(operator) != TAG_TOKEN_LEFT_PAREN)
    {
      push_out(operator);
    }
  }
  int size = tag_values.out_top - tag_values.out;
  for (int i = 0 ; i < size ; i++)
  {
    result[i] = tag_values.out[i];
  }
  return size;
}


typedef struct node_t
{
  bool data;
  struct node_t* next;
} node_t;

typedef struct 
{
  unsigned int size;
  node_t* top;  
} rpn_stack_t;


static node_t* new_node(bool data) 
{
    node_t* node = (node_t*) malloc(sizeof(node_t));
    node->data = data;
    node->next = NULL;
    return node;
}

static void init_stack(rpn_stack_t* stack) 
{
    stack->size = 0;
    stack->top = NULL;
}

static void free_stack(rpn_stack_t* stack)
{
  node_t* node = stack->top;
  while (node)
  {
    node_t* next = node->next;
    free(node);
    node = next;
  }
}

static bool is_empty(rpn_stack_t* stack) 
{
    return stack->top == NULL;
}

static void push_to_stack(rpn_stack_t* stack, bool data) 
{
    node_t* node = new_node(data);
    node->next = stack->top;
    stack->top = node;
    stack->size++;
}

static bool pop_from_stack(rpn_stack_t* stack) 
{
  if (is_empty(stack)) {
      return false;
  }

  node_t* temp = stack->top;
  bool data = temp->data;
  stack->top = temp->next;
  stack->size--;
  free(temp);
  return data;
}

static void clear_stack(rpn_stack_t* stack)
{
    while (!is_empty(stack))
    {
        pop_from_stack(stack);
    }
}

static bool is_same(obj_string* s1, obj_string* s2)
{
  if (s1->length != s2->length)
  {
    return false;
  }
  return memcmp(s1->chars, s2->chars, s1->length) == 0;
}

static bool contains(obj_string* value, cuke_value* tags, int tags_count)
{
  for (int i = 0 ; i < tags_count ; i++)
  {
    if (is_same(value, AS_STRING(tags[i])))
    {
      return true;
    }
  }
  return false; 
}

bool evaluate_tags(cuke_value* rpn_stack, int rpn_size, cuke_value* tags, int tags_count)
{
  rpn_stack_t stack; 
  init_stack(&stack);

  for (unsigned int i = 0 ; i < rpn_size ; i++)
  {
    if (IS_STRING(rpn_stack[i])) 
    {
      push_to_stack(&stack, contains(AS_STRING(rpn_stack[i]), tags, tags_count));
    }
    else if (IS_INT(rpn_stack[i]))
    {
      switch (AS_LONG(rpn_stack[i]))
      {
        case TAG_TOKEN_NOT:
        {
          bool value = pop_from_stack(&stack);
          push_to_stack(&stack, !value);
        }
        break; case TAG_TOKEN_OR:
        {
          bool lhs = pop_from_stack(&stack);
          bool rhs = pop_from_stack(&stack);
          push_to_stack(&stack, lhs || rhs);
        }
        break; case TAG_TOKEN_AND:
        {
          bool lhs = pop_from_stack(&stack);
          bool rhs = pop_from_stack(&stack);
          push_to_stack(&stack, lhs && rhs);
        }
        break; case TAG_TOKEN_XOR:
        {
          bool lhs = pop_from_stack(&stack);
          bool rhs = pop_from_stack(&stack);
          push_to_stack(&stack, lhs != rhs);
        }
        break; default : ;// can not happen ... 
      }
    }
  }
  bool result = pop_from_stack(&stack);
  free_stack(&stack);
  return result;
  // return pop_from_stack(&stack);
}

