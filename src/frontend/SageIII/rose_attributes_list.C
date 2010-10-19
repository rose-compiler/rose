#include "sage3basic.h"
#include "errno.h"
#include "rose_attributes_list.h"
#include "stringify.h"

// DQ (11/28/2009): I think this is equivalent to "USE_ROSE"
// #if CAN_NOT_COMPILE_WITH_ROSE != true
// #if (CAN_NOT_COMPILE_WITH_ROSE == 0)
// #ifndef USE_ROSE

///////////////////////////////////////////////////////////////////////////////
//  Include the token class from Wave
///////////////////////////////////////////////////////////////////////////////

//  Include Wave itself
#include <boost/wave.hpp>

token_container wave_tokenStream;


// #include <boost/wave/grammars/cpp_xpression_grammar.hpp> //as_string

// #endif 


//AS(01/04/07) Global map of filenames to PreprocessingInfo*'s as it is inefficient
//to get this by a traversal of the AST
std::map<std::string,ROSEAttributesList* > mapFilenameToAttributes;



// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;


// JH (01/03/2006) methods for packing the PreprocessingInfo data, in order to store it into
// a file and rebuild it!
unsigned int PreprocessingInfo::packed_size () const
   {
  // This function computes the size of the packed representation of this classes data members.

     ROSE_ASSERT(this != NULL);

     unsigned int packedSize = sizeof (file_info) +
  /* string size and string */ sizeof (unsigned int) + internalString.size() +
                               sizeof (numberOfLines) +
                               sizeof (whatSortOfDirective) +
                               sizeof (relativePosition) +
                               sizeof (lineNumberForCompilerGeneratedLinemarker) +
  /* string size and string */ sizeof (unsigned int) + filenameForCompilerGeneratedLinemarker.size() +
  /* string size and string */ sizeof (unsigned int) + optionalflagsForCompilerGeneratedLinemarker.size();

  // This is part of Wave support in ROSE.
// #ifndef USE_ROSE
  // Add in the four pointers required for the Wave support.
  // Until we add the support to save all the Wave data into 
  // the AST file we would have to reprocess the relevant 
  // file to store this.
     packedSize += sizeof (tokenStream) +
                   sizeof (macroDef) +
                   sizeof (macroCall) +
                   sizeof (includeDirective);
// #endif

  // Debugging information.  What can we assert about the packedSize vs. the sizeof(PreprocessingInfo)?
  // If there is anything, then it might make for a simple test here.  However, there does not appear to
  // be any relationship since the sizeof(PreprocessingInfo) does not account for the sizes of internal 
  // strings used.
  // printf ("In PreprocessingInfo::packed_size(): packedSize = %u sizeof(PreprocessingInfo) = %zu \n",packedSize,sizeof(PreprocessingInfo));

  // I think that because we have to save additional information the packedSize will 
  // be a little larger than the sizeof(PreprocessingInfo).  So assert this as a test.
  // Unfortunately it is not always true!
  // ROSE_ASSERT(packedSize >= sizeof(PreprocessingInfo));

     return packedSize;
   }


// JH (01/03/2006) This pack methods might cause memory leaks. Think of deleting them after stored to file ...
char* PreprocessingInfo::packed()  const
   {
     ROSE_ASSERT(this != NULL);

  // printf ("Inside of PreprocessingInfo::packed() internalString = %s \n",internalString.c_str());

     const char* saveString  = internalString.c_str();
     unsigned int stringSize = internalString.size();

  // Wouldn't padding of data cause us to under compute the size of the buffer?
     char* returnData = new char[ packed_size() ];

     char* storePointer = returnData;

  // printf ("Error, need to get the info out of the SgFileInfo object! \n");
  // ROSE_ASSERT(false);

  // DQ (2/28/2010): We do want to write out the data value for this since it has been
  // converted to a global index value in the AST file I/O.
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

  // printf ("Inside of PreprocessingInfo::packed(): Note some Fortran specific data members are not packed yet (also all the Wave data is not packed). \n");

#if 0
     cout << "  packed data in PrerocessingInfo ... " << endl;
#endif

  // DQ (2/28/2010): Some assertion checking that will be done later in the unparser.
  // printf ("In PreprocessingInfo::packed(): getTypeOfDirective() = %d \n",getTypeOfDirective());
     ROSE_ASSERT (getTypeOfDirective() != PreprocessingInfo::CpreprocessorUnknownDeclaration);

     return returnData;
   }

// JH (01/03/2006) This unpack method wors conremplary to packed ...
void PreprocessingInfo::unpacked( char* storePointer )
   {
     ROSE_ASSERT(this != NULL);
#if 0
     printf ("Inside of PreprocessingInfo::unpacked() \n");
     printf ("Before overwriting memory: Calling display on unpacked Sg_File_Info object file_info = %p \n",file_info);
     file_info->display("In PreprocessingInfo::unpacked()");
     printf ("DONE: Before overwriting memory: Calling display on unpacked Sg_File_Info object \n");
#endif

  // std::cout << " in PreprocessingInfo::unpacked ... " << std::endl;
  // printf ("Error, need to build a new SgFileInfo object! \n");
  // ROSE_ASSERT(false);
#if 0
  // DQ (2/28/2010): This is a pointer to an IR node and they are handled using global index values 
  // that are mapped back to pointer values after reading. This will trash a properly set value!
     memcpy ( (char*)(&file_info), storePointer, sizeof(file_info) );
#endif

  // DQ (2/28/2010): But jump over the file_info data member so that all ther other data members will be unpacked properly.
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
     cout << " getting in trouble at String ... " << endl;
#endif
     internalString = string ( storePointer, stringSize );
#if 0
     cout << " but survived " << endl;
#endif

#if 0
     printf ("In PreprocessingInfo::unpacked(%p) internalString = %s \n",storePointer,internalString.c_str());
     printf ("Calling display on unpacked Sg_File_Info object file_info = %p \n",file_info);
     file_info->display("In PreprocessingInfo::unpacked()");
     printf ("DONE: Calling display on unpacked Sg_File_Info object \n");
#endif

// DQ (11/29/2009): MSVC does not understnad use of "true" in macros.
// #if CAN_NOT_COMPILE_WITH_ROSE != true
// #if (CAN_NOT_COMPILE_WITH_ROSE == 0)
// #ifndef USE_ROSE
  // DQ and AS (6/23/2006): and the stuff of Wave specific macro support ...
     tokenStream      = NULL;
     macroDef         = NULL;
     macroCall        = NULL;
     includeDirective = NULL;
// #endif     

  // DQ (2/28/2010): Some assertion checking that will be done later in the unparser.
  // This test helps debug if any of the data members are set at an offset to there 
  // proper positions.
  // printf ("In PreprocessingInfo::unpacked(): getTypeOfDirective() = %d \n",getTypeOfDirective());
     ROSE_ASSERT (getTypeOfDirective() != PreprocessingInfo::CpreprocessorUnknownDeclaration);
   }


