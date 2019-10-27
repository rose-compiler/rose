#include "sage3basic.h"
#include "UntypedConverter.h"
#include "general_language_translation.h"
#include "Jovial_to_ROSE_translation.h"

#define DEBUG_UNTYPED_CONVERTER 0
#define DEBUG_SOURCE_POSITION   0

using namespace Untyped;
using std::cout;
using std::cerr;
using std::endl;

// Should get rid of this and just use SageInterface::setSourcePosition()
// It is okay to reset it later.
void
UntypedConverter::setSourcePositionUnknown(SgLocatedNode* locatedNode)
{
  // This function sets the source position to be marked as not available (since we don't have token information)
  // These nodes WILL be unparsed in the code generation phase.

#if DEBUG_SOURCE_POSITION
     printf ("UntypedConverter::setSourcePositionUnknown: locatedNode = %p = %s \n",locatedNode,locatedNode->class_name().c_str());
#endif

  // The SgLocatedNode has both a startOfConstruct and endOfConstruct source position.
     ROSE_ASSERT(locatedNode != NULL);

  // Make sure we never try to reset the source position of the global scope (set elsewhere in ROSE).
     ROSE_ASSERT(isSgGlobal(locatedNode) == NULL);

  // Check the endOfConstruct first since it is most likely NULL (helpful in debugging)
     if (locatedNode->get_endOfConstruct() != NULL || locatedNode->get_startOfConstruct() != NULL)
        {
        // TODO - figure out if anything needs to be done here
        // printf ("In setSourcePositionUnknown: source position known locatedNode = %p = %s \n",locatedNode,locatedNode->class_name().c_str());
        }
     else
        {
           ROSE_ASSERT(locatedNode->get_endOfConstruct()   == NULL);
           ROSE_ASSERT(locatedNode->get_startOfConstruct() == NULL);
           SageInterface::setSourcePosition(locatedNode);
        }
}

void
UntypedConverter::setSourcePositionFrom ( SgLocatedNode* toNode, SgLocatedNode* fromNode )
{
   ROSE_ASSERT(toNode != NULL && fromNode != NULL);

   Sg_File_Info* start = fromNode->get_startOfConstruct();
   Sg_File_Info*   end = fromNode->get_endOfConstruct();

   if (start == NULL || end == NULL) {
      cerr << "UntypedConverter::setSourcePositionFrom:  --- toNode: " << toNode << ": " << toNode->class_name()
           << " from: " << fromNode << ": " << fromNode->class_name() << endl;
      ROSE_ASSERT(start != NULL && end != NULL);
   }

// SageBuilder may have been used and it builds FileInfo
   if (toNode->get_startOfConstruct() != NULL) {
      delete toNode->get_startOfConstruct();
      toNode->set_startOfConstruct(NULL);
   }
   if (toNode->get_endOfConstruct() != NULL) {
      delete toNode->get_endOfConstruct();
      toNode->set_endOfConstruct(NULL);
   }

#if DEBUG_SOURCE_POSITION
   cout << "UntypedConverter::setSourcePositionFrom:  --- toNode: " << toNode << " from: " << fromNode;
   cout << " strt: " << start->get_line() << " " << start->get_col();
   cout << " end:  " <<   end->get_line() << " " <<   end->get_col() << std::endl;
#endif

   std::string filename = start->get_filenameString();

   toNode->set_startOfConstruct(new Sg_File_Info(filename, start->get_line(), start->get_col()));
   toNode->get_startOfConstruct()->set_parent(toNode);

   toNode->set_endOfConstruct(new Sg_File_Info(filename, end->get_line(), end->get_col()));
   toNode->get_endOfConstruct()->set_parent(toNode);

   SageInterface::setSourcePosition(toNode);
}

void
UntypedConverter::setSourcePositionIncluding ( SgLocatedNode* toNode, SgLocatedNode* startNode, SgLocatedNode* endNode )
{
   ROSE_ASSERT(toNode != NULL && startNode != NULL && endNode != NULL);

   Sg_File_Info* start = startNode->get_startOfConstruct();
   Sg_File_Info* end = endNode->get_endOfConstruct();

   ROSE_ASSERT(start != NULL && end != NULL);
   ROSE_ASSERT(toNode->get_startOfConstruct() == NULL);
   ROSE_ASSERT(toNode->get_endOfConstruct()   == NULL);

#if DEBUG_SOURCE_POSITION
   std::cout << "UntypedConverter::setSourcePositionIncluding: ";
   printf("   --- toNode: %p start: %p end %p", toNode, startNode, endNode);
   std::cout << " strt: " << start->get_line() << " " << start->get_col();
   std::cout << " end:  " <<   end->get_line() << " " <<   end->get_col() << std::endl;
#endif

   std::string filename = start->get_filenameString();

   toNode->set_startOfConstruct(new Sg_File_Info(filename, start->get_line(), start->get_col()));
   toNode->get_startOfConstruct()->set_parent(toNode);

   toNode->set_endOfConstruct(new Sg_File_Info(filename, end->get_line(), end->get_col()));
   toNode->get_endOfConstruct()->set_parent(toNode);

   SageInterface::setSourcePosition(toNode);
}

void
UntypedConverter::convertFunctionPrefix (SgUntypedExprListExpression* prefix_list, SgFunctionDeclaration* function_decl)
{
   BOOST_FOREACH(SgUntypedExpression* ut_expr, prefix_list->get_expressions())
   {
      ROSE_ASSERT( isSgUntypedOtherExpression(ut_expr) );

      using namespace General_Language_Translation;

      switch(ut_expr->get_expression_enum())
       {
         case e_function_modifier_none:
            {
            // No function modifier to add
               break;
            }
         case e_function_modifier_elemental:
            {
               cout << "..SETTING.. elemental \n";
               function_decl->get_functionModifier().setElemental();
               break;
            }
#if 0
      // TODO
         case e_function_modifier_impure:
            {
               function_decl->get_functionModifier().setImpure();
               break;
            }
      // TODO
         case e_function_modifier_module:
            {
               function_decl->get_functionModifier().setModule();
               break;
            }
#endif
         case e_function_modifier_pure:
            {
               function_decl->get_functionModifier().setPure();
               break;
            }
         case e_function_modifier_recursive:
            {
               function_decl->get_functionModifier().setRecursive();
               break;
            }

       // CUDA function modifiers/qualifiers
       // ----------------------------------
          case e_cuda_host:
            {
               function_decl->get_functionModifier().setCudaHost();
               break;
            }
          case e_cuda_global_function:
            {
               function_decl->get_functionModifier().setCudaGlobalFunction();
               break;
            }
          case e_cuda_device:
            {
               function_decl->get_functionModifier().setCudaDevice();
               break;
            }
          case e_cuda_grid_global:
            {
               function_decl->get_functionModifier().setCudaGridGlobal();
               break;
            }

         default:
            {
               cerr << "ERROR: UntypedConverter::convertFunctionPrefix: unimplemented prefix for enum "
                    << ut_expr->get_expression_enum() << endl;
               ROSE_ASSERT(0);  // NOT IMPLEMENTED                                                                  
            }
       }
   }
}

void
UntypedConverter::setDeclarationModifiers(SgDeclarationStatement* decl, SgUntypedExprListExpression* modifier_list)
{
   using namespace General_Language_Translation;

   // Set to undefined and change if necessary
   decl->get_declarationModifier().get_accessModifier().setUndefined();

   BOOST_FOREACH(SgUntypedExpression* ut_expr, modifier_list->get_expressions())
    {
    // Check for Jovial LocationSpecifier
       SgUntypedExprListExpression* expr_list = isSgUntypedExprListExpression(ut_expr);
       if (expr_list)
          {
             SgVariableDeclaration* var_decl = isSgVariableDeclaration(decl);
             ROSE_ASSERT(var_decl);
             ROSE_ASSERT(expr_list->get_expressions().size() == 2);

          // If deleted illegal object is found in tree traversal
             SgExprListExp* sg_location_specifier = convertUntypedExprListExpression(expr_list, /*delete*/false);
             ROSE_ASSERT(sg_location_specifier);

          // The bitfield will contain both the start_bit and start_word as an expression list
             var_decl->set_bitfield(sg_location_specifier);
             sg_location_specifier->set_parent(var_decl);

          // NOTE: this saves indenting the rather large switch statement; not clear the an else clause is more readable
             continue;
          }

    // Otherwise not a Jovial LocationSpecifier, must be a simple modifier based on the expression enum
       ROSE_ASSERT( isSgUntypedOtherExpression(ut_expr) );

       switch(ut_expr->get_expression_enum())
        {
          case e_access_modifier_public:
            {
               decl->get_declarationModifier().get_accessModifier().setPublic();
               break;
            }
          case e_access_modifier_private:
            {
               decl->get_declarationModifier().get_accessModifier().setPrivate();
               break;
            }
          case e_type_modifier_allocatable:
            {
               decl->get_declarationModifier().get_typeModifier().setAllocatable();
               break;
            }
          case e_type_modifier_asynchronous:
            {
               decl->get_declarationModifier().get_typeModifier().setAsynchronous();
               break;
            }
          case e_storage_modifier_contiguous:
            {
               decl->get_declarationModifier().get_storageModifier().setContiguous();
               break;
            }
          case e_storage_modifier_external:
            {
               decl->get_declarationModifier().get_storageModifier().setExtern();
               break;
            }
          case e_storage_modifier_jovial_def:
            {
               decl->get_declarationModifier().get_storageModifier().setJovialDef();
               break;
            }
          case e_storage_modifier_jovial_ref:
            {
               decl->get_declarationModifier().get_storageModifier().setJovialRef();
               break;
            }
          case e_storage_modifier_static:
            {
               cerr << "WARNING UNIMPLEMENTED: setDeclarationModifiers: Static\n";
               break;
            }
           case e_type_modifier_intent_in:
            {
               decl->get_declarationModifier().get_typeModifier().setIntent_in();
               break;
            }
          case e_type_modifier_intent_out:
            {
               decl->get_declarationModifier().get_typeModifier().setIntent_out();
               break;
            }
          case e_type_modifier_intent_inout:
            {
               decl->get_declarationModifier().get_typeModifier().setIntent_inout();
               break;
            }
          case e_type_modifier_intrinsic:
            {
               decl->get_declarationModifier().get_typeModifier().setIntrinsic();
               break;
            }
          case e_type_modifier_optional:
            {
               decl->get_declarationModifier().get_typeModifier().setOptional();
               break;
            }
#if 0
// TODO - Fortran is a special case and must be handled differently
          case Fortran_ROSE_Translation::e_parameter:
          case e_type_modifier_const:
            {
               decl->get_declarationModifier().get_typeModifier().get_constVolatileModifier().setConst();
               break;
            }
#endif
#if 0
// TODO - Fortran is a special case and must be handled differently
          case e_pointer:
            {
               decl->get_declarationModifier().get_typeModifier().setPointer();
               break;
            }
#endif
#if 0
// TODO - Fortran is a special case and must be handled differently
          case e_type_modifier_protected:
            {
               decl->get_declarationModifier().get_typeModifier().setProtected();
               break;
            }
#endif
          case e_type_modifier_const:
            {
               decl->get_declarationModifier().get_typeModifier().get_constVolatileModifier().setConst();
               break;
            }
          case e_type_modifier_save:
            {
               decl->get_declarationModifier().get_typeModifier().setSave();
               break;
            }
          case e_type_modifier_target:
            {
               decl->get_declarationModifier().get_typeModifier().setTarget();
               break;
            }
          case e_type_modifier_value:
            {
               decl->get_declarationModifier().get_typeModifier().setValue();
               break;
            }
          case e_type_modifier_volatile:
            {
               decl->get_declarationModifier().get_typeModifier().get_constVolatileModifier().setVolatile();
               break;
            }

       // CUDA variable attributes/qualifiers
       // -----------------------------------
          case e_cuda_device_memory:
            {
               decl->get_declarationModifier().get_storageModifier().setCudaDeviceMemory();
               break;
            }
          case e_cuda_managed:
            {
               decl->get_declarationModifier().get_storageModifier().setCudaManaged();
               break;
            }
          case e_cuda_constant:
            {
               decl->get_declarationModifier().get_storageModifier().setCudaConstant();
               break;
            }
          case e_cuda_shared:
            {
               decl->get_declarationModifier().get_storageModifier().setCudaShared();
               break;
            }
          case e_cuda_pinned:
            {
               decl->get_declarationModifier().get_storageModifier().setCudaPinned();
               break;
            }
          case e_cuda_texture:
            {
               decl->get_declarationModifier().get_storageModifier().setCudaTexture();
               break;
            }

          default:
            {
               std::cerr << "ERROR: UntypedConverter::setDeclarationModifiers: unimplemented variable modifier, "
                         << "expression enum is " << ut_expr->get_expression_enum() << std::endl;
               ROSE_ASSERT(0);  // NOT IMPLEMENTED                                                                  
            }
       }
   }
}

SgType*
UntypedConverter::convertUntypedType (SgUntypedType* ut_type, SgScopeStatement* scope)
{
   int type_enum;
   SgType* sg_type = NULL;
   SgUntypedType* ut_base_type = NULL;
   SgUntypedExprListExpression* dim_info = NULL;
   SgExprListExp* sg_dim_info = NULL;

   bool is_table_type = false;
   bool is_array_type = false;
   bool has_dim_info  = false;

#if 0
   cout << "--- convertUntypedType: ut_type is " << ut_type << " : " << ut_type->class_name() << endl;
   cout << "--- convertUntypedType: is_intrinsic is " << ut_type->get_is_intrinsic() << endl;
   cout << "--- convertUntypedType: user_defined is " << ut_type->get_is_user_defined() << endl;
   cout << "--- convertUntypedType: is_class     is " << ut_type->get_is_class() << endl;
   cout << "--- convertUntypedType: type enum    is " << ut_type->get_type_enum_id() << endl;
   if (ut_type->get_is_user_defined())
      cout << "                                name is " << ut_type->get_type_name() << endl;
#endif

// Temporary assertions as this conversion is completed
   ROSE_ASSERT(ut_type->get_is_literal() == false);
   ROSE_ASSERT(ut_type->get_is_constant() == false);

   if (ut_type->get_is_user_defined() == true) {

   // This type should have already been created by a type declaration statement
   //
      SgName name = ut_type->get_type_name();
      SgClassSymbol* class_symbol = SageInterface::lookupClassSymbolInParentScopes(name, scope);

      if (class_symbol != NULL)
         {
            sg_type = class_symbol->get_type();
         }
      ROSE_ASSERT(sg_type != NULL);

      return sg_type;
   }

// TODO - Fortran needs type-attr-spec-list?

   SgExpression* kindExpression = NULL;

   if (ut_type->get_has_kind())
      {
         SgNodePtrList children;
         SgUntypedExpression* ut_kind = ut_type->get_type_kind();
      // TODO - figure out how to handle operators (or anything with children)
         ROSE_ASSERT(isSgUntypedValueExpression(ut_kind) != NULL || isSgUntypedReferenceExpression(ut_kind) != NULL);
         kindExpression = convertUntypedExpression(ut_kind, children, scope);

      // Causes problems in addAssociatedNodes if not deleted
         delete ut_kind;
      }

// Jovial tables and Fortran arrays should be handled first and in a similar manner
// as they both have a base type and dimension information.

// This is the default but may be changed to the enum of the base type
   type_enum = ut_type->get_type_enum_id();

// Consider Jovial table instance
   if (type_enum == SgUntypedType::e_table)
      {
      // Use the base type to access the underlying type of the table
         SgUntypedTableType* ut_table_type = isSgUntypedTableType(ut_type);
         ROSE_ASSERT(ut_table_type != NULL);

         dim_info = ut_table_type->get_dim_info();
         ROSE_ASSERT(dim_info != NULL);

         ut_base_type = ut_table_type->get_base_type();
         ROSE_ASSERT(ut_base_type != NULL);

         is_table_type = true;
         has_dim_info  = true;

         type_enum = ut_base_type->get_type_enum_id();
      }

// Consider Fortran array instance
   else if (isSgUntypedArrayType(ut_type) != NULL)
      {
         SgUntypedArrayType* ut_array_type = isSgUntypedArrayType(ut_type);
         ROSE_ASSERT(ut_array_type != NULL);

         dim_info = ut_array_type->get_dim_info();
         ROSE_ASSERT(dim_info != NULL);

      // The base type is the type (unlike Sage array nodes)
         ut_base_type = ut_type;
         ROSE_ASSERT(ut_base_type);

         is_array_type = true;
         has_dim_info  = true;

         type_enum = ut_base_type->get_type_enum_id();
      }

   switch(type_enum)
      {
     // Unknown type commonly used for function parameters before actual type is declared
        case SgUntypedType::e_unknown:        sg_type = SgTypeUnknown::createType();               break;

        case SgUntypedType::e_void:           sg_type = SageBuilder::buildVoidType();              break;
        case SgUntypedType::e_int:            sg_type = SgTypeInt::createType(0, kindExpression);  break;
        case SgUntypedType::e_uint:           sg_type = SgTypeUnsignedInt::createType(kindExpression); break;
        case SgUntypedType::e_float:          sg_type = SgTypeFloat::createType(kindExpression);   break;
        case SgUntypedType::e_double:         sg_type = SageBuilder::buildDoubleType();            break;

     // complex types
        case SgUntypedType::e_complex:        sg_type = SgTypeComplex::createType(SgTypeFloat::createType(kindExpression), kindExpression); break;
        case SgUntypedType::e_double_complex: sg_type = SgTypeComplex::createType(SgTypeDouble::createType());                              break;

     // TODO - is incorrect (may be closer to a hexadecimal or Hollerith, but used in boolean expressions)
        case SgUntypedType::e_bit:            sg_type = SgTypeBool::createType(kindExpression);    break;
        case SgUntypedType::e_bool:           sg_type = SgTypeBool::createType(kindExpression);    break;

        case SgUntypedType::e_char:
           {
              ROSE_ASSERT(kindExpression == NULL);
              sg_type = SageBuilder::buildCharType();
              break;
           }
        case SgUntypedType::e_string:
           {
              SgExpression* sg_length = NULL;

              if (ut_type->get_char_length_is_string())
                 {
                    SgIntVal* str_length = SageBuilder::buildIntVal(atoi(ut_type->get_char_length_string().c_str()));
                    ROSE_ASSERT(str_length);
                    str_length->set_valueString(ut_type->get_char_length_string());
                    sg_length = str_length;
                 }
              else
                 {
                    SgNodePtrList children;
                    SgUntypedExpression* ut_length = ut_type->get_char_length_expression();
                 // TODO - figure out how to handle operators (or anything with children)
                    ROSE_ASSERT(isSgUntypedValueExpression(ut_length) != NULL || isSgUntypedReferenceExpression(ut_length) != NULL);

                    sg_length = convertUntypedExpression(ut_length, children, scope);
                 }

              ROSE_ASSERT(sg_length != NULL);
              sg_type = SageBuilder::buildStringType(sg_length);
              break;
           }

        default:
           {
              cerr << "UntypedConverter::convertUntypedType: failed to find known type, enum is "
                   << ut_type->get_type_enum_id() << endl;
              ROSE_ASSERT(0);
           }
      }

// TODO - determine if this is necessary
   if (kindExpression != NULL)
      {
         kindExpression->set_parent(sg_type);
      }

   if (has_dim_info)
      {
      // Must go south here!!!
      // sg_dim_info = convertUntypedExprListExpression(ut_array_type->get_dim_info(),/*delete*/true);
         sg_dim_info = convertUntypedExprListExpression(dim_info,/*delete*/false);
         ROSE_ASSERT(sg_dim_info != NULL);
      }

   if (is_table_type)
      {
         SgUntypedTableType* ut_table_type = isSgUntypedTableType(ut_type);
         ROSE_ASSERT(ut_table_type != NULL);

         SgType* sg_base_type = sg_type;

         SgJovialTableType* sg_table_type = SageBuilder::buildJovialTableType(ut_table_type->get_type_name(), sg_base_type, sg_dim_info, scope);
         ROSE_ASSERT(sg_table_type != NULL);

         sg_type = sg_table_type;

      // TODO: There needs to be a declaration but can't do it here because declaration context is probably calling
      // this function
#if 0
         SgClassType* classType = isSgClassType(sg_type);
         cout << "-x-   class type is " << classType << ": " << classType->class_name() << endl;
         if (classType != NULL)
            {
               SgDeclarationStatement* declaration = classType->get_declaration();
               ROSE_ASSERT(declaration != NULL);
               SgClassDeclaration* classDeclaration = isSgClassDeclaration(declaration);
               ROSE_ASSERT(classDeclaration != NULL);
            }
#endif
      }

   else if (is_array_type)
      {
         SgUntypedArrayType* ut_array_type = isSgUntypedArrayType(ut_type);
         ROSE_ASSERT(ut_array_type != NULL);

         SgType* sg_base_type = sg_type;

         SgArrayType* sg_array_type = SageBuilder::buildArrayType(sg_base_type, sg_dim_info);
         ROSE_ASSERT(sg_array_type != NULL);

         sg_type = sg_array_type;

      // TODO: There needs to be a declaration but can't do it here because declaration context is probably calling
      // this function
      }

   ROSE_ASSERT(sg_type != NULL);

   return sg_type;
}

