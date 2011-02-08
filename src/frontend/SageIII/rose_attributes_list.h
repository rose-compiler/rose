#ifndef __ROSEAttributesList_H__
#define __ROSEAttributesList_H__

//#include "setup.h"

//#include <list>
//#include <vector>
#include <map>

// Include the ROSE lex specific definitions of tokens
#include "general_token_defs.h"

// #ifdef CAN_NOT_COMPILE_WITH_ROSE
//    #warning "CAN_NOT_COMPILE_WITH_ROSE IS defined"
// #else
//   #warning "CAN_NOT_COMPILE_WITH_ROSE is NOT defined"
// #endif

// DQ (2/28/2010): Skip this if we are compiling ROSE using ROSE.
// This is being used in place of the CAN_NOT_COMPILE_WITH_ROSE macro.
// Note that CAN_NOT_COMPILE_WITH_ROSE is set by the following projects:
//     1) projects/DocumentationGenerator
//     2) projects/haskellport
// in their Makefile.am files.  I think that using CXX_IS_ROSE_ANALYSIS
// will be equivalent (used to indicate the a ROSE translator is being 
// used to compile the ROSE source code).
// However, it might be that this is equivalent to the USE_ROSE macro, 
// which is set for all ROSE translators when they compile any code.
// DQ (12/22/2008): I would appreciate it if this were a better name...
// #if !CAN_NOT_COMPILE_WITH_ROSE 
// #ifndef USE_ROSE

// DQ (5/21/2010): I have built a separate macro for tuning off the compilation of WAVE
// since it is done only for the purpose of allowing ROSE based projects:
//     1) projects/DocumentationGenerator
//     2) projects/haskellport
// to process the ROSE files cleanly.  ROSE can however process and compile ROSE
// (including a slightly modified version of WAVE that EDG will accept) and we
// separately test this in noightly tests.  The previous 5/18/2010 fix to permit
// the Haskell support to skip processign WAVE turned this off and broke the nightly
// tests of ROSE compiling ROSE.  This use of the macro ROSE_SKIP_COMPILATION_OF_WAVE
// it menat to be turned on by ROSE based tools that need to process the ROSE code
// and which currently fail (because of Wave) and so turn of the processing of Wave
// for those tools.
// #ifdef USE_ROSE
//  #define ROSE_SKIP_COMPILATION_OF_WAVE
// #endif

#ifndef ROSE_SKIP_COMPILATION_OF_WAVE
  #if _MSC_VER < 1600  // 1600 == VC++ 10.0
    #include <boost/preprocessor/iteration/iterate.hpp> // Liao, 7/10/2009, required by GCC 4.4.0 for a #define line of BOOST_PP_ITERATION_DEPTH
    #ifdef _MSC_VER
      #include <boost/wave.hpp> // CH (4/7/2010): Put this header here to avoid compiling error about mismatch between defination and declaration
    #endif
    #include <boost/wave/cpplexer/cpp_lex_token.hpp>    // token class
    #include <boost/wave/cpplexer/cpp_lex_iterator.hpp>   // lexer type
  #else
// #warning "Setting CAN_NOT_COMPILE_WITH_ROSE to value = 1"
// #define CAN_NOT_COMPILE_WITH_ROSE 1
// tps (12/4/2009) : This is not found in VC++ 10.0 and Boost 1.4
    #pragma message ("Boost preprocessor and wave not included yet for VC++ 10.0")

  #endif
#endif

//template boost::wave::cpplexer::impl::token_data<std::string, boost::wave::util::file_position_type>::delete(std::size_t) ; 
// DQ (10/16/2002): Required for compiling with SUN 5.2 C++ compiler
#ifndef NAMESPACE_IS_BROKEN
// DQ (12/30/2005): This is a Bad Bad thing to do (I can explain)
// it hides names in the global namespace and causes errors in 
// otherwise valid and useful code. Where it is needed it should
// appear only in *.C files (and only ones not included for template 
// instantiation reasons) else they effect user who use ROSE unexpectedly.
// using namespace std;
#endif

class PreprocessingInfo;
class ROSEAttributesList;
//AS(01/04/07) Global map of filenames to PreprocessingInfo*'s as it is inefficient
//to get this by a traversal of the AST
extern std::map<std::string,ROSEAttributesList* > mapFilenameToAttributes;


// DQ (4/19/2006): Forward declaration so that PreprocessingInfo can
// contain a pointer to a Sg_File_Info object.
class Sg_File_Info;

// DQ (1/21/2008): Need forward declaration
class SgFile;

