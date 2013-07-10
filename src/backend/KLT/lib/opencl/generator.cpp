
#include "KLT/OpenCL/generator.hpp"

namespace KLT {

namespace OpenCL {

void Generator::doCodeGeneration(Core::Kernel * kernel, const Core::CG_Config & cg_config) {

}

void Generator::init() {
  // TODO
}

Generator::Generator(SgProject * project, const std::string & filename_) :
  Core::Generator(project),
  p_filename(filename_)
{}

Generator::~Generator() {}

Core::Kernel * Generator::makeKernel() const {
  // TODO
  return NULL;
}

}

}
