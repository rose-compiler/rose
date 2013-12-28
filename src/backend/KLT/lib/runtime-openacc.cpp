
#include "KLT/runtime-openacc.hpp"

namespace KLT {

namespace Runtime {

OpenACC::exec_mode_e OpenACC::default_execution_mode = OpenACC::gr_ws_vs;

OpenACC::loop_shape_t::loop_shape_t(long tile_0_, long gang_, long tile_1_, long worker_, long tile_2_, long vector_, long tile_3_) :
  tile_0(tile_0_),
  gang(gang_),
  tile_1(tile_1_),
  worker(worker_),
  tile_2(tile_2_),
  vector(vector_),
  tile_3(tile_3_)
{}

}

}

