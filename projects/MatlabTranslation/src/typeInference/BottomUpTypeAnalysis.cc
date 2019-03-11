#include "BottomUpTypeAnalysis.h"

#include <iostream> //TODO: delete it later

#include "FunctionAnalyzer.h"
#include "TypeAttribute.h"
#include "FastNumericsRoseSupport.h"
#include "utility/FunctionReturnAttribute.h"
#include "utility/utils.h"
#include "sageBuilder.h"
#include "sageGeneric.h"

namespace sb = SageBuilder;
namespace si = SageInterface;
namespace ru = RoseUtils;


//
// auxiliary functions

/// \brief  applies the function object fun to each pair in the ranges [first1, last1)
///         and [first2, first2 + (last1-first1) ).
/// \return a copy of the function object after it has completed the traversal.
template <class InputIterator, class InputIterator2, class BinaryFunction>
inline
BinaryFunction
zip(InputIterator aa1, InputIterator zz1, InputIterator2 aa2, BinaryFunction fun)
{
  while (aa1 != zz1)
  {
    fun(*aa1, *aa2);
    ++aa1; ++aa2;
  }

  return fun;
}

struct TypeCopier
{
  void operator()(SgInitializedName* n, SgType* t)
  {
    n->set_type(t);
  }
};

/**
 * Update the function delcaration's symbol table to set the input parameters
 * to the types passed in inputArguments.
 */
static
void
updateFunctionDeclarationInputParameters( SgFunctionDeclaration* functionDeclaration,
                                          Rose_STL_Container<SgType*> args
                                        )
{
  SgFunctionParameterList* parameters = functionDeclaration->get_parameterList();
  SgInitializedNamePtrList parms = parameters->get_args();

  ROSE_ASSERT(parms.size() >= args.size());
  zip(parms.begin(), parms.end(), args.begin(), TypeCopier());
}


/**
 * Returns a list of types of the input arguments in a function call
 * Ex. foo(1, 1.5, "asd") returns a list of <int, double, string>
 */
static
Rose_STL_Container<SgType*> getArgumentTypeList(SgFunctionCallExp* functionCall)
{
  Rose_STL_Container<SgType*> inputArguments;

  SgExprListExp*              argList = functionCall->get_args();

  // \todo regularize code to give every function call an argument list
  if (!argList)
  {
    return inputArguments;
  }

  SgExpressionPtrList         callArguments = argList->get_expressions();

  //Update the inputArguments vector with the arguments's type
  for ( SgExpressionPtrList::iterator argument = callArguments.begin();
        argument != callArguments.end();
        ++argument
      )
  {
    TypeAttribute* attachedTypeAttribute = TypeAttribute::get_typeAttribute(*argument);
    SgType*        currentArgumentType = NULL;

    if (attachedTypeAttribute != NULL)
    {
      currentArgumentType = attachedTypeAttribute->get_inferred_type();
      // std::cerr << "inf - " << currentArgumentType->unparseToString() << std::endl;
    }
    else
    {
      currentArgumentType = (*argument)->get_type();
      // std::cerr << "act - " << currentArgumentType->unparseToString() << std::endl;
    }

    inputArguments.push_back(currentArgumentType);
  }

  return inputArguments;
}


static inline
std::string
genCallString(std::string functionName, const Rose_STL_Container<SgType*>& args)
{
  std::string res = functionName + "(" + RoseUtils::getFunctionCallTypeString(args);

  // std::cerr << "Function call: " << res << std::endl;
  return res;
}


//
// auxiliary globals

typedef std::map<std::string, SgType*> FunctionReturnTypeMap;

/// A mapping from a function call string and its output
/// Eg: foo(int) -> double
static FunctionReturnTypeMap functionCallToReturnType;



namespace MatlabAnalysis
{
  //
  // Ctx static definitions

  NameToDeclarationMap                       Ctx::nameToFunctionDeclaration;
  NameToDeclarationMap                       Ctx::matlabBuiltins;

  /// given a function call site, finds the best overload
  /// based on number of arguments.
  /// A function is only a candidate, iff its number of arguments >=
  /// number of arguments at the call site.
  // \pp \todo double-check last condition
  struct ArityMatcher
  {
    typedef MatlabAnalysis::MatlabFunctionRec MatlabFunctionRec;

    static const size_t INF = 99;

    explicit
    ArityMatcher(size_t arity)
    : callSiteArity(arity), ambiguous(true), arityDiff(INF), rec(NULL)
    {}

