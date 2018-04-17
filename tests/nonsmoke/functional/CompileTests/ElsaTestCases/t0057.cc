// t0057.cc
// problem with template-id as a qualifier

// this testcase demonstrates the need to fully implement
// template specialization matching in order to properly
// disambiguate C++ syntax

template <class T>
class Foo {
public:
  // the absence of this declaration in the primary template
  // is what makes the access of 'x' appear to fail, below
  //  static int x();

  static int y(int q);
  typedef int z;
};

// specialization
template <>
class Foo<int> {
public:
  static int x();
  typedef int y;
  static int z(int);
};

int j,k;

int main()
{
  int i;

  // for this one, I pretend 'x' is a variable of type ST_ERROR
  // to suppress the error and make it appear unambiguous
  i = Foo<int>::x();
  
  // correct interpretation: declaration
  // my interpretation: function call
  Foo<int>::y(j);

  // correct interpretation: function call
  // my interpretation: declaration
  Foo<int>::z(k);
  
  // UPDATE 9/05/03: I now have a very restricted form of specialization
  // implemented, and some of the above may now actually work right.
}

