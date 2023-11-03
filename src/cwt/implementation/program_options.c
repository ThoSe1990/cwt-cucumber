#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "program_options.h"
#include "compiler.h"

static void tags(int index, int argc, const char* argv[])
{
    if (index+1 < argc)
    {
        set_tag_option(argv[index+1]);
    }
    else 
    {
        fprintf(stderr, "Expect value after '%s'.", argv[index]);
    }
}

void option_lines(int argc, const char* current_argv[], int current_idx, int out[MAX_LINES], int* count)
{
  int i = current_idx + 1;
  int out_idx = 0;
  while (i < argc)
  {
    if ((strcmp(current_argv[i], "--line") == 0 || strcmp(current_argv[i], "-l") == 0) 
      && i+1 < argc)
    {
      i++;
      out[out_idx++] = atoi(current_argv[i]);
      *count = i;
    }
    else 
    {
      break;
    }
    i++;
  }
}

void global_options(int argc, const char* argv[])
{
  for (int i = 1; i < argc; i++) 
  {
    if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) 
    {
      // TODO
    }
    else if (strcmp(argv[i], "--quiet") == 0 || strcmp(argv[i], "-q") == 0) 
    {
      set_quiet();
    }
    else if (strcmp(argv[i], "--tags") == 0 || strcmp(argv[i], "-t") == 0) 
    {
      tags(i, argc, argv);
    } 
    else if (strcmp(argv[i], "--lines") == 0 || strcmp(argv[i], "-l") == 0) 
    {

    }
    else if (*argv[i] == '-')
    {
      fprintf(stderr, "Unknown program option: '%s'.", argv[i]);
    }
  }
}