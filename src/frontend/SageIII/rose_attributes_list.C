
// #include "rose.h"
#include "sage3.h"
// #include "rose_attributes_list.h"


#ifdef USE_ROSE_BOOST_WAVE_SUPPORT
PreprocessingInfo::token_container wave_tokenStream;

///////////////////////////////////////////////////////////////////////////////
//  Include the token class from Wave
///////////////////////////////////////////////////////////////////////////////
//  Include Wave itself
#include <boost/wave.hpp>
//#include <boost/wave/grammars/cpp_xpression_grammar.hpp> //as_string

#endif

//AS(01/04/07) Global map of filenames to PreprocessingInfo*'s as it is inefficient
//to get this by a traversal of the AST
std::map<std::string,std::vector<PreprocessingInfo*>* > mapFilenameToAttributes;



// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;


// JH (01/03/2006) methods for packing the PreprocessingInfo data, in order to store it into
// a file and rebuild it!
unsigned int PreprocessingInfo::packed_size () const
   {
     ROSE_ASSERT(this != NULL);

     return
       sizeof (file_info) +
    // sizeof (lineNumber) +
    // sizeof (columnNumber) +
       sizeof (numberOfLines) +
       sizeof (whatSortOfDirective) +
       sizeof (relativePosition) +
       sizeof (unsigned int) +  // string size
       internalString.size();
  // and the stuff of macros ...
   }
// JH (01/03/2006) This pack methods might cause memory leaks. Think of deleting them after stored to file ...
char* PreprocessingInfo::packed()  const
   {
     ROSE_ASSERT(this != NULL);

     const char* saveString  = internalString.c_str();
     unsigned int stringSize = internalString.size();

  // Wouldn't padding of data cause us to under compute the size of the buffer?
     char* returnData = new char[ packed_size() ];

     char* storePointer = returnData;

  // printf ("Error, need to get the info out of the SgFileInfo object! \n");
  // ROSE_ASSERT(false);
     memcpy (storePointer , (char*)(&file_info), sizeof(file_info) );
     storePointer += sizeof(file_info);
  // memcpy (storePointer , (char*)(&lineNumber), sizeof(lineNumber) );
  // storePointer += sizeof(lineNumber);
  // memcpy (storePointer , (char*)(&columnNumber), sizeof(columnNumber) );
  // storePointer += sizeof(columnNumber);

     memcpy (storePointer , (char*)(&numberOfLines), sizeof(numberOfLines) );
     storePointer += sizeof(numberOfLines);
     memcpy (storePointer , (char*)(&whatSortOfDirective),  sizeof(whatSortOfDirective) );
     storePointer +=  sizeof(DirectiveType) ;
     memcpy (storePointer , (char*)(&relativePosition), sizeof(relativePosition) );
     storePointer +=  sizeof(RelativePositionType);
     memcpy (storePointer , (char*)(&stringSize), sizeof(stringSize) );
     storePointer +=  sizeof(stringSize);
     memcpy (storePointer , saveString, stringSize );
#if 0
     std::cout << "  packed data in PrerocessingInfo ... " << std::endl;
#endif

  // DQ and AS (6/23/2006): and the stuff of macros ...
#ifdef USE_ROSE_BOOST_WAVE_SUPPORT
  // AS add macro definition
  // macroDef = NULL;
  // AS add macro call
  // macroCall = NULL;
#endif

     return returnData;
   }
// JH (01/03/2006) This unpack method wors conremplary to packed ...
void PreprocessingInfo::unpacked( char* storePointer )
   {
     ROSE_ASSERT(this != NULL);

  //    std::cout << " in PreprocessingInfo::unpacked ... " << std::endl;
  // printf ("Error, need to build a new SgFileInfo object! \n");
  // ROSE_ASSERT(false);
     memcpy ( (char*)(&file_info), storePointer, sizeof(file_info) );
     storePointer += sizeof(file_info);
  // memcpy ( (char*)(&lineNumber), storePointer, sizeof(lineNumber) );
  // storePointer += sizeof(lineNumber);
  // memcpy ( (char*)(&columnNumber), storePointer, sizeof(columnNumber) );
  // storePointer += sizeof(columnNumber);

     memcpy ( (char*)(&numberOfLines), storePointer, sizeof(numberOfLines) );
     storePointer += sizeof(numberOfLines);
     memcpy ( (char*)(&whatSortOfDirective), storePointer , sizeof(whatSortOfDirective) );
     storePointer +=  sizeof(DirectiveType) ;
     memcpy ( (char*)(&relativePosition), storePointer , sizeof(relativePosition) );
     storePointer +=  sizeof(RelativePositionType);
     int stringSize = 0;
     memcpy ( (char*)(&stringSize), storePointer, sizeof(stringSize) );
     storePointer +=  sizeof(stringSize);
#if 0
     std::cout << " getting in trouble at String ... " << std::endl;
#endif
     internalString = std::string ( storePointer, stringSize );
#if 0
     std::cout << " but survived " << std::endl;
#endif

  // DQ and AS (6/23/2006): and the stuff of macros ...
#ifdef USE_ROSE_BOOST_WAVE_SUPPORT
  // AS add macro definition
     macroDef = NULL;
  // AS add macro call
     macroCall = NULL;
     tokenStream = NULL;
#endif
   }


// ********************************************
// Member functions for class PreprocessingInfo
// ********************************************


#if USE_ROSE_BOOST_WAVE_SUPPORT
// AS(012006) Added to support macros
PreprocessingInfo::rose_macro_call* PreprocessingInfo::get_macro_call(){ return macroCall; } 
// AS(012006) Added to support macros
PreprocessingInfo::rose_macro_definition* PreprocessingInfo::get_macro_def(){ return macroDef; } 

//AS(060706) Added support for include directive
PreprocessingInfo::rose_include_directive* PreprocessingInfo::get_include_directive(){ return includeDirective; } 

const PreprocessingInfo::token_container* PreprocessingInfo::get_token_stream(){ return tokenStream; } 


