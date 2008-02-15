/* unparser.C
 * Contains the implementation of the constructors, destructor, formatting functions,
 * and fucntions that unparse directives.
 */

#ifdef HAVE_CONFIG_H
// This avoids requiring the user to use config.h and follows 
// the automake manual request that we use <> instead of ""
#include <config.h>
#endif

#include "rose.h"

// include "array_class_interface.h"
#include "unparser.h"
#include "unparser_fort.h"

extern ROSEAttributesList *getPreprocessorDirectives( char *fileName); // [DT] 3/16/2000

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
Unparser::Unparser( ostream* nos, char *fname, Unparser_Opt nopt, int nline,
                    UnparseFormatHelp *h, UnparseDelegate* r)
  : cur(nos, h), repl(r)
   {
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
     strcpy(currentOutputFileName, fname);

  // dir_list         = nlist;
  // dir_listList     = nlistList;
     opt              = nopt;

     // MK: If overload option is set TRUE, the keyword "operator" occurs in the output.
     // Usually, that's not what you want, but it can be used to avoid a current bug,
     // see file TODO_MK. The default is to set this flag to FALSE, see file
     // preprocessorSupport.C in the src directory
     // opt.set_overload_opt(TRUE);

     cur_index        = 0;
     line_to_unparse  = nline;
   }

//-----------------------------------------------------------------------------------
//  Unparser::~Unparser
//
//  Destructor
//-----------------------------------------------------------------------------------
Unparser::~Unparser()
   {}

UnparseFormat& Unparser::get_output_stream()
{
  return cur;
}


char*
UnparserBase::getFileName(SgNode* stmt)
   {
#if 0
     ROSE_ASSERT(stmt != NULL);

  // DQ: New assertion (10/21/2000)
     ROSE_ASSERT(stmt->get_file_info() != NULL);

     ROSE_ASSERT(stmt->get_file_info()->get_filename() != NULL);
     return stmt->get_file_info()->get_filename();
#endif

     char* returnString = NULL;

  // check to see if fileinfo is not available
     if (stmt->get_file_info() != NULL)
        {
          returnString = stmt->get_file_info()->get_filename();
        }
       else
        {
       // the fileinfo was not available, so check the stmt's parent for the fileinfo
          SgNode* p_stmt = stmt->get_parent();
          if (p_stmt != NULL)
               getFileName(p_stmt);
        }

     return returnString;

#if 0
  //
  // [DT] 3/9/2000 -- Simply return the currentFileName member.
  //
  //      3/10/2000 -- Moved this inside the else clause.  It was
  //      previously outside the if statement altogether.  That
  //      was resulting in !strcpy(getFileName(stmt),getFileName)
  //      and similar checks always evaluating to TRUE, and that
  //      causing problems.  I'm not sure if this change here is
  //      going to be sufficient to fix the problem
  //      entirely.
  // 
  //      3/13/2000 -- Moved this again.  Outside the else clause
  //      and *after* the whole if-else statement.  Still not sure
  //      if this is going to work.
  //
  //      3/17/2000 -- The alternative is to fix SAGE to associate
  //      a file name with every statement.
  //

  //   (*primary_os) << "In Unparser::getFileName(stmt): returning: currentOutputFileName = " 
  //                 <<  currentOutputFileName << endl;

     return currentOutputFileName;

  // Parent statement was NULL, so return NO FILENAME.
  // return "NO FILENAME";
#endif
   }

