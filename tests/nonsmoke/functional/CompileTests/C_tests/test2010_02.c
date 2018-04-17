// Interesting bug report (same as Cxx_tests test2010_16.C).
// This is a strictness of EDG whereas GNU allows this (at least for C++).
int main()
{
  return main();
}
