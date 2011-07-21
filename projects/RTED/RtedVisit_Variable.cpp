// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
#include "rose.h"

// DQ (2/9/2010): Testing use of ROSE to compile ROSE.
#ifndef USE_ROSE

#include <algorithm>
#include <functional>
#include <numeric>
#include <string>

#include "sageGeneric.hpp"

#include "RtedSymbols.h"
#include "DataStructures.h"
#include "RtedTransformation.h"

#include "RtedVisit.h"

namespace rted
{
  VariableTraversal::VariableTraversal(RtedTransformation* t)
  : Base(), transf(t)
  {
    ROSE_ASSERT(transf != NULL);
  }

  static
  bool isInitializedNameInForStatement(SgStatement* lastGForLoop, const SgInitializedName& name)
  {
     if (!lastGForLoop) return false;

     SgForInitStatement* inits = GeneralizdFor::initializer(lastGForLoop);

     // Capture for( int i = 0;
     SgNodePtrList        initialized_names = NodeQuery::querySubTree(inits, V_SgInitializedName);
     // Capture int i; for( i = 0;
     const SgNodePtrList& init_var_refs = NodeQuery::querySubTree(inits, V_SgVarRefExp);
     for (std::vector<SgNode*>::const_iterator i = init_var_refs.begin(); i != init_var_refs.end(); ++i)
        initialized_names.push_back(isSgVarRefExp(*i) -> get_symbol() -> get_declaration());

     return (find(initialized_names.begin(), initialized_names.end(), &name) != initialized_names.end());
  }

  /// \brief   tests whether astNode directly or indirectly is on the right hand
  ///          side of a binary expressions.
  /// \details indirectly means that if astNode is a child of a non-binary
  ///          expression or a dot-expression, isRightOfBinaryOp is invoked
  ///          'recursively' (with the parent node) as the new astNode.
  static
  bool isRightOfBinaryOp(const SgExpression* expr)
  {
     const SgNode* temp = expr;
     while (!isSgProject(temp)) {
        if ( temp->get_parent()
           && isSgBinaryOp(temp->get_parent())
           && !(  isSgDotExp(temp->get_parent())
               || isSgPointerDerefExp(temp->get_parent())
               )
           )
        {
           if (isSgBinaryOp(temp->get_parent())->get_rhs_operand() == temp) {
              return true;
           } else
              break;
        }
        temp = temp->get_parent();
     }
     return false;
  }

  static
  bool isUsableAsSgPointerType( SgType* type ) {
      return isSgPointerType( skip_ReferencesAndTypedefs( type ));
  }

  namespace
  {
    struct VarTypeHandler
    {
      RtedTransformation& transf;
      SgInitializedName&  initname;

      VarTypeHandler(RtedTransformation& rtedobj, SgInitializedName& iname)
      : transf(rtedobj), initname(iname)
      {}

      void handle(SgNode&) { assert(false); }

      // regular variable declaration
      void handle(SgType&)
      {
        transf.variable_declarations.push_back(&initname);
      }

      // nothing to be done for references
      void handle(SgReferenceType&) {}

      // handle arrays
      void handle(SgArrayType& arrtype)
      {
        // \pp \note the Nov'10 RTED code would skip this when we get a modifier-type
        //     something like: if (&n != initname.get_type()) return;
        RtedArray arrayRted(&initname, getSurroundingStatement(&initname), varAllocKind(initname));

        transf.populateDimensions( arrayRted, initname, arrtype );
        transf.create_array_define_varRef_multiArray_stack[&initname] = arrayRted;
      }
    };
  }

  /// \brief tests if the first argument is an ancestor of the second argument
  /// \note  see also SageInterface::isAncestor (which is defined over arbitrary
  ///        nodes).
  bool isAncestorOf(const SgExpression& ancestor, const SgExpression& expr)
  {
    const SgExpression* parent = isSgExpression(expr.get_parent());

    return (  (parent != NULL)
           && ( (&ancestor == parent) || isAncestorOf(ancestor, *parent) )
           );
  }

