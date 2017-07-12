#include "sage3basic.h"

#include "ATtoUntypedTraversal.h"
#include <iostream>

#define PRINT_ATERM_TRAVERSAL 0
#define SET_SOURCE_POSITION_UNKNOWN 1

using namespace OFP;

ATtoUntypedTraversal::ATtoUntypedTraversal(SgSourceFile* source)
{
   SgUntypedDeclarationStatementList* sg_decls = new SgUntypedDeclarationStatementList();
   SgUntypedStatementList*            sg_stmts = new SgUntypedStatementList();
   SgUntypedFunctionDeclarationList*  sg_funcs = new SgUntypedFunctionDeclarationList();
   SgUntypedGlobalScope*          global_scope = new SgUntypedGlobalScope("",SgToken::FORTRAN_UNKNOWN,sg_decls,sg_stmts,sg_funcs);

   pSourceFile = source;
   ROSE_ASSERT(pSourceFile != NULL);

   pUntypedFile = new SgUntypedFile(global_scope);
   ROSE_ASSERT(pUntypedFile != NULL);

// DQ (2/25/2013): Set the default for source position generation to be consistent with other languages (e.g. C/C++).
   SageBuilder::setSourcePositionClassificationMode(SageBuilder::e_sourcePositionFrontendConstruction);
//TODO     SageBuilder::setSourcePositionClassificationMode(SageBuilder::e_sourcePositionCompilerGenerated);
}

ATtoUntypedTraversal::~ATtoUntypedTraversal()
{
   delete pUntypedFile;
}

void
ATtoUntypedTraversal::setSourcePositionUnknown(SgLocatedNode* locatedNode)
{
  // This function sets the source position to be marked as not available (since we don't have token information)
  // These nodes WILL be unparsed in the code generation phase.

#if DEBUG_UNTYPED_CONVERTER
     printf ("UntypedConverter::setSourcePositionUnknown: locatedNode = %p = %s \n",locatedNode,locatedNode->class_name().c_str());
#endif

#if 0
  // The SgLocatedNode has both a startOfConstruct and endOfConstruct source position.
     ROSE_ASSERT(locatedNode != NULL);

  // Make sure we never try to reset the source position of the global scope (set elsewhere in ROSE).
     ROSE_ASSERT(isSgGlobal(locatedNode) == NULL);

  // Check the endOfConstruct first since it is most likely NULL (helpful in debugging)
     if (locatedNode->get_endOfConstruct() != NULL || locatedNode->get_startOfConstruct() != NULL)
        {
        // TODO - figure out if anything needs to be done here
        // printf ("In setSourcePositionUnknown: source position known locatedNode = %p = %s \n",locatedNode,locatedNode->class_name().c_str());
        }
     else
        {
           ROSE_ASSERT(locatedNode->get_endOfConstruct()   == NULL);
           ROSE_ASSERT(locatedNode->get_startOfConstruct() == NULL);
           SageInterface::setSourcePosition(locatedNode);
        }
#endif
}

#if 0
//TODO - fix runtime errors
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
#endif

#if 0
//TODO - fix runtime errors
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
#endif

// This version can be used to drop eos (EndOfStmt) location
//
#ifdef REPLACE_WITH_SgUntyped
static FAST::PosInfo getLocation(ATerm startTerm, ATerm endTerm)
{
   FAST::PosInfo start = getLocation(startTerm);
   FAST::PosInfo   end = getLocation(  endTerm);

   return FAST::PosInfo(start.getStartLine(),start.getStartCol(),end.getEndLine(),end.getEndCol());
}
#endif

// This version is based on eos (EndOfStmt) location
//
#ifdef REPLACE_WITH_SgUntyped
static FAST::PosInfo getLocationFromEOS(ATerm startTerm, ATerm eosTerm)
{
   FAST::PosInfo start = getLocation(startTerm);
   FAST::PosInfo   end = getLocation(  eosTerm);

   return FAST::PosInfo(start.getStartLine(),start.getStartCol(),end.getStartLine(),end.getStartCol()-1);
}
#endif

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
// Program (R201)
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_Program(ATerm term)
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
ATbool ATtoUntypedTraversal::traverse_ProgramUnitList(ATerm term, SgUntypedGlobalScope* scope)
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
ATbool ATtoUntypedTraversal::traverse_OptInternalSubprogramPart(ATerm term, SgUntypedOtherStatement** contains_stmt, SgUntypedScope* scope)
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
ATbool ATtoUntypedTraversal::traverse_InternalSubprogramList(ATerm term, SgUntypedScope* scope)
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
ATbool ATtoUntypedTraversal::traverse_StartCommentBlock(ATerm term, std::string & var_StartCommentBlock)
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
ATbool ATtoUntypedTraversal::traverse_OptLabel(ATerm term, std::string & var_OptLabel)
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
// TODO - I think this can go away in OFP/FAST
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_Name(ATerm term, std::string & name)
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
ATbool ATtoUntypedTraversal::traverse_OptName(ATerm term, std::string & name)
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
ATbool ATtoUntypedTraversal::traverse_NameList(ATerm term, std::vector<std::string> & name_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_NameList: %s\n", ATwriteToString(term));
#endif
  
   ATerm list;
   char* arg1;

   if (ATmatch(term, "NameList(<term>)", &list)) {
      ATermList tail = (ATermList) ATmake("<term>", list);
      while (! ATisEmpty(tail)) {
         ATerm head = ATgetFirst(tail);
         tail = ATgetNext(tail);
         if (ATmatch(head, "<str>", &arg1)) {
            // Matched a name
            name_list.push_back(arg1);
         } else return ATfalse;
      }
   }

   return ATtrue;
}

