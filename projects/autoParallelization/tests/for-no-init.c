// converted from /usr/include/c++/4.4.7/bits/stl_algobase.h(701,7)
void fill (int* first, int* last, const int value)
{
  const int tmp= value;
  for (; first!= last; ++first)
    *first = tmp;
}
