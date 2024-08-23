/* unparser.h
 * This header file contains the class declaration for the newest unparser. Six
 * C files include this header file: unparser.C, modified_sage.C, unparse_stmt.C,
 * unparse_expr.C, unparse_type.C, and unparse_sym.C.
 */

#ifndef UNPARSER_H
#define UNPARSER_H


#include "unparser_opt.h"
#include "unparse_format.h"

// #include <algorithm>
#include "unparseCxx_types.h"
#include "name_qualification_support.h"
// #include "unparseCxx_templates.h"
#include "unparse_sym.h"
#include "unparse_debug.h"
#include "modified_sage.h"
#include "unparseCxx.h"

#include "unparseFortran.h"
#include "unparseFortran_types.h"
#include "unparseJava.h"
#include "unparsePHP.h"
#include "unparsePython.h"
#include "unparseAda.h"
#include "unparseJovial.h"

#include "UnparserDelegate.h"
#include "UnparserFortran.h"

// DQ (7/20/2008): New mechanism to permit unparsing of arbitrary strings at IR nodes.
// This is intended to suppport non standard backend compiler annotations.
#include "astUnparseAttribute.h"
#include "includeFileSupport.h"

class Unparser_Nameq;

// Macro used for debugging.  If true it fixes the anonymous typedef and anonymous declaration
// bugs, but causes several other problems.  If false, everything works except the anonymous
// typedef and anonymous declaration bugs.
#define ANONYMOUS_TYPEDEF_FIX false

// Whether to use Rice's code to wrap long lines in Fortran.
#define USE_RICE_FORTRAN_WRAPPING  0  // 1 if you're Rice, 0 if you want to get through Jenkins

// Maximum line lengths for Fortran fixed source form and free source form, per the F90 specification.
#if USE_RICE_FORTRAN_WRAPPING
  #define MAX_F90_LINE_LEN_FIXED  72
  #define MAX_F90_LINE_LEN_FREE  132
#else
  #define MAX_F90_LINE_LEN      132
#endif

#define KAI_NONSTD_IOSTREAM 1

#if USE_OLD_MECHANISM_OF_HANDLING_PREPROCESSING_INFO
// I think this is part of the connection to lex support for comments
// extern ROSEAttributesList* getPreprocessorDirectives( char *fileName);
#endif

 /* \if documentDevelopmentVersionUsingDoxygen
          \ingroup backendGenerator
          \brief This function is used by the SgNode object to connect the unparser to the AST.

          This function hides the complexity of generating a string from any subtree
          of the AST (represented by a SgNode*).

          \internal This function uses the standard stringstream mechanism in C++ to
                    convert the stream output to a string.
     \endif
  */

std::string get_output_filename( SgFile& file);
//! returns the name of type t
std::string get_type_name( SgType* t);

//! Unparse the declaration as a string for use in prototypes within the AST rewrite mechanism prefix mechanism
ROSE_DLL_API std::string unparseDeclarationToString ( SgDeclarationStatement* declaration, bool unparseAsDeclaration = true );

//! Unparse the header of the scope statement (used in the prefix generation to
//! permit context to be accounted for the in generation of AST fragements from
//! strings, e.g. for cases in SgSwitchStatement).
std::string unparseScopeStatementWithoutBasicBlockToString ( SgScopeStatement* scope );

//! Unparse header of statements that have bodies (but are not scopes) (e.g. SgDefaultOptionStmt)
std::string unparseStatementWithoutBasicBlockToString      ( SgStatement* statement );

// DQ (3/14/2021): Output include saved in the SgIncludeFile about first and last computed statements in each header file.
// void outputFirstAndLastIncludeFileInfo();
void outputFirstAndLastIncludeFileInfo( SgSourceFile* sourceFile );

