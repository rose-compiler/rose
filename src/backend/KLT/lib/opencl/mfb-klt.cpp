
#include "KLT/OpenCL/mfb-klt.hpp"

namespace MultiFileBuilder {

KLT< ::KLT::OpenCL::Kernel>::object_desc_t::object_desc_t() {}

template <>
KLT< ::KLT::OpenCL::Kernel>::build_result_t Driver<KLT>::build< ::KLT::OpenCL::Kernel>(const KLT< ::KLT::OpenCL::Kernel>::object_desc_t & desc) {
  KLT< ::KLT::OpenCL::Kernel>::build_result_t result;

  // TODO

  return result;
}

}
