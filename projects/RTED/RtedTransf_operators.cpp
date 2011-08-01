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
    static const std::string comment = "RS : movePointer(typedesc, address, class_name, location)";

    SgStatement*   stmt = getSurroundingStatement( operand );
    ROSE_ASSERT( stmt );

    SgExprListExp* mp_args = SB::buildExprListExp();

    SI::appendExpression( mp_args, ctorTypeDesc(mkTypeInformation(operand->get_type(), false, false)) );
    appendAddress  ( mp_args, operand );
    appendClassName( mp_args, operand->get_type() );
    appendFileInfo ( mp_args, stmt->get_scope(), operand->get_file_info() );

    SgStatement*     check = insertCheck(ilAfter, stmt, symbols.roseMovePointer, mp_args, comment);

    // for UPC shared pointers we protect the update / check combination with
    //   a critical region
    if ( isUpcSharedPointer(operand->get_type()) )
    {
      SgExprListExp* lock_args   = SB::buildExprListExp();
      SgExprListExp* unlock_args = SB::buildExprListExp();

      SI::appendExpression(lock_args,   SI::deepCopy(operand));
      SI::appendExpression(unlock_args, SI::deepCopy(operand));

      insertCheck(ilBefore, stmt, symbols.roseUpcEnterSharedPtr, lock_args);
      insertCheck(ilAfter,  stmt, symbols.roseUpcExitSharedPtr,  unlock_args);
    }
}

#endif
