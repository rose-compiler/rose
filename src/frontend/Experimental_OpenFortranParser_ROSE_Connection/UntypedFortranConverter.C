#include "sage3basic.h"
#include "UntypedFortranConverter.h"
#include "Fortran_to_ROSE_translation.h"
#include "general_language_translation.h"

#define DEBUG_UNTYPED_CONVERTER 0

using namespace Untyped;
using std::cout;
using std::cerr;
using std::endl;

bool
UntypedFortranConverter::convertLabel(SgUntypedStatement* ut_stmt, SgStatement* sg_stmt, SgScopeStatement* label_scope)
{
// Assume a start label type as the most common
   return convertLabel(ut_stmt, sg_stmt, SgLabelSymbol::e_start_label_type, label_scope);
}

bool
UntypedFortranConverter::convertLabel(SgUntypedStatement* ut_stmt, SgStatement* sg_stmt,
                                      SgLabelSymbol::label_type_enum label_type, SgScopeStatement* label_scope)
{
   bool hasLabel = false;

   std::string label_name = ut_stmt->get_label_string();
   if (!label_name.empty())
      {
         char* next;
      // SageInterface::setFortranNumericLabel(sg_stmt, std::stoul(label_name));
      // The modifications in setFortranNumericLabel should be moved to SageInterface
         setFortranNumericLabel(sg_stmt, strtoul(label_name.c_str(),&next,10), label_type, label_scope);
         ROSE_ASSERT(next != label_name.c_str());
         hasLabel = true;
      }
   return hasLabel;
}

//! Set a numerical label for a Fortran statement. The statement should have a enclosing function definition already. SgLabelSymbol and
//  SgLabelRefExp are created transparently as needed.
void
UntypedFortranConverter::setFortranNumericLabel(SgStatement* stmt, int label_value, SgLabelSymbol::label_type_enum label_type, SgScopeStatement* label_scope)
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
UntypedFortranConverter::convertUntypedFunctionDeclarationList (SgUntypedFunctionDeclarationList* ut_list, SgScopeStatement* scope)
{
   if (scope->variantT() == V_SgBasicBlock || scope->variantT() == V_SgClassDefinition)
      {
         if ( ! ut_list->get_func_list().empty() )
            {
               // Need to add a contains statement to the current scope as it currently
               // doesn't exist in OFP's Fortran AST (FAST) design (part of concrete syntax only)
               SgContainsStatement* containsStatement = new SgContainsStatement();
               UntypedConverter::setSourcePositionUnknown(containsStatement);
//TODO - maybe ok
            // ROSE_ASSERT(0);

               containsStatement->set_definingDeclaration(containsStatement);

               scope->append_statement(containsStatement);
               ROSE_ASSERT(containsStatement->get_parent() != NULL);
            }
      }
}

