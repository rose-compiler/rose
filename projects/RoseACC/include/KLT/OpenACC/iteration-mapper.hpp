
#ifndef __OPENACC_KLT_ITMAP_HPP__
#define __OPENACC_KLT_ITMAP_HPP__

#include "openacc_spec.hpp"

namespace KLT {

class SingleVersionItMapper : public IterationMapper<Annotation, Language::OpenCL, Runtime::OpenACC> {
  private:
    long tile_0_0;
    long tile_0_1;
    long tile_0_2;
    long tile_1_0;
    long tile_1_1;
    long tile_1_2;

  public:
    SingleVersionItMapper(long tile_0_0_, long tile_0_1_, long tile_0_2_, long tile_1_0_, long tile_1_1_, long tile_1_2_);

  private:
    void computeValidShapes(
      LoopTrees<Annotation>::loop_t * loop,
      std::vector<Runtime::OpenACC::loop_shape_t *> & shapes
    ) const;
};

namespace OpenACC {

class IterationMapper : public ::KLT::IterationMapper<Annotation, Language::OpenCL, Runtime::OpenACC> {
  private:
    std::vector<unsigned> tiling_sizes;

  public:
    IterationMapper();
    IterationMapper(const std::vector<unsigned> & tiling_sizes_);

  private:
    void computeValidShapes(
      LoopTrees<Annotation>::loop_t * loop,
      std::vector<Runtime::OpenACC::loop_shape_t *> & shapes
    ) const;
};

}

}

#endif /* __OPENACC_KLT_ITMAP_HPP__ */

