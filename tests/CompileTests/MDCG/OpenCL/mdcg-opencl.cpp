
#include "MDCG/model-builder.hpp"

#include <vector>
#include <string>

#include <cassert>

/// \todo #include "MFB/api-builder.hpp"
namespace MFB {
  class ApiBuilder { 
    public:
      ApiBuilder(const std::vector<std::string> & inc_paths);
  };
}

int main(int argc, char ** argv) {
  std::vector<std::string> inc_paths;
    inc_paths.push_back(std::string(argv[1]));
  MFB::ApiBuilder api_builder(inc_paths);

  MDCG::ModelBuilder model_builder(api_builder);

  std::vector<std::string> headers;
    headers.push_back("CL/cl.hpp");
  MDCG::Model::model_id_t opencl_model_id = model_builder.build(headers);

  const MDCG::Model::Model & model = model_builder.get(opencl_model_id);

  /// \todo display model

  return 0;
}