    static
    size_t functionArity(SgFunctionDeclaration& decl)
    {
      SgFunctionParameterList&  parms = sg::deref(decl.get_parameterList());
      SgInitializedNamePtrList& arglst = parms.get_args();

      return arglst.size();
    }

    static
    size_t functionArity(MatlabFunctionRec& rec)
    {
      ROSE_ASSERT(rec.first);
      return functionArity(*rec.first);
    }

    void operator()(MatlabFunctionRec& cand)
    {
      size_t declArity = functionArity(cand);
      size_t declDiff  = INF;

      // std::cerr << "[" << declArity << std::endl;
      if (declArity >= callSiteArity)
      {
        declDiff = declArity - callSiteArity;
      }

      if (declDiff < arityDiff)
      {
        rec       = &cand;
        arityDiff = declDiff;
        ambiguous = false;
      }
      else if (declDiff == arityDiff)
      {
        ambiguous = true;
      }
    }

    void validate()
    {
      if (ambiguous)
      {
        std::cerr << callSiteArity << " " << arityDiff << std::endl;
        if (rec && rec->first)
        {
          std::cerr << rec->first->unparseToString() << std::endl;
        }
      }

      ROSE_ASSERT(!ambiguous);
    }

    operator MatlabFunctionRec&()
    {
      validate();
      return *rec;
    }

    operator SgFunctionDeclaration*()
    {
      validate();
      return rec->first;
    }

    const size_t       callSiteArity;
    bool               ambiguous;
    size_t             arityDiff;
    MatlabFunctionRec* rec;
  };


  //
  // type analysis functions

  struct BottomUpTypeAnalysis : AstBottomUpProcessing<TypeAttribute*>
  {
      BottomUpTypeAnalysis(SgProject* proj, FunctionAnalyzer* funAnalyzer)
      : project(proj), functionAnalyzer(funAnalyzer)
      {}

      virtual
      TypeAttribute*
      evaluateSynthesizedAttribute(SgNode*, SynthesizedAttributesList);

    private:
      SgType* symbolicExecuteFunction(SgFunctionCallExp* functionCall);

      void handleAssignOp(SgAssignOp *assignOp);

      /// Checks if a given function call expression is a Matrix access operation
      bool isMatrixAccess(SgFunctionCallExp *functionCall);

      /// A function call is recursive if the function to be called is the same
      /// as the enclosing function
      /// \todo cover mutually recursive functions
      bool isRecursive(SgFunctionCallExp *functionCall);

      // fields
      SgProject*        project;
      FunctionAnalyzer* functionAnalyzer;
  };

  void runBottomUpInference(SgExpression* op, SgProject* proj, FunctionAnalyzer* funan)
  {
    ROSE_ASSERT(op && proj && funan);

    BottomUpTypeAnalysis inferencer(proj, funan);

    inferencer.traverse(op);
  }

  static
  void dumpFunctionInfo(std::string msg, SgFunctionDeclaration* fn)
  {
    ROSE_ASSERT(fn);

    std::cerr << msg << '\n'
              << "fn = " << fn << '\n'
              << "  defdecl: " << fn->get_definingDeclaration() << '\n'
              << "  1stdecl: " << fn->get_firstNondefiningDeclaration() << '\n'
              << "  mangled: " << fn->get_mangled_name() << '\n'
              << std::flush;
  }

  static
  SgFunctionDeclaration* cloneFunction(SgFunctionDeclaration* function)
  {
    std::cerr << "Copying function "
              << function->get_name()
              << "/" << function->get_mangled_name()
              << std::endl;

    SgFunctionDeclaration*  copy        = si::deepCopy(function);
    SgExprListExp*          returnList  = FunctionReturnAttribute::getReturnList(function);

    if (returnList != NULL)
    {
      FunctionReturnAttribute* returnAttribute = new FunctionReturnAttribute(returnList);

      returnAttribute->attachTo(copy);
    }

    return copy;
  }


  static inline
  SgInitializedNamePtrList&
  functionArgumentList(SgFunctionParameterList* params)
  {
    ROSE_ASSERT(params);

    return params->get_args();
  }

  static inline
  SgInitializedNamePtrList&
  functionArgumentList(SgFunctionDeclaration* fundecl)
  {
    ROSE_ASSERT(fundecl);

    return functionArgumentList(fundecl->get_parameterList());
  }