// AS(012006) Added to support macros
PreprocessingInfo::PreprocessingInfo(token_container tokCont, DirectiveType typeOfDirective, RelativePositionType relPos) 
   : whatSortOfDirective(typeOfDirective), relativePosition(relPos)
   {

  //ROSE_ASSERT(false);
  //implement the position information
     tokenStream = new token_container();

     int lineNo = tokCont[0].get_position().get_line(); 
     int colNo  = tokCont[0].get_position().get_column(); 

     file_info = new Sg_File_Info(tokCont[0].get_position().get_file().c_str(),lineNo,colNo);
     ROSE_ASSERT(file_info != NULL);

  // DQ (12/23/2006): Mark this as a comment or directive (mostly so that we can know that the parent being NULL is not meaningful.
     file_info->setCommentOrDirective();

  // lineNumber   = lineNo;//macroDef->macrodef.lineNumber;
  // columnNumber = colNo; //macroDef->macrodef.columnNumber;

     (*tokenStream)= tokCont;
     internalString = string(boost::wave::util::impl::as_string(*tokenStream).c_str());

     if(SgProject::get_verbose() >= 1)
         std::cout << " String for declaration:" << internalString<< " at line: " << lineNo << " and col:" << colNo << std::endl;

   }

//AS(012006) Added to support macros
PreprocessingInfo::PreprocessingInfo(rose_macro_call* mcall, RelativePositionType relPos) 
   : macroCall(mcall), relativePosition(relPos)
   {
     tokenStream = new token_container();
	  
     whatSortOfDirective = PreprocessingInfo::CMacroCall;
     ROSE_ASSERT(mcall != NULL);
     ROSE_ASSERT(mcall->macro_def != NULL);
  // implement the position information

     int lineNo = mcall->macro_call.get_position().get_line(); 
     int colNo  = mcall->macro_call.get_position().get_column(); 

     macroCall = mcall;

     tokenStream->push_back(macroCall->macro_call); 

     typedef token_container_container::const_iterator vec_call_iterator_t;

  //BEGIN: Make a copy of the arguments

     vec_call_iterator_t it = macroCall->arguments.begin();
     vec_call_iterator_t it_end = macroCall->arguments.end();

     if(macroCall->is_functionlike == true){
          token_type tk1(boost::wave::T_LEFTPAREN,"(",boost::wave::util::file_position_type(BOOST_WAVE_STRINGTYPE(),lineNo,colNo));
          tokenStream->push_back(tk1);

          while (it != it_end ){
            //         std::cout << boost::wave::util::impl::as_string(*it);
               copy (it->begin(), it->end(),
                   inserter(*tokenStream, tokenStream->end()));
               token_type tk(boost::wave::T_COMMA,",",boost::wave::util::file_position_type("",lineNo,colNo));
               ++it;
               if(it != it_end)
                    tokenStream->push_back(tk);

          }
          token_type tk2(boost::wave::T_RIGHTPAREN,")",boost::wave::util::file_position_type("",lineNo,colNo));
          tokenStream->push_back(tk2);
     }


     file_info = new Sg_File_Info( mcall->macro_call.get_position().get_file().c_str(),lineNo,colNo);
  // lineNumber     = lineNo;
  // columnNumber   = colNo;

  // DQ (12/23/2006): Mark this as a comment or directive (mostly so that we can know that the parent being NULL is not meaningful.
     file_info->setCommentOrDirective();

     internalString = string(boost::wave::util::impl::as_string(*tokenStream).c_str()) ;
   }

PreprocessingInfo::PreprocessingInfo(rose_macro_definition* mdef, RelativePositionType relPos) 
   : macroDef(mdef), relativePosition(relPos)
   {
     tokenStream = new token_container();

     whatSortOfDirective = PreprocessingInfo::CpreprocessorDefineDeclaration;
     ROSE_ASSERT(mdef != NULL);
     macroDef = mdef;
  //implement the position information
     int lineNo, colNo;
     string filename;
     if(mdef->is_predefined == false){
         lineNo = mdef->macro_name.get_position().get_line(); 
         colNo  = mdef->macro_name.get_position().get_column(); 
         filename = std::string(mdef->macro_name.get_position().get_file().c_str());
     }else{
         lineNo = 0;
         colNo  = 0;
         filename="<builltin>";
     }
     file_info = new Sg_File_Info(filename, lineNo, colNo);

     tokenStream->push_back(macroDef->macro_name); 

     typedef token_container::const_iterator vec_call_iterator_t;

  //BEGIN: Make a copy of the arguments

     vec_call_iterator_t it = macroDef->paramaters.begin();
     vec_call_iterator_t it_end = macroDef->paramaters.end();


     if(macroDef->is_functionlike == true){
          token_type tk1(boost::wave::T_LEFTPAREN,"(",boost::wave::util::file_position_type(filename.c_str(),lineNo,colNo));
          tokenStream->push_back(tk1);

          while (it != it_end ){
            //         std::cout << boost::wave::util::impl::as_string(*it);
               tokenStream->push_back(*it);
               token_type tk(boost::wave::T_COMMA,",",boost::wave::util::file_position_type(filename.c_str(),lineNo,colNo));
               ++it;
               if(it != it_end)
                    tokenStream->push_back(tk);

          }
          token_type tk2(boost::wave::T_RIGHTPAREN,")",boost::wave::util::file_position_type(filename.c_str(),lineNo,colNo));
          tokenStream->push_back(tk2);
     }

     token_type tk1(boost::wave::T_SPACE,"\t",boost::wave::util::file_position_type(filename.c_str(),lineNo,colNo));
     tokenStream->push_back(tk1);

     copy (macroDef->definition.begin(), macroDef->definition.end(),
         inserter(*tokenStream, tokenStream->end()));

  //Support macros declared on the commandline. If declared on the commandline
  //set filename to ""


  // lineNumber     = lineNo;
  // columnNumber   = colNo;

  // DQ (12/23/2006): Mark this as a comment or directive (mostly so that we can know that the parent being NULL is not meaningful.
     file_info->setCommentOrDirective();

     internalString = string("#define\t")+string(boost::wave::util::impl::as_string(*tokenStream).c_str()) ;
     if(SgProject::get_verbose() >= 1)
         std::cout << "Internal string is: " << internalString << std::endl;
  //     internalString = boost::wave::util::impl::as_string(tokenStream) ;
   }


