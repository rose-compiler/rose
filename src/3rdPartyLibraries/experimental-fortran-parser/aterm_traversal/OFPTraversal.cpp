#include "OFPTraversal.hpp"

#define PRINT_ATERM_TRAVERSAL 0

using namespace OFP;

//========================================================================================
// Program
//----------------------------------------------------------------------------------------
ATbool Traversal::traverse_Program(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Program: %s\n", ATwriteToString(term));
#endif

   FAST::Scope* fast_global_scope = new FAST::Scope();

   ATerm term1, term2;
   std::string arg1;

   if (ATmatch(term, "Program(<term>,<term>)", &term1,&term2)) {
      if (traverse_OptStartCommentBlock(term1, arg1)) {
         // MATCHED OptStartCommentBlock
      } else return ATfalse;

      if (traverse_ListPlusOfProgramUnit(term2, fast_global_scope)) {
         // MATCHED ListPlusOfProgramUnit
      } else return ATfalse;

   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// OptStartCommentBlock
//----------------------------------------------------------------------------------------
ATbool Traversal::traverse_OptStartCommentBlock(ATerm term, std::string & var_OptStartCommentBlock)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptStartCommentBlock: %s\n", ATwriteToString(term));
#endif

   char* arg1;

   if (ATmatch(term, "no-comments()")) {
      // MATCHED no-comments
   }
   else if (ATmatch(term, "<str>", &arg1)) {
      // MATCHED string
      var_OptStartCommentBlock += arg1;
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// ListPlusOfProgramUnit
//----------------------------------------------------------------------------------------
ATbool Traversal::traverse_ListPlusOfProgramUnit(ATerm term, FAST::Scope* scope)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_ListPlusOfProgramUnit: %s\n", ATwriteToString(term));
#endif

  ATermList tail = (ATermList) ATmake("<term>", term);
  if (ATisEmpty(tail)) {
     fprintf(stderr, "WARNING, ProgramUnit list is empty \n");
     return ATfalse;
  }

  while (! ATisEmpty(tail)) {
     ATerm head = ATgetFirst(tail);
     tail = ATgetNext(tail);

     FAST::MainProgram* main_program;
     if (traverse_MainProgram(head, &main_program)) {
        // MATCHED MainProgram
        converter->convert_MainProgram(main_program);
     } else return ATfalse;

   //TODO - traverse other program-units
  }

  return ATtrue;
}

//========================================================================================
// MainProgram
//----------------------------------------------------------------------------------------
ATbool Traversal::traverse_MainProgram(ATerm term, FAST::MainProgram** program)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_MainProgram: %s\n", ATwriteToString(term));
#endif

  ATerm term1, term2, term3, term4, term5;
  FAST::ProgramStmt* program_stmt;
  FAST::EndProgramStmt* end_program_stmt;
  
  FAST::Scope* local_scope = new FAST::Scope();

  if (ATmatch(term, "MainProgram(<term>,<term>,<term>,<term>,<term>)", &term1,&term2,&term3,&term4,&term5)) {
    if (traverse_OptProgramStmt(term1, &program_stmt)) {
      // OptProgramStmt
    } else return ATfalse;
    if (traverse_InitialSpecPart(term2, local_scope)) {
      // InitialSpecPart
    } else return ATfalse;
    if (traverse_SpecAndExecPart(term3, local_scope)) {
      // SpecAndExecPart
    } else return ATfalse;
    if (traverse_OptInternalSubprogramPart(term4, local_scope)) {
      // OptInternalSubprogramPart
    } else return ATfalse;
    if (traverse_EndProgramStmt(term5, &end_program_stmt)) {
      // EndProgramStmt
    } else return ATfalse;
  } else return ATfalse;

  *program = new FAST::MainProgram(program_stmt, local_scope, NULL/*contains*/, end_program_stmt);

  return ATtrue;
}

//========================================================================================
// OptProgramStmt
//----------------------------------------------------------------------------------------
ATbool Traversal::traverse_OptProgramStmt(ATerm term, FAST::ProgramStmt** var_OptProgramStmt)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_OptProgramStmt: %s\n", ATwriteToString(term));
#endif
  
  ATerm term1, term2, term3;
  std::string label;
  std::string name;
  std::string eos;

  *var_OptProgramStmt = NULL;
  if (ATmatch(term, "ProgramStmt(<term>,<term>,<term>)", &term1,&term2,&term3)) {
    if (traverse_OptLabel(term1, label)) {
      // MATCHED OptLabel
    } else return ATfalse;
    if (traverse_Name(term2, name)) {
      // MATCHED ProgramName string
    } else return ATfalse;
    if (traverse_eos(term3, eos)) {
      // MATCHED eos string
    } else return ATfalse;
    *var_OptProgramStmt = new FAST::ProgramStmt(label, name, eos);
  }
  else if (ATmatch(term, "no-program-stmt()")) {
     // MATCHED no-program-stmt
  } else return ATfalse;

  return ATtrue;
}

