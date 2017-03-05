#ifndef UNTYPED_CONVERTER_H
#define UNTYPED_CONVERTER_H

//-----------------------------------------------------------------------------------
// The SgUntyped::UntypedConverter class functions are used to convert
// SgUntypedNodes to regular SgNodes.
//-----------------------------------------------------------------------------------

namespace Fortran {
namespace Untyped {

class UntypedConverter
  {
    public:

      static void              setSourcePositionUnknown( SgLocatedNode* locatedNode );
      static SgScopeStatement* initialize_global_scope ( SgSourceFile* file );

      static void convertLabel(SgUntypedStatement* ut_stmt, SgStatement* sg_stmt,
                               SgLabelSymbol::label_type_enum label_type = SgLabelSymbol::e_start_label_type, SgScopeStatement* label_scope = NULL);

      static SgInitializedName*     convertSgUntypedInitializedName    (SgUntypedInitializedName* ut_name, SgType* sg_type, SgInitializer* sg_init);

      static SgImplicitStatement*   convertSgUntypedImplicitDeclaration(SgUntypedImplicitDeclaration* ut_decl, SgScopeStatement* scope);
      static SgVariableDeclaration* convertSgUntypedVariableDeclaration(SgUntypedVariableDeclaration* ut_decl, SgScopeStatement* scope);
      static SgType*                convertSgUntypedType               (SgUntypedType* ut_type,                SgScopeStatement* scope);



  };

} // namespace Fortran
} // namespace Untyped

// endif for UNTYPED_CONVERTER_H
#endif