  static inline
  SgInitializedNamePtrList&
  functionArgumentList(MatlabFunctionRec& rec)
  {
    return functionArgumentList(rec.first);
  }

  static inline
  void appendDummyTypesToArglist( const SgInitializedNamePtrList& parms,
                                  std::vector<SgType*>& argtypes
                                )
  {
    // \todo use real dummy type instead of int (needs inferencer support)
    for (size_t i = argtypes.size(); i < parms.size(); ++i)
    {
      argtypes.push_back(sb::buildIntType());
    }
  }


  /// reads entry and argument types; instantiates a new function if needed
  ///   needed is true unless it is a function that takes 0 arguments.
  static
  SgFunctionDeclaration*
  instantiateIfNeeded(MatlabFunctionRec& entry, const std::vector<SgType*>& argtypes)
  {
    SgFunctionDeclaration* prototype = entry.first;

    // if there are no arguments, instantiation cannot be performed
    //   so we use the original declaration.
    if (argtypes.size() == 0)
    {
      // prototype type can only be inferred once
      ROSE_ASSERT(!hasBeenAnalyzed(entry));

      // run type inferencer
      setAnalyzed(entry);
      dumpFunctionInfo("useproto", prototype);
      return prototype;
    }

    // Set that the function has been copied. This means we will delete the
    // original declaration from the AST
    SgFunctionDeclaration* funclone = cloneFunction(prototype);

    // need to upd params before we prepend, otherwise it may mess up
    //   defining and first_nondefining declaration links.
    updateFunctionDeclarationInputParameters(funclone, argtypes);
    si::prependStatement(funclone, si::getGlobalScope(prototype));

    // Update the list of cloned declarations
    //~ Ctx::clonedDeclarations.push_back(funclone);

    dumpFunctionInfo("prototype", prototype);
    dumpFunctionInfo("cloned", funclone);
    return funclone;
  }


  /// finds a built in function with a given name and number of arguments.
  static
  MatlabFunctionRec&
  matlabBuiltInFunction(std::string functionName, const std::vector<SgType*>& argtypes)
  {
    // try matlab built in functions
    NameToDeclarationMap::iterator mbiPos = Ctx::matlabBuiltins.find(functionName);

    if (mbiPos == Ctx::matlabBuiltins.end())
    {
      std::cerr << "Function not found/Matlab builtin? " << functionName << " " << Ctx::matlabBuiltins.size() << std::endl;
      ROSE_ASSERT(false);
    }

    MatlabOverloadSet& overloads = (*mbiPos).second;
    MatlabFunctionRec& declEntry = std::for_each( overloads.begin(),
                                                  overloads.end(),
                                                  ArityMatcher(argtypes.size())
                                                );

    return declEntry;
  }


  /// returns a modifiable function declaration.
  /// if |argtypes| > 0 return specialized instance; otherwise return the prototype
  /// note, prototype and specialized instance need different argument pairs, in order
  /// to distinguish C++ overloads correctly (may mess up defining and nondefining
  /// declaration links).
  static
  MatlabFunctionRec&
  matlabFunction(std::string functionName, const std::vector<SgType*>& argtypes)
  {
    // First, try user defined functions
    NameToDeclarationMap::iterator udfPos = Ctx::nameToFunctionDeclaration.find(functionName);

    if (udfPos == Ctx::nameToFunctionDeclaration.end())
    {
      // not user defined; maybe a Matlab builtin?
      return matlabBuiltInFunction(functionName, argtypes);
    }

    // User defined function
    MatlabOverloadSet& overloads = (*udfPos).second;

    ROSE_ASSERT(overloads.size() == 1); // \pp \todo choose using arity
    return overloads.back();
  }

  bool BottomUpTypeAnalysis::isMatrixAccess(SgFunctionCallExp* functionCall)
  {
    SgVarRefExp* functionVarRef = isSgVarRefExp(functionCall->get_function());
    ROSE_ASSERT(functionVarRef && functionVarRef->get_type());

    return (functionVarRef->get_type()->variantT() == V_SgTypeMatrix);
  }


  //A function call is recursive if the function to be called is the same as the enclosing function
  bool BottomUpTypeAnalysis::isRecursive(SgFunctionCallExp *functionCall)
  {
    ROSE_ASSERT(functionCall);

    SgVarRefExp*           functionVarRef = isSgVarRefExp(functionCall->get_function());
    ROSE_ASSERT(functionVarRef);

    SgName                 functionCalled = ru::nameOf(functionVarRef);
    SgFunctionDeclaration* enclosingFunction = sg::ancestor<SgFunctionDeclaration>(functionCall);

    return (sg::deref(enclosingFunction).get_name() == functionCalled);
  }


