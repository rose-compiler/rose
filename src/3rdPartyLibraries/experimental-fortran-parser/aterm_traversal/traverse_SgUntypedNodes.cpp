//Rasmussen-delete-me #define ROSE_USE_ROSE_ATERM_SUPPORT 1
//Rasmussen-delete-me #define DEBUG_PRINT

#define DEBUG_PRINT

#include "rosePublicConfig.h"
#include "sage3basic.h"

#include <aterm2.h>
#include "traverse_SgUntypedNodes.hpp"
//#include "build_SgUntypedNodes.hpp"

//========================================================================================
// SgLocatedNode
//----------------------------------------------------------------------------------------
ATbool traverse_SgLocatedNode(ATerm term, SgLocatedNode** var_SgLocatedNode)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgLocatedNode: %s\n", ATwriteToString(term));
#endif


  *var_SgLocatedNode = NULL;
  //CER-FIXME  if (ATmatch(term, "SgLocatedNode()", )) {
  if (ATmatch(term, "SgLocatedNode()")) {
  } else return ATfalse;

  *var_SgLocatedNode = new SgLocatedNode();

  return ATtrue;
}

//========================================================================================
// SgLocatedNodeSupport
//----------------------------------------------------------------------------------------
ATbool traverse_SgLocatedNodeSupport(ATerm term, SgLocatedNodeSupport** var_SgLocatedNodeSupport)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgLocatedNodeSupport: %s\n", ATwriteToString(term));
#endif


  *var_SgLocatedNodeSupport = NULL;
  //CER-FIXME  if (ATmatch(term, "SgLocatedNodeSupport()", )) {
  if (ATmatch(term, "SgLocatedNodeSupport()")) {
  } else return ATfalse;

  *var_SgLocatedNodeSupport = new SgLocatedNodeSupport();

  return ATtrue;
}

//========================================================================================
// SgUntypedNode
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedNode(ATerm term, SgUntypedNode** var_SgUntypedNode)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedNode: %s\n", ATwriteToString(term));
#endif


  *var_SgUntypedNode = NULL;
  //CER-FIXME  if (ATmatch(term, "SgUntypedNode()", )) {
  if (ATmatch(term, "SgUntypedNode()")) {
  } else return ATfalse;

  *var_SgUntypedNode = new SgUntypedNode();

  return ATtrue;
}

//========================================================================================
// SgUntypedExpression
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedExpression(ATerm term, SgUntypedExpression** var_SgUntypedExpression)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedExpression: %s\n", ATwriteToString(term));
#endif

  ATerm term1;
  //CER-FIXME  SgToken::ROSE_Fortran_Keywords* arg1;
  SgToken::ROSE_Fortran_Keywords arg1;

  *var_SgUntypedExpression = NULL;
  if (traverse_SgUntypedReferenceExpression(term, (SgUntypedReferenceExpression**) var_SgUntypedExpression)) {
    // SgUntypedReferenceExpression
  }
  else if (traverse_SgUntypedValueExpression(term, (SgUntypedValueExpression**) var_SgUntypedExpression)) {
    // SgUntypedValueExpression
  }
  else if (traverse_SgUntypedBinaryOperator(term, (SgUntypedBinaryOperator**) var_SgUntypedExpression)) {
    // SgUntypedBinaryOperator
  }
  else if (ATmatch(term, "SgUntypedExpression(<term>)", &term1)) {
    if (traverse_SgToken_ROSE_Fortran_Keywords(term1, &arg1)) {
      // SgToken_ROSE_Fortran_Keywords
    } else return ATfalse;
    *var_SgUntypedExpression = new SgUntypedExpression(arg1);
  }
  else return ATfalse;

  return ATtrue;
}

//========================================================================================
// SgToken_ROSE_Fortran_Keywords
//----------------------------------------------------------------------------------------
//CER-FIXME ATbool traverse_SgToken_ROSE_Fortran_Keywords(ATerm term, SgToken::ROSE_Fortran_Keywords** var_SgToken_ROSE_Fortran_Keywords)
ATbool traverse_SgToken_ROSE_Fortran_Keywords(ATerm term, SgToken::ROSE_Fortran_Keywords* var_SgToken_ROSE_Fortran_Keywords)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgToken_ROSE_Fortran_Keywords: %s\n", ATwriteToString(term));
#endif

  int arg1;

  //CER-FIXME  *var_SgToken_ROSE_Fortran_Keywords = NULL;
  *var_SgToken_ROSE_Fortran_Keywords = SgToken::FORTRAN_UNKNOWN;
  if (ATmatch(term, "SgToken_ROSE_Fortran_Keywords(<int>)", &arg1)) {
  } else return ATfalse;

  //CER-FIXME  *var_SgToken_ROSE_Fortran_Keywords = new SgToken::ROSE_Fortran_Keywords(arg1);
  *var_SgToken_ROSE_Fortran_Keywords = (SgToken::ROSE_Fortran_Keywords) arg1;

  return ATtrue;
}

//========================================================================================
// SgToken_ROSE_Fortran_Operators
//----------------------------------------------------------------------------------------
//CER-FIXME ATbool traverse_SgToken_ROSE_Fortran_Operators(ATerm term, SgToken::ROSE_Fortran_Operators** var_SgToken_ROSE_Fortran_Operators)
ATbool traverse_SgToken_ROSE_Fortran_Operators(ATerm term, SgToken::ROSE_Fortran_Operators* var_SgToken_ROSE_Fortran_Operators)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgToken_ROSE_Fortran_Operators: %s\n", ATwriteToString(term));
#endif

  int arg1;

  //CER-FIXME *var_SgToken_ROSE_Fortran_Operators = NULL;
  if (ATmatch(term, "SgToken_ROSE_Fortran_Operators(<int>)", &arg1)) {
  } else return ATfalse;

//CER-FIXME   *var_SgToken_ROSE_Fortran_Operators = new SgToken::ROSE_Fortran_Operators(arg1);
  *var_SgToken_ROSE_Fortran_Operators = (SgToken::ROSE_Fortran_Operators) arg1;

  return ATtrue;
}

