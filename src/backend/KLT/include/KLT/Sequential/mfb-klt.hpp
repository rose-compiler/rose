
#ifndef __KLT_SEQUENTIAL_MFB_KLT_HPP__
#define __KLT_SEQUENTIAL_MFB_KLT_HPP__

#include "KLT/Core/mfb-klt.hpp"

namespace KLT {
namespace Sequential {
class Kernel;
}
}

namespace MultiFileBuilder {

template <>
class KLT< ::KLT::Sequential::Kernel> {
  public:
    struct object_desc_t {
      ::KLT::Sequential::Kernel * kernel;
      unsigned long file_id;

      object_desc_t(::KLT::Sequential::Kernel * kernel_, unsigned long file_id_);
    };

    struct build_result_t {
      SgFunctionSymbol * kernel;
      SgClassSymbol *    arguments_packer;
    };

};

template <>
KLT< ::KLT::Sequential::Kernel>::build_result_t Driver<KLT>::build< ::KLT::Sequential::Kernel>(const KLT< ::KLT::Sequential::Kernel>::object_desc_t & desc);

}

#endif /* __KLT_SEQUENTIAL_MFB_KLT_HPP__ */