// #if !CAN_NOT_COMPILE_WITH_ROSE 
// #ifndef USE_ROSE
#ifndef ROSE_SKIP_COMPILATION_OF_WAVE

typedef boost::wave::cpplexer::lex_token<>  token_type;
typedef std::vector<token_type>             token_container;
typedef std::list<token_type>               token_list_container;
typedef std::vector<std::list<token_type> > token_container_container;

#endif

//! For preprocessing information including source comments, #include , #if, #define, etc
class  PreprocessingInfo
   {
     public:
      //  DQ (10/15/2002) moved this to nested scope to avoid global name pollution :-).
      //! MK: Enum type to store if the directive goes before or after the
      //! corresponding line of source code
          enum RelativePositionType
             {
               defaultValue = 0, // let the zero value be an error value
               undef        = 1, // Position of the directive is only going to be defined
                                 // when the preprocessing object is copied into the AST,
                                 // it remains undefined before that
               before       = 2, // Directive goes before the correponding code segment
               after        = 3, // Directive goes after the correponding code segment
               inside       = 4, // Directive goes inside the correponding code segment (as in between "{" and "}" of an empty basic block)

            // DQ (7/19/2008): Added additional fields so that we could use this enum type in the AstUnparseAttribute
            // replace       = 5, // Support for replacing the IR node in the unparsing of any associated subtree
               before_syntax = 6, // We still have to specify the syntax
               after_syntax  = 7  // We still have to specify the syntax
             };

       // Enum type to help classify the type for string that has been saved.
       // This helps in the unparsing to make sure that line feeds are introduced properly.
       //
       // Rama (08/17/07): Adding a CpreprocessorDeadIfDeclaration and its support
       // in various files.
          enum DirectiveType
             {
            // This is treated as an error
               CpreprocessorUnknownDeclaration,

            // These are a classification for comments
               C_StyleComment,
               CplusplusStyleComment,
               FortranStyleComment,

           // FMZ(5/14/2010): Added  freeform comments (started with "!")
               F90StyleComment,

            // DQ (11/20/2008): Added classification for blank line (a language independent form of comment).
               CpreprocessorBlankLine,

            // These used to be translated into IR nodes (and will be in the future).
               CpreprocessorIncludeDeclaration,
               CpreprocessorIncludeNextDeclaration,
               CpreprocessorDefineDeclaration,
               CpreprocessorUndefDeclaration,
               CpreprocessorIfdefDeclaration,
               CpreprocessorIfndefDeclaration,
               CpreprocessorIfDeclaration,
               CpreprocessorDeadIfDeclaration,
               CpreprocessorElseDeclaration,
               CpreprocessorElifDeclaration,
               CpreprocessorEndifDeclaration,
               CpreprocessorLineDeclaration,
               CpreprocessorErrorDeclaration,

            // DQ (10/19/2005): Added CPP warning directive
               CpreprocessorWarningDeclaration,
               CpreprocessorEmptyDeclaration,

            // AS (11/18/05): Added macro support (these are generated by the Wave 
            // support, but need to be better documented as to what they mean).
               CSkippedToken,
               CMacroCall,

            // AS & LIAO (8/12/2008): A PreprocessingInfo that is a 
            // hand made MacroCall that will expand into a valid statement.
               CMacroCallStatement,

            // DQ (11/28/2008): What does this mean!
            // A line replacement will replace a sub-tree in the AST
            // after a node with position (filename,line)
               LineReplacement,

            // The is the 'extern "C" {' construct.  Note that this is not captured in 
            // the EDG AST and it is required to be captured as part of the CPP and 
            // comment preprocessing.
               ClinkageSpecificationStart,
               ClinkageSpecificationEnd,

            // DQ (11/17/2008): Added support for #ident
               CpreprocessorIdentDeclaration,

            // DQ (11/17/2008): This handles the case CPP declarations (called "linemarkers")
            // (see Google for more details) such as: "# 1 "test2008_05.F90"", "# 1 "<built-in>"", 
            // "# 1 "<command line>"" "# 1 "test2008_05.F90""
            // The first token is the line number,
            // the second token is the filename (or string),
            // the optional tokens (zero or more) are flags:
            //   '1' indicates the start of a new file.
            //   '2' indicates returning to a file (having included another file).
            //   '3' indicates that the following text comes from a system header file, so certain warnings should be supressed. 
            //   '4' indicates that the following text should be treated as being wrapped in an implicit 'extern "C"' block
               CpreprocessorCompilerGeneratedLinemarker,

               LastDirectiveType
             };

       // DQ (7/10/2004): Make the data private
     private:

       // DQ (4/19/2006): Use the SgFileInfo object to hold the more complete 
       // information about the filename, line number, and column number.
          Sg_File_Info* file_info;
       // int   lineNumber;
       // int   columnNumber;

       // Use string class to improve implementation
       // char* stringPointer;
          std::string internalString;

          int   numberOfLines;

       // enum value representing a classification of the different types of directives
          DirectiveType whatSortOfDirective;

       // Corresponding enum value
          RelativePositionType relativePosition;

       // DQ (11/28/2008): Support for CPP generated linemarkers
          int lineNumberForCompilerGeneratedLinemarker;
          std::string filenameForCompilerGeneratedLinemarker;
          std::string optionalflagsForCompilerGeneratedLinemarker;

// This is part of Wave support in ROSE.
// #ifndef USE_ROSE
     public:
/*
       // AS using the lexer_token from boost_wave in order to store structures
          typedef boost::wave::cpplexer::lex_token<> token_type;
          typedef std::vector<token_type>            token_container;
          typedef std::list<token_type>              token_list_container;
          typedef std::vector<std::list<token_type> >       token_container_container;
*/
     private:
       // FIXME: To support Jochens AST binary save work the tokenSteam must
       // have a pointer type.

#ifndef ROSE_SKIP_COMPILATION_OF_WAVE
       // A stream of tokens representing the current prerpocessing info
       // object. This is equivalent to the internal string, but of cause
       // contains more information since it is a tokenized stream.
          token_container* tokenStream;

     public:         
          typedef struct r_include_directive
             {
            // The parameter 'directive' contains the (expanded) file name found after 
            // the #include directive. This has the format '<file>', '"file"' or 'file'.
               token_type directive;
            // The paths plus name to the include directive filename
               std::string absname;
               std::string relname;
             } rose_include_directive;

       // Internal representation of a macro #define directive
          typedef struct r_macro_def
             {
               bool is_functionlike; 
               bool is_predefined;
               token_type macro_name;
               token_container paramaters;
               token_list_container definition;
               r_macro_def() : macro_name(), paramaters(),definition() {}
             } rose_macro_definition;

       // Internal representation of a macro call
       // e.g #define MACRO_CALL int x;
       // MACRO_CALL
          typedef struct r_macro_call
             {
               bool is_functionlike;
               PreprocessingInfo* macro_def;
               token_type macro_call;
               token_container_container arguments;
               token_container expanded_macro;                

            // Get string representation of the expanded macro
               std::string get_expanded_string()
                  {
                    std::ostringstream os;
                    token_container::const_iterator iter;
                    for (iter=expanded_macro.begin(); iter!=expanded_macro.end(); iter++)
                         os << (*iter).get_value();
                    return os.str();
                  }

               r_macro_call() : macro_call(), arguments(),expanded_macro() {}
             } rose_macro_call;

     private:
       // AS add macro definition
          rose_macro_definition*  macroDef;
       // AS add macro call
          rose_macro_call*        macroCall;
       // AS include directive
          rose_include_directive* includeDirective;

#endif

  // member functions
     public:
         ~PreprocessingInfo();
          PreprocessingInfo();

// #ifndef USE_ROSE
#ifndef ROSE_SKIP_COMPILATION_OF_WAVE
       // AS (112105) Added constructors to support macros
          PreprocessingInfo(token_container, DirectiveType, RelativePositionType); 
          PreprocessingInfo(rose_macro_call*, RelativePositionType); 
          PreprocessingInfo(rose_macro_definition*, RelativePositionType); 
          PreprocessingInfo(token_type, token_list_container, bool, DirectiveType,RelativePositionType); 
          PreprocessingInfo(rose_include_directive*, RelativePositionType);
#endif

       // This constructor is called from the C++ code generated from the lex file (preproc.lex)
       // PreprocessingInfo(DirectiveType, const char *inputStringPointer, int line_no , int col_no,
       //                   int nol, RelativePositionType relPos, bool copiedFlag, bool unparsedFlag) ROSE_DEPRECATED_FUNCTION;

       // DQ (7/19/2008): I have removed the bool copiedFlag and bool unparsedFlag parameters because they are not used 
       // and are present only because in an older implementation of the unparser it would make the PreprocessingInfo 
       // as unparsed (and maybe copied) but this sort of side-effect of the unparser was later removed to make the 
       // unparsing side-effect free.
       // DQ (4/19/2006): Use the SgFileInfo object to hold the more complete 
       // information about the filename, line number, and column number.
       // DQ (3/15/2006): Build constructor that uses C++ string as input (to replace the char* based constructor)
       // PreprocessingInfo(DirectiveType, const std::string inputString, int line_no , int col_no,
       //                   int nol, RelativePositionType relPos, bool copiedFlag, bool unparsedFlag);
       // PreprocessingInfo(DirectiveType, const std::string & inputString,
       //      const std::string & filenameString, int line_no , int col_no,
       //      int nol, RelativePositionType relPos, bool copiedFlag, bool unparsedFlag );
          PreprocessingInfo(DirectiveType, const std::string & inputString,
               const std::string & filenameString, int line_no , int col_no,
               int nol, RelativePositionType relPos );

       // Copy constructor
          PreprocessingInfo(const PreprocessingInfo &prepInfo);

          void display(const std::string & label) const;

       // Access functions
          int getLineNumber() const;
          int getColumnNumber() const;
          std::string getString() const;
          void setString ( const std::string & s );
          int getStringLength() const;
          DirectiveType getTypeOfDirective() const;
          RelativePositionType getRelativePosition(void) const;
          void setRelativePosition(RelativePositionType relPos);

       // Number of lines occupied by this comment (count the number of line feeds)
          int getNumberOfLines() const;
          int getColumnNumberOfEndOfString() const; // only correct for single line directives

       // Used in unparse to string mechanism
       // char* removeLeadingWhiteSpace (const char* inputStringPointer);

       // DQ (8/6/2006): Modified to make these static functions
       // useful for debugging
          static std::string directiveTypeName (const DirectiveType & directive);
          static std::string relativePositionName (const RelativePositionType & position);

       // JH (01/03/2006) methods for packing the PreprocessingInfo data, in order to store it into
       // a file and rebuild it!
          unsigned int packed_size () const;

       // JH (01/032006) This pack methods might cause memory leaks. Think of deleting them after stored to file ...
          char* packed()  const;
          void unpacked( char* storePointer );

       // DQ (4/19/2006): Added Sg_File_Info objects to each PreprocessingInfo object
          Sg_File_Info* get_file_info() const;
          void set_file_info( Sg_File_Info* info );


       // DQ (11/28/2008): Support for CPP generated linemarkers
          int get_lineNumberForCompilerGeneratedLinemarker();
          std::string get_filenameForCompilerGeneratedLinemarker();
          std::string get_optionalflagsForCompilerGeneratedLinemarker();

       // DQ (11/28/2008): Support for CPP generated linemarkers
          void set_lineNumberForCompilerGeneratedLinemarker( int x );
          void set_filenameForCompilerGeneratedLinemarker( std::string x );
          void set_optionalflagsForCompilerGeneratedLinemarker( std::string x );

// #ifndef USE_ROSE
#ifndef ROSE_SKIP_COMPILATION_OF_WAVE
  // Wave specific member functions.
     public:
       // Access functions to get the macro call or macro definition.
       // These are NULL if the type is not CMacroCall or
       // CpreprocessorDefineDeclaration
          rose_macro_call* get_macro_call(); 
          rose_macro_definition* get_macro_def();
          rose_include_directive* get_include_directive();

          const token_container* get_token_stream();
          void push_front_token_stream(token_type tok);
          void push_back_token_stream(token_type tok);

#endif
   };

