// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "unparser.h"
#include <limits>

#include "OmpAttribute.h" // to support unparsing OpenMP constructs

// DQ (10/29/2013): Adding support for unparsing from the token stream.
#include "tokenStreamMapping.h"

// DQ (11/30/2013): Added more support for token handling.
#include "previousAndNextNode.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;
using namespace Rose;

// DQ (3/24/2016): Adding Robb's message logging mechanism to contrl output debug message from the EDG/ROSE connection code.
using namespace Rose::Diagnostics;

// DQ (3/24/2016): Adding Message logging mechanism.
Sawyer::Message::Facility UnparseLanguageIndependentConstructs::mlog;


#define OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES 0
#define OUTPUT_HIDDEN_LIST_DATA 0
#define OUTPUT_DEBUGGING_INFORMATION 0

#define HIGH_FEDELITY_TOKEN_UNPARSING 1


// DQ (12/5/2014): Adding support to track transitions between unparsing via the AST and unparsing via the Token Stream.
SgStatement* global_lastStatementUnparsed = NULL;

UnparseLanguageIndependentConstructs::unparsed_as_enum_type global_unparsed_as = UnparseLanguageIndependentConstructs::e_unparsed_as_error;

void 
UnparseLanguageIndependentConstructs::initDiagnostics() 
   {
     static bool initialized = false;
     if (!initialized) 
        {
          initialized = true;
          Rose::Diagnostics::initAndRegister(&mlog, "Rose::UnparseLanguageIndependentConstructs");
          mlog.comment("generating source code for language-indepentend constructs");
        }
   }


std::string
UnparseLanguageIndependentConstructs::unparsed_as_kind(unparsed_as_enum_type x)
   {
     std::string s;

     switch (x)
        {
          case e_unparsed_as_error:                  s = "e_unparsed_as_error"; break;
          case e_unparsed_as_AST:                    s = "e_unparsed_as_AST"; break;
          case e_unparsed_as_partial_token_sequence: s = "e_unparsed_as_partial_token_sequence"; break;
          case e_unparsed_as_token_stream:           s = "e_unparsed_as_token_stream"; break;
          case e_unparsed_as_last:                   s = "e_unparsed_as_last"; break;

          default:
             {
               printf ("Error: default reached in switch: x = %d \n",x);
               ROSE_ASSERT(false);
             }
        }

     return s;
   }



// DQ (8/13/2007): This function was implemented by Thomas
std::string
UnparseLanguageIndependentConstructs::resBool(bool val) const
   {
     return val ? "True" : "False" ;
   }

#if 0
// DQ (7/1/2013): This needs to be defined in the header file, else the GNU 4.5 and 4.6 compilers will have undefined references at link time.
// DQ (8/13/2007): This function was implemented by Thomas
template<typename T>
std::string
UnparseLanguageIndependentConstructs::tostring(T t) const
   {
     std::ostringstream myStream; //creates an ostringstream object
     myStream << std::showpoint << t << std::flush; // Distinguish integer and floating-point numbers
     return myStream.str(); //returns the string form of the stringstream object
   }
#endif

// TODO: This code is identical to 'FortranCodeGeneration_locatedNode::curprint'. Factor this!
void
UnparseLanguageIndependentConstructs::curprint (const std::string & str) const
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

     unp->u_sage->curprint(str);
     
#endif  // USE_RICE_FORTRAN_WRAPPING
}

// DQ (8/13/2007): This has been moved to the base class (language independent code)
void
UnparseLanguageIndependentConstructs::markGeneratedFile() const
   {
     unp->u_sage->curprint("\n#define ROSE_GENERATED_CODE\n");

  // DQ (2/23/2014): Added to test modifications of projects to handle ROSE code when compiled with GNU gcc/g++.
     unp->u_sage->curprint("\n#define USE_ROSE\n");
   }

// This has been simplified by Markus Kowarschik. We need to introduce the
// case of statements that have been introduced by transformations.
// bool Unparser::statementFromFile ( SgStatement* stmt, char* sourceFilename )
// bool UnparseLanguageIndependentConstructs::statementFromFile ( SgStatement* stmt, string sourceFilename )
bool
UnparseLanguageIndependentConstructs::statementFromFile ( SgStatement* stmt, string sourceFilename, SgUnparse_Info& info )
   {
  // If the filename of the statement and the input filename are the same then 
  // the return result is true.  IF not, then we have to look to see if there
  // was a "#line xx "filename"" macro that was used (to explain the difference).

     ROSE_ASSERT (stmt != NULL);

     bool statementInFile = false;

#if 0
     printf ("\nIn statementFromFile(): sourceFilename = %s stmt = %p = %s \n",sourceFilename.c_str(),stmt,stmt->class_name().c_str());
#endif

#if 0
     printf ("\n");
     printf ("In statementFromFile(): sourceFilename = %s stmt = %p = %s \n",sourceFilename.c_str(),stmt,stmt->class_name().c_str());
     printf ("   --- stmt = %s \n",SageInterface::get_name(stmt).c_str());
     printf ("   --- stmt->get_file_info()->get_fileIDsToUnparse().size() = %zu \n",stmt->get_file_info()->get_fileIDsToUnparse().size());
#endif

#if 0
  // DQ (11/10/2019): This is too simple of a tests, and while it frequently will work we need to see if the 
  // current file being unparse is actually in the list defined by fileIDsToUnparse.

  // DQ (2/26/2019): Adding support for multiple file to reference defining declaration and still unparse them.
     if (stmt->get_file_info()->get_fileIDsToUnparse().empty() == false)
        {
       // Found case of multiple file handling causing a defining declaration to be used within more than one file.
       // This design permits both files to reference the single definig declaration, while having only one 
       // defining declaration across the multi-file support (this permits global analysis, especially effective 
       // when used with the AST merge mechanism).

       // For the moment we can attemt to test this support by retuning true when we detect the use of this feature.
#if 0
          printf ("In statementFromFile(): stmt->get_file_info()->get_fileIDsToUnparse().empty() == false: return true \n");
#endif
          return true;
        }
#endif

  // FMZ (comment by DQ (11/14/2008)):
  // This is part of the support for module files in Fortran.  Modules seen in the compilation 
  // of a Fortran program cause a "<module name>.rmod" file to be generated. When we unparse 
  // the "*.rmod" we want to output all statements, but since they came from the original 
  // fortran file (a better translation would avoid this problem), the function would conclude
  // that they should not be unparsed (this fix forces the statements in a "*.rmod" file to 
  // always be unparsed.  If the SgSourceFile built to represent the "*.rmod" file had been 
  // constructed as a transformation then the file info objects would have been marked as
  // part of a transforamtion and this fix would not have been required.  At some point this
  // can be improved.  So this is a fine temporary fix for now.
     if (StringUtility::fileNameSuffix(sourceFilename) == "rmod") 
        {
       // If we are to unparse a module into the .rmod file this this is ALWAYS true
          return true;
        }

#if 0
     if (stmt->get_file_info()->isFrontendSpecific() == false)
        {
          int    stmt_line              = stmt->get_file_info()->get_line();
          int    stmt_physical_line     = stmt->get_file_info()->get_physical_line();
          string stmt_filename          = stmt->get_file_info()->get_filenameString();
          string stmt_physical_filename = stmt->get_file_info()->get_physical_filename();

          printf ("In UnparseLanguageIndependentConstructs::statementFromFile(): sourceFilename = %s \n",sourceFilename.c_str());
          printf ("   --- stmt_physical_filename = %s stmt_physical_line = %d \n",stmt_physical_filename.c_str(),stmt_physical_line);
          printf ("   --- stmt = %p = %s stmt_filename  = %s line = %d \n",stmt,stmt->class_name().c_str(),stmt_filename.c_str(),stmt_line);

          if (isSgTemplateInstantiationDecl(stmt) != NULL)
             {
               stmt->get_file_info()->display("case of SgTemplateInstantiationDecl: debug");
             }
        }
#endif

#if 0
  // DQ (2/15/2013): This might not be required now that we support physical filenames (and physical line numbers).

  // DQ (12/23/2012): This special handling of the "conftest.c" file is no linger required.
  // It is used to map filenames generated from a specific #line directives used in autoconf 
  // generated files to the physical filename.  However, now that we internally keep references 
  // to both the logical source position AND the physical source position, this should not 
  // be required.

#error "DEAD CODE!"

  // DQ (9/17/2013): Updated this test to handle C++ versions of autoconf tests.
  // DQ (10/8/2012): We want to allow ROSE to work with autoconf tests.  The nature
  // of these tests are that they have a #line directive "#line 1227 "configure"" 
  // and are in a file called: "conftest.c" and in some cases have a include file 
  // named: "confdef.h".
     string stmt_filename = StringUtility::stripPathFromFileName(sourceFilename);
     if ( (stmt_filename == "conftest.c") || (stmt_filename == "conftest.C") )
        {
          ROSE_ASSERT(stmt->get_file_info() != NULL);
          string statementfilename = stmt->get_file_info()->get_filenameString();
#if 0
          printf ("statementfilename = %s \n",statementfilename.c_str());
#endif
       // Note that the #line directive will cause the statement's source file position 
       // to match that of the filename in the line directive.
          if (statementfilename == "configure")
             {
#if 0
               printf ("In statementFromFile(): Detected an autoconf (configure) generated file used to test aspects of the system as part of an application's build system stmt = %p = %s \n",stmt,stmt->class_name().c_str());
#endif
               return true;
             }
        }
#endif

#if 0
     printf ("In statementFromFile(): unp->opt.get_unparse_includes_opt() = %s \n",unp->opt.get_unparse_includes_opt() ? "true" : "false");
#endif

  // DQ (8/7/2018): I think we can assert this (but check it). Definitely not, there is at least 
  // one test of this feature in the C_tests directory.
  // ROSE_ASSERT(unp->opt.get_unparse_includes_opt() == false);

     if (unp->opt.get_unparse_includes_opt() == true)
        {
#if 0
          printf ("In statementFromFile(): unp->opt.get_unparse_includes_opt() == true: stmt = %p = %s \n",stmt,stmt->class_name().c_str());
#endif
       // If we are to unparse all included files into the source file this this is ALWAYS true
          statementInFile = true;

       // DQ (9/16/2013): Restrict the unparsing using the -rose:unparse_includes option to eliminate the declarations added as part of the front-end support for compatability with the backend.
          SgDeclarationStatement* declarationStatement = isSgDeclarationStatement(stmt);
          if (declarationStatement != NULL && stmt->get_file_info()->isFrontendSpecific() == true)
             {
#if 0
               curprint ( string("\n/* Inside of UnparseLanguageIndependentConstructs::statementFromFile (" ) + StringUtility::numberToString(stmt) + "): class_name() = " + stmt->class_name() + " (skipped) */ \n");
#endif
               statementInFile = false;
             }
        }
       else
        {
       // Compare the file names from the file info object in each statement
       // char* statementfilename = Rose::getFileName(stmt);
       // const char* statementfilename = "default";
          string statementfilename = "default";

       // DQ (8/17/2005): At some point all transformation should be marked as isOutputInCodeGeneration
       // DQ (6/17/2005): Not all compiler generated IR nodes are intended to be unparsed (e.g. implicit casts)
       // some compiler generated IR nodes (e.g. required templates) are required in the generated source code 
       // so check explicitly for compiler generated IR nodes which are marked for output withn the generated 
       // source code (unparser).
       // DQ (5/26/2005): Query isCompilerGenerated and isTransformation before processing the filename.
       // bool isCompilerGenerated = stmt->get_file_info()->isCompilerGenerated();
       // bool isCompilerGenerated = stmt->get_file_info()->isCompilerGeneratedNodeToBeUnparsed();
       // bool isTransformation    = stmt->get_file_info()->isTransformation();
       // if (isCompilerGenerated || isTransformation)
          if (stmt->get_file_info() == NULL)
             {
               printf ("Error: stmt->get_file_info() == NULL stmt = %p = %s \n",stmt,stmt->class_name().c_str());
             }
          ROSE_ASSERT(stmt->get_file_info() != NULL);
          bool isOutputInCodeGeneration = stmt->get_file_info()->isOutputInCodeGeneration();

          SgSourceFile* sourceFile = info.get_current_source_file();

       // DQ (5/19/2011): Output generated code... (allows unparseToString() to be used with template instantations to support name qualification).
          bool forceOutputOfGeneratedCode = info.outputCompilerGeneratedStatements();

       // DQ (10/31/2018): Added assertion.
       // ROSE_ASSERT(sourceFile != NULL);

#if 0
          printf ("In statementFromFile(): isOutputInCodeGeneration = %s \n",isOutputInCodeGeneration ? "true" : "false");
          if (sourceFile != NULL)
             {
               printf ("   --- sourceFile->get_unparse_tokens()          = %s \n",sourceFile->get_unparse_tokens()     ? "true" : "false");
               printf ("   --- sourceFile->get_unparseHeaderFiles()      = %s \n",sourceFile->get_unparseHeaderFiles() ? "true" : "false");
             }
          printf ("   --- forceOutputOfGeneratedCode                = %s \n",forceOutputOfGeneratedCode ? "true" : "false");
#endif
       // DQ (10/25/2018): If we are using the unarsing of header files then we require a more complex test to avoid 
       // transformations in header files being unparsed in the output source (e.g. *.C) file.
       // if (isOutputInCodeGeneration == true && sourceFile->get_unparse_tokens() == true)
       // if (isOutputInCodeGeneration == true && (sourceFile->get_unparse_tokens() == true || sourceFile->get_unparseHeaderFiles() == true))
       // if ((isOutputInCodeGeneration == true || forceOutputOfGeneratedCode == true) && (sourceFile->get_unparse_tokens() == true || sourceFile->get_unparseHeaderFiles() == true))
          if ((isOutputInCodeGeneration == true || forceOutputOfGeneratedCode == true) && (sourceFile != NULL) && (sourceFile->get_unparse_tokens() == true || sourceFile->get_unparseHeaderFiles() == true))
             {
            // We need to evaluate if this is a transformation which should be output, since under the header file unparsing it would 
            // be output in every file using the existing logic.  This is because ROSE was originally designed to have only one file 
            // output and now has been modified to unparse multiple files (the original input source file and posible many header files 
            // as well).

            // The solution will be to match on the sourceFile name and the physical file name associated with the transformation.
            // This will mean that transformations using the header file unparsing will have to explicitly specify the physical file
            // where they eill be unparsed, at least were it can not be determined by surounding statement's associated physical file).
            // Note that the names are interpreted, so we need to use the get_physical_file_id() functions instead.
#if 0
               printf ("isOutputInCodeGeneration == true && sourceFile->get_unparse_tokens() == true: sourceFile->getFileName()                       = %s \n",sourceFile->getFileName().c_str());
               printf ("isOutputInCodeGeneration == true && sourceFile->get_unparse_tokens() == true: stmt-->get_file_info()->get_physical_filename() = %s \n",stmt->get_file_info()->get_physical_filename().c_str());
               printf ("   --- sourceFile->get_file_info()->get_physical_file_id() = %d \n",sourceFile->get_file_info()->get_physical_file_id());
               printf ("   --- stmt->get_file_info()->get_physical_file_id()       = %d \n",stmt->get_file_info()->get_physical_file_id());
#endif

            // DQ (11/1/2018): If we are not unparsing header files then any/every transformation can be output in the default file (input source file).
            // if (sourceFile->getFileName() != stmt->get_file_info()->get_physical_filename())
            // if (sourceFile->get_file_info()->get_physical_file_id() != stmt->get_file_info()->get_physical_file_id())
            // if (sourceFile->get_file_info()->get_physical_file_id() != stmt->get_file_info()->get_physical_file_id())
            // if (sourceFile->get_file_info()->get_physical_file_id() != stmt->get_file_info()->get_physical_file_id() && sourceFile->get_unparseHeaderFiles() == true)
               bool isCompilerGenerated = stmt->get_file_info()->isCompilerGenerated();
#if 0
               printf ("In statementFromFile(): stmt isCompilerGenerated = %s \n",isCompilerGenerated ? "true" : "false");
#endif
               if (sourceFile->get_file_info()->get_physical_file_id() != stmt->get_file_info()->get_physical_file_id() && 
                   sourceFile->get_unparseHeaderFiles() == true && isCompilerGenerated == false)
                  {
                    isOutputInCodeGeneration   = false;

                 // DQ (10/30/2018): Also for this to be false since it was required to trigger the traversal 
                 // of the outer most scope that are referenced by header files.
                    forceOutputOfGeneratedCode = false;
                  }
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }

#if 0
       // DQ (8/7/2018): This is one approach, but I would prefer to set info.outputCompilerGeneratedStatements() in the caller.
          if (isSgGlobal(stmt) != NULL)
             {
               printf ("In statementFromFile(): found a SgGlobal, so force output \n");

               forceOutputOfGeneratedCode = true;
             }
#endif
#if 0
          printf ("In statementFromFile(): stmt = %p = %s isOutputInCodeGeneration   = %s \n",stmt,stmt->class_name().c_str(),isOutputInCodeGeneration   ? "true" : "false");
          printf ("In statementFromFile(): stmt = %p = %s forceOutputOfGeneratedCode = %s \n",stmt,stmt->class_name().c_str(),forceOutputOfGeneratedCode ? "true" : "false");
#endif
#if 0
          info.display("In statementFromFile()");
#endif
       // DQ (1/11/2006): OutputCodeGeneration is not set to be true where transformations 
       // require it.  Transformation to include header files don't set the OutputCodeGeneration flag.
       // if (isOutputInCodeGeneration || isTransformation)
       // if (isOutputInCodeGeneration == true)

       // DQ (5/19/2011): Output generated code is specified.
          if (isOutputInCodeGeneration == true || forceOutputOfGeneratedCode == true)
             {
               statementInFile = true;
             }
            else
             {
            // DQ (8/17/2005): Need to replace this with call to compare Sg_File_Info::file_id 
            // numbers so that we can remove the string comparision operator.
            // statementfilename = Rose::getFileName(stmt);

            // DQ (9/20/2013): We need to use the physical file name in checking which statements to unparse.
            // statementfilename = stmt->get_file_info()->get_filenameString();
               statementfilename = stmt->get_file_info()->get_physical_filename();
#if 0
               printf ("In statementFromFile(): statementfilename = %s \n",statementfilename.c_str());
#endif
               if (info.get_language() == SgFile::e_Fortran_language)
                  {
                 // DQ (9/24/2013): In the case of Fortran we need to generate the preprocessor name (at least for file requireing CPP).
                 // This was handled properly under the previous implementation using the logical source position, so for Fortran we 
                 // use the logical source position as a basis for knowing which statements to be output.  The case of C/C++ is 
                 // more sophisticated (test autoconf test codes) and so it requires the physical source position.  Ideally, the 
                 // fortran support would have the same implementation, but the handling of intermdiate preprocessed files makes 
                 // this more complex (and it should be a seperate fix to handle that).
                 // statementfilename = SgFile::generate_C_preprocessor_intermediate_filename(stmt->get_file_info()->get_physical_filename());
                 // statementfilename = SgFile::generate_C_preprocessor_intermediate_filename(sourceFilename);
                    statementfilename = stmt->get_file_info()->get_filenameString();
#if 0
                    printf ("sourceFilename                                 = %s \n",sourceFilename.c_str());
                    printf ("statementfilename                              = %s \n",statementfilename.c_str());
                    printf ("stmt->get_file_info()->get_physical_filename() = %s \n",stmt->get_file_info()->get_physical_filename().c_str());
                    printf ("stmt->get_file_info()->get_filenameString()    = %s \n",stmt->get_file_info()->get_filenameString().c_str());
#endif
#if 0
                    printf ("In statementFromFile(): Exiting as a test in the Fortran support for source file identification \n");
                    ROSE_ASSERT(false);
#endif
                  }
                 else
                  {
                    statementfilename = stmt->get_file_info()->get_physical_filename();
#if 0
                    printf ("In statementFromFile(): Exiting as a test in the NON-Fortran support for source file identification \n");
                    ROSE_ASSERT(false);
#endif
                  }
#if 0
               printf ("In statementFromFile(): statementfilename = %s sourceFilename = %s \n",statementfilename.c_str(),sourceFilename.c_str());
               printf ("In statementFromFile(): stmt->get_file_info()->get_physical_filename() = %s \n",stmt->get_file_info()->get_physical_filename().c_str());
               printf ("   --- sourceFile->get_file_info()->get_physical_file_id() = %d \n",sourceFile->get_file_info()->get_physical_file_id());
               printf ("   --- stmt->get_file_info()->get_physical_file_id()       = %d \n",stmt->get_file_info()->get_physical_file_id());
#endif
            // DQ (10/22/2007): Allow empty name strings (to support #line n "")
            // ROSE_ASSERT (statementfilename.empty() == false);

            // DQ (12/12/2018): Adding testing for failing test in virtual function analysis.
            // ROSE_ASSERT(sourceFile != NULL);
            // ROSE_ASSERT(sourceFile->get_file_info() != NULL);
               ROSE_ASSERT(stmt != NULL);
               ROSE_ASSERT(stmt->get_file_info() != NULL);

            // DQ (10/31/2018): Because the filenames are interpreted, we have to use the file_id values.
            // DQ (9/20/2013): If this is a performance issue, an optimization would be to use file_id's instead of strings (filenames).
            // However, this does not appear to be an important optimization.
            // if ( statementfilename == sourceFilename )
            // if (sourceFile->get_file_info()->get_physical_file_id() == stmt->get_file_info()->get_physical_file_id())
               if ((sourceFile != NULL) && (sourceFile->get_file_info()->get_physical_file_id() == stmt->get_file_info()->get_physical_file_id()))
                  {
                    statementInFile = true;
                  }

            // DQ (11/10/2019): Details apply when nodes are shared across multiple files.
               if (stmt->get_file_info()->isShared() == true)
                  {
                 // Need to consult the fileIDsToUnparse to see if the current file id is in the list.
                    SgFileIdList & fileIdList = stmt->get_file_info()->get_fileIDsToUnparse();
                    ROSE_ASSERT(fileIdList.size() == stmt->get_file_info()->get_fileLineNumbersToUnparse().size());
#if 0
                    printf ("Output fileIdList (size = %zu): \n",fileIdList.size());
                    for (size_t i = 0; i < fileIdList.size(); i++)
                       {
                         printf ("  fileIdList[%zu] = %d \n",i,fileIdList[i]);
                       }
#endif
                    SgFileIdList::const_iterator pos = find(fileIdList.begin(),fileIdList.end(),sourceFile->get_file_info()->get_physical_file_id());
                    if (pos != fileIdList.end())
                       {
#if 0
                         printf ("In statementFromFile(): current file is sharing this IR node: source file_id = %d stmt physical_file_id = %d \n",
                              sourceFile->get_file_info()->get_physical_file_id(),stmt->get_file_info()->get_physical_file_id());
#endif
                         statementInFile = true;
                       }

#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }

            // negara1 (08/15/2011): Make a special consideration for header file bodies in include directive statements.
            // TODO: Change when SgIncludeDirectiveStatement is used instead of attached PreprocessingInfo.
               SgIncludeDirectiveStatement* includeDirectiveStatement = isSgIncludeDirectiveStatement(stmt);
               if (includeDirectiveStatement != NULL) 
                  {
                 // DQ (3/24/2019): The newest use of this IR nodes does not accomidate the headerFileBody.
                 // if (includeDirectiveStatement->get_headerFileBody()->get_file_info()->get_filenameString() == sourceFilename)
                    if (includeDirectiveStatement->get_headerFileBody() != NULL && includeDirectiveStatement->get_headerFileBody()->get_file_info()->get_filenameString() == sourceFilename)
                       {
                         statementInFile = true;
                       }
                  }
             }
#if 1
            // DQ (1/4/2014): commented out to test with using token based unparsing.

            // DQ (12/22/2014): this is the most general way to supress the output of normalized template declaration member and non-member functions.
            // The alternative approach is implemented in the unparseTemplateDeclarationStatment_support() function.
               SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(stmt);
               if (functionDeclaration != NULL && functionDeclaration->isNormalizedTemplateFunction() == true)
                  {
                 // SgSourceFile* sourcefile = info.get_current_source_file();
#if 0
                    printf ("output of normalized template declaration member and non-member functions: sourcefile = %p \n",sourcefile);
#endif

#if 1
                 // DQ (5/30/2019): If we are using the token unparsing then we need to supress the unparsing of the normalized functions.
                 // See moveDeclarationTool/inputmoveDeclarationToInnermostScope_test2014_26.C for an example of this.
                    if ( (sourceFile != NULL) && (sourceFile->get_unparse_tokens() == true || sourceFile->get_unparseHeaderFiles() == true))
                      {
#if 0
                         printf ("In statementFromFile(): Detected a normalized template declaration: functionDeclaration = %p = %s name = %s \n",
                              functionDeclaration,functionDeclaration->class_name().c_str(),functionDeclaration->get_name().str());
#endif
                         statementInFile = false;
                      }
#endif

#if 0
                 // DQ (5/28/2019): I think we should allow this to be unparsed, and so that any attached CPP directives 
                 // can be ouput, even if within the unparser we don't output the function definition.
                    if (sourcefile == NULL || sourcefile->get_unparse_edg_normalized_method_ROSE_1392() == false) 
                       {
#if 0
                         printf ("In statementFromFile(): Detected a normalized template declaration: functionDeclaration = %p = %s name = %s \n",
                              functionDeclaration,functionDeclaration->class_name().c_str(),functionDeclaration->get_name().str());
#endif
                         statementInFile = false;
                       }
#endif
                  }
#endif
#if 0
          printf ("In statementFromFile (statementInFile = %s output = %s stmt = %p = %s = %s in file = %s sourceFilename = %s ) \n",
               (statementInFile == true) ? "true": "false", (isOutputInCodeGeneration == true) ? "true": "false", stmt, 
               stmt->class_name().c_str(), SageInterface::get_name(stmt).c_str(),statementfilename.c_str(), sourceFilename.c_str());
#endif
#if 0
          stmt->get_file_info()->display("In Unparser::statementFromFile()");
#endif
        }

#if 0
     printf ("Leaving statementFromFile(): stmt = %p = %s = %s statementInFile = %s sourceFilename = %s \n",
          stmt,stmt->class_name().c_str(),SageInterface::get_name(stmt).c_str(),(statementInFile == true) ? "true" : "false",sourceFilename.c_str());
#endif
#if 0
     if (isSgFunctionDeclaration(stmt) != NULL)
        {
          SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(stmt);
          printf (" --- functionDeclaration->get_definition() = %p \n",functionDeclaration->get_definition());
        }
#endif

#if 0
  // DQ (10/15/2019): limit output spew when debugging.
     if (statementInFile == true)
        {
          printf ("Leaving statementFromFile(): stmt = %p = %s = %s statementInFile = %s \n",stmt,stmt->class_name().c_str(),SageInterface::get_name(stmt).c_str(),(statementInFile == true) ? "true" : "false");
        }
       else
        {
          printf ("@@@@@@@@@@@@@@ Leaving statementFromFile(): stmt = %p = %s = %s statementInFile = %s \n",stmt,stmt->class_name().c_str(),SageInterface::get_name(stmt).c_str(),(statementInFile == true) ? "true" : "false");
        }
#endif

#if 0
  // stmt->get_file_info()->display("debug why false");
  // if (statementInFile == false)
        {
          stmt->get_file_info()->display("debug why false");
        }
#endif

#if 0
  // Debugging support
     SgDeclarationStatement* declarationStatement = isSgDeclarationStatement(stmt);
     if (declarationStatement != NULL && statementInFile == false && stmt->get_file_info()->isFrontendSpecific() == false)
        {
          curprint ( string("\n/* Inside of UnparseLanguageIndependentConstructs::statementFromFile (" ) + StringUtility::numberToString(stmt) + "): class_name() = " + stmt->class_name() + " (skipped) */ \n");
        }
#endif

     return statementInFile;
   }


// DQ (9/6/2006): Modified to return "std::string" instead of "char*"
string
UnparseLanguageIndependentConstructs::getFileName(SgNode* stmt)
   {
     string returnString;

  // check to see if fileinfo is not available
     if (stmt->get_file_info() != NULL)
        {
          returnString = stmt->get_file_info()->get_filenameString();
        }
       else
        {
       // the fileinfo was not available, so check the stmt's parent for the fileinfo
          SgNode* p_stmt = stmt->get_parent();
          if (p_stmt != NULL)
               returnString = getFileName(p_stmt);
        }

     return returnString;
   }


// DQ (9/6/2006): Modified to return "std::string" instead of "char*"
string
UnparseLanguageIndependentConstructs::getFileName()
   {
     return currentOutputFileName;
   }


void
UnparseLanguageIndependentConstructs::printOutComments ( SgLocatedNode* locatedNode ) const
   {
  // Debugging function to print out comments in the statements (added by DQ)

     ROSE_ASSERT(locatedNode != NULL);

  // DQ (3/22/2019): Refactored code to SageInterface. Actually, this version needs to unparse 
  // the comments to the output file AND to stdout, while the other version in SageInterface 
  // outputs to stdout and is for debugging.
  // SageInterface::printOutComments(locatedNode);

     AttachedPreprocessingInfoType* comments = locatedNode->getAttachedPreprocessingInfo();

#if 0
     curprint ("/* Inside of printOutComments() */");
#endif

     if (comments != NULL)
        {
#if 0
          printf ("Found attached comments (at %p of type: %s): \n",locatedNode,locatedNode->class_name().c_str());
          curprint ("/* Inside of printOutComments(): comments != NULL */");
#endif

          AttachedPreprocessingInfoType::iterator i;
          for (i = comments->begin(); i != comments->end(); i++)
             {
               ROSE_ASSERT ( (*i) != NULL );
               printf ("          Attached Comment (relativePosition=%s): %s\n",
                    ((*i)->getRelativePosition() == PreprocessingInfo::before) ? "before" : "after",
                    (*i)->getString().c_str());
               printf ("Comment/Directive getNumberOfLines = %d getColumnNumberOfEndOfString = %d \n",(*i)->getNumberOfLines(),(*i)->getColumnNumberOfEndOfString());
               curprint (string("/* Inside of printOutComments(): comments = ") +  (*i)->getString() + " */");

#if 0
               (*i)->get_file_info()->display("comment/directive location");
#endif
             }
        }
       else
        {
          printf ("No attached comments (at %p of type: %s): \n",locatedNode,locatedNode->sage_class_name());
        }
   }


void
UnparseLanguageIndependentConstructs::outputCompilerGeneratedStatements( SgUnparse_Info & info )
   {
  // DQ (3/28/2017): Eliminate warning about unused variable from Clang.
  // list<SgStatement*>::iterator i = unp->compilerGeneratedStatementQueue.begin();

  // printf ("compilerGeneratedStatementQueue.size() = %" PRIuPTR " \n",compilerGeneratedStatementQueue.size());
     while (info.outputCompilerGeneratedStatements() == false && unp->compilerGeneratedStatementQueue.empty() == false)
        {
       // now output the compiler generated statements
          SgStatement* compilerGeneratedStatement = unp->compilerGeneratedStatementQueue.back();

       // printf ("compilerGeneratedStatement = %p = %s \n",compilerGeneratedStatement,compilerGeneratedStatement->sage_class_name());

       // Drain the queue (remove last element)
          unp->compilerGeneratedStatementQueue.pop_back();

       // Generate a new SgUnparse_Info object
          SgUnparse_Info tempInfo(info);
          tempInfo.set_outputCompilerGeneratedStatements();

       // Now output the previously saved statement (recursive call)
       // printf ("Now output the previously saved statement (recursive call) \n");
          unparseStatement(compilerGeneratedStatement,tempInfo);
        }
   }



// void UnparseLanguageIndependentConstructs::unparseStatementNumbers ( SgStatement* stmt )
void
UnparseLanguageIndependentConstructs::unparseStatementNumbers ( SgStatement* stmt, SgUnparse_Info & info )
   {
  // This is the base class (which is called only for C/C++ code generation).

  // This is a Fortran specific case (different from use of SgLabelStatement in C/C++).
  // This is a virtual function and defined in the base class as just a test on the 
  // value range of the in the numeric_label (default value is -1).
  // ROSE_ASSERT(stmt->get_numeric_label() == -1);
     ROSE_ASSERT(stmt->get_numeric_label() == NULL);
   }


void
UnparseLanguageIndependentConstructs::unparseLineDirectives ( SgStatement* stmt )
   {
  // DQ (12/4/2007): This is the control for the output of #line "" directives
  // This used to be present and was removed at some point.
     if (unp->opt.get_linefile_opt() == true)
        {
          static int previousFileId     = 0;
          static int previousLineNumber = 0;
          int currentFileId             = stmt->get_startOfConstruct()->get_file_id();
          int currentLineNumber         = stmt->get_startOfConstruct()->get_line();

#if 0
       // Try not to output a #line directive for every line number (however this may be required for greater precision).
          bool outputdirective = false;
       // if (currentFileId != previousFileId)
          if (currentFileId != previousFileId || currentLineNumber != previousLineNumber)
             {
               previousFileId = currentFileId;
               outputdirective = true;
             }
#else
          bool outputdirective = true;
#endif

       // There are a number of IR nodes that we don't want to have generate #line directives.
          if ( (isSgGlobal(stmt) != NULL) ||
               (isSgFunctionDefinition(stmt) != NULL) ||
               (isSgClassDefinition(stmt) != NULL) ||
               (isSgBasicBlock(stmt) != NULL) )
             {
               outputdirective = false;
             }

          if (outputdirective == true && ( (currentFileId == previousFileId) && (currentLineNumber == previousLineNumber) ))
             {
               outputdirective = false;
             }
            else
             {
               previousFileId     = currentFileId;
               previousLineNumber = currentLineNumber;
             }

          if (outputdirective == true)
             {
               string filename   = stmt->get_startOfConstruct()->get_filenameString();
               string lineNumber = StringUtility::numberToString(stmt->get_startOfConstruct()->get_line());
               string lineDirective = "#line " + lineNumber + " \"" + filename + "\"";
               unp->u_sage->curprint_newline();
               curprint (lineDirective);
               unp->u_sage->curprint_newline();
             }
        }
   }


bool
UnparseLanguageIndependentConstructs::canBeUnparsedFromTokenStream(SgSourceFile* sourceFile, SgStatement* stmt)
   {
  // This function factors out the details of the conditions under which a statement can be unparsed from the token stream.
  // Note that it is conditional upon if there is a mapping identified between the token stream and the statement.  These
  // mapping can be shared across more than one statement, or not exist, depending on the statement and the use of macro 
  // expansion in the statement (or across multiple statements).

  // Note that we might want this function to return a pointer to a TokenStreamSequenceToNodeMapping instead (and NULL if no info is available)

     ROSE_ASSERT(sourceFile != NULL);
     ROSE_ASSERT(stmt != NULL);

     bool canBeUnparsed = false;

     std::map<SgNode*,TokenStreamSequenceToNodeMapping*> & tokenStreamSequenceMap = sourceFile->get_tokenSubsequenceMap();

  // If a set of statements are associated with the same interval of the token stream, then we have to detect this.
  // The first statement will be mapped to the token stream, but then I am less clear on what happens.

     if (tokenStreamSequenceMap.find(stmt) != tokenStreamSequenceMap.end())
        {
          TokenStreamSequenceToNodeMapping* tokenSubsequence = tokenStreamSequenceMap[stmt];
       // ROSE_ASSERT(tokenSubsequence != NULL);
          if (tokenSubsequence != NULL)
             {
#if 0
               printf ("In canBeUnparsedFromTokenStream(): stmt = %p = %s \n",stmt,stmt->class_name().c_str());
               printf ("   --- tokenStreamSequenceMap: leading  (start,end) = (%d,%d) \n",tokenSubsequence->leading_whitespace_start, tokenSubsequence->leading_whitespace_end);
               printf ("   --- tokenStreamSequenceMap: node     (start,end) = (%d,%d) \n",tokenSubsequence->token_subsequence_start,  tokenSubsequence->token_subsequence_end);
               printf ("   --- tokenStreamSequenceMap: trailing (start,end) = (%d,%d) \n",tokenSubsequence->trailing_whitespace_start,tokenSubsequence->trailing_whitespace_end);
#endif
               ROSE_ASSERT(stmt->get_file_info() != NULL);
#if 0
               stmt->get_file_info()->display("In canBeUnparsedFromTokenStream(): debug");
#endif
               canBeUnparsed = (tokenSubsequence->token_subsequence_start != -1);

            // DQ (11/29/2013): Added support for the detection of redundantly mapped statements to token sequences.
            // E.g. ROSE normalizations of variable declaration with multiple variables into seperate (multiple) 
            // SgVariableDeclaration IR nodes in the AST.
               std::multimap<int,SgStatement*> & redundantlyMappedTokensToStatementMultimap = sourceFile->get_redundantlyMappedTokensToStatementMultimap();

               std::set<int> & redundantTokenEndings = sourceFile->get_redundantTokenEndingsSet();

               std::set<int>::iterator k = redundantTokenEndings.begin();
               while (k != redundantTokenEndings.end())
                  {
                    int lastTokenIndex = *k;
#if 0
                    printf ("Redundant statement list: lastTokenIndex = %d \n",lastTokenIndex);
#endif
                    std::pair<std::multimap<int,SgStatement*>::iterator,std::multimap<int,SgStatement*>::iterator> range_iterator = redundantlyMappedTokensToStatementMultimap.equal_range(lastTokenIndex);
                    std::multimap<int,SgStatement*>::iterator first_iterator = range_iterator.first;
                    std::multimap<int,SgStatement*>::iterator last_iterator  = range_iterator.second;

                    std::multimap<int,SgStatement*>::iterator local_iterator = first_iterator;
                    while (local_iterator != last_iterator)
                       {
#if 0
                         SgStatement* stmt = local_iterator->second;
                         printf ("   --- redundant statement for lastTokenIndex = %d stmt = %p = %s \n",lastTokenIndex,stmt,stmt->class_name().c_str());
#endif
                         local_iterator++;
                       }

                    k++;
                  }
             }
        }
       else
        {
#if 0
          printf ("Note: In canBeUnparsedFromTokenStream(): the requested subsequence mapping object was not found: stmt = %p = %s \n",stmt,stmt->class_name().c_str());
#endif
        }

#if 0
     printf ("Leaving canBeUnparsedFromTokenStream(): stmt = %p = %s canBeUnparsed = %s \n",stmt,stmt->class_name().c_str(),canBeUnparsed ? "true" : "false");
#endif

     return canBeUnparsed;
   }


bool
UnparseLanguageIndependentConstructs::redundantStatementMappingToTokenSequence(SgSourceFile* sourceFile, SgStatement* stmt)
   {
     ROSE_ASSERT(sourceFile != NULL);
     ROSE_ASSERT(stmt != NULL);

     static std::set<SgStatement*> previouslySeenStatement;

     bool redundantStatement = false;

#if 0
     printf ("In redundantStatementMappingToTokenSequence(): stmt = %p = %s \n",stmt,stmt->class_name().c_str());
#endif

  // std::set<int>::iterator k = redundantTokenEndings.begin();
  // while (k != redundantTokenEndings.end())

     std::map<SgNode*,TokenStreamSequenceToNodeMapping*> & tokenStreamSequenceMap = sourceFile->get_tokenSubsequenceMap();

  // DQ (1/13/2015): Adding another mechanism to support supression of previously unparsed token subsequences (required to support macros that map to multiple statements).
     static std::set<TokenStreamSequenceToNodeMapping*> previouslyUnparsedTokenSubsequences;

     if (tokenStreamSequenceMap.find(stmt) != tokenStreamSequenceMap.end())
        {
          TokenStreamSequenceToNodeMapping* tokenSubsequence = tokenStreamSequenceMap[stmt];
          int lastTokenIndex = tokenSubsequence->token_subsequence_end;

       // DQ (11/29/2013): Added support for the detection of redundantly mapped statements to token sequences.
       // E.g. ROSE normalizations of variable declaration with multiple variables into seperate (multiple) 
       // SgVariableDeclaration IR nodes in the AST.
          std::multimap<int,SgStatement*> & redundantlyMappedTokensToStatementMultimap = sourceFile->get_redundantlyMappedTokensToStatementMultimap();
          std::set<int> & redundantTokenEndings = sourceFile->get_redundantTokenEndingsSet();
#if 0
          printf ("Redundant statement list: lastTokenIndex = %d \n",lastTokenIndex);
#endif
          if (redundantTokenEndings.find(lastTokenIndex) != redundantTokenEndings.end())
             {
#if 0
               printf ("Found in redundantTokenEndings: lastTokenIndex = %d \n",lastTokenIndex);
#endif
               std::pair<std::multimap<int,SgStatement*>::iterator,std::multimap<int,SgStatement*>::iterator> range_iterator = redundantlyMappedTokensToStatementMultimap.equal_range(lastTokenIndex);
               std::multimap<int,SgStatement*>::iterator first_iterator = range_iterator.first;
               std::multimap<int,SgStatement*>::iterator last_iterator  = range_iterator.second;

               std::multimap<int,SgStatement*>::iterator local_iterator = first_iterator;
#if 1
            // DQ (1/28/2015): Switched logic to report the first statement as not redundant, but all others as redundant.
               if (previouslySeenStatement.find(stmt) == previouslySeenStatement.end())
                  {
                 // This is a previously processed statment.
                    redundantStatement = false;
#if 0
                    printf ("   ---   --- Detected first use of statment = %p = %s \n",stmt,stmt->class_name().c_str());
#endif
                 // Add all of the redundnat statement to the previouslySeenStatement (so that they will all 
                 // trigger redundantStatement = true when used (unparsed) later).
                    while (local_iterator != last_iterator)
                       {
                         SgStatement* redundant_stmt = local_iterator->second;
#if 0
                         printf ("   --- redundant statement for lastTokenIndex = %d redundant_stmt = %p = %s \n",lastTokenIndex,redundant_stmt,redundant_stmt->class_name().c_str());
#endif
                         previouslySeenStatement.insert(redundant_stmt);

                         local_iterator++;
                       }
                  }
                 else
                  {
                 // This is a previously processed statment.
                    redundantStatement = true;
                  }
#else
            // DQ (1/28/2015): This older code reported the last of the redundnat set of statement to be non-redundnat (instead of the first where the CPP directives would be located).
               while (local_iterator != last_iterator)
                  {
                 // DQ (1/28/2015): Switched logic to report the first statement as not redundant, but all others as redundant.
                 // SgStatement* redundant_stmt = local_iterator->second;
                    SgStatement* redundant_stmt = local_iterator->second;
#if 0
                    printf ("   --- redundant statement for lastTokenIndex = %d redundant_stmt = %p = %s \n",lastTokenIndex,redundant_stmt,redundant_stmt->class_name().c_str());
#endif
                    if (previouslySeenStatement.find(stmt) != previouslySeenStatement.end())
                       {
                      // This is a previously processed statment.
                         redundantStatement = true;
#if 0
                         printf ("   ---   --- Detected redundant statment = %p = %s \n",stmt,stmt->class_name().c_str());
#endif
                       }
                      else
                       {
                      // We have not processed this statement, so add it to the static local map.
#if 0
                         printf ("   ---   --- Adding to previouslySeenStatement list: redundant_stmt = %p = %s \n",redundant_stmt,redundant_stmt->class_name().c_str());
#endif
                         previouslySeenStatement.insert(redundant_stmt);
                       }

                    local_iterator++;
                  }
#endif
             }
            else
             {
#if 0
               printf ("Not found in redundantTokenEndings: lastTokenIndex = %d \n",lastTokenIndex);
#endif
            // DQ (1/13/2015): We might need to output the last statement that has a replicated token sequence, and not the first (I think).
               if (previouslyUnparsedTokenSubsequences.find(tokenSubsequence) != previouslyUnparsedTokenSubsequences.end())
                  {
#if 0
                    printf ("Return TRUE from redundantStatementMappingToTokenSequence(): tokenSubsequence = %p stmt = %p = %s \n",tokenSubsequence,stmt,stmt->class_name().c_str());
#endif
                    return true;
                  }
                 else
                  {
#if 0
                    printf ("Record that this TokenStreamSequenceToNodeMapping data has been processed: tokenSubsequence = %p \n",tokenSubsequence);
#endif
                    previouslyUnparsedTokenSubsequences.insert(tokenSubsequence);
                  }
             }
        }
       else
        {
#if 0
          printf ("In redundantStatementMappingToTokenSequence(): not found in tokenStreamSequenceMap: stmt = %p = %s \n",stmt,stmt->class_name().c_str());
#endif
        }

#if 0
     printf ("Leaving redundantStatementMappingToTokenSequence(): stmt = %p = %s redundantStatement = %s \n",stmt,stmt->class_name().c_str(),redundantStatement ? "true" : "false");
#endif

     return redundantStatement;
   }


bool
UnparseLanguageIndependentConstructs::unparseAttachedPreprocessingInfoUsingTokenStream(
   SgLocatedNode* stmt,
   SgUnparse_Info& info,
   PreprocessingInfo::RelativePositionType whereToUnparse)
   {
  // Get atached preprocessing info
     AttachedPreprocessingInfoType *prepInfoPtr = stmt->getAttachedPreprocessingInfo();

  // DQ (1/18/2015): The default should always be to output the tokens from the token stream, unless we detect a transformation or this is a shared token stream.
  // bool unparseUsingTokenStream = false;
     bool unparseUsingTokenStream = true;

#if 0
     printf ("In unparseAttachedPreprocessingInfoUsingTokenStream(): stmt = %p = %s \n",stmt,stmt->class_name().c_str());
#endif

  // If we are skiping BOTH comments and CPP directives then there is nothing to do
     if ( info.SkipComments() && info.SkipCPPDirectives() )
        {
       // There's no preprocessing info attached to the current statement
#if 0
          printf ("In Unparse_ExprStmt::unparseAttachedPreprocessingInfoUsingTokenStream(): Skipping output or comments and CPP directives \n");
#endif
          return false;
        }

#if 0
     printf ("In unparseAttachedPreprocessingInfoUsingTokenStream(): calling printOutComments(stmt): stmt = %p = %s \n",stmt,stmt->class_name().c_str());
     printOutComments(stmt);
#endif

  // DQ (1/17/2015): We need to handle shared token streams and there mappings to statements.
     SgSourceFile* sourceFile = info.get_current_source_file();

  // DQ (1/19/2015): Some new_app files demostrate that we can't assume that sourceFile != NULL.
  // ROSE_ASSERT(sourceFile != NULL);

  // DQ (1/19/2015): Skip this case when info.get_current_source_file() == NULL.
     if (sourceFile != NULL)
        {
          std::map<SgNode*,TokenStreamSequenceToNodeMapping*> & tokenStreamSequenceMap = sourceFile->get_tokenSubsequenceMap();
          if (stmt->get_containsTransformationToSurroundingWhitespace() == false)
             {
               if (tokenStreamSequenceMap.find(stmt) != tokenStreamSequenceMap.end())
                  {
                    TokenStreamSequenceToNodeMapping* tokenSubsequence = tokenStreamSequenceMap[stmt];
                    if (tokenSubsequence != NULL)
                       {
                         if (tokenSubsequence->shared == true)
                            {
                              ROSE_ASSERT(tokenSubsequence->nodeVector.empty() == false);

                              SgStatement* last_shared_statement = isSgStatement(tokenSubsequence->nodeVector[tokenSubsequence->nodeVector.size()-1]);
                              ROSE_ASSERT(last_shared_statement != NULL);
#if 0
                              printf ("tokenSubsequence->nodeVector.size() = %zu \n",tokenSubsequence->nodeVector.size());
                              printf ("   --- stmt = %p = %s \n",stmt,stmt->class_name().c_str());
                              printf ("   --- last_shared_statement = %p = %s \n",last_shared_statement,last_shared_statement->class_name().c_str());
#endif
                              if (last_shared_statement == stmt)
                                 {
#if 0
                                   printf ("Detected a statement associated with a shared token sequence, returing true for last shared statement. \n");
#endif
                                // return true;
                                   unparseUsingTokenStream = true;
                                 }
                                else
                                 {
                                   unparseUsingTokenStream = false;
                                 }
#if 0
                              printf ("Exiting as a test! \n");
                              ROSE_ASSERT(false);
#endif
                            }
                       }
                  }
             }
            else
             {
#if 0
               printf ("NOTE: In Unparse_ExprStmt::unparseAttachedPreprocessingInfoUsingTokenStream(): containsTransformationToSurroundingWhitespace == true \n");
               curprint("/* In UnparseLanguageIndependentConstructs::unparseAttachedPreprocessingInfoUsingTokenStream(): containsTransformationToSurroundingWhitespace == true */");
#endif
            // This is set below.
            // unparseUsingTokenStream = false;
             }
        }
       else
        {
          printf ("NOTE: In Unparse_ExprStmt::unparseAttachedPreprocessingInfoUsingTokenStream(): isolated case where info.get_current_source_file() == NULL \n");
        }

     if (prepInfoPtr != NULL)
        {
#if 0
          info.display("In Unparse_ExprStmt::unparseAttachedPreprocessingInfoUsingTokenStream()");
#endif

       // Traverse the container of PreprocessingInfo objects
          AttachedPreprocessingInfoType::iterator i;
          for (i = prepInfoPtr->begin(); i != prepInfoPtr->end(); ++i)
             {
            // i is a pointer to the current prepInfo object, print current preprocessing info
            // Assert that i points to a valid preprocssingInfo object
               ROSE_ASSERT ((*i) != NULL);
               ROSE_ASSERT ((*i)->getTypeOfDirective()  != PreprocessingInfo::CpreprocessorUnknownDeclaration);
               ROSE_ASSERT ((*i)->getRelativePosition() == PreprocessingInfo::before || 
                            (*i)->getRelativePosition() == PreprocessingInfo::after  ||
                            (*i)->getRelativePosition() == PreprocessingInfo::inside);
#if 0
            // DQ (1/28/2013): Fixed to use output of PreprocessingInfo::relativePositionName() and thus provide more accurate debug information.
               printf ("In Unparse_ExprStmt::unparseAttachedPreprocessingInfoUsingTokenStream(): Stored comment: (*i)->getRelativePosition() = %s (*i)->getString() = %s \n",
                    PreprocessingInfo::relativePositionName((*i)->getRelativePosition()).c_str(),
                    (*i)->getString().c_str());
#endif
               if ((*i)->getRelativePosition() == whereToUnparse)
                  {
#if 0
                    printf ("In UnparseLanguageIndependentConstructs::unparseAttachedPreprocessingInfoUsingTokenStream(): return true \n");
#endif
#if 0
                    curprint("/* In UnparseLanguageIndependentConstructs::unparseAttachedPreprocessingInfoUsingTokenStream(): return true */");
#endif
                    unparseUsingTokenStream = true;
                  }
             }
        }

  // DQ (1/15/2015): Added support for token-based unparsing (transformations on the comments and CPP directives on a 
  // statement will triger a mode to unparse the comments and CPP directives from the AST and not from the token stream.
     if (stmt->get_containsTransformationToSurroundingWhitespace() == true)
        {
          unparseUsingTokenStream = false;
#if 0
          printf ("In UnparseLanguageIndependentConstructs::unparseAttachedPreprocessingInfoUsingTokenStream(): return false \n");
#endif
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

     return unparseUsingTokenStream;
   }

int
UnparseLanguageIndependentConstructs::unparseStatementFromTokenStreamForNodeContainingTransformation(
   SgSourceFile* sourceFile, SgStatement* stmt, SgUnparse_Info & info, bool & lastStatementOfGlobalScopeUnparsedUsingTokenStream, unparsed_as_enum_type unparsed_as)
   {
  // This function returns non-zero value if the input statement can be wholely unparsed using the token stream (not partially and not from the AST).

  // TODO: it would be better to add this specific logic to the canBeUnparsedFromTokenStream() function and eliminate this function.
  // TODO: Also this function is not clearly named under its revised semantics (doe not actaully unparse any tokens).

     ROSE_ASSERT(sourceFile != NULL);
     ROSE_ASSERT(stmt != NULL);

#if 0
     printf ("In unparseStatementFromTokenStreamForNodeContainingTransformation(): stmt = %p = %s \n",stmt,stmt->class_name().c_str());
#endif

  // Return status 1 means that this failed (so unparse from the AST).
     int returnStatus = 1;

     if ( SgProject::get_verbose() > 0 )
        {
          string s = "/* Unparse a partial token sequence: contains transformation: stmt = " + stmt->class_name() + " */ ";
          curprint (s);
        }

     std::map<SgNode*,TokenStreamSequenceToNodeMapping*> & tokenStreamSequenceMap = sourceFile->get_tokenSubsequenceMap();

  // DQ (3/28/2017): Eliminate warning about unused variable from Clang.
  // SgTokenPtrList & tokenVector = sourceFile->get_token_list();

  // This implementation uses the refactored code.
     bool unparseStatus = (canBeUnparsedFromTokenStream(sourceFile,stmt) == true);

#if 0
     printf ("In unparseStatementFromTokenStreamForNodeContainingTransformation(): canBeUnparsedFromTokenStream(): unparseStatus = %s \n",unparseStatus ? "true" : "false");
#endif

  // if (canBeUnparsedFromTokenStream(sourceFile,stmt) == true)
     if (unparseStatus == true)
        {
       // Check if this is a previously processed statement (static map is located in redundantStatementMappingToTokenSequence() function.
          bool redundantStatement = redundantStatementMappingToTokenSequence(sourceFile,stmt);

          if (redundantStatement == false)
             {
               TokenStreamSequenceToNodeMapping* statement_tokenSubsequence = tokenStreamSequenceMap[stmt];
               ROSE_ASSERT(statement_tokenSubsequence != NULL);

               if (statement_tokenSubsequence != NULL)
                  {
                 // Return status 0 means that this worked (will cause partial unparsing via token stream to be set in SgUnparse_Info).
                    returnStatus = 0;
                  }
                 else
                  {
                 // Return status 1 means that this failed (so unparse from the AST).
                    returnStatus = 1;
                  }
             }
        }

#if 0
     printf ("Leaving unparseStatementFromTokenStreamForNodeContainingTransformation(): stmt = %p = %s returnStatus = %d \n",stmt,stmt->class_name().c_str(),returnStatus);
#endif

     return returnStatus;
   }


int
UnparseLanguageIndependentConstructs::unparseStatementFromTokenStream(SgSourceFile* sourceFile, SgStatement* stmt, SgUnparse_Info & info, bool & lastStatementOfGlobalScopeUnparsedUsingTokenStream)
   {
  // DQ (11/13/2015): Note that this function name is shared with one defined (overloaded) in the unparseCxx_Statements.C file.

     ROSE_ASSERT(sourceFile != NULL);
     ROSE_ASSERT(stmt != NULL);

#define OUTPUT_TOKEN_STREAM_FOR_DEBUGGING 0

  // DQ (11/12/2014): turn this off to test test2014_101.c (which demonstrates an error, but for which this fixes the error).
  // DQ (1/29/2014): Control use of format mechanism to unparse the token stream vs. a higher fedelity 
  // mechanism that does not drop line endings.  The high fidelity version is just prettier, but 
  // pretty counts...!

     std::map<SgNode*,TokenStreamSequenceToNodeMapping*> & tokenStreamSequenceMap = sourceFile->get_tokenSubsequenceMap();

     SgTokenPtrList & tokenVector = sourceFile->get_token_list();

#if 0
     printf ("In unparseStatementFromTokenStream(): sourceFile = %s \n",sourceFile->getFileName().c_str());
#endif

  // This implementation uses the refactored code.
     bool unparseStatus = (canBeUnparsedFromTokenStream(sourceFile,stmt) == true);

#if 0
     printf ("In unparseStatementFromTokenStream(): stmt = %p = %s unparseStatus = %s \n",stmt,stmt->class_name().c_str(),unparseStatus ? "true" : "false");
#endif

#if 0
     if (unparseStatus == false)
        {
          printf ("In unparseStatementFromTokenStream(): unparseStatus == false: stmt = %p = %s \n",stmt,stmt->class_name().c_str());
          stmt->get_file_info()->display("unparseStatus == false");
        }
#endif

  // if (canBeUnparsedFromTokenStream(sourceFile,stmt) == true)
     if (unparseStatus == true)
        {
       // Check if this is a previously processed statement (static map is located in redundantStatementMappingToTokenSequence() function.
          bool redundantStatement = redundantStatementMappingToTokenSequence(sourceFile,stmt);

       // DQ (9/24/2018): These are always going to be redundant, since these are inserted after the token stream mapping, 
       // and redundantly represent the same tokens in the token stream.
          if (isSgIncludeDirectiveStatement(stmt) != NULL)
             {
#if 0
               printf ("In unparseStatementFromTokenStream(): Detected a SgIncludeDirectiveStatement: unparseStatus = %s \n",unparseStatus ? "true" : "false");
#endif
               return (unparseStatus == true) ? 0 : 1;
             }

#if 0
          printf ("In unparseStatementFromTokenStream(): stmt = %p = %s redundantStatement = %s \n",stmt,stmt->class_name().c_str(),redundantStatement ? "true" : "false");
#endif
#if 0
          curprint( string("/* In unparseStatementFromTokenStream(): redundantStatement = ") + (redundantStatement ? "true" : "false") + " */");
#endif
       // DQ (11/13/2015): Comment added: redundant statements are generated when multiple 
       // statements in the AST are mapped to a single token sequence for a single statement 
       // in the token stream.  This happens for normalizations, generally, but really only
       // in the case of a variable declaration with multiple variables which in the AST is
       // currently normalized to multiple variable declaration statements, but in the token
       // stream is the original variable declaration with multiple variables.  I have 
       // worked out the fix in the front-end to support mulple SgInitializedName objects in
       // a single SgVariableDeclStatement, but we have not taken the step to eliminate this
       // normalization because it would likely break code where people have grown dependent
       // on it.  So this de-normalization will have to be added as an option in ROSE so that
       // we can permit the evaluation of existing code, in a staged release of a future version
       // which will eliminate this normalization.  For now we record the variable declarations
       // that appear after the first one as redundant in the token-based unparsing.
          if (redundantStatement == false)
             {
            // Check for the leading token stream for this statement.  Unparse it if the previous statement was unparsed as a token stream.
               std::map<SgNode*,PreviousAndNextNodeData*> & previousAndNextFrontierDataMap = sourceFile->get_token_unparse_frontier_adjacency();

               TokenStreamSequenceToNodeMapping* tokenSubsequence = tokenStreamSequenceMap[stmt];
               ROSE_ASSERT(tokenSubsequence != NULL);
#if 0
               printf ("In unparseStatementFromTokenStream(): tokenSubsequence = %p (%d,%d) \n",tokenSubsequence,tokenSubsequence->token_subsequence_start,tokenSubsequence->token_subsequence_end);
#endif
               ROSE_ASSERT(tokenSubsequence->token_subsequence_start != -1);

            // Sometimes the previousAndNextFrontierDataMap is not defined for a stmt.
            // ROSE_ASSERT(previousAndNextFrontierDataMap.find(stmt) != previousAndNextFrontierDataMap.end());
               bool unparseStatus_previousStatement = false;
               bool unparseLeadingTokenStream       = false;
#if 0
               printf ("In unparseStatementFromTokenStream(): previousAndNextFrontierDataMap.find(stmt) != previousAndNextFrontierDataMap.end() = %s \n",previousAndNextFrontierDataMap.find(stmt) != previousAndNextFrontierDataMap.end() ? "true" : "false");
#endif
               if (previousAndNextFrontierDataMap.find(stmt) != previousAndNextFrontierDataMap.end())
                  {
                    PreviousAndNextNodeData* previousAndNextFrontierData = previousAndNextFrontierDataMap[stmt];
                    ROSE_ASSERT(previousAndNextFrontierData != NULL);
                    ROSE_ASSERT(previousAndNextFrontierData->previous != NULL);
                    SgStatement* previousStatement = isSgStatement(previousAndNextFrontierData->previous);
                    ROSE_ASSERT(previousStatement != NULL);

                 // This fails in the case where the whole AST is unparsed from the token stream.
                 // ROSE_ASSERT(previousStatement != stmt);

                 // DQ (12/1/2013): Not clear if this is helpful or not (but it communicates in the 
                 // unparsed code what statements were unparse using either the AST or the token stream).
                    if ( SgProject::get_verbose() > 0 )
                       {
                         string s = "/* Unparsing from the token stream stmt = " + stmt->class_name() + " */ ";
                         curprint (s);
                       }

                 // bool unparseStatus_previousStatement = (canBeUnparsedFromTokenStream(sourceFile,previousStatement) == true);
                 // bool unparseLeadingTokenStream = unparseAttachedPreprocessingInfoUsingTokenStream(stmt,info,PreprocessingInfo::before);
                    unparseStatus_previousStatement = (canBeUnparsedFromTokenStream(sourceFile,previousStatement) == true);

                 // DQ (1/15/2015): We should maybe call the unparseAttachedPreprocessingInfoUsingTokenStream() function so that we can determin if
                 // there are added comments or CPP directives (as a result of transformations) and so that we can know to unparse them NOT using the token stream.
                 // DQ (12/23/2014): I think this should be true when we unparse from the token stream (partial or fully), but not when we unparse from the AST.
                 // unparseLeadingTokenStream = unparseAttachedPreprocessingInfoUsingTokenStream(stmt,info,PreprocessingInfo::before);
                 // bool unused_unparseLeadingTokenStream = unparseAttachedPreprocessingInfoUsingTokenStream(stmt,info,PreprocessingInfo::before);
                    unparseLeadingTokenStream = true;
#if 0
                    printf ("In unparseStatementFromTokenStream(): unparseStatus_previousStatement = %s \n",unparseStatus_previousStatement ? "true" : "false");
                    printf ("In unparseStatementFromTokenStream(): unparseLeadingTokenStream = %s \n",unparseLeadingTokenStream ? "true" : "false");
#endif
                  }
                 else
                  {
#if 0
                    printf ("In unparseStatementFromTokenStream(): stmt not in previousAndNextFrontierDataMap: unparseLeadingTokenStream = %s \n",unparseLeadingTokenStream ? "true" : "false");
                    printf ("   --- set unparseLeadingTokenStream = true \n");
#endif
#if 0
                    if ( SgProject::get_verbose() > 0 )
                       {
                         string s = "/* Unparse a partial token sequence (stmt not found in previousAndNextFrontierDataMap: setting unparseLeadingTokenStream = true): stmt = " + stmt->class_name() + " */ ";
                         curprint (s);
                       }
#endif
                    unparseLeadingTokenStream = true;
                  }
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
#if 0
               printf ("In unparseStatementFromTokenStream(): stmt = %p = %s unparseStatus_previousStatement = %s unparseLeadingTokenStream = %s \n",
                    stmt,stmt->class_name().c_str(),unparseStatus_previousStatement ? "true" : "false",unparseLeadingTokenStream ? "true" : "false");
#endif
            // if (unparseStatus_previousStatement == true)
               if (unparseStatus_previousStatement == true || unparseLeadingTokenStream == true)
                  {
#if 0
                    printf ("Output the leading tokens for this statement = %p = %s \n",stmt,stmt->class_name().c_str());
#endif
#if 0
                    printf ("   --- tokenSubsequence->leading_whitespace_start = %d tokenSubsequence->leading_whitespace_end = %d \n",tokenSubsequence->leading_whitespace_start,tokenSubsequence->leading_whitespace_end);
#endif
                    SgGlobal* globalScope = isSgGlobal(stmt);
#if 0
                    printf ("In unparseStatementFromTokenStream(): globalScope = %p \n",globalScope);
                    if (globalScope != NULL)
                       {
                         printf ("globalScope->get_declarations().size() = %" PRIuPTR " \n",globalScope->get_declarations().size());
                         for (size_t i = 0; i < globalScope->get_declarations().size(); i++)
                            {
                              SgDeclarationStatement* decl = globalScope->get_declarations()[i];
                              ROSE_ASSERT(decl != NULL);
                              printf ("   --- global scope statements: i = %p = %s \n",decl,decl->class_name().c_str());
                            }
                       }
#endif

                 // DQ (1/7/2015): I think that we can't process the SgGlobal using this function.
                 // ROSE_ASSERT(globalScope == NULL);

                    if (globalScope != NULL)
                       {
                      // DQ (1/7/2015): I think this must be true if the SgGlobal is called using this function.
                         ROSE_ASSERT(globalScope->get_containsTransformation() == false);

#if 0
                         printf ("Processing corner case of empty global scope unparsed using the token stream \n");
#endif
#if 0
                         if (globalScope->get_declarations().empty() == true)
                            {
                           // If this is the empty global scope then consider the SgGlobal to be the last statement to be unparsed.
                           // This will trigger the output of the remaining tokens in the file and suppress any CPP directives and 
                           // comments that are attached to the SgGlobal (because there was no other IR node to attach them to which
                           // could be considered a part of the input file).
                              lastStatementOfGlobalScopeUnparsedUsingTokenStream = true;
                            }
                           else
                            {
                           // Else we have to make sure that none of the declarations in global scope are from this file.
                           // These can de declarations from header files or the rose_edg_required_macros_and_functions.h 
                           // file that is read to support GNU predefined macros.
                              lastStatementOfGlobalScopeUnparsedUsingTokenStream = true;
                              for (size_t i = 0; i < globalScope->get_declarations().size(); i++)
                                 {
                                   SgDeclarationStatement* decl = globalScope->get_declarations()[i];
                                   ROSE_ASSERT(decl != NULL);
#if OUTPUT_TOKEN_STREAM_FOR_DEBUGGING
                                   printf ("   --- global scope statements: i = %p = %s \n",decl,decl->class_name().c_str());
#endif
                                   if (statementFromFile(decl, getFileName(), info) == true)
                                      {
#if 0
                                        printf ("Found statement that will be output: set lastStatementOfGlobalScopeUnparsedUsingTokenStream == true: decl = %p = %s \n",decl,decl->class_name().c_str());
#endif
                                        lastStatementOfGlobalScopeUnparsedUsingTokenStream = false;
                                      }
                                 }

                              if (lastStatementOfGlobalScopeUnparsedUsingTokenStream == true)
                                 {
                                // This SgGlobal will be treated as the last statement so that we can know to supporess the generation 
                                // of CPP directives and comments that are in the AST and associated with the SgGlobal.
#if 0
                                   printf ("Global scope being treated as last statement in file (will be unparsed using the token sequence and CPP directives associated with SgGlobal will be supressed) \n");
#endif
                                 }
                            }
#else
#if 0
                         printf ("@@@@@@@@@@ I don't know why this is a special case: globalScope != NULL @@@@@@@@@@@ \n");
#endif
                      // This likely needs to be set to avoid redunent output of CPP directives at the end of a file.
                         lastStatementOfGlobalScopeUnparsedUsingTokenStream = true;
#endif
                       }

                 // DQ (11/13/2015): We want to unparse the leading tokens for a statement if there is an associated comment 
                 // or CPP directive, but even if NOT we want to unparse the associated whitespace.  This handles only the 
                 // case where there is an associated comment or CPP directive.
#if 0
                 // DQ (11/13/2015): Original version of code.
                    bool unparseLeadingTokenStream = unparseAttachedPreprocessingInfoUsingTokenStream(stmt,info,PreprocessingInfo::before);
#else
                 // DQ (11/13/2015): I think this is the better code to use.
                    bool unparseLeadingTokenStream = true;
#endif

#if 0
                    printf ("In UnparseLanguageIndependentConstructs::unparseStatementFromTokenStream(): stmt = %p = %s unparseLeadingTokenStream = %s \n",stmt,stmt->class_name().c_str(),unparseLeadingTokenStream ? "true" : "false");
                    curprint(string("\n/* In UnparseLanguageIndependentConstructs::unparseStatementFromTokenStream(SgSourceFile*,,,): unparseLeadingTokenStream = ") + (unparseLeadingTokenStream ? "true" : "false") + " */");
#endif
                    if (unparseLeadingTokenStream == true)
                       {
                         if (tokenSubsequence->leading_whitespace_start != -1 && tokenSubsequence->leading_whitespace_end != -1)
                            {
                              for (int j = tokenSubsequence->leading_whitespace_start; j <= tokenSubsequence->leading_whitespace_end; j++)
                                 {
#if OUTPUT_TOKEN_STREAM_FOR_DEBUGGING
                                   printf ("Output leading whitespace tokenVector[j=%d]->get_lexeme_string() = %s \n",j,tokenVector[j]->get_lexeme_string().c_str());
#endif
#if HIGH_FEDELITY_TOKEN_UNPARSING
                                // DQ (1/29/2014): Implementing better fedility in the unparsing of tokens (avoid line ending interpretations 
                                // in curprint() function. Note that "unp->get_output_stream().output_stream()" is of type: "std::ostream*" type.
                                // *(unp->get_output_stream().output_stream()) << tokenVector[j]->get_lexeme_string();
                                // unp->get_output_stream() << tokenVector[j]->get_lexeme_string();
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
#if 1
                         printf ("Unparse the leading whitespace from the AST because it's comments and/or CPP directives have been modified: stmt = %p = %s \n",stmt,stmt->class_name().c_str());
#endif
                         unparseAttachedPreprocessingInfo(stmt,info,PreprocessingInfo::before);
                       }
                  }
#if 0
               printf ("In unparseStatementFromTokenStream(): DONE with leading whitespace: stmt = %p = %s \n",stmt,stmt->class_name().c_str());
               curprint(string("\n/* In UnparseLanguageIndependentConstructs::unparseStatementFromTokenStream(SgSourceFile*,,,): DONE with leading whitespace: stmt = ") + stmt->class_name().c_str() + " */");
#endif
               for (int j = tokenSubsequence->token_subsequence_start; j <= tokenSubsequence->token_subsequence_end; j++)
                  {
#if OUTPUT_TOKEN_STREAM_FOR_DEBUGGING
                    printf ("Output tokenVector[j=%d]->get_lexeme_string() = %s \n",j,tokenVector[j]->get_lexeme_string().c_str());
#endif
#if HIGH_FEDELITY_TOKEN_UNPARSING
                 // DQ (1/29/2014): Implementing better fedility in the unparsing of tokens (avoid line ending interpretations 
                 // in curprint() function. Note that "unp->get_output_stream().output_stream()" is of type: "std::ostream*" type.
                 // *(unp->get_output_stream().output_stream()) << tokenVector[j]->get_lexeme_string();
                 // unp->get_output_stream() << tokenVector[j]->get_lexeme_string();
                    *(unp->get_output_stream().output_stream()) << tokenVector[j]->get_lexeme_string();
#else
                 // Note that this will interprete line endings which is not going to provide the precise token based output.
                    curprint(tokenVector[j]->get_lexeme_string());
#endif
                  }
#if 0
               printf ("In unparseStatementFromTokenStream(): DONE with token output: stmt = %p = %s \n",stmt,stmt->class_name().c_str());
               curprint(string("\n/* In UnparseLanguageIndependentConstructs::unparseStatementFromTokenStream(SgSourceFile*,,,): DONE with token output: stmt = ") + stmt->class_name().c_str() + " */");
#endif
#if 1
            // DQ (1/6/2014): The code here is used to close off the global scope when the token stream unparsing is used, 
            // else the global scope will be closed off by the code in the unparseGlobalScope function.

            // DQ (1/29/2014): The only consequence that I can see in not closing off the trailing token stream is that we 
            // will (at least sometimes) not output a trailing CR after the last line.
            // DQ (12/1/2013): I am not clear if there are cases where we need to output the associated trailing tokens.
            // None of these cases appear to be an issue in the C regression tests.

               bool isLastStatementOfScope = false;

            // DQ (1/7/2015): We want the parent instead of the scope, because this is a structural issue.
            // SgScopeStatement* scope = stmt->get_scope();
               SgScopeStatement* scope = isSgScopeStatement(stmt->get_parent());

            // Note that the parent of the global scope is not a scope, so we handle this as a special case.
               SgGlobal* globalScope = isSgGlobal(stmt);
               if (scope == NULL && globalScope == NULL)
                  {
                    printf ("Error: parent of stmt = %p = %s is not a scope \n",stmt,stmt->class_name().c_str());
                  }

            // DQ (6/10/2015): This is overly conservative and does not permit stmt to be a SgFunctionDefinition (see C++ test2015_26.C).
            // This assertion was fine for C, but not for C++, not exactly clear why.
               ROSE_ASSERT(scope != NULL || globalScope != NULL);

            // SgGlobal* globalScope = isSgGlobal(scope);
#if 0
            // if (globalScope != NULL)
               if (scope != NULL)
                  {
                    printf ("scope = %p = %s scope->get_containsTransformation() = %s \n",scope,scope->class_name().c_str(),scope->get_containsTransformation() ? "true" : "false");
                  }
#endif
            // ROSE_ASSERT(globalScope == NULL);

            // DQ (1/6/2014): Get the last statement in the global scope that has valid token information.
            // if (globalScope != NULL && globalScope->get_containsTransformation() == true)
               if (scope != NULL && scope->get_containsTransformation() == true)
                  {
#if 1
                    SgStatement* lastStatement = SageInterface::lastStatementOfScopeWithTokenInfo (scope, tokenStreamSequenceMap);
#if 0
                    printf ("computed lastStatement of scope = %p = %s stmt = %p = %s \n",lastStatement,lastStatement->class_name().c_str(),stmt,stmt->class_name().c_str());
#endif
                    isLastStatementOfScope = (stmt == lastStatement);
#else
                 // Check if this is the last statement in the file (global scope).
                 // SgDeclarationStatementPtrList & declarationList = globalScope->get_declarations();
                    SgStatementPtrList statementList = scope->generateStatementList();
                 // SgDeclarationStatement* lastDeclaration = NULL;
                    SgStatement* lastStatement = NULL;

#error "DEAD CODE!"

                 // if (declarationList.rbegin() != declarationList.rend())
                    if (statementList.rbegin() != statementList.rend())
                       {
                      // Find the last statement with token stream information.
                         int counter = 0;
                      // SgDeclarationStatementPtrList::reverse_iterator i = declarationList.rbegin();
                         SgStatementPtrList::reverse_iterator i = statementList.rbegin();
#if 0
                      // printf ("i != declarationList.rend()                                     = %s \n",i != declarationList.rend() ? "true" : "false");
                         printf ("i != statementList.rend()                                     = %s \n",i != statementList.rend() ? "true" : "false");
                         printf ("tokenStreamSequenceMap.find(*i) == tokenStreamSequenceMap.end() = %s \n",tokenStreamSequenceMap.find(*i) == tokenStreamSequenceMap.end() ? "true" : "false");
                         if (tokenStreamSequenceMap.find(*i) != tokenStreamSequenceMap.end())
                            {
                              printf ("tokenStreamSequenceMap[*i=%p=%s] = %p \n",*i,(*i)->class_name().c_str(),tokenStreamSequenceMap[*i]);
                            }
                         printf ("tokenStreamSequenceMap.find(*i) == tokenStreamSequenceMap.end() || tokenStreamSequenceMap[*i] == NULL = %s \n",
                              tokenStreamSequenceMap.find(*i) == tokenStreamSequenceMap.end() || tokenStreamSequenceMap[*i] == NULL ? "true" : "false");
                         if (tokenStreamSequenceMap.find(*i) != tokenStreamSequenceMap.end())
                            {
                              TokenStreamSequenceToNodeMapping* tmp_tokenSubsequence = tokenStreamSequenceMap[*i];
                           // ROSE_ASSERT(tmp_tokenSubsequence != NULL);
                              if (tmp_tokenSubsequence != NULL)
                                 {
                                   tmp_tokenSubsequence->display("tmp_tokenSubsequence");
                                 }
                            }
#endif
#if 0
                         printf ("BEFORE LOOP: SgDeclarationStatementPtrList::reverse_iterator i = %p = %s \n",*i,(*i)->class_name().c_str());
#endif
                      // while (i != declarationList.rend() && tokenStreamSequenceMap.find(*i) == tokenStreamSequenceMap.end())
                      // while (i != declarationList.rend() && (tokenStreamSequenceMap.find(*i) == tokenStreamSequenceMap.end() || tokenStreamSequenceMap[*i] == NULL) )
                         while (i != statementList.rend() && (tokenStreamSequenceMap.find(*i) == tokenStreamSequenceMap.end() || tokenStreamSequenceMap[*i] == NULL) )
                            {
#if 0
                              printf ("IN LOOP: SgDeclarationStatementPtrList::reverse_iterator i = %p = %s \n",*i,(*i)->class_name().c_str());
#endif
                              i++;

                              counter++;
                            }
#if 0
                         printf ("AFTER LOOP: SgDeclarationStatementPtrList::reverse_iterator i = %p = %s \n",*i,(*i)->class_name().c_str());
                         printf ("Number of declarations without token information at the bottom of the global scope: counter = %d \n",counter);
#endif
                      // ROSE_ASSERT(i != declarationList.rend());
                         ROSE_ASSERT(i != statementList.rend());
                         ROSE_ASSERT(tokenStreamSequenceMap.find(*i) != tokenStreamSequenceMap.end());
                      // lastDeclaration = *i;
                         lastStatement = *i;
#if 0
                      // printf ("lastDeclaration = %p = %s stmt = %p = %s \n",lastDeclaration,lastDeclaration->class_name().c_str(),stmt,stmt->class_name().c_str());
                         printf ("computed lastStatement of scope = %p = %s stmt = %p = %s \n",lastStatement,lastStatement->class_name().c_str(),stmt,stmt->class_name().c_str());
#endif
                      // if (stmt == lastDeclaration)
                         if (stmt == lastStatement)
                            {
#if 0
                              printf ("In unparseStatementFromTokenStream(): identified last statement: stmt = %p = %s \n",stmt,stmt->class_name().c_str());
#endif
                              isLastStatementOfScope = true;
                            }
                       }
                      else
                       {
#if 0
                         printf ("In unparseStatementFromTokenStream(): scope is empty! \n");
#endif
                       }
#endif
                 // ROSE_ASSERT(globalScope == NULL);
                  }
                 else
                  {
#if 0
                    printf ("This stmt = %p = %s does not have a scope (or scope->get_containsTransformation() == false)  \n",stmt,stmt->class_name().c_str());
                    if (scope != NULL)
                       {
                         printf ("   --- scope->get_containsTransformation() = %s \n",scope->get_containsTransformation() ? "true" : "false");
                       }
#endif
                  }
#if 0
               curprint("/* In UnparseLanguageIndependentConstructs::unparseStatementFromTokenStream(SgSourceFile*,,,): calling unparseAttachedPreprocessingInfoUsingTokenStream test 0 */");
#endif
            // DQ (3/28/2017): Eliminate warning about unused variable from Clang.
            // DQ (1/15/2014): This value is not used in the logic below.
            // bool unparseTrailingTokenStream = unparseAttachedPreprocessingInfoUsingTokenStream(stmt,info,PreprocessingInfo::after);
#if 0
               curprint("/* DONE: In UnparseLanguageIndependentConstructs::unparseStatementFromTokenStream(SgSourceFile*,,,): calling unparseAttachedPreprocessingInfoUsingTokenStream test 0 */");
#endif
#if 0
               printf ("In unparseStatementFromTokenStream(): isLastStatementOfScope     = %s \n",isLastStatementOfScope ? "true" : "false");
               printf ("In unparseStatementFromTokenStream(): unparseTrailingTokenStream = %s \n",unparseTrailingTokenStream ? "true" : "false");
#endif
            // The last statement has to handle the output of the tokens for the rest of the file.
            // If the last statement is output from the AST, then it will be handled using information 
            // in the AST (not the token stream).
               if (isLastStatementOfScope == true)
                  {
#if 0
                    printf ("In unparseStatementFromTokenStream(): Process the tokens associated with the trailing edge of the last statement \n");
#endif
                 // Set the return parameter to skip the unparsing of the tailing CPP directives and 
                 // comments from the AST (since they are being output via the token stream).
                    lastStatementOfGlobalScopeUnparsedUsingTokenStream = true;

                    if (tokenSubsequence->trailing_whitespace_start != -1 && tokenSubsequence->trailing_whitespace_end != -1)
                       {
                         for (int j = tokenSubsequence->trailing_whitespace_start; j <= tokenSubsequence->trailing_whitespace_end; j++)
                            {
#if OUTPUT_TOKEN_STREAM_FOR_DEBUGGING
                              printf ("Output trailing whitespace tokenVector[j=%d]->get_lexeme_string() = %s \n",j,tokenVector[j]->get_lexeme_string().c_str());
#endif
#if HIGH_FEDELITY_TOKEN_UNPARSING
                           // DQ (1/29/2014): Implementing better fedility in the unparsing of tokens (avoid line ending interpretations 
                           // in curprint() function. Note that "unp->get_output_stream().output_stream()" is of type: "std::ostream*" type.
                           // *(unp->get_output_stream().output_stream()) << tokenVector[j]->get_lexeme_string();
                           // unp->get_output_stream() << tokenVector[j]->get_lexeme_string();
                              *(unp->get_output_stream().output_stream()) << tokenVector[j]->get_lexeme_string();
#else
                           // Note that this will interprete line endings which is not going to provide the precise token based output.
                              curprint(tokenVector[j]->get_lexeme_string());
#endif
                            }
                       }
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }
#else

#error "DEAD CODE!"
               printf ("Supress the output of the trailing tokens of the last statement in the global scope \n");
               curprint(" /* Supress the output of the trailing tokens of the last statement in the global scope */");
#endif
             }
        }

#if HIGH_FEDELITY_TOKEN_UNPARSING
  // If we are directly operating on the ostream, then flush after each statement.
  // unp->get_output_stream().output_stream()->flush();
  // unp->get_output_stream().flush();
     unp->get_output_stream().output_stream()->flush();
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

  // Test this function here to be true.
  // ROSE_ASSERT(canBeUnparsedFromTokenStream(sourceFile,stmt) == true);

#if 0
     printf ("Leaving unparseStatementFromTokenStream(): lastStatementOfGlobalScopeUnparsedUsingTokenStream = %s \n",lastStatementOfGlobalScopeUnparsedUsingTokenStream ? "true" : "false");
     curprint("/* Leaving unparseStatementFromTokenStream() */");
#endif

  // Return zero to indicate that there was no error in the unparsing from the token stream.
  // return 0;
     return (unparseStatus == true) ? 0 : 1;
   }


#if 0
bool
UnparseLanguageIndependentConstructs::isTransitionFromTokenUnparsingToASTunparsing(SgStatement* statement)
   {
  // This function helps support the token based unparsing. In transitions bewteen 
  // the token unparsing and the AST unparsing, we have to suppress the output of 
  // CPP directives and comments to avoid them being output redundanrtly (as part 
  // of the token stream).

     printf ("In UnparseLanguageIndependentConstructs::isTransitionFromTokenUnparsingToASTunparsing(): statement = %p = %s \n",statement,statement->class_name().c_str());

     bool returnValue = false;

     SgFile* cur_file = SageInterface::getEnclosingFileNode(stmt);

     if (cur_file != NULL && cur_file->get_unparse_tokens() == true)
        {
          SgSourceFile* sourceFile = isSgSourceFile(cur_file);
          ROSE_ASSERT(sourceFile != NULL);

          bool unparse = unparseStatementFromTokenStream(sourceFile, statement);

        }

     return returnValue;
   }
#endif

//-----------------------------------------------------------------------------------
//  void Unparse_ExprStmt::unparseStatement
//
//  General function that gets called when unparsing a statement. Then it routes
//  to the appropriate function to unparse each kind of statement.
//-----------------------------------------------------------------------------------
void
UnparseLanguageIndependentConstructs::unparseStatement(SgStatement* stmt, SgUnparse_Info & info)
   {
     ROSE_ASSERT(stmt != NULL);

#if 0
  // DQ (10/30/2013): Debugging support for file info data for each IR node (added comment only)
     int line    = stmt->get_startOfConstruct()->get_raw_line();
     string file = stmt->get_startOfConstruct()->get_filenameString();
     printf ("\nIn unparseStatement(): (language independent = %s) statement (%p): %s line = %d file = %s \n",languageName().c_str(),stmt,stmt->class_name().c_str(),line,file.c_str());
#endif

#if 0
     printf ("In unparseStatement(): info.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
     printf ("In unparseStatement(): info.SkipEnumDefinition()  = %s \n",(info.SkipEnumDefinition() == true) ? "true" : "false");
#endif

#if 0
  // DQ (10/4/2018): Force agressive testing implemented within get_tokenSubsequenceMap() access function.
     SgSourceFile* sourceFile = isSgSourceFile(SageInterface::getEnclosingFileNode(stmt));
     ROSE_ASSERT(sourceFile != NULL);
     printf ("In unparseStatement(): sourceFile filename (from getEnclosingFileNode()) = %s \n",sourceFile->getFileName().c_str());
     printf ("In unparseStatement(): sourceFile->get_tokenSubsequenceMap().size()      = %zu \n",sourceFile->get_tokenSubsequenceMap().size());
#endif

#if 0
     info.display("In unparseStatement()");
#endif

  // DQ (9/9/2016): These should have been setup to be the same.
     ROSE_ASSERT(info.SkipClassDefinition() == info.SkipEnumDefinition());

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES
  // DQ (10/30/2013): Debugging support for file info data for each IR node (added comment only)
     printf ("Unparse statement (%p): %s name = %s \n",stmt,stmt->class_name().c_str(),SageInterface::get_name(stmt).c_str());

  // DQ (4/17/2007): Added enforcement for endOfConstruct().
     ROSE_ASSERT (stmt->get_endOfConstruct() != NULL);
#endif

#if 0
  // DQ (10/30/2013): Debugging support for file info data for each IR node (added comment only)
     curprint ( string("\n/* Unparse statement (" ) + StringUtility::numberToString(stmt) 
         + "): class_name() = " + stmt->class_name() 
                + " raw line (start) = " + tostring(stmt->get_startOfConstruct()->get_raw_line()) 
                + " raw line (end) = " + tostring(stmt->get_endOfConstruct()->get_raw_line()) 
         + " */ \n");
     char buffer[100];
     snprintf (buffer,100,"%p",stmt);
     curprint ("\n/* Top of unparseStatement() " + stmt->class_name() + " at: " + buffer + " */ \n");
#endif

#if 0
  // DQ (10/25/2006): Debugging support for file info data for each IR node
     if (stmt->get_endOfConstruct() == NULL)
        {
          printf ("Error in unparseStatement(): stmt = %p = %s stmt->get_endOfConstruct() == NULL \n",stmt,stmt->class_name().c_str());
          stmt->get_file_info()->display("unparseStatement (debug)");
        }
  // ROSE_ASSERT(stmt->get_endOfConstruct() != NULL);

     curprint ( string("\n/* Top of unparseStatement(): (UnparseLanguageIndependentConstructs)" ) + string(stmt->sage_class_name()) + " */\n ");
     ROSE_ASSERT(stmt->get_startOfConstruct() != NULL);
  // ROSE_ASSERT(stmt->getAttachedPreprocessingInfo() != NULL);
     int numberOfComments = -1;
     if (stmt->getAttachedPreprocessingInfo() != NULL)
        {
          numberOfComments = stmt->getAttachedPreprocessingInfo()->size();
        }

     curprint ( string("/* startOfConstruct: file = " ) + stmt->get_startOfConstruct()->get_filenameString()
         + " raw filename = " + stmt->get_startOfConstruct()->get_raw_filename()
         + " raw line = "     + StringUtility::numberToString(stmt->get_startOfConstruct()->get_raw_line())
         + " raw column = "   + StringUtility::numberToString(stmt->get_startOfConstruct()->get_raw_col())
         + " #comments = "    + StringUtility::numberToString(numberOfComments)
         + " */\n ");

     if (stmt->get_endOfConstruct() != NULL)
        {
          curprint ( string("/* endOfConstruct: file = " ) + stmt->get_endOfConstruct()->get_filenameString()
              + " raw filename = " + stmt->get_endOfConstruct()->get_raw_filename() 
              + " raw line = "     + StringUtility::numberToString(stmt->get_endOfConstruct()->get_raw_line())
              + " raw column = "   + StringUtility::numberToString(stmt->get_endOfConstruct()->get_raw_col())
              + " */\n ");
        }
       else
        {
          curprint ( string("/* endOfConstruct == NULL */\n " ) );
        }
     
  // ROSE_ASSERT(stmt->get_endOfConstruct() != NULL);

     SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(stmt);
     if (variableDeclaration != NULL)
        {
          SgInitializedNamePtrList & nameList = variableDeclaration->get_variables();
          SgInitializedNamePtrList::iterator i = nameList.begin();
          while(i != nameList.end())
             {
               curprint ( string("\n/* SgInitializedName = " ) + (*i)->get_name()  + " in file: " 
                   + (*i)->get_file_info()->get_raw_filename() + " at line: "
                   + StringUtility::numberToString((*i)->get_file_info()->get_raw_line()) + " at column: "
                   + StringUtility::numberToString((*i)->get_file_info()->get_raw_col())  + " */\n ");
               i++;
             }
        }
#endif

#if 0
  // Debugging support
     printOutComments (stmt);
#endif

     ROSE_ASSERT(stmt->get_file_info() != NULL);

  // FIXME cause conflict in "make check"?
  // DQ (5/19/2011): Allow unparsing of even compiler generated statements when specified via the SgUnparse_Info object.
  // FMZ : we have ".rmod" file which will not satisfy this condition
  // JJW (6/23/2008): Move check for statement-within-file here rather than in individual procedures
  // if (!statementFromFile(stmt, getFileName()))
  // if (!statementFromFile(stmt, getFileName(), info))
     if (statementFromFile(stmt, getFileName(), info) == false)
        {
#if 0
          printf ("WARNING: Skipping calls to output statements that are not recorded as being in the target file: stmt = %p = %s \n",stmt,stmt->class_name().c_str());
          printf ("   --- getFileName() = %s \n",getFileName().c_str());
#endif
#if 0
          stmt->get_file_info()->display("WARNING: Skipping calls to output statements that are not recorded as being in the target file: debug");
#endif

       // If this is not a statement to be unparsed then exit imediately.
          return;
        }

  // saveCompilerGeneratedStatements(stmt,info);
  // DQ (5/27/2005): fixup ordering of comments and any compiler generated code
     if ( info.outputCompilerGeneratedStatements() == false && stmt->get_file_info()->isCompilerGenerated() == true && isSgGlobal(stmt->get_parent()) != NULL )
        {
       // DQ (10/30/2013): This code is executed for C++ code (e.g. for test2004_58.C -- template support).

#if 0
          printf ("fixup ordering of comments and any compiler generated code: returning after push onto queue \n");
#endif

       // push all compiler generated nodes onto the static stack and unparse them after comments and directives 
       // of the next statement are output but before the associated statement to which they are attached.

       // printf ("Save the compiler-generated statement (%s), putting it onto the queue \n",stmt->class_name().c_str());
          unp->compilerGeneratedStatementQueue.push_front(stmt);

       // This return prevents this code from being trivially separated out into function.
          return;
        }


     if ( unparseLineReplacement(stmt,info) )
        {
       // DQ (10/30/2013): Not clear why we want a return here...
#if 0
          printf ("In unparseLineReplacement() case \n");
#endif
          return;
        }

  // curprint("/* Calling unparseAttachedPreprocessingInfo */ \n ");

  // Markus Kowarschik: This is the new code to unparse directives before the current statement
  // AS(05/20/09): LineReplacement should replace a statement with a line. Override unparsing of subtree.
  // unparseAttachedPreprocessingInfo(stmt, info, PreprocessingInfo::before);

  // DQ (10/20/2012): Note that function definitions need to be processed as a special case (unparsing CCP directived handled directly).
  //    1) UnparseLanguageIndependentConstructs::unparseStatement() (with SgFunctionDeclaration)
  //    2) unparseLanguageSpecificStatement() (with SgFunctionDeclaration)
  //    3) unparseFuncDeclStmt() (with SgFunctionDeclaration)
  //    4) unparse CPP directives on: funcdecl_stmt->get_parameterList()
  //    5) Calling UnparseLanguageIndependentConstructs::unparseStatement()
  //    6) unparseFuncDefnStmt()
  //    7) unparse CPP directives on: funcdecl_stmt->get_parameterList()
  //    8) calling unparseFuncDeclStmt
  //    9) calling unparse for funcdefn_stmt->get_body()
  //   10) then trailing comments and CPP directives are output on the body, the function definition, and the function declaration (in that order).
  // bool skipOutputOfPreprocessingInfo = (isSgFunctionDefinition(stmt) != NULL);
  // bool skipOutputOfPreprocessingInfo = (isSgFunctionDefinition(stmt) != NULL) || (isSgTypedefDeclaration(stmt) != NULL);
     bool skipOutputOfPreprocessingInfo = (isSgFunctionDefinition(stmt) != NULL);

#if 0
     printf ("In unparseStatement(): skipOutputOfPreprocessingInfo = %s \n",skipOutputOfPreprocessingInfo ? "true" : "false");
     printf ("   --- stmt = %p = %s \n",stmt,stmt->class_name().c_str());
#endif

     if (skipOutputOfPreprocessingInfo == false)
        {
#if 0
          printf ("In unparseStatement(): Output the comments and CCP directives for the SgStatement stmt = %p = %s (before) \n",stmt,stmt->class_name().c_str());
#endif
#if 0
          bool transitionFromTokenUnparsingToASTunparsing = isTransitionFromTokenUnparsingToASTunparsing(stmt);
          if (transitionFromTokenUnparsingToASTunparsing == true)
             {
               curprint ("/* transitionFromTokenUnparsingToASTunparsing == true */ ");
             }
            else
             {
               curprint ("/* transitionFromTokenUnparsingToASTunparsing == false */ ");
             }
#endif
#if 0
       // DQ (11/30/2013): Move this to below where we can better support the token unparsing.
          unparseAttachedPreprocessingInfo(stmt, info, PreprocessingInfo::before);
#endif
#if 0
          printf ("DONE: In unparseStatement(): Output the comments and CCP directives for the SgStatement stmt = %p = %s (before) \n",stmt,stmt->class_name().c_str());
#endif
        }
       else
        {
#if 0
          printf ("In unparseStatement(): skipping output of comments and CCP directives for SgStatement stmt = %p = %s (before) \n",stmt,stmt->class_name().c_str());
#endif
        }

  // This is the other part of the accumulation of the compiler-generated statements into compilerGeneratedStatementQueue
  // so that they can be unparsed after any CPP directives or comments are unparsed (above). Because there is a return
  // in the code to accumulate the compiler-generated statements the code (above) could not be refactored as nicely.
     outputCompilerGeneratedStatements(info);

  // DQ (5/27/2005): fixup ordering of comments and any compiler generated code
  // ROSE_ASSERT(line_to_unparse == 0);

  // DQ (8/19/2007): Please let's get rid of this, it seems that it has been added back in after an intial 
  // attempt to remove it.  See me if you feel your really need this mechanism.
  // ROSE_ASSERT(unp->ltu == 0);

  // DQ (10/25/2006): Debugging support for file info data for each IR node
#define OUTPUT_EMBEDDED_COLOR_CODES_FOR_STATEMENTS 0
#if OUTPUT_EMBEDDED_COLOR_CODES_FOR_STATEMENTS
     vector< pair<bool,std::string> > stateVector;
     if (get_embedColorCodesInGeneratedCode() > 0)
        {
          setupColorCodes ( stateVector );
          printColorCodes ( stmt, true, stateVector );
        }
#endif

  // DQ (12/26/2007): Moved from language independent handling to C/C++ specific handling 
  // becasue we don't want it to appear in the Fortran code generation.
  // DQ (added comments) this is where the new lines are introduced before statements.
  // unp->cur.format(stmt, info, FORMAT_BEFORE_STMT);
  // curprint("/* After FORMAT_BEFORE_STMT */ \n ");

  // This is the added code to support the copy based unparsing mechanism.
  // Since there is a return here, it might be that comments after the 
  // statement will not be unparsed properly (check this at some point).
     if (unp->repl != NULL)
        {
       // printf ("Unparser Delegate found! \n");
          if (unp->repl->unparse_statement(stmt,info, unp->cur))
             {
#if 0
               printf ("Delegate unparser returned true for repl->unparse_statement(%p) \n",stmt);
#endif
               return;
             }
        }

  // DQ (1/30/204): We need this to permit knowing when to unparse the trialing CPP directives and 
  // comments from the AST.  If they were unparsed from the token steam (as part of unparsing the 
  // last statement from the token stream) then unparsing them from the AST would be redundant
  // (though likely harmless).
     bool lastStatementOfGlobalScopeUnparsedUsingTokenStream = false;

  // DQ (10/30/2013): We can support the output of the statements using the token stream, it this is done then we don't output the statement as unparsed from the AST.
     bool outputStatementAsTokens = false;

  // DQ (12/5/2014): For statements that contain transformations, we need to uparse the leading and 
  // trailing parts of the statement from the token stream so that the diff is as small as possible.
  // The records where unparsing the leading and trailing parts (or middle part in the case of a SgIfStmt)
  // was sucessful.  In this case the unparsing of the AST should be skipped for these leading and 
  // trailing parts of the statement.
     bool outputPartialStatementAsTokens = false;

  // DQ (12/5/2014): Adding support to track transition between token stream unparsing, partial token stream unparsing, and AST unparsing.
     global_lastStatementUnparsed = stmt;

  // DQ (12/5/2014): Adding support to track transitions between unparsing using 
  // tokens sequences, partial tokens sequences, and directly from the AST.
     unparsed_as_enum_type global_previous_unparsed_as = global_unparsed_as;

  // DQ (7/20/2008): This mechanism is now extended to SgStatement and revised to handle 
  // more cases than just replacement of the 
  // AST subtree with a string.  Now we can add arbitrary text into different locations
  // relative to the specific IR node.  For now we are supporting before, replace, and after.
     AstUnparseAttribute* unparseAttribute = dynamic_cast<AstUnparseAttribute*>(stmt->getAttribute(AstUnparseAttribute::markerName));
     if (unparseAttribute != NULL)
        {
       // Note that in most cases unparseLanguageSpecificStatement() will be called, some formatting 
       // via "unp->cur.format(stmt, info, FORMAT_BEFORE_STMT);" may be done.  This can cause extra 
       // CRs to be inserted (which only looks bad).  Not clear now to best clean this up.
          string code = unparseAttribute->toString(AstUnparseAttribute::e_before);
          curprint (code);
        }

  // Only replace the unparsing of the IR node with a string if a string is marked as AstUnparseAttribute::e_replace.
     if (unparseAttribute != NULL && unparseAttribute->replacementStringExists() == true)
        {
          string code = unparseAttribute->toString(AstUnparseAttribute::e_replace);
          curprint (code);
        }
       else
        {
       // Use a static varible to track the previous statement.
       // static SgStatement* previousStatement = NULL;

       // DQ (10/30/2013): We can support the output of the statements using the token stream, it this is done then we don't output the statement as unparsed from the AST.
       // bool outputStatementAsTokens = false;

       // DQ (12/5/2014): For statements that contain transformations, we need to uparse the leading and 
       // trailing parts of the statement from the token stream so that the diff is as small as possible.
       // The records where unparsing the leading and trailing parts (or middle part in the case of a SgIfStmt)
       // was sucessful.  In this case the unparsing of the AST should be skipped for these leading and 
       // trailing parts of the statement.
       // bool outputPartialStatementAsTokens = false;

       // DQ (10/24/2018): This is a bug fix specific to supporting the header file unparsing using the token streams.
       // Namely we need to compute the file from the information in the Sg_Unparse_Info object instead of from the statement.
       // This is because the statement chain of parents will alway lead to the input file translation unit, instead of to the 
       // additional SgSourceFile represented by the header file.  This is because the statements in the global scope are
       // owned by the translation unit (parent pointers lead to that SgGlobal scope) but shared by the SgGlobal that is
       // introduced as part of the support for header files (that is itroduced in the AST only when header file unparsing 
       // is turned on).  When header file unparsing is turned off, then the translaton unit and the current source file 
       // will be the same.

       // Get the file and check if -rose:unparse_tokens was used then we want to try to access the token stream and output this statement directly as tokens.
       // SgFile* cur_file = SageInterface::getEnclosingFileNode(stmt);
          SgFile* cur_file = info.get_current_source_file();
#if 0
          if (cur_file == NULL)
             {
               printf ("Warning: In UnparseLanguageIndependentConstructs::unparseStatement(): cur_file == NULL: from info.get_current_source_file() \n");
             }
#endif
       // ROSE_ASSERT(cur_file != NULL);

       // DQ (1/18/2015): Output a message when this is not true (note: sometimes info.get_current_source_file() == NULL).
#if 0
          if (cur_file != info.get_current_source_file())
             {
                printf ("Warning: SageInterface::getEnclosingFileNode(stmt) != info.get_current_source_file() \n");
                printf ("   --- stmt = %p = %s \n",stmt,stmt->class_name().c_str());
                printf ("   --- cur_file = %p = %s \n",cur_file,cur_file != NULL ? cur_file->getFileName().c_str() : "null");
                SgSourceFile* sourceFile = info.get_current_source_file();
                printf ("   --- info.get_current_source_file() = %p = %s \n",sourceFile,sourceFile != NULL ? sourceFile->getFileName().c_str() : "null");
             }
#endif
       // ROSE_ASSERT(cur_file == info.get_current_source_file());
#if 0
          printf ("In UnparseLanguageIndependentConstructs::unparseStatement(): cur_file = %p = %s \n",cur_file,cur_file->class_name().c_str());
          printf ("In UnparseLanguageIndependentConstructs::unparseStatement(): cur_file->get_unparse_tokens() = %s \n",cur_file->get_unparse_tokens() ? "true" : "false");
#endif

#if 0
          printf ("In UnparseLanguageIndependentConstructs::unparseStatement(): cur_file->getFileName() = %s \n",cur_file->getFileName().c_str());
          printf ("In UnparseLanguageIndependentConstructs::unparseStatement(): info.get_current_source_file()->getFileName() = %s \n",info.get_current_source_file()->getFileName().c_str());
          printf ("In UnparseLanguageIndependentConstructs::unparseStatement(): info.get_current_source_file()->get_unparse_tokens() = %s \n",info.get_current_source_file()->get_unparse_tokens() ? "true" : "false");
#endif

       // DQ (10/31/2018): Allowing info.get_current_source_file() to be NULL (e.g. for types output from unparseToString()).
       // DQ (10/24/2018): Added assertion to test consistancy.
       // ROSE_ASSERT(info.get_current_source_file()->get_unparse_tokens() == cur_file->get_unparse_tokens());
          ROSE_ASSERT(cur_file == NULL || info.get_current_source_file()->get_unparse_tokens() == cur_file->get_unparse_tokens());

       // DQ (10/30/2013): This command-line option controls the use of the token stream in the unparsing.
       // Currently in it's development, we are always unparsing the statements using the token stream if 
       // they qualify.  Later we need to connect a test that will detect if a transformation has been done 
       // in the subtree rerpresented by a statement and only qualify the statement on the basis of this 
       // additional test.
       // Note that loopProcessing tests use a generated statement which are not processed for tokens and 
       // in this case the (cur_file == NULL).
          if (cur_file != NULL && cur_file->get_unparse_tokens() == true)
             {
            // First we want to restrict this to unparsing the simplest statements, e.g. those 
            // that are expression statements (e.g. containing no nested statements).

               SgSourceFile* sourceFile = isSgSourceFile(cur_file);
               ROSE_ASSERT(sourceFile != NULL);
#if 0
               curprint ("/* case of cur_file->get_unparse_tokens() == true */");
#endif
            // This will be connected to a test to check if the statement has been transformed (might be 
            // precomputed in a single traversal with results propogated to statements).  Assume no transformations 
            // in early stags of testing (note command-line option -rose:is also required).
               bool statementTransformed = false;

            // We will over time increase the number of types of statements allowed to be unparsed via the token stream.
            // bool unparseViaTokenStream = (isSgExprStatement(stmt) != NULL) && (info.inConditional() == false);

            // Check if this is a frontier node and unparse it using the token stream (we actually need to check that is not marked to be parsed from the AST).
            // vector<FrontierNode*> & frontier_nodes = sourceFile->get_token_unparse_frontier();
            // bool isFrontierNode = (find(frontier_nodes.begin(),frontier_nodes.end(),stmt) != frontier_nodes.end());
               std::map<SgStatement*,FrontierNode*> & frontier_nodes = sourceFile->get_token_unparse_frontier();
#if 0
               printf ("frontier_nodes.size() = %zu \n",frontier_nodes.size());
#endif
               std::map<SgStatement*,FrontierNode*>::iterator i = frontier_nodes.find(stmt);
               bool isFrontierNode = (i != frontier_nodes.end());
               FrontierNode* associatedFrontierNode = (isFrontierNode == true) ? i->second : NULL;

            // Check is this is marked as already being handled via the unparsing of the token stream from another statement.
            // For example, variable declarations containing multiple variables will be represented as seperate SgVariableDeclaration 
            // IR nodes in the AST, but will have been unparsed using a single token stream.
            // static int lastUnparsedToken = 0;

            // bool unparseViaTokenStream = (isFrontierNode == true);
               bool unparseViaTokenStream = (isFrontierNode == true && associatedFrontierNode->unparseUsingTokenStream == true);
#if 0
               printf ("In UnparseLanguageIndependentConstructs::unparseStatement(): isFrontierNode = %s \n",isFrontierNode ? "true" : "false");
               printf ("In UnparseLanguageIndependentConstructs::unparseStatement(): associatedFrontierNode = %p \n",associatedFrontierNode);
               if (associatedFrontierNode != NULL)
                  {
                    printf ("In UnparseLanguageIndependentConstructs::unparseStatement(): associatedFrontierNode->unparseUsingTokenStream = %s \n",associatedFrontierNode->unparseUsingTokenStream ? "true" : "false");
                  }
               printf ("In UnparseLanguageIndependentConstructs::unparseStatement(): stmt = %p = %s unparseViaTokenStream = %s \n",stmt,stmt->class_name().c_str(),unparseViaTokenStream ? "true" : "false");
               printf ("In UnparseLanguageIndependentConstructs::unparseStatement(): statementTransformed = %s \n",statementTransformed ? "true" : "false");
#endif
            // Only unparse from the token stream if this was not a transformed statement.
               unparseViaTokenStream = unparseViaTokenStream && (statementTransformed == false);
#if 0
               printf ("In UnparseLanguageIndependentConstructs::unparseStatement(): stmt = %p = %s unparseViaTokenStream = %s \n",stmt,stmt->class_name().c_str(),unparseViaTokenStream ? "true" : "false");
#endif
            // Try overruling the logic to compute if this should be unparsed from the token stream.
               if (unparseViaTokenStream == false)
                  {
                 // unparseViaTokenStream = (stmt->get_containsTransformation() == false && stmt->isTransformation() == false);
                    if (stmt->get_containsTransformation() == false && stmt->isTransformation() == false)
                       {
                      // I think this is an error and that we should be unparsing from the token stream.
#if 0
                         printf ("##### Overrule the frontier logic to unparse from the token stream: reset unparseViaTokenStream = true: stmt = %p = %s \n",stmt,stmt->class_name().c_str());
#endif
                         unparseViaTokenStream = true;
                       }
                  }

#if 0
               printf ("In UnparseLanguageIndependentConstructs::unparseStatement(): lastStatementOfGlobalScopeUnparsedUsingTokenStream = %s \n",lastStatementOfGlobalScopeUnparsedUsingTokenStream == true ? "true" : "false");
               printf ("In UnparseLanguageIndependentConstructs::unparseStatement(): unparseViaTokenStream = %s \n",unparseViaTokenStream == true ? "true" : "false");
#endif

            // DQ (11/12/2014): Added support for unparsing the associated comments that can be attached to the first declaration in global scope.
               if (unparseViaTokenStream == true)
                  {
#if 0
                    printf ("In unparseStatement(): unparseViaTokenStream == true: skipOutputOfPreprocessingInfo = %s \n",skipOutputOfPreprocessingInfo ? "true" : "false");
                    printf ("   --- stmt = %p = %s \n",stmt,stmt->class_name().c_str());
#endif
#if 0
                 // DQ (11/13/2014): This is the wrong approach.
                    if (skipOutputOfPreprocessingInfo == false)
                       {
                      // DQ (11/30/2013): Move from above to where we can better support the token unparsing.
                         unparseAttachedPreprocessingInfo(stmt, info, PreprocessingInfo::before);
                       }
#else
                 // If we are unparsing from the token stream, then we need to handle the attached preprocessing info as well.
                 // But we need to handle them as part of unparing the token stream, not from the AST. The reason this is important
                 // is demonstrated by test2014_101.c and test2014_102.c when used with the testing mode (ROSE_tokenUnparsingTestingMode == true).
                 // In this mode AST nodes are periodically marked for unparsing from the AST and thus exersizing the logic to 
                 // switch back and forth between the unparsing from the token stream and unparsing from the AST.
#endif
#if 0
                    printf ("In UnparseLanguageIndependentConstructs::unparseStatement(): unparseViaTokenStream == true: Calling unparseStatementFromTokenStream() \n");
#endif
#if 0
                    curprint("/* In unparseStatement(): unparseViaTokenStream == true */");
#endif
#if 0
                    curprint("/* In unparseStatement(): unparse using FULL token stream */");
#endif
                    int status = unparseStatementFromTokenStream(sourceFile,stmt,info,lastStatementOfGlobalScopeUnparsedUsingTokenStream);
#if 0
                    curprint("/* In unparseStatement(): DONE: unparseViaTokenStream == true */");
#endif
#if 0
                    printf ("In UnparseLanguageIndependentConstructs::unparseStatement(): DONE: unparseStatementFromTokenStream(): status = %d \n",status);
#endif
                 // If we have unparsed this statement via the token stream then we don't have to unparse it from the AST (so return).
                    outputStatementAsTokens = (status == 0);
                  }
                 else
                  {
                 // DQ (12/4/2014): This is a candidate for a partial unparse using the token stream.
                 // This would be unparsed via the AST, but since it is because it contains a transformation 
                 // rather than that it is a transformation, we should inst3ead just unpars it using the 
                 // token stream, but in two parts.  The first part is from the start of the current AST node 
                 // up to the start of the next AST node.  The last part will be to the end of the current
                 // AST node (not clear how to compute the start of the last part of the token stream).
#if 0
                    printf ("In UnparseLanguageIndependentConstructs::unparseStatement(): unparseViaTokenStream == false: Calling unparseStatementFromTokenStream() \n");
#endif
#if 0
                    curprint("/* In unparseStatement(): unparseViaTokenStream == false */");
#endif
#if 0
                    printf ("In UnparseLanguageIndependentConstructs::unparseStatement(): stmt->get_containsTransformation() = %s \n",stmt->get_containsTransformation() ? "true" : "false");
#endif
                    if (stmt->get_containsTransformation() == true)
                       {
                      // This should not BE a transformation (else it needs to be unparsed using the AST).
                         ROSE_ASSERT(stmt->isTransformation() == false);
#if 0
                         printf ("In UnparseLanguageIndependentConstructs::unparseStatement(): unparseViaTokenStream == false: stmt->get_containsTransformation() == true: Calling unparseStatementFromTokenStream() \n");
#endif
#if 0
                         curprint("/* In unparseStatement(): stmt->get_containsTransformation() == true */");
#endif
#if 0
                         curprint("/* In unparseStatement(): unparse using PARTIAL token stream */");
#endif
                         int status = unparseStatementFromTokenStreamForNodeContainingTransformation(sourceFile,stmt,info,lastStatementOfGlobalScopeUnparsedUsingTokenStream,global_previous_unparsed_as);
#if 0
                         printf ("In UnparseLanguageIndependentConstructs::unparseStatement(): unparseStatementFromTokenStreamForNodeContainingTransformation(): status = %d \n",status);
#endif
                      // If we have unparsed this statement via the token stream then we don't have to unparse it from the AST (so return).
                      // outputStatementAsTokens = (status == 0);
                         outputPartialStatementAsTokens = (status == 0);

                         if (outputPartialStatementAsTokens == true)
                            {
                           // Mark the SgUnparse_Info object to record that the statement was partially unparsed using the token stream.
#if 0
                              curprint("\n /* In unparseStatement(): outputPartialStatementAsTokens == true */ \n");
#endif
#if 0
                              printf ("@@@@@ Calling info.set_unparsedPartiallyUsingTokenStream() \n");
#endif

                              info.set_unparsedPartiallyUsingTokenStream();

                           // DQ (12/5/2014): And skip output of redundant comments and CPP directives.
                              skipOutputOfPreprocessingInfo = true;

                           // If this is a SgIfStmt (as a special case) and the true block does not have any token info, then we have to unparse this from the AST (not partially from the AST).
                           // If might be better to catch this in the simmpleFrontier traversal so that it is more uniformally handled.
                              SgIfStmt* ifStatement = isSgIfStmt(stmt);
                              if (ifStatement != NULL)
                                 {
                                   SgSourceFile* sourceFile = isSgSourceFile(SageInterface::getEnclosingFileNode(stmt));
                                   ROSE_ASSERT(sourceFile != NULL);
                                   std::map<SgNode*,TokenStreamSequenceToNodeMapping*> & tokenStreamSequenceMap = sourceFile->get_tokenSubsequenceMap();

                                   SgStatement* trueBody = ifStatement->get_true_body();
                                   if (trueBody != NULL && tokenStreamSequenceMap.find(trueBody) == tokenStreamSequenceMap.end())
                                      {
                                        printf ("It would be an error to unparse the SgIfStmt partially from the token stream. \n");

                                     // See if this will fix the problem, then we can design a better fix in the morning.
                                        info.unset_unparsedPartiallyUsingTokenStream();
#if 0
                                        printf ("ERROR: can't use this node for partial token stream unparsing \n");
                                        ROSE_ASSERT(false);
#endif
                                      }
                                 }

                           // DQ (12/15/2014): This might also depend on the value of global_previous_unparsed_as 
                           // (perhaps only when global_previous_unparsed_as == e_unparsed_as_partial_token_sequence or
                           // global_previous_unparsed_as == e_unparsed_as_token_stream).
                              if (info.unparsedPartiallyUsingTokenStream() == true)
                                 {
                                // We need to unparse the leading white space from the token stream if we are about the unparse this statement partially using token stream.

                                   bool unparseLeadingTokenStream = unparseAttachedPreprocessingInfoUsingTokenStream(stmt,info,PreprocessingInfo::before);
#if 0
                                   curprint(string("\n/* In unparseStatement(): unparseLeadingTokenStream = ") + (unparseLeadingTokenStream ? "true" : "false") + " */");
#endif
                                   if (unparseLeadingTokenStream == true)
                                      {
                                     // DQ (12/15/2014): We need to skip the unparsing of the leading white space when unparsing the SgFunctionDefinition, 
                                     // because it is called by the SgFunctionDeclaration.
                                     // unparseStatementFromTokenStream (stmt, e_leading_whitespace_start, e_token_subsequence_start);
                                        SgFunctionDefinition* functionDefinition = isSgFunctionDefinition(stmt);
                                        if (functionDefinition == NULL)
                                           {
#if 0
                                             printf ("In UnparseLanguageIndependentConstructs::unparseStatement(): unparseViaTokenStream == false: stmt->get_containsTransformation() == true: functionDefinition == NULL: Calling unparseStatementFromTokenStream() \n");
#endif
#if 0
                                          // DQ (10/27/2018): Added debugging code to identify that we must pass the Sg_File_Info object to unparseStatementFromTokenStream().
                                             printf ("In UnparseLanguageIndependentConstructs::unparseStatement(): Calling unparseStatementFromTokenStream(): stmt = %p = %s \n",stmt,stmt->class_name().c_str());
                                             SgDeclarationStatement* declaration = isSgDeclarationStatement(stmt);
                                             if (declaration != NULL)
                                                {
                                                  printf ("   --- declaration = %p = %s \n",declaration,declaration->class_name().c_str());
                                                  printf ("   --- declaration->get_firstNondefiningDeclaration() = %p \n",declaration->get_firstNondefiningDeclaration());
                                                  printf ("   --- --- filename = %s \n",declaration->get_firstNondefiningDeclaration()->get_file_info()->get_filenameString().c_str());
                                                  printf ("   --- declaration->get_definingDeclaration()         = %p \n",declaration->get_definingDeclaration());
                                                  printf ("   --- --- filename = %s \n",declaration->get_definingDeclaration()->get_file_info()->get_filenameString().c_str());
                                                }
#endif
                                             unparseStatementFromTokenStream (stmt, e_leading_whitespace_start, e_token_subsequence_start, info);
                                           }
                                          else
                                           {
#if 0
                                             curprint("/* In unparseStatement(): skip leading whitespace for SgFunctionDefinition (when unparsing using partial token stream mode) */");
#endif
                                           }
                                      }
                                     else
                                      {
#if 1
                                        printf ("In unparseStatement(): Unparse the leading whitespace from the AST because it's comments and/or CPP directives have been modified \n");
#endif
                                        bool unparseExtraNewLine = (stmt->getAttachedPreprocessingInfo() != NULL);
                                        if (unparseExtraNewLine == true)
                                           {
                                          // curprint ("// new line added \n  ");
                                             curprint ("\n ");
                                           }
                                        unparseAttachedPreprocessingInfo(stmt,info,PreprocessingInfo::before);
                                      }
#if 0
                                   curprint(string("\n/* In unparseStatement(): (suppress global scope?): stmt = ") + (stmt->class_name()) + " */");
#endif
                                 }
                            }
                           else
                            {
#if 0
                              curprint("/* In unparseStatement(): unparse using AST (not using token stream) */");
#endif
#if 0
                              curprint("\n /* In unparseStatement(): outputPartialStatementAsTokens == false */ \n");
#endif
#if 0
                              printf ("@@@@@ Calling info.unset_unparsedPartiallyUsingTokenStream() \n");
#endif
                              info.unset_unparsedPartiallyUsingTokenStream();
                            }
#if 0
                         printf ("This is a canidate for a partial unparse using the token stream \n");
                         ROSE_ASSERT(false);
#endif
                       }
                      else
                       {
#if 0
                         printf ("stmt->get_containsTransformation() == false \n");
                         printf ("stmt->isTransformation() = %s \n",stmt->isTransformation() ? "true" : "false");
#endif
                         if (stmt->isTransformation() == true)
                            {
#if 0
                              printf ("This is a transformation, so unparse via the AST (turn off unparsedPartiallyUsingTokenStream) \n");
#endif
                              info.unset_unparsedPartiallyUsingTokenStream();
#if 0
                              printf ("Exiting as a test! \n");
                              ROSE_ASSERT(false);
#endif
                            }
                       }
#if 0
                    curprint("/* In unparseStatement(): DONE: unparseViaTokenStream == false */");
#endif
                  }
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }

#if 0
          printf ("outputStatementAsTokens = %s \n",outputStatementAsTokens ? "true" : "false");
          if (outputStatementAsTokens == false)
             {
               printf ("outputPartialStatementAsTokens = %s \n",outputPartialStatementAsTokens ? "true" : "false");
             }
#endif

       // DQ (12/5/2014): Adding support to track transitions between unparsing using 
       // tokens sequences, partial tokens sequences, and directly from the AST.
       // unparsed_as_enum_type global_unparsed_as = e_unparsed_as_error;
          if (outputStatementAsTokens == true)
             {
#if 0
               curprint("/* In unparseStatement(): set global_unparsed_as = e_unparsed_as_token_stream */");
#endif
               global_unparsed_as = e_unparsed_as_token_stream;
             }
            else
             {
               if (outputPartialStatementAsTokens == true)
                  {
#if 0
                    curprint("/* In unparseStatement(): set global_unparsed_as = e_unparsed_as_partial_token_sequence */");
#endif
                    global_unparsed_as = e_unparsed_as_partial_token_sequence;
                  }
                 else
                  {
#if 0
                    curprint("/* In unparseStatement(): set global_unparsed_as = e_unparsed_as_AST */");
#endif
                    global_unparsed_as = e_unparsed_as_AST;
                  }
             }
#if 0
          printf ("global_unparsed_as = %d \n",global_unparsed_as);
#endif
       // At this point we could test for a gap in the mapping of the current and previous statements being unparsed.
       // Then unparse the leading white space for the current statement.
       // Ignore the trailing white space for the previous statement; anything interesting should have been:
       //   1) moved with any statements that were removed
       //   2) been associated with the current statement if nothing was removed.
       //   3) or be added as a result on a transformation being inserted.

          if (global_previous_unparsed_as == e_unparsed_as_token_stream || global_previous_unparsed_as == e_unparsed_as_partial_token_sequence)
             {
#if 0
               printf ("(global_previous_unparsed_as == e_unparsed_as_token_stream || global_previous_unparsed_as == e_unparsed_as_partial_token_sequence) == true \n");
#endif
#if 0
               if (global_unparsed_as == e_unparsed_as_partial_token_sequence || global_unparsed_as == e_unparsed_as_AST)
                  {
                 // Unparse the leading whitespace for the current stmt?
                 // Add format statment to output CR.
                    printf ("Output a CR at a transition \n");

                    unp->cur.format(stmt, info, FORMAT_BEFORE_STMT);
                  }
#endif
               if (global_unparsed_as == e_unparsed_as_AST)
                  {
                 // Add format statement to output CR.
#if 0
                    curprint("/* In unparseStatement(): calling unp->cur.format() (global_unparsed_as == e_unparsed_as_AST) */");
#endif
#if 0
                    printf ("Calling unp->cur.reset_chars_on_line() (to reset the formatting for unparsing from the AST) \n");
#endif

                 // DQ (11/14/2015): If we are unparsing statements in a SgBasicBlock, then we want to
                 // know if the SgBasicBlock is being unparsed using the partial_token_sequence so that
                 // we can supress the formatting that adds a CR to the start of the current statement 
                 // being unparsed.
                    bool parentStatementListBeingUnparsedUsingPartialTokenSequence = info.parentStatementListBeingUnparsedUsingPartialTokenSequence();

#if 1
                    if (parentStatementListBeingUnparsedUsingPartialTokenSequence == true)
                       {
#if 0
                         printf ("In unparseStatement(): We need to supress the leading CR for this case (initially statements in a SgBasicBlock) \n");
#endif
#if 0
                         curprint("/* In unparseStatement(): suppress CR */");
#endif
                      // ROSE_ASSERT(false);
                       }
                      else
                       {
                      // Note that becasue of the logic below, this is the first CR for a SgBasicBlock (I don't know exactly why).
                      // The other CR frequenty introduces is in the unparseLanguageSpecificStatement() function.
                         unp->cur.reset_chars_on_line();
#if 0
                         curprint("/* In unparseStatement(): before format: FORMAT_BEFORE_STMT */");
#endif
                         unp->cur.format(stmt, info, FORMAT_BEFORE_STMT);
#if 0
                         curprint("/* In unparseStatement(): after format: FORMAT_BEFORE_STMT */");
#endif
                       }
#else
                    unp->cur.reset_chars_on_line();

#error "DEAD CODE!"
                    
#if 0
                    curprint("/* In unparseStatement(): before format: FORMAT_BEFORE_STMT */");
#endif
                    unp->cur.format(stmt, info, FORMAT_BEFORE_STMT);
#if 0
                    curprint("/* In unparseStatement(): after format: FORMAT_BEFORE_STMT */");
#endif
#endif
                 // DQ (12/12/2014): If we are transitioning to unparsing from the AST, then this should be valid.
                    if (info.unparsedPartiallyUsingTokenStream() == true)
                       {
#if 0
                         printf ("WARNING: reset info.unparsedPartiallyUsingTokenStream() == false (test 1) \n");
#endif
                         info.unset_unparsedPartiallyUsingTokenStream();
                       }
                    ROSE_ASSERT(info.unparsedPartiallyUsingTokenStream() == false);
                  }
             }

       // DQ (12/12/2014): If we are truely unparsing from the AST, then this should be valid.
          if (global_unparsed_as == e_unparsed_as_AST)
             {
#if 0
               printf ("(global_unparsed_as == e_unparsed_as_AST) == true \n");
#endif
            // DQ (12/12/2014): If we are transitioning to unparsing from the AST, then this should be valid.
               if (info.unparsedPartiallyUsingTokenStream() == true)
                  {
#if 0
                    printf ("WARNING: reset info.unparsedPartiallyUsingTokenStream() == false (test 2) \n");
#endif
                    info.unset_unparsedPartiallyUsingTokenStream();
                  }
               ROSE_ASSERT(info.unparsedPartiallyUsingTokenStream() == false);
             }

#if 0
          printf ("In UnparseLanguageIndependentConstructs::unparseStatement(): outputStatementAsTokens = %s \n",outputStatementAsTokens == true ? "true" : "false");
          printf ("global_previous_unparsed_as: %s \n",unparsed_as_kind(global_previous_unparsed_as).c_str());
          printf ("global_unparsed_as:          %s \n",unparsed_as_kind(global_unparsed_as).c_str());
          printf ("info.unparsedPartiallyUsingTokenStream() = %s \n",info.unparsedPartiallyUsingTokenStream() ? "true" : "false");
#endif

       // Only unparse using the AST if this was not able to be unparsed from the token stream.
          if (outputStatementAsTokens == false)
             {
            // DQ (4/1/2014): Suggested fix to prevent unparsing of C style comments in Fortran 
            // codes when using the verbose modes.
            // DQ (12/1/2013): Not clear if this is helpful or not (but it communicates in the 
            // unparsed code what statements were unparse using either the AST or the token stream).
            // if ( SgProject::get_verbose() > 0 )
            // if ( SgProject::get_verbose() > 0 && SageInterface::getProject()->get_C_only() == true)
               if ( SgProject::get_verbose() > 0 && (SageInterface::getProject()->get_C_only() == true || SageInterface::getProject()->get_Cxx_only() == true) )
                  {
                    string s = "/* Unparsing from the AST stmt (or partially from token stream) = " + stmt->class_name() + " */ ";
                    curprint (s);
                  }
#if 0
               printf ("In unparseStatement(): outputStatementAsTokens == false: skipOutputOfPreprocessingInfo = %s \n",skipOutputOfPreprocessingInfo ? "true" : "false");
               printf ("   --- stmt = %p = %s \n",stmt,stmt->class_name().c_str());
#endif
            // bool skipOutputOfPreprocessingInfo = (isSgFunctionDefinition(stmt) != NULL);
               if (skipOutputOfPreprocessingInfo == false)
                  {
#if 0
                    curprint("/* In UnparseLanguageIndependentConstructs::unparseStatement(): calling unparseAttachedPreprocessingInfoUsingTokenStream test 2 */");
#endif
                 // DQ (1/15/2015): Check for comments or CPP directives associated with the statement.
                 // DQ (11/13/2014): Add a new line (CR) to address when we may have unparsed the previous statement from the token stream.
                 // bool unparseExtraNewLine = unparseAttachedPreprocessingInfoUsingTokenStream(stmt,info,PreprocessingInfo::before);
                    bool unparseExtraNewLine = (stmt->getAttachedPreprocessingInfo() != NULL);
#if 0
                    curprint("/* In UnparseLanguageIndependentConstructs::unparseStatement(): calling unparseAttachedPreprocessingInfoUsingTokenStream test 3 */ \n ");
#endif
                    if (cur_file != NULL && cur_file->get_unparse_tokens() == true && unparseExtraNewLine == true)
                       {
#if 0
                      // Add a new line.
                         printf ("##### Adding a new line (previous statement may have been unparsed using the token stream and be missing a CR at the end; error for CPP directives) \n");
#endif
                      // Adding a new line is more complex than it should be because the the CR is interpreted and 
                      // ignored if it is at the end of the string, so adding extra space fixes this temporarily.
                      // unp->cur.insert_newline(1);
                      // curprint ("// new line added \n  ");
                         curprint ("\n ");
                       }
#if 0
                    printf("In UnparseLanguageIndependentConstructs::unparseStatement(): calling unparseAttachedPreprocessingInfo test 4: before \n");
#endif
#if 0
                    curprint("/* In UnparseLanguageIndependentConstructs::unparseStatement(): calling unparseAttachedPreprocessingInfo test 4 */ \n ");
#endif
                 // DQ (11/30/2013): Move from above to where we can better support the token unparsing.
                    unparseAttachedPreprocessingInfo(stmt, info, PreprocessingInfo::before);
#if 0
                    printf("In UnparseLanguageIndependentConstructs::unparseStatement(): DONE calling unparseAttachedPreprocessingInfo test 4: before \n");
#endif
#if 0
                    curprint("/* In UnparseLanguageIndependentConstructs::unparseStatement(): calling unparseAttachedPreprocessingInfo test 5 */ \n ");
#endif
                  }
                 else
                  {
#if 0
                    printf ("PreprocessingInfo::before: If we are not unparsing an attached PreprocessingInfo from the AST, we need to unparse it from the token stream \n");
                    curprint("/* PreprocessingInfo::before: If we are not unparsing an attached PreprocessingInfo from the AST, we need to unparse it from the token stream */\n");
#endif
                  }

            // DQ (12/4/2007): Added to ROSE (was removed at some point).
               unparseLineDirectives(stmt);

            // DQ (7/19/2007): This only applies to Fortran where every statement can have a statement number (numeric lable, different from SgLabelStatement)
               unparseStatementNumbers(stmt,info);
#if 0
               printf ("In UnparseLanguageIndependentConstructs::unparseStatement(): Selecting an unparse function for stmt = %p = %s \n",stmt,stmt->class_name().c_str());
               curprint("/* test 1 */\n");
#endif

#if 0
            // DQ (11/13/2018): Configure a temporary test.
               if (isSgIncludeDirectiveStatement(stmt) != NULL)
                  {
                    static int count = 0;
                    count++;
                    if (count > 3)
                       {
                         printf ("Exiting as a test! count = %d \n",count);
                         ROSE_ASSERT(false);
                       }
                  }
#endif

#if 0
               curprint("/* In UnparseLanguageIndependentConstructs::unparseStatement(): Selecting an unparse function for stmt */");
#endif
               switch (stmt->variantT())
                  {
                    case V_SgGlobal:                        unparseGlobalStmt                    (stmt, info); break;
                    case V_SgFunctionTypeTable:             unparseFuncTblStmt                   (stmt, info); break;
                    case V_SgNullStatement:                 unparseNullStatement                 (stmt, info); break;

                 // DQ (11/29/2008): Added support for unparsing CPP directives now supported as IR nodes.
                    case V_SgIncludeDirectiveStatement:     unparseIncludeDirectiveStatement     (stmt, info); break;
                    case V_SgDefineDirectiveStatement:      unparseDefineDirectiveStatement      (stmt, info); break;
                    case V_SgUndefDirectiveStatement:       unparseUndefDirectiveStatement       (stmt, info); break;
                    case V_SgIfdefDirectiveStatement:       unparseIfdefDirectiveStatement       (stmt, info); break;
                    case V_SgIfndefDirectiveStatement:      unparseIfndefDirectiveStatement      (stmt, info); break;
                    case V_SgDeadIfDirectiveStatement:      unparseDeadIfDirectiveStatement      (stmt, info); break;
                    case V_SgIfDirectiveStatement:          unparseIfDirectiveStatement          (stmt, info); break;
                    case V_SgElseDirectiveStatement:        unparseElseDirectiveStatement        (stmt, info); break;
                    case V_SgElseifDirectiveStatement:      unparseElseifDirectiveStatement      (stmt, info); break;
                    case V_SgEndifDirectiveStatement:       unparseEndifDirectiveStatement       (stmt, info); break;
                    case V_SgLineDirectiveStatement:        unparseLineDirectiveStatement        (stmt, info); break;
                    case V_SgWarningDirectiveStatement:     unparseWarningDirectiveStatement     (stmt, info); break;
                    case V_SgErrorDirectiveStatement:       unparseErrorDirectiveStatement       (stmt, info); break;
                    case V_SgEmptyDirectiveStatement:       unparseEmptyDirectiveStatement       (stmt, info); break;
                    case V_SgIdentDirectiveStatement:       unparseIdentDirectiveStatement       (stmt, info); break;
                    case V_SgIncludeNextDirectiveStatement: unparseIncludeNextDirectiveStatement (stmt, info); break;
                    case V_SgLinemarkerDirectiveStatement:  unparseLinemarkerDirectiveStatement  (stmt, info); break;

                 // Liao 10/21/2010. Handle generic OpenMP directive unparsing here.
                    case V_SgOmpSectionStatement:
                    case V_SgOmpTaskwaitStatement:
                    case V_SgOmpBarrierStatement:           unparseOmpSimpleStatement        (stmt, info);break;
                    case V_SgOmpThreadprivateStatement:     unparseOmpThreadprivateStatement (stmt, info);break;
                    case V_SgOmpFlushStatement:             unparseOmpFlushStatement         (stmt, info);break;
                    case V_SgOmpDeclareSimdStatement:       unparseOmpDeclareSimdStatement   (stmt, info);break;

                 // Generic OpenMP directives with a format of : begin-directive, begin-clauses, body, end-directive , end-clauses
                    case V_SgOmpCriticalStatement:
                    case V_SgOmpMasterStatement:
                    case V_SgOmpOrderedStatement:
                    case V_SgOmpSectionsStatement:
                    case V_SgOmpParallelStatement:
                    case V_SgOmpTargetStatement:
                    case V_SgOmpTargetDataStatement:
                    case V_SgOmpWorkshareStatement:
                    case V_SgOmpSingleStatement:
                    case V_SgOmpTaskStatement:
                    case V_SgOmpSimdStatement:
                    case V_SgOmpAtomicStatement: // Atomic may have clause now
                         unparseOmpGenericStatement (stmt, info); 
                         break;

                    default:
                      // DQ (11/4/2008): This is a bug for the case of a SgFortranDo statement, unclear what to do about this.
                      // Call the derived class implementation for C, C++, or Fortran specific language unparsing.
                      // unparseLanguageSpecificStatement(stmt,info);
                      // unp->repl->unparseLanguageSpecificStatement(stmt,info);
#if 0
                         printf ("In UnparseLanguageIndependentConstructs::unparseStatement(): Calling unparseLanguageSpecificStatement() for stmt = %p = %s \n",stmt,stmt->class_name().c_str());
#endif
                         unparseLanguageSpecificStatement(stmt,info);
                         break;
                  }

#if 0
               curprint("/* In UnparseLanguageIndependentConstructs::unparseStatement(): DONE Selecting an unparse function for stmt */ \n ");
#endif

            // DQ (5/8/2010): Reset the source code position in the AST.
               if (unp->get_resetSourcePosition() == true)
                  {
                    unp->resetSourcePosition(stmt);
                  }
             }
        }

#if 0
     printf ("Here is where we want to output the trailing whitespace for the last statement in each scope: stmt = %p = %s \n",stmt,stmt->class_name().c_str());
     printf ("   --- In UnparseLanguageIndependentConstructs::unparseStatement(): outputStatementAsTokens = %s \n",outputStatementAsTokens == true ? "true" : "false");
     printf ("   --- global_previous_unparsed_as:                         %s \n",unparsed_as_kind(global_previous_unparsed_as).c_str());
     printf ("   --- global_unparsed_as:                                  %s \n",unparsed_as_kind(global_unparsed_as).c_str());
     printf ("   --- info.unparsedPartiallyUsingTokenStream()           = %s \n",info.unparsedPartiallyUsingTokenStream() ? "true" : "false");
     printf ("   --- lastStatementOfGlobalScopeUnparsedUsingTokenStream = %s \n",lastStatementOfGlobalScopeUnparsedUsingTokenStream ? "true" : "false");
#endif

  // DQ (1/10/2014): We need to handle the general case of trailing tokens at the end of a statements.
     if (outputStatementAsTokens == true)
        {
       // If these is the case then the last statement for the collection of statements was notices and 
       // if this statement matches it then the trailing tokens were used to close off the scope.
        }
       else
        {
       // Here we are either unparsing from the AST or partially from the token stream (based on the AST).
       // This is the only case were we have to worry about unparsing the trailing tokens (since they case 
       // when unparsing using the token stream is well handled, though not refactored to be located here).
#if 0
          curprint(string("/* FORMATTING: UnparseLanguageIndependentConstructs::unparseStatement(): Here is where we want to output the trailing whitespace for the last statement in each scope: ") + stmt->class_name() + " */ \n ");
#endif
          SgScopeStatement* scope = isSgScopeStatement(stmt->get_parent());

       // ROSE_ASSERT(scope != NULL);
       // Note that the parent of the global scope is not a scope, so we handle this as a special case.
       // Also the parent of a SgFunctionDefinition is a SgFunctionDeclaration (also not a scope).
          SgGlobal* globalScope = isSgGlobal(stmt);
          SgFunctionDefinition* functionDefinition = isSgFunctionDefinition(stmt);
       // if (scope == NULL && globalScope == NULL)
          if (scope == NULL && globalScope == NULL && functionDefinition == NULL)
             {
#if 0
               printf ("Warning: parent of stmt = %p = %s is not a scope \n",stmt,stmt->class_name().c_str());
#endif
             }
       // ROSE_ASSERT(scope != NULL || globalScope != NULL);

       // DQ (1/10/2015) We have added support to carry a pointer to the SgSourceFile within the SgUnparse_Info.
          SgSourceFile* sourceFile = info.get_current_source_file();

#if 0
          printf ("In UnparseLanguageIndependentConstructs::unparseStatement(): sourceFile         = %p \n",sourceFile);
          printf ("In UnparseLanguageIndependentConstructs::unparseStatement(): scope              = %p \n",scope);
          printf ("In UnparseLanguageIndependentConstructs::unparseStatement(): globalScope        = %p \n",globalScope);
          printf ("In UnparseLanguageIndependentConstructs::unparseStatement(): functionDefinition = %p \n",functionDefinition);
#endif

#if 0
          curprint("/* test 2 */\n");
#endif

       // DQ (1/10/2015): We can't enforce this for all expresions (not clear why).
       // ROSE_ASSERT(sourceFile != NULL);
          if (sourceFile != NULL)
             {
               std::map<SgNode*,TokenStreamSequenceToNodeMapping*> & tokenStreamSequenceMap = sourceFile->get_tokenSubsequenceMap();
               bool isLastStatementOfScope = false;
               SgStatement* lastStatement = NULL;
               if (scope != NULL)
                  {
                 // DQ (1/12/2015): The call to lastStatementOfScopeWithTokenInfo() can fail when the scope is a SgIfStmt 
                 // (this happens in the tests/nonsmoke/functional/CompileTests/Cxx_tests test codes).
                 // lastStatement = SageInterface::lastStatementOfScopeWithTokenInfo (scope, tokenStreamSequenceMap);
                    if (sourceFile->get_unparse_tokens() == true)
                       {
                         if (isSgIfStmt(scope) != NULL)
                            {
                              printf ("Warning: can't call SageInterface::lastStatementOfScopeWithTokenInfo() with scope == SgIfStmt \n");
                            }
                         lastStatement = SageInterface::lastStatementOfScopeWithTokenInfo (scope, tokenStreamSequenceMap);
                       }
                    isLastStatementOfScope = (stmt == lastStatement);
#if 0
                    printf ("isLastStatementOfScope = %s \n",isLastStatementOfScope ? "true" : "false");
#endif
                  }
                 else
                  {
                    if (globalScope != NULL)
                       {
                      // DQ (1/10/2015): The case of SgGlobalScope does not permit the output of a trailing whitespce (since it is not defined).
                      // isLastStatementOfScope = true;
                      // lastStatement = stmt;
                         isLastStatementOfScope = false;
                         lastStatement = stmt;
                       }
                      else
                       {
#if 0
                         printf ("ERROR: either scope != NULL or globalScope != NULL: stmt = %p = %s \n",stmt,stmt->class_name().c_str());
                      // ROSE_ASSERT(false);
#endif
                       }
                  }
#if 0
               if (lastStatement != NULL)
                  {
                    printf ("   --- computed lastStatement of scope = %p = %s stmt = %p = %s \n",lastStatement,lastStatement->class_name().c_str(),stmt,stmt->class_name().c_str());
                  }
               printf ("   --- isLastStatementOfScope = %s \n",isLastStatementOfScope ? "true" : "false");
#endif

            // DQ (1/10/2015): Output the token sequence representing the trailing whitespace, except for the SgGlobalScope (where it is not defined).
               if (isLastStatementOfScope == true)
                  {
#if 0
                    curprint("/* In UnparseLanguageIndependentConstructs::unparseStatement(): isLastStatementOfScope == true */ \n ");
#endif
                    ROSE_ASSERT(lastStatement != NULL);

#if 0
                    printf ("In UnparseLanguageIndependentConstructs::unparseStatement(): isLastStatementOfScope == true: Calling unparseStatementFromTokenStream() \n");
#endif
                 // Unparse the sequence of tokens from e_trailing_whitespace_start to (but excluding) e_trailing_whitespace_end.
                    unparseStatementFromTokenStream (stmt, e_trailing_whitespace_start, e_trailing_whitespace_end, info);

#if 0
                    printf ("In UnparseLanguageIndependentConstructs::unparseStatement(): Unparse the last token explicitly: Calling unparseStatementFromTokenStream() \n");
#endif
                 // Unparse the last token explicitly.
                    unparseStatementFromTokenStream (stmt, e_trailing_whitespace_end, e_trailing_whitespace_end, info);
                  }
             }
            else
             {
#if 0
            // DQ (1/12/2015): This message it commented out, it is frequently triggered for SgVariableDeclaration IR nodes.
               printf ("NOTE: info.get_current_source_file() == NULL for stmt = %p = %s \n",stmt,stmt->class_name().c_str());
#endif
             }
        }

#if 0
  // DQ (8/7/2012): I don't think we need this.
  // DQ (11/3/2007): Save the original scope so that we can restore it at the end (since we don't use a new SgUnparse_Info object).
     if (scopeStatement != NULL)
        {
       // DQ (12/5/2007): This assertion appears to work better.
          ROSE_ASSERT(savedScope != NULL || isSgGlobal(scopeStatement) != NULL);

#if 0
          printf ("At end of scope: Setting the current_scope in info: scopeStatement = %p = %s = %s \n",
               scopeStatement,scopeStatement->class_name().c_str(),SageInterface::get_name(scopeStatement).c_str());

          if (savedScope != NULL)
               printf ("At end of scope: Resetting using the savedScope = %p = %s = %s \n",
                    savedScope,savedScope->class_name().c_str(),SageInterface::get_name(savedScope).c_str());
#endif
          info.set_current_scope(NULL);
          info.set_current_scope(savedScope);
        }

  // DQ (12/5/2007): Check if the call to unparse any construct changes the scope stored in info.
  // This does error checking on ALL statements!
     SgScopeStatement* scopeAfterUnparseStatement = info.get_current_scope();
     if (savedScope != scopeAfterUnparseStatement)
        {
          printf ("WARNING: scopes stored in SgUnparse_Info object have been changed \n");
          ROSE_ASSERT(false);
        }
#endif

#if OUTPUT_EMBEDDED_COLOR_CODES_FOR_STATEMENTS
     if (get_embedColorCodesInGeneratedCode() > 0)
        {
          printColorCodes ( stmt, false, stateVector );
        }
#endif

  // DQ (7/20/2008): Part of new support for unparsing arbitrary strings into the unparsed code.
     if (unparseAttribute != NULL)
        {
          string code = unparseAttribute->toString(AstUnparseAttribute::e_after);
          curprint (code);
        }

#if 0
     curprint("/* FORMATTING: UnparseLanguageIndependentConstructs::unparseStatement() */");
#endif

  // We only want to output formatting operations if we are not unparsing from the token stream.
  // DQ (comments) This is where new lines are output after the statement.
  // unp->cur.format(stmt, info, FORMAT_AFTER_STMT);
     if (outputStatementAsTokens == false && outputPartialStatementAsTokens == false)
        {
       // DQ (comments) This is where new lines are output after the statement.
       // I think this will only output a newline if the statement unparsed is 
       // long enough (beyond some specific threshhold).
#if 0
          curprint("/* FORMATTING: UnparseLanguageIndependentConstructs::unparseStatement(): calling unp->cur.format() (outputStatementAsTokens == false && outputPartialStatementAsTokens == false) */");
#endif
#if 0
          curprint("/* In unparseStatement(): before format: FORMAT_AFTER_STMT */");
#endif
          unp->cur.format(stmt, info, FORMAT_AFTER_STMT);
#if 0
          curprint("/* In unparseStatement(): after format: FORMAT_AFTER_STMT */");
#endif
        }

  // Markus Kowarschik: This is the new code to unparse directives after the current statement
#if 0
     printf ("Output the comments and CCP directives for the SgStatement stmt = %p = %s (after) lastStatementOfGlobalScopeUnparsedUsingTokenStream = %s \n",
          stmt,stmt->class_name().c_str(),lastStatementOfGlobalScopeUnparsedUsingTokenStream ? "true" : "false");
#endif
  // unparseAttachedPreprocessingInfo(stmt, info, PreprocessingInfo::after);
  // if (outputStatementAsTokens == false)
#if 0
     if (lastStatementOfGlobalScopeUnparsedUsingTokenStream == false)
        {
          unparseAttachedPreprocessingInfo(stmt, info, PreprocessingInfo::after);
        }
#else
#if 0
     printf ("calling unparseAttachedPreprocessingInfo(stmt, info, PreprocessingInfo::after): skipOutputOfPreprocessingInfo = %s \n",skipOutputOfPreprocessingInfo ? "true" : "false");
#endif

  // DQ (1/6/2014): This appears to always be false, and it should be set to true for the last statement.
  // ROSE_ASSERT(lastStatementOfGlobalScopeUnparsedUsingTokenStream == false);

#if 0
     curprint("/* test 3 */\n");
#endif

     if (skipOutputOfPreprocessingInfo == false)
        {
          if (lastStatementOfGlobalScopeUnparsedUsingTokenStream == false)
             {
#if 0
               printf("PreprocessingInfo::after: skipOutputOfPreprocessingInfo == false (unparse attached comment or directive: after) \n");
#endif
#if 0
               curprint("/* PreprocessingInfo::after: skipOutputOfPreprocessingInfo == false (unparse attached comment or directive) */\n");
#endif
               unparseAttachedPreprocessingInfo(stmt, info, PreprocessingInfo::after);
             }
        }
       else
        {
#if 0
          printf ("PreprocessingInfo::after: If we are not unparsing an attached PreprocessingInfo from the AST, we need to unparse it from the token stream \n");
          curprint("/* PreprocessingInfo::after: If we are not unparsing an attached PreprocessingInfo from the AST, we need to unparse it from the token stream */\n");
#endif
        }
#endif
#if 0
     printf ("DONE: Output the comments and CCP directives for the SgStatement stmt = %p = %s (after) \n",stmt,stmt->class_name().c_str());
#endif

#if 0
     curprint("/* test 4 */\n");
#endif

  // DQ (5/31/2005): special handling for compiler generated statements
     if (isSgGlobal(stmt) != NULL)
        {
#if 0
          printf ("Output template definitions after the final comments in the file \n");
#endif
          outputCompilerGeneratedStatements(info);
        }

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES || 0
     printf ("Leaving unparse statement (%p): sage_class_name() = %s name = %s \n",stmt,stmt->sage_class_name(),SageInterface::get_name(stmt).c_str());
  // printf ("Leaving unparse statement (%p): sage_class_name() = %s \n",stmt,stmt->sage_class_name());
  // curprint ( string("\n/* Bottom of unparseStatement: sage_class_name() = " + stmt->sage_class_name() + " */ \n";
     curprint ( string("\n/* Bottom of unparseStatement (" ) + StringUtility::numberToString(stmt) 
         + "): sage_class_name() = " + stmt->sage_class_name() + " */ \n");
#endif

#if 0
     printf ("Leaving unparseStatement(): info.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
     printf ("Leaving unparseStatement(): info.SkipEnumDefinition()  = %s \n",(info.SkipEnumDefinition() == true) ? "true" : "false");
#endif

  // DQ (9/9/2016): These should have been setup to be the same.
     ROSE_ASSERT(info.SkipClassDefinition() == info.SkipEnumDefinition());

#if 0
     curprint ("/* Leaving unparse statement() */");
#endif

#if 0
  // DQ (11/13/2018): Configure a temporary test.
     if (isSgIncludeDirectiveStatement(stmt) != NULL)
        {
          static int count = 0;
          count++;
          if (count > 3)
             {
               printf ("Exiting as a test! count = %d \n",count);
               ROSE_ASSERT(false);
             }
        }
#endif

   }


//-----------------------------------------------------------------------------------
//  void Unparse_ExprStmt::unparseExpression
//  
//  General unparse function for expressions. Then it routes to the appropriate 
//  function to unparse each kind of expression. Type and symbols still use the 
//  original unparse function because they don't have file_info and therefore, 
//  will not print out file information
//-----------------------------------------------------------------------------------
void
UnparseLanguageIndependentConstructs::unparseExpression(SgExpression* expr, SgUnparse_Info & info)
   {
  // directives(expr);

  // DQ (3/21/2004): This assertion should have been in place before now!
     ROSE_ASSERT (expr != NULL);

#if 0
     printf ("unparseExpression() (language independent = %s) expression (%p): %s compiler-generated = %s \n",
          languageName().c_str(),expr,expr->class_name().c_str(),expr->get_file_info()->isCompilerGenerated() ? "true" : "false");
     curprint(string("\n /*    unparseExpression(): class name  = ") + expr->class_name().c_str() + " */ \n");
#endif

#if 0
     printf ("In unparse language independent expression(): info.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
     printf ("In unparse language independent expression(): info.SkipEnumDefinition()  = %s \n",(info.SkipEnumDefinition()  == true) ? "true" : "false");
#endif

  // DQ (9/9/2016): These should have been setup to be the same.
     ROSE_ASSERT(info.SkipClassDefinition() == info.SkipEnumDefinition());

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES
  // DQ (8/21/2005): Suppress comments when unparsing to build type names
     if ( !info.SkipComments() || !info.SkipCPPDirectives() )
        {
          ROSE_ASSERT(expr->get_startOfConstruct() != NULL);
          ROSE_ASSERT(expr->get_file_info() != NULL);
          printf ("Unparse expression (%p): %s compiler-generated = %s \n",expr,expr->class_name().c_str(),expr->get_file_info()->isCompilerGenerated() ? "true" : "false");
          char buffer[100];
          snprintf (buffer,100,"%p",expr);
          curprint ( "\n/* Top of unparseExpression " + expr->class_name() 
              + " at: " + buffer 
              + " compiler-generated (file_info) = " + (expr->get_file_info()->isCompilerGenerated() ? "true" : "false")
              + " compiler-generated (startOfConstruct) = " + (expr->get_startOfConstruct()->isCompilerGenerated() ? "true" : "false") + " */ \n");
        }
#endif

     ROSE_ASSERT(expr != NULL);
     ROSE_ASSERT(expr->get_startOfConstruct() != NULL);
     ROSE_ASSERT(expr->get_file_info() != NULL);
     if (expr->get_file_info()->isCompilerGenerated() != expr->get_startOfConstruct()->isCompilerGenerated())
        {
          printf ("In unparseExpression(%p = %s): Detected error expr->get_file_info()->isCompilerGenerated() != expr->get_startOfConstruct()->isCompilerGenerated() \n",expr,expr->class_name().c_str());
          printf ("  -- expr->get_file_info() = %p expr->get_operatorPosition() = %p expr->get_startOfConstruct() = %p \n",expr->get_file_info(),expr->get_operatorPosition(),expr->get_startOfConstruct());

          printf ("  -- expr->get_file_info()->isCompilerGenerated()        = %s \n",expr->get_file_info()->isCompilerGenerated()        ? "true" : "false");
          printf ("  -- expr->get_startOfConstruct()->isCompilerGenerated() = %s \n",expr->get_startOfConstruct()->isCompilerGenerated() ? "true" : "false");

       // DQ (9/11/2011): Reorganize to make this better code that can be analyized using static analysis (static analysis tools don't understand access functions).
       // ROSE_ASSERT(expr->get_file_info()->get_parent() != NULL);
       // printf ("parent of file info = %p = %s \n",expr->get_file_info()->get_parent(),expr->get_file_info()->get_parent()->class_name().c_str());
          ROSE_ASSERT(expr != NULL);
          Sg_File_Info* fileInfo = expr->get_file_info();
          ROSE_ASSERT(fileInfo != NULL);
          SgNode* fileInfoParent = fileInfo->get_parent();
          if (fileInfoParent == NULL) {
            printf ("[unparseExpression] file info = %p = %s has null parent.\n",fileInfo,fileInfo->class_name().c_str());
          } else {
            printf("parent of file info = %p = %s \n",fileInfoParent,fileInfoParent->class_name().c_str());
          }
          ROSE_ASSERT(fileInfoParent != NULL);

       // DQ (9/11/2011): Reorganize to make this better code that can be analyized using static analysis (static analysis tools don't understand access functions).
       // expr->get_file_info()->display("expr->get_file_info(): debug");
       // expr->get_startOfConstruct()->display("expr->get_startOfConstruct(): debug");
          fileInfo->display("expr->get_file_info(): debug");

       // Sg_File_Info* startOfConstructFileInfo = expr->get_file_info();
          Sg_File_Info* startOfConstructFileInfo = expr->get_startOfConstruct();
          ROSE_ASSERT(startOfConstructFileInfo != NULL);
          startOfConstructFileInfo->display("expr->get_startOfConstruct(): debug");
        }
  // Fails when merging ASTs loaded from files
     ROSE_ASSERT(expr->get_file_info()->isCompilerGenerated() == expr->get_startOfConstruct()->isCompilerGenerated());

#if 0
     printf ("In unparseExpression(%p = %s) \n",expr,expr->class_name().c_str());
     expr->get_file_info()->display("unparseExpression (debug)");
#endif

  // DQ (12/5/2006): Let's ignore the case of a transformation for now!
     if (expr->get_endOfConstruct() == NULL && expr->get_file_info()->isTransformation() == false)
        {
          printf ("Error in unparseExpression(): expr = %p = %s expr->get_endOfConstruct() == NULL \n",expr,expr->class_name().c_str());
          expr->get_file_info()->display("unparseExpression (debug)");
        }
  // ROSE_ASSERT(expr->get_endOfConstruct() != NULL);

#if 0
  // DQ (10/25/2006): Debugging support for file info data for each IR node
     curprint ( "\n/* Top of unparseExpression " + string(expr->sage_class_name()) + " */\n ");
     ROSE_ASSERT (expr->get_startOfConstruct() != NULL);
     curprint ( "/* startOfConstruct: file = " << expr->get_startOfConstruct()->get_filenameString() 
         + " raw filename = " << expr->get_startOfConstruct()->get_raw_filename() 
         + " raw line = " << expr->get_startOfConstruct()->get_raw_line() 
         + " raw column = " << expr->get_startOfConstruct()->get_raw_col() 
         + " */\n ");
     if (expr->get_endOfConstruct() != NULL)
        {
          curprint ( "/* endOfConstruct: file = " << expr->get_endOfConstruct()->get_filenameString()
              + " raw filename = " << expr->get_endOfConstruct()->get_raw_filename() 
              + " raw line = " << expr->get_endOfConstruct()->get_raw_line() 
              + " raw column = " << expr->get_endOfConstruct()->get_raw_col() 
              + " */\n ");
        }
#endif

  // DQ (10/25/2006): Debugging support for file info data for each IR node
#define OUTPUT_EMBEDDED_COLOR_CODES_FOR_EXPRESSIONS 0
#if OUTPUT_EMBEDDED_COLOR_CODES_FOR_EXPRESSIONS
     vector< pair<bool,std::string> > stateVector;
     if (get_embedColorCodesInGeneratedCode() > 0)
        {
          setupColorCodes ( stateVector );
          printColorCodes ( expr, true, stateVector );
        }
#endif

#if 0
  // Liao 11/2/2010 Skip the case that an expression is located from another file (included in the current file)
  // I moved the code to the unparser function for SgAggregatedInitializer to have bigger picture about what to parse or not
     SgFile* cur_file = SageInterface::getEnclosingFileNode(expr);
     if (cur_file != NULL)
     {
       // normal file info 
       if (expr->get_file_info()->isTransformation() == false &&  expr->get_file_info()->isCompilerGenerated() ==false)
       {
         if (cur_file->get_file_info()->get_filename() != expr->get_file_info()->get_filename())
           return;
       }
     }
#endif

     if ( unparseLineReplacement(expr,info) )
        {
          return;
        }

  // DQ (7/19/2008): This is the new code to unparse directives before the current expression
     unparseAttachedPreprocessingInfo(expr, info, PreprocessingInfo::before);

  // MS 2003: experimental backend source replacement
  // Either use the source string attached to the AST by a transformation
  // (and do not traverse the subtree with 'epxr' as its root node)
  // OR unparse the expression (the whole subtree)
  // if (expr->attribute.exists("_UnparserSourceReplacement"))
  // if (expr->get_attribute() != NULL && expr->attribute().exists("_UnparserSourceReplacement"))

  // DQ (7/20/2008): This is now revised to handle more cases than just replacement of the 
  // AST subtree with a string.  Now we can add arbitrary text into different locations
  // relative to the specific IR node.  For now we are supporting before, replace, and after.

  // TV (04/22/11): More generic support of Original Expression Tree. Introduce to support function pointer cast
  //     in function pointer initialization (test2006_160.C)
     SgExpression* expressionTree = NULL; //expr->get_originalExpressionTree();
     switch (expr->variantT()) {
       case V_SgVarRefExp:
       {
         expressionTree = isSgExpression(isSgVarRefExp(expr)->get_originalExpressionTree());
         break;
       }
       case V_SgCastExp:
       {
         expressionTree = isSgExpression(isSgCastExp(expr)->get_originalExpressionTree());
         break;
       }
       default:
       {
         SgFunctionRefExp * func_ref = isSgFunctionRefExp(expr);
         if (func_ref) {
           expressionTree = isSgExpression(func_ref->get_originalExpressionTree());
         }
       }
     }
     
     AstUnparseAttribute* unparseAttribute = dynamic_cast<AstUnparseAttribute*>(expr->getAttribute(AstUnparseAttribute::markerName));
     if (unparseAttribute != NULL)
        {
          string code = unparseAttribute->toString(AstUnparseAttribute::e_before);
          curprint (code);
        }
        
  // Only replace the unparsing of the IR node with a string if a string is marked as AstUnparseAttribute::e_replace.
     if (unparseAttribute != NULL && unparseAttribute->replacementStringExists() == true)
        {
          string code = unparseAttribute->toString(AstUnparseAttribute::e_replace);
          curprint (code);
        }
       else if (expressionTree != NULL && info.SkipConstantFoldedExpressions() == false)
        {
          unparseExpression(expressionTree, info);
        }
       else
        {
       // DQ (5/21/2004): revised need_paren handling in EDG/SAGE III and within SAGE III IR)
       // QY (7/9/2004): revised to use the new unp->u_sage->PrintStartParen test
          bool printParen = requiresParentheses(expr,info);
#if 0
          printf ("In unparseExpression(): expr = %p = %s printParen = %s \n",expr,expr->class_name().c_str(),printParen ? "true" : "false");
          curprint (string("\n/* In unparseExpression(): printParen = ") + (printParen ? "true" : "false") + " */ \n ");
#endif

#if 0
       // DQ (8/21/2005): Suppress comments when unparsing to build type names
          if ( !info.SkipComments() || !info.SkipCPPDirectives() )
             {
               string name = SageInterface::get_name(expr);
               SgFunctionCallExp* functionCall = isSgFunctionCallExp(expr);
               if (functionCall != NULL)
                  {
                    SgExpression* expression = functionCall->get_function();
                    SgDotExp* dotExp = isSgDotExp(expression);
                    if (dotExp != NULL)
                       {
                         SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(dotExp->get_lhs_operand());
                      // ROSE_ASSERT(memberFunctionRefExp != NULL);
                         if (memberFunctionRefExp != NULL)
                            {
                              SgMemberFunctionSymbol* memberFunctionSymbol = memberFunctionRefExp->get_symbol();
                              ROSE_ASSERT(memberFunctionSymbol != NULL);
                              name = memberFunctionSymbol->get_declaration()->get_name().str();
                            }
                           else
                            {
                              printf ("*** dotExp->get_lhs_operand() not processed: dotExp->get_lhs_operand() = %s \n",dotExp->get_lhs_operand()->class_name().c_str());
                            }
                       }
                      else
                       {
                         printf ("*** functionCall->get_function() not processed: functionCall->get_function() = %s \n",functionCall->get_function()->class_name().c_str());
                       }
                  }
                 else
                  {
                    printf ("*** expr not processed: expr = %s \n",expr->class_name().c_str());
                  }

            // curprint("\n /* In unparseExpression paren " + expr->class_name() + " paren printParen = " + (printParen ? "true" : "false") + " */ \n");
               curprint("\n /* In unparseExpression paren " + expr->class_name() + " name = " + name + " paren printParen = " + (printParen ? "true" : "false") + " */ \n");
             }
#endif

       // if (printParen)
       // ROSE_ASSERT(currentFile != NULL);
       // if ( (printParen == true) && (currentFile->get_Fortran_only() == false) )
          if (printParen == true)
             {
               if(isSgMatrixExp(expr))
                 {
                   printParen = false;
                 }
               else
                 {               
                   // Make sure this is not an expresion list
                   ROSE_ASSERT (isSgExprListExp(expr) == NULL);
#if 0
                   curprint ("/* In unparseExpression(): output ( */ \n ");
#endif
                   // Output the left paren
                   curprint ("(");
                 }
             }

       // DQ (10/7/2004): Definitions should never be unparsed within code generation for expressions
          if (info.SkipClassDefinition() == false)
             {
            // printf ("Skip output of class definition in unparseExpression \n");
            // DQ (10/8/2004): Skip all definitions when outputing expressions!
            // info.set_SkipClassDefinition();
            // info.set_SkipDefinition();
             }

       // DQ (10/13/2006): Remove output of qualified names from this level of generality!
       // DQ (12/22/2005): Output any name qualification that is required 
       // (we only explicitly store the global scope qualification since 
       // this is all that it seems that EDG stores).
       // unparseQualifiedNameList(expr->get_qualifiedNameList());

          switch (expr->variant())
             {
            // DQ (4/18/2013): I don't think this is ever called this way, IR node resolve to the derived classes not the base classes.
               case UNARY_EXPRESSION:
                  {
                    printf ("This should never be called: case UNARY_EXPRESSION\n");
                    ROSE_ASSERT(false);

                    unparseUnaryExpr (expr, info);
                    break; 
                  }

            // DQ (4/18/2013): I don't think this is ever called this way, IR node resolve to the derived classes not the base classes.
               case BINARY_EXPRESSION:
                  {
                    printf ("This should never be called: case BINARY_EXPRESSION\n");
                    ROSE_ASSERT(false);

                    unparseBinaryExpr(expr, info);
                    break;
                  }

             case MATRIX_EXP:
               {
                 unparseMatrixExp(expr, info);
                 break;
               }
                  
            // DQ (8/15/2007): This has been moved to the base class
               case EXPR_LIST: { unparseExprList(expr, info); break; }

            // DQ (7/31/2014): Adding support for C++11 nullptr const value expressions.
               case NULLPTR_VAL:

            // DQ: These cases are separated out so that we can handle the 
            // original expression tree from any possible constant folding by EDG.
               case BOOL_VAL:
               case SHORT_VAL:
               case CHAR_VAL:
               case UNSIGNED_CHAR_VAL:
               case WCHAR_VAL:
               case STRING_VAL:
               case UNSIGNED_SHORT_VAL:
               case ENUM_VAL:
               case INT_VAL:
               case UNSIGNED_INT_VAL:
               case LONG_INT_VAL:
               case LONG_LONG_INT_VAL:
               case UNSIGNED_LONG_LONG_INT_VAL:
               case UNSIGNED_LONG_INT_VAL:
               case FLOAT_VAL:
               case DOUBLE_VAL:
               case LONG_DOUBLE_VAL:
               case COMPLEX_VAL:
                  {
                    unparseValue(expr, info);
                    break;
                  }

               default:
                  {
                 // Call the derived class implementation for C, C++, or Fortran specific language unparsing.
                    unparseLanguageSpecificExpression(expr,info);

                    break;
                  }
      
             }

          if (printParen)
             {
#if 0
               curprint ("/* In unparseExpression(): output ) */ \n ");
#endif
            // Output the right paren
               curprint (")");
             }

       // calls the logical_unparse function in the sage files
       // expr->logical_unparse(info, curprint);
        } // unparse source replacement END

#if OUTPUT_EMBEDDED_COLOR_CODES_FOR_EXPRESSIONS
     if (get_embedColorCodesInGeneratedCode() > 0)
        {
          printColorCodes ( expr, false, stateVector );
        }
#endif

  // DQ (7/20/2008): Part of new support for unparsing arbitrary strings into the unparsed code.
     if (unparseAttribute != NULL)
        {
          string code = unparseAttribute->toString(AstUnparseAttribute::e_after);
          curprint (code);
        }

  // DQ (7/19/2008): This is the new code to unparse directives before the current expression
     unparseAttachedPreprocessingInfo(expr, info, PreprocessingInfo::after);

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES || 0
  // DQ (8/21/2005): Suppress comments when unparsing to build type names
     if ( !info.SkipComments() || !info.SkipCPPDirectives() )
        {
          printf ("Leaving unparse expression (%p): sage_class_name() = %s \n",expr,expr->sage_class_name());
       // unp->u->sage->curprint ( "\n/* Bottom of unparseExpression " << string(expr->sage_class_name()) << " */ \n");
          curprint ( "\n/* Bottom of unparseExpression " + expr->class_name() + " */ \n");
        }
#endif

#if 0
     printf ("Leaving unparse language independent expression(): info.SkipClassDefinition() = %s \n",(info.SkipClassDefinition() == true) ? "true" : "false");
     printf ("Leaving unparse language independent expression(): info.SkipEnumDefinition()  = %s \n",(info.SkipEnumDefinition() == true) ? "true" : "false");
#endif

  // DQ (9/9/2016): These should have been setup to be the same.
     ROSE_ASSERT(info.SkipClassDefinition() == info.SkipEnumDefinition());
   }


#if 0
void
UnparseLanguageIndependentConstructs::unparseLanguageSpecificStatement  (SgStatement* stmt,  SgUnparse_Info& info)
   {
     printf ("Error: base class implementation of unparseLanguageSpecificStatement called \n");
     ROSE_ASSERT(false);
   }
#endif
#if 0
void
UnparseLanguageIndependentConstructs::unparseLanguageSpecificExpression (SgExpression* expr, SgUnparse_Info& info)
   {
     printf ("Error: base class implementation of unparseLanguageSpecificExpression called \n");
     ROSE_ASSERT(false);
   }
#endif

void
UnparseLanguageIndependentConstructs::unparseNullStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
  // Nothing to do here! (unless we need a ";" or something)
     SgNullStatement* nullStatement = isSgNullStatement(stmt);
     ROSE_ASSERT(nullStatement != NULL);

#if 0
     printf ("In unparseNullStatement() stmt = %p \n",stmt);
#endif

  // Not much to do here except output a ";", not really required however.
  // if (!info.inConditional() && !info.SkipSemiColon())
     if (info.SkipSemiColon() == false)
        {
          curprint(";");
        }
       else
        {
#if 0
          printf ("In unparseNullStatement() stmt = %p (skipped output of semi-colon) info.inConditional() = %s info.SkipSemiColon() = %s \n",stmt,info.inConditional() ? "true" : "false",info.SkipSemiColon() ? "true" : "false");
#endif
        }
   }

void
UnparseLanguageIndependentConstructs::unparseNullExpression (SgExpression* expr, SgUnparse_Info& info)
   {
  // Nothing to do here! (unless we need a ";" or something)
#if 0
     printf ("In unparseNullExpression() expr = %p \n",expr);
#endif
   }


bool 
UnparseLanguageIndependentConstructs::isTransformed(SgStatement* stmt)
   {
  // This function must traverse the AST and look for any sign that 
  // the subtree has been transformed.  This might be a difficult 
  // function to write.  We might have to force transformations to
  // do something to make their presence better known (e.g. removing
  // a statement will leave no trace in the AST of the transformation).

  // DQ (3/2/2005): Change this to see if we can output each specialization 
  // as if we were transforming each template specialization
  // Assume no transformation at the moment while we debug templates.

  // DQ (6/29/2005): return false while we try to return to compiling KULL
#if 0
     return false;
#else
  // DQ (5/9/2017): Fix this to look at the statement (non-defining template instantiations are not being unparsed).
#if 0
     printf ("In UnparseLanguageIndependentConstructs::isTransformed(): stmt = %p = %s stmt->isTransformation() = %s \n",
          stmt,stmt->class_name().c_str(),(stmt->isTransformation() == true) ? "true" : "false");
#endif
     return stmt->isTransformation();
#endif
   }


void
UnparseLanguageIndependentConstructs::unparseGlobalStmt (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgGlobal* globalScope = isSgGlobal(stmt);
     ROSE_ASSERT(globalScope != NULL);

#if 0
     printf ("\n ***** Unparsing the global Scope ***** \n\n");
#endif

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES || 0
     printf ("global scope file = %s \n",TransformationSupport::getSourceFile(globalScope)->getFileName().c_str());
     printf ("global scope size = %ld \n",globalScope->get_declarations().size());
#endif

#if OUTPUT_HIDDEN_LIST_DATA
     outputHiddenListData (globalScope);
#endif

#if 0
     curprint(string(" /* In unparseGlobalStmt(): global scope size = ") + StringUtility::numberToString(globalScope->get_declarations().size()) + " */ \n ");
#endif

#if 0
     int declarationCounter = 0;
#endif

  // DQ (12/22/2014): We need to make sure that the last_statement is the last statement that had a token mapping.
  // SgSourceFile* sourceFile = isSgSourceFile(SageInterface::getEnclosingFileNode(stmt));
  // ROSE_ASSERT(sourceFile != NULL);
  // DQ (12/10/2014): Unparse the trailing whitespace at the end of the file (global scope).
     ROSE_ASSERT(globalScope->get_parent() != NULL);
     SgSourceFile* sourceFile = isSgSourceFile(globalScope->get_parent());

     ROSE_ASSERT(sourceFile != NULL);
     ROSE_ASSERT(info.get_current_source_file() != NULL);
     ROSE_ASSERT(sourceFile != NULL);

#if 0
     printf ("In unparseGlobalStmt(): info.get_current_source_file() = %p filename = %s \n",info.get_current_source_file(),info.get_current_source_file()->getFileName().c_str());
     printf ("In unparseGlobalStmt(): sourceFile                     = %p filename = %s \n",sourceFile,sourceFile->getFileName().c_str());
#endif

  // DQ (1/10/2014): Support new definition of the SgSourceFile via the SgUnparse_Info (verify it is the same, 
  // then we can eliminate the computation via the parent pointer).  This will be significantly more efficent
  // where we need a reference to the SgSourceFile in the unparsing of more general statements when using the 
  // token based unparsing.
  // ROSE_ASSERT(info.get_current_source_file() == sourceFile);
     if (info.get_current_source_file() != sourceFile)
        {
          printf ("WARNING: info.get_current_source_file() != sourceFile \n");
          printf ("   --- info.get_current_source_file() = %p filename = %s \n",info.get_current_source_file(),info.get_current_source_file()->getFileName().c_str());
          printf ("   --- sourceFile                     = %p filename = %s \n",sourceFile,sourceFile->getFileName().c_str());

          printf ("Return as a test! \n");

#if 1
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
          return;
        }

  // DQ (3/16/2015): This can be the SgGlobal that is in the SgProject (used for a larger concept fo global scope across multiple files).
  // In this case the globalScope->get_parent() is a SgProject. 
  // ROSE_ASSERT(sourceFile != NULL);

     if (sourceFile != NULL)
        {
          std::map<SgNode*,TokenStreamSequenceToNodeMapping*> & tokenStreamSequenceMap = sourceFile->get_tokenSubsequenceMap();
          if (sourceFile->get_unparse_tokens() == false)
             {
            // DQ (12/14/2015): Even if we don't unparse using the token stream, we can still use the 
            // token sequence mapping to build more accurate source position information in the AST.
            // ROSE_ASSERT(tokenStreamSequenceMap.size() == 0);
               if (sourceFile->get_use_token_stream_to_improve_source_position_info() == false)
                  {
                    if (tokenStreamSequenceMap.size() > 0)
                       {
                         printf ("Note: sourceFile->get_use_token_stream_to_improve_source_position_info() == false: but we have a valid tokenStreamSequenceMap.size() = %zu \n",tokenStreamSequenceMap.size());
                       }
                 // ROSE_ASSERT(tokenStreamSequenceMap.size() == 0);
                  }
             }
            else
             {
            // DQ (2/28/2015): This assertion will be false where the input is an empty file.
            // DQ (1/6/2015): If we are calling this function and sourceFile->get_unparse_tokens() == true, then globalScope->get_containsTransformation() == true.
            // ROSE_ASSERT(globalScope->get_containsTransformation() == true);
             }
#if 0
          printf ("In unparseGlobalStmt(): sourceFile->get_unparse_tokens() = %s \n",sourceFile->get_unparse_tokens() ? "true" : "false");
#endif
       // DQ (1/4/2015): Find the first statement so that we can unparse the tokens leading up to it.
          SgStatement* first_statement = NULL;
          if (sourceFile->get_unparse_tokens() == true)
             {
            // Setup an iterator to go through all the statements in the top scope of the file.
               SgDeclarationStatementPtrList & globalStatementList = globalScope->get_declarations();
               SgDeclarationStatementPtrList::iterator statementIterator = globalStatementList.begin();
#if 0
               int first_statement_declarationCounter = 0;
#endif
#if 0
               printf ("WARNING: This logic is causing the first few statement that are a part of a shared token stream to be skipped (see test2015_58.C) \n");
#endif
               while ( statementIterator != globalStatementList.end() )
                  {
                    SgStatement* currentStatement = *statementIterator;
                    ROSE_ASSERT(currentStatement != NULL);
#if 0
                    printf ("In unparseGlobalStmt(): currentStatement is %p = %s = %s \n",currentStatement,currentStatement->class_name().c_str(),SageInterface::get_name(currentStatement).c_str());
#endif
#if 0
                    printf ("In unparseGlobalStmt(): find first statement: first_statement_declaration #%d is %p = %s = %s \n",
                         first_statement_declarationCounter++,currentStatement,currentStatement->class_name().c_str(),SageInterface::get_name(currentStatement).c_str());
#endif
                 // DQ (1/16/2015): This logic is causing the first few statement that are a part of a shared token stream to be skipped (see test2015_58.C).
                 // DQ (12/22/2014): The stl semantics are allowing NULL pointers to get into the tokenStreamSequenceMap container.
                 // bool found_token_data = (tokenStreamSequenceMap.find(currentStatement) != tokenStreamSequenceMap.end());
                    bool found_token_data = (tokenStreamSequenceMap.find(currentStatement) != tokenStreamSequenceMap.end()) && tokenStreamSequenceMap[currentStatement] != NULL;
                    if (found_token_data == true && first_statement == NULL)
                       {
                         first_statement = currentStatement;
                       }

                 // Go to the next statement
                    statementIterator++;
                  }

               if (first_statement != NULL)
                  {
#if 0
                    printf ("In unparseGlobalStmt(): first_statement is %p = %s = %s \n",first_statement,first_statement->class_name().c_str(),SageInterface::get_name(first_statement).c_str());
#endif
#if 0
                    printf ("first_statement = %p = %s \n",first_statement,first_statement->class_name().c_str());
                    first_statement->get_file_info()->display("first_statement: debug");
#endif
                 // Unparse the leading part of the file's token stream up to the leading whitespace of the first statement to be unparsed.
                    unparseStatementFromTokenStream(globalScope, first_statement, e_token_subsequence_start, e_leading_whitespace_start, info);
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }

#if 0
               curprint(string(" /* In unparseGlobalStmt(): Done with output of first_statement = ") + StringUtility::numberToString(first_statement) + " */ \n ");
#endif
             }

       // DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
       // DQ (12/10/2014): This is used to support the token-based unparsing.
       // SgStatement* last_statement = NULL;

       // Setup an iterator to go through all the statements in the top scope of the file.
          SgDeclarationStatementPtrList & globalStatementList = globalScope->get_declarations();
          SgDeclarationStatementPtrList::iterator statementIterator = globalStatementList.begin();
          while ( statementIterator != globalStatementList.end() )
             {
               SgStatement* currentStatement = *statementIterator;
               ROSE_ASSERT(currentStatement != NULL);
#if 0
            // printf ("In unparseGlobalStmt(): declaration #%d is %p = %s = %s \n",declarationCounter++,currentStatement,currentStatement->class_name().c_str(),SageInterface::get_name(currentStatement).c_str());
               printf ("\nIn unparseGlobalStmt(): declaration = %p = %s = %s \n",currentStatement,currentStatement->class_name().c_str(),SageInterface::get_name(currentStatement).c_str());
#endif

               if (ROSE_DEBUG > 3)
                  {
                 // (*primary_os)
                    cout << "In run_unparser(): getLineNumber(currentStatement) = "
#if 1
                         << currentStatement->get_file_info()->displayString()
#else
                         << Rose::getLineNumber(currentStatement)
                         << " getFileName(currentStatement) = " 
                         << Rose::getFileName(currentStatement)
#endif
                         << " unp->cur_index = " 
                         << unp->cur_index
                         << endl;
                  }

            // DQ (6/4/2007): Make a new SgUnparse_Info object for each statement in global scope
            // This should permit children to set the current_scope and not effect other children
            // see test2007_56.C for example "namespace A { extern int x; } int A::x = 42;"
            // Namespace definition scope should not effect scope set in SgGlobal.
            // unparseStatement(currentStatement, info);
               SgUnparse_Info infoLocal(info);
               unparseStatement(currentStatement, infoLocal);

            // DQ (12/10/2014): Save the last statement.
            // last_statement = currentStatement;
               if (sourceFile->get_unparse_tokens() == true)
                  {
                 // DQ (12/22/2014): The stl semantics are allowing NULL pointers to get into the tokenStreamSequenceMap container.
                 // bool found_token_data = (tokenStreamSequenceMap.find(currentStatement) != tokenStreamSequenceMap.end());
                    bool found_token_data = (tokenStreamSequenceMap.find(currentStatement) != tokenStreamSequenceMap.end()) && tokenStreamSequenceMap[currentStatement] != NULL;
#if 0
                    printf ("In unparseGlobalStmt(): currentStatement = %p = %s found_token_data = %s \n",currentStatement,currentStatement->class_name().c_str(),found_token_data ? "true" : "false");
#endif
                    if (found_token_data == true)
                       {
                      // DQ (12/10/2016): Eliminating a warning that we want to be an error: -Werror=unused-but-set-variable.
                      // last_statement = currentStatement;
#if 0
                         TokenStreamSequenceToNodeMapping* tokenStreamSequence = tokenStreamSequenceMap[currentStatement];
                         ROSE_ASSERT(tokenStreamSequence != NULL);
                         tokenStreamSequence->display("In unparseGlobalStmt(): tokenStreamSequence");
#endif
                       }
                  }

            // Go to the next statement
               statementIterator++;
             }

#if 0
          curprint("/* Leaving unparseGlobalStmt(): unparse the trailing whitespace via the token stream */");
#endif

       // DQ (12/10/2014): Unparse the trailing whitespace at the end of the file (global scope).
       // SgSourceFile* sourceFile = isSgSourceFile(globalScope->get_parent());
       // ROSE_ASSERT(sourceFile != NULL);
          if (sourceFile->get_unparse_tokens() == true)
             {
            // DQ (12/26/2014): Handle case where last_statement == NULL.
            // ROSE_ASSERT(last_statement != NULL);
#if 0
               if (last_statement != NULL)
                    printf ("In UnparseLanguageIndependentConstructs::unparseGlobalStmt(): last_statement = %p = %s \n",last_statement,last_statement->class_name().c_str());
            // printf ("global_previous_unparsed_as: %s \n",unparsed_as_kind(global_previous_unparsed_as).c_str());
               printf ("global_unparsed_as:          %s \n",unparsed_as_kind(global_unparsed_as).c_str());
#endif
#if 0
            // DQ (1/7/2014): We want to have this logic in the unparseStatement() function so it will 
            // be in a single location instead of in each construct that contains a list of statements.
               if (globalScope->get_containsTransformation() == true)
                  {
                 // This has to be handled using the unparsing for the partial token stream (just the trailing whitespace at the end fo the file).
                    ROSE_ASSERT(globalScope->isTransformation() == false);

                  // DQ (12/26/2014): Handle case where last_statement == NULL.
                  // unparseStatementFromTokenStream (last_statement, globalScope, UnparseLanguageIndependentConstructs::e_trailing_whitespace_start, UnparseLanguageIndependentConstructs::e_token_subsequence_end);
                     if (last_statement != NULL)
                       {
#if 0
                         curprint("/* unparse the trailing tokens in the file */");
#endif
                         unparseStatementFromTokenStream (last_statement, globalScope, UnparseLanguageIndependentConstructs::e_trailing_whitespace_start, UnparseLanguageIndependentConstructs::e_token_subsequence_end);
                       }
                      else
                       {
                         printf ("NOTE: last_statement == NULL: skipped call to unparseStatementFromTokenStream(last_statement, globalScope) \n");
                       }
#if 0
                    printf ("In UnparseLanguageIndependentConstructs::unparseGlobalStmt(): unparse the last token as well \n");
#endif
#if 0
                    curprint(" /* unparse the last token in the file (commented out) */");
#endif
                 // Unparse the last token as well.
                    unparseStatementFromTokenStream (globalScope, UnparseLanguageIndependentConstructs::e_token_subsequence_end, UnparseLanguageIndependentConstructs::e_token_subsequence_end);
                  }
#endif
             }
            else
             {
            // DQ (12/10/2014): Moved the end of this funbction (only applies when sourceFile->get_unparse_tokens() == false).
            // DQ (4/21/2005): Output a new line at the end of the file (some compilers complain if this is not present)
               unp->cur.insert_newline(1);
             }
        }
       else
        {
       // DQ (3/16/2015): This can be the SgGlobal that is in the SgProject (used for a larger concept fo global scope across multiple files).
#if 0
          printf ("In unparseGlobalStmt(): globalScope->get_parent() = %p = %s \n",globalScope->get_parent(),globalScope->get_parent()->class_name().c_str());
#endif
          ROSE_ASSERT(isSgProject(globalScope->get_parent()) != NULL);
        }

  // DQ (5/27/2005): Added support for compiler-generated statements that might appear at the end of the applications
  // printf ("At end of unparseGlobalStmt \n");
  // outputCompilerGeneratedStatements(info);

#if 0
     curprint(string(" /* Leaving unparseGlobalStmt(): global scope size = ") + StringUtility::numberToString(globalScope->get_declarations().size()) + " */ \n ");
#endif

#if 0
     printf ("Leaving UnparseLanguageIndependentConstructs::unparseGlobalStmt() \n\n");
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

  // DQ (12/10/2014): Moved to the locate in the false block of if (sourceFile->get_unparse_tokens() == true).
  // DQ (4/21/2005): Output a new line at the end of the file (some compilers complain if this is not present)
  // unp->cur.insert_newline(1);
   }


void
UnparseLanguageIndependentConstructs::unparseFuncTblStmt(SgStatement* stmt, SgUnparse_Info& info)
   {
     SgFunctionTypeTable* functbl_stmt = isSgFunctionTypeTable(stmt);
     ROSE_ASSERT(functbl_stmt != NULL);

     stringstream  out;
     functbl_stmt->print_functypetable(out);
     curprint ( out.str());
   }


// DQ (8/13/2007): Who wrote this?  Why is not "basic_stmt->get_statements().size();" enough!
//--------------------------------------------------------------------------------
//  void Unparse_ExprStmt::num_stmt_in_block
//
//  returns the number of statements in the basic block
//--------------------------------------------------------------------------------  
int
UnparseLanguageIndependentConstructs::num_stmt_in_block(SgBasicBlock* basic_stmt)
   {
  // counter to keep number of statements in the block
     int num_stmt = 0;
     SgStatementPtrList::iterator p = basic_stmt->get_statements().begin();
     while (p != basic_stmt->get_statements().end())
        {
          num_stmt++;
          p++;
        }

     return num_stmt;
   }


bool
UnparseLanguageIndependentConstructs::unparseLineReplacement( SgLocatedNode* stmt, SgUnparse_Info& info )
   {
     int replacedLines = 0;
  // Get atached preprocessing info
     AttachedPreprocessingInfoType *prepInfoPtr= stmt->getAttachedPreprocessingInfo();
     if (prepInfoPtr == NULL)
        {
       // There's no preprocessing info attached to the current statement
       // printf ("No comments or CPP directives associated with this statement ... \n");
          return replacedLines;
        }

  // If we are skiping BOTH comments and CPP directives then there is nothing to do
     if ( info.SkipComments() && info.SkipCPPDirectives() )
        {
       // There's no preprocessing info attached to the current statement
       // printf ("Skipping output or comments and CPP directives \n");
          return replacedLines;
        }

#if 0
     info.display("In Unparse_ExprStmt::unparseLineReplacement()");
#endif

  // Traverse the container of PreprocessingInfo objects
     AttachedPreprocessingInfoType::iterator i;
     for(i = prepInfoPtr->begin(); i != prepInfoPtr->end(); ++i)
        {
       // i ist a pointer to the current prepInfo object, print current preprocessing info
       // Assert that i points to a valid preprocssingInfo object
          ROSE_ASSERT ( (*i) != NULL );
          ROSE_ASSERT ( (*i)->getTypeOfDirective()  != PreprocessingInfo::CpreprocessorUnknownDeclaration );
          ROSE_ASSERT ( (*i)->getRelativePosition() == PreprocessingInfo::before || 
                        (*i)->getRelativePosition() == PreprocessingInfo::after  ||
                        (*i)->getRelativePosition() == PreprocessingInfo::inside );

#if 0
          printf ("Stored comment: (*i)->getRelativePosition() = %s (*i)->getString() = %s \n",
             ((*i)->getRelativePosition() == PreprocessingInfo::before) ? "before" : "after", (*i)->getString().c_str());
#endif

       // Check and see if the info object would indicate that the statement would 
       // be printed, if not then don't print the comments associated with it.
       // These might have to be handled on a case by case basis.
       // bool infoSaysGoAhead = !info.SkipDefinition();
          bool infoSaysGoAhead = !info.SkipEnumDefinition() && !info.SkipClassDefinition() && !info.SkipFunctionDefinition();

       // DQ (7/19/2008): Allow expressions to have there associated comments unparsed.
          infoSaysGoAhead = (infoSaysGoAhead == true) || (isSgExpression(stmt) != NULL);

#if 0
          printf ("(*i)->getRelativePosition() == whereToUnparse (matches == %s) \n",(*i)->getRelativePosition() == whereToUnparse ? "true" : "false");
          printf ("unp->opt.get_unparse_includes_opt() == %s \n",(unp->opt.get_unparse_includes_opt() == true) ? "true" : "false");
#endif

          switch ( (*i)->getTypeOfDirective() )
             {
               case PreprocessingInfo::LineReplacement:
                  {
                    if (isSgExpression(stmt) == NULL ) //Do this when line replacement matches a whole statement
                         unp->cur.format(stmt, info, FORMAT_BEFORE_DIRECTIVE);

                    curprint ( (*i)->getString());
                 // unp->cur.format(stmt, info, FORMAT_AFTER_DIRECTIVE);

                    replacedLines++;
                    break;
                  }

               default:
                    break;
             }
        }

  // there should only be one linereplacement directive for a statement
     ROSE_ASSERT(replacedLines <= 1);

     return replacedLines;
   }


void
UnparseLanguageIndependentConstructs::unparseAttachedPreprocessingInfo(
// SgStatement* stmt,
   SgLocatedNode* stmt,
   SgUnparse_Info& info,
   PreprocessingInfo::RelativePositionType whereToUnparse)
   {
  // Get atached preprocessing info
     AttachedPreprocessingInfoType *prepInfoPtr = stmt->getAttachedPreprocessingInfo();

#if 0
  // Debugging added by DQ (only output debug information if comments/directives exist for this statement).
     if ( (prepInfoPtr != NULL) && (prepInfoPtr->empty() == false) )
        {
          printf ("\n\n");
          printf ("****************************************************************************** \n");
          printf ("In Unparse_ExprStmt::unparseAttachedPreprocessingInfo(%p = %s): whereToUnparse = %s \n",
               stmt,stmt->sage_class_name(),
               (whereToUnparse == PreprocessingInfo::before) ? "before" : "after");
          printf ("stmt->get_startOfConstruct() = %p stmt->get_endOfConstruct() = %p \n",stmt->get_startOfConstruct(),stmt->get_endOfConstruct());
          stmt->get_startOfConstruct()->display("startOfConstruct");
          if (stmt->get_endOfConstruct() != NULL)
             {
               stmt->get_endOfConstruct()->display("endOfConstruct");
             }
            else
             {
               printf ("Error in unparseAttachedPreprocessingInfo, stmt->get_endOfConstruct() == NULL: stmt = %p = %s = %s \n",
                    stmt,stmt->class_name().c_str(),SageInterface::get_name(stmt).c_str());
             }
          printOutComments(stmt);
        }
#endif

#if 0
     printOutComments(stmt);
#endif

     if (prepInfoPtr == NULL)
        {
       // There's no preprocessing info attached to the current statement
#if 0
          printf ("In Unparse_ExprStmt::unparseAttachedPreprocessingInfo(): prepInfoPtr == NULL: No comments or CPP directives associated with this statement ... \n");
#endif
          return;
        }

  // If we are skiping BOTH comments and CPP directives then there is nothing to do
     if ( info.SkipComments() && info.SkipCPPDirectives() )
        {
       // There's no preprocessing info attached to the current statement
#if 0
          printf ("Skipping output or comments and CPP directives \n");
#endif
          return;
        }

#if 0
     info.display("In Unparse_ExprStmt::unparseAttachedPreprocessingInfo()");
#endif

  // Traverse the container of PreprocessingInfo objects
     AttachedPreprocessingInfoType::iterator i;
     for (i = prepInfoPtr->begin(); i != prepInfoPtr->end(); ++i)
        {
       // i is a pointer to the current prepInfo object, print current preprocessing info
       // Assert that i points to a valid preprocssingInfo object
          ROSE_ASSERT ((*i) != NULL);
          ROSE_ASSERT ((*i)->getTypeOfDirective()  != PreprocessingInfo::CpreprocessorUnknownDeclaration);
          ROSE_ASSERT ((*i)->getRelativePosition() == PreprocessingInfo::before || 
                       (*i)->getRelativePosition() == PreprocessingInfo::after  ||
                       (*i)->getRelativePosition() == PreprocessingInfo::inside);

#if 0
       // DQ (1/28/2013): Fixed to use output of PreprocessingInfo::relativePositionName() and thus provide more accurate debug information.
          printf ("Stored comment: (*i)->getRelativePosition() = %s (*i)->getString() = %s \n",
            // ((*i)->getRelativePosition() == PreprocessingInfo::before) ? "before" : "after",
               PreprocessingInfo::relativePositionName((*i)->getRelativePosition()).c_str(),
               (*i)->getString().c_str());

       // DQ (2/27/2019): Adding support for multi-file handling.
          printf (" --- SgUnparse_Info: filename = %s \n",info.get_current_source_file()->getFileName().c_str());
          printf (" --- file_id = %d line = %d filename = %s \n",(*i)->getFileId(),(*i)->getLineNumber(),(*i)->getFilename().c_str());
#endif

       // Check and see if the info object would indicate that the statement would 
       // be printed, if not then don't print the comments associated with it.
       // These might have to be handled on a case by case basis.
       // bool infoSaysGoAhead = !info.SkipDefinition();
          bool infoSaysGoAhead = !info.SkipEnumDefinition()  &&
                                 !info.SkipClassDefinition() &&
                                 !info.SkipFunctionDefinition();
#if 0
          printf ("info.SkipEnumDefinition()     = %s \n",info.SkipEnumDefinition() ? "true" : "false");
          printf ("info.SkipClassDefinition()    = %s \n",info.SkipClassDefinition() ? "true" : "false");
          printf ("info.SkipFunctionDefinition() = %s \n",info.SkipFunctionDefinition() ? "true" : "false");
          printf ("Test 1: infoSaysGoAhead       = %s \n",infoSaysGoAhead ? "true" : "false");
#endif

#if 0
          printf ("Reset infoSaysGoAhead == true \n");
          infoSaysGoAhead = true;
#endif

       // DQ (7/19/2008): Allow expressions to have there associated comments unparsed.
       // Liao 11/9/2010: allow SgInitializedName also
       // negara1 (08/15/2011): Allow SgHeaderFileBody as well.
          infoSaysGoAhead = (infoSaysGoAhead == true) || (isSgExpression(stmt) != NULL) || (isSgInitializedName (stmt) != NULL) || (isSgHeaderFileBody(stmt) != NULL);

       // DQ (2/27/2019): Added assertions for debugging.
          ROSE_ASSERT(*i != NULL);

       // DQ (2/27/2019): Added assertions for debugging, for Cxx_tests/test2005_15.C (and many other files) this can be NULL.
       // ROSE_ASSERT(info.get_current_source_file() != NULL);
          bool isCommentFromCurrentFile = true;

          bool isSharedLocatedNode = (stmt->get_file_info()->isShared() == true);

       // DQ (3/12/2019): Only review the decission to reset infoSaysGoAhead if it is true.
       // if (info.get_current_source_file() != NULL)
       // if (infoSaysGoAhead == true && info.get_current_source_file() != NULL)
          if (isSharedLocatedNode == true && infoSaysGoAhead == true && info.get_current_source_file() != NULL)
             {
               ROSE_ASSERT(info.get_current_source_file()->get_file_info() != NULL);

            // DQ (2/27/2019): If this is a comment from a different file (not current file) then we can't unparse it here.
               isCommentFromCurrentFile = (info.get_current_source_file()->get_file_info()->get_file_id() == (*i)->getFileId());

               if (isCommentFromCurrentFile == false)
                  {
#if 0
                    printf ("Error: we can't unparse the current comment or CPP directive because it is from a different file: infoSaysGoAhead = %s \n",infoSaysGoAhead ? "true" : "false");
#endif
                 // DQ (3/2/2019): so when this fails for generated comments, what does the file info look like?
                 // (*i)->get_file_info()->display("so when this fails for generated comments, what does the file info look like");

                 // DQ (3/2/2019): I will alow this for now, but it is an inappropriate use of the Sg_File_Info object to define a file that does not exist.
                    if ( ((*i)->get_file_info()->get_filenameString() == "Compiler-Generated in PRE") ||
                         ((*i)->get_file_info()->get_filenameString() =="Compiler-Generated in Finite Differencing") ||
                         ((*i)->get_file_info()->isTransformation() == true) )
                       {
                      // Don't suppress the output of ROSE generated comments in this case.
                       }
                      else
                       {
                         infoSaysGoAhead = false;
#if 0
                         printf (" --- stmt = %p = %s \n",stmt,stmt->class_name().c_str());
                         printf (" --- stmt->get_file_info()->isShared() = %s \n",stmt->get_file_info()->isShared() ? "true" : "false");
                         printf (" --- Test 1.5: infoSaysGoAhead = %s \n",infoSaysGoAhead ? "true" : "false");
#endif
#if 0
                      // DQ (1/28/2013): Fixed to use output of PreprocessingInfo::relativePositionName() and thus provide more accurate debug information.
                         printf (" --- Stored comment: (*i)->getRelativePosition() = %s (*i)->getString() = %s \n",
                              PreprocessingInfo::relativePositionName((*i)->getRelativePosition()).c_str(),
                              (*i)->getString().c_str());

                      // DQ (2/27/2019): Adding support for multi-file handling.
                         printf (" --- --- SgUnparse_Info: filename = %s \n",info.get_current_source_file()->getFileName().c_str());
                         printf (" --- --- file_id = %d line = %d filename = %s \n",(*i)->getFileId(),(*i)->getLineNumber(),(*i)->getFilename().c_str());
#endif
                       }
                  }
             }

#if 0
          printf ("stmt = %p = %s \n",stmt,stmt->class_name().c_str());
          printf ("Test 2: infoSaysGoAhead = %s \n",infoSaysGoAhead ? "true" : "false");
#endif

#if 0
          static int counter = 0;
          counter++;
          if (counter > 3)
             {
               printf ("Exiting in Unparse_ExprStmt::unparseAttachedPreprocessingInfo() \n");
               ROSE_ABORT();
             }
#endif
       // DQ (2/5/2003):
       // The old directive handling allows all the test codes to parse properly, but
       // is not sufficent for handling the A++ transformations which are more complex.
       // I am trying to recover the old way which permitted all the test codes to work
       // before I fix it to be consistant with how it must work for the A++ transformation
       // to work properly.  This is work that was unfinished by Markus K. summer 2002.
       // (Though he did do a great job not enough information was provided in SAGE from EDG 
       // to finish it (I think)).

       // DQ (2/18/2003): Work to allow all CPP directives to be unparsed correctly on a statement
       //                 by statement basis has been completed, tested, and checked in.

#if 0
          printf ("(*i)->getRelativePosition() == whereToUnparse (matches == %s) \n",(*i)->getRelativePosition() == whereToUnparse ? "true" : "false");
          printf ("unp->opt.get_unparse_includes_opt() == %s \n",(unp->opt.get_unparse_includes_opt() == true) ? "true" : "false");
#endif
          if (infoSaysGoAhead && (*i)->getRelativePosition() == whereToUnparse)
             {
#if 0
               printf ("Calling unp->cur.format(FORMAT_BEFORE_DIRECTIVE): stmt = %p = %s \n",stmt,stmt->class_name().c_str());
#endif
               unp->cur.format(stmt, info, FORMAT_BEFORE_DIRECTIVE);
#if 0
               printf ("DONE: Calling unp->cur.format(): stmt = %p = %s \n",stmt,stmt->class_name().c_str());
#endif
            // DQ (7/19/2008): If we can assert this, then we can simpleify the code below!
            // It is turned on in the tests/nonsmoke/functional/roseTests/programTransformationTests/implicitCodeGenerationTest.C
            // But I still don't know what it does.
            // ROSE_ASSERT(unp->opt.get_unparse_includes_opt() == false);

               if (unp->opt.get_unparse_includes_opt() == true)
                  {
                 // DQ (9/16/2013): This is an error for C style comments spanning more than one line.
                 // To fix this just unparse the comment directly, since the syntax to make it a comment 
                 // is included in the string.
                 // Original comment: If we are unparsing the include files then we can simplify the 
                 // CPP directive processing and unparse them all as comments!
                 // Comments can also be unparsed as comments (I think!).
                 // curprint (  "// " + (*i)->getString());

                 // DQ (9/16/2013): New version of code.
                    switch ( (*i)->getTypeOfDirective() )
                       {
                      // Comments don't have to be further commented
                         case PreprocessingInfo::FortranStyleComment:
                         case PreprocessingInfo::F90StyleComment:
                         case PreprocessingInfo::C_StyleComment:
                         case PreprocessingInfo::CplusplusStyleComment:
                              if ( !info.SkipComments() )
                                 {
                                   curprint ( (*i)->getString());
                                 }
                              break;

                         default:
                            {
                              if ((*i)->getNumberOfLines() == 1)
                                 {
                                // DQ (9/16/2013): Commented out single line CPP directives is easy, so go ahead and do that.
                                // This used later style C comment syntax.  This permits the user to see the original CPP
                                // directives in a way that they will have no effect.
                                   curprint("// " + (*i)->getString());
                                 }
                                else
                                 {
                                // DQ (9/16/2013): Multi-line CPP directives are a bit more complex so ignore them.
                                   curprint("/* multi-line CPP directive ignored (no robust way to comment them out yet implemented) */\n");
                                 }
                            }
                       }
                  }
                 else
                  {
                 // DQ (1/28/2013): Fixed indentation of code block.
                    switch ( (*i)->getTypeOfDirective() )
                       {
                      // All #include directives are unparsed so that we can make the 
                      // output codes as similar as possible to the input codes. This also
                      // simplifies the debugging. On the down side it sets up a chain of 
                      // problems that force us to unparse most of the other directives 
                      // which makes the unparsing a bit more complex.
                         case PreprocessingInfo::CpreprocessorIncludeDeclaration:
                         case PreprocessingInfo::CpreprocessorIncludeNextDeclaration:
                              if ( !info.SkipComments() )
                                 {
                                   ROSE_ASSERT(unp->opt.get_unparse_includes_opt() == false);
#if 1
                                // DQ (9/16/2013): This is simpler code.
                                   curprint((*i)->getString());
#else
                                // DQ (9/16/2013): This predicate should be always false.
                                   if (unp->opt.get_unparse_includes_opt() == true)
                                        curprint ( string("// " ) + (*i)->getString());
                                     else
                                        curprint ( (*i)->getString());
#endif
                                 }
                              break;

                      // Comments don't have to be further commented
                         case PreprocessingInfo::FortranStyleComment:
                         case PreprocessingInfo::F90StyleComment:
                         case PreprocessingInfo::C_StyleComment:
                         case PreprocessingInfo::CplusplusStyleComment:
                              if ( !info.SkipComments() )
                                 {
                                   curprint ( (*i)->getString());
                                 }
                              break;

                      // extern declarations must be handled as comments since the EDG frontend strips them away
                         case PreprocessingInfo::ClinkageSpecificationStart:
                         case PreprocessingInfo::ClinkageSpecificationEnd:
                              if ( !info.SkipComments() )
                                 {
                                   if (unp->opt.get_unparse_includes_opt() == true)
                                        curprint (  string("// ") + (*i)->getString());
                                     else
                                        curprint ( (*i)->getString());
                                 }
                              break;

                      // Must unparse these because they could hide a #define 
                      // directive which would then be seen e.g.
                      //      #if 0
                      //      #define printf parallelPrintf
                      //      #endif
                      // So because we unparse the #define we must unparse 
                      // the #if, #ifdef, #else, and #endif directives.
                      // line declarations should also appear in the output 
                      // to permit the debugger to see the original code
                         case PreprocessingInfo::CpreprocessorIfdefDeclaration:
                         case PreprocessingInfo::CpreprocessorIfndefDeclaration:
                         case PreprocessingInfo::CpreprocessorIfDeclaration:
                      // Rama (08/17/07): Adding support so that pseudo-comments can be attached properly.
                         case PreprocessingInfo::CpreprocessorDeadIfDeclaration:
                         case PreprocessingInfo::CpreprocessorElseDeclaration:
                         case PreprocessingInfo::CpreprocessorElifDeclaration:
                         case PreprocessingInfo::CpreprocessorEndifDeclaration:
                         case PreprocessingInfo::CpreprocessorLineDeclaration:
                      // AS(120506) Added support for skipped tokens for Wave
                         case PreprocessingInfo::CSkippedToken: 
                              if ( !info.SkipComments() )
                                 {
                                   if (unp->opt.get_unparse_includes_opt() == true)
                                      {
                                        curprint ( string("// (previously processed: ignored) " ) + (*i)->getString());
                                      }
                                     else
                                      {
                                        curprint ( (*i)->getString());
                                      }
                                 }
                                else
                                 {
                                   curprint ( (*i)->getString());
                                 }
                              break;

                      // Comment out these declarations where they occur because we don't need
                      // them (they have already been evaluated by the front-end and would be
                      // redundent).
                         case PreprocessingInfo::CpreprocessorWarningDeclaration:
                         case PreprocessingInfo::CpreprocessorErrorDeclaration:
                         case PreprocessingInfo::CpreprocessorEmptyDeclaration:
                              if ( !info.SkipCPPDirectives() )
                                 {
                                // DQ (11/29/2006): Let's try to generate code which handles these better.
                                // curprint ( string("// (previously processed: ignored) " + (*i)->getString() ;
                                   curprint ( (*i)->getString());
                                 }
                              break;

                      // We skip commenting out these cases for the moment
                      // We must unparse these since they could control the path 
                      // taken in header files included separately e.g.
                      //      #define OPTIMIZE_ME
                      //      // optimization.h could include two paths dependent on the value of OPTIMIZE_ME
                      //      #include "optimization.h"
                         case PreprocessingInfo::CpreprocessorDefineDeclaration:
                         case PreprocessingInfo::CpreprocessorUndefDeclaration:
                              if ( !info.SkipCPPDirectives() )
                                 {
                                   if (unp->opt.get_unparse_includes_opt() == true)
                                      {
                                        curprint(string("// (previously processed: ignored) " ) + (*i)->getString());
                                      }
                                     else
                                      {
                                     // curprint((*i)->getString());

                                     // DQ (12/30/2013): Handle the case of a self referential macro declaration e.g. "#define foo  X->foo".
                                        if ((*i)->getTypeOfDirective() == PreprocessingInfo::CpreprocessorDefineDeclaration)
                                           {
#if 0
                                             printf ("In unparser: CPP macro = %s \n",(*i)->getString().c_str());
#endif
                                          // We need to supress the output of self-referential macros since they will be expanded 
                                          // twice in the back-end compilation of the ROSE generated code.
                                             bool isSelfReferential = (*i)->isSelfReferential();
#if 0
                                             printf ("In unparser: isSelfReferential = %s CPP macro = %s \n",isSelfReferential ? "true" : "false",(*i)->getString().c_str());
#endif
                                             if (isSelfReferential == true)
                                                {
#if 0
                                               // DQ (12/31/2013): Note that the final CR is a part of the CPP #define directive (so we don't need another one).
                                                  printf ("Detected self-referential macro (supressed in generated code) macro = %s ",(*i)->getString().c_str());
#endif
                                               // DQ (12/31/2013): We can't use /* */ to comment out the #define macro since it might also include a "/* ... */" substring.
                                               // curprint(string("/* (previously processed: ignoring self-referential macro declaration) " ) + (*i)->getString() + " */\n");
                                               // curprint(string("// (previously processed: ignoring self-referential macro declaration) " ) + (*i)->getString() + " \n");
#if 0
                                                  if ((*i)->getNumberOfLines() == 1)
                                                     {
                                                    // DQ (1/21/2014): This reports: "error: stray '#' in program"
                                                    // curprint(string("// (previously processed: ignoring self-referential macro declaration) " ) + (*i)->getString() + " \n");
                                                       curprint(string("// (previously processed: ignoring self-referential macro declaration) " ) + (*i)->getMacroName() + " \n");
                                                     }
                                                    else
                                                     {
                                                       ROSE_ASSERT((*i)->getNumberOfLines() > 1);
                                                       curprint(string("/* (previously processed: ignoring self-referential macro declaration) " ) + (*i)->getMacroName() + " */\n");
                                                     }
#else
                                               // DQ (1/21/2014): This has to be a C style comments for the options used in Valgrind compilation (not C99, I think).
                                                  curprint(string("/* (previously processed: ignoring self-referential macro declaration) macro name = " ) + (*i)->getMacroName() + " */ \n");
#endif
                                                }
                                               else
                                                {
                                                  curprint((*i)->getString());
                                                }
                                           }
                                          else
                                           {
                                             curprint((*i)->getString());
                                           }
                                      }
                                 }
                              break;

                         case PreprocessingInfo::CpreprocessorUnknownDeclaration:
                              printf ("Error: CpreprocessorUnknownDeclaration found \n");
                              ROSE_ABORT();
                              break;
                         case PreprocessingInfo::CMacroCall:
                           // AS(1/04/07) Macro rewrapping is currently not supported
                              break;
                         case PreprocessingInfo::CMacroCallStatement:
                              curprint ( (*i)->getString());
                              break;

                         case PreprocessingInfo::LineReplacement:
                              break;

                         case PreprocessingInfo::CpreprocessorIdentDeclaration:
                              curprint ( (*i)->getString());
                              break;

                         case PreprocessingInfo::CpreprocessorCompilerGeneratedLinemarker:
                              curprint ( (*i)->getString());
                              break;

                         default:
                              printf ("Error: default reached in switch in Unparse_ExprStmt::unparseAttachedPreprocessingInfo()\n");
                              ROSE_ABORT();
                       }
                  }

            // DQ (7/19/2008): Moved from outer nested scope level (below)
               unp->cur.format(stmt, info, FORMAT_AFTER_DIRECTIVE);
             }

       // DQ (7/19/2008): Moved to previous nested scope level
       // unp->cur.format(stmt, info, FORMAT_AFTER_DIRECTIVE);
        }

#if 0
     printf ("In unparseAttachedPreprocessingInfo(): stmt = %p = %s \n",stmt,stmt->class_name().c_str());
  // curprint ("\n /* Inside of unparseAttachedPreprocessingInfo() */ \n");
     curprint (string("/* Inside of unparseAttachedPreprocessingInfo() stmt = ") + stmt->class_name() + " */ \n");
#endif
   }



void
UnparseLanguageIndependentConstructs::unparseUnaryExpr(SgExpression* expr, SgUnparse_Info& info) 
   {
#if 0
     printf ("In unparseUnaryExpr(): expr = %p = %s \n",expr,expr->class_name().c_str());
     curprint ("\n /* Inside of unparseUnaryExpr */ \n");
     curprint (string("/* Inside of unparseUnaryExpr() expr = ") + expr->class_name() + " */ \n");
     unp->u_debug->printDebugInfo("entering unparseUnaryExpr", true);
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

     SgUnaryOp* unary_op = isSgUnaryOp(expr);
     ROSE_ASSERT(unary_op != NULL);

  // int toplevel_expression = !info.get_nested_expression();

     info.set_nested_expression();

  //
  // Flag to indicate whether the operand contains an overloaded arrow operator
  //
     bool arrow_op = false;
     arrow_op = unp->u_sage->NoDereference(expr);

#if 0
     printf ("In Unparse_ExprStmt::unparseUnaryExpr: unary_op->get_parent() = %s pointer \n",
          (unary_op->get_parent() != NULL) ? "VALID" : "NULL");
  // printf ("In Unparse_ExprStmt::unparseUnaryExpr: toplevel_expression = %d arrow_op = %d \n",toplevel_expression,arrow_op);
#endif

  // We have to test to see if the operand associated with this unary expression is a function pointer 
  // then we can use either function pointer calling syntax
  // (for void (*functionPointer) (int) as a declaration):
  //      1) functionPointer (x);
  //      2) (*functionPointer) (x);
  // Either is valid syntax (see Stroustrup (2nd Edition) section 7.7 "Pointer to Function" page 156.)
  // It seems that EDG and SAGE represent the two similarly, so we have to choose which format we 
  // want to have in the unparsing. Likely this should be an option to the unparser.

  // bool isFunctionType = (isSgFunctionType(unary_op->get_type()) != NULL) ? true : false;

#if 0
     printf ("unary_op->get_mode() == SgUnaryOp::prefix is  %s \n",(unary_op->get_mode() == SgUnaryOp::prefix)  ? "true" : "false");
     printf ("unary_op->get_mode() == SgUnaryOp::postfix is %s \n",(unary_op->get_mode() == SgUnaryOp::postfix) ? "true" : "false");
     printf ("In Unparse_ExprStmt::unparseUnaryExpr: arrow_op = %s \n",arrow_op ? "true" : "false");
  // printf ("isFunctionType = %s \n",(isFunctionType == true) ? "true" : "false");

     printf ("unary_op->get_operand()->class_name() = %s \n",unary_op->get_operand()->class_name().c_str());
     printf ("unary_op->get_type()->class_name()    = %s \n",unary_op->get_type()->class_name().c_str());
     printf ("info.get_operator_name()              = %s \n",info.get_operator_name().c_str());
#endif

  // DQ (2/22/2005): Ignoring if this is a SgFunctionType (test ...)
  // Bugfix (2/26/2001) If this is for a function pointer then skip printing out 
  // the operator name (for dereferencing operator)
#if 1
  // if (unary_op->get_mode() != SgUnaryOp::postfix && !arrow_op)
     if (unary_op->get_mode() != SgUnaryOp::postfix)
#else
#error "DEAD CODE!"
     if (unary_op->get_mode() != SgUnaryOp::postfix && !arrow_op && !isFunctionType)
#endif
        {
#if 0
          curprint ( "\n /* Unparsing a prefix unary operator */ \n");
#endif
       // DQ (2/25/2005): Trap case of SgPointerDerefExp so that "*" can't be 
       // turned into "/*" if preceeded by a SgDivideOp or overloaded "operator/()"
       // Put in an extra space so that if this happens we only generate "/ *"
       // test2005_09.C demonstrates this bug!
          if (isSgPointerDerefExp(expr) != NULL)
             {
               curprint(" ");
             }
          curprint(info.get_operator_name());
        }

#if 0
     printf ("In unparseUnaryExpr(): info.isPrefixOperator() = %s \n",info.isPrefixOperator() ? "true" : "false");
#endif

  // DQ (1/25/2014): Added support to avoid unparsing "- -5" as "--5".
     SgValueExp* valueExp = isSgValueExp(unary_op->get_operand());
     SgMinusOp* minus_op = isSgMinusOp(unary_op);
     if (minus_op != NULL && valueExp != NULL)
        {
       // We need to make sure we don't unparse: "- -5" as "--5".
       // I think we need an isNegative() query function so that we could refine this test to only apply to negative literals.
#if 0
          printf ("We need to make sure we don't unparse: \"- -5\" as \"--5\" \n");
#endif
          curprint(" ");
        }

#if 0
     curprint ("\n /* Calling unparseExpression from unparseUnaryExpr */ \n");
#endif
     unparseExpression(unary_op->get_operand(), info);
#if 0
     curprint ("\n /* DONE: Calling unparseExpression from unparseUnaryExpr */ \n");
#endif

     if (unary_op->get_mode() == SgUnaryOp::postfix && !arrow_op)
        {
#if 0
          curprint ( "\n /* Unparsing a postfix unary operator */ \n");
#endif
          curprint(info.get_operator_name());
        }

     info.unset_nested_expression();

#if 0
     curprint ("\n /* Leaving of unparseUnaryExpr */ \n");
#endif
   }


bool
UnparseLanguageIndependentConstructs::isDotExprWithAnonymousUnion(SgExpression* expr)
   {
  // DQ (1/23/2014): This function support detecting when the supress the output of the SgDotExp
  // in the access of data members from un-named unions.  Note that variables of type that are
  // un-named unions are given a unique generated name of the form "__anonymous_0x" as a 
  // prefix to the pointer value of the declaration that defines the un-named union.  The
  // handling here is not specific to unions and handles any type where the declaration is
  // using a generated name of this specific form.

     bool returnValue = false;

     SgDotExp* dotExp = isSgDotExp(expr);
     if (dotExp != NULL)
        {
          SgBinaryOp* binary_op = isSgBinaryOp(dotExp->get_lhs_operand());
          if (binary_op != NULL)
             {
               SgExpression* rhs = binary_op->get_rhs_operand();
               ROSE_ASSERT (rhs != NULL);
               SgVarRefExp* varRefExp = isSgVarRefExp(rhs);
               if (varRefExp != NULL)
                  {
                    bool isAnonymousName = (string(varRefExp->get_symbol()->get_name()).substr(0,14) == "__anonymous_0x");
#if 0
                    printf ("In isDotExprWithAnonymousUnion(): (hidden in SgBinaryOp): dotExp = %p isAnonymousName = %s \n",dotExp,isAnonymousName ? "true" : "false");
#endif
                   returnValue = isAnonymousName;
                  }
             }
            else
             {
            // The other case we have to handle is that the lhs is a SgVarRefExp to an un-named variable.  See test2014_152.C.
               SgVarRefExp* varRefExp = isSgVarRefExp(dotExp->get_lhs_operand());
               if (varRefExp != NULL)
                  {
                    bool isAnonymousName = (string(varRefExp->get_symbol()->get_name()).substr(0,14) == "__anonymous_0x");
#if 0
                    printf ("In isDotExprWithAnonymousUnion(): (hidden directly in the lhs operand): dotExp = %p isAnonymousName = %s \n",dotExp,isAnonymousName ? "true" : "false");
#endif
                    returnValue = isAnonymousName;
                  }
             }
        }

     return returnValue;
   }


bool
UnparseLanguageIndependentConstructs::isImplicitArrowExpWithinLambdaFunction(SgExpression* expr, SgUnparse_Info& info)
   {
     bool suppressOutputOfImplicitArrowExp = false;

#define DEBUG_IMPLICIT_ARROWEXP_WITHIN_LAMBDA 0

#if DEBUG_IMPLICIT_ARROWEXP_WITHIN_LAMBDA
     printf ("&&&&&&&&&&&&& In isImplicitArrowExpWithinLambdaFunction(): expr = %p = %s info.supressImplicitThisOperator = %s \n",expr,expr->class_name().c_str(),info.supressImplicitThisOperator() ? "true" : "false");
#endif
#if DEBUG_IMPLICIT_ARROWEXP_WITHIN_LAMBDA
     curprint (" /* &&&&&&&&&&&&& In isImplicitArrowExpWithinLambdaFunction() */ ");
#endif

     if (info.supressImplicitThisOperator() == true)
        {
          SgArrowExp* arrowExp = isSgArrowExp(expr);
          if (arrowExp != NULL)
             {
               SgExpression* lhs = arrowExp->get_lhs_operand();
               ROSE_ASSERT(lhs != NULL);

               SgThisExp* thisExp = isSgThisExp(lhs);
               if (thisExp != NULL)
                  {
                    if (thisExp->get_file_info()->isCompilerGenerated() == true)
                       {
#if DEBUG_IMPLICIT_ARROWEXP_WITHIN_LAMBDA
                         printf ("In isImplicitArrowExpWithinLambdaFunction(): found compiler generated this expression: thisExp = %p = %s \n",thisExp,thisExp->class_name().c_str());
#endif
                         suppressOutputOfImplicitArrowExp = true;
                       }
                  }

#if 0
            // DQ (7/23/2018): Need to support cases where the rhs is a valid operand (see test2018_85.C).
            // But we still want test2018_120.C to work properly.
               SgExpression* rhs = arrowExp->get_rhs_operand();
               if (rhs != NULL)
                  {
                 // suppressOutputOfImplicitArrowExp = false;
                    if (rhs->get_file_info()->isCompilerGenerated() == false)
                       {
#if DEBUG_IMPLICIT_ARROWEXP_WITHIN_LAMBDA
                         printf ("In isImplicitArrowExpWithinLambdaFunction(): found NON-compiler generated this expression: rhs = %p = %s \n",rhs,rhs->class_name().c_str());
#endif
                      // suppressOutputOfImplicitArrowExp = false;
                         suppressOutputOfImplicitArrowExp = false;
#if 0
                         printf ("found NON-compiler generated this expression: exiting as a test! \n");
                         ROSE_ASSERT(false);
#endif
                       }
                  }
#endif

            // DQ (11/20/2017): Added recursive step for chains of arrow operators (see C++11 test2017_29.C).
               SgArrowExp* nested_arrowExp = isSgArrowExp(lhs);
               if (nested_arrowExp != NULL)
                  {
#if DEBUG_IMPLICIT_ARROWEXP_WITHIN_LAMBDA
                    printf ("In isImplicitArrowExpWithinLambdaFunction(): detected nested arrow expression: nested_arrowExp = %p = %s \n",nested_arrowExp,nested_arrowExp->class_name().c_str());
#endif
                    suppressOutputOfImplicitArrowExp = isImplicitArrowExpWithinLambdaFunction(nested_arrowExp,info);

                 // DQ (2/12/2018): And if the rhs is not compiler generated then we want to return false.
                    SgExpression* nested_operator_rhs = nested_arrowExp->get_rhs_operand();
                    if (nested_operator_rhs->get_file_info()->isCompilerGenerated() == false)
                       {
// #if DEBUG_ARROW_OPERATOR_CHAIN
#if DEBUG_IMPLICIT_ARROWEXP_WITHIN_LAMBDA
                         printf ("In isImplicitArrowExpWithinLambdaFunction(): detected nested arrow expression: found NON compiler generated expression: nested_operator_rhs = %p = %s \n",
                              nested_operator_rhs,nested_operator_rhs->class_name().c_str());
#endif
                         suppressOutputOfImplicitArrowExp = false;
                       }
                  }
#if 0
            // DQ (7/24/2018): I now think this is a mistake to make this a recursive function.
            // DQ (11/20/2017): Added recursive step for chains of arrow operators (see C++11 test2017_29.C).
               SgCastExp* nested_cast = isSgCastExp(lhs);
               if (nested_cast != NULL)
                  {
#if DEBUG_IMPLICIT_ARROWEXP_WITHIN_LAMBDA
                    printf ("In isImplicitArrowExpWithinLambdaFunction(): detected nested cast expression: nested_cast = %p = %s \n",nested_cast,nested_cast->class_name().c_str());
#endif
                    if (nested_cast->get_file_info()->isCompilerGenerated() == true)
                       {
                         ROSE_ASSERT(nested_cast->get_operand() != NULL);
                         SgArrowExp* nested_arrowExp = isSgArrowExp(nested_cast->get_operand());
                         if (nested_arrowExp != NULL)
                            {
#if DEBUG_IMPLICIT_ARROWEXP_WITHIN_LAMBDA
                              printf ("In isImplicitArrowExpWithinLambdaFunction(): detected nested arrow expression behind cast: nested_arrowExp = %p = %s \n",nested_arrowExp,nested_arrowExp->class_name().c_str());
#endif
                              suppressOutputOfImplicitArrowExp = isImplicitArrowExpWithinLambdaFunction(nested_arrowExp,info);

#if DEBUG_IMPLICIT_ARROWEXP_WITHIN_LAMBDA
                              printf ("In isImplicitArrowExpWithinLambdaFunction(): detected nested arrow expression behind cast: nested_arrowExp = %p = %s suppressOutputOfImplicitArrowExp = %s \n",nested_arrowExp,nested_arrowExp->class_name().c_str(),suppressOutputOfImplicitArrowExp ? "true" : "false");
#endif
                            }
                       }
                  }
#endif
             }
        }

#if DEBUG_IMPLICIT_ARROWEXP_WITHIN_LAMBDA
     printf ("&&&&&&&&&&&&& Leaving isImplicitArrowExpWithinLambdaFunction(): expr = %p = %s suppressOutputOfImplicitArrowExp = %s \n",expr,expr->class_name().c_str(),suppressOutputOfImplicitArrowExp ? "true" : "false");
#endif
#if DEBUG_IMPLICIT_ARROWEXP_WITHIN_LAMBDA
     curprint (" /* &&&&&&&&&&&&& Leaving isImplicitArrowExpWithinLambdaFunction() */ ");
#endif

     return suppressOutputOfImplicitArrowExp;
   }


#if 0
// DQ (7/6/2014): A different version of this is in the unparseCxx_expressions.C file.
bool
partOfArrowOperatorChain(SgExpression* expr)
   {
#define DEBUG_ARROW_OPERATOR_CHAIN 0

#error "DEAD CODE!"

     SgBinaryOp* binary_op = isSgBinaryOp(expr);
     ROSE_ASSERT(binary_op != NULL);

     bool result = false;

#if DEBUG_ARROW_OPERATOR_CHAIN
     printf ("Inside of partOfArrowOperatorChain(): binary_op = %p = %s \n",binary_op,binary_op->class_name().c_str());
#endif

#error "DEAD CODE!"

  // DQ (4/9/2013): Added support for unparsing "operator+(x,y)" in place of "x+y".  This is 
  // required in places even though we have historically defaulted to the generation of the 
  // operator syntax (e.g. "x+y"), see test2013_100.C for an example of where this is required.
     SgNode* possibleParentFunctionCall = binary_op->get_parent();

#error "DEAD CODE!"

  // DQ (4/9/2013): This fails for test2006_92.C.
  // ROSE_ASSERT(possibleFunctionCall != NULL);
     bool parent_is_a_function_call                    = false;
     bool parent_function_call_uses_operator_syntax    = false;
     bool parent_function_is_overloaded_arrow_operator = false;
     bool parent_function_call_is_compiler_generated   = false;
     if (possibleParentFunctionCall != NULL)
        {

#error "DEAD CODE!"

          SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(possibleParentFunctionCall);
          if (functionCallExp != NULL)
             {
               parent_is_a_function_call                  = true;
               parent_function_call_uses_operator_syntax  = functionCallExp->get_uses_operator_syntax();
               parent_function_call_is_compiler_generated = functionCallExp->isCompilerGenerated();
#if 1
            // DQ (7/5/2014): Add code to detect use of overloaded "operator->" as a special case.
               SgExpression* rhs = binary_op->get_rhs_operand();
               bool isRelevantOverloadedOperator = false;
               SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(rhs);
               if (memberFunctionRefExp != NULL)
                  {
                    string functionName = memberFunctionRefExp->get_symbol()->get_name();
#if DEBUG_ARROW_OPERATOR_CHAIN
                    printf ("--- parent function is: functionName = %s \n",functionName.c_str());
#endif
                    if (functionName == "operator->")
                       {
                         parent_function_is_overloaded_arrow_operator = true;
                       }
                  }
#endif

#error "DEAD CODE!"

               if (parent_function_is_overloaded_arrow_operator == true)
                  {
                    SgExpression* expression = isSgExpression(functionCallExp->get_parent());
                    if (expression != NULL)
                       {
                         SgArrowExp* arrowExp = isSgArrowExp(expression);
                         if (arrowExp != NULL)
                            {
                              result = true;
                            }
                           else
                            {
                              result = partOfArrowOperatorChain(expression);
                            }
                       }
                      else
                       {
                         result = false;
                       }
#error "DEAD CODE!"

                  }
                 else
                  {
                    result = false;
                  }
             }
        }

#error "DEAD CODE!"

     return result;
   }
#endif



// DQ (4/14/2013): This is the new reimplemented version of the function (above).
void
UnparseLanguageIndependentConstructs::unparseBinaryExpr(SgExpression* expr, SgUnparse_Info& info) 
   {

#define DEBUG_BINARY_OPERATORS 0

     SgBinaryOp* binary_op = isSgBinaryOp(expr);
     ROSE_ASSERT(binary_op != NULL);

#if DEBUG_BINARY_OPERATORS
     curprint ( string("\n\n /* @@@@@ Inside of unparseBinaryExpr (operator name = ") + info.get_operator_name() + " */ \n");
     printf ("\n @@@@@ In unparseBinaryExpr(): expr = %p %s \n",expr,expr->class_name().c_str());
#endif

  // DQ (1/23/2014): Added better support for unparsing of data member access of un-named class (structs and unions) typed variables.
     bool suppressOutputOfDotExp           = isDotExprWithAnonymousUnion(expr);
     bool suppressOutputOfImplicitArrowExp = isImplicitArrowExpWithinLambdaFunction(expr,info);

  // DQ (4/15/2018): Fixup for Cxx11_tests/test2018_85.C
     SgArrowExp* arrowExp = isSgArrowExp(expr);
     if (suppressOutputOfImplicitArrowExp == true && arrowExp != NULL)
        {
          SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(arrowExp->get_rhs_operand());
          if (memberFunctionRefExp != NULL)
             {
            // If this is a member function, then we will need to include that reference to the calling class through the arrow operator.

            // DQ (7/5/2018): If we are going to force the output of the "->" operator, then we have to force the output of the lhs 
            // expression (see test2018_120.C) or the lhs of the lhs, ... unclear how to handle this more complex case.  Unless we 
            // explicitly search the lhs expression for the "this" operand so that we can unparse it when there is not a variable
            // reference expression as in test2018_85.C.  I would rather output the "this->" part of the unparsed expression than
            // skip it, however, the case in test2018_120.C has a compiler generated "this" and so it might be better to use this
            // as a test for if we should make this a special case and unparse the "->" operator.  This would be inconsistant with
            // the original code, but then the EDG AST does marks even the "this" expression as comnpiler generated and that is 
            // equivalent semantics, so that might be the best solution.  EDG just normalizes the code with respect to the source 
            // position information in this case.

#if 0
               printf ("In unparseBinaryExpr(): Set suppressOutputOfImplicitArrowExp = false: as special case of a member function reference: memberFunctionRefExp = %p = %s \n",memberFunctionRefExp,memberFunctionRefExp->class_name().c_str());
               curprint ( string("\n\n /* @@@@@ Inside of unparseBinaryExpr (operator name = ") + info.get_operator_name() + " : suppressOutputOfImplicitArrowExp set to false */ \n");
#endif
            // DQ (7/24/2018): Instead of making this an exception, we should try to handle this case better in test2018_85.C and test2018_130.C.
            // suppressOutputOfImplicitArrowExp = false;
             }
            else
             {
#if 0
               printf ("In unparseBinaryExpr(): arrowExp->get_rhs_operand() = %p = %s \n",arrowExp->get_rhs_operand(),arrowExp->get_rhs_operand()->class_name().c_str());
#endif
             }
        }

#if 0
  // DQ (2/12/2018): Debuging Cxx11_tests/test2018_10.C
     printf ("In unparseBinaryExpr(): suppressOutputOfImplicitArrowExp: expr = %p = %s suppressOutputOfImplicitArrowExp = %s \n",
          expr,expr->class_name().c_str(),suppressOutputOfImplicitArrowExp ? "true" : "false");
     curprint ("/* In unparseBinaryExpr(): binary_op = " + StringUtility::numberToString(binary_op) + " = " + binary_op->class_name() + " lhs = " + binary_op->get_lhs_operand()->class_name() + " */\n ");
     curprint ("/* In unparseBinaryExpr(): binary_op = " + StringUtility::numberToString(binary_op) + " = " + binary_op->class_name() + " rhs = " + binary_op->get_rhs_operand()->class_name() + " */\n ");
     curprint ("/* In unparseBinaryExpr(): suppressOutputOfImplicitArrowExp: suppressOutputOfImplicitArrowExp = " + string(suppressOutputOfImplicitArrowExp ? "true" : "false") + " */\n");
  // suppressOutputOfImplicitArrowExp = false;
  // suppressOutputOfImplicitArrowExp = false;
#endif

#if DEBUG_BINARY_OPERATORS
  // printf ("In Unparse_ExprStmt::unparseBinaryExpr() expr = %s \n",expr->sage_class_name());
     curprint ( string("\n /* Inside of unparseBinaryExpr (expr class name        = ") + StringUtility::numberToString(binary_op) + " = " + binary_op->class_name() + " */ \n");
     curprint ( string("\n /*                              lhs class name         = ") + StringUtility::numberToString(binary_op->get_lhs_operand()) + " = " + binary_op->get_lhs_operand()->class_name() + " */ \n");
     curprint ( string("\n /*                              rhs class name         = ") + StringUtility::numberToString(binary_op->get_rhs_operand()) + " = " + binary_op->get_rhs_operand()->class_name() + " */ \n");
     curprint ( string("\n /*                              suppressOutputOfDotExp = ") + (suppressOutputOfDotExp ? "true" : "false") + " */ \n");
#endif

  // DQ (4/9/2013): Added support for unparsing "operator+(x,y)" in place of "x+y".  This is 
  // required in places even though we have historically defaulted to the generation of the 
  // operator syntax (e.g. "x+y"), see test2013_100.C for an example of where this is required.
     SgNode* possibleParentFunctionCall = binary_op->get_parent();

  // DQ (4/9/2013): This fails for test2006_92.C.
  // ROSE_ASSERT(possibleFunctionCall != NULL);
     bool parent_is_a_function_call                    = false;
     bool parent_function_call_uses_operator_syntax    = false;
//   bool parent_function_is_overloaded_arrow_operator = false;
     bool parent_function_call_is_compiler_generated   = false;
     if (possibleParentFunctionCall != NULL)
        {
          SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(possibleParentFunctionCall);
          if (functionCallExp != NULL)
             {
               parent_is_a_function_call                  = true;
               parent_function_call_uses_operator_syntax  = functionCallExp->get_uses_operator_syntax();
               parent_function_call_is_compiler_generated = functionCallExp->isCompilerGenerated();
#if 1
            // DQ (7/5/2014): Add code to detect use of overloaded "operator->" as a special case.
               SgExpression* rhs = binary_op->get_rhs_operand();
            // bool isRelevantOverloadedOperator = false;
               SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(rhs);
               if (memberFunctionRefExp != NULL)
                  {
                    string functionName = memberFunctionRefExp->get_symbol()->get_name();
#if DEBUG_BINARY_OPERATORS
                    printf ("--- parent function is: functionName = %s \n",functionName.c_str());
#endif
#if 0
                    if (functionName == "operator->")
                       {
                         parent_function_is_overloaded_arrow_operator = true;
                       }
#endif
                  }
#endif
             }
        }

  // bool isPartOfArrowOperatorChain = partOfArrowOperatorChain(binary_op);

#if DEBUG_BINARY_OPERATORS
  // printf ("In unparseBinaryExpr(): isPartOfArrowOperatorChain                   = %s \n",isPartOfArrowOperatorChain ? "true" : "false");
     printf ("In unparseBinaryExpr(): suppressOutputOfDotExp                       = %s \n",suppressOutputOfDotExp     ? "true" : "false");
  // printf ("In unparseBinaryExpr(): parent_function_is_overloaded_arrow_operator = %s \n",parent_function_is_overloaded_arrow_operator ? "true" : "false");
#endif

  // DQ (4/13/13): Checking the current level function call expression.
     SgNode* possibleFunctionCall = binary_op->get_lhs_operand();
     ROSE_ASSERT(possibleFunctionCall != NULL);
     bool is_currently_a_function_call = false;
     bool current_function_call_uses_operator_syntax = false;
     bool current_function_call_is_compiler_generated = false;
     if (possibleFunctionCall != NULL)
        {
          SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(possibleFunctionCall);
          if (functionCallExp != NULL)
             {
               is_currently_a_function_call                = true;
               current_function_call_uses_operator_syntax  = functionCallExp->get_uses_operator_syntax();
               current_function_call_is_compiler_generated = functionCallExp->isCompilerGenerated();
             }
        }

#if DEBUG_BINARY_OPERATORS
     printf ("In unparseBinaryExpr(): BEFORE resetting current_function_call_uses_operator_syntax: current_function_call_uses_operator_syntax = %s \n",
          current_function_call_uses_operator_syntax  == true ? "true" : "false");
     printf ("In unparseBinaryExpr(): BEFORE resetting current_function_call_uses_operator_syntax: unp->opt.get_overload_opt()                = %s \n",
          unp->opt.get_overload_opt() == true ? "true" : "false");
#endif

  // If unp->opt.get_overload_opt() == true then use the overloaded operator names uniformally (Note that this is not well tested).
     current_function_call_uses_operator_syntax = ( (current_function_call_uses_operator_syntax == true) && !(unp->opt.get_overload_opt()) );

#if 0
  // Setting this to call or true should reproduce the older default behavior in ROSE.
     parent_function_call_uses_operator_syntax  = false;
     current_function_call_uses_operator_syntax = false;
#endif
#if 0
  // Setting this to call or true should reproduce the older default behavior in ROSE.
     parent_function_call_uses_operator_syntax  = true;
     current_function_call_uses_operator_syntax = true;
#endif
#if DEBUG_BINARY_OPERATORS
     printf ("In unparseBinaryExpr(): binary_op = %p = %s isCompilerGenerated() = %s \n",binary_op,binary_op->class_name().c_str(),binary_op->isCompilerGenerated() == true ? "true" : "false");
     printf ("In unparseBinaryExpr(): parent_is_a_function_call                    = %s \n",parent_is_a_function_call                   == true ? "true" : "false");
     printf ("In unparseBinaryExpr(): parent_function_call_uses_operator_syntax    = %s \n",parent_function_call_uses_operator_syntax   == true ? "true" : "false");
     printf ("In unparseBinaryExpr(): parent_function_call_is_compiler_generated   = %s \n",parent_function_call_is_compiler_generated  == true ? "true" : "false");
     printf ("In unparseBinaryExpr(): is_currently_a_function_call                 = %s \n",is_currently_a_function_call                == true ? "true" : "false");
     printf ("In unparseBinaryExpr(): current_function_call_uses_operator_syntax   = %s \n",current_function_call_uses_operator_syntax  == true ? "true" : "false");
     printf ("In unparseBinaryExpr(): current_function_call_is_compiler_generated  = %s \n",current_function_call_is_compiler_generated == true ? "true" : "false");
#endif

  // DQ (2/7/2011): Unparser support for more general originalExpressionTree handling.
     SgExpression* expressionTree = binary_op->get_originalExpressionTree();
     if (expressionTree != NULL && info.SkipConstantFoldedExpressions() == false)
        {
#if DEBUG_BINARY_OPERATORS
          printf ("Found and expression tree representing a cast expression (unfolded constant expression requiring a cast) expressionTree = %p = %s \n",
               expressionTree,expressionTree->class_name().c_str());
#endif
          unparseExpression(expressionTree,info);

          return;
        }

     info.set_nested_expression();

     if (info.get_operator_name() == "[]" )
        {
       // Special case:

       // DQ (4/14/2013): This likely requires some extra support where the operator syntax is not being used, but for
       // now this operator is always unparsed using it's operator syntax instead of using the overloaded operator name.
       // This needs to be fixed later.
#if DEBUG_BINARY_OPERATORS
          printf ("In unparseBinaryExp(): Special case of operator[] found \n");
          curprint ("/* Special case of operator[] found */\n");
#endif
          unparseExpression(binary_op->get_lhs_operand(),info);
          curprint ("[") ;
          unparseExpression(binary_op->get_rhs_operand(),info);
          curprint ("]");
        }
       else
        {
       // This is the more general case (supporting both infix, prefix, and postfix operators.
       // DQ (4/14/2013): I think that postfix operators and handled using specific mechanims and may not be well tested.
#if DEBUG_BINARY_OPERATORS
          printf ("In unparseBinaryExp(): Case 1 \n");
          curprint ( "/* NOT a special case of operator[] */\n");
#endif
          if (current_function_call_uses_operator_syntax == true)
             {
#if DEBUG_BINARY_OPERATORS
               printf ("In unparseBinaryExp(): Case 1.1 \n");
#endif
            // printf ("overload option is turned off! (output as "A+B" instead of "A.operator+(B)") \n");
            // First check if the right hand side is an unary operator function.
#if DEBUG_BINARY_OPERATORS
               curprint ( string("\n /* output as A+B instead of A.operator+(B): (u_sage->isUnaryOperator(binary_op->get_rhs_operand())) = ") + 
                    ((unp->u_sage->isUnaryOperator(binary_op->get_rhs_operand())) ? "true" : "false") + " */ \n");
#endif
            // if (unp->u_sage->isUnaryOperator(binary_op->get_rhs_operand())
               if (unp->u_sage->isUnaryOperator(binary_op->get_rhs_operand()) == true)
                  {
                 // printf ("Found case of rhs being a unary operator! \n");
#if DEBUG_BINARY_OPERATORS
                    printf ("In unparseBinaryExp(): Case 1.1.1 \n");
#endif
                    if (unp->u_sage->isUnaryPostfixOperator(binary_op->get_rhs_operand()))  // Postfix unary operator.
                       {
                      // ... nothing to do here (output the operator later!) ???
                      // printf ("... nothing to do here (output the postfix operator later!) \n");
#if DEBUG_BINARY_OPERATORS
                         printf ("In unparseBinaryExp(): Case 1.1.1.1 \n");
#endif
                       }
                      else 
                       {
                      // Prefix unary operator.
#if DEBUG_BINARY_OPERATORS
                         printf ("In unparseBinaryExp(): Case 1.1.1.2 \n");
#endif
#if DEBUG_BINARY_OPERATORS
                      // printf ("Handle prefix operator ... \n");
                         printf ("Prefix unary operator: Output the RHS operand ... = %s \n",binary_op->get_rhs_operand()->sage_class_name());
                         curprint("\n /* Prefix unary operator: Output the RHS operand ... */ \n");
#endif
                         if (info.isPrefixOperator() == false)
                            {
#if DEBUG_BINARY_OPERATORS
                              printf ("In unparseBinaryExp(): info.isPrefixOperator() == false: reset to be true! \n");
#endif
                              info.set_prefixOperator();
                            }
#if DEBUG_BINARY_OPERATORS
                         printf ("In unparseBinaryExpr(): info.isPrefixOperator() = %s \n",info.isPrefixOperator() ? "true" : "false");
#endif
                         unparseExpression(binary_op->get_rhs_operand(), info);
                       }
                  }
                 else
                  {
#if DEBUG_BINARY_OPERATORS
                    printf ("In unparseBinaryExp(): Case 1.1.2 binary_op->get_rhs_operand() is NOT a unary operator (skipping output) \n");
#endif
                  }
             }
            else
             {
               ROSE_ASSERT(current_function_call_uses_operator_syntax == false);
                
#if DEBUG_BINARY_OPERATORS
               printf ("In unparseBinaryExp(): Case 1.2 \n");
#endif
#if DEBUG_BINARY_OPERATORS
               printf ("In unparseBinaryExp(): parent_function_call_uses_operator_syntax                     = %s \n",parent_function_call_uses_operator_syntax == true ? "true" : "false");
               printf ("In unparseBinaryExp(): SageInterface::isPrefixOperator(binary_op->get_rhs_operand()) = %s \n",SageInterface::isPrefixOperator(binary_op->get_rhs_operand()) == true ? "true" : "false");
               printf ("In unparseBinaryExp(): current_function_call_uses_operator_syntax                    = %s (unhandled case) \n",current_function_call_uses_operator_syntax == true ? "true" : "false");
               printf ("In unparseBinaryExp(): binary_op->get_rhs_operand()                      = %p = %s \n",binary_op->get_rhs_operand(),binary_op->get_rhs_operand()->class_name().c_str());
#endif
            // DQ (4/13/2013): Adding support for prefix operators.
               if ( (parent_function_call_uses_operator_syntax == true) && (SageInterface::isPrefixOperator(binary_op->get_rhs_operand()) == true) )
                  {
#if DEBUG_BINARY_OPERATORS
                    printf ("In unparseBinaryExp(): Case 1.2.1 \n");
#endif
#if 0
                    printf ("Special handling for overloaded prefix operators \n");
                    ROSE_ASSERT(false);
#endif
#if DEBUG_BINARY_OPERATORS
                    curprint ( "\n /* unparseBinaryExpr(): Test 15  before unparseExpression() binary_op->get_rhs_operand() = " + binary_op->get_rhs_operand()->class_name() + "*/ \n");
#endif
                 // unparseExpression(binary_op->get_rhs_operand(), info);

                 // Mark this as a prefix operator so that unparseMFuncRefSupport() will know to unparse the operator name.
                    SgUnparse_Info newinfo(info);
                    newinfo.set_prefixOperator();

                    unparseExpression(binary_op->get_rhs_operand(), newinfo);
#if DEBUG_BINARY_OPERATORS
                    curprint ( "\n /* unparseBinaryExpr(): Test 16  after unparseExpression() binary_op->get_rhs_operand() = " + binary_op->get_rhs_operand()->class_name() + "*/ \n");
#endif
                  }
             }

#if DEBUG_BINARY_OPERATORS
          printf ("DONE with possible prefix operator processing expr = %p = %s \n",expr,expr->class_name().c_str());
#endif

#if DEBUG_BINARY_OPERATORS
          printf ("parent_function_call_uses_operator_syntax  = %s \n",parent_function_call_uses_operator_syntax  ? "true" : "false");
          printf ("current_function_call_uses_operator_syntax = %s \n",current_function_call_uses_operator_syntax ? "true" : "false");
          printf ("unp->opt.get_this_opt()                    = %s \n",unp->opt.get_this_opt()                    ? "true" : "false");
          printf ("unp->opt.get_overload_opt()                = %s \n",unp->opt.get_overload_opt()                ? "true" : "false");
          printf ("expr                          = %p = %s \n",expr,expr->class_name().c_str());
#endif

#if DEBUG_BINARY_OPERATORS
          curprint ("/* STARTING LHS: Calling unparseExpression(): " + StringUtility::numberToString(binary_op) + " = " + binary_op->class_name() + " lhs = " + binary_op->get_lhs_operand()->class_name() + " */\n");
          printf ("STARTING LHS: Calling unparseExpression(): for LHS = %p = %s \n",binary_op->get_lhs_operand(),binary_op->get_lhs_operand()->class_name().c_str());
          printf ("STARTING LHS: Calling unparseExpression(): suppressOutputOfImplicitArrowExp = %s \n",suppressOutputOfImplicitArrowExp ? "true" : "false");
          curprint ("/* STARTING LHS: Calling unparseExpression(): suppressOutputOfImplicitArrowExp = " + string(suppressOutputOfImplicitArrowExp ? "true" : "false") + " */\n");
#endif

       // DQ (9/3/2014): Adding support to supress the output if this operators in lambda functions.
       // unparseExpression(binary_op->get_lhs_operand(), info);
          if (suppressOutputOfImplicitArrowExp == false)
             {
            // DQ (7/12/2018): Check if this is a member function and if it is from a compiler generated variable and then if "this" should be output.
#if 0
               printf ("NOTE: suppressOutputOfImplicitArrowExp == false: unparse the binary_op->get_lhs_operand() = %p = %s \n",binary_op->get_lhs_operand(),binary_op->get_lhs_operand()->class_name().c_str());
#endif
#if 0
           // Test for if this should be output by calling: 
               bool inner_suppressOutputOfImplicitArrowExp = isImplicitArrowExpWithinLambdaFunction(binary_op->get_lhs_operand(),info);
#if 0
               printf ("################# inner_suppressOutputOfImplicitArrowExp = %s \n",inner_suppressOutputOfImplicitArrowExp ? "true" : "false");
#endif
#endif
               unparseExpression(binary_op->get_lhs_operand(), info);

#if 0
               printf ("DONE: NOTE: suppressOutputOfImplicitArrowExp == false: unparse the binary_op->get_lhs_operand() = %p = %s \n",binary_op->get_lhs_operand(),binary_op->get_lhs_operand()->class_name().c_str());
#endif
#if 0
               printf ("NOTE: suppressOutputOfImplicitArrowExp == false: exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }

#if DEBUG_BINARY_OPERATORS
          curprint ("/* FINISHED LHS: Calling unparseExpression(): binary_op = " + StringUtility::numberToString(binary_op) + " = " + binary_op->class_name() + " lhs = " + binary_op->get_lhs_operand()->class_name() + " */\n ");
          printf ("FINISHED LHS: Calling unparseExpression(): binary_op = %p = %s for LHS = %p = %s \n",binary_op,binary_op->class_name().c_str(),binary_op->get_lhs_operand(),binary_op->get_lhs_operand()->class_name().c_str());
#endif

       // if (SageInterface::isPrefixOperator(binary_op->get_rhs_operand()) == true)
          if ( (parent_function_call_uses_operator_syntax == true) && (SageInterface::isPrefixOperator(binary_op->get_rhs_operand()) == true) )
             {
#if DEBUG_BINARY_OPERATORS
               printf ("In unparseBinaryExp(): Leaving after output of prefix operator and lhs in Case 1 \n");
#endif
               return;
             }

#if DEBUG_BINARY_OPERATORS
          printf ("In unparseBinaryExpr(): binary_op->get_rhs_operand()  = %p = %s \n",binary_op->get_rhs_operand(),binary_op->get_rhs_operand()->class_name().c_str());
          printf ("unp->u_sage->isOperator(binary_op->get_rhs_operand()) = %s \n",unp->u_sage->isOperator(binary_op->get_rhs_operand()) ? "true" : "false");
#endif
       // Before checking to insert a newline to prevent linewrapping, check that this
       // expression is a primitive operator and not dot or arrow expressions.
#if DEBUG_BINARY_OPERATORS
          curprint ( string("\n/* output info.get_operator_name() = ") + info.get_operator_name() + " */ \n");
          curprint ( string("\n/*    --- current_function_call_uses_operator_syntax = ") + (current_function_call_uses_operator_syntax ? "true" : "false") + " */ \n");
          curprint ( string("\n/*    --- parent_function_call_uses_operator_syntax  = ") + (parent_function_call_uses_operator_syntax ? "true" : "false") + " */ \n");
#endif
#if DEBUG_BINARY_OPERATORS
       // printf ("parent_function_call_uses_operator_syntax  = %s \n",parent_function_call_uses_operator_syntax ? "true" : "false");
       // printf ("current_function_call_uses_operator_syntax = %s \n",current_function_call_uses_operator_syntax ? "true" : "false");

          printf ("In unparseBinaryExpr(): (after LHS) binary_op = %p = %s isCompilerGenerated() = %s \n",binary_op,binary_op->class_name().c_str(),binary_op->isCompilerGenerated() == true ? "true" : "false");
          printf ("In unparseBinaryExpr(): (after LHS) suppressOutputOfDotExp                       = %s \n",suppressOutputOfDotExp                      == true ? "true" : "false");
          printf ("In unparseBinaryExpr(): (after LHS) parent_is_a_function_call                    = %s \n",parent_is_a_function_call                   == true ? "true" : "false");
          printf ("In unparseBinaryExpr(): (after LHS) parent_function_call_uses_operator_syntax    = %s \n",parent_function_call_uses_operator_syntax   == true ? "true" : "false");
          printf ("In unparseBinaryExpr(): (after LHS) parent_function_call_is_compiler_generated   = %s \n",parent_function_call_is_compiler_generated  == true ? "true" : "false");
          printf ("In unparseBinaryExpr(): (after LHS) is_currently_a_function_call                 = %s \n",is_currently_a_function_call                == true ? "true" : "false");
          printf ("In unparseBinaryExpr(): (after LHS) current_function_call_uses_operator_syntax   = %s \n",current_function_call_uses_operator_syntax  == true ? "true" : "false");
          printf ("In unparseBinaryExpr(): (after LHS) current_function_call_is_compiler_generated  = %s \n",current_function_call_is_compiler_generated == true ? "true" : "false");
#endif

#if 0
       // DQ (1/22/2014): Look ahead to see if this is a SgVarRefExp of a variable with a generated name.
       // We can't support this approach.  We have to look from the SgDotExp down to see if their is a 
       // variable reference to a variables named "__anonymous_0x" so that we can know to not output the
       // SgDotExp operator name, and then always in the SgVarRef supress the name when we detect the
       // "__anonymous_0x" named variables.

          bool rhs_is_varRef = (isSgVarRefExp(binary_op->get_rhs_operand()) != NULL);
          bool isAnonymousName = false;
          if (rhs_is_varRef == true)
             {

#error "DEAD CODE!"

#if DEBUG_BINARY_OPERATORS
               printf ("Identified a SgVarRefExp: checking the name for __anonymous_0x \n");
#endif
               SgVarRefExp* varRefExp = isSgVarRefExp(binary_op->get_rhs_operand());
               ROSE_ASSERT(varRefExp != NULL);
               SgVariableSymbol* variableSymbol = varRefExp->get_symbol();
               ROSE_ASSERT(variableSymbol != NULL);
            // SgInitializedName* initializedName = variableSymbol->get_definition();
            // ROSE_ASSERT(initializedName != NULL);
            // isAnonymousName = (string(initializedName->get_name()).substr(0,14) == "__anonymous_0x");
               isAnonymousName = (string(variableSymbol->get_name()).substr(0,14) == "__anonymous_0x");
#if DEBUG_BINARY_OPERATORS
               printf ("In unparseBinaryExpr(): isAnonymousName = %s \n",isAnonymousName ? "true" : "false");
#endif
             }
#endif

#if DEBUG_BINARY_OPERATORS
          printf ("In unparseBinaryExpr(): (after LHS): suppressOutputOfImplicitArrowExp            = %s \n",suppressOutputOfImplicitArrowExp ? "true" : "false");
          curprint ("/* In unparseBinaryExpr(): (after LHS): suppressOutputOfImplicitArrowExp = " + string(suppressOutputOfImplicitArrowExp ? "true" : "false") + " */\n ");
#endif

       // DQ: This is handling that is specific to anonomous unions.
       // if (suppressOutputOfDotExp == false)
          if (suppressOutputOfDotExp == false && suppressOutputOfImplicitArrowExp == false)
             {
               if ( ( (current_function_call_uses_operator_syntax == false) && (parent_function_call_uses_operator_syntax == false) ) || 
                       isRequiredOperator(binary_op,current_function_call_uses_operator_syntax,parent_function_call_uses_operator_syntax) == true )
                  {
#if DEBUG_BINARY_OPERATORS
                    printf("In unparseBinaryExp(): Case 1.4.2.2.1 \n");
                    printf("   --- In unparseBinaryExpr(): Output operator name = %s \n",info.get_operator_name().c_str());
                    curprint("/* Output operator name = " + info.get_operator_name() + " */\n ");
#endif
                    curprint ( string(" ") + info.get_operator_name() + " ");

                 // DQ (7/5/2014): Add assertions using simpler evaluation against stored valuses from the EDG translation.
                    if (is_currently_a_function_call == true)
                       {
                      // DQ (7/5/2014): Reset based on test2004_149.C (can't be set true or false based on test2004_149.C).
                      // ROSE_ASSERT(current_function_call_uses_operator_syntax == true);
                      // ROSE_ASSERT(current_function_call_uses_operator_syntax == false);

                      // DQ (7/5/2014): Reset based on test2004_142.C (can't be set true or false based on test2004_149.C).
                      // ROSE_ASSERT(current_function_call_is_compiler_generated == false);
                      // ROSE_ASSERT(current_function_call_is_compiler_generated == true);
                       }
                 }
                else
                 {
#if DEBUG_BINARY_OPERATORS
                    printf("~~~~~~~ In unparseBinaryExpr(): SKIPPING output of SgDotExp (operator name = %s) \n",info.get_operator_name().c_str());
                    curprint("/* SKIPPING output of operator name = " + info.get_operator_name() + " */\n ");
#endif
                 // DQ (7/5/2014): Add assertions using simpler evaluation against stored valuses from the EDG translation.
                    if (is_currently_a_function_call == true)
                       {
                      // DQ (7/5/2014): Reset based on test2013_113.C (can't be set true or false based on test2004_47.C).
                      // ROSE_ASSERT(current_function_call_uses_operator_syntax == true);
                      // ROSE_ASSERT(current_function_call_uses_operator_syntax == false);

                      // DQ (7/5/2014): Reset based on test2010_04.C (can't be set true or false based on test2004_47.C).
                      // ROSE_ASSERT(current_function_call_is_compiler_generated == true);
                      // ROSE_ASSERT(current_function_call_is_compiler_generated == false);
                       }
                 }
             }
            else
             {
            // If this is the case of suppressOutputOfDotExp == true, then output a space to seperate the output tokens (simplifies debugging).
               curprint(" ");
             }

       // DQ (2/9/2010): Shouldn't this be true (it should also return a bool type).
          ROSE_ASSERT(info.get_nested_expression() != 0);
#if DEBUG_BINARY_OPERATORS
          printf ("In unparseBinaryExpr() -- before output of RHS: info.get_nested_expression() = %d info.get_operator_name() = %s \n",info.get_nested_expression(),info.get_operator_name().c_str());
          curprint ("\n /* unparseBinaryExpr(): Test 4.9  before unparseExpression() info.get_operator_name() = " + info.get_operator_name() + "*/ \n");
#endif
          SgExpression* rhs = binary_op->get_rhs_operand();

       // DQ (4/13/2013): We need to detect if this is a prefix operator, and if we unparse it before 
       // the LHS if we are using the oprator syntax, e.g. when current_function_call_uses_operator_syntax == true

#if DEBUG_BINARY_OPERATORS
          printf ("In unparseBinaryExp(): Case 1.6 \n");
          curprint ( "\n /* unparseBinaryExpr(): Test 5  before unparseExpression() rhs = " + rhs->class_name() + "*/ \n");
#endif
       // unparseExpression(rhs, info);

#if DEBUG_BINARY_OPERATORS
          printf ("++++++++++++++++ Evaluate use of RHS: parent_function_call_uses_operator_syntax = %s \n",parent_function_call_uses_operator_syntax ? "true" : "false");
#endif

#if DEBUG_BINARY_OPERATORS
          printf ("parent_function_call_uses_operator_syntax    = %s \n",parent_function_call_uses_operator_syntax ? "true" : "false");
       // printf ("parent_function_is_overloaded_arrow_operator = %s \n",parent_function_is_overloaded_arrow_operator ? "true" : "false");
          printf ("is_currently_a_function_call                 = %s \n",is_currently_a_function_call ? "true" : "false");
          printf ("current_function_call_uses_operator_syntax   = %s \n",current_function_call_uses_operator_syntax ? "true" : "false");
#endif
       // DQ (7/4/2014): Consider this as a possible fix (put back so I can look at this in the morning).
       // Use (current_function_call_uses_operator_syntax == false) only if the child is a function call.

#define NEW_CASE 0

#if NEW_CASE
       // if (parent_function_call_uses_operator_syntax == true)
       // if (current_function_call_uses_operator_syntax == false && parent_function_call_uses_operator_syntax == true)
       // if ( (is_currently_a_function_call == false) || (is_currently_a_function_call == true && current_function_call_uses_operator_syntax == false) )
#if 0
          if ( (parent_function_call_uses_operator_syntax == false) || 
                    ( (is_currently_a_function_call == false) || 
                      ( (is_currently_a_function_call == true) && 
                        (current_function_call_uses_operator_syntax == false) && 
                        (parent_function_is_overloaded_arrow_operator == false) ) ) )
#endif
          if ( (parent_function_call_uses_operator_syntax == false) || 
                    (parent_function_is_overloaded_arrow_operator == false) ||
                    ( (is_currently_a_function_call == false) || 
                      ( (is_currently_a_function_call == true) && 
                        (current_function_call_uses_operator_syntax == false) ) ) )
#endif
             {

            // DQ (7/6/2014): change this to be symetric with prefix operator handling (switched back: see test2014_75.C).
               if (parent_function_call_uses_operator_syntax == true)
            // if (current_function_call_uses_operator_syntax == true)
                  {
                 // DQ (4/20/2013): I think we want to restrict this to the case of overloaded operators for "->" (and maybe ".").
                 // test2013_114.C demonstrates where just because we detect a cast we don't want to skip the output of other operators.
                 // This is because the introduction of the compiler generated case causes only a restricted number of extra functions
                 // to be calls (e.g. SgArrowExp or it's overloaded version).  I am unclear if the SgDotExp can be caused to be generated 
                 // by a cases (but I expect it can).

                 // DQ (7/4/2014): I think that we need to check if the current_function_call_uses_operator_syntax, if true then we don't 
                 // want to output the operator here.  This is the key point of the bug associated with test2014_75.C.  This would avoid
                 // and elaborate lookup from the rhs expression to see if there were to sequential calls to output an overloaded operator 
                 // (such as "->").

                    ROSE_ASSERT(parent_function_call_uses_operator_syntax == true);
                 // ROSE_ASSERT(current_function_call_uses_operator_syntax == true);

                    bool isRelevantOverloadedOperator = false;
#if 1
                 // DQ (7/6/2014): I would like to eliminate this code to simplify the handling of operator syntax.
                    SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(rhs);
                    if (memberFunctionRefExp != NULL)
                       {
                         string functionName = memberFunctionRefExp->get_symbol()->get_name();
#if DEBUG_BINARY_OPERATORS
                         printf ("++++++++++++++++ functionName = %s \n",functionName.c_str());
#endif
                         if (functionName == "operator->")
                            {
                           // DQ (7/6/2014): Only allow this when it is not compiler generated.
#if 1
                              isRelevantOverloadedOperator = true;
#else
#error "DEAD CODE!"
                           // DQ (7/6/2014): This was a bad idea.
                              if (parent_function_call_is_compiler_generated == false)
                                 {
                                   isRelevantOverloadedOperator = true;
                                 }
#endif
                            }
                       }
#endif
#if DEBUG_BINARY_OPERATORS
                    printf ("++++++++++++++++ binary_op = %p = %s parent_function_call_uses_operator_syntax == true: search for SgCastExp \n",binary_op,binary_op->class_name().c_str());
                    printf ("++++++++++++++++ isRelevantOverloadedOperator = %s \n",isRelevantOverloadedOperator ? "true" : "false");
#endif
                 // If this is a consequence of a cast that was implicit (compiler generated), then we don't want to output this operator.
                    ROSE_ASSERT(binary_op->get_parent() != NULL);
                 // TV (11/08/2018): that can be called when creating an array type where the index is a call expression to a method...
                 // ROSE_ASSERT(binary_op->get_parent()->get_parent() != NULL);

#if DEBUG_BINARY_OPERATORS
                    printf ("++++++++++++++++ binary_op->get_parent()->get_parent() = %p = %s \n",binary_op->get_parent()->get_parent(),binary_op->get_parent()->get_parent()->class_name().c_str());
#endif
                    SgCastExp* castExp = isSgCastExp(binary_op->get_parent()->get_parent());
                 // if (castExp != NULL)
                    if (castExp != NULL && isRelevantOverloadedOperator == true)
                       {
#if DEBUG_BINARY_OPERATORS
                         printf ("++++++++++++++++ Found SgCastExp \n");
#endif
                         if (castExp->get_startOfConstruct()->isCompilerGenerated() == true)
                            {
                           // Skip the output of the RHS.
#if DEBUG_BINARY_OPERATORS
                              printf ("++++++++++++++++ Skipping the output of the RHS \n");
#endif
#if 0
                              printf ("Exiting as a test! \n");
                              ROSE_ASSERT(false);
#endif
                            }
                           else
                            {
#if DEBUG_BINARY_OPERATORS
                              printf ("++++++++++++++++ output of the RHS 1 \n");
                              curprint ( "\n /* unparseBinaryExpr(): output RHS 1 */ \n");
#endif
                           // DQ (7/5/2014): Add assertions using simpler evaluation against stored valuses from the EDG translation.
                              if (is_currently_a_function_call == true)
                                 {
                                   ROSE_ASSERT(current_function_call_uses_operator_syntax == true);
                                   ROSE_ASSERT(current_function_call_is_compiler_generated == false);
                                 }

                           // DQ (7/6/2014): Add assertions using simpler evaluation against stored valuses from the EDG translation.
                              if (parent_is_a_function_call == true)
                                 {
                                // DQ (7/6/2014): This is true only because of the outer predicate.
                                // ROSE_ASSERT(parent_function_call_uses_operator_syntax == true);
                                   ROSE_ASSERT(parent_function_call_is_compiler_generated == false);
                                 }

                              unparseExpression(rhs, info);
                            }
                       }
                      else
                       {
#if DEBUG_BINARY_OPERATORS
                         printf ("++++++++++++++++ output of the RHS 2 \n");
                         curprint ( "\n /* unparseBinaryExpr(): output RHS 2 */ \n");
#endif
                      // DQ (7/5/2014): Add assertions using simpler evaluation against stored valuses from the EDG translation.
                         if (is_currently_a_function_call == true)
                            {
                           // DQ (7/6/2014): Reset based on test2013_117.C (can't be set based on test2013_117.C).
                           // ROSE_ASSERT(current_function_call_uses_operator_syntax == false);

                           // DQ (7/5/2014): Reset based on test2013_38.C (can't be set based on test2010_04.C).
                           // ROSE_ASSERT(current_function_call_is_compiler_generated == false);
                           // ROSE_ASSERT(current_function_call_is_compiler_generated == true);
                            }

                      // DQ (7/6/2014): Add assertions using simpler evaluation against stored valuses from the EDG translation.
                         if (parent_is_a_function_call == true)
                            {
                           // DQ (7/6/2014): This is true only because of the outer predicate.
                           // ROSE_ASSERT(parent_function_call_uses_operator_syntax == true);

                           // DQ (7/5/2014): Reset based on test2004_47.C (can't be set according to test2005_141.C).
                           // ROSE_ASSERT(parent_function_call_is_compiler_generated == true);
                            }

                         unparseExpression(rhs, info);
                       }
                  }
                 else
                  {
#if DEBUG_BINARY_OPERATORS
                    printf ("++++++++++++++++ output of the RHS 3 (non-operator syntax used) \n");
                    curprint ( "\n /* unparseBinaryExpr(): output RHS 3 */ \n");
#endif
                    ROSE_ASSERT(parent_function_call_uses_operator_syntax == false);
                 // ROSE_ASSERT(current_function_call_uses_operator_syntax == false);

                    unparseExpression(rhs, info);

                 // DQ (7/5/2014): Add assertions using simpler evaluation against stored valuses from the EDG translation.
                    if (is_currently_a_function_call == true)
                       {
                      // DQ (7/5/2014): Reset based on test2004_149.C (can't be set according to test2004_47.C).
                      // ROSE_ASSERT(current_function_call_uses_operator_syntax == true);
                      // ROSE_ASSERT(current_function_call_uses_operator_syntax == false);

                      // DQ (7/5/2014): Reset based on test2014_75.C (can't be set according to test2004_149.C).
                      // ROSE_ASSERT(current_function_call_is_compiler_generated == false);
                      // ROSE_ASSERT(current_function_call_is_compiler_generated == true);
                       }
#if 0
                 // DQ (7/6/2014): Add assertions using simpler evaluation against stored valuses from the EDG translation.
                    if (parent_is_a_function_call == true)
                       {
                      // DQ (7/6/2014): Reset based on test2001_28.C // (can't be set according to test2004_149.C).
                         ROSE_ASSERT(parent_function_call_uses_operator_syntax == false);

                      // DQ (7/6/2014): Reset based on test2004_141.C (can't be set according to test2001_28.C).
                      // ROSE_ASSERT(parent_function_call_is_compiler_generated == true);
                       }
#endif
#if DEBUG_BINARY_OPERATORS
                    curprint ( "\n /* DONE: unparseBinaryExpr(): output RHS 3 */ \n");
#endif
                  }
             }
#if NEW_CASE
            else
             {
#if DEBUG_BINARY_OPERATORS
               printf ("++++++++++++++++ skipping output of the RHS 4 \n");
#endif
            // DQ (7/5/2014): Add assertions using simpler evaluation against stored valuses from the EDG translation.
               if (is_currently_a_function_call == true)
                  {
                    ROSE_ASSERT(current_function_call_uses_operator_syntax == true);
                    ROSE_ASSERT(current_function_call_is_compiler_generated == true);
                  }
             }
#endif

#if DEBUG_BINARY_OPERATORS
               curprint ( "\n /* unparseBinaryExpr(): Test 6  after unparseExpression() rhs = " + rhs->class_name() + "*/ \n");
#endif
        }

     info.unset_nested_expression();

#if DEBUG_BINARY_OPERATORS
     printf ("Leaving unparseBinaryExpr(): exp = %p = %s \n",expr,expr->class_name().c_str());
     curprint ( "\n /* Leaving unparseBinaryExpr (expr = " + expr->class_name() + " = " + StringUtility::numberToString(expr) + ") */ \n");
#endif
   }



bool
UnparseLanguageIndependentConstructs::isRequiredOperator( SgBinaryOp* binary_op, bool current_function_call_uses_operator_syntax, bool parent_function_call_uses_operator_syntax )
   {
  // DQ (4/14/2013): The mixing of overloaded operator names and operator syntax has been a bit complex.
  // There are special cases that are especially troubling, so this code tries to handle this.
  // ROSE now preserves the original form of the operator if it was used as either:
  //    1) the overloaded operator name, e.g. "i = result.operator&()->size();", or
  //    2) using the operator syntax, e,g, "i = (&result)->size();"
  // There are different function resolution lookup rules for each type of representaion.
  // So this is a subtle area of C++ to start with.  ROSE tracks in the IR (in the SgFunctionCallExp)
  // if the function call uses the operator syntax (data member, p_uses_operator_syntax, with 
  // set and get access functions automatically generated by ROSETTA.

  // Some complex examples are:
  //    1) "i = result.operator&()->size();" vs. "i = (&result)->size();", the use of SgArrowOp "->" is required.
  //    2) "ref.operator->()->getFormat();" vs. "ref->getFormat();", the use of SgArrowOp "->" is supressed.
  //    3) "s.operator&();" vs. "&s;", example of prefix operator syntax.

     bool returnValue = false;

     ROSE_ASSERT(binary_op != NULL);

#if 0
     printf ("In isRequiredOperator(binary_op = %p = %s, current_function_call_uses_operator_syntax = %s, parent_function_call_uses_operator_syntax = %s \n",
          binary_op,binary_op->class_name().c_str(), current_function_call_uses_operator_syntax ? "true" : "false",parent_function_call_uses_operator_syntax ? "true" : "false");
     printf ("   --- binary_op->get_lhs_operand() = %p = %s \n",binary_op->get_lhs_operand(),binary_op->get_lhs_operand()->class_name().c_str());
     printf ("   --- binary_op->get_rhs_operand() = %p = %s \n",binary_op->get_rhs_operand(),binary_op->get_rhs_operand()->class_name().c_str());

#endif

  // DQ (7/6/2014): Simpler approach, but wrong since overloaded operators unparsed 
  // using operator syntax will always be marked as compiler generated.
  // bool is_compiler_generated = binary_op->isCompilerGenerated();

     bool isArrowExp = (isSgArrowExp(binary_op) != NULL);

     if (isArrowExp == true)
        {
          return true;
        }

  // returnValue = (is_compiler_generated == false || isArrowExp);

  // if (unp->u_sage->isOperator(binary_op->get_rhs_operand()) == false)
  //      returnValue = true;

     SgExpression* lhs = binary_op->get_lhs_operand();
     ROSE_ASSERT(lhs != NULL);
  // SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(lhs);

     SgType* lhs_type = lhs->get_type();
     ROSE_ASSERT(lhs_type != NULL);

  // DQ (4/15/2013): This is required for test2005_129.C
  // SgType::stripType (unsigned char bit_array=STRIP_MODIFIER_TYPE|STRIP_REFERENCE_TYPE|STRIP_POINTER_TYPE|STRIP_ARRAY_TYPE|STRIP_TYPEDEF_TYPE) const
     SgType* stripped_lhs_type = lhs_type->stripType (SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_ARRAY_TYPE|SgType::STRIP_TYPEDEF_TYPE);
     ROSE_ASSERT(stripped_lhs_type != NULL);

#if 0
     printf ("In isRequiredOperator(): lhs_type = %p = %s stripped_lhs_type = %p = %s \n",lhs_type,lhs_type->class_name().c_str(),stripped_lhs_type,stripped_lhs_type->class_name().c_str());
#endif

  // SgClassType*     classType     = isSgClassType    (stripped_lhs_type);
  // SgReferenceType* referenceType = isSgReferenceType(stripped_lhs_type);
     SgPointerType*   pointerType   = isSgPointerType  (stripped_lhs_type);

  // DQ (4/15/2013): I think what makes a greater difference is that this is not a SgArrowExp (see test2013_108.C).
  // DQ (4/15/2013): Added support for SgClassType to handle test2005_141.C.
  // DQ (4/15/2013): Note that of stripped_lhs_type is SgTypeBool we also want to process this branch, I think the 
  // point is that the type is not SgPointerType.
  // if (referenceType != NULL && isSgDotExp(binary_op) != NULL)
  // if (referenceType != NULL && isSgArrowExp(binary_op) == NULL)
  // if ( (referenceType != NULL || classType != NULL) && isSgArrowExp(binary_op) == NULL)
     if ( (pointerType == NULL) && (isSgArrowExp(binary_op) == NULL) )
        {
       // In case of operator>> we need to investigate further, just like the case of operator-> for the SgArrowExp.
       // returnValue = true;

          if (parent_function_call_uses_operator_syntax == true)
             {
            // This addresses the requirement of test2013_97.C
               returnValue = false;
             }
            else
             {
               returnValue = true;
             }
        }
       else
        {
          if (pointerType != NULL && isSgArrowExp(binary_op) != NULL)
             {
            // This make since unless the lhs is an operator->.

               SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(lhs);
               ROSE_ASSERT(functionCallExp != NULL);

               SgDotExp* dotExp = isSgDotExp(functionCallExp->get_function());
               ROSE_ASSERT(dotExp != NULL);

               SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(dotExp->get_rhs_operand());
               if (memberFunctionRefExp != NULL)
                  {
                    SgMemberFunctionDeclaration* memberFunctionDeclaration = memberFunctionRefExp->getAssociatedMemberFunctionDeclaration();
                    ROSE_ASSERT(memberFunctionDeclaration != NULL);
#if 0
                    printf ("In isRequiredOperator(): memberFunctionDeclaration->get_name() = %s \n",memberFunctionDeclaration->get_name().str());
#endif
                    if (memberFunctionDeclaration->get_name() == "operator->")
                       {
                      // Avoid putting out "->->"
                         returnValue = false;
                       }
                      else
                       {
                         returnValue = true;
                       }
                  }
                 else
                  {
                    returnValue = true;
                  }
             }
            else
             {
            // This is the case for test2013_121.C
               returnValue = true;
             }
        }

#if 0
     printf ("Leaving isRequiredOperator(): returnValue = %s \n",returnValue ? "true" : "false");
#endif

     return returnValue;
   }


void
UnparseLanguageIndependentConstructs::unparseValue(SgExpression* expr, SgUnparse_Info& info)
   {
  // DQ (11/9/2005): refactored handling of expression trees stemming from the folding of constants.
     SgValueExp* valueExp = isSgValueExp(expr);

  // DQ (9/11/2011): Added error checking pointed out from static analysis.
     ROSE_ASSERT(valueExp != NULL);

#if 0
     printf ("Inside of unparseValue = %p \n",valueExp);
     valueExp->get_file_info()->display("unparseValue");
#endif

     SgExpression* expressionTree = valueExp->get_originalExpressionTree();
     if (expressionTree != NULL && info.SkipConstantFoldedExpressions() == false)
        {
#if 0
          printf ("Found and expression tree representing a constant generated via constant folding \n");
#endif
#if 0
          curprint ( "\n/* Found and expression tree representing a constant generated via constant folding */\n ");
#endif
       // unparseExpression(expressionTree,info);
          switch (valueExp->variantT())
             {
            // Handle enums so that they will be unparsed as "enum name" instead of as integers
            // bool does not require special handling.
            // case V_SgBoolValExp:             { unparseBoolVal(expr, info);         break; }
               case V_SgEnumVal:                { unparseEnumVal(expr, info);         break; }
               default:
                  {
                    unparseExpression(expressionTree,info);
                  }
             }
        }
       else
        {
          switch (valueExp->variantT())
             {
               case V_SgBoolValExp:             { unparseBoolVal(expr, info);         break; }
               case V_SgCharVal:                { unparseCharVal(expr, info);         break; }
               case V_SgShortVal:               { unparseShortVal(expr, info);        break; }
               case V_SgUnsignedCharVal:        { unparseUCharVal(expr, info);        break; }
               case V_SgWcharVal:               { unparseWCharVal(expr, info);        break; }

            // DQ (2/16/2018): Adding support for char16_t and char32_t (C99 and C++11 specific types).
               case V_SgChar16Val:              { unparseChar16Val(expr, info);       break; }
               case V_SgChar32Val:              { unparseChar32Val(expr, info);       break; }

               case V_SgStringVal:              { unparseStringVal(expr, info);       break; }
               case V_SgUnsignedShortVal:       { unparseUShortVal(expr, info);       break; }
               case V_SgEnumVal:                { unparseEnumVal(expr, info);         break; }
               case V_SgIntVal:                 { unparseIntVal(expr, info);          break; }
               case V_SgUnsignedIntVal:         { unparseUIntVal(expr, info);         break; }
               case V_SgLongIntVal:             { unparseLongIntVal(expr, info);      break; }
               case V_SgLongLongIntVal:         { unparseLongLongIntVal(expr, info);  break; }
               case V_SgUnsignedLongLongIntVal: { unparseULongLongIntVal(expr, info); break; }
               case V_SgUnsignedLongVal:        { unparseULongIntVal(expr, info);     break; }
               case V_SgFloatVal:               { unparseFloatVal(expr, info);        break; }
               case V_SgDoubleVal:              { unparseDoubleVal(expr, info);       break; }
               case V_SgLongDoubleVal:          { unparseLongDoubleVal(expr, info);   break; }
               case V_SgComplexVal:             { unparseComplexVal(expr, info);      break; }

            // DQ (7/31/2014): Adding support for C++11 nullptr const value expressions.
               case V_SgNullptrValExp:          { unparseNullptrVal(expr, info);      break; }

               default:
                  {
                    printf ("Default reached in switch statement valueExp = %p = %s \n",valueExp,valueExp->class_name().c_str());
                    ROSE_ASSERT(false);
                  }
                
             }
        }
   }


// DQ (7/31/2014): Adding support for C++11 nullptr const value expressions.
void
UnparseLanguageIndependentConstructs::unparseNullptrVal (SgExpression* expr, SgUnparse_Info& info)
   {
     ROSE_ASSERT(expr != NULL);
#if 0
     printf ("In UnparseLanguageIndependentConstructs::unparseNullptrVal(): expr = %p = %s \n",expr,expr->class_name().c_str());
#endif

     curprint("nullptr");

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }


void
UnparseLanguageIndependentConstructs::unparseBoolVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgBoolValExp* bool_val = isSgBoolValExp(expr);
     ROSE_ASSERT(bool_val != NULL);

  // Bug reported by Yarden (IBM), output for C should not use C++ keywords ("true" and "false")
  // Note that the getProject() function will use the parent pointers to traverse back to the SgProject node
     bool C_language_support = false;
     SgFile* file = TransformationSupport::getFile(expr);

#if 0
     printf ("In unparseBoolVal(): resolving file to be %p \n",file);
#endif

     if (file == NULL)
        {
       // DQ (9/15/2012): We have added a mechanism for the language to be specified directly.
       // C_language_support = true;
          if (info.get_language() != SgFile::e_default_language)
             {
#if 0
               printf ("In unparseBoolVal(): The output language has been specified directly info.get_language() = %d \n");
#endif
               C_language_support = (info.get_language() == SgFile::e_C_language);
             }
            else
             {
               C_language_support = true;
             }
#if 0
          printf ("Warning: getting the SgFile from SgBoolValExp() expr = %p (trace back to SgFile failed, assuming C language support) \n",expr);
          ROSE_ASSERT(expr->get_file_info() != NULL);
          expr->get_file_info()->display("location of problem bool expression");
          ROSE_ASSERT(expr->get_parent() != NULL);
          printf ("expr->get_parent() = %p = %s \n",expr->get_parent(),expr->get_parent()->class_name().c_str());
          SgTemplateArgument* templateArgument = isSgTemplateArgument(expr->get_parent());
          if (templateArgument != NULL)
             {
               printf ("templateArgument->get_type()                  = %p \n",templateArgument->get_type());
               printf ("templateArgument->get_expression()            = %p \n",templateArgument->get_expression());
            // printf ("templateArgument->get_templateInstantiation() = %p \n",templateArgument->get_templateInstantiation());
             }
            else
             {
               ROSE_ASSERT(expr->get_parent()->get_file_info() != NULL);
               expr->get_parent()->get_file_info()->display("location of problem bool expression (parent)");
             }
#endif
        }
       else
        {
          ROSE_ASSERT(file != NULL);
          C_language_support = file->get_C_only() || file->get_C99_only();
        }

     if (unp->opt.get_num_opt() || (C_language_support == true))
        {
       // The C language does not support boolean values (C99 does, as I recall)
       // we want to print the boolean values as numerical values
       // DQ (11/29/2009): get_value() returns an "int" and MSVC issues a warning for mixed type predicates.
       // if (bool_val->get_value() == true)
          if (bool_val->get_value() != 0)
             {
               curprint("1");
             }
            else
             {
               curprint("0");
             }
        }
       else
        {
       // This is the C++ case (and any language supporting boolean values).

       // print them as "true" or "false"
       // DQ (11/29/2009): get_value() returns an "int" and MSVC issues a warning for mixed type predicates.
       // if (bool_val->get_value() == true)
          if (bool_val->get_value() != 0)
             {
               curprint("true");
             }
            else
             {
               curprint("false");
             }
        }
   }

void
UnparseLanguageIndependentConstructs::unparseShortVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgShortVal* short_val = isSgShortVal(expr);
     ROSE_ASSERT(short_val != NULL);

  // DQ (8/30/2006): Make change suggested by Rama (patch)
  // curprint ( short_val->get_value();
     if (short_val->get_valueString() == "")
        {
          curprint ( tostring(short_val->get_value()));
        }
       else
        {
          curprint ( short_val->get_valueString());
        }
   }

void
UnparseLanguageIndependentConstructs::unparseCharVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgCharVal* char_val = isSgCharVal(expr);
     ROSE_ASSERT(char_val != NULL);

  // DQ (9/30/2006): Use the string where it is available (I think the string 
  // based literals for non-floating point cases are not finished yet).
     if (char_val->get_valueString() == "")
        {
       // curprint ( char_val->get_value();

       // DQ (3/19/2005): Many different literal characters were not being output properly or were being output as 
       // integers which when used as function parameters lead to the wrong function resolution.
       // We can't just output the integer conversion of the character since where this is used as a function 
       // argument it will match a different function prototype (which might not even exist) (see bug 2005_30.C).
       // curprint ( (int) char_val->get_value();
          switch(char_val->get_value())
             {
               case '\0': curprint ( "\'\\0\'"); break;
               case '\1': curprint ( "\'\\1\'"); break;
               case '\2': curprint ( "\'\\2\'"); break;
               case '\3': curprint ( "\'\\3\'"); break;
               case '\4': curprint ( "\'\\4\'"); break;
               case '\5': curprint ( "\'\\5\'"); break;
               case '\6': curprint ( "\'\\6\'"); break;
            // EDG complains that \7, \8, and \9 are redundent (and EDG is correct!)
            // This case is replicated with ASCII BS case '\a' (below)
            // case '\7': curprint ( "\'\\7\'"; break;
            // This case is replicated with ASCI BEL case '\b' (below)
            // case '\8': curprint ( "\'\\8\'"; break;
            // This case is replicated with ASCI HT case '\t' (below)
            // case '\9': curprint ( "\'\\9\'"; break;
            // Note that if we skip this case then '\b' is converted to '^H' which is likely 
            // equivalant but is different enough to be annoying.  Likely other literals have 
            // similar equivalants.  I now expect that '^H' is the wrong translation of '\b'.
            // So the cases below are required.
               case '\n': curprint ( "\'\\n\'"); break;
               case '\t': curprint ( "\'\\t\'"); break;
               case '\v': curprint ( "\'\\v\'"); break;
               case '\b': curprint ( "\'\\b\'"); break;
               case '\r': curprint ( "\'\\r\'"); break;
               case '\f': curprint ( "\'\\f\'"); break;
               case '\a': curprint ( "\'\\a\'"); break;
               case '\'': curprint ( "\'\\'\'"); break;
               case '\"': curprint ( "\'\"\'"); break;
            // Handle special case of ASCI DEL (decimal 127)
            // case '\177': curprint ( "\'\177\'"; break;
            // case 127: curprint ( "\'\177\'"; break;
               case 127: curprint ( "char(127)"); break;
            // This case is required since "\\" is the C++ name of the ASCII "\" character
               case '\\': curprint ( "\'\\\\'"); break;

#if 0
            // DQ (3/19/2005): These should no longer be required, since they are handled properly by the C++ string output!
            // Provide these cases explicitly so that '0' is not converted to '48' 
            // (which is a multi-character character constant, not what we want!)
               case '0': curprint ( "\'0\'"); break;
               case '1': curprint ( "\'1\'"); break;
               case '2': curprint ( "\'2\'"); break;
               case '3': curprint ( "\'3\'"); break;
               case '4': curprint ( "\'4\'"); break;
               case '5': curprint ( "\'5\'"); break;
               case '6': curprint ( "\'6\'"); break;
               case '7': curprint ( "\'7\'"); break;
               case '8': curprint ( "\'8\'"); break;
               case '9': curprint ( "\'9\'"); break;

               case '\?': curprint ( "\'\\?\'"); break;
               case '`': curprint ( "\'`\'"); break;
               case '~': curprint ( "\'~\'"); break;
               case '_': curprint ( "\'_\'"); break;
#endif
               default:
                  {
                 // I could not get char to be output as anything but an integer, so I converted the char to a string 
                 // and then output the string this resulted in not every case (value of char) requiring special handling.
                 // Generate a C string and copy it to a C++ string and then output the C++ string so that we 
                 // can leverage the C++ string handling of character literals.
                    char c[2]; 
                    c[0] = char_val->get_value();
                    c[1] = '\0';
                    string s = c;
                 // curprint ( "\'" + (char)char_val->get_value() + "\'";
                    curprint ( "\'" + s + "\'");
                    break;
                  }
             }
        }
       else
        {
       // Use the string representing the literal where it is available
          curprint ( char_val->get_valueString());
        }
   }

void
UnparseLanguageIndependentConstructs::unparseUCharVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgUnsignedCharVal* uchar_val = isSgUnsignedCharVal(expr);
     ROSE_ASSERT(uchar_val != NULL);

  // DQ (8/30/2006): Make change suggested by Rama (patch)
  // curprint ( (int) uchar_val->get_value();
     if (uchar_val->get_valueString() == "")
        {
          curprint ( tostring((int) uchar_val->get_value()));
        }
       else
        {
          curprint ( uchar_val->get_valueString());
        }
   }

void
UnparseLanguageIndependentConstructs::unparseWCharVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgWcharVal* wchar_val = isSgWcharVal(expr);
     ROSE_ASSERT(wchar_val != NULL);

  //DONT KNOW HOW TO GET ACCESS TO p_valueUL, so just use p_value for now
  //if(wchar_val->p_valueUL) {
  //  curprint ( (wchar_t ) wchar_val->p_valueUL;
  //} else curprint ( (int) wchar_val->get_value();

  // DQ (8/30/2006): Make change suggested by Rama (patch)
  // curprint ( (int) wchar_val->get_value();
     if (wchar_val->get_valueString() == "")
        {
          curprint ( tostring(wchar_val->get_value()));
        }
       else
        {
          curprint ( wchar_val->get_valueString());
        }
   }

void
UnparseLanguageIndependentConstructs::unparseChar16Val(SgExpression* expr, SgUnparse_Info& info)
   {
     SgChar16Val* char_val = isSgChar16Val(expr);
     ROSE_ASSERT(char_val != NULL);

  // DQ (8/30/2006): Make change suggested by Rama (patch)
  // curprint ( (int) wchar_val->get_value();
     if (char_val->get_valueString() == "")
        {
          curprint (tostring(char_val->get_value()));
        }
       else
        {
          curprint (char_val->get_valueString());
        }
   }

void
UnparseLanguageIndependentConstructs::unparseChar32Val(SgExpression* expr, SgUnparse_Info& info)
   {
     SgChar32Val* char_val = isSgChar32Val(expr);
     ROSE_ASSERT(char_val != NULL);

  // DQ (8/30/2006): Make change suggested by Rama (patch)
  // curprint ( (int) wchar_val->get_value();
     if (char_val->get_valueString() == "")
        {
          curprint (tostring(char_val->get_value()));
        }
       else
        {
          curprint (char_val->get_valueString());
        }
   }

#if 0
// Because of the details of Fortran string, this can't be language independent.
void
UnparseLanguageIndependentConstructs::unparseStringVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgStringVal* str_val = isSgStringVal(expr);
     ROSE_ASSERT(str_val != NULL);

  // Handle special case of macro specification (this is a temporary hack to permit us to
  // specify macros within transformations)

     int wrap = unp->u_sage->cur_get_linewrap();
     unp->u_sage->cur_get_linewrap();

#if 0
  // const char* targetString = "ROSE-TRANSFORMATION-MACRO:";
     const char* targetString = "ROSE-MACRO-EXPRESSION:";
     int targetStringLength = strlen(targetString);
  // if (str_val->get_value() == NULL)
     if (str_val->get_value().empty() == true)
        {
          printf ("Found an pointer in SgStringVal = %p for value of string! \n",str_val);
          str_val->get_file_info()->display("Called from unparseStringVal: debug");
        }
     ROSE_ASSERT(str_val->get_value() != NULL);
     if (strncmp(str_val->get_value(),targetString,targetStringLength) == 0)
        {
       // unparse the string without the surrounding quotes and with a new line at the end
          char* remainingString = str_val->get_value()+targetStringLength;
          printf ("Specify a MACRO: remainingString = %s \n",remainingString);
       // Put in a leading CR so that the macro will always be unparsed onto its own line
       // Put in a trailing CR so that the trailing ";" will be unparsed onto its own line too!
          curprint ( "\n" + remainingString + "\n");
        }
       else
        {
          curprint ( "\"" + str_val->get_value() + "\"");
        }
     ROSE_ASSERT(str_val->get_value() != NULL);
#else
  // DQ (3/25/2006): Finally we can use the C++ string class
     string targetString = "ROSE-MACRO-CALL:";
     int targetStringLength = targetString.size();
     string stringValue = str_val->get_value();
     string::size_type location = stringValue.find(targetString);
     if (location != string::npos)
        {
       // unparse the string without the surrounding quotes and with a new line at the end
          string remainingString = stringValue.replace(location,targetStringLength,"");
       // printf ("Specify a MACRO: remainingString = %s \n",remainingString.c_str());
          remainingString.replace(remainingString.find("\\\""),4,"\"");
          curprint ( "\n" + remainingString + "\n");
        }
       else
        {
       // curprint ( "\"" + str_val->get_value() + "\"";
          if (str_val->get_wcharString() == true)
               curprint ( "L");
          curprint ( "\"" + str_val->get_value() + "\"");
        }
#endif
     unp->u_sage->cur_set_linewrap(wrap);
   }
#endif


void
UnparseLanguageIndependentConstructs::unparseUShortVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgUnsignedShortVal* ushort_val = isSgUnsignedShortVal(expr);
     ROSE_ASSERT(ushort_val != NULL);

     curprint ( tostring(ushort_val->get_value()));
#if 0
  // DQ (8/30/2006): Make change suggested by Rama (patch)
  // There appears to not be a concept of "short" literal (even in hex or octal).
  // So it may be that the unsigned short and short types are not even used!
     if (ushort_val->get_valueString() == "")
        {
          curprint ( tostring(ushort_val->get_value()));
        }
       else
        {
          curprint ( ushort_val->get_valueString());
        }
#endif
   }


void
UnparseLanguageIndependentConstructs::unparseEnumVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgEnumVal* enum_val = isSgEnumVal(expr);
     ROSE_ASSERT(enum_val != NULL);

#define DEBUG_UNPARSE_ENUM_VAL 0

#if DEBUG_UNPARSE_ENUM_VAL
     printf ("In Unparse_ExprStmt::unparseEnumVal:\n");
     printf ("  -- info.inEnumDecl() = %s \n",info.inEnumDecl() ? "true" : "false");
     printf ("  -- enum_val->get_requiresNameQualification() = %s\n", enum_val->get_requiresNameQualification() ? "true" : "false");
     printf ("  -- enum_val->get_qualified_name_prefix() = %s\n", enum_val->get_qualified_name_prefix().getString().c_str());
#endif
#if 0
     curprint("\n/* In Unparse_ExprStmt::unparseEnumVal() */\n");
#endif

  // todo: optimize this so that the qualified name is only printed when necessary.
     if (info.inEnumDecl() == true)
        {
       // The enum value output in the enum declaration should be a value (it could be an enum constant
       // if it has already been output as a value (repreated reference) but this is an obsure detail).
          curprint(tostring(enum_val->get_value()));
        }
       else
        {
       // DQ (12/20/2005): Added more general support for name qualification for enum values (to fix test2005_188.C).
       // ROSE_ASSERT(enum_val->get_declaration() != NULL);
       // ROSE_ASSERT(enum_val->get_declaration()->get_scope() != NULL);

       // DQ (10/14/2006): Reimplemented support for name qualification.
       // if (SageInterface::is_Cxx_language() == true)
          if (enum_val->get_declaration() != NULL)
             {
            // DQ (12/20/2005): Added more general support for name qualification for enum values (to fix test2005_188.C).
               ROSE_ASSERT(enum_val->get_declaration() != NULL);
               ROSE_ASSERT(enum_val->get_declaration()->get_scope() != NULL);

            // DQ (10/14/2006): Reimplemented support for name qualification.
               if (SageInterface::is_Cxx_language() == true)
                  {
                 // SgScopeStatement* parentScope = decl_item->get_scope();
#if 0
                 // DQ (6/15/2013): Added in name qualification support for enum values.
                 // SgName nameQualifier = "NEED_QUALIFIED_NAME_for_unparseEnumVal::";
                 // DQ (5/29/2011): Newest refactored support for name qualification.
                 // printf ("In unparseFuncRef(): Looking for name qualification for SgFunctionRefExp = %p \n",func_ref);
                    SgName nameQualifier = enum_val->get_qualified_name_prefix();
#else
                 // DQ (12/22/2006): This is use the information that qualification is required. This will trigger the use of 
                 // global qualification even if it is not required with normal qualification.  That is that the specification 
                 // of qualification triggers possible (likely) over qualification.  Overqualification is generally the default
                 // this flag is sometime taken to mean that the "::" is required as well.
#if DEBUG_UNPARSE_ENUM_VAL
                    printf ("enum_val->get_requiresNameQualification() = %s \n",enum_val->get_requiresNameQualification() ? "true" : "false");
#endif
                 // cur << "\n/* funcdecl_stmt->get_requiresNameQualificationOnReturnType() = " << (funcdecl_stmt->get_requiresNameQualificationOnReturnType() ? "true" : "false") << " */ \n";
                    if (enum_val->get_requiresNameQualification() == true)
                       {
                      // Note that general qualification of types is separated from the use of globl qualification.
                      // info.set_forceQualifiedNames();
                         info.set_requiresGlobalNameQualification();
                       }

                 // DQ (6/9/2011): Newest refactored support for name qualification.
                 // SgName nameQualifier = unp->u_name->generateNameQualifier(enum_val->get_declaration(),info);
                    SgName nameQualifier = enum_val->get_qualified_name_prefix();
#if DEBUG_UNPARSE_ENUM_VAL
                    printf ("In Unparse_ExprStmt::unparseEnumVal: nameQualifier = %s \n",nameQualifier.str());
#endif
                 // DQ (8/31/2012): If we are going to NOT output a name, then we had better not out any name qualification.
                    if (enum_val->get_name().is_null() == true)
                       {
                         printf ("If the enum name does not exist, then we can't qualify it nameQualifier = %s (reset) \n",(nameQualifier.is_null() == false) ? nameQualifier.str() : "NULL");
                         nameQualifier = "";
                         ROSE_ASSERT(nameQualifier.is_null() == true);
                       }
#endif
#if DEBUG_UNPARSE_ENUM_VAL
                    printf ("enum value's nameQualifier = %s \n",(nameQualifier.is_null() == false) ? nameQualifier.str() : "NULL");
#endif
                 // ROSE_ASSERT (nameQualifier.is_null() == false);
                    if (nameQualifier.is_null() == false)
                       {
                         curprint ( nameQualifier.str() );
                       }
                  }
            // printf ("In Unparser::unparseEnumVal: classdefn = %s pointer \n",classdefn ? "VALID" : "NULL");
             }
            else
             {
               ROSE_ASSERT (enum_val->get_name().is_null() == false);
               printf ("Warning in Unparser::unparseEnumVal(): no associated enum declaration specificed for enum value = %s \n",enum_val->get_name().str());
             }

#if DEBUG_UNPARSE_ENUM_VAL
       // printf ("In Unparse_ExprStmt::unparseEnumVal: classdefn = %s pointer \n",classdefn ? "VALID" : "NULL");
          printf ("In Unparse_ExprStmt::unparseEnumVal: enum_val->get_name().is_null() = %s \n",enum_val->get_name().is_null() ? "true" : "false");
#endif
       // DQ (8/31/2012): We need to allow for values that would not be mapped to enum names and in this case 
       // are output as enum values (see test2012_202.C for an example of this).
       // DQ (6/18/2006): Identify the case of an un-named enum, would be an error if we unparsed this directly.
       // ROSE_ASSERT (enum_val->get_name().is_null() == false);
       // curprint (  enum_val->get_name().str());

       // DQ (11/4/2012): Detect if this is a generated name with an cast from an un-named type (we can't output these names).
          SgName enum_value_name = enum_val->get_name();
          SgName substring = enum_value_name.head(strlen("__anonymous_"));
          bool isGeneratedName = (substring == "__anonymous_");
#if DEBUG_UNPARSE_ENUM_VAL
          printf ("enum_value_name = %s \n",enum_value_name.str());
          printf ("substring = %s \n",substring.str());
          printf ("isGeneratedName = %s \n",isGeneratedName ? "true" : "false");
#endif

       // DQ (11/4/2012): Never output a generated name since it will cause link failures 
       // (and frequently also interpretation as implicit function calls in C).
       // if (enum_val->get_name().is_null() == false)
          if ( (enum_val->get_name().is_null() == false) && (isGeneratedName == false) )
             {
            // This is the typical case.
            // curprint(enum_val->get_name().str());

            // DQ (5/14/2018): For C++11 enum class declarations, the assocated enum value will 
            // ALWAYS require an explicit cast or additional name qualification.
               SgEnumDeclaration* enumDeclaration = enum_val->get_declaration();
               if (enumDeclaration != NULL)
                  {
                    if (enumDeclaration->get_isScopedEnum() == true)
                       {
                      // curprint(enum_val->get_name().str());
                         curprint(enumDeclaration->get_name().str());
                         curprint("(");
                         string valueString = StringUtility::numberToString(enum_val->get_value());
                         curprint(valueString);
                         curprint(")");
                       }
                      else
                       {
                         curprint(enum_val->get_name().str());
                       }
                  }
                 else
                  {
                    curprint(enum_val->get_name().str());
                  }
             }
            else
             {
               curprint(tostring(enum_val->get_value()));
             }
        }

#if DEBUG_UNPARSE_ENUM_VAL
     printf ("Leaving Unparse_ExprStmt::unparseEnumVal: info.inEnumDecl() = %s \n",info.inEnumDecl() ? "true" : "false");
#endif
#if 0
     curprint("\n/* Leaving Unparse_ExprStmt::unparseEnumVal() */\n");
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }


void 
UnparseLanguageIndependentConstructs::unparseIntVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgIntVal* int_val = isSgIntVal(expr);
     ROSE_ASSERT(int_val != NULL);

  // printf ("In Unparse_ExprStmt::unparseIntVal(): int_val->get_value() = %d \n",int_val->get_value());
  // curprint ( int_val->get_value();
  // curprint ( int_val->get_valueString();

  // DQ (8/30/2006): Make change suggested by Rama (patch)
     if (int_val->get_valueString() == "")
        {
          curprint(tostring(int_val->get_value()));
        }
       else
        {
          curprint(int_val->get_valueString());
        }
   }


void
UnparseLanguageIndependentConstructs::unparseUIntVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgUnsignedIntVal* uint_val = isSgUnsignedIntVal(expr);
     ROSE_ASSERT(uint_val != NULL);

  // curprint ( uint_val->get_value();
  // DQ (7/20/2006): Bug reported by Yarden, see test2006_94.C for where this is important (e.g. evaluation of "if (INT_MAX + 1U > 0)").
  // curprint ( "U";

  // DQ (8/30/2006): Make change suggested by Rama (patch)
     if (uint_val->get_valueString() == "")
        {
          curprint(tostring(uint_val->get_value()));
        }
       else
        {
          curprint(uint_val->get_valueString());
        }
   }

void
UnparseLanguageIndependentConstructs::unparseLongIntVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgLongIntVal* longint_val = isSgLongIntVal(expr);
     ROSE_ASSERT(longint_val != NULL);

  // curprint ( longint_val->get_value();
  // DQ (7/20/2006): Bug reported by Yarden, see test2006_94.C for where this is important (e.g. evaluation of "if (INT_MAX + 1U > 0)").
  // curprint ( "L";

  // DQ (8/30/2006): Make change suggested by Rama (patch)
     if (longint_val->get_valueString() == "")
        {
          curprint(tostring(longint_val->get_value()));
        }
       else
        {
          curprint(longint_val->get_valueString());
        }
   }

void
UnparseLanguageIndependentConstructs::unparseLongLongIntVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgLongLongIntVal* longlongint_val = isSgLongLongIntVal(expr);
     ROSE_ASSERT(longlongint_val != NULL);

  // curprint ( longlongint_val->get_value();
  // DQ (7/20/2006): Bug reported by Yarden, see test2006_94.C for where this is important (e.g. evaluation of "if (INT_MAX + 1U > 0)").
  // curprint ( "LL";

  // DQ (8/30/2006): Make change suggested by Rama (patch)
     if (longlongint_val->get_valueString() == "")
        {
          curprint(tostring(longlongint_val->get_value()));
        }
       else
        {
          curprint(longlongint_val->get_valueString());
        }
   }

void
UnparseLanguageIndependentConstructs::unparseULongLongIntVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgUnsignedLongLongIntVal* ulonglongint_val = isSgUnsignedLongLongIntVal(expr);
     ROSE_ASSERT(ulonglongint_val != NULL);

  // curprint ( ulonglongint_val->get_value();
  // DQ (7/20/2006): Bug reported by Yarden, see test2006_94.C for where this is important (e.g. evaluation of "if (INT_MAX + 1U > 0)").
  // curprint ( "ULL";

  // DQ (8/30/2006): Make change suggested by Rama (patch)
     if (ulonglongint_val->get_valueString() == "")
        {
          curprint(tostring(ulonglongint_val->get_value()));
        }
       else
        {
          curprint(ulonglongint_val->get_valueString());
        }
   }

void
UnparseLanguageIndependentConstructs::unparseULongIntVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgUnsignedLongVal* ulongint_val = isSgUnsignedLongVal(expr);
     ROSE_ASSERT(ulongint_val != NULL);

  // curprint ( ulongint_val->get_value();
  // DQ (7/20/2006): Bug reported by Yarden, see test2006_94.C for where this is important (e.g. evaluation of "if (INT_MAX + 1U > 0)").
  // curprint ( "UL";

  // DQ (8/30/2006): Make change suggested by Rama (patch)
     if (ulongint_val->get_valueString() == "")
        {
          curprint(tostring(ulongint_val->get_value()));
        }
       else
        {
          curprint(ulongint_val->get_valueString());
        }
   }

void
UnparseLanguageIndependentConstructs::unparseFloatVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgFloatVal* float_val = isSgFloatVal(expr);
     ROSE_ASSERT(float_val != NULL);

#if 0
     printf ("Inside of unparseFloatVal = %p \n",float_val);
     float_val->get_file_info()->display("unparseFloatVal");
#endif

  // DQ (10/18/2005): Need to handle C code which cannot use C++ mechanism to specify 
  // infinity, quiet NaN, and signaling NaN values.  Note that we can't use the C++
  // interface since the input program, and thus the generated code, might not have 
  // included the "limits" header file.
     float float_value = float_val->get_value();
#if 0
     if (SageInterface::is_C_language() == true)
        {
          if (float_val->get_valueString() == "")
             {
               curprint(tostring(float_val->get_value()));
             }
            else
             {
               curprint(float_val->get_valueString());
             }
        }
       else
        {
        }
#endif

     if (float_value == std::numeric_limits<float>::infinity())
        {
       // Because of Fortran kind (compiler dependent) the string literal may be a double.
       // Thus it makes more sense to print the original string literal [Rasmussen 4/28/2019].
          if (SageInterface::is_Fortran_language() && float_val->get_valueString().length() > 0)
             {
               curprint(float_val->get_valueString());
             }
            else
             {
            // curprint ( "std::numeric_limits<float>::infinity()";
               curprint( "__builtin_huge_valf()");
             }
        }
       else
        {
       // Test for NaN value (famous test of to check for equality) or check for C++ definition of NaN.
       // We detect C99 and C "__NAN__" in EDG, but translate to backend specific builtin function.
          if ((float_value != float_value) || (float_value == std::numeric_limits<float>::quiet_NaN()) )
             {
            // curprint ( "std::numeric_limits<float>::quiet_NaN()";
               curprint( "__builtin_nanf (\"\")");
             }
            else
             {
               if (float_value == std::numeric_limits<float>::signaling_NaN())
                  {
                 // curprint ( "std::numeric_limits<float>::signaling_NaN()";
                    curprint("__builtin_nansf (\"\")");
                  }
                 else
                  {
                 // typical case!
                 // curprint ( float_val->get_value();
                 // AS (11/08/2005) add support for values as string
                    if (float_val->get_valueString() == "")
                       {
                         curprint(tostring(float_val->get_value()));
                       }
                      else
                       {
                         curprint(float_val->get_valueString());
                       }
                  }
             }
        }
   }

#if 0
// DQ: This is no longer used (we use properties of the iostream to formate floating point now).
bool
UnparseLanguageIndependentConstructs::zeroRemainder( long double doubleValue )
   {
     int integerValue = (int)doubleValue;
     double remainder = doubleValue - (double)integerValue;

     return (remainder == 0) ? true : false;
   }
#endif

void
UnparseLanguageIndependentConstructs::unparseDoubleVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgDoubleVal* dbl_val = isSgDoubleVal(expr);
     ROSE_ASSERT(dbl_val != NULL);

#if 0
     printf ("Inside of unparseDblVal = %p \n",dbl_val);
     dbl_val->get_file_info()->display("unparseDblVal");
#endif

  // os->setf(ios::showpoint);
  // curprint ( dbl_val->get_value();
  // curprint ( setiosflags(ios::showpoint) + setprecision(4) + dbl_val->get_value();

     setiosflags(ios::showpoint);

  // DQ (10/16/2004): Not sure what 4 implies, but we get 16 digits after the decimal 
  // point so it should be fine (see test2004_114.C)!
     setprecision(4);

  // curprint ( dbl_val->get_value();
  // os->unsetf(ios::showpoint);

  // DQ (10/18/2005): Need to handle C code which cannot use C++ mechanism to specify 
  // infinity, quiet NaN, and signaling NaN values.
     double double_value = dbl_val->get_value();
     if (double_value == std::numeric_limits<double>::infinity())
        {
       // printf ("Infinite value found as value in unparseFloatVal() \n");
       // curprint ( "std::numeric_limits<double>::infinity()";
          curprint ( "__builtin_huge_val()");
        }
       else
        {
       // Test for NaN value (famous test of to check for equality) or check for C++ definition of NaN.
       // We detect C99 and C "__NAN__" in EDG, but translate to backend specific builtin function.
          if ( (double_value != double_value) || (dbl_val->get_value() == std::numeric_limits<double>::quiet_NaN()) )
             {
            // curprint ( "std::numeric_limits<double>::quiet_NaN()";
               curprint ( "__builtin_nan (\"\")");
             }
            else
             {
               if (double_value == std::numeric_limits<double>::signaling_NaN())
                  {
                 // curprint ( "std::numeric_limits<double>::signaling_NaN()";
                    curprint ( "__builtin_nans (\"\")");
                  }
                 else
                  {
                 // typical case!
                 // curprint ( dbl_val->get_value();
                 // AS (11/08/2005) add support for values as string
                    if (dbl_val->get_valueString() == "")
                         curprint(tostring(double_value));
                      else
                         curprint(dbl_val->get_valueString());
                  }
             }
        }
   }

void
UnparseLanguageIndependentConstructs::unparseLongDoubleVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgLongDoubleVal* longdbl_val = isSgLongDoubleVal(expr);
     ROSE_ASSERT(longdbl_val != NULL);
  /* code inserted from specification */
  
  // curprint ( longdbl_val->get_value();

  // DQ (10/18/2005): Need to handle C code which cannot use C++ mechanism to specify 
  // infinity, quiet NaN, and signaling NaN values.
     long double longDouble_value = longdbl_val->get_value();
     if (longDouble_value == std::numeric_limits<long double>::infinity())
        {
       // printf ("Infinite value found as value in unparseFloatVal() \n");
       // curprint ( "std::numeric_limits<long double>::infinity()";
          curprint ( "__builtin_huge_vall()");
        }
       else
        {
       // Test for NaN value (famous test of to check for equality) or check for C++ definition of NaN.
       // We detect C99 and C "__NAN__" in EDG, but translate to backend specific builtin function.
          if ( (longDouble_value != longDouble_value) || (longDouble_value == std::numeric_limits<long double>::quiet_NaN()) )
             {
            // curprint ( "std::numeric_limits<long double>::quiet_NaN()";
               curprint ( "__builtin_nanl (\"\")");
             }
            else
             {
               if (longDouble_value == std::numeric_limits<long double>::signaling_NaN())
                  {
                 // curprint ( "std::numeric_limits<long double>::signaling_NaN()";
                    curprint ( "__builtin_nansl (\"\")");
                  }
                 else
                  {
                 // typical case!
                 // curprint ( longdbl_val->get_value();
                 // AS (11/08/2005) add support for values as string
                    if (longdbl_val->get_valueString() == "")
                         curprint ( tostring(longDouble_value));
                      else
                         curprint ( longdbl_val->get_valueString());
                  }
             }
        }
   }

void
UnparseLanguageIndependentConstructs::unparseComplexVal(SgExpression* expr, SgUnparse_Info& info)
   {
     SgComplexVal* complex_val = isSgComplexVal(expr);
     ROSE_ASSERT(complex_val != NULL);

     if (complex_val->get_valueString() != "") { // Has string
       curprint (complex_val->get_valueString());
     } else if (complex_val->get_real_value() == NULL) { // Pure imaginary
       curprint ("(0.0, ");
       unparseValue(complex_val->get_imaginary_value(), info);
       curprint (")");
     } else { // Complex number
       curprint ("(");
       unparseValue(complex_val->get_real_value(), info);
       curprint (", ");
       unparseValue(complex_val->get_imaginary_value(), info);
       curprint (")");
     }
   }


void UnparseLanguageIndependentConstructs::unparseMatrixExp(SgExpression *expr, SgUnparse_Info& info)
{
  SgMatrixExp *matrix = isSgMatrixExp(expr);

  SgExpressionPtrList::iterator i = matrix->get_expressions().begin();

  curprint("[");
  
  for(; i != matrix->get_expressions().end(); ++i)
    {
      SgUnparse_Info newinfo(info);
      newinfo.set_SkipBaseType();

      unparseExpression(*i, newinfo);

      curprint(";");
    }

  curprint("]");
}

void
UnparseLanguageIndependentConstructs::unparseExprList(SgExpression* expr, SgUnparse_Info& info)
   {
     SgExprListExp* expr_list = isSgExprListExp(expr);
     ROSE_ASSERT(expr_list != NULL);

#if 0
     curprint("/* output SgExprListExp */");
#endif

     SgExpressionPtrList::iterator i = expr_list->get_expressions().begin();

     if (i != expr_list->get_expressions().end())
        {
          while (true)
             {
               SgUnparse_Info newinfo(info);
               newinfo.set_SkipBaseType();

#if 0
            // DQ (2/20/2019): Check if this is a compiler generated SgConstructorInitializer
            // (see Cxx11_tests/test2019_171.C).  I think this may be the wrong place for this.
               SgConstructorInitializer* constructorInitializer = isSgConstructorInitializer(*i);
               if (constructorInitializer != NULL)
                  {
                    if (constructorInitializer->isCompilerGenerated() == true)
                       {
                         printf ("In UnparseLanguageIndependentConstructs::unparseExprList(): Found compiler generated constructor initializer \n");
                      // break out of this loop.
                         break;
                       }
                  }
#endif

               unparseExpression(*i, newinfo);
               i++;
               if (i != expr_list->get_expressions().end())
                  {
                    curprint ( ",");
                  }
                 else
                  {
                    break;
                  }
             }
        }
   }


void 
UnparseLanguageIndependentConstructs::unparseIncludeDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgIncludeDirectiveStatement* directive = isSgIncludeDirectiveStatement(stmt);
     ROSE_ASSERT(directive != NULL);
     
  // negara1 (08/15/2011): Do not unparse the directive string as long as there are PreprocessorInfos attached to nodes in order to avoid double unparsing.
  // Also, note that it might belong to a different file since the code can reach here when the include directive statement's header file body belongs to the unparsed file.     
  // curprint(directive->get_directiveString());
  // unp->cur.insert_newline(1);

#if 0
     info.display("In unparseIncludeDirectiveStatement");
#endif

     ROSE_ASSERT(info.get_current_source_file() != NULL);
     bool usingTokenUnparsing = info.get_current_source_file()->get_unparse_tokens();

#if 0
     printf ("In unparseIncludeDirectiveStatement: usingTokenUnparsing = %s \n",usingTokenUnparsing ? "true" : "false");
#endif

#if 1
  // DQ (12/1/2018): This code may be required when unparsing using the header file unparsing support but not using the token unparsing.
     if (usingTokenUnparsing == false)
        {
       // DQ (11/13/2018): I want to try to turn this off as a tst for test9 in UnparseHeader_tests.
       // DQ (10/31/2018): This might be a better approach that I first realized (since the header file can contain just another header file with no statements).

       // DQ (9/24/2018): I think we want CPP directivs to be unparsed from the SgAttachedPreprocessingInfo lists on statements, instead of seperately.
       // This is the better choice because then the other comments and any other CPP directives will be unparsed as in the original code.
       // NOTE: If we don't suppores this here, then there will be two include directives unparsed.
          SgHeaderFileBody* headerFileBody = directive -> get_headerFileBody();

       // DQ (3/24/2019): The newest use of this IR nodes does not accomidate the headerFileBody.
       // ROSE_ASSERT(headerFileBody != NULL);
          if (headerFileBody != NULL)
             {
#if 0
               printf ("In unparseIncludeDirectiveStatement(): headerFileBody -> get_file_info() -> get_filenameString() = %s \n",headerFileBody -> get_file_info() -> get_filenameString().c_str());
               printf ("In unparseIncludeDirectiveStatement(): getFileName() = %s \n",getFileName().c_str());
#endif
               if (headerFileBody -> get_file_info() -> get_filenameString() == getFileName())
                  {
                    unparseAttachedPreprocessingInfo(headerFileBody, info, PreprocessingInfo::after); //Its always "after" if attached to a header file body.
                  }
             }
            else
             {
            // DQ (3/24/2019): The newest use of this IR nodes does not accomidate the headerFileBody.
               ROSE_ASSERT(directive != NULL);
               curprint("\n ");

  // DQ (3/24/2019): Adding extra CR.
     unp->cur.insert_newline(1);

               curprint(directive->get_directiveString());
            // unp->u_sage->curprint_newline();
               unp->cur.insert_newline(1);
             }
        }
#else
     printf ("In unparseIncludeDirectiveStatement(): skipping unparsing of directives and comments \n");
#endif
   }

void 
UnparseLanguageIndependentConstructs::unparseDefineDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgDefineDirectiveStatement* directive = isSgDefineDirectiveStatement(stmt);
     ROSE_ASSERT(directive != NULL);

  // DQ (3/24/2019): We need "\n " instead of "\n" to force a CR before unparsing the CPP directive.
  // ALSO: we need the "unp->cur.insert_newline(1);" statement as well.
  // I forget the details of why this is an issue in the curprint() implementation.
     curprint("\n ");
  // unp->u_sage->curprint_newline();

  // DQ (3/24/2019): Adding extra CR.
     unp->cur.insert_newline(1);

     curprint(directive->get_directiveString());
  // unp->u_sage->curprint_newline();
     unp->cur.insert_newline(1);

#if 0
     printf ("Unparsing from unparseDefineDirectiveStatement() \n");
#endif
   }

void 
UnparseLanguageIndependentConstructs::unparseUndefDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgUndefDirectiveStatement* directive = isSgUndefDirectiveStatement(stmt);
     ROSE_ASSERT(directive != NULL);
     curprint("\n ");

  // DQ (3/24/2019): Adding extra CR.
     unp->cur.insert_newline(1);

     curprint(directive->get_directiveString());
     unp->u_sage->curprint_newline();
   }

void 
UnparseLanguageIndependentConstructs::unparseIfdefDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgIfdefDirectiveStatement* directive = isSgIfdefDirectiveStatement(stmt);
     ROSE_ASSERT(directive != NULL);
     curprint("\n ");

  // DQ (3/24/2019): Adding extra CR.
     unp->cur.insert_newline(1);

     curprint(directive->get_directiveString());
     unp->u_sage->curprint_newline();
   }

void 
UnparseLanguageIndependentConstructs::unparseIfndefDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgIfndefDirectiveStatement* directive = isSgIfndefDirectiveStatement(stmt);
     ROSE_ASSERT(directive != NULL);
     curprint("\n ");

  // DQ (3/24/2019): Adding extra CR.
     unp->cur.insert_newline(1);

     curprint(directive->get_directiveString());
     unp->u_sage->curprint_newline();
   }

void 
UnparseLanguageIndependentConstructs::unparseDeadIfDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgDeadIfDirectiveStatement* directive = isSgDeadIfDirectiveStatement(stmt);
     ROSE_ASSERT(directive != NULL);
     curprint("\n ");

  // DQ (3/24/2019): Adding extra CR.
     unp->cur.insert_newline(1);

     curprint(directive->get_directiveString());
     unp->u_sage->curprint_newline();
   }

void 
UnparseLanguageIndependentConstructs::unparseIfDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgIfDirectiveStatement* directive = isSgIfDirectiveStatement(stmt);
     ROSE_ASSERT(directive != NULL);

  // curprint("/* CR START */");
     curprint("\n ");
  // unp->u_sage->curprint_newline();
  // curprint("/* CR END */");

  // DQ (3/24/2019): Adding extra CR.
     unp->cur.insert_newline(1);

     curprint(directive->get_directiveString());
     unp->u_sage->curprint_newline();
   }

void 
UnparseLanguageIndependentConstructs::unparseElseDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgElseDirectiveStatement* directive = isSgElseDirectiveStatement(stmt);
     ROSE_ASSERT(directive != NULL);
     curprint("\n ");

  // DQ (3/24/2019): Adding extra CR.
     unp->cur.insert_newline(1);

     curprint(directive->get_directiveString());
     unp->u_sage->curprint_newline();
   }

void 
UnparseLanguageIndependentConstructs::unparseElseifDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgElseifDirectiveStatement* directive = isSgElseifDirectiveStatement(stmt);
     ROSE_ASSERT(directive != NULL);
     curprint("\n ");

  // DQ (3/24/2019): Adding extra CR.
     unp->cur.insert_newline(1);

     curprint(directive->get_directiveString());
     unp->u_sage->curprint_newline();
   }

void 
UnparseLanguageIndependentConstructs::unparseEndifDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgEndifDirectiveStatement* directive = isSgEndifDirectiveStatement(stmt);
     ROSE_ASSERT(directive != NULL);
     curprint("\n ");

  // DQ (3/24/2019): Adding extra CR.
     unp->cur.insert_newline(1);

     curprint(directive->get_directiveString());
     unp->u_sage->curprint_newline();
   }

void 
UnparseLanguageIndependentConstructs::unparseLineDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgLineDirectiveStatement* directive = isSgLineDirectiveStatement(stmt);
     ROSE_ASSERT(directive != NULL);
     curprint("\n ");

  // DQ (3/24/2019): Adding extra CR.
     unp->cur.insert_newline(1);

     curprint(directive->get_directiveString());
     unp->u_sage->curprint_newline();
   }

void 
UnparseLanguageIndependentConstructs::unparseWarningDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgWarningDirectiveStatement* directive = isSgWarningDirectiveStatement(stmt);
     ROSE_ASSERT(directive != NULL);
     curprint("\n ");

  // DQ (3/24/2019): Adding extra CR.
     unp->cur.insert_newline(1);

     curprint(directive->get_directiveString());
     unp->u_sage->curprint_newline();
   }

void 
UnparseLanguageIndependentConstructs::unparseErrorDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgErrorDirectiveStatement* directive = isSgErrorDirectiveStatement(stmt);
     ROSE_ASSERT(directive != NULL);
     curprint("\n ");

  // DQ (3/24/2019): Adding extra CR.
     unp->cur.insert_newline(1);

     curprint(directive->get_directiveString());
     unp->u_sage->curprint_newline();
   }

void 
UnparseLanguageIndependentConstructs::unparseEmptyDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgEmptyDirectiveStatement* directive = isSgEmptyDirectiveStatement(stmt);
     ROSE_ASSERT(directive != NULL);
     curprint("\n ");

  // DQ (3/24/2019): Adding extra CR.
     unp->cur.insert_newline(1);

     curprint(directive->get_directiveString());
     unp->u_sage->curprint_newline();
   }

void 
UnparseLanguageIndependentConstructs::unparseIdentDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgIdentDirectiveStatement* directive = isSgIdentDirectiveStatement(stmt);
     ROSE_ASSERT(directive != NULL);
     curprint("\n ");

  // DQ (3/24/2019): Adding extra CR.
     unp->cur.insert_newline(1);

     curprint(directive->get_directiveString());
     unp->u_sage->curprint_newline();
   }

void 
UnparseLanguageIndependentConstructs::unparseIncludeNextDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgIncludeNextDirectiveStatement* directive = isSgIncludeNextDirectiveStatement(stmt);
     ROSE_ASSERT(directive != NULL);
     curprint("\n ");

  // DQ (3/24/2019): Adding extra CR.
     unp->cur.insert_newline(1);

     curprint(directive->get_directiveString());
     unp->u_sage->curprint_newline();
   }

void 
UnparseLanguageIndependentConstructs::unparseLinemarkerDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgLinemarkerDirectiveStatement* directive = isSgLinemarkerDirectiveStatement(stmt);
     ROSE_ASSERT(directive != NULL);
     curprint("\n ");

  // DQ (3/24/2019): Adding extra CR.
     unp->cur.insert_newline(1);

     curprint(directive->get_directiveString());
     unp->u_sage->curprint_newline();
   }

void UnparseLanguageIndependentConstructs::unparseOmpDefaultClause(SgOmpClause* clause, SgUnparse_Info& info)
{
  ROSE_ASSERT(clause != NULL);
  SgOmpDefaultClause * c = isSgOmpDefaultClause(clause);
  ROSE_ASSERT(c!= NULL);
  curprint(string(" default("));
  SgOmpClause::omp_default_option_enum dv = c->get_data_sharing(); 
  switch (dv)
  {
    case SgOmpClause::e_omp_default_none:
      {
        curprint(string("none"));
        break;
      }
    case SgOmpClause::e_omp_default_shared:
      {
        curprint(string("shared"));
        break;
      }   
    case SgOmpClause::e_omp_default_private:
      {
        curprint(string("private"));
        break;
      }
    case SgOmpClause::e_omp_default_firstprivate:
      {
        curprint(string("firstprivate"));
        break; 
      }
    default:
      cerr<<"Error: UnparseLanguageIndependentConstructs::unparseOmpDefaultClause() meets unacceptable default option value:"<<dv<<endl;
      ROSE_ASSERT (false);
      break;
  }    
  curprint(string(")"));
}

void UnparseLanguageIndependentConstructs::unparseOmpProcBindClause(SgOmpClause* clause, SgUnparse_Info& info)
{
  ROSE_ASSERT(clause != NULL);
  SgOmpProcBindClause * c = isSgOmpProcBindClause(clause);
  ROSE_ASSERT(c!= NULL);
  curprint(string(" proc_bind("));
  SgOmpClause::omp_proc_bind_policy_enum dv = c->get_policy(); 
  switch (dv)
  {
    case SgOmpClause::e_omp_proc_bind_policy_master:
      {
        curprint(string("master"));
        break;
      }
    case SgOmpClause::e_omp_proc_bind_policy_close:
      {
        curprint(string("close"));
        break;
      }   
    case SgOmpClause::e_omp_proc_bind_policy_spread:
      {
        curprint(string("spread"));
        break;
      }
   default:
      cerr<<"Error: UnparseLanguageIndependentConstructs::unparseOmpProcBindClause() meets unacceptable default option value:"<<dv<<endl;
      ROSE_ASSERT (false);
      break;
  }    
  curprint(string(")"));
}

void UnparseLanguageIndependentConstructs::unparseOmpAtomicClause(SgOmpClause* clause, SgUnparse_Info& info)
{
  ROSE_ASSERT(clause != NULL);
  SgOmpAtomicClause * c = isSgOmpAtomicClause(clause);
  ROSE_ASSERT(c!= NULL);
//  curprint(string(" "));
  SgOmpClause::omp_atomic_clause_enum dv = c->get_atomicity(); 
  switch (dv)
  {
    case SgOmpClause::e_omp_atomic_clause_read:
      {
        curprint(string("read"));
        break;
      }
    case SgOmpClause::e_omp_atomic_clause_write:
      {
        curprint(string("write"));
        break;
      }
    case SgOmpClause::e_omp_atomic_clause_update:
      {
        curprint(string("update"));
        break;
      }
    case SgOmpClause::e_omp_atomic_clause_capture:
      {
        curprint(string("capture"));
        break;
      }
  default:
      cerr<<"Error: "<< __FUNCTION__ <<" meets unacceptable default option value:"<<dv<<endl;
      ROSE_ASSERT (false);
      break;
  }    
}



void UnparseLanguageIndependentConstructs::unparseOmpScheduleClause(SgOmpClause* clause, SgUnparse_Info& info)
{
  ROSE_ASSERT(clause != NULL);
  SgOmpScheduleClause* c = isSgOmpScheduleClause(clause);
  ROSE_ASSERT(c!= NULL);
  curprint (string (" schedule("));
  SgOmpClause::omp_schedule_kind_enum skind = c-> get_kind ();
  switch (skind)
  {
    case SgOmpClause::e_omp_schedule_static:
      {
        curprint(string("static"));
        break;
      }
    case SgOmpClause::e_omp_schedule_dynamic:
      {
        curprint(string("dynamic"));
        break;
      }
    case SgOmpClause::e_omp_schedule_guided:
      {
        curprint(string("guided"));
        break;
      }
    case SgOmpClause::e_omp_schedule_auto :
      {
        curprint(string("auto"));
        break;
      }
    case SgOmpClause::e_omp_schedule_runtime :
      {
        curprint(string("runtime"));
        break;
      }
    default:
      cerr<<"Error: UnparseLanguageIndependentConstructs::unparseOmpScheduleClause() meets unacceptable kind option value:"<<skind<<endl;
      ROSE_ASSERT (false);
      break;
  }

  // chunk_size expression
  SgUnparse_Info ninfo(info);
  if (c->get_chunk_size())
  {
    curprint(string(" , "));
    unparseExpression(c->get_chunk_size(), ninfo);
  }

  curprint(string(")"));
}

#if 1
//! A helper function to convert reduction operators to strings
// TODO put into a better place and expose it to users.
static std::string reductionOperatorToString(SgOmpClause::omp_reduction_operator_enum ro)
{
  string result;
  switch (ro)
  {
    case SgOmpClause::e_omp_reduction_plus: 
      {
        result = "+";
        break;
      }
    case SgOmpClause::e_omp_reduction_mul: 
      {
        result = "*";
        break;
      }
    case SgOmpClause::e_omp_reduction_minus:   
      {
        result = "-";
        break;
      }
    case SgOmpClause::e_omp_reduction_bitand:  
      {
        result = "&";
        break;
      }
    case SgOmpClause::e_omp_reduction_bitor :  
      {
        result = "|";
        break;
      }
      //------------
    case SgOmpClause::e_omp_reduction_bitxor:  
      {
        result = "^";
        break;
      }
    case SgOmpClause::e_omp_reduction_logand:  
      {
        result = "&&";
        break;
      }
    case SgOmpClause::e_omp_reduction_logor :  
      {
        result = "||";
        break;
      }
    case SgOmpClause::e_omp_reduction_and  : 
      {
        result = ".and.";
        break;
      }
    case SgOmpClause::e_omp_reduction_or : 
      {
        result = ".or.";
        break;
      }
     //------------
    case SgOmpClause::e_omp_reduction_eqv:   
      {
        result = ".eqv.";
        break;
      }
    case SgOmpClause::e_omp_reduction_neqv : 
      {
        result = ".neqv.";
        break;
      }
    case SgOmpClause::e_omp_reduction_max  : 
      {
        result = "max";
        break;
      }
    case SgOmpClause::e_omp_reduction_min  : 
      {
        result = "min";
        break;
      }
    case SgOmpClause::e_omp_reduction_iand : 
      {
        result = "iand";
        break;
      }

      //------------
    case SgOmpClause::e_omp_reduction_ior  : 
      {
        result = "ior";
        break;
      }
    case SgOmpClause::e_omp_reduction_ieor : 
      {
        result = "ieor";
        break;
      }
    default:
      {
        cerr<<"Error: unhandled operator type reductionOperatorToString():"<< ro <<endl;
        ROSE_ASSERT(false);
      }
  }
  return result;
}

//! A helper function to convert dependence type to strings
// TODO put into a better place and expose it to users.
static std::string dependenceTypeToString(SgOmpClause::omp_dependence_type_enum ro)
{
  string result;
  switch (ro)
  {
    case SgOmpClause::e_omp_depend_in: 
      {
        result = "in";
        break;
      }
    case SgOmpClause::e_omp_depend_out: 
      {
        result = "out";
        break;
      }
    case SgOmpClause::e_omp_depend_inout:   
      {
        result = "inout";
        break;
      }
    default:
      {
        cerr<<"Error: unhandled operator type"<<__func__<< "():"<< ro <<endl;
        ROSE_ASSERT(false);
      }
  }
  return result;
}


static std::string mapOperatorToString(SgOmpClause::omp_map_operator_enum ro)
{
  string result;
  switch (ro)
  {
    case SgOmpClause::e_omp_map_tofrom: 
      {
        result = "tofrom";
        break;
      }
    case SgOmpClause::e_omp_map_to: 
      {
        result = "to";
        break;
      }
    case SgOmpClause::e_omp_map_from:   
      {
        result = "from";
        break;
      }
    case SgOmpClause::e_omp_map_alloc:  
      {
        result = "alloc";
        break;
      }
   default:
      {
        cerr<<"Error: unhandled operator type MapOperatorToString():"<< ro <<endl;
        ROSE_ASSERT(false);
      }
  }
  return result;
}

static std::string distPolicyToString(SgOmpClause::omp_map_dist_data_enum ro)
{
  string result;
  switch (ro)
  {
    case SgOmpClause::e_omp_map_dist_data_duplicate: 
      {
        result = "DUPLICATE";
        break;
      }
    case SgOmpClause::e_omp_map_dist_data_block: 
      {
        result = "BLOCK";
        break;
      }
    case SgOmpClause::e_omp_map_dist_data_cyclic:   
      {
        result = "CYCLIC";
        break;
      }
    default:
      {
        cerr<<"Error: unhandled dist data policy type mapDistPolicyToString():"<< ro <<endl;
        ROSE_ASSERT(false);
      }
  }
  return result;
}

// Generate dist_data(p1, p2, p3)
void UnparseLanguageIndependentConstructs::unparseMapDistDataPoliciesToString (std::vector< std::pair< SgOmpClause::omp_map_dist_data_enum, SgExpression * > > policies, SgUnparse_Info& info) 
{
  curprint(string(" dist_data("));
  for (size_t i =0; i< policies.size(); i++)
  {
    curprint(distPolicyToString (policies[i].first)); 
    if (policies[i].second != NULL)
    {
      curprint(string("("));
      unparseExpression(policies[i].second, info);
      curprint(string(")"));
      //      cerr<<"Error: unhandled dist_data policy with expression option in mapDistDataPoliciesToString() of unparseLanguageIndependentConstructs.C"<<endl;
      //      ROSE_ASSERT(false);
    }
    if (i!= policies.size() -1)
      curprint(string(","));
  }

  curprint(string(")"));
}

#endif

//! Unparse an OpenMP clause with a variable list
void UnparseLanguageIndependentConstructs::unparseOmpVariablesClause(SgOmpClause* clause, SgUnparse_Info& info)
{
  ROSE_ASSERT(clause != NULL);
  SgOmpVariablesClause* c= isSgOmpVariablesClause (clause);  
  ROSE_ASSERT(c!= NULL);
  bool is_map = false;
  bool is_depend= false;
  // unparse the  clause name first
  switch (c->variantT())
  {
    case V_SgOmpCopyinClause:
      curprint(string(" copyin("));
      break;
    case V_SgOmpCopyprivateClause:
      curprint(string(" copyprivate("));
      break;
    case V_SgOmpFirstprivateClause:
      curprint(string(" firstprivate("));
      break;
    case V_SgOmpLastprivateClause:
      curprint(string(" lastprivate("));
      break;
    case V_SgOmpPrivateClause:
      curprint(string(" private("));
      break;
    case V_SgOmpUniformClause:
      curprint(string(" uniform("));
      break;
    case V_SgOmpAlignedClause:
      curprint(string(" aligned("));
      break;
    case V_SgOmpReductionClause:
      {
        curprint(string(" reduction("));
        //reductionOperatorToString() will handle language specific issues 
        curprint(reductionOperatorToString(isSgOmpReductionClause(c)->get_operation()));
        curprint(string(" : "));
        break;
      }
    case V_SgOmpDependClause:
      {
        curprint(string(" depend("));
        curprint(dependenceTypeToString(isSgOmpDependClause(c)->get_dependence_type()));
        curprint(string(" : "));
        is_depend = true; 
        break;
      }
    case V_SgOmpLinearClause:
      curprint(string(" linear("));
      break;
    case V_SgOmpMapClause:
      {
        is_map = true;
        curprint(string(" map("));
        curprint(mapOperatorToString(isSgOmpMapClause(c)->get_operation()));
        curprint(string(" : "));
      break;
      }
 
    case V_SgOmpSharedClause:
      curprint(string(" shared("));
      break;
    default:
      cerr<<"Error: unhandled clause type in UnparseLanguageIndependentConstructs::unparseOmpVariablesClause ():"<< clause->class_name()<<endl;
      ROSE_ASSERT(false);
      break;
  }

  // prepare array dimension info for map variables
  std::map<SgSymbol*, std::vector<std::pair<SgExpression*, SgExpression*> > > dims;
  std::map< SgSymbol *, std::vector< std::pair< SgOmpClause::omp_map_dist_data_enum, SgExpression * > > > dist_policies; 
  if (is_map)
  {
    SgOmpMapClause * m_clause = isSgOmpMapClause (clause);
    ROSE_ASSERT (m_clause != NULL);
    dims = m_clause->get_array_dimensions();
    dist_policies = m_clause->get_dist_data_policies();
  } 
  else if (is_depend) // task depend(A[i:BS][j:BS]) , is also stored as array section. 
   // TODO: long term, we need a dedicated array section AST node
  {
    SgOmpDependClause* m_clause = isSgOmpDependClause (clause);
    ROSE_ASSERT (m_clause != NULL);
    dims = m_clause->get_array_dimensions();
  }

  //unparse variable list then
  SgExpressionPtrList::iterator p = c->get_variables()->get_expressions().begin();
  while ( p != c->get_variables()->get_expressions().end() )
  {
    // We now try to put array reference expression into variable list.
    if (SgPntrArrRefExp* aref = isSgPntrArrRefExp(*p)) 
    {
      // curprint (aref->unparseToString()); // This does not work!
      SgUnparse_Info ninfo(info);
      unparseExpression(aref, ninfo);
    }
    else if (SgVarRefExp* vref= isSgVarRefExp(*p))
    {
      SgInitializedName* init_name = vref->get_symbol()->get_declaration();           
      SgName tmp_name  = init_name->get_name();
      curprint( tmp_name.str());
      SgVariableSymbol * sym  = isSgVarRefExp(*p)->get_symbol();
      ROSE_ASSERT (sym != NULL);
      if (is_map)
      {
        std::vector<std::pair<SgExpression*, SgExpression*> > bounds = dims[sym];
        if (bounds.size() >0)
        {
          std::vector<std::pair<SgExpression*, SgExpression*> >:: const_iterator iter;
          for (iter = bounds.begin(); iter != bounds.end(); iter ++)
          {
            SgUnparse_Info ninfo(info);
            std::pair<SgExpression*, SgExpression*> bound  = (*iter);
            SgExpression* lower = bound.first;
            SgExpression* upper = bound.second;
            ROSE_ASSERT (lower != NULL);
            ROSE_ASSERT (upper != NULL);

            curprint(string("["));
            //          curprint(lower->unparseToString());
            unparseExpression(lower, ninfo);
            curprint(string(":"));
            //          curprint(upper->unparseToString());
            unparseExpression(upper, ninfo);
            curprint(string("]"));

            std::vector< std::pair< SgOmpClause::omp_map_dist_data_enum, SgExpression * > > policies = dist_policies[sym];
            if (policies.size() !=0)
              unparseMapDistDataPoliciesToString (policies, ninfo);
            //curprint(mapDistDataPoliciesToString (policies));

          } // end for
        } // end if has bounds
      } // end if map 
      else if (is_depend)
      {
        std::vector<std::pair<SgExpression*, SgExpression*> > bounds = dims[sym];
        if (bounds.size() >0)
        {
          std::vector<std::pair<SgExpression*, SgExpression*> >:: const_iterator iter;
          for (iter = bounds.begin(); iter != bounds.end(); iter ++)
          {
            SgUnparse_Info ninfo(info);
            std::pair<SgExpression*, SgExpression*> bound  = (*iter);
            SgExpression* lower = bound.first;
            SgExpression* upper = bound.second;
            ROSE_ASSERT (lower != NULL);
            ROSE_ASSERT (upper != NULL);

            curprint(string("["));
            unparseExpression(lower, ninfo);
            curprint(string(":"));
            unparseExpression(upper, ninfo);
            curprint(string("]"));
          } // end for
        } // end if has bounds
      }
    }
    else
    {
      cerr<<"Unhandled type of variable in a varlist:"<< (*p)->class_name()<<endl;
      ROSE_ASSERT (false);
    }

    // output the optional dimension info for map() variable 
    // Move to the next argument
    p++;

    // Check if this is the last argument (output a "," separator if not)
    if (p != c->get_variables()->get_expressions().end())
    {
      curprint( ",");
    }
  }

  // optional :step  for linear(list:step)
  if (isSgOmpLinearClause(c) && isSgOmpLinearClause(c)->get_step())
  {
    curprint(string(":"));
    unparseExpression(isSgOmpLinearClause(c)->get_step(), info);
  }
  
   // optional :alignment for aligned(list:alignment)
  if (isSgOmpAlignedClause(c) && isSgOmpAlignedClause(c)->get_alignment())
  {
    curprint(string(":"));
    unparseExpression(isSgOmpAlignedClause(c)->get_alignment(), info);
  }
 
  curprint(string(")"));
}

void UnparseLanguageIndependentConstructs::unparseOmpExpressionClause(SgOmpClause* clause, SgUnparse_Info& info)
{
  ROSE_ASSERT(clause != NULL);
  SgOmpExpressionClause * c = isSgOmpExpressionClause (clause);
  ROSE_ASSERT  (c);
  SgOmpExpressionClause* exp_clause = isSgOmpExpressionClause(c);
  ROSE_ASSERT(exp_clause);
 

  // ordered (n) vs ordered : (n) is optional
  if (isSgOmpOrderedClause(c) && (exp_clause->get_expression() == NULL))
  {  
    curprint(string(" ordered"));
    return; 
  }  

  if (isSgOmpCollapseClause(c))
    curprint(string(" collapse("));
  else if (isSgOmpIfClause(c))
    curprint(string(" if("));
  else if (isSgOmpOrderedClause(c))
    curprint(string(" ordered("));
  else if (isSgOmpFinalClause(c))
    curprint(string(" final("));
  else if (isSgOmpPriorityClause(c))
    curprint(string(" priority("));
  else if (isSgOmpNumThreadsClause(c))
    curprint(string(" num_threads("));
  else if (isSgOmpDeviceClause(c))
    curprint(string(" device("));
  else if (isSgOmpSafelenClause(c))
    curprint(string(" safelen("));
  else if (isSgOmpSimdlenClause(c))
    curprint(string(" simdlen("));
  else {
    cerr<<"Error: unacceptable clause type within unparseOmpExpressionClause():"<< clause->class_name()<<endl;
    ROSE_ASSERT(false);
  }    

  // unparse the expression
  SgUnparse_Info ninfo(info);
  if (exp_clause->get_expression())
    unparseExpression(exp_clause->get_expression(), ninfo);
  else
  {
    cerr<<"Error: missing expression within unparseOmpExpressionClause():"<< clause->class_name()<<endl;
    ROSE_ASSERT(false);
  }

  curprint(string(")"));
}      

// Entry point for unparsing OpenMP clause
void UnparseLanguageIndependentConstructs::unparseOmpClause(SgOmpClause* clause, SgUnparse_Info& info)
{
  ROSE_ASSERT(clause != NULL);
  switch (clause->variantT())
  {
    case V_SgOmpDefaultClause:
      {
        unparseOmpDefaultClause(isSgOmpDefaultClause(clause),info);
        break;
      }
    case V_SgOmpProcBindClause:
      {
        unparseOmpProcBindClause(isSgOmpProcBindClause(clause),info);
        break;
      }
    case V_SgOmpAtomicClause:
      {
        unparseOmpAtomicClause(isSgOmpAtomicClause(clause),info);
        break;
      }
 
    case V_SgOmpNowaitClause:
      {
        curprint(string(" nowait"));
        break;
      }
    case V_SgOmpInbranchClause:
      {
        curprint(string(" inbranch"));
        break;
      }
     case V_SgOmpNotinbranchClause:
      {
        curprint(string(" notinbranch"));
        break;
      }
#if 0  // this becomes an expression clause since OpenMP 4.5
    case V_SgOmpOrderedClause:
      {
        curprint(string(" ordered"));
        break;
      }
#endif      
    case V_SgOmpUntiedClause:
      {
        curprint(string(" untied"));
        break;
      }
    case V_SgOmpMergeableClause:
      {
        curprint(string(" mergeable"));
        break;
      }
    case V_SgOmpBeginClause:
      {
        curprint(string(" begin"));
        break;
      }
    case V_SgOmpEndClause:
      {
        curprint(string(" end"));
        break;
      }
    case V_SgOmpScheduleClause:
      {
        unparseOmpScheduleClause(isSgOmpScheduleClause(clause), info);
        break;
      }
    case V_SgOmpDeviceClause:
    case V_SgOmpCollapseClause:
    case V_SgOmpIfClause:  
    case V_SgOmpFinalClause:  
    case V_SgOmpPriorityClause:  
    case V_SgOmpNumThreadsClause:  
    case V_SgOmpSafelenClause:  
    case V_SgOmpSimdlenClause:  
    case V_SgOmpOrderedClause:
      //case V_SgOmpExpressionClause: // there should be no instance for this clause
      {
        unparseOmpExpressionClause(isSgOmpExpressionClause(clause), info);
        break; 
      }                       
    case V_SgOmpCopyprivateClause:
    case V_SgOmpCopyinClause:
    case V_SgOmpFirstprivateClause:
    case V_SgOmpLastprivateClause:
    case V_SgOmpPrivateClause:
    case V_SgOmpReductionClause:
    case V_SgOmpDependClause:
    case V_SgOmpMapClause:
    case V_SgOmpSharedClause:
    case V_SgOmpUniformClause:
    case V_SgOmpAlignedClause:
    case V_SgOmpLinearClause: 
      {     
        unparseOmpVariablesClause(isSgOmpVariablesClause(clause), info);
        break;
      }     
   default:
      {
        cerr<<"Unhandled OpenMP clause type in UnparseLanguageIndependentConstructs::unparseOmpClause():"<<clause->class_name()<<endl;
        ROSE_ASSERT(false);
        break;  
      }
  }    
}

//! This is not intended to be directly called anytime.
//  Individual languages should have implemented their own OpenMP prefixes 
void UnparseLanguageIndependentConstructs::unparseOmpPrefix(SgUnparse_Info& info)
{
  cerr<<"Error: UnparseLanguageIndependentConstructs::unparseOmpPrefix() should not be called directly!"<<endl;
  cerr<<"Individual languages should have implemented their own OpenMP prefixes."<<endl; 
  ROSE_ASSERT (false);
}
// simple directives: atomic, section, taskwait, barrier
void UnparseLanguageIndependentConstructs::unparseOmpSimpleStatement(SgStatement * stmt,  SgUnparse_Info& info)
{
  ROSE_ASSERT (stmt != NULL);
  unparseOmpDirectivePrefixAndName(stmt, info); 
  unp->u_sage->curprint_newline();
  SgOmpBodyStatement* b_stmt = isSgOmpBodyStatement(stmt);
  if (b_stmt)
  {
    ROSE_ASSERT (stmt->variantT() == V_SgOmpSectionStatement);
    SgUnparse_Info ninfo(info);
    unparseStatement(b_stmt->get_body(), ninfo);
  }
}

//----- refactor unparsing for threadprivate and flush ???
void UnparseLanguageIndependentConstructs::unparseOmpFlushStatement(SgStatement* stmt,     SgUnparse_Info& info)
{
  ROSE_ASSERT (stmt != NULL);
  SgOmpFlushStatement * s = isSgOmpFlushStatement(stmt);
  ROSE_ASSERT (s!= NULL);

  unparseOmpDirectivePrefixAndName(stmt, info); 
  if (s->get_variables().size()>0)
    curprint(string ("("));
  //unparse variable list then
  SgVarRefExpPtrList::iterator p = s->get_variables().begin();
  while ( p != s->get_variables().end() )
  {
    ROSE_ASSERT ((*p)->get_symbol() != NULL);
    SgInitializedName* init_name = (*p)->get_symbol()->get_declaration();
    ROSE_ASSERT (init_name != NULL);
    SgName tmp_name  = init_name->get_name();
    curprint( tmp_name.str());

    // Move to the next argument
    p++;

    // Check if this is the last argument (output a "," separator if not)
    if (p != s->get_variables().end())
    {
      curprint( ",");
    }
  }
  if (s->get_variables().size()>0)
    curprint (string (")"));
  unp->u_sage->curprint_newline();
}

void UnparseLanguageIndependentConstructs::unparseOmpDeclareSimdStatement(SgStatement* stmt,     SgUnparse_Info& info)
{
  ROSE_ASSERT (stmt != NULL);
  SgOmpDeclareSimdStatement * s = isSgOmpDeclareSimdStatement(stmt);
  ROSE_ASSERT (s!= NULL);
//cout<<"debug "<<s->get_clauses().size()<<endl;
  unparseOmpDirectivePrefixAndName(stmt, info); 

  unparseOmpBeginDirectiveClauses(stmt, info);
  unp->u_sage->curprint_newline();

}


void UnparseLanguageIndependentConstructs::unparseOmpThreadprivateStatement(SgStatement* stmt,     SgUnparse_Info& info)
{
  ROSE_ASSERT (stmt != NULL);
  SgOmpThreadprivateStatement * s = isSgOmpThreadprivateStatement(stmt);
  ROSE_ASSERT (s!= NULL);
  unparseOmpDirectivePrefixAndName(stmt, info); 
  curprint(string ("("));
  //unparse variable list then
  SgVarRefExpPtrList::iterator p = s->get_variables().begin();
  while ( p != s->get_variables().end() )
  {
    ROSE_ASSERT ( (*p)->get_symbol() != NULL);
    SgInitializedName* init_name = (*p)->get_symbol()->get_declaration();
    ROSE_ASSERT (init_name);
    SgName tmp_name  = init_name->get_name();
    curprint( tmp_name.str());

    // Move to the next argument
    p++;

    // Check if this is the last argument (output a "," separator if not)
    if (p != s->get_variables().end())
    {
      curprint( ",");
    }
  } // end while

  curprint (string (")"));
  unp->u_sage->curprint_newline();
}

// A helper function to just unparse omp-prefix directive-name, without bothering clauses
// examples: 
//  #pragma omp parallel, 
//  !$omp parallel, 
void UnparseLanguageIndependentConstructs::unparseOmpDirectivePrefixAndName (SgStatement* stmt,     SgUnparse_Info& info)
{
  ROSE_ASSERT(stmt != NULL);
  unp->u_sage->curprint_newline();
  switch (stmt->variantT())
  {
      case V_SgOmpAtomicStatement:
      {
        unparseOmpPrefix(info);
        curprint(string ("atomic "));
        break;
      }
      case V_SgOmpSectionStatement:
      {
        unparseOmpPrefix(info);
        curprint(string ("section "));
        break;
      }
      case V_SgOmpTaskStatement:
      {
        unparseOmpPrefix(info);
        curprint(string ("task "));
        break;
      }
       case V_SgOmpTaskwaitStatement:
      {
        unparseOmpPrefix(info);
        curprint(string ("taskwait "));
        break;
      }
     case V_SgOmpFlushStatement:
      {
        unparseOmpPrefix(info);
        curprint(string ("flush "));
        break;
      }
      case V_SgOmpThreadprivateStatement:
      {
        unparseOmpPrefix(info);
        curprint(string ("threadprivate "));
        break;
      }
     case V_SgOmpBarrierStatement:
      {
        unparseOmpPrefix(info);
        curprint(string ("barrier "));
        break;
      }
    case V_SgOmpParallelStatement:
      {
        unparseOmpPrefix(info);
        curprint(string ("parallel "));
        break;
      }
    case V_SgOmpTargetStatement:
      {
        unparseOmpPrefix(info);
        curprint(string ("target "));
        break;
      }
    case V_SgOmpTargetDataStatement:
      {
        unparseOmpPrefix(info);
        curprint(string ("target data "));
        break;
      }
     case V_SgOmpCriticalStatement:
      {
        unparseOmpPrefix(info);
        curprint(string ("critical "));
        if (isSgOmpCriticalStatement(stmt)->get_name().getString()!="")
        {
          curprint (string ("("));
          curprint (isSgOmpCriticalStatement(stmt)->get_name().getString());
          curprint (string (")"));
        }
        break;
      }
         case V_SgOmpForStatement:
      {
        unparseOmpPrefix(info);
        curprint(string ("for "));
        break;
      }
         case V_SgOmpForSimdStatement:
      {
        unparseOmpPrefix(info);
        curprint(string ("for simd "));
        break;
      }
        case V_SgOmpDoStatement:
      {
        unparseOmpPrefix(info);
        curprint(string ("do "));
        break;
      }
       case V_SgOmpMasterStatement:
      {
        unparseOmpPrefix(info);
        curprint(string ("master "));
        break;
      }
      case V_SgOmpOrderedStatement:
      {
        unparseOmpPrefix(info);
        curprint(string ("ordered "));
        break;
      }
    case V_SgOmpWorkshareStatement:
      {
        unparseOmpPrefix(info);
        curprint(string ("workshare "));
        break;
      }
      case V_SgOmpSingleStatement:
      {
        unparseOmpPrefix(info);
        curprint(string ("single "));
        break;
      }
      case V_SgOmpSimdStatement:
      {
        unparseOmpPrefix(info);
        curprint(string ("simd"));
        break;
      }
      case V_SgOmpDeclareSimdStatement:
      {
        unparseOmpPrefix(info);
        curprint(string ("declare simd"));
        break;
      }
     case V_SgOmpSectionsStatement:
      {
        unparseOmpPrefix(info);
        curprint(string ("sections "));
        break;
      }
    default:
      {
        cerr<<"error: unacceptable OpenMP directive type within unparseOmpDirectivePrefixAndName(): "<<stmt->class_name()<<endl;
        ROSE_ASSERT(false);
        break;
      }
  } // end switch
}

// This is necessary since some clauses should only appear with the begin part of a directive
// C/C++ derivation: unparse all clauses attached to the directive
// Fortran derivation: unparse most clauses except a few nowait, copyprivate clauses which should appear with the end directive
void UnparseLanguageIndependentConstructs::unparseOmpBeginDirectiveClauses      (SgStatement* stmt,     SgUnparse_Info& info)
{
  cerr<<"Error: UnparseLanguageIndependentConstructs::unparseOmpBeginDirectiveClauses() should not be called directly"<<endl;
  ROSE_ASSERT (false);
}

// Output the corresponding end directive text for an OpenMP AST nodes for directive
void UnparseLanguageIndependentConstructs::unparseOmpEndDirectivePrefixAndName (SgStatement* stmt,     SgUnparse_Info& info)
{
  ROSE_ASSERT(stmt != NULL);
  // This one should do nothing by default
  // Only Fortran derived implementation should output something there
}

// Default behavior for unparsing clauses appearing with 'end xxx'
void UnparseLanguageIndependentConstructs::unparseOmpEndDirectiveClauses        (SgStatement* stmt,     SgUnparse_Info& info)
{
  // it should not do anything here , and for C/C++ subclass
  // Derived implementation in Fortran should do something.
}

// This is a catch-all helper function 
void UnparseLanguageIndependentConstructs::unparseOmpGenericStatement (SgStatement* stmt,     SgUnparse_Info& info)
{
  ROSE_ASSERT(stmt != NULL);
  // unparse the begin directive
  unparseOmpDirectivePrefixAndName (stmt, info);
  // unparse the begin directive's clauses
  unparseOmpBeginDirectiveClauses(stmt, info);
  unp->u_sage->curprint_newline();

  // unparse the body, if exists. 
  SgOmpBodyStatement* b_stmt = isSgOmpBodyStatement(stmt);
  if (b_stmt)
  {
    SgUnparse_Info ninfo(info);
    unparseStatement(b_stmt->get_body(), ninfo);
  }
  else
 {
   //TODO assertion for must-have bodies
  }

  // unparse the end directive and name 
  unparseOmpEndDirectivePrefixAndName (stmt, info);
  // unparse the end directive's clause
  unparseOmpEndDirectiveClauses(stmt, info);

} // end unparseOmpGenericStatement


PrecedenceSpecifier
UnparseLanguageIndependentConstructs::getPrecedence(SgExpression* expr)
   {
  // DQ (11/24/2007): This is a redundant mechanism for computing the precedence of expressions (NO!)
  // DQ (4/20/2013): Actually, this is the support for operator precedence that is correctly handling
  // overloaded operators to be the precedence of the operators that they are overloading, so this 
  // is an important part of the unparser infrastructure.  There is also a specifictaion of 
  // operator precedence a static data members on each expression IR node, and this function could 
  // and likely should use the values that are set there to avoud some level of redundancy.

  // DQ (2/5/2015): Added note from google search for precedence of the noexcept operator.
  // The standard itself doesn't specify precedence levels. They are derived from the grammar.
  // const_cast, static_cast, dynamic_cast, reinterpret_cast, typeid, sizeof..., noexcept and 
  // alignof are not included since they are never ambiguous. 

#if PRINT_DEVELOPER_WARNINGS
     printf ("This is a redundant mechanism for computing the precedence of expressions \n");
#endif

#if 0
     printf ("In getPrecedence(): expr = %p = %s \n",expr,expr->class_name().c_str());
#endif

  // This call to GetOperatorVariant will map overloaded operators for syntax (e.g. operator+()) 
  // to the associated operators (e.g. +) so that the overloaded operators will have the same 
  // precedence as the operators they are overloading.
     int variant = GetOperatorVariant(expr);

#if 0
     printf ("In getPrecedence(): variant = %d = %s \n",variant,Cxx_GrammarTerminalNames[variant].name.c_str());
     curprint(string("\n/* In getPrecedence(): variant = ") + Cxx_GrammarTerminalNames[variant].name + " */ \n");
#endif

     PrecedenceSpecifier precedence_value = 0;

     switch (variant)
        {
          case V_SgExprListExp:
          case V_SgCommaOpExp:       // return 1;
                                     precedence_value = 1; break;

          case V_SgAssignOp:         // return 2;
       // DQ (2/1/2009): Added precedence for SgPointerAssignOp (Fortran 90)
          case V_SgPointerAssignOp:  // return 2;
          case V_SgPlusAssignOp:     // return 2;
          case V_SgMinusAssignOp:    // return 2;
          case V_SgAndAssignOp:      // return 2;
          case V_SgIorAssignOp:      // return 2;
          case V_SgMultAssignOp:     // return 2;
          case V_SgDivAssignOp:      // return 2;
          case V_SgModAssignOp:      // return 2;
          case V_SgXorAssignOp:      // return 2;
          case V_SgLshiftAssignOp:   // return 2;
          case V_SgRshiftAssignOp:   // return 2;
                                     precedence_value = 2; break;

           case V_SgConditionalExp:   // return 3;
                                     precedence_value = 3; break;

          case V_SgOrOp:             // return 4;
                                     precedence_value = 4; break;

          case V_SgAndOp:            // return 5;
                                     precedence_value = 5; break;

          case V_SgBitOrOp:          // return 6;
                                     precedence_value = 6; break;

          case V_SgBitXorOp:         // return 7;
                                     precedence_value = 7; break;

          case V_SgBitAndOp:         // return 8;
                                     precedence_value = 8; break;

          case V_SgEqualityOp:       // return 9;
          case V_SgNotEqualOp:       // return 9;
                                     precedence_value = 9; break;

          case V_SgLessThanOp:       // return 10;
          case V_SgGreaterThanOp:    // return 10;
          case V_SgLessOrEqualOp:    // return 10;
          case V_SgGreaterOrEqualOp: // return 10;
                                     precedence_value = 10; break;


       // DQ (1/26/2013): I think this is wrong, "<<" and ">>" have value 7 (lower than "==") (see test2013_42.C).
       // case V_SgLshiftOp:         return 11;
       // case V_SgRshiftOp:         return 11;
          case V_SgLshiftOp:         // return 11;
          case V_SgRshiftOp:         // return 11;

          case V_SgJavaUnsignedRshiftOp: // return 11;
                                     precedence_value = 11; break;

          case V_SgAddOp:            // return 12;
                                     precedence_value = 12; break;


       // DQ (2/1/2009): Added operator (which should have been here before)
          case V_SgMinusOp:          // return 15;
          case V_SgUnaryAddOp:       // return 15;
                                     precedence_value = 15; break;


          case V_SgSubtractOp:       // return 12;
                                     precedence_value = 12; break;

          case V_SgMultiplyOp:       // return 13;
          case V_SgIntegerDivideOp:
          case V_SgDivideOp:         // return 13;
          case V_SgModOp:            // return 13;
                                     precedence_value = 13; break;

          case V_SgDotStarOp:        // return 14;
          case V_SgArrowStarOp:      // return 14;
                                     precedence_value = 14; break;

          case V_SgPlusPlusOp:       // return 15;
          case V_SgMinusMinusOp:     // return 15;
          case V_SgBitComplementOp:  // return 15;
          case V_SgNotOp:            // return 15;
          case V_SgPointerDerefExp:
          case V_SgAddressOfOp:
          case V_SgUpcLocalsizeofExpression:   // \pp 03/03/11
          case V_SgSizeOfOp:         // return 15;

       // DQ (6/20/2013): Added support for __alignof__ operator.
          case V_SgAlignOfOp:        // return 15;

       // DQ (2/5/2015): Need to define the precedence of this new C++11 operator.
       // The rules say that this can never be ambigious, so it's precedence is not important (I am not yet clear on this point).
          case V_SgNoexceptOp:        // return 15;

       // DQ (2/6/2015): Need to define the precedence of this new C++11 operator (but it is not clear to me that this is correcct).
       // I am so far unable to find data on the precedence of the lambda expression.
          case V_SgLambdaExp:        // return 15;
                                     precedence_value = 15; break;


          case V_SgFunctionCallExp:
             {
            // DQ (4/17/2013): If this is an overloaded operator then we can't just treat it like a normal function (must detect if it is an overloaded operator).
               SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(expr);
               ROSE_ASSERT(functionCallExp != NULL);
#if 1
               if (functionCallExp->get_uses_operator_syntax() == true)
                  {
                 // DQ (3/5/2017): Converted to use message logging, but the mechanism is not supported here yet.
                    mprintf ("WARNING: In getPrecedence(): case V_SgFunctionCallExp: If this is an overloaded operator then the precedence should be that of the operator being overloaded (not zero). \n");
                    mprintf ("   --- functionCallExp = %p functionCallExp->get_uses_operator_syntax() = %s \n",functionCallExp,functionCallExp->get_uses_operator_syntax() ? "true" : "false");
                  }
#endif
            // ROSE_ASSERT(functionCallExp->get_uses_operator_syntax() == false);
#if 0
               if (functionCallExp != NULL && functionCallExp->get_uses_operator_syntax() == true)
                  {
                 // This is an overloaded operator and has the precedence of the operator that is being overloaded.

                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
                  }
#endif
               // return 16;
               precedence_value = 16;
               break;
             }

          case V_SgPntrArrRefExp:    // return 16;
          case V_SgArrowExp:         // return 16;
          case V_SgDotExp:           // return 16;

          case V_SgImpliedDo:        // return 16;

          case V_SgLabelRefExp:      // return 16;
          case V_SgActualArgumentExpression: // return 16;

       // DQ (2/1/2009): Added support for Fortran operator.
          case V_SgExponentiationOp: // return 16;
                                     precedence_value = 16; break;

          case V_SgConcatenationOp:  // return 11;
                                     precedence_value = 11; break;

          case V_SgSubscriptExpression:  // return 16;  // Make the same as for SgPntrArrRefExp
                                     precedence_value = 16; break;

       // DQ (2/1/2009): This was missing from before.
          case V_SgThisExp:          // return 0;
                                     precedence_value = 0; break;

          case V_SgCastExp:
             {
            // DQ (4/17/2013): If this is a compiler generated cast then it will not be output and the precedence should reflect that.
               SgCastExp* castExp = isSgCastExp(expr);
               if (castExp == NULL)
                  {
                 // This is likely the case of an overloaded operator (but verify this).
                    SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(expr);
                    if (functionCallExp != NULL)
                       {
                      // This should be an overloaded cast expression.
                         SgName name;
                         SgExpression* function = functionCallExp->get_function();
                         ROSE_ASSERT(function != NULL);
                         SgBinaryOp* binaryOperator = isSgBinaryOp(function);
                         if (binaryOperator != NULL)
                            {
                              SgExpression* rhs = binaryOperator->get_rhs_operand();
                              ROSE_ASSERT(rhs != NULL);

                           // This could be a member function from a class or a templated class.
                              SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(rhs);
                              if (mfunc_ref != NULL)
                                 {
                                   name = mfunc_ref->get_symbol()->get_name();
                                 }
                                else
                                 {
                                   SgTemplateMemberFunctionRefExp* template_mfunc_ref = isSgTemplateMemberFunctionRefExp(rhs);
                                   ROSE_ASSERT(template_mfunc_ref != NULL);
                                   name = template_mfunc_ref->get_symbol()->get_name();
                                 }
                            }
                           else
                            {
                           // This is a non-member function reference: SgFunctionRefExp.

                           // This could be a function from a class or a templated class.
                              SgFunctionRefExp* func_ref = isSgFunctionRefExp(function);
                              if (func_ref != NULL)
                                 {
                                   name = func_ref->get_symbol()->get_name();
#if 0
                                   printf ("In getPrecedence(): Get function name = %s \n",name.str());
#endif
                                 }
                                else
                                 {
                                   SgTemplateFunctionRefExp* template_func_ref = isSgTemplateFunctionRefExp(function);
                                   ROSE_ASSERT(template_func_ref != NULL);
                                   name = template_func_ref->get_symbol()->get_name();
                                 }
#if 0
                              printf ("ERROR: case V_SgCastExp: functionCallExp = %p associated function was not a SgBinaryOp: function = %p = %s \n",functionCallExp,function,function->class_name().c_str());
                              ROSE_ASSERT(false);
#endif
                            }

                         string func_name = name.str();
#if 0
                         printf ("case V_SgCastExp: functionCallExp = %p associated function func_name = %s \n",functionCallExp,func_name.c_str());
#endif
                         ROSE_ASSERT(func_name.find("operator") != string::npos);

                       }
                      else
                       {
                         printf ("ERROR: case V_SgCastExp: If this is not a SgCastExp (or overloaded cast operator) then what is it: expr = %p = %s = %s \n",expr,expr->class_name().c_str(),SageInterface::get_name(expr).c_str());
                         ROSE_ASSERT(false);
                       }
                  }
                 else
                  {
                 // This is a regular SgCastExp expression.
                  }

            // Since both SgCastExp and overloaded cast operators may be compiler generated we just want to know if this is compiler generated (so use "expr" instead of "castExp" below).
               ROSE_ASSERT(expr->get_startOfConstruct() != NULL);
               if (expr->get_startOfConstruct()->isCompilerGenerated() == true)
                  {
                 // It is a problem to return zero since this causes the calling function to return "true" for needing parenthesis.
                 // So I have returned the value "1".
#if 0
                    printf ("In getPrecedence(): case V_SgCastExp: If this is an overloaded operator then the precedence should be that of the operator being overloaded (not zero). \n");
                    printf ("   --- castExp = %p isCompilerGenerated = true (return precedence = 1) \n",castExp);
#endif
                 // If this is compiler generated then we have to look at the precedence of the unary operator's operand.
                 // return 1;
                    if (castExp != NULL)
                       {
                      // return getPrecedence(castExp->get_operand());
                         precedence_value = getPrecedence(castExp->get_operand()); 
                       }
                      else
                       {
                      // DQ (8/29/2014): Changed the precedence to avoid over parenthization of user-defined conversion operators 
                      // (see test2006_185.C).  If it is compiler generated then we can argue that it has a higher precedence and 
                      // this avoids the redundant parenthization which is a bug for GNU 4.4 and other versions of GNU.

                      // If this is compiler generated then we have to look at the precedence of the unary operator's operand.
                      // printf ("WARNING: case of overloaded cast operator: If this is compiler generated then we have to look at the precedence of the unary operator's operand (returning 1) \n");
                      // return 1;
                         mprintf ("WARNING: case of overloaded cast operator: If this is compiler generated then we have to look at the precedence of the unary operator's operand (returning 16) \n");
                      // return 16;
                         precedence_value = 16; 
                       }
                  }
                 else
                  {
                    precedence_value = 0; 
                  }

            // return 0;
               break;
             }

       // DQ (11/14/2016): Added support for SgBracedInitializer (see Cxx11_tests/test2016_82.C for an example).
          case V_SgBracedInitializer:

       // DQ (8/29/2014): Added support for SgAggregateInitializer (failed in tutorial examples).
          case V_SgAggregateInitializer:

       // DQ (7/13/2013): Added support to support this kind of value (I think this is correct, but not sure).
          case V_SgTemplateParameterVal:   // return 0;

       // DQ (11/10/2014): Added support to support this C++11 value.
          case V_SgNullptrValExp:

       // DQ (5/24/2015): Added support for this type.
          case V_SgUnsignedShortVal:       // return 0;
          case V_SgShortVal:               // return 0;
          case V_SgUnsignedCharVal:        // return 0;

          case V_SgBoolValExp:             // return 0;
          case V_SgIntVal:                 // return 0;
          case V_SgThrowOp:                // return 0;
          case V_SgDoubleVal:              // return 0;
          case V_SgUnsignedIntVal:         // return 0;
          case V_SgAssignInitializer:      // return 0;
          case V_SgFloatVal:               // return 0;
          case V_SgVarArgOp:               // return 0;
          case V_SgLongDoubleVal:          // return 0;
          case V_SgLongIntVal:             // return 0;
          case V_SgLongLongIntVal:         // return 0;
          case V_SgVarArgStartOp:          // return 0;
          case V_SgNewExp:                 // return 0;
          case V_SgDeleteExp:              // return 0;
          case V_SgStringVal:              // return 0;
          case V_SgCharVal:                // return 0;
          case V_SgUnsignedLongLongIntVal: // return 0;
          case V_SgUnsignedLongVal:        // return 0;
          case V_SgComplexVal:             // return 0;
          case V_SgMatrixExp:
          case V_SgEnumVal:
                                     precedence_value = 0; break;

          case V_SgCAFCoExpression:        // return 16;
                                     precedence_value = 16; break;

       // Liao, 7/15/2009, UPC nodes
          case V_SgUpcThreads:             // return 0;
          case V_SgUpcMythread:            // return 0;
          case V_SgNullExpression:         // return 0;
       // TV (04/26/2010): CUDA nodes
          case V_SgCudaKernelExecConfig:   // return 0;
          case V_SgCudaKernelCallExp:      // return 0;
                                     precedence_value = 0; break;

       // TV (04/24/2011): Add FunctionRefExp to avoid the following Warning. It occurs
       //     after my modification for a more generic support of the original
       //     expression tree field (especially the case of FunctionRefExp used for
       //     function pointers initialisation).
       // case V_SgFunctionRefExp:    return 0;
          case V_SgFunctionRefExp:
             {
#if 0
#if 0
               printf ("In getPrecedence(): case V_SgFunctionRefExp: If this is an overloaded operator then the precedence should be that of the operator being overloaded (not zero). \n");
#endif
            // DQ (4/17/2013): If this is an overloaded operator then we can't just treat it like a normal function.
               SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(expr);
               ROSE_ASSERT(functionRefExp != NULL);
               ROSE_ASSERT(functionRefExp->get_parent() != NULL);
               SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(functionRefExp->get_parent()->get_parent());
               ROSE_ASSERT(functionCallExp != NULL);

               ROSE_ASSERT(functionCallExp->get_uses_operator_syntax() == false);
#if 0
               if (functionCallExp != NULL && functionCallExp->get_uses_operator_syntax() == true)
                  {
                 // This is an overloaded operator and has the precedence of the operator that is being overloaded.

                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
                  }
#endif
#endif
            // return 0;
               precedence_value = 0; 
               break;
             }

       // DQ (10/8/2012): Unclear if this is the correct precedence for this GNU specific feature.
       // Note that this setting is equivalent to what was being returned, so I expect it is fine since it represents no change.
          case V_SgStatementExpression:      // return 0;
                                     precedence_value = 0; break;

       // DQ (10/8/2012): Unclear if this is the correct precedence for this GNU specific feature.
       // Note that this setting is equivalent to what was being returned, so I expect it is fine since it represents no change.
          case V_SgVarRefExp:                // return 0;
                                     precedence_value = 0; break;

       // DQ (7/22/2013): I think this needs to be set so that we never output parenthesis for this case.
       // DQ (10/17/2012): Added support for SgDesignatedInitializer.
       // case V_SgDesignatedInitializer:    return 0;
       // case V_SgDesignatedInitializer:    return 16;
          case V_SgDesignatedInitializer: // return 0;
                                     precedence_value = 0; break;

       // DQ (1/26/2013): This case needs to be supported (see test2013_42.C).
          case V_SgTypeIdOp:                 // return 16;
                                     precedence_value = 16; break;

       // DQ (7/13/2013): Added support to type trait builtin functions (not clear if this is the correct value).
       // Make this the same precedence as a SgFunctionCallExp.
          case V_SgTypeTraitBuiltinOperator: // return 16;
                                     precedence_value = 16; break;

       // DQ (9/25/2013): Adding support for new IR node (C90 and C++ compound literals).
          case V_SgCompoundLiteralExp: // return 0;
                                     precedence_value = 0; break;

       // DQ (9/25/2013): Adding support for Fortran user defined binary operators (however, I am not certain this is the correct precedence).
          case V_SgUserDefinedBinaryOp: // return 0;
                                     precedence_value = 0; break;

       // DQ (9/25/2013): Adding support for C/C++ asm operator (however, I am not certain this is the correct precedence).
          case V_SgAsmOp: // return 0;
                                     precedence_value = 0; break;

       // DQ (11/10/2014): Not clear if this is the correct precedence for this C++11 expression.
          case V_SgFunctionParameterRefExp:
                                     precedence_value = 0; break;

       // DQ (4/29/2016): Not clear if this is the correct precedence for these C++11 expressions.
          case V_SgRealPartOp:
          case V_SgImagPartOp:
                                     precedence_value = 0; break;
          case V_SgNonrealRefExp:
                                     precedence_value = 0; break;

          default:
             {
            // We want this to be a printed warning (so we can catch these missing cases), but it is not worthy of calling an error since the default works fine.
               printf ("Warning: getPrecedence() in unparseLanguageIndependentConstructs.C: Undefined expression variant = %d = %s \n",variant,Cxx_GrammarTerminalNames[variant].name.c_str());
#if 1
            // DQ (9/25/2013): Temporarily added assertion to get rid of warnings (catching them and fixing them).
               printf ("Error: getPrecedence() in unparseLanguageIndependentConstructs.C: Undefined expression variant = %d = %s \n",variant,Cxx_GrammarTerminalNames[variant].name.c_str());
               ROSE_ASSERT(false);
#endif
             }
        }

#if 0
     printf ("Leaving getPrecedence(): variant = %d = %s precedence_value = %d \n",variant,Cxx_GrammarTerminalNames[variant].name.c_str(),precedence_value);
     curprint(string("\n/* Leaving getPrecedence(): variant = ") + Cxx_GrammarTerminalNames[variant].name + " precedence_value = " + StringUtility::numberToString(precedence_value) + " */ \n");
#endif

  // DQ (8/29/2014): Modified this function to make it easier to debug the precedence return values directly.
  // return 0;
     return precedence_value;
   }


AssociativitySpecifier
UnparseLanguageIndependentConstructs::getAssociativity(SgExpression* expr) 
   {
  // DQ (7/23/2013): This should match the table in: http://en.wikipedia.org/wiki/Operators_in_C_and_C%2B%2B#Operator_precedence
  // Note also that this table has the precedence in the wrong order compared to how we have listed it in ROSE.

  // I have added the case for SgCastExp, but noticed that there appear to be many incorrect entries for associativity for the 
  // other operators.  This function is called in the evaluation for added "()" using the operator precedence (obtained from
  // the function: getPrecedence()).

  // DQ (9/25/2013): It is an additional issue that some associativity rules are language dependent.  For example, I understand 
  // that Fortran supports A - B - C as A - (B - C) (right associative) where as C and C++ would treat it as (A - B) - C (left 
  // associative).  Currently all associativity is defined in terms of C/C++, this is something to fix for the Fortran.  In 
  // general we add parenthesis to support the explict handling wherever possible (I think). As a rule, Fortran relational 
  // operators are not associative.  The exponentiation operator associates right to left (right associative).  Thus, A**B**C 
  // is equal to A**(B**C) rather than (A**B)**C. All other FORTRAN operators are left to right associative (left associative)
  // (however it appears to contradict the stated rule for minus (above).

  // DQ (4/20/2018): Added assertion.
     ROSE_ASSERT(expr != NULL);

     int variant = GetOperatorVariant(expr);

#if 0
     printf ("In getAssociativity(): variant = %d = %s \n",variant,Cxx_GrammarTerminalNames[variant].name.c_str());
     curprint(string("\n/* In getAssociativity(): variant = ") + Cxx_GrammarTerminalNames[variant].name + " */ \n");
#endif

     switch (variant)
        {
       // DQ (7/23/2013): Added cast operator.
          case V_SgCastExp:
             {
               if (expr->get_file_info()->isCompilerGenerated() == true)
                  {
#if 0
                    printf ("WARNING: In getAssociativity(): We should not be calling getAssociativity(SgExpression* expr) if this is a compiler generated (implicit) cast (returing e_assoc_none) \n");
#endif
                    return e_assoc_none;
                  }
                 else
                  {
                 // The cast is right associative.
                    return e_assoc_right;
                  }
             }

          case V_SgPlusPlusOp:
          case V_SgMinusMinusOp:
             {
            // DQ (7/23/2013): The associativity of these operators depends upon if they are pre or post operators (assuming post-fix).
            // Note: post-fix is left associative, and pre-fix is right associative.

               AssociativitySpecifier associativitySpecifier = e_assoc_none;

               ROSE_ASSERT(expr != NULL);
               SgUnaryOp* unaryOp = isSgUnaryOp(expr);

            // DQ (4/20/2018): Added suppofr for function and member function operator++ and operator-- and there prefix and postfix variations.
               if (unaryOp == NULL)
                  {
#if 0
                    printf ("ERROR: unaryOp == NULL: expr = %p = %s \n",expr,expr->class_name().c_str());
#endif
                    SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(expr);
                    ROSE_ASSERT(functionCallExp != NULL);
                    SgExpression* functionExp = functionCallExp->get_function();
                    ROSE_ASSERT(functionExp != NULL);
                    SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(functionExp);
                    ROSE_ASSERT(functionRefExp != NULL);
                    ROSE_ASSERT(functionRefExp->get_symbol() != NULL);
#if 0
                    printf ("function name = %s \n",functionRefExp->get_symbol()->get_name().str());
#endif
                    SgFunctionSymbol* functionSymbol = functionRefExp->get_symbol();
                    ROSE_ASSERT(functionSymbol != NULL);
                    SgFunctionDeclaration* functionDeclaration = functionSymbol->get_declaration();
                    ROSE_ASSERT(functionDeclaration != NULL);
#if 0
                    printf ("functionDeclaration = %p = %s \n",functionDeclaration,functionDeclaration->class_name().c_str());
                    printf ("functionDeclaration->get_name() = %s \n",functionDeclaration->get_name().str());
                    printf ("   --- functionDeclaration->get_args().size() = %zu \n",functionDeclaration->get_args().size());
#endif
                    int numberOfArguments = functionDeclaration->get_args().size();

                 // The number of function parametes is what determins if this is a prefix or postfix operator.
                 // For non-member functions: The prefix operator has 1, and the postfix operator has 2.
                 // For member functions: The prefix operator has 0, and the postfix operator has 1.
                    SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(functionDeclaration);
                    bool isPrefixOperator = false;
                    if (memberFunctionDeclaration != NULL)
                       {
                         if (numberOfArguments == 0)
                            {
                              isPrefixOperator = true;
                            }
                           else
                            {
                              ROSE_ASSERT(numberOfArguments == 1);
                              isPrefixOperator = false;
                            }

                       }
                      else
                       {
                         if (numberOfArguments == 1)
                            {
                              isPrefixOperator = true;
                            }
                           else
                            {
                              ROSE_ASSERT(numberOfArguments == 2);
                              isPrefixOperator = false;
                            }
                       }

                    if (isPrefixOperator == true)
                       {
                         associativitySpecifier = e_assoc_right;
                       }
                      else
                       {
                         associativitySpecifier = e_assoc_left;
                       }

                  }
                 else
                  {
                    ROSE_ASSERT(unaryOp != NULL);

                    if (unaryOp->get_mode() == SgUnaryOp::prefix)
                       {
                         associativitySpecifier = e_assoc_right;
                       }
                      else
                       {
                         ROSE_ASSERT(unaryOp->get_mode() == SgUnaryOp::postfix);
                         associativitySpecifier = e_assoc_left;
                       }
                  }

               return associativitySpecifier;
             }
           
          case V_SgNotOp:
             {
#if 0
               printf ("WARNING: In getAssociativity(): I think that the logical not operator should be right associative! \n");
#endif
            // This has forever been marked as left associative in ROSE.
               return e_assoc_left;
             }

          case V_SgAssignOp:
          case V_SgAndAssignOp:
          case V_SgIorAssignOp:
          case V_SgMultAssignOp:
          case V_SgDivAssignOp:
          case V_SgModAssignOp:
          case V_SgXorAssignOp:
          case V_SgLshiftAssignOp:
          case V_SgRshiftAssignOp:
          case V_SgPlusAssignOp:
          case V_SgMinusAssignOp:
          case V_SgConditionalExp:
          case V_SgBitComplementOp:
          case V_SgPointerDerefExp:
          case V_SgAddressOfOp:
          case V_SgSizeOfOp:
             {
              return e_assoc_left;
             }

          case V_SgCommaOpExp:
          case V_SgOrOp:
          case V_SgAndOp:
          case V_SgBitOrOp:
          case V_SgBitXorOp:
          case V_SgBitAndOp:
          case V_SgEqualityOp:
          case V_SgNotEqualOp:
          case V_SgLessThanOp:
          case V_SgGreaterThanOp:
          case V_SgLessOrEqualOp:
          case V_SgGreaterOrEqualOp:
          case V_SgLshiftOp:
          case V_SgRshiftOp:
          case V_SgAddOp:
          case V_SgSubtractOp:
          case V_SgMultiplyOp:
          case V_SgIntegerDivideOp:
          case V_SgDivideOp:
          case V_SgModOp:
          case V_SgDotStarOp:
          case V_SgArrowStarOp:
          case V_SgFunctionCallExp:
          case V_SgPntrArrRefExp:
          case V_SgArrowExp:
          case V_SgDotExp:
             {
              return e_assoc_right;
             }

       // DQ (9/25/2013): The Fortran SgExponentiationOp has right associativity.
           case V_SgExponentiationOp:
             {
               return e_assoc_right;
             }

       // DQ (9/25/2013): I believe that the Fortran SgConcatenationOp has left associativity.
          case V_SgConcatenationOp:
             {
               return e_assoc_left;
             }

       // DQ (1/25/2014): This is not really defined for unary operators, but it does not make sense to output the warning below either.
          case V_SgMinusOp:
          case V_SgUnaryAddOp:
             {
               return e_assoc_none;
             }
          
          default:
             {
            // We want this to be a printed warning (so we can catch these missing cases), but it is not worthy of calling an error since the default works fine.
            // The implementation of this function assumes unhandled cases are not associative.
               printf ("Warning: getAssociativity(): Undefined expression variant = %d = %s (returning e_assoc_none) \n",variant,Cxx_GrammarTerminalNames[variant].name.c_str());
#if 0
            // DQ (9/25/2013): Temporarily added assertion to get rid of warnings (catching them and fixing them).
               printf ("Error: getAssociativity(): Undefined expression variant = %d = %s (returning e_assoc_none) \n",variant,Cxx_GrammarTerminalNames[variant].name.c_str());
               ROSE_ASSERT(false);
#endif
             }
        }

     return e_assoc_none;
   }

bool
UnparseLanguageIndependentConstructs::requiresParentheses(SgExpression* expr, SgUnparse_Info& info) 
   {
     ROSE_ASSERT(expr != NULL);

#if 0
     if (isSgSubscriptExpression(expr) != NULL || isSgDotExp(expr) || isSgCAFCoExpression(expr) || isSgPntrArrRefExp(expr) )
        {
          return false;
        }
#endif

     SgExpression* parentExpr = isSgExpression(expr->get_parent());

#define DEBUG_PARENTHESIS_PLACEMENT 0

#if DEBUG_PARENTHESIS_PLACEMENT || 0
     printf ("\n\n***** In requiresParentheses() \n");
     printf ("In requiresParentheses(): expr = %p = %s need_paren = %s \n",expr,expr->class_name().c_str(),expr->get_need_paren() ? "true" : "false");
     printf ("In requiresParentheses(): isOverloadedArrowOperator(expr) = %s \n",(unp->u_sage->isOverloadedArrowOperator(expr) == true) ? "true" : "false");
     curprint(string("\n /* In requiresParentheses(): expr = ") + StringUtility::numberToString((void*)expr) + " */ \n ");
     curprint(string("/* In requiresParentheses(): expr = ") + expr->class_name() + " */ \n");
  // curprint( "\n /* RECORD_REF = " << RECORD_REF << " expr->variant() = " << expr->variant() << " */ \n");

     if (parentExpr != NULL)
        {
          printf ("In requiresParentheses(): parentExpr = %s \n",parentExpr->sage_class_name());
          printf ("isOverloadedArrowOperator(parentExpr) = %s \n",(unp->u_sage->isOverloadedArrowOperator(parentExpr) == true) ? "true" : "false");
       // curprint( "\n /* parentExpr = " << parentExpr->sage_class_name() << " */ \n");
        }
       else
        {
          printf ("In PrintStartParen(): parentExpr == NULL \n");
        }
#endif

  // DQ (1/26/2013): Moved to be located after the debugging information.
     if (isSgSubscriptExpression(expr) != NULL || isSgDotExp(expr) || isSgCAFCoExpression(expr) || isSgPntrArrRefExp(expr) )
        {
#if DEBUG_PARENTHESIS_PLACEMENT
          printf ("In requiresParentheses(): Case 1: Output false \n");
          curprint("/* In requiresParentheses(): Case 1: Output false */ \n");
#endif
          return false;
        }

     if ( (isSgBinaryOp(expr) != NULL) && (expr->get_need_paren() == true) )
        {
#if DEBUG_PARENTHESIS_PLACEMENT
          printf ("     Special case of expr->get_need_paren(): (return true) \n");
#endif
          return true;
        }

  // DQ (11/9/2009): I think this can no longer be true since we have removed the use of SgExpressionRoot.
     ROSE_ASSERT(parentExpr == NULL || parentExpr->variantT() != V_SgExpressionRoot);

     if ( parentExpr == NULL || parentExpr->variantT() == V_SgExpressionRoot || expr->variantT() == V_SgExprListExp || expr->variantT() == V_SgConstructorInitializer || expr->variantT() == V_SgDesignatedInitializer)
        {
#if DEBUG_PARENTHESIS_PLACEMENT
          printf ("     Special case of parentExpr == NULL || SgExpressionRoot || SgExprListExp || SgConstructorInitializer || SgDesignatedInitializer (return false) \n");
#endif
          return false;
        }

#if 1
    // Liao, 8/27/2008, bug 229
    // A nasty workaround since set_need_paren() has no definite effect
    // SgExprListExp-> SgAssignInitializer -> SgFunctionCallExp:
    // no () is needed for SgAssignInitializer
    // e.g:  int array[] = {func1()}; // int func1();
     SgAssignInitializer* assign_init = isSgAssignInitializer(expr);
     if ((assign_init != NULL) && (isSgExprListExp(parentExpr)))
        {
          SgExpression* operand = assign_init->get_operand();
          if (isSgFunctionCallExp(operand))
             {
#if DEBUG_PARENTHESIS_PLACEMENT
               printf ("In requiresParentheses(): Case 2: Output false \n");
#endif
               return false;
             }
        }
#endif

  // TV (04/24/11): As compiler generated cast are not unparsed they don't need additional parenthesis.
     if (isSgCastExp(expr) && expr->get_startOfConstruct()->isCompilerGenerated())
        {
#if DEBUG_PARENTHESIS_PLACEMENT
          printf ("In requiresParentheses(): Case 3 (compiler generated SgCastExp): Output false \n");
          curprint("/* In requiresParentheses(): Case 3 (compiler generated SgCastExp): Output false */ \n");
#endif
          return false;
        }

  // DQ (8/6/2005): Never output "()" where the parent is a SgAssignInitializer
     if (parentExpr != NULL && parentExpr->variantT() == V_SgAssignInitializer)
        {
#if DEBUG_PARENTHESIS_PLACEMENT
          printf ("     Special case of parentExpr == SgAssignInitializer (return false) \n");
#endif
          return false;
        }

     switch (expr->variant())
        {
       // DQ (11/18/2007): Don't use parens for these cases
          case TEMP_ColonShapeExp:
          case TEMP_AsteriskShapeExp:

       // DQ (12/2/2004): Original cases
          case VAR_REF:
          case NONREAL_REF:
          case CLASSNAME_REF:
          case FUNCTION_REF:
          case MEMBER_FUNCTION_REF:

       // DQ (4/25/2012): Added template support (avoids output of extra "()" see test2012_51.C).
          case TEMPLATE_FUNCTION_REF:
          case TEMPLATE_MEMBER_FUNCTION_REF:

          case PSEUDO_DESTRUCTOR_REF:
          case BOOL_VAL:
          case SHORT_VAL:
          case CHAR_VAL:
          case UNSIGNED_CHAR_VAL:
          case WCHAR_VAL:
          case STRING_VAL:
          case UNSIGNED_SHORT_VAL:
          case ENUM_VAL:
          case INT_VAL:
          case UNSIGNED_INT_VAL:
          case LONG_INT_VAL:
          case LONG_LONG_INT_VAL:
          case UNSIGNED_LONG_LONG_INT_VAL:
          case UNSIGNED_LONG_INT_VAL:
          case FLOAT_VAL:
          case DOUBLE_VAL:
          case LONG_DOUBLE_VAL:
          case AGGREGATE_INIT:
          case SUPER_NODE: // charles4:  This case needed for Java only.
             {
#if DEBUG_PARENTHESIS_PLACEMENT
               printf ("     case statements return false \n");
#endif
               return false;
             }

          default:
             {
            // DQ (8/29/2014): If this is a user-defined operator (SgFunctionCallExp) nested in a user-defined 
            // operator (SgFunctionCallExp) then we need a more useful parent than the parent function's SgExprListExpr.
               SgExprListExp* parent_exprListExp = isSgExprListExp(parentExpr);
#if 0
               printf ("parent_exprListExp = %p \n",parent_exprListExp);
#endif
               if (parent_exprListExp != NULL)
                  {
                 // DQ (4/19/2018): This might be looking at the wrong node for the SgFunctionCallExp.
#if 0
                    printf ("NOTE: Look at the parent of the SgExprListExp not the expr for the next SgFunctionCallExp \n");
#endif
                 // SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(expr);
                    SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(expr);
#if 0
                    printf ("   --- current expr functionCallExp = %p \n",functionCallExp);
#endif
                    if (functionCallExp != NULL)
                       {
                      // Find a better parent node to use (reach to the parent SgFunctionCallExp).
                         SgNode* local_parentExpr = parentExpr;
                         local_parentExpr = local_parentExpr->get_parent();
                         SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(local_parentExpr);
#if 0
                         printf ("   --- --- functionCallExp = %p \n",functionCallExp);
#endif
                         if (functionCallExp != NULL)
                            {
#if DEBUG_PARENTHESIS_PLACEMENT || 0
                              printf ("In requiresParentheses(): Found a better node to use in determining precedence: functionCallExp = %p \n",functionCallExp);
#endif
                              parentExpr = functionCallExp;
                            }
                       }
                      else
                       {
                      // DQ (4/19/2018): This is the case of both expressions in a binary operator not being overloaded.
#if 0
                         printf ("parent_exprListExp->get_expressions().size() = %zu \n",parent_exprListExp->get_expressions().size());
#endif
                      // Find a better parent node to use (reach to the parent SgFunctionCallExp).
                         SgNode* local_parentExpr = parentExpr;
                         ROSE_ASSERT(local_parentExpr != NULL);
#if 0
                         printf ("local_parentExpr = parentExpr: local_parentExpr = %p \n",local_parentExpr);
                         if (local_parentExpr->get_parent() == NULL)
                            {
                              printf ("local_parentExpr->get_parent() == NULL: local_parentExpr = %p = %s \n",local_parentExpr,local_parentExpr->class_name().c_str());
                            }
#endif
                         local_parentExpr = local_parentExpr->get_parent();
#if 0
                         printf ("local_parentExpr = local_parentExpr->get_parent(): local_parentExpr = %p \n",local_parentExpr);
#endif
                      // ROSE_ASSERT(local_parentExpr != NULL);

                         SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(local_parentExpr);
#if 0
                         printf ("   --- --- parent parent functionCallExp = %p \n",functionCallExp);
#endif
                         SgFunctionRefExp*       functionRefExp       = NULL;
                         SgMemberFunctionRefExp* memberFunctionRefExp = NULL;
                         if (functionCallExp != NULL)
                            {
#if 0
                              printf ("functionRefExp == NULL: local_parentExpr = %p = %s \n",local_parentExpr,local_parentExpr->class_name().c_str());
#endif
                              functionRefExp       = isSgFunctionRefExp(functionCallExp->get_function());
                              memberFunctionRefExp = isSgMemberFunctionRefExp(functionCallExp->get_function());

                            }

                      // ROSE_ASSERT(functionRefExp != NULL);
                         if (memberFunctionRefExp != NULL || functionRefExp != NULL)
                            {
                              SgFunctionSymbol* functionSymbol = NULL;
                              if (functionRefExp != NULL)
                                 {
#if 0
                                   printf ("functionRefExp != NULL: functionCallExp->get_function() = %p = %s \n",functionCallExp->get_function(),functionCallExp->get_function()->class_name().c_str());
#endif
                                   functionSymbol = functionRefExp->get_symbol();
                                 }
                                else
                                 {
                                   ROSE_ASSERT(memberFunctionRefExp != NULL);
#if 0
                                   printf ("memberFunctionRefExp != NULL: functionCallExp->get_function() = %p = %s \n",functionCallExp->get_function(),functionCallExp->get_function()->class_name().c_str());
#endif
                                   functionSymbol = memberFunctionRefExp->get_symbol();
                                 }
                              ROSE_ASSERT(functionSymbol != NULL);
                              SgFunctionDeclaration* functionDeclaration = functionSymbol->get_declaration();
#if 0
                              printf ("functionDeclaration->get_specialFunctionModifier().isOperator() = %s \n",functionDeclaration->get_specialFunctionModifier().isOperator() ? "true" : "false");
#endif
                           // DQ (4/21/2018): We need to avoid puting out too many parenthesis.
                              bool isOperator = functionDeclaration->get_specialFunctionModifier().isOperator();
                              if (isOperator == false)
                                 {
#if 0
                                   printf ("Detected that this was not an operator, so suppresss the parenthesis \n");
#endif
                                   return false;
                                 }
                            }

                      // if (functionCallExp != NULL)
                         if (functionCallExp != NULL)
                            {
#if DEBUG_PARENTHESIS_PLACEMENT || 0
                              printf ("In requiresParentheses(): Found a better node to use in determining precedence: functionCallExp = %p \n",functionCallExp);
#endif
                              parentExpr = functionCallExp;
                            }
                       }
                  }

               int parentVariant   = GetOperatorVariant(parentExpr);
               SgExpression* first = GetFirstOperand(parentExpr);
               if (parentVariant == V_SgPntrArrRefExp && first != expr)
                  {
                 // This case avoids redundent parenthesis within array substripts.
#if DEBUG_PARENTHESIS_PLACEMENT
                    printf ("     parentVariant  == V_SgPntrArrRefExp && first != expr (return false) \n");
#endif
                    return false;
                  }
#if 0
               printf ("Calling getPrecedence(): parentExpr = %p = %s \n",parentExpr,parentExpr->class_name().c_str());
#endif
               PrecedenceSpecifier parentPrecedence = getPrecedence(parentExpr);

#if DEBUG_PARENTHESIS_PLACEMENT
               printf ("parentExpr = %p = %s parentVariant = %d  parentPrecedence = %d \n",parentExpr,parentExpr->class_name().c_str(),parentVariant,parentPrecedence);
#endif

            // DQ (7/22/2013): Don't return true if this is a SgDesignatedInitializer.
               if (parentPrecedence == 0 && isSgDesignatedInitializer(parentExpr) != NULL)
                  {
#if DEBUG_PARENTHESIS_PLACEMENT
                    printf ("     case of SgDesignatedInitializer: parentPrecedence == 0 return true \n");
                    curprint(string("/* case of SgDesignatedInitializer parentPrecedence == 0 return false parentExpr = ") + parentExpr->class_name() + " */ \n");
#endif
                    return false;
                  }

               if (parentPrecedence == 0)
                  {
#if DEBUG_PARENTHESIS_PLACEMENT
                    printf ("     parentPrecedence == 0 return true \n");
                    curprint(string("/* parentPrecedence == 0 return true parentExpr = ") + parentExpr->class_name() + " */ \n");
#endif
                    return true;
                  }

#if 0
               printf ("Calling getPrecedence(): expr = %p = %s \n",expr,expr->class_name().c_str());
#endif
            // int exprVariant = GetOperatorVariant(expr);
               PrecedenceSpecifier exprPrecedence = getPrecedence(expr);

#if DEBUG_PARENTHESIS_PLACEMENT
               int exprVariant = GetOperatorVariant(expr);
               printf ("expr = %p = %s exprVariant = %d  exprPrecedence = %d parentPrecedence = %d \n",expr,expr->class_name().c_str(),exprVariant,exprPrecedence,parentPrecedence);
#endif
               if (exprPrecedence > parentPrecedence)
                  {
#if DEBUG_PARENTHESIS_PLACEMENT
                    printf ("     exprPrecedence > parentPrecedence return false \n");
#endif
                    return false;
                  }
                 else
                  {
                    if (exprPrecedence == parentPrecedence)
                       {
                         if (first == NULL)
                            {
#if DEBUG_PARENTHESIS_PLACEMENT
                              printf ("     exprPrecedence == parentPrecedence return true \n");
#endif
                              return true;
                            }
                         AssociativitySpecifier assoc = getAssociativity(parentExpr);
                         if (assoc == e_assoc_left && first != expr)
                            {
#if DEBUG_PARENTHESIS_PLACEMENT
                              printf ("     assoc > 0 && first != expr return false \n");
#endif
                              return false;
                            }
                         if (assoc == e_assoc_right && first == expr)
                            {
#if DEBUG_PARENTHESIS_PLACEMENT
                              printf ("     assoc < 0 && first == expr return false \n");
#endif
                              return false;
                            }

                      // DQ (7/22/2013): It appears that in many cases this is not handled in the getAssociativity() function.
                         if (assoc == e_assoc_none)
                            {
#if DEBUG_PARENTHESIS_PLACEMENT
                              printf ("WARNING: In requiresParentheses(): assoc == e_assoc_none (not clear what to return, returning true is required to pass regression tests) \n");
#endif
                           // DQ (7/23/2013): This is required to be true for test2012_104.c to pass.
                           // return false;
                              return true;
                            }
                       }
                      else
                       {
#if DEBUG_PARENTHESIS_PLACEMENT
                         printf ("     exprPrecedence != parentPrecedence return true \n");
#endif
                       }
                  }
             }
        }

#if DEBUG_PARENTHESIS_PLACEMENT
     printf ("     base of function return true \n");
#endif

     return true;
   }


