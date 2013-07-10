
#include "KLT/Core/mfb-klt.hpp"

namespace MultiFileBuilder {

KLT_Driver::KLT_Driver(SgProject * project_) :
  Driver<Sage>(project_),
  Driver<KLT>()
{}

}