//========================================================================================
// eos
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_eos(ATerm term, std::string & var_eos)
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
ATbool ATtoUntypedTraversal::traverse_InitialSpecPart(ATerm term, SgUntypedDeclarationStatementList* decl_list)
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
// UseStmtList
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_UseStmtList(ATerm term, SgUntypedDeclarationStatementList* decl_list)
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
// UseStmt
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_UseStmt(ATerm term, SgUntypedDeclarationStatementList* decl_list)
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

#ifdef REPLACE_WITH_SgUntyped
  *var_UseStmt = new FAST::UseStmt(label, name, nature, eos, getLocation(term));
  scope->get_declaration_list().push_back(stmt);

  if (only_list.size() > 0) {
     (*var_UseStmt)->setOnlyList(only_list);
  }
  if (rename_map.size() > 0) {
     (*var_UseStmt)->setRenameMap(rename_map);
  }
#endif

  return ATtrue;
}

//========================================================================================
// OptModuleNature
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_OptModuleNature(ATerm term, FAST::UseStmt::ModuleNature* var_OptModuleNature)
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
ATbool ATtoUntypedTraversal::traverse_SpecAndExecPart(ATerm term, SgUntypedDeclarationStatementList* decl_list,
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
// ExecutionPartConstruct
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_ExecutionPartConstruct(ATerm term, SgUntypedStatementList* stmt_list)
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
   else return ATfalse;

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
ATbool ATtoUntypedTraversal::traverse_SpecStmt(ATerm term, SgUntypedDeclarationStatementList* decl_list)
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
   else {
      return ATfalse;
   }

   return ATtrue;
}

//========================================================================================
// Traverse executable statements
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_ExecStmt(ATerm term, SgUntypedStatementList* stmt_list)
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
   ImplicitSpecMatch(ATtoUntypedTraversal* traversal, std::vector<FAST::ImplicitSpec>* list)
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
   ATtoUntypedTraversal* pTraversal;
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
#ifdef REPLACE_WITH_SgUntyped
            attr_spec = new FAST::AttrSpec(FAST::AttrSpec::Public, getLocation(term));
#endif
         } else return ATfalse;

         pAttrSpecList->push_back(attr_spec);

         return ATtrue;
      }
 protected:
   std::vector<FAST::AttrSpec*>* pAttrSpecList;
};

//========================================================================================
// DeclarationTypeSpec
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_DeclarationTypeSpec(ATerm term, SgUntypedType** type)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_DeclarationTypeSpec: %s\n", ATwriteToString(term));
#endif
  
  if (traverse_IntrinsicTypeSpec(term, type)) {
     //MATCHED IntrinsicTypeSpec
  }
//TODO DerivedTypeSpec
  else return ATfalse;

  return ATtrue;
}

//========================================================================================
// IntrinsicTypeSpec
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_IntrinsicTypeSpec(ATerm term, SgUntypedType** type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_IntrinsicTypeSpec: %s\n", ATwriteToString(term));
#endif
  
   SgUntypedExpression* type_kind = NULL;
   bool has_kind = false;
   bool is_literal = false;
   bool is_class = false;
   bool is_intrinsic = true;
   bool is_constant = false;
   bool is_user_defined = false;
   SgUntypedExpression* char_length_expr = NULL;
   std::string char_length;
   bool char_length_is_string = false;
   SgUntypedType::type_enum type_enum = SgUntypedType::e_unknown;

  //TODO kind-type

  if (ATmatch(term, "IntrinsicType(INTEGER())")) {
     type_enum = SgUntypedType::e_int;
     *type = new SgUntypedType("integer",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,is_user_defined,
                                         char_length_expr,char_length,char_length_is_string,type_enum);
  }
  else if (ATmatch(term, "IntrinsicType(REAL())")) {
     type_enum = SgUntypedType::e_float;
     *type = new SgUntypedType("real",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,is_user_defined,
                                      char_length_expr,char_length,char_length_is_string,type_enum);
  }
  else if (ATmatch(term, "IntrinsicType(LOGICAL())")) {
     type_enum = SgUntypedType::e_bool;
     *type = new SgUntypedType("logical",type_kind,has_kind,is_literal,is_class,is_intrinsic,is_constant,is_user_defined,
                                         char_length_expr,char_length,char_length_is_string,type_enum);
  }
  else return ATfalse;

  return ATtrue;
}

