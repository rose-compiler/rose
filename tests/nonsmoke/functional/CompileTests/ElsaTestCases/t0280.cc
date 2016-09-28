// t0280.cc
// overloading + template inst.

void f(int) {}                        // line 4
template <class T2> void f(T2) {}     // ;ine 5

int main()
{
  f(1);                // non-template
  f('c');              // template
  f<>(1);              // template
}
