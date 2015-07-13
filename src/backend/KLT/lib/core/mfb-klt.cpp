
#include "sage3basic.h"
#include "KLT/Core/mfb-klt.hpp"

namespace MFB {

Driver<KLT>::Driver(SgProject * project_, bool guard_kernel_decl_) :
  Driver<Sage>(project_),
  guard_kernel_decl(guard_kernel_decl_)
{}

Driver<KLT>::~Driver() {}

}

