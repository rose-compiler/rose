struct A {};
template<class T> struct B {
  template<class R> int operator*(R&);                     // #1
};
template<class T, class R> int operator*(T&, R&);          // #2
int main() {
  A a;
  B<A> b;
  b * a; // template argument deduction for int B<A>::operator*(R&) gives R=A 
         //                             for int operator*(T&, R&), T=B<A>, R=A
// For the purpose of partial ordering, the member template B<A>::operator*
// is transformed into template<class R> int operator*(B<A>&, R&);
// partial ordering between 
//     int operator*(   T&, R&)  T=B<A>, R=A
// and int operator*(B<A>&, R&)  R=A 
// selects int operator*(B<A>&, A&) as more specialized
}

