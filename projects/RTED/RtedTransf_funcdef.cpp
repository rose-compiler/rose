#include <rose.h>
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
      body->prepend_statement(
        buildVariableCreateCallStmt(
          param, getSurroundingStatement( param), true));
    }
}
