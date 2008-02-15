// t0417.cc
// order of template parameters

template <class S, class T>
struct A {
  int f0(S *s, T *t);
  int f1(S *s, T *t);
  int f2(S *s, T *t);
  int f3(S *s, T *t);
  int f4(S *s, T *t);
  int f5(S *s, T *t);
  int f6(S *s, T *t);
  int f7(S *s, T *t);
};

// normal
template <class S, class T>
int A<S,T>::f0(S *s, T *t)
{
  return 0;
}

// three places S,T could be swapped:
//   - template declaration parameter list    001 = 1
//   - template-id before the ::              010 = 2
//   - function parameter list                100 = 4

//ERROR(1): template <class T, class S /*swapped*/>
//ERROR(1): int A<S,T>::f1(S *s, T *t)
//ERROR(1): {
//ERROR(1):   return 0;
//ERROR(1): }

//ERROR(2): template <class S, class T>
//ERROR(2): int A<T,S /*swapped*/ >::f2(S *s, T *t)
//ERROR(2): {
//ERROR(2):   return 0;
//ERROR(2): }

//ERROR(3): template <class T, class S /*swapped*/>
//ERROR(3): int A<T,S /*swapped*/>::f3(S *s, T *t)
//ERROR(3): {
//ERROR(3):   return 0;
//ERROR(3): }

//ERROR(4): template <class S, class T>
//ERROR(4): int A<S,T>::f4(T *t, S *s /*swapped*/)
//ERROR(4): {
//ERROR(4):   return 0;
//ERROR(4): }

//ERROR(5): template <class T, class S /*swapped*/>
//ERROR(5): int A<S,T>::f5(T *t, S *s /*swapped*/)
//ERROR(5): {
//ERROR(5):   return 0;
//ERROR(5): }

//ERROR(6): template <class S, class T>
//ERROR(6): int A<T,S /*swapped*/ >::f6(T *t, S *s /*swapped*/)
//ERROR(6): {
//ERROR(6):   return 0;
//ERROR(6): }

// when all are swapped, is ok again
template <class T, class S /*swapped*/>
int A<T,S /*swapped*/>::f7(T *t, S *s /*swapped*/)
{
  return 0;
}



void f()
{
  A<int,float> a;
}
