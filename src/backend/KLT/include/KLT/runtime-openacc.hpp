
#ifndef __KLT_RUNTIME_OPENACC_HPP__
#define __KLT_RUNTIME_OPENACC_HPP__

namespace KLT {

namespace Runtime {

class OpenACC {
  public:
    struct loop_shape_t {
      bool gang;
      bool worker;
      bool vector;
    };

    struct shape_config_t {
      long tile_0;
      long tile_1;
      long tile_2;
      long tile_3;
    };
};

}

}

#endif /* __KLT_RUNTIME_OPENACC_HPP__ */