// DQ (10/15/2002) Changed list element from "PreprocessingInfo" to 
// "PreprocessingInfo*" to avoid redundant copying of internal data.
// Define a new data type for the container that stores the
// PreprocessingInfo objects attached to an AST node
typedef Rose_STL_Container<PreprocessingInfo*> AttachedPreprocessingInfoType;

class ROSEAttributesList
   {
     private:
       // DQ replaced use of old list class with STL
          std::vector<PreprocessingInfo*> attributeList;

          LexTokenStreamTypePointer rawTokenStream;

       // [DT] 3/15/2000 -- Name of file from which the directives come.
       // char fileName[256];
          std::string fileName;

       //      3/16/2000 -- Index into the list.  Not sure if this is really
       //      necessary.  See implementation in unparser.C.
       //
       // This is where the current line number is stored while we 
       // go off and unparse a different include file.  This really should have
       // been stored in a static structure (I think) rather than in this list.
          int index;

     public:
       // DQ (11/19/2008): Added language selection support for handling comments
          enum languageTypeEnum
             {
               e_unknown_language   = 0,
               e_C_language         = 1,
               e_Cxx_language       = 2,
               e_Fortran77_language = 3,
               e_Fortran9x_language = 4,
               e_lastLanguage
             };

          ROSEAttributesList();
         ~ROSEAttributesList();
       // DQ (4/19/2006): Adding SgFileInfo objects so we need to pass in a filename string
       // void addElement(PreprocessingInfo::DirectiveType, const char *pLine, int lineNumber, int columnNumber, int numberOfLines);
          void addElement(PreprocessingInfo::DirectiveType, const std::string & pLine, const std::string & filename, int lineNumber, int columnNumber, int numberOfLines);
#if 1
       // DQ (5/9/2007): This is required for WAVE support.
       // DQ (4/13/2007): I would like to remove this function, but it is used by WAVE support within ROSE.
          void addElement( PreprocessingInfo &pRef );
#endif
       // void addElements( ROSEAttributesList &);
          void moveElements( ROSEAttributesList &);

#if 1
       // DQ (5/9/2007): This is required for WAVE support.
       // DQ (4/13/2007): I would like to remove this function
          void insertElement( PreprocessingInfo & pRef );
#endif

       // [DT] 3/15/2000 -- Interface to fileName member.
          void setFileName(const std::string & fName);
          std::string getFileName();

       // 3/16/2000 -- Interface to index member.
          void setIndex(int i);
          int getIndex();

          PreprocessingInfo* operator[](int i);
          int size(void);
          int getLength(void);
          void deepClean(void);
          void clean(void);

       // Access function for list
          std::vector<PreprocessingInfo*> & getList() { return attributeList; };

          void display ( const std::string & label );          // DQ 02/18/2001 -- For debugging.

       // DQ (1/21/2008): Added access function to save the raw token stream from the lex pass.
          void set_rawTokenStream( LexTokenStreamTypePointer s );
          LexTokenStreamTypePointer get_rawTokenStream();

       // This function processes the token stream to generate the input for what weaves the 
       // CPP directives and comments into the AST.  All other tokens are ignore in this pass.
          void generatePreprocessorDirectivesAndCommentsForAST( const std::string & filename );

       // DQ (11/26/2008): This is old code!
       // Collection comments and CPP directives for fixed format (easier case)
       // void collectFixedFormatPreprocessorDirectivesAndCommentsForAST( const std::string & filename );

       // DQ (11/16/2008): Adding support for recognition of CPP directives outside of the lex tokenization.
          void collectPreprocessorDirectivesAndCommentsForAST( const std::string & filename, languageTypeEnum languageType );

       // DQ (11/17/2008): Refactored the code.
          bool isFortran77Comment( const std::string & line );
          bool isFortran90Comment( const std::string & line );
          bool isCppDirective( const std::string & line, PreprocessingInfo::DirectiveType & cppDeclarationKind, std::string & restOfTheLine );
   };