SgProcedureHeaderStatement*
UntypedFortranConverter::convertUntypedSubroutineDeclaration (SgUntypedSubroutineDeclaration* ut_function, SgScopeStatement* scope)
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
UntypedFortranConverter::convertUntypedFunctionDeclaration (SgUntypedFunctionDeclaration* ut_function, SgScopeStatement* scope)
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
UntypedFortranConverter::convertUntypedBlockDataDeclaration (SgUntypedBlockDataDeclaration* ut_block_data, SgScopeStatement* scope)
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

      SgBasicBlock*         blockDataBody       = SageBuilder::buildBasicBlock();
      SgFunctionDefinition* blockDataDefinition = new SgFunctionDefinition(blockDataDeclaration, blockDataBody);

      setSourcePositionFrom(blockDataDefinition, ut_block_data);
      setSourcePositionFrom(blockDataBody,       ut_block_data->get_declaration_list());

      ROSE_ASSERT(blockDataDeclaration->get_definition() != NULL);

   // Specify case insensitivity for Fortran.
      blockDataDefinition->setCaseInsensitive(true);
      blockDataDeclaration->set_scope (currentScopeOfFunctionDeclaration);
      blockDataDeclaration->set_parent(currentScopeOfFunctionDeclaration);

      SageBuilder::pushScopeStack(blockDataDefinition);
      SageBuilder::pushScopeStack(blockDataBody);

   // Convert the labels for the program begin and end statements
      UntypedFortranConverter::convertLabel(ut_block_data,                      blockDataDeclaration, SgLabelSymbol::e_start_label_type, /*label_scope=*/ blockDataDefinition);
      UntypedFortranConverter::convertLabel(ut_block_data->get_end_statement(), blockDataDeclaration, SgLabelSymbol::e_end_label_type,   /*label_scope=*/ blockDataDefinition);

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
UntypedFortranConverter::convertSgUntypedVariableDeclaration (SgUntypedVariableDeclaration* ut_decl, SgScopeStatement* scope)
{
   ROSE_ASSERT(scope->variantT() == V_SgBasicBlock || scope->variantT() == V_SgClassDefinition);

   SgUntypedType* ut_base_type = ut_decl->get_type();
   SgType*        sg_base_type = convertUntypedType(ut_base_type, scope);

   SgUntypedInitializedNamePtrList ut_vars = ut_decl->get_variables()->get_name_list();

#if 0
   cout << "--- convertSgUntypedVariableDeclaration: ut_decl: " << ut_decl << endl;
   cout << "--- convertSgUntypedVariableDeclaration:       # vars is " << ut_vars.size() << endl;
   cout << "--- convertSgUntypedVariableDeclaration: ut_base_type is " << ut_base_type->class_name()
                                                                << " " << ut_base_type << endl;
   cout << "--- convertSgUntypedVariableDeclaration: sg_base_type is " << sg_base_type->class_name() << endl;
#endif

   SgInitializedNamePtrList sg_name_list;

// Declare the first variable
#if 0
//TODO - not sure this is correct and is ackward anyway as it would be nice to create a variable declaration
// without any variables and then add them all later.
   SgVariableDeclaration* sg_decl = SageBuilder::buildVariableDeclaration(name, sg_type, /*sg_init*/NULL, scope);
#else
   SgVariableDeclaration* sg_decl = new SgVariableDeclaration();
   setSourcePositionFrom(sg_decl, ut_decl);

#if 0
   cout << "--- convertSgUntypedVariableDeclaration: sg_decl: " << sg_decl << endl;
   cout << "                                                  " << sg_decl->get_firstNondefiningDeclaration() << endl;
#endif

   sg_decl->set_parent(scope);
   sg_decl->set_definingDeclaration(sg_decl);
   setDeclarationModifiers(sg_decl, ut_decl->get_modifiers());
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
         //   7. Dan warned me about sharing types but it looks like the base type is shared in inames
      SgInitializedName* sg_init_name = convertSgUntypedInitializedName(ut_init_name, sg_base_type);
      SgName var_name = sg_init_name->get_name();

#if 0
      cout << "--- convertSgUntypedVariableDeclaration: var name is " << ut_init_name->get_name() << endl;
      cout << "--- convertSgUntypedVariableDeclaration:  ut_type is " << ut_init_name->get_type()->class_name()
                                                               << " " << ut_init_name->get_type() << endl;
      cout << "--- convertSgUntypedVariableDeclaration:  sg_type is " << sg_init_name->get_type()->class_name() << endl;
#endif

   // Finished with the untyped initialized name and associated types.  Don't delete untyped
   // initialized names now as they will be deleted after the traversal but delete types now.
      if (ut_init_name->get_type() != ut_base_type) delete ut_init_name->get_type();
      ut_init_name->set_type(NULL);
      delete ut_base_type;

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
            std::cout << "--- but variable symbol is _NOT_ NULL for " << var_name << std::endl;

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
            scope->insert_symbol(var_name,variableSymbol);
            ROSE_ASSERT (sg_init_name->get_symbol_from_symbol_table () != NULL);
         }
      }
      ROSE_ASSERT(variableSymbol != NULL);
      ROSE_ASSERT(sg_init_name->get_scope() != NULL);
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
   cout << "--- finished converting type-declaration-stmt " << sg_decl->class_name() << endl;
#endif

   return sg_decl;
}