SgType*
UntypedConverter::convertUntypedType (SgUntypedInitializedName* ut_name, SgScopeStatement* scope, bool delete_ut_type)
{
   SgType* sg_type = NULL;
   SgUntypedType* ut_type = ut_name->get_type();

#if 0
   cout << "--- convertUntypedType: ut_type is " << ut_type << " : " << ut_type->class_name() << endl;
   cout << "--- convertUntypedType: name is " << ut_name->get_name() << " ut_name is " << ut_name << endl;
   cout << "--- convertUntypedType:  t_enum is " << ut_type->get_type_enum_id() << endl;
   cout << "--- convertUntypedType:  delete is " << delete_ut_type << endl;
#endif

   if (SageInterface::is_Fortran_language() && (ut_type->get_type_enum_id() == SgUntypedType::e_implicit))
      {
      // TODO - create function to determine implicit type, for now pick real
      // Change the type enum to an explicit type based on the variable name
      // WARNING: Temporary HACK ATTACK
         cout << "--- convertUntypedType: Fortran type is implicit, NEED to set type by name " << endl;
         ut_type->set_type_enum_id(SgUntypedType::e_float);
      }

   sg_type = convertUntypedType(ut_type, scope);

   ROSE_ASSERT(sg_type != NULL);

   if (delete_ut_type)
      {
      // Untyped types are not traversed so here is an opportunity to delete the type.
      // If this is an array type the dim_info expressions will have already been deleted by convertUntypedType
         ut_name->set_type(NULL);
         delete ut_type;
      }

   return sg_type;
}

SgInitializer* UntypedConverter::
convertUntypedInitializerOnly (SgUntypedInitializedName* ut_name)
{
   SgInitializer* sg_initializer = NULL;

   if (ut_name->get_has_initializer())
      {
         SgUntypedExpression* ut_init_expr = ut_name->get_initializer();
         SgExpression* sg_init_expr = convertUntypedExpression(ut_init_expr);
         ROSE_ASSERT(sg_init_expr);

         sg_initializer = new SgAssignInitializer(sg_init_expr, sg_init_expr->get_type());
         ROSE_ASSERT(sg_initializer);
         setSourcePositionFrom(sg_initializer, ut_init_expr);

      // The initialization expression isn't traversed so it should be deleted
         delete ut_init_expr;
      }

   return sg_initializer;
}

SgInitializedName* UntypedConverter::
convertUntypedInitializedName (SgUntypedInitializedName* ut_name, SgType* sg_base_type)
{
   SgType* sg_type = sg_base_type;

#if 0
   cout << "--- convertUntypedInitializedName:    name is " << ut_name->get_name() << endl;
   cout << "--- convertUntypedInitializedName:  b_type is " << sg_base_type << ": " << sg_base_type->class_name() << endl;
   cout << "--- convertUntypedInitializedName: ut_type is " << ut_name->get_type() << ": " << endl;
#endif

// TODO - This needs to be combined/eliminated with convertUntypedType
   if (isSgUntypedArrayType(ut_name->get_type()))
      {
         SgUntypedArrayType* ut_array_type = isSgUntypedArrayType(ut_name->get_type());
         SgUntypedExprListExpression* dim_info = ut_array_type->get_dim_info();
         ROSE_ASSERT(dim_info != NULL);

         cout << "--- convertUntypedInitializedName:dim_info is "
              << dim_info << ": " << dim_info->class_name() << endl;

//       SgExprListExp* sg_dim_info = convertUntypedExprListExpression(dim_info,/*delete*/true);
         SgExprListExp* sg_dim_info = convertUntypedExprListExpression(dim_info,/*delete*/false);

         sg_type = SageBuilder::buildArrayType(sg_base_type, sg_dim_info);
      }

   SgInitializedName* sg_name = SageBuilder::buildInitializedName_nfi(ut_name->get_name(), sg_type, /*init*/NULL);
   ROSE_ASSERT(sg_name);
   setSourcePositionFrom(sg_name, ut_name);

// Convert the initializer too if it exists
   if (ut_name->get_has_initializer())
      {
         SgInitializer* sg_initializer = convertUntypedInitializerOnly(ut_name);
         ROSE_ASSERT(sg_initializer != NULL);

         sg_name->set_initializer(sg_initializer);
         sg_initializer->set_parent(sg_name);
      }

   return sg_name;
}

SgInitializedNamePtrList*
UntypedConverter::convertUntypedInitializedNameList (SgUntypedInitializedNameList* ut_name_list, SgType* sg_base_type)
{
   SgUntypedInitializedNamePtrList & ut_names = ut_name_list->get_name_list();

   SgInitializedNamePtrList* sg_names = new SgInitializedNamePtrList();

   BOOST_FOREACH(SgUntypedInitializedName* ut_name, ut_names)
      {
         SgInitializedName* sg_name = convertUntypedInitializedName(ut_name, sg_base_type);
         sg_names->push_back(sg_name);
      }

   return sg_names;
}

SgGlobal*
UntypedConverter::convertUntypedGlobalScope (SgUntypedGlobalScope* ut_scope, SgScopeStatement* scope)
{
// The global scope should not have executables
   ROSE_ASSERT(ut_scope->get_statement_list()  -> get_traversalSuccessorContainer().size() == 0);

   SgGlobal* sg_scope = isSgGlobal(scope);
   ROSE_ASSERT(sg_scope == SageBuilder::getGlobalScopeFromScopeStack());

   return sg_scope;
}

SgDeclarationStatement*
UntypedConverter::convertUntypedDirectiveDeclaration (SgUntypedDirectiveDeclaration* ut_stmt, SgScopeStatement* scope)
   {
      using namespace General_Language_Translation;

      SgDeclarationStatement* sg_stmt = NULL;

      switch (ut_stmt->get_statement_enum())
        {
        case e_define_directive_stmt:
           {
              std::string define_string = ut_stmt->get_directive_string();

              SgJovialDefineDeclaration* define_decl = new SgJovialDefineDeclaration(define_string);
              ROSE_ASSERT(define_decl);
              setSourcePositionFrom(define_decl, ut_stmt);

           // The first nondefining declaration must be set
              define_decl->set_firstNondefiningDeclaration(define_decl);

              SageInterface::appendStatement(define_decl, scope);
              define_decl->set_parent(scope);

              sg_stmt = define_decl;
              break;
           }
        case Jovial_ROSE_Translation::e_compool_directive_stmt:
           {
              std::string compool_string = ut_stmt->get_directive_string();

              SgJovialDirectiveStatement* compool_stmt = new SgJovialDirectiveStatement(compool_string, SgJovialDirectiveStatement::e_compool);
              ROSE_ASSERT(compool_stmt);
              setSourcePositionFrom(compool_stmt, ut_stmt);

           // The first nondefining declaration must be set
              compool_stmt->set_firstNondefiningDeclaration(compool_stmt);

              SageInterface::appendStatement(compool_stmt, scope);
              compool_stmt->set_parent(scope);

              sg_stmt = compool_stmt;
              break;
           }
        case  Jovial_ROSE_Translation::e_reducible_directive_stmt:
           {
              std::string dir_string = ut_stmt->get_directive_string();

              SgJovialDirectiveStatement* reducible_stmt = new SgJovialDirectiveStatement(dir_string, SgJovialDirectiveStatement::e_reducible);
              ROSE_ASSERT(reducible_stmt);
              setSourcePositionFrom(reducible_stmt, ut_stmt);

              // The first nondefining declaration must be set
              reducible_stmt->set_firstNondefiningDeclaration(reducible_stmt);

              SageInterface::appendStatement(reducible_stmt, scope);
              reducible_stmt->set_parent(scope);

              sg_stmt = reducible_stmt;
              break;
           }
        case  Jovial_ROSE_Translation::e_order_directive_stmt:
           {
              std::string dir_string = ut_stmt->get_directive_string();

              SgJovialDirectiveStatement* order_stmt = new SgJovialDirectiveStatement(dir_string, SgJovialDirectiveStatement::e_order);
              ROSE_ASSERT(order_stmt);
              setSourcePositionFrom(order_stmt, ut_stmt);

              // The first nondefining declaration must be set
              order_stmt->set_firstNondefiningDeclaration(order_stmt);

              SageInterface::appendStatement(order_stmt, scope);
              order_stmt->set_parent(scope);

              sg_stmt = order_stmt;
              break;
           }
        default:
           {
              cout << "Warning: UntypedDirectiveDeclaration stmt_enum not handled is " << ut_stmt->get_statement_enum() << endl;
           }
        }

      ROSE_ASSERT(sg_stmt);

      return sg_stmt;
   }

SgEnumDeclaration*
UntypedConverter::convertUntypedEnumDeclaration (SgUntypedEnumDeclaration* ut_decl, SgScopeStatement* scope)
   {
      int enum_val = 0;
      SgName enum_name = ut_decl->get_enum_name();

      SgEnumDeclaration* enum_decl = SageBuilder::buildEnumDeclaration(enum_name, scope);
      ROSE_ASSERT(enum_decl);
      setSourcePositionFrom(enum_decl, ut_decl);

   // First assume easy case where enum hasn't been declared yet
      SgEnumSymbol* enum_symbol = SageInterface::lookupEnumSymbolInParentScopes(enum_name, scope);
      ROSE_ASSERT(enum_symbol);

   // Build the integer type for tne enumerators
      SgType * enum_int_type = SageBuilder::buildIntType();
      ROSE_ASSERT(enum_int_type != NULL);

      SgUntypedInitializedNamePtrList ut_init_names = ut_decl->get_enumerators()->get_name_list();

      BOOST_FOREACH(SgUntypedInitializedName* ut_init_name, ut_init_names)
         {
         // BIG ASSUMPTION: assume that the initializer is an integer value expression
            SgIntVal* val_expr = NULL;

            SgName name = ut_init_name->get_name();

         // enumerator value
            if (ut_init_name->get_has_initializer())
               {
                  SgUntypedExpression* ut_init_expr = ut_init_name->get_initializer();
                  SgUntypedValueExpression* ut_val_expr = isSgUntypedValueExpression(ut_init_expr);
                  ROSE_ASSERT(ut_val_expr);
                  SgValueExp* sg_val_expr = convertUntypedValueExpression(ut_val_expr, true);
                  val_expr = isSgIntVal(sg_val_expr);
                  ROSE_ASSERT(val_expr);
                  enum_val = val_expr->get_value();
               }
            else
               {
                  val_expr = SageBuilder::buildIntVal(enum_val);
                  ROSE_ASSERT(val_expr);
                  SageInterface::setSourcePosition(val_expr);
               }

            enum_val += 1;

         // enumerator initializer
            SgAssignInitializer* initializer = SageBuilder::buildAssignInitializer(val_expr, enum_int_type);
            ROSE_ASSERT(initializer);
            SageInterface::setSourcePosition(initializer);

         // initialized name
            SgInitializedName* init_name = SageBuilder::buildInitializedName(name, enum_int_type, initializer);
            ROSE_ASSERT(init_name);
            SageInterface::setSourcePosition(init_name);

         // set the scope (especially) otherwise will fail in initializeExplicitScopeData.C
            init_name->set_parent(enum_decl);
            init_name->set_scope(scope);

            enum_decl->append_enumerator(init_name);
         }

      SageInterface::appendStatement(enum_decl, scope);

      return enum_decl;
   }

void
UntypedConverter::convertUntypedFunctionDeclarationList (SgUntypedFunctionDeclarationList* ut_list, SgScopeStatement* scope)
   {
   // Only a Fortran specific implementation needed for now
      cerr << "WARNING UNIMPLEMENTED: convertUntypedFunctionDeclarationList \n";
   }

#if 0
SgDeclarationStatement*
UntypedConverter::convertUntypedStructureDeclaration (SgUntypedStructureDeclaration* ut_struct, SgScopeStatement* scope)
   {
   // Implementation in derived classes
      ROSE_ASSERT(false);
      return NULL;
   }
#endif

SgTypedefDeclaration*
UntypedConverter::convertUntypedTypedefDeclaration  (SgUntypedTypedefDeclaration* ut_decl, SgScopeStatement* scope)
   {
      SgTypedefDeclaration* sg_decl = NULL;
      std::string name = ut_decl->get_name();

      SgUntypedType* ut_base_type = ut_decl->get_base_type();
      SgType*        sg_base_type = convertUntypedType(ut_base_type, scope);

      sg_decl = SageBuilder::buildTypedefDeclaration (name, sg_base_type, scope, /*has_defining_base*/false);
      ROSE_ASSERT(sg_decl != NULL);
      setSourcePositionFrom(sg_decl, ut_decl);

      SageInterface::appendStatement(sg_decl, scope);

      return sg_decl;
   }

SgModuleStatement*
UntypedConverter::convertUntypedModuleDeclaration (SgUntypedModuleDeclaration* ut_module, SgScopeStatement* scope)
{
  // This function builds a class declaration and definition 
  // (both the defining and nondefining declarations as required).

     std::string name = ut_module->get_name();

  // This is the class definition (the fileInfo is the position of the opening brace)
     SgClassDefinition* classDefinition = new SgClassDefinition();
     assert(classDefinition != NULL);

     setSourcePositionFrom(classDefinition, ut_module);

  // May be case insensitive (Fortran)
     classDefinition->setCaseInsensitive(pCaseInsensitive);

  // This is the defining declaration for the class (with a reference to the class definition)
     SgModuleStatement* classDeclaration = new SgModuleStatement(name.c_str(),SgClassDeclaration::e_struct,NULL,classDefinition);
     assert(classDeclaration != NULL);

     setSourcePositionFrom(classDeclaration, ut_module);

  // Set the defining declaration in the defining declaration!
     classDeclaration->set_definingDeclaration(classDeclaration);

  // Set the non defining declaration in the defining declaration (both are required)
     SgModuleStatement* nondefiningClassDeclaration = new SgModuleStatement(name.c_str(),SgClassDeclaration::e_struct,NULL,NULL);
     assert(classDeclaration != NULL);

     setSourcePositionFrom(nondefiningClassDeclaration, ut_module);

  // DQ (3/4/2013): Set the firstNondefiningDeclaration declaration in the firstNondefiningDeclaration.
     ROSE_ASSERT(nondefiningClassDeclaration->get_firstNondefiningDeclaration() == NULL);
     nondefiningClassDeclaration->set_firstNondefiningDeclaration(nondefiningClassDeclaration);

     ROSE_ASSERT(nondefiningClassDeclaration->get_firstNondefiningDeclaration() != NULL);

  // Liao 10/30/2009. we now ask for explicit creation of SgClassType. The constructor will not create it by default
     if (nondefiningClassDeclaration->get_type () == NULL) {
        nondefiningClassDeclaration->set_type (SgClassType::createType(nondefiningClassDeclaration));
     }
     classDeclaration->set_type(nondefiningClassDeclaration->get_type());

  // Set the internal reference to the non-defining declaration
     classDeclaration->set_firstNondefiningDeclaration(nondefiningClassDeclaration);

  // Set the parent explicitly
     nondefiningClassDeclaration->set_parent(scope);

  // Set the defining and non-defining declarations in the non-defining class declaration!
     nondefiningClassDeclaration->set_firstNondefiningDeclaration(nondefiningClassDeclaration);
     nondefiningClassDeclaration->set_definingDeclaration(classDeclaration);

  // Set the nondefining declaration as a forward declaration!
     nondefiningClassDeclaration->setForward();

  // Don't forget the set the declaration in the definition (IR node constructors are side-effect free!)!
     classDefinition->set_declaration(classDeclaration);

  // set the scope explicitly (name qualification tricks can imply it is not always the parent IR node!)
     classDeclaration->set_scope(scope);
     nondefiningClassDeclaration->set_scope(scope);

  // Set the parent explicitly
     classDeclaration->set_parent(scope);

  // A type should have been build at this point, since we will need it later!
     ROSE_ASSERT(classDeclaration->get_type() != NULL);

  // We use the nondefiningClassDeclaration, though it might be that for Fortran the rules that cause this to be important are not so complex as for C/C++.
     SgClassSymbol* classSymbol = new SgClassSymbol(nondefiningClassDeclaration);

  // Add the symbol to the current scope (the specified input scope)
     scope->insert_symbol(name,classSymbol);

     ROSE_ASSERT(scope->lookup_class_symbol(name) != NULL);

  // some error checking
     assert(classDeclaration->get_definingDeclaration() != NULL);
     assert(classDeclaration->get_firstNondefiningDeclaration() != NULL);
     assert(classDeclaration->get_definition() != NULL);

     ROSE_ASSERT(classDeclaration->get_definition()->get_parent() != NULL);

     scope->append_statement(classDeclaration);
     classDeclaration->set_parent(scope);

     SageBuilder::pushScopeStack(classDeclaration->get_definition());

     return classDeclaration;
}

