
#include "KLT/Core/mfb-klt.hpp"

namespace MFB {

KLT_Driver::KLT_Driver(SgProject * project_, bool guard_kernel_decl) :
  Driver<Sage>(project_),
  Driver<KLT>(guard_kernel_decl)
{}

KLT_Driver::~KLT_Driver() {}

Driver<KLT>::Driver(bool guard_kernel_decl_) :
  guard_kernel_decl(guard_kernel_decl_)
{}

Driver<KLT>::~Driver() {}

}

