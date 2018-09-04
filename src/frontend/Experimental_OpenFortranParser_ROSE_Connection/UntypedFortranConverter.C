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
UntypedFortranConverter::convertSgUntypedFunctionDeclarationList (SgUntypedFunctionDeclarationList* ut_list, SgScopeStatement* scope)
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
UntypedFortranConverter::convertSgUntypedSubroutineDeclaration (SgUntypedSubroutineDeclaration* ut_function, SgScopeStatement* scope)
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
UntypedFortranConverter::convertSgUntypedFunctionDeclaration (SgUntypedFunctionDeclaration* ut_function, SgScopeStatement* scope)
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
UntypedFortranConverter::convertSgUntypedBlockDataDeclaration (SgUntypedBlockDataDeclaration* ut_block_data, SgScopeStatement* scope)
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

   // Specify case insensitivity for Fortran.
      blockDataBody->setCaseInsensitive(true);
      blockDataDefinition->setCaseInsensitive(true);
      blockDataDeclaration->set_scope (currentScopeOfFunctionDeclaration);
      blockDataDeclaration->set_parent(currentScopeOfFunctionDeclaration);

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
   SgType*        sg_base_type = convertSgUntypedType(ut_base_type, scope);

   SgUntypedInitializedNamePtrList ut_vars = ut_decl->get_variables()->get_name_list();
   SgUntypedInitializedNamePtrList::const_iterator it;

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
      SgUntypedNamePtrList::const_iterator it;

      switch (ut_decl->get_statement_enum())
        {
        case General_Language_Translation::e_fortran_import_stmt:
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
             UntypedFortranConverter::convertLabel(ut_decl, attr_spec_stmt);

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
             sg_stmt = new SgReturnStmt(sg_expr);
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
UntypedFortranConverter::convertSgUntypedForStatement (SgUntypedForStatement* ut_stmt, SgNodePtrList& children, SgScopeStatement* scope)
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

             scope->append_statement(labelStatement);

          // TODO - why does this only work here??????
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
             fprintf(stderr, "UntypedFortranConverter::convertSgUntypedOtherStatement: failed to find known statement enum, is %d\n", ut_stmt->get_statement_enum());
             ROSE_ASSERT(0);
          }
       }
   }
