/**
 *  \file Transform/Transform.hh
 *
 *  \brief Implements the outlining transformation.
 */

#if !defined(INC_LIAOUTLINER_TRANSFORM_HH)
#define INC_LIAOUTLINER_TRANSFORM_HH //!< Transform/Transform.hh included.

#include <string>

#include <Outliner.hh>
#include <VarSym.hh>

class SgGlobalScope;

namespace Outliner
{
  namespace Transform
  {
    /*!
     *  \brief Outlines the given basic block into a function named
     *  'name'.
     *
     *  This routine performs the outlining transformation, including
     *  insertion of the new outlined-function declaration and call.
     */
    Result outlineBlock (SgBasicBlock* b, const std::string& name);

    /*!
     *  \brief Computes the set of variables in 's' that need to be
     *  passed to the outlined routine.
     */
    void collectVars (const SgStatement* s, ASTtools::VarSymSet_t& syms);

    /*!
     *  \brief Returns a new outlined function containing a deep-copy
     *  of s.
     *
     *  This function only creates and returns an outlined function
     *  definition, f. Although it sets the scope of 'f' to be the
     *  first scope surrounding 's' that may contain a function (or
     *  member function) definition, it does not insert 'f' into that
     *  scope.
     *
     *  This function is an "inner" outlining routine which does not
     *  properly handle non-local control flow. To outline statements
     *  containing non-local flow, a caller should instead call \ref
     *  Outliner::outline(), which preprocesses non-local control
     *  flow appropriately. See \ref
     *  Outliner::transformNonLocalControlFlow() for more details.
     *
     *  \pre The statement does not contain non-local control flow.
     */
    SgFunctionDeclaration*
    generateFunction (const SgBasicBlock* s,
                      const std::string& func_name_str,
                      const ASTtools::VarSymSet_t& syms);

     //! Generate packing statements for the variables to be passed 
     //return the unique wrapper parameter for the outlined function
    std::string generatePackingStatements(SgStatement* target, ASTtools::VarSymSet_t & syms);

    /*!
     *  \brief Inserts an outlined-function declaration into global scope.
     *
     *  The caller specifies the global scope into which this routine will
     *  insert the declaration.
     *
     *  The caller also provides the original function from which a block
     *  was outlined (i.e., the "target" function). This routine uses this
     *  information to insert the prototypes into the correct places in
     *  the AST.
     */
    void insert (SgFunctionDeclaration* func,
                 SgGlobal* scope,
                 SgFunctionDeclaration* target_func);

    /*!
     *  \brief Generates a call to an outlined function.
     */
    SgStatement* generateCall (SgFunctionDeclaration* out_func, const ASTtools::VarSymSet_t& syms, std::string wrapper_arg_name,SgScopeStatement* scope);
  } /* namespace Transform */
} /* namespace Outliner */

#endif

// eof