  TypeAttribute*
  BottomUpTypeAnalysis::evaluateSynthesizedAttribute(SgNode* node, SynthesizedAttributesList childAttributes)
  {
    if (isSgValueExp(node))
    {
      SgExpression*  valueExpression = isSgExpression(node);
      TypeAttribute* typeAttribute = TypeAttribute::buildAttribute(valueExpression->get_type());

      typeAttribute->attach_to(valueExpression);
      return typeAttribute;
    }

    if (SgMatrixExp* matrixExp = isSgMatrixExp(node))
    {
      SgType*       innerType = FastNumericsRoseSupport::get_dominant_type(childAttributes);
      SgTypeMatrix* matrixType = isSgTypeMatrix(matrixExp->get_type());

      matrixType->set_base_type(innerType);

      TypeAttribute* typeAttribute = TypeAttribute::buildAttribute(matrixType);

      typeAttribute->attach_to(matrixExp);
      return typeAttribute;
    }

    if (SgRangeExp* rangeExp = isSgRangeExp(node))
    {
      SgType*       innerType = FastNumericsRoseSupport::get_dominant_type(childAttributes);
      SgTypeMatrix* matrixType = SageBuilder::buildMatrixType();

      matrixType->set_base_type(innerType);

      TypeAttribute* typeAttribute = TypeAttribute::buildAttribute(matrixType);

      typeAttribute->attach_to(rangeExp);
      return typeAttribute;
    }

    if (SgVarRefExp* varref = isSgVarRefExp(node))
    {
      // if the node is a varRefExp, either it has already been assigned a type
      //   or it may be untyped.
      // if it has been assigned a type, send its type upward to the parent.

      SgType* vartype = varref->get_type();
      ROSE_ASSERT(vartype);

      if (vartype->variantT() == V_SgTypeUnknown)
      {
        // \todo \pp quick fix for missing information from type inferencer
        //           --> we just assume that we got ints :*)
        vartype = sb::buildIntType();
      }

      //Return the type that was deduced from earlier assignments
      TypeAttribute *typeAttribute = TypeAttribute::buildAttribute(vartype);

      typeAttribute->attach_to(node);
      return typeAttribute;
    }


    if (SgFunctionCallExp *functionCall = isSgFunctionCallExp(node))
    {
      //std::cout << functionCall->unparseToString() << std::endl;

      /*
        A = [1 2 3];
        y = A(2); Here A(2) is stored in AST as a function call but it is a Matrix element access.
      */
      if(isMatrixAccess(functionCall))
      {
        //If one of the arguments is either a COLON or a list of numbers, then the output should be a matrix
        //Else it should be the base type of the matrix

        //for now let us treat it as purely element access
        SgType *innerType = isSgTypeMatrix(functionCall->get_function()->get_type())->get_base_type();

        TypeAttribute *typeAttribute = TypeAttribute::buildAttribute(innerType);
        typeAttribute->attach_to(node);

        return typeAttribute;
      }

      if (isRecursive(functionCall))
      {
        ROSE_ASSERT(false); // \pp \todo added assert false; not sure what else to do...
        // \pp return XYZ is missing...
      }

      // For a function call, we have to pass the types of the arguments
      // the types of arguments should be already known by now
      // Update the called function's symbol table so that the parameters will now have types
      // then recursively do the analysis on the function
      SgType* returnType = symbolicExecuteFunction(functionCall);
      TypeAttribute* typeAttribute = TypeAttribute::buildAttribute(returnType);

      typeAttribute->attach_to(node);
      return typeAttribute;
    }

    if (SgAssignOp *assignOp = isSgAssignOp(node))
    {
      handleAssignOp(assignOp);

      // \pp added return statement (was missing).
      // \todo can Matlab AssignOps be chanined?
      return TypeAttribute::buildAttribute(sb::buildVoidType());
    }

    // anything else

    /*
     * For other expressions with multiple children
     * e.g., A SgMatrixExp that has many children
     *
     * Get the dominant type from the children, that could be assigned to the
     * current expression.
     * e.g., a matrixRow in the could have doubles and ints: the dominant type
     *       would be double, hence the row would be of double type
     */
    SgType*        dominantType = FastNumericsRoseSupport::get_dominant_type(childAttributes);

    if (dominantType == NULL)
    {
      // \pp \todo quick fix, if type inference fails..
      std::cerr << "OOPS: " << node->unparseToString() << std::endl;

      dominantType = sb::buildIntType();
    }

    TypeAttribute* dominantTypeAttribute = TypeAttribute::buildAttribute(dominantType);

    dominantTypeAttribute->attach_to(node);
    return dominantTypeAttribute;
  }


