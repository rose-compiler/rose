
#ifndef __KLT_CORE_GENERATOR_HPP__
#define __KLT_CORE_GENERATOR_HPP__

#include <map>
#include <set>
#include <list>
#include <string>

class SgProject;
class SgSourceFile;

namespace KLT {

namespace Core {

class LoopTrees;
class Kernel;
class LoopSelector;
class CG_Config;
class DataFlow;

class Generator {
  public:

  protected:
    SgProject * p_project;

  protected:
    Generator(SgProject * project);

    void generate(const LoopTrees & loop_trees, std::list<Kernel *> & kernels, const DataFlow & data_flow, const LoopSelector & loop_selector);

    virtual Kernel * makeKernel() const = 0;
    virtual void doCodeGeneration(Kernel * kernel, const CG_Config & cg_config) = 0;

  public:
    virtual ~Generator();

    /// Produce a set of Kernel (and associated dependencies map) from a LoopTrees object
//  bool generate(const LoopTrees & loop_trees, std::set<Kernel *> & kernels, std::map<unsigned long, std::set<unsigned long> > & kernel_deps);

    /// 
    virtual void generate(const LoopTrees & loop_trees, std::list<Kernel *> & kernels, const CG_Config & cg_config);
};

}

}

#endif /* __KLT_CORE_GENERATOR_HPP__ */

