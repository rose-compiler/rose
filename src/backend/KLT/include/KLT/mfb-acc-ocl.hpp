
#ifndef __MFB_ACC_OCL_HPP__
#define __MFB_ACC_OCL_HPP__

#include "KLT/kernel.hpp"
#include "KLT/runtime-openacc.hpp"
#include "KLT/language-opencl.hpp"

#include "DLX/OpenACC/language.hpp"
#include "DLX/klt-annotations.hpp"

#include <vector>

typedef ::KLT::Kernel< ::DLX::KLT_Annotation< ::DLX::OpenACC::language_t>, ::KLT::Language::OpenCL, ::KLT::Runtime::OpenACC> Kernel_OpenCL_OpenACC;
typedef ::KLT::IterationMap< ::DLX::KLT_Annotation< ::DLX::OpenACC::language_t>, ::KLT::Language::OpenCL, ::KLT::Runtime::OpenACC> ItMap_OpenCL_OpenACC;

namespace MFB {

/*!
 * \addtogroup grp_mfb_acc_ocl
 * @{
*/

template <>
class KLT<Kernel_OpenCL_OpenACC> {
  public:
    struct object_desc_t {
      Kernel_OpenCL_OpenACC * kernel;
      Kernel_OpenCL_OpenACC::loop_mapping_t * loop_mapping;
      ItMap_OpenCL_OpenACC * iteration_map;
      unsigned long file_id;

      object_desc_t(
        Kernel_OpenCL_OpenACC * kernel_,
        Kernel_OpenCL_OpenACC::loop_mapping_t * loop_mapping_,
        ItMap_OpenCL_OpenACC * iteration_map_,
        unsigned long file_id_
      );
    };

    typedef Kernel_OpenCL_OpenACC::a_kernel * build_result_t;
};

/** @} */

}

#endif /* __MFB_ACC_OCL_HPP__ */

