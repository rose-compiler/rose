#include "sage3basic.h"

#include "ATermToUntypedFortranTraversal.h"
#include "general_language_translation.h"
#include "Fortran_to_ROSE_translation.h"
#include "untypedBuilder.h"
#include <iostream>

#define PRINT_ATERM_TRAVERSAL 0
#define PRINT_SOURCE_POSITION 0

using namespace ATermSupport;
using std::cout;
using std::cerr;
using std::endl;


ATermToUntypedFortranTraversal::ATermToUntypedFortranTraversal(SgSourceFile* source) : ATermToUntypedTraversal(source)
{
   UntypedBuilder::set_language(SgFile::e_Fortran_language);
}

static ATbool convert_list_item(ATerm term, const char* match, std::map<std::string,std::string> & map)
{
   char * str1, * str2;
   if (ATmatch(term, match, &str1, &str2)) {
      // MATCHED Rename
      cout << "--- Matched: " << match << " " << str1 << " " << str2 << endl;
      map[str1] = str2;
   } else return ATfalse;

   return ATtrue;
}

class ATListToMap
{
  public:
    ATListToMap(std::string match) : pMatch(match) { }

    ATbool operator() (ATerm term)
      {
         return convert_list_item(term, pMatch.c_str(), pMap);
      }

   const std::map<std::string,std::string> & getMap()  { return pMap; }

  private:
   std::string pMatch;
   std::map<std::string,std::string> pMap;
};

// Traverse a list
//
template <typename FuncType>
ATbool traverse_List(ATerm term, FuncType & convert_item)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_List: %s\n", ATwriteToString(term));
#endif

  ATermList tail = (ATermList) ATmake("<term>", term);
  while (! ATisEmpty(tail)) {
     ATerm head = ATgetFirst(tail);
     tail = ATgetNext(tail);
     if ( !convert_item(head) ) {
        return ATfalse;
     }
  }

  return ATtrue;
}

// Traverse a named list
//
template <typename FuncType>
ATbool traverse_List(ATerm term, const char * match, FuncType & convert_item)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_List: %s\n", ATwriteToString(term));
#endif

  ATerm terms;
  if (ATmatch(term, match, &terms)) {
     ATermList tail = (ATermList) ATmake("<term>", terms);
     while (! ATisEmpty(tail)) {
        ATerm head = ATgetFirst(tail);
        tail = ATgetNext(tail);
        if ( !convert_item(head) ) {
           return ATfalse;
        }
     }
     printf("--- found named list\n");
  }
  else return ATfalse;

  return ATtrue;
}

// Traverse an optional list
//
template <typename FuncType>
ATbool traverse_OptCommaList(ATerm term, FuncType & convert_item)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_OptionalCommaList: %s\n", ATwriteToString(term));
#endif

  ATerm terms;
  if (ATmatch(term, "no-list()")) {
  }
  else if (ATmatch(term, "comma-list(<term>)", &terms)) {
     ATermList tail = (ATermList) ATmake("<term>", terms);
     while (! ATisEmpty(tail)) {
        ATerm head = ATgetFirst(tail);
        tail = ATgetNext(tail);
        if ( !convert_item(head) ) {
           return ATfalse;
        }
     }
  }
  else return ATfalse;

  return ATtrue;
}

//========================================================================================
// Program (R201)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_Program(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Program: %s\n", ATwriteToString(term));
#endif

   SgUntypedGlobalScope* global_scope = get_scope();

   ATerm term1, term2;
   std::string start_comments;

   if (ATmatch(term, "Program(<term>,<term>)", &term1,&term2)) {
      if (traverse_StartCommentBlock(term1, start_comments)) {
         // MATCHED StartCommentBlock
      } else return ATfalse;

      if (traverse_ProgramUnitList(term2, global_scope)) {
         // MATCHED ProgramUnitList
      } else return ATfalse;

   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// ProgramUnitList (R202)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_ProgramUnitList(ATerm term, SgUntypedGlobalScope* scope)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ProgramUnitList: %s\n", ATwriteToString(term));
#endif

   ATermList tail = (ATermList) ATmake("<term>", term);

   while (! ATisEmpty(tail)) {
      ATerm head = ATgetFirst(tail);
      tail = ATgetNext(tail);

      if (traverse_MainProgram(head, scope)) {
         // MATCHED MainProgram
      }
      else if (traverse_SubroutineSubprogram(head, scope)) {
         // MATCHED SubroutineSubprogram
      }
      else if (traverse_FunctionSubprogram(head, scope)) {
         // MATCHED FunctionSubprogram
      }
      else if (traverse_Module(head, scope)) {
         // MATCHED Module
      }
      else if (traverse_Submodule(head, scope)) {
         // MATCHED Submodule
      }
      else if (traverse_BlockData(head, scope)) {
         // MATCHED BlockData
      }
      else {
         return ATfalse;
      }
   }

   return ATtrue;
}

//========================================================================================
// InternalSubprogramPart (R210)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_OptInternalSubprogramPart(ATerm term, SgUntypedOtherStatement** contains_stmt, SgUntypedScope* scope)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptInternalSubprogramPart: %s\n", ATwriteToString(term));
#endif
  
   ATerm term1, term2;

   if (ATmatch(term, "no-subprogram-part()")) {
      // MATCHED no-subprogram-part
   }
   else if (ATmatch(term, "SubprogramPart(<term>,<term>)", &term1,&term2)) {
      if (traverse_ContainsStmt(term1, contains_stmt)) {
         // MATCHED ContainsStmt
      } else return ATfalse;
      if (traverse_InternalSubprogramList(term2, scope)) {
         // MATCHED InternalSubprogram
      } else return ATfalse;
   } else return ATfalse;

  return ATtrue;
}

//========================================================================================
// InternalSubprogram (R211)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_InternalSubprogramList(ATerm term, SgUntypedScope* scope)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_InternalSubprogramList: %s\n", ATwriteToString(term));
#endif
  
  ATermList tail = (ATermList) ATmake("<term>", term);
  while (! ATisEmpty(tail)) {
     ATerm head = ATgetFirst(tail);
     tail = ATgetNext(tail);
     if (traverse_SubroutineSubprogram(head, scope)) {
        // MATCHED SubroutineSubprogram
     }
     else if (traverse_FunctionSubprogram(head, scope)) {
        // MATCHED FunctionSubprogram
     }
     else return ATfalse;
  }

  return ATtrue;
}

//========================================================================================
// StartCommentBlock
//  - this is optional
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_StartCommentBlock(ATerm term, std::string & var_StartCommentBlock)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_StartCommentBlock: %s\n", ATwriteToString(term));
#endif

   char* arg1;

   if (ATmatch(term, "no-comments()")) {
      // MATCHED no-comments
   }
   else if (ATmatch(term, "<str>", &arg1)) {
      // MATCHED string
      var_StartCommentBlock += arg1;
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// OptLabel
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_OptLabel(ATerm term, std::string & var_OptLabel)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_OptLabel: %s\n", ATwriteToString(term));
#endif

  char* arg1;
  
  if (ATmatch(term, "no-label()")) {
    // MATCHED no-comments
  }
  else if (ATmatch(term, "<str>", &arg1)) {
      var_OptLabel += arg1;
  }
  else return ATfalse;

  return ATtrue;
}

//========================================================================================
// Name
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_Name(ATerm term, std::string & name)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Name: %s\n", ATwriteToString(term));
#endif
  
   char* arg1;

   if (ATmatch(term, "<str>", &arg1)) {
      // MATCHED Name
      name += arg1;
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// OptName
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_OptName(ATerm term, std::string & name)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_OptName: %s\n", ATwriteToString(term));
#endif

  char* arg1;
  
  if (ATmatch(term, "no-name()")) {
    // MATCHED no-name
  }
  else if (ATmatch(term, "<str>", &arg1)) {
      name += arg1;
  }
  else return ATfalse;

  return ATtrue;
}

//========================================================================================
// NameList
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_NameList(ATerm term, SgUntypedNameList* name_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_NameList: %s\n", ATwriteToString(term));
#endif
  
   char* arg1;

   ATermList tail = (ATermList) ATmake("<term>", term);
   while (! ATisEmpty(tail)) {
      ATerm head = ATgetFirst(tail);
      tail = ATgetNext(tail);
      if (ATmatch(head, "<str>", &arg1)) {
         // Matched a name
         SgUntypedName* name = new SgUntypedName(arg1);
         setSourcePosition(name, head);
         name_list->get_name_list().push_back(name);
      } else return ATfalse;
   }

   return ATtrue;
}

