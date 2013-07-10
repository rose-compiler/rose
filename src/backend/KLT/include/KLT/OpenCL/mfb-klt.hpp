
#ifndef __KLT_OPENCL_MFB_KLT_HPP__
#define __KLT_OPENCL_MFB_KLT_HPP__

#include "KLT/Core/mfb-klt.hpp"

//#include "KLT/OpenCL/kernel.hpp"

namespace KLT {
namespace OpenCL {
class Kernel;
}
}

namespace MultiFileBuilder {

template <>
class KLT< ::KLT::OpenCL::Kernel> {
  public:
    struct object_desc_t {
      ::KLT::OpenCL::Kernel * kernel;

      object_desc_t();
    };

    struct build_result_t {
      // TODO
    };

};

template <>
KLT< ::KLT::OpenCL::Kernel>::build_result_t Driver<KLT>::build< ::KLT::OpenCL::Kernel>(const KLT< ::KLT::OpenCL::Kernel>::object_desc_t & desc);

}

#endif /* __KLT_OPENCL_MFB_KLT_HPP__ */