//========================================================================================
// SgUntypedType_type_enum
//----------------------------------------------------------------------------------------
//CER-FIXME ATbool traverse_SgUntypedType_type_enum(ATerm term, SgUntypedType::type_enum** var_SgUntypedType_type_enum)
ATbool traverse_SgUntypedType_type_enum(ATerm term, SgUntypedType::type_enum* var_SgUntypedType_type_enum)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedType_type_enum: %s\n", ATwriteToString(term));
#endif

  int arg1;

  //CER-FIXME *var_SgUntypedType_type_enum = NULL;
  if (ATmatch(term, "SgUntypedType_type_enum(<int>)", &arg1)) {
  } else return ATfalse;

//CER-FIXME   *var_SgUntypedType_type_enum = new SgUntypedType_type_enum(arg1);
  *var_SgUntypedType_type_enum = (SgUntypedType::type_enum) arg1;

  return ATtrue;
}

//========================================================================================
// bool
//----------------------------------------------------------------------------------------
ATbool traverse_bool(ATerm term, bool** var_bool)
{
#ifdef DEBUG_PRINT
  printf("... traverse_bool: %s\n", ATwriteToString(term));
#endif

  int arg1;

  *var_bool = NULL;
  if (ATmatch(term, "bool(<int>)", &arg1)) {
  } else return ATfalse;

  *var_bool = new bool(arg1);

  return ATtrue;
}

//========================================================================================
// SgUntypedUnaryOperator
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedUnaryOperator(ATerm term, SgUntypedUnaryOperator** var_SgUntypedUnaryOperator)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedUnaryOperator: %s\n", ATwriteToString(term));
#endif

  ATerm term1, term2, term4;
  //CER-FIXME SgToken::ROSE_Fortran_Keywords* arg1;
  //CER-FIXME SgToken::ROSE_Fortran_Operators* arg2;
  SgToken::ROSE_Fortran_Keywords arg1;
  SgToken::ROSE_Fortran_Operators arg2;
  char* arg3;
  SgUntypedExpression* arg4;

  *var_SgUntypedUnaryOperator = NULL;
  if (ATmatch(term, "SgUntypedUnaryOperator(<term>,<term>,<str>,<term>)", &term1,&term2,&arg3,&term4)) {
    if (traverse_SgToken_ROSE_Fortran_Keywords(term1, &arg1)) {
      // SgToken_ROSE_Fortran_Keywords
    } else return ATfalse;
    if (traverse_SgToken_ROSE_Fortran_Operators(term2, &arg2)) {
      // SgToken_ROSE_Fortran_Operators
    } else return ATfalse;
    if (traverse_SgUntypedExpression(term4, &arg4)) {
      // SgUntypedExpression
    } else return ATfalse;
  } else return ATfalse;

  *var_SgUntypedUnaryOperator = new SgUntypedUnaryOperator(arg1,arg2,arg3,arg4);

  return ATtrue;
}

//========================================================================================
// SgUntypedBinaryOperator
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedBinaryOperator(ATerm term, SgUntypedBinaryOperator** var_SgUntypedBinaryOperator)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedBinaryOperator: %s\n", ATwriteToString(term));
#endif

  ATerm term1, term2, term4, term5;
//CER-FIXME   SgToken::ROSE_Fortran_Keywords* arg1;
//CER-FIXME   SgToken::ROSE_Fortran_Operators* arg2;
  SgToken::ROSE_Fortran_Keywords arg1;
  SgToken::ROSE_Fortran_Operators arg2;
  char* arg3;
  SgUntypedExpression* arg4;
  SgUntypedExpression* arg5;

  *var_SgUntypedBinaryOperator = NULL;
  if (ATmatch(term, "SgUntypedBinaryOperator(<term>,<term>,<str>,<term>,<term>)", &term1,&term2,&arg3,&term4,&term5)) {
    if (traverse_SgToken_ROSE_Fortran_Keywords(term1, &arg1)) {
      // SgToken_ROSE_Fortran_Keywords
    } else return ATfalse;
    if (traverse_SgToken_ROSE_Fortran_Operators(term2, &arg2)) {
      // SgToken_ROSE_Fortran_Operators
    } else return ATfalse;
    if (traverse_SgUntypedExpression(term4, &arg4)) {
      // SgUntypedExpression
    } else return ATfalse;
    if (traverse_SgUntypedExpression(term5, &arg5)) {
      // SgUntypedExpression
    } else return ATfalse;
  } else return ATfalse;

  *var_SgUntypedBinaryOperator = new SgUntypedBinaryOperator(arg1,arg2,arg3,arg4,arg5);

  return ATtrue;
}

//========================================================================================
// SgUntypedValueExpression
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedValueExpression(ATerm term, SgUntypedValueExpression** var_SgUntypedValueExpression)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedValueExpression: %s\n", ATwriteToString(term));
#endif

  ATerm term1, term3;
  //CER-FIXME  SgToken::ROSE_Fortran_Keywords* arg1;
  SgToken::ROSE_Fortran_Keywords arg1;
  char* arg2;
  SgUntypedType* arg3;

  *var_SgUntypedValueExpression = NULL;
  if (ATmatch(term, "SgUntypedValueExpression(<term>,<str>,<term>)", &term1,&arg2,&term3)) {
    if (traverse_SgToken_ROSE_Fortran_Keywords(term1, &arg1)) {
      // SgToken_ROSE_Fortran_Keywords
    } else return ATfalse;
    if (traverse_SgUntypedType(term3, &arg3)) {
      // SgUntypedType
    } else return ATfalse;
  } else return ATfalse;

  *var_SgUntypedValueExpression = new SgUntypedValueExpression(arg1,arg2,arg3);

  return ATtrue;
}

//========================================================================================
// SgUntypedArrayReferenceExpression
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedArrayReferenceExpression(ATerm term, SgUntypedArrayReferenceExpression** var_SgUntypedArrayReferenceExpression)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedArrayReferenceExpression: %s\n", ATwriteToString(term));
#endif

  ATerm term1;
  //CER-FIXME  SgToken::ROSE_Fortran_Keywords* arg1;
  SgToken::ROSE_Fortran_Keywords arg1;

  *var_SgUntypedArrayReferenceExpression = NULL;
  if (ATmatch(term, "SgUntypedArrayReferenceExpression(<term>)", &term1)) {
    if (traverse_SgToken_ROSE_Fortran_Keywords(term1, &arg1)) {
      // SgToken_ROSE_Fortran_Keywords
    } else return ATfalse;
  } else return ATfalse;

  *var_SgUntypedArrayReferenceExpression = new SgUntypedArrayReferenceExpression(arg1);

  return ATtrue;
}