SgProgramHeaderStatement*
UntypedConverter::convertUntypedProgramHeaderDeclaration (SgUntypedProgramHeaderDeclaration* ut_program, SgScopeStatement* scope)
{
   ROSE_ASSERT(scope->variantT() == V_SgGlobal);

   SgUntypedNamedStatement* ut_program_end_statement = ut_program->get_end_statement();
   ROSE_ASSERT(ut_program_end_statement != NULL);

   SgName programName = ut_program->get_name();

   if (programName.get_length() == 0)
      {
      // This program has no program-stmt; indicate this by well-known name
      // TODO - assert that this is a Fortran program
         programName = ROSE_IMPLICIT_FORTRAN_PROGRAM_NAME;
      }

// We should test if this is in the function type table, but do this later?
   SgFunctionType* type = new SgFunctionType(SgTypeVoid::createType(), false);

   SgProgramHeaderStatement* programDeclaration = new SgProgramHeaderStatement(programName, type, NULL);

// A Fortran program has no non-defining declaration (assume same for other languages)
   programDeclaration->set_definingDeclaration(programDeclaration);

   programDeclaration->set_scope(scope);
   programDeclaration->set_parent(scope);

 // Add the program declaration to the global scope
   SgGlobal* globalScope = isSgGlobal(scope);
   ROSE_ASSERT(globalScope != NULL);
   globalScope->append_statement(programDeclaration);

// A symbol using this name should not already exist
   ROSE_ASSERT(!globalScope->symbol_exists(programName));

// Add a symbol to the symbol table in global scope
   SgFunctionSymbol* symbol = new SgFunctionSymbol(programDeclaration);
   globalScope->insert_symbol(programName, symbol);

   SgBasicBlock* programBody = new SgBasicBlock();
   SgFunctionDefinition* programDefinition = new SgFunctionDefinition(programDeclaration, programBody);

// May be case insensitive (Fortran)
   programBody->setCaseInsensitive(pCaseInsensitive);
   programDefinition->setCaseInsensitive(pCaseInsensitive);

   SageBuilder::pushScopeStack(programDefinition);
   SageBuilder::pushScopeStack(programBody);

   programBody->set_parent(programDefinition);
   programDefinition->set_parent(programDeclaration);

   setSourcePositionFrom(programDeclaration, ut_program);
// TODO - see if param list unknown is ok (as there is no param list
// setSourcePositionFrom(programDeclaration->get_parameterList(), ut_program);
   setSourcePositionUnknown(programDeclaration->get_parameterList());

// Make sure there are no function modifiers (just checking, not expected to happen)
   SgUntypedExprListExpression* ut_modifiers = ut_program->get_modifiers();
   ROSE_ASSERT(ut_modifiers);
   ROSE_ASSERT(ut_modifiers->get_expressions().size() == 0);

// Convert the labels for the program begin and end statements
   convertLabel(ut_program,               programDeclaration, SgLabelSymbol::e_start_label_type, /*label_scope=*/ programDefinition);
   convertLabel(ut_program_end_statement, programDeclaration, SgLabelSymbol::e_end_label_type,   /*label_scope=*/ programDefinition);

// Set the end statement name if it exists
   if (ut_program_end_statement->get_statement_name().empty() != true)
      {
         programDeclaration->set_named_in_end_statement(true);
      }

#if THIS_PART_NEEDS_TO_IMPLEMENT_NO_PROGRAM_STATEMENT
        ROSE_ASSERT(programDeclaration->get_parameterList() != NULL);

        if (programKeyword != NULL)
        {
            setSourcePosition(programDeclaration, programKeyword);
            setSourcePosition(programDeclaration->get_parameterList(), programKeyword);
        }
        else
        {
           // These will be marked as isSourcePositionUnavailableInFrontend = true and isOutputInCodeGeneration = true

           // DQ (12/18/2008): These need to make marked with a valid file id (not NULL_FILE, internally),
           // so that any attached comments and CPP directives will be properly attached.
              setSourcePosition(programDeclaration, tokenList);
              setSourcePosition(programDeclaration->get_parameterList(), tokenList);
        }

        setSourcePosition(programDefinition, tokenList);
        setSourcePosition(programBody, tokenList);
#endif

//TODO - the start for both of these should be the first statement in the program (if non-empty)
//TODO - perhaps the end of the block could be the last statement in the program
//TODO - look at C for the answer (original front-end looks suspicious)
   setSourcePositionIncluding(programDefinition, ut_program, ut_program_end_statement);
   setSourcePositionIncluding(programBody,       ut_program, ut_program_end_statement);

#if 0
   if (programDeclaration->get_program_statement_explicit() == false)
      {
         // The function declaration should be forced to match the "end" keyword.
         // Reset the declaration to the current filename.
         //FIXME-no this      programDeclaration->get_startOfConstruct()->set_filenameString(p_source_file->getFileName());
         //FIXME              programDeclaration->get_endOfConstruct()->set_filenameString(p_source_file->getFileName());
      }
#endif

#if DEBUG_UNTYPED_CONVERTER
   cout << "--- finished building program " << programDeclaration->get_name() << endl;
#endif

   ROSE_ASSERT(programBody == SageBuilder::topScopeStack());
   ROSE_ASSERT(programDeclaration->get_firstNondefiningDeclaration() == NULL);

// Untyped types are not traversed so delete them when finished.
   if (ut_program->get_type())
      {
         delete ut_program->get_type();
         ut_program->set_type(NULL);
      }

   return programDeclaration;
}

SgProcedureHeaderStatement*
UntypedConverter::convertUntypedSubroutineDeclaration (SgUntypedSubroutineDeclaration* ut_function, SgScopeStatement* scope)
   {
      SgName name = ut_function->get_name();

      SgFunctionType* functionType = new SgFunctionType(SgTypeVoid::createType(), false);

   // Note that a ProcedureHeaderStatement is derived from a SgFunctionDeclaration (and is Fortran specific).
      SgProcedureHeaderStatement* subroutineDeclaration = new SgProcedureHeaderStatement(name, functionType, NULL);

      setSourcePositionFrom(subroutineDeclaration,                      ut_function);
//TODO - for now (param_list should have its own source position
      setSourcePositionFrom(subroutineDeclaration->get_parameterList(), ut_function);

   // Mark this as a subroutine.
      subroutineDeclaration->set_subprogram_kind( SgProcedureHeaderStatement::e_subroutine_subprogram_kind );

   // TODO - suffix
      printf ("...TODO... convert suffix\n");

      buildProcedureSupport(ut_function, subroutineDeclaration, scope);

      return subroutineDeclaration;
   }


SgFunctionDeclaration*
UntypedConverter::convertUntypedFunctionDeclaration (SgUntypedFunctionDeclaration* ut_function, SgScopeStatement* scope)
{
   SgFunctionDeclaration* function_decl;
   SgFunctionDefinition*  function_def;
   SgBasicBlock* function_body;

   isConvertingFunctionDecl = true;
   isDefiningDeclaration    = false;

   SgUntypedType* ut_type = ut_function->get_type();
   SgType*    return_type = convertUntypedType(ut_type, scope);

   //cout << "---                 : rtn type is " << return_type->class_name() << endl;

   SgUntypedNamedStatement* end_stmt = ut_function->get_end_statement();
   isDefiningDeclaration = (end_stmt->get_statement_enum() == General_Language_Translation::e_end_proc_def_stmt);

   //cout << "---                 : end enum is " << end_stmt->get_statement_enum() << endl;
   //cout << "---                 : is defd  is " << isDefiningDeclaration << endl;

// Finished with untyped type so delete it ???
// TODO (this should be done by destructors in ROSETTA)
// delete ut_type;

//   SgType* sg_temp_param_type = SgTypeUnknown::createType();
//   SgInitializedNamePtrList*     sg_param_names = convertUntypedInitializedNameList(ut_param_names, sg_temp_param_type);

// Build empty parameter list, need to add initialized names
   SgUntypedInitializedNameList* ut_params = ut_function->get_parameters();
   ROSE_ASSERT(ut_params);

   SgFunctionParameterList* param_list = SageBuilder::buildFunctionParameterList();
   ROSE_ASSERT(param_list);
   setSourcePositionFrom(param_list, ut_params);

   SgUntypedInitializedNamePtrList & ut_names = ut_params->get_name_list();

   BOOST_FOREACH(SgUntypedInitializedName* ut_name, ut_names)
      {
         //cout << "---                 : arg name is " << ut_name->get_name() << endl;

         SgUntypedType* ut_type = ut_name->get_type();
         ROSE_ASSERT(ut_type);

         SgUntypedExprListExpression* ut_modifiers = ut_type->get_modifiers();
         ROSE_ASSERT(ut_modifiers);

         SgUntypedExpression* ut_param_binding = ut_modifiers->get_expressions()[0];
         ROSE_ASSERT(ut_param_binding);

#if 0
         cout << "---                 : mod list is " << ut_modifiers << endl;
         cout << "---                 :     size is " << ut_modifiers->get_expressions().size() << endl;
         cout << "---                 :     expr is " << ut_param_binding << endl;
#endif

         bool isOutParam = false;
         if (ut_modifiers->get_expressions().size() == 2) {
            SgUntypedExpression* ut_out_param = ut_modifiers->get_expressions()[1];
            //ROSE_ASSERT(ut_out_param->get_expression_enum() == General_Language_Translation::e_type_modifier_intent_out);
            delete ut_out_param;
            isOutParam = true;
         }

      // Temporarily
      // ROSE_ASSERT(ut_param_binding->get_expression_enum() == General_Language_Translation::e_unknown);
      // DELETE FOR NOW
         delete ut_param_binding;

         SgType* sg_type = convertUntypedType(ut_name, scope, /*delete_ut_type*/true);
         ROSE_ASSERT(sg_type);
         //cout << "---                 : arg type is " << sg_type->class_name() << endl;

      // TODO - fix/overload convertUntypedInitializedName so that it doesn't take a type
      // SgInitializedName* sg_name = convertUntypedInitializedName(ut_name, sg_base_type);

         SgInitializedName* sg_name = SageBuilder::buildInitializedName(ut_name->get_name(), sg_type /*, sg_init*/);
         ROSE_ASSERT(sg_name);
         setSourcePositionFrom(sg_name, ut_name);

      // TODO - This is a hack, figure out how to set intent out for a parameter
      //        For now use storage modifier e_mutable
         if (isOutParam) {
            SgStorageModifier & modifier = sg_name->get_storageModifier();
            modifier.setMutable();
#if 0
            cout << "---                 : modifier is " << modifier.isMutable() << endl;
            cout << "---                 : modifier is " <<  sg_name->get_storageModifier().isMutable() << endl;
#endif
         }

         param_list->get_args().push_back(sg_name);
      }

   SgName name = ut_function->get_name();

   if (isDefiningDeclaration == false)
      {
         function_decl = SageBuilder::buildNondefiningFunctionDeclaration(name, return_type, param_list, scope);
         ROSE_ASSERT(function_decl);
         setSourcePositionFrom(function_decl, ut_function);
      }
   else
      {
         function_decl = SageBuilder::buildDefiningFunctionDeclaration(name, return_type, param_list, scope);
         ROSE_ASSERT(function_decl);
         setSourcePositionFrom(function_decl, ut_function);

         function_body = new SgBasicBlock();
         ROSE_ASSERT(function_body);
         setSourcePositionFrom(function_body, ut_function->get_scope());

         function_def = new SgFunctionDefinition(function_decl, function_body);
         ROSE_ASSERT(function_def);
         setSourcePositionFrom(function_def, ut_function->get_scope());

         function_def->set_parent(function_decl);
         function_def->set_body(function_body);
         function_body->set_parent(function_def);

         ROSE_ASSERT(function_decl->get_definition() != NULL);

      // May be case insensitive (Fortran and Jovial)
         function_body->setCaseInsensitive(pCaseInsensitive);
         function_def ->setCaseInsensitive(pCaseInsensitive);

#if 0
         cout << "---                 : function_def "  << function_decl->get_definition() << endl;
         cout << "---                 : function_def "  << function_def << endl;
         cout << "---                 : function_body " << function_body << endl;
#endif
      }

   if (isDefiningDeclaration == true)
      {
         SageBuilder::pushScopeStack(function_def);
         SageBuilder::pushScopeStack(function_body);
      }

   //cout << "---                 : function_decl " << function_decl << endl;

   SageInterface::appendStatement(function_decl, scope);

   return function_decl;
}

SgFunctionDeclaration*
UntypedConverter::convertUntypedFunctionDeclaration (SgUntypedFunctionDeclaration* ut_function, SgNodePtrList& children, SgScopeStatement* scope)
{
   SgFunctionDeclaration* function_decl;

   ROSE_ASSERT(children.size() == 4);

   SgName name = ut_function->get_name();

   SgFunctionSymbol* prev_func_sym = SageInterface::lookupFunctionSymbolInParentScopes(name, scope);
   ROSE_ASSERT(prev_func_sym);
   SgFunctionDeclaration* prev_nondef_func_decl = prev_func_sym->get_declaration();
   ROSE_ASSERT(prev_nondef_func_decl);

#if 0
   cout << "---                             : " << children[0] << endl;
   cout << "---                             : " << children[1] << endl;
   cout << "---                             : " << children[2] << endl;
   cout << "---                             : " << children[3] << endl;

   cout << "---                             : " << name << endl;
   cout << "---                             : " << prev_func_sym << endl;
   cout << "---                             : " << prev_nondef_func_decl->get_name() << endl;
   cout << "---              prev func decl : " << prev_nondef_func_decl << endl;
   cout << "---                             : " << name << endl;
   cout << "---                 : function_decl " << prev_nondef_func_decl << endl;
   cout << "---                 : function_def "  << prev_nondef_func_decl->get_definition() << endl;
#endif

   SgFunctionParameterList* prev_params = prev_nondef_func_decl->get_parameterList();
   ROSE_ASSERT(prev_params);
   SgInitializedNamePtrList & prev_names = prev_params->get_args();

   //cout << "---                   # args is : " << prev_names.size() << endl;

   SgFunctionParameterList* param_list = SageBuilder::buildFunctionParameterList();
   ROSE_ASSERT(param_list);
   setSourcePositionFrom(param_list, prev_params);

   //cout << "---             prev_param_list : " << prev_params << endl;
   //cout << "---                  param_list : " << param_list << endl;

   BOOST_FOREACH(SgInitializedName* prev_name, prev_names)
      {
         SgName arg_name = prev_name->get_name();

         //cout << "---                 arg name is : " << arg_name << endl;

         SgVariableSymbol* variable_sym = scope->lookup_variable_symbol(arg_name);
         //cout << "---                             : " << variable_sym << endl;
         SgInitializedName* variable_decl = variable_sym->get_declaration();
         //cout << "---                             : " << variable_decl->get_name() << endl;
         SgType* variable_type = variable_decl->get_typeptr();
         //cout << "---                             : " << variable_type->class_name() << endl;

#if 0
         SgUntypedExprListExpression* ut_modifiers = ut_type->get_modifiers();
         ROSE_ASSERT(ut_modifiers);
         ROSE_ASSERT(ut_modifiers->get_expressions().size() == 1);

         SgUntypedExpression* ut_param_binding = ut_modifiers->get_expressions()[0];
         ROSE_ASSERT(ut_param_binding);

         //cout << "---                 : mod list is " << ut_modifiers << endl;
         //cout << "---                 :     size is " << ut_modifiers->get_expressions().size() << endl;
         //cout << "---                 :     expr is " << ut_param_binding << endl;

      // Temporarily
         ROSE_ASSERT(ut_param_binding->get_expression_enum() == General_Language_Translation::e_unknown);
      // DELETE FOR NOW
         delete ut_param_binding;

         SgType* sg_type = convertUntypedType(ut_name, scope, /*delete_ut_type*/true);
         ROSE_ASSERT(sg_type);
#if 0
         cout << "---                 : arg type is " << sg_type->class_name()
              << endl;
#endif

      // TODO - fix/overload convertUntypedInitializedName so that it doesn't take a type
      // SgInitializedName* sg_name = convertUntypedInitializedName(ut_name, sg_base_type);

#endif

         SgInitializedName* param_name = SageBuilder::buildInitializedName(arg_name, variable_type, NULL/*sg_init*/);
         ROSE_ASSERT(param_name);
//       setSourcePositionFrom(param_name, arg_name);

         if (prev_name->get_storageModifier().isMutable())
            {
               //cout << "---               MUTABLE OUT VARIABLE FOUND\n";
               param_name->get_storageModifier().setMutable();
            }

         param_list->get_args().push_back(param_name);
      }

#if 0
   SgInitializedNamePtrList & new_names = param_list->get_args();
   cout << "---                   # args is : " << new_names.size() << endl;
#endif

// return type
   SgType* return_type = prev_nondef_func_decl->get_type()->get_return_type();

   if (isDefiningDeclaration == false)
      {
         function_decl = SageBuilder::buildNondefiningFunctionDeclaration(name, return_type, param_list, scope);
         ROSE_ASSERT(function_decl);
         setSourcePositionFrom(function_decl, ut_function);

      // cleanup old function declaration
         prev_nondef_func_decl->set_definition(NULL);
         SageInterface::replaceStatement(prev_nondef_func_decl, function_decl);
      }
   else
      {
      // function body
         SgBasicBlock* function_body = isSgBasicBlock(scope);
         //cout << "---            function body is : " << function_body << endl;
         ROSE_ASSERT(function_body);

      // function definition
         SgFunctionDefinition* function_def = isSgFunctionDefinition(function_body->get_parent());
         //cout << "---      function definition is : " << function_def << endl;
         ROSE_ASSERT(function_def);

      // previous function declaration
         SgFunctionDeclaration* prev_func_decl = isSgFunctionDeclaration(function_def->get_parent());
#if 0
         cout << "---      function definition is : " << function_def << endl;
         cout << "---   previous function decl is : " << prev_func_decl << endl;
         cout << "---   previous function name is : " << prev_func_decl->get_name() << endl;
#endif
         ROSE_ASSERT(prev_func_decl);
         ROSE_ASSERT(name == prev_func_decl->get_name());

      // function scope (parent)
         SgScopeStatement* function_scope = isSgScopeStatement(prev_func_decl->get_parent());
         ROSE_ASSERT(function_scope);

         function_decl = SageBuilder::buildDefiningFunctionDeclaration(name, return_type, param_list, function_scope);
         ROSE_ASSERT(function_decl);
         setSourcePositionFrom(function_decl, ut_function);

      // setup new function declaration
         function_decl->set_definition(function_def);
         function_body->set_parent(function_def);
         function_def ->set_parent(function_decl);
         function_def ->set_body(function_body);

         ROSE_ASSERT(function_decl->get_definition() != NULL);

      // May be case insensitive (Fortran and Jovial)
         function_body->setCaseInsensitive(pCaseInsensitive);
         function_def ->setCaseInsensitive(pCaseInsensitive);

      // cleanup old function declaration
         prev_func_decl->set_definition(NULL);
         SageInterface::replaceStatement(prev_func_decl, function_decl);

      // restore scope stack
         SageBuilder::popScopeStack();  // procedure body
         SageBuilder::popScopeStack();  // procedure definition
      }

// restore state of function conversion
   isDefiningDeclaration    = false;
   isConvertingFunctionDecl = false;

   return function_decl;
}


