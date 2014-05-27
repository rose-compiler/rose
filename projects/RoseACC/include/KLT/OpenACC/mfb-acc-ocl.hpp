
#ifndef __MFB_ACC_OCL_HPP__
#define __MFB_ACC_OCL_HPP__

#include "KLT/Core/kernel.hpp"
#include "KLT/Core/loop-trees.hpp"
#include "KLT/OpenACC/runtime-openacc.hpp"
#include "KLT/OpenACC/language-opencl.hpp"

#include "DLX/OpenACC/language.hpp"
#include "DLX/klt-annotations.hpp"

#include <vector>

typedef ::KLT::Kernel< ::DLX::KLT_Annotation< ::DLX::OpenACC::language_t>, ::KLT::Language::OpenCL, ::KLT::Runtime::OpenACC> Kernel_OpenCL_OpenACC;

namespace MFB {

/*!
 * \addtogroup grp_mfb_acc_ocl
 * @{
*/

template <>
class KLT<Kernel_OpenCL_OpenACC> {
  public:
    typedef ::DLX::KLT_Annotation< ::DLX::OpenACC::language_t> Annotation;
    typedef ::KLT::Language::OpenCL Language;
    typedef ::KLT::Runtime::OpenACC Runtime;

    struct object_desc_t {
      unsigned id;
      Kernel_OpenCL_OpenACC * kernel;
      unsigned long file_id;
      std::map<
        ::KLT::LoopTrees< ::DLX::KLT_Annotation< ::DLX::OpenACC::language_t> >::loop_t *,
        Runtime::loop_shape_t *
      > shapes;

      object_desc_t(
        unsigned id_,
        Kernel_OpenCL_OpenACC * kernel_,
        unsigned long file_id_
      );
    };

    typedef Kernel_OpenCL_OpenACC::a_kernel * build_result_t;
};

/** @} */

}

#endif /* __MFB_ACC_OCL_HPP__ */