// ********************************************
// Member functions for class PreprocessingInfo
// ********************************************

// DQ (11/29/2009): MSVC does not understnad use of "true" in macros.
// #if CAN_NOT_COMPILE_WITH_ROSE != true
// #if (CAN_NOT_COMPILE_WITH_ROSE == 0)
// #ifndef USE_ROSE
// AS(012006) Added to support macros
PreprocessingInfo::rose_macro_call*
PreprocessingInfo::get_macro_call()
   { 
     return macroCall;
   } 

// AS(012006) Added to support macros
PreprocessingInfo::rose_macro_definition*
PreprocessingInfo::get_macro_def()
   {
     return macroDef;
   } 

//AS(060706) Added support for include directive
PreprocessingInfo::rose_include_directive*
PreprocessingInfo::get_include_directive()
   {
     return includeDirective;
   } 

const token_container*
PreprocessingInfo::get_token_stream()
   {
     return tokenStream;
   } 

void PreprocessingInfo::push_back_token_stream(token_type tok)
   {
     tokenStream->push_back(tok);

     internalString = string(boost::wave::util::impl::as_string(*tokenStream).c_str()) ;

   } 

void PreprocessingInfo::push_front_token_stream(token_type tok)
   {
     tokenStream->insert(tokenStream->begin(),tok);

     internalString = string(boost::wave::util::impl::as_string(*tokenStream).c_str());

  }


