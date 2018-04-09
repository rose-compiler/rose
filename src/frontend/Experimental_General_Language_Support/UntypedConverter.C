#include "sage3basic.h"
#include "UntypedConverter.h"
#include "general_language_translation.h"

#define DEBUG_UNTYPED_CONVERTER 0

using namespace Untyped;
using std::cout;
using std::cerr;
using std::endl;

void
UntypedConverter::setSourcePositionUnknown(SgLocatedNode* locatedNode)
{
  // This function sets the source position to be marked as not available (since we don't have token information)
  // These nodes WILL be unparsed in the code generation phase.

#if DEBUG_UNTYPED_CONVERTER
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

   ROSE_ASSERT(start != NULL && end != NULL);

// SageBuilder may have been used and it builds FileInfo
   if (toNode->get_startOfConstruct() != NULL) {
      delete toNode->get_startOfConstruct();
      toNode->set_startOfConstruct(NULL);
   }
   if (toNode->get_endOfConstruct() != NULL) {
      delete toNode->get_endOfConstruct();
      toNode->set_endOfConstruct(NULL);
   }

#if DEBUG_UNTYPED_CONVERTER
   std::cout << "UntypedConverter::setSourcePositionFrom: ";
   printf("   --- toNode: %p from: %p", toNode, fromNode);
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
UntypedConverter::setSourcePositionIncluding ( SgLocatedNode* toNode, SgLocatedNode* startNode, SgLocatedNode* endNode )
{
   ROSE_ASSERT(toNode != NULL && startNode != NULL && endNode != NULL);

   Sg_File_Info* start = startNode->get_startOfConstruct();
   Sg_File_Info* end = endNode->get_endOfConstruct();

   ROSE_ASSERT(start != NULL && end != NULL);
   ROSE_ASSERT(toNode->get_startOfConstruct() == NULL);
   ROSE_ASSERT(toNode->get_endOfConstruct()   == NULL);

#if DEBUG_UNTYPED_CONVERTER
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

//! Set a numerical label for a Fortran statement. The statement should have a enclosing function definition already. SgLabelSymbol and
//  SgLabelRefExp are created transparently as needed.
void
UntypedConverter::setFortranNumericLabel(SgStatement* stmt, int label_value, SgLabelSymbol::label_type_enum label_type, SgScopeStatement* label_scope)
{
// TODO - convert from Fortran specific (at least in name and perhaps digit values, ...)
   ROSE_ASSERT (stmt != NULL);
   ROSE_ASSERT (label_value >0 && label_value <=99999); //five digits for Fortran label

   if (label_scope == NULL)
      {
         label_scope = SageInterface::getEnclosingFunctionDefinition(stmt);
      }
   ROSE_ASSERT (label_scope != NULL);

   SgName label_name(Rose::StringUtility::numberToString(label_value));
   SgLabelSymbol * symbol = label_scope->lookup_label_symbol (label_name);
   if (symbol == NULL)
      {
      // DQ (12/4/2011): This is the correct handling for SgLabelStatement (always in the function scope, same as C and C++).
      // DQ (2/2/2011): We want to call the old constructor (we now have another constructor that takes a SgInitializedName pointer).
      // symbol = new SgLabelSymbol(NULL);
         symbol = new SgLabelSymbol((SgLabelStatement*) NULL);
         ROSE_ASSERT(symbol != NULL);
         symbol->set_fortran_statement(stmt);
         symbol->set_numeric_label_value(label_value);
         symbol->set_label_type(label_type);
         label_scope->insert_symbol(label_name,symbol);
      }
   else
      {
         cerr << "Error. SageInterface::setFortranNumericLabel() tries to set a duplicated label value!" << endl;
         ROSE_ASSERT (false);
      }

   SgLabelRefExp* ref_exp = SageBuilder::buildLabelRefExp(symbol);
   ref_exp->set_parent(stmt);

   switch(label_type)
      {
        case SgLabelSymbol::e_start_label_type:
           {
              stmt->set_numeric_label(ref_exp);
              break;
           }
        case SgLabelSymbol::e_end_label_type:
           {
              stmt->set_end_numeric_label(ref_exp);
              break;
           }
         default:
            {
               fprintf(stderr, "SageInterface::setFortranNumericLabel: unimplemented for label_type %d \n", label_type);
               ROSE_ASSERT(0);  // NOT IMPLEMENTED
            }
        }
}

void
UntypedConverter::convertFunctionPrefix (SgUntypedExprListExpression* prefix_list, SgFunctionDeclaration* function_decl)
{
   BOOST_FOREACH(SgUntypedExpression* ut_expr, prefix_list->get_expressions())
   {
      ROSE_ASSERT( isSgUntypedOtherExpression(ut_expr) );

      switch(ut_expr->get_expression_enum())
       {
         case General_Language_Translation::e_function_modifier_elemental:
            {
               cout << "..SETTING.. elemental \n";
               function_decl->get_functionModifier().setElemental();
               break;
            }
#if 0
      // TODO
         case General_Language_Translation::e_function_modifier_impure:
            {
               function_decl->get_functionModifier().setImpure();
               break;
            }
      // TODO
         case General_Language_Translation::e_function_modifier_module:
            {
               function_decl->get_functionModifier().setModule();
               break;
            }
#endif
         case General_Language_Translation::e_function_modifier_pure:
            {
               function_decl->get_functionModifier().setPure();
               break;
            }
         case General_Language_Translation::e_function_modifier_recursive:
            {
               function_decl->get_functionModifier().setRecursive();
               break;
            }

       // CUDA function modifiers/qualifiers
       // ----------------------------------
          case General_Language_Translation::e_cuda_host:
            {
               function_decl->get_functionModifier().setCudaHost();
               break;
            }
          case General_Language_Translation::e_cuda_global_function:
            {
               function_decl->get_functionModifier().setCudaGlobalFunction();
               break;
            }
          case General_Language_Translation::e_cuda_device:
            {
               function_decl->get_functionModifier().setCudaDevice();
               break;
            }
          case General_Language_Translation::e_cuda_grid_global:
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
UntypedConverter::setDeclarationModifiers (SgDeclarationStatement* decl, SgUntypedExprListExpression* modifier_list)
{
   // Set to undefined and change if necessary
   decl->get_declarationModifier().get_accessModifier().setUndefined();

   BOOST_FOREACH(SgUntypedExpression* ut_expr, modifier_list->get_expressions())
    {
       ROSE_ASSERT( isSgUntypedOtherExpression(ut_expr) );

       switch(ut_expr->get_expression_enum())
        {
          case General_Language_Translation::e_access_modifier_public:
            {
               decl->get_declarationModifier().get_accessModifier().setPublic();
               break;
            }
          case General_Language_Translation::e_access_modifier_private:
            {
               decl->get_declarationModifier().get_accessModifier().setPrivate();
               break;
            }
          case General_Language_Translation::e_type_modifier_allocatable:
            {
               decl->get_declarationModifier().get_typeModifier().setAllocatable();
               break;
            }
          case General_Language_Translation::e_type_modifier_asynchronous:
            {
               decl->get_declarationModifier().get_typeModifier().setAsynchronous();
               break;
            }
          case General_Language_Translation::e_storage_modifier_contiguous:
            {
               decl->get_declarationModifier().get_storageModifier().setContiguous();
               break;
            }
          case General_Language_Translation::e_storage_modifier_external:
            {
               decl->get_declarationModifier().get_storageModifier().setExtern();
               break;
            }
           case General_Language_Translation::e_type_modifier_intent_in:
            {
               decl->get_declarationModifier().get_typeModifier().setIntent_in();
               break;
            }
          case General_Language_Translation::e_type_modifier_intent_out:
            {
               decl->get_declarationModifier().get_typeModifier().setIntent_out();
               break;
            }
          case General_Language_Translation::e_type_modifier_intent_inout:
            {
               decl->get_declarationModifier().get_typeModifier().setIntent_inout();
               break;
            }
          case General_Language_Translation::e_type_modifier_intrinsic:
            {
               decl->get_declarationModifier().get_typeModifier().setIntrinsic();
               break;
            }
          case General_Language_Translation::e_type_modifier_optional:
            {
               decl->get_declarationModifier().get_typeModifier().setOptional();
               break;
            }
#if 0
// TODO - Fortran is a special case and must be handled differently
          case Fortran_ROSE_Translation::e_parameter:
          case General_Language_Translation::e_type_modifier_const:
            {
               decl->get_declarationModifier().get_typeModifier().get_constVolatileModifier().setConst();
               break;
            }
#endif
#if 0
// TODO - Fortran is a special case and must be handled differently
          case General_Language_Translation::e_pointer:
            {
               decl->get_declarationModifier().get_typeModifier().setPointer();
               break;
            }
#endif
#if 0
// TODO - Fortran is a special case and must be handled differently
          case General_Language_Translation::e_type_modifier_protected:
            {
               decl->get_declarationModifier().get_typeModifier().setProtected();
               break;
            }
#endif
          case General_Language_Translation::e_type_modifier_save:
            {
               decl->get_declarationModifier().get_typeModifier().setSave();
               break;
            }
          case General_Language_Translation::e_type_modifier_target:
            {
               decl->get_declarationModifier().get_typeModifier().setTarget();
               break;
            }
          case General_Language_Translation::e_type_modifier_value:
            {
               decl->get_declarationModifier().get_typeModifier().setValue();
               break;
            }
          case General_Language_Translation::e_type_modifier_volatile:
            {
               decl->get_declarationModifier().get_typeModifier().get_constVolatileModifier().setVolatile();
               break;
            }

       // CUDA variable attributes/qualifiers
       // -----------------------------------
          case General_Language_Translation::e_cuda_device_memory:
            {
               decl->get_declarationModifier().get_storageModifier().setCudaDeviceMemory();
               break;
            }
          case General_Language_Translation::e_cuda_managed:
            {
               decl->get_declarationModifier().get_storageModifier().setCudaManaged();
               break;
            }
          case General_Language_Translation::e_cuda_constant:
            {
               decl->get_declarationModifier().get_storageModifier().setCudaConstant();
               break;
            }
          case General_Language_Translation::e_cuda_shared:
            {
               decl->get_declarationModifier().get_storageModifier().setCudaShared();
               break;
            }
          case General_Language_Translation::e_cuda_pinned:
            {
               decl->get_declarationModifier().get_storageModifier().setCudaPinned();
               break;
            }
          case General_Language_Translation::e_cuda_texture:
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
UntypedConverter::convertSgUntypedType (SgUntypedType* ut_type, SgScopeStatement* scope)
{
   SgType* sg_type = NULL;

   if (isSgUntypedArrayType(ut_type)) {
      cerr << "HELP --- found an array type\n\n";
   }

// Temporary assertions as this conversion is completed
   ROSE_ASSERT(ut_type->get_is_literal() == false);
   ROSE_ASSERT(ut_type->get_is_class() == false);
   ROSE_ASSERT(ut_type->get_is_constant() == false);

// If a nonintrinsic type we only need the name
// TODO - Fortran needs type-attr-spec-list
   if (ut_type->get_is_intrinsic() == false) {
      ROSE_ASSERT(ut_type->get_is_user_defined() == true);
  //  TODO: sg_type = SgClassType::createType(ut_type->get_type_name());
      ROSE_ASSERT(sg_type != NULL);

      cout << "convert SgUntypedType NAMED TYPE ..................... " << sg_type->class_name() << endl;

      return sg_type;
   }

   ROSE_ASSERT(ut_type->get_is_user_defined() == false);

   SgExpression*   kindExpression = NULL;
   SgExpression* lengthExpression = NULL;

   if (ut_type->get_has_kind())
      {
         SgExpressionPtrList children;
         SgUntypedExpression* ut_kind = ut_type->get_type_kind();
      // TODO - figure out how to handle operators (or anything with children)
         ROSE_ASSERT(isSgUntypedValueExpression(ut_kind) != NULL || isSgUntypedReferenceExpression(ut_kind) != NULL);
         kindExpression = convertSgUntypedExpression(ut_kind, children);
      }
   if (ut_type->get_char_length_is_string())
      {
         SgExpressionPtrList children;
         SgUntypedExpression* ut_length = ut_type->get_char_length_expression();
      // TODO - figure out how to handle operators (or anything with children)
         ROSE_ASSERT(isSgUntypedValueExpression(ut_length) != NULL || isSgUntypedReferenceExpression(ut_length) != NULL);
         lengthExpression = convertSgUntypedExpression(ut_length, children);
      }

// TODO - determine if SageBuilder can be used (or perhaps should be updated)
   switch(ut_type->get_type_enum_id())
      {
        case SgUntypedType::e_void:           sg_type = SageBuilder::buildVoidType();              break;
        case SgUntypedType::e_int:            sg_type = SgTypeInt::createType(0, kindExpression);  break;
        case SgUntypedType::e_float:          sg_type = SgTypeFloat::createType(kindExpression);   break;
        case SgUntypedType::e_double:         sg_type = SageBuilder::buildDoubleType();            break;

     // complex types
        case SgUntypedType::e_complex:        sg_type = SgTypeComplex::createType(SgTypeFloat::createType(kindExpression), kindExpression); break;
        case SgUntypedType::e_double_complex: sg_type = SgTypeComplex::createType(SgTypeDouble::createType());                              break;

        case SgUntypedType::e_bool:           sg_type = SgTypeBool::createType(kindExpression);    break;

     // character and string types
        case SgUntypedType::e_char:
           {
              if (lengthExpression)
                 {
                    sg_type = SgTypeString::createType(lengthExpression, kindExpression);          break;
                 }
              else
                 {
                    sg_type = SgTypeChar::createType(kindExpression);                              break;
                 }
           }

        default:
           {
              fprintf(stderr, "UntypedConverter::convertSgUntypedType: failed to find known type, enum is %d \n", ut_type->get_type_enum_id());
              ROSE_ASSERT(0);
           }
      }

// TODO - determine if this is necessary
   if (kindExpression != NULL)
      {
         kindExpression->set_parent(sg_type);
      }
   if (lengthExpression != NULL)
      {
         lengthExpression->set_parent(sg_type);
      }

   ROSE_ASSERT(sg_type != NULL);

   return sg_type;
}


SgInitializedName*
UntypedConverter::convertSgUntypedInitializedName (SgUntypedInitializedName* ut_name, SgType* sg_base_type)
{
   SgType* sg_type = sg_base_type;

#if 0
   std::cerr << "convertSgUntypedInitializedName:    name is " << ut_name->get_name() << endl;
   std::cerr << "convertSgUntypedInitializedName:  b_type is " << sg_base_type->class_name() << endl;
   std::cerr << "convertSgUntypedInitializedName: ut_type is " << ut_name->get_type()->class_name() << endl;
#endif

   if (isSgUntypedArrayType(ut_name->get_type()))
      {
         cout << "convertSgUntypedInitializedName:   YIKES this is an array, quick, DO SOMETHING!" << endl;

         SgUntypedArrayType* ut_array_type = isSgUntypedArrayType(ut_name->get_type());
         SgExprListExp* sg_dim_info = convertSgUntypedExprListExpression(ut_array_type->get_dim_info());

         sg_type = SageBuilder::buildArrayType(sg_base_type, sg_dim_info);
      }

   SgInitializedName* sg_name = SageBuilder::buildInitializedName(ut_name->get_name(), sg_type /*, sg_init*/);
   setSourcePositionFrom(sg_name, ut_name);

#if DEBUG_UNTYPED_CONVERTER
   printf("--- finished converting initialized name %s\n", ut_name->get_name().c_str());
#endif

   return sg_name;
}

SgInitializedNamePtrList*
UntypedConverter::convertSgUntypedInitializedNameList (SgUntypedInitializedNameList* ut_name_list, SgType* sg_base_type)
{
   SgUntypedInitializedNamePtrList & ut_names = ut_name_list->get_name_list();

   std::cerr << "convertSgUntypedInitializedNameList: size is " << ut_names.size() << endl;

   SgInitializedNamePtrList* sg_names = new SgInitializedNamePtrList();

   BOOST_FOREACH(SgUntypedInitializedName* ut_name, ut_names)
      {
         SgInitializedName* sg_name = convertSgUntypedInitializedName(ut_name, sg_base_type);
         sg_names->push_back(sg_name);
      }

   return sg_names;
}

SgGlobal*
UntypedConverter::convertSgUntypedGlobalScope (SgUntypedGlobalScope* ut_scope, SgScopeStatement* scope)
{
// The global scope should not have executables
   ROSE_ASSERT(ut_scope->get_statement_list()  -> get_traversalSuccessorContainer().size() == 0);

   SgGlobal* sg_scope = isSgGlobal(scope);
   ROSE_ASSERT(sg_scope == SageBuilder::getGlobalScopeFromScopeStack());

   return sg_scope;
}

void
UntypedConverter::convertSgUntypedFunctionDeclarationList (SgUntypedFunctionDeclarationList* ut_list, SgScopeStatement* scope)
   {
   // Only a Fortran specific implementation needed for now
   }

SgModuleStatement*
UntypedConverter::convertSgUntypedModuleDeclaration (SgUntypedModuleDeclaration* ut_module, SgScopeStatement* scope)
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

  // Set the defining and no-defining declarations in the non-defining class declaration!
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
UntypedConverter::convertSgUntypedProgramHeaderDeclaration (SgUntypedProgramHeaderDeclaration* ut_program, SgScopeStatement* scope)
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
   printf("--- finished building program %s\n", programDeclaration->get_name().str());
#endif

   ROSE_ASSERT(programBody == SageBuilder::topScopeStack());
   ROSE_ASSERT(programDeclaration->get_firstNondefiningDeclaration() == NULL);

   return programDeclaration;
}


SgProcedureHeaderStatement*
UntypedConverter::convertSgUntypedSubroutineDeclaration (SgUntypedSubroutineDeclaration* ut_function, SgScopeStatement* scope)
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

printf ("...TODO... convert untyped sub: scope type ... %s\n", scope->class_name().c_str());

      buildProcedureSupport(ut_function, subroutineDeclaration, scope);

      return subroutineDeclaration;
   }


SgProcedureHeaderStatement*
UntypedConverter::convertSgUntypedFunctionDeclaration (SgUntypedFunctionDeclaration* ut_function, SgScopeStatement* scope)
{
   SgName name = ut_function->get_name();

// TODO - fix function type
   SgType* returnType = SgTypeVoid::createType();
   SgFunctionType* functionType = new SgFunctionType(returnType, false);

// Note that a ProcedureHeaderStatement is derived from a SgFunctionDeclaration (and is Fortran specific).
   SgProcedureHeaderStatement* functionDeclaration = new SgProcedureHeaderStatement(name, functionType, NULL);
   setSourcePositionFrom(functionDeclaration, ut_function);
// TODO - for now (param_list should have its own source position
   setSourcePositionFrom(functionDeclaration->get_parameterList(), ut_function);

// Mark this as a function.
   functionDeclaration->set_subprogram_kind(SgProcedureHeaderStatement::e_function_subprogram_kind);

// TODO - suffix
   printf ("...TODO... convert suffix\n");

printf ("...TODO... convert untyped function: scope type ... %s\n", scope->class_name().c_str());

   buildProcedureSupport(ut_function, functionDeclaration, scope);

   return functionDeclaration;
}


SgProcedureHeaderStatement*
UntypedConverter::convertSgUntypedBlockDataDeclaration (SgUntypedBlockDataDeclaration* ut_block_data, SgScopeStatement* scope)
   {
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

      SgBasicBlock*         blockDataBody       = new SgBasicBlock();
      SgFunctionDefinition* blockDataDefinition = new SgFunctionDefinition(blockDataDeclaration, blockDataBody);

      setSourcePositionFrom(blockDataDefinition, ut_block_data);
      setSourcePositionFrom(blockDataBody,       ut_block_data->get_declaration_list());

      ROSE_ASSERT(blockDataDeclaration->get_definition() != NULL);

   // May be case insensitive (Fortran)
      blockDataBody->setCaseInsensitive(pCaseInsensitive);
      blockDataDefinition->setCaseInsensitive(pCaseInsensitive);
      blockDataDeclaration->set_scope (currentScopeOfFunctionDeclaration);
      blockDataDeclaration->set_parent(currentScopeOfFunctionDeclaration);

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


//TODO-WARNING: This needs help!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
SgVariableDeclaration*
UntypedConverter::convertSgUntypedVariableDeclaration (SgUntypedVariableDeclaration* ut_decl, SgScopeStatement* scope)
{
#if 0
   ROSE_ASSERT(scope->variantT() == V_SgBasicBlock || scope->variantT() == V_SgClassDefinition);

   SgUntypedType* ut_base_type = ut_decl->get_type();
   SgType*        sg_base_type = convertSgUntypedType(ut_base_type, scope);

// Apparently Jovial can only have one variable per declaration statement
   ROSE_ASSERT(ut_decl->get_variables()->get_name_list().size() == 1);

   SgName name = ut_decl->get_variables()->get_name_list().front()->get_name();

   SgVariableDeclaration* sg_decl = SageBuilder::buildVariableDeclaration_nfi(name, sg_base_type, /*sg_init*/NULL, scope);
   setSourcePositionFrom(sg_decl, ut_decl);

   scope->append_statement(sg_decl);
   convertLabel(ut_decl, sg_decl);

   return sg_decl;

//------------------------ FORTRAN + --------------------------

#else
   ROSE_ASSERT(scope->variantT() == V_SgBasicBlock || scope->variantT() == V_SgClassDefinition);

   SgUntypedType* ut_base_type = ut_decl->get_type();
   SgType*        sg_base_type = convertSgUntypedType(ut_base_type, scope);

   SgUntypedInitializedNamePtrList ut_vars = ut_decl->get_variables()->get_name_list();
   SgUntypedInitializedNamePtrList::const_iterator it;

#if 0
   std::cerr << "convertSgUntypedVariableDeclaration: # vars is " << ut_vars.size() << endl;
   std::cerr << "convertSgUntypedVariableDeclaration: b_type is " << sg_base_type->class_name() << endl;
   std::cerr << "convertSgUntypedVariableDeclaration:   name is " << ut_vars[0]->get_name() << endl;
   ROSE_ASSERT(ut_vars[0]->get_type() != NULL);
   std::cerr << "convertSgUntypedVariableDeclaration:   type is " << ut_vars[0]->get_type()->class_name() << endl;
#endif

   SgInitializedNamePtrList sg_name_list;
// TODO: convertSgUntypedInitializedNameList(ut_decl->get_variables(), sg_base_type);

// Declare the first variable
#if 0
//TODO - not sure this is correct and is ackward anyway as it would be nice to create a variable declaration
// without any variables and then add them all later.
// SgVariableDeclaration* sg_decl = SageBuilder::buildVariableDeclaration    ((*i)->get_name(), sg_type, /*sg_init*/NULL, scope);
   SgVariableDeclaration* sg_decl = SageBuilder::buildVariableDeclaration_nfi((*i)->get_name(), sg_type, /*sg_init*/NULL, scope);
#endif

#if 1
   SgVariableDeclaration* sg_decl = new SgVariableDeclaration();
   setSourcePositionFrom(sg_decl, ut_decl);

#if 0
   std::cerr << "convertSgUntypedVariableDeclaration:    var is " << sg_decl << endl;
#endif

   sg_decl->set_parent(scope);
   //   sg_decl->set_definingDeclaration(sg_decl);
   sg_decl->set_firstNondefiningDeclaration(sg_decl);
   setDeclarationModifiers(sg_decl, ut_decl->get_modifiers());
#endif

// add variables
   for (it = ut_vars.begin(); it != ut_vars.end(); it++)
   {
         // TODO
         //   1. initializer
         //   2. CharLength: SgTypeString::createType(charLenExpr, typeKind)
         //   3. ArraySpec: buildArrayType
         //   4. CoarraySpec: buildArrayType with coarray attribute
         //   5. Pointers: new SgPointerType(sg_type)
         //   7. Dan warned me about sharing types but it looks like the base type is shared in inames
      SgInitializedName* initializedName = convertSgUntypedInitializedName((*it), sg_base_type);
      SgName variableName = initializedName->get_name();

#if 0
      std::cerr << "convertSgUntypedVariableDeclaration: class name is " << (*it)->get_type()->class_name()   << endl;
#endif

      initializedName->set_declptr(sg_decl);
      sg_decl->append_variable(initializedName, initializedName->get_initializer());

      SgVariableSymbol* variableSymbol = NULL;
      SgFunctionDefinition* functionDefinition = SageInterface::getEnclosingProcedure(scope);
      if (functionDefinition != NULL)
      {
      // Check in the function definition for an existing symbol
         variableSymbol = functionDefinition->lookup_variable_symbol(variableName);
         if (variableSymbol != NULL)
         {
            std::cout << "--- but variable symbol is _NOT_ NULL for " << variableName << std::endl;

         // This variable symbol has already been placed into the function definition's symbol table
         // Link the SgInitializedName in the variable declaration with its entry in the function parameter list.
            initializedName->set_prev_decl_item(variableSymbol->get_declaration());
         // Set the referenced type in the function parameter to be the same as that in the declaration being processed.
            variableSymbol->get_declaration()->set_type(initializedName->get_type());
         // Function parameters are in the scope of the function definition (same for C/C++)
            initializedName->set_scope(functionDefinition);
         }
      }

      if (variableSymbol == NULL)
      {
      // Check the current scope
         variableSymbol = scope->lookup_variable_symbol(variableName);

         initializedName->set_scope(scope);
         if (variableSymbol == NULL)
         {
            variableSymbol = new SgVariableSymbol(initializedName);
            scope->insert_symbol(variableName,variableSymbol);
            ROSE_ASSERT (initializedName->get_symbol_from_symbol_table () != NULL);
         }
      }
      ROSE_ASSERT(variableSymbol != NULL);
      ROSE_ASSERT(initializedName->get_scope() != NULL);
   }

   scope->append_statement(sg_decl);
   convertLabel(ut_decl, sg_decl);

   //        SgInitializedNamePtrList& varList = varDecl->get_variables ();
   //        SgInitializedName* firstInitializedNameForSourcePosition = varList.front();
   //        SgInitializedName* lastInitializedNameForSourcePosition = varList.back();
   //        ROSE_ASSERT(DeclAttributes.getDeclaration()->get_startOfConstruct() != NULL);
   //        ROSE_ASSERT(firstInitializedNameForSourcePosition->get_startOfConstruct() != NULL);
   //        ROSE_ASSERT(lastInitializedNameForSourcePosition->get_startOfConstruct() != NULL);
   //        *(DeclAttributes.getDeclaration()->get_startOfConstruct()) = *(firstInitializedNameForSourcePosition->get_startOfConstruct());
   //        *(DeclAttributes.getDeclaration()->get_endOfConstruct()) = *(lastInitializedNameForSourcePosition->get_startOfConstruct());
   //        DeclAttributes.reset();

#if DEBUG_UNTYPED_CONVERTER
   printf("--- finished converting type-declaration-stmt %s\n", sg_decl->class_name().c_str());
#endif

   return sg_decl;
#endif
}

SgImplicitStatement*
UntypedConverter::convertSgUntypedImplicitDeclaration(SgUntypedImplicitDeclaration* ut_decl, SgScopeStatement* scope)
   {
   // Only a Fortran specific implementation needed for now
      cerr << "UntypedConverter::convertSgUntypedImplicitDeclaration must be implemented in derived class if needed" << endl;
      ROSE_ASSERT(0);

      return NULL;
   }

SgDeclarationStatement*
UntypedConverter::convertSgUntypedNameListDeclaration (SgUntypedNameListDeclaration* ut_decl, SgScopeStatement* scope)
   {
      SgUntypedNamePtrList ut_names = ut_decl->get_names()->get_name_list();
      SgUntypedNamePtrList::const_iterator it;

      switch (ut_decl->get_statement_enum())
        {
        case SgToken::FORTRAN_IMPORT:
           {
              SgImportStatement* importStatement = new SgImportStatement();
              setSourcePositionFrom(importStatement, ut_decl);

              importStatement->set_definingDeclaration(importStatement);
              importStatement->set_firstNondefiningDeclaration(importStatement);

              SgExpressionPtrList localList;

              for (it = ut_names.begin(); it != ut_names.end(); it++)
              {
                 SgName name = (*it)->get_name();
                 std::cout << "... IMPORT name is " << name << std::endl;
                 SgVariableSymbol* variableSymbol = SageInterface::lookupVariableSymbolInParentScopes(name, scope);
                 ROSE_ASSERT(variableSymbol != NULL);

                 SgVarRefExp* variableReference = new SgVarRefExp(variableSymbol);
              // TODO
              // setSourcePositionFrom(variableReference);

                 importStatement->get_import_list().push_back(variableReference);
              }
              scope->append_statement(importStatement);
           }

        case SgToken::FORTRAN_EXTERNAL:
          {
          // TODO - name seems to need a parent found in get_name sageInterface.c, line 1528
          //      - actually may be attr_spec_stmt without a parent
             SgAttributeSpecificationStatement* attr_spec_stmt = new SgAttributeSpecificationStatement();
             setSourcePositionFrom(attr_spec_stmt, ut_decl);

             attr_spec_stmt->set_definingDeclaration(attr_spec_stmt);
             attr_spec_stmt->set_firstNondefiningDeclaration(attr_spec_stmt);

             attr_spec_stmt->set_attribute_kind(SgAttributeSpecificationStatement::e_externalStatement);

          // Build the SgExprListExp in the attributeSpecificationStatement if it has not already been built
          // TODO - check to see if this is done in constructor?????????
             if (attr_spec_stmt->get_parameter_list() == NULL)
                {
                   SgExprListExp* parameterList = new SgExprListExp();
                   attr_spec_stmt->set_parameter_list(parameterList);
                   parameterList->set_parent(attr_spec_stmt);
                   setSourcePositionUnknown(parameterList);
                }

             for (it = ut_names.begin(); it != ut_names.end(); it++)
             {
                std::string name = (*it)->get_name();
                std::cout << "... EXTERNAL name is " << name << std::endl;

#if 0
             // TODO - pick and implement one of these
                SgExpression* parameterExpression = astExpressionStack.front();
                SgFunctionRefExp* functionRefExp = generateFunctionRefExp(nameToken);

                attr_spec_stmt->get_parameter_list()->prepend_expression(parameterExpression);
#endif
             }
             scope->append_statement(attr_spec_stmt);     
             convertLabel(ut_decl, attr_spec_stmt);

             return attr_spec_stmt;
         }

       default:
          {
             fprintf(stderr, "UntypedConverter::convertSgUntypedNameListDeclaration: failed to find known statement enum, is %d\n", ut_decl->get_statement_enum());
             ROSE_ASSERT(0);
          }
       }
   }


// Executable statements
//----------------------

SgExprStatement*
UntypedConverter::convertSgUntypedAssignmentStatement (SgUntypedAssignmentStatement* ut_stmt, SgExpressionPtrList& children, SgScopeStatement* scope)
   {
      SgExprStatement* expressionStatement = NULL;

      ROSE_ASSERT(children.size() == 2);
      SgExpression* lhs = isSgExpression(children[0]);
      SgExpression* rhs = isSgExpression(children[1]);

      if ( lhs != NULL && rhs != NULL )
         {
            SgExpression* assignmentExpr = new SgAssignOp(lhs, rhs, NULL);
            setSourcePositionIncluding(assignmentExpr, lhs, rhs);

         // lhs expression now becomes an lvalue
            lhs->set_lvalue(true);

            SgExprStatement* expressionStatement = new SgExprStatement(assignmentExpr);
            setSourcePositionFrom(expressionStatement, ut_stmt);

            scope->append_statement(expressionStatement);

            convertLabel(ut_stmt, expressionStatement);
         }

      return expressionStatement;
   }

SgStatement*
UntypedConverter::convertSgUntypedExpressionStatement (SgUntypedExpressionStatement* ut_stmt, SgExpressionPtrList& children, SgScopeStatement* scope)
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
        case SgToken::FORTRAN_RETURN:
          {
             sg_stmt = new SgReturnStmt(sg_expr);
             break;
          }
        default:
          {
             fprintf(stderr, "UntypedConverter::convertSgUntypedExpressionStatement: failed to find known statement enum, is %d\n", ut_stmt->get_statement_enum());
             ROSE_ASSERT(0);
          }
      }
      
      ROSE_ASSERT(sg_stmt != NULL);
      setSourcePositionFrom(sg_stmt, ut_stmt);

   // any IR node can have a parent, it makes sense to associate the expression with the statement
      sg_expr->set_parent(sg_stmt);

      scope->append_statement(sg_stmt);

      convertLabel(ut_stmt, sg_stmt);

      return sg_stmt;
   }

SgStatement*
UntypedConverter::convertSgUntypedOtherStatement (SgUntypedOtherStatement* ut_stmt, SgScopeStatement* scope)
   {
      switch (ut_stmt->get_statement_enum())
        {

    // Nothing so far for general languages

       default:
          {
             fprintf(stderr, "UntypedConverter::convertSgUntypedOtherStatement: failed to find known statement enum, is %d\n", ut_stmt->get_statement_enum());
             ROSE_ASSERT(0);
          }
       }
   }

SgNullStatement*
UntypedConverter::convertSgUntypedNullStatement (SgUntypedNullStatement* ut_stmt, SgScopeStatement* scope)
   {
      SgNullStatement* nullStatement = new SgNullStatement();
      setSourcePositionFrom(nullStatement, ut_stmt);

      scope->append_statement(nullStatement);

      return nullStatement;
   }


// Expressions
//

SgExpression*
UntypedConverter::convertSgUntypedExpression(SgUntypedExpression* ut_expr)
   {
      SgExpression* sg_expr = NULL;

      switch (ut_expr->variantT())
         {
           case V_SgUntypedSubscriptExpression:
              {
                 SgUntypedSubscriptExpression* ut_subscript_expr = isSgUntypedSubscriptExpression(ut_expr);
                 sg_expr = convertSgUntypedSubscriptExpression(ut_subscript_expr);
                 break;
              }
           case V_SgUntypedNullExpression:
              {
                 sg_expr = new SgNullExpression();
                 setSourcePositionFrom(sg_expr, ut_expr);
                 break;
              }
           case V_SgUntypedReferenceExpression:
              {
                 SgUntypedReferenceExpression* ref_expr = isSgUntypedReferenceExpression(ut_expr);
                 sg_expr = SageBuilder::buildVarRefExp(ref_expr->get_name(), NULL);
                 ROSE_ASSERT(sg_expr != NULL);
                 setSourcePositionFrom(sg_expr, ut_expr);

#if DEBUG_UNTYPED_CONVERTER
                 printf ("  - reference expression ==>   %s\n", ref_expr->get_name().c_str());
#endif
                 break;
         }
           case V_SgUntypedValueExpression:
              {
                 SgUntypedValueExpression* ut_value_expr = isSgUntypedValueExpression(ut_expr);
                 sg_expr = convertSgUntypedValueExpression(ut_value_expr);
                 setSourcePositionFrom(sg_expr, ut_expr);
                 break;
              }
           default:
              {
                 cerr << "UntypedConverter::convertSgUntypedExpression: unimplemented for class " << ut_expr->class_name() << endl;
                 ROSE_ASSERT(0);  // Unimplemented
              }
         }

      return sg_expr;
   }


SgExpression*
UntypedConverter::convertSgUntypedExpression(SgUntypedExpression* ut_expr, SgExpressionPtrList& children)
   {
      SgExpression* sg_expr = NULL;

      if ( isSgUntypedBinaryOperator(ut_expr) != NULL )
         {
            SgUntypedBinaryOperator* op = dynamic_cast<SgUntypedBinaryOperator*>(ut_expr);
            ROSE_ASSERT(children.size() == 2);
            SgBinaryOp* sg_operator = convertSgUntypedBinaryOperator(op, children[0], children[1]);
            sg_expr = sg_operator;
#if DEBUG_UNTYPED_CONVERTER
            printf ("  - binary operator      ==>   %s\n", op->get_operator_name().c_str());
#endif
         }
      else
         {
            cerr << "UntypedConverter::convertSgUntypedExpression: unimplemented for class " << ut_expr->class_name() << endl;
            ROSE_ASSERT(0);  // Unimplemented
         }

      return sg_expr;
   }


SgExpression*
UntypedConverter::convertSgUntypedExpression(SgUntypedExpression* ut_expr, SgExpressionPtrList& children, SgScopeStatement* scope)
   {
      SgExpression* sg_expr = NULL;

      if ( isSgUntypedBinaryOperator(ut_expr) != NULL )
         {
            SgUntypedBinaryOperator* op = dynamic_cast<SgUntypedBinaryOperator*>(ut_expr);
            ROSE_ASSERT(children.size() == 2);
            SgBinaryOp* sg_operator = convertSgUntypedBinaryOperator(op, children[0], children[1]);
            sg_expr = sg_operator;
#if DEBUG_UNTYPED_CONVERTER
            printf ("  - binary operator      ==>   %s\n", op->get_operator_name().c_str());
#endif
         }
      if ( isSgUntypedUnaryOperator(ut_expr) != NULL )
         {
            SgUntypedUnaryOperator* op = dynamic_cast<SgUntypedUnaryOperator*>(ut_expr);
            ROSE_ASSERT(children.size() == 1);
            SgUnaryOp* sg_operator = convertSgUntypedUnaryOperator(op, children[0]);
            sg_expr = sg_operator;
#if DEBUG_UNTYPED_CONVERTER
            printf ("  - unary operator       ==>   %s\n", op->get_operator_name().c_str());
#endif
         }
      else if ( isSgUntypedValueExpression(ut_expr) != NULL )
         {
            SgUntypedValueExpression* expr = dynamic_cast<SgUntypedValueExpression*>(ut_expr);
            sg_expr = convertSgUntypedValueExpression(expr);
#if DEBUG_UNTYPED_CONVERTER
            printf ("  - value expression     ==>   %s\n", expr->get_value_string().c_str());
#endif
         }
      else if ( isSgUntypedReferenceExpression(ut_expr) != NULL )
         {
            SgUntypedReferenceExpression* expr = dynamic_cast<SgUntypedReferenceExpression*>(ut_expr);
            SgVarRefExp* varRef = SageBuilder::buildVarRefExp(expr->get_name(), NULL);
            ROSE_ASSERT(varRef != NULL);

            sg_expr = varRef;
            setSourcePositionFrom(sg_expr, ut_expr);

#if DEBUG_UNTYPED_CONVERTER
            printf ("  - reference expression ==>   %s\n", expr->get_name().c_str());
#endif
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
UntypedConverter::convertSgUntypedValueExpression (SgUntypedValueExpression* ut_expr)
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
                     cerr << "WARNING: UntypedConverter::convertSgUntypedValueExpression: kind value not handled \n";
                  }

               sg_expr = new SgFloatVal(atof(ut_expr->get_value_string().c_str()), constant_text);
               setSourcePositionFrom(sg_expr, ut_expr);
               break;
            }

         default:
            {
               ROSE_ASSERT(0);  // NOT IMPLEMENTED
            }
       }

    return sg_expr;
 }

SgUnaryOp*
UntypedConverter::convertSgUntypedUnaryOperator(SgUntypedUnaryOperator* untyped_operator, SgExpression* expr)
 {
    SgUnaryOp* op = NULL;

    switch(untyped_operator->get_expression_enum())
       {
         case General_Language_Translation::e_operator_unary_plus:
            {
#if DEBUG_UNTYPED_CONVERTER
               printf("  - e_operator_unary_add: \n");
#endif
               op = new SgUnaryAddOp(expr, NULL);
               setSourcePositionFrom(op, expr);
               break;
            }
         case General_Language_Translation::e_operator_unary_minus:
            {
#if DEBUG_UNTYPED_CONVERTER
               printf("  - e_operator_unary_minus: \n");
#endif
               op = new SgMinusOp(expr, NULL);
               setSourcePositionFrom(op, expr);
               break;
            }
         case General_Language_Translation::e_operator_unary_not:
            {
#if DEBUG_UNTYPED_CONVERTER
               printf("  - e_operator_unary_not: \n");
#endif
               op = new SgNotOp(expr, NULL);
               break;
            }
         case General_Language_Translation::e_operator_unity:
            {
            // Nothing to do here
               printf("  - e_operator_unary_unity: \n");
               break;
            }

         default:
            {
               cerr << "UntypedConverter::convertSgUntypedUnaryOperator: unknown expression enum value "
                    << untyped_operator->get_expression_enum() << endl;
               ROSE_ASSERT(0);  // NOT IMPLEMENTED
            }
       }
    return op;
 }

SgBinaryOp*
UntypedConverter::convertSgUntypedBinaryOperator(SgUntypedBinaryOperator* untyped_operator, SgExpression* lhs, SgExpression* rhs)
 {
    SgBinaryOp* op = NULL;

    switch(untyped_operator->get_expression_enum())
       {
         case General_Language_Translation::e_operator_add:
            {
#if DEBUG_UNTYPED_CONVERTER
               printf("  - e_operator_add: lhs=%p rhs=%p \n", lhs, rhs);
#endif
               op = new SgAddOp(lhs, rhs, NULL);
               setSourcePositionIncluding(op, lhs, rhs);
               break;
            }
         case General_Language_Translation::e_operator_subtract:
            {
#if DEBUG_UNTYPED_CONVERTER
               printf("  - e_operator_subtract: lhs=%p rhs=%p\n", lhs, rhs);
#endif
               op = new SgSubtractOp(lhs, rhs, NULL);
               setSourcePositionIncluding(op, lhs, rhs);
               break;
            }
         case General_Language_Translation::e_operator_exponentiate:
            {
#if DEBUG_UNTYPED_CONVERTER
               printf("  - e_operator_exponentiate:\n");
#endif
               op = new SgExponentiationOp(lhs, rhs, NULL);
               setSourcePositionIncluding(op, lhs, rhs);
               break;
            }
         case General_Language_Translation::e_operator_concatenate:
            {
#if DEBUG_UNTYPED_CONVERTER
               printf("  - e_operator_concatenate:\n");
#endif
               op = new SgConcatenationOp(lhs, rhs, NULL);
               setSourcePositionIncluding(op, lhs, rhs);
               break;
            }
         case General_Language_Translation::e_operator_multiply:
            {
#if DEBUG_UNTYPED_CONVERTER
               printf("  - e_operator_multiply: lhs=%p rhs=%p\n", lhs, rhs);
#endif
               op = new SgMultiplyOp(lhs, rhs, NULL);
               setSourcePositionIncluding(op, lhs, rhs);
               break;
            }
         case General_Language_Translation::e_operator_divide:
            {
#if DEBUG_UNTYPED_CONVERTER
               printf("  - e_operator_divide: lhs=%p rhs=%p\n", lhs, rhs);
#endif
               op = new SgDivideOp(lhs, rhs, NULL);
               setSourcePositionIncluding(op, lhs, rhs);
               break;
            }
         case General_Language_Translation::e_operator_and:
            {
#if DEBUG_UNTYPED_CONVERTER
               printf("  - e_operator_and
:\n");
#endif
               op = new SgAndOp(lhs, rhs, NULL);
               setSourcePositionIncluding(op, lhs, rhs);
               break;
            }
         case General_Language_Translation::e_operator_or:
            {
#if DEBUG_UNTYPED_CONVERTER
               printf("  - e_operator_or:\n");
#endif
               op = new SgOrOp(lhs, rhs, NULL);
               setSourcePositionIncluding(op, lhs, rhs);
               break;
            }
#if 0
         case General_Language_Translation::e_operator_equiv:
            {
#if DEBUG_UNTYPED_CONVERTER
               printf("  - e_operator_equiv:\n");
#endif
               op = new SgEqualityOp(lhs, rhs, NULL);
               ROSE_ASSERT(0);  // check on logical operands
               setSourcePositionIncluding(op, lhs, rhs);
               break;
            }
         case General_Language_Translation::e_operator_not_equiv:
            {
#if DEBUG_UNTYPED_CONVERTER
               printf("  - e_operator_not_equiv:\n");
#endif
               op = new SgNotEqualOp(lhs, rhs, NULL);
               ROSE_ASSERT(0);  // check on logical operands
               setSourcePositionIncluding(op, lhs, rhs);
               break;
            }
#endif
         case General_Language_Translation::e_operator_equality:
            {
#if DEBUG_UNTYPED_CONVERTER
               printf("  - e_operator_equality:\n");
#endif
               op = new SgEqualityOp(lhs, rhs, NULL);
               setSourcePositionIncluding(op, lhs, rhs);
               break;
            }
         case General_Language_Translation::e_operator_not_equal:
            {
#if DEBUG_UNTYPED_CONVERTER
               printf("  - e_operator_not_equal:\n");
#endif
               op = new SgNotEqualOp(lhs, rhs, NULL);
               setSourcePositionIncluding(op, lhs, rhs);
               break;
            }
         case General_Language_Translation::e_operator_greater_than_or_equal:
            {
#if DEBUG_UNTYPED_CONVERTER
               printf("  - e_operator_greater_than_or_equal:\n");
#endif
               op = new SgGreaterOrEqualOp(lhs, rhs, NULL);
               setSourcePositionIncluding(op, lhs, rhs);
               break;
            }
         case General_Language_Translation::e_operator_less_than_or_equal:
            {
#if DEBUG_UNTYPED_CONVERTER
               printf("  - e_operator_less_than_or_equal:\n");
#endif
               op = new SgLessOrEqualOp(lhs, rhs, NULL);
               setSourcePositionIncluding(op, lhs, rhs);
               break;
            }
         case General_Language_Translation::e_operator_less_than:
            {
#if DEBUG_UNTYPED_CONVERTER
               printf("  -  e_operator_less_than:\n");
#endif
               op = new SgLessThanOp(lhs, rhs, NULL);
               setSourcePositionIncluding(op, lhs, rhs);
               break;
            }
         case General_Language_Translation::e_operator_greater_than:
            {
#if DEBUG_UNTYPED_CONVERTER
               printf("  - e_operator_greater_than:\n");
#endif
               op = new SgGreaterThanOp(lhs, rhs, NULL);
               setSourcePositionIncluding(op, lhs, rhs);
               break;
            }
         default:
            {
               ROSE_ASSERT(0);  // NOT IMPLEMENTED
            }
       }
    return op;
 }


SgExprListExp*
UntypedConverter::convertSgUntypedExprListExpression(SgUntypedExprListExpression* ut_expr_list)
{
   SgExprListExp* sg_expr_list = new SgExprListExp();
   setSourcePositionFrom(sg_expr_list, ut_expr_list);

   BOOST_FOREACH(SgUntypedExpression* ut_expr, ut_expr_list->get_expressions())
      {
         SgExpression* sg_expr = convertSgUntypedExpression(ut_expr);
         sg_expr_list->append_expression(sg_expr);
      }

   return sg_expr_list;
}


SgExpression*
UntypedConverter::convertSgUntypedSubscriptExpression(SgUntypedSubscriptExpression* ut_expr)
{
   cerr << "UntypedConverter::convertSgUntypedSubscriptExpression: class " << ut_expr->class_name() << endl;
   cerr << "UntypedConverter::convertSgUntypedSubscriptExpression:  enum " << ut_expr->get_expression_enum() << endl;
   cerr << "UntypedConverter::convertSgUntypedSubscriptExpression:    lb " << ut_expr->get_lower_bound()->class_name() << endl;
   cerr << "UntypedConverter::convertSgUntypedSubscriptExpression:    ub " << ut_expr->get_upper_bound()->class_name() << endl;
   cerr << "UntypedConverter::convertSgUntypedSubscriptExpression:    st " << ut_expr->get_stride()     ->class_name() << endl;

   SgExpression* sg_expr = NULL;
   SgExpression* sg_upper_bound = NULL;

   switch(ut_expr->get_expression_enum())
      {
        case General_Language_Translation::e_explicit_shape:
        case General_Language_Translation::e_explicit_dimension:
           {
              cerr << "... explicit dimension\n";
              sg_upper_bound = convertSgUntypedExpression(ut_expr->get_upper_bound());
              break;
           }
        case General_Language_Translation::e_assumed_shape:
           {
              cerr << "... assumed shape\n";
              if (isSgUntypedNullExpression(ut_expr->get_lower_bound()))
                 sg_upper_bound = new SgColonShapeExp();
              else
                 sg_upper_bound = new SgNullExpression();
              setSourcePositionUnknown(sg_upper_bound);
              break;
           }
        case General_Language_Translation::e_assumed_or_implied_shape:
        case General_Language_Translation::e_assumed_size:
        case General_Language_Translation::e_star_dimension:
           {
              cerr << "... assumed size\n";
              sg_upper_bound = new SgAsteriskShapeExp();
              setSourcePositionUnknown(sg_upper_bound);
              break;
           }
        default:
           {
              cerr << "ERROR with array shape in UntypedConverter::convertSgUntypedSubscriptExpression for : "
                   << ut_expr->get_expression_enum() << endl;
              ROSE_ASSERT(0);
           }
      }

   if (isSgUntypedNullExpression(ut_expr->get_lower_bound()))
      {
         sg_expr = sg_upper_bound;
         setSourcePositionFrom(sg_expr, ut_expr);
      }
   else
      {
         SgExpression* sg_lower_bound = convertSgUntypedExpression(ut_expr->get_lower_bound());
         SgExpression* sg_stride = new SgIntVal(1,"1");
         setSourcePositionUnknown(sg_stride);
         sg_expr = new SgSubscriptExpression(sg_lower_bound, sg_upper_bound, sg_stride);
         setSourcePositionFrom(sg_expr, ut_expr);
      }

    return sg_expr;
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

     printf("------------------buildProcedureSupport: need to finish %p %p %p\n", scope, ut_function, procedureDeclaration);

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
     SgUntypedInitializedNamePtrList::const_iterator it;

  // Add function arguments
     for (it = ut_params.begin(); it != ut_params.end(); it++)
        {
           SgUntypedInitializedName* ut_name = *it;
           SgName arg_name = ut_name->get_name();

           std::cout << "In buildProcedureSupport(): building function parameter name " << arg_name << std::endl;

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

     ROSE_ASSERT(procedureDeclaration->get_parameterList() != NULL);
   }