//-----------------------------------------------------------------------------------
//  char* Unparser::getCurOutFileName() 
//
//  get the filename from the Sage File Object
//-----------------------------------------------------------------------------------
char*
Unparser::getCurOutFileName()
   {
  //
  // [DT] 3/9/2000 -- Simply return the currentFileName member.
  //
  //      3/13/2000 -- First check to see if 
  //      globalScope->get_file_info()->get_filename() exists.
  //

#if 0
  // Use the currentOutputFileName so that we don't require the file to be stored
     ROSE_ASSERT(file != NULL);
     SgScopeStatement* globalScope = (SgScopeStatement*) (&(file->root()));

     if(globalScope!=NULL)
          if(globalScope->get_file_info()!=NULL)
               if(globalScope->get_file_info()->get_filename()!=NULL)
                    return globalScope->get_file_info()->get_filename();
#endif

  return currentOutputFileName;

  //---------------------------------------------------------
  //
  // [DT] Following is the original version of this function.
  //
#if 0
  ROSE_ASSERT(file != NULL);
  SgScopeStatement* globalScope = (SgScopeStatement*) (&(file->root()));

  ROSE_ASSERT(globalScope != NULL);
  ROSE_ASSERT(globalScope->get_file_info() != NULL);
  ROSE_ASSERT(globalScope->get_file_info()->get_filename() != NULL);
  return globalScope->get_file_info()->get_filename();
#endif

}  

bool UnparserBase::isPartOfTransformation( SgLocatedNode *n )
{
   return (n->get_file_info()==0 || n->get_file_info()->get_isPartOfTransformation());
}