SgProcedureHeaderStatement*
UntypedConverter::convertUntypedBlockDataDeclaration (SgUntypedBlockDataDeclaration* ut_block_data, SgScopeStatement* scope)
   {
   // This is implemented in UntypedFortranConverter subclass.  Is there any need for it here?
      ROSE_ASSERT(0);

   // The block data statement is implemented to build a function (which initializes data)
   // Note that it can be declared with the "EXTERNAL" statement and as such it works much
   // the same as any other procedure.

      SgName name = ut_block_data->get_name();
      SgFunctionType* functionType = new SgFunctionType(SgTypeVoid::createType(), false);

   // TODO - take better care of instance when there is no name
   // TODO - which begs the question of what to do with duplicate symbols and looking them up
   // TODO - implement symbol lookup
      if (name.get_length() == 0) {
         std::cout << "...TODO... WARNING: block data name is UNKNOWN" << std::endl;
         name = "Block_Data_Name_UNKNOWN";
      }

   // Note that a ProcedureHeaderStatement is derived from a SgFunctionDeclaration (and is Fortran specific).
   // The SgProcedureHeaderStatement can be used for a Fortran function, subroutine, or block data declaration.

      SgProcedureHeaderStatement* blockDataDeclaration = new SgProcedureHeaderStatement(name, functionType, NULL);

   // TODO - this should be only BlockDataStmt (or exclude decl_list)
      setSourcePositionFrom(blockDataDeclaration,                      ut_block_data);
      setSourcePositionFrom(blockDataDeclaration->get_parameterList(), ut_block_data);

      blockDataDeclaration->set_subprogram_kind(SgProcedureHeaderStatement::e_block_data_subprogram_kind);

//    bool hasDummyArgList = false;
//    buildProcedureSupport(ut_block_data, blockDataDeclaration, hasDummyArgList);

   // This will be the defining declaration
      blockDataDeclaration->set_definingDeclaration(blockDataDeclaration);
      blockDataDeclaration->set_firstNondefiningDeclaration(NULL);

      SgScopeStatement* currentScopeOfFunctionDeclaration = scope;
      ROSE_ASSERT(currentScopeOfFunctionDeclaration != NULL);

      currentScopeOfFunctionDeclaration->append_statement(blockDataDeclaration);

   // See if this was previously declared
   // Assume NULL for now, does it even need a symbol?
  //     SgFunctionSymbol* functionSymbol = SageInterface::lookupFunctionSymbolInParentScopes (procedureDeclaration->get_name(), scope);

      SgFunctionSymbol* functionSymbol = new SgFunctionSymbol(blockDataDeclaration);
      currentScopeOfFunctionDeclaration->insert_symbol(blockDataDeclaration->get_name(), functionSymbol);

      SgBasicBlock*         blockDataBody       = SageBuilder::buildBasicBlock();
      SgFunctionDefinition* blockDataDefinition = new SgFunctionDefinition(blockDataDeclaration, blockDataBody);

      ROSE_ASSERT(blockDataBody);
      ROSE_ASSERT(blockDataDefinition);

      setSourcePositionFrom(blockDataDefinition, ut_block_data);
      setSourcePositionFrom(blockDataBody,       ut_block_data->get_declaration_list());

      ROSE_ASSERT(blockDataDeclaration->get_definition() != NULL);

   // May be case insensitive (Fortran)
      blockDataDefinition->setCaseInsensitive(pCaseInsensitive);
      blockDataDeclaration->set_scope (currentScopeOfFunctionDeclaration);
      blockDataDeclaration->set_parent(currentScopeOfFunctionDeclaration);

      SageBuilder::pushScopeStack(blockDataDefinition);
      SageBuilder::pushScopeStack(blockDataBody);

   // Convert the labels for the program begin and end statements
      convertLabel(ut_block_data,                      blockDataDeclaration, SgLabelSymbol::e_start_label_type, /*label_scope=*/ blockDataDefinition);
      convertLabel(ut_block_data->get_end_statement(), blockDataDeclaration, SgLabelSymbol::e_end_label_type,   /*label_scope=*/ blockDataDefinition);

   // Set the end statement name if it exists
      if (ut_block_data->get_end_statement()->get_statement_name().empty() != true)
         {
            blockDataDeclaration->set_named_in_end_statement(true);
         }

   // TODO - implement conversion of decl_list

      ROSE_ASSERT(functionType->get_arguments().empty() == true);

      blockDataBody->set_parent(blockDataDefinition);
      blockDataDefinition->set_parent(blockDataDeclaration);

      ROSE_ASSERT(blockDataDeclaration->get_parameterList() != NULL);

      return blockDataDeclaration;
   }

SgBasicBlock*
UntypedConverter::convertUntypedBlockStatement (SgUntypedBlockStatement* ut_block_stmt, SgScopeStatement* scope)
{
   SgBasicBlock* sg_basic_block = SageBuilder::buildBasicBlock();
   ROSE_ASSERT(sg_basic_block);

#if 0
   cout << "--- UntypedConverter:: SgBasicBlock: " << sg_basic_block << endl;
   cout << "--- UntypedConverter:: current scope is : " << scope << endl;
   cout << "--- UntypedConverter:: parent is : " << sg_basic_block->get_parent() << endl;
   // I think this is the containing scope based on the parent.
   //   cout << "--- UntypedConverter:: scope  is : " << sg_basic_block->get_scope() << endl;
#endif

// I don't understand this (setting parent breaks Jovial compound statement)
// Because part of a block of another statement (e.g., while statement)
#if 1
   if (isSgBasicBlock(scope) && sg_basic_block->get_parent() == NULL)
      {
         SgBasicBlock* parent = isSgBasicBlock(scope);
         sg_basic_block->set_parent(parent);
         SageInterface::appendStatement(sg_basic_block, parent);
         cout << "--- UntypedConverter:: WARNING: parent was NULL is : " << sg_basic_block->get_parent() << endl;
      }
#endif

// Who sets the parent of this?
// This definitely needs more work.  Currently hacked to work with case/switch statements I believe
   SageBuilder::pushScopeStack(sg_basic_block);

   return sg_basic_block;
}

SgUseStatement*
UntypedConverter::convertUntypedUseStatement (SgUntypedUseStatement* ut_use_stmt, SgScopeStatement* scope)
{
   std::string name = ut_use_stmt->get_module_name();

   SgUseStatement* sg_use_stmt = new SgUseStatement(name, /*only_option*/false);
   ROSE_ASSERT(sg_use_stmt);
   setSourcePositionFrom(sg_use_stmt, ut_use_stmt);

   SageInterface::appendStatement(sg_use_stmt, scope);

   return sg_use_stmt;
}

//TODO-WARNING: This needs help!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
SgVariableDeclaration*
UntypedConverter::convertUntypedVariableDeclaration (SgUntypedVariableDeclaration* ut_decl, SgScopeStatement* scope)
{
   ROSE_ASSERT(ut_decl != NULL);
   ROSE_ASSERT(scope   != NULL);

   ROSE_ASSERT(scope->variantT() == V_SgBasicBlock || scope->variantT() == V_SgClassDefinition
            || scope->variantT() == V_SgGlobal);  // global scope used for Jovial

   SgUntypedDeclarationStatement* base_type_decl = NULL;

   SgUntypedType* ut_type = ut_decl->get_type();

   cout << "-x- convertUntypedVariableDeclaration:  ut_type enum is " << ut_type->get_type_enum_id() << ": e_table is " << SgUntypedType::e_table << endl;

   if (ut_decl->get_has_base_type()) {
      // This is an anonymous type declaration, convert it first so that there will be a type in the symbol table
      // Normally the type declaration would come before it is used in the variable declaration.  But in this
      // case we need to get the type declaration, convert it, then remove it from the variable declaration so that it isn't
      // subsequently traversed on the way down.
      base_type_decl = ut_decl->get_base_type_declaration();

      cout << "-x- convertUntypedVariableDeclaration: variable decl has base type declaration: "
           << base_type_decl
           << endl;

// skip for now to see if structure declaration is traversed
// perhaps can get it on the rebound ...
      return NULL;
   }

   SgType* sg_type = convertUntypedType(ut_type, scope);
   ROSE_ASSERT(sg_type != NULL);

   SgUntypedInitializedNamePtrList ut_vars = ut_decl->get_variables()->get_name_list();
   SgUntypedInitializedNamePtrList::const_iterator it;

#if 0
   cout << "--- convertUntypedVariableDeclaration:  # vars is " << ut_vars.size() << endl;
   cout << "--- convertUntypedVariableDeclaration: ut_decl is " << ut_decl << ": " << ut_decl->class_name() << endl;
   cout << "--- convertUntypedVariableDeclaration: ut_type is " << ut_type << ": " << ut_type->class_name() << endl;
   cout << "--- convertUntypedVariableDeclaration: sg_type is " << sg_type << ": " << sg_type->class_name() << endl;
#endif

   SgInitializedNamePtrList sg_name_list;

// Declare the first variable
#if 0
//TODO - not sure this is correct and is ackward anyway as it would be nice to create a variable declaration
// without any variables and then add them all later.
   SgVariableDeclaration* sg_decl = SageBuilder::buildVariableDeclaration_nfi(name(), sg_type, /*sg_init*/NULL, scope);
#else
   SgVariableDeclaration* sg_decl = new SgVariableDeclaration();
   ROSE_ASSERT(sg_decl);
   setSourcePositionFrom(sg_decl, ut_decl);

   sg_decl->set_parent(scope);
   //   sg_decl->set_definingDeclaration(sg_decl);
   sg_decl->set_firstNondefiningDeclaration(sg_decl);
#endif

// add variables
   BOOST_FOREACH(SgUntypedInitializedName* ut_init_name, ut_vars)
   {
         // TODO
         //   1. initializer
         //   2. CharLength: SgTypeString::createType(charLenExpr, typeKind)
         //   3. ArraySpec: buildArrayType
         //   4. CoarraySpec: buildArrayType with coarray attribute
         //   5. Pointers: new SgPointerType(sg_type)

      SgInitializedName* sg_init_name = convertUntypedInitializedName(ut_init_name, sg_type);
      SgName var_name = sg_init_name->get_name();

#if 0
      cout << "--- convertUntypedVariableDeclaration: var name is " << ut_init_name->get_name() << endl;
      cout << "--- convertUntypedVariableDeclaration: has init is " << ut_init_name->get_has_initializer() << endl;
      cout << "--- convertUntypedVariableDeclaration:  ut_type is " << ut_init_name->get_type()->class_name() << endl;
      cout << "--- convertUntypedVariableDeclaration:  sg_type is " << sg_init_name->get_type()->class_name() << endl;
#endif

// DELETE-ME: setting the initializer has been moved to convertUntypedInitializedName
#if 0
      if (ut_init_name->get_has_initializer())
      {
         SgUntypedExpression* ut_init_expr = ut_init_name->get_initializer();
         SgExpression* sg_init_expr = convertUntypedExpression(ut_init_expr);
         ROSE_ASSERT(sg_init_expr);

         SgAssignInitializer* sg_initializer = new SgAssignInitializer(sg_init_expr, sg_init_expr->get_type());
         ROSE_ASSERT(sg_initializer);
         setSourcePositionFrom(sg_initializer, ut_init_expr);

         sg_init_name->set_initializer(sg_initializer);
         sg_initializer->set_parent(sg_init_name);

      // The initialization expression isn't traversed so it needs to be deleted
         delete ut_init_expr;
      }
#endif

   // Finished with the untyped initialized name and associated types.
   // The untyped initialized name will be deleted after the traversal, delete types now.
      //      if (ut_init_name->get_type() != ut_type) delete ut_init_name->get_type();
      //      ut_init_name->set_type(NULL);
      //      delete ut_type;

      sg_init_name->set_declptr(sg_decl);
      sg_decl->append_variable(sg_init_name, sg_init_name->get_initializer());

      SgVariableSymbol* variableSymbol = NULL;
      SgFunctionDefinition* functionDefinition = SageInterface::getEnclosingProcedure(scope);
      if (functionDefinition != NULL)
      {
      // Check in the function definition for an existing symbol
         variableSymbol = functionDefinition->lookup_variable_symbol(var_name);
         if (variableSymbol != NULL)
         {
            cout << "--- but variable symbol is _NOT_ NULL for " << var_name << endl;

         // This variable symbol has already been placed into the function definition's symbol table
         // Link the SgInitializedName in the variable declaration with its entry in the function parameter list.
            sg_init_name->set_prev_decl_item(variableSymbol->get_declaration());
         // Set the referenced type in the function parameter to be the same as that in the declaration being processed.
            variableSymbol->get_declaration()->set_type(sg_init_name->get_type());
         // Function parameters are in the scope of the function definition (same for C/C++)
            sg_init_name->set_scope(functionDefinition);
         }
      }

      if (variableSymbol == NULL)
      {
      // Check the current scope
         variableSymbol = scope->lookup_variable_symbol(var_name);

         sg_init_name->set_scope(scope);
         if (variableSymbol == NULL)
         {
            variableSymbol = new SgVariableSymbol(sg_init_name);
            scope->insert_symbol(var_name, variableSymbol);
            ROSE_ASSERT (sg_init_name->get_symbol_from_symbol_table () != NULL);
         }
      }
      ROSE_ASSERT(variableSymbol != NULL);
      ROSE_ASSERT(sg_init_name->get_scope() != NULL);
   }

// Modifiers must be set after the variable declaration setup is finished
   setDeclarationModifiers(sg_decl, ut_decl->get_modifiers());

   scope->append_statement(sg_decl);
   convertLabel(ut_decl, sg_decl, scope);

   //        SgInitializedNamePtrList& varList = varDecl->get_variables ();
   //        SgInitializedName* firstInitializedNameForSourcePosition = varList.front();
   //        SgInitializedName* lastInitializedNameForSourcePosition = varList.back();
   //        ROSE_ASSERT(DeclAttributes.getDeclaration()->get_startOfConstruct() != NULL);
   //        ROSE_ASSERT(firstInitializedNameForSourcePosition->get_startOfConstruct() != NULL);
   //        ROSE_ASSERT(lastInitializedNameForSourcePosition->get_startOfConstruct() != NULL);
   //        *(DeclAttributes.getDeclaration()->get_startOfConstruct()) = *(firstInitializedNameForSourcePosition->get_startOfConstruct());
   //        *(DeclAttributes.getDeclaration()->get_endOfConstruct()) = *(lastInitializedNameForSourcePosition->get_startOfConstruct());
   //        DeclAttributes.reset();

   return sg_decl;
}

SgVariableDeclaration* UntypedConverter::
convertUntypedVariableDeclaration(SgUntypedVariableDeclaration* ut_decl, SgScopeStatement* scope,
                                  std::string base_type_name, std::string var_name, SgInitializer* var_initializer)
{
   // Only a Jovial specific implementation needed for now
      cerr << "UntypedConverter::convertUntypedVariableDeclaration (with base_type_name) must be implemented in derived class if needed"
           << endl;
      ROSE_ASSERT(0);

      return NULL;
}

