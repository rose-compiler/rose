// Interesting bug report (same as C_tests test2010_02.c).
// This is a strictness of EDG whereas GNU allows this (at least for C++).
int main()
{
  return main();
}