//========================================================================================
// R305 literal-constant
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_LiteralConstant(ATerm term, SgUntypedExpression** var_expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_LiteralConstant: %s\n", ATwriteToString(term));
#endif

   ATerm term2;
   char* arg1;
   std::string value;
   SgToken::ROSE_Fortran_Keywords keyword = SgToken::FORTRAN_TYPE;
   SgUntypedType* type;

   *var_expr = NULL;
   if (ATmatch(term, "IntVal(<str>)", &arg1)) {
      // MATCHED IntVal
      value += arg1;
      type = new SgUntypedType("integer",NULL,false,true,false,true,false,false,NULL,"",false,SgUntypedType::e_int);
   }
   else if (ATmatch(term, "IntVal(<str>,<term>)", &arg1,&term2)) {
      SgUntypedExpression* kind;
      if (traverse_Expression(term2, &kind)) {
         // MATCHED KindParam
      }
      value += arg1;
      type = new SgUntypedType("integer",kind,true,true,false,true,false,false,NULL,"",false,SgUntypedType::e_int);
   }
   else return ATfalse;

   *var_expr = new SgUntypedValueExpression(keyword,value,type);

   return ATtrue;
}

//========================================================================================
// R309 intrinsic-operator
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_Operator(ATerm term, SgUntypedExpression** var_expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Operator: %s\n", ATwriteToString(term));
#endif

   ATerm term1, term2;

   SgToken::ROSE_Fortran_Keywords stmt_enum = SgToken::FORTRAN_UNKNOWN;
   SgToken::ROSE_Fortran_Operators  op_enum;
   std::string op_name;
   SgUntypedExpression* lhs;
   SgUntypedExpression* rhs;

   *var_expr = NULL;

// Binary operators
//
   if (ATmatch(term, "Power(<term>,<term>)", &term1,&term2)) {
      op_enum = SgToken::FORTRAN_INTRINSIC_POWER;
      op_name = "**";
   }
   else if (ATmatch(term, "Mult(<term>,<term>)", &term1,&term2)) {
      op_enum = SgToken::FORTRAN_INTRINSIC_TIMES;
      op_name = "*";
   }
   else if (ATmatch(term, "Div(<term>,<term>)", &term1,&term2)) {
      op_enum = SgToken::FORTRAN_INTRINSIC_DIVIDE;
      op_name = "/";
   }
   else if (ATmatch(term, "Plus(<term>,<term>)", &term1,&term2)) {
      op_enum = SgToken::FORTRAN_INTRINSIC_PLUS;
      op_name = "+";
   }
   else if (ATmatch(term, "Minus(<term>,<term>)", &term1,&term2)) {
      op_enum = SgToken::FORTRAN_INTRINSIC_MINUS;
      op_name = "-";
   }
   else if (ATmatch(term, "EQ(<term>,<term>)", &term1,&term2)) {
      op_enum = SgToken::FORTRAN_INTRINSIC_EQ;
      op_name = "==";
   }
   else if (ATmatch(term, "NE(<term>,<term>)", &term1,&term2)) {
      op_enum = SgToken::FORTRAN_INTRINSIC_NE;
      op_name = "/=";
   }
   else if (ATmatch(term, "LT(<term>,<term>)", &term1,&term2)) {
      op_enum = SgToken::FORTRAN_INTRINSIC_LT;
      op_name = "<";
   }
   else if (ATmatch(term, "GT(<term>,<term>)", &term1,&term2)) {
      op_enum = SgToken::FORTRAN_INTRINSIC_GT;
      op_name = ">";
   }
   else if (ATmatch(term, "GE(<term>,<term>)", &term1,&term2)) {
      op_enum = SgToken::FORTRAN_INTRINSIC_GE;
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

   *var_expr = new SgUntypedBinaryOperator(stmt_enum,op_enum,op_name,lhs,rhs);

  return ATtrue;
}

//========================================================================================
// R422 char-length (optional only for now)
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_OptCharLength(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptCharLength: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-char-length()")) {
   }
 //TODO - implement CharLength
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// R501 type-declaration-stmt
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_TypeDeclarationStmt(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_TypeDeclarationStmt: %s\n", ATwriteToString(term));
#endif
  
   ATerm term1, term2, term3, term4, term_eos;
   std::string label;
   std::string eos;
   SgUntypedType* declared_type;
//TODO_SgUntyped - need way to add attributes
   std::vector<FAST::AttrSpec*> attr_list;
   AttrSpecMatch match_attrs(&attr_list);

   SgUntypedVariableDeclaration* variable_decl = NULL;
   SgUntypedInitializedNameList* var_name_list = new SgUntypedInitializedNameList();

   if (ATmatch(term, "TypeDeclarationStmt(<term>,<term>,<term>,<term>,<term>)", &term1,&term2,&term3,&term4,&term_eos)) {
      if (traverse_OptLabel(term1, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_DeclarationTypeSpec(term2, &declared_type)) {
         // MATCHED DeclarationTypeSpec
      } else return ATfalse;
      if (traverse_OptCommaList(term3, match_attrs)) {
         // MATCHED AttrSpecList
      //TODO_SgUntyped - need way to add attributes
      } else return ATfalse;
      if (traverse_EntityDeclList(term4, declared_type, var_name_list)) {
         // MATCHED EntityDeclList
      } else return ATfalse;
      if (traverse_eos(term_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

// SgUntyped additions:
//   1. AttrSpecList, this can be a list of enums as the array specification can be placed in SgUntypedArrayType

   variable_decl = new SgUntypedVariableDeclaration(label, SgToken::FORTRAN_TYPE, declared_type, var_name_list);
   decl_list->get_decl_list().push_back(variable_decl);

//TODO - getLocationFromEOS(term,term_eos)

  return ATtrue;
}

//========================================================================================
// R503 entity-decl
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_EntityDecl(ATerm term, SgUntypedType* declared_type, SgUntypedInitializedNameList* name_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_EntityDecl: %s\n", ATwriteToString(term));
#endif

   ATerm term1, term2, term3, term4, eos_term;
   std::string name;

   SgUntypedType* initialized_type = NULL;
   SgUntypedInitializedName* initialized_name = NULL;
   SgUntypedExpression* char_length = NULL;
   SgUntypedExpression* initialization = NULL;

 //TODO - is this ok (can pointers to types be shared/copied)
   initialized_type = declared_type;

   if (ATmatch(term, "EntityDecl(<term>,<term>,<term>,<term>,<term>)",&term1,&term2,&term3,&term4,&eos_term)) {
      if (traverse_Name(term1, name)) {
         // MATCHED ObjectName                                                                                      
      } else return ATfalse;
      if (traverse_OptArraySpec(term2, declared_type, &initialized_type)) {
         // MATCHED ArraySpec
         // will have changed to an SgUntypedArrayType
         //TODO - implement in traverseal
      } else return ATfalse;
      if (traverse_OptCoarraySpec(term3, declared_type, &initialized_type)) {
         // MATCHED CoarraySpec
         // may have changed to an SgUntypedArrayType
      //TODO - implement in traverseal
      } else return ATfalse;
      if (traverse_OptCharLength(term4, &char_length)) {
         // MATCHED OptCharLength
         if (char_length != NULL) {
            initialized_type->set_char_length_expression(char_length);
         }
      } else return ATfalse;
      if (traverse_OptInitialization(eos_term, &initialization)) {
         // MATCHED ArraySpec
      //TODO_SgUntyped - SgUntypedInitializedName/SgUntypedType requires ability to initialize
#if 0
         if (initialization != NULL) {
            initialized_type->set_char_length_expression(char_length);
         }
#endif
      } else return ATfalse;
   } else return ATfalse;

   initialized_name = new SgUntypedInitializedName(initialized_type, name);
   name_list->get_name_list().push_back(initialized_name);

//TODO
//   setSourcePosition(initialized_name, ?);

   return ATtrue;
}

//========================================================================================
// entity-decl-list
//  - this is a bare list (without a cons name)
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_EntityDeclList(ATerm term, SgUntypedType* declared_type, SgUntypedInitializedNameList* name_list)
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
ATbool ATtoUntypedTraversal::traverse_OptInitialization(ATerm term, SgUntypedExpression** expr)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Initialization: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-init()")) {
   }
 //TODO - implement initialization
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// R509 coarray-spec (optional only for now)
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_OptCoarraySpec(ATerm term, SgUntypedType* declared_type, SgUntypedType** initialized_type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptCoarraySpec: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-list()")) {
   }
 //TODO - implement CoarraySpec  
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// R515 array-spec (optional only for now)
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_OptArraySpec(ATerm term, SgUntypedType* declared_type, SgUntypedType** initialized_type)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptArraySpec: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-list()")) {
   }
 //TODO - implement ArraySpec  
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// R560 implicit-stmt
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_ImplicitStmt(ATerm term, SgUntypedDeclarationStatementList* decl_list)
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

   SgUntypedImplicitDeclaration* implicit_decl = NULL;

   if (ATmatch(term, "ImplicitNoneStmt(<term>,<term>)", &term1,&term_eos)) {
      if (traverse_OptLabel(term1, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_eos(term_eos, eos)) {
         // MATCHED eos string
      } else return ATfalse;

      implicit_decl = new SgUntypedImplicitDeclaration(label, SgToken::FORTRAN_IMPLICIT_NONE);
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

    //TODO_SgUntyped
      return ATfalse;
   }
   else return ATfalse;

   decl_list->get_decl_list().push_back(implicit_decl);

   return ATtrue;
}

//========================================================================================
// R611 data-ref
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_DataRef(ATerm term, SgUntypedExpression** var_expr)
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
ATbool ATtoUntypedTraversal::traverse_PartRef(ATerm term, SgUntypedExpression** var_expr)
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
   }
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// R620 section-subscript
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_OptSectionSubscripts(ATerm term)
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
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// R624 image-selector
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_OptImageSelector(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptImageSelector: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-image-selector()")) {
      // MATCHED empty list
   }
   //TODO_SgUntyped - needs image-selector (ImageSelector - CosubscriptList)
   //TODO_SgUntyped - (substring-section-range)
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// R722 expr
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_Expression(ATerm term, SgUntypedExpression** var_expr)
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

ATbool ATtoUntypedTraversal::traverse_OptExpr( ATerm term, SgUntypedExpression** expr )
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_OptExpr: %s\n", ATwriteToString(term));
#endif

   return ATfalse;
}

//========================================================================================
// R732 assignment-statement
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_AssignmentStmt(ATerm term, SgUntypedStatementList* stmt_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_AssignmentStmt: %s\n", ATwriteToString(term));
#endif
  
   ATerm term1, term2, term3, term_eos;
   std::string label;
   std::string eos;
   SgUntypedExpression* variable;
   SgUntypedExpression* expr;

   SgToken::ROSE_Fortran_Keywords keyword = SgToken::FORTRAN_ASSIGN;

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

   SgUntypedAssignmentStatement* assign_stmt = new SgUntypedAssignmentStatement(label,keyword,variable,expr);
   stmt_list->get_stmt_list().push_back(assign_stmt);

   return ATtrue;
}

//========================================================================================
// R854 continue-stmt
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_ContinueStmt(ATerm term, SgUntypedStatementList* stmt_list)
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
   stmt_list->get_stmt_list().push_back(continue_stmt);

   return ATtrue;
}

