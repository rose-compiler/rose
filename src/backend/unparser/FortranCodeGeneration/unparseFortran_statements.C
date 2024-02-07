/* unparseFortran_statements.C
 *
 * Code to unparse Sage/Fortran statement nodes.
 *
 */

#include "sage3basic.h"
#include "unparser.h"
#include <limits>

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

#ifdef _MSC_VER
#define strncasecmp _strnicmp
#endif

using namespace std;
using namespace Rose;

#ifdef ROSE_EXPERIMENTAL_FLANG_ROSE_CONNECTION
  constexpr bool flangParser{true};
  constexpr bool keywordsAreUpperCase{false}; // should be a command-line option
#else
  constexpr bool flangParser{false};
  constexpr bool keywordsAreUpperCase{true};
#endif

// Unparse language keywords
void FortranCodeGeneration_locatedNode::
curprint_keyword(const std::string &keyword, SgUnparse_Info& info)
{
  if (keywordsAreUpperCase) {
    // The default construction used below
    curprint(keyword);
  }
  else {
    std::string lowered{keyword};
    transform(lowered.begin(), lowered.end(), lowered.begin(), ::tolower);
    curprint(lowered);
  }
}

inline bool
namesMatch ( const string &x, const string &y )
   {
  // This function checks a case insensitive match of x against y.
  // This is required because Fortran is case insensitive.

     size_t x_length = x.length();
     size_t y_length = y.length();
     ROSE_ASSERT(x_length > 0 && y_length > 0);
     return (x_length == y_length) ? strncasecmp(x.c_str(),y.c_str(),x_length) == 0 : false;
   }


FortranCodeGeneration_locatedNode::FortranCodeGeneration_locatedNode(Unparser* unp, std::string fname)
   : UnparseLanguageIndependentConstructs(unp,fname)
   {
  // Nothing to do here!
   }

FortranCodeGeneration_locatedNode::~FortranCodeGeneration_locatedNode()
   {
  // Nothing to do here!
   }

void
FortranCodeGeneration_locatedNode::unparseStatementNumbersSupport ( SgLabelRefExp* numeric_label_exp, SgUnparse_Info& info )
   {
  // This is a supporting function for the unparseStatementNumbers, but can be called directly for statments
  // in the IR that can have botha starting yntax and an ending syntax, both of which can be labeled.  
  // See test2007_01.f90 for an example of the SgProgramHeaderStatement used this way.

  // In fixed format all labels must appear within columns 1-5 (where column 1 is the first column)
  // and the 6th column is for the line continuation character (any character, I think).
     const int NumericLabelIndentation = 6;

     if (info.SkipFormatting())
        {
          return;
        }

  // Let the default be fixed format for now (just for fun)
     bool fixedFormat = (unp->currentFile == nullptr) ||
                        (unp->currentFile->get_outputFormat() == SgFile::e_unknown_output_format) ||
                        (unp->currentFile->get_outputFormat() == SgFile::e_fixed_form_output_format);

     if (numeric_label_exp != nullptr)
        {
          SgLabelSymbol* numeric_label_symbol = numeric_label_exp->get_symbol();
          int numeric_label = numeric_label_symbol->get_numeric_label_value();

          ASSERT_require(numeric_label >= -1);

       // DQ (12/24/2007): I think that this value is an error in all versions of Fortran
          ROSE_ASSERT(numeric_label != 0);

       // If it is greater than zero then output the value converted to a string.
          if (numeric_label >= 0)
             {
            // A label exists in the source code
               string numeric_label_string = StringUtility::numberToString(numeric_label);

            // append an extra blank to seperate the lable from other code (if fixedFormat == true 
            // then this puts a blank into column 6 as required for this to be a code statement).
               numeric_label_string += " ";

               if (fixedFormat)
                  {
                 // Now indent the statement so that it will appear uniform (just for fun!)
                    int spacing = numeric_label_string.size();
                    while (spacing < NumericLabelIndentation)
                       {
                      // prepend the extra blanks to right justify the numeric labels
                      // (we have to fill the space anyway and this makes them look nice).
                         numeric_label_string = " " + numeric_label_string;
                         spacing++;
                       }
                  }

               curprint( numeric_label_string );
             }
            else
             {
               if (fixedFormat)
                  {
                 // if fixed format then output 6 blanks
                    curprint("      ");
                  }
             }
        }
       else
        {
          if (fixedFormat)
             {
            // if fixed format then output 6 blanks
               curprint("      ");
             }
        }
   }

void
FortranCodeGeneration_locatedNode::unparseStatementNumbers ( SgStatement* stmt, SgUnparse_Info& info )
   {
  // This is a virtual function (called by the UnparseLanguageIndependentConstructs::unparseStatement() member function).

  // DQ (11/29/2008): If this is a CPP directive then don't output statement 
  // number or the white space for then in fixed format mode.
     if (isSgC_PreprocessorDirectiveStatement(stmt) != nullptr)
        {
          printf ("This is a CPP directive, skip leading white space in unparsing. \n");
          return;
        }

  // This fixes a formatting problem, an aspect fo which was reported by Liao 12/28/2007).
     if ( isSgGlobal(stmt) != nullptr || isSgBasicBlock(stmt) != nullptr )
        {
       // Skip any formatting since these don't result in statements that are output!
        }
       else
        {
          SgProgramHeaderStatement* program_header = isSgProgramHeaderStatement(stmt);
          if (program_header != nullptr)
             {
               if (program_header->get_name() != ROSE_IMPLICIT_FORTRAN_PROGRAM_NAME)
                  {
                 // If this is a program name that will be output then format the start 
                 // of the output (in case there is a label or this is fixed format).
                    unparseStatementNumbersSupport(stmt->get_numeric_label(),info);
                  }
             }
            else
             {
            // This is a Fortran specific case (different from use of SgLabelStatement in C/C++).
               unparseStatementNumbersSupport(stmt->get_numeric_label(),info);
             }
        }
   }

void
FortranCodeGeneration_locatedNode::unparseLanguageSpecificStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
  // This function unparses the language specific parse not handled by the base class unparseStatement() member function
     ASSERT_not_null(stmt);

     switch (stmt->variantT())
        {
       // program units
       // case V_SgModuleStatement:            unparseModuleStmt(stmt, info);            break;
          case V_SgProgramHeaderStatement:     unparseProgHdrStmt(stmt, info);           break;
          case V_SgProcedureHeaderStatement:   unparseProcHdrStmt(stmt, info);           break;

       // declarations
          case V_SgInterfaceStatement:         unparseInterfaceStmt(stmt, info);         break;
          case V_SgCommonBlock:                unparseCommonBlock(stmt, info);           break;
          case V_SgVariableDeclaration:        unparseVarDeclStmt(stmt, info);           break;
          case V_SgVariableDefinition:         unparseVarDefnStmt(stmt, info);           break;
          case V_SgParameterStatement:         unparseParamDeclStmt(stmt, info);         break;
          case V_SgUseStatement:               unparseUseStmt(stmt, info);               break;

          case V_SgDerivedTypeStatement:       unparseClassDeclStmt_derivedType(stmt, info); break;
          case V_SgModuleStatement:            unparseClassDeclStmt_module(stmt, info);      break;

          case V_SgClassDefinition:            unparseClassDefnStmt(stmt, info);         break;

       // executable statements, control flow
          case V_SgBasicBlock:                 unparseBasicBlockStmt(stmt, info);        break;
          case V_SgIfStmt:                     unparseIfStmt(stmt, info);                break;
          case V_SgFortranDo:                  unparseDoStmt(stmt, info);                break;
          case V_SgSwitchStatement:            unparseSwitchStmt(stmt, info);            break;
          case V_SgCaseOptionStmt:             unparseCaseStmt(stmt, info);              break;
          case V_SgDefaultOptionStmt:          unparseDefaultStmt(stmt, info);           break;
          case V_SgProcessControlStatement:    unparseProcessControlStmt(stmt, info);    break;


       // These are derived from SgIOStatement
          case V_SgPrintStatement:             unparsePrintStatement(stmt, info);        break;
          case V_SgReadStatement:              unparseReadStatement(stmt, info);         break;
          case V_SgWriteStatement:             unparseWriteStatement(stmt, info);        break;
          case V_SgOpenStatement:              unparseOpenStatement(stmt, info);         break;
          case V_SgCloseStatement:             unparseCloseStatement(stmt, info);        break;
          case V_SgInquireStatement:           unparseInquireStatement(stmt, info);      break;
          case V_SgFlushStatement:             unparseFlushStatement(stmt, info);        break;
          case V_SgRewindStatement:            unparseRewindStatement(stmt, info);       break;
          case V_SgBackspaceStatement:         unparseBackspaceStatement(stmt, info);    break;
          case V_SgEndfileStatement:           unparseEndfileStatement(stmt, info);      break;
          case V_SgWaitStatement:              unparseWaitStatement(stmt, info);         break;

       // These are derived from SgImageControlStatement
          case V_SgSyncAllStatement:           unparseSyncAllStatement(stmt, info);      break;
          case V_SgSyncImagesStatement:        unparseSyncImagesStatement(stmt, info);   break;
          case V_SgSyncMemoryStatement:        unparseSyncMemoryStatement(stmt, info);   break;
          case V_SgSyncTeamStatement:          unparseSyncTeamStatement(stmt, info);     break;
          case V_SgLockStatement:              unparseLockStatement(stmt, info);         break;
          case V_SgUnlockStatement:            unparseUnlockStatement(stmt, info);       break;

          case V_SgAssociateStatement:         unparseAssociateStatement(stmt, info);    break;

          case V_SgFunctionDefinition:         unparseFuncDefnStmt(stmt, info);          break;
          case V_SgExprStatement:              unparseExprStmt(stmt, info);              break;

          case V_SgImplicitStatement:          unparseImplicitStmt(stmt, info);          break;
          case V_SgBlockDataStatement:         unparseBlockDataStmt(stmt, info);         break;
          case V_SgStatementFunctionStatement: unparseStatementFunctionStmt(stmt, info); break;
          case V_SgWhereStatement:             unparseWhereStmt(stmt, info);             break;
          case V_SgElseWhereStatement:         unparseElseWhereStmt(stmt, info);         break;
          case V_SgNullifyStatement:           unparseNullifyStmt(stmt, info);           break;
          case V_SgEquivalenceStatement:       unparseEquivalenceStmt(stmt, info);       break;
          case V_SgArithmeticIfStatement:      unparseArithmeticIfStmt(stmt, info);      break;
          case V_SgAssignStatement:            unparseAssignStmt(stmt, info);            break;
          case V_SgComputedGotoStatement:      unparseComputedGotoStmt(stmt, info);      break;
          case V_SgAssignedGotoStatement:      unparseAssignedGotoStmt(stmt, info);      break;

       // DQ (11/16/2007): This is unparsed as a CONTINUE statement
          case V_SgLabelStatement:             unparseLabelStmt(isSgLabelStatement(stmt), info); break;

       // DQ (11/16/2007): This is a "DO WHILE" statement
          case V_SgWhileStmt:                  unparseWhileStmt(stmt, info); break;

       // DQ (11/17/2007): This is unparsed as a Fortran EXIT statement
          case V_SgBreakStmt:                  unparseBreakStmt(stmt, info); break;

       // This is unparsed as a Fortran CYCLE statement
          case V_SgContinueStmt:        unparseContinueStmt(isSgContinueStmt(stmt), info); break;
          case V_SgFortranContinueStmt: unparseFortranContinueStmt(isSgFortranContinueStmt(stmt), info); break;

          case V_SgAttributeSpecificationStatement: unparseAttributeSpecificationStatement(stmt, info); break;
          case V_SgNamelistStatement:          unparseNamelistStatement(stmt, info); break;
          case V_SgReturnStmt:                 unparseReturnStmt(stmt, info); break;
          case V_SgImportStatement:            unparseImportStatement(stmt, info); break;
          case V_SgFormatStatement:            unparseFormatStatement(stmt, info);      break;
          case V_SgGotoStatement:              unparseGotoStmt(isSgGotoStatement(stmt), info); break;

       // Rasmussen (10/02/2018): This is temporary fix (actual ForAllStatements aren't created)
          case V_SgForAllStatement:            unparseForAllStatement(stmt, info); break;

          case V_SgContainsStatement:          unparseContainsStatement(stmt, info); break;
          case V_SgEntryStatement:             unparseEntryStatement(stmt, info); break;
          case V_SgFortranIncludeLine:         unparseFortranIncludeLine(stmt, info); break;
          case V_SgAllocateStatement:          unparseAllocateStatement(stmt, info); break;
          case V_SgDeallocateStatement:        unparseDeallocateStatement(stmt, info); break;

          case V_SgCAFWithTeamStatement:       unparseWithTeamStatement(stmt, info); break;

       // Language independent code generation (placed in base class)
       // scope
       // case V_SgGlobal:                     unparseGlobalStmt(stmt, info); break;
       // case V_SgScopeStatement:             unparseScopeStmt(stmt, info); break;
       // case V_SgWhileStmt:                  unparseWhileStmt(stmt, info); break;
       // executable statements, other
       // case V_SgExprStatement:              unparseExprStmt(stmt, info); break;
       //  Liao 10/18/2010, I turn on the pragma unparsing here to help debugging OpenMP programs
       //  , where OpenMP directive comments are used to generate C/C++-like pragmas internally.
       //  Those pragmas later are used to reuse large portion of OpenMP AST construction of C/C++
       // pragmas
        case V_SgPragmaDeclaration:          unparsePragmaDeclStmt(stmt, info); break;
        // Liao 10/21/2010, Fortran-only OpenMP handling
        case V_SgOmpDoStatement:             unparseOmpDoStatement(stmt, info); break;
          default:
             {
               printf("FortranCodeGeneration_locatedNode::unparseLanguageSpecificStatement: Error: No unparse function for %s (variant: %d)\n",stmt->sage_class_name(), stmt->variantT());
               ROSE_ABORT();
             }
        }
   }



void
FortranCodeGeneration_locatedNode::unparseFortranIncludeLine (SgStatement* stmt, SgUnparse_Info& info)
   {
  // This is support for the language specific include mechanism.
     if (info.outputFortranModFile())  // rmod file expands the include file but does not contain the include statement
         return;
     SgFortranIncludeLine* includeLine = isSgFortranIncludeLine(stmt);

     curprint("include ");

  // DQ (10/3/2008): Added special case code generation to support an inconsistant 
  // behavior between gfortran 4.2 and previous versions in the Fortran include mechanism.
     string includeFileName = includeLine->get_filename();

#if USE_GFORTRAN_IN_ROSE

     bool usingGfortran = false;
     #ifdef USE_CMAKE
       #ifdef CMAKE_COMPILER_IS_GNUG77
         usingGfortran = true;
       #endif
     #else
       string fortranCompilerName = BACKEND_FORTRAN_COMPILER_NAME_WITH_PATH;
       usingGfortran = (fortranCompilerName == "gfortran");
     #endif

     if (usingGfortran)
        {
       // DQ (3/17/2017): Fixed this to support GNU 5.1.
          if ( (BACKEND_FORTRAN_COMPILER_MAJOR_VERSION_NUMBER == 3) || 
               ( (BACKEND_FORTRAN_COMPILER_MAJOR_VERSION_NUMBER == 4) && (BACKEND_FORTRAN_COMPILER_MINOR_VERSION_NUMBER <= 1) ) )
             {
            // gfortran versions before 4.2 can not handle absolute path names in the Fortran specific include mechanism.

            // Note that this fix would mistakenly strip all specified include files to their basename, even include files 
            // specified as "../sys/math.h" would become "math.h" and this could cause an error.
               printf ("Warning: gfortran versions before 4.2 can not handle absolute path names in the Fortran specific include mechanism (using basename)... \n");

               includeFileName = StringUtility::stripPathFromFileName(includeLine->get_filename());
             }
        }
       else
        {
       // What is this compiler
          printf ("Default compiler behavior ... in code generation (Fortran include uses absolute paths) \n");
        }
#endif

     curprint("\"");
     curprint(includeFileName);
     curprint("\"");

     unp->cur.insert_newline(1);
   }

void
FortranCodeGeneration_locatedNode::unparseEntryStatement   (SgStatement* stmt, SgUnparse_Info& info)
   {
  // This is much like a function declaration inside of an existing function

     SgEntryStatement* entryStatement = isSgEntryStatement(stmt);

     curprint("entry ");
     curprint(entryStatement->get_name());

     curprint("(");
     unparseFunctionArgs(entryStatement,info);     
     curprint(")");

  // Unparse the result(<name>) suffix if present
     if (entryStatement->get_result_name() != nullptr)
        {
          curprint(" result(");
          curprint(entryStatement->get_result_name()->get_name());
          curprint(")");
        }

     unp->cur.insert_newline(1);
   }

void
FortranCodeGeneration_locatedNode::unparseContainsStatement (SgStatement*, SgUnparse_Info&)
   {
     curprint("CONTAINS");
     unp->cur.insert_newline(1);
   }

void
FortranCodeGeneration_locatedNode::unparseNamelistStatement (SgStatement* stmt, SgUnparse_Info&)
   {
     SgNamelistStatement* namelistStatement = isSgNamelistStatement(stmt);

     curprint("namelist ");

     SgNameGroupPtrList & groupList = namelistStatement->get_group_list();
     SgNameGroupPtrList::iterator i = groupList.begin();
     while (i != groupList.end())
        {
          SgNameGroup* nameGroup = *i;
          curprint ("/" + nameGroup->get_group_name() + "/ ");
          SgStringList & nameList = nameGroup->get_name_list();
          SgStringList::iterator j = nameList.begin();
          while (j != nameList.end())
             {
               curprint (*j);
               j++;
               if (j != nameList.end())
                  {
                    curprint (",");
                  }
             }

          i++;

       // Put a little space before the next group name (it there are multiple groups specified)
          if (i != groupList.end())
             {
               curprint(" ");
             }
        }

     unp->cur.insert_newline(1);
   }

void
FortranCodeGeneration_locatedNode::unparseFormatItemList (SgFormatItemList* formatItemList, SgUnparse_Info& info)
   {
     SgFormatItemPtrList & formatList = formatItemList->get_format_item_list();
     SgFormatItemPtrList::iterator i = formatList.begin();
     while (i != formatList.end())
        {
          bool skip_comma = false;

          SgFormatItem* formatItem = *i;

       // The default value is "-1" so zero should be an invalid value
          int repeat_specification = formatItem->get_repeat_specification();
          ROSE_ASSERT(repeat_specification != 0);

       // Valid values are > 0
          if (repeat_specification > 0)
             {
               string stringValue = StringUtility::numberToString(repeat_specification);
               curprint(stringValue);
               curprint(" ");
             }

          if (formatItem->get_data() != nullptr)
             {
               SgStringVal* stringValue = isSgStringVal(formatItem->get_data());
               ASSERT_not_null(stringValue);

            // The string is stored without quotes, and we put them back on as required in code generation
               string str;
               if (stringValue->get_usesSingleQuotes())
                  {
                    str = string("\'") + stringValue->get_value() + string("\'");
                  }
                 else
                  {
                    if (stringValue->get_usesDoubleQuotes())
                       {
                         str = string("\"") + stringValue->get_value() + string("\"");
                       }
                      else
                       {
                      // Normally if usesSingleQuotes == false we use double quotes, but that would be
                      // a mistake since this is not a string literal used in the format statement.
                      // At some point we want to classify this, since it is a specific kind of edit 
                      // descriptor (see R1005, R1011, R1013, R1015, R1016, R1017, R1018).

                         str = stringValue->get_value();
                       }
                  }
               curprint(str);

             }
            else
             {
               if (formatItem->get_format_item_list() != nullptr)
                  {
                    curprint("(");
                    unparseFormatItemList(formatItem->get_format_item_list(),info);
                    curprint(")");
                  }
                 else
                  {
                 // This is the case of "format (10/)" which processes "10" and "/" seperately (I think this is a bug, see test2007_241.f).

                    printf ("Error: both get_data() and get_format_item_list() are NULL \n");

                 // In this case we want to avoid "10,/" to be  output!
                    skip_comma = true;
                  }
             }

          i++;

          if (i != formatList.end() && skip_comma == false )
             {
               curprint (",");
             }
        }
   }

