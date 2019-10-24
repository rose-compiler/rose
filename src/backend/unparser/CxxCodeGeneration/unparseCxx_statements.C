/* unparse_stmt.C
 * Contains functions that unparse statements
 *
 * FORMATTING WILL BE DONE IN TWO WAYS:
 * 1. using the file_info object to get information from line and column number 
 *    (for original source code)
 * 2. following a specified format that I have specified with indentations of
 *    length TABINDENT (for transformations)
 * 
 * REMEMBER: For types and symbols, we still call the original unparse function 
 * defined in sage since they dont have file_info. For expressions, 
 * Unparse_ExprStmt::unparse is called, and for statements, 
 * Unparse_ExprStmt::unparseStatement is called.
 *
 */
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "unparser.h"

// DQ (8/31/2013):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

// DQ (12/6/2014): Adding support for unparsing from the token stream.
#include "tokenStreamMapping.h"

#define ROSE_TRACK_PROGRESS_OF_ROSE_COMPILING_ROSE 0

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;
using namespace Rose;

#define OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES 0
#define OUTPUT_DEBUGGING_FUNCTION_INTERNALS  0
#define OUTPUT_DEBUGGING_UNPARSE_INFO        0

// Output the class name and function names as we unparse (for debugging)
#define OUTPUT_DEBUGGING_CLASS_NAME    0
#define OUTPUT_DEBUGGING_FUNCTION_NAME 0
#define OUTPUT_HIDDEN_LIST_DATA 0


#define HIGH_FEDELITY_TOKEN_UNPARSING 1

#define OUTPUT_PLACEHOLDER_COMMENTS_FOR_SUPRESSED_TEMPLATE_IR_NODES 0

Unparse_ExprStmt::Unparse_ExprStmt(Unparser* unp, std::string fname)
   : UnparseLanguageIndependentConstructs(unp,fname)
   {
  // Nothing to do here!
   }

Unparse_ExprStmt::~Unparse_ExprStmt()
   {
  // Nothing to do here!
   }

string UnparseLanguageIndependentConstructs::token_sequence_position_name( UnparseLanguageIndependentConstructs::token_sequence_position_enum_type e )
   {
     string s;
     switch(e)
        {
          case e_leading_whitespace_start:  s = "e_leading_whitespace_start";  break;
          case e_leading_whitespace_end:    s = "e_leading_whitespace_end";    break;
          case e_token_subsequence_start:   s = "e_token_subsequence_start";   break;
          case e_token_subsequence_end:     s = "e_token_subsequence_end";     break;
          case e_trailing_whitespace_start: s = "e_trailing_whitespace_start"; break;
          case e_trailing_whitespace_end:   s = "e_trailing_whitespace_end";   break;

       // DQ (12/31/2014): Added to support the middle subsequence of tokens in the SgIfStmt as a special case.
          case e_else_whitespace_start:     s = "e_else_whitespace_start";     break;
          case e_else_whitespace_end:       s = "e_else_whitespace_end";       break;

          default:
             {
               printf ("default reached in switch: value = %d \n",e);
             }
        }

     return s;
   }


// void UnparseLanguageIndependentConstructs::unparseStatementFromTokenStream (
// SgStatement* stmt, 
// UnparseLanguageIndependentConstructs::token_sequence_position_enum_type e_token_sequence_position_start, 
// UnparseLanguageIndependentConstructs::token_sequence_position_enum_type e_token_sequence_position_end)
void UnparseLanguageIndependentConstructs::unparseStatementFromTokenStream (
   SgStatement* stmt,
   UnparseLanguageIndependentConstructs::token_sequence_position_enum_type e_token_sequence_position_start,
   UnparseLanguageIndependentConstructs::token_sequence_position_enum_type e_token_sequence_position_end, 
   SgUnparse_Info & info)
   {
#if 0
      printf ("unparseStatementFromTokenStream(stmt = %p = %s): \n",stmt,stmt->class_name().c_str());
      printf ("   --- stmt: filename = %s \n",stmt->getFilenameString().c_str());
#endif

     if ( SgProject::get_verbose() > 0 )
        {
          string s = "/* Unparse a partial token sequence: 1 stmt: stmt = " + stmt->class_name() + " */ ";
          curprint (s);
        }

  // unparseStatementFromTokenStream(stmt,stmt,e_token_sequence_position_start,e_token_sequence_position_end);
     unparseStatementFromTokenStream(stmt,stmt,e_token_sequence_position_start,e_token_sequence_position_end,info);
   }


// void
// UnparseLanguageIndependentConstructs::unparseStatementFromTokenStream (
// // SgStatement* stmt_1, SgStatement* stmt_2, 
//    SgLocatedNode* stmt_1, SgLocatedNode* stmt_2, 
//    UnparseLanguageIndependentConstructs::token_sequence_position_enum_type e_token_sequence_position_start, 
//    UnparseLanguageIndependentConstructs::token_sequence_position_enum_type e_token_sequence_position_end,
//    bool unparseOnlyWhitespace )
void
UnparseLanguageIndependentConstructs::unparseStatementFromTokenStream (
   SgLocatedNode* stmt_1, SgLocatedNode* stmt_2,
   UnparseLanguageIndependentConstructs::token_sequence_position_enum_type e_token_sequence_position_start, 
   UnparseLanguageIndependentConstructs::token_sequence_position_enum_type e_token_sequence_position_end,
   SgUnparse_Info & info,
   bool unparseOnlyWhitespace )
   {
  // unparseStatementFromTokenStream (stmt, e_leading_whitespace_start, e_token_subsequence_start);
  // Check for the leading token stream for this statement.  Unparse it if the previous statement was unparsed as a token stream.

#define DEBUG_TOKEN_STREAM_UNPARSING 0

#if DEBUG_TOKEN_STREAM_UNPARSING
     printf ("unparseStatementFromTokenStream(stmt_1=%p=%s,stmt_2=%p=%s): \n",stmt_1,stmt_1->class_name().c_str(),stmt_2,stmt_2->class_name().c_str());
     printf ("   --- e_token_sequence_position_start = %d = %s \n",e_token_sequence_position_start,token_sequence_position_name(e_token_sequence_position_start).c_str());
     printf ("   --- e_token_sequence_position_end   = %d = %s \n",e_token_sequence_position_end,token_sequence_position_name(e_token_sequence_position_end).c_str());
     printf ("   --- unparseOnlyWhitespace = %s \n",unparseOnlyWhitespace ? "true" : "false");

     SgDeclarationStatement* declarationStatement_1 = isSgDeclarationStatement(stmt_1);
     if (declarationStatement_1 != NULL)
        {
          printf ("   --- declarationStatement_1->get_firstNondefiningDeclaration() = %p \n",declarationStatement_1->get_firstNondefiningDeclaration());
          printf ("   --- declarationStatement_1->get_definingDeclaration()         = %p \n",declarationStatement_1->get_definingDeclaration());
        }

     SgDeclarationStatement* declarationStatement_2 = isSgDeclarationStatement(stmt_2);
     if (declarationStatement_2 != NULL)
        {
          printf ("   --- declarationStatement_2->get_firstNondefiningDeclaration() = %p \n",declarationStatement_2->get_firstNondefiningDeclaration());
          printf ("   --- declarationStatement_2->get_definingDeclaration()         = %p \n",declarationStatement_2->get_definingDeclaration());
        }

     printf ("   --- stmt_1->get_file_info()->get_filenameString() = %s \n",stmt_1->get_file_info()->get_filenameString().c_str());
     printf ("   --- stmt_2->get_file_info()->get_filenameString() = %s \n",stmt_2->get_file_info()->get_filenameString().c_str());
#endif

     if ( SgProject::get_verbose() > 0 )
        {
       // Avoid redundant output from unparseStatementFromTokenStream() taking a single SgStatement.
          if (stmt_1 != stmt_2)
             {
               string s = "/* Unparse a partial token sequence: 2 stmt: stmt_1 = " + stmt_1->class_name() + " stmt_2 = " + stmt_2->class_name() +  " */ ";
               curprint (s);
             }
        }

  // DQ (10/27/2018): This is the wrong source file when we are unparsing header files (which share the global scope 
  // but for which this function will traverse paranter pointest to find the translation units SgSourceFile (which is 
  // that of the input source code and not the associated header file).
  // SgSourceFile* sourceFile = isSgSourceFile(SageInterface::getEnclosingFileNode(stmt_1));
     SgSourceFile* sourceFile = info.get_current_source_file();
     ROSE_ASSERT(sourceFile != NULL);

#if 0
     printf ("In unparseStatementFromTokenStream(): sourceFile filename = %s \n",sourceFile->getFileName().c_str());
     printf ("In unparseStatementFromTokenStream(): sourceFile->get_tokenSubsequenceMap().size()         = %zu \n",sourceFile->get_tokenSubsequenceMap().size());
#endif

  // DQ (12/26/2018): Moved this to the outer function scopw so that we can assert that any element index is less then the tokenVectorSize.
     SgTokenPtrList & tokenVector = sourceFile->get_token_list();
     int tokenVectorSize = tokenVector.size();

  // Note: that there is a single global map that is accessed from the get_tokenSubsequenceMap() function, not one per SgSourceFile.
  // I am not clear if this is an issue for the header file unparsing using the token steam, I think that since the map is based on 
  // keys that are IR node pointers, they are all unique.
  // DQ (9/28/2018): There is not one map per file to simplify debugging (and maybe fix some subtle problems).
     std::map<SgNode*,TokenStreamSequenceToNodeMapping*> & tokenStreamSequenceMap = sourceFile->get_tokenSubsequenceMap();

#if 0
     printf ("In unparseStatementFromTokenStream(): tokenStreamSequenceMap.size() = %zu \n",tokenStreamSequenceMap.size());
#endif

  // TokenStreamSequenceToNodeMapping* tokenSubsequence_1 = tokenStreamSequenceMap[stmt_1];
  // ROSE_ASSERT(tokenSubsequence_1 != NULL);
     TokenStreamSequenceToNodeMapping* tokenSubsequence_1 = NULL;

  // DQ (10/4/2018): This is a essential test to avoid pointers to default constructed objects from appearing accedentally in the STL map.
     if (tokenStreamSequenceMap.find(stmt_1) == tokenStreamSequenceMap.end())
        {
#if 0
       // DQ (11/4/2018): This is not an error when using the unparse to header files with the token-based unparsing.
          printf ("WARNING: This IR node is not in the tokenStreamSequenceMap! \n");
#endif
        }
       else
        {
          tokenSubsequence_1 = tokenStreamSequenceMap[stmt_1];
        }

  // DQ (9/25/2018): I think this is an issue for new IR nodes added to the AST (such a SgIncludeDirective IR nodes within the header file unparsing support).
  // However there is a current bug where the global scope for include files are missing.
     if (tokenSubsequence_1 == NULL)
        {
       // DQ (11/4/2018): This is not an error when using the unparse to header files with the token-based unparsing.
#if 0
          printf ("Warning: In unparseStatementFromTokenStream(): tokenSubsequence_1 == NULL: stmt_1 = %p = %s \n",stmt_1,stmt_1->class_name().c_str());
          printf ("   --- stmt_1 = %p = %s \n",stmt_1,stmt_1->class_name().c_str());
          printf ("   --- stmt_2 = %p = %s \n",stmt_2,stmt_2->class_name().c_str());
       // printf ("   --- tokenSubsequence_2 = %p \n",tokenSubsequence_2);

          SgGlobal* global_stmt_1 = isSgGlobal(stmt_1);
          if (global_stmt_1 != NULL)
             {
               printf ("Found a SgGlobal that is not in the tokenStreamSequenceMap: could be associated with the header file unparsing \n");
               SgSourceFile* tmp_sourceFile = isSgSourceFile(global_stmt_1->get_parent());
               ROSE_ASSERT(tmp_sourceFile != NULL);
               printf ("global scope is associated with source file: filename = %s \n",tmp_sourceFile->getFileName().c_str());
             }

       // Output the tokenStreamSequenceMap:
#if 0
          printf ("In unparseStatementFromTokenStream(): Output TokenStreamSequenceToNodeMapping \n");
#endif
          std::map<SgNode*,TokenStreamSequenceToNodeMapping*>::iterator i = tokenStreamSequenceMap.begin();
          while (i != tokenStreamSequenceMap.end())
             {
#if 0
               printf ("i->first = %p = %s: \n",i->first,i->first->class_name().c_str());
#endif
            // DQ (9/28/2018): Adding assertion.
               ROSE_ASSERT(i->second->node != NULL);
#if 0
               i->second->display("unparseStatementFromTokenStream(stmt,stmt): token sequence");
#endif

#if 0
            // DQ (12/26/2018): Declaration moved to location above, but in this function.
            // SgTokenPtrList & tokenVector = sourceFile->get_token_list();
            // int tokenVectorSize = tokenVector.size();

               int start = i->second->token_subsequence_start;
               int end   = i->second->token_subsequence_end;

               printf ("tokenVectorSize = %d \n",tokenVectorSize);
               printf ("start = %d \n",start);
               printf ("end   = %d \n",end);

               for (int j = start; j < end; j++)
                  {
                    printf ("tokenVector[%d] = %s \n",j,tokenVector[j]->get_lexeme_string().c_str());
                  }
#endif
               i++;
             }
#endif
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

  // DQ (10/4/2018): Move this to be initialized later so that we can understnad how a bug is happening
  // between where the tokenStreamSequenceMap is accessed and where it is tested below.
  // TokenStreamSequenceToNodeMapping* tokenSubsequence_2 = tokenStreamSequenceMap[stmt_2];
     TokenStreamSequenceToNodeMapping* tokenSubsequence_2 = NULL;

  // tokenSubsequence_2 = tokenStreamSequenceMap[stmt_2];
  // DQ (10/4/2018): This is a essential test to avoid pointers to default constructed objects from appearing accedentally in the STL map.
     if (tokenStreamSequenceMap.find(stmt_2) == tokenStreamSequenceMap.end())
        {
#if 0
       // DQ (11/4/2018): This is not an error when using the unparse to header files with the token-based unparsing.
          printf ("WARNING: This IR node is not in the tokenStreamSequenceMap! \n");
#endif
        }
       else
        {
       // DQ (10/26/2018): Bug fix: I think this was a cut and paste error.
       // tokenSubsequence_2 = tokenStreamSequenceMap[stmt_1];
          tokenSubsequence_2 = tokenStreamSequenceMap[stmt_2];
        }

  // DQ (9/25/2018): I think this is an issue for new IR nodes added to the AST (such a SgIncludeDirective IR nodes within the header file unparsing support.
     if (tokenSubsequence_2 == NULL)
        {
#if 0
       // DQ (11/4/2018): This is not an error when using the unparse to header files with the token-based unparsing.
          printf ("WARNING: In unparseStatementFromTokenStream(): tokenSubsequence_2 == NULL: stmt_2 = %p = %s \n",stmt_2,stmt_2->class_name().c_str());
#endif
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

#if 0
  // DQ (11/12/2018): Debugging test9 in UnparseHeaders test.
     printf ("tokenSubsequence_1 = %p tokenSubsequence_2 = %p \n",tokenSubsequence_1,tokenSubsequence_2);
#endif

  // DQ (12/10/2014): The mapping for stmt_2 might not exist, e.g. if it was added as part of a transformation.
  // in this case then there is no associated token stream to output.
  // ROSE_ASSERT(tokenSubsequence_2 != NULL);
     if (tokenSubsequence_1 != NULL && tokenSubsequence_2 != NULL)
        {
          ROSE_ASSERT(tokenSubsequence_1->token_subsequence_start != -1);
          ROSE_ASSERT(tokenSubsequence_2->token_subsequence_start != -1);

       // This is correct for the SgFunctionDefinition IR node.
       // int start = functionDefinition_tokenSubsequence->leading_whitespace_start;
       // int end   = functionDefinition_tokenSubsequence->token_subsequence_start;

          int start = 0;
          int end   = 0;

          bool start_reset_because_requestion_position_was_not_defined = false;

          switch (e_token_sequence_position_start)
             {
               case e_leading_whitespace_start:
                    start = tokenSubsequence_1->leading_whitespace_start;

                 // DQ (12/13/2014): Note that white space is not always available.
                    if (start == -1)
                       {
                         start = tokenSubsequence_1->token_subsequence_start;
                         start_reset_because_requestion_position_was_not_defined = true;
                       }
                    ROSE_ASSERT(start >= 0);

                 // DQ (12/26/2018): We have to make sure that we stay in bounds of the number of tokens in the loken list.
                    ROSE_ASSERT(start < tokenVectorSize);
                    break;

               case e_leading_whitespace_end:
                    start = tokenSubsequence_1->leading_whitespace_end;
                    ROSE_ASSERT(start >= 0);

                 // DQ (12/26/2018): We have to make sure that we stay in bounds of the number of tokens in the loken list.
                    ROSE_ASSERT(start < tokenVectorSize);
                    break;

               case e_token_subsequence_start:
                    start = tokenSubsequence_1->token_subsequence_start;
                    ROSE_ASSERT(start >= 0);

                 // DQ (12/26/2018): We have to make sure that we stay in bounds of the number of tokens in the loken list.
                    ROSE_ASSERT(start < tokenVectorSize);
                    break;

               case e_token_subsequence_end:
                    start = tokenSubsequence_1->token_subsequence_end;
                    ROSE_ASSERT(start >= 0);

                 // DQ (12/26/2018): We have to make sure that we stay in bounds of the number of tokens in the loken list.
                    ROSE_ASSERT(start < tokenVectorSize);
                    break;

               case e_trailing_whitespace_start:
                    start = tokenSubsequence_1->trailing_whitespace_start;
#if 0
                    printf ("Case e_trailing_whitespace_start: tokenVectorSize = %d start = %d \n",tokenVectorSize,start);
#endif
                 // DQ (12/10/2014): Note that white space is not always available.
                    if (start == -1)
                       {
                         start = tokenSubsequence_1->token_subsequence_end + 1;
                         start_reset_because_requestion_position_was_not_defined = true;
#if 0
                         printf ("Case e_trailing_whitespace_start: start = -1: adjusting: set start_reset_because_requestion_position_was_not_defined = true: start = %d \n",start);
#endif
                       }
                    ROSE_ASSERT(start >= 0);

                 // DQ (12/26/2018): If this is out of bounds then we have to set it to not available.
                    if (start >= tokenVectorSize)
                       {
#if 0
                         printf ("Case e_trailing_whitespace_start: start >= tokenVectorSize: resetting to mark as unavailable: start = %d \n",start);
#endif
                      // start = -1;
                         start = tokenVectorSize - 1;

                      // Unclear if this should also be set.
                         start_reset_because_requestion_position_was_not_defined = true;
                       }

                 // DQ (12/26/2018): We have to make sure that we stay in bounds of the number of tokens in the loken list.
                    ROSE_ASSERT(start < tokenVectorSize);
                    break;

               case e_trailing_whitespace_end:
                    start = tokenSubsequence_1->trailing_whitespace_end;
#if 0
                    printf ("Case e_trailing_whitespace_end: start = %d \n",start);
#endif
                 // DQ (1/2/2015): Note that white space is not always available.
                    if (start == -1)
                       {
                      // DQ (1/10/2014): Make sure that we don't use data that is unavailable.
                         ROSE_ASSERT(tokenSubsequence_1->token_subsequence_end != -1);
                         start = tokenSubsequence_1->token_subsequence_end + 1;
                         start_reset_because_requestion_position_was_not_defined = true;
#if 0
                         printf ("Case e_trailing_whitespace_end: start == -1: adjusting: set start_reset_because_requestion_position_was_not_defined = true: start = %d \n",start);
#endif
                       }
                    ROSE_ASSERT(start >= 0);

                 // DQ (12/26/2018): We have to make sure that we stay in bounds of the number of tokens in the loken list.
                    ROSE_ASSERT(start < tokenVectorSize);
                    break;

               default:
                  {
                    printf ("Default reached in unparseStatementFromTokenStream(): e_token_sequence_position_start = %d \n",e_token_sequence_position_start);
                    ROSE_ASSERT(false);
                  }
             }

          if (stmt_1 == stmt_2 && e_token_sequence_position_start == e_token_sequence_position_end)
             {
            // DQ (1/24/2015): If the token sequence was not defined then we don't want to output any tokens.
            // This fixes cases where we have multiple blocks closed using "}}" (see test2015_96.C).
            // This should trigger a single token to be output.
            // end = start + 1;
#if 0
               printf ("(stmt_1 == stmt_2 && e_token_sequence_position_start == e_token_sequence_position_end) == true \n");
               printf ("   --- start_reset_because_requestion_position_was_not_defined = %s \n",start_reset_because_requestion_position_was_not_defined ? "true" : "false");
#endif
               if (start_reset_because_requestion_position_was_not_defined == false)
                  {
                    end = start + 1;

                 // DQ (12/27/2018): avoid out of range access.
                    if (end == tokenVectorSize)
                       {
                      // DQ (12/27/2018): This case is special to the last token in the file that is unparsed with its own call to unparseStatementFromTokenStream().
#if 0
                         printf ("NOTE: ALLOW out of range value: make sure there is no access: start = %d end = %d tokenVectorSize = %d (reset end to start) \n",start,end,tokenVectorSize);
#endif
                       }
                      else
                       {
                         ROSE_ASSERT(end < tokenVectorSize);
                       }
                  }
                 else
                  {
                 // This is a value that will prevent any output of tokens.
                    end = start;

                    ROSE_ASSERT(end < tokenVectorSize);
                  }

            // DQ (12/26/2018): We have to make sure that we stay in bounds of the number of tokens in the loken list.
               if (end >= tokenVectorSize)
                  {
                 // DQ (12/27/2018): This is allowed for the last token of the file (which is a special case).
                 // printf ("Error: start = %d end = %d tokenVectorSize = %d \n",start,end,tokenVectorSize);
                  }

            // DQ (12/27/2018): Modified assertion to test all but the case of end == tokenVectorSize (above).
            // ROSE_ASSERT(end < tokenVectorSize);
             }
            else
             {
#if 0
               printf ("(stmt_1 == stmt_2 && e_token_sequence_position_start == e_token_sequence_position_end) == false \n");
#endif
               switch (e_token_sequence_position_end)
                  {
#if 0
                    case e_leading_whitespace_start:  end = tokenSubsequence_2->leading_whitespace_start;  break;
                    case e_leading_whitespace_end:    end = tokenSubsequence_2->leading_whitespace_end;    break;
                    case e_token_subsequence_start:   end = tokenSubsequence_2->token_subsequence_start;   break;
                    case e_token_subsequence_end:     end = tokenSubsequence_2->token_subsequence_end;     break;
                    case e_trailing_whitespace_start: end = tokenSubsequence_2->trailing_whitespace_start; break;
                    case e_trailing_whitespace_end:   end = tokenSubsequence_2->trailing_whitespace_end;   break;
#else
                 // DQ (12/28/2014): We need to accound for where the leading and trailing token streams might not be available.
                    case e_leading_whitespace_start:
                         end = tokenSubsequence_2->leading_whitespace_start;

                      // DQ (12/28/2014): Note that white space is not always available.
                         if (end == -1)
                            {
                              end = tokenSubsequence_2->token_subsequence_start;
                            }
                         ROSE_ASSERT(end >= 0);

                      // DQ (12/26/2018): We have to make sure that we stay in bounds of the number of tokens in the loken list.
                         ROSE_ASSERT(end < tokenVectorSize);
                         break;

                    case e_leading_whitespace_end:
                         end = tokenSubsequence_2->leading_whitespace_end;

                      // DQ (12/30/2014): Note that white space is not always available.
                         if (end == -1)
                            {
                              end = tokenSubsequence_2->token_subsequence_start;
                            }
                         ROSE_ASSERT(end >= 0);

                      // DQ (12/26/2018): We have to make sure that we stay in bounds of the number of tokens in the loken list.
                         ROSE_ASSERT(end < tokenVectorSize);
                         break;

                    case e_token_subsequence_start:
                         end = tokenSubsequence_2->token_subsequence_start;
                         ROSE_ASSERT(end >= 0);

                      // DQ (12/26/2018): We have to make sure that we stay in bounds of the number of tokens in the loken list.
                         ROSE_ASSERT(end < tokenVectorSize);
                         break;

                    case e_token_subsequence_end:
                         end = tokenSubsequence_2->token_subsequence_end;
                         ROSE_ASSERT(end >= 0);

                      // DQ (12/26/2018): We have to make sure that we stay in bounds of the number of tokens in the loken list.
                         ROSE_ASSERT(end < tokenVectorSize);
                         break;

                    case e_trailing_whitespace_start:
                         end = tokenSubsequence_2->trailing_whitespace_start;

                      // DQ (12/28/2014): Note that white space is not always available.
                         if (end == -1)
                            {
                              end = tokenSubsequence_2->token_subsequence_end + 1;
                            }
                         ROSE_ASSERT(end >= 0);

                      // DQ (12/26/2018): We have to make sure that we stay in bounds of the number of tokens in the loken list.
                         ROSE_ASSERT(end < tokenVectorSize);
                         break;

                    case e_trailing_whitespace_end:
                         end = tokenSubsequence_2->trailing_whitespace_end;

                      // DQ (1/2/2015): Note that white space is not always available.
                         if (end == -1)
                            {
                           // DQ (1/10/2014): Make sure that we don't use data that is unavailable.
                              ROSE_ASSERT(tokenSubsequence_2->token_subsequence_end != -1);
                              end = tokenSubsequence_2->token_subsequence_end + 1;
                            }
                         ROSE_ASSERT(end >= 0);

                      // DQ (12/26/2018): We have to make sure that we stay in bounds of the number of tokens in the loken list.
                         ROSE_ASSERT(end < tokenVectorSize);
                         break;
#endif
                    default:
                       {
                         printf ("Default reached in unparseStatementFromTokenStream(): e_token_sequence_position_end = %d \n",e_token_sequence_position_end);
                         ROSE_ASSERT(false);
                       }
                  }
             }

#if DEBUG_TOKEN_STREAM_UNPARSING
          printf ("unparseStatementFromTokenStream(): Iterate from start = %d to end = %d \n",start,end);
#endif
          ROSE_ASSERT(start >= 0);

       // DQ (12/26/2018): This is now declared in the function body scope.
       // SgTokenPtrList & tokenVector = sourceFile->get_token_list();
       // int tokenVectorSize = tokenVector.size();
#if 0
          printf ("tokenVectorSize = %d start = %d end = %d \n",tokenVectorSize,start,end);
#endif

       // DQ (11/13/2018): This assertion will fail where a class declaration and the end of the file has no white-space 
       // (see test9 of UnparseHeaders_tests demo_error_Simple.h for an example).  This will have to be fixed later.
       // DQ (11/12/2018): Try to comment out this assertion (leave the output message while we evaluate this).
       // DQ (1/10/2015): I think we can assert this now that we no longer call this function to output the trailing whitespace of the SgGlobalScope.
          if ( !(start < tokenVectorSize && end <= tokenVectorSize) )
             {
               printf ("Error: tokenVectorSize = %d start = %d end = %d \n",tokenVectorSize,start,end);
             }
          ROSE_ASSERT(start < tokenVectorSize && end <= tokenVectorSize);

       // DQ (12/27/2018): It appears that we allow the "end" to be equal to "tokenVectorSize".  Could this be an error?
          if (start < tokenVectorSize && end <= tokenVectorSize)
             {
            // We don't want to unparse the token at the end.
#if 0
               printf ("unparseOnlyWhitespace = %s \n",unparseOnlyWhitespace ? "true" : "false");
#endif
            // DQ (11/4/2015): Adding support to optionally only unparse the associated whitespace with any region of a statement.
            // This is used when we want to unparse the leading whitespace of a statement as part of a transformation, yet we need 
            // to ONLY unparse the spaces and CR's.
               if (unparseOnlyWhitespace == true)
                  {
                 // If this is whitespace with embedded comments (which we consider to be in the leading a trailing whitespace for each statement),
                 // then we only want to use the non-whitespace that is at the end of the leading whitespace for the statement.

                    SgTokenPtrList whitespaceTokens;
#if 0
                    printf ("(unparseOnlyWhitespace == true): end = %d \n",end);
#endif
                 // We don't want to unparse the token at the end.
                    int j = end-1;

                    bool firstCarriageReturn = false;
                    bool still_is_whitespace = true;
                    while ( (j >= start) && (still_is_whitespace == true) )
                       {
                      // DQ (1/10/2014): Make sure that we don't use data that is unavailable.
                         ROSE_ASSERT(j < (int)tokenVector.size());

#if DEBUG_TOKEN_STREAM_UNPARSING
                         printf ("possible whitespace: start = %d j = %d \n",start,j);
#endif
#if DEBUG_TOKEN_STREAM_UNPARSING
                         printf ("iterate j=end-1 to j >= start: unparseStatementFromTokenStream: Output tokenVector[j=%d]->get_lexeme_string() = %s \n",j,tokenVector[j]->get_lexeme_string().c_str());
#endif
                         if (tokenVector[j]->get_classification_code() == ROSE_token_ids::C_CXX_WHITESPACE)
                            {
                           // outputString << tokenVector[j]->get_lexeme_string();
                              whitespaceTokens.push_back(tokenVector[j]);
                            }
                           else
                            {
                              still_is_whitespace = false;
#if 0
                              printf ("unparseOnlyWhitespace == true: Skipping output of tokenVector[j=%d]->get_lexeme_string() = %s \n",j,tokenVector[j]->get_lexeme_string().c_str());
#endif
                            }

                      // DQ (11/20/2015): Note that to avoid extra lines in the output we only want to the whitespace up to the first CR.
                         firstCarriageReturn = tokenVector[j]->isCarriageReturn();
                         if (firstCarriageReturn == true)
                            {
                              still_is_whitespace = false;
#if 0
                              printf ("unparseOnlyWhitespace == true: j = %d set still_is_whitespace = false (seenCarriageReturn == true) \n",j);
#endif
                            }

                         j--;
                       }

                 // Output the whitespace tokens in the reverse order.
#if DEBUG_TOKEN_STREAM_UNPARSING
                    printf ("whitespaceTokens.size() = %zu \n",whitespaceTokens.size());
#endif
                    SgTokenPtrList::reverse_iterator m = whitespaceTokens.rbegin();
                    while (m != whitespaceTokens.rend())
                       {
                      // Print token (whitespace).
#if HIGH_FEDELITY_TOKEN_UNPARSING
                         *(unp->get_output_stream().output_stream()) << (*m)->get_lexeme_string();
#else
                      // Note that this will interprete line endings which is not going to provide the precise token based output.
                         curprint((*m)->get_lexeme_string());
#endif
                         m++;
                       }
                  }
                 else
                  {
                 // DQ (12/27/2018): Now that we enforce uniformally that the end is in bounds of the token vector, we DO want to unparse the end.
                 // It seems that we can't handle this issue this way.
                 // We don't want to unparse the token at the end.
                 // for (int j = start; j < end; j++)
                    for (int j = start; j < end; j++)
                       {
                      // DQ (1/10/2014): Make sure that we don't use data that is unavailable.
                          ROSE_ASSERT(j < (int)tokenVector.size());

#if DEBUG_TOKEN_STREAM_UNPARSING
                         printf ("iterate j=start to j < end: unparseStatementFromTokenStream: Output tokenVector[j=%d]->get_lexeme_string() = %s \n",j,tokenVector[j]->get_lexeme_string().c_str());
#endif
#if HIGH_FEDELITY_TOKEN_UNPARSING
                         *(unp->get_output_stream().output_stream()) << tokenVector[j]->get_lexeme_string();
#else
                      // Note that this will interprete line endings which is not going to provide the precise token based output.
                         curprint(tokenVector[j]->get_lexeme_string());
#endif
                       }
                  }
             }
            else
             {
            // DQ (1/10/2015): The case of SgGlobalScope does not permit the output of a trailing whitespce (since it is not defined).
               if (isSgGlobal(stmt_1) == NULL || isSgGlobal(stmt_2) == NULL)
                  {
                    printf ("ERROR: unparseStatementFromTokenStream(): skipped output of token range: start = %d end = %d tokenVectorSize = %d \n",start,end,tokenVectorSize);
                    ROSE_ASSERT(false);
                  }
             }
        }
       else
        {
       // DQ (12/30/2014): This will likely cause an error since some subsequence of the token stream will not be unparsed.
#if 0
          printf ("ERROR: unparseStatementFromTokenStream(stmt_1=%p=%s,stmt_2=%p=%s): \n",stmt_1,stmt_1->class_name().c_str(),stmt_2,stmt_2->class_name().c_str());
          printf ("   --- e_token_sequence_position_start = %d = %s \n",e_token_sequence_position_start,token_sequence_position_name(e_token_sequence_position_start).c_str());
          printf ("   --- e_token_sequence_position_end   = %d = %s \n",e_token_sequence_position_end,token_sequence_position_name(e_token_sequence_position_end).c_str());
#endif
#if 0
       // DQ (11/4/2018): This is not an error when using the unparse to header files with the token-based unparsing.
          printf ("ERROR: Token subsequence position unavailable: tokenSubsequence_1 = %p tokenSubsequence_2 = %p \n",tokenSubsequence_1,tokenSubsequence_2);
       // ROSE_ASSERT(false);
#endif
        }
   }


//-----------------------------------------------------------------------------------
//  void Unparse_MOD_SAGE::unparseOneElemConInit
//
//  This function is used to only unparse constructor initializers with one 
//  element in their argument list. Because there is only one argument in the
//  list, the parenthesis around the arguments are not unparsed (to control
//  the excessive printing of parenthesis). 
//-----------------------------------------------------------------------------------
void 
Unparse_ExprStmt::unparseOneElemConInit(SgConstructorInitializer* con_init, SgUnparse_Info& info)
   {
  // DQ (6/2/2011): This code is largely the same as that in unparseConInit() in unparseCxx_expressions.C
  // So there is an opportunity to refactor this code.

     ROSE_ASSERT(con_init != NULL);

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES
     printf ("\n\nInside of Unparse_MOD_SAGE::unparseOneElemConInit (%p) \n",con_init);
     curprint( "\n /* Inside of Unparse_MOD_SAGE::unparseOneElemConInit */ \n");
#endif

#if 0
     printf ("\n\nInside of Unparse_ExprStmt::unparseOneElemConInit (%p) \n",con_init);
#endif
#if 0
     curprint("\n /* Inside of Unparse_ExprStmt::unparseOneElemConInit */ \n");
#endif

  // taken from unparseConInit
     SgUnparse_Info newinfo(info);

#if 0
     printf ("con_init->get_need_name()      = %s \n",(con_init->get_need_name() == true) ? "true" : "false");
     printf ("con_init->get_need_qualifier() = %s \n",(con_init->get_need_qualifier() == true) ? "true" : "false");
     printf ("con_init->get_declaration()    = %p \n",con_init->get_declaration());

  // curprint(string("\n /* con_init->get_need_name()        = ") << string((con_init->get_need_name()        ? "true" : "false")) << string(" */ \n"));
  // curprint("\n /* con_init->get_is_explicit_cast() = " << (con_init->get_is_explicit_cast() ? "true" : "false") << " */ \n");
#endif

  // DQ (3/24/2005): added checking for is_explicit_cast flag
  // if (con_init->get_need_name() == true)
     if ((con_init->get_need_name() == true) && (con_init->get_is_explicit_cast() == true) )
        {
          SgName name;
#if 0
          printf ("In unparseOneElemConInit(): con_init->get_declaration() = %p \n",con_init->get_declaration());
#endif
          if (con_init->get_declaration())
             {
            // DQ (11/12/2004)  Use the qualified name always (since get_need_qualifier() does
            //                  not appear to get set correctly (perhaps within EDG as before)
#if 0
               if(con_init->get_need_qualifier()) 
                    nm = con_init->get_declaration()->get_qualified_name();
                 else
                    nm = con_init->get_declaration()->get_name();
#else
#if 0
               nm = con_init->get_declaration()->get_qualified_name();
#else
            // DQ (6/1/2011): Newest refactored support for name qualification.
            // nm = con_init->get_declaration()->get_qualified_name();
               SgName nameQualifier = con_init->get_qualified_name_prefix();

            // DQ (2/8/2014): Adding the trimming of the constructor name where it is required for the
            // GNU g++ version 4.5 and greater compilers (used in the backend compilation within ROSE).
               bool skipOutputOfFunctionName = false;
               nameQualifier = trimOutputOfFunctionNameForGNU_4_5_VersionAndLater(nameQualifier,skipOutputOfFunctionName);

            // name = nameQualifier + con_init->get_declaration()->get_name();
               if (skipOutputOfFunctionName == false)
                  {
                 // Case for g++ version less than version 4.5
                    name = nameQualifier + con_init->get_declaration()->get_name();
                  }
                 else
                  {
                 // Case for g++ versions equal to or greater than version 4.5
                    name = nameQualifier;
                  }
#if 0
               printf ("In unparseOneElemConInit(): name = %s \n",name.str());
#endif
#if 0
               curprint( "\n /* In unparseOneElemConInit(): (con_init->get_declaration() != NULL): unp->u_sage->printConstructorName(con_init) == true */ \n");
#endif
            // DQ (8/19/2013): I am not sure that this will include name qualification on possible template arguments.
            // We need an example of this.
               if ( unp->u_sage->printConstructorName(con_init))
                  {
                    curprint( name.str());
                  }
#endif
#endif
             }
            else
             {
//             ROSE_ASSERT (con_init->get_class_decl() != NULL);
#if 0
               printf ("In unparseOneElemConInit(): con_init->get_class_decl() = %p \n",con_init->get_class_decl());
#endif
               if(con_init->get_class_decl())
                  {
                 // DQ (11/12/2004)  Use the qualified name always (since get_need_qualifier() does
                 //                  not appear to get set correctly (perhaps within EDG as before)
#if 0
                    if(con_init->get_need_qualifier()) 
                         nm = con_init->get_class_decl()->get_qualified_name();
                      else
                         nm = con_init->get_class_decl()->get_name();
#else
#if 0
                    nm = con_init->get_class_decl()->get_qualified_name();
#else
                 // DQ (6/1/2011): Newest refactored support for name qualification.
                 // nm = con_init->get_class_decl()->get_qualified_name();
                    SgName nameQualifier = con_init->get_qualified_name_prefix();
                 // nm = nameQualifier + con_init->get_class_decl()->get_name();

#if 0
                    printf ("In unparseOneElemConInit(): nameQualifier = %s \n",nameQualifier.str());
#endif

                 // DQ (8/19/2013): We need to unparse the type using any possible qualification on the type name (e.g. name qualification on template arguments).
                    if ( unp->u_sage->printConstructorName(con_init))
                       {
#if 0
                         printf ("In unparseOneElemConInit(): Unparse the nameQualifier = %s \n",nameQualifier.str());
#endif
#if 0
                         curprint( "\n /* In unparseOneElemConInit(): (con_init->get_declaration() == NULL): unp->u_sage->printConstructorName(con_init) == true */ \n");
#endif
                         curprint(nameQualifier.str());
#if 0
                         printf ("In unparseOneElemConInit(): set_reference_node_for_qualification(con_init = %p) \n",con_init);
#endif
                         newinfo.set_reference_node_for_qualification(con_init);

                      // DQ (8/19/2013): This has no effect if the type string is taken from the type name map and so the "class" needs
                      // to be eliminated in the generation of the intial string as part of the generation of the name qualification.
                      // printf ("In unparseOneElemConInit(): calling set_SkipClassSpecifier() \n");
                      // newinfo.set_SkipClassSpecifier();
#if 0
                         printf ("In unparseOneElemConInit(): Unparse the type = %p = %s \n",con_init->get_type(),con_init->get_type()->class_name().c_str());
#endif
                         unp->u_type->unparseType(con_init->get_type(),newinfo);
#if 0
                         printf ("DONE: In unparseOneElemConInit(): unparseType() \n");
#endif
                       }
#endif
#endif
                  }
             }

#if 0
          printf ("In unparseOneElemConInit(): nm = %s \n",name.str());
          printf ("In unparseOneElemConInit(): get_qualified_name_prefix_for_referenced_type() = %s \n",con_init->get_qualified_name_prefix_for_referenced_type().str());
          printf ("In unparseOneElemConInit(): get_qualified_name_prefix() = %s \n",con_init->get_qualified_name_prefix().str());
#endif
#if 0
       // DQ (8/19/2013): Removed the refactoring of this code from the two branches above so that we could tailor the 
       // name qualification to include the handling of template arguments in the type that require qualification.
          if ( unp->u_sage->printConstructorName(con_init))
             {
               curprint( name.str());
             }
#endif
        }

#if 0
     curprint( "\n /* Done with name output in Unparse_MOD_SAGE::unparseOneElemConInit */ \n");
#endif

  // DQ (1/18/2019): Test the current SgConstructorInitializer.
     bool this_constructor_initializer_is_using_Cxx11_initializer_list = isAssociatedWithCxx11_initializationList(con_init,info);

#if 0
     printf ("In unparseOneElemConInit(): this_constructor_initializer_is_using_Cxx11_initializer_list = %s \n",(this_constructor_initializer_is_using_Cxx11_initializer_list == true) ? "true" : "false");
     printf ("In unparseOneElemConInit(): con_init->get_need_name()                                    = %s \n",(con_init->get_need_name() == true) ? "true" : "false");
     printf ("In unparseOneElemConInit(): unp->u_sage->printConstructorName(con_init)                  = %s \n",(unp->u_sage->printConstructorName(con_init) == true) ? "true" : "false");
#endif

  // DQ (1/30/2019): Refactored code from below.
     bool outputParenthisis = false;
  // if (con_init->get_need_name() && unp->u_sage->printConstructorName(con_init)) 
     if ((con_init->get_need_name() == true) && (con_init->get_is_explicit_cast() == true) && unp->u_sage->printConstructorName(con_init) == true)
       {
          outputParenthisis = true;
       }

  // DQ (1/30/2019): Copy of code from the unparse function for the constructor initializer.
  // DQ (1/18/2019): this indicates a silent mode and no output should be generated from where
  // the constructor initializer is associated with the C++11 initialization_list class.
     if (this_constructor_initializer_is_using_Cxx11_initializer_list == true)
        {
#if 0
          printf ("In unparseOneElemConInit(): Suppress output for constructor initializer from the C++11 initializer_list class: this_constructor_initializer_is_using_Cxx11_initializer_list == true: reset outputParenthisis = false \n");
#endif
          outputParenthisis = false;
        }

#if 0
     printf ("@@@@@@@@@@@@@@@@@@@@ In unparseOneElemConInit(): outputParenthisis = %s \n",(outputParenthisis == true) ? "true" : "false");
#endif

  // taken from unparseExprList
  // check whether the constructor name was printed. If so, we need to surround
  // the arguments of the constructor with parenthesis.
  // printf ("printConstructorName() = %s \n",(printConstructorName(con_init) == true) ? "true" : "false");
  // if (con_init->get_need_name() && unp->u_sage->printConstructorName(con_init)) 
     if (outputParenthisis == true)
        {
#if 0
          printf ("In unparseOneElemConInit(): output parenthisis \n");
          curprint( "\n /* Output paren in Unparse_MOD_SAGE::unparseOneElemConInit */ \n");
#endif
          curprint("("); 
          unp->u_debug->printDebugInfo("( from OneElemConInit", true);
        }

  // printf ("con_init->get_args() = %p \n",con_init->get_args());
     if (con_init->get_args())
        {
          SgExprListExp* expr_list = isSgExprListExp(con_init->get_args());
          ROSE_ASSERT(expr_list != NULL);
          SgExpressionPtrList::iterator i = expr_list->get_expressions().begin();
          if (i != expr_list->get_expressions().end())
             {
               SgUnparse_Info arg_newinfo(info);
               arg_newinfo.set_SkipBaseType();
               unp->u_exprStmt->unparseExpression(*i, arg_newinfo);
             }
        }

  // if (con_init->get_need_name() && unp->u_sage->printConstructorName(con_init)) 
     if (outputParenthisis == true)
        {
          curprint( ")");
          unp->u_debug->printDebugInfo(") from OneElemConInit", true);
        }

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES
     printf ("Leaving of Unparse_MOD_SAGE::unparseOneElemConInit \n\n\n");
     curprint( "\n /* Leaving of Unparse_MOD_SAGE::unparseOneElemConInit */ \n");
#endif
#if 0
     printf ("Leaving of Unparse_MOD_SAGE::unparseOneElemConInit \n\n\n");
     curprint( "\n /* Leaving of Unparse_MOD_SAGE::unparseOneElemConInit */ \n");
#endif
   }


void
Unparse_ExprStmt::unparseFunctionParameterDeclaration ( 
   SgFunctionDeclaration* funcdecl_stmt, 
   SgInitializedName* initializedName,
   bool outputParameterDeclaration,
   SgUnparse_Info& info )
   {
     ROSE_ASSERT (funcdecl_stmt != NULL);

     ROSE_ASSERT (initializedName != NULL);
     SgName        tmp_name  = initializedName->get_name();
     SgInitializer *tmp_init = initializedName->get_initializer();
     SgType        *tmp_type = initializedName->get_type();

  // DQ (9/7/2014): These should have been setup to be the same.
     ROSE_ASSERT(info.SkipClassDefinition() == info.SkipEnumDefinition());

#if 0
     printf ("In unparseFunctionParameterDeclaration(): TOP \n");
     printf ("   --- funcdecl_stmt                                 = %p = %s \n",funcdecl_stmt,funcdecl_stmt->get_name().str());
     printf ("   --- funcdecl_stmt->get_type_syntax_is_available() = %s \n",funcdecl_stmt->get_type_syntax_is_available() ? "true" : "false");
     printf ("   --- initializedName                               = %p = %s \n",initializedName,initializedName->get_name().str());
     printf ("   --- initializedName->get_name()                   = %s \n",initializedName->get_name().str());
     printf ("   --- initializedName->get_type()                   = %p = %s \n",initializedName->get_type(),initializedName->get_type()->class_name().c_str());
#endif

#if 1
  // DQ (9/14/2015): Test disabling this for C++11 mode.

  // DQ (7/10/2014): Added support for using the original type syntax (saved as the declared function type).
     if (funcdecl_stmt->get_type_syntax_is_available() == true)
        {
#if 0
       // DQ (4/13/2018): Since the API permits the specification of the correct SgInitializedName we don't need this code
       // which incedentally also set the type to be used incorectly (not matching the type syntax and initialized name 
       // used in the original syntax of the function prototype).

       // Here we want to use the type syntax that originally appears with this function declaration in the original code.
          SgFunctionType* function_type = funcdecl_stmt->get_type_syntax();
          ROSE_ASSERT(function_type != NULL);
#if 0
       // printf ("Found the original function type syntax: function_type = %p = %s \n",function_type,function_type->unparseToString().c_str());
          printf ("In unparseFunctionParameterDeclaration(): Found the original function type syntax: function_type = %p = %s \n",function_type,function_type->class_name().c_str());
#endif
          SgFunctionParameterTypeList* type_argument_list = function_type->get_argument_list();
          ROSE_ASSERT(type_argument_list != NULL);

       // DQ (4/13/2018): I think it is awkward that we need to introduce this test here (there might be a better API for this function).
       // find the associated index from the initializedName.
       // SgFunctionParameterList* name_argument_list = funcdecl_stmt->get_parameterList();
          SgFunctionParameterList* name_argument_list = NULL;
          if (funcdecl_stmt->get_type_syntax_is_available() == true)
             {
               name_argument_list = funcdecl_stmt->get_parameterList_syntax();
             }
            else
             {
               name_argument_list = funcdecl_stmt->get_parameterList();
             }
          ROSE_ASSERT(name_argument_list != NULL);

          SgInitializedNamePtrList & name_list = name_argument_list->get_args();

          SgInitializedNamePtrList::iterator i = name_list.begin();
          size_t counter = 0;
          while (i != name_list.end() && (*i) != initializedName)
             {
#if 0
               printf ("In unparseFunctionParameterDeclaration(): loop: counter = %" PRIuPTR " \n",counter);
#endif
               counter++;
               i++;
             }

          ROSE_ASSERT(i != name_list.end());
#if 0
          printf ("In unparseFunctionParameterDeclaration(): counter = %" PRIuPTR " \n",counter);
#endif
       // SgTypePtrList & get_arguments()
          tmp_type = type_argument_list->get_arguments()[counter];
#endif

#if 0
          printf ("Resetting tmp_type = %p = %s \n",tmp_type,tmp_type->class_name().c_str());
#endif
#if 0
          printf ("In unparseFunctionParameterDeclaration(): (funcdecl_stmt->get_type_syntax_is_available() == true): exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }
       else
        {
       // DQ (7/10/2014): Enforce this rule.
          ROSE_ASSERT(funcdecl_stmt->get_type_syntax() == NULL);
        }
#endif

#if 0
     printf ("In unparseFunctionParameterDeclaration(): exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

  // DQ (8/9/2013): refactored to support additional refactoring to seperate out code to unparse SgInitializedName.
     bool oldStyleDefinition = funcdecl_stmt->get_oldStyleDefinition();

#if 0
     printf ("unparseFunctionParameterDeclaration(): funcdecl_stmt->get_args().size() = %" PRIuPTR " \n",funcdecl_stmt->get_args().size());
     curprint( string("\n/* unparseFunctionParameterDeclaration(): funcdecl_stmt->get_args().size() = ") + StringUtility::numberToString((int)(funcdecl_stmt->get_args().size())) + " */ \n");
#endif
#if 0
     info.display("unparseFunctionParameterDeclaration()");
#endif
#if 0
   // Liao 11/9/2010, moved to upper callers since this is called when unparsing both old-style and new-style function parameter lists
   // Skip duplicated unparsing of the attached information for C function arguments declared in old style.
   // They usually should be unparsed when unparsing the arguments which are outside of the parameter list
   // See example code: tests/nonsmoke/functional/CompileTests/C_tests/test2010_10.c
    if (oldStyleDefinition == false )
       unparseAttachedPreprocessingInfo(initializedName, info, PreprocessingInfo::before);
#endif
  // printf ("In unparseFunctionParameterDeclaration(): Argument name = %s \n",
  //      (tmp_name.str() != NULL) ? tmp_name.str() : "NULL NAME");

  // initializedName.get_storageModifier().display("New storage modifiers in unparseFunctionParameterDeclaration()");

#if 0
 // DQ (*8/18/2014): Microsoft declspec attributes that apply to function parameters.
    appdomain
    deprecated
    dllimport
    dllexport
    novtable
    process
    restrict
    selectany
    thread
    uuid(" ComObjectGUID ")
#endif

#define MS_DECLSPEC_DEBUG 0

     if (initializedName->is_ms_declspec_parameter_appdomain())
        {
#if MS_DECLSPEC_DEBUG
          printf ("In Unparse_ExprStmt::unparseFunctionParameterDeclaration(): Output the Microsoft __declspec(appdomain) \n");
#endif
          curprint("__declspec(appdomain) ");
        }
    
     if (initializedName->is_ms_declspec_parameter_deprecated())
        {
#if MS_DECLSPEC_DEBUG
          printf ("In Unparse_ExprStmt::unparseFunctionParameterDeclaration(): Output the Microsoft __declspec(deprecated) \n");
#endif
          curprint("__declspec(deprecated) ");
        }
    
     if (initializedName->is_ms_declspec_parameter_dllimport())
        {
#if MS_DECLSPEC_DEBUG
          printf ("In Unparse_ExprStmt::unparseFunctionParameterDeclaration(): Output the Microsoft __declspec(dllimport) \n");
#endif
          curprint("__declspec(dllimport) ");
        }
    
     if (initializedName->is_ms_declspec_parameter_dllexport())
        {
#if MS_DECLSPEC_DEBUG
          printf ("In Unparse_ExprStmt::unparseFunctionParameterDeclaration(): Output the Microsoft __declspec(dllexport) \n");
#endif
          curprint("__declspec(dllexport) ");
        }
    
     if (initializedName->is_ms_declspec_parameter_novtable())
        {
#if MS_DECLSPEC_DEBUG
          printf ("In Unparse_ExprStmt::unparseFunctionParameterDeclaration(): Output the Microsoft __declspec(novtable) \n");
#endif
          curprint("__declspec(novtable) ");
        }
    
     if (initializedName->is_ms_declspec_parameter_process())
        {
#if MS_DECLSPEC_DEBUG
          printf ("In Unparse_ExprStmt::unparseFunctionParameterDeclaration(): Output the Microsoft __declspec(process) \n");
#endif
          curprint("__declspec(process) ");
        }
    
     if (initializedName->is_ms_declspec_parameter_restrict())
        {
#if MS_DECLSPEC_DEBUG
          printf ("In Unparse_ExprStmt::unparseFunctionParameterDeclaration(): Output the Microsoft __declspec(restrict) \n");
#endif
          curprint("__declspec(restrict) ");
        }
    
     if (initializedName->is_ms_declspec_parameter_selectany())
        {
#if MS_DECLSPEC_DEBUG
          printf ("In Unparse_ExprStmt::unparseFunctionParameterDeclaration(): Output the Microsoft __declspec(selectany) \n");
#endif
          curprint("__declspec(selectany) ");
        }
    
     if (initializedName->is_ms_declspec_parameter_thread())
        {
#if MS_DECLSPEC_DEBUG
          printf ("In Unparse_ExprStmt::unparseFunctionParameterDeclaration(): Output the Microsoft __declspec(thread) \n");
#endif
          curprint("__declspec(thread) ");
        }
    
     if (initializedName->is_ms_declspec_parameter_uuid())
        {
#if MS_DECLSPEC_DEBUG
          printf ("In Unparse_ExprStmt::unparseFunctionParameterDeclaration(): Output the Microsoft __declspec(uuid) \n");
#endif
       // curprint("__declspec(uuid) ");
          curprint("__declspec(uuid(\"");
          curprint(initializedName->get_microsoft_uuid_string());
          curprint("\")) ");
        }

     SgStorageModifier & storage = initializedName->get_storageModifier();
     if (storage.isExtern())
        {
#if 0
          printf ("In Unparse_ExprStmt::unparseFunctionParameterDeclaration(): Output the extern keyword \n");
#endif
          curprint("extern ");
        }

  // DQ (7/202/2006): The isStatic() function in the SgStorageModifier held by the SgInitializedName object should always be false.
  // This is because the static-ness of a variable is held by the SgVariableDeclaration (and the SgStorageModified help in the SgDeclarationModifier).
  // printf ("In initializedName = %p test the return value of storage.isStatic() = %d = %d (should be boolean value) \n",initializedName,storage.isStatic(),storage.get_modifier());
     ROSE_ASSERT(storage.isStatic() == false);

  // This was a bug mistakenly reported by Isaac
     ROSE_ASSERT(storage.get_modifier() >= 0);

     if (storage.isStatic())
        {
          curprint("static ");
        }

     if (storage.isAuto())
        {
       // DQ (4/30/2004): Auto is a default which is to be supressed 
       // in C old-style parameters and not really ever needed anyway?
       // curprint( "auto ");
        }

     if (storage.isRegister())
        {
       // DQ (12/10/2007): This is a fix for C_tests/test2007_177.c (this is only an issue if --edg:restrict is used on the commandline).
       // curprint( "register ");
          if ( (oldStyleDefinition == false) || (outputParameterDeclaration == true) )
             {
               curprint("register ");
             }
        }

     if (storage.isMutable())
        {
          curprint("mutable ");
        }

     if (storage.isTypedef())
        {
          curprint("typedef ");
        }

     if (storage.isAsm())
        {
       // DQ (2/6/2014): Fix to support GNU gcc.
       // curprint("asm ");
          curprint("__asm__ ");
        }

  // TV (05/06/2010): CUDA storage modifiers
     if (storage.isCudaGlobal())
        {
          curprint("__device__ ");
        }

     if (storage.isCudaConstant())
        {
          curprint("__device__ __constant__ ");
        }

     if (storage.isCudaShared())
        {
          curprint("__device__ __shared__ ");
        }

     if (storage.isCudaDynamicShared())
        {
          curprint("extern __device__ __shared__ ");
        }

  // Error checking, if we are using old style C function parameters, then I hope this is not C++ code!
     if (oldStyleDefinition == true)
        {
          if (SageInterface::is_Cxx_language() == true)
             {
               printf ("Warning: Mixing old style C function parameters with C++ is maybe not well defined \n");
             }
          ROSE_ASSERT (SageInterface::is_Cxx_language() == false);
        }

     if ( (oldStyleDefinition == false) || (outputParameterDeclaration == true) )
        {
       // output the type name for each argument
          if (tmp_type != NULL)
             {
               SgUnparse_Info ninfo(info);
#if 0
               printf ("In unparseFunctionParameterDeclaration(): tmp_type = %p = %s \n",tmp_type,tmp_type->class_name().c_str());
#endif
            // DQ (2/3/2019): In the case of function parameters, the member pointer types need an extra parenthesis.
            // This might just apply to arrays of SgMemberPointerType.
               SgPointerMemberType* pointerToMemberType = isSgPointerMemberType(tmp_type);
               if (pointerToMemberType != NULL)
                  {
                    ninfo.set_inArgList();
                  }
#if 1
            // DQ (4/12/2019): This version is required for C old-style function parameters.
            // DQ (4/11/2019): Try to comment this out to support Clang 8.0 which can't handle the "enum class" type elaboration.
               if (initializedName->get_needs_definitions())
                  {
                    ninfo.unset_SkipClassDefinition();
                    ninfo.unset_SkipEnumDefinition();
                  }
#else
            // If we are using Clang then we might require this, GNU accepts with or without..
               if (isSgEnumType(tmp_type) != NULL)
                  {
                    ninfo.set_SkipClassSpecifier();
                  }
#endif
            // DQ (5/5/2013): Refactored code used here and in the unparseTemplateArgument().
               unp->u_type->outputType<SgInitializedName>(initializedName,tmp_type,ninfo);

            // DQ (2/3/2019): In the case of function parameters, the member pointer types need an extra parenthesis.
               if (pointerToMemberType != NULL)
                  {
                    ninfo.unset_inArgList();
                  }
             }
            else
             {
               curprint(tmp_name.str()); // for ... case
             }
        }
       else
        {
          curprint(tmp_name.str()); // for ... case
        }

     SgUnparse_Info ninfo3(info);
     ninfo3.unset_inArgList();

  // DQ (4/27/2013): We now have better support in ROSE to know when to output the default arguments,
  // so we don't want to use this mechanism above.  So now we always output the default arguments for
  // function parameters in a function declaration if they are defined in the AST. It is up to the
  // specification in the AST to have them in the correct locations, consistant with the source code.
     bool outputInitializer = true;

  // Add an initializer if it exists
     if ( outputInitializer == true && tmp_init != NULL )
        {
       // Cong (6/28/2011): When unparsing an initializer for a function parameter, we should add a space before '='.
       // Or else, foo(const int& = 1) will be unparsed to foo(const int&=1) which contains an operator '&=", which is 
       // incorrect.
          curprint(" = ");
          unp->u_exprStmt->unparseExpression(tmp_init, ninfo3);
        }

  // DQ (1/7/2014): Adding support for GNU specific noreturn attribute for function parameters 
  // (only applies to parameters that are of function pointer type).
     if (initializedName->isGnuAttributeNoReturn() == true)
        {
          curprint(" __attribute__((noreturn))");
#if 0
          printf ("Detected initializedName->isGnuAttributeNoReturn() == true: (not implemented) \n");
          ROSE_ASSERT(false);
#endif
        }

#if 0
     printf ("Leaving unparseFunctionParameterDeclaration(): funcdecl_stmt->get_args().size() = %" PRIuPTR " \n",funcdecl_stmt->get_args().size());
     curprint( string("\n/* Leaving unparseFunctionParameterDeclaration(): funcdecl_stmt->get_args().size() = ") + StringUtility::numberToString((int)(funcdecl_stmt->get_args().size())) + " */ \n");
#endif
   }


void
Unparse_ExprStmt::unparseFunctionArgs(SgFunctionDeclaration* funcdecl_stmt, SgUnparse_Info& info)
   {
     ROSE_ASSERT (funcdecl_stmt != NULL);

#if 0
     printf ("In unparseFunctionArgs(): funcdecl_stmt->get_args().size() = %" PRIuPTR " \n",funcdecl_stmt->get_args().size());
     curprint("\n/* unparseFunctionArgs(): funcdecl_stmt->get_args().size() = " + StringUtility::numberToString((int)(funcdecl_stmt->get_args().size())) + " */ \n");
     printf ("In unparseFunctionArgs(): funcdecl_stmt->get_prototypeIsWithoutParameters() = %s \n",funcdecl_stmt->get_prototypeIsWithoutParameters() ? "true" : "false");
#endif

  // DQ (9/7/2014): These should have been setup to be the same.
     ROSE_ASSERT(info.SkipClassDefinition() == info.SkipEnumDefinition());

  // DQ (1/18/2014): This is a better implementation than setting the source position info on the function
  // parameters.  See test2014_35.c for an example that requires this solution using a new data member.
     if (funcdecl_stmt->get_prototypeIsWithoutParameters() == true )
        {
#if 0
          printf ("In unparseFunctionArgs(): Detected prototypeIsWithoutParameters == true (funcdecl_stmt = %p) \n",funcdecl_stmt);
#endif
          return;
        }

#if 0
  // DQ (5/29/2019): Disavle this feature as a test for test2019_444.C.
     if (funcdecl_stmt->get_type_syntax_is_available() == true)
        {
          printf ("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& \n");
          printf ("Disabling the type_syntax_is_available feature for function parameters \n");
          printf ("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& \n");

          ROSE_ASSERT(funcdecl_stmt->get_type_syntax() != NULL);

          funcdecl_stmt->set_type_syntax_is_available(false);
          funcdecl_stmt->set_type_syntax(NULL);
        }
     ROSE_ASSERT(funcdecl_stmt->get_type_syntax_is_available() == false);
     ROSE_ASSERT(funcdecl_stmt->get_type_syntax() == NULL);
#endif

#if 0
  // DQ (1/17/2014): Adding support in C to output function prototypes without function parameters.
     bool functionParametersMarkedToBeOutput = false;
     SgInitializedNamePtrList::iterator temp_p = funcdecl_stmt->get_args().begin();
     while ( temp_p != funcdecl_stmt->get_args().end() )
        {
       // If any are marked with valid source position then functionParametersMarkedToBeOutput will be true after the loop.
          if (functionParametersMarkedToBeOutput == false && (*temp_p)->get_file_info()->isCompilerGenerated() == true)
             {
               functionParametersMarkedToBeOutput = false;
             }
            else
             {
               functionParametersMarkedToBeOutput = true;
             }
#if 0
          (*temp_p)->get_file_info()->display("unparseFunctionArgs(): SgInitializedName: debug");
#endif
          temp_p++;
        }

     bool outputFunctionParameters = functionParametersMarkedToBeOutput;
#else
     bool outputFunctionParameters = true;
#endif

     SgInitializedNamePtrList::iterator p        = funcdecl_stmt->get_args().begin();

  // DQ (4/13/2018): I want to initialize this iterator, but it is not clear what to initialize it to...
     SgInitializedNamePtrList::iterator p_syntax = funcdecl_stmt->get_args().begin();
     if (funcdecl_stmt->get_type_syntax_is_available() == true)
        {
          p_syntax = funcdecl_stmt->get_parameterList_syntax()->get_args().begin();

#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

     while ( p != funcdecl_stmt->get_args().end() )
        {
        // Liao 11/9/2010, 
        // Skip duplicated unparsing of the attached information for C function arguments declared in old style.
        // They usually should be unparsed when unparsing the arguments which are outside of the parameter list
        // are outside of the parameter list
        // See example code: tests/nonsmoke/functional/CompileTests/C_tests/test2010_10.c
          if (funcdecl_stmt->get_oldStyleDefinition() == false)
             {
               unparseAttachedPreprocessingInfo(*p, info, PreprocessingInfo::before);
             }

       // DQ (1/17/2014): Adding support in C to output function prototypes without function parameters.
       // unparseFunctionParameterDeclaration (funcdecl_stmt,*p,false,info);
       // if (outputFunctionParameters == true)
          if ( (outputFunctionParameters == true) || (funcdecl_stmt->get_oldStyleDefinition() == true) )
             {
#if 0
               printf ("In unparseFunctionArgs(): Calling unparseFunctionParameterDeclaration() \n");
#endif
            // DQ (4/13/2018): If we have saved the original syntax then use it, else use the default (which is matching the defining function declaration).
            // unparseFunctionParameterDeclaration (funcdecl_stmt,*p,false,info);
               if (funcdecl_stmt->get_type_syntax_is_available() == true)
                  {
                 // DQ (4/13/2018): One question would be are we using the correct name qualification for any type referenced.
#if 0
                    printf ("In unparseFunctionArgs(): Output the syntax for function parameters: (*p_syntax)->get_name() = %s \n",(*p_syntax)->get_name().str());
#endif
                    unparseFunctionParameterDeclaration (funcdecl_stmt,*p_syntax,false,info);

#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }
                 else
                  {
                    unparseFunctionParameterDeclaration (funcdecl_stmt,*p,false,info);
                  }
             }

       // Move to the next argument
          p++;

       // DQ (4/13/2018): Increment the type syntax iterator in unison.
          if (funcdecl_stmt->get_type_syntax_is_available() == true)
             {
               p_syntax++;
             }

       // Check if this is the last argument (output a "," separator if not)
          if (p != funcdecl_stmt->get_args().end())
             {
               curprint(",");
             }
        }

#if 0
     printf ("Leaving unparseFunctionArgs(): funcdecl_stmt->get_args().size() = %" PRIuPTR " \n",funcdecl_stmt->get_args().size());
     curprint("\n/* Leaving unparseFunctionArgs(): funcdecl_stmt->get_args().size() = " + StringUtility::numberToString((int)(funcdecl_stmt->get_args().size())) + " */ \n");
#endif
   }

//-----------------------------------------------------------------------------------
//  void Unparse_ExprStmt::unparse_helper
//
//  prints out the function parameters in a function declaration or function
//  call. For now, all parameters are printed on one line since there is no
//  file information for each parameter.
//-----------------------------------------------------------------------------------
void
Unparse_ExprStmt::unparse_helper(SgFunctionDeclaration* funcdecl_stmt, SgUnparse_Info& info)
   {
     ROSE_ASSERT (funcdecl_stmt != NULL);

     SgTemplateInstantiationFunctionDecl* templateFunctionDeclaration = isSgTemplateInstantiationFunctionDecl(funcdecl_stmt);

#if 0
     printf ("In unparse_helper(): funcdecl_stmt->get_name() = %s \n",funcdecl_stmt->get_name().str());
     if (templateFunctionDeclaration != NULL)
        {
          printf ("   --- templateFunctionDeclaration->get_name()         = %s \n",templateFunctionDeclaration->get_name().str());
          printf ("   --- templateFunctionDeclaration->get_templateName() = %s \n",templateFunctionDeclaration->get_templateName().str());
        }
     printf ("funcdecl_stmt                                    = %p \n",funcdecl_stmt);
     printf ("funcdecl_stmt->get_firstNondefiningDeclaration() = %p \n",funcdecl_stmt->get_firstNondefiningDeclaration());
     printf ("funcdecl_stmt->get_definingDeclaration()         = %p \n",funcdecl_stmt->get_definingDeclaration());
#endif
#if 0
     curprint(" /* TOP unparse_helper */ \n");
#endif

  // DQ (9/8/2007): Friend function declaration should be qualified, if the associated function has already been seen.
  // See test2007_124.C for an example. Friend declarations of operators are not qualified, or at least should not
  // use global qualification. If this is the first declaration, then no qualification should be used (see test2004_117.C).
     SgUnparse_Info ninfo(info);
  // printf ("funcdecl_stmt = %p funcdecl_stmt->get_definingDeclaration() = %p funcdecl_stmt->get_firstNondefiningDeclaration() = %p \n",
  //      funcdecl_stmt,funcdecl_stmt->get_definingDeclaration(),funcdecl_stmt->get_firstNondefiningDeclaration());
     bool isFirstDeclaration = funcdecl_stmt == funcdecl_stmt->get_firstNondefiningDeclaration();
     if (funcdecl_stmt->get_declarationModifier().isFriend() == true && funcdecl_stmt->get_specialFunctionModifier().isOperator() == false && isFirstDeclaration == false )
        {
          ninfo.set_forceQualifiedNames();
        }

  // DQ (11/18/2004): Added support for qualified name of template declaration!
  // But it appears that the qualified name is included within the template text string so that 
  // we should not output the qualified name spearately!
#if 0
  // DQ (3/4/2009): This code fails for tutorial/rose_inputCode_InstrumentationTranslator.C
  // commented out this branch in favor of the other one!

  // printf ("Before calling generateNameQualifier(): ninfo.get_current_scope() = %p = %s \n",ninfo.get_current_scope(),ninfo.get_current_scope()->class_name().c_str());

  // DQ (2/22/2009): Added assertion.
  // ROSE_ASSERT(funcdecl_stmt->get_symbol_from_symbol_table() != NULL);
     if (funcdecl_stmt->get_firstNondefiningDeclaration() != NULL)
        {
          ROSE_ASSERT (funcdecl_stmt->get_firstNondefiningDeclaration()->get_symbol_from_symbol_table() != NULL);
        }

     SgName nameQualifier = unp->u_name->generateNameQualifier( funcdecl_stmt , ninfo );
#else
  // DQ (10/24/2007): Added fix by Jeremiah (not well tested) This is suggested by Jeremiah, 
  // but it does not yet address friend functions which might require qualification.

     SgUnparse_Info ninfoForFunctionName(ninfo);

  // DQ (5/13/2011): Support for new name qualification.
     ninfoForFunctionName.set_name_qualification_length(funcdecl_stmt->get_name_qualification_length());
     ninfoForFunctionName.set_global_qualification_required(funcdecl_stmt->get_global_qualification_required());

#if 0
     printf ("In unparse_helper(): funcdecl_stmt->get_name_qualification_length() = %d funcdecl_stmt->get_global_qualification_required() = %s \n",funcdecl_stmt->get_name_qualification_length(),funcdecl_stmt->get_global_qualification_required() ? "true" : "false");
#endif

     if (isSgClassDefinition(funcdecl_stmt->get_parent()))
        {
       // JJW 10-23-2007 Never qualify a member function name
          ninfoForFunctionName.set_SkipQualifiedNames();
        }
  // SgName nameQualifier = unp->u_name->generateNameQualifier( funcdecl_stmt , ninfoForFunctionName );
     SgName nameQualifier = funcdecl_stmt->get_qualified_name_prefix();
#endif

#if 0
     printf ("In unparse_helper(): nameQualifier (from unp->u_name->generateNameQualifier function) = %s \n",nameQualifier.str());
#endif

  // DQ (10/12/2006): need to trim off the global scope specifier (I think).
  // curprint( "\n/* Calling trimGlobalScopeQualifier() */\n ");
  // curprint( "\n/* Skipping trimGlobalScopeQualifier() */\n ");
  // nameQualifier = trimGlobalScopeQualifier ( nameQualifier.str() ).c_str();

     curprint(nameQualifier.str());

  // DQ (11/16/2013): See test2013_273.C for where we don't always want to always output the name with template arguments.
  // Alternatively we can just make sure that we use correct name qualification on the template arguments when this is a
  // template instantiation.
  // output the function name
  // curprint( funcdecl_stmt->get_name().str());
     if (templateFunctionDeclaration != NULL)
        {
       // curprint(templateFunctionDeclaration->get_templateName().str());
#if 0
          printf ("Calling unparseTemplateFunctionName() \n");
          curprint(" /* In unparse_helper(): Calling unparseTemplateFunctionName() */ \n");
#endif
          unp->u_exprStmt->unparseTemplateFunctionName(templateFunctionDeclaration,info);
#if 0
          printf ("Done: unparseTemplateFunctionName() \n");
          curprint(" /* In unparse_helper(): Done: unparseTemplateFunctionName() */ \n");
#endif
        }
       else
        {
          curprint(funcdecl_stmt->get_name().str());
        }

#if 0
  // DQ (2/16/2005): Function name has been modified instead.
  // DQ (2/15/2005): If this is a template function then we need to output the template parameters!
     SgTemplateInstantiationFunctionDecl* templateFunctionDeclaration = isSgTemplateInstantiationFunctionDecl(funcdecl_stmt);
     if (templateFunctionDeclaration != NULL)
        {
          printf ("Found a template function: output the template parameters! \n");
          curprint( " /* < template parameters > */ ");
          SgTemplateArgumentPtrListPtr templateArguments = templateFunctionDeclaration->get_templateArguments();
          printf ("Number of template arguments for instantiated template function = %" PRIuPTR " \n",templateArguments->size());
          if (templateArguments->size() > 0)
             {
             }
        }
#endif

  // DQ (9/7/2014): These should have been setup to be the same.
     ROSE_ASSERT(info.SkipClassDefinition() == info.SkipEnumDefinition());

     SgUnparse_Info ninfo2(info);
     ninfo2.set_inArgList();

  // DQ (5/14/2003): Never output the class definition in the argument list.
  // Using this C++ constraint avoids building a more complex mechanism to turn it off.
     ninfo2.set_SkipClassDefinition();

  // DQ (9/7/2014): Also set the associated flag to supress output of enum definitions.
     ninfo2.set_SkipEnumDefinition();

  // DQ (9/7/2014): These should have been setup to be the same.
     ROSE_ASSERT(ninfo2.SkipClassDefinition() == ninfo2.SkipEnumDefinition());

     curprint("(");

     unparseFunctionArgs(funcdecl_stmt,ninfo2);

  // printf ("Adding a closing \")\" to the end of the argument list \n");
     curprint(")");

#if 0
     printf ("In unparse_helper(): funcdecl_stmt->get_oldStyleDefinition() = %s \n",funcdecl_stmt->get_oldStyleDefinition() ? "true" : "false");
#endif

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
#if 0
               printf ("Output the comments and CCP directives for the SgInitializedName function args = %p \n",*p);
#endif
               unparseAttachedPreprocessingInfo(*p, info, PreprocessingInfo::before);
#if 0
               printf ("DONE: Output the comments and CCP directives for the SgInitializedName function args = %p \n",*p);
#endif
            // Output declarations for function parameters (using old-style K&R syntax)
            // printf ("Output declarations for function parameters (using old-style K&R syntax) \n");
               unparseFunctionParameterDeclaration(funcdecl_stmt,*p,true,ninfo2);

               curprint( ";");
               unp->u_sage->curprint_newline();
               p++;
             }
        }

#if 0
     curprint(" /* BOTTOM unparse_helper */ \n");
#endif
#if 0
  // curprint( endl;
     curprint("/* Added closing \")\" to the end of the argument list */ \n");
  // curprint(flush();
     printf ("Leaving Unparse_ExprStmt::unparse_helper() \n");
#endif

#if 0
     printf ("Leaving unparse_helper(): funcdecl_stmt->get_name() = %s \n",funcdecl_stmt->get_name().str());
#endif
   }


void
Unparse_ExprStmt::unparseLanguageSpecificStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
  // This function unparses the language specific parse not handled by the base class unparseStatement() member function

     ROSE_ASSERT(stmt != NULL);

  // curprint("In unparseLanguageSpecificStatement()");

#if 0
     printf ("Top of unparseLanguageSpecificStatement (Unparse_ExprStmt) = %p = %s \n",stmt,stmt->class_name().c_str());
#endif
#if 0
     curprint ( string("\n/* Top of unparseLanguageSpecificStatement (Unparse_ExprStmt) " ) + stmt->class_name() + " */\n ");
#endif
#if 0
     ROSE_ASSERT(stmt->get_startOfConstruct() != NULL);
  // ROSE_ASSERT(stmt->getAttachedPreprocessingInfo() != NULL);
     int numberOfComments = -1;
     if (stmt->getAttachedPreprocessingInfo() != NULL)
          numberOfComments = stmt->getAttachedPreprocessingInfo()->size();
     curprint(string("/* startOfConstruct: file = " ) + stmt->get_startOfConstruct()->get_filenameString()
        + " raw filename = " + stmt->get_startOfConstruct()->get_raw_filename()
        + " raw line = "     + StringUtility::numberToString(stmt->get_startOfConstruct()->get_raw_line())
        + " raw column = "   + StringUtility::numberToString(stmt->get_startOfConstruct()->get_raw_col())
        + " #comments = "    + StringUtility::numberToString(numberOfComments)
        + " */\n ");
#endif

#if 0
     printf ("In unparseLanguageSpecificStatement(): info.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
     printf ("In unparseLanguageSpecificStatement(): info.SkipEnumDefinition()  = %s \n",(info.SkipEnumDefinition() == true) ? "true" : "false");
#endif

  // DQ (1/9/2014): These should have been setup to be the same.
     ROSE_ASSERT(info.SkipClassDefinition() == info.SkipEnumDefinition());

#if 0
  // Debugging support
     SgDeclarationStatement* declarationStatement = isSgDeclarationStatement(stmt);
     if (declarationStatement != NULL)
        {
          curprint("/* In unparseLanguageSpecificStatement(): declarationStatement->get_declarationModifier().isFriend() = ");
          declarationStatement->get_declarationModifier().isFriend() ? curprint("true") : curprint("false");
          curprint("*/ \n ");
        }
#endif

#if ROSE_TRACK_PROGRESS_OF_ROSE_COMPILING_ROSE || 0
     printf ("In unparseLanguageSpecificStatement(): file = %s line = %d \n",stmt->get_startOfConstruct()->get_filenameString().c_str(),stmt->get_startOfConstruct()->get_line());
#endif

#if 0
  // Debugging support
     SgDeclarationStatement* declarationStatement = isSgDeclarationStatement(stmt);
     if (declarationStatement != NULL)
        {
          curprint ( string("\n/* Inside of Unparse_ExprStmt::unparseLanguageSpecificStatement (" ) + StringUtility::numberToString(stmt) + "): sage_class_name() = " + stmt->sage_class_name() + " */ \n");
        }
#endif

#if 0
  // This is done in: UnparseLanguageIndependentConstructs::unparseStatement()
  // DQ (12/5/2007): Check if the call to unparse any construct changes the scope stored in info.
     SgScopeStatement* savedScope = info.get_current_scope();
#endif

  // DQ (12/16/2008): Added support for unparsing statements around C++ specific statements
  // unparseAttachedPreprocessingInfo(stmt, info, PreprocessingInfo::before);

  // DQ (12/26/2007): Moved from language independent handling to C/C++ specific handling 
  // because we don't want it to appear in the Fortran code generation.
  // DQ (added comments) this is where the new lines are introduced before statements.
  // unp->cur.format(stmt, info, FORMAT_BEFORE_STMT);
  // if (info.unparsedPartiallyUsingTokenStream() == false)
     bool saved_unparsedPartiallyUsingTokenStream = info.unparsedPartiallyUsingTokenStream();
     if (saved_unparsedPartiallyUsingTokenStream == false)
        {
#if 0
          curprint("/* In unparseLanguageSpecificStatement(): CR: START */");
#endif
#if 1
       // DQ (11/14/2015): If we are unparsing statements in a SgBasicBlock, then we want to
       // know if the SgBasicBlock is being unparsed using the partial_token_sequence so that
       // we can supress the formatting that adds a CR to the start of the current statement 
       // being unparsed.
          bool parentStatementListBeingUnparsedUsingPartialTokenSequence = info.parentStatementListBeingUnparsedUsingPartialTokenSequence();
          if (parentStatementListBeingUnparsedUsingPartialTokenSequence == true)
             {
#if 0
               printf ("In unparseLanguageSpecificStatement(): We need to supress the leading CR for this case (initially statements in a SgBasicBlock) \n");
#endif
#if 0
               curprint("/* In InunparseLanguageSpecificStatement(): suppress CR */");
#endif
            // ROSE_ASSERT(false);
             }
            else
             {
               unp->cur.format(stmt, info, FORMAT_BEFORE_STMT);
             }
#else
          unp->cur.format(stmt, info, FORMAT_BEFORE_STMT);
#endif
#if 0
          curprint("/* InunparseLanguageSpecificStatement(): CR: END */");
#endif
        }

#if 0
     printf ("In Unparse_ExprStmt::unparseLanguageSpecificStatement(): Selecting an unparse function for stmt = %p = %s \n",stmt,stmt->class_name().c_str());
#endif

     switch (stmt->variantT())
        {
       // DQ (8/14/2007): Need to move the C and C++ specific unparse member functions from the base class to this function.

       // scope
       // case V_SgGlobal:                 unparseGlobalStmt(stmt, info); break;
       // case V_SgScopeStatement:         unparseScopeStmt(stmt, info); break;

       // pragmas
       // case V_SgPragmaDeclaration:      unparsePragmaDeclStmt(stmt, info); break;
       // scope
       // case V_SgGlobal:                 unparseGlobalStmt(stmt, info); break;
       //        case V_SgScopeStatement:         unparseScopeStmt (stmt, info); break;

       // program units
       // case V_SgModuleStatement:          unparseModuleStmt (stmt, info); break;
       // case V_SgProgramHeaderStatement:   unparseProgHdrStmt(stmt, info); break;
       // case V_SgProcedureHeaderStatement: unparseProcHdrStmt(stmt, info); break;

       // declarations
       // case V_SgInterfaceStatement:     unparseInterfaceStmt(stmt, info); break;
       // case V_SgCommonBlock:            unparseCommonBlock  (stmt, info); break;
          case V_SgVariableDeclaration:    unparseVarDeclStmt  (stmt, info); break;
          case V_SgVariableDefinition:     unparseVarDefnStmt  (stmt, info); break;
       // case V_SgParameterStatement:     unparseParamDeclStmt(stmt, info); break;
       // case V_SgUseStatement:           unparseUseStmt      (stmt, info); break;

       // executable statements, control flow
          case V_SgBasicBlock:             unparseBasicBlockStmt (stmt, info); break;
          case V_SgIfStmt:                 unparseIfStmt         (stmt, info); break;
       // case V_SgFortranDo:              unparseDoStmt         (stmt, info); break;
          case V_SgWhileStmt:              unparseWhileStmt      (stmt, info); break;
          case V_SgSwitchStatement:        unparseSwitchStmt     (stmt, info); break;
          case V_SgCaseOptionStmt:         unparseCaseStmt       (stmt, info); break;
          case V_SgDefaultOptionStmt:      unparseDefaultStmt    (stmt, info); break;
          case V_SgBreakStmt:              unparseBreakStmt      (stmt, info); break;
          case V_SgLabelStatement:         unparseLabelStmt      (stmt, info); break;
          case V_SgGotoStatement:          unparseGotoStmt       (stmt, info); break;
       // case V_SgStopOrPauseStatement:   unparseStopOrPauseStmt(stmt, info); break;
          case V_SgReturnStmt:             unparseReturnStmt     (stmt, info); break;

       // executable statements, IO
       // case V_SgIOStatement:            unparseIOStmt    (stmt, info); break;
       // case V_SgIOControlStatement:     unparseIOCtrlStmt(stmt, info); break;

       // pragmas
          case V_SgPragmaDeclaration:      unparsePragmaDeclStmt(stmt, info); break;

       // DQ (3/22/2019): Adding EmptyDeclaration to support addition of comments and CPP directives that will permit 
       // token-based unparsing to work with greater precision. For example, used to add an include directive with 
       // greater precision to the global scope and permit the unparsing via the token stream to be used as well.
          case V_SgEmptyDeclaration:      unparseEmptyDeclaration(stmt, info); break;

       // case DECL_STMT:          unparseDeclStmt(stmt, info);         break;
       // case SCOPE_STMT:         unparseScopeStmt(stmt, info);        break;
       //        case V_SgFunctionTypeTable:      unparseFuncTblStmt(stmt, info);      break;
       // case GLOBAL_STMT:        unparseGlobalStmt(stmt, info);       break;
       // case V_SgBasicBlock:             unparseBasicBlockStmt(stmt, info);   break;
       // case IF_STMT:            unparseIfStmt(stmt, info);           break;

          case V_SgForStatement:           unparseForStmt(stmt, info);          break; 

       // DQ (3/26/2018): Adding support for C++11 IR node (previously missed).
          case V_SgRangeBasedForStatement: unparseRangeBasedForStmt(stmt, info); break; 

          case V_SgFunctionDeclaration:    unparseFuncDeclStmt(stmt, info);     break;
          case V_SgTemplateFunctionDefinition: unparseTemplateFunctionDefnStmt(stmt, info); break;
          case V_SgFunctionDefinition:     unparseFuncDefnStmt(stmt, info);     break;
          case V_SgMemberFunctionDeclaration: unparseMFuncDeclStmt(stmt, info); break;
       // case VAR_DECL_STMT:      unparseVarDeclStmt(stmt, info);      break;
       // case VAR_DEFN_STMT:      unparseVarDefnStmt(stmt, info);      break;
          case V_SgClassDeclaration:       unparseClassDeclStmt(stmt, info);    break;
          case V_SgClassDefinition:        unparseClassDefnStmt(stmt, info);    break;
          case V_SgEnumDeclaration:        unparseEnumDeclStmt(stmt, info);     break;
          case V_SgExprStatement:          unparseExprStmt(stmt, info);         break;
       // case LABEL_STMT:         unparseLabelStmt(stmt, info);        break;
       // case WHILE_STMT:         unparseWhileStmt(stmt, info);        break;
          case V_SgDoWhileStmt:            unparseDoWhileStmt(stmt, info);      break;
       // case SWITCH_STMT:        unparseSwitchStmt(stmt, info);       break;
       // case CASE_STMT:          unparseCaseStmt(stmt, info);         break;
          case V_SgTryStmt:                unparseTryStmt(stmt, info);          break;
          case V_SgCatchOptionStmt:        unparseCatchStmt(stmt, info);        break;
       // case DEFAULT_STMT:       unparseDefaultStmt(stmt, info);      break;
       // case BREAK_STMT:         unparseBreakStmt(stmt, info);        break;
          case V_SgContinueStmt:           unparseContinueStmt(stmt, info);     break;
       // case RETURN_STMT:        unparseReturnStmt(stmt, info);       break;
       // case GOTO_STMT:          unparseGotoStmt(stmt, info);         break;
          case V_SgAsmStmt:                unparseAsmStmt(stmt, info);          break;
       // case SPAWN_STMT:         unparseSpawnStmt(stmt, info);        break;
          case V_SgTypedefDeclaration:     unparseTypeDefStmt(stmt, info);      break;
          case V_SgTemplateDeclaration:    unparseTemplateDeclStmt(stmt, info); break;

       // DQ (6/11/2011): Added support for new template IR nodes.
       // case V_SgTemplateClassDeclaration:                unparseTemplateDeclStmt(stmt, info); break;
       // case V_SgTemplateFunctionDeclaration:             unparseTemplateDeclStmt(stmt, info); break;
       // case V_SgTemplateMemberFunctionDeclaration:       unparseTemplateDeclStmt(stmt, info); break;
       // case V_SgTemplateVariableDeclaration:             unparseTemplateDeclStmt(stmt, info); break;

       // DQ (12/26/2011): New design for template declarations (no longer derived from StTemplateDeclaration).
          case V_SgTemplateClassDeclaration:                unparseTemplateClassDeclStmt(stmt, info);          break;
          case V_SgTemplateClassDefinition:                 unparseTemplateClassDefnStmt(stmt, info);          break;
          case V_SgTemplateFunctionDeclaration:             unparseTemplateFunctionDeclStmt(stmt, info);       break;
          case V_SgTemplateMemberFunctionDeclaration:       unparseTemplateMemberFunctionDeclStmt(stmt, info); break;
          case V_SgTemplateVariableDeclaration:             unparseTemplateVariableDeclStmt(stmt, info);       break;

          case V_SgTemplateInstantiationDecl:               unparseTemplateInstantiationDeclStmt(stmt, info); break;
          case V_SgTemplateInstantiationFunctionDecl:       unparseTemplateInstantiationFunctionDeclStmt(stmt, info); break;
          case V_SgTemplateInstantiationMemberFunctionDecl: unparseTemplateInstantiationMemberFunctionDeclStmt(stmt, info); break;
          case V_SgTemplateInstantiationDirectiveStatement: unparseTemplateInstantiationDirectiveStmt(stmt, info); break;

#if 0
          case PRAGMA_DECL:
            // cerr + "WARNING: unparsePragmaDeclStmt not implemented in SAGE 3 (exiting ...)" + endl;
            // This can't be an error since the A++ preprocessor currently processes #pragmas
            // (though we can ignore unparsing them)
            // ROSE_ABORT();

               unparsePragmaDeclStmt(stmt, info);
               break;
#endif

          case V_SgForInitStatement:                   unparseForInitStmt(stmt, info); break;

       // Comments could be attached to these statements
          case V_SgCatchStatementSeq:     // CATCH_STATEMENT_SEQ:
          case V_SgFunctionParameterList: // FUNCTION_PARAMETER_LIST:
          case V_SgCtorInitializerList:   // CTOR_INITIALIZER_LIST:
#if PRINT_DEVELOPER_WARNINGS
               printf ("Ignore these newly implemented cases (case of %s) \n",stmt->sage_class_name());
               printf ("WARNING: These cases must be implemented so that comments attached to them can be processed \n");
#endif
            // ROSE_ABORT();
               break;

          case V_SgNamespaceDeclarationStatement:      unparseNamespaceDeclarationStatement (stmt, info);      break;
          case V_SgNamespaceDefinitionStatement:       unparseNamespaceDefinitionStatement (stmt, info);       break;
          case V_SgNamespaceAliasDeclarationStatement: unparseNamespaceAliasDeclarationStatement (stmt, info); break;
          case V_SgUsingDirectiveStatement:            unparseUsingDirectiveStatement (stmt, info);            break;
          case V_SgUsingDeclarationStatement:          unparseUsingDeclarationStatement (stmt, info);          break;

       // DQ (3/2/2005): Added support for unparsing template class definitions.  This is the case: TEMPLATE_INST_DEFN_STMT
          case V_SgTemplateInstantiationDefn:          unparseClassDefnStmt(stmt, info); break;

       // case V_SgNullStatement:                      unparseNullStatement(stmt, info); break;

       // Liao, 6/13/2008: UPC support
          case V_SgUpcNotifyStatement:                  unparseUpcNotifyStatement(stmt, info); break;
          case V_SgUpcWaitStatement:                    unparseUpcWaitStatement(stmt, info); break;
          case V_SgUpcBarrierStatement:                 unparseUpcBarrierStatement(stmt, info); break;
          case V_SgUpcFenceStatement:                   unparseUpcFenceStatement(stmt, info); break;
          case V_SgUpcForAllStatement:                  unparseUpcForAllStatement(stmt, info);    break; 

       // Liao, 5/31/2009, add OpenMP support, TODO refactor some code to language independent part
          case V_SgOmpForStatement:                      unparseOmpForStatement(stmt, info); break;
          case V_SgOmpForSimdStatement:                  unparseOmpForSimdStatement(stmt, info); break;

       // DQ (4/16/2011): Added Java specific IR node until we support the Java specific unparsing.
          case V_SgJavaImportStatement:
               printf ("Unsupported Java specific unparsing for import statement \n");
            // unparseForStmt(stmt, info);
               break;

       // DQ (7/25/2014): Adding support for C11 static assertions.
          case V_SgStaticAssertionDeclaration:          unparseStaticAssertionDeclaration (stmt, info);    break;

       // DQ (8/17/2014): Adding support for Microsoft attributes.
          case V_SgMicrosoftAttributeDeclaration:       unparseMicrosoftAttributeDeclaration (stmt, info); break;

       // DQ 11/3/2014): Adding C++11 templated typedef declaration support.
          case V_SgTemplateTypedefDeclaration:          unparseTemplateTypedefDeclaration (stmt, info); break;

          case V_SgNonrealDecl:                         unparseNonrealDecl(stmt,info); break;

          default:
             {
               printf("CxxCodeGeneration_locatedNode::unparseLanguageSpecificStatement: Error: No handler for %s (variant: %d)\n",stmt->sage_class_name(), stmt->variantT());
               ROSE_ASSERT(false);
               break;
             }
        }

  // DQ (12/16/2008): Added support for unparsing statements around C++ specific statements
  // unparseAttachedPreprocessingInfo(stmt, info, PreprocessingInfo::after);

#if 0
  // This is done in: UnparseLanguageIndependentConstructs::unparseStatement()
  // DQ (12/5/2007): Check if the call to unparse any construct changes the scope stored in info.
     SgScopeStatement* scopeAfterUnparseStatement = info.get_current_scope();
     if (savedScope != scopeAfterUnparseStatement)
        {
          printf ("WARNING: scopes stored in SgUnparse_Info object have been changed \n");
        }
#endif

#if 0
     printf ("Leaving unparseLanguageSpecificStatement(): stmt = %p = %s \n",stmt,stmt->class_name().c_str());
#endif
#if 0
     curprint (string("/* Leaving of unparseLanguageSpecificStatement() stmt = ") + stmt->class_name() + " */ \n");
#endif

#if 0
     printf ("Leaving unparseLanguageSpecificStatement(): info.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
     printf ("Leaving unparseLanguageSpecificStatement(): info.SkipEnumDefinition()  = %s \n",(info.SkipEnumDefinition() == true) ? "true" : "false");
#endif

  // DQ (1/9/2014): These should have been setup to be the same.
     ROSE_ASSERT(info.SkipClassDefinition() == info.SkipEnumDefinition());
   }


#if 0
// DQ (8/13/2007): This has been moved to the base class (language independent code)

void
Unparse_ExprStmt::unparseNullStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
  // Nothing to do here! (unless we need a ";" or something)
     SgNullStatement* nullStatement = isSgNullStatement(stmt);
     ROSE_ASSERT(nullStatement != NULL);

  // Not much to do here except output a ";", not really required however.
  // curprint ( string(";";
   }
#endif


void
Unparse_ExprStmt::unparseNamespaceDeclarationStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
  // There is a SgNamespaceDefinition, but it is not unparsed except through the SgNamespaceDeclaration
     SgNamespaceDeclarationStatement* namespaceDeclaration = isSgNamespaceDeclarationStatement(stmt);
     ROSE_ASSERT (namespaceDeclaration != NULL);

#if 0
     printf("In unparseNamespaceDeclarationStatement(): stmt = %p = %s \n",stmt,stmt->class_name().c_str());
     curprint("/* In unparseNamespaceDeclarationStatement() */ ");
#endif

  // DQ (12/17/2014): Test for if we have unparsed partially using the token stream. 
  // If so then we don't want to unparse this syntax, if not then we require this syntax.
     bool saved_unparsedPartiallyUsingTokenStream = info.unparsedPartiallyUsingTokenStream();
     if (saved_unparsedPartiallyUsingTokenStream == false)
        {
       // DQ (8/12/2014): Adding support for inlined namespaces (C++11 support).
          if (namespaceDeclaration->get_isInlinedNamespace() == true)
             {
               curprint("inline ");
             }

          curprint("namespace ");

       // This can be an empty string (in the case of an unnamed namespace)
          SgName name = namespaceDeclaration->get_name();
          curprint(name.str());
        }
       else
        {
       // DQ (12/17/2014): Unparse a partial representation of the namespace.
          SgNamespaceDefinitionStatement* namespaceDefinition = namespaceDeclaration->get_definition();
          if (namespaceDefinition != NULL)
             {
#if 0
               curprint ("/* unparse start of SgNamespaceDeclarationStatement using tokens */");
#endif
               unparseStatementFromTokenStream (stmt, namespaceDefinition, e_token_subsequence_start, e_token_subsequence_start, info);
             }
        }

#if 0
     printf ("In unparseNamespaceDeclarationStatement(): namespaceDeclaration->get_definition() = %p \n",namespaceDeclaration->get_definition());
#endif

  // DQ (8/6/2012): test2010_24.C causes the new namespace alias support to generate namespaceDeclaration->get_definition() == NULL.
  // I don't know yet if this is reasonable, so output a warning for now.
  // unparseStatement(namespaceDeclaration->get_definition(),info);
     if (namespaceDeclaration->get_definition() != NULL)
        {
#if 0
          printf ("Calling unparseStatement() for namespaceDeclaration->get_definition() = %p \n",namespaceDeclaration->get_definition());
          curprint ("/* calling unparseNamespaceDefinitionStatement using tokens */");
#endif
       // DQ (8/19/2014): If we unparse the SgNamespaceDeclarationStatement, then we mean to unparse the SgNamespaceDefinition as well.
       // test2014_110.C demonstrates where the SgNamespaceDefinition has the wrong source position (from a header file) and
       // thus is not unparsed (filterd by the logic in unparseStatement()).  So try to call the correct unparse function directly.
       // unparseStatement(namespaceDeclaration->get_definition(),info);
          unparseNamespaceDefinitionStatement(namespaceDeclaration->get_definition(),info);
        }
       else
        {
          printf ("WARNING: I think we were expecting a definition associated with this SgNamespaceDeclarationStatement \n");
        }

#if 0
     printf("Leaving unparseNamespaceDeclarationStatement(): stmt = %p = %s \n",stmt,stmt->class_name().c_str());
     curprint("/* Leaving unparseNamespaceDeclarationStatement() */ ");
#endif

#if 0
  // DQ (5/19/2013): There should always be proper source file position infomation so this should not be required.
  // if (stmt->get_file_info()->isCompilerGenerated() == true && stmt->get_file_info()->isOutputInCodeGeneration() == true)
     if (stmt->get_file_info()->isCompilerGenerated() == false && stmt->get_file_info()->isOutputInCodeGeneration() == true)
        {
          printf ("WARNING: stmt = %p = %s stmt->get_file_info()->isCompilerGenerated() == true && stmt->get_file_info()->isOutputInCodeGeneration() == true \n",stmt,stmt->class_name().c_str());
          stmt->get_file_info()->display("Leaving unparseNamespaceDeclarationStatement(): debug");
        }
#endif
  // DQ (5/20/2013): I think this is the wrong assertion, see test2013_170.C.  Basically, line directives might make the logical file
  // name so that this declaration would not be output, but we want to output the declaration and set isOutputInCodeGeneration() == false 
  // to support the output of the declaration.
  // ROSE_ASSERT(stmt->get_file_info()->isCompilerGenerated() == true || stmt->get_file_info()->isOutputInCodeGeneration() == false);
   }


void
Unparse_ExprStmt::unparseNamespaceDefinitionStatement ( SgStatement* stmt, SgUnparse_Info & info )
   {
     ROSE_ASSERT (stmt != NULL);
     SgNamespaceDefinitionStatement* namespaceDefinition = isSgNamespaceDefinitionStatement(stmt);
     ROSE_ASSERT (namespaceDefinition != NULL);

#if 0
     printf ("In unparseNamespaceDefinitionStatement() \n");
     curprint("/* In unparseNamespaceDefinitionStatement() */ ");
#endif

#if OUTPUT_HIDDEN_LIST_DATA
     outputHiddenListData (namespaceDefinition);
#endif

     SgUnparse_Info ninfo(info);

  // DQ (11/6/2004): Added support for saving current namespace!
     ROSE_ASSERT(namespaceDefinition->get_namespaceDeclaration() != NULL);
     SgNamespaceDeclarationStatement *saved_namespace = ninfo.get_current_namespace();

#if 0
     if (saved_namespace != NULL)
          printf ("In unparseNamespaceDefinitionStatement(): saved_namespace = %p = %s \n",saved_namespace,saved_namespace->class_name().c_str());
#endif

  // DQ (6/13/2007): Set to null before resetting to non-null value 
     ninfo.set_current_namespace(NULL);
     ninfo.set_current_namespace(namespaceDefinition->get_namespaceDeclaration());

  // DQ (12/17/2014): Test for if we have unparsed partially using the token stream. 
  // If so then we don't want to unparse this syntax, if not then we require this syntax.
     bool saved_unparsedPartiallyUsingTokenStream = info.unparsedPartiallyUsingTokenStream();
     if (saved_unparsedPartiallyUsingTokenStream == false)
        {
          unp->cur.format(namespaceDefinition, info, FORMAT_BEFORE_BASIC_BLOCK2);
          curprint("{");
          unp->cur.format(namespaceDefinition, info, FORMAT_AFTER_BASIC_BLOCK2);
        }
       else
        {
          unparseStatementFromTokenStream (stmt, e_token_subsequence_start, e_token_subsequence_start, info);
        }

     SgStatement* last_stmt = NULL;

  // unparse all the declarations
     SgDeclarationStatementPtrList & statementList = namespaceDefinition->get_declarations();
     SgDeclarationStatementPtrList::iterator statementIterator = statementList.begin();
     while ( statementIterator != statementList.end() )
        {
          SgStatement* currentStatement = *statementIterator;
          ROSE_ASSERT(currentStatement != NULL);

       // DQ (11/6/2004): use ninfo instead of info for nested declarations in namespace
          unparseStatement(currentStatement, ninfo);

       // DQ (12/18/2014): Save the last statement so that we can use the trailing token stream if using the token-based unparsing.
          last_stmt = currentStatement;

       // Go to the next statement
          statementIterator++;
        }

#if 0
  // DQ (12/22/2014): NOTE: last_stmt might have to be based on token stream sugsequence availability.
     printf ("NOTE: In unparseNamespaceDefinitionStatement(): last_stmt might have to be based on token stream subsequence availability. \n");
#endif

     if (saved_unparsedPartiallyUsingTokenStream == false)
        {
       // DQ (3/17/2005): This helps handle cases such as void foo () { #include "constant_code.h" }
          unparseAttachedPreprocessingInfo(namespaceDefinition, info, PreprocessingInfo::inside);

          unp->cur.format(namespaceDefinition, info, FORMAT_BEFORE_BASIC_BLOCK2);
          curprint("}\n");
          unp->cur.format(namespaceDefinition, info, FORMAT_AFTER_BASIC_BLOCK2);
        }
       else
        {
#if 0
          printf ("unparse end of SgNamespaceDefinitionStatement: info.unparsedPartiallyUsingTokenStream() = %s last_stmt = %p \n",info.unparsedPartiallyUsingTokenStream() ? "true" : "false",last_stmt);
          if (last_stmt != NULL)
             {
               printf ("   --- last_stmt = %p = %s \n",last_stmt,last_stmt->class_name().c_str());
             }
#endif

       // unparseStatementFromTokenStream (stmt, e_token_subsequence_end, e_token_subsequence_end);
          if (last_stmt != NULL)
             {
#if 0
               curprint("/* last_stmt != NULL: trailing whitespace from the last statement */ ");
#endif
            // Unparse the trailing white space of the last statement.
               unparseStatementFromTokenStream (last_stmt, stmt, e_trailing_whitespace_start, e_token_subsequence_end, info);
#if 0
               curprint("/* last_stmt != NULL: unparse the } */ ");
#endif
            // Unparse the final "}" for the SgNamespaceDefinitionStatement.
               unparseStatementFromTokenStream (stmt, e_token_subsequence_end, e_token_subsequence_end, info);
             }
            else
             {
#if 0
               curprint("/* last_stmt == NULL */ ");
#endif
               unparseStatementFromTokenStream (stmt, e_token_subsequence_end, e_token_subsequence_end, info);
             }
        }

  // DQ (11/3/2007): Since "ninfo" will go out of scope shortly, this is not significant.
  // DQ (6/13/2007): Set to null before resetting to non-null value 
  // DQ (11/6/2004): Added support for saving current namespace!
     ninfo.set_current_namespace(NULL);
     ninfo.set_current_namespace(saved_namespace);

#if 0
     if (saved_namespace != NULL)
          printf ("In unparseNamespaceDefinitionStatement(): reset saved_namespace = %p = %s \n",saved_namespace,saved_namespace->class_name().c_str());
#endif
#if 0
     curprint("/* Leaving unparseNamespaceDefinitionStatement() */ ");
#endif
   }


void
Unparse_ExprStmt::unparseNamespaceAliasDeclarationStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgNamespaceAliasDeclarationStatement* namespaceAliasDeclaration = isSgNamespaceAliasDeclarationStatement(stmt);
     ROSE_ASSERT (namespaceAliasDeclaration != NULL);

     curprint ("\nnamespace ");
     curprint ( namespaceAliasDeclaration->get_name().str());
     curprint (" = ");
     ROSE_ASSERT(namespaceAliasDeclaration->get_namespaceDeclaration() != NULL);

  // DQ (7/8/2014): Support for new name qualification.
     SgUnparse_Info tmp_info(info);
     tmp_info.set_name_qualification_length(namespaceAliasDeclaration->get_name_qualification_length());
     tmp_info.set_global_qualification_required(namespaceAliasDeclaration->get_global_qualification_required());

#if 0
  // DQ (7/8/2014): Compute the name qualification separately.
     curprint ( usingDirective->get_namespaceDeclaration()->get_name().str();
     curprint ( usingDirective->get_namespaceDeclaration()->get_qualified_name().str();
#endif

  // DQ (7/8/2014): We store the information about the name qualification in the reference to the namespace, 
  // and not in the namespace.  This is so that multiple references to the namespace can be supported using 
  // different levels of qualification.
     SgName nameQualifier = namespaceAliasDeclaration->get_qualified_name_prefix();

#if 0
     printf ("In unparseNamespaceAliasDeclarationStatement(): nameQualifier = %s \n",nameQualifier.str());
#endif
     curprint ( nameQualifier);

  // DQ (4/9/2018): Added support for aliases of namespace alias namespaces.
  // curprint ( namespaceAliasDeclaration->get_namespaceDeclaration()->get_name().str());
     if (namespaceAliasDeclaration->get_is_alias_for_another_namespace_alias() == false)
        {
          curprint ( namespaceAliasDeclaration->get_namespaceDeclaration()->get_name().str());
        }
       else
        {
       // DQ (4/9/2018): This is the case of an alis to a namespace alias (see Cxx_tests/test2018_26.C).
          curprint ( namespaceAliasDeclaration->get_namespaceAliasDeclaration()->get_name().str());
        }

     curprint ( string(";\n"));
   }


void
Unparse_ExprStmt::unparseUsingDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgUsingDirectiveStatement* usingDirective = isSgUsingDirectiveStatement(stmt);
     ROSE_ASSERT (usingDirective != NULL);

  // DQ (8/26/2004): This should be "using namespace" instead of just "using"
     curprint ( string("\nusing namespace "));
     ROSE_ASSERT(usingDirective->get_namespaceDeclaration() != NULL);
#if 0
     printf ("In unparseUsingDirectiveStatement using namespace = %s qualified name = %s \n",
          usingDirective->get_namespaceDeclaration()->get_name().str(),
          usingDirective->get_namespaceDeclaration()->get_qualified_name().str());
#endif

  // DQ (5/12/2011): Support for new name qualification.
     SgUnparse_Info tmp_info(info);
     tmp_info.set_name_qualification_length(usingDirective->get_name_qualification_length());
     tmp_info.set_global_qualification_required(usingDirective->get_global_qualification_required());

  // DQ (6/7/2007): Compute the name qualification separately.
  // curprint ( usingDirective->get_namespaceDeclaration()->get_name().str();
  // curprint ( usingDirective->get_namespaceDeclaration()->get_qualified_name().str();

  // DQ (5/12/2011): We store the information about the name qualification in the reference to the namespace, 
  // and not in the namespace.  This is so that multiple references to the namespace can be supported using 
  // different levels of qualification.
  // SgName nameQualifier = unp->u_name->generateNameQualifier( usingDirective->get_namespaceDeclaration() , info );
  // SgName nameQualifier = unp->u_name->generateNameQualifier( usingDirective->get_namespaceDeclaration() , tmp_info );
     SgName nameQualifier = usingDirective->get_qualified_name_prefix();

  // printf ("In unparseUsingDirectiveStatement(): nameQualifier = %s \n",nameQualifier.str());
     curprint ( nameQualifier);

     curprint ( usingDirective->get_namespaceDeclaration()->get_name().str());

     curprint ( string(";\n"));
   }

void
Unparse_ExprStmt::unparseUsingDeclarationStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgUsingDeclarationStatement* usingDeclaration = isSgUsingDeclarationStatement(stmt);
     ROSE_ASSERT (usingDeclaration != NULL);

  // DQ (1/30/2019): This code is required for the output of the access specifier 
  // (public, protected, private) and applies only within classes.  Use get_parent()
  // instead of get_scope() since we are looking for the structural position of the 
  // declaration (is it is a class).
     SgClassDefinition *classDefinition = isSgClassDefinition(usingDeclaration->get_parent());
     if (classDefinition != NULL)
        {
       // Don't output an access specifier in this is a struct or union!
       // printf ("Don't output an access specifier in this is a struct or union! \n");

       // DQ and PC (6/1/2006): Added Peter's suggested fixes to support unparsing fully qualified names (supporting auto-documentation).
       // if (classDefinition->get_declaration()->get_class_type() == SgClassDeclaration::e_class)
          if (classDefinition->get_declaration()->get_class_type() == SgClassDeclaration::e_class && !info.skipCheckAccess())
               info.set_CheckAccess();
       // inClass = true;
       // inCname = isSgClassDefinition(vardecl_stmt->get_parent())->get_declaration()->get_name();
        }

#if 0
     curprint( "\n/* Calling printSpecifier() */ ");
#endif

  // DQ (1/30/2019): Adding support to output the access specifier when we are in a class definition.
  // info.set_CheckAccess();
     unp->u_sage->printSpecifier(usingDeclaration, info);
     info.unset_CheckAccess();

#if 0
     curprint( "\n/* DONE: Calling printSpecifier() */ ");
#endif

     curprint ( string("\nusing "));

  // DQ (9/11/2004): We only save the declaration and get the name by unparsing the declaration
  // Might have to setup info1 to only output the name that we want!
     SgUnparse_Info info1(info);
  // info1.unset_CheckAccess();
  // info1.set_PrintName();
     info1.unset_isWithType();

  // DQ (7/21/2005): Either one or the other of these are valid. A using declaration can have either 
  // a reference to a declaration (SgDeclarationStatement) or a variable or enum file name (SgInitializedName).
     SgDeclarationStatement* declarationStatement = usingDeclaration->get_declaration();
     SgInitializedName*      initializedName      = usingDeclaration->get_initializedName();

  // Enforce that only one is a vaild pointer
     ROSE_ASSERT(declarationStatement != NULL || initializedName != NULL);
     ROSE_ASSERT(declarationStatement == NULL || initializedName == NULL);

  // printf ("In unparseUsingDeclarationStatement(): declarationStatement = %s \n",declarationStatement->sage_class_name());
  // unparseStatement(declarationStatement,info1);

#if 1
  // DQ (5/12/2011): Support for new name qualification.
     SgUnparse_Info tmp_info1(info);
     tmp_info1.set_name_qualification_length(usingDeclaration->get_name_qualification_length());
     tmp_info1.set_global_qualification_required(usingDeclaration->get_global_qualification_required());
#endif

     if (initializedName != NULL)
        {
       // DQ (5/12/2011): Support for new name qualification.
       // DQ (6/5/2011): This case is demonstrated by test2005_114.C.
#if 0
          printf ("I think that this case might not be used...(or we need a test case for it) \n");
          ROSE_ASSERT(false);
#endif
          SgName nameQualifier = usingDeclaration->get_qualified_name_prefix();

          curprint(nameQualifier.str());
          curprint(initializedName->get_name().str());
        }

     if (declarationStatement != NULL)
        {
          SgName nameQualifier = usingDeclaration->get_qualified_name_prefix();
          curprint(nameQualifier.str());

       // Handle the different sorts of declarations explicitly since the existing unparse functions for 
       // declarations are not setup for what the using declaration unparser requires.
          switch (declarationStatement->variantT())
             {
               case V_SgVariableDeclaration:
                  {
                 // DQ (7/21/2005): Now that we have added support for SgUsingDeclarations to reference a 
                 // SgDeclarationStatment or a SgInitializedName we could have the SgVariableDeclaration
                 // be implemented to more precisely reference the variable directly instead of the 
                 // declaration where the variable was defined.

                 // get the name of the variable in the declaration
                    SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(declarationStatement);
                    ROSE_ASSERT(variableDeclaration != NULL);
                    SgInitializedNamePtrList & variableList = variableDeclaration->get_variables();
                 // using directives must be issued separately for each variable!
                    ROSE_ASSERT(variableList.size() == 1);
                    SgInitializedName* initializedName = *(variableList.begin());
                    unparseAttachedPreprocessingInfo(initializedName, info, PreprocessingInfo::before);
                    ROSE_ASSERT(initializedName != NULL);
                    SgName variableName = initializedName->get_name();
                    curprint ( variableName.str());
                    break;
                  }

               case V_SgVariableDefinition:
                  {
                 // DQ (6/18/2006): Associated declaration can be a SgVariableDefinition,
                 // get the name of the variable using the variable definition
                    SgVariableDefinition* variableDefinition = isSgVariableDefinition(declarationStatement);
                    ROSE_ASSERT(variableDefinition != NULL);
                    SgInitializedName* initializedName = variableDefinition->get_vardefn();
                    ROSE_ASSERT(initializedName != NULL);
                    SgName variableName = initializedName->get_name();
                    curprint ( variableName.str());
                    break;
                  }

               case V_SgNamespaceDeclarationStatement:
                  {
                    SgNamespaceDeclarationStatement* namespaceDeclaration = isSgNamespaceDeclarationStatement(declarationStatement);
                    ROSE_ASSERT(namespaceDeclaration != NULL);
                    SgName namespaceName = namespaceDeclaration->get_name();
                    curprint ( namespaceName.str());
                    break;
                  }           

               case V_SgFunctionDeclaration:
                  {
                    SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(declarationStatement);
                    ROSE_ASSERT(functionDeclaration != NULL);
                    SgName functionName = functionDeclaration->get_name();
                    curprint ( functionName.str());
                    break;
                  }

               case V_SgTemplateInstantiationMemberFunctionDecl:
               case V_SgMemberFunctionDeclaration:
                  {
                    SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(declarationStatement);
                    ROSE_ASSERT(memberFunctionDeclaration != NULL);
                    SgName memberFunctionName = memberFunctionDeclaration->get_name();
                    curprint ( memberFunctionName.str());
                    break;
                  }

               case V_SgClassDeclaration:
                  {
                    SgClassDeclaration* classDeclaration = isSgClassDeclaration(declarationStatement);
                    ROSE_ASSERT(classDeclaration != NULL);
                    SgName className = classDeclaration->get_name();

                 // DQ (1/11/2019): Adding support for C++11 inheriting constructor.
                    if (usingDeclaration->get_is_inheriting_constructor() == true)
                       {
#if 0
                         printf ("Adding support for C++11 inheriting constructor: className = %s \n",className.str());
#endif
                         curprint(className.str());
                         curprint("::");
                       }

                    curprint (className.str());
                    break;
                  }

               case V_SgTypedefDeclaration:
                  {
                    SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(declarationStatement);
                    ROSE_ASSERT(typedefDeclaration != NULL);
                    SgName typedefName = typedefDeclaration->get_name();
                    curprint ( typedefName.str());
                    break;
                  }

            // DQ (12/29/2011): Added more template support for declarations.
            // I don't know if this case has to be separated out from case V_SgClassDeclaration
               case V_SgTemplateClassDeclaration:
                  {
                    SgTemplateClassDeclaration* templateDeclaration = isSgTemplateClassDeclaration(declarationStatement);
                    ROSE_ASSERT(templateDeclaration != NULL);
                    SgName templateName = templateDeclaration->get_name();
                    curprint ( templateName.str());
                    break;
                  }

            // DQ (12/29/2011): Added more template support for declarations.
            // I don't know if these case have to be separated out from case V_SgFunctionDeclaration
               case V_SgTemplateFunctionDeclaration:
               case V_SgTemplateMemberFunctionDeclaration:
                  {
                    SgTemplateFunctionDeclaration* templateDeclaration = isSgTemplateFunctionDeclaration(declarationStatement);

                 // DQ (1/19/2017): Modify this to be a warning instead of an assertion for the GNU 6.1 compiler.
                 // This is a problem for test2011_121.C and several other test codes in the unparseToString_tests directory.
                 // ROSE_ASSERT(templateDeclaration != NULL);
                    if (templateDeclaration != NULL)
                      {
                        SgName templateName = templateDeclaration->get_name();
                        curprint ( templateName.str());
                      }
                     else
                      {
                        printf ("WARNING: In unparseUsingDeclarationStatement(): declarationStatement = %s templateDeclaration == NULL \n",declarationStatement->sage_class_name());
                      }
                    break;
                  }

            // DQ (6/11/2011): Added support for new template IR nodes.
               case V_SgTemplateDeclaration:
                  {
                 // DQ (9/12/2004): This function outputs the default template name which is not correct, we need 
                 // to get more information out of EDG about the template name if we are to get this correct in 
                 // the future.  This could (and likely will) cause generated code to not compile, but I will 
                 // worry about that after we can compile Kull.
                    SgTemplateDeclaration* templateDeclaration = isSgTemplateDeclaration(declarationStatement);
                    ROSE_ASSERT(templateDeclaration != NULL);
                    SgName templateName = templateDeclaration->get_name();
                    curprint (templateName.str());
                    break;
                  }

            // DQ (5/22/2007): Added support for enum types in using declaration (test2007_50.C).
               case V_SgEnumDeclaration:
                  {
                    SgEnumDeclaration* enumDeclaration = isSgEnumDeclaration(declarationStatement);
                    ROSE_ASSERT(enumDeclaration != NULL);
                    SgName enumName = enumDeclaration->get_name();
                    curprint (enumName.str());
                    break;
                  }

            // DQ (3/8/2017): Added support for SgTemplateTypedefDeclaration IR nodes in using declaration (Cxx11_tests/test20017_03.C).
               case V_SgTemplateTypedefDeclaration:
                  {
                    SgTemplateTypedefDeclaration* templateTypedefDeclaration = isSgTemplateTypedefDeclaration(declarationStatement);
                    ROSE_ASSERT(templateTypedefDeclaration != NULL);
                    SgName name = templateTypedefDeclaration->get_name();
                    curprint (name.str());
                    break;
                  }

               default:
                  {
                    printf ("Default reached in unparseUsingDeclarationStatement(): case is not implemented for %s \n",declarationStatement->sage_class_name());
                    ROSE_ASSERT(false);
                  }
             }
        }

     curprint ( string(";\n"));
   }

void 
Unparse_ExprStmt::unparseTemplateInstantiationDirectiveStmt (SgStatement* stmt, SgUnparse_Info& info)
   {
  // DQ (4/16/2005): Added support for explicit template instatination directives
     SgTemplateInstantiationDirectiveStatement* templateInstantiationDirective = isSgTemplateInstantiationDirectiveStatement(stmt);
     ROSE_ASSERT(templateInstantiationDirective != NULL);

     SgDeclarationStatement* declarationStatement = templateInstantiationDirective->get_declaration();
     ROSE_ASSERT(declarationStatement != NULL);

#if 0
     printf ("Inside of unparseTemplateInstantiationDirectiveStmt declaration = %s \n",declarationStatement->class_name().c_str());
     curprint("/* unparseTemplateInstantiationDirectiveStmt(): explicit template instantiation */ \n ");
#endif

  // curprint ( string("template ";

  // DQ (8/2/2014): Added support for C++ directive to surpress template instantiation. 
     if (templateInstantiationDirective->get_do_not_instantiate() == true)
        {
       // syntax for C++11 "do not instantiate" directive.
          curprint ("extern ");
        }

     ROSE_ASSERT(declarationStatement->get_file_info() != NULL);
  // declarationStatement->get_file_info()->display("Location of SgTemplateInstantiationDirectiveStatement \n");

  // unparseStatement(declaration,info);
     switch (declarationStatement->variantT())
        {
          case V_SgTemplateInstantiationDecl:
             {
#if 0
               printf ("Unparsing of SgTemplateInstantiationDecl in unparseTemplateInstantiationDirectiveStmt ... \n");
#endif
            // unparseClassDeclStmt(declarationStatement,info);
            // unparseTemplateInstantiationDeclStmt(declarationStatement,info);
               SgClassDeclaration* classDeclaration = isSgClassDeclaration(declarationStatement);
               ROSE_ASSERT(classDeclaration != NULL);

#if 0
               printf ("classDeclaration = %p = %s \n",classDeclaration,classDeclaration->class_name().c_str());
               printf ("classDeclaration->get_parent() = %p = %s \n",classDeclaration->get_parent(),classDeclaration->get_parent()->class_name().c_str());
#endif
            // DQ (8/29/2005): "template" keyword now output by Unparse_ExprStmt::outputTemplateSpecializationSpecifier()
            // curprint ( string("template ";
#if 1
            // DQ (8/19/2014): Original code.
               unparseClassDeclStmt(classDeclaration,info);
#else
            // DQ (8/19/2014): New code.
               unparseTemplateInstantiationDeclStmt(declarationStatement,info);
#endif
               break;
             }

          case V_SgTemplateInstantiationFunctionDecl:
             {
            // printf ("Unparsing of SgTemplateInstantiationFunctionDecl in unparseTemplateInstantiationDirectiveStmt ... \n");
            // ROSE_ASSERT(false);
               SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(declarationStatement);
               ROSE_ASSERT(functionDeclaration != NULL);
            // DQ (8/29/2005): "template" keyword now output by Unparse_ExprStmt::outputTemplateSpecializationSpecifier()
            // curprint ( string("template ";
               unparseFuncDeclStmt(functionDeclaration,info);
               break;
             }

          case V_SgTemplateInstantiationMemberFunctionDecl:
             {
            // printf ("Unparsing of SgTemplateInstantiationMemberFunctionDecl in unparseTemplateInstantiationDirectiveStmt ... \n");
            // ROSE_ASSERT(false);
               SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(declarationStatement);
               ROSE_ASSERT(memberFunctionDeclaration != NULL);

            // DQ (5/31/2005): for now we will only output directives for template member functions and not non-template 
            // member functions.  In the case of a template class NOT output as a specialization then the template 
            // instantiation directive for a non-templated member function is allows (likely is just instatiates the class).
               if (memberFunctionDeclaration->isTemplateFunction() == true)
                  {
                 // DQ (8/29/2005): "template" keyword now output by Unparse_ExprStmt::outputTemplateSpecializationSpecifier()
                 // curprint ( string("template ";
#if 0
                    printf ("memberFunctionDeclaration = %p = %s = %s \n",
                          memberFunctionDeclaration,
                          memberFunctionDeclaration->class_name().c_str(),
                          memberFunctionDeclaration->get_name().str());
#endif
                    unparseMFuncDeclStmt(memberFunctionDeclaration,info);
                  }
                 else
                  {
                 // It seems that if the class declaration is not specialized then the non-member function template 
                 // instantiation directive is allowed. But we don't at this point know if the class declaration has 
                 // been output so skip all template instantiations of non-template member functions (in general).
                 // Issue a warning message for now!
#if PRINT_DEVELOPER_WARNINGS
                    printf ("Warning: Skipping output of directived to build non-template member functions! \n");
                    curprint ( string("\n/* Warning: Skipping output of directived to build non-template member functions! */"));
#endif
                  }
               break;
             }

          case V_SgVariableDeclaration:
             {
               printf ("Unparsing of SgVariableDeclaration in unparseTemplateInstantiationDirectiveStmt not implemented \n");

               SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(declarationStatement);
               ROSE_ASSERT(variableDeclaration != NULL);

               unparseVarDeclStmt (variableDeclaration,info);
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // DQ (8/13/2005): Added this case because it comes up in compiling KULL (KULL/src/transport/CommonMC/Particle/mcapm.cc)
          case V_SgMemberFunctionDeclaration:
             {
            // DQ (8/31/2005): This should be an error now!  Template instantiations never generate
            // a SgMemberFunctionDeclaration and always generate a SgTemplateInstantiationMemberFunctionDecl
               printf ("Error: SgMemberFunctionDeclaration case found in unparseTemplateInstantiationDirectiveStmt ... (exiting) \n");
               ROSE_ASSERT(false);
#if 0
               SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(declarationStatement);
               ROSE_ASSERT(memberFunctionDeclaration != NULL);

               ROSE_ASSERT (memberFunctionDeclaration->isTemplateFunction() == false);

            // curprint ( string("\n/* Skipped unparsing of SgMemberFunctionDeclaration in unparseTemplateInstantiationDirectiveStmt untested */ \n ";
            // curprint ( string("template ";
            // unparseMFuncDeclStmt(memberFunctionDeclaration,info);
#endif
               break;
             }

       // DQ (2/2/2018): Added case for currently unimplemented unparsing support for template variable declarations.
          case V_SgTemplateVariableDeclaration:
             {
            // printf ("Unparsing of SgTemplateVariableDeclaration in unparseTemplateInstantiationDirectiveStmt not implemented \n");
            // ROSE_ASSERT(false);

               SgTemplateVariableDeclaration* variableDeclaration = isSgTemplateVariableDeclaration(declarationStatement);
               ROSE_ASSERT(variableDeclaration != NULL);

               unparseTemplateVariableDeclStmt(variableDeclaration,info);
#if 0
               printf ("Unparsing of SgTemplateVariableDeclaration in unparseTemplateInstantiationDirectiveStmt not implemented \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

          default:
             {
               printf ("Error: default reached in switch (declarationStatement = %s) \n",declarationStatement->class_name().c_str());
               ROSE_ASSERT(false);
             }
        }

#if 0
     printf ("Leaving unparseTemplateInstantiationDirectiveStmt declaration = %s \n",declarationStatement->class_name().c_str());
     curprint("/* Leaving unparseTemplateInstantiationDirectiveStmt(): explicit template instantiation */ \n ");
#endif
   }


void
Unparse_ExprStmt::unparseTemplateInstantiationDeclStmt (SgStatement* stmt, SgUnparse_Info& info)
   {
  // DQ (2/29/2004): New function to support templates
     SgTemplateInstantiationDecl* templateInstantiationDeclaration = isSgTemplateInstantiationDecl(stmt);
     ROSE_ASSERT(templateInstantiationDeclaration != NULL);

     SgClassDeclaration* classDeclaration = isSgClassDeclaration(templateInstantiationDeclaration);
     ROSE_ASSERT(classDeclaration != NULL);

#if 0
     printf ("Inside of unparseTemplateInstantiationDeclStmt() stmt = %p = %s \n",stmt,stmt->class_name().c_str());
     curprint("/* Output in curprint in Unparse_ExprStmt::unparseTemplateInstantiationDeclStmt() */");
#endif
#if OUTPUT_DEBUGGING_CLASS_NAME || 0
     printf ("Inside of unparseTemplateInstantiationDeclStmt() stmt = %p/%p name = %s  templateName = %s transformed = %s/%s prototype = %s compiler-generated = %s compiler-generated and marked for output = %s \n",
          classDeclaration,templateInstantiationDeclaration,
          templateInstantiationDeclaration->get_name().str(),
          templateInstantiationDeclaration->get_templateName().str(),
          isTransformed (templateInstantiationDeclaration) ? "true" : "false",
          (templateInstantiationDeclaration->get_file_info()->isTransformation() == true) ? "true" : "false",
          (templateInstantiationDeclaration->get_definition() == NULL) ? "true" : "false",
          (templateInstantiationDeclaration->get_file_info()->isCompilerGenerated() == true) ? "true" : "false",
          (templateInstantiationDeclaration->get_file_info()->isCompilerGeneratedNodeToBeUnparsed() == true) ? "true" : "false");
#endif

  // Call the unparse function for a class declaration
  // If the template has not been modified then don't output the template specialization 
  // (force the backend compiler to handle the template specialization and instantiation).

  // DQ (8/2/2012): I think we don't need to always outoput the template instantiation.
  // However, we do want to test this so that we make sure that when we do we can.
  // Test code test2012_155.C demonstrates that we need to output the template arguments 
  // with qualification.  I'm not sure how this has been missed before.
  // The fix will be to build up the name from the template name and template arguments so 
  // that we can support the use of any qualified names that might be associated with these.

#if 1
     bool outputClassTemplateInstantiation = true;
#else
     printf ("In unparseTemplateInstantiationDeclStmt(): Skipping the unconditional output of the template instantiation for this class. \n");
     bool outputClassTemplateInstantiation = false;
#endif

     if (isTransformed (templateInstantiationDeclaration) == true )
        {
       // If the template has been transformed then we have to output the special version 
       // of the template as a template specialization.

       // If a class template has been modified then we need to make sure that all the 
       //      static data members, and 
       //      member functions 
       // are instantiated (on the next pass through the prelinker). The process should 
       // involve a call to the EDG function:
       //      static void set_instantiation_required_for_template_class_members (a_type_ptr class_type)
       // I am not currently sure how to make this happen, but it should involve the *.ti 
       // files (I guess).
#if 0
          printf ("In unparseTemplateInstantiationDeclStmt(): Calling unparseClassDeclStmt to unparse the SgTemplateInstantiationDecl \n");
#endif
#if 0
       // This case is not supported if member functions or static data members are present in the class
       // (could generate code which would compile but not link!).
          printf ("\n\n");
          printf ("WARNING: Transformations on templated classes can currently generate code which \n");
          printf ("         will not compile, since member function and static data members are not \n");
          printf ("         presently generated for the new specialization of the transformed template class. \n");
          printf ("\n");
#endif

#if 0
       // DQ (5/8/2004): Make this an explicit specialization (using the newer C++ syntax to support this)
       // DQ (3/2/2005): Uncommented output of "template <>"
          bool locatedInNamespace = isSgNamespaceDefinitionStatement(classDeclaration->get_scope()) != NULL;
          printf ("locatedInNamespace = %s \n",locatedInNamespace ? "true" : "false");

          if (locatedInNamespace == true)
             {
               curprint ( string("namespace std {"));
             }

       // curprint ( string("\n /* unparseTemplateInstantiationDeclStmt */ ";

       // DQ (8/29/2005): This is now output by the Unparse_ExprStmt::outputTemplateSpecializationSpecifier() member function
       // curprint ( string("\ntemplate <> \n";

          unparseClassDeclStmt(classDeclaration,info);

          if (locatedInNamespace == true)
             {
               curprint ( string("   }"));
             }
#endif

       // DQ (8/19/2005): If transformed then always output the template instantiation (no matter where it is from)
          outputClassTemplateInstantiation = true;
        }
       else
        {
          if ( templateInstantiationDeclaration->get_file_info()->isOutputInCodeGeneration() == true )
             {
#if PRINT_DEVELOPER_WARNINGS || 0
               printf ("In unparseTemplateInstantiationDeclStmt(): Class template is marked for output in the current source file. \n");
#endif
               outputClassTemplateInstantiation = true;
             }
            else
             {
#if PRINT_DEVELOPER_WARNINGS || 0
               printf ("In unparseTemplateInstantiationDeclStmt(): Class template is NOT marked for output in the current source file. \n");
#endif
             }
#if 0
       // If not transformed then we only want to output the template (and usually only the 
       // name of the template specialization) in variable declarations and the like.
       // These locations control the output of the template specialization explicitly 
       // through the SgUnparse_Info object (default for outputClassTemplateName is false). 
       // printf ("info.outputClassTemplateName() = %s \n",info.outputClassTemplateName() ? "true" : "false");
          if ( info.outputClassTemplateName() == true )
             {
            // printf ("Calling unparseClassDeclStmt to unparse the SgTemplateInstantiationDecl \n");

            // DQ (9/8/2004):
            // The unparseClassDeclStmt does much more that what we require and is not setup to 
            // handle templates well, it generally works well if the template name required does 
            // not need any qualification (e.g. std::templateName<int>).  Thus don't use the 
            // unparseClassDeclStmt and just output the qualified template name.
            // unparseClassDeclStmt(classDeclaration,info);

            // Output the qualified template name
               curprint ( templateInstantiationDeclaration->get_qualified_name().str());
             }
#if 0
            else
             {
               printf ("Skipping call to unparse the SgTemplateInstantiationDecl = %p \n",
                    templateInstantiationDeclaration);
             }
#endif
#endif
        }

#if 0
     printf ("In unparseTemplateInstantiationDeclStmt(): outputClassTemplateInstantiation = %s \n",outputClassTemplateInstantiation ? "true" : "false");
#endif

     if (outputClassTemplateInstantiation == true)
        {
       // DQ (5/8/2004): Make this an explicit specialization (using the newer C++ syntax to support this)
       // DQ (3/2/2005): Uncommented output of "template <>"

       // DQ (9/1/2005): This is a temporary fix to handle a bug in g++ (3.3.x and 3.4.x) which 
       // requires class specialization declared in a namespace to be output in a namespace instead 
       // of with the alternative proper namespace name qualifiers.
       // Note: If this is in a nested namespace then it might be that this will not work 
       // (since namespace names can't be name qualified).  A loop over the nested namespaces might 
       // be required to handle this case, better yet would be to transform the AST in a special pass
       // to fix this up to handle backend compiler limitations (as we currently do for other backend 
       // compiler bugs).
          SgNamespaceDefinitionStatement* namespaceDefinition = isSgNamespaceDefinitionStatement(classDeclaration->get_scope());
          bool locatedInNamespace = (namespaceDefinition != NULL);

       // DQ (8/2/2012): Set this to be always false for the new EDG 4.x work (see test2004_112.C).
#if 0
          printf ("In unparseTemplateInstantiationDeclStmt(): Setting locatedInNamespace (work around from 2005) to false \n");
#endif
          locatedInNamespace = false;

       // printf ("locatedInNamespace = %s \n",locatedInNamespace ? "true" : "false");
          if (locatedInNamespace == true)
             {
               string namespaceName = namespaceDefinition->get_namespaceDeclaration()->get_name().str();
            // curprint ( string("namespace std /* temporary fix for g++ bug in namespace name qualification */ \n   {";
               curprint ( string("namespace " ) + namespaceName + " /* temporary fix for g++ bug in namespace name qualification */ \n   {");
             }
#if 0
          printf ("In unparseTemplateInstantiationDeclStmt(): Calling unparseClassDeclStmt() \n");
#endif
       // curprint ( string("\n /* unparseTemplateInstantiationDeclStmt */ ";
       // DQ (8/29/2005): This is now output by the Unparse_ExprStmt::outputTemplateSpecializationSpecifier() member function
       // curprint ( string("\ntemplate <> \n";
          unparseClassDeclStmt(classDeclaration,info);

          if (locatedInNamespace == true)
             {
               curprint("   }");
             }
        }
       else
        {
#if 0
          curprint(string("/* Skipped output of template class declaration (name = ") + templateInstantiationDeclaration->get_qualified_name().str() + ") */ \n");
#endif
       // If not transformed then we only want to output the template (and usually only the 
       // name of the template specialization) in variable declarations and the like.
       // These locations control the output of the template specialization explicitly 
       // through the SgUnparse_Info object (default for outputClassTemplateName is false). 
#if 0
          printf("info.outputClassTemplateName() = %s \n",info.outputClassTemplateName() ? "true" : "false");
#endif
          if ( info.outputClassTemplateName() == true )
             {
#if 0
               printf ("Calling unparseClassDeclStmt to unparse the SgTemplateInstantiationDecl \n");
#endif
            // DQ (9/8/2004):
            // The unparseClassDeclStmt does much more that what we require and is not setup to 
            // handle templates well, it generally works well if the template name required does 
            // not need any qualification (e.g. std::templateName<int>).  Thus don't use the 
            // unparseClassDeclStmt and just output the qualified template name.
            // unparseClassDeclStmt(classDeclaration,info);

            // Output the qualified template name
               curprint(templateInstantiationDeclaration->get_qualified_name().str());
             }
        }

#if 0
     printf ("Leaving of unparseTemplateInstantiationDeclStmt() \n");
#endif
   }



void
Unparse_ExprStmt::unparseTemplateInstantiationFunctionDeclStmt (SgStatement* stmt, SgUnparse_Info& info)
   {
  // DQ (6/8/2005): If this is an inlined function, we need to make sure that 
  // the function has not been used anywhere before where we output it here.

  // DQ (3/24/2004): New function to support templates
     SgTemplateInstantiationFunctionDecl* templateInstantiationFunctionDeclaration = isSgTemplateInstantiationFunctionDecl(stmt);
     ROSE_ASSERT(templateInstantiationFunctionDeclaration != NULL);
     ROSE_ASSERT(templateInstantiationFunctionDeclaration->get_file_info() != NULL);

     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(templateInstantiationFunctionDeclaration);

     ROSE_ASSERT(functionDeclaration != NULL);

#if 0
     printf ("Inside of Unparse_ExprStmt::unparseTemplateInstantiationFunctionDeclStmt() templateInstantiationFunctionDeclaration = %p \n",templateInstantiationFunctionDeclaration);
     printf ("   --- isTransformed (templateInstantiationFunctionDeclaration) = %s \n",isTransformed (templateInstantiationFunctionDeclaration) ? "true" : "false");
     printf ("   --- nondefining declaration = %p \n",templateInstantiationFunctionDeclaration->get_firstNondefiningDeclaration());
     printf ("   --- defining declaration    = %p \n",templateInstantiationFunctionDeclaration->get_definingDeclaration());
     curprint("/* In Unparse_ExprStmt::unparseTemplateInstantiationFunctionDeclStmt() */");
#endif

#if OUTPUT_DEBUGGING_FUNCTION_NAME || 0
     printf ("In unparseTemplateInstantiationFunctionDeclStmt() name = %s (qualified_name = %s)  transformed = %s prototype = %s static = %s friend = %s compiler generated = %s transformed = %s output = %s \n",
       // templateInstantiationFunctionDeclaration->get_name().str(),
          templateInstantiationFunctionDeclaration->get_name().str(),
          templateInstantiationFunctionDeclaration->get_qualified_name().str(),
          isTransformed (templateInstantiationFunctionDeclaration) ? "true" : "false",
          (templateInstantiationFunctionDeclaration->get_definition() == NULL) ? "true" : "false",
          (templateInstantiationFunctionDeclaration->get_declarationModifier().get_storageModifier().isStatic() == true) ? "true" : "false",
          (templateInstantiationFunctionDeclaration->get_declarationModifier().isFriend() == true) ? "true" : "false",
          (templateInstantiationFunctionDeclaration->get_file_info()->isCompilerGenerated() == true) ? "true" : "false",
          (templateInstantiationFunctionDeclaration->get_file_info()->isTransformation() == true) ? "true" : "false",
          (templateInstantiationFunctionDeclaration->get_file_info()->isOutputInCodeGeneration() == true) ? "true" : "false");
#endif

     bool outputInstantiatedTemplateFunction = false;
     if ( isTransformed (templateInstantiationFunctionDeclaration) == true )
        {
       // DQ (5/16/2005): This is an attempt to remove explicit declarations of specializations which 
       // are preventing template instantiations of function definitions within the prelinking process.
          bool skipforwardDeclarationOfTemplateSpecialization = 
               (templateInstantiationFunctionDeclaration->get_file_info()->isCompilerGenerated() == true) && 
               (templateInstantiationFunctionDeclaration->get_definition() == NULL) &&
               (templateInstantiationFunctionDeclaration->get_definingDeclaration() == NULL);
#if 0
          printf ("In unparseTemplateInstantiationFunctionDeclStmt(): skipforwardDeclarationOfTemplateSpecialization = %s \n",skipforwardDeclarationOfTemplateSpecialization ? "true" : "false");
#endif
          if (skipforwardDeclarationOfTemplateSpecialization == true)
             {
            // This is a compiler generated forward function declaration of a template instatiation, so skip it!
#if 0
               printf ("This is a compiler generated forward function declaration of a template instatiation, so skip it! \n");
#endif
#if OUTPUT_PLACEHOLDER_COMMENTS_FOR_SUPRESSED_TEMPLATE_IR_NODES
               curprint("\n/* Skipping output of compiler generated forward function declaration of a template specialization */");
#endif
#if PRINT_DEVELOPER_WARNINGS || 0
               printf ("This is a compiler generated forward function declaration of a template instatiation, so skip it! \n");
               curprint ( string("\n/* Skipping output of compiler generated forward function declaration of a template specialization */"));
#endif
               return;
             }

// TV (10/15/18): this is an issue when forcing ROSE to unparse the template instantiation in Kripke::Kernel
#if 0
          bool skipInlinedTemplates = templateInstantiationFunctionDeclaration->get_functionModifier().isInline();
          if (skipInlinedTemplates == true)
             {
            // skip output of inlined templates since these are likely to have been used 
            // previously and would be defined too late if provided as an inline template 
            // specialization output in the source code.
#if PRINT_DEVELOPER_WARNINGS || 1
               printf ("This is an inlined template which might have been used previously (skipping output of late specialization) \n");
               curprint ( string("\n/* Skipping output of inlined template specialization */"));
#endif
               return;
             }
#endif

#if PRINT_DEVELOPER_WARNINGS
          curprint ( string("\n/* In unparseTemplateInstantiationFunctionDeclStmt(): part of transformation - output the template function declaration */ \n "));
#endif
          outputInstantiatedTemplateFunction = true;
        }
       else
        {
       // Also output the template member function declaration the template declaration appears in the source file.
          string currentFileName = getFileName();
#if 0
          printf ("Inside of unparseTemplateInstantiationFunctionDeclStmt(): currentFileName = %s \n",currentFileName.c_str());
#endif
       // DQ (5/2/2012): If the template declaration is not available then it is likely that is does not exist and so we will need to output the instantiation.
       // The problem with this is that we actually build a template declaration in this case but it is not represented by a string (so until we
       // abandon the string use of the template declaration in the unparsing we can't take advantage of this).
#if 0
          printf ("templateInstantiationFunctionDeclaration->get_templateDeclaration() = %p \n",templateInstantiationFunctionDeclaration->get_templateDeclaration());
#endif
       // ROSE_ASSERT(templateInstantiationFunctionDeclaration->get_templateDeclaration() != NULL);
          if (templateInstantiationFunctionDeclaration->get_templateDeclaration() != NULL)
             {
               ROSE_ASSERT(templateInstantiationFunctionDeclaration->get_templateDeclaration()->get_file_info() != NULL);
               ROSE_ASSERT(templateInstantiationFunctionDeclaration->get_templateDeclaration()->get_file_info()->get_filename() != NULL);
               string declarationFileName = templateInstantiationFunctionDeclaration->get_templateDeclaration()->get_file_info()->get_filename();
#if 0
               printf ("In unparseTemplateInstantiationFunctionDeclStmt(): currentFileName     = %s \n",currentFileName.c_str());
               printf ("In unparseTemplateInstantiationFunctionDeclStmt(): declarationFileName = %s \n",declarationFileName.c_str());
               printf ("templateInstantiationFunctionDeclaration source position information: \n");
               templateInstantiationFunctionDeclaration->get_file_info()->display("templateInstantiationFunctionDeclaration: debug");
               templateInstantiationFunctionDeclaration->get_templateDeclaration()->get_file_info()->display("templateInstantiationFunctionDeclaration->get_templateDeclaration(): debug");
#endif
            // if ( declarationFileName == currentFileName )
            // if ( declarationFileName == currentFileName && templateInstantiationMemberFunctionDeclaration->get_file_info()->isOutputInCodeGeneration() == true)

            // DQ (7/21/2012): We have already made the decission to output this function declaration (before we called this function).
            // and in the case of an explicit template specialization the souce position would have a valid location with isOutputInCodeGeneration() set to false 
            // (since this would not be compiler generated) see test2012_132.C for an example.

            // if ( templateInstantiationFunctionDeclaration->get_file_info()->isCompilerGenerated() == false )
               if ( templateInstantiationFunctionDeclaration->get_file_info()->isCompilerGenerated() == false && templateInstantiationFunctionDeclaration->get_file_info()->isSourcePositionUnavailableInFrontend() == false )
                  {
#if 0
                    printf ("Declaration is not compiler generate or marked as frontend specific \n");
#endif
                 // DQ (8/2/2012): If this is not compiler generated then is was in the original source code ans we have to out put it in the generated code.
                    outputInstantiatedTemplateFunction = true;
                  }
                 else
                  {
                 // DQ (8/2/2012): Else it was compiler generated and we have to explicitly ask if we have seperately determined that it should be output.
#if 1
                 // We only look at the isOutputInCodeGeneration() if this is a compiler generated function.
                    if ( templateInstantiationFunctionDeclaration->get_file_info()->isOutputInCodeGeneration() == true )
#else
                    if ( true )
#endif
                       {
#if 0
                         printf ("Declaration appears in the current source file. \n");
#endif
#if PRINT_DEVELOPER_WARNINGS
                         curprint ( string("\n/* In unparseTemplateInstantiationFunctionDeclStmt(): output the template function declaration */ \n "));
#endif
                         outputInstantiatedTemplateFunction = true;
                       }
                      else
                       {
#if 0
                         printf ("Declaration does NOT appear in the current source file. \n");
#endif
                      // curprint ( string("\n/* In unparseTemplateInstantiationFunctionDeclStmt(): skip output of template function declaration */ \n ";
#if PRINT_DEVELOPER_WARNINGS
                         curprint ( string("/* Skipped output of template function declaration (name = " ) + templateInstantiationFunctionDeclaration->get_qualified_name().str() + ") */ \n");
#endif
                       }
                  }
             }
            else
             {
            // DQ (5/2/2012): We need to output the template instantiation in some cases (see documentation above).
               printf ("Note: the template function declaration is not available (this happens for declarations in templated classes): so output the template instantiation. stmt = %p = %s \n",stmt,stmt->class_name().c_str());
               outputInstantiatedTemplateFunction = true;
             }
        }

#if 0
     printf ("In unparseTemplateInstantiationFunctionDeclStmt(): outputInstantiatedTemplateFunction = %s \n",outputInstantiatedTemplateFunction ? "true" : "false");
#endif

     if (outputInstantiatedTemplateFunction == true)
        {
       // const SgTemplateArgumentPtrList& templateArgListPtr = templateInstantiationFunctionDeclaration->get_templateArguments();
#if 0
       // DQ (8/29/2005): This is now output by the Unparse_ExprStmt::outputTemplateSpecializationSpecifier() member function

       // DQ (3/2/2005): Comment out use of "template<>"
       // DQ (5/8/2004): Make this an explicit specialization (using the newer C++ syntax to support this)
       // Output: "template <type>" before function declaration.
          ROSE_ASSERT(templateInstantiationFunctionDeclaration->get_templateArguments() != NULL);
          if (templateInstantiationFunctionDeclaration->get_templateArguments()->size() > 0)
             {
            // printf ("Declaration is a prototype functionDeclaration->get_definition() = %p \n",functionDeclaration->get_definition());
               curprint ( string("\n/* ROSE generated template specialization " ) + 
                      ( (functionDeclaration->get_definition() == NULL) ? "(prototype)" : "(explicit definition)") + " */");
               curprint ( string("\ntemplate <> "));
             }
       // unparseTemplateArguments(templateArgListPtr,info);
#endif

       // Now output the function declaration
#if 0
          printf ("Now output the function declaration (unparseFuncDeclStmt) \n");
          curprint ("\n/* Now output the function declaration (unparseFuncDeclStmt) */\n ");
#endif
          unparseFuncDeclStmt(functionDeclaration,info);
#if 0
          curprint ("\n/* DONE: Now output the function declaration (unparseFuncDeclStmt) */\n ");
#endif
        }

#if 0
     printf ("Leaving unparseTemplateInstantiationFunctionDeclStmt() \n");
     curprint ("\n/* Leaving unparseTemplateInstantiationFunctionDeclStmt() */\n ");
#endif
   }


void
Unparse_ExprStmt::unparseTemplateInstantiationMemberFunctionDeclStmt (SgStatement* stmt, SgUnparse_Info& info)
   {
  // Rules for output of member templates functions:
  //  1) When we unparse the template declaration as a string EDG removes the member 
  //     function definitions so we are forced to output all template member functions.
  //  2) If the member function is specified outside of the class then we don't have to
  //     explicitly output the instantiation.

  // DQ (3/24/2004): New function to support templates
     SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDeclaration = 
          isSgTemplateInstantiationMemberFunctionDecl(stmt);
     ROSE_ASSERT(templateInstantiationMemberFunctionDeclaration != NULL);

// #if OUTPUT_DEBUGGING_FUNCTION_NAME
#if 0
     printf ("Inside of unparseTemplateInstantiationMemberFunctionDeclStmt() = %p name = %s  transformed = %s prototype = %s static = %s compiler generated = %s transformation = %s output = %s \n",
       // templateInstantiationMemberFunctionDeclaration->get_templateName().str(),
          templateInstantiationMemberFunctionDeclaration,
          templateInstantiationMemberFunctionDeclaration->get_qualified_name().str(),
          isTransformed (templateInstantiationMemberFunctionDeclaration) ? "true" : "false",
          (templateInstantiationMemberFunctionDeclaration->get_definition() == NULL) ? "true" : "false",
          (templateInstantiationMemberFunctionDeclaration->get_declarationModifier().get_storageModifier().isStatic() == true) ? "true" : "false",
          (templateInstantiationMemberFunctionDeclaration->get_file_info()->isCompilerGenerated() == true) ? "true" : "false",
          (templateInstantiationMemberFunctionDeclaration->get_file_info()->isTransformation() == true) ? "true" : "false",
          (templateInstantiationMemberFunctionDeclaration->get_file_info()->isOutputInCodeGeneration() == true) ? "true" : "false");
#endif

#if 0
     curprint("/* Output in curprint in Unparse_ExprStmt::unparseTemplateInstantiationMemberFunctionDeclStmt() */");
#endif

  // DQ (6/1/2005): Use this case when PROTOTYPE_INSTANTIATIONS_IN_IL is to true in EDG's host_envir.h
     bool outputMemberFunctionTemplateInstantiation = false;
     if ( isTransformed (templateInstantiationMemberFunctionDeclaration) == true )
        {
       // Always output the template member function declaration if they are transformed.
#if 0
          printf ("templateInstantiationMemberFunctionDeclaration has been transformed \n");
#endif
          SgDeclarationStatement* definingDeclaration = templateInstantiationMemberFunctionDeclaration->get_definingDeclaration();
       // ROSE_ASSERT(definingDeclaration != NULL);
          SgMemberFunctionDeclaration* memberFunctionDeclaration = (definingDeclaration == NULL) ? NULL : isSgMemberFunctionDeclaration(definingDeclaration);
       // ROSE_ASSERT(memberFunctionDeclaration != NULL);

       // SgTemplateDeclaration* templateDeclaration = templateInstantiationMemberFunctionDeclaration->get_templateDeclaration();
       // ROSE_ASSERT(templateDeclaration != NULL);

          bool hasDefinition = (memberFunctionDeclaration != NULL && memberFunctionDeclaration->get_definition() != NULL);

       // printf ("hasDefinition = %s \n",hasDefinition ? "true" : "false");

          if (hasDefinition == true)
             {
            // printf ("Output this member function \n");
               outputMemberFunctionTemplateInstantiation = true;
             }
            else
             {
#if 0
               printf ("In unparseTemplateInstantiationMemberFunctionDeclStmt(): function has no definition, so skip output! \n");
#endif
#if OUTPUT_PLACEHOLDER_COMMENTS_FOR_SUPRESSED_TEMPLATE_IR_NODES
               curprint (" /* function has no definition, so skip output */ ");
#endif
             }
        }
       else
        {
       // Also output the template member function declaration the template declaration appears in the source file.
          string currentFileName = getFileName();

          if (templateInstantiationMemberFunctionDeclaration->get_templateDeclaration() == NULL)
             {
            // DQ (4/6/2014): This happens when a member function template in embedded in a class
            // template and thus there is not an associated template for the member function separate
            // from the class declaration.  It is not rare for many system template libraries (e.g. iostream).
#if 0
               printf (" I think that this can happen (see test2005_139.C) \n");
#endif
             }
#if 0
          ROSE_ASSERT(templateInstantiationMemberFunctionDeclaration->get_templateDeclaration() != NULL);
          ROSE_ASSERT(templateInstantiationMemberFunctionDeclaration->get_templateDeclaration()->get_file_info() != NULL);
          ROSE_ASSERT(templateInstantiationMemberFunctionDeclaration->get_templateDeclaration()->get_file_info()->get_filename() != NULL);
          string declarationFileName = templateInstantiationMemberFunctionDeclaration->get_templateDeclaration()->get_file_info()->get_filename();
#endif
#if 0
          printf ("In unparseTemplateInstantiationMemberFunctionDeclStmt(): currentFileName     = %s \n",currentFileName.c_str());
          printf ("In unparseTemplateInstantiationMemberFunctionDeclStmt(): declarationFileName = %s \n",declarationFileName.c_str());
          printf ("templateInstantiationMemberFunctionDeclaration source position information: \n");
          templateInstantiationMemberFunctionDeclaration->get_file_info()->display("template instantiation: debug");
          templateInstantiationMemberFunctionDeclaration->get_templateDeclaration()->get_file_info()->display("template declaration: debug");
#endif

       // DQ (8/19/2005): We only have to test for if the template instantiation
       // is marked for output!
       // DQ (8/17/2005): We have to additionally mark the member function 
       // instantiation for output since the correct specialization would 
       // disqualify the member function for output! The rules are simple:
       //    1) The member function must be defined in the current source file 
       //       (else it will be defined in a header file and we need not output 
       //       it explicitly (since we handle only non-transformed template 
       //       instantiations in this branch).  And,
       //    2) The member function must be marked for output to avoid the output
       //       of the member function in the case of a template specialiazation.
       // if ( declarationFileName == currentFileName )
       // if ( declarationFileName == currentFileName && templateInstantiationMemberFunctionDeclaration->get_file_info()->isOutputInCodeGeneration() == true)
          if ( templateInstantiationMemberFunctionDeclaration->get_file_info()->isOutputInCodeGeneration() == true )
             {
#if 0
               printf ("Declaration appears in the current source file. \n");
#endif
               outputMemberFunctionTemplateInstantiation = true;
             }
            else
             {
            // DQ (5/22/2013): Added to support output of non-template member functions with valid source position (e.g. the constructor in test2013_176.C).
               if (templateInstantiationMemberFunctionDeclaration->get_file_info()->get_file_id() >= 0)
                  {
                    outputMemberFunctionTemplateInstantiation = true;
                  }
#if 0
               printf ("Declaration does NOT appear in the current source file (templateInstantiationMemberFunctionDeclaration = %p = %s) \n",
                    templateInstantiationMemberFunctionDeclaration, templateInstantiationMemberFunctionDeclaration->get_qualified_name().str());
               printf ("   templateInstantiationMemberFunctionDeclaration->get_name() = %s \n",templateInstantiationMemberFunctionDeclaration->get_name().str());
               printf ("   isSpecialization() = %s \n",templateInstantiationMemberFunctionDeclaration->isSpecialization() ? "true" : "false");
#endif
             }
        }

#if 0
     printf ("Inside of unparseTemplateInstantiationMemberFunctionDeclStmt(): outputMemberFunctionTemplateInstantiation = %s \n",outputMemberFunctionTemplateInstantiation ? "true" : "false");
#endif

     if (outputMemberFunctionTemplateInstantiation == true )
        {
          SgFunctionDeclaration* memberFunctionDeclaration = 
               isSgMemberFunctionDeclaration(templateInstantiationMemberFunctionDeclaration);
          ROSE_ASSERT(memberFunctionDeclaration != NULL);

       // DQ (3/3/2005): Commented out since it was a problem in test2004_36.C
       // DQ (5/8/2004): Make this an explicit specialization (using the newer C++ syntax to support this)
       // curprint ( string("template <> \n";
       // ROSE_ASSERT(templateInstantiationMemberFunctionDeclaration->get_templateArguments() != NULL);
       // if (templateInstantiationMemberFunctionDeclaration->get_templateArguments()->size() > 0)
          if (templateInstantiationMemberFunctionDeclaration->isSpecialization() == true)
             {
               if ( (templateInstantiationMemberFunctionDeclaration->get_file_info()->isCompilerGenerated() == true) &&
                    (templateInstantiationMemberFunctionDeclaration->isForward() == true) )
                  {
                 // This is a ROSE generated forward declaration of a ROSE specialized member function (required).
                 // It is built in ROSE/src/roseSupport/templateSupport.C void fixupInstantiatedTemplates ( SgProject* project ).
                 // The forward declaration is placed directly after the template declaration so that no uses of the function can exist
                 // prior to its declaration.  Output a message into the gnerated source code identifying this transformation.
#if PRINT_DEVELOPER_WARNINGS || 0
                    curprint ( string("\n/* ROSE generated forward declaration of the ROSE generated member template specialization */"));
#endif
                  }
                 else
                  {
                 // This is the ROSE generated template specialization for the template member function 
                 // (required to be defined since the function is used (called)).  This function is defined 
                 // at the end of file and may be defined there because a forward declaration for the 
                 // specialization was output directly after the template declaration (before any use of 
                 // the function could have been made ???).
#if PRINT_DEVELOPER_WARNINGS || 0
                    curprint ( string("\n/* ROSE generated member template specialization */"));
#endif
                  }

            // DQ (8/27/2005): This might be required for g++ 3.4.x and optional for g++ 3.3.x
            // DQ (8/19/2005): It is incorrect when used for non-template member functions on templated
            // classes defined in the class
            // Output the syntax for template specialization (appears to be largely optional (at least with GNU g++)
            // curprint ( string("\ntemplate <> ";
             }

       // DQ (8/29/2005): This is now output by the Unparse_ExprStmt::outputTemplateSpecializationSpecifier() member function
       // curprint ( string("\ntemplate <> ";
#if 0
          printf ("Calling unparseMFuncDeclStmt() \n");
#endif
          unparseMFuncDeclStmt(memberFunctionDeclaration,info);
        }
       else
        {
#if 0
          curprint ( string("/* Skipped output of member function declaration (name = ") + templateInstantiationMemberFunctionDeclaration->get_templateName().getString() + string(") */ \n") );
#endif
#if PRINT_DEVELOPER_WARNINGS
          curprint ( string("/* Skipped output of template member function declaration (name = " ) + templateInstantiationMemberFunctionDeclaration->get_qualified_name().str() + ") */ \n");
#endif
        }

#if 0
     printf ("Leaving unparseTemplateInstantiationMemberFunctionDeclStmt(): outputMemberFunctionTemplateInstantiation = %s \n",outputMemberFunctionTemplateInstantiation ? "true" : "false");
#endif
   }

void
Unparse_ExprStmt::unparsePragmaDeclStmt (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgPragmaDeclaration* pragmaDeclaration = isSgPragmaDeclaration(stmt);
     ROSE_ASSERT(pragmaDeclaration != NULL);

     SgPragma* pragma = pragmaDeclaration->get_pragma();
     ROSE_ASSERT(pragma != NULL);

  // Request from Boyanna at ANL:
  // DQ (6/22/2006): Start all pragmas at the start of the line.  Since these are
  // handled as IR nodes (#pragma is part of the C and C++ grammar afterall)they 
  // are indented for as any other sort of statements.  I have added a CR
  // to put the pragma at the start of the next line.  A better solution might be to 
  // have the indent mechanism look ahead to see any upcoming SgPragmaDeclarations
  // so that the indentation (insertion of extra spaces) could be skipped.  This would
  // avoid the insertion of empty lines in the generated code.
  // curprint ( string("#pragma " + pragma->get_pragma() + "\n";

     string pragmaString = pragma->get_pragma();
     string identSubstring = "ident";

  // Test for and ident string (which has some special quoting rules that apply to some compilers)
     if (pragmaString.substr(0,identSubstring.size()) == identSubstring)
        {
          curprint ( string("\n#pragma ident \"" ) + pragmaString.substr(identSubstring.size()+1) + "\"\n");
        }
       else
        {
          curprint ( string("\n#pragma " ) + pragma->get_pragma() + "\n");
        }

  // printf ("Output the pragma = %s \n",pragma->get_pragma());
  // ROSE_ASSERT (0);
   }


void
Unparse_ExprStmt::unparseEmptyDeclaration (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgEmptyDeclaration* emptyDeclaration = isSgEmptyDeclaration(stmt);
     ROSE_ASSERT(emptyDeclaration != NULL);

  // unparseAttachedPreprocessingInfo(stmt, info, PreprocessingInfo::before);

  // Nothing to unparse for this case, comment and CPP directives should have been unparsed before getting to this point.
#if 0
     curprint( string("\n /* unparseEmptyDeclaration */ " ) );
#endif

  // unparseAttachedPreprocessingInfo(stmt, info, PreprocessingInfo::after);
   }


void
Unparse_ExprStmt::unparseBasicBlockStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgBasicBlock* basic_stmt = isSgBasicBlock(stmt);
     ROSE_ASSERT(basic_stmt != NULL);

#define DEBUG_BASIC_BLOCK 0

  // unparseAttachedPreprocessingInfo(basic_stmt, info, PreprocessingInfo::before);

#if DEBUG_BASIC_BLOCK
     printf ("In unparseBasicBlock (stmt = %p) \n",stmt);
     curprint ("/* In unparseBasicBlock */");
#endif

  // DQ (12/15/2014): Debugging, support to detect where this is changed between the top of the block and the bottom of the block.
     bool saved_top_unparsedPartiallyUsingTokenStream = info.unparsedPartiallyUsingTokenStream();

  // DQ (12/16/2014): The value of info.unparsedPartiallyUsingTokenStream() is used as a sort of 
  // global state, we want it to be consistant within the processing of this function.
  // SgUnparse_Info ninfo(info);
     bool saved_unparsedPartiallyUsingTokenStream = info.unparsedPartiallyUsingTokenStream();

#if 0
     saved_unparsedPartiallyUsingTokenStream = true;
     printf ("In unparseBasicBlock (stmt = %p) reset saved_unparsedPartiallyUsingTokenStream = true \n");
#endif

#if DEBUG_BASIC_BLOCK
     printf ("In unparseBasicBlock (stmt = %p) saved_unparsedPartiallyUsingTokenStream = %s \n",basic_stmt,saved_unparsedPartiallyUsingTokenStream ? "true" : "false");
#endif

  // DQ (12/5/2014): Test for if we have unparsed partially using the token stream. 
  // If so then we don't want to unparse this syntax, if not then we require this syntax.
  // if (info.unparsedPartiallyUsingTokenStream() == false)
     if (saved_unparsedPartiallyUsingTokenStream == false)
        {
          unp->cur.format(basic_stmt, info, FORMAT_BEFORE_BASIC_BLOCK1);
       // curprint ( string("{"));
          if (SgProject::get_verbose() > 0)
               curprint("/* syntax from AST */ {");
            else
               curprint("{");

          unp->cur.format(basic_stmt, info, FORMAT_AFTER_BASIC_BLOCK1);
        }
       else
        {
       // DQ (1/14/2015): We need to unparse syntax instead of the initial token, because this can be a macro expansion
       // (see tests/nonsmoke/functional/roseTests/astInterfaceTests/inputmoveDeclarationToInnermostScope_test2015_57.C).
       // unparseStatementFromTokenStream (stmt, e_leading_whitespace_start, e_token_subsequence_start);
       // unparseStatementFromTokenStream (stmt, e_token_subsequence_start, e_token_subsequence_start);
          curprint("{");
#if DEBUG_BASIC_BLOCK
          curprint ("/* unparse start of SgBasicBlock */");
#endif
        }

     if (basic_stmt->get_asm_function_body().empty() == false)
        {
#if DEBUG_BASIC_BLOCK
          curprint ("/* unparse asm function body of SgBasicBlock */");
#endif
       // This is an asm function body.
          curprint (basic_stmt->get_asm_function_body());

       // Make sure this is a function definition.
          ROSE_ASSERT(isSgFunctionDefinition(basic_stmt->get_parent()) != NULL);
        }

  // DQ (1/9/2007): This is useful for understanding which blocks are marked as compiler generated.
  // curprint ( string(" /* block compiler generated = " + (basic_stmt->get_startOfConstruct()->isCompilerGenerated() ? "true" : "false") + " */ \n ";

  // curprint ( string(" /* block size = " + basic_stmt->get_statements().size() + " */ \n ";

  // printf ("block scope = %p = %s \n",basic_stmt,basic_stmt->class_name().c_str());
  // basic_stmt->get_file_info()->display("basic_stmt block scope: debug");

#if OUTPUT_HIDDEN_LIST_DATA
     outputHiddenListData (basic_stmt);
#endif

#if DEBUG_BASIC_BLOCK
  // DQ (1/7/2015): The funcationality to output the trailing tokens of the last statement is implemented in the unparseStatementFromTokenStream() function.
     SgStatement* last_stmt = NULL;
#endif

#if 0
  // DQ (11/12/2015): Compute the statement to use for representative whitespace once.
     SgStatementPtrList::iterator representativeStatementForWhitespace = basic_stmt->get_statements().begin();

  // DQ (11/14/2015): Adding this test allows us to avoid locations where the use of the SgLocatedNode::isTransformation() 
  // function will enforce consistancy of the test on the multiple SgFileInfo objects. Something that should be fix in the
  // fixup of the AST (and reported as a warning where detected).
     if (saved_unparsedPartiallyUsingTokenStream == true)
        {
          while (representativeStatementForWhitespace != basic_stmt->get_statements().end() && (*representativeStatementForWhitespace)->isTransformation() == true)
             {
               representativeStatementForWhitespace++;
             }
        }
#else

  // DQ (10/31/2018): We need to get the current source file from the SgUnparseInfo object instead of 
  // computing it through the chain of parent pointers in the translation unit.  This is essential for
  // header file processing since the header file will have a copy of the trnslation unit's global scope.
  // DQ (9/28/2018): We need to get the SgSourceFile so that we can use the correct map from the map 
  // of maps in the modified implementation that supports multiple files for token based unparsing.
  // SgSourceFile* sourceFile = TransformationSupport::getSourceFile(basic_stmt);
     SgSourceFile* sourceFile = info.get_current_source_file();

  // DQ (10/31/2018): This is not always non-null (e.g. when used with the unparseToString() function).
  // ROSE_ASSERT(sourceFile != NULL);

#if 0
  // DQ (9/28/2018): This should also be true, or is at least what we really want, I think.
  // This is not always true, statemetns can come from any header file which will make it false.
     if (sourceFile->get_startOfConstruct()->get_file_id() != basic_stmt->get_startOfConstruct()->get_file_id())
        {
          printf ("In unparseBasicBlock(): sourceFile->getFileName()                                = %s \n",sourceFile->getFileName().c_str());
          printf ("In unparseBasicBlock(): info.get_current_source_file()->getFileName()            = %s \n",info.get_current_source_file()->getFileName().c_str());
          printf ("In unparseBasicBlock(): basic_stmt->get_startOfConstruct()->get_filenameString() = %s \n",basic_stmt->get_startOfConstruct()->get_filenameString().c_str());
          printf ("In unparseBasicBlock(): sourceFile->get_startOfConstruct()->get_file_id() = %d \n",sourceFile->get_startOfConstruct()->get_file_id());
          printf ("In unparseBasicBlock(): basic_stmt->get_startOfConstruct()->get_file_id() = %d \n",basic_stmt->get_startOfConstruct()->get_file_id());
#if 0
          printf ("Output the internal information in the file_id - filename map: \n");
          Sg_File_Info::display_static_data("In unparseBasicBlock()");
#endif
        }
  // ROSE_ASSERT(sourceFile->get_startOfConstruct()->get_file_id() == basic_stmt->get_startOfConstruct()->get_file_id());
#endif

  // DQ (9/28/2018): Older code.
  // SgStatementPtrList::iterator representativeStatementForWhitespace = sourceFile->get_representativeWhitespaceStatementMap()[basic_stmt];

  // DQ (9/28/2018): We need to get the SgSourceFile so that we can use the correct map from the map 
  // of maps in the modified implementation that supports multiple files for token based unparsing.
     SgStatement* representativeStatementForWhitespace = NULL;
  // if (SgSourceFile::get_representativeWhitespaceStatementMap().find(basic_stmt) != SgSourceFile::get_representativeWhitespaceStatementMap().end())
  // if (sourceFile->get_representativeWhitespaceStatementMap().find(basic_stmt) != sourceFile->get_representativeWhitespaceStatementMap().end())
     if (sourceFile != NULL && sourceFile->get_representativeWhitespaceStatementMap().find(basic_stmt) != sourceFile->get_representativeWhitespaceStatementMap().end())
        {
       // representativeStatementForWhitespace = SgSourceFile::get_representativeWhitespaceStatementMap()[basic_stmt];
          representativeStatementForWhitespace = sourceFile->get_representativeWhitespaceStatementMap()[basic_stmt];
          ROSE_ASSERT(representativeStatementForWhitespace != NULL);
        }
#endif

  // DQ (11/15/2015): if this is on because it is from an inherited SgBasicBlock then turn off the flag to control formatting.
  // I don't like this method of handling the inherited attribute, and perhaps this poitn to why this formatting should be 
  // controled using a different mechanism (though other mechanisms had there problems in thinking them through).
     info.unset_parentStatementListBeingUnparsedUsingPartialTokenSequence();

#if 0
     if (representativeStatementForWhitespace != basic_stmt->get_statements().end())
        {
          printf ("representativeStatementForWhitespace = %p = %s \n",*representativeStatementForWhitespace,(*representativeStatementForWhitespace)->class_name().c_str());
       // printf ("   --- (*representativeStatementForWhitespace)->unparseToString() = %s \n",(*representativeStatementForWhitespace)->unparseToString().c_str());
          bool ignoreDifferenceBetweenDefiningAndNondefiningDeclarations = true;
          printf ("   --- SageInterface::generateUniqueName(*representativeStatementForWhitespace) = %s \n",
               SageInterface::generateUniqueName(*representativeStatementForWhitespace,ignoreDifferenceBetweenDefiningAndNondefiningDeclarations).c_str());
        }
       else
        {
       // If we don't find anything then the falback position could be to use the whitespace associated 
       // with the SgBasicBlock, except that this would be strange for many common formatting styles.
          printf ("WARNING: no representative whitespace identified for SgBasicBlock = %p \n",basic_stmt);
        }
#endif

     SgStatementPtrList::iterator p = basic_stmt->get_statements().begin();
     while(p != basic_stmt->get_statements().end())
        { 
          ROSE_ASSERT((*p) != NULL);

#if DEBUG_BASIC_BLOCK
          printf ("In unparseBasicBlock (block = %p) statement = %p = %s saved_unparsedPartiallyUsingTokenStream = %s \n",
               basic_stmt,*p,(*p)->class_name().c_str(),saved_unparsedPartiallyUsingTokenStream ? "true" : "false");
#endif
#if DEBUG_BASIC_BLOCK && 0
          curprint ("/* LOOP: START unparse statement in SgBasicBlock */");
#endif

          SgUnparse_Info local_info(info);

       // DQ (11/4/2015): Adding in the leading white space of the first statement (whatever statement is first).
          if (saved_unparsedPartiallyUsingTokenStream == true)
             {
            // DQ (11/12/2015): We don't want to do this for just the first statement.
            // if (p == basic_stmt->get_statements().begin())
                  {
                 // We want to output the whitespace of the first statement, but the first statement may have been moved.
                 // But we can at least output the leading white space for whateve is currently the first statement.
                 // Unfortunately this can cause problems if this is more than just whitespace (e.g. "#if 1").
                 // So we need to check if this is only whitespace and then we can unparse it.  This would be 
                 // best handled by adding this feature to the unparseStatementFromTokenStream() function (I think).

                    local_info.set_parentStatementListBeingUnparsedUsingPartialTokenSequence();

                 // curprint("\n");
                    bool statement_is_transformation = (*p)->isTransformation();
#if DEBUG_BASIC_BLOCK || 0
                    printf ("statement is: %p = %s isTransformation() = %s \n",(*p),(*p)->class_name().c_str(),(*p)->isTransformation() ? "true" : "false");
                    string s = statement_is_transformation ? "true" : "false";
#endif
#if 0
                    curprint ("/* unparse leading white space of statement: START (next_statement_present_is_transformation = " + s + ") */");
#endif
                    if (statement_is_transformation == true)
                       {
                      // An additional issue is that we should implement unparseOnlyWhitespace support to only unparse 
                      // the trailing whitespace tokens instead of all the tokens except for non-whitespace).
#if 0
                      // Find representative whitespace for statements in this basic block.
                         SgStatementPtrList::iterator q = representativeStatementForWhitespace;
                         if (q != basic_stmt->get_statements().end())
                            {
                           // Found a statement in the basic block that we can use to represent representative whitespace.
                              bool unparseOnlyWhitespace = true;
#if 1
                              unparseStatementFromTokenStream (*q, *q, e_leading_whitespace_start, e_token_subsequence_start, info, unparseOnlyWhitespace);
#endif
                            }
                           else
                            {
                           // The least we can do is to output a CR in this case where we have no representative whitespace.
                           // curprint("\n");
#if 0
                              printf ("Not clear how to compute spacing, but at least we need a CR \n");
#endif
#if 1
                              curprint("\n");
#else
                              curprint("\n/* no representative whitespace available */ ");
#endif
                            }
#else
                      // DQ (11/20/2015): This implementation uses a previously prepared map of representative statements in 
                      // the scope so that we can support the use of the whitespace from these statements when unparsing 
                      // statements in the current scope that are transformations.
                         SgStatement* q = representativeStatementForWhitespace;
                         if (q != NULL)
                            {
                           // Found a statement in the basic block that we can use to represent representative whitespace.
                              bool unparseOnlyWhitespace = true;
#if 1
                              unparseStatementFromTokenStream (q, q, e_leading_whitespace_start, e_token_subsequence_start, info, unparseOnlyWhitespace);
#endif
                            }
                           else
                            {
                           // This is the backup plan if there was no identified statement associated with the current scope.
#if 0
                              printf ("Not clear how to compute spacing, but at least we need a CR \n");
#endif
#if 1
                              curprint("\n");
#else
                              curprint("\n/* no representative whitespace available */ ");
#endif
                            }
#endif
                       }
#if DEBUG_BASIC_BLOCK || 0
                    curprint ("/* unparse leading white space of first statement: END */");
#endif
                  }
             }

#if DEBUG_BASIC_BLOCK || 0
          curprint ("/* calling unparseStatement(): START */");
#endif
       // unparseStatement((*p), info);
          unparseStatement((*p), local_info);

#if DEBUG_BASIC_BLOCK || 0
          curprint ("/* calling unparseStatement(): END */");
#endif

#if DEBUG_BASIC_BLOCK || 0
          curprint ("/* LOOP: END unparse statement in SgBasicBlock */");
#endif
       // DQ (12/6/2014): Save the last statement so that we can use the trailing token stream if using the token-based unparsing.
       // last_stmt = *p;

          p++;
        }

#if DEBUG_BASIC_BLOCK || 0
     printf ("Inside of Unparse_ExprStmt::unparseBasicBlockStmt: output comment \n");
     curprint ("/* Inside of Unparse_ExprStmt::unparseBasicBlockStmt: output comment */");
#endif

#if 0
     printf ("Inside of Unparse_ExprStmt::unparseBasicBlockStmt: saved_unparsedPartiallyUsingTokenStream = %s \n",saved_unparsedPartiallyUsingTokenStream ? "true" : "false");
#endif

  // DQ (12/16/2014): This should be controled by the saved_unparsedPartiallyUsingTokenStream value.
  // DQ (3/17/2005): This helps handle cases such as void foo () { #include "constant_code.h" }
  // unparseAttachedPreprocessingInfo(basic_stmt, info, PreprocessingInfo::inside);
     if (saved_unparsedPartiallyUsingTokenStream == false)
        {
       // DQ (3/17/2005): This helps handle cases such as void foo () { #include "constant_code.h" }
#if 0
          printf ("Calling unparseAttachedPreprocessingInfo(): INSIDE: basic_stmt = %p = %s \n",basic_stmt,basic_stmt->class_name().c_str());
          printOutComments(basic_stmt);
          printf ("In unparseBasicBlockStmt(): info.SkipFunctionDefinition() = %s \n",info.SkipFunctionDefinition() ? "true" : "false");
#endif

          unparseAttachedPreprocessingInfo(basic_stmt, info, PreprocessingInfo::inside);
#if 0
       // DQ (2/18/2018): Added to test output missing comments.
          printf ("Calling unparseAttachedPreprocessingInfo(): part 1: AFTER: basic_stmt = %p = %s \n",basic_stmt,basic_stmt->class_name().c_str());
          curprint ("/* Inside of Unparse_ExprStmt::unparseBasicBlockStmt: output comment: part 1 (after) */");
          unparseAttachedPreprocessingInfo(basic_stmt, info, PreprocessingInfo::after);
          curprint ("/* DONE: Inside of Unparse_ExprStmt::unparseBasicBlockStmt: output comment: part 1 (after) */");
#endif
        }

#if DEBUG_BASIC_BLOCK
     printf ("DONE: Inside of Unparse_ExprStmt::unparseBasicBlockStmt: output comment \n");
     curprint ("/* DONE: Inside of Unparse_ExprStmt::unparseBasicBlockStmt: output comment */");
#endif

#if DEBUG_BASIC_BLOCK
     printf ("unparse end of SgBasicBlock: info.unparsedPartiallyUsingTokenStream() = %s last_stmt = %p \n",info.unparsedPartiallyUsingTokenStream() ? "true" : "false",last_stmt);
     if (last_stmt != NULL)
        {
          printf ("   --- last_stmt = %p = %s \n",last_stmt,last_stmt->class_name().c_str());
        }
#endif

  // DQ (12/15/2014): Debugging, support to detect where this is changed between the top of the block and the bottom of the block.
     bool saved_bottom_unparsedPartiallyUsingTokenStream = info.unparsedPartiallyUsingTokenStream();
  // ROSE_ASSERT(saved_top_unparsedPartiallyUsingTokenStream == saved_bottom_unparsedPartiallyUsingTokenStream);
     if (saved_top_unparsedPartiallyUsingTokenStream != saved_bottom_unparsedPartiallyUsingTokenStream)
        {
#if DEBUG_BASIC_BLOCK
          printf ("WARNING: value of info.unparsedPartiallyUsingTokenStream() changed within SgBasicBlock \n");
#endif
        }

#if DEBUG_BASIC_BLOCK
     curprint ("/* unparse end of SgBasicBlock */");
#endif

  // DQ (12/5/2014): Test for if we have unparsed partially using the token stream.
  // If so then we don't want to unparse this syntax, if not then we require this syntax.
  // if (info.unparsedPartiallyUsingTokenStream() == false)
     if (saved_unparsedPartiallyUsingTokenStream == false)
        {
          unp->cur.format(basic_stmt, info, FORMAT_BEFORE_BASIC_BLOCK2);

#if 0
       // DQ (2/18/2018): Debugging Cxx11_tests/test2018_28.C
          if (saved_unparsedPartiallyUsingTokenStream == false)
             {
               printf ("Inside of Unparse_ExprStmt::unparseBasicBlockStmt: output comment: part 2 (after) \n");
               curprint ("/* Inside of Unparse_ExprStmt::unparseBasicBlockStmt: output comment: part 2 (after) */");
               unparseAttachedPreprocessingInfo(basic_stmt, info, PreprocessingInfo::after);
               curprint ("/* DONE: Inside of Unparse_ExprStmt::unparseBasicBlockStmt: output comment: part 2 (after) */");
             }
#endif

       // curprint ( string("}"));
          if (SgProject::get_verbose() > 0)
               curprint("/* syntax from AST */ }");
            else
               curprint("}");

          unp->cur.format(basic_stmt, info, FORMAT_AFTER_BASIC_BLOCK2);
        }
       else
        {
#if 0
       // We might have to make sure that this SgStatement would be output.
          if (last_stmt != NULL)
             {
#if 0
               printf ("unparse white space at end of SgBasicBlock \n");
               curprint ("/* unparse white space at end of SgBasicBlock */");
#endif
            // unparseStatementFromTokenStream (last_stmt, stmt, e_token_subsequence_end, e_token_subsequence_start);
               unparseStatementFromTokenStream (last_stmt, stmt, e_trailing_whitespace_start, e_token_subsequence_end, info);
             }
#endif
#if DEBUG_BASIC_BLOCK
          printf ("unparse last token in SgBasicBlock \n");
          curprint ("/* unparse last token in SgBasicBlock */");
#endif

       // DQ (1/14/2015): We need to unparse syntax instead of the initial token, becasue this can be a macro expansion
       // (see tests/nonsmoke/functional/roseTests/astInterfaceTests/inputmoveDeclarationToInnermostScope_test2015_57.C).
       // unparseStatementFromTokenStream (stmt, e_token_subsequence_end, e_token_subsequence_end);
          curprint("}");
        }

#if DEBUG_BASIC_BLOCK
     printf ("Leaving unparseBasicBlock (stmt = %p) \n",stmt);
     curprint ("/* Leaving unparseBasicBlock */");
#endif
   }


// Determine how many "else {}"'s an outer if that has an else clause needs to
// prevent dangling if problems
static size_t countElsesNeededToPreventDangling(SgStatement* s)
   {
  // The basic rule here is that anything that has a defined end marker
  // (i.e., cannot end with an unmatched if statement) returns 0, everything
  // else (except if) gets the correct number of elses from its body
     switch (s->variantT())
        {
          case V_SgCaseOptionStmt: return countElsesNeededToPreventDangling(isSgCaseOptionStmt(s)->get_body());
          case V_SgCatchStatementSeq:
             {
               SgCatchStatementSeq* cs = isSgCatchStatementSeq(s);
               const SgStatementPtrList& seq = cs->get_catch_statement_seq();
               ROSE_ASSERT (!seq.empty());
               return countElsesNeededToPreventDangling(seq.back());
             }
          case V_SgDefaultOptionStmt: return countElsesNeededToPreventDangling(isSgCaseOptionStmt(s)->get_body());
          case V_SgLabelStatement: return countElsesNeededToPreventDangling(isSgLabelStatement(s)->get_statement());
          case V_SgCatchOptionStmt: return countElsesNeededToPreventDangling(isSgCatchOptionStmt(s)->get_body());
          case V_SgForStatement: return countElsesNeededToPreventDangling(isSgForStatement(s)->get_loop_body());
          case V_SgIfStmt:
             {
               SgIfStmt* ifs = isSgIfStmt(s);
               if (ifs->get_false_body() != NULL)
                  {
                    return 0;
                  } 
                 else
                  {
                    return countElsesNeededToPreventDangling(ifs->get_true_body()) + 1;
                  }
             }
          case V_SgWhileStmt: return countElsesNeededToPreventDangling(isSgWhileStmt(s)->get_body());
          case V_SgSwitchStatement: ROSE_ASSERT(isSgBasicBlock(isSgSwitchStatement(s)->get_body())); return 0;

          default: 
               return 0;
        }
   }


void Unparse_ExprStmt::unparseIfStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // DQ (12/13/2005): I don't like this implementation with the while loop...

#if 0
     printf ("In unparseIfStmt(): unparse if statement stmt = %p \n",stmt);
     curprint("/* Unparse the if statement */");
#endif
#if 0
     printf ("In unparseIfStmt(stmt=%p): info.unparsedPartiallyUsingTokenStream() = %s \n",stmt,info.unparsedPartiallyUsingTokenStream() ? "true" : "false");
#endif

     bool saved_unparsedPartiallyUsingTokenStream = info.unparsedPartiallyUsingTokenStream();

     SgIfStmt* if_stmt = isSgIfStmt(stmt);
     assert (if_stmt != NULL);

     while (if_stmt != NULL)
        {
          SgStatement *tmp_stmt = NULL;
#if 0
          printf ("In unparseIfStmt(): in while loop over if statements: if_stmt = %p \n",if_stmt);
#endif

       // DQ (12/6/2014): Test for if we have unparsed partially using the token stream. 
       // If so then we don't want to unparse this syntax, if not then we require this syntax.
       // curprint ( string("if ("));
       // if (info.unparsedPartiallyUsingTokenStream() == false)
          if (saved_unparsedPartiallyUsingTokenStream == false)
             {
               curprint("if (");

               SgUnparse_Info testInfo(info);
               testInfo.set_SkipSemiColon();
               testInfo.set_inConditional();
            // info.set_inConditional();
               if ( (tmp_stmt = if_stmt->get_conditional()) )
                  {
                 // Unparse using base class function so we get any required comments and CPP directives.
                 // unparseStatement(tmp_stmt, testInfo);
                    UnparseLanguageIndependentConstructs::unparseStatement(tmp_stmt, testInfo);
                  }
               testInfo.unset_inConditional();
            // curprint ( string(") "));
               if (SgProject::get_verbose() > 0)
                    curprint ("/* syntax from AST */ ) ");
                 else
                    curprint (") ");
             }
            else
             {
            // DQ (12/9/2014): Adding more support for unparsing using the token stream.
               SgStatement* true_body = if_stmt->get_true_body();
               if (true_body != NULL)
                  {
                 // DQ (1/18/2015): With the denormalization of SgBasicBlock in the SgIfStmt false body we have to use the computed if_stmt and not the outer stmt.
                 // unparseStatementFromTokenStream (stmt, true_body, e_leading_whitespace_start, e_token_subsequence_start);
                 // unparseStatementFromTokenStream (stmt, true_body, e_token_subsequence_start, e_token_subsequence_start);
                 // unparseStatementFromTokenStream (stmt, true_body, e_token_subsequence_start, e_leading_whitespace_start);
                    unparseStatementFromTokenStream (if_stmt, true_body, e_token_subsequence_start, e_leading_whitespace_start, info);
                  }
             }

          if ( (tmp_stmt = if_stmt->get_true_body()) ) 
             {
#if 0
               printf ("Unparse the if true body \n");
               curprint("\n/* Unparse the if true body */ \n");
#endif
            // DQ (12/6/2014): Test for if we have unparsed partially using the token stream. 
            // If so then we don't want to unparse this syntax, if not then we require this syntax.
            // unp->cur.format(tmp_stmt, info, FORMAT_BEFORE_NESTED_STATEMENT);
            // if (info.unparsedPartiallyUsingTokenStream() == false)
               if (saved_unparsedPartiallyUsingTokenStream == false)
                  {
                    unp->cur.format(tmp_stmt, info, FORMAT_BEFORE_NESTED_STATEMENT);
                  }

            // Unparse using base class function so we get any required comments and CPP directives.
            // unparseStatement(tmp_stmt, info);
               UnparseLanguageIndependentConstructs::unparseStatement(tmp_stmt, info);

            // DQ (12/6/2014): Test for if we have unparsed partially using the token stream. 
            // If so then we don't want to unparse this syntax, if not then we require this syntax.
            // unp->cur.format(tmp_stmt, info, FORMAT_AFTER_NESTED_STATEMENT);
            // if (info.unparsedPartiallyUsingTokenStream() == false)
               if (saved_unparsedPartiallyUsingTokenStream == false)
                  {
                    unp->cur.format(tmp_stmt, info, FORMAT_AFTER_NESTED_STATEMENT);
                  }
#if 0
               curprint("\n/* DONE: Unparse the if true body */ \n");
#endif
             }

#if 0
          printf ("Check for a false body \n");
          curprint("\n/* Check for a false body */ \n");
#endif

          if ( (tmp_stmt = if_stmt->get_false_body()) )
             {
#if 0
               printf ("Unparse the else between the true and false body \n");
               curprint("\n/* Unparse the else between the true and false body */ \n");
#endif
               size_t elsesNeededForInnerIfs = countElsesNeededToPreventDangling(if_stmt->get_true_body());
               for (size_t i = 0; i < elsesNeededForInnerIfs; ++i) 
                  {
                    curprint ( string(" else {}") ); // Ensure this else does not match an inner if statement
                  }

            // unp->cur.format(if_stmt, info, FORMAT_BEFORE_STMT);
            // curprint ( string("else "));
            // if (info.unparsedPartiallyUsingTokenStream() == false)
               if (saved_unparsedPartiallyUsingTokenStream == false)
                  {
                    unp->cur.format(if_stmt, info, FORMAT_BEFORE_STMT);
                 // curprint ( string("else "));
                    if (SgProject::get_verbose() > 0)
                       {
                         curprint ("/* syntax from AST (part 1) */ else ");
                       }
                      else
                       {
                      // DQ (1/4/2015): Remove trailing space to avoid redundant output of whitespace in token unparsing.
                      // We actually need to extra space to avoid unparsing "elseif" by mistake (likely we can address this detail later).
                      // curprint(" else ");
                         curprint(" else ");
                       }
                  }
                 else
                  {
#if 0
                    printf ("Unparse the else before the false body \n");
                    curprint("/* Unparse the else before the false body */");
#endif
                 // DQ (12/9/2014): Adding more support for unparsing using the token stream.
                    SgStatement* true_body  = if_stmt->get_true_body();
                    SgStatement* false_body = if_stmt->get_false_body();
                    if (true_body != NULL && false_body != NULL)
                       {
                      // DQ (1/4/2015): If the false body is a transformation then the token sequence will not exist 
                      // and the unparseStatementFromTokenStream() function will not output any token sequence.
                      // unparseStatementFromTokenStream (true_body, false_body, e_trailing_whitespace_start, e_token_subsequence_start);
                         if (SgProject::get_verbose() > 0)
                            {
                              curprint ("/* syntax from AST (part 2) */ else ");
                            }
                           else
                            {
                           // curprint(" else ");
                           // printf ("In unparseIfStmt(): Output the else part between the true and false cases of the if statement \n");
                           // unparseStatementFromTokenStream (true_body, false_body, e_trailing_whitespace_start, e_token_subsequence_start);
                           // unparseStatementFromTokenStream (true_body, true_body, e_trailing_whitespace_start, e_trailing_whitespace_end);

                           // We might need to check that there are whitespace tokens assocated with the trailing whitespace before the else.
                           // Also if the false block is a transformation then we need to output a CR or a space.
                              if (true_body->isTransformation() == true || false_body->isTransformation() == true)
                                 {
                                   curprint(" else ");
                                 }
                                else
                                 {
#if 0
                                   printf ("In unparseIfStmt(): true and false body not transformed: output the else part of token stream \n");
#endif
                                // unparseStatementFromTokenStream (false_body, false_body, e_leading_whitespace_start, e_leading_whitespace_end);
                                // unparseStatementFromTokenStream (true_body, false_body, e_trailing_whitespace_start, e_leading_whitespace_start);
                                // unparseStatementFromTokenStream (true_body, false_body, e_trailing_whitespace_start, e_leading_whitespace_start);
                                   unparseStatementFromTokenStream (true_body, false_body, e_trailing_whitespace_start, e_leading_whitespace_start, info);
                                 }
                            }
                       }
#if 0
                    printf ("DONE: Unparse the else before the false body \n");
                    curprint("/* DONE: Unparse the else before the false body */");
#endif
                  }
#if 0
               printf ("In unparseIfStmt(): unparsing the false branch: tmp_stmt = %p = %s \n",tmp_stmt,tmp_stmt->class_name().c_str());
#endif
               if_stmt = isSgIfStmt(tmp_stmt);
               if (if_stmt == NULL) 
                  {
                 // unp->cur.format(tmp_stmt, info, FORMAT_BEFORE_NESTED_STATEMENT);
                 // if (info.unparsedPartiallyUsingTokenStream() == false)
                    if (saved_unparsedPartiallyUsingTokenStream == false)
                       {
                         unp->cur.format(tmp_stmt, info, FORMAT_BEFORE_NESTED_STATEMENT);
                       }
#if 0
                    curprint("/* Unparse the if false body */");
#endif
#if 0
                    curprint("\n/* Unparse the if false body */ \n");
#endif
                 // Unparse using base class function so we get any required comments and CPP directives.
                 // unparseStatement(tmp_stmt, info);
                    UnparseLanguageIndependentConstructs::unparseStatement(tmp_stmt, info);
#if 0
                    curprint("\n/* DONE: Unparse the if false body */ \n");
#endif
                 // if (info.unparsedPartiallyUsingTokenStream() == false)
                    if (saved_unparsedPartiallyUsingTokenStream == false)
                       {
                         unp->cur.format(tmp_stmt, info, FORMAT_AFTER_NESTED_STATEMENT);
                       }
                  }
             }
            else
             {
               if_stmt = NULL;
             }

       // DQ (12/16/2008): Need to process any associated CPP directives and comments
          if (if_stmt != NULL)
             {
            // At this point if_stmt is a nested if statement in the true and false branch of the original if statement.
#if 0
               printf ("NESTED IF statement: might need to unparse white space for resolve else if: if_stmt = %p \n",if_stmt);
               curprint("/* NESTED IF statement: might need to unparse white space for resolve else if */");
#endif
#if 0
            // Original code.
   #if 0
               printf ("Calling unparseAttachedPreprocessingInfo(PreprocessingInfo::before): if_stmt = %p \n",if_stmt);
               curprint("/* Calling unparseAttachedPreprocessingInfo(PreprocessingInfo::before) */");
   #endif
               unparseAttachedPreprocessingInfo(if_stmt, info, PreprocessingInfo::before);
#else
               if (saved_unparsedPartiallyUsingTokenStream == true)
                  {
                 // New code where we unparse the whitespace between the else and the nested if statement.
                 // unparseStatementFromTokenStream (false_body, false_body, e_leading_whitespace_start, e_token_subsequence_start);
                    unparseStatementFromTokenStream (if_stmt, e_leading_whitespace_start, e_token_subsequence_start, info);
                  }
                 else
                  {
                 // original code if we are not unparsing from the token stream.
#if 0
                    printf ("Calling unparseAttachedPreprocessingInfo(PreprocessingInfo::before): if_stmt = %p \n",if_stmt);
                    curprint("/* Calling unparseAttachedPreprocessingInfo(PreprocessingInfo::before) */");
#endif
                    unparseAttachedPreprocessingInfo(if_stmt, info, PreprocessingInfo::before);
                  }
#endif
             }
#if 0
          printf ("In unparseIfStmt(): END of while loop over if statements: if_stmt = %p \n",if_stmt);
#endif
#if 0
          curprint("/* END of while loop in unparseIfStmt */");
#endif
        }

#if 0
     curprint("/* DONE: Unparse the if statement */");
#endif
   }


#if 1
// DQ (8/13/2007): This is no longer used, I think, however it might be required for the A++/P++ array optimizer.

//--------------------------------------------------------------------------------
//  void Unparse_ExprStmt::unparseWhereStmt
//
//  This special function unparses where and elsewhere statements. Where 
//  statements are actually represented as for statements in the Sage program
//  tree. Thus, the type of the where_stmt is SgForStatement. The part that
//  we are interested in unparsing is in the initializer statement of the 
//  for statement. In particular, we want to unparse the arguments of the
//  rhs of the initializer. The rhs should be a function call expression.
//  The same applies for elsewhere statements.
//--------------------------------------------------------------------------------
void
Unparse_ExprStmt::unparseWhereStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgForStatement* where_stmt = isSgForStatement(stmt);
     ROSE_ASSERT(where_stmt != NULL);

     printf ("In Unparse_ExprStmt::unparseWhereStmt() \n");

     SgStatement *tmp_stmt;
  // DQ (4/7/2001) we don't want the unparser to depend on the array grammar
  // (so comment this out and introduce the A++ "where" statment in some other way)
#if 0
     if (ArrayClassSageInterface::isROSEWhereStatement(stmt))
        {
          curprint ( string("where ("));
        }
       else
        {
       // isROSEElseWhereStatement
          curprint ( string("elsewhere ("));
        }
#else
          curprint ( string("elsewhere ("));
#endif

     SgUnparse_Info newinfo(info);
     newinfo.set_SkipSemiColon();
  // if(where_stmt->get_init_stmt() != NULL ) {
     if(where_stmt->get_init_stmt().size() > 0 )
        {
          SgStatementPtrList::iterator i=where_stmt->get_init_stmt().begin();
          if ((*i) != NULL && (*i)->variant() == EXPR_STMT)
             {
               SgExprStatement* pExprStmt = isSgExprStatement(*i);
            // SgAssignOp* pAssignOp = isSgAssignOp(pExprStmt->get_the_expr());
               SgAssignOp* pAssignOp = isSgAssignOp(pExprStmt->get_expression());
               if ( pAssignOp != NULL )
                  {
                    SgFunctionCallExp* pFunctionCallExp = isSgFunctionCallExp(pAssignOp->get_rhs_operand());
                    if(pFunctionCallExp != NULL)
                       {
                         if(pFunctionCallExp->get_args())
                            {
                              SgExpressionPtrList& list = pFunctionCallExp->get_args()->get_expressions();
                              SgExpressionPtrList::iterator arg = list.begin();
                              while (arg != list.end())
                                 {
                                   unparseExpression((*arg), newinfo);
                                   arg++;
                                   if (arg != list.end())
                                      {
                                      curprint ( string(","));
                                      }
                                 }
                            }
                       } //pFunctionCallExp != NULL 
                  } //pAssignOp != NULL
             } //(*i).irep() != NULL && (*i).irep()->variant() == EXPR_STMT
        } //where_stmt->get_init_stmt() != NULL

     curprint ( string(")"));

     if ( (tmp_stmt = where_stmt->get_loop_body()) )
        {
          unparseStatement(tmp_stmt, info);
        }
       else
        {
          if (!info.SkipSemiColon())
             {
               curprint ( string(";"));
             }
        }
   }
#endif


void
Unparse_ExprStmt::unparseForInitStmt (SgStatement* stmt, SgUnparse_Info& info)
   {
  // DQ (7/11/2004): Added to simplify debugging for everyone (requested by Willcock)

#if 0
     printf ("Unparse for loop initializers \n");
#endif

     SgForInitStatement* forInitStmt = isSgForInitStatement(stmt);
     ROSE_ASSERT(forInitStmt != NULL);

     SgStatementPtrList::iterator i = forInitStmt->get_init_stmt().begin();

  // DQ (12/8/2004): Build a new info object so that we can supress the unparsing of 
  // the base type once the first variable has been unparsed.
     SgUnparse_Info newinfo(info);

     while(i != forInitStmt->get_init_stmt().end())
        {
#if 0
          printf ("--- *i = %p = %s \n",*i,(*i)->class_name().c_str());
          curprint("/* unparseForInitStmt: " + (*i)->class_name() + " */ ");
#endif

#if 0
       // DQ (8/30/2014): For all but the first entry we should have get_isAssociatedWithDeclarationList() == true.
       // if (i != forInitStmt->get_init_stmt().begin())
          if (forInitStmt->get_init_stmt().size() > 1)
             {
               SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(*i);
            // DQ (3/2/2019): Commented out output spew (see roseTests/programTransformationTests/pass1.C for an example).
            // Note that not all entries will be a SgVariableDeclaration (e.g. detected case of SgNullStatement).
            // ROSE_ASSERT(variableDeclaration != NULL);
               if (variableDeclaration != NULL)
                  {
                 // DQ (8/31/2014): This fails for one of the transformation tests: tests/nonsmoke/functional/roseTests/programTransformationTests/pass1.C
                 // This is likely an error in the implementation of the transformation (all other tests of the AST without transformations pass).
                    if (variableDeclaration->get_isAssociatedWithDeclarationList() == false)
                       {
                         printf ("WARNING: In unparseForInitStmt(forInitStmt = %p): I would have expected that variableDeclaration->get_isAssociatedWithDeclarationList() == true \n",forInitStmt);
                       }
                 // ROSE_ASSERT(variableDeclaration->get_isAssociatedWithDeclarationList() == true);
                  }
             }
#endif

          unparseStatement(*i, newinfo);
          i++;

       // After unparsing the first variable declaration with the type 
       // we want to unparse the rest without the base type.
          newinfo.set_SkipBaseType();

          if (i != forInitStmt->get_init_stmt().end())
             {
               curprint ( string(", "));
             }
        }

  // DQ (11/4/2015): Change the unparsing semantics to for loop initializer statement to exclude 
  // the " " after the ";" so that we can more faithfully represent the unparsed code when using 
  // the token-based unparsing.
  // curprint("; ");
     curprint(";");
   }


void
Unparse_ExprStmt::unparseForStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // printf ("Unparse for loop \n");
     SgForStatement* for_stmt = isSgForStatement(stmt);
     ROSE_ASSERT(for_stmt != NULL);

     bool saved_unparsedPartiallyUsingTokenStream = info.unparsedPartiallyUsingTokenStream();

#define DEBUG_FOR_STMT 0

#if DEBUG_FOR_STMT
     printf ("In unparseForStmt(): saved_unparsedPartiallyUsingTokenStream = %s \n",saved_unparsedPartiallyUsingTokenStream ? "true" : "false");
     curprint ("/* Top of unparseForStmt */");
#endif

  // curprint ( string("for ("));
  // DQ (12/5/2014): Test for if we have unparsed partially using the token stream. 
  // If so then we don't want to unparse this syntax, if not then we require this syntax.
  // if (info.unparsedPartiallyUsingTokenStream() == false)
     if (saved_unparsedPartiallyUsingTokenStream == false)
        {
#if DEBUG_FOR_STMT
          printf ("In unparseForStmt(): unparse the \"for (\" directly (not using the token stream) \n");
#endif
          curprint ("for (");
        }
       else
        {
       // unparseStatementFromTokenStream (stmt, e_leading_whitespace_start, e_token_subsequence_start);

          SgStatement *tmp_stmt = for_stmt->get_for_init_stmt();

       // Not yet clear how to handle case where tmp_stmt == NULL.
          ROSE_ASSERT(tmp_stmt != NULL);
#if DEBUG_FOR_STMT
          curprint ("/* unparse start of SgForStatement */");
          printf ("In unparseForStmt(): unparse from token stream from start of SgForStatement to for loop initializer \n");
#endif
       // unparseStatementFromTokenStream (for_stmt, tmp_stmt, e_token_subsequence_start, e_token_subsequence_start);
          unparseStatementFromTokenStream (for_stmt, tmp_stmt, e_token_subsequence_start, e_leading_whitespace_start, info);
#if DEBUG_FOR_STMT
          curprint ("/* DONE: unparse start of SgForStatement */");
#endif
        }

     SgUnparse_Info newinfo(info);
     newinfo.set_SkipSemiColon();
     newinfo.set_inConditional();  // set to prevent printing line and file information

#if DEBUG_FOR_STMT
     printf ("In unparseForStmt(): unparse the for loop initializer \n");
#endif

  // curprint(" /* initializer */ ");
     SgStatement *tmp_stmt = for_stmt->get_for_init_stmt();
  // curprint(" /* initializer: " + tmp_stmt->class_name() + " */ ");
  // ROSE_ASSERT(tmp_stmt != NULL);
  // Milind Chabbi (9/5/2013): if the for init statement is NULL or not unparsed, we should output a semicolon. 
     if (tmp_stmt == NULL || !statementFromFile(tmp_stmt, getFileName(), newinfo))
        {
#if 0
       // DQ (10/8/2012): Commented out to avoid output spew.
          printf ("Warning in unparseForStmt(): for_stmt->get_for_init_stmt() == NULL \n");
          curprint("/* Warning in unparseForStmt(): for_stmt->get_for_init_stmt() == NULL */\n ");
#endif
          curprint("; ");
        }
       else
        {
#if DEBUG_FOR_STMT
          curprint("/* Unparse the for_init_stmt */\n ");
          printf ("In unparseForStmt(): unparse the for loop initializer (by calling unparseStatement()) \n");
#endif
          unparseStatement(tmp_stmt,newinfo);

          if (saved_unparsedPartiallyUsingTokenStream == false)
             {
            // DQ (11/4/2015): Change the unparsing semantics to for loop initializer statement to exclude 
            // the " " after the ";" so that we can more faithfully represent the unparsed code when using 
            // the token-based unparsing.
               curprint (" ");
             }

#if DEBUG_FOR_STMT
          curprint("/* DONE: Unparse the for_init_stmt */\n ");
#endif
        }
     newinfo.unset_inConditional();

#if DEBUG_FOR_STMT
     printf ("In unparseForStmt(): unparse the for loop test expression (can be simple declaration statement) \n");
#endif

#if 0

#error "DEAD CODE!"

     SgExpression *tmp_expr = NULL;
     if ( (tmp_expr = for_stmt->get_test_expr()))
          unparseExpression(tmp_expr, info);
#else
  // DQ (12/13/2005): New code for handling the test (which could be a declaration!)
#if DEBUG_FOR_STMT
     printf ("Output the test in the for statement format newinfo.inConditional() = %s \n",newinfo.inConditional() ? "true" : "false");
     curprint (" /* test */ ");
#endif
     SgStatement *test_stmt = for_stmt->get_test();
     ROSE_ASSERT(test_stmt != NULL);
  // if ( test_stmt != NULL )
     SgUnparse_Info testinfo(info);

  // DQ (11/2/2015): Change the unparsing semantics to treat the test as a statement instead 
  // of a conditional.  This should make it processing more uniform and independent of if the 
  // test statement is unparsed using either the AST or the token stream.
  // testinfo.set_SkipSemiColon();
  // testinfo.set_inConditional();

  // DQ (11/2/2015): With the new change to not set SkipSemiColon and inConditional unparse 
  // info fields, we have to explicitly specify that we want to skip class elaboration as 
  // well (SkipClassSpecifier).  See test2015_110.C and other older test codes.  Additionally,
  // we need to make this as a conditional so that it will be unparsed as "type var = value"
  // instead of "type var(value)" in the case of a class constructor initialization call.
  // This is because the syntax required for C++ in a condition is that of a simple declaration
  // which is a part of C++ syntax not directly supported in ROSE for simplicity.
     testinfo.set_SkipClassSpecifier();
     testinfo.set_inConditional();

#if DEBUG_FOR_STMT
     printf ("Output the test in the for statement format testinfo.inConditional() = %s \n",testinfo.inConditional() ? "true" : "false");
#endif
     unparseStatement(test_stmt, testinfo);
#endif
#if DEBUG_FOR_STMT
     printf ("In unparseForStatement(): saved_unparsedPartiallyUsingTokenStream = %s \n",saved_unparsedPartiallyUsingTokenStream ? "true" : "false");
     printf ("In unparseForStatement(): test_stmt->isTransformation()           = %s \n",test_stmt->isTransformation() ? "true" : "false");
#endif
  // DQ (4/6/2015): If the test is a transformation, then we have to output the semi-colon directly (see inliner tutorial test).
     if (saved_unparsedPartiallyUsingTokenStream == true && test_stmt->isTransformation() == true)
        {
       // ROSE_ASSERT(test_stmt->isTransformation() == true);
       // curprint (" /* output semi-colon at end of test */ ");

       // DQ (11/2/2015): Change the unparsing semantics to treat the test as a statement instead 
       // of a conditional.  This should make it processing more uniform and independe of if the 
       // test statement is unparsed using either the AST or the token stream.
       // curprint (";");
        }

     if (saved_unparsedPartiallyUsingTokenStream == false)
        {
       // DQ (11/2/2015): Change the unparsing semantics to treat the test as a statement instead 
       // of a conditional.  This should make it processing more uniform and independe of if the 
       // test statement is unparsed using either the AST or the token stream.  However, we want
       // to add a space after the test and before the increment to make the generated code better 
       // looking.
          curprint (" ");
        }

#if DEBUG_FOR_STMT
     printf ("In unparseForStmt(): unparse the for loop increment expression \n");
#endif

#if 0
     curprint ( string("; "));

  // curprint ( string(" /* increment */ ";
  // SgExpression *increment_expr = for_stmt->get_increment_expr();
     SgExpression *increment_expr = for_stmt->get_increment();
     ROSE_ASSERT(increment_expr != NULL);
     if ( increment_expr != NULL )
          unparseExpression(increment_expr, info);
     curprint ( string(") "));
#endif

  // DQ (10/14/2015): If the test was unparsed from the AST then we need to unparse the increment from the AST,
  // but it the test was unparsed as parrt of a partial token stream unparse of the SgForStatement, then we can
  // unparse the increment from the token stream (as a continuation of the use of the token stream in the test).
  // DQ (12/5/2014): Test for if we have unparsed partially using the token stream. 
  // If so then we don't want to unparse this syntax, if not then we require this syntax.
  // if (info.unparsedPartiallyUsingTokenStream() == false)
  // if (saved_unparsedPartiallyUsingTokenStream == false)
  // if (saved_unparsedPartiallyUsingTokenStream == false && test_stmt->isTransformation() == true)
  // if (test_stmt->isTransformation() == true)
  // if (saved_unparsedPartiallyUsingTokenStream == false || test_stmt->isTransformation() == false)
#if 0
  // Original code: "if (saved_unparsedPartiallyUsingTokenStream == false)"
     if (saved_unparsedPartiallyUsingTokenStream == false || test_stmt->isTransformation() == true)
        {
          curprint (" /* output semi-colon before increment */ ");
          curprint("; ");
          SgExpression *increment_expr = for_stmt->get_increment();
          ROSE_ASSERT(increment_expr != NULL);
          if ( increment_expr != NULL )
               unparseExpression(increment_expr, info);
          curprint(") ");
        }
#else
  // if (saved_unparsedPartiallyUsingTokenStream == false)
  // if (saved_unparsedPartiallyUsingTokenStream == false || test_stmt->isTransformation() == false)
  // if (test_stmt->isTransformation() == true)
  // if (saved_unparsedPartiallyUsingTokenStream == false && test_stmt->isTransformation() == true)
  // if (test_stmt->isTransformation() == true)
  // if (saved_unparsedPartiallyUsingTokenStream == false)
  // if ( (saved_unparsedPartiallyUsingTokenStream == false || test_stmt->isTransformation() == false) )
     if (saved_unparsedPartiallyUsingTokenStream == false)
        {
       // curprint (" /* output semi-colon before increment */ ");

       // DQ (11/2/2015): Change the unparsing semantics to treat the test as a statement instead 
       // of a conditional.  This should make it processing more uniform and independe of if the 
       // test statement is unparsed using either the AST or the token stream.
       // curprint("; ");
        }

     if (saved_unparsedPartiallyUsingTokenStream == false)
        {
          SgExpression *increment_expr = for_stmt->get_increment();
          ROSE_ASSERT(increment_expr != NULL);
          if ( increment_expr != NULL )
               unparseExpression(increment_expr, info);
          curprint(") ");
        }
#endif
       else
        {
       // DQ (12/15/2014): Note that the increment expression is not a Statement, so it will be unparsed in the 
       // token stream of the body (assuming it is unparsed via the token stream).  Note clear how to look ahead
       // to check this or make the unparsing of the increment expression conditional upon this.
#if 1
       // SgStatement *tmp_stmt = for_stmt->get_for_init_stmt();
          SgStatement* body = for_stmt->get_loop_body();

       // Not yet clear how to handle case where tmp_stmt == NULL.
          ROSE_ASSERT(test_stmt != NULL);
          ROSE_ASSERT(body != NULL);

       // If this is compiler generated this this must be handled similarly as to the SgIfStmt with compiler generated body.
          ROSE_ASSERT(body->isCompilerGenerated() == false);

#if DEBUG_FOR_STMT
          curprint ("/* unparse increment expression in SgForStatement header */");
#endif
       // DQ (12/16/2014): When a SgBasicBlock has been substituted for the loop_body then there is not associated token stream (see test2014_14.C).
       // In this case it is better to use the start and end of the trailing whitespace subsequence.
       // unparseStatementFromTokenStream (test_stmt, body, e_trailing_whitespace_start, e_token_subsequence_start);
          unparseStatementFromTokenStream (test_stmt, e_trailing_whitespace_start, e_trailing_whitespace_end, info);
       // curprint("/* syntax from partial token unparse */ )");
       // SgStatement* loopBody = for_stmt->get_loop_body();
       // unparseStatementFromTokenStream (test_stmt, loopBody, e_trailing_whitespace_end, e_leading_whitespace_start);

       // DQ (1/24/2015): Supress the output of the increment expression when it is a SgNullExpression (which does not have an associated token sequence).
       // unparseStatementFromTokenStream (test_stmt, increment_expr, e_trailing_whitespace_end, e_trailing_whitespace_end);
          SgExpression *increment_expr = for_stmt->get_increment();
          if (isSgNullExpression(increment_expr) == NULL)
             {
               unparseStatementFromTokenStream (test_stmt, increment_expr, e_trailing_whitespace_end, e_trailing_whitespace_end, info);
             }
#if 0
          curprint("/* syntax from partial token unparse */");
#endif
          curprint(")");
#if 0
          curprint ("/* unparse end of SgForStatement header */");
#endif
#endif
        }


  // Added support to output the header without the body to support the addition 
  // of more context in the prefix used with the AST Rewrite Mechanism.
  // if ( (tmp_stmt = for_stmt->get_loop_body()) )

     SgStatement* loopBody = for_stmt->get_loop_body();
     ROSE_ASSERT(loopBody != NULL);
  // printf ("loopBody = %p         = %s \n",loopBody,loopBody->class_name().c_str());
  // printf ("info.SkipBasicBlock() = %s \n",info.SkipBasicBlock() ? "true" : "false");

  // if ( (tmp_stmt = for_stmt->get_loop_body()) && !info.SkipBasicBlock())
     if ( (loopBody != NULL) && !info.SkipBasicBlock())
        {
#if DEBUG_FOR_STMT
          printf ("Unparse the for loop body \n");
          curprint("/* Unparse the for loop body */ ");
#endif
       // unparseStatement(tmp_stmt, info);

          unp->cur.format(loopBody, info, FORMAT_BEFORE_NESTED_STATEMENT);
          unparseStatement(loopBody, info);
          unp->cur.format(loopBody, info, FORMAT_AFTER_NESTED_STATEMENT);
#if DEBUG_FOR_STMT
          curprint("/* DONE: Unparse the for loop body */ ");
#endif
        }
       else
        {
       // printf ("No for loop body to unparse! \n");
       // curprint ( string("\n/* No for loop body to unparse! */ \n";
          if (!info.SkipSemiColon())
             {
               curprint ( string(";"));
             }
        }
   }



void
Unparse_ExprStmt::unparseRangeBasedForStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // printf ("Unparse range-based for loop \n");
     SgRangeBasedForStatement* for_stmt = isSgRangeBasedForStatement(stmt);
     ROSE_ASSERT(for_stmt != NULL);

#define DEBUG_RANGE_BASED_FOR_STMT 0

#if DEBUG_RANGE_BASED_FOR_STMT
     bool saved_unparsedPartiallyUsingTokenStream = info.unparsedPartiallyUsingTokenStream();

     printf ("In unparseRangeBasedForStmt(): saved_unparsedPartiallyUsingTokenStream = %s \n",saved_unparsedPartiallyUsingTokenStream ? "true" : "false");
     curprint ("/* Top of unparseForStmt */");
#endif

  // printf ("ERROR: Range-based For statement unparseRangeBasedForStmt() not implemented \n");

     curprint("for ( ");

     SgVariableDeclaration* interator_declaration = for_stmt->get_iterator_declaration();
     ROSE_ASSERT(interator_declaration != NULL);

     SgUnparse_Info ninfo(info);

  // Need to suppress the output of the semicolon in the output of the variable declaration.
     ninfo.set_SkipSemiColon();
     ninfo.set_SkipInitializer();

     unparseStatement(interator_declaration, ninfo);

     curprint(" : ");

  // SgVarRefExp* range_variable = for_stmt->range_variable_reference();
  // ROSE_ASSERT(range_variable != NULL);
     SgExpression* range_expression = for_stmt->range_expression();
     ROSE_ASSERT(range_expression != NULL);

     unparseExpression(range_expression, info);

     curprint(" )");

     SgStatement* loopBody = for_stmt->get_loop_body();
     ROSE_ASSERT(loopBody != NULL);
  // printf ("loopBody = %p         = %s \n",loopBody,loopBody->class_name().c_str());
  // printf ("info.SkipBasicBlock() = %s \n",info.SkipBasicBlock() ? "true" : "false");

     if ( (loopBody != NULL) && !info.SkipBasicBlock())
        {
#if DEBUG_RANGE_BASED_FOR_STMT
          printf ("Unparse the for loop body \n");
          curprint("/* Unparse the for loop body */ ");
#endif
          unp->cur.format(loopBody, info, FORMAT_BEFORE_NESTED_STATEMENT);
          unparseStatement(loopBody, info);
          unp->cur.format(loopBody, info, FORMAT_AFTER_NESTED_STATEMENT);

#if DEBUG_RANGE_BASED_FOR_STMT
          curprint("/* DONE: Unparse the range-based for loop body */ ");
#endif
        }
       else
        {
       // printf ("No range-based for loop body to unparse! \n");
       // curprint ( string("\n/* No range-based for loop body to unparse! */ \n";
          if (!info.SkipSemiColon())
             {
               curprint ( string(";"));
             }
        }

   }

void
Unparse_ExprStmt::unparseExceptionSpecification(const SgTypePtrList& exceptionSpecifierList, SgUnparse_Info& info)
   {
  // DQ (6/27/2006): Added support for throw modifier and its exception specification lists

#if 0
     printf ("Inside of unparseExceptionSpecification() \n");
#endif

     curprint ( string(" throw("));
     if (!exceptionSpecifierList.empty())
        {
          SgTypePtrList::const_iterator i = exceptionSpecifierList.begin();
          while (i != exceptionSpecifierList.end())
             {
            // Handle class type as a special case to make sure the names are always output (see test2004_91.C).
            // unparseType(*i,info);
            // printf ("Note: Type found in function throw specifier type = %p = %s \n",*i,i->class_name().c_str());

               ROSE_ASSERT(*i != NULL);
#if 0
               SgClassType* classType = isSgClassType(*i);
               if (classType != NULL)
                  {
                    SgDeclarationStatement* declaration = classType->get_declaration();
                    SgClassDeclaration* classDeclaration = isSgClassDeclaration(declaration);
                    ROSE_ASSERT(classDeclaration != NULL);
                 // printf ("     exception specification parameter: classDeclaration->get_name() = %s \n",classDeclaration->get_name().str());
                    ROSE_ASSERT(classDeclaration->get_name().is_null() == false);
                    curprint ( classDeclaration->get_name().str());
                  }
                 else
                  {
                 // Handle other types using the normal unparsing
                    unparseType(*i,info);
                  }
#else
            // DQ (6/2/2011): Added support for name qualification.
               info.set_reference_node_for_qualification(*i);
               ROSE_ASSERT(info.get_reference_node_for_qualification() != NULL);

               unp->u_type->unparseType(*i,info);

            // DQ (6/2/2011): Since we are not using a new SgUnparse_Info object, clear the reference node for name qualification after it has been used.
               info.set_reference_node_for_qualification(NULL);
#endif
               i++;
               if (i != exceptionSpecifierList.end())
                  curprint ( string(","));
             }
        }
       else
        {
       // There was no exception specification list of types
        }
     curprint ( string(")"));
   }


// DQ (11/7/2007): Make this a more general function so that we can use it for the unparsing of SgClassDeclaration objects too.
// void fixupScopeInUnparseInfo ( SgUnparse_Info& ninfo , SgFunctionDeclaration* functionDeclaration )
void
fixupScopeInUnparseInfo ( SgUnparse_Info& ninfo , SgDeclarationStatement* declarationStatement )
   {
  // DQ (11/3/2007): This resets the current scope stored in the SgUnparse_Info object so that the name qualification will work properly.
  // It used to be that this would be the scope of the caller (which for unparsing the function prototype would be the outer scope (OK), 
  // but for function definitions would be the scope of the function definition (VERY BAD).  Because of a previous bug (just fixed) in the 
  // SgStatement::get_scope() function, the scope of the SgFunctionDefinition would be set to the parent of the SgFunctionDeclaration 
  // (structural) instead of the scope of the SgFunctionDeclaration (semantics). It is the perfect example of two bugs working together
  // to be almost always correct :-).  Note that "ninfo" will go out of scope, so we don't have to reset it at the end of this function.
  // Note that that it is FROM this scope that the name qualification is computed, so this is structural, not semantic.

  // DQ (11/9/2007): If we want to force the use of qualified names then don't reset the internal scope (required for new ROSE Doxygen 
  // documentation generator).
     if (ninfo.forceQualifiedNames() == false)
        {
          SgScopeStatement* currentScope = isSgScopeStatement(declarationStatement->get_parent());

          if (currentScope == NULL)
             {
            // printf ("In fixupScopeInUnparseInfo(): declarationStatement = %p = %s = %s \n",declarationStatement,declarationStatement->class_name().c_str(),SageInterface::get_name(declarationStatement).c_str());
               SgNode* parentOfFunctionDeclaration = declarationStatement->get_parent();
               ROSE_ASSERT(parentOfFunctionDeclaration != NULL);

               switch (parentOfFunctionDeclaration->variantT())
                  {
                 // This is one way that the funcdecl_stmt->get_parent() can not be a SgScopeStatement (there might be a few more!)
                    case V_SgTemplateInstantiationDirectiveStatement:
                       {
                         SgTemplateInstantiationDirectiveStatement* directive = isSgTemplateInstantiationDirectiveStatement(parentOfFunctionDeclaration);
                      // currentScope = isSgScopeStatement(funcdecl_stmt->get_parent()->get_parent());
                         currentScope = directive->get_scope();
                         break;
                       }

                    case V_SgVariableDeclaration:
                       {
                         SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(parentOfFunctionDeclaration);
                      // currentScope = isSgScopeStatement(funcdecl_stmt->get_parent()->get_parent());
                         currentScope = variableDeclaration->get_scope();
                         break;
                       }

                 // DQ (6/19/2012): Added case to support test2012_103.C.
                    case V_SgForInitStatement:
                       {
                         SgForInitStatement* forInitDeclaration = isSgForInitStatement(parentOfFunctionDeclaration);
                      // currentScope = isSgScopeStatement(funcdecl_stmt->get_parent()->get_parent());
                         currentScope = forInitDeclaration->get_scope();
                         ROSE_ASSERT(currentScope != NULL);
                         break;
                       }

                 // DQ (2/16/2014): The SystemC example (in systemc_tests) demonstrates where this case must be handled.
                 // I think it should be the scope of the SgTypedefDeclaration.
                    case V_SgTypedefDeclaration:
                       {
                         SgTypedefDeclaration* declaration = isSgTypedefDeclaration(parentOfFunctionDeclaration);
                         currentScope = declaration->get_scope();
                         break;
                       }

                    case V_SgLambdaExp:
                       {
                         // This happens when calling unparseToString on the function declaration associated with a lambda
                         printf("WARNING: In fixupScopeInUnparseInfo: Case of a lambda expression !!!\n");
                         SgLambdaExp * lambda = isSgLambdaExp(parentOfFunctionDeclaration);
                         currentScope = SageInterface::getEnclosingStatement(lambda)->get_scope();
                         break;
                       }

                    default:
                       {
                         printf ("Error: default reached in evaluation of function declaration structural location parentOfFunctionDeclaration = %s \n",parentOfFunctionDeclaration->class_name().c_str());
                         printf ("     declarationStatement = %p = %s = %s \n",declarationStatement,declarationStatement->class_name().c_str(),SageInterface::get_name(declarationStatement).c_str());
                         declarationStatement->get_startOfConstruct()->display("default reached: debug");
                         ROSE_ASSERT(false);
                       }
                  }
             }

       // printf ("In fixupScopeInUnparseInfo(): currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
          ROSE_ASSERT(currentScope != NULL);

          ninfo.set_current_scope(currentScope);
        }

  // printf ("Set current scope (stored in ninfo): currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
   }
 
 
void
Unparse_ExprStmt::unparseFuncDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgFunctionDeclaration* funcdecl_stmt = isSgFunctionDeclaration(stmt);
     ROSE_ASSERT(funcdecl_stmt != NULL);

#if 0
     printf ("Inside of unparseFuncDeclStmt(): name = %p = %s \n",funcdecl_stmt,funcdecl_stmt->get_name().str());
  // curprint ( string("/* Inside of Unparse_ExprStmt::unparseFuncDeclStmt */";
     curprint ( string("\n/* Inside of Unparse_ExprStmt::unparseFuncDeclStmt (" ) + StringUtility::numberToString(stmt) 
                + "): class_name() = " + stmt->class_name().c_str() + " */ \n");

  // stmt->get_startOfConstruct()->display("Inside of unparseFuncDeclStmt(): START debug");
  // stmt->get_endOfConstruct()  ->display("Inside of unparseFuncDeclStmt(): END   debug");

  // info.display("Inside of unparseFuncDeclStmt()");
#endif

  // printf ("In unparseFuncDeclStmt(): info.get_current_scope() = %p = %s \n",info.get_current_scope(),info.get_current_scope()->class_name().c_str());

  // if (stmt->get_startOfConstruct()->isOutputInCodeGeneration()==false)
  //      return;

#if 0
  // DQ (10/26/2018): We might not need this code now that I have fixed a cut and paste error 
  // in the latest debugging of the unparseStatementFromTokenStream() function.
  // DQ (10/25/2018): Test for if we have unparsed partially using the token stream.
  // If so then we don't want to unparse this syntax, if not then we require this syntax.
     bool saved_unparsedPartiallyUsingTokenStream = info.unparsedPartiallyUsingTokenStream();
     if (saved_unparsedPartiallyUsingTokenStream == false)
        {
          printf ("I think there is nothing to do here! \n");

#if 1
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }
       else
        {
       // unparseStatementFromTokenStream (stmt, e_token_subsequence_start, e_token_subsequence_start);
       // unparseStatementFromTokenStream (stmt, e_token_subsequence_start, e_token_subsequence_end);
          SgFunctionDefinition* function_definition = funcdecl_stmt->get_definition();
          ROSE_ASSERT(function_definition != NULL);
          SgStatement* function_body = function_definition->get_body();
          ROSE_ASSERT(function_body != NULL);
          unparseStatementFromTokenStream (stmt, function_body, e_token_subsequence_start, e_token_subsequence_start, info);
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }
#endif

  // DQ (1/19/2014): Adding support for attributes that must be prefixed to the function declarations (e.g. "__attribute__((regnum(3)))").
  // It is output here for non-defining declarations, but in unparseFuncDefnStmt() function for the attribute to be associated with the defining declaration.
  // unp->u_sage->printPrefixAttributes(funcdecl_stmt,info);
     if (funcdecl_stmt->isForward() == true) 
        {
          unp->u_sage->printPrefixAttributes(funcdecl_stmt,info);
        }
#if 0
       else
        {
          if (funcdecl_stmt == funcdecl_stmt->get_definingDeclaration())
             {
               unp->u_sage->printPrefixAttributes(funcdecl_stmt,info);
             }
        }
#endif

#if 0
  // DQ (5/12/2017): Commented this out appears to cause a problem for Fortran tests in the:
  //    tests/nonsmoke/functional/roseTests/astOutliningTests directory.
  // DQ (5/10/2017): This should be commented out (since we now use the specification of the nondefining 
  // declaration as "output in generated code" to determine when it should be output).

  // DQ (11/27/2015): The updated support for templates demonstrates that we need this code (see test2004_37.C).
  // However, the larger issue is that the defining function declaration should not have been output, which is 
  // the root cause of this problem.
  // DQ (8/19/2012): I don't think I like how we are skipping forward declarations here (need to understand this better).
  // Liao, 9/25/2009, skip the compiler generated forward declaration for a SgTemplateInstantiationFunctionDecl
  // see bug 369: https://outreach.scidac.gov/tracker/index.php?func=detail&aid=369&group_id=24&atid=185
     if (funcdecl_stmt->isForward() == true) 
        {
          SgFunctionDeclaration* def_decl = isSgFunctionDeclaration(funcdecl_stmt->get_definingDeclaration());
          if (def_decl != NULL)
             {
               if (isSgTemplateInstantiationFunctionDecl(def_decl))
                  {
                 // cout<<"Skipping a forward declaration of a template instantiation function declaration..."<<endl;
#if 0
                    printf ("In unparseFuncDeclStmt(): Skipping a forward declaration of a template instantiation function declaration... \n");
#endif
#if 0
                    curprint("/* In unparseFuncDeclStmt(): Skipping a forward declaration of a template instantiation function declaration...*/ \n");
#endif
                    return;
                  }
             }
        }
#endif

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
     curprint("\n/* */");
#endif

#if 0
  // DQ (10/20/2012): Unparse any comments of directives attached to the SgFunctionParameterList
     ROSE_ASSERT (funcdecl_stmt != NULL);
     printf ("Output comments and CCP directives for funcdecl_stmt->get_parameterList() \n");
     if (funcdecl_stmt->get_parameterList() != NULL)
        {
          printf ("Output the comments and CCP directives for the SgFunctionDeclaration funcdecl_stmt = %p \n",funcdecl_stmt);

          printf ("funcdecl_stmt                                    = %p \n",funcdecl_stmt);
          printf ("funcdecl_stmt->get_firstNondefiningDeclaration() = %p \n",funcdecl_stmt->get_firstNondefiningDeclaration());
          printf ("funcdecl_stmt->get_definingDeclaration()         = %p \n",funcdecl_stmt->get_definingDeclaration());
          printf ("funcdecl_stmt->get_parameterList()               = %p \n",funcdecl_stmt->get_parameterList());

          unparseAttachedPreprocessingInfo(funcdecl_stmt->get_parameterList(), info, PreprocessingInfo::before);
          printf ("DONE: Output the comments and CCP directives for the SgFunctionDeclaration funcdecl_stmt = %p \n",funcdecl_stmt);
        }
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

     fixupScopeInUnparseInfo (ninfo,funcdecl_stmt);

  // DQ (10/10/2006): Do output any qualified names (particularly for non-defining declarations).
  // ninfo.set_forceQualifiedNames();

#if 0
     printf ("funcdecl_stmt->isForward()      = %s \n",funcdecl_stmt->isForward() ? "true" : "false");
     printf ("funcdecl_stmt->get_definition() = %s \n",funcdecl_stmt->get_definition() ? "true" : "false");
     printf ("info.SkipFunctionDefinition()   = %s \n",info.SkipFunctionDefinition() ? "true" : "false");
#endif


  // if (!funcdecl_stmt->isForward() && funcdecl_stmt->get_definition() && !info.SkipFunctionDefinition())
     if ( (funcdecl_stmt->isForward() == false) && (funcdecl_stmt->get_definition() != NULL) && (info.SkipFunctionDefinition() == false) )
        {
#if 0
          printf ("Not a forward function (normal function) \n");
          curprint("\n/* Not a forward function (normal function) */ \n ");
#endif
       // DQ (12/5/2007): This call to unparse the definition can change the scope in info, so save it and restore it
       // SgScopeStatement* savedScope = info.get_current_scope();

       // DQ (6/23/2015): Added output of type attributes for defining function declaration (see test2015_164.c).
       // Within GNU the attribute can appear before or after the return type, here it is before the return type.
       // unp->u_sage->printAttributes(funcdecl_stmt,info);
       // unp->u_sage->printAttributesForType(funcdecl_stmt,info);

       // DQ (12/3/2007): We want the changes to the access state to be saved in
       // the info object. See test2007_172.C for example of why this as a problem,
       // though it is not clear that a private friend is any different than a 
       // public friend.
#if 0
          printf ("Calling unparseStatement(): using funcdecl_stmt->get_definition() = %p = %s \n",funcdecl_stmt->get_definition(),funcdecl_stmt->get_definition()->class_name().c_str());
#endif
       // unparseStatement(funcdecl_stmt->get_definition(), ninfo);
          unparseStatement(funcdecl_stmt->get_definition(), info);

       // printf ("After call to unparseStatement(funcdecl_stmt->get_definition(), info): info.get_current_scope() = %p = %s \n",info.get_current_scope(),info.get_current_scope()->class_name().c_str());
       // info.set_current_scope(savedScope);
       // printf ("After restoring the saved scope: info.get_current_scope() = %p = %s \n",info.get_current_scope(),info.get_current_scope()->class_name().c_str());

          if (funcdecl_stmt->isExternBrace())
             {
               curprint(" }");
             }
        }
       else
        {
#if 0
          printf ("Forward function (function prototype) \n");
          curprint("\n/* Forward function (function prototype) */ \n");
#endif
          SgClassDefinition *cdefn = isSgClassDefinition(funcdecl_stmt->get_parent());

          if (cdefn && cdefn->get_declaration()->get_class_type() == SgClassDeclaration::e_class)
               ninfo.set_CheckAccess();

       // printf ("Comment out call to get_suppress_atomic(funcdecl_stmt) \n");
#if 0
#error "DEAD CODE!"
          if (get_suppress_atomic(funcdecl_stmt))
               ninfo.set_SkipAtomic();   // attributes.h
#endif

       // DQ (10/17/2004): Skip output of class definition for function declaration! C++ standard does not permit 
       // a defining declaration within a return type, function parameter, or sizeof expression. And by extention 
       // any function declaration!
          ninfo.set_SkipClassDefinition();
          ninfo.set_SkipEnumDefinition();

       // DQ (3/4/2010): Added support for asm functions (see test2010_12.C).
          SgStorageModifier & storage = funcdecl_stmt->get_declarationModifier().get_storageModifier();
       // printf ("storage.isAsm() = %s \n",storage.isAsm() ? "true" : "false");
          if (storage.isAsm() == true)
             {
            // DQ (2/6/2014): Modified to support gcc.
            // curprint("asm ");
               curprint("__asm__ ");
             }

#if 0
          curprint( "\n/* Calling printSpecifier() */ ");
#endif
          unp->u_sage->printSpecifier(funcdecl_stmt, ninfo);
#if 0
          curprint( "\n/* DONE: Calling printSpecifier() */ ");
#endif

       // DQ (10/21/2013): In cases where this is a function declaration the "extern" keyword may have been in a #if...#endif block and then
       // the #endif will be placed as "inside" the statement (instead of at the beginning or the end.  In this case we want to unparse
       // any attached CPP directives (or comments) that are listed as "inside" the declaration after the unparsing of the specifiers (e.g. "extern").
       // This is a new bug that is a result of having better source position information (required for the token handling support) and 
       // getting the source position of the function declaration at the start of the specifiers (where this is available).  However,
       // this then means that the "#end" is between the start and the end fo the statement (thus the "#end is considered "inside" the
       // function declaration statement.  Previously the source position for the start of a function declaration had been the start of the
       // function name (and not any specifiers).  Test code test2013_271.C and test2013_272.C demonstrate this problem.  So here well
       // unparse any attached CPP directives that are marked as inside the declaration.  We can't actually know where the CPP directived
       // should go relative to the different parts of the declaration, so this is approximate.  But at least we know that the CPP directive
       // will be unparsed and this avoids unmatched #if...#endif pairings.  A more faithful job can only be done by the token based unparsing
       // which is still in development.
          unparseAttachedPreprocessingInfo(stmt, info, PreprocessingInfo::inside);
#if 0
          curprint( "\n/* DONE: In unparseFuncDeclStmt(): calling unparseAttachedPreprocessingInfo() */ ");
#endif
          ninfo.unset_CheckAccess();

       // DQ (11/10/2007): Modified from info.set_access_attribute(...) --> ninfo.set_access_attribute(...)
          info.set_access_attribute(ninfo.get_access_attribute());

          SgType *rtype = funcdecl_stmt->get_orig_return_type();
          if (!rtype)
             {
               rtype = funcdecl_stmt->get_type()->get_return_type();
             }
          ninfo.set_isTypeFirstPart();

          SgUnparse_Info ninfo_for_type(ninfo);

       // DQ (12/20/2006): This is used to specify global qualification separately from the more general name 
       // qualification mechanism.  Note that SgVariableDeclarations don't use the requiresGlobalNameQualificationOnType
       // on the SgInitializedNames in their list since the SgVariableDeclaration IR nodes is marked directly.
       // printf ("funcdecl_stmt->get_requiresNameQualificationOnReturnType() = %s \n",funcdecl_stmt->get_requiresNameQualificationOnReturnType() ? "true" : "false");
       // curprint ( string("\n/* funcdecl_stmt->get_requiresNameQualificationOnReturnType() = " + (funcdecl_stmt->get_requiresNameQualificationOnReturnType() ? "true" : "false") + " */ \n";
          if (funcdecl_stmt->get_requiresNameQualificationOnReturnType() == true)
             {
            // Output the name qualification for the type in the variable declaration.
            // But we have to do so after any modifiers are output, so in unp->u_type->unparseType().
            // printf ("In Unparse_ExprStmt::unparseFunctionDeclaration(): This return type requires a global qualifier \n");

            // Note that general qualification of types is separated from the use of globl qualification.
            // ninfo2.set_forceQualifiedNames();
               ninfo_for_type.set_requiresGlobalNameQualification();
             }

       // output the return type
#define OUTPUT_FUNCTION_DECLARATION_DATA 0
#if OUTPUT_FUNCTION_DECLARATION_DATA
          printf ("rtype = %p = %s \n",rtype,rtype->class_name().c_str());
          curprint ("\n/* output the return type */ \n");
#endif
#if 0
          curprint ("\n/* output the return type */ \n");
#endif
       // DQ (5/30/2011): Added support for name qualification.
          ninfo_for_type.set_reference_node_for_qualification(funcdecl_stmt);
          ROSE_ASSERT(ninfo_for_type.get_reference_node_for_qualification() != NULL);

          ninfo_for_type.set_name_qualification_length(funcdecl_stmt->get_name_qualification_length_for_return_type());
          ninfo_for_type.set_global_qualification_required(funcdecl_stmt->get_global_qualification_required_for_return_type());
          ninfo_for_type.set_type_elaboration_required(funcdecl_stmt->get_type_elaboration_required_for_return_type());

       // unp->u_type->unparseType(rtype, ninfo);
          unp->u_type->unparseType(rtype, ninfo_for_type);

       // output the rest of the function declaration
#if OUTPUT_FUNCTION_DECLARATION_DATA
          curprint ("/* after unparsing the return type */");
#endif

#if 0
          printf ("funcdecl_stmt->isForward() = %s \n",funcdecl_stmt->isForward() ? "true" : "false");
          printf ("ninfo.SkipSemiColon()      = %s \n",ninfo.SkipSemiColon() ? "true" : "false");
#endif

// #if 1
       // DQ (6/23/2015): Output the GNU attribute where it appears between the return type and the function name.
       // unp->u_sage->printAttributes(funcdecl_stmt,info);
       // if (funcdecl_stmt->isForward() == false && !ninfo.SkipSemiColon())
          if (funcdecl_stmt->isForward() == false)
             {
#if 0
               curprint ("/* before unparsing the printAttributes */");
#endif
               unp->u_sage->printAttributes(funcdecl_stmt,info);

#if 0
               curprint ("/* after unparsing the printAttributes */");
#endif
             }
// #endif

       // DQ (10/15/2006): Mark that we are unparsing a function declaration (or member function declaration)
       // this will help us know when to trim the "::" prefix from the name qualiciation.  The "::" global scope
       // qualifier is not used in function declarations, but is used for function calls.
          ninfo.set_declstatement_ptr(NULL);
          ninfo.set_declstatement_ptr(funcdecl_stmt);

       // DQ (2/22/2009): Added assertion.
          if (funcdecl_stmt->get_firstNondefiningDeclaration() != NULL)
             {
               SgFunctionDeclaration* firstNondefiningFunction = isSgFunctionDeclaration(funcdecl_stmt->get_firstNondefiningDeclaration());
               ROSE_ASSERT(firstNondefiningFunction != NULL);
               ROSE_ASSERT(firstNondefiningFunction->get_firstNondefiningDeclaration() != NULL);
#if 0
               printf ("firstNondefiningFunction                                    = %p \n",firstNondefiningFunction);
               printf ("firstNondefiningFunction->get_firstNondefiningDeclaration() = %p \n",firstNondefiningFunction->get_firstNondefiningDeclaration());
               printf ("firstNondefiningFunction->get_definingDeclaration()         = %p \n",firstNondefiningFunction->get_definingDeclaration());
               printf ("firstNondefiningFunction->get_scope()                       = %p \n",firstNondefiningFunction->get_scope());
#endif
#if 0
               SgSourceFile* sourceFile = TransformationSupport::getSourceFile(funcdecl_stmt);
               printf ("sourceFile->getFileName() = %s \n",sourceFile->getFileName().c_str());
#endif

               if (firstNondefiningFunction != NULL)
                  {
                    ROSE_ASSERT(TransformationSupport::getSourceFile(funcdecl_stmt) != NULL);
                    ROSE_ASSERT(TransformationSupport::getSourceFile(firstNondefiningFunction) != NULL);
#if 0
                 // DQ (7/24/2018): This will be output spew from the Cxx_tests test_multiple_files rule (compiling two files with "void foo();" in each file.
                 // I am guessing that if comes from the use of a common global scope across the two files.
                    if (TransformationSupport::getSourceFile(funcdecl_stmt) != TransformationSupport::getSourceFile(firstNondefiningFunction))
                       {
                         printf ("firstNondefiningFunction = %p \n",firstNondefiningFunction);
                         printf ("funcdecl_stmt = %p funcdecl_stmt->get_scope()                                        = %p \n",funcdecl_stmt,funcdecl_stmt->get_scope());
                         printf ("funcdecl_stmt = %p funcdecl_stmt->get_declarationModifier().isFriend()               = %s \n",funcdecl_stmt,funcdecl_stmt->get_declarationModifier().isFriend() ? "true" : "false");
                         printf ("firstNondefiningFunction = %p firstNondefiningFunction->get_declarationModifier().isFriend() = %s \n",firstNondefiningFunction,firstNondefiningFunction->get_declarationModifier().isFriend() ? "true" : "false");
                         printf ("TransformationSupport::getSourceFile(funcdecl_stmt = %p)->getFileName()              = %s \n",funcdecl_stmt,TransformationSupport::getSourceFile(funcdecl_stmt)->getFileName().c_str());
                         printf ("TransformationSupport::getSourceFile(funcdecl_stmt->get_scope() = %p)->getFileName() = %s \n",funcdecl_stmt->get_scope(),TransformationSupport::getSourceFile(funcdecl_stmt->get_scope())->getFileName().c_str());
                         printf ("TransformationSupport::getSourceFile(firstNondefiningFunction = %p)->getFileName()   = %s \n",firstNondefiningFunction,TransformationSupport::getSourceFile(firstNondefiningFunction)->getFileName().c_str());
                         printf ("TransformationSupport::getSourceFile(firstNondefiningFunction->get_scope() = %p)->getFileName() = %s \n",firstNondefiningFunction->get_scope(),TransformationSupport::getSourceFile(firstNondefiningFunction->get_scope())->getFileName().c_str());

                         printf ("TransformationSupport::getSourceFile(funcdecl_stmt = %p)->getFileName()            = %s \n",funcdecl_stmt,TransformationSupport::getSourceFile(funcdecl_stmt)->getFileName().c_str());
                         printf ("TransformationSupport::getSourceFile(firstNondefiningFunction = %p)->getFileName() = %s \n",firstNondefiningFunction,TransformationSupport::getSourceFile(firstNondefiningFunction)->getFileName().c_str());
                       }
#endif

                 // TV (07/01/2013) : prevent separated generation of decl and defn over an header file and a source file
                 // RPM (12/30/2013): Commented out because this fails for astSnippetTests which deep-copy a function
                 // declaration with definition and then call SageInterface::insertStatementBefore() to insert the copied
                 // ast into a different file.  Granted, this isn't a robust way to copy code from one AST into another
                 // because it doesn't try to resolve any conflicts with symbols, but if the following assertion is
                 // commented out it at least works.
                    //ROSE_ASSERT(TransformationSupport::getSourceFile(funcdecl_stmt) == TransformationSupport::getSourceFile(firstNondefiningFunction));

#if 0
                 // DQ (7/24/2018): This will be output spew from the Cxx_tests test_multiple_files rule (compiling two files with "void foo();" in each file.
                 // I am guessing that if comes from the use of a common global scope across the two files.
                 // DQ (2/26/2009): Commented out because moreTest3.cpp fails for outlining to a separate file.
                    if (TransformationSupport::getSourceFile(funcdecl_stmt->get_scope()) != TransformationSupport::getSourceFile(firstNondefiningFunction))
                       {
                         printf ("firstNondefiningFunction = %p \n",firstNondefiningFunction);
                         printf ("firstNondefiningFunction = %s \n",firstNondefiningFunction->get_name().str());
                         printf ("funcdecl_stmt = %p funcdecl_stmt->get_scope()                                        = %p \n",funcdecl_stmt,funcdecl_stmt->get_scope());
                         printf ("funcdecl_stmt = %p funcdecl_stmt->get_declarationModifier().isFriend()               = %s \n",funcdecl_stmt,funcdecl_stmt->get_declarationModifier().isFriend() ? "true" : "false");
                         printf ("firstNondefiningFunction = %p firstNondefiningFunction->get_declarationModifier().isFriend() = %s \n",firstNondefiningFunction,firstNondefiningFunction->get_declarationModifier().isFriend() ? "true" : "false");
                         printf ("TransformationSupport::getSourceFile(funcdecl_stmt = %p)->getFileName()              = %s \n",funcdecl_stmt,TransformationSupport::getSourceFile(funcdecl_stmt)->getFileName().c_str());
                         printf ("TransformationSupport::getSourceFile(funcdecl_stmt->get_scope() = %p)->getFileName() = %s \n",funcdecl_stmt->get_scope(),TransformationSupport::getSourceFile(funcdecl_stmt->get_scope())->getFileName().c_str());
                         printf ("TransformationSupport::getSourceFile(firstNondefiningFunction = %p)->getFileName()   = %s \n",firstNondefiningFunction,TransformationSupport::getSourceFile(firstNondefiningFunction)->getFileName().c_str());
                         printf ("TransformationSupport::getSourceFile(firstNondefiningFunction->get_scope() = %p)->getFileName() = %s \n",firstNondefiningFunction->get_scope(),TransformationSupport::getSourceFile(firstNondefiningFunction->get_scope())->getFileName().c_str());
                       }
#endif

                 // TV (07/01/2013) : prevent separated generation of decl and defn over an header file and a source file
                 // RPM (12/10/2013): Commented out because this fails for astSnippetTests which deep-copy a function
                 // forward declaration and SageInterface::insertStatementBefore() the decl into a different file. The
                 // copied-and-inserted declaration is unparsed correctly if this assert is commented out.
                    //ROSE_ASSERT(TransformationSupport::getSourceFile(funcdecl_stmt->get_scope()) == TransformationSupport::getSourceFile(firstNondefiningFunction));
                  }
#if 0
               printf ("Unparser: firstNondefiningFunction = %p \n",firstNondefiningFunction);
#endif

            // DQ (3/4/2009): This test appear to only fail for tutorial/rose_inputCode_InstrumentationTranslator.C 
               if (firstNondefiningFunction->get_symbol_from_symbol_table() == NULL)
                  {
                    printf ("Warning failing test: firstNondefiningFunction->get_symbol_from_symbol_table() == NULL, appears to happen for tutorial/rose_inputCode_InstrumentationTranslator.C \n");
                  }
            // ROSE_ASSERT(firstNondefiningFunction->get_symbol_from_symbol_table() != NULL);
             }

#if 0
          printf ("Unparser: funcdecl_stmt = %p \n",funcdecl_stmt);
#endif

#if 0
       // DQ (3/4/2009): Comment out as a test!

       // DQ (2/24/2009): If this is the defining declaration and when there is no non-defining 
       // declaration the symbol references the defining declaration.
          if (funcdecl_stmt->get_firstNondefiningDeclaration() == NULL)
             {
               if (funcdecl_stmt->get_definingDeclaration() != funcdecl_stmt)
                  {
                    printf ("funcdecl_stmt = %p = %s = %s \n",funcdecl_stmt,funcdecl_stmt->class_name().c_str(),SageInterface::get_name(funcdecl_stmt).c_str());
                    printf ("funcdecl_stmt = %p != funcdecl_stmt->get_definingDeclaration() = %p \n",funcdecl_stmt,funcdecl_stmt->get_definingDeclaration());
                    printf ("funcdecl_stmt = %p != funcdecl_stmt->get_scope() = %p = %s \n",funcdecl_stmt,funcdecl_stmt->get_scope(),funcdecl_stmt->get_scope()->class_name().c_str());
                    printf ("Error: funcdecl_stmt = %p != funcdecl_stmt->get_definingDeclaration() = %p \n",funcdecl_stmt,funcdecl_stmt->get_definingDeclaration());
                  }
            // DQ (3/4/2009): This fails for test2006_78.C because the only non-defining declaration 
            // is a declaration in the scope of a function which does not provide enough information 
            // to associate the scope.
            // ROSE_ASSERT (funcdecl_stmt->get_definingDeclaration() == funcdecl_stmt);
            // ROSE_ASSERT (funcdecl_stmt->get_definingDeclaration() == funcdecl_stmt || isSgBasicBlock(funcdecl_stmt->get_scope()) != NULL);
               ROSE_ASSERT (funcdecl_stmt->get_definingDeclaration() == funcdecl_stmt || isSgBasicBlock(funcdecl_stmt->get_parent()) != NULL);
#if 0
               printf ("TransformationSupport::getSourceFile(funcdecl_stmt = %p)->getFileName() = %s \n",funcdecl_stmt,TransformationSupport::getSourceFile(funcdecl_stmt)->getFileName().c_str());
#endif
               ROSE_ASSERT (funcdecl_stmt->get_symbol_from_symbol_table() != NULL);
             }
            else
             {
               ROSE_ASSERT (funcdecl_stmt->get_firstNondefiningDeclaration()->get_symbol_from_symbol_table() != NULL);
             }
#endif

#if OUTPUT_FUNCTION_DECLARATION_DATA || 0
          printf ("calling unparse_helper \n");
          curprint ("/* calling unparse_helper */");
#endif
          unparse_helper(funcdecl_stmt, ninfo);

       // DQ (10/15/2006): Matching call to unset the stored declaration.
          ninfo.set_declstatement_ptr(NULL);

#if OUTPUT_FUNCTION_DECLARATION_DATA || 0
          printf ("DONE: calling unparse_helper \n");
          curprint ("/* DONE: calling unparse_helper */");
#endif

          ninfo.set_isTypeSecondPart();

#if OUTPUT_FUNCTION_DECLARATION_DATA || 0
          printf ("output the second part of the type \n");
          curprint ("/* output the second part of the type */");
#endif

          unp->u_type->unparseType(rtype, ninfo);

#if OUTPUT_FUNCTION_DECLARATION_DATA || 0
          printf ("DONE: output the second part of the type \n");
          curprint("/* DONE: output the second part of the type */");
#endif

       // DQ (4/28/2004): Added support for throw modifier
          if (funcdecl_stmt->get_declarationModifier().isThrow())
             {
#if 0
               printf ("In Unparse_ExprStmt::unparseFuncDeclStmt(): Output throw modifier \n");
#endif
            // printf ("Output throw modifier (incomplete implementation) \n");
            // curprint ( string(" throw( /* from unparseFuncDeclStmt() type list output not implemented */ )";
               const SgTypePtrList& exceptionSpecifierList = funcdecl_stmt->get_exceptionSpecification();

            // DQ (6/2/2011): Note that the throw can have a list, of types, and so they can be qualified.
               info.set_reference_node_for_qualification(funcdecl_stmt);

               unparseExceptionSpecification(exceptionSpecifierList,info);

            // Clear the reference after the throw exception list is output.
               info.set_reference_node_for_qualification(NULL);
             }

       // DQ (1/25/2009): Function can be defined using asm function names. The name is held as a string.
          if (funcdecl_stmt->get_asm_name().empty() == false)
             {
            // DQ (2/6/2014): Fix to support GNU gcc.
            // an asm ("<function name>") is in use
            // curprint ( string(" asm (\""));
               curprint(" __asm__ (\"");
               curprint( funcdecl_stmt->get_asm_name() );
               curprint( string("\")"));
             }

          if (funcdecl_stmt->isForward() && !ninfo.SkipSemiColon())
             {
            // DQ (2/26/2013): Added support for missing attributes.
               unp->u_sage->printAttributes(funcdecl_stmt,info);

            // DQ (1/6/2014): Added output of type attributes for function declaration.
            // This function must be a seperate call so that for variable declarations 
            // it can be put after the type and before the variable name.
               unp->u_sage->printAttributesForType(funcdecl_stmt,info);

               curprint(";");
#if 0
               printf ("In Unparse_ExprStmt::unparseFuncDeclStmt(): funcdecl_stmt->isExternBrace() = %s \n",funcdecl_stmt->isExternBrace() ? "true" : "false");
#endif
               if (funcdecl_stmt->isExternBrace())
                  {
#if 0
                    printf ("In Unparse_ExprStmt::unparseFuncDeclStmt(): output extern brace \n");
#endif
                    curprint(" }");
                  }
             }
            else
             {
#if 0
            // DQ (1/19/2014): Added support for missing attributes.
            // We certainly need to represent a number of different kinds of gnu attributes 
            // consistantly on both the non-defining and defining function declarations.
            // However, this is a bug if the attribute appears after the function declaration
            // (at least for the case of the defining declaration).
               unp->u_sage->printAttributes(funcdecl_stmt,info);
#endif
             }
        }

#if 0
  // DQ (4/28/2004): Added support for throw modifier
     if (info.SkipClassDefinition() && funcdecl_stmt->get_declarationModifier().isThrow())
        {
          curprint ( string(" throw()"));
        }
#endif

  // DQ (1/23/03) Added option to support rewrite mechanism (generation of declarations)
     if (info.AddSemiColonAfterDeclaration())
        {
          curprint (";");
        }

#if 0
  // curprint("/* End of Unparse_ExprStmt::unparseFuncDeclStmt */");
     curprint("\n/* Leaving Unparse_ExprStmt::unparseFuncDeclStmt (" + StringUtility::numberToString(stmt) + "): sage_class_name() = " + stmt->sage_class_name() + " */ \n");
     printf ("Leaving Unparse_ExprStmt::unparseFuncDeclStmt() \n");
#endif
   }


void
Unparse_ExprStmt::unparseTemplateFunctionDefnStmt(SgStatement *stmt_, SgUnparse_Info& info)
   {
     SgTemplateFunctionDefinition *stmt = isSgTemplateFunctionDefinition(stmt_);
     assert(stmt!=NULL);
     SgStatement *declstmt = isSgTemplateFunctionDeclaration(stmt->get_declaration());
     assert(declstmt!=NULL);
      
     //unparseTemplateFunctionDeclStmt(declstmt, info); // we should not go back to parent declaration and unparse it. bad logic and cause recursion.

     SgSourceFile* sourcefile = info.get_current_source_file();
     if (sourcefile != NULL && sourcefile->get_unparse_template_ast() == true)
     {
       //Liao, 12/15/2016
       // We should only unparse the definition, not going back to parent node to unparse the entire declaration including the header.
       SgFunctionDefinition* funcdefn_stmt = stmt;
       ROSE_ASSERT(funcdefn_stmt != NULL);

#if OUTPUT_HIDDEN_LIST_DATA
       outputHiddenListData (funcdefn_stmt);
#endif

       // Unparse any comments of directives attached to the SgFunctionParameterList
       ROSE_ASSERT (funcdefn_stmt->get_declaration() != NULL);
#if 0
       printf ("Unparse comments and CCP directives at funcdefn_stmt->get_declaration()->get_parameterList() \n");
#endif
       if (funcdefn_stmt->get_declaration()->get_parameterList() != NULL)
       {
#if 0
         printf ("Output the comments and CCP directives for the SgFunctionDefinition funcdefn_stmt = %p \n",funcdefn_stmt);

         printf ("funcdefn_stmt->get_declaration()                                    = %p \n",funcdefn_stmt->get_declaration());
         printf ("funcdefn_stmt->get_declaration()->get_firstNondefiningDeclaration() = %p \n",funcdefn_stmt->get_declaration()->get_firstNondefiningDeclaration());
         printf ("funcdefn_stmt->get_declaration()->get_definingDeclaration()         = %p \n",funcdefn_stmt->get_declaration()->get_definingDeclaration());
         printf ("funcdefn_stmt->get_declaration()->get_parameterList()               = %p \n",funcdefn_stmt->get_declaration()->get_parameterList());
#endif
#if 0
         curprint("/* Inside of Unparse_ExprStmt::unparseTemplateFunctionDefnStmt: calling unparseAttachedPreprocessingInfo() */ ");
#endif
         unparseAttachedPreprocessingInfo(funcdefn_stmt->get_declaration()->get_parameterList(), info, PreprocessingInfo::before);
#if 0
         printf ("DONE: Output the comments and CCP directives for the SgFunctionDefinition funcdefn_stmt = %p \n",funcdefn_stmt);
#endif
       }

       info.set_SkipFunctionDefinition();
       //     SgStatement *declstmt = funcdefn_stmt->get_declaration();

       // DQ (1/19/2014): Adding gnu attribute prefix support.
       ROSE_ASSERT(funcdefn_stmt->get_declaration() != NULL);

#if 0
       // DQ (6/23/2015): Added output of type attributes for defining function declaration (see test2015_164.c).
       // Within GNU the attribute can appear before or after the return type, here it is before the return type.
       SgFunctionDeclaration* funcdecl_stmt = isSgFunctionDeclaration(funcdefn_stmt->get_declaration());
       ROSE_ASSERT(funcdecl_stmt != NULL);
       unp->u_sage->printAttributes(funcdecl_stmt,info);
#endif

       unp->u_sage->printPrefixAttributes(funcdefn_stmt->get_declaration(),info);

       // DQ (3/24/2004): Need to permit SgMemberFunctionDecl and SgTemplateInstantiationMemberFunctionDecl
       // if (declstmt->variant() == MFUNC_DECL_STMT)

       // DQ (5/8/2004): Any generated specialization needed to use the 
       // C++ syntax for explicit specification of specializations.
       // if (isSgTemplateInstantiationMemberFunctionDecl(declstmt) != NULL)
       //      curprint ( string("template<> ";

#if 0
       printf ("Inside of Unparse_ExprStmt::unparseTemplateFunctionDefnStmt: calling unparseMFuncDeclStmt or unparseFuncDeclStmt \n");
       curprint ("/* Inside of Unparse_ExprStmt::unparseTemplateFunctionDefnStmt: calling unparseMFuncDeclStmt or unparseFuncDeclStmt */");
#endif

       // DQ (10/11/2006): As part of new implementation of qualified names we now default to the generation of all qualified names unless they are skipped.
       // info.set_SkipQualifiedNames();

       // DQ (10/15/2006): Mark that we are unparsing a function declaration (or member function declaration)
       // this will help us know when to trim the "::" prefix from the name qualiciation.  The "::" global scope
       // qualifier is not used in function declarations, but is used for function calls.
       info.set_declstatement_ptr(NULL);
       info.set_declstatement_ptr(funcdefn_stmt->get_declaration());

#if 0 // Liao, 12/15/2016  avoid duplicated unparsing of function header
       // DQ (12/5/2014): Test for if we have unparsed partially using the token stream.
       // If so then we don't want to unparse this syntax, if not then we require this syntax.
       // if (info.unparsedPartiallyUsingTokenStream() == false)
       bool saved_unparsedPartiallyUsingTokenStream = info.unparsedPartiallyUsingTokenStream();
       if (saved_unparsedPartiallyUsingTokenStream == false)
       {
         if (isSgMemberFunctionDeclaration(declstmt)) 
         {
           unparseMFuncDeclStmt( declstmt, info);
         }
         else 
         {
           unparseFuncDeclStmt( declstmt, info);
         }
       }
       else
       {
         // DQ (12/6/2014): Unparse the equivalent tokens instead.

#if 0
         curprint ("/* Inside of Unparse_ExprStmt::unparseTemplateFunctionDefnStmt: unparse partially from tokens START */");
#endif
         // unparseStatementFromTokenStream (SgStatement* stmt, token_sequence_position_enum_type e_leading_whitespace_start, token_sequence_position_enum_type e_token_subsequence_start)
         // unparseStatementFromTokenStream (declstmt, e_leading_whitespace_start, e_token_subsequence_start);
         // unparseStatementFromTokenStream (stmt, e_leading_whitespace_start, e_token_subsequence_start);
         // unparseStatementFromTokenStream (declstmt, stmt, e_token_subsequence_start, e_leading_whitespace_end);
         unparseStatementFromTokenStream (declstmt, stmt, e_token_subsequence_start, e_token_subsequence_start, info);
#if 0
         curprint ("/* Inside of Unparse_ExprStmt::unparseTemplateFunctionDefnStmt: unparse partially from tokens END */");
#endif
       }

#endif
       // curprint ("/* Inside of Unparse_ExprStmt::unparseTemplateFunctionDefnStmt: DONE calling unparseMFuncDeclStmt or unparseFuncDeclStmt */ ");

       // DQ (10/15/2006): Also un-mark that we are unparsing a function declaration (or member function declaration)
       info.set_declstatement_ptr(NULL);

       // DQ (10/11/2006): As part of new implementation of qualified names we now default to the generation of all qualified names unless they are skipped.
       // info.unset_SkipQualifiedNames();

#if 0
       printf ("Inside of Unparse_ExprStmt::unparseTemplateFunctionDefnStmt: comments before the output of the function body \n");
       curprint ("/* Inside of Unparse_ExprStmt::unparseTemplateFunctionDefnStmt: comments before the output of the function body */");
#endif

       info.unset_SkipFunctionDefinition();
       SgUnparse_Info ninfo(info);

       // DQ (10/20/2012): Ouput the comments and CPP directives on the function definition.
       // Note must be outside of SkipFunctionDefinition to be output.
       unparseAttachedPreprocessingInfo(funcdefn_stmt, info, PreprocessingInfo::before);

#if 0
       printf ("Inside of Unparse_ExprStmt::unparseTemplateFunctionDefnStmt: output the function body \n");
       curprint ("/* Inside of Unparse_ExprStmt::unparseTemplateFunctionDefnStmt: output the function body */");
#endif

       // now the body of the function
       if (funcdefn_stmt->get_body())
       {
         unparseStatement(funcdefn_stmt->get_body(), ninfo);
       }
       else
       {
         curprint ("{}");

         // DQ (9/22/2004): I think this is an error!
         printf ("Error: Should be an error to not have a function body in the AST \n");
         ROSE_ASSERT(false);
       }

#if 0
       curprint("/* Inside of Unparse_ExprStmt::unparseTemplateFunctionDefnStmt: calling unparseAttachedPreprocessingInfo() */ ");
#endif
       // DQ (10/20/2012): Not clear if this is in the correct location (shouldn't it be BEFORE the function body?).
       // Unparse any comments of directives attached to the SgFunctionParameterList
       unparseAttachedPreprocessingInfo(funcdefn_stmt->get_declaration()->get_parameterList(), info, PreprocessingInfo::after);

       // DQ (10/20/2012): Ouput the comments and CPP directives on the function definition.
       unparseAttachedPreprocessingInfo(funcdefn_stmt, info, PreprocessingInfo::after);

#if 0
       curprint("/* Leaving Unparse_ExprStmt::unparseTemplateFunctionDefnStmt: calling unparseAttachedPreprocessingInfo() */ ");
#endif
     }

   }


// NOTE: Bug in Sage: No file information provided for FuncDeclStmt. 
void
Unparse_ExprStmt::unparseFuncDefnStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
#if 0
     printf ("Inside of unparseFuncDefnStmt() \n");
     curprint("/* Inside of Unparse_ExprStmt::unparseFuncDefnStmt */ ");
#endif

     SgFunctionDefinition* funcdefn_stmt = isSgFunctionDefinition(stmt);
     ROSE_ASSERT(funcdefn_stmt != NULL);

#if OUTPUT_HIDDEN_LIST_DATA
     outputHiddenListData (funcdefn_stmt);
#endif

  // Unparse any comments of directives attached to the SgFunctionParameterList
     ROSE_ASSERT (funcdefn_stmt->get_declaration() != NULL);
#if 0
     printf ("Unparse comments and CCP directives at funcdefn_stmt->get_declaration()->get_parameterList() \n");
#endif
     if (funcdefn_stmt->get_declaration()->get_parameterList() != NULL)
        {
#if 0
          printf ("Output the comments and CCP directives for the SgFunctionDefinition funcdefn_stmt = %p \n",funcdefn_stmt);

          printf ("funcdefn_stmt->get_declaration()                                    = %p \n",funcdefn_stmt->get_declaration());
          printf ("funcdefn_stmt->get_declaration()->get_firstNondefiningDeclaration() = %p \n",funcdefn_stmt->get_declaration()->get_firstNondefiningDeclaration());
          printf ("funcdefn_stmt->get_declaration()->get_definingDeclaration()         = %p \n",funcdefn_stmt->get_declaration()->get_definingDeclaration());
          printf ("funcdefn_stmt->get_declaration()->get_parameterList()               = %p \n",funcdefn_stmt->get_declaration()->get_parameterList());
#endif
#if 0
          curprint("/* Inside of Unparse_ExprStmt::unparseFuncDefnStmt: calling unparseAttachedPreprocessingInfo() */ ");
#endif
          unparseAttachedPreprocessingInfo(funcdefn_stmt->get_declaration()->get_parameterList(), info, PreprocessingInfo::before);
#if 0
          printf ("DONE: Output the comments and CCP directives for the SgFunctionDefinition funcdefn_stmt = %p \n",funcdefn_stmt);
#endif
        }

     info.set_SkipFunctionDefinition();
     SgStatement *declstmt = funcdefn_stmt->get_declaration();

  // DQ (1/19/2014): Adding gnu attribute prefix support.
     ROSE_ASSERT(funcdefn_stmt->get_declaration() != NULL);

#if 0
  // DQ (6/23/2015): Added output of type attributes for defining function declaration (see test2015_164.c).
  // Within GNU the attribute can appear before or after the return type, here it is before the return type.
     SgFunctionDeclaration* funcdecl_stmt = isSgFunctionDeclaration(funcdefn_stmt->get_declaration());
     ROSE_ASSERT(funcdecl_stmt != NULL);
     unp->u_sage->printAttributes(funcdecl_stmt,info);
#endif

     unp->u_sage->printPrefixAttributes(funcdefn_stmt->get_declaration(),info);

  // DQ (3/24/2004): Need to permit SgMemberFunctionDecl and SgTemplateInstantiationMemberFunctionDecl
  // if (declstmt->variant() == MFUNC_DECL_STMT)

  // DQ (5/8/2004): Any generated specialization needed to use the 
  // C++ syntax for explicit specification of specializations.
  // if (isSgTemplateInstantiationMemberFunctionDecl(declstmt) != NULL)
  //      curprint ( string("template<> ";

#if 0
     printf ("Inside of Unparse_ExprStmt::unparseFuncDefnStmt: calling unparseMFuncDeclStmt or unparseFuncDeclStmt \n");
     curprint ("/* Inside of Unparse_ExprStmt::unparseFuncDefnStmt: calling unparseMFuncDeclStmt or unparseFuncDeclStmt */");
#endif

  // DQ (10/11/2006): As part of new implementation of qualified names we now default to the generation of all qualified names unless they are skipped.
  // info.set_SkipQualifiedNames();

  // DQ (10/15/2006): Mark that we are unparsing a function declaration (or member function declaration)
  // this will help us know when to trim the "::" prefix from the name qualiciation.  The "::" global scope
  // qualifier is not used in function declarations, but is used for function calls.
     info.set_declstatement_ptr(NULL);
     info.set_declstatement_ptr(funcdefn_stmt->get_declaration());

  // DQ (12/5/2014): Test for if we have unparsed partially using the token stream.
  // If so then we don't want to unparse this syntax, if not then we require this syntax.
  // if (info.unparsedPartiallyUsingTokenStream() == false)
     bool saved_unparsedPartiallyUsingTokenStream = info.unparsedPartiallyUsingTokenStream();
     if (saved_unparsedPartiallyUsingTokenStream == false)
        {
          if (isSgMemberFunctionDeclaration(declstmt)) 
             {
               unparseMFuncDeclStmt( declstmt, info);
             }
            else 
             {
               unparseFuncDeclStmt( declstmt, info);
             }
        }
       else
        {
       // DQ (12/6/2014): Unparse the equivalent tokens instead.

#if 0
           curprint ("/* Inside of Unparse_ExprStmt::unparseFuncDefnStmt: unparse partially from tokens START */");
#endif
        // unparseStatementFromTokenStream (SgStatement* stmt, token_sequence_position_enum_type e_leading_whitespace_start, token_sequence_position_enum_type e_token_subsequence_start)
        // unparseStatementFromTokenStream (declstmt, e_leading_whitespace_start, e_token_subsequence_start);
        // unparseStatementFromTokenStream (stmt, e_leading_whitespace_start, e_token_subsequence_start);
        // unparseStatementFromTokenStream (declstmt, stmt, e_token_subsequence_start, e_leading_whitespace_end);
           unparseStatementFromTokenStream (declstmt, stmt, e_token_subsequence_start, e_token_subsequence_start, info);
#if 0
           curprint ("/* Inside of Unparse_ExprStmt::unparseFuncDefnStmt: unparse partially from tokens END */");
#endif
        }

  // curprint ("/* Inside of Unparse_ExprStmt::unparseFuncDefnStmt: DONE calling unparseMFuncDeclStmt or unparseFuncDeclStmt */ ");

  // DQ (10/15/2006): Also un-mark that we are unparsing a function declaration (or member function declaration)
     info.set_declstatement_ptr(NULL);

  // DQ (10/11/2006): As part of new implementation of qualified names we now default to the generation of all qualified names unless they are skipped.
  // info.unset_SkipQualifiedNames();

#if 0
     printf ("Inside of Unparse_ExprStmt::unparseFuncDefnStmt: comments before the output of the function body \n");
     curprint ("/* Inside of Unparse_ExprStmt::unparseFuncDefnStmt: comments before the output of the function body */");
#endif

     info.unset_SkipFunctionDefinition();
     SgUnparse_Info ninfo(info);
  
  // DQ (10/20/2012): Ouput the comments and CPP directives on the function definition.
  // Note must be outside of SkipFunctionDefinition to be output.
     unparseAttachedPreprocessingInfo(funcdefn_stmt, info, PreprocessingInfo::before);

#if 0
     printf ("Inside of Unparse_ExprStmt::unparseFuncDefnStmt: output the function body \n");
     curprint ("/* Inside of Unparse_ExprStmt::unparseFuncDefnStmt: output the function body */");
#endif

  // now the body of the function
     if (funcdefn_stmt->get_body())
        {
          unparseStatement(funcdefn_stmt->get_body(), ninfo);
        }
       else
        {
          curprint ("{}");

       // DQ (9/22/2004): I think this is an error!
          printf ("Error: Should be an error to not have a function body in the AST \n");
          ROSE_ASSERT(false);
        }

#if 0
     curprint("/* Inside of Unparse_ExprStmt::unparseFuncDefnStmt: calling unparseAttachedPreprocessingInfo() */ ");
#endif
  // DQ (10/20/2012): Not clear if this is in the correct location (shouldn't it be BEFORE the function body?).
  // Unparse any comments of directives attached to the SgFunctionParameterList
     unparseAttachedPreprocessingInfo(funcdefn_stmt->get_declaration()->get_parameterList(), info, PreprocessingInfo::after);

  // DQ (10/20/2012): Ouput the comments and CPP directives on the function definition.
     unparseAttachedPreprocessingInfo(funcdefn_stmt, info, PreprocessingInfo::after);

#if 0
     printf("Leaving Unparse_ExprStmt::unparseFuncDefnStmt: calling unparseAttachedPreprocessingInfo() \n");
     curprint("/* Leaving Unparse_ExprStmt::unparseFuncDefnStmt: calling unparseAttachedPreprocessingInfo() */ ");
#endif
   }


string
Unparse_ExprStmt::trimGlobalScopeQualifier ( string qualifiedName )
   {
  //! DQ (10/12/2006): Support for qualified names (function names can't have global scope specifier in GNU, or so it seems).

  // DQ (10/11/2006): Now that we use fully qualified names in most places we need this 
  // editing to remove the leading global qualifier (once again!).

  // DQ (8/25/2005): This is the case where we previously named the global scope as "::"
  // within name qualification.  This was done to handle test2005_144.C but it broke
  // test2004_80.C So we have moved to an explicit marking of IR nodes using global scope
  // qualification (since it clearly seems to be required).
  // For member functions we need to remove the leading "::" since GNU g++ can't always handle it for member functions
     string s = qualifiedName;
     size_t subStringLocationOfScopeQualifier = s.find("::");
  // printf ("Location of member function substring = %d \n",subStringLocationOfScopeQualifier);
     if (subStringLocationOfScopeQualifier == 0)
        {
       // printf ("Found global scope qualifier at start of function or member function name qualification \n");
          s.replace(s.find("::"),2,"");

       // reset the string in scopename!
          qualifiedName = s.c_str();
        }

     return qualifiedName;
   }


#if 0
void
Unparse_ExprStmt::unparseGeneratedTemplateArgumentsList (SgName unqualifiedName, SgName qualifiedName, SgLocatedNode* locatedNode, SgUnparse_Info& info)
   {
  // DQ (5/25/2013): For statements that unparse with a template argument list, we need to refactor 
  // the support for unparsing the template argument list when it is computed with associated 
  // name qualification.  This supposrt is redundantly represented in the unparsing of the
  // SgClassType.

     bool isTemplateWithTemplateArgumentList = true;

#error "DEAD CODE!"

     printf ("unparseGeneratedTemplateArgumentsList(): This function is not yet called \n");
     ROSE_ASSERT(false);

  // DQ (6/25/2011): Fixing name qualifiction to work with unparseToString().  In this case we don't 
  // have an associated node to reference as a way to lookup the strored name qualification.  In this 
  // case we return a fully qualified name.
     if (info.get_reference_node_for_qualification() == NULL)
        {
#if 0
          printf ("WARNING: In unparseClassType(): info.get_reference_node_for_qualification() == NULL (assuming this is for unparseToString() \n");
#endif
       // SgName nameQualifierAndType = class_type->get_qualified_name();
          SgName nameQualifierAndType = qualifiedName;
          curprint(nameQualifierAndType.str());
        }
       else
        {
       // DQ (6/2/2011): Newest support for name qualification...
#if 0
          printf ("info.get_reference_node_for_qualification() = %p = %s \n",info.get_reference_node_for_qualification(),info.get_reference_node_for_qualification()->class_name().c_str());
#endif
          SgName nameQualifier = unp->u_name->lookup_generated_qualified_name(info.get_reference_node_for_qualification());
#if 0
          printf ("nameQualifier (from initializedName->get_qualified_name_prefix_for_type() function) = %s \n",nameQualifier.str());
#endif

#error "DEAD CODE!"

          SgName nm = unqualifiedName;
#if 0
       // printf ("nameQualifier (from unp->u_name->generateNameQualifier function) = %s \n",nameQualifier.str());
       // curprint ("\n/* nameQualifier (from unp->u_name->generateNameQualifier function) = " + nameQualifier + " */ \n ";
          curprint (nameQualifier.str());
          SgName nm = class_type->get_name();

#error "DEAD CODE!"
          if (nm.getString() != "")
             {
            // printf ("Output qualifier of current types to the name = %s \n",nm.str());
               curprint ( nm.getString() + " ");
             }
#endif
       // SgName nameQualifier = unp->u_name->generateNameQualifierForType( type , info );
#if 0
          printf ("In unparseClassType: nameQualifier (from initializedName->get_qualified_name_prefix_for_type() function) = %s \n",nameQualifier.str());
       // curprint ( string("\n/* In unparseClassType: nameQualifier (from unp->u_name->generateNameQualifier function) = ") + nameQualifier + " */ \n ");
#endif
          curprint(nameQualifier.str());

       // SgTemplateInstantiationDecl* templateInstantiationDeclaration = isSgTemplateInstantiationDecl(decl);
       // if (isSgTemplateInstantiationDecl(decl) != NULL)
          if (isTemplateWithTemplateArgumentList == true)
             {
            // Handle case of class template instantiation (code located in unparse_stmt.C)
#if 0
               curprint ("/* Calling unparseTemplateName */ \n ");
#endif
#if 0
               printf ("In unparseClassType: calling unparseTemplateName() for templateInstantiationDeclaration = %p \n",templateInstantiationDeclaration);
#endif
               SgUnparse_Info ninfo(info);

            // DQ (5/7/2013): This fixes the test2013_153.C test code.
               if (ninfo.isTypeFirstPart() == true)
                  {
#if 0
                    printf ("In unparseClassType(): resetting isTypeFirstPart() == false \n");
#endif
                    ninfo.unset_isTypeFirstPart();
                  }

               if (ninfo.isTypeSecondPart() == true)
                  {
#if 0
                    printf ("In unparseClassType(): resetting isTypeSecondPart() == false \n");
#endif
                    ninfo.unset_isTypeSecondPart();
                  }

#error "DEAD CODE!"

            // DQ (5/7/2013): I think these should be false so that the full type will be output.
               ROSE_ASSERT(ninfo.isTypeFirstPart()  == false);
               ROSE_ASSERT(ninfo.isTypeSecondPart() == false);

#error "DEAD CODE!"

            // unp->u_exprStmt->unparseTemplateName(templateInstantiationDeclaration,info);
            // unp->u_exprStmt->unparseTemplateName(templateInstantiationDeclaration,ninfo);
            // unp->u_exprStmt->unparseTemplateName(locatedNode,ninfo);
#if 0
               curprint ("/* DONE: Calling unparseTemplateName (commented out) */ \n ");
#endif
             }
            else
             {
               curprint ( string(nm.str()) + " ");
#if 0
               printf ("class type name: nm = %s \n",nm.str());
#endif
             }
        }

#error "DEAD CODE!"

   }
#endif




void
Unparse_ExprStmt::unparseReturnType (SgFunctionDeclaration* funcdecl_stmt, SgType* & rtype, SgUnparse_Info& ninfo)
   {
  // DQ (9/7/2014): Refactored this code so we could call it from the template member and non-member function declaration unparse function.
  // Note that we pass a reference to the return type so that we can call unparseType a second time to unparse the second part 
  // (not yet refactored, since it is much simpler).

#if 0
     printf ("In unparseReturnType(): funcdecl_stmt = %p funcdecl_stmt->get_name() = %s \n",funcdecl_stmt,funcdecl_stmt->get_name().str());
     curprint ("/* In unparseReturnType(): */ \n ");
#endif

#if 0
     printf ("funcdecl_stmt->get_type_syntax_is_available() = %s \n",funcdecl_stmt->get_type_syntax_is_available() ? "true" : "false");
     printf ("funcdecl_stmt->get_type_syntax()              = %p \n",funcdecl_stmt->get_type_syntax());
#endif

     SgClassDefinition *parent_class = isSgClassDefinition(funcdecl_stmt->get_parent());

       // This is a test for if the member function is structurally in the class where it is defined.
       // printf ("parent_class = %p mfuncdecl_stmt->get_scope() = %p \n",parent_class,mfuncdecl_stmt->get_scope());

       // DQ (11/5/2007): This test is not good enough (does not handle case of nested classes and the definition
       // of member function outside of the nested class and inside of another class.
       // if (parent_class)
          if (parent_class == funcdecl_stmt->get_scope())
             {
            // JJW 10-23-2007 This member function is declared inside the
            // class, so its name should never be qualified

            // printf ("mfuncdecl_stmt->get_declarationModifier().isFriend() = %s \n",mfuncdecl_stmt->get_declarationModifier().isFriend() ? "true" : "false");
               if (funcdecl_stmt->get_declarationModifier().isFriend() == false)
                  {
                 // printf ("Setting SkipQualifiedNames (this is a member function located in its own class) \n");
                    ninfo.set_SkipQualifiedNames();
                  }
             }

          ninfo.set_SkipClassDefinition();
          ninfo.set_SkipEnumDefinition();

       // DQ (6/10/2007): set the declaration pointer so that the name qualification can see if this is 
       // the declaration (so that exceptions to qualification can be tracked).
          ninfo.set_declstatement_ptr(NULL);
          ninfo.set_declstatement_ptr(funcdecl_stmt);

       // if (!(mfuncdecl_stmt->isConstructor() || mfuncdecl_stmt->isDestructor() || mfuncdecl_stmt->isConversion()))
          if ( !( funcdecl_stmt->get_specialFunctionModifier().isConstructor() || 
                  funcdecl_stmt->get_specialFunctionModifier().isDestructor()  ||
                  funcdecl_stmt->get_specialFunctionModifier().isConversion() ) )
             {
#if 0
               printf ("In unparseReturnType(): NOT a constructor, destructor or conversion operator \n");
               curprint ("/* In unparseReturnType(): NOT a constructor, destructor or conversion operator */ \n ");
#endif
#if 0
               printf ("funcdecl_stmt->get_orig_return_type() = %p \n",funcdecl_stmt->get_orig_return_type());
#endif
               if (funcdecl_stmt->get_orig_return_type() != NULL)
                  {
#if 0
                    printf ("funcdecl_stmt->get_orig_return_type() = %p = %s \n",funcdecl_stmt->get_orig_return_type(),funcdecl_stmt->get_orig_return_type()->class_name().c_str());
#endif
                    rtype = funcdecl_stmt->get_orig_return_type();

#if 1
                 // DQ (2/24/2019): Use the type syntax when it is available.
                    if (funcdecl_stmt->get_type_syntax_is_available() == true)
                       {
#if 0
                         printf ("Using the type_syntax since it is available: funcdecl_stmt->get_type_syntax() = %p \n",funcdecl_stmt->get_type_syntax());
#endif
                      // DQ (2/25/2019): Get the return type from the function type that is held in memberFunctionDeclaration->get_type_syntax().
                      // rtype = funcdecl_stmt->get_type_syntax();
                         SgFunctionType* functionType = isSgFunctionType(funcdecl_stmt->get_type_syntax());
                         ROSE_ASSERT(functionType != NULL);
                      // return_syntax_type = memberFunctionDeclaration->get_type_syntax();
                         if (functionType->get_orig_return_type() != NULL)
                            {
                              rtype = functionType->get_orig_return_type();
                            }
                           else
                            {
                              rtype = functionType->get_return_type();
                            }
                         ROSE_ASSERT(rtype != NULL);
                       }
#endif
                  }
                 else
                  {
                    printf ("In unparseReturnType(): (should not happen) funcdecl_stmt->get_type()->get_return_type() = %p = %s \n",
                         funcdecl_stmt->get_type()->get_return_type(),funcdecl_stmt->get_type()->get_return_type()->class_name().c_str());
                    rtype = funcdecl_stmt->get_type()->get_return_type();
                  }

#if 0
               printf ("In unparseReturnType(): rtype = %p = %s \n",rtype,rtype->class_name().c_str());
#endif

#if 0
               SgTemplateFunctionDeclaration* templateFunctionDeclaration             = isSgTemplateFunctionDeclaration(funcdecl_stmt);
               SgTemplateMemberFunctionDeclaration* templateMemberFunctionDeclaration = isSgTemplateMemberFunctionDeclaration(funcdecl_stmt);
               if (templateFunctionDeclaration != NULL || templateMemberFunctionDeclaration != NULL)
                  {
                 // DQ (9/9/2014): Evaluate if we want to use the "typename" keyword before the output of the return type (required in some cases for g++ (version 4.4 through 4.8, at least)).
                 // Note: We might want to refine this criteria to if the associated class is a SgTemplateClassDeclaration.

                 // DQ (9/10/2014): Add the typename based on the base type ignoreing modifiers, etc.
                 // SgType* baseTypeOfPointerOrReference = rtype->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_RVALUE_REFERENCE_TYPE|SgType::STRIP_POINTER_TYPE);
                 // TV (03/27/2018) : go all the way down to the base type
                    SgType * btype = rtype->stripType();
                    ROSE_ASSERT(btype != NULL);

#if 0
                    printf ("  btype = %p = %s \n",btype,btype->class_name().c_str());
#endif

                 // TV (03/27/2018): condition to determine whether or not to prefix return types with "typename"

                    SgClassType*   classType   = isSgClassType(btype);
                    SgTypedefType* typedefType = isSgTypedefType(btype);
                    SgTemplateType* templateType = isSgTemplateType(btype);


                    SgDeclarationStatement * assoc_decl_stmt = btype->getAssociatedDeclaration();
#if 0
                    printf ("  assoc_decl_stmt = %p = %s \n", assoc_decl_stmt, assoc_decl_stmt ? assoc_decl_stmt->class_name().c_str() : "");
#endif

                    ROSE_ASSERT(classType == NULL || assoc_decl_stmt != NULL);

                 // TV (03/27/2018): "typename" is needed if the base-type is a non-real declaration
                    bool type_needs_typename = isSgNonrealDecl(assoc_decl_stmt);

                 // TV (03/27/2018): Only for function template or methods outside of their classes
                    bool parent_is_scope = funcdecl_stmt->get_parent() == funcdecl_stmt->get_scope();
                    bool method_outside_class_scope = templateMemberFunctionDeclaration && !parent_is_scope;
                    bool function_or_method_outside_class_scope = templateFunctionDeclaration || method_outside_class_scope;

                 // TV (03/27/2018): kept that condition from previous code (TODO relevant example)
                    bool isOperator = funcdecl_stmt->get_specialFunctionModifier().isOperator();

                 // TV (03/27/2018): whether or not to add "typename"
                    bool prepend_typename = type_needs_typename && function_or_method_outside_class_scope && !isOperator;
#if 0
                    printf ("  type_needs_typename = %s \n", type_needs_typename ? "true" : "false");
                    printf ("  parent_is_scope = %s \n", parent_is_scope ? "true" : "false");
                    printf ("  method_outside_class_scope = %s \n", method_outside_class_scope ? "true" : "false");
                    printf ("  function_or_method_outside_class_scope = %s \n", function_or_method_outside_class_scope ? "true" : "false");
                    printf ("  isOperator = %s \n",isOperator ? "true" : "false");
                    printf ("  prepend_typename = %s \n", prepend_typename ? "true" : "false");
#endif

                 // DQ (9/10/2014): Another case where typename is required for g++ (see test2014_208.C).
                    if (prepend_typename)
                       {
                         curprint("typename ");
                       }
                  }
#endif

               ninfo.set_isTypeFirstPart();
               ninfo.set_SkipClassSpecifier();

               SgUnparse_Info ninfo_for_type(ninfo);

            // DQ (6/10/2007): set the declaration pointer so that the name qualification can see if this is 
            // the declaration (so that exceptions to qualification can be tracked).
               ROSE_ASSERT(ninfo_for_type.get_declstatement_ptr() != NULL);

            // DQ (12/20/2006): This is used to specify global qualification separately from the more general name 
            // qualification mechanism.  Note that SgVariableDeclarations don't use the requiresGlobalNameQualificationOnType
            // on the SgInitializedNames in their list since the SgVariableDeclaration IR nodes is marked directly.
#if 0
               printf ("funcdecl_stmt->get_requiresNameQualificationOnReturnType() = %s \n",funcdecl_stmt->get_requiresNameQualificationOnReturnType() ? "true" : "false");
#endif
            // curprint ( string("\n/* funcdecl_stmt->get_requiresNameQualificationOnReturnType() = " + (mfuncdecl_stmt->get_requiresNameQualificationOnReturnType() ? "true" : "false") + " */ \n";
               if (funcdecl_stmt->get_requiresNameQualificationOnReturnType() == true)
            // if (funcdecl_stmt->get_requiresNameQualificationOnReturnType() == true || isSgNonrealType(rtype->stripType()))
                  {
                 // Output the name qualification for the type in the variable declaration.
                 // But we have to do so after any modifiers are output, so in unp->u_type->unparseType().
                 // printf ("In Unparse_ExprStmt::unparseMemberFunctionDeclaration(): This return type requires a global qualifier \n");

#if 0
                    printf("  Requires for global name qualification\n");
#endif
                 // Note that general qualification of types is separated from the use of globl qualification.
                    ninfo_for_type.set_requiresGlobalNameQualification();
                  }

            // DQ (5/30/2011): Added support for name qualification.
               ninfo_for_type.set_reference_node_for_qualification(funcdecl_stmt);
               ROSE_ASSERT(ninfo_for_type.get_reference_node_for_qualification() != NULL);

#if 0
               printf("  funcdecl_stmt->get_name_qualification_length_for_return_type() = %d\n", funcdecl_stmt->get_name_qualification_length_for_return_type());
               printf("  funcdecl_stmt->get_global_qualification_required_for_return_type() = %s\n", funcdecl_stmt->get_global_qualification_required_for_return_type() ? "true" : "false");
               printf("  funcdecl_stmt->get_type_elaboration_required_for_return_type() = %s\n", funcdecl_stmt->get_type_elaboration_required_for_return_type() ? "true" : "false");
#endif

               ninfo_for_type.set_name_qualification_length(funcdecl_stmt->get_name_qualification_length_for_return_type());
               ninfo_for_type.set_global_qualification_required(funcdecl_stmt->get_global_qualification_required_for_return_type());
               ninfo_for_type.set_type_elaboration_required(funcdecl_stmt->get_type_elaboration_required_for_return_type());

#if 0
               printf ("In unparseReturnType(): calling unparseType: rtype = %p = %s \n",rtype,rtype->class_name().c_str());
               curprint ("/* In unparseReturnType(): calling unparseType */ \n ");
#endif

            // unp->u_type->unparseType(rtype, ninfo);
               unp->u_type->unparseType(rtype, ninfo_for_type);

#if 0
               printf ("In unparseReturnType(): DONE: calling unparseType: rtype = %p = %s \n",rtype,rtype->class_name().c_str());
               curprint ("/* In unparseReturnType(): DONE: calling unparseType */ \n ");
#endif

               ninfo.unset_SkipClassSpecifier();
            // printf ("In unparser: DONE with NOT a constructor, destructor or conversion operator \n");
             }
            else
             {
            // DQ (9/17/2004): What can we assume about the return type of a constructor, destructor, or conversion operator?
               if (funcdecl_stmt->get_orig_return_type() == NULL)
                  {
                    printf ("funcdecl_stmt->get_orig_return_type() == NULL funcdecl_stmt = %p = %s = %s \n",
                         funcdecl_stmt,funcdecl_stmt->class_name().c_str(),funcdecl_stmt->get_name().str());
                  }

               ROSE_ASSERT(funcdecl_stmt->get_orig_return_type() != NULL);
               ROSE_ASSERT(funcdecl_stmt->get_type()->get_return_type() != NULL);
             }

#if 0
     printf ("Leaving unparseReturnType(): funcdecl_stmt = %p funcdecl_stmt->get_name() = %s \n",funcdecl_stmt,funcdecl_stmt->get_name().str());
     curprint ("/* Leaving unparseReturnType(): */ \n ");
#endif
   }
                



void
Unparse_ExprStmt::unparseMFuncDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgMemberFunctionDeclaration* mfuncdecl_stmt = isSgMemberFunctionDeclaration(stmt);
     ROSE_ASSERT(mfuncdecl_stmt != NULL);

#if 0
     printf ("Inside of Unparse_ExprStmt::unparseMFuncDeclStmt(stmt = %p = %s) \n",stmt,stmt->class_name().c_str());
     curprint ("\n/* Inside of Unparse_ExprStmt::unparseMFuncDeclStmt */ \n"); 
#endif

#if OUTPUT_DEBUGGING_FUNCTION_NAME || 0
     printf ("Inside of unparseMFuncDeclStmt() name = %s  transformed = %s prototype = %s \n",
         mfuncdecl_stmt->get_qualified_name().str(), isTransformed (mfuncdecl_stmt) ? "true" : "false", (mfuncdecl_stmt->get_definition() == NULL) ? "true" : "false");
#endif

#if 0
  // printf ("Inside of Unparse_ExprStmt::unparseMFuncDeclStmt(): mfuncdecl_stmt->get_from_template() = %s \n",
  //      mfuncdecl_stmt->get_from_template() ? "true" : "false");
  // if (mfuncdecl_stmt->get_from_template() == true)
  //      curprint ( string("/* Unparser comment: Templated Member Function */";
  // curprint ( string("\n/* Unparser comment: mfuncdecl_stmt->get_from_template() = " + 
  //        mfuncdecl_stmt->get_from_template() + " */";
     curprint ( string("\n/* Unparser comment: mfuncdecl_stmt->get_definition() = " ) + StringUtility::numberToString(mfuncdecl_stmt->get_definition()) + " */");
#if 0
     curprint ("\n/* Unparser comment: mfuncdecl_stmt->get_definition_ref() = " ) + StringUtility::numberToString(mfuncdecl_stmt->get_definition_ref()) + " */");
     curprint ("\n/* Unparser comment: mfuncdecl_stmt->get_forwardDefinition() = " ) + StringUtility::numberToString(mfuncdecl_stmt->get_forwardDefinition()) + " */");
#endif
     curprint ( string("\n/* Unparser comment: mfuncdecl_stmt->get_definingDeclaration() = " ) + StringUtility::numberToString(mfuncdecl_stmt->get_definingDeclaration()) + " */");
     curprint ( string("\n/* Unparser comment: mfuncdecl_stmt->get_firstNondefiningDeclaration() = " ) + StringUtility::numberToString(mfuncdecl_stmt->get_firstNondefiningDeclaration()) + " */");
     curprint ("\n/* */");
#endif

  // DQ (12/3/2007): This causes a bug in the output of access level (public, protected, private)
  // because the inforamtion change in ninfo is not propogated to info.
  // DQ (11/3/2007): Moved construction of ninfo to start of function!
     SgUnparse_Info ninfo(info);

     fixupScopeInUnparseInfo (ninfo,mfuncdecl_stmt);

  // Unparse any comments of directives attached to the SgCtorInitializerList
     if (mfuncdecl_stmt->get_CtorInitializerList() != NULL)
        {
       // unparseAttachedPreprocessingInfo(mfuncdecl_stmt->get_CtorInitializerList(), info, PreprocessingInfo::before);
          unparseAttachedPreprocessingInfo(mfuncdecl_stmt->get_CtorInitializerList(), ninfo, PreprocessingInfo::before);
        }

  // DQ (11/23/2004): Experiment with skipping the output of specialized template member functions!
  // this needs to be handled better in the future!
     if (isSgTemplateInstantiationMemberFunctionDecl(mfuncdecl_stmt) != NULL)
        {
#if PRINT_DEVELOPER_WARNINGS || 0
          printf ("DQ (11/23/2004): Experiment with skipping the output of specialized template member functions! \n");
#endif

          SgStatement* parentStatement = isSgStatement(mfuncdecl_stmt->get_parent());
          ROSE_ASSERT(parentStatement != NULL);
          if ( isSgTemplateInstantiationDirectiveStatement(parentStatement) != NULL )
             {
            // output the member function declaration if it is part of an explicit instatiation directive
            // Note this code is confusiong becase what we are doing is NOT returning!
            // printf ("This template member function is part of an explicit template directive! \n");
            // curprint ( string("\n/* This template member function is part of an explicit template directive! */";
             }
            else
             {
#if 0
               if ( isTransformed (mfuncdecl_stmt) == false )
                  {
                 // DQ (3/2/2005): I think we are ready to output the member functions of template classes (or even templated member functions).
                 // This should be made depedent upon the evaluation of bool Unparse_ExprStmt::isTransformed(SgStatement* stmt) so that it matches the 
                 // output of specialized class declarations!  And the output of template functions should then be made consistant.
                    curprint ( string("\n /* DQ (11/23/2004): Experiment with skipping the output of specialized template member functions! */ \n"));
                    return;
                  }
#else
            // curprint ( string("\n /* Output of template member functions turned on 3/2/2005 */ \n";
#endif
             }
        }

  // DQ (4/21/2005): This is a very old comment and I think its information 
  // is now incorrect.  Likely this comment could be removed!
  /* EXCEPTION HANDLING: Member Forward Declarations in *.C file */
  // DO NOT use the file information sage gives us because the line information
  // refers to the function definition (if it is defined in the same file). Instead, 
  // the line of the forward declaration is set one after the line number of the 
  // previous node or directive. By doing this, any comments around the forward 
  // declaration will come after the declaration, since I'm setting the line number 
  // to be right after the previous thing we unparsed.

  // the following is a HACK. I want to know if this statement is in a header or
  // C++ file. If it is in a header file, then I proceed as normal. If the
  // statement is in a C++ file and is a forward declaration, then I must 
  // follow this HACK. When the Sage bug of forward declaration is fixed, remove
  // this code.
  /* EXCEPTION HANDLING: Member Forward Declarations in *.C file */

#if 0
     printf ("mfuncdecl_stmt->isForward()      = %s \n",mfuncdecl_stmt->isForward() ? "true" : "false");
     printf ("mfuncdecl_stmt->get_definition() = %s \n",mfuncdecl_stmt->get_definition() ? "true" : "false");
     printf ("info.SkipFunctionDefinition()    = %s \n",info.SkipFunctionDefinition() ? "true" : "false");
#endif

  // DQ (4/13/2019): If this is a defaulted constructor, then we don't want to unparse the body, so we want to treat it the same as a forward declaration.
  // bool isDefaultedMemberFunction = (mfuncdecl_stmt->get_functionModifier().isMarkedDefault() == true);
     bool isDefaultedOrDeletedMemberFunction = (mfuncdecl_stmt->get_functionModifier().isMarkedDefault() == true) || (mfuncdecl_stmt->get_functionModifier().isMarkedDelete() == true);

#if 0
     printf ("In unparseMFuncDeclStmt(): isDefaultedOrDeletedMemberFunction = %s \n",isDefaultedOrDeletedMemberFunction ? "true" : "false");
#endif

  // DQ (4/13/2019): If this is a defaulted constructor, then we don't want to unparse the body, so we want to treat it the same as a forward declaration.
  // if ( !mfuncdecl_stmt->isForward() && mfuncdecl_stmt->get_definition() && !info.SkipFunctionDefinition() )
  // if ( !mfuncdecl_stmt->isForward() && mfuncdecl_stmt->get_definition() && !ninfo.SkipFunctionDefinition() )
     if ( !mfuncdecl_stmt->isForward() && mfuncdecl_stmt->get_definition() && !ninfo.SkipFunctionDefinition() && isDefaultedOrDeletedMemberFunction == false)
        {
#if 0
          printf ("Unparsing special case of non-forward, valid definition and !skip function definition \n");
       // curprint ( string("\n/* Unparsing special case of non-forward, valid definition and !skip function definition */ \n"; 
#endif

       // DQ (12/3/2007): We want the changes to the access state to be saved in
       // the info object. See test2007_171.C for example of why this is critical.
       // unparseStatement(mfuncdecl_stmt->get_definition(), info);
       // unparseStatement(mfuncdecl_stmt->get_definition(), ninfo);
          unparseStatement(mfuncdecl_stmt->get_definition(), info);

          if (mfuncdecl_stmt->isExternBrace())
             {
               unp->cur.format(mfuncdecl_stmt, info, FORMAT_BEFORE_BASIC_BLOCK2);
               curprint ( string(" }"));
               unp->cur.format(mfuncdecl_stmt, info, FORMAT_AFTER_BASIC_BLOCK2);
             }
        }
       else 
        {
#if 0
          printf ("Normal case for unparsing member function declarations! \n");
          curprint("\n/* Normal case for unparsing member function declarations */ \n"); 
#endif
#if 0
       // DQ (5/8/2004): Any generated specialization needed to use the 
       // C++ syntax for explicit specification of specializations.
          SgTemplateInstantiationMemberFunctionDecl* templateMemberFunction = isSgTemplateInstantiationMemberFunctionDecl(mfuncdecl_stmt);
          if (templateMemberFunction != NULL && templateMemberFunction->get_templateArguments()->size() > 0)
             {
               curprint ( string("/* explicit template specialization */ "));
               curprint ( string("template<> "));
             }
#endif
          ROSE_ASSERT (mfuncdecl_stmt->get_parent() != NULL);

       // DQ (11/15/2004): It might be safer to always force the qualified function name to be output!
       // Better yet we should use the scope that is now explicitly stored in the Sage III AST!
#if PRINT_DEVELOPER_WARNINGS
          printf ("IMPLEMENTATION NOTE: This does not test if the parent class is the correct class in deeply nested class structures \n");
#endif
          SgClassDefinition *parent_class = isSgClassDefinition(mfuncdecl_stmt->get_parent());
       // printf ("In unparseMFuncDeclStmt(): parent_class of member function declaration = %p = %s \n",mfuncdecl_stmt->get_parent(),mfuncdecl_stmt->get_parent()->sage_class_name());

       // printf ("Commented out call to get_suppress_atomic(mfuncdecl_stmt) \n");
#if 0
          if (get_suppress_atomic(mfuncdecl_stmt))
               info.set_SkipAtomic();
#endif

       // DQ and PC (6/1/2006): Added Peter's suggested fixes to support unparsing fully qualified names (supporting auto-documentation).
       // if (parent_class && parent_class->get_declaration()->get_class_type() == SgClassDeclaration::e_class) 
          if (parent_class && parent_class->get_declaration()->get_class_type() == SgClassDeclaration::e_class && !info.skipCheckAccess())
             {
               info.set_CheckAccess();
             }

#if 0
          curprint("/* Calling printSpecifier1() */\n ");
#endif
       // printDebugInfo("entering unp->u_sage->printSpecifier1", true);
          unp->u_sage->printSpecifier1(mfuncdecl_stmt, info);
#if 0
          curprint("/* Calling printSpecifier2() */\n ");
#endif
       // printDebugInfo("entering unp->u_sage->printSpecifier2", true);
          unp->u_sage->printSpecifier2(mfuncdecl_stmt, info);
          info.unset_CheckAccess();

       // DQ (11/3/2007): Moved construction of ninfo to start of function!
       // SgUnparse_Info ninfo(info);
          SgType *rtype = NULL;

#if 0
          curprint("/* Calling unparseReturnType() */\n ");
#endif

#if 1
       // DQ (9/7/2014): Refactored this code so we could call it from the template member and non-member function declaration unparse function.
          unparseReturnType (mfuncdecl_stmt,rtype,ninfo);

#if 0
          printf ("After unparseReturnType(): rtype = %p \n",rtype);
          curprint("/* DONE: unparseReturnType() */\n ");
#endif

#else
       // DQ (10/10/2006): Do output any qualified names (particularly for non-defining declarations).
       // ninfo.set_forceQualifiedNames();

#error "DEAD CODE!"

#if 0
          curprint("/* force output of qualified names */\n ");
#endif

#error "DEAD CODE!"

       // DQ (10/17/2004): Added code to form skipping enum definitions.
       // DQ (10/7/2004): Skip output of class definition for return type! C++ standard does not permit 
       // a defining declaration within a return type, function parameter, or sizeof expression. 

#if 1
       // This is a test for if the member function is structurally in the class where it is defined.
       // printf ("parent_class = %p mfuncdecl_stmt->get_scope() = %p \n",parent_class,mfuncdecl_stmt->get_scope());

       // DQ (11/5/2007): This test is not good enough (does not handle case of nested classes and the definition
       // of member function outside of the nested class and inside of another class.
       // if (parent_class)
          if (parent_class == mfuncdecl_stmt->get_scope())
             {
            // JJW 10-23-2007 This member function is declared inside the
            // class, so its name should never be qualified

#error "DEAD CODE!"

            // printf ("mfuncdecl_stmt->get_declarationModifier().isFriend() = %s \n",mfuncdecl_stmt->get_declarationModifier().isFriend() ? "true" : "false");
               if (mfuncdecl_stmt->get_declarationModifier().isFriend() == false)
                  {
                 // printf ("Setting SkipQualifiedNames (this is a member function located in its own class) \n");
                    ninfo.set_SkipQualifiedNames();
                  }
             }
#endif
          ninfo.set_SkipClassDefinition();
          ninfo.set_SkipEnumDefinition();

#error "DEAD CODE!"

       // DQ (6/10/2007): set the declaration pointer so that the name qualification can see if this is 
       // the declaration (so that exceptions to qualification can be tracked).
          ninfo.set_declstatement_ptr(NULL);
          ninfo.set_declstatement_ptr(mfuncdecl_stmt);

#error "DEAD CODE!"

       // if (!(mfuncdecl_stmt->isConstructor() || mfuncdecl_stmt->isDestructor() || mfuncdecl_stmt->isConversion()))
          if ( !( mfuncdecl_stmt->get_specialFunctionModifier().isConstructor() || 
                  mfuncdecl_stmt->get_specialFunctionModifier().isDestructor()  ||
                  mfuncdecl_stmt->get_specialFunctionModifier().isConversion() ) )
             {
            // printf ("In unparser: NOT a constructor, destructor or conversion operator \n");

#error "DEAD CODE!"

            // printf ("mfuncdecl_stmt->get_orig_return_type() = %p \n",mfuncdecl_stmt->get_orig_return_type());

               if (mfuncdecl_stmt->get_orig_return_type() != NULL)
                  {
                 // printf ("mfuncdecl_stmt->get_orig_return_type() = %p = %s \n",mfuncdecl_stmt->get_orig_return_type(),mfuncdecl_stmt->get_orig_return_type()->sage_class_name());
                    rtype = mfuncdecl_stmt->get_orig_return_type();
                  }
                 else
                  {
                    printf ("In unparseMFuncDeclStmt: (should not happen) mfuncdecl_stmt->get_type()->get_return_type() = %p = %s \n",
                         mfuncdecl_stmt->get_type()->get_return_type(),mfuncdecl_stmt->get_type()->get_return_type()->sage_class_name());
                    rtype = mfuncdecl_stmt->get_type()->get_return_type();
                  }
            // printf ("rtype = %p = %s \n",rtype,rtype->sage_class_name());

               ninfo.set_isTypeFirstPart();
               ninfo.set_SkipClassSpecifier();

               SgUnparse_Info ninfo_for_type(ninfo);

            // DQ (6/10/2007): set the declaration pointer so that the name qualification can see if this is 
            // the declaration (so that exceptions to qualification can be tracked).
               ROSE_ASSERT(ninfo_for_type.get_declstatement_ptr() != NULL);

#error "DEAD CODE!"

            // DQ (12/20/2006): This is used to specify global qualification separately from the more general name 
            // qualification mechanism.  Note that SgVariableDeclarations don't use the requiresGlobalNameQualificationOnType
            // on the SgInitializedNames in their list since the SgVariableDeclaration IR nodes is marked directly.
            // printf ("mfuncdecl_stmt->get_requiresNameQualificationOnReturnType() = %s \n",mfuncdecl_stmt->get_requiresNameQualificationOnReturnType() ? "true" : "false");
            // curprint ( string("\n/* funcdecl_stmt->get_requiresNameQualificationOnReturnType() = " + (mfuncdecl_stmt->get_requiresNameQualificationOnReturnType() ? "true" : "false") + " */ \n";
               if (mfuncdecl_stmt->get_requiresNameQualificationOnReturnType() == true)
                  {
                 // Output the name qualification for the type in the variable declaration.
                 // But we have to do so after any modifiers are output, so in unp->u_type->unparseType().
                 // printf ("In Unparse_ExprStmt::unparseMemberFunctionDeclaration(): This return type requires a global qualifier \n");

                 // Note that general qualification of types is separated from the use of globl qualification.
                 // ninfo2.set_forceQualifiedNames();
                    ninfo_for_type.set_requiresGlobalNameQualification();
                  }

#error "DEAD CODE!"

            // DQ (5/30/2011): Added support for name qualification.
               ninfo_for_type.set_reference_node_for_qualification(mfuncdecl_stmt);
               ROSE_ASSERT(ninfo_for_type.get_reference_node_for_qualification() != NULL);

               ninfo_for_type.set_name_qualification_length(mfuncdecl_stmt->get_name_qualification_length_for_return_type());
               ninfo_for_type.set_global_qualification_required(mfuncdecl_stmt->get_global_qualification_required_for_return_type());
               ninfo_for_type.set_type_elaboration_required(mfuncdecl_stmt->get_type_elaboration_required_for_return_type());

            // unp->u_type->unparseType(rtype, ninfo);
               unp->u_type->unparseType(rtype, ninfo_for_type);

               ninfo.unset_SkipClassSpecifier();
            // printf ("In unparser: DONE with NOT a constructor, destructor or conversion operator \n");
             }
            else
             {

#error "DEAD CODE!"

            // DQ (9/17/2004): What can we assume about the return type of a constructor, destructor, or conversion operator?
               if (mfuncdecl_stmt->get_orig_return_type() == NULL)
                  {
                    printf ("mfuncdecl_stmt->get_orig_return_type() == NULL mfuncdecl_stmt = %p = %s = %s \n",
                         mfuncdecl_stmt,mfuncdecl_stmt->class_name().c_str(),mfuncdecl_stmt->get_name().str());
                  }
               ROSE_ASSERT(mfuncdecl_stmt->get_orig_return_type() != NULL);
               ROSE_ASSERT(mfuncdecl_stmt->get_type()->get_return_type() != NULL);
             }
#endif

          ROSE_ASSERT (mfuncdecl_stmt != NULL);

       // printf ("In unparser: parent_class = %p \n",parent_class);
       // printf ("In unparser: mfuncdecl_stmt->get_name() = %s \n",mfuncdecl_stmt->get_name().str());

       // DQ (10/11/2006): As part of new implementation of qualified names we now default to the generation of all qualified names unless they are skipped.
       // ninfo.set_SkipQualifiedNames();

       // ninfo.display("unparse SgMemberFunction: ninfo");
       // DQ (5/13/2011): Support for new name qualification.
          ninfo.set_name_qualification_length(mfuncdecl_stmt->get_name_qualification_length());
          ninfo.set_global_qualification_required(mfuncdecl_stmt->get_global_qualification_required());

       // Generate the qualified name
       // SgName nameQualifier = unp->u_name->generateNameQualifier( mfuncdecl_stmt , ninfo );
          SgName nameQualifier = mfuncdecl_stmt->get_qualified_name_prefix();
#if 0
          printf ("nameQualifier for member function = %s \n",nameQualifier.str());
#endif
          curprint ( nameQualifier.str() );

#if 0
       // DQ (4/2/2018): Older version of code didn't use support to get the template arguments unparsed with name qualification.
#if 0
          printf ("mfuncdecl_stmt->get_name() for member function = %s \n",mfuncdecl_stmt->get_name().str());
#endif
          curprint ( mfuncdecl_stmt->get_name().str());
#else
       // DQ (4/2/2018): Adding support for alternative and more sophisticated handling of the function name 
       // (e.g. with template arguments correctly qualified, etc.).
          if (isSgTemplateInstantiationMemberFunctionDecl(mfuncdecl_stmt) != NULL)
             {
#if 0
               printf ("Calling unparseTemplateMemberFunctionName() \n");
               curprint(" /* In unparse_helper(): Calling unparseTemplateMemberFunctionName() */ \n");
#endif

            // unp->u_exprStmt->unparseTemplateFunctionName(isSgTemplateInstantiationMemberFunctionDecl(mfuncdecl_stmt),ninfo);
               unp->u_exprStmt->unparseTemplateMemberFunctionName(isSgTemplateInstantiationMemberFunctionDecl(mfuncdecl_stmt),ninfo);

#if 0
               printf ("Done: unparseTemplateMemberFunctionName() \n");
               curprint(" /* In unparse_helper(): Done: unparseTemplateMemberFunctionName() */ \n");
#endif
             }
            else
             {
#if 0
               printf ("mfuncdecl_stmt->get_name() for member function = %s \n",mfuncdecl_stmt->get_name().str());
#endif
               curprint ( mfuncdecl_stmt->get_name().str());
             }
#endif

#if 0
       // DQ (4/1/2018): We need to output the member functions template name and the template arguments directly, 
       // so that any computed name qualification can be included.  Just the template instatiation member function 
       // name will fail to have the computed name qualification. 
          if (isSgTemplateInstantiationMemberFunctionDecl(mfuncdecl_stmt) != NULL)
             {
               printf ("WARNING: We cannot use the SgTemplateInstantiationMemberFunctionDecl stored name, e.g. get_name() function, since it will fail to have name qualification for template arguments \n");
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }
#endif

          SgUnparse_Info ninfo2(info);

       // DQ (10/10/2006): Do output any qualified names (particularly for non-defining declarations).
       // ninfo2.set_forceQualifiedNames();

       // DQ (10/17/2004): Skip output of enum and class definitions for return type! C++ standard does not permit 
       // a defining declaration within a return type, function parameter, or sizeof expression. 
          ninfo2.set_SkipClassDefinition();
          ninfo2.set_SkipEnumDefinition();

       // DQ (6/10/2007): set the declaration pointer so that the name qualification can see if this is 
       // the declaration (so that exceptions to qualification can be tracked).
          ninfo2.set_declstatement_ptr(NULL);
          ninfo2.set_declstatement_ptr(mfuncdecl_stmt);

          ninfo2.set_inArgList();
          SgName tmp_name;

       // DQ (10/7/2004): Skip output of class definition for return type! C++ standard does not permit 
       // a defining declaration within a return type, function parameter, or sizeof expression. 
          ninfo2.set_SkipClassDefinition();

#if 0
          printf ("In Unparse_ExprStmt::unparseMFuncDeclStmt(): calling unparseFunctionArgs() \n");
          curprint ("\n/* In Unparse_ExprStmt::unparseMFuncDeclStmt: calling unparseFunctionArgs() */ \n"); 
#endif
          curprint ( string("("));
       // Unparse the function arguments
          unparseFunctionArgs(mfuncdecl_stmt,ninfo2);
          curprint ( string(")"));

#if 0
          printf ("In Unparse_ExprStmt::unparseMFuncDeclStmt(): DONE: calling unparseFunctionArgs() \n");
          curprint ("\n/* In Unparse_ExprStmt::unparseMFuncDeclStmt: DONE: calling unparseFunctionArgs() */ \n"); 
#endif

#if 0
          printf ("Before calling unparseType(rtype): rtype = %p \n",rtype);
#endif

          if (rtype != NULL)
             {
               SgUnparse_Info ninfo3(ninfo);
               ninfo3.set_isTypeSecondPart();
#if 0
               printf ("In Unparse_ExprStmt::unparseMFuncDeclStmt(): calling unparseType(rtype) \n");
               curprint ("\n/* In Unparse_ExprStmt::unparseMFuncDeclStmt: calling unparseType(rtype) */ \n"); 
#endif
               unp->u_type->unparseType(rtype, ninfo3);

#if 0
               printf ("In Unparse_ExprStmt::unparseMFuncDeclStmt(): DONE: calling unparseType(rtype) \n");
               curprint ("\n/* In Unparse_ExprStmt::unparseMFuncDeclStmt: DONE: calling unparseType(rtype) */ \n"); 
#endif
             }

#if 1

#if 0
          printf ("In Unparse_ExprStmt::unparseMFuncDeclStmt(): calling unparseTrailingFunctionModifiers() \n");
          curprint ("\n/* In Unparse_ExprStmt::unparseMFuncDeclStmt: calling unparseTrailingFunctionModifiers() */ \n"); 
#endif
       // DQ (9/9/2014): Refactored support for function modifiers.
          unparseTrailingFunctionModifiers(mfuncdecl_stmt,info);
#else

#error "DEAD CODE!"

          bool outputRestrictKeyword = false;
          SgMemberFunctionType *mftype = isSgMemberFunctionType(mfuncdecl_stmt->get_type());
          if (!info.SkipFunctionQualifier() && mftype )
             {
               if (mftype->isConstFunc())
                  {
                    curprint(" const");
                  }
               if (mftype->isVolatileFunc())
                  {
                    curprint(" volatile");
                  }

            // DQ (12/11/2012): Added support for restrict (in EDG 4.x we want this to be more uniform with "const" and "volatile" modifier handling.
               if (mftype->isRestrictFunc())
                  {
#if 0
                    printf ("In unparseMFuncDeclStmt: unparse restrict keyword from specification in SgMemberFunctionType \n");
#endif
                    outputRestrictKeyword = true;

                 // DQ (12/11/2012): Make sure that this way of specifing the restrict keyword is set.
                    ROSE_ASSERT (mfuncdecl_stmt->get_declarationModifier().get_typeModifier().isRestrict() == true);

                 // curprint ( string(" restrict"));
                  }
             }

#error "DEAD CODE!"

       // DQ (12/11/2012): Avoid redundant output of the restrict keyword.
       // DQ (4/28/2004): Added support for restrict modifier
       // if (mfuncdecl_stmt->get_declarationModifier().get_typeModifier().isRestrict())
          if (mfuncdecl_stmt->get_declarationModifier().get_typeModifier().isRestrict() && (outputRestrictKeyword == false))
             {
               outputRestrictKeyword = true;
#if 0
               printf ("In unparseMFuncDeclStmt: unparse restrict keyword from specification in mfuncdecl_stmt->get_declarationModifier().get_typeModifier() \n");
#endif
            // DQ (12/11/2012): Error checking.
               if (mftype != NULL) 
                  {
                    ROSE_ASSERT (mftype->isRestrictFunc() == true);
                  }

            // curprint ( string(" restrict"));
             }

#error "DEAD CODE!"

       // DQ (12/11/2012): We have two ways of setting the specification of the restrict keyword, but we only want to output the keyword once.
       // This make this code less sensative to which way it is specified and enforces that both ways are set.
       // At the moment there are two ways that a member function is marked as restrict:
       //    1) Via it's function type modifier (const-volatile modifier)
       //    2) The declaration modifier's const-volatile modifier.
       // It does not appear that the "restrict" keyword modifies the type of the function (g++ does not allow overloading on restrict, for example).
       // Thus if it is not a part of the type then it should be a part of the declaration modifier and not in the SgMemberFunctionType.
       // So maybe we should remove it from the SgMemberFunctionType?  I am not clear on this design point at present, so we have forced both
       // to be set consistantly (and this is handled in the SageBuilder interface), plus a consistancy test in the AST consistancy tests.
       // The reason it is in the type modifier held by the declaration modifier is because it is not a prat of the function type (formally).
       // But the reason it is a part of the type modifier is because it is used for function parameter types.  This design point is
       // less than elegant and I'm not clear on what would make this simpler.  For the moment we have focused on making it consistant
       // across the two ways it can be represented (and fixing the SageBuilder Interface to set it consistantly).
          if (outputRestrictKeyword == true)
             {
               curprint(Unparse_Type::unparseRestrictKeyword());
             }

#error "DEAD CODE!"

       // DQ (4/28/2004): Added support for throw modifier
          if (mfuncdecl_stmt->get_declarationModifier().isThrow())
             {
            // Unparse SgThrow
#if 0
               printf ("In Unparse_ExprStmt::unparseMFuncDeclStmt(): Output throw modifier \n");
#endif
            // unparseThrowExp(mfuncdecl_stmt->get_throwExpression,info);
            // printf ("Incomplete implementation of throw specifier on function \n");
            // curprint ( string(" throw( /* from unparseMFuncDeclStmt() type list output not implemented */ )";
               const SgTypePtrList& exceptionSpecifierList = mfuncdecl_stmt->get_exceptionSpecification();
               unparseExceptionSpecification(exceptionSpecifierList,ninfo);
             }

#error "DEAD CODE!"

       // if (mfuncdecl_stmt->isPure())
          if (mfuncdecl_stmt->get_functionModifier().isPureVirtual())
             {
            // DQ (1/22/2013): Supress the output of the pure virtual syntax if this is the defining declaration (see test2013_26.C).
            // curprint ( string(" = 0"));
               if (mfuncdecl_stmt != mfuncdecl_stmt->get_definingDeclaration())
                  {
                    curprint(" = 0");
                  }
             }

#error "DEAD CODE!"

       // DQ (8/11/2014): Added support for final keyword unparsing.
          if (mfuncdecl_stmt->get_declarationModifier().isFinal() == true)
             {
               curprint(" final");
             }

#error "DEAD CODE!"

       // DQ (8/11/2014): Added support for final keyword unparsing.
          if (mfuncdecl_stmt->get_declarationModifier().isOverride() == true)
             {
               curprint(" override");
             }
#endif

#if 0
          curprint(" /* unparse semicolon */ ");
#endif

       // DQ (4/13/2019): Handle the case of a defaulted constructor.
       // if (mfuncdecl_stmt->isForward() && !info.SkipSemiColon())
          if ((mfuncdecl_stmt->isForward() && !info.SkipSemiColon()) || (isDefaultedOrDeletedMemberFunction == true))
             {
               curprint(";");
               if (mfuncdecl_stmt->isExternBrace())
                  {
                    curprint(" }");
                  }
             }
            else
             {
               int first = 1;
               SgInitializedNamePtrList::iterator p = mfuncdecl_stmt->get_ctors().begin();
#if 0
               printf ("Unparsing the preinitialization list \n");
#endif
            // DQ (10/17/2004): Skip output of enum and class definitions for return type! C++ standard does not permit 
            // a defining declaration within a return type, function parameter, or sizeof expression. 
            // SgUnparse_Info ninfo3(info);
            // ninfo3.set_SkipClassDefinition();
            // ninfo3.set_SkipEnumDefinition();

               while (p != mfuncdecl_stmt->get_ctors().end())
                  {
                    if (first)
                       {
                         curprint(" : ");
                         first = 0;
                       }
                      else
                       {
                         curprint(", ");
                       }

                    ROSE_ASSERT ((*p) != NULL);
                    unparseAttachedPreprocessingInfo(*p, info, PreprocessingInfo::before);

                 // DQ (12/16/2013): Here is where we need support for name qualification of SgInitializedName 
                 // objects in the preinitialization list.
#if 0
                    curprint("/* output any required name qualification for preinitialization list */ ");
#endif
                 // DQ (12/16/2013): Not clear if this should be calling "get_qualified_name()" (error: get_qualified_name() is the incorrect function to call).
                 // test2013_286.C demonstrates where we need the output of name qualification for elements of the preinitialization list.
                 // SgName nameQualifier = (*p)->get_qualified_name_prefix();
                 // SgName nameQualifier = (*p)->get_qualified_name();
                    SgName nameQualifier = (*p)->get_qualified_name_prefix();
#if 0
                    printf ("In unparseMFuncDeclStmt(): preinitialization list element name = %s nameQualifier = %s \n",(*p)->get_name().str(),(nameQualifier.is_null() == false) ? nameQualifier.str() : "NULL");
#endif
                    if (nameQualifier.is_null() == false)
                       {
                         curprint ( nameQualifier.str());
                       }
#if 0
                    curprint("/* output the name of the ctor in the preinitialization list */ ");
#endif
                    curprint((*p)->get_name().str());

                 // DQ (8/4/2005): Removed the use of "()" here since it breaks test2005_123.C
                 // DQ (8/2/2005): Added "()" to constructor initialization list (better here than for all SgAssignInitializer's expressions)
                    SgExpression* initializer = (*p)->get_initializer();
                    if (initializer != NULL)
                       {
                      // printf ("Unparsing the preinitialization list: found valid inializer = %p = %s \n",initializer,initializer->class_name().c_str());
#if 1
                         bool outputParenthesis = true;

                      // DQ (7/21/2012): I think this setting is new for the NEW EDG support (and would allow use to later remove this data member).
                         SgConstructorInitializer* constructorInitializer = isSgConstructorInitializer(initializer);
                         if (constructorInitializer != NULL)
                            {
                           // ROSE_ASSERT(constructorInitializer->get_need_parenthesis_after_name() == false);
                              outputParenthesis = (constructorInitializer->get_need_parenthesis_after_name() == false);
                            }
#else
#error "DEAD CODE!"
                      // DQ (8/6/2005): Remove "()" when SgConstructorInitializer is used in preinitialization list (since it will output a "()" already)
                      // printf ("Output parenthesis for all but constructor initializers \n");
                         bool outputParenthesis =  (isSgConstructorInitializer(initializer) == NULL);
#endif
                         if (outputParenthesis == true)
                            {
                              curprint ( string("("));
                            }
                      // DQ (6/4/2011): Set this in case the initializer is an expression that requires name 
                      // qualification (e.g. SgConstructorInitializer).  See test2005_42.C for an example.
                         info.set_reference_node_for_qualification(initializer);

                      // DQ (11/16/2013): Use ninfo2 instead of info since it has been setup to avoid things 
                      // that should not happen in preinitialization lists (e.g. output of type definitions).
                      // unparseExpression((*p)->get_initializer(), info);
                      // unparseExpression(initializer, info);
                         unparseExpression(initializer, ninfo2);

                      // DQ (6/4/2011): Clear this since we have used the input SgUnparse_Info.
                         info.set_reference_node_for_qualification(NULL);

                         if (outputParenthesis == true)
                            {
                              curprint ( string(")"));
                            }
                       }
                    p++;
                  }

            // printf ("DONE: Unparsing the preinitialization list \n");
             }
        }

  // DQ (1/23/03) Added option to support rewrite mechanism (generation of declarations)
     if (info.AddSemiColonAfterDeclaration())
        {
          curprint ( string(";"));
        }

  // Unparse any comments of directives attached to the SgCtorInitializerList
     if (mfuncdecl_stmt->get_CtorInitializerList() != NULL)
        {
          unparseAttachedPreprocessingInfo(mfuncdecl_stmt->get_CtorInitializerList(), info, PreprocessingInfo::after);
        }

#if 0
     printf ("Leaving Unparse_ExprStmt::unparseMFuncDeclStmt(stmt = %p = %s) \n",stmt,stmt->class_name().c_str());
     curprint ("/*  Leaving Unparse_ExprStmt::unparseMFuncDeclStmt(stm) */ ");
#endif
   }


void
Unparse_ExprStmt::unparseTrailingFunctionModifiers(SgMemberFunctionDeclaration* mfuncdecl_stmt, SgUnparse_Info& info)
   {
  // DQ (9/9/2014): Refactored support for function modifiers.
     bool outputRestrictKeyword = false;
     SgMemberFunctionType *mftype = isSgMemberFunctionType(mfuncdecl_stmt->get_type());

  // DQ (9/9/2014): Note this was using info where it was refactored from and ninfo is passed to this function.
     if (!info.SkipFunctionQualifier() && mftype )
        {
          if (mftype->isConstFunc())
             {
               curprint(" const");
             }
          if (mftype->isVolatileFunc())
             {
               curprint(" volatile");
             }

       // DQ (12/11/2012): Added support for restrict (in EDG 4.x we want this to be more uniform with "const" and "volatile" modifier handling.
          if (mftype->isRestrictFunc())
             {
#if 0
               printf ("In unparseTrailingFunctionModifiers: unparse restrict keyword from specification in SgMemberFunctionType \n");
#endif
               outputRestrictKeyword = true;

            // DQ (12/11/2012): Make sure that this way of specifing the restrict keyword is set.
               ROSE_ASSERT (mfuncdecl_stmt->get_declarationModifier().get_typeModifier().isRestrict() == true);

            // curprint ( string(" restrict"));
             }
        }

  // DQ (12/11/2012): Avoid redundant output of the restrict keyword.
  // DQ (4/28/2004): Added support for restrict modifier
  // if (mfuncdecl_stmt->get_declarationModifier().get_typeModifier().isRestrict())
     if (mfuncdecl_stmt->get_declarationModifier().get_typeModifier().isRestrict() && (outputRestrictKeyword == false))
        {
          outputRestrictKeyword = true;
#if 0
          printf ("In unparseTrailingFunctionModifiers: unparse restrict keyword from specification in mfuncdecl_stmt->get_declarationModifier().get_typeModifier() \n");
#endif
       // DQ (12/11/2012): Error checking.
          if (mftype != NULL) 
             {
               ROSE_ASSERT (mftype->isRestrictFunc() == true);
             }

       // curprint ( string(" restrict"));
        }

  // DQ (12/11/2012): We have two ways of setting the specification of the restrict keyword, but we only want to output the keyword once.
  // This make this code less sensative to which way it is specified and enforces that both ways are set.
  // At the moment there are two ways that a member function is marked as restrict:
  //    1) Via it's function type modifier (const-volatile modifier)
  //    2) The declaration modifier's const-volatile modifier.
  // It does not appear that the "restrict" keyword modifies the type of the function (g++ does not allow overloading on restrict, for example).
  // Thus if it is not a part of the type then it should be a part of the declaration modifier and not in the SgMemberFunctionType.
  // So maybe we should remove it from the SgMemberFunctionType?  I am not clear on this design point at present, so we have forced both
  // to be set consistantly (and this is handled in the SageBuilder interface), plus a consistancy test in the AST consistancy tests.
  // The reason it is in the type modifier held by the declaration modifier is because it is not a prat of the function type (formally).
  // But the reason it is a part of the type modifier is because it is used for function parameter types.  This design point is
  // less than elegant and I'm not clear on what would make this simpler.  For the moment we have focused on making it consistant
  // across the two ways it can be represented (and fixing the SageBuilder Interface to set it consistantly).
     if (outputRestrictKeyword == true)
        {
          curprint(Unparse_Type::unparseRestrictKeyword());
        }

  // DQ (4/28/2004): Added support for throw modifier
     if (mfuncdecl_stmt->get_declarationModifier().isThrow())
        {
       // Unparse SgThrow
#if 0
          printf ("In Unparse_ExprStmt::unparseTrailingFunctionModifiers(): Output throw modifier \n");
#endif
       // unparseThrowExp(mfuncdecl_stmt->get_throwExpression,info);
       // printf ("Incomplete implementation of throw specifier on function \n");
       // curprint ( string(" throw( /* from unparseTrailingFunctionModifiers() type list output not implemented */ )";
          const SgTypePtrList& exceptionSpecifierList = mfuncdecl_stmt->get_exceptionSpecification();
       // unparseExceptionSpecification(exceptionSpecifierList,ninfo);
          unparseExceptionSpecification(exceptionSpecifierList,info);
        }

  // if (mfuncdecl_stmt->isPure())
     if (mfuncdecl_stmt->get_functionModifier().isPureVirtual())
        {
       // DQ (1/22/2013): Supress the output of the pure virtual syntax if this is the defining declaration (see test2013_26.C).
       // curprint ( string(" = 0"));
          if (mfuncdecl_stmt != mfuncdecl_stmt->get_definingDeclaration())
             {
               curprint(" = 0");
             }
        }

  // DQ (8/11/2014): Added support for final keyword unparsing.
     if (mfuncdecl_stmt->get_declarationModifier().isFinal() == true)
        {
       // DQ (2/12/2019): Testing, final can't be used on prototypes (I think).
       // curprint(" /* output from test 1 */ ");
          curprint(" final");
        }

  // DQ (8/11/2014): Added support for final keyword unparsing.
     if (mfuncdecl_stmt->get_declarationModifier().isOverride() == true)
        {
          curprint(" override");
        }

  // DQ (4/13/2019): Added support for default keyword unparsing.
     if (mfuncdecl_stmt->get_functionModifier().isMarkedDefault() == true)
        {
          curprint(" = default");
        }

  // DQ (4/13/2019): Added support for delete keyword unparsing.
     if (mfuncdecl_stmt->get_functionModifier().isMarkedDelete() == true)
        {
          curprint(" = delete");
        }
   }


void
Unparse_ExprStmt::unparseVarDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
#define DEBUG_VARIABLE_DECLARATION 0

#if DEBUG_VARIABLE_DECLARATION
     printf ("Inside of unparseVarDeclStmt(%p) \n",stmt);
#endif
#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES || 0
     printf ("Inside of unparseVarDeclStmt(%p) \n",stmt);
  // ROSE_ASSERT(info.get_current_scope() != NULL);
  // printf ("An the current scope is (from info): info.get_current_scope() = %p = %s = %s \n",
  //      info.get_current_scope(),info.get_current_scope()->class_name().c_str(),SageInterface::get_name(info.get_current_scope()).c_str());
     curprint("\n /* Inside of unparseVarDeclStmt() */ \n");
#endif
#if DEBUG_VARIABLE_DECLARATION
     curprint("\n /* Inside of unparseVarDeclStmt() */ \n");
#endif
#if 0
     curprint("/* Inside of unparseVarDeclStmt() */");
#endif

     SgVariableDeclaration* vardecl_stmt = isSgVariableDeclaration(stmt);
     ROSE_ASSERT(vardecl_stmt != NULL);

#if 0
     printf ("In unparseVarDeclStmt(): vardecl_stmt->get_declarationModifier().get_storageModifier().isStatic()  = %s \n",vardecl_stmt->get_declarationModifier().get_storageModifier().isStatic() ? "true" : "false");
     printf ("In unparseVarDeclStmt(): vardecl_stmt->get_declarationModifier().get_storageModifier().isExtern()  = %s \n",vardecl_stmt->get_declarationModifier().get_storageModifier().isExtern() ? "true" : "false");
     printf ("In unparseVarDeclStmt(): vardecl_stmt->get_declarationModifier().get_storageModifier().isMutable() = %s \n",vardecl_stmt->get_declarationModifier().get_storageModifier().isMutable() ? "true" : "false");
     printf ("In unparseVarDeclStmt(): vardecl_stmt->get_is_thread_local  = %s \n",vardecl_stmt->get_is_thread_local() ? "true" : "false");
#endif

  // DQ (4/14/2015): This should always be false because there is nothing to partialy unparse within a variable declaration (that we support).
     bool saved_unparsedPartiallyUsingTokenStream = info.unparsedPartiallyUsingTokenStream();
     ROSE_ASSERT(saved_unparsedPartiallyUsingTokenStream == false);

  // DQ (7/25/2014): We can assume that if this is g++ then we are using gcc for the backend C compiler.
     bool usingGxx = false;
     #ifdef USE_CMAKE
       #ifdef CMAKE_COMPILER_IS_GNUCXX
         usingGxx = true;
       #endif
     #else
       string backEndCompiler = BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH;
       usingGxx = (backEndCompiler == "g++");
     #endif

#if 0
  // DQ (5/24/2015): Moved to output specifier after the "extern" and "static" keywords.
     if (usingGxx)
        {
          SgFile* file = TransformationSupport::getFile(vardecl_stmt);
#if 0
          printf ("In unparseVarDeclStmt(): resolving file to be %p \n",file);
#endif
          bool is_Cxx_Compiler = false;
          bool is_C_Compiler   = false;
          if (file != NULL)
             {
               is_Cxx_Compiler = file->get_Cxx_only();
               is_C_Compiler   = file->get_C_only();
             }
            else
             {
            // DQ (3/6/2017): Added support for message logging to control output from ROSE tools.
               mprintf ("Warning: TransformationSupport::getFile(vardecl_stmt) == NULL \n");
             }

       // For C we need to use the GNU 4.9 compiler.
       // Now check the version of the identified GNU g++ compiler.
          if ((BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER == 4 && BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER >= 9) || (BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER > 4))
              {
            // DQ (7/25/2014): Adding C11 thread local support.
            // if (vardecl_stmt->get_is_thread_local() == true)
               if (is_C_Compiler == true && vardecl_stmt->get_is_thread_local() == true)
                  {
                    curprint("_Thread_local ");
                  }
             }
            else
             {
            // For C++ we can use the GNU 4.8 compiler.
               if ((BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER == 4 && BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER >= 8) || (BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER > 4))
                  {
                 // DQ (8/13/2014): Adding C++11 thread local support.
                    if (is_Cxx_Compiler == true && vardecl_stmt->get_is_thread_local() == true)
                       {
                         curprint("thread_local ");
                       }
                  }
                 else
                  {
                 // DQ (5/24/2015): Adding support for GNU __thread keyword (thread local support for older versions of C).
                 // For older compilers we use the __thread modifier.  This may also we what is required for non-C11 support.
                    if (is_C_Compiler == true && vardecl_stmt->get_is_thread_local() == true)
                       {
                         curprint("__thread ");
                       }
                  }
             }
        }
#endif

#if 0
     vardecl_stmt->get_declarationModifier().display("Called from unparseVarDeclStmt()");
#endif

  // printf ("In unparseVarDeclStmt(): info.get_current_scope() = %p = %s \n",info.get_current_scope(),info.get_current_scope()->class_name().c_str());

  // curprint ( string("\n/* In unparseVarDeclStmt(): vardecl_stmt->get_need_name_qualifier() = " + (vardecl_stmt->get_need_name_qualifier() == true ? "true" : "false") + " */ \n";

  // 3 types of output
  //    var1=2, var2=3 (enum list)
  //    int var1=2, int var2=2 (arg list)
  //    int var1=2, var2=2 ; (vardecl list)
  // must also allow for this
  //    void (*set_foo)()=doo

#if 0
     printf ("In unparseVarDeclStmt(): info.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
     printf ("In unparseVarDeclStmt(): info.SkipEnumDefinition()  = %s \n",(info.SkipEnumDefinition() == true) ? "true" : "false");
     printf ("In unparseVarDeclStmt(): info.SkipBaseType()        = %s \n",(info.SkipBaseType() == true) ? "true" : "false");
#endif

  // DQ (1/9/2014): These should have been setup to be the same.
     ROSE_ASSERT(info.SkipClassDefinition() == info.SkipEnumDefinition());

  // Build a new SgUnparse_Info object to represent formatting options for this statement
     SgUnparse_Info ninfo(info);

#if 0
     ninfo.display ("At top of Unparse_ExprStmt::unparseVarDeclStmt");
#endif

#if DEBUG_VARIABLE_DECLARATION
     printf ("Inside of unparseVarDeclStmt(): ninfo.SkipBaseType() = %s \n",ninfo.SkipBaseType() ? "true" : "false");
#endif

  // DQ (10/14/2006): Set the context to record the variable declaration being generated!
  // We can use this later if we have to query the AST for if a variable using a class 
  // declaration appears before or after it's definition (required to know how to qualify 
  // the SgClassType)
     ninfo.set_declstatement_ptr(NULL);
     ninfo.set_declstatement_ptr(vardecl_stmt);

  // curprint ( string("\n/* After being set (unparseVarDeclStmt): ninfo.get_declstatement_ptr() = " + 
  //    ((ninfo.get_declstatement_ptr() != NULL) ? ninfo.get_declstatement_ptr()->class_name() : "no declaration statement defined") + " */\n ";
     ROSE_ASSERT(ninfo.get_declstatement_ptr() != NULL);

  // DQ (11/29/2004): This code is required for the output of the access specifier 
  // (public, protected, private) and applies only within classes.  Use get_parent()
  // instead of get_scope() since we are looking for the structural position of the 
  // variable declaration (is it is a class).
     SgClassDefinition *classDefinition = isSgClassDefinition(vardecl_stmt->get_parent());
     if (classDefinition != NULL)
        {
       // Don't output an access specifier in this is a struct or union!
       // printf ("Don't output an access specifier in this is a struct or union! \n");

       // DQ and PC (6/1/2006): Added Peter's suggested fixes to support unparsing fully qualified names (supporting auto-documentation).
       // if (classDefinition->get_declaration()->get_class_type() == SgClassDeclaration::e_class)
          if (classDefinition->get_declaration()->get_class_type() == SgClassDeclaration::e_class && !info.skipCheckAccess())
               ninfo.set_CheckAccess();
       // inClass = true;
       // inCname = isSgClassDefinition(vardecl_stmt->get_parent())->get_declaration()->get_name();
        }

     ROSE_ASSERT(ninfo.get_declstatement_ptr() != NULL);

#if DEBUG_VARIABLE_DECLARATION
     printf ("Calling unp->u_sage->printSpecifier1() \n");
     curprint("\n/* Calling unp->u_sage->printSpecifier1() */ \n");
  // printDebugInfo("entering unp->u_sage->printSpecifier1", true);
#endif

     unp->u_sage->printSpecifier1(vardecl_stmt, ninfo);

#if DEBUG_VARIABLE_DECLARATION
     printf ("DONE: Calling unp->u_sage->printSpecifier1() \n");
     curprint("\n/* DONE: Calling unp->u_sage->printSpecifier1() */ \n");
#endif

  // Save the input information
     SgUnparse_Info saved_ninfo(ninfo); 
  // this call has been moved below, after we indent
  // unp->u_sage->printSpecifier2(vardecl_stmt, ninfo);

  // Setup the SgUnparse_Info object for this statement
     ninfo.unset_CheckAccess();
     info.set_access_attribute(ninfo.get_access_attribute());

  // DQ (11/28/2004): There should be at least a single variable here!
     ROSE_ASSERT(vardecl_stmt->get_variables().size() > 0);
  // printf ("Number of variables = %" PRIuPTR " \n",vardecl_stmt->get_variables().size());

  // SgInitializedNamePtrList::iterator p = vardecl_stmt->get_variables().begin();
  // ROSE_ASSERT(p != vardecl_stmt->get_variables().end());

     SgName tmp_name;
     SgType *tmp_type        = NULL;
  // SgPointerMemberType *pm_type = NULL;
     SgInitializer *tmp_init = NULL;

     SgInitializedName *decl_item      = NULL;
  // SgInitializedName *prev_decl_item = NULL;

  // DQ (11/28/2004): Is this even used in enum declarations! (I don't think so!)
     ROSE_ASSERT(ninfo.inEnumDecl() == false);

  // DQ (11/28/2004): Within an enum declaration there should not be a type associated with the variables???
     if (ninfo.inEnumDecl())
          ninfo.unset_isWithType();
       else
          ninfo.set_isWithType();

  // DQ (10/5/2004): This is the explicitly set boolean value which indicates that a class declaration is buried inside
  // the current variable declaration (e.g. struct A { int x; } a;).  In this case we have to output the base type with
  // its definition.
     bool outputTypeDefinition = vardecl_stmt->get_variableDeclarationContainsBaseTypeDefiningDeclaration();

#if 0
     printf ("In unparseVarDeclStmt(): vardecl_stmt = %p outputTypeDefinition = %s \n",vardecl_stmt,(outputTypeDefinition == true) ? "true" : "false");
#endif

  // if (p != vardecl_stmt->get_variables().end())
     SgInitializedNamePtrList::iterator p = vardecl_stmt->get_variables().begin();
     ROSE_ASSERT(p != vardecl_stmt->get_variables().end());

#if 0
     printf ("vardecl_stmt->get_variables().size() = %" PRIuPTR " \n",vardecl_stmt->get_variables().size());
     printf ("(*p)->get_using_C11_Alignas_keyword() = %s \n",(*p)->get_using_C11_Alignas_keyword() ? "true" : "false");
#endif

  // DQ (7/26/2014): Adding support for C11 _Alignas keyword.
     if ((*p)->get_using_C11_Alignas_keyword() == true)
        {
          curprint("_Alignas(");
          SgNode*       constant_or_type = (*p)->get_constant_or_type_argument_for_Alignas_keyword();
          SgType*       type_operand     = isSgType(constant_or_type);
          SgExpression* constant_operand = isSgExpression(constant_or_type);
          if (type_operand != NULL)
             {
               unp->u_type->unparseType(type_operand,info);
             }
            else
             {
               if (constant_operand != NULL)
                  {
                    unparseExpression(constant_operand,info);
                  }
                 else
                  {
                    printf ("Error: C11 _Alignas operand is not a type or constant \n");
                    ROSE_ASSERT(false);
                  }
             }
          curprint(")");
        }

     while (p != vardecl_stmt->get_variables().end())
        {
#if DEBUG_VARIABLE_DECLARATION
          curprint ("\n /* Inside of unparseVarDeclStmt(): top of loop over variables */ \n");
#endif
          decl_item = *p;
          ROSE_ASSERT(decl_item != NULL);

          tmp_init = NULL;

          tmp_type = decl_item->get_type();

       // DQ (5/11/2007): This fails in astCopy_tests for copyExample using copyExampleInput.C
          ROSE_ASSERT(isSgType(tmp_type) != NULL);

#if 0
          printf ("SgInitializedName decl_item = %p \n",decl_item);
          decl_item->get_startOfConstruct()->display("SgInitializedName decl_item");
#endif

#define REWRITE_MACRO_HANDLING
#ifdef REWRITE_MACRO_HANDLING
       // DQ (9/17/2003) feature required for rewrite mechanism (macro handling)

       // Search for special coded declaration which are specific to the 
       // handling of MACROS in ROSE.  variable declarations of a specific form:
       //      char * rose_macro_declaration_1 = "<macro string>";
       // are unparsed as:
       //      macro string
       // to permit macros to be passed unevaluated through the transformation 
       // process (unevaluated in the intermediate files generated to process 
       // the transformations).

          if (!vardecl_stmt->isForward() && !isSgFunctionType(tmp_type))
             {
               ROSE_ASSERT (decl_item != NULL);
               tmp_init = decl_item->get_initializer();

               if (tmp_init != NULL)
                  {
                    SgAssignInitializer *assignInitializer = isSgAssignInitializer(tmp_init);
                    if (assignInitializer != NULL)
                       {
                         SgStringVal *stringValueExpression = isSgStringVal(assignInitializer->get_operand());
                         if (stringValueExpression != NULL)
                            {
#ifndef CXX_IS_ROSE_CODE_GENERATION
                           // DQ (3/25/2006): Finally we can use the C++ string class
                              string targetString = "ROSE-MACRO-CALL:";
                              int targetStringLength = targetString.size();
                              string stringValue = stringValueExpression->get_value();
                              string::size_type location = stringValue.find(targetString);
                              if (location != string::npos)
                                 {
                                // unparse the string without the surrounding quotes and with a new line at the end
                                   string remainingString = stringValue.replace(location,targetStringLength,"");
                                // printf ("Specify a MACRO: remainingString = %s \n",remainingString.c_str());
                                   remainingString.replace(remainingString.find("\\\""),4,"\"");
                                   curprint ( string("\n" ) + remainingString + "\n");
                                   return;
                                 }
#endif
                            }
                       }
                  }
             }
#endif

       // DQ (7/26/2014): comment out to avoid compiler warning.
       // bool first = true;

       // DQ (11/28/2004): I hate it when "while(true)" is used!
       // while(true)
          if (p == vardecl_stmt->get_variables().begin())
             {
            // If this is the first variable then output the base type
#if DEBUG_VARIABLE_DECLARATION
               curprint ("/* In unparseVarDeclStmt(): (first variable): cname = decl_item->get_name() = " + decl_item->get_name() + " */ \n");
#endif
#if 0
            // DQ (5/6/2013): Experiment with calling the new refactored function. This code is perhaps specific to the problem demonstrated in test2013_153.C.
            // void outputType(T* referenceNode, SgType* referenceNodeType, SgUnparse_Info & info);
            // unp->u_type->outputType<SgInitializedName>(decl_item,tmp_type,ninfo);
               SgPointerType* pointerType = isSgPointerType(tmp_type);
               bool outputAsFunctionPointer = false;
               if (pointerType != NULL && isSgFunctionType(pointerType->get_base_type()) != NULL)
                  {
                    outputAsFunctionPointer = true;
                  }

#error "DEAD CODE!"

               printf ("outputAsFunctionPointer = %s \n",outputAsFunctionPointer ? "true" : "false");

               outputAsFunctionPointer = true;
               if (outputAsFunctionPointer == false)
                  {
                 // This is a new branch that handles the typical case...(outputAsFunctionPointer == false)
                    unp->u_type->unparseType(tmp_type, ninfo);
                    SgName nm = decl_item->get_name();
                    curprint(nm + " ");
                  }
                 else
                  {
                 // This is the original branch that was always taken.
#endif
               decl_item = *p;
               ROSE_ASSERT(decl_item != NULL);
#if DEBUG_VARIABLE_DECLARATION
               curprint ("\n /* In unparseVarDeclStmt(): before comments for cname = decl_item->get_name() = " + decl_item->get_name() + " */ \n");
#endif
               unp->u_exprStmt->unparseAttachedPreprocessingInfo(decl_item, info, PreprocessingInfo::before);
#if DEBUG_VARIABLE_DECLARATION
               printf ("In unparseVarDeclStmt(): cname = decl_item->get_name() = %s \n",decl_item->get_name().str());
#endif
#if DEBUG_VARIABLE_DECLARATION
               curprint ("\n /* In unparseVarDeclStmt(): cname = decl_item->get_name() = " + decl_item->get_name() + " */ \n");
#endif
               tmp_name = decl_item->get_name();
               tmp_type = decl_item->get_type();
               ROSE_ASSERT(isSgType(tmp_type) != NULL);

            // TV (09/06/2018): if auto keyword is used then we unparse the associated declared type (before `auto` is resolved)
               if (decl_item->get_auto_decltype() != NULL)
                  {
                    tmp_type = decl_item->get_auto_decltype();
                  }

            // DQ (11/28/2004): Added to support new design
               tmp_init = decl_item->get_initializer();

#if DEBUG_VARIABLE_DECLARATION
               printf ("In unparseVarDeclStmt(): tmp_type = %p = %s \n",tmp_type,tmp_type->class_name().c_str());
#endif

            // DQ (10/17/2012): I am not clear why we need the "declStmt" variable, since it does not appear to be used for anything important.
            // In particular it appears that we carefully construct it to be either the defining or non-defining declaration; but I don't see why.
            // I recall in the past that this was an important subject for handling the output of class definitions in types, but I don't see 
            // that below.  So maybe this part of the code could be greatly simplified.

               SgNamedType *namedType = isSgNamedType(tmp_type->findBaseType());
            // SgDeclarationStatement* declStmt = (namedType) ? namedType->get_declaration() : NULL;
               SgDeclarationStatement* declStmt = NULL;

               ROSE_ASSERT(ninfo.get_declstatement_ptr() != NULL);

#if DEBUG_VARIABLE_DECLARATION
               printf ("Inside of unparseVarDeclStmt: namedType = %p \n",namedType);
#endif

               if (namedType != NULL)
                  {
                 // DQ (10/5/2004): This controls the unparsing of the class definition
                 // when unparsing the type within this variable declaration.
#if 0
                    printf ("In unparseVarDeclStmt(): outputTypeDefinition = %s \n",outputTypeDefinition ? "true" : "false");
#endif
                    if (outputTypeDefinition == true)
                       {
                      // printf ("In unparseVarDeclStmt(): Use the defining declaration as a basis for the variable declaration \n");
                         ROSE_ASSERT(namedType->get_declaration() != NULL);
                         declStmt = namedType->get_declaration()->get_definingDeclaration();
                      // printf ("outputTypeDefinition == true: declStmt = %p \n",declStmt);
                         if (declStmt == NULL)
                            {
#if 0
                              printf ("namedType->get_declaration() = %p = %s \n",namedType->get_declaration(),namedType->get_declaration()->class_name().c_str());
#endif
                           // it is likely an enum declaration which does not yet use the defing vs. non-defining mechanisms
                           // so just set it to the currently available declaration (since for enums there is only one!)
                              declStmt = namedType->get_declaration();
                            }
                         ROSE_ASSERT(declStmt != NULL);
#if 0
                      // DQ (12/12/2016): Added debugging information.
                         if (declStmt->isForward() == true)
                            {
                              printf ("In unparseVarDeclStmt(): Detecting a forward declaration \n");
                              printf ("In unparseVarDeclStmt(): vardecl_stmt = %p = %s \n",vardecl_stmt,vardecl_stmt->class_name().c_str());
                              printf ("In unparseVarDeclStmt(): decl_item = %p = %s \n",decl_item,decl_item->class_name().c_str());
                              printf ("In unparseVarDeclStmt(): cname = decl_item->get_name() = %s \n",decl_item->get_name().str());
                              printf ("In unparseVarDeclStmt(): tmp_type = %p = %s \n",tmp_type,tmp_type->class_name().c_str());

                              printf ("Calling unp->u_sage->printSpecifier2 \n");
                              curprint ("\n/* Calling unp->u_sage->printSpecifier2() */ \n");
#if 0
                              declStmt->get_file_info()->display("ERROR: This should be a forward declaration: debug");
#endif
#if 0
                           // Put a marker into the generated code so that I can better understand where this appears.
                              curprint("\n/* Note: declStmt->isForward() == true */\n ");
#endif
                            }
#endif
                      // DQ (12/18/2016): We can't enforce this in the case of C++11 lambda functions passed into functions as variables.
                      // See Cxx11_tests/test2016_95.C for an example of this.
                      // ROSE_ASSERT(declStmt->isForward() == false);

                      // DQ (10/9/2006): Don't output the qualified name of a defining declaration.
                      // ninfo.unset_forceQualifiedNames();

                      // DQ (10/11/2006): As part of new implementation of qualified names we now default to the generation of all qualified names unless they are skipped.
                         ninfo.set_SkipQualifiedNames();

                      // DQ (5/8/2013): This is a problem for where we call the unparser in the name qualification and set 
                      // SkipEnumDefinition and SkipClassDefinition to generate types with qualified names.
                      // DQ (5/23/2007): Commented these out since they are not applicable for statement expressions (see test2007_51.C).
                      // DQ (10/5/2004): If this is a defining declaration then make sure that we don't skip the definition
                      // ROSE_ASSERT(ninfo.SkipClassDefinition() == false);
                      // ROSE_ASSERT(ninfo.SkipEnumDefinition()  == false);
                      // ROSE_ASSERT(ninfo.SkipDefinition()      == false);
                       }
                      else
                       {
#if 0
                         printf ("In unparseVarDeclStmt(): outputTypeDefinition == false \n");
                         curprint("\n/* Note: outputTypeDefinition == false */\n ");
#endif
                      // printf ("Use what is surely a non-defining declaration as a basis for the variable declaration \n");
                         declStmt = namedType->get_declaration();
                         ROSE_ASSERT(declStmt != NULL);
                      // printf ("outputTypeDefinition == false: declStmt = %p \n",declStmt);
                      // Only implement test for class declarations
                         if (isSgClassDeclaration(declStmt) != NULL)
                            {
                           // DQ (10/8/2004): If this is not a enum then expect a forward declaration!
                           // Enum declarations are an exception since forward declarations of enums 
                           // are not permitted in C and C++ according to the standard!
                              if (declStmt->isForward() == false)
                                 {
                                   printf ("Warning about declStmt = %p = %s \n",declStmt,declStmt->sage_class_name());
                                   declStmt->get_file_info()->display("Warning: declStmt->isForward() == false (should be true)");
                                 }

#if PRINT_DEVELOPER_WARNINGS
                             printf ("Commented out assertion in unparser to handle AST Merge bug: declStmt->isForward() == true \n");
#endif
                          // ROSE_ASSERT(declStmt->isForward() == true);
                            }

                      // DQ (8/6/2007): Skip forcing the output of qualified names now that we have a hidden list mechanism.
                      // DQ (10/9/2006): Force output any qualified names (particularly for non-defining declarations).
                      // This is a special case for types of variable declarations.
                      // ninfo.set_forceQualifiedNames();

                      // curprint ( string("/* outputTypeDefinition = false and calling ninfo.set_forceQualifiedNames() */ ";

                      // DQ (10/5/2004): If this is a non-defining declaration then skip the definition
                      // ninfo.set_SkipClassDefinition();
                         ninfo.set_SkipDefinition();
                         ROSE_ASSERT(ninfo.SkipClassDefinition() == true);
                         ROSE_ASSERT(ninfo.SkipEnumDefinition() == true);
                       }
                  }

#if DEBUG_VARIABLE_DECLARATION
               printf ("Calling unp->u_sage->printSpecifier2 \n");
               curprint ("\n/* Calling unp->u_sage->printSpecifier2() */ \n");
            // printDebugInfo("entering unp->u_sage->printSpecifier2", true);
#endif
               unp->u_sage->printSpecifier2(vardecl_stmt, saved_ninfo);

#if DEBUG_VARIABLE_DECLARATION
               printf ("DONE: Calling unp->u_sage->printSpecifier2 \n");
               curprint ("\n/* DONE: Calling unp->u_sage->printSpecifier2() */ \n");
#endif

#if 1
               SgFile* file = TransformationSupport::getFile(vardecl_stmt);
#if 0
               printf ("In unparseVarDeclStmt(): resolving file to be %p \n",file);
#endif
               bool is_Cxx_Compiler = false;
               bool is_C_Compiler   = false;
               if (file != NULL)
                  {
                    is_Cxx_Compiler = file->get_Cxx_only();
                    is_C_Compiler   = file->get_C_only();
                  }
                 else
                  {
                 // DQ (3/6/2017): Added support for message logging to control output from ROSE tools.
                    mprintf ("Warning: TransformationSupport::getFile(vardecl_stmt) == NULL \n");
                  }
#if 0
               printf ("In unparseVarDeclStmt(): is_C_Compiler = %s is_Cxx_Compiler = %s \n",is_C_Compiler ? "true" : "false",is_Cxx_Compiler ? "true" : "false");
#endif
            // DQ (5/24/2015): Moved to output specifier after the "extern" and "static" keywords.
            // Note this this is required for test2009_19.c.
               if (usingGxx)
                  {
#if 0
                    SgFile* file = TransformationSupport::getFile(vardecl_stmt);
#if 0
                    printf ("In unparseVarDeclStmt(): resolving file to be %p \n",file);
#endif
                    bool is_Cxx_Compiler = false;
                    bool is_C_Compiler   = false;
                    if (file != NULL)
                       {
                         is_Cxx_Compiler = file->get_Cxx_only();
                         is_C_Compiler   = file->get_C_only();
                       }
                      else
                       {
                         printf ("Warning: TransformationSupport::getFile(vardecl_stmt) == NULL \n");
                       }
#if 0
                    printf ("In unparseVarDeclStmt(): is_C_Compiler = %s is_Cxx_Compiler = %s \n",is_C_Compiler ? "true" : "false",is_Cxx_Compiler ? "true" : "false");
#endif
#endif
                 // DQ (5/24/2015): I think I menat to say that For C we need to use the EDG 4.9 frontend (?).
                 // For C we need to use the GNU 4.9 compiler.
                 // Now check the version of the identified GNU g++ compiler.
                    if ((BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER == 4 && BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER >= 9) || (BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER > 4))
                        {
                      // DQ (7/25/2014): Adding C11 thread local support.
                      // if (vardecl_stmt->get_is_thread_local() == true)
#if 0
                         printf ("In unparseVarDeclStmt(): GNU or EDG? 4.9 or greater: vardecl_stmt->get_is_thread_local() = %s \n",vardecl_stmt->get_is_thread_local() ? "true" : "false");
#endif
                         if (is_C_Compiler == true && vardecl_stmt->get_is_thread_local() == true)
                            {
                              curprint("_Thread_local ");
                            }
                       }
                      else
                       {
                      // For C++ we can use the GNU 4.8 compiler.
                         if ((BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER == 4 && BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER >= 8) || (BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER > 4))
                            {
                           // DQ (8/13/2014): Adding C++11 thread local support.
                              if (is_Cxx_Compiler == true && vardecl_stmt->get_is_thread_local() == true)
                                 {
                                   curprint("thread_local ");
                                 }
                                else
                                 {
                                // DQ (5/24/2015): Added this case to support the C language work even when using the GNU 4.8 compiler.
                                   if (is_C_Compiler == true && vardecl_stmt->get_is_thread_local() == true)
                                      {
                                     // curprint("_Thread_local ");
                                        curprint("__thread ");
                                      }
                                 }
                            }
                           else
                            {
                           // DQ (5/24/2015): Adding support for GNU __thread keyword (thread local support for older versions of C).
                           // For older compilers we use the __thread modifier.  This may also we what is required for non-C11 support.
                              if (is_C_Compiler == true && vardecl_stmt->get_is_thread_local() == true)
                                 {
                                   curprint("__thread ");
                                 }
                            }
                       }
                  }
                 else
                  {
                 // DQ (8/28/2015): Adding support for not GNU compilers (e.g. Intel v14 compiler.
#if 0
                    printf ("usingGxx == false: so __thread not output: vardecl_stmt->get_is_thread_local() == %s \n",(vardecl_stmt->get_is_thread_local() == true) ? "true" : "false");
#endif
                 // DQ (8/13/2014): Adding C++11 thread local support.
                    if (is_Cxx_Compiler == true && vardecl_stmt->get_is_thread_local() == true)
                       {
                         curprint("thread_local ");
                       }
                      else
                       {
                      // DQ (5/24/2015): Added this case to support the C language work even when using the GNU 4.8 compiler.
                         if (is_C_Compiler == true && vardecl_stmt->get_is_thread_local() == true)
                            {
                           // curprint("_Thread_local ");
                              curprint("__thread ");
                            }
                       }
                  }
#endif

            // DQ (11/28/2004): Are these true! No! declStmt is really the declaration of any parent scope (excluding global scope!)
            // ROSE_ASSERT(declStmt != NULL);
            // ROSE_ASSERT(isSgTypedefDeclaration(declStmt) == NULL);

#if 0
            // printf ("Inside of unparseVarDeclStmt: first = %s \n",(first == true) ? "true" : "false");
               printf ("Inside of unparseVarDeclStmt: declStmt = %p \n",declStmt);
               printf ("Inside of unparseVarDeclStmt: isSgTypedefDeclaration(declStmt) = %p \n",isSgTypedefDeclaration(declStmt));
#endif

            // DQ (11/28/2004): Is this always true?  If so then we can simplify the code!
               ROSE_ASSERT(tmp_type != NULL);

               SgUnparse_Info ninfo_for_type(ninfo);
#if 0
               printf ("In unparseVarDeclStmt(): ninfo_for_type.SkipClassDefinition() = %s \n",(ninfo_for_type.SkipClassDefinition() == true) ? "true" : "false");
               printf ("In unparseVarDeclStmt(): ninfo_for_type.SkipEnumDefinition()  = %s \n",(ninfo_for_type.SkipEnumDefinition() == true) ? "true" : "false");
               printf ("In unparseVarDeclStmt(): ninfo_for_type.SkipBaseType()        = %s \n",(ninfo_for_type.SkipBaseType() == true) ? "true" : "false");
#endif

            // DQ (1/9/2014): These should have been setup to be the same.
               ROSE_ASSERT(ninfo_for_type.SkipClassDefinition() == ninfo_for_type.SkipEnumDefinition());

               ROSE_ASSERT(ninfo_for_type.get_declstatement_ptr() != NULL);
#if 0
               printf ("At TOP: ninfo2.SkipClassDefinition() = %s \n",(ninfo_for_type.SkipClassDefinition() == true) ? "true" : "false");
#endif
            // if (vardecl_stmt->skipElaborateType())
               if ( (vardecl_stmt->skipElaborateType()) && (declStmt != NULL) && (isSgTypedefDeclaration(declStmt) == NULL) )
                  {
                 // ninfo2.set_SkipClassSpecifier();
                    ninfo_for_type.set_SkipClassSpecifier();
                  }

            // DQ (2/2/2014): Adding support to supress the output of the array bound in secondary array typed variable declarations.
               if (decl_item->get_hasArrayTypeWithEmptyBracketSyntax() == true)
                  {
                    ninfo_for_type.set_supressArrayBound();
#if 0
                    printf ("In unparseVarDeclStmt(): decl_item->get_hasArrayTypeWithEmptyBracketSyntax() == true: calling ninfo_for_type.set_supressArrayBound() \n");
#endif
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }

            // printf ("After isNameOnly() test: ninfo2.SkipClassDefinition() = %s \n",(ninfo2.SkipClassDefinition() == true) ? "true" : "false");

            // DQ (8/23/2006): This was used to specify global qualification separately from the more general name 
            // qualification mechanism, however having two mechanisms is a silly level of redundancy so we now use 
            // just one (the more general one) even though it is only used for global name qualification.
            // DQ (8/20/2006): We can't mark the SgType (since it is shared), and we can't mark the SgInitializedName,
            // so we have to carry the information that we should mark the type in the SgVariableDeclaration.
#if DEBUG_VARIABLE_DECLARATION
               printf ("vardecl_stmt->get_requiresGlobalNameQualificationOnType() = %s \n",vardecl_stmt->get_requiresGlobalNameQualificationOnType() ? "true" : "false");
#endif
            // ROSE_ASSERT(vardecl_stmt->get_requiresGlobalNameQualificationOnType() == true);
               if (vardecl_stmt->get_requiresGlobalNameQualificationOnType() == true)
                  {
                 // Output the name qualification for the type in the variable declaration.
                 // But we have to do so after any modifiers are output, so in unp->u_type->unparseType().
                 // printf ("In Unparse_ExprStmt::unparseVarDeclStmt(): This variable declaration requires a global qualifier \n");
                 // ninfo2.set_forceQualifiedNames();
                    ninfo_for_type.set_requiresGlobalNameQualification();
                  }

            // DQ (5/21/2011): Set the reference node for the qualified name lookup.
               ninfo_for_type.set_reference_node_for_qualification(decl_item);
               ROSE_ASSERT(ninfo_for_type.get_reference_node_for_qualification() != NULL);

            // ninfo2.set_isTypeFirstPart();
               ninfo_for_type.set_isTypeFirstPart();
#if 0
               printf ("ninfo2.SkipClassDefinition() = %s \n",(ninfo_for_type.SkipClassDefinition() == true) ? "true" : "false");
               curprint("\n/* START: output using unp->u_type->unparseType (1st part) */ \n");
               printf ("Calling 1st part of unp->u_type->unparseType for %s \n",tmp_type->class_name().c_str());
#endif
            // ROSE_ASSERT(ninfo2.get_declstatement_ptr() != NULL);
               ROSE_ASSERT(ninfo_for_type.get_declstatement_ptr() != NULL);

            // DQ (5/13/2011): Added support for newer name qualification implementation.
            // printf ("Inside of unparseVarDeclStmt: decl_item->get_name_qualification_length_for_type() = %d \n",decl_item->get_name_qualification_length_for_type());
               ninfo_for_type.set_name_qualification_length(decl_item->get_name_qualification_length_for_type());
               ninfo_for_type.set_global_qualification_required(decl_item->get_global_qualification_required_for_type());
               ninfo_for_type.set_type_elaboration_required(decl_item->get_type_elaboration_required_for_type());
#if 0
               printf ("Inside of unparseVarDeclStmt: calling unparseType() tmp_type = %p = %s \n",tmp_type,tmp_type->class_name().c_str());
#endif
            // unp->u_type->unparseType(tmp_type, ninfo2);
               ROSE_ASSERT(isSgType(tmp_type) != NULL);
#if 1
            // DQ (7/28/2012): This is similar to code in the variable declaration unpaser function and so might be refactored.
            // DQ (7/28/2012): If this is a declaration associated with a declaration list from a previous (the last statement) typedef
            // then output the name if that declaration had an un-named type (class or enum).
#if 0
               printf ("In unparseVarDeclStmt(): vardecl_stmt->get_isAssociatedWithDeclarationList() = %s \n",vardecl_stmt->get_isAssociatedWithDeclarationList() ? "true" : "false");
#endif
#if 0
               ninfo_for_type.display ("At top of Unparse_ExprStmt::unparseVarDeclStmt: ninfo_for_type");
#endif
#if 0
               printf ("Inside of unparseVarDeclStmt(): ninfo_for_type.SkipBaseType() = %s \n",ninfo_for_type.SkipBaseType() ? "true" : "false");
#endif
#if 0
               curprint ("/* In unparseVarDeclStmt(): vardecl_stmt->get_isAssociatedWithDeclarationList() = " + string(vardecl_stmt->get_isAssociatedWithDeclarationList() ? "true" : "false") + " */ \n");
               curprint ("/* In unparseVarDeclStmt(): ninfo_for_type.SkipBaseType() = " + string(ninfo_for_type.SkipBaseType() ? "true" : "false") + " */ \n");
#endif
               if (vardecl_stmt->get_isAssociatedWithDeclarationList() == true)
                  {
#if 1
#if 0
                    printf ("Using ninfo_for_type.set_PrintName(); to support unparsing of type \n");
#endif
                 // This is an alternative to permit the unparsing of the type to control the name output for types.
                 // But it would have to be uniform that all the pieces of the first part of the type would have to 
                 // be output.  E.g. "*" in "*X".
                    ninfo_for_type.set_PrintName();
#if 0

#error "DEAD CODE!"
                    unp->u_type->unparseType(tmp_type, ninfo_for_type);
#else
                    if (ninfo_for_type.SkipBaseType() == false)
                       {
#if 0
                         printf ("In unparseVarDeclStmt(): calling unparseType() \n");
                         curprint("\n/* In unparseVarDeclStmt(): calling unparseType() */ \n");
#endif
                         unp->u_type->unparseType(tmp_type, ninfo_for_type);
#if 0
                         printf ("In unparseVarDeclStmt(): DONE: calling unparseType() \n");
                         curprint("\n/* In unparseVarDeclStmt(): DONE: calling unparseType() */ \n");
#endif
                       }
                      else
                       {
                      // DQ (8/20/2014): We need to check if this is a pointer or reference (or rvalue reference?).
#if 0
                         printf ("In unparseVarDeclStmt(): isAssociatedWithDeclarationList: Note that (ninfo_for_type.SkipBaseType() == true) so type was not unparsed (but we need to handle pointers) \n");
#endif
                         SgPointerType* pointerType = isSgPointerType(tmp_type);
                         if (pointerType != NULL)
                            {
                              curprint(" *");

                              SgPointerType* nested_pointerType = isSgPointerType(pointerType->get_base_type());
                              if (nested_pointerType != NULL)
                                 {
                                   curprint(" *");
                                   printf ("Maybe we need to check the base_type to see if this is a chain of pointers. \n");
                                 }
                            }
                       }
#endif
#else
                 // Get the base type and if it is a class or enum, is it associated with a un-named 
                 // declaration (if so then we need to output the name in this associated declaration).
                 // if (isSgNamedType(tmp_type) != NULL)
#error "DEAD CODE!"
                    SgType* baseType = tmp_type->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_RVALUE_REFERENCE_TYPE|SgType::STRIP_POINTER_TYPE|SgType::STRIP_ARRAY_TYPE);
                    SgClassType* classType = isSgClassType(baseType);
                    SgEnumType*  enumType  = isSgEnumType(baseType);
                    if (classType != NULL || enumType != NULL)
                       {
                         if (classType != NULL)
                            {
                              SgClassDeclaration *decl = isSgClassDeclaration(classType->get_declaration());
                              if (decl->get_isUnNamed() == false)
                                 {
                                   SgName nm = decl->get_name();
                                   curprint (nm + " ");
                                 }
                            }
#error "DEAD CODE!"
                         if (enumType != NULL)
                            {
                              SgEnumDeclaration *decl = isSgEnumDeclaration(enumType->get_declaration());
                              if (decl->get_isUnNamed() == true)
                                 {
                                   SgName nm = decl->get_name();
                                   curprint (nm + " ");
                                 }
                            }
                       }
                      else
                       {
                      // If this is not a class or enum type then output the type as we would otherwise.
                         unp->u_type->unparseType(tmp_type, ninfo_for_type);
                       }
#endif
                  }
                 else
                  {
                 // DQ (5/7/2013): Added this since it was not set.
                    ninfo.set_isTypeFirstPart();

                 // DQ (5/7/2013): This should be set.
                    ROSE_ASSERT(ninfo.isTypeFirstPart()  == true);
                    ROSE_ASSERT(ninfo.isTypeSecondPart() == false);
                    ROSE_ASSERT(ninfo_for_type.isTypeFirstPart()  == true);
                    ROSE_ASSERT(ninfo_for_type.isTypeSecondPart() == false);
#if 0
                    printf ("##### This works for test2013_156.C and not quite for test2013_158.C, but it should be using ninfo_for_type so that the name qualification info is passed properly! \n");
                    printf ("   --- tmp_type = %p = %s \n",tmp_type,tmp_type->class_name().c_str());
#endif
                 // DQ (5/7/2013): This is an attempt to fix test2013_156.C.
                 // DQ (7/28/2012): Output the type if this is not associated with a declaration list from a previous declaration.
                 // unp->u_type->unparseType(tmp_type, ninfo_for_type);
                 // unp->u_type->unparseType(tmp_type, ninfo);
                 // unp->u_type->unparseType(tmp_type, ninfo_for_type);
                    if (ninfo_for_type.SkipBaseType() == false)
                       {
                         if (decl_item->get_name_qualification_length() > 0)
                            {
#if 0
                              printf ("Set reference_node_for_qualification = %p name = %s \n",decl_item,decl_item->get_name().str());
#endif
                           // info.set_reference_node_for_qualification(decl_item);
                              ninfo_for_type.set_reference_node_for_qualification(decl_item);
                            }

#if 0
                         printf ("In unparseVarDeclStmt(): calling unparseType(): tmp_type = %p = %s \n",tmp_type,tmp_type->class_name().c_str());
                         curprint("\n/* In unparseVarDeclStmt(): first part: calling unparseType() */ \n");
#endif
                         unp->u_type->unparseType(tmp_type, ninfo_for_type);
#if 0
                         printf ("In unparseVarDeclStmt(): DONE: calling unparseType() \n");
                         curprint("\n/* In unparseVarDeclStmt(): DONE: first part: calling unparseType() */ \n");
#endif
                       }
                      else
                       {
                         printf ("In unparseVarDeclStmt(): Note that (ninfo_for_type.SkipBaseType() == true) so type was not unparsed \n");
                       }
                  }
#else

#error "DEAD CODE!"

               unp->u_type->unparseType(tmp_type, ninfo_for_type);
#endif

#if 0
               printf ("Inside of unparseVarDeclStmt: DONE calling unparseType() \n");
#endif
            // ROSE_ASSERT(ninfo2.get_declstatement_ptr() != NULL);
               ROSE_ASSERT(ninfo_for_type.get_declstatement_ptr() != NULL);
#if 0
               curprint("\n/* END: output using unp->u_type->unparseType (1st part) */ \n");
#endif
#if 0
               curprint("/* END: output using unp->u_type->unparseType (1st part) */");
#endif
#if 0
            // DQ (12/31/2013): review this in light of change to support type attributres directly.
               printf ("Inside of unparseVarDeclStmt: calling printAttributes(decl_item,info): Find out what what sorts of gnu attributes this causes to be put out! \n");
#endif
            // DQ (2/6/2014): Move this to be after the name is output.
            // DQ (8/31/2013): Added support for missing attributes.
            // unp->u_sage->printAttributes(decl_item,info);

            // DQ (12/30/2013): Adding support to seperate how packing is handled when attached to the type of a variable vs. the variable directly.
               if (!ninfo.inEnumDecl() && !ninfo.inArgList() && !ninfo.SkipSemiColon())
                  {
                 // unp->u_sage->printAttributes(vardecl_stmt,info);
                    unp->u_sage->printAttributesForType(vardecl_stmt,info);
                  }
#if 0
               printf ("tmp_name = %s \n",tmp_name.str());
#endif
            // DQ (11/28/2004): Added qualifier to variable name.

            // DQ (10/6/2004): Changed this back to the previous ordering so that we could handle test2004_104.C
            // DQ (9/28/2004): Output the variable name after the first and second parts of the type!
               if (tmp_name.is_null() == false)
                  {
                 // printf ("Before output --- Variable Name: tmp_name = %s \n",tmp_name.str());
                 // Only output the name qualifier if we are going to output the variable name!
                    ROSE_ASSERT(decl_item != NULL);
                 // SgInitializedName* decl_item_in_scope = decl_item->get_prev_decl_item();
                 // printf ("Should be valid pointer: decl_item->get_prev_decl_item() = %p \n",decl_item->get_prev_decl_item());

                 // DQ (11/28/2004): Find the scope of the current SgInitializedName object and get the list of 
                 // declarations in that scope.  Get the first declaration and use it as input to the unp->u_type->unp->u_name->generateNameQualifier()
                 // function (which requires a declaration).  We can't use the current SgVariableDeclaration we 
                 // are processing since it might be from a different scope!
                    ROSE_ASSERT(decl_item != NULL);
                    ROSE_ASSERT(decl_item->get_parent() != NULL);

                 // DQ (5/1/2005): Not clear if we can assert this!
                 // ROSE_ASSERT(decl_item->get_prev_decl_item() != NULL);
                    if (decl_item->get_prev_decl_item() != NULL)
                       {
                      // printf ("decl_item->get_prev_decl_item() = %p get_name() = %s \n",decl_item->get_prev_decl_item(),decl_item->get_prev_decl_item()->get_name().str());

                      // DQ (2/12/2011): Commented out to support generation of graph to debug test2011_08.C, this test codes 
                      // demonstrates that the SgInitializedName build first might only be to support a symbol and not have a
                      // proper parent.
                      // ROSE_ASSERT(decl_item->get_prev_decl_item()->get_parent() != NULL);
                         SgInitializedName* previousInitializedName = decl_item->get_prev_decl_item();

                      // DQ (6/5/2011): The initial SgInitializedName need not really be used, but it is present in the AST.
                      // We now reset the symbol to the SgInitializedName that is used in the declaration.
                      // So this test can be removed.
                         if (previousInitializedName->get_prev_decl_item() != NULL)
                              ROSE_ASSERT(previousInitializedName->get_parent() != NULL);
                       }

                 // DQ (1/22/2014): Added support for supressing generated names for un-named variables.
                 // bool isAnonymousName = (string(decl_item->get_name()).substr(0,14) == "__anonymous_0x") && (classdecl_stmt->get_class_type() == SgClassDeclaration::e_union);
                    bool isAnonymousName = (string(decl_item->get_name()).substr(0,14) == "__anonymous_0x");
                 // bool isAnonymousName = false;

#if DEBUG_VARIABLE_DECLARATION
                    printf ("In unparseVarDeclStmt(): isAnonymousName = %s \n",isAnonymousName ? "true" : "false");
#endif
                 // DQ (10/10/2006): Only do name qualification for C++
                 // if (SageInterface::is_Cxx_language() == true)
                    if (isAnonymousName == false && SageInterface::is_Cxx_language() == true)
                       {
                         SgUnparse_Info ninfo_for_variable(ninfo);
#if 0
                      // DQ (10/24/2007): Added fix by Jeremiah (not well tested)
                         if (classDefinition != NULL)
                            {
                           // JJW 10-23-2007
                           // Never qualify a member variable of a class
                              ninfo_for_variable.set_SkipQualifiedNames();
                            }
#endif
                      // DQ (1/5/2007): C++ can be more complex and we can be initializing a variable in global scope initially declared in a class or namespace.
                      // printf ("variable name = %s decl_item->get_prev_decl_item() = %p \n",tmp_name.str(),decl_item->get_prev_decl_item());

                      // SgScopeStatement* parentScope = decl_item->get_scope();
                      // ROSE_ASSERT(ninfo2.get_declstatement_ptr() != NULL);
                         ROSE_ASSERT(ninfo_for_variable.get_declstatement_ptr() != NULL);
#if 0
                         printf ("vardecl_stmt->get_name_qualification_length()     = %d \n",vardecl_stmt->get_name_qualification_length());
                         printf ("vardecl_stmt->get_global_qualification_required() = %s \n",vardecl_stmt->get_global_qualification_required() ? "true" : "false");
                         printf ("vardecl_stmt->get_type_elaboration_required()     = %s \n",vardecl_stmt->get_type_elaboration_required() ? "true" : "false");
#endif
#if 0
                      // DQ (3/31/2019): These are new variables just added to support the SgPointerMemberType.
                         printf ("decl_item->get_name_qualification_length()     = %d \n",decl_item->get_name_qualification_length());
                         printf ("decl_item->get_global_qualification_required() = %s \n",decl_item->get_global_qualification_required() ? "true" : "false");
                         printf ("decl_item->get_type_elaboration_required()     = %s \n",decl_item->get_type_elaboration_required() ? "true" : "false");
#endif
#if 0
                         printf ("decl_item->get_name_qualification_length_for_type()     = %d \n",decl_item->get_name_qualification_length_for_type());
                         printf ("decl_item->get_global_qualification_required_for_type() = %s \n",decl_item->get_global_qualification_required_for_type() ? "true" : "false");
                         printf ("decl_item->get_type_elaboration_required_for_type()     = %s \n",decl_item->get_type_elaboration_required_for_type() ? "true" : "false");
#endif
                      // DQ (5/12/2011): Added support for newer name qualification implementation.
                         ninfo_for_variable.set_name_qualification_length(vardecl_stmt->get_name_qualification_length());
                         ninfo_for_variable.set_global_qualification_required(vardecl_stmt->get_global_qualification_required());
                         ninfo_for_variable.set_type_elaboration_required(vardecl_stmt->get_type_elaboration_required());

                      // DQ (4/27/2019): We need to get the name qualification from the SgInitializedName, and not the SgVariableDeclaration.
                      // SgName nameQualifier = unp->u_type->unp->u_name->generateNameQualifier(decl_item,ninfo2);
                      // SgName nameQualifier = unp->u_name->generateNameQualifier(decl_item,ninfo_for_variable);
                      // SgName nameQualifier = vardecl_stmt->get_qualified_name_prefix();
                         SgName nameQualifier = decl_item->get_qualified_name_prefix();

#if DEBUG_VARIABLE_DECLARATION
                         printf ("variable declaration name = %s nameQualifier = %s \n",tmp_name.str(),(nameQualifier.is_null() == false) ? nameQualifier.str() : "NULL");
#endif
                         if (nameQualifier.is_null() == false)
                            {
                              curprint(nameQualifier.str());
                            }
                       }

#if DEBUG_VARIABLE_DECLARATION
                    printf ("Variable Name: tmp_name = %s \n",tmp_name.str());
#endif
                 // Output the name of the variable...
                 // curprint(tmp_name.str());
                    if (isAnonymousName == false)
                       {
#if 0
                      // DQ (5/26/2014): The fix to output a space in unparseTemplateArgumentList() when we have an empty template argument list means
                      // that we no longer require this space to be output here (which was a problem for more general non template test codes that
                      // were using diff against a gold standard for there tests (in the ROSE regression tests).
                      // DQ (5/17/2014): With fixes to the determination of template declarations and template instantiation declarations, test2005_163.C
                      // now requires  whitespace between the variable's type and the variable's name.
                         curprint(" ");
#endif
#if 0
                         curprint("\n/* output the name */ \n");
#endif
                         curprint(tmp_name.str());
#if 0
                         curprint("\n/* DONE: output the name */ \n");
#endif
                       }
#if 0
                 // DQ (4/20/2015): Moved the output of the asm declaration specified to after the 2nd part of
                 // the type has been output (required for variable declarations of type array).See test2015_105.c.

                 // DQ (7/25/2006): Support for asm register naming within variable declarations (should also be explicitly marked as "register")
                 // ROSE_ASSERT(decl_item->get_register_name() == 0);
                    if (decl_item->get_register_name_code() != SgInitializedName::e_invalid_register)
                       {
#if 0
                         printf ("In unparseVarDeclStmt(): Output asm register name code \n");
#endif
                      // an asm ("<register name>") is in use
                         curprint ( string(" asm (\""));
                      // curprint ( string("<unparse register name>";
                         curprint ( unparse_register_name(decl_item->get_register_name_code()));
                         curprint ( string("\")"));
                       }

                 // DQ (1/25/2009): If we are not using the Assembly Register codes then we might be using the string 
                 // mechanism (stored in SgInitializedName::p_register_name_string). The new EDG/Sage interface can
                 // support the use of either Assembly Register codes or raw strings.
                    if (decl_item->get_register_name_string().empty() == false)
                       {
#if 0
                         printf ("In unparseVarDeclStmt(): Output asm register name \n");
#endif
                      // an asm ("<register name>") is in use
                         curprint ( string(" asm (\""));
                         curprint ( decl_item->get_register_name_string() );
                         curprint ( string("\")"));
                       }
#endif
                  }
                 else
                  {
#if 0
                    printf ("In unparseVarDeclStmt(): (tmp_name.is_null() == true): variable Name: tmp_name = %s \n",tmp_name.str());
#endif
                  }

            // ninfo2.set_isTypeSecondPart();
               ninfo_for_type.set_isTypeSecondPart();

#if DEBUG_VARIABLE_DECLARATION
               curprint("\n/* START: output using unp->u_type->unparseType (2nd part) */ \n");
               printf ("Calling 2nd part of unp->u_type->unparseType for %s \n",tmp_type->sage_class_name());
               printf ("ninfo2.SkipClassDefinition() = %s \n",(ninfo_for_type.SkipClassDefinition() == true) ? "true" : "false");
#endif

            // DQ (5/7/2013): This should be set.
            // ROSE_ASSERT(ninfo.isTypeSecondPart() == true);
            // ROSE_ASSERT(ninfo.isTypeFirstPart() == false);
               ROSE_ASSERT(ninfo_for_type.isTypeSecondPart() == true);
               ROSE_ASSERT(ninfo_for_type.isTypeFirstPart() == false);
#if 0
               printf ("In unparseVarDeclStmt(): ninfo_for_type.SkipClassDefinition() = %s \n",(ninfo_for_type.SkipClassDefinition() == true) ? "true" : "false");
               printf ("In unparseVarDeclStmt(): ninfo_for_type.SkipEnumDefinition()  = %s \n",(ninfo_for_type.SkipEnumDefinition() == true) ? "true" : "false");
#endif
            // DQ (1/9/2014): These should have been setup to be the same.
               ROSE_ASSERT(ninfo_for_type.SkipClassDefinition() == ninfo_for_type.SkipEnumDefinition());

            // DQ (7/31/2006): I think that we can simplify to just this code.
            // unp->u_type->unparseType(tmp_type, ninfo2);
               unp->u_type->unparseType(tmp_type, ninfo_for_type);

#if DEBUG_VARIABLE_DECLARATION
               printf ("DONE: Calling 2nd part of unp->u_type->unparseType for %s \n",tmp_type->sage_class_name());
               curprint("\n/* END: output using unp->u_type->unparseType (2nd part) */ \n ");
#endif
#if 0
               curprint("/* END: output using unp->u_type->unparseType (2nd part) */");
#endif
            // DQ (4/20/2015): Moved the output of the asm declaration specified to after the 2nd part of
            // the type has been output (required for variable declarations of type array). See test2015_105.c.
               if (tmp_name.is_null() == false)
                  {
#if 0
                    printf ("After output type: --- Variable Name: tmp_name = %s (and second part of type) \n",tmp_name.str());
                    curprint("\n/* After output type: --- Variable Name */ \n");
#endif
                    ROSE_ASSERT(decl_item != NULL);

                 // DQ (7/25/2006): Support for asm register naming within variable declarations (should also be explicitly marked as "register")
                 // ROSE_ASSERT(decl_item->get_register_name() == 0);
                    if (decl_item->get_register_name_code() != SgInitializedName::e_invalid_register)
                       {
#if 0
                         printf ("In unparseVarDeclStmt(): Output asm register name code \n");
#endif
                      // an asm ("<register name>") is in use
#ifdef BACKEND_CXX_IS_INTEL_COMPILER
                      // DQ (2/4/2017): Added support for asm register names for Intel compiler (see test2015_105,c).
                         curprint ( string(" __asm__ (\""));
#else
                         curprint ( string(" asm (\""));
#endif
                      // curprint ( string("<unparse register name>";
                         curprint ( unparse_register_name(decl_item->get_register_name_code()));
                         curprint ( string("\")"));
                       }

                 // DQ (1/25/2009): If we are not using the Assembly Register codes then we might be using the string 
                 // mechanism (stored in SgInitializedName::p_register_name_string). The new EDG/Sage interface can
                 // support the use of either Assembly Register codes or raw strings.
                    if (decl_item->get_register_name_string().empty() == false)
                       {
#if 0
                         printf ("In unparseVarDeclStmt(): Output asm register name \n");
#endif
                      // an asm ("<register name>") is in use
#ifdef BACKEND_CXX_IS_INTEL_COMPILER
                      // DQ (2/4/2017): Added support for asm register names for Intel compiler (see test2015_105,c).
                         curprint ( string(" __asm__ (\""));
#else
                         curprint ( string(" asm (\""));
#endif
                         curprint ( decl_item->get_register_name_string() );
                         curprint ( string("\")"));
                       }
                  }
               
#if 0
               printf ("Calling printAttributes() \n");
               curprint("\n/* Calling printAttributes() */ \n ");
#endif
            // DQ (2/6/2014): Move this to be after the name is output.
            // DQ (8/31/2013): Added support for missing attributes.
               unp->u_sage->printAttributes(decl_item,info);

            // DQ (7/26/2014): comment out to avoid compiler warning.
            // Mark that we are no longer processing the first entry 
            // (first variable in a declaration containing multiple "," separated names)
            // first = false;

            // DQ (5/6/2013): Associated end of block for alternative handling of type in variable declaration.
            //    }
#if DEBUG_VARIABLE_DECLARATION
               printf ("In unparseVarDeclStmt(): Handle initializers (if any) \n");
               curprint("\n/* Handle initializers (if any) */ \n");
#endif
            // Unparse the initializers if any exist

#if 0
               if (tmp_init != NULL)
                  {
                    printf ("In unparseVarDeclStmt(): Initializer tmp_init = %p = %s \n",tmp_init,tmp_init->class_name().c_str());
                    printf (" --- decl_item->get_using_assignment_copy_constructor_syntax() = %s \n",decl_item->get_using_assignment_copy_constructor_syntax() ? "true" : "false");
#if 0
                    tmp_init->get_file_info()->display("Initializer tmp_init: debug");
#endif
                  }
#endif

#define DEBUG_COPY_INITIALIZER_SYNTAX 0

            // DQ (5/31/2019): We need additional control over which form of syntax is output for the copy constructor 
            // (the "A a = B" copy constructor syntax as opposed to the "A a(B)" syntax).
               bool output_using_assignment_copy_constructor_syntax = ( (tmp_init != NULL) && (decl_item->get_using_assignment_copy_constructor_syntax() == true) );

#if DEBUG_COPY_INITIALIZER_SYNTAX
            // DQ (5/31/2019): Check for brace initialization.
               printf ("decl_item->get_is_braced_initialized() = %s \n",decl_item->get_is_braced_initialized() ? "true" : "false");
#endif

            // DQ (7/23/2013): Added better control over when to output the initializer.
               bool outputInitializerBasedOnSourcePositionInfo = 
                    ( (tmp_init != NULL) &&
                      ( (tmp_init->get_file_info()->isCompilerGenerated() == false) || 
                        (tmp_init->get_file_info()->isCompilerGenerated() == true && tmp_init->get_file_info()->isOutputInCodeGeneration() == true) ) );

#if DEBUG_COPY_INITIALIZER_SYNTAX
               printf ("tmp_init                                        = %p \n",tmp_init);
               if (tmp_init != NULL)
                  {
                    printf ("tmp_init                                        = %p = %s \n",tmp_init,tmp_init->class_name().c_str());
                  }
               printf ("ninfo.SkipInitializer()                         = %s \n",ninfo.SkipInitializer() ? "true" : "false");
               printf ("outputInitializerBasedOnSourcePositionInfo      = %s \n",outputInitializerBasedOnSourcePositionInfo ? "true" : "false");
               printf ("output_using_assignment_copy_constructor_syntax = %s \n",output_using_assignment_copy_constructor_syntax ? "true" : "false");
#endif
            // DQ (7/23/2013): Modified back to a previously implemented case of checking (tmp_init->get_file_info()->isOutputInCodeGeneration() == true).
            //     See test2013_250.C for an example.
            // DQ (3/29/2013): Don't output the initializer if it was compiler generated and not meant to be output (see test2013_78.C).
            // This fails for test2007_06.C, so output the initializer if it is available in the AST.
            // if (tmp_init)
            // if ( (tmp_init != NULL) && !ninfo.SkipInitializer())
            // if ( (tmp_init != NULL) && !ninfo.SkipInitializer() && (tmp_init->get_file_info()->isOutputInCodeGeneration() == true))
               if ( (tmp_init != NULL) && !ninfo.SkipInitializer() && (outputInitializerBasedOnSourcePositionInfo == true))
                  {
                 // DQ (8/5/2005): generate more faithful representation of assignment operator!
#if 0
                    ninfo.display ("In Unparse_ExprStmt::unparseVarDeclStmt --- handling the initializer");
#endif
                    SgConstructorInitializer* constructor = isSgConstructorInitializer(tmp_init);
#if DEBUG_COPY_INITIALIZER_SYNTAX
                 // DQ (1/16/2019): Added debug information.
                    printf ("In Unparse_ExprStmt::unparseVarDeclStmt(): constructor = %p \n",constructor);
                    if (constructor != NULL)
                      {
                        printf ("In Unparse_ExprStmt::unparseVarDeclStmt(): output_using_assignment_copy_constructor_syntax = %s \n",output_using_assignment_copy_constructor_syntax ? "true" : "false");
                        printf ("In Unparse_ExprStmt::unparseVarDeclStmt(): constructor->get_need_name()                    = %s \n",constructor->get_need_name() ? "true" : "false");
                        printf ("In Unparse_ExprStmt::unparseVarDeclStmt(): constructor->get_associated_class_unknown()     = %s \n",constructor->get_associated_class_unknown() ? "true" : "false");
                        printf ("In Unparse_ExprStmt::unparseVarDeclStmt(): ninfo.inConditional()                           = %s \n",ninfo.inConditional() ? "true" : "false");
                      }
#endif
                    if ( (tmp_init->variant() == ASSIGN_INIT) ||
                         (tmp_init->variant() == AGGREGATE_INIT) ||
                      // ( (constructor != NULL) && constructor->get_need_name() && constructor->get_is_explicit_cast() ) )
                      // DQ (7/12/2006): Bug fix reported by Peter Collingbourne
                      // ( (constructor != NULL) && (constructor->get_need_name() || constructor->get_associated_class_unknown()) ) )

                      // DQ (11/9/2009): Turn off this optional handling since "IntStack::Iter z = (&x)" is not interpreted 
                      // the same as "IntStack::Iter z(&x)" at least in a SgForInitializationStatement (see test2009_40.C).
                      // I think that this was always cosmetic anyway.
                      // ( (constructor != NULL) && (constructor->get_need_name() || constructor->get_associated_class_unknown() || ninfo.inConditional()) ) )
                      // ( false ) )
                      // ( (constructor != NULL) && (constructor->get_need_name() || constructor->get_associated_class_unknown() || ninfo.inConditional()) ) )
                      // ( (constructor != NULL) && (constructor->get_need_name() || constructor->get_associated_class_unknown() || ninfo.inConditional()) ) )
                         ( (constructor != NULL) && (constructor->get_need_name() || constructor->get_associated_class_unknown() || ninfo.inConditional() || output_using_assignment_copy_constructor_syntax == true) ) )
                       {
                      // DQ (11/9/2009): Skip the case of when we are in a isSgForInitStmt, since this is a bug in GNU g++ (at least version 4.2)
                      // See test2009_40.C test2009_41.C, and test2009_42.C
                      // curprint ( string(" = "));
#if DEBUG_COPY_INITIALIZER_SYNTAX
                         printf ("Found SgConstructorInitializer: check if we want to use the assignment form of syntax for the constructor call \n");
#endif
                         if ( constructor != NULL && isSgForInitStatement(stmt->get_parent()) != NULL )
                            {
#if DEBUG_COPY_INITIALIZER_SYNTAX
                              printf ("This is the special case of a constructor call \n");
#endif
                           // DQ (2/9/2010): Previous code had this commented out to fix test2009_40.C.
                           // curprint (" = ");

                           // DQ (2/9/2010): See test2010_05.C
                              if (constructor->get_need_name() == true && constructor->get_is_explicit_cast() == true )
                                 {
                                // This is the syntax: class X = X(arg)
#if DEBUG_COPY_INITIALIZER_SYNTAX
                                   printf ("Output the = syntax \n");
#endif
                                   curprint (" = ");
                                 }
                                else
                                 {
                                // This is the alternative syntax: class X(arg)
                                // So don't output a "="
#if DEBUG_COPY_INITIALIZER_SYNTAX
                                   printf ("Skip output of the = syntax \n");
#endif
                                 }
                            }
                           else
                            {
#if DEBUG_COPY_INITIALIZER_SYNTAX
                              printf ("This is the MORE general case of a constructor call \n");
#endif
                           // DQ (1/16/2019): only use the "=" syntax if we are going to output the name.
                           // curprint (" = ");

#if DEBUG_COPY_INITIALIZER_SYNTAX
                              if (constructor != NULL)
                                 {
                                   printf ("constructor                         = %p \n",constructor);
                                   printf ("constructor->get_need_name()        = %s \n",constructor->get_need_name() ? "true" : "false");
                                   printf ("constructor->get_is_explicit_cast() = %s \n",constructor->get_is_explicit_cast() ? "true" : "false");
                                 }
                                else
                                 {
                                   printf ("constructor == NULL \n");
                                 }
#endif
                           // if ( (constructor != NULL) && (constructor->get_need_name() == true) && (constructor->get_is_explicit_cast() == true) )
                              if ( (constructor != NULL) && ( ( (constructor->get_need_name() == true) && (constructor->get_is_explicit_cast() == true) ) || (output_using_assignment_copy_constructor_syntax == true) ) )
                                 {
                                // This is the syntax: class X = X(arg)
#if DEBUG_COPY_INITIALIZER_SYNTAX
                                   printf ("Output the = syntax \n");
#endif

                                // DQ (5/31/2019): We need to supress the "=" if there will be no output from the unparsing of the initializer.
                                // curprint (" = ");
                                   ROSE_ASSERT(constructor != NULL);
                                // bool suppressAssignmentSyntax = (constructor->get_args()->get_expressions().size() == 0);
                                   bool suppressAssignmentSyntax = (constructor->get_args()->get_expressions().size() == 0 && (constructor->get_is_explicit_cast() == false) );

#if DEBUG_COPY_INITIALIZER_SYNTAX
                                   printf ("constructor->get_is_braced_initialized() = %s \n",constructor->get_is_braced_initialized() ? "true" : "false");
#endif
                                // DQ (5/31/2019): This effects only Cxx11_tests/test2012_25.C
                                   if (constructor->get_is_braced_initialized() == true)
                                      {
#if DEBUG_COPY_INITIALIZER_SYNTAX
                                        printf ("Set suppressAssignmentSyntax = false because this is braced initialized \n");
#endif
                                        suppressAssignmentSyntax = true;
                                      }

#if DEBUG_COPY_INITIALIZER_SYNTAX
                                   printf ("suppressAssignmentSyntax = %s \n",suppressAssignmentSyntax ? "true" : "false");
#endif
                                   if (suppressAssignmentSyntax == false)
                                      {
                                        curprint (" = ");
                                      }
                                 }
                                else
                                 {
                                // DQ (1/16/2019): Output the equals operator in the case of a assignment or aggregate initializer.
                                   if ( (tmp_init->variant() == ASSIGN_INIT) || (tmp_init->variant() == AGGREGATE_INIT) )
                                // if ( (tmp_init->variant() == ASSIGN_INIT) || (tmp_init->variant() == AGGREGATE_INIT) || (output_using_assignment_copy_constructor_syntax == true) )
                                      {
#if DEBUG_COPY_INITIALIZER_SYNTAX
                                        printf ("Output the = syntax in the case of a assignment or aggregate initializer \n");
#endif
                                        curprint (" = ");
                                      }
                                     else
                                      {
                                     // This is the alternative syntax: class X(arg)
                                     // So don't output a "="
#if DEBUG_COPY_INITIALIZER_SYNTAX
                                        printf ("Skip output of the = syntax \n");
#endif
                                      }
                                 }
                            }
                       }
                      else
                       {
#if DEBUG_COPY_INITIALIZER_SYNTAX
                         printf ("This is the MOST general case of not using an initializer \n");
#endif
                       }

                    SgUnparse_Info statementInfo(ninfo);
                    statementInfo.set_SkipClassDefinition();

                 // DQ (1/9/2014): We have to make the handling of enum definitions consistant with that of class definitions.
                    statementInfo.set_SkipEnumDefinition();

                 // DQ (1/7/2007): Unset the declstatement_ptr so that we can know that we 
                 // are processing initalizers which might require name qualification!
                    statementInfo.set_declstatement_ptr(NULL);

                 // statementInfo.display("Debugging the initializer (set SkipClassDefinition");

                 // DQ (2/26/2012): Added test.
                    ROSE_ASSERT(tmp_init != NULL);
#if DEBUG_COPY_INITIALIZER_SYNTAX
                    printf ("Unparse the initializer = %p = %s \n",tmp_init,tmp_init->class_name().c_str());
#endif
                 // DQ (5/26/2013): Added support for name qualification.
                    statementInfo.set_reference_node_for_qualification(tmp_init);
                    ROSE_ASSERT(statementInfo.get_reference_node_for_qualification() != NULL);
#if DEBUG_COPY_INITIALIZER_SYNTAX
                    printf ("In unparseVarDeclStmt(): statementInfo.SkipClassDefinition() = %s \n",(statementInfo.SkipClassDefinition() == true) ? "true" : "false");
                    printf ("In unparseVarDeclStmt(): statementInfo.SkipEnumDefinition()  = %s \n",(statementInfo.SkipEnumDefinition() == true) ? "true" : "false");
#endif
                 // DQ (1/9/2014): These should have been setup to be the same.
                    ROSE_ASSERT(statementInfo.SkipClassDefinition() == statementInfo.SkipEnumDefinition());

                 // curprint (" /* Unparse the initializer */ ");
                 // unparseExpression(tmp_init, ninfo);
                    unparseExpression(tmp_init, statementInfo);

#if DEBUG_COPY_INITIALIZER_SYNTAX
                    printf ("DONE: Unparse the initializer \n");
                    curprint (" /* DONE: Unparse the initializer */ ");
#endif
                  }
             }
            else
             {
            // DQ (7/16/2012): Added new support for multiple variables in the same declaration, output the name of the variable...
               tmp_name = decl_item->get_name();
               curprint(tmp_name.str());
             }

       // DQ (8/31/2013): I think this is the wrong location for the attribute (see test2013_40.c).
       // DQ (2/27/2013): Added support for missing attributes.
       // unp->u_sage->printAttributes(decl_item,info);

#if 0
          curprint("\n /* Inside of unparseVarDeclStmt(): increment the variable iterator */ \n");
#endif
       // Increment the iterator through the list of variables within a single variable declaration.
       // Currently each variable declaration contains only a single variable!
          p++;

       // DQ (7/16/2012): In the newer version of EDG 4.3 support this is no longer always true.
       // DQ (11/28/2004): Within the current design this is always true. Since we normalize 
       // multiple variable declarations into single variable declarations.
       // ROSE_ASSERT (p == vardecl_stmt->get_variables().end());

       // DQ (11/28/2004): This this is always false within the current design.
          if (p != vardecl_stmt->get_variables().end())
             {
               if (!ninfo.inArgList())
                    ninfo.set_SkipBaseType();
               curprint ( string(","));
             }

          unparseAttachedPreprocessingInfo(decl_item, ninfo, PreprocessingInfo::after);    
        }

#if 0
     curprint ("\n/* Handle bit fields specifiers (if any) */ \n");
#endif

  // DQ (11/28/2004): Bit fields specifiers should be associated with the SgInitializedName 
  // and not the SgVariableDeclaration!  However this works because variable declarations 
  // with multiple variables within a single declaration are separated out as single variable 
  // declarations (so including the bitfield width with the variable declaration works).  
  // If we ever permit unparsing of multiple variables within a single variable declaration 
  // then this would be a problem within the current design.

  // Bit fields appear as "a_bit_field autonomous_tag_decl:1;"
     SgVariableDefinition *defn = vardecl_stmt->get_definition();
     if (defn != NULL)
        {
       // DQ (2/3/2007): Implement this unparse functions so that we can refactor this code
       // and so that the unparseToString will work properly for the SgVariableDefinition IR node.
          unparseVarDefnStmt(vardecl_stmt->get_definition(),ninfo);
        }

  // ninfo.display("Close off the statement with a \";\"?");

  // curprint ( string("\n/* START: Close off the statement with a \";\" */ \n";

  // Close off the statement with a ";"
  // DQ (10/18/2012): In the case of a condirional the point of skipping the ";" is to permit multiple declarations, 
  // this should be handled at a different level (if possible).  This change is required for test2012_47.c.
  // This breaks other test codes, but the grainularity of the specification of output of ";" and where the 
  // set_SkipSemiColon() function is called is now more precise.  So the other codes now pass.
  // DQ (7/12/2006): Bug fix reported by Peter Collingbourne
  // if (!ninfo.inEnumDecl() && !ninfo.inArgList() && !ninfo.SkipSemiColon())
  // if (!ninfo.inEnumDecl() && !ninfo.inArgList() && !ninfo.inConditional() && !ninfo.SkipSemiColon())
     if (!ninfo.inEnumDecl() && !ninfo.inArgList() && !ninfo.SkipSemiColon())
        {
       // DQ (2/27/2013): Added support for missing attributes.
          unp->u_sage->printAttributes(vardecl_stmt,info);

          curprint(";");
        }
#if 0
     curprint("/* END: Close off the statement with a \";\" */");
#endif
#if 0
     curprint("/* aaaa */");
#endif
#if 0
     printf ("Leaving unparseVarDeclStmt() \n");
     curprint("/* Leaving unparseVarDeclStmt() */ \n");
#endif
   }


void
Unparse_ExprStmt::unparseVarDefnStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgVariableDefinition* vardefn_stmt = isSgVariableDefinition(stmt);
     ROSE_ASSERT(vardefn_stmt != NULL);

  // DQ: (9/17/2003)
  // Although I have not seen it in any of our tests of ROSE the SgVariableDefinition
  // does appear to be used in the declaration of bit fields!  Note the comment at the
  // end of the unparseVarDeclStmt() function where the bit field is unparsed! Though 
  // it appears that the unparseVarDefnStmt is not required to the unparsing of the 
  // bit field, so this function is never called!

  // DQ (2/3/2007): However, for the ODR check in the AST merge we require something to
  // be generated for everything that could be shared.  So we should unparse something,
  // perhaps the variable declaration?

  // DQ (1/20/2014): This has been changed to be a SgValueExp (required).  Plus as a 
  // generated value expression we include the expression from which the value was 
  // generated.  This is important where this is a constant expression generated from
  // sizes of machine dependent types.
  // SgUnsignedLongVal *bitfield = vardefn_stmt->get_bitfield();
     SgExpression* bitfield = vardefn_stmt->get_bitfield();
     if (bitfield != NULL)
        {
          curprint ( string(":"));
          unparseExpression(bitfield, info);
        }
   }


void
Unparse_ExprStmt::initializeDeclarationsFromParent ( 
   SgDeclarationStatement* declarationStatement,
   SgClassDefinition* & cdefn,
   SgNamespaceDefinitionStatement* & namespaceDefn, int debugSupport )
   {
  // DQ (11/18/2004): Now that we store the scope explicitly we don't have to interprete the parent pointer!
     ROSE_ASSERT(declarationStatement != NULL);
     SgScopeStatement* parentScope = declarationStatement->get_scope();
     ROSE_ASSERT(parentScope != NULL);

#if 0
     if (debugSupport > 0)
          printf ("In initializeDeclarationsFromParent(): parentScope = %p = %s \n",parentScope,parentScope->sage_class_name());
#endif

     cdefn         = isSgClassDefinition(parentScope);
     namespaceDefn = isSgNamespaceDefinitionStatement(parentScope);      

#if 0
     if (debugSupport > 0)
          printf ("In initializeDeclarationsFromParent(): cdefn = %p namespaceDefn = %p \n",cdefn,namespaceDefn);
#endif
   }



void
Unparse_ExprStmt::unparseClassDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgClassDeclaration* classdecl_stmt = isSgClassDeclaration(stmt);
     ROSE_ASSERT(classdecl_stmt != NULL);

#if 0
  // printf ("Inside of Unparse_ExprStmt::unparseClassDeclStmt(): classdecl_stmt->get_from_template() = %s \n",
  //      classdecl_stmt->get_from_template() ? "true" : "false");
     if (classdecl_stmt->get_from_template() == true)
          curprint ( string("/* Unparser comment: Templated Class Declaration Function */"));
     Sg_File_Info* classDeclarationfileInfo = classdecl_stmt->get_file_info();
     ROSE_ASSERT ( classDeclarationfileInfo != NULL );
     if ( classDeclarationfileInfo->isCompilerGenerated() == false)
        {
       // curprint(string("\n/* file: ") + classDeclarationfileInfo->get_filenameString() + " line: " classDeclarationfileInfo->get_line()  " col: " classDeclarationfileInfo->get_col() " */ \n");
          curprint(string("\n/* file: ") + classDeclarationfileInfo->get_filenameString() + " */ \n");
        }
#endif

  // info.display("Inside of unparseClassDeclStmt");

#if 0
     printf ("At top of unparseClassDeclStmt name = %s \n",classdecl_stmt->get_name().str());
#endif
#if 0
     printf ("In Unparse_ExprStmt::unparseClassDeclStmt(): classdecl_stmt = %p isForward() = %s info.SkipClassDefinition() = %s name = %s \n",
          classdecl_stmt,(classdecl_stmt->isForward() == true) ? "true" : "false",
          (info.SkipClassDefinition() == true) ? "true" : "false",classdecl_stmt->get_name().str());
#endif

  // DQ (11/7/2007): Fixup the SgUnparse_Info object to store the correct scope.
     SgUnparse_Info class_info(info);
     fixupScopeInUnparseInfo (class_info,classdecl_stmt);

     if (!classdecl_stmt->isForward() && classdecl_stmt->get_definition() && !info.SkipClassDefinition())
        {
          SgUnparse_Info ninfox(class_info);
#if 0
          printf ("In unparseClassDeclStmt(): calling ninfox.unset_SkipSemiColon() \n");
#endif
          ninfox.unset_SkipSemiColon();

       // DQ (6/13/2007): Set to null before resetting to non-null value 
          ninfox.set_declstatement_ptr(NULL);
          ninfox.set_declstatement_ptr(classdecl_stmt);

       // printf ("Calling unparseStatement(classdecl_stmt->get_definition(), ninfox); for %s \n",classdecl_stmt->get_name().str());
          unparseStatement(classdecl_stmt->get_definition(), ninfox);

          if (!info.SkipSemiColon())
             {
               curprint(";");
             }
        }
       else
        {
#if 0
          printf ("In unparseClassDeclStmt(): Not unparsing the class definition \n");
#endif
          if (!info.inEmbeddedDecl())
             {
               SgUnparse_Info ninfo(class_info);
               if (classdecl_stmt->get_parent() == NULL)
                  {
                    printf ("classdecl_stmt->isForward() = %s \n",(classdecl_stmt->isForward() == true) ? "true" : "false");
                  }

            // DQ (5/20/2006): This is false within "stdio.h"
               if (classdecl_stmt->get_parent() == NULL)
                  {
                    classdecl_stmt->get_file_info()->display("In Unparse_ExprStmt::unparseClassDeclStmt(): classdecl_stmt->get_parent() == NULL");
                  }
            // ROSE_ASSERT (classdecl_stmt->get_parent() != NULL);
               SgClassDefinition *cdefn = isSgClassDefinition(classdecl_stmt->get_parent());

               if(cdefn && cdefn->get_declaration()->get_class_type() == SgClassDeclaration::e_class)
                  {
                    ninfo.set_CheckAccess();
                  }

            // DQ (8/19/2004): Removed functions using old attribute mechanism (old CC++ mechanism)
            // printf ("Commented out get_suppress_global(classdecl_stmt) \n");
            // if (get_suppress_global(classdecl_stmt))
            //      ninfo.set_SkipGlobal(); //attributes.h
#if 0
               printf ("In unparseClassDeclStmt(): Calling printSpecifier() \n");
#endif
            // printDebugInfo("entering unp->u_sage->printSpecifier", true);
               unp->u_sage->printSpecifier(classdecl_stmt, ninfo);
               info.set_access_attribute(ninfo.get_access_attribute());
             }

          info.unset_inEmbeddedDecl();
#if 0
          printf ("In unparseClassDeclStmt(): info.SkipClassSpecifier() = %s \n",info.SkipClassSpecifier() ? "true" : "false");
#endif
          if (!info.SkipClassSpecifier())
             {
#if 0
               printf ("Processing the class specifier \n");
               curprint ("\n/* Processing the class specifier */ \n ");
#endif
               switch (classdecl_stmt->get_class_type()) 
                  {
                    case SgClassDeclaration::e_class : 
                       {
                         curprint("class ");
                         break;
                       }
                    case SgClassDeclaration::e_struct :
                       {
                         curprint("struct ");
                         break;
                       }
                    case SgClassDeclaration::e_union :
                       {
                         curprint("union ");
                         break;
                       }

                 // DQ (4/17/2007): Added this enum value to the switch cases.
                    case SgClassDeclaration::e_template_parameter :
                       {
                      // skip type elaboration here.
                         curprint(" ");
                         break;
                       }

                 // DQ (4/17/2007): Added this enum value to the switch cases.
                    default:
                       {
                         printf ("Error: default reached in unparseClassDeclStmt() \n");
                         ROSE_ASSERT(false);
                         break;
                       }
                  }
              }

       /* have to make sure if it needs qualifier or not */

          SgName nm = classdecl_stmt->get_name();

       // DQ (8/19/2014): Adding code to output the template instantiation with template arguments processed to support name qualification.
          SgTemplateInstantiationDecl* templateInstantiation = isSgTemplateInstantiationDecl(classdecl_stmt);
          if (templateInstantiation != NULL)
             {
#if 0
               printf ("In unparseClassDeclStmt(): name used must be reset to exclude template arguments (since they must be output seperately): nm = %s \n",nm.str());
               printf ("   --- templateInstantiation->get_templateName() = %s \n",templateInstantiation->get_templateName().str());
               printf ("   --- templateInstantiation->get_name()         = %s \n",templateInstantiation->get_name().str());
#endif
               nm = templateInstantiation->get_name();
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }

#if 0
          if (nm == "Zone")
             {
               printf ("In unparseClassDeclStmt() class Zone: unp->u_type->unp->u_name->generateNameQualifier( cdecl , info ) = %s \n",
                    unp->u_type->unp->u_name->generateNameQualifier( classdecl_stmt , info ).str());
             }
#endif

#if 0
       // DQ (10/11/2006): Don't generate qualified names for the class name a forward declaration
       // curprint ( string("/* In unparseClassDeclStmt: (skip qualified name if true) classdecl_stmt->get_forward() = " + ((classdecl_stmt->get_forward() == true) ? "true" : "false") + " */\n ";
          if (classdecl_stmt->get_forward() == false)
             {
            // ninfo.set_SkipQualifiedNames();

            // SgName nameQualifier = unp->u_name->generateNameQualifier( classdecl_stmt , info );
               SgName nameQualifier = unp->u_name->generateNameQualifier( classdecl_stmt , class_info );
            // printf ("In unparseClassDeclStmt() nameQualifier (from unp->u_type->unp->u_name->generateNameQualifier function) = %s \n",nameQualifier.str());

               curprint(nameQualifier.str());
             }
#endif
       // DQ (7/20/2011): Test compilation without these functions.

#if 1
       // DQ (7/28/2012): This is the original code (I think it is what we really want, but we need to test this.
       // DQ (6/5/2011): Newest refactored support for name qualification.
          SgName nameQualifier = classdecl_stmt->get_qualified_name_prefix();
#if 0
          printf ("In unparseClassDeclStmt(): Output SgClassDeclaration = %p = %s qualified name: nameQualifier = %s \n",classdecl_stmt,classdecl_stmt->get_name().str(),nameQualifier.str());
#endif
#if 0
          printf ("In unparseClassDeclStmt(): classdecl_stmt->get_isUnNamed() = %s \n",classdecl_stmt->get_isUnNamed() ? "true" : "false");
          printf ("In unparseClassDeclStmt(): Output className = %s \n",classdecl_stmt->get_name().str());
#endif

       // DQ (6/9/2013): Further restrict this to the special case of un-named unions.
       // bool isAnonymousName = (string(classdecl_stmt->get_name()).substr(0,14) == "__anonymous_0x") != string::npos);
       // bool isAnonymousName = (string(classdecl_stmt->get_name()).substr(0,14) == "__anonymous_0x");
          bool isAnonymousName = (string(classdecl_stmt->get_name()).substr(0,14) == "__anonymous_0x") && (classdecl_stmt->get_class_type() == SgClassDeclaration::e_union);
#if 0
          printf ("In unparseClassDeclStmt(): isAnonymousName = %s \n",isAnonymousName ? "true" : "false");
#endif
#if 0
       // DQ (8/19/2014): Original code.
       // DQ (6/9/2013): Skip output of name when it is a generated name such as "__anonymous_0x10f3efa8"
       // curprint (nameQualifier);
       // curprint ( (nm + " ").str());
       // if (isAnonymousName == false)
          if (isAnonymousName == false && classdecl_stmt->get_isUnNamed() == false)
             {
            // Output the qualified name.
               curprint (nameQualifier);
               curprint ( (nm + " ").str());
             }
#else
       // DQ (8/19/2014): Adding code to output the template instantiation with template arguments processed to support name qualification.
          if (templateInstantiation != NULL)
             {
#if 0
               printf ("In unparseClassDeclStmt(): Now output the template name plus arguments \n");
#endif
            // DQ (4/13/2019): Make this conditional upon the setting of info.SkipNameQualification()
            // curprint (nameQualifier);
               if (info.SkipNameQualification() == false)
                  {
                    curprint (nameQualifier);
                 // curprint ("/* conditional output of name qualification */");
                  }

            // DQ (4/13/2019): Turn this off before processing the rest of the template instantiation which man contain template 
            // arguments that require name qualification.
               info.unset_SkipNameQualification();

               unparseTemplateName(templateInstantiation,info);
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }
            else
             {
            // DQ (8/19/2014): Original code (copied to the else branch).
               if (isAnonymousName == false && classdecl_stmt->get_isUnNamed() == false)
                  {
                 // Output the qualified name.
                    curprint (nameQualifier);
                    curprint ( (nm + " ").str());
                  }
             }
#endif
#else
       // DQ (7/28/2012): This is the modified version of the code and I think it is not required.
       // DQ (7/28/2012): Support un-named classes (see test2012_141.C).

#error "DEAD CODE!"

          printf ("In unparseClassDeclStmt(): classdecl_stmt->get_isUnNamed() = %s \n",classdecl_stmt->get_isUnNamed() ? "true" : "false");
          if (classdecl_stmt->get_isUnNamed() == false)
             {
            // DQ (6/5/2011): Newest refactored support for name qualification.
               SgName nameQualifier = classdecl_stmt->get_qualified_name_prefix();
#if 0
               printf ("In unparseClassDeclStmt(): Output SgClassDeclaration = %p = %s qualified name: nameQualifier = %s \n",classdecl_stmt,classdecl_stmt->get_name().str(),nameQualifier.str());
#endif
            // Output the qualified name.
               curprint (nameQualifier);

#error "DEAD CODE!"

            // printf ("Output className = %s \n",classdecl_stmt->get_name().str());
            // curprint ( (classdecl_stmt->get_name() + " ").str();
               curprint ( (nm + " ").str());
             }
            else
             {
               printf ("In unparseClassDeclStmt(): This is an un-named declaration. \n");
            // DQ (7/28/2012): If this is unnamed then this would not be consistant (so check it).
               ROSE_ASSERT(classdecl_stmt->isForward() == false && info.SkipSemiColon() == false);
             }
#endif

#if 0
          curprint("/* After name in Unparse_ExprStmt::unparseClassDeclStmt */ \n");
#endif

       // DQ (2/12/2019): The "final" keyword can ounly be output on the defining declaration (at least for GNU g++ version 5.1).
       // It is however consistant in ROSE that it be marked uniformally within the defining and nondefining declaration.
       // DQ (8/11/2014): Added support for final keyword unparsing.
       // if (classdecl_stmt->get_declarationModifier().isFinal() == true)
          if ( (classdecl_stmt->isForward() == false) && (classdecl_stmt->get_declarationModifier().isFinal() == true) )
             {
            // DQ (2/12/2019): Testing, final can't be used on prototypes (I think).
            // curprint(" /* output from test 2 */ ");
#if 0
               printf ("In unparseClassDeclStmt(): classdecl_stmt         = %p = %s \n",classdecl_stmt,classdecl_stmt->class_name().c_str());
               printf ("classdecl_stmt->get_firstNondefiningDeclaration() = %p \n",classdecl_stmt->get_firstNondefiningDeclaration());
               printf ("classdecl_stmt->get_definingDeclaration()         = %p \n",classdecl_stmt->get_definingDeclaration());
               printf ("classdecl_stmt->isForward()                       = %s \n",classdecl_stmt->isForward() ? "true" : "false");
#endif
               curprint("final ");
             }

          if (classdecl_stmt->isForward() && !info.SkipSemiColon())
             {
               curprint(";");

               if (classdecl_stmt->isExternBrace())
                  {
                    curprint(" }");
                  }
             }
        }

#if 0
     printf ("Leaving unparseClassDeclStmt \n");
#endif
#if 0
     curprint ("/* Leaving unparseClassDeclStmt */ \n");
#endif
   }


void
Unparse_ExprStmt::unparseClassDefnStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
#if 0
     printf ("Inside of unparseClassDefnStmt \n");
     curprint("/* Inside of unparseClassDefnStmt */ \n");
#endif

     SgClassDefinition* classdefn_stmt = isSgClassDefinition(stmt);
     ROSE_ASSERT(classdefn_stmt != NULL);

#if OUTPUT_HIDDEN_LIST_DATA
     outputHiddenListData (classdefn_stmt);
#endif

     SgUnparse_Info ninfo(info);

  // curprint ( string("/* Print out class declaration */ \n";

     ninfo.set_SkipClassDefinition();

  // DQ (9/9/2016): Added to conform to unifor testing that these are always equal.
     ninfo.set_SkipEnumDefinition();
               
  // DQ (10/13/2006): test2004_133.C demonstrates where we need to unparse qualified names for class definitions (defining declaration).
  // DQ (10/11/2006): Don't generate qualified names for the class name of a defining declaration
  // ninfo.set_SkipQualifiedNames();

  // DQ (7/19/2003) skip the output of the semicolon
     ninfo.set_SkipSemiColon();

  // printf ("Calling unparseClassDeclStmt = %p isForward = %s from unparseClassDefnStmt = %p \n",
  //      classdefn_stmt->get_declaration(),(classdefn_stmt->get_declaration()->isForward() == true) ? "true" : "false",classdefn_stmt);
     ROSE_ASSERT(classdefn_stmt->get_declaration() != NULL);
     unparseClassDeclStmt( classdefn_stmt->get_declaration(), ninfo);

  // DQ (7/19/2003) unset the specification to skip the output of the semicolon
     ninfo.unset_SkipSemiColon();

  // DQ (10/11/2006): Don't generate qualified names for the class name of a defining declaration
  // ninfo.unset_SkipQualifiedNames();

     ninfo.unset_SkipClassDefinition();

  // DQ (9/9/2016): Added to conform to unifor testing that these are always equal.
     ninfo.unset_SkipEnumDefinition();
               
#if 0
  // DQ (1/2/2012): The name stored in SgClassDeclaration contains the template arguments (now that this is set we don't need this code).
  // DQ (1/2/2012): If this is the template instantiation then it could be marked as a specialization (supported by ROSE) 
  // and thus needs to be output as such. I think this is the new style of specification for C++ template specialization.
     if (classdefn_stmt->get_declaration()->get_specialization() == SgDeclarationStatement::e_specialization)
        {
       // curprint("/* Output the template arguments for this specialization */ \n");
          SgTemplateInstantiationDecl* templateInstantiationClassDeclaration = isSgTemplateInstantiationDecl(classdefn_stmt->get_declaration());
          ROSE_ASSERT(templateInstantiationClassDeclaration != NULL);
          SgTemplateArgumentPtrList& templateArgListPtr = templateInstantiationClassDeclaration->get_templateArguments();
          unparseTemplateArgumentList(templateArgListPtr,info);
        }
#endif

  // curprint("/* END: Print out class declaration */ \n");

     SgNamedType *saved_context = ninfo.get_current_context();

  // DQ (11/29/2004): The use of a primary and secondary declaration casue two SgClassType nodes to be generated 
  // (which should be fixed) since this is compared to another SgClassType within the generateQualifiedName() 
  // function we have to get the the type from the non-defining declaration uniformally. Same way each time so that
  // the pointer test will be meaningful.
  // ninfo.set_current_context(classdefn_stmt->get_declaration()->get_type());
     ROSE_ASSERT(classdefn_stmt->get_declaration()->get_firstNondefiningDeclaration() != NULL);
     SgClassDeclaration* classDeclaration = isSgClassDeclaration(classdefn_stmt->get_declaration()->get_firstNondefiningDeclaration());
     ROSE_ASSERT(classDeclaration->get_type() != NULL);

  // DQ (6/13/2007): Set to null before resetting to non-null value 
  // ninfo.set_current_context(classdefn_stmt->get_declaration()->get_firstNondefiningDeclaration()->get_type());
     ninfo.set_current_context(NULL);
     ninfo.set_current_context(classDeclaration->get_type());

#if 0
     printf ("In unparseClassDefnStmt(): Print out inheritance \n");
     curprint("/* Print out inheritance */ \n");
#endif

  // print out inheritance
     SgBaseClassPtrList::iterator p = classdefn_stmt->get_inheritances().begin();
  // int tmp_spec = 0;
     SgClassDeclaration *tmp_decl;
     if (p != classdefn_stmt->get_inheritances().end())
        {
          curprint ( string(": "));

       // DQ (5/9/2011): This loop structure should be rewritten...
          while(true)
             {
            // DQ (4/25/2004): Use the new modifier interface
            // tmp_spec = (*p).get_base_specifier();
            // SgBaseClassModifier & baseClassModifier = (*p).get_baseClassModifier();

            // DQ (1/21/2019): Moved to using ROSETTA generated access functions which return a pointer.
            // SgBaseClassModifier & baseClassModifier = (*p)->get_baseClassModifier();
               SgBaseClassModifier & baseClassModifier = *((*p)->get_baseClassModifier());

            // tmp_decl = (*p).get_base_class();
               tmp_decl = (*p)->get_base_class();
            // specifier
            // if (tmp_spec & SgDeclarationStatement::e_virtual)
            // if (tmp_spec & SgDeclarationStatement::e_virtual)
               if (baseClassModifier.isVirtual())
                  {
                    curprint ( string("virtual "));
                  }
            // if (tmp_spec & SgDeclarationStatement::e_public)
               if (baseClassModifier.get_accessModifier().isPublic())
                  {
                    curprint ( string("public "));
                  }
            // if (tmp_spec & SgDeclarationStatement::e_private)
               if (baseClassModifier.get_accessModifier().isPrivate())
                  {
                    curprint ( string("private "));
                  }
            // if (tmp_spec & SgDeclarationStatement::e_protected)
               if (baseClassModifier.get_accessModifier().isProtected())
                  {
                    curprint ( string("protected "));
                  }

            // DQ (5/12/2011): This might have to be a qualified name...
               SgUnparse_Info tmp_ninfo(ninfo);
               tmp_ninfo.set_name_qualification_length((*p)->get_name_qualification_length());
               tmp_ninfo.set_global_qualification_required((*p)->get_global_qualification_required());

               ROSE_ASSERT(tmp_decl != NULL);

            // SgName nameQualifier = unp->u_name->generateNameQualifier(tmp_decl,tmp_ninfo);
               SgName nameQualifier = (*p)->get_qualified_name_prefix();

            // Debugging code.
               if (tmp_ninfo.get_name_qualification_length() > 0 || tmp_ninfo.get_global_qualification_required() == true)
                  {
                 // printf ("In Unparse_ExprStmt::unparseClassDefnStmt(): nameQualifier = %s \n",nameQualifier.str());
                  }

            // DQ (4/12/2019): Supress the name qualification we are are using a previously generated string for the class name.
            // curprint(nameQualifier.str());

            // print the base class name
            // DQ (8/20/2014): We need to output the template name when this is a templated base class.
            // curprint(tmp_decl->get_name().str());
               SgTemplateInstantiationDecl* templateInstantiationDeclaration = isSgTemplateInstantiationDecl(tmp_decl);
#if 0
               printf ("In unparseClassDefnStmt(): base class output: tmp_decl = %p = %s \n",tmp_decl,tmp_decl->class_name().c_str());
#endif
               if (templateInstantiationDeclaration != NULL)
                  {
#if 0
                    printf ("In unparseClassDefnStmt(): calling unparseTemplateName() \n");
                    curprint ("/* calling unparseTemplateName */ ");
#endif

#if 1
                 // DQ (4/12/2019): Support for output of generated string for type (used where name 
                 // qualification is required for subtypes (e.g. template arguments)).
                 // SgNode* nodeReferenceToClass = info.get_reference_node_for_qualification();
                    SgNode* nodeReferenceToClass = *p;
#if 0
                    printf ("In unparseClassDefnStmt(): nodeReferenceToClass = %p \n",nodeReferenceToClass);
#endif
                    if (nodeReferenceToClass != NULL)
                       {
#if 0
                         printf ("rrrrrrrrrrrr In unparseClassDefnStmt() output type generated name: nodeReferenceToClass = %p = %s SgNode::get_globalTypeNameMap().size() = %" PRIuPTR " \n",
                              nodeReferenceToClass,nodeReferenceToClass->class_name().c_str(),SgNode::get_globalTypeNameMap().size());
#endif
                         std::map<SgNode*,std::string>::iterator i = SgNode::get_globalTypeNameMap().find(nodeReferenceToClass);
                         if (i != SgNode::get_globalTypeNameMap().end())
                            {
                           // I think this branch supports non-template member functions in template classes (called with explicit template arguments).
                           // usingGeneratedNameQualifiedClassNameString = true;

                              string classNameString = i->second.c_str();
#if 0
                              printf ("ssssssssssssssss Found type name in SgNode::get_globalTypeNameMap() typeNameString = %s for nodeReferenceToType = %p = %s \n",
                                   classNameString.c_str(),nodeReferenceToClass,nodeReferenceToClass->class_name().c_str());
#endif

                              curprint(nameQualifier.str());

                              curprint (classNameString);
                            }
                           else
                            {
                            // Note that the globalTypeNameMap is populated with entries only when name qualification is detected, so it is OK for entries not to be found there.
#if 0
                              printf ("Could not find saved name qualified class name in globalTypeNameMap: using key: nodeReferenceToClass = %p = %s \n",nodeReferenceToClass,nodeReferenceToClass->class_name().c_str());
#endif

                           // curprint ("/* Could not find properly saved name from name qualification */ ");

#if 1
                           // DQ (4/12/2019): We need to access any possible previously saved stringified version 
                           // of the type name from the name qualificaiton.
                              SgUnparse_Info ninfo2(ninfo);

                              SgBaseClass* baseClass = *p;
                              ROSE_ASSERT(baseClass != NULL);

                           // Output the name qualification explicitly.
                              curprint(nameQualifier.str());

                           // We want to use the templateInstantiationDeclaration if is is not shared, but I think it is shared.  So use the SgBaseClass (*p).
                              ninfo2.set_reference_node_for_qualification(baseClass);

                           // unparseTemplateName(templateInstantiationDeclaration,info);
                              unparseTemplateName(templateInstantiationDeclaration,ninfo2);
#endif
#if 0
                              printf ("Error: there should have been a globalTypeNameMap entry! \n");
                              ROSE_ASSERT(false);
#endif
                            }

#if 0
                         printf ("Exiting as a test! \n");
                         ROSE_ASSERT(false);
#endif
                       }
                      else
                       {
#if 1
                      // DQ (4/12/2019): We need to access any possible previously saved stringified version 
                      // of the type name from the name qualificaiton.
                         SgUnparse_Info ninfo2(ninfo);

                         SgBaseClass* baseClass = *p;
                         ROSE_ASSERT(baseClass != NULL);

                      // Output the name qualification explicitly.
                         curprint(nameQualifier.str());

                      // We want to use the templateInstantiationDeclaration if is is not shared, but I think it is shared.  So use the SgBaseClass (*p).
                         ninfo2.set_reference_node_for_qualification(baseClass);

                      // unparseTemplateName(templateInstantiationDeclaration,info);
                         unparseTemplateName(templateInstantiationDeclaration,ninfo2);
#endif
                       }
#endif
#if 0
                    printf ("In unparseClassDefnStmt(): DONE calling unparseTemplateName() \n");
                    curprint ("/* DONE calling unparseTemplateName */ ");
#endif
                  }
                 else
                  {
                 // DQ (4/12/2019): Use the name qualification if we are not using the previously generated string for the class name.
                    curprint(nameQualifier.str());

                    curprint(tmp_decl->get_name().str());
                  }

               p++;

               if (p != classdefn_stmt->get_inheritances().end())
                  {
                    curprint ( string(","));
                  }
                 else
                  {
                    break;
                  }
             }
        }

#if 0
  // curprint ( string("\n/* After specification of base classes unparse the declaration body */ \n";
     printf ("After specification of base classes unparse the declaration body  info.SkipBasicBlock() = %s \n",(info.SkipBasicBlock() == true) ? "true" : "false");
#endif

  // DQ (9/28/2004): Turn this back on as the only way to prevent this from being unparsed!
  // DQ (11/22/2003): Control unparsing of the {} part of the definition
  // if ( !info.SkipBasicBlock() )
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
          curprint ( string("{"));
          unp->cur.format(classdefn_stmt, info, FORMAT_AFTER_BASIC_BLOCK1);

       // DQ (2/25/2016): Adding support for specification of AstUnparseAttribute for placement inside of a SgClassDefinition.
       // Note that this does not replace any existing declarations in the class definition.
          AstUnparseAttribute* unparseAttribute = dynamic_cast<AstUnparseAttribute*>(classdefn_stmt->getAttribute(AstUnparseAttribute::markerName));
          if (unparseAttribute != NULL)
             {
            // Note that in most cases unparseLanguageSpecificStatement() will be called, some formatting 
            // via "unp->cur.format(stmt, info, FORMAT_BEFORE_STMT);" may be done.  This can cause extra 
            // CRs to be inserted (which only looks bad).  Not clear now to best clean this up.
               string code = unparseAttribute->toString(AstUnparseAttribute::e_inside);
               curprint (code);
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }

          SgDeclarationStatementPtrList::iterator pp = classdefn_stmt->get_members().begin();

          while ( pp != classdefn_stmt->get_members().end() )
             {
#if 0
               printf ("In unparseClassDefnStmt(): (*pp)->get_declarationModifier().get_accessModifier().isProtected() = %s \n",
                    (*pp)->get_declarationModifier().get_accessModifier().isProtected() ? "true" : "false");
#endif
               unparseStatement((*pp), ninfo);
               pp++;
             }

       // DQ (3/17/2005): This helps handle cases such as class foo { #include "constant_code.h" }
          ROSE_ASSERT(classdefn_stmt->get_startOfConstruct() != NULL);
          ROSE_ASSERT(classdefn_stmt->get_endOfConstruct() != NULL);
#if 0
          printf ("classdefn_stmt range %d - %d \n",
               classdefn_stmt->get_startOfConstruct()->get_line(),
               classdefn_stmt->get_endOfConstruct()->get_line());
#endif

#if 0
          curprint("/* test 5 */ \n");
#endif
          unparseAttachedPreprocessingInfo(classdefn_stmt, info, PreprocessingInfo::inside);

#if 0
          curprint("/* test 6 */ \n");
#endif

          unp->cur.format(classdefn_stmt, info, FORMAT_BEFORE_BASIC_BLOCK2);
          curprint ( string("}"));

       // DQ (6/14/2006): Add packing pragma support (reset the packing 
       // alignment to the default, part of packing pragma normalization).
          if (packingAlignment != 0)
             {
               curprint ( string("\n#pragma pack()"));
             }

          unp->cur.format(classdefn_stmt, info, FORMAT_AFTER_BASIC_BLOCK2);
        }

  // DQ (6/13/2007): Set to null before resetting to non-null value 
     ninfo.set_current_context(NULL);
     ninfo.set_current_context(saved_context);

#if 0
     printf ("Calling unparseTypeAttributes(): classdefn_stmt->get_declaration() = %p \n",classdefn_stmt->get_declaration());
#endif

     unparseTypeAttributes(classdefn_stmt->get_declaration());

#if 0
     printf ("Leaving unparseClassDefnStmt \n");
     curprint("/* Leaving unparseClassDefnStmt */ \n");
#endif
   }


void
Unparse_ExprStmt::unparseTypeAttributes ( SgDeclarationStatement* declaration )
   {
  // DQ (10/4/2012): Added support for transparent unions.
     ROSE_ASSERT(declaration != NULL);

     bool isGnuAttributeTransparentUnion = declaration->get_declarationModifier().get_typeModifier().isGnuAttributeTransparentUnion();

  // If this came from a type then declaration is the first nondefining declaration (see test2012_10_4.c).
     bool definingDeclaration_isGnuAttributeTransparentUnion = false;
     if (declaration->get_definingDeclaration() != NULL)
          definingDeclaration_isGnuAttributeTransparentUnion = declaration->get_definingDeclaration()->get_declarationModifier().get_typeModifier().isGnuAttributeTransparentUnion();

     if (definingDeclaration_isGnuAttributeTransparentUnion == true)
        isGnuAttributeTransparentUnion = true;

#if 0
     printf ("In unparseTypeAttributes(): declaration = %p = %s isGnuAttributeTransparentUnion() = %s \n",declaration,declaration->class_name().c_str(),isGnuAttributeTransparentUnion ? "true" : "false");
#endif

  // This should only be set for unions.
     if (isGnuAttributeTransparentUnion == true)
        {
#if 0
       // The declaration can sometimes be a typedef declaration, so we can't test this.
          SgClassDeclaration* classDeclaration = isSgClassDeclaration(declaration);
          ROSE_ASSERT(classDeclaration != NULL);
          ROSE_ASSERT(classDeclaration->get_class_type() == SgClassDeclaration::e_union);
          ROSE_ASSERT(classDeclaration->get_class_type() == SgClassDeclaration::e_union);
#endif
          curprint(" __attribute__((__transparent_union__))");
        }

  // DQ (1/3/2014): Added support for packing attribute.
     if (declaration->get_declarationModifier().get_typeModifier().isGnuAttributePacked() == true)
        {
       // curprint(" /* from unparseTypeAttributes(SgDeclarationStatement*) */ __attribute__((packed))");
          curprint(" __attribute__((packed))");
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

  // DQ (7/24/2014): Added support to unparse the alignment attribute.
     short alignmentValue = declaration->get_declarationModifier().get_typeModifier().get_gnu_attribute_alignment();

#if 0
     printf ("In unparseTypeAttributes(SgDeclarationStatement*): alignmentValue = %d \n",(int)alignmentValue);
#endif

  // DQ (7/24/2014): The default value is changed from zero to -1 (and the type was make to be a short (signed) value).
     if (alignmentValue >= 0)
        {
#if 0
          curprint(" /* alignment attribute on decl_stmt->get_declarationModifier().get_typeModifier() */ ");
#endif
       // DQ (7/27/2014): Fixed attribute to have correct spelling.
       // curprint(" __attribute__((align(N)))");
       // curprint(" __attribute__((align(");
          curprint(" __attribute__((aligned(");
          curprint(StringUtility::numberToString((int)alignmentValue));
          curprint(")))");
        }
   }


void
Unparse_ExprStmt::unparseEnumDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgEnumDeclaration* enum_stmt = isSgEnumDeclaration(stmt);
     ROSE_ASSERT(enum_stmt != NULL);

  // info.display("Called inside of unparseEnumDeclStmt()");

#if 0
     printf ("In unparseEnumDeclStmt(): stmt = %p = %s \n",stmt,stmt->class_name().c_str());
#endif

     string enum_string = "enum ";

  // DQ (8/12/2014): Adding support for C++11 scoped enums (syntax is "enum class ").
     if (enum_stmt->get_isScopedEnum() == true)
        {
          enum_string += "class ";
        }

  // Check if this enum declaration appears imbedded within another declaration
     if ( !info.inEmbeddedDecl() )
        {
       // This is the more common declaration of an enum with the definition attached.
#if 0
          printf ("In unparseEnumDeclStmt(): common declaration of an enum with the definition attached \n");
#endif
       // If this is part of a class definition then get the access information
          SgClassDefinition *cdefn = isSgClassDefinition(enum_stmt->get_parent());
          if (cdefn && cdefn->get_declaration()->get_class_type() == SgClassDeclaration::e_class)
             {
               info.set_CheckAccess();
             }
       // printDebugInfo("entering unp->u_sage->printSpecifier", true);
          unp->u_sage->printSpecifier(enum_stmt, info);
          info.unset_CheckAccess();

       // DQ (2/14/2019): Adding name qualification support.
       // curprint(enum_string + enum_stmt->get_name().str() + " ");
          curprint(enum_string);

       // DQ (5/12/2011): This might have to be a qualified name...
          SgUnparse_Info ninfo(info);
          ninfo.set_name_qualification_length(enum_stmt->get_name_qualification_length());
          ninfo.set_global_qualification_required(enum_stmt->get_global_qualification_required());

          ROSE_ASSERT(enum_stmt != NULL);

          SgName nameQualifier = enum_stmt->get_qualified_name_prefix();

          curprint(nameQualifier.str());

          curprint(enum_stmt->get_name() + " ");
        }
       else
        {
       // DQ (2/14/2019): Test if this branch is ever taken.
          printf ("Exiting as a test (need to know if this branch is ever taken) \n");
          ROSE_ASSERT(false);

       // This is a declaration of an enum appearing within another declaration (e.g. function declaration as a return type).
          SgClassDefinition *cdefn = NULL;

       // DQ (9/9/2004): Put this message in place at least for now!
          printf ("Need logic to handle enums defined in namespaces and which require qualification \n");

       // ck if defined within a var decl
          int v = GLOBAL_STMT;
       // SgStatement *cparent=enum_stmt->get_parent();
          SgStatement *cparent = isSgStatement(enum_stmt->get_parent());
          v = cparent->variant();
          if (v == VAR_DECL_STMT || v == TYPEDEF_STMT)
               cdefn = isSgClassDefinition(cparent->get_parent());
            else
               if ( v == CLASS_DEFN_STMT )
                    cdefn = isSgClassDefinition(cparent);

          if ( cdefn )
             {
               SgNamedType *ptype=isSgNamedType(cdefn->get_declaration()->get_type());
               if (!ptype || (info.get_current_context() == ptype))
                  {
                 // curprint( string("enum " ) +  enum_stmt->get_name().str() + " ");
                    curprint(enum_string + enum_stmt->get_name().str() + " ");
                  }
                 else
                  {
                 // add qualifier of current types to the name
                    SgName nm = cdefn->get_declaration()->get_qualified_name();
                    if ( !nm.is_null() )
                       {
                         curprint ( string(nm.str()) + "::" + enum_stmt->get_name().str() + " ");
                       }
                      else
                       { 
                      // curprint( string("enum " ) + enum_stmt->get_name().str() + " ");
                         curprint(enum_string + enum_stmt->get_name().str() + " ");
                       }
                  }
             }
            else
             {
            // curprint ( string("enum " ) + enum_stmt->get_name().str() + " ");
               curprint (enum_string + enum_stmt->get_name().str() + " ");
             }
        }

#if 0
     printf ("info.SkipEnumDefinition()  = %s \n",info.SkipEnumDefinition()  ? "true" : "false");
     printf ("enum_stmt->get_embedded()  = %s \n",enum_stmt->get_embedded()  ? "true" : "false");
     printf ("enum_stmt->isExternBrace() = %s \n",enum_stmt->isExternBrace() ? "true" : "false");
#endif

  // DQ (8/12/2014): Adding support for C++11 base type specification syntax.
     if (enum_stmt->get_field_type() != NULL)
        {
          curprint(" : ");

       // Make a new SgUnparse_Info object.
          SgUnparse_Info ninfo(info);
          unp->u_type->unparseType(enum_stmt->get_field_type(),ninfo);           
        }

#if 0
     printf ("enum_stmt = %p \n",enum_stmt);
     printf ("enum_stmt->get_definingDeclaration() = %p \n",enum_stmt->get_definingDeclaration());
#endif

  // DQ (6/26/2005): Support for empty enum declarations!
     if (enum_stmt == enum_stmt->get_definingDeclaration())
        {
#if 0
          printf ("In the unparser this is the Enum's defining declaration! \n");
#endif
          curprint ("{"); 
        }

  // if (!info.SkipDefinition()
     if (!info.SkipEnumDefinition()
    /* [BRN] 4/19/2002 --  part of the fix in unparsing var decl including enum definition */
                  || enum_stmt->get_embedded())
        {
          SgUnparse_Info ninfo(info);
          ninfo.set_inEnumDecl();
          SgInitializer *tmp_init=NULL;
          SgName tmp_name;

       // TODO wrap into a function and to be called by all
          SgInitializedNamePtrList::iterator p      = enum_stmt->get_enumerators().begin();
          SgInitializedNamePtrList::iterator p_last = enum_stmt->get_enumerators().end();

       // Guard against decrementing an invalid iterator
          if (p != p_last)
               p_last--;

          for (; p!=enum_stmt->get_enumerators().end(); p++)
          {
            // Liao, 5/14/2009
            // enumerators may come from another included file
            // have to tell if it matches the current declaration's file before unparsing it!!
            // See test case: tests/nonsmoke/functional/CompileTests/C_test/test2009_05.c
            // TODO: still need work on mixed cases: part of elements are in the original file and others are from a header
            SgInitializedName* field = *p;
            ROSE_ASSERT(field !=NULL);
            bool isInSameFile = (field->get_file_info()->get_filename()==enum_stmt->get_file_info()->get_filename());
            if (isInSameFile)
            {
              unparseAttachedPreprocessingInfo(field, info, PreprocessingInfo::before);
              // unparse the element   
              ROSE_ASSERT((*p) != NULL);
              tmp_name=(*p)->get_name();
              tmp_init=(*p)->get_initializer();
              curprint ( tmp_name.str());
              if (tmp_init != NULL)
              {
                curprint("=");
                unparseExpression(tmp_init, ninfo);
              }

              //if (p != (enum_stmt->get_enumerators().end()))
              if (p != p_last)
              {
                curprint(",");
              }
 
            } // end same file
          } // end for

          if  (enum_stmt->get_enumerators().size()!=0)
            // DQ (3/17/2005): This helps handle cases such as void foo () { #include "constant_code.h" }
            unparseAttachedPreprocessingInfo(enum_stmt, info, PreprocessingInfo::inside);

#if 0
          if (!info.SkipSemiColon())
             {
            // curprint ( string(" /* output a ; */ ";
               curprint ( string(";"));
               if (enum_stmt->isExternBrace())
                  {
                    curprint(" }");
                  }
             }
#endif
       /* [BRN] 4/19/2002 -- part of fix in unparsing var decl including enum definition */
          if (enum_stmt->get_embedded())
             {
               curprint(" ");
             }
          enum_stmt->set_embedded(false);
       /* [BRN] end */
        } /* if */

  // DQ (6/26/2005): Support for empty enum declarations!
     if (enum_stmt == enum_stmt->get_definingDeclaration())
        {
          curprint("} ");
        }

  // DQ (6/26/2005): Moved to location after output of closing "}" from enum definition
     if (!info.SkipSemiColon())
        {
       // curprint ( string(" /* output a ; */ ";
          curprint ( string(";"));
          if (enum_stmt->isExternBrace())
             {
               curprint(" }");
             }
        }

#if 0
     printf ("Leaving unparseEnumDeclStmt(): \n");
#endif
   }


void
Unparse_ExprStmt::unparseExprStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgExprStatement* expr_stmt = isSgExprStatement(stmt);
     ROSE_ASSERT(expr_stmt != NULL);

     SgUnparse_Info newinfo(info);

  // DQ (5/9/2015): Added assertion.
     ROSE_ASSERT(expr_stmt->get_expression() != NULL);

#if 0
     printf ("\n\nTop of unparseExprStmt() expr_stmt->get_expression() = %p = %s \n",expr_stmt->get_expression(),expr_stmt->get_expression()->class_name().c_str());
#endif

  // Expressions are another place where a class definition should NEVER be unparsed
  // DQ (5/23/2007): Note that statement expressions can have class definition
  // (so they are exceptions, see test2007_51.C).
     newinfo.set_SkipClassDefinition();

  // DQ (1/9/2014): We have to make the handling of enum definitions consistant with that of class definitions.
     newinfo.set_SkipEnumDefinition();

#if 0
     printf ("In unparseExprStmt(): info.SkipClassDefinition() = %s \n",(newinfo.SkipClassDefinition() == true) ? "true" : "false");
     printf ("In unparseExprStmt(): info.SkipEnumDefinition()  = %s \n",(newinfo.SkipEnumDefinition() == true) ? "true" : "false");
#endif

  // DQ (1/9/2014): These should have been setup to be the same.
     ROSE_ASSERT(newinfo.SkipClassDefinition() == newinfo.SkipEnumDefinition());

  // if (expr_stmt->get_the_expr())
     if (expr_stmt->get_expression())
        {
       // printDebugInfo(getSgVariant(expr_stmt->get_the_expr()->variant()), true); 
       // unparseExpression(expr_stmt->get_the_expr(), newinfo);
          unparseExpression(expr_stmt->get_expression(), newinfo);
        }
       else
        {
          assert(false);
        }

     if (newinfo.inVarDecl())
        {
          curprint(",");
        }
       else
        {
       // DQ (11/2/2015): This is part of a change to support uniformity in how for statement tests are unparsed.
       // if (!newinfo.inConditional() && !newinfo.SkipSemiColon())
       // if (newinfo.SkipSemiColon() == false)
#if 0
          if (newinfo.inConditional() == false)
#else
          if (newinfo.SkipSemiColon() == false)
#endif
             {
            // DQ (11/2/2015): Add a space to match previous behavior (and tests using diff). No, I don't like this.
            // curprint(";");
               curprint(";");
             }
        }

#if 0
     printf ("Leaving unparseExprStmt() expr_stmt->get_expression() = %p = %s \n",expr_stmt->get_expression(),expr_stmt->get_expression()->class_name().c_str());
#endif
   }


void Unparse_ExprStmt::unparseLabelStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgLabelStatement* label_stmt = isSgLabelStatement(stmt);
     ROSE_ASSERT(label_stmt != NULL);

#if 0
     printf ("Inside of unparseLabelStmt(label_stmt = %p) \n",label_stmt);
#endif

     curprint ( string(label_stmt->get_label().str()) + ":");

  // DQ (3/15/2006): Remove the "/* empty statement */" comment (leave the ";").
  // DQ (10/20/2005): Unparse an empty statement with each label
  // curprint ( string(" /* empty statement */ ;";

  // DQ (3/18/2006): I don't think we need this and if we do then we need an example of where we need it.
  // test2005_164.C demonstrates that we need the ";" if the label is the last statment in the block.
  // EDG is more accepting and does not require a ";" for a label appearing at the end of the block,
  // but g++ is particular on this subtle point.  So we should make the unparser figure this out.
  // curprint ( string(" ;";

#if 0
  // DQ (10/28/2012): Added debugging code for test2012_100.c.
     if (label_stmt->get_parent() == NULL)
        {
          printf ("ERROR: label_stmt->get_parent() == NULL \n");
          label_stmt->get_startOfConstruct()->display("ERROR: label_stmt->get_parent() == NULL");
        }
     ROSE_ASSERT(label_stmt->get_parent() != NULL);

  // DQ (10/28/2012): Added debugging code for test2012_100.c.
     if (isSgScopeStatement(label_stmt->get_parent()) == NULL)
        {
          printf ("ERROR: label_stmt->get_parent() = %p = %s but is not a SgScopeStatement \n",label_stmt->get_parent(),label_stmt->get_parent()->class_name().c_str());
          label_stmt->get_startOfConstruct()->display("ERROR: label_stmt->get_parent() == NULL");
        }
  // ROSE_ASSERT(isSgScopeStatement(label_stmt->get_parent()) != NULL);
#endif

     ROSE_ASSERT(label_stmt->get_parent() != NULL);

  // DQ (10/25/2007): Modified handling of labels so that they explicitly include a scope set to the 
  // SgFunctionDefinition. SgLabelSymbol objects are also now placed into the SgFunctionDefinition's 
  // symbol table.
  // SgScopeStatement* scope = label_stmt->get_scope();
     SgScopeStatement* scope = isSgScopeStatement(label_stmt->get_parent());
  // ROSE_ASSERT(scope != NULL);

  // SgStatementPtrList & statementList = scope->getStatementList();
     SgStatementPtrList* statementList = NULL;

#if 0
     printf ("In unparseLabelStmt(): scope = %p \n",scope);
     if (scope != NULL)
        {
          printf ("In unparseLabelStmt(): scope = %p = %s \n",scope,scope->class_name().c_str());
        }
#endif

     bool allocatedStatementList = false;
     if (scope != NULL)
        {
       // In C and C++, labels can't be places where only declarations are allowed so we know to get the 
       // Statement list instead of the declaration list.
          ROSE_ASSERT(scope->containsOnlyDeclarations() == false);

       // DQ (10/27/2012): This is called as part of processing test2012_104.c.  So I guess we have to support it...
          SgIfStmt* ifStatement = isSgIfStmt(scope);

          if (ifStatement != NULL)
             {
            // The test2012_116.c and the smaller test2012_117.c demonstate and example of a label attached to one side of a conditional.
#if 0
               printf ("We can't implment this calling the getStatementList() from the SgIfStmt scope (becasue there are two lists (one for the TRUE branch and one for the FALSE branch). \n");
               ifStatement->get_startOfConstruct()->display("We can't implment this calling the getStatementList() from the SgIfStmt scope: debug");
               bool label_is_on_true_branch = (stmt == ifStatement->get_true_body());
               printf ("label_is_on_true_branch = %s \n",label_is_on_true_branch ? "true" : "false");
#endif
               statementList = new SgStatementPtrList();
               ROSE_ASSERT(statementList != NULL);

               allocatedStatementList = true;

               statementList->push_back(ifStatement->get_true_body());
               statementList->push_back(ifStatement->get_false_body());
#if 0
               printf ("statementList->size() = %" PRIuPTR " \n",statementList->size());
#endif
             }
            else
             {
               statementList = &(scope->getStatementList());
             }
        }
       else
        {
       // This is going to be the trivial case of only the label statement itself in the statementList.
          statementList = new SgStatementPtrList();
          ROSE_ASSERT(statementList != NULL);

          allocatedStatementList = true;

          statementList->push_back(label_stmt);
        }

     ROSE_ASSERT(statementList != NULL);

#if 0
     printf ("In unparseLabelStmt(): statementList->size() = %zu \n",statementList->size());
#endif

  // Find the label in the parent scope
     SgStatementPtrList::iterator positionOfLabel = find(statementList->begin(),statementList->end(),label_stmt);

  // Verify that we found the label in the scope
  // ROSE_ASSERT(positionOfLabel != SgStatementPtrList::npos);
     if (positionOfLabel == statementList->end())
        {
#if 0
       // DQ (1/6/2018): I think this is no longer an error in the new design that treats lables as compound statements.
          printf ("ERROR: Found label = %p = %s at end of scope! \n",label_stmt,label_stmt->get_label().str());
          label_stmt->get_startOfConstruct()->display("positionOfLabel == statementList.end()");
          ROSE_ASSERT(positionOfLabel != statementList->end());
#endif
        }
       else
        {
       // ROSE_ASSERT(positionOfLabel != statementList.end());
          ROSE_ASSERT(*positionOfLabel == label_stmt);
#if 0
          printf ("label_stmt->get_label() = %p = %s \n",label_stmt,label_stmt->get_label().str());
#endif
       // DQ (10/27/2012): I think it is a bug to increment positionOfLabel.
       // Increment past the label to see what is next (but don't count SgNullStatements)
       // positionOfLabel++;

          while ( ( positionOfLabel != statementList->end() ) && ( (*positionOfLabel)->variantT() == V_SgNullStatement ) )
             {
#if 0
               printf ("Found a SgNullStatement (skipping over it) *positionOfLabel = %p = %s \n",*positionOfLabel,(*positionOfLabel)->class_name().c_str());
#endif
               positionOfLabel++;
             }

       // If we are at the end (just past the last statement) then we need the ";" 
          if (positionOfLabel == statementList->end())
             {
#if 0
               printf ("Note: positionOfLabel == statementList->end(): so output a closing semi-colon \n");
#endif
               curprint ( string(";"));
             }
        }

  // If this is memory that we allocated on the heap then call delete.
     if (allocatedStatementList == true)
        {
          delete statementList;
          statementList = NULL;
        }

#if 0
     printf ("Unparse the associated label's statement (if it is available) label_stmt->get_statement() = %p = %s \n",
          label_stmt->get_statement(),label_stmt->get_statement() != NULL ? label_stmt->get_statement()->class_name().c_str() : "null");
#endif

  // DQ (1/14/2013): However, we want it to be unparsed by the function unparsing the statement list (typically SgBasicBlock) instead of here.
  // The statement associated with the SgLabelStatement was not previously being inserted into the list of the current scope and this is a 
  // problem for the data flow analysis (bug reported by Robb).  Also we don't want to have a mechanism for hidding statements behind 
  // SgLabelStatements in general, so it makes more sense (and is consistant with ROSE based on EDG 3.3) to not unparse the associated 
  // statement here.  Even though we do not correctly reference the label's associated statement correctly in this version of ROSE based on EDG 4.x).

#if 1
  // DQ (1/6/2018): Turn this back on since we handled labels as compound statements now, at
  // least where they are processed in switch statements (which will have to be made uniform).
  // DQ (10/27/2012): Unparse the associated statement to the label.
  // Note that in the edg33 version of ROSE this was always a SgNullStatement, this is corrected in the design with the edg4x work.
     if (label_stmt->get_statement() != NULL)
        {
#if 0
          printf ("Calling unparseStatement() stmt = %p = %s \n",label_stmt->get_statement(),label_stmt->get_statement()->class_name().c_str());
#endif
          unparseStatement(label_stmt->get_statement(), info);
        }
#endif

#if 0
     printf ("Leaving unparseLabelStmt(label_stmt = %p) \n",label_stmt);
#endif
   }


void
Unparse_ExprStmt::unparseWhileStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgWhileStmt* while_stmt = isSgWhileStmt(stmt);
     ROSE_ASSERT(while_stmt != NULL);

#if 0
     printf ("In unparseWhileStmt(): info.unparsedPartiallyUsingTokenStream() = %s \n",info.unparsedPartiallyUsingTokenStream() ? "true" : "false");
     curprint("/* In unparseWhileStmt(): TOP */ ");
#endif

  // DQ (12/17/2014): Test for if we have unparsed partially using the token stream. 
  // If so then we don't want to unparse this syntax, if not then we require this syntax.
  // curprint("while(");
     bool saved_unparsedPartiallyUsingTokenStream = info.unparsedPartiallyUsingTokenStream();
     if (saved_unparsedPartiallyUsingTokenStream == false)
        {
       // unp->cur.format(namespaceDefinition, info, FORMAT_BEFORE_BASIC_BLOCK2);
       // curprint("/* from AST SgWhileStmt */ while(");
          curprint("while(");
       // unp->cur.format(namespaceDefinition, info, FORMAT_AFTER_BASIC_BLOCK2);
        }
       else
        {
       // SgStatement* body = while_stmt->get_body();
       // curprint("/* partial token sequence SgWhileStmt */ ");
          SgStatement* condition = while_stmt->get_condition();
          unparseStatementFromTokenStream (stmt, condition, e_token_subsequence_start, e_token_subsequence_start, info);
        }

#if 0
     printf ("In unparseWhileStmt(): unparse the condition \n");
     curprint("/* In unparseWhileStmt(): unparse the condition */ ");
#endif

  // DQ (10/19/2012): We now want to have more control over where ";" is output.
  // See test2012_47.c for an example of there this can't be explicitly handled 
  // for all parts of a conditional. In this case we call unset_SkipSemiColon()
  // in SgClassDefinition so that they will be output properly there.
  // Build a specific SgUnparse_Info to support the conditional.
  // info.set_inConditional();
  // info.set_inConditional();
  // unparseStatement(while_stmt->get_condition(), info);
  // info.unset_inConditional();

     SgUnparse_Info ninfo(info);
     ninfo.set_inConditional();
     ninfo.set_SkipSemiColon();
     unparseStatement(while_stmt->get_condition(), ninfo);

  // curprint(")");
     if (saved_unparsedPartiallyUsingTokenStream == false)
        {
          curprint(")");

          if (while_stmt->get_body())
             {
               unp->cur.format(while_stmt->get_body(), info, FORMAT_BEFORE_NESTED_STATEMENT);
               unparseStatement(while_stmt->get_body(), info);
               unp->cur.format(while_stmt->get_body(), info, FORMAT_AFTER_NESTED_STATEMENT);
             }
            else
             {
               if (!info.SkipSemiColon())
                  {
                    curprint(";");
                  }
             }
        }
       else
        {
          SgStatement* condition = while_stmt->get_condition();
          SgStatement* body      = while_stmt->get_body();

          ROSE_ASSERT(condition != NULL);
          ROSE_ASSERT(body != NULL);

       // unparseStatementFromTokenStream (condition, body, e_trailing_whitespace_start, e_token_subsequence_start);
       // unparseStatementFromTokenStream (condition, body, e_trailing_whitespace_start, e_leading_whitespace_start);
          unparseStatementFromTokenStream (condition, e_trailing_whitespace_start, e_trailing_whitespace_end, info);

       // Output syntax explicitly.
          curprint(")");

          unparseStatement(while_stmt->get_body(), info);
#if 0
          curprint("/* end of SgWhileStmt body (partial token strean unparse) */ ");
#endif
        }

#if 0
     if (saved_unparsedPartiallyUsingTokenStream == false)
        {
       // Nothing to do here.
        }
       else
        {
          SgStatement* condition = while_stmt->get_condition();
          SgStatement* body      = while_stmt->get_body();

          ROSE_ASSERT(condition != NULL);
          ROSE_ASSERT(body != NULL);

          curprint("/* end of SgWhileStmt (partial token strean unparse) */ ");
       // unparseStatementFromTokenStream (condition, body, e_trailing_whitespace_start, e_token_subsequence_start);
       // unparseStatementFromTokenStream (condition, body, e_trailing_whitespace_start, e_leading_whitespace_start);
        }
#endif

#if 0
     printf ("Leaving unparseWhileStmt() \n");
     curprint("/* Leaving unparseWhileStmt() */ ");
#endif
   }


void
Unparse_ExprStmt::unparseDoWhileStmt(SgStatement* stmt, SgUnparse_Info& info) 
   {
     SgDoWhileStmt* dowhile_stmt = isSgDoWhileStmt(stmt);
     ROSE_ASSERT(dowhile_stmt != NULL);

     curprint("do ");

     unp->cur.format(dowhile_stmt->get_body(), info, FORMAT_BEFORE_NESTED_STATEMENT);
     unparseStatement(dowhile_stmt->get_body(), info);
     unp->cur.format(dowhile_stmt->get_body(), info, FORMAT_AFTER_NESTED_STATEMENT);

  // curprint( string("while " ) + "(");
     curprint("while (");

     SgUnparse_Info ninfo(info);
     ninfo.set_inConditional();

  // DQ (11/2/2015): Skip output of ";" in conditional.
     ninfo.set_SkipSemiColon();

  // we need to keep the properties of the prevnode (The next prevnode will set the
  // line back to where "do" was printed) 
  // SgLocatedNode* tempnode = prevnode;

     unparseStatement(dowhile_stmt->get_condition(), ninfo);

  // Note that unseting this flag is not significant.
     ninfo.unset_inConditional();

     curprint(")");

     if (!info.SkipSemiColon()) 
        {
          curprint(";");
        }
   }

void
Unparse_ExprStmt::unparseSwitchStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgSwitchStatement* switch_stmt = isSgSwitchStatement(stmt);
  
     ROSE_ASSERT(switch_stmt != NULL);

#if 0
     printf ("In unparseSwitchStmt() \n");
     curprint("/* In unparseSwitchStmt() */ ");
#endif

     bool saved_unparsedPartiallyUsingTokenStream = info.unparsedPartiallyUsingTokenStream();

  // DQ (12/28/2014): Test for if we have unparsed partially using the token stream. 
     if (saved_unparsedPartiallyUsingTokenStream == false)
        {
          curprint ("switch(");
        }
       else
        {
          SgStatement* item_selector = switch_stmt->get_item_selector();
       // unparseStatementFromTokenStream (stmt, item_selector, e_token_subsequence_start, e_leading_whitespace_start);
       // unparseStatementFromTokenStream (stmt, item_selector, e_token_subsequence_start, e_token_subsequence_start);
          unparseStatementFromTokenStream (stmt, item_selector, e_token_subsequence_start, e_leading_whitespace_start, info);
        }

     SgUnparse_Info ninfo(info);
     ninfo.set_SkipSemiColon();
     ninfo.set_inConditional();
     unparseStatement(switch_stmt->get_item_selector(), ninfo);

     if (saved_unparsedPartiallyUsingTokenStream == false)
        {
          curprint (")");
        }
       else
        {
          SgStatement* item_selector = switch_stmt->get_item_selector();
          SgStatement* switch_body = switch_stmt->get_body();

          unparseStatementFromTokenStream (item_selector, switch_body, e_trailing_whitespace_start, e_leading_whitespace_start, info);
        }

#if 0
     printf ("In unparseSwitchStmt(): unparse the switch body \n");
     curprint("/* In unparseSwitchStmt() unparse the switch body */ ");
#endif

  // DQ (11/5/2003): Support for skipping basic block added to support 
  //                 prefix generation for AST Rewrite Mechanism
  // if(switch_stmt->get_body())
     if ( (switch_stmt->get_body() != NULL) && !info.SkipBasicBlock())
        {
          unparseStatement(switch_stmt->get_body(), info);
        }

#if 0
     printf ("Leaving unparseSwitchStmt() \n");
     curprint("/* Leaving unparseSwitchStmt() */ ");
#endif
   }

void
Unparse_ExprStmt::unparseCaseStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgCaseOptionStmt* case_stmt = isSgCaseOptionStmt(stmt);
     ROSE_ASSERT(case_stmt != NULL);

#if 0
     printf ("In unparseCaseStmt() \n");
     curprint("/* In unparseCaseStmt() */ ");
#endif

     bool saved_unparsedPartiallyUsingTokenStream = info.unparsedPartiallyUsingTokenStream();

  // DQ (12/28/2014): Test for if we have unparsed partially using the token stream. 
     if (saved_unparsedPartiallyUsingTokenStream == false)
        {
          curprint("case ");

          unparseExpression(case_stmt->get_key(), info);

       // DQ (1/31/2014): Adding support for gnu case range extension.
          if (case_stmt->get_key_range_end() != NULL)
             {
            // Note that the spaces on each side of the "..." are required to avoid interpretation 
            // of the case range as a floating point number by the gnu parser.
               curprint(" ... ");
               unparseExpression(case_stmt->get_key_range_end(), info);
             }

          curprint(":");
#if 0
          if ( (case_stmt->get_body() != NULL) && !info.SkipBasicBlock())
             {
               unparseStatement(case_stmt->get_body(), info);
             }
#endif
        }
       else
        {
          SgStatement* case_body = case_stmt->get_body();
       // unparseStatementFromTokenStream (case_stmt, case_body, e_token_subsequence_start, e_leading_whitespace_start);
          if (case_body->isCompilerGenerated() == true)
             {
               SgBasicBlock* basicBlock = isSgBasicBlock(case_body);
               ROSE_ASSERT(basicBlock != NULL);

            // DQ (3/28/2017): Eliminate warning about unused variable from Clang.
            // SgStatementPtrList::iterator first = basicBlock->get_statements().begin();

#if 0
#if 0
               printf ("Top of loop to find first non-transformation: *first = %p = %s \n",*first,(*first)->class_name().c_str());
#endif
               while (first != basicBlock->get_statements().end() && (*first)->isTransformation() == true)
                  {
#if 0
                    printf ("In loop to find first non-transformation: *first = %p = %s \n",*first,(*first)->class_name().c_str());
#endif
                    first++;
                  }

               if (first != basicBlock->get_statements().end())
                  {
                    SgStatement* firstStatementOfCompilerGeneratedBlock = *first;
                    ROSE_ASSERT(firstStatementOfCompilerGeneratedBlock != NULL);
#if 0
                    printf ("firstStatementOfCompilerGeneratedBlock = %p = %s \n",firstStatementOfCompilerGeneratedBlock,firstStatementOfCompilerGeneratedBlock->class_name().c_str());
#endif
                    unparseStatementFromTokenStream (case_stmt, firstStatementOfCompilerGeneratedBlock, e_token_subsequence_start, e_leading_whitespace_start, info);
                  }
                 else
                  {
                    printf("Error: no non-tranformation left in case option block (unparse the SgBasicBlock from the AST directly) \n");
                    ROSE_ASSERT(false);
                  }
#if 0
               if ( (case_stmt->get_body() != NULL) && !info.SkipBasicBlock())
                  {
                    unparseStatement(case_stmt->get_body(), info);
                  }
#endif
#else
            // DQ (1/22/2015): Since there is not token information inside of a compiler-generated SgBasicBlock, we have to output the whole SgCaseOptionStmt as it's associated tokens.
            // unparseStatementFromTokenStream (case_stmt, e_token_subsequence_start, e_token_subsequence_end);
               unparseStatementFromTokenStream (case_stmt, e_token_subsequence_start, e_trailing_whitespace_start, info);
#endif
             }
            else
             {
               unparseStatementFromTokenStream (case_stmt, case_body, e_token_subsequence_start, e_leading_whitespace_start, info);
             }
        }

  // DQ (1/3/2018): Put back this original behavior, because the case option statment must be a compound statement 
  // (just like a label statement, see test2017_20.c).
  // DQ (12/20/2017): Comment this out to experiment with alternative support for switch (part of new duff's device support).
  // At the very least, commenting this out permis the cases to be adjusted to have defined bodies later (if that ultimately makes sense).
#if 1
  // if(case_stmt->get_body())
  // if ( (case_stmt->get_body() != NULL) && !info.SkipBasicBlock())
     if ( (case_stmt->get_body() != NULL) && !info.SkipBasicBlock())
        {
          unparseStatement(case_stmt->get_body(), info);
        }
#else
     printf ("In unparseCaseStmt(): Modified to skip unparsing the body! \n");
#endif
#if 0
     printf ("Leaving unparseCaseStmt() \n");
     curprint("/* Leaving unparseCaseStmt() */ ");
#endif
   }


void
Unparse_ExprStmt::unparseTryStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgTryStmt* try_stmt = isSgTryStmt(stmt);
     ROSE_ASSERT(try_stmt != NULL);

     curprint ( string("try "));
  
     unp->cur.format(try_stmt->get_body(), info, FORMAT_BEFORE_NESTED_STATEMENT);
     unparseStatement(try_stmt->get_body(), info);
     unp->cur.format(try_stmt->get_body(), info, FORMAT_AFTER_NESTED_STATEMENT);
  
     SgStatementPtrList::iterator i=try_stmt->get_catch_statement_seq().begin();
     while (i != try_stmt->get_catch_statement_seq().end())
        {
          unparseStatement(*i, info);
          i++;
        }
   }


void
Unparse_ExprStmt::unparseCatchStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgCatchOptionStmt* catch_statement = isSgCatchOptionStmt(stmt);
     ROSE_ASSERT(catch_statement != NULL);

     curprint ( string("catch " ) + "(");
     if (catch_statement->get_condition())
        {
          SgUnparse_Info ninfo(info);
          ninfo.set_inVarDecl();
#if 0
       // DQ (12/8/2011): debugging catch parameter...
          curprint ( string("/* START: variable declaration */ "));
          printf ("In unparseCatchStmt(): catch_statement->get_condition() = %p = %s \n",catch_statement->get_condition(),catch_statement->get_condition()->class_name().c_str());
#endif
#if 0
          catch_statement->get_condition()->get_file_info()->display("catch condition in unparseCatchStmt(): debug");
#endif
       // DQ (5/6/2004): this does not unparse correctly if the ";" is included
          ninfo.set_SkipSemiColon();
          ninfo.set_SkipClassSpecifier();
          unparseStatement(catch_statement->get_condition(), ninfo);
#if 0
          curprint ( string("/* END: variable declaration */ "));
       // printf ("DONE: In unparseCatchStmt(): catch_statement->get_condition() = %p = %s \n",catch_statement->get_condition(),catch_statement->get_condition()->class_name().c_str());
#endif
        }

     curprint ( string(")"));
  // if (catch_statement->get_condition() == NULL) prevnode = catch_statement;

     unp->cur.format(catch_statement->get_body(), info, FORMAT_BEFORE_NESTED_STATEMENT);
     unparseStatement(catch_statement->get_body(), info);
     unp->cur.format(catch_statement->get_body(), info, FORMAT_AFTER_NESTED_STATEMENT);
   }

void
Unparse_ExprStmt::unparseDefaultStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgDefaultOptionStmt* default_stmt = isSgDefaultOptionStmt(stmt);
     ROSE_ASSERT(default_stmt != NULL);

#if 0
     printf ("In unparseDefaultStmt() \n");
     curprint("/* In unparseDefaultStmt() */ ");
#endif

     bool saved_unparsedPartiallyUsingTokenStream = info.unparsedPartiallyUsingTokenStream();

  // DQ (12/28/2014): Test for if we have unparsed partially using the token stream. 
     if (saved_unparsedPartiallyUsingTokenStream == false)
        {
          curprint ("default:");
        }
       else
        {
          SgStatement* default_body = default_stmt->get_body();
          unparseStatementFromTokenStream (default_stmt, default_body, e_token_subsequence_start, e_leading_whitespace_start, info);
        }

  // DQ (1/3/2018): Put back this original behavior, because the case option statment must be a compound statement 
  // (just like a label statement, see test2017_20.c).
  // DQ (12/20/2017): Comment this out to experiment with alternative support for switch (part of new duff's device support).
  // At the very least, commenting this out permis the cases to be adjusted to have defined bodies later (if that ultimately makes sense).
#if 1
  // if(default_stmt->get_body()) 
     if ( (default_stmt->get_body() != NULL) && !info.SkipBasicBlock())
        {
          unparseStatement(default_stmt->get_body(), info);
        }
#endif
#if 0
     printf ("Leaving unparseDefaultStmt() \n");
     curprint("/* Leaving unparseDefaultStmt() */ ");
#endif
   }

void
Unparse_ExprStmt::unparseBreakStmt(SgStatement* stmt, SgUnparse_Info& info) 
   {
     SgBreakStmt* break_stmt = isSgBreakStmt(stmt);
     ROSE_ASSERT(break_stmt != NULL);

     curprint ("break; ");
   }

void
Unparse_ExprStmt::unparseContinueStmt(SgStatement* stmt, SgUnparse_Info& info) 
   {
     SgContinueStmt* continue_stmt = isSgContinueStmt(stmt);
     ROSE_ASSERT(continue_stmt != NULL);

     curprint ("continue; ");
   }

void
Unparse_ExprStmt::unparseReturnStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgReturnStmt* return_stmt = isSgReturnStmt(stmt);
     ROSE_ASSERT(return_stmt != NULL);

     curprint("return ");
     SgUnparse_Info ninfo(info);

  // DQ (3/26/2012): Added assertion.
     ROSE_ASSERT(return_stmt->get_expression() != NULL);

#if 0
     printf ("In unparseReturnStmt(): return_stmt->get_expression() = %p = %s \n",return_stmt->get_expression(),return_stmt->get_expression()->class_name().c_str());
#endif

  // DQ (6/4/2011): Set this in case the initializer is an expression that requires name 
  // qualification (e.g. SgConstructorInitializer).  See test2005_42.C for an example.
  // ninfo.set_reference_node_for_qualification(return_stmt);

     if (return_stmt->get_expression())
        {
       // DQ (2/8/2019): Restricting output of definitions in the return statement.
          ninfo.set_SkipDefinition();

       // DQ (2/8/2019): Double check that these are all set.
          ROSE_ASSERT(ninfo.SkipClassDefinition() == true);
          ROSE_ASSERT(ninfo.SkipEnumDefinition()  == true);
          ROSE_ASSERT(ninfo.SkipDefinition()      == true);

          unparseExpression(return_stmt->get_expression(), ninfo);
        }

     if (!ninfo.SkipSemiColon())
        {
          curprint(";");
        }
   }

void
Unparse_ExprStmt::unparseGotoStmt(SgStatement* stmt, SgUnparse_Info& info) 
   {
     SgGotoStatement* goto_stmt = isSgGotoStatement(stmt);
     ROSE_ASSERT(goto_stmt != NULL);

     if (goto_stmt->get_label() != NULL)
        {
       // DQ (11/22/2017): Original code.
          curprint ( string("goto " ) + goto_stmt->get_label()->get_label().str());

        }
       else
        {
       // DQ (11/22/2017): Added suport for GNU extension for computed goto.
          curprint ("goto *");
          SgExpression* selector_expression = goto_stmt->get_selector_expression();
          ROSE_ASSERT(selector_expression != NULL);

          SgUnparse_Info ninfo(info);
          unparseExpression(selector_expression,ninfo);
        }

     if (!info.SkipSemiColon())
        {
          curprint ( string(";")); 
        }
   }


static bool
isOutputAsmOperand(SgAsmOp* asmOp)
   {
  // There are two way of evaluating if an SgAsmOp is an output operand, 
  // depending of if we are using the specific mechanism that knows 
  // records register details or the more general mechanism that records 
  // the registers as strings.  The string based mechanism lack precision 
  // and would require parsing to retrive the instruction details, but it 
  // is instruction set independent.  The more precise mechanism records 
  // the specific register codes and could in the future be interpreted
  // to be a part of the binary analysis support in ROSE.

     return (asmOp->get_recordRawAsmOperandDescriptions() == true) ? (asmOp->get_isOutputOperand() == true) : (asmOp->get_modifiers() & SgAsmOp::e_output);
   }


static std::string 
asm_escapeString(const std::string & s) 
   {
  // DQ (2/4/2014): We need a special version of this function for unparsing the asm strings.
  // The version of escapeString in util will expand '\' to be '\\' and this should not
  // be done to the "\n" and "\t" substrings.

     std::string result;
     for (size_t i = 0; i < s.length(); ++i) 
        {
          switch (s[i]) 
             {
#if 0
            // DQ (2/4/2014): I think this is a problem for asm strings (it translates "\n" 
            // substrings into "\\\n" substrings which are a problem for the assembler.
               case '\\': 
#if 1
                    printf ("In asm_escapeString(): processing \\\\ character \n");
#endif
                    result += "\\\\";
                    break;
#endif
               case '"':
#if 0
                    printf ("In asm_escapeString(): processing \\\" character \n");
#endif
                    result += "\\\"";
                    break;
               case '\a':
#if 0
                    printf ("In asm_escapeString(): processing \\a character \n");
#endif
                    result += "\\a";
                    break;
               case '\f':
#if 0
                    printf ("In asm_escapeString(): processing \\f character \n");
#endif
                    result += "\\f";
                    break;
               case '\n':
#if 0
                    printf ("In asm_escapeString(): processing \\n character \n");
#endif
                    result += "\\n";
                    break;
               case '\r':
#if 0
                    printf ("In asm_escapeString(): processing \\r character \n");
#endif
                    result += "\\r";
                    break;
               case '\t':
#if 0
                    printf ("In asm_escapeString(): processing \\t character \n");
#endif
                    result += "\\t";
                    break;
               case '\v':
#if 0
                    printf ("In asm_escapeString(): processing \\v character \n");
#endif
                    result += "\\v";
                    break;
               default:
#if 0
                    printf ("In asm_escapeString(): processing default case character \n");
#endif
                    if (isprint(s[i])) 
                       {
                         result.push_back(s[i]);
                       }
                      else 
                       {
                         std::ostringstream stream;
                         stream << '\\';
                         stream << std::setw(3) << std::setfill('0') <<std::oct << (unsigned)(unsigned char)(s[i]);
                         result += stream.str();
                       }
                    break;
             }
        }

     return result;
   }



void
Unparse_ExprStmt::unparseAsmStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // This function is called as part of the handling of the C "asm" 
  // statement.  The "asm" statement supports inline assembly in C.
  // These sorts of statements are not common in most user code 
  // (except embedded code), but are common in many system header files.

     SgAsmStmt* asm_stmt = isSgAsmStmt(stmt);
     ROSE_ASSERT(asm_stmt != NULL);

#define ASM_DEBUGGING 0

#if ASM_DEBUGGING
     printf ("In unparseAsmStmt(): stmt = %p = %s \n",stmt,stmt->class_name().c_str());
#endif

  // DQ (5/23/2015): The p_skip_unparse_asm_commands data member has been changed to be a static data member
  // so that we can support ASM statments hidden in AST islands (AST subtrees hidden in types and thus not
  // connected to the AST (since types are shared).  The use of the typeof operator in conjunction with the
  // GNU statemnet expression can permit this configuration.

  // DQ (5/19/2015): Note that sourceFile will be NULL in the case where the asm statement is in 
  // a GNU statement expression in a typeof operator. Not clear yet what to do about this case.
  // See test2015_141.c for an example of this. One solution might be to make the 
  // skip_unparse_asm_commands variable a static data member.
  // SgSourceFile* sourceFile = TransformationSupport::getSourceFile(stmt);
  // ROSE_ASSERT(sourceFile != NULL);

  // DQ (1/10/2009): The C language ASM statements are providing significant trouble, they are
  // frequently machine specific and we are compiling then on architectures for which they were 
  // not designed.  This option allows then to be read, constructed in the AST to support analysis
  // but not unparsed in the code given to the backend compiler, since this can fail. (See 
  // test2007_20.C from Linux Kernel for an example).
  // if (sourceFile->get_skip_unparse_asm_commands() == true)
     if (SgSourceFile::get_skip_unparse_asm_commands() == true)
        {
       // This is a case were we skip the unparsing of the C language ASM statements, because while 
       // we can read then into the AST to support analysis, we can not always output them correctly.  
       // This subject requires additional work.

          printf ("Warning: Sorry, C language ASM statement skipped (parsed and built in AST, but not output by the code generation phase (unparser)) \n");

       // DQ (10/25/2012): This needs to be made a bit more sophisticated (tailored to the type of associated scope in the source code).
       // DQ (10/23/2012): If we skip the ASM statment then include an expression statement to take it's place.
       // curprint ( "/* C language ASM statement skipped (in code generation phase) */ ");
       // curprint ( "/* C language ASM statement skipped (in code generation phase) */ \"compiled using -rose:skip_unparse_asm_commands\"; ");
          SgScopeStatement* associatedScope = stmt->get_scope();
          ROSE_ASSERT(associatedScope != NULL);

          if (associatedScope->containsOnlyDeclarations() == true)
             {
            // Output a simple string.
               curprint ( "/* C language ASM statement skipped (in code generation phase) */ ");
             }
            else
             {
            // We might need a expression statement to properly close off another statement (e.g. a "if" statement).
               curprint ( "/* C language ASM statement skipped (in code generation phase) */ \"compiled using -rose:skip_unparse_asm_commands\"; ");
             }

          return;
        }

#if 0
  // DQ (2/4/2014): Note that test2012_175.c demonstrates where EDG causes the IR node in ROSE to be marked 
  // as volatile and it causes an error in the generated code.

  // DQ (7/23/2006): Added support for volatile as modifier.
     if (asm_stmt->get_declarationModifier().get_typeModifier().get_constVolatileModifier().isVolatile())
        {
#if ASM_DEBUGGING
          curprint("/* output volatile keyword from unparseAsmStmt */ \n ");
#endif
          curprint("volatile ");
        }
#endif

  // Output the "asm" keyword.
  // DQ (8/31/2013): We have to output either "__asm__" or "asm" (for MSVisual C++ I think we might need "__asm").
#if 0
     string backEndCompiler = BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH;
     if (backEndCompiler == "g++" || backEndCompiler == "gcc" || backEndCompiler == "mpicc" || backEndCompiler == "mpicxx")
        {
       // curprint("__asm__ ");
          curprint("asm ");
        }
       else
        {
          curprint("asm ");
        }
#else

// DQ (2/25/2014): Note that the 4.2.4 compiler will define both BACKEND_C_COMPILER_SUPPORTS_ASM and BACKEND_C_COMPILER_SUPPORTS_UNDESCORE_ASM
// So we need to use another macro BACKEND_C_COMPILER_SUPPORTS_LONG_STRING_ASM that will work uniformally on both 4.4.7 and 4.2.4 versions of 
// the GNU compiler.  This is truely strange behavior.
// DQ (2/25/2014): This is the new support for use of "asm" or "__asm__" (which should maybe be refactored).
// #if (defined(BACKEND_C_COMPILER_SUPPORTS_ASM) && defined(BACKEND_C_COMPILER_SUPPORTS_UNDESCORE_ASM))
//    #error "Error: BACKEND_C_COMPILER_SUPPORTS_ASM and BACKEND_C_COMPILER_SUPPORTS_UNDESCORE_ASM are both defined!"
// #endif
#ifndef _MSC_VER
#if (defined(BACKEND_C_COMPILER_SUPPORTS_LONG_STRING_ASM) && defined(BACKEND_C_COMPILER_SUPPORTS_UNDESCORE_ASM))
// DQ (2/26/2014): Allow the CMake tests to pass for now.
   #warning "Warning: BACKEND_C_COMPILER_SUPPORTS_LONG_STRING_ASM and BACKEND_C_COMPILER_SUPPORTS_UNDESCORE_ASM are both defined!"
#endif

// #ifdef BACKEND_C_COMPILER_SUPPORTS_ASM
#ifdef BACKEND_C_COMPILER_SUPPORTS_LONG_STRING_ASM
     curprint("asm ");
#else
#ifdef BACKEND_C_COMPILER_SUPPORTS_UNDESCORE_ASM
     curprint("__asm__ ");
#else
   #warning "Warning: either BACKEND_C_COMPILER_SUPPORTS_LONG_STRING_ASM or BACKEND_C_COMPILER_SUPPORTS_UNDESCORE_ASM should be defined (but not both)!"

  // DQ (2/26/2014): Allow the default behavior on CMake build systems to use the GNU compiler specific version or "__asm__".
     curprint("__asm__ ");
#endif
#endif
#else
  // DQ (2/26/2014): I assume that MSVC would use the C standard representation.
     curprint("asm ");
#endif

#endif

     curprint("(");

  // DQ (7/22/2006): This IR node has been changed to have a list of SgAsmOp IR nodes
  // unparseExpression(asm_stmt->get_expr(), info);

  // printf ("unparsing asm statement = %ld \n",asm_stmt->get_operands().size());
  // Process the asm template (always the first operand)
     string asmTemplate = asm_stmt->get_assemblyCode();

#if 0
     string testString = "pxor %%mm7, %%mm7";
     printf ("In unparseAsmStmt(): testString                = %s \n",testString.c_str());
     printf ("In unparseAsmStmt(): escapeString(testString)  = %s \n",escapeString(testString).c_str());
#endif

#if ASM_DEBUGGING
     printf ("In unparseAsmStmt(): asmTemplate.length()      = %" PRIuPTR " \n",(size_t)asmTemplate.length());
#endif
#if 0
     for (size_t i=0; i < asmTemplate.length(); i++)
        {
          printf ("   --- ascii value for asmTemplate[i=%" PRIuPTR "] = %u \n",i,asmTemplate[i]);
        }
#endif

#if ASM_DEBUGGING
     printf ("In unparseAsmStmt(): asmTemplate               = %s \n",asmTemplate.c_str());
     printf ("In unparseAsmStmt(): escapeString(asmTemplate) = %s \n",asm_escapeString(asmTemplate).c_str());
#endif

  // DQ (2/4/2014): We don't want to escape this string (see test2014_83.c, test2014_84.c, and test2014_85.c).
     curprint("\"" + asm_escapeString(asmTemplate) + "\"");
  // curprint("\"" + asmTemplate + "\"");

#if ASM_DEBUGGING
     printf ("In unparseAsmStmt(): asm_stmt->get_useGnuExtendedFormat() = %s \n",asm_stmt->get_useGnuExtendedFormat() ? "true" : "false");
#endif

     if (asm_stmt->get_useGnuExtendedFormat())
        {
          size_t numOutputOperands = 0;
          size_t numInputOperands  = 0;

       // Count the number of input vs. output operands
          for (SgExpressionPtrList::const_iterator i = asm_stmt->get_operands().begin(); i != asm_stmt->get_operands().end(); ++i)
             {
               SgAsmOp* asmOp = isSgAsmOp(*i);
               ROSE_ASSERT (asmOp);
#if ASM_DEBUGGING
               printf ("asmOp->get_modifiers() = %d SgAsmOp::e_output = %d asmOp->get_isOutputOperand() = %s \n",(int)asmOp->get_modifiers(),(int)SgAsmOp::e_output,asmOp->get_isOutputOperand() ? "true" : "false");
               printf ("asmOp->get_recordRawAsmOperandDescriptions() = %s \n",asmOp->get_recordRawAsmOperandDescriptions() ? "true" : "false");
#endif
            // if (asmOp->get_modifiers() & SgAsmOp::e_output)
            // if ( (asmOp->get_modifiers() & SgAsmOp::e_output) || (asmOp->get_isOutputOperand() == true) )
               if ( isOutputAsmOperand(asmOp) == true )
                  {
                    ++numOutputOperands;
#if ASM_DEBUGGING
                    printf ("Marking as an output operand! \n");
#endif
                  }
                 else
                  {
                    ++numInputOperands;
#if ASM_DEBUGGING
                    printf ("Marking as an input operand! \n");
#endif
                  }
             }

          size_t numClobbers = asm_stmt->get_clobberRegisterList().size();

#if ASM_DEBUGGING
          printf ("In unparseAsmStmt(): numClobbers = %" PRIuPTR " \n",numClobbers);
#endif

#if ASM_DEBUGGING
          printf ("In unparseAsmStmt(): numOutputOperands = %" PRIuPTR " numInputOperands = %" PRIuPTR " numClobbers = %" PRIuPTR " \n",numOutputOperands,numInputOperands,numClobbers);
#endif

       // DQ (2/4/2014): Adding initializer (to make me feel better about this code).
          bool first = false;
          if (numInputOperands == 0 && numOutputOperands == 0 && numClobbers == 0)
             {
#if ASM_DEBUGGING
               printf ("In unparseAsmStmt(): (numInputOperands == 0 && numOutputOperands == 0 && numClobbers == 0): goto donePrintingConstraints \n");
#endif
            // DQ (9/14/2013): Output required if we branch to label (see test2013_72.c).
               curprint(" :: "); // Start of output operands

               goto donePrintingConstraints;
             }
          curprint(" : "); // Start of output operands

#if ASM_DEBUGGING
          curprint(" /* asm output operands */ "); // Debugging output
#endif

       // Record if this is the first operand so that we can surpress the "," 
          first = true;
          for (SgExpressionPtrList::const_iterator i = asm_stmt->get_operands().begin(); i != asm_stmt->get_operands().end(); ++i)
             {
               SgAsmOp* asmOp = isSgAsmOp(*i);
               ROSE_ASSERT (asmOp != NULL);
            // if (asmOp->get_modifiers() & SgAsmOp::e_output)
            // if ( (asmOp->get_modifiers() & SgAsmOp::e_output) || (asmOp->get_isOutputOperand() == true) )
               if ( isOutputAsmOperand(asmOp) == true )
                  {
                    if (!first)
                         curprint(", ");
                    first = false;
                    unparseExpression(asmOp, info);
                  }
             }

          if (numInputOperands == 0 && numClobbers == 0)
             {
#if ASM_DEBUGGING
               printf ("In unparseAsmStmt(): (numInputOperands == 0 && numClobbers == 0): goto donePrintingConstraints \n");
#endif
            // DQ (9/14/2013): Output required if we branch to label (see test2013_72.c, but this is not a good example).
            // curprint(" : "); // Start of output operands

               goto donePrintingConstraints;
             }
          curprint(" : "); // Start of input operands
#if ASM_DEBUGGING
          curprint(" /* asm input operands */ "); // Debugging output
#endif
          first = true;
          for (SgExpressionPtrList::const_iterator i = asm_stmt->get_operands().begin(); i != asm_stmt->get_operands().end(); ++i)
             {
               SgAsmOp* asmOp = isSgAsmOp(*i);
               ROSE_ASSERT (asmOp != NULL);
            // if (!(asmOp->get_modifiers() & SgAsmOp::e_output))
               if ( isOutputAsmOperand(asmOp) == false )
                  {
                    if (!first)
                         curprint(", ");
                    first = false;
                    unparseExpression(asmOp, info);
                  }
             }

          if (numClobbers == 0)
             {
#if ASM_DEBUGGING
               printf ("In unparseAsmStmt(): (numClobbers == 0): goto donePrintingConstraints \n");
#endif
            // DQ (9/14/2013): Output required if we branch to label (see test2013_72.c, but this is not a good example).
            // curprint(" : "); // Start of output operands

               goto donePrintingConstraints;
             }

          curprint(" : "); // Start of clobbers

#if ASM_DEBUGGING
          curprint(" /* asm clobbers */ "); // Debugging output
#endif
          first = true;
          for (SgAsmStmt::AsmRegisterNameList::const_iterator i = asm_stmt->get_clobberRegisterList().begin(); i != asm_stmt->get_clobberRegisterList().end(); ++i)
             {
               if (!first) curprint(", ");
               first = false;
               curprint("\"" + unparse_register_name(*i) + "\"");
             }

donePrintingConstraints: {}

#if ASM_DEBUGGING
          printf ("In unparseAsmStmt(): base of conditional block: asm_stmt->get_useGnuExtendedFormat() = %s \n",asm_stmt->get_useGnuExtendedFormat() ? "true" : "false");
#endif
        }

     curprint ( string(")"));

     if (!info.SkipSemiColon())
        {
          curprint ( string(";"));
        }

#if ASM_DEBUGGING
     printf ("Leaving unparseAsmStmt(): stmt = %p = %s \n",stmt,stmt->class_name().c_str());
#endif
   }


// DQ 11/3/2014): Adding C++11 templated typedef declaration support.
void
Unparse_ExprStmt::unparseTemplateTypedefDeclaration(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgTemplateTypedefDeclaration* templateTypedef_stmt = isSgTemplateTypedefDeclaration(stmt);
     ROSE_ASSERT(templateTypedef_stmt != NULL);

#define DEBUG_TEMPLATE_TYPEDEF 0

#if DEBUG_TEMPLATE_TYPEDEF
     printf ("In unparseTemplateTypeDefStmt() = %p \n",templateTypedef_stmt);
#endif
#if DEBUG_TEMPLATE_TYPEDEF
     curprint(" /* Calling unparseTemplateDeclarationStatment_support<SgTemplateTypedefDeclaration>() */ ");
#endif

     if (templateTypedef_stmt->get_templateParameters().empty() == false)
        {
       // DQ (2/19/2019): The support for unparsing a SgTemplateTypedefDeclaration is different enough that this function is not useful.
#if DEBUG_TEMPLATE_TYPEDEF
          curprint(" /* templateParameters FOUND: Calling unparseTemplateDeclarationStatment_support<SgTemplateTypedefDeclaration>() */ ");
#endif
          unparseTemplateDeclarationStatment_support<SgTemplateTypedefDeclaration>(stmt,info);
        }
       else
        {
#if DEBUG_TEMPLATE_TYPEDEF
          curprint(" /* templateParameters NOT found: Calling unparseTemplateDeclarationStatment_support<SgTemplateTypedefDeclaration>() */ ");
#endif

       // Unparse_ExprStmt::unparseTemplateParameterList( const SgTemplateParameterPtrList & templateParameterList, SgUnparse_Info& info, bool is_template_header)
       // bool is_template_header = false;
       // unparseTemplateParameterList(templateTypedef_stmt->get_templateParameters(),info,is_template_header);
          unparseTemplateHeader<SgTemplateTypedefDeclaration>(templateTypedef_stmt,info);

       // DQ (2/19/2019): I think this is how we detect the template parameters.
          if (templateTypedef_stmt->get_templateParameters().empty() == false)
             {
               curprint(" /* unparse template paremeters */ ");
             }

       // DQ (2/19/2019): Not clear that I want the extra "\n".
          curprint("\nusing ");

#if DEBUG_TEMPLATE_TYPEDEF
          printf ("In unparseTemplateTypeDefStmt(): templateTypedef_stmt->get_name() = %s \n",templateTypedef_stmt->get_name().str());
#endif

          curprint(templateTypedef_stmt->get_name().str());

          curprint(" = ");

          SgType* base_type = templateTypedef_stmt->get_base_type();
          ROSE_ASSERT(base_type != NULL);

#if DEBUG_TEMPLATE_TYPEDEF
          printf ("In unparseTemplateTypeDefStmt(): base_type = %p = %s \n",base_type,base_type->class_name().c_str());
#endif

          SgUnparse_Info ninfo(info);

          ROSE_ASSERT(ninfo.SkipClassDefinition() == false);
          ROSE_ASSERT(ninfo.SkipEnumDefinition()  == false);

#if DEBUG_TEMPLATE_TYPEDEF
          printf ("In unparseTemplateTypeDefStmt(): templateTypedef_stmt->get_declaration() = %p \n",templateTypedef_stmt->get_declaration());
#endif
#if DEBUG_TEMPLATE_TYPEDEF
          printf ("In unparseTemplateTypeDefStmt(): set reference_node_for_qualification: templateTypedef_stmt = %p = %s \n",templateTypedef_stmt,templateTypedef_stmt->class_name().c_str());
#endif
          ninfo.set_reference_node_for_qualification(templateTypedef_stmt);

       // DQ (2/19/2019): Cxx_tests/test2019_153.C demonstrates that a class declaration can be define in the C++11 SgTemplateTypedefDeclaration.
       // ROSE_ASSERT(templateTypedef_stmt->get_declaration() == NULL);

          if (templateTypedef_stmt->get_declaration() == NULL)
             {
               ninfo.set_SkipClassDefinition();
               ninfo.set_SkipEnumDefinition();
             }

          unp->u_type->unparseType(base_type, ninfo);

          curprint(";");
        }

#if DEBUG_TEMPLATE_TYPEDEF
     printf ("Leaving unparseTemplateTypeDefStmt() = %p \n",templateTypedef_stmt);
#endif
#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }

void
Unparse_ExprStmt::unparseNonrealDecl(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgNonrealDecl * nrdecl = isSgNonrealDecl(stmt);
     ROSE_ASSERT(nrdecl != NULL);

     printf("WARNING: Asked to unparse a non-real declaration!\n");

     curprint(nrdecl->get_name().str());
   }

void
Unparse_ExprStmt::unparseTypeDefStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgTypedefDeclaration* typedef_stmt = isSgTypedefDeclaration(stmt);
     ROSE_ASSERT(typedef_stmt != NULL);

#define DEBUG_TYPEDEF_DECLARATIONS 0

#if DEBUG_TYPEDEF_DECLARATIONS
     printf ("In unp->u_type->unparseTypeDefStmt() = %p \n",typedef_stmt);
#endif
#if DEBUG_TYPEDEF_DECLARATIONS
     curprint("\n /* In unp->u_type->unparseTypeDefStmt() */ \n");
#endif

  // DQ (10/5/2004): This is the explicitly set boolean value which indicates that a class declaration is buried inside
  // the current variable declaration (e.g. struct A { int x; } a;).  In this case we have to output the base type with
  // its definition.
     bool outputTypeDefinition = typedef_stmt->get_typedefBaseTypeContainsDefiningDeclaration();

#if DEBUG_TYPEDEF_DECLARATIONS
     printf ("In unparseTypeDefStmt(): typedef_stmt = %p outputTypeDefinition = %s \n",typedef_stmt,(outputTypeDefinition == true) ? "true" : "false");
#endif

     if (!info.inEmbeddedDecl())
        {
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS || 0
          curprint("\n/* NOT an embeddedDeclaration */ \n");
#endif
          SgClassDefinition *cdefn = isSgClassDefinition(typedef_stmt->get_parent());
          if (cdefn && cdefn->get_declaration()->get_class_type() == SgClassDeclaration::e_class)
               info.set_CheckAccess();
       // printDebugInfo("entering unp->u_sage->printSpecifier", true);
          unp->u_sage->printSpecifier(typedef_stmt, info);
          info.unset_CheckAccess();
        }
       else
        {
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS || 0
          curprint("\n/* Found an embeddedDeclaration */ \n");
#endif
        }

     SgUnparse_Info ninfo(info);

  // DQ (10/10/2006): Do output any qualified names (particularly for non-defining declarations).
  // ninfo.set_forceQualifiedNames();

  // DQ (10/5/2004): This controls the unparsing of the class definition
  // when unparsing the type within this variable declaration.
     if (outputTypeDefinition == true)
        {
       // printf ("Output the full definition as a basis for the typedef base type \n");
       // DQ (10/5/2004): If this is a defining declaration then make sure that we don't skip the definition
          ROSE_ASSERT(ninfo.SkipClassDefinition() == false);

       // DQ (12/22/2005): Enum definition should be handled here as well
          ROSE_ASSERT(ninfo.SkipEnumDefinition() == false);

       // DQ (10/14/2006): As part of new implementation of qualified names we now default to the generation of all qualified names unless they are skipped.
          ninfo.set_SkipQualifiedNames();
       // curprint ( string("\n/* Case of typedefs for outputTypeDefinition == true  */\n ";
        }
       else
        {
       // printf ("Skip output of the full definition as a basis for the typedef base type \n");
       // DQ (10/5/2004): If this is a non-defining declaration then skip the definition
          ninfo.set_SkipClassDefinition();
          ROSE_ASSERT(ninfo.SkipClassDefinition() == true);

       // DQ (12/22/2005): Enum definition should be handled here as well
          ninfo.set_SkipEnumDefinition();
          ROSE_ASSERT(ninfo.SkipEnumDefinition() == true);

       // DQ (10/14/2006): Force output any qualified names (particularly for non-defining declarations).
       // This is a special case for types of variable declarations.
       // ninfo.set_forceQualifiedNames();
       // curprint ( string("\n/* Case of typedefs, should we forceQualifiedNames -- outputTypeDefinition == false  */\n ";
        }

  // Note that typedefs of function pointers and member function pointers 
  // are quite different from ordinary typedefs and so should be handled
  // separately.

  // First look for a pointer to a function
     SgPointerType* pointerToType = isSgPointerType(typedef_stmt->get_base_type());

     SgFunctionType* functionType = NULL;
     if (pointerToType != NULL)
          functionType = isSgFunctionType(pointerToType->get_base_type());

  // DQ (2/3/2019): Adding support to unparse SgPointerMemberType with parenthesis now that they have been 
  // discovered to not be required in the unparse type code for SgPointerMemberType.  See Cxx11_tests/test2019_77.C.
     SgPointerMemberType* pointerToMemberType = isSgPointerMemberType(typedef_stmt->get_base_type());

  // DQ (9/15/2004): Added to support typedefs of member pointers
     SgMemberFunctionType* pointerToMemberFunctionType = isSgMemberFunctionType(typedef_stmt->get_base_type());

  // DQ (2/14/2019): Adding name qualification support for C++11 enum declarations in typedef types.
     SgEnumType* enumType = isSgEnumType(typedef_stmt->get_base_type());
     if (enumType != NULL)
        {
          SgDeclarationStatement* declarationReference = typedef_stmt->get_declaration();
          ROSE_ASSERT(declarationReference != NULL);
          SgEnumDeclaration* enumDeclaration = isSgEnumDeclaration(declarationReference);
          ROSE_ASSERT(enumDeclaration != NULL);
          SgEnumDeclaration* definingEnumDeclaration = isSgEnumDeclaration(enumDeclaration->get_definingDeclaration());

#if DEBUG_TYPEDEF_DECLARATIONS
          printf ("enumDeclaration->get_name() = %s \n",enumDeclaration->get_name().str());
#endif
#if DEBUG_TYPEDEF_DECLARATIONS
          curprint ( string("\n/* In unparseTypeDefStmt: enum name                                  = ") + enumDeclaration->get_name().str() + " */ \n ");
          curprint ( string("\n/* In unparseTypeDefStmt: enumDeclaration != definingEnumDeclaration = ") + ((enumDeclaration != definingEnumDeclaration) ? "true" : "false") + " */ \n ");
#endif
       // DQ (2/20/2019): If this is not the defining declaration referenced through the declarationReference then use 
       // the usual method for name qualification via the base type.
          if (enumDeclaration != definingEnumDeclaration)
             {
#if DEBUG_TYPEDEF_DECLARATIONS
               printf ("internal declarationReference != definingEnumDeclaration: so use the name qualification via the base type \n");
#endif
               enumType = NULL;
             }
            else
             {
            // If this is the defining declaration, then if it is an anonymous class then output as a type.
#if DEBUG_TYPEDEF_DECLARATIONS
               printf ("enumDeclaration->get_name() = %s \n",enumDeclaration->get_name().str());
#endif
               bool isAnonymousName = (string(enumDeclaration->get_name()).substr(0,14) == "__anonymous_0x");
               if (isAnonymousName == true)
                  {
#if DEBUG_TYPEDEF_DECLARATIONS
                    printf ("internal declarationReference == definingEnumDeclaration: isAnonymousName == true: so output enum declaration via the base type \n");
#endif
                    enumType = NULL;
                  }
             }
        }

  // DQ (2/18/2019): Adding name qualification support for class declarations in typedef types (declared in other scopes).
     SgClassType* classType = isSgClassType(typedef_stmt->get_base_type());
     if (classType != NULL)
        {
          SgDeclarationStatement* declarationReference = typedef_stmt->get_declaration();
          ROSE_ASSERT(declarationReference != NULL);
          SgClassDeclaration* classDeclaration = isSgClassDeclaration(declarationReference);
          ROSE_ASSERT(classDeclaration != NULL);
          SgClassDeclaration* definingClassDeclaration = isSgClassDeclaration(classDeclaration->get_definingDeclaration());

#if DEBUG_TYPEDEF_DECLARATIONS
          printf ("classDeclaration->get_name() = %s \n",classDeclaration->get_name().str());
#endif
#if DEBUG_TYPEDEF_DECLARATIONS
          curprint ( string("\n/* In unparseTypeDefStmt: class name                                   = ") + classDeclaration->get_name().str() + " */ \n ");
          curprint ( string("\n/* In unparseTypeDefStmt: classDeclaration != definingClassDeclaration = ") + ((classDeclaration != definingClassDeclaration) ? "true" : "false") + " */ \n ");
#endif
       // DQ (2/20/2019): If this is not the defining declaration referenced through the declarationReference then use 
       // the usual method for name qualification via the base type.
          if (classDeclaration != definingClassDeclaration)
             {
#if DEBUG_TYPEDEF_DECLARATIONS
               printf ("internal declarationReference != definingClassDeclaration: so use the name qualification via the base type \n");
#endif
               classType = NULL;
             }
            else
             {
            // If this is the defining declaration, then if it is an anonymous class then output as a type.
#if DEBUG_TYPEDEF_DECLARATIONS
               printf ("classDeclaration->get_name() = %s \n",classDeclaration->get_name().str());
#endif
               bool isAnonymousName = (string(classDeclaration->get_name()).substr(0,14) == "__anonymous_0x");
               if (isAnonymousName == true)
                  {
#if DEBUG_TYPEDEF_DECLARATIONS
                    printf ("internal declarationReference == definingClassDeclaration: isAnonymousName == true: so output class declaration via the base type \n");
#endif
                    classType = NULL;
                  }
             }
        }

#if DEBUG_TYPEDEF_DECLARATIONS
     printf ("In unp->u_type->unparseTypedef: functionType                = %p \n",functionType);
     printf ("In unp->u_type->unparseTypedef: pointerToMemberType         = %p \n",pointerToMemberType);
     printf ("In unp->u_type->unparseTypedef: pointerToMemberFunctionType = %p \n",pointerToMemberFunctionType);
     printf ("In unp->u_type->unparseTypedef: enumType                    = %p \n",enumType);
#endif

  // DQ (9/22/2004): It is not clear why we need to handle this case with special code.
  // We are only putting out the return type of the function type (for functions or member functions).
  // It seems that the reason we handle function pointers separately is that typedefs of non function 
  // pointers could include the complexity of class declarations with definitions and separating the 
  // code for function pointers allows for easier debugging.  When typedefs of defining class 
  // declarations is fixed we might be able to unify these separate cases.

  // DQ (2/14/2019): Adding name qualification support for C++11 enum declarations in typedef types.
  // DQ (2/3/2019): See if this is a better branch for handling the SgPointerMemberType.
  // This handles pointers to functions and member function (but not pointers to members!)
  // if ( (functionType != NULL) || (pointerToMemberType != NULL) )
  // if ( (functionType != NULL) || (pointerToMemberFunctionType != NULL) )
  // if ( (functionType != NULL) || (pointerToMemberFunctionType != NULL) || (pointerToMemberType != NULL) )
  // if ( (functionType != NULL) || (pointerToMemberFunctionType != NULL) || (enumType != NULL) )

  // DQ (2/20/2019): Removing the use of classType allows the name qualification to be handled properly thorugh the type.
  // But I need to check why this was added in the first place (what breaks).
  // if ( (functionType != NULL) || (pointerToMemberFunctionType != NULL) || (enumType != NULL) || (classType != NULL))
  // if ( (functionType != NULL) || (pointerToMemberFunctionType != NULL) || (enumType != NULL) )
  // if ( (functionType != NULL) || (pointerToMemberFunctionType != NULL) || (enumType != NULL) || (classType != NULL))
  // if ( (functionType != NULL) || (pointerToMemberFunctionType != NULL) || (enumType != NULL) )
     if ( (functionType != NULL) || (pointerToMemberFunctionType != NULL) || (enumType != NULL) || (classType != NULL))
        {
       // Newly implemented case of typedefs for function and member function pointers
#if DEBUG_TYPEDEF_DECLARATIONS
          printf ("In unparseTypeDefStmt(): case of typedefs for function and member function pointers \n");
#endif
#if OUTPUT_DEBUGGING_FUNCTION_INTERNALS || 0
          curprint("\n/* Case of typedefs for function and member function pointers */ \n");
#endif
          ninfo.set_SkipFunctionQualifier();
          curprint("typedef ");

       // Specify that only the first part of the type shold be unparsed 
       // (this will permit the introduction of the name into the member
       // function pointer declaration)
          ninfo.set_isTypeFirstPart();

#if OUTPUT_DEBUGGING_UNPARSE_INFO || 0
          curprint ( string("\n/* " ) + ninfo.displayString("After return Type now output the base type (first part then second part)") + " */ \n");
#endif

       // The base type contains the function po9inter type
          SgType *btype = typedef_stmt->get_base_type();

// #if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
#if DEBUG_TYPEDEF_DECLARATIONS
          curprint("\n/* Output base type (first part) (for functionType or pointerToMemberFunctionType) */ \n");
#endif

#if DEBUG_TYPEDEF_DECLARATIONS
          printf ("In unparseTypeDefStmt(): btype = %p = %s \n",btype,btype->class_name().c_str());
#endif

       // DQ (1/10/2007): Set the current declaration statement so that if required we can do
       // context dependent searches of the AST to determine if name qualification is required.
       // This is done now for the case of function and member function typedefs.
          SgUnparse_Info ninfo_for_type(ninfo);
          ninfo_for_type.set_declstatement_ptr(typedef_stmt);

#if 0
       // DQ (6/2/2011): Note that this might cause name qualification to be uniform for all subtypes.
       // We don't presently have a better way to handle this since types are shared and even types
       // that reference types are shared.  Need to think about this (similar problem to throw 
       // expression lists).
          ninfo_for_type.set_reference_node_for_qualification(typedef_stmt);
#else
       // DQ (2/18/2019): The name qualification support sets the name qualification for enums 
       // declarations using the enum declaration, so the typedef declaration is not appropriate 
       // to use with set_reference_node_for_qualification().
          SgDeclarationStatement* declaration = typedef_stmt->get_declaration();
          if (declaration != NULL)
             {
#if DEBUG_TYPEDEF_DECLARATIONS
               printf ("Found declaration statment in typedef declaration (need to use it as reference node for qualification \n");
#endif
               ninfo_for_type.set_reference_node_for_qualification(declaration);
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }
            else
             {
            // DQ (6/2/2011): Note that this might cause name qualification to be uniform for all subtypes.
            // We don't presently have a better way to handle this since types are shared and even types
            // that reference types are shared.  Need to think about this (similar problem to throw 
            // expression lists).
#if DEBUG_TYPEDEF_DECLARATIONS
               printf ("typedef_stmt->get_declaration() == NULL: using typedef_stmt as reference_node_for_qualification \n");
#endif
               ninfo_for_type.set_reference_node_for_qualification(typedef_stmt);
             }
#endif
       // Only pass the ninfo_for_type to support name qualification of the base type.
       // unp->u_type->unparseType(btype, ninfo);
          unp->u_type->unparseType(btype, ninfo_for_type);

// #if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
#if DEBUG_TYPEDEF_DECLARATIONS
          curprint("\n/* Done: Output base type (first part) */ \n");
#endif

          curprint(typedef_stmt->get_name().str());

       // Now unparse the second part of the typedef
// #if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
#if DEBUG_TYPEDEF_DECLARATIONS
          curprint("\n/* Output base type (second part) */ \n");
#endif

          ninfo.set_isTypeSecondPart();
          unp->u_type->unparseType(btype, ninfo);

// #if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
#if DEBUG_TYPEDEF_DECLARATIONS
          curprint("\n/* Done: Output base type (second part) */ \n");
#endif

#if 0
          if (!info.SkipSemiColon())
             {
               curprint ( string(";"));
             }
#endif
        }
       else
        {
       // previously implemented case of unparsing the typedef does not handle 
       // function pointers properly (so they are handled explicitly above!)
#if DEBUG_TYPEDEF_DECLARATIONS
          printf ("Not a typedef for a function type or member function type \n");
#endif

          ninfo.set_SkipFunctionQualifier();
          curprint ("typedef ");

          ninfo.set_SkipSemiColon();
          SgType *btype = typedef_stmt->get_base_type();

       // DQ (2/3/2019): Make the unparse_info object so that the unparseType() can output the required parenthesis when the base type is a pointer to member.
          if ( pointerToMemberType != NULL )
             {
               ninfo.set_inTypedefDecl();
             }

          ninfo.set_isTypeFirstPart();

       // DQ (10/11/2006): As part of new implementation of qualified names we now default to the generation of all qualified names unless they are skipped.
       // ninfo.set_SkipQualifiedNames();
       // curprint ( string("\n/* Commented out call to ninfo.set_SkipQualifiedNames() */\n ";

       // printf ("Before first part of base type (type = %p = %s) \n",btype,btype->sage_class_name());
       // ninfo.display ("Before first part of type in unp->u_type->unparseTypeDefStmt()");

          SgUnparse_Info ninfo_for_type(ninfo);

       // DQ (1/10/2007): Set the current declaration statement so that if required we can do
       // context dependent searches of the AST to determine if name qualification is required.
          ninfo_for_type.set_declstatement_ptr(NULL);
          ninfo_for_type.set_declstatement_ptr(typedef_stmt);

          if (typedef_stmt->get_requiresGlobalNameQualificationOnType() == true)
             {
#if DEBUG_TYPEDEF_DECLARATIONS
               printf ("In Unparse_ExprStmt::unp->u_type->unparseTypedefStmt(): This base type requires a global qualifier \n");
               curprint("\n/* This base type requires a global qualifier, calling set_requiresGlobalNameQualification() */ \n");
#endif
            // ninfo_for_type.set_forceQualifiedNames();
               ninfo_for_type.set_requiresGlobalNameQualification();
             }
#if 0
#if 0
       // DQ (8/23/2006): This was used to specify global qualification separately from the more general name 
       // qualification mechanism, however having two mechanisms is a silly level of redundancy so we now use 
       // just one (the more general one) even though it is only used for global name qualification.
       // DQ (8/20/2006): We can't mark the SgType (since it is shared), and we can't mark the SgInitializedName,
       // so we have to carry the information that we should mark the type in the SgVariableDeclaration.
       // printf ("vardecl_stmt->get_requiresNameQualification() = %s \n",vardecl_stmt->get_requiresNameQualification() ? "true" : "false");
          if (typedef_stmt->get_requiresGlobalNameQualificationOnType() == true)
             {
            // Output the name qualification for the type in the variable declaration.
            // But we have to do so after any modifiers are output, so in unp->u_type->unparseType().
               printf ("In Unparse_ExprStmt::unp->u_type->unparseTypedefStmt(): This base type requires a global qualifier \n");
            // ninfo2.set_forceQualifiedNames();
               ninfo_for_type.set_requiresGlobalNameQualification();

           // ninfo_for_type.display("This base type requires a global qualifier");
             }
#else
       // DQ (1/10/2007): Actually we can't do this since test2007_15.C demonstrates where 
       // for the __FILE_IO in a typedef is context sensitive as to if it requires or accepts 
       // name qualification.
       // DQ (1/10/2007): I think we want to force the use of qualified names generally 
       // (over qualification can't be avoided since it is sometime required).
       // printf ("SKIP Forcing the use of qualified names for the base type of typedefs (independent of the setting of the typedef_stmt->get_requiresGlobalNameQualificationOnType() \n");
       // ninfo_for_type.set_forceQualifiedNames();
#endif
#endif

#if 0
       // DQ (1/10/2007): If this is C++ then we can drop the class specifier (and it is good 
       // to do so to avoid having inappropriate name qualification cause generation of new 
       // types in the generated code which masks errors we want to trap).
          if (SageInterface::is_Cxx_language() == true)
             {
            // BUG: Currently we can't do this because the information in EDG is unavailable as to 
            // when the class specifier is used. So we have to always output it in the generated code.
            // At worst this appears to only mask errors in the name qualification of base types for 
            // typedefs. Eventually we want to fix this.
               ninfo_for_type.set_SkipClassSpecifier();
             }
#endif

       // curprint ( string("\n/* unp->u_type->unparseTypeDefStmt: Before first part of type */ \n";
// #if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
#if DEBUG_TYPEDEF_DECLARATIONS
          curprint("\n/* Output base type (first part) (Not a typedef for a function type or member function type) */ \n");
#endif

       // DQ (5/30/2011): Added support for name qualification.
          ninfo_for_type.set_reference_node_for_qualification(typedef_stmt);
          ROSE_ASSERT(ninfo_for_type.get_reference_node_for_qualification() != NULL);

       // DQ (5/14/2011): Added support for newer name qualification implementation.
          ninfo_for_type.set_name_qualification_length(typedef_stmt->get_name_qualification_length_for_base_type());
          ninfo_for_type.set_global_qualification_required(typedef_stmt->get_global_qualification_required_for_base_type());
          ninfo_for_type.set_type_elaboration_required(typedef_stmt->get_type_elaboration_required_for_base_type());

#if 1
       // DQ (7/28/2012): This is similar to code in the variable declaration unparser function and so might be refactored.
       // DQ (7/28/2012): If this is a declaration associated with a declaration list from a previous (the last statement) typedef
       // then output the name if that declaration had an un-named type (class or enum).
#if DEBUG_TYPEDEF_DECLARATIONS
          printf ("In unparseTypedefStmt(): typedef_stmt->get_isAssociatedWithDeclarationList() = %s \n",typedef_stmt->get_isAssociatedWithDeclarationList() ? "true" : "false");
#endif
          if (typedef_stmt->get_isAssociatedWithDeclarationList() == true)
             {
            // DQ (8/2/2012): Make this consistant with the design for the variable declarations.
            // This is an alternative to permit the unparsing of the type to control the name output for types.
            // But it would have to be uniform that all the pieces of the first part of the type would have to 
            // be output.  E.g. "*" in "*X".
               ninfo_for_type.set_PrintName();
               unp->u_type->unparseType(btype, ninfo_for_type);
             }
            else
             {
            // DQ (7/28/2012): Output the type if this is not associated with a declaration list from a previous declaration.
            // unp->u_type->unparseType(btype, ninfo);
#if 0
               ninfo_for_type.display("Output base type (first part)");
#endif
#if DEBUG_TYPEDEF_DECLARATIONS
               printf ("In unparseTypedefStmt(): (first part): btype = %p = %s \n",btype,btype->class_name().c_str());
#endif
// #if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
#if DEBUG_TYPEDEF_DECLARATIONS
               curprint("\n/* Output a non function pointer typedef (Not a typedef for a function type or member function type) */ \n");
#endif

            // DQ (5/7/2013): Using ninfoallows test2013_156.C to work.
            // unp->u_type->unparseType(btype, ninfo_for_type);
            // unp->u_type->unparseType(btype, ninfo);
               unp->u_type->unparseType(btype, ninfo_for_type);
             }
#else

#error "DEAD CODE!"

          unp->u_type->unparseType(btype, ninfo_for_type);
#endif

// #if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
#if DEBUG_TYPEDEF_DECLARATIONS
          curprint("\n/* Done: Output base type (first part) */ \n");
#endif

       // curprint ( string("\n/* unp->u_type->unparseTypeDefStmt: After first part of type */ \n";
       // printf ("After first part of type \n");

       // DQ (10/11/2006): As part of new implementation of qualified names we now default to the generation of all qualified names unless they are skipped.
       // ninfo.unset_SkipQualifiedNames();

       // DQ (10/7/2004): Moved the output of the name to before the output of the second part of the type
       // to handle the case of "typedef A* A_Type[10];" (see test2004_104.C).

#if 0
       // DQ (5/26/2014): The fix to output a space in unparseTemplateArgumentList() when we have an empty template argument list means
       // that we no longer require this space to be output here (which was a problem for more general non template test codes that
       // were using diff against a gold standard for there tests (in the ROSE regression tests).
       // DQ (5/17/2014): With fixes to the determination of template declarations and template instantiation declarations, test2004_145.C
       // (and associated test code test2014_58.C) now require  whitespace between the typedef's base type and the typedef's name.
          curprint(" ");
#endif

#if DEBUG_TYPEDEF_DECLARATIONS
          curprint("\n/* Output typedef name */ \n");
#endif
       // The name of the type (X, in the following example) has to appear after the 
       // declaration. Example: struct { int a; } X;
          curprint(typedef_stmt->get_name().str());
       // curprint(string("/* before 2nd part */ ") + typedef_stmt->get_name().str());

          ninfo.set_isTypeSecondPart();

       // printf ("Before 2nd part of type \n");
       // curprint ( string("\n/* unp->u_type->unparseTypeDefStmt: Before second part of type */ \n";
// #if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
#if DEBUG_TYPEDEF_DECLARATIONS
          curprint("\n/* Output base type (second part) */ \n");
#endif

          unp->u_type->unparseType(btype, ninfo);
       // unp->u_type->unparseType(btype, ninfo_for_type);
       // unp->u_type->unparseType(btype, ninfo_for_type);

// #if OUTPUT_DEBUGGING_FUNCTION_INTERNALS
#if DEBUG_TYPEDEF_DECLARATIONS
          curprint("\n/* Done: Output base type (second part) */ \n");
#endif
       // curprint ( string("\n/* unp->u_type->unparseTypeDefStmt: After second part of type */ \n";
       // printf ("After 2nd part of type \n");

#if 0
          if (!info.SkipSemiColon())
             {
               curprint ( string(";"));
             }
#endif
       // DQ (2/3/2019): Unset this to avoid use outside of typedef unparsing.
          ninfo.unset_inTypedefDecl();
        }

#if DEBUG_TYPEDEF_DECLARATIONS
     printf ("In unparseTypedefStmt(): outputTypeDefinition = %s \n",outputTypeDefinition ? "true" : "false");
#endif
     if (outputTypeDefinition == true)
        {
#if 0
          printf ("In unparseTypedefStmt(): typedef_stmt = %p = %s \n",typedef_stmt,typedef_stmt->class_name().c_str());
#endif
          unparseTypeAttributes(typedef_stmt);
        }

  // DQ (2/26/2013): Output any attributes.
     unp->u_sage->printAttributes(typedef_stmt,info);

     if (!info.SkipSemiColon())
        {
          curprint(";");
        }

  // info.display ("At base of unp->u_type->unparseTypeDefStmt()");
#if DEBUG_TYPEDEF_DECLARATIONS
     printf ("Leaving unparseTypedefStmt() \n");
     curprint ("/* Leaving unparseTypedefStmt */ \n");
#endif
   }


void
Unparse_ExprStmt::unparseTemplateDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // This function is called for the processing of template declarations (SgTemplateDeclaration).
  // However a newer design as of 11/19/2011 is using separate derived classes (IR nodes) from
  // SgTemplateDeclaration to represent SgTemplateClassDeclaration (and template functions, 
  // template member functions, etc).  This work is part of the processing of the template
  // declarations into their own AST (different from the EDG 3.3 template support).

     SgTemplateDeclaration* template_stmt = isSgTemplateDeclaration(stmt);
     ROSE_ASSERT(template_stmt != NULL);

  // printf ("In unparseTemplateDeclStmt(template_stmt = %p) \n",template_stmt);
  // template_stmt->get_declarationModifier().display("In unparseTemplateDeclStmt()");

  // DQ (11/20/2011): Detect derived classes that should not be used in the new EDG 4.x support in ROSE.
     if (isSgTemplateClassDeclaration(stmt) != NULL)
        {
#if 0
          printf ("Note: Using the saved template declaration as a string to output the template declaration (AST for the template declaration is also now available in the AST) \n");
#endif
#if 0
          printf ("Exiting in unparseTemplateDeclStmt() \n");
          ROSE_ASSERT(false);
#endif
        }

  // SgUnparse_Info ninfo(info);

  // Check to see if this is an object defined within a class
     ROSE_ASSERT (template_stmt->get_parent() != NULL);
     SgClassDefinition *cdefn = isSgClassDefinition(template_stmt->get_parent());
     if (cdefn != NULL)
        {
          if (cdefn->get_declaration()->get_class_type() == SgClassDeclaration::e_class)
             {
               info.set_CheckAccess();
             }
        }

  // SgUnparse_Info saved_ninfo(ninfo);
  // this call has been moved below, after we indent
  // unp->u_sage->printSpecifier2(vardecl_stmt, ninfo);

  // Setup the SgUnparse_Info object for this statement
  // ninfo.unset_CheckAccess();
  // info.set_access_attribute(ninfo.get_access_attribute());

  // info.display("In unparseTemplateDeclStmt()");

  // Output access modifiers
     unp->u_sage->printSpecifier1(template_stmt, info);

  // printf ("template_stmt->get_string().str() = %s \n",template_stmt->get_string().str());

  // DQ (1/21/2004): Use the string class to simplify the previous version of the code
     string templateString = template_stmt->get_string().str();

  // DQ (4/29/2004): Added support for "export" keyword (not supported by g++ yet)
     if (template_stmt->get_declarationModifier().isExport())
          curprint ( string("export "));

#if 0
  // DQ (11/18/2004): Added support for qualified name of template declaration!
  // But it appears that the qualified name is included within the template text string so that 
  // we should not output the qualified name spearately!
     SgName nameQualifier = unp->u_type->unp->u_name->generateNameQualifier( template_stmt , info );
     printf ("In unparseTemplateDeclStmt(): nameQualifier (from unp->u_type->unp->u_name->generateNameQualifier function) = %s \n",nameQualifier.str());
  // curprint ( nameQualifier.str();
#endif

  // printf ("template_stmt->get_template_kind() = %d \n",template_stmt->get_template_kind());
     switch (template_stmt->get_template_kind())
        {
          case SgTemplateDeclaration::e_template_class :
          case SgTemplateDeclaration::e_template_m_class :
          case SgTemplateDeclaration::e_template_function :
          case SgTemplateDeclaration::e_template_m_function :
          case SgTemplateDeclaration::e_template_m_data :
             {
            // printf ("debugging 64 bit bug: templateString = %s \n",templateString.c_str());
               if (templateString.empty() == true)
                  {
                 // DQ (12/22/2006): This is typically a template member class (see test2004_128.C and test2004_138.C).
                 // It is not clear to me why the names are missing, though perhaps they have not been computed yet 
                 // (until the templated clas is instantiated)!

                 // DQ (11/27/2011): Uncommented for debugging new EDG 4.x connection.
                    printf ("Warning: templateString name is empty in Unparse_ExprStmt::unparseTemplateDeclStmt() \n");
                    printf ("     template_stmt->get_template_kind() = %d \n",template_stmt->get_template_kind());
                  }
            // ROSE_ASSERT(templateString.empty() == false);

               curprint ( string("\n" ) + templateString);
               break;
             }

          case SgTemplateDeclaration::e_template_none :
            // printf ("Do we need this extra \";\"? \n");
            // curprint ( templateString + ";";
               printf ("Error: SgTemplateDeclaration::e_template_none found (not sure what to do here) \n");
               ROSE_ASSERT (false);
               break;

          default:
               printf ("Error: default reached \n");
               ROSE_ASSERT (false);
        }
   }


void
Unparse_ExprStmt::unparseTemplateClassDefnStmt(SgStatement* stmt_, SgUnparse_Info& info)
   {
     SgTemplateClassDefinition *stmt = isSgTemplateClassDefinition(stmt_);
     assert(stmt!=NULL);
     unparseTemplateClassDeclStmt(stmt->get_declaration(), info);
   }


void
Unparse_ExprStmt::unparseTemplateClassDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
#if 1
     unparseTemplateDeclarationStatment_support<SgTemplateClassDeclaration>(stmt,info);
#else
     SgTemplateClassDeclaration* template_stmt = isSgTemplateClassDeclaration(stmt);
     ROSE_ASSERT(template_stmt != NULL);

#if 0
     printf ("Note: Using the saved template declaration as a string to output the template declaration (AST for the template declaration is also now available in the AST) \n");
#endif

#error "DEAD CODE!"

  // Check to see if this is an object defined within a class
     ROSE_ASSERT (template_stmt->get_parent() != NULL);
     SgClassDefinition *cdefn = isSgClassDefinition(template_stmt->get_parent());
     if (cdefn != NULL)
        {
          if (cdefn->get_declaration()->get_class_type() == SgClassDeclaration::e_class)
             {
               info.set_CheckAccess();
             }
        }

#error "DEAD CODE!"

  // Output access modifiers
     unp->u_sage->printSpecifier1(template_stmt, info);

  // printf ("template_stmt->get_string().str() = %s \n",template_stmt->get_string().str());

  // DQ (1/21/2004): Use the string class to simplify the previous version of the code
     string templateString = template_stmt->get_string().str();

#error "DEAD CODE!"

  // DQ (4/29/2004): Added support for "export" keyword (not supported by g++ yet)
     if (template_stmt->get_declarationModifier().isExport())
        {
          curprint(string("export "));
        }

  // printf ("template_stmt->get_template_kind() = %d \n",template_stmt->get_template_kind());
     curprint ( string("\n" ) + templateString);
#endif
   }


void
Unparse_ExprStmt::unparseTemplateFunctionDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // DQ (8/6/2012): Unparse the associated comments.
  // We can't unparse comments in the templae declarations until we stop using saved string form 
  // of the template declaration.  This will be done in a later version of the release of the
  // new template support.  In the mean time we have to supress attaching CPP directives to the
  // inside of template declarations.

     unparseTemplateDeclarationStatment_support<SgTemplateFunctionDeclaration>(stmt,info);
   }


void
Unparse_ExprStmt::unparseTemplateMemberFunctionDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
#if 0
     printf ("In unparseTemplateMemberFunctionDeclStmt(stmt = %p) \n",stmt);
     curprint("/* In unparseTemplateMemberFunctionDeclStmt() */ \n");
#endif

     unparseTemplateDeclarationStatment_support<SgTemplateMemberFunctionDeclaration>(stmt,info);

  // DQ (5/28/2019): If there are any attached CPP directives then unparse them.
  // This will cause then to be output twice.
  // unparseAttachedPreprocessingInfo(stmt, info, PreprocessingInfo::after);

#if 0
     printf ("DONE: In unparseTemplateMemberFunctionDeclStmt(stmt = %p) \n",stmt);
     curprint("/* DONE: In unparseTemplateMemberFunctionDeclStmt() */ \n");
#endif
   }


void
Unparse_ExprStmt::unparseTemplateVariableDeclStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
  // DQ (1/3/2016): Present this function and the associated SgTemplateVariableDeclaration IR node 
  // is being used for both the template and the instanatiation of variables.  We might want to
  // have an IR node specific to template variable instantition.

#if 0
     printf ("In Unparse_ExprStmt::unparseTemplateVariableDeclStmt(): stmt = %p = %s \n",stmt,stmt->class_name().c_str());
#endif

     unparseTemplateDeclarationStatment_support<SgTemplateVariableDeclaration>(stmt,info);
   }

template<class T>
void
Unparse_ExprStmt::unparseTemplateHeader(T* decl, SgUnparse_Info& info) {
  if (!decl->get_templateParameters().empty()) {
    curprint("template ");
    SgTemplateParameterPtrList tlist =  decl->get_templateParameters ();
    Unparse_ExprStmt::unparseTemplateParameterList (tlist, info, true);
    curprint("\n");
  }
}

std::string 
replaceString(std::string subject, const std::string& search, const std::string& replace) 
   {
     size_t pos = 0;
     while((pos = subject.find(search, pos)) != std::string::npos) 
        {
          subject.replace(pos, search.length(), replace);
          pos += replace.length();
        }

     return subject;
   }

template<class T>
void
Unparse_ExprStmt::unparseTemplateDeclarationStatment_support(SgStatement* stmt, SgUnparse_Info& info)
   {
     ROSE_ASSERT(stmt != NULL);

#if 0
      printf ("In unparseTemplateDeclarationStatment_support(stmt = %p = %s) \n",stmt,stmt->class_name().c_str());
#endif
#if 0
     curprint("/* In unparseTemplateDeclarationStatment_support() */ \n");
#endif

     T* template_stmt = dynamic_cast<T*>(stmt);
     ROSE_ASSERT(template_stmt != NULL);

#if 0
     printf ("Note: In unparseTemplateDeclarationStatment_support(): Using the saved template declaration as a string to output the template declaration (AST for the template declaration is also now available in the AST) \n");
#endif

#if 0
     curprint("/* In unparseTemplateDeclarationStatment_support(): calling unparseAttachedPreprocessingInfo() */ \n");
#endif

  // DQ (1/28/2013): This helps handle cases such as "#if 1 void foo () #endif { }"
     unparseAttachedPreprocessingInfo(template_stmt, info, PreprocessingInfo::inside);

#if 0
     curprint("/* In unparseTemplateDeclarationStatment_support(): DONE: calling unparseAttachedPreprocessingInfo() */ \n");
#endif

  // Check to see if this is an object defined within a class
     ROSE_ASSERT (template_stmt->get_parent() != NULL);
     SgClassDefinition *cdefn = isSgClassDefinition(template_stmt->get_parent());
     if (cdefn != NULL)
        {
          if (cdefn->get_declaration()->get_class_type() == SgClassDeclaration::e_class)
             {
               info.set_CheckAccess();
             }
        }

  // Output access modifiers
     unp->u_sage->printSpecifier1(template_stmt, info);

  // DQ (4/29/2004): Added support for "export" keyword (not supported by g++ yet)
     if (template_stmt->get_declarationModifier().isExport())
        {
          curprint(string("export "));
        }

     bool string_represents_function_body = false;
     SgTemplateClassDeclaration*          templateClassDeclaration          = isSgTemplateClassDeclaration(stmt);
     SgTemplateFunctionDeclaration*       templateFunctionDeclaration       = isSgTemplateFunctionDeclaration(stmt);
     SgTemplateMemberFunctionDeclaration* templateMemberFunctionDeclaration = isSgTemplateMemberFunctionDeclaration(stmt);
     SgTemplateVariableDeclaration*       templateVariableDeclaration       = isSgTemplateVariableDeclaration(stmt);
     SgTemplateTypedefDeclaration*        templateTypedefDeclaration        = isSgTemplateTypedefDeclaration(stmt);
     if (templateFunctionDeclaration != NULL)
        {
       // printf ("This is a SgTemplateFunctionDeclaration \n");
          string_represents_function_body = templateFunctionDeclaration->get_string_represents_function_body();
        }
       else
        {
          if (templateMemberFunctionDeclaration != NULL)
             {
#if 0
               printf ("This is a SgTemplateMemberFunctionDeclaration \n");
#endif
               string_represents_function_body = templateMemberFunctionDeclaration->get_string_represents_function_body();
            // template_function_name = templateMemberFunctionDeclaration->get_name();
#if 0
               printf ("This is a SgTemplateMemberFunctionDeclaration:  string_represents_function_body = %s \n",string_represents_function_body ? "true" : "false");
#endif
#if 0
            // DQ (1/20/2016): Added mechanism to output the EDG normalized template member functions (unclear if this will work).
               if (string_represents_function_body == false)
                  {
                    unparseMFuncDeclStmt(templateMemberFunctionDeclaration, info);
                  }
#endif
             }
            else
             {
               if (templateVariableDeclaration != NULL)
                  {
                 // DQ (7/6/2015): Markus suggested that this might be an unhandled case, but it is not clear it needs to be processed, for now let's detect it.
#if 0
                    printf ("Note: In unparseTemplateDeclarationStatment_support(): Found a SgTemplateVariableDeclaration = %p = %s (not handled) \n",stmt,stmt->class_name().c_str());
#endif
#if 0
                    printf ("templateVariableDeclaration->get_string().is_null() = %s \n",templateVariableDeclaration->get_string().is_null() ? "true" : "false");
                    printf ("templateVariableDeclaration->get_string() = %s \n",templateVariableDeclaration->get_string().str());
#endif

                 // DQ (1/4/2016): Note that this is the case of a instantiation for a template variable, we don't yet have 
                 // a concept like this in the ROSE IR.  It would be just a SgVariableDeclaration, I think; however we isolate 
                 // it out as a SgTemplateVariabelDeclaration and just don't have an associated string for the template and 
                 // it is put into both the instantiation of the template class and the global scope.  We supress it from being 
                 // output in the global scope (since that does not compile and is somewhat redundant with output in the class, 
                 // except that it is what would be required for a non-template class).
                    if (templateVariableDeclaration->get_string().is_null() == true)
                       {
#if 0
                         printf ("Calling the base class support function unparseVarDeclStmt() to unparse the variable declaration as a non-template \n");
                         printf ("templateVariableDeclaration->get_parent() = %p = %s \n",templateVariableDeclaration->get_parent(),templateVariableDeclaration->get_parent()->class_name().c_str());
#endif
                      // DQ (1/4/2016): Only output the variable declaration that is NOT in the global scope (where it exists).
                      // unparseVarDeclStmt(templateVariableDeclaration,info);
                         if (isSgGlobal(templateVariableDeclaration->get_parent()) == NULL)
                            {
                              unparseVarDeclStmt(templateVariableDeclaration,info);
                            }
#if 0
                         printf ("DONE: Calling the base class support function unparseVarDeclStmt() to unparse the variable declaration as a non-template \n");
#endif
                       }
                  }
                 else
                  {
                 // This is likely a SgTemplateClassDeclaration???
                    if (isSgTemplateClassDeclaration(stmt) == NULL)
                       {
                      // DQ (9/12/2016): We want to know if this is something other than a SgTemplateClassDeclaration or SgTemplateTypedefDeclaration (C++11 feature).
                         if (templateTypedefDeclaration == NULL)
                            {
                              printf ("Note: In unparseTemplateDeclarationStatment_support(): What is this if not a template function or template member function stmt = %p = %s \n",stmt,stmt->class_name().c_str());
                            }
                           else
                            {
                           // DQ (2/19/2019): Added comment and changed predicate to use templateTypedefDeclaration and avoid compiler warning.
#if 0
                              printf ("Is there some support required for templateTypedefDeclaration = %p  \n",templateTypedefDeclaration);
#endif
#if 0
                              printf ("Exiting as a test! \n");
                              ROSE_ASSERT(false);
#endif
                            }
                       }
                  }
             }
        }

#if 0
     printf ("In unparseTemplateDeclarationStatment_support(): string_represents_function_body = %s \n",string_represents_function_body ? "true" : "false");
#endif

#if 0
     printf ("template_stmt->get_string().str() = %s \n",template_stmt->get_string().str());
#endif

  // DQ (1/21/2004): Use the string class to simplify the previous version of the code
     string templateString = template_stmt->get_string().str();

  // DQ (12//13/2015): The handling of "__decltype" in non-c++11 mode is a problem because EDG normalizes
  // this in the template string to be "decltype" which is not recognized by the backend compiler.
#if 0
     info.display("denormalize template string for decltype");
#endif

     SgSourceFile* sourcefile = info.get_current_source_file();
  // ROSE_ASSERT(sourcefile != NULL);
     if (sourcefile == NULL)
        {
#if 0
          printf ("NOTE: source file not available in SgUnparse_Info in unparseTemplateDeclarationStatment_support(): stmt = %p = %s \n",stmt,stmt->class_name().c_str());
#endif
        }

  // DQ (10/23/2016): Allow C++14 support to behave the same as C++11 support below.
  // We only do this denormalization if we are not using C++11 or later version of C++.
  // if (sourcefile->get_Cxx11_only() == false)
  // if (sourcefile != NULL && sourcefile->get_Cxx11_only() == false)
     if (sourcefile != NULL && sourcefile->get_Cxx11_only() == false && sourcefile->get_Cxx14_only() == false)
        {
          ROSE_ASSERT(sourcefile->get_Cxx14_only() == false);
#if 0
          printf ("Intercept the template string: \n");
          printf ("templateString = %s \n",templateString.c_str());
#endif
       // Substitute " decltype" with " __decltype" to fix this.
       // templateString.subst(" decltype"," __decltype");
          string denormalizedTemplateString = replaceString (templateString," decltype"," __decltype");

       // DQ (4/18/2018): Added denormalization of __ALIGNOF__ to __alignof__ name for operator.
       // Not clear if this is only a C++11 issue or a C++14 issue as well.
       // denormalizedTemplateString = replaceString (denormalizedTemplateString," __ALIGNOF__"," __alignof__");
#if 0
          printf ("denormalizedTemplateString = %s \n",denormalizedTemplateString.c_str());
#endif
          templateString = denormalizedTemplateString;
        }

  // DQ (4/18/2018): Added denormalization of __ALIGNOF__ to __alignof__ name for operator.
  // Not clear if this is only a C++11 issue or a C++14 issue as well.
     string denormalizedAlignofTemplateString = replaceString (templateString," __ALIGNOF__"," __alignof__");
#if 0
     printf ("denormalizedAlignofTemplateString = %s \n",denormalizedAlignofTemplateString.c_str());
#endif
     templateString = denormalizedAlignofTemplateString;
 
     if (sourcefile != NULL && sourcefile->get_unparse_template_ast() == true)
        {
          if (templateMemberFunctionDeclaration != NULL) {
            SgDeclarationStatement * assoc_decl = templateMemberFunctionDeclaration->get_associatedClassDeclaration();
            SgTemplateClassDeclaration * assoc_tpl_class_decl = isSgTemplateClassDeclaration(assoc_decl);

            SgNode * parent = templateMemberFunctionDeclaration->get_parent();
            SgTemplateClassDefinition * parent_is_tpl_class_defn = isSgTemplateClassDefinition(parent);

            if (assoc_tpl_class_decl != NULL && parent_is_tpl_class_defn == NULL) {
              unparseTemplateHeader(assoc_tpl_class_decl,info);
            }
          }

          unparseTemplateHeader(template_stmt,info);

          SgUnparse_Info ninfo(info);

          if (templateClassDeclaration != NULL) {
            ninfo.unset_SkipSemiColon();
            ninfo.set_declstatement_ptr(NULL);
            ninfo.set_declstatement_ptr(templateClassDeclaration);

            SgClassDefinition * class_defn = templateClassDeclaration->get_definition();
            if (class_defn != NULL) {
              unparseClassDefnStmt(templateClassDeclaration->get_definition(), ninfo);
            }
            else {
              SgClassDeclaration::class_types class_type = templateClassDeclaration->get_class_type();

              switch (class_type) {
                case SgClassDeclaration::e_class :
                  {
                    curprint("class ");
                    break;
                  }
                case SgClassDeclaration::e_struct :
                  {
                    curprint("struct ");
                    break;
                  }
                case SgClassDeclaration::e_union :
                  {
                    curprint("union ");
                    break;
                  }
                case SgClassDeclaration::e_template_parameter :
                  {
                    curprint(" ");
                    break;
                  }
                default:
                  {
                    printf ("Error: default reached in unparseClassDeclStmt() \n");
                    ROSE_ASSERT(false);
                    break;
                  }
              }

              SgName class_name = templateClassDeclaration->get_name();
              curprint(class_name.getString().c_str());
            }
            ninfo.set_declstatement_ptr(NULL);

            if (!info.SkipSemiColon())
              curprint(";");
          }
          else if (templateFunctionDeclaration != NULL || templateMemberFunctionDeclaration != NULL) {

            SgFunctionDeclaration * functionDeclaration = isSgFunctionDeclaration(stmt);
            ROSE_ASSERT(functionDeclaration != NULL);

            SgType * rtype = functionDeclaration->get_type()->get_return_type();
            unparseReturnType (functionDeclaration,rtype,ninfo);

            ninfo.unset_SkipSemiColon();
            ninfo.set_declstatement_ptr(NULL);
            ninfo.set_declstatement_ptr(functionDeclaration);

            unparse_helper(functionDeclaration, ninfo);

            ninfo.set_declstatement_ptr(NULL);

            if (rtype != NULL) {
              SgUnparse_Info ninfo3(ninfo);
              ninfo3.set_isTypeSecondPart();

              unp->u_type->unparseType(rtype, ninfo3);
            }

            if (templateMemberFunctionDeclaration != NULL) {
              unparseTrailingFunctionModifiers(templateMemberFunctionDeclaration,ninfo);
            }

            SgFunctionDefinition * functionDefn = functionDeclaration->get_definition();
            if (functionDefn != NULL) {
              SgBasicBlock * body = functionDefn->get_body();
              unparseStatement(body, info);
            }

            if (functionDefn == NULL && !info.SkipSemiColon()) {
                curprint(";");
            }
          }
          else if (templateVariableDeclaration != NULL) {
            ROSE_ASSERT(false); // TODO
          }
          else {
            printf("Error: unexpected node variant: %s\n", stmt->class_name().c_str());
            ROSE_ASSERT(false);
          }

          curprint ("\n");
        }
       else if (string_represents_function_body == true) {
       // DQ (9/7/2014): This is the special case (to output template member and non-member function declarations after EDG normalization 
       // to move then out of a template class declaration.
          SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(template_stmt);
          ROSE_ASSERT(functionDeclaration != NULL);
          ROSE_ASSERT(functionDeclaration->isNormalizedTemplateFunction());

          ROSE_ASSERT(templateMemberFunctionDeclaration != NULL);

       // TV (10/08/2018): temporary switch for ROSE-1392 (relies on template unparsing from AST)
          if (sourcefile->get_unparse_edg_normalized_method_ROSE_1392()) {
            SgDeclarationStatement * assoc_decl = templateMemberFunctionDeclaration->get_associatedClassDeclaration();
            SgTemplateClassDeclaration * assoc_tpl_class_decl = isSgTemplateClassDeclaration(assoc_decl);

            SgNode * parent = templateMemberFunctionDeclaration->get_parent();
            SgTemplateClassDefinition * parent_is_tpl_class_defn = isSgTemplateClassDefinition(parent);

            if (assoc_tpl_class_decl != NULL && parent_is_tpl_class_defn == NULL) {
              unparseTemplateHeader(assoc_tpl_class_decl,info);
            }

            unparseTemplateHeader(templateMemberFunctionDeclaration,info);

            SgUnparse_Info ninfo(info);

            SgType *rtype = NULL;
            unparseReturnType (functionDeclaration,rtype,ninfo);

            ninfo.set_declstatement_ptr(NULL);
            ninfo.set_declstatement_ptr(functionDeclaration);

            unparse_helper(functionDeclaration, ninfo);

            ninfo.set_declstatement_ptr(NULL);

            if (rtype != NULL)
             {
               SgUnparse_Info ninfo3(ninfo);
               ninfo3.set_isTypeSecondPart();

               unp->u_type->unparseType(rtype, ninfo3);
             }

            unparseTrailingFunctionModifiers(templateMemberFunctionDeclaration,ninfo);

            curprint(string("\n") + templateString + string("\n"));
          }
        }
       else
        {
       // DQ (9/7/2014): This is the typical case.
#if OUTPUT_PLACEHOLDER_COMMENTS_FOR_SUPRESSED_TEMPLATE_IR_NODES
       // DQ (4/5/2018): For debugging, output something so that we know why nothing is output.
          if (templateString.size() == 0)
             {
               curprint (" /* Output the templateString: templateString.size() = " + StringUtility::numberToString(templateString.size()) + " */ ");
             }
#endif
#if 0
          printf ("In unparseTemplateDeclarationStatment_support(): Output the templateString = %s \n",templateString.c_str());
#endif
       // printf ("template_stmt->get_template_kind() = %d \n",template_stmt->get_template_kind());
          curprint(string("\n") + templateString);

        }

#if 0
      printf ("Leaving unparseTemplateDeclarationStatment_support(stmt = %p = %s) \n",stmt,stmt->class_name().c_str());
      curprint (" /* Leaving unparseTemplateDeclarationStatment_support() */ \n");
#endif
   }
 
 
//#if USE_UPC_IR_NODES //TODO need this?
//#if UPC_EXTENSIONS_ALLOWED
 // Liao, 6/13/2008, unparsing UPC nodes in the AST
void
Unparse_ExprStmt::unparseUpcNotifyStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgUpcNotifyStatement* input = isSgUpcNotifyStatement(stmt);
     ROSE_ASSERT(input != NULL);

     curprint ( string("upc_notify "));
     SgUnparse_Info ninfo(info);

     if (input->get_notify_expression())
        {
          unparseExpression(input->get_notify_expression(), ninfo);
        }

     if (!ninfo.SkipSemiColon())
        {
          curprint ( string(";"));
        }
   }

void
Unparse_ExprStmt::unparseUpcWaitStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgUpcWaitStatement* input = isSgUpcWaitStatement(stmt);
     ROSE_ASSERT(input != NULL);

     curprint ( string("upc_wait "));
     SgUnparse_Info ninfo(info);

     if (input->get_wait_expression())
        {
          unparseExpression(input->get_wait_expression(), ninfo);
        }
     if (!ninfo.SkipSemiColon())
        {
          curprint ( string(";"));
        }
   }

void
Unparse_ExprStmt::unparseUpcBarrierStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgUpcBarrierStatement* input = isSgUpcBarrierStatement(stmt);
     ROSE_ASSERT(input != NULL);

     curprint ( string("upc_barrier "));
     SgUnparse_Info ninfo(info);

     if (input->get_barrier_expression())
        {
          unparseExpression(input->get_barrier_expression(), ninfo);
        }

     if (!ninfo.SkipSemiColon())
        {
          curprint ( string(";"));
        }
   }

void
Unparse_ExprStmt::unparseUpcFenceStatement(SgStatement* stmt, SgUnparse_Info& info)
 {
   SgUpcFenceStatement* input = isSgUpcFenceStatement(stmt);
   ROSE_ASSERT(input != NULL);

   curprint ( string("upc_fence "));
   SgUnparse_Info ninfo(info);

   if (!ninfo.SkipSemiColon())
        curprint ( string(";"));
 }

// Liao, 6/17/2008, unparse upc_forall 
// Most code is copied from Unparse_ExprStmt::unparseForStmt()
void
Unparse_ExprStmt::unparseUpcForAllStatement(SgStatement* stmt, SgUnparse_Info& info)
   {
  // printf ("Unparse for loop \n");
     SgUpcForAllStatement* for_stmt = isSgUpcForAllStatement(stmt);
     ROSE_ASSERT(for_stmt != NULL);

     curprint ( string("upc_forall ("));
     SgUnparse_Info newinfo(info);
     newinfo.set_SkipSemiColon();
     newinfo.set_inConditional();  // set to prevent printing line and file information

  // curprint ( string(" /* initializer */ ";
     SgStatement *tmp_stmt = for_stmt->get_for_init_stmt();
  // ROSE_ASSERT(tmp_stmt != NULL);
     if (tmp_stmt != NULL)
        {
          unparseStatement(tmp_stmt,newinfo);
        }
       else
        {
#if 0
       // DQ (10/8/2012): Commented out to avoid output spew.
          printf ("Warning in unparseForStmt(): for_stmt->get_for_init_stmt() == NULL \n");
#endif
          curprint ( string("; "));
        }
     newinfo.unset_inConditional();

#if 0
     SgExpression *tmp_expr = NULL;
     if ( (tmp_expr = for_stmt->get_test_expr()))
          unparseExpression(tmp_expr, info);
#else
  // DQ (12/13/2005): New code for handling the test (which could be a declaration!)
  // printf ("Output the test in the for statement format newinfo.inConditional() = %s \n",newinfo.inConditional() ? "true" : "false");
  // curprint ( string(" /* test */ ";
     SgStatement *test_stmt = for_stmt->get_test();
     ROSE_ASSERT(test_stmt != NULL);
  // if ( test_stmt != NULL )
     SgUnparse_Info testinfo(info);
     testinfo.set_SkipSemiColon();
     testinfo.set_inConditional();
  // printf ("Output the test in the for statement format testinfo.inConditional() = %s \n",testinfo.inConditional() ? "true" : "false");
     unparseStatement(test_stmt, testinfo);
#endif
     curprint ( string("; "));

  // curprint ( string(" /* increment */ ";
  // SgExpression *increment_expr = for_stmt->get_increment_expr();
     SgExpression *increment_expr = for_stmt->get_increment();
     ROSE_ASSERT(increment_expr != NULL);
     if ( increment_expr != NULL )
          unparseExpression(increment_expr, info);

     curprint ( string("; "));
  // Liao, unparse the affinity expression
     SgExpression * affinity_expr = for_stmt->get_affinity();
     ROSE_ASSERT(affinity_expr != NULL);
     SgExpression * null_expr = isSgNullExpression(affinity_expr);
     if (null_expr)
       curprint (string("continue"));
     else
       unparseExpression(affinity_expr, info); 
     curprint ( string(") "));
   // Added support to output the header without the body to support the addition 
  // of more context in the prefix used with the AST Rewrite Mechanism.
  // if ( (tmp_stmt = for_stmt->get_loop_body()) )

     SgStatement* loopBody = for_stmt->get_loop_body();
     ROSE_ASSERT(loopBody != NULL);

  // if ( (tmp_stmt = for_stmt->get_loop_body()) && !info.SkipBasicBlock())
     if ( (loopBody != NULL) && !info.SkipBasicBlock())
        {
          unp->cur.format(loopBody, info, FORMAT_BEFORE_NESTED_STATEMENT);
          unparseStatement(loopBody, info);
          unp->cur.format(loopBody, info, FORMAT_AFTER_NESTED_STATEMENT);
        }
       else
        {
          if (!info.SkipSemiColon())
             {
               curprint ( string(";"));
             }
        }
   }


// OpenMP support 
void Unparse_ExprStmt::unparseOmpPrefix(SgUnparse_Info& info)
{
  curprint(string ("#pragma omp "));
}


void Unparse_ExprStmt::unparseOmpForStatement (SgStatement* stmt,     SgUnparse_Info& info)
{
  ROSE_ASSERT (stmt != NULL);
  SgOmpForStatement * f_stmt = isSgOmpForStatement (stmt);
  ROSE_ASSERT (f_stmt != NULL);

  unparseOmpDirectivePrefixAndName(stmt, info);

  unparseOmpBeginDirectiveClauses(stmt, info);
  // TODO a better way to new line? and add indentation 
  curprint (string ("\n"));

  SgUnparse_Info ninfo(info);
  if (f_stmt->get_body())
  {
    unparseStatement(f_stmt->get_body(), ninfo);
  }
  else
  {
    cerr<<"Error: empty body for:"<<stmt->class_name()<<" is not allowed!"<<endl;
    ROSE_ASSERT(false);
  }
}

void Unparse_ExprStmt::unparseOmpForSimdStatement (SgStatement* stmt,     SgUnparse_Info& info)
{
  ROSE_ASSERT (stmt != NULL);
  SgOmpForSimdStatement * f_stmt = isSgOmpForSimdStatement (stmt);
  ROSE_ASSERT (f_stmt != NULL);

  unparseOmpDirectivePrefixAndName(stmt, info);

  unparseOmpBeginDirectiveClauses(stmt, info);
  // TODO a better way to new line? and add indentation 
  curprint (string ("\n"));

  SgUnparse_Info ninfo(info);
  if (f_stmt->get_body())
  {
    unparseStatement(f_stmt->get_body(), ninfo);
  }
  else
  {
    cerr<<"Error: empty body for:"<<stmt->class_name()<<" is not allowed!"<<endl;
    ROSE_ASSERT(false);
  }
}


void
Unparse_ExprStmt::unparseOmpBeginDirectiveClauses (SgStatement* stmt,     SgUnparse_Info& info)
{
  ROSE_ASSERT (stmt != NULL);
  // optional clauses
  SgOmpClauseBodyStatement* bodystmt= isSgOmpClauseBodyStatement(stmt);
  SgOmpDeclareSimdStatement* simdstmt= isSgOmpDeclareSimdStatement(stmt);
  if (bodystmt||simdstmt)
  {
    const SgOmpClausePtrList& clause_ptr_list = bodystmt?bodystmt->get_clauses():simdstmt->get_clauses();
    SgOmpClausePtrList::const_iterator i;
    for (i= clause_ptr_list.begin(); i!= clause_ptr_list.end(); i++)
    {
      SgOmpClause* c_clause = *i;
      unparseOmpClause(c_clause, info);
    }
  } 
}


void
Unparse_ExprStmt::unparseStaticAssertionDeclaration (SgStatement* stmt, SgUnparse_Info& info)
   {
  // DQ (7/25/2014): Adding support for C11 static assertions.

  // For C11 this whould be unparsed as "_Static_assert", but for C++ it should be unparsed as "static_assert".
     SgStaticAssertionDeclaration* staticAssertionDeclaration = isSgStaticAssertionDeclaration(stmt);
     ROSE_ASSERT(staticAssertionDeclaration != NULL);

  // DQ (4/29/2017): This is the C11 syntax, and for C++11 we need the alternative syntax ("static_assert").
  // curprint("_Static_assert(");
     if (SageInterface::is_Cxx_language() == true)
        {
       // This must be C++11 (or later).
          curprint("static_assert(");
        }
       else
        {
       // This must be C11 (or later).
          curprint("_Static_assert(");
        }

     unparseExpression(staticAssertionDeclaration->get_condition(), info);
     curprint(",\"");
  // unparseExpression(staticAssertionDeclaration->get_string_literal(), info);
     curprint(staticAssertionDeclaration->get_string_literal());
     curprint("\");");

#if 0
      printf ("Exiting as a test! (unparseStaticAssertionDeclaration not implemented) \n");
      ROSE_ASSERT(false);
#endif
   }



void
Unparse_ExprStmt::unparseMicrosoftAttributeDeclaration (SgStatement* stmt, SgUnparse_Info& info)
   {
  // DQ (8/17/2014): Adding support for Microsoft attributes.
     SgMicrosoftAttributeDeclaration* microsoftAttributeDeclaration = isSgMicrosoftAttributeDeclaration(stmt);
     ROSE_ASSERT(microsoftAttributeDeclaration != NULL);

     curprint("[");
     curprint(microsoftAttributeDeclaration->get_attribute_string());
     curprint("]");

#if 0
      printf ("Exiting as a test! (unparseMicrosoftAttributeDeclaration not implemented) \n");
      ROSE_ASSERT(false);
#endif
   }


 // EOF



