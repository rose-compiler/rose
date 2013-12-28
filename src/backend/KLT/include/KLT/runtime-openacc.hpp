
#ifndef __KLT_RUNTIME_OPENACC_HPP__
#define __KLT_RUNTIME_OPENACC_HPP__

class SgExpression;

namespace KLT {

namespace Runtime {

class OpenACC {
  public:
    struct loop_shape_t {
      loop_shape_t(long, long, long, long, long, long, long);

      long tile_0;
      long gang;
      long tile_1;
      long worker;
      long tile_2;
      long vector;
      long tile_3;
    };

    struct a_loop {
      SgExpression * lb;
      SgExpression * ub;

      loop_shape_t shape;
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
};

}

}

#endif /* __KLT_RUNTIME_OPENACC_HPP__ */