//========================================================================================
// SgUntypedOtherExpression
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedOtherExpression(ATerm term, SgUntypedOtherExpression** var_SgUntypedOtherExpression)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedOtherExpression: %s\n", ATwriteToString(term));
#endif

  ATerm term1;
  //CER-FIXME  SgToken::ROSE_Fortran_Keywords* arg1;
  SgToken::ROSE_Fortran_Keywords arg1;

  *var_SgUntypedOtherExpression = NULL;
  if (ATmatch(term, "SgUntypedOtherExpression(<term>)", &term1)) {
    if (traverse_SgToken_ROSE_Fortran_Keywords(term1, &arg1)) {
      // SgToken_ROSE_Fortran_Keywords
    } else return ATfalse;
  } else return ATfalse;

  *var_SgUntypedOtherExpression = new SgUntypedOtherExpression(arg1);

  return ATtrue;
}

//========================================================================================
// SgUntypedFunctionCallOrArrayReferenceExpression
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedFunctionCallOrArrayReferenceExpression(ATerm term, SgUntypedFunctionCallOrArrayReferenceExpression** var_SgUntypedFunctionCallOrArrayReferenceExpression)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedFunctionCallOrArrayReferenceExpression: %s\n", ATwriteToString(term));
#endif

  ATerm term1;
  //CER-FIXME  SgToken::ROSE_Fortran_Keywords* arg1;
  SgToken::ROSE_Fortran_Keywords arg1;

  *var_SgUntypedFunctionCallOrArrayReferenceExpression = NULL;
  if (ATmatch(term, "SgUntypedFunctionCallOrArrayReferenceExpression(<term>)", &term1)) {
    if (traverse_SgToken_ROSE_Fortran_Keywords(term1, &arg1)) {
      // SgToken_ROSE_Fortran_Keywords
    } else return ATfalse;
  } else return ATfalse;

  *var_SgUntypedFunctionCallOrArrayReferenceExpression = new SgUntypedFunctionCallOrArrayReferenceExpression(arg1);

  return ATtrue;
}

//========================================================================================
// SgUntypedReferenceExpression
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedReferenceExpression(ATerm term, SgUntypedReferenceExpression** var_SgUntypedReferenceExpression)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedReferenceExpression: %s\n", ATwriteToString(term));
#endif

  ATerm term1;
  //CER-FIXME  SgToken::ROSE_Fortran_Keywords* arg1;
  SgToken::ROSE_Fortran_Keywords arg1;
  char* arg2;

  *var_SgUntypedReferenceExpression = NULL;
  if (ATmatch(term, "SgUntypedReferenceExpression(<term>,<str>)", &term1,&arg2)) {
    if (traverse_SgToken_ROSE_Fortran_Keywords(term1, &arg1)) {
      // SgToken_ROSE_Fortran_Keywords
    } else return ATfalse;
  } else return ATfalse;

  *var_SgUntypedReferenceExpression = new SgUntypedReferenceExpression(arg1,arg2);

  return ATtrue;
}

//========================================================================================
// SgUntypedImplicitDeclaration
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedImplicitDeclaration(ATerm term, SgUntypedImplicitDeclaration** var_SgUntypedImplicitDeclaration)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedImplicitDeclaration: %s\n", ATwriteToString(term));
#endif

  //CER-FIXME (inserted all arguments)
  char* arg1;
  ATerm term2;
  SgToken::ROSE_Fortran_Keywords arg2;

  *var_SgUntypedImplicitDeclaration = NULL;
  //CER-FIXME  if (ATmatch(term, "SgUntypedImplicitDeclaration()", )) {
  if (ATmatch(term, "SgUntypedImplicitDeclaration(<str>,<term>)", &arg1,&term2)) {
     if (traverse_SgToken_ROSE_Fortran_Keywords(term2, &arg2)) {
        // SgToken_ROSE_Fortran_Keywords
     } else return ATfalse;
  } else return ATfalse;

  *var_SgUntypedImplicitDeclaration = new SgUntypedImplicitDeclaration(arg1,arg2);

  return ATtrue;
}

//========================================================================================
// SgUntypedVariableDeclaration
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedVariableDeclaration(ATerm term, SgUntypedVariableDeclaration** var_SgUntypedVariableDeclaration)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedVariableDeclaration: %s\n", ATwriteToString(term));
#endif

  //CER-FIXME (arguments)
  ATerm term2, term3, term4;
  char* arg1;
  SgToken::ROSE_Fortran_Keywords arg2;
  SgUntypedType* arg3;
  SgUntypedInitializedNameList* arg4;

  *var_SgUntypedVariableDeclaration = NULL;
  if (ATmatch(term, "SgUntypedVariableDeclaration(<str>,<term>,<term>,<term>)", &arg1,&term2,&term3,&term4)) {
    if (traverse_SgToken_ROSE_Fortran_Keywords(term2, &arg2)) {
      // SgToken_ROSE_Fortran_Keywords
    } else return ATfalse;
    if (traverse_SgUntypedType(term3, &arg3)) {
      // SgUntypedType
    } else return ATfalse;
    if (traverse_SgUntypedInitializedNameList(term4, &arg4)) {
       // SgUntypedInitializedNameList
    } else return ATfalse;
  } else return ATfalse;

  *var_SgUntypedVariableDeclaration = new SgUntypedVariableDeclaration(arg1,arg2,arg3,arg4);

  return ATtrue;
}

//========================================================================================
// SgUntypedFunctionDeclaration
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedFunctionDeclaration(ATerm term, SgUntypedFunctionDeclaration** var_SgUntypedFunctionDeclaration)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedFunctionDeclaration: %s\n", ATwriteToString(term));
#endif

  char* arg1;

  *var_SgUntypedFunctionDeclaration = NULL;
  if (ATmatch(term, "SgUntypedFunctionDeclaration(<str>)", &arg1)) {
  } else return ATfalse;

  *var_SgUntypedFunctionDeclaration = new SgUntypedFunctionDeclaration(arg1);

  return ATtrue;
}

