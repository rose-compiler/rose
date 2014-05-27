
#include "KLT/OpenACC/runtime-openacc.hpp"
#include "MFB/Sage/api.hpp"

namespace KLT {

namespace Runtime {

OpenACC::exec_mode_e OpenACC::default_execution_mode = OpenACC::gr_ws_vs;

OpenACC::loop_shape_t::loop_shape_t(
  long tile_0_, long gang_, long tile_1_, long worker_, long tile_2_, long vector_, long tile_3_,
  bool unroll_tile_0_, bool unroll_tile_1_, bool unroll_tile_2_, bool unroll_tile_3_
) :
  tile_0(tile_0_),
  gang(gang_),
  tile_1(tile_1_),
  worker(worker_),
  tile_2(tile_2_),
  vector(vector_),
  tile_3(tile_3_),
  unroll_tile_0(unroll_tile_0_),
  unroll_tile_1(unroll_tile_1_),
  unroll_tile_2(unroll_tile_2_),
  unroll_tile_3(unroll_tile_3_)
{}


SgClassSymbol * OpenACC::runtime_device_context_symbol = NULL;
OpenACC::runtime_device_function_symbols_t OpenACC::runtime_device_function_symbols = { NULL, NULL };
OpenACC::runtime_context_symbols_t         OpenACC::runtime_context_symbols = { NULL, NULL, NULL, NULL, NULL };
OpenACC::runtime_kernel_loop_symbols_t     OpenACC::runtime_kernel_loop_symbols = { NULL, NULL, NULL, NULL };
OpenACC::runtime_loop_desc_symbols_t       OpenACC::runtime_loop_desc_symbols = { NULL, NULL, NULL, NULL };

void OpenACC::loadAPI(MFB::Driver<MFB::Sage> & mfb_driver, std::string inc_path) {
  // Make sure MFB's Driver have loaded OpenACC Device API (from OpenCL file, to get OCL's preinclude)
  mfb_driver.add(boost::filesystem::path(inc_path + "/OpenACC/device/api.cl"));

  // Get entire API because MDCG might have already loaded some declaration through other headers...
  MFB::api_t * api = mfb_driver.getAPI();

  std::set<SgClassSymbol *>::const_iterator it_class;
  for (it_class = api->class_symbols.begin(); it_class != api->class_symbols.end(); it_class++)
    if ((*it_class)->get_name().getString() == "acc_context_t_")
      runtime_device_context_symbol = *it_class;
  assert(runtime_device_context_symbol != NULL);

  std::set<SgFunctionSymbol *>::const_iterator it_func;
  for (it_func = api->function_symbols.begin(); it_func != api->function_symbols.end(); it_func++) {
    if ((*it_func)->get_name().getString() == "acc_gang_iteration")
      runtime_device_function_symbols.gang_iter_symbol = *it_func;
    else if ((*it_func)->get_name().getString() == "acc_worker_iteration")
      runtime_device_function_symbols.worker_iter_symbol = *it_func;
  }
  assert(runtime_device_function_symbols.gang_iter_symbol != NULL);
  assert(runtime_device_function_symbols.worker_iter_symbol != NULL);

  std::set<SgVariableSymbol *>::const_iterator it_var;
  for (it_var = api->variable_symbols.begin(); it_var != api->variable_symbols.end(); it_var++) {
    if ((*it_var)->get_name().getString() == "num_gang")
      runtime_context_symbols.num_gang_symbol = *it_var;
    else if ((*it_var)->get_name().getString() == "num_worker")
      runtime_context_symbols.num_worker_symbol = *it_var;
    else if ((*it_var)->get_name().getString() == "vector_length")
      runtime_context_symbols.vector_length_symbol = *it_var;
    else if ((*it_var)->get_name().getString() == "num_loop")
      runtime_context_symbols.num_loop_symbol = *it_var;
    else if ((*it_var)->get_name().getString() == "loops")
      runtime_context_symbols.loops_symbol = *it_var;

    else if ((*it_var)->get_name().getString() == "original")
      runtime_kernel_loop_symbols.original_symbol = *it_var;
    else if ((*it_var)->get_name().getString() == "tiles")
      runtime_kernel_loop_symbols.tiles_symbol = *it_var;
    else if ((*it_var)->get_name().getString() == "length")
      runtime_kernel_loop_symbols.tiles_length_symbol = *it_var;

    else if ((*it_var)->get_name().getString() == "lower")
      runtime_loop_desc_symbols.lower_symbol = *it_var;
    else if ((*it_var)->get_name().getString() == "upper")
      runtime_loop_desc_symbols.upper_symbol = *it_var;
    else if ((*it_var)->get_name().getString() == "nbr_it")
      runtime_loop_desc_symbols.nbr_it_symbol = *it_var;

    else if ((*it_var)->get_name().getString() == "stride") {
      SgClassDefinition * class_def = isSgClassDefinition((*it_var)->get_scope());
      if (class_def != NULL) {
        if (class_def->get_declaration()->get_name().getString() == "acc_loop_desc_t_")
          runtime_kernel_loop_symbols.tiles_stride_symbol = *it_var;
        else if (class_def->get_declaration()->get_name().getString() == "kernel_loop_tile_t_")
          runtime_loop_desc_symbols.stride_symbol = *it_var;
      }
    }
  }
  assert(runtime_context_symbols.num_gang_symbol != NULL);
  assert(runtime_context_symbols.num_worker_symbol != NULL);
  assert(runtime_context_symbols.vector_length_symbol != NULL);
  assert(runtime_context_symbols.num_loop_symbol != NULL);
  assert(runtime_context_symbols.loops_symbol != NULL);
  assert(runtime_kernel_loop_symbols.original_symbol != NULL);
  assert(runtime_kernel_loop_symbols.tiles_symbol != NULL);
  assert(runtime_kernel_loop_symbols.tiles_length_symbol != NULL);
  assert(runtime_loop_desc_symbols.lower_symbol != NULL);
  assert(runtime_loop_desc_symbols.upper_symbol != NULL);
  assert(runtime_loop_desc_symbols.nbr_it_symbol != NULL);
  assert(runtime_kernel_loop_symbols.tiles_stride_symbol != NULL);
  assert(runtime_loop_desc_symbols.stride_symbol != NULL);

}

}

}