void
FortranCodeGeneration_locatedNode::unparseFormatStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
  // Note that we use a SgStringVal in the SgFormatItem to hold a string which is not really 
  // interpreted as a literal in the Fortram grammar (I think).

     SgFormatStatement* formatStatement = isSgFormatStatement(stmt);
     curprint("format ( ");

     SgFormatItemList* formatItemList = formatStatement->get_format_item_list();
     unparseFormatItemList(formatItemList,info);
     curprint(" )");

     unp->cur.insert_newline(1);
   }

void
FortranCodeGeneration_locatedNode::unparseImportStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgImportStatement* importStatement = isSgImportStatement(stmt);
     SgExpressionPtrList & importList = importStatement->get_import_list();
     SgExpressionPtrList::iterator i = importList.begin();

     curprint("import ");
     if (importList.size() > 0) curprint(":: ");

     while (i != importList.end())
        {
          unparseExpression(*i,info);

          i++;

       // Put a little space before the next name (it there are multiple names specified)
          if (i != importList.end())
             {
               curprint(", ");
             }
        }

     unp->cur.insert_newline(1);
   }

bool
unparseDimensionStatementForArrayVariable( SgPntrArrRefExp* arrayReference )
   {
  // If an array variable has an explicit variable declaration (in the code) then the dimension 
  // information will be output there.  If not then we have to output the dimension statement
  // and an entry for this variable.

     ASSERT_not_null(arrayReference);
     SgVarRefExp* variableReference = isSgVarRefExp(arrayReference->get_lhs_operand());
     ASSERT_not_null(variableReference);
     SgVariableSymbol* variableSymbol = variableReference->get_symbol();
     ASSERT_not_null(variableSymbol);
     SgInitializedName* variableName = variableSymbol->get_declaration();
     ASSERT_not_null(variableName);

     SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(variableName->get_parent());

  // If there is a SgVariableDeclaration then it is simpler to look for it in the scope, 
  // else we have to look at each variable declaration for the SgInitializedName (which 
  // is only more expensive).
     bool foundArrayVariableDeclaration = false;
     if (variableDeclaration != nullptr)
        {
          SgScopeStatement* variableScope = variableDeclaration->get_scope();
          switch(variableScope->variantT())
             {
               case V_SgBasicBlock:
                  {
                    SgBasicBlock* basicBlock = isSgBasicBlock(variableScope);
                    SgStatementPtrList statementList = basicBlock->get_statements();
                    SgStatementPtrList::iterator i = find(statementList.begin(),statementList.end(),variableDeclaration);
                    foundArrayVariableDeclaration = (i != statementList.end());
                    break;
                  }

               default:
                  {
                    printf ("Default reached, variableScope = %p = %s \n",variableScope,variableScope->class_name().c_str());
                    ROSE_ABORT();
                  }
             }
        }
       else
        {
       // There was no variable declaration found though the symbol, so we have to look 
       // for the SgInitializedName in each SgVariableDeclaration.  However there will be
       // at least two (and hopefully no more) SgInitializedName objects for a function 
       // parameter if it also has an explicit declaration in a SgVariableDeclaration.
       // It would be cleaner to have one, and it could be consistant with old Style K&R C,
       // however it is not clear if this would be a problem for where we would visit the 
       // IR node twice in the traversals.  Need to look at the implementation of the old 
       // style C function parameter handling.

          SgScopeStatement* variableScope = variableName->get_scope();

          SgFunctionDefinition* functionDefinition   = isSgFunctionDefinition (variableScope);
          ASSERT_not_null(functionDefinition);

       // SgFunctionDeclaration* functionDeclaration = functionDefinition->get_declaration();

          SgBasicBlock* basicBlock = functionDefinition->get_body();
          ASSERT_not_null(basicBlock);

          SgStatementPtrList statementList = basicBlock->get_statements();

          SgStatementPtrList::iterator i = statementList.begin();
          while (i != statementList.end())
             {
               SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(*i);
               if (variableDeclaration != nullptr)
                  {
                    SgInitializedNamePtrList & variableList = variableDeclaration->get_variables();
                    SgInitializedNamePtrList::iterator i = find(variableList.begin(),variableList.end(),variableName);

                    foundArrayVariableDeclaration = (i != variableList.end());
                  }

               i++;
             }
        }

  // If we found the variation declaration then we do NOT need to output the dimension 
  // statement (since the array will be dimensioned in the variable declaration).
     return (foundArrayVariableDeclaration == false);
   }


bool
unparseDimensionStatement(SgStatement* stmt)
   {
  // DQ (12/9/2007): If the dimension statement is what declares a variable (array) then we need it,
  // else it is redundant (and an error) when used with the dimensioning specification in the variable
  // declaration (which will be built from the type information in the variable declaration.

     SgAttributeSpecificationStatement* attributeSpecificationStatement = isSgAttributeSpecificationStatement(stmt);

     bool unparseDimensionStatementResult = false;

     ROSE_ASSERT(attributeSpecificationStatement->get_attribute_kind() == SgAttributeSpecificationStatement::e_dimensionStatement);

     ASSERT_not_null(attributeSpecificationStatement->get_parameter_list());
     SgExpressionPtrList & parameterList = attributeSpecificationStatement->get_parameter_list()->get_expressions();
     SgExpressionPtrList::iterator i = parameterList.begin();

  // Loop over the array variables and see if there is an explicit declaration for it.
  // If so then the dimension information will be output in the associated SgVariableDeclaration.
     while (i != parameterList.end())
        {
          SgPntrArrRefExp* arrayReference = isSgPntrArrRefExp(*i);
          ASSERT_not_null(arrayReference);
          bool unparseForArrayVariable = unparseDimensionStatementForArrayVariable(arrayReference);

          if (unparseForArrayVariable)
               unparseDimensionStatementResult = true;
          i++;
        }

     return unparseDimensionStatementResult;
   }

void
FortranCodeGeneration_locatedNode::unparseAttributeSpecificationStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgAttributeSpecificationStatement* attributeSpecificationStatement = isSgAttributeSpecificationStatement(stmt);

     if (attributeSpecificationStatement->get_attribute_kind() == SgAttributeSpecificationStatement::e_dimensionStatement)
        {
       // The dimension statement will have changed the type and the original declaration will have been
       // output with the dimension computed as part of the type. The only exception is that there may 
       // have been no explicit declaration (only an implicit declaration from the dimension statement).

       // DQ (12/9/2007):
       // This test checks if we will need a dimension statement, we still might not want all entries in
       // the dimension statement to be unparsed (because some, but not all, might have appeared in an 
       // explicit declaration previously. I hate this part of Fortran!

          if (unparseDimensionStatement(stmt) == false)
             {
            // Output the new line so that we leave a hole where the dimension statement was and don't
            // screwup the formatting of the labels (in columns 1-6)
            // curprint("! Skipping output of dimension statement (handled in declaration)");
               unp->cur.insert_newline(1);
               return;
             }
        }

     string name;
     switch(attributeSpecificationStatement->get_attribute_kind())
        {
          case SgAttributeSpecificationStatement::e_unknown_attribute_spec: name = "unknown_attribute"; break;
          case SgAttributeSpecificationStatement::e_accessStatement_private:name = "private";           break;
          case SgAttributeSpecificationStatement::e_accessStatement_public: name = "public";            break;
          case SgAttributeSpecificationStatement::e_allocatableStatement:   name = "allocatable";       break;
          case SgAttributeSpecificationStatement::e_asynchronousStatement:  name = "asynchronous";      break;
          case SgAttributeSpecificationStatement::e_bindStatement:          name = "bind";              break;
          case SgAttributeSpecificationStatement::e_dataStatement:          name = "data";              break;
          case SgAttributeSpecificationStatement::e_dimensionStatement:     name = "dimension";         break;
          case SgAttributeSpecificationStatement::e_externalStatement:      name = "external";          break;
          case SgAttributeSpecificationStatement::e_intentStatement:        name = "intent";            break;
          case SgAttributeSpecificationStatement::e_intrinsicStatement:     name = "intrinsic";         break;
          case SgAttributeSpecificationStatement::e_optionalStatement:      name = "optional";          break;
          case SgAttributeSpecificationStatement::e_parameterStatement:     name = "parameter";         break;
          case SgAttributeSpecificationStatement::e_pointerStatement:       name = "pointer";           break;
          case SgAttributeSpecificationStatement::e_protectedStatement:     name = "protected";         break;
          case SgAttributeSpecificationStatement::e_saveStatement:          name = "save";              break;
          case SgAttributeSpecificationStatement::e_targetStatement:        name = "target";            break;
          case SgAttributeSpecificationStatement::e_valueStatement:         name = "value";             break;
          case SgAttributeSpecificationStatement::e_volatileStatement:      name = "volatile";          break;
          case SgAttributeSpecificationStatement::e_last_attribute_spec:    name = "last_attribute";    break;

          default:
             {
               printf ("Error: default reached %d \n",attributeSpecificationStatement->get_attribute_kind());
               ROSE_ABORT();
             }
        }

     curprint(name);

     if (attributeSpecificationStatement->get_attribute_kind() == SgAttributeSpecificationStatement::e_intentStatement)
        {
       // This define is copied from OFP actionEnum.h This needs to be better handled later (using a proper enum type).
#define IntentSpecBase 600
#ifndef _MSC_VER
                        // tps (02/02/2010) : error C2513: 'const int' : no variable declared before '='
          const int IN    = IntentSpecBase+0;
          const int OUT   = IntentSpecBase+1;
          const int INOUT = IntentSpecBase+2;

          string intentString;
          switch(attributeSpecificationStatement->get_intent())
             {
               case IN:    intentString = "in";    break;
               case OUT:   intentString = "out";   break;
               case INOUT: intentString = "inout"; break;

               default:
                  {
                    printf ("Error: default reached attributeSpecificationStatement->get_intent() = %d \n",attributeSpecificationStatement->get_intent());
                    ROSE_ABORT();
                  }
             }

          curprint("(" + intentString + ")");
#endif
        }

  // The parameter statement is a bit different from the other attribute statements (perhaps enough for it to be it's own IR node.
     if (attributeSpecificationStatement->get_attribute_kind() == SgAttributeSpecificationStatement::e_parameterStatement)
        {
          ASSERT_not_null(attributeSpecificationStatement->get_parameter_list());

          curprint("(");
          unparseExpression(attributeSpecificationStatement->get_parameter_list(),info);
          curprint(")");
        }

     if ( attributeSpecificationStatement->get_attribute_kind() == SgAttributeSpecificationStatement::e_bindStatement )
        {
          ASSERT_not_null(attributeSpecificationStatement->get_bind_list());
          ROSE_ASSERT(attributeSpecificationStatement->get_declarationModifier().isBind());
          
          curprint("(");
          curprint(attributeSpecificationStatement->get_linkage());
          if (attributeSpecificationStatement->get_binding_label().empty() == false)
             {
               curprint(",NAME=\"");
               curprint(attributeSpecificationStatement->get_binding_label());
               curprint("\"");
             }
          curprint(")");
        }

     if ( (attributeSpecificationStatement->get_attribute_kind() != SgAttributeSpecificationStatement::e_parameterStatement) &&
          (attributeSpecificationStatement->get_attribute_kind() != SgAttributeSpecificationStatement::e_dataStatement) && 
          ( (attributeSpecificationStatement->get_attribute_kind() != SgAttributeSpecificationStatement::e_accessStatement_private && 
             attributeSpecificationStatement->get_attribute_kind() != SgAttributeSpecificationStatement::e_accessStatement_public) && 
             attributeSpecificationStatement->get_parameter_list() != nullptr) )
        {
       // The parameter and data statement do not use "::" in their syntax
          curprint(" :: ");
        }
       else
        {
       // Need a space to prevent variables from being too close to the keywords (e.g. "privatei" should be "private i").
          curprint(" ");
        }

     if ( attributeSpecificationStatement->get_attribute_kind() == SgAttributeSpecificationStatement::e_bindStatement )
        {
          ASSERT_not_null(attributeSpecificationStatement->get_bind_list());

          unparseExpression(attributeSpecificationStatement->get_bind_list(),info);
        }

     if ( attributeSpecificationStatement->get_attribute_kind() == SgAttributeSpecificationStatement::e_allocatableStatement )
        {
          ASSERT_not_null(attributeSpecificationStatement->get_parameter_list());

          unparseExpression(attributeSpecificationStatement->get_parameter_list(),info);
        }

     if ( attributeSpecificationStatement->get_attribute_kind() == SgAttributeSpecificationStatement::e_externalStatement )
        {
       // for this case the functions need to be output just as names without the "()"
          ASSERT_not_null(attributeSpecificationStatement->get_parameter_list());

          SgExpressionPtrList & functionNameList = attributeSpecificationStatement->get_parameter_list()->get_expressions();
          SgExpressionPtrList::iterator i = functionNameList.begin();
          while (i != functionNameList.end())
             {
               SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(*i);
               ASSERT_not_null(functionRefExp);

               SgName name = functionRefExp->get_symbol()->get_name();
               curprint(name);
               
               i++;

               if (i != functionNameList.end())
                    curprint(", ");
             }
        }

     if ( attributeSpecificationStatement->get_attribute_kind() == SgAttributeSpecificationStatement::e_dataStatement )
        {
          curprint(" ");

          SgDataStatementGroupPtrList & dataStatementGroupList = attributeSpecificationStatement->get_data_statement_group_list();
          SgDataStatementGroupPtrList::iterator i_group = dataStatementGroupList.begin();
          while (i_group != dataStatementGroupList.end())
             {
               SgDataStatementObjectPtrList & dataStatementObjectList = (*i_group)->get_object_list();
               SgDataStatementObjectPtrList::iterator i_object = dataStatementObjectList.begin();

               while (i_object != dataStatementObjectList.end())
                  {
                    unparseExpression((*i_object)->get_variableReference_list(),info);
                    i_object++;
                    if (i_object != dataStatementObjectList.end())
                       {
                         curprint(", ");
                       }
                  }

            // Now output the data values
               curprint(" / ");
               SgDataStatementValuePtrList  & dataStatementValueList  = (*i_group)->get_value_list();
               SgDataStatementValuePtrList::iterator i_value  = dataStatementValueList.begin();
               while (i_value != dataStatementValueList.end())
                  {
                    SgDataStatementValue::data_statement_value_enum value_kind = (*i_value)->get_data_initialization_format();
                    switch(value_kind)
                       {
                         case SgDataStatementValue::e_unknown:
                         case SgDataStatementValue::e_default:
                            {
                              printf ("Error: value_kind == e_unknown or e_default value_kind = %d \n",value_kind);
                              ROSE_ABORT();
                            }

                         case SgDataStatementValue::e_explict_list:
                            {
                              unparseExpression((*i_value)->get_initializer_list(),info);
                              break;
                            }

                         case SgDataStatementValue::e_implicit_list:
                            {
                              ROSE_ASSERT((*i_value)->get_initializer_list()->get_expressions().empty());

                              SgExpression* repeatExpression   = (*i_value)->get_repeat_expression();
                              ASSERT_not_null(repeatExpression);
                              SgExpression* constantExpression = (*i_value)->get_constant_expression();
                              ASSERT_not_null(constantExpression);

                              unparseExpression(repeatExpression,info);
                              curprint(" * ");
                              unparseExpression(constantExpression,info);
                              break;
                            }

                         case SgDataStatementValue::e_implied_do:
                            {
                              printf ("Error: value_kind == e_implied_do (not yet supported) \n");
                              break;
                            }

                         default:
                            {
                              printf ("Error: default reached value_kind = %d \n",value_kind);
                              ROSE_ABORT();
                            }
                       }

                    i_value++;
                    if (i_value != dataStatementValueList.end())
                       {
                         curprint(", ");
                       }
                  }
               curprint(" / ");

               i_group++;
               if (i_group != dataStatementGroupList.end())
                  {
                    curprint(", ");
                  }
             }
        }


     if ( attributeSpecificationStatement->get_attribute_kind() == SgAttributeSpecificationStatement::e_dimensionStatement )
        {
          ASSERT_not_null(attributeSpecificationStatement->get_parameter_list());

          ASSERT_not_null(attributeSpecificationStatement->get_parameter_list());
          SgExpressionPtrList & parameterList = attributeSpecificationStatement->get_parameter_list()->get_expressions();
          SgExpressionPtrList::iterator i = parameterList.begin();

       // Loop over the array variables and see if there is an explicit declaration for it.
       // If so then this SgPntrArrRefExp will be output in this dimension statement, else
       // the dimension information was output as part of the variable declaration.
          bool unparseComma = false;
          while (i != parameterList.end())
             {
               SgPntrArrRefExp* arrayReference = isSgPntrArrRefExp(*i);
               ASSERT_not_null(arrayReference);

               bool unparseForArrayVariable = unparseDimensionStatementForArrayVariable(arrayReference);
               if (unparseForArrayVariable)
                  {
                    if (unparseComma)
                       {
                         curprint(", ");
                       }

                    unparseComma = true;
                    unparseExpression(arrayReference,info);
                  }

               i++;
             }

        }

     const SgStringList & localList = attributeSpecificationStatement->get_name_list();

  // We need to recognize the commonblockobject in the list
     Rose_STL_Container<SgNode*> commonBlockList = NodeQuery::querySubTree (attributeSpecificationStatement->get_scope(),V_SgCommonBlockObject);

     SgStringList::const_iterator i = localList.begin();
     string outputName = "";
     while (i != localList.end())
        {
          outputName = *i;
          for (Rose_STL_Container<SgNode*>::iterator j = commonBlockList.begin(); j != commonBlockList.end(); j++)
            {
               SgCommonBlockObject* commonBlockObject = isSgCommonBlockObject(*j);
               ROSE_ASSERT(commonBlockObject);
               string blockName = commonBlockObject->get_block_name();
               if (namesMatch(blockName, outputName))
                {
                  outputName = "/" + outputName + "/";
                  break;
                }
            }

          curprint(outputName);

          i++;
          if (i != localList.end())
             {
               curprint(", ");
             }
        }

     unp->cur.insert_newline(1);
   }


