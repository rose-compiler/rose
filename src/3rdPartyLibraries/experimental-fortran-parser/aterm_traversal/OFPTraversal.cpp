#include "OFPTraversal.hpp"
#include <iostream>

#define PRINT_ATERM_TRAVERSAL 0

using namespace OFP;

#ifdef TO_BE_REMOVED

static void fixupLocation(FAST::PosInfo & loc)
{
   int end_col = loc.getEndCol();

// make sure start column isn't the same as end col
   if (loc.getStartLine() == loc.getEndLine() && loc.getStartCol() == loc.getEndCol())
      {
         return;
      }

// check that end column isn't first column in the line
   if (end_col > 1)
      {
         loc.setEndCol(end_col - 1);
      }
}

static FAST::PosInfo getLocation(ATerm term)
{
   FAST::PosInfo pinfo;
   
   ATerm annotations = ATgetAnnotations(term);
   if (annotations) {
      int i1,i2,i3,i4;
      ATerm loc = ATgetFirst(annotations);
      if (ATmatch(loc, "Location(<int>,<int>,<int>,<int>)", &i1,&i2,&i3,&i4)) {
#if PRINT_ATERM_TRAVERSAL
         printf("... loc: %d %d %d %d\n", i1,i2,i3,i4);
#endif
         pinfo = FAST::PosInfo(i1,i2,i3,i4);
      }
   }
   fixupLocation(pinfo);

   return pinfo;
}

// This version can be used to drop eos (EndOfStmt) location
//
static FAST::PosInfo getLocation(ATerm startTerm, ATerm endTerm)
{
   FAST::PosInfo start = getLocation(startTerm);
   FAST::PosInfo   end = getLocation(  endTerm);

   return FAST::PosInfo(start.getStartLine(),start.getStartCol(),end.getEndLine(),end.getEndCol());
}

// This version is based on eos (EndOfStmt) location
//
static FAST::PosInfo getLocationFromEOS(ATerm startTerm, ATerm eosTerm)
{
   FAST::PosInfo start = getLocation(startTerm);
   FAST::PosInfo   end = getLocation(  eosTerm);

   return FAST::PosInfo(start.getStartLine(),start.getStartCol(),end.getStartLine(),end.getStartCol()-1);
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

// Traverse a use-stmt only list
//
ATbool traverse_OnlyList(ATerm term, std::vector<std::string> & only_list, std::map<std::string,std::string> & rename_map)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_OnlyList: %s\n", ATwriteToString(term));
#endif

  ATerm terms;
  char * str1, * str2;

  if (ATmatch(term, "OnlyList(<term>)", &terms)) {
     ATermList tail = (ATermList) ATmake("<term>", terms);
     while (! ATisEmpty(tail)) {
        ATerm head = ATgetFirst(tail);
        tail = ATgetNext(tail);
        if (ATmatch(head, "<str>", &str1)) {
           printf("--- Only: %s\n", str1);
           only_list.push_back(str1);
        }
        else if (ATmatch(head, "Rename(<str>,<str>)", &str1, &str2)) {
           printf("--- Rename: %s %s\n", str1, str2);
           rename_map[str1] = str2;
        }
        else return ATfalse;
     }
  }
  else return ATfalse;

  return ATtrue;
}


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
  FAST::ContainsStmt* contains_stmt;
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
    if (traverse_OptInternalSubprogramPart(term4, &contains_stmt, local_scope)) {
      // OptInternalSubprogramPart
    } else return ATfalse;
    if (traverse_EndProgramStmt(term5, &end_program_stmt)) {
      // EndProgramStmt
    } else return ATfalse;
  } else return ATfalse;

  *program = new FAST::MainProgram(program_stmt, local_scope,
                                   contains_stmt, end_program_stmt, getLocation(term));

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

    *var_OptProgramStmt = new FAST::ProgramStmt(label, name, eos, getLocation(term,term2));
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
  FAST::PosInfo pinfo;

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

  *var_EndProgramStmt = new FAST::EndProgramStmt(label, name, eos, getLocation(term,term2));

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
        scope->get_declaration_list().push_back(stmt);
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
  FAST::UseStmt::ModuleNature nature;
  std::string name;
  std::map<std::string,std::string> rename_map;
  std::vector<std::string> only_list;
  ATListToMap convert_list_type("Rename(<str>,<str>)");
  std::string eos;
  
  // Regular UseStmt without an ONLY clause
  //
  if (ATmatch(term, "UseStmt(<term>,<term>,<term>,<term>,<term>)", &term1,&term2,&term3,&term4,&term5)) {
    if (traverse_OptLabel(term1, label)) {
       // OptLabel
    } else return ATfalse;
    if (traverse_OptModuleNature(term2, &nature)) {
       // OptModuleNature
    } else return ATfalse;
    if (traverse_Name(term3, name)) {
       // ModuleName
    } else return ATfalse;
    if (traverse_OptCommaList(term4, convert_list_type)) {
       // RenameMap
       rename_map = convert_list_type.getMap();
       printf("--- rename list size %ld\n", rename_map.size());
    } else return ATfalse;
    if (traverse_eos(term5, eos)) {
       // EOS
    } else return ATfalse;
  }

  // UseStmt with an ONLY clause (may also have rename in list)
  //
  else if (ATmatch(term, "UseOnlyStmt(<term>,<term>,<term>,<term>,<term>)", &term1,&term2,&term3,&term4,&term5)) {
    if (traverse_OptLabel(term1, label)) {
       // OptLabel
    } else return ATfalse;
    if (traverse_OptModuleNature(term2, &nature)) {
       // OptModuleNature
    } else return ATfalse;
    if (traverse_Name(term3, name)) {
       // ModuleName
    } else return ATfalse;
    if (traverse_OnlyList(term4, only_list, rename_map)) {
       // OnlyList and RenameMap
       printf("--- rename map size %ld\n", rename_map.size());
       printf("--- only  list size %ld\n", only_list.size());
    } else return ATfalse;
    if (traverse_eos(term5, eos)) {
       // EOS
    } else return ATfalse;
  } else return ATfalse;

  *var_UseStmt = new FAST::UseStmt(label, name, nature, eos, getLocation(term));
  if (only_list.size() > 0) {
     (*var_UseStmt)->setOnlyList(only_list);
  }
  if (rename_map.size() > 0) {
     (*var_UseStmt)->setRenameMap(rename_map);
  }

  return ATtrue;
}