//========================================================================================
// SgUntypedModuleDeclaration
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedModuleDeclaration(ATerm term, SgUntypedModuleDeclaration** var_SgUntypedModuleDeclaration)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedModuleDeclaration: %s\n", ATwriteToString(term));
#endif


  *var_SgUntypedModuleDeclaration = NULL;
  //CER-FIXME  if (ATmatch(term, "SgUntypedModuleDeclaration()", )) {
  if (ATmatch(term, "SgUntypedModuleDeclaration()")) {
  } else return ATfalse;

  *var_SgUntypedModuleDeclaration = new SgUntypedModuleDeclaration();

  return ATtrue;
}

//========================================================================================
// SgUntypedProgramHeaderDeclaration
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedProgramHeaderDeclaration(ATerm term, SgUntypedProgramHeaderDeclaration** var_SgUntypedProgramHeaderDeclaration)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedProgramHeaderDeclaration: %s\n", ATwriteToString(term));
#endif

  ATerm term2, term4, term5, term6, term7;
  char* arg1;
  SgToken::ROSE_Fortran_Keywords arg2;
  char* arg3;
  SgUntypedInitializedNameList* arg4;
  SgUntypedType* arg5;
  SgUntypedFunctionScope* arg6;
  SgUntypedNamedStatement* arg7;

  *var_SgUntypedProgramHeaderDeclaration = NULL;
  if (ATmatch(term, "SgUntypedProgramHeaderDeclaration(<str>,<term>,<str>,<term>,<term>,<term>,<term>)", &arg1,&term2,&arg3,&term4,&term5,&term6,&term7)) {
    if (traverse_SgToken_ROSE_Fortran_Keywords(term2, &arg2)) {
      // SgToken_ROSE_Fortran_Keywords
    } else return ATfalse;
    if (traverse_SgUntypedInitializedNameList(term4, &arg4)) {
      // SgUntypedInitializedNameList
    } else return ATfalse;
    if (traverse_SgUntypedType(term5, &arg5)) {
      // SgUntypedType
    } else return ATfalse;
    if (traverse_SgUntypedFunctionScope(term6, &arg6)) {
      // SgUntypedFunctionScope
    } else return ATfalse;
    if (traverse_SgUntypedNamedStatement(term7, &arg7)) {
      // SgUntypedNamedStatement
    } else return ATfalse;
  } else return ATfalse;

  *var_SgUntypedProgramHeaderDeclaration = new SgUntypedProgramHeaderDeclaration(arg1,arg2,arg3,arg4,arg5,arg6,arg7);
  return ATtrue;
}

//========================================================================================
// SgUntypedSubroutineDeclaration
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedSubroutineDeclaration(ATerm term, SgUntypedSubroutineDeclaration** var_SgUntypedSubroutineDeclaration)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedSubroutineDeclaration: %s\n", ATwriteToString(term));
#endif

  ATerm term2, term4, term5, term6, term7;
  char* arg1;
  SgToken::ROSE_Fortran_Keywords arg2;
  char* arg3;
  SgUntypedInitializedNameList* arg4;
  SgUntypedType* arg5;
  SgUntypedFunctionScope* arg6;
  SgUntypedNamedStatement* arg7;

  *var_SgUntypedSubroutineDeclaration = NULL;
  if (ATmatch(term, "SgUntypedSubroutineDeclaration(<str>,<term>,<str>,<term>,<term>,<term>,<term>)", &arg1,&term2,&arg3,&term4,&term5,&term6,&term7)) {
    if (traverse_SgToken_ROSE_Fortran_Keywords(term2, &arg2)) {
      // SgToken_ROSE_Fortran_Keywords
    } else return ATfalse;
    if (traverse_SgUntypedInitializedNameList(term4, &arg4)) {
      // SgUntypedInitializedNameList
    } else return ATfalse;
    if (traverse_SgUntypedType(term5, &arg5)) {
      // SgUntypedType
    } else return ATfalse;
    if (traverse_SgUntypedFunctionScope(term6, &arg6)) {
      // SgUntypedFunctionScope
    } else return ATfalse;
    if (traverse_SgUntypedNamedStatement(term7, &arg7)) {
      // SgUntypedNamedStatement
    } else return ATfalse;
  } else return ATfalse;

  *var_SgUntypedSubroutineDeclaration = new SgUntypedSubroutineDeclaration(arg1,arg2,arg3,arg4,arg5,arg6,arg7);

  return ATtrue;
}

//========================================================================================
// SgUntypedStatement
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedStatement(ATerm term, SgUntypedStatement** var_SgUntypedStatement)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedStatement: %s\n", ATwriteToString(term));
#endif

  ATerm term2;
  char* arg1;
  //CER-FIXME  SgToken::ROSE_Fortran_Keywords* arg2;
  SgToken::ROSE_Fortran_Keywords arg2;

  *var_SgUntypedStatement = NULL;
  if (ATmatch(term, "SgUntypedStatement(<str>,<term>)", &arg1,&term2)) {
    if (traverse_SgToken_ROSE_Fortran_Keywords(term2, &arg2)) {
      // SgToken_ROSE_Fortran_Keywords
    } else return ATfalse;
  } else return ATfalse;

  *var_SgUntypedStatement = new SgUntypedStatement(arg1,arg2);

  return ATtrue;
}