//========================================================================================
// EndProgramStmt
//----------------------------------------------------------------------------------------
ATbool Traversal::traverse_EndProgramStmt(ATerm term, FAST::EndProgramStmt** var_EndProgramStmt)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_EndProgramStmt: %s\n", ATwriteToString(term));
#endif
  
  ATerm term1, term2, term3;
  std::string label;
  std::string name;
  std::string eos;

  *var_EndProgramStmt = NULL;
  if (ATmatch(term, "EndProgramStmt(<term>,<term>,<term>)", &term1,&term2,&term3)) {
    if (traverse_OptLabel(term1, label)) {
      // MATCHED OptLabel
    } else return ATfalse;
    if (traverse_OptProgramName(term2, name)) {
      // MATCHED ProgramName string
    } else return ATfalse;
    if (traverse_eos(term3, eos)) {
      // MATCHED eos string
    } else return ATfalse;
  } else return ATfalse;

  *var_EndProgramStmt = new FAST::EndProgramStmt(label, name, eos);

  return ATtrue;
}

//========================================================================================
// OptLabel
//----------------------------------------------------------------------------------------
ATbool Traversal::traverse_OptLabel(ATerm term, std::string & var_OptLabel)
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
ATbool Traversal::traverse_Name(ATerm term, std::string & var_Name)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Name: %s\n", ATwriteToString(term));
#endif
  
   char* arg1;

   if (ATmatch(term, "<str>", &arg1)) {
      // MATCHED string
      var_Name += arg1;
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// OptProgramName
//----------------------------------------------------------------------------------------
ATbool Traversal::traverse_OptProgramName(ATerm term, std::string & var_OptProgramName)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_OptProgramName: %s\n", ATwriteToString(term));
#endif

  char* arg1;
  
  if (ATmatch(term, "no-program-name()")) {
    // MATCHED no-comments
  }
  else if (ATmatch(term, "<str>", &arg1)) {
      var_OptProgramName += arg1;
  }
  else return ATfalse;

  return ATtrue;
}

//========================================================================================
// eos
//----------------------------------------------------------------------------------------
ATbool Traversal::traverse_eos(ATerm term, std::string & var_eos)
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
ATbool Traversal::traverse_InitialSpecPart(ATerm term, FAST::Scope* scope)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_InitialSpecPart: %s\n", ATwriteToString(term));
#endif

   ATerm use_stmts;
   ATerm import_stmts;
  
   if (ATmatch(term, "initial-spec-part(<term>,<term>)", &use_stmts, &import_stmts)) {
      if (traverse_ListStarOfUseStmt(use_stmts, scope)) {
         // ListStarOfUseStmt
      } else return ATfalse;
#if 0 //TODO
      if (traverse_ListStarOfImportStmt(term2, scope)) {
         // ListStarOfImportStmt
      } else return ATfalse;
#endif
   } else return ATfalse;

   return ATtrue;
}

