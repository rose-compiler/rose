
#include "test2014_19.h"

// GNU attribute __attribute__((noreturn)) is not unparsed in the parameter list.
// Even more dramatice, it appears the the GNU attributes are a part of the type 
// system for function resolution (so it appears from gcc's error message).
void i_set_fatal_handler(failure_callback_t *callback ATTR_NORETURN)
   {
  // fatal_handler = callback;
   }

