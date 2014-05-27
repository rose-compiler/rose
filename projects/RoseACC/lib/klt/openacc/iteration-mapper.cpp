
#include "KLT/OpenACC/iteration-mapper.hpp"

namespace KLT {

unsigned calls = 0;

SingleVersionItMapper::SingleVersionItMapper(long tile_0_0_, long tile_0_1_, long tile_0_2_, long tile_1_0_, long tile_1_1_, long tile_1_2_) :
  IterationMapper<Annotation, Language::OpenCL, Runtime::OpenACC>(),
  tile_0_0(tile_0_0_),
  tile_0_1(tile_0_1_),
  tile_0_2(tile_0_2_),
  tile_1_0(tile_1_0_),
  tile_1_1(tile_1_1_),
  tile_1_2(tile_1_2_)
{}

void SingleVersionItMapper::computeValidShapes(
  LoopTrees<Annotation>::loop_t * loop,
  std::vector<Runtime::OpenACC::loop_shape_t *> & shapes
) const {
  if (!loop->isDistributed()) return;

  std::vector<Annotation>::const_iterator it_annot;
  long gang = 1;
  long worker = 1;
  long vector = 1;
  for (it_annot = loop->annotations.begin(); it_annot != loop->annotations.end(); it_annot++) {
    switch (it_annot->clause->kind) {
      case ::DLX::OpenACC::language_t::e_acc_clause_gang:
        gang = 0; /// \todo might have a static value (from SgExpression)
        break;
      case ::DLX::OpenACC::language_t::e_acc_clause_worker:
        worker = 0; /// \todo might have a static value (from SgExpression)
        break;
      case ::DLX::OpenACC::language_t::e_acc_clause_vector:
        vector = 0; /// \todo might have a static value (from SgExpression)
        break;
      default: break; // to avoid tons of warning...
    }
  }
  assert(vector == 1);

  if (calls == 0)
    shapes.push_back(new Runtime::OpenACC::loop_shape_t(gang == 1 ? 1 : tile_0_0, gang, tile_0_1, worker, worker == 1 ? 1 : tile_0_2, 1, 1, false, false, false, false));
  else if (calls == 1)
    shapes.push_back(new Runtime::OpenACC::loop_shape_t(gang == 1 ? 1 : tile_1_0, gang, tile_1_1, worker, worker == 1 ? 1 : tile_1_2, 1, 1, false, false, false, false));
  else assert(false);

  calls++;
}



IterationMapperOpenACC::IterationMapperOpenACC() :
  IterationMapper<Annotation, Language::OpenCL, Runtime::OpenACC>(),
  tiling_sizes()
{}

IterationMapperOpenACC::IterationMapperOpenACC(const std::vector<unsigned> & tiling_sizes_) :
  IterationMapper<Annotation, Language::OpenCL, Runtime::OpenACC>(),
  tiling_sizes(tiling_sizes_)
{}

void IterationMapperOpenACC::computeValidShapes(
  LoopTrees<Annotation>::loop_t * loop,
  std::vector<Runtime::OpenACC::loop_shape_t *> & shapes
) const {
  if (!loop->isDistributed()) {
    shapes.push_back(new Runtime::OpenACC::loop_shape_t(0, 1, 1, 1, 1, 1, 1, false, false, false, false));
    return;
  }

  std::vector<Annotation>::const_iterator it_annot;
  long gang = 1;
  long worker = 1;
  long vector = 1;
  for (it_annot = loop->annotations.begin(); it_annot != loop->annotations.end(); it_annot++) {
    switch (it_annot->clause->kind) {
      case ::DLX::OpenACC::language_t::e_acc_clause_gang:
        gang = 0; /// \todo might have a static value (from SgExpression)
        break;
      case ::DLX::OpenACC::language_t::e_acc_clause_worker:
        worker = 0; /// \todo might have a static value (from SgExpression)
        break;
      case ::DLX::OpenACC::language_t::e_acc_clause_vector:
        vector = 0; /// \todo might have a static value (from SgExpression)
        break;
      default: break; // to avoid tons of warning...
    }
  }
  assert(vector == 1); /// \fixme vector not supported yet

  std::vector<unsigned>::const_iterator it_tiling_size;
  if (gang != 1 && worker != 1 && vector == 1) {
    // gang and vector: 3 tiles: tile_0, tile_1 and tile_2

    shapes.push_back(new Runtime::OpenACC::loop_shape_t(1, gang, 1, worker, 1, 1, 1, false, false, false, false));
/*
    shapes.push_back(new Runtime::OpenACC::loop_shape_t(0, gang, 1, worker, 1, 1, 1, false, false, false, false));
    shapes.push_back(new Runtime::OpenACC::loop_shape_t(1, gang, 0, worker, 1, 1, 1, false, false, false, false));
    shapes.push_back(new Runtime::OpenACC::loop_shape_t(1, gang, 1, worker, 0, 1, 1, false, false, false, false));
*/
    for (it_tiling_size = tiling_sizes.begin(); it_tiling_size != tiling_sizes.end(); it_tiling_size++) {
      unsigned tile = *it_tiling_size;

      assert(tile > 1);

      shapes.push_back(new Runtime::OpenACC::loop_shape_t(tile, gang,    1, worker,    1, 1, 1, false, false, false, false));
      shapes.push_back(new Runtime::OpenACC::loop_shape_t(tile, gang,    1, worker,    0, 1, 1, false, false, false, false));
      shapes.push_back(new Runtime::OpenACC::loop_shape_t(tile, gang,    0, worker,    1, 1, 1, false, false, false, false));

      shapes.push_back(new Runtime::OpenACC::loop_shape_t(   1, gang, tile, worker,    1, 1, 1, false, false, false, false));
      shapes.push_back(new Runtime::OpenACC::loop_shape_t(   1, gang, tile, worker,    0, 1, 1, false, false, false, false));
      shapes.push_back(new Runtime::OpenACC::loop_shape_t(   0, gang, tile, worker,    1, 1, 1, false, false, false, false));

      shapes.push_back(new Runtime::OpenACC::loop_shape_t(   1, gang,    1, worker, tile, 1, 1, false, false, false, false));
      shapes.push_back(new Runtime::OpenACC::loop_shape_t(   1, gang,    0, worker, tile, 1, 1, false, false, false, false));
      shapes.push_back(new Runtime::OpenACC::loop_shape_t(   0, gang,    1, worker, tile, 1, 1, false, false, false, false));

      shapes.push_back(new Runtime::OpenACC::loop_shape_t(tile, gang,    1, worker,    1, 1, 1,  true, false, false, false));
      shapes.push_back(new Runtime::OpenACC::loop_shape_t(tile, gang,    1, worker,    0, 1, 1,  true, false, false, false));
      shapes.push_back(new Runtime::OpenACC::loop_shape_t(tile, gang,    0, worker,    1, 1, 1,  true, false, false, false));

      shapes.push_back(new Runtime::OpenACC::loop_shape_t(   1, gang, tile, worker,    1, 1, 1, false,  true, false, false));
      shapes.push_back(new Runtime::OpenACC::loop_shape_t(   1, gang, tile, worker,    0, 1, 1, false,  true, false, false));
      shapes.push_back(new Runtime::OpenACC::loop_shape_t(   0, gang, tile, worker,    1, 1, 1, false,  true, false, false));

      shapes.push_back(new Runtime::OpenACC::loop_shape_t(   1, gang,    1, worker, tile, 1, 1, false, false,  true, false));
      shapes.push_back(new Runtime::OpenACC::loop_shape_t(   1, gang,    0, worker, tile, 1, 1, false, false,  true, false));
      shapes.push_back(new Runtime::OpenACC::loop_shape_t(   0, gang,    1, worker, tile, 1, 1, false, false,  true, false));
    }
  }
  else if (gang != 1 && worker == 1 && vector == 1) {
    // gang only      : 2 tiles: tile_0 and tile_1

    shapes.push_back(new Runtime::OpenACC::loop_shape_t(1, gang, 1, 1, 1, 1, 1, false, false, false, false));
/*
    shapes.push_back(new Runtime::OpenACC::loop_shape_t(0, gang, 1, 1, 1, 1, 1, false, false, false, false));
    shapes.push_back(new Runtime::OpenACC::loop_shape_t(1, gang, 0, 1, 1, 1, 1, false, false, false, false));
*/
    for (it_tiling_size = tiling_sizes.begin(); it_tiling_size != tiling_sizes.end(); it_tiling_size++) {
      unsigned tile = *it_tiling_size;

      assert(tile > 1);

      shapes.push_back(new Runtime::OpenACC::loop_shape_t(tile, gang,    1, 1, 1, 1, 1, false, false, false, false));
      shapes.push_back(new Runtime::OpenACC::loop_shape_t(tile, gang,    0, 1, 1, 1, 1, false, false, false, false));

      shapes.push_back(new Runtime::OpenACC::loop_shape_t(   1, gang, tile, 1, 1, 1, 1, false, false, false, false));
      shapes.push_back(new Runtime::OpenACC::loop_shape_t(   0, gang, tile, 1, 1, 1, 1, false, false, false, false));

      shapes.push_back(new Runtime::OpenACC::loop_shape_t(tile, gang,    1, 1, 1, 1, 1,  true, false, false, false));
      shapes.push_back(new Runtime::OpenACC::loop_shape_t(tile, gang,    0, 1, 1, 1, 1,  true, false, false, false));

      shapes.push_back(new Runtime::OpenACC::loop_shape_t(   1, gang, tile, 1, 1, 1, 1, false,  true, false, false));
      shapes.push_back(new Runtime::OpenACC::loop_shape_t(   0, gang, tile, 1, 1, 1, 1, false,  true, false, false));
    }
  }
  else if (gang == 1 && worker != 1 && vector == 1) {
    // vector only    : 2 tiles: tile_1 and tile_2

    shapes.push_back(new Runtime::OpenACC::loop_shape_t(1, 1, 1, worker, 1, 1, 1, false, false, false, false));
/*
    shapes.push_back(new Runtime::OpenACC::loop_shape_t(1, 1, 0, worker, 1, 1, 1, false, false, false, false));
    shapes.push_back(new Runtime::OpenACC::loop_shape_t(1, 1, 1, worker, 0, 1, 1, false, false, false, false));
*/
    for (it_tiling_size = tiling_sizes.begin(); it_tiling_size != tiling_sizes.end(); it_tiling_size++) {
      unsigned tile = *it_tiling_size;

      assert(tile > 1);

      shapes.push_back(new Runtime::OpenACC::loop_shape_t(1, 1, tile, worker,    1, 1, 1, false, false, false, false));
      shapes.push_back(new Runtime::OpenACC::loop_shape_t(1, 1, tile, worker,    0, 1, 1, false, false, false, false));

      shapes.push_back(new Runtime::OpenACC::loop_shape_t(1, 1,    1, worker, tile, 1, 1, false, false, false, false));
      shapes.push_back(new Runtime::OpenACC::loop_shape_t(1, 1,    0, worker, tile, 1, 1, false, false, false, false));

      shapes.push_back(new Runtime::OpenACC::loop_shape_t(1, 1, tile, worker,    1, 1, 1, false,  true, false, false));
      shapes.push_back(new Runtime::OpenACC::loop_shape_t(1, 1, tile, worker,    0, 1, 1, false,  true, false, false));

      shapes.push_back(new Runtime::OpenACC::loop_shape_t(1, 1,    1, worker, tile, 1, 1, false, false,  true, false));
      shapes.push_back(new Runtime::OpenACC::loop_shape_t(1, 1,    0, worker, tile, 1, 1, false, false,  true, false));
    }
  }
  else if (gang == 1 && worker == 1 && vector != 1) {
    assert(false); /// \todo vector not supported yet
  }
  else if (gang == 1 && worker != 1 && vector != 1) {
    assert(false); /// \todo vector not supported yet
  }
  else if (gang != 1 && worker == 1 && vector != 1) {
    assert(false); /// \todo vector not supported yet
  }
  else if (gang != 1 && worker != 1 && vector != 1) {
    assert(false); /// \todo vector not supported yet
  }
  else assert(false); // Not reachable => !loop->isDistributed()
}

}

