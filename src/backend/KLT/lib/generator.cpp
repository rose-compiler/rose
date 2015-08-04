
#include "sage3basic.h"

#include "MFB/Sage/class-declaration.hpp"
#include "MFB/Sage/driver.hpp"
#include "MFB/KLT/driver.hpp"

#include "MDCG/Core/model.hpp"
#include "MDCG/Core/model-builder.hpp"

#include "KLT/generator.hpp"
#include "KLT/descriptor.hpp"
#include "KLT/api.hpp"

namespace KLT {

Generator::Generator(MFB::Driver<MFB::KLT::KLT> & driver_, MDCG::ModelBuilder & model_builder_) :
  driver(driver_), model_builder(model_builder_),
  tilek_model(model_builder.create()),
  kernel_file_id(0), static_file_id(0),
  host_api(NULL), kernel_api(NULL), call_interface(NULL)
{}

void Generator::loadModel(const std::string & klt_inc_dir, const std::string & usr_inc_dir) {
  model_builder.add(tilek_model, "data",    klt_inc_dir + "/KLT/RTL", "h");
  model_builder.add(tilek_model, "tile",    klt_inc_dir + "/KLT/RTL", "h");
  model_builder.add(tilek_model, "loop",    klt_inc_dir + "/KLT/RTL", "h");
  model_builder.add(tilek_model, "context", klt_inc_dir + "/KLT/RTL", "h");

  loadExtraModel(usr_inc_dir);

  assert(host_api != NULL);
  host_api->load(model_builder.get(tilek_model));

  assert(kernel_api != NULL);
  kernel_api->load(model_builder.get(tilek_model));
}

MFB::Driver<MFB::KLT::KLT> & Generator::getDriver() { return driver; }
const MFB::Driver<MFB::KLT::KLT> & Generator::getDriver() const { return driver; }
MDCG::ModelBuilder & Generator::getModelBuilder() { return model_builder; }
const MDCG::ModelBuilder & Generator::getModelBuilder() const { return model_builder; }
API::host_t & Generator::getHostAPI() { return *host_api; }
const API::host_t & Generator::getHostAPI() const { return *host_api; }
API::kernel_t & Generator::getKernelAPI() { return *kernel_api; }
const API::kernel_t & Generator::getKernelAPI() const { return *kernel_api; }
API::call_interface_t & Generator::getCallInterface() { return *call_interface; }
const API::call_interface_t & Generator::getCallInterface() const { return *call_interface; }
MFB::file_id_t Generator::getKernelFileID() const { return kernel_file_id; }
MFB::file_id_t Generator::getStaticFileID() const { return static_file_id; }

void Generator::solveDataFlow(
  Kernel::kernel_t * kernel,
  const std::vector<Kernel::kernel_t *> & subkernels,
  std::map<Descriptor::kernel_t *, std::vector<Descriptor::kernel_t *> > & kernel_deps_map,
  const std::map<Kernel::kernel_t *, Descriptor::kernel_t *> & translation_map,
  const std::map<Descriptor::kernel_t *, Kernel::kernel_t *> & rtranslation_map
) const {
  // Simplest implementation I can think of: enforce text order...

  Descriptor::kernel_t * previous = NULL;
  Descriptor::kernel_t * current = NULL;

  std::map<Descriptor::kernel_t *, std::vector<Descriptor::kernel_t *> >::iterator it_kernel_deps;
  std::map<Kernel::kernel_t *, Descriptor::kernel_t *>::const_iterator it_trans;

  std::vector<Kernel::kernel_t *>::const_iterator it;
  for (it = subkernels.begin(); it != subkernels.end(); it++) {
    it_trans = translation_map.find(*it);
    assert(it_trans != translation_map.end());
    current = it_trans->second;

    it_kernel_deps = kernel_deps_map.find(current);
    assert(it_kernel_deps != kernel_deps_map.end());
    std::vector<Descriptor::kernel_t *> & deps = it_kernel_deps->second;

    if (previous != NULL)
      deps.push_back(previous);

    previous = current;
  }
}

} // namespace KLT

