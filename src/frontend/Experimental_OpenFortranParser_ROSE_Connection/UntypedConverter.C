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
UntypedConverter::convertLabel(SgUntypedStatement* ut_stmt, SgStatement* sg_stmt,
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

// R403 declaration-type-spec
//
SgType*
UntypedConverter::convertSgUntypedType(SgUntypedType* ut_type, SgScopeStatement* scope)
{
// Temporary assertions as this conversion is completed
   ROSE_ASSERT(ut_type->get_is_intrinsic() == true);
   ROSE_ASSERT(ut_type->get_is_literal() == false);
   ROSE_ASSERT(ut_type->get_is_class() == false);
   ROSE_ASSERT(ut_type->get_is_constant() == false);
   ROSE_ASSERT(ut_type->get_is_user_defined() == false);
   ROSE_ASSERT(ut_type->get_has_kind() == false);

   ROSE_ASSERT(ut_type->get_type_enum_id() == SgUntypedType::e_int);
   SgType* sg_type = SageBuilder::buildIntType();

   printf("--- finished converting type %s\n", ut_type->get_type_name().c_str());

   return sg_type;
}

SgInitializedName*
UntypedConverter::convertSgUntypedInitializedName(SgUntypedInitializedName* ut_name, SgType* sg_type, SgInitializer* sg_init)
{
   SgInitializedName* sg_name = SageBuilder::buildInitializedName(ut_name->get_name(), sg_type, sg_init);

   printf("--- finished converting initialized name %s\n", ut_name->get_name().c_str());

   return sg_name;
}


// R501 type-declaration-stmt
//
SgVariableDeclaration*
UntypedConverter::convertSgUntypedVariableDeclaration(SgUntypedVariableDeclaration* ut_decl, SgScopeStatement* scope)
{
   ROSE_ASSERT(scope->variantT() == V_SgBasicBlock || scope->variantT() == V_SgClassDefinition);

   SgUntypedType* ut_type = ut_decl->get_type();
   SgUntypedInitializedNameList* ut_params = ut_decl->get_parameters();

   SgType* sg_type = convertSgUntypedType(ut_type, scope);

   SgInitializer* varInit = NULL;

   SgUntypedInitializedNamePtrList vars = ut_params->get_name_list();

   SgVariableDeclaration* sg_decl = SageBuilder::buildVariableDeclaration(vars[0]->get_name(), sg_type, varInit, scope);

   SgInitializedName* var = SageBuilder::buildInitializedName(vars[1]->get_name(), sg_type, varInit);

   sg_decl->append_variable(var, NULL);


   // TODO - figure out why this has source position
   // setSourcePositionUnknown(sg_decl);

   //NEEDED?   sg_decl->set_parent(scope);
   //NEEDED?   sg_decl->set_definingDeclaration(sg_decl);

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
          printf ("In setSourcePosition(SgLocatedNode* locatedNode): locatedNode = %p = %s \n",locatedNode,locatedNode->class_name().c_str());
        }
     ROSE_ASSERT(locatedNode->get_endOfConstruct()   == NULL);
     ROSE_ASSERT(locatedNode->get_startOfConstruct() == NULL);

     SageInterface::setSourcePosition(locatedNode);
}