  /// tests if the second argument is (or is part of) the upc_forall loop's
  /// affinity expression.
  static
  bool partOfUpcForallAffinityExpr(const SgUpcForAllStatement& upcForall, const SgExpression& expr)
  {
    const SgExpression* affinity = upcForall.get_affinity();

    ROSE_ASSERT(affinity != NULL);
    return (affinity == &expr || isAncestorOf(*affinity, expr));
  }

  /// tests if the first argument is an upc_forall loop AND if the second
  /// argument occurs in the context of the former's affinity expression.
  static
  bool partOfUpcForallAffinityExpr(const SgStatement* gforLoop, const SgExpression& expr)
  {
    const SgUpcForAllStatement* upcForall = isSgUpcForAllStatement( gforLoop );

    return (upcForall != NULL && partOfUpcForallAffinityExpr(*upcForall, expr));
  }

  /// \brief   defines patterns of binary operations that should not be tested
  /// \note    the for loop test needed to be integrated into the binary test
  ///          b/c it is an alternative if varref is not in a binary context.
  /// \return  true, if an access guard should be skipped
  ///          false, otherwise
  static
  bool test_binaryop_and_forloop( SgVarRefExp& varref,
                                  const SgInitializedName& varname,
                                  const InheritedAttribute& inh
                                )
  {
    // \pp this function seems to be to broad
    //     For starters, I do not understand why we want to skip the test to
    //     check whether ptr in ptr+3 is initialized (assuming ptr is of pointer type)?
    if ( rted::partOfUpcForallAffinityExpr(inh.lastGForLoop, varref) ) return true;

    // not in binary context
    if (!inh.isBinaryOp)
    {
      return rted::isInitializedNameInForStatement(inh.lastGForLoop, varname);
    }

    // do the real checks
    const SgBinaryOp*   binop = inh.lastBinary;
    ROSE_ASSERT(binop);

    const SgExpression* rhs = binop->get_rhs_operand();

    // \note isRightOfBinaryOp(&varref)
    //       does not mean that varref == rhs. varref could also be
    //       an (indirect) right side child of binop.
    // \pp not sure why the side on which varref occurs matters,
    //     when we, for example, have a commutative operation?
    return (  !isRightOfBinaryOp(&varref)
           || isSgArrayType(rhs->get_type())
           || isSgNewExp(rhs)
           || isSgReferenceType(binop->get_lhs_operand()->get_type())
           );
  }



  /// \brief   tests whether a varref is related to an assign initializer
  static
  bool test_assign_initializer(const InheritedAttribute& inh, const SgVarRefExp& varref)
  {
     if (!inh.isAssignInitializer) return false;

     SgInitializedName* initName = isSgInitializedName(varref.get_parent() ->get_parent()-> get_parent());
     if (initName == NULL)
        initName = isSgInitializedName(varref.get_parent() ->get_parent());

     return (initName && isSgReferenceType(initName -> get_type()));
  }

  /// wraps ::isUsableAsSgArrayType and makes sure that type is
  /// not null.
  static
  bool isUsableAsSgArrayTypeChecked(SgType* type)
  {
    return (type != NULL) && ::isUsableAsSgArrayType(type);
  }

  /// wraps ::isUsableAsSgReferenceType and makes sure that
  /// type is not null.
  static
  bool isUsableAsSgReferenceTypeChecked(SgType* type)
  {
    return (type != NULL) && ::isUsableAsSgReferenceType(type);
  }