PreprocessingInfo::PreprocessingInfo(rose_include_directive* inclDir, RelativePositionType relPos) 
   : includeDirective(inclDir), relativePosition(relPos)
   {
     tokenStream = new token_container();

     whatSortOfDirective = PreprocessingInfo::CpreprocessorIncludeDeclaration;
     ROSE_ASSERT(inclDir != NULL);
  //implement the position information
     int lineNo = inclDir->directive.get_position().get_line(); 
     int colNo  = inclDir->directive.get_position().get_column(); 


  //Support macros declared on the commandline. If declared on the commandline
  //set filename to ""

     if(inclDir->directive.get_position().get_file().size() != 0)
          file_info = new Sg_File_Info(std::string(inclDir->directive.get_position().get_file().c_str()),lineNo,colNo);
     else
          file_info = new Sg_File_Info("", lineNo, colNo);

  // DQ (12/23/2006): Mark this as a comment or directive (mostly so that we can know that the parent being NULL is not meaningful.
     file_info->setCommentOrDirective();

#if 0
     lineNumber     = lineNo;
     columnNumber   = colNo;
#endif
     internalString = std::string(inclDir->directive.get_value().c_str()) ;

   }


  PreprocessingInfo::PreprocessingInfo( token_type directive, 
   token_list_container expression, bool expression_value, 
   DirectiveType dirType, RelativePositionType relPos )
   : relativePosition(relPos)
   {
     tokenStream = new token_container();

  // ROSE_ASSERT(false);
  // implement the position information
     int lineNo = directive.get_position().get_line(); 
     int colNo  = directive.get_position().get_column(); 

     whatSortOfDirective = dirType;
     file_info = new Sg_File_Info(directive.get_position().get_file().c_str(),lineNo,colNo);
  // lineNumber   = lineNo;//macroDef->macrodef.lineNumber;
  // columnNumber = colNo; //macroDef->macrodef.columnNumber;

  // DQ (12/23/2006): Mark this as a comment or directive (mostly so that we can know that the parent being NULL is not meaningful.
     file_info->setCommentOrDirective();

     tokenStream->push_back(directive);
     token_type tk1(boost::wave::T_SPACE," ",boost::wave::util::file_position_type(directive.get_position().get_file().c_str(),lineNo,colNo));
     tokenStream->push_back(tk1);
     copy (expression.begin(), expression.end(),
         inserter(*tokenStream, tokenStream->end()));

     internalString = string(boost::wave::util::impl::as_string(*tokenStream).c_str()) +"\n";
     if(SgProject::get_verbose() >= 1)
        std::cout << "INTERNAL IF STRING: " << internalString << std::endl;

   }

 // endif for USE_ROSE_BOOST_WAVE_SUPPORT
#endif


PreprocessingInfo::PreprocessingInfo()
   {
  // Set these values so that they are not set to zero (a valid value) if a PreprocessingInfo object is reused

  // DQ (4/22/2006): This default constructor is called by the 
  // EasyStorage<PreprocessingInfo*>::rebuildDataStoredInEasyStorageClass()
  // if this constructor builds a Sg_File_Info object during the AST 
  // reconstruction phase then errors result in the final AST. 
  // Very Strange Errors!!!

  // DQ (4/21/2006): These are illegal values for a Sg_File_Info object
  // file_info = new Sg_File_Info("comment filename",-1,-1);
  // file_info = new Sg_File_Info("comment filename",0,0);
     file_info = NULL;
  // lineNumber          = -1;
  // columnNumber        = -1;

     numberOfLines       = -1;
     whatSortOfDirective = CpreprocessorUnknownDeclaration;
     relativePosition    = before;
   }

// Typical constructor used by lex-based code retrieve comments and preprocessor control directives
PreprocessingInfo::PreprocessingInfo (
    DirectiveType dt, 
    const string & inputString,
    const string & inputFileName,
    int line_no, int col_no, int nol, RelativePositionType relPos,
    bool copiedFlag, bool unparsedFlag )
   : 
     file_info(NULL),
  // lineNumber(line_no), columnNumber (col_no),
     numberOfLines(nol),
     whatSortOfDirective(dt),
     relativePosition(relPos)
   {
  // DQ (10/29/2007): Test the filename is a way similar to how it is failing in lower level code
     if (inputFileName == "NULL_FILE")
        {
       // printf ("In PreprocessingInfo constructor, inputFileName == "NULL_FILE" \n");
          ROSE_ASSERT(true);
        }

  // printf ("In PreprocessingInfo (constructor): dt = %d line_no = %d col_no = %d nol = %d s = %s \n",dt,line_no,col_no,nol,inputStringPointer.c_str());
     file_info = new Sg_File_Info(inputFileName,line_no,col_no);

  // DQ (12/23/2006): Mark this as a comment or directive (mostly so that we can know
  // that the parent being NULL is not meaningful in the AST consistancy tests).
     file_info->setCommentOrDirective();

  // DQ (6/13/2007): Set the parent to a shared type for now so that it is at least set to a non-null value
  // This can if we like to used as a signature for Sg_File_Info nodes that are associated with comments and directives.
  // file_info->set_parent(file_info);
  // file_info->set_parent(SgTypeShort::get_builtin_type());
     file_info->set_parent(SgTypeLongLong::createType());

  // DQ (4/15/2007): Temp code to trace common position in unparsing.
  // internalString = inputString;
#if 0
  // DQ (4/15/2007): This adds the line number to the C or C++ style comments to trace there position in the unparsed (generated) code.
     if (dt == CplusplusStyleComment || dt == C_StyleComment)
          internalString = string("/* line = ") + StringUtility::numberToString(line_no) + string(" */ ") + inputString;
       else
          internalString = inputString;
#else
  // Normal code
     internalString = inputString;
#endif
   }

// Copy constructor
PreprocessingInfo::PreprocessingInfo(const PreprocessingInfo & prepInfo)
   {
     ROSE_ASSERT(prepInfo.file_info != NULL);
     file_info           = new Sg_File_Info(*(prepInfo.file_info));

  // DQ (12/23/2006): Mark this as a comment or directive (mostly so that we can know that the parent being NULL is not meaningful.
     file_info->setCommentOrDirective();

  // lineNumber          = prepInfo.getLineNumber();
  // columnNumber        = prepInfo.getColumnNumber();
     numberOfLines       = prepInfo.getNumberOfLines();
     whatSortOfDirective = prepInfo.getTypeOfDirective();
     relativePosition    = prepInfo.getRelativePosition();
     internalString      = prepInfo.internalString;
   }

PreprocessingInfo::~PreprocessingInfo()
   {
     ROSE_ASSERT(this != NULL);

  // Reset these values so that they are not set to zero (a valid value) if a PreprocessingInfo object is reused
     delete file_info;
     file_info            = NULL;
  // lineNumber          = -1;
  // columnNumber        = -1;
     numberOfLines       = -1;
     relativePosition    = undef;
     whatSortOfDirective = CpreprocessorUnknownDeclaration;
     internalString      = "";
   }

