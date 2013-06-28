
#ifndef __KLT_CORE_GENERATOR_HPP__
#define __KLT_CORE_GENERATOR_HPP__

#include <map>
#include <set>
#include <list>
#include <string>

#include <KLT/Core/loop-trees.hpp>

class SgProject;
class SgSourceFile;
class SgVariableSymbol;

namespace KLT {

namespace Core {

class Data;
class Kernel;

class CG_Config;
class LoopSelector;
class DataFlow;

class Generator {
  public:

  protected:
    SgProject * p_project;

  protected:
    Generator(SgProject * project);

    virtual void buildArgumentLists(
      const std::map<Kernel *, LoopTrees::node_t *> & kernels_map,
      const std::set<SgVariableSymbol *> & parameters,
      const std::set<SgVariableSymbol *> & coefficients,
      const std::set<Data *> & datas_in,
      const std::set<Data *> & datas_out,
      const std::set<Data *> & datas_local
    ); 

    void generate(const LoopTrees & loop_trees, std::list<Kernel *> & kernels, const DataFlow & data_flow, const LoopSelector & loop_selector);

    virtual void doCodeGeneration(Kernel * kernel, const CG_Config & cg_config) = 0;

  public:
    virtual ~Generator();

    virtual Kernel * makeKernel() const = 0;

    /// Produce a set of Kernel (and associated dependencies map) from a LoopTrees object
//  bool generate(const LoopTrees & loop_trees, std::set<Kernel *> & kernels, std::map<unsigned long, std::set<unsigned long> > & kernel_deps);

    /// 
    virtual void generate(const LoopTrees & loop_trees, std::list<Kernel *> & kernels, const CG_Config & cg_config);
};

}

}

#endif /* __KLT_CORE_GENERATOR_HPP__ */