//========================================================================================
// MainProgram (R1101)
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_MainProgram(ATerm term, SgUntypedScope* scope)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_MainProgram: %s\n", ATwriteToString(term));
#endif

   ATerm term1, term2, term3, term4, term5;
   std::string label, name;

   SgUntypedProgramHeaderDeclaration* main_program;
   SgUntypedNamedStatement* program_stmt;
   SgUntypedOtherStatement* contains_stmt;
   SgUntypedNamedStatement* end_program_stmt;
   SgUntypedFunctionScope * function_scope;

   SgToken::ROSE_Fortran_Keywords keyword = SgToken::FORTRAN_PROGRAM;

// scope and parameter lists
   SgUntypedDeclarationStatementList* decl_list  = new SgUntypedDeclarationStatementList();
   SgUntypedStatementList*            stmt_list  = new SgUntypedStatementList();
   SgUntypedFunctionDeclarationList*  func_list  = new SgUntypedFunctionDeclarationList();
   SgUntypedInitializedNameList*     param_list  = new SgUntypedInitializedNameList();

   if (ATmatch(term, "MainProgram(<term>,<term>,<term>,<term>,<term>)", &term1,&term2,&term3,&term4,&term5)) {
      if (traverse_OptProgramStmt(term1, &program_stmt)) {
         // ProgramStmt
      } else return ATfalse;
      if (traverse_InitialSpecPart(term2, decl_list)) {
         // InitialSpecPart
      } else return ATfalse;
      if (traverse_SpecAndExecPart(term3, decl_list, stmt_list)) {
         // SpecAndExecPart
      } else return ATfalse;

      label = program_stmt->get_label_string();
      name  = program_stmt->get_statement_name();
      function_scope = new SgUntypedFunctionScope(label,keyword,decl_list,stmt_list,func_list);

      if (traverse_OptInternalSubprogramPart(term4, &contains_stmt, function_scope)) {
         // OptInternalSubprogramPart
      } else return ATfalse;
      if (traverse_EndProgramStmt(term5, &end_program_stmt)) {
         // EndProgramStmt
      } else return ATfalse;
   } else return ATfalse;