/* starting column == 1 (DQ (10/27/2006): used to be 0, but changed to 1 for consistancy with EDG) */
int
PreprocessingInfo::getColumnNumberOfEndOfString() const
   {
     ROSE_ASSERT(this != NULL);
     int col = 1;
     int i   = 0;

  // DQ (10/27/2006): the last line has a '\n' so we need the length 
  // of the last line before the '\n" triggers the counter to be reset!
  // This fix is required because the strings we have include the final '\n"
     int previousLineLength = col;
     while (internalString[i] != '\0')
        {
          if (internalString[i] == '\n')
             {
               previousLineLength = col;
               col = 0;
             }
            else
             {
               col++;
               previousLineLength = col;
             }
          i++;
        }

     int endingColumnNumber   = previousLineLength;

  // If this is a one line comment then the ending position is the length of the comment PLUS the starting column position
     if (getNumberOfLines() == 1)
          endingColumnNumber += get_file_info()->get_col() - 1;

     return endingColumnNumber;
   }

PreprocessingInfo::DirectiveType
PreprocessingInfo::getTypeOfDirective () const
   {
  // Access function for the type of directive
     ROSE_ASSERT(this != NULL);
     return whatSortOfDirective;
   }

#if 0
************* OLD CODE **************
char*
PreprocessingInfo::removeLeadingWhiteSpace (const char* inputStringPointer)
   {
  // This returns a substring that starts with the first non blank character
     const char* startOfStringExcludingLeadingWhiteSpace = inputStringPointer;
     ROSE_ASSERT (startOfStringExcludingLeadingWhiteSpace != NULL);

     do {
       // Find the number of ' ' characters at the start of the string
          unsigned int lengthOfLeadingBlanks = strspn (startOfStringExcludingLeadingWhiteSpace," ");
          ROSE_ASSERT (lengthOfLeadingBlanks < strlen(startOfStringExcludingLeadingWhiteSpace));

       // Find the number of TAB characters at the start of the string
          unsigned int lengthOfLeadingTabs   = strspn (startOfStringExcludingLeadingWhiteSpace,"\t");
          ROSE_ASSERT (lengthOfLeadingTabs   < strlen(startOfStringExcludingLeadingWhiteSpace));

          int lengthOfLeadingWhiteSpace = lengthOfLeadingBlanks + lengthOfLeadingTabs;

       // Get the new address into the existing string (using pointer arithmetic)
       // startOfStringExcludingLeadingWhiteSpace = &( ((char*) startOfStringExcludingLeadingWhiteSpace)[lengthOfLeadingWhiteSpace] );
          startOfStringExcludingLeadingWhiteSpace += lengthOfLeadingWhiteSpace;
     }
     while ( (startOfStringExcludingLeadingWhiteSpace[0] == ' ') || 
         (startOfStringExcludingLeadingWhiteSpace[0] == '\t') );

     ROSE_ASSERT (startOfStringExcludingLeadingWhiteSpace != NULL);

     return (char *)startOfStringExcludingLeadingWhiteSpace;
   }
************* OLD CODE **************
#endif

string
PreprocessingInfo::directiveTypeName ( const DirectiveType & directive )
   {
     string returnString;
     switch (directive)
        {
          case CpreprocessorUnknownDeclaration: 
             returnString = "CpreprocessorUnknownDeclaration";
             break;
          case FortranStyleComment:
             returnString = "FortranStyleComment";
             break;
          case C_StyleComment:
             returnString = "C_StyleComment";
             break;
          case CplusplusStyleComment:
             returnString = "CplusplusStyleComment";
             break;
          case CpreprocessorIncludeDeclaration:
             returnString = "CpreprocessorIncludeDeclaration";
             break;
          case CpreprocessorIncludeNextDeclaration:
             returnString = "CpreprocessorIncludeNextDeclaration";
             break;
          case CpreprocessorDefineDeclaration:
             returnString = "CpreprocessorDefineDeclaration";
             break;
          case CpreprocessorUndefDeclaration:
             returnString = "CpreprocessorUndefDeclaration";
             break;
          case CpreprocessorIfdefDeclaration:
             returnString = "CpreprocessorIfdefDeclaration";
             break;
          case CpreprocessorElseDeclaration:
             returnString = "CpreprocessorElseDeclaration";
             break;
          case CpreprocessorElifDeclaration:
             returnString = "CpreprocessorElifDeclaration";
             break;
          case CpreprocessorIfndefDeclaration:
             returnString = "CpreprocessorIfndefDeclaration";
             break;
          case CpreprocessorIfDeclaration:
             returnString = "CpreprocessorIfDeclaration";
             break;
          case CpreprocessorDeadIfDeclaration:
             returnString = "CpreprocessorDeadIfDeclaration";
             break;
          case CpreprocessorEndifDeclaration:
             returnString = "CpreprocessorEndifDeclaration";
             break;
          case CpreprocessorLineDeclaration:
             returnString = "CpreprocessorLineDeclaration";
             break;
          case ClinkageSpecificationStart:
             returnString = "ClinkageSpecificationStart";
             break;
          case ClinkageSpecificationEnd:
             returnString = "ClinkageSpecificationEnd";
             break;
          case CpreprocessorErrorDeclaration:
             returnString = "CpreprocessorErrorCDeclaration";
             break;
          case CpreprocessorWarningDeclaration:
             returnString = "CpreprocessorWarningDeclaration";
             break;
          case CpreprocessorEmptyDeclaration:
             returnString = "CpreprocessorEmptyCDeclaration";
             break;
          case CSkippedToken:
             returnString = "CSkippedToken";
             break;
          case CMacroCall:
             returnString = "CMacroCall";
             break;
          case LineReplacement:
             returnString = "LineReplacement";
             break;

          default:
             returnString = "ERROR DEFAULT REACHED";
             printf ("Default reached in PreprocessingInfo::directiveTypeName() exiting ... (directive = %d) \n",directive);
             ROSE_ABORT();
             break;
        }

     return returnString;
   }


int
PreprocessingInfo::getLineNumber() const
   {
     ROSE_ASSERT(this != NULL);
     ROSE_ASSERT(file_info != NULL);
     return file_info->get_line();
  // return lineNumber;
   }

