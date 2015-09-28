
#ifndef __MFB_KLT_KLT_HPP__
#define __MFB_KLT_KLT_HPP__

// Needs Annotation and Runtime to be defined

#include "KLT/Core/loop-trees.hpp"
typedef ::KLT::LoopTrees<Annotation> LoopTrees; // KLT's representation of loop-nests

#include "KLT/Core/kernel.hpp"
typedef ::KLT::Kernel<Annotation, Runtime> Kernel; // KLT's kernel object

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

      object_desc_t(size_t id_, Kernel * kernel_, size_t file_id_) :
        id(id_), kernel(kernel_), file_id(file_id_), tiling()
      {}
    };

    typedef Kernel::kernel_desc_t * build_result_t;
};

} // namespace MFB

#endif /* __MFB_KLT_KLT_HPP__ */