//========================================================================================
// OptModuleNature
//----------------------------------------------------------------------------------------
ATbool Traversal::traverse_OptModuleNature(ATerm term, FAST::UseStmt::ModuleNature* var_OptModuleNature)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_OptModuleNature: %s\n", ATwriteToString(term));
#endif

  ATerm term1;
  
  if (ATmatch(term, "no-module-nature()")) {
     // MATCHED no-module-nature
     *var_OptModuleNature = FAST::UseStmt::None;
  }
  else if (ATmatch(term, "ModuleNature(<term>)", &term1)) {
     if (ATmatch(term1, "INTRINSIC()")) {
        // MATCHED INTRINSIC
        *var_OptModuleNature = FAST::UseStmt::Intrinsic;
     }
     else if (ATmatch(term1, "NON_INTRINSIC()")) {
        // MATCHED NON_INTRINSIC
        *var_OptModuleNature = FAST::UseStmt::NonIntrinsic;
     } else return ATfalse;
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
         if (traverse_SpecStmt(head, scope)) {
            // MATCHED a declaration
            return ATtrue;
         }
         else if (traverse_ExecStmt(head, scope)) {
            // MATCHED an executable statement
            seen_first_exec_stmt = true;
            return ATtrue;
         } else return ATfalse;
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
ATbool Traversal::traverse_SpecStmt(ATerm term, FAST::Scope* scope)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_SpecStmt: %s\n", ATwriteToString(term));
#endif
  
  if (traverse_ImplicitStmt(term, scope)) {
     return ATtrue;
  }
  else if (traverse_TypeDeclarationStmt(term, scope)) {
     return ATtrue;
  }

  return ATfalse;
}

//========================================================================================
// Traverse executable statements
//----------------------------------------------------------------------------------------
ATbool Traversal::traverse_ExecStmt(ATerm term, FAST::Scope* scope)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_ExecStmt: %s\n", ATwriteToString(term));
#endif

  if (traverse_ContinueStmt(term, scope)) {
     return ATtrue;
  }

  return ATfalse;
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
   ImplicitSpecMatch(Traversal* traversal, std::vector<FAST::ImplicitSpec>* list)
      : pTraversal(traversal), pImplicitSpecList(list)
      {
      }
    ATbool operator() (ATerm term)
      {
         ATerm term1, term2;
         FAST::TypeSpec* type_spec;
         std::vector<FAST::LetterSpec> letter_spec_list;
         LetterSpecMatch match_letter_spec(&letter_spec_list);
         if (ATmatch(term, "ImplicitSpec(<term>,<term>)", &term1, &term2)) {
            if (pTraversal->traverse_DeclarationTypeSpec(term1, &type_spec)) {
            } else return ATfalse;
            if (traverse_List(term2, match_letter_spec)) {
            } else return ATfalse;
         } else return ATfalse;

         FAST::ImplicitSpec implicit_spec(type_spec, getLocation(term));
         pImplicitSpecList->push_back(implicit_spec);

         return ATtrue;
      }
 protected:
   Traversal* pTraversal;
   std::vector<FAST::ImplicitSpec>* pImplicitSpecList;
};