int
PreprocessingInfo::getColumnNumber() const
   {
     ROSE_ASSERT(this != NULL);
     ROSE_ASSERT(file_info != NULL);
     return file_info->get_col();
  // return columnNumber;
   }

string
PreprocessingInfo::getString() const
   {
     ROSE_ASSERT(this != NULL);
     return internalString;
   }

void
PreprocessingInfo::setString ( const std::string & s )
   {
     ROSE_ASSERT(this != NULL);
     internalString = s;
   }

int
PreprocessingInfo::getNumberOfLines() const
   {
     ROSE_ASSERT(this != NULL);
#if 1
     int line = 0;
     int i = 0;
     while (internalString[i] != '\0')
        {
          if (internalString[i] == '\n')
             {
               line++;
             }
          i++;
        }

     if (line == 0)
          line = 1;

     ROSE_ASSERT(line > 0);
     return line;
#else
     return numberOfLines;
#endif
   }

void
PreprocessingInfo::display (const string & label) const
   {
     printf ("\n");
     printf ("Inside of PreprocessingInfo display(%s): \n",label.c_str());
     ROSE_ASSERT(this != NULL);
     file_info->display(label);
  // printf ("     lineNumber     = %d \n",lineNumber);
  // printf ("     columnNumber   = %d \n",columnNumber);
     printf ("     numberOfLines  = %d \n",numberOfLines);
     printf ("     relativePosition = %s \n",relativePositionName(relativePosition).c_str());
     printf ("     directiveType  = %s \n",directiveTypeName(whatSortOfDirective).c_str());
     printf ("     internalString = %s \n",internalString.c_str());
     printf ("\n");
   }

std::string
PreprocessingInfo::relativePositionName (const RelativePositionType & position)
   {
     string returnString;
     switch (position)
        {
          case defaultValue: 
             returnString = "defaultValue";
             break;
          case undef: 
             returnString = "undef";
             break;
          case before: 
             returnString = "before";
             break;
          case after: 
             returnString = "after";
             break;
          case inside: 
             returnString = "inside";
             break;

          default:
             returnString = "ERROR DEFAULT REACHED";
             printf ("Default reached in PreprocessingInfo::relativePositionName() exiting ... \n");
             ROSE_ABORT();
             break;
        }

     return returnString;
   }


PreprocessingInfo::RelativePositionType
PreprocessingInfo::getRelativePosition(void) const
   {
     ROSE_ASSERT(this != NULL);

     return relativePosition;
   }

  void
PreprocessingInfo::setRelativePosition( RelativePositionType relPos )
   {
     ROSE_ASSERT(this != NULL);

     relativePosition = relPos;
   }

int
PreprocessingInfo::getStringLength(void) const
   {
     ROSE_ASSERT(this != NULL);

     return internalString.length();
   }

Sg_File_Info*
PreprocessingInfo::get_file_info() const
   {
     ROSE_ASSERT(this != NULL);

     ROSE_ASSERT(file_info != NULL);
     return file_info;
   }

  void
PreprocessingInfo::set_file_info( Sg_File_Info* info )
   {
     ROSE_ASSERT(this != NULL);

     file_info = info;
     ROSE_ASSERT(file_info != NULL);
   }




// *********************************************
// Member functions for class ROSEATTRIBUTESList
// *********************************************

ROSEAttributesList::ROSEAttributesList()
   {
     index = 0;
   }

ROSEAttributesList::~ROSEAttributesList()
   {
  // Nothing to do here
   }

#if 0
// Old code!
void
ROSEAttributesList::addElement(
    PreprocessingInfo::DirectiveType dt, const char *pLine,
    int lineNumber, int columnNumber, int numOfLines )
   {
     ROSE_ASSERT(pLine!=0);
     ROSE_ASSERT(*pLine!='\0');
     ROSE_ASSERT(lineNumber>0);
     ROSE_ASSERT(columnNumber>0); 
     ROSE_ASSERT(numOfLines>=0); // == 0, if cpp_comment in a single line
     PreprocessingInfo *pElem = new PreprocessingInfo(dt, pLine, lineNumber, columnNumber,
         numOfLines, PreprocessingInfo::undef, false, false);
     PreprocessingInfo &pRef = *pElem;
     insertElement(pRef);
   }
#else
void
ROSEAttributesList::addElement(
    PreprocessingInfo::DirectiveType dt, const std::string & pLine,
    const std::string & filename, int lineNumber, int columnNumber, int numOfLines )
   {
     ROSE_ASSERT(this != NULL);
  // ROSE_ASSERT(pLine != NULL);

  // DQ (10/28/2007): An empty file name is now allowed (to handle #line 1 "" directives)
  // ROSE_ASSERT(filename.empty() == false);
     ROSE_ASSERT(pLine.empty() == false);
     ROSE_ASSERT(lineNumber    >  0);
     ROSE_ASSERT(columnNumber  >  0); 
     ROSE_ASSERT(numOfLines    >= 0); // == 0, if cpp_comment in a single line
     PreprocessingInfo *pElem = new PreprocessingInfo(dt, pLine, filename, lineNumber, columnNumber,
         numOfLines, PreprocessingInfo::undef, false, false);

  // PreprocessingInfo &pRef = *pElem;
  // insertElement(pRef);
     attributeList.push_back(pElem);
   }
#endif

void
ROSEAttributesList::moveElements( ROSEAttributesList & pList )
   {
     ROSE_ASSERT(this != NULL);

     int length = pList.size();
     if ( length > 0 )
        {
          vector<PreprocessingInfo*>::iterator i = pList.attributeList.begin();
          for( i = pList.attributeList.begin(); i != pList.attributeList.end(); i++)
             {
               length = pList.getLength();
            // PreprocessingInfo *pElem = new PreprocessingInfo((*i)->stringPointer, (*i)->lineNumber, (*i)->columnNumber);
            // PreprocessingInfo & pRef = *pElem;
#if 0
            // DQ (4/13/2007): This call to insertElement() forces a retraversal of the list which is not required.
            // This makes this step take 9 seconds in the processing of Cxx_Grammar.C.
               PreprocessingInfo & pRef = *(*i);
               insertElement( pRef );
#else
            // DQ (4/13/2007): Skip the insertElement() which requires a traversal over the list, 
            // we are building this in order so the order is preserved in copying from pList.
            // This is a performance optimization.
               PreprocessingInfo & pRef = *(*i);
               attributeList.push_back(&pRef);
#endif
             }

       // empty the STL list
          vector<PreprocessingInfo*>::iterator head = pList.attributeList.begin();
          vector<PreprocessingInfo*>::iterator tail = pList.attributeList.end();
          pList.attributeList.erase(head,tail);
          ROSE_ASSERT (pList.attributeList.size() == 0);
        }
   }

