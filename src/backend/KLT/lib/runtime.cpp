
#include "sage3basic.h"

#include "MFB/Sage/class-declaration.hpp"
#include "MFB/Sage/driver.hpp"

#include "MDCG/Core/model.hpp"
//#include "MDCG/Core/model-class.hpp"
#include "MDCG/Core/model-builder.hpp"

#include "KLT/runtime.hpp"
#include "KLT/api.hpp"
#include "KLT/descriptor.hpp"

namespace KLT {

Runtime::Runtime(
  MDCG::ModelBuilder & model_builder_,
  const std::string & klt_rtl_inc_dir_, const std::string & klt_rtl_lib_dir_,
  const std::string & usr_rtl_inc_dir_, const std::string & usr_rtl_lib_dir_,
  kernel_api_t * kernel_api_, host_api_t * host_api_
) :
  model_builder(model_builder_),
  klt_rtl_inc_dir(klt_rtl_inc_dir_),
  klt_rtl_lib_dir(klt_rtl_lib_dir_),
  usr_rtl_inc_dir(usr_rtl_inc_dir_),
  usr_rtl_lib_dir(usr_rtl_lib_dir_),
  kernel_api(kernel_api_ != NULL ? kernel_api : new kernel_api_t()),
  host_api  (host_api_   != NULL ? host_api   : new host_api_t()),
  tilek_model(model_builder.create())
{}

Runtime::~Runtime() {
  if (kernel_api != NULL) delete kernel_api;
  if (host_api != NULL) delete host_api;
}

void Runtime::loadModel() {
  model_builder.add(tilek_model, "data",    klt_rtl_inc_dir + "/KLT/RTL", "h");
  model_builder.add(tilek_model, "tile",    klt_rtl_inc_dir + "/KLT/RTL", "h");
  model_builder.add(tilek_model, "loop",    klt_rtl_inc_dir + "/KLT/RTL", "h");
  model_builder.add(tilek_model, "context", klt_rtl_inc_dir + "/KLT/RTL", "h");

  loadUserModel();

  host_api->load(model_builder.get(tilek_model));
  kernel_api->load(model_builder.get(tilek_model));
}

void Runtime::useKernelSymbols(MFB::Driver<MFB::Sage> & driver, size_t file_id) const {
  driver.useSymbol<SgClassDeclaration>(kernel_api->getLoopContextClass(), file_id);
  useUserKernelSymbols(driver, file_id);
}

void Runtime::useHostSymbols(MFB::Driver<MFB::Sage> & driver, size_t file_id) const {
  driver.useSymbol<SgClassDeclaration>(host_api->getDataClass(),   file_id);
  driver.useSymbol<SgClassDeclaration>(host_api->getTileClass(),   file_id);
  driver.useSymbol<SgClassDeclaration>(host_api->getLoopClass(),   file_id);
  driver.useSymbol<SgClassDeclaration>(host_api->getKernelClass(), file_id);
  useUserHostSymbols(driver, file_id);
}

} // namespace KLT