void
FortranCodeGeneration_locatedNode::unparseImplicitStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgImplicitStatement* implicitStatement = isSgImplicitStatement(stmt);

     if (implicitStatement->get_implicit_none())
        {
          curprint_keyword("IMPLICIT", info);
          curprint(" ");
          curprint_keyword("NONE", info);
          curprint(" ");

          switch(implicitStatement->get_implicit_spec())
            {
              case SgImplicitStatement::e_none_external:          curprint(" (EXTERNAL)");       break;
              case SgImplicitStatement::e_none_type:              curprint(" (TYPE)");           break;
              case SgImplicitStatement::e_none_external_and_type: curprint(" (EXTERNAL, TYPE)"); break;
              default: break;
            }
        }
       else
        {
       // This is a range such as "DOUBLE PRECISION (D-E)" or a singleton such as "COMPLEX (C)"

          SgInitializedNamePtrList & nameList =  implicitStatement->get_variables();
          if (nameList.empty())
             {
            // For now I just want to skip where alternative implicit rules are specified.
               if ( SgProject::get_verbose() >= 1 )
                    printf ("***** WARNING: in unparser NON \"IMPLICT NONE\" STATEMENT NOT YET FINISHED (skipped, default implicit type rules apply) *****\n");
             }
            else
             {
               ROSE_ASSERT(nameList.empty() == false);

               curprint_keyword("IMPLICIT", info);
               curprint(" ");

               SgInitializedNamePtrList::iterator i = nameList.begin();
            // DQ (12/2/2010): New code to handle implicit statements.
               while (i != nameList.end())
                  {
                    SgInitializedName* implicitTypeName = *i;

                    unp->u_fortran_type->unparseType(implicitTypeName->get_type(),info);
                    curprint("(");
                    curprint(implicitTypeName->get_name().str());
                    curprint(")");

                    i++;
                    if (i != nameList.end())
                         curprint(",");
               }
             }
        }

     unp->cur.insert_newline(1);
   }

void
FortranCodeGeneration_locatedNode::unparseBlockDataStmt(SgStatement*, SgUnparse_Info&)
   {
     printf ("Sorry, unparseBlockDataStmt() not implemented \n");
   }

void
FortranCodeGeneration_locatedNode::unparseStatementFunctionStmt(SgStatement*, SgUnparse_Info&)
   {
     printf ("Sorry, unparseStatementFunctionStmt() not implemented \n");
   }

void
FortranCodeGeneration_locatedNode::unparseWhereStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // Currently the simple "where (a) b = 0" is unparsed as "where (a) b = 0 endwhere"

     SgWhereStatement* whereStatement = isSgWhereStatement(stmt);
     ASSERT_not_null(whereStatement);

     if (whereStatement->get_string_label().empty() == false)
        {
       // Output the string label
          curprint(whereStatement->get_string_label() + ": ");
        }

     curprint("WHERE (");
     unparseExpression(whereStatement->get_condition(), info);
     curprint(") ");

     bool output_endwhere = whereStatement->get_has_end_statement();

     if (output_endwhere)
        {
          ASSERT_not_null(whereStatement->get_body());
          unparseStatement(whereStatement->get_body(),info);
        }
       else
        {
          SgStatementPtrList & statementList = whereStatement->get_body()->get_statements();
          ROSE_ASSERT(statementList.size() == 1);
          SgStatement* statement = *(statementList.begin());
          ASSERT_not_null(statement);
          SgUnparse_Info info_without_formating(info);
          info_without_formating.set_SkipFormatting();
          unparseStatement(statement, info_without_formating);
        }

     SgElseWhereStatement* elsewhereStatement = whereStatement->get_elsewhere();
     if (elsewhereStatement != nullptr)
        {
          if (output_endwhere)
             {
               unparseStatement(elsewhereStatement,info);
             }
            else
             {
            // Output the statement on the same line as the "else"
               SgStatementPtrList & statementList = elsewhereStatement->get_body()->get_statements();
               ROSE_ASSERT(statementList.size() == 1);
               SgStatement* statement = *(statementList.begin());
               ASSERT_not_null(statement);
               printf ("Output false statement = %p = %s \n",statement,statement->class_name().c_str());
               SgUnparse_Info info_without_formating(info);
               info_without_formating.set_SkipFormatting();
               unparseStatement(statement, info_without_formating);
             }
        }

  // The end where statement can have a label
     if (output_endwhere)
        {
          unparseStatementNumbersSupport(whereStatement->get_end_numeric_label(),info);
          curprint("END WHERE");
          if (whereStatement->get_string_label().empty() == false)
             {
            // Output the string label
               curprint(" " + whereStatement->get_string_label());
             }
        }

     unp->cur.insert_newline(1);
   }

void
FortranCodeGeneration_locatedNode::unparseElseWhereStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgElseWhereStatement* elseWhereStatement = isSgElseWhereStatement(stmt);
     ASSERT_not_null(elseWhereStatement);

     curprint("ELSEWHERE ");

     ASSERT_not_null(elseWhereStatement->get_condition());

  // Only unparse the "()" if there is a valid elsewhere mask.
     if (isSgNullExpression(elseWhereStatement->get_condition()) == nullptr)
        {
          curprint("(");
          unp->u_exprStmt->unparseExpression(elseWhereStatement->get_condition(), info);
          curprint(")");
        }

     ASSERT_not_null(elseWhereStatement->get_body());
     unparseStatement(elseWhereStatement->get_body(),info);

     SgElseWhereStatement* nested_elseWhereStatement = elseWhereStatement->get_elsewhere();
     if (nested_elseWhereStatement != nullptr)
        {
          unparseStatement(nested_elseWhereStatement,info);
        }
   }

void
FortranCodeGeneration_locatedNode::unparseNullifyStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
       curprint("NULLIFY ");
       curprint("(");
       SgExprListExp* dlist = (isSgNullifyStatement(stmt))->get_pointer_list();
       SgExpressionPtrList::iterator i = dlist->get_expressions().begin();
       while (i != dlist->get_expressions().end())
        {
          unparseExpression(*i,info);
          i++;

          if (i != dlist->get_expressions().end())
             {
               curprint(", ");
             }
        }

     curprint(")");
     unp->cur.insert_newline(1);
   }

void
FortranCodeGeneration_locatedNode::unparseEquivalenceStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // This organization is as a SgExprListExp of SgExprListExp of SgExpression objects.
  // This we can represent: "equivalence (i,j), (k,l,m,n)"

     SgEquivalenceStatement* equivalenceStatement = isSgEquivalenceStatement(stmt);

     curprint("equivalence ");

     ASSERT_not_null(equivalenceStatement->get_equivalence_set_list());

     SgExpressionPtrList & expressionList = equivalenceStatement->get_equivalence_set_list()->get_expressions();
     SgExpressionPtrList::iterator i = expressionList.begin();
     while (i != expressionList.end())
        {
          curprint("( ");
          unparseExpression(*i,info);
          curprint(" )");

          i++;

          if (i != expressionList.end())
             {
               curprint(", ");
             }
        }

     unp->cur.insert_newline(1);
   }

void
FortranCodeGeneration_locatedNode::unparseLabel ( SgLabelRefExp* exp )
   {
     ASSERT_not_null(exp);
     SgLabelSymbol* symbol = exp->get_symbol();
     ASSERT_not_null(symbol);

  // DQ (12/24/2007): Every numeric label should have been associated with a statement!
     ASSERT_not_null(symbol->get_fortran_statement());
     int numericLabel = symbol->get_numeric_label_value();

     curprint( StringUtility::numberToString(numericLabel) );
   }

void
FortranCodeGeneration_locatedNode::unparseArithmeticIfStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgArithmeticIfStatement* arithmeticIf = isSgArithmeticIfStatement(stmt);
     ASSERT_not_null(arithmeticIf);
     ROSE_ASSERT(arithmeticIf->get_conditional());

  // condition
     curprint("IF (");
     info.set_inConditional();

  // DQ (8/15/2007): In C the condiion is a statment, and in Fortran the condition is an expression!
  // We might want to fix this by having an IR node to represent the Fortran "if" statement.
     SgExpression* expression = isSgExpression(arithmeticIf->get_conditional());
     unparseExpression(expression, info);

     info.unset_inConditional();
     curprint(") ");

     unparseLabel(arithmeticIf->get_less_label());
     curprint(",");
     unparseLabel(arithmeticIf->get_equal_label());
     curprint(",");
     unparseLabel(arithmeticIf->get_greater_label());

     unp->cur.insert_newline(1);
   }

void
FortranCodeGeneration_locatedNode::unparseAssignStmt(SgStatement*, SgUnparse_Info&)
   {
     printf ("Sorry, unparseAssignStmt() not implemented \n");
   }

void
FortranCodeGeneration_locatedNode::unparseComputedGotoStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgComputedGotoStatement* computedGoto = isSgComputedGotoStatement(stmt);

     curprint("GOTO (");

     ASSERT_not_null(computedGoto->get_labelList());
     SgExpressionPtrList & labelList = computedGoto->get_labelList()->get_expressions();

     int size = labelList.size();
     for (int i=0; i < size; i++)
        {
          SgLabelRefExp* labelRefExp = isSgLabelRefExp(labelList[i]);
          ASSERT_not_null(labelRefExp);

          SgLabelSymbol* labelSymbol = labelRefExp->get_symbol();

       // DQ (12/24/2007): Every numeric label should have been associated with a statement!
          ASSERT_not_null(labelSymbol->get_fortran_statement());
          int numericLabel = labelSymbol->get_numeric_label_value();

          ROSE_ASSERT(numericLabel >= 0);
          string numericLabelString = StringUtility::numberToString(numericLabel);
          curprint(numericLabelString);

          if (i < size-1)
             {
               curprint(", ");
             }
        }

     curprint(" ) ");

     unparseExpression(computedGoto->get_label_index(), info);
     unp->cur.insert_newline(1);
   }

void
FortranCodeGeneration_locatedNode::unparseAssignedGotoStmt(SgStatement*, SgUnparse_Info&)
   {
     printf ("Sorry, unparseAssignedGotoStmt() not implemented \n");
   }

//----------------------------------------------------------------------------
//  FortranCodeGeneration_locatedNode::<program units>
//----------------------------------------------------------------------------

void 
FortranCodeGeneration_locatedNode::unparseModuleStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // Sage node corresponds to Fortran module

     SgModuleStatement* mod = isSgModuleStatement(stmt);
     ROSE_ASSERT(mod);

     curprint("MODULE ");
     curprint(mod->get_name().str());

  // body
     ASSERT_not_null(mod->get_definition());
     SgUnparse_Info ninfo(info);
     unparseStatement(mod->get_definition(), ninfo);

     unparseStatementNumbersSupport(mod->get_end_numeric_label(),info);

     curprint("END MODULE");
     ASSERT_not_null(unp);
     unp->cur.insert_newline(1);
   }

void 
FortranCodeGeneration_locatedNode::unparseProgHdrStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // Sage node corresponds to Fortran program

     SgProgramHeaderStatement* proghdr = isSgProgramHeaderStatement(stmt);
     ASSERT_not_null(proghdr);

     if (!proghdr->isForward() && proghdr->get_definition() != nullptr && !info.SkipFunctionDefinition())
        {
       // Output the function declaration with definition

       // The unparsing of the definition will cause the unparsing of the declaration (with SgUnparse_Info
       // flags set to just unparse a forward declaration!)
          SgUnparse_Info ninfo(info);

       // To avoid end of statement formatting (added CR's) we call the unparseFuncDefnStmt directly
          unparseFuncDefnStmt(proghdr->get_definition(), ninfo);

          unparseStatementNumbersSupport(proghdr->get_end_numeric_label(),info);

       // This is a special name for the case where the program header should not be output (it did not appear in the original source file).
          if (proghdr->get_name() != ROSE_IMPLICIT_FORTRAN_PROGRAM_NAME)
             {
            // The "END" has just been output by the unparsing of the SgFunctionDefinition so just add "PROGRAM <name>".
               curprint("END PROGRAM ");
               if (proghdr->get_named_in_end_statement())
                  {
                    curprint(proghdr->get_name().str());
                  }

            // Output 2 new lines to better separate functions visually in the output
               unp->cur.insert_newline(1);
               unp->cur.insert_newline(2); //FMZ
             }
            else
             {
            // And "end" is always required even if the program-stmt is not explicitly used.
               curprint("END ");

            // Added to fix problem reported by Liao (email 12/28/2007).
               unp->cur.insert_newline(1);
             }
        }
       else
        {
       // Output the forward declaration only

       // This is a special name for the case where the program header should not be output (it did not appear in the original source file).
          if (proghdr->get_name() != ROSE_IMPLICIT_FORTRAN_PROGRAM_NAME)
             {
            // Are there possible qualifiers that we are missing?
               curprint("PROGRAM ");
               curprint(proghdr->get_name().str());
             }

       // Output 1 new line so that new statements will appear on their own line after the SgProgramHeaderStatement declaration.
          unp->cur.insert_newline(1);
        }
   }


//----------------------------------------------------------------------------
//  FortranCodeGeneration_locatedNode::<declarations>
//----------------------------------------------------------------------------

void
FortranCodeGeneration_locatedNode::unparseInterfaceStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // Sage node corresponds to Fortran interface statement
     SgInterfaceStatement* interfaceStatement = isSgInterfaceStatement(stmt);
     ASSERT_not_null(interfaceStatement);

     string nm = interfaceStatement->get_name().str();
     curprint("INTERFACE ");

     switch (interfaceStatement->get_generic_spec())
        {
          case SgInterfaceStatement::e_named_interface_type:
             {
               curprint(nm);
               break;
             }
          case SgInterfaceStatement::e_operator_interface_type:
             {
               curprint("operator(");
               curprint(nm);
               curprint(")");
               break;
             }
          case SgInterfaceStatement::e_assignment_interface_type:
             {
               curprint("assignment(");
               curprint(nm);
               curprint(")");
               break;
             }
          case SgInterfaceStatement::e_unnamed_interface_type:
             {
            // Nothing to do for this case!
               break;
             }
          default:
             {
               printf ("Error: value of interfaceStatement->get_generic_spec() = %d \n",interfaceStatement->get_generic_spec());
               ROSE_ABORT();
             }
        }

     unp->cur.insert_newline(1);

     for (size_t i = 0; i < interfaceStatement->get_interface_body_list().size(); i++)
        {
          bool outputFunctionName = interfaceStatement->get_interface_body_list()[i]->get_use_function_name();
          SgName functionName = interfaceStatement->get_interface_body_list()[i]->get_function_name();
          SgFunctionDeclaration* functionDeclaration = interfaceStatement->get_interface_body_list()[i]->get_functionDeclaration();

          if (outputFunctionName)
             {
               curprint("MODULE PROCEDURE ");
               curprint(functionName.str());
               unp->cur.insert_newline(1);
             }
            else
             {
               unparseStatement(functionDeclaration, info);
             }
        }

     unparseStatementNumbersSupport(interfaceStatement->get_end_numeric_label(),info);

     curprint("END INTERFACE ");

     ASSERT_not_null(unp);
     unp->cur.insert_newline(1);
   }

void
FortranCodeGeneration_locatedNode::unparseCommonBlock(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgCommonBlock* commonBlock = isSgCommonBlock(stmt);
     ASSERT_not_null(commonBlock);

     SgScopeStatement* parentScope = isSgScopeStatement(commonBlock->get_parent());
     ASSERT_not_null(parentScope);

     curprint("COMMON ");

     SgCommonBlockObjectPtrList & blockList = commonBlock->get_block_list();
     SgCommonBlockObjectPtrList::iterator i = blockList.begin();
     while (i != blockList.end())
        {
          curprint("/ ");
          curprint((*i)->get_block_name());
          curprint(" / ");
     // Pei-Hung (07/30/2020) if declaration stmt is not available, the type attribute has to be unparsed
          SgExprListExp* expr_list = isSgExprListExp((*i)->get_variable_reference_list()); 
          ASSERT_not_null(expr_list);
          SgExpressionPtrList::iterator iexp = expr_list->get_expressions().begin();

          if (iexp != expr_list->get_expressions().end())
             {
               while (true)
                  {
                    SgVarRefExp* varRef = isSgVarRefExp(*iexp);
                    if(varRef != nullptr)
                    {
                      SgVariableSymbol* varSym = isSgVariableSymbol(varRef->get_symbol());
                      ASSERT_not_null(varSym);
                      SgInitializedName* initName = isSgInitializedName(varSym->get_declaration());
                      ASSERT_not_null(initName);
                      SgVariableDeclaration* varDecl = isSgVariableDeclaration(initName->get_parent());
                      ASSERT_not_null(varDecl);
                      SgType* type = initName->get_typeptr();
                      ASSERT_not_null(type);
                      if(isSgBasicBlock(parentScope) != nullptr)
                      {
                        SgBasicBlock* basicBlock = isSgBasicBlock(parentScope); 
                        ASSERT_not_null(basicBlock);
                        SgStatementPtrList& stmtList = basicBlock->get_statements();
                        if(std::find(stmtList.begin(),stmtList.end(), varDecl) == stmtList.end())
                        {
                          unparseExpression(*iexp,info);
                          // third argument has to be false to have the attribute unparsed to individual variable
                          unparseEntityTypeAttr(type, info, false);
                        }
                        else
                          unparseExpression(*iexp,info);
                      }
                      else if(isSgGlobal(parentScope) != nullptr)
                      {
                        SgGlobal* globalScope = isSgGlobal(parentScope); 
                        ASSERT_not_null(globalScope);
                        SgDeclarationStatementPtrList& stmtList = globalScope->get_declarations();
                        if(std::find(stmtList.begin(),stmtList.end(), varDecl) == stmtList.end())
                        {
                          unparseExpression(*iexp,info);
                          // third argument has to be false to have the attribute unparsed to individual variable
                          unparseEntityTypeAttr(type, info, false);
                        }
                        else
                          unparseExpression(*iexp,info);
                      }
                      else 
                        unparseExpression(*iexp,info);
                    }
                    else
                      unparseExpression(*iexp,info);
                    iexp++;
                    if (iexp != expr_list->get_expressions().end())
                       {
                         curprint ( ",");
                       }
                      else
                       {
                         break;
                       }
                  }
             }

          i++;
          if (i != blockList.end())
             {
               curprint(", ");
             }
        }

     unp->cur.insert_newline(1);
   }

