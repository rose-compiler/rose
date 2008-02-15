// minimized from nsAtomTable.i
typedef int A;

template <class T>
struct B {};

template <class T2>
struct B<T2*> {
  int f(T2*) {
    return A(3);
  }
};
