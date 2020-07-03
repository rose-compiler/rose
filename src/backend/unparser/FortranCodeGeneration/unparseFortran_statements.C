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

// void FortranCodeGeneration_locatedNode::unparseStatementNumbersSupport ( int numeric_label )
// void FortranCodeGeneration_locatedNode::unparseStatementNumbersSupport ( SgLabelSymbol* numeric_label_symbol )
// void FortranCodeGeneration_locatedNode::unparseStatementNumbersSupport ( SgLabelSymbol* numeric_label_symbol, SgUnparse_Info& info )
void
FortranCodeGeneration_locatedNode::unparseStatementNumbersSupport ( SgLabelRefExp* numeric_label_exp, SgUnparse_Info& info )
   {
  // This is a supporting function for the unparseStatementNumbers, but can be called directly for statments
  // in the IR that can have botha starting yntax and an ending syntax, both of which can be labeled.  
  // See test2007_01.f90 for an example of the SgProgramHeaderStatement used this way.

  // In fixed format all labels must appear within columns 1-5 (where column 1 is the first column)
  // and the 6th column is for the line continuation character (any character, I think).
     const int NumericLabelIndentation = 6;

     if (info.SkipFormatting() == true)
        {
          return;
        }

  // Let the default be fixed format for now (just for fun)
     bool fixedFormat = (unp->currentFile==NULL) ||
                        (unp->currentFile->get_outputFormat() == SgFile::e_unknown_output_format) ||
                        (unp->currentFile->get_outputFormat() == SgFile::e_fixed_form_output_format);

  // if (numeric_label_symbol != NULL)
     if (numeric_label_exp != NULL)
        {
       // ASSERT_not_null(numeric_label_exp);
          SgLabelSymbol* numeric_label_symbol = numeric_label_exp->get_symbol();

          int numeric_label = numeric_label_symbol->get_numeric_label_value();

       // printf ("In unparseStatementNumbers: numeric_label = %d \n",numeric_label);
          ROSE_ASSERT(numeric_label >= -1);

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

               if (fixedFormat == true)
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

            // printf ("In unparseStatementNumbers: numeric_label_string = %s \n",numeric_label_string.c_str());

               curprint( numeric_label_string );
             }
            else
             {
               if (fixedFormat == true)
                  {
                 // if fixed format then output 6 blanks
                    curprint("      ");
                  }
             }
        }
       else
        {
          if (fixedFormat == true)
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

  // printf ("In unparseStatementNumbers(): stmt = %p = %s \n",stmt,stmt->class_name().c_str());

  // This is a Fortran specific case (different from use of SgLabelStatement in C/C++).
  // unparseStatementNumbersSupport(stmt->get_numeric_label(),info);

  // DQ (11/29/2008): If this is a CPP directive then don't output statement 
  // number or the white space for then in fixed format mode.
     if (isSgC_PreprocessorDirectiveStatement(stmt) != NULL)
        {
          printf ("This is a CPP directive, skip leading white space in unparsing. \n");
          return;
        }

  // This fixes a formatting problem, an aspect fo which was reported by Liao 12/28/2007).
     if ( isSgGlobal(stmt) != NULL || isSgBasicBlock(stmt) != NULL )
        {
       // Skip any formatting since these don't result in statements that are output!
        }
       else
        {
          SgProgramHeaderStatement* program_header = isSgProgramHeaderStatement(stmt);
          if (program_header != NULL)
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

  // The default value is -1 and any non-negative value is allowed as a label
  // ROSE_ASSERT(stmt->get_numeric_label() >= -1);
   }

void
FortranCodeGeneration_locatedNode::unparseLanguageSpecificStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
  // This function unparses the language specific parse not handled by the base class unparseStatement() member function

     ASSERT_not_null(stmt);

#if 0
     printf ("In FortranCodeGeneration_locatedNode::unparseLanguageSpecificStatement ( stmt = %p = %s ) language = %s \n",stmt,stmt->class_name().c_str(),languageName().c_str());
#endif

  // DQ (11/17/2007): Add numeric lables where they apply, this is called in UnparseLanguageIndependentConstructs::unparseStatement().
  // unparseStatementNumbers(stmt);

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

       // DQ (8/25/2007): Added to support Fortran derived types
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
          case V_SgStopOrPauseStatement:       unparseStopOrPauseStmt(stmt, info);       break;

       // executable statements, IO
       // case V_SgIOStatement:                unparseIOStmt(stmt, info);                break;

       // DQ (11/25/2007): These are derived from SgIOStatement
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

       // Rasmussen (9/21/2018): These are derived from SgImageControlStatement
          case V_SgSyncAllStatement:           unparseSyncAllStatement(stmt, info);      break;
          case V_SgSyncImagesStatement:        unparseSyncImagesStatement(stmt, info);   break;
          case V_SgSyncMemoryStatement:        unparseSyncMemoryStatement(stmt, info);   break;
          case V_SgSyncTeamStatement:          unparseSyncTeamStatement(stmt, info);     break;
          case V_SgLockStatement:              unparseLockStatement(stmt, info);         break;
          case V_SgUnlockStatement:            unparseUnlockStatement(stmt, info);       break;

       // DQ (11/30/2007): Added support for associate statement (F2003)
          case V_SgAssociateStatement:         unparseAssociateStatement(stmt, info);    break;

       // DQ (11/25/2007): This has now been eliminated
       // case V_SgIOControlStatement:         unparse_IO_ControlStatement(stmt, info);  break;

       // case V_SgIOFileControlStmt:          unparseIOFileControlStatement(stmt, info);break;

       // DQ (8/22/2007): We have made unparsing of a SgFunctionDeclaration C/C++ specific, and 
       // defined derived classes for SgProgramHeaderStatement and SgProcedureHeaderStatement objects.
       // case V_SgFunctionDeclaration:        unparseFuncDeclStmt(stmt, info);          break;
          case V_SgFunctionDefinition:         unparseFuncDefnStmt(stmt, info);          break;
          case V_SgExprStatement:              unparseExprStmt(stmt, info);              break;

       // DQ (8/22/2007): New statements
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
          case V_SgLabelStatement:             unparseLabelStmt(stmt, info); break;

       // DQ (11/16/2007): This is a "DO WHILE" statement
          case V_SgWhileStmt:                  unparseWhileStmt(stmt, info); break;

       // DQ (11/17/2007): This is unparsed as a Fortran EXIT statement
          case V_SgBreakStmt:                  unparseBreakStmt(stmt, info); break;

       // DQ (11/17/2007): This is unparsed as a Fortran CYCLE statement
          case V_SgContinueStmt:               unparseContinueStmt(stmt, info); break;

       // DQ (11/17/2007): Added support for Fortran attribute statements.
          case V_SgAttributeSpecificationStatement: unparseAttributeSpecificationStatement(stmt, info); break;

       // DQ (11/19/2007): Added support for Fortran namelist statement.
          case V_SgNamelistStatement:          unparseNamelistStatement(stmt, info); break;

       // DQ (11/21/2007): Added support for Fortran return statement
          case V_SgReturnStmt:                 unparseReturnStmt(stmt, info); break;

       // DQ (11/21/2007): Added support for Fortran return statement
          case V_SgImportStatement:            unparseImportStatement(stmt, info); break;

       // DQ (12/18/2007): Added support for format statement
          case V_SgFormatStatement:            unparseFormatStatement(stmt, info);      break;

          case V_SgGotoStatement:              unparseGotoStmt(stmt, info); break;

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
       // case V_SgLabelStatement:             unparseLabelStmt(stmt, info); break;
       // case V_SgGotoStatement:              unparseGotoStmt(stmt, info); break;
       // executable statements, other
       // case V_SgExprStatement:              unparseExprStmt(stmt, info); break;
       //  Liao 10/18/2010, I turn on the pragma unparsing here to help debugging OpenMP programs
       //  , where OpenMP directive comments are used to generate C/C++-like pragmas internally.
       //  Those pragmas later are used to reuse large portion of OpenMP AST construction of C/C++
       // pragmas
        case V_SgPragmaDeclaration:          unparsePragmaDeclStmt(stmt, info); break;
        // Liao 10/21/2010, Fortran-only OpenMP handling
        case V_SgOmpDoStatement:             unparseOmpDoStatement(stmt, info); break;

#if 0
       // Optional support for unparsing Fortran from C
          case V_SgFunctionDeclaration:        unparseProcHdrStmt(stmt, info);          break;
#endif

          default:
             {
               printf("FortranCodeGeneration_locatedNode::unparseLanguageSpecificStatement: Error: No unparse function for %s (variant: %d)\n",stmt->sage_class_name(), stmt->variantT());
               ROSE_ASSERT(false);
               break;
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
       // ROSE_ASSERT(false);
        }
#endif

  // printf ("Unparsing Fortran include using includeFileName = %s \n",includeFileName.c_str());

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
     if (entryStatement->get_result_name() != NULL)
        {
          curprint(" result(");
          curprint(entryStatement->get_result_name()->get_name());
          curprint(")");
        }

     unp->cur.insert_newline(1);
   }

void
FortranCodeGeneration_locatedNode::unparseContainsStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     curprint("CONTAINS");
     unp->cur.insert_newline(1);
   }

// DQ (11/19/2007): support for type attributes when used as statements.
void
FortranCodeGeneration_locatedNode::unparseNamelistStatement (SgStatement* stmt, SgUnparse_Info& info)
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