SgImplicitStatement*
UntypedConverter::convertUntypedImplicitDeclaration(SgUntypedImplicitDeclaration* ut_decl, SgScopeStatement* scope)
   {
   // Only a Fortran specific implementation needed for now
      cerr << "UntypedConverter::convertUntypedImplicitDeclaration must be implemented in derived class if needed" << endl;
      ROSE_ASSERT(0);

      return NULL;
   }

SgDeclarationStatement*
UntypedConverter::convertUntypedNameListDeclaration (SgUntypedNameListDeclaration* ut_decl, SgScopeStatement* scope)
   {
      SgUntypedNamePtrList ut_names = ut_decl->get_names()->get_name_list();

      switch (ut_decl->get_statement_enum())
        {
        case SgToken::FORTRAN_IMPORT:
           {
              SgImportStatement* importStatement = new SgImportStatement();
              setSourcePositionFrom(importStatement, ut_decl);

              importStatement->set_definingDeclaration(importStatement);
              importStatement->set_firstNondefiningDeclaration(importStatement);

              SgExpressionPtrList localList;

              BOOST_FOREACH(SgUntypedName* ut_name, ut_names)
              {
                 SgName name = ut_name->get_name();
                 std::cout << "... IMPORT name is " << name << std::endl;
                 SgVariableSymbol* variableSymbol = SageInterface::lookupVariableSymbolInParentScopes(name, scope);
                 ROSE_ASSERT(variableSymbol != NULL);

                 SgVarRefExp* variableReference = new SgVarRefExp(variableSymbol);
              // TODO
              // setSourcePositionFrom(variableReference);

                 importStatement->get_import_list().push_back(variableReference);
              }
              SageInterface::appendStatement(importStatement, scope);
           }

        case SgToken::FORTRAN_EXTERNAL:
          {
          // TODO - name seems to need a parent found in get_name sageInterface.c, line 1528
          //      - actually may be attr_spec_stmt without a parent
             SgAttributeSpecificationStatement::attribute_spec_enum attr_enum = SgAttributeSpecificationStatement::e_externalStatement;
             SgAttributeSpecificationStatement* attr_spec_stmt = SageBuilder::buildAttributeSpecificationStatement(attr_enum);
             ROSE_ASSERT(attr_spec_stmt);
             setSourcePositionFrom(attr_spec_stmt, ut_decl);

             BOOST_FOREACH(SgUntypedName* ut_name, ut_names)
             {
                SgName name = ut_name->get_name();

                SgType* type = SageBuilder::buildFortranImplicitType(name);

                SgFunctionRefExp* func_ref = SageBuilder::buildFunctionRefExp(name, type, scope);
                attr_spec_stmt->get_parameter_list()->prepend_expression(func_ref);

#if 0
             // TODO - pick and implement one of these
                SgExpression* parameterExpression = astExpressionStack.front();
                SgFunctionRefExp* functionRefExp = generateFunctionRefExp(nameToken);
                attr_spec_stmt->get_parameter_list()->prepend_expression(parameterExpression);

#endif
             }
             SageInterface::appendStatement(attr_spec_stmt, scope);
             convertLabel(ut_decl, attr_spec_stmt, scope);

             return attr_spec_stmt;
         }
        case General_Language_Translation::e_jovial_compool_stmt:
           {
              return convertUntypedJovialCompoolStatement(ut_decl, scope);
           }

       default:
          {
             cerr << "UntypedConverter::convertUntypedNameListDeclaration: failed to find known statement enum, is "
                  << ut_decl->get_statement_enum() << endl;
             ROSE_ASSERT(0);
          }
       }
   }


SgDeclarationStatement* UntypedConverter::
convertUntypedInitializedNameListDeclaration (SgUntypedInitializedNameListDeclaration* ut_decl, SgScopeStatement* scope)
   {
      SgExpression* sg_var_expr;
      SgAttributeSpecificationStatement* sg_decl = NULL;

      switch (ut_decl->get_statement_enum())
        {
        case General_Language_Translation::e_fortran_dimension_stmt:
           {
              sg_decl = SageBuilder::buildAttributeSpecificationStatement(SgAttributeSpecificationStatement::e_dimensionStatement);
              ROSE_ASSERT(sg_decl != NULL);
              setSourcePositionFrom(sg_decl, ut_decl);

              SgExprListExp* param_list = sg_decl->get_parameter_list();
              SgUntypedInitializedNamePtrList ut_init_names = ut_decl->get_variables()->get_name_list();


              // Why not convert initialized name
              // SgInitializedNamePtrList* convertUntypedInitializedNameList (SgUntypedInitializedNameList* ut_name_list, SgType* sg_base_type);


              BOOST_FOREACH(SgUntypedInitializedName* ut_init_name, ut_init_names)
                 {
                    SgName name = ut_init_name->get_name();

                    cout << "-x- dimension statement name is " << name << endl;

                 // First assume easy case where variable hasn't been declared yet
                    SgVariableSymbol* variableSymbol = SageInterface::lookupVariableSymbolInParentScopes(name, scope);
                    ROSE_ASSERT(variableSymbol == NULL);

                 // Build the declaration for the variable
                    SgType * base_type = SageBuilder::buildFortranImplicitType(name);
                    ROSE_ASSERT(base_type != NULL);

#if 0
                    SgUntypedType* ut_type = ut_init_name->get_type();
                    ROSE_ASSERT(ut_type);
                    cout << "--- DIMENSION:: name is " << name << std::endl;
                    cout << "--- DIMENSION:: init name " << ut_init_name << endl;
                    cout << "--- DIMENSION:: found ut_type " << ut_type << " : " << ut_type->class_name() << std::endl;
                    cout << "--- DIMENSION:: found symbol " << variableSymbol << std::endl;
#endif

                    SgType* sg_type = convertUntypedType(ut_init_name, scope, /*delete_ut_type*/true);

                 // A Fortran DIMENSION statement must be an array type
                    ROSE_ASSERT(isSgArrayType(sg_type));

                    SgVariableDeclaration* sg_var_decl = SageBuilder::buildVariableDeclaration(name, sg_type, NULL, scope);
                    ROSE_ASSERT(sg_var_decl != NULL);

                    SgVarRefExp* sg_var_ref = SageBuilder::buildVarRefExp(sg_var_decl);
                    ROSE_ASSERT(sg_var_ref != NULL);

                    SgArrayType* array_type = dynamic_cast<SgArrayType*>(sg_type);
                    SgExprListExp* dim_info = array_type->get_dim_info();
                    sg_var_expr = SageBuilder::buildPntrArrRefExp(sg_var_ref, dim_info);
                    ROSE_ASSERT(sg_var_expr != NULL);

                 // append variable
                    param_list->append_expression(sg_var_expr);
                 }
              break;
         }

       default:
          {
             cerr << "UntypedConverter::convertUntypedInitializedNameListDeclaration: failed to find known statement enum, is "
                  << ut_decl->get_statement_enum() << endl;
             ROSE_ASSERT(0);
          }
       }

      SageInterface::appendStatement(sg_decl, scope);
      convertLabel(ut_decl, sg_decl, scope);

      return sg_decl;
   }


// Executable statements
//----------------------

SgExprStatement*
UntypedConverter::convertUntypedAssignmentStatement (SgUntypedAssignmentStatement* ut_stmt, SgNodePtrList& children, SgScopeStatement* scope)
   {
      ROSE_ASSERT(children.size() == 2);

      SgExprStatement* assign_stmt = NULL;

      SgExpression* lhs = isSgExpression(children[0]);
      SgExpression* rhs = isSgExpression(children[1]);
      ROSE_ASSERT(lhs != NULL && rhs != NULL);

      SgExpression* assign_expr = new SgAssignOp(lhs, rhs, NULL);
      setSourcePositionIncluding(assign_expr, lhs, rhs);

   // lhs expression now becomes an lvalue
      lhs->set_lvalue(true);

      assign_stmt = SageBuilder::buildExprStatement(assign_expr);
      ROSE_ASSERT(assign_stmt);
      setSourcePositionFrom(assign_stmt, ut_stmt);

      SageInterface::appendStatement(assign_stmt, scope);

      convertLabel(ut_stmt, assign_stmt, scope);

      return assign_stmt;
   }

SgStatement*
UntypedConverter::convertUntypedExpressionStatement (SgUntypedExpressionStatement* ut_stmt, SgNodePtrList& children, SgScopeStatement* scope)
   {
      SgStatement* sg_stmt = NULL;

      ROSE_ASSERT(children.size() == 1);

      SgExpression* sg_expr = isSgExpression(children[0]);
      ROSE_ASSERT(sg_expr != NULL);

      switch (ut_stmt->get_statement_enum())
      {
        case SgToken::FORTRAN_STOP:
          {
             SgStopOrPauseStatement* stop_stmt = new SgStopOrPauseStatement(sg_expr);
             stop_stmt->set_stop_or_pause(SgStopOrPauseStatement::e_stop);
             sg_stmt = stop_stmt;
             break;
          }
        case SgToken::FORTRAN_ERROR_STOP:
          {
             SgStopOrPauseStatement* stop_stmt = new SgStopOrPauseStatement(sg_expr);
             stop_stmt->set_stop_or_pause(SgStopOrPauseStatement::e_error_stop);
             sg_stmt = stop_stmt;
             break;
          }
        default:
          {
             cerr << "UntypedConverter::convertUntypedExpressionStatement: failed to find known statement enum, is "
                  << ut_stmt->get_statement_enum() << endl;
             ROSE_ASSERT(0);
          }
      }
      
      ROSE_ASSERT(sg_stmt != NULL);
      setSourcePositionFrom(sg_stmt, ut_stmt);

   // any IR node can have a parent, it makes sense to associate the expression with the statement
      sg_expr->set_parent(sg_stmt);
      convertLabel(ut_stmt, sg_stmt, scope);

      SageInterface::appendStatement(sg_stmt, scope);

      return sg_stmt;
   }

SgStatement*
UntypedConverter::convertUntypedForStatement (SgUntypedForStatement* ut_stmt, SgNodePtrList& children, SgScopeStatement* scope)
   {
      cerr << "UntypedConverter::convertUntypedForStatement must be implemented in derived class \n";
      ROSE_ASSERT (0);

      return NULL;
   }

SgStatement*
UntypedConverter::convertUntypedForAllStatement (SgUntypedForAllStatement* ut_stmt, SgScopeStatement* scope)
   {
      SgUntypedExprListExpression* iterates = ut_stmt->get_iterates();
      std::string name = ut_stmt->get_do_construct_name();

#if 0
      SgUntypedType* type = ut_stmt->get_type();
      SgUntypedExpression* mask = ut_stmt->get_mask();
      SgUntypedExprListExpression* locality = ut_stmt->get_local();
      cout << "-x- convert forall: enum is "       << ut_stmt->get_statement_enum() << endl;
      cout << "-x- convert forall: type is "       << type << endl;
      cout << "-x- convert forall: iterates are "  << iterates << endl;
      cout << "-x- convert forall: locality is "   << locality << endl;
      cout << "-x- convert forall: mask is "       << mask << endl;
      cout << "-x- convert forall: name is "       << name << endl;
#endif

      SgExprListExp* sg_expr_list = new SgExprListExp();
      setSourcePositionFrom(sg_expr_list, iterates);

      BOOST_FOREACH(SgUntypedExpression* ut_expr, iterates->get_expressions())
         {
            SgUntypedNamedExpression* named_expr = dynamic_cast<SgUntypedNamedExpression*>(ut_expr);
            ROSE_ASSERT(isSgUntypedNamedExpression(named_expr));

            SgName name = named_expr->get_expression_name();

            SgUntypedExpression* expr = named_expr->get_expression();
            SgUntypedSubscriptExpression* ut_triplet = dynamic_cast<SgUntypedSubscriptExpression*>(expr);

         // First assume easy case where variable has been declared yet
            SgVariableSymbol* sg_var_sym = SageInterface::lookupVariableSymbolInParentScopes(name, scope);
            ROSE_ASSERT(sg_var_sym);

            SgVarRefExp* sg_var_ref = SageBuilder::buildVarRefExp(sg_var_sym);
            ROSE_ASSERT(sg_var_ref);

            SgExpression* sg_triplet = convertUntypedSubscriptExpression(ut_triplet, false);

         // Append the variable and iteration bounds as a pair
            sg_expr_list->append_expression(sg_var_ref);
            sg_expr_list->append_expression(sg_triplet);
         }

   // At the moment can only handle DO CONCURRENT
   //   - need to get assignment statement from statement_list somehow
   //   - can this be obtained from the scope, converted, then deleted?
   //   - this would modify the tree during traversal, perhaps could mark node as converted all ready?
   //
      int stmt_enum = ut_stmt->get_statement_enum();
      ROSE_ASSERT(stmt_enum == General_Language_Translation::e_fortran_do_concurrent_stmt);

      SgBasicBlock* body = SageBuilder::buildBasicBlock();
      ROSE_ASSERT(body);
      SageInterface::setSourcePosition(body);

      SgForAllStatement* sg_stmt = new SgForAllStatement(sg_expr_list, body);
      ROSE_ASSERT(sg_stmt);
      setSourcePositionFrom(sg_stmt, ut_stmt);

      body->setCaseInsensitive(true);
      body->set_parent(sg_stmt);

      sg_expr_list->set_parent(sg_stmt);

      sg_stmt->setCaseInsensitive(true);
      sg_stmt->set_has_end_statement(true);
      sg_stmt->set_forall_statement_kind(SgForAllStatement::e_do_concurrent_statement);

      SageInterface::appendStatement(sg_stmt, scope);

   // The ForAllStatement body becomes the current scope
      SageBuilder::pushScopeStack(body);

      return sg_stmt;
   }

SgWhileStmt*
UntypedConverter::convertUntypedWhileStatement (SgUntypedWhileStatement* ut_stmt, SgNodePtrList& children, SgScopeStatement* scope)
   {
      ROSE_ASSERT(children.size() == 2);

      SgExpression* condition = isSgExpression(children[0]);
      ROSE_ASSERT(condition);

      SgStatement* body = isSgStatement(children[1]);
      ROSE_ASSERT(body);

   // The body (at least for a single statement) will have been inserted
      SageInterface::removeStatement(body);

      SgExprStatement* condition_stmt = SageBuilder::buildExprStatement(condition);
      ROSE_ASSERT(condition_stmt);

      SgWhileStmt* sg_stmt = SageBuilder::buildWhileStmt(condition_stmt, body);
      ROSE_ASSERT(sg_stmt);
      setSourcePositionFrom(sg_stmt, ut_stmt);

      SageInterface::appendStatement(sg_stmt, scope);

      return sg_stmt;
   }

SgExprStatement*
UntypedConverter::convertUntypedFunctionCallStatement (SgUntypedFunctionCallStatement* ut_stmt, SgNodePtrList& children, SgScopeStatement* scope)
   {
#if 0
      cout << "-x- convert func call: # children is " << children.size() << endl;
      cout << "                     :   child[0] is " << children[0] << endl;
      cout << "                     :   child[1] is " << children[1] << endl;
#endif

      ROSE_ASSERT(children.size() == 2);

      SgExprStatement* sg_stmt = NULL;

      SgFunctionRefExp* function = isSgFunctionRefExp(children[0]);
      ROSE_ASSERT(function);

      SgName function_name = function->get_symbol()->get_name();

      SgExprListExp* args = isSgExprListExp(children[1]);
      ROSE_ASSERT(args);

#if 0

   // This is a subroutine/procedure call (probably should not be a FuctionCallStatement

      SgFunctionType * func_type = buildFunctionType(return_type, typeList);
      SgFunctionRefExp* func_ref = buildFunctionRefExp(name,func_type,scope);

#endif

      SgTypeVoid* func_type = SageBuilder::buildVoidType();

      sg_stmt = SageBuilder::buildFunctionCallStmt(function_name, func_type, args, scope);
      ROSE_ASSERT(sg_stmt);

      SageInterface::appendStatement(sg_stmt, scope);

      return sg_stmt;
   }

SgIfStmt*
UntypedConverter::convertUntypedIfStatement (SgUntypedIfStatement* ut_stmt, SgNodePtrList& children, SgScopeStatement* scope)
   {
      ROSE_ASSERT(children.size() == 3);

#if 0
      cout << "-x- convert if: conditional is " << ut_stmt->get_conditional() << " " << ut_stmt->get_conditional()->class_name() << endl;
      cout << "-x- convert if:   true_body is " << ut_stmt->get_true_body() << " "   << ut_stmt->get_true_body()->class_name()   << endl;
      cout << "-x- convert if:  false_body is " << ut_stmt->get_false_body() << "\n";
#endif

      SgExpression* conditional = isSgExpression(children[0]);
      ROSE_ASSERT(conditional != NULL);

      SgStatement* true_body = isSgStatement(children[1]);
      ROSE_ASSERT(true_body != NULL);

   // This needs to be removed because statements are appended to a scope
   // True and false bodies will be replaced by the containing if statement
      SageInterface::removeStatement(true_body, scope);

   // The false body has been appended to the scope at this point. Because it
   // will be contained by the SgIfStmt it needs to be removed from the scope.
      SgStatement* false_body = isSgStatement(children[2]);
   // The false body is allowed to be NULL
      if (false_body != NULL) {
         SageInterface::removeStatement(false_body);
      }

   // The false_body may be an SgIfStmt (representing else if ...),
   // if so the unparser needs to know there is no END IF for the if statement.
      SgIfStmt* else_if_stmt = isSgIfStmt(children[2]);
      if (else_if_stmt != NULL) {
         else_if_stmt->set_is_else_if_statement(true);
         else_if_stmt->set_has_end_statement(false);
      }

      SgIfStmt* sg_stmt = SageBuilder::buildIfStmt(conditional, true_body, false_body);

      ROSE_ASSERT(sg_stmt != NULL);
      setSourcePositionFrom(sg_stmt, ut_stmt);

   // Determine if this is a simple if statement (no then or endif needed)
      if (ut_stmt->get_statement_enum() == General_Language_Translation::e_fortran_if_stmt)
         {
            sg_stmt->set_use_then_keyword(false);
            sg_stmt->set_has_end_statement(false);
         }

   // Determine if an if-construct-name is present
      if (ut_stmt->get_label_string().empty() == false)
         {
            sg_stmt->set_string_label(ut_stmt->get_label_string());
         }

      SageInterface::appendStatement(sg_stmt, scope);

// TEMPORARY (fix numeric labels especially here)
//    convertLabel(ut_stmt, sg_stmt, scope);

      return sg_stmt;
   }

