// t0089.cc
// replicate problem with reported decl/defn mismatch with static funcs

class A {
  static int foo();
};

int A::foo()
{}
