#include <rose.h>

// DQ (2/9/2010): Testing use of ROE to compile ROSE.
#ifndef USE_ROSE

#include <string>
#include <boost/foreach.hpp>
#include "RtedSymbols.h"
#include "DataStructures.h"
#include "RtedTransformation.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;


void
RtedTransformation::insertVariableCreateInitForParams( SgFunctionDefinition* fndef) {

    SgBasicBlock* body = fndef->get_body();
    ROSE_ASSERT( body);

    SgInitializedNamePtrList names
      = fndef->get_declaration()->get_parameterList()->get_args();

    BOOST_FOREACH( SgInitializedName* param, names) {
        if( isSgReferenceType( param -> get_type() ))
            // reference variables don't allocate new memory
            // if we call createVariable the RTS will think it's a double
            // allocation fault
            continue;

        body->prepend_statement(
            buildVariableCreateCallStmt(
                param, getSurroundingStatement( param), true ));
    }
}

void
RtedTransformation::appendSignature( SgExprListExp* arg_list, SgType* return_type, const SgTypePtrList& param_types)
{
	// number of type descriptors (arguments + return type)
	appendExpression( arg_list, buildIntVal( param_types.size() + 1));

  // generate a list of typedesc aggregate initializers representing the
  //   function signature.
  SgExprListExp* type_list = buildExprListExp();

  appendExpression( type_list, mkTypeInformation(return_type, true, true) );

  BOOST_FOREACH( SgType* p_type, param_types ) {
    appendExpression( type_list, mkTypeInformation( p_type, true, true ) );
  }

  // \pp \note probably roseTypeDesc below should really be an array of roseTypeDescs
	appendExpression( arg_list, ctorTypeDescList(genAggregateInitializer(type_list, roseTypeDesc())) );
}

void
RtedTransformation::insertConfirmFunctionSignature( SgFunctionDefinition* fndef )
{
	SgFunctionDeclaration* fndecl = fndef->get_declaration();

  if (isSgMemberFunctionDeclaration(fndecl))
		return;

	SgExprListExp*         arg_list = buildExprListExp();

  // first arg is the name
	// \todo
	// FIXME 2: This probably needs to be something closer to the mangled_name,
	// or perhaps we can simply skip the check entirely for C++
	appendExpression( arg_list, buildStringVal(fndecl -> get_name()) );

	SgFunctionType*        fntype = fndecl->get_type();
	SgType*                fnreturn = fntype->get_return_type();
	SgTypePtrList&         fnparams = fntype->get_arguments();

	appendSignature( arg_list, fnreturn, fnparams );

  SgFunctionRefExp*      callee = buildFunctionRefExp( symbols.roseConfirmFunctionSignature );
  SgStatement*           stmt = buildFunctionCallStmt( callee, arg_list );

	fndef->get_body()->prepend_statement(stmt);
}

#endif