//========================================================================================
// SgUntypedType
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedType(ATerm term, SgUntypedType** var_SgUntypedType)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedType: %s\n", ATwriteToString(term));
#endif

  ATerm term2, term9, term12;
  char* arg1;
  SgUntypedExpression* arg2;
  int arg3;
  int arg4;
  int arg5;
  int arg6;
  int arg7;
  int arg8;
  SgUntypedExpression* arg9;
  char* arg10;
  int arg11;
  //CER-FIXME  SgUntypedType::type_enum* arg12;
  SgUntypedType::type_enum arg12;

  //CER-FIXME  *var_SgUntypedType = NULL;
  if (ATmatch(term, "SgUntypedType(<str>,<term>,<int>,<int>,<int>,<int>,<int>,<int>,<term>,<str>,<int>,<term>)", &arg1,&term2,&arg3,&arg4,&arg5,&arg6,&arg7,&arg8,&term9,&arg10,&arg11,&term12)) {
    if (traverse_SgUntypedExpression(term2, &arg2)) {
      // SgUntypedExpression
    } else return ATfalse;
    if (traverse_SgUntypedExpression(term9, &arg9)) {
      // SgUntypedExpression
    } else return ATfalse;
    if (traverse_SgUntypedType_type_enum(term12, &arg12)) {
      // SgUntypedType_type_enum
    } else return ATfalse;
  } else return ATfalse;

  *var_SgUntypedType = new SgUntypedType(arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11,arg12);

  return ATtrue;
}

//========================================================================================
// SgUntypedAttribute
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedAttribute(ATerm term, SgUntypedAttribute** var_SgUntypedAttribute)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedAttribute: %s\n", ATwriteToString(term));
#endif

  ATerm term1;
  //CER-FIXME  SgToken::ROSE_Fortran_Keywords* arg1;
  SgToken::ROSE_Fortran_Keywords arg1;
  char* arg2;

  *var_SgUntypedAttribute = NULL;
  if (ATmatch(term, "SgUntypedAttribute(<term>,<str>)", &term1,&arg2)) {
    if (traverse_SgToken_ROSE_Fortran_Keywords(term1, &arg1)) {
      // SgToken_ROSE_Fortran_Keywords
    } else return ATfalse;
  } else return ATfalse;

  *var_SgUntypedAttribute = new SgUntypedAttribute(arg1,arg2);

  return ATtrue;
}

//========================================================================================
// SgUntypedInitializedName
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedInitializedName(ATerm term, SgUntypedInitializedName** var_SgUntypedInitializedName)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedInitializedName: %s\n", ATwriteToString(term));
#endif

  ATerm term1;
  SgUntypedType* arg1;
  char* arg2;

  *var_SgUntypedInitializedName = NULL;
  if (ATmatch(term, "SgUntypedInitializedName(<term>,<str>)", &term1,&arg2)) {
    if (traverse_SgUntypedType(term1, &arg1)) {
      // SgUntypedType
    } else return ATfalse;
  } else return ATfalse;

  *var_SgUntypedInitializedName = new SgUntypedInitializedName(arg1,arg2);

  return ATtrue;
}

//========================================================================================
// SgUntypedFile
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedFile(ATerm term, SgUntypedFile** var_SgUntypedFile)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedFile: %s\n", ATwriteToString(term));
#endif

  ATerm term1;
  SgUntypedGlobalScope* arg1;

  *var_SgUntypedFile = NULL;
  if (ATmatch(term, "SgUntypedFile(<term>)", &term1)) {
    if (traverse_SgUntypedGlobalScope(term1, &arg1)) {
      // SgUntypedGlobalScope
    } else return ATfalse;
  } else return ATfalse;

  *var_SgUntypedFile = new SgUntypedFile(arg1);

  return ATtrue;
}

//========================================================================================
// SgUntypedStatementList
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedStatementList(ATerm term, SgUntypedStatementList** var_SgUntypedStatementList)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedStatementList: %s\n", ATwriteToString(term));
#endif

  ATerm term1;

  *var_SgUntypedStatementList = NULL;
  if (ATmatch(term, "SgUntypedStatementList(<term>)", &term1)) {
     SgUntypedStatementList* plist = new SgUntypedStatementList();

     ATermList tail = (ATermList) ATmake("<term>", term1);
     while (! ATisEmpty(tail)) {
        SgUntypedStatement* arg;
        ATerm head = ATgetFirst(tail);
        tail = ATgetNext(tail);

        if (traverse_SgUntypedScope(head, (SgUntypedScope**) &arg)) {
           // SgUntypedScope
           plist->get_stmt_list().push_back(arg);
           continue;
        }
        if (traverse_SgUntypedOtherStatement(head, (SgUntypedOtherStatement**) &arg)) {
           // SgUntypedOtherStatement
           plist->get_stmt_list().push_back(arg);
           continue;
        }
        if (traverse_SgUntypedNamedStatement(head, (SgUntypedNamedStatement**) &arg)) {
           // SgUntypedNamedStatement
           plist->get_stmt_list().push_back(arg);
           continue;
        }
        if (traverse_SgUntypedBlockStatement(head, (SgUntypedBlockStatement**) &arg)) {
           // SgUntypedBlockStatement
           plist->get_stmt_list().push_back(arg);
           continue;
        }
        if (traverse_SgUntypedFunctionCallStatement(head, (SgUntypedFunctionCallStatement**) &arg)) {
           // SgUntypedFunctionCallStatement
           plist->get_stmt_list().push_back(arg);
           continue;
        }
        if (traverse_SgUntypedAssignmentStatement(head, (SgUntypedAssignmentStatement**) &arg)) {
           // SgUntypedAssignmentStatement
           plist->get_stmt_list().push_back(arg);
           continue;
        }
        if (traverse_SgUntypedDeclarationStatement(head, (SgUntypedDeclarationStatement**) &arg)) {
           // SgUntypedDeclarationStatement
           plist->get_stmt_list().push_back(arg);
           continue;
        }

        delete plist;
        return ATfalse;
     }
     *var_SgUntypedStatementList = plist;
  }
  else return ATfalse;

  return ATtrue;
}