void
FortranCodeGeneration_locatedNode::unparseVarDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // Sage node corresponds to Fortran variable declaration

     SgVariableDeclaration* vardecl = isSgVariableDeclaration(stmt);
     ASSERT_not_null(vardecl);
  
  // In Fortran we should never have to deal with a type declaration
  // inside a variable declaration (e.g. struct A { int x; } a;)
     ROSE_ASSERT(vardecl->get_variableDeclarationContainsBaseTypeDefiningDeclaration() == false);

  // Build a new SgUnparse_Info object to represent formatting options
  // for this statement
     SgUnparse_Info ninfo(info);

  // FIXME: we may need to do something analagous for modules?
  // Check to see if this is an object defined within a class

     SgName inCname;
     ROSE_ASSERT(vardecl->get_parent());
     SgClassDefinition *cdefn = isSgClassDefinition(vardecl->get_parent());
     if (cdefn)
        {
          inCname = cdefn->get_declaration()->get_name();
          if (cdefn->get_declaration()->get_class_type()        == SgClassDeclaration::e_class)
               ninfo.set_CheckAccess();
        }

  // Save the input information
     SgUnparse_Info saved_ninfo(ninfo);

  // Setup the SgUnparse_Info object for this statement
     ninfo.unset_CheckAccess();
     info.set_access_attribute(ninfo.get_access_attribute());
     SgInitializedNamePtrList::iterator p = vardecl->get_variables().begin();
     unparseVarDecl(vardecl, *p, ninfo);
     ninfo.set_SkipBaseType();
     p++;
     while (p != vardecl->get_variables().end()) {
         curprint(", ");
         unparseVarDecl(vardecl, *p, ninfo);
         p++;
     }

     unp->cur.insert_newline(1);
   }

void
FortranCodeGeneration_locatedNode::unparseVarDefnStmt(SgStatement* stmt, SgUnparse_Info&)
{
  // Sage node has no Fortran correspondence
  SgVariableDefinition* vardefn_stmt = isSgVariableDefinition(stmt);
  ASSERT_not_null(vardefn_stmt);
  ROSE_ASSERT(false && "FortranCodeGeneration_locatedNode::unparseVarDefnStmt");
}

void
FortranCodeGeneration_locatedNode::unparseParamDeclStmt(SgStatement*, SgUnparse_Info&)
{
  // Sage node corresponds to Fortran parameter declaration
  ROSE_ASSERT(false && "FortranCodeGeneration_locatedNode::unparseParamDeclStmt");
}

void
FortranCodeGeneration_locatedNode::unparseUseStmt(SgStatement* stmt, SgUnparse_Info&)
   {
  // Sage node corresponds to Fortran use statement
  
     SgUseStatement* useStmt = isSgUseStatement(stmt);
     ASSERT_not_null(useStmt);

     curprint("USE ");

     // Pei-Hung (03/09/21) added unparsing for module nature (intrinsic or non_intrinsic)
     std::string nature = useStmt->get_module_nature();
     if (nature != "")
        {
          curprint(", " + nature + " :: ");
        }
   
     curprint(useStmt->get_name().str());

     if (useStmt->get_only_option())
        {
         // FMZ: move comma here
          curprint(", ");
          curprint("ONLY : ");
        }

     int listSize = useStmt->get_rename_list().size();
     if (listSize > 0 && !useStmt->get_only_option())  // need to print a comma and a space
         curprint(", ");
     for (int i=0; i < listSize; i++)
        {
          SgRenamePair* renamePair = useStmt->get_rename_list()[i];
          ASSERT_not_null(renamePair);

          if (renamePair->isRename())
             {
               SgName local_name = renamePair->get_local_name();
               SgName use_name   = renamePair->get_use_name();
               curprint(local_name);
               curprint(" => ");
               curprint(use_name);
             }
            else
             {
               SgName use_name   = renamePair->get_use_name();
               curprint(use_name);
             }

          if (i < listSize-1)
               curprint(" , ");
        }

     unp->cur.insert_newline(1);
   }


//----------------------------------------------------------------------------
//  FortranCodeGeneration_locatedNode::<executable statements, control flow>
//----------------------------------------------------------------------------

void
FortranCodeGeneration_locatedNode::unparseBasicBlockStmt(SgStatement* bb, SgUnparse_Info& info)
{
  SgBasicBlock* block = isSgBasicBlock(bb);
  ASSERT_not_null(block);

  // space here is required to get "else if" blocks formatted correctly (at least).
  unp->cur.format(block, info, FORMAT_BEFORE_BASIC_BLOCK1);

  for (auto stmt : block->get_statements()) {
    ASSERT_not_null(stmt);
    // FMZ: for module file, only output the variable declarations (not definitions)
    // Pei-Hung (05/23/2019) Need to add SgUseStatement, SgimplicitStatement and SgDerivedTypeStatement into rmod file
    if ( !info.outputFortranModFile() || stmt->variantT()==V_SgVariableDeclaration
                 || stmt->variantT()==V_SgAttributeSpecificationStatement // DXN (02/07/2012): unparse attribute statements also
                 || stmt->variantT()==V_SgUseStatement
                 || stmt->variantT()==V_SgImplicitStatement
                 || stmt->variantT()==V_SgDerivedTypeStatement) {
      unparseStatement(stmt, info);
    }
  }

  // Liao (10/14/2010): This helps handle cases such as 
  //    c$OMP END PARALLEL
  //          END
  unparseAttachedPreprocessingInfo(block, info, PreprocessingInfo::inside);
}

bool
hasCStyleElseIfConstruction(SgIfStmt* parentIfStatement)
   {
  // Rasmussen(7/17/2018): Check for C style AST else-if construction.
  // The C style else-if AST doesn't have an SgBasicBlock immediately
  // preceding the SgIfStmt representing the else-if clause; the SgIfStmt
  // itself is the false branch.
  //
     SgIfStmt* else_if_stmt = isSgIfStmt(parentIfStatement->get_false_body());

     return (else_if_stmt != nullptr);
   }


SgIfStmt*
getElseIfStatement ( SgIfStmt* parentIfStatement )
   {
  // This returns the elseif statement in a SgIfStmt object, else returns NULL.

     SgIfStmt* childIfStatement{nullptr};

     SgBasicBlock* falseBlock = isSgBasicBlock(parentIfStatement->get_false_body());

  // Rasmussen (7/23/2018): Simplification of logic allowed because usage of is_else_if_statement
  // was fixed in frontend.  Previously is_else_if_statement was not used in the unparser and
  // this confused users at NCAR when attempting transformations.
  //
     if (falseBlock != nullptr)
        {
          if (falseBlock->get_statements().empty() == false)
             {
               childIfStatement = isSgIfStmt(*(falseBlock->get_statements().begin()));
               if (childIfStatement != nullptr)
                  {
                     if (childIfStatement->get_is_else_if_statement() == false)
                        {
                           childIfStatement = nullptr;
                        }
                  }
             }
        }

  // Rasmussen (7/23/2018): This branch added for the experimental Fortran parser where
  // the AST was designed to follow the C if statement.  For the new design there is no
  // SgBasicBlock immediately preceding the SgIfStmt (the SgIfStmt is the false branch).
  //
     SgIfStmt* else_if_stmt = isSgIfStmt(parentIfStatement->get_false_body());
     if (else_if_stmt != nullptr)
        {
           childIfStatement = else_if_stmt;
        }

     return childIfStatement;
   }


void 
FortranCodeGeneration_locatedNode::unparseIfStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // Sage node corresponding to Fortran 'if'
  //

  // Rasmussen(7/23/2018): Modified much of the unparsing of if statements and if
  // constructs because:
  //   1. The AST was modified to follow the C-style AST for the experimental branch.
  //   2. Original Fortran frontend AST construction also moved towards C by using NULL for false body.
  //   3. ELSE was not unparsed if the else-block was empty (no reason not to unparse it).
  //   4. A bug was fixed in unparsing the if-construct label name (label_string).
  //
     SgIfStmt* if_stmt = isSgIfStmt(stmt);
     ASSERT_not_null(if_stmt);
     ROSE_ASSERT(if_stmt->get_conditional());
     ROSE_ASSERT(if_stmt->get_true_body());

  // Output the if-construct-name string (if present) if this is an else-if branch
     if (if_stmt->get_string_label().empty() == false && if_stmt->get_is_else_if_statement() == false)
        {
          curprint(if_stmt->get_string_label() + ": ");
        }

  // IF keyword and conditional
     curprint("IF (");
     info.set_inConditional();

  // DQ (8/15/2007): In C the condition is a statement, and in Fortran the condition is an expression!
  // We might want to fix this by having an IR node to represent the Fortran "if" statement.
     SgExprStatement* expressionStatement = isSgExprStatement(if_stmt->get_conditional());
     unparseExpression(expressionStatement->get_expression(), info);

     info.unset_inConditional();
     curprint(") ");

  // DQ (12/26/2007): handling cases where endif is not in the source code and not required (stmt vs. construct)
  // This is also (primarily) used to keep else-if-stmt from printing extra END IF.
     bool output_endif = if_stmt->get_has_end_statement();

     SgIfStmt* elseIfStatement = getElseIfStatement ( if_stmt );

  // THEN keyword
  // DQ (12/26/2007): If this is an elseif statement then output the "THEN" even though we will not output an "ENDIF"
     if (output_endif)
        {
       // IF THEN statement branch
       //
          ROSE_ASSERT(if_stmt->get_use_then_keyword());
       // This branch taken for an if-then-stmt.
       // Note that the string label if output before "IF", not after "THEN"
          curprint("THEN");
          unparseStatement(if_stmt->get_true_body(), info);
        }
       else
        {
          if (if_stmt->get_use_then_keyword())
             {
            // ELSE IF statement branch (uses if_stmt to unparse the "IF")
            //
            // This branch taken for an else-if-stmt.
            // Note that the string label if output after "THEN"
               curprint("THEN");
            // Output the if-construct-name string after THEN if needed
               if (if_stmt->get_string_label().empty() == false) curprint(" " + if_stmt->get_string_label());
               unparseStatement(if_stmt->get_true_body(),info);
             }
            else
             {
            // IF statement branch (not if-construct)
            //
            // "THEN" is not output for the case of "IF (C) B = 0"
               ROSE_ASSERT (isSgBasicBlock(if_stmt->get_true_body()));
               SgStatementPtrList & statementList = isSgBasicBlock(if_stmt->get_true_body())->get_statements();
               ROSE_ASSERT(statementList.size() == 1);
               SgStatement* statement = *(statementList.begin());
               ASSERT_not_null(statement);

            // Fixed format code includes a call to insert 6 spaces (or numeric label if available), we want to suppress this.
               SgUnparse_Info info_without_formating(info);
               info_without_formating.set_SkipFormatting();
               unparseStatement(statement, info_without_formating);
             }
        }

  // ELSE and ELSE IF statements
  //
  // Rasmussen(7/23/2018): Added unparsing of C style else-if AST construction.  The C
  // AST does not use an SgBasicBlock to precede an SgIfStmt representing the else-if-stmt.
  // Also simplified the logic somewhat, now allowed because the false_body is NULL
  // if no else-stmt (changed in the frontend to follow C AST).
  //
     SgBasicBlock* fbb = isSgBasicBlock(if_stmt->get_false_body());

     if (fbb || hasCStyleElseIfConstruction(if_stmt))
        {
       // The else statement might have its own numeric label
          unparseStatementNumbersSupport(if_stmt->get_else_numeric_label(),info);
          curprint("ELSE");

       // However, currently there is no information on else if-construct name in
       // SgIfStmt so we won't try to unparse it.  NOTE, output could be different from input.

          if (elseIfStatement != nullptr)
             {
             // ELSE IF statement branch
                ROSE_ASSERT(elseIfStatement->get_is_else_if_statement());

             // Call the associated unparse function directly to avoid formatting
                curprint(" ");
                unparseIfStmt(elseIfStatement, info);
             }
            else
             {
             // ELSE statement branch
                unparseStatement(if_stmt->get_false_body(), info);
             }
        }

  // END IF statement
  //
     if (output_endif)
        {
          unparseStatementNumbersSupport(if_stmt->get_end_numeric_label(),info);
          curprint("END IF");
       // Output the if-construct-name string if present
          if (if_stmt->get_string_label().empty() == false) curprint(" " + if_stmt->get_string_label());
        }

     ASSERT_not_null(unp);
     unp->cur.insert_newline(1);
   }


void
FortranCodeGeneration_locatedNode::unparseForAllStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgForAllStatement* forAllStatement = isSgForAllStatement(stmt);
     ASSERT_not_null(forAllStatement);

  // The FORALL statement has been deprecated and replaced by a DO CONCURRENT construct.
  // Since they are very similar they share the same Sage node and are distinguished by an enum.
  //
     if (forAllStatement->get_forall_statement_kind() == SgForAllStatement::e_do_concurrent_statement)
        {
           unparseDoConcurrentStatement(stmt, info);
           return;
        }

  // Note the return in the preceding for DO CONCURRENT.  What follows unparses a FORALL construct.
  //
     ROSE_ASSERT(forAllStatement->get_forall_statement_kind() == SgForAllStatement::e_forall_statement);

     SgExprListExp* forAllHeader = forAllStatement->get_forall_header();
     ASSERT_not_null(forAllHeader);

     curprint("FORALL ( ");
     unparseExpression(forAllHeader,info);
     curprint(" ) ");

     SgStatement* statement{nullptr};
     if (forAllStatement->get_has_end_statement())
        {
          statement = forAllStatement->get_body();
          ASSERT_not_null(statement);

          unparseStatement(statement,info);
        }
       else
        {
          SgBasicBlock* body = isSgBasicBlock(forAllStatement->get_body());
          ASSERT_not_null(body);

          SgStatementPtrList & statementList = body->get_statements();
          ROSE_ASSERT(statementList.size() == 1);
          statement = *(statementList.begin());
          ASSERT_not_null(statement);

          unparseLanguageSpecificStatement(statement,info);
        }

     unp->cur.insert_newline(1);

     if (forAllStatement->get_has_end_statement())
        {
          unparseStatementNumbersSupport(forAllStatement->get_end_numeric_label(),info);
          curprint("END FORALL");
        }
   }


void
FortranCodeGeneration_locatedNode::unparseDoConcurrentStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgForAllStatement* forAllStatement = isSgForAllStatement(stmt);
     ASSERT_not_null(forAllStatement);

     SgExprListExp* forAllHeader = forAllStatement->get_forall_header();
     ASSERT_not_null(forAllHeader);

     SgExpressionPtrList header = forAllHeader->get_expressions();
     int num_vars = header.size();

     curprint("DO CONCURRENT (");

     for (int i = 0; i < num_vars; i++)
        {
           if (i != 0) curprint(", ");

           SgAssignOp* assignOp = isSgAssignOp(header[i]);
           ROSE_ASSERT(assignOp);

           unparseExpression(assignOp->get_lhs_operand_i(), info);
           curprint("=");
           unparseExpression(assignOp->get_rhs_operand_i(), info);
        }

     curprint(")");
     unp->cur.insert_newline(1);

  // Unparse the body
     SgStatement* statement{nullptr};
     if (forAllStatement->get_has_end_statement())
        {
          statement = forAllStatement->get_body();
          ASSERT_not_null(statement);

          unparseStatement(statement,info);
        }
       else
        {
          SgBasicBlock* body = isSgBasicBlock(forAllStatement->get_body());
          ASSERT_not_null(body);

          SgStatementPtrList & statementList = body->get_statements();
          ROSE_ASSERT(statementList.size() == 1);
          statement = *(statementList.begin());
          ASSERT_not_null(statement);

          unparseLanguageSpecificStatement(statement,info);
        }

     unp->cur.insert_newline(1);

  // Unparse the end statement
     if (forAllStatement->get_has_end_statement())
        {
          unparseStatementNumbersSupport(forAllStatement->get_end_numeric_label(),info);
          curprint("END DO");
          unp->cur.insert_newline(1);
        }
   }


void 
FortranCodeGeneration_locatedNode::unparseDoStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // Sage node corresponds to Fortran 'do'

  // This is a Fortran specific IR node and it stores its condition and increment differently 
  // (since Fortran uses only values to represent the bound and the stride instead of 
  // expressions that include the index variable).

     SgFortranDo* doloop = isSgFortranDo(stmt);
     ASSERT_not_null(doloop);

  // NOTE: for now we are responsible for unparsing the
  // initialization, condition and update expressions into a triplet.
  // We assume that these statements are of a very restricted form.
     SgExpression* initExp = doloop->get_initialization();

     SgExpression* condExp = doloop->get_bound();
     ASSERT_not_null(condExp);

     SgExpression* updateExp = doloop->get_increment();
     ASSERT_not_null(updateExp);

     if (doloop->get_string_label().empty() == false)
        {
       // Output the string label
          curprint(doloop->get_string_label() + ": ");
        }

     curprint("DO ");

     if (doloop->get_end_numeric_label() != nullptr)
        {
          SgLabelSymbol* endLabelSymbol = doloop->get_end_numeric_label()->get_symbol();
          ASSERT_not_null(endLabelSymbol);

          ASSERT_not_null(endLabelSymbol->get_fortran_statement());
          int loopEndLabel = endLabelSymbol->get_numeric_label_value();
          string numeric_label_string = StringUtility::numberToString(loopEndLabel);
          curprint(numeric_label_string + " ");
        }

     unparseExpression(initExp, info);
     if (isSgNullExpression(initExp) == nullptr)
        {
          curprint(", ");
          unparseExpression(condExp, info);
        }

  // If this is NOT a SgNullExpression, then output the "," and the stride expression.
     if (isSgNullExpression(updateExp) == nullptr)
        {
          curprint(", ");
          unparseExpression(updateExp, info);
        }

  // loop body (must always exist)
     SgStatement *body = doloop->get_body();
     unparseStatement(body, info);

     ASSERT_not_null(unp);
     unp->cur.insert_newline(1);

  // This setting converts all non-block where statements into blocked where statements.
  // So "DO I=1,2 B = 0" becomes:
  // "DO I=1,2
  //     B = 0
  //  END DO"

  // DQ (12/26/2007): handling cases where enddo is not in the source code and not required (stmt vs. construct)
     bool output_enddo = doloop->get_has_end_statement();

     if (output_enddo)
        {
          unparseStatementNumbersSupport(doloop->get_end_numeric_label(),info);

          curprint("END DO");
          if (doloop->get_string_label().empty() == false)
             {
            // Output the string label
               curprint(" " + doloop->get_string_label());
             }
        }

     ASSERT_not_null(unp);
     unp->cur.insert_newline(1);
   }

void 
FortranCodeGeneration_locatedNode::unparseWhileStmt(SgStatement* stmt, SgUnparse_Info& info) 
   {
  // Sage node corresponds to Fortran 'DO WHILE' (pre-test)

     SgWhileStmt* while_stmt = isSgWhileStmt(stmt);
     ASSERT_not_null(while_stmt);

     if (while_stmt->get_string_label().empty() == false)
        {
       // Output the string label
          curprint(while_stmt->get_string_label() + ": ");
        }

     curprint_keyword("DO", info);
     curprint(" ");

     if (while_stmt->get_end_numeric_label() != nullptr)
        {
          SgLabelSymbol* endLabelSymbol = while_stmt->get_end_numeric_label()->get_symbol();

          ASSERT_not_null(endLabelSymbol->get_fortran_statement());
          int loopEndLabel = endLabelSymbol->get_numeric_label_value();
          string numeric_label_string = StringUtility::numberToString(loopEndLabel);
          curprint(numeric_label_string + " ");
        }

     curprint_keyword("WHILE", info);
     curprint(" (");
     info.set_inConditional(); // prevent printing line and file info

     SgExprStatement* conditionStatement = isSgExprStatement(while_stmt->get_condition());
     ASSERT_not_null(conditionStatement);
     unparseExpression(conditionStatement->get_expression(), info);
     info.unset_inConditional();
     curprint(")");
  
  // loop body (must always exist)
     unparseStatement(while_stmt->get_body(), info);

     unparseStatementNumbersSupport(while_stmt->get_end_numeric_label(),info);

  // This setting converts all non-block where statements into blocked where statements.
  // So "DO WHILE (A) B = 0" becomes:
  // "DO WHILE (A)
  //     B = 0
  //  END DO"

     if (while_stmt->get_has_end_statement()) {
       curprint_keyword("END", info);
       curprint(" ");
       curprint_keyword("DO", info);

       if (while_stmt->get_string_label().empty() == false) {
         // Output the string label
         curprint(" " + while_stmt->get_string_label());
       }
     }

     ASSERT_not_null(unp);
     unp->cur.insert_newline(1);
   }

