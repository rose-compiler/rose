//TODO
// 1. sage:bool -> bool (DONE)
// 2. Turn build functions back on using BuildStmt in sage-to-traverse.str (DONE)
// 3. Fix SgUntypedType and remove SgUntypedTypeFloat, ...  (DONE)
// 4. Fix SgUntypedExpression
// 5. SgUntypedFunctionDeclaration, SgUntypedDeclarationStatement, SgUntypedExpression
// - It's basically the base classes that need fixing

#include "rosePublicConfig.h"
#include "sage3basic.h"

#include "aterm2.h"

#include "traverse_SgUntypedNodes.hpp"
#include "build_SgUntypedNodes.hpp"

//FIXME --------------------------------------
// Must add by hand the three following
//

//========================================================================================
// SgUntypedFunctionDeclaration
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedFunctionDeclaration(ATerm term, SgUntypedFunctionDeclaration** var_SgUntypedFunctionDeclaration)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedFunctionDeclaration: %s\n", ATwriteToString(term));
#endif

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

  return ATtrue;
}

//FIXME END  ---------------------------------

//========================================================================================
// SgUntypedAttribute
//----------------------------------------------------------------------------------------
ATbool traverse_SgUntypedAttribute(ATerm term, SgUntypedAttribute** var_SgUntypedAttribute)
{
#ifdef DEBUG_PRINT
  printf("... traverse_SgUntypedAttribute: %s\n", ATwriteToString(term));
#endif

  int arg1;
  char* arg2;
  
  *var_SgUntypedAttribute = NULL;
  if (ATmatch(term, "SgUntypedAttribute(<int>,<str>)", &arg1,&arg2)) {
  } else return ATfalse;

  *var_SgUntypedAttribute = build_SgUntypedAttribute(arg1,arg2);

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

  *var_SgUntypedInitializedName = build_SgUntypedInitializedName(arg1,arg2);

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

  *var_SgUntypedFile = build_SgUntypedFile(arg1);

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
        ATerm head = ATgetFirst(tail);
        tail = ATgetNext(tail);
        {
           SgUntypedScope* arg;
           if (traverse_SgUntypedScope(head, &arg)) {
              // SgUntypedScope
              plist->get_stmt_list().push_back(arg);
           }
           else {
              delete plist;
              return ATfalse;
           }
        }
        {
           SgUntypedOtherStatement* arg;
           if (traverse_SgUntypedOtherStatement(head, &arg)) {
              // SgUntypedOtherStatement
              plist->get_stmt_list().push_back(arg);
           }
           else {
              delete plist;
              return ATfalse;
           }
        }
        {
           SgUntypedNamedStatement* arg;
           if (traverse_SgUntypedNamedStatement(head, &arg)) {
              // SgUntypedNamedStatement
              plist->get_stmt_list().push_back(arg);
           }
           else {
              delete plist;
              return ATfalse;
           }
        }
        {
           SgUntypedBlockStatement* arg;
           if (traverse_SgUntypedBlockStatement(head, &arg)) {
              // SgUntypedBlockStatement
              plist->get_stmt_list().push_back(arg);
           }
           else {
              delete plist;
              return ATfalse;
           }
        }
        {
           SgUntypedFunctionCallStatement* arg;
           if (traverse_SgUntypedFunctionCallStatement(head, &arg)) {
              // SgUntypedFunctionCallStatement
              plist->get_stmt_list().push_back(arg);
           }
           else {
              delete plist;
              return ATfalse;
           }
        }
        {
           SgUntypedAssignmentStatement* arg;
           if (traverse_SgUntypedAssignmentStatement(head, &arg)) {
              // SgUntypedAssignmentStatement
              plist->get_stmt_list().push_back(arg);
           }
           else {
              delete plist;
              return ATfalse;
           }
        }
        {
           SgUntypedDeclarationStatement* arg;
           if (traverse_SgUntypedDeclarationStatement(head, &arg)) {
              // SgUntypedDeclarationStatement
              plist->get_stmt_list().push_back(arg);
           }
           else {
              delete plist;
              return ATfalse;
           }
        }
     }
     *var_SgUntypedStatementList = plist;
  }
  else return ATfalse;
  // turn on build functions (using BuildStmt) in sage-to-traverse.str

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
        ATerm head = ATgetFirst(tail);
        tail = ATgetNext(tail);
        {
           SgUntypedSubroutineDeclaration* arg;
           if (traverse_SgUntypedSubroutineDeclaration(head, &arg)) {
              // SgUntypedSubroutineDeclaration
              plist->get_func_list().push_back(arg);
           }
           else {
              delete plist;
              return ATfalse;
           }
        }
        {
           SgUntypedProgramHeaderDeclaration* arg;
           if (traverse_SgUntypedProgramHeaderDeclaration(head, &arg)) {
              // SgUntypedProgramHeaderDeclaration
              plist->get_func_list().push_back(arg);
           }
           else {
              delete plist;
              return ATfalse;
           }
        }
     }
     *var_SgUntypedFunctionDeclarationList = plist;
  }
  else return ATfalse;
  // turn on build functions (using BuildStmt) in sage-to-traverse.str

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
        ATerm head = ATgetFirst(tail);
        tail = ATgetNext(tail);
        {
           SgUntypedInitializedName* arg;
           if (traverse_SgUntypedInitializedName(head, &arg)) {
              // SgUntypedInitializedName
              plist->get_name_list().push_back(arg);
           }
           else {
              delete plist;
              return ATfalse;
           }
        }
     }
     *var_SgUntypedInitializedNameList = plist;
  }
  else return ATfalse;
  // turn on build functions (using BuildStmt) in sage-to-traverse.str

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

  ATerm term3;
  int arg1;
  char* arg2;
  SgUntypedType* arg3;
  
  *var_SgUntypedValueExpression = NULL;
  if (ATmatch(term, "SgUntypedValueExpression(<int>,<str>,<term>)", &arg1,&arg2,&term3)) {
    if (traverse_SgUntypedType(term3, &arg3)) {
      // SgUntypedType
    } else return ATfalse;
  } else return ATfalse;

  *var_SgUntypedValueExpression = build_SgUntypedValueExpression(arg1,arg2,arg3);

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

  int arg1;
  
  *var_SgUntypedArrayReferenceExpression = NULL;
  if (ATmatch(term, "SgUntypedArrayReferenceExpression(<int>)", &arg1)) {
  } else return ATfalse;

  *var_SgUntypedArrayReferenceExpression = build_SgUntypedArrayReferenceExpression(arg1);

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

  int arg1;
  
  *var_SgUntypedOtherExpression = NULL;
  if (ATmatch(term, "SgUntypedOtherExpression(<int>)", &arg1)) {
  } else return ATfalse;

  *var_SgUntypedOtherExpression = build_SgUntypedOtherExpression(arg1);

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

  int arg1;
  
  *var_SgUntypedFunctionCallOrArrayReferenceExpression = NULL;
  if (ATmatch(term, "SgUntypedFunctionCallOrArrayReferenceExpression(<int>)", &arg1)) {
  } else return ATfalse;

  *var_SgUntypedFunctionCallOrArrayReferenceExpression = build_SgUntypedFunctionCallOrArrayReferenceExpression(arg1);

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

  int arg1;
  char* arg2;
  
  *var_SgUntypedReferenceExpression = NULL;
  if (ATmatch(term, "SgUntypedReferenceExpression(<int>,<str>)", &arg1,&arg2)) {
  } else return ATfalse;

  *var_SgUntypedReferenceExpression = build_SgUntypedReferenceExpression(arg1,arg2);

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

  ATerm term3, term4;
  char* arg1;
  int arg2;
  SgUntypedExpression* arg3;
  SgUntypedExpression* arg4;
  
  *var_SgUntypedAssignmentStatement = NULL;
  if (ATmatch(term, "SgUntypedAssignmentStatement(<str>,<int>,<term>,<term>)", &arg1,&arg2,&term3,&term4)) {
    if (traverse_SgUntypedExpression(term3, &arg3)) {
      // SgUntypedExpression
    } else return ATfalse;
    if (traverse_SgUntypedExpression(term4, &arg4)) {
      // SgUntypedExpression
    } else return ATfalse;
  } else return ATfalse;

  *var_SgUntypedAssignmentStatement = build_SgUntypedAssignmentStatement(arg1,arg2,arg3,arg4);

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

  char* arg1;
  int arg2;
  
  *var_SgUntypedFunctionCallStatement = NULL;
  if (ATmatch(term, "SgUntypedFunctionCallStatement(<str>,<int>)", &arg1,&arg2)) {
  } else return ATfalse;

  *var_SgUntypedFunctionCallStatement = build_SgUntypedFunctionCallStatement(arg1,arg2);

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

  char* arg1;
  int arg2;
  char* arg3;
  
  *var_SgUntypedNamedStatement = NULL;
  if (ATmatch(term, "SgUntypedNamedStatement(<str>,<int>,<str>)", &arg1,&arg2,&arg3)) {
  } else return ATfalse;

  *var_SgUntypedNamedStatement = build_SgUntypedNamedStatement(arg1,arg2,arg3);

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

  char* arg1;
  int arg2;
  
  *var_SgUntypedOtherStatement = NULL;
  if (ATmatch(term, "SgUntypedOtherStatement(<str>,<int>)", &arg1,&arg2)) {
  } else return ATfalse;

  *var_SgUntypedOtherStatement = build_SgUntypedOtherStatement(arg1,arg2);

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

  char* arg1;
  int arg2;
  
  *var_SgUntypedImplicitDeclaration = NULL;
  if (ATmatch(term, "SgUntypedImplicitDeclaration(<str>,<int>)", &arg1,&arg2)) {
  } else return ATfalse;

  *var_SgUntypedImplicitDeclaration = build_SgUntypedImplicitDeclaration(arg1,arg2);

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

  ATerm term3, term4;
  char* arg1;
  int arg2;
  SgUntypedType* arg3;
  SgUntypedInitializedNameList* arg4;
  
  *var_SgUntypedVariableDeclaration = NULL;
  if (ATmatch(term, "SgUntypedVariableDeclaration(<str>,<int>,<term>,<term>)", &arg1,&arg2,&term3,&term4)) {
    if (traverse_SgUntypedType(term3, &arg3)) {
      // SgUntypedType
    } else return ATfalse;
    if (traverse_SgUntypedInitializedNameList(term4, &arg4)) {
      // SgUntypedInitializedNameList
    } else return ATfalse;
  } else return ATfalse;

  *var_SgUntypedVariableDeclaration = build_SgUntypedVariableDeclaration(arg1,arg2,arg3,arg4);

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

  ATerm term4, term5;
  char* arg1;
  int arg2;
  char* arg3;
  SgUntypedModuleScope* arg4;
  SgUntypedNamedStatement* arg5;
  
  *var_SgUntypedModuleDeclaration = NULL;
  if (ATmatch(term, "SgUntypedModuleDeclaration(<str>,<int>,<str>,<term>,<term>)", &arg1,&arg2,&arg3,&term4,&term5)) {
    if (traverse_SgUntypedModuleScope(term4, &arg4)) {
      // SgUntypedModuleScope
    } else return ATfalse;
    if (traverse_SgUntypedNamedStatement(term5, &arg5)) {
      // SgUntypedNamedStatement
    } else return ATfalse;
  } else return ATfalse;

  *var_SgUntypedModuleDeclaration = build_SgUntypedModuleDeclaration(arg1,arg2,arg3,arg4,arg5);

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

  ATerm term4, term5, term6, term7;
  char* arg1;
  int arg2;
  char* arg3;
  SgUntypedInitializedNameList* arg4;
  SgUntypedType* arg5;
  SgUntypedFunctionScope* arg6;
  SgUntypedNamedStatement* arg7;
  
  *var_SgUntypedProgramHeaderDeclaration = NULL;
  if (ATmatch(term, "SgUntypedProgramHeaderDeclaration(<str>,<int>,<str>,<term>,<term>,<term>,<term>)", &arg1,&arg2,&arg3,&term4,&term5,&term6,&term7)) {
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

  *var_SgUntypedProgramHeaderDeclaration = build_SgUntypedProgramHeaderDeclaration(arg1,arg2,arg3,arg4,arg5,arg6,arg7);

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

  ATerm term4, term5, term6, term7;
  char* arg1;
  int arg2;
  char* arg3;
  SgUntypedInitializedNameList* arg4;
  SgUntypedType* arg5;
  SgUntypedFunctionScope* arg6;
  SgUntypedNamedStatement* arg7;
  
  *var_SgUntypedSubroutineDeclaration = NULL;
  if (ATmatch(term, "SgUntypedSubroutineDeclaration(<str>,<int>,<str>,<term>,<term>,<term>,<term>)", &arg1,&arg2,&arg3,&term4,&term5,&term6,&term7)) {
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

  *var_SgUntypedSubroutineDeclaration = build_SgUntypedSubroutineDeclaration(arg1,arg2,arg3,arg4,arg5,arg6,arg7);

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

  ATerm term3, term4, term5;
  char* arg1;
  int arg2;
  SgUntypedDeclarationStatementList* arg3;
  SgUntypedStatementList* arg4;
  SgUntypedFunctionDeclarationList* arg5;
  
  *var_SgUntypedFunctionScope = NULL;
  if (ATmatch(term, "SgUntypedFunctionScope(<str>,<int>,<term>,<term>,<term>)", &arg1,&arg2,&term3,&term4,&term5)) {
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

  *var_SgUntypedFunctionScope = build_SgUntypedFunctionScope(arg1,arg2,arg3,arg4,arg5);

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

  ATerm term3, term4, term5;
  char* arg1;
  int arg2;
  SgUntypedDeclarationStatementList* arg3;
  SgUntypedStatementList* arg4;
  SgUntypedFunctionDeclarationList* arg5;
  
  *var_SgUntypedModuleScope = NULL;
  if (ATmatch(term, "SgUntypedModuleScope(<str>,<int>,<term>,<term>,<term>)", &arg1,&arg2,&term3,&term4,&term5)) {
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

  *var_SgUntypedModuleScope = build_SgUntypedModuleScope(arg1,arg2,arg3,arg4,arg5);

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

  ATerm term3, term4, term5;
  char* arg1;
  int arg2;
  SgUntypedDeclarationStatementList* arg3;
  SgUntypedStatementList* arg4;
  SgUntypedFunctionDeclarationList* arg5;
  
  *var_SgUntypedGlobalScope = NULL;
  if (ATmatch(term, "SgUntypedGlobalScope(<str>,<int>,<term>,<term>,<term>)", &arg1,&arg2,&term3,&term4,&term5)) {
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

  *var_SgUntypedGlobalScope = build_SgUntypedGlobalScope(arg1,arg2,arg3,arg4,arg5);

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

  ATerm term2, term9;
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
  
  *var_SgUntypedArrayType = NULL;
  if (ATmatch(term, "SgUntypedArrayType(<str>,<term>,<int>,<int>,<int>,<int>,<int>,<int>,<term>,<str>,<int>)", &arg1,&term2,&arg3,&arg4,&arg5,&arg6,&arg7,&arg8,&term9,&arg10,&arg11)) {
    if (traverse_SgUntypedExpression(term2, &arg2)) {
      // SgUntypedExpression
    } else return ATfalse;
    if (traverse_SgUntypedExpression(term9, &arg9)) {
      // SgUntypedExpression
    } else return ATfalse;
  } else return ATfalse;

  *var_SgUntypedArrayType = build_SgUntypedArrayType(arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8,arg9,arg10,arg11);

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
        ATerm head = ATgetFirst(tail);
        tail = ATgetNext(tail);
        {
           SgUntypedModuleDeclaration* arg;
           if (traverse_SgUntypedModuleDeclaration(head, &arg)) {
              // SgUntypedModuleDeclaration
              plist->get_decl_list().push_back(arg);
           }
           else {
              delete plist;
              return ATfalse;
           }
        }
        {
           SgUntypedFunctionDeclaration* arg;
           if (traverse_SgUntypedFunctionDeclaration(head, &arg)) {
              // SgUntypedFunctionDeclaration
              plist->get_decl_list().push_back(arg);
           }
           else {
              delete plist;
              return ATfalse;
           }
        }
        {
           SgUntypedVariableDeclaration* arg;
           if (traverse_SgUntypedVariableDeclaration(head, &arg)) {
              // SgUntypedVariableDeclaration
              plist->get_decl_list().push_back(arg);
           }
           else {
              delete plist;
              return ATfalse;
           }
        }
        {
           SgUntypedImplicitDeclaration* arg;
           if (traverse_SgUntypedImplicitDeclaration(head, &arg)) {
              // SgUntypedImplicitDeclaration
              plist->get_decl_list().push_back(arg);
           }
           else {
              delete plist;
              return ATfalse;
           }
        }
     }
     *var_SgUntypedDeclarationStatementList = plist;
  }
  else return ATfalse;
  // turn on build functions (using BuildStmt) in sage-to-traverse.str

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

  ATerm term3, term4, term5;
  char* arg1;
  int arg2;
  SgUntypedDeclarationStatementList* arg3;
  SgUntypedStatementList* arg4;
  SgUntypedFunctionDeclarationList* arg5;
  
  *var_SgUntypedScope = NULL;
  if (ATmatch(term, "SgUntypedScope(<str>,<int>,<term>,<term>,<term>)", &arg1,&arg2,&term3,&term4,&term5)) {
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

  *var_SgUntypedScope = build_SgUntypedScope(arg1,arg2,arg3,arg4,arg5);

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

  ATerm term4, term5;
  char* arg1;
  int arg2;
  char* arg3;
  SgUntypedScope* arg4;
  SgUntypedNamedStatement* arg5;
  
  *var_SgUntypedBlockStatement = NULL;
  if (ATmatch(term, "SgUntypedBlockStatement(<str>,<int>,<str>,<term>,<term>)", &arg1,&arg2,&arg3,&term4,&term5)) {
    if (traverse_SgUntypedScope(term4, &arg4)) {
      // SgUntypedScope
    } else return ATfalse;
    if (traverse_SgUntypedNamedStatement(term5, &arg5)) {
      // SgUntypedNamedStatement
    } else return ATfalse;
  } else return ATfalse;

  *var_SgUntypedBlockStatement = build_SgUntypedBlockStatement(arg1,arg2,arg3,arg4,arg5);

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

  ATerm term4;
  int arg1;
  int arg2;
  char* arg3;
  SgUntypedExpression* arg4;
  
  *var_SgUntypedUnaryOperator = NULL;
  if (ATmatch(term, "SgUntypedUnaryOperator(<int>,<int>,<str>,<term>)", &arg1,&arg2,&arg3,&term4)) {
    if (traverse_SgUntypedExpression(term4, &arg4)) {
      // SgUntypedExpression
    } else return ATfalse;
  } else return ATfalse;

  *var_SgUntypedUnaryOperator = build_SgUntypedUnaryOperator(arg1,arg2,arg3,arg4);

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

  ATerm term4, term5;
  int arg1;
  int arg2;
  char* arg3;
  SgUntypedExpression* arg4;
  SgUntypedExpression* arg5;
  
  *var_SgUntypedBinaryOperator = NULL;
  if (ATmatch(term, "SgUntypedBinaryOperator(<int>,<int>,<str>,<term>,<term>)", &arg1,&arg2,&arg3,&term4,&term5)) {
    if (traverse_SgUntypedExpression(term4, &arg4)) {
      // SgUntypedExpression
    } else return ATfalse;
    if (traverse_SgUntypedExpression(term5, &arg5)) {
      // SgUntypedExpression
    } else return ATfalse;
  } else return ATfalse;

  *var_SgUntypedBinaryOperator = build_SgUntypedBinaryOperator(arg1,arg2,arg3,arg4,arg5);

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

  char* arg1;
  
  *var_SgUntypedType = NULL;
  if (ATmatch(term, "SgUntypedType(<str>)", &arg1)) {
  } else return ATfalse;

  *var_SgUntypedType = build_SgUntypedType(arg1);

  return ATtrue;
}