  /// \brief   tests whether a varref is related to a function call
  /// \note    do we assume that the function has a declaration?
  /// \todo    \pp Can't we take the type from the call expression
  ///          (in case that the callee is computed)?
  static
  bool test_call_argument(RtedTransformation* transf, const SgVarRefExp& varref)
  {
    // this does not work if there are for example casts in between :(
    const SgExprListExp* exprl = isSgExprListExp(varref.get_parent());
    if (exprl == NULL) return false;

    const SgFunctionCallExp* callexp = isSgFunctionCallExp(exprl->get_parent());
    if (callexp == NULL) return false;

    // \note do not move up the isUsableAsSgArrayType test
    //       b/c we only want it to succeed if this is indeed a function call expr
    if (isUsableAsSgArrayTypeChecked(varref.get_type())) return true;

    // \pp \todo
    // Instead of getting the type form the fundecl, it might be better to get
    // the type from the callee expression. This way we would always know the
    // argument types.

    // try to determine the parameter type
    const SgFunctionDeclaration*   fndecl = callexp->getAssociatedFunctionDeclaration();
    if (fndecl == NULL) return false; // \pp not sure if this is correct

    const SgExpressionPtrList&     args = exprl->get_expressions();
    size_t                         arg_pos = 0;

    ROSE_ASSERT(args.size() > 0); // at least varref is in args

    while (args[arg_pos] != &varref)
    {
      ++arg_pos;
      ROSE_ASSERT( arg_pos < args.size() );
    }

    const SgInitializedNamePtrList& param_lst = fndecl->get_parameterList()->get_args();

    // \note param_lst.size() could be 0, if the compiler generated a function
    //       prototype (i.e., there was no prototype present in the code.
    return (  arg_pos < param_lst.size()
           && (param_lst[arg_pos] != NULL)
           && isUsableAsSgReferenceTypeChecked(param_lst[arg_pos]->get_type())
           );
  }


  /// wraps RtedTransformation::isInInstrumentedFile and makes sure that
  /// the argument is not null.
  static
  bool in_instrumented_file(RtedTransformation* transf, SgInitializedName* name)
  {
    return (name != NULL) && !transf->isInInstrumentedFile(name -> get_declaration());
  }

  struct InheritedAttributeHandler
  {
    VariableTraversal& vt;
    InheritedAttribute ia;

    InheritedAttributeHandler(VariableTraversal& trav, const InheritedAttribute& inh)
    : vt(trav), ia(inh)
    {}

    void storeUpcBlockingOp(SgStatement& n)
    {
      vt.transf->upcBlockingOps.push_back(&n);
    }

    void handle(SgNode&) {}

    void handle(SgSourceFile& n)
    {
      const bool first = vt.transf->srcfiles.empty();

      ROSE_ASSERT(first || vt.transf->srcfiles.back() != &n);

      if (first) vt.transf->loadFunctionSymbols(n);
      vt.transf->srcfiles.push_back(&n);
    }

    void handle(SgFunctionDefinition& n)
    {
        vt.transf->transformIfMain(n);
        vt.transf->function_definitions.push_back(&n);

        ia.function = true;
        // do not handle as SgScopeStatement
    }

    void handle(SgInitializedName& n)
    {
      // something like: struct type { int before; char c[ 10 ]; int after; }
      // does not need a createarray call, as the type is registered and any array
      // information will be tracked when variables of that type are created.
      // ignore arrays in parameter lists as they're actually pointers, not stack arrays
      if (isStructMember(n) || isFunctionParameter(n)) return;

      // \pp should we also skip typedefs?
      SgType* unmodType = skip_ModifierType(n.get_type());

      sg::dispatch(VarTypeHandler(*vt.transf, n), unmodType);
    }

    void handle(SgAssignInitializer& n)
    {
        vt.transf->visit_isAssignInitializer(&n);
        ia.isAssignInitializer = true;
    }

    void handle(SgReturnStmt& n)
    {
      if (!n.get_expression()) return;

      vt.transf->returnstmt.push_back(&n);
    }

    void handle(SgUpcBarrierStatement& n) { storeUpcBlockingOp(n); }

    void handle(SgUpcWaitStatement& n)
    {
      storeUpcBlockingOp(n);
    }