void
FortranCodeGeneration_locatedNode::unparseSwitchStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // Sage node corresponds to Fortran 'select'
     SgSwitchStatement* switch_stmt = isSgSwitchStatement(stmt);
     ASSERT_not_null(switch_stmt);

     if (switch_stmt->get_string_label().empty() == false)
        {
       // Output the string label
          curprint(switch_stmt->get_string_label() + ": ");
        }

     curprint("SELECT CASE(");

     SgExprStatement* expressionStatement = isSgExprStatement(switch_stmt->get_item_selector());
     ASSERT_not_null(expressionStatement);
     unparseExpression(expressionStatement->get_expression(), info);
     curprint(")");
  
     if (switch_stmt->get_body())
        {
          unparseStatement(switch_stmt->get_body(), info);
        }

     unparseStatementNumbersSupport(switch_stmt->get_end_numeric_label(),info);

     curprint("END SELECT");

     if (switch_stmt->get_string_label().empty() == false)
        {
       // Output the string label
          curprint(" " + switch_stmt->get_string_label());
        }

     ASSERT_not_null(unp);
     unp->cur.insert_newline(1);
   }

void
FortranCodeGeneration_locatedNode::unparseCaseStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // Sage node corresponds to Fortran 'case'
     SgCaseOptionStmt* case_stmt = isSgCaseOptionStmt(stmt);
     ASSERT_not_null(case_stmt);

     curprint("CASE (");
     unparseExpression(case_stmt->get_key(), info);
     curprint(")");
  
     if (case_stmt->get_case_construct_name().empty() == false)
        {
       // Output the string case construct name
          curprint(" " + case_stmt->get_case_construct_name());
        }

     if (case_stmt->get_body())
        {
          unparseStatement(case_stmt->get_body(), info);
        }
   }

void 
FortranCodeGeneration_locatedNode::unparseDefaultStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // Sage node corresponds to Fortran 'case default'
     SgDefaultOptionStmt* default_stmt = isSgDefaultOptionStmt(stmt);
     ASSERT_not_null(default_stmt);
  
     curprint("CASE DEFAULT");

     if (default_stmt->get_default_construct_name().empty() == false)
        {
       // Output the string default construct name
          curprint(" " + default_stmt->get_default_construct_name());
        }

     if (default_stmt->get_body())
        {
          unparseStatement(default_stmt->get_body(), info);
        }
   }

void
FortranCodeGeneration_locatedNode::unparseBreakStmt(SgStatement* stmt, SgUnparse_Info&)
   {
  // This IR node corresponds to the Fortran 'exit'
     SgBreakStmt* break_stmt = isSgBreakStmt(stmt);
     ASSERT_not_null(break_stmt);
     curprint("EXIT");

  // If this is for a named do loop, this is the optional name.
     if (break_stmt->get_do_string_label().empty() == false)
        {
          curprint(" " + break_stmt->get_do_string_label());
        }
     unp->cur.insert_newline(1);
   }

void
FortranCodeGeneration_locatedNode::unparseContinueStmt(SgContinueStmt* continueStmt, SgUnparse_Info& info)
{
  // This IR node corresponds to a Fortran 'CYCLE' statement,
  // because semantically the same as a C/C++ continue statement.
  curprint_keyword("CYCLE", info);

  // If this is for a named do loop, this is the optional name.
  if (continueStmt->get_do_string_label().empty() == false) {
    curprint(" " + continueStmt->get_do_string_label());
  }

  unp->cur.insert_newline(1);
}

void
FortranCodeGeneration_locatedNode::unparseFortranContinueStmt(SgFortranContinueStmt* continueStmt, SgUnparse_Info& info)
{
  curprint_keyword("CONTINUE", info);
  unp->cur.insert_newline(1);
}

void 
FortranCodeGeneration_locatedNode::unparseLabelStmt(SgLabelStatement* labelStmt, SgUnparse_Info& info)
{
  if (flangParser) {
    // The SgLabelStatement is used for the label
    if (labelStmt->get_label().getString().size() > 0) {
      // Print label without formatting for now, think about fixed form later
      curprint(labelStmt->get_label().getString() + " ");
    }
    unparseLanguageSpecificStatement(labelStmt->get_statement(), info);
  }
  else {
    // This IR node corresponds to Fortran 'label CONTINUE' statement
    // TODO: rethink label handling in old parser
    curprint_keyword("CONTINUE", info);
    unp->cur.insert_newline(1);
  }
}

void
FortranCodeGeneration_locatedNode::unparseGotoStmt(SgGotoStatement* gotoStmt, SgUnparse_Info& info)
{
   ASSERT_not_null(gotoStmt);
   curprint_keyword("GOTO", info);
   curprint(" ");

   // The Flang parser uses an SgLabelStatement for a statement label, at this point it has
   // already been printed.  Printing the goto label is simple, just print it and return.
   if (gotoStmt->get_label()) {
       // Flang unparser
       curprint(gotoStmt->get_label()->get_label());
       unp->cur.insert_newline(1);
       return;
     }

   // Old OFP parser uses numeric label handling which is different than C/C++.
   ASSERT_not_null(gotoStmt->get_label_expression());
   SgLabelSymbol* labelSymbol = gotoStmt->get_label_expression()->get_symbol();

   ASSERT_not_null(labelSymbol);

   // Every numeric label should be associated with a statement
   ASSERT_not_null(labelSymbol->get_fortran_statement());
   int numeric_label = labelSymbol->get_numeric_label_value();

   ASSERT_require(numeric_label >= 0);
   string numeric_label_string = StringUtility::numberToString(numeric_label);
   curprint(numeric_label_string);

   unp->cur.insert_newline(1);
}

void
FortranCodeGeneration_locatedNode::unparseProcessControlStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgProcessControlStatement* ctrl_stmt = isSgProcessControlStatement(stmt);
     ASSERT_not_null(ctrl_stmt);

     SgExpression* quiet_expr = ctrl_stmt->get_quiet();
     SgProcessControlStatement::control_enum kind = ctrl_stmt->get_control_kind();

     switch (kind)
        {
          case SgProcessControlStatement::e_stop:
             {
                curprint_keyword("STOP", info);
                curprint(" ");
                unparseExpression(ctrl_stmt->get_code(), info);
                // F2018 syntax
                if (quiet_expr && !isSgNullExpression(quiet_expr))
                   {
                      curprint(", ");
                      curprint_keyword("QUIET", info);
                      curprint("=");
                      unparseExpression(quiet_expr, info);
                   }
                break;
             }
          case SgProcessControlStatement::e_error_stop:
             {
                curprint_keyword("ERROR", info);
                curprint(" ");
                curprint_keyword("STOP", info);
                curprint(" ");
                unparseExpression(ctrl_stmt->get_code(), info);
                // F2018 syntax
                if (quiet_expr && !isSgNullExpression(quiet_expr))
                   {
                      curprint(", ");
                      curprint_keyword("QUIET", info);
                      curprint("=");
                      unparseExpression(quiet_expr, info);
                   }
                break;
             }
          case SgProcessControlStatement::e_fail_image:
             {
                curprint_keyword("FAIL", info);
                curprint(" ");
                curprint_keyword("IMAGE", info);
                unparseExpression(ctrl_stmt->get_code(), info);
                break;
             }
          case SgProcessControlStatement::e_pause:
             {
                curprint("PAUSE ");
                unparseExpression(ctrl_stmt->get_code(), info);
                break;
             }
          default:
             {
               cerr << "error: unparseProcessControlStatement() is unimplemented for enum value "
                    << kind << "\n";
               ROSE_ABORT();
             }
        }

     unp->cur.insert_newline(1);
   }

void
FortranCodeGeneration_locatedNode::unparseReturnStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // This IR node is the same for C and Fortran
     SgReturnStmt* return_stmt = isSgReturnStmt(stmt);
     ASSERT_not_null(return_stmt);

     curprint("RETURN");

  // The expression can only be a scalar integer for an alternate return
     SgExpression* altret = return_stmt->get_expression();
     ASSERT_not_null(altret);

     if (isSgNullExpression(altret) == nullptr)
        {
       // ROSE_ASSERT(isSgValueExp(altret));
          curprint(" ");
          unparseExpression(altret, info);
        }

     unp->cur.insert_newline(1);
   }

//----------------------------------------------------------------------------
//  void FortranCodeGeneration_locatedNode::<executable statements, IO>
//----------------------------------------------------------------------------

void 
FortranCodeGeneration_locatedNode::unparsePrintStatement(SgStatement* stmt, SgUnparse_Info& info) 
   {
  // Sage node corresponds to Fortran input/output statement
     SgPrintStatement* printStatement = isSgPrintStatement(stmt);
     ASSERT_not_null(printStatement);

     curprint("PRINT ");

     SgExpression* fmt = printStatement->get_format();
     if (fmt != nullptr)
        {
          unparseExpression(fmt, info);
          curprint(", ");
        }
       else
        {
       // Default if we don't have a valid format
          curprint("*, ");
        }

     SgExprListExp* iolist = printStatement->get_io_stmt_list();

     unparseExprList(iolist, info);

     unp->cur.insert_newline(1);
   }

bool
FortranCodeGeneration_locatedNode::unparse_IO_Support(SgStatement* stmt, bool skipUnit, SgUnparse_Info& info)
   {
  // Sage node corresponds to Fortran IO control info
     SgIOStatement* io_stmt = isSgIOStatement(stmt);
     ASSERT_not_null(io_stmt);

     bool isLeadingEntry = false;
     if (skipUnit == false)
        {
       // DQ (12/12/2010): Also for at least the gnu gfortran version 4.2.4, we can't output the "UNIT=" 
       // string for the write statement. See test2010_144.f90 for an example of this.
          bool skipOutputOfUnitString = (isSgWriteStatement(stmt) != nullptr);
          if (skipOutputOfUnitString == false)
             {
               curprint("UNIT=");
             }

          if (io_stmt->get_unit() != nullptr)
             {
               unparseExpression(io_stmt->get_unit(), info);
             }
            else
             {
               curprint("*");
             }
        }
       else
        {
          isLeadingEntry = true;
        }

     unparse_IO_Control_Support("IOSTAT",io_stmt->get_iostat(),isLeadingEntry,info);
     isLeadingEntry = isLeadingEntry && (io_stmt->get_iostat() == nullptr);

     unparse_IO_Control_Support("ERR",io_stmt->get_err(),isLeadingEntry,info);
     isLeadingEntry = isLeadingEntry && (io_stmt->get_err() == nullptr);

     unparse_IO_Control_Support("IOMSG",io_stmt->get_iomsg(),isLeadingEntry,info);
     isLeadingEntry = isLeadingEntry && (io_stmt->get_iomsg() == nullptr);

     return isLeadingEntry;
   }

void 
FortranCodeGeneration_locatedNode::unparse_IO_Control_Support( string name, SgExpression* expr, bool isLeadingEntry, SgUnparse_Info& info)
   {
     if (expr != nullptr)
        {
          if (isLeadingEntry == false)
               curprint(", ");

          curprint(name);
          curprint("=");
          unparseExpression(expr, info);
        }
   }

void 
FortranCodeGeneration_locatedNode::unparseReadStatement(SgStatement* stmt, SgUnparse_Info& info) 
   {
  // Sage node corresponds to Fortran input/output statement
     SgReadStatement* readStatement = isSgReadStatement(stmt);
     ASSERT_not_null(readStatement);

     curprint("READ ");

     SgExprListExp* iolist = readStatement->get_io_stmt_list();

  // If only "READ 1,A" then this is using the format label "1" which is an alternative form of the read statement.
  // In this case the unit is not specified.
     if (readStatement->get_format() != nullptr && readStatement->get_unit() == nullptr)
        {
          unparseExpression(readStatement->get_format(), info);
          if (iolist->get_expressions().empty() == false)
             {
               curprint(",");
             }
        }
       else
        {
          curprint("(");
          unparse_IO_Support(readStatement,false,info);

       // Added missing items to the io-control-spec-list [Rasmussen, 2019.05.31]

          unparse_IO_Control_Support("FMT",         readStatement->get_format(),      false,info);
          unparse_IO_Control_Support("NML",         readStatement->get_namelist(),    false,info);
          unparse_IO_Control_Support("ADVANCE",     readStatement->get_advance(),     false,info);
          unparse_IO_Control_Support("ASYNCHRONOUS",readStatement->get_asynchronous(),false,info);
          unparse_IO_Control_Support("BLANK",       readStatement->get_blank(),       false,info);
          unparse_IO_Control_Support("DECIMAL",     readStatement->get_decimal(),     false,info);
          unparse_IO_Control_Support("DELIM",       readStatement->get_delim(),       false,info);
          unparse_IO_Control_Support("END",         readStatement->get_end(),         false,info);
          unparse_IO_Control_Support("EOR",         readStatement->get_eor(),         false,info);
          unparse_IO_Control_Support("ID",          readStatement->get_id(),          false,info);
          unparse_IO_Control_Support("PAD",         readStatement->get_pad(),         false,info);
          unparse_IO_Control_Support("POS",         readStatement->get_pos(),         false,info);
          unparse_IO_Control_Support("REC",         readStatement->get_rec(),         false,info);
          unparse_IO_Control_Support("ROUND",       readStatement->get_round(),       false,info);
          unparse_IO_Control_Support("SIGN",        readStatement->get_sign(),        false,info);
          unparse_IO_Control_Support("SIZE",        readStatement->get_size(),        false,info);

          curprint(") ");
        }

     unparseExprList(iolist, info);

     unp->cur.insert_newline(1);
   }

void 
FortranCodeGeneration_locatedNode::unparseWriteStatement(SgStatement* stmt, SgUnparse_Info& info) 
   {
  // Sage node corresponds to Fortran input/output statement
     SgWriteStatement* writeStatement = isSgWriteStatement(stmt);
     ASSERT_not_null(writeStatement);

     curprint("WRITE (");

     unparse_IO_Support(stmt,false,info);

     unparse_IO_Control_Support("FMT",         writeStatement->get_format(),      false,info);
     unparse_IO_Control_Support("NML",         writeStatement->get_namelist(),    false,info);
     unparse_IO_Control_Support("ADVANCE",     writeStatement->get_advance(),     false,info);
     unparse_IO_Control_Support("ASYNCHRONOUS",writeStatement->get_asynchronous(),false,info);
     unparse_IO_Control_Support("BLANK",       writeStatement->get_blank(),       false,info);
     unparse_IO_Control_Support("DECIMAL",     writeStatement->get_decimal(),     false,info);
     unparse_IO_Control_Support("DELIM",       writeStatement->get_delim(),       false,info);
     unparse_IO_Control_Support("END",         writeStatement->get_end(),         false,info);
     unparse_IO_Control_Support("EOR",         writeStatement->get_eor(),         false,info);
     unparse_IO_Control_Support("ID",          writeStatement->get_id(),          false,info);
     unparse_IO_Control_Support("PAD",         writeStatement->get_pad(),         false,info);
     unparse_IO_Control_Support("POS",         writeStatement->get_pos(),         false,info);
     unparse_IO_Control_Support("REC",         writeStatement->get_rec(),         false,info);
     unparse_IO_Control_Support("ROUND",       writeStatement->get_round(),       false,info);
     unparse_IO_Control_Support("SIGN",        writeStatement->get_sign(),        false,info);
     unparse_IO_Control_Support("SIZE",        writeStatement->get_size(),        false,info);

     curprint(") ");

     SgExprListExp* iolist = writeStatement->get_io_stmt_list();

     unparseExprList(iolist, info);

     unp->cur.insert_newline(1);
   }

void 
FortranCodeGeneration_locatedNode::unparseOpenStatement(SgStatement* stmt, SgUnparse_Info& info) 
   {
  // Sage node corresponds to Fortran input/output statement
     SgOpenStatement* openStatement = isSgOpenStatement(stmt);
     ASSERT_not_null(openStatement);

     curprint("OPEN (");

     unparse_IO_Support(stmt,false,info);

     unparse_IO_Control_Support("FILE",openStatement->get_file(),false,info);
     unparse_IO_Control_Support("STATUS",openStatement->get_status(),false,info);
     unparse_IO_Control_Support("ACCESS",openStatement->get_access(),false,info);
     unparse_IO_Control_Support("FORM",openStatement->get_form(),false,info);
     unparse_IO_Control_Support("RECL",openStatement->get_recl(),false,info);
     unparse_IO_Control_Support("BLANK",openStatement->get_blank(),false,info);

  // F90 specific 
     unparse_IO_Control_Support("POSITION",openStatement->get_position(),false,info);
     unparse_IO_Control_Support("ACTION",openStatement->get_action(),false,info);
     unparse_IO_Control_Support("DELIM",openStatement->get_delim(),false,info);
     unparse_IO_Control_Support("PAD",openStatement->get_pad(),false,info);

  // F2003 specific
     unparse_IO_Control_Support("ASYNCHRONOUS",openStatement->get_asynchronous(),false,info);

     curprint(") ");

     unp->cur.insert_newline(1);
   }

void 
FortranCodeGeneration_locatedNode::unparseCloseStatement(SgStatement* stmt, SgUnparse_Info& info) 
   {
  // Sage node corresponds to Fortran input/output statement
     SgCloseStatement* closeStatement = isSgCloseStatement(stmt);
     ASSERT_not_null(closeStatement);

     curprint("CLOSE (");

     unparse_IO_Support(stmt,false,info);

     unparse_IO_Control_Support("STATUS",closeStatement->get_status(),false,info);

     curprint(") ");

     unp->cur.insert_newline(1);
   }

