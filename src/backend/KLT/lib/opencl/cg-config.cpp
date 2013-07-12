
#include "KLT/OpenCL/cg-config.hpp"

#include "KLT/Core/data-flow.hpp"
#include "KLT/Core/loop-selector.hpp"

#include "KLT/OpenCL/work-size-shaper.hpp"

#include <cassert>

namespace KLT {

namespace OpenCL {

CG_Config::CG_Config(Core::DataFlow * data_flow, Core::LoopSelector * loop_selector, WorkSizeShaper * work_size_shaper) :
  Core::CG_Config(data_flow, loop_selector),
  p_work_size_shaper(work_size_shaper)
{
  assert(p_work_size_shaper != NULL);
}

CG_Config::~CG_Config() {
  if (p_work_size_shaper != NULL) delete p_work_size_shaper;
}

const WorkSizeShaper & CG_Config::getWorkSizeShaper() const { return *p_work_size_shaper; }

}

}