    // implicitly casts to SgScopeStatement
    void store_scope(SgScopeStatement& n) { vt.transf->scopes[&n] = &n; }

    void handle(SgClassDefinition& n)
    {
      vt.transf->visit_isClassDefinition(&n);
    }

    void handle_gfor(SgScopeStatement& sgfor)
    {
      store_scope(sgfor);

      ia.lastGForLoop = &sgfor;
      ia.isForStatement = true;
    }

    void handle(SgForStatement& n)
    {
      handle_gfor(n);
    }

    void handle(SgUpcForAllStatement& n)
    {
      handle_gfor(n);
    }

    void handle(SgWhileStmt& n)       { store_scope(n); }
    void handle(SgDoWhileStmt& n)     { store_scope(n); }
    void handle(SgSwitchStatement& n) { store_scope(n); }
    void handle(SgIfStmt& n)          { store_scope(n); }

    void handle(SgBasicBlock& n)
    {
      if (isSgBasicBlock(n.get_parent())) store_scope(n);
    }

     //
     // Expressions
     //

     /// Visit pointer assignments whose lhs is computed from the original value of
     /// the pointer by virtue of the operator alone (e.g. ++, --)  As a heuristic,
     /// we say that such operations should not change the @e "Memory Chunk", i.e.
     /// the array the pointer refers to.
     void push_if_ptr_movement(SgExpression& astNode, SgExpression* operand)
     {
        if( isUsableAsSgPointerType( operand -> get_type() )) {
           // we don't care about int++, only pointers, or reference to pointers.
           vt.transf->pointer_movements.push_back( &astNode );
        }
     }

     void handle(SgVarRefExp& n)
     {
       SgInitializedName* name = n.get_symbol()->get_declaration();
       bool elide_access_guard = (  (ia.isArrowExp                          )
                                 || (ia.isAddressOfOp                       )
                                 || (!ia.function                           )
                                 || (in_instrumented_file(vt.transf, name)  )
                                 || (test_binaryop_and_forloop(n, *name, ia))
                                 || (test_assign_initializer(ia, n)         )
                                 || (test_call_argument(vt.transf, n)       )
                              );

    if (elide_access_guard)
    {
      // std::cerr << "### ELIDE " << where << " " << varref->unparseToString() << std::endl;
      return;
    }

    // its a plain variable access
       vt.transf->variable_access_varref.push_back(&n);
     }

     // unary

     void handle(SgAddressOfOp& n)
     {
       ia.isAddressOfOp = true;
       /* returns immediately */
     }

     void handle(SgPointerDerefExp& n)
     {
        // if this is a varrefexp and it is not initialized, we flag it.
        // do only if it is by itself or on right hand side of assignment
        vt.transf->visit_isSgPointerDerefExp(&n);
     }


     // binary

     void handle(SgBinaryOp& n)
     {
        if (ia.isArrowExp || ia.isAddressOfOp) return;

        ia.lastBinary = &n;
        ia.isBinaryOp = true;
     }

     void handle_binary(SgBinaryOp& n) { handle(n); }  // implcitely casts to SgBinaryOp

     void handle(SgDotExp&) { /* skip binary handling */ }

     void handle(SgPntrArrRefExp& n)
     {
       vt.transf->visit_isArrayPntrArrRefExp(&n);
       handle_binary(n);
     }

     void handle(SgArrowExp& n)
     {
       vt.transf->visit_isSgArrowExp(&n);
       ia.isArrowExp = true;
       /* skip binary handling */
     }

     void handle(SgAssignOp& n)
     {
        // 1. look for MALLOC
        // 2. Look for assignments to variables - i.e. a variable is initialized
        // 3. Assign variables that come from assign initializers (not just assignments
        std::cerr << n.unparseToString() << std::endl;
        vt.transf->visit_isArraySgAssignOp(&n);
        handle_binary(n);
     }

