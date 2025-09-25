#ifndef ROSE_Source_Analysis_Visitor_H
#define ROSE_Source_Analysis_Visitor_H

#include <RoseFirst.h>
#include <Rose/Source/Analysis/Analyzer.h>
#include <sageTraversal.h>

namespace Rose {
namespace Source {
namespace Analysis {

class Visitor : public Analyzer, private ROSE_VisitTraversal {
  protected:
    Visitor(std::string const & name, std::string const & version);
    int apply() final;
};

}
}
}

#endif /* ROSE_Source_Analysis_Visitor_H */