SgStatement*
UntypedConverter::convertUntypedCaseStatement (SgUntypedCaseStatement* ut_stmt, SgNodePtrList& children, SgScopeStatement* scope)
  {
     using namespace General_Language_Translation;

      ROSE_ASSERT(children.size() == 2);

      SgStatement* sg_stmt = NULL;

      if (ut_stmt->get_statement_enum() == e_switch_stmt)
        {
           SgExpression* selector = isSgExpression(children[0]);
           SgStatement* body = isSgStatement(children[1]);

           ROSE_ASSERT(selector);
           ROSE_ASSERT(body);

           SgSwitchStatement* switch_stmt = SageBuilder::buildSwitchStatement(selector, body);
           ROSE_ASSERT(switch_stmt);

           switch_stmt->set_string_label(ut_stmt->get_case_construct_name());

           sg_stmt = switch_stmt;
        }
      else if (ut_stmt->get_statement_enum() == e_case_option_stmt)
        {
           SgExpression* key = isSgExpression(children[0]);
           SgStatement* body = isSgStatement(children[1]);

           ROSE_ASSERT(key);
           ROSE_ASSERT(body);

           SgCaseOptionStmt* case_option_stmt = SageBuilder::buildCaseOptionStmt(key, body);
           ROSE_ASSERT(case_option_stmt);

           case_option_stmt->set_case_construct_name (ut_stmt->get_case_construct_name());
           case_option_stmt->set_has_fall_through    (ut_stmt->get_has_fall_through());

           sg_stmt = case_option_stmt;
        }
      else if (ut_stmt->get_statement_enum() == e_case_default_option_stmt)
        {
           SgStatement* body = isSgStatement(children[1]);
           ROSE_ASSERT(body);

           SgDefaultOptionStmt* default_option_stmt = SageBuilder::buildDefaultOptionStmt(body);
           ROSE_ASSERT(default_option_stmt);

           default_option_stmt->set_default_construct_name (ut_stmt->get_case_construct_name());
           default_option_stmt->set_has_fall_through       (ut_stmt->get_has_fall_through());

           sg_stmt = default_option_stmt;
        }
      else
        {
           cerr << "ERROR: unknown statement_enum for convertUntypedCaseStatement \n";
           ROSE_ASSERT(0);
        }

      ROSE_ASSERT(sg_stmt);
      SageInterface::appendStatement(sg_stmt, scope);

      return sg_stmt;
  }

SgStatement*
UntypedConverter::convertUntypedAbortStatement (SgUntypedAbortStatement* ut_stmt, SgScopeStatement* scope)
   {
      bool hasLabel;

      SgExpression* abortExpression = new SgNullExpression();
      ROSE_ASSERT(abortExpression != NULL);

      SageInterface::setSourcePosition(abortExpression);

      SgStopOrPauseStatement* abortStatement = new SgStopOrPauseStatement(abortExpression);
      ROSE_ASSERT(abortStatement != NULL);

      abortStatement->set_stop_or_pause(SgStopOrPauseStatement::e_abort);
      setSourcePositionFrom(abortStatement, ut_stmt);
      abortExpression->set_parent(abortStatement);

      hasLabel = convertLabel(ut_stmt, abortStatement, scope);
      if (hasLabel == false) {
      // There is no enclosing SgLabelStatement so this statement is added to the scope
         scope->append_statement(abortStatement);
      }

      return abortStatement;
   }

SgStatement*
UntypedConverter::convertUntypedExitStatement (SgUntypedExitStatement* ut_stmt, SgScopeStatement* scope)
   {
      bool hasLabel;

      SgExpression* exitExpression = new SgNullExpression();
      ROSE_ASSERT(exitExpression != NULL);

      SageInterface::setSourcePosition(exitExpression);

      SgStopOrPauseStatement* exitStatement = new SgStopOrPauseStatement(exitExpression);
      ROSE_ASSERT(exitStatement != NULL);

      exitStatement->set_stop_or_pause(SgStopOrPauseStatement::e_exit);
      setSourcePositionFrom(exitStatement, ut_stmt);

      exitExpression->set_parent(exitStatement);

      hasLabel = convertLabel(ut_stmt, exitStatement, scope);
      if (hasLabel == false) {
      // There is no enclosing SgLabelStatement so this statement is added to the scope
         scope->append_statement(exitStatement);
      }

      return exitStatement;
   }

SgStatement*
UntypedConverter::convertUntypedGotoStatement (SgUntypedGotoStatement* ut_stmt, SgScopeStatement* scope)
   {
      SgLabelSymbol* target_symbol = NULL;
      SgLabelStatement* label_stmt = NULL;
      SgGotoStatement*   goto_stmt = NULL;
      bool hasLabel;

      SgFunctionDefinition * label_scope = SageInterface::getEnclosingFunctionDefinition(scope, true);
      ROSE_ASSERT (label_scope);

      SgName target_name(ut_stmt->get_target_label());

      if (label_scope->symbol_exists(target_name))
         {
            target_symbol = label_scope->lookup_label_symbol(target_name);
            ROSE_ASSERT(target_symbol != NULL);
         }
      else
         {
         // The label statement doesn't exist yet but we have to build one to use later
            label_stmt = SageBuilder::buildLabelStatement(target_name, NULL, label_scope);
            SageInterface::setSourcePosition(label_stmt);

            target_symbol = label_scope->lookup_label_symbol(target_name);
            ROSE_ASSERT(target_symbol);

         }

      goto_stmt = SageBuilder::buildGotoStatement(target_symbol);
      setSourcePositionFrom(goto_stmt, ut_stmt);

      hasLabel = convertLabel(ut_stmt, goto_stmt, scope);
      if (hasLabel == false) {
      // There is no enclosing SgLabelStatement so this statement is added to the scope
         scope->append_statement(goto_stmt);
      }

      return goto_stmt;
   }

SgStatement*
UntypedConverter::convertUntypedLabelStatement_decl (SgUntypedLabelStatement* ut_stmt, SgScopeStatement* scope)
   {
      SgLabelSymbol*  label_symbol = NULL;
      SgLabelStatement* label_stmt = NULL;

      SgFunctionDefinition * label_scope = SageInterface::getEnclosingFunctionDefinition(scope, true);
      ROSE_ASSERT (label_scope);

      SgName label_name(ut_stmt->get_label_string());

      if (label_scope->symbol_exists(label_name))
         {
         // The label statement already exists because of, perhaps, a GOTO statement
            label_symbol = label_scope->lookup_label_symbol(label_name);
            ROSE_ASSERT(label_symbol != NULL);

            label_stmt = label_symbol->get_declaration();

         }
      else
         {
            label_stmt = SageBuilder::buildLabelStatement(label_name, NULL, label_scope);
         }
      setSourcePositionFrom(label_stmt, ut_stmt);

   // Check to see if the SgUntypedLabelStatement has a label statement as a parent,
   // if so connect the two sage nodes in the AST by setting parent and statement
      SgNode* ut_parent = ut_stmt->get_parent();

      if (ut_parent == NULL)
         {
         // First label is appended to the scope like other statements
            scope->append_statement(label_stmt);
         }
      else
         {
         // Subsequent labels belong in the function definition scope
            label_stmt->set_scope(label_scope);

            SgUntypedLabelStatement* ut_parent_label_stmt = isSgUntypedLabelStatement(ut_parent);
            if (ut_parent_label_stmt != NULL)
               {
                  SgName parent_label_name(ut_parent_label_stmt->get_label_string());
                  SgLabelSymbol* parent_label_symbol = label_scope->lookup_label_symbol(parent_label_name);
                  ROSE_ASSERT(parent_label_symbol != NULL);

                  SgLabelStatement* parent_label_stmt = parent_label_symbol->get_declaration();
                  ROSE_ASSERT(parent_label_stmt);

                  label_stmt->set_parent(parent_label_stmt);
                  parent_label_stmt->set_statement(label_stmt);
               }
         }

      return label_stmt;
   }

SgStatement*
UntypedConverter::convertUntypedLabelStatement (SgUntypedLabelStatement* ut_stmt, SgStatement* sg_stmt, SgScopeStatement* scope)
   {
      ROSE_ASSERT(scope != NULL);

      SgName label(ut_stmt->get_label_string());

      SgLabelStatement* label_stmt = SageBuilder::buildLabelStatement(label, sg_stmt);
      ROSE_ASSERT(label_stmt != NULL);
      setSourcePositionFrom(label_stmt, ut_stmt);

      bool hasLabel = convertLabel(ut_stmt, label_stmt, scope);
      if (hasLabel == false) {
      // There is no enclosing SgLabelStatement so this statement is added to the scope
         scope->append_statement(label_stmt);
      }

      return label_stmt;
   }

SgStatement*
UntypedConverter::convertUntypedNamedStatement (SgUntypedNamedStatement* ut_stmt, SgScopeStatement* scope)
   {
      using namespace General_Language_Translation;

      switch (ut_stmt->get_statement_enum())
        {
        case e_fortran_end_do_stmt:
           {
              SageBuilder::popScopeStack();
              scope = SageBuilder::topScopeStack();
              break;
           }

#if 0
     // Something needs to be done about adding language specific header files for enums
        case Fortran_ROSE_Translation::e_program_stmt:
        case Fortran_ROSE_Translation::e_end_program_stmt:
           {
           // Nothing to do here
              break;
           }

        case Fortran_ROSE_Translation::e_block_stmt:
        case Fortran_ROSE_Translation::e_end_block_stmt:
#endif

        case e_end_switch_stmt:
           {
              cout << "WARNING UNIMPLEMENTED: convertUntypedNamedStatement - e_end_switch_stmt\n";
              break;
           }
        case e_fortran_end_forall_stmt:
           {
              cout << "WARNING UNIMPLEMENTED: convertUntypedNamedStatement - e_fortran_end_forall_stmt\n";
              break;
           }
        case e_end_proc_def_stmt:
           {
              cout << "WARNING UNIMPLEMENTED: convertUntypedNamedStatement - e_end_proc_def_stmt\n";
              break;
           }
        case e_end_proc_ref_stmt:
           {
              cout << "WARNING UNIMPLEMENTED: convertUntypedNamedStatement - e_end_proc_ref_stmt\n";
              break;
           }
        case e_unknown:
           {
              cout << "WARNING UNIMPLEMENTED: convertUntypedNamedStatement - e_unknown\n";
              break;
           }

        default:
           {
              cout << "WARNING UNIMPLEMENTED: convertUntypedNamedStatement - stmt_enum not handled is " << ut_stmt->get_statement_enum()
                   << ": Could be one of the Fortran end statements (for example): ut_stmt is " << ut_stmt << endl;
           }
        }

      return scope;
   }

SgNullStatement*
UntypedConverter::convertUntypedNullStatement (SgUntypedNullStatement* ut_stmt, SgScopeStatement* scope)
   {
      SgNullStatement* nullStatement = SageBuilder::buildNullStatement();
      setSourcePositionFrom(nullStatement, ut_stmt);

      SageInterface::appendStatement(nullStatement, scope);
      convertLabel(ut_stmt, nullStatement, scope);

      return nullStatement;
   }

SgStatement*
UntypedConverter::convertUntypedOtherStatement (SgUntypedOtherStatement* ut_stmt, SgScopeStatement* scope)
   {
      switch (ut_stmt->get_statement_enum())
        {

    // Nothing so far for general languages

       default:
          {
             cerr << "UntypedConverter::convertUntypedOtherStatement: failed to find known statement enum, is "
                  << ut_stmt->get_statement_enum() << endl;
             ROSE_ASSERT(0);
          }
       }
   }

SgImageControlStatement*
UntypedConverter::convertUntypedImageControlStatement (SgUntypedImageControlStatement* ut_stmt, SgScopeStatement* scope)
   {
      switch (ut_stmt->get_statement_enum())
        {

    // Nothing so far for general languages
       default:
          {
             cerr << "UntypedConverter::convertUntypedImageControlStatement: implemented only for Fortran, statement enum, is "
                  << ut_stmt->get_statement_enum() << endl;
             ROSE_ASSERT(0);
          }
       }

   // Should never reach here.
      return NULL;
   }

SgImageControlStatement*
UntypedConverter::convertUntypedImageControlStatement (SgUntypedImageControlStatement* ut_stmt, SgNodePtrList& children, SgScopeStatement* scope)
   {
      switch (ut_stmt->get_statement_enum())
        {

    // Nothing so far for general languages
       default:
          {
             cerr << "UntypedConverter::convertUntypedImageControlStatement: implemented only for Fortran, statement enum, is "
                  << ut_stmt->get_statement_enum() << endl;
             ROSE_ASSERT(0);
          }
       }

   // Should never reach here.
      return NULL;
   }

SgReturnStmt*
UntypedConverter::convertUntypedReturnStatement (SgUntypedReturnStatement* ut_stmt, SgNodePtrList& children, SgScopeStatement* scope)
   {
      SgReturnStmt* return_stmt;
      bool hasLabel;

      ROSE_ASSERT(children.size() == 1);

      SgExpression* sg_expr = isSgExpression(children[0]);
      ROSE_ASSERT(sg_expr != NULL);

      return_stmt = SageBuilder::buildReturnStmt(sg_expr);
      ROSE_ASSERT(return_stmt != NULL);

      setSourcePositionFrom(return_stmt, ut_stmt);

      hasLabel = convertLabel(ut_stmt, return_stmt, scope);
      if (hasLabel == false) {
      // There is no enclosing SgLabelStatement so this statement is added to the scope
         scope->append_statement(return_stmt);
      }

      return return_stmt;
   }

SgStatement*
UntypedConverter::convertUntypedStopStatement (SgUntypedStopStatement* ut_stmt, SgNodePtrList& children, SgScopeStatement* scope)
   {
      bool hasLabel;
      SgStopOrPauseStatement* stop_stmt;

      ROSE_ASSERT(children.size() == 1);

      SgExpression* stop_expr = isSgExpression(children[0]);
      ROSE_ASSERT(stop_expr != NULL);

      stop_stmt = new SgStopOrPauseStatement(stop_expr);
      stop_stmt->set_stop_or_pause(SgStopOrPauseStatement::e_stop);

      ROSE_ASSERT(stop_stmt != NULL);
      setSourcePositionFrom(stop_stmt, ut_stmt);

      stop_expr->set_parent(stop_stmt);

      hasLabel = convertLabel(ut_stmt, stop_stmt, scope);
      if (hasLabel == false) {
      // There is no enclosing SgLabelStatement so this statement is added to the scope
         SageInterface::appendStatement(stop_stmt, scope);
      }

      return stop_stmt;
   }

// Jovial specific
//

SgDeclarationStatement*
UntypedConverter::convertUntypedJovialCompoolStatement(SgUntypedNameListDeclaration* ut_decl, SgScopeStatement* scope)
   {
      switch (ut_decl->get_statement_enum())
        {

    // Nothing so far for general languages
       default:
          {
             cerr << "UntypedConverter::convertUntypedJovialCompoolStatement: implemented only for Jovial, statement enum, is "
                  << ut_decl->get_statement_enum() << endl;
             ROSE_ASSERT(0);
          }
       }

   // Should never reach here.
      return NULL;
   }

// Expressions
//

SgExpression*
UntypedConverter::convertUntypedExpression(SgUntypedExpression* ut_expr, bool delete_ut_expr)
   {
      SgExpression* sg_expr = NULL;

      switch (ut_expr->variantT())
         {
           case V_SgUntypedSubscriptExpression:
              {
                 SgUntypedSubscriptExpression* ut_subscript_expr = isSgUntypedSubscriptExpression(ut_expr);
                 sg_expr = convertUntypedSubscriptExpression(ut_subscript_expr, delete_ut_expr);
                 break;
              }
           case V_SgUntypedNullExpression:
              {
              // Ignore source position information for now, for some reason it is broken (perhaps filename)
                 sg_expr = SageBuilder::buildNullExpression();
                 break;
              }
           case V_SgUntypedReferenceExpression:
              {
                 SgUntypedReferenceExpression* ref_expr = isSgUntypedReferenceExpression(ut_expr);
                 sg_expr = convertUntypedReferenceExpression(ref_expr, delete_ut_expr);
                 break;
              }
           case V_SgUntypedValueExpression:
              {
                 SgUntypedValueExpression* ut_value_expr = isSgUntypedValueExpression(ut_expr);
                 sg_expr = convertUntypedValueExpression(ut_value_expr, delete_ut_expr);
                 if (delete_ut_expr && ut_value_expr->get_type())
                    {
                       delete ut_value_expr->get_type();
                       ut_value_expr->set_type(NULL);
                    }
                 break;
              }
           case V_SgUntypedOtherExpression:
              {
                 SgUntypedOtherExpression* ut_other_expr = isSgUntypedOtherExpression(ut_expr);
                 int expr_enum = ut_other_expr->get_expression_enum();
                 if (expr_enum == General_Language_Translation::e_star_expression)
                    {
                    // Ignore source position information for now, for some reason it is broken (perhaps filename)
                       sg_expr = SageBuilder::buildNullExpression();
                    // Break here on purpose to otherwise fall through to default
                       break;
                    }
              }
           case V_SgUntypedExprListExpression:
              {
                 SgUntypedExprListExpression* ut_list = isSgUntypedExprListExpression(ut_expr);
                 SgExprListExp* sg_expr_list = convertUntypedExprListExpression(ut_list,delete_ut_expr);
                 sg_expr = sg_expr_list;
                 break;
              }
           default:
              {
                 cerr << "UntypedConverter::convertUntypedExpression: unimplemented for class " << ut_expr->class_name()
                      << ": " << ut_expr << endl;
                 ROSE_ASSERT(0);  // Unimplemented
              }
         }

      if (delete_ut_expr)
         {
#if 0
            cout << "--- deleting node " << ut_expr << " " << ut_expr->class_name() << endl;
#endif
            delete ut_expr;
         }

      return sg_expr;
   }