// organize parameters to construct the program
//
   SgUntypedExpression* null_expr = new SgUntypedExpression(SgToken::FORTRAN_NULL);
   SgUntypedType* type = new SgUntypedType("void",null_expr/*type_kind*/,false,false,false,true/*is_intrinsic*/,false,false,
                                                  null_expr/*char_length*/,"",false/*char_length_is_string*/,SgUntypedType::e_void);

// create the program
   main_program   = new SgUntypedProgramHeaderDeclaration(label,keyword,name,param_list,type,function_scope,end_program_stmt);

// set source positions
#if SET_SOURCE_POSITION_UNKNOWN
   ATtoUntypedTraversal::setSourcePositionUnknown(main_program);
   ATtoUntypedTraversal::setSourcePositionUnknown( end_program_stmt);
#else
   setSourcePosition(main_program, programStmt->getPosInfo());
   setSourcePosition(end_program_stmt, endProgramStmt->getPosInfo());
#endif

// add program to the global scope
   scope->get_function_list()->get_func_list().push_back(main_program);

// replaced by the program
   delete program_stmt;

   return ATtrue;
}

//========================================================================================
// R1102 program-stmt
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_OptProgramStmt(ATerm term, SgUntypedNamedStatement** program_stmt)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_OptProgramStmt: %s\n", ATwriteToString(term));
#endif
  
  ATerm term1, term2, term3;
  std::string label;
  std::string name;
  std::string eos;

  SgToken::ROSE_Fortran_Keywords keyword = SgToken::FORTRAN_PROGRAM;

  *program_stmt = NULL; 
  if (ATmatch(term, "ProgramStmt(<term>,<term>,<term>)", &term1,&term2,&term3)) {
    if (traverse_OptLabel(term1, label)) {
      // MATCHED OptLabel
    } else return ATfalse;
    if (traverse_Name(term2, name)) {
      // MATCHED ProgramName
    } else return ATfalse;
    if (traverse_eos(term3, eos)) {
      // MATCHED eos string
    } else return ATfalse;
  }
  else if (ATmatch(term, "no-program-stmt()")) {
   // There is no program-stmt so mark the name as special so that ROSE
   // knows that the statement is implied/compiler generated.
      name += "rose_fortran_main";
  } else return ATfalse;

  *program_stmt = new SgUntypedNamedStatement(label,keyword,name);

  return ATtrue;
}

