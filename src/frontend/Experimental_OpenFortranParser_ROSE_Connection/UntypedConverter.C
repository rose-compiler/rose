#include "sage3basic.h"
#include "UntypedConverter.h"

using namespace Fortran::Untyped;


//! Set a numerical label for a Fortran statement. The statement should have a enclosing function definition already. SgLabelSymbol and SgLabelR
//efExp are created transparently as needed.
static void setFortranNumericLabel(SgStatement* stmt, int label_value, SgLabelSymbol::label_type_enum label_type, SgScopeStatement* label_scope = NULL)
{
   ROSE_ASSERT (stmt != NULL);
   ROSE_ASSERT (label_value >0 && label_value <=99999); //five digits for Fortran label

   if (label_scope == NULL)
      {
         label_scope = SageInterface::getEnclosingFunctionDefinition(stmt);
      }
   ROSE_ASSERT (label_scope != NULL);

   SgName label_name(rose::StringUtility::numberToString(label_value));
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
         std::cerr << "Error. SageInterface::setFortranNumericLabel() tries to set a duplicated label value!" << std::endl;
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
UntypedConverter::convertLabel (SgUntypedStatement* ut_stmt, SgStatement* sg_stmt,
                                SgLabelSymbol::label_type_enum label_type, SgScopeStatement* label_scope)
{
   std::string label_name = ut_stmt->get_label_string();
   if (!label_name.empty())
      {
      // SageInterface::setFortranNumericLabel(sg_stmt, std::stoul(label_name));
      // The modifications in setFortranNumericLabel should be moved to SageInterface
         setFortranNumericLabel(sg_stmt, std::stoul(label_name), label_type, label_scope);
      }
}


SgType*
UntypedConverter::convertSgUntypedType (SgUntypedType* ut_type, SgScopeStatement* scope)
{
   SgType* sg_type = NULL;

// Temporary assertions as this conversion is completed
   ROSE_ASSERT(ut_type->get_is_intrinsic() == true);
   ROSE_ASSERT(ut_type->get_is_literal() == false);
   ROSE_ASSERT(ut_type->get_is_class() == false);
   ROSE_ASSERT(ut_type->get_is_constant() == false);
   ROSE_ASSERT(ut_type->get_is_user_defined() == false);
   ROSE_ASSERT(ut_type->get_has_kind() == false);

   switch(ut_type->get_type_enum_id())
      {
        case SgUntypedType::e_unknown:
           {
              if (ut_type->get_type_name() == "bool")
                 {
                    sg_type = SageBuilder::buildBoolType();
                    fprintf(stderr, "UntypedConverter::convertSgUntypedType: bool type %p \n", sg_type);
                 }
              else
                 {
                    fprintf(stderr, "UntypedConverter::convertSgUntypedType: failed to find known type \n");
                    ROSE_ASSERT(0);
                 }
              break;
           }
        case SgUntypedType::e_void:
           {
              sg_type = SageBuilder::buildVoidType();
              break;
           }
        case SgUntypedType::e_int:
           {
              sg_type = SageBuilder::buildIntType();
              break;
           }
        case SgUntypedType::e_float:
           {
              sg_type = SageBuilder::buildFloatType();
              break;
           }
        case SgUntypedType::e_double:
           {
              sg_type = SageBuilder::buildDoubleType();
              break;
           }
        case SgUntypedType::e_complex:
           {
              sg_type = SageBuilder::buildComplexType();
              break;
           }
        case SgUntypedType::e_char:
           {
           // sg_type = SageBuilder::buildCharType(SgExpression* stringLengthExpression);
           // TODO - need stringLengthExpression, the following will fail on an assertion
              sg_type = SageBuilder::buildCharType();
              break;
           }
        default:
           {
              fprintf(stderr, "UntypedConverter::convertSgUntypedType: failed to find known type \n");
              ROSE_ASSERT(0);
           }
      }

   printf("--- finished converting type %s\n", ut_type->get_type_name().c_str());

   ROSE_ASSERT(sg_type != NULL);

   return sg_type;
}


SgInitializedName*
UntypedConverter::convertSgUntypedInitializedName (SgUntypedInitializedName* ut_name, SgType* sg_type, SgInitializer* sg_init)
{
   SgInitializedName* sg_name = SageBuilder::buildInitializedName(ut_name->get_name(), sg_type, sg_init);
   setSourcePositionUnknown(sg_name);

   printf("--- finished converting initialized name %s\n", ut_name->get_name().c_str());

   return sg_name;
}


SgGlobal*
UntypedConverter::convertSgUntypedGlobalScope (SgUntypedGlobalScope* ut_scope, SgScopeStatement* scope)
{
// The global scope should have neither declarations nor executables
   ROSE_ASSERT(ut_scope->get_declaration_list()-> get_traversalSuccessorContainer().size() == 0);
   ROSE_ASSERT(ut_scope->get_statement_list()  -> get_traversalSuccessorContainer().size() == 0);

   SgGlobal* sg_scope = isSgGlobal(scope);
   ROSE_ASSERT(sg_scope == SageBuilder::getGlobalScopeFromScopeStack());

   return sg_scope;
}


void
UntypedConverter::convertSgUntypedFunctionDeclarationList (SgUntypedFunctionDeclarationList* ut_list, SgScopeStatement* scope)
{
   if (scope->variantT() == V_SgBasicBlock || scope->variantT() == V_SgClassDefinition)
      {
         if ( ! ut_list->get_func_list().empty() )
            {
               // Need to add a contains statement to the current scope as it currently
               // doesn't exist in OFP's Fortran AST (FAST) design (part of concrete syntax only)
               SgContainsStatement* containsStatement = new SgContainsStatement();
               UntypedConverter::setSourcePositionUnknown(containsStatement);
               containsStatement->set_definingDeclaration(containsStatement);

               scope->append_statement(containsStatement);
               ROSE_ASSERT(containsStatement->get_parent() != NULL);
            }
      }
}


SgProgramHeaderStatement*
UntypedConverter::convertSgUntypedProgramHeaderDeclaration (SgUntypedProgramHeaderDeclaration* ut_program, SgScopeStatement* scope)
{
   ROSE_ASSERT(scope->variantT() == V_SgGlobal);

   SgUntypedNamedStatement* ut_program_end_statement = ut_program->get_end_statement();
   ROSE_ASSERT(ut_program_end_statement != NULL);

   SgName programName = ut_program->get_name();

// We should test if this is in the function type table, but do this later?
   SgFunctionType* type = new SgFunctionType(SgTypeVoid::createType(), false);

   SgProgramHeaderStatement* programDeclaration = new SgProgramHeaderStatement(programName, type, NULL);

// A Fortran program has no non-defining declaration
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

//FIXME if (SgProject::get_verbose() > DEBUG_COMMENT_LEVEL)
   {
      printf("Inserted SgFunctionSymbol in globalScope using name = %s \n", programName.str());
   }

   SgBasicBlock* programBody = new SgBasicBlock();
   SgFunctionDefinition* programDefinition = new SgFunctionDefinition(programDeclaration, programBody);

   programBody->setCaseInsensitive(true);
   programDefinition->setCaseInsensitive(true);

   SageBuilder::pushScopeStack(programDefinition);
   SageBuilder::pushScopeStack(programBody);

   programBody->set_parent(programDefinition);
   programDefinition->set_parent(programDeclaration);

   UntypedConverter::setSourcePositionUnknown(programDeclaration->get_parameterList());
   UntypedConverter::setSourcePositionUnknown(programDeclaration);

// Convert the labels for the program begin and end statements
   UntypedConverter::convertLabel(ut_program,               programDeclaration, SgLabelSymbol::e_start_label_type, /*label_scope=*/ programDefinition);
   UntypedConverter::convertLabel(ut_program_end_statement, programDeclaration, SgLabelSymbol::e_end_label_type,   /*label_scope=*/ programDefinition);

// Set the end statement name if it exists
   if (ut_program_end_statement->get_statement_name().empty() != true)
      {
         programDeclaration->set_named_in_end_statement(true);
      }

#if THIS_PART_NEEDS_TO_IMPLEMENT_NO_PROGRAM_STATEMENT
        ROSE_ASSERT(programDeclaration->get_parameterList() != NULL);

        if (programKeyword != NULL)
        {
            UntypedConverter::setSourcePosition(programDeclaration->get_parameterList(), programKeyword);
            UntypedConverter::setSourcePosition(programDeclaration, programKeyword);
        }
        else
           {
           // These will be marked as isSourcePositionUnavailableInFrontend = true and isOutputInCodeGeneration = true

           // DQ (12/18/2008): These need to make marked with a valid file id (not NULL_FILE, internally),
           // so that any attached comments and CPP directives will be properly attached.
              UntypedConverter::setSourcePosition(programDeclaration->get_parameterList(), tokenList);
              UntypedConverter::setSourcePosition(programDeclaration, tokenList);
        }

        // Unclear if we should use the same token list for resetting the source position in all three IR nodes.
        UntypedConverter::setSourcePosition(programDefinition, tokenList);
        UntypedConverter::setSourcePosition(programBody, tokenList);
#endif

   UntypedConverter::setSourcePositionUnknown(programDefinition);
   UntypedConverter::setSourcePositionUnknown(programBody);

   ROSE_ASSERT(programDeclaration->get_firstNondefiningDeclaration() != programDeclaration);
   ROSE_ASSERT(programDeclaration->get_firstNondefiningDeclaration() == NULL);

#if 0
   if (programDeclaration->get_program_statement_explicit() == false)
      {
         // The function declaration should be forced to match the "end" keyword.
         // Reset the declaration to the current filename.
         //FIXME-no this      programDeclaration->get_startOfConstruct()->set_filenameString(p_source_file->getFileName());
         //FIXME              programDeclaration->get_endOfConstruct()->set_filenameString(p_source_file->getFileName());
      }
#endif

   printf("--- finished building program %s\n", programDeclaration->get_name().str());

   ROSE_ASSERT(programBody == SageBuilder::topScopeStack());

   return programDeclaration;
}


SgVariableDeclaration*
UntypedConverter::convertSgUntypedVariableDeclaration (SgUntypedVariableDeclaration* ut_decl, SgScopeStatement* scope)
{
   ROSE_ASSERT(scope->variantT() == V_SgBasicBlock || scope->variantT() == V_SgClassDefinition);

   SgUntypedType* ut_type = ut_decl->get_type();
   SgType*        sg_type = convertSgUntypedType(ut_type, scope);

   SgUntypedInitializedNamePtrList ut_vars = ut_decl->get_parameters()->get_name_list();
   SgUntypedInitializedNamePtrList::const_iterator i = ut_vars.begin();

// Declare the first variable
   SgVariableDeclaration* sg_decl = SageBuilder::buildVariableDeclaration((*i)->get_name(), sg_type, /*sg_init*/NULL, scope);

// And now the rest of the variables
   for (i = ut_vars.begin() + 1; i != ut_vars.end(); i++)
      {
      SgInitializedName* initializedName = UntypedConverter::convertSgUntypedInitializedName((*i), sg_type, /*sg_init*/NULL);
      SgName variableName = initializedName->get_name();

      initializedName->set_declptr(sg_decl);
      sg_decl->append_variable(initializedName, /*sg_init*/NULL);

      SgVariableSymbol* variableSymbol = NULL;
      SgFunctionDefinition * functionDefinition = SageInterface::getEnclosingProcedure(scope);
      if (functionDefinition != NULL)
         {
         // Check in the function definition for an existing symbol
            variableSymbol = functionDefinition->lookup_variable_symbol(variableName);
            if (variableSymbol != NULL) // found a function parameter with the same name
               {
                  // look at code in sageBuilder.C and fortran_support.C
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

   // setSourcePositionUnknown(sg_decl);

// Need to set attributes here
// TODO - add attr-spec-list to RTG so that it is the SgUntypedVariableDeclaration constructor
   sg_decl->get_declarationModifier().get_accessModifier().setUndefined();

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

   printf("--- finished converting type-declaration-stmt %s\n", sg_decl->class_name().c_str());

   return sg_decl;
}


// R560 implicit-stmt
//
SgImplicitStatement*
UntypedConverter::convertSgUntypedImplicitDeclaration(SgUntypedImplicitDeclaration* ut_decl, SgScopeStatement* scope)
{
// FIXME - needs an implicit-spec-list
   bool isImplicitNone = true;

   SgImplicitStatement* implicitStatement = new SgImplicitStatement(isImplicitNone);
   setSourcePositionUnknown(implicitStatement);

   ROSE_ASSERT(scope->variantT() == V_SgBasicBlock);
   scope->append_statement(implicitStatement);

   convertLabel(ut_decl, implicitStatement);

   printf("--- finished converting implicit-stmt %s\n", implicitStatement->class_name().c_str());

   return implicitStatement;
}


SgScopeStatement*
UntypedConverter::initialize_global_scope(SgSourceFile* file)
{
 // First we have to get the global scope initialized (and pushed onto the stack).

 // Set the default for source position generation to be consistent with other languages (e.g. C/C++).
    SageBuilder::setSourcePositionClassificationMode(SageBuilder::e_sourcePositionFrontendConstruction);

    SgGlobal* globalScope = file->get_globalScope();
    ROSE_ASSERT(globalScope != NULL);
    ROSE_ASSERT(globalScope->get_parent() != NULL);

 // Fortran is case insensitive
    globalScope->setCaseInsensitive(true);

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

 // CER - DELETE_ME
    printf ("                    sg_global scope       %p\n", globalScope);

#if WHERE_IS_DEBUG_COMMENT_LEVEL
    if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
       {
          SageBuilder::topScopeStack()->get_startOfConstruct()->display("In initialize_global_scope(): start");
          SageBuilder::topScopeStack()->get_endOfConstruct  ()->display("In initialize_global_scope(): end");
       }
#endif

    return globalScope;
}


void
UntypedConverter::setSourcePositionUnknown(SgLocatedNode* locatedNode)
{
  // This function sets the source position to be marked as not available (since we don't have token information)
  // These nodes WILL be unparsed in the code generation phase.

  // The SgLocatedNode has both a startOfConstruct and endOfConstruct source position.
     ROSE_ASSERT(locatedNode != NULL);

  // Make sure we never try to reset the source position of the global scope (set elsewhere in ROSE).
     ROSE_ASSERT(isSgGlobal(locatedNode) == NULL);

  // Check the endOfConstruct first since it is most likely NULL (helpful in debugging)
     if (locatedNode->get_endOfConstruct() != NULL || locatedNode->get_startOfConstruct() != NULL)
        {
          printf ("In setSourcePositionUnknown: source position known locatedNode = %p = %s \n",locatedNode,locatedNode->class_name().c_str());
        }
     else
        {
           ROSE_ASSERT(locatedNode->get_endOfConstruct()   == NULL);
           ROSE_ASSERT(locatedNode->get_startOfConstruct() == NULL);
           SageInterface::setSourcePosition(locatedNode);
        }
}
