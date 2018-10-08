// Note: this requires that the path be known to the systemc.h include directory.
// DQ (4/18/18): On my machine it is at: /home/quinlan1/ROSE/SystemC/include
// This systemc.h header file is not distributed with ROSE.

#include "systemc.h"

#include <iostream>


class Foo: public sc_module {

public:

  SC_HAS_PROCESS(Foo);

  Foo(sc_module_name mn):
    sc_module(mn)
  {
    SC_THREAD(foo);
  }

  void foo()
  {
    std::cout << "Hello World!" << std::endl;
  }
};

int sc_main(int argc, char **argv)
{
  Foo f("foo");

  sc_start();

  return 0;
}

