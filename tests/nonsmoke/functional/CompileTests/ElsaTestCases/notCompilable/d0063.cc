// this one puts g++ 3.4.0 into an infinite loop also but again only
// if you have the instantiation

template<class T, class U>
void f(T x, U y, T z) {
  f(&y,                         // T bound to a pointer to U
    &x,                         // U bound to a pointer to T
    &x                          // infinite loop
    );
}

int main() {
  f(3, 4, 5);
}