SgDeclarationStatement*
UntypedFortranConverter::convertUntypedStructureDeclaration (SgUntypedStructureDeclaration* ut_struct, SgScopeStatement* scope)
   {
      //
      // There is a nice implementation of this all in the OFP parser implementation in fortran_support.C
      //

      SgName name = ut_struct->get_name();

   // This function builds a class declaration and definition with both the defining and nondefining declarations as required
      SgDerivedTypeStatement* type_decl = SageBuilder::buildDerivedTypeStatement(name, scope);
      ROSE_ASSERT(type_decl);
      setSourcePositionFrom(type_decl, ut_struct);

      if (ut_struct->get_modifiers()->get_expressions().size() > 0)
         {
            cout << "-x- TODO: implement type modifiers in UntypedFortranConverter::convertUntypedStructureDeclaration \n";

            SgExprListExp* sg_expr_list = convertSgUntypedExprListExpression(ut_struct->get_modifiers(),/*delete*/true);
            ROSE_ASSERT(sg_expr_list);

         // TODO: don't have anything to do with this yet, following is for Jovial
         // ROSE_ASSERT(sg_expr_list->get_expressions().size() == 1);

         // Assume that this is a Jovial_ROSE_Translation::e_words_per_entry_w

         // class_decl->set_has_table_entry_size(true);
         // class_decl->set_table_entry_size(sg_expr_list->get_expressions()[0]);
         }

      SgClassDefinition* class_def = type_decl->get_definition();
      ROSE_ASSERT(class_def);

   // Fortran is insensitive to case
      class_def->setCaseInsensitive(true);

      SgScopeStatement* class_scope = class_def->get_scope();
      ROSE_ASSERT(class_scope);

      SageInterface::appendStatement(type_decl, scope);

      SageBuilder::pushScopeStack(class_def);

      return type_decl;
   }

// R560 implicit-stmt
//
SgImplicitStatement*
UntypedFortranConverter::convertSgUntypedImplicitDeclaration(SgUntypedImplicitDeclaration* ut_decl, SgScopeStatement* scope)
{
// FIXME - needs an implicit-spec-list
   bool isImplicitNone = true;

   SgImplicitStatement* implicitStatement = new SgImplicitStatement(isImplicitNone);
   setSourcePositionFrom(implicitStatement, ut_decl);

   ROSE_ASSERT(scope->variantT() == V_SgBasicBlock || scope->variantT() == V_SgClassDefinition);
   scope->append_statement(implicitStatement);

   convertLabel(ut_decl, implicitStatement);

#if DEBUG_UNTYPED_CONVERTER
   cout << "--- finished converting implicit-stmt " << implicitStatement->class_name() << endl;
#endif

   return implicitStatement;
}

SgDeclarationStatement*
UntypedFortranConverter::convertSgUntypedNameListDeclaration (SgUntypedNameListDeclaration* ut_decl, SgScopeStatement* scope)
   {
      SgUntypedNamePtrList ut_names = ut_decl->get_names()->get_name_list();

      switch (ut_decl->get_statement_enum())
        {
        case General_Language_Translation::e_fortran_import_stmt:
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
              scope->append_statement(importStatement);
           }

        case SgToken::FORTRAN_EXTERNAL:
          {
             SgAttributeSpecificationStatement::attribute_spec_enum attr_enum = SgAttributeSpecificationStatement::e_externalStatement;
             SgAttributeSpecificationStatement* attr_spec_stmt = SageBuilder::buildAttributeSpecificationStatement(attr_enum);
             ROSE_ASSERT(attr_spec_stmt);
             setSourcePositionFrom(attr_spec_stmt, ut_decl);

             BOOST_FOREACH(SgUntypedName* ut_name, ut_names)
             {
                std::string name = ut_name->get_name();
                std::cout << "... EXTERNAL name is " << name << std::endl;

             // TODO - what about symbol table, perhaps already typed ................................................................
                SgType* return_type = SageBuilder::buildFortranImplicitType(name);

                SgFunctionParameterTypeList * func_params = SageBuilder::buildFunctionParameterTypeList();
                SgFunctionType*               func_type   = SageBuilder::buildFunctionType(return_type, func_params);
                SgFunctionRefExp *            func_ref    = SageBuilder::buildFunctionRefExp(name, func_type, scope);

                attr_spec_stmt->get_parameter_list()->prepend_expression(func_ref);

                std::cout << "...      func_ref is " << func_ref << std::endl;
                std::cout << "...     func_type is " << func_type << " " << func_type->class_name() << std::endl;
                std::cout << "...   func_params is " << func_params << endl;

             }
             SageInterface::appendStatement(attr_spec_stmt, scope);
             UntypedFortranConverter::convertLabel(ut_decl, attr_spec_stmt);

             std::cout << "...attr_spec_stmt is " << attr_spec_stmt << std::endl;
             std::cout << "...    param_list is " << attr_spec_stmt->get_parameter_list() << std::endl;

             return attr_spec_stmt;
         }

     // TODO - create and use General_Language_Translation::StatementKind for DIMENSION
     // TODO - probably will require an SgUntypedExprListExpression
        case SgToken::FORTRAN_DIMENSION:
          {
             cerr << "UntypedFortranConverter::convertSgUntypedNameListDeclaration: DIMENSION statement unimplemented" << endl;
             ROSE_ASSERT(0);
             break;
          }

     // TODO - this should be modified to work like DIMENSION
        case General_Language_Translation::e_cuda_device:
        case General_Language_Translation::e_cuda_managed:
        case General_Language_Translation::e_cuda_constant:
        case General_Language_Translation::e_cuda_shared:
        case General_Language_Translation::e_cuda_pinned:
        case General_Language_Translation::e_cuda_texture:
          {
             cerr << "UntypedFortranConverter::convertSgUntypedNameListDeclaration: CUDA attributes statement unimplemented" << endl;
             ROSE_ASSERT(0);
             break;
          }

       default:
          {
             cerr << "UntypedFortranConverter::convertSgUntypedNameListDeclaration: failed to find known statement enum, is "
                  << ut_decl->get_statement_enum() << endl;
             ROSE_ASSERT(0);
          }
       }
   }


