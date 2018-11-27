#ifndef UNTYPED_JOVIAL_CONVERTER_H
#define UNTYPED_JOVIAL_CONVERTER_H

//-----------------------------------------------------------------------------------
// The SgUntyped::UntypedConverter class functions are used to convert
// SgUntypedNodes to regular SgNodes.
//-----------------------------------------------------------------------------------

#include "sageBuilder.h"
#include "UntypedConverter.h"

namespace Untyped {

class UntypedJovialConverter : public UntypedConverter
  {
    public:

      virtual bool convertLabel(SgUntypedStatement* ut_stmt, SgStatement* sg_stmt, SgScopeStatement* label_scope=NULL);

      virtual bool convertLabel(SgUntypedStatement* ut_stmt, SgStatement* sg_stmt,
                                SgLabelSymbol::label_type_enum label_type, SgScopeStatement* label_scope=NULL);

   // Declaration statements
   //

   // Executable statements
   //

    virtual SgStatement* convertSgUntypedCaseStatement (SgUntypedCaseStatement* ut_stmt, SgNodePtrList& children, SgScopeStatement* scope);

   // Expressions
   //

  };

} // namespace Untyped

// endif for UNTYPED_JOVIAL_CONVERTER_H
#endif