//========================================================================================
// EndProgramStmt (R1103)
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_EndProgramStmt(ATerm term, SgUntypedNamedStatement** end_program_stmt)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_EndProgramStmt: %s\n", ATwriteToString(term));
#endif

  ATerm term1, term2, term3;
  std::string label;
  std::string name;
  std::string eos;

  SgToken::ROSE_Fortran_Keywords keyword = SgToken::FORTRAN_END_PROGRAM;
  FAST::PosInfo pinfo;

  *end_program_stmt = NULL;
  if (ATmatch(term, "EndProgramStmt(<term>,<term>,<term>)", &term1,&term2,&term3)) {
    if (traverse_OptLabel(term1, label)) {
      // MATCHED OptLabel
    } else return ATfalse;
    if (traverse_OptName(term2, name)) {
      // MATCHED ProgramName string
    } else return ATfalse;
    if (traverse_eos(term3, eos)) {
      // MATCHED eos string
    } else return ATfalse;
  } else return ATfalse;

  *end_program_stmt = new SgUntypedNamedStatement(label,keyword,name);

  return ATtrue;
}

//========================================================================================
// Module (R1103)
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_Module(ATerm term, SgUntypedScope* scope)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Module: %s\n", ATwriteToString(term));
#endif

// TODO - implementation
   return ATfalse;
}

//========================================================================================
// Submodule (R1116)
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_Submodule(ATerm term, SgUntypedScope* scope)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_Submodule: %s\n", ATwriteToString(term));
#endif

// TODO - implementation
   return ATfalse;
}

//========================================================================================
// BlockData (R1120)
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_BlockData(ATerm term, SgUntypedScope* scope)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_BlockData: %s\n", ATwriteToString(term));
#endif

// TODO - implementation
   return ATfalse;
}

//========================================================================================
// ImportStmt (R1209)
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_ImportStmt(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_ImportStmt: %s\n", ATwriteToString(term));
#endif

  ATerm term1, term2, eos_term;
  std::string label;
  std::vector<std::string> name_list;
  std::string eos;
  
  if (ATmatch(term, "ImportStmt(<term>,<term>,<term>", &term1,&term2,&eos_term)) {
    if (traverse_OptLabel(term1, label)) {
       //matched OptLabel
    } else return ATfalse;
    if (traverse_NameList(term2, name_list)) {
       //matched NameList
    } else return ATfalse;
    if (traverse_eos(eos_term, eos)) {
       //matched EOS
    } else return ATfalse;
  }
  else return ATfalse;

  return ATtrue;
}