  SgType*
  BottomUpTypeAnalysis::symbolicExecuteFunction(SgFunctionCallExp* functionCall)
  {
    // For now we have stored the SgFunctionCallExp to have a SgVarRefExp to store function name
    SgVarRefExp*                functionVarRef = isSgVarRefExp(functionCall->get_function());
    ROSE_ASSERT(functionVarRef);

    std::string                 fnname         = ru::nameOf(functionVarRef);
    Rose_STL_Container<SgType*> callArgs       = getArgumentTypeList(functionCall);
    MatlabFunctionRec&          matlabFn       = matlabFunction(fnname, callArgs);

    // fill up arguments with dummy types
    appendDummyTypesToArglist(functionArgumentList(matlabFn), callArgs);

    // Stores string representation of function call:
    //   e.g., foo(int, int) -> "foo(int,int,"
    const std::string           callString     = genCallString(fnname, callArgs);

    // Check if we have already computed the return type of this function call
    FunctionReturnTypeMap::iterator frtpos     = functionCallToReturnType.find(callString);
    if (frtpos != functionCallToReturnType.end())
    {
      return frtpos->second;
    }

    // create a clone, unless the argument list is empty
    SgFunctionDeclaration*      thisFunDecl    = instantiateIfNeeded(matlabFn, callArgs);

    // \pp do not we need to insert the function declaration into
    //     functionCallToReturnType for handling recursive calls?

    // Now lets analyze the called function recursively
    functionAnalyzer->analyse_function(thisFunDecl);

    // Get the return type of the called function executed with the current inputArguments
    SgFunctionType*             funtype        = thisFunDecl->get_type();
    SgType*                     returnType     = sg::deref(funtype).get_return_type();

    // Update the functionCallString to Type map
    functionCallToReturnType.insert(std::make_pair(callString, returnType));

    // \pp \todo replace call target with new function
    return returnType;
  }

  /// Assigns the type of RHS to LHS.
  /// Also sets the typeof AssignOp expression to be RHS type.
  void BottomUpTypeAnalysis::handleAssignOp(SgAssignOp* assignOp)
  {
    SgExpression*  lhs = assignOp->get_lhs_operand();
    SgExpression*  rhs = assignOp->get_rhs_operand();
    TypeAttribute* rhsTypeAttribute = TypeAttribute::get_typeAttribute(rhs);

    // Make sure that the rhs has a type assigned. Else it is a type error
    ROSE_ASSERT(rhsTypeAttribute != NULL);

    rhsTypeAttribute->attach_to(assignOp);

    if (SgVarRefExp* lhsVarRef = isSgVarRefExp(lhs))
    {
      // LHS may already have a type, so update it with the dominant type of the new type and its existing type
      FastNumericsRoseSupport::updateVarRefTypeWithDominantType(lhsVarRef, rhsTypeAttribute);
    }
    else
    {
      SgExprListExp* varList = isSgExprListExp(lhs);
      ROSE_ASSERT(varList);

      //handle cases like [x,y] = functionCall(...)
      //The type should be a tuple type and each variable is assigned a corresponding type

      rhsTypeAttribute->attach_to(varList);

      SgTypeTuple*   typeTuple = isSgTypeTuple(rhsTypeAttribute->get_inferred_type());
      SgTypePtrList  typeList  = typeTuple->get_types();

      ROSE_ASSERT(typeTuple != NULL);

      int                 i = 0;
      SgExpressionPtrList varPtrList = varList->get_expressions();

      for (SgExpressionPtrList::iterator it = varPtrList.begin(); it != varPtrList.end(); ++it)
      {
        SgVarRefExp*   varRef = isSgVarRefExp(*it);
        SgType*        type = typeList[i];
        TypeAttribute* typeAttribute = TypeAttribute::buildAttribute(type);
        //typeAttribute->attach_to(varRef);

        ++i;
        FastNumericsRoseSupport::updateVarRefTypeWithDominantType(varRef, typeAttribute);
      }
    }
  }
}