// DQ (12/18/2007): support for format statement
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

       // ASSERT_not_null(formatItem->get_data());
          if (formatItem->get_data() != NULL)
             {
               SgStringVal* stringValue = isSgStringVal(formatItem->get_data());
               ASSERT_not_null(stringValue);

            // The string is stored without quotes, and we put them back on as required in code generation
               string str;
               if (stringValue->get_usesSingleQuotes() == true)
                  {
                    str = string("\'") + stringValue->get_value() + string("\'");
                  }
                 else
                  {
                    if (stringValue->get_usesDoubleQuotes() == true)
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
               if (formatItem->get_format_item_list() != NULL)
                  {
                    curprint("(");
                    unparseFormatItemList(formatItem->get_format_item_list(),info);
                    curprint(")");
                  }
                 else
                  {
                 // This is the case of "format (10/)" which processes "10" and "/" seperately (I think this is a bug, see test2007_241.f).

                    printf ("Error: both get_data() and get_format_item_list() are NULL \n");
                 // ROSE_ASSERT(false);

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
#if 0
     SgFormatItemPtrList & formatList = formatStatement->get_format_item_list();
     SgFormatItemPtrList::iterator i = formatList.begin();
     while (i != formatList.end())
        {
          SgFormatItem* formatItem = *i;

          ASSERT_not_null(formatItem->get_data());
          SgStringVal* stringValue = isSgStringVal(formatItem->get_data());
          ASSERT_not_null(stringValue);

       // The string is stored without quotes, and we put them back on as required in code generation
          string str;
          if (stringValue->get_usesSingleQuotes() == true)
             {
               str = string("\'") + stringValue->get_value() + string("\'");
             }
            else
             {
            // Noremally if usesSingleQuotes == false we use double quotes, but that would be
            // a mistake since this is not a string literal used in the format statement.
               str = stringValue->get_value();
             }
          curprint(str);

          i++;

          if (i != formatList.end())
             {
               curprint (",");
             }
        }
#else
#if 1
     SgFormatItemList* formatItemList = formatStatement->get_format_item_list();
     unparseFormatItemList(formatItemList,info);
#else
     SgFormatItemPtrList & formatList = formatStatement->get_format_item_list()->get_format_item_list();
     SgFormatItemPtrList::iterator i = formatList.begin();
     while (i != formatList.end())
        {
          SgFormatItem* formatItem = *i;

       // The default value is "-1" so zero should be an invalid value
          ROSE_ASSERT(formatItem->get_repeat_specifier() != 0);

       // Valid values are > 0
          if (formatItem->get_repeat_specifier() > 0)
             {
               string stringValue = StringUtility::numberToString(formatItem->get_repeat_specifier());
               curprint(stringValue);
               curprint(" ");
             }

       // ASSERT_not_null(formatItem->get_data());
          if (formatItem->get_data() != NULL)
             {
               SgStringVal* stringValue = isSgStringVal(formatItem->get_data());
               ASSERT_not_null(stringValue);

            // The string is stored without quotes, and we put them back on as required in code generation
               string str;
               if (stringValue->get_usesSingleQuotes() == true)
                  {
                    str = string("\'") + stringValue->get_value() + string("\'");
                  }
                 else
                  {
                    if (stringValue->get_usesDoubleQuotes() == true)
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

               i++;

               if (i != formatList.end())
                  {
                    curprint (",");
                  }
             }
            else
             {
               if (formatItem->get_format_item_list() != NULL)
                  {
                    format_item_list
                  }
                 else
                  {
                    printf ("Error: both get_data() and get_format_item_list() are NULL \n");
                    ROSE_ASSERT(false);
                  }
             }
        }
#endif
#endif
     curprint(" )");

     unp->cur.insert_newline(1);
   }

// DQ (11/19/2007): support for type attributes when used as statements.
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

  // printf ("variableName = %p = %s \n",variableName,variableName->get_name().str());
  // variableName->get_file_info()->display("variableName: unparseDimensionStatementForArrayVariable");

     SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(variableName->get_parent());

  // If there is a SgVariableDeclaration then it is simpler to look for it in the scope, 
  // else we have to look at each variable declaration for the SgInitializedName (which 
  // is only more expensive).
     bool foundArrayVariableDeclaration = false;
     if (variableDeclaration != NULL)
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
                    ROSE_ASSERT(false);
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

       // printf ("There was no variable declaration found though the symbol, so we have to look for the SgInitializedName in each SgVariableDeclaration \n");

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
               if (variableDeclaration != NULL)
                  {
                    SgInitializedNamePtrList & variableList = variableDeclaration->get_variables();
                    SgInitializedNamePtrList::iterator i = find(variableList.begin(),variableList.end(),variableName);

                    foundArrayVariableDeclaration = (i != variableList.end());
                  }

               i++;
             }
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

  // printf ("foundArrayVariableDeclaration = %s \n",foundArrayVariableDeclaration ? "true" : "false");

  // variableDeclaration->get_file_info()->display("variableDeclaration: unparseDimensionStatementForArrayVariable");
  // return variableDeclaration->get_file_info()->isCompilerGenerated();

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

#if 0
     SgDimensionObjectPtrList & dimensionObjectList = attributeSpecificationStatement->get_dimension_object_list();

  // printf ("dimensionObjectList.size() = %" PRIuPTR " \n",dimensionObjectList.size());

     SgDimensionObjectPtrList::iterator i_object = dimensionObjectList.begin();
     while (i_object != dimensionObjectList.end())
        {
       // Output the array name
       // printf ("case e_dimensionStatement: Array name = %s \n",(*i_object)->get_array()->get_name().str());

       // The SgDimensionObject should store a variable reference instead of a stringafied name!
          printf ("The SgDimensionObject should store a variable reference instead of a stringified name! \n");

          SgName name = (*i_object)->get_array()->get_name();
          SgScopeStatement* currentScope = attributeSpecificationStatement->get_scope();
          ASSERT_not_null(currentScope);
          SgVariableSymbol* variableSymbol = currentScope->lookup_variable_symbol(name);

          if (variableSymbol == NULL)
             {
            // This is a function parameter, so get the function scope and look for the symbol there
            // attributeSpecificationStatement->get_file_info()->display("Error: variableSymbol == NULL");

               SgScopeStatement* functionScope = TransformationSupport::getFunctionDefinition(currentScope);
               ASSERT_not_null(functionScope);
               variableSymbol = functionScope->lookup_variable_symbol(name);

            // If this was a function parameter then unparse the dimension statement
               unparseDimensionStatementResult = true;
             }
          ASSERT_not_null(variableSymbol);

          SgInitializedName* initializedName = variableSymbol->get_declaration();
          ASSERT_not_null(initializedName);
          SgNode* parentNode = initializedName->get_parent();
       // printf ("unparsing dimension statement: parentNode = %s \n",parentNode->class_name().c_str());
          SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(parentNode);

          if (variableDeclaration != NULL)
             {
               variableDeclaration->get_startOfConstruct()->display("Is this compiler generated");

            // Iterate over all the variables.
               for (unsigned long i=0; i < variableDeclaration->get_variables().size(); i++)
                  {
                 // if (variableDeclaration->get_startOfConstruct()->isSourcePositionUnavailableInFrontend() == true)
                    if (variableDeclaration->get_variables()[i]->get_startOfConstruct()->isSourcePositionUnavailableInFrontend() == true)
                       {
                      // This was not a part of the original source code so the dimension statement must be put out!
                         unparseDimensionStatementResult = true;
                       }
                  }
             }

          i_object++;
        }
#else
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
       // printf ("unparseForArrayVariable = %s \n",unparseForArrayVariable ? "true" : "false");

          if (unparseForArrayVariable == true)
               unparseDimensionStatementResult = true;
          i++;
        }

  // unparseDimensionStatementResult = true;
#endif

  // printf ("unparseDimensionStatementResult = %s \n",unparseDimensionStatementResult ? "true" : "false");
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

       // printf ("This is a dimension statement \n");
          if (unparseDimensionStatement(stmt) == false)
             {
            // Output the new line so that we leave a hole where the dimension statement was and don't
            // screwup the formatting of the labels (in columns 1-6)
            // curprint("! Skipping output of dimension statement (handled in declaration)");
               unp->cur.insert_newline(1);
               return;
             }
            else
             {
            // printf ("Unparsing the dimension statement \n");
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
               ROSE_ASSERT(false);
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
                    ROSE_ASSERT(false);
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
          ROSE_ASSERT(attributeSpecificationStatement->get_declarationModifier().isBind() == true);
          
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

  // if ( (attributeSpecificationStatement->get_attribute_kind() != SgAttributeSpecificationStatement::e_parameterStatement) &&
  //      (attributeSpecificationStatement->get_attribute_kind() != SgAttributeSpecificationStatement::e_dataStatement) )
     if ( (attributeSpecificationStatement->get_attribute_kind() != SgAttributeSpecificationStatement::e_parameterStatement) &&
          (attributeSpecificationStatement->get_attribute_kind() != SgAttributeSpecificationStatement::e_dataStatement) && 
          ( (attributeSpecificationStatement->get_attribute_kind() != SgAttributeSpecificationStatement::e_accessStatement_private && 
             attributeSpecificationStatement->get_attribute_kind() != SgAttributeSpecificationStatement::e_accessStatement_public) && 
             attributeSpecificationStatement->get_parameter_list() != NULL) )
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

       // unparseExpression(attributeSpecificationStatement->get_parameter_list(),info);
          SgExpressionPtrList & functionNameList = attributeSpecificationStatement->get_parameter_list()->get_expressions();
          SgExpressionPtrList::iterator i = functionNameList.begin();
          while (i != functionNameList.end())
             {
            // printf ("case e_externalStatement: *i = %p = %s \n",*i,(*i)->class_name().c_str());
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
       // unparseExpression(attributeSpecificationStatement->get_data_statement_group_list(),info);

          curprint(" ");

          SgDataStatementGroupPtrList & dataStatementGroupList = attributeSpecificationStatement->get_data_statement_group_list();
          SgDataStatementGroupPtrList::iterator i_group = dataStatementGroupList.begin();
          while (i_group != dataStatementGroupList.end())
             {
               SgDataStatementObjectPtrList & dataStatementObjectList = (*i_group)->get_object_list();
               SgDataStatementObjectPtrList::iterator i_object = dataStatementObjectList.begin();

            // curprint("(");
               while (i_object != dataStatementObjectList.end())
                  {
                    unparseExpression((*i_object)->get_variableReference_list(),info);
                    i_object++;
                    if (i_object != dataStatementObjectList.end())
                       {
                         curprint(", ");
                       }
                  }
            // curprint(")");

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
                              ROSE_ASSERT(false);
                              break;
                            }

                         case SgDataStatementValue::e_explict_list:
                            {
                              unparseExpression((*i_value)->get_initializer_list(),info);
                              break;
                            }

                         case SgDataStatementValue::e_implicit_list:
                            {
                              ROSE_ASSERT((*i_value)->get_initializer_list()->get_expressions().empty() == true);

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
                              ROSE_ASSERT(false);
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

       // unparseExpression(attributeSpecificationStatement->get_parameter_list(),info);

       // bool unparseForArrayVariable = unparseDimensionStatementForArrayVariable(arrayReference);
       // printf ("unparseForArrayVariable = %s \n",unparseForArrayVariable ? "true" : "false");

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
            // printf ("unparseForArrayVariable = %s \n",unparseForArrayVariable ? "true" : "false");

               if (unparseForArrayVariable == true)
                  {
                    if (unparseComma == true)
                       {
                         curprint(", ");
                       }

                    unparseComma = true;
                    unparseExpression(arrayReference,info);
                  }

               i++;
             }

        }

#if 0
     if ( attributeSpecificationStatement->get_attribute_kind() == SgAttributeSpecificationStatement::e_dimensionStatement )
        {
       // unparseExpression(attributeSpecificationStatement->get_data_statement_group_list(),info);

          curprint(" ");

          SgDimensionObjectPtrList & dimensionObjectList = attributeSpecificationStatement->get_dimension_object_list();

       // printf ("dimensionObjectList.size() = %" PRIuPTR " \n",dimensionObjectList.size());

          SgDimensionObjectPtrList::iterator i_object = dimensionObjectList.begin();
          while (i_object != dimensionObjectList.end())
             {
            // Output the array name
            // printf ("case e_dimensionStatement: Array name = %s \n",(*i_object)->get_array()->get_name().str());
#if 0
               SgName name = (*i_object)->get_array()->get_name();
               SgScopeStatement* currentScope = attributeSpecificationStatement->get_scope();
               ASSERT_not_null(currentScope);
               SgVariableSymbol* variableSymbol = currentScope->lookup_variable_symbol(name);
               ASSERT_not_null(variableSymbol);
               SgInitializedName* initializedName = variableSymbol->get_declaration();
               ASSERT_not_null(initializedName);
               SgNode* parentNode = initializedName->get_parent();
               printf ("unparsing dimension statement: parentNode = %s \n",parentNode->class_name().c_str());
               if (isSgVariableDeclaration(parentNode) != NULL)
                  {
                    printf ("this variable was part of a declaration, so skip the dimension entry for this variable \n");
                    i_object++;
                  }
                 else
#endif
                  {
                    curprint((*i_object)->get_array()->get_name());

                 // Output the associated shape
                    curprint("(");
                    unparseExpression((*i_object)->get_shape(),info);
                    curprint(")");

                    i_object++;
                    if (i_object != dimensionObjectList.end())
                       {
                         curprint(", ");
                       }
                  }
             }
        }
#endif

     const SgStringList & localList = attributeSpecificationStatement->get_name_list();

  // We need to recognize the commonblockobject in the list
     Rose_STL_Container<SgNode*> commonBlockList = NodeQuery::querySubTree (attributeSpecificationStatement->get_scope(),V_SgCommonBlockObject);
  // printf ("In unparseAttributeSpecificationStatement(): localList size = %" PRIuPTR " \n",localList.size());

     SgStringList::const_iterator i = localList.begin();
     string outputName = "";
     while (i != localList.end())
        {
       // printf ("Output name = %s \n",(*i).c_str());
          outputName = *i;
          for (Rose_STL_Container<SgNode*>::iterator j = commonBlockList.begin(); j != commonBlockList.end(); j++)
            {
               SgCommonBlockObject* commonBlockObject = isSgCommonBlockObject(*j);
               ROSE_ASSERT(commonBlockObject);
               string blockName = commonBlockObject->get_block_name();
//               std::cout << "commonblock:" << commonBlockObject << blockName << std::endl;
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

  // unp->cur.format(stmt, info, FORMAT_BEFORE_STMT);
  // curprint("IMPLICIT ");
     if (implicitStatement->get_implicit_none() == true)
        {
          curprint("IMPLICIT NONE");
        }
       else
        {
       // This is a range such as "DOUBLE PRECISION (D-E)" or a singleton such as "COMPLEX (C)"

          SgInitializedNamePtrList & nameList =  implicitStatement->get_variables();
          if (nameList.empty() == true)
             {
            // For now I just want to skip where alternative implicit rules are specified.
               if ( SgProject::get_verbose() >= 1 )
                    printf ("***** WARNING: in unparser NON \"IMPLICT NONE\" STATEMENT NOT YET FINISHED (skipped, default implicit type rules apply) *****\n");
             }
            else
             {
               ROSE_ASSERT(nameList.empty() == false);

               curprint("IMPLICIT ");

               SgInitializedNamePtrList::iterator i = nameList.begin();
#if 0
               SgInitializedName* firstName = *i;
               SgInitializedName* secondName = NULL;
               i++;

            // Check for existence of second name
               if (i != nameList.end())
                  {
                    secondName = *i;
                  }

               ASSERT_not_null(firstName->get_type());

            // Output the type
               unp->u_fortran_type->unparseType(firstName->get_type(),info);
               curprint(" (");
               curprint(firstName->get_name().str());
               if (secondName != NULL)
                  {
                 // DQ (12/2/2010): These need not match.
                 // Make sure that the types match for consistancy (error checking)
                 // ROSE_ASSERT(firstName->get_type() == secondName->get_type());
                    curprint(" - ");
                    curprint(secondName->get_name().str());
                  }
               curprint(")");
#else
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
#endif
             }
        }

     unp->cur.insert_newline(1);
  // unp->cur.format(stmt, info, FORMAT_AFTER_BASIC_BLOCK1);
   }

void
FortranCodeGeneration_locatedNode::unparseBlockDataStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     printf ("Sorry, unparseBlockDataStmt() not implemented \n");
   }

void
FortranCodeGeneration_locatedNode::unparseStatementFunctionStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     printf ("Sorry, unparseStatementFunctionStmt() not implemented \n");
   }

void
FortranCodeGeneration_locatedNode::unparseWhereStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // Currently the simple "where (a) b = 0" is unparsed as "where (a) b = 0 endwhere"

  // printf ("In FortranCodeGeneration_locatedNode::unparseWhereStmt() \n");

     SgWhereStatement* whereStatement = isSgWhereStatement(stmt);
     ASSERT_not_null(whereStatement);

     if (whereStatement->get_string_label().empty() == false)
        {
       // Output the string label
          curprint(whereStatement->get_string_label() + ": ");
        }

  // printf ("Unparse the where statement predicate \n");
     curprint("WHERE (");
  // unp->u_exprStmt->unparseExpression(whereStatement->get_condition(), info);
     unparseExpression(whereStatement->get_condition(), info);
     curprint(") ");

     bool output_endwhere = whereStatement->get_has_end_statement();

     if (output_endwhere == true)
        {
       // printf ("Unparse the where statement body \n");
          ASSERT_not_null(whereStatement->get_body());
          unparseStatement(whereStatement->get_body(),info);
       // printf ("DONE: Unparse the where statement body \n");
        }
       else
        {
          SgStatementPtrList & statementList = whereStatement->get_body()->get_statements();
          ROSE_ASSERT(statementList.size() == 1);
          SgStatement* statement = *(statementList.begin());
          ASSERT_not_null(statement);
       // printf ("Output true (where) statement = %p = %s \n",statement,statement->class_name().c_str());
          SgUnparse_Info info_without_formating(info);
          info_without_formating.set_SkipFormatting();
          unparseStatement(statement, info_without_formating);
        }

     SgElseWhereStatement* elsewhereStatement = whereStatement->get_elsewhere();
     if (elsewhereStatement != NULL)
        {
          if (output_endwhere == true)
             {
            // printf ("Unparse the elsewhere statement \n");
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
     if (output_endwhere == true)
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

  // DQ (10/2/2007): Only unparse the "()" if there is a valid elsewhere mask.
     if (isSgNullExpression(elseWhereStatement->get_condition()) == NULL)
        {
          curprint("(");
          unp->u_exprStmt->unparseExpression(elseWhereStatement->get_condition(), info);
          curprint(")");
        }

     ASSERT_not_null(elseWhereStatement->get_body());
     unparseStatement(elseWhereStatement->get_body(),info);

     SgElseWhereStatement* nested_elseWhereStatement = elseWhereStatement->get_elsewhere();
     if (nested_elseWhereStatement != NULL)
        {
          unparseStatement(nested_elseWhereStatement,info);
        }

  // There is no end statement label for an elsewhere statement, I think.
  // unparseStatementNumbersSupport(elseWhereStatement->get_end_numeric_label());
  // unp->cur.insert_newline(1);
   }

void
FortranCodeGeneration_locatedNode::unparseNullifyStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
#if 0
     printf ("Sorry, unparseNullifyStmt() not implemented \n");
#else  //(FMZ 10/12/2009) Added unparsing nullify statement
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

#endif
   }

void
FortranCodeGeneration_locatedNode::unparseEquivalenceStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // This organization is as a SgExprListExp of SgExprListExp of SgExpression objects.
  // This we can represent: "equivalence (i,j), (k,l,m,n)"

     SgEquivalenceStatement* equivalenceStatement = isSgEquivalenceStatement(stmt);

     curprint("equivalence ");

     ASSERT_not_null(equivalenceStatement->get_equivalence_set_list());
  // unparseExpression(equivalenceStatement->get_equivalence_set_list(),info);

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


// void FortranCodeGeneration_locatedNode::unparseLabel ( SgLabelSymbol* symbol )
void
FortranCodeGeneration_locatedNode::unparseLabel ( SgLabelRefExp* exp )
   {
     ASSERT_not_null(exp);
     SgLabelSymbol* symbol = exp->get_symbol();
     ASSERT_not_null(symbol);

  // DQ (12/24/2007): Every numeric lable should have been associated with a statement!
     ASSERT_not_null(symbol->get_fortran_statement());
     int numericLabel = symbol->get_numeric_label_value();

  // printf ("Output numeric label = %d \n",numericLabel);
     curprint( StringUtility::numberToString(numericLabel) );
   }

void
FortranCodeGeneration_locatedNode::unparseArithmeticIfStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgArithmeticIfStatement* arithmeticIf = isSgArithmeticIfStatement(stmt);
     ASSERT_not_null(arithmeticIf);
     ROSE_ASSERT(arithmeticIf->get_conditional());
#if 0
     if (arithmeticIf->get_string_label().empty() == false)
        {
       // Output the string label
          curprint(arithmeticIf->get_string_label() + ": ");
        }
#endif

  // condition
     curprint("IF (");
     info.set_inConditional();

  // DQ (8/15/2007): In C the condiion is a statment, and in Fortran the condition is an expression!
  // We might want to fix this by having an IR node to represent the Fortran "if" statement.
  // unparseStatement(if_stmt->get_conditional(), info);
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
FortranCodeGeneration_locatedNode::unparseAssignStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     printf ("Sorry, unparseAssignStmt() not implemented \n");
   }

void
FortranCodeGeneration_locatedNode::unparseComputedGotoStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgComputedGotoStatement* computedGoto = isSgComputedGotoStatement(stmt);

     curprint("GOTO (");
  // SgLabelSymbolPtrList & labelList = computedGoto->get_labelList();
  // SgExprListExp* labelList = computedGoto->get_labelList();
     ASSERT_not_null(computedGoto->get_labelList());
     SgExpressionPtrList & labelList = computedGoto->get_labelList()->get_expressions();

     int size = labelList.size();
     for (int i=0; i < size; i++)
        {
       // SgLabelSymbol* labelSymbol = labelList[i];
          SgLabelRefExp* labelRefExp = isSgLabelRefExp(labelList[i]);
          ASSERT_not_null(labelRefExp);

          SgLabelSymbol* labelSymbol = labelRefExp->get_symbol();

       // DQ (12/24/2007): Every numeric lable should have been associated with a statement!
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

  // unp->u_exprStmt->unparseExpression(elseWhereStatement->get_condition(), info);
     unparseExpression(computedGoto->get_label_index(), info);
     unp->cur.insert_newline(1);
   }

void
FortranCodeGeneration_locatedNode::unparseAssignedGotoStmt(SgStatement* stmt, SgUnparse_Info& info)
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
  // unparseStatement(mod->get_body(), ninfo);
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
     ROSE_ASSERT(proghdr);

     if (!proghdr->isForward() && proghdr->get_definition() != NULL && !info.SkipFunctionDefinition())
        {
       // Output the function declaration with definition
       // printf ("Output the SgProgramHeaderStatement declaration with definition \n");
       // curprint ("! Output the function declaration with definition \n ");

       // The unparsing of the definition will cause the unparsing of the declaration (with SgUnparse_Info
       // flags set to just unparse a forward declaration!)
          SgUnparse_Info ninfo(info);

       // To avoid end of statement formatting (added CR's) we call the unparseFuncDefnStmt directly
       // unparseStatement(proghdr->get_definition(), ninfo);
          unparseFuncDefnStmt(proghdr->get_definition(), ninfo);

          unparseStatementNumbersSupport(proghdr->get_end_numeric_label(),info);

       // This is a special name for the case where the program header should not be output (it did not appear in the original source file).
       // if (proghdr->get_name() != "rose_implicit_program_header")
          if (proghdr->get_name() != ROSE_IMPLICIT_FORTRAN_PROGRAM_NAME)
             {
            // DQ (8/19/2007): The "END" has just been output by the unparsing of the SgFunctionDefinition 
            // so we just want to finish it off with "PROGRAM <name>".
               curprint("END PROGRAM ");
               if (proghdr->get_named_in_end_statement() == true)
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
       // printf ("Output the forward declaration only \n");
       // curprint ("! Output the forward declaration only \n ");

       // This is a special name for the case where the program header should not be output (it did not appear in the original source file).
       // if (proghdr->get_name() != "rose_implicit_program_header")
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

#if 0
  // don't generate an explicit interface if the body is empty ... (why not?)
     if (interfaceStatement->get_body() != NULL)
        {
          string nm = if_stmt->get_name().str();
          curprint("INTERFACE ");
          curprint(nm);
 
       // body
          unparseStatement(if_stmt->get_body(), info);
  
       // scope
       // SgStatement* scope = if_stmt->get_scope();
       // if (scope) { unparseStatement(scope, info); }
  
          unparseStatementNumbersSupport(if_stmt->get_end_numeric_label(),info);

          curprint("END INTERFACE");

          ASSERT_not_null(unp);
          unp->cur.insert_newline(1); 
       }
#else
     string nm = interfaceStatement->get_name().str();
     curprint("INTERFACE ");

  // curprint(nm);
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
               ROSE_ASSERT(false);
             }
        }

     unp->cur.insert_newline(1); 

