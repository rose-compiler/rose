template<class T>  void  f(T);               // #1
template<class T>  void  f(T*, int=1);       // #2

#if 0
void m(int* ip) {
  int* ip;
  f(ip);     // calls #2 (T* is more specialized than T)
}
#else
void m() {
  int* ip;
  f(ip);     // calls #2 (T* is more specialized than T)
}
#endif