// This is the class representing all the code generation support for C and C++.  It is separated from
// being a part of the AST IR so that it can be more easily developed as a separate modular piece of ROSE.
class Unparser
   {
     public:
          Unparse_Type* u_type;
          Unparser_Nameq* u_name;
          Unparse_Sym* u_sym;
          Unparse_Debug* u_debug;
          Unparse_MOD_SAGE* u_sage;
          Unparse_ExprStmt* u_exprStmt;

       // DQ (8/14/2007): I have added this here to be consistant, but I question if this is a good design!
          UnparseFortran_type* u_fortran_type;
          FortranCodeGeneration_locatedNode* u_fortran_locatedNode;

     public:

      //! Used to support unparsing of doubles and long double as x.0 instead of just x if they are whole number values.
       // bool zeroRemainder( long double doubleValue );

      //! holds all desired options for this unparser
          Unparser_Opt opt;

      //! used to index the preprocessor list
          int cur_index;

      //! The previous directive was a CPP statment (otherwise it was a comment)
          bool prevdir_was_cppDeclaration;

       // DQ (8/19/2007): Added simple access to the SgFile so that options specified there are easily available.
       // Using this data member a number of mechanism in the unparser could be simplified to be more efficient
       // (they currently search bacj through the AST to get the SgFile).
          SgFile* currentFile;

      //! This is a cursor mechanism which is not encapsulated into the curprint() member function.
     public:
          UnparseFormat cur;

     public:
      //! delegate unparser that can be used to replace the output of this unparser
          UnparseDelegate* delegate;

      // DQ (10/23/2006): Moved to be private after Thomas noticed this was incorrectly marked
      // public in this program vizualization.
      //! compiler generated code statements are pushed into a temporary queue so that they can
      //! be output after any statements attached to the next statements and before the next statement
          std::list<SgStatement*> compilerGeneratedStatementQueue;

      private:
       // DQ (12/5/2006): Output information that can be used to colorize properties of generated code (useful for debugging).
          int embedColorCodesInGeneratedCode;

       // DQ (12/5/2006): Output separate file containing source position information for highlighting (useful for debugging).
          int generateSourcePositionCodes;

       // DQ (5/8/2010): Added support to force unparser to reset the source positon in the AST (this is the only side-effect in unparsing).
          bool p_resetSourcePosition;

     public:

      //! constructor
          Unparser(std::ostream* localStream, std::string filename, Unparser_Opt info,
                   UnparseFormatHelp*h = nullptr, UnparseDelegate* delegate = nullptr);

      //! destructor
          virtual ~Unparser();

          Unparser(const Unparser &) = delete;
          Unparser & operator=(const Unparser &) = delete;

      //! get the output stream wrapper
          UnparseFormat& get_output_stream();

      //! true if SgLocatedNode is part of a transformation on the AST
          bool isPartOfTransformation( SgLocatedNode *n);

      //! true if SgLocatedNode is part of a compiler generated part of the AST (e.g template instatiation)
          bool isCompilerGenerated( SgLocatedNode *n);

      //! counts the number of lines in one directive
          int line_count(char*);

      //! Used to decide which include files (most often header files) will be unparsed
          bool containsLanguageStatements (char* fileName);

      //! special case of extern "C" { \n\#include "foo.h" }
          bool includeFileIsSurroundedByExternCBraces ( char* tempFilename );

      //! incomplete-documentation
          bool isASecondaryFile ( SgStatement* stmt );

      //! friend string globalUnparseToString ( SgNode* astNode );

          void unparseFile (SgSourceFile* file, SgUnparse_Info &info, SgScopeStatement* unparseScope=nullptr);
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
          void unparseFile ( SgBinaryComposite*, SgUnparse_Info& info );
          void unparseFile ( SgJvmComposite*, SgUnparse_Info& info );

       // Unparses a single physical file
          void unparseAsmFile ( SgAsmGenericFile* asmFile, SgUnparse_Info & info );
#endif

      //! remove unneccessary white space to build a condensed string
          static std::string removeUnwantedWhiteSpace ( const std::string & X );

       // DQ (12/5/2006): Output separate file containing source position information for highlighting (useful for debugging).
          int get_embedColorCodesInGeneratedCode();
          int get_generateSourcePositionCodes();
          void set_embedColorCodesInGeneratedCode( int x );
          void set_generateSourcePositionCodes( int x );

       // DQ (5/8/2010): Added support to force unparser to reset the source positon in the AST (this is the only side-effect in unparsing).
          void set_resetSourcePosition(bool x);
          bool get_resetSourcePosition();

       // DQ (5/8/2010): Added support to force unparser to reset the source positon in the AST (this is the only side-effect in unparsing).
      //! Reset the Sg_File_Info to reference the unparsed (generated) source code.
          void resetSourcePosition (SgStatement* stmt);

       // DQ (9/30/2013): Unparse the file using the token stream (stored in the SgFile).
          void unparseFileUsingTokenStream ( SgSourceFile* file );

       // DQ (9/30/2013): Supporting function for evaluating token source position information.
          static int getNumberOfLines( std::string s );
          static int getColumnNumberOfEndOfString( std::string s );

       // DQ (8/7/2018): Refactored code for name qualification (so that we can call it once before all files
       // are unparsed (where we unparse multiple files because fo the use of header file unparsing)).
          static void computeNameQualification ( SgSourceFile* file );
   };


// DQ (5/8/2010): Refactored code to generate the Unparser object.
void resetSourcePositionToGeneratedCode(SgFile* file, UnparseFormatHelp* unparseHelp);

//! User callable function available if compilation using the backend compiler is not required.
ROSE_DLL_API void unparseFile(SgFile* file, UnparseFormatHelp* unparseHelp = nullptr,
                              UnparseDelegate *delegate = nullptr, SgScopeStatement* unparseScope = nullptr);

//! User callable function available if compilation using the backend compiler is not required.
ROSE_DLL_API void unparseIncludedFiles(SgProject* project, UnparseFormatHelp* unparseHelp = nullptr,
                                       UnparseDelegate* delegate = nullptr);

//! User callable function available if compilation using the backend compiler is not required.
ROSE_DLL_API void unparseProject(SgProject* project, UnparseFormatHelp* unparseHelp = nullptr,
                                 UnparseDelegate *delegate = nullptr);

//! Support for handling directories of files in ROSE (useful for code generation).
void unparseDirectory(SgDirectory* directory, UnparseFormatHelp* unparseHelp = nullptr,
                      UnparseDelegate* delegate = nullptr);

// DQ (1/19/2010): Added support for refactored handling directories of files.
//! Support for refactored handling directories of files.
void unparseFileList(SgFileList* fileList, UnparseFormatHelp* unparseFormatHelp = nullptr,
                     UnparseDelegate* unparseDelegate = nullptr);

// DQ (10/1/2019): Adding support to generate SgSourceFile for individual header files on demand.
// This is required for the optimization of the header files because in this optimization all the
// header files will not be processed at one time in the operation to attach the CPP and comments to the AST.
// Instead we defer the transformations on the header files and make a note of what header files will be
// transformed, and then prepare the individual header files that we will transform by collecint CPP
// directives and comments and weaving them into those subsequences of the AST and then perform the
// defered transforamtion, and then unparse the header files.  This is a moderately complex operation.
SgSourceFile* buildSourceFileForHeaderFile(SgProject* project, std::string originalFileName);

#endif
