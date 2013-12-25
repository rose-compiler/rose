
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
};

}

}

#endif /* __KLT_RUNTIME_OPENACC_HPP__ */

