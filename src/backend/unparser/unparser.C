/* unparser.C
 * Contains the implementation of the constructors, destructor, formatting functions,
 * and fucntions that unparse directives.
 */

// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
// #include "propagateHiddenListData.h"
// #include "HiddenList.h"

// include "array_class_interface.h"
#include "unparser.h"
#include "keep_going.h"

// DQ (10/21/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
   #include "AsmUnparser_compat.h"
#endif

#include <string.h>
#if _MSC_VER
#include <direct.h>
#include <process.h>
#endif

#include "IncludedFilesUnparser.h"
#include "FileHelper.h"

#include <boost/algorithm/string.hpp>

// DQ (3/19/2014): Used for BOOST_CHECK_EQUAL_COLLECTIONS
// #include <boost/test/unit_test.hpp>

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// extern ROSEAttributesList *getPreprocessorDirectives( char *fileName); // [DT] 3/16/2000

// DQ (6/25/2011): Forward declaration for new name qualification support.
void generateNameQualificationSupport( SgNode* node, std::set<SgNode*> & referencedNameSet );

// DQ (12/6/2014): The call to this function has been moved to the sage_support.cpp file
// so that it can be called on the AST before transformations.  However it is now
// split into two parts so that the token stream can be mapped to the AST before 
// transformations, and then the token stream frontier can be computed after 
// transformations have been done in the AST.
// DQ (10/27/2013): Added forward declaration for new token stream support.
// void buildTokenStreamMapping(SgSourceFile* sourceFile);
void buildTokenStreamFrontier(SgSourceFile* sourceFile);


//-----------------------------------------------------------------------------------
//  Unparser::Unparser
//  
//  Constructor that takes a SgFile*, iostream*, ROSEAttributesList*, Unparser_Opt,
//  and int. All other fields are set to NULL or 0.
//-----------------------------------------------------------------------------------
// [DT] Old version: Unparser::Unparser(SgFile* nfile, iostream* nos, 
//                         ROSEAttributesList* nlist, Unparser_Opt nopt, int nline) {
// Unparser::Unparser(SgFile* nfile, ostream* nos, char *fname, 
//                    ROSEAttributesList* nlist, 
//                    ROSEAttributesListContainer* nlistList, 
//                    Unparser_Opt nopt, int nline)
// Unparser::Unparser(SgFile* nfile, ostream* nos, char *fname,
//                    Unparser_Opt nopt, int nline)
// Unparser::Unparser( ostream* nos, char *fname, Unparser_Opt nopt, int nline,
//                    UnparseFormatHelp *h, UnparseDelegate* r)