//========================================================================================
// AttrSpec
//----------------------------------------------------------------------------------------
class AttrSpecMatch
{
 public:
   AttrSpecMatch(std::vector<FAST::AttrSpec*>* list) : pAttrSpecList(list)
      {
      }
    ATbool operator() (ATerm term)
      {
         FAST::AttrSpec* attr_spec = NULL;

         if (ATmatch(term, "PUBLIC()")) {
            attr_spec = new FAST::AttrSpec(FAST::AttrSpec::Public, getLocation(term));
         } else return ATfalse;

         pAttrSpecList->push_back(attr_spec);

         return ATtrue;
      }
 protected:
   std::vector<FAST::AttrSpec*>* pAttrSpecList;
};

//========================================================================================
// EntityDecl
//----------------------------------------------------------------------------------------
class EntityDeclMatch
{
 public:
   EntityDeclMatch(std::vector<FAST::EntityDecl*>* list) : pEntityDeclList(list)
      {
      }
    ATbool operator() (ATerm term)
      {
         char* name;
         FAST::EntityDecl* entity_decl = NULL;

         if (ATmatch(term, "EntityDecl(<str>, no-list(), no-list(), no-char-length(), no-init())", &name)) {
            entity_decl = new FAST::EntityDecl(name, getLocation(term));
         } else return ATfalse;

         pEntityDeclList->push_back(entity_decl);

         return ATtrue;
      }
 protected:
   std::vector<FAST::EntityDecl*>* pEntityDeclList;
};

//========================================================================================
// DeclarationTypeSpec
//----------------------------------------------------------------------------------------
ATbool Traversal::traverse_DeclarationTypeSpec(ATerm term, FAST::TypeSpec** type_spec)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_DeclarationTypeSpec: %s\n", ATwriteToString(term));
#endif
  
  if (traverse_IntrinsicTypeSpec(term, type_spec)) {
     //MATCHED IntrinsicTypeSpec
  }
//TODO DerivedTypeSpec
  else return ATfalse;

  return ATtrue;
}

//========================================================================================
// IntrinsicTypeSpec
//----------------------------------------------------------------------------------------
ATbool Traversal::traverse_IntrinsicTypeSpec(ATerm term, FAST::TypeSpec** type_spec)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_IntrinsicTypeSpec: %s\n", ATwriteToString(term));
#endif
  
  if (ATmatch(term, "IntrinsicType(INTEGER())")) {
     *type_spec = new FAST::IntrinsicTypeSpec(FAST::TypeSpec::Integer, getLocation(term));
  }
  else return ATfalse;

  return ATtrue;
}

//========================================================================================
// ImplicitStmt
//----------------------------------------------------------------------------------------
ATbool Traversal::traverse_ImplicitStmt(ATerm term, FAST::Scope* scope)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_ImplicitStmt: %s\n", ATwriteToString(term));
#endif
  
  ATerm term1, term2, term_eos;
  std::string label;
  std::string eos;
  std::vector<FAST::ImplicitSpec> spec_list;
  ImplicitSpecMatch match_spec(this, &spec_list);
  FAST::PosInfo pinfo;

  FAST::ImplicitStmt* implicit_stmt = NULL;

  if (ATmatch(term, "ImplicitNoneStmt(<term>,<term>)", &term1,&term_eos)) {
    if (traverse_OptLabel(term1, label)) {
      // MATCHED OptLabel
    } else return ATfalse;
    if (traverse_eos(term_eos, eos)) {
      // MATCHED eos string
    } else return ATfalse;
  }

  else if (ATmatch(term, "ImplicitStmt(<term>,<term>,<term>)", &term1,&term2,&term_eos)) {
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
  }
  else return ATfalse;

  implicit_stmt = new FAST::ImplicitStmt(label, spec_list, eos, getLocationFromEOS(term,term_eos));
  scope->get_declaration_list().push_back(implicit_stmt);

  return ATtrue;
}