SgExpression*
UntypedConverter::convertUntypedExpression(SgUntypedExpression* ut_expr, SgNodePtrList& children)
   {
      SgExpression* sg_expr = NULL;

      if ( isSgUntypedBinaryOperator(ut_expr) != NULL )
         {
            SgUntypedBinaryOperator* op = dynamic_cast<SgUntypedBinaryOperator*>(ut_expr);
            ROSE_ASSERT(children.size() == 2);

            SgExpression* expr1 = isSgExpression(children[0]);
            SgExpression* expr2 = isSgExpression(children[1]);
            ROSE_ASSERT(expr1 && expr2);

            SgBinaryOp* sg_operator = convertUntypedBinaryOperator(op, expr1, expr2);
            sg_expr = sg_operator;
#if DEBUG_UNTYPED_CONVERTER
            printf ("  - binary operator      ==>   %s\n", op->get_operator_name().c_str());
#endif
         }
      else if ( isSgUntypedUnaryOperator(ut_expr) != NULL )
         {
            SgUntypedUnaryOperator* op = dynamic_cast<SgUntypedUnaryOperator*>(ut_expr);
            ROSE_ASSERT(children.size() == 1);

            SgExpression* expr1 = isSgExpression(children[0]);
            ROSE_ASSERT(expr1);

            SgUnaryOp* sg_operator = convertUntypedUnaryOperator(op, expr1);
            sg_expr = sg_operator;
#if DEBUG_UNTYPED_CONVERTER
            printf ("  -  unary operator      ==>   %s\n", op->get_operator_name().c_str());
#endif
         }
      else
         {
            cerr << "UntypedConverter::convertUntypedExpression: unimplemented for class " << ut_expr->class_name() << endl;
            ROSE_ASSERT(0);  // Unimplemented
         }

      return sg_expr;
   }


SgExpression*
UntypedConverter::convertUntypedExpression(SgUntypedExpression* ut_expr, SgNodePtrList& children, SgScopeStatement* scope)
   {
      SgExpression* sg_expr = NULL;

      if ( isSgUntypedBinaryOperator(ut_expr) != NULL )
         {
            SgUntypedBinaryOperator* op = dynamic_cast<SgUntypedBinaryOperator*>(ut_expr);
            ROSE_ASSERT(children.size() == 2);

            SgExpression* expr1 = isSgExpression(children[0]);
            SgExpression* expr2 = isSgExpression(children[1]);
            ROSE_ASSERT(expr1 && expr2);

            SgBinaryOp* sg_operator = convertUntypedBinaryOperator(op, expr1, expr2);
            sg_expr = sg_operator;
         }
      if ( isSgUntypedUnaryOperator(ut_expr) != NULL )
         {
            SgUntypedUnaryOperator* op = dynamic_cast<SgUntypedUnaryOperator*>(ut_expr);
            ROSE_ASSERT(children.size() == 1);

            SgExpression* expr1 = isSgExpression(children[0]);
            ROSE_ASSERT(expr1);

            SgUnaryOp* sg_operator = convertUntypedUnaryOperator(op, expr1);
            sg_expr = sg_operator;
         }
      else if ( isSgUntypedValueExpression(ut_expr) != NULL )
         {
            SgUntypedValueExpression* expr = dynamic_cast<SgUntypedValueExpression*>(ut_expr);
            sg_expr = convertUntypedValueExpression(expr);
         }
      else if ( isSgUntypedReferenceExpression(ut_expr) != NULL )
         {
            SgUntypedReferenceExpression* ref_expr = dynamic_cast<SgUntypedReferenceExpression*>(ut_expr);
            sg_expr = convertUntypedReferenceExpression(ref_expr);
         }
      else if ( isSgUntypedOtherExpression(ut_expr) != NULL )
         {
            SgUntypedOtherExpression* expr = dynamic_cast<SgUntypedOtherExpression*>(ut_expr);
            if (expr->get_expression_enum() == SgToken::FORTRAN_NULL)
               {
                  sg_expr = new SgNullExpression();
                  setSourcePositionFrom(sg_expr, ut_expr);
               }
         }

      return sg_expr;
   }


SgValueExp*
UntypedConverter::convertUntypedValueExpression (SgUntypedValueExpression* ut_expr, bool delete_ut_expr)
{
   SgValueExp* sg_expr = NULL;

   // TODO - what about doubles, longs, Jovial fixed, ...
   switch(ut_expr->get_type()->get_type_enum_id())
       {
         case SgUntypedType::e_int:
         case SgUntypedType::e_uint:
            {
               std::string constant_text = ut_expr->get_value_string();

            // preserve kind parameter if any
               if (ut_expr->get_type()->get_has_kind())
                  {
                     SgUntypedValueExpression* ut_kind_expr = isSgUntypedValueExpression(ut_expr->get_type()->get_type_kind());
                     ROSE_ASSERT(ut_kind_expr != NULL);
                  // For now just append to the value string
                     constant_text += std::string("_") + ut_kind_expr->get_value_string();

// kind value need to be handled correctly, probably via the type system (maybe somewhat like below)
#if 0
                  // TODO - also must expect a scalar-int-constant-name
                     ROSE_ASSERT(ut_kind_expr != NULL);
                     SgValueExpr* sg_kind_expr = convert_SgUntypedValueExpression(ut_kind_expr);

                     ROSE_ASSERT(sg_kind_expr->get_parent() == NULL);
                     SgTypeInt* integerType = SgTypeInt::createType(0, sg_kind_expr);
                     sg_kind_expr->set_parent(integerType);
                     ROSE_ASSERT(sg_kind_expr->get_parent() != NULL);
#endif
                  // Causes problems in addAssociatedNodes if not deleted
                     delete ut_kind_expr;
                  }

               sg_expr = new SgIntVal(atoi(ut_expr->get_value_string().c_str()), constant_text);
               setSourcePositionFrom(sg_expr, ut_expr);
               break;
            }

         case SgUntypedType::e_float:
            {
               std::string constant_text = ut_expr->get_value_string();

            // preserve kind parameter if any
               if (ut_expr->get_type()->get_has_kind())
                  {
                     cerr << "WARNING: UntypedConverter::convertUntypedValueExpression: kind value not handled \n";
                  }

               sg_expr = new SgFloatVal(atof(ut_expr->get_value_string().c_str()), constant_text);
               setSourcePositionFrom(sg_expr, ut_expr);
               break;
            }

         case SgUntypedType::e_bool:
            {
               bool bool_val;
               std::string constant_text = ut_expr->get_value_string();

               if      (constant_text == "TRUE")  bool_val = 1;
               else if (constant_text == "FALSE") bool_val = 0;
               else    ROSE_ASSERT(0);

            // preserve kind parameter if any
               if (ut_expr->get_type()->get_has_kind())
                  {
                     cerr << "WARNING: UntypedConverter::convertUntypedValueExpression: kind value not handled \n";
                  }

               sg_expr = SageBuilder::buildBoolValExp(bool_val);
               setSourcePositionFrom(sg_expr, ut_expr);
               break;
            }

         case SgUntypedType::e_bit:
            {
               cerr << "WARNING UNIMPLEMENTED: convertSgUntypedValueExpression: BitFormula\n";
               break;
            }

         default:
            {
               ROSE_ASSERT(0);  // NOT IMPLEMENTED
            }
       }

    if (delete_ut_expr)
       {
#if 0
          cout << "--- deleting node " << ut_expr << " " << ut_expr->class_name() << endl;
#endif
          delete ut_expr;
       }

    return sg_expr;
 }

SgExprListExp*
UntypedConverter::convertUntypedExprListExpression(SgUntypedExprListExpression* ut_expr_list, bool delete_ut_expr)
{
   SgExprListExp* sg_expr_list = new SgExprListExp();
   setSourcePositionFrom(sg_expr_list, ut_expr_list);

   BOOST_FOREACH(SgUntypedExpression* ut_expr, ut_expr_list->get_expressions())
      {
         SgExpression* sg_expr = convertUntypedExpression(ut_expr, delete_ut_expr);
         sg_expr_list->append_expression(sg_expr);
      }

   if (delete_ut_expr)
      {
#if 0
         cout << "--- deleting node " << ut_expr_list << " " << ut_expr_list->class_name() << endl;
#endif
         delete ut_expr_list;
      }

   return sg_expr_list;
}


SgExprListExp*
UntypedConverter::convertUntypedExprListExpression(SgUntypedExprListExpression* ut_expr_list, SgNodePtrList& children)
{
   using namespace General_Language_Translation;

   SgExprListExp* sg_expr_list = NULL;

   switch(ut_expr_list->get_expression_enum())
     {
       case e_argument_list:
       case e_array_subscripts:
       case e_case_selector:
       case e_section_subscripts:
         {
            sg_expr_list = new SgExprListExp();
            ROSE_ASSERT(sg_expr_list);

            setSourcePositionFrom(sg_expr_list, ut_expr_list);

            BOOST_FOREACH(SgLocatedNode* sg_node, children)
              {
                 SgExpression* sg_expr = dynamic_cast<SgExpression*>(sg_node);
                 ROSE_ASSERT(sg_expr);
                 sg_expr_list->append_expression(sg_expr);
              }
            break;
         }

    // Some lists are converted explicitly (not via a traversal) so they don't belong here
    //
       case e_array_shape:                  break;
       case e_type_modifier_list:           break;
       case e_function_modifier_list:       break;

       case e_fortran_sync_stat_list:       break;
       case e_fortran_sync_stat_stat:       break;
       case e_fortran_sync_stat_errmsg:     break;
       case e_fortran_stat_acquired_lock:   break;

    // Jovial specific (for location-specifier)
       case e_struct_item_modifier_list:    break;
       case e_storage_modifier_location:    break;

       case e_unknown:
         {
            cerr << "WARNING: convertUntypedExprListExpression: has children and enum is unknown: "
                 << ut_expr_list->get_expression_enum() << ": list is " << ut_expr_list <<endl;
            ROSE_ASSERT(children.size() == 0);
            ROSE_ASSERT(ut_expr_list->get_expressions().size() == 0);
            break;
         }

       default:
         {
            cerr << "WARNING: convertUntypedExprListExpression: unknown enum, is "
                 << ut_expr_list->get_expression_enum() << ": list is " << ut_expr_list <<endl;
         }
     }

   return sg_expr_list;
}

SgExpression*
UntypedConverter::convertUntypedSubscriptExpression(SgUntypedSubscriptExpression* ut_expr, bool delete_ut_expr)
{
   using namespace General_Language_Translation;

#if 0
   cout << "UntypedConverter::convertUntypedSubscriptExpression: class " << ut_expr->class_name() << " " << ut_expr << endl;
   cout << "UntypedConverter::convertUntypedSubscriptExpression:  enum " << ut_expr->get_expression_enum() << endl;
   cout << "UntypedConverter::convertUntypedSubscriptExpression:    lb " << ut_expr->get_lower_bound()->class_name()
        << " " << ut_expr->get_lower_bound() << endl;
   cout << "UntypedConverter::convertUntypedSubscriptExpression:    ub " << ut_expr->get_upper_bound()->class_name()
        << " " << ut_expr->get_upper_bound() << endl;
   cout << "UntypedConverter::convertUntypedSubscriptExpression:    st " << ut_expr->get_stride()->class_name()
        << " " << ut_expr->get_stride() << endl;
   cout << "UntypedConverter::convertUntypedSubscriptExpression:   del " << delete_ut_expr << endl;
#endif

   SgExpression* sg_expr = NULL;
   SgExpression* sg_lower_bound = NULL;
   SgExpression* sg_upper_bound = NULL;
   SgExpression* sg_stride = NULL;

   switch(ut_expr->get_expression_enum())
      {
        case e_array_index_triplet: // Fortran: stride may be NullExpression
        case e_explicit_shape:      // Fortran: stride is NullExpression or "1"
        case e_explicit_dimension:  // Jovial : lower may be and stride is NullExpression
           {
              // cout << "-x- explicit shape or triplet: expr_enum is " << ut_expr->get_expression_enum() << endl;
              sg_upper_bound = convertUntypedExpression(ut_expr->get_upper_bound(), delete_ut_expr);
              if (!isSgUntypedNullExpression(ut_expr->get_stride()))
                 {
                    sg_stride = convertUntypedExpression(ut_expr->get_stride(), delete_ut_expr);
                 }
              break;
           }
        case e_assumed_shape: // Fortran: upper and stride are NullExpression (and lower may be)
           {
              // cout << "-x- assumed shape\n";
              if (isSgUntypedNullExpression(ut_expr->get_lower_bound()))
                 sg_upper_bound = new SgColonShapeExp();
              else
                 sg_upper_bound = new SgNullExpression();
              setSourcePositionUnknown(sg_upper_bound);
              break;
           }
        case e_assumed_or_implied_shape: // Fortran: upper and stride are NullExpression
        case e_assumed_size:             // Fortran: upper and stride are NullExpression
        case e_star_dimension:           // Jovial : lower, upper, and stride are NullExpression
           {
              // cout << "-x- assumed size\n";
              sg_upper_bound = new SgAsteriskShapeExp();
              setSourcePositionUnknown(sg_upper_bound);
              break;
           }
        default:
           {
              cout << "ERROR with array shape in UntypedConverter::convertUntypedSubscriptExpression for : "
                   << ut_expr->get_expression_enum() << endl;
              ROSE_ASSERT(0);
           }
      }

   if (isSgUntypedNullExpression(ut_expr->get_lower_bound()))
      {
         sg_expr = sg_upper_bound;
      }
   else
      {
         sg_lower_bound = convertUntypedExpression(ut_expr->get_lower_bound(), delete_ut_expr);

         // cout << "-x- ut_stride " << ut_expr->get_stride() << endl;
         if (sg_stride == NULL)
            {
               // PLEASE clean this up, PLEASE (look at e_array_index_triplet logic)
               sg_stride = new SgIntVal(1,"1");
               setSourcePositionUnknown(sg_stride);
            }

         sg_expr = new SgSubscriptExpression(sg_lower_bound, sg_upper_bound, sg_stride);
         setSourcePositionFrom(sg_expr, ut_expr);

      // Set the parents of all the parts of the SgSubscriptExpression
         sg_lower_bound->set_parent(sg_expr);
         sg_upper_bound->set_parent(sg_expr);
         sg_stride     ->set_parent(sg_expr);
      }

   if (delete_ut_expr)
      {
      // THIS SEEMS BOGUS (lower, upper, and strides are deleted above)
      // The untyped expression should not be deleted yet, but SgUntypedNullExpression members should be.
         if (isSgUntypedNullExpression(ut_expr->get_lower_bound())) delete ut_expr->get_lower_bound();
         if (isSgUntypedNullExpression(ut_expr->get_upper_bound())) delete ut_expr->get_upper_bound();
         if (isSgUntypedNullExpression(ut_expr->get_stride()     )) delete ut_expr->get_stride();
      }

   return sg_expr;
}


SgExpression*
UntypedConverter::convertUntypedSubscriptExpression(SgUntypedSubscriptExpression* ut_expr, SgNodePtrList& children)
{
   ROSE_ASSERT(children.size() == 3);

   SgExpression* sg_expr = NULL;

   // try doing this for specific nodes only, perhaps it will work for all of the subscript expressions
   if (ut_expr->get_expression_enum() == General_Language_Translation::e_case_range)
     {
        SgExpression* lower_bound = isSgExpression(children[0]);
        SgExpression* upper_bound = isSgExpression(children[1]);
        SgExpression* stride      = isSgExpression(children[2]);
        ROSE_ASSERT(lower_bound && upper_bound && stride);

     // stride has to be converted to the integer 1
        delete stride;
        stride = new SgIntVal(1,"1");
        SageInterface::setSourcePosition(stride);

        sg_expr = new SgSubscriptExpression(lower_bound, upper_bound, stride);
        ROSE_ASSERT(sg_expr != NULL);
        setSourcePositionFrom(sg_expr, ut_expr);
     }

   return sg_expr;
}

SgExpression*
UntypedConverter::convertUntypedReferenceExpression(SgUntypedReferenceExpression* ut_expr, bool delete_ut_expr)
 {
    ROSE_ASSERT(ut_expr);

    SgExpression* sg_expr = NULL;
    SgScopeStatement* scope = SageBuilder::topScopeStack();
    int expr_enum = ut_expr->get_expression_enum();

    if (expr_enum == General_Language_Translation::e_function_reference)
       {
       // Use the version in this class rather than SageBuilder (as it has been removed from the API)
          sg_expr = buildFunctionRefExp(ut_expr->get_name(), scope);
          //cout << "-x- building function ref expr " << sg_expr << endl;
       }
    else
       {
          sg_expr = SageBuilder::buildVarRefExp(ut_expr->get_name(), scope);
       }

    ROSE_ASSERT(sg_expr != NULL);
    setSourcePositionFrom(sg_expr, ut_expr);

    return sg_expr;
 }

SgPntrArrRefExp*
UntypedConverter::convertUntypedArrayReferenceExpression (SgUntypedArrayReferenceExpression* ut_expr, SgNodePtrList& children)
 {
    ROSE_ASSERT(ut_expr);
    ROSE_ASSERT(children.size() == 2);

    SgPntrArrRefExp* array_ref = NULL;
    SgScopeStatement* scope = SageBuilder::topScopeStack();

    SgExpression*   array_subscripts = isSgExpression(children[0]);
    SgExpression* coarray_subscripts = isSgExpression(children[1]);
    ROSE_ASSERT(isSgExprListExp(array_subscripts));

 // No coarrays for the moment
    ROSE_ASSERT(isSgNullExpression(coarray_subscripts));
    delete coarray_subscripts;

    SgVarRefExp* var_ref = SageBuilder::buildVarRefExp(ut_expr->get_name(), scope);
    ROSE_ASSERT(var_ref);
    setSourcePositionFrom(var_ref, ut_expr);

    array_ref = SageBuilder::buildPntrArrRefExp(var_ref, array_subscripts);
    ROSE_ASSERT(array_ref);
    setSourcePositionFrom(array_ref, ut_expr);

    return array_ref;
 }

