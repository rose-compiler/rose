#ifndef ROSE_SourceCode_Analysis_Visitor_H
#define ROSE_SourceCode_Analysis_Visitor_H

#include <RoseFirst.h>
#include <Rose/SourceCode/Analysis/Analyzer.h>
#include <sageTraversal.h>

namespace Rose {
namespace SourceCode {
namespace Analysis {

class Visitor : public Analyzer, private ROSE_VisitTraversal {
  protected:
    Visitor(std::string const & name, std::string const & version);
    int apply() final;
};

}
}
}

#endif /* ROSE_SourceCode_Analysis_Visitor_H */
