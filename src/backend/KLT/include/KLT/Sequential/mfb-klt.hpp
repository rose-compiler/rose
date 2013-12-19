
#ifndef __KLT_SEQUENTIAL_MFB_KLT_HPP__
#define __KLT_SEQUENTIAL_MFB_KLT_HPP__

#include "KLT/Core/mfb-klt.hpp"

#include "KLT/Sequential/kernel.hpp"

class SgFunctionSymbol;
class SgClassSymbol;

namespace KLT {
namespace Core {
template <typename Kernel> class IterationMap;
}
}

namespace MFB {

/*!
 * \addtogroup grp_klt_mfb
 * @{
*/

template <>
class KLT< ::KLT::Sequential::Kernel> {
  public:
    struct object_desc_t {
      ::KLT::Sequential::Kernel * kernel;
      ::KLT::Sequential::Kernel::loop_mapping_t * loop_mapping;
      ::KLT::Core::IterationMap< ::KLT::Sequential::Kernel> * iteration_map;
      unsigned long file_id;

      object_desc_t(
        ::KLT::Sequential::Kernel * kernel_,
        ::KLT::Sequential::Kernel::loop_mapping_t * loop_mapping_,
        ::KLT::Core::IterationMap< ::KLT::Sequential::Kernel> * iteration_map_,
        unsigned long file_id_
      );
    };

    typedef ::KLT::Sequential::Kernel::a_kernel * build_result_t;

};

template <>
KLT< ::KLT::Sequential::Kernel>::build_result_t Driver<KLT>::build< ::KLT::Sequential::Kernel>(const KLT< ::KLT::Sequential::Kernel>::object_desc_t & desc);

/** @} */

}

#endif /* __KLT_SEQUENTIAL_MFB_KLT_HPP__ */
