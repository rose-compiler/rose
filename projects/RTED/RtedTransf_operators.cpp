#include <rose.h>

// DQ (2/9/2010): Testing use of ROE to compile ROSE.
#ifndef USE_ROSE

#include "RtedSymbols.h"
#include "DataStructures.h"
#include "RtedTransformation.h"

using namespace SageBuilder;

void RtedTransformation::insert_pointer_change( SgExpression* operand )
{
    static const std::string comment = "RS : movePointer , parameters : ( address, type, base_type, indirection_level, class_name, filename, lineno, linetransformed)"

    SgStatement*   stmt = getSurroundingStatement( operand );
    ROSE_ASSERT( stmt );

    SgExprListExp* mp_args = buildExprListExp();

    appendExpression( mp_args, ctorTypeDesc(mkTypeInformation(operand->get_type(), false, false)) );
    appendAddress   ( mp_args, operand );
    appendClassName ( mp_args, operand -> get_type() );
    appendFileInfo  ( mp_args, stmt->get_scope(), exp -> get_file_info() );

    ROSE_ASSERT(symbols.roseMovePointer);
    SgExprStatement* mp_call = buildFunctionCallStmt( buildFunctionRefExp(), mp_args );

    insertCheck(ilAfter, stmt, symbols.roseMovePointer, mp_args, comment);
}

#endif