//========================================================================================
// TypeDeclarationStmt
//----------------------------------------------------------------------------------------
ATbool Traversal::traverse_TypeDeclarationStmt(ATerm term, FAST::Scope* scope)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_TypeDeclarationStmt: %s\n", ATwriteToString(term));
#endif
  
  ATerm term1, term2, term3, term4, term_eos;
  std::string label;
  std::string eos;
  FAST::TypeSpec* type_spec;
  std::vector<FAST::AttrSpec*> attr_list;
  std::vector<FAST::EntityDecl*> entity_decl_list;
  AttrSpecMatch match_attrs(&attr_list);
  EntityDeclMatch match_entity_decls(&entity_decl_list);

  FAST::TypeDeclarationStmt* type_decl_stmt = NULL;

  if (ATmatch(term, "TypeDeclarationStmt(<term>,<term>,<term>,<term>,<term>)", &term1,&term2,&term3,&term4,&term_eos)) {
    if (traverse_OptLabel(term1, label)) {
      // MATCHED OptLabel
    } else return ATfalse;
    if (traverse_DeclarationTypeSpec(term2, &type_spec)) {
      // MATCHED DeclarationTypeSpec
    } else return ATfalse;
    if (traverse_OptCommaList(term3, match_attrs)) {
      // MATCHED AttrSpecList
    } else return ATfalse;
    if (traverse_List(term4, match_entity_decls)) {
      // MATCHED EntityDeclList
    } else return ATfalse;
    if (traverse_eos(term_eos, eos)) {
      // MATCHED eos string
    } else return ATfalse;
  }
  else return ATfalse;

  type_decl_stmt = new FAST::TypeDeclarationStmt(label, type_spec, attr_list, entity_decl_list, eos, getLocationFromEOS(term,term_eos));
  scope->get_declaration_list().push_back(type_decl_stmt);

  return ATtrue;
}

//========================================================================================
// OptInternalSubprogramPart
//----------------------------------------------------------------------------------------
ATbool Traversal::traverse_OptInternalSubprogramPart(ATerm term, FAST::ContainsStmt** contains_stmt, FAST::Scope* scope)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_OptInternalSubprogramPart: %s\n", ATwriteToString(term));
#endif
  
  ATerm term1, term2;

  if (ATmatch(term, "no-subprogram-part()")) {
     // MATCHED no-subprogram-part
  } else if (ATmatch(term, "SubprogramPart(<term>,<term>)", &term1,&term2)) {
    if (traverse_ContainsStmt(term1, contains_stmt)) {
      // MATCHED ContainsStmt
    } else return ATfalse;
#ifdef TODO
    if (traverse_List(term4, match_internal_subprogram)) {
      // MATCHED InternalSubprogram
    } else return ATfalse;
#endif
  } else return ATfalse;

  return ATtrue;
}

//========================================================================================
// ContinueStmt
//----------------------------------------------------------------------------------------
ATbool Traversal::traverse_ContinueStmt(ATerm term, FAST::Scope* scope)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_ContinueStmt: %s\n", ATwriteToString(term));
#endif

  ATerm term1, term_eos;
  std::string label;
  std::string eos;

  FAST::ContinueStmt* continue_stmt = NULL;

  if (ATmatch(term, "ContinueStmt(<term>,<term>)", &term1,&term_eos)) {
    if (traverse_OptLabel(term1, label)) {
      // MATCHED OptLabel
    } else return ATfalse;
    if (traverse_eos(term_eos, eos)) {
      // MATCHED eos string
    } else return ATfalse;
  }
  else return ATfalse;

  continue_stmt = new FAST::ContinueStmt(label, eos, getLocationFromEOS(term,term_eos));
  scope->get_declaration_list().push_back(continue_stmt);

  return ATtrue;
}

//========================================================================================
// ContainsStmt
//----------------------------------------------------------------------------------------
ATbool Traversal::traverse_ContainsStmt(ATerm term, FAST::ContainsStmt** contains_stmt)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_ContainsStmt: %s\n", ATwriteToString(term));
#endif
  
  ATerm term1, term_eos;
  std::string label;
  std::string eos;

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

  *contains_stmt = new FAST::ContainsStmt(label, eos, getLocationFromEOS(term,term_eos));

  return ATtrue;
}
#endif
