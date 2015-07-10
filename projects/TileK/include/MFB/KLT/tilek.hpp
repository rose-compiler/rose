
#ifndef __KLT_TILEK_HPP__
#define __KLT_TILEK_HPP__

#include "KLT/Language/none.hpp"
#include "KLT/Runtime/tilek.hpp"
#include "DLX/TileK/language.hpp"

namespace MFB {

typedef ::KLT::Kernel< ::DLX::KLT::Annotation< ::DLX::TileK::language_t>, ::KLT::Language::None, ::KLT::Runtime::TileK> tilek_kernel_t;

template <>
class KLT<tilek_kernel_t> {
  public:
    typedef ::DLX::KLT::Annotation< ::DLX::TileK::language_t> Annotation;
    typedef ::KLT::Language::None Language;
    typedef ::KLT::Runtime::TileK Runtime;

    typedef ::KLT::LoopTrees< ::DLX::KLT::Annotation< ::DLX::TileK::language_t> > LoopTrees;
    typedef ::KLT::LoopTiler<DLX::KLT::Annotation<DLX::TileK::language_t>, Language::None, Runtime::TileK> LoopTiler;

    struct object_desc_t {
      unsigned id;
      tilek_kernel_t * kernel;
      unsigned long file_id;
      std::map<LoopTrees::loop_t *, LoopTiler::loop_tiling_t *> tiling;

      object_desc_t(
        unsigned id_,
        tilek_kernel_t * kernel_,
        unsigned long file_id_
      );
    };

    typedef tilek_kernel_t::kernel_desc_t * build_result_t;
};

} // namespace MFB

#endif /* __KLT_TILEK_HPP__ */

