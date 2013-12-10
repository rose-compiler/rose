
#ifndef __HANDLES_TRANSFORMATION_HPP__
#define __HANDLES_TRANSFORMATION_HPP__

#include "hooks.hpp"

namespace Handles {

struct SymbolReplace {};
struct PragmaReplace {};
 
struct TransformationHandler :
  public handle_hook_t<TransformationHandler, SymbolReplace>,
  public handle_hook_t<TransformationHandler, PragmaReplace>
{
  typedef unsigned handle_id_t;
 
  bool process();
};

}

#endif /* __HANDLES_TRANSFORMATION_HPP__ */

