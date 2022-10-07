
#pragma once

#include <cstdio>
#include <cstring>

#if __cplusplus < 201103L
#define OVERRIDE
#define NULLPTR NULL
#else
#define OVERRIDE override
#define NULLPTR nullptr
#endif


// forward declaration, implemented in the test methods
extern const char* description;
extern const char* expectedout;

void run();

bool TEST_IS_ACTIVE = false;

void program_description()
{
  printf("%s\n", description);
  printf("  -h --help     prints this help message.\n");
  printf("  -e --expected prints the expected output.\n");
}

void expected_output()
{
  printf("%s\n", expectedout);
}

void process_args(int argv, char** argc)
{
  static const char* help          = "-h";
  static const char* help_long     = "--help";
  static const char* expected      = "-e";
  static const char* expected_long = "--expected";

  for (int i = 1; i < argv; ++i)
  {
    const char* arg = argc[i];

    if ((strcmp(help, arg) == 0) || (strcmp(help_long, arg) == 0))
      program_description();
    else if ((strcmp(expected, arg) == 0) || (strcmp(expected_long, arg) == 0))
      expected_output();
    else
      printf("unknown argument: %s\n", arg);
  }
}

int main(int argv, char** argc)
{
  if (argv > 1)
  {
    process_args(argv, argc);
    return 0;
  }

  TEST_IS_ACTIVE = true;
  printf("{");

  try
  {
    run();
  }
  catch (...)
  {
    printf("!");
  }

  printf("}\n");
  TEST_IS_ACTIVE = false;
  return 0;
}