void 
FortranCodeGeneration_locatedNode::unparseInquireStatement(SgStatement* stmt, SgUnparse_Info& info) 
   {
  // Sage node corresponds to Fortran input/output statement
     SgInquireStatement* inquireStatement = isSgInquireStatement(stmt);
     ASSERT_not_null(inquireStatement);

     curprint("INQUIRE (");

     bool isLeadingEntry = true;
     if (inquireStatement->get_iolengthExp() != nullptr)
        {
          curprint("IOLENGTH=");
          unparseExpression(inquireStatement->get_iolengthExp(),info);
          isLeadingEntry = false;
        }
       else
        {
       // This is the "INQUIRE(inquire-spec-list)" case.

          if (inquireStatement->get_unit() != nullptr)
             {
            // Fortran rules don't allow output if "unit=*"
               isLeadingEntry = unparse_IO_Support(stmt,false,info);
             }

          unparse_IO_Control_Support("FILE",inquireStatement->get_file(),isLeadingEntry,info);

          isLeadingEntry = isLeadingEntry && (inquireStatement->get_file() == nullptr);
          ROSE_ASSERT(isLeadingEntry == false);

          unparse_IO_Control_Support("ACCESS",inquireStatement->get_access(),false,info);
          unparse_IO_Control_Support("FORM",inquireStatement->get_form(),false,info);
          unparse_IO_Control_Support("RECL",inquireStatement->get_recl(),false,info);
          unparse_IO_Control_Support("BLANK",inquireStatement->get_blank(),false,info);
          unparse_IO_Control_Support("EXIST",inquireStatement->get_exist(),false,info);
          unparse_IO_Control_Support("OPENED",inquireStatement->get_opened(),false,info);
          unparse_IO_Control_Support("NUMBER",inquireStatement->get_number(),false,info);
          unparse_IO_Control_Support("NAMED",inquireStatement->get_named(),false,info);
          unparse_IO_Control_Support("NAME",inquireStatement->get_name(),false,info);
          unparse_IO_Control_Support("SEQUENTIAL",inquireStatement->get_sequential(),false,info);
          unparse_IO_Control_Support("DIRECT",inquireStatement->get_direct(),false,info);
          unparse_IO_Control_Support("FORMATTED",inquireStatement->get_formatted(),false,info);
          unparse_IO_Control_Support("UNFORMATTED",inquireStatement->get_unformatted(),false,info);
          unparse_IO_Control_Support("NEXTREC",inquireStatement->get_nextrec(),false,info);

       // F90 specific 
          unparse_IO_Control_Support("POSITION",inquireStatement->get_position(),false,info);
          unparse_IO_Control_Support("ACTION",inquireStatement->get_action(),false,info);
          unparse_IO_Control_Support("READ",inquireStatement->get_read(),false,info);
          unparse_IO_Control_Support("WRITE",inquireStatement->get_write(),false,info);
          unparse_IO_Control_Support("READWRITE",inquireStatement->get_readwrite(),false,info);
          unparse_IO_Control_Support("DELIM",inquireStatement->get_delim(),false,info);
          unparse_IO_Control_Support("PAD",inquireStatement->get_pad(),false,info);

       // F2003 specific
          unparse_IO_Control_Support("ASYNCHRONOUS",inquireStatement->get_asynchronous(),false,info);
          unparse_IO_Control_Support("DECIMAL",inquireStatement->get_decimal(),false,info);
          unparse_IO_Control_Support("STREAM",inquireStatement->get_stream(),false,info);
          unparse_IO_Control_Support("SIZE",inquireStatement->get_size(),false,info);
          unparse_IO_Control_Support("PENDING",inquireStatement->get_pending(),false,info);
        }

     curprint(") ");

     SgExprListExp* iolist = inquireStatement->get_io_stmt_list();
     if (iolist != nullptr)
        {
       // DQ (3/28/2017): Eliminate warning of overloaded virtual function in base class (from Clang).
       // unparseExprList(iolist, info, false /*paren*/);
          unparseExprList(iolist, info);
        }

     unp->cur.insert_newline(1);
   }

void 
FortranCodeGeneration_locatedNode::unparseFlushStatement(SgStatement* stmt, SgUnparse_Info& info) 
   {
  // Sage node corresponds to Fortran input/output statement
     SgFlushStatement* flushStatement = isSgFlushStatement(stmt);
     ASSERT_not_null(flushStatement);

     curprint("FLUSH (");

     unparse_IO_Support(stmt,false,info);

     curprint(") ");

     SgExprListExp* iolist = flushStatement->get_io_stmt_list();
     if (iolist != nullptr)
        {
          unparseExprList(iolist, info);
        }
     unp->cur.insert_newline(1);
   }

void 
FortranCodeGeneration_locatedNode::unparseRewindStatement(SgStatement* stmt, SgUnparse_Info& info) 
   {
  // Sage node corresponds to Fortran input/output statement
     SgRewindStatement* rewindStatement = isSgRewindStatement(stmt);
     ASSERT_not_null(rewindStatement);

     curprint("REWIND (");

     unparse_IO_Support(stmt,false,info);

     curprint(") ");

     SgExprListExp* iolist = rewindStatement->get_io_stmt_list();
     if (iolist != nullptr)
        {
       // DQ (3/28/2017): Eliminate warning of overloaded virtual function in base class (from Clang).
       // unparseExprList(iolist, info, false /*paren*/);
          unparseExprList(iolist, info);
        }

     unp->cur.insert_newline(1);
   }

void 
FortranCodeGeneration_locatedNode::unparseBackspaceStatement(SgStatement* stmt, SgUnparse_Info& info) 
   {
  // Sage node corresponds to Fortran input/output statement
     SgBackspaceStatement* backspaceStatement = isSgBackspaceStatement(stmt);
     ASSERT_not_null(backspaceStatement);

     curprint("BACKSPACE (");

     unparse_IO_Support(stmt,false,info);

     curprint(") ");

     SgExprListExp* iolist = backspaceStatement->get_io_stmt_list();
     if (iolist != nullptr)
        {
       // DQ (3/28/2017): Eliminate warning of overloaded virtual function in base class (from Clang).
       // unparseExprList(iolist, info, false /*paren*/);
          unparseExprList(iolist, info);
        }
     unp->cur.insert_newline(1);
   }

void
FortranCodeGeneration_locatedNode::unparseEndfileStatement(SgStatement* stmt, SgUnparse_Info& info) 
   {
  // Sage node corresponds to Fortran input/output statement
     SgEndfileStatement* endfileStatement = isSgEndfileStatement(stmt);
     ASSERT_not_null(endfileStatement);

     curprint("ENDFILE (");

     unparse_IO_Support(stmt,false,info);

     curprint(") ");

     SgExprListExp* iolist = endfileStatement->get_io_stmt_list();
     if (iolist != nullptr)
        {
       // DQ (3/28/2017): Eliminate warning of overloaded virtual function in base class (from Clang).
       // unparseExprList(iolist, info, false /*paren*/);
          unparseExprList(iolist, info);
        }

     unp->cur.insert_newline(1);
   }

void
FortranCodeGeneration_locatedNode::unparseWaitStatement(SgStatement* stmt, SgUnparse_Info& info) 
   {
  // Sage node corresponds to Fortran input/output statement
     SgWaitStatement* waitStatement = isSgWaitStatement(stmt);
     ASSERT_not_null(waitStatement);

     curprint("WAIT (");

     unparse_IO_Support(stmt,false,info);

     curprint(") ");

     SgExprListExp* iolist = waitStatement->get_io_stmt_list();
     if (iolist != nullptr)
        {
       // DQ (3/28/2017): Eliminate warning of overloaded virtual function in base class (from Clang).
       // unparseExprList(iolist, info, false /*paren*/);
          unparseExprList(iolist, info);
        }

     unp->cur.insert_newline(1);
   }

void
FortranCodeGeneration_locatedNode::unparse_Image_Ctrl_Stmt_Support(SgImageControlStatement* stmt, bool print_comma, SgUnparse_Info& info)
   {
      ROSE_ASSERT(stmt);

      if (stmt->get_stat())
        {
           if (print_comma) curprint(", "); else print_comma = true;
           curprint("STAT=");
           unparseExpression(stmt->get_stat(), info);
        }
     if (stmt->get_err_msg())
        {
          if (print_comma) curprint(", "); else print_comma = true;
          curprint("ERRMSG=");
          unparseExpression(stmt->get_err_msg(), info);
        }
   }

void
FortranCodeGeneration_locatedNode::unparseSyncAllStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgSyncAllStatement* sync_stmt = isSgSyncAllStatement(stmt);
     ROSE_ASSERT(sync_stmt);

     bool print_initial_comma = false;

     curprint("SYNC ALL (");

     unparse_Image_Ctrl_Stmt_Support(sync_stmt, print_initial_comma, info);

     curprint(")");
     unp->cur.insert_newline(1);
   }

void
FortranCodeGeneration_locatedNode::unparseSyncImagesStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgSyncImagesStatement* sync_stmt = isSgSyncImagesStatement(stmt);
     ROSE_ASSERT(sync_stmt);

     SgExpression* image_set = sync_stmt->get_image_set();
     ROSE_ASSERT(image_set);

     bool print_comma = true;

     curprint("SYNC IMAGES (");

  // unparse the image set
     if (isSgNullExpression(image_set))
        {
        // A null expression is used to indicate lack of an actual/"real" expression
           curprint("*");
        }
     else
        {
           unparseExpression(sync_stmt->get_image_set(), info);
        }

     if (sync_stmt->get_stat())
        {
          if (print_comma) curprint(", "); else print_comma = true;
          curprint("STAT=");
          unparseExpression(sync_stmt->get_stat(), info);
        }
     if (sync_stmt->get_err_msg())
        {
          if (print_comma) curprint(", "); else print_comma = true;
          curprint("ERRMSG=");
          unparseExpression(sync_stmt->get_err_msg(), info);
        }

     curprint(")");

     unp->cur.insert_newline(1);
   }

void
FortranCodeGeneration_locatedNode::unparseSyncMemoryStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgSyncMemoryStatement* sync_stmt = isSgSyncMemoryStatement(stmt);
     ROSE_ASSERT(sync_stmt);

     bool print_comma = false;

     curprint("SYNC MEMORY (");

     if (sync_stmt->get_stat())
        {
          if (print_comma) curprint(", "); else print_comma = true;
          curprint("STAT=");
          unparseExpression(sync_stmt->get_stat(), info);
        }
     if (sync_stmt->get_err_msg())
        {
          if (print_comma) curprint(", "); else print_comma = true;
          curprint("ERRMSG=");
          unparseExpression(sync_stmt->get_err_msg(), info);
        }

     curprint(")");

     unp->cur.insert_newline(1);
   }

void
FortranCodeGeneration_locatedNode::unparseSyncTeamStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgSyncTeamStatement* sync_stmt = isSgSyncTeamStatement(stmt);
     ROSE_ASSERT(sync_stmt);

     bool print_comma = true;

     curprint("SYNC TEAM (");

  // unparse the team value
     unparseExpression(sync_stmt->get_team_value(), info);

     if (sync_stmt->get_stat())
        {
          if (print_comma) curprint(", "); else print_comma = true;
          curprint("STAT=");
          unparseExpression(sync_stmt->get_stat(), info);
        }
     if (sync_stmt->get_err_msg())
        {
          if (print_comma) curprint(", "); else print_comma = true;
          curprint("ERRMSG=");
          unparseExpression(sync_stmt->get_err_msg(), info);
        }

     curprint(")");
     unp->cur.insert_newline(1);
   }

void
FortranCodeGeneration_locatedNode::unparseLockStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgLockStatement* lock_stmt = isSgLockStatement(stmt);
     ROSE_ASSERT(lock_stmt);

     bool print_initial_comma = true;

     curprint("LOCK (");

  // unparse the lock variable
     unparseExpression(lock_stmt->get_lock_variable(), info);

     if (lock_stmt->get_acquired_lock())
        {
          curprint(", ");
          curprint("ACQUIRED_LOCK=");
          unparseExpression(lock_stmt->get_acquired_lock(), info);
        }
     unparse_Image_Ctrl_Stmt_Support(lock_stmt, print_initial_comma, info);

     curprint(")");
     unp->cur.insert_newline(1);
   }

void
FortranCodeGeneration_locatedNode::unparseUnlockStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgUnlockStatement* unlock_stmt = isSgUnlockStatement(stmt);
     ROSE_ASSERT(unlock_stmt);

     bool print_initial_comma = true;

     curprint("UNLOCK (");

  // unparse the lock variable
     unparseExpression(unlock_stmt->get_lock_variable(), info);

     unparse_Image_Ctrl_Stmt_Support(unlock_stmt, print_initial_comma, info);

     curprint(")");
     unp->cur.insert_newline(1);
   }

void
FortranCodeGeneration_locatedNode::unparseAssociateStatement(SgStatement* stmt, SgUnparse_Info& info) 
   {
  // Sage node corresponds to Fortran input/output statement
     SgAssociateStatement* associateStatement = isSgAssociateStatement(stmt);
     ASSERT_not_null(associateStatement);

     curprint("ASSOCIATE (");

     // Pei-Hung (07/24/2019) unparse SgDeclarationStatementPtrList for multiple associates
     SgDeclarationStatementPtrList::iterator pp = associateStatement->get_associates().begin();
     while ( pp != associateStatement->get_associates().end() )
        {
          SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(*pp);
          ASSERT_not_null(variableDeclaration);

          SgInitializedName* variable = *(variableDeclaration->get_variables().begin());
          ASSERT_not_null(variable);

          curprint(variable->get_name());
          curprint(" => ");
          unparseExpression(variable->get_initializer(),info);
          pp++;
          if(pp != associateStatement->get_associates().end())
            curprint(", ");
          
        }
        curprint(") ");

     ASSERT_not_null(associateStatement->get_body());
     unparseStatement(associateStatement->get_body(), info);

     unparseStatementNumbersSupport(nullptr, info);

     curprint("END ASSOCIATE");

     unp->cur.insert_newline(1);
   }

//----------------------------------------------------------------------------
//  void FortranCodeGeneration_locatedNode::<executable statements, other>
//----------------------------------------------------------------------------

void 
FortranCodeGeneration_locatedNode::unparseExprStmt(SgStatement* stmt, SgUnparse_Info& info) 
   {
  // Sage node corresponds to Fortran expression
     SgExprStatement* expr_stmt = isSgExprStatement(stmt);
     ASSERT_not_null(expr_stmt);
     ROSE_ASSERT(expr_stmt->get_expression());

     SgUnparse_Info ninfo(info);

  // Never unparse class definition in expression stmt
     ninfo.set_SkipClassDefinition();

     ninfo.set_SkipEnumDefinition();
     unparseExpression(expr_stmt->get_expression(), ninfo);

     if (ninfo.inVarDecl())
        {
          curprint(",");
        }

     unp->u_sage->curprint_newline();
   }


//----------------------------------------------------------------------------
//  FortranCodeGeneration_locatedNode::<pragmas>
//----------------------------------------------------------------------------

void
FortranCodeGeneration_locatedNode::unparsePragmaDeclStmt (SgStatement* stmt, SgUnparse_Info&)
{
  // Sage node corresponds to Fortran convention !pragma
  SgPragmaDeclaration* pragmaDeclaration = isSgPragmaDeclaration(stmt);
  ASSERT_not_null(pragmaDeclaration);
  
  SgPragma* pragma = pragmaDeclaration->get_pragma();
  ASSERT_not_null(pragma);
  
  string txt = pragma->get_pragma();
  AstAttribute* att = stmt->getAttribute("OmpAttributeList");
  if (att)
    curprint("!$");
  else
    curprint("!pragma ");
  curprint(txt);
  curprint("\n");
}

//----------------------------------------------------------------------------
//  Program unit helpers
//----------------------------------------------------------------------------

void
FortranCodeGeneration_locatedNode::genPUAutomaticStmts(SgStatement* stmt, SgUnparse_Info& info)
   {
  // For formatting purposes, pretend we have a small basic block
     unp->cur.format(stmt, info, FORMAT_BEFORE_BASIC_BLOCK2);
     curprint("USE ROSE__TYPES");

     unp->cur.format(stmt, info, FORMAT_BEFORE_STMT);

     unp->cur.format(stmt, info, FORMAT_AFTER_BASIC_BLOCK1);
   }


void
FortranCodeGeneration_locatedNode::unparseFuncArgs(SgInitializedNamePtrList* args, 
                              SgUnparse_Info& info)
{
  unparseInitNamePtrList(args, info);
}

void
FortranCodeGeneration_locatedNode::unparseInitNamePtrList(SgInitializedNamePtrList* args, SgUnparse_Info&)
{
  SgInitializedNamePtrList::iterator it = args->begin();
  while (it != args->end()) {
    SgInitializedName* arg = *it;
    curprint(arg->get_name().str());
    
    // Move to the next argument
    it++;
    
    // Check if this is the last argument (output a "," separator if not)
    if (it != args->end()) {
      curprint(", ");
    }
  }
}

//----------------------------------------------------------------------------
//  Declarations helpers
//----------------------------------------------------------------------------
void FortranCodeGeneration_locatedNode::unparseArrayAttr(SgArrayType* type, SgUnparse_Info& info, bool oneVarOnly)
{
    if (!oneVarOnly)
    {
      ASSERT_not_null(type);
      curprint(type->get_isCoArray()? "[": "(");
      unparseExpression(type->get_dim_info(), info);
      curprint(type->get_isCoArray()? "]": ")");
    }
}

void FortranCodeGeneration_locatedNode::unparseStringAttr(SgTypeString* type, SgUnparse_Info& info, bool oneVarOnly)
{
    if (!oneVarOnly)
    {
      curprint("*");
      curprint("(");
      unparseExpression(type->get_lengthExpression(), info);
      curprint(")");
    }
}

void FortranCodeGeneration_locatedNode::unparseEntityTypeAttr(SgType* type, SgUnparse_Info& info, bool oneVarOnly)
{
    if (type->get_isCoArray())
    {
        SgType* baseType;
        SgArrayType* arrayType = nullptr;
        if (isSgPointerType(type))
            baseType = isSgPointerType(type)->get_base_type();
        else
        {
            arrayType = isSgArrayType(type);
            baseType = arrayType->get_base_type();
        }
        if (isSgPointerType(type))
            unparseEntityTypeAttr(baseType, info, oneVarOnly);
        else if (isSgTypeString(baseType))
        {
            unparseArrayAttr(arrayType, info, oneVarOnly);  // print codimension
            unparseStringAttr(isSgTypeString(baseType), info, oneVarOnly);
        }
        else if (isSgArrayType(baseType))
        {
            SgArrayType* arrayBaseType = isSgArrayType(baseType);
            unparseArrayAttr(arrayBaseType, info, oneVarOnly);
            unparseArrayAttr(arrayType, info, oneVarOnly);  // print codimension
            SgTypeString* stringType = isSgTypeString(arrayBaseType->get_base_type());
            if (stringType)
                unparseStringAttr(stringType, info, oneVarOnly);
        }
        else
            unparseArrayAttr(arrayType, info, oneVarOnly);  // print codimension
    }
    else if (isSgArrayType(type))
    {
        SgArrayType* arrayType = isSgArrayType(type);
        unparseArrayAttr(arrayType, info, oneVarOnly);
        SgTypeString* stringType = isSgTypeString(arrayType->get_base_type());
        if (stringType)
            unparseStringAttr(stringType, info, oneVarOnly);
    }
    else if (isSgPointerType(type))
        unparseEntityTypeAttr(isSgPointerType(type)->get_base_type(), info, oneVarOnly);
    else if (isSgTypeString(type))
        unparseStringAttr(isSgTypeString(type), info, oneVarOnly);
}


