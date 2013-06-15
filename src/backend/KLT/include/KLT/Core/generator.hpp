
#ifndef __KLT_CORE_GENERATOR_HPP__
#define __KLT_CORE_GENERATOR_HPP__

#include <map>
#include <set>

namespace KLT {

namespace Core {

class LoopTrees;
class Kernel;

class Generator {
  protected:
    

  protected:
    Generator();

  public:
    virtual ~Generator();

    /// Produce a set of Kernel (and associated dependencies map) from a LoopTrees object
    virtual bool generate(const LoopTrees & loop_trees, std::set<Kernel> & kernels, std::map<unsigned long, std::set<unsigned long> > & kernel_deps) = 0;
};

}

}

#endif /* __KLT_CORE_GENERATOR_HPP__ */

