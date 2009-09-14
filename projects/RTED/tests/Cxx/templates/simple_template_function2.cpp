
template< class T > 
void foo( T* t ) {
    // error, double new, memory leak
    t = new int;
}


int main() {
  int x =0;
  foo( &x );
  return 0;
}
