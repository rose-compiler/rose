/*!
 * 
 * \file DLX/Core/compiler.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __DLX_COMPILER_HPP__
#define __DLX_COMPILER_HPP__

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

template <class language_tpl, class TransformationHandler_tpl>
class Compiler {
  public:
    typedef language_tpl language_t;
    typedef TransformationHandler_tpl TransformationHandler;

    typedef Directives::directive_t<language_t> directive_t;
    typedef std::vector<directive_t *> directives_ptr_set_t;

    typedef typename language_t::compiler_module_handlers_t module_handlers_t;

  protected:
    TransformationHandler & p_transformation_handler;

    module_handlers_t & p_module_handlers;

  public:
    Compiler(TransformationHandler & transformation_handler, module_handlers_t & module_handlers) :
      p_transformation_handler(transformation_handler),
      p_module_handlers(module_handlers)
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

#endif /* __DLX_COMPILER_HPP__ */

