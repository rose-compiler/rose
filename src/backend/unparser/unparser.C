/* unparser.C
 * Contains the implementation of the constructors, destructor, formatting functions,
 * and fucntions that unparse directives.
 */

#include "sage3basic.h"
#include "unparser.h"
#include "keep_going.h"
#include "rose_config.h"

#include <Rose/AST/Traversal.h>

#include <string.h>
#if _MSC_VER
#include <direct.h>
#include <process.h>
#endif

// DQ (8/1/2018): This is the suppport for unparsing of header files.
#include "IncludedFilesUnparser.h"

// PP (1/9/24): commented out b/c FileHelper.h gets included through cmdline.h and has no include guards
// #include "FileHelper.h"

#include <boost/algorithm/string.hpp>

// DQ (9/26/2018): Added so that we can call the display function for TokenStreamSequenceToNodeMapping (for debugging).
#include "tokenStreamMapping.h"

// PP (1/9/24): added so the Ada unparser can access the Ada specific command line options (i.e., outputPath).
#include "cmdline.h"

// Rasmussen (1/6/2025): JVM unparsing
#include <Rose/BinaryAnalysis/Partitioner2/ModulesJvm.h>

using namespace std;
using namespace Rose;
using namespace Rose::Diagnostics; // for mlog, INFO, WARN, ERROR, FATAL, etc.
namespace SI = SageInterface;

void generateNameQualificationSupport(SgNode* node, std::set<SgNode*> &referencedNameSet);
void buildTokenStreamFrontier(SgSourceFile* sourceFile, bool traverseHeaderFiles);

//-----------------------------------------------------------------------------------
//  Unparser::Unparser
//-----------------------------------------------------------------------------------
Unparser::Unparser(ostream* nos, string fname, Unparser_Opt nopt, UnparseFormatHelp* h, UnparseDelegate* d)
   : cur(nos, h), delegate(d)
   {
     ASSERT_not_null(nos);

     u_type     = new Unparse_Type(this);
     u_name     = new Unparser_Nameq(this);
     u_sym      = new Unparse_Sym(this);
     u_debug    = new Unparse_Debug(this);
     u_sage     = new Unparse_MOD_SAGE(this);
     u_exprStmt = new Unparse_ExprStmt(this, fname);

  // DQ (8/14/2007): I have added this here to be consistant, but I question if this is a good design!
  // UnparseFortran_type* u_fortran_type;
  // FortranCodeGeneration_locatedNode* u_fortran_locatedNode;
     u_fortran_type = new UnparseFortran_type(this);
     u_fortran_locatedNode = new FortranCodeGeneration_locatedNode(this, fname);

     opt = nopt;
     cur_index = 0;
     currentFile = nullptr;

     set_resetSourcePosition(false);
   }

//-----------------------------------------------------------------------------------
//  Unparser::~Unparser
//
//  Destructor
//-----------------------------------------------------------------------------------
Unparser::~Unparser()
   {
     delete u_type;
     delete u_name;
     delete u_sym;
     delete u_debug;
     delete u_sage;
     delete u_exprStmt;
     delete u_fortran_type;
     delete u_fortran_locatedNode;
   }

UnparseFormat &Unparser::get_output_stream()
   {
     return cur;
   }

bool
Unparser::isPartOfTransformation(SgLocatedNode* n)
   {
     ASSERT_not_null(n);
     ASSERT_not_null(n->get_file_info());

  // DQ (5/10/2005): For now let's check both, but I think we want to favor isTransfrmation() over get_isPartOfTransformation() in the future.
     return (n->get_file_info()->isTransformation() || n->get_file_info()->get_isPartOfTransformation());
   }

bool
Unparser::isCompilerGenerated( SgLocatedNode *n)
   {
     ASSERT_not_null(n);
     ASSERT_not_null(n->get_file_info());

  // Support for including any compiler generated code (such as instatiated templates).
     return n->get_file_info()->isCompilerGenerated();
   }

bool
Unparser::containsLanguageStatements(char* fileName)
   {
  // We need to implement this later
     ASSERT_not_null(fileName);

  // Note that: false will imply that the file contains only preprocessor declarations
  //                  and thus the file need not be unparsed with a different name
  //                  (a local header file).
  //            true  will imply that it needs to be unparsed as a special renamed
  //                  header file so that transformations in the header files can be
  //                  supported.
     return false;
   }

bool
Unparser::includeFileIsSurroundedByExternCBraces(char*)
   {
  // For the first attempt at writing this function we will check to see if all the
  // declarations in the target file have "C" linkage.  This is neccessary but not
  // a sufficent condition to knowing if the user really specific a surrounding
  // extern "C" around an include file.

  // A more robust test is to search all the declaration until the next declaration coming
  // from the current file (if they ALL have "C" linkage then it is at least equivalent to
  // unparsing with a surrounding extern "C" (with braces) specification.

  // For the moment just assume that all files require or don't require extern "C" linkage.
  // Why is this not good enough for compiling (if we don't link anything)?

     return false;
   }

//-----------------------------------------------------------------------------------
//  int Unparser::line_count
//
//  counts the number of lines in one directive
//-----------------------------------------------------------------------------------
int Unparser::line_count(char* directive) {
  int lines = 1;

  for (int i = 0; directive[i] != '\0'; i++) {
    if (directive[i] == '\n')
      lines++;
  }
  return lines;
}


bool
Unparser::isASecondaryFile(SgStatement*)
   {
  // for now just assume there are no secondary files
     return false;
   }

void
Unparser::computeNameQualification(SgSourceFile* file)
   {
  // DQ (8/7/2018): Refactored code for name qualification (so that we can call it once before all files
  // are unparsed (where we unparse multiple files because fo the use of header file unparsing)).

  // DQ (8/7/2018): Copied this logic from where computeNameQualification() was originally called to
  // limit the number of parameters in the function API.
     bool isCxxFile = false;
     if ( ( (file->get_Cxx_only() == true) && (file->get_outputLanguage() == SgFile::e_default_language) ) || (file->get_outputLanguage() == SgFile::e_Cxx_language) )
       {
         isCxxFile = true;
       }

     if ((isCxxFile == true) || SageInterface::is_Ada_language())
        {
       // Build the local set to use to record when declaration that might required qualified references have been seen.
          std::set<SgNode*> referencedNameSet;
          std::set<SgLocatedNode*> modifiedLocatedNodesSet_1 = SageInterface::collectModifiedLocatedNodes(file);
          size_t numberOfModifiedNodesBeforeNameQualification = modifiedLocatedNodesSet_1.size();
          if (SgProject::get_verbose() > 0)
             {
               printf ("Calling name qualification support \n");
             }

       // DQ (10/17/2020): I have discovered that this was added by Tristan, and it is not clear to me what it is about.
       // After discussion with Tristan, this is specific to Jovial to C++ translator, and supports the development of
       // separate header files that are built, instead of using the single translation unit and the unparse header file
       // support that has been recently built into ROSE (last year).  This is fine, but it brings up a possible somewhat
       // philosophical discussion about how to define a translation unit in C and C++, nameily that the SgFile and SgSourceFile
       // is really a translation unit for the source code in any source file (and does not refer to only the source file
       // to the exclusion of associated included file via CPP #include directives.
          SgNodePtrList & nodes_for_namequal_init = file->get_extra_nodes_for_namequal_init();
          for (SgNodePtrList::iterator it = nodes_for_namequal_init.begin(); it != nodes_for_namequal_init.end(); ++it) {
            generateNameQualificationSupport(*it, referencedNameSet);
          }

          generateNameQualificationSupport(file, referencedNameSet);

          if (SgProject::get_verbose() > 0)
             {
               printf ("DONE: Calling name qualification support \n");
             }

          propagateHiddenListData(file);

       // DQ (6/11/2015): Added to support debugging the difference between C and C++ support for token-based unparsing.
          std::set<SgLocatedNode*> modifiedLocatedNodesSet_2 = SageInterface::collectModifiedLocatedNodes(file);
          size_t numberOfModifiedNodesAfterNameQualification = modifiedLocatedNodesSet_2.size();

       // DQ (6/11/2015): Introduce error checking on the AST if we are using the token-based unparsing.
          if ( (file->get_unparse_tokens() == true) && (numberOfModifiedNodesAfterNameQualification != numberOfModifiedNodesBeforeNameQualification) )
             {
               printf ("In Unparser::computeNameQualification(): numberOfModifiedNodesBeforeNameQualification = %zu numberOfModifiedNodesAfterNameQualification = %zu \n",
                    numberOfModifiedNodesBeforeNameQualification,numberOfModifiedNodesAfterNameQualification);
               printf ("ERROR: namequalification step has introduced modified IR nodes in the AST (a problem for the token-based unparsing) \n");
               ROSE_ABORT();
             }

        }
   }

void
Unparser::unparseFile(SgSourceFile* file, SgUnparse_Info& info, SgScopeStatement* unparseScope)
   {
#define DEBUG_UNPARSE_FILE 0
     ASSERT_not_null(file);

  // DQ (6/5/2021): Save the previous statement that was just unparsed (at this point we just want to clear the value from any other file).
     SgUnparse_Info::set_previouslyUnparsedStatement(nullptr);
     SgUnparse_Info::set_previousStatementUnparsedFromTokenStream(false);

  // DQ (10/29/2018): I now think we need to support this mechanism of specifying the scope to be unparsed seperately.
  // This is essential to the support for header files representing nested scopes inside of the global scope.
  // Traversing the global scope does not permit these inner nested scopes to be traversed using the unparser.

  // DQ (8/16/2018): We can remove this as part of a more conventional usage with a single SgSourceFile and SgGlobal for each header file.
  // ROSE_ASSERT(unparseScope == NULL);

  // DQ (10/27/2019): This assertion is false for test2 in our UnparseHeadersTests regression tests.
  // DQ (10/22/2019): I think we can specify this (and later cleanup some of the code below).
  // ROSE_ASSERT(unparseScope == NULL);

#if DEBUG_UNPARSE_FILE || 0
     printf ("\n\n");
     printf ("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ \n");
     printf ("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ \n");
     printf ("In unparseFile(): file = %p filename = %s unparseScope = %p \n",file,file->getFileName().c_str(),unparseScope);
     printf (" --- file->get_header_file_unparsing_optimization()             = %s \n",file->get_header_file_unparsing_optimization() ? "true" : "false");
     if (unparseScope != nullptr)
        {
          printf ("   --- unparseScope = %p = %s \n",unparseScope,unparseScope->class_name().c_str());
        }
     printf ("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ \n");
     printf ("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ \n");
#endif

  // DQ (5/22/2021): Set the current_source_file in the SgUnparse_Info object.
     SgSourceFile* sourceFile = info.get_current_source_file();
     ROSE_ASSERT(sourceFile != nullptr);

#if 0
  // DQ (4/24/2021): Sorting out the header file optimization, so that we can correctly handle when both ON or OFF.
  // This data member appears to always be false.
     if (file->get_header_file_unparsing_optimization_header_file() == true)
        {
          printf ("Found case of file->get_header_file_unparsing_optimization_header_file() == true \n");
        }
#endif

#if 1
        {
          SgGlobal* tmp_globalScope = isSgGlobal(file->get_globalScope());
          if (tmp_globalScope != nullptr)
             {
               ASSERT_not_null(tmp_globalScope->get_parent());
             }
            else
             {
               printf ("Exiting because I think this is an error! \n");
               ROSE_ABORT();
             }
        }
#endif

  // Detect reuse of an Unparser object with a different file
     ASSERT_require(currentFile == nullptr);

     currentFile = file;
     ASSERT_not_null(currentFile);

#if 0
     file->display("file: Unparser::unparseFile");
#endif

     // What kind of language are we unparsing.  These are more robust tests
     // for multiple files of multiple languages than using SageInterface.  This
     // is also how the language-dependent parser is called.
     bool isFortranFile = false;
     if ( ((file->get_Fortran_only()) && (file->get_outputLanguage() == SgFile::e_default_language)) ||
          (file->get_outputLanguage() == SgFile::e_Fortran_language) )
       {
         isFortranFile = true;
       }
     bool isCfile = false;
     if ( ( (file->get_C_only() == true) &&
            (file->get_outputLanguage() ==
             SgFile::e_default_language) ) ||
          (file->get_outputLanguage() == SgFile::e_C_language) )
       {
         isCfile = true;
       }
     bool isCxxFile = false;
     if ( ( (file->get_Cxx_only() == true) &&
            (file->get_outputLanguage() ==
             SgFile::e_default_language) ) ||
          (file->get_outputLanguage() == SgFile::e_Cxx_language) )
       {
         isCxxFile = true;
       }
     // What about all the others?  Java, ...?
     // Can only be parsing to one language!
     ROSE_ASSERT(((int)isFortranFile + (int)isCfile + (int)isCxxFile) <= 1);

     TimingPerformance timer{"Unparse File:"};

     ASSERT_not_null(info.get_current_source_file());
     info.set_current_source_file(file);

#if DEBUG_UNPARSE_FILE || 0
     printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
     printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
     printf ("In Unparser::unparseFile(): file->get_unparse_tokens() = %s \n",file->get_unparse_tokens() ? "true" : "false");
     printf ("isCfile                    = %s \n",isCfile ? "true" : "false");
     printf ("isCxxFile                  = %s \n",isCxxFile ? "true" : "false");
     printf ("file->get_unparse_tokens() = %s \n",file->get_unparse_tokens() ? "true" : "false");
     printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
     printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
#endif

  // DQ (12/6/2014): This is the part of the token stream support that is required after transformations have been done in the AST.
     if ( (isCfile || isCxxFile) && file->get_unparse_tokens() == true)
        {
#define DEBUG_UNPARSE_TOKENS 0

       // This is only currently being tested and evaluated for C language (should also work for C++, but not yet for Fortran).
#if DEBUG_UNPARSE_TOKENS
          printf ("In Unparser::unparseFile(): END: this->currentFile = %p this->currentFile->getFileName() = %s \n",this->currentFile,this->currentFile->getFileName().c_str());
#endif
#if DEBUG_UNPARSE_TOKENS
          printf ("In Unparser::unparseFile(): Building token stream mapping frontier: filename = %s \n",file->getFileName().c_str());
#endif

          ROSE_ASSERT(mapFilenameToAttributes.empty() == true);

       // This function builds the data base (STL map) for the different subsequences ranges of the token stream.
       // and attaches the toke stream to the SgSourceFile IR node.

       // DQ (8/8/2018): It also marks IR nodes as transformations where they are detected to be a part of any
       // modifications (where the isModified flag detected to be true).

          SgGlobal* nested_globalScope = isSgGlobal(file->get_globalScope());

       // DQ (8/17/2018): Every source file should have a global scope.
          ASSERT_not_null(nested_globalScope);

          if (nested_globalScope != nullptr)
             {
               ASSERT_not_null(nested_globalScope->get_parent());
             }

#if DEBUG_UNPARSE_TOKENS || 0
          printf ("################################################################################################## \n");
          printf (" The token processing is now done in the secondaryFileProcessing (so we do not need to do it here) \n");
          printf ("################################################################################################## \n");
#endif
#if DEBUG_UNPARSE_TOKENS
          printf ("******************************************************************* \n");
          printf ("In Unparser::unparseFile(): Building token stream mapping frontier! \n");
          printf ("   --- file = %s \n",file->getFileName().c_str());
          printf ("******************************************************************* \n");
#endif

#if DEBUG_UNPARSE_TOKENS
          printf ("file->get_unparseHeaderFiles() = %s \n",file->get_unparseHeaderFiles() ? "true" : "false");
#endif

#if DEBUG_UNPARSE_FILE
          printf ("In Unparser::unparseFile(): END: this->currentFile = %p this->currentFile->getFileName() = %s \n",this->currentFile,this->currentFile->getFileName().c_str());
#endif
        }
       else
        {
          if (file->get_unparse_tokens() == true)
             {
               printf ("Warning: unparse_tokens support is only available for C and C++ languages at present \n");
             }
        }

  // Turn ON the error checking which triggers an if the default SgUnparse_Info constructor is called
     SgUnparse_Info::set_forceDefaultConstructorToTriggerError(true);

     if (file->get_markGeneratedFiles() == true)
        {
       // Output marker to identify code generated by ROSE (causes "#define ROSE_GENERATED_CODE" to be placed at the top of the file).
          u_exprStmt->markGeneratedFile();
        }

     SgScopeStatement* globalScope = file->get_globalScope();
     ASSERT_not_null(globalScope);

  // Make sure that both the C/C++ and Fortran unparsers are present!
     ASSERT_not_null(u_exprStmt);
     ASSERT_not_null(u_fortran_locatedNode);

     ROSE_ASSERT(file->get_outputLanguage() != SgFile::e_error_language);
     ROSE_ASSERT(file->get_outputLanguage() != SgFile::e_Promela_language);

  // DQ (8/29/2017): Adding more general handling for language support.

  // Not clear if we want this, translators might not want to have this constraint.
  // But use this for debugging initially.  I would like to see the default setting
  // for the output language set to be the same as the input language and then a
  // translator could change this setting.  If their is a different between the input
  // language and the output language then a collection of tests should be generated
  // that verify compliance of the AST with the output language specified.

     ROSE_ASSERT(file->get_inputLanguage() == file->get_outputLanguage());

     switch (file->get_outputLanguage())
        {
          case SgFile::e_error_language:
             {
               printf ("Error: SgFile::e_error_language detected in unparser \n");
               ROSE_ABORT();
             }

          case SgFile::e_default_language:
             {
#if 0
               printf ("Warning: SgFile::e_default_language detected in unparser \n");
#endif
             }

          case SgFile::e_C_language:
          case SgFile::e_Cxx_language:
             {
#if DEBUG_UNPARSE_FILE
               printf ("case SgFile::e_C/Cxx_language: Unparse using C/C++ unparser by default: unparseScope = %p \n",unparseScope);
               printf ("In Unparser::unparseFile(): case SgFile::e_C/Cxx_language: this->currentFile->getFileName() = %s \n",this->currentFile->getFileName().c_str());
#endif
            // DQ (10/29/2018): I now think we need to support this mechanism of specifying the scope to be unparsed separately.
            // This is essential to the support for header files representing nested scopes inside of the global scope.
            // Traversing the global scope does not permit these inner nested scopes to be traversed using the unparser.

            // negara1 (06/29/2011): If unparseScope is provided, unparse it. Otherwise, unparse the global scope (the default behavior).
               if (unparseScope != nullptr)
                  {
#if DEBUG_UNPARSE_FILE
                    printf ("In Unparser::unparseFile(): unparseScope = %p = %s \n",unparseScope,unparseScope->class_name().c_str());
#endif
                    ASSERT_not_null(this->currentFile);

                    SgStatementPtrList statements = unparseScope->generateStatementList();
                    for (SgStatementPtrList::iterator statement = statements.begin(); statement != statements.end(); statement++)
                       {
#if DEBUG_UNPARSE_FILE
                         printf ("Unparsing the statements on the unparseScope statement by statement (what about comments) statement = %p = %s \n",*statement,(*statement)->class_name().c_str());
#endif
                         u_exprStmt -> unparseStatement(*statement, info);
                       }
                  }
                 else
                  {
                    ASSERT_require(unparseScope == nullptr);

#if DEBUG_UNPARSE_FILE || 0
                    printf ("In Unparser::unparseFile(): case C/C++: unparseStatement(globalScope, info): globalScope = %p \n",globalScope);
                    printf ("globalScope->getDeclarationList().size() = %zu \n",globalScope->getDeclarationList().size());
                    SgGlobal* temp_globalScope = isSgGlobal(globalScope);
                    ROSE_ASSERT(temp_globalScope != nullptr);
                    printf ("Global scope being unparsed: temp_globalScope = %p \n",temp_globalScope);
                    printf ("temp_globalScope->get_declarations().size() = %zu \n",temp_globalScope->get_declarations().size());
#endif
                    ASSERT_not_null(globalScope->get_parent());
#if DEBUG_UNPARSE_FILE
                       {
                         printf ("Error: In Unparser::unparseFile(): this->currentFile != globalScope->get_parent() \n");
                         printf (" --- this->currentFile         = %p \n",this->currentFile);
                         printf (" --- globalScope->get_parent() = %p \n",globalScope->get_parent());
                         ROSE_ASSERT(this->currentFile != nullptr);
                         ROSE_ASSERT(globalScope->get_parent() != nullptr);

                         SgSourceFile* currentSourceFile = isSgSourceFile(this->currentFile);
                         ROSE_ASSERT(currentSourceFile != nullptr);

                         SgGlobal* globalScope_from_currentFile = currentSourceFile->get_globalScope();
                         printf ("globalScope_from_currentFile = %p \n",globalScope_from_currentFile);
                         printf ("globalScope                  = %p \n",globalScope);

                         SgSourceFile* source_file_from_currentFile        = isSgSourceFile(this->currentFile);
                         SgSourceFile* source_file_from_globalScope_parent = isSgSourceFile(globalScope->get_parent());
                         ROSE_ASSERT(source_file_from_currentFile != nullptr);
                         ROSE_ASSERT(source_file_from_globalScope_parent != nullptr);
                         printf ("source_file_from_currentFile        = %p filename = %s \n",source_file_from_currentFile,source_file_from_currentFile->getFileName().c_str());
                         printf ("source_file_from_globalScope_parent = %p filename = %s \n",source_file_from_globalScope_parent,source_file_from_globalScope_parent->getFileName().c_str());
                       }
#endif

#if DEBUG_UNPARSE_FILE
                    if (this->currentFile == globalScope->get_parent())
                       {
                         printf ("Unparsing a source file (different from all other source files) \n");
                       }
                      else
                       {
                         printf ("Unparsing a header file of an AST from a given source file \n");
                       }
#endif
                    SgSourceFile* currentSourceFile = isSgSourceFile(this->currentFile);
                    ROSE_ASSERT(currentSourceFile != nullptr);

#if DEBUG_UNPARSE_FILE
                    printf ("globalScope                      = %p \n",globalScope);
                    printf ("currentSourceFile->getFileName() = %s \n",currentSourceFile->getFileName().c_str());
#endif
                    SgGlobal* globalScope_from_currentFile = currentSourceFile->get_globalScope();
                    ASSERT_require(globalScope == globalScope_from_currentFile);

                    u_exprStmt->unparseStatement(globalScope, info);
#if DEBUG_UNPARSE_FILE
                    printf ("DONE: In Unparser::unparseFile(): case C/C++: unparseStatement(globalScope, info): globalScope = %p \n",globalScope);
#endif
                  }
               break;
             }

          case SgFile::e_Fortran_language:
             {
               TimingPerformance timer ("Source code generation from AST (Fortran):");
               if (delegate) {
                 delegate->unparse_statement(globalScope, info, cur);
               }
               else {
                 u_fortran_locatedNode->unparseStatement(globalScope, info);
               }
               break;
             }

          case SgFile::e_Java_language:
             {
               Unparse_Java unparser(this, file->getFileName());
               unparser.unparseJavaFile(file, info);
               break;
             }

          case SgFile::e_Promela_language:
             {
               printf ("Error: SgFile::e_Promela_language detected in unparser (unparser not implemented, unparsing ignored) \n");
               break;
             }

          case SgFile::e_PHP_language:
             {
               Unparse_PHP unparser(this,file->get_unparse_output_filename());
               unparser.unparseStatement(globalScope, info);
               break;
             }

          case SgFile::e_Python_language:
             {
#ifdef ROSE_BUILD_PYTHON_LANGUAGE_SUPPORT
               Unparse_Python unparser(this,file->get_unparse_output_filename());
               unparser.unparseGlobalStmt(globalScope, info);
#else
               ASSERT_not_implemented("unparsing Python requires ROSE to have been configured with Python analysis support\n");
#endif
               break;
             }

          case SgFile::e_Csharp_language:
             {
               printf ("Error: SgFile::e_Csharp_language detected in unparser (unparser not implemented, unparsing ignored) \n");
               break;
             }

          case SgFile::e_Ada_language:
             {
               // PP (04/21/15) rm output since there is another one mentioning that
               //               Ada unparsing is experimental.
               //~ printf ("NOTE: SgFile::e_Ada_language detected in unparser (initial start at unparser) \n");
               Unparse_Ada adagen{this, file->getFileName()};

               adagen.unparseAdaFile(file, info);
               break;
             }

          case SgFile::e_Jovial_language:
             {
               UnparseJovial unparser{this, file->getFileName()};
               unparser.unparseJovialFile(file, info);
               break;
             }

          case SgFile::e_last_language:
             {
               mlog[ERROR] << "SgFile::e_last_language detected in unparser\n";
               ROSE_ABORT();
             }

          default:
             {
               mlog[ERROR] << "Error: default reached in unparser (unknown output language specified)\n";
               ROSE_ABORT();
             }
        }

     cur.flush();

#if DEBUG_UNPARSE_FILE
     printf ("Leaving Unparser::unparseFile(): file = %s = %s \n",file->get_sourceFileNameWithPath().c_str(),file->get_sourceFileNameWithoutPath().c_str());
     printf ("Leaving Unparser::unparseFile(): SageInterface::is_Cxx_language()     = %s \n",SageInterface::is_Cxx_language() ? "true" : "false");
#endif

  // Turn OFF the error checking which triggers an if the default SgUnparse_Info constructor is called
     SgUnparse_Info::set_forceDefaultConstructorToTriggerError(false);
   }

int
Unparser::getNumberOfLines(std::string internalString)
   {
  // This code is copied from the similar support in rose_attributes_list.C.
     int line = 0;
     int i    = 0;
     while (internalString[i] != '\0')
        {
          if (internalString[i] == '\n')
             {
               line++;
             }
          i++;
        }

     return line;
   }

int
Unparser::getColumnNumberOfEndOfString( std::string internalString )
   {
  // This code is copied from the similar support in rose_attributes_list.C.
     int col = 1;
     int i   = 0;

  // DQ (10/1/2013): I think we want to have the column number after a '\n' be zero.
  // DQ (10/27/2006): the last line has a '\n' so we need the length
  // of the last line before the '\n" triggers the counter to be reset!
  // This fix is required because the strings we have include the final '\n"
     int previousLineLength = col;
     while (internalString[i] != '\0')
        {
          if (internalString[i] == '\n')
             {
            // previousLineLength = col;
               col = 1;
               previousLineLength = col;
             }
            else
             {
               col++;
               previousLineLength = col;
             }
          i++;
        }

     int endingColumnNumber = previousLineLength;
     return endingColumnNumber;
   }


void
Unparser::unparseFileUsingTokenStream ( SgSourceFile* file )
   {
  // DQ (9/30/2013): Unparse the file using the token stream (stored in the SgFile).

  // DQ (10/27/2013): Now that we have setup the token_list in the SgSourceFile, this should be a valid list (unless this is completly blank file).
  // The assignment to the token_list in the SgSourceFile is handled in "void buildTokenStreamMapping(SgSourceFile* sourceFile);".

  // Note that these are the SgToken IR nodes and we have generated a token stream via the type: LexTokenStreamType.
  // ROSE_ASSERT(file->get_token_list().empty() == true);

     ROSE_ASSERT(file != nullptr);
     string fileNameForTokenStream = file->getFileName();

     if (file->get_token_list().empty() == true)
        {
          printf ("Warning: unparseFileUsingTokenStream(): ERROR no tokens found \n");
          return;
        }

     ASSERT_not_null(file->get_preprocessorDirectivesAndCommentsList());
     ROSEAttributesListContainerPtr filePreprocInfo = file->get_preprocessorDirectivesAndCommentsList();

  // We should at least have the current files CPP/Comment/Token information (even if it is an empty file).
     ROSE_ASSERT(filePreprocInfo->getList().size() > 0);

  // This is an empty list not useful outside of the Flex file to gather the CPP directives, comments, and tokens.
     ROSE_ASSERT(mapFilenameToAttributes.empty() == true);

#if 0
     printf ("In unparseFileUsingTokenStream(): Evaluate what files are processed in map (filePreprocInfo->getList().size() = %" PRIuPTR ") \n",filePreprocInfo->getList().size());
     std::map<std::string,ROSEAttributesList* >::iterator map_iterator = filePreprocInfo->getList().begin();
     int counter = 0;
     while (map_iterator != filePreprocInfo->getList().end())
        {
          printf ("map entry: %d \n",counter);
          printf ("   --- map_iterator->first  = %s \n",map_iterator->first.c_str());
          printf ("   --- map_iterator->second = %p \n",map_iterator->second);

          map_iterator++;
          counter++;
        }
     printf ("DONE: Evaluate what files are processed in map (filePreprocInfo->getList().size() = %" PRIuPTR ") \n",filePreprocInfo->getList().size());
#endif

  // std::map<std::string,ROSEAttributesList* >::iterator currentFileItr = mapFilenameToAttributes.find(fileNameForTokenStream);
     std::map<std::string,ROSEAttributesList* >::iterator currentFileItr = filePreprocInfo->getList().find(fileNameForTokenStream);
  // ROSE_ASSERT(currentFileItr != mapFilenameToAttributes.end());
  // ROSE_ASSERT(currentFileItr != NULL);
     ROSE_ASSERT(currentFileItr != filePreprocInfo->getList().end());

#if 0
     printf ("Get the ROSEAttributesList from the map iterator \n");
#endif

  // If there already exists a list for the current file then get that list.
     ASSERT_not_null(currentFileItr->second);
     ROSEAttributesList* existingListOfAttributes = currentFileItr->second;
     LexTokenStreamType & tokenList = *(existingListOfAttributes->get_rawTokenStream());

  // Write out the tokens into the output file.
     int current_line_number   = 1;
     int current_column_number = 1;

     for (LexTokenStreamType::iterator i = tokenList.begin(); i != tokenList.end(); i++)
        {
          std::string s   = (*i)->p_tok_elem->token_lexeme;
          int lines       = getNumberOfLines(s);
          int line_length = getColumnNumberOfEndOfString(s);

       // Check starting position
          if ((*i)->beginning_fpi.line_num != current_line_number)
             {
               printf ("error: (*i)->beginning_fpi.line_num = %d \n",(*i)->beginning_fpi.line_num);
               printf ("error: current_line_number          = %d \n",current_line_number);
               ROSE_ABORT();
             }

          if ((*i)->beginning_fpi.column_num != current_column_number)
             {
            // DQ (1/4/2014): This problem is demonstrated by tests/nonsmoke/functional/roseTests/astInterfaceTests/inputmoveDeclarationToInnermostScope_test2015_18.C when using the "-rose:verbose 2" option.
               printf ("error: In Unparser::unparseFileUsingTokenStream(): (*i)->beginning_fpi.column_num = %d \n",(*i)->beginning_fpi.column_num);
               printf ("error: In Unparser::unparseFileUsingTokenStream(): current_line_number = %d current_column_number = %d \n",current_line_number,current_column_number);

            // DQ (1/4/2014): Commented this assertion out as part of debugging tests/nonsmoke/functional/roseTests/astInterfaceTests/inputmoveDeclarationToInnermostScope_test2015_18.C when using the "-rose:verbose 2" option.
            // Note that "}" that is a part of an "extern \"C\" {" fails this test (is classified as CPP_PREPROCESSING_INFO).
            // ROSE_ASSERT(false);
             }

          current_line_number += lines;

          if (lines == 0)
             {
            // Increment the column number.
               current_column_number += (line_length-1);
             }
            else
             {
            // reset the column number.
               current_column_number = line_length;
             }

       // Check starting position
          if ((*i)->ending_fpi.line_num != current_line_number)
             {
               printf ("error: (*i)->ending_fpi.line_num = %d \n",(*i)->ending_fpi.line_num);
               printf ("error: current_line_number = %d \n",current_line_number);
               ROSE_ABORT();
             }

       // The position of the end of the last token is one less than the current position for the next token.
          if ((*i)->ending_fpi.column_num != (current_column_number - 1))
             {
               printf ("error: (*i)->ending_fpi.column_num = %d \n",(*i)->ending_fpi.column_num);
               printf ("error: current_line_number = %d current_column_number = %d \n",current_line_number,current_column_number);
               ROSE_ABORT();
             }
        }

  // DQ (10/27/2013): Use a different filename for the output of the raw token stream (not associated with individual statements).
     string outputFilename = "rose_raw_tokens_" + file->get_sourceFileNameWithoutPath();
     fstream ROSE_RawTokenStream_OutputFile(outputFilename.c_str(),ios::out);

     if (!ROSE_RawTokenStream_OutputFile)
        {
          printf ("Error detected in opening file %s for output \n",outputFilename.c_str());
          ROSE_ABORT();
        }

  // Use a different filename for the output of the raw token stream (which is a file generated for debugging support).

  // Write out the tokens into the output file.
     for (LexTokenStreamType::iterator i = tokenList.begin(); i != tokenList.end(); i++)
        {
          ROSE_RawTokenStream_OutputFile << (*i)->p_tok_elem->token_lexeme;
        }

     ROSE_RawTokenStream_OutputFile.flush();
   }

