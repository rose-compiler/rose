/*!
 * 
 * \file DLX/Core/compiler.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __DLX_CORE_COMPILER_HPP__
#define __DLX_CORE_COMPILER_HPP__

#include "DLX/Core/directives.hpp"

/*!
 * \addtogroup grp_rose
 * @{
 */

class SgNode;
class SgSymbol;

typedef SgNode * ast_fragment_t;

/** @} */

namespace DLX {

namespace Compiler {

/*!
 * \addtogroup grp_dlx_core_compiler
 * @{
 */

template <class language_tpl, class compiler_modules_t>
class Compiler {
  public:
    typedef language_tpl language_t;

    typedef Directives::directive_t<language_t> directive_t;
    typedef std::vector<directive_t *> directives_ptr_set_t;

    compiler_modules_t & compiler_modules;

  public:
    Compiler(compiler_modules_t & compiler_modules_) :
      compiler_modules(compiler_modules_)
    {}

    /*!
     * \brief It applies the transformation associated with the directives.
     * \param directives the set of all the directive found
     * \param graph_entry the set of directives without predecessor (or parent)
     * \param graph_final the set of directives without successor (or children)
     * return true if the compilation is successful
     */
    bool compile(const directives_ptr_set_t & directives, const directives_ptr_set_t & graph_entry, const directives_ptr_set_t & graph_final);
};

/** @} */

}

}

#endif /* __DLX_COMPILER_CORE_HPP__ */