#if 0
  // ASSERT_not_null(interfaceStatement->get_body());
  // unparseStatement(interfaceStatement->get_body(), info);
#if 0
     if (interfaceStatement->get_function() != NULL)
        {
          unparseStatement(interfaceStatement->get_function(), info);
        }
#else
  // DQ (10/1/2008): Modified IR to support multiple interface specifications (function).
  // However, this needs more work to just support the output of a non-defining declaration 
  // instead of the defining declaration as is done currently.
     for (size_t i = 0; i < interfaceStatement->get_interface_procedure_declarations().size(); i++)
        {
#if 0
          printf ("interfaceStatement->get_interface_procedure_declarations()[i] = %p = %s \n",
               interfaceStatement->get_interface_procedure_declarations()[i],
               interfaceStatement->get_interface_procedure_declarations()[i]->class_name().c_str());
#endif
       // unparseStatement(interfaceStatement->get_interface_specifications()[i], info);
          SgProcedureHeaderStatement* procedure = isSgProcedureHeaderStatement(interfaceStatement->get_interface_procedure_declarations()[i]);

          if (interfaceStatement->get_generic_spec() == SgInterfaceStatement::e_assignment_interface_type)
             {
            // Assignment operators are handled as a special case (since there may not have been 
            // enough information in the origianl source to build the function prototype)...
               string procedureName = procedure->get_name().getString();
               curprint("MODULE PROCEDURE ");
               curprint(procedureName);
             }
            else
             {
#if 0
               printf ("procedure = %p procedure->get_definingDeclaration() = %p procedure->get_firstNondefiningDeclaration() = %p \n",procedure,procedure->get_definingDeclaration(),procedure->get_firstNondefiningDeclaration());
#endif
               unparseStatement(procedure, info);
             }

          unp->cur.insert_newline(1); 
        }
#endif
#else
     for (size_t i = 0; i < interfaceStatement->get_interface_body_list().size(); i++)
        {
#if 0
          printf ("interfaceStatement->get_interface_body_list()[i] = %p = %s \n",
               interfaceStatement->get_interface_body_list()[i],
               interfaceStatement->get_interface_body_list()[i]->class_name().c_str());
#endif
          bool outputFunctionName = interfaceStatement->get_interface_body_list()[i]->get_use_function_name();
          SgName functionName = interfaceStatement->get_interface_body_list()[i]->get_function_name();
          SgFunctionDeclaration* functionDeclaration = interfaceStatement->get_interface_body_list()[i]->get_functionDeclaration();

#if 0
          printf ("outputFunctionName = %s \n",outputFunctionName ? "true" : "false");
          printf ("functionName = %s \n",functionName.str());
          if (functionDeclaration != NULL)
               printf ("functionDeclaration = %p = %s \n",functionDeclaration,functionDeclaration->class_name().c_str());
#endif
          if (outputFunctionName == true)
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
#endif

     unparseStatementNumbersSupport(interfaceStatement->get_end_numeric_label(),info);

     curprint("END INTERFACE ");

  // DQ (10/2/2008): At least for an "interface assignment(=)", it is an error to output the name
  // curprint(nm);

     ASSERT_not_null(unp);
     unp->cur.insert_newline(1); 
#endif
   }

void
FortranCodeGeneration_locatedNode::unparseCommonBlock(SgStatement* stmt, SgUnparse_Info& info)
   {
#if 0
  // Sage node corresponds to Fortran common block
     SgCommonBlock* comblk = isSgCommonBlock(stmt);
     ASSERT_not_null(comblk);

     string nm = comblk->get_name().str();
     SgInitializedNamePtrList& vars = comblk->get_variables();
  
     curprint("COMMON /");
     curprint(nm);
     curprint("/ ");
     unparseInitNamePtrList(&vars, info);
#else
     SgCommonBlock* commonBlock = isSgCommonBlock(stmt);
     ASSERT_not_null(commonBlock);

     curprint("COMMON ");

     SgCommonBlockObjectPtrList & blockList = commonBlock->get_block_list();
     SgCommonBlockObjectPtrList::iterator i = blockList.begin();
     while (i != blockList.end())
        {
          curprint("/ ");
          curprint((*i)->get_block_name());
          curprint(" / ");
          unparseExpression((*i)->get_variable_reference_list(),info);

          i++;

          if (i != blockList.end())
             {
               curprint(", ");
             }
        }

     unp->cur.insert_newline(1);
#endif
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

  // DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
  // int inClass = false;

     SgName inCname;
     ROSE_ASSERT(vardecl->get_parent());
     SgClassDefinition *cdefn = isSgClassDefinition(vardecl->get_parent());
     if (cdefn)
        {
       // DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
       // inClass = true;
          inCname = cdefn->get_declaration()->get_name();
          if (cdefn->get_declaration()->get_class_type()        == SgClassDeclaration::e_class)
               ninfo.set_CheckAccess();
        }

  // DQ (10/3/2008): This should not be called for Fortran code!
  // printAccessModifier(vardecl, ninfo);

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

  // After a variable declaration insert a new line
  // curprint(" ! After a variable declaration ");
     unp->cur.insert_newline(1);
   }

void
FortranCodeGeneration_locatedNode::unparseVarDefnStmt(SgStatement* stmt, SgUnparse_Info& info)
{
  // Sage node has no Fortran correspondence
  SgVariableDefinition* vardefn_stmt = isSgVariableDefinition(stmt);
  ASSERT_not_null(vardefn_stmt);
  ROSE_ASSERT(false && "FortranCodeGeneration_locatedNode::unparseVarDefnStmt");
}

void
FortranCodeGeneration_locatedNode::unparseParamDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
{
  // Sage node corresponds to Fortran parameter declaration
  ROSE_ASSERT(false && "FortranCodeGeneration_locatedNode::unparseParamDeclStmt");
}

void
FortranCodeGeneration_locatedNode::unparseUseStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // Sage node corresponds to Fortran use statement
  
     SgUseStatement* useStmt = isSgUseStatement(stmt);
     ASSERT_not_null(useStmt);

     curprint("USE ");
     curprint(useStmt->get_name().str());

#if 0
     SgExprListExp*       u_rename = useStmt->get_rename_list();
     SgUseOnlyExpression* u_only   = useStmt->get_use_only();
  
     if (u_rename)
        {
          curprint(",");
       // DQ (3/28/2017): Eliminate warning of overloaded virtual function in base class (from Clang).
       // unparseExprList(u_rename, info, false /*paren*/);
          unparseExprList(u_rename, info);
        }
       else
        {
          if (u_only)
             {
               unparseUseOnly(u_only, info);
             }
        }
#else
     if (useStmt->get_only_option() == true)
        {
         // FMZ: move comma here
          curprint(", ");
          curprint("ONLY : ");

       // printf ("Need to output use-only name/rename list \n");
        }

     int listSize = useStmt->get_rename_list().size();
     if (listSize > 0 && !useStmt->get_only_option())  // need to print a comma and a space
         curprint(", ");
     for (int i=0; i < listSize; i++)
        {
          SgRenamePair* renamePair = useStmt->get_rename_list()[i];
          ASSERT_not_null(renamePair);

          if (renamePair->isRename() == true)
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

  // curprint(" ! name/rename list ");
#endif

     unp->cur.insert_newline(1);
   }


//----------------------------------------------------------------------------
//  FortranCodeGeneration_locatedNode::<executable statements, control flow>
//----------------------------------------------------------------------------

void
FortranCodeGeneration_locatedNode::unparseBasicBlockStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // printf ("In FortranCodeGeneration_locatedNode::unparseBasicBlockStmt %p\n", stmt);

     SgBasicBlock* basic_stmt = isSgBasicBlock(stmt);
     ASSERT_not_null(basic_stmt);

#if 1
  // DQ (10/6/2008): Adding space here is required to get "else if" blocks formatted correctly (at least).
     unp->cur.format(basic_stmt, info, FORMAT_BEFORE_BASIC_BLOCK1);

  // Not required for correct unparsing
  // unp->cur.format(basic_stmt, info, FORMAT_BEFORE_BASIC_BLOCK2);
#endif

     SgStatementPtrList::iterator p = basic_stmt->get_statements().begin();
     for ( ; p != basic_stmt->get_statements().end(); ++p)
     {
       // cout << "stmt: " << hex << (*p) << dec << endl;
          ASSERT_not_null((*p));
         // FMZ: for module file, only output the variable declarations (not definitions)
         // Pei-Hung (05/23/2019) Need to add SgUseStatement, SgimplicitStatement and SgDerivedTypeStatement into rmod file
         if ( !info.outputFortranModFile() || (*p)->variantT()==V_SgVariableDeclaration
                 || (*p)->variantT()==V_SgAttributeSpecificationStatement // DXN (02/07/2012): unparse attribute statements also
                 || (*p)->variantT()==V_SgUseStatement
                 || (*p)->variantT()==V_SgImplicitStatement
                 || (*p)->variantT()==V_SgDerivedTypeStatement)
             unparseStatement((*p), info);
     }

  // Liao (10/14/2010): This helps handle cases such as 
  //    c$OMP END PARALLEL
  //          END
     unparseAttachedPreprocessingInfo(basic_stmt, info, PreprocessingInfo::inside);

#if 0
  // DQ (10/6/2008): This does not appear to be required (passes all tests).
     unp->cur.format(basic_stmt, info, FORMAT_AFTER_BASIC_BLOCK1);
     unp->cur.format(basic_stmt, info, FORMAT_AFTER_BASIC_BLOCK2);
#endif
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

     return (else_if_stmt != NULL);
   }


