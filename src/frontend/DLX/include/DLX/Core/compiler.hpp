
#ifndef __DLX_COMPILER_HPP__
#define __DLX_COMPILER_HPP__

class SgNode;
class SgSymbol;

typedef SgNode * ast_fragment_t;

namespace DLX {

namespace Compiler {

template <class language_tpl, class TransformationHandler_tpl>
class Compiler {
  public:
    typedef language_tpl language_t;
    typedef TransformationHandler_tpl TransformationHandler;

    typedef typename language_t::compiler_module_handlers_t module_handlers_t;

  protected:
    TransformationHandler & p_transformation_handler;

    module_handlers_t & p_module_handlers;

  public:
    Compiler(TransformationHandler & transformation_handler, module_handlers_t & module_handlers);

    bool compile(SgNode *);
};

}

}

#endif /* __DLX_COMPILER_HPP__ */

