
#ifndef __MFB_KLT_KLT_HPP__
#define __MFB_KLT_KLT_HPP__

#include "DLX/TileK/language.hpp"
typedef ::DLX::TileK::language_t Dlang; // Directives' Language

#include "KLT/Language/c-family.hpp"
typedef ::KLT::Language::C Hlang; // Host Code Language
typedef ::KLT::Language::C Klang; // Kernel Language

#include "DLX/KLT/annotations.hpp"
typedef ::DLX::KLT::Annotation<Dlang> Annotation; // Alias for clauses parsed by DLX

#include "MDCG/KLT/runtime.hpp"
typedef ::MDCG::KLT::Runtime<Hlang, Klang> Runtime; // Runtime Description

#include "MDCG/KLT/model.hpp"
typedef ::MDCG::KLT::KernelDesc<Hlang, Klang> KernelDesc; // Model for Static Initializer

#include "KLT/Core/loop-trees.hpp"
typedef ::KLT::LoopTrees<Annotation> LoopTrees; // KLT's representation of loop-nests

#include "KLT/Core/kernel.hpp"
typedef ::KLT::Kernel<Annotation, Runtime> Kernel; // KLT's kernel object

#include "KLT/Core/data.hpp"
typedef ::KLT::Data<Annotation> Data; // KLT's data object

#include "KLT/Core/loop-tiler.hpp"
typedef ::KLT::LoopTiler<Annotation, Runtime> LoopTiler; // (TO BE IMPROVED)

#include "KLT/Core/mfb-klt.hpp"

namespace MFB {

template <>
class KLT<Kernel> {
  public:
    struct object_desc_t {
      size_t id;
      Kernel * kernel;
      size_t file_id;
      std::map<LoopTrees::loop_t *, LoopTiler::loop_tiling_t *> tiling;

      object_desc_t(size_t id_, Kernel * kernel_, size_t file_id_);
    };

    typedef Kernel::kernel_desc_t * build_result_t;
};

} // namespace MFB

#endif /* __MFB_KLT_KLT_HPP__ */

