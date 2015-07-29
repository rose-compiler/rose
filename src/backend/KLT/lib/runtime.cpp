
#include "sage3basic.h"

#include "MFB/Sage/class-declaration.hpp"
#include "MFB/Sage/driver.hpp"

#include "MDCG/Core/model.hpp"
#include "MDCG/Core/model-builder.hpp"

#include "KLT/runtime.hpp"
#include "KLT/descriptor.hpp"
#include "KLT/api.hpp"

namespace KLT {

Runtime::Runtime(
  MDCG::ModelBuilder & model_builder_,
  const std::string & klt_rtl_inc_dir_, const std::string & klt_rtl_lib_dir_,
  const std::string & usr_rtl_inc_dir_, const std::string & usr_rtl_lib_dir_,
  API::host_t * host_api_, API::kernel_t * kernel_api_, API::call_interface_t * call_interface_
) :
  model_builder(model_builder_),
  klt_rtl_inc_dir(klt_rtl_inc_dir_), klt_rtl_lib_dir(klt_rtl_lib_dir_),
  usr_rtl_inc_dir(usr_rtl_inc_dir_), usr_rtl_lib_dir(usr_rtl_lib_dir_),
  host_api(host_api_), kernel_api(kernel_api_), call_interface(call_interface_),
  tilek_model(model_builder.create())
{}

void Runtime::loadModel() {
  model_builder.add(tilek_model, "data",    klt_rtl_inc_dir + "/KLT/RTL", "h");
  model_builder.add(tilek_model, "tile",    klt_rtl_inc_dir + "/KLT/RTL", "h");
  model_builder.add(tilek_model, "loop",    klt_rtl_inc_dir + "/KLT/RTL", "h");
  model_builder.add(tilek_model, "context", klt_rtl_inc_dir + "/KLT/RTL", "h");

  loadExtraModel();

    host_api->load(model_builder.get(tilek_model));
  kernel_api->load(model_builder.get(tilek_model));
}

      API::host_t & Runtime::getHostAPI()       { return *host_api; }
const API::host_t & Runtime::getHostAPI() const { return *host_api; }
      API::kernel_t & Runtime::getKernelAPI()       { return *kernel_api; }
const API::kernel_t & Runtime::getKernelAPI() const { return *kernel_api; }
      API::call_interface_t & Runtime::getCallInterface()       { return *call_interface; }
const API::call_interface_t & Runtime::getCallInterface() const { return *call_interface; }

} // namespace KLT