bool
Unparser::containsLanguageStatements ( char* fileName )
   {
  // We need to implement this later
     ROSE_ASSERT (fileName != NULL);
  // printf ("Warning: Unparser::containsLanguageStatements(%s) not implemented! \n",fileName);

// Note that: FALSE will imply that the file contains only preprocessor declarations
//                  and thus the file need not be unparsed with a different name
//                  (a local header file).
//            TRUE  will imply that it needs to be unparsed as a special renamed 
//                  header file so that transformations in the header files can be
//                  supported.

#if 1
     return FALSE;
#else
     return TRUE;
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
     return FALSE;
#else
     return TRUE;
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

//-----------------------------------------------------------------------------------
//  void Unparser::printDebugInfo
//  
//  prints out debugging information specified in debug_info. A newline is also
//  inserted if TRUE. 
//-----------------------------------------------------------------------------------
void
UnparserBase::printDebugInfo(const char* debug_info, bool newline)
   {
     if (isDebug()) 
        {
//        opt.display("Unparser::printDebugInfo(char*,bool)");

          cout << debug_info;
          if (newline)
               cout << endl;

//        printf ("Exiting in Unparser::printDebugInfo(char*,bool) \n");
//        ROSE_ABORT();
        }
   }

void
UnparserBase::printDebugInfo(int debug_info, bool newline)
   {
     if (isDebug())
        {
//        opt.display("Unparser::printDebugInfo(int,bool)");

          cout << debug_info;
          if (newline)
               cout << endl;

//        printf ("Exiting in Unparser::printDebugInfo(int,bool) \n");
//        ROSE_ABORT();
        }
   }

// This has been simplified by Markus Kowarschik. We need to introduce the
// case of statements that have been introduced by transformations.
bool
Unparser::statementFromFile ( SgStatement* stmt, char* sourceFilename )
   {
  // If the filename of the statement and the input filename are the same then 
  // the return result is TRUE.  IF not then we have to look to see if there
  // was a "#line xx "filename"" macro that was used (to explain the difference).

     ROSE_ASSERT (stmt != NULL);

     bool statementInFile = false;
     if (opt.get_unparse_includes_opt() == true)
        {
        // If we are to unparse all included files into the source file this this is ALWAYS true
           statementInFile = true;
        }
       else
        {
       // Compare the file names from the file info object in each statement
          char* statementfilename = ROSE::getFileName(stmt);
          ROSE_ASSERT (statementfilename != NULL);

       // statementInFile = (strcmp(statementfilename, sourceFilename) == 0) ? true : false;
       // The statement is considered to be in the file if it is part of a transformation added to the AST
       // if (statementInFile == false)
          if ( strcmp(statementfilename, sourceFilename) == 0 )
             {
                statementInFile = true;
             }
            else
             {
            // if filenames don't match then check the transformation flag (see if this is part of a transformation)
#if 0
               if (stmt->get_file_info() != NULL)
                  {
                 // printf ("Found a valid file info object \n");
                    ROSE_ASSERT (stmt->get_file_info() != NULL);
                    if (isPartOfTransformation(stmt))
                       {
                      // printf ("setting statementInFile == true \n");
                         statementInFile = true;
                       }
                  }
#else
            // printf ("isPartOfTransformation() = %s \n",isPartOfTransformation(stmt) ? "true" : "false");
               if ( isPartOfTransformation(stmt))
                  {
                    statementInFile = true;
                  }
#endif
             }
        }

  // printf ("In Unparser::statementFromFile (stmt file = %s sourceFilename = %s ) \n",
  //      statementfilename, sourceFilename);

  // printf ("statementInFile = %s \n",(statementInFile == true) ? "true" : "false");

     return statementInFile;
   }


bool
Unparser::isASecondaryFile ( SgStatement* stmt )
   {
  // for now just assume there are no secondary files
     return FALSE;
   }

#if 0
// This does not appear to be used any more (member functions in the SgProject 
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

void
Unparser::unparseFile ( SgFile* file, SgUnparse_Info& info )
   {
  // Turn ON the error checking which triggers an if the default SgUnparse_Info constructor is called
     SgUnparse_Info::forceDefaultConstructorToTriggerError = true;

     SgScopeStatement* globalScope = (SgScopeStatement*) (&(file->root()));
     ROSE_ASSERT(globalScope != NULL);

  // printf ("Calling unparseGlobalStmt() \n");
     unparseStatement(globalScope, info);

  // DQ (7/19/2004): Added newline at end of file
  // (some compilers (e.g. g++) complain if no newline is present)
  // This does not work, not sure why
  // cur << "\n/* EOF: can't insert newline at end of file to avoid g++ compiler warning */\n\n";

  // DQ: This does not compile
  // cur << std::endl;

  // DQ: This does not force a new line either!
  // cur << "\n\n\n";
     cur.flush();

  // Turn OFF the error checking which triggers an if the default SgUnparse_Info constructor is called
     SgUnparse_Info::forceDefaultConstructorToTriggerError = false;
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
                    if ( (*i)->get_name().str() != NULL)
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

bool 
Unparser::isTransformed(SgStatement* stmt)
   {
  // This function must traverse the AST and look for any sign that 
  // the subtree has been transformed.  This might be a difficult 
  // function to write.  We might have to force transformations to
  // do something to make their presence better known (e.g. removing
  // a statement will leave no trace in the AST of the transformation).

  // Assume no transformation at the moment while we debug templates
     return false;
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
string
globalUnparseToString ( SgNode* astNode, SgUnparse_Info* inputUnparseInfoPointer )
   {
  // This global function permits any SgNode (including it's subtree) to be turned into a string

     string returnString;

  // all options are now defined to be false. When these options can be passed in
  // from the prompt, these options will be set accordingly.
     bool _auto                         = FALSE;
     bool linefile                      = FALSE;
     bool useOverloadedOperators        = FALSE;
     bool num                           = FALSE;

  // It is an error to have this always turned off (e.g. pointer = this; will not unparse correctly)
     bool _this                         = TRUE;

     bool caststring                    = FALSE;
     bool _debug                        = FALSE;
     bool _class                        = FALSE;
     bool _forced_transformation_format = FALSE;
     bool _unparse_includes             = FALSE;

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

     int lineNumber = 0;  // Zero indicates that ALL lines should be unparsed

     // Initialize the Unparser using a special string stream inplace of the usual file stream 
     ostringstream outputString;

     SgLocatedNode* locatedNode = isSgLocatedNode(astNode);
     char* fileNameOfStatementsToUnparse = NULL;
     if (locatedNode == NULL)
        {
       // printf ("WARNING: applying AST -> string for non expression/statement AST objects \n");
          fileNameOfStatementsToUnparse = "defaultFileNameInGlobalUnparseToString";
        }
       else
        {
          ROSE_ASSERT (locatedNode != NULL);
          fileNameOfStatementsToUnparse = locatedNode->getFileName();
        }
     ROSE_ASSERT (fileNameOfStatementsToUnparse != NULL);

     Unparser roseUnparser ( &outputString, fileNameOfStatementsToUnparse, roseOptions, lineNumber );

  // Information that is passed down through the tree (inherited attribute)
  // Use the input SgUnparse_Info object if it is available.
     SgUnparse_Info* inheritedAttributeInfoPointer = NULL;
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
//        inheritedAttributeInfoPointer = new SgUnparse_Info (NO_UNPARSE_INFO);
          inheritedAttributeInfoPointer = new SgUnparse_Info();
          ROSE_ASSERT (inheritedAttributeInfoPointer != NULL);

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
        }

     ROSE_ASSERT (inheritedAttributeInfoPointer != NULL);
     SgUnparse_Info & inheritedAttributeInfo = *inheritedAttributeInfoPointer;

  // Turn ON the error checking which triggers an error if the default SgUnparse_Info constructor is called
  // SgUnparse_Info::forceDefaultConstructorToTriggerError = true;

#if 1
  // DQ (10/19/2004): Cleaned up this code, remove this dead code after we are sure that this worked propoerly
  // Actually, this code is required to be this way, since after this branch the current function returns and
  // some data must be cleaned up differently!  So put this back and leave it this way, and remove the
  // "Implementation Note".

  // Both SgProject and SgFile are handled via recursive calls
     if ( (isSgProject(astNode) != NULL) || (isSgFile(astNode) != NULL) )
        {
       // printf ("Implementation Note: Put these cases (unparsing the SgProject and SgFile into the cases for nodes derived from SgSupport below! \n");

       // Handle recursive call for SgProject
          if (isSgProject(astNode) != NULL)
             {
               SgProject* project = isSgProject(astNode);
               ROSE_ASSERT(project != NULL);
               for (int i = 0; i < project->numberOfFiles(); i++)
                  {
                    SgFile* file = &(project->get_file(i));
                    ROSE_ASSERT(file != NULL);
                    string unparsedFileString = globalUnparseToString(file,inputUnparseInfoPointer);
                    string prefixString       = string("/* TOP:")      + string(ROSE::getFileName(file)) + string(" */ \n");
                    string suffixString       = string("\n/* BOTTOM:") + string(ROSE::getFileName(file)) + string(" */ \n\n");
                    returnString += prefixString + unparsedFileString + suffixString;
                  }
             }

       // Handle recursive call for SgFile
          if (isSgFile(astNode) != NULL)
             {
               SgFile* file = isSgFile(astNode);
               ROSE_ASSERT(file != NULL);
               SgGlobal* globalScope = file->get_root();
               ROSE_ASSERT(globalScope != NULL);
               returnString = globalUnparseToString(globalScope,inputUnparseInfoPointer);
             }
        }
       else
#endif
        {
       // DQ (1/12/2003): Only now try to trap use of SgUnparse_Info default constructor
       // Turn ON the error checking which triggers an error if the default SgUnparse_Info constructor is called
          SgUnparse_Info::forceDefaultConstructorToTriggerError = true;

          if (isSgStatement(astNode) != NULL)
             {
               SgStatement* stmt = isSgStatement(astNode);
               roseUnparser.unparseStatement ( stmt, inheritedAttributeInfo );
             }

          if (isSgExpression(astNode) != NULL)
             {
               SgExpression* expr = isSgExpression(astNode);
               roseUnparser.unparseExpression ( expr, inheritedAttributeInfo );
             }

          if (isSgType(astNode) != NULL)
             {
               SgType* type = isSgType(astNode);
               roseUnparser.unparseType ( type, inheritedAttributeInfo );
             }

          if (isSgSymbol(astNode) != NULL)
             {
               SgSymbol* symbol = isSgSymbol(astNode);
               roseUnparser.unparseSymbol ( symbol, inheritedAttributeInfo );
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
                              string unparsedFileString = globalUnparseToString(file,inputUnparseInfoPointer);
                              string prefixString       = string("/* TOP:")      + string(ROSE::getFileName(file)) + string(" */ \n");
                              string suffixString       = string("\n/* BOTTOM:") + string(ROSE::getFileName(file)) + string(" */ \n\n");
                              returnString += prefixString + unparsedFileString + suffixString;
                            }
                         break;
                       }
                    case V_SgFile:
                       {
                         SgFile* file = isSgFile(astNode);
                         ROSE_ASSERT(file != NULL);
                         SgGlobal* globalScope = file->get_root();
                         ROSE_ASSERT(globalScope != NULL);
                         returnString = globalUnparseToString(globalScope,inputUnparseInfoPointer);
                         break;
                       }
#endif
                    case V_SgTemplateParameter:
                       {
                         SgTemplateParameter* templateParameter = isSgTemplateParameter(astNode);
                         roseUnparser.unparseTemplateParameter(templateParameter,inheritedAttributeInfo);
                         break;
                       }
                    case V_SgTemplateArgument:
                       {
                         SgTemplateArgument* templateArgument = isSgTemplateArgument(astNode);
                         roseUnparser.unparseTemplateArgument(templateArgument,inheritedAttributeInfo);
                         break;
                       }
                    case V_SgInitializedName:
                       {
                      // QY: not sure how to implement this
                      // DQ (7/23/2004): This should unparse as a declaration 
                      // (type and name with initializer).
                         break;
                       }

                 // Perhaps the support for SgFile and SgProject shoud be moved to this location?
                    default:
                         printf ("Error: default reached in node derived from SgSupport astNode = %s \n",astNode->sage_class_name());
                         ROSE_ABORT();
                }
             }

       // Turn OFF the error checking which triggers an if the default SgUnparse_Info constructor is called
          SgUnparse_Info::forceDefaultConstructorToTriggerError = false;

       // MS: following is the rewritten code of the above outcommented 
       //     code to support ostringstream instead of ostrstream.
          returnString = outputString.str();

       // Call function to tighten up the code to make it more dense
          if (inheritedAttributeInfo.SkipWhitespaces() == true)
             {
               returnString = roseUnparser.removeUnwantedWhiteSpace ( returnString );
             }

       // delete the allocated SgUnparse_Info object
          if (inputUnparseInfoPointer == NULL)
               delete inheritedAttributeInfoPointer;
        }

     return returnString;
   }