#if 1
// DQ (5/9/2007): This is required for WAVE support.
// DQ (4/13/2007): I would like to remove this function, but this is part of WAVE support
void
ROSEAttributesList::addElement( PreprocessingInfo &pRef )
   {
     ROSE_ASSERT(this != NULL);

     insertElement(pRef);
   }
#endif

#if 1
// DQ (5/9/2007): This is required for WAVE support.
// DQ (4/13/2007): I would like to remove this function
void
ROSEAttributesList::insertElement( PreprocessingInfo & pRef )
   {
     ROSE_ASSERT(this != NULL);

     int done = 0;
     vector<PreprocessingInfo*>::iterator i = attributeList.begin();
     if ( attributeList.size() > 0 )
        {
       // Note that the insertion requires a traversal over the whole list 
       // that is being built (n^2 complexity if used in moveElements()).
          while( i != attributeList.end() )
             {
               if( (*i)->getLineNumber() <= pRef.getLineNumber())
                  {
                    i++;
                  }
                 else
                  {
                    attributeList.insert( i, &pRef );
                    done = 1;
                    break;
                  }
             }

       // If it has not been added yet, then at least include it at the end!
          if(!done)
               attributeList.push_back( &pRef );
        }
       else
        {
       // Handle the first element of the list directly
          attributeList.push_back( &pRef );   
        }
   }
#endif

void
ROSEAttributesList::setFileName(const string & fName)
   {
     ROSE_ASSERT(this != NULL);

  // Should have an assert(fName!=NULL) here?
  // strcpy(fileName,fName);
     fileName = fName;

     printf ("Verify that the filenames are correct for all comments in this list! \n");
     ROSE_ASSERT(false);

  // Error checking!
     vector<PreprocessingInfo*>::iterator i = attributeList.begin();
     while( i != attributeList.end() )
        {
          ROSE_ASSERT( (*i)->get_file_info()->get_filenameString() == getFileName());
          i++;
        }
   }

string
ROSEAttributesList::getFileName()
   {
     ROSE_ASSERT(this != NULL);
     return fileName;
   }

void
ROSEAttributesList::setIndex(int i)
   {
     ROSE_ASSERT (this != NULL);
     index = i;
   }

int
ROSEAttributesList::getIndex()
   {
     ROSE_ASSERT (this != NULL);
     return index;
   }

int
ROSEAttributesList::size(void)
   {
     ROSE_ASSERT(this != NULL);
     return getLength();
   }

int
ROSEAttributesList::getLength(void)
   {
     ROSE_ASSERT(this != NULL);
     return attributeList.size();
   }

void
ROSEAttributesList::clean(void)
   {
     ROSE_ASSERT(this != NULL);

  // Nothing to do here?
   }

void
ROSEAttributesList::deepClean(void)
   {
     ROSE_ASSERT(this != NULL);

  // Nothing to do here?
  // DQ (9/6/2001) Bugfix: Implemented removal of all elements of the list (required for multi source file support)
     vector<PreprocessingInfo*>::iterator head = attributeList.begin();
     vector<PreprocessingInfo*>::iterator tail = attributeList.end();
     attributeList.erase(head,tail);
     ROSE_ASSERT (attributeList.size() == 0);
   }

PreprocessingInfo* 
ROSEAttributesList::operator[]( int i)
   {
     ROSE_ASSERT(this != NULL);
     return attributeList[i];
   }

void
ROSEAttributesList::display ( const string & label )
   {
     printf ("ROSEAttributesList::display (label = %s) \n",label.c_str());
     ROSE_ASSERT(this != NULL);

  // fprintf(outFile,"\n%s: \n", getFileName() );
     vector<PreprocessingInfo*>::iterator j = attributeList.begin();
     for (j = attributeList.begin(); j != attributeList.end(); j++)
        {
       // printf("  %s\n",( (*j)->stringPointer );
          ROSE_ASSERT ( (*j) != NULL );
          printf("LineNumber: %5d: %s\n",(*j)->getLineNumber(),(*j)->getString().c_str());
        }
   }

void
ROSEAttributesList::set_rawTokenStream( LexTokenStreamTypePointer s )
   {
     ROSE_ASSERT (this != NULL);
     rawTokenStream = s;
   }

LexTokenStreamTypePointer
ROSEAttributesList::get_rawTokenStream()
   {
     ROSE_ASSERT (this != NULL);
     return rawTokenStream;
   }


// void ROSEAttributesList::generatePreprocessorDirectivesAndCommentsForAST( SgFile* file )
void
ROSEAttributesList::generatePreprocessorDirectivesAndCommentsForAST( const string & filename )
   {
  // This function does not work for fixed-format, which is processed seperately.
  // This function reads the token stream and extracts out the comments for inclusion into the attributeList.

     ROSE_ASSERT (this != NULL);
     ROSE_ASSERT (filename.empty() == false);

     ROSE_ASSERT(rawTokenStream != NULL);
     ROSE_ASSERT(attributeList.empty() == true);

  // printf ("In ROSEAttributesList::generatePreprocessorDirectivesAndCommentsForAST(): rawTokenStream->size() = %zu \n",rawTokenStream->size());

     int count = 0;
     LexTokenStreamType::iterator i = rawTokenStream->begin();
     while (i != rawTokenStream->end())
        {
       // print out the tokens
       // printf ("token stream element #%d \n",count);
          token_element* token = (*i)->p_tok_elem;
          ROSE_ASSERT(token != NULL);
          file_pos_info & start = (*i)->beginning_fpi;

          bool isComment = (token->token_id == SgToken::FORTRAN_COMMENTS);
#if 0
          file_pos_info & end   = (*i)->ending_fpi;
          printf ("Token #%3d isComment = %s start = %d:%d end = %d:%d token code = %d token = %s \n",
               count,isComment ? "true " : "false",start.line_num,start.column_num,end.line_num,end.column_num,token->token_id,token->token_lexeme.c_str());
#endif
          if (isComment == true)
             {
            // PreprocessingInfo(DirectiveType, const std::string & inputString, const std::string & filenameString, 
            //      int line_no , int col_no, int nol, RelativePositionType relPos, bool copiedFlag, bool unparsedFlag);

               int numberOfLines = 1;
               bool copiedFlag   = false;
               bool unparsedFlag = false;
            // PreprocessingInfo* comment = new PreprocessingInfo(PreprocessingInfo::FortranStyleComment,token->token_lexeme,file->get_sourceFileNameWithPath(),
            //                                                    start.line_num,start.column_num,numberOfLines,PreprocessingInfo::before,copiedFlag,unparsedFlag);
               PreprocessingInfo* comment = new PreprocessingInfo(PreprocessingInfo::FortranStyleComment,token->token_lexeme,filename,
                                                                  start.line_num,start.column_num,numberOfLines,PreprocessingInfo::before,copiedFlag,unparsedFlag);
               ROSE_ASSERT(comment != NULL);
               attributeList.push_back(comment);
             }

          i++;
          count++;
        }

  // printf ("attributeList.size() = %zu \n",attributeList.size());
   }


