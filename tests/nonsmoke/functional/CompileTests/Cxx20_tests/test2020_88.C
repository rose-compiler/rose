template<class  T>  void  g(T);               // #1
template<class  T>  void  g(T*, ...);         // #2
void m(int* ip) {
   g(ip);     // calls #2 (T* is more specialized than T)
}