//========================================================================================
// SgUntypedDeclarationStatementList
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedDeclarationStatementList(ATerm term, SgUntypedDeclarationStatementList** var_SgUntypedDeclarationStatementList)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedDeclarationStatementList: %s\n", ATwriteToString(term));
#endif

  ATerm term1;

  *var_SgUntypedDeclarationStatementList = NULL;
  if (ATmatch(term, "SgUntypedDeclarationStatementList(<term>)", &term1)) {
     SgUntypedDeclarationStatementList* plist = new SgUntypedDeclarationStatementList();

     ATermList tail = (ATermList) ATmake("<term>", term1);
     while (! ATisEmpty(tail)) {
        SgUntypedDeclarationStatement* arg;
        ATerm head = ATgetFirst(tail);
        tail = ATgetNext(tail);

        if (traverse_SgUntypedModuleDeclaration(head, (SgUntypedModuleDeclaration**) &arg)) {
           // SgUntypedModuleDeclaration
           plist->get_decl_list().push_back(arg);
           continue;
        }
        if (traverse_SgUntypedFunctionDeclaration(head, (SgUntypedFunctionDeclaration**) &arg)) {
           // SgUntypedFunctionDeclaration
           plist->get_decl_list().push_back(arg);
           continue;
        }
        if (traverse_SgUntypedVariableDeclaration(head, (SgUntypedVariableDeclaration**) &arg)) {
           // SgUntypedVariableDeclaration
           plist->get_decl_list().push_back(arg);
           continue;
        }
        if (traverse_SgUntypedImplicitDeclaration(head, (SgUntypedImplicitDeclaration**) &arg)) {
           // SgUntypedImplicitDeclaration
           plist->get_decl_list().push_back(arg);
           continue;
        }

        delete plist;
        return ATfalse;
     }
     *var_SgUntypedDeclarationStatementList = plist;
  }
  else return ATfalse;

  return ATtrue;
}

//========================================================================================
// SgUntypedFunctionDeclarationList
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedFunctionDeclarationList(ATerm term, SgUntypedFunctionDeclarationList** var_SgUntypedFunctionDeclarationList)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedFunctionDeclarationList: %s\n", ATwriteToString(term));
#endif

  ATerm term1;
  
  *var_SgUntypedFunctionDeclarationList = NULL;
  if (ATmatch(term, "SgUntypedFunctionDeclarationList(<term>)", &term1)) {
     SgUntypedFunctionDeclarationList* plist = new SgUntypedFunctionDeclarationList();

     ATermList tail = (ATermList) ATmake("<term>", term1);
     while (! ATisEmpty(tail)) {
        SgUntypedFunctionDeclaration* arg = NULL;
        ATerm head = ATgetFirst(tail);
        tail = ATgetNext(tail);
        if (traverse_SgUntypedSubroutineDeclaration(head, (SgUntypedSubroutineDeclaration**) &arg)) {
           // SgUntypedSubroutineDeclaration
           plist->get_func_list().push_back(arg);
           continue;
        }
        if (traverse_SgUntypedProgramHeaderDeclaration(head, (SgUntypedProgramHeaderDeclaration**) &arg)) {
           // SgUntypedProgramHeaderDeclaration
           plist->get_func_list().push_back(arg);
           continue;
        }

        delete plist;
        return ATfalse;
     }
     *var_SgUntypedFunctionDeclarationList = plist;
  }
  else return ATfalse;

  return ATtrue;
}

//========================================================================================
// SgUntypedInitializedNameList
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedInitializedNameList(ATerm term, SgUntypedInitializedNameList** var_SgUntypedInitializedNameList)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedInitializedNameList: %s\n", ATwriteToString(term));
#endif

  ATerm term1;
  
  *var_SgUntypedInitializedNameList = NULL;
  if (ATmatch(term, "SgUntypedInitializedNameList(<term>)", &term1)) {
     SgUntypedInitializedNameList* plist = new SgUntypedInitializedNameList();

     ATermList tail = (ATermList) ATmake("<term>", term1);
     while (! ATisEmpty(tail)) {
        SgUntypedInitializedName* arg;
        ATerm head = ATgetFirst(tail);
        tail = ATgetNext(tail);

        if (traverse_SgUntypedInitializedName(head, (SgUntypedInitializedName**) &arg)) {
           // SgUntypedInitializedName
           plist->get_name_list().push_back(arg);
           continue;
        }

        delete plist;
        return ATfalse;
     }
     *var_SgUntypedInitializedNameList = plist;
  }
  else return ATfalse;
  // turn on build functions (using BuildStmt) in sage-to-traverse.str

  return ATtrue;
}

//========================================================================================
// SgUntypedDeclarationStatement
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedDeclarationStatement(ATerm term, SgUntypedDeclarationStatement** var_SgUntypedDeclarationStatement)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedDeclarationStatement: %s\n", ATwriteToString(term));
#endif

  ATerm term2;
  char* arg1;
  //CER-FIXME  SgToken::ROSE_Fortran_Keywords* arg2;
  SgToken::ROSE_Fortran_Keywords arg2;
  
  *var_SgUntypedDeclarationStatement = NULL;
  if (ATmatch(term, "SgUntypedDeclarationStatement(<str>,<term>)", &arg1,&term2)) {
    if (traverse_SgToken_ROSE_Fortran_Keywords(term2, &arg2)) {
      // SgToken_ROSE_Fortran_Keywords
    } else return ATfalse;
  } else return ATfalse;

  *var_SgUntypedDeclarationStatement = new SgUntypedDeclarationStatement(arg1,arg2);

  return ATtrue;
}

//========================================================================================
// SgUntypedAssignmentStatement
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedAssignmentStatement(ATerm term, SgUntypedAssignmentStatement** var_SgUntypedAssignmentStatement)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedAssignmentStatement: %s\n", ATwriteToString(term));
#endif

  ATerm term2, term3, term4;
  char* arg1;
  //CER-FIXME  SgToken::ROSE_Fortran_Keywords* arg2;
  SgToken::ROSE_Fortran_Keywords arg2;
  SgUntypedExpression* arg3;
  SgUntypedExpression* arg4;
  
  *var_SgUntypedAssignmentStatement = NULL;
  if (ATmatch(term, "SgUntypedAssignmentStatement(<str>,<term>,<term>,<term>)", &arg1,&term2,&term3,&term4)) {
    if (traverse_SgToken_ROSE_Fortran_Keywords(term2, &arg2)) {
      // SgToken_ROSE_Fortran_Keywords
    } else return ATfalse;
    if (traverse_SgUntypedExpression(term3, &arg3)) {
      // SgUntypedExpression
    } else return ATfalse;
    if (traverse_SgUntypedExpression(term4, &arg4)) {
      // SgUntypedExpression
    } else return ATfalse;
  } else return ATfalse;

  *var_SgUntypedAssignmentStatement = new SgUntypedAssignmentStatement(arg1,arg2,arg3,arg4);

  return ATtrue;
}