#ifdef ROSE_ENABLE_BINARY_ANALYSIS
/** Unparses a single physical, binary file.
 *
 *  Recreates the original binary file from the container representation under the SgAsmGenericFile node. This does not
 *  include instruction nodes since they're under the SgAsmInterpretation.  Instead, for any section that contained machine
 *  instructions, we simply write those bytes back to the new file.
 *
 *  If the AST has not been modified since the binary file was parsed, then the result should be byte-for-byte identical with
 *  the original. This tests that we have completely represented the binary file format in ROSE. Any transformations to parts
 *  of the binary file format in the AST will be represented in the regenerated binary.
 *
 *  The name of the new file is created by appending ".new" to the original file name. Leading path components are stripped so
 *  that the file is created in the current working directory. */
void
Unparser::unparseAsmFile(SgAsmGenericFile *file, SgUnparse_Info &)
{
     if ( SgProject::get_verbose() > 0 )
          printf ("In Unparser::unparseAsmFile... file = %p = %s \n",file,file->class_name().c_str());

    ASSERT_not_null(file);

    /* Genenerate an ASCII dump of the entire file contents.  Generate the dump before unparsing because unparsing may perform
     * certain relocations and normalization to the AST. */
    // DQ (8/30/2008): This is temporary, we should review how we want to name the files
    // generated in the unparse phase of processing a binary.
    file->dumpAll(true, ".dump");

    /* Generate file name for uparser output */
    // DQ (8/30/2008): This is temporary, we should review how we want to name the files
    // generated in the unparse phase of processing a binary.
    std::string output_name = file->get_name() + ".new";
    size_t slash = output_name.find_last_of('/');
    if (slash!=output_name.npos)
        output_name.replace(0, slash+1, "");
    if (SgProject::get_verbose() >= 1)
        std::cout << "output re-generated binary as: " << output_name << std::endl;

    /* Unparse the file to create a new executable */
    SgAsmExecutableFileFormat::unparseBinaryFormat(output_name, file);
}

void
Unparser::unparseFile(SgBinaryComposite *binary, SgUnparse_Info &info)
{
     if ( SgProject::get_verbose() > 0 )
        {
          printf ("In Unparser::unparseFile(SgBinaryComposite *binary, SgUnparse_Info &info): file = %p = %s \n",binary,binary->class_name().c_str());
        }

    ASSERT_not_null(binary);
    ASSERT_require(binary->get_binary_only()) ;

    /* Unparse each file and create an ASCII dump as well */
    const SgAsmGenericFilePtrList &files = binary->get_genericFileList()->get_files();
    ROSE_ASSERT(!files.empty());

    for (size_t i=0; i<files.size(); i++) {
        unparseAsmFile(files[i], info);
    }

    // Generate an ASCII dump of disassembled instructions for interpretations that we didn't already dump in `uparseAsmFile`. In
    // other words, dump interpretations that span multiple files.
    //
    // [Robb Matzke 2024-10-18]: There are better ways of producing assembly listings, but they don't generally work directly on an
    // AST. See Rose::BinaryAnalysis::Unparser.
    size_t nwritten = 0;
    const SgAsmInterpretationPtrList &interps = binary->get_interpretations()->get_interpretations();
    for (SgAsmInterpretation *interp: interps) {
        SgAsmGenericFilePtrList interp_files = interp->get_files();
        if (interp_files.size() > 1) {
            const std::string interp_name = (boost::format("interp-%03zu.dump") % nwritten++).str();
            std::ofstream output(interp_name.c_str());
            ASSERT_require(output);
            output <<"Interpretation spanning these input files:\n";
            for (SgAsmGenericFile *interp_file: interp_files)
                output <<"  " <<interp_file->get_name() <<"\n";
            AST::Traversal::forwardPre<SgAsmInstruction>(interp, [&output](SgAsmInstruction *insn) {
                output <<insn->toString() <<"\n";
            });
        }
    }

    /* Generate the rose_*.s (get_unparse_output_filename()) assembly file. It will contain all the interpretations. */
    // [Robb Matzke 2024-10-18]: There are better ways to make assembly listings, but you need a Partitioner object.
    if (binary->get_unparse_output_filename() != "") {
        std::ofstream asm_file(binary->get_unparse_output_filename().c_str());
        if (asm_file) {
            for (size_t i = 0; i < interps.size(); ++i) {
                AST::Traversal::forwardPre<SgAsmInstruction>(interps[i], [&asm_file](SgAsmInstruction *insn) {
                    asm_file <<insn->toString() <<"\n";
                });
            }
        }
    }
}

void
Unparser::unparseFile(SgJvmComposite* jvm, SgUnparse_Info &)
{
  namespace bfs = boost::filesystem;

  ASSERT_not_null(jvm);
  std::vector<SgAsmGenericFile*> files{};

  // Make sure the files are in the file system and not a jar file
  for (auto file : jvm->get_genericFileList()->get_files()) {
    auto path = file->get_name();
    if (bfs::exists(path)) {
      if (!CommandlineProcessing::isJavaJarFile(path)) {
        files.push_back(file);
      }
    }
  }

  // Multiple class files may be included (as SgAsmGenericFile(s)) in one SgJvmComposite instance
  for (SgAsmGenericFile* file : files) {
    std::string filename = bfs::path{file->get_name()}.filename().string();
    if (CommandlineProcessing::isJavaJarFile(filename)) {
      // Don't unparse jar files
      continue;
    }
    SgAsmExecutableFileFormat::unparseBinaryFormat("rose_" + filename, file);
  }
}

#endif // ROSE_ENABLE_BINARY_ANALYSIS

string
unparseStatementWithoutBasicBlockToString(SgStatement* statement)
   {
      string statementString;
      ASSERT_not_null( statement);

   // Build a SgUnparse_Info object to represent formatting options for
   // this statement (use the default values).
      SgUnparse_Info info;

   // exclude comments
      info.set_SkipComments();

   // exclude body and the trailing semicolon
      info.set_SkipBasicBlock();
      info.set_SkipSemiColon();

   // exclude all CPP directives (since they have already been evaluated by the front-end)
      info.set_SkipCPPDirectives();

      switch ( statement->variantT() )
        {
          case V_SgCaseOptionStmt:
          case V_SgDefaultOptionStmt:
               statementString = globalUnparseToString(statement,&info);
          break;

          default:
               printf ("Error, default case in switch within unparseStatementWithoutBasicBlockToString() \n");
        }

     printf ("In unparseStatementWithoutBasicBlockToString(): statementString = %s \n",statementString.c_str());

     return statementString;
   }

string
unparseScopeStatementWithoutBasicBlockToString ( SgScopeStatement* scope )
   {
      string scopeString;

      ASSERT_not_null( scope);
   // printf ("unparseScopeStatementWithoutBasicBlockToString(): scope->sage_class_name() = %s \n",scope->sage_class_name());

   // Build a SgUnparse_Info object to represent formatting options for
   // this statement (use the default values).
      SgUnparse_Info info;

   // exclude comments
      info.set_SkipComments();

   // exclude body and the trailing semicolon
      info.set_SkipBasicBlock();
      info.set_SkipSemiColon();

   // exclude all CPP directives (since they have already been evaluated by the front-end)
      info.set_SkipCPPDirectives();

      switch ( scope->variantT() )
        {
          case V_SgSwitchStatement:
          case V_SgForStatement:
          case V_SgWhileStmt:
          case V_SgDoWhileStmt:
          case V_SgIfStmt:
          case V_SgCatchOptionStmt:
          case V_SgCaseOptionStmt:
               scopeString = globalUnparseToString(scope,&info);
          break;
          default:
               printf ("Error, default case in switch within unparseScopeStatementWithoutBasicBlockToString() \n");
               ROSE_ABORT ();
        }

  // printf ("In unparseScopeStatementWithoutBasicBlockToString(): scopeString = %s \n",scopeString.c_str());

     return scopeString;
   }

string
unparseDeclarationToString ( SgDeclarationStatement* declaration, bool unparseAsDeclaration )
   {
  // This function generates a string for a declaration. The string is required for
  // the intermediate file to make sure that all transformation code will compile
  // (since it could depend on declarations defined within the code).

  // Details:
  //   1) Only record declarations found within the source file (exclude all header files
  //      since they will be seen when the same header files are included).
  //   2) Resort the variable declarations to remove redundent entries.
  //        WRONG: variable declarations could have dependences upon class declarations!
  //   3) Don't sort all declarations since some could have dependences.
  //        a) class declarations
  //        b) typedefs
  //        c) function declarations
  //        d) template declarations
  //        e) variable definition???

  // ASSERT_this();
     ASSERT_not_null( declaration);

#if 0
     printf ("generateDeclarationToString(): unparseAsDeclaration = %s declaration->sage_class_name() = %s \n",
     unparseAsDeclaration ? "true" : "false",declaration->sage_class_name());
#endif

     string declarationString;

  // Build a SgUnparse_Info object to represent formatting options for
  // this statement (use the default values).
     SgUnparse_Info info;

   // exclude comments
      info.set_SkipComments();

   // exclude all CPP directives (since they have already been evaluated by the front-end)
      info.set_SkipCPPDirectives();

      switch ( declaration->variantT() )
        {
       // Enum declarations should not skip their definition since
       // this is where the constants are declared.
          case V_SgEnumDeclaration:

          case V_SgVariableDeclaration:
          case V_SgTemplateDeclaration:
          case V_SgTypedefDeclaration:
            // Need to figure out if a forward declaration would work or be
            // more conservative and always output the complete class definition.

            // turn off output of initializer values
               info.set_SkipInitializer();
            // output the declaration as a string
               declarationString = globalUnparseToString(declaration,&info);
               break;

          case V_SgClassDeclaration:
            // Need to figure out if a forward declaration would work or be
            // more conservative and always output the complete class definition.

            // turn off the generation of the function definitions only
            // (we still want the restof the class definition since these
            // define all member data and member functions).
               info.set_SkipFunctionDefinition();
               info.set_AddSemiColonAfterDeclaration();

               if (unparseAsDeclaration == false)
                  {
                    info.set_SkipClassDefinition();
                  }
                 else
                  {
//                  info.set_AddSemiColonAfterDeclaration();
                  }

            // output the declaration as a string
               declarationString = globalUnparseToString(declaration,&info);
               break;

       // For functions just output the declaration and skip the definition
       // (This also avoids the generation of redundent definitions since the
       // function we are in when we generate all declarations would be included).
          case V_SgMemberFunctionDeclaration:
          case V_SgFunctionDeclaration:
             {
            // turn off the generation of the definition
               info.set_SkipFunctionDefinition();

            // unparse with the ";" (as a declaration) or unparse without the ";"
            // as a function for which we will attach a local scope (basic block).
            // printf ("In generateDeclarationString(): unparseAsDeclaration = %s \n",(unparseAsDeclaration) ? "true" : "false");
               if (unparseAsDeclaration == true)
                    info.set_AddSemiColonAfterDeclaration();

            // output the declaration as a string
               declarationString = globalUnparseToString(declaration,&info);
               break;
             }

          case V_SgFunctionParameterList:
             {
            // Handle generation of declaration strings this case differently from unparser
            // since want to generate declaration strings and not function parameter lists
            // (function parameter lists would be delimited by "," while declarations would
            // be delimited by ";").
               SgFunctionParameterList* parameterListDeclaration = dynamic_cast<SgFunctionParameterList*>(declaration);
               ASSERT_not_null(parameterListDeclaration);
               SgInitializedNamePtrList & argList = parameterListDeclaration->get_args();
               SgInitializedNamePtrList::iterator i;
               for (i = argList.begin(); i != argList.end(); i++)
                  {
//                  printf ("START: Calling unparseToString on type! \n");
                    ASSERT_not_null((*i));
                    string typeNameString = (*i)->get_type()->unparseToString();
//                  printf ("DONE: Calling unparseToString on type! \n");

                    string variableName;
                    if ( (*i)->get_name().getString() != "")
                       {
                         variableName   = (*i)->get_name().str();
                         declarationString += typeNameString + " " + variableName + "; ";
                       }
                      else
                       {
                      // Don't need the tailing ";" if there is no variable name (I think)
                         declarationString += typeNameString + " ";
                       }
                  }
               break;
             }

       // ignore this case ... not really a declaration
          case V_SgCtorInitializerList:
            // printf ("Ignore the SgCtorInitializerList (constructor initializer list) \n");
               break;

          case V_SgVariableDefinition:
               printf ("ERROR: SgVariableDefinition nodes not used in AST \n");
               ROSE_ABORT();
               break;

       // DQ (7/31/2006): Suggested additions by Markus Schordan.
          case V_SgPragma:
               break;
          case V_SgPragmaDeclaration:
               break;

       // default case should always be an error
          default:
               printf ("Default reached in AST_Rewrite::AccumulatedDeclarationsAttribute::generateDeclarationString() \n");
               printf ("     declaration->sage_class_name() = %s \n",declaration->sage_class_name());
               ROSE_ABORT();
               break;
        }

  // Add a space to make it easier to read (not required)
     declarationString += " ";

  // printf ("For this scope: declarationString = %s \n",declarationString.c_str());

     return declarationString;
   }


string
Unparser::removeUnwantedWhiteSpace ( const string & X )
   {
     string returnString;
     int stringLength = X.length();

     for (int i=0; i < stringLength; i++)
        {
          if ( (X[i] != ' ') && (X[i] != '\n') )
             {
               returnString += X[i];
             }
            else
             {
               if ( (i > 0) && (X[i] == ' ') &&
                  ( (X[i-1] != ' ') && (X[i-1] != '\n') && (X[i-1] != '{') && (X[i-1] != ';') && (X[i-1] != '}')) )
                  {
                    if ( (i < stringLength-1) && (X[i] == ' ') && (X[i+1] != '(') )
                         returnString += X[i];
                  }
             }
        }

     return returnString;
   }


// DQ (12/5/2006): Output separate file containing source position information for highlighting (useful for debugging).
int
Unparser::get_embedColorCodesInGeneratedCode()
   {
     return embedColorCodesInGeneratedCode;
   }

int
Unparser::get_generateSourcePositionCodes()
   {
     return generateSourcePositionCodes;
   }

void
Unparser::set_embedColorCodesInGeneratedCode( int x )
   {
     embedColorCodesInGeneratedCode = x;
   }

void
Unparser::set_generateSourcePositionCodes( int x )
   {
     generateSourcePositionCodes = x;
   }

void
Unparser::set_resetSourcePosition(bool x)
   {
     p_resetSourcePosition = x;
   }

bool
Unparser::get_resetSourcePosition()
   {
      return p_resetSourcePosition;
   }

// DQ (5/8/2010): Added support to force unparser to reset the source positon in the AST (this is the only side-effect in unparsing).
//! Reset the Sg_File_Info to reference the unparsed (generated) source code.
void
Unparser::resetSourcePosition (SgStatement* stmt)
   {
     static int previous_line_number   = 0;
     static int previous_column_number = 0;

     Sg_File_Info* originalFileInfo = stmt->get_file_info();
     ASSERT_not_null(originalFileInfo);

     if ( SgProject::get_verbose() > 0 )
          printf ("Reset the source code position from %s:%d:%d to ",originalFileInfo->get_filename(),originalFileInfo->get_line(),originalFileInfo->get_col());

  // This is the current output file.
  // string newFilename = "output";
  // Detect reuse of an Unparser object with a different file
     ASSERT_not_null(currentFile);
     string newFilename = get_output_filename(*currentFile);

  // This is the position of the start of the stmt.
     int line = cur.current_line();

  // This is the position of the end of the stmt (likely we can refine this later).
  // int column = cur.current_col();
  // int column = previous_column_number;
     int column = (line == previous_line_number) ? previous_column_number : 0;

  // Save the current position
     previous_line_number   = line;
     previous_column_number = cur.current_col();

     originalFileInfo->set_filenameString(newFilename);
     originalFileInfo->set_line(line);
     originalFileInfo->set_col(column);

     if ( SgProject::get_verbose() > 0 )
          printf ("%s:%d:%d \n",originalFileInfo->get_filename(),originalFileInfo->get_line(),originalFileInfo->get_col());
   }


void
resetSourcePositionToGeneratedCode( SgFile* file, UnparseFormatHelp *unparseHelp )
   {
  // DQ (5/8/2010): This function uses the unparsing operation to record the locations of
  // the unparsed language constructs and reset the source code position to that of the
  // generated code.

  // DQ (4/22/2006): This can be true when the "-E" option is used, but then we should not have called this function!
     ROSE_ASSERT(file->get_skip_unparse() == false);

  // It does not make sense to reset the source file positions for a binary (at least not yet).
     ROSE_ASSERT (file->get_binary_only() == false);

  // If we did unparse an intermediate file then we want to compile that file instead of the original source file.
     string outputFilename;
     if (file->get_unparse_output_filename().empty() == true)
        {
          outputFilename = "rose_" + file->get_sourceFileNameWithoutPath();

          if (file->get_binary_only() == true)
             {
               outputFilename += ".s";
             }

       // DQ (4/2/2011): Java output files must have the same name as the class and so all we can do is use the same name but put the generated file into the compile tree.
       // Note that if the generated file is put into the source tree it will overwite the original source file.
          if (file->get_Java_only() == true)
             {
               outputFilename = file->get_sourceFileNameWithoutPath();

               printf ("Warning, output file name of generated Java code is same as input file name but must be but into a separate directory. \n");
               ROSE_ABORT();
             }
        }
       else
        {
          outputFilename = file->get_unparse_output_filename();
        }

  // Set the output file name, since this may be called before unparse().
     file->set_unparse_output_filename(outputFilename);
     ASSERT_require(file->get_unparse_output_filename().empty() == false);

     printf ("Exiting output file name of generated Java code is same as input file name but must be but into a separate directory. \n");
     ROSE_ABORT();

  // Name the file with a separate extension.
     outputFilename += ".resetSourcePosition";

  // printf ("Inside of resetSourcePositionToGeneratedCode(UnparseFormatHelp*) outputFilename = %s \n",outputFilename.c_str());

     if ( SgProject::get_verbose() > -1 )
          printf ("Calling the resetSourcePositionToGeneratedCode: outputFilename = %s \n",outputFilename.c_str());

     fstream ROSE_OutputFile(outputFilename.c_str(),ios::out);

     if (!ROSE_OutputFile)
        {
       // throw std::exception("(fstream) error while opening file.");
          printf ("Error detected in opening file %s for output \n",outputFilename.c_str());
          ROSE_ABORT();
        }

     ROSE_ASSERT(ROSE_OutputFile);

  // all options are now defined to be false. When these options can be passed in
  // from the prompt, these options will be set accordingly.
     bool UseAutoKeyword                = false;
     bool generateLineDirectives        = file->get_unparse_line_directives();

  // DQ (6/19/2007): note that test2004_24.C will fail if this is false.
  // If false, this will cause A.operator+(B) to be unparsed as "A+B". This is a confusing point!
     bool useOverloadedOperators        = false;

     bool num                           = false;

  // It is an error to have this always turned off (e.g. pointer = this; will not unparse correctly)
     bool _this                         = true;

     bool caststring                    = false;
     bool _debug                        = false;
     bool _class                        = false;
     bool _forced_transformation_format = false;

  // control unparsing of include files into the source file (default is false)
     bool _unparse_includes             = file->get_unparse_includes();

     Unparser_Opt roseOptions( UseAutoKeyword,
                               generateLineDirectives,
                               useOverloadedOperators,
                               num,
                               _this,
                               caststring,
                               _debug,
                               _class,
                               _forced_transformation_format,
                               _unparse_includes );

     Unparser roseUnparser ( &ROSE_OutputFile, file->get_file_info()->get_filenameString(), roseOptions, unparseHelp, nullptr );

  // DQ (12/5/2006): Output information that can be used to colorize properties of generated code (useful for debugging).
     roseUnparser.set_embedColorCodesInGeneratedCode ( file->get_embedColorCodesInGeneratedCode() );
     roseUnparser.set_generateSourcePositionCodes    ( file->get_generateSourcePositionCodes() );

  // This turnes on the mechanism to force resetting the AST's source position information.
     roseUnparser.set_generateSourcePositionCodes(true);

  // information that is passed down through the tree (inherited attribute)
  // SgUnparse_Info inheritedAttributeInfo (NO_UNPARSE_INFO);
     SgUnparse_Info inheritedAttributeInfo;

     SgSourceFile* sourceFile = isSgSourceFile(file);
     ASSERT_not_null(sourceFile);

     roseUnparser.unparseFile(sourceFile,inheritedAttributeInfo);

  // And finally we need to close the file (to flush everything out!)
     ROSE_OutputFile.close();
   }


 /*! \brief This function is the connection from the SgNode::unparseToString() function
            to the unparser.

     This function connects the SgNode::unparseToString() function
     to the unparser.  It takes an optional SgUnparse_Info object pointer.
     If a SgUnparse_Info object is provided then it is not deleted by this
     function.

     \internal Internally this function allocates a SgUnparse_Info object if one is
               not proviede within the function interface.  If the function allocates
               a SgUnparse_Info object it will delete it.  The user is always responcible
               for the allocation and destruction of objects provided to the interface
               of functions.
  */

// DQ (9/13/2014): Added support for unparsing of STL lists (specifically SgTemplateArgumentPtrList and SgTemplateParameterPtrList).
// This allows us to define a simpler API for the name qualification and refactor as much of the support as possible.
// string globalUnparseToString_OpenMPSafe ( const SgNode* astNode, SgUnparse_Info* inputUnparseInfoPointer );
string globalUnparseToString_OpenMPSafe ( const SgNode* astNode, const SgTemplateArgumentPtrList* templateArgumentList,
                                          const SgTemplateParameterPtrList* templateParameterList, SgUnparse_Info* inputUnparseInfoPointer );


