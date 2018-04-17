// t0279.cc
// overloading template vs. non-template

void f(int) {}                        // line 4
template <class T2> void f(T2) {}     // ;ine 5

int main()
{
  __checkCalleeDefnLine(f(1), 4);                // non-template
  __checkCalleeDefnLine(f('c'), 5);              // template
}
