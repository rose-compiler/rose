
#include "KLT/mfb-klt.hpp"

namespace MFB {

KLT_Driver::KLT_Driver(SgProject * project_) :
  Driver<Sage>(project_),
  Driver<KLT>()
{}

}