// Executable statements
//----------------------

SgStatement*
UntypedFortranConverter::convertSgUntypedExpressionStatement (SgUntypedExpressionStatement* ut_stmt, SgNodePtrList& children, SgScopeStatement* scope)
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
             sg_stmt = SageBuilder::buildReturnStmt(sg_expr);
             break;
          }
        default:
          {
             fprintf(stderr, "UntypedFortranConverter::convertSgUntypedExpressionStatement: failed to find known statement enum, is %d\n", ut_stmt->get_statement_enum());
             ROSE_ASSERT(0);
          }
      }
      
      ROSE_ASSERT(sg_stmt != NULL);
      setSourcePositionFrom(sg_stmt, ut_stmt);

   // any IR node can have a parent, it makes sense to associate the expression with the statement
      sg_expr->set_parent(sg_stmt);

      scope->append_statement(sg_stmt);

      UntypedFortranConverter::convertLabel(ut_stmt, sg_stmt);

      return sg_stmt;
   }

SgStatement*
UntypedFortranConverter::convertUntypedForStatement (SgUntypedForStatement* ut_stmt, SgNodePtrList& children, SgScopeStatement* scope)
   {
      ROSE_ASSERT(children.size() == 4);

#if 0
      cout << "-x- convert do: initialization is " << ut_stmt->get_initialization() << " " << ut_stmt->get_initialization()->class_name() << endl;
      cout << "-x- convert do:          bound is " << ut_stmt->get_bound() << " " << ut_stmt->get_bound()->class_name() << endl;
      cout << "-x- convert do:      increment is " << ut_stmt->get_increment() << " " << ut_stmt->get_increment()->class_name() << endl;
      cout << "-x- convert do:           body is " << ut_stmt->get_body() << " " << endl;
      cout << "-x- convert do: construct name is " << ut_stmt->get_do_construct_name() << endl;
#endif

      SgAssignOp* initialization = isSgAssignOp(children[0]);
      ROSE_ASSERT(initialization != NULL);

      SgExpression* upper_bound = isSgExpression(children[1]);
      ROSE_ASSERT(upper_bound != NULL);

      SgExpression* increment = isSgExpression(children[2]);
      ROSE_ASSERT(increment != NULL);

   // Allowed to be NULL in SageBuilder
      SgBasicBlock* loop_body = isSgBasicBlock(children[3]);

      SgFortranDo* sg_stmt = SageBuilder::buildFortranDo(initialization, upper_bound, increment, loop_body);
      ROSE_ASSERT(sg_stmt != NULL);
      setSourcePositionFrom(sg_stmt, ut_stmt);

  // For now assume this is a do-construct
     sg_stmt->set_has_end_statement(true);

   // Determine if do-construct-name is present
      if (ut_stmt->get_do_construct_name().empty() == false)
         {
            sg_stmt->set_string_label(ut_stmt->get_do_construct_name());
         }

      SageInterface::appendStatement(sg_stmt, scope);

// TEMPORARY (fix numeric labels especially here)
//    convertLabel(ut_stmt, sg_stmt, scope);

      return sg_stmt;
   }

