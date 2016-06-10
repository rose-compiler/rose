#ifndef INPUT_MEMBER_FUNCTION_H_INCLUDED_
#define INPUT_MEMBER_FUNCTION_H_INCLUDED_

class Foo {

  Foo() {
    bar();
  }

  // NO defining declaration is available if a) + b) + c) are positive
  // at the same time:
  //
  //  a) The definition of bar is in a different source file
  //  b) The function bar is called in the ctor
  //  c) The declaration of bar is after the definition of the ctor Foo
  void bar();

  // Observation:
  //
  // * If this declaration is before the defintion of the ctor of Foo,
  //   the defining declaration is available
  // * If this function isn't called in the ctor, a defining declaration
  //   is available
  // * If the declaration is defining at the same time the definition
  //   of bar is available
};



#endif /* INPUT_MEMBER_FUNCTION_H_INCLUDED_ */