// AS(012006) Added to support macros
PreprocessingInfo::PreprocessingInfo(token_container tokCont, DirectiveType typeOfDirective, RelativePositionType relPos) 
   : whatSortOfDirective(typeOfDirective), relativePosition(relPos)
   {
  // ROSE_ASSERT(false);
  // implement the position information
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

// AS(012006) Added to support macros
PreprocessingInfo::PreprocessingInfo(rose_macro_call* mcall, RelativePositionType relPos) 
// DQ (2/28/2010): Removed preinitialization list to avoid compiler warnings about the order 
// of the initializations.  These are due to a reordering of the data members in the class
// so that we can get the AST File I/O working.
// : macroCall(mcall), relativePosition(relPos)
   {
  // DQ (2/28/2010): Removed preinitialization list and moved data member initialization to here.
     macroCall        = mcall;
     relativePosition = relPos;

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


     internalString = string(boost::wave::util::impl::as_string(*tokenStream).c_str());

   }

PreprocessingInfo::PreprocessingInfo(rose_macro_definition* mdef, RelativePositionType relPos) 
// DQ (2/28/2010): Removed preinitialization list to avoid compiler warnings about the order 
// of the initializations.  These are due to a reordering of the data members in the class
// so that we can get the AST File I/O working.
// : macroDef(mdef), relativePosition(relPos)
   {
  // DQ (2/28/2010): Removed preinitialization list and moved data member initialization to here.
     macroDef        = mdef;
     relativePosition = relPos;

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


     internalString = string("#define\t")+string(boost::wave::util::impl::as_string(*tokenStream).c_str());


	 if(SgProject::get_verbose() >= 1)
         std::cout << "Internal string is: " << internalString << std::endl;
  //     internalString = boost::wave::util::impl::as_string(tokenStream) ;
   }


PreprocessingInfo::PreprocessingInfo(rose_include_directive* inclDir, RelativePositionType relPos) 
// DQ (2/28/2010): Removed preinitialization list to avoid compiler warnings about the order 
// of the initializations.  These are due to a reordering of the data members in the class
// so that we can get the AST File I/O working.
// : includeDirective(inclDir), relativePosition(relPos)
   {
  // DQ (2/28/2010): Removed preinitialization list and moved data member initialization to here.
     includeDirective = inclDir;
     relativePosition = relPos;

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

// #endif

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
     int line_no, int col_no, int nol, RelativePositionType relPos
  // DQ (7/19/2008): Removed these: bool copiedFlag, bool unparsedFlag
   )
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

  // printf ("In PreprocessingInfo (constructor): dt = %d line_no = %d col_no = %d nol = %d s = %s \n",dt,line_no,col_no,nol,inputString.c_str());
     file_info = new Sg_File_Info(inputFileName,line_no,col_no);

  // DQ (12/23/2006): Mark this as a comment or directive (mostly so that we can know
  // that the parent being NULL is not meaningful in the AST consistancy tests).
     file_info->setCommentOrDirective();

  // DQ (3/7/2010): Switch this is a SgTypeDefault since one of these are referenced in the 
  // generated rose_edg_required_macros_and_functions.h which means that it will always be 
  // formally in the AST.  This may fix (or help fix) a bug in the AST file I/O where nodes
  // not properly connected to the AST don't appear to get there global index and freepointer 
  // set properly.
  // DQ (6/13/2007): Set the parent to a shared type for now so that it is at least set to a non-null value
  // This can if we like to used as a signature for Sg_File_Info nodes that are associated with comments and directives.
  // file_info->set_parent(file_info);
  // file_info->set_parent(SgTypeShort::get_builtin_type());
  // file_info->set_parent(SgTypeLongLong::createType());
     file_info->set_parent(SgTypeDefault::createType());

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
          case F90StyleComment:
             returnString = "F90StyleComment";
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

       // DQ (11/17/2008): Added support for #ident
          case CpreprocessorIdentDeclaration:
             returnString = "CpreprocessorIdentDeclaration";
             break;

       // DQ (11/17/2008): Added support for things like:  # 1 "<command line>"
          case CpreprocessorCompilerGeneratedLinemarker:
             returnString = "CpreprocessorCompilerGeneratedLinemarker";
             break;

          default:
             returnString = "ERROR DEFAULT REACHED";
             printf ("Default reached in PreprocessingInfo::directiveTypeName() exiting ... (directive = %d) \n",directive);
             ROSE_ASSERT(false);
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
    return stringifyPreprocessingInfoRelativePositionType(position);
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

// DQ (11/28/2008): Support for CPP generated linemarkers
int
PreprocessingInfo::get_lineNumberForCompilerGeneratedLinemarker()
   {
     return lineNumberForCompilerGeneratedLinemarker;
   }

std::string
PreprocessingInfo::get_filenameForCompilerGeneratedLinemarker()
   {
     return filenameForCompilerGeneratedLinemarker;
   }

std::string
PreprocessingInfo::get_optionalflagsForCompilerGeneratedLinemarker()
   {
     return optionalflagsForCompilerGeneratedLinemarker;
   }

// DQ (11/28/2008): Support for CPP generated linemarkers
void
PreprocessingInfo::set_lineNumberForCompilerGeneratedLinemarker( int x ) 
   {
     lineNumberForCompilerGeneratedLinemarker = x;
   }

void
PreprocessingInfo::set_filenameForCompilerGeneratedLinemarker( std::string x )
   {
     filenameForCompilerGeneratedLinemarker = x;
   }

void
PreprocessingInfo::set_optionalflagsForCompilerGeneratedLinemarker( std::string x )
   {
     optionalflagsForCompilerGeneratedLinemarker = x;
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
  // PreprocessingInfo *pElem = new PreprocessingInfo(dt, pLine, filename, lineNumber, columnNumber, numOfLines, PreprocessingInfo::undef, false, false);
     PreprocessingInfo *pElem = new PreprocessingInfo(dt, pLine, filename, lineNumber, columnNumber, numOfLines, PreprocessingInfo::undef);

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

       // DQ (12/19/2008): Modified to report NULL pointers
       // ROSE_ASSERT ( (*j) != NULL );
       // printf("LineNumber: %5d: %s\n",(*j)->getLineNumber(),(*j)->getString().c_str());
          printf("-----------------------\n"); 
          if ( *j != NULL )
               printf("Directive Type: %s; Relative position: %s; \nLine:%5d; Column:%5d; String: %s\n",
               PreprocessingInfo::directiveTypeName ((*j)->getTypeOfDirective ()).c_str (),
               PreprocessingInfo::relativePositionName((*j)->getRelativePosition()).c_str (),
               (*j)->getLineNumber(),
               (*j)->getColumnNumber(), 
               (*j)->getString().c_str());
            else
               printf ("Warning: PreprocessingInfo *j == NULL \n");
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
  // This function does not work for fixed-format, which is processed separately.
  // This function reads the token stream and extracts out the comments for inclusion into the attributeList.

     ROSE_ASSERT (this != NULL);
     ROSE_ASSERT (filename.empty() == false);

     printf ("This is an old version of the function to collect CPP directives and comments \n");
     ROSE_ASSERT(false);

     ROSE_ASSERT(rawTokenStream != NULL);

#if 1
  // DQ (11/16/2008): Added test.
     if (attributeList.empty() == false)
        {
       // Detect where these these have been previously built using a mechanism we are testing.
       // Delete the entries built by the expermiental mechanism and use the previous approach.
       // This allows for the new mechanism to be widely tested in C, C++, and Fortran.

          printf ("attributeList has already been build, remove the existing entries attributeList.size() = %zu \n",attributeList.size());
          std::vector<PreprocessingInfo*>::iterator i = attributeList.begin();
          while (i != attributeList.end())
             {
               delete *i;
               i++;
             }
          attributeList.clear();
        }
#endif
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
            // bool copiedFlag   = false;
            // bool unparsedFlag = false;
            // PreprocessingInfo* comment = new PreprocessingInfo(PreprocessingInfo::FortranStyleComment,token->token_lexeme,file->get_sourceFileNameWithPath(),
            //                                                    start.line_num,start.column_num,numberOfLines,PreprocessingInfo::before,copiedFlag,unparsedFlag);
            // PreprocessingInfo* comment = new PreprocessingInfo(PreprocessingInfo::FortranStyleComment,token->token_lexeme,filename,
            //                                                    start.line_num,start.column_num,numberOfLines,PreprocessingInfo::before,copiedFlag,unparsedFlag);
               PreprocessingInfo* comment = new PreprocessingInfo(PreprocessingInfo::FortranStyleComment,token->token_lexeme,filename,
                                                                  start.line_num,start.column_num,numberOfLines,PreprocessingInfo::before);
               ROSE_ASSERT(comment != NULL);
               attributeList.push_back(comment);

            // comment->display("In ROSEAttributesList::generatePreprocessorDirectivesAndCommentsForAST() \n");
             }

          i++;
          count++;
        }

  // printf ("attributeList.size() = %zu \n",attributeList.size());
   }


bool
ROSEAttributesList::isFortran90Comment( const string & line )
   {
  // This refactored code test if a line is a fortran comment.
  // Fortran 90 comments are more complex to recognise than
  // F77.  This function only recognizes F90 comments that have
  // a leading "!".  Other uses of "!" at the end of a valid
  // Fortran statement are not yet captured, but that would be
  // handled by this function (later).

     bool isComment = false;

     char firstNonBlankCharacter = line[0];
     size_t i = 0;
     size_t lineLength = line.length();

  // Loop over any leading blank spaces.
     while (i < lineLength && firstNonBlankCharacter == ' ')
        {
          firstNonBlankCharacter = line[i];
          i++;
        }

  // The character "!" starts a comment if only blanks are in the leading white space.
     if (firstNonBlankCharacter == '!')
        {
       // printf ("This is a F90 style comment: line = %s length = %zu \n",line.c_str(),line.length());
          isComment = true;
        }

  // return isFortran77Comment(line);
     return isComment;
   }