SgStatement*
UntypedFortranConverter::convertSgUntypedOtherStatement (SgUntypedOtherStatement* ut_stmt, SgScopeStatement* scope)
   {
      switch (ut_stmt->get_statement_enum())
        {
        case SgToken::FORTRAN_CONTINUE:
          {
             SgLabelStatement* labelStatement = new SgLabelStatement(ut_stmt->get_label_string(), NULL);
             setSourcePositionFrom(labelStatement, ut_stmt);

             SgFunctionDefinition* currentFunctionScope = TransformationSupport::getFunctionDefinition(scope);
             ROSE_ASSERT(currentFunctionScope != NULL);
             labelStatement->set_scope(currentFunctionScope);
             ROSE_ASSERT(labelStatement->get_scope() != NULL);

          // SageInterface should be used but probably can't until SgFortranContinueStmt is used
          // SageInterface::appendStatement(labelStatement, scope);
             scope->append_statement(labelStatement);

          // TODO - why does this only work here?????? (this may be an old comment; check if need to pass scope immediately below)
          // UntypedFortranConverter::convertLabel(ut_stmt, labelStatement, currentFunctionScope);
             UntypedFortranConverter::convertLabel(ut_stmt, labelStatement);

             return labelStatement;
         }
        case SgToken::FORTRAN_CONTAINS:
           {
           // Contains statement is just syntax so don't convert
              return NULL;
           }
       default:
          {
             cerr << "UntypedFortranConverter::convertSgUntypedOtherStatement: failed to find known statement enum, is "
                  << ut_stmt->get_statement_enum() << endl;
             ROSE_ASSERT(0);
          }
       }
   }

