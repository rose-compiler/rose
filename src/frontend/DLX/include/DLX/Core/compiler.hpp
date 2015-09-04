/*!
 * 
 * \file DLX/Core/compiler.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __DLX_CORE_COMPILER_HPP__
#define __DLX_CORE_COMPILER_HPP__

#include "DLX/Core/frontend.hpp"

#ifndef OUTPUT_DIRECTIVES_GRAPHVIZ
#define OUTPUT_DIRECTIVES_GRAPHVIZ 1
#endif
#if OUTPUT_DIRECTIVES_GRAPHVIZ
#include <iostream>
#include <fstream>
#include <sstream>
#endif

class SgNode;

namespace DLX {

template <class language_tpl>
class Compiler {
  protected:
    Frontend::Frontend<language_tpl> frontend;

  public:
    Compiler() : frontend() {
      language_tpl::init();
    }

    bool parse(SgNode * node) {
      bool res = frontend.parseDirectives(node);
#if OUTPUT_DIRECTIVES_GRAPHVIZ
      std::ostringstream oss; oss << "directives_" << node << ".dot";
      std::ofstream out(oss.str().c_str(), std::ofstream::out);
      frontend.toGraphViz(out);
      out.close();
#endif
      return res;
    }
};

}

#endif /* __DLX_CORE_COMPILER_HPP__ */

