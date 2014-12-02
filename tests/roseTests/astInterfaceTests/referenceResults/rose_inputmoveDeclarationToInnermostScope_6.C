// For loop with a single statement as body, no basic block

void foo(int value,int *first,int *last)
{
  for (; first != last; ++first) {
    const int tmp = value;
     *first = tmp;
  }
//test if false body is not created.
  if (( *first)) {
    ;
  }
}