void
FortranCodeGeneration_locatedNode::unparseVarDecl(SgStatement* stmt, SgInitializedName* initializedName, SgUnparse_Info& info)
   {
  // DQ (9/22/2007): Note that this function does not use its SgStatement* stmt parameter!

  // General format:
  //   <type> <attributes> :: <variable>

     SgName name         = initializedName->get_name();
     SgType* type        = initializedName->get_type();
     SgInitializer* init = initializedName->get_initializer();  
     ROSE_ASSERT(type);

     // Find out how many variables are declared in the given stmt:
     SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(stmt);
     ASSERT_not_null(variableDeclaration);
     int numVar = variableDeclaration->get_variables().size();

     if (info.SkipBaseType() == false )
        {
          // DXN (08/19/2011): unparse the base type when there are more than one declared variables
          if (numVar > 1) {
             SgType* baseType = type->stripType(SgType::STRIP_ARRAY_TYPE);
             unp->u_fortran_type->unparseType(baseType, info, false);  // do not print type attributes such as dimension, length on the left of ::
          }
          else {
             unp->u_fortran_type->unparseType(type, info, true);  // print type attribute on the left of ::
          }

          if (variableDeclaration->get_declarationModifier().get_typeModifier().isAllocatable())
             {
               curprint(", ALLOCATABLE");
             }

          if (variableDeclaration->get_declarationModifier().get_typeModifier().isAsynchronous())
             {
               curprint(", ASYNCHRONOUS");
             }

          if (variableDeclaration->get_declarationModifier().get_typeModifier().isIntent_in())
             {
               curprint(", INTENT(IN)");
             }

          if (variableDeclaration->get_declarationModifier().get_typeModifier().isIntent_out())
             {
               curprint(", INTENT(OUT)");
             }

          if (variableDeclaration->get_declarationModifier().get_typeModifier().isIntent_inout())
             {
               curprint(", INTENT(INOUT)");
             }

          if (variableDeclaration->get_declarationModifier().get_typeModifier().get_constVolatileModifier().isVolatile())
             {
               curprint(", VOLATILE");
             }

          if (variableDeclaration->get_declarationModifier().get_storageModifier().isExtern())
             {
               if (type->variantT()==V_SgTypeVoid) //FMZ 6/17/2009
                  curprint("EXTERNAL");
               else 
               curprint(", EXTERNAL");
             }

       // Fortran contiguous array storage attribute
          if (variableDeclaration->get_declarationModifier().get_storageModifier().isContiguous())
             {
               curprint(", CONTIGUOUS");
             }

       // Fortran CUDA support
          if (variableDeclaration->get_declarationModifier().get_storageModifier().isCudaDeviceMemory())
             {
               curprint(", device");
             }
          if (variableDeclaration->get_declarationModifier().get_storageModifier().isCudaManaged())
             {
               curprint(", managed");
             }
          if (variableDeclaration->get_declarationModifier().get_storageModifier().isCudaConstant())
             {
               curprint(", constant");
             }
          if (variableDeclaration->get_declarationModifier().get_storageModifier().isCudaShared())
             {
               curprint(", shared");
             }
          if (variableDeclaration->get_declarationModifier().get_storageModifier().isCudaPinned())
             {
               curprint(", pinned");
             }
          if (variableDeclaration->get_declarationModifier().get_storageModifier().isCudaTexture())
             {
               curprint(", texture");
             }

          if (variableDeclaration->get_declarationModifier().get_typeModifier().get_constVolatileModifier().isConst())
             {
            // PARAMETER in Fortran implies const in C/C++
               curprint(", PARAMETER");
             }

          if (variableDeclaration->get_declarationModifier().get_accessModifier().isPublic())
             {
            // The PUBLIC keyword is only permitted within Modules
               if ( TransformationSupport::getModuleStatement(variableDeclaration) != nullptr )
                  {
                    curprint(", PUBLIC");
                  }
                 else
                  {
                    // Liao 12/14/2010
                    // SgAccessModifier::post_construction_initialization() will set the modifier to e_default, which in turn is equal to e_public
                    // variable declarations should have public access by default.
                    // So I turn off this warning after discussing this issue with Dan
                   // printf ("Warning: statement marked as public in non-module scope in FortranCodeGeneration_locatedNode::unparseVarDecl(). \n");
                  }
             }

          if (variableDeclaration->get_declarationModifier().get_accessModifier().isPrivate())
             {
            // The PRIVATE keyword is only permitted within Modules
               if ( TransformationSupport::getModuleStatement(variableDeclaration) != nullptr )
                  {
                    curprint(", PRIVATE");
                  }
                 else
                  {
                    printf ("Warning: statement marked as private in non-module scope \n");
                  }
             }

          bool is_protected = true;
          SgInitializedNamePtrList & variableList = variableDeclaration->get_variables();
          ROSE_ASSERT(variableList.empty() == false);
          SgInitializedNamePtrList::iterator i = variableList.begin();
          while(i != variableList.end())
             {
               if ( (*i)->get_protected_declaration() == false)
                    is_protected = false;
               i++;
             }
          if (is_protected && (variableList.empty() == false))
             {
               curprint(", PROTECTED");
             }

          if (variableDeclaration->get_declarationModifier().get_typeModifier().isIntrinsic())
             {
               curprint(", INTRINSIC");
             }

          if (variableDeclaration->get_declarationModifier().isBind())
             {
               curprint(", ");

            // This is factored so that it can be called for function declarations, and variable declarations
               unparseBindAttribute(variableDeclaration);
             }

          if (variableDeclaration->get_declarationModifier().get_typeModifier().isOptional())
             {
               curprint(", OPTIONAL");
             }

          if (variableDeclaration->get_declarationModifier().get_typeModifier().isSave())
             {
               curprint(", SAVE");
             }

          if (variableDeclaration->get_declarationModifier().get_typeModifier().isTarget())
             {
               curprint(", TARGET");
             }

          if (variableDeclaration->get_declarationModifier().get_typeModifier().isValue())
             {
               curprint(", VALUE");
             }

       //FMZ (4/14/2009): Cray Pointer
          if (isSgTypeCrayPointer(type) == nullptr)
             {
               curprint(" :: ");
             }
            else
             {
               curprint(" (");
             }
        }
      // FMZ 
      // FIXME: currenly use "prev_decl_item" to denote the pointee
     curprint(name.str());

     if (isSgTypeCrayPointer(type) != nullptr)
     {
           SgInitializedName *pointeeVar = initializedName->get_prev_decl_item();
           ASSERT_not_null(pointeeVar);
           SgName pointeeName = pointeeVar->get_name();
           curprint(",");
           curprint(pointeeName.str());
           curprint(") ");
      }
      else
          unparseEntityTypeAttr(type, info, numVar == 1);

       // Unparse the initializers if any exist
       // printf ("In FortranCodeGeneration_locatedNode::unparseVarDecl(initializedName=%p): variable initializer = %p \n",initializedName,init);
      if (init != nullptr)
      {
           if (isSgPointerType(type))
           {  // this is a pointer null-init; OFP 0.8.2 has yet to implement pointer => init-data-object
               // TODO: need an IR to model pointer initialization, something like SgPointerInitializer.
               curprint(" => NULL()");

           }
           else
           {
               curprint(" = ");
               SgInitializer* initializer = isSgInitializer(init);
               ASSERT_not_null(initializer);
               unparseExpression(initializer, info);
           }
      }
   }

//----------------------------------------------------------------------------
//  void Unparser::printDeclModifier
//  void Unparser::printAccessModifier   
//  void Unparser::printStorageModifier
//  
//  The following 2 functions: printAccessModifier and printStorageModifier,
//  are just the two halves from printDeclModifier. These two functions
//  are used in the unparse functions for SgMemberFunctionDeclarations
//  and SgVariableDeclaration.  printAccessModifier is first called before
//  the format function. If "private", "protected", or "public" is to
//  be printed out, it does so here. Then I format which will put me
//  in position to unparse the declaration. Then I call
//  printSpecifer2, which will print out any keywords if the option is
//  turned on.  Then the declaration is printed in the same line. If I
//  didnt do this, the printing of keywords would be done before
//  formatting, and would put the declaration on another line (and
//  would look terribly formatted).
//----------------------------------------------------------------------------

void
FortranCodeGeneration_locatedNode::printDeclModifier(SgDeclarationStatement*, SgUnparse_Info&)
   {
     printf ("Access modifiers are handled differently for Fortran, this function printDeclModifier() should not be called! \n");
     ROSE_ABORT();
   }

void
FortranCodeGeneration_locatedNode::printAccessModifier(SgDeclarationStatement*, SgUnparse_Info&)
{
  // FIXME: this will look different for full-featured Fortran
     printf ("Access modifiers are handled differently for Fortran, this function printAccessModifier() should not be called! \n");
}

void
FortranCodeGeneration_locatedNode::unparseBindAttribute ( SgDeclarationStatement* declaration )
   {
  // Code generation support for "bind" attribute
     if (declaration->get_declarationModifier().isBind())
        {
          curprint(" bind(");

          curprint(declaration->get_linkage());

          if (declaration->get_binding_label().empty() == false)
             {
               curprint(",NAME=\"");
               curprint(declaration->get_binding_label());
               curprint("\"");
             }
          curprint(")");
        }
   }


void
FortranCodeGeneration_locatedNode::printStorageModifier(SgDeclarationStatement*, SgUnparse_Info&)
   {
  // FIXME: this will look different for full-featured Fortran
     printf ("Access modifiers are handled differently for Fortran, this function printStorageModifier() should not be called! \n");
     ROSE_ABORT();
   }

void
FortranCodeGeneration_locatedNode::unparseProcHdrStmt(SgStatement* stmt, SgUnparse_Info& info)
{
  // Sage node corresponds to Fortran procedure program unit

  SgProcedureHeaderStatement* procedureHeader = isSgProcedureHeaderStatement(stmt);
  ASSERT_not_null(procedureHeader);

  string typeOfFunction;
  if (procedureHeader->isFunction()) {
    typeOfFunction = " FUNCTION";
  }
  else {
    if (procedureHeader->isSubroutine()) {
      typeOfFunction = "SUBROUTINE";
    }
    else {
      ASSERT_require(procedureHeader->isBlockData());
      typeOfFunction = "BLOCK DATA";
    }
  }

  if (!procedureHeader->isForward() && procedureHeader->get_definition() != nullptr && !info.SkipFunctionDefinition()) {
    // Output the function declaration with definition

    // The unparsing of the definition will cause the unparsing of the declaration (with SgUnparse_Info
    // flags set to just unparse a forward declaration!)
    SgUnparse_Info ninfo(info);

    // To avoid end of statement formatting (added CR's) we call the unparseFuncDefnStmt directly
    unparseFuncDefnStmt(procedureHeader->get_definition(), ninfo);

    unp->cur.insert_newline(1);

    // The "END" has just been output by the unparsing of the SgFunctionDefinition
    // so we just want to finish it off with "PROGRAM <name>".

    unparseStatementNumbersSupport(procedureHeader->get_end_numeric_label(),info);
    curprint("END " + typeOfFunction + " ");
    if (procedureHeader->get_named_in_end_statement()) {
      curprint(procedureHeader->get_name().str());
    }

    // Output 2 new lines to better separate functions visually in the output
    unp->cur.insert_newline(1);
    unp->cur.insert_newline(2); //FMZ
  }
  else {
    if (procedureHeader->get_functionModifier().isPure()) {
      curprint("PURE ");
    }
    if (procedureHeader->get_functionModifier().isElemental()) {
      curprint("ELEMENTAL ");
    }
    if (procedureHeader->get_functionModifier().isRecursive()) {
      curprint("RECURSIVE ");
    }
    if (procedureHeader->get_functionModifier().isCudaHost()) {
      curprint("attributes(host) ");
    }
    if (procedureHeader->get_functionModifier().isCudaGlobalFunction()) {
      curprint("attributes(global) ");
    }
    if (procedureHeader->get_functionModifier().isCudaDevice()) {
      curprint("attributes(device) ");
    }
    if (procedureHeader->get_functionModifier().isCudaGridGlobal()) {
      curprint("attributes(grid_global) ");
    }

    //FMZ (5/13/2010): If there is declaration of "result", we need to check if the
    //                 type of the function is already declared by the "result"
    bool need_type = true;
    string result_name_str;
          
    if (procedureHeader->get_result_name() != nullptr) {
      SgInitializedName* rslt_name = procedureHeader->get_result_name();
      SgDeclarationStatement* rslt_decl = rslt_name->get_definition();

      // check declaraion stmts
      if (rslt_decl != nullptr) {
        need_type = false;
        result_name_str = rslt_name->get_name().str();
      }
    }

    if (procedureHeader->isFunction() && need_type) {
      // Unparse the return type
      SgFunctionType* functionType = procedureHeader->get_type();
      ASSERT_not_null(functionType);
      SgType* returnType = functionType->get_return_type();
      ASSERT_not_null(returnType);

      unp->u_fortran_type->unparseType(returnType,info);
    }

    // Are there possible qualifiers that we are missing?
    curprint(typeOfFunction + " ");
    curprint(procedureHeader->get_name().str());

    SgUnparse_Info ninfo2(info);
    ninfo2.set_inArgList();

    // Fortran Block Data statements don't have operands (I think)
    if (procedureHeader->isBlockData() == false) {
      curprint("(");
      unparseFunctionArgs(procedureHeader,ninfo2);
      curprint(")");
    }

    unparseBindAttribute(procedureHeader);

    // Unparse the result(<name>) suffix if present
    if (procedureHeader->get_result_name() != nullptr &&
        procedureHeader->get_name()!= procedureHeader->get_result_name()->get_name())
      {
        curprint(" result(");
        curprint(procedureHeader->get_result_name()->get_name());
        curprint(")");
      }

    // Output 1 new line so that new statements will appear on their own line after the SgProgramHeaderStatement declaration.
    unp->cur.insert_newline(1);
  }
}

void
FortranCodeGeneration_locatedNode::unparseFuncDefnStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgFunctionDefinition* funcdefn_stmt = isSgFunctionDefinition(stmt);
     ASSERT_not_null(funcdefn_stmt);

  // Unparse any comments of directives attached to the SgFunctionParameterList
     ASSERT_not_null(funcdefn_stmt->get_declaration());
     if (funcdefn_stmt->get_declaration()->get_parameterList() != nullptr)
         unparseAttachedPreprocessingInfo(funcdefn_stmt->get_declaration()->get_parameterList(), info, PreprocessingInfo::before);

     info.set_SkipFunctionDefinition();
     SgStatement *declstmt = funcdefn_stmt->get_declaration();

  // DQ (10/15/2006): Mark that we are unparsing a function declaration (or member function declaration)
  // this will help us know when to trim the "::" prefix from the name qualiciation.  The "::" global scope
  // qualifier is not used in function declarations, but is used for function calls.
     info.set_declstatement_ptr(nullptr);
     info.set_declstatement_ptr(funcdefn_stmt->get_declaration());

     if (isSgProgramHeaderStatement(declstmt) != nullptr) {
        unparseProgHdrStmt(declstmt, info);
     }
     else {
        ASSERT_not_null(isSgProcedureHeaderStatement(declstmt));
        unparseProcHdrStmt(declstmt, info);
     }

  // Un-mark that we are unparsing a function declaration (or member function declaration)
     info.set_declstatement_ptr(nullptr);

     info.unset_SkipFunctionDefinition();
     SgUnparse_Info ninfo(info);
  
  // now the body of the function
     if (funcdefn_stmt->get_body())
        {
          unparseStatement(funcdefn_stmt->get_body(), ninfo);
        }
       else
        {
          curprint ( string("{}"));
          printf ("Error: Should be an error to not have a function body in the AST \n");
          ROSE_ABORT();
        }

  // Unparse any comments of directives attached to the SgFunctionParameterList
     unparseAttachedPreprocessingInfo(funcdefn_stmt->get_declaration()->get_parameterList(), info, PreprocessingInfo::after);
   }

void
FortranCodeGeneration_locatedNode::unparseFunctionParameterDeclaration(
   SgFunctionDeclaration* funcdecl_stmt, 
   SgInitializedName* initializedName,
   bool /*outputParameterDeclaration*/,
   SgUnparse_Info&)
   {
     ASSERT_not_null(funcdecl_stmt);
     ASSERT_not_null(initializedName);

     curprint(initializedName->get_name().str());
   }

void
FortranCodeGeneration_locatedNode::unparseFunctionArgs(SgFunctionDeclaration* funcdecl_stmt, SgUnparse_Info& info)
   {
     ASSERT_not_null(funcdecl_stmt);

     SgInitializedNamePtrList::iterator p = funcdecl_stmt->get_args().begin();
     while ( p != funcdecl_stmt->get_args().end() )
        {
          unparseFunctionParameterDeclaration (funcdecl_stmt,*p,false,info);

       // Move to the next argument
          p++;

       // Check if this is the last argument (output a "," separator if not)
          if (p != funcdecl_stmt->get_args().end())
             {
               curprint( ",");
             }
        }
   }

//-----------------------------------------------------------------------------------
//  void Unparse_ExprStmt::unparse_helper
//
//  prints out the function parameters in a function declaration or function
//  call. For now, all parameters are printed on one line since there is no
//  file information for each parameter.
//-----------------------------------------------------------------------------------
void
FortranCodeGeneration_locatedNode::unparse_helper(SgFunctionDeclaration* funcdecl_stmt, SgUnparse_Info& info)
   {
     ASSERT_not_null(funcdecl_stmt);

  // output the function name
     curprint( funcdecl_stmt->get_name().str());

     SgUnparse_Info ninfo2(info);
     ninfo2.set_inArgList();

  // DQ (5/14/2003): Never output the class definition in the argument list.
  // Using this C++ constraint avoids building a more complex mechanism to turn it off.
     ninfo2.set_SkipClassDefinition();

  // DQ (9/9/2016): These should have been setup to be the same.
     ninfo2.set_SkipEnumDefinition();

     curprint("(");
     unparseFunctionArgs(funcdecl_stmt,ninfo2);
     curprint(")");
   }