//========================================================================================
// eos
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_eos(ATerm term, std::string & var_eos)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_eos: %s\n", ATwriteToString(term));
#endif

   ATerm term1;
   char* arg1;
  
   if (ATmatch(term, "eos(<term>)", &term1)) {
      if (ATmatch(term1, "<str>", &arg1)) {
         // MATCHED eos string
         var_eos += arg1;
      } else return ATfalse;
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// InitialSpecPart
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_InitialSpecPart(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_InitialSpecPart: %s\n", ATwriteToString(term));
#endif

   ATerm use_stmts;
   ATerm import_stmts;
  
   if (ATmatch(term, "InitialSpecPart(<term>,<term>)", &use_stmts, &import_stmts)) {
      if (traverse_UseStmtList(use_stmts, decl_list)) {
         // Matched UseStmtList
      } else return ATfalse;
      if (traverse_ImportStmtList(import_stmts, decl_list)) {
         // Matched ImportStmtList
      } else return ATfalse;
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// defined-operator (R310)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_DefinedOperator(ATerm term, std::string & name)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_DefinedOperator: %s\n", ATwriteToString(term));
#endif

   cout << "TODO - traverse_DefinedOperator \n";

   //  DefinedUnaryOp                -> DefinedOperator {prefer} %% AMBIGUOUS both unary and binary are DOP
   //  DefinedBinaryOp               -> DefinedOperator
   //  ExtendedIntrinsicOp           -> DefinedOperator
   //  Dop                           -> DefinedBinaryOp

   //  "." Letter+ "."               -> Dop

   return ATfalse;
}

//========================================================================================
// specification-part (R204)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_SpecificationPart(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SpecificationPart: %s\n", ATwriteToString(term));
#endif

   ATerm term1, term2, term3, term4;

   if (ATmatch(term, "SpecificationPart(<term>,<term>,<term>,<term>)", &term1,&term2,&term3,&term4)) {

      if (traverse_UseStmtList(term1, decl_list)) {
         // MATCHED UseStmtList
      } else return ATfalse;
      if (traverse_ImportStmtList(term2, decl_list)) {
         // MATCHED ImportStmtList
      } else return ATfalse;
      if (traverse_OptImplicitPart(term3, decl_list)) {
         // MATCHED OptImplicitPart
      } else return ATfalse;
      if (traverse_DeclarationConstructList(term4, decl_list)) {
         // MATCHED DeclarationConstruct list
      } else return ATfalse;

   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// SpecAndExecPart
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_SpecAndExecPart(ATerm term, SgUntypedDeclarationStatementList* decl_list,
                                                                  SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SpecAndExecPart: %s\n", ATwriteToString(term));
#endif

   ATerm term1;

   if (ATmatch(term, "SpecAndExecPart(<term>)", &term1)) {
      bool seen_first_exec_stmt = false;

      ATermList tail = (ATermList) ATmake("<term>", term1);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);

         if (seen_first_exec_stmt == false) {
            if (traverse_SpecStmt(head, decl_list)) {
               // MATCHED a declaration
            }
            else if (traverse_ExecStmt(head, stmt_list)) {
               // MATCHED an executable statement
               seen_first_exec_stmt = true;
            }
            else return ATfalse;
         }
         else {
            // have seen first executable statement
            if (traverse_ExecutionPartConstruct(head, stmt_list)) {
               // MATCHED a specification/executable statement
            } else return ATfalse;
         }
      }
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// implicit-part (R205)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_OptImplicitPart(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptImplicitPart: %s\n", ATwriteToString(term));
#endif

   ATerm t_stmt_list, t_implicit_stmt;

   if (ATmatch(term, "no-implicit-part()")) {
      // MATCHED no-implicit-part
   }
   else if (ATmatch(term, "ImplicitPart(<term>,<term>)", &t_stmt_list, &t_implicit_stmt)) {
      // MATCHED ImplicitPart
      if (traverse_ImplicitPartStmtList(t_stmt_list, decl_list)) {
         // MATCHED ImplicitPartStmt list
      } else return ATfalse;
      if (traverse_ImplicitStmt(t_implicit_stmt, decl_list)) {
         // MATCHED ImplicitStmt
      } else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// implicit-part-stmt (R206)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_ImplicitPartStmtList(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ImplicitPartStmtList: \n");
#endif

// NOTE: The statements in this list are formally supposed to end with an ImplicitStmt.
//       However, we let the grammar take care of ordering and just process potential
//       list members.

   ATermList tail = (ATermList) ATmake("<term>", term);
   while (! ATisEmpty(tail)) {
      ATerm head = ATgetFirst(tail);
      tail = ATgetNext(tail);

      if (traverse_ImplicitStmt(head, decl_list)) {
         // MATCHED ImplicitStmt
      }
#if 0
      else if (traverse_ParameterStmt(head, decl_list)) {
         // MATCHED ParameterStmt
      }
      else if (traverse_FormatStmt(head, decl_list)) {
         // MATCHED FormatStmt
      }
      else if (traverse_EntryStmt(head, decl_list)) {
         // MATCHED EntryStmt
      }
#endif
      else return ATfalse;
   }

   return ATtrue;
}

//========================================================================================
// declaration-construct (R207)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_DeclarationConstruct(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_DeclarationConstruct: %s\n", ATwriteToString(term));
#endif

   if (traverse_InterfaceBlock(term, decl_list)) {
      // MATCHED InterfaceBlock
   }

   else if (traverse_DerivedTypeDef(term, decl_list)) {
      // MATCHED DerivedTypeDef
   }

#if 0
   else if (traverse_EntryStmt(term, decl_list)) {
      // MATCHED EntryStmt
   }
   else if (traverse_EnumDef(term, decl_list)) {
      // MATCHED EnumDef
   }
   else if (traverse_FormatStmt(term, decl_list)) {
      // MATCHED FormatStmt
   }
   else if (traverse_ParameterStmt(term, decl_list)) {
      // MATCHED ParameterStmt
   }
   else if (traverse_ProcedureDeclarationStmt(term, decl_list)) {
      // MATCHED ProcedureDeclarationStmt
   }
#endif

   else if (traverse_TypeDeclarationStmt(term, decl_list)) {
      // MATCHED TypeDeclarationStmt
   }

#if 0
// TODO - is this correct?
   else if (traverse_IncludeStmt(term, decl_list)) {
      // MATCHED IncludeStmtStmt
   }
#endif
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// DeclarationConstructList
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_DeclarationConstructList(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_DeclarationConstructList: %s\n", ATwriteToString(term));
#endif

   ATermList tail = (ATermList) ATmake("<term>", term);
   while (! ATisEmpty(tail)) {
      ATerm head = ATgetFirst(tail);
      tail = ATgetNext(tail);

      if (traverse_DeclarationConstruct(head, decl_list)) {
         // MATCHED DeclarationConstruct
      }
      else return ATfalse;
   }

   return ATtrue;
}

//========================================================================================
// execution-part-construct (R209)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_ExecutionPartConstruct(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ExecutionPartConstruct: %s\n", ATwriteToString(term));
#endif

// TODO - Not sure about the format, entry, and data statements here (see grammar Main.sdf and BlockExecutionPartConstruct)
// TODO - traverse_FormatStmt
// TODO - traverse_EntryStmt
// TODO - traverse_DataStmt
   if (traverse_ExecStmt(term, stmt_list)) {
      // MATCHED an executable statement
   }
   else {
      std::cerr << "...TODO... implement FormatStmt, EntryStmt, DataStmt in ExecutionPartConstruct" << std::endl;
      return ATfalse;
   }

   return ATtrue;
}

ATbool ATermToUntypedFortranTraversal::traverse_BlockExecutionPartConstruct(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BlockExecutionPartConstruct: %s\n", ATwriteToString(term));
#endif

// TODO - traverse_FormatStmt
// TODO - traverse_EntryStmt
// TODO - traverse_DataStmt
   if (traverse_ExecStmt(term, stmt_list)) {
      // MATCHED an executable statement
   }
   else {
      std::cerr << "...TODO... implement FormatStmt, EntryStmt, DataStmt in BlockExecutionPartConstruct" << std::endl;
      return ATfalse;
   }

   return ATtrue;
}

// List of possible declarations statements
//
//  ImplicitStmt                           -> SpecAndExecConstruct
//  DeclarationConstruct                   -> SpecAndExecConstruct
//  ExecutableConstruct                    -> SpecAndExecConstruct
//
//  DerivedTypeDef                         -> DeclarationConstruct
//  EntryStmt                              -> DeclarationConstruct
//  EnumDef                                -> DeclarationConstruct
//  FormatStmt                             -> DeclarationConstruct
//  InterfaceBlock                         -> DeclarationConstruct
//  ParameterStmt                          -> DeclarationConstruct
//  ProcedureDeclarationStmt               -> DeclarationConstruct
//  OtherSpecificationStmt                 -> DeclarationConstruct
//  TypeDeclarationStmt                    -> DeclarationConstruct
//%%StmtFunctionStmt                       -> DeclarationConstruct  %% AMBIGUOUS with array assignment

//========================================================================================
// Traverse specification statements
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_SpecStmt(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SpecStmt: %s\n", ATwriteToString(term));
#endif
  
   if (traverse_TypeDeclarationStmt(term, decl_list)) {
      // Matched TypeDeclarationStmt
   }
   else if (traverse_DerivedTypeDef(term, decl_list)) {
      // Matched DerivedTypeDef
   }
   else if (traverse_ImplicitStmt(term, decl_list)) {
      // Matched ImplicitStmt
   }
   else if (traverse_DimensionStmt(term, decl_list)) {
      // Matched ExternalStmt
   }
   else if (traverse_ExternalStmt(term, decl_list)) {
      // Matched ExternalStmt
   }
   else if (traverse_CudaAttributesStmt(term, decl_list)) {
      // Matched ExternalStmt
   }

   //  EntryStmt                              -> DeclarationConstruct
   //  EnumDef                                -> DeclarationConstruct
   //  FormatStmt                             -> DeclarationConstruct
   //  InterfaceBlock                         -> DeclarationConstruct
   //  ParameterStmt                          -> DeclarationConstruct
   //  ProcedureDeclarationStmt               -> DeclarationConstruct
   //  OtherSpecificationStmt                 -> DeclarationConstruct

   else {
      return ATfalse;
   }

   return ATtrue;
}

//========================================================================================
// Traverse executable statements
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_ExecStmt(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ExecStmt: %s\n", ATwriteToString(term));
#endif
  
   if (traverse_AssignmentStmt(term, stmt_list)) {
      // Matched AssignmentStmt
   }
   else if (traverse_CallStmt(term, stmt_list)) {
      // Matched CallStmt
   }
   else if (traverse_IfConstruct(term, stmt_list)) {
      // Matched IfConstruct
   }
   else if (traverse_IfStmt(term, stmt_list)) {
      // Matched IfStmt
   }
   else if (traverse_NonlabelDoStmt(term, stmt_list)) {
      // Matched NonlabelDoStmt
   }
   else if (traverse_EndDoStmt(term, stmt_list)) {
      // Matched EndDoStmt
   }
#if 0
   else if (traverse_LabelDoStmt(term, stmt_list)) {
      // Matched LabelDoStmt
   }
#endif
   else if (traverse_BlockConstruct(term, stmt_list)) {
      // Matched BlockConstruct
   }
   else if (traverse_CaseConstruct(term, stmt_list)) {
      // Matched ContinueStmt
   }
   else if (traverse_ContinueStmt(term, stmt_list)) {
      // Matched ContinueStmt
   }
   else if (traverse_ForallConstruct(term, stmt_list)) {
      // Matched ForallConstruct
   }
   else if (traverse_ForallStmt(term, stmt_list)) {
      // Matched ForallStmt
      // Deprecated
   }
   else if (traverse_GotoStmt(term, stmt_list)) {
      // Matched GotoStmt
   }
   else if (traverse_StopStmt(term, stmt_list)) {
      // Matched StopStmt
   }
   else if (traverse_ErrorStopStmt(term, stmt_list)) {
      // Matched ErrorStopStmt
   }
   else if (traverse_ReturnStmt(term, stmt_list)) {
      // Matched ReturnStmt
   }

// Image control statements (F2018)
   else if (traverse_SyncAllStmt(term, stmt_list)) {
      // Matched SyncAllStmt
   }
   else if (traverse_SyncImagesStmt(term, stmt_list)) {
      // Matched SyncImagesStmt
   }
   else if (traverse_SyncMemoryStmt(term, stmt_list)) {
      // Matched SyncMemoryStmt
   }
   else if (traverse_SyncTeamStmt(term, stmt_list)) {
      // Matched SyncTeamStmt
   }
   else if (traverse_LockStmt(term, stmt_list)) {
      // Matched LockStmt
   }
   else if (traverse_UnlockStmt(term, stmt_list)) {
      // Matched UnlockStmt
   }

   else {
      return ATfalse;
   }

   return ATtrue;
}

//========================================================================================
// LetterSpec
//----------------------------------------------------------------------------------------
class LetterSpecMatch
{
 public:
   LetterSpecMatch(std::vector<FAST::LetterSpec>* list) : pLetterSpecList(list)
      {
      }
   ATbool operator() (ATerm term)
      {
         FAST::LetterSpec letter_spec;
         char * begin, * end;
         if (ATmatch(term, "LetterSpec(<str>,<str>)", &begin, &end)) {
            letter_spec.setLetterBegin(begin[0]);
            letter_spec.setLetterEnd   (end[0]);
         }
         else if (ATmatch(term, "LetterSpec(<str>,no-letter())", &begin)) {
            letter_spec.setLetterBegin(begin[0]);
         }
         else return ATfalse;

         pLetterSpecList->push_back(letter_spec);

         return ATtrue;
      }
 private:
   std::vector<FAST::LetterSpec>* pLetterSpecList;
};

//========================================================================================
// ImplicitSpec
//----------------------------------------------------------------------------------------
class ImplicitSpecMatch
{
 public:
   ImplicitSpecMatch(ATermToUntypedFortranTraversal* traversal, std::vector<FAST::ImplicitSpec>* list)
      : pTraversal(traversal), pImplicitSpecList(list)
      {
      }
    ATbool operator() (ATerm term)
      {
         ATerm term1, term2;
         SgUntypedType* type_spec;
         std::vector<FAST::LetterSpec> letter_spec_list;
         LetterSpecMatch match_letter_spec(&letter_spec_list);
         if (ATmatch(term, "ImplicitSpec(<term>,<term>)", &term1, &term2)) {
            if (pTraversal->traverse_DeclarationTypeSpec(term1, type_spec)) {
            } else return ATfalse;
            if (traverse_List(term2, match_letter_spec)) {
            } else return ATfalse;
         } else return ATfalse;

#ifdef PUT_BACK
         FAST::ImplicitSpec implicit_spec(type_spec, getLocation(term));
         pImplicitSpecList->push_back(implicit_spec);
#endif

         return ATtrue;
      }
 protected:
   ATermToUntypedFortranTraversal* pTraversal;
   std::vector<FAST::ImplicitSpec>* pImplicitSpecList;
};

//========================================================================================
// DeclarationTypeSpec
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_DeclarationTypeSpec(ATerm term, SgUntypedType* & type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_DeclarationTypeSpec: %s\n", ATwriteToString(term));
#endif
  
   if (traverse_IntrinsicTypeSpec(term, type)) {
      //MATCHED IntrinsicTypeSpec
   }
   else if (traverse_DerivedTypeSpec(term, type)) {
      //MATCHED DerivedTypeSpec
   }
   else return ATfalse;

  return ATtrue;
}

//========================================================================================
// IntrinsicTypeSpec
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_IntrinsicTypeSpec(ATerm term, SgUntypedType* & type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_IntrinsicTypeSpec: %s\n", ATwriteToString(term));
#endif
  
   ATerm t_type, t_kind;

   type = NULL;

   if (ATmatch(term, "IntrinsicType(<term>)", &t_type)) {
      // MATCHED IntrinsicType
   } else return ATfalse;

// Check for type declarations without type-kind parameter
   if (ATmatch(t_type, "INTEGER()")) {
      type = UntypedBuilder::buildType(SgUntypedType::e_int);
   }
   else if (ATmatch(t_type, "REAL()")) {
      type = UntypedBuilder::buildType(SgUntypedType::e_float);
   }
   else if (ATmatch(t_type, "DOUBLEPRECISION()")) {
      //type = UntypedBuilder::buildType(SgUntypedType::e_float);
   }
   else if (ATmatch(t_type, "CHARACTER()")) {
   // No length parameter given so build a char type
      type = UntypedBuilder::buildType(SgUntypedType::e_char);
   }
   else if (ATmatch(t_type, "COMPLEX()")) {
      type = UntypedBuilder::buildType(SgUntypedType::e_complex);
   }
   else if (ATmatch(t_type, "LOGICAL()")) {
      type = UntypedBuilder::buildType(SgUntypedType::e_bool);
   }
   else if (ATmatch(t_type, "DOUBLECOMPLEX()")) {
      //type = UntypedBuilder::buildType(SgUntypedType::e_float);
   }

   if (type != NULL) {
   // have a type without a type-kind parameter, return
      return ATtrue;
   }

// Only type declarations with a kind parameter or length parameters left
// ----------------------------------------------------------------------

   if (ATmatch(t_type, "INTEGER(<term>)", &t_kind)) {
      type = UntypedBuilder::buildType(SgUntypedType::e_int);
   }
   else if (ATmatch(t_type, "REAL(<term>)", &t_kind)) {
      type = UntypedBuilder::buildType(SgUntypedType::e_float);
   }
   else if (ATmatch(t_type, "COMPLEX(<term>)", &t_kind)) {
      //type = UntypedBuilder::buildType(SgUntypedType::e_complex);
      return ATfalse;
   }
   else if (ATmatch(t_type, "LOGICAL(<term>)", &t_kind)) {
      type = UntypedBuilder::buildType(SgUntypedType::e_bool);
   }

// Check for a type-kind parameter
   if (type != NULL) {
      SgUntypedExpression* kind;
      if (traverse_KindSelector(t_kind, &kind)) {
      // add type kind expression
         type->set_has_kind(true);
         type->set_type_kind(kind);
      }
      else {
         cerr << "ERROR: no expected kind-type parameter in IntrinsictypeSpec" << endl;
         return ATfalse;
      }
   }
   else {
      cerr << "ERROR: unknown intrinsic type in IntrinsictypeSpec" << endl;
      return ATfalse;
   }

// TODO - Check for a character string
   if (type != NULL) {
   // have a type with a type-kind parameter, return
      return ATtrue;
   }

  return ATtrue;
}

//========================================================================================
// DerivedTypeSpec
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_DerivedTypeSpec(ATerm term, SgUntypedType* & type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_DerivedTypeSpec: %s\n", ATwriteToString(term));
#endif

   ATerm t_type, t_list;
   char* name;

   bool is_class = false;
   bool is_user_defined = false;
   bool is_assumed_class = false;
   bool is_assumed_type  = false;

   type = NULL;

   if (ATmatch(term, "derived-type-spec(<term>)", &t_type)) {
      is_user_defined = true;
   }
   else if (ATmatch(term, "class-type-spec(<term>)", &t_type)) {
      is_class = true;
   }
   else if (ATmatch(term, "AssumedClass()")) {
      is_assumed_class = true;
      std::cerr << "...TODO... found AssumedClass" << std::endl;
   }
   else if (ATmatch(term, "AssumedType()")) {
      is_assumed_type = true;
      std::cerr << "...TODO... found AssumedClass" << std::endl;
   }
   else return ATfalse;

   if (ATmatch(t_type, "DerivedTypeSpec(<str>,<term>)", &name, &t_list)) {
      // DerivedTypeSpec applies for both TYPE and CLASS

      if (ATmatch(t_list, "no-list()")) {
         // There is no DerivedParamSpecList
      }
      else {
         std::cerr << "...TODO... found DerivedTypeSpec name " << name << " : need to implement TypeParamSpecList \n";
         ROSE_ASSERT(false);
         return ATfalse;
      }

      type = UntypedBuilder::buildType(SgUntypedType::e_user_defined, name);
      type->set_is_class(is_class);
   }

   else if (is_assumed_class || is_assumed_type) {
   }

   else return ATfalse;

   if (type == NULL) {
      std::cerr << "...TODO... implement DerivedTypeSpec" << std::endl;
      ROSE_ASSERT(false);
   }

   return ATtrue;
}

//========================================================================================
// R305 literal-constant
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_LiteralConstant(ATerm term, SgUntypedExpression** var_expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_LiteralConstant: %s\n", ATwriteToString(term));
#endif

   ATerm t_kind;
   char* arg1;
   std::string value;
   SgToken::ROSE_Fortran_Keywords keyword = SgToken::FORTRAN_TYPE;
   SgUntypedType* type;

   *var_expr = NULL;
   if (ATmatch(term, "IntVal(<str>)", &arg1)) {
      value += arg1;
      type = UntypedBuilder::buildType(SgUntypedType::e_int);
   }
   else if (ATmatch(term, "RealVal(<str>)", &arg1)) {
      value += arg1;
      type = UntypedBuilder::buildType(SgUntypedType::e_float);
   }
   else if (ATmatch(term, "TRUE()")) {
      value += "TRUE";
      type = UntypedBuilder::buildType(SgUntypedType::e_bool);
   }
   else if (ATmatch(term, "FALSE()")) {
      value += "FALSE";
      type = UntypedBuilder::buildType(SgUntypedType::e_bool);
   }

// Handle literals with kind expressions
   else if (ATmatch(term, "IntVal(<str>,<term>)", &arg1,&t_kind)) {
      SgUntypedExpression* kind;
      if (traverse_Expression(t_kind, &kind)) {
         // MATCHED Expression
      } else return ATfalse;
      value += arg1;
      type = UntypedBuilder::buildType(SgUntypedType::e_int);
   // add type kind expression
      type->set_has_kind(true);
      type->set_type_kind(kind);
   }
   else if (ATmatch(term, "TRUE(<term>)", &t_kind)) {
      SgUntypedExpression* kind;
      if (traverse_Expression(t_kind, &kind)) {
         // MATCHED Expression
      } else return ATfalse;
      value += "TRUE";
      type = UntypedBuilder::buildType(SgUntypedType::e_bool);
   // add type kind expression
      type->set_has_kind(true);
      type->set_type_kind(kind);
   }
   else return ATfalse;

   *var_expr = new SgUntypedValueExpression(keyword,value,type);
   setSourcePosition(*var_expr, term);

   return ATtrue;
}

//========================================================================================
// R309 intrinsic-operator
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_Operator(ATerm term, SgUntypedExpression** var_expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Operator: %s\n", ATwriteToString(term));
#endif

   ATerm term1, term2;

   int op_enum;
   std::string op_name;
   bool is_unary_op;
   SgUntypedExpression* lhs;
   SgUntypedExpression* rhs;

   *var_expr = NULL;

// Unary operators
//
   is_unary_op = false;

   if (ATmatch(term, "UnaryMinus(<term>)", &term1)) {
      op_enum = General_Language_Translation::e_operator_unary_minus;
      op_name = "-";
      is_unary_op = true;
   }
   else if (ATmatch(term, "UnaryPlus(<term>)", &term1)) {
      op_enum = General_Language_Translation::e_operator_unary_plus;
      op_name = "+";
      is_unary_op = true;
   }
   else if (ATmatch(term, "Parens(<term>)", &term1)) {
      // TODO - test this, it seems to work OK
      cerr << "...WARNING...: throwing away Parens Operator in traverse_Operator (test expressions)\n";
      return traverse_Expression(term1, var_expr);
   }

   if (is_unary_op) {
      SgUntypedExpression* expr;
      if (traverse_Expression(term1, &expr)) {
         // MATCHED Expression
      } else return ATfalse;

      *var_expr = new SgUntypedUnaryOperator(op_enum,op_name,expr);
      setSourcePosition(*var_expr, term);

      return ATtrue;
   }

// Binary operators
//
   if (ATmatch(term, "Power(<term>,<term>)", &term1,&term2)) {
      op_enum = General_Language_Translation::e_operator_exponentiate;
      op_name = "**";
   }
   else if (ATmatch(term, "Mult(<term>,<term>)", &term1,&term2)) {
      op_enum = General_Language_Translation::e_operator_multiply;
      op_name = "*";
   }
   else if (ATmatch(term, "Div(<term>,<term>)", &term1,&term2)) {
      op_enum = General_Language_Translation::e_operator_divide;
      op_name = "/";
   }
   else if (ATmatch(term, "Plus(<term>,<term>)", &term1,&term2)) {
      op_enum = General_Language_Translation::e_operator_add;
      op_name = "+";
   }
   else if (ATmatch(term, "Minus(<term>,<term>)", &term1,&term2)) {
      op_enum = General_Language_Translation::e_operator_subtract;
      op_name = "-";
   }
   else if (ATmatch(term, "EQ(<term>,<term>)", &term1,&term2)) {
      op_enum = General_Language_Translation::e_operator_equality;
      op_name = "==";
   }
   else if (ATmatch(term, "NE(<term>,<term>)", &term1,&term2)) {
      op_enum = General_Language_Translation::e_operator_not_equal;
      op_name = "/=";
   }
   else if (ATmatch(term, "LT(<term>,<term>)", &term1,&term2)) {
      op_enum = General_Language_Translation::e_operator_less_than;
      op_name = "<";
   }
   else if (ATmatch(term, "LE(<term>,<term>)", &term1,&term2)) {
      op_enum = General_Language_Translation::e_operator_less_than_or_equal;
      op_name = "<=";
   }
   else if (ATmatch(term, "GT(<term>,<term>)", &term1,&term2)) {
      op_enum = General_Language_Translation::e_operator_greater_than;
      op_name = ">";
   }
   else if (ATmatch(term, "GE(<term>,<term>)", &term1,&term2)) {
      op_enum = General_Language_Translation::e_operator_greater_than_or_equal;
      op_name = ">=";
   }
   else {
      return ATfalse;
   }
  
   if (traverse_Expression(term1, &lhs)) {
      // MATCHED Expression
   } else return ATfalse;
   if (traverse_Expression(term2, &rhs)) {
      // MATCHED Expression
   } else return ATfalse;

   ROSE_ASSERT(lhs);
   ROSE_ASSERT(rhs);

   *var_expr = new SgUntypedBinaryOperator(op_enum,op_name,lhs,rhs);
   setSourcePosition(*var_expr, term);

   return ATtrue;
}

//========================================================================================
// R405 kind-selector
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_KindSelector(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_KindSelector: %s\n", ATwriteToString(term));
#endif

   ATerm t_kind;

   if (ATmatch(term, "Kind(<term>)", &t_kind)) {
      if (traverse_Expression(t_kind, expr)) {
         // MATCHED type-kind expression
      } else return ATfalse;
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// R422 char-length (optional only for now)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_OptCharLength(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptCharLength: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-char-length()")) {
   }

   else {
      //TODO - implement CharLength
      std::cerr << "...TODO... implement OptCharLength" << std::endl;
      return ATfalse;
   }

   return ATtrue;
}

//========================================================================================
// R425 derived-type-def
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_DerivedTypeDef(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_DerivedTypeDef: %s\n", ATwriteToString(term));
#endif

   ATerm t_type_stmt, t_param, t_private, t_components, t_type_bound, t_end_stmt;

   if (ATmatch(term, "DerivedTypeDef(<term>,<term>,<term>,<term>,<term>,<term>)",
                     &t_type_stmt, &t_param, &t_private, &t_components, &t_type_bound, &t_end_stmt)) {
   }
   else return ATfalse;

// Handle the DerivedTypeStmt here as all of the information needed has been matched
//
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_DerivedTypeStmt: %s\n", ATwriteToString(t_type_stmt));
#endif

   ATerm t_label, t_name, t_attr_list, t_param_list, t_eos;

   char* name;
   std::string label, struct_name, end_stmt_name, end_stmt_label, eos;

   SgUntypedStructureDeclaration* struct_decl = NULL;
   SgUntypedStructureDefinition*  struct_def  = NULL;
   SgUntypedExprListExpression*     attr_list = NULL;
   SgUntypedExprListExpression*      dim_info = NULL;

   if (ATmatch(t_type_stmt, "DerivedTypeStmt(<term>,<term>,<str>,<term>,<term>)",
                            &t_label, &t_attr_list, &name, &t_param_list, &t_eos)) {
      struct_name = name;

   // Traverse the easy stuff first before creating new nodes
   //
      if (traverse_OptLabel(t_label, label)) {
         // MATCHED OptLabel
      } else return ATfalse;

      if (traverse_eos(t_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;

      // EndTypeStmt can pretty much be ignored (except for name)
      if (ATmatch(t_end_stmt, "EndTypeStmt(<term>,<term>,<term>)", &t_label, &t_name, &t_eos)) {

         if (traverse_OptLabel(t_label, end_stmt_label)) {
            // MATCHED OptLabel
         } else return ATfalse;
         if (traverse_eos(t_eos, eos)) {
            // MATCHED eos string
         } else return ATfalse;

         if (traverse_OptName(t_name, end_stmt_name)) {
            // MATCHED OptName
         } else return ATfalse;

         // if there is an end_stmt_name the names must match
         if (end_stmt_name.length() > 0) {
            ROSE_ASSERT(struct_name == end_stmt_name);
         }

      } else return ATfalse;

      struct_def = UntypedBuilder::buildStructureDefinition(struct_name, /*has_body*/true, /*scope*/NULL);
      ROSE_ASSERT(struct_def);

      SgUntypedScope* struct_scope = struct_def->get_scope();
      ROSE_ASSERT(struct_scope);

      attr_list = new SgUntypedExprListExpression(General_Language_Translation::e_struct_modifier_list);
      ROSE_ASSERT(attr_list);
      setSourcePosition(attr_list, t_attr_list);

      // This won't be used for Fortran
      dim_info = new SgUntypedExprListExpression(General_Language_Translation::e_array_shape);
      ROSE_ASSERT(dim_info);
      SageInterface::setSourcePosition(dim_info);

// TODO
      std::cout << "...TODO... TypeParamNameList not implemented \n";
#if 0
      if (traverse_TypeAttrSpecList(t_attr_list, attr_list)) {
         // MATCHED TypeAttrSpecList
      } else return ATfalse;
#endif

      // TODO: TypeParamNameList needs to be implemented
      if (ATmatch(t_param_list, "no-list()")) {
      }
      else {
         std::cout << "TODO: TypeParamNameList not implemented \n";
         ROSE_ASSERT(0);
      }

      // traverse components adding to struct_scope
      SgUntypedDeclarationStatementList* component_decl_list = struct_scope->get_declaration_list();
      ROSE_ASSERT(component_decl_list);

      if (traverse_DataComponentDefStmtList(t_components, component_decl_list)) {
         // Added components
      }
      else ROSE_ASSERT(0); // Not completely implemented so fail locally for now

      // TODO:
      // t_param
      // t_private, can this be added to the attr_list?
      // t_type_bound

      struct_decl = new SgUntypedStructureDeclaration(label, struct_name, attr_list, dim_info, struct_def);
      ROSE_ASSERT(struct_decl);
      setSourcePosition(struct_decl, term);
   }
   else return ATfalse;

   ROSE_ASSERT(struct_decl);

   decl_list->get_decl_list().push_back(struct_decl);

   return ATtrue;
}

//========================================================================================
// R436 DataComponentDefStmt
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_DataComponentDefStmt(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_DataComponentDefStmt: %s\n", ATwriteToString(term));
#endif

   printf("...TODO... DataComponentDefStmt: \n");

   return ATfalse;
}

//========================================================================================
// DataComponentDefStmtList
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_DataComponentDefStmtList(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_DataComponentDefStmtList: %s\n", ATwriteToString(term));
#endif

   ATermList tail = (ATermList) ATmake("<term>", term);
   while (! ATisEmpty(tail)) {
      ATerm head = ATgetFirst(tail);
      tail = ATgetNext(tail);

      if (traverse_DataComponentDefStmt(head, decl_list)) {
         // MATCHED DeclarationConstruct
      }
      else return ATfalse;
   }

   return ATtrue;
}

//========================================================================================
// R501 type-declaration-stmt
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_TypeDeclarationStmt(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_TypeDeclarationStmt: %s\n", ATwriteToString(term));
#endif
  
   ATerm term1, term2, term3, term4, term_eos;
   std::string label;
   std::string eos;
   SgUntypedType* declared_type;

   SgUntypedVariableDeclaration* variable_decl = NULL;
   SgUntypedInitializedNameList* var_name_list = NULL;
   SgUntypedExprListExpression*      attr_list = NULL;

   if (ATmatch(term, "TypeDeclarationStmt(<term>,<term>,<term>,<term>,<term>)", &term1,&term2,&term3,&term4,&term_eos)) {
      if (traverse_OptLabel(term1, label)) {
         // MATCHED OptLabel
      } else return ATfalse;

      if (traverse_DeclarationTypeSpec(term2, declared_type)) {
         // MATCHED DeclarationTypeSpec
         ROSE_ASSERT(declared_type);
      } else return ATfalse;

      attr_list = new SgUntypedExprListExpression(General_Language_Translation::e_type_modifier_list);
      if (traverse_OptAttrSpecList(term3, attr_list)) {
         // MATCHED AttrSpecList
      } else return ATfalse;

      var_name_list = new SgUntypedInitializedNameList();
      if (traverse_EntityDeclList(term4, declared_type, var_name_list)) {
         // MATCHED EntityDeclList
         setSourcePosition(var_name_list, term4);
      } else return ATfalse;

      if (traverse_eos(term_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

// SgUntyped additions:
//   1. AttrSpecList, this can be a list of enums as the array specification can be placed in SgUntypedArrayType
//   ! NO NO NO -> BIND(C,expression)

   std::cerr << "...TODO... fully implement AttrSpecList in TypeDeclarationStmt: list is " << attr_list << std::endl;

   variable_decl = new SgUntypedVariableDeclaration(label, declared_type, attr_list, var_name_list);
   setSourcePositionExcludingTerm(variable_decl, term, term_eos);

   decl_list->get_decl_list().push_back(variable_decl);

  return ATtrue;
}

//========================================================================================
// R502 attr-spec
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_OptAttrSpecList(ATerm term, SgUntypedExprListExpression* attr_list)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_OptAttrSpecList: %s\n", ATwriteToString(term));
#endif

  using namespace General_Language_Translation;

  ATerm terms;

  if (ATmatch(term, "no-list()")) {
  }

  else if (ATmatch(term, "comma-list(<term>)", &terms)) {
     ATermList tail = (ATermList) ATmake("<term>", terms);
     while (! ATisEmpty(tail)) {
        SgUntypedExpression* attr;
        ATerm head = ATgetFirst(tail);
        tail = ATgetNext(tail);
        if (ATmatch(head, "PUBLIC()")) {
           attr = new SgUntypedOtherExpression(e_access_modifier_public);
        }
        else if (ATmatch(head, "PRIVATE()")) {
           attr = new SgUntypedOtherExpression(e_access_modifier_private);
        }
        else if (ATmatch(head, "ALLOCATABLE()")) {
           attr = new SgUntypedOtherExpression(e_type_modifier_allocatable);
        }
        else if (ATmatch(head, "ASYNCHRONOUS()")) {
           attr = new SgUntypedOtherExpression(e_type_modifier_asynchronous);
        }
     // TODO - Codimension
        else if (ATmatch(head, "CONTIGUOUS()")) {
           attr = new SgUntypedOtherExpression(e_storage_modifier_contiguous);
        }
     // TODO - Dimension
        else if (ATmatch(head, "Dimension()")) {
           attr = new SgUntypedOtherExpression(e_unknown);
           return ATfalse;
        }
        else if (ATmatch(head, "EXTERNAL()")) {
           attr = new SgUntypedOtherExpression(e_storage_modifier_external);
        }
        else if (ATmatch(head, "Intent(IN())")) {
           attr = new SgUntypedOtherExpression(e_type_modifier_intent_in);
        }
        else if (ATmatch(head, "Intent(OUT())")) {
           attr = new SgUntypedOtherExpression(e_type_modifier_intent_out);
        }
        else if (ATmatch(head, "Intent(INOUT())")) {
           attr = new SgUntypedOtherExpression(e_type_modifier_intent_inout);
        }
        else if (ATmatch(head, "INTRINSIC()")) {
           attr = new SgUntypedOtherExpression(e_type_modifier_intrinsic);
        }
     // TODO - LanguageBindingSpec
        else if (ATmatch(head, "OPTIONAL()")) {
           attr = new SgUntypedOtherExpression(e_type_modifier_optional);
        }
        else if (ATmatch(head, "PARAMETER()")) {
           attr = new SgUntypedOtherExpression(e_type_modifier_const);
        }
        else if (ATmatch(head, "POINTER()")) {
           attr = new SgUntypedOtherExpression(e_type_modifier_pointer);
        }
        else if (ATmatch(head, "PROTECTED()")) {
           attr = new SgUntypedOtherExpression(e_type_modifier_protected);
        }
        else if (ATmatch(head, "SAVE()")) {
           attr = new SgUntypedOtherExpression(e_type_modifier_save);
        }
        else if (ATmatch(head, "TARGET()")) {
           attr = new SgUntypedOtherExpression(e_type_modifier_target);
        }
        else if (ATmatch(head, "VALUE()")) {
           attr = new SgUntypedOtherExpression(e_type_modifier_value);
        }
        else if (ATmatch(head, "VOLATILE()")) {
           attr = new SgUntypedOtherExpression(e_type_modifier_volatile);
        }
     // CUDA variable attributes/modifiers/qualifiers
     // ---------------------------------------------
        else if (ATmatch(head, "CUDA_DEVICE()")) {
           attr = new SgUntypedOtherExpression(e_cuda_device_memory);
        }
        else if (ATmatch(head, "CUDA_MANAGED()")) {
           attr = new SgUntypedOtherExpression(e_cuda_managed);
        }
        else if (ATmatch(head, "CUDA_CONSTANT()")) {
           attr = new SgUntypedOtherExpression(e_cuda_constant);
        }
        else if (ATmatch(head, "CUDA_SHARED()")) {
           attr = new SgUntypedOtherExpression(e_cuda_shared);
        }
        else if (ATmatch(head, "CUDA_PINNED()")) {
           attr = new SgUntypedOtherExpression(e_cuda_pinned);
        }
        else if (ATmatch(head, "CUDA_TEXTURE()")) {
           attr = new SgUntypedOtherExpression(e_cuda_texture);
        }

        else {
           std::cerr << "...TODO... finish attributes in OptAttrSpecList" << std::endl;
           return ATfalse;
        }

        setSourcePosition(attr, head);
        attr_list->get_expressions().push_back(attr);
     }
  }
  else return ATfalse;

  return ATtrue;
}

ATbool ATermToUntypedFortranTraversal::traverse_CudaAttributesSpec(ATerm term, SgUntypedOtherExpression** attr_spec)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CudaAttributesSpec: %s\n", ATwriteToString(term));
#endif

   *attr_spec = NULL;

   if (ATmatch(term, "CudaAttributesSpec(CUDA_DEVICE())")) {
      *attr_spec = new SgUntypedOtherExpression(General_Language_Translation::e_cuda_device_memory);
   }
   else if (ATmatch(term, "CudaAttributesSpec(CUDA_MANAGED())")) {
      *attr_spec = new SgUntypedOtherExpression(General_Language_Translation::e_cuda_managed);
   }
   else if (ATmatch(term, "CudaAttributesSpec(CUDA_CONSTANT())")) {
      *attr_spec = new SgUntypedOtherExpression(General_Language_Translation::e_cuda_constant);
   }
   else if (ATmatch(term, "CudaAttributesSpec(CUDA_SHARED())")) {
      *attr_spec = new SgUntypedOtherExpression(General_Language_Translation::e_cuda_shared);
   }
   else if (ATmatch(term, "CudaAttributesSpec(CUDA_PINNED())")) {
      *attr_spec = new SgUntypedOtherExpression(General_Language_Translation::e_cuda_pinned);
   }
   else if (ATmatch(term, "CudaAttributesSpec(CUDA_TEXTURE())")) {
      *attr_spec = new SgUntypedOtherExpression(General_Language_Translation::e_cuda_texture);
   }
   else return ATfalse;

   setSourcePosition(*attr_spec, term);

   return ATtrue;
}

//========================================================================================
// R503 entity-decl
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_EntityDecl(ATerm term, SgUntypedType* declared_type, SgUntypedInitializedNameList* name_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_EntityDecl: %s\n", ATwriteToString(term));
#endif

   ATerm t_name, t_array_spec, t_coarray_spec, t_char_length, eos_term;
   std::string name;

   SgUntypedArrayType* array_type = NULL;
   SgUntypedInitializedName* initialized_name = NULL;
   SgUntypedExpression* char_length = NULL;
   SgUntypedExpression* initialization = NULL;

   SgUntypedType* type = declared_type;

   if (ATmatch(term, "EntityDecl(<term>,<term>,<term>,<term>,<term>)",&t_name,&t_array_spec,&t_coarray_spec,&t_char_length,&eos_term)) {
      if (traverse_Name(t_name, name)) {
         // MATCHED ObjectName
      } else return ATfalse;

      if (traverse_OptArraySpec(t_array_spec, type, &array_type)) {
         if (array_type != NULL) type = array_type;
      } else return ATfalse;

      if (traverse_OptCoarraySpec(t_coarray_spec, type, &array_type)) {
         //TODO - implement in traverseal
         if (array_type != NULL) type = array_type;
      } else return ATfalse;

      if (traverse_OptCharLength(t_char_length, &char_length)) {
         // MATCHED OptCharLength
         if (char_length != NULL) {
            type->set_char_length_expression(char_length);
         }
      } else return ATfalse;

   // TODO_SgUntyped - SgUntypedInitializedName/SgUntypedType requires ability to initialize
      if (initialization != NULL) {
         type->set_char_length_expression(char_length);
      }

   } else return ATfalse;

   initialized_name = new SgUntypedInitializedName(type, name);
   setSourcePosition(initialized_name, term);

   name_list->get_name_list().push_back(initialized_name);

   return ATtrue;
}

//========================================================================================
// entity-decl-list
//  - this is a bare list (without a cons name)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_EntityDeclList(ATerm term, SgUntypedType* declared_type, SgUntypedInitializedNameList* name_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_EntityDeclList: %s\n", ATwriteToString(term));
#endif

   ATermList tail = (ATermList) ATmake("<term>", term);
   while (! ATisEmpty(tail)) {
      ATerm head = ATgetFirst(tail);
      tail = ATgetNext(tail);

      if (traverse_EntityDecl(head, declared_type, name_list)) {
         // MATCHED EntityDecl
      } else return ATfalse;
   }

   return ATtrue;
};

//========================================================================================
// R505 initialization (optional only for now)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_OptInitialization(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Initialization: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-init()")) {
   }
   else {
      //TODO - implement initialization
      std::cerr << "...TODO... implement OptInitialization" << std::endl;
      return ATfalse;
   }

   return ATtrue;
}

//========================================================================================
// R509 coarray-spec
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_CoarraySpec(ATerm term, SgUntypedType* base_type, SgUntypedArrayType** array_type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CoarraySpec: %s\n", ATwriteToString(term));
#endif

 //TODO - implement CoarraySpec
   std::cerr << "...TODO... implement OptCoarraySpec" << std::endl;
   return ATfalse;
}

ATbool ATermToUntypedFortranTraversal::traverse_OptCoarraySpec(ATerm term, SgUntypedType* base_type, SgUntypedArrayType** array_type)
{
   if (ATmatch(term, "no-list()")) {
      // MATCHED no-list()
   }
   else if (traverse_CoarraySpec(term, base_type, array_type)) {
      // MATCHED CoarraySpec
   }
   else {
      return ATfalse;
   }
   return ATtrue;
}

//========================================================================================
// R515 array-spec
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_ArraySpec(ATerm term, SgUntypedType* base_type, SgUntypedArrayType** array_type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ArraySpec: %s\n", ATwriteToString(term));
#endif

   ATerm t_array_spec_arg;
   SgUntypedSubscriptExpression *range;
   SgUntypedExpression *lower_bound, *upper_bound, *stride;
   SgUntypedExprListExpression *dim_info;
   int expr_enum = General_Language_Translation::e_unknown;
   int rank = 0;

   ROSE_ASSERT(base_type != NULL);
   *array_type = NULL;

   if (ATmatch(term, "ArraySpec(<term>)", &t_array_spec_arg)) {

      dim_info = new SgUntypedExprListExpression(General_Language_Translation::e_array_shape);
      setSourcePosition(dim_info, t_array_spec_arg);

   // check for non-list array-spec term first, e.g., dimension A(*)
   // 
      if (traverse_AssumedOrImpliedSpec(t_array_spec_arg, &lower_bound)) {
         upper_bound = UntypedBuilder::buildUntypedNullExpression();
         setSourcePositionUnknown(upper_bound);

         stride = UntypedBuilder::buildUntypedNullExpression();
         setSourcePositionUnknown(stride);

         expr_enum = General_Language_Translation::e_assumed_or_implied_shape;
         range = new SgUntypedSubscriptExpression(expr_enum, lower_bound, upper_bound, stride);
         setSourcePosition(range, t_array_spec_arg);

         dim_info->get_expressions().push_back(range);

      // assumed and implied shape arrays have rank 1
         rank = 1;
      // TODO: The builder should probably be based on the declared type
         *array_type = UntypedBuilder::buildArrayType(base_type->get_type_enum_id(),dim_info,rank);

        return ATtrue;
      }

   // check for an assumed-size array, e.g., dimension A(3,*)
   // 
      if (traverse_AssumedSize(t_array_spec_arg, base_type, array_type)) {
         return ATtrue;
      }

   // final choice is a list of array-spec terms
   //
      ATermList tail = (ATermList) ATmake("<term>", t_array_spec_arg);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_ExplicitShape(head, &lower_bound, &upper_bound)) {
            expr_enum = General_Language_Translation::e_explicit_shape;

            rank += 1;

            stride = UntypedBuilder::buildUntypedNullExpression();
            setSourcePositionUnknown(stride);

            range = new SgUntypedSubscriptExpression(expr_enum, lower_bound, upper_bound, stride);
            setSourcePosition(range, head);

            dim_info->get_expressions().push_back(range);
         }
         else if (traverse_AssumedShape(head, &lower_bound)) {
            cout << "... this is assumed shape why care about expr_num == " << expr_enum << endl;
    //??            ROSE_ASSERT(expr_enum == General_Language_Translation::e_unknown ||
    //??                        expr_enum == General_Language_Translation::e_assumed_size);
            expr_enum = General_Language_Translation::e_assumed_shape;

            rank += 1;

            upper_bound = UntypedBuilder::buildUntypedNullExpression();
            setSourcePositionUnknown(upper_bound);

            stride = UntypedBuilder::buildUntypedNullExpression();
            setSourcePositionUnknown(stride);

            range = new SgUntypedSubscriptExpression(expr_enum, lower_bound, upper_bound, stride);
            setSourcePosition(range, head);

            dim_info->get_expressions().push_back(range);
         }
        else {
           std::cerr << "traverse_ArraySpec: ERROR in ArraySpec list" << std::endl;
           return ATfalse;
        }
      }

   // TODO: The builder should probably be based on the declared (or base) type
      *array_type = UntypedBuilder::buildArrayType(base_type->get_type_enum_id(),dim_info,rank);
      cout << ".x. in ArraySpec type is " << *array_type << " : " << (*array_type)->class_name() << endl;
   }
   else {
      return ATfalse;
   }

   return ATtrue;
}

