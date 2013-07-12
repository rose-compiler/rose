
#ifndef __KLT_OPENCL_MFB_KLT_HPP__
#define __KLT_OPENCL_MFB_KLT_HPP__

#include <vector>
#include <string>

#include "KLT/Core/mfb-klt.hpp"

#include "KLT/OpenCL/kernel.hpp"

namespace KLT {
namespace OpenCL {
class WorkSizeShape;
}
}

namespace MultiFileBuilder {

template <>
class KLT< ::KLT::OpenCL::Kernel> {
  public:
    struct object_desc_t {
      ::KLT::OpenCL::Kernel * kernel;
      ::KLT::OpenCL::WorkSizeShape * work_size_shape;
      unsigned long file_id;

      object_desc_t(::KLT::OpenCL::Kernel * kernel_, ::KLT::OpenCL::WorkSizeShape * work_size_shape_, unsigned long file_id_);
    };

    typedef ::KLT::OpenCL::Kernel::a_kernel * build_result_t;

};

template <>
KLT< ::KLT::OpenCL::Kernel>::build_result_t Driver<KLT>::build< ::KLT::OpenCL::Kernel>(const KLT< ::KLT::OpenCL::Kernel>::object_desc_t & desc);

}

#endif /* __KLT_OPENCL_MFB_KLT_HPP__ */
