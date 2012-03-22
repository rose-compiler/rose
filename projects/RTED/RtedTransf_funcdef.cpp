#include <rose.h>

// DQ (2/9/2010): Testing use of ROE to compile ROSE.
#ifndef USE_ROSE

#include <string>
#include <boost/foreach.hpp>
#include "RtedSymbols.h"
#include "DataStructures.h"
#include "RtedTransformation.h"

namespace SI = SageInterface;
namespace SB = SageBuilder;

void
RtedTransformation::insertVariableCreateInitForParams( SgFunctionDefinition& fndef)
{
    SgBasicBlock* body = fndef.get_body();
    ROSE_ASSERT( body);

    SgInitializedNamePtrList names = fndef.get_declaration()->get_parameterList()->get_args();

    BOOST_FOREACH( SgInitializedName* param, names)
    {
      SgType* initType = param->get_type();

      // nov2010 code:
      // reference variables don't allocate new memory
      // if we call createVariable the RTS will think it's a double
      // allocation fault

      // \pp we skip array types because they will be handled elsewhere
      // \todo not sure if this is correct here, b/c arrays would decay
      //       to pointers anyway. However, this decay is not represented
      //       in the nov2010 code, thus we skip these initializations
      //       here.
      if ( isSgReferenceType(initType) || isSgArrayType(skip_ModifierType(initType)) )
        continue;

      SgFunctionDeclaration* fndecl = fndef.get_declaration();
      std::cerr << ">>> " << fndecl->get_name() << std::endl;
      ROSE_ASSERT(isSgFunctionDefinition(param->get_scope()));

      body->prepend_statement( buildVariableCreateCallStmt(param, true) );
    }
}

void
RtedTransformation::appendSignature( SgExprListExp* arg_list, SgType* return_type, const SgTypePtrList& param_types)
{
  // number of type descriptors (arguments + return type)
  SI::appendExpression( arg_list, SB::buildIntVal( param_types.size() + 1));

  // generate a list of typedesc aggregate initializers representing the
  //   function signature.
  SgExprListExp* type_list = SB::buildExprListExp();

  SI::appendExpression( type_list, mkTypeInformation(return_type, true, true) );

  BOOST_FOREACH( SgType* p_type, param_types ) {
    SI::appendExpression( type_list, mkTypeInformation( p_type, true, true ) );
  }

  // \pp \note probably roseTypeDesc below should really be an array of roseTypeDescs
  SI::appendExpression( arg_list, ctorTypeDescList(genAggregateInitializer(type_list, roseTypeDesc())) );
}

static
void insertSignatureCheck(SgFunctionDefinition& fndef, RtedTransformation& trans)
{
  SgFunctionDeclaration* fndecl = fndef.get_declaration();

  // \pp \todo replace with !is_C_linkage
  if (isSgMemberFunctionDeclaration(fndecl))
    return;

  SgExprListExp*         arg_list = SB::buildExprListExp();

  // first arg is the name
  // \todo
  // FIXME 2: This probably needs to be something closer to the mangled_name,
  // or perhaps we can simply skip the check entirely for C++
  SI::appendExpression( arg_list, SB::buildStringVal(fndecl -> get_name()) );

  SgFunctionType*        fntype = fndecl->get_type();
  SgType*                fnreturn = fntype->get_return_type();
  SgTypePtrList&         fnparams = fntype->get_arguments();

  trans.appendSignature( arg_list, fnreturn, fnparams );

  SgFunctionRefExp*      callee = SB::buildFunctionRefExp( trans.symbols.roseConfirmFunctionSignature );
  SgStatement*           stmt = SB::buildFunctionCallStmt( callee, arg_list );

  fndef.get_body()->prepend_statement(stmt);
}

void
RtedTransformation::insertDeallocationCheck(SgStatement& loc, SgInitializedName& resvar)
{
  SgExprListExp* args = SB::buildExprListExp();
  SgVarRefExp*   ptr = SB::buildVarRefExp(&resvar);
  SgAddressOfOp* ptraddr = SB::buildAddressOfOp(ptr);
  SgPointerType* ppvoid = SB::buildPointerType(SB::buildPointerType(SB::buildVoidType()));

  // \todo build reinterpret cast for C++ codes
  SI::appendExpression(args, SB::buildCastExp(ptraddr, ppvoid));

  insertCheck(ilAfter, &loc, symbols.roseCheckTransientPtr, args);
}

static
SgName gen_wrapper_name(const SgName& n)
{
  std::string fnname = n.getString();

  fnname.append("_impl");
  return fnname;
}

void
RtedTransformation::handleFunctionDefinition( FunctionDefContainer::value_type entry )
{
  typedef std::pair<SgStatement*, SgInitializedName*> WrapperDesc;

  // insert signature check

  SgFunctionDefinition&  fndef = *entry.first;

  insertVariableCreateInitForParams(fndef);
  insertSignatureCheck(fndef, *this);

  if (entry.second != ReturnInfo::rtIndirection) return;
  // insert function wrapper if return type is a pointer (\todo should be contains a pointer)

  SgFunctionDeclaration* fndecl = fndef.get_declaration();

  if (typeid(*fndecl) != typeid(SgFunctionDeclaration))
  {
    std::cerr << "error: " << typeid(*fndecl).name() << " wrapping not yet handled." << std::endl;
    return ;
  }

  // create a function wrapper
  WrapperDesc            funpair = SI::wrapFunction(*fndecl, gen_wrapper_name);
  ROSE_ASSERT(funpair.second);

  // add checks to the wrapper that guarantee that the object pointed to by
  //   the return value was not deallocated on scope exit.
  insertDeallocationCheck(*funpair.first, *funpair.second);
}

#endif
