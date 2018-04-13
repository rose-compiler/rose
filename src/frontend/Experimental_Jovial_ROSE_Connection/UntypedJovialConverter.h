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

      virtual void convertLabel(SgUntypedStatement* ut_stmt, SgStatement* sg_stmt,
                                SgLabelSymbol::label_type_enum label_type = SgLabelSymbol::e_start_label_type,
                                SgScopeStatement* label_scope=NULL);

   // Declaration statements
   //

   // Executable statements
   //

   // Expressions
   //

  };

} // namespace Untyped

// endif for UNTYPED_JOVIAL_CONVERTER_H
#endif

