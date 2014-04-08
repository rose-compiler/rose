
#ifndef __KLT_RUNTIME_OPENACC_HPP__
#define __KLT_RUNTIME_OPENACC_HPP__

#include "MFB/Sage/driver.hpp"

class SgExpression;
class SgVariableSymbol;
class SgFunctionSymbol;
class SgClassSymbol;

namespace KLT {

namespace Runtime {

class OpenACC {
  public:
    struct loop_shape_t {
      loop_shape_t(long, long, long, long, long, long, long, bool, bool, bool, bool);

      long tile_0;
      long gang;
      long tile_1;
      long worker;
      long tile_2;
      long vector;
      long tile_3;

      bool unroll_tile_0;
      bool unroll_tile_1;
      bool unroll_tile_2;
      bool unroll_tile_3;

      SgVariableSymbol * iterators[7];
    };

    struct a_loop {
      unsigned id;

      SgExpression * lb;
      SgExpression * ub;

      long tile_0;
      long gang;
      long tile_1;
      long worker;
      long tile_2;
      long vector;
      long tile_3;

      bool unroll_tile_0;
      bool unroll_tile_1;
      bool unroll_tile_2;
      bool unroll_tile_3;
    };

    enum exec_mode_e {
      gr_ws_vs, /// Gang Redondant   , Worker Single      , Vector Single
      gp_ws_vs, /// Gang Partitioned , Worker Single      , Vector Single
      gr_wp_vs, /// Gang Redondant   , Worker Partitioned , Vector Single
      gp_wp_vs, /// Gang Partitioned , Worker Partitioned , Vector Single
      gr_ws_vp, /// Gang Redondant   , Worker Single      , Vector Partitioned
      gp_ws_vp, /// Gang Partitioned , Worker Single      , Vector Partitioned
      gr_wp_vp, /// Gang Redondant   , Worker Partitioned , Vector Partitioned
      gp_wp_vp  /// Gang Partitioned , Worker Partitioned , Vector Partitioned
    };

    static exec_mode_e default_execution_mode;

    static SgClassSymbol * runtime_device_context_symbol;

    struct runtime_device_function_symbols_t {
      SgFunctionSymbol * gang_iter_symbol;
      SgFunctionSymbol * worker_iter_symbol;
    };
    static runtime_device_function_symbols_t runtime_device_function_symbols;

    struct runtime_context_symbols_t {
      SgVariableSymbol * num_gang_symbol;
      SgVariableSymbol * num_worker_symbol;
      SgVariableSymbol * vector_length_symbol;
      SgVariableSymbol * num_loop_symbol;
      SgVariableSymbol * loops_symbol;
    };
    static runtime_context_symbols_t runtime_context_symbols;

    struct runtime_kernel_loop_symbols_t {
      SgVariableSymbol * original_symbol;
      SgVariableSymbol * tiles_symbol;
      SgVariableSymbol * tiles_stride_symbol;
      SgVariableSymbol * tiles_length_symbol;
    };
    static runtime_kernel_loop_symbols_t runtime_kernel_loop_symbols;

    struct runtime_loop_desc_symbols_t {
      SgVariableSymbol * lower_symbol;
      SgVariableSymbol * upper_symbol;
      SgVariableSymbol * stride_symbol;
      SgVariableSymbol * nbr_it_symbol;
    };
    static runtime_loop_desc_symbols_t runtime_loop_desc_symbols;

    static void loadAPI(MFB::Driver<MFB::Sage> & mfb_driver, std::string inc_path);
};

}

}

#endif /* __KLT_RUNTIME_OPENACC_HPP__ */

