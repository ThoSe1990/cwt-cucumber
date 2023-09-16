#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "chunk.h"
#include "object.h"
#include "compiler.h"

#include "debug.h"


static char* read_file(const char* path) 
{
  FILE* file = fopen(path, "rb");
  if (file == NULL) 
  {
    fprintf(stderr, "Could not open file \"%s\".\n", path);
    exit(74);
  }

  fseek(file, 0L, SEEK_END);
  size_t file_size = ftell(file);
  rewind(file);

  char* buffer = (char*)malloc(file_size+1);
  if (buffer == NULL)
  {
    fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
    exit(74);
  }

  size_t bytes_read = fread(buffer, sizeof(char), file_size, file);
  if (bytes_read < file_size)
  {
    fprintf(stderr, "Could not read file \"%s\".\n", path);
    exit(74);
  }
  buffer[bytes_read] = '\0';
  fclose(file);
  return buffer;
}
#include "memory.h"
static void run_file(const char* path) 
{
  char* source = read_file(path);

  obj_function* func = compile(source);
  if (func == NULL) 
  {
    fprintf(stderr, "************COMPILE ERROR!*****************");
  }
  else 
  {
      disassemble_chunk(&func->chunk, "chunk");
  }

  free(source);
  // TODO Not here
  free_object(func);
  
}

int main(int argc, const char* argv[])
{
  // TODO run more files
  if (argc == 2)
  {
    run_file(argv[1]);
  }
  else 
  {
    fprintf(stderr, "Usage: clox [path]\n");
    exit(64);
  }

  return 0;
}