//========================================================================================
// SgUntypedFunctionCallStatement
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedFunctionCallStatement(ATerm term, SgUntypedFunctionCallStatement** var_SgUntypedFunctionCallStatement)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedFunctionCallStatement: %s\n", ATwriteToString(term));
#endif

  ATerm term2;
  char* arg1;
  //CER-FIXME  SgToken::ROSE_Fortran_Keywords* arg2;
  SgToken::ROSE_Fortran_Keywords arg2;
  
  *var_SgUntypedFunctionCallStatement = NULL;
  if (ATmatch(term, "SgUntypedFunctionCallStatement(<str>,<term>)", &arg1,&term2)) {
    if (traverse_SgToken_ROSE_Fortran_Keywords(term2, &arg2)) {
      // SgToken_ROSE_Fortran_Keywords
    } else return ATfalse;
  } else return ATfalse;

  *var_SgUntypedFunctionCallStatement = new SgUntypedFunctionCallStatement(arg1,arg2);

  return ATtrue;
}

//========================================================================================
// SgUntypedBlockStatement
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedBlockStatement(ATerm term, SgUntypedBlockStatement** var_SgUntypedBlockStatement)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedBlockStatement: %s\n", ATwriteToString(term));
#endif

  ATerm term2, term3;
  char* arg1;
  //CER-FIXME  SgToken::ROSE_Fortran_Keywords* arg2;
  SgToken::ROSE_Fortran_Keywords arg2;
  SgUntypedScope* arg3;
  
  *var_SgUntypedBlockStatement = NULL;
  if (ATmatch(term, "SgUntypedBlockStatement(<str>,<term>,<term>)", &arg1,&term2,&term3)) {
    if (traverse_SgToken_ROSE_Fortran_Keywords(term2, &arg2)) {
      // SgToken_ROSE_Fortran_Keywords
    } else return ATfalse;
    if (traverse_SgUntypedScope(term3, &arg3)) {
      // SgUntypedScope
    } else return ATfalse;
  } else return ATfalse;

  *var_SgUntypedBlockStatement = new SgUntypedBlockStatement(arg1,arg2,arg3);

  return ATtrue;
}

//========================================================================================
// SgUntypedNamedStatement
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedNamedStatement(ATerm term, SgUntypedNamedStatement** var_SgUntypedNamedStatement)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedNamedStatement: %s\n", ATwriteToString(term));
#endif

  ATerm term2;
  char* arg1;
  //CER-FIXME  SgToken::ROSE_Fortran_Keywords* arg2;
  SgToken::ROSE_Fortran_Keywords arg2;
  char* arg3;
  
  *var_SgUntypedNamedStatement = NULL;
  if (ATmatch(term, "SgUntypedNamedStatement(<str>,<term>,<str>)", &arg1,&term2,&arg3)) {
    if (traverse_SgToken_ROSE_Fortran_Keywords(term2, &arg2)) {
      // SgToken_ROSE_Fortran_Keywords
    } else return ATfalse;
  } else return ATfalse;

  *var_SgUntypedNamedStatement = new SgUntypedNamedStatement(arg1,arg2,arg3);

  return ATtrue;
}

//========================================================================================
// SgUntypedOtherStatement
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedOtherStatement(ATerm term, SgUntypedOtherStatement** var_SgUntypedOtherStatement)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedOtherStatement: %s\n", ATwriteToString(term));
#endif

  ATerm term2;
  char* arg1;
  //CER-FIXME  SgToken::ROSE_Fortran_Keywords* arg2;
  SgToken::ROSE_Fortran_Keywords arg2;
  
  *var_SgUntypedOtherStatement = NULL;
  if (ATmatch(term, "SgUntypedOtherStatement(<str>,<term>)", &arg1,&term2)) {
    if (traverse_SgToken_ROSE_Fortran_Keywords(term2, &arg2)) {
      // SgToken_ROSE_Fortran_Keywords
    } else return ATfalse;
  } else return ATfalse;

  *var_SgUntypedOtherStatement = new SgUntypedOtherStatement(arg1,arg2);

  return ATtrue;
}

//========================================================================================
// SgUntypedScope
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedScope(ATerm term, SgUntypedScope** var_SgUntypedScope)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedScope: %s\n", ATwriteToString(term));
#endif

  ATerm term2, term3, term4, term5;
  char* arg1;
  //CER-FIXME  SgToken::ROSE_Fortran_Keywords* arg2;
  SgToken::ROSE_Fortran_Keywords arg2;
  SgUntypedDeclarationStatementList* arg3;
  SgUntypedStatementList* arg4;
  SgUntypedFunctionDeclarationList* arg5;
  
  *var_SgUntypedScope = NULL;
  if (ATmatch(term, "SgUntypedScope(<str>,<term>,<term>,<term>,<term>)", &arg1,&term2,&term3,&term4,&term5)) {
    if (traverse_SgToken_ROSE_Fortran_Keywords(term2, &arg2)) {
      // SgToken_ROSE_Fortran_Keywords
    } else return ATfalse;
    if (traverse_SgUntypedDeclarationStatementList(term3, &arg3)) {
      // SgUntypedDeclarationStatementList
    } else return ATfalse;
    if (traverse_SgUntypedStatementList(term4, &arg4)) {
      // SgUntypedStatementList
    } else return ATfalse;
    if (traverse_SgUntypedFunctionDeclarationList(term5, &arg5)) {
      // SgUntypedFunctionDeclarationList
    } else return ATfalse;
  } else return ATfalse;

  *var_SgUntypedScope = new SgUntypedScope(arg1,arg2,arg3,arg4,arg5);

  return ATtrue;
}