     void handle(SgMinusAssignOp& n)
     {
       push_if_ptr_movement(n, n.get_lhs_operand());
       handle_binary(n);
     }

     void handle(SgPlusAssignOp& n)
     {
       push_if_ptr_movement(n, n.get_lhs_operand());
       handle_binary(n);
     }

     // n-ary

     // \note see note comment in handle(SgFunctionCallExp)
     bool suppress_binary_for_specific_calls(const SgFunctionDeclaration* fndecl)
     {
       return ((fndecl != NULL) && isLibFunctionRequiringArgCheck(fndecl->get_name()));
     }

     void handle(SgFunctionCallExp& n)
     {
        vt.transf->visit_isFunctionCall(&n);

        // \pp binary should be suppressed for all calls
        //     in order not to impact existing RTED tests, we suppress for
        //     cases relevant to UPC
        // \todo 1) make suppression unconditional (remove if)
        // \todo 2) I think the following code is obsolete
        if (suppress_binary_for_specific_calls(n.getAssociatedFunctionDeclaration()))
        {
          ia.isBinaryOp = false;
        }
     }

     void handle(SgPlusPlusOp& n)
     {
       push_if_ptr_movement(n, n.get_operand());
     }

     void handle(SgMinusMinusOp& n)
     {
       push_if_ptr_movement(n, n.get_operand());
     }

     void handle(SgDeleteExp& del)
     {
        typedef RtedTransformation::Deallocations Deallocations;

        const AllocKind allocKind = (del.get_is_array() ? akCxxArrayNew : akCxxNew);

        vt.transf->frees.push_back( Deallocations::value_type(&del, allocKind) );
     }

     operator InheritedAttribute()
     {
       return ia;
     }
  };

  InheritedAttribute VariableTraversal::evaluateInheritedAttribute(SgNode* astNode, InheritedAttribute inheritedAttribute)
  {
    return sg::dispatch(InheritedAttributeHandler(*this, inheritedAttribute), astNode);
  }
}

#if INCOMPLETE_CODE
  >>> imported from RtedTransforamtion.h
   typedef std::vector< std::pair<const SgSourceFile*, SgPointerDerefExp*> > PtrDerefContainer;

   PtrDerefContainer    ptrderefs;
   // \todo add to ctor member initializer list

   typedef std::pair<SgPointerType*, std::string>                            SafeFunctionKey;
   typedef std::map< SafeFunctionKey, SgFunctionSymbol* >                    SafeFunctionContainer;
   typedef std::map< const SgSourceFile*, SafeFunctionContainer >            GeneratedFunctions;
  <<< imported from RtedTransforamtion.h

  struct SafeEval
  {
    static const char* const read  = "r";
    static const char* const write = "w";

    static const char* const deref   = "Deref"
    static const char* const none    = "None"
    static const char* const assign  = "Assign"

    SynthesizedAttribute             res;
    VariableTraversal&               vt;
    const SynthesizedAttributesList& subeval;

    SafeEval(VariableTraversal& trav, const SynthesizedAttributesList& childattr)
    : res(NULL, none), vt(trav), subeval(childattr)
    {}

    void handle_subexpr(SgUnaryOp& n, std::string mode)
    {
      ROSE_ASSERT(subeval.size() == 1);

      const SynthesizedAttribute& attr = subeval.front();
      SgExpression&               operand = *n.get_operand();

      attr.modify(*vt.transf, operand, vt.get_safeFunctionSym(attr.operation, mode));
    }

    void handle(SgNode&) {}

    void handle(SgPointerDerefExp& n)
    {
      handle_sub(n, read);

      res.operation = deref;
      res.handler = RtedTransformation::replaceNode;
    }

    void handle(SgAssignOp& n)
    {
      handle_subexpr(n, read, write);

      if (subeval.front().operation == none) return;

      res.operand = assign;
      res.handler = RtedTransformation::replaceNode;
    }
  }
#endif /* INCOMPLETE_CODE */


#endif