bool
ROSEAttributesList::isFortran77Comment( const string & line )
   {
  // This refactored code tests if a line is a fortran fixed format comment (it maybe that it is less specific to F77).
  // It is a very simple test on the character in column zero, but there are a few details...

#if 0
  // Debugging output
     cout << "collect comments: " << line << endl;
#endif

  // We handle CPP directives first and then comments, Fortran fixed format comments should be easy.
  // if there is a character in the first column, then the whole line is a comment.
  // Also, more subtle, if it is a blank line then it is a comment, so save the blank lines too.

     bool isComment = false;

     char firstCharacter = line[0];
     if (firstCharacter != ' '  /* SPACE */ && firstCharacter != '\n' /* CR  */ && 
         firstCharacter != '\0' /* NUL   */ && firstCharacter != '\t' /* TAB */)
        {
       // This has something in the first column, so it might be a comment (check further)...

#if 1
       // Error checking on first character, I believe we can't enforce this, but I would like to have it be a warning.
          if (!(firstCharacter >= ' ') || !(firstCharacter < 126))
             {
               printf ("Warning: firstCharacter = %d (not an acceptable character value for Fortran) line.length() = %zu \n",(int)firstCharacter,line.length());
             }
#endif

       // Error checking on first character
       // DQ (5/15/2008): The filter is in the conditional above and is not required to be repeated.
       // ROSE_ASSERT(firstCharacter >= ' ' && firstCharacter < 126);

#define RELAXED_FORTRAN_COMMENT_SPECIFICATION 1
#if RELAXED_FORTRAN_COMMENT_SPECIFICATION
       // Most fortran compilers do not enforce the strinct langauge definition of what a comment is 
       // so we have to handle the more relaxed comment specification (which does not appear to be 
       // written down anywhere).
       // Make sure it is not part a number (which could be part of a Fortran label)
          if (firstCharacter >= '0' && firstCharacter <= '9')
             {
            // This is NOT a comment it is part of a label in the first column (see test2008_03.f)
            // Some compilers (gfortran) can interprete a lable even if it starts in the first 
            // column (column 1 (fortran perspective) column 0 (C perspective)).
            // printf ("This is not a comment, it is part of a label in the first column: line = %s \n",line.c_str());
             }
            else
             {
            // DQ (11/19/2008): Commented this out since I can't understand 
            // why it was here and it appears to mark everything as a comment!

            // This is position (column) 0 in the line, for F77 this means it is a comment.
            // Note that we check for CPP directives first and only then if the line is not 
            // a CPP directive do we test for a F77 style comment, so if the first character 
            // of the line is a '#' then it will only be considered a comment if it is not a CPP directive.
               isComment = true;
             }
#else
       // DQ (1/22/2008): Separate from the F77 standard, no compiler is this restrictive (unfortunately)!
       // The Fortran 77 standard says: comments must have a C or * in the first column (check for case)
          if (firstCharacter == 'C' || firstCharacter == 'c' || firstCharacter == '*')
             {
               isComment = true;
             }
#endif
       // printf ("This is a comment! lineCounter = %d \n",lineCounter);
        }

#if 0
  // DQ (11/23/2008): This is part of the collection of blank lines in Fortran code, as comments.
  // I have turned this off for now.  We can include blank lines later or perhaps explicitly marked 
  // as blank lines (and stored in the AST just like comments).
     char firstNonBlankCharacter = line[0];
     size_t i = 0;
     size_t lineLength = line.length();
     while (i < lineLength && firstNonBlankCharacter == ' ')
        {
          firstNonBlankCharacter = line[i];
          i++;
        }

  // We want this function to be side-effect free.
     if (firstNonBlankCharacter == '\n' || firstNonBlankCharacter == '\0')
        {
       // This is a blank line, save it as a comment too!
       // printf ("This is a blank line, save it as a comment too! lineCounter = %d line = %s length = %zu \n",lineCounter,line.c_str(),line.length());

       // Need to reset this to "\n" to save it as a comment in ROSE.
          line = "\n ";
       // printf ("   after being reset: lineCounter = %d line = %s length = %zu \n",lineCounter,line.c_str(),line.length());

          isComment = true;
        }
#endif

#if 0
  // Debugging output!
     if (isComment == true)
        {
          printf ("This is an F77 (fixed format) Fortran comment: line = %s \n",line.c_str());
        }
#endif

     return isComment;
   }

#if 0
void
ROSEAttributesList::collectFixedFormatPreprocessorDirectivesAndCommentsForAST( const string & filename )
   {
  // This function only collects comments, not CPP directives, it is being replaced by:
  // collectPreprocessorDirectivesAndCommentsForAST() (below).

  // The lex pass for fixed-format Fortran ignores comments and does not get CPP directives correct.
  // So maybe we should just extract them separately in an other pass over the file.  Fixed format
  // comments in Fortran are supposed to be easy to extract.

     ROSE_ASSERT(this != NULL);

#error "DEAD CODE"

     printf ("This is an old version of the function to collect CPP directives and comments \n");
     ROSE_ASSERT(false);

     ROSE_ASSERT (filename.empty() == false);

  // Open file for reading line by line!
     string line;

  // printf ("In ROSEAttributesList::collectFixedFormatPreprocessorDirectivesAndCommentsForAST: Opening file %s for reading comments and CPP directives \n",filename.c_str());

#error "DEAD CODE"

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

            // DQ (11/17/2008): Refactored the code.
               bool isComment = isFortran77Comment(line);

               if (isComment == true)
                  {
                 // PreprocessingInfo(DirectiveType, const std::string & inputString, const std::string & filenameString, 
                 //      int line_no , int col_no, int nol, RelativePositionType relPos, bool copiedFlag, bool unparsedFlag);

#error "DEAD CODE"

                    int numberOfLines = 1;
                 // bool copiedFlag   = false;
                 // bool unparsedFlag = false;
                 // PreprocessingInfo* comment = new PreprocessingInfo(PreprocessingInfo::FortranStyleComment,line,filename,
                 //                                                    lineCounter,0,numberOfLines,PreprocessingInfo::before, copiedFlag,unparsedFlag);
                    PreprocessingInfo* comment = new PreprocessingInfo(PreprocessingInfo::FortranStyleComment,line,filename,
                                                                       lineCounter,0,numberOfLines,PreprocessingInfo::before);
                    ROSE_ASSERT(comment != NULL);
                    attributeList.push_back(comment);
                  }

               lineCounter++;
             }