//========================================================================================
// SgUntypedFunctionScope
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedFunctionScope(ATerm term, SgUntypedFunctionScope** var_SgUntypedFunctionScope)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedFunctionScope: %s\n", ATwriteToString(term));
#endif

  ATerm term2, term3, term4, term5;
  char* arg1;
  //CER-FIXME  SgToken::ROSE_Fortran_Keywords* arg2;
  SgToken::ROSE_Fortran_Keywords arg2;
  SgUntypedDeclarationStatementList* arg3;
  SgUntypedStatementList* arg4;
  SgUntypedFunctionDeclarationList* arg5;
  
  *var_SgUntypedFunctionScope = NULL;
  if (ATmatch(term, "SgUntypedFunctionScope(<str>,<term>,<term>,<term>,<term>)", &arg1,&term2,&term3,&term4,&term5)) {
    if (traverse_SgToken_ROSE_Fortran_Keywords(term2, &arg2)) {
      // SgToken_ROSE_Fortran_Keywords
    } else return ATfalse;
    if (traverse_SgUntypedDeclarationStatementList(term3, &arg3)) {
      // SgUntypedDeclarationStatementList
    } else return ATfalse;
    if (traverse_SgUntypedStatementList(term4, &arg4)) {
      // SgUntypedStatementList
    } else return ATfalse;
    if (traverse_SgUntypedFunctionDeclarationList(term5, &arg5)) {
      // SgUntypedFunctionDeclarationList
    } else return ATfalse;
  } else return ATfalse;

  *var_SgUntypedFunctionScope = new SgUntypedFunctionScope(arg1,arg2,arg3,arg4,arg5);

  return ATtrue;
}

//========================================================================================
// SgUntypedModuleScope
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedModuleScope(ATerm term, SgUntypedModuleScope** var_SgUntypedModuleScope)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedModuleScope: %s\n", ATwriteToString(term));
#endif

  ATerm term2, term3, term4, term5;
  char* arg1;
  //CER-FIXME  SgToken::ROSE_Fortran_Keywords* arg2;
  SgToken::ROSE_Fortran_Keywords arg2;
  SgUntypedDeclarationStatementList* arg3;
  SgUntypedStatementList* arg4;
  SgUntypedFunctionDeclarationList* arg5;
  
  *var_SgUntypedModuleScope = NULL;
  if (ATmatch(term, "SgUntypedModuleScope(<str>,<term>,<term>,<term>,<term>)", &arg1,&term2,&term3,&term4,&term5)) {
    if (traverse_SgToken_ROSE_Fortran_Keywords(term2, &arg2)) {
      // SgToken_ROSE_Fortran_Keywords
    } else return ATfalse;
    if (traverse_SgUntypedDeclarationStatementList(term3, &arg3)) {
      // SgUntypedDeclarationStatementList
    } else return ATfalse;
    if (traverse_SgUntypedStatementList(term4, &arg4)) {
      // SgUntypedStatementList
    } else return ATfalse;
    if (traverse_SgUntypedFunctionDeclarationList(term5, &arg5)) {
      // SgUntypedFunctionDeclarationList
    } else return ATfalse;
  } else return ATfalse;

  *var_SgUntypedModuleScope = new SgUntypedModuleScope(arg1,arg2,arg3,arg4,arg5);

  return ATtrue;
}

//========================================================================================
// SgUntypedGlobalScope
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedGlobalScope(ATerm term, SgUntypedGlobalScope** var_SgUntypedGlobalScope)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedGlobalScope: %s\n", ATwriteToString(term));
#endif

  ATerm term2, term3, term4, term5;
  char* arg1;
  SgToken::ROSE_Fortran_Keywords arg2;
  SgUntypedDeclarationStatementList* arg3;
  SgUntypedStatementList* arg4;
  SgUntypedFunctionDeclarationList* arg5;
  
  *var_SgUntypedGlobalScope = NULL;
  if (ATmatch(term, "SgUntypedGlobalScope(<str>,<term>,<term>,<term>,<term>)", &arg1,&term2,&term3,&term4,&term5)) {
    if (traverse_SgToken_ROSE_Fortran_Keywords(term2, &arg2)) {
      // SgToken_ROSE_Fortran_Keywords
    } else return ATfalse;
    if (traverse_SgUntypedDeclarationStatementList(term3, &arg3)) {
      // SgUntypedDeclarationStatementList
    } else return ATfalse;
    if (traverse_SgUntypedStatementList(term4, &arg4)) {
      // SgUntypedStatementList
    } else return ATfalse;
    if (traverse_SgUntypedFunctionDeclarationList(term5, &arg5)) {
      // SgUntypedFunctionDeclarationList
    } else return ATfalse;
  } else return ATfalse;

  *var_SgUntypedGlobalScope = new SgUntypedGlobalScope(arg1,arg2,arg3,arg4,arg5);

  return ATtrue;
}

//========================================================================================
// SgUntypedArrayType
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedArrayType(ATerm term, SgUntypedArrayType** var_SgUntypedArrayType)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedArrayType: %s\n", ATwriteToString(term));
#endif

  ATerm term2, term9, term12, term13;
  char* arg1;
  SgUntypedExpression* arg2;
  int arg3;
  int arg4;
  int arg5;
  int arg6;
  int arg7;
  int arg8;
  SgUntypedExpression* arg9;
  char* arg10;
  int arg11;
  //CER-FIXME  SgUntypedType::type_enum* arg12;
  SgUntypedType::type_enum arg12;
  SgUntypedExpression* arg13;
  
  //CER-FIXME *var_SgUntypedArrayType = NULL;
  if (ATmatch(term, "SgUntypedArrayType(<str>,<term>,<int>,<int>,<int>,<int>,<int>,<int>,<term>,<str>,<int>,<term>,<term>)", &arg1,&term2,&arg3,&arg4,&arg5,&arg6,&arg7,&arg8,&term9,&arg10,&arg11,&term12,&term13)) {
    if (traverse_SgUntypedExpression(term2, &arg2)) {
      // SgUntypedExpression
    } else return ATfalse;
    if (traverse_SgUntypedExpression(term9, &arg9)) {
      // SgUntypedExpression
    } else return ATfalse;
    if (traverse_SgUntypedType_type_enum(term12, &arg12)) {
      // SgUntypedType_type_enum
    } else return ATfalse;
    if (traverse_SgUntypedExpression(term13, &arg13)) {
      // SgUntypedExpression
    } else return ATfalse;
  } else return ATfalse;

  *var_SgUntypedArrayType = new SgUntypedArrayType(arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11,arg12,arg13);

  return ATtrue;
}

