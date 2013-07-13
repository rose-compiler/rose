
#ifndef __KLT_OPENCL_MFB_KLT_HPP__
#define __KLT_OPENCL_MFB_KLT_HPP__

#include <vector>
#include <string>

#include "KLT/Core/mfb-klt.hpp"

#include "KLT/OpenCL/kernel.hpp"

namespace KLT {
namespace Core {
template <typename Kernel> class IterationMap;
}
}

namespace MultiFileBuilder {

template <>
class KLT< ::KLT::OpenCL::Kernel> {
  public:
    struct object_desc_t {
      ::KLT::OpenCL::Kernel * kernel;
      ::KLT::OpenCL::Kernel::loop_distribution_t * loop_distribution;
      ::KLT::Core::IterationMap< ::KLT::OpenCL::Kernel> * iteration_map;
      unsigned long file_id;

      object_desc_t(
        ::KLT::OpenCL::Kernel * kernel_,
        ::KLT::OpenCL::Kernel::loop_distribution_t * loop_distribution_,
        ::KLT::Core::IterationMap< ::KLT::OpenCL::Kernel> * iteration_map_,
        unsigned long file_id_
      );
    };

    typedef ::KLT::OpenCL::Kernel::an_opencl_kernel * build_result_t;

};

template <>
KLT< ::KLT::OpenCL::Kernel>::build_result_t Driver<KLT>::build< ::KLT::OpenCL::Kernel>(const KLT< ::KLT::OpenCL::Kernel>::object_desc_t & desc);

}

#endif /* __KLT_OPENCL_MFB_KLT_HPP__ */
