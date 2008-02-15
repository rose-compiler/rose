// From in/big/nsAtomTable.i
template <class T> struct f {};
template <class T> class R {
  f<T> m;
};
int d(R<int>&);
