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

int get_scenario_lines(int argc, const char* args[], int out[MAX_SCENARIOS])
{
  int i = 0;
  while (i < argc)
  {
    if ((strcmp(args[i], "--line") == 0 || strcmp(args[i], "-l") == 0) 
      && i+1 < argc)
    {
      int out_idx = i;
      i++;
      out[out_idx] = atoi(args[i]);
    }
    else 
    {
      break;
    }
    i++;
  }
  return i;
}

void program_options(int argc, const char* argv[])
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
    else if (*argv[i] == '-')
    {
      fprintf(stderr, "Unknown program option: '%s'.", argv[i]);
    }
  }
}