SgIfStmt*
getElseIfStatement ( SgIfStmt* parentIfStatement )
   {
  // This returns the elseif statement in a SgIfStmt object, else returns NULL.

     SgIfStmt* childIfStatement = NULL;

     SgBasicBlock* falseBlock = isSgBasicBlock(parentIfStatement->get_false_body());

  // Rasmussen (7/23/2018): Simplification of logic allowed because usage of is_else_if_statement
  // was fixed in frontend.  Previously is_else_if_statement was not used in the unparser and
  // this confused users at NCAR when attempting transformations.
  //
     if (falseBlock != NULL)
        {
          if (falseBlock->get_statements().empty() == false)
             {
               childIfStatement = isSgIfStmt(*(falseBlock->get_statements().begin()));
               if (childIfStatement != NULL)
                  {
                     if (childIfStatement->get_is_else_if_statement() == false)
                        {
                           childIfStatement = NULL;
                        }
                  }
             }
        }

  // Rasmussen (7/23/2018): This branch added for the experimental Fortran parser where
  // the AST was designed to follow the C if statement.  For the new design there is no
  // SgBasicBlock immediately preceding the SgIfStmt (the SgIfStmt is the false branch).
  //
     SgIfStmt* else_if_stmt = isSgIfStmt(parentIfStatement->get_false_body());
     if (else_if_stmt != NULL)
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
  //
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

#if 0
     printf ("\nIn unparseIfStmt(): if_stmt = %p line = %d \n", if_stmt, if_stmt->get_file_info()->get_line());
     printf ("In unparseIfStmt(): if_stmt->get_use_then_keyword()     = %s \n",if_stmt->get_use_then_keyword() ? "true" : "false");
     printf ("In unparseIfStmt(): if_stmt->get_is_else_if_statement() = %s \n",if_stmt->get_is_else_if_statement() ? "true" : "false");
     printf ("In unparseIfStmt(): if_stmt->get_has_end_statement()    = %s \n",if_stmt->get_has_end_statement() ? "true" : "false");
     printf ("In unparseIfStmt(): if_stmt->get_string_label()         = %s \n",if_stmt->get_string_label().c_str());
     printf ("In unparseIfStmt(): elseIfStatement                     = %p \n", elseIfStatement);
#endif

// Rasmussen(7/23/2018): Removed several lines of dead code containing logic that
// apparently conflicts with the Fortran standard.

  // THEN keyword
  //
  // DQ (12/26/2007): If this is an elseif statement then output the "THEN" even though we will not output an "ENDIF"
     if (output_endif == true)
        {
       // IF THEN statement branch
       //
          ROSE_ASSERT(if_stmt->get_use_then_keyword() == true);
       // This branch taken for an if-then-stmt.
       // Note that the string label if output before "IF", not after "THEN"
          curprint("THEN");
          unparseStatement(if_stmt->get_true_body(), info);
        }
       else
        {
          if (if_stmt->get_use_then_keyword() == true)
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

          if (elseIfStatement != NULL)
             {
             // ELSE IF statement branch
                ROSE_ASSERT(elseIfStatement->get_is_else_if_statement() == true);

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
     if (output_endif == true)
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

     SgStatement* statement = NULL;
     if (forAllStatement->get_has_end_statement() == true)
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

     if (forAllStatement->get_has_end_statement() == true)
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

#if 0
  // This was redesigned for SDF parser using ATerms.  This was implemented incorrectly,
  // it should use SgAssignOp expressions instead.  If this is ever resurrected it must
  // be fixed [Rasmussen 2019.08.23].

  // The expressions in the forall header are in pairs (var, SgSubscriptExpression)
     ROSE_ASSERT( ( forAllHeader->get_expressions().size() % 2 ) == 0);
     int num_vars = forAllHeader->get_expressions().size() / 2;
     for (int i = 0; i <= num_vars; i += 2)
#endif

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
     SgStatement* statement = NULL;
     if (forAllStatement->get_has_end_statement() == true)
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
     if (forAllStatement->get_has_end_statement() == true)
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
  // SgExpression* initExp = isSgExprStatement(doloop->get_initialization())->get_the_expr();
  // SgExpression* initExp = isSgExprStatement(doloop->get_initialization())->get_expression();
     SgExpression* initExp = doloop->get_initialization();

  // DQ (11/16/2007): If the is a simple "DO" loop without expressions then this will fail (see test2007_73.f90).
  // SgAssignOp* init = isSgAssignOp(initExp);
  // ASSERT_not_null(init);

  // SgExpression* condExp = isSgExprStatement(doloop->get_condition())->get_the_expr();
  // SgExpression* condExp = isSgExprStatement(doloop->get_condition())->get_expression();
  // SgExpression* condExp = doloop->get_condition();
     SgExpression* condExp = doloop->get_bound();
     ASSERT_not_null(condExp);
  // SgBinaryOp* cond = (isSgLessOrEqualOp(condExp) ? isSgBinaryOp(isSgLessOrEqualOp(condExp)) : isSgBinaryOp(isSgGreaterOrEqualOp(condExp)));
  // ROSE_ASSERT(cond);

  // SgExpression* updateExp = isSgExprStatement(doloop->get_increment())->get_the_expr();
  // SgExpression* updateExp = isSgExprStatement(doloop->get_increment())->get_expression();
     SgExpression* updateExp = doloop->get_increment();
     ASSERT_not_null(updateExp);
  // SgAssignOp* update = isSgAssignOp(updateExp);
  // ROSE_ASSERT(update);
  // SgBinaryOp* update1 = (isSgAddOp(update->get_rhs_operand()) ? isSgBinaryOp(isSgAddOp(update->get_rhs_operand())) : isSgBinaryOp(isSgSubtractOp(update->get_rhs_operand())));
  // ROSE_ASSERT(update1);
  // induction var: (i = lb)
  // SgVarRefExp* inducVar = isSgVarRefExp(init->get_lhs_operand());
  // ROSE_ASSERT(inducVar);
  // lower bound: (i = lb)
  // SgExpression* lb = init->get_rhs_operand();
  // upper bound: (i <= ub) or (i >= ub)
  // SgExpression* ub = cond->get_rhs_operand();
  // step: (i = i +/- step)
  // SgExpression* step = update1->get_rhs_operand();
  
     if (doloop->get_string_label().empty() == false)
        {
       // Output the string label
          curprint(doloop->get_string_label() + ": ");
        }

     curprint("DO ");

#if 0
     int loopEndLabel = doloop->get_end_numeric_label();
     if (loopEndLabel != -1)
        {
          string numeric_label_string = StringUtility::numberToString(loopEndLabel);
          curprint(numeric_label_string + " ");
        }
#else
  // DQ (12/24/2007): Every numeric label should have been associated with a statement!
  // SgLabelSymbol* endLabelSymbol = doloop->get_end_numeric_label();
  // ASSERT_not_null(doloop->get_end_numeric_label());
  // SgLabelSymbol* endLabelSymbol = doloop->get_end_numeric_label()->get_symbol();

  // if (endLabelSymbol != NULL)
     if (doloop->get_end_numeric_label() != NULL)
        {
          SgLabelSymbol* endLabelSymbol = doloop->get_end_numeric_label()->get_symbol();
          ASSERT_not_null(endLabelSymbol);

          ASSERT_not_null(endLabelSymbol->get_fortran_statement());
          int loopEndLabel = endLabelSymbol->get_numeric_label_value();
          string numeric_label_string = StringUtility::numberToString(loopEndLabel);
          curprint(numeric_label_string + " ");
        }
#endif

  // unparseExpression(inducVar, info);
  // curprint(" = ");
  // unparseExpression(lb, info);

     unparseExpression(initExp, info);
     if (isSgNullExpression(initExp) == NULL)
        {
          curprint(", ");
          unparseExpression(condExp, info);
        }

  // If this is NOT a SgNullExpression, then output the "," and the stride expression.
     if (isSgNullExpression(updateExp) == NULL)
        {
          curprint(", ");
          unparseExpression(updateExp, info);
        }

  // loop body (must always exist)
     SgStatement *body = doloop->get_body();
     unparseStatement(body, info);

  // unparseStatementNumbersSupport(doloop->get_end_numeric_label(),info);

     ASSERT_not_null(unp);
     unp->cur.insert_newline(1);

  // curprint("END DO");
#if 1
  // This setting converts all non-block where statements into blocked where statements.
  // So "DO I=1,2 B = 0" becomes:
  // "DO I=1,2
  //     B = 0
  //  END DO"
  // bool output_enddo = true;

  // DQ (12/24/2007): Control use of end-do
  // bool output_enddo = doloop->get_old_style() == false;
  // DQ (12/26/2007): handling cases where enddo is not in the source code and not required (stmt vs. construct)
     bool output_enddo = doloop->get_has_end_statement();
#else
     bool output_enddo = 
          (doloop->get_body()->get_statements().size() > 1) || 
          (doloop->get_string_label().empty() == false);
#endif

  // printf ("In unparseDoStmt(): output_enddo = %s \n",output_enddo ? "true" : "false");
     if (output_enddo == true)
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
  // Sage node corresponds to Fortran 'do while' (pre-test)
  
     SgWhileStmt* while_stmt = isSgWhileStmt(stmt);
     ASSERT_not_null(while_stmt);

     if (while_stmt->get_string_label().empty() == false)
        {
       // Output the string label
          curprint(while_stmt->get_string_label() + ": ");
        }

  // curprint("DO WHILE ");
     curprint("DO ");
#if 0
     int loopEndLabel = while_stmt->get_end_numeric_label();
     if (loopEndLabel != -1)
        {
          string numeric_label_string = StringUtility::numberToString(loopEndLabel);
          curprint(numeric_label_string + " ");
        }
#else
  // DQ (12/24/2007): Every numeric label should have been associated with a statement!
  // SgLabelSymbol* endLabelSymbol = while_stmt->get_end_numeric_label();
  // ASSERT_not_null(while_stmt->get_end_numeric_label());
  // SgLabelSymbol* endLabelSymbol = while_stmt->get_end_numeric_label()->get_symbol();

  // if (endLabelSymbol != NULL)
     if (while_stmt->get_end_numeric_label() != NULL)
        {
          SgLabelSymbol* endLabelSymbol = while_stmt->get_end_numeric_label()->get_symbol();

          ASSERT_not_null(endLabelSymbol->get_fortran_statement());
          int loopEndLabel = endLabelSymbol->get_numeric_label_value();
          string numeric_label_string = StringUtility::numberToString(loopEndLabel);
          curprint(numeric_label_string + " ");
        }
#endif

     curprint("WHILE ");
     curprint("(");
     info.set_inConditional(); // prevent printing line and file info
  // unparseStatement(while_stmt->get_condition()->get_expression(), info);
     SgExprStatement* conditionStatement = isSgExprStatement(while_stmt->get_condition());
     ASSERT_not_null(conditionStatement);
     unparseExpression(conditionStatement->get_expression(), info);
     info.unset_inConditional();
     curprint(")");
  
  // loop body (must always exist)
     unparseStatement(while_stmt->get_body(), info);

     unparseStatementNumbersSupport(while_stmt->get_end_numeric_label(),info);
  // curprint("END WHILE");
  // curprint("END DO");
#if 1
  // This setting converts all non-block where statements into blocked where statements.
  // So "DO WHILE (A) B = 0" becomes:
  // "DO WHILE (A)
  //     B = 0
  //  END DO"
  // bool output_endwhile = true;

  // DQ (12/26/2007): handling cases where enddo is not in the source code (for do while loop which is mapped to SgWhile IR node) and not required (stmt vs. construct)
     bool output_endwhile = while_stmt->get_has_end_statement();
#else
     bool output_endwhile = 
          (while_stmt->get_body()->get_statements().size() > 1) || 
          (while_stmt->get_string_label().empty() == false);
#endif
     if (output_endwhile == true)
        {
       // unparseStatementNumbersSupport(if_stmt->get_end_numeric_label(),info);
          curprint("END DO");
          if (while_stmt->get_string_label().empty() == false)
             {
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
  // DQ (8/14/2007): This has been changed to a statement because it is a statement in 
  // the C and C++ grammar, but it is an expression in the Fortran Grammar, I think).
  // unparseExpression(switch_stmt->get_item_selector(), info);
  // unparseStatement(switch_stmt->get_item_selector(), info);
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
FortranCodeGeneration_locatedNode::unparseBreakStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // This IR node corresponds to the Fortran 'exit'
     SgBreakStmt* break_stmt = isSgBreakStmt(stmt);
     ASSERT_not_null(break_stmt);
     curprint("EXIT");

  // If this is for a named do loop, this is the optional name.
     if (break_stmt->get_do_string_label().empty() == false)
        {
       // Output the string label
          curprint(" " + break_stmt->get_do_string_label());
        }
     unp->cur.insert_newline(1); 
   }

void
FortranCodeGeneration_locatedNode::unparseContinueStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgContinueStmt* continue_stmt = isSgContinueStmt(stmt);
     ASSERT_not_null(continue_stmt);

     curprint ("CYCLE");

  // If this is for a named do loop, this is the optional name.
     if (continue_stmt->get_do_string_label().empty() == false)
        {
       // Output the string label
          curprint(" " + continue_stmt->get_do_string_label());
        }
     unp->cur.insert_newline(1); 
   }

void 
FortranCodeGeneration_locatedNode::unparseLabelStmt(SgStatement* stmt, SgUnparse_Info& info) 
   {
  // This IR node corresponds to Fortran 'label continue'
     SgLabelStatement* label_stmt = isSgLabelStatement(stmt);
     ASSERT_not_null(label_stmt);

  // Note that the label is the numeric label (the name in get_label() and the numeric label also match)
  // curprint(label_stmt->get_label().str());
  // curprint(" CONTINUE");
     curprint("CONTINUE");
     unp->cur.insert_newline(1); 
   }

void
FortranCodeGeneration_locatedNode::unparseGotoStmt(SgStatement* stmt, SgUnparse_Info& info) 
   {
  // This IR node is the same for C and Fortran
     SgGotoStatement* goto_stmt = isSgGotoStatement(stmt);
     ASSERT_not_null(goto_stmt);
     curprint("GOTO ");

  // At present the goto statement holds a pointer to the SgLabelStatement, later it will use
  // hold a SgLabelSymbol.  this is both a better design and more consistant with the rest of ROSE.
#if 0
     ASSERT_not_null(goto_stmt->get_label());
     curprint(goto_stmt->get_label()->get_label().str());
#else
  // SgLabelSymbol* labelSymbol = goto_stmt->get_label_symbol();
     ASSERT_not_null(goto_stmt->get_label_expression());
     SgLabelSymbol* labelSymbol = goto_stmt->get_label_expression()->get_symbol();

     ASSERT_not_null(labelSymbol);

#if 0
     SgStatement* fortranStatement = labelSymbol->get_fortran_statement();
     ASSERT_not_null(fortranStatement);
     int numeric_label = fortranStatement->get_numeric_label();
#else
  // DQ (12/24/2007): Every numeric label should have been associated with a statement!
     ASSERT_not_null(labelSymbol->get_fortran_statement());
     int numeric_label = labelSymbol->get_numeric_label_value();
#endif

#if 0
  // DQ (12/24/2007): This is not a problem now that we have stored SgLabelSymbols 
  // and the numeric label value is held in the SgLabelSymbols object.

  // if (numeric_label < 0)
     if (labelSymbol->get_elseLabel() == true || labelSymbol->get_endLabel() == true)
        {
       // This could be a statement which has an end_numeric_label
       // ROSE_ASSERT(labelSymbol->get_elseLabel() == true || labelSymbol->get_endLabel() == true);
          switch(fortranStatement->variantT())
             {
               case V_SgFortranDo:
                  {
                    SgFortranDo* doStatement = isSgFortranDo(fortranStatement);
                    numeric_label = doStatement->get_end_numeric_label();
                    break;
                  }

               case V_SgProgramHeaderStatement:
                  {
                    SgProgramHeaderStatement* statement = isSgProgramHeaderStatement(fortranStatement);
                    numeric_label = statement->get_end_numeric_label();
                    break;
                  }

               default:
                  {
                    printf ("default reached: fortranStatement = %p = %s \n",fortranStatement,fortranStatement->class_name().c_str());
                    ROSE_ASSERT(false);
                  }
             }
        }
#endif

     ROSE_ASSERT(numeric_label >= 0);
     string numeric_label_string = StringUtility::numberToString(numeric_label);
     curprint(numeric_label_string);
#endif

     unp->cur.insert_newline(1); 
   }

void
FortranCodeGeneration_locatedNode::unparseStopOrPauseStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgStopOrPauseStatement* sp_stmt = isSgStopOrPauseStatement(stmt);
     ASSERT_not_null(sp_stmt);

  // SgStopOrPauseStatement::stop_or_pause knd = (SgStopOrPauseStatement::stop_or_pause) sp_stmt->get_stmt_kind();
     SgStopOrPauseStatement::stop_or_pause_enum kind = sp_stmt->get_stop_or_pause();

     if (kind == SgStopOrPauseStatement::e_stop)
        {
          curprint("STOP ");
          unparseExpression(sp_stmt->get_code(), info);
        }
     else if (kind == SgStopOrPauseStatement::e_error_stop)
        {
          curprint("ERROR STOP ");
          unparseExpression(sp_stmt->get_code(), info);
        }
     else
        {
          ROSE_ASSERT(kind == SgStopOrPauseStatement::e_pause);
          curprint("PAUSE ");
          unparseExpression(sp_stmt->get_code(), info);
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

  // if (altret != NULL)
     if (isSgNullExpression(altret) == NULL)
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

#if 0
void 
FortranCodeGeneration_locatedNode::unparseIOStmt(SgStatement* stmt, SgUnparse_Info& info) 
   {
  // Sage node corresponds to Fortran input/output statement

     printf ("In unparseIOStmt(): stmt = %p = %s \n",stmt,stmt->class_name().c_str());

     SgIOStatement* io_stmt = isSgIOStatement(stmt);
     ASSERT_not_null(io_stmt);

  // SgIOStatement::io_stmts iofn = (SgIOStatement::io_stmts)io_stmt->get_io_function();
     SgIOStatement::io_statement_enum iofn = io_stmt->get_io_statement();

  // DQ (8/15/2007): Need to handle this
     SgExprListExp* iolist = io_stmt->get_io_stmt_list();

     string iofn_nm;
     switch (iofn)
        {
          case SgIOStatement::e_rewind:    iofn_nm = "REWIND";    break;
          case SgIOStatement::e_backspace: iofn_nm = "BACKSPACE"; break;
          case SgIOStatement::e_endfile:   iofn_nm = "END FILE";  break;

          case SgIOStatement::e_print:
          case SgIOStatement::e_read:
          case SgIOStatement::e_write:
          case SgIOStatement::e_open:
          case SgIOStatement::e_close:
          case SgIOStatement::e_inquire:
             {
               printf ("Error: unparseIOStmt, these cases have there own unparse function that should be called io_stmt->get_io_statement() = %d \n",io_stmt->get_io_statement());
               ROSE_ASSERT(false);
             }

          default: 
             {
               printf ("Error: unparseIOStmt, default case in switch reached io_stmt->get_io_statement() = %d \n",io_stmt->get_io_statement());
               ROSE_ASSERT(false);
             }
        }

     curprint(iofn_nm);
#if 0
  // DQ (8/15/2007): Need to handle this
     SgIOControlStatement* ioctrl = io_stmt->get_io_control();
     if (ioctrl != NULL)
        {
          unparseIOCtrlStmt(ioctrl, info);
        }
       else
        {
       // This is the default case 
          curprint(" *,");
        }
#endif

     SgExpression* unit    = ioctrl_stmt->get_unit();
     SgExpression* err_lbl = ioctrl_stmt->get_err();
     SgExpression* iostat  = ioctrl_stmt->get_iostat();

     curprint(" (UNIT=");
     unparseExpression(unit, info);

     if (err_lbl)
        {
          curprint(", ERR=");
          unparseExpression(err_lbl, info);
        }

     if (iostat)
        {
          curprint(", IOSTAT=");
          unparseExpression(iostat, info);
        }
     
     curprint(" ");

  // DQ (3/28/2017): Eliminate warning of overloaded virtual function in base class (from Clang).
  // unparseExprList(iolist, info, false /*paren*/);
     unparseExprList(iolist, info);

     unp->cur.insert_newline(1); 
   }
#endif

#if 0
void
FortranCodeGeneration_locatedNode::unparse_IO_ControlStatement(SgStatement* stmt, SgUnparse_Info& info) 
   {
  // Sage node corresponds to Fortran IO control info
     SgIOControlStatement* ioctrl_stmt = isSgIOControlStatement(stmt);
     ASSERT_not_null(ioctrl_stmt);

     SgExpression* unit    = ioctrl_stmt->get_unit();
     SgExpression* err_lbl = ioctrl_stmt->get_err();
     SgExpression* iostat  = ioctrl_stmt->get_iostat();

     curprint(" (UNIT=");
     unparseExpression(unit, info);

     if (err_lbl)
        {
          curprint(", ERR=");
          unparseExpression(err_lbl, info);
        }

     if (iostat)
        {
          curprint(", IOSTAT=");
          unparseExpression(iostat, info);
        }

#if 0
     if (isSgInputOutputStatement(ioctrl_stmt))
        {
          unparseInOutStmt(ioctrl_stmt, info);
        }
#else
     switch (ioctrl_stmt->variantT())
        {
          case V_SgIOControlStatement:
          // This is either a backspace, endfile, or rewind statement
             break;

          case V_SgReadStatement:
             unparseReadStatement(ioctrl_stmt, info);
             break;

          case V_SgWriteStatement:
             unparseWriteStatement(ioctrl_stmt, info);
             break;

          case V_SgOpenStatement:
             unparseOpenStatement(ioctrl_stmt, info);
             break;

          case V_SgCloseStatement:
             unparseCloseStatement(ioctrl_stmt, info);
             break;

          case V_SgInquireStatement:
             unparseInquireStatement(ioctrl_stmt, info);
             break;

          default:
             {
               printf ("Error: default reached ioctrl_stmt = %s \n",ioctrl_stmt->class_name().c_str());
             }
        }
#endif
     curprint(") ");
   }
#endif

#if 0
void 
FortranCodeGeneration_locatedNode::unparseInOutStmt(SgStatement* stmt, SgUnparse_Info& info) 
   {
  // Sage node corresponds to Fortran input/output statement
     SgInputOutputStatement* io_stmt = isSgInputOutputStatement(stmt);
     ASSERT_not_null(io_stmt);

  // SgVarRefExp* fmt      = io_stmt->get_format();
     SgExpression* fmt     = io_stmt->get_format();
     SgVarRefExp* nmlst    = io_stmt->get_namelist_nm();
     SgExpression* adv     = io_stmt->get_advance();
     SgExpression* end_lbl = io_stmt->get_end_label();
     SgExpression* eor_lbl = io_stmt->get_eor_label();
     SgExpression* rec     = io_stmt->get_rec();
     SgExpression* sz      = io_stmt->get_size();

     if (fmt)
        {
       // curprint(", FMT=\"");
          curprint(", FMT=");
          unparseExpression(fmt, info);
       // curprint("\"");
          curprint("");
        }
     if (nmlst)
        {
          curprint(", NML=");
          unparseExpression(nmlst, info);
        }
     if (adv)
        {
          curprint(", ADVANCE=");
          unparseExpression(adv, info);
        }
     if (end_lbl)
        {
          curprint(", END=");
          unparseExpression(end_lbl, info);
        }
     if (eor_lbl)
        {
          curprint(", EOR=");
          unparseExpression(eor_lbl, info);
        }
     if (rec)
        {
          curprint(", REC=");
          unparseExpression(rec, info);
        }
     if (sz)
        {
          curprint(", SIZE=");
          unparseExpression(sz, info);
        }
   }
#endif

void 
FortranCodeGeneration_locatedNode::unparsePrintStatement(SgStatement* stmt, SgUnparse_Info& info) 
   {
  // Sage node corresponds to Fortran input/output statement
     SgPrintStatement* printStatement = isSgPrintStatement(stmt);
     ASSERT_not_null(printStatement);

     curprint("PRINT ");

     SgExpression* fmt = printStatement->get_format();
     if (fmt != NULL)
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

  // DQ (3/28/2017): Eliminate warning of overloaded virtual function in base class (from Clang).
  // unparseExprList(iolist, info, false /*paren*/);
     unparseExprList(iolist, info);

     unp->cur.insert_newline(1); 
   }

// void
bool
FortranCodeGeneration_locatedNode::unparse_IO_Support(SgStatement* stmt, bool skipUnit, SgUnparse_Info& info)
   {
  // Sage node corresponds to Fortran IO control info
     SgIOStatement* io_stmt = isSgIOStatement(stmt);
     ASSERT_not_null(io_stmt);

  // Unit is always unparsed (required for all but print statement)
  // ASSERT_not_null(io_stmt->get_unit());

     bool isLeadingEntry = false;
     if (skipUnit == false)
        {
       // We need to generate code without the leading ","
       // unparse_IO_Control_Support("UNIT",io_stmt->get_unit(),info);

       // DQ (12/12/2010): Also for at least the gnu gfortran version 4.2.4, we can't output the "UNIT=" 
       // string for the write statement. See test2010_144.f90 for an example of this.
          bool skipOutputOfUnitString = (isSgWriteStatement(stmt) != NULL);
          if (skipOutputOfUnitString == false)
             {
               curprint("UNIT=");
             }

          if (io_stmt->get_unit() != NULL)
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
     isLeadingEntry = isLeadingEntry && (io_stmt->get_iostat() == NULL);

     unparse_IO_Control_Support("ERR",io_stmt->get_err(),isLeadingEntry,info);
     isLeadingEntry = isLeadingEntry && (io_stmt->get_err() == NULL);

     unparse_IO_Control_Support("IOMSG",io_stmt->get_iomsg(),isLeadingEntry,info);
     isLeadingEntry = isLeadingEntry && (io_stmt->get_iomsg() == NULL);

     return isLeadingEntry;
   }

void 
FortranCodeGeneration_locatedNode::unparse_IO_Control_Support( string name, SgExpression* expr, bool isLeadingEntry, SgUnparse_Info& info)
   {
     if (expr != NULL)
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
     if (readStatement->get_format() != NULL && readStatement->get_unit() == NULL)
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

  // DQ (3/28/2017): Eliminate warning of overloaded virtual function in base class (from Clang).
  // unparseExprList(iolist, info, false /*paren*/);
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

  // Added missing items to the io-control-spec-list [Rasmussen, 2019.05.31]

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

  // DQ (3/28/2017): Eliminate warning of overloaded virtual function in base class (from Clang).
  // unparseExprList(iolist, info, false /*paren*/);
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

  // The open statement does not have additional arguments
  // SgExprListExp* iolist = openStatement->get_io_stmt_list();
  // unparseExprList(iolist, info, false /*paren*/);
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

  // SgExprListExp* iolist = closeStatement->get_io_stmt_list();
  // unparseExprList(iolist, info, false /*paren*/);
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
     if (inquireStatement->get_iolengthExp() != NULL)
        {
       // This is the "INQUIRE (IOLENGTH=IOL)" case.
       // unparse_IO_Control_Support("IOLENGTH",inquireStatement->get_iolengthExp(),info);
          curprint("IOLENGTH=");
          unparseExpression(inquireStatement->get_iolengthExp(),info);
          isLeadingEntry = false;
        }
       else
        {
       // This is the "INQUIRE(inquire-spec-list)" case.

       // DQ (12/11/2010): Fix for test2010_139.f90.
          if (inquireStatement->get_unit() != NULL)
             {
            // Fortran rules don't allow output if "unit=*"
               isLeadingEntry = unparse_IO_Support(stmt,false,info);
             }

          unparse_IO_Control_Support("FILE",inquireStatement->get_file(),isLeadingEntry,info);

       // DQ (12/11/2010): If this fails then I guess we need to construct a more complex handling or a better approach).
          isLeadingEntry = isLeadingEntry && (inquireStatement->get_file() == NULL);
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
     if (iolist != NULL)
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
     if (iolist != NULL)
        {
       // DQ (3/28/2017): Eliminate warning of overloaded virtual function in base class (from Clang).
       // unparseExprList(iolist, info, false /*paren*/);
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
     if (iolist != NULL)
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
     if (iolist != NULL)
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
     if (iolist != NULL)
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
     if (iolist != NULL)
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





  // unp->cur.insert_newline(1);

     ASSERT_not_null(associateStatement->get_body());
     unparseStatement(associateStatement->get_body(),info);

  // unparseStatementNumbersSupport(-1);
     unparseStatementNumbersSupport(NULL,info);

     curprint("END ASSOCIATE");

     unp->cur.insert_newline(1); 
   }

#if 0
void
FortranCodeGeneration_locatedNode::unparseIOFileControlStatement(SgStatement* stmt, SgUnparse_Info& info) 
   {
  // Sage node corresponds to Fortran input/output statement
     SgIOFileControlStmt* statement = isSgIOFileControlStmt(stmt);
     ASSERT_not_null(statement);

     unparseIOStmt(statement,info);
   }
#endif

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

  // DQ (9/9/2016): These should have been setup to be the same.
     ninfo.set_SkipEnumDefinition();

  // curprint("!what line an I on?!");

  // printDebugInfo(getSgVariant(expr_stmt->get_expression()->variant()), true);
     unparseExpression(expr_stmt->get_expression(), ninfo);

     if (ninfo.inVarDecl())
        {
          curprint(",");
        }

  // DQ (8/15/2007): Added a new line!
     unp->u_sage->curprint_newline();
   }


//----------------------------------------------------------------------------
//  FortranCodeGeneration_locatedNode::<pragmas>
//----------------------------------------------------------------------------

void
FortranCodeGeneration_locatedNode::unparsePragmaDeclStmt (SgStatement* stmt, SgUnparse_Info& info)
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


#if 0
//----------------------------------------------------------------------------
//  FortranCodeGeneration_locatedNode::unparseAttachedPreprocessingInfo
//----------------------------------------------------------------------------

// DQ (8/19/2008): This is defined in the base class and is language independent.

void
FortranCodeGeneration_locatedNode::unparseAttachedPreprocessingInfo(SgStatement* stmt,SgUnparse_Info& info,PreprocessingInfo::RelativePositionType whereToUnparse)
   {
     AttachedPreprocessingInfoType *ppInfo = stmt->getAttachedPreprocessingInfo();
  if (!ppInfo) {
    return;
  }

  // Continue only if options indicate
  if (info.SkipComments()) {
    return;
  }
  
  // Traverse the container of PreprocessingInfo objects, unparsing if
  // necessary.
  AttachedPreprocessingInfoType::iterator i;
  for (i = ppInfo->begin(); i != ppInfo->end(); ++i) {
    // Assert that i points to a valid preprocssingInfo object
    ASSERT_not_null((*i));
    ROSE_ASSERT ((*i)->getTypeOfDirective()  != PreprocessingInfo::CpreprocessorUnknownDeclaration);
    ROSE_ASSERT ((*i)->getRelativePosition() == PreprocessingInfo::before || 
                 (*i)->getRelativePosition() == PreprocessingInfo::after);
    
    // Check and see if the statement should be printed.
    if ((*i)->getRelativePosition() == whereToUnparse) {
      unp->cur.format(stmt, info, FORMAT_BEFORE_DIRECTIVE);
      
      switch ((*i)->getTypeOfDirective()) {
        // Comments don't have to be further commented
      case PreprocessingInfo::C_StyleComment:
      case PreprocessingInfo::CplusplusStyleComment:
        if ( !info.SkipComments() ) {
          curprint("! ");
     curprint((*i)->getString());
        }
        break;
        
      default:
        printf ("Error: FortranCodeGeneration_locatedNode::unparseAttachedPreprocessingInfo(): default switch reached\n");
        ROSE_ABORT();
      }
      unp->cur.format(stmt, info, FORMAT_AFTER_DIRECTIVE);      
    }
  }
}
#endif

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
  // curprint("IMPLICIT NONE";

     unp->cur.format(stmt, info, FORMAT_AFTER_BASIC_BLOCK1);
   }


void
FortranCodeGeneration_locatedNode::unparseFuncArgs(SgInitializedNamePtrList* args, 
                              SgUnparse_Info& info)
{
  unparseInitNamePtrList(args, info);
}

void
FortranCodeGeneration_locatedNode::unparseInitNamePtrList(SgInitializedNamePtrList* args, 
                                     SgUnparse_Info& info)
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
        SgArrayType* arrayType = NULL;
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
  // FIXME: eventually we will probably use this
  // SgStorageModifier& storage = initializedName->get_storageModifier();

  // printf ("In unparseVarDecl(SgStatement,SgInitializedName,SgUnparse_Info): info.SkipBaseType() = %s \n",info.SkipBaseType() ? "true" : "false");
     if (info.SkipBaseType() == false )
        {
       // printf ("In unparseVarDecl(): calling unparseType on type = %p = %s \n",type,type->class_name().c_str());

          // DXN (08/19/2011): unparse the base type when there are more than one declared variables
         if (numVar > 1)
         {
            SgType* baseType = type->stripType(SgType::STRIP_ARRAY_TYPE);
            unp->u_fortran_type->unparseType(baseType, info, false);  // do not print type attributes such as dimension, length on the left of ::
         }
         else
            unp->u_fortran_type->unparseType(type, info, true);  // print type attribute on the left of ::

          // DQ (11/18/2007): Added support for ALLOCATABLE declaration attribute
          // SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(stmt);
          // ASSERT_not_null(variableDeclaration);

       // DIMENSION is already handled (within the unparsing of the type)
       // DQ (3/23/2008): Likely POINTER should also be handled in the unparsing of the type!

       // printf ("variableDeclaration->get_declarationModifier().get_typeModifier().isAllocatable() = %s \n",variableDeclaration->get_declarationModifier().get_typeModifier().isAllocatable() ? "true" : "false");
          if (variableDeclaration->get_declarationModifier().get_typeModifier().isAllocatable() == true)
             {
               curprint(", ALLOCATABLE");
             }

       // printf ("variableDeclaration->get_declarationModifier().get_typeModifier().isAsynchronous() = %s \n",variableDeclaration->get_declarationModifier().get_typeModifier().isAsynchronous() ? "true" : "false");
          if (variableDeclaration->get_declarationModifier().get_typeModifier().isAsynchronous() == true)
             {
               curprint(", ASYNCHRONOUS");
             }

       // Need to handle INTENT better
       // printf ("variableDeclaration->get_declarationModifier().get_typeModifier().isIntent_in() = %s \n",variableDeclaration->get_declarationModifier().get_typeModifier().isIntent_in() ? "true" : "false");
          if (variableDeclaration->get_declarationModifier().get_typeModifier().isIntent_in() == true)
             {
               curprint(", INTENT(IN)");
             }

       // printf ("variableDeclaration->get_declarationModifier().get_typeModifier().isIntent_out() = %s \n",variableDeclaration->get_declarationModifier().get_typeModifier().isIntent_out() ? "true" : "false");
          if (variableDeclaration->get_declarationModifier().get_typeModifier().isIntent_out() == true)
             {
               curprint(", INTENT(OUT)");
             }

       // printf ("variableDeclaration->get_declarationModifier().get_typeModifier().isIntent_inout() = %s \n",variableDeclaration->get_declarationModifier().get_typeModifier().isIntent_inout() ? "true" : "false");
          if (variableDeclaration->get_declarationModifier().get_typeModifier().isIntent_inout() == true)
             {
               curprint(", INTENT(INOUT)");
             }

       // printf ("variableDeclaration->get_declarationModifier().get_typeModifier().get_constVolatileModifier().isVolatile() = %s \n",variableDeclaration->get_declarationModifier().get_typeModifier().get_constVolatileModifier().isVolatile() ? "true" : "false");
          if (variableDeclaration->get_declarationModifier().get_typeModifier().get_constVolatileModifier().isVolatile() == true)
             {
               curprint(", VOLATILE");
             }

       // printf ("variableDeclaration->get_declarationModifier().get_storageModifier().isExtern() = %s \n",variableDeclaration->get_declarationModifier().get_storageModifier().isExtern() ? "true" : "false");
          if (variableDeclaration->get_declarationModifier().get_storageModifier().isExtern() == true)
             {
               if (type->variantT()==V_SgTypeVoid) //FMZ 6/17/2009
                  curprint("EXTERNAL");
               else 
               curprint(", EXTERNAL");
             }

       // Rasmussen (02/06/2018): Fortran contiguous array storage attribute
          if (variableDeclaration->get_declarationModifier().get_storageModifier().isContiguous() == true)
             {
               curprint(", CONTIGUOUS");
             }

       // Rasmussen (02/06/2018): Fortran CUDA support
          if (variableDeclaration->get_declarationModifier().get_storageModifier().isCudaDeviceMemory() == true)
             {
               curprint(", device");
             }
          if (variableDeclaration->get_declarationModifier().get_storageModifier().isCudaManaged() == true)
             {
               curprint(", managed");
             }
          if (variableDeclaration->get_declarationModifier().get_storageModifier().isCudaConstant() == true)
             {
               curprint(", constant");
             }
          if (variableDeclaration->get_declarationModifier().get_storageModifier().isCudaShared() == true)
             {
               curprint(", shared");
             }
          if (variableDeclaration->get_declarationModifier().get_storageModifier().isCudaPinned() == true)
             {
               curprint(", pinned");
             }
          if (variableDeclaration->get_declarationModifier().get_storageModifier().isCudaTexture() == true)
             {
               curprint(", texture");
             }

       // printf ("variableDeclaration->get_declarationModifier().get_typeModifier().get_constVolatileModifier().isConst() = %s \n",variableDeclaration->get_declarationModifier().get_typeModifier().get_constVolatileModifier().isConst() ? "true" : "false");
          if (variableDeclaration->get_declarationModifier().get_typeModifier().get_constVolatileModifier().isConst() == true)
             {
            // PARAMETER in Fortran implies const in C/C++
               curprint(", PARAMETER");
             }
#if 0
       // DQ (1/17/2011): Pointers and arrays are not correctly handled in the unparsing of the type directly.
       // DQ (5/14/2008): Note that POINTER is only relevant if the variable is NOT declared as ALLOCATABLE.
       //                 If it is ALLOCATABLE then POINTER is not used.
       // DQ (11/23/2007): A better implementation of this might require that we strip off some modifiers
       // printf ("initializedName->get_type() = %s \n",initializedName->get_type()->class_name().c_str());
       // if (isSgPointerType(initializedName->get_type()) != NULL)
          if ( (isSgPointerType(initializedName->get_type()) != NULL) && (variableDeclaration->get_declarationModifier().get_typeModifier().isAllocatable() == false) )
             {
               curprint(", POINTER");
             }
#endif
       // printf ("variableDeclaration->get_declarationModifier().get_accessModifier().isPublic() = %s \n",variableDeclaration->get_declarationModifier().get_accessModifier().isPublic() ? "true" : "false");
          if (variableDeclaration->get_declarationModifier().get_accessModifier().isPublic() == true)
             {
            // The PUBLIC keyword is only permitted within Modules
               if ( TransformationSupport::getModuleStatement(variableDeclaration) != NULL )
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

       // printf ("variableDeclaration->get_declarationModifier().get_accessModifier().isPrivate() = %s \n",variableDeclaration->get_declarationModifier().get_accessModifier().isPrivate() ? "true" : "false");
          if (variableDeclaration->get_declarationModifier().get_accessModifier().isPrivate() == true)
             {
            // The PRIVATE keyword is only permitted within Modules
               if ( TransformationSupport::getModuleStatement(variableDeclaration) != NULL )
                  {
                    curprint(", PRIVATE");
                  }
                 else
                  {
                    printf ("Warning: statement marked as private in non-module scope \n");
                  }
             }

       // DQ (10/25/2010): The protected semantics can be applied to each variable separately.
       // This may be true for other declarations which might force them to be handled similarly to this implementation below.
       // Note that in Fortran, PROTECTED is not used as a access modifier in the language (only PUBLIC and PRIVATE exist).
       // printf ("variableDeclaration->get_declarationModifier().get_accessModifier().isProtected() = %s \n",variableDeclaration->get_declarationModifier().get_accessModifier().isProtected() ? "true" : "false");
       // if (variableDeclaration->get_declarationModifier().get_accessModifier().isProtected() == true)
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
          if (is_protected == true && (variableList.empty() == false))
             {
               curprint(", PROTECTED");
             }

       // printf ("variableDeclaration->get_declarationModifier().get_typeModifier().isIntrinsic() = %s \n",variableDeclaration->get_declarationModifier().get_typeModifier().isIntrinsic() ? "true" : "false");
          if (variableDeclaration->get_declarationModifier().get_typeModifier().isIntrinsic() == true)
             {
               curprint(", INTRINSIC");
             }

          if (variableDeclaration->get_declarationModifier().isBind() == true)
             {
               curprint(", ");

            // This is factored so that it can be called for function declarations, and variable declarations
               unparseBindAttribute(variableDeclaration);
             }

       // printf ("variableDeclaration->get_declarationModifier().get_typeModifier().isOptional() = %s \n",variableDeclaration->get_declarationModifier().get_typeModifier().isOptional() ? "true" : "false");
          if (variableDeclaration->get_declarationModifier().get_typeModifier().isOptional() == true)
             {
               curprint(", OPTIONAL");
             }

       // printf ("variableDeclaration->get_declarationModifier().get_typeModifier().isSave() = %s \n",variableDeclaration->get_declarationModifier().get_typeModifier().isSave() ? "true" : "false");
          if (variableDeclaration->get_declarationModifier().get_typeModifier().isSave() == true)
             {
               curprint(", SAVE");
             }

       // printf ("variableDeclaration->get_declarationModifier().get_typeModifier().isTarget() = %s \n",variableDeclaration->get_declarationModifier().get_typeModifier().isTarget() ? "true" : "false");
          if (variableDeclaration->get_declarationModifier().get_typeModifier().isTarget() == true)
             {
               curprint(", TARGET");
             }
#if 0
          if (variableDeclaration->get_declarationModifier().get_typeModifier().isCoTarget())  // DXN (04/11/2011) TODO
             {
               curprint(", COTARGET");
             }

#endif

       // printf ("variableDeclaration->get_declarationModifier().get_typeModifier().isValue() = %s \n",variableDeclaration->get_declarationModifier().get_typeModifier().isValue() ? "true" : "false");
          if (variableDeclaration->get_declarationModifier().get_typeModifier().isValue() == true)
             {
               curprint(", VALUE");
             }

       //FMZ (4/14/2009): Cray Pointer
          if (isSgTypeCrayPointer(type) == NULL)
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

     if (isSgTypeCrayPointer(type) != NULL)
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
      if (init != NULL)
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
FortranCodeGeneration_locatedNode::printDeclModifier(SgDeclarationStatement* decl_stmt, SgUnparse_Info & info)
   {
     printf ("Access modifiers are handled differently for Fortran, this function printDeclModifier() should not be called! \n");
     ROSE_ASSERT(false);

  // DQ (10/3/2008): This should not be called for Fortran code!
  // printAccessModifier(decl_stmt, info);

  // printStorageModifier(decl_stmt, info);
   }

void
FortranCodeGeneration_locatedNode::printAccessModifier(SgDeclarationStatement * decl_stmt, SgUnparse_Info & info)
{
  // FIXME: this will look different for full-featured Fortran

     printf ("Access modifiers are handled differently for Fortran, this function printAccessModifier() should not be called! \n");
//   ROSE_ASSERT(false);

#if 0
  if (info.CheckAccess()) {
    ASSERT_not_null(decl_stmt);
    bool flag = false;
    if (info.isPrivateAccess()) {
      if (!decl_stmt->get_declarationModifier().get_accessModifier().isPrivate())
        flag = true;
    }
    else {
      if (info.isProtectedAccess()) {
        if (!decl_stmt->get_declarationModifier().get_accessModifier().isProtected())
          flag = true;
      }
      else {
        if (info.isPublicAccess()) {
          if (!decl_stmt->get_declarationModifier().get_accessModifier().isPublic())
            flag = true;
        }
        else
          flag = true;
      }
    }
    
    info.set_isUnsetAccess();
    
    if (decl_stmt->get_declarationModifier().get_accessModifier().isPrivate()) {
      info.set_isPrivateAccess();
      if (flag) {
        curprint("private: ");
      }
    }
    else {
      if (decl_stmt->get_declarationModifier().get_accessModifier().isProtected()) {
        info.set_isProtectedAccess();
        if (flag) {
          curprint("protected: ");
        }
      }
      else {
        /* default, always print Public */
        ROSE_ASSERT (decl_stmt->get_declarationModifier().get_accessModifier().isPublic() == true);
        info.set_isPublicAccess();
        if (flag) {
          curprint("public: ");
        }
      }
    }
  }
#endif
}

void
FortranCodeGeneration_locatedNode::unparseBindAttribute ( SgDeclarationStatement* declaration )
   {
  // Code generation support for "bind" attribute
  // if (procedureHeader->get_functionModifier().isBind() == true)
     if (declaration->get_declarationModifier().isBind() == true)
        {
          curprint(" bind(");

       // DQ (11/23/2007): We now use the linkage string to hold the bind_languag information.
       // curprint(procedureHeader->get_bind_language());
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
FortranCodeGeneration_locatedNode::printStorageModifier(SgDeclarationStatement* decl_stmt, SgUnparse_Info& info) 
   {
  // FIXME: this will look different for full-featured Fortran

     printf ("Access modifiers are handled differently for Fortran, this function printStorageModifier() should not be called! \n");
     ROSE_ASSERT(false);

  // printf ("printStorageModifier not implemented for Fortran \n");
   }

// void FortranCodeGeneration_locatedNode::unparseFuncDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
void
FortranCodeGeneration_locatedNode::unparseProcHdrStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // Sage node corresponds to Fortran procedure program unit

  // printf ("Inside of unparseProcHdrStmt() \n");
     SgProcedureHeaderStatement* procedureHeader = isSgProcedureHeaderStatement(stmt);
     ASSERT_not_null(procedureHeader);

  // Sage node corresponds to Fortran program
  // SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(stmt);
  // ASSERT_not_null(functionDeclaration);

  // ROSE_ASSERT(procedureHeader->get_functionModifier().isPure() == true);

     string typeOfFunction;
     if (procedureHeader->isFunction() == true)
        {
          typeOfFunction = " FUNCTION";
        }
       else
        {
          if (procedureHeader->isSubroutine() == true)
             {
               typeOfFunction = "SUBROUTINE";
             }
            else
             {
               ROSE_ASSERT (procedureHeader->isBlockData() == true);
               typeOfFunction = "BLOCK DATA";
             }
        }

     if (!procedureHeader->isForward() && procedureHeader->get_definition() != NULL && !info.SkipFunctionDefinition())
        {
       // Output the function declaration with definition
       // printf ("Output the SgProgramHeaderStatement declaration with definition \n");
       // curprint ("! Output the function declaration with definition \n ");

       // The unparsing of the definition will cause the unparsing of the declaration (with SgUnparse_Info
       // flags set to just unparse a forward declaration!)
          SgUnparse_Info ninfo(info);

       // To avoid end of statement formatting (added CR's) we call the unparseFuncDefnStmt directly
       // unparseStatement(proghdr->get_definition(), ninfo);
          unparseFuncDefnStmt(procedureHeader->get_definition(), ninfo);

          unp->cur.insert_newline(1);

       // DQ (8/19/2007): The "END" has just been output by the unparsing of the SgFunctionDefinition 
       // so we just want to finish it off with "PROGRAM <name>".

       // printf ("Need to add data member for end_numeric_label for SgFunctionDeclaration \n");
          unparseStatementNumbersSupport(procedureHeader->get_end_numeric_label(),info);
       // curprint("END FUNCTION ");
          curprint("END " + typeOfFunction + " ");
          if (procedureHeader->get_named_in_end_statement() == true)
             {
               curprint(procedureHeader->get_name().str());
             }

       // Output 2 new lines to better separate functions visually in the output
          unp->cur.insert_newline(1);
          unp->cur.insert_newline(2); //FMZ
        }
       else
        {
       // Code generation support for "pure" attribute
          if (procedureHeader->get_functionModifier().isPure() == true)
             {
               curprint("PURE ");
             }

          if (procedureHeader->get_functionModifier().isElemental() == true)
             {
               curprint("ELEMENTAL ");
             }

          if (procedureHeader->get_functionModifier().isRecursive() == true)
             {
               curprint("RECURSIVE ");
             }

       // Rasmussen (02/06/2018): CUDA function modifiers (prefix)

          if (procedureHeader->get_functionModifier().isCudaHost() == true)
             {
               curprint("attributes(host) ");
             }
          if (procedureHeader->get_functionModifier().isCudaGlobalFunction() == true)
             {
               curprint("attributes(global) ");
             }
          if (procedureHeader->get_functionModifier().isCudaDevice() == true)
             {
               curprint("attributes(device) ");
             }
          if (procedureHeader->get_functionModifier().isCudaGridGlobal() == true)
             {
               curprint("attributes(grid_global) ");
             }

       // Output the forward declaration only
       // printf ("Output the forward declaration only \n");
       // curprint ("! Output the forward declaration only \n ");

       //FMZ (5/13/2010): If there is declaration of "result", we need to check if the 
       //                 type of the function is already declared by the "result"
          bool need_type = true;
          string result_name_str;
          
          if (procedureHeader->get_result_name() != NULL) {
              SgInitializedName* rslt_name = procedureHeader->get_result_name();
              SgDeclarationStatement* rslt_decl = rslt_name->get_definition();

              // check declaraion stmts
              if (rslt_decl !=NULL) {
                   need_type = false;
                   result_name_str = rslt_name->get_name().str();
              }
   
          } 


          if (procedureHeader->isFunction() == true && need_type == true)
             {
            // DQ (12/18/2007): Unparse the return type
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
          if (procedureHeader->isBlockData() == false)
             {
               curprint("(");
               unparseFunctionArgs(procedureHeader,ninfo2);     
               curprint(")");
             }

#if 1
          unparseBindAttribute(procedureHeader);

       // Unparse the result(<name>) suffix if present
          if (procedureHeader->get_result_name() != NULL &&
                  procedureHeader->get_name()!= procedureHeader->get_result_name()->get_name())
             {
               curprint(" result(");
               curprint(procedureHeader->get_result_name()->get_name());
               curprint(")");
             }
#else
       // Code generation support for "bind" attribute
       // if (procedureHeader->get_functionModifier().isBind() == true)
          if (procedureHeader->get_declarationModifier().isBind() == true)
             {
               curprint(" bind(");

            // DQ (11/23/2007): We now use the linkage string to hold the bind_languag information.
            // curprint(procedureHeader->get_bind_language());
               curprint(procedureHeader->get_linkage());

               if (procedureHeader->get_binding_label().empty() == false)
                  {
                    curprint(",NAME=\"");
                    curprint(procedureHeader->get_binding_label());
                    curprint("\"");
                  }
               curprint(")");
             }
#endif
       // Output 1 new line so that new statements will appear on their own line after the SgProgramHeaderStatement declaration.
          unp->cur.insert_newline(1);
        }
   }

#if 0
void
FortranCodeGeneration_locatedNode::unparseFuncDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
#if 0
     printf ("Inside of unparseFuncDeclStmt() \n");
  // curprint ( string("/* Inside of Unparse_ExprStmt::unparseFuncDeclStmt */";
     curprint ( string("\n/* Inside of Unparse_ExprStmt::unparseFuncDeclStmt (" ) + StringUtility::numberToString(stmt) 
         + "): sage_class_name() = " + stmt->sage_class_name() + " */ \n");

     stmt->get_startOfConstruct()->display("Inside of unparseFuncDeclStmt()");

  // info.display("Inside of unparseFuncDeclStmt()");
#endif

     SgFunctionDeclaration* funcdecl_stmt = isSgFunctionDeclaration(stmt);
     ASSERT_not_null(funcdecl_stmt);

#if 0
     printf ("funcdecl_stmt = %p = %s \n",funcdecl_stmt,funcdecl_stmt->get_name().str());
     funcdecl_stmt->get_startOfConstruct()->display("Inside of unparseFuncDeclStmt()");
     if (funcdecl_stmt->get_definingDeclaration() != NULL)
          funcdecl_stmt->get_definingDeclaration()->get_startOfConstruct()->display("Inside of unparseFuncDeclStmt(): definingDeclaration");
     if (funcdecl_stmt->get_firstNondefiningDeclaration() != NULL)
          funcdecl_stmt->get_firstNondefiningDeclaration()->get_startOfConstruct()->display("Inside of unparseFuncDeclStmt(): firstNondefiningDeclaration");
#endif

#if OUTPUT_DEBUGGING_FUNCTION_NAME
  // Avoid output for both definition and declaration (twice) which unparsing the defining declaration.
     if (info.SkipFunctionDefinition() == false)
        {
          printf ("Inside of unparseFuncDeclStmt() name = %s  isTransformed() = %s fileInfo->isTransformed() = %s definition = %p isForward() = %s \n",
               funcdecl_stmt->get_qualified_name().str(),
               isTransformed (funcdecl_stmt) ? "true" : "false",
               funcdecl_stmt->get_file_info()->isTransformation() ? "true" : "false",
               funcdecl_stmt->get_definition(),
               funcdecl_stmt->isForward() ? "true" : "false");
        }
#endif

#if 0
  // printf ("Inside of Unparse_ExprStmt::unparseFuncDeclStmt(): funcdecl_stmt->get_from_template() = %s \n",
  //      funcdecl_stmt->get_from_template() ? "true" : "false");
  // if (funcdecl_stmt->get_from_template() == true)
  //      curprint ( string("/* Unparser comment: Templated Function */";
  // curprint ( string("\n/* Unparser comment: funcdecl_stmt->get_from_template() = " + 
  //        funcdecl_stmt->get_from_template() + " */";
     curprint ( string("\n/* Unparser comment: funcdecl_stmt->get_definition() = " ) + 
            StringUtility::numberToString(funcdecl_stmt->get_definition()) + " */");
#if 0
     curprint ( string("\n/* Unparser comment: funcdecl_stmt->get_definition_ref() = " ) + 
            StringUtility::numberToString(funcdecl_stmt->get_definition_ref()) + " */");
     curprint ( string("\n/* Unparser comment: funcdecl_stmt->get_forwardDefinition() = " ) + 
            StringUtility::numberToString(funcdecl_stmt->get_forwardDefinition()) + " */");
#endif
     curprint ( string("\n/* Unparser comment: funcdecl_stmt->get_definingDeclaration() = " ) + 
            StringUtility::numberToString(funcdecl_stmt->get_definingDeclaration()) + " */");
     curprint ( string("\n/* Unparser comment: funcdecl_stmt->get_firstNondefiningDeclaration() = " ) + 
            StringUtility::numberToString(funcdecl_stmt->get_firstNondefiningDeclaration()) + " */");
     curprint ( string("\n/* */");
#endif

  /* EXCEPTION HANDLING: Forward Declarations */
  // DO NOT use the file information sage gives us because the line information
  // refers to the function definition (if it is defined in the same file). Instead, 
  // the line of the forward declaration is set one after the line number of the 
  // previous node or directive. By doing this, any comments around the forward 
  // declaration will come after the declaration, since I'm setting the line number 
  // to be right after the previous thing we unparsed.

  // the following is a HACK. I want to know if this statement is in a header or
  // C++ file. If it is in a header file, then I proceed as normal because header 
  // files provide correct information. If the statement is in a C++ file and is a
  // forward declaration, then I must follow this HACK.
  /* EXCEPTION HANDLING: Forward Declarations */
     SgUnparse_Info ninfo(info);

  // DQ (10/10/2006): Do output any qualified names (particularly for non-defining declarations).
  // ninfo.set_forceQualifiedNames();

     if (!funcdecl_stmt->isForward() && funcdecl_stmt->get_definition() != NULL && !info.SkipFunctionDefinition())
        {
#if 1
       // printf ("Not a forward function (normal function) \n");
          curprint ( string("\n/* Not a forward function (normal function) */ \n") );
#endif
          unparseStatement(funcdecl_stmt->get_definition(), ninfo);
          if (funcdecl_stmt->isExternBrace())
             {
               curprint ( string(" }"));
             }
        }
       else
        {
#if 0
          printf ("Forward function (function prototype) \n");
          curprint ( string("\n/* Forward function (function prototype) */ \n"));
#endif
          SgClassDefinition *cdefn = isSgClassDefinition(funcdecl_stmt->get_parent());

          if (cdefn && cdefn->get_declaration()->get_class_type()==SgClassDeclaration::e_class)
               ninfo.set_CheckAccess();

       // printf ("Comment out call to get_suppress_atomic(funcdecl_stmt) \n");
#if 0
          if (get_suppress_atomic(funcdecl_stmt))
               ninfo.set_SkipAtomic();   // attributes.h
#endif

       // DQ (10/17/2004): Skip output of class definition for function declaration! C++ standard does not permit 
       // a defining declaration within a return type, function parameter, or sizeof expression. And by extention 
       // any function declaration!
          ninfo.set_SkipClassDefinition();
          ninfo.set_SkipEnumDefinition();

       // DQ (8/16/2007): This is not required for Fortran, I think!
       // unp->u_sage->printSpecifier(funcdecl_stmt, ninfo);

          ninfo.unset_CheckAccess();
          info.set_access_attribute(ninfo.get_access_attribute());

          SgType *rtype = funcdecl_stmt->get_orig_return_type();
          if (!rtype)
               rtype = funcdecl_stmt->get_type()->get_return_type();
          ninfo.set_isTypeFirstPart();

          SgUnparse_Info ninfo_for_type(ninfo);

       // output the return type
#define OUTPUT_FUNCTION_DECLARATION_DATA 0
#if OUTPUT_FUNCTION_DECLARATION_DATA
          curprint ( string("\n/* output the return type */ \n"));
#endif

       // unp->u_type->unparseType(rtype, ninfo);
       // unp->u_type->unparseType(rtype, ninfo_for_type);
          unp->u_fortran_type->unparseType(rtype, ninfo_for_type);
          curprint(" FUNCTION ");

       // output the rest of the function declaration
#if OUTPUT_FUNCTION_DECLARATION_DATA
          curprint ( string("/* calling unparse_helper */"));
#endif

       // DQ (10/15/2006): Mark that we are unparsing a function declaration (or member function declaration)
       // this will help us know when to trim the "::" prefix from the name qualiciation.  The "::" global scope
       // qualifier is not used in function declarations, but is used for function calls.
          ninfo.set_declstatement_ptr(NULL);
          ninfo.set_declstatement_ptr(funcdecl_stmt);

          unparse_helper(funcdecl_stmt, ninfo);

       // DQ (10/15/2006): Matching call to unset the stored declaration.
          ninfo.set_declstatement_ptr(NULL);

       // curprint ( string("/* DONE: calling unparse_helper */";

          ninfo.set_isTypeSecondPart();

#if OUTPUT_FUNCTION_DECLARATION_DATA
          curprint ( string("/* output the second part of the type */"));
#endif

       // unp->u_type->unparseType(rtype, ninfo);
          unp->u_fortran_type->unparseType(rtype, ninfo);

#if OUTPUT_FUNCTION_DECLARATION_DATA
          curprint ( string("/* DONE: output the second part of the type */"));
#endif

#if 0
       // DQ (4/28/2004): Added support for throw modifier
          if (funcdecl_stmt->get_declarationModifier().isThrow())
             {
            // printf ("Output throw modifier (incomplete implementation) \n");
            // curprint ( string(" throw( /* from unparseFuncDeclStmt() type list output not implemented */ )";
               SgTypePtrList* exceptionSpecifierList = funcdecl_stmt->get_exceptionSpecification();
               unparseExceptionSpecification(exceptionSpecifierList,info);
             }
#endif
          if (funcdecl_stmt->isForward() && !ninfo.SkipSemiColon())
             {
               curprint ( string(";"));
               if (funcdecl_stmt->isExternBrace())
                  {
                    curprint ( string(" }"));
                  }
             }
        }

#if 0
  // DQ (4/28/2004): Added support for throw modifier
     if (info.SkipClassDefinition() && funcdecl_stmt->get_declarationModifier().isThrow())
        {
          curprint ( string(" throw()"));
        }
#else
  // DQ (8/19/2007): For Fortran this should always be false
     ROSE_ASSERT( funcdecl_stmt->get_declarationModifier().isThrow() == false );
#endif

#if 0
  // DQ (1/23/03) Added option to support rewrite mechanism (generation of declarations)
     if (info.AddSemiColonAfterDeclaration())
        {
          curprint ( string(";"));
        }
#else
  // DQ (8/19/2007): For Fortran this should always be false
     ROSE_ASSERT( info.AddSemiColonAfterDeclaration() == false );
#endif

#if 0
  // curprint ( string("/* End of Unparse_ExprStmt::unparseFuncDeclStmt */";
     curprint ( string("\n/* End of Unparse_ExprStmt::unparseFuncDeclStmt (" ) + StringUtility::numberToString(stmt) 
         + "): sage_class_name() = " + stmt->sage_class_name() + " */ \n");
     printf ("End of Unparse_ExprStmt::unparseFuncDeclStmt() \n");
#endif
   }
#endif




void
FortranCodeGeneration_locatedNode::unparseFuncDefnStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
#if 0
     printf ("Inside of unparseFuncDefnStmt() \n");
     curprint ( string("/* Inside of Unparse_ExprStmt::unparseFuncDefnStmt */"));
#endif

     SgFunctionDefinition* funcdefn_stmt = isSgFunctionDefinition(stmt);
     ASSERT_not_null(funcdefn_stmt);

#if OUTPUT_HIDDEN_LIST_DATA
     outputHiddenListData (funcdefn_stmt);
#endif

  // Unparse any comments of directives attached to the SgFunctionParameterList
     ASSERT_not_null(funcdefn_stmt->get_declaration());
     if (funcdefn_stmt->get_declaration()->get_parameterList() != NULL)
         unparseAttachedPreprocessingInfo(funcdefn_stmt->get_declaration()->get_parameterList(), info, PreprocessingInfo::before);

     info.set_SkipFunctionDefinition();
     SgStatement *declstmt = funcdefn_stmt->get_declaration();

  // DQ (3/24/2004): Need to permit SgMemberFunctionDecl and SgTemplateInstantiationMemberFunctionDecl
  // if (declstmt->variant() == MFUNC_DECL_STMT)

  // DQ (5/8/2004): Any generated specialization needed to use the 
  // C++ syntax for explicit specification of specializations.
  // if (isSgTemplateInstantiationMemberFunctionDecl(declstmt) != NULL)
  //      curprint ( string("template<> ";

#if 0
     printf ("Inside of Unparse_ExprStmt::unparseFuncDefnStmt: calling unparseFuncDeclStmt or unparseFuncDeclStmt \n");
     curprint ( string("/* Inside of Unparse_ExprStmt::unparseFuncDefnStmt: calling unparseFuncDeclStmt or unparseFuncDeclStmt */"));
#endif

  // DQ (10/11/2006): As part of new implementation of qualified names we now default to the generation of all qualified names unless they are skipped.
  // info.set_SkipQualifiedNames();

  // DQ (10/15/2006): Mark that we are unparsing a function declaration (or member function declaration)
  // this will help us know when to trim the "::" prefix from the name qualiciation.  The "::" global scope
  // qualifier is not used in function declarations, but is used for function calls.
     info.set_declstatement_ptr(NULL);
     info.set_declstatement_ptr(funcdefn_stmt->get_declaration());
#if 0
  // DQ (8/17/2007): Ignore member function details while we debug the fortran support.
     if ( isSgMemberFunctionDeclaration(declstmt) != NULL )
          unparseMFuncDeclStmt( declstmt, info);
       else
          unparseFuncDeclStmt( declstmt, info);
#else
  // DQ (8/19/2007): Handle details of Program header statement
     if ( isSgProgramHeaderStatement(declstmt) != NULL )
        {
          unparseProgHdrStmt ( declstmt, info );
        }
       else
        {
       // unparseFuncDeclStmt( declstmt, info );
          ASSERT_not_null(isSgProcedureHeaderStatement(declstmt));
          unparseProcHdrStmt( declstmt, info );
        }
#endif

  // DQ (10/15/2006): Also un-mark that we are unparsing a function declaration (or member function declaration)
     info.set_declstatement_ptr(NULL);

  // DQ (10/11/2006): As part of new implementation of qualified names we now default to the generation of all qualified names unless they are skipped.
  // info.unset_SkipQualifiedNames();

#if 0
     printf ("Inside of Unparse_ExprStmt::unparseFuncDefnStmt: output the function body \n");
     curprint ( string("/* Inside of Unparse_ExprStmt::unparseFuncDefnStmt: output the function body */"));
#endif

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

       // DQ (9/22/2004): I think this is an error!
          printf ("Error: Should be an error to not have a function body in the AST \n");
          ROSE_ASSERT(false);
        }

  // DQ (8/19/2007): We can't unparse this hear, since we might have to output a label before it.
  // DQ (8/19/2007): In the case of a program header statement, we want to still be able to output
  // the "PROGRAM <name>" text after the "END" so don't add a comment or CR after "END " here.
  // curprint ("\nEND ! end of function definition\n ");
  // curprint ("\n! end of function definition\n ");
  // curprint ("\nEND ");

  // Unparse any comments of directives attached to the SgFunctionParameterList
     unparseAttachedPreprocessingInfo(funcdefn_stmt->get_declaration()->get_parameterList(), info, PreprocessingInfo::after);
   }


void
FortranCodeGeneration_locatedNode::unparseFunctionParameterDeclaration ( 
   SgFunctionDeclaration* funcdecl_stmt, 
   SgInitializedName* initializedName,
   bool outputParameterDeclaration,
   SgUnparse_Info& info )
   {
  // In C/C++ this function is moderately complex, but for Fortran it seems to be very simple.

     ASSERT_not_null(funcdecl_stmt);
     ASSERT_not_null(initializedName);

#if 1
     curprint(initializedName->get_name().str());
#else
     SgName        tmp_name  = initializedName->get_name();
     SgInitializer *tmp_init = initializedName->get_initializer();

     SgType        *tmp_type = initializedName->get_type();

  // printf ("In unparseFunctionParameterDeclaration(): Argument name = %s \n",
  //      (tmp_name.str() != NULL) ? tmp_name.str() : "NULL NAME");

  // initializedName.get_storageModifier().display("New storage modifiers in unparseFunctionParameterDeclaration()");

     SgStorageModifier & storage = initializedName->get_storageModifier();
     if (storage.isExtern())
        {
          curprint( "extern ");
        }

  // DQ (7/202/2006): The isStatic() function in the SgStorageModifier held by the SgInitializedName object should always be false.
  // This is because the static-ness of a variable is held by the SgVariableDeclaration (and the SgStorageModified help in the SgDeclarationModifier).
  // printf ("In initializedName = %p test the return value of storage.isStatic() = %d = %d (should be boolean value) \n",initializedName,storage.isStatic(),storage.get_modifier());
     ROSE_ASSERT(storage.isStatic() == false);

  // This was a bug mistakenly reported by Isaac
     ROSE_ASSERT(storage.get_modifier() >= 0);

     if (storage.isStatic())
        {
          curprint( "static ");
        }

     if (storage.isAuto())
        {
       // DQ (4/30/2004): Auto is a default which is to be supressed 
       // in C old-style parameters and not really ever needed anyway?
       // curprint( "auto ");
        }

     if (storage.isRegister())
        {
          curprint( "register ");
        }

     if (storage.isMutable())
        {
          curprint( "mutable ");
        }

     if (storage.isTypedef())
        {
          curprint( "typedef ");
        }

     if (storage.isAsm())
        {
          curprint( "asm ");
        }

  // Error checking, if we are using old style C function parameters, then I hope this is not C++ code!
     if (funcdecl_stmt->get_oldStyleDefinition() == true)
        {
          if (SageInterface::is_Cxx_language() == true)
             {
               printf ("Mixing old style C function parameters with C++ is not well defined, I think \n");
             }
          ROSE_ASSERT (SageInterface::is_Cxx_language() == false);
        }

     if ( (funcdecl_stmt->get_oldStyleDefinition() == false) || (outputParameterDeclaration == true) )
        {
       // output the type name for each argument
          if (tmp_type != NULL)
             {
#if 0
            // DQ (10/17/2004): This is now made more uniform and output in the unparseType() function
               if (isSgNamedType(tmp_type))
                  {
                    SgName theName;
                    theName = isSgNamedType(tmp_type)->get_qualified_name().str();
                    if (!theName.is_null())
                       {
                         curprint( theName.str() << "::");
                       }
                  }
#endif
               info.set_isTypeFirstPart();
            // curprint( "\n/* unparse_helper(): output the 1st part of the type */ \n");

            // DQ (8/6/2007): Skip forcing the output of qualified names now that we have a hidden list mechanism.
            // DQ (10/14/2006): Since function can appear anywhere types referenced in function 
            // declarations have to be fully qualified.  We can't tell from the type if it requires 
            // qualification we would need the type and the function declaration (and then some 
            // analysis).  So fully qualify all function parameter types.  This is a special case
            // (documented in the Unparse_ExprStmt::unp->u_name->generateNameQualifier() member function.
            // info.set_forceQualifiedNames();

               SgUnparse_Info ninfo_for_type(info);

#if 1
            // DQ (12/20/2006): This is used to specify global qualification separately from the more general name 
            // qualification mechanism.  Note that SgVariableDeclarations don't use the requiresGlobalNameQualificationOnType
            // on the SgInitializedNames in their list since the SgVariableDeclaration IR nodes is marked directly.
               if (initializedName->get_requiresGlobalNameQualificationOnType() == true)
                  {
                 // Output the name qualification for the type in the variable declaration.
                 // But we have to do so after any modifiers are output, so in unparseType().
                 // printf ("In Unparse_ExprStmt::unparseFunctionParameterDeclaration(): This function parameter type requires a global qualifier \n");

                 // Note that general qualification of types is separated from the use of globl qualification.
                 // ninfo2.set_forceQualifiedNames();
                    ninfo_for_type.set_requiresGlobalNameQualification();
                  }
#endif

            // unparseType(tmp_type, info);
            // unp->u_type->unparseType(tmp_type, ninfo_for_type);
               unp->u_fortran_type->unparseType(tmp_type, ninfo_for_type);

            // curprint( "\n/* DONE - unparse_helper(): output the 1st part of the type */ \n");

            // forward declarations don't necessarily need the name of the argument
            // so we must check if not NULL before adding to chars_on_line
            // This is a more consistant way to handle the NULL string case
            // curprint( "\n/* unparse_helper(): output the name of the type */ \n");
               // if (tmp_name.str() != NULL)
                  // {
                    curprint( tmp_name.str());
                  // }
                 // else
                  // {
                 // printf ("In unparse_helper(): Argument name is NULL \n");
                  // }

            // output the rest of the type
               info.set_isTypeSecondPart();

            // info.display("unparse_helper(): output the 2nd part of the type");

            // printf ("unparse_helper(): output the 2nd part of the type \n");
            // curprint( "\n/* unparse_helper(): output the 2nd part of the type */ \n");
            // unp->u_type->unparseType(tmp_type, info);
               unp->u_fortran_type->unparseType(tmp_type, info);
            // printf ("DONE: unparse_helper(): output the 2nd part of the type \n");
            // curprint( "\n/* DONE: unparse_helper(): output the 2nd part of the type */ \n");
             }
            else
             {
               curprint( tmp_name.str()); // for ... case
             }
        }
       else
        {
          curprint( tmp_name.str()); // for ... case
        }
 
     SgUnparse_Info ninfo3(info);
     ninfo3.unset_inArgList();

  // DQ (6/16/2005): control output of initializers (should only be output once and never in 
  // generated declarations for member functions) member function function declaration declared 
  // outside of the class.  Avoid case of "X { public: X(int i = 0); }; X::X(int i = 0) {}"
  // see test2005_87.C for example and details.
     bool outputInitializer = true;
     if (funcdecl_stmt->get_scope() != funcdecl_stmt->get_parent())
        {
       // This function declaration is appearing in a different scope there where it was first 
       // declared so avoid output of the default initializers of any function parameters!
       // printf ("Skipping output of initializer since this is not the original declaration! \n");
          outputInitializer = false;
        }

  // Add an initializer if it exists
     if ( outputInitializer == true && tmp_init != NULL )
        {
       // DQ (6/14/2005): We only want to avoid the redefinition of function parameters.
       // DQ (4/20/2005): Removed from_template data member since it is redundant 
       //                 in design with handling of templates in ROSE.
       // if(!(funcdecl_stmt->get_from_template() && !funcdecl_stmt->isForward()))
       // if ( !funcdecl_stmt->isForward() )
          curprint( "=");
          unp->u_exprStmt->unparseExpression(tmp_init, ninfo3);
        }
#endif
   }

void
FortranCodeGeneration_locatedNode::unparseFunctionArgs(SgFunctionDeclaration* funcdecl_stmt, SgUnparse_Info& info)
   {
     ASSERT_not_null(funcdecl_stmt);

#if 0
     printf ("funcdecl_stmt->get_args().size() = %" PRIuPTR " \n",funcdecl_stmt->get_args().size());
     curprint( "\n/* funcdecl_stmt->get_args().size() = " << (int)(funcdecl_stmt->get_args().size()) << " */ \n");
#endif

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

#if 0
  // DQ (11/18/2004): Added support for qualified name of template declaration!
  // But it appears that the qualified name is included within the template text string so that 
  // we should not output the qualified name spearately!
     SgName nameQualifier = unp->u_name->generateNameQualifier( funcdecl_stmt , info );
  // printf ("In unparse_helper(): nameQualifier (from unp->u_name->generateNameQualifier function) = %s \n",nameQualifier.str());

  // DQ (10/12/2006): need to trim off the global scope specifier (I think).
  // curprint( "\n/* Calling trimGlobalScopeQualifier() */\n ");
  // curprint( "\n/* Skipping trimGlobalScopeQualifier() */\n ");
  // nameQualifier = trimGlobalScopeQualifier ( nameQualifier.str() ).c_str();

     if (nameQualifier.is_null() == false)
        {
          curprint( nameQualifier.str());
        }
#else
     printf ("In FortranCodeGeneration_locatedNode::unparse_helper(): Skipping name qualification for now, likely we WILL need this in Fortran 90 specific cases later \n");
#endif

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
     
  // printf ("Adding a closing \")\" to the end of the argument list \n");
     curprint(")");

#if 0
  // DQ (8/19/2007): This is not required for Fortran function declarations
     if ( funcdecl_stmt->get_oldStyleDefinition() )
        {
       // Output old-style C (K&R) function definition
       // printf ("Output old-style C (K&R) function definition \n");
       // curprint( "/* Output old-style C (K&R) function definition */ \n");

          SgInitializedNamePtrList::iterator p = funcdecl_stmt->get_args().begin();
          if (p != funcdecl_stmt->get_args().end())
               unp->u_sage->curprint_newline();

          while ( p != funcdecl_stmt->get_args().end() )
             {
            // Output declarations for function parameters (using old-style K&R syntax)
            // printf ("Output declarations for function parameters (using old-style K&R syntax) \n");
               unparseFunctionParameterDeclaration(funcdecl_stmt,*p,true,ninfo2);
               curprint( ";");
               unp->u_sage->curprint_newline();
               p++;
             }
        }
#endif

  // curprint( endl;
  // curprint( "Added closing \")\" to the end of the argument list \n");
  // curprint(flush();

  // printf ("End of function Unparse_ExprStmt::unparse_helper() \n");
   }

void
FortranCodeGeneration_locatedNode::unparseClassDeclStmt_derivedType(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgDerivedTypeStatement* classdecl_stmt = isSgDerivedTypeStatement(stmt);
     ASSERT_not_null(classdecl_stmt);

#if 0
  // printf ("Inside of Unparse_ExprStmt::unparseClassDeclStmt(): classdecl_stmt->get_from_template() = %s \n",
  //      classdecl_stmt->get_from_template() ? "true" : "false");
     if (classdecl_stmt->get_from_template() == true)
          curprint ( string("/* Unparser comment: Templated Class Declaration Function */"));
     Sg_File_Info* classDeclarationfileInfo = classdecl_stmt->get_file_info();
     ASSERT_not_null();
     if ( classDeclarationfileInfo->isCompilerGenerated() == false)
          curprint ( string("\n/* file: " ) + classDeclarationfileInfo->get_filename() + " line: " + classDeclarationfileInfo->get_line() + " col: " + classDeclarationfileInfo->get_col() + " */ \n");
#endif

  // info.display("Inside of unparseClassDeclStmt");

#if 0
     printf ("At top of unparseClassDeclStmt name = %s \n",classdecl_stmt->get_name().str());
     printf ("In Unparse_ExprStmt::unparseClassDeclStmt(): classdecl_stmt = %p isForward() = %s info.SkipClassDefinition() = %s name = %s \n",
          classdecl_stmt,(classdecl_stmt->isForward() == true) ? "true" : "false",
          (info.SkipClassDefinition() == true) ? "true" : "false",classdecl_stmt->get_name().str());
#endif

     if (!classdecl_stmt->isForward() && classdecl_stmt->get_definition() && !info.SkipClassDefinition())
        {
          SgUnparse_Info ninfox(info);

          ninfox.unset_SkipSemiColon();

       // DQ (6/13/2007): Set to null before resetting to non-null value 
          ninfox.set_declstatement_ptr(NULL);
          ninfox.set_declstatement_ptr(classdecl_stmt);

       // printf ("Calling unparseStatement(classdecl_stmt->get_definition(), ninfox); for %s \n",classdecl_stmt->get_name().str());
          unparseStatement(classdecl_stmt->get_definition(), ninfox);
       // curprint("! Comment in unparseClassDeclStmt_derivedType() ");

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

       // printf ("classdecl_stmt->get_declarationModifier().get_accessModifier().isPublic() = %s \n",classdecl_stmt->get_declarationModifier().get_accessModifier().isPublic() ? "true" : "false");
          if (classdecl_stmt->get_declarationModifier().get_accessModifier().isPublic() == true)
             {
            // The PUBLIC keyword is only permitted within Modules
               if ( TransformationSupport::getModuleStatement(classdecl_stmt) != NULL )
                  {
                    curprint(", PUBLIC");
                  }
                 else
                  {
                    printf ("Warning: statement marked as public in non-module scope in FortranCodeGeneration_locatedNode::unparseClassDeclStmt_derivedType(). \n");
                  }
             }

       // printf ("classdecl_stmt->get_declarationModifier().get_accessModifier().isPrivate() = %s \n",classdecl_stmt->get_declarationModifier().get_accessModifier().isPrivate() ? "true" : "false");
          if (classdecl_stmt->get_declarationModifier().get_accessModifier().isPrivate() == true)
             {
            // The PRIVATE keyword is only permitted within Modules
               if ( TransformationSupport::getModuleStatement(classdecl_stmt) != NULL )
                  {
                    curprint(", PRIVATE");
                  }
                 else
                  {
                    printf ("Warning: statement marked as private in non-module scope \n");
                  }
             }

       // printf ("classdecl_stmt->get_declarationModifier().get_typeModifier().isBind() = %s \n",classdecl_stmt->get_declarationModifier().get_typeModifier().isBind() ? "true" : "false");
          if (classdecl_stmt->get_declarationModifier().get_typeModifier().isBind() == true)
             {
            // The BIND keyword is only permitted within Modules
               if ( TransformationSupport::getModuleStatement(classdecl_stmt) != NULL )
                  {
                 // I think that bind implies "BIND(C)"
                    curprint(", BIND(C)");
                  }
                 else
                  {
                    printf ("Warning: statement marked as bind in non-module scope \n");
                  }
             }

       // printf ("classdecl_stmt->get_declarationModifier().get_typeModifier().isExtends() = %s \n",classdecl_stmt->get_declarationModifier().get_typeModifier().isExtends() ? "true" : "false");
          if (classdecl_stmt->get_declarationModifier().get_typeModifier().isExtends() == true)
             {
            // The EXTENDS keyword is only permitted within Modules
               if ( TransformationSupport::getModuleStatement(classdecl_stmt) != NULL )
                  {
                    curprint(", EXTENDS(PARENT-TYPE-NAME-NOT-IMPLEMENTED)");
                  }
                 else
                  {
                    printf ("Warning: statement marked as extends in non-module scope \n");
                  }
             }

       // printf ("classdecl_stmt->get_declarationModifier().get_typeModifier().isAbstract() = %s \n",classdecl_stmt->get_declarationModifier().get_typeModifier().isAbstract() ? "true" : "false");
          if (classdecl_stmt->get_declarationModifier().get_typeModifier().isAbstract() == true)
             {
            // The ABSTRACT keyword is only permitted within Modules
               if ( TransformationSupport::getModuleStatement(classdecl_stmt) != NULL )
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
  // SgClassDeclaration* classdecl_stmt = isSgClassDeclaration(stmt);
     SgModuleStatement* classdecl_stmt = isSgModuleStatement(stmt);
     ASSERT_not_null(classdecl_stmt);

#if 0
  // printf ("Inside of Unparse_ExprStmt::unparseClassDeclStmt(): classdecl_stmt->get_from_template() = %s \n",
  //      classdecl_stmt->get_from_template() ? "true" : "false");
     if (classdecl_stmt->get_from_template() == true)
          curprint ( string("/* Unparser comment: Templated Class Declaration Function */"));
     Sg_File_Info* classDeclarationfileInfo = classdecl_stmt->get_file_info();
     ASSERT_not_null();
     if ( classDeclarationfileInfo->isCompilerGenerated() == false)
          curprint ( string("\n/* file: " ) + classDeclarationfileInfo->get_filename() + " line: " + classDeclarationfileInfo->get_line() + " col: " + classDeclarationfileInfo->get_col() + " */ \n");
#endif

  // info.display("Inside of unparseClassDeclStmt");

  // printf ("At top of unparseClassDeclStmt name = %s \n",classdecl_stmt->get_name().str());

#if 0
     printf ("In Unparse_ExprStmt::unparseClassDeclStmt(): classdecl_stmt = %p isForward() = %s info.SkipClassDefinition() = %s name = %s \n",
          classdecl_stmt,(classdecl_stmt->isForward() == true) ? "true" : "false",
          (info.SkipClassDefinition() == true) ? "true" : "false",classdecl_stmt->get_name().str());
#endif

     if (!classdecl_stmt->isForward() && classdecl_stmt->get_definition() && !info.SkipClassDefinition())
        {
          SgUnparse_Info ninfox(info);

          ninfox.unset_SkipSemiColon();

       // DQ (6/13/2007): Set to null before resetting to non-null value 
          ninfox.set_declstatement_ptr(NULL);
          ninfox.set_declstatement_ptr(classdecl_stmt);

       // printf ("Calling unparseStatement(classdecl_stmt->get_definition(), ninfox); for %s \n",classdecl_stmt->get_name().str());
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
  // printf ("Inside of unparseClassDefnStmt \n");
  // curprint ( string("/* Inside of unparseClassDefnStmt */ \n";

     SgClassDefinition* classdefn_stmt = isSgClassDefinition(stmt);
     ASSERT_not_null(classdefn_stmt);

#if OUTPUT_HIDDEN_LIST_DATA
     outputHiddenListData (classdefn_stmt);
#endif

     SgUnparse_Info ninfo(info);

  // curprint ( string("/* Print out class declaration */ \n";

     ninfo.set_SkipClassDefinition();

  // DQ (9/9/2016): These should have been setup to be the same.
     ninfo.set_SkipEnumDefinition();

  // DQ (10/13/2006): test2004_133.C demonstrates where we need to unparse qualified names for class definitions (defining declaration).
  // DQ (10/11/2006): Don't generate qualified names for the class name of a defining declaration
  // ninfo.set_SkipQualifiedNames();

  // DQ (7/19/2003) skip the output of the semicolon
     ninfo.set_SkipSemiColon();

  // printf ("Calling unparseClassDeclStmt = %p isForward = %s from unparseClassDefnStmt = %p \n",
  //      classdefn_stmt->get_declaration(),(classdefn_stmt->get_declaration()->isForward() == true) ? "true" : "false",classdefn_stmt);
     ASSERT_not_null(classdefn_stmt->get_declaration());
  // unparseClassDeclStmt( classdefn_stmt->get_declaration(), ninfo);

  // printf ("classdefn_stmt->get_declaration() = %p = %s \n",classdefn_stmt->get_declaration(),classdefn_stmt->get_declaration()->class_name().c_str());
     if (isSgModuleStatement(classdefn_stmt->get_declaration()) != NULL)
        {
          unparseClassDeclStmt_module( classdefn_stmt->get_declaration(), ninfo);
        }
       else
        {
          unparseClassDeclStmt_derivedType( classdefn_stmt->get_declaration(), ninfo);
        }

  // DQ (7/19/2003) unset the specification to skip the output of the semicolon
     ninfo.unset_SkipSemiColon();

  // DQ (10/11/2006): Don't generate qualified names for the class name of a defining declaration
  // ninfo.unset_SkipQualifiedNames();

     ninfo.unset_SkipClassDefinition();

  // DQ (9/9/2016): These should have been setup to be the same.
     ninfo.unset_SkipEnumDefinition();

  // curprint ( string("/* END: Print out class declaration */ \n";

     SgNamedType *saved_context = ninfo.get_current_context();

  // DQ (11/29/2004): The use of a primary and secondary declaration casue two SgClassType nodes to be generated 
  // (which should be fixed) since this is compared to another SgClassType within the generateQualifiedName() 
  // function we have to get the the type from the non-defining declaration uniformally. Same way each time so that
  // the pointer test will be meaningful.
  // ninfo.set_current_context(classdefn_stmt->get_declaration()->get_type());
     ASSERT_not_null(classdefn_stmt->get_declaration()->get_firstNondefiningDeclaration());
     SgClassDeclaration* classDeclaration = isSgClassDeclaration(classdefn_stmt->get_declaration()->get_firstNondefiningDeclaration());
     ASSERT_not_null(classDeclaration->get_type());

  // DQ (6/13/2007): Set to null before resetting to non-null value 
  // ninfo.set_current_context(classdefn_stmt->get_declaration()->get_firstNondefiningDeclaration()->get_type());
     ninfo.set_current_context(NULL);
     ninfo.set_current_context(classDeclaration->get_type());

  // For Fortran we don't have an inheritance concept, I think.
     ROSE_ASSERT(classdefn_stmt->get_inheritances().empty() == true);

  // DQ (9/28/2004): Turn this back on as the only way to prevent this from being unparsed!
  // DQ (11/22/2003): Control unparsing of the {} part of the definition
     if ( info.SkipBasicBlock() == false )
        {
       // curprint ( string("\n/* Unparsing class definition within unparseClassDefnStmt */ \n";

       // DQ (6/14/2006): Add packing pragma support (explicitly set the packing 
       // alignment to the default, part of packing pragma normalization).
          unsigned int packingAlignment = classdefn_stmt->get_packingAlignment();
          if (packingAlignment != 0)
             {
               curprint ( string("\n#pragma pack(") + StringUtility::numberToString(packingAlignment) + string(")"));
             }

          ninfo.set_isUnsetAccess();
          unp->cur.format(classdefn_stmt, info, FORMAT_BEFORE_BASIC_BLOCK1);
       // curprint ( string("{"));
          unp->cur.format(classdefn_stmt, info, FORMAT_AFTER_BASIC_BLOCK1);

          if (classdefn_stmt->get_isSequence() == true)
             {
            // Get the spacing correct (for fixed vs. free format)
            // unparseStatementNumbersSupport(-1);
               unparseStatementNumbersSupport(NULL,info);
               curprint ("sequence");
               unp->u_sage->curprint_newline();
             }

          if (classdefn_stmt->get_isPrivate() == true)
             {
            // Get the spacing correct (for fixed vs. free format)
            // unparseStatementNumbersSupport(-1);
               unparseStatementNumbersSupport(NULL,info);
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
#if 0
          printf ("classdefn_stmt range %d - %d \n",
               classdefn_stmt->get_startOfConstruct()->get_line(),
               classdefn_stmt->get_endOfConstruct()->get_line());
#endif
          unparseAttachedPreprocessingInfo(classdefn_stmt, info, PreprocessingInfo::inside);
        }

  // DQ (6/13/2007): Set to null before resetting to non-null value 
     ninfo.set_current_context(NULL);
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

     if (s->get_stat_expression() != NULL)
        {
          curprint(", STAT = ");
          unparseExpression(s->get_stat_expression(), info);
        }

     if (s->get_errmsg_expression() != NULL)
        {
          curprint(", ERRMSG = ");
          unparseExpression(s->get_errmsg_expression(), info);
        }

     if (s->get_source_expression() != NULL)
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

     if (s->get_stat_expression() != NULL)
        {
          curprint(", STAT = ");
          unparseExpression(s->get_stat_expression(), info);
        }

     if (s->get_errmsg_expression() != NULL)
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
     // print out "withteam" stmt
     curprint("WITHTEAM ");
    // curprint(withTeamStmt->get_teamId());
    
     SgVarRefExp* teamIdRef = withTeamStmt->get_teamId(); 

     SgInitializedName* teamDecl = teamIdRef->get_symbol()->get_declaration();

     curprint(teamDecl->get_name().str());

     unp->cur.insert_newline(1); 

     // unparser the body
     SgBasicBlock * body = isSgBasicBlock(withTeamStmt->get_body());
     ASSERT_not_null(body);

     unparseBasicBlockStmt(body, info);

    // "end withteam()" stmt
     curprint("END WITHTEAM ");
     //curprint(withTeamStmt->get_teamId());
     curprint(teamDecl->get_name().str());
     unp->cur.insert_newline(1); 
 
   }



// TODO: This code is identical to 'UnparseLanguageIndependentConstructs::curprint'. Factor this!
void
FortranCodeGeneration_locatedNode::curprint(const std::string & str) const
{
#if USE_RICE_FORTRAN_WRAPPING

    if( unp->currentFile != NULL && unp->currentFile->get_Fortran_only() )
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
     bool is_fortran90 =  (unp->currentFile != NULL ) &&
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
        ROSE_ASSERT(false);
        break;
      }
  } // end switch
  //  unp->u_sage->curprint_newline(); // prepare end clauses, they have to be on the same line
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
    ROSE_ASSERT(false);
  }

    // unparse the end directive and name 
  unparseOmpEndDirectivePrefixAndName (stmt, info);

  // unparse the end directive's clause
  unparseOmpEndDirectiveClauses(stmt, info);

}
