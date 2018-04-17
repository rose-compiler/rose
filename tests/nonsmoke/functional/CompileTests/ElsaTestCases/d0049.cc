// from nonport.i
// error: there is no variable called `temp-name-1'
struct A {};
A f (A);
A g() {
  A o;
  return f(o);
}