//========================================================================================
// ImportStmtList
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_ImportStmtList(ATerm term, SgUntypedDeclarationStatementList* decl_list)
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
ATbool ATtoUntypedTraversal::traverse_ExternalStmt(ATerm term, SgUntypedDeclarationStatementList* decl_list)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ExternalStmt: %s\n", ATwriteToString(term));
#endif

   ATerm term1, term2, eos_term;
   std::string label;
   std::vector<std::string> name_list;
   std::string eos;
  
  if (ATmatch(term, "ExternalStmt(<term>,<term>,<term>)", &term1,&term2,&eos_term)) {
     if (traverse_OptLabel(term1, label)) {
        // MATCHED OptLabel
     } else return ATfalse;
     if (traverse_NameList(term2, name_list)) {
        // MATCHED NameList
     } else return ATfalse;
     if (traverse_eos(eos_term, eos)) {
        // MATCHED EOS
     } else return ATfalse;
  }
  else return ATfalse;

  return ATtrue;
}

//========================================================================================
// Prefix (R1225)
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_OptPrefix(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptPrefix: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-prefix()")) {
      // there is no prefix
   }

// TODO - implementation
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// FunctionSubprogram (R1227)
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_FunctionSubprogram(ATerm term, SgUntypedScope* scope)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_FunctionSubprogram: %s\n", ATwriteToString(term));
#endif

// TODO - implementation
   return ATfalse;
}

//========================================================================================
// ProcLanguageBindingSpec (R1229)
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_OptProcLanguageBindingSpec(ATerm term)
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
// SubroutineSubprogram (R1233)
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_SubroutineSubprogram(ATerm term, SgUntypedScope* scope)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_SubroutineSubprogram: %s\n", ATwriteToString(term));
#endif

   ATerm term1, term2, term3, term4, term5, term6;
   std::string label, name;

   SgUntypedSubroutineDeclaration* subroutine;
   SgUntypedOtherStatement* contains_stmt;
   SgUntypedNamedStatement* end_subroutine_stmt;
   SgUntypedFunctionScope * function_scope;

// scope and parameter lists
   SgUntypedDeclarationStatementList* decl_list  = new SgUntypedDeclarationStatementList();
   SgUntypedStatementList*            stmt_list  = new SgUntypedStatementList();
   SgUntypedFunctionDeclarationList*  func_list  = new SgUntypedFunctionDeclarationList();
   SgUntypedInitializedNameList*     param_list  = new SgUntypedInitializedNameList();

   SgToken::ROSE_Fortran_Keywords keyword = SgToken::FORTRAN_SUBROUTINE;

   if (ATmatch(term, "SubroutineSubprogram(<term>,<term>,<term>,<term>,<term>)", &term1,&term2,&term3,&term4,&term5,&term6)) {
      ATerm label_term, prefix_term, name_term, arglist_term, binding_term, eos_term;
      std::string eos;

   // retrieve the subroutine-stmt
   //
      if (ATmatch(term1, "SubroutineStmt(<term>,<term>,<term>,<term>,<term>,<term>)"
                       , &label_term, &prefix_term, &name_term, &arglist_term, &binding_term, &eos_term)) {
         if (traverse_OptLabel(label_term, label)) {
            // MATCHED OptLabel
         } else return ATfalse;
         if (traverse_OptPrefix(prefix_term)) {
            // MATCHED OptPrefix
         } else return ATfalse;
         if (traverse_Name(name_term, name)) {
            // MATCHED ProgramName string
         } else return ATfalse;
         if (traverse_OptDummyArgList(arglist_term)) {
            // MATCHED OptDummyArgList
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
      } else return ATfalse;

      function_scope = new SgUntypedFunctionScope(label,keyword,decl_list,stmt_list,func_list);

      if (traverse_OptInternalSubprogramPart(term4, &contains_stmt, function_scope)) {
         // OptInternalSubprogramPart
      } else return ATfalse;
      if (traverse_EndSubroutineStmt(term5, &end_subroutine_stmt)) {
         // EndSubroutineStmt
      } else return ATfalse;
   } else return ATfalse;

// organize parameters to construct the subroutine
//
   SgUntypedExpression* null_expr = new SgUntypedExpression(SgToken::FORTRAN_NULL);

   SgUntypedType* type = new SgUntypedType("void",null_expr/*type_kind*/,false,false,false,true/*is_intrinsic*/,false,false,
                                                  null_expr/*char_length*/,"",false/*char_length_is_string*/,SgUntypedType::e_void);

// create the subroutine
   subroutine = new SgUntypedSubroutineDeclaration(label,keyword,name,param_list,type,function_scope,end_subroutine_stmt);

// set source positions
#if SET_SOURCE_POSITION_UNKNOWN
   ATtoUntypedTraversal::setSourcePositionUnknown(subroutine);
   ATtoUntypedTraversal::setSourcePositionUnknown(end_subroutine_stmt);
#else
   setSourcePosition(subroutine, subroutineStmt->getPosInfo());
   setSourcePosition(end_subroutine_stmt, endSubroutineStmt->getPosInfo());
#endif

// add the subroutine to the outer scope
   scope->get_function_list()->get_func_list().push_back(subroutine);

   return ATtrue;
}

