template<class T>  void  f(T);               // #1
template<class T>  void  f(T*, int=1);       // #2
void m(int* ip) {
  int* ip;
  f(ip);     // calls #2 (T* is more specialized than T)
}