SgImageControlStatement*
UntypedFortranConverter::convertSgUntypedImageControlStatement (SgUntypedImageControlStatement* ut_stmt, SgScopeStatement* scope)
   {
      using namespace General_Language_Translation;

      ROSE_ASSERT(ut_stmt);

      SgImageControlStatement* sg_stmt = NULL;

      SgUntypedExprListExpression* ut_status_list = ut_stmt->get_status_list();
      ROSE_ASSERT(ut_status_list);

      switch (ut_stmt->get_statement_enum())
        {
        case e_fortran_sync_all_stmt:
          {
             sg_stmt = new SgSyncAllStatement();
             break;
          }
        case e_fortran_sync_images_stmt:
          {
             ROSE_ASSERT(ut_stmt->get_expression());

             SgExpression* image_set = convertSgUntypedExpression(ut_stmt->get_expression());
             ROSE_ASSERT(image_set);

             sg_stmt = new SgSyncImagesStatement(image_set);
             break;
          }
        case e_fortran_sync_memory_stmt:
          {
             sg_stmt = new SgSyncMemoryStatement();
             break;
          }
        case e_fortran_sync_team_stmt:
          {
             ROSE_ASSERT(ut_stmt->get_expression());

             SgExpression* team_value = convertSgUntypedExpression(ut_stmt->get_expression());
             ROSE_ASSERT(team_value);

             sg_stmt = new SgSyncTeamStatement(team_value);
             break;
          }
        case e_fortran_lock_stmt:
          {
             ROSE_ASSERT(ut_stmt->get_variable());

             SgExpression* lock_variable = convertSgUntypedExpression(ut_stmt->get_variable());
             ROSE_ASSERT(lock_variable);

             sg_stmt = new SgLockStatement(lock_variable);
             break;
          }
        case e_fortran_unlock_stmt:
          {
             ROSE_ASSERT(ut_stmt->get_variable());

             SgExpression* lock_variable = convertSgUntypedExpression(ut_stmt->get_variable());
             ROSE_ASSERT(lock_variable);

             sg_stmt = new SgUnlockStatement(lock_variable);
             break;
          }
        default:
          {
             cerr << "UntypedFortranConverter::convertSgUntypedImageControlStatement: failed to find known statement enum, is "
                  << ut_stmt->get_statement_enum() << endl;
             ROSE_ASSERT(0);
          }
        }

      ROSE_ASSERT(sg_stmt);

      cout << "-dn- ICS: sg_stmt = " << sg_stmt << ": " << sg_stmt->class_name()  << endl;

      switch (ut_stmt->get_statement_enum())
        {
        case e_fortran_sync_all_stmt:
        case e_fortran_sync_images_stmt:
        case e_fortran_sync_memory_stmt:
        case e_fortran_sync_team_stmt:
        case e_fortran_lock_stmt:
        case e_fortran_unlock_stmt:
          {
             BOOST_FOREACH(SgUntypedExpression* ut_expr, ut_status_list->get_expressions())
                {
                   SgUntypedExprListExpression* ut_status_container = isSgUntypedExprListExpression(ut_expr);
                   ROSE_ASSERT(ut_status_container);

                   SgExpression* sg_expr = convertSgUntypedExpression(ut_status_container->get_expressions().front());
                   ROSE_ASSERT(sg_expr);

                   switch (ut_status_container->get_expression_enum())
                     {
                     case e_fortran_sync_stat_stat:    sg_stmt->set_stat    (sg_expr);  break;
                     case e_fortran_sync_stat_errmsg:  sg_stmt->set_err_msg (sg_expr);  break;
                     case e_fortran_stat_acquired_lock:
                        {
                           SgLockStatement* lock_stmt = isSgLockStatement(sg_stmt);
                           ROSE_ASSERT(lock_stmt);
                           lock_stmt->set_acquired_lock(sg_expr);
                           break;
                        }
                     default: ROSE_ASSERT(0);
                     }

                }
             break;
          }
       default:
          {
             cerr << "UntypedFortranConverter::convertSgUntypedImageControlStatement: failed to find known statement enum, is "
                  << ut_stmt->get_statement_enum() << endl;
             ROSE_ASSERT(0);
          }
       }

      ROSE_ASSERT(sg_stmt);
      setSourcePositionFrom(sg_stmt, ut_stmt);

      SageInterface::appendStatement(sg_stmt, scope);

      return sg_stmt;
   }

SgImageControlStatement*
UntypedFortranConverter::convertSgUntypedImageControlStatement (SgUntypedImageControlStatement* ut_stmt,
                                                                SgNodePtrList& children, SgScopeStatement* scope)
   {
      SgImageControlStatement* sg_stmt = NULL;

      cout << "-up- UntypedFortranConverter::convertSgUntypedImageControlStatement: statement enum, is "
           << ut_stmt->get_statement_enum() << " # children is " << children.size() << endl;

      switch (ut_stmt->get_statement_enum())
        {
        case General_Language_Translation::e_fortran_sync_all_stmt:
        case General_Language_Translation::e_fortran_sync_images_stmt:
        case General_Language_Translation::e_fortran_sync_memory_stmt:
        case General_Language_Translation::e_fortran_sync_team_stmt:
        case General_Language_Translation::e_fortran_lock_stmt:
        case General_Language_Translation::e_fortran_unlock_stmt:
          {
             cout << "-up- UntypedFortranConverter::convertSgUntypedImageControlStatement: statement enum, is "
                  << ut_stmt->get_statement_enum() << " e_fortran_sync_all..team_stmt"<< endl;
             SgStatement* sg_node = scope->getStatementList().back();
             cout << "-up- sg_node = " << sg_node << endl;
             SgImageControlStatement* sg_stmt = dynamic_cast<SgImageControlStatement*>(sg_node);
             ROSE_ASSERT(sg_stmt);
             break;
          }
       default:
          {
             cerr << "UntypedFortranConverter::convertSgUntypedImageControlStatement: failed to find known statement enum, is "
                  << ut_stmt->get_statement_enum() << endl;
             ROSE_ASSERT(0);
          }
       }

   // The source position has already been set for the node in the down traversal (see previous function).
   // No need to append sg_stmt either.

      return sg_stmt;
   }