void
ROSEAttributesList::collectFixedFormatPreprocessorDirectivesAndCommentsForAST( const string & filename )
   {
  // The lex pass for fixed-format Fortran ignores comments and does not get CPP directives correct.
  // So maybe we should just extract them seperately in an other pass over the file.  Fixed format
  // comments in Fortran are supposed to be easy to extract.

     ROSE_ASSERT (filename.empty() == false);

  // Open file for reading line by line!
     string line;

  // printf ("In ROSEAttributesList::collectFixedFormatPreprocessorDirectivesAndCommentsForAST: Opening file %s for reading comments and CPP directives \n",filename.c_str());

     ifstream fixedFormatFile (filename.c_str());
     if (fixedFormatFile.is_open())
        {
       // The first line is defined to be line 1, line zero does not exist  and is an error value.
       // This synch's the line numbering convention of the OFP with the line numbering convention 
       // for CPP directives and comments.
          int lineCounter = 1;
          while ( fixedFormatFile.eof() == false )
             {
               getline (fixedFormatFile,line);
            // cout << "collect comments: " << line << endl;

            // Handle comments first, Fortran fixed format comments should be easy.
            // if there is a character in the first column, then the whole line is a comment.
            // Also, more subtle, if it is a blank line then it is a comment, so save the blank lines too.

               bool isComment = false;

               char firstCharacter = line[0];
               if (firstCharacter != ' ' && firstCharacter != '\n' && firstCharacter != '\0' )
                  {
                 // This has something in the first colum, it might be a comment!

                 // Error checking on first character
                 // printf ("firstCharacter = %d line.length() = %zu \n",(int)firstCharacter,line.length());
                    ROSE_ASSERT(firstCharacter >= ' ' && firstCharacter < 126);
#if 1
                 // Make sure it is not part a number (which could be part of a label)
                    if (firstCharacter >= '0' && firstCharacter <= '9')
                       {
                      // This is NOT a comment it is part of a label in the first column (see test2008_03.f)
                      // printf ("This is not a comment, it is part of a label in the first column: line = %s \n",line.c_str());
                       }
                      else
                       {
                         isComment = true;
                       }
#else
                 // DQ (1/22/2008): Seperate from the F77 standard, no compiler is this restrictive!
                 // The Fortran 77 standard says: comments must have a C or * in the first column (check for case)
                    if (firstCharacter == 'C' || firstCharacter == 'c' || firstCharacter == '*')
                       {
                         isComment = true;
                       }
#endif
                 // printf ("This is a comment! lineCounter = %d \n",lineCounter);
                  }

               char firstNonBlankCharacter = line[0];
               size_t i = 0;
               size_t lineLength = line.length();
               while (i < lineLength && firstNonBlankCharacter == ' ')
                  {
                    firstNonBlankCharacter = line[i];
                    i++;
                  }

             // The character "!" starts a comment if only blanks are in the leading white space.
               if (firstNonBlankCharacter == '!')
                  {
                 // printf ("This is a F90 style comment: lineCounter = %d line = %s length = %zu \n",lineCounter,line.c_str(),line.length());
                    isComment = true;
                  }

               if (firstNonBlankCharacter == '\n' || firstNonBlankCharacter == '\0')
                  {
                 // This is a blank line, save it as a comment too!
                 // printf ("This is a blank line, save it as a comment too! lineCounter = %d line = %s length = %zu \n",lineCounter,line.c_str(),line.length());

                 // Need to reset this to "\n" to save it as a comment in ROSE.
                    line = "\n ";
                 // printf ("   after being reset: lineCounter = %d line = %s length = %zu \n",lineCounter,line.c_str(),line.length());

                    isComment = true;
                  }

               if (isComment == true)
                  {
                 // PreprocessingInfo(DirectiveType, const std::string & inputString, const std::string & filenameString, 
                 //      int line_no , int col_no, int nol, RelativePositionType relPos, bool copiedFlag, bool unparsedFlag);

                    int numberOfLines = 1;
                    bool copiedFlag   = false;
                    bool unparsedFlag = false;
                    PreprocessingInfo* comment = new PreprocessingInfo(PreprocessingInfo::FortranStyleComment,line,filename,
                                                                       lineCounter,0,numberOfLines,PreprocessingInfo::before,
                                                                       copiedFlag,unparsedFlag);
                    ROSE_ASSERT(comment != NULL);
                    attributeList.push_back(comment);
                  }

#if 0
            // Try to handle CPP directives, finish this later...
               size_t positonOfLineContinuation = rfind('&');
               bool hasLineContinuation = (positonOfLineContinuation != string::npos);
               size_t positonOfQuote = rfind('\'');
               bool hasQuote = (positonOfQuote != string::npos);
               if (hasQuote == false)
               {
                  if (hasLineContinuation == true)
                  {
                  }
               }
#endif

               lineCounter++;
             }

          fixedFormatFile.close();
        }
       else
        {
          cerr << "Unable to open fixed format Fortran file";
          ROSE_ASSERT(false);
        }
   }



//##############################################################################
//
// [DT] 3/15/2000 -- Begin member function definitions for 
//      ROSEAttributesListContainer.
//

ROSEAttributesListContainer::ROSEAttributesListContainer()
   {
  // Nothing to do here?
   }

ROSEAttributesListContainer::~ROSEAttributesListContainer()
   {
  // Nothing to do here?
   }

  void
