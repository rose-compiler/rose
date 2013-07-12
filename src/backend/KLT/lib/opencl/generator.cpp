
#include "KLT/OpenCL/generator.hpp"

#include "KLT/OpenCL/kernel.hpp"

#include "KLT/OpenCL/mfb-klt.hpp"

#include "KLT/OpenCL/cg-config.hpp"
#include "KLT/OpenCL/work-size-shaper.hpp"

#include <cassert>

namespace KLT {

namespace OpenCL {

void Generator::doCodeGeneration(Core::Kernel * kernel_, const Core::CG_Config & cg_config_) {
  Kernel * kernel = dynamic_cast<Kernel *>(kernel_);
  assert(kernel != NULL);

  const OpenCL::CG_Config & cg_config = dynamic_cast<const OpenCL::CG_Config &>(cg_config_); // can throw an exception of type std::bad_cast

  const WorkSizeShaper & work_size_shaper = cg_config.getWorkSizeShaper();

  assert(kernel->isDataflowDone());
  assert(kernel->isArgumentDone());
  assert(kernel->isContentDone());

  std::set<WorkSizeShape *> shapes;
  work_size_shaper.generateShapes(kernel->getPerfectlyNestedLoops(), kernel->getParametersArguments(), shapes);

  std::set<WorkSizeShape *>::const_iterator it_shape;
  for (it_shape = shapes.begin(); it_shape != shapes.end(); it_shape++) {
    WorkSizeShape * work_size_shape = *it_shape;

    ::MultiFileBuilder::KLT<Kernel>::object_desc_t kernel_desc(kernel, work_size_shape, p_file_id);

    ::MultiFileBuilder::KLT<Kernel>::build_result_t result = p_klt_driver.build<Kernel>(kernel_desc);

    kernel->addKernel(result);
  }

}

Core::Kernel * Generator::makeKernel() const {
  return new Kernel();
}

Generator::Generator(SgProject * project, const std::string & filename_) :
  Core::Generator(project),
  p_filename(filename_),
  p_file_id(p_sage_driver.createStandaloneSourceFile(p_filename, "cl"))
{}

Generator::~Generator() {}

}

}
