class A {
 public:
  int m;
};

int gi = 42;
int* gip = &gi;

// Template declaration
template <typename T>
T tempFunc(T t);

// Template definition
//  (implicit specializations use this definition)
//  (can be merged with template declaration)
//  (not necessary if no implicit specialization)
template <typename T>
T tempFunc(T t) {
  return t;
}

// Declaration of explicit specialization with A*
template<>
A* tempFunc(A* ap);

// Definition of explicit specialization with A*
template<>
A* tempFunc(A* ap) {
  ap->m = 1;
  return ap;
}

// Definition of explicit specialization with bool
template <>
bool tempFunc(bool b) {
  return !b;
}

// Definition of explicit specialization with int*
template <>
int* tempFunc(int*) {
  return gip;
}

// Definition of explicit instantiation of implicit specialization with bool*
template
bool* tempFunc<bool*>(bool* bp);

// Definition of explicit instantiation of explicit specialization with A*
template
A* tempFunc<A*>(A* ap);

// Content of other source file (to show difference between DEFINITION of explicit instantiation and
//  DECLARATION of explicit instantiation):
/*
  // Template definition
  template <typename T>
  T tempFunc(T t) {
   return t;
  }

  // Definition of explicit specialization with double**
  template<>
  double** tempFunc(double** dpp) {
    return dpp;
  }

  // Definition of explicit instantiation of implicit specialization with double*
  template
  double* tempFunc<double*>(double* dp);

  // Definition of explicit instantiation of explicit specialization with double**
  template
  double** tempFunc<double**>(double** dpp);
*/

// Declaration of explicit instantiation (prevents implicit instantiations. Definition of explicit instantiation
//  (of implicit specialization with double*) in other file will be used).
extern template
double* tempFunc<double*>(double* dp);

// Declaration of explicit instantiation (prevents implicit instantiations. Definition of explicit instantiation
//  (of explicit specialization with double**) in other file will be used).
extern template
double** tempFunc<double**>(double** dpp);

int main() {
  int i;
  i = tempFunc(i); // Implicit Instantiation of implicit specialization with int
  bool b_var;
  b_var = tempFunc<bool>(true); // Implicit Instantiation of explicit specialization with bool
  bool b_var2 = false;
  bool* bp_var;
  bp_var = tempFunc<bool*>(&b_var2); // Uses explicit instantiation (of implicit specialization) with bool*
  A a;
  A* ap_var;
  ap_var = tempFunc<A*>(&a); // Uses explicit instantiation (of explicit specialization) with A*
  double d_var = 1.1;
  double* dp_var = &d_var;
  //double** dpp_var  = tempFunc<double**>(&dp_var); // Uses explicit instantiation with double** (in other source file (see comment above))
}