// DQ (8/19/2007): I have removed the "int nline" function parameter becuase it was part of an old mechanism
// to unparse a specific line, now replaced by the unparseToString() member function on each IR node.
// Unparser::Unparser( ostream* nos, string fname, Unparser_Opt nopt, int nline, UnparseFormatHelp *h, UnparseDelegate* r)
Unparser::Unparser( ostream* nos, string fname, Unparser_Opt nopt, UnparseFormatHelp *h, UnparseDelegate* r)
   : cur(nos, h), repl(r)
   {
     u_type     = new Unparse_Type(this);
     u_name     = new Unparser_Nameq(this);
  // u_support  = new Unparse_Support(this);
     u_sym      = new Unparse_Sym(this);
     u_debug    = new Unparse_Debug(this);
     u_sage     = new Unparse_MOD_SAGE(this);
     u_exprStmt = new Unparse_ExprStmt(this, fname);

  // DQ (8/14/2007): I have added this here to be consistant, but I question if this is a good design!
  // UnparseFortran_type* u_fortran_type;
  // FortranCodeGeneration_locatedNode* u_fortran_locatedNode;
     u_fortran_type = new UnparseFortran_type(this);
     u_fortran_locatedNode = new FortranCodeGeneration_locatedNode(this, fname);

  // ROSE_ASSERT(nfile != NULL);
     ROSE_ASSERT(nos != NULL);

  // ROSE_ASSERT(nlist != NULL);
  // file       = nfile;
  //   primary_os = nos; // [DT] 3/9/2000
  //
  // [DT] 3/17/2000 -- Should be careful here.  fname could include a path,
  //      and I think currentOutputFileName should be in the local directory,
  //      or a subdirectory of the local directory, perhaps.
  //
  //   strcpy(primaryOutputFileName, fname);
  // strcpy(currentOutputFileName, fname.c_str());


  // dir_list         = nlist;
  // dir_listList     = nlistList;
     opt              = nopt;

  // MK: If overload option is set true, the keyword "operator" occurs in the output.
  // Usually, that's not what you want, but it can be used to avoid a current bug,
  // see file TODO_MK. The default is to set this flag to false, see file
  // preprocessorSupport.C in the src directory
  // opt.set_overload_opt(true);

     cur_index        = 0;

     currentFile      = NULL;

  // DQ (5/8/2010): The default setting for this if "false".
     set_resetSourcePosition(false);

  // DQ (8/19/2007): Removed this old unpasing mechanism.
  // line_to_unparse  = nline;
  // ltu  = nline;
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


Unparser::Unparser(const Unparser & X)
   {
  // DQ (9/11/2011): Added explicit copy constructor to avoid possible double free of formatHelpInfo (reported by static analysis).
  // DQ (9/11/2011): This function is provided to make this code better so that can be analyized using static analysis 
  // (static analysis tools don't understand access functions).

  // Call the operator=() member function.
     *this = X;

     printf ("Error: I think we likely don't want to be using this constructor (UnparseFormat(const UnparseFormat & X)). \n");
     ROSE_ASSERT(false);
   }

Unparser & Unparser::operator=(const Unparser & X)
   {
  // DQ (9/11/2011): Added explicit operator=() to avoid possible double free of formatHelpInfo (reported by static analysis).
  // DQ (9/11/2011): This function is provided to make this code better so that can be analyized using static analysis 
  // (static analysis tools don't understand access functions).

  // DQ (9/12/2011): This avoids the memory leak that could happend with self assignment.
     if (&X == this)
        {
          return *this;
        }
      
     u_type      = NULL; // new Unparse_Type(this);
     u_name      = NULL; // new Unparser_Nameq(this);
     u_sym       = NULL; // new Unparse_Sym(this);
     u_debug     = NULL; // new Unparse_Debug(this);
     u_sage      = NULL; // new Unparse_MOD_SAGE(this);
     u_exprStmt  = NULL; // new Unparse_ExprStmt(this, fname);

     opt         = X.opt;
     cur_index   = 0;
     currentFile = NULL;

     prevdir_was_cppDeclaration = false;

     cur         = X.cur;
     repl        = X.repl;

     embedColorCodesInGeneratedCode = 0;
     generateSourcePositionCodes    = 0;

     p_resetSourcePosition = false;

     printf ("Error: I think we likely don't want to be using this operator (UnparseFormat::operator=(const UnparseFormat & X)). \n");
     ROSE_ASSERT(false);

     return *this;
   }

UnparseFormat& Unparser::get_output_stream()
   {
     return cur;
   }




bool
Unparser::isPartOfTransformation( SgLocatedNode *n)
   {
  // return (n->get_file_info()==0 || n->get_file_info()->get_isPartOfTransformation());

     ROSE_ASSERT(n != NULL);
     ROSE_ASSERT(n->get_file_info() != NULL);

  // DQ (5/10/2005): For now let's check both, but I think we want to favor isTransfrmation() over get_isPartOfTransformation() in the future.
     return (n->get_file_info()->isTransformation() || n->get_file_info()->get_isPartOfTransformation());
   }

bool
Unparser::isCompilerGenerated( SgLocatedNode *n)
   {
     ROSE_ASSERT(n != NULL);
     ROSE_ASSERT(n->get_file_info() != NULL);

  // DQ (5/22/2005): Support for including any compiler generated code (such as instatiated templates).
     return n->get_file_info()->isCompilerGenerated();
   }

bool
Unparser::containsLanguageStatements ( char* fileName )
   {
  // We need to implement this later
     ROSE_ASSERT (fileName != NULL);
  // printf ("Warning: Unparser::containsLanguageStatements(%s) not implemented! \n",fileName);

  // Note that: false will imply that the file contains only preprocessor declarations
  //                  and thus the file need not be unparsed with a different name
  //                  (a local header file).
  //            true  will imply that it needs to be unparsed as a special renamed 
  //                  header file so that transformations in the header files can be
  //                  supported.

#if 1
     return false;
#else
     return true;
#endif
   }

bool
Unparser::includeFileIsSurroundedByExternCBraces ( char* tempFilename )
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

#if 1
     return false;
#else
     return true;
#endif
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
Unparser::isASecondaryFile ( SgStatement* stmt )
   {
  // for now just assume there are no secondary files
     return false;
   }

#if 0
// DQ (10/11/2007): This does not appear to be used any more (member functions in the SgProject 
// call the member functions in the SgFile which call the  unparseFile function 
// directly).
void
Unparser::unparseProject ( SgProject* project, SgUnparse_Info& info )
   {
     ROSE_ASSERT(project != NULL);
     SgFile* file = &(project->get_file(0));
     ROSE_ASSERT(file != NULL);

     unparseFile(file, info);
   }
#endif

// DQ (9/2/2008): Seperate out the details of unparsing source files from binary files.
void
Unparser::unparseFile ( SgSourceFile* file, SgUnparse_Info& info, SgScopeStatement* unparseScope )
   {
  // ROSE_ASSERT(file != NULL);
  // unparseFile (file,info);

     ROSE_ASSERT(file != NULL);

  // Detect reuse of an Unparser object with a different file
     ROSE_ASSERT(currentFile == NULL);

     currentFile = file;
     ROSE_ASSERT(currentFile != NULL);

#if 0
     printf ("In Unparser::unparseFile(): SageInterface::is_Cxx_language()     = %s \n",SageInterface::is_Cxx_language() ? "true" : "false");
     printf ("In Unparser::unparseFile(): SageInterface::is_Fortran_language() = %s \n",SageInterface::is_Fortran_language() ? "true" : "false");
     printf ("In Unparser::unparseFile(): SageInterface::is_Java_language()    = %s \n",SageInterface::is_Java_language() ? "true" : "false");
     printf ("In Unparser::unparseFile(): SageInterface::is_X10_language()    = %s \n",SageInterface::is_X10_language() ? "true" : "false");
     printf ("In Unparser::unparseFile(): file->get_outputLanguage()           = %s \n",file->get_outputLanguage() == SgFile::e_C_output_language ? "C" : 
                                  file->get_outputLanguage() == SgFile::e_Fortran_output_language ? "Fortran" : 
                                  file->get_outputLanguage() == SgFile::e_Java_output_language ? "Java" : "unknown");
#endif
#if 0
     file->display("file: Unparser::unparseFile");
#endif

  // DQ (6/30/2013): Added support to time the unparsing of the file (name qualification will be nested in this time).
     TimingPerformance timer ("Unparse File:");

  // DQ (1/10/2015): Set the current source file.
     info.set_current_source_file(file);

  // DQ (5/15/2011): Moved this to be called in the postProcessingSupport() (before resetTemplateNames() else template names will not be set properly).

  // DQ (11/10/2007): Moved computation of hidden list from astPostProcessing.C to unparseFile so that 
  // it will be called AFTER any transformations and immediately before code generation where it is 
  // really required.  This part of a fix for Liao's outliner, but should be useful for numerous 
  // transformations.  This also make simple analysis much cheaper since the hidel list computation is
  // expensive (in this implementation).
  // DQ (8/6/2007): Only compute the hidden lists if working with C++ code!
     if (SageInterface::is_Cxx_language() == true)
        {
       // DQ (5/22/2007): Moved from SgProject::parse() function to here so that propagateHiddenListData() could be called afterward.
       // DQ (5/8/2007): Now build the hidden lists for types and declarations (Robert Preissl's work)
#if 0
       // DQ (6/25/2011): This will be the default (the old system for name qualification so that initial transition windows will provide backward compatability).
          Hidden_List_Computation::buildHiddenTypeAndDeclarationLists(file);
#else
       // DQ (5/15/2011): Test clearing the mangled name map.
       // printf ("Calling SgNode::clearGlobalMangledNameMap() \n");

       // DQ (6/25/2011): Test if this is required...it works, I think we don't need to clear the global managled name table...
       // SgNode::clearGlobalMangledNameMap();

        // Build the local set to use to record when declaration that might required qualified references have been seen.
          std::set<SgNode*> referencedNameSet;

       // printf ("Developing a new implementation of the name qualification support. \n");
          generateNameQualificationSupport(file,referencedNameSet);
       // printf ("DONE: new name qualification support built. \n*************************\n\n");
#endif

       // DQ (6/5/2007): We actually need this now since the hidden lists are not pushed to lower scopes where they are required.
       // DQ (5/22/2007): Added support for passing hidden list information about types, declarations and elaborated types to child scopes.
          propagateHiddenListData(file);
        }

#if 0
     printf ("In Unparser::unparseFile(): SageInterface::is_C_language() = %s \n",SageInterface::is_C_language() ? "true" : "false");
     printf ("In Unparser::unparseFile(): file->get_unparse_tokens()      = %s \n",file->get_unparse_tokens() ? "true" : "false");
#endif

#if 0
  // DQ (12/6/2014): We need this computed in terms of the original AST before transformations, so we have to move this to after the
  // AST is built, instead of before it is unparsed.  This makes no difference if we don't do transformations, but if transformations
  // are done if this is computed here then statements removed from the AST showup in the white space between statements (an error).

  // DQ (10/27/2013): Adding support for token stream use in unparser. We might want to only turn this of when -rose:unparse_tokens is specified.
  // if (SageInterface::is_C_language() == true)
  // if (SageInterface::is_C_language() == true && file->get_unparse_tokens() == true)
     if ( ( (SageInterface::is_C_language() == true) || (SageInterface::is_Cxx_language() == true) ) && file->get_unparse_tokens() == true)
        {
       // This is only currently being tested and evaluated for C language (should also work for C++, but not yet for Fortran).
#if 0
          printf ("Building token stream mapping map! \n");
#endif
       // This function builds the data base (STL map) for the different subsequences ranges of the token stream.
       // and attaches the toke stream to the SgSourceFile IR node.  
       // *** Next we have to attached the data base ***
          buildTokenStreamMapping(file);

#if 1
          if ( SgProject::get_verbose() > 0 )
             {
               printf ("In Unparser::unparseFile(): SgTokenPtrList token_list: token_list.size() = %" PRIuPTR " \n",file->get_token_list().size());
             }
#endif

#if 0
          printf ("DONE: Building token stream mapping map! \n");
#endif
        }
#endif

  // DQ (12/6/2014): this is the part of the token stream support that is required after transformations have been done in the AST.
     if ( ( (SageInterface::is_C_language() == true) || (SageInterface::is_Cxx_language() == true) ) && file->get_unparse_tokens() == true)
        {
       // This is only currently being tested and evaluated for C language (should also work for C++, but not yet for Fortran).
#if 0
          printf ("Building token stream mapping frontier! \n");
#endif
       // This function builds the data base (STL map) for the different subsequences ranges of the token stream.
       // and attaches the toke stream to the SgSourceFile IR node.  
       // *** Next we have to attached the data base ***
       // buildTokenStreamMapping(file);
          buildTokenStreamFrontier(file);
#if 0
          printf ("DONE: Building token stream mapping frontier! \n");
#endif
        }
     


  // Turn ON the error checking which triggers an if the default SgUnparse_Info constructor is called
     SgUnparse_Info::set_forceDefaultConstructorToTriggerError(true);

     if (file->get_markGeneratedFiles() == true)
        {
       // Output marker to identify code generated by ROSE (causes "#define ROSE_GENERATED_CODE" to be placed at the top of the file).
       // printf ("Output marker to identify code generated by ROSE \n");
          u_exprStmt->markGeneratedFile();
        }

     if ( SgProject::get_verbose() > 0 )
        {
          if (file->get_unparse_tokens() == true)
             {
            // This now unparses the raw token stream as a seperate file with the prefix "rose_tokens_"

            // This is just unparsing the token stream WITHOUT using the mapping information that relates it to the AST.
//MH-20140701 removed comment-out
#if 0
               printf ("In Unparser::unparseFile(): Detected case of file->get_unparse_tokens() == true \n");
#endif
            // Note that this is not yet using the SgTokenPtrList of SgToken IR nodes (this is using a lower level data structure).
               unparseFileUsingTokenStream(file);

            // Now we want to just continue to unparse the file that will be generated from the AST 
            // (and modify that code to selectively use the token stream).
             }
        }

  // SgScopeStatement* globalScope = (SgScopeStatement*) (&(file->root()));
     SgScopeStatement* globalScope = file->get_globalScope();
     ROSE_ASSERT(globalScope != NULL);

  // Make sure that both the C/C++ and Fortran unparsers are present!
     ROSE_ASSERT(u_exprStmt != NULL);
     ROSE_ASSERT(u_fortran_locatedNode != NULL);

     ROSE_ASSERT(file->get_outputLanguage() != SgFile::e_error_output_language);
     ROSE_ASSERT(file->get_outputLanguage() != SgFile::e_Promela_output_language);

  // Use the information in the SgFile object to control which unparser is called.
     if ( ( (file->get_Fortran_only() == true) && (file->get_outputLanguage() == SgFile::e_default_output_language) ) || (file->get_outputLanguage() == SgFile::e_Fortran_output_language) )
        {
       // DQ (6/30/2013): Added support to time the unparsing of the file.
          TimingPerformance timer ("Source code generation from AST (Fortran):");

       // Unparse using the new Fortran unparser!
          u_fortran_locatedNode->unparseStatement(globalScope, info);
        }
       else
        {
       // DQ (6/30/2013): Added support to time the unparsing of the file.
          TimingPerformance timer ("Source code generation from AST:");
#if 0
          printf ("This is not a Fortran file! \n");
#endif
          if ( ( ( (file->get_C_only() == true) || (file->get_Cxx_only() == true) || (file->get_Cuda_only() == true) || (file->get_OpenCL_only() == true) ) && (file->get_outputLanguage() == SgFile::e_default_output_language) ) || 
               ( (file->get_outputLanguage() == SgFile::e_C_output_language) || (file->get_outputLanguage() == SgFile::e_Cxx_output_language) ) )
             {
            // Unparse using C/C++ unparser by default
#if 0
               printf ("Unparse using C/C++ unparser by default: unparseScope = %p \n",unparseScope);
#endif
            // negara1 (06/29/2011): If unparseScope is provided, unparse it. Otherwise, unparse the global scope (the default behavior).
               if (unparseScope != NULL) 
                  {
                    if (isSgGlobal(unparseScope) != NULL || isSgClassDefinition(unparseScope) != NULL)
                       {
                         info.set_current_scope(unparseScope);
                         const SgDeclarationStatementPtrList& declarations = unparseScope -> getDeclarationList();
                         for (SgDeclarationStatementPtrList::const_iterator declaration = declarations.begin(); declaration != declarations.end(); declaration++) 
                            {
                              u_exprStmt -> unparseStatement(*declaration, info);
                            }
                       }
                      else
                       {
                      // Simulate that the unparsed scope is global in order to unparse an included file.
                         SgGlobal* fakeGlobal = new SgGlobal();
                         fakeGlobal -> set_file_info(unparseScope -> get_file_info());
                         info.set_current_scope(fakeGlobal);

                         const SgStatementPtrList& statements = unparseScope -> getStatementList();
                         for (SgStatementPtrList::const_iterator statement = statements.begin(); statement != statements.end(); statement++)
                            {
                              u_exprStmt -> unparseStatement(*statement, info);
                            }
                       }
                    info.set_current_scope(NULL);
                  }
                 else
                  {
                    u_exprStmt->unparseStatement(globalScope, info);
                  }
             }
            else
             {
               if (file->get_PHP_only())
                  {
                    Unparse_PHP unparser(this,file->get_unparse_output_filename());
                    unparser.unparseStatement(globalScope, info);
                  }
                 else
                  {
                    if (file->get_Java_only())
                       {
                      // DQ (8/19/2011): Now that the unparser is working better and we generate a more 
                      // correct AST for Java, we want to use better mechanisms to control the output of 
                      // different parts of the AST (implicit vs. explicit classes in Java).
                      // info.set_outputCompilerGeneratedStatements();

                         Unparse_Java unparser(this, file->getFileName());
                         unparser.unparseJavaFile(file, info);
                       }
                      else
                       {
                           if (file->get_Python_only())
                              {
#ifdef ROSE_BUILD_PYTHON_LANGUAGE_SUPPORT
                                  Unparse_Python unparser(this,file->get_unparse_output_filename());
                                  unparser.unparseGlobalStmt(globalScope, info);
#else
                                  ROSE_ABORT("unparsing Python requires ROSE_USE_PYTHON be set");
#endif
                              }
                          else
                          {
                              if (file->get_X10_only())
                              {
                                   Unparse_X10 unparser(this, file->getFileName());
// MH (7/2/2014) : disabled unparseStatement() and instead invoke unparseX10File()
#if 0
                                   unparser.unparseStatement(globalScope, info);
#else
                                   unparser.unparseX10File(file, info);
#endif
                              }
                              else
                              {
                                 printf ("Error: unclear how to unparse the input code! \n");
                                 ROSE_ASSERT(false);
                              }
                          }
                       }
                  }
             }
        }

  // DQ (7/19/2004): Added newline at end of file
  // (some compilers (e.g. g++) complain if no newline is present)
  // This does not work, not sure why
  // cur << "\n/* EOF: can't insert newline at end of file to avoid g++ compiler warning */ \n\n";

  // DQ: This does not compile
  // cur << std::endl;

  // DQ: This does not force a new line either!
  // cur << "\n\n\n";
     cur.flush();

//MH-20140701 removed comment-out
#if 0
     printf ("Leaving Unparser::unparseFile(): file = %s = %s \n",file->get_sourceFileNameWithPath().c_str(),file->get_sourceFileNameWithoutPath().c_str());
     printf ("Leaving Unparser::unparseFile(): SageInterface::is_Cxx_language()     = %s \n",SageInterface::is_Cxx_language() ? "true" : "false");
#endif

  // Turn OFF the error checking which triggers an if the default SgUnparse_Info constructor is called
     SgUnparse_Info::set_forceDefaultConstructorToTriggerError(false);
   }



// DQ (9/30/2013): Supporting function for evaluating token source position information.
int
Unparser::getNumberOfLines( std::string internalString )
   {
  // This code is copied from the similar support in rose_attributes_list.C.

  // ROSE_ASSERT(this != NULL);

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

  // ROSE_ASSERT(this != NULL);

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

     int endingColumnNumber   = previousLineLength;

#if 0
     printf ("Unparser::getColumnNumberOfEndOfString(): endingColumnNumber = %d \n",endingColumnNumber);
#endif

#if 0
  // If this is a one line comment then the ending position is the length of the comment PLUS the starting column position
     if (getNumberOfLines(internalString) == 1)
        {
       // endingColumnNumber += get_file_info()->get_col() - 1;
          endingColumnNumber += internalString.length() - 1;
        }
#endif

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


  // ROSE_ASSERT(file->get_token_list().empty() == false);
     if (file->get_token_list().empty() == true)
        {
          printf ("Warning: unparseFileUsingTokenStream(): In no tokens found \n");
          return;
        }
#if 0
     ROSEAttributesList* currentListOfAttributes = attributeMapForAllFiles[currentFileNameId];
     ROSE_ASSERT(currentListOfAttributes != NULL);
#endif

     string fileNameForTokenStream = file->getFileName();

#if 1
     printf ("In Unparser::unparseFileUsingTokenStream(): fileNameForTokenStream = %s \n",fileNameForTokenStream.c_str());
#endif

     ROSE_ASSERT(file->get_preprocessorDirectivesAndCommentsList() != NULL);
     ROSEAttributesListContainerPtr filePreprocInfo = file->get_preprocessorDirectivesAndCommentsList();

#if 0
     printf ("filePreprocInfo->getList().size() = %" PRIuPTR " \n",filePreprocInfo->getList().size());
#endif

  // We should at least have the current files CPP/Comment/Token information (even if it is an empty file).
     ROSE_ASSERT(filePreprocInfo->getList().size() > 0);

  // This is an empty list not useful outside of the Flex file to gather the CPP directives, comments, and tokens.
     ROSE_ASSERT(mapFilenameToAttributes.empty() == true);

#if 0
     printf ("Evaluate what files are processed in map (filePreprocInfo->getList().size() = %" PRIuPTR ") \n",filePreprocInfo->getList().size());
     std::map<std::string,ROSEAttributesList* >::iterator map_iterator = filePreprocInfo->getList().begin();
     while (map_iterator != filePreprocInfo->getList().end())
        {
          printf ("   --- map_iterator->first  = %s \n",map_iterator->first.c_str());
          printf ("   --- map_iterator->second = %p \n",map_iterator->second);

          map_iterator++;
        }
     printf ("DONE: Evaluate what files are processed in map (filePreprocInfo->getList().size() = %" PRIuPTR ") \n",filePreprocInfo->getList().size());
#endif

  // std::map<std::string,ROSEAttributesList* >::iterator currentFileItr = mapFilenameToAttributes.find(fileNameForTokenStream);
     std::map<std::string,ROSEAttributesList* >::iterator currentFileItr = filePreprocInfo->getList().find(fileNameForTokenStream);
  // ROSE_ASSERT(currentFileItr != mapFilenameToAttributes.end());
     ROSE_ASSERT(currentFileItr != filePreprocInfo->getList().end());

#if 0
     printf ("Get the ROSEAttributesList from the map iterator \n");
#endif

  // If there already exists a list for the current file then get that list.
     ROSE_ASSERT( currentFileItr->second != NULL);

     ROSEAttributesList* existingListOfAttributes = currentFileItr->second;
#if 0
     printf ("existingListOfAttributes = %p \n",existingListOfAttributes);
#endif
  // LexTokenStreamTypePointer tokenStream = existingListOfAttributes->get_rawTokenStream();
  // ROSE_ASSERT(tokenStream != NULL);

     LexTokenStreamType & tokenList = *(existingListOfAttributes->get_rawTokenStream());

#if 0
     printf ("Output token list (number of CPP directives and comments = %d): \n",existingListOfAttributes->size());
     printf ("Output token list (number of tokens = %" PRIuPTR "): \n",tokenList.size());
#endif

#if 0
     int counter = 0;
     for (LexTokenStreamType::iterator i = tokenList.begin(); i != tokenList.end(); i++)
        {
          printf ("   --- token #%d token = %p \n",counter,(*i)->p_tok_elem);
          if ((*i)->p_tok_elem != NULL)
             {
               printf ("   --- --- token id = %d token = %s \n",(*i)->p_tok_elem->token_id,(*i)->p_tok_elem->token_lexeme.c_str());
             }

       // DQ (9/29/2013): Added support for reference to the PreprocessingInfo object in the token stream.
          printf ("   --- token #%d p_preprocessingInfo = %p \n",counter,(*i)->p_preprocessingInfo);
          printf ("   --- token #%d beginning_fpi line  = %d column = %d \n",counter,(*i)->beginning_fpi.line_num,(*i)->beginning_fpi.column_num);
          printf ("   --- token #%d ending_fpi    line  = %d column = %d \n",counter,(*i)->ending_fpi.line_num,(*i)->ending_fpi.column_num);

          counter++;
        }
#endif

  // Write out the tokens into the output file.
     int current_line_number   = 1;
     int current_column_number = 1;

#if 0
     printf ("Starting: line = %d column = %d \n",current_line_number,current_column_number);
#endif

     int output_token_counter = 0;
     for (LexTokenStreamType::iterator i = tokenList.begin(); i != tokenList.end(); i++)
        {
#if 0
          printf ("TOP OF LOOP: line = %d column = %d \n",current_line_number,current_column_number);
#endif
#if 0
          printf ("   --- token #%d token = %p \n",output_token_counter,(*i)->p_tok_elem);
          if ((*i)->p_tok_elem != NULL)
             {
               printf ("   --- --- token id = %d token = %s \n",(*i)->p_tok_elem->token_id,(*i)->p_tok_elem->token_lexeme.c_str());
             }

       // DQ (9/29/2013): Added support for reference to the PreprocessingInfo object in the token stream.
          printf ("   --- token #%d p_preprocessingInfo = %p \n",output_token_counter,(*i)->p_preprocessingInfo);
          printf ("   --- token #%d beginning_fpi line  = %d column = %d \n",output_token_counter,(*i)->beginning_fpi.line_num,(*i)->beginning_fpi.column_num);
          printf ("   --- token #%d ending_fpi    line  = %d column = %d \n",output_token_counter,(*i)->ending_fpi.line_num,(*i)->ending_fpi.column_num);
#endif
          output_token_counter++;

          std::string s = (*i)->p_tok_elem->token_lexeme;
          int lines = getNumberOfLines(s);
          int line_length = getColumnNumberOfEndOfString(s);
#if 0
          printf ("   --- lines = %d \n",lines);
          printf ("   --- line_length = %d \n",line_length);
          printf ("   --- s = -->|%s|<-- \n",s.c_str());
#endif
       // Check starting position
          if ((*i)->beginning_fpi.line_num != current_line_number)
             {
            // printf ("error: (*i)->beginning_fpi.line_num = %d current_line_number = %d \n",(*i)->beginning_fpi.line_num,current_line_number);
               printf ("error: (*i)->beginning_fpi.line_num = %d \n",(*i)->beginning_fpi.line_num);
               printf ("error: current_line_number          = %d \n",current_line_number);
               ROSE_ASSERT(false);
             }

          if ((*i)->beginning_fpi.column_num != current_column_number)
             {
            // DQ (1/4/2014): This problem is demonstrated by tests/roseTests/astInterfaceTests/inputmoveDeclarationToInnermostScope_test2015_18.C when using the "-rose:verbose 2" option.
               printf ("error: In Unparser::unparseFileUsingTokenStream(): (*i)->beginning_fpi.column_num = %d \n",(*i)->beginning_fpi.column_num);
               printf ("error: In Unparser::unparseFileUsingTokenStream(): current_line_number = %d current_column_number = %d \n",current_line_number,current_column_number);

            // DQ (1/4/2014): Commented this assertion out as part of debugging tests/roseTests/astInterfaceTests/inputmoveDeclarationToInnermostScope_test2015_18.C when using the "-rose:verbose 2" option.
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
               ROSE_ASSERT(false);
             }

       // The position of the end of the last token is one less than the current position for the next token.
          if ((*i)->ending_fpi.column_num != (current_column_number - 1))
             {
               printf ("error: (*i)->ending_fpi.column_num = %d \n",(*i)->ending_fpi.column_num);
               printf ("error: current_line_number = %d current_column_number = %d \n",current_line_number,current_column_number);
               ROSE_ASSERT(false);
             }
#if 0
          printf ("BASE OF LOOP: current_line_number = %d current_column_number = %d \n",current_line_number,current_column_number);
#endif
        }

  // We could output a banner, but this would make the input and output files different.
  // cur << "/* ROSE Generated file from token stream */ \n";


  // DQ (10/27/2013): Use a different filename for the output of the raw token stream (not associated with individual statements).
     string outputFilename = "rose_raw_tokens_" + file->get_sourceFileNameWithoutPath();

#if 1
     printf ("In Unparser::unparseFileUsingTokenStream(): Output tokens stream to file: %s \n",outputFilename.c_str());
#endif

     fstream ROSE_RawTokenStream_OutputFile(outputFilename.c_str(),ios::out);
  // ROSE_OutputFile.open(s_file.c_str());

  // DQ (12/8/2007): Added error checking for opening out output file.
     if (!ROSE_RawTokenStream_OutputFile)
        {
       // throw std::exception("(fstream) error while opening file.");
          printf ("Error detected in opening file %s for output \n",outputFilename.c_str());
          ROSE_ASSERT(false);
        }
#if 0
     ROSE_ASSERT(cur.output_stream() != NULL);
     std::ostream & output_stream = *(cur.output_stream());

  // Write out the tokens into the output file.
     for (LexTokenStreamType::iterator i = tokenList.begin(); i != tokenList.end(); i++)
        {
          output_stream << (*i)->p_tok_elem->token_lexeme;
        }

     output_stream.flush();
#else
  // Use a different filename for the output of the raw token stream (which is a file generated for debugging support).

  // Write out the tokens into the output file.
     for (LexTokenStreamType::iterator i = tokenList.begin(); i != tokenList.end(); i++)
        {
          ROSE_RawTokenStream_OutputFile << (*i)->p_tok_elem->token_lexeme;
        }

     ROSE_RawTokenStream_OutputFile.flush();
#endif
   }





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
Unparser::unparseAsmFile(SgAsmGenericFile *file, SgUnparse_Info &info)
{
     if ( SgProject::get_verbose() > 0 )
          printf ("In Unparser::unparseAsmFile... file = %p = %s \n",file,file->class_name().c_str());

#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
    ROSE_ASSERT(file!=NULL);

    /* Genenerate an ASCII dump of the entire file contents.  Generate the dump before unparsing because unparsing may perform
     * certain relocations and normalization to the AST. */
    // DQ (8/30/2008): This is temporary, we should review how we want to name the files 
    // generated in the unparse phase of processing a binary.
    file->dump_all(true, ".dump");

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
#endif
}

void
Unparser::unparseFile(SgBinaryComposite *binary, SgUnparse_Info &info)
{
     if ( SgProject::get_verbose() > 0 )
          printf ("In Unparser::unparseFile... file = %p = %s \n",binary,binary->class_name().c_str());

#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
    ROSE_ASSERT(binary != NULL);
    ROSE_ASSERT(binary->get_binary_only()) ;

    /* Unparse each file and create an ASCII dump as well */
    const SgAsmGenericFilePtrList &files = binary->get_genericFileList()->get_files();
    ROSE_ASSERT(!files.empty());
    for (size_t i=0; i<files.size(); i++) {
        unparseAsmFile(files[i], info);
    }

    /* Generate an ASCII dump of disassembled instructions for interpretations that we didn't already dump in
     * unparseAsmFile(). In other words, dump interpretations that span multiple files. */
    size_t nwritten=0;
    const SgAsmInterpretationPtrList &interps = binary->get_interpretations()->get_interpretations();
    for (size_t i=0; i<interps.size(); i++) {
        SgAsmGenericFilePtrList interp_files = interps[i]->get_files();
        if (interp_files.size()>1) {
            char interp_name[64];
            sprintf(interp_name, "interp-%03zu.dump", nwritten++);
            FILE *interp_file = fopen(interp_name, "wb");
            ROSE_ASSERT(interp_file!=NULL);
            fprintf(interp_file, "Interpretation spanning these input files:\n");
            for (size_t j=0; j<interp_files.size(); j++) {
                fprintf(interp_file, "  %s\n", interp_files[j]->get_name().c_str());
            }
            fputs(unparseAsmInterpretation(interps[i]).c_str(), interp_file);
            fclose(interp_file);
        }
    }

    /* Generate the rose_*.s (get_unparse_output_filename()) assembly file. It will contain all the interpretations. */
    if (binary->get_unparse_output_filename()!="") {
        FILE *asm_file = fopen(binary->get_unparse_output_filename().c_str(), "wb");
        if (asm_file!=NULL) {
            for (size_t i=0; i<interps.size(); i++) {
                fputs(unparseAsmInterpretation(interps[i]).c_str(), asm_file);
            }
            fclose(asm_file);
        }
    }
#endif
}


string
unparseStatementWithoutBasicBlockToString ( SgStatement* statement )
   {
      string statementString;

      ROSE_ASSERT ( statement != NULL );
   // printf ("unparseStatementWithoutBasicBlockToString(): statement->sage_class_name() = %s \n",statement->sage_class_name());

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

      ROSE_ASSERT ( scope != NULL );
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
               ROSE_ASSERT (false);
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

  // ROSE_ASSERT (this != NULL);
     ROSE_ASSERT ( declaration != NULL );

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
               ROSE_ASSERT (parameterListDeclaration != NULL);
               SgInitializedNamePtrList & argList = parameterListDeclaration->get_args();
               SgInitializedNamePtrList::iterator i;
               for (i = argList.begin(); i != argList.end(); i++)
                  {
//                  printf ("START: Calling unparseToString on type! \n");
                    ROSE_ASSERT ((*i) != NULL);
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
     ROSE_ASSERT(originalFileInfo != NULL);

     if ( SgProject::get_verbose() > 0 )
          printf ("Reset the source code position from %s:%d:%d to ",originalFileInfo->get_filename(),originalFileInfo->get_line(),originalFileInfo->get_col());

  // This is the current output file.
  // string newFilename = "output";
  // Detect reuse of an Unparser object with a different file
     ROSE_ASSERT(currentFile != NULL);
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
               ROSE_ASSERT(false);
             }
            else
             {
               if (file->get_X10_only() == true)
                  {
                    outputFilename = file->get_sourceFileNameWithoutPath();

                    printf ("[Warning] Output file name of generated X10 code is the "
                       "same as the input file name, but must be build into a "
                       "separate directory.\n");
                    ROSE_ASSERT(false);
                  }
             }
        }
       else
        {
          outputFilename = file->get_unparse_output_filename();
        }

  // Set the output file name, since this may be called before unparse().
     file->set_unparse_output_filename(outputFilename);
     ROSE_ASSERT (file->get_unparse_output_filename().empty() == false);

     printf ("Exiting output file name of generated Java code is same as input file name but must be but into a separate directory. \n");
     ROSE_ASSERT(false);

  // Name the file with a separate extension.
     outputFilename += ".resetSourcePosition";

  // printf ("Inside of resetSourcePositionToGeneratedCode(UnparseFormatHelp*) outputFilename = %s \n",outputFilename.c_str());

     if ( SgProject::get_verbose() > 0 )
          printf ("Calling the resetSourcePositionToGeneratedCode: outputFilename = %s \n",outputFilename.c_str());

     fstream ROSE_OutputFile(outputFilename.c_str(),ios::out);

     if (!ROSE_OutputFile)
        {
       // throw std::exception("(fstream) error while opening file.");
          printf ("Error detected in opening file %s for output \n",outputFilename.c_str());
          ROSE_ASSERT(false);
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

     Unparser roseUnparser ( &ROSE_OutputFile, file->get_file_info()->get_filenameString(), roseOptions, unparseHelp, NULL );

  // DQ (12/5/2006): Output information that can be used to colorize properties of generated code (useful for debugging).
     roseUnparser.set_embedColorCodesInGeneratedCode ( file->get_embedColorCodesInGeneratedCode() );
     roseUnparser.set_generateSourcePositionCodes    ( file->get_generateSourcePositionCodes() );

  // This turnes on the mechanism to force resetting the AST's source position information.
     roseUnparser.set_generateSourcePositionCodes(true);

  // information that is passed down through the tree (inherited attribute)
  // SgUnparse_Info inheritedAttributeInfo (NO_UNPARSE_INFO);
     SgUnparse_Info inheritedAttributeInfo;

     SgSourceFile* sourceFile = isSgSourceFile(file);
     ROSE_ASSERT(sourceFile != NULL);

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
string globalUnparseToString_OpenMPSafe ( const SgNode* astNode, const SgTemplateArgumentPtrList* templateArgumentList, const SgTemplateParameterPtrList* templateParameterList, SgUnparse_Info* inputUnparseInfoPointer );

string
globalUnparseToString ( const SgNode* astNode, SgUnparse_Info* inputUnparseInfoPointer )
   {
     string returnString;

// tps (Jun 24 2008) added because OpenMP crashes all the time at the unparser
#if ROSE_GCC_OMP
#pragma omp critical (unparser)
#endif
        {
          if (inputUnparseInfoPointer != NULL)
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
          returnString = globalUnparseToString_OpenMPSafe(astNode,NULL,NULL,inputUnparseInfoPointer);
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
          if (inputUnparseInfoPointer != NULL)
             {
            // DQ (1/13/2014): These should have been setup to be the same.
               ROSE_ASSERT(inputUnparseInfoPointer->SkipClassDefinition() == inputUnparseInfoPointer->SkipEnumDefinition());
             }

       // DQ (9/13/2014): Call internal funtion modified to be more general.
       // returnString = globalUnparseToString_OpenMPSafe(astNode,inputUnparseInfoPointer);
          returnString = globalUnparseToString_OpenMPSafe(NULL,templateArgumentList,NULL,inputUnparseInfoPointer);
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
          if (inputUnparseInfoPointer != NULL)
             {
            // DQ (1/13/2014): These should have been setup to be the same.
               ROSE_ASSERT(inputUnparseInfoPointer->SkipClassDefinition() == inputUnparseInfoPointer->SkipEnumDefinition());
             }

       // DQ (9/13/2014): Call internal funtion modified to be more general.
       // returnString = globalUnparseToString_OpenMPSafe(astNode,inputUnparseInfoPointer);
          returnString = globalUnparseToString_OpenMPSafe(NULL,NULL,templateParameterList,inputUnparseInfoPointer);
        }

     return returnString;
   }

// DQ (9/13/2014): Modified to extend the API of this internal function.
// string globalUnparseToString_OpenMPSafe ( const SgNode* astNode, SgUnparse_Info* inputUnparseInfoPointer )
string
globalUnparseToString_OpenMPSafe ( const SgNode* astNode, const SgTemplateArgumentPtrList* templateArgumentList, const SgTemplateParameterPtrList* templateParameterList, SgUnparse_Info* inputUnparseInfoPointer )
   {
  // This global function permits any SgNode (including it's subtree) to be turned into a string

  // DQ (9/13/2014): Modified the API to be more general (as part of refactoring support for name qualification).
  // DQ (3/2/2006): Let's make sure we have a valid IR node!
  // ROSE_ASSERT(astNode != NULL);
     ROSE_ASSERT(astNode != NULL || templateArgumentList != NULL || templateParameterList != NULL);

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

  // printf ("In globalUnparseToString(): astNode->sage_class_name() = %s \n",astNode->sage_class_name());

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
     if (locatedNode == NULL)
        {
       // printf ("WARNING: applying AST -> string for non expression/statement AST objects \n");
          fileNameOfStatementsToUnparse = "defaultFileNameInGlobalUnparseToString";
        }
       else
        {
          ROSE_ASSERT (locatedNode != NULL);

       // DQ (5/31/2005): Get the filename from a traversal back through the parents to the SgFile
       // fileNameOfStatementsToUnparse = locatedNode->getFileName();
       // fileNameOfStatementsToUnparse = ROSE::getFileNameByTraversalBackToFileNode(locatedNode);
          if (locatedNode->get_parent() == NULL)
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
               if (arrayType != NULL)
                  {
                 // If this is an index of a SgArrayType node then handle as a special case
                    fileNameOfStatementsToUnparse = "defaultFileNameInGlobalUnparseToString";
                  }
                 else
                  {
#if 1
                    fileNameOfStatementsToUnparse = ROSE::getFileNameByTraversalBackToFileNode(locatedNode);
#else
                    SgSourceFile* sourceFile = TransformationSupport::getSourceFile(locatedNode);
                    ROSE_ASSERT(sourceFile != NULL);
                    fileNameOfStatementsToUnparse = sourceFile->getFileName();
#endif
                  }
             }
        }  // end if locatedNode

     ROSE_ASSERT (fileNameOfStatementsToUnparse.size() > 0);

  // Unparser roseUnparser ( &outputString, fileNameOfStatementsToUnparse, roseOptions, lineNumber );
     Unparser roseUnparser ( &outputString, fileNameOfStatementsToUnparse, roseOptions );

  // Information that is passed down through the tree (inherited attribute)
  // Use the input SgUnparse_Info object if it is available.
     SgUnparse_Info* inheritedAttributeInfoPointer = NULL;

  // DQ (2/18/2013): Keep track of local allocation of the SgUnparse_Info object in this function
  // This is design to fix what appears to be a leak in ROSE (abby-normal growth of the SgUnparse_Info
  // memory pool for compiling large files.
     bool allocatedSgUnparseInfoObjectLocally = false;

     if (inputUnparseInfoPointer != NULL)
        {
       // printf ("Using the input inputUnparseInfoPointer object \n");

       // Use the user provided SgUnparse_Info object
          inheritedAttributeInfoPointer = inputUnparseInfoPointer;
        }
       else
        {
       // DEFINE DEFAULT BEHAVIOUR FOR THE CASE WHEN NO inputUnparseInfoPointer (== NULL) IS 
       // PASSED AS ARGUMENT TO THE FUNCTION
       // printf ("Building a new Unparse_Info object \n");

       // If no input parameter has been specified then allocate one
       // inheritedAttributeInfoPointer = new SgUnparse_Info (NO_UNPARSE_INFO);
          inheritedAttributeInfoPointer = new SgUnparse_Info();
          ROSE_ASSERT (inheritedAttributeInfoPointer != NULL);

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
          if ( (isSgProject(astNode) != NULL || isSgFile(astNode) != NULL ) == false )
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
       // ROSE_ASSERT(sourceFile != NULL);
          if (sourceFile == NULL)
             {
#if 0
            // DQ (1/12/2015): This message it commented out, it is frequently triggered for expression IR nodes (SgNullExpression, SgIntVal, SgTemplateParameterVal, SgAddOp, etc.).
               printf ("NOTE: in globalUnparseToString(): TransformationSupport::getSourceFile(astNode = %p = %s) == NULL \n",astNode,astNode->class_name().c_str());
#endif
             }

          inheritedAttributeInfoPointer->set_current_source_file(sourceFile);
        }

     ROSE_ASSERT (inheritedAttributeInfoPointer != NULL);
     SgUnparse_Info & inheritedAttributeInfo = *inheritedAttributeInfoPointer;

  // DQ (5/27/2007): Commented out, uncomment when we are ready for Robert's new hidden list mechanism.
  // if (inheritedAttributeInfo.get_current_scope() == NULL)
     if (astNode != NULL && inheritedAttributeInfo.get_current_scope() == NULL)
        {
       // printf ("In globalUnparseToString(): inheritedAttributeInfo.get_current_scope() == NULL astNode = %p = %s \n",astNode,astNode->class_name().c_str());

       // DQ (6/2/2007): Find the nearest containing scope so that we can fill in the current_scope, so that the name qualification can work.
#if 1
          SgStatement* stmt = TransformationSupport::getStatement(astNode);
#else
          SgStatement* stmt = NULL;
       // DQ (6/27/2007): SgProject and SgFile are not contained in any statement
          if (isSgProject(astNode) == NULL && isSgFile(astNode) == NULL)
               stmt = TransformationSupport::getStatement(astNode);
#endif

          if (stmt != NULL)
             {
               SgScopeStatement* scope = stmt->get_scope();
               ROSE_ASSERT(scope != NULL);
               inheritedAttributeInfo.set_current_scope(scope);
             }
            else
             {
            // DQ (6/27/2007): If we unparse a type then we can't find the enclosing statement, so 
            // assume it is SgGlobal. But how do we find a SgGlobal IR node to use?  So we have to 
            // leave it NULL and hand this case downstream!
               inheritedAttributeInfo.set_current_scope(NULL);
             }

          const SgTemplateArgument* templateArgument = isSgTemplateArgument(astNode);
          if (templateArgument != NULL)
             {
            // debugging code!
            // printf ("Exiting to debug case of SgTemplateArgument \n");
            // ROSE_ASSERT(false);

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
  // ROSE_ASSERT(info.get_current_scope() != NULL);

  // Turn ON the error checking which triggers an error if the default SgUnparse_Info constructor is called
  // SgUnparse_Info::forceDefaultConstructorToTriggerError = true;

  // DQ (10/19/2004): Cleaned up this code, remove this dead code after we are sure that this worked properly
  // Actually, this code is required to be this way, since after this branch the current function returns and
  // some data must be cleaned up differently!  So put this back and leave it this way, and remove the
  // "Implementation Note".

#if 0
     printf ("In globalUnparseToString(): astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif

#if 0
     printf ("In globalUnparseToString_OpenMPSafe(): inheritedAttributeInfo.SkipClassDefinition() = %s \n",(inheritedAttributeInfo.SkipClassDefinition() == true) ? "true" : "false");
     printf ("In globalUnparseToString_OpenMPSafe(): inheritedAttributeInfo.SkipEnumDefinition()  = %s \n",(inheritedAttributeInfo.SkipEnumDefinition()  == true) ? "true" : "false");
#endif
  // DQ (1/13/2014): These should have been setup to be the same.
     ROSE_ASSERT(inheritedAttributeInfo.SkipClassDefinition() == inheritedAttributeInfo.SkipEnumDefinition());

  // Both SgProject and SgFile are handled via recursive calls
     if ( (isSgProject(astNode) != NULL) || (isSgSourceFile(astNode) != NULL) )
        {
       // printf ("Implementation Note: Put these cases (unparsing the SgProject and SgFile into the cases for nodes derived from SgSupport below! \n");

       // Handle recursive call for SgProject
          const SgProject* project = isSgProject(astNode);
          if (project != NULL)
             {
               for (int i = 0; i < project->numberOfFiles(); i++)
                  {
                 // SgFile* file = &(project->get_file(i));
                    SgFile* file = project->get_fileList()[i];
                    ROSE_ASSERT(file != NULL);
                    string unparsedFileString = globalUnparseToString_OpenMPSafe(file,NULL,NULL,inputUnparseInfoPointer);
                 // string prefixString       = string("/* TOP:")      + string(ROSE::getFileName(file)) + string(" */ \n");
                 // string suffixString       = string("\n/* BOTTOM:") + string(ROSE::getFileName(file)) + string(" */ \n\n");
                    string prefixString       = string("/* TOP:")      + file->getFileName() + string(" */ \n");
                    string suffixString       = string("\n/* BOTTOM:") + file->getFileName() + string(" */ \n\n");
                    returnString += prefixString + unparsedFileString + suffixString;
                  }
             }

       // Handle recursive call for SgFile
          const SgSourceFile* file = isSgSourceFile(astNode);
          if (file != NULL)
             {
               SgGlobal* globalScope = file->get_globalScope();
               ROSE_ASSERT(globalScope != NULL);
               returnString = globalUnparseToString_OpenMPSafe(globalScope,NULL,NULL,inputUnparseInfoPointer);
             }
        }
       else
        {
       // DQ (1/12/2003): Only now try to trap use of SgUnparse_Info default constructor
       // Turn ON the error checking which triggers an error if the default SgUnparse_Info constructor is called
       // GB (09/27/2007): Took this out because it breaks parallel traversals that call unparseToString. It doesn't
       // seem to have any other effect (whatever was debugged with this seems to be fixed now).
       // SgUnparse_Info::set_forceDefaultConstructorToTriggerError(true);

          if (isSgStatement(astNode) != NULL)
             {
               const SgStatement* stmt = isSgStatement(astNode);

            // DQ (9/6/2010): Added support to detect use of C (default) or Fortran code.
            // DQ (2/2/2007): Note that we should modify the unparser to take the IR nodes as const pointers, but this is a bigger job than I want to do now!
            // roseUnparser.u_exprStmt->unparseStatement ( const_cast<SgStatement*>(stmt), inheritedAttributeInfo );
               if (SageInterface::is_Fortran_language() == true)
                  {
                 // Unparse as a Fortran code.
                    ROSE_ASSERT(roseUnparser.u_fortran_locatedNode != NULL);
                    roseUnparser.u_fortran_locatedNode->unparseStatement ( const_cast<SgStatement*>(stmt), inheritedAttributeInfo );
                  }
                 else
                  {
                 // Unparse as a C/C++ code.

                    ROSE_ASSERT(roseUnparser.u_exprStmt != NULL);

                 // printf ("Calling roseUnparser.u_exprStmt->unparseStatement() stmt = %s \n",stmt->class_name().c_str());
                 // roseUnparser.u_exprStmt->curprint ("Output from curprint");
                    roseUnparser.u_exprStmt->unparseStatement ( const_cast<SgStatement*>(stmt), inheritedAttributeInfo );
                  }
             }

          if (isSgExpression(astNode) != NULL)
             {
               const SgExpression* expr = isSgExpression(astNode);

            // DQ (9/6/2010): Added support to detect use of C (default) or Fortran code.
            // DQ (2/2/2007): Note that we should modify the unparser to take the IR nodes as const pointers, but this is a bigger job than I want to do now!
            // roseUnparser.u_exprStmt->unparseExpression ( const_cast<SgExpression*>(expr), inheritedAttributeInfo );
               if (SageInterface::is_Fortran_language() == true)
                  {
                 // Unparse as a Fortran code.
                    ROSE_ASSERT(roseUnparser.u_fortran_locatedNode != NULL);
                    roseUnparser.u_fortran_locatedNode->unparseExpression ( const_cast<SgExpression*>(expr), inheritedAttributeInfo );
                  }
                 else
                  {
                 // Unparse as a C/C++ code.
                    ROSE_ASSERT(roseUnparser.u_exprStmt != NULL);
                    roseUnparser.u_exprStmt->unparseExpression ( const_cast<SgExpression*>(expr), inheritedAttributeInfo );
                  }
             }

          if (isSgType(astNode) != NULL)
             {
               const SgType* type = isSgType(astNode);

#if 0
               printf ("In globalUnparseToString_OpenMPSafe(): inheritedAttributeInfo.SkipClassDefinition() = %s \n",(inheritedAttributeInfo.SkipClassDefinition() == true) ? "true" : "false");
               printf ("In globalUnparseToString_OpenMPSafe(): inheritedAttributeInfo.SkipEnumDefinition()  = %s \n",(inheritedAttributeInfo.SkipEnumDefinition()  == true) ? "true" : "false");
#endif
            // DQ (1/13/2014): These should have been setup to be the same.
               ROSE_ASSERT(inheritedAttributeInfo.SkipClassDefinition() == inheritedAttributeInfo.SkipEnumDefinition());

            // DQ (9/6/2010): Added support to detect use of C (default) or Fortran code.
            // DQ (2/2/2007): Note that we should modify the unparser to take the IR nodes as const pointers, but this is a bigger job than I want to do now!
#if 1
               ROSE_ASSERT(roseUnparser.u_type != NULL);
               roseUnparser.u_type->unparseType ( const_cast<SgType*>(type), inheritedAttributeInfo );
#else
               if (SageInterface::is_Fortran_language() == true)
                  {
                 // Unparse as a Fortran code.
                    roseUnparser.u_fortran_locatedNode->unparseType ( const_cast<SgType*>(type), inheritedAttributeInfo );
                  }
                 else
                  {
                 // Unparse as a C/C++ code.
                    roseUnparser.u_type->unparseType ( const_cast<SgType*>(type), inheritedAttributeInfo );
                  }
#endif
             }

          if (isSgSymbol(astNode) != NULL)
             {
               const SgSymbol* symbol = isSgSymbol(astNode);

            // DQ (2/2/2007): Note that we should modify the unparser to take the IR nodes as const pointers, but this is a bigger job than I want to do now!
               ROSE_ASSERT(roseUnparser.u_sym != NULL);
               roseUnparser.u_sym->unparseSymbol ( const_cast<SgSymbol*>(symbol), inheritedAttributeInfo );
             }

          if (isSgSupport(astNode) != NULL)
             {
            // Handle different specific cases derived from SgSupport 
            // (e.g. template parameters and template arguments).
               switch (astNode->variantT())
                  {
#if 0
                    case V_SgProject:
                       {
                         SgProject* project = isSgProject(astNode);
                         ROSE_ASSERT(project != NULL);
                         for (int i = 0; i < project->numberOfFiles(); i++)
                            {
                              SgFile* file = &(project->get_file(i));
                              ROSE_ASSERT(file != NULL);
                              string unparsedFileString = globalUnparseToString_OpenMPSafe(file,inputUnparseInfoPointer);
                              string prefixString       = string("/* TOP:")      + string(ROSE::getFileName(file)) + string(" */ \n");
                              string suffixString       = string("\n/* BOTTOM:") + string(ROSE::getFileName(file)) + string(" */ \n\n");
                              returnString += prefixString + unparsedFileString + suffixString;
                            }
                         break;
                       }
#error "DEAD CODE!"
                 // case V_SgFile:
                       {
                         SgFile* file = isSgFile(astNode);
                         ROSE_ASSERT(file != NULL);
                         SgGlobal* globalScope = file->get_globalScope();
                         ROSE_ASSERT(globalScope != NULL);
                         returnString = globalUnparseToString_OpenMPSafe(globalScope,inputUnparseInfoPointer);
                         break;
                       }
#endif
                    case V_SgTemplateParameter:
                       {
                         const SgTemplateParameter* templateParameter = isSgTemplateParameter(astNode);

                      // DQ (2/2/2007): Note that we should modify the unparser to take the IR nodes as const pointers, but this is a bigger job than I want to do now!
                         ROSE_ASSERT(roseUnparser.u_exprStmt != NULL);
                         roseUnparser.u_exprStmt->unparseTemplateParameter(const_cast<SgTemplateParameter*>(templateParameter),inheritedAttributeInfo);
                         break;
                       }

                    case V_SgTemplateArgument:
                       {
                         const SgTemplateArgument* templateArgument = isSgTemplateArgument(astNode);
#if 0
                      // printf ("In globalUnparseToString_OpenMPSafe(): case V_SgTemplateArgument (before): returnString = %s outputString = %s \n",returnString.c_str(),outputString.str());
                         printf ("In globalUnparseToString_OpenMPSafe(): case V_SgTemplateArgument (before): returnString = %s \n",returnString.c_str());
#endif
                      // DQ (2/2/2007): Note that we should modify the unparser to take the IR nodes as const pointers, but this is a bigger job than I want to do now!
                         ROSE_ASSERT(roseUnparser.u_exprStmt != NULL);
                         roseUnparser.u_exprStmt->unparseTemplateArgument(const_cast<SgTemplateArgument*>(templateArgument),inheritedAttributeInfo);
#if 0
                      // printf ("In globalUnparseToString_OpenMPSafe(): case V_SgTemplateArgument (after): returnString = %s outputString = %s \n",returnString.c_str(),outputString.str());
                         printf ("In globalUnparseToString_OpenMPSafe(): case V_SgTemplateArgument (after): returnString = %s \n",returnString.c_str());
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
                         ROSE_ASSERT(roseUnparser.u_exprStmt != NULL);
                         roseUnparser.u_exprStmt->unparseStatement ( decl, inheritedAttributeInfo );
                         break;
                       }

                    case V_SgFileList:
                       {
                      // DQ (1/23/2010): Not sure how or if we should implement this
                         const SgFileList* fileList = isSgFileList(astNode);
                         ROSE_ASSERT(fileList != NULL);
#if 0
                         for (int i = 0; i < project->numberOfFiles(); i++)
                            {
                              SgFile* file = &(project->get_file(i));
                              ROSE_ASSERT(file != NULL);
                              string unparsedFileString = globalUnparseToString_OpenMPSafe(file,inputUnparseInfoPointer);
                              string prefixString       = string("/* TOP:")      + string(ROSE::getFileName(file)) + string(" */ \n");
                              string suffixString       = string("\n/* BOTTOM:") + string(ROSE::getFileName(file)) + string(" */ \n\n");
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
                         ROSE_ABORT();
                       }
                  }
             }

          if (astNode == NULL)
             {
            // DQ (9/13/2014): This is where we could put support for when astNode == NULL, and the input was an STL list of IR node pointers.
               if (templateArgumentList != NULL)
                  {
#if 0
                    printf ("Detected SgTemplateArgumentPtrList: templateArgumentList = %p size = %zu \n",templateArgumentList,templateArgumentList->size());
#endif
                    roseUnparser.u_exprStmt->unparseTemplateArgumentList(*templateArgumentList, inheritedAttributeInfo );
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }

               if (templateParameterList != NULL)
                  {
#if 0
                    printf ("Detected SgTemplateParameterPtrList: templateParameterList = %p size = %zu \n",templateParameterList,templateParameterList->size());
#endif
                    roseUnparser.u_exprStmt->unparseTemplateParameterList(*templateParameterList, inheritedAttributeInfo );
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
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
               if (isSgGlobal(scope) == NULL && scope->containsOnlyDeclarations() == true)
                  {
                    roseUnparser.get_output_stream() << roseUnparser.u_exprStmt->trimGlobalScopeQualifier ( scope->get_qualified_name().getString() ) << "::";
                  }
               roseUnparser.get_output_stream() << initializedName->get_name().str();
            // break;
             }


       // Liao, 8/28/2009, support for SgLocatedNodeSupport
          if (isSgLocatedNodeSupport(astNode) !=  NULL) 
             {
               if (isSgOmpClause(astNode))
                  {
                    SgOmpClause * omp_clause = const_cast<SgOmpClause*>(isSgOmpClause(astNode));
                    ROSE_ASSERT(omp_clause);

                    ROSE_ASSERT(roseUnparser.u_exprStmt != NULL);
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
     if (inputUnparseInfoPointer == NULL)
        {
          delete inheritedAttributeInfoPointer;
          inheritedAttributeInfoPointer = NULL;
        }

#if 0
     printf ("In globalUnparseToString_OpenMPSafe(): returnString = %s \n",returnString.c_str());
#endif

  // DQ (2/18/2013): Keep track of local allocation of the SgUnparse_Info object in this function
     if (allocatedSgUnparseInfoObjectLocally == true)
        {
          ROSE_ASSERT(inheritedAttributeInfoPointer == NULL);
        }

     return returnString;
   }

string get_output_filename( SgFile& file)
   {
  // DQ (10/15/2005): This can now be made to be a simpler function!
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
  // DQ (1/24/2010): Refactored code to cal this more directly (part of support for SgDirectory).
  // DQ (7/12/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("AST Code Generation (unparsing):");

  // Call the unparser mechanism

#if 0
     printf ("Inside of unparseFile ( SgFile* file ) (using filename = %s) \n",file->get_unparse_output_filename().c_str());
#endif

#if 0
     printf ("In unparseFile(SgFile* file): file->get_outputLanguage() = %d \n",file->get_outputLanguage());
     printf ("In unparseFile(SgFile* file): file->get_outputLanguage() = %s \n",SgFile::get_outputLanguageOptionName(file->get_outputLanguage()).c_str());
#endif

  // debugging assertions
  // ROSE_ASSERT ( file.get_verbose() == true );
  // ROSE_ASSERT ( file.get_skip_unparse() == false );
  // file.set_verbose(true);

     ROSE_ASSERT(file != NULL);

  // FMZ (12/21/2009) the imported files by "use" statements should not be unparsed 
     if (file->get_skip_unparse() == true)
        {
       // We need to be careful about this premature return.
          return;
        }

#if 0
  // DQ (5/31/2006): It is a message that I think we can ignore (was a problem for Yarden)
  // DQ (4/21/2006): This would prevent the file from being unparsed twice,
  // but then I am not so sure we want to support that.
     if (file->get_unparse_output_filename().empty() == false)
        {
          printf ("Warning, the unparse_output_filename should be set by the unparser or the backend compilation if not set by the unparser ... \n");
        }
#endif
  // Not that this fails in the AST File I/O tests and since the file in unparsed a second time
  // ROSE_ASSERT (file->get_unparse_output_filename().empty() == true);

  // DQ (4/22/2006): This can be true when the "-E" option is used, but then we should not have called unparse()!
     ROSE_ASSERT(file->get_skip_unparse() == false);

  // If we did unparse an intermediate file then we want to compile that file instead of the original source file.
     if (file->get_unparse_output_filename().empty() == true)
        {
          string outputFilename = "rose_" + file->get_sourceFileNameWithoutPath();

       // DQ (9/15/2013): Added support for generated file to be placed into the same directory as the source file.
          SgProject* project = TransformationSupport::getProject(file);
       // ROSE_ASSERT(project != NULL);
          if (project != NULL)
             {
#if 0
               printf ("project->get_unparse_in_same_directory_as_input_file() = %s \n",project->get_unparse_in_same_directory_as_input_file() ? "true" : "false");
#endif
               if (project->get_unparse_in_same_directory_as_input_file() == true)
                  {
                    outputFilename = ROSE::getPathFromFileName(file->get_sourceFileNameWithPath()) + "/rose_" + file->get_sourceFileNameWithoutPath();
#if 0
                    printf ("Using filename for unparsed file into same directory as input file: outputFilename = %s \n",outputFilename.c_str());
#endif
#if 0
                    printf("Exiting as test! \n");
                    ROSE_ASSERT(false);
#endif
                  }
             }
            else
             {
               printf ("WARNING: In unparseFile(): file = %p has no associated project \n",file);
             }

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
                           ROSE_ASSERT(packageDef != NULL);
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
        else if (file -> get_Java_only() == true) {
            // We try to get the package information back to output the translated source file
            // in the correct folder structure.
            SgSourceFile *sourcefile = isSgSourceFile(file);
            ROSE_ASSERT(sourcefile && "Try to unparse an SgFile not being an SgSourceFile using the java unparser");

            SgProject *project = sourcefile -> get_project();
            ROSE_ASSERT(project != NULL);

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
#if 0
            printf ("In unparseFile(): generated Java outputFilename = %s \n",outputFilename.c_str());
#endif
        }
        // Liao 12/29/2010, generate cuda source files
        else if (file->get_Cuda_only() == true)
        {
            outputFilename = StringUtility::stripFileSuffixFromFileName (outputFilename);
            outputFilename += ".cu";
        }
        else if (file->get_X10_only())
        {
            // X10 is Java source code; see Java file/class naming conventions.
            // Filenames are based on the Java Class name contained in the file.
            SgSourceFile *sourcefile = isSgSourceFile(file);
            ROSE_ASSERT(sourcefile && "Try to unparse an SgFile not being an SgSourceFile using the x10 unparser");

            SgProject *project = sourcefile -> get_project();
            ROSE_ASSERT(project != NULL);

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
            string mkdirCommand = string("mkdir -p ") + outFolder;
            int status = system (mkdirCommand.c_str());
            ROSE_ASSERT(status == 0);
            outputFilename = outFolder + file -> get_sourceFileNameWithoutPath();
        }
        else
        {
            //ROSE_ASSERT (! "Not implemented, or unknown file type");
        }

       // DQ (9/15/2013): Added assertion.
          ROSE_ASSERT (file->get_unparse_output_filename().empty() == true);

        // TOO1 (3/20/2014): Clobber the original input source file X_X
        //
        //            **CAUTION**RED*ALERT**CAUTION**
        //
        SgSourceFile* source_file = isSgSourceFile(file);
        if (source_file != NULL)
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

        file->set_unparse_output_filename(outputFilename);
        ROSE_ASSERT (file->get_unparse_output_filename().empty() == false);
     }

#if 0
     printf ("Inside of unparseFile ( SgFile* file ) file->get_skip_unparse() = %s \n",file->get_skip_unparse() ? "true" : "false");
#endif

     if (file->get_skip_unparse() == true)
        {
       // MS: commented out the following output
       // if ( file.get_verbose() == true )
            // printf ("### ROSE::skip_unparse == true: Skipping all source code generation by ROSE generated preprocessor! \n");
        }
       else
        {
       // Open the file where we will put the generated code
          string outputFilename = get_output_filename(*file);

          if ( SgProject::get_verbose() > 0 )
               printf ("Calling the unparser: outputFilename = %s \n",outputFilename.c_str());

       // printf ("In unparseFile(SgFile*): open file for output of generated source code: outputFilename = %s \n",outputFilename.c_str());

       // DQ (3/19/2014): Added support for noclobber option.
          boost::filesystem::path output_file = outputFilename;

          bool trigger_file_comparision = false;

          string saved_filename       = outputFilename;
          string alternative_filename = outputFilename + ".noclobber_compare";

          if (boost::filesystem::exists(output_file))
             {
               if ( SgProject::get_verbose() > 0 )
                    printf ("In unparseFile(SgFile*): (outputFilename) output file exists = %s \n",output_file.string().c_str());

               SgProject* project = TransformationSupport::getProject(file);
               ROSE_ASSERT(project != NULL);

               if (project->get_noclobber_output_file() == true)
                  {
                 // If the file exists then it is an error if -rose:noclobber_output_file option was specified.
                    printf ("\n\n****************************************************************************************************** \n");
                    printf ("Error: the output file already exists, cannot overwrite (-rose:noclobber_output_file option specified) \n");
                    printf ("   --- outputFilename = %s \n",outputFilename.c_str());
                    printf ("****************************************************************************************************** \n\n\n");
                 // ROSE_ASSERT(false);
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

          fstream ROSE_OutputFile(outputFilename.c_str(),ios::out);
       // ROSE_OutputFile.open(s_file.c_str());

       // DQ (12/8/2007): Added error checking for opening out output file.
          if (!ROSE_OutputFile)
             {
            // throw std::exception("(fstream) error while opening file.");
               printf ("Error detected in opening file %s for output \n",outputFilename.c_str());
               ROSE_ASSERT(false);
             }

       // file.set_unparse_includes(false);
       // ROSE_ASSERT (file.get_unparse_includes() == false);

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
       // bool linefile                      = false;
          bool generateLineDirectives        = file->get_unparse_line_directives();

       // DQ (6/19/2007): note that test2004_24.C will fail if this is false.
       // If false, this will cause A.operator+(B) to be unparsed as "A+B". This is a confusing point!
          bool useOverloadedOperators        = false;
       // bool useOverloadedOperators        = true;

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

       // printf ("ROSE::getFileName(file) = %s \n",ROSE::getFileName(file));
       // printf ("file->get_file_info()->get_filenameString = %s \n",file->get_file_info()->get_filenameString().c_str());

       // DQ (7/19/2007): Remove lineNumber from constructor parameter list.
       // int lineNumber = 0;  // Zero indicates that ALL lines should be unparsed
       // Unparser roseUnparser ( &file, &ROSE_OutputFile, ROSE::getFileName(&file), roseOptions, lineNumber );
       // Unparser roseUnparser ( &ROSE_OutputFile, ROSE::getFileName(&file), roseOptions, lineNumber, NULL, repl );
       // Unparser roseUnparser ( &ROSE_OutputFile, ROSE::getFileName(file), roseOptions, lineNumber, unparseHelp, unparseDelegate );
       // Unparser roseUnparser ( &ROSE_OutputFile, file->get_file_info()->get_filenameString(), roseOptions, lineNumber, unparseHelp, unparseDelegate );

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

       // Call member function to start the unparsing process
       // roseUnparser.run_unparser();
       // roseUnparser.unparseFile(file,inheritedAttributeInfo);

       // DQ (9/2/2008): This one way to handle the variations in type
          switch (file->variantT())
             {
               case V_SgSourceFile:
                  {
                    SgSourceFile* sourceFile = isSgSourceFile(file);
                    roseUnparser.unparseFile(sourceFile,inheritedAttributeInfo, unparseScope);
                    break;
                  }

               case V_SgBinaryComposite:
                  {
                    SgBinaryComposite* binary = isSgBinaryComposite(file);
                    roseUnparser.unparseFile(binary,inheritedAttributeInfo);
                    break;
                  }

               case V_SgUnknownFile:
                  {
                    SgUnknownFile* unknownFile = isSgUnknownFile(file);

                    unknownFile->set_skipfinalCompileStep(true);

                    printf ("Warning: Unclear what to unparse from a SgUnknownFile (set skipfinalCompileStep) \n");
                    break;
                  }

               default:
                  {
                    printf ("Error: default reached in unparser: file = %s \n",file->class_name().c_str());
                    ROSE_ASSERT(false);
                  }
             }          

       // And finally we need to close the file (to flush everything out!)
          ROSE_OutputFile.close();

       // Invoke post-output user-defined callbacks if any.  We must pass the absolute output name because the build system may
       // have changed directories by now and the callback might need to know how this name compares to the top of the build
       // tree.
          if (unparseHelp != NULL) {
              rose::FileSystem::Path fullOutputName = rose::FileSystem::makeAbsolute(outputFilename);
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


void prependIncludeOptionsToCommandLine(SgProject* project, const list<string>& includeCompilerOptions) {
    SgStringList argumentList = project -> get_originalCommandLineArgumentList();
    //Note: Insert -I options starting from the second argument, because the first argument is the name of the executable.
    argumentList.insert(++argumentList.begin(), includeCompilerOptions.begin(), includeCompilerOptions.end());
    project -> set_originalCommandLineArgumentList(argumentList);
    const SgFilePtrList& fileList = project -> get_fileList();
    for (SgFilePtrList::const_iterator sgFilePtr = fileList.begin(); sgFilePtr != fileList.end(); sgFilePtr++) {
        argumentList = (*sgFilePtr) -> get_originalCommandLineArgumentList();
        argumentList.insert(++argumentList.begin(), includeCompilerOptions.begin(), includeCompilerOptions.end());
        (*sgFilePtr) -> set_originalCommandLineArgumentList(argumentList);
    }
}


void unparseIncludedFiles ( SgProject* project, UnparseFormatHelp *unparseFormatHelp, UnparseDelegate* unparseDelegate) { 
    ROSE_ASSERT(project != NULL);
    //Proceed only if there are input files and they require header files unparsing.
    if (!project -> get_fileList().empty() && (*(project -> get_fileList()).begin()) -> get_unparseHeaderFiles()) {
        if (SgProject::get_verbose() >= 1) {
            cout << endl << "***HEADER FILES UNPARSING***" << endl << endl;
        }
        IncludedFilesUnparser includedFilesUnparser(project);
        includedFilesUnparser.unparse();
        const string& unparseRootPath = includedFilesUnparser.getUnparseRootPath();
        const map<string, string>& unparseMap = includedFilesUnparser.getUnparseMap();
        const map<string, SgScopeStatement*>& unparseScopesMap = includedFilesUnparser.getUnparseScopesMap();

        prependIncludeOptionsToCommandLine(project, includedFilesUnparser.getIncludeCompilerOptions());

        for (map<string, string>::const_iterator unparseMapEntry = unparseMap.begin(); unparseMapEntry != unparseMap.end(); unparseMapEntry++) {
            SgSourceFile* unparsedFile = new SgSourceFile();
            unparsedFile -> set_Cxx_only(true); //TODO: Generalize this hard coded trick.
            const string& originalFileName = unparseMapEntry -> first;
            if (!includedFilesUnparser.isInputFile(originalFileName)) { //Unparse here only files that would not be unparsed otherwise.
                if (SgProject::get_verbose() >= 1) {
                    cout << "Unparsing included file:" << originalFileName << endl;
                }
                map<string, SgScopeStatement*>::const_iterator unparseScopesMapEntry = unparseScopesMap.find(originalFileName);
                ROSE_ASSERT(unparseScopesMapEntry != unparseScopesMap.end());

                unparsedFile -> set_sourceFileNameWithoutPath(FileHelper::getFileName(originalFileName));
                unparsedFile -> set_sourceFileNameWithPath(originalFileName);
                const string& outputFileName = FileHelper::concatenatePaths(unparseRootPath, unparseMapEntry -> second);
                FileHelper::ensureParentFolderExists(outputFileName);
                unparsedFile -> set_unparse_output_filename(outputFileName);

                Sg_File_Info* unparsedFileInfo = new Sg_File_Info(originalFileName, 0,0);
                unparsedFile -> set_file_info(unparsedFileInfo);

                //Set SgGlobal to avoid problems with checks during unparsing.
                SgGlobal* fakeGlobal = new SgGlobal();
                fakeGlobal -> set_file_info(unparsedFileInfo);                 
                unparsedFile -> set_globalScope(fakeGlobal);

                unparseFile(unparsedFile, unparseFormatHelp, unparseDelegate, unparseScopesMapEntry -> second);
            }
        }
    }    
}


// DQ (10/11/2007): I think this is redundant with the Unparser::unparseProject() member function
// But it is allowed to call it directly from the user's translator if compilation using the backend 
// is not required!  So we have to allow it to be here.
void unparseProject ( SgProject* project, UnparseFormatHelp *unparseFormatHelp, UnparseDelegate* unparseDelegate)
   {
     ROSE_ASSERT(project != NULL);

     // negara1 (06/27/2011) 
     unparseIncludedFiles(project, unparseFormatHelp, unparseDelegate);

#if ROSE_USING_OLD_PROJECT_FILE_LIST_SUPPORT
#error "This implementation of the support for the older interface has been refactored"
     for (int i=0; i < project->numberOfFiles(); ++i)
        {
          SgFile & file = project->get_file(i);
          unparseFile(&file,unparseFormatHelp,unparseDelegate);
        }
#else
     if ( SgProject::get_verbose() > 0 )
          printf ("Unparse the file list first, then the directory list \n");

  // DQ (1/23/2010): refactored the SgFileList
     unparseFileList(project->get_fileList_ptr(),unparseFormatHelp,unparseDelegate);

     if ( SgProject::get_verbose() > 0 )
          printf ("Unparse the directory list... \n");

     for (int i = 0; i < project->numberOfDirectories(); ++i)
        {
          if ( SgProject::get_verbose() > 0 )
               printf ("Unparse each directory (i = %d) \n",i);

          ROSE_ASSERT(project->get_directoryList() != NULL);
          SgDirectory* directory = project->get_directoryList()->get_listOfDirectories()[i];
          unparseDirectory(directory,unparseFormatHelp,unparseDelegate);
        }
#endif
   }


// DQ (1/19/2010): Added support for handling directories of files.
void unparseDirectory ( SgDirectory* directory, UnparseFormatHelp* unparseFormatHelp, UnparseDelegate *unparseDelegate )
   {
     int status = 0;

     ROSE_ASSERT(directory != NULL);

#if 0
  // Check the current directory
     printf ("Current directory BEFORE building/moving to subdirectory defined by SgDirectory IR node. \n");
     status = system ("pwd");
     ROSE_ASSERT(status == 0);
#endif

  // Part of the unparcing support for directories is to change the current system directory.
     string directoryName = directory->get_name();
     ROSE_ASSERT(directoryName != "");

     string mkdirCommand = string("mkdir -p ") + directoryName;

  // DQ (1/24/2010): This is a potential security problem!
     if ( SgProject::get_verbose() > 0 )
          printf ("WARNING: calling system using mkdirCommand = %s \n",mkdirCommand.c_str());

     status = system (mkdirCommand.c_str());
     ROSE_ASSERT(status == 0);

  // Now change the current working directory to the new directory
     status = chdir(directoryName.c_str());
     ROSE_ASSERT(status == 0);

#if 0
  // Check the current directory
     printf ("Current directory AFTER building/moving to subdirectory defined by SgDirectory IR node. \n");
     status = system ("pwd");
     ROSE_ASSERT(status == 0);
#endif

  // DQ (1/23/2010): refactored the SgFileList
     unparseFileList(directory->get_fileList(),unparseFormatHelp,unparseDelegate);

  // printf ("Unparse the directory list... \n");
     for (int i = 0; i < directory->numberOfDirectories(); ++i)
        {
       // printf ("Unparse each directory (i = %d) \n",i);
          ROSE_ASSERT(directory->get_directoryList() != NULL);
          SgDirectory* subdirectory = directory->get_directoryList()->get_listOfDirectories()[i];
          unparseDirectory(subdirectory,unparseFormatHelp,unparseDelegate);
        }

  // DQ (1/24/2010): This is a potential security problem!
     string chdirCommand = "..";

     if ( SgProject::get_verbose() > 0 )
          printf ("WARNING: calling system using chdirCommand = %s \n",chdirCommand.c_str());

  // Now change the current working directory to the new directory
     status = chdir(chdirCommand.c_str());
     ROSE_ASSERT(status == 0);

#if 0
  // Check the current directory
     printf ("Current directory AFTER cd .. (supporting SgDirectory IR node). \n");
     status = system ("pwd");
     ROSE_ASSERT(status == 0);
#endif
   }

// DQ (1/19/2010): Added support for refactored handling directories of files.
void unparseFileList ( SgFileList* fileList, UnparseFormatHelp *unparseFormatHelp, UnparseDelegate* unparseDelegate)
{
  ROSE_ASSERT(fileList != NULL);

  int status_of_function = 0;

  for (size_t i=0; i < fileList->get_listOfFiles().size(); ++i)
  {
      SgFile* file = fileList->get_listOfFiles()[i];
      {
          ROSE_ASSERT(file != NULL);

          if (SgProject::get_verbose() > 1)
          {
               printf("Unparsing file = %p = %s \n",
                      file,
                      file->class_name().c_str());
          }

      #ifndef _MSC_VER
          if (KEEP_GOING_CAUGHT_BACKEND_UNPARSER_SIGNAL)
          {
              std::cout
                  << "[WARN] "
                  << "Configured to keep going after catching a "
                  << "signal in Unparser::unparseFile()"
                  << std::endl;

              if (file != NULL)
              {
                  file->set_unparserErrorCode(100);
                  status_of_function =
                      max(100, status_of_function);
              }
              else
              {
                  std::cout
                      << "[FATAL] "
                      << "Unable to keep going due to an unrecoverable internal error"
                      << std::endl;
                  exit(1);
              }
          }
      #else
      if (false) {}
      #endif
          else if (!isSgSourceFile(file) || isSgSourceFile(file) -> get_frontendErrorCode() == 0)
          {
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
      }//file
  }//for each
}
