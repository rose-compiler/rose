
#include "klt_itmap.hpp"

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
    shapes.push_back(new Runtime::OpenACC::loop_shape_t(gang == 1 ? 1 : tile_0_0, gang, tile_0_1, worker, worker == 1 ? 1 : tile_0_2, 1, 1));
  else if (calls == 1)
    shapes.push_back(new Runtime::OpenACC::loop_shape_t(gang == 1 ? 1 : tile_1_0, gang, tile_1_1, worker, worker == 1 ? 1 : tile_1_2, 1, 1));
  else assert(false);

  calls++;
}

}
