// utility_functions.h -- header file for the ROSE Optimizing Preprocessor

#ifndef UTILITY_FUNCTIONS_H
#define UTILITY_FUNCTIONS_H

#define BACKEND_VERBOSE_LEVEL 2

// returns a string containing the current version number
std::string version();

// Simple interface for ROSE (error codes are in SgProject.frontendErrorCode(), backendErrorCode() )
SgProject* frontend ( int argc, char** argv);
SgProject* frontend ( const std::vector<std::string>& argv);

// This builds a shell of a frontend SgProject with associated SgFile objects (but with empty 
// SgGlobal objects) supporting only commandline processing and requiring the frontend to be 
// called explicitly for each SgFile object.  See tutorial/selectedFileTranslation.C for example.
SgProject* frontendShell ( int argc, char** argv);
SgProject* frontendShell ( const std::vector<std::string>& argv);

// DQ (3/18/2006): Modified backend function interface to permit handling of user specified
// objects to control the formatting of code generation and the use of alternative code generation
// techniques (e.g. copy-based code generation).
// int backend ( SgProject* project );
int backend ( SgProject* project, UnparseFormatHelp *unparseFormatHelp = NULL, UnparseDelegate* unparseDelagate = NULL );

//QY: new back end that performs only source-to-source translations 
// of the original file. Furthermore, statements are copied from 
// the original file if they are not changed
int copy_backend( SgProject* project, UnparseFormatHelp *unparseFormatHelp = NULL );

// int globalBackendErrorCode  = 0;
// int backend ( const SgProject & project, int & errorCode = globalBackendErrorCode);
// SgProject & frontend ( int argc, char * argv[] );

void generatePDF ( const SgProject & project );
void generateDOT ( const SgProject & project );

// DQ (6/14/2007): Support for whole AST graphs output with attributes (types, symbols, all edges, etc.)
// We define a default value for the maximum graph size (number of nodes).
void generateAstGraph ( const SgProject* project, int maxSize = 2000, std::string filenameSuffix = "" );

// output of EDG AST (useful for debugging connection to SAGE)
//void pdfPrintAbstractSyntaxTreeEDG ( SgFile *file );
void generatePDFofEDG ( const SgProject & project );


#if 1
class ROSE
   {
  // This class serves as a catch all location for functions of general use within ROSE
  // we have added variables that are set using command line parameters to avoid the use of
  // global variables.  

     public:

       // DQ (8/10/2004): This was moved to the SgFile a long time ago and should not be used any more)
       // DQ (8/11/2004): Need to put this back so that there is a global concept of verbosity for all of ROSE.
       // static int roseVerboseLevel;

       // These functions trim the header files from the unparsed output.
       // static int isCutStart ( SgStatement *st );
       // static int isCutEnd ( SgStatement *st );
          static void ROSE_Unparse ( SgFile *f , std::ostream *of );

       // This function helps isolate the details of the UNIX strcmp function
          // static int isSameName ( const std::string& s1, const std::string& s2 );
          static int containsString ( const std::string& masterString, const std::string& targetString );

          static std::string getFileNameByTraversalBackToFileNode ( const SgNode* astNode );
          static std::string getFileName ( const SgFile* file );

       // DQ (5/25/2005): Removed from ROSE class (since they are redundant with other uses)
          static std::string getFileName     ( SgLocatedNode* locatedNodePointer ) ROSE_DEPRECATED_FUNCTION;
          static int   getLineNumber   ( SgLocatedNode* locatedNodePointer ) ROSE_DEPRECATED_FUNCTION;
          static int   getColumnNumber ( SgLocatedNode* locatedNodePointer ) ROSE_DEPRECATED_FUNCTION;
          static bool  isPartOfTransformation( SgLocatedNode* locatedNodePointer ) ROSE_DEPRECATED_FUNCTION;
#if 0
          static char* getWorkingDirectory (); //! get the current directory
          static char* getSourceDirectory  ( char* fileNameWithPath ); //! get the sourceDirectory directory
#else
          static std::string getWorkingDirectory (); //! get the current directory
          static std::string getSourceDirectory  ( std::string fileNameWithPath ); //! get the sourceDirectory directory
#endif
          static std::string getFileNameWithoutPath ( SgStatement* statementPointer );
          static std::string stripPathFromFileName ( const std::string& fileNameWithPath ); //! get the filename from the full filename
#if 0
          static char* getPathFromFileName   ( const char* fileNameWithPath ); //! get the path from the full filename
#else
          static std::string getPathFromFileName   ( std::string fileNameWithPath ); //! get the path from the full filename
#endif
          static std::string stripFileSuffixFromFileName ( const std::string& fileNameWithSuffix ); //! get the name without the ".C"

          static std::string getPragmaString ( SgStatement  *stmt );
          static std::string getPragmaString ( SgExpression *expr );

          static SgPragma* getPragma ( SgExpression *expr );
          static SgPragma* getPragma ( SgStatement  *stmt );
          static SgPragma* getPragma ( SgBinaryOp   *binaryOperator );

          static std::string identifyVariant ( int Code );

          static SgName concatenate ( const SgName & X, const SgName & Y );

          static void usage (int status);

          static void filterInputFile ( const std::string inputFileName, const std::string outputFileName );

      //! Functions to move to SgStatement object in SAGE III later
          static SgStatement* getPreviousStatement ( SgStatement *targetStatement );
          static SgStatement* getNextStatement     ( SgStatement *targetStatement );
#if 0
       // Moved to ROSE/src/midend/astDiagnostics/astPerformance.[hC].
       // DQ (12/11/2006): Added simpler function than ramust mechanism, used for computing the memory in use.
      //! Function that I got from Bill Henshaw (originally from PetSc), for computing current memory in use.
       // static double getCurrentMemoryUsage();
          static long int getCurrentMemoryUsage();
#endif

     public:
      //! Should be public to avoid EDG warning
          ROSE();

     private:
       // Define these here to prevent them from being defined by the compiler
       // Except that the EDG front end complains (a warning) about this so we will make the default constructor public
          ROSE ( const ROSE & X );
          ROSE & operator= ( const ROSE & X ); 
   };
#endif

#endif // ifndef UTILITY_FUNCTIONS_H







