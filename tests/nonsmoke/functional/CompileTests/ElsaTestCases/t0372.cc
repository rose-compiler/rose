// t0372.cc
// testing 14.2 para 2

template <class T>
void templ1(int x, T y);

void templ1(int x, int y, int z);


void templ2(int x, int y, int z);

template <class T>
void templ2(int x, T y);


void nontempl(int x, int y);


void bar()
{
  templ1(3,4);
  templ1<int>(3,4);

  nontempl(3,4);
  //ERROR(1): nontempl<int>(3,4);

  templ2(3,4);
  templ2<int>(3,4);
}