ROSEAttributesListContainer::addList ( std::string fileName, ROSEAttributesList* listPointer )
   {
  // attributeListList.push_back ( listPointer );
     attributeListMap[fileName] = listPointer;
   }

#if 0
  ROSEAttributesList*
ROSEAttributesListContainer::operator[](int i)
   {
     return attributeListMap[i];
   }
#endif

  ROSEAttributesList &
ROSEAttributesListContainer::operator[] ( const string & fName )
   {
  // return *(findList (fName.c_str()) );
  // return *( findList (fName) );
     return *( attributeListMap[fName] );
   }

  bool
ROSEAttributesListContainer::isInList ( const string & fName )
   {
     bool returnValue = false;

  // printf ("Looking for list for file = %s attributeListList.size() = %d \n",fName,attributeListList.size());

  // Trap out this special case (generated by SAGE/EDG code)
  // if ( ROSE::containsString (fName,"NULL_FILE") == true )
     if ( fName == "NULL_FILE" )
          return false;

#if 0
  // ROSE_ASSERT ( ROSE::containsString (fName,"NULL_FILE") == false );
     ROSE_ASSERT ( fName != "NULL_FILE" );

     vector<ROSEAttributesList*>::iterator i = attributeListList.begin();
     for (i = attributeListList.begin(); i != attributeListList.end(); i++)
        {
#if 0
          printf ("In ROSEAttributesListContainer::isInList(%s): (*i)->getFileName() = %s fName = %s \n",
              fName.c_str(),(*i)->getFileName().c_str(),fName.c_str());
#endif
       // if (!strcmp( (*i)->getFileName() , fName ) )
          if ( (*i)->getFileName() == fName )
               returnValue = TRUE;
        }
#else
     returnValue = (attributeListMap.find(fName) != attributeListMap.end());
#endif

  // printf ("In ROSEAttributesListContainer::isInList(): returnValue = %d \n",returnValue);

     return returnValue;
   }

#if 0
  ROSEAttributesList *
ROSEAttributesListContainer::findList ( const string & fName )
   {
  //
  // Return the attribute list for file fName.
  //
  // display("Inside of ROSEAttributesListContainer::findList()");
     ROSEAttributesList* returnList = NULL;

  // printf ("Inside of ROSEAttributesListContainer::findList (%s): attributeListList.size() = %d \n",fName,attributeListList.size());

     vector<ROSEAttributesList*>::iterator i = attributeListList.begin();


  // This would be a while loop instead (but I will leave it for now)
     for (i = attributeListList.begin(); i != attributeListList.end(); i++)
        {
#if 0
          printf ("In ROSEAttributesListContainer::findList(%s): (*i)->getFileName() = %s fName = %s \n",
              fName,(*i)->getFileName(),fName);
#endif
       // if (!strcmp( (*i)->getFileName() , fName ) )
          if ( (*i)->getFileName() == fName )
             {
            // printf ("Found the correct list of directives fName = %s \n",fName.c_str());
               returnList = *i;
             }
          else
             {
            // printf ("NOT FOUND: searching for the list of directives fName = %s \n",fName.c_str());
             }
        }

     ROSE_ASSERT (returnList != NULL);

#if 0
  // Return NULL if there was no match.
     if( i != attributeListList.end() )
        {
          printf ("In ROSEAttributesListContainer::findList(): But this is an empty list so return NULL! \n");
          return NULL;
        }
#endif

     return returnList;
   }
#endif

#if 0
  int
ROSEAttributesListContainer::getLength(void)
   {
     return attributeListMap.size();
   }
#endif

#if 0
  int
ROSEAttributesListContainer::size(void)
   {
     return attributeListMap.size();
   }
#endif

  void
ROSEAttributesListContainer::dumpContents(void)
   {
     FILE *outFile               = NULL;

  // This should be based on the specificed output file name
     outFile = fopen("rose_directives_list.txt","w");

#if 0
     vector<ROSEAttributesList*>::iterator i = attributeListList.begin();
     for (i = attributeListList.begin(); i != attributeListList.end(); i++)
        {
          fprintf(outFile,"\n%s: \n", (*i)->getFileName().c_str() );
          vector<PreprocessingInfo*>::iterator j = (*i)->getList().begin();
          for (j = (*i)->getList().begin(); j != (*i)->getList().end(); j++)
             {
               ROSE_ASSERT ( (*j) != NULL );
               fprintf(outFile,"LineNumber: %5d: %s\n",(*j)->getLineNumber(),(*j)->getString().c_str());
             }
        }
#else
     printf ("ROSEAttributesListContainer::dumpContents() not implemented for new map datastructure \n");
     ROSE_ASSERT(false);
#endif

     fclose(outFile);
   }

  void
ROSEAttributesListContainer::display ( const string & label )
   {
     printf ("ROSEAttributesListContainer::display (label = %s) \n",label.c_str());
#if 0
     vector<ROSEAttributesList*>::iterator i = attributeListList.begin();
     for (i = attributeListList.begin(); i != attributeListList.end(); i++)
        {
          printf("\n%s: \n", (*i)->getFileName().c_str());
          vector<PreprocessingInfo*>::iterator j = (*i)->getList().begin();
          for (j = (*i)->getList().begin(); j != (*i)->getList().end(); j++)
             {
               ROSE_ASSERT ( (*j) != NULL );
               printf("LineNumber: %5d: %s\n",(*j)->getLineNumber(),(*j)->getString().c_str());
             }
        }
#else
  // std::map<std::string, ROSEAttributesList*> attributeListMap;

     printf ("In ROSEAttributesListContainer::display(): attributeListMap.size() = %zu \n",attributeListMap.size());
     map<std::string, ROSEAttributesList*>::iterator i = attributeListMap.begin();
     while (i != attributeListMap.end())
        {
          string filename = i->first;
          ROSEAttributesList* attributeList = i->second;

          printf ("filename = %s \n",filename.c_str());
          ROSE_ASSERT(attributeList != NULL);

          i++;
        }

  // printf ("ROSEAttributesListContainer::display() not implemented for new map datastructure \n");
  // ROSE_ASSERT(false);
#endif
   }

  void
ROSEAttributesListContainer::deepClean(void)
   {
  //
  // Call deepClean for each list.
  //

  // Nothing to do?
   }

  void
ROSEAttributesListContainer::clean(void)
   {
  //
  // Call clean for each list.
  //

  // Nothing to do?
   }

// EOF
