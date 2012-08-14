

// This is a simplified example from test2005_137.C

// Template function
template <typename T>
void foo()
   {
   }

// specialization for when "int" is used
template <>
void foo<int>()
   {
   }

// general template instatiation directive
// This will trigger an error if the specialization has not been put out!
template void foo<long>();

#if 0
void foobar()
{
  foo<int>();
}
#endif
