#ifndef __ABSTRACT_EXCEPTION__
#define __ABSTRACT_EXCEPTION__


// Abstract Base class for all (my) exceptions
// define the generic behavior for all exceptions

class AbstractException 
{

public:
  // instances of this method should send output to cerr
  virtual void debug_print() const { };

};

#endif