ATbool ATermToUntypedFortranTraversal::traverse_OptArraySpec(ATerm term, SgUntypedType* base_type, SgUntypedArrayType** array_type)
{
   if (ATmatch(term, "no-list()")) {
      // MATCHED no-list()
   }
   else if (traverse_ArraySpec(term, base_type, array_type)) {
      // MATCHED ArraySpec
   }
   else {
      return ATfalse;
   }
   return ATtrue;
}

//========================================================================================
// R516 explicit-shape-spec
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_ExplicitShape(ATerm term, SgUntypedExpression** lower_bound, SgUntypedExpression** upper_bound)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ExplicitShape: %s\n", ATwriteToString(term));
#endif

   ATerm t_lower_bound, t_upper_bound;

   *lower_bound = NULL;
   *upper_bound = NULL;

   if (ATmatch(term, "ExplicitShape(<term>,<term>)", &t_lower_bound, &t_upper_bound)) {
      if (ATmatch(t_lower_bound, "no-lower-bound()")) {
         *lower_bound = UntypedBuilder::buildUntypedNullExpression();
         setSourcePositionUnknown(*lower_bound);
      }
      else if (traverse_Expression(t_lower_bound, lower_bound)) {
      } else return ATfalse;
      
      if (traverse_Expression(t_upper_bound, upper_bound)) {
      } else return ATfalse;
      
   } else return ATfalse;

   return ATtrue;
}

ATbool ATermToUntypedFortranTraversal::traverse_ExplicitShapeList(ATerm term, SgUntypedExprListExpression* dim_info)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ExplicitShapeList: %s\n", ATwriteToString(term));
#endif

   SgUntypedExpression *lower_bound, *upper_bound, *stride, *range;

   // traverse list of explicit-shape terms
   //
   ATermList tail = (ATermList) ATmake("<term>", term);
   while (! ATisEmpty(tail)) {
      ATerm head = ATgetFirst(tail);
      tail = ATgetNext(tail);
      if (traverse_ExplicitShape(head, &lower_bound, &upper_bound)) {
         int expr_enum = General_Language_Translation::e_explicit_shape;
         
         if (isSgUntypedNullExpression(lower_bound))
            {
            // can be simplifified by only using upper bound
               delete lower_bound;
               range = upper_bound;
            }
         else
            {
            // add stride explicitly and let unparser ignore it
               stride = new SgUntypedValueExpression(SgToken::FORTRAN_TYPE,"1",
                                                     UntypedBuilder::buildType(SgUntypedType::e_int));
               setSourcePositionUnknown(stride);
         
               range = new SgUntypedSubscriptExpression(expr_enum, lower_bound, upper_bound, stride);
               setSourcePosition(range, head);
#if 0
               cout << ".x. ExplicitShape       range " << range << endl;
               cout << ".x. ExplicitShape lower_bound " << lower_bound << endl;
               cout << ".x. ExplicitShape upper_bound " << upper_bound << endl;
               cout << ".x. ExplicitShape      stride " << stride << endl;
#endif
            }
         
         dim_info->get_expressions().push_back(range);

      } else return ATfalse;
   }

   return ATtrue;
}