string get_output_filename( SgFile& file )
{
       // Use the filename with ".rose" suffix for the output file from ROSE (the unparsed C++ code file)
       // This allows Makefiles to have suffix rules drive the generation of the *.C.rose from from ROSE
       // and then permits the C++ compiler to be called to generate the *.o file from the *.C.rose file
          char outputFilename[256];

          if (file.get_unparse_output_filename() != NULL)
             {
            // allow the user to specify the name of the output file
#if 0
               printf ("VALID file.get_unparse_output_filename() found: %s \n",
                    file.get_unparse_output_filename());
#endif
               sprintf(outputFilename,"%s",file.get_unparse_output_filename());
             }
            else
             {
            // use a prefix plus the original input file name to name the output file
#if 0
               printf ("NO VALID file.get_unparse_output_filename() found, using filename to build one: rose_<%s> \n",
                    file.getFileName());
#endif
               sprintf(outputFilename,"rose_%s",file.getFileName());

               printf ("I think this case is never used! Exiting ... \n");
               ROSE_ASSERT (1 == 2);
             }
#if 1
          if ( file.get_verbose() == TRUE )
               printf ("\nROSE unparsed outputFile name is %s \n\n",outputFilename);
#endif
    return string(outputFilename);
}



static UnparserBase*
getCppFileUnparser ( SgFile & file, ostream* os, UnparseDelegate* repl );

