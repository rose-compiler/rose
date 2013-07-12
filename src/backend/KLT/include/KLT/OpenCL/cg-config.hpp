
#ifndef __KLT_OPENCL_CG_CONFIG_HPP__
#define __KLT_OPENCL_CG_CONFIG_HPP__

#include "KLT/Core/cg-config.hpp"

#include <cstddef>

namespace KLT {

namespace OpenCL {

class WorkSizeShaper;

class CG_Config : public Core::CG_Config {
  protected:
    WorkSizeShaper * p_work_size_shaper;

  public:
    CG_Config(Core::DataFlow * data_flow, Core::LoopSelector * loop_selector, WorkSizeShaper * work_size_shaper);
    virtual ~CG_Config();

    const WorkSizeShaper & getWorkSizeShaper() const;
};

}

}

#endif /* __KLT_OPENCL_CG_CONFIG_HPP__ */