//========================================================================================
// DummyArgList (R1235)
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_OptDummyArgList(ATerm term)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptDummyArgList: %s\n", ATwriteToString(term));
#endif

   if (ATmatch(term, "no-list()")) {
      // there are no dummy arguments
   }

// TODO - implementation
   else return ATfalse;

   return ATtrue;
}

//========================================================================================
// EndSubroutineStmt (R1236)
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_EndSubroutineStmt(ATerm term, SgUntypedNamedStatement** end_subroutine_stmt)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_EndSubroutineStmt: %s\n", ATwriteToString(term));
#endif

   ATerm term1, term2, term3;
   std::string label, name, eos;

   SgToken::ROSE_Fortran_Keywords keyword = SgToken::FORTRAN_END_SUBROUTINE;
   FAST::PosInfo pinfo;

   *end_subroutine_stmt = NULL;
   if (ATmatch(term, "EndSubroutineStmt(<term>,<term>,<term>)", &term1,&term2,&term3)) {
      if (traverse_OptLabel(term1, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_OptName(term2, name)) {
         // MATCHED SubroutineName string
      } else return ATfalse;
      if (traverse_eos(term3, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

   *end_subroutine_stmt = new SgUntypedNamedStatement(label,keyword,name);

   return ATtrue;
}

//========================================================================================
// R1238 mp-subprogram-stmt
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_MpSubprogramStmt(ATerm term, SgUntypedNamedStatement** mp_subprogram_stmt)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_OptMpSubprogramStmt: %s\n", ATwriteToString(term));
#endif
  
   ATerm term1, term2, term3;
   std::string label;
   std::string name;
   std::string eos;

   SgToken::ROSE_Fortran_Keywords keyword = SgToken::FORTRAN_UNKNOWN; // NEW_TOKEN::FORTRAN_MP_SUBPROGRAM

   *mp_subprogram_stmt = NULL; 
   if (ATmatch(term, "MpSubprogramStmt(<term>,<term>,<term>)", &term1,&term2,&term3)) {
      if (traverse_OptLabel(term1, label)) {
         // MATCHED OptLabel
      } else return ATfalse;
      if (traverse_Name(term2, name)) {
         // MATCHED ProcedureName string
      } else return ATfalse;
      if (traverse_eos(term3, eos)) {
         // MATCHED eos string
      } else return ATfalse;
   }
   else return ATfalse;

  *mp_subprogram_stmt = new SgUntypedNamedStatement(label,keyword,name);

  return ATtrue;
}

//========================================================================================
// R1239 end-mp-subprogram-stmt
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_EndMpSubprogramStmt(ATerm term, SgUntypedNamedStatement** end_mp_subprogram_stmt)
{
#if PRINT_ATERM_TRAVERSAL
  printf("... traverse_EndMpSubprogramStmt: %s\n", ATwriteToString(term));
#endif

  ATerm term1, term2, term3;
  std::string label;
  std::string name;
  std::string eos;

  SgToken::ROSE_Fortran_Keywords keyword = SgToken::FORTRAN_UNKNOWN; // NEW_TOKEN::FORTRAN_END_MP_PROGRAM
  FAST::PosInfo pinfo;

  *end_mp_subprogram_stmt = NULL;
  if (ATmatch(term, "EndMpSubprogramStmt(<term>,<term>,<term>)", &term1,&term2,&term3)) {
    if (traverse_OptLabel(term1, label)) {
      // MATCHED OptLabel
    } else return ATfalse;
    if (traverse_OptName(term2, name)) {
      // MATCHED ProcedureName string
    } else return ATfalse;
    if (traverse_eos(term3, eos)) {
      // MATCHED eos string
    } else return ATfalse;
  } else return ATfalse;

  *end_mp_subprogram_stmt = new SgUntypedNamedStatement(label,keyword,name);

  return ATtrue;
}

//========================================================================================
// R1240 entry-stmt
//----------------------------------------------------------------------------------------

//========================================================================================
// R1241 return-stmt
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_ReturnStmt(ATerm term, SgUntypedExpressionStatement** return_stmt)
{
#if PRINT_ATERM_TRAVERSAL
   printf("... traverse_ReturnStmt: %s\n", ATwriteToString(term));
#endif
  
   ATerm term1, term2, term_eos;
   std::string label;
   std::string eos;
   SgUntypedExpression* expr;

   *return_stmt = NULL;

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

#ifdef NEW_UNTYPED_STATEMENT
   SgToken::ROSE_Fortran_Keywords keyword = SgToken::FORTRAN_RETURN;
   *return_stmt = new SgUntypedExpressionStatement(label, keyword, expr);
#endif

   return ATtrue;
}

//========================================================================================
// R1242 contains-stmt
//----------------------------------------------------------------------------------------
ATbool ATtoUntypedTraversal::traverse_ContainsStmt(ATerm term, SgUntypedOtherStatement** contains_stmt)
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

   return ATtrue;
}
