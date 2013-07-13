
#include "KLT/Sequential/generator.hpp"

#include "KLT/Sequential/kernel.hpp"

#include "KLT/Sequential/mfb-klt.hpp"

#include <cassert>

namespace KLT {

namespace Sequential {

void Generator::doCodeGeneration(Core::Kernel * kernel_, const Core::CG_Config & cg_config) {
  Kernel * kernel = dynamic_cast<Kernel *>(kernel_);
  assert(kernel != NULL);

  assert(kernel->isDataflowDone());
  assert(kernel->isArgumentDone());
  assert(kernel->isLoopDistributionDone());
  assert(kernel->isIterationMapDone());
  
/*
  ::MultiFileBuilder::KLT<Kernel>::object_desc_t kernel_desc(kernel, p_file_id);

  ::MultiFileBuilder::KLT<Kernel>::build_result_t result = p_klt_driver.build<Kernel>(kernel_desc);

  kernel->setKernelSymbol(result.kernel);
  kernel->setArgumentPacker(result.arguments_packer);
*/
}

Core::Kernel * Generator::makeKernel() const {
  return new Kernel();
}

Generator::Generator(SgProject * project, const std::string & filename_) :
  Core::Generator(project),
  p_filename(filename_),
  p_file_id(p_sage_driver.createPairOfFiles(p_filename))
{}

Generator::~Generator() {}

}

}