void
FortranCodeGeneration_locatedNode::unparseClassDeclStmt_derivedType(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgDerivedTypeStatement* classdecl_stmt = isSgDerivedTypeStatement(stmt);
     ASSERT_not_null(classdecl_stmt);

     if (!classdecl_stmt->isForward() && classdecl_stmt->get_definition() && !info.SkipClassDefinition())
        {
          SgUnparse_Info ninfox(info);

          ninfox.unset_SkipSemiColon();

       // DQ (6/13/2007): Set to null before resetting to non-null value 
          ninfox.set_declstatement_ptr(nullptr);
          ninfox.set_declstatement_ptr(classdecl_stmt);

          unparseStatement(classdecl_stmt->get_definition(), ninfox);
          unparseStatementNumbersSupport(classdecl_stmt->get_end_numeric_label(),info);

          curprint("END TYPE ");
          curprint(classdecl_stmt->get_name().str());

          ASSERT_not_null(unp);
          unp->cur.insert_newline(1);
        }
       else
        {
          if (!info.inEmbeddedDecl())
             {
               SgUnparse_Info ninfo(info);
               ASSERT_not_null(classdecl_stmt->get_parent());
               SgClassDefinition *cdefn = isSgClassDefinition(classdecl_stmt->get_parent());

               if(cdefn && cdefn->get_declaration()->get_class_type() == SgClassDeclaration::e_class)
                    ninfo.set_CheckAccess();

               unp->u_sage->printSpecifier(classdecl_stmt, ninfo);
               info.set_access_attribute(ninfo.get_access_attribute());
             }

          info.unset_inEmbeddedDecl();

          curprint ("TYPE ");

          if (classdecl_stmt->get_declarationModifier().get_accessModifier().isPublic())
             {
            // The PUBLIC keyword is only permitted within Modules
               if ( TransformationSupport::getModuleStatement(classdecl_stmt) != nullptr )
                  {
                    curprint(", PUBLIC");
                  }
                 else
                  {
                    printf ("Warning: statement marked as public in non-module scope in FortranCodeGeneration_locatedNode::unparseClassDeclStmt_derivedType(). \n");
                  }
             }

          if (classdecl_stmt->get_declarationModifier().get_accessModifier().isPrivate())
             {
            // The PRIVATE keyword is only permitted within Modules
               if ( TransformationSupport::getModuleStatement(classdecl_stmt) != nullptr )
                  {
                    curprint(", PRIVATE");
                  }
                 else
                  {
                    printf ("Warning: statement marked as private in non-module scope \n");
                  }
             }

          if (classdecl_stmt->get_declarationModifier().get_typeModifier().isBind())
             {
            // The BIND keyword is only permitted within Modules
               if ( TransformationSupport::getModuleStatement(classdecl_stmt) != nullptr )
                  {
                 // I think that bind implies "BIND(C)"
                    curprint(", BIND(C)");
                  }
                 else
                  {
                    printf ("Warning: statement marked as bind in non-module scope \n");
                  }
             }

          if (classdecl_stmt->get_declarationModifier().get_typeModifier().isExtends())
             {
            // The EXTENDS keyword is only permitted within Modules
               if ( TransformationSupport::getModuleStatement(classdecl_stmt) != nullptr )
                  {
                    curprint(", EXTENDS(PARENT-TYPE-NAME-NOT-IMPLEMENTED)");
                  }
                 else
                  {
                    printf ("Warning: statement marked as extends in non-module scope \n");
                  }
             }

          if (classdecl_stmt->get_declarationModifier().get_typeModifier().isAbstract())
             {
            // The ABSTRACT keyword is only permitted within Modules
               if ( TransformationSupport::getModuleStatement(classdecl_stmt) != nullptr )
                  {
                    curprint(", ABSTRACT");
                  }
                 else
                  {
                    printf ("Warning: statement marked as abstract in non-module scope \n");
                  }
             }

       // DQ (8/28/2010): I think this is require to separate type attribute specifiers from the name of the type.
          curprint (" :: ");

          curprint(classdecl_stmt->get_name().str());
        }
   }

void
FortranCodeGeneration_locatedNode::unparseClassDeclStmt_module(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgModuleStatement* classdecl_stmt = isSgModuleStatement(stmt);
     ASSERT_not_null(classdecl_stmt);

     if (!classdecl_stmt->isForward() && classdecl_stmt->get_definition() && !info.SkipClassDefinition())
        {
          SgUnparse_Info ninfox(info);

          ninfox.unset_SkipSemiColon();

       // DQ (6/13/2007): Set to null before resetting to non-null value 
          ninfox.set_declstatement_ptr(nullptr);
          ninfox.set_declstatement_ptr(classdecl_stmt);

          unparseStatement(classdecl_stmt->get_definition(), ninfox);

          unparseStatementNumbersSupport(classdecl_stmt->get_end_numeric_label(),info);
          curprint("END MODULE ");
          curprint(classdecl_stmt->get_name().str());

          ASSERT_not_null(unp);
          unp->cur.insert_newline(1);
          unp->cur.insert_newline(2);  //FMZ
        }
       else
        {
          if (!info.inEmbeddedDecl())
             {
               SgUnparse_Info ninfo(info);
               ASSERT_not_null(classdecl_stmt->get_parent());
               SgClassDefinition *cdefn = isSgClassDefinition(classdecl_stmt->get_parent());

               if(cdefn && cdefn->get_declaration()->get_class_type() == SgClassDeclaration::e_class)
                    ninfo.set_CheckAccess();

               unp->u_sage->printSpecifier(classdecl_stmt, ninfo);
               info.set_access_attribute(ninfo.get_access_attribute());
             }

          info.unset_inEmbeddedDecl();

          curprint ( "MODULE ");
          curprint(classdecl_stmt->get_name().str());

          SgName nm = classdecl_stmt->get_name();
        }
   }

void
FortranCodeGeneration_locatedNode::unparseClassDefnStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgClassDefinition* classdefn_stmt = isSgClassDefinition(stmt);
     ASSERT_not_null(classdefn_stmt);

     SgUnparse_Info ninfo(info);

     ninfo.set_SkipClassDefinition();
     ninfo.set_SkipEnumDefinition();
     ninfo.set_SkipSemiColon();

     ASSERT_not_null(classdefn_stmt->get_declaration());

     if (isSgModuleStatement(classdefn_stmt->get_declaration()) != nullptr) {
        unparseClassDeclStmt_module(classdefn_stmt->get_declaration(), ninfo);
     }
     else {
        unparseClassDeclStmt_derivedType(classdefn_stmt->get_declaration(), ninfo);
     }

     ninfo.unset_SkipSemiColon();
     ninfo.unset_SkipClassDefinition();
     ninfo.unset_SkipEnumDefinition();

     SgNamedType *saved_context = ninfo.get_current_context();

     ASSERT_not_null(classdefn_stmt->get_declaration()->get_firstNondefiningDeclaration());
     SgClassDeclaration* classDeclaration = isSgClassDeclaration(classdefn_stmt->get_declaration()->get_firstNondefiningDeclaration());
     ASSERT_not_null(classDeclaration->get_type());

  // DQ (6/13/2007): Set to null before resetting to non-null value 
     ninfo.set_current_context(nullptr);
     ninfo.set_current_context(classDeclaration->get_type());

  // For Fortran we don't have an inheritance concept, I think.
     ROSE_ASSERT(classdefn_stmt->get_inheritances().empty());

  // DQ (9/28/2004): Turn this back on as the only way to prevent this from being unparsed!
  // DQ (11/22/2003): Control unparsing of the {} part of the definition
     if ( info.SkipBasicBlock() == false )
        {
       // DQ (6/14/2006): Add packing pragma support (explicitly set the packing 
       // alignment to the default, part of packing pragma normalization).
          unsigned int packingAlignment = classdefn_stmt->get_packingAlignment();
          if (packingAlignment != 0)
             {
               curprint ( string("\n#pragma pack(") + StringUtility::numberToString(packingAlignment) + string(")"));
             }

          ninfo.set_isUnsetAccess();
          unp->cur.format(classdefn_stmt, info, FORMAT_BEFORE_BASIC_BLOCK1);
          unp->cur.format(classdefn_stmt, info, FORMAT_AFTER_BASIC_BLOCK1);

          if (classdefn_stmt->get_isSequence())
             {
               unparseStatementNumbersSupport(nullptr,info);
               curprint ("sequence");
               unp->u_sage->curprint_newline();
             }

          if (classdefn_stmt->get_isPrivate())
             {
               unparseStatementNumbersSupport(nullptr,info);
               curprint ("private");
               unp->u_sage->curprint_newline();
             }

          SgDeclarationStatementPtrList::iterator pp = classdefn_stmt->get_members().begin();

          while ( pp != classdefn_stmt->get_members().end() )
             {
               unparseStatement((*pp), ninfo);
            // curprint("! Comment in unparseClassDefnStmt() (after each member declaration) \n");
               pp++;
             }

       // DQ (3/17/2005): This helps handle cases such as class foo { #include "constant_code.h" }
          ASSERT_not_null(classdefn_stmt->get_startOfConstruct());
          ASSERT_not_null(classdefn_stmt->get_endOfConstruct());

          unparseAttachedPreprocessingInfo(classdefn_stmt, info, PreprocessingInfo::inside);
        }

  // DQ (6/13/2007): Set to null before resetting to non-null value 
     ninfo.set_current_context(nullptr);
     ninfo.set_current_context(saved_context);
   }

void
FortranCodeGeneration_locatedNode::unparseAllocateStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgAllocateStatement* s = isSgAllocateStatement(stmt);
     SgExprListExp* exprList = s->get_expr_list();
     ASSERT_not_null(exprList);

     curprint("allocate( ");

  // DQ (3/28/2017): Eliminate warning of overloaded virtual function in base class (from Clang).
  // unparseExprList(exprList, info, false /*paren*/);
     unparseExprList(exprList, info);

     if (s->get_stat_expression() != nullptr)
        {
          curprint(", STAT = ");
          unparseExpression(s->get_stat_expression(), info);
        }

     if (s->get_errmsg_expression() != nullptr)
        {
          curprint(", ERRMSG = ");
          unparseExpression(s->get_errmsg_expression(), info);
        }

     if (s->get_source_expression() != nullptr)
        {
          curprint(", SOURCE = ");
          unparseExpression(s->get_source_expression(), info);
        }

     curprint(" )");
     unp->cur.insert_newline(1);
   }
 
void
FortranCodeGeneration_locatedNode::unparseDeallocateStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgDeallocateStatement* s = isSgDeallocateStatement(stmt);
     SgExprListExp* exprList = s->get_expr_list();
     ASSERT_not_null(exprList);

     curprint("deallocate( ");

  // DQ (3/28/2017): Eliminate warning of overloaded virtual function in base class (from Clang).
  // unparseExprList(exprList, info, false /*paren*/);
     unparseExprList(exprList, info);

     if (s->get_stat_expression() != nullptr)
        {
          curprint(", STAT = ");
          unparseExpression(s->get_stat_expression(), info);
        }

     if (s->get_errmsg_expression() != nullptr)
        {
          curprint(", ERRMSG = ");
          unparseExpression(s->get_errmsg_expression(), info);
        }

     curprint(" )");
     unp->cur.insert_newline(1);
   }



void
FortranCodeGeneration_locatedNode::unparseWithTeamStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgCAFWithTeamStatement* withTeamStmt = isSgCAFWithTeamStatement(stmt);
     ASSERT_not_null(withTeamStmt);

     curprint("WITHTEAM ");
    
     SgVarRefExp* teamIdRef = withTeamStmt->get_teamId(); 

     SgInitializedName* teamDecl = teamIdRef->get_symbol()->get_declaration();

     curprint(teamDecl->get_name().str());

     unp->cur.insert_newline(1);

     // unparse the body
     SgBasicBlock * body = isSgBasicBlock(withTeamStmt->get_body());
     ASSERT_not_null(body);

     unparseBasicBlockStmt(body, info);

     curprint("END WITHTEAM ");

     curprint(teamDecl->get_name().str());
     unp->cur.insert_newline(1);
 
   }

// TODO: This code is identical to 'UnparseLanguageIndependentConstructs::curprint'. Factor this!
void
FortranCodeGeneration_locatedNode::curprint(const std::string & str) const
{
#if USE_RICE_FORTRAN_WRAPPING

    if( unp->currentFile != nullptr && unp->currentFile->get_Fortran_only() )
    {
        // determine line wrapping parameters -- 'pos' variables are one-based
        bool is_fixed_format = unp->currentFile->get_outputFormat() == SgFile::e_fixed_form_output_format;
        bool is_free_format  = unp->currentFile->get_outputFormat() == SgFile::e_free_form_output_format;
        int usable_cols = ( is_fixed_format ? MAX_F90_LINE_LEN_FIXED
                          : is_free_format  ? MAX_F90_LINE_LEN_FREE - 1 // reserve a column in free-format for possible trailing '&'
                          : unp->cur.get_linewrap() );

        // check whether line wrapping is needed
        int used_cols = unp->cur.current_col();     // 'current_col' is zero-based
        int free_cols = usable_cols - used_cols;
        if( str.size() > free_cols )
        {
            if( is_fixed_format )
            {
                // only noncomment lines need wrapping
                if( ! (used_cols == 0 && str[0] != ' ' ) )
                {
                    // warn if successful wrapping is impossible
                    if( 6 + str.size() > usable_cols )
                        printf("Warning: can't wrap long line in Fortran fixed format (continuation + text is longer than a line)\n");

                    // emit fixed-format line continuation
                    unp->cur.insert_newline(1);
                    unp->u_sage->curprint("     &");
                }
            }
            else if( is_free_format )
            {
                // warn if successful wrapping is impossible
                if( str.size() > usable_cols )
                    printf("Warning: can't wrap long line in Fortran free format (text is longer than a line)\n");

                // emit free-format line continuation even if result will still be too long
                unp->u_sage->curprint("&");
                unp->cur.insert_newline(1);
                unp->u_sage->curprint("&");
            }
            else
                printf("Warning: long line not wrapped (unknown output format)\n");
        }
    }

    unp->u_sage->curprint(str);
     
#else  // ! USE_RICE_FORTRAN_WRAPPING

     // FMZ (3/22/2010) added fortran continue line support
     bool is_fortran90 =  (unp->currentFile != nullptr ) &&
                              (unp->currentFile->get_F90_only() ||
                                  unp->currentFile->get_CoArrayFortran_only());

     int str_len       = str.size();
     int curr_line_len = unp->cur.current_col();

     if (is_fortran90 && 
              curr_line_len!=0 && 
               (str_len + curr_line_len)> MAX_F90_LINE_LEN) {
          unp->u_sage->curprint("&");
          unp->cur.insert_newline(1);
     }

     if (str_len <= MAX_F90_LINE_LEN || str[0] == '#' || str[0] == '!')
     {
       unp->u_sage->curprint(str);
     }
     else
     {
       for(int stridx=0; stridx <str_len; stridx += MAX_F90_LINE_LEN)
       {
         std::string substring = str.substr(stridx, std::min(MAX_F90_LINE_LEN,str_len-stridx));
         unp->u_sage->curprint(substring);
         if (stridx + MAX_F90_LINE_LEN < str_len)
         {
           unp->u_sage->curprint("&");
           unp->cur.insert_newline(1);
         }
       }
     }
     
#endif  // USE_RICE_FORTRAN_WRAPPING
}

void FortranCodeGeneration_locatedNode::unparseOmpPrefix     (SgUnparse_Info& info)
{
  curprint(string ("!$omp "));
}

// Just skip nowait and copyprivate clauses for Fortran 
void
FortranCodeGeneration_locatedNode::unparseOmpBeginDirectiveClauses (SgStatement* stmt,     SgUnparse_Info& info)
{
  ASSERT_not_null(stmt);
  // optional clauses
  if (isSgOmpClauseBodyStatement(stmt))
  {
    const SgOmpClausePtrList& clause_ptr_list = isSgOmpClauseBodyStatement(stmt)->get_clauses();
    SgOmpClausePtrList::const_iterator i;
    for (i= clause_ptr_list.begin(); i!= clause_ptr_list.end(); i++)
    {
      SgOmpClause* c_clause = *i;
      if (isSgOmpNowaitClause(c_clause) || isSgOmpCopyprivateClause(c_clause) )
         continue;
      unparseOmpClause(c_clause, info);
    }
  }
  unp->u_sage->curprint_newline();
}

// Only unparse nowait or copyprivate clauses here
void
FortranCodeGeneration_locatedNode::unparseOmpEndDirectiveClauses(SgStatement* stmt,     SgUnparse_Info& info)
{
  ASSERT_not_null(stmt);
  // optional clauses
  if (isSgOmpClauseBodyStatement(stmt))
  {
    const SgOmpClausePtrList& clause_ptr_list = isSgOmpClauseBodyStatement(stmt)->get_clauses();
    SgOmpClausePtrList::const_iterator i;
    for (i= clause_ptr_list.begin(); i!= clause_ptr_list.end(); i++)
    {
      SgOmpClause* c_clause = *i;
      if (isSgOmpNowaitClause(c_clause) || isSgOmpCopyprivateClause(c_clause) )
        unparseOmpClause(c_clause, info);
    }
  }
  unp->u_sage->curprint_newline();
}

void FortranCodeGeneration_locatedNode::unparseOmpEndDirectivePrefixAndName (SgStatement* stmt,     SgUnparse_Info& info)
{
  ASSERT_not_null(stmt);
  unp->u_sage->curprint_newline();
  switch (stmt->variantT())
  {
    case V_SgOmpParallelStatement:
      {
        unparseOmpPrefix(info);
        curprint(string ("end parallel "));
        break;
      }
     case V_SgOmpCriticalStatement:
      {
        unparseOmpPrefix(info);
        curprint(string ("end critical "));
        if (isSgOmpCriticalStatement(stmt)->get_name().getString()!="")
        {

          curprint (string ("("));
          curprint (isSgOmpCriticalStatement(stmt)->get_name().getString());
          curprint (string (")"));
        }
        break;
      }
        case V_SgOmpSectionsStatement:
      {
        unparseOmpPrefix(info);
        curprint(string ("end sections"));
        break;
      }
       case V_SgOmpMasterStatement:
      {
        unparseOmpPrefix(info);
        curprint(string ("end master "));
        break;
      }
      case V_SgOmpOrderedStatement:
      {
        unparseOmpPrefix(info);
        curprint(string ("end ordered "));
        break;
      }
    case V_SgOmpWorkshareStatement:
      {
        unparseOmpPrefix(info);
        curprint(string ("end workshare "));
        break;
      }
      case V_SgOmpSingleStatement:
      {
        unparseOmpPrefix(info);
        curprint(string ("end single "));
        break;
      }
     case V_SgOmpTaskStatement:
      {
        unparseOmpPrefix(info);
        curprint(string ("end task "));
        break;
      }
     case V_SgOmpDoStatement:
      {
        unparseOmpPrefix(info);
        curprint(string ("end do "));
        break;
      }
    default:
      {
        cerr<<"error: unacceptable OpenMP directive type within unparseOmpDirectivePrefixAndName(): "<<stmt->class_name()<<endl;
        ROSE_ABORT();
      }
  } // end switch
}

void FortranCodeGeneration_locatedNode::unparseOmpDoStatement     (SgStatement* stmt, SgUnparse_Info& info)
{
  ASSERT_not_null(stmt);
  SgOmpDoStatement * d_stmt = isSgOmpDoStatement (stmt);
  ASSERT_not_null(d_stmt);
  
  unparseOmpDirectivePrefixAndName(stmt, info);
  unparseOmpBeginDirectiveClauses(stmt, info);

  SgUnparse_Info ninfo(info);
  if (d_stmt->get_body())
  {
    unparseStatement(d_stmt->get_body(), ninfo);
  }
  else
  {
    cerr<<"Error: empty body for:"<<stmt->class_name()<<" is not allowed!"<<endl;
    ROSE_ABORT();
  }

    // unparse the end directive and name 
  unparseOmpEndDirectivePrefixAndName (stmt, info);

  // unparse the end directive's clause
  unparseOmpEndDirectiveClauses(stmt, info);
}
