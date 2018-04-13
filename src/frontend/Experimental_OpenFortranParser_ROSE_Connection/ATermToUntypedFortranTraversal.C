#include "sage3basic.h"

#include "ATermToUntypedFortranTraversal.h"
#include "general_language_translation.h"
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
      std::cout << "--- Matched: " << match << " " << str1 << " " << str2 << std::endl;
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

   std::cout << "TODO - traverse_DefinedOperator \n";

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

   if (ATmatch(term, "no-implicit-part()")) {
      // MATCHED no-implicit-part
   }
   else if (traverse_ImplicitPartStmtList(term, decl_list)) {
      // MATCHED ImplicitPartStmt list
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
   printf("... traverse_ImplicitPartStmtList: %s\n", ATwriteToString(term));
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

#if 0
   if (traverse_DerivedTypeDef(term, decl_list)) {
      // MATCHED DerivedTypeDef
   }
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
   else if (traverse_ImplicitStmt(term, decl_list)) {
      // Matched ImplicitStmt
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
   //  TypeDeclarationStmt                    -> DeclarationConstruct

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
   else if (traverse_ContinueStmt(term, stmt_list)) {
      // Matched ContinueStmt
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
            if (pTraversal->traverse_DeclarationTypeSpec(term1, &type_spec)) {
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
ATbool ATermToUntypedFortranTraversal::traverse_DeclarationTypeSpec(ATerm term, SgUntypedType** type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_DeclarationTypeSpec: %s\n", ATwriteToString(term));
#endif
  
   if (traverse_IntrinsicTypeSpec(term, type)) {
      //MATCHED IntrinsicTypeSpec
   }

   else {
      //TODO DerivedTypeSpec
      std::cerr << "...TODO... implement DerivedTypeSpec" << std::endl;
      return ATfalse;
   }

  return ATtrue;
}

//========================================================================================
// IntrinsicTypeSpec
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_IntrinsicTypeSpec(ATerm term, SgUntypedType** type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_IntrinsicTypeSpec: %s\n", ATwriteToString(term));
#endif
  
   ATerm t_type, t_kind;

   *type = NULL;

   if (ATmatch(term, "IntrinsicType(<term>)", &t_type)) {
      // MATCHED IntrinsicType
   } else return ATfalse;

// Check for type declarations without type-kind parameter
   if (ATmatch(t_type, "INTEGER()")) {
      *type = UntypedBuilder::buildType(SgUntypedType::e_int);
   }
   else if (ATmatch(t_type, "REAL()")) {
      *type = UntypedBuilder::buildType(SgUntypedType::e_float);
   }
   else if (ATmatch(t_type, "DOUBLEPRECISION()")) {
      //*type = UntypedBuilder::buildType(SgUntypedType::e_float);
   }
   else if (ATmatch(t_type, "COMPLEX()")) {
      //*type = UntypedBuilder::buildType(SgUntypedType::e_complex);
   }
   else if (ATmatch(t_type, "CHARACTER()")) {
      *type = UntypedBuilder::buildType(SgUntypedType::e_char);
   }
   else if (ATmatch(t_type, "LOGICAL()")) {
      *type = UntypedBuilder::buildType(SgUntypedType::e_bool);
   }
   else if (ATmatch(t_type, "DOUBLECOMPLEX()")) {
      //*type = UntypedBuilder::buildType(SgUntypedType::e_float);
   }

   if (*type != NULL) {
   // have a type without a type-kind parameter, return
      return ATtrue;
   }

// Only type declarations with a kind parameter or length parameters left
// ----------------------------------------------------------------------

   if (ATmatch(t_type, "INTEGER(<term>)", &t_kind)) {
      *type = UntypedBuilder::buildType(SgUntypedType::e_int);
   }
   else if (ATmatch(t_type, "REAL(<term>)", &t_kind)) {
      *type = UntypedBuilder::buildType(SgUntypedType::e_float);
   }
   else if (ATmatch(t_type, "COMPLEX(<term>)", &t_kind)) {
      //*type = UntypedBuilder::buildType(SgUntypedType::e_complex);
      return ATfalse;
   }
   else if (ATmatch(t_type, "LOGICAL(<term>)", &t_kind)) {
      *type = UntypedBuilder::buildType(SgUntypedType::e_bool);
   }

// Check for a type-kind parameter
   if (*type != NULL) {
      SgUntypedExpression* kind;
      if (traverse_KindSelector(t_kind, &kind)) {
      // add type kind expression
         (*type)->set_has_kind(true);
         (*type)->set_type_kind(kind);
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
   if (*type != NULL) {
   // have a type with a type-kind parameter, return
      return ATtrue;
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
      // MATCHED IntVal
      value += arg1;
      type = UntypedBuilder::buildType(SgUntypedType::e_int);
   }
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

   General_Language_Translation::ExpressionKind op_enum;
   std::string op_name;
   SgUntypedExpression* lhs;
   SgUntypedExpression* rhs;

   *var_expr = NULL;

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

      if (traverse_DeclarationTypeSpec(term2, &declared_type)) {
         // MATCHED DeclarationTypeSpec
      } else return ATfalse;

      attr_list = new SgUntypedExprListExpression();
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

   std::cerr << "...TODO... fully implement AttrSpecList in TypeDeclarationStmt" << std::endl;

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
   General_Language_Translation::ExpressionKind expr_enum = General_Language_Translation::e_unknown;
   int rank = 0;

   *array_type = NULL;

   if (ATmatch(term, "ArraySpec(<term>)", &t_array_spec_arg)) {

      dim_info = new SgUntypedExprListExpression(General_Language_Translation::e_array_shape);
      setSourcePosition(dim_info, t_array_spec_arg);

   // check for non-list array-spec term first, e.g., dimension A(*)
   // 
      if (traverse_AssumedOrImpliedSpec(t_array_spec_arg, &lower_bound)) {
         upper_bound = new SgUntypedNullExpression();
         setSourcePositionUnknown(upper_bound);

         stride = new SgUntypedNullExpression();
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

            stride = new SgUntypedNullExpression();
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

            upper_bound = new SgUntypedNullExpression();
            setSourcePositionUnknown(upper_bound);

            stride = new SgUntypedNullExpression();
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
         *lower_bound = new SgUntypedNullExpression();
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
         *lower_bound = new SgUntypedNullExpression();
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

   SgUntypedExprListExpression* dim_info = new SgUntypedExprListExpression(General_Language_Translation::e_array_shape);
   setSourcePosition(dim_info, term);

   if (ATmatch(term, "AssumedSize(<term>,<term>)", &t_explicit_shape_list, &t_lower_bound)) {

   // traverse the list of explicit shape dimension before the final '*'
      if (traverse_ExplicitShapeList(t_explicit_shape_list, dim_info)) {
         // list of ExplicitShape terms are added to dim_info
      } else return ATfalse;

   // match the final dimension which makes it assumed-size, i.e., '*'
      if (ATmatch(t_lower_bound, "no-lower-bound()")) {
         lower_bound = new SgUntypedNullExpression();
         setSourcePositionUnknown(lower_bound);
      }
      else if (traverse_Expression(t_lower_bound, &lower_bound)) {
      } else return ATfalse;

      upper_bound = new SgUntypedNullExpression();
      setSourcePositionUnknown(upper_bound);

      stride = new SgUntypedNullExpression();
      setSourcePositionUnknown(stride);

      int expr_enum = General_Language_Translation::e_assumed_size;
      SgUntypedSubscriptExpression* range = new SgUntypedSubscriptExpression(expr_enum, lower_bound, upper_bound, stride);
      setSourcePosition(range, t_lower_bound);

      dim_info->get_expressions().push_back(range);
      dim_info->get_expressions().size();

   } else return ATfalse;

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
         *lower_bound = new SgUntypedNullExpression();
         setSourcePositionUnknown(*lower_bound);
      }
      else if (traverse_Expression(t_lower_bound, lower_bound)) {
      } else return ATfalse;
      
   } else return ATfalse;

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

   ATerm term1, term2, term3;
   char* arg1;

   std::string name;
   SgToken::ROSE_Fortran_Keywords keyword = SgToken::FORTRAN_UNKNOWN;

   *var_expr = NULL;
   if (ATmatch(term, "PartRef(<term>,<term>,<term>)", &term1,&term2,&term3)) {
      if (ATmatch(term1, "<str>", &arg1)) {
         // MATCHED string
         name += arg1;
      } else return ATfalse;
      if (traverse_OptSectionSubscripts(term2)) {
         //TODO_SgUntyped - need way to handle list
      } else return ATfalse;
      if (traverse_OptImageSelector(term3)) {
         //TODO_SgUntyped - need way to handle list
      } else return ATfalse;

      *var_expr = new SgUntypedReferenceExpression(keyword, name);
      setSourcePosition(*var_expr, term);
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// R620 section-subscript
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_OptSectionSubscripts(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptSectionSubscripts: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-section-subscripts()")) {
      // MATCHED empty list
   }
   //TODO_SgUntyped - (paren-list)
   //TODO_SgUntyped - (substring-section-range)
   //TODO_SgUntyped - (function-ref-no-args)
   //TODO_SgUntyped - needs section subscripts
   else {
      std::cerr << "...TODO... implement OptSectionSubscripts" << std::endl;
      return ATfalse;
   }

   return ATtrue;
}

//========================================================================================
// R624 image-selector
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_OptImageSelector(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptImageSelector: %s\n", ATwriteToString(term));
#endif

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
      *expr = new SgUntypedNullExpression();
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

   SgToken::ROSE_Fortran_Keywords keyword = SgToken::FORTRAN_STOP;

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

   SgUntypedExpressionStatement* stop_stmt = new SgUntypedExpressionStatement(label, keyword, stop_code);
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
      *stop_code = new SgUntypedNullExpression();
      setSourcePositionUnknown(*stop_code);
   }
   else if (traverse_Expression(term, stop_code)) {
      // MATCHED StopCode
   }
   else return ATfalse;

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

  SgToken::ROSE_Fortran_Keywords keyword = SgToken::FORTRAN_PROGRAM;

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

    *program_stmt = new SgUntypedNamedStatement(label,keyword,name);
    setSourcePositionExcludingTerm(*program_stmt, term, term_eos);
  }
  else if (ATmatch(term, "no-program-stmt()")) {
   // There is no program-stmt so mark the name as special so that ROSE
   // knows that the statement is implied/compiler generated.
      name += "rose_fortran_main";
      *program_stmt = new SgUntypedNamedStatement(label,keyword,name);
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

   SgToken::ROSE_Fortran_Keywords keyword = SgToken::FORTRAN_END_PROGRAM;

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

   *end_program_stmt = new SgUntypedNamedStatement(label,keyword,name);
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
// ImportStmt (R1209)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_ImportStmt(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ImportStmt: %s\n", ATwriteToString(term));
#endif

   ATerm term1, term2, eos_term;
   std::string label;
   std::string eos;
  
   SgUntypedNameList* name_list;
   SgUntypedNameListDeclaration* import_stmt;

   if (ATmatch(term, "ImportStmt(<term>,<term>,<term>)", &term1,&term2,&eos_term))
   {
      if (traverse_OptLabel(term1, label)) {
         //matched OptLabel
      } else return ATfalse;

      name_list = new SgUntypedNameList();
      setSourcePosition(name_list, term2);

      if (traverse_NameList(term2, name_list)) {
         //matched NameList
      } else return ATfalse;
      if (traverse_eos(eos_term, eos)) {
         //matched EOS
      } else return ATfalse;
   }
   else return ATfalse;

   import_stmt = new SgUntypedNameListDeclaration(label, SgToken::FORTRAN_IMPORT, name_list);
   setSourcePositionExcludingTerm(import_stmt, term, eos_term);

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
// ExternalStmt (R1210)
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
// Prefix (R1225)
//----------------------------------------------------------------------------------------
ATbool ATermToUntypedFortranTraversal::traverse_OptPrefix(ATerm term, SgUntypedExprListExpression* prefix_list, SgUntypedType** type)
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

         if (traverse_OptPrefix(prefix_term, prefix_list, &function_type)) {
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

         if (traverse_OptPrefix(prefix_term, prefix_list, &function_type)) {
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

   SgToken::ROSE_Fortran_Keywords keyword = SgToken::FORTRAN_RETURN;

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

   SgUntypedExpressionStatement* return_stmt = new SgUntypedExpressionStatement(label, keyword, expr);
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
