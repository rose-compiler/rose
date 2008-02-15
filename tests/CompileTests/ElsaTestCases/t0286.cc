// t0286.cc
// non-member operator |=

enum E { evalue };

E &operator|= (E &e1, E e2)
{
  e1 = (E)(e1 | e2);
  return e1;
}
 
