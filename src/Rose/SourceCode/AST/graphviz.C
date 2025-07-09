
#include "sage3basic.h"

#include "Rose/SourceCode/AST/cmdline.h"

#include "wholeAST_API.h"

namespace Rose { namespace SourceCode { namespace AST { namespace cmdline {

__when_T<graphviz_t> graphviz;

void graphviz_t::exec(SgProject * project) const {
  if (!on) return;
  std::string fname = out;
  switch (mode) {
    case Mode::tree: {
      if (fname.size() == 0) fname = "rose-ast-tree.dot";
      AstDOTGeneration astdotgen;
      astdotgen.generate(project, fname);
      break;
    }
    case Mode::graph: {
      if (fname.size() == 0) fname = "rose-ast-graph.dot";
      generateWholeGraphOfAST(fname); // TODO filter from argv -rose:dotgraph:...
      break;
    }
    default:
      ROSE_ABORT();
  }
}

graphviz_t::Mode graphviz_t::__mode(std::string const & str) {
  if (str == "tree") {
    return Mode::tree;
  } else if (str == "graph") {
    return Mode::graph;
  } else {
    return Mode::unknown;
  }
}

} } } }