static UnparserBase*
getF90FileUnparser ( SgFile & file, ostream* os, UnparseDelegate* repl );


// Later we might want to move this to the SgProject or SgFile support class (generated by ROSETTA)
void
unparseFile ( SgFile & file, UnparseDelegate* repl  )
   {
  // Call the unparser mechanism

  // printf ("Inside of unparseFile ( SgFile & file ) (using filename = %s) \n",file.get_unparse_output_filename());

  // debugging assertions
  // ROSE_ASSERT ( file.get_verbose() == TRUE );
  // ROSE_ASSERT ( file.get_skip_unparse() == FALSE );
  // file.set_verbose(TRUE);

     if (file.get_skip_unparse() == TRUE)
        {
       // MS: commented out the following output
       // if ( file.get_verbose() == TRUE )
	    // printf ("### ROSE::skip_unparse == TRUE: Skipping all source code generation by ROSE generated preprocessor! \n");
        }
       else
        {
          bool _unparse_f90 = file.get_unparse_f90();
          string outputFilename = get_output_filename(file);
          fstream ROSE_OutputFile(outputFilename.c_str(),ios::out);

//        file.set_unparse_includes(FALSE);
//        ROSE_ASSERT (file.get_unparse_includes() == FALSE);

       // This is the new unparser that Gary Lee is developing
       // The goal of this unparser is to provide formatting
       // similar to that of the original application code
#if 0
          if ( file.get_verbose() == TRUE )
               printf ("Calling the NEWER unparser mechanism: outputFilename = %s \n",outputFilename);
#endif

	  UnparserBase* roseUnparser = NULL;
	  if (_unparse_f90) {
	       roseUnparser = getF90FileUnparser(file, &ROSE_OutputFile, repl);
	  }
	  else {
	       roseUnparser = getCppFileUnparser(file, &ROSE_OutputFile, repl);
	  }

       // Location to turn on unparser specific debugging data that shows up in the output file
       // This prevents the unparsed output file from compiling properly!
       // ROSE_DEBUG = 0;

       // information that is passed down through the tree (inherited attribute)
//        SgUnparse_Info inheritedAttributeInfo (NO_UNPARSE_INFO);
          SgUnparse_Info inheritedAttributeInfo;

       // Call member function to start the unparsing process
       // roseUnparser.run_unparser();
          roseUnparser->unparseFile(&file,inheritedAttributeInfo);
	  
       // And finally we need to close the file (to flush everything out!)
          ROSE_OutputFile.close();

	  delete roseUnparser;
        }
   }