SgUnaryOp*
UntypedConverter::convertUntypedUnaryOperator(SgUntypedUnaryOperator* untyped_operator, SgExpression* expr)
 {
    using namespace General_Language_Translation;
    SgUnaryOp* op = NULL;

    switch(untyped_operator->get_expression_enum())
       {
         case e_operator_unary_plus:
            {
               op = new SgUnaryAddOp(expr, NULL);
               setSourcePositionFrom(op, expr);
               break;
            }
         case e_operator_unary_minus:
            {
               op = new SgMinusOp(expr, NULL);
               setSourcePositionFrom(op, expr);
               break;
            }
         case e_operator_unary_not:
            {
               op = new SgNotOp(expr, NULL);
               break;
            }
         case e_operator_unity:
            {
            // Nothing to do here
               cout << "  - e_operator_unary_unity: \n";
               break;
            }

         default:
            {
               cerr << "UntypedConverter::convertUntypedUnaryOperator: unknown expression enum value "
                    << untyped_operator->get_expression_enum() << endl;
               ROSE_ASSERT(0);  // NOT IMPLEMENTED
            }
       }
    return op;
 }

SgBinaryOp*
UntypedConverter::convertUntypedBinaryOperator(SgUntypedBinaryOperator* untyped_operator, SgExpression* lhs, SgExpression* rhs)
 {
    using namespace General_Language_Translation;

    SgBinaryOp* op = NULL;

    ROSE_ASSERT(lhs);
    ROSE_ASSERT(rhs);

    switch(untyped_operator->get_expression_enum())
       {
         case e_operator_assign:
            {
               op = new SgAssignOp(lhs, rhs, NULL);
            // lhs expression now becomes an lvalue
               lhs->set_lvalue(true);
               break;
            }
         case e_operator_add:
            {
               op = new SgAddOp(lhs, rhs, NULL);
               break;
            }
         case e_operator_subtract:
            {
               op = new SgSubtractOp(lhs, rhs, NULL);
               break;
            }
         case e_operator_exponentiate:
            {
               op = new SgExponentiationOp(lhs, rhs, NULL);
               break;
            }
         case e_operator_concatenate:
            {
               op = new SgConcatenationOp(lhs, rhs, NULL);
               break;
            }
         case e_operator_multiply:
            {
               op = new SgMultiplyOp(lhs, rhs, NULL);
               break;
            }
         case e_operator_divide:
            {
               op = new SgDivideOp(lhs, rhs, NULL);
               break;
            }
         case e_operator_mod:
            {
               op = new SgModOp(lhs, rhs, NULL);
               break;
            }
         case e_operator_and:
            {
               op = new SgAndOp(lhs, rhs, NULL);
               break;
            }
         case e_operator_or:
            {
               op = new SgOrOp(lhs, rhs, NULL);
               break;
            }
#if 0
         case e_operator_equiv:
            {
               op = new SgEqualityOp(lhs, rhs, NULL);
               ROSE_ASSERT(0);  // check on logical operands
               break;
            }
         case e_operator_not_equiv:
            {
               op = new SgNotEqualOp(lhs, rhs, NULL);
               ROSE_ASSERT(0);  // check on logical operands
               break;
            }
#endif
         case e_operator_equality:
            {
               op = new SgEqualityOp(lhs, rhs, NULL);
               break;
            }
         case e_operator_not_equal:
            {
               op = new SgNotEqualOp(lhs, rhs, NULL);
               break;
            }
         case e_operator_greater_than_or_equal:
            {
               op = new SgGreaterOrEqualOp(lhs, rhs, NULL);
               break;
            }
         case e_operator_less_than_or_equal:
            {
               op = new SgLessOrEqualOp(lhs, rhs, NULL);
               break;
            }
         case e_operator_less_than:
            {
               op = new SgLessThanOp(lhs, rhs, NULL);
               break;
            }
         case e_operator_greater_than:
            {
               op = new SgGreaterThanOp(lhs, rhs, NULL);
               break;
            }
         default:
            {
               ROSE_ASSERT(0);  // NOT IMPLEMENTED
            }
       }

    ROSE_ASSERT(op);
    setSourcePositionIncluding(op, lhs, rhs);

    lhs->set_parent(op);
    rhs->set_parent(op);

    return op;
 }


SgScopeStatement*
UntypedConverter::initialize_global_scope(SgSourceFile* file)
{
 // First we have to get the global scope initialized (and pushed onto the stack).

 // Set the default for source position generation to be consistent with other languages (e.g. C/C++).
   SageBuilder::setSourcePositionClassificationMode(SageBuilder::e_sourcePositionFrontendConstruction);
//TODO      SageBuilder::setSourcePositionClassificationMode(SageBuilder::e_sourcePositionCompilerGenerated);

    SgGlobal* globalScope = file->get_globalScope();
    ROSE_ASSERT(globalScope != NULL);
    ROSE_ASSERT(globalScope->get_parent() != NULL);

 // May be case insensitive (Fortran)
    globalScope->setCaseInsensitive(pCaseInsensitive);

 // DQ (8/21/2008): endOfConstruct is not set to be consistent with startOfConstruct.
    ROSE_ASSERT(globalScope->get_endOfConstruct()   != NULL);
    ROSE_ASSERT(globalScope->get_startOfConstruct() != NULL);

 // DQ (10/10/2010): Set the start position of global scope to "1".
    globalScope->get_startOfConstruct()->set_line(1);

 // DQ (10/10/2010): Set this position to the same value so that if we increment
 // by "1" the start and end will not be the same value.
    globalScope->get_endOfConstruct()->set_line(1);

    ROSE_ASSERT(SageBuilder::emptyScopeStack() == true);
    SageBuilder::pushScopeStack(globalScope);

#if WHERE_IS_DEBUG_COMMENT_LEVEL
    if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
       {
          SageBuilder::topScopeStack()->get_startOfConstruct()->display("In initialize_global_scope(): start");
          SageBuilder::topScopeStack()->get_endOfConstruct  ()->display("In initialize_global_scope(): end");
       }
#endif

    return globalScope;
}


// Lookup a C style function symbol to create a function reference expression to it
// Copied from SageBuilder which had removed it from the API.  However, it is usefule
// for Jovial until intrinsic function are declared.  If a first pass is made over the
// untyped nodes to make non-defining function declarations then this wouldn't be necessary.
//
// WARNING: This should be removed in a redesign using class member variables to save state.
//
SgFunctionRefExp *
UntypedConverter::buildFunctionRefExp(const SgName& name, SgScopeStatement* scope /*=NULL*/)
{
  if (scope == NULL)
     {
        scope = SageBuilder::topScopeStack();
     }
  ROSE_ASSERT(scope);

  SgFunctionSymbol* symbol = SageInterface::lookupFunctionSymbolInParentScopes(name,scope);

  if (symbol == NULL)
     {
        // assume int return type, and empty parameter list
           SgFunctionDeclaration* funcDecl = NULL;

           SgType* return_type = SageBuilder::buildIntType();
           SgFunctionParameterList *parList = SageBuilder::buildFunctionParameterList();
           SageInterface::setSourcePosition(parList);

           SgGlobal* globalscope = SageInterface::getGlobalScope(scope);

           funcDecl = SageBuilder::buildNondefiningFunctionDeclaration(name,return_type,parList,globalscope);

           funcDecl->get_declarationModifier().get_storageModifier().setExtern();
     }

  symbol = SageInterface::lookupFunctionSymbolInParentScopes(name,scope);
  ROSE_ASSERT(symbol);

  SgFunctionRefExp* func_ref = SageBuilder::buildFunctionRefExp(symbol);
  ROSE_ASSERT(func_ref);
  SageInterface::setSourcePosition(func_ref);

  return func_ref;
}


//! Collects code common to building function and subroutine declarations.
void
UntypedConverter::buildProcedureSupport (SgUntypedFunctionDeclaration* ut_function, SgProcedureHeaderStatement* procedureDeclaration, SgScopeStatement* scope)
   {
     ROSE_ASSERT(procedureDeclaration != NULL);

   // Convert procedure prefix (e.g., PURE ELEMENTAL ...)
      SgUntypedExprListExpression* modifiers = ut_function->get_modifiers();
      convertFunctionPrefix(modifiers, procedureDeclaration);

  // This will be the defining declaration
     procedureDeclaration->set_definingDeclaration(procedureDeclaration);
     procedureDeclaration->set_firstNondefiningDeclaration(NULL);

     SgScopeStatement* currentScopeOfFunctionDeclaration = scope;
     ROSE_ASSERT(currentScopeOfFunctionDeclaration != NULL);

     printf("...TODO... buildProcedureSupport: need to finish %p %p %p\n", scope, ut_function, procedureDeclaration);

#if 0
     if (astInterfaceStack.empty() == false)
  // TODO - figure out how to do interface declarations
        {
          SgInterfaceStatement* interfaceStatement = astInterfaceStack.front();

       // DQ (10/6/2008): The use of the SgInterfaceBody IR nodes allows the details of if
       // it was a procedure name or a procedure declaration to be abstracted away and saves
       // this detail of how it was structured in the source code in the AST (for the unparser).
          SgName name = procedureDeclaration->get_name();
          SgInterfaceBody* interfaceBody = new SgInterfaceBody(name,procedureDeclaration,/*use_function_name*/ false);
          procedureDeclaration->set_parent(interfaceStatement);
          interfaceStatement->get_interface_body_list().push_back(interfaceBody);
          interfaceBody->set_parent(interfaceStatement);
          //TODO          setSourcePosition(interfaceBody);
        }
       else
        {
#endif
       // The function was not processed as part of an interface so add it to the current scope.
          currentScopeOfFunctionDeclaration->append_statement(procedureDeclaration);
#if 0
        }
#endif

  // See if this was previously declared
     SgFunctionSymbol* functionSymbol = SageInterface::lookupFunctionSymbolInParentScopes (procedureDeclaration->get_name(), scope);

#if DEBUG_UNTYPED_CONVERTER
     printf ("  - In buildProcedureSupport(): functionSymbol = %p from trace_back_through_parent_scopes_lookup_function_symbol() \n",functionSymbol);
     printf ("  - In buildProcedureSupport(): procedureDeclaration scope = %p \n",procedureDeclaration->get_scope());
     printf ("  - In buildProcedureSupport(): currentScopeOfFunctionDeclaration = %p = %s \n",currentScopeOfFunctionDeclaration,currentScopeOfFunctionDeclaration->class_name().c_str());
#endif

     if (functionSymbol != NULL)
        {
          SgFunctionDeclaration* nondefiningDeclaration = functionSymbol->get_declaration();
          ROSE_ASSERT(nondefiningDeclaration != NULL);

          procedureDeclaration->set_firstNondefiningDeclaration(nondefiningDeclaration);

       // And set the defining declaration in the non-defining declaration
          nondefiningDeclaration->set_definingDeclaration(procedureDeclaration);

       // update scope information
          if (nondefiningDeclaration->get_scope()->symbol_exists(functionSymbol))
             {
                nondefiningDeclaration->get_scope()->remove_symbol(functionSymbol);
             }
          nondefiningDeclaration->set_scope(currentScopeOfFunctionDeclaration);
          nondefiningDeclaration->set_parent(currentScopeOfFunctionDeclaration);
          currentScopeOfFunctionDeclaration->insert_symbol(nondefiningDeclaration->get_name(), functionSymbol);
          functionSymbol->set_declaration(procedureDeclaration);  // update the defining declaration
        }
       else
        {
       // Build the function symbol and put it into the symbol table for the current scope
       // It might be that we should build a nondefining declaration for use in the symbol.
          functionSymbol = new SgFunctionSymbol(procedureDeclaration);
          currentScopeOfFunctionDeclaration->insert_symbol(procedureDeclaration->get_name(), functionSymbol);
#if DEBUG_UNTYPED_CONVERTER
          printf ("  - In buildProcedureSupport(): Added SgFunctionSymbol = %p to scope = %p = %s \n",functionSymbol,currentScopeOfFunctionDeclaration,currentScopeOfFunctionDeclaration->class_name().c_str());
#endif
        }

     SgBasicBlock*         procedureBody       = new SgBasicBlock();
     SgFunctionDefinition* procedureDefinition = new SgFunctionDefinition(procedureDeclaration,procedureBody);

     setSourcePositionFrom(procedureDefinition, ut_function->get_scope());
     setSourcePositionFrom(procedureBody,       ut_function->get_scope());

     ROSE_ASSERT(procedureDeclaration->get_definition() != NULL);

  // May be case insensitive (Fortran)
     procedureBody->setCaseInsensitive(pCaseInsensitive);
     procedureDefinition->setCaseInsensitive(pCaseInsensitive);
     procedureDeclaration->set_scope (currentScopeOfFunctionDeclaration);
     procedureDeclaration->set_parent(currentScopeOfFunctionDeclaration);

  // Function parameters are in the scope of the procedure
     SageBuilder::pushScopeStack(procedureDefinition);

#if TODO_TODO
  // Now push the function definition onto the astScopeStack (so that the function parameters will be build in the correct scope)
     astScopeStack.push_front(procedureDefinition);

  // This code is specific to the case where the procedureDeclaration is a Fortran function (not a subroutine or data block)
  // If there was a result specificed for the function then the SgInitializedName list is returned on the astNodeStack.
     if (astNodeStack.empty() == false)
        {
          SgInitializedName* returnVar = isSgInitializedName(astNodeStack.front());
          ROSE_ASSERT(returnVar != NULL);
       // returnVar->set_scope(functionBody);
          returnVar->set_parent(procedureDeclaration);
          returnVar->set_scope(procedureDefinition);
          procedureDeclaration->set_result_name(returnVar);
          astNodeStack.pop_front();

          SgFunctionType* functionType = procedureDeclaration->get_type();
          returnVar->set_type(functionType->get_return_type());

       // Now build associated SgVariableSymbol and put it into the current scope (function definition scope)
          SgVariableSymbol* returnVariableSymbol = new SgVariableSymbol(returnVar);
          procedureDefinition->insert_symbol(returnVar->get_name(),returnVariableSymbol);

       // printf ("Processing the return var in a function \n");
       // ROSE_ASSERT(false);
        }
#endif // TODO_TODO

     SgUntypedInitializedNamePtrList ut_params = ut_function->get_parameters()->get_name_list();

  // Add function arguments
     BOOST_FOREACH(SgUntypedInitializedName* ut_name, ut_params)
        {
           SgName arg_name = ut_name->get_name();

#if 0
           cout << "In buildProcedureSupport(): building function parameter name " << arg_name << endl;
#endif

        // The argument could be an alternate-return dummy argument
           SgInitializedName* initializedName = NULL;
           bool isAltReturn = (arg_name == "*");
           if (isAltReturn == true)
              {
              // DQ (2/1/2011): Since we will generate a label and with name "*" and independently resolve which
              // label argument is referenced in the return statement, we need not bury the name directly into
              // the arg_name (unless we need to have the references be separate in the symbol table, so maybe we do!).

              // Note that alternate return is an obsolescent feature in Fortran 95 and Fortran 90
              // initializedName = new SgInitializedName(arg_name,SgTypeVoid::createType(),NULL,procedureDeclaration,NULL);
                 initializedName = new SgInitializedName(arg_name,SgTypeLabel::createType(),NULL,procedureDeclaration,NULL);
              }
           else
              {
              // DQ (2/2/2011): The type might not be specified using implicit type rules, so we should likely define
              // the type as SgTypeUnknown and then fix it up later (at the end of the functions declarations).
       //           initializedName = new SgInitializedName(arg_name,generateImplicitType(arg_name.str()),NULL,procedureDeclaration,NULL);
                 initializedName = new SgInitializedName(arg_name,SgTypeUnknown::createType(),NULL,procedureDeclaration,NULL);

              }
           setSourcePositionFrom(initializedName, ut_name);
           procedureDeclaration->append_arg(initializedName);

           initializedName->set_parent(procedureDeclaration->get_parameterList());
           ROSE_ASSERT(initializedName->get_parent() != NULL);

           initializedName->set_scope(procedureDefinition);

        // TODO
        // setSourcePosition(initializedName,astNameStack.front());

           if (isAltReturn == true)
              {
              // If this is a label argument then build a SgLabelSymbol.
              // We might want them to be positionally relevant rather than name relevent,
              // this would define a mechanism that was insensitive to transformations.
              // We need a new SgLabelSymbol constructor to support the use here.
              // SgLabelSymbol* labelSymbol = new SgLabelSymbol(arg_name);
                 SgLabelSymbol* labelSymbol = new SgLabelSymbol(initializedName);
                 procedureDefinition->insert_symbol(arg_name,labelSymbol);
              }
           else
              {
              // Now build associated SgVariableSymbol and put it into the current scope (function definition scope)
                 SgVariableSymbol* variableSymbol = new SgVariableSymbol(initializedName);
                 procedureDefinition->insert_symbol(arg_name,variableSymbol);
              }

        // DQ (12/17/2007): Make sure the scope was set!
           ROSE_ASSERT(initializedName->get_scope() != NULL);
        }

     SgFunctionType* functionType = isSgFunctionType(procedureDeclaration->get_type());
     ROSE_ASSERT(functionType != NULL);

  // DQ (2/2/2011): This should be empty at this point, it will be fixed up either as we process declarations
  // in the function that will defin the types or types will be assigned using the implicit type rules (which
  // might not have even been seen yet for the function) when we are finished processing all of the functions
  // declarations.  Note that this information will be need by the alternative return support when we compute
  // the index for the unparsed code.
     ROSE_ASSERT(functionType->get_arguments().empty() == true);

     procedureBody->set_parent(procedureDefinition);
     procedureDefinition->set_parent(procedureDeclaration);

     SageBuilder::pushScopeStack(procedureBody);

     ROSE_ASSERT(procedureDeclaration->get_parameterList() != NULL);
   }