string
globalUnparseToString ( const SgNode* astNode, SgUnparse_Info* inputUnparseInfoPointer )
   {
     string returnString;

#if 0
     printf ("Inside of globalUnparseToString(): astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif


// tps (Jun 24 2008) added because OpenMP crashes all the time at the unparser
#if ROSE_GCC_OMP
#pragma omp critical (unparser)
#endif
        {
          if (inputUnparseInfoPointer != nullptr)
             {
#if 0
               printf ("In globalUnparseToString(): inputUnparseInfoPointer->SkipClassDefinition() = %s \n",(inputUnparseInfoPointer->SkipClassDefinition() == true) ? "true" : "false");
               printf ("In globalUnparseToString(): inputUnparseInfoPointer->SkipEnumDefinition()  = %s \n",(inputUnparseInfoPointer->SkipEnumDefinition()  == true) ? "true" : "false");
#endif
            // DQ (1/13/2014): These should have been setup to be the same.
               ROSE_ASSERT(inputUnparseInfoPointer->SkipClassDefinition() == inputUnparseInfoPointer->SkipEnumDefinition());
             }

       // DQ (9/13/2014): Call internal funtion modified to be more general.
       // returnString = globalUnparseToString_OpenMPSafe(astNode,inputUnparseInfoPointer);
          returnString = globalUnparseToString_OpenMPSafe(astNode,nullptr,nullptr,inputUnparseInfoPointer);
        }

     return returnString;
   }


string
globalUnparseToString ( const SgTemplateArgumentPtrList* templateArgumentList, SgUnparse_Info* inputUnparseInfoPointer )
   {
     string returnString;

// tps (Jun 24 2008) added because OpenMP crashes all the time at the unparser
#if ROSE_GCC_OMP
#pragma omp critical (unparser)
#endif
        {
          if (inputUnparseInfoPointer != nullptr)
             {
            // DQ (1/13/2014): These should have been setup to be the same.
               ROSE_ASSERT(inputUnparseInfoPointer->SkipClassDefinition() == inputUnparseInfoPointer->SkipEnumDefinition());
             }

       // DQ (9/13/2014): Call internal funtion modified to be more general.
       // returnString = globalUnparseToString_OpenMPSafe(astNode,inputUnparseInfoPointer);
          returnString = globalUnparseToString_OpenMPSafe(nullptr,templateArgumentList,nullptr,inputUnparseInfoPointer);
        }

     return returnString;
   }


string
globalUnparseToString ( const SgTemplateParameterPtrList* templateParameterList, SgUnparse_Info* inputUnparseInfoPointer )
   {
     string returnString;

// tps (Jun 24 2008) added because OpenMP crashes all the time at the unparser
#if ROSE_GCC_OMP
#pragma omp critical (unparser)
#endif
        {
          if (inputUnparseInfoPointer != nullptr)
             {
            // DQ (1/13/2014): These should have been setup to be the same.
               ROSE_ASSERT(inputUnparseInfoPointer->SkipClassDefinition() == inputUnparseInfoPointer->SkipEnumDefinition());
             }

       // DQ (9/13/2014): Call internal funtion modified to be more general.
       // returnString = globalUnparseToString_OpenMPSafe(astNode,inputUnparseInfoPointer);
          returnString = globalUnparseToString_OpenMPSafe(nullptr,nullptr,templateParameterList,inputUnparseInfoPointer);
        }

     return returnString;
   }


string
globalUnparseToString_OpenMPSafe ( const SgNode* astNode, const SgTemplateArgumentPtrList* templateArgumentList,
                                   const SgTemplateParameterPtrList* templateParameterList, SgUnparse_Info* inputUnparseInfoPointer )
   {
  // This global function permits any SgNode (including it's subtree) to be turned into a string

  // DQ (9/13/2014): Modified the API to be more general (as part of refactoring support for name qualification).
     ROSE_ASSERT(astNode != nullptr || templateArgumentList != nullptr || templateParameterList != nullptr);

     string returnString;

  // all options are now defined to be false. When these options can be passed in
  // from the prompt, these options will be set accordingly.
     bool _auto                         = false;
     bool linefile                      = false;
     bool useOverloadedOperators        = false;
     bool num                           = false;

  // It is an error to have this always turned off (e.g. pointer = this; will not unparse correctly)
     bool _this                         = true;

     bool caststring                    = false;
     bool _debug                        = false;
     bool _class                        = false;
     bool _forced_transformation_format = false;
     bool _unparse_includes             = false;

     Unparser_Opt roseOptions( _auto,
                               linefile,
                               useOverloadedOperators,
                               num,
                               _this,
                               caststring,
                               _debug,
                               _class,
                               _forced_transformation_format,
                               _unparse_includes );


  // DQ (7/19/2007): Remove lineNumber from constructor parameter list.
  // int lineNumber = 0;  // Zero indicates that ALL lines should be unparsed

  // Initialize the Unparser using a special string stream inplace of the usual file stream
     ostringstream outputString;

     const SgLocatedNode* locatedNode = isSgLocatedNode(astNode);
     string fileNameOfStatementsToUnparse;
     if (locatedNode == nullptr)
        {
#if 0
          printf ("WARNING: applying AST -> string for non expression/statement AST objects \n");
#endif
          fileNameOfStatementsToUnparse = "defaultFileNameInGlobalUnparseToString";
        }
       else
        {
          ASSERT_not_null(locatedNode);

       // DQ (5/31/2005): Get the filename from a traversal back through the parents to the SgFile
       // fileNameOfStatementsToUnparse = locatedNode->getFileName();
       // fileNameOfStatementsToUnparse = Rose::getFileNameByTraversalBackToFileNode(locatedNode);
          if (locatedNode->get_parent() == nullptr)
             {
            // DQ (7/29/2005):
            // Allow this function to be called with disconnected AST fragments not connected to
            // a previously generated AST.  This happens in Qing's interface where AST fragements
            // are built and meant to be unparsed.  Only the parent of the root of the AST
            // fragement is expected to be NULL.
            // fileNameOfStatementsToUnparse = locatedNode->getFileName();
               fileNameOfStatementsToUnparse = locatedNode->getFilenameString();
             }
            else
             {
            // DQ (2/20/2007): The expression being unparsed could be one contained in a SgArrayType
               SgArrayType* arrayType = isSgArrayType(locatedNode->get_parent());
               if (arrayType != nullptr)
                  {
                 // If this is an index of a SgArrayType node then handle as a special case
                    fileNameOfStatementsToUnparse = "defaultFileNameInGlobalUnparseToString";
                  }
                 else
                  {
#if 1
                    fileNameOfStatementsToUnparse = Rose::getFileNameByTraversalBackToFileNode(locatedNode);
#else
#error "DEAD CODE!"
#endif
                  }
             }
        }  // end if locatedNode

     ROSE_ASSERT (fileNameOfStatementsToUnparse.size() > 0);

  // Unparser roseUnparser ( &outputString, fileNameOfStatementsToUnparse, roseOptions, lineNumber );
     Unparser roseUnparser ( &outputString, fileNameOfStatementsToUnparse, roseOptions );

  // Information that is passed down through the tree (inherited attribute)
  // Use the input SgUnparse_Info object if it is available.
     SgUnparse_Info* inheritedAttributeInfoPointer = nullptr;

  // DQ (2/18/2013): Keep track of local allocation of the SgUnparse_Info object in this function
  // This is design to fix what appears to be a leak in ROSE (abby-normal growth of the SgUnparse_Info
  // memory pool for compiling large files.
     bool allocatedSgUnparseInfoObjectLocally = false;

     if (inputUnparseInfoPointer != nullptr)
        {
#if 0
          printf ("Using the input inputUnparseInfoPointer object \n");
#endif
       // Use the user provided SgUnparse_Info object
          inheritedAttributeInfoPointer = inputUnparseInfoPointer;
        }
       else
        {
       // DEFINE DEFAULT BEHAVIOUR FOR THE CASE WHEN NO inputUnparseInfoPointer (== NULL) IS
       // PASSED AS ARGUMENT TO THE FUNCTION
#if 0
          printf ("Building a new Unparse_Info object \n");
#endif
       // If no input parameter has been specified then allocate one
       // inheritedAttributeInfoPointer = new SgUnparse_Info (NO_UNPARSE_INFO);
          bool prev_state = SgUnparse_Info::get_forceDefaultConstructorToTriggerError();
          SgUnparse_Info::set_forceDefaultConstructorToTriggerError(false);
          inheritedAttributeInfoPointer = new SgUnparse_Info();
          ASSERT_not_null(inheritedAttributeInfoPointer);
          SgUnparse_Info::set_forceDefaultConstructorToTriggerError(prev_state);

       // DQ (2/18/2013): Keep track of local allocation of the SgUnparse_Info object in this function
          allocatedSgUnparseInfoObjectLocally = true;

       // MS: 09/30/2003: comments de-activated in unparsing
          ROSE_ASSERT (inheritedAttributeInfoPointer->SkipComments() == false);

       // Skip all comments in unparsing
          inheritedAttributeInfoPointer->set_SkipComments();
          ROSE_ASSERT (inheritedAttributeInfoPointer->SkipComments() == true);
       // Skip all whitespace in unparsing (removed in generated string)
          inheritedAttributeInfoPointer->set_SkipWhitespaces();
          ROSE_ASSERT (inheritedAttributeInfoPointer->SkipWhitespaces() == true);

       // Skip all directives (macros are already substituted by the front-end, so this has no effect on those)
          inheritedAttributeInfoPointer->set_SkipCPPDirectives();
          ROSE_ASSERT (inheritedAttributeInfoPointer->SkipCPPDirectives() == true);

#if 1
       // DQ (8/1/2007): Test if we can force the default to be to unparse fully qualified names.
       // printf ("Setting the default to generate fully qualified names, astNode = %p = %s \n",astNode,astNode->class_name().c_str());
          inheritedAttributeInfoPointer->set_forceQualifiedNames();

       // DQ (8/6/2007): Avoid output of "public", "private", and "protected" in front of class members.
       // This does not appear to have any effect, because it it explicitly set in the unparse function
       // for SgMemberFunctionDeclaration.
          inheritedAttributeInfoPointer->unset_CheckAccess();

       // DQ (8/1/2007): Only try to set the current scope to the SgGlobal scope if this is NOT a SgProject or SgFile
          if ( (isSgProject(astNode) != nullptr || isSgFile(astNode) != nullptr ) == false )
             {
            // This will be set to NULL where astNode is a SgType!
               inheritedAttributeInfoPointer->set_current_scope(TransformationSupport::getGlobalScope(astNode));
             }
#endif

       // DQ (5/19/2011): Allow compiler generated statements to be unparsed by default.
          inheritedAttributeInfoPointer->set_outputCompilerGeneratedStatements();

       // DQ (1/10/2015): Add initialization of the current_source_file.
       // This is required where this function is called from the name qualification support.
          SgSourceFile* sourceFile = TransformationSupport::getSourceFile(astNode);
       // ASSERT_not_null(sourceFile);
          if (sourceFile == nullptr)
             {
#if 0
            // DQ (1/12/2015): This message it commented out, it is frequently triggered for expression IR nodes (SgNullExpression, SgIntVal, SgTemplateParameterVal, SgAddOp, etc.).
               printf ("NOTE: in globalUnparseToString(): TransformationSupport::getSourceFile(astNode = %p = %s) == NULL \n",astNode,astNode->class_name().c_str());
#endif
             }

          inheritedAttributeInfoPointer->set_current_source_file(sourceFile);
        }

     ASSERT_not_null(inheritedAttributeInfoPointer);
     SgUnparse_Info & inheritedAttributeInfo = *inheritedAttributeInfoPointer;

  // DQ (1/6/2021): Adding support to detect use of unparseToString() functionality.  This is required to avoid premature saving of state
  // regarding the static previouslyUnparsedTokenSubsequences which is required to support multiple statements (e.g. a variable declarations
  // with containing multiple variables which translates (typically) to multiple variable declarations (each with one variable) within the AST).
     inheritedAttributeInfoPointer->set_usedInUparseToStringFunction();

  // DQ (5/27/2007): Commented out, uncomment when we are ready for Robert's new hidden list mechanism.
  // if (inheritedAttributeInfo.get_current_scope() == NULL)
     if (astNode != nullptr && inheritedAttributeInfo.get_current_scope() == nullptr)
        {
#if 0
          printf ("In globalUnparseToString(): inheritedAttributeInfo.get_current_scope() == NULL astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif
       // DQ (6/2/2007): Find the nearest containing scope so that we can fill in the current_scope, so that the name qualification can work.
          SgStatement* stmt = TransformationSupport::getStatement(astNode);

#if 0
       // DQ (1/3/2020): Handle the case of a type (see Cxx11_tests/test2020_07.C).
          if (stmt == nullptr)
            {
#if 0
              printf ("If astNode is not a statement, then check it is a type and compute the associated declaration of the type \n");
#endif
           // This could be any named type.
              const SgTypedefType* typedefType = isSgTypedefType(astNode);
              if (typedefType != nullptr)
                 {
                   SgDeclarationStatement* associatedDeclaration = typedefType->getAssociatedDeclaration();
                   ASSERT_not_null(associatedDeclaration);
                   SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(associatedDeclaration);
                   ASSERT_not_null(typedefDeclaration);
                   stmt = typedefDeclaration;
                 }
            }
#endif

       // DQ (6/27/2007): If we unparse a type then we can't find the enclosing statement, so
       // assume it is SgGlobal. But how do we find a SgGlobal IR node to use?  So we have to
       // leave it NULL and hand this case downstream!
       // TV (05/24/2018): in the case of template arguments the statement's parent might not have been set (template argument are unparsed to qualify names for lookup when translating from EDG to SAGE)
          SgScopeStatement* scope = isSgScopeStatement(stmt);
          if (scope == nullptr && stmt != nullptr) {
            scope = stmt->get_scope();
          }
#if 0
          if (scope == nullptr) {
            printf("NOTE: in globalUnparseToString(): failed to find the nearest containing scope for %p (%s)\n", astNode, astNode ? astNode->class_name().c_str() : "");
            printf("  --- astNode->get_parent() %p (%s)\n", astNode->get_parent(), astNode->get_parent() ? astNode->get_parent()->class_name().c_str() : "");
          }
#endif

#if 0
          printf ("In globalUnparseToString(): scope = %p \n",scope);
          if (scope != nullptr)
             {
               printf ("In globalUnparseToString(): scope = %p = %s \n",scope,scope->class_name().c_str());
             }
#endif
          inheritedAttributeInfo.set_current_scope(scope);

          const SgTemplateArgument* templateArgument = isSgTemplateArgument(astNode);
          if (templateArgument != nullptr)
             {
#if 0
            // DQ (9/15/2012): Commented this out since while we build the AST we don't have parents of classes set (until the class declaration is attached to the AST).
               SgScopeStatement* scope = templateArgument->get_scope();
            // printf ("SgTemplateArgument case: scope = %p = %s \n",scope,scope->class_name().c_str());
               inheritedAttributeInfo.set_current_scope(scope);
#else

// DQ (5/25/2013): Commented out this message (too much output spew for test codes, debugging test2013_191.C).
// #ifdef ROSE_DEBUG_NEW_EDG_ROSE_CONNECTION
#if 0
               printf ("Skipping set of inheritedAttributeInfo.set_current_scope(scope); for SgTemplateArgument \n");
#endif
#endif
             }
       // stmt->get_startOfConstruct()->display("In unparseStatement(): info.get_current_scope() == NULL: debug");
       // ROSE_ASSERT(false);
        }
  // ASSERT_not_null(info.get_current_scope());

  // Turn ON the error checking which triggers an error if the default SgUnparse_Info constructor is called
  // SgUnparse_Info::forceDefaultConstructorToTriggerError = true;

  // DQ (10/19/2004): Cleaned up this code, remove this dead code after we are sure that this worked properly
  // Actually, this code is required to be this way, since after this branch the current function returns and
  // some data must be cleaned up differently!  So put this back and leave it this way, and remove the
  // "Implementation Note".

#if 0
     printf ("In globalUnparseToString(): astNode = %p \n",astNode);
     if (astNode != nullptr)
        {
          printf ("In globalUnparseToString(): astNode = %p = %s \n",astNode,astNode->class_name().c_str());
        }
#endif

#if 0
     printf ("In globalUnparseToString_OpenMPSafe(): inheritedAttributeInfo.SkipClassDefinition() = %s \n",(inheritedAttributeInfo.SkipClassDefinition() == true) ? "true" : "false");
     printf ("In globalUnparseToString_OpenMPSafe(): inheritedAttributeInfo.SkipEnumDefinition()  = %s \n",(inheritedAttributeInfo.SkipEnumDefinition()  == true) ? "true" : "false");
#endif

  // DQ (1/13/2014): These should have been setup to be the same.
     ROSE_ASSERT(inheritedAttributeInfo.SkipClassDefinition() == inheritedAttributeInfo.SkipEnumDefinition());

  // Both SgProject and SgFile are handled via recursive calls
     if ( (isSgProject(astNode) != nullptr) || (isSgSourceFile(astNode) != nullptr) )
        {
       // printf ("Implementation Note: Put these cases (unparsing the SgProject and SgFile into the cases for nodes derived from SgSupport below! \n");

       // Handle recursive call for SgProject
          const SgProject* project = isSgProject(astNode);
          if (project != nullptr)
             {
               for (int i = 0; i < project->numberOfFiles(); i++)
                  {
                 // SgFile* file = &(project->get_file(i));
                    SgFile* file = project->get_fileList()[i];
                    ASSERT_not_null(file);
                    string unparsedFileString = globalUnparseToString_OpenMPSafe(file,nullptr,nullptr,inputUnparseInfoPointer);
                 // string prefixString       = string("/* TOP:")      + string(Rose::getFileName(file)) + string(" */ \n");
                 // string suffixString       = string("\n/* BOTTOM:") + string(Rose::getFileName(file)) + string(" */ \n\n");
                    string prefixString       = string("/* TOP:")      + file->getFileName() + string(" */ \n");
                    string suffixString       = string("\n/* BOTTOM:") + file->getFileName() + string(" */ \n\n");
                    returnString += prefixString + unparsedFileString + suffixString;
                  }
             }

       // Handle recursive call for SgFile
          const SgSourceFile* file = isSgSourceFile(astNode);
          if (file != nullptr)
             {
               SgGlobal* globalScope = file->get_globalScope();
               ASSERT_not_null(globalScope);
               returnString = globalUnparseToString_OpenMPSafe(globalScope,nullptr,nullptr,inputUnparseInfoPointer);
             }
        }
       else
        {
          if (isSgStatement(astNode) != nullptr)
             {
               const SgStatement* stmt = isSgStatement(astNode);

               if (SageInterface::is_Fortran_language())
                  {
                    ASSERT_not_null(roseUnparser.u_fortran_locatedNode);
                    roseUnparser.u_fortran_locatedNode->unparseStatement ( const_cast<SgStatement*>(stmt), inheritedAttributeInfo );
                  }
               else if (SageInterface::is_Ada_language())
                  {
                    Unparse_Ada adagen{&roseUnparser, ""};

                    adagen.unparseStatement( const_cast<SgStatement*>(stmt), inheritedAttributeInfo );
                  }
                 else
                  {
                 // Unparse as a C/C++ code.
                 // DQ (12/13/2018): Adding logic to skip cases where the SgGlobal can not be associate with SgSourceFile.
                    SgGlobal* globalScope = isSgGlobal(const_cast<SgStatement*>(stmt));
                    bool skipCallToUnparseStatement = false;
                    if (globalScope != nullptr)
                       {
                         ASSERT_not_null(globalScope->get_parent());
                         SgProject* project = isSgProject(globalScope->get_parent());
                         if (project != nullptr) {
                           skipCallToUnparseStatement = true;
                         }
                       }
                    ASSERT_not_null(roseUnparser.u_exprStmt);

                    if (skipCallToUnparseStatement == false) {
                      roseUnparser.u_exprStmt->unparseStatement(const_cast<SgStatement*>(stmt), inheritedAttributeInfo);
                    }
                  }
             }

          if (isSgExpression(astNode) != nullptr)
             {
               const SgExpression* expr = isSgExpression(astNode);

               if (SageInterface::is_Fortran_language())
                  {
                    ASSERT_not_null(roseUnparser.u_fortran_locatedNode);
                    roseUnparser.u_fortran_locatedNode->unparseExpression(const_cast<SgExpression*>(expr), inheritedAttributeInfo);
                  }
               else if (SageInterface::is_Jovial_language())
                  {
                    UnparseJovial jovial{&roseUnparser, ""};
                    jovial.unparseExpression(const_cast<SgExpression*>(expr), inheritedAttributeInfo);
                  }
               else if (SageInterface::is_Ada_language())
                  {
                    Unparse_Ada   adagen{&roseUnparser, ""};
                    SgExpression* exprNonConst = const_cast<SgExpression*>(expr);
                    bool          replNull = !inheritedAttributeInfo.get_current_scope();

                    if (replNull)
                      adagen.setInitialScope(inheritedAttributeInfo, exprNonConst);

                    adagen.unparseExpression( exprNonConst, inheritedAttributeInfo );

                    if (replNull)
                      adagen.setInitialScope(inheritedAttributeInfo, nullptr);
                  }
                 else
                  {
                 // Unparse as a C/C++ code.
                    ASSERT_not_null(roseUnparser.u_exprStmt);
                    roseUnparser.u_exprStmt->unparseExpression ( const_cast<SgExpression*>(expr), inheritedAttributeInfo );
                  }
             }

          if (isSgType(astNode) != nullptr)
             {
               const SgType* type = isSgType(astNode);
               ROSE_ASSERT(inheritedAttributeInfo.SkipClassDefinition() == inheritedAttributeInfo.SkipEnumDefinition());

            // DQ (9/6/2010): Added support to detect use of C (default) or Fortran code.
            // DQ (2/2/2007): Note that we should modify the unparser to take the IR nodes as const pointers, but this is a bigger job than I want to do now!
#if 1
               // PP (07/31/1973): check for Ada language
               if (SageInterface::is_Ada_language())
                  {
                    Unparse_Ada   adagen{&roseUnparser, ""};
                    SgType*       tyNonConst = const_cast<SgType*>(type);
                    bool          replNull = !inheritedAttributeInfo.get_current_scope();

                    adagen.unparseType( tyNonConst, inheritedAttributeInfo );

                    if (replNull)
                      adagen.setInitialScope(inheritedAttributeInfo, nullptr);
                  }
               else
                  {
                     ASSERT_not_null(roseUnparser.u_type);
                     roseUnparser.u_type->unparseType ( const_cast<SgType*>(type), inheritedAttributeInfo );
                  }
#else
               if (SageInterface::is_Fortran_language() == true)
                  {
                 // Unparse as a Fortran code.
                    roseUnparser.u_fortran_locatedNode->unparseType ( const_cast<SgType*>(type), inheritedAttributeInfo );
#error "DEAD CODE!"
                  }
                 else
                  {
                 // Unparse as a C/C++ code.
                    roseUnparser.u_type->unparseType ( const_cast<SgType*>(type), inheritedAttributeInfo );
                  }
#endif
#if 0
               string type_string = outputString.str();
            // printf ("outputString = %s \n",outputString.str());
               printf ("type_string = %s \n",type_string.c_str());
#endif
             }

          if (isSgSymbol(astNode) != nullptr)
             {
               const SgSymbol* symbol = isSgSymbol(astNode);

            // DQ (2/2/2007): Note that we should modify the unparser to take the IR nodes as const pointers, but this is a bigger job than I want to do now!
               ASSERT_not_null(roseUnparser.u_sym);
               roseUnparser.u_sym->unparseSymbol ( const_cast<SgSymbol*>(symbol), inheritedAttributeInfo );
             }

          if (isSgSupport(astNode) != nullptr)
             {
            // Handle different specific cases derived from SgSupport
            // (e.g. template parameters and template arguments).
               ASSERT_not_null(astNode);
               switch (astNode->variantT())
                  {
#if 0
                    case V_SgProject:
                       {
                         SgProject* project = isSgProject(astNode);
                         ASSERT_not_null(project);
                         for (int i = 0; i < project->numberOfFiles(); i++)
                            {
                              SgFile* file = &(project->get_file(i));
                              ASSERT_not_null(file);
                              string unparsedFileString = globalUnparseToString_OpenMPSafe(file,inputUnparseInfoPointer);
                              string prefixString       = string("/* TOP:")      + string(Rose::getFileName(file)) + string(" */ \n");
                              string suffixString       = string("\n/* BOTTOM:") + string(Rose::getFileName(file)) + string(" */ \n\n");
                              returnString += prefixString + unparsedFileString + suffixString;
                            }
                         break;
                       }
#error "DEAD CODE!"
                 // case V_SgFile:
                       {
                         SgFile* file = isSgFile(astNode);
                         ASSERT_not_null(file);
                         SgGlobal* globalScope = file->get_globalScope();
                         ASSERT_not_null(globalScope);
                         returnString = globalUnparseToString_OpenMPSafe(globalScope,inputUnparseInfoPointer);
                         break;
                       }
#endif
                    case V_SgTemplateParameter:
                       {
                         const SgTemplateParameter* templateParameter = isSgTemplateParameter(astNode);

                      // DQ (2/2/2007): Note that we should modify the unparser to take the IR nodes as const pointers, but this is a bigger job than I want to do now!
                         ASSERT_not_null(roseUnparser.u_exprStmt);
                         roseUnparser.u_exprStmt->unparseTemplateParameter(const_cast<SgTemplateParameter*>(templateParameter),inheritedAttributeInfo);
                         break;
                       }

                    case V_SgTemplateArgument:
                       {
                         const SgTemplateArgument* templateArgument = isSgTemplateArgument(astNode);
#if 0
                      // printf ("In globalUnparseToString_OpenMPSafe(): case V_SgTemplateArgument (before): returnString = %s outputString = %s \n",returnString.c_str(),outputString.str());
                         printf ("In globalUnparseToString_OpenMPSafe(): case V_SgTemplateArgument (before): returnString = %s outputString = %s \n",returnString.c_str(),outputString.str().c_str());
                      // printf ("In globalUnparseToString_OpenMPSafe(): case V_SgTemplateArgument (before): returnString = %s \n",returnString.c_str());
#endif
                      // DQ (2/2/2007): Note that we should modify the unparser to take the IR nodes as const pointers, but this is a bigger job than I want to do now!
                         ASSERT_not_null(roseUnparser.u_exprStmt);
                         roseUnparser.u_exprStmt->unparseTemplateArgument(const_cast<SgTemplateArgument*>(templateArgument),inheritedAttributeInfo);

                      // printf ("In globalUnparseToString_OpenMPSafe(): case V_SgTemplateArgument (after): returnString = %s outputString = %s \n",returnString.c_str(),outputString.str());
                      // printf ("In globalUnparseToString_OpenMPSafe(): case V_SgTemplateArgument (after): returnString = %s outputString = %s \n",returnString.c_str(),outputString.str());
                      // printf ("In globalUnparseToString_OpenMPSafe(): case V_SgTemplateArgument (after): returnString = %s \n",returnString.c_str());

#if 0
                         string local_returnString = outputString.str();
                         printf ("In globalUnparseToString_OpenMPSafe(): case V_SgTemplateArgument (after): local_returnString = %s \n",local_returnString.c_str());
#endif
                         break;
                       }

                    case V_Sg_File_Info:
                       {
                      // DQ (8/5/2007): This is implemented above as a special case!
                      // DQ (5/11/2006): Not sure how or if we should implement this
                         break;
                       }

                    case V_SgPragma:
                       {
                         const SgPragma* pr = isSgPragma(astNode);
                         SgPragmaDeclaration* decl = isSgPragmaDeclaration(pr->get_parent());
                         ROSE_ASSERT (decl);
                         ASSERT_not_null(roseUnparser.u_exprStmt);
                         roseUnparser.u_exprStmt->unparseStatement ( decl, inheritedAttributeInfo );
                         break;
                       }

                    case V_SgFileList:
                       {
                      // DQ (1/23/2010): Not sure how or if we should implement this
                         const SgFileList* fileList = isSgFileList(astNode);
                         ASSERT_not_null(fileList);
#if 0
                         for (int i = 0; i < project->numberOfFiles(); i++)
                            {
                              SgFile* file = &(project->get_file(i));
                              ASSERT_not_null(file);
                              string unparsedFileString = globalUnparseToString_OpenMPSafe(file,inputUnparseInfoPointer);
                              string prefixString       = string("/* TOP:")      + string(Rose::getFileName(file)) + string(" */ \n");
                              string suffixString       = string("\n/* BOTTOM:") + string(Rose::getFileName(file)) + string(" */ \n\n");
                              returnString += prefixString + unparsedFileString + suffixString;
                            }
#else
                         printf ("WARNING: SgFileList support not implemented for unparser...\n");
#endif
                         break;
                       }

                 // Perhaps the support for SgFile and SgProject shoud be moved to this location?
                    default:
                       {
                         printf ("Error: default reached in node derived from SgSupport astNode = %s \n",astNode->class_name().c_str());

                      // DQ (4/12/2019): Calling ROSE_ASSERT() is more useful in debugging than calling ROSE_ABORT().
                      // ROSE_ABORT();
                         ROSE_ABORT();
                       }
                  }
             }

          if (astNode == nullptr)
             {
            // DQ (9/13/2014): This is where we could put support for when astNode == NULL, and the input was an STL list of IR node pointers.
               if (templateArgumentList != nullptr)
                  {
#if 0
                    printf ("Detected SgTemplateArgumentPtrList: templateArgumentList = %p size = %zu \n",templateArgumentList,templateArgumentList->size());
#endif
                    roseUnparser.u_exprStmt->unparseTemplateArgumentList(*templateArgumentList, inheritedAttributeInfo );
                  }

               if (templateParameterList != nullptr)
                  {
#if 0
                    printf ("Detected SgTemplateParameterPtrList: templateParameterList = %p size = %zu \n",templateParameterList,templateParameterList->size());
#endif
                    roseUnparser.u_exprStmt->unparseTemplateParameterList(*templateParameterList, inheritedAttributeInfo );
                  }
             }

       // Liao 11/5/2010 move out of SgSupport
          if (isSgInitializedName(astNode)) //       case V_SgInitializedName:
             {
            // DQ (8/6/2007): This should just unparse the name (fully qualified if required).
            // QY: not sure how to implement this
            // DQ (7/23/2004): This should unparse as a declaration (type and name with initializer).
               const SgInitializedName* initializedName = isSgInitializedName(astNode);
            // roseUnparser.get_output_stream() << initializedName->get_qualified_name().str();
               SgScopeStatement* scope = initializedName->get_scope();
               if (isSgGlobal(scope) == nullptr && scope->containsOnlyDeclarations() == true)
                  {
                    roseUnparser.get_output_stream() << roseUnparser.u_exprStmt->trimGlobalScopeQualifier ( scope->get_qualified_name().getString() ) << "::";
                  }
               roseUnparser.get_output_stream() << initializedName->get_name().str();
            // break;
             }


       // Liao, 8/28/2009, support for SgLocatedNodeSupport
          if (isSgLocatedNodeSupport(astNode) !=  nullptr)
             {
               if (isSgOmpClause(astNode))
                  {
                    SgOmpClause * omp_clause = const_cast<SgOmpClause*>(isSgOmpClause(astNode));
                    ROSE_ASSERT(omp_clause);

                    ASSERT_not_null(roseUnparser.u_exprStmt);
                    roseUnparser.u_exprStmt->unparseOmpClause(omp_clause, inheritedAttributeInfo);
                  }
             }

       // Turn OFF the error checking which triggers an if the default SgUnparse_Info constructor is called
       // GB (09/27/2007): Removed this error check, see above.
       // SgUnparse_Info::set_forceDefaultConstructorToTriggerError(false);

       // MS: following is the rewritten code of the above outcommented
       //     code to support ostringstream instead of ostrstream.
          returnString = outputString.str();

       // Call function to tighten up the code to make it more dense
          if (inheritedAttributeInfo.SkipWhitespaces() == true)
             {
               returnString = roseUnparser.removeUnwantedWhiteSpace ( returnString );
             }
        }

  // delete the allocated SgUnparse_Info object
     if (inputUnparseInfoPointer == nullptr)
        {
          delete inheritedAttributeInfoPointer;
          inheritedAttributeInfoPointer = nullptr;
        }

#if 0
     printf ("In globalUnparseToString_OpenMPSafe(): returnString = %s \n",returnString.c_str());
#endif

  // DQ (2/18/2013): Keep track of local allocation of the SgUnparse_Info object in this function
     if (allocatedSgUnparseInfoObjectLocally == true)
        {
          ROSE_ASSERT(inheritedAttributeInfoPointer == nullptr);
        }

     return returnString;
   }

string get_output_filename( SgFile& file)
   {
     if (file.get_unparse_output_filename().empty() == true)
        {
          printf ("Error: no output file name specified, use \"-o <output filename>\" option on commandline (see --help for options) \n");
        }
     ROSE_ASSERT(file.get_unparse_output_filename().empty() == false);

     return file.get_unparse_output_filename();
   }


// DQ (10/11/2007): I think this is redundant with the Unparser::unparseFile() member function
// HOWEVER, this is called by the SgFile::unparse() member function, so it has to be here!

// Later we might want to move this to the SgProject or SgFile support class (generated by ROSETTA)
void
unparseFile ( SgFile* file, UnparseFormatHelp *unparseHelp, UnparseDelegate* unparseDelegate, SgScopeStatement* unparseScope )
   {
     TimingPerformance timer ("AST Code Generation (unparsing):");
     ASSERT_not_null(file);

  // Call the unparser mechanism

  // FMZ (12/21/2009) the imported files by "use" statements should not be unparsed
     if (file->get_skip_unparse() == true)
        {
       // We need to be careful about this premature return.
          return;
        }

     ROSE_ASSERT(file->get_skip_unparse() == false);

#if 0
#error "DEAD CODE!"

  // If we did unparse an intermediate file then we want to compile that file instead of the original source file.
     if (file->get_unparse_output_filename().empty() == true)
        {
          string outputFilename = "rose_" + file->get_sourceFileNameWithoutPath();
       // DQ (9/15/2013): Added support for generated file to be placed into the same directory as the source file.
          SgProject* project = TransformationSupport::getProject(file);
       // ASSERT_not_null(project);
          if (project != nullptr)
             {
               if (project->get_unparse_in_same_directory_as_input_file() == true)
                  {
                    outputFilename = Rose::getPathFromFileName(file->get_sourceFileNameWithPath()) + "/rose_" + file->get_sourceFileNameWithoutPath();
                  }
             }
            else
             {
               printf ("WARNING: In unparseFile(): file = %p has no associated project \n",file);
             }


#error "DEAD CODE!"

        if (file->get_binary_only() == true)
        {
            // outputFilename = file->get_sourceFileNameWithoutPath();
               outputFilename += ".s";
        }
        // DQ (4/2/2011): Added Java support which requires that the filename for Java match the input file.
// TODO: Remove this !!!
/*
        else if (file->get_Java_only() == true)
        {
                // We try to get the package information back to output the translated source file
                   // in the correct folder structure.
                   ROSE_ASSERT(isSgSourceFile(file) && "Try to unparse an SgFile not being an SgSourceFile using the java unparser");
                   SgGlobal * gs = ((SgSourceFile *) file)->get_globalScope();
                   SgClassDeclaration * packageDecl = NULL;
                   string packageName = "";
                   const SgDeclarationStatementPtrList& globalDecls = gs->get_declarations();
                   for(SgDeclarationStatementPtrList::const_iterator it = globalDecls.begin();
                                   ((it != globalDecls.end()) && (packageDecl == NULL)); it ++) {
                           packageDecl = isSgClassDeclaration(*it);
                           SgClassDefinition * packageDef = packageDecl->get_definition();
                           ASSERT_not_null(packageDef);
                           AstRegExAttribute * attribute = (AstRegExAttribute *) packageDef->getAttribute("translated_package");
                           if (attribute == NULL) {
                                   packageDecl = NULL;
                           } else {
                                   packageName = attribute->expression;
                           }
                   }
                   //NOTE: Default package equals the empty string ""
                   //ROSE_ASSERT((packageDecl != NULL) && "Couldn't find the package definition of the java source file");
                   string outFolder = "";
                   SgProject *project = file->get_project();
                   string ds = project->get_Java_source_destdir();
                   if (ds != "") {
                           outFolder = ds;
                           outFolder += "/";
                   }
                   outFolder += "rose-output/";
                   boost::replace_all(packageName, ".", "/");
                   outFolder += packageName;
                   outFolder += "/";
                   // Create package folder structure
                   string mkdirCommand = string("mkdir -p ") + outFolder;
                   int status = system (mkdirCommand.c_str());
                   ROSE_ASSERT(status == 0);
                   outputFilename = outFolder + file->get_sourceFileNameWithoutPath();
        }
*/

#error "DEAD CODE!"

        else if (file -> get_Java_only() == true) {
            // We try to get the package information back to output the translated source file
            // in the correct folder structure.
            SgSourceFile *sourcefile = isSgSourceFile(file);
            ROSE_ASSERT(sourcefile && "Try to unparse an SgFile not being an SgSourceFile using the java unparser");

            SgProject *project = sourcefile -> get_project();
            ASSERT_not_null(project);

            SgJavaPackageStatement *package_statement = sourcefile -> get_package();
            string package_name = (package_statement ? package_statement -> get_name().getString() : "");
            //NOTE: Default package equals the empty string ""
            //ROSE_ASSERT((packageDecl != NULL) && "Couldn't find the package definition of the java source file");
            string outFolder = "";
            string ds = project -> get_Java_source_destdir();
            if (ds != "") {
                outFolder = ds;
                outFolder += "/";
            }
            outFolder += "rose-output/";
            boost::replace_all(package_name, ".", "/");
            outFolder += package_name;
            outFolder += (package_name.size() > 0 ? "/" : "");
            // Create package folder structure
            boost::filesystem::create_directories(outFolder);
            ROSE_ASSERT(boost::filesystem::exists(outFolder));
            outputFilename = outFolder + file -> get_sourceFileNameWithoutPath();
            // Convert Windows-style paths to POSIX-style.
            #ifdef _MSC_VER
            boost::replace_all(outputFilename, "\\", "/");
            #endif
        }

#error "DEAD CODE!"

        // Liao 12/29/2010, generate cuda source files
        else if (file->get_Cuda_only() == true)
        {
            outputFilename = StringUtility::stripFileSuffixFromFileName (outputFilename);
            outputFilename += ".cu";
        }
        else
        {
            //ROSE_ASSERT (! "Not implemented, or unknown file type");
        }


#error "DEAD CODE!"

       // DQ (9/15/2013): Added assertion.
          ROSE_ASSERT (file->get_unparse_output_filename().empty() == true);

        // TOO1 (3/20/2014): Clobber the original input source file X_X
        //
        //            **CAUTION**RED*ALERT**CAUTION**
        //
        SgSourceFile* source_file = isSgSourceFile(file);
        if (source_file != nullptr)
        {
            if (project->get_unparser__clobber_input_file())
            {
                outputFilename = source_file->get_sourceFileNameWithPath();
                std::cout
                    << "[WARN] [Unparser] Clobbering the original input file: "
                    << outputFilename
                    << std::endl;
            }
        }

#error "DEAD CODE!"

        file->set_unparse_output_filename(outputFilename);
        ROSE_ASSERT (file->get_unparse_output_filename().empty() == false);
     }

#error "DEAD CODE!"

#else
  // DQ (9/7/2017): This is new code to introduce more general language handling to ROSE.

  // Output prefix (typically "rose_").
     string output_filename_prefix;

  // Name of output filename (with prefix).
     string outputFilename;

#if 0
     printf ("In unparseFile(SgFile* file): before switch: file->get_unparse_output_filename() = %s \n",file->get_unparse_output_filename().c_str());
#endif

  // If we did unparse an intermediate file then we want to compile that file instead of the original source file.
     if (file->get_unparse_output_filename().empty() == true)
        {
#if 0
          printf ("In unparseFile(SgFile* file): top of switch: outputFilename not set using default: outputFilename = %s \n",outputFilename.c_str());
#endif

          switch (file->get_outputLanguage())
             {
               case SgFile::e_error_language:
                  {
                    printf ("Error: SgFile::e_error_language detected in unparser \n");
                    ROSE_ABORT();
                  }

               case SgFile::e_default_language:
                  {
                 // printf ("Error: SgFile::e_default_language detected in unparser \n");
                 // ROSE_ASSERT(false);
#if 0
                    printf ("Warning: SgFile::e_default_language detected in unparser \n");
#endif
                  }

               case SgFile::e_C_language:
               case SgFile::e_Cxx_language:
                  {
#if 0
                 // DQ (9/14/2018): Set the output filename.
                 // const string& tmp_outputFileName = FileHelper::concatenatePaths(unparseRootPath, unparseMapEntry -> second);
                 // FileHelper::ensureParentFolderExists(tmp_outputFileName);
                 // unparsedFile -> set_unparse_output_filename(tmp_outputFileName);
                    if (file->get_sourceFileNameWithoutPath().empty() == false)
                       {
                         printf ("In unparseFile(SgFile* file): file->get_sourceFileNameWithoutPath() = %s \n",file->get_sourceFileNameWithoutPath().c_str());
                       }
                      else
                       {
                         printf ("In unparseFile(SgFile* file): file->get_sourceFileNameWithoutPath() IS EMPTY \n");
                       }
#endif
                 // DQ (9/17/2018): When used withouth header file unparsing, this is a valid string.  So likely in needs to be setup else
                 // where so that we have consistancy of how it is setup independent of if the header file unparsing is used or not.
                    ROSE_ASSERT(file->get_sourceFileNameWithoutPath().empty() == false);
                    outputFilename = "rose_" + file->get_sourceFileNameWithoutPath();

                 // Liao 12/29/2010, generate cuda source files
                    if (file->get_Cuda_only() == true)
                       {
                         outputFilename = StringUtility::stripFileSuffixFromFileName (outputFilename);
                         outputFilename += ".cu";
                       }

                    break;
                  }

            // DQ (9/7/2017): Newly added enum value to support binaries (added to general language handling).
               case SgFile::e_Binary_language:
                  {
                    ASSERT_require(file->get_binary_only() == true);
                    outputFilename = "rose_" + file->get_sourceFileNameWithoutPath();
                    outputFilename += ".s";
                    break;
                  }

               case SgFile::e_Fortran_language:
                  {
                    outputFilename = "rose_" + file->get_sourceFileNameWithoutPath();
                    break;
                  }

               case SgFile::e_Java_language:
                  {
                    ROSE_ASSERT(file->get_Java_only() == true);

                 // We try to get the package information back to output the translated source file in the correct folder structure.
                    SgSourceFile *sourcefile = isSgSourceFile(file);
                    ROSE_ASSERT(sourcefile && "Try to unparse an SgFile not being an SgSourceFile using the java unparser");

                    SgProject *project = sourcefile -> get_project();
                    ASSERT_not_null(project);

                    SgJavaPackageStatement *package_statement = sourcefile -> get_package();
                    string package_name = (package_statement ? package_statement -> get_name().getString() : "");

                 // NOTE: Default package equals the empty string ""
                    string outFolder = "";
                    string ds = project -> get_Java_source_destdir();
                    if (ds != "")
                       {
                         outFolder = ds;
                         outFolder += "/";
                       }

                    outFolder += "rose-output/";
                    boost::replace_all(package_name, ".", "/");
                    outFolder += package_name;
                    outFolder += (package_name.size() > 0 ? "/" : "");

                 // Create package folder structure
                    boost::filesystem::create_directories(outFolder);
                    ASSERT_require(boost::filesystem::exists(outFolder));
                    outputFilename = outFolder + file -> get_sourceFileNameWithoutPath();

                 // Convert Windows-style paths to POSIX-style.
#ifdef _MSC_VER
                    boost::replace_all(outputFilename, "\\", "/");
#endif
                    break;
                  }

               case SgFile::e_Jvm_language:
                  {
                    outputFilename = "rose_" + file->get_sourceFileNameWithoutPath();
                    break;
                  }

               case SgFile::e_Promela_language:
                  {
                    printf ("Error: SgFile::e_Promela_language detected in unparser (unparser not implemented, unparsing ignored) \n");
                    break;
                  }

               case SgFile::e_PHP_language:
                  {
                    printf ("Error: SgFile::e_PHP_language detected in unparser (unparser not implemented, unparsing ignored) \n");
                    break;
                  }

               case SgFile::e_Python_language:
                  {
                    printf ("Error: SgFile::e_Python_language detected in unparser (unparser not implemented, unparsing ignored) \n");

                    break;
                  }

               case SgFile::e_Csharp_language:
                  {
                    printf ("Error: SgFile::e_Csharp_language detected in unparser (unparser not implemented, unparsing ignored) \n");
                    break;
                  }

               case SgFile::e_Ada_language:
                  {
                 // GNAT Ada does not allow the flename to be changed, but we can put it into the build tree instead of the source tree (same as Java).
                 // This detail is not a part of the language standard.
                    // RC-285 requested output be produced to a separate directory
                    Rose::Cmdline::Ada::CmdlineSettings settings = Rose::Cmdline::Ada::commandlineSettings();

                    boost::filesystem::create_directory(settings.outputPath);
                    outputFilename = settings.outputPath + "/" + file->get_sourceFileNameWithoutPath();

                    mlog[TRACE] << "Ada output language: outputFilename = " << outputFilename << std::endl;
                    break;
                  }

               case SgFile::e_Jovial_language:
                  {
                    outputFilename = "rose_" + file->get_sourceFileNameWithoutPath();
                    break;
                  }

               case SgFile::e_last_language:
                  {
                    mlog[ERROR] << "SgFile::e_last_language detected in unparser\n";
                    ROSE_ABORT();
                  }

               default:
                  {
                    mlog[ERROR] << "reached in unparser (unknown output language specified)\n";
                    ROSE_ABORT();
                  }
             }

#if 0
       // DQ (9/7/2017): Make sure that binaries are processed through the new general language support.
          ROSE_ASSERT(file->get_binary_only() == false);
#endif

#if 0
          printf ("In unparseFile(SgFile* file): end of switch: outputFilename not set using default: outputFilename = %s \n",outputFilename.c_str());
#endif

       // DQ (9/7/2017): Added support for generated file to be placed into the same directory as the source file.
          SgProject* project = TransformationSupport::getProject(file);

          if (project != nullptr)
             {
#if 0
               printf ("project->get_unparse_in_same_directory_as_input_file() = %s \n",project->get_unparse_in_same_directory_as_input_file() ? "true" : "false");
#endif
               if (project->get_unparse_in_same_directory_as_input_file() == true)
                  {
                 // outputFilename = Rose::getPathFromFileName(file->get_sourceFileNameWithPath()) + "/rose_" + file->get_sourceFileNameWithoutPath();
                    outputFilename = Rose::getPathFromFileName(file->get_sourceFileNameWithPath()) + "/" + output_filename_prefix + file->get_sourceFileNameWithoutPath();
#if 0
                    printf ("Using filename for unparsed file into same directory as input file: outputFilename = %s \n",outputFilename.c_str());
#endif
#if 0
                    printf("Exiting as test! \n");
                    ROSE_ABORT();
#endif
                  }
             }
            else
             {
               printf ("WARNING: In unparseFile(): file = %p has no associated project \n",file);
             }

       // DQ (9/15/2013): Added assertion.
          ROSE_ASSERT (file->get_unparse_output_filename().empty() == true);

          SgSourceFile* source_file = isSgSourceFile(file);
          if (source_file != nullptr)
             {
               ASSERT_not_null(project);
               if (project->get_unparser__clobber_input_file())
                  {
                 // TOO1 (3/20/2014): Clobber the original input source file X_X
                 //
                 //            **CAUTION**RED*ALERT**CAUTION**
                 //

                    outputFilename = source_file->get_sourceFileNameWithPath();
                    mlog[WARN] << "[Unparser] Clobbering the original input file: " << outputFilename << "\n";
                  }
             }

       // Set the output filename in the SgFile IR node.
          file->set_unparse_output_filename(outputFilename);
        }
#endif

     ROSE_ASSERT (file->get_unparse_output_filename().empty() == false);

     if (file->get_skip_unparse() == true)
        {
       // MS: commented out the following output
       // if ( file.get_verbose() == true )
            // printf ("### Rose::skip_unparse == true: Skipping all source code generation by ROSE generated preprocessor! \n");
        }
       else
        {
       // Open the file where we will put the generated code
          string outputFilename = get_output_filename(*file);

          if ( SgProject::get_verbose() > 0 )
             {
               printf ("Calling the unparser: outputFilename = %s \n",outputFilename.c_str());
             }

       // DQ (3/19/2014): Added support for noclobber option.
          boost::filesystem::path output_file = outputFilename;

          bool trigger_file_comparision = false;

          string saved_filename       = outputFilename;
          string alternative_filename = outputFilename + ".noclobber_compare";

          if (boost::filesystem::exists(output_file))
             {
               if ( SgProject::get_verbose() > 0 )
                  {
                    mlog[INFO] << "In unparseFile(SgFile*): (outputFilename) output file exists = " << output_file.string() << "\n";
                  }

               SgProject* project = TransformationSupport::getProject(file);
               ASSERT_not_null(project);

               if (project->get_noclobber_output_file() == true)
                  {
                 // If the file exists then it is an error if -rose:noclobber_output_file option was specified.
                    printf ("\n\n****************************************************************************************************** \n");
                    printf ("Error: the output file already exists, cannot overwrite (-rose:noclobber_output_file option specified) \n");
                    printf ("   --- outputFilename = %s \n",outputFilename.c_str());
                    printf ("****************************************************************************************************** \n\n\n");
                    ROSE_ABORT();
                  }
                 else
                  {
                 // If the file exists then generate an alternative file so that we can compare the new file to the existing file (error if not identical).
                    if (project->get_noclobber_if_different_output_file() == true)
                       {
                      // If the file exists and the generated file is not identical, then it is an error if -rose:noclobber_if_different_output_file option was specified.
                         trigger_file_comparision = true;

                         if ( SgProject::get_verbose() > 0 )
                              printf ("Generate test output in alternative_filename = %s \n",alternative_filename.c_str());

                         outputFilename = alternative_filename;
                       }
                 // Pei-Hung (8/6/2014) appending PID as alternative name to avoid collision
                      else
                       {
                         if (project->get_appendPID() == true)
                            {
                              ostringstream os;
                              #ifdef _MSC_VER
                              os << _getpid();
                              #else
                              os << getpid();
                              #endif
                              unsigned dot = outputFilename.find_last_of(".");
                              outputFilename = outputFilename.substr(0,dot) + "_" + os.str() + outputFilename.substr(dot);
                              if ( SgProject::get_verbose() > 0 )
                                   printf ("Generate test output name with PID = %s \n",outputFilename.c_str());

                            }
                       }
                  }

               file->set_unparse_output_filename(outputFilename);
             }

          fstream ROSE_OutputFile(outputFilename.c_str(), ios::out);

          if (!ROSE_OutputFile.is_open()) {
             mlog[FATAL] << "Error detected in opening file " << outputFilename << " for output\n";
             ROSE_ABORT();
          }

       // This is the new unparser that Gary Lee is developing
       // The goal of this unparser is to provide formatting
       // similar to that of the original application code
#if 0
          if ( file.get_verbose() > 0 )
               printf ("Calling the NEWER unparser mechanism: outputFilename = %s \n",outputFilename);
#endif

       // all options are now defined to be false. When these options can be passed in
       // from the prompt, these options will be set accordingly.
          bool UseAutoKeyword                = false;
          bool generateLineDirectives        = file->get_unparse_line_directives();
          bool useOverloadedOperators        = false;
          bool num                           = false;

       // It is an error to have this always turned off (e.g. pointer = this; will not unparse correctly)
          bool _this                         = true;

          bool caststring                    = false;
          bool _debug                        = false;
          bool _class                        = false;
          bool _forced_transformation_format = false;

       // control unparsing of include files into the source file (default is false)
          bool _unparse_includes             = file->get_unparse_includes();

          Unparser_Opt roseOptions( UseAutoKeyword,
                                    generateLineDirectives,
                                    useOverloadedOperators,
                                    num,
                                    _this,
                                    caststring,
                                    _debug,
                                    _class,
                                    _forced_transformation_format,
                                    _unparse_includes );

          Unparser roseUnparser ( &ROSE_OutputFile, file->get_file_info()->get_filenameString(), roseOptions, unparseHelp, unparseDelegate );

       // Location to turn on unparser specific debugging data that shows up in the output file
       // This prevents the unparsed output file from compiling properly!
       // ROSE_DEBUG = 0;

       // DQ (12/5/2006): Output information that can be used to colorize properties of generated code (useful for debugging).
          roseUnparser.set_embedColorCodesInGeneratedCode ( file->get_embedColorCodesInGeneratedCode() );
          roseUnparser.set_generateSourcePositionCodes    ( file->get_generateSourcePositionCodes() );

       // information that is passed down through the tree (inherited attribute)
       // SgUnparse_Info inheritedAttributeInfo (NO_UNPARSE_INFO);
          SgUnparse_Info inheritedAttributeInfo;

       // DQ (9/24/2013): Set the output language to the inpuse language.
          inheritedAttributeInfo.set_language(file->get_outputLanguage());

       // inheritedAttributeInfo.display("Inside of unparseFile(SgFile* file)");
#if 0
          printf ("In unparseFile(SgFile*): Calling the unparser for SgFile: file = %p = %s \n",file,file->class_name().c_str());
#endif
       // Call member function to start the unparsing process
       // roseUnparser.run_unparser();
       // roseUnparser.unparseFile(file,inheritedAttributeInfo);

       // DQ (9/2/2008): This one way to handle the variations in type
          switch (file->variantT())
             {
               case V_SgSourceFile:
                  {
                    SgSourceFile* sourceFile = isSgSourceFile(file);

                    ROSE_ASSERT(inheritedAttributeInfo.get_current_source_file() == nullptr);

                 // DQ (8/16/2018): Set this here before it is passed into unparseFile().
                    inheritedAttributeInfo.set_current_source_file(sourceFile);

                    ASSERT_not_null(inheritedAttributeInfo.get_current_source_file());

#if 0
                    printf ("In unparseFile(SgFile*): inheritedAttributeInfo.get_current_source_file() = %p filename = %s \n",
                         inheritedAttributeInfo.get_current_source_file(),inheritedAttributeInfo.get_current_source_file()->getFileName().c_str());
#endif
                 // DQ (10/29/2018): I now think we need to support this mechanism of specifying the scope to be unparsed separately.
                 // This is essential to the support for header files representing nested scopes inside of the global scope.
                 // Traversing the global scope does not permit these inner nested scopes to be traversed using the unparser.

                 // DQ (8/16/2018): the more conventional usage is to us a single SgSourceFile and SgGlobal for each header file.
                    roseUnparser.unparseFile(sourceFile,inheritedAttributeInfo, unparseScope);
                    break;
                  }

#ifdef ROSE_ENABLE_BINARY_ANALYSIS
               case V_SgBinaryComposite:
                  {
                    roseUnparser.unparseFile(isSgBinaryComposite(file), inheritedAttributeInfo);
                    break;
                  }
               case V_SgJvmComposite:
                  {
                    roseUnparser.unparseFile(isSgJvmComposite(file), inheritedAttributeInfo);
                    break;
                  }
#endif

               case V_SgUnknownFile:
                  {
                    SgUnknownFile* unknownFile = isSgUnknownFile(file);
                    unknownFile->set_skipfinalCompileStep(true);

                    mlog[WARN] << "Unclear what to unparse from a SgUnknownFile (set skipfinalCompileStep)\n";
                    break;
                  }

               default:
                  {
                    mlog[FATAL] << "Error: default reached in unparser: file = " << file->class_name() << "\n";
                    ROSE_ABORT();
                  }
             }

       // And finally we need to close the file (to flush everything out!)
          ROSE_OutputFile.close();

       // Invoke post-output user-defined callbacks if any.  We must pass the absolute output name because the build system may
       // have changed directories by now and the callback might need to know how this name compares to the top of the build
       // tree.
          if (unparseHelp != nullptr) {
              Rose::FileSystem::Path fullOutputName = Rose::FileSystem::makeAbsolute(outputFilename);
              UnparseFormatHelp::PostOutputCallback::Args args(file, fullOutputName);
              unparseHelp->postOutputCallbacks.apply(true, args);
          }

       // DQ (3/19/2014): If -rose:noclobber_if_different_output, then test the generated file against the original file.
          if (trigger_file_comparision == true)
             {
            // Test the generated file against the previously generated file of the same original name.
            // if different, it is an error, if the same it is OK.

               if ( SgProject::get_verbose() > 0 )
                  {
                    printf ("Testing saved_filename against alternative_filename (using boost::filesystem::equivalent()): \n");
                    printf ("   --- saved_filename       = %s \n",saved_filename.c_str());
                    printf ("   --- alternative_filename = %s \n",alternative_filename.c_str());
                  }

               boost::filesystem::path saved_output_file       = saved_filename;
               boost::filesystem::path alternative_output_file = alternative_filename;

               std::ifstream ifs1(saved_filename.c_str());
               std::ifstream ifs2(alternative_filename.c_str());

               std::istream_iterator<char> b1(ifs1);
               std::istream_iterator<char> b2(ifs2);

               bool files_are_identical = true;

            // Check the sizes, if the same then we have to check the contents.
               if (file_size(saved_output_file) == file_size(alternative_output_file))
                  {
                    size_t filesize = file_size(saved_output_file);

                    if ( SgProject::get_verbose() > 0 )
                       {
                         printf ("   --- files are the same size = %" PRIuPTR " (checking contents) \n",filesize);
                       }

                     size_t counter = 0;
                     files_are_identical = true;
                     while (files_are_identical == true && counter < filesize)
                        {
                       // check for inequality of file data (at least this is portable).
                          if (*b1 != *b2)
                             {
                               if ( SgProject::get_verbose() > 0 )
                                    printf ("...detected file content inequality... \n");

                               files_are_identical = false;
                             }

                          b1++;
                          b2++;
                          counter++;
                        }
                  }
                 else
                  {
                 // If not the same size then they are not the same files.
                    if ( SgProject::get_verbose() > 0 )
                       {
                         size_t saved_filesize       = file_size(saved_output_file);
                         size_t alternative_filesize = file_size(alternative_output_file);
                         printf ("   --- files are not the same size saved_filesize = %" PRIuPTR " alternative_filesize = %" PRIuPTR " \n",saved_filesize,alternative_filesize);
                       }

                    files_are_identical = false;
                  }

               if ( SgProject::get_verbose() > 0 )
                  {
                    printf ("   --- files_are_identical = %s \n",files_are_identical ? "true" : "false");
                  }

               if (files_are_identical == false)
                  {
                    printf ("\n\n****************************************************************************************************** \n");
                    printf ("Error: files are not equivalent: \n");
                    printf ("   --- saved_filename       = %s \n",saved_filename.c_str());
                    printf ("   --- alternative_filename = %s \n",alternative_filename.c_str());
                    printf ("****************************************************************************************************** \n\n\n");

                 // remove the generated file or leave in place to allow users to examine file differences.
                 // boost::filesystem::remove(unparsed_file);

                    ROSE_ABORT();
                  }
             }
        }
   }


void prependIncludeOptionsToCommandLine(SgProject* project, const list<string>& includeCompilerOptions)
   {
     SgStringList argumentList = project -> get_originalCommandLineArgumentList();

#if 0
  // DQ (4/4/2020): Added header file unparsing feature specific debug level.
     if (SgProject::get_unparseHeaderFilesDebug() >= 4)
        {
          printf ("In prependIncludeOptionsToCommandLine(): includeCompilerOptions.size() = %zu \n",includeCompilerOptions.size());
          for (list<string>::const_iterator i = includeCompilerOptions.begin(); i != includeCompilerOptions.end(); i++)
             {
               printf (" --- *i = %s \n",(*i).c_str());
             }
        }
#endif

  // Note: Insert -I options starting from the second argument, because the first argument is the name of the executable.
     argumentList.insert(++argumentList.begin(), includeCompilerOptions.begin(), includeCompilerOptions.end());

     project -> set_originalCommandLineArgumentList(argumentList);
     const SgFilePtrList& fileList = project -> get_fileList();
     for (SgFilePtrList::const_iterator sgFilePtr = fileList.begin(); sgFilePtr != fileList.end(); sgFilePtr++)
        {
          argumentList = (*sgFilePtr) -> get_originalCommandLineArgumentList();
          argumentList.insert(++argumentList.begin(), includeCompilerOptions.begin(), includeCompilerOptions.end());
          (*sgFilePtr) -> set_originalCommandLineArgumentList(argumentList);
       }
   }


// DQ (10/1/2019): This is required for the buildSourceFileForHeaderFile() function below.
namespace EDG_ROSE_Translation
   {
  // DQ (9/18/2018): Declare this map so that we can use it for the unparse header files option.
#if defined(ROSE_BUILD_CXX_LANGUAGE_SUPPORT) && !defined(ROSE_USE_CLANG_FRONTEND)
  // DQ (12/11/2018): Use the definition in the EDG edgRose.C file if C/C++ support IS defined.
     extern std::map<std::string, SgIncludeFile*> edg_include_file_map;
#else
  // DQ (12/11/2018): Allow this to be the definition if C/C++ support is NOT defined.
     extern std::map<std::string, SgIncludeFile*> edg_include_file_map;
#endif
   }


SgSourceFile*
buildSourceFileForHeaderFile(SgProject* project, string includedFileName)
   {
  // When we have not processed all of the header files we need to support specific ones separately.
  // This function supports this separate handlign for individual header files and is part of the
  // header fine unparsing optimization.

     SgSourceFile* include_sourceFile = nullptr;

     ASSERT_not_null(project);

#define DEBUG_BUILD_SOURCE_FILE_FOR_HEADER_FILE 0

#if DEBUG_BUILD_SOURCE_FILE_FOR_HEADER_FILE
     printf ("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n");
     printf ("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n");
     printf ("TOP of buildSourceFileForHeaderFile(): EDG_ROSE_Translation::edg_include_file_map.size() = %zu \n",EDG_ROSE_Translation::edg_include_file_map.size());
     printf (" --- includedFileName = %s \n",includedFileName.c_str());
     printf (" --- project->get_unparse_tokens() = %s \n",project->get_unparse_tokens() ? "true" : "false");
     printf ("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n");
     printf ("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n");
#endif

     ROSE_ASSERT(project->get_files().empty() == false);

  // Getting the first file might not be correct later.
  // SgSourceFile* sourceFile = isSgSourceFile(project->get_files()[0]);
  // ASSERT_not_null(sourceFile);

#if 1
  // DQ (4/4/2020): Added header file unparsing feature specific debug level.
     if (SgProject::get_unparseHeaderFilesDebug() >= 4)
        {
          printf ("$$$$$$$$$$$$$$$$$$$$$$ In buildSourceFileForHeaderFile(): Using the first file in the project file list as the sourceFile! \n");
        }
#endif
#if 0
  // printf (" --- sourceFile = %p filename = %s \n",sourceFile,sourceFile->getFileName().c_str());
#endif

#if 1
  // DQ (4/4/2020): Added header file unparsing feature specific debug level.
     if (SgProject::get_unparseHeaderFilesDebug() >= 4)
        {
          printf ("In buildSourceFileForHeaderFile(): includedFileName = %s \n",includedFileName.c_str());
          printf (" --- EDG_ROSE_Translation::edg_include_file_map.size() = %zu \n",EDG_ROSE_Translation::edg_include_file_map.size());
#if 1
          printf ("EDG_ROSE_Translation::edg_include_file_map entries: \n");
          std::map<std::string, SgIncludeFile*>::iterator i = EDG_ROSE_Translation::edg_include_file_map.begin();
          while (i != EDG_ROSE_Translation::edg_include_file_map.end())
            {
              printf (" --- i->first = %s i->second = %p \n",i->first.c_str(),i->second);
              i++;
            }
#endif
        }
#endif

  // DQ (11/18/2019): Check the flag that indicates that this SgSourceFile has had its CPP directives and comments added.
  // ROSE_ASSERT(sourceFile->get_processedToIncludeCppDirectivesAndComments() == true);

     ROSE_ASSERT(EDG_ROSE_Translation::edg_include_file_map.find(includedFileName) != EDG_ROSE_Translation::edg_include_file_map.end());

     SgIncludeFile* include_file = EDG_ROSE_Translation::edg_include_file_map[includedFileName];
     ASSERT_not_null(include_file);

  // DQ (3/2/2021): This should not have been built yet (we allow for this below).
  // ROSE_ASSERT(include_file->get_source_file() == NULL);

#if DEBUG_BUILD_SOURCE_FILE_FOR_HEADER_FILE
     printf ("In buildSourceFileForHeaderFile(): include_file = %p includedFileName = %s \n",include_file,includedFileName.c_str());
#endif

#if DEBUG_BUILD_SOURCE_FILE_FOR_HEADER_FILE
     printf ("In buildSourceFileForHeaderFile(): EDG_ROSE_Translation::edg_include_file_map.size() = %zu \n",EDG_ROSE_Translation::edg_include_file_map.size());
#endif

  // DQ (5/8/2021): Adding support for the isApplicationFile flag (set from command line option: -rose:applicationRootDirectory.
     if (include_file->get_isApplicationFile() == false)
        {
          printf ("This is not an application file, so we don't need to build a SgSourceFile for this include file: includedFileName = %s \n",includedFileName.c_str());
          return nullptr;
        }

     include_sourceFile = include_file->get_source_file();

#if DEBUG_BUILD_SOURCE_FILE_FOR_HEADER_FILE
     if (include_sourceFile != nullptr)
        {
          printf ("include_sourceFile = %p include_file = %p include_file->get_filename() = %s \n",include_sourceFile,include_file,include_file->get_filename().str());
          printf (" --- include_sourceFile->get_unparse_tokens() = %s \n",include_sourceFile->get_unparse_tokens() ? "true" : "false");
        }
       else
        {
          printf ("include_sourceFile == NULL \n");
        }
#endif

  // DQ (5/8/2021): If the applicationRootDirectory is set and does not match the first part of the filename,
  // then there is no include_sourceFile built., and include_sourceFile->get_isApplicationFile() should be false.
  // DQ (4/11/2021): I think that we should already ahave an associated source file for every header file.
     if (include_sourceFile == nullptr)
        {
          printf ("In buildSourceFileForHeaderFile(): include_sourceFile == NULL: include_file = %p filename = %s \n",include_file,include_file->get_filename().str());
          ROSE_ASSERT(include_file->get_isApplicationFile() == false);
        }
  // ROSE_ASSERT(include_sourceFile != NULL);

  // DQ (10/26/2019): Added debugging code.
     if (include_sourceFile == nullptr)
        {
#if 0
          printf ("In buildSourceFileForHeaderFile(): include_sourceFile == NULL: include_file = %p include_file->get_filename() = %s \n",include_file,include_file->get_filename().str());
#endif
       // DQ (10/26/2019): If it does not exist, then add one here.
       // SgSourceFile* sourceFile = new SgSourceFile();

          string filename = includedFileName;

       // DQ (10/26/2019): This is taken from the edgRose.C file.
          include_sourceFile = new SgSourceFile();
          ASSERT_not_null(include_sourceFile);

       // DQ (2/25/2021): Set the unparse_tokens flag in the SgSourceFile that is associated with the header file.
          include_sourceFile->set_unparse_tokens(project->get_unparse_tokens());
          include_sourceFile->set_sourceFileNameWithPath(filename);
          include_sourceFile->set_startOfConstruct(new Sg_File_Info(filename));

       // DQ (11/10/2018): This needs to be set before we check using SageIII/astPostProcessing/resetParentPointers.C
          include_sourceFile->get_startOfConstruct()->set_parent(include_sourceFile);

       // DQ (9/23/2019): For C/C++ code this should be false (including all headers).
          if (include_sourceFile->get_requires_C_preprocessor() == true)
             {
               include_sourceFile->set_requires_C_preprocessor(false);
             }
          ASSERT_require(include_sourceFile->get_requires_C_preprocessor() == false);

          include_file->set_source_file(include_sourceFile);
          ASSERT_not_null(include_file->get_source_file());

       // DQ (11/10/2018): It might be enough that it is the parent, but build a more explicit connection.
          include_sourceFile->set_associated_include_file(include_file);
          include_sourceFile->set_parent(include_file);

       // DQ (11/10/2018): This might be redundant with the constructor initialization.
          include_sourceFile->set_isHeaderFile(true);
          include_sourceFile->set_isHeaderFileIncludedMoreThanOnce(false);

#if DEBUG_INCLUDE_TREE_SUPPORT
          printf ("$$$$$$$$$$$$$$$$$$ Building new SgSourceFile (to support include file): include_sourceFile = %p filename = %s \n",include_sourceFile,include_sourceFile->getFileName().c_str());
#endif
          ASSERT_not_null(include_file->get_source_file());

#ifdef ROSE_BUILD_CPP_LANGUAGE_SUPPORT
       // DQ (5/20/2020): Collect the Comments and CPP directives so that can be inserted into
       // the AST as part of building the source file fro this include file.
          ROSEAttributesList* returnListOfAttributes = nullptr;

       // DQ (7/4/2020): This function should not be called for binaries (only for C/C++ code).
          returnListOfAttributes = getPreprocessorDirectives(filename);

       // DQ (9/17/2020): Added a use of the variable to prevent compiler warning.
          ROSE_ASSERT(returnListOfAttributes != nullptr);
#endif
        }
       else
        {
#if DEBUG_BUILD_SOURCE_FILE_FOR_HEADER_FILE
          printf ("In buildSourceFileForHeaderFile(): include_sourceFile != NULL: include_file = %p include_file->get_filename() = %s \n",include_file,include_file->get_filename().str());
#endif
       // DQ (2/25/2021): Set the unparse_tokens flag in the SgSourceFile that is associated with the header file.
          include_sourceFile->set_unparse_tokens(project->get_unparse_tokens());

#if DEBUG_BUILD_SOURCE_FILE_FOR_HEADER_FILE
          printf (" --- project->get_unparse_tokens()            = %s \n",project->get_unparse_tokens() ? "true" : "false");
          printf (" --- include_sourceFile->get_unparse_tokens() = %s \n",include_sourceFile->get_unparse_tokens() ? "true" : "false");
#endif
        }
     ASSERT_not_null(include_sourceFile);

     if (include_sourceFile->get_processedToIncludeCppDirectivesAndComments() == false)
        {
          printf ("WARNING: In buildSourceFileForHeaderFile(): include_sourceFile->get_processedToIncludeCppDirectivesAndComments() == false \n");
        }

#if DEBUG_BUILD_SOURCE_FILE_FOR_HEADER_FILE
     printf ("include_sourceFile = %p include_sourceFile->getFileName() = %s \n",include_sourceFile,include_sourceFile->getFileName().c_str());
#endif

  // DQ (4/11/2021): We should already have a valid SgGlobal (global scope).
     ROSE_ASSERT(include_sourceFile->get_globalScope() != nullptr);

#if DEBUG_BUILD_SOURCE_FILE_FOR_HEADER_FILE
     printf ("include_sourceFile->get_globalScope() = %p \n",include_sourceFile->get_globalScope());
#endif

#if DEBUG_BUILD_SOURCE_FILE_FOR_HEADER_FILE
     SgGlobal* headerFileGlobal = include_sourceFile->get_globalScope();
     SgSourceFile* sourceFileFromHeaderFile = isSgSourceFile(headerFileGlobal->get_parent());
     printf ("sourceFileFromHeaderFile = %p \n",sourceFileFromHeaderFile);

     SgGlobal* globalScope_from_include_sourceFile       = include_sourceFile->get_globalScope();
     SgGlobal* globalScope_from_sourceFileFromHeaderFile = include_sourceFile->get_globalScope();

     printf ("globalScope_from_include_sourceFile       = %p \n",globalScope_from_include_sourceFile);
     printf ("globalScope_from_sourceFileFromHeaderFile = %p \n",globalScope_from_sourceFileFromHeaderFile);

     printf ("globalScope_from_include_sourceFile->get_startOfConstruct()->get_filename()       = %s \n",globalScope_from_include_sourceFile->get_startOfConstruct()->get_filename());
     printf ("globalScope_from_sourceFileFromHeaderFile->get_startOfConstruct()->get_filename() = %s \n",globalScope_from_sourceFileFromHeaderFile->get_startOfConstruct()->get_filename());
#endif

#if 0
     printf ("Adding SgGlobal headerFileGlobal = %p to include_sourceFile = %p \n",headerFileGlobal,include_sourceFile);
     printf (" --- headerFileGlobal = %p \n",headerFileGlobal);
     printf (" --- include_file->get_source_file_of_translation_unit() = %p \n",include_file->get_source_file_of_translation_unit());
     printf (" --- include_sourceFile->get_startOfConstruct()->get_filename() = %s \n",include_sourceFile->get_startOfConstruct()->get_filename());
     ASSERT_not_null(include_sourceFile->get_globalScope());
  // ASSERT_not_null(include_sourceFile->get_globalScope()->get_startOfConstruct());
  // printf ("include_sourceFile->get_globalScope()->get_startOfConstruct()->get_filename() = %s \n",include_sourceFile->get_globalScope()->get_startOfConstruct()->get_filename());
#endif

  // DQ (4/11/2021): We are not overwritting the global scope, and a new global scope should
  // not be needed (so we should have a valid source position as a result.
  // DQ (11/20/2019): If we build a new SgGlobal then this will be NULL.
  // ROSE_ASSERT(include_sourceFile->get_globalScope()->get_startOfConstruct() == NULL);
     ROSE_ASSERT(include_sourceFile->get_globalScope()->get_startOfConstruct() != nullptr);

#if 0
  // DQ (4/11/2021): We are not building a new global scope, so we need not give it a source position.
  // headerFileGlobal->set_file_info(unparsedFileInfo);
     string headerFileName = includedFileName;
     Sg_File_Info* startOfConstructFileInfo = new Sg_File_Info(headerFileName, 0,0);
     Sg_File_Info* endOfConstructFileInfo   = new Sg_File_Info(headerFileName, 0,0);

     headerFileGlobal->set_startOfConstruct(startOfConstructFileInfo);
     headerFileGlobal->set_endOfConstruct  (endOfConstructFileInfo);

#error "DEAD CODE!"

     headerFileGlobal->set_parent(include_sourceFile);

  // DQ (10/23/2018): We need to reset the isModified flag for the headerFileGlobal.
     headerFileGlobal->set_isModified(false);

     ASSERT_not_null(headerFileGlobal->get_parent());
     ASSERT_not_null(include_sourceFile->get_globalScope());

  // This should not have been setup yet.
     ROSE_ASSERT(headerFileGlobal->get_declarations().empty() == true);

#error "DEAD CODE!"

  // DQ (11/20/2019): Added test.
     ROSE_ASSERT(include_sourceFile->get_globalScope()->get_declarations().empty() == true);

     SgSourceFile* sourceFile = include_file->get_source_file_of_translation_unit();
     ASSERT_not_null(sourceFile);

  // DQ (9/25/2018): NOTE: we need to add the new SgGlobal IR node into the token mapping
  // (with the same entry as for the sourceFile's global scope???)
  // Copy the list of declarations to the copy of the global scope.
  // headerFileGlobal->get_declarations() = sourceFile->get_globalScope()->get_declarations();
  // headerFileGlobal->get_declarations() = include_sourceFile->get_globalScope()->get_declarations();
     headerFileGlobal->get_declarations() = sourceFile->get_globalScope()->get_declarations();

     SgGlobal* globalScope = include_sourceFile->get_globalScope();
     ASSERT_not_null(globalScope);
#else

  // #error "DEAD CODE!"

  // DQ (11/20/2019): Make sure that we have some declarations.
     ROSE_ASSERT(include_sourceFile->get_globalScope()->get_declarations().empty() == false);
#endif

     ASSERT_not_null(include_sourceFile);

  // DQ (10/2/1019): Set this here.
  // include_sourceFile->set_project(project);

  // DQ (10/2/2019): This will be checked below (test it here), but it is not reasonable for a header file when using the header file unparsing optimization.
  // ASSERT_not_null(include_sourceFile->get_project());

  // DQ (5/4/2010): This does not have to be true, just building the support to have the comments and CPP
  // directives embedded in the AST does not have to require that we are unparsing the header files (or
  // even the source file) The might, for exaple be required to support analysis only.
  // DQ (11/9/2019): We need this to be set so that transformation in the AST will be unparsed consistantly
  // between source files where this is true and header files where this has sometimes been false.
  // ROSE_ASSERT(include_sourceFile->get_unparseHeaderFiles() == false);
#if 0
     if (include_sourceFile->get_unparseHeaderFiles() == true)
        {
          printf ("NOTE: include_sourceFile->get_unparseHeaderFiles() = %s \n",include_sourceFile->get_unparseHeaderFiles() ? "true" : "false");
        }

  // DQ (12/10/2019): Changed to warning so that we can debug tool_G with test_33.cpp regression test.
  // DQ (11/20/2019): Comments and CPP directives should have been added at this point.
     if (include_sourceFile->get_processedToIncludeCppDirectivesAndComments() == false)
        {
          printf ("WARNING: In buildSourceFileForHeaderFile(): test 2: include_sourceFile->get_processedToIncludeCppDirectivesAndComments() == false \n");
        }
  // ROSE_ASSERT(include_sourceFile->get_processedToIncludeCppDirectivesAndComments() == true);
#endif

  // DQ (5/4/2010): This does not have to be true, just building the support to have the comments and CPP
  // directives embedded in the AST does not have to require that we are unparsing the header files (or
  // even the source file) The might, for exaple be required to support analysis only.
  // include_sourceFile->set_unparseHeaderFiles(true);
  // ROSE_ASSERT(include_sourceFile->get_unparseHeaderFiles() == true);

#if 0
     printf ("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ Leaving buildSourceFileForHeaderFile(): return include_sourceFile = %p \n",include_sourceFile);
#endif

#if 0
     printf ("BOTTOM of buildSourceFileForHeaderFile(): EDG_ROSE_Translation::edg_include_file_map.size() = %zu \n",EDG_ROSE_Translation::edg_include_file_map.size());
     printf (" --- include_sourceFile = %p = %s \n",include_sourceFile,include_sourceFile->getFileName().c_str());
     printf (" --- include_sourceFile->get_globalScope() = %p \n",include_sourceFile->get_globalScope());
     ROSE_ASSERT(include_sourceFile->get_globalScope() != nullptr);
     printf (" --- include_sourceFile->get_globalScope()->get_declarations().size() = %zu \n",include_sourceFile->get_globalScope()->get_declarations().size());
#endif

     return include_sourceFile;
   }


// DQ (3/14/2021): Output include saved in the SgIncludeFile about first and last computed statements in each header file.
// void outputFirstAndLastIncludeFileInfo()
void outputFirstAndLastIncludeFileInfo(SgSourceFile* sourceFile)
   {
#define DEBUG_OUTPUT_FIRST_LAST_DATA 0

     ROSE_ASSERT(sourceFile != nullptr);

#if DEBUG_OUTPUT_FIRST_LAST_DATA
     printf ("In outputFirstAndLastIncludeFileInfo(): EDG_ROSE_Translation::edg_include_file_map.size() = %zu \n",EDG_ROSE_Translation::edg_include_file_map.size());
#endif

#if DEBUG_OUTPUT_FIRST_LAST_DATA
     printf ("\nOutput collected information about first and last statements for each include file \n");
     printf ("\nOutput information on sourceFile = %s \n",sourceFile->getFileName().c_str());
     printf (" --- sourceFile->get_isDynamicLibrary() = %s \n",sourceFile->get_isDynamicLibrary() ? "true" : "false");
#endif

#if DEBUG_OUTPUT_FIRST_LAST_DATA || 0
  // DQ (5/20/2021): Adding support to the input source file to compute first and last statements for the unparser.
     if (sourceFile->get_firstStatement() != nullptr)
        {
          printf (" --- sourceFile->get_firstStatement() = %p = %s name = %s \n",sourceFile->get_firstStatement(),
               sourceFile->get_firstStatement()->class_name().c_str(),SageInterface::get_name(sourceFile->get_firstStatement()).c_str());
        }
       else
        {
          printf (" --- sourceFile->get_firstStatement() = %p \n",sourceFile->get_firstStatement());
        }

     if (sourceFile->get_lastStatement() != nullptr)
        {
          printf (" --- sourceFile->get_lastStatement()  = %p = %s name = %s \n",sourceFile->get_lastStatement(),
               sourceFile->get_lastStatement()->class_name().c_str(),SageInterface::get_name(sourceFile->get_lastStatement()).c_str());
        }
       else
        {
          printf (" --- sourceFile->get_lastStatement()  = %p \n",sourceFile->get_lastStatement());
        }
#endif

  // DQ (5/22/2021): Only output the information about headers if we have set sourceFile->get_unparseHeaderFiles() to TRUE on the command line.
     if (sourceFile->get_unparseHeaderFiles() == true)
        {
#if DEBUG_OUTPUT_FIRST_LAST_DATA
          printf ("\nOutput information on header files: \n");
#endif

     for (std::map<std::string, SgIncludeFile*>::iterator i = EDG_ROSE_Translation::edg_include_file_map.begin();
          i != EDG_ROSE_Translation::edg_include_file_map.end(); i++)
        {
          string filename            = i->first;
          SgIncludeFile* includeFile = i->second;
#if DEBUG_OUTPUT_FIRST_LAST_DATA
          printf (" --- filename = %s includeFile = %p \n",filename.c_str(),includeFile);
#endif
          ROSE_ASSERT(includeFile != nullptr);
#if DEBUG_OUTPUT_FIRST_LAST_DATA
          printf (" --- includeFile = %p filename = %s \n",includeFile,includeFile->get_filename().str());
          printf (" --- --- includeFile->get_first_source_sequence_number() = %u \n",includeFile->get_first_source_sequence_number());
          printf (" --- --- includeFile->get_last_source_sequence_number()  = %u \n",includeFile->get_last_source_sequence_number());
#endif

#if DEBUG_OUTPUT_FIRST_LAST_DATA
          printf (" --- --- includeFile->get_firstStatement() = %p \n",includeFile->get_firstStatement());
          printf (" --- --- includeFile->get_lastStatement()  = %p \n",includeFile->get_lastStatement());
#endif

       // New design puts the first and last directly into the SgIncludeFile.
          SgStatement* firstStatement = includeFile->get_firstStatement();
          SgStatement* lastStatement  = includeFile->get_lastStatement();

          if (firstStatement != nullptr)
             {
               Sg_File_Info* first_file_info = firstStatement->get_file_info();
#if DEBUG_OUTPUT_FIRST_LAST_DATA
               printf (" --- firstStatement = %p = %s \n",firstStatement,firstStatement->class_name().c_str());
               printf (" --- firstStatement = %s \n",SageInterface::get_name(firstStatement).c_str());
               printf (" --- firstStatement: line = %d column = %d filename = %s \n",first_file_info->get_line(),first_file_info->get_col(),first_file_info->get_filenameString().c_str());
               printf (" --- firstStatement: (raw) line = %d column = %d filename = %s \n",first_file_info->get_raw_line(),first_file_info->get_raw_col(),first_file_info->get_raw_filename().c_str());
               printf (" --- firstStatement: (physical) line = %d column = %d filename = %s \n",first_file_info->get_physical_line(),first_file_info->get_col(),first_file_info->get_physical_filename().c_str());
               printf (" --- firstStatement->get_file_info()->get_source_sequence_number() = %u \n",first_file_info->get_source_sequence_number());
#endif
               if (first_file_info->get_source_sequence_number() < includeFile->get_first_source_sequence_number())
                  {
                    printf ("In outputFirstAndLastIncludeFileInfo(): ###### Failing test: (first_file_info->get_source_sequence_number() >= includeFile->get_first_source_sequence_number()) == false \n");
                  }
               ROSE_ASSERT(first_file_info->get_source_sequence_number() <= includeFile->get_last_source_sequence_number());
             }
            else
             {
            // Not all include files have a valid statement (some just include other include files, or define macros).
#if DEBUG_OUTPUT_FIRST_LAST_DATA
               printf (" --- firstStatement == NULL: filename = %s \n",includeFile->get_filename().str());
#endif
             }

          if (lastStatement != nullptr)
             {
               Sg_File_Info* last_file_info  = lastStatement->get_file_info();
#if DEBUG_OUTPUT_FIRST_LAST_DATA
               printf (" --- lastStatement  = %p = %s \n",lastStatement,lastStatement->class_name().c_str());
               printf (" --- lastStatement = %s \n",SageInterface::get_name(lastStatement).c_str());
               printf (" --- lastStatement: line = %d column = %d filename = %s \n",last_file_info->get_line(),last_file_info->get_col(),last_file_info->get_filenameString().c_str());
               printf (" --- lastStatement: (raw) line = %d column = %d filename = %s \n",last_file_info->get_raw_line(),last_file_info->get_raw_col(),last_file_info->get_raw_filename().c_str());
               printf (" --- lastStatement: (physical) line = %d column = %d filename = %s \n",last_file_info->get_physical_line(),last_file_info->get_col(),last_file_info->get_physical_filename().c_str());
               printf (" --- lastStatement->get_file_info()->get_source_sequence_number() = %u \n",last_file_info->get_source_sequence_number());
#endif

               if (last_file_info->get_source_sequence_number() < includeFile->get_first_source_sequence_number())
                 {
                    printf ("In outputFirstAndLastIncludeFileInfo(): ###### Failing test: (last_file_info->get_source_sequence_number() >= includeFile->get_first_source_sequence_number()) == false \n");
                 }
               if (last_file_info->get_source_sequence_number() > includeFile->get_last_source_sequence_number())
                  {
                    printf ("In outputFirstAndLastIncludeFileInfo(): ###### Failing test: (last_file_info->get_source_sequence_number() <= includeFile->get_last_source_sequence_number()) == false \n");
                  }
             }
            else
             {
            // Not all include files have a valid statement (some just include other include files, or define macros).
#if DEBUG_OUTPUT_FIRST_LAST_DATA
               printf (" --- lastStatement == NULL: filename = %s \n",includeFile->get_filename().str());
#endif
             }

       // Not all include files have a valid statement (some just include other include files, or define macros).
          ROSE_ASSERT ( ((firstStatement != nullptr) && (lastStatement != nullptr)) || ((firstStatement == nullptr) && (lastStatement == nullptr)) );
        }
        }
   }


#if 0
void
computeFileAndScopeRelations()
   {
  // DQ (5/8/2021): Adding computation of scope and file relationships so that we can have a file dependent concept
  // of when a scope includes a transformation (we used to juat set the containsTransformation flag in the scope,
  // however, this is not enough to support the token-based unparsing when compbined with header file unparsing.

     int counter = 0;

#define DEBUG_FILE_AND_SCOPE_RELATION 0

#if DEBUG_FILE_AND_SCOPE_RELATION
     printf ("In computeFileAndScopeRelations(): EDG_ROSE_Translation::edg_include_file_map.size() = %zu \n",EDG_ROSE_Translation::edg_include_file_map.size());
#endif

#error "DEAD CODE!"

#if DEBUG_FILE_AND_SCOPE_RELATION || 0
  // printf ("In computeFileAndScopeRelations(): calling outputFirstAndLastIncludeFileInfo() \n");
  // outputFirstAndLastIncludeFileInfo();
#endif

#if DEBUG_FILE_AND_SCOPE_RELATION
     printf ("In computeFileAndScopeRelations(): iterate over the header files \n");
#endif

#error "DEAD CODE!"

  // while (i != statementBoundsMap.end())
     for (std::map<std::string, SgIncludeFile*>::iterator i = EDG_ROSE_Translation::edg_include_file_map.begin();
          i != EDG_ROSE_Translation::edg_include_file_map.end(); i++)
        {
       // SgIncludeFile* includeFile = i->first;
       // First_And_Last_Statements* firstLastStatement = i->second;
       // ROSE_ASSERT(includeFile        != NULL);
       // ROSE_ASSERT(firstLastStatement != NULL);

#error "DEAD CODE!"

          string filename            = i->first;
          SgIncludeFile* includeFile = i->second;
#if DEBUG_FILE_AND_SCOPE_RELATION
          printf (" --- filename = %s includeFile = %p \n",filename.c_str(),includeFile);
#endif
          ROSE_ASSERT(includeFile != nullptr);
#if DEBUG_FILE_AND_SCOPE_RELATION
          printf ("\ncounter = %d \n",counter);
          printf (" --- includeFile = %p filename = %s \n",includeFile,includeFile->get_filename().str());
          printf (" --- --- includeFile->get_first_source_sequence_number() = %u \n",includeFile->get_first_source_sequence_number());
          printf (" --- --- includeFile->get_last_source_sequence_number()  = %u \n",includeFile->get_last_source_sequence_number());
#endif

#if DEBUG_FILE_AND_SCOPE_RELATION
          printf (" --- --- includeFile->get_firstStatement() = %p \n",includeFile->get_firstStatement());
          printf (" --- --- includeFile->get_lastStatement()  = %p \n",includeFile->get_lastStatement());
#endif
       // New design puts the first and last directly into the SgIncludeFile.
          SgStatement* firstStatement = includeFile->get_firstStatement();
          SgStatement* lastStatement  = includeFile->get_lastStatement();

#error "DEAD CODE!"

       // Not all include files have a valid statement (some just include other include files, or define macros).
          ROSE_ASSERT ( ((firstStatement != nullptr) && (lastStatement != nullptr)) || ((firstStatement == nullptr) && (lastStatement == nullptr)) );

          if ((firstStatement != nullptr) && (lastStatement != nullptr))
             {
               SgScopeStatement* scopeOfFirstStatement = firstStatement->get_scope();
               SgScopeStatement* scopeOfLastStatement  = lastStatement->get_scope();

#if DEBUG_FILE_AND_SCOPE_RELATION
               printf ("scopeOfFirstStatement = %p = %s \n",scopeOfFirstStatement,scopeOfFirstStatement->class_name().c_str());
               printf ("scopeOfLastStatement  = %p = %s \n",scopeOfLastStatement,scopeOfLastStatement->class_name().c_str());
#endif

#error "DEAD CODE!"

#if 0
#error "DEAD CODE!"

               if (Rose::containsTransformationMap.find(includeFile) == Rose::headerFileRepresentsAllStatementsInParentScope.end())
                  {
                    scopeContainsTransformationMap = new std::map<SgScopeStatement*,bool>();
                  }
                 else
                  {
#if DEBUG_FILE_AND_SCOPE_RELATION
                    printf ("Reuse an entry for this file: includeFile = %p filename = %s \n",includeFile,includeFile->get_filename().str());
#endif
                    scopeContainsTransformationMap = Rose::containsTransformationMap[includeFile];

                    printf ("Error: I think that this case cannot happen or is not supported (header file used twice in AST) \n");
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
                  }
#endif

#error "DEAD CODE!"

               if (scopeOfFirstStatement == scopeOfLastStatement)
                  {
                 // This is the typical case.
                    if (scopeOfFirstStatement->containsOnlyDeclarations() == true)
                       {
                         SgDeclarationStatementPtrList & declarationStatementList = scopeOfFirstStatement->getDeclarationList();

                         printf ("In computeFileAndScopeRelations(): DeclarationStatementList not implemented \n");

#if DEBUG_FILE_AND_SCOPE_RELATION
                         printf ("declarationStatementList.size() = %zu \n",declarationStatementList.size());
#endif
                         printf ("Exiting as a test! \n");
                         ROSE_ASSERT(false);
                       }
                      else
                       {
                         SgStatementPtrList & statementList = scopeOfFirstStatement->getStatementList();

                         printf ("In computeFileAndScopeRelations(): StatementList not implemented \n");

                         SgStatementPtrList::iterator first = find(statementList.begin(),statementList.end(),firstStatement);
                         SgStatementPtrList::iterator last  = find(statementList.begin(),statementList.end(),lastStatement);

                         size_t countBetweenStatement = 1;
                         while (first != last)
                            {
                              countBetweenStatement++;
                              first++;
                            }

#error "DEAD CODE!"

#if DEBUG_FILE_AND_SCOPE_RELATION
                         printf ("countBetweenStatement = %zu \n",countBetweenStatement);
                         printf ("statementList.size()  = %zu \n",statementList.size());
#endif
#if DEBUG_FILE_AND_SCOPE_RELATION
                         printf ("scopeOfFirstStatement->get_containsTransformation() = %s \n",scopeOfFirstStatement->get_containsTransformation() ? "true" : "false");
                         printf ("firstStatement->get_containsTransformation() = %s \n",firstStatement->get_containsTransformation() ? "true" : "false");
                         printf ("lastStatement->get_containsTransformation()  = %s \n",lastStatement->get_containsTransformation() ? "true" : "false");
#endif
                         if (countBetweenStatement == statementList.size())
                            {
                           // The header file COMPLETELY represents the statements in the scope
#if DEBUG_FILE_AND_SCOPE_RELATION
                              printf ("The header file COMPLETELY represents the statements in the scope \n");
#endif
                              includeFile->set_headerFileRepresentsAllStatementsInParentScope(true);
                            }
                           else
                            {
#error "DEAD CODE!"
#if DEBUG_FILE_AND_SCOPE_RELATION
                              printf ("The header file does NOT completely represent the statements in the scope \n");
#endif
                              includeFile->set_headerFileRepresentsAllStatementsInParentScope(false);
                            }
                       }
                  }
                 else
                  {
#if DEBUG_FILE_AND_SCOPE_RELATION
                    printf ("Scopes of the first and last statements are different (rare case not implemented (supported) yet) \n");
#endif
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
                  }

#error "DEAD CODE!"

             }

          counter++;
        }

#error "DEAD CODE!"

#if DEBUG_FILE_AND_SCOPE_RELATION || 0
     printf ("\n\nOutput include file info: \n");
     for (std::map<std::string, SgIncludeFile*>::iterator i = EDG_ROSE_Translation::edg_include_file_map.begin();
          i != EDG_ROSE_Translation::edg_include_file_map.end(); i++)
        {
       // SgIncludeFile* includeFile = i->first;
       // First_And_Last_Statements* firstLastStatement = i->second;
       // ROSE_ASSERT(includeFile        != NULL);
       // ROSE_ASSERT(firstLastStatement != NULL);

          string filename            = i->first;
          SgIncludeFile* includeFile = i->second;

          printf (" --- filename = %s includeFile = %p \n",filename.c_str(),includeFile);

          bool headerFileRepresentsAllStatementsInParentScope = includeFile->get_headerFileRepresentsAllStatementsInParentScope();

          printf (" --- headerFileRepresentsAllStatementsInParentScope = %s \n",headerFileRepresentsAllStatementsInParentScope ? "true" : "false");
        }
#endif

#error "DEAD CODE!"

#if 0
#if DEBUG_FILE_AND_SCOPE_RELATION || 0
     // Output the data within the Rose::containsTransformationMap.
     std::map<SgIncludeFile*,std::map<SgScopeStatement*,bool>*>::iterator i = Rose::containsTransformationMap.begin();

     printf ("\n\nOutput Rose::containsTransformationMap info: \n");
     while (i != Rose::containsTransformationMap.end())
       {
         SgIncludeFile* includeFile = i->first;
         std::map<SgScopeStatement*,bool>* scopeContainsTransformationMap = i->second;

         printf (" --- includeFile = %p filename = %s \n",includeFile,includeFile->get_filename().str());

         std::map<SgScopeStatement*,bool>::iterator j = scopeContainsTransformationMap->begin();
         while (j != scopeContainsTransformationMap->end())
           {
             SgScopeStatement* scope = j->first;
             bool containsTransformation = j->second;

             printf (" --- --- scope = %p = %s filename = %s \n",scope,scope->class_name().c_str(),scope->get_file_info()->get_filenameString().c_str());
             printf (" --- --- --- containsTransformation              = %s \n",containsTransformation ? "true" : "false");
             printf (" --- --- --- scope->get_containsTransformation() = %s \n",scope->get_containsTransformation() ? "true" : "false");

             j++;
           }


         i++;
       }
#endif
#endif

#error "DEAD CODE!"
   }
#endif


void buildFirstAndLastStatementsForIncludeFiles ( SgProject* project )
// void buildFirstAndLastStatementsForIncludeFiles ( SgSourceFile* sourceFile )
   {
  // This function build the mapping of the first and last statements associated with each SgIncludeFile.
  // It is required to support the token-based unparsing so that we can know when the last statement in
  // the SgIncludeFile has been reached, so that we can output the trailing whitespace tokens properly.
  // The function uses the source sequence limits for the file, and computes which statements in the AST
  // are associated with these limits.  It uses a traversal to evaluate the statements (and the associated
  // source sequence number for each statement) against the limits for each file.

  // DQ (3/10/2021): Add performance analysis support.
     TimingPerformance timer ("AST buildFirstAndLastStatementsForIncludeFiles:");

#define DEBUG_FIRST_LAST_STMTS 0

#if DEBUG_FIRST_LAST_STMTS || 0
     printf ("###################################################### \n");
     printf ("###################################################### \n");
     printf ("####  buildFirstAndLastStatementsForIncludeFiles  #### \n");
     printf ("###################################################### \n");
     printf ("###################################################### \n");
#endif

#if DEBUG_FIRST_LAST_STMTS
     printf ("In buildFirstAndLastStatementsForIncludeFiles(): project = %p \n",project);
  // printf ("In buildFirstAndLastStatementsForIncludeFiles(): sourceFile = %p \n",sourceFile);
#endif

     class IncludeFileStatementTraversal : public AstSimpleProcessing
        {
          public:
            // DQ (3/13/2021): This needs to be the header file and not the original input file.
            // IncludeFileStatementTraversal(SgSourceFile* sourceFile);

            // We need to recorde the first and last statement that are in the same scope.
            // We want to record the first and last in the same scope.  However, an arbitrary
            // include file could has a last statement in a different scope.  Now clear how
            // to handle this pathological case.
               SgScopeStatement* target_scope = nullptr;

               SgSourceFile* sourceFile = nullptr;

               void visit (SgNode* node)
                  {
#if DEBUG_FIRST_LAST_STMTS
                 // printf ("In IncludeFileStatementTraversal::visit(): node = %p = %s \n",node,node->class_name().c_str());
                    printf ("In buildFirstAndLastStatementsForIncludeFiles(): IncludeFileStatementTraversal::visit(): node = %p = %s name = %s \n",
                         node,node->class_name().c_str(),SageInterface::get_name(node).c_str());
                    printf (" --- filename = %s \n",node->get_file_info()->get_filenameString().c_str());
#endif
                    SgSourceFile* tmp_sourceFile = isSgSourceFile(node);
                    if (tmp_sourceFile != nullptr)
                       {
                         sourceFile = tmp_sourceFile;

#if DEBUG_FIRST_LAST_STMTS
                         printf ("Found the input source file: sourceFile->getFileName() = %s \n",sourceFile->getFileName().c_str());
#endif
                       }
                    ROSE_ASSERT(sourceFile != nullptr);

                 // DQ (4/25/2021): I forget why this is a SgDeclarationStatement instead of a SgStatement.
                 // SgStatement*             statement             = isSgStatement(node);
                 // SgStatement*             statement             = isSgDeclarationStatement(node);
#if 1
                 // DQ (4/28/2021): I think this is the better solution, since we make sure that the last statement is in the same scope (as I recall) below.
                    SgStatement*             statement             = isSgStatement(node);
#else
                    SgStatement*             statement             = isSgDeclarationStatement(node);
                 // DQ (4/25/2021): Handle case of a simple expression statement.
                    if (statement == nullptr)
                       {
#if DEBUG_FIRST_LAST_STMTS
                         printf ("Handle the case of an SgExprStatement \n");
#endif
                         statement = isSgExprStatement(node);
                       }
#endif
                    SgGlobal*                globalScope           = isSgGlobal(statement);
                    SgFunctionParameterList* functionParameterList = isSgFunctionParameterList(node);
                    SgCtorInitializerList*   ctorInitializerList   = isSgCtorInitializerList(node);

                    SgTemplateInstantiationDecl*               templateInstantiationDecl               = isSgTemplateInstantiationDecl(node);
                    SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDecl = isSgTemplateInstantiationMemberFunctionDecl(node);

                 // IR nodes for which we don't want to identify as the first or last statement of a file (header file).
                    bool processStatement = globalScope == nullptr && functionParameterList == nullptr && ctorInitializerList == nullptr &&
                                            templateInstantiationDecl == nullptr && templateInstantiationMemberFunctionDecl == nullptr;

#if DEBUG_FIRST_LAST_STMTS
                    printf ("statement        = %p \n",statement);
                    printf ("processStatement = %s \n",processStatement ? "true" : "false");
#endif
                    if (statement != nullptr && processStatement == true)
                       {
                         Sg_File_Info* file_info = statement->get_file_info();
                         ROSE_ASSERT(file_info != nullptr);
#if DEBUG_FIRST_LAST_STMTS
                         printf ("\nIn IncludeFileStatementTraversal::visit(): statement = %p = %s \n",node,node->class_name().c_str());
                         printf (" --- statement = %s \n",SageInterface::get_name(statement).c_str());
                         printf (" --- statement: line = %d column = %d filename = %s \n",file_info->get_line(),file_info->get_col(),file_info->get_filenameString().c_str());
                         printf (" --- statement: (physical) line = %d column = %d filename = %s \n",file_info->get_physical_line(),file_info->get_col(),file_info->get_physical_filename().c_str());
#endif
#if 0
                      // DQ (3/10/2021): Debugging this specific case. Some of these have no valid source position (are listed as compiler generated).
                         SgTemplateTypedefDeclaration*              templateTypedefDeclaration              = isSgTemplateTypedefDeclaration(node);
                         if (templateTypedefDeclaration != nullptr)
                            {
                              printf ("\nIn IncludeFileStatementTraversal::visit(): statement = %p = %s \n",node,node->class_name().c_str());
                              printf (" --- statement = %s \n",SageInterface::get_name(statement).c_str());
                              printf (" --- statement: line = %d column = %d filename = %s \n",file_info->get_line(),file_info->get_col(),file_info->get_filenameString().c_str());
                              printf (" --- statement: (physical) line = %d column = %d filename = %s \n",file_info->get_physical_line(),file_info->get_col(),file_info->get_physical_filename().c_str());
                            }
#endif
                      // DQ (3/23/2021): The physical_filename of a transformation needs to be derived directly
                      // from the physical file id, instead of computed by the get_physical_filename() function.
                      // This could be using the file_id instead of the filename as a string.
                         string filename = file_info->get_physical_filename();
                         int    physical_file_id  = file_info->get_physical_file_id();
#if DEBUG_FIRST_LAST_STMTS
                         printf ("before reset filename: physical_file_id = %d filename = %s \n",physical_file_id,filename.c_str());
#endif
                         filename = Sg_File_Info::getFilenameFromID(physical_file_id);
#if DEBUG_FIRST_LAST_STMTS
                         printf ("after reset filename: physical_file_id  = %d filename = %s \n",physical_file_id,filename.c_str());
#endif

                         if (EDG_ROSE_Translation::edg_include_file_map.find(filename) != EDG_ROSE_Translation::edg_include_file_map.end())
                            {
                              SgIncludeFile* includeFile = EDG_ROSE_Translation::edg_include_file_map[filename];
                              ROSE_ASSERT(includeFile != nullptr);

                              SgSourceFile* header_file_asssociated_source_file = includeFile->get_source_file();
#if DEBUG_FIRST_LAST_STMTS
                              printf ("Found an SgIncludeFile: includeFile = %p header_file_asssociated_source_file = %p \n",includeFile,header_file_asssociated_source_file);
#endif
                           // DQ (3/14/2021): This is null for rose_edg_required_macros_and_functions.h (pre-included for all ROSE processed code).
                           // ROSE_ASSERT(header_file_asssociated_source_file != NULL);
                              if (header_file_asssociated_source_file != nullptr)
                                 {
#if DEBUG_FIRST_LAST_STMTS
                                   printf ("header_file_asssociated_source_file = %s \n",header_file_asssociated_source_file->getFileName().c_str());
                                   printf ("Rose::tokenSubsequenceMapOfMapsBySourceFile.find(header_file_asssociated_source_file) != Rose::tokenSubsequenceMapOfMapsBySourceFile.end() = %s \n",
                                        Rose::tokenSubsequenceMapOfMapsBySourceFile.find(header_file_asssociated_source_file) != Rose::tokenSubsequenceMapOfMapsBySourceFile.end() ? "true" : "false");
#endif
                                   if (Rose::tokenSubsequenceMapOfMapsBySourceFile.find(header_file_asssociated_source_file) != Rose::tokenSubsequenceMapOfMapsBySourceFile.end())
                                      {
                                     // DQ (3/13/2021): Adding support to filter out collecting references to statements that don't have a corresponding token subsequence.
                                        std::map<SgNode*,TokenStreamSequenceToNodeMapping*> & tokenStreamSequenceMap = header_file_asssociated_source_file->get_tokenSubsequenceMap();
#if DEBUG_FIRST_LAST_STMTS
                                        printf (" --- tokenStreamSequenceMap.size() = %zu \n",tokenStreamSequenceMap.size());
#endif
                                     // ROSE_ASSERT(statementBoundsMap.find(includeFile) != statementBoundsMap.end());
                                        if (includeFile->get_firstStatement() == nullptr)
                                           {
#if DEBUG_FIRST_LAST_STMTS
                                             printf ("Previously NULL: first time seeing a statement for includeFile->get_filename() = %s \n",includeFile->get_filename().str());
#endif
                                          // DQ (3/13/2021): We need to make sure that the first and last statements that we select correspond
                                          // to a collected token subsequence. In codeSegregation test_141_1.h, demonstrates such a case.
                                          // includeFile->set_firstStatement(statement);
                                          // target_scope = isSgScopeStatement(statement->get_parent());

                                          // TokenStreamSequenceToNodeMapping* tokenSubsequence = tokenStreamSequenceMap[stmt];
                                             if (tokenStreamSequenceMap.find(statement) != tokenStreamSequenceMap.end())
                                                {
                                                  ROSE_ASSERT(statement != nullptr);

                                                  includeFile->set_firstStatement(statement);

                                                  ROSE_ASSERT(statement->get_parent() != nullptr);

                                                  target_scope = isSgScopeStatement(statement->get_parent());

#if DEBUG_FIRST_LAST_STMTS
                                                  printf (" --- target_scope = %p = %s \n",target_scope,target_scope->class_name().c_str());
#endif
                                                }
                                               else
                                                {
#if DEBUG_FIRST_LAST_STMTS
                                                  printf ("We can't record this as a first statement becuae it does not correspond to a token subsequence \n");
#endif
                                                }
                                           }

#if DEBUG_FIRST_LAST_STMTS
                                        printf (" --- (before testing statement parent) statement = %p = %s \n",statement,statement->class_name().c_str());
#endif
                                        ROSE_ASSERT(statement->get_parent() != nullptr);
#if DEBUG_FIRST_LAST_STMTS
                                        if (target_scope != nullptr)
                                           {
                                             printf (" --- (before testing statement parent) target_scope            = %p = %s \n",target_scope,target_scope->class_name().c_str());
                                           }
                                        printf (" --- (before testing statement parent) statement->get_parent() = %p = %s \n",statement->get_parent(),statement->get_parent()->class_name().c_str());
#endif
                                     // includeFile->set_lastStatement(statement);
                                     // if (statement->get_parent() == target_scope)
                                        if (target_scope != nullptr && statement->get_parent() == target_scope)
                                           {
                                          // DQ (3/13/2021): We need to make sure that the first and last statements that we select correspond
                                          // to a collected token subsequence. In codeSegregation test_141_1.h, demonstrates such a case.
                                          // includeFile->set_lastStatement(statement);
                                             if (tokenStreamSequenceMap.find(statement) != tokenStreamSequenceMap.end())
                                                {
#if DEBUG_FIRST_LAST_STMTS
                                                  printf ("This can be a last statement (it has an associated token subsequence) \n");
#endif
                                                  includeFile->set_lastStatement(statement);
                                                }
                                               else
                                                {
#if DEBUG_FIRST_LAST_STMTS
                                               // printf ("We can't record this as a last statement because it does not correspond to a token subsequence \n");
                                                  printf ("This can be a last statement even if it does not have an associated token subsequence (e.g. it may be a transforamtion) \n");
#endif
                                               // DQ (23/2021): I think we should because it might be that a transformation is a last statement of a
                                               // header file and in which case it is still the last statement independent of if it is unparsed via
                                               // the token stream or from the AST.
                                                  includeFile->set_lastStatement(statement);
                                                }

                                             SgStatement* computedLastStatement = SageInterface::getLastStatement(target_scope);

                                          // It might be better to make sure that the last "}" is in the current file.
                                          // if (statement == computedLastStatement && includeFile->get_firstStatement() == target_scope)
                                             if (statement == computedLastStatement && target_scope->get_endOfConstruct() )
                                                {
                                               // Then make the scope the last statement.
#if DEBUG_FIRST_LAST_STMTS
                                                  printf ("Since this is the last statement of the scope, then make the scope the last statement \n");
#endif
                                                  includeFile->set_lastStatement(target_scope);
                                                }
                                           }
                                          else
                                           {
#if DEBUG_FIRST_LAST_STMTS
                                          // printf ("Rose::tokenSubsequenceMapOfMapsBySourceFile.find(header_file_asssociated_source_file) == Rose::tokenSubsequenceMapOfMapsBySourceFile.end() \n");
                                             printf ("This is a different scope: statement->get_parent() == target_scope (first and last statements must be in the same scope) \n");
#endif
                                           }
                                      }
                                     else
                                      {
#if DEBUG_FIRST_LAST_STMTS
                                        printf ("This header file has not been processed to record its first and last statements \n");
#endif
                                      }
                                 }
                            }
                           else
                            {
                           // Not all statements will be in the header files, however this could be the input source file,
                           // so we need to support computing the first and last statements in that file as well.
#if DEBUG_FIRST_LAST_STMTS
                              printf ("filename not found in edg_include_file_map: filename = %s \n",filename.c_str());
#endif
                              ROSE_ASSERT(sourceFile != nullptr);

                           // DQ (5/22/2021): Added to avoid processing compiler generted statements.
                           // if (physical_file_id >= 0)
                           //    {

                           // DQ (3/13/2021): Adding support to filter out collecting references to statements that don't have a corresponding token subsequence.
                              std::map<SgNode*,TokenStreamSequenceToNodeMapping*> & tokenStreamSequenceMap = sourceFile->get_tokenSubsequenceMap();
#if DEBUG_FIRST_LAST_STMTS
                              printf (" --- tokenStreamSequenceMap.size() = %zu \n",tokenStreamSequenceMap.size());
#endif
                           // ROSE_ASSERT(statementBoundsMap.find(includeFile) != statementBoundsMap.end());

                           // DQ (5/20/2021): The firstStatment and lastStatement in the source file are only used for the
                           // input source file (the include files used their own data member).
                              if (sourceFile->get_firstStatement() == nullptr)
                                 {
#if DEBUG_FIRST_LAST_STMTS
                                   printf ("Previously NULL: first time seeing a statement for sourceFile->get_filename() = %s \n",sourceFile->getFileName().c_str());
#endif
                                // DQ (3/13/2021): We need to make sure that the first and last statements that we select correspond
                                // to a collected token subsequence. In codeSegregation test_141_1.h, demonstrates such a case.
                                // includeFile->set_firstStatement(statement);
                                // target_scope = isSgScopeStatement(statement->get_parent());

                                // TokenStreamSequenceToNodeMapping* tokenSubsequence = tokenStreamSequenceMap[stmt];
                                   if (tokenStreamSequenceMap.find(statement) != tokenStreamSequenceMap.end())
                                      {
                                        sourceFile->set_firstStatement(statement);
                                        target_scope = isSgScopeStatement(statement->get_parent());

#if DEBUG_FIRST_LAST_STMTS
                                        printf (" --- target_scope = %p = %s \n",target_scope,target_scope->class_name().c_str());
#endif
                                      }
                                     else
                                      {
#if DEBUG_FIRST_LAST_STMTS
                                        printf ("We can't record this as a first statement becuase it does not correspond to a token subsequence \n");
#endif
                                      }
                                 }


                              ROSE_ASSERT(statement->get_parent() != nullptr);
#if DEBUG_FIRST_LAST_STMTS
                              printf (" --- (before testing statement parent) target_scope            = %p = %s \n",target_scope,target_scope->class_name().c_str());
                              printf (" --- (before testing statement parent) statement->get_parent() = %p = %s \n",statement->get_parent(),statement->get_parent()->class_name().c_str());
#endif
                           // includeFile->set_lastStatement(statement);
                              if (target_scope != nullptr && statement->get_parent() == target_scope)
                                 {
                                // DQ (5/20/2021): We can't have the last statement be the global scope in the input source file.
                                // DQ (3/13/2021): We need to make sure that the first and last statements that we select correspond
                                // to a collected token subsequence. In codeSegregation test_141_1.h, demonstrates such a case.
                                // includeFile->set_lastStatement(statement);
                                // if (tokenStreamSequenceMap.find(statement) != tokenStreamSequenceMap.end())
                                   if (isSgGlobal(statement) == nullptr)
                                      {
                                        if (tokenStreamSequenceMap.find(statement) != tokenStreamSequenceMap.end())
                                           {
#if DEBUG_FIRST_LAST_STMTS
                                             printf ("This can be a last statement (it has an associated token subsequence) \n");
#endif
                                             sourceFile->set_lastStatement(statement);
                                           }
                                          else
                                           {
#if DEBUG_FIRST_LAST_STMTS
                                          // printf ("We can't record this as a last statement because it does not correspond to a token subsequence \n");
                                             printf ("This can be a last statement even if it does not have an associated token subsequence (e.g. it may be a transforamtion) \n");
#endif
                                          // DQ (23/2021): I think we should because it might be that a transformation is a last statement of a
                                          // header file and in which case it is still the last statement independent of if it is unparsed via
                                          // the token stream or from the AST.
                                          // sourceFile->set_lastStatement(statement);
                                             if (tokenStreamSequenceMap.find(statement) != tokenStreamSequenceMap.end())
                                                {
                                                  sourceFile->set_lastStatement(statement);
                                                }
                                               else
                                                {
#if DEBUG_FIRST_LAST_STMTS
                                                  printf ("We can't record this as a last statement becuase it does not correspond to a token subsequence \n");
#endif
                                                }
                                           }
                                      }

                                   SgStatement* computedLastStatement = SageInterface::getLastStatement(target_scope);
#if DEBUG_FIRST_LAST_STMTS
                                   printf ("computedLastStatement = %p = %s \n",computedLastStatement,computedLastStatement->class_name().c_str());
#endif
                                // It might be better to make sure that the last "}" is in the current file.
                                // if (statement == computedLastStatement && includeFile->get_firstStatement() == target_scope)
                                // if (statement == computedLastStatement && target_scope->get_endOfConstruct() )
                                // if (isSgGlobal(statement) == NULL && statement == computedLastStatement && target_scope->get_endOfConstruct() )
                                // if (statement == computedLastStatement && target_scope->get_endOfConstruct() )
                                   if (isSgGlobal(target_scope) == nullptr && statement == computedLastStatement && target_scope->get_endOfConstruct() )
                                      {
                                     // Then make the scope the last statement.
#if DEBUG_FIRST_LAST_STMTS
                                        printf ("Since this is the last statement of the scope, then make the scope the last statement (except for SgGlobal) \n");
#endif
                                        sourceFile->set_lastStatement(target_scope);
                                      }
                                 }
                                else
                                 {
#if DEBUG_FIRST_LAST_STMTS
                                // printf ("Rose::tokenSubsequenceMapOfMapsBySourceFile.find(header_file_asssociated_source_file) == Rose::tokenSubsequenceMapOfMapsBySourceFile.end() \n");
                                   printf ("This is a different scope: statement->get_parent() == target_scope (first and last statements must be in the same scope) \n");
#endif
                                 }
                            }
                       }
                  }
        };

     IncludeFileStatementTraversal traversal;

#if DEBUG_FIRST_LAST_STMTS
     printf ("Before call to traversal \n");
#endif

  // DQ (3/11/2021): Need to call this on a specific source file (the one on the original command line,
  // not the dynamic library file, so that we will avoid marking SgIncludeFiles twice (since two files
  // will include the same header files).  An altrnative implementation could define a header file list
  // for each source file, but this would be more complex and error prone, and hard to debug)).
  // traversal.traverse(project,preorder);
  // SgFilePtrList & fileList = project->get_files();
#if 1
     SgFilePtrList & fileList = project->get_fileList();
     for (size_t i = 0; i < fileList.size(); i++)
        {
          SgSourceFile* sourceFile = isSgSourceFile(fileList[i]);
          ROSE_ASSERT(sourceFile != nullptr);

#if DEBUG_FIRST_LAST_STMTS
          printf ("\nLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL \n");
          printf ("Testing for isDynamicLibrary before calling traversal for filename = %s \n",sourceFile->getFileName().c_str());
#endif
#if DEBUG_FIRST_LAST_STMTS
          printf ("sourceFile->get_isDynamicLibrary() = %s \n",sourceFile->get_isDynamicLibrary() ? "true" : "false");
#endif
       // DQ (5/24/2021): Since we have to unparse all the files, we need to compute first and last on all of the files.
       // What is less clear is what to do with the information about shared header files. I think that at worst it is
       // redundant information.
       // if (sourceFile->get_isDynamicLibrary() == false)
             {
#if DEBUG_FIRST_LAST_STMTS
               printf ("Calling traversal for filename = %s \n",sourceFile->getFileName().c_str());
#endif
               traversal.traverse(sourceFile,preorder);
             }

#if DEBUG_FIRST_LAST_STMTS && 0
       // DQ (5/20/2021): Adding support to the input source file to compute first and last statements for the unparser.
          printf ("sourceFile->get_firstStatement() = %p \n",sourceFile->get_firstStatement());
          if (sourceFile->get_firstStatement() != nullptr)
             {
               printf ("sourceFile->get_firstStatement() = %p = %s name = %s \n",sourceFile->get_firstStatement(),
                    sourceFile->get_firstStatement()->class_name().c_str(),SageInterface::get_name(sourceFile->get_firstStatement()).c_str());
             }
          printf ("sourceFile->get_lastStatement()  = %p \n",sourceFile->get_lastStatement());
          if (sourceFile->get_lastStatement() != nullptr)
             {
               printf ("sourceFile->get_lastStatement()  = %p = %s name = %s \n",sourceFile->get_lastStatement(),
                    sourceFile->get_lastStatement()->class_name().c_str(),SageInterface::get_name(sourceFile->get_lastStatement()).c_str());
             }
#endif

#if DEBUG_FIRST_LAST_STMTS
          printf ("LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL \n\n");
#endif
        }
#else
#error "DEAD CODE!"
#endif

#if DEBUG_FIRST_LAST_STMTS
     printf ("After call to traversal \n");
#endif

#if DEBUG_FIRST_LAST_STMTS || 0
     for (size_t i = 0; i < fileList.size(); i++)
        {
          SgSourceFile* sourceFile = isSgSourceFile(fileList[i]);
          ROSE_ASSERT(sourceFile != nullptr);

       // DQ (5/24/2021): Since we have to unparse all the files, we need to compute first and last on all of the files.
       // What is less clear is what to do with the information about shared header files. I think that at worst it is
       // redundant information.
       // if (sourceFile->get_isDynamicLibrary() == false)
             {
            // Only output once... some tools (e.g. codeSegregation tool) will build an additional file, in which case we don't want to output info for that file.
               outputFirstAndLastIncludeFileInfo(sourceFile);
             }
        }
#endif

#if DEBUG_FIRST_LAST_STMTS
     printf ("Leaving buildFirstAndLastStatementsForIncludeFiles(): project = %p \n",project);
#endif
   }


void buildFirstAndLastStatementsForScopes ( SgProject* project )
   {
  // DQ (5/27/2021): We need a more comprehensive handling of identifing first and last statements specific
  // to each scope and for each file.  It is similar to the function above that computed the first and last
  // statement for each file (source files and header files that have been transformed).


  // This function build the mapping of the first and last statements associated with each SgScopeStatement.
  // It is required to support the token-based unparsing so that we can know when the last statement in
  // the SgIncludeFile has been reached, so that we can output the trailing whitespace tokens properly.
  // The function uses the source sequence limits for the file, and computes which statements in the AST
  // are associated with these limits.  It uses a traversal to evaluate the statements (and the associated
  // source sequence number for each statement) against the limits for each file.

  // DQ (3/10/2021): Add performance analysis support.
     TimingPerformance timer ("AST buildFirstAndLastStatementsForScopes:");

#define DEBUG_FIRST_LAST_STMTS_SCOPES 0

#if DEBUG_FIRST_LAST_STMTS_SCOPES || 0
     printf ("################################################ \n");
     printf ("################################################ \n");
     printf ("####  buildFirstAndLastStatementsForScopes  #### \n");
     printf ("################################################ \n");
     printf ("################################################ \n");
#endif

#if DEBUG_FIRST_LAST_STMTS_SCOPES
     printf ("In buildFirstAndLastStatementsForScopes(): project = %p \n",project);
#endif

     class StatementTraversal : public AstSimpleProcessing
        {
          public:
            // We need to recorde the first and last statement that are in the same scope.

               SgSourceFile* sourceFile = nullptr;

            // DQ (5/27/2021): Map to support storing the first and last statement for each scope.
            // std::map<SgScopeStatement*,std::pair<SgStatement*,SgStatement*> >* firstAndLastStatementsToUnparseInScopeMap;

            // DQ (6/1/2021): Added to handle.
               int physical_file_id_from_source_file = -1;

               void visit (SgNode* node)
                  {
#if DEBUG_FIRST_LAST_STMTS_SCOPES
                 // printf ("In StatementTraversal::visit(): node = %p = %s \n",node,node->class_name().c_str());
                    printf ("\n");
                    printf ("In buildFirstAndLastStatementsForScopes(): StatementTraversal::visit(): node = %p = %s name = %s \n",
                         node,node->class_name().c_str(),SageInterface::get_name(node).c_str());
                    printf (" --- filename = %s \n",node->get_file_info()->get_filenameString().c_str());
                 // ROSE_ASSERT(sourceFile != NULL);
                    if (sourceFile != nullptr)
                       {
                         printf ("sourceFile = %p filename = %s \n",sourceFile,sourceFile->getFileName().c_str());
                       }
#endif
                 // If we need this it will be set to a valid pointer value later, just make sure it is NULL as this early stage.
                 // firstAndLastStatementsToUnparseInScopeMap = NULL;
                 // ROSE_ASSERT(firstAndLastStatementsToUnparseInScopeMap == NULL);

                    SgSourceFile* tmp_sourceFile = isSgSourceFile(node);
                    if (tmp_sourceFile != nullptr)
                       {
                         sourceFile = tmp_sourceFile;

#if DEBUG_FIRST_LAST_STMTS_SCOPES
                         printf ("Found the input source file: sourceFile->getFileName() = %s \n",sourceFile->getFileName().c_str());
#endif

                         SgGlobal* globalScope = sourceFile->get_globalScope();
                         physical_file_id_from_source_file = globalScope->get_file_info()->get_physical_file_id();

#if DEBUG_FIRST_LAST_STMTS_SCOPES
                         printf ("setting physical_file_id_from_source_file = %d \n",physical_file_id_from_source_file);
#endif
                       }
                    ROSE_ASSERT(sourceFile != nullptr);

                 // DQ (4/25/2021): I forget why this is a SgDeclarationStatement instead of a SgStatement.
                 // SgStatement*             statement             = isSgStatement(node);
                 // SgStatement*             statement             = isSgDeclarationStatement(node);
#if 1
                 // DQ (4/28/2021): I think this is the better solution, since we make sure that the last statement is in the same scope (as I recall) below.
                    SgStatement*             statement             = isSgStatement(node);
#else
                    SgStatement*             statement             = isSgDeclarationStatement(node);
                 // DQ (4/25/2021): Handle case of a simple expression statement.
                    if (statement == nullptr)
                       {
#if DEBUG_FIRST_LAST_STMTS_SCOPES
                         printf ("Handle the case of an SgExprStatement \n");
#endif
                         statement = isSgExprStatement(node);
                       }
#endif
                    SgGlobal*                globalScope           = isSgGlobal(statement);
                    SgFunctionParameterList* functionParameterList = isSgFunctionParameterList(node);
                    SgCtorInitializerList*   ctorInitializerList   = isSgCtorInitializerList(node);

                    SgTemplateInstantiationDecl*               templateInstantiationDecl               = isSgTemplateInstantiationDecl(node);
                    SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDecl = isSgTemplateInstantiationMemberFunctionDecl(node);

                 // IR nodes for which we don't want to identify as the first or last statement of a file (header file).
                    bool processStatement = globalScope == nullptr && functionParameterList == nullptr && ctorInitializerList == nullptr &&
                                            templateInstantiationDecl == nullptr && templateInstantiationMemberFunctionDecl == nullptr;

#if DEBUG_FIRST_LAST_STMTS_SCOPES
                    printf ("statement        = %p \n",statement);
                    printf ("processStatement = %s \n",processStatement ? "true" : "false");
#endif

                 // DQ (5/27/2021): Add an entry for the global scope (even through we don't process the global scope directly
                 // (see definition of processStatement), we store information specific to the parent of the statements that
                 // we process and so we store information in the entry for the global scope).
                    if (globalScope != nullptr)
                       {
                      // firstAndLastStatementsToUnparseInScopeMap[globalScope] = std::pair<SgStatement*,SgStatement*>(NULL,NULL);
                      // firstAndLastStatementsToUnparseInScopeMap->[sourceFile][globalScope] = std::pair<SgStatement*,SgStatement*>(NULL,NULL);
                         if (Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile.find(sourceFile) == Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile.end())
                            {
#if DEBUG_FIRST_LAST_STMTS_SCOPES
                              printf ("A map for this file already exists \n");
#endif
                              Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[sourceFile] = std::map<SgScopeStatement*,std::pair<SgStatement*,SgStatement*> >();
                            }
                           else
                            {
#if DEBUG_FIRST_LAST_STMTS_SCOPES
                              printf ("A map for this file already exists \n");
#endif
                            }

                      // firstAndLastStatementsToUnparseInScopeMap = &(Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[sourceFile]);
                      // ROSE_ASSERT(firstAndLastStatementsToUnparseInScopeMap != NULL);

                       }

                    if (statement != nullptr && processStatement == true)
                       {
                         Sg_File_Info* file_info = statement->get_file_info();
                         ROSE_ASSERT(file_info != nullptr);

#if DEBUG_FIRST_LAST_STMTS_SCOPES
                         printf ("\nIn StatementTraversal::visit(): statement = %p = %s \n",node,node->class_name().c_str());
                         printf (" --- statement = %s \n",SageInterface::get_name(statement).c_str());
                         printf (" --- statement: line = %d column = %d filename = %s \n",file_info->get_line(),file_info->get_col(),file_info->get_filenameString().c_str());
                         printf (" --- statement: (physical) line = %d column = %d filename = %s \n",file_info->get_physical_line(),file_info->get_col(),file_info->get_physical_filename().c_str());
#endif
                         SgScopeStatement* parentScope = isSgScopeStatement(statement->get_parent());

#if DEBUG_FIRST_LAST_STMTS_SCOPES
                         printf ("parentScope = %p \n",parentScope);
                         if (parentScope != nullptr)
                            {
                              printf ("parentScope = %p = %s \n",parentScope,parentScope->class_name().c_str());
                            }
#endif
                      // DQ (5/27/2021): Filter out some scopes that don't have lists of children.
                         bool skipScope = (isSgFunctionDefinition(statement) != nullptr);

                      // DQ (5/27/2021): Filter out some scopes that don't have lists of children.
                         bool skipParentScope = (isSgFunctionDefinition(parentScope) != nullptr);
                         if (skipParentScope == true)
                            {
#if DEBUG_FIRST_LAST_STMTS_SCOPES
                              printf ("setting parentScope = nullptr \n");
#endif
                              parentScope = nullptr;
                            }

                      // DQ (3/23/2021): The physical_filename of a transformation needs to be derived directly
                      // from the physical file id, instead of computed by the get_physical_filename() function.
                      // This could be using the file_id instead of the filename as a string.
                         string filename = file_info->get_physical_filename();
                         int    physical_file_id  = file_info->get_physical_file_id();

#if DEBUG_FIRST_LAST_STMTS_SCOPES
                         printf ("before reset filename: physical_file_id = %d filename = %s \n",physical_file_id,filename.c_str());
#endif
                         filename = Sg_File_Info::getFilenameFromID(physical_file_id);

#if DEBUG_FIRST_LAST_STMTS_SCOPES || 0
                         printf ("after reset filename: physical_file_id  = %d filename = %s \n",physical_file_id,filename.c_str());
#endif
                         if (EDG_ROSE_Translation::edg_include_file_map.find(filename) != EDG_ROSE_Translation::edg_include_file_map.end())
                            {
                              SgIncludeFile* includeFile = EDG_ROSE_Translation::edg_include_file_map[filename];
                              ROSE_ASSERT(includeFile != nullptr);

                              SgSourceFile* header_file_asssociated_source_file = includeFile->get_source_file();

#if DEBUG_FIRST_LAST_STMTS_SCOPES || 0
                              printf ("Found an SgIncludeFile: includeFile = %p header_file_asssociated_source_file = %p \n",includeFile,header_file_asssociated_source_file);
#endif
                           // DQ (3/14/2021): This is null for rose_edg_required_macros_and_functions.h (pre-included for all ROSE processed code).
                           // ROSE_ASSERT(header_file_asssociated_source_file != NULL);
                              if (header_file_asssociated_source_file != nullptr)
                                 {
#if DEBUG_FIRST_LAST_STMTS_SCOPES || 0
                                   printf ("header_file_asssociated_source_file = %s \n",header_file_asssociated_source_file->getFileName().c_str());
                                   printf ("Rose::tokenSubsequenceMapOfMapsBySourceFile.find(header_file_asssociated_source_file) != Rose::tokenSubsequenceMapOfMapsBySourceFile.end() = %s \n",
                                        Rose::tokenSubsequenceMapOfMapsBySourceFile.find(header_file_asssociated_source_file) != Rose::tokenSubsequenceMapOfMapsBySourceFile.end() ? "true" : "false");
#endif
                                // DQ (6/28/2021): Header files that have not been transformed will not have had their token streams collected.
                                // Since this is an important performance optimization to only collect information about header files that will
                                // be transformed (and then unparsed), we can't expect that Rose::tokenSubsequenceMapOfMapsBySourceFile will have
                                // and entry for each header file that we will want to compute the first and last statement for.  Because this is
                                // handled in a single pass, per source file in the projects file list, we can compute the first and last statement
                                // for each header file, so that the information will be available if the header file is unparsed.  Note that this
                                // issue only appears to effect the output of a closing #endif and or comments at the end of an unparsed header file.
                                // if (Rose::tokenSubsequenceMapOfMapsBySourceFile.find(header_file_asssociated_source_file) != Rose::tokenSubsequenceMapOfMapsBySourceFile.end())
                                   if (true)
                                      {
                                     // DQ (5/27/2021): Add an entry for the global scope (even through we don't process the global scope directly
                                     // (see definition of processStatement), we store information specific to the parent of the statements that
                                     // we process and so we store information in the entry for the global scope).
                                     // if (globalScope != NULL)
                                           {
                                          // firstAndLastStatementsToUnparseInScopeMap[globalScope] = std::pair<SgStatement*,SgStatement*>(NULL,NULL);
                                          // firstAndLastStatementsToUnparseInScopeMap->[sourceFile][globalScope] = std::pair<SgStatement*,SgStatement*>(NULL,NULL);
                                             if (Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile.find(header_file_asssociated_source_file) ==
                                                 Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile.end())
                                                {
#if DEBUG_FIRST_LAST_STMTS_SCOPES || 0
                                                  printf ("Add map for this file is it does NOT already exists \n");
#endif
                                                  Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[header_file_asssociated_source_file] = std::map<SgScopeStatement*,std::pair<SgStatement*,SgStatement*> >();
                                                }
                                               else
                                                {
#if DEBUG_FIRST_LAST_STMTS_SCOPES || 0
                                                  printf ("A map for this file ALREADY exists \n");
#endif
                                                }
                                           }

                                     // std::map<SgScopeStatement*,std::pair<SgStatement*,SgStatement*> > firstAndLastStatementsToUnparseInScopeMap;
                                     // SgScopeStatement* scope = isSgScopeStatement(statement);

                                     // if (skipScope == false && scope != NULL)
                                     //   {
                                             ROSE_ASSERT(Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile.find(header_file_asssociated_source_file) !=
                                                         Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile.end());

                                             if (Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile.find(header_file_asssociated_source_file) ==
                                                 Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile.end())
                                                {
#if DEBUG_FIRST_LAST_STMTS_SCOPES || 0
                                                  printf ("A map for this file already exists \n");
#endif
                                                  Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[header_file_asssociated_source_file] =
                                                       std::map<SgScopeStatement*,std::pair<SgStatement*,SgStatement*> >();
                                                }
                                               else
                                                {
#if DEBUG_FIRST_LAST_STMTS_SCOPES || 0
                                                  printf ("A map for this file already exists \n");
#endif
                                                }

                                          // Need to add the global scope to support children of the global scope that are in the inlcude files.
                                          // SgGlobal* globalScope = isSgGlobal(parentScope);
                                          // if (globalScope != NULL)
                                             if (parentScope != nullptr)
                                                {
#if DEBUG_FIRST_LAST_STMTS_SCOPES || 0
                                                  printf ("parentScope != NULL \n");
#endif
                                                  if (Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[header_file_asssociated_source_file].find(parentScope) ==
                                                      Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[header_file_asssociated_source_file].end())
                                                     {
#if DEBUG_FIRST_LAST_STMTS_SCOPES || 0
                                                       printf ("Adding parentScope = %p = %s \n",parentScope,parentScope->class_name().c_str());
#endif
                                                       Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[header_file_asssociated_source_file][parentScope] = std::pair<SgStatement*,SgStatement*>(nullptr,nullptr);
                                                     }
                                                    else
                                                     {
#if DEBUG_FIRST_LAST_STMTS_SCOPES || 0
                                                       printf ("Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[header_file_asssociated_source_file][parentScope] is initialized \n");
#endif
                                                    // printf ("This should not be possible! \n");
                                                    // ROSE_ASSERT(false);
                                                     }
                                                }
                                               else
                                                {
#if DEBUG_FIRST_LAST_STMTS_SCOPES || 0
                                                  printf ("parentScope == NULL \n");
#endif
                                                }
#if 0
                                          // Since the SgGlobal is shared across the input source file and header files, this should be true for each source file seperately.
                                          // ROSE_ASSERT (firstAndLastStatementsToUnparseInScopeMap.find(scope) == firstAndLastStatementsToUnparseInScopeMap.end());
                                          // if (firstAndLastStatementsToUnparseInScopeMap.find(scope) == firstAndLastStatementsToUnparseInScopeMap.end())
                                          // ROSE_ASSERT(firstAndLastStatementsToUnparseInScopeMap != NULL);
                                          // ROSE_ASSERT (firstAndLastStatementsToUnparseInScopeMap->find(scope) == firstAndLastStatementsToUnparseInScopeMap->end());
                                          // if (firstAndLastStatementsToUnparseInScopeMap->find(scope) == firstAndLastStatementsToUnparseInScopeMap->end())
                                             if (Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[header_file_asssociated_source_file].find(scope) ==
                                                 Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[header_file_asssociated_source_file].end())
                                                {
                                                  printf ("Adding scope = %p = %s \n",scope,scope->class_name().c_str());
                                                  Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[header_file_asssociated_source_file][scope] = std::pair<SgStatement*,SgStatement*>(nullptr,nullptr);
                                                }
                                               else
                                                {
                                                  printf ("This should not be possible! \n");
                                                  ROSE_ASSERT(false);
                                                }
#endif
                                        // }

                                     // std::map<SgScopeStatement*,std::pair<SgStatement*,SgStatement*> > & firstAndLastStatementsToUnparseInScopeMap =
                                     //      Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[header_file_asssociated_source_file];

                                        if (parentScope != nullptr)
                                           {
                                             ROSE_ASSERT (Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile.find(header_file_asssociated_source_file) !=
                                                          Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile.end());
                                          // if (firstAndLastStatementsToUnparseInScopeMap->find(parentScope) == firstAndLastStatementsToUnparseInScopeMap->end())
                                             if (Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[header_file_asssociated_source_file].find(parentScope) ==
                                                 Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[header_file_asssociated_source_file].end())
                                                {
                                               // printf ("Error: (firstAndLastStatementsToUnparseInScopeMap->find(parentScope) == firstAndLastStatementsToUnparseInScopeMap->end()) == true \n");
                                                  printf ("Error: (Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[header_file_asssociated_source_file].find(parentScope) == Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[header_file_asssociated_source_file].end()) == true \n");
                                                  printf (" --- statement   = %p = %s = %s \n",statement,statement->class_name().c_str(),SageInterface::get_name(statement).c_str());
                                                  printf (" --- parentScope = %p = %s = %s \n",parentScope,parentScope->class_name().c_str(),SageInterface::get_name(parentScope).c_str());
                                                }
                                          // ROSE_ASSERT (firstAndLastStatementsToUnparseInScopeMap->find(parentScope) != firstAndLastStatementsToUnparseInScopeMap->end());
                                             ROSE_ASSERT (Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[header_file_asssociated_source_file].find(parentScope) !=
                                                          Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[header_file_asssociated_source_file].end());
                                          // ROSE_ASSERT (firstAndLastStatementsToUnparseInScopeMap[scope].first == scope);

                                          // firstAndLastStatementsToUnparseInScopeMap[parentScope].first  = NULL;
                                          // firstAndLastStatementsToUnparseInScopeMap[parentScope].second = NULL;
#if 0
                                          // DQ (6/28/2021): We want to remove the dependence upon the tokenStreamSequenceMap.
                                          // DQ (3/13/2021): Adding support to filter out collecting references to statements that don't have a corresponding token subsequence.
                                             std::map<SgNode*,TokenStreamSequenceToNodeMapping*> & tokenStreamSequenceMap = header_file_asssociated_source_file->get_tokenSubsequenceMap();
#if DEBUG_FIRST_LAST_STMTS_SCOPES || 0
                                             printf (" --- tokenStreamSequenceMap.size() = %zu \n",tokenStreamSequenceMap.size());
#endif
#endif
                                          // ROSE_ASSERT(statementBoundsMap.find(includeFile) != statementBoundsMap.end());
                                          // if (includeFile->get_firstStatement() == NULL)
                                          // if ( (firstAndLastStatementsToUnparseInScopeMap->[parentScope]).first == NULL)
                                             if (Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[header_file_asssociated_source_file][parentScope].first == nullptr)
                                                {
#if DEBUG_FIRST_LAST_STMTS_SCOPES
                                                  printf ("Previously NULL: first time seeing a statement for includeFile->get_filename() = %s \n",includeFile->get_filename().str());
#endif
#if 0
                                               // DQ (6/28/2021): We want to remove the dependence upon the tokenStreamSequenceMap.
                                                  if (tokenStreamSequenceMap.find(statement) != tokenStreamSequenceMap.end())
                                                     {
                                                       ROSE_ASSERT(statement != nullptr);

                                                    // includeFile->set_firstStatement(statement);
                                                    // firstAndLastStatementsToUnparseInScopeMap->[parentScope].first = statement;
                                                       Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[header_file_asssociated_source_file][parentScope].first = statement;

                                                       ROSE_ASSERT(statement->get_parent() != nullptr);
                                                     }
                                                    else
                                                     {
#if DEBUG_FIRST_LAST_STMTS_SCOPES
                                                       printf ("We can't record this as a first statement because it does not correspond to a token subsequence \n");
#endif
#if DEBUG_FIRST_LAST_STMTS_SCOPES
                                                       printf ("physical_file_id_from_source_file = %d \n",physical_file_id_from_source_file);
                                                       printf ("physical_file_id                  = %d \n",physical_file_id);
#endif
                                                       if (physical_file_id == physical_file_id_from_source_file)
                                                          {
                                                            Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[header_file_asssociated_source_file][parentScope].first = statement;
                                                          }
                                                     }
#else
#if DEBUG_FIRST_LAST_STMTS_SCOPES || 0
                                                  printf ("physical_file_id_from_source_file = %d \n",physical_file_id_from_source_file);
                                                  printf ("physical_file_id                  = %d \n",physical_file_id);
#endif
                                                  if (physical_file_id == physical_file_id_from_source_file)
                                                     {
                                                       Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[header_file_asssociated_source_file][parentScope].first = statement;
                                                     }
                                                    else
                                                     {
#if DEBUG_FIRST_LAST_STMTS_SCOPES || 0
                                                       printf ("Note: physical_file_id != physical_file_id_from_source_file: handling first statement \n");
#endif
                                                    // DQ (6/28/2021): We need to save the computed first and last statements for other files (e.g. header files) as well.
                                                       Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[header_file_asssociated_source_file][parentScope].first = statement;
                                                     }
#endif
                                                }

#if DEBUG_FIRST_LAST_STMTS_SCOPES
                                             printf (" --- (before testing statement parent) statement = %p = %s \n",statement,statement->class_name().c_str());
#endif
                                             ROSE_ASSERT(statement->get_parent() != nullptr);

#if 0
                                          // DQ (6/28/2021): We want to remove the dependence upon the tokenStreamSequenceMap.
                                          // DQ (3/13/2021): We need to make sure that the first and last statements that we select correspond
                                          // to a collected token subsequence. In codeSegregation test_141_1.h, demonstrates such a case.
                                          // includeFile->set_lastStatement(statement);
                                             if (tokenStreamSequenceMap.find(statement) != tokenStreamSequenceMap.end())
                                                {
#if DEBUG_FIRST_LAST_STMTS_SCOPES || 0
                                                  printf ("This can be a last statement (it has an associated token subsequence) \n");
#endif
                                               // includeFile->set_lastStatement(statement);
                                               // firstAndLastStatementsToUnparseInScopeMap->[parentScope].second = statement;
                                                  Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[header_file_asssociated_source_file][parentScope].second = statement;
                                                }
                                               else
                                                {
#if DEBUG_FIRST_LAST_STMTS_SCOPES || 0
                                                  printf ("We can't record this as a last statement because it does not correspond to a token subsequence \n");
                                               // printf ("This can be a last statement even if it does not have an associated token subsequence (e.g. it may be a transforamtion) \n");
#endif
#if DEBUG_FIRST_LAST_STMTS_SCOPES || 0
                                                  printf ("physical_file_id_from_source_file = %d \n",physical_file_id_from_source_file);
                                                  printf ("physical_file_id                  = %d \n",physical_file_id);
#endif
                                                  if (physical_file_id == physical_file_id_from_source_file)
                                                     {
                                                       Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[header_file_asssociated_source_file][parentScope].second = statement;
                                                     }
                                                    else
                                                     {
#if DEBUG_FIRST_LAST_STMTS_SCOPES || 0
                                                       printf ("Note: physical_file_id != physical_file_id_from_source_file: handling the last statement \n");
#endif
                                                     }
                                                }
#else
#if DEBUG_FIRST_LAST_STMTS_SCOPES || 0
                                             printf ("physical_file_id_from_source_file = %d \n",physical_file_id_from_source_file);
                                             printf ("physical_file_id                  = %d \n",physical_file_id);
#endif
                                             if (physical_file_id == physical_file_id_from_source_file)
                                                {
                                                  Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[header_file_asssociated_source_file][parentScope].second = statement;
                                                }
                                               else
                                                {
#if DEBUG_FIRST_LAST_STMTS_SCOPES || 0
                                                  printf ("Note: physical_file_id != physical_file_id_from_source_file: handling the last statement \n");
#endif
                                               // DQ (6/28/2021): We need to save the computed first and last statements for other files (e.g. header files) as well.
                                                  Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[header_file_asssociated_source_file][parentScope].second = statement;
                                                }
#endif
                                           }
                                          else
                                           {
#if DEBUG_FIRST_LAST_STMTS_SCOPES || 0
                                             printf ("parentScope == nullptr \n");
#endif
                                           }
                                      }
                                     else
                                      {
#if DEBUG_FIRST_LAST_STMTS_SCOPES || 0
                                        printf ("Token stream not found for this file: header_file_asssociated_source_file = %s \n",header_file_asssociated_source_file->getFileName().c_str());
#endif
#if 1
                                        printf ("We want to fix this so that this case should not be possible! \n");
                                        ROSE_ASSERT(false);
#endif
                                      }
                                 }
                            }
                           else
                            {
                           // Not all statements will be in the header files, however this could be the input source file,
                           // so we need to support computing the first and last statements in that file as well.
#if DEBUG_FIRST_LAST_STMTS_SCOPES || 0
                              printf ("filename not found in edg_include_file_map: filename = %s \n",filename.c_str());
#endif
                              ROSE_ASSERT(sourceFile != nullptr);

                              if (Rose::tokenSubsequenceMapOfMapsBySourceFile.find(sourceFile) != Rose::tokenSubsequenceMapOfMapsBySourceFile.end())
                                 {
                                // DQ (3/13/2021): Adding support to filter out collecting references to statements that don't have a corresponding token subsequence.
                                   std::map<SgNode*,TokenStreamSequenceToNodeMapping*> & tokenStreamSequenceMap = sourceFile->get_tokenSubsequenceMap();
#if DEBUG_FIRST_LAST_STMTS_SCOPES
                                   printf (" --- tokenStreamSequenceMap.size() = %zu \n",tokenStreamSequenceMap.size());
#endif
                                // ROSE_ASSERT(statementBoundsMap.find(includeFile) != statementBoundsMap.end());

                                // firstAndLastStatementsToUnparseInScopeMap = &(Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[header_file_asssociated_source_file]);
                                // ROSE_ASSERT(firstAndLastStatementsToUnparseInScopeMap != NULL);

                                   SgScopeStatement* scope = isSgScopeStatement(statement);
                                // if (scope != NULL)
                                   if (skipScope == false && scope != nullptr)
                                      {
                                     // Since the SgGlobal is shared across the input source file and header files, this should be true for each source file seperately.
#if 0
                                        ROSE_ASSERT (firstAndLastStatementsToUnparseInScopeMap.find(scope) == firstAndLastStatementsToUnparseInScopeMap.end());
                                        if (firstAndLastStatementsToUnparseInScopeMap.find(scope) == firstAndLastStatementsToUnparseInScopeMap.end())
                                           {
                                          // std::pair<SgStatement*,SgStatement*> tmp_pair = std::pair<SgStatement*,SgStatement*>(NULL,NULL);
                                          // firstAndLastStatementsToUnparseInScopeMap[scope] = tmp_pair;
                                             firstAndLastStatementsToUnparseInScopeMap[scope] = std::pair<SgStatement*,SgStatement*>(NULL,NULL);
                                           }
                                          else
                                           {
                                             printf ("This should not be possible! \n");
                                             ROSE_ASSERT(false);
                                           }
#endif
                                        ROSE_ASSERT(Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile.find(sourceFile) !=
                                                    Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile.end());

                                        if (Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile.find(sourceFile) ==
                                            Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile.end())
                                           {
#if DEBUG_FIRST_LAST_STMTS_SCOPES || 0
                                             printf ("A map for this file DOES already exist \n");
#endif
                                             Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[sourceFile] =
                                                  std::map<SgScopeStatement*,std::pair<SgStatement*,SgStatement*> >();
                                           }
                                          else
                                           {
#if DEBUG_FIRST_LAST_STMTS_SCOPES || 0
                                             printf ("A map for this file does NOT already exist \n");
#endif
                                           }
                                      }

                                // Need to add the global scope to support children of the global scope that are in the inlcude files.
                                   if (parentScope != nullptr)
                                      {
#if DEBUG_FIRST_LAST_STMTS_SCOPES
                                        printf ("parentScope != NULL \n");
#endif
                                        if (Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[sourceFile].find(parentScope) ==
                                            Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[sourceFile].end())
                                           {
#if DEBUG_FIRST_LAST_STMTS_SCOPES
                                             printf ("Adding parentScope = %p = %s \n",parentScope,parentScope->class_name().c_str());
#endif
                                             Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[sourceFile][parentScope] = std::pair<SgStatement*,SgStatement*>(nullptr,nullptr);
                                           }
                                          else
                                           {
#if DEBUG_FIRST_LAST_STMTS_SCOPES
                                             printf ("Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[sourceFile][parentScope] is initialized \n");
#endif
                                          // printf ("This should not be possible! \n");
                                          // ROSE_ASSERT(false);
                                           }
                                      }

                                // SgScopeStatement* parentScope = isSgScopeStatement(statement->get_parent());
                                   if (parentScope != nullptr)
                                      {
                                     // ROSE_ASSERT (firstAndLastStatementsToUnparseInScopeMap.find(parentScope) != firstAndLastStatementsToUnparseInScopeMap.end());
                                        ROSE_ASSERT (Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[sourceFile].find(parentScope) !=
                                                     Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[sourceFile].end());
                                     // ROSE_ASSERT (firstAndLastStatementsToUnparseInScopeMap[scope].first == scope);

                                     // DQ (5/20/2021): The firstStatment and lastStatement in the source file are only used for the
                                     // input source file (the include files used their own data member).
                                     // if (sourceFile->get_firstStatement() == NULL)
                                        if (Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[sourceFile][parentScope].first == nullptr)
                                           {
#if DEBUG_FIRST_LAST_STMTS_SCOPES
                                             printf ("Previously NULL: first time seeing a statement for sourceFile->get_filename() = %s \n",sourceFile->getFileName().c_str());
#endif
                                             if (tokenStreamSequenceMap.find(statement) != tokenStreamSequenceMap.end())
                                                {
                                               // sourceFile->set_firstStatement(statement);
                                               // firstAndLastStatementsToUnparseInScopeMap[parentScope].first = statement;
                                                  Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[sourceFile][parentScope].first = statement;
                                                }
                                               else
                                                {
#if DEBUG_FIRST_LAST_STMTS_SCOPES
                                                  printf ("We can't record this as a first statement becuase it does not correspond to a token subsequence \n");
#endif
                                               // DQ (6/1/2021): Record the first statement if is is associated with the current file.
                                               // This might be a new statement (transformation).
#if DEBUG_FIRST_LAST_STMTS_SCOPES
                                                  printf ("physical_file_id_from_source_file = %d \n",physical_file_id_from_source_file);
                                                  printf ("physical_file_id                  = %d \n",physical_file_id);
#endif
                                                  if (physical_file_id == physical_file_id_from_source_file)
                                                     {
                                                       Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[sourceFile][parentScope].first = statement;
                                                     }
                                                }
                                           }

                                        ROSE_ASSERT(statement->get_parent() != nullptr);
#if DEBUG_FIRST_LAST_STMTS_SCOPES
                                        printf (" --- (before testing statement parent) statement->get_parent() = %p = %s \n",statement->get_parent(),statement->get_parent()->class_name().c_str());
#endif
                                     // DQ (5/20/2021): We can't have the last statement be the global scope in the input source file.
                                     // DQ (3/13/2021): We need to make sure that the first and last statements that we select correspond
                                     // to a collected token subsequence. In codeSegregation test_141_1.h, demonstrates such a case.
                                     // includeFile->set_lastStatement(statement);
                                     // if (tokenStreamSequenceMap.find(statement) != tokenStreamSequenceMap.end())
                                        if (tokenStreamSequenceMap.find(statement) != tokenStreamSequenceMap.end())
                                           {
#if DEBUG_FIRST_LAST_STMTS_SCOPES
                                             printf ("This can be a last statement (it has an associated token subsequence) \n");
#endif
                                          // sourceFile->set_lastStatement(statement);
                                          // firstAndLastStatementsToUnparseInScopeMap[parentScope].second = statement;
                                             Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[sourceFile][parentScope].second = statement;
                                           }
                                          else
                                           {
#if DEBUG_FIRST_LAST_STMTS_SCOPES
                                          // printf ("We can't record this as a last statement because it does not correspond to a token subsequence \n");
                                             printf ("This can be a last statement even if it does not have an associated token subsequence (e.g. it may be a transforamtion) \n");
#endif
                                          // DQ (6/1/2021): Record the first statement if is is associated with the current file.
                                          // This might be a new statement (transformation).
#if DEBUG_FIRST_LAST_STMTS_SCOPES
                                             printf ("physical_file_id_from_source_file = %d \n",physical_file_id_from_source_file);
                                             printf ("physical_file_id                  = %d \n",physical_file_id);
#endif
                                             if (physical_file_id == physical_file_id_from_source_file)
                                                {
                                                  Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[sourceFile][parentScope].second = statement;
                                                }
                                           }
                                      }
                                 }
                                else
                                 {
                                // The token stream was not found, however, I think that w still must compute the first and last statment,
                                // independent of if there was a token stream associated with it.

#if DEBUG_FIRST_LAST_STMTS_SCOPES
                                   printf ("Token stream not found for this file: sourceFile = %s \n",sourceFile->getFileName().c_str());
#endif
                                 }
                            }
                       }
                  }
        };


     StatementTraversal traversal;

#if DEBUG_FIRST_LAST_STMTS_SCOPES
     printf ("Before call to traversal \n");
#endif

  // DQ (3/11/2021): Need to call this on a specific source file (the one on the original command line,
  // not the dynamic library file, so that we will avoid marking SgIncludeFiles twice (since two files
  // will include the same header files).  An altrnative implementation could define a header file list
  // for each source file, but this would be more complex and error prone, and hard to debug)).
  // traversal.traverse(project,preorder);
  // SgFilePtrList & fileList = project->get_files();
     SgFilePtrList & fileList = project->get_fileList();
     for (size_t i = 0; i < fileList.size(); i++)
        {
          SgSourceFile* sourceFile = isSgSourceFile(fileList[i]);

       // DQ (8/23/2021): If this is a binary file then sourceFile == NULL (see tests in
       // tests/nonsmoke/functional/CompilerOptionsTests/testGenerateSourceFileNames)
       // ROSE_ASSERT(sourceFile != NULL);
          if (sourceFile != nullptr)
             {
#if DEBUG_FIRST_LAST_STMTS_SCOPES
               printf ("\nFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF \n");
               printf ("Testing for isDynamicLibrary before calling traversal for filename = %s \n",sourceFile->getFileName().c_str());
#endif
#if DEBUG_FIRST_LAST_STMTS_SCOPES
               printf ("sourceFile->get_isDynamicLibrary() = %s \n",sourceFile->get_isDynamicLibrary() ? "true" : "false");
#endif
            // DQ (5/24/2021): Since we have to unparse all the files, we need to compute first and last on all of the files.
            // What is less clear is what to do with the information about shared header files. I think that at worst it is
            // redundant information.
#if DEBUG_FIRST_LAST_STMTS_SCOPES
               printf ("Calling traversal for filename = %s \n",sourceFile->getFileName().c_str());
#endif
               traversal.traverse(sourceFile,preorder);

            // Copy the information of first and last statement per scope for each file to store it in the source file.
            // Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[sourceFile] = traversal.firstAndLastStatementsToUnparseInScopeMap;

#if DEBUG_FIRST_LAST_STMTS_SCOPES
               printf ("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF \n\n");
#endif
             }
        }

#if DEBUG_FIRST_LAST_STMTS_SCOPES
     printf ("After call to traversal \n");
#endif

#if DEBUG_FIRST_LAST_STMTS_SCOPES || 0
  // DQ (3/14/2021): Output include saved in the SgIncludeFile about first and last computed statements in each header file.
  // void outputFirstAndLastIncludeFileInfo();
  // outputFirstAndLastIncludeFileInfo();

  // for (size_t i = 0; i < fileList.size(); i++)
  //    {
  //      SgSourceFile* sourceFile = isSgSourceFile(fileList[i]);
  //      ROSE_ASSERT(sourceFile != NULL);

  //      printf ("In buildFirstAndLastStatementsForScopes(): iteration i = %d \n",i);

       // DQ (5/24/2021): Since we have to unparse all the files, we need to compute first and last on all of the files.
       // What is less clear is what to do with the information about shared header files. I think that at worst it is
       // redundant information.
       // if (sourceFile->get_isDynamicLibrary() == false)
       //    {
            // Only output once... some tools (e.g. codeSegregation tool) will build an additional file, in which case we don't want to output info for that file.
            // outputFirstAndLastIncludeFileInfo(sourceFile);
            // outputFirstAndLastStatementsInScope(sourceFile);

            // ROSE_ASSERT(Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile.find(sourceFile) != Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile.end());

               std::map<SgSourceFile*,std::map<SgScopeStatement*,std::pair<SgStatement*,SgStatement*> > >::iterator j = Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile.begin();

               printf ("Output Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile (size = %zu): \n",Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile.size());
               while (j != Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile.end())
                  {
                    SgSourceFile* tmp_sourceFile = j->first;
                    printf ("tmp_sourceFile = %p name = %s \n",tmp_sourceFile,tmp_sourceFile->getFileName().c_str());

                 // std::map<SgScopeStatement*,std::pair<SgStatement*,SgStatement*> > firstAndLastStatements = Rose::firstAndLastStatementsToUnparseInScopeMapBySourceFile[sourceFile];
                    std::map<SgScopeStatement*,std::pair<SgStatement*,SgStatement*> > firstAndLastStatements = j->second;

                    printf (" --- firstAndLastStatements (size() = %zu): \n",firstAndLastStatements.size());
                    std::map<SgScopeStatement*,std::pair<SgStatement*,SgStatement*> >::iterator i = firstAndLastStatements.begin();
                    while (i != firstAndLastStatements.end())
                       {
                         SgScopeStatement* scope = i->first;
                         ROSE_ASSERT(scope != nullptr);
                         printf (" --- --- scope              = %p = %s name = %s \n",scope,scope->class_name().c_str(),SageInterface::get_name(scope).c_str());

                         SgStatement* firstStatement = i->second.first;
                         printf (" --- --- --- firstStatement  = %p \n",firstStatement);
                         if (firstStatement != nullptr)
                            {
                              printf (" --- --- --- firstStatement = %p = %s name = %s \n",firstStatement,firstStatement->class_name().c_str(),SageInterface::get_name(firstStatement).c_str());
                            }

                         SgStatement* lastStatement = i->second.second;
                         printf (" --- --- --- lastStatement  = %p \n",lastStatement);
                         if (lastStatement != nullptr)
                            {
                              printf (" --- --- --- lastStatement  = %p = %s name = %s \n",lastStatement,lastStatement->class_name().c_str(),SageInterface::get_name(lastStatement).c_str());
                            }

                         printf ("\n");

                         i++;
                       }

                    printf ("\n");

                    j++;
                  }
#endif

#if DEBUG_FIRST_LAST_STMTS_SCOPES
     printf ("Leaving buildFirstAndLastStatementsForScopes(): project = %p \n",project);
#endif
   }

// DQ (11/10/2018): Move this ot a more common location.
void generateGraphOfIncludeFiles( SgProject* project, std::string filename );

void unparseIncludedFiles ( SgProject* project, UnparseFormatHelp *unparseFormatHelp, UnparseDelegate* unparseDelegate)
   {
     ASSERT_not_null(project);

#define DEBUG_UNPARSE_INCLUDE_FILES 0

  // DQ (3/10/2021): Add performance analysis support.
     TimingPerformance timer ("AST unparseIncludedFiles:");

  // DQ (3/11/2020): The transformation of header files causes them to be output into a separate directory location.
  // The paths associated with each transformed header file must be saved so that then can be output on the compile
  // line for the generated source files.  The current design supports an extra include path list so that we can support
  // the specification of the paths to the transformed header files ahead of the source file's original include path list.
  // However, the extra include paths are strored in the SgSourceFile IR nodes, and there can be more than one source
  // file that is required to support the include path list that includes the paths to the transformed header files.
  // Not clear which is the best way to support this.
  // 1) Currently we build the list in the SgSourceFile for the include file to have a modified extra include path list,
  //    however, this is pointless since the header files are not compiled. And it is insufficient because the source
  //    files that are compiled don't have the correct extra include path entries.
  // 2) We could store the extra include paths as they are built up in this function, but I don't think this function
  //    is calling the backend compiler.
  // 3) We could build the list of the source files to be compiled, but this might not be a great solution since it
  // would cause every source file to have the same extra include paths.
  // 4) (best idea so far) We need to add a second extra list of include paths to the SgProject object, then when
  // building the include list for each file we can first (or second) include the paths from the SgProject's extra
  // include paths list before adding those specific to the SgSourceFile.

#if DEBUG_UNPARSE_INCLUDE_FILES
     printf ("In unparseIncludedFiles(): project = %p \n",project);
#endif
#if DEBUG_UNPARSE_INCLUDE_FILES
     printf ("TOP of unparseIncludedFiles(): EDG_ROSE_Translation::edg_include_file_map.size() = %zu \n",EDG_ROSE_Translation::edg_include_file_map.size());
#endif

#if DEBUG_UNPARSE_INCLUDE_FILES
     printf ("In unparseIncludedFiles(): Output include_file_graph_from_top_of_unparseIncludedFiles DOT graph \n");
     string dotgraph_filename = "include_file_graph_from_top_of_unparseIncludedFiles";
     generateGraphOfIncludeFiles(project,dotgraph_filename);
     printf ("DONE: In unparseIncludedFiles(): Output include_file_graph_from_top_of_unparseIncludedFiles DOT graph \n");
#endif

  // DQ (5/2/2021): Get the file so that we can get the data member for unparseHeaderFiles.
  // NOTE: An improvement would be to make the data member for unparseHeaderFiles a static data member.
     SgFile* file = nullptr;
     if (!project -> get_fileList().empty() && (*(project -> get_fileList()).begin()) -> get_unparseHeaderFiles())
        {
          file = *((project -> get_fileList()).begin());
        }

  // Proceed only if there are input files and they require header files unparsing.
  // if (!project -> get_fileList().empty() && (*(project -> get_fileList()).begin()) -> get_unparseHeaderFiles())
     if (file != nullptr && file->get_unparseHeaderFiles() == true)
        {
          if (SgProject::get_verbose() >= 1)
             {
               cout << endl << "***HEADER FILES UNPARSING***" << endl << endl;
             }
          IncludedFilesUnparser includedFilesUnparser(project);

#if DEBUG_UNPARSE_INCLUDE_FILES
          printf ("BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB \n");
          printf ("BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB \n");
          printf ("In unparseIncludedFiles(): calling buildFirstAndLastStatementsForIncludeFiles() \n");
          printf ("BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB \n");
          printf ("BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB \n");
#endif

          SgSourceFile* sourceFile = isSgSourceFile(file);
          ROSE_ASSERT(sourceFile != NULL);
          if (sourceFile->get_unparse_tokens() == true)
             {
               buildFirstAndLastStatementsForIncludeFiles(project);

            // DQ (5/27/2021): We need to debug the collection of first and last statements associated with each scope
            // (actually we just need the last statement for each scope). This only important for the token-based unarpsing.
               buildFirstAndLastStatementsForScopes(project);
             }

#if DEBUG_UNPARSE_INCLUDE_FILES
          printf ("In unparseIncludedFiles(): before figureOutWhichFilesToUnparse(): EDG_ROSE_Translation::edg_include_file_map.size() = %zu \n",EDG_ROSE_Translation::edg_include_file_map.size());
#endif
          includedFilesUnparser.figureOutWhichFilesToUnparse();

#if DEBUG_UNPARSE_INCLUDE_FILES
          printf ("In unparseIncludedFiles(): after figureOutWhichFilesToUnparse(): EDG_ROSE_Translation::edg_include_file_map.size() = %zu \n",EDG_ROSE_Translation::edg_include_file_map.size());
#endif

#if 0
       // Output the DOT graph for debugging.
          generateDOT_withIncludes(*project, ".unparsing_headers");
#endif

          const string& unparseRootPath                          = includedFilesUnparser.getUnparseRootPath();
          const map<string, string>& unparseMap                  = includedFilesUnparser.getUnparseMap();
          const map<string, SgScopeStatement*>& unparseScopesMap = includedFilesUnparser.getUnparseScopesMap();

#if DEBUG_UNPARSE_INCLUDE_FILES
          printf ("In unparseIncludedFiles(): Calling prependIncludeOptionsToCommandLine(): includedFilesUnparser.getIncludeCompilerOptions().size() = %zu \n",
               includedFilesUnparser.getIncludeCompilerOptions().size());
          list<string> includeCompilerOptionsList = includedFilesUnparser.getIncludeCompilerOptions();
          for (list<string>::iterator i = includeCompilerOptionsList.begin(); i != includeCompilerOptionsList.end(); i++)
             {
            // printf (" --- includedFilesUnparser.getIncludeCompilerOptions() = %s \n",(*i).c_str());
               string s = *i;
               printf (" --- includedFilesUnparser.getIncludeCompilerOptions() = %s \n",s.c_str());
             }
#endif
#if DEBUG_UNPARSE_INCLUDE_FILES
          printf ("In unparseIncludedFiles(): before prependIncludeOptionsToCommandLine(): EDG_ROSE_Translation::edg_include_file_map.size() = %zu \n",EDG_ROSE_Translation::edg_include_file_map.size());
#endif
          prependIncludeOptionsToCommandLine(project, includedFilesUnparser.getIncludeCompilerOptions());
#if 0
       // DQ (10/23/2018): Output report of AST nodes marked as modified!
          SageInterface::reportModifiedStatements("After prependIncludeOptionsToCommandLine()",project);
#endif

          if (SgProject::get_unparseHeaderFilesDebug() >= 2)
             {
               const map<string, SgSourceFile*> & temp_unparseSourceFileMap = includedFilesUnparser.getUnparseSourceFileMap();
               printf ("Output the temp_unparseSourceFileMap: \n");
               for (map<string, SgSourceFile*>::const_iterator sourceFile = temp_unparseSourceFileMap.begin(); sourceFile != temp_unparseSourceFileMap.end(); sourceFile++)
                  {
                    printf ("   --- sourceFile->first = %s sourceFile->second = %p = %s \n",sourceFile->first.c_str(),sourceFile->second,sourceFile->second->class_name().c_str());
                  }
             }

#if DEBUG_UNPARSE_INCLUDE_FILES
          printf ("In unparseIncludedFiles(): before while loop over unparseMap (files to copy): EDG_ROSE_Translation::edg_include_file_map.size() = %zu \n",EDG_ROSE_Translation::edg_include_file_map.size());
#endif

       // DQ (11/19/2018): Copy the files that are specified in the filesToCopy list.
          set<string> copySet = includedFilesUnparser.getFilesToCopy();
          set<string>::iterator copySetInterator = copySet.begin();
          while (copySetInterator != copySet.end())
             {
#if DEBUG_UNPARSE_INCLUDE_FILES
               printf ("copySetInterator = %s \n",(*copySetInterator).c_str());
#endif
               string originalFileName = *copySetInterator;

               if (SgProject::get_unparseHeaderFilesDebug() > 4)
                  {
                    printf ("Loop over files to copy: originalFileName = %s not found in unparseSourceFileMap \n",originalFileName.c_str());
                  }

            // DQ (11/19/2018): Retrieve the original SgSourceFile constructed within the frontend processing.
               map<string, SgSourceFile*> unparseSourceFileMap = includedFilesUnparser.getUnparseSourceFileMap();
               if (unparseSourceFileMap.find(originalFileName) == unparseSourceFileMap.end())
                  {
                    if (SgProject::get_unparseHeaderFilesDebug() > 4)
                       {
                         printf ("NOTE: originalFileName = %s not found in unparseSourceFileMap \n",originalFileName.c_str());
                       }

                    ASSERT_not_null(project);
                    string applicationRootDirectory = project->get_applicationRootDirectory();
                    string filenameWithOutPath = FileHelper::getFileName(originalFileName);
                    string adjusted_header_file_directory = unparseRootPath;
                    string name_used_in_include_directive = originalFileName;
                    string directoryPathPrefix = Rose::getPathFromFileName(name_used_in_include_directive);

                    size_t pos = directoryPathPrefix.find(applicationRootDirectory);
                    if (pos != string::npos)
                       {
                         directoryPathPrefix.erase(pos,applicationRootDirectory.length());
                       }
                    if (directoryPathPrefix == ".")
                       {
                         directoryPathPrefix = "";
                       }

                 // DQ (11/30/2019): Avoid output of string with double "/" as in "//".
                    if (directoryPathPrefix != "")
                       {
                         directoryPathPrefix += "/";
                       }

                    adjusted_header_file_directory += "/" + directoryPathPrefix;
                    string newFileName = adjusted_header_file_directory + filenameWithOutPath;

                    boost::filesystem::path pathPrefix(adjusted_header_file_directory);
                    create_directories(pathPrefix);

                    boost::filesystem::path originalFileNamePath(originalFileName);
                    boost::filesystem::path newFileNamePath(newFileName);
                    if (exists(newFileNamePath) == false)
                       {
                         copy_file(originalFileNamePath, newFileNamePath);
                       }
                  }
                 else
                  {
                    ROSE_ASSERT(unparseSourceFileMap.find(originalFileName) != unparseSourceFileMap.end());

                    SgSourceFile* unparsedFile = unparseSourceFileMap[originalFileName];
                    ASSERT_not_null(unparsedFile);

                    string filenameWithOutPath = FileHelper::getFileName(originalFileName);

                    string adjusted_header_file_directory = unparseRootPath;

                 // DQ (1/1/2019): Append the filename as a suffix to the userSpecifiedUnparseRootFolder so that we can avoid header file
                 // location collisions when compiling either multiple files or multiple files in parallel.

                    SgIncludeFile* associated_include_file = unparsedFile->get_associated_include_file();
                    ASSERT_not_null(associated_include_file);

                    string name_used_in_include_directive = associated_include_file->get_name_used_in_include_directive();
                    string directoryPathPrefix = Rose::getPathFromFileName(name_used_in_include_directive);
                    if (directoryPathPrefix == ".") {
                        directoryPathPrefix = "";
                    }

                    directoryPathPrefix += "/";
                    adjusted_header_file_directory += "/" + directoryPathPrefix;

                    string newFileName = adjusted_header_file_directory + filenameWithOutPath;

                    bool isApplicationFile = associated_include_file->get_isApplicationFile();
                    if (isApplicationFile)
                       {
                         boost::filesystem::path pathPrefix(adjusted_header_file_directory);
                         create_directories(pathPrefix);

                         boost::filesystem::path originalFileNamePath(originalFileName);
                         boost::filesystem::path newFileNamePath(newFileName);

                      // This will throw an error if newFileNamePath exists
                         copy_file(originalFileNamePath, newFileNamePath);
                       }

                    printf ("Exiting as a test! \n");
                    ROSE_ABORT();
                  }

               copySetInterator++;
             }

#if DEBUG_UNPARSE_INCLUDE_FILES
          printf ("DONE: In unparseIncludedFiles(): before while loop over unparseMap (files to copy): EDG_ROSE_Translation::edg_include_file_map.size() = %zu \n",
               EDG_ROSE_Translation::edg_include_file_map.size());
#endif

#if DEBUG_UNPARSE_INCLUDE_FILES
          printf ("In unparseIncludedFiles(): before for loop over unparseMap: EDG_ROSE_Translation::edg_include_file_map.size() = %zu \n",EDG_ROSE_Translation::edg_include_file_map.size());
#endif

#if DEBUG_UNPARSE_INCLUDE_FILES || 0
          printf ("file->get_unparseHeaderFiles() = %s \n",file->get_unparseHeaderFiles() ? "true" : "false");
#endif

       // DQ (5/2/2021): We can assert this because of the predicate for this true case (above).
          ROSE_ASSERT(file->get_unparseHeaderFiles() == true);

#if 0
          printf ("Generating a dot file... (debugging token based unparsing of header file unparsing) \n");
          generateDOT_withIncludes ( *project , "dot_file_graph_from_token_frontier_of_unparseIncludedFiles" );
#endif

          for (map<string, string>::const_iterator unparseMapEntry = unparseMap.begin(); unparseMapEntry != unparseMap.end(); unparseMapEntry++)
             {
               string originalFileName = unparseMapEntry->first;
               string originalFileNameWithoutPath = Rose::utility_stripPathFromFileName(originalFileName);

            // DQ (3/7/2020): Save the path so that we can include it in the list of include paths that we need to add.
               string originalFileNamePath = Rose::getPathFromFileName(originalFileName);

#if DEBUG_UNPARSE_INCLUDE_FILES
               printf ("Use this path when compiling generated code: originalFileNamePath = %s \n",originalFileNamePath.c_str());
#endif
               if (SgProject::get_unparseHeaderFilesDebug() >= 4)
                  {
                    printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                    printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                    printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                    printf ("TOP of loop over unparseMap \n");
                    printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                    printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                    printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                  }

               if (SgProject::get_unparseHeaderFilesDebug() >= 7)
                  {
                    printf ("In unparseIncludedFiles(): Processing unparseMapEntries: originalFileName            = %s \n",originalFileName.c_str());
                    printf ("In unparseIncludedFiles(): Processing unparseMapEntries: originalFileNameWithoutPath = %s \n",originalFileNameWithoutPath.c_str());
                  }
#if 0
            // DQ (10/23/2018): Output report of AST nodes marked as modified!
               SageInterface::reportModifiedStatements("Top of loop in unparseIncludedFiles()",project);
#endif

// #if 1
            // DQ (9/7/2018): Retrieve the original SgSourceFile constructed within the frontend processing.
            // map<string, SgSourceFile*> unparseSourceFileMap = includedFilesUnparser.getUnparseSourceFileMap();
               map<string, SgSourceFile*> unparseSourceFileMap = includedFilesUnparser.getUnparseSourceFileMap();

            // DQ (9/10/2018): This appears to be an error, force this as a test and exit until we fix this.
               if (unparseSourceFileMap.find(originalFileName) == unparseSourceFileMap.end())
                  {
#if 0
                    printf ("In unparseIncludedFiles(): looking in unparseSourceFileMap for originalFileName = %s \n",originalFileName.c_str());
                    map<string, SgSourceFile*>::iterator i = unparseSourceFileMap.begin();
                    while (i != unparseSourceFileMap.end())
                       {
                         string name = i->first;
                         printf (" --- unparseSourceFileMap: name = %s \n",name.c_str());
                         i++;
                       }
#endif
#if 0
                 // DQ (9/27/2019): Since header files are in a separate list (I think) it should not be an error to misss it in this list.
                 // I think I need to re-evaluate this!
                    printf ("NOTE: originalFileName = %s not found in unparseSourceFileMap \n",originalFileName.c_str());
#endif

                 // DQ (10/1/2019): We need to build a SgSourceFile to hold the statements that are in the header file which we have not processed yet.
                 // It might be more effective to build the list and then process all of the files at once, though it might be more complex and better
                 // to do that later.
#if 0
                    printf ("We need to build a SgSourceFile to hold the statements that are in the header file which we have not processed yet \n");
#endif
#if 0
                    printf ("In unparseIncludedFiles(): before buildSourceFileForHeaderFile(): EDG_ROSE_Translation::edg_include_file_map.size() = %zu \n",
                         EDG_ROSE_Translation::edg_include_file_map.size());
                    printf (" --- project->get_unparse_tokens() = %s \n",project->get_unparse_tokens() ? "true" : "false");
#endif
                 // Build a SgSourceFile into the unparseSourceFileMap (mark it as a header file and point to the global scope that has the subset
                 // of its statements, including any statements from nested header files).
                    SgSourceFile* headerFileOnDemand = buildSourceFileForHeaderFile(project,originalFileName);

                 // DQ (5/8/2021): If this is not an application file, then buildSourceFileForHeaderFile() returns NULL.
                 // ASSERT_not_null(headerFileOnDemand);
                    if (headerFileOnDemand != nullptr)
                       {
#if 0
                         printf ("In unparseIncludedFiles(): after buildSourceFileForHeaderFile(): EDG_ROSE_Translation::edg_include_file_map.size() = %zu \n",
                              EDG_ROSE_Translation::edg_include_file_map.size());
                         printf (" --- headerFileOnDemand->getFileName() = %s \n",headerFileOnDemand->getFileName().c_str());
                         printf (" --- headerFileOnDemand->get_unparse_tokens() = %s \n",headerFileOnDemand->get_unparse_tokens() ? "true" : "false");
#endif
                      // DQ (10/2/2019): This will be checked below (test it here), but it is not reasonable for a header file when using the header file unparsing optimization.
                      // ASSERT_not_null(headerFileOnDemand->get_project());

                      // Add the new SgSourceFile for the header file to the unparseSourceFileMap (shoudl this be a reference to the includedFilesUnparser.getUnparseSourceFileMap()?)
                         unparseSourceFileMap[originalFileName] = headerFileOnDemand;
                       }
                      else
                       {
                      // DQ (5/8/2021): Make an entry in the map, but with associated NULL pointer.
                         unparseSourceFileMap[originalFileName] = nullptr;
                       }

#if 0
                    printf ("Error: originalFileName = %s not found in unparseSourceFileMap added: headerFileOnDemand = %p \n",originalFileName.c_str(),headerFileOnDemand);
                    ROSE_ABORT();
#endif
                  }
#if 1
            // DQ (9/27/2019): Since header files are in a separate list (I think) it should not be an error to misss it in this list.
            // I think I need to re-evaluate this!
               ROSE_ASSERT(unparseSourceFileMap.find(originalFileName) != unparseSourceFileMap.end());
#endif

#if 0
               printf ("Look for the originalFileName = %s in the edg_include_file_map (size = %zu) \n",originalFileName.c_str(),edg_include_file_map.size());

            // DQ (9/29/2019): If the filename is not in the unparseSourceFileMap, then look into the edg_include_file_map (which should maybe be a variable that we should rename).
               if (unparseSourceFileMap.find(originalFileName) == unparseSourceFileMap.end())
                  {
                    ROSE_ASSERT(edg_include_file_map.find(originalFileName) != edg_include_file_map.end());

                 // Then get the SgSourceFile and add it to the unparseSourceFileMap?
                  }
#endif
#if 1
            // DQ (9/27/2019): Since header files are in a separate list (I think) it should not be an error to misss it in this list.
            // I think I need to re-evaluate this!
               ROSE_ASSERT(unparseSourceFileMap.find(originalFileName) != unparseSourceFileMap.end());
#endif
               SgSourceFile* unparsedFile = unparseSourceFileMap[originalFileName];

            // ASSERT_not_null(unparsedFile);

            // DQ (5/8/2021): Handle the case when unparsedFile != NULL.
               if (unparsedFile != nullptr)
                  {

                 // #if DEBUG_UNPARSE_INCLUDE_FILES
                 // DQ (4/4/2020): Added header file unparsing feature specific debug level.
                    if (SgProject::get_unparseHeaderFilesDebug() >= 7)
                       {
                         printf ("Unparse file from unparseMap: unparsedFile = %p filename = %s \n",unparsedFile,unparsedFile->getFileName().c_str());
                         printf ("                              size of global scope = %zu \n",unparsedFile->get_globalScope()->get_declarations().size());
                       }

                 // DQ (11/12/2018): This is the newer approach to getting any associated SgIncludeFile information.
                    SgIncludeFile* associated_include_file = unparsedFile->get_associated_include_file();
                    if (associated_include_file != nullptr)
                       {

                      // DQ (11/15/2018): Mark this as a header file that will be unparsed.
                         associated_include_file->set_will_be_unparsed(true);

                         if (associated_include_file->get_can_be_supported_using_token_based_unparsing() == false)
                            {
                              if (SgProject::get_unparseHeaderFilesDebug() > 0)
                                 {
                                   printf ("Skip over this entry in the unparseMap \n");
                                 }
                              continue;
                            }
                       }

                    unparsedFile -> set_sourceFileNameWithoutPath(FileHelper::getFileName(originalFileName));
                    unparsedFile -> set_sourceFileNameWithPath(originalFileName);

                    ASSERT_not_null(unparsedFile->get_parent());
                    if (SgProject::get_unparseHeaderFilesDebug() >= 7)
                       {
                         printf ("Processing unparseMapEntries: unparseMapEntry->second = %s \n",unparseMapEntry->second.c_str());
                         printf ("Processing unparseMapEntries: unparsedFile->get_parent() = %p = %s \n",unparsedFile->get_parent(),unparsedFile->get_parent()->class_name().c_str());
                       }

                    if (SgProject::get_unparseHeaderFilesDebug() >= 7)
                       {
                         printf ("\n\n");
                         printf ("********************************************************** \n");
                         printf ("Computing the adjusted header file directory for unparsing \n");
                         printf ("********************************************************** \n");
                       }

                 // DQ (11/6/2018): The header file directory can be a subdir of the unparseRootPath if it is specified in
                 // the include directive with a path prefix.  If this is the case then it is recomputed below.
                    string adjusted_header_file_directory = unparseRootPath;

                 // DQ (1/1/2019): Append the filename as a suffix to the userSpecifiedUnparseRootFolder so that we can avoid header file
                 // location collissions when compileing either multiple files or multiple files in parallel.
                 // string filenameWithOutPath = FileHelper::getFileName(originalFileName);
                 // adjusted_header_file_directory += "/" + filenameWithOutPath;
#if 0
                    printf ("Modified adjusted_header_file_directory = %s \n",adjusted_header_file_directory.c_str());
                    printf ("unparsedFile->get_parent() = %p \n",unparsedFile->get_parent());
                    if (unparsedFile->get_parent() != nullptr)
                       {
                         printf ("unparsedFile->get_parent() = %p = %s \n",unparsedFile->get_parent(),unparsedFile->get_parent()->class_name().c_str());
                       }
#endif
                    SgHeaderFileBody* associated_header_file_body = isSgHeaderFileBody(unparsedFile->get_parent());

                 // DQ (4/4/2020): Added header file unparsing feature specific debug level.
                    if (SgProject::get_unparseHeaderFilesDebug() >= 7)
                       {
                         printf ("(associated_header_file_body != NULL) = %s \n",(associated_header_file_body != NULL) ? "true" : "false");
                       }

                    if (associated_header_file_body != nullptr)
                       {
                         SgIncludeDirectiveStatement* associated_include_directive = isSgIncludeDirectiveStatement(associated_header_file_body->get_parent());
                         ASSERT_not_null(associated_include_directive);
                         if (associated_include_directive != nullptr)
                            {
                              string name_used_in_include_directive = associated_include_directive->get_name_used_in_include_directive();

                              if (SgProject::get_unparseHeaderFilesDebug() >= 7)
                                 {
                                   printf ("Processing unparseMapEntries: name_used_in_include_directive = %s \n",name_used_in_include_directive.c_str());
                                 }

                           // DQ (11/11/2018): Need to connect the extraIncludeDirectorySpecifierList to the translation unit,
                           // and not the source file associated with any header.
                              SgIncludeFile* include_file_support = associated_include_directive->get_include_file_heirarchy();
                              ASSERT_not_null(include_file_support);
                              SgSourceFile* translation_unit_source_file = include_file_support->get_source_file_of_translation_unit();

                              ASSERT_not_null(translation_unit_source_file);
                              if (SgProject::get_unparseHeaderFilesDebug() >= 7)
                                 {
                                   printf ("(check directory prefix) translation_unit_source_file: filename = %s \n",translation_unit_source_file->getFileName().c_str());
                                 }

                           // DQ (11/6/2018): Adding support for when the header file has a path prefix.
                              string directoryPathPrefix = Rose::getPathFromFileName(name_used_in_include_directive);

                              if (SgProject::get_unparseHeaderFilesDebug() >= 7)
                                 {
                                   printf ("directoryPathPrefix = %s \n",directoryPathPrefix.c_str());
                                 }

                              string include_filename = Rose::utility_stripPathFromFileName(name_used_in_include_directive);
                              if (directoryPathPrefix != ".")
                                 {
                                   adjusted_header_file_directory = unparseRootPath + "/" + directoryPathPrefix;

                                // DQ (11/6/2018): Build the path.
                                   boost::filesystem::path pathPrefix(adjusted_header_file_directory);
                                   create_directories(pathPrefix);

                                // DQ (11/8/2018): Adding the "-I" prefix required for use on the command line.
                                   string include_line = string("-I") + adjusted_header_file_directory;
                                   translation_unit_source_file->get_extraIncludeDirectorySpecifierBeforeList().push_back(include_line);
                                 }

                              printf ("Exiting as a test! \n");
                              ROSE_ABORT();
                            }
                           else
                            {
                              printf ("Note: associated_include_file == NULL: associated_header_file_body->get_parent() = %p = %s \n",
                                   associated_header_file_body->get_parent(),associated_header_file_body->get_parent()->class_name().c_str());
                            }
                         printf ("Exiting as a test! \n");
                         ROSE_ABORT();
                       }
                      else
                       {
                         if (SgProject::get_unparseHeaderFilesDebug() >= 4)
                            {
                              printf ("Note: associated_include_file == NULL: filename = %s unparsedFile->get_parent() = %p = %s \n",
                                   unparsedFile->getFileName().c_str(),unparsedFile->get_parent(),unparsedFile->get_parent()->class_name().c_str());
                              printf ("Before modification for source file: adjusted_header_file_directory = %s \n",adjusted_header_file_directory.c_str());
                            }

                         ASSERT_not_null(project);
                         string applicationRootDirectory = project->get_applicationRootDirectory();

                         if (SgProject::get_unparseHeaderFilesDebug() >= 4)
                            {
                              printf ("applicationRootDirectory = %s \n",applicationRootDirectory.c_str());
                            }

                         boost::filesystem::path applicationRootDirectoryPath(applicationRootDirectory);
                         boost::filesystem::path currentDirectoryPath(adjusted_header_file_directory);

                         string source_filename = unparsedFile->getFileName();
                         string source_file_directory = Rose::getPathFromFileName(source_filename);

                      // DQ (4/4/2020): Added header file unparsing feature specific debug level.
                         if (SgProject::get_unparseHeaderFilesDebug() >= 4)
                            {
                              printf ("source_filename                = %s \n",source_filename.c_str());
                              printf ("source_file_directory          = %s \n",source_file_directory.c_str());
                              printf ("adjusted_header_file_directory = %s \n",adjusted_header_file_directory.c_str());
                            }

                         boost::filesystem::path source_file_directory_path(source_file_directory);

                      // DQ (4/4/2020): Added header file unparsing feature specific debug level.
                         if (SgProject::get_unparseHeaderFilesDebug() >= 4)
                            {
                              printf ("currentDirectoryPath.generic_string()       = %s \n",currentDirectoryPath.generic_string().c_str());
                              printf ("source_file_directory_path.generic_string() = %s \n",source_file_directory_path.generic_string().c_str());
                            }

                         bool paths_are_equivalent = equivalent(currentDirectoryPath,source_file_directory_path);

                         if (paths_are_equivalent == true)
                            {
                           // Nothing to do here.
                            }
                           else
                            {
                           // Need to modify the path associated with the output filename.

                           // remove substring (represented by applicationRootDirectory) from source_file_directory, and append this to the adjusted_header_file_directory.
                              string source_file_directory_copy = source_file_directory;
                              string::size_type i = source_file_directory_copy.find(applicationRootDirectory);

                              if (i != string::npos)
                                 {
                                   source_file_directory_copy.erase(i, applicationRootDirectory.length());
                                 }
                              string added_directory = source_file_directory_copy;

                           // DQ (2/1/2020): Handle the case of a name specified in the current directory.
                              if (added_directory == ".")
                                 {
                                   added_directory = "";
                                 }

                              adjusted_header_file_directory += added_directory;

                           // We might need to build the added directory.
                              boost::filesystem::path adjusted_header_file_directory_path(adjusted_header_file_directory);
                              create_directories(adjusted_header_file_directory_path);
                              if (added_directory == ".")
                                 {
                                   printf ("Exiting as a test! added_directory = %s \n",added_directory.c_str());
                                   ROSE_ABORT();
                                 }

                              string adjusted_header_file_directory_include_line = string("-I") + adjusted_header_file_directory;
                              unparsedFile->get_extraIncludeDirectorySpecifierBeforeList().push_back(adjusted_header_file_directory_include_line);

                              ASSERT_not_null(project);
                              project->get_extraIncludeDirectorySpecifierBeforeList().push_back(adjusted_header_file_directory_include_line);
                            }
                       }

                    const string& outputFileName = FileHelper::concatenatePaths(adjusted_header_file_directory, unparseMapEntry -> second);

                    FileHelper::ensureParentFolderExists(outputFileName);
                    unparsedFile -> set_unparse_output_filename(outputFileName);

                 // DQ (9/11/2018): Check that this is a header file (and not the original source file).
                    if (unparsedFile->get_isHeaderFile() == true)
                       {
                      // Unparse only included files (the original source file will be unparsed as usual).

                      // DQ (10/29/2018): We can't just unparse the file using the translation unit's global scope
                      // since we would not visit statements from header files that are nested. So we need to either
                      // put the statements from the associated scope were the header file's statements are located
                      // into the global scope, or reference the associated inner scope directly so that it will be
                      // unparsed (directly).  The previous solution was to call the unparser for each statement in
                      // a loop over the statements in the associated scope.  Not clear if that is enough for the
                      // token-based unparsing.

                         const string filename = unparsedFile->getFileName();
                         ROSE_ASSERT(unparseScopesMap.find(filename) != unparseScopesMap.end());

                         map<string, SgScopeStatement*>::const_iterator unparseScopesMapEntry = unparseScopesMap.find(originalFileName);
                         ROSE_ASSERT(unparseScopesMapEntry != unparseScopesMap.end());

                         SgScopeStatement* header_file_associated_scope = unparseScopesMapEntry->second;
                         ASSERT_not_null(header_file_associated_scope);

#if 0
                         unparseFile(unparsedFile, unparseFormatHelp, unparseDelegate, header_file_associated_scope);
#else
                      // This might be a better solution.
                         if (isSgGlobal(header_file_associated_scope) != nullptr)
                            {
#if DEBUG_UNPARSE_INCLUDE_FILES
                              printf ("isSgGlobal(header_file_associated_scope) != NULL: calling unparseFile() \n");
#endif
                              unparseFile(unparsedFile, unparseFormatHelp, unparseDelegate, nullptr);
#if DEBUG_UNPARSE_INCLUDE_FILES
                              printf ("DONE: isSgGlobal(header_file_associated_scope) != NULL: calling unparseFile() \n");
#endif
                            }
                           else
                            {
#if DEBUG_UNPARSE_INCLUDE_FILES
                              printf ("calling unparseFile(): using header_file_associated_scope %p = %s : calling unparseFile() \n",
                                   header_file_associated_scope,header_file_associated_scope->class_name().c_str());
#endif
#if DEBUG_UNPARSE_INCLUDE_FILES
                              printf ("header_file_associated_scope->get_containsTransformation() = %s \n",header_file_associated_scope->get_containsTransformation() ? "true" : "false");
#endif

                              unparseFile(unparsedFile, unparseFormatHelp, unparseDelegate, header_file_associated_scope);

#if DEBUG_UNPARSE_INCLUDE_FILES
                              printf ("DONE: calling unparseFile(): using header_file_associated_scope %p = %s : calling unparseFile() \n",
                                   header_file_associated_scope,header_file_associated_scope->class_name().c_str());
#endif
                            }
#endif
                       }
                  }
                 else
                  {
                 // DQ (5/24/2021): This communicates the predicate that caused this false branch.
                    ROSE_ASSERT(unparsedFile == nullptr);

                 // DQ (5/8/2021): Debug info for case of unparsedFile == NULL.
                    printf ("unparsedFile == NULL: skipping the support for this header file (but we might need copy the file or to fixup the include path so that we can still find it) \n");

                 // We need to at least add the path to the original source file.
                    printf (" --- originalFileName = %s \n",originalFileName.c_str());
                    printf ("NOTE: This case is likely caused by the application directory path not being specified correctly. \n");
                  }

               if (SgProject::get_unparseHeaderFilesDebug() >= 4)
                  {
                    printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                    printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                    printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                    printf ("BOTTOM of loop over unparseMap \n");
                    printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                    printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                    printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
                  }
             }

       // DQ (11/18/2018): For any include file that is unparsed, it can cause a nested include file to be missed if it used a
       // relative path (common).  This step detects child include files that are not unparsed and explicitly provides a path
       // for the original child include file to be found in the backend compile step.  This is less about the unparsing of
       // include files than the testing of the include files that are being generated (but the testing is essential).
          ASSERT_not_null(sourceFile);

          SgIncludeFile* includeFile = sourceFile->get_associated_include_file();
          if (includeFile != nullptr)
             {
               std::set<std::string> added_include_path_set = IncludeFileSupport::headerFilePrefix (includeFile);

               if (SgProject::get_unparseHeaderFilesDebug() >= 4)
                  {
                    printf ("In unparseIncludedFiles(): added_include_path_set.size() = %zu \n",added_include_path_set.size());
                  }

               SgSourceFile* translation_unit_source_file = includeFile->get_source_file_of_translation_unit();
               ASSERT_not_null(translation_unit_source_file);

               if (SgProject::get_unparseHeaderFilesDebug() >= 4)
                  {
                    printf ("(add extra include paths) translation_unit_source_file: filename = %s \n",translation_unit_source_file->getFileName().c_str());
                    printf ("added_include_path_set.size() = %zu \n",added_include_path_set.size());
                  }

               std::set<std::string>::iterator i = added_include_path_set.begin();
               while (i != added_include_path_set.end())
                  {
                    string header_file_directory = *i;
                    string include_line = string("-I") + header_file_directory;

                    if (SgProject::get_unparseHeaderFilesDebug() >= 4)
                       {
                         printf ("   --- include_line = %s \n",include_line.c_str());
                       }

                    string applicationRootDirectory = sourceFile->get_project()->get_applicationRootDirectory();
                    string includeFileName = includeFile->get_filename();
                    size_t location = header_file_directory.find(applicationRootDirectory);
                    bool isSubstring = (location == 0);

                    if (SgProject::get_unparseHeaderFilesDebug() >= 4)
                       {
                         printf ("location = %zu isSubstring = %s \n",location,isSubstring ? "true" : "false");
                       }

                 // Check is this is a path in the application directory (avoid system or compiler include paths).
                    if (isSubstring == true)
                       {
                         translation_unit_source_file->get_extraIncludeDirectorySpecifierBeforeList().push_back(include_line);
                       }
                    i++;
                  }
             }
        }
       else
        {
          printf ("This may be where we need to compute the first and last statements for each scope \n");
          ROSE_ASSERT(file != nullptr);
          SgSourceFile* sourceFile = isSgSourceFile(file);

          if (sourceFile != nullptr && sourceFile->get_unparse_tokens() == true)
             {
               buildFirstAndLastStatementsForIncludeFiles(project);

            // DQ (5/27/2021): We need to debug the collection of first and last statements associated with each scope
            // (actually we just need the last statement for each scope). This only important for the token-based unarpsing.
               buildFirstAndLastStatementsForScopes(project);
             }
        }

     if (SgProject::get_unparseHeaderFilesDebug() >= 4)
        {
          printf ("Leaving unparseIncludedFiles() project = %p \n",project);
        }
   }

void unparseProject ( SgProject* project, UnparseFormatHelp *unparseFormatHelp, UnparseDelegate* unparseDelegate)
   {
     ASSERT_not_null(project);

#define DEBUG_UNPARSE_PROJECT 0

     if (SgProject::get_unparseHeaderFilesDebug() >= 4)
        {
          printf ("In unparseProject(): project = %p \n",project);
        }

  // Put the call to support name qualification here!
#if DEBUG_UNPARSE_PROJECT || 0
     printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
     printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
     printf ("In unparseProject(): calling computeNameQualification() for the whole AST \n");
     printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
     printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! \n");
#endif

#if DEBUG_UNPARSE_PROJECT
     printf ("In unparseProject(): Calling buildFirstAndLastStatementsForScopes(): Testing for robustness ... \n");
#endif

     buildFirstAndLastStatementsForScopes(project);

#if DEBUG_UNPARSE_PROJECT
     printf ("DONE: In unparseProject(): Calling buildFirstAndLastStatementsForScopes(): Testing for robustness ... \n");
#endif

     ASSERT_not_null(project->get_fileList_ptr());

     SgSourceFile* sourceFile = nullptr;
  // DQ (8/7/2018): Call the name qualification support on each file in the project.
     for (size_t i=0; i < project->get_fileList_ptr()->get_listOfFiles().size(); ++i)
        {
       // These are actually separate translation units.
          SgFile* file = project->get_fileList_ptr()->get_listOfFiles()[i];
          ASSERT_not_null(file);

          sourceFile = isSgSourceFile(file);

       // DQ (8/7/2018): We might want to allow mixed collections of binaries and source files.
          if (sourceFile != nullptr)
             {
            // DQ (4/4/2020): Added header file unparsing feature specific debug level.
               if (SgProject::get_unparseHeaderFilesDebug() >= 2)
                  {
                    printf ("In unparseProject(): loop over all files: calling computeNameQualification() for sourceFile = %p = %s \n",sourceFile,sourceFile->getFileName().c_str());
                  }

               Unparser::computeNameQualification(sourceFile);

               if (SgProject::get_unparseHeaderFilesDebug() >= 2)
                  {
                    printf ("DONE: In unparseProject(): loop over all files: calling computeNameQualification() for sourceFile = %p = %s \n",sourceFile,sourceFile->getFileName().c_str());
                  }

               bool traverseHeaderFiles = false;
               if (sourceFile->get_unparseHeaderFiles() == true)
                  {
                    traverseHeaderFiles = true;
                  }

               if (sourceFile->get_unparse_tokens() == true)
                  {
                    buildTokenStreamFrontier(sourceFile,traverseHeaderFiles);
                  }
             }
        }

     if (sourceFile != nullptr && sourceFile->get_unparseHeaderFiles() == true)
        {
          unparseIncludedFiles(project, unparseFormatHelp, unparseDelegate);
        }

     if ( SgProject::get_verbose() >= 1 )
        {
          printf ("In unparseProject(): Unparse the file list first, then the directory list \n");
        }
     if (SgProject::get_unparseHeaderFilesDebug() >= 4)
        {
          printf ("In unparseProject(): Unparse the file list first, then the directory list \n");
        }

     unparseFileList(project->get_fileList_ptr(),unparseFormatHelp,unparseDelegate);

     if (SgProject::get_verbose() >= 1) {
        printf ("In unparseProject(): Unparse the directory list... \n");
     }
     if (SgProject::get_unparseHeaderFilesDebug() >= 4) {
        printf ("In unparseProject(): Unparse the directory list... \n");
     }

     for (int i = 0; i < project->numberOfDirectories(); ++i) {
        if (SgProject::get_verbose() > 0) {
           printf ("Unparse each directory (i = %d) \n",i);
        }
        ASSERT_not_null(project->get_directoryList());
        SgDirectory* directory = project->get_directoryList()->get_listOfDirectories()[i];
        unparseDirectory(directory,unparseFormatHelp,unparseDelegate);
     }

     if (SgProject::get_unparseHeaderFilesDebug() >= 4) {
        printf ("Leaving unparseProject(): project = %p \n",project);
     }
   }


void unparseDirectory(SgDirectory* directory, UnparseFormatHelp* unparseFormatHelp, UnparseDelegate* unparseDelegate)
   {
     int status = 0;
     ASSERT_not_null(directory);

  // Part of the unparcing support for directories is to change the current system directory.
     string directoryName = directory->get_name();
     ROSE_ASSERT(directoryName != "");

     string mkdirCommand = string("mkdir -p ") + directoryName;

  // DQ (1/24/2010): This is a potential security problem!
     if (SgProject::get_verbose() > 0) {
        printf ("WARNING: calling system using mkdirCommand = %s \n",mkdirCommand.c_str());
     }

     status = system (mkdirCommand.c_str());
     ASSERT_require(status == 0);

  // Now change the current working directory to the new directory
     status = chdir(directoryName.c_str());
     ASSERT_require(status == 0);

     unparseFileList(directory->get_fileList(),unparseFormatHelp,unparseDelegate);

     for (int i = 0; i < directory->numberOfDirectories(); ++i)
        {
          ASSERT_not_null(directory->get_directoryList());
          SgDirectory* subdirectory = directory->get_directoryList()->get_listOfDirectories()[i];
          unparseDirectory(subdirectory,unparseFormatHelp,unparseDelegate);
        }

  // DQ (1/24/2010): This is a potential security problem!
     string chdirCommand = "..";

     if ( SgProject::get_verbose() > 0 )
          printf ("WARNING: calling system using chdirCommand = %s \n",chdirCommand.c_str());

  // Now change the current working directory to the new directory
     status = chdir(chdirCommand.c_str());
     ASSERT_require(status == 0);
   }

/* Disable address sanitizer for this function */
// __attribute__((no_sanitize("address")))
void unparseFileList(SgFileList* fileList, UnparseFormatHelp *unparseFormatHelp, UnparseDelegate* unparseDelegate)
   {
     ASSERT_not_null(fileList);

     int status_of_function = 0;

     if (SgProject::get_unparseHeaderFilesDebug() >= 3) {
        printf ("In unparseFileList(): fileList->get_listOfFiles().size() = %zu \n",fileList->get_listOfFiles().size());
     }

     for (SgFile* file : fileList->get_listOfFiles())
        {
          ASSERT_not_null(file);
          if (SgProject::get_unparseHeaderFilesDebug() >= 4) {
               printf ("\n**************************************************** \n");
               printf ("**************************************************** \n");
               printf ("**************************************************** \n");
               printf ("**************************************************** \n");
               printf ("In unparseFileList(): unparse file = %p filename = %s \n",file,file->getFileName().c_str());
          }
          if (SgProject::get_verbose() > 1) {
             printf("Unparsing file = %p = %s \n",file,file->class_name().c_str());
          }

#ifndef _MSC_VER
          if (KEEP_GOING_CAUGHT_BACKEND_UNPARSER_SIGNAL) {
               mlog[WARN] << "Configured to keep going after catching a signal in Unparser::unparseFile()\n";

               if (file != nullptr) {
                  file->set_unparserErrorCode(100);
                  status_of_function = max(100, status_of_function);
               }
               else {
                  mlog[FATAL] << "Unable to keep going due to an unrecoverable internal error\n";
                  exit(1);
               }
          }
#else
          if (false)
             {
             }
#endif
            else
             {
               if (!isSgSourceFile(file) || isSgSourceFile(file) -> get_frontendErrorCode() == 0)
                  {
                    if (SgProject::get_unparseHeaderFilesDebug() >= 3) {
                         printf ("In unparseFileList(): calling unparseFile(): filename = %s \n",file->getFileName().c_str());
                    }
                    unparseFile(file, unparseFormatHelp, unparseDelegate);
                  }
                 else
                  {
                    if (SgProject::get_verbose() > 1)
                       {
                         std::cout
                             << "[WARN] "
                             << "Skipping unparsing of file "
                             << file->getFileName()
                             << std::endl;
                       }
                  }
             }

          if (SgProject::get_unparseHeaderFilesDebug() >= 4) {
               printf ("In unparseFileList(): base of loop \n");
               printf ("**************************************************** \n");
               printf ("**************************************************** \n");
               printf ("**************************************************** \n");
               printf ("**************************************************** \n");
          }
        } // for each file in fileList
   }
