// t0456.cc
// make sure Elsa can handle #line directives that refer to
// nonexistent files

int foo();

# 10 "nonexist.cc"

int bar();

// even with an error there, causing an error message to point at it
//ERROR(1): int array[does_not_exist];

# 15 "t0456.cc"

int main()
{
  return foo() + bar();
}