//========================================================================================
// R519 assumed-shape-spec
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_AssumedShape(ATerm term, SgUntypedExpression** lower_bound)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_AssumedShape: %s\n", ATwriteToString(term));
#endif

   ATerm t_lower_bound;

   *lower_bound = NULL;

   if (ATmatch(term, "AssumedShape(<term>)", &t_lower_bound)) {
      if (ATmatch(t_lower_bound, "no-lower-bound()")) {
         *lower_bound = UntypedBuilder::buildUntypedNullExpression();
         setSourcePositionUnknown(*lower_bound);
      }
      else if (traverse_Expression(t_lower_bound, lower_bound)) {
      } else return ATfalse;

   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// R521 assumed-size
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_AssumedSize(ATerm term, SgUntypedType* base_type, SgUntypedArrayType** array_type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_AssumedSize: %s\n", ATwriteToString(term));
#endif

   int rank;
   ATerm t_explicit_shape_list, t_lower_bound;
   SgUntypedExpression *lower_bound, *upper_bound, *stride;
   SgUntypedExprListExpression* dim_info = NULL;

   if (ATmatch(term, "AssumedSize(<term>,<term>)", &t_explicit_shape_list, &t_lower_bound)) {

      dim_info = new SgUntypedExprListExpression(General_Language_Translation::e_array_shape);
      setSourcePosition(dim_info, term);

   // traverse the list of explicit shape dimension before the final '*'
      if (traverse_ExplicitShapeList(t_explicit_shape_list, dim_info)) {
         // list of ExplicitShape terms are added to dim_info
      } else return ATfalse;

   // match the final dimension which makes it assumed-size, i.e., '*'
      if (ATmatch(t_lower_bound, "no-lower-bound()")) {
         lower_bound = UntypedBuilder::buildUntypedNullExpression();
         setSourcePositionUnknown(lower_bound);
      }
      else if (traverse_Expression(t_lower_bound, &lower_bound)) {
      } else return ATfalse;

      upper_bound = UntypedBuilder::buildUntypedNullExpression();
      setSourcePositionUnknown(upper_bound);

      stride = UntypedBuilder::buildUntypedNullExpression();
      setSourcePositionUnknown(stride);

      int expr_enum = General_Language_Translation::e_assumed_size;
      SgUntypedSubscriptExpression* range = new SgUntypedSubscriptExpression(expr_enum, lower_bound, upper_bound, stride);
      setSourcePosition(range, t_lower_bound);

      dim_info->get_expressions().push_back(range);
      dim_info->get_expressions().size();

#if 0
      cout << ".x. AssumedSize    dim_info " << dim_info << endl;
      cout << ".x. AssumedSize lower_bound " << lower_bound << endl;
      cout << ".x. AssumedSize upper_bound " << upper_bound << endl;
      cout << ".x. AssumedSize      stride " << stride << endl;
#endif
   } else return ATfalse;

   ROSE_ASSERT(dim_info != NULL);
   rank = dim_info->get_expressions().size();

// TODO: The array-type builder should probably be based on the declared type
   *array_type = UntypedBuilder::buildArrayType(base_type->get_type_enum_id(),dim_info,rank);

   return ATtrue;
}

//========================================================================================
// R522 assumed-spec or implied-spec
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_AssumedOrImpliedSpec(ATerm term, SgUntypedExpression** lower_bound)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_AssumedOrImpliedSpec: %s\n", ATwriteToString(term));
#endif

   ATerm t_lower_bound;

   *lower_bound = NULL;

   if (ATmatch(term, "AssumedOrImpliedSpec(<term>)", &t_lower_bound)) {
      if (ATmatch(t_lower_bound, "no-lower-bound()")) {
         *lower_bound = UntypedBuilder::buildUntypedNullExpression();
         setSourcePositionUnknown(*lower_bound);
      }
      else if (traverse_Expression(t_lower_bound, lower_bound)) {
      } else return ATfalse;
      
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// R545 dimension-stmt
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_DimensionStmt(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_DimensionStmt: %s\n", ATwriteToString(term));
#endif

   ATerm t_label, t_spec_list, t_eos;
   std::string label;
   std::string eos;
   SgUntypedInitializedNameList* var_name_list = NULL;

   SgUntypedInitializedNameListDeclaration* dimension_decl = NULL;

   if (ATmatch(term, "DimensionStmt(<term>,<term>,<term>)", &t_label,&t_spec_list,&t_eos)) {
      if (traverse_OptLabel(t_label, label)) {
         // MATCHED OptLabel
      } else return ATfalse;

      SgUntypedType* base_type = UntypedBuilder::buildType(SgUntypedType::e_implicit);
      ROSE_ASSERT(base_type != NULL);

      var_name_list = new SgUntypedInitializedNameList();
      setSourcePosition(var_name_list, t_spec_list);

      ATermList tail = (ATermList) ATmake("<term>", t_spec_list);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);

         if (traverse_ArrayNameSpec(head, base_type, var_name_list)) {
            // MATCHED ArrayNameSpec
         } else return ATfalse;
      }

   // The base type will have been replaced by an array type.
      delete base_type;

      if (traverse_eos(t_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;

      int stmt_enum = General_Language_Translation::e_fortran_dimension_stmt;
      dimension_decl = new SgUntypedInitializedNameListDeclaration(label, stmt_enum, var_name_list);
      setSourcePosition(dimension_decl, term);
   }
   else return ATfalse;

   decl_list->get_decl_list().push_back(dimension_decl);

   return ATtrue;
}

ATbool ATermToUntypedFortranTraversal::traverse_ArrayNameSpec(ATerm term, SgUntypedType* base_type, SgUntypedInitializedNameList* name_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ArrayNameSpec: %s\n", ATwriteToString(term));
#endif

   ATerm t_name, t_array_spec;
   std::string name;

   SgUntypedArrayType* array_type = NULL;
   SgUntypedInitializedName* initialized_name = NULL;

   if (ATmatch(term, "ArrayNameSpec(<term>,<term>)",&t_name,&t_array_spec)) {
      if (traverse_Name(t_name, name)) {
         // MATCHED ObjectName
      } else return ATfalse;

      if (traverse_ArraySpec(t_array_spec, base_type, &array_type)) {
         // MATCHED ArraySpec
         ROSE_ASSERT(array_type != NULL);
      } else return ATfalse;

   } else return ATfalse;

   initialized_name = new SgUntypedInitializedName(array_type, name);
   setSourcePosition(initialized_name, term);

   name_list->get_name_list().push_back(initialized_name);

   return ATtrue;
}

//========================================================================================
// R560 implicit-stmt
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_ImplicitStmt(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ImplicitStmt: %s\n", ATwriteToString(term));
#endif
  
   ATerm term1, term2, term_eos;
   std::string label;
   std::string eos;
   std::vector<FAST::ImplicitSpec> spec_list;
   ImplicitSpecMatch match_spec(this, &spec_list);

   SgUntypedImplicitDeclaration* implicit_decl = NULL;

   if (ATmatch(term, "ImplicitNoneStmt(<term>,<term>)", &term1,&term_eos)) {
      if (traverse_OptLabel(term1, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_eos(term_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;

      implicit_decl = new SgUntypedImplicitDeclaration(label);
      setSourcePosition(implicit_decl, term);
   }

   else if (ATmatch(term, "ImplicitStmt(<term>,<term>,<term>)", &term1,&term2,&term_eos)) {
      std::cerr << "...TODO... matched ImplicitStmt (not IMPLICIT NONE)" << std::endl;
      std::vector<FAST::ImplicitSpec> spec_list;
      if (traverse_OptLabel(term1, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_List(term2, match_spec)) {
         // MATCHED ImplicitSpecList
      } else return ATfalse;
      if (traverse_eos(term_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;

    //TODO_SgUntyped
      std::cerr << "...TODO... implement ImplicitStmt (not just IMPLICIT NONE)" << std::endl;
      return ATfalse;
   }
   else return ATfalse;

   decl_list->get_decl_list().push_back(implicit_decl);

   return ATtrue;
}

//========================================================================================
// R603 variable-name ("VarRef")
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_VarRef(ATerm term, SgUntypedExpression** var_expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_VarRef: %s\n", ATwriteToString(term));
#endif

   ATerm t_var;
   char* char_name;
   std::string name;

   SgToken::ROSE_Fortran_Keywords keyword = SgToken::FORTRAN_UNKNOWN;

   *var_expr = NULL;
   if (ATmatch(term, "VarRef(<term>)", &t_var)) {
      // MATCHED VarRef

      if (ATmatch(t_var, "<str>", &char_name)) {
         // MATCHED string
         name += char_name;
      } else return ATfalse;

      *var_expr = new SgUntypedReferenceExpression(keyword, name);
      setSourcePosition(*var_expr, term);
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// R611 data-ref
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_DataRef(ATerm term, SgUntypedExpression** var_expr)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_DataRef: %s\n", ATwriteToString(term));
#endif

  ATerm term1;

  *var_expr = NULL;
  if (ATmatch(term, "DataRef(<term>)", &term1)) {
     // MATCHED DataRef
     ATermList tail = (ATermList) ATmake("<term>", term1);
     while (! ATisEmpty(tail)) {
        ATerm head = ATgetFirst(tail);
        tail = ATgetNext(tail);
        if (traverse_PartRef(head, var_expr)) {
           // MATCHED ImportStmt
        } else return ATfalse;
     }
  }
  else return ATfalse;

  return ATtrue;
}

//========================================================================================
// R612 part-ref
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_PartRef(ATerm term, SgUntypedExpression** var_expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_PartRef: %s\n", ATwriteToString(term));
#endif

   ATerm t_name, t_subscripts, t_image_selector;
   char* arg1;

   std::string name;
   bool isArray = false;
   SgUntypedExprListExpression* subscripts = NULL;
   SgUntypedExprListExpression* image_selector = NULL;

   *var_expr = NULL;
   if (ATmatch(term, "PartRef(<term>,<term>,<term>)", &t_name, &t_subscripts, &t_image_selector)) {
      if (ATmatch(t_name, "<str>", &arg1)) {
         // MATCHED string
         name += arg1;
      } else return ATfalse;
      if (traverse_OptSectionSubscripts(t_subscripts, &subscripts)) {
         // MATCHED OptSectionSubscripts
         if (subscripts) isArray = true;
      } else return ATfalse;
      if (traverse_OptImageSelector(t_image_selector, &image_selector)) {
         // MATCHED OptImageSelector
         if (image_selector) isArray = true;
      } else return ATfalse;

      if (isArray) {
         int expr_enum = General_Language_Translation::e_array_reference;
         SgUntypedExpression*   array_subscripts = subscripts;
         SgUntypedExpression* coarray_subscripts = image_selector;
         if (  array_subscripts == NULL)   array_subscripts = UntypedBuilder::buildUntypedNullExpression();
         if (coarray_subscripts == NULL) coarray_subscripts = UntypedBuilder::buildUntypedNullExpression();
         ROSE_ASSERT(  array_subscripts);
         ROSE_ASSERT(coarray_subscripts);
         *var_expr = new SgUntypedArrayReferenceExpression(expr_enum, name, array_subscripts, coarray_subscripts);
      }
      else {
         int expr_enum = General_Language_Translation::e_variable_reference;
         *var_expr = new SgUntypedReferenceExpression(expr_enum, name);
      }
   }
   else return ATfalse;

   ROSE_ASSERT(*var_expr);
   setSourcePosition(*var_expr, term);

   return ATtrue;
}

//========================================================================================
// R620 section-subscript
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_SectionSubscript(ATerm term, SgUntypedExpression** subscript)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SectionSubscript: %s\n", ATwriteToString(term));
#endif

   *subscript = NULL;

   if (traverse_Expression(term, subscript)) {
      // MATCHED Subscript
   }
   else if (traverse_Triplet(term, subscript)) {
      // MATCHED Triplet (SubscriptTriplet)
   }

   // TODO: Keyword '=' ActualArg                                  -> SectionSubscript     {cons("ActualArgSpec")}
   // TODO: AltReturnSpec                                          -> SectionSubscript

   else {
      std::cerr << "...TODO... implement SectionSubscript" << std::endl;
      return ATfalse;
   }

   ROSE_ASSERT(*subscript);

   return ATtrue;
}

ATbool ATermToUntypedFortranTraversal::traverse_OptSectionSubscripts(ATerm term, SgUntypedExprListExpression** subscripts)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptSectionSubscripts: %s\n", ATwriteToString(term));
#endif

   ATerm t_subscript_list;
   SgUntypedExpression* subscript;
   SgUntypedExprListExpression* section_subscripts = NULL;

   *subscripts = NULL;

   if (ATmatch(term, "no-section-subscripts()")) {
      // MATCHED empty list
   }

   // list of section subscripts
   else if (ATmatch(term, "paren-list(<term>)", &t_subscript_list)) {
      section_subscripts = new SgUntypedExprListExpression(General_Language_Translation::e_section_subscripts);
      ROSE_ASSERT(section_subscripts);
      setSourcePosition(section_subscripts, term);

      ATermList tail = (ATermList) ATmake("<term>", t_subscript_list);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);

         if (traverse_SectionSubscript(head, &subscript)) {
            // MATCHED SectionSubscript
         } else return ATfalse;

         ROSE_ASSERT(subscript);
         section_subscripts->get_expressions().push_back(subscript);
      }
   }

   //TODO_SgUntyped - (substring-section-range)
   //TODO_SgUntyped - (function-ref-no-args)
   //TODO_SgUntyped - needs section subscripts
   else {
      std::cerr << "...TODO... implement OptSectionSubscripts" << std::endl;
      return ATfalse;
   }

// nullptr is ok for "no-section-subscripts"
   *subscripts = section_subscripts;

   return ATtrue;
}

//========================================================================================
// R621 subscript-triplet
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_Triplet(ATerm term, SgUntypedExpression** range)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Triplet: %s\n", ATwriteToString(term));
#endif

   ATerm t_lower_bound, t_upper_bound, t_opt_stride, t_stride;

   SgUntypedExpression* lower_bound = NULL;
   SgUntypedExpression* upper_bound = NULL;
   SgUntypedExpression* stride = NULL;

   *range = NULL;

   if (ATmatch(term, "Triplet(<term>,<term>,<term>)", &t_lower_bound, &t_upper_bound, &t_opt_stride)) {

      // Lower bound
      if (ATmatch(t_lower_bound, "no-subscript()")) {
         lower_bound = UntypedBuilder::buildUntypedNullExpression();
         setSourcePositionUnknown(lower_bound);
      }
      else if (traverse_Expression(t_lower_bound, &lower_bound)) {
      } else return ATfalse;

      // Upper bound
      if (ATmatch(t_upper_bound, "no-subscript()")) {
         upper_bound = UntypedBuilder::buildUntypedNullExpression();
         setSourcePositionUnknown(upper_bound);
      }
      else if (traverse_Expression(t_upper_bound, &upper_bound)) {
      } else return ATfalse;

      // Stride
      if (ATmatch(t_opt_stride, "no-stride()")) {
         stride = UntypedBuilder::buildUntypedNullExpression();
         setSourcePositionUnknown(stride);
      }
      else if (ATmatch(t_opt_stride, "opt-stride(<term>)", &t_stride)) {
         if (traverse_Expression(t_stride, &stride)) {
            // MATCHED stride
         } else return ATfalse;
      }
      else return ATfalse;

      ROSE_ASSERT(lower_bound);
      ROSE_ASSERT(upper_bound);
      ROSE_ASSERT(stride);

      int expr_enum = General_Language_Translation::e_array_index_triplet;

      *range = new SgUntypedSubscriptExpression(expr_enum, lower_bound, upper_bound, stride);
      ROSE_ASSERT(*range);
      setSourcePosition(*range, term);

   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// R624 image-selector
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_OptImageSelector(ATerm term, SgUntypedExprListExpression** image_selector)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptImageSelector: %s\n", ATwriteToString(term));
#endif

   *image_selector = NULL;

   if (ATmatch(term, "no-image-selector()")) {
      // MATCHED empty list
   }
   //TODO_SgUntyped - needs image-selector (ImageSelector - CosubscriptList)
   //TODO_SgUntyped - (substring-section-range)
   else {
      std::cerr << "...TODO... implement OptImageSelector" << std::endl;
      return ATfalse;
   }

   return ATtrue;
}

//========================================================================================
// R722 expr
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_Expression(ATerm term, SgUntypedExpression** var_expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Expression: %s\n", ATwriteToString(term));
#endif

   *var_expr = NULL;
   if (traverse_LiteralConstant(term, var_expr)) {
      // MATCHED LiteralConstant
   }
   else if (traverse_Operator(term, var_expr)) {
      // SgUntypedBinaryOperator
   }
   else if (traverse_DataRef(term, var_expr)) {
      // MATCHED DataRef
   }
   else if (traverse_VarRef(term, var_expr)) {
      // MATCHED VarRef
   }
   else if (ATmatch(term, "STAR()")) {
      // at times, '*' is used as a stand-in for an expression, e.g., dimension(1,*)
      *var_expr = new SgUntypedOtherExpression(General_Language_Translation::e_star_expression);
      ROSE_ASSERT(*var_expr);
      setSourcePositionUnknown(*var_expr);
   }

  else return ATfalse;

  return ATtrue;
}

ATbool ATermToUntypedFortranTraversal::traverse_OptExpr( ATerm term, SgUntypedExpression** expr )
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_OptExpr: %s\n", ATwriteToString(term));
#endif

//TODO - there is an OptExpr term cons "opt-expr"
//     - maybe this shouldn't be shared, determine for LoopControl implementation

   if (ATmatch(term, "no-expr()")) {
      // No Expression
      *expr = UntypedBuilder::buildUntypedNullExpression();
      setSourcePositionUnknown(*expr);
   }
   else if (traverse_Expression(term, expr)) {
      // MATCHED an Expression
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// R732 assignment-statement
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_AssignmentStmt(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_AssignmentStmt: %s\n", ATwriteToString(term));
#endif
  
   ATerm term1, term2, term3, term_eos;
   std::string label;
   std::string eos;
   SgUntypedExpression* variable;
   SgUntypedExpression* expr;

   if (ATmatch(term, "AssignmentStmt(<term>,<term>,<term>,<term>)", &term1,&term2,&term3,&term_eos)) {
      if (traverse_OptLabel(term1, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_Expression(term2, &variable)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_Expression(term3, &expr)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_eos(term_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

   SgUntypedAssignmentStatement* assign_stmt = new SgUntypedAssignmentStatement(label,variable,expr);
   setSourcePosition(assign_stmt, term);

   stmt_list->get_stmt_list().push_back(assign_stmt);

   return ATtrue;
}

//========================================================================================
// R801 block
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_Block(ATerm term, SgUntypedBlockStatement** block_stmt)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Block: %s\n", ATwriteToString(term));
#endif

   ATerm t_block;
   SgUntypedStatementList* stmt_list;

   if (ATmatch(term, "Block(<term>)", &t_block)) {
      // MATCHED Block

      *block_stmt = SageBuilder::buildUntypedBlockStatement("");
      stmt_list = (*block_stmt)->get_scope()->get_statement_list();

      ATermList tail = (ATermList) ATmake("<term>", t_block);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_BlockExecutionPartConstruct(head, stmt_list)) {
            // MATCHED stmt in block list
         } else return ATfalse;
      }
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// R817 nonlabel-do-stmt
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_NonlabelDoStmt(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_NonlabelDoStmt: %s\n", ATwriteToString(term));
#endif

   ATerm t_label, t_name, t_loop_ctrl, t_eos;
   std::string label;
   std::string do_construct_name;
   std::string eos;

// Variables for do loop
   SgUntypedExpression* initialization = NULL;
   SgUntypedExpression* upper_bound = NULL;
   SgUntypedExpression* increment = NULL;

// Variables for do concurrent
   SgUntypedType* concur_type = NULL;
   SgUntypedExpression*  mask = NULL;
   SgUntypedExprListExpression* iterates = NULL;
   SgUntypedExprListExpression* locality = NULL;

   SgUntypedStatement* loop_stmt = NULL;

   bool isConcurrent = false;

   if (ATmatch(term, "NonlabelDoStmt(<term>,<term>,<term>,<term>)", &t_label,&t_name,&t_loop_ctrl,&t_eos)) {
      if (traverse_OptLabel(t_label, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_OptName(t_name, do_construct_name)) {
         // MATCHED OptName
      } else return ATfalse;

   // Regular do loops are checked first
   //   - first if statement handles the optional part (no-loop-control)
      if (traverse_OptLoopControl(t_loop_ctrl, &initialization, &upper_bound, &increment)) {
         // MATCHED OptLoopControl
      }
   // DO CONCURRENT
      else if (traverse_LoopConcurrentControl(t_loop_ctrl, concur_type, &iterates, &locality, &mask)) {
         // This is a DO CONCURRENT construct
         isConcurrent = true;
      }
      else return ATfalse;

      if (traverse_eos(t_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

// The do body is not used currently in the grammar, it must be found during conversion to Sage nodes.
   SgUntypedStatement * body = NULL;

   if (isConcurrent) {
      int stmt_enum = General_Language_Translation::e_fortran_do_concurrent_stmt;
      SgUntypedForAllStatement* forall_stmt = new SgUntypedForAllStatement("",stmt_enum,concur_type,iterates,locality,mask,do_construct_name);
      ROSE_ASSERT(forall_stmt);
      setSourcePosition(forall_stmt, term);

      loop_stmt = forall_stmt;
   }
   else {
      int stmt_enum = Fortran_ROSE_Translation::e_do_stmt;
      SgUntypedForStatement* for_stmt = new SgUntypedForStatement("",stmt_enum,initialization,upper_bound,increment,body,do_construct_name);
      ROSE_ASSERT(for_stmt);
      setSourcePosition(for_stmt, term);

      loop_stmt = for_stmt;
   }

   ROSE_ASSERT(loop_stmt);
   stmt_list->get_stmt_list().push_back(loop_stmt);

   return ATtrue;
}

//========================================================================================
// R818 loop-control
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_OptLoopControl(ATerm term, SgUntypedExpression** initialization,
                                                                           SgUntypedExpression** upper_bound, SgUntypedExpression** incr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptLoopControl: %s\n", ATwriteToString(term));
#endif

   ATerm t_init_var, t_lbound, t_ubound, t_incr;

   SgUntypedExpression* init_var = NULL;
   SgUntypedExpression* lower_bound = NULL;

   *initialization = NULL;
   *upper_bound = NULL;
   *incr = NULL;

   if (ATmatch(term, "no-loop-control()")) {
      // MATCHED no-loop-control
      return ATtrue;
   }

// At this point there shall be a loop-control
   if (ATmatch(term, "LoopControl(<term>,<term>,<term>,<term>)", &t_init_var, &t_lbound, &t_ubound, &t_incr)) {

      if (traverse_Expression(t_init_var, &init_var)) {
         // MATCHED Expression
      } else return ATfalse;
      if (traverse_Expression(t_lbound, &lower_bound)) {
         // MATCHED Expression
      } else return ATfalse;
      if (traverse_OptExpr(t_ubound, upper_bound)) {
         // MATCHED Expression
      } else return ATfalse;
      if (traverse_OptExpr(t_incr, incr)) {
         // MATCHED OptExpr
      } else return ATfalse;
   }
   else return ATfalse;

   ROSE_ASSERT(init_var);
   ROSE_ASSERT(lower_bound);
   ROSE_ASSERT(*upper_bound);
   ROSE_ASSERT(*incr);

   int op_enum = General_Language_Translation::e_operator_assign;
   *initialization = new SgUntypedBinaryOperator(op_enum, "=", init_var, lower_bound);
   ROSE_ASSERT(*initialization);
   setSourcePositionIncludingTerm(*initialization, t_init_var, t_lbound);

   return ATtrue;
}

//========================================================================================
// R822 end-do-stmt
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_EndDoStmt(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_EndDoStmt: %s\n", ATwriteToString(term));
#endif

   ATerm t_label, t_name, t_eos;
   std::string label;
   std::string do_construct_name;
   std::string eos;

   int stmt_enum = General_Language_Translation::e_fortran_end_do_stmt;

   if (ATmatch(term, "EndDoStmt(<term>,<term>,<term>)", &t_label,&t_name,&t_eos)) {
      if (traverse_OptLabel(t_label, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_OptName(t_name, do_construct_name)) {
         // MATCHED OptName
      } else return ATfalse;
      if (traverse_eos(t_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

   SgUntypedNamedStatement* end_do_stmt = new SgUntypedNamedStatement(label, stmt_enum, do_construct_name);
   setSourcePosition(end_do_stmt, term);

   stmt_list->get_stmt_list().push_back(end_do_stmt);

   return ATtrue;
}

//========================================================================================
// R832 if-construct
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_IfConstruct(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_IfConstruct: %s\n", ATwriteToString(term));
#endif

   ATerm t_ifthen, t_block, t_elseif, t_else, t_endif;
   SgUntypedExpressionStatement* if_then_stmt;
   SgUntypedStatement* else_stmt;
   SgUntypedBlockStatement *if_block, *else_block;
   SgUntypedIfStatement *first_else_if, *last_else_if;
   SgUntypedStatement *true_body, *false_body;

   if (ATmatch(term, "IfConstruct(<term>,<term>,<term>,<term>,<term>)", &t_ifthen,&t_block,&t_elseif,&t_else,&t_endif)) {
      if (traverse_IfThenStmt(t_ifthen, &if_then_stmt)) {
         // MATCHED IfThenStmt
      } else return ATfalse;

      if (traverse_Block(t_block, &if_block)) {
         // MATCHED Block
      } else return ATfalse;

      if (traverse_ElseIfStmtList(t_elseif, &first_else_if, &last_else_if)) {
         // MATCHED ElseIfStmtList; first_else_if and last_else_if will be NULL if list is empty
      } else return ATfalse;

      if (traverse_OptElseStmtAndBlock(t_else, &else_stmt, &else_block)) {
         // TODO - need to retain else-stmt label (not sure you can branch to it however)
         // else_label = else_stmt->get_label_string();
      } else return ATfalse;
   }
   else return ATfalse;

   SgUntypedExpression* conditional = if_then_stmt->get_statement_expression();
   ROSE_ASSERT(conditional);

#if 0
// keep until labels are finished
   std::cout << "--- traverseIfConstruct first_else_if " << first_else_if << std::endl;
   std::cout << "--- traverseIfConstruct last_else_if " << last_else_if << std::endl;
   std::cout << "--- traverseIfConstruct else_block " << else_block << std::endl;
#endif

   if (first_else_if != NULL) {
      false_body = first_else_if;
      ROSE_ASSERT(last_else_if != NULL);
      last_else_if->set_false_body(else_block);
   }
   else {
      false_body = else_block;
   }
   true_body = if_block;

#if 0
   std::cout << "--- traverseIfConstruct  true_body " <<  true_body << std::endl;
   std::cout << "--- traverseIfConstruct false_body " << false_body << std::endl;
#endif

   // TODO - create begin and end statements for SgUntypedBlockStatement

   std::string if_construct_name = if_then_stmt->get_label_string();

   SgUntypedIfStatement* if_stmt = SageBuilder::buildUntypedIfStatement(if_construct_name,conditional,true_body,false_body);
   setSourcePosition(if_stmt, term);

   stmt_list->get_stmt_list().push_back(if_stmt);

// No longer needed (replaced by contents of if_stmt)
   if (if_then_stmt) delete if_then_stmt;
   if (else_stmt)    delete else_stmt;

   return ATtrue;
}

//========================================================================================
// R833 if-then-stmt
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_IfThenStmt(ATerm term, SgUntypedExpressionStatement** if_then_stmt)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_IfThenStmt: %s\n", ATwriteToString(term));
#endif

   ATerm t_label, t_name, t_expr, t_eos;
   std::string label;
   std::string if_construct_name;
   std::string eos;
   SgUntypedExpression* conditional;
   int stmt_enum = General_Language_Translation::e_fortran_if_then_stmt;

   *if_then_stmt = NULL;

   if (ATmatch(term, "IfThenStmt(<term>,<term>,<term>,<term>)", &t_label,&t_name,&t_expr,&t_eos)) {
      if (traverse_OptLabel(t_label, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_OptName(t_name, if_construct_name)) {
         // MATCHED OptName
      } else return ATfalse;
      if (traverse_Expression(t_expr, &conditional)) {
         // MATCHED Expression
      } else return ATfalse;
      if (traverse_eos(t_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

   // Generic SgUntypedExpressionStatement used for Fortran if-then-stmt
   //   1. Note that the if-construct-name is used for the label_name slot
   //   2. TODO - if there is an actual label statement, use SgUntypedLabelStatement as container
   *if_then_stmt = new SgUntypedExpressionStatement(if_construct_name, stmt_enum, conditional);
   setSourcePosition(*if_then_stmt, term);

   return ATtrue;
}

//========================================================================================
// R834 else-if-stmt
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_ElseIfStmt(ATerm term, SgUntypedExpressionStatement** else_if_stmt)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ElseIfStmt: %s\n", ATwriteToString(term));
#endif

   ATerm t_label, t_expr, t_name, t_eos;
   std::string label;
   std::string if_construct_name;
   std::string eos;
   SgUntypedExpression* conditional;
   int stmt_enum = General_Language_Translation::e_fortran_else_if_stmt;

   *else_if_stmt = NULL;

   if (ATmatch(term, "ElseIfStmt(<term>,<term>,<term>,<term>)", &t_label,&t_expr,&t_name,&t_eos)) {
      if (traverse_OptLabel(t_label, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_Expression(t_expr, &conditional)) {
         // MATCHED Expression
      } else return ATfalse;
      if (traverse_OptName(t_name, if_construct_name)) {
         // MATCHED OptName
      } else return ATfalse;
      if (traverse_eos(t_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

   // Generic SgUntypedExpressionStatement used for Fortran else-if-stmt
   //   1. Note that the if-construct-name is used for the label_name slot
   //   2. TODO - if there is an actual label statement, use SgUntypedLabelStatement as container
   *else_if_stmt = new SgUntypedExpressionStatement(if_construct_name, stmt_enum, conditional);
   setSourcePosition(*else_if_stmt, term);

   return ATtrue;
}

ATbool ATermToUntypedFortranTraversal::traverse_ElseIfStmtList(ATerm term, SgUntypedIfStatement** if_stmt,
                                                                           SgUntypedIfStatement** last_if_stmt)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ElseIfStmtList: %s\n", ATwriteToString(term));
#endif

   ATerm t_elseif_list;

   SgUntypedIfStatement* previous_if_stmt = NULL;
   SgUntypedIfStatement*  current_if_stmt = NULL;

   *if_stmt = NULL;

   if (ATmatch(term, "ElseIfStmtList(<term>)", &t_elseif_list)) {

      ATermList tail = (ATermList) ATmake("<term>", t_elseif_list);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);

         if (traverse_ElseIfStmtBlock(head, &current_if_stmt)) {
            // This if statement will have a NULL false body
         } else return ATfalse;

#if 0
         std::cout << "-w- elseiflist: previous_if_stmt = " << previous_if_stmt << previous_if_stmt->class_name() << std::endl;
         std::cout << "-w- elseiflist:  current_if_stmt = " <<  current_if_stmt <<  current_if_stmt->class_name() << std::endl;
#endif

         if (previous_if_stmt != NULL) {
            previous_if_stmt->set_false_body(current_if_stmt);
         }
         previous_if_stmt = current_if_stmt;

         if (*if_stmt == NULL) {
            *if_stmt = current_if_stmt;
            // source position has already been set, check to see if it needs refinement
            // setSourcePosition(*if_stmt, term);
         }
#if 0
         std::cout << "-w- elseiflist:          if_stmt = " <<         *if_stmt << std::endl;
#endif
      }
   }
   else return ATfalse;

   // The false body of the last else if statement is needed for the else statement
   *last_if_stmt = current_if_stmt;

   return ATtrue;
}

ATbool ATermToUntypedFortranTraversal::traverse_ElseIfStmtBlock(ATerm term, SgUntypedIfStatement** if_stmt)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ElseIfStmtBlock: %s\n", ATwriteToString(term));
#endif

   ATerm t_else_if, t_block;
   SgUntypedExpressionStatement* else_if_stmt;
   SgUntypedBlockStatement* true_body;
   SgUntypedBlockStatement* false_body;

   *if_stmt = NULL;

   if (ATmatch(term, "ElseIfStmtBlock(<term>,<term>)", &t_else_if,&t_block)) {

      if (traverse_ElseIfStmt(t_else_if, &else_if_stmt)) {
         // MATCHED ElseIfStmt
      } else return ATfalse;

      if (traverse_Block(t_block, &true_body)) {
         // MATCHED Block
      } else return ATfalse;
   }
   else return ATfalse;

   // The false body of the (else) if statement is NULL, unless
   // there is an else-stmt, in which case if becomes the false body
   false_body = NULL;

   SgUntypedExpression* conditional = else_if_stmt->get_statement_expression();
   ROSE_ASSERT(conditional);

   std::string if_construct_name = else_if_stmt->get_label_string();

   *if_stmt = SageBuilder::buildUntypedIfStatement(if_construct_name,conditional,true_body,false_body);
   setSourcePosition(*if_stmt, term);

#if 0
   std::cout << "--- traverseElseIf  true_body " <<  true_body << std::endl;
   std::cout << "--- traverseElseIf false_body " << false_body << std::endl;
   std::cout << "--- traverseElseIf else_if_stmt " << else_if_stmt << std::endl;
   std::cout << "--- traverseElseIf      if_stmt " << *if_stmt << std::endl;
#endif

   if (else_if_stmt) delete else_if_stmt;

   return ATtrue;
}

//========================================================================================
// R835 else-stmt
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_ElseStmt(ATerm term, SgUntypedStatement** else_stmt)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ElseStmt: %s\n", ATwriteToString(term));
#endif

   ATerm t_label, t_name, t_eos;
   std::string label;
   std::string if_construct_name;
   std::string eos;
   int stmt_enum = General_Language_Translation::e_fortran_else_stmt;

   *else_stmt = NULL;

   if (ATmatch(term, "ElseStmt(<term>,<term>,<term>)", &t_label,&t_name,&t_eos)) {
      if (traverse_OptLabel(t_label, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_OptName(t_name, if_construct_name)) {
         // MATCHED OptName
      } else return ATfalse;
      if (traverse_eos(t_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

   // Generic SgUntypedExpressionStatement used for Fortran else-stmt
   //   1. Note that the if-construct-name cannot be used in the current Sage IR node SgIfStmt
   *else_stmt = new SgUntypedOtherStatement(label, stmt_enum);

   setSourcePosition(*else_stmt, term);

   return ATtrue;
}

ATbool ATermToUntypedFortranTraversal::traverse_OptElseStmtAndBlock(ATerm term, SgUntypedStatement** else_stmt,
                                                                                SgUntypedBlockStatement** false_body)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptElseStmtAndBlock: %s\n", ATwriteToString(term));
#endif

   ATerm t_else, t_block;

   *else_stmt = NULL;
   *false_body = NULL;

   if (ATmatch(term, "no-else-stmt()")) {
      return ATtrue;
   }

   if (ATmatch(term, "ElseStmtBlock(<term>,<term>)", &t_else, &t_block)) {

      if (traverse_ElseStmt(t_else, else_stmt)) {
         // MATCHED ElseStmt
      } else return ATfalse;

      if (traverse_Block(t_block, false_body)) {
         // MATCHED Block
      } else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// R837 if-stmt
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_IfStmt(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_IfStmt: %s\n", ATwriteToString(term));
#endif

   ATerm t_label, t_expr, t_action_stmt;
   std::string label;
   SgUntypedExpression* conditional;
   SgUntypedIfStatement* if_stmt;
   SgUntypedStatement *action_stmt;
   SgUntypedBlockStatement *true_body, *false_body;

   if (ATmatch(term, "IfStmt(<term>,<term>,<term>)", &t_label,&t_expr,&t_action_stmt)) {
      if (traverse_OptLabel(t_label, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_Expression(t_expr, &conditional)) {
         // MATCHED Expression
      } else return ATfalse;
      if (traverse_ExecStmt(t_action_stmt, stmt_list)) {
         // MATCHED ExecStmt
      } else return ATfalse;
   }
   else return ATfalse;

   // The action statement has been added to stmt_list, it needs to be retrieved and removed
   action_stmt = stmt_list->get_stmt_list().back();
                 stmt_list->get_stmt_list().pop_back();

   true_body = SageBuilder::buildUntypedBlockStatement("");
   true_body->get_scope()->get_statement_list()->get_stmt_list().push_back(action_stmt);

   false_body = NULL;

// TODO - label statement
// if_stmt = SageBuilder::buildUntypedIfStatement(label,conditional,true_body,false_body);
   if_stmt = SageBuilder::buildUntypedIfStatement(""   ,conditional,true_body,false_body);
   setSourcePosition(if_stmt, term);

// Specify that this is an if-stmt and not if-construct/if-then-else...
   if_stmt->set_statement_enum(General_Language_Translation::e_fortran_if_stmt);

   stmt_list->get_stmt_list().push_back(if_stmt);

   return ATtrue;
}

//========================================================================================
// R838 case-construct
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_CaseConstruct(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CaseConstruct: %s\n", ATwriteToString(term));
#endif

   ATerm t_case_stmt, t_case_list, t_end_case_stmt;
   SgUntypedCaseStatement* select_case_stmt;
   SgUntypedBlockStatement* case_body;
   SgUntypedNamedStatement* end_select_stmt;

   if (ATmatch(term, "CaseConstruct(<term>,<term>,<term>)", &t_case_stmt,&t_case_list,&t_end_case_stmt)) {
      if (traverse_SelectCaseStmt(t_case_stmt, &select_case_stmt)) {
         // MATCHED OptLabel
      } else return ATfalse;
      ROSE_ASSERT(select_case_stmt != NULL);

      case_body = SageBuilder::buildUntypedBlockStatement("");
      ROSE_ASSERT(case_body != NULL);

      select_case_stmt->set_body(case_body);

      SgUntypedStatementList* my_stmt_list = case_body->get_scope()->get_statement_list();

      ATermList tail = (ATermList) ATmake("<term>", t_case_list);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);

         if (traverse_CaseStmtAndBlock(head, my_stmt_list)) {
            // MATCHED CaseStmtAndBlock
         } else return ATfalse;
      }

      if (traverse_EndSelectStmt(t_end_case_stmt, &end_select_stmt)) {
         // MATCHED EndSelectStmt
      } else return ATfalse;

      my_stmt_list->get_stmt_list().push_back(end_select_stmt);
   }
   else return ATfalse;

   stmt_list->get_stmt_list().push_back(select_case_stmt);

   return ATtrue;
}

ATbool ATermToUntypedFortranTraversal::traverse_CaseStmtAndBlock(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CaseStmtAndBlock: %s\n", ATwriteToString(term));
#endif

   ATerm t_case_stmt, t_block;
   SgUntypedCaseStatement* case_stmt;
   SgUntypedBlockStatement* block;

   // NOTE constructor name doesn't match the function name (cons name could be changed but not necessary).
   if (ATmatch(term, "CaseStmtBlock(<term>,<term>)", &t_case_stmt,&t_block)) {
      if (traverse_CaseStmt(t_case_stmt, &case_stmt)) {
         // MATCHED CaseStmt
      } else return ATfalse;
      if (traverse_Block(t_block, &block)) {
         // MATCHED Block
      } else return ATfalse;
   }
   else return ATfalse;

   ROSE_ASSERT(case_stmt);
   ROSE_ASSERT(block);

   case_stmt->set_body(block);
   stmt_list->get_stmt_list().push_back(case_stmt);

   return ATtrue;
}

//========================================================================================
// R839 select-case-stmt
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_SelectCaseStmt(ATerm term, SgUntypedCaseStatement** case_stmt)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SelectCaseStmt: %s\n", ATwriteToString(term));
#endif

   ATerm t_label, t_name, t_expr, t_eos;
   std::string label, case_name, eos;
   SgUntypedExpression* case_expr;
   SgUntypedCaseStatement* select_case_stmt;

   int stmt_enum = General_Language_Translation::e_switch_stmt;

   if (ATmatch(term, "SelectCaseStmt(<term>,<term>,<term>,<term>)", &t_label,&t_name,&t_expr,&t_eos)) {
      if (traverse_OptLabel(t_label, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_OptName(t_name, case_name)) {
         // MATCHED OptName
      } else return ATfalse;
      if (traverse_Expression(t_expr, &case_expr)) {
         // MATCHED Expression
      } else return ATfalse;
      if (traverse_eos(t_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

   SgUntypedBlockStatement* body = NULL;

   select_case_stmt = new SgUntypedCaseStatement(label, stmt_enum, case_expr, body, case_name, false);
   ROSE_ASSERT(select_case_stmt);
   setSourcePosition(select_case_stmt, term);

   *case_stmt = select_case_stmt;

   return ATtrue;
}

//========================================================================================
// R840 case-stmt
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_CaseStmt(ATerm term, SgUntypedCaseStatement** case_stmt)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CaseStmt: %s\n", ATwriteToString(term));
#endif

   ATerm t_label, t_selector, t_name, t_eos;
   std::string label, case_name, eos;
   int stmt_enum;
   SgUntypedExprListExpression* selector = NULL;

   if (ATmatch(term, "CaseStmt(<term>,<term>,<term>,<term>)", &t_label,&t_selector,&t_name,&t_eos)) {
      if (traverse_OptLabel(t_label, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_CaseSelector(t_selector, &selector)) {
         // MATCHED Expression
      } else return ATfalse;
      if (traverse_OptName(t_name, case_name)) {
         // MATCHED OptName
      } else return ATfalse;
      if (traverse_eos(t_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

   SgUntypedBlockStatement* body = NULL;

   if (selector) stmt_enum = General_Language_Translation::e_case_option_stmt;
   else          stmt_enum = General_Language_Translation::e_case_default_option_stmt;

   *case_stmt = new SgUntypedCaseStatement(label, stmt_enum, selector, body, case_name, false);
   ROSE_ASSERT(*case_stmt);
   setSourcePosition(*case_stmt, term);

   return ATtrue;
}

//========================================================================================
// R841 end-select-stmt
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_EndSelectStmt(ATerm term, SgUntypedNamedStatement** end_select_stmt)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_EndSelectStmt: %s\n", ATwriteToString(term));
#endif

   ATerm t_label, t_name, t_eos;
   std::string label, construct_name, eos;

   int stmt_enum = General_Language_Translation::e_end_switch_stmt;

   if (ATmatch(term, "EndSelectStmt(<term>,<term>,<term>)", &t_label,&t_name,&t_eos)) {
      if (traverse_OptLabel(t_label, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_OptName(t_name, construct_name)) {
         // MATCHED OptName
      } else return ATfalse;
      if (traverse_eos(t_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

   *end_select_stmt = new SgUntypedNamedStatement(label,stmt_enum,construct_name);

   ROSE_ASSERT(*end_select_stmt);
   setSourcePosition(*end_select_stmt, term);

   return ATtrue;
}

//========================================================================================
// R843 case-selector
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_CaseSelector(ATerm term, SgUntypedExprListExpression** selector)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CaseSelector: %s\n", ATwriteToString(term));
#endif

   ATerm t_selector_list;
   SgUntypedExpression* case_value_range;
   SgUntypedExprListExpression* case_selector;

   *selector = NULL;

   if (ATmatch(term, "paren-list(<term>)", &t_selector_list)) {
      case_selector = new SgUntypedExprListExpression(General_Language_Translation::e_case_selector);
      ROSE_ASSERT(case_selector);
      setSourcePosition(case_selector, term);

      ATermList tail = (ATermList) ATmake("<term>", t_selector_list);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);

         if (traverse_CaseValueRange(head, &case_value_range)) {
            // MATCHED CaseStmtAndBlock
         } else return ATfalse;
         case_selector->get_expressions().push_back(case_value_range);
      }
   }
   else if (ATmatch(term, "DEFAULT()")) {
      // MATCHED DEFAULT case-selector
      case_selector = NULL;
   }
   else return ATfalse;

   // Ok to be NULL if DEFAULT selector
   *selector = case_selector;

   return ATtrue;
}

//========================================================================================
// R844 case-value-range
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_CaseValueRange(ATerm term, SgUntypedExpression** case_value_range)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_CaseValueRange: %s\n", ATwriteToString(term));
#endif

   ATerm t_expr1, t_expr2;

   SgUntypedSubscriptExpression* range = NULL;
   SgUntypedExpression* lower_bound = NULL;
   SgUntypedExpression* upper_bound = NULL;
   SgUntypedExpression* stride = NULL;
   SgUntypedExpression* value = NULL;

   *case_value_range = NULL;

   if (ATmatch(term, "CaseValue(<term>)", &t_expr1)) {
      if (traverse_Expression(t_expr1, &value)) {
         // MATCHED Expression
      } else return ATfalse;

   } else if (ATmatch(term, "CaseValueRange(<term>,no-high())", &t_expr1)) {
      if (traverse_Expression(t_expr1, &lower_bound)) {
         // MATCHED Expression
      } else return ATfalse;
      upper_bound = UntypedBuilder::buildUntypedNullExpression();
      setSourcePositionUnknown(upper_bound);

   } else if (ATmatch(term, "CaseValueRange(no-low(),<term>)", &t_expr2)) {
      if (traverse_Expression(t_expr2, &upper_bound)) {
         // MATCHED Expression
      } else return ATfalse;
      lower_bound = UntypedBuilder::buildUntypedNullExpression();
      setSourcePositionUnknown(lower_bound);

   } else if (ATmatch(term, "CaseValueRange(<term>,<term>)", &t_expr1,&t_expr2)) {
      if (traverse_Expression(t_expr1, &lower_bound)) {
         // MATCHED Expression
      } else return ATfalse;
      if (traverse_Expression(t_expr2, &upper_bound)) {
         // MATCHED Expression
      } else return ATfalse;
   }
   else return ATfalse;

   if (value) {
      *case_value_range = value;
   }
   else if (lower_bound && upper_bound) {
      int expr_enum = General_Language_Translation::e_case_range;
      stride = UntypedBuilder::buildUntypedNullExpression();
      setSourcePositionUnknown(stride);
      range = new SgUntypedSubscriptExpression(expr_enum, lower_bound, upper_bound, stride);
      setSourcePosition(range, term);
      *case_value_range = range;
   }
   else {
      ROSE_ASSERT(0);
   }
   ROSE_ASSERT(*case_value_range);

   return ATtrue;
}

//========================================================================================
// R851 goto-stmt
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_GotoStmt(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_GotoStmt: %s\n", ATwriteToString(term));
#endif

   ATerm t_label, t_target, t_eos;
   char* char_target;
   std::string label;
   std::string eos;

   if (ATmatch(term, "GotoStmt(<term>,<term>,<term>)", &t_label, &t_target, &t_eos)) {
      if (traverse_OptLabel(t_label, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (ATmatch(t_target, "<str>", &char_target)) {
         // MATCHED target label string
      } else return ATfalse;
      if (traverse_eos(t_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

   SgUntypedGotoStatement* goto_stmt = new SgUntypedGotoStatement(label, char_target);
   setSourcePosition(goto_stmt, term);

   stmt_list->get_stmt_list().push_back(goto_stmt);

   return ATtrue;
}

//========================================================================================
// R854 continue-stmt
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_ContinueStmt(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_ContinueStmt: %s\n", ATwriteToString(term));
#endif

   ATerm term1, term_eos;
   std::string label;
   std::string eos;

   SgToken::ROSE_Fortran_Keywords keyword = SgToken::FORTRAN_CONTINUE;

   if (ATmatch(term, "ContinueStmt(<term>,<term>)", &term1,&term_eos)) {
      if (traverse_OptLabel(term1, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_eos(term_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

   SgUntypedOtherStatement* continue_stmt = new SgUntypedOtherStatement(label, keyword);
   setSourcePosition(continue_stmt, term);

   stmt_list->get_stmt_list().push_back(continue_stmt);

   return ATtrue;
}

//========================================================================================
// R855 stop-stmt
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_StopStmt(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_StopStmt: %s\n", ATwriteToString(term));
#endif

   ATerm term1, term2, term_eos;
   std::string label;
   std::string eos;
   SgUntypedExpression* stop_code;

   if (ATmatch(term, "StopStmt(<term>,<term>,<term>)", &term1,&term2,&term_eos)) {
      if (traverse_OptLabel(term1, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_OptStopCode(term2, &stop_code)) {
         // MATCHED OptStopCode
      } else return ATfalse;
      if (traverse_eos(term_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

   SgUntypedStopStatement* stop_stmt = new SgUntypedStopStatement(label, stop_code);
   setSourcePositionExcludingTerm(stop_stmt, term, term_eos);

   stmt_list->get_stmt_list().push_back(stop_stmt);

   return ATtrue;
}

//========================================================================================
// R856 error-stop-stmt
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_ErrorStopStmt(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_ErrorStopStmt: %s\n", ATwriteToString(term));
#endif

   ATerm term1, term2, term_eos;
   std::string label;
   std::string eos;
   SgUntypedExpression* stop_code;

   SgToken::ROSE_Fortran_Keywords keyword = SgToken::FORTRAN_ERROR_STOP;

   if (ATmatch(term, "ErrorStopStmt(<term>,<term>,<term>)", &term1,&term2,&term_eos)) {
      if (traverse_OptLabel(term1, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_OptStopCode(term2, &stop_code)) {
         // MATCHED OptStopCode
      } else return ATfalse;
      if (traverse_eos(term_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

   SgUntypedExpressionStatement* error_stop_stmt = new SgUntypedExpressionStatement(label, keyword, stop_code);
   setSourcePositionExcludingTerm(error_stop_stmt, term, term_eos);

   stmt_list->get_stmt_list().push_back(error_stop_stmt);

   return ATtrue;
}

ATbool ATermToUntypedFortranTraversal::traverse_OptStopCode(ATerm term, SgUntypedExpression** stop_code)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_OptStopStmt: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-stop-code()")) {
      // No StopCode
      *stop_code = UntypedBuilder::buildUntypedNullExpression();
      setSourcePositionUnknown(*stop_code);
   }
   else if (traverse_Expression(term, stop_code)) {
      // MATCHED StopCode
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// forall-construct R1050-2018-N2146
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_ForallConstruct(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ForallConstruct: %s\n", ATwriteToString(term));
#endif

   ATerm t_forall_stmt, t_forall_body, t_end_forall_stmt;
   SgUntypedForAllStatement* forall_stmt;
   SgUntypedBlockStatement* forall_body;
   SgUntypedNamedStatement* end_forall_stmt;

   if (ATmatch(term, "ForallConstruct(<term>,<term>,<term>)", &t_forall_stmt,&t_forall_body,&t_end_forall_stmt)) {
      cout << "\n Forall ......................................\n";
      if (traverse_ForallConstructStmt(t_forall_stmt, &forall_stmt)) {
         // MATCHED ForallConstructStmt
      } else return ATfalse;
      ROSE_ASSERT(forall_stmt != NULL);

      forall_body = SageBuilder::buildUntypedBlockStatement("");
      ROSE_ASSERT(forall_body != NULL);

      SgUntypedStatementList* body_stmt_list = forall_body->get_scope()->get_statement_list();
      ROSE_ASSERT(body_stmt_list != NULL);

      cout << ".x.   Forall stmt_list: " << body_stmt_list << endl;
      stmt_list->get_stmt_list().push_back(forall_stmt);

      if (traverse_ForallBodyConstruct(t_forall_body, body_stmt_list)) {
         // MATCHED ForallBodyConstruct
      } else return ATfalse;

      cout << ".x.   body_stmt size is " << body_stmt_list->get_stmt_list().size() << endl;

      if (traverse_EndForallStmt(t_end_forall_stmt, &end_forall_stmt)) {
         // MATCHED EndForallStmt
      } else return ATfalse;
      ROSE_ASSERT(end_forall_stmt != NULL);
   }
   else return ATfalse;

   // Push all three statements on the statement list and sort out details later based on the statement enum
   stmt_list->get_stmt_list().push_back(forall_stmt);
   stmt_list->get_stmt_list().push_back(forall_body);
   stmt_list->get_stmt_list().push_back(end_forall_stmt);

   cout << "\n Forall finished ......................................\n\n";

   return ATtrue;
}

//========================================================================================
// forall-construct-stmt R1051-2018-N2146
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_ForallConstructStmt(ATerm term, SgUntypedForAllStatement** forall_stmt)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ForallConstructStmt: %s\n", ATwriteToString(term));
#endif

   ATerm t_label, t_name, t_header, t_eos;
   std::string label, forall_construct_name;
   std::string eos;

   SgUntypedType* forall_type = NULL;
   SgUntypedExpression*  mask = NULL;
   SgUntypedExprListExpression* iterates = NULL;
   SgUntypedExprListExpression* locality = NULL;

   int stmt_enum = General_Language_Translation::e_fortran_forall_stmt;

   *forall_stmt = NULL;

   if (ATmatch(term, "ForallConstructStmt(<term>,<term>,<term>,<term>)", &t_label,&t_name,&t_header,&t_eos)) {
      if (traverse_OptLabel(t_label, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_OptName(t_name, forall_construct_name)) {
         // MATCHED OptName
      } else return ATfalse;

      if (traverse_ConcurrentHeader(t_header, forall_type, &iterates, &mask)) {
         // MATCHED ConcurrentHeader
      } else return ATfalse;

      *forall_stmt = new SgUntypedForAllStatement("", stmt_enum, forall_type, iterates, locality, mask, forall_construct_name);
      ROSE_ASSERT(*forall_stmt);
      setSourcePosition(*forall_stmt, term);
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// forall-body-construct R1052-2018-N2146
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_ForallBodyConstruct(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ForallBodyConstruct: %s\n", ATwriteToString(term));
#endif

   ATermList tail = (ATermList) ATmake("<term>", term);
   while (! ATisEmpty(tail)) {
      ATerm head = ATgetFirst(tail);
      tail = ATgetNext(tail);

      cout << ".x. traversing a ForAllBodyConstruct --------------------\n";

      if (traverse_AssignmentStmt(head, stmt_list)) {
         // MATCHED AssignmentStmt
      }
      //  PointerAssignmentStmt                            -> ForallAssignmentStmt

      //  ForallAssignmentStmt                             -> ForallBodyConstruct
      //  WhereStmt                                        -> ForallBodyConstruct
      //  WhereConstruct                                   -> ForallBodyConstruct
      else if (traverse_ForallConstruct(head, stmt_list)) {
         // MATCHED ForallConstructStmt
      }
      //  ForallStmt                                       -> ForallBodyConstruct

      else return ATfalse;
      cout << ".x. traversed a ForAllBodyConstruct --------------------\n";
   }

   cout << ".x. finished traversed ForAllBodyConstruct --------------------\n\n";
   return ATtrue;
}

//========================================================================================
// end-forall-stmt R1052-2018-N2146
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_EndForallStmt(ATerm term, SgUntypedNamedStatement** end_forall_stmt)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_EndForallStmt: %s\n", ATwriteToString(term));
#endif

   ATerm t_label, t_name, t_eos;
   std::string label;
   std::string forall_construct_name;
   std::string eos;

   int stmt_enum = General_Language_Translation::e_fortran_end_forall_stmt;

   if (ATmatch(term, "EndForallStmt(<term>,<term>,<term>)", &t_label,&t_name,&t_eos)) {
      if (traverse_OptLabel(t_label, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_OptName(t_name, forall_construct_name)) {
         // MATCHED OptName
      } else return ATfalse;
      if (traverse_eos(t_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

   *end_forall_stmt = new SgUntypedNamedStatement(label, stmt_enum, forall_construct_name);
   ROSE_ASSERT(*end_forall_stmt);
   setSourcePosition(*end_forall_stmt, term);

   return ATtrue;
}

//========================================================================================
// forall-stmt R1055-2018-N2146
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_ForallStmt(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ForallStmt: %s\n", ATwriteToString(term));
#endif

   ATerm t_label, t_header, t_assign;
   std::string label;

   SgUntypedType* forall_type = NULL;
   SgUntypedExpression*  mask = NULL;
   SgUntypedExprListExpression* iterates = NULL;
   SgUntypedExprListExpression* locality = NULL;

   int stmt_enum = General_Language_Translation::e_fortran_forall_stmt;

   SgUntypedForAllStatement* forall_stmt = NULL;

   if (ATmatch(term, "ForallStmt(<term>,<term>,<term>)", &t_label, &t_header, &t_assign)) {
      if (traverse_OptLabel(t_label, label)) {
         // MATCHED OptLabel
      } else return ATfalse;

      if (traverse_ConcurrentHeader(t_header, forall_type, &iterates, &mask)) {
         // MATCHED ConcurrentHeader
      } else return ATfalse;

      forall_stmt = new SgUntypedForAllStatement(label, stmt_enum, forall_type, iterates, locality, mask, "");
      ROSE_ASSERT(forall_stmt);
      setSourcePosition(forall_stmt, term);

      // Push all three statements on the statement list and sort out details later based on the statement enum
      stmt_list->get_stmt_list().push_back(forall_stmt);

      // Traverse the AssignmentStmt last so statement list is ordered with forall-assignment-stmt as body
      if (traverse_AssignmentStmt(t_assign, stmt_list)) {
         // MATCHED AssignmentStmt
   cout << ".x. completed AssignmentStmt \n";
      }
      // else if PointerAssignmentStmt                            -> ForallAssignmentStmt
      else return ATfalse;
   }
   else return ATfalse;

   cout << ".x. completed ForallStmt \n";

   return ATtrue;
}


//========================================================================================
// MainProgram (R1101)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_MainProgram(ATerm term, SgUntypedScope* scope)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_MainProgram: %s\n", ATwriteToString(term));
#endif

   ATerm term1, term2, term3, term4, term5;
   std::string label, name;

   SgUntypedProgramHeaderDeclaration* main_program = NULL;;
   SgUntypedNamedStatement*     program_stmt = NULL;;
   SgUntypedOtherStatement*    contains_stmt = NULL;;
   SgUntypedNamedStatement* end_program_stmt = NULL;;
   SgUntypedFunctionScope * function_scope = NULL;;

// scope and parameter lists
   SgUntypedDeclarationStatementList* decl_list = NULL;
   SgUntypedStatementList*            stmt_list = NULL;
   SgUntypedFunctionDeclarationList*  func_list = NULL;
   SgUntypedInitializedNameList*     param_list = NULL;
   SgUntypedExprListExpression*     prefix_list = NULL;

   if (ATmatch(term, "MainProgram(<term>,<term>,<term>,<term>,<term>)", &term1,&term2,&term3,&term4,&term5))
   {
      if (traverse_OptProgramStmt(term1, &program_stmt)) {
         // ProgramStmt
      } else return ATfalse;

      decl_list   = new SgUntypedDeclarationStatementList();
      stmt_list   = new SgUntypedStatementList();
      func_list   = new SgUntypedFunctionDeclarationList();
      param_list  = new SgUntypedInitializedNameList();
      prefix_list = new SgUntypedExprListExpression();

      if (traverse_InitialSpecPart(term2, decl_list)) {
         // InitialSpecPart
      } else return ATfalse;
      if (traverse_SpecAndExecPart(term3, decl_list, stmt_list)) {
         // SpecAndExecPart
         // TODO - FIXME - list changes from decl to stmt someplace in traverse_SpecAndExecPart
         setSourcePositionIncludingTerm(decl_list, term2, term3);
         setSourcePosition(stmt_list, term3);
      } else return ATfalse;

      label = program_stmt->get_label_string();
      name  = program_stmt->get_statement_name();
      function_scope = new SgUntypedFunctionScope(label,decl_list,stmt_list,func_list);

      if (traverse_OptInternalSubprogramPart(term4, &contains_stmt, function_scope)) {
         if (contains_stmt != NULL)
            {
               stmt_list->get_stmt_list().push_back(contains_stmt);
            }
         setSourcePosition(func_list, term4);
      } else return ATfalse;
      if (traverse_EndProgramStmt(term5, &end_program_stmt)) {
         // EndProgramStmt
      } else return ATfalse;
   } else return ATfalse;

// create the program

   SgUntypedType* type = UntypedBuilder::buildType(SgUntypedType::e_void);
   main_program = new SgUntypedProgramHeaderDeclaration(label, name, param_list, type,
                                                        function_scope, prefix_list, end_program_stmt);
   setSourcePositionIncludingNode(main_program, term, end_program_stmt);

// add program to the global scope
   scope->get_function_list()->get_func_list().push_back(main_program);

// replaced by the program
   delete program_stmt;

   return ATtrue;
}

//========================================================================================
// R1102 program-stmt
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_OptProgramStmt(ATerm term, SgUntypedNamedStatement** program_stmt)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_OptProgramStmt: %s\n", ATwriteToString(term));
#endif
  
  ATerm term1, term2, term_eos;
  std::string label;
  std::string name;
  std::string eos;

  int stmt_enum = Fortran_ROSE_Translation::e_program_stmt;

  *program_stmt = NULL; 
  if (ATmatch(term, "ProgramStmt(<term>,<term>,<term>)", &term1,&term2,&term_eos)) {
    if (traverse_OptLabel(term1, label)) {
      // MATCHED OptLabel
    } else return ATfalse;
    if (traverse_Name(term2, name)) {
      // MATCHED ProgramName
    } else return ATfalse;
    if (traverse_eos(term_eos, eos)) {
      // MATCHED eos string
    } else return ATfalse;

    *program_stmt = new SgUntypedNamedStatement(label, stmt_enum, name);
    setSourcePositionExcludingTerm(*program_stmt, term, term_eos);
  }
  else if (ATmatch(term, "no-program-stmt()")) {
   // There is no program-stmt so mark the name as special so that ROSE
   // knows that the statement is implied/compiler generated.
      name += "rose_fortran_main";
      *program_stmt = new SgUntypedNamedStatement(label, stmt_enum, name);
      setSourcePosition(*program_stmt, term);
  } else return ATfalse;

  return ATtrue;
}

//========================================================================================
// EndProgramStmt (R1103)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_EndProgramStmt(ATerm term, SgUntypedNamedStatement** end_program_stmt)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_EndProgramStmt: %s\n", ATwriteToString(term));
#endif

   ATerm term1, term2, term_eos;
   std::string label;
   std::string name;
   std::string eos;

   int stmt_enum = Fortran_ROSE_Translation::e_end_program_stmt;

   *end_program_stmt = NULL;
   if (ATmatch(term, "EndProgramStmt(<term>,<term>,<term>)", &term1,&term2,&term_eos)) {
      if (traverse_OptLabel(term1, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_OptName(term2, name)) {
         // MATCHED ProgramName string
      } else return ATfalse;
      if (traverse_eos(term_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   } else return ATfalse;

   *end_program_stmt = new SgUntypedNamedStatement(label, stmt_enum, name);
   setSourcePositionExcludingTerm(*end_program_stmt, term, term_eos);

  return ATtrue;
}

//========================================================================================
// Module (R1104)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_Module(ATerm term, SgUntypedScope* scope)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Module: %s\n", ATwriteToString(term));
#endif

   ATerm term1, term2, term3, term4;
   std::string label, name;

   SgUntypedModuleDeclaration* module;
   SgUntypedNamedStatement* module_stmt;
   SgUntypedOtherStatement* contains_stmt;
   SgUntypedNamedStatement* end_module_stmt;
   SgUntypedModuleScope * module_scope;

// scope lists
   SgUntypedDeclarationStatementList* decl_list;
   SgUntypedStatementList*            stmt_list;
   SgUntypedFunctionDeclarationList*  func_list;

   if (ATmatch(term, "Module(<term>,<term>,<term>,<term>)", &term1,&term2,&term3,&term4))
   {
      if (traverse_ModuleStmt(term1, &module_stmt)) {
         // MATCHED ModuleStmt
      } else return ATfalse;

      decl_list = new SgUntypedDeclarationStatementList();
      stmt_list = new SgUntypedStatementList();
      func_list = new SgUntypedFunctionDeclarationList();

      label = module_stmt->get_label_string();
      name  = module_stmt->get_statement_name();

      module_scope = new SgUntypedModuleScope(label,decl_list,stmt_list,func_list);
      ROSE_ASSERT(module_scope);

      if (traverse_SpecificationPart(term2, decl_list)) {
         // MATCHED SpecificationPart
         setSourcePosition(decl_list, term2);
      } else return ATfalse;
      if (traverse_OptModuleSubprogramPart(term3, &contains_stmt, module_scope)) {
         // MATCHED OptModuleSubprogramPart
         setSourcePosition(func_list, term3);
      } else return ATfalse;

      if (traverse_EndModuleStmt(term4, &end_module_stmt)) {
         // MATCHED EndModuleStmt
      } else return ATfalse;
   } else return ATfalse;

   module = new SgUntypedModuleDeclaration(label,name,module_scope,end_module_stmt);
   ROSE_ASSERT(module);

   setSourcePositionIncludingNode(module, term, end_module_stmt);
   setSourcePositionIncludingNode(module->get_scope(), term2, end_module_stmt);

// add the module to the outer scope
   scope->get_declaration_list()->get_decl_list().push_back(module);

// no longer need module_stmt as this information is contained in the module declaration
   delete module_stmt;

   return ATtrue;
}

//========================================================================================
// module-stmt (R1105)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_ModuleStmt(ATerm term, SgUntypedNamedStatement** module_stmt)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ModuleStmt: %s\n", ATwriteToString(term));
#endif
  
   ATerm term1, term2, term_eos;
   std::string label;
   std::string name;
   std::string eos;

   SgToken::ROSE_Fortran_Keywords keyword = SgToken::FORTRAN_MODULE;

   *module_stmt = NULL; 
   if (ATmatch(term, "ModuleStmt(<term>,<term>,<term>)", &term1,&term2,&term_eos)) {
      if (traverse_OptLabel(term1, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_Name(term2, name)) {
         // MATCHED ProcedureName string
      } else return ATfalse;
      if (traverse_eos(term_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

   *module_stmt = new SgUntypedNamedStatement(label,keyword,name);
   setSourcePositionExcludingTerm(*module_stmt, term, term_eos);

  return ATtrue;
}

//========================================================================================
// end-module-stmt (R1106)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_EndModuleStmt(ATerm term, SgUntypedNamedStatement** end_module_stmt)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_EndModuleStmt: %s\n", ATwriteToString(term));
#endif

   ATerm term1, term2, term_eos;
   std::string label, name, eos;

   SgToken::ROSE_Fortran_Keywords keyword = SgToken::FORTRAN_END_MODULE;

   *end_module_stmt = NULL;
   if (ATmatch(term, "EndModuleStmt(<term>,<term>,<term>)", &term1,&term2,&term_eos)) {
      if (traverse_OptLabel(term1, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_OptName(term2, name)) {
         // MATCHED ModuleName string
      } else return ATfalse;
      if (traverse_eos(term_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

   *end_module_stmt = new SgUntypedNamedStatement(label,keyword,name);
   setSourcePositionExcludingTerm(*end_module_stmt, term, term_eos);

   return ATtrue;
}

//========================================================================================
// block-construct (R1107-2018-N2146)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_BlockConstruct(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BlockConstruct: %s\n", ATwriteToString(term));
#endif

   printf("-------------------------------------\n");

   ATerm t_block_stmt, t_spec_part, t_block, t_end_block;

   SgUntypedBlockStatement* block = NULL;

   if (ATmatch(term,"BlockConstruct(<term>,<term>,<term>,<term>)",&t_block_stmt,&t_spec_part,&t_block,&t_end_block))
     {
        SgUntypedNamedStatement * block_stmt, * end_block_stmt;

        if (traverse_BlockStmt(t_block_stmt, &block_stmt)) {
           // MATCHED BlockStmt
        } else return ATfalse;

     // Traverse block out of order in order to have access to declaration and statement lists
        if (traverse_Block(t_block, &block)) {
           // MATCHED Block
        } else return ATfalse;

        ROSE_ASSERT(block_stmt);
        setSourcePosition(block, term);

        SgUntypedDeclarationStatementList* decl_list = block->get_scope()->get_declaration_list();

        if (traverse_SpecificationPart(t_spec_part, decl_list)) {
           // MATCHED SpecificationPart
           setSourcePosition(decl_list, term);
        } else return ATfalse;

        if (traverse_EndBlockStmt(t_end_block, &end_block_stmt)) {
           // MATCHED EndBlockStmt
        } else return ATfalse;

     // block-stmt and end-block-stmt unused (except for block-construct-name)
        delete block_stmt;
        delete end_block_stmt;
     }
   else return ATfalse;

   stmt_list->get_stmt_list().push_back(block);

   return ATtrue;
}

//========================================================================================
// block-stmt (R1108-2018-N2146)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_BlockStmt(ATerm term, SgUntypedNamedStatement** block_stmt)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BlockStmt: %s\n", ATwriteToString(term));
#endif

   ATerm t_label, t_name, t_eos;
   std::string label, name, eos;

   int stmt_enum = Fortran_ROSE_Translation::e_block_stmt;

   *block_stmt = NULL;
   if (ATmatch(term, "BlockStmt(<term>,<term>,<term>)", &t_label,&t_name,&t_eos)) {
      if (traverse_OptLabel(t_label, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_OptName(t_name, name)) {
         // MATCHED ModuleName string
      } else return ATfalse;
      if (traverse_eos(t_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

   *block_stmt = new SgUntypedNamedStatement(label,stmt_enum,name);
   setSourcePositionExcludingTerm(*block_stmt, term, t_eos);

   return ATtrue;
}

//========================================================================================
// end-block-stmt (R1110-2018-N2146)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_EndBlockStmt(ATerm term, SgUntypedNamedStatement** end_block_stmt)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_EndBlockStmt: %s\n", ATwriteToString(term));
#endif

   ATerm t_label, t_name, t_eos;
   std::string label, name, eos;

   int stmt_enum = Fortran_ROSE_Translation::e_end_block_stmt;

   *end_block_stmt = NULL;
   if (ATmatch(term, "EndBlockStmt(<term>,<term>,<term>)", &t_label,&t_name,&t_eos)) {
      if (traverse_OptLabel(t_label, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_OptName(t_name, name)) {
         // MATCHED ModuleName string
      } else return ATfalse;
      if (traverse_eos(t_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

   *end_block_stmt = new SgUntypedNamedStatement(label,stmt_enum,name);
   setSourcePositionExcludingTerm(*end_block_stmt, term, t_eos);

   return ATtrue;
}

//========================================================================================
// module-subprogram-part (R1107)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_OptModuleSubprogramPart(ATerm term, SgUntypedOtherStatement** contains_stmt, SgUntypedScope* scope)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptModuleSubprogramPart: %s\n", ATwriteToString(term));
#endif
  
   ATerm term1, term2;

   if (ATmatch(term, "no-module-subprogram-part()")) {
      // MATCHED no-module-subprogram-part
   }
   else if (ATmatch(term, "ModuleSubprogramPart(<term>,<term>)", &term1,&term2)) {
      if (traverse_ContainsStmt(term1, contains_stmt)) {
         // MATCHED ContainsStmt
      } else return ATfalse;
      if (traverse_ModuleSubprogramList(term2, scope)) {
         // MATCHED ModuleSubprogram list
      } else return ATfalse;
   } else return ATfalse;

  return ATtrue;
}

//========================================================================================
// module-subprogram (R1108)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_ModuleSubprogramList(ATerm term, SgUntypedScope* scope)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_ModuleSubprogramList: %s\n", ATwriteToString(term));
#endif
  
  ATermList tail = (ATermList) ATmake("<term>", term);
  while (! ATisEmpty(tail)) {
     ATerm head = ATgetFirst(tail);
     tail = ATgetNext(tail);
     if (traverse_SubroutineSubprogram(head, scope)) {
        // MATCHED SubroutineSubprogram
     }
     else if (traverse_FunctionSubprogram(head, scope)) {
        // MATCHED FunctionSubprogram
     }
     else if (traverse_SeparateModuleSubprogram(head, scope)) {
        // MATCHED FunctionSubprogram
     }
     else return ATfalse;
  }

  return ATtrue;
}

//========================================================================================
// use-stmt (R1109)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_UseStmt(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_UseStmt: %s\n", ATwriteToString(term));
#endif

   ATerm term1, term2, term3, term4, eos_term;
   std::string label, name, eos;
   SgUntypedTokenPairList* rename_or_only_list;
   SgToken::ROSE_Fortran_Keywords module_nature;

   bool isOnlyList;

  if (ATmatch(term, "UseStmt(<term>,<term>,<term>,<term>,<term>)", &term1,&term2,&term3,&term4,&eos_term))
  {
   // Regular UseStmt without an ONLY clause
      isOnlyList = false;
  }
  else if (ATmatch(term, "UseOnlyStmt(<term>,<term>,<term>,<term>,<term>)", &term1,&term2,&term3,&term4,&eos_term))
  {
   // UseStmt with an ONLY clause (may also have rename in list)
      isOnlyList = true;
  }
  else return ATfalse;

  if (traverse_OptLabel(term1, label)) {
     // MATCHED OptLabel
  } else return ATfalse;
  if (traverse_OptModuleNature(term2, module_nature)) {
     // MATCHED OptModuleNature
  } else return ATfalse;
  if (traverse_Name(term3, name)) {
     // MATCHED ModuleName
  } else return ATfalse;

  rename_or_only_list = new SgUntypedTokenPairList();

  if (traverse_RenameOrOnlyList(term4, isOnlyList, rename_or_only_list)) {
     // MATCHED a rename or only list
  } else return ATfalse;

  if (traverse_eos(eos_term, eos)) {
     // MATCHED eos
  } else return ATfalse;

  std::cout << "--- rename_or_only_list length is " <<  rename_or_only_list->get_token_pair_list().size() << std::endl;

  SgUntypedUseStatement* use_stmt = new SgUntypedUseStatement(label,name,module_nature,rename_or_only_list,isOnlyList);
  setSourcePositionExcludingTerm(use_stmt, term, eos_term);

  decl_list->get_decl_list().push_back(use_stmt);

  return ATtrue;
}

//========================================================================================
// use-stmt-list
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_UseStmtList(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_UseStmtList: %s\n", ATwriteToString(term));
#endif

  ATermList tail = (ATermList) ATmake("<term>", term);
  while (! ATisEmpty(tail)) {
     ATerm head = ATgetFirst(tail);
     tail = ATgetNext(tail);
     if (traverse_UseStmt(head, decl_list)) {
        // MATCHED UseStmt
     } else return ATfalse;
  }

  return ATtrue;
}

//========================================================================================
// module-nature (R1110)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_OptModuleNature(ATerm term, SgToken::ROSE_Fortran_Keywords & module_nature)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_OptModuleNature: %s\n", ATwriteToString(term));
#endif

  ATerm term1;
  
  if (ATmatch(term, "no-module-nature()")) {
     // MATCHED no-module-nature
     module_nature = SgToken::FORTRAN_UNKNOWN;
  }
  else if (ATmatch(term, "ModuleNature(<term>)", &term1)) {
     if (ATmatch(term1, "INTRINSIC()")) {
        // MATCHED INTRINSIC
        module_nature = SgToken::FORTRAN_INTRINSIC;
     }
     else if (ATmatch(term1, "NON_INTRINSIC()")) {
        // MATCHED NON_INTRINSIC
        module_nature = SgToken::FORTRAN_NON_INTRINSIC;
     } else return ATfalse;
  } else return ATfalse;

  return ATtrue;
}

// Traverse a use-stmt only list
//
//========================================================================================
// rename (R1111), only (R1112)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_RenameOrOnlyList(ATerm term, bool isOnlyList, SgUntypedTokenPairList* rename_or_only_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_RenameOrOnlyList: %s\n", ATwriteToString(term));
#endif

   ATerm terms;

// First match the cons name for the list to obtain the actual list [ ]
//
   if (isOnlyList) {
      if (ATmatch(term, "OnlyList(<term>)", &terms)) {
         // This is an only/rename-list
      }
      else return ATfalse;
   }
   else {
      if (ATmatch(term, "no-list()")) {
         // No rename list
         return ATtrue;
      }
      else if (ATmatch(term, "comma-list(<term>)", &terms)) {
         // This is a rename-list
      }
      else return ATfalse;
   }

// Now traverse the list
//
   ATermList tail = (ATermList) ATmake("<term>", terms);
   while (! ATisEmpty(tail))
   {
      char * str1, * str2;
      SgUntypedToken* generic_spec = NULL;
      SgUntypedTokenPair* only_or_rename = NULL;

      std::string only_name = "";

      ATerm head = ATgetFirst(tail);
      tail = ATgetNext(tail);

   // Try matching GenericSpec (this will catch use-only name cases without a rename)
      if (traverse_OptGenericSpec(head, only_name, &generic_spec))
      {
         if (only_name.length() > 0)
         {
            // There is an ONLY name and thus no rename clause
            std::cout << "--- ONLY name is " << only_name << std::endl;
            only_or_rename = new SgUntypedTokenPair(only_name, SgToken::FORTRAN_ONLY,
                                                    "",        SgToken::FORTRAN_UNKNOWN);
         }
         else if (generic_spec->get_lexeme_string() == "NO_GENERIC_SPEC")
            {
               // No generic-spec so it must be a rename
               if (ATmatch(head, "Rename(<str>,<str>)", &str1, &str2))
                  {
                     printf("--- RENAME: %s => %s\n", str1, str2);
                     // TODO - would be nice to use FORTRAN_RENAME
                     only_or_rename = new SgUntypedTokenPair(str1, SgToken::FORTRAN_UNKNOWN,
                                                             str2, SgToken::FORTRAN_UNKNOWN);
                  }
               else if (ATmatch(head, "RenameOp(<str>,<str>)", &str1, &str2))
                  {
                     printf("--- REMANE_OP: %s => %s\n", str1, str2);
                     // TODO - would be nice to use FORTRAN_RENAME
                     only_or_rename = new SgUntypedTokenPair(str1, SgToken::FORTRAN_UNKNOWN,
                                                             str2, SgToken::FORTRAN_UNKNOWN);
                  }
               else return ATfalse;
            }
         else
            {
               ROSE_ASSERT(generic_spec->get_lexeme_string()       == "NO_GENERIC_SPEC");
               ROSE_ASSERT(generic_spec->get_classification_code() == SgToken::FORTRAN_UNKNOWN);
               delete generic_spec;
               // TODO - generic_spec should probably just be NULL
               std::cout << "--- No generic spec - can we even be here\n";
            }
      }

      if (only_or_rename != NULL)
      {
         setSourcePosition(only_or_rename, head);
         rename_or_only_list->get_token_pair_list().push_back(only_or_rename);
      }
  }

  return ATtrue;
}

//========================================================================================
// submodule (R1116)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_Submodule(ATerm term, SgUntypedScope* scope)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Submodule: %s\n", ATwriteToString(term));
#endif

// TODO - implementation
   std::cerr << "...TODO... implement Submodule" << std::endl;

   ATerm term1, term2, term3, term4;
   std::string label, name, ancestor, parent;

   SgUntypedSubmoduleDeclaration* submodule;
   SgUntypedNamedStatement* submodule_stmt;
   SgUntypedOtherStatement* contains_stmt;
   SgUntypedNamedStatement* end_submodule_stmt;
   SgUntypedModuleScope * submodule_scope;

// scope lists
   SgUntypedDeclarationStatementList* decl_list;
   SgUntypedStatementList*            stmt_list;
   SgUntypedFunctionDeclarationList*  func_list;

   if (ATmatch(term, "Submodule(<term>,<term>,<term>,<term>)", &term1,&term2,&term3,&term4))
   {
      if (traverse_SubmoduleStmt(term1, &submodule_stmt, ancestor, parent)) {
         // MATCHED SubmoduleStmt
      } else return ATfalse;

      decl_list = new SgUntypedDeclarationStatementList();
      stmt_list = new SgUntypedStatementList();
      func_list = new SgUntypedFunctionDeclarationList();

      label = submodule_stmt->get_label_string();
      name  = submodule_stmt->get_statement_name();
      submodule_scope = new SgUntypedModuleScope(label,decl_list,stmt_list,func_list);

      if (traverse_SpecificationPart(term2, decl_list)) {
         // MATCHED SpecificationPart
         setSourcePosition(decl_list, term2);
      } else return ATfalse;
      if (traverse_OptModuleSubprogramPart(term3, &contains_stmt, submodule_scope)) {
         // MATCHED OptModuleSubprogramPart
         setSourcePosition(func_list, term3);
      } else return ATfalse;

      if (traverse_EndSubmoduleStmt(term4, &end_submodule_stmt)) {
         // MATCHED EndSubmoduleStmt
      } else return ATfalse;
   } else return ATfalse;

   submodule = new SgUntypedSubmoduleDeclaration(label,name,ancestor,parent,
                                                 submodule_scope,end_submodule_stmt);

   setSourcePositionIncludingNode(submodule, term, end_submodule_stmt);
   setSourcePositionIncludingNode(submodule->get_scope(), term2, end_submodule_stmt);

// add the submodule to the outer scope
   scope->get_declaration_list()->get_decl_list().push_back(submodule);

// no longer need submodule_stmt as this information is contained in the submodule declaration
   delete submodule_stmt;

   return ATfalse;
}

//========================================================================================
// submodule-stmt (R1117)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_SubmoduleStmt(ATerm term, SgUntypedNamedStatement** submodule_stmt,
                                                    std::string & ancestor, std::string & parent)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SubmoduleStmt: %s\n", ATwriteToString(term));
#endif

   ATerm term1, term2, term3, term_eos;
   std::string label, name, eos;

   SgToken::ROSE_Fortran_Keywords keyword = SgToken::FORTRAN_SUBMODULE;

   *submodule_stmt = NULL;
   if (ATmatch(term, "SubmoduleStmt(<term>,<term>,<term>,<term>)", &term1,&term2,&term3,&term_eos)) {
      if (traverse_OptLabel(term1, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_Name(term2, name)) {
         // MATCHED ProcedureName string
      } else return ATfalse;
      if (traverse_ParentIdentifier(term3, ancestor, parent)) {
         // MATCHED ProcedureName string
      } else return ATfalse;
      if (traverse_eos(term_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

   *submodule_stmt = new SgUntypedNamedStatement(label,keyword,name);
   setSourcePositionExcludingTerm(*submodule_stmt, term, term_eos);

  return ATtrue;
}

//========================================================================================
// parent-identifier (R1118)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_ParentIdentifier(ATerm term, std::string & ancestor, std::string & parent)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ParentIdentifier: %s\n", ATwriteToString(term));
#endif

   char * arg1, * arg2;

   if (ATmatch(term, "ParentIdentifier(<str>,no-name())", &arg1)) {
      // MATCHED ParentIdentifier (with no parent)
      ancestor += arg1;
   }
   else if (ATmatch(term, "ParentIdentifier(<str>,<str>)", &arg1, &arg2)) {
      // MATCHED ParentIdentifier (parent)
      ancestor += arg1;
      parent   += arg2;
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// end-module-stmt (R1119)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_EndSubmoduleStmt(ATerm term, SgUntypedNamedStatement** end_submodule_stmt)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_EndSubmoduleStmt: %s\n", ATwriteToString(term));
#endif

   ATerm term1, term2, term_eos;
   std::string label, name, eos;

   SgToken::ROSE_Fortran_Keywords keyword = SgToken::FORTRAN_END_SUBMODULE;

   *end_submodule_stmt = NULL;
   if (ATmatch(term, "EndSubmoduleStmt(<term>,<term>,<term>)", &term1,&term2,&term_eos)) {
      if (traverse_OptLabel(term1, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_OptName(term2, name)) {
         // MATCHED SubmoduleName string
      } else return ATfalse;
      if (traverse_eos(term_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

   *end_submodule_stmt = new SgUntypedNamedStatement(label,keyword,name);
   setSourcePositionExcludingTerm(*end_submodule_stmt, term, term_eos);

   return ATtrue;
}

//========================================================================================
// BlockData (R1120)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_BlockData(ATerm term, SgUntypedScope* scope)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BlockData: %s\n", ATwriteToString(term));
#endif

   ATerm term1, term2, term3;
   std::string label, name;

   SgUntypedBlockDataDeclaration*  block_data;
   SgUntypedNamedStatement*        block_data_stmt;
   SgUntypedNamedStatement*    end_block_data_stmt;

   SgUntypedDeclarationStatementList* decl_list;

   if (ATmatch(term, "BlockData(<term>,<term>,<term>)", &term1,&term2,&term3))
   {
      if (traverse_BlockDataStmt(term1, &block_data_stmt)) {
         // MATCHED BlockDataStmt
      } else return ATfalse;

      decl_list = new SgUntypedDeclarationStatementList();

      label = block_data_stmt->get_label_string();
      name  = block_data_stmt->get_statement_name();

      if (traverse_SpecificationPart(term2, decl_list)) {
         // MATCHED SpecificationPart
         setSourcePosition(decl_list, term2);
      } else return ATfalse;
      if (traverse_EndBlockDataStmt(term3, &end_block_data_stmt)) {
         // MATCHED EndBlockDataStmt
      } else return ATfalse;
   } else return ATfalse;

   block_data = new SgUntypedBlockDataDeclaration(label,name,false,decl_list,end_block_data_stmt);

   setSourcePositionIncludingNode(block_data, term, end_block_data_stmt);

// add block data to the outer scope
   scope->get_declaration_list()->get_decl_list().push_back(block_data);

// no longer need block_data_stmt as this information is contained in the block data declaration
   delete block_data_stmt;
 
   return ATtrue;
}

//========================================================================================
// block-data-stmt (R1121)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_BlockDataStmt(ATerm term, SgUntypedNamedStatement** block_data_stmt)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BlockDataStmt: %s\n", ATwriteToString(term));
#endif
  
   ATerm term1, term2, term_eos;
   std::string label;
   std::string name;
   std::string eos;

   SgToken::ROSE_Fortran_Keywords keyword = SgToken::FORTRAN_BLOCK_DATA;

   *block_data_stmt = NULL; 
   if (ATmatch(term, "BlockDataStmt(<term>,<term>,<term>)", &term1,&term2,&term_eos)) {
      if (traverse_OptLabel(term1, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_OptName(term2, name)) {
         // MATCHED optional BlockDataName string
      } else return ATfalse;
      if (traverse_eos(term_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

   *block_data_stmt = new SgUntypedNamedStatement(label,keyword,name);
   setSourcePositionExcludingTerm(*block_data_stmt, term, term_eos);

  return ATtrue;
}

//========================================================================================
// end-block-data-stmt (R1122)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_EndBlockDataStmt(ATerm term, SgUntypedNamedStatement** end_block_data_stmt)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_EndBlockDataStmt: %s\n", ATwriteToString(term));
#endif

   ATerm term1, term2, term_eos;
   std::string label, name, eos;

   SgToken::ROSE_Fortran_Keywords keyword = SgToken::FORTRAN_END_BLOCK_DATA;

   *end_block_data_stmt = NULL;
   if (ATmatch(term, "EndBlockDataStmt(<term>,<term>,<term>)", &term1,&term2,&term_eos)) {
      if (traverse_OptLabel(term1, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_OptName(term2, name)) {
         // MATCHED ModuleName string
      } else return ATfalse;
      if (traverse_eos(term_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

   *end_block_data_stmt = new SgUntypedNamedStatement(label,keyword,name);
   setSourcePositionExcludingTerm(*end_block_data_stmt, term, term_eos);

   return ATtrue;
}

//========================================================================================
// loop-control (R1123-2018-N2146): DO CONCURRENT
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_LoopConcurrentControl(ATerm term, SgUntypedType* & type, SgUntypedExprListExpression** header,
                                                                                  SgUntypedExprListExpression** locality, SgUntypedExpression** mask)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_LoopConcurrentControl: %s\n", ATwriteToString(term));
#endif

   ATerm t_header, t_locality;

   type      = NULL;
   *header   = NULL;
   *locality = NULL;
   *mask     = NULL;

   if (ATmatch(term, "LoopConcurrentControl(<term>,<term>)", &t_header, &t_locality)) {

      if (traverse_ConcurrentHeader(t_header, type, header, mask)) {
         // MATCHED ConcurrentHeader
      } else return ATfalse;
      if (traverse_ConcurrentLocality(t_locality, locality)) {
         // MATCHED ConcurrentLocality
      } else return ATfalse;

   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// concurrent-header (R1125-2018-N2146)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_ConcurrentHeader(ATerm term, SgUntypedType* & type,
                                                                             SgUntypedExprListExpression** header, SgUntypedExpression** mask)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ConcurrentHeader: %s\n", ATwriteToString(term));
#endif

   using namespace General_Language_Translation;

   ATerm t_type, t_header, t_mask;

   SgUntypedExprListExpression* concurrent_header  = NULL;
   SgUntypedNamedExpression*    concurrent_control = NULL;

   type      = NULL;
   *header   = NULL;
   *mask     = NULL;

   if (ATmatch(term, "ConcurrentHeader(<term>,<term>,<term>)", &t_type, &t_header, &t_mask)) {

      if (ATmatch(t_type, "no-type-spec()")) {
         // there is no type spec
         type = UntypedBuilder::buildType(SgUntypedType::e_unknown);
      }
      else if (traverse_IntrinsicTypeSpec(t_type, type)) {
         // MATCHED TypeSpec
      }
      else return ATfalse;

   // slightly easier to check for mask first (note doesn't match parse order)
      if (ATmatch(t_mask, "no-mask-expr()")) {
         // there is no mask expression
      }
      else if (traverse_Expression(t_mask, mask)) {
         // MATCHED Expression
      }
      else return ATfalse;

      concurrent_header = new SgUntypedExprListExpression(e_fortran_concurrent_header);
      ROSE_ASSERT(concurrent_header);
      setSourcePosition(concurrent_header, t_header);

      ATermList tail = (ATermList) ATmake("<term>", t_header);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);

         if (traverse_ConcurrentControl(head, &concurrent_control)) {
            // MATCHED ConcurrentControl
         } else return ATfalse;
         ROSE_ASSERT(concurrent_control);
         concurrent_header->get_expressions().push_back(concurrent_control);
      }
   }
   else return ATfalse;

   ROSE_ASSERT(concurrent_header);
   *header = concurrent_header;

   return ATtrue;
}

//========================================================================================
// concurrent-control (R1126-2018-N2146)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_ConcurrentControl(ATerm term, SgUntypedNamedExpression** control)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ConcurrentControl: %s\n", ATwriteToString(term));
#endif

   using namespace General_Language_Translation;

   ATerm t_name, t_begin, t_end, t_step, t_step_ctrl;
   std::string name;
   SgUntypedExpression* begin = NULL;
   SgUntypedExpression* end   = NULL;
   SgUntypedExpression* step  = NULL;

   *control = NULL;

   if (ATmatch(term, "ConcurrentControl(<term>,<term>,<term>,<term>)", &t_name, &t_begin, &t_end, &t_step_ctrl)) {

      if (traverse_Name(t_name, name)) {
         // MATCHED ProcedureName string
      } else return ATfalse;

      if (traverse_Expression(t_begin, &begin)) {
         // MATCHED begin expression
      } else return ATfalse;

      if (traverse_Expression(t_end, &end)) {
         // MATCHED end expression
      } else return ATfalse;

      if (ATmatch(t_step_ctrl, "no-concurrent-step()")) {
         step = UntypedBuilder::buildUntypedNullExpression();
         ROSE_ASSERT(step);
         setSourcePositionUnknown(step);
      }
      else if (ATmatch(t_step_ctrl, "ConcurrentStep(<term>)", &t_step)) {
         if (traverse_Expression(t_step, &step)) {
            // MATCHED step expression
         } else return ATfalse;
      }
      else return ATfalse;
   }
   else return ATfalse;

   ROSE_ASSERT(begin && end && step);

   SgUntypedSubscriptExpression* triplet = new SgUntypedSubscriptExpression(e_array_index_triplet, begin, end, step);
   ROSE_ASSERT(triplet);
   setSourcePositionIncludingNode(triplet, t_begin, step);

   *control = new SgUntypedNamedExpression(e_fortran_concurrent_control, name, triplet);
   ROSE_ASSERT(*control);
   setSourcePosition(*control, term);

   return ATtrue;
}

//========================================================================================
// concurrent-locality (R1130-2018-N2146)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_ConcurrentLocality(ATerm term, SgUntypedExprListExpression** locality)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ConcurrentLocality: %s\n", ATwriteToString(term));
#endif

   using namespace General_Language_Translation;

   ATerm t_namelist;

   *locality = NULL;

   if (ATmatch(term, "ConcurrentLocality(<term>)", &t_namelist)) {
      cout << "WARNING: need to implement locality expr_enum is " << e_fortran_concurrent_locality << endl;
      *locality = new SgUntypedExprListExpression(e_fortran_concurrent_locality);
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// sync-all-stmt (R1164-2018-N2146)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_SyncAllStmt(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SyncAllStmt: %s\n", ATwriteToString(term));
#endif

   using namespace General_Language_Translation;

   ATerm t_label, t_stat_list, t_eos;
   std::string label;
   std::string eos;
   SgUntypedExprListExpression* sync_stat_list = NULL;

   if (ATmatch(term, "SyncAllStmt(<term>,<term>,<term>)", &t_label, &t_stat_list, &t_eos)) {
      if (traverse_OptLabel(t_label, label)) {
         // MATCHED OptLabel
      } else return ATfalse;

      sync_stat_list = new SgUntypedExprListExpression(e_fortran_sync_stat_list);
      ROSE_ASSERT(sync_stat_list);
      setSourcePosition(sync_stat_list, t_stat_list);

      if (traverse_ImageControlStatList(t_stat_list, sync_stat_list)) {
         // MATCHED OptStopCode
      } else return ATfalse;
      if (traverse_eos(t_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

   int stmt_enum = e_fortran_sync_all_stmt;

   SgUntypedImageControlStatement* sync_all_stmt = new SgUntypedImageControlStatement(label, stmt_enum, NULL, NULL, sync_stat_list);
   ROSE_ASSERT(sync_all_stmt);
   setSourcePositionExcludingTerm(sync_all_stmt, term, t_eos);

   stmt_list->get_stmt_list().push_back(sync_all_stmt);

   return ATtrue;
}

//========================================================================================
// sync-images-stmt (R1166-2018-N2146)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_SyncImagesStmt(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SyncImagesStmt: %s\n", ATwriteToString(term));
#endif

   using namespace General_Language_Translation;

   ATerm t_label, t_image_set, t_stat_list, t_eos;
   std::string label;
   std::string eos;
   SgUntypedExpression* image_set = NULL;
   SgUntypedExprListExpression* sync_stat_list = NULL;

   if (ATmatch(term, "SyncImagesStmt(<term>,<term>,<term>,<term>)", &t_label, &t_image_set, &t_stat_list, &t_eos)) {
      if (traverse_OptLabel(t_label, label)) {
         // MATCHED OptLabel
      } else return ATfalse;

      if (traverse_Expression(t_image_set, &image_set)) {
         // MATCHED ImagesValue (scalar-expr)
      } else return ATfalse;

      sync_stat_list = new SgUntypedExprListExpression(e_fortran_sync_stat_list);
      ROSE_ASSERT(sync_stat_list);
      setSourcePosition(sync_stat_list, t_stat_list);

      if (traverse_ImageControlStatList(t_stat_list, sync_stat_list)) {
         // MATCHED OptStopCode
      } else return ATfalse;
      if (traverse_eos(t_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

   int stmt_enum = e_fortran_sync_images_stmt;

   SgUntypedImageControlStatement* sync_images_stmt = new SgUntypedImageControlStatement(label, stmt_enum, NULL, image_set, sync_stat_list);
   ROSE_ASSERT(sync_images_stmt);
   setSourcePositionExcludingTerm(sync_images_stmt, term, t_eos);

   stmt_list->get_stmt_list().push_back(sync_images_stmt);

   return ATtrue;
}

//========================================================================================
// sync-memory-stmt (R1168-2018-N2146)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_SyncMemoryStmt(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SyncMemoryStmt: %s\n", ATwriteToString(term));
#endif

   using namespace General_Language_Translation;

   ATerm t_label, t_stat_list, t_eos;
   std::string label;
   std::string eos;
   SgUntypedExprListExpression* sync_stat_list = NULL;

   if (ATmatch(term, "SyncMemoryStmt(<term>,<term>,<term>)", &t_label, &t_stat_list, &t_eos)) {
      if (traverse_OptLabel(t_label, label)) {
         // MATCHED OptLabel
      } else return ATfalse;

      sync_stat_list = new SgUntypedExprListExpression(e_fortran_sync_stat_list);
      ROSE_ASSERT(sync_stat_list);
      setSourcePosition(sync_stat_list, t_stat_list);

      if (traverse_ImageControlStatList(t_stat_list, sync_stat_list)) {
         // MATCHED OptStopCode
      } else return ATfalse;
      if (traverse_eos(t_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

   int stmt_enum = e_fortran_sync_memory_stmt;

   SgUntypedImageControlStatement* sync_memory_stmt = new SgUntypedImageControlStatement(label, stmt_enum, NULL, NULL, sync_stat_list);
   ROSE_ASSERT(sync_memory_stmt);
   setSourcePositionExcludingTerm(sync_memory_stmt, term, t_eos);

   stmt_list->get_stmt_list().push_back(sync_memory_stmt);

   return ATtrue;
}

//========================================================================================
// sync-team-stmt (R1169-2018-N2146)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_SyncTeamStmt(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SyncTeamStmt: %s\n", ATwriteToString(term));
#endif

   using namespace General_Language_Translation;

   ATerm t_label, t_team, t_stat_list, t_eos;
   std::string label;
   std::string eos;
   SgUntypedExpression* team_value = NULL;
   SgUntypedExprListExpression* sync_stat_list = NULL;

   if (ATmatch(term, "SyncTeamStmt(<term>,<term>,<term>,<term>)", &t_label, &t_team, &t_stat_list, &t_eos)) {
      if (traverse_OptLabel(t_label, label)) {
         // MATCHED OptLabel
      } else return ATfalse;

      if (traverse_Expression(t_team, &team_value)) {
         // MATCHED TeamValue (scalar-expr)
      } else return ATfalse;

      sync_stat_list = new SgUntypedExprListExpression(e_fortran_sync_stat_list);
      ROSE_ASSERT(sync_stat_list);
      setSourcePosition(sync_stat_list, t_stat_list);

      if (traverse_ImageControlStatList(t_stat_list, sync_stat_list)) {
         // MATCHED OptStopCode
      } else return ATfalse;
      if (traverse_eos(t_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

   int stmt_enum = e_fortran_sync_team_stmt;

   SgUntypedImageControlStatement* sync_team_stmt = new SgUntypedImageControlStatement(label, stmt_enum, NULL, team_value, sync_stat_list);
   ROSE_ASSERT(sync_team_stmt);
   setSourcePositionExcludingTerm(sync_team_stmt, term, t_eos);

   stmt_list->get_stmt_list().push_back(sync_team_stmt);

   return ATtrue;
}

//========================================================================================
// sync-stat-list (R1165-F2018-N2146)
//
// Plus:
//
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_ImageControlStatList( ATerm term, SgUntypedExprListExpression* sync_stat_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ImageControlStatList: %s\n", ATwriteToString(term));
#endif

   using namespace General_Language_Translation;

   ATerm t_expr, t_stat_list;
   SgUntypedExprListExpression* status_container = NULL;

   if (ATmatch(term, "no-list()")) {
   }
   else if (ATmatch(term, "image-ctrl-stat-list(<term>)", &t_stat_list)) {

      ATermList tail = (ATermList) ATmake("<term>", t_stat_list);
      while (! ATisEmpty(tail)) {
         SgUntypedExpression* status = NULL;
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (ATmatch(head, "STAT(<term>)", &t_expr)) {
            traverse_Expression(t_expr, &status);
            status_container = new SgUntypedExprListExpression(e_fortran_sync_stat_stat);
         }
         else if (ATmatch(head, "ERRMSG(<term>)", &t_expr)) {
            traverse_Expression(t_expr, &status);
            status_container = new SgUntypedExprListExpression(e_fortran_sync_stat_errmsg);
         }
         else if (ATmatch(head, "ACQUIRED_LOCK(<term>)", &t_expr)) {
            traverse_Expression(t_expr, &status);
            status_container = new SgUntypedExprListExpression(e_fortran_stat_acquired_lock);
         }
         else {
            std::cerr << "ERROR: unknown sync-stat/lock-stat" << std::endl;
            return ATfalse;
         }

         ROSE_ASSERT(status);
         ROSE_ASSERT(status_container);

         status_container->get_expressions().push_back(status);
         sync_stat_list->get_expressions().push_back(status_container);
      }
   }

   return ATtrue;
}

//========================================================================================
// lock-stmt (R1179-2018-N2146)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_LockStmt(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_LockStmt: %s\n", ATwriteToString(term));
#endif

   using namespace General_Language_Translation;

   ATerm t_label, t_lock_variable, t_stat_list, t_eos;
   std::string label;
   std::string eos;
   SgUntypedExpression* lock_variable = NULL;
   SgUntypedExprListExpression* sync_stat_list = NULL;

   if (ATmatch(term, "LockStmt(<term>,<term>,<term>,<term>)", &t_label, &t_lock_variable, &t_stat_list, &t_eos)) {
      if (traverse_OptLabel(t_label, label)) {
         // MATCHED OptLabel
      } else return ATfalse;

      if (traverse_Expression(t_lock_variable, &lock_variable)) {
         // MATCHED TeamValue (scalar-expr)
      } else return ATfalse;

      sync_stat_list = new SgUntypedExprListExpression(e_fortran_sync_stat_list);
      ROSE_ASSERT(sync_stat_list);
      setSourcePosition(sync_stat_list, t_stat_list);

      if (traverse_ImageControlStatList(t_stat_list, sync_stat_list)) {
         // MATCHED OptStopCode
      } else return ATfalse;
      if (traverse_eos(t_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

   int stmt_enum = e_fortran_lock_stmt;

   SgUntypedImageControlStatement* lock_stmt = new SgUntypedImageControlStatement(label, stmt_enum, lock_variable, NULL, sync_stat_list);
   ROSE_ASSERT(lock_stmt);
   setSourcePositionExcludingTerm(lock_stmt, term, t_eos);

   stmt_list->get_stmt_list().push_back(lock_stmt);

   return ATtrue;
}

//========================================================================================
// unlock-stmt (R1181-2018-N2146)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_UnlockStmt(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_UnlockStmt: %s\n", ATwriteToString(term));
#endif

   using namespace General_Language_Translation;

   ATerm t_label, t_lock_variable, t_stat_list, t_eos;
   std::string label;
   std::string eos;
   SgUntypedExpression* lock_variable = NULL;
   SgUntypedExprListExpression* sync_stat_list = NULL;

   if (ATmatch(term, "UnlockStmt(<term>,<term>,<term>,<term>)", &t_label, &t_lock_variable, &t_stat_list, &t_eos)) {
      if (traverse_OptLabel(t_label, label)) {
         // MATCHED OptLabel
      } else return ATfalse;

      if (traverse_Expression(t_lock_variable, &lock_variable)) {
         // MATCHED TeamValue (scalar-expr)
      } else return ATfalse;

      sync_stat_list = new SgUntypedExprListExpression(e_fortran_sync_stat_list);
      ROSE_ASSERT(sync_stat_list);
      setSourcePosition(sync_stat_list, t_stat_list);

      if (traverse_ImageControlStatList(t_stat_list, sync_stat_list)) {
         // MATCHED OptStopCode
      } else return ATfalse;
      if (traverse_eos(t_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

   int stmt_enum = e_fortran_unlock_stmt;

   SgUntypedImageControlStatement* lock_stmt = new SgUntypedImageControlStatement(label, stmt_enum, lock_variable, NULL, sync_stat_list);
   ROSE_ASSERT(lock_stmt);
   setSourcePositionExcludingTerm(lock_stmt, term, t_eos);

   stmt_list->get_stmt_list().push_back(lock_stmt);

   return ATtrue;
}

//========================================================================================
// interface-block (R1201)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_InterfaceBlock(ATerm term, SgUntypedDeclarationStatementList* parent_decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_InterfaceBlock: %s\n", ATwriteToString(term));
#endif

   ATerm term1, term2, term3;
   std::string label, name;

   SgUntypedInterfaceDeclaration* interface_decl;
   SgUntypedNamedStatement* end_interface_stmt;

// scope lists
   SgUntypedDeclarationStatementList* decl_list;
   SgUntypedStatementList*            stmt_list;
   SgUntypedFunctionDeclarationList*  func_list;

   if (ATmatch(term, "InterfaceBlock(<term>,<term>,<term>)", &term1,&term2,&term3))
   {
      if (traverse_InterfaceStmt(term1, &interface_decl)) {
         // MATCHED InterfaceStmt
      } else return ATfalse;

      decl_list = new SgUntypedDeclarationStatementList();
      stmt_list = new SgUntypedStatementList();
      func_list = new SgUntypedFunctionDeclarationList();

   // Declarations and statements lists not used
      setSourcePositionUnknown(decl_list);
      setSourcePositionUnknown(stmt_list);

      if (traverse_InterfaceSpecificationList(term2, func_list)) {
         // MATCHED InitialSpecification
         setSourcePosition(func_list, term2);
      } else return ATfalse;
      if (traverse_EndInterfaceStmt(term3, &end_interface_stmt)) {
         // MATCHED EndInterfaceStmt
         interface_decl->set_end_statement(end_interface_stmt);
      } else return ATfalse;
   } else return ATfalse;

   setSourcePositionIncludingNode(interface_decl, term, end_interface_stmt);
   setSourcePositionIncludingNode(interface_decl->get_scope(), term2, end_interface_stmt);

   parent_decl_list->get_decl_list().push_back(interface_decl);

   return ATtrue;
}

//========================================================================================
// interface-specification (R1202)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_InterfaceSpecificationList(ATerm term, SgUntypedFunctionDeclarationList* func_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_InterfaceSpecificationList: %s\n", ATwriteToString(term));
#endif

   ATermList tail = (ATermList) ATmake("<term>", term);
   while (! ATisEmpty(tail)) {
      ATerm head = ATgetFirst(tail);
      tail = ATgetNext(tail);

      if (traverse_InterfaceBody(head, func_list)) {
         // MATCHED InterfaceBlock
      }
      else if (traverse_ProcedureStmt(head, func_list)) {
         // MATCHED ProcedureStmt
      }
      else return ATfalse;
   }

   return ATtrue;
}

//========================================================================================
// interface-stmt (R1203)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_InterfaceStmt(ATerm term, SgUntypedInterfaceDeclaration** interface_decl)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_InterfaceStmt: %s\n", ATwriteToString(term));
#endif

   ATerm term1, term2, eos_term;
   std::string label, name, eos;

   SgToken::ROSE_Fortran_Keywords interface_type;
   SgUntypedToken* generic_spec = NULL;

   if (ATmatch(term, "InterfaceStmt(<term>,<term>)", &term1, &term2, &eos_term))
   {
      interface_type = SgToken::FORTRAN_INTERFACE;

      if (traverse_OptLabel(term1, label)) {
         //MATCHED OptLabel
      } else return ATfalse;
      if (traverse_OptGenericSpec(term2, name, &generic_spec)) {
         //MATCHED OptGenericSpec
      } else return ATfalse;
      if (traverse_eos(eos_term, eos)) {
         //MATCHED EOS
      } else return ATfalse;
   }
   else if (ATmatch(term, "AbstractInterfaceStmt(<term>,<term>)", &term1, &eos_term))
   {
      interface_type = SgToken::FORTRAN_ABSTRACT_INTERFACE;

      if (traverse_OptLabel(term1, label)) {
         //MATCHED OptLabel
      } else return ATfalse;

      generic_spec = new SgUntypedToken("ABSTRACT INTERFACE", SgToken::FORTRAN_ABSTRACT_INTERFACE);
      setSourcePosition(generic_spec, term);

      if (traverse_eos(eos_term, eos)) {
         //MATCHED EOS
      } else return ATfalse;
   }
   else return ATfalse;

// Scope lists (only function declaration list is potentially used)
   SgUntypedDeclarationStatementList* decl_list = new SgUntypedDeclarationStatementList();
   SgUntypedStatementList*            stmt_list = new SgUntypedStatementList();
   SgUntypedFunctionDeclarationList*  func_list = new SgUntypedFunctionDeclarationList();
   SgUntypedInitializedNameList*     param_list = new SgUntypedInitializedNameList();
   SgUntypedExprListExpression*     prefix_list = new SgUntypedExprListExpression();

   SgUntypedFunctionScope* interface_scope = new SgUntypedFunctionScope(label,decl_list,stmt_list,func_list);

   *interface_decl = new SgUntypedInterfaceDeclaration(label, name, param_list,
                                                       UntypedBuilder::buildType(SgUntypedType::e_unknown),
                                                       interface_scope, prefix_list, NULL, interface_type);

// Source positions can only be set once end-interface-stmt is matched

   return ATtrue;
}

//========================================================================================
// end-interface-stmt (R1204)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_EndInterfaceStmt(ATerm term, SgUntypedNamedStatement** end_interface_stmt)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_EndInterfaceStmt: %s\n", ATwriteToString(term));
#endif

   ATerm term1, term2, term_eos;
   std::string label, name, eos;

   SgToken::ROSE_Fortran_Keywords keyword = SgToken::FORTRAN_END_INTERFACE;
   SgUntypedToken* generic_spec = NULL;

   *end_interface_stmt = NULL;
   if (ATmatch(term, "EndInterfaceStmt(<term>,<term>,<term>)", &term1,&term2,&term_eos)) {
      if (traverse_OptLabel(term1, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_OptGenericSpec(term2, name, &generic_spec)) {
         // MATCHED OptGenericSpec string
      } else return ATfalse;
      if (traverse_eos(term_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

// If there is a generic-spec it must match the original one from the interface-stmt,
// thus, discard this one.  This constraint could be checked but currently isn't.
// Ignore potential generic-name as well.

   if (generic_spec != NULL)
   {
      delete generic_spec;
   }

   *end_interface_stmt = new SgUntypedNamedStatement(label,keyword,name);
   setSourcePositionExcludingTerm(*end_interface_stmt, term, term_eos);

   return ATtrue;
}

//========================================================================================
// interface-body (R1205)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_InterfaceBody(ATerm term, SgUntypedFunctionDeclarationList* func_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_InterfaceBody: %s\n", ATwriteToString(term));
#endif

   return ATfalse;
}

//========================================================================================
// procedure-stmt (R1206)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_ProcedureStmt(ATerm term, SgUntypedFunctionDeclarationList* func_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ProcedureStmt: %s\n", ATwriteToString(term));
#endif

   return ATfalse;
}

//========================================================================================
// generic-spec (R1207)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_OptGenericSpec(ATerm term, std::string & name, SgUntypedToken** generic_spec)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptGenericSpec: %s\n", ATwriteToString(term));
#endif

   ATerm term1;
   char* arg1;

   std::string defined_operator;

   std::cout << "TODO - OptGenericSpec: handle no-generic-spec() specifically (return NULL?)\n";

   if (ATmatch(term, "<str>", &arg1)) {
      // MATCHED GenericName
      name = arg1;
      std::cout << "TODO - OptGenericSpec is use of generic-name OK?\n";
      *generic_spec = new SgUntypedToken(name, SgToken::FORTRAN_UNKNOWN);
   }
   else if (ATmatch(term, "OPERATOR(<term>)", &term1)) {
      if (traverse_DefinedOperator(term1, defined_operator)) {
         //MATCHED DefinedOperator
         *generic_spec = new SgUntypedToken(defined_operator, SgToken::FORTRAN_OPERATOR);
      } else return ATfalse;
   }
   else if (ATmatch(term, "ASSIGNMENT()")) {
      *generic_spec = new SgUntypedToken("ASSIGNMENT", SgToken::FORTRAN_ASSIGNMENT);
   }
// DefinedIoGenericSpec
//
   else if (ATmatch(term, "READ_FORMATTED()")) {
      *generic_spec = new SgUntypedToken("READ(FORMATTED)", SgToken::FORTRAN_READ_FORMATTED);
   }
   else if (ATmatch(term, "READ_UNFORMATTED()")) {
      *generic_spec = new SgUntypedToken("READ(UNFORMATTED)", SgToken::FORTRAN_READ_UNFORMATTED);
   }
   else if (ATmatch(term, "WRITE_FORMATTED()")) {
      *generic_spec = new SgUntypedToken("WRITE(FORMATTED)", SgToken::FORTRAN_WRITE_FORMATTED);
   }
   else if (ATmatch(term, "WRITE_UNFORMATTED()")) {
      *generic_spec = new SgUntypedToken("WRITE(UNFORMATTED)", SgToken::FORTRAN_WRITE_UNFORMATTED);
   }
// Optional case, no generic name or other generic spec
//
   else {
      *generic_spec = new SgUntypedToken("NO_GENERIC_SPEC", SgToken::FORTRAN_UNKNOWN);
   }

   setSourcePosition(*generic_spec, term);

   return ATtrue;
}

//========================================================================================
// import-stmt (R1209)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_ImportStmt(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ImportStmt: %s\n", ATwriteToString(term));
#endif

   ATerm t_label, t_name_list, t_eos;
   std::string label;
   std::string eos;
  
   SgUntypedNameList* name_list;
   SgUntypedNameListDeclaration* import_stmt;

   if (ATmatch(term, "ImportStmt(<term>,<term>,<term>)", &t_label,&t_name_list,&t_eos))
   {
      if (traverse_OptLabel(t_label, label)) {
         //matched OptLabel
      } else return ATfalse;

      name_list = new SgUntypedNameList();
      setSourcePosition(name_list, t_name_list);

      if (traverse_NameList(t_name_list, name_list)) {
         //matched NameList
      } else return ATfalse;
      if (traverse_eos(t_eos, eos)) {
         //matched EOS
      } else return ATfalse;
   }
   else return ATfalse;

   import_stmt = new SgUntypedNameListDeclaration(label, General_Language_Translation::e_fortran_import_stmt, name_list);
   setSourcePositionExcludingTerm(import_stmt, term, t_eos);

   decl_list->get_decl_list().push_back(import_stmt);

   return ATtrue;
}

//========================================================================================
// ImportStmtList
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_ImportStmtList(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_ImportStmtList: %s\n", ATwriteToString(term));
#endif

  ATermList tail = (ATermList) ATmake("<term>", term);
  while (! ATisEmpty(tail)) {
     ATerm head = ATgetFirst(tail);
     tail = ATgetNext(tail);
     if (traverse_ImportStmt(head, decl_list)) {
        // MATCHED ImportStmt
     } else return ATfalse;
  }

  return ATtrue;
}

//========================================================================================
// external-stmt (R1210)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_ExternalStmt(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ExternalStmt: %s\n", ATwriteToString(term));
#endif

   ATerm term1, term2, eos_term;
   std::string label;
   std::string eos;
  
   SgUntypedNameList* name_list;
   SgUntypedNameListDeclaration* external_stmt;

   if (ATmatch(term, "ExternalStmt(<term>,<term>,<term>)", &term1,&term2,&eos_term))
   {
      if (traverse_OptLabel(term1, label)) {
         // MATCHED OptLabel
      } else return ATfalse;

      name_list = new SgUntypedNameList();
      setSourcePosition(name_list, term2);

      if (traverse_NameList(term2, name_list)) {
         // MATCHED NameList
      } else return ATfalse;
      if (traverse_eos(eos_term, eos)) {
         // MATCHED EOS
      } else return ATfalse;
   }
   else return ATfalse;

   external_stmt = new SgUntypedNameListDeclaration(label, SgToken::FORTRAN_EXTERNAL, name_list);
   setSourcePositionExcludingTerm(external_stmt, term, eos_term);

   decl_list->get_decl_list().push_back(external_stmt);

   return ATtrue;
}

//========================================================================================
// call-stmt (R1220)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_CallStmt(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CallStmt: %s\n", ATwriteToString(term));
#endif

   ATerm t_label, t_proc, t_args, t_eos;
   std::string label;
   std::string eos;

   SgUntypedExpression* procedure;
   SgUntypedExprListExpression* args;
   SgUntypedFunctionCallStatement* call_stmt;

   if (ATmatch(term, "CallStmt(<term>,<term>,<term>,<term>)", &t_label,&t_proc,&t_args,&t_eos))
   {
      if (traverse_OptLabel(t_label, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_ProcedureDesignator(t_proc, &procedure)) {
         // MATCHED ProcedureDesignator
      } else return ATfalse;
      if (traverse_ActualArgSpecList(t_args, &args)) {
         // MATCHED ActualArgSpecList
      } else return ATfalse;
      if (traverse_eos(t_eos, eos)) {
         // MATCHED EOS
      } else return ATfalse;
   }
   else return ATfalse;

   int stmt_enum = General_Language_Translation::e_procedure_call;
   call_stmt = new SgUntypedFunctionCallStatement(label, stmt_enum, procedure, args, ""/*abort_name*/);
   ROSE_ASSERT(call_stmt);
   setSourcePositionExcludingTerm(call_stmt, term, t_eos);

   stmt_list->get_stmt_list().push_back(call_stmt);

   return ATtrue;
}

//========================================================================================
// procedure-designator (R1221)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_ProcedureDesignator(ATerm term, SgUntypedExpression** procedure)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CallStmt: %s\n", ATwriteToString(term));
#endif

   ATerm t_name;
   std::string procedure_name;

   if (ATmatch(term, "ProcedureDesignator(<term>)", &t_name))
   {
      if (traverse_Name(t_name, procedure_name)) {
         // MATCHED ProcedureName
         int expr_enum = General_Language_Translation::e_function_reference;
         *procedure = new SgUntypedReferenceExpression(expr_enum, procedure_name);
         setSourcePosition(*procedure, term);
      }
#if 0
      else if (traverse_ProcComponentRef(term, procedure)) {
         // MATCHED ProcComponentRef
      }
#endif
      else return ATfalse;
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// actual-arg-spec (R1222)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_ActualArgSpec(ATerm term, SgUntypedExpression** arg)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ActualArgSpec: %s\n", ATwriteToString(term));
#endif

   ATerm t_keyword, t_arg;
   std::string keyword;
   SgUntypedExpression* actual_arg;

   if (ATmatch(term, "ActualArgSpec(<term>,<term>)", &t_keyword, &t_arg))
   {
      cout << ".x. matched ActualArgSpec \n";

      // first get the argument expression
      if (traverse_Expression(t_arg, &actual_arg)) {
         // MATCHED ActualArg expression
      }

      cout << ".x. matched expression \n";

      if (ATmatch(t_keyword, "no-keyword()")) {
         // MATCHED no-keyword
         cout << ".x. matched no-keyword \n";
         *arg = actual_arg;
         cout << ".x. returning arg " << *arg << endl;
      }
      else if (traverse_Name(t_keyword, keyword)) {
         // MATCHED Keyword
         int expr_enum = General_Language_Translation::e_argument_keyword;
         *arg = new SgUntypedNamedExpression(expr_enum, keyword, actual_arg);
         ROSE_ASSERT(*arg);
         setSourcePosition(*arg, term);
      }
      else return ATfalse;
   }
   else return ATfalse;

   cout << ".x. returning arg " << *arg << endl;

   return ATtrue;
}

ATbool ATermToUntypedFortranTraversal::traverse_ActualArgSpecList(ATerm term, SgUntypedExprListExpression** args)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ActualArgSpecList: %s\n", ATwriteToString(term));
#endif

   SgUntypedExpression* arg;
   SgUntypedExprListExpression* arg_list;

   *args = NULL;

   arg_list = new SgUntypedExprListExpression(General_Language_Translation::e_argument_list);
   ROSE_ASSERT(arg_list);
   setSourcePosition(arg_list, term);

   if (ATmatch(term, "no-list()")) {
      // There doesn't always have to be a list
   }
   else {
      ATermList tail = (ATermList) ATmake("<term>", term);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_ActualArgSpec(head, &arg)) {
            // MATCHED ActualArgSpec
    cout << ".x. received arg  " << arg << endl;
            arg_list->get_expressions().push_back(arg);
         }
         else {
         // ERROR condition, cleanup
            delete arg_list;
            return ATfalse;
         }
      }
   }

   *args = arg_list;

   return ATtrue;
}

//========================================================================================
// prefix (R1225)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_OptPrefix(ATerm term, SgUntypedExprListExpression* prefix_list, SgUntypedType* & type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptPrefix: %s\n", ATwriteToString(term));
#endif

   using namespace General_Language_Translation;

   if (ATmatch(term, "no-prefix()"))
   {
      // there is no prefix
   }
   else
   {
   // Traverse the prefix-spec-list
      ATermList tail = (ATermList) ATmake("<term>", term);
      while (! ATisEmpty(tail))
      {
         SgUntypedOtherExpression* prefix = NULL;
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (traverse_DeclarationTypeSpec(head, type)) {
            // MATCHED DeclarationTypeSpec
         }
         else if (ATmatch(head, "ELEMENTAL()")) {
            prefix = new SgUntypedOtherExpression(e_function_modifier_elemental);
         }
         else if (ATmatch(head, "IMPURE()")) {
            prefix = new SgUntypedOtherExpression(e_function_modifier_impure);
         }
         else if (ATmatch(head, "MODULE()")) {
            prefix = new SgUntypedOtherExpression(e_function_modifier_module);
         }
         else if (ATmatch(head, "PURE()")) {
            prefix = new SgUntypedOtherExpression(e_function_modifier_pure);
         }
         else if (ATmatch(head, "RECURSIVE()")) {
            prefix = new SgUntypedOtherExpression(e_function_modifier_recursive);
         }
         else if (traverse_CudaAttributesPrefix(head, &prefix)) {
            // MATCHED CudaAttributesPrefix
         }
         else return ATfalse;

         if (prefix != NULL) {
            setSourcePosition(prefix, head);
            prefix_list->get_expressions().push_back(prefix);
         }
      }
   }

   return ATtrue;
}

ATbool ATermToUntypedFortranTraversal::traverse_CudaAttributesPrefix(ATerm term, SgUntypedOtherExpression** prefix)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CudaAttributesPrefix: %s\n", ATwriteToString(term));
#endif

   *prefix = NULL;

   if (ATmatch(term, "CudaAttributesPrefix(CUDA_HOST())")) {
      *prefix = new SgUntypedOtherExpression(General_Language_Translation::e_cuda_host);
   }
   else if (ATmatch(term, "CudaAttributesPrefix(CUDA_GLOBAL())")) {
      *prefix = new SgUntypedOtherExpression(General_Language_Translation::e_cuda_global_function);
   }
   else if (ATmatch(term, "CudaAttributesPrefix(CUDA_DEVICE())")) {
      *prefix = new SgUntypedOtherExpression(General_Language_Translation::e_cuda_device);
   }
   else if (ATmatch(term, "CudaAttributesPrefix(CUDA_GRID_GLOBAL())")) {
      *prefix = new SgUntypedOtherExpression(General_Language_Translation::e_cuda_grid_global);
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// FunctionSubprogram (R1227)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_FunctionSubprogram(ATerm term, SgUntypedScope* scope)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FunctionSubprogram: %s\n", ATwriteToString(term));
#endif

   ATerm term1, term2, term3, term4, term5, term6;
   std::string label, name;

   SgUntypedFunctionDeclaration* function;
   SgUntypedOtherStatement* contains_stmt;
   SgUntypedNamedStatement* end_function_stmt;
   SgUntypedFunctionScope * function_scope;

   SgUntypedType* function_type = NULL;

// scope and parameter lists
   SgUntypedDeclarationStatementList* decl_list = NULL;
   SgUntypedStatementList*            stmt_list = NULL;
   SgUntypedFunctionDeclarationList*  func_list = NULL;
   SgUntypedInitializedNameList*     param_list = NULL;
   SgUntypedExprListExpression*     prefix_list = NULL;

   if (ATmatch(term, "FunctionSubprogram(<term>,<term>,<term>,<term>,<term>)", &term1,&term2,&term3,&term4,&term5,&term6))
   {
      ATerm label_term, prefix_term, name_term, arglist_term, suffix_term, eos_term;
      std::string eos;

   // retrieve the function-stmt
   //
      if (ATmatch(term1, "FunctionStmt(<term>,<term>,<term>,<term>,<term>,<term>)"
                       , &label_term, &prefix_term, &name_term, &arglist_term, &suffix_term, &eos_term))
      {
         if (traverse_OptLabel(label_term, label)) {
            // MATCHED OptLabel
         } else return ATfalse;

         decl_list   = new SgUntypedDeclarationStatementList();
         stmt_list   = new SgUntypedStatementList();
         func_list   = new SgUntypedFunctionDeclarationList();
         param_list  = new SgUntypedInitializedNameList();
         prefix_list = new SgUntypedExprListExpression();

         if (traverse_OptPrefix(prefix_term, prefix_list, function_type)) {
            // MATCHED OptPrefix
            std::cerr << "...TODO... implement function prefix in SgUntypedFunctionDeclaration" << std::endl;
         } else return ATfalse;
         if (traverse_Name(name_term, name)) {
            // MATCHED FunctionName string
         } else return ATfalse;
         if (traverse_OptDummyArgList(arglist_term, param_list)) {
            // MATCHED OptDummyArgList
            setSourcePosition(param_list, arglist_term);
         } else return ATfalse;
         if (traverse_OptSuffix(suffix_term)) {
            // MATCHED OptProcLanguageBindingSpec
            std::cerr << "...TODO... implement function suffix in SgUntypedFunctionDeclaration" << std::endl;
         } else return ATfalse;
         if (traverse_eos(eos_term, eos)) {
            // MATCHED eos string
         } else return ATfalse;
      }
      else return ATfalse;

   // retrieve the rest of the function
   //
      if (traverse_InitialSpecPart(term2, decl_list)) {
         // InitialSpecPart
      } else return ATfalse;
      if (traverse_SpecAndExecPart(term3, decl_list, stmt_list)) {
         // SpecAndExecPart
         // TODO - FIXME - list changes from decl to stmt someplace in traverse_SpecAndExecPart
         setSourcePositionIncludingTerm(decl_list, term2, term3);
         setSourcePosition(stmt_list, term3);
      } else return ATfalse;

      function_scope = new SgUntypedFunctionScope(label,decl_list,stmt_list,func_list);

      if (traverse_OptInternalSubprogramPart(term4, &contains_stmt, function_scope)) {
         // OptInternalSubprogramPart
         setSourcePosition(func_list, term4);
      } else return ATfalse;
      if (traverse_EndFunctionStmt(term5, &end_function_stmt)) {
         // TRAVERSED EndFunctionStmt
      } else return ATfalse;
   } else return ATfalse;


   if (function_type == NULL) {
      // Build an unknown type for now
      // TODO - check suffix for RESULT and find correct type
      function_type = UntypedBuilder::buildType(SgUntypedType::e_unknown);
      std::cerr << "...TODO... implement function type SgUntypedFunctionDeclaration" << std::endl;
   }

// create the function
   function = new SgUntypedFunctionDeclaration(label, name, param_list, function_type,
                                               function_scope, prefix_list, end_function_stmt);
   setSourcePositionIncludingNode(function, term, end_function_stmt);
   setSourcePositionIncludingNode(function_scope, term2, end_function_stmt);

// add the function to the outer scope
   scope->get_function_list()->get_func_list().push_back(function);

   return ATtrue;
}

//========================================================================================
// ProcLanguageBindingSpec (R1229)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_OptProcLanguageBindingSpec(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptProcLanguageBindingSpec: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-language-binding()")) {
      // there is no language binding
   }

// TODO - implementation
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// Suffix (R1231)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_OptSuffix(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Suffix: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-suffix()")) {
      // there is no suffix
   }

//TODO - implementation
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// EndFunctiontmt (R1232)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_EndFunctionStmt(ATerm term, SgUntypedNamedStatement** end_function_stmt)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_EndFunctionStmt: %s\n", ATwriteToString(term));
#endif

   ATerm term1, term2, term_eos;
   std::string label, name, eos;

   SgToken::ROSE_Fortran_Keywords keyword = SgToken::FORTRAN_END_FUNCTION;

   *end_function_stmt = NULL;
   if (ATmatch(term, "EndFunctionStmt(<term>,<term>,<term>)", &term1,&term2,&term_eos)) {
      if (traverse_OptLabel(term1, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_OptName(term2, name)) {
         // MATCHED FunctionName string
      } else return ATfalse;
      if (traverse_eos(term_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

   *end_function_stmt = new SgUntypedNamedStatement(label,keyword,name);
   setSourcePositionExcludingTerm(*end_function_stmt, term, term_eos);

   return ATtrue;
}

//========================================================================================
// SubroutineSubprogram (R1233)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_SubroutineSubprogram(ATerm term, SgUntypedScope* scope)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SubroutineSubprogram: %s\n", ATwriteToString(term));
#endif

   ATerm term1, term2, term3, term4, term5;
   std::string label, name;

   SgUntypedSubroutineDeclaration* subroutine;
   SgUntypedOtherStatement* contains_stmt;
   SgUntypedNamedStatement* end_subroutine_stmt;
   SgUntypedFunctionScope * function_scope;

   SgUntypedType* function_type = NULL;

// scope and parameter lists
   SgUntypedDeclarationStatementList* decl_list = NULL;
   SgUntypedStatementList*            stmt_list = NULL;
   SgUntypedFunctionDeclarationList*  func_list = NULL;
   SgUntypedInitializedNameList*     param_list = NULL;
   SgUntypedExprListExpression*     prefix_list = NULL;

   if (ATmatch(term, "SubroutineSubprogram(<term>,<term>,<term>,<term>,<term>)", &term1,&term2,&term3,&term4,&term5))
   {
      ATerm label_term, prefix_term, name_term, arglist_term, binding_term, eos_term;
      std::string eos;

   // retrieve the subroutine-stmt
   //
      if (ATmatch(term1, "SubroutineStmt(<term>,<term>,<term>,<term>,<term>,<term>)"
                       , &label_term, &prefix_term, &name_term, &arglist_term, &binding_term, &eos_term))
      {
         if (traverse_OptLabel(label_term, label)) {
            // MATCHED OptLabel
         } else return ATfalse;

         decl_list   = new SgUntypedDeclarationStatementList();
         stmt_list   = new SgUntypedStatementList();
         func_list   = new SgUntypedFunctionDeclarationList();
         param_list  = new SgUntypedInitializedNameList();
         prefix_list = new SgUntypedExprListExpression();

         if (traverse_OptPrefix(prefix_term, prefix_list, function_type)) {
            // MATCHED OptPrefix
            ROSE_ASSERT(function_type == NULL);
         } else return ATfalse;
         if (traverse_Name(name_term, name)) {
            // MATCHED SubroutineName string
         } else return ATfalse;
         if (traverse_OptDummyArgList(arglist_term, param_list)) {
            // MATCHED OptDummyArgList
            setSourcePosition(param_list, arglist_term);
         } else return ATfalse;
         if (traverse_OptProcLanguageBindingSpec(binding_term)) {
            // MATCHED OptProcLanguageBindingSpec
         } else return ATfalse;
         if (traverse_eos(eos_term, eos)) {
            // MATCHED eos string
         } else return ATfalse;
      }
      else return ATfalse;

   // retrieve the rest of the subroutine
   //
      if (traverse_InitialSpecPart(term2, decl_list)) {
         // InitialSpecPart
      } else return ATfalse;
      if (traverse_SpecAndExecPart(term3, decl_list, stmt_list)) {
         // SpecAndExecPart
         // TODO - FIXME - list changes from decl to stmt someplace in traverse_SpecAndExecPart
         setSourcePositionIncludingTerm(decl_list, term2, term3);
         setSourcePosition(stmt_list, term3);
      } else return ATfalse;

      function_scope = new SgUntypedFunctionScope(label,decl_list,stmt_list,func_list);

      if (traverse_OptInternalSubprogramPart(term4, &contains_stmt, function_scope)) {
         // OptInternalSubprogramPart
         setSourcePosition(func_list, term4);
      } else return ATfalse;
      if (traverse_EndSubroutineStmt(term5, &end_subroutine_stmt)) {
         // EndSubroutineStmt
      } else return ATfalse;
   } else return ATfalse;

// organize parameters to construct the subroutine
//
   SgUntypedType* type = UntypedBuilder::buildType(SgUntypedType::e_void);

// create the subroutine
   subroutine = new SgUntypedSubroutineDeclaration(label, name, param_list, type,
                                                   function_scope, prefix_list, end_subroutine_stmt);
   setSourcePositionIncludingNode(subroutine, term, end_subroutine_stmt);
   setSourcePositionIncludingNode(function_scope, term2, end_subroutine_stmt);

// add the subroutine to the outer scope
   scope->get_function_list()->get_func_list().push_back(subroutine);

   return ATtrue;
}

//========================================================================================
// DummyArgList (R1235)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_OptDummyArgList(ATerm term, SgUntypedInitializedNameList* param_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptDummyArgList: %s\n", ATwriteToString(term));
#endif

   char* arg;

   if (ATmatch(term, "no-list()")) {
      // there are no dummy arguments
   }
   else {
      ATermList tail = (ATermList) ATmake("<term>", term);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (ATmatch(head, "<str>", &arg)) {
            // Matched an arg name
            SgUntypedInitializedName* iname = new SgUntypedInitializedName(UntypedBuilder::buildType(SgUntypedType::e_unknown), arg);
            setSourcePosition(iname, head);
            param_list->get_name_list().push_back(iname);
         } else return ATfalse;
      }
   }

   return ATtrue;
}

//========================================================================================
// EndSubroutineStmt (R1236)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_EndSubroutineStmt(ATerm term, SgUntypedNamedStatement** end_subroutine_stmt)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_EndSubroutineStmt: %s\n", ATwriteToString(term));
#endif

   ATerm term1, term2, term_eos;
   std::string label, name, eos;

   SgToken::ROSE_Fortran_Keywords keyword = SgToken::FORTRAN_END_SUBROUTINE;

   *end_subroutine_stmt = NULL;
   if (ATmatch(term, "EndSubroutineStmt(<term>,<term>,<term>)", &term1,&term2,&term_eos)) {
      if (traverse_OptLabel(term1, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_OptName(term2, name)) {
         // MATCHED SubroutineName string
      } else return ATfalse;
      if (traverse_eos(term_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

   *end_subroutine_stmt = new SgUntypedNamedStatement(label,keyword,name);
   setSourcePositionExcludingTerm(*end_subroutine_stmt, term, term_eos);

   return ATtrue;
}

//========================================================================================
// separate-module-subprogram (R1237)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_SeparateModuleSubprogram(ATerm term, SgUntypedScope* scope)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SeparateModuleSubprogram: %s\n", ATwriteToString(term));
#endif

   ATerm term1, term2, term3, term4, term5;
   std::string label, name;

   SgUntypedSubroutineDeclaration* mp_subprogram;
   SgUntypedNamedStatement* mp_subprogram_stmt;
   SgUntypedOtherStatement* contains_stmt;
   SgUntypedNamedStatement* end_mp_subprogram_stmt;
   SgUntypedFunctionScope * function_scope;

// scope and parameter lists
   SgUntypedDeclarationStatementList* decl_list = NULL;
   SgUntypedStatementList*            stmt_list = NULL;
   SgUntypedFunctionDeclarationList*  func_list = NULL;
   SgUntypedInitializedNameList*     param_list = NULL;
   SgUntypedExprListExpression*     prefix_list = NULL;

   if (ATmatch(term, "SeparateModuleSubprogram(<term>,<term>,<term>,<term>,<term>)", &term1,&term2,&term3,&term4,&term5))
   {
      if (traverse_MpSubprogramStmt(term1, &mp_subprogram_stmt)) {
         // MATCHED MpSubprogramStmt
      } else return ATfalse;

      decl_list   = new SgUntypedDeclarationStatementList();
      stmt_list   = new SgUntypedStatementList();
      func_list   = new SgUntypedFunctionDeclarationList();
      param_list  = new SgUntypedInitializedNameList();
      prefix_list = new SgUntypedExprListExpression();

      if (traverse_InitialSpecPart(term2, decl_list)) {
         // InitialSpecPart
      } else return ATfalse;
      if (traverse_SpecAndExecPart(term3, decl_list, stmt_list)) {
         // SpecAndExecPart
         // TODO - FIXME - list changes from decl to stmt someplace in traverse_SpecAndExecPart
         setSourcePositionIncludingTerm(decl_list, term2, term3);
         setSourcePosition(stmt_list, term3);
      } else return ATfalse;

      label = mp_subprogram_stmt->get_label_string();
      name  = mp_subprogram_stmt->get_statement_name();
      function_scope = new SgUntypedFunctionScope(label,decl_list,stmt_list,func_list);

      if (traverse_OptInternalSubprogramPart(term4, &contains_stmt, function_scope)) {
         // OptInternalSubprogramPart
      } else return ATfalse;
      if (traverse_EndMpSubprogramStmt(term5, &end_mp_subprogram_stmt)) {
         // MATCHED EndMpSubprogramStmt
      } else return ATfalse;

   }
   else return ATfalse;

// organize parameters to construct the separate-module-subprogram
//
//TODO - is this OK (check everywhere in this file)
   SgUntypedType* type = UntypedBuilder::buildType(SgUntypedType::e_void);

// create the subroutine
   mp_subprogram = new SgUntypedSubroutineDeclaration(label, name, param_list, type,
                                                      function_scope, prefix_list, end_mp_subprogram_stmt);
   setSourcePositionIncludingNode(mp_subprogram, term, end_mp_subprogram_stmt);

// add the subprogram to the outer scope
   scope->get_function_list()->get_func_list().push_back(mp_subprogram);

   return ATtrue;
}

//========================================================================================
// R1238 mp-subprogram-stmt
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_MpSubprogramStmt(ATerm term, SgUntypedNamedStatement** mp_subprogram_stmt)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptMpSubprogramStmt: %s\n", ATwriteToString(term));
#endif
  
   ATerm term1, term2, term_eos;
   std::string label;
   std::string name;
   std::string eos;

   SgToken::ROSE_Fortran_Keywords keyword = SgToken::FORTRAN_MODULE_PROC;

   *mp_subprogram_stmt = NULL; 
   if (ATmatch(term, "MpSubprogramStmt(<term>,<term>,<term>)", &term1,&term2,&term_eos)) {
      if (traverse_OptLabel(term1, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_Name(term2, name)) {
         // MATCHED ProcedureName string
      } else return ATfalse;
      if (traverse_eos(term_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

   *mp_subprogram_stmt = new SgUntypedNamedStatement(label,keyword,name);
   setSourcePositionExcludingTerm(*mp_subprogram_stmt, term, term_eos);

  return ATtrue;
}

//========================================================================================
// R1239 end-mp-subprogram-stmt
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_EndMpSubprogramStmt(ATerm term, SgUntypedNamedStatement** end_mp_subprogram_stmt)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_EndMpSubprogramStmt: %s\n", ATwriteToString(term));
#endif

   ATerm term1, term2, term_eos;
   std::string label;
   std::string name;
   std::string eos;

   SgToken::ROSE_Fortran_Keywords keyword = SgToken::FORTRAN_UNKNOWN; // NEW_TOKEN::FORTRAN_END_MP_PROGRAM

   *end_mp_subprogram_stmt = NULL;
   if (ATmatch(term, "EndMpSubprogramStmt(<term>,<term>,<term>)", &term1,&term2,&term_eos)) {
      if (traverse_OptLabel(term1, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_OptName(term2, name)) {
         // MATCHED ProcedureName string
      } else return ATfalse;
      if (traverse_eos(term_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   } else return ATfalse;

   *end_mp_subprogram_stmt = new SgUntypedNamedStatement(label,keyword,name);
   setSourcePositionExcludingTerm(*end_mp_subprogram_stmt, term, term_eos);

  return ATtrue;
}

//========================================================================================
// R1240 entry-stmt
//----------------------------------------------------------------------------------------

//========================================================================================
// R1241 return-stmt
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_ReturnStmt(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ReturnStmt: %s\n", ATwriteToString(term));
#endif
  
   ATerm term1, term2, term_eos;
   std::string label;
   std::string eos;
   SgUntypedExpression* expr;

   if (ATmatch(term, "ReturnStmt(<term>,<term>,<term>)", &term1,&term2,&term_eos)) {
      if (traverse_OptLabel(term1, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_OptExpr(term2, &expr)) {
         // MATCHED Expr                                                                                            
      } else return ATfalse;
      if (traverse_eos(term_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

   SgUntypedReturnStatement* return_stmt = new SgUntypedReturnStatement(label, expr);
   setSourcePositionExcludingTerm(return_stmt, term, term_eos);

   stmt_list->get_stmt_list().push_back(return_stmt);

   return ATtrue;
}

//========================================================================================
// R1242 contains-stmt
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_ContainsStmt(ATerm term, SgUntypedOtherStatement** contains_stmt)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_ContainsStmt: %s\n", ATwriteToString(term));
#endif
  
   ATerm term1, term_eos;
   std::string label;
   std::string eos;

   SgToken::ROSE_Fortran_Keywords keyword = SgToken::FORTRAN_CONTAINS;

   *contains_stmt = NULL;

   if (ATmatch(term, "ContainsStmt(<term>,<term>)", &term1,&term_eos)) {
      if (traverse_OptLabel(term1, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_eos(term_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

   *contains_stmt = new SgUntypedOtherStatement(label, keyword);
   setSourcePositionExcludingTerm(*contains_stmt, term, term_eos);

   return ATtrue;
}

//========================================================================================
// CudaAttributesStmt
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_CudaAttributesStmt(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_CudaAttributesStmt: %s\n", ATwriteToString(term));
#endif

   ATerm term1, term2, term3, eos_term;
   std::string label;
   std::string eos;

   int attr_enum;
   SgUntypedNameList* name_list;
   SgUntypedOtherExpression* attr_spec;
   SgUntypedNameListDeclaration* cuda_attributes_stmt;

   if (ATmatch(term, "CudaAttributesStmt(<term>,<term>,<term>,<term>)", &term1,&term2,&term3,&eos_term))
   {
      if (traverse_OptLabel(term1, label)) {
         // MATCHED OptLabel
      } else return ATfalse;

      if (traverse_CudaAttributesSpec(term2, &attr_spec)) {
         // MATCHED CudaAttributesSpec
      } else return ATfalse;

      attr_enum = attr_spec->get_expression_enum();
      delete attr_spec;

      name_list = new SgUntypedNameList();
      setSourcePosition(name_list, term3);

      if (traverse_NameList(term3, name_list)) {
         // MATCHED NameList
      } else return ATfalse;

      if (traverse_eos(eos_term, eos)) {
         // MATCHED EOS
      } else return ATfalse;
   }
   else return ATfalse;

   cuda_attributes_stmt = new SgUntypedNameListDeclaration(label, attr_enum, name_list);
   setSourcePositionExcludingTerm(cuda_attributes_stmt, term, eos_term);

   decl_list->get_decl_list().push_back(cuda_attributes_stmt);

   return ATtrue;
}