//
// [DT] 3/16/2000 -- Want to have preprocessing info for
//      each file included from the main source file.
//
class ROSEAttributesListContainer
   {
     private:
       // DQ replaced use of old list class with STL
       // std::vector<ROSEAttributesList*> attributeListList;
       // std::map<std::string,ROSEAttributesList*>* attrMap;
          std::map<std::string, ROSEAttributesList*> attributeListMap;

     public:
          ROSEAttributesListContainer();
         ~ROSEAttributesListContainer();
       // void addList ( ROSEAttributesList* listPointer );
          void addList ( std::string fileName, ROSEAttributesList* listPointer );
       // void addList(ROSEAttributesList &aRef);
       // void insertList(ROSEAttributesList &aRef);
       // ROSEAttributesList* operator[](int i);
       // ROSEAttributesList* findList ( const std::string & fName );

       // Check to see if the ROSEAttributesList for the fName (filename) is in the container
          bool isInList ( const std::string & fName );

       // int size(void);
       // int getLength(void);
          void dumpContents(void); // [DT] 3/16/2000 -- For debugging.
          void deepClean(void);
          void clean(void);
          ROSEAttributesList & operator[]( const std::string & fName);

       // Access function for list
       // std::vector<ROSEAttributesList*> & getList() { return attributeListList; };
          std::map<std::string, ROSEAttributesList*> & getList() { return attributeListMap; };
          void display ( const std::string & label );          // DQ 02/18/2001 -- For debugging.
   };


// #ifndef USE_ROSE
#ifndef ROSE_SKIP_COMPILATION_OF_WAVE

extern token_container wave_tokenStream;

#endif

#endif