#error "DEAD CODE"

          fixedFormatFile.close();
        }
       else
        {
          cerr << "Unable to open fixed format Fortran file";
          ROSE_ASSERT(false);
        }
   }
#endif

#define DEBUG_CPP_DIRECTIVE_COLLECTION 0

bool
ROSEAttributesList::isCppDirective( const string & line, PreprocessingInfo::DirectiveType & cppDeclarationKind, std::string & restOfTheLine )
   {
  // This function tests if a string is a CPP directive (the first line of a CPP directive).

     bool cppDirective = false;
     bool isLikelyCppDirective = false;

     char firstNonBlankCharacter = line[0];
     size_t i = 0;
     size_t lineLength = line.length();

  // Loop through any initial white space.
     while (i < lineLength && firstNonBlankCharacter == ' ')
        {
          firstNonBlankCharacter = line[i];
          i++;
        }

  // The character "!" starts a comment if only blanks are in the leading white space.
     int positionofHashCharacter = -1;
     if (firstNonBlankCharacter == '#')
        {
#if DEBUG_CPP_DIRECTIVE_COLLECTION
       // printf ("This is a CPP directive: i = %d lineCounter = %d line = %s length = %zu \n",i,lineCounter,line.c_str(),line.length());
#endif
          isLikelyCppDirective = true;
          positionofHashCharacter = i;
        }

#if DEBUG_CPP_DIRECTIVE_COLLECTION
     printf ("i = %d positionofHashCharacter = %d \n",i,positionofHashCharacter);
#endif
     bool hasLineContinuation = false;

  // DQ (4/21/2009): Fixed possible buffer underflow...
  // char lastCharacter = line[lineLength-1];
     char lastCharacter = (lineLength > 0) ? line[lineLength-1] : '\0';
     if (lastCharacter == '\\')
        {
          hasLineContinuation = true;
        }

#if DEBUG_CPP_DIRECTIVE_COLLECTION
     printf ("hasLineContinuation = %s \n",hasLineContinuation ? "true" : "false");
#endif

  // int numberOfLines = 1;

     if (isLikelyCppDirective == true)
        {
       // PreprocessingInfo(DirectiveType, const std::string & inputString, const std::string & filenameString, 
       //      int line_no , int col_no, int nol, RelativePositionType relPos, bool copiedFlag, bool unparsedFlag);

       // firstNonBlankCharacter = ' ';
       // printf ("firstNonBlankCharacter = %c \n",firstNonBlankCharacter);
          bool spaceAfterHash = false;

       // DQ (12/16/2008): Added support fo tabs between "#" and the directive identifier.
       // Note that Fortran modes of CPP should not allow any whitespace here (at least for gfortran).
          while ((i < lineLength && (firstNonBlankCharacter == ' ' || firstNonBlankCharacter == '\t')) || firstNonBlankCharacter == '#')
             {
#if DEBUG_CPP_DIRECTIVE_COLLECTION
               printf ("Looping over # or white space between # and CPP directive i = %d \n",i);
#endif
               firstNonBlankCharacter = line[i];
               if (spaceAfterHash == false)
                    spaceAfterHash = (firstNonBlankCharacter == ' ');

               i++;
             }

          int positionOfFirstCharacterOfCppIdentifier = i-1;

#if DEBUG_CPP_DIRECTIVE_COLLECTION
          printf ("positionOfFirstCharacterOfCppIdentifier = %d spaceAfterHash = %s \n",positionOfFirstCharacterOfCppIdentifier,spaceAfterHash ? "true" : "false");
#endif
       // Need to back up one!
          i = positionOfFirstCharacterOfCppIdentifier;

          char nonBlankCharacter = line[positionOfFirstCharacterOfCppIdentifier];
          int positionOfLastCharacterOfCppIdentifier = positionOfFirstCharacterOfCppIdentifier;
       // while (i < lineLength && isLegalCharacterForCppIndentifier(nonBlankCharacter) == true))
          while (i <= lineLength && ( ((nonBlankCharacter >= 'a' && nonBlankCharacter <= 'z') == true) || (nonBlankCharacter >= '0' && nonBlankCharacter <= '9') == true))
             {
               nonBlankCharacter = line[i];
#if DEBUG_CPP_DIRECTIVE_COLLECTION
               printf ("In loop: i = %d lineLength = %d nonBlankCharacter = %c \n",i,lineLength,isprint(nonBlankCharacter) ? nonBlankCharacter : '.');
#endif
               i++;
             }

#if DEBUG_CPP_DIRECTIVE_COLLECTION
          printf ("i = %d \n",i);
#endif

       // Need to backup two (for example if this is the end of the line, as in "#endif")
          positionOfLastCharacterOfCppIdentifier = i-2;

#if DEBUG_CPP_DIRECTIVE_COLLECTION
          printf ("positionOfLastCharacterOfCppIdentifier = %d \n",positionOfLastCharacterOfCppIdentifier);
#endif
          int cppIdentifierLength = (positionOfLastCharacterOfCppIdentifier - positionOfFirstCharacterOfCppIdentifier) + 1;
          string cppIndentifier = line.substr(positionOfFirstCharacterOfCppIdentifier,cppIdentifierLength);

       // Some names will convert to integer values
          long integerValue = -1;
          if (spaceAfterHash == true)
             {
            // This is likely going to be a number but test2005_92.C demonstrates a case where this is not true.

            // printf ("firstNonBlankCharacter = %c \n",firstNonBlankCharacter);
            // ROSE_ASSERT(firstNonBlankCharacter == '\"');
#if 0
            // The atoi() function is not supposed to be used any more.
               integerValue = atoi(cppIndentifier.c_str());
#else
            // The modern way to handle conversion of string to integer value is to 
            // use strtol(), and not atoi().  But atoi() is simpler.
               const char* str = cppIndentifier.c_str();
               int size = strlen(str)+1;
               char* buffer = new char[size];

            // Make a copy of the pointer so that we can always delete the memory that was allocated.
               char* original_buffer = buffer;

            // We should initialize "buffer" to all Nul chars (this includes a null terminator and the end of the string).
               for (int j=0; j < size; j++)
                    buffer[j] = '\0';

            // strtol will put the string into buffer if str is not a number and 2nd parameter is not NULL.
               errno = 0;
            // integerValue = strtol(str,NULL,10);
               integerValue = strtol(str,&buffer,10);

            // Setting and checking errno does not appear to work for the detection of errors in the use of strtol
               if (errno != 0)
                  {
                    printf ("Using errno: This was not a valid string (errno = %d returned) \n",errno);
                  }

               bool isANumber = true;
               if (strcmp(str,buffer) == 0)
                  {
                 // printf ("Using strcmp(): This was not a valid string (buffer = %s returned) \n",buffer);
                    isANumber = false;
                  }
            // printf ("cppIndentifier = %s integerValue = %ld \n",cppIndentifier.c_str(),integerValue);
#if 1
            // Avoid memory leak!
               delete original_buffer;
               original_buffer = NULL;
               buffer = NULL;
#endif
            // This value will be a constant value used to identify a numerical value.
            // This value should be a macro defined in some centralized location.
               if (isANumber == true)
                  {
                    cppIndentifier = "numeric value";

                 // Allow the line number to be a part of the restOfTheLine so it can be processed separately.
                 // printf ("cppIdentifierLength = %d \n",cppIdentifierLength);
                 // printf ("Before being reset: positionOfLastCharacterOfCppIdentifier = %d \n",positionOfLastCharacterOfCppIdentifier);
                    positionOfLastCharacterOfCppIdentifier -= cppIdentifierLength;
                 // printf ("After being reset: positionOfLastCharacterOfCppIdentifier = %d \n",positionOfLastCharacterOfCppIdentifier);
                  }
                 else
                  {
                 // printf ("This is not a number: cppIndentifier = %s \n",cppIndentifier.c_str());
                  }
#endif
             }

#if DEBUG_CPP_DIRECTIVE_COLLECTION
          printf ("cppIdentifierLength = %d cppIndentifier = %s integerValue = %ld \n",cppIdentifierLength,cppIndentifier.c_str(),integerValue);
#endif

       // classify the CCP directive
          if (cppIndentifier == "include")
             {
               cppDeclarationKind = PreprocessingInfo::CpreprocessorIncludeDeclaration;
             }
         // Is it "includenext" or "include_next", we need more agressive tests!
            else if (cppIndentifier == "includenext")
             {
               cppDeclarationKind = PreprocessingInfo::CpreprocessorIncludeNextDeclaration;
             }
            else if (cppIndentifier == "define")
             {
               cppDeclarationKind = PreprocessingInfo::CpreprocessorDefineDeclaration;
             }
            else if (cppIndentifier == "undef")
             {
               cppDeclarationKind = PreprocessingInfo::CpreprocessorUndefDeclaration;
             }
            else if (cppIndentifier == "ifdef")
             {
               cppDeclarationKind = PreprocessingInfo::CpreprocessorIfdefDeclaration;
             }
            else if (cppIndentifier == "ifndef")
             {
               cppDeclarationKind = PreprocessingInfo::CpreprocessorIfndefDeclaration;
             }
            else if (cppIndentifier == "if")
             {
               cppDeclarationKind = PreprocessingInfo::CpreprocessorIfDeclaration;
             }
            else if (cppIndentifier == "else")
             {
               cppDeclarationKind = PreprocessingInfo::CpreprocessorElseDeclaration;
             }
            else if (cppIndentifier == "elif")
             {
               cppDeclarationKind = PreprocessingInfo::CpreprocessorElifDeclaration;
             }
            else if (cppIndentifier == "endif")
             {
               cppDeclarationKind = PreprocessingInfo::CpreprocessorEndifDeclaration;
             }
            else if (cppIndentifier == "line")
             {
               cppDeclarationKind = PreprocessingInfo::CpreprocessorLineDeclaration;
             }
            else if (cppIndentifier == "error")
             {
               cppDeclarationKind = PreprocessingInfo::CpreprocessorErrorDeclaration;
             }
            else if (cppIndentifier == "warning")
             {
               cppDeclarationKind = PreprocessingInfo::CpreprocessorWarningDeclaration;
             }
            else if (cppIndentifier == "pragma")
             {
            // Ignore case of #pragma, since it is not a CPP directive and is handled by the C language definition only.
               cppDeclarationKind = PreprocessingInfo::CpreprocessorUnknownDeclaration;
             }
            else if (cppIndentifier == "ident")
             {
            // Ignore case of #ident
               cppDeclarationKind = PreprocessingInfo::CpreprocessorIdentDeclaration;
             }
         // Recognize the case of a numeric value...set if there was white space following the '#' and then a numeric (integer) value.
            else if (cppIndentifier == "numeric value")
             {
            // DQ (11/17/2008): This handles the case CPP declarations
            // such as: "# 1 "test2008_05.F90"", "# 1 "<built-in>"", 
            // "# 1 "<command line>"" "# 1 "test2008_05.F90""
               cppDeclarationKind = PreprocessingInfo::CpreprocessorCompilerGeneratedLinemarker;
             }
            else
             {
            // This case should be an error...
            // Liao, 5/13/2009
            // This should not be an error. Any weird string can show up in a block of /* */
            // Check the test input: tests/CompileTests/C_tests/test2009_01.c
#if 0             
               printf ("Error: Unknown cppIndentifier = %s \n",cppIndentifier.c_str());
               ROSE_ASSERT(false);
               cppDeclarationKind = PreprocessingInfo::CpreprocessorUnknownDeclaration;
#else
               printf ("Warning: ROSEAttributesList::isCppDirective() found unknown cppIndentifier = %s\n Extracted from a line: =%s\n",
                        cppIndentifier.c_str(), line.c_str());
               cppDeclarationKind = PreprocessingInfo::CpreprocessorUnknownDeclaration;
#endif               
             }

       // Collect the rest of the line: (line length - next character position) + 1.
          int restOfTheLineLength = (lineLength - (positionOfLastCharacterOfCppIdentifier+1)) + 1;
          restOfTheLine = line.substr(positionOfLastCharacterOfCppIdentifier+1,restOfTheLineLength);

#if 0
       // Debug output...
          printf ("cppDeclarationKind = %s restOfTheLine = %s \n",PreprocessingInfo::directiveTypeName(cppDeclarationKind).c_str(),restOfTheLine.c_str());
#endif

       // Set the return value
          if (cppDeclarationKind != PreprocessingInfo::CpreprocessorUnknownDeclaration)
             {
               cppDirective = true;
             }
        }

     return cppDirective;
   }

