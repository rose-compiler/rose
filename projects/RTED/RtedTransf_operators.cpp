#include <rose.h>

// DQ (2/9/2010): Testing use of ROE to compile ROSE.
#ifndef USE_ROSE

#include "RtedSymbols.h"
#include "DataStructures.h"
#include "RtedTransformation.h"

namespace SI = SageInterface;
namespace SB = SageBuilder;

void RtedTransformation::insert_pointer_change( SgExpression* operand )
{
    ROSE_ASSERT(operand);

    SgStatement*   stmt = getSurroundingStatement( *operand );
    SgExprListExp* mp_args = SB::buildExprListExp();

    SI::appendExpression( mp_args, ctorTypeDesc(mkTypeInformation(operand -> get_type(), false, false)) );
    appendAddress       ( mp_args, operand );
    appendClassName     ( mp_args, operand -> get_type() );
    appendFileInfo      ( mp_args, stmt->get_scope(), operand->get_file_info() );

    insertCheck(ilAfter, stmt, symbols.roseMovePointer, mp_args);
}

#endif
