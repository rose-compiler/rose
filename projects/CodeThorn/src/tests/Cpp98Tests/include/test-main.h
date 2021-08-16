
#pragma once

#include <cstdio>
// #include <string>

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
#if 0
  static const std::string help          = "-h";
  static const std::string help_long     = "--help";
  static const std::string expected      = "-e";
  static const std::string expected_long = "--expected";

  for (int i = 1; i < argv; ++i)
  {
    const char* arg = argc[i];

    if (help == arg || help_long == arg)
      program_description();
    else if (expected == arg || expected_long == arg)
      expected_output();
    else
      printf("unknown argument: %s\n", arg);
  }
#endif
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
