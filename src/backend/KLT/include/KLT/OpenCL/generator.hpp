
#ifndef __KLT_OPENCL_GENERATOR_HPP__
#define __KLT_OPENCL_GENERATOR_HPP__

#include "KLT/Core/generator.hpp"

#include <string>

namespace KLT {

namespace OpenCL {

class Generator : public virtual Core::Generator {
  protected:
    std::string p_filename;
    unsigned long p_file_id;
    
  protected:
    virtual void doCodeGeneration(Core::Kernel * kernel, const Core::CG_Config & cg_config);
    
  public:
    Generator(SgProject * project, const std::string & filename_);
    virtual ~Generator();

    virtual Core::Kernel * makeKernel() const;
};

}

}

#endif /* __KLT_OPENCL_GENERATOR_HPP__ */