//========================================================================================
// ListStarOfUseStmt
//----------------------------------------------------------------------------------------
ATbool Traversal::traverse_ListStarOfUseStmt(ATerm term, FAST::Scope* scope)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_ListStarOfUseStmt: %s\n", ATwriteToString(term));
#endif

  FAST::UseStmt* stmt;

  ATermList tail = (ATermList) ATmake("<term>", term);
  while (! ATisEmpty(tail)) {
     ATerm head = ATgetFirst(tail);
     tail = ATgetNext(tail);
     if (traverse_UseStmt(head, &stmt)) {
        // MATCHED UseStmt
     } else return ATfalse;
  }

  return ATtrue;
}

//========================================================================================
// UseStmt
//----------------------------------------------------------------------------------------
ATbool Traversal::traverse_UseStmt(ATerm term, FAST::UseStmt** var_UseStmt)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_UseStmt: %s\n", ATwriteToString(term));
#endif

  ATerm term1, term2, term3, term4, term5;
  std::string label;
#if 0 //TODO
  OptModuleNature* arg2;
  std::string name;
  OptOnlyList* arg4;
  std::string eos;
#endif
  
  if (ATmatch(term, "UseStmt(<term>,<term>,<term>,<term>,<term>)", &term1,&term2,&term3,&term4,&term5)) {
    if (traverse_OptLabel(term1, label)) {
      // OptLabel
    } else return ATfalse;
#if 0 //TODO
    if (traverse_OptModuleNature(term2, &arg2)) {
      // OptModuleNature
    } else return ATfalse;
    if (traverse_ModuleName(term3, name)) {
      // ModuleName
    } else return ATfalse;
    if (traverse_OptOnlyList(term4, &arg4)) {
      // OptOnlyList
    } else return ATfalse;
    if (traverse_EOS(term5, eos)) {
      // EOS
    } else return ATfalse;
#endif
  } else return ATfalse;

  return ATtrue;
}

//========================================================================================
// SpecAndExecPart
//----------------------------------------------------------------------------------------
ATbool Traversal::traverse_SpecAndExecPart(ATerm term, FAST::Scope* scope)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_SpecAndExecPart: %s\n", ATwriteToString(term));
#endif

  ATerm term1;
  
  if (ATmatch(term, "SpecAndExecPart(<term>)", &term1)) {
    if (traverse_ListStarOfSpecAndExecConstruct(term1, scope)) {
      // ListStarOfSpecAndExecConstruct
    } else return ATfalse;
  } else return ATfalse;

  return ATtrue;
}

//========================================================================================
// ListStarOfSpecAndExecConstruct
//----------------------------------------------------------------------------------------
ATbool Traversal::traverse_ListStarOfSpecAndExecConstruct(ATerm term, FAST::Scope* scope)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ListStarOfSpecAndExecConstruct: %s\n", ATwriteToString(term));
#endif

   bool seen_first_exec_stmt = false;

   ATermList tail = (ATermList) ATmake("<term>", term);
   while (! ATisEmpty(tail)) {
      ATerm head = ATgetFirst(tail);
      tail = ATgetNext(tail);

      if (seen_first_exec_stmt == false) {
#if 0 //TODO
         if (traverse_SpecStmt(head, scope, &seen_first_exec_stmt)) {
            // MATCHED a declaration
         } else return ATfalse;
#endif
      }
      else {
#if 0 //TODO
         if (traverse_SpecOrExecStmt(head, scope)) {
            // MATCHED a specification/executable statement
         } else return ATfalse;
#endif
      }
   }

   return ATtrue;
}

//========================================================================================
// OptInternalSubprogramPart
//----------------------------------------------------------------------------------------
ATbool Traversal::traverse_OptInternalSubprogramPart(ATerm term, FAST::Scope* scope)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_OptInternalSubprogramPart: %s\n", ATwriteToString(term));
#endif
  
  if (ATmatch(term, "no-subprogram-part()")) {
     // MATCHED no-subprogram-part
  } else if (false) {
     //TODO match subprogram-part
     // starts with contains-stmt
  } else return ATfalse;

  return ATtrue;
}

