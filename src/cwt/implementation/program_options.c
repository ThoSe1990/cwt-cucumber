#include <stdio.h>
#include <string.h>

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

void program_options(int argc, const char* argv[])
{
  for (int i = 1; i < argc; i++) 
  {
    if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) 
    {

    }
    else if (strcmp(argv[i], "--silent") == 0 || strcmp(argv[i], "-s") == 0) 
    {
        set_silent();
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