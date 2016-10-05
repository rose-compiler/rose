
#if 0

Dan,

Here is the requirements from David:
"the most important feature we have been using from C++14 is new "generic lambda". Basically, that means we can do:
    [=](auto my_param) { // something that uses my_param }
and have a lambda that can take an argument of any type. It's the equivalent of a templated lambda function.
"

// ------------------------------------
// Slightly modified test case from David, generic-lambda.cxx
// Liao 8/12/2016
#include <typeinfo>
#include <iostream>

int main(int argc, char* argv[]) {
  int a = 0;
  std::string test = "testing";

  auto my_lambda = [=] (auto val) {
    std::cout << val << " is a " << typeid(decltype(val)).name() << std::endl;
  };

  my_lambda(1);
  my_lambda(a);
  my_lambda(test);
}

//-----------------------compile---------------

identityTranslator -rose:Cxx14 -c generic-lambda.cxx
"/home/liao6/workspace/ASC-LEARN-project/generic-lambda.cxx", line 10: error:
          "auto" is not allowed here
    auto my_lambda = [=] (auto val) {
                          ^

"/home/liao6/workspace/ASC-LEARN-project/generic-lambda.cxx", line 7: warning:
          variable "a" was set but never used
    int a = 0;
        ^

Errors in Processing Input File: throwing an instance of "frontend_failed" exception due to syntax errors detected in the input code

//GCC 4.9 works

// /nfs/casc/overture/ROSE/opt/rhel7/x86_64/gcc/4.9.3/mpc/1.0/mpfr/3.1.2/gmp/5.1.2/bin/g++ -c -std=c++14 generic-lambda.cxx

#endif


#include <typeinfo>
#include <iostream>

int main(int argc, char* argv[]) {
  int a = 0;
  std::string test = "testing";

  auto my_lambda = [=] (auto val) {
    std::cout << val << " is a " << typeid(decltype(val)).name() << std::endl;
  };

  my_lambda(1);
  my_lambda(a);
  my_lambda(test);
}
