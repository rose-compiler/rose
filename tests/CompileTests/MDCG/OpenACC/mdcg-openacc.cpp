
#include "MDCG/model-builder.hpp"

#include <cassert>

namespace MFB {
  class ApiBuilder {}; /// \todo #include "MFB/api-builder.hpp"
}

int main(int argc, char ** argv) {
  MFB::ApiBuilder api_builder;
  MDCG::ModelBuilder model_builder(api_builder);

  return 0;
}