void
ROSEAttributesList::collectPreprocessorDirectivesAndCommentsForAST( const string & filename, ROSEAttributesList::languageTypeEnum languageType )
   {
  // This is required for Fortran, but is redundant for C and C++.
  // This is a more direct approach to collecting the CPP directives, where as for C and C++
  // we have had a solution (using lex) and a second (superior) solution using wave, the
  // Fortran support for CPP is not addressed properly by the existing lex approach (and Wave
  // does not work on Fortran).  Thus we have implemented a more direct collection of CPP
  // directives to support the requirements of Fortran CPP handling (such files have a suffix
  // such as: "F", "F90", "F95", "F03", "F08".

  // The lex pass for free-format Fortran collects comments properly, but does not classify CPP directives properly.
  // So maybe we should just extract them separately in an other pass over the file.  Also if we separate out 
  // the recognition of CPP directives from comments this function may be useful for the fix format CPP case.
  // CPP directives should also be easier than a lot of other token recognition. 

  // printf ("This function ROSEAttributesList::collectFreeFormatPreprocessorDirectivesAndCommentsForAS(): is not implemented yet! \n");
  // ROSE_ASSERT(false);

     ROSE_ASSERT (this != NULL);

     ROSE_ASSERT (filename.empty() == false);

  // Open file for reading line by line!
     string line;

#if DEBUG_CPP_DIRECTIVE_COLLECTION
     printf ("In ROSEAttributesList::collectPreprocessorDirectivesAndCommentsForAST: Opening file %s for reading comments and CPP directives \n",filename.c_str());
#endif

     ifstream targetFile (filename.c_str());
     if (targetFile.is_open())
        {
       // The first line is defined to be line 1, line zero does not exist  and is an error value.
       // This synch's the line numbering convention of the OFP with the line numbering convention 
       // for CPP directives and comments.
          int lineCounter = 1;
          while ( targetFile.eof() == false )
             {
#if DEBUG_CPP_DIRECTIVE_COLLECTION
               printf ("At top of loop over lines in the file ... lineCounter = %d \n",lineCounter);
#endif
               getline (targetFile,line);

#if DEBUG_CPP_DIRECTIVE_COLLECTION
            // Debugging output
               cout << "collect CPP directives: " << line << endl;
#endif
               int numberOfLines = 1;

               string restOfTheLine;
               PreprocessingInfo::DirectiveType cppDeclarationKind = PreprocessingInfo::CpreprocessorUnknownDeclaration;
               bool cppDirective = isCppDirective(line,cppDeclarationKind,restOfTheLine);

#if DEBUG_CPP_DIRECTIVE_COLLECTION
            // printf ("cppDirective = %s \n",cppDirective ? "true" : "false");
               printf ("cppDirective = %s cppDeclarationKind = %s \n",cppDirective ? "true" : "false",PreprocessingInfo::directiveTypeName(cppDeclarationKind).c_str());
#endif

               if (cppDirective == true)
                  {
#if 0
                    printf ("line.length() = %zu line = %s \n",line.length(),line.c_str());
                    printf ("line[line.length()-1] = %c \n",line[line.length()-1]);
#endif
                    if (line[line.length()-1] == '\\')
                       {
#if 0
                         printf ("Found line continuation: line = %s \n",line.c_str());
#endif
                         string nextLine;
                         while (line[line.length()-1] == '\\')
                            {
                              getline(targetFile,nextLine);

                           // Add linefeed to force nextLine onto the next line when output.
                              line += "\n" + nextLine;
                            }
                       }

                 // printf ("After processing continuation lines: line.length() = %zu line = %s \n",line.length(),line.c_str());
                  }

#if 1
            // DQ (11/17/2008): Refactored the code to make it simpler to add here!
            // If this is not a CPP directive, then check if it is a comment (note 
            // that for Fortran (for fixed format), a CPP directive could be identified 
            // as a comment so we have to check for CPP directives first.
               if (cppDirective == false)
                  {
                    bool isComment = false;

                 // Used switch to provide room for PHP, and pernaps C, C++ if we wanted
                 // to handle then this way.  Note that C permits multiple comments on a 
                 // single line, this is not addressed here.
                    switch (languageType)
                       {
                      // case e_Cxx_language: /* C and C++ cases are already handled via the lex based pass. */

                      // For C and C++ ignore the collection of comments for now (this function is defined 
                      // for Fortran but since C and C++ code is great for testing the CPP we allow it to 
                      // be used for testing CPP on C and C++ code, but we ignore comments for this case.
                         case e_C_language:   isComment = false; break;
                         case e_Cxx_language: isComment = false; break;

                         case e_Fortran77_language: isComment = isFortran77Comment(line); break;

                         case e_Fortran9x_language: isComment = isFortran90Comment(line); break;

                         default:
                            {
                              printf ("Error: default in switch over languageType = %d \n",languageType);
                              ROSE_ASSERT(false);
                            }
                       }

                 // bool isComment = isFortran90Comment(line);
                    if (isComment == true)
                       {
                      // printf ("This is a comment, set to PreprocessingInfo::FortranStyleComment \n");
                         cppDeclarationKind = PreprocessingInfo::FortranStyleComment;
                       }
                  }
#endif

#if 0
            // printf ("Before lineCounter = %d \n",lineCounter);
               printf ("cppDeclarationKind = %s \n",PreprocessingInfo::directiveTypeName(cppDeclarationKind).c_str());
#endif

            // Note that #pragma maps to CpreprocessorUnknownDeclaration so ignore that case!
               if (cppDeclarationKind != PreprocessingInfo::CpreprocessorUnknownDeclaration)
                  {
                    PreprocessingInfo* cppDirective = new PreprocessingInfo(cppDeclarationKind,line,filename,
                                                                            lineCounter,0,numberOfLines,PreprocessingInfo::before);
                    ROSE_ASSERT(cppDirective != NULL);
                    attributeList.push_back(cppDirective);

                 // DQ (11/28/2008): Gather additional data for specific directives (CPP generated linemarkers (e.g. "# <line number> <filename> <flags>").
                    if (cppDeclarationKind == PreprocessingInfo::CpreprocessorCompilerGeneratedLinemarker)
                       {
                      // Gather the line number, filename, and any optional flags.
                      // printf ("\nProcessing a CpreprocessorCompilerGeneratedLinemarker: restOfTheLine = %s \n",restOfTheLine.c_str());

                      // The IR node has not been build yet, we have to save the required information into the PreprocessingInfo object.
                      // SgLinemarkerDirectiveStatement* linemarkerDirective = isSgLinemarkerDirectiveStatement(cppDirective);
                      // ROSE_ASSERT(linemarkerDirective != NULL);

                         size_t i = 0;
                         size_t positionOfFirstCharacterOfIntegerValue = 0;
                         size_t lineLength = restOfTheLine.length();
                         char nonBlankCharacter = restOfTheLine[0];
                         while (i <= lineLength && (nonBlankCharacter >= '0' && nonBlankCharacter <= '9') == true)
                                 {
                                   nonBlankCharacter = restOfTheLine[i];
#if 0
                                   printf ("In loop: i = %d lineLength = %d nonBlankCharacter = %c \n",i,lineLength,isprint(nonBlankCharacter) ? nonBlankCharacter : '.');
#endif
                                   i++;
                                 }

#if 0
                         printf ("i = %d \n",i);
#endif

                      // Need to backup two (for example if this is the end of the line, as in "#endif")
                         size_t positionOfLastCharacterOfIntegerValue = i-2;

#if 0
                         printf ("positionOfLastCharacterOfIntegerValue = %d \n",positionOfLastCharacterOfIntegerValue);
#endif
                         int lineNumberLength = (positionOfLastCharacterOfIntegerValue - positionOfFirstCharacterOfIntegerValue) + 1;
                         string cppIndentifier = restOfTheLine.substr(positionOfFirstCharacterOfIntegerValue,lineNumberLength);

                      // Some names will convert to integer values
                         long integerValue = -1;

                      // printf ("firstNonBlankCharacter = %c \n",firstNonBlankCharacter);
                      // ROSE_ASSERT(firstNonBlankCharacter == '\"');
#if 0
                      // The atoi() function is not supposed to be used any more.
                         integerValue = atoi(cppIndentifier.c_str());
#else
                      // The modern way to handle conversion of string to integer value is to 
                      // use strtol(), and not atoi().  But atoi() is simpler.
                         const char* str = cppIndentifier.c_str();

                      // strtol will put the string into buffer if str is not a number and 2nd parameter is not NULL.
                         integerValue = strtol(str,NULL,10);
#endif
#if 0
                         printf ("integerValue = %ld \n",integerValue);
#endif
                         cppDirective->set_lineNumberForCompilerGeneratedLinemarker(integerValue);

                         size_t remainingLineLength   = (lineLength - positionOfLastCharacterOfIntegerValue) - 1;
                         string remainingLine = restOfTheLine.substr(positionOfLastCharacterOfIntegerValue+1,remainingLineLength);
#if 0
                         printf ("lineLength    = %d positionOfLastCharacterOfIntegerValue = %d \n",lineLength,positionOfLastCharacterOfIntegerValue);
                         printf ("remainingLineLength = %d remainingLine = %s \n",remainingLineLength,remainingLine.c_str());
#endif
                         size_t positionOfFirstQuote = remainingLine.find('"');

                         // Liao, 5/13/2009
                         // "#  1 2 3" can show up in a comment block /* */, 
                         // In this case it is not a CPP generated linemarker at all.
                         // We should allow to skip this line as tested in tests/CompileTests/C_tests/test2009_02.c
#if 0                     
                         ROSE_ASSERT(positionOfFirstQuote != string::npos);
#else                         
                         if (positionOfFirstQuote == string::npos ) 
                         {
                           //rollback and skip to the next line
                           delete cppDirective;
                           continue;
                         }
#endif                           

                         size_t positionOfLastQuote = remainingLine.rfind('"');
                         ROSE_ASSERT(positionOfLastQuote != string::npos);
#if 0
                         printf ("positionOfFirstQuote = %zu positionOfLastQuote = %zu \n",positionOfFirstQuote,positionOfLastQuote);
#endif
                         int filenameLength = (positionOfLastQuote - positionOfFirstQuote) + 1;
#if 0
                         printf ("filenameLength = %zu \n",filenameLength);
#endif
                         string filename = remainingLine.substr(positionOfFirstQuote,filenameLength);

                         cppDirective->set_filenameForCompilerGeneratedLinemarker(filename);

                      // Add 1 to move past the last quote and 1 more to move beyond any white space.
                         string optionalFlags;
                         if (positionOfLastQuote+2 < remainingLineLength)
                            {
#if 0
                              printf ("Computing optional flags \n");
#endif
                              optionalFlags = remainingLine.substr(positionOfLastQuote+2);
#if 0
                              printf ("Computing optional flags: optionalFlags = %s \n",optionalFlags.c_str());
#endif
                            }

                      // printf ("optionalFlags = %s \n",optionalFlags.c_str());
                         cppDirective->set_optionalflagsForCompilerGeneratedLinemarker(optionalFlags);
#if 0
                         printf ("cppDirective.lineNumberForCompilerGeneratedLinemarker    = %d \n",cppDirective->get_lineNumberForCompilerGeneratedLinemarker());
                         printf ("cppDirective.filenameForCompilerGeneratedLinemarker      = %s \n",cppDirective->get_filenameForCompilerGeneratedLinemarker().c_str());
                         printf ("cppDirective.optionalflagsForCompilerGeneratedLinemarker = %s \n",cppDirective->get_optionalflagsForCompilerGeneratedLinemarker().c_str());
#endif
#if 0
                         printf ("Exiting as part of debugging ... \n");
                         ROSE_ASSERT(false);
#endif
                       }
                  }

               lineCounter++;

            // printf ("increment lineCounter = %d \n",lineCounter);
             }

       // printf ("Closing file \n");
          targetFile.close();
        }
       else
        {
          cerr << "Warning: unable to open target source file: " << filename << "\n";
       // ROSE_ASSERT(false);
        }

  // printf ("Leaving collectPreprocessorDirectivesAndCommentsForAST() \n");
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
               returnValue = true;
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
