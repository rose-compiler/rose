// 10 matches

int foo() { return 1; }
float foo2() { return 2.0f; }
typedef int (*pt2foo)(); /* (+1) */
typedef float (*pt2foo2)(); /* (+1) */

// 2 x (+4): each template instantiation generates (+4)
// for this templated function.
template <typename T>
int get_num_from_function(T foo) /* 2 x (+2) */
  {
    T b; /* 2 x (+1) */
    return foo(); /* 2 x (+1) */
  }

//template <typename T>
//int get_num_from_function2(T foo)
//  {
//  }


// TODO: none of these are detected!
int main () {
  // SgFunctionCallExpo > SgExprListExp > SgFunctionRefExp > SgCastExp > SgFunctionRefExp
  get_num_from_function<pt2foo>( foo ); /* TODO: template */
//  get_num_from_function2<pt2foo>( &foo ); /* TODO: template */
  get_num_from_function<pt2foo2>( foo2 ); /* TODO: template */
}