UnparserBase*
getCppFileUnparser ( SgFile & file, ostream* os, UnparseDelegate* repl )
   {
     // all options are now defined to be false. When these options can be passed in
     // from the prompt, these options will be set accordingly.
     bool _auto                         = FALSE;
     bool linefile                      = FALSE;
     bool useOverloadedOperators        = FALSE;
     bool num                           = FALSE;
     
     // It is an error to have this always turned off (e.g. pointer = this; will not unparse correctly)
     bool _this                         = TRUE;
     
     bool caststring                    = FALSE;
     bool _debug                        = FALSE;
     bool _class                        = FALSE;
     bool _forced_transformation_format = FALSE;
     
     // control unparsing of include files into the source file (default is false)
     bool _unparse_includes             = file.get_unparse_includes();
     
     Unparser_Opt opts( _auto,
			linefile,
			useOverloadedOperators,
			num,
			_this,
			caststring,
			_debug,
			_class,
			_forced_transformation_format,
			_unparse_includes );
     
     int lineNumber = 0;  // Zero indicates that ALL lines should be unparsed
     
     Unparser* unparser = new Unparser( os, ROSE::getFileName(&file), 
					opts, lineNumber, NULL, repl );
     return unparser;
   }


UnparserBase*
getF90FileUnparser ( SgFile & file, ostream* os, UnparseDelegate* repl )
   {
     int lineNumber = 0;  // Zero indicates that ALL lines should be unparsed
     
     Unparser_Opt opts;
     UnparserFort* unparser = new UnparserFort( os, ROSE::getFileName(&file),
						opts, lineNumber, NULL, repl );
     return unparser;
   }

