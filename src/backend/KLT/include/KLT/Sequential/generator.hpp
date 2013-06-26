
#ifndef __KLT_SEQUENTIAL_GENERATOR_HPP__
#define __KLT_SEQUENTIAL_GENERATOR_HPP__

#include "KLT/Core/generator.hpp"

#include "KLT/Sequential/kernel.hpp"

#include <string>

namespace KLT {

namespace Sequential {

class Generator : public virtual Core::Generator {
  protected:
    SgSourceFile * p_decl_file;
    SgSourceFile * p_defn_file;

  protected:
    virtual Kernel * makeKernel() const;
    virtual void doCodeGeneration(Core::Kernel * kernel, const Core::CG_Config & cg_config);

  public:
    Generator(SgProject * project, std::string filename);
    virtual ~Generator();

    void unparse();
};

}

}

#endif /* __KLT_SEQUENTIAL_GENERATOR_HPP__ */

