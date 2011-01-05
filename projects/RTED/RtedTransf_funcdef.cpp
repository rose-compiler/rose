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
RtedTransformation::visit_isFunctionDefinition( SgNode* node) {
    visit_checkIsMain( node);

    SgFunctionDefinition* fndef = isSgFunctionDefinition( node);
    ROSE_ASSERT( fndef);

    function_definitions.push_back( fndef);
}


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
RtedTransformation::insertConfirmFunctionSignature( SgFunctionDefinition* fndef ) {

    SgExprListExp* arg_list = buildExprListExp();

	// first arg is the name
	// FIXME 2: This probably needs to be something closer to the mangled_name,
	// or perhaps we can simply skip the check entirely for C++
    if (isSgMemberFunctionDeclaration(fndef->get_declaration()))
		return;
	appendExpression( arg_list, buildStringVal(
		fndef -> get_declaration() -> get_name()
	));

	// append param count (+1 for return type) and types
	Rose_STL_Container< SgType* > param_types
		= fndef -> get_declaration() -> get_type() -> get_arguments();
	appendExpression( arg_list, buildIntVal( param_types.size() + 1));

	// return type
	appendTypeInformation(
	  arg_list,
		fndef -> get_declaration() -> get_type() -> get_return_type(),
		true,
		true );

	// parameter types
	BOOST_FOREACH( SgType* param_type, param_types ) {
		appendTypeInformation( arg_list, param_type, true, true );
	}


	fndef -> get_body() -> prepend_statement(
		buildExprStatement(
			buildFunctionCallExp(
				buildFunctionRefExp( symbols.roseConfirmFunctionSignature ),
				arg_list ))
	);
}

#endif
