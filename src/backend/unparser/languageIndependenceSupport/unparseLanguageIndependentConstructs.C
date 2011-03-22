// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "unparser.h"
#include <limits>

#include "OmpAttribute.h" // to support unparsing OpenMP constructs
// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

#define OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES 0
#define OUTPUT_HIDDEN_LIST_DATA 0
#define OUTPUT_DEBUGGING_INFORMATION 0

// DQ (8/13/2007): This function was implemented by Thomas
std::string
UnparseLanguageIndependentConstructs::resBool(bool val) const
   {
     return val ? "True" : "False" ;
   }

// DQ (8/13/2007): This function was implemented by Thomas
template<typename T>
std::string
UnparseLanguageIndependentConstructs::tostring(T t) const
   {
     std::ostringstream myStream; //creates an ostringstream object
     myStream << std::showpoint << t << std::flush; // Distinguish integer and floating-point numbers
     return myStream.str(); //returns the string form of the stringstream object
   }

// DQ (8/13/2007): This function was implemented by Thomas
void
UnparseLanguageIndependentConstructs::curprint (const std::string & str) const
   {

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

   }

// DQ (8/13/2007): This has been moved to the base class (language independent code)
void
UnparseLanguageIndependentConstructs::markGeneratedFile() const
   {
     unp->u_sage->curprint ( "\n#define ROSE_GENERATED_CODE\n" );
   }

// This has been simplified by Markus Kowarschik. We need to introduce the
// case of statements that have been introduced by transformations.
// bool Unparser::statementFromFile ( SgStatement* stmt, char* sourceFilename )
bool
UnparseLanguageIndependentConstructs::statementFromFile ( SgStatement* stmt, string sourceFilename )
   {
  // If the filename of the statement and the input filename are the same then 
  // the return result is true.  IF not then we have to look to see if there
  // was a "#line xx "filename"" macro that was used (to explain the difference).

     ROSE_ASSERT (stmt != NULL);

     bool statementInFile = false;

  // FMZ (comment by DQ (11/14/2008)):
  // This is part of the support for module files in Fortran.  Modules seen in the compilation 
  // of a Fortran program cause a "<module name>.rmod" file to be generated. When we unparse 
  // the "*.rmod" we want to output all statements, but since they came from the original 
  // fortran file (a better translation would avoid this problem), the function would conclude
  // that they should not be unparsed (this fix forces the statements in a "*.rmod" file to 
  // always be unparsed.  If the SgSourceFile vuilt to represent the "*.rmod" file had been 
  // constructed as a transformation then the file info objects would have been marked as
  // part of a transforamtion and this fix would not have been required.  At some point this
  // can be improved.  So this is a fine temporary fix for now.
     if (StringUtility::fileNameSuffix(sourceFilename)=="rmod") 
        {
       // If we are to unparse a module  into the .rmod file this this is ALWAYS true
          return true;
        }

     if (unp->opt.get_unparse_includes_opt() == true)
        {
        // If we are to unparse all included files into the source file this this is ALWAYS true
           statementInFile = true;
        }
       else
        {
       // Compare the file names from the file info object in each statement
       // char* statementfilename = ROSE::getFileName(stmt);
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

       // DQ (1/11/2006): OutputCodeGeneration is not set to be true where transformations 
       // require it.  Transformation to include header files don't set the OutputCodeGeneration flag.
       // if (isOutputInCodeGeneration || isTransformation)
          if (isOutputInCodeGeneration == true)
             {
               statementInFile = true;
             }
            else
             {
            // DQ (8/17/2005): Need to replace this with call to compare Sg_File_Info::file_id 
            // numbers so that we can remove the string comparision operator.
            // statementfilename = ROSE::getFileName(stmt);
               statementfilename = stmt->get_file_info()->get_filenameString();
#if 0
               printf ("Inside of statementFromFile(): statementfilename = %s sourceFilename = %s \n",statementfilename.c_str(),sourceFilename.c_str());
#endif
            // DQ (10/22/2007): Allow empty name strings (to support #line n "")
            // ROSE_ASSERT (statementfilename.empty() == false);

            // if ( strcmp(statementfilename, sourceFilename) == 0 )
               if ( statementfilename == sourceFilename )
                  {
                    statementInFile = true;
                  }
             }
#if 0
          printf ("In Unparser::statementFromFile (statementInFile = %s output = %s stmt = %p = %s = %s in file = %s sourceFilename = %s ) \n",
               (statementInFile == true) ? "true": "false", (isOutputInCodeGeneration == true) ? "true": "false", 
               stmt, stmt->class_name().c_str(), SageInterface::get_name(stmt).c_str(), 
               statementfilename.c_str(), sourceFilename.c_str());
#endif
#if 0
          stmt->get_file_info()->display("In Unparser::statementFromFile()");
#endif
        }

#if 0
     printf ("statementInFile = %p = %s = %s = %s \n",stmt,stmt->class_name().c_str(),SageInterface::get_name(stmt).c_str(),(statementInFile == true) ? "true" : "false");
  // stmt->get_file_info()->display("debug why false");
#endif

#if 0
  // Debugging support
     SgDeclarationStatement* declarationStatement = isSgDeclarationStatement(stmt);
     if (declarationStatement != NULL && statementInFile == false && stmt->get_file_info()->isFrontendSpecific() == false)
        {
          curprint ( string("\n/* Inside of UnparseLanguageIndependentConstructs::statementFromFile (" ) + StringUtility::numberToString(stmt) + "): sage_class_name() = " + stmt->sage_class_name() + " (skipped) */ \n");
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
     AttachedPreprocessingInfoType* comments = locatedNode->getAttachedPreprocessingInfo();

     if (comments != NULL)
        {
          printf ("Found attached comments (at %p of type: %s): \n",locatedNode,locatedNode->sage_class_name());
          AttachedPreprocessingInfoType::iterator i;
          for (i = comments->begin(); i != comments->end(); i++)
             {
               ROSE_ASSERT ( (*i) != NULL );
               printf ("          Attached Comment (relativePosition=%s): %s\n",
                    ((*i)->getRelativePosition() == PreprocessingInfo::before) ? "before" : "after",
                    (*i)->getString().c_str());
               printf ("Comment/Directive getNumberOfLines = %d getColumnNumberOfEndOfString = %d \n",(*i)->getNumberOfLines(),(*i)->getColumnNumberOfEndOfString());
               (*i)->get_file_info()->display("comment/directive location");
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
     list<SgStatement*>::iterator i = unp->compilerGeneratedStatementQueue.begin();
  // printf ("compilerGeneratedStatementQueue.size() = %zu \n",compilerGeneratedStatementQueue.size());
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

  // curprint("/* Top of statement */ \n ");

#if 0
     int line    = stmt->get_startOfConstruct()->get_raw_line();
     string file = stmt->get_startOfConstruct()->get_filenameString();
     printf ("Unparse (language independent = %s) statement (%p): %s line = %d file = %s \n",languageName().c_str(),stmt,stmt->class_name().c_str(),line,file.c_str());
#endif

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES
     printf ("Unparse statement (%p): %s name = %s \n",stmt,stmt->class_name().c_str(),SageInterface::get_name(stmt).c_str());

  // DQ (4/17/2007): Added enforcement for endOfConstruct().
     ROSE_ASSERT (stmt->get_endOfConstruct() != NULL);

     curprint ( string("\n/* Unparse statement (" ) + StringUtility::numberToString(stmt) 
         + "): class_name() = " + stmt->class_name() 
		+ " raw line (start) = " + tostring(stmt->get_startOfConstruct()->get_raw_line()) 
		+ " raw line (end) = " + tostring(stmt->get_endOfConstruct()->get_raw_line()) 
         + " */ \n");
     char buffer[100];
     snprintf (buffer,100,"%p",stmt);
     curprint ( string("\n/* Top of unparseStatement " ) + stmt->class_name() + " at: " + buffer + " */ \n");
#endif

#if 0
  // DQ (10/25/2006): Debugging support for file info data for each IR node
     if (stmt->get_endOfConstruct() == NULL)
        {
          printf ("Error in unparseStatement(): stmt = %p = %s stmt->get_endOfConstruct() == NULL \n",stmt,stmt->class_name().c_str());
          stmt->get_file_info()->display("unparseStatement (debug)");
        }
  // ROSE_ASSERT(stmt->get_endOfConstruct() != NULL);

     curprint ( string("\n/* Top of unparseStatement (UnparseLanguageIndependentConstructs)" ) + string(stmt->sage_class_name()) + " */\n ");
     ROSE_ASSERT(stmt->get_startOfConstruct() != NULL);
  // ROSE_ASSERT(stmt->getAttachedPreprocessingInfo() != NULL);
     int numberOfComments = -1;
     if (stmt->getAttachedPreprocessingInfo() != NULL)
        numberOfComments = stmt->getAttachedPreprocessingInfo()->size();
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

  // Debugging support
     if (ROSE_DEBUG > 3)
        {
          cout << "In unparseStatement(): getLineNumber(stmt) = "
               << stmt->get_file_info()->displayString("unparseStatement")
               << " unp->cur_index = " << unp->cur_index << endl;
        }

#if 0
  // Debugging support
     printOutComments (stmt);
#endif

     if (stmt->get_file_info() == NULL)
        {
          printf ("Error: stmt->get_file_info() == NULL stmt = %p = %s \n",stmt,stmt->class_name().c_str());
        }
     ROSE_ASSERT(stmt->get_file_info() != NULL);

#if 1 // FIXME cause conflict in "make check"?
  // FMZ : we have ".rmod" file which will not satisfy this condition
  // JJW (6/23/2008): Move check for statement-within-file here rather than in individual procedures
     if (!statementFromFile(stmt, getFileName()))
        {
          return;
        }
#endif

  // saveCompilerGeneratedStatements(stmt,info);
  // DQ (5/27/2005): fixup ordering of comments and any compiler generated code
     if ( info.outputCompilerGeneratedStatements() == false && 
          stmt->get_file_info()->isCompilerGenerated() == true && 
          isSgGlobal(stmt->get_parent()) != NULL )
        {
       // push all compiler generated nodes onto the static stack and unparse them after comments and directives 
       // of the next statement are output but before the associated statement to which they are attached.

       // printf ("Save the compiler-generated statement (%s), putting it onto the queue \n",stmt->sage_class_name());
          unp->compilerGeneratedStatementQueue.push_front(stmt);

       // This return prevents this code from being trivially separated out into function.
          return;
        }


     if( unparseLineReplacement(stmt,info) )
       return;

  // Markus Kowarschik: This is the new code to unparse directives before the current statement
     //AS(05/20/09): LineReplacement should replace a statement with a line. Override unparsing
     //of subtree.
     unparseAttachedPreprocessingInfo(stmt, info, PreprocessingInfo::before);

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
            // printf ("Delegate unparser retruned true for repl->unparse_statement(%p) \n",stmt);
               return;
             }
        }

  // DQ (5/22/2007): Added to support name qualification and access to the new hidden 
  // type, declaration and class elaboration lists stored in the scopes.
     SgScopeStatement* scopeStatement = isSgScopeStatement(stmt);
     SgScopeStatement* savedScope = NULL;
     if (scopeStatement != NULL)
        {
       // DQ (11/3/2007): Save the original scope so that we can restore it at the end (since we don't use a new SgUnparse_Info object).
          savedScope = info.get_current_scope();
#if 0
          printf ("Setting the current_scope in info: scopeStatement = %p = %s = %s \n",
               scopeStatement,scopeStatement->class_name().c_str(),SageInterface::get_name(scopeStatement).c_str());

          if (savedScope != NULL)
               printf ("Setting the savedScope = %p = %s = %s \n",
                    savedScope,savedScope->class_name().c_str(),SageInterface::get_name(savedScope).c_str());
#endif
          info.set_current_scope(NULL);
          info.set_current_scope(scopeStatement);
        }
       else
        {
       // In this case we are only saving the scope to do error checking
          savedScope = info.get_current_scope();
        }
     

  // DQ (5/27/2007): Commented out, uncomment when we are ready for Robert's new hidden list mechanism.
     if (info.get_current_scope() == NULL)
        {
          printf ("In unparseStatement(): info.get_current_scope() == NULL (likely called from SgNode::unparseToString()) stmt = %p = %s \n",stmt,stmt->class_name().c_str());
          stmt->get_startOfConstruct()->display("In unparseStatement(): info.get_current_scope() == NULL: debug");
          ROSE_ASSERT(false);
        }
  // ROSE_ASSERT(info.get_current_scope() != NULL);

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
       // DQ (12/4/2007): Added to ROSE (was removed at some point).
          unparseLineDirectives(stmt);

       // DQ (7/19/2007): This only applies to Fortran where every statement can have a statement number (numeric lable, different from SgLabelStatement)
          unparseStatementNumbers(stmt,info);

          switch (stmt->variantT())
             {
               case V_SgGlobal:            unparseGlobalStmt   (stmt, info); break;
               case V_SgFunctionTypeTable: unparseFuncTblStmt  (stmt, info); break;
               case V_SgNullStatement:     unparseNullStatement(stmt, info); break;

            // DQ (11/29/2008): Added support for unparsing CPP directives now supported as IR nodes.
               case V_SgIncludeDirectiveStatement: unparseIncludeDirectiveStatement (stmt, info); break;
               case V_SgDefineDirectiveStatement:  unparseDefineDirectiveStatement  (stmt, info); break;
               case V_SgUndefDirectiveStatement:   unparseUndefDirectiveStatement  (stmt, info); break;
               case V_SgIfdefDirectiveStatement:   unparseIfdefDirectiveStatement  (stmt, info); break;
               case V_SgIfndefDirectiveStatement:  unparseIfndefDirectiveStatement  (stmt, info); break;
               case V_SgDeadIfDirectiveStatement:  unparseDeadIfDirectiveStatement  (stmt, info); break;
               case V_SgIfDirectiveStatement:      unparseIfDirectiveStatement  (stmt, info); break;
               case V_SgElseDirectiveStatement:    unparseElseDirectiveStatement  (stmt, info); break;
               case V_SgElseifDirectiveStatement:  unparseElseifDirectiveStatement  (stmt, info); break;
               case V_SgEndifDirectiveStatement:   unparseEndifDirectiveStatement  (stmt, info); break;
               case V_SgLineDirectiveStatement:    unparseLineDirectiveStatement  (stmt, info); break;
               case V_SgWarningDirectiveStatement: unparseWarningDirectiveStatement  (stmt, info); break;
               case V_SgErrorDirectiveStatement:   unparseErrorDirectiveStatement  (stmt, info); break;
               case V_SgEmptyDirectiveStatement:   unparseEmptyDirectiveStatement  (stmt, info); break;
               case V_SgIdentDirectiveStatement:   unparseIdentDirectiveStatement  (stmt, info); break;
               case V_SgIncludeNextDirectiveStatement: unparseIncludeNextDirectiveStatement  (stmt, info); break;
               case V_SgLinemarkerDirectiveStatement:  unparseLinemarkerDirectiveStatement  (stmt, info); break;
            // Liao 10/21/2010. Handle generic OpenMP directive unparsing here.
               case V_SgOmpAtomicStatement:
               case V_SgOmpSectionStatement:
               case V_SgOmpTaskwaitStatement:
               case V_SgOmpBarrierStatement:           unparseOmpSimpleStatement        (stmt, info);break;
               case V_SgOmpThreadprivateStatement:     unparseOmpThreadprivateStatement (stmt, info);break;
               case V_SgOmpFlushStatement:             unparseOmpFlushStatement         (stmt, info);break;
               // Generic OpenMP directives with a format of : begin-directive, begin-clauses, body, end-directive , end-clauses
               case V_SgOmpCriticalStatement:
               case V_SgOmpMasterStatement:
               case V_SgOmpOrderedStatement:
               case V_SgOmpSectionsStatement:
               case V_SgOmpParallelStatement:
               case V_SgOmpWorkshareStatement:
               case V_SgOmpSingleStatement:
               case V_SgOmpTaskStatement:  unparseOmpGenericStatement (stmt, info); break;
               default:
                 // DQ (11/4/2008): This is a bug for the case of a SgFortranDo statement, unclear what to do about this.
                 // Call the derived class implementation for C, C++, or Fortran specific language unparsing.
                 // unparseLanguageSpecificStatement(stmt,info);
                 // unp->repl->unparseLanguageSpecificStatement(stmt,info);
                    unparseLanguageSpecificStatement(stmt,info);
                    break;
             }

       // DQ (5/8/2010): Reset the source code position in the AST.
          if (unp->get_resetSourcePosition() == true)
             {
               unp->resetSourcePosition(stmt);
             }
        }

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

  // DQ (comments) This is where new lines are output after the statement.
     unp->cur.format(stmt, info, FORMAT_AFTER_STMT);

  // Markus Kowarschik: This is the new code to unparse directives after the current statement
     unparseAttachedPreprocessingInfo(stmt, info, PreprocessingInfo::after);

  // DQ (5/31/2005): special handling for compiler generated statements
     if (isSgGlobal(stmt) != NULL)
        {
       // printf ("Output template definitions after the final comments in the file \n");
          outputCompilerGeneratedStatements(info);
        }

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES
     printf ("Leaving unparse statement (%p): sage_class_name() = %s name = %s \n",stmt,stmt->sage_class_name(),SageInterface::get_name(stmt).c_str());
  // printf ("Leaving unparse statement (%p): sage_class_name() = %s \n",stmt,stmt->sage_class_name());
  // curprint ( string("\n/* Bottom of unparseStatement: sage_class_name() = " + stmt->sage_class_name() + " */ \n";
     curprint ( string("\n/* Bottom of unparseStatement (" ) + StringUtility::numberToString(stmt) 
         + "): sage_class_name() = " + stmt->sage_class_name() + " */ \n");
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
     printf ("Unparse (language independent = %s) expression (%p): %s compiler-generated = %s \n",languageName().c_str(),expr,expr->class_name().c_str(),expr->get_file_info()->isCompilerGenerated() ? "true" : "false");
#endif

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
          printf ("In unparseExpression(%s): Detected error expr->get_file_info()->isCompilerGenerated() != expr->get_startOfConstruct()->isCompilerGenerated() \n",expr->class_name().c_str());
          printf ("     expr->get_file_info() = %p expr->get_operatorPosition() = %p expr->get_startOfConstruct() = %p \n",expr->get_file_info(),expr->get_operatorPosition(),expr->get_startOfConstruct());
          ROSE_ASSERT(expr->get_file_info()->get_parent() != NULL);
          printf ("parent of file info = %p = %s \n",expr->get_file_info()->get_parent(),expr->get_file_info()->get_parent()->class_name().c_str());
          expr->get_file_info()->display("expr->get_file_info(): debug");
          expr->get_startOfConstruct()->display("expr->get_startOfConstruct(): debug");
        }
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
     if( unparseLineReplacement(expr,info) )
       return;

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
     AstUnparseAttribute* unparseAttribute = dynamic_cast<AstUnparseAttribute*>(expr->getAttribute(AstUnparseAttribute::markerName));
     if (unparseAttribute != NULL)
        {
          string code = unparseAttribute->toString(AstUnparseAttribute::e_before);
          curprint (code);
        }
#if 0
     if (expr->attributeExists("_UnparserSourceReplacement") == true)
        {
       // string rep=(expr->attribute["_UnparserSourceReplacement"])->toString();
       // string rep=(expr->attribute()["_UnparserSourceReplacement"])->toString();
          string rep = expr->getAttribute("_UnparserSourceReplacement")->toString();
          cout << "UNPARSER: SOURCE REPLACEMENT:" << rep << endl;
          curprint ( rep);
        }
#else
  // Only replace the unparsing of the IR node with a string if a string is marked as AstUnparseAttribute::e_replace.
     if (unparseAttribute != NULL && unparseAttribute->replacementStringExists() == true)
        {
          string code = unparseAttribute->toString(AstUnparseAttribute::e_replace);
          curprint (code);
        }
#endif
       else
        {
       // DQ (5/21/2004): revised need_paren handling in EDG/SAGE III and within SAGE III IR)
       // QY (7/9/2004): revised to use the new unp->u_sage->PrintStartParen test
          bool printParen = unp->u_sage->PrintStartParen(expr,info);
#if 0
       // DQ (8/21/2005): Suppress comments when unparsing to build type names
          if ( !info.SkipComments() || !info.SkipCPPDirectives() )
             {
               curprint ( string("\n /* In unparseExpression paren ") + expr->sage_class_name() + string(" paren printParen = ") + (printParen ? "true" : "false") + string(" */ \n"));
             }
#endif
       // if (printParen)
       // ROSE_ASSERT(currentFile != NULL);
       // if ( (printParen == true) && (currentFile->get_Fortran_only() == false) )
          if (printParen == true)
             {
            // Make sure this is not an expresion list
               ROSE_ASSERT (isSgExprListExp(expr) == NULL);

            // Output the left paren
               curprint ( "(");
             }

       // DQ (10/7/2004): Definitions should never be unparsed within code generation for expressions
          if (info.SkipClassDefinition() == false)
             {
            // printf ("Skip output of class definition in unparseExpression \n");
            // DQ (10/8/2004): Skip all definitions when outputing expressions!
            // info.set_SkipClassDefinition();
            // info.set_SkipDefinition();
             }

       // DQ (10/13/2006): Remove output of qualified names from this leve of generality!
       // DQ (12/22/2005): Output any name qualification that is required 
       // (we only explicitly store the global scope qualification since 
       // this is all that it seems that EDG stores).
       // unparseQualifiedNameList(expr->get_qualifiedNameList());

          switch (expr->variant())
             {
               case UNARY_EXPRESSION:  { unparseUnaryExpr (expr, info); break; }
               case BINARY_EXPRESSION: { unparseBinaryExpr(expr, info); break; }

            // DQ (8/15/2007): This has been moved to the base class
               case EXPR_LIST: { unparseExprList(expr, info); break; }

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
            // Output the right paren
               curprint ( ")");
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

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES
  // DQ (8/21/2005): Suppress comments when unparsing to build type names
     if ( !info.SkipComments() || !info.SkipCPPDirectives() )
        {
          printf ("Leaving unparse expression (%p): sage_class_name() = %s \n",expr,expr->sage_class_name());
       // unp->u->sage->curprint ( "\n/* Bottom of unparseExpression " << string(expr->sage_class_name()) << " */ \n");
          curprint ( "\n/* Bottom of unparseExpression " + expr->class_name() + " */ \n");
        }
#endif
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

  // Not much to do here except output a ";", not really required however.
  // curprint ( string(";";
   }

void
UnparseLanguageIndependentConstructs::unparseNullExpression (SgExpression* expr, SgUnparse_Info& info)
   {
  // Nothing to do here! (unless we need a ";" or something)
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
#if 1
     return false;
#else
     return true;
#endif
   }

void
UnparseLanguageIndependentConstructs::unparseGlobalStmt (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgGlobal* globalScope = isSgGlobal(stmt);
     ROSE_ASSERT(globalScope != NULL);

#if OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES
     printf ("\n ***** Unparsing the global Scope ***** \n\n");
     printf ("global scope file = %s \n",TransformationSupport::getSourceFile(globalScope)->getFileName().c_str());
     printf ("global scope size = %ld \n",globalScope->get_declarations().size());
#endif

#if OUTPUT_HIDDEN_LIST_DATA
     outputHiddenListData (globalScope);
#endif

  // curprint ( string(" /* global scope size = " + globalScope->get_declarations().size() + " */ \n ";

#if 0
     int declarationCounter = 0;
#endif

  // Setup an iterator to go through all the statements in the top scope of the file.
     SgDeclarationStatementPtrList & globalStatementList = globalScope->get_declarations();
     SgDeclarationStatementPtrList::iterator statementIterator = globalStatementList.begin();
     while ( statementIterator != globalStatementList.end() )
        {
          SgStatement* currentStatement = *statementIterator;
          ROSE_ASSERT(currentStatement != NULL);

#if 0
          printf ("In unparseGlobalStmt(): declaration #%d is %p = %s = %s \n",declarationCounter++,currentStatement,currentStatement->class_name().c_str(),SageInterface::get_name(currentStatement).c_str());
#endif

          if (ROSE_DEBUG > 3)
             {
            // (*primary_os)
               cout << "In run_unparser(): getLineNumber(currentStatement) = "
#if 1
                    << currentStatement->get_file_info()->displayString()
#else
                    << ROSE::getLineNumber(currentStatement)
                    << " getFileName(currentStatement) = " 
                    << ROSE::getFileName(currentStatement)
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

       // Go to the next statement
          statementIterator++;
        }

  // DQ (5/27/2005): Added support for compiler-generated statements that might appear at the end of the applications
  // printf ("At end of unparseGlobalStmt \n");
  // outputCompilerGeneratedStatements(info);

  // DQ (4/21/2005): Output a new line at the end of the file (some compilers complain if this is not present)
     unp->cur.insert_newline(1);
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

bool UnparseLanguageIndependentConstructs::unparseLineReplacement(
   SgLocatedNode* stmt,
   SgUnparse_Info& info
    )
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
  info.display("In Unparse_ExprStmt::unparseAttachedPreprocessingInfo()");
#endif

  // Traverse the container of PreprocessingInfo objects
  AttachedPreprocessingInfoType::iterator i;
  for(i = prepInfoPtr->begin(); i != prepInfoPtr->end(); ++i)
  {
    // i ist a pointer to the current prepInfo object, print current preprocessing info
    // Assert that i points to a valid preprocssingInfo object
    ROSE_ASSERT ((*i) != NULL);
    ROSE_ASSERT ((*i)->getTypeOfDirective()  != PreprocessingInfo::CpreprocessorUnknownDeclaration);
    ROSE_ASSERT ((*i)->getRelativePosition() == PreprocessingInfo::before || 
        (*i)->getRelativePosition() == PreprocessingInfo::after  ||
        (*i)->getRelativePosition() == PreprocessingInfo::inside);

#if 0
    printf ("Stored comment: (*i)->getRelativePosition() = %s (*i)->getString() = %s \n",
        ((*i)->getRelativePosition() == PreprocessingInfo::before) ? "before" : "after",
        (*i)->getString().c_str());
#endif

    // Check and see if the info object would indicate that the statement would 
    // be printed, if not then don't print the comments associated with it.
    // These might have to be handled on a case by case basis.
    // bool infoSaysGoAhead = !info.SkipDefinition();
    bool infoSaysGoAhead = !info.SkipEnumDefinition()  &&
      !info.SkipClassDefinition() &&
      !info.SkipFunctionDefinition();

    // DQ (7/19/2008): Allow expressions to have there associated comments unparsed.
    infoSaysGoAhead = (infoSaysGoAhead == true) || (isSgExpression(stmt) != NULL);

#if 0
    printf ("(*i)->getRelativePosition() == whereToUnparse (matches == %s) \n",(*i)->getRelativePosition() == whereToUnparse ? "true" : "false");
    printf ("unp->opt.get_unparse_includes_opt() == %s \n",(unp->opt.get_unparse_includes_opt() == true) ? "true" : "false");
#endif


    switch ( (*i)->getTypeOfDirective() )
    {
      case PreprocessingInfo::LineReplacement:

        if(isSgExpression(stmt) == NULL ) //Do this when line replacement matches a whole statement
          unp->cur.format(stmt, info, FORMAT_BEFORE_DIRECTIVE);

        curprint ( (*i)->getString());
  //      unp->cur.format(stmt, info, FORMAT_AFTER_DIRECTIVE);

        replacedLines++;
        break;
      default:
        break;
    }

  }

  //there should only be one linereplacement directive for a statement
  ROSE_ASSERT(replacedLines <= 1);
  return replacedLines;
};


void
UnparseLanguageIndependentConstructs::unparseAttachedPreprocessingInfo(
// SgStatement* stmt,
   SgLocatedNode* stmt,
   SgUnparse_Info& info,
   PreprocessingInfo::RelativePositionType whereToUnparse)
   {
  // Get atached preprocessing info
     AttachedPreprocessingInfoType *prepInfoPtr= stmt->getAttachedPreprocessingInfo();

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
          printOutComments (stmt);
        }
#endif

     if (prepInfoPtr == NULL)
        {
       // There's no preprocessing info attached to the current statement
       // printf ("No comments or CPP directives associated with this statement ... \n");
          return;
        }

  // If we are skiping BOTH comments and CPP directives then there is nothing to do
     if ( info.SkipComments() && info.SkipCPPDirectives() )
        {
       // There's no preprocessing info attached to the current statement
       // printf ("Skipping output or comments and CPP directives \n");
          return;
        }

#if 0
     info.display("In Unparse_ExprStmt::unparseAttachedPreprocessingInfo()");
#endif

  // Traverse the container of PreprocessingInfo objects
     AttachedPreprocessingInfoType::iterator i;
     for(i = prepInfoPtr->begin(); i != prepInfoPtr->end(); ++i)
        {
       // i ist a pointer to the current prepInfo object, print current preprocessing info
       // Assert that i points to a valid preprocssingInfo object
          ROSE_ASSERT ((*i) != NULL);
          ROSE_ASSERT ((*i)->getTypeOfDirective()  != PreprocessingInfo::CpreprocessorUnknownDeclaration);
          ROSE_ASSERT ((*i)->getRelativePosition() == PreprocessingInfo::before || 
                       (*i)->getRelativePosition() == PreprocessingInfo::after  ||
                       (*i)->getRelativePosition() == PreprocessingInfo::inside);

#if 0
          printf ("Stored comment: (*i)->getRelativePosition() = %s (*i)->getString() = %s \n",
               ((*i)->getRelativePosition() == PreprocessingInfo::before) ? "before" : "after",
               (*i)->getString().c_str());
#endif

       // Check and see if the info object would indicate that the statement would 
       // be printed, if not then don't print the comments associated with it.
       // These might have to be handled on a case by case basis.
       // bool infoSaysGoAhead = !info.SkipDefinition();
          bool infoSaysGoAhead = !info.SkipEnumDefinition()  &&
                                 !info.SkipClassDefinition() &&
                                 !info.SkipFunctionDefinition();

       // DQ (7/19/2008): Allow expressions to have there associated comments unparsed.
       // Liao 11/9/2010: allow SgInitializedName also
          infoSaysGoAhead = (infoSaysGoAhead == true) || (isSgExpression(stmt) != NULL) || (isSgInitializedName (stmt) != NULL) ;

#if 0
          printf ("infoSaysGoAhead = %s \n",infoSaysGoAhead ? "true" : "false");
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

               unp->cur.format(stmt, info, FORMAT_BEFORE_DIRECTIVE);

            // DQ (7/19/2008): If we can assert this, then we can simpleify the code below!
            // It is turned on in the tests/roseTests/programTransformationTests/implicitCodeGenerationTest.C
            // But I still don't know what it does.
            // ROSE_ASSERT(unp->opt.get_unparse_includes_opt() == false);

               if (unp->opt.get_unparse_includes_opt() == true)
                  {
                 // If we are unparsing the include files then we can simplify the 
                 // CPP directive processing and unparse them all as comments!
                 // Comments can also be unparsed as comments (I think!).
                    curprint (  "// " + (*i)->getString());
                  }
                 else
                  {
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
                              if (unp->opt.get_unparse_includes_opt() == true)
                                   curprint ( string("// " ) + (*i)->getString());
                                else
                                   curprint ( (*i)->getString());
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

                 // extern declarations must be handled as comments since 
                 // the EDG frontend strips them away
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
                                   curprint ( (*i)->getString());
                            }
						else
						   curprint ( (*i)->getString());
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
                                   curprint ( string("// (previously processed: ignored) " ) + (*i)->getString());
                                else
                                   curprint ( (*i)->getString());
                            }
                         break;

                    case PreprocessingInfo::CpreprocessorUnknownDeclaration:
                         printf ("Error: CpreprocessorUnknownDeclaration found \n");
                         ROSE_ABORT();
                         break;
                    case PreprocessingInfo::CMacroCall:
                         //AS(1/04/07) Macro rewrapping is currently not supported
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
   }



void
UnparseLanguageIndependentConstructs::unparseUnaryExpr(SgExpression* expr, SgUnparse_Info& info) 
   {
#if 0
     curprint ( "\n /* Inside of unparseUnaryExpr */ \n");
     unp->u_debug->printDebugInfo("entering unparseUnaryExpr", true);
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
     printf ("unary_op->get_mode() != SgUnaryOp::postfix is %s \n",(unary_op->get_mode() != SgUnaryOp::postfix) ? "true" : "false");
     printf ("In Unparse_ExprStmt::unparseUnaryExpr: arrow_op = %d \n",arrow_op);
     printf ("isFunctionType = %s \n",(isFunctionType == true) ? "true" : "false");

     printf ("unary_op->get_operand()->sage_class_name() = %s \n",unary_op->get_operand()->sage_class_name());
     printf ("unary_op->get_type()->sage_class_name() = %s \n",unary_op->get_type()->sage_class_name());
     printf ("info.get_operator_name() = %s \n",info.get_operator_name());
#endif

  // DQ (2/22/2005): Ignoring if this is a SgFunctionType (test ...)
  // Bugfix (2/26/2001) If this is for a function pointer then skip printing out 
  // the operator name (for dereferencing operator)
#if 1
  // if (unary_op->get_mode() != SgUnaryOp::postfix && !arrow_op)
     if (unary_op->get_mode() != SgUnaryOp::postfix)
#else
     if (unary_op->get_mode() != SgUnaryOp::postfix && !arrow_op && !isFunctionType)
#endif
        {
       // curprint ( "\n /* Unparsing a prefix unary operator */ \n";
       // DQ (2/25/2005): Trap case of SgPointerDerefExp so that "*" can't be 
       // turned into "/*" if preceeded by a SgDividOp or overloaded "operator/()"
       // Put in an extra space so that if this happens we only generate "/ *"
       // test2005_09.C demonstrates this bug!
          if (isSgPointerDerefExp(expr) != NULL)
               curprint (  " ");
          curprint ( info.get_operator_name());
        }

     unparseExpression(unary_op->get_operand(), info);

     if (unary_op->get_mode() == SgUnaryOp::postfix && !arrow_op) 
        { 
          curprint (  info.get_operator_name()); 
        }

     info.unset_nested_expression();

#if 0
     curprint ( "\n /* Leaving of unparseUnaryExpr */ \n");
#endif
   }


void
UnparseLanguageIndependentConstructs::unparseBinaryExpr(SgExpression* expr, SgUnparse_Info& info) 
   {
#if 0
      curprint ( string("\n /* Inside of unparseBinaryExpr (operator name = ") + info.get_operator_name() + " */ \n");
      printf ("In unparseBinaryExpr() expr = %s \n",expr->class_name().c_str());
#endif

     unp->u_debug->printDebugInfo("entering unparseBinaryExpr", true);
     SgBinaryOp* binary_op = isSgBinaryOp(expr);
     ROSE_ASSERT(binary_op != NULL);

#if 0
  // printf ("In Unparse_ExprStmt::unparseBinaryExpr() expr = %s \n",expr->sage_class_name());
      curprint ( string ("\n /* Inside of unparseBinaryExpr (expr class name = ") + binary_op->class_name() + " */ \n");
      curprint ( string("\n /*                              lhs class name  = ") + binary_op->get_lhs_operand()->class_name() + " */ \n");
      curprint ( string("\n /*                              rhs class name  = ") + binary_op->get_rhs_operand()->class_name() + " */ \n");
#endif

  // int toplevel_expression = !info.get_nested_expression();
     bool iostream_op = false;

  // Same reasoning above except with parenthesis operator function.
     bool paren_op = false;

  // Same reasoning above except with "this" expression.
     bool this_op = false;

  // Flag to indicate whether the rhs operand is an overloaded arrow operator
  // (to control the printing of parenthesis).
     bool arrow_op = false;

  // Flag to indicate whether the lhs operand contains an overloaded arrow operator
  // to control printing of operator.
     bool overload_arrow = false;

     if (!unp->opt.get_overload_opt() && unp->u_sage->isIOStreamOperator(binary_op->get_rhs_operand()) )
          iostream_op = true;

     if (!unp->opt.get_overload_opt() && unp->u_sage->isBinaryParenOperator(binary_op->get_rhs_operand()) )
          paren_op = true;

     if (!unp->opt.get_this_opt() && isSgThisExp(binary_op->get_lhs_operand()) )
          this_op = true;

     if (!unp->opt.get_overload_opt() && unp->u_sage->isOverloadedArrowOperator(binary_op->get_rhs_operand()) )
          arrow_op = true;

     info.set_nested_expression();

  // [DTdbug] 3/23/2000 -- Trying to figure out why overloaded square bracket
  //          operators are not being handled correctly.
  //
  //          3/30/2000 -- operator[]s have been handled.  See unparseFuncCall().
  //

#if 0
     printf ("In unparseBinaryExp(): unp->opt.get_overload_opt() = %s \n",(unp->opt.get_overload_opt() == true) ? "true" : "false");
     curprint ( string("\n /* unp->opt.get_overload_opt() = ") + ((unp->opt.get_overload_opt() == true) ? "true" : "false") + " */ \n");
     curprint ( string("\n /* arrow_op = ") + resBool(arrow_op) + " */ \n");
     curprint ( string("\n /* this_op = ") + resBool(this_op) + " */ \n");
#endif

     if (info.get_operator_name() == "[]" )
        {
       // Special case:
       // curprint ( "/* Special case of operator[] found */\n";
          unp->u_debug->printDebugInfo("we have special case: []", true);
          unp->u_debug->printDebugInfo("lhs: ", false);
          unparseExpression(binary_op->get_lhs_operand(), info);
          curprint (  "[") ; 
          unp->u_debug->printDebugInfo("rhs: ", false);
          unparseExpression(binary_op->get_rhs_operand(), info);
          curprint (  "]"); 
        }
       else 
        {
#if 0
          curprint ( "/* NOT a special case of operator[] */\n");
          curprint ( string("/* unp->opt.get_overload_opt() = ") + (unp->opt.get_overload_opt() == true ? "true" : "false") + " */\n ");
          unp->opt.display("unparseBinaryExpr()");
#endif

       // Check whether overload option is turned on or off.  If it is off, the conditional is true.
       // Meaning that overloaded operators such as "A.operator+(B)" are output as "A + B".
          if (!unp->opt.get_overload_opt())
             {
            // printf ("overload option is turned off! (output as "A+B" instead of "A.operator+(B)") \n");
            // First check if the right hand side is an unary operator function.
#if 0
               curprint ( string("\n /* output as A+B instead of A.operator+(B): (u_sage->isUnaryOperator(binary_op->get_rhs_operand())) = ") + 
                    ((unp->u_sage->isUnaryOperator(binary_op->get_rhs_operand())) ? "true" : "false") + " */ \n");
#endif
               if (unp->u_sage->isUnaryOperator(binary_op->get_rhs_operand()))
                  {
                 // printf ("Found case of rhs being a unary operator! \n");

                 // Two cases must be considered here: prefix unary and postfix unary 
                 // operators. Most of the unary operators are prefix. In this case, we must
                 // first unparse the rhs and then the lhs.	
                 // if (isUnaryPostfixOperator(binary_op->get_rhs_operand())); // Postfix unary operator.
                    if (unp->u_sage->isUnaryPostfixOperator(binary_op->get_rhs_operand()))  // Postfix unary operator.
                       {
                      // ... nothing to do here (output the operator later!) ???
                      // printf ("... nothing to do here (output the postfix operator later!) \n");
                       }
                      else 
                       {
                      // Prefix unary operator.
                      // printf ("Handle prefix operator ... \n");
                         unp->u_debug->printDebugInfo("prefix unary operator found", true);
                         unp->u_debug->printDebugInfo("rhs: ", false);

                      // printf ("Prefix unary operator: Output the RHS operand ... = %s \n",binary_op->get_rhs_operand()->sage_class_name());
                      // curprint ( "\n /* Prefix unary operator: Output the RHS operand ... */ \n";
                         unparseExpression(binary_op->get_rhs_operand(), info);

                         unp->u_debug->printDebugInfo("lhs: ", false);

                      // DQ (2/22/2005): Treat the operator->() the same as an SgArrowExp IR node
                      // DQ (2/19/2005): When converting to overloaded operator syntax (e.g. "++" instead of "operator++()")
                      // detect the case of pointer dereferencing (from the SgArrowOp) via "x->operator++()" and convert 
                      // to "++(*x)" instead of "*++x".
                      // if ( isSgArrowExp(expr) )

                         bool outputParen = false;
                         SgExpression* lhs = binary_op->get_lhs_operand();
                         ROSE_ASSERT(lhs != NULL);
                         SgConstructorInitializer* constructor = isSgConstructorInitializer(lhs);
                         if (constructor != NULL)
                            {
                              outputParen = true;
                            }

                         if ( outputParen == true )
                            {
                           // curprint ( " /* output paren for constructor intializer */ ";
                              curprint ( "(");
                            }

                         if ( isSgArrowExp(expr) || arrow_op == true )
                            {
                           // printf ("This is an isSgArrowExp operator so dereference the lhs when converting to the dot syntax: opening \n");
                           // curprint ( " /* opening paren for prefix fixup */ ";
                              curprint ( "(*");
                            }

                      // printf ("Prefix unary operator: Output the LHS operand ... = %s \n",binary_op->get_lhs_operand()->sage_class_name());
                      // curprint ( "\n /* Prefix unary operator: Output the LHS operand ... */ \n";
                      // unparseExpression(binary_op->get_lhs_operand(), info);
                         unparseExpression(lhs, info);
                         info.unset_nested_expression();

                      // DQ (2/22/2005): Treat the operator->() the same as an SgArrowExp IR node
                      // if ( isSgArrowExp(expr) )
                         if ( isSgArrowExp(expr) || arrow_op == true )
                            {
                           // printf ("This is an isSgArrowExp operator so dereference the lhs when converting to the dot syntax: closing \n");
                           // curprint ( " /* closing paren for prefix fixup */ ";
                              curprint ( " )");
                            }

                         if ( outputParen == true )
                            {
                              curprint ( ")");
                            }
                      // printf ("Calling \"return\" from this binary operator ... \n");
                      // curprint ( "\n /* Calling \"return\" from this binary operator ... */ \n";
                         return;
                       }
                  }
             }

       // Check if this is a dot expression and the overload option is turned off. If so,
       // we need to handle this differently. Otherwise, skip this section and unparse
       // using the default case below.
       // if (!unp->opt.get_overload_opt() && strcmp(info.get_operator_name(),".")==0) 
          if (!unp->opt.get_overload_opt() && isSgDotExp(expr) != NULL )
             {
#if 0
               printf ("overload option is turned off and this is a SgDotExp! \n");
               if ( !info.SkipComments() || !info.SkipCPPDirectives() )
                  {
                    curprint ( "\n /* ( !unp->opt.get_overload_opt() && isSgDotExp(expr) ) == true */ \n");
                  }
#endif
               overload_arrow = unp->u_sage->NoDereference(binary_op->get_lhs_operand());

            // curprint ( "\n /* overload_arrow = " + overload_arrow + " */ \n";

               SgMemberFunctionRefExp* mfunc_ref = isSgMemberFunctionRefExp(binary_op->get_rhs_operand());

            // curprint ( "\n /* mfunc_ref = " + StringUtility::numberToString(mfunc_ref) + " */ \n";

               unp->u_debug->printDebugInfo("lhs: ", false);

               bool addParensForLhs = false;
               SgExpression* lhs = binary_op->get_lhs_operand();
               ROSE_ASSERT(lhs != NULL);
            // if (isSgBinaryOp(lhs) != NULL || isSgConstructorInitializer(lhs) != NULL )
               SgConstructorInitializer* constructor = isSgConstructorInitializer(lhs);

            // printf ("############## constructor = %p \n",constructor);
               ROSE_ASSERT( (constructor == NULL) || (constructor != NULL && constructor->get_args() != NULL) );

            // Make sure that lhs of SgDotExp include "()" as in: "length = (pos-fpos).magnitude();"
               if (constructor != NULL)
                  {
                 // printf ("constructor->get_args()->get_expressions().size() = %zu \n",constructor->get_args()->get_expressions().size());
                    if (constructor->get_args()->get_expressions().size() > 0)
                       {
                         addParensForLhs = true;
                       }
                  }

            // Make sure that lhs of SgDotExp include "()" as in: "length = (pos-fpos).magnitude();"
               if (mfunc_ref != NULL)
                  {
                 // addParensForLhs = true;
                  }

               if (addParensForLhs == true)
                  {
                 // DQ (2/22/2005): Removed special case from Unparse_ExprStmt::unp->u_sage->PrintStartParen(SgExpression* expr, SgUnparse_Info& info)
                 // curprint ("/* added paren in binary operator */ ( ");
                    curprint ("(");
                  }

            // unparseExpression(binary_op->get_lhs_operand(), info);
               unparseExpression(lhs, info);
               unp->u_debug->printDebugInfo(getSgVariant(expr->variant()), true);

               if (addParensForLhs == true)
                  {
                 // curprint ("/* closing paren */ ) ");
                    curprint (")");
                  }

            // Check if the rhs is not a member function. If so, then it is most likely a  
            // data member of a class. We print the dot in this case.
               if (!mfunc_ref && !overload_arrow)  
                  {
                 // Print out the dot:
                 // curprint ( "\n /* Print out the dot */ \n";
                    curprint ( info.get_operator_name());
                    unp->u_debug->printDebugInfo("printed dot because is not member function", true);
                  }
                 else
                  {
                 // Now check if this member function is an operator overloading function. If it
                 // is, then we don't print the dot. If not, then print the dot.
#if 0
                    curprint ( "\n /* Print out the dot: Now check if this member function is an operator overloading function */ \n");
                    if (mfunc_ref != NULL)
                         curprint ( string("\n /* isOperator(mfunc_ref) = ") + ((unp->u_sage->isOperator(mfunc_ref) == true) ? "true" : "false") + " */ \n");
#endif
                 // DQ (12/11/2004): Added assertion to catch case of "a.operator->();"
                 // ROSE_ASSERT(mfunc_ref != NULL);
                 // if (!isOperator(mfunc_ref) && !overload_arrow)
//                  if ( (mfunc_ref != NULL) && !isOperator(mfunc_ref) && !overload_arrow)
//                  if ( !isOperator(mfunc_ref) && !overload_arrow)
//                  if ( overload_arrow == true )
//                  if ( true )
//                  if ( !isOperator(mfunc_ref) )
//                  if ( (mfunc_ref != NULL) && !isOperator(mfunc_ref) )
                    if ( (mfunc_ref == NULL) || !unp->u_sage->isOperator(mfunc_ref) )
                       {
                      // curprint ( "\n /* Print out the dot in second case */ \n";
                         curprint ( info.get_operator_name());
                         unp->u_debug->printDebugInfo("printed dot because is not operator overloading function", true);
                       }
                  }
             }
         // else if (!unp->opt.get_this_opt() && strcmp(info.get_operator_name(),"->")==0) 
            else
             {
            // printf ("overload option is turned on! \n");
            // Check if this is an arrow expression and the "this" option is turned off. If 
            // so, we need to handle this differently. Otherwise, skip this section and 
            // unparse using the default case below.

            // curprint ( "\n /* ( !unp->opt.get_overload_opt() && isSgDotExp(expr) ) == false */ \n";
#if 0
               curprint ( string("\n /* ( !unp->opt.get_overload_opt() && isSgArrowExp(expr) ) = ") + 
                    ((!unp->opt.get_this_opt() && isSgArrowExp(expr)) ? "true" : "false") + " */ \n");
#endif
               if ( !unp->opt.get_this_opt() && isSgArrowExp(expr) ) 
                  {

                 // This is a special case to check for. We are now checking for arrow 
                 // expressions with the overload operator option off. If so, that means that 
                 // we cannot print "operator", which means that printing "->" is wrong. So we 
                 // must dereference the variable and suppress the printing of "->". Jump to
                 // "dereference" is true.
                    if (!unp->opt.get_overload_opt() && unp->u_sage->isOperator(binary_op->get_rhs_operand()))
                       goto dereference;

                    unp->u_debug->printDebugInfo("lhs: ", false);
                    unparseExpression(binary_op->get_lhs_operand(), info);
                    unp->u_debug->printDebugInfo(getSgVariant(expr->variant()), true);

                 // Check if the lhs is a this expression. If so, then don't print the arrow. 
                    if (!isSgThisExp(binary_op->get_lhs_operand())) 
                       {
                      // Is not "this" exp, so print the arrow.
                         curprint ( info.get_operator_name());
                       }
                  }
              // else if (!unp->opt.get_overload_opt() && strcmp(info.get_operator_name(),"->")==0 && isOperator(binary_op->get_rhs_operand())) 
                 else 
                  {
                 // We must also check the special case here mentioned above since the "this" 
                 // option may be true, but the overload option is false.
                    if (!unp->opt.get_overload_opt() && isSgArrowExp(expr) && unp->u_sage->isOperator(binary_op->get_rhs_operand())) 
                       {
                         dereference:

                      // Before dereferencing, first check if the function is preceded by a class
                      // name. If not, then dereference.

                         if (unp->u_sage->noQualifiedName(binary_op->get_rhs_operand())) 
                            {
                              curprint ( "(*"); 
                              unp->u_debug->printDebugInfo("lhs: ", false);
                              unparseExpression(binary_op->get_lhs_operand(), info);
                              unp->u_debug->printDebugInfo(getSgVariant(expr->variant()), true);
                              curprint ( ")");
                            }
                       }
                      else 
                       {
                      // We reach this if the options were on, or if this was not a dot, arrow, or unary
                      // prefix expression. This is the default case.

                         unp->u_debug->printDebugInfo("lhs: ", false);
                      // curprint ( "/* lhs = " + binary_op->get_lhs_operand()->class_name() + " */\n ");
                         unparseExpression(binary_op->get_lhs_operand(), info);
                      // curprint ( "/* DONE: lhs = " + binary_op->get_lhs_operand()->class_name() + " */\n ");
                         unp->u_debug->printDebugInfo(getSgVariant(expr->variant()), true);

                      // Before checking to insert a newline to prevent linewrapping, check that this
                      // expression is a primitive operator and not dot or arrow expressions.
                         curprint ( string(" ") + info.get_operator_name() + " ");
                       }
                  }
             }

       // DQ (2/9/2010): Shouldn't this be true (it should also return a bool type).
          ROSE_ASSERT(info.get_nested_expression() != 0);
#if 0
          printf ("In unparseBinaryExpr() -- before output of RHS: info.get_nested_expression() = %d info.get_operator_name() = %s \n",info.get_nested_expression(),info.get_operator_name().c_str());
#endif
          SgExpression* rhs = binary_op->get_rhs_operand();
          if (info.get_operator_name() == ",")
             {
               SgUnparse_Info newinfo(info);
               newinfo.set_inRhsExpr();
               unp->u_debug->printDebugInfo("rhs: ", false);
            // unparseExpression(binary_op->get_rhs_operand(), newinfo);
               unparseExpression(rhs, newinfo);
             }
            else
             {
            // unparseExpression(binary_op->get_rhs_operand(), info);
               unparseExpression(rhs, info);
             }
        }

     info.unset_nested_expression();

#if 0
     curprint ( "\n /* Leaving unparseBinaryExpr */ \n");
#endif
   }


void
UnparseLanguageIndependentConstructs::unparseValue(SgExpression* expr, SgUnparse_Info& info)
   {
  // DQ (11/9/2005): refactored handling of expression trees stemming from the folding of constants.
     SgValueExp* valueExp = isSgValueExp(expr);

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

               default:
                  {
                    printf ("Default reached in switch statement valueExp = %p = %s \n",valueExp,valueExp->class_name().c_str());
                    ROSE_ASSERT(false);
                  }
                
             }
        }
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
     if (file == NULL)
        {
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
               printf ("templateArgument->get_templateInstantiation() = %p \n",templateArgument->get_templateInstantiation());
             }
            else
             {
               ROSE_ASSERT(expr->get_parent()->get_file_info() != NULL);
               expr->get_parent()->get_file_info()->display("location of problem bool expression (parent)");
             }
#endif
          C_language_support = true;
        }
       else
        {
          ROSE_ASSERT(file != NULL);
          C_language_support = file->get_C_only() || file->get_C99_only();
        }

     if (unp->opt.get_num_opt() || (C_language_support == true))
        {
       // we want to print the boolean values as numerical values
       // DQ (11/29/2009): get_value() returns an "int" and MSVC issues a warning for mixed type predicates.
       // if (bool_val->get_value() == true)
          if (bool_val->get_value() != 0)
             {
               curprint ( "1");
             }
            else
             {
               curprint (  "0");
             }
        }
       else
        {
       // print them as "true" or "false"
       // DQ (11/29/2009): get_value() returns an "int" and MSVC issues a warning for mixed type predicates.
       // if (bool_val->get_value() == true)
          if (bool_val->get_value() != 0)
             {
               curprint (  "true");
             }
            else
             {
               curprint (  "false");
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

  // printf ("In Unparse_ExprStmt::unparseEnumVal: info.inEnumDecl() = %s \n",info.inEnumDecl() ? "true" : "false");
  // curprint ( "\n/* In Unparse_ExprStmt::unparseEnumVal: info.inEnumDecl() = " + (info.inEnumDecl() ? "true" : "false") + " */\n ";

  // todo: optimize this so that the qualified name is only printed when necessary.
     if (info.inEnumDecl() == true)
        {
       // The enum value output in the enum declaration should be a value (it could be an enum constant
       // if it has already been output as a value (repreated reference) but this is an obsure detail).
          curprint ( tostring(enum_val->get_value()));
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
#if 1
                 // DQ (12/22/2006): This is use the information that qualification is required. This will trigger the use of 
                 // global qualification even if it is not required with normal qualification.  That is that the specification 
                 // of qualification triggers possible (likely) over qualification.  Overqualification is generally the default
                 // this flag is sometime taken to mean that the "::" is required as well.

                 // printf ("enum_val->get_requiresNameQualification() = %s \n",enum_val->get_requiresNameQualification() ? "true" : "false");
                 // cur << "\n/* funcdecl_stmt->get_requiresNameQualificationOnReturnType() = " << (funcdecl_stmt->get_requiresNameQualificationOnReturnType() ? "true" : "false") << " */ \n";
                    if (enum_val->get_requiresNameQualification() == true)
                       {
                      // Note that general qualification of types is separated from the use of globl qualification.
                      // info.set_forceQualifiedNames();
                         info.set_requiresGlobalNameQualification();
                       }
#endif
                    SgName nameQualifier = unp->u_name->generateNameQualifier(enum_val->get_declaration(),info);
                 // printf ("variable's nameQualifier = %s \n",(nameQualifier.is_null() == false) ? nameQualifier.str() : "NULL");
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

       // printf ("In Unparse_ExprStmt::unparseEnumVal: classdefn = %s pointer \n",classdefn ? "VALID" : "NULL");

       // DQ (6/18/2006): Identify the case of an un-named enum, would be an error if we unparsed this directly.
          ROSE_ASSERT (enum_val->get_name().is_null() == false);

          curprint (  enum_val->get_name().str());
        }
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
          curprint ( tostring(int_val->get_value()));
        }
       else
        {
          curprint ( int_val->get_valueString());
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
          curprint ( tostring(uint_val->get_value()));
        }
       else
        {
          curprint ( uint_val->get_valueString());
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
          curprint ( tostring(longint_val->get_value()));
        }
       else
        {
          curprint ( longint_val->get_valueString());
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
          curprint ( tostring(longlongint_val->get_value()));
        }
       else
        {
          curprint ( longlongint_val->get_valueString());
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
          curprint ( tostring(ulonglongint_val->get_value()));
        }
       else
        {
          curprint ( ulonglongint_val->get_valueString());
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
          curprint ( tostring(ulongint_val->get_value()));
        }
       else
        {
          curprint ( ulongint_val->get_valueString());
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
               curprint ( tostring(float_val->get_value()));
             }
            else
             {
               curprint ( float_val->get_valueString());
             }
        }
       else
        {
        }
#endif

     if (float_value == std::numeric_limits<float>::infinity())
        {
       // printf ("Infinite value found as value in unparseFloatVal() \n");
       // curprint ( "std::numeric_limits<float>::infinity()";
          curprint ( "__builtin_huge_valf()");
        }
       else
        {
       // Test for NaN value (famous test of to check for equality) or check for C++ definition of NaN.
       // We detect C99 and C "__NAN__" in EDG, but translate to backend specific builtin function.
          if ((float_value != float_value) || (float_value == std::numeric_limits<float>::quiet_NaN()) )
             {
            // curprint ( "std::numeric_limits<float>::quiet_NaN()";
               curprint ( "__builtin_nanf (\"\")");
             }
            else
             {
               if (float_value == std::numeric_limits<float>::signaling_NaN())
                  {
                 // curprint ( "std::numeric_limits<float>::signaling_NaN()";
                    curprint ( "__builtin_nansf (\"\")");
                  }
                 else
                  {
                 // typical case!
                 // curprint ( float_val->get_value();
                 // AS (11/08/2005) add support for values as string
                    if (float_val->get_valueString() == "")
                       {
                         curprint ( tostring(float_val->get_value()));
                       }
                      else
                       {
                         curprint ( float_val->get_valueString());
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
                         curprint ( tostring(double_value));
                      else
                         curprint ( dbl_val->get_valueString());
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


void
UnparseLanguageIndependentConstructs::unparseExprList(SgExpression* expr, SgUnparse_Info& info)
   {
     SgExprListExp* expr_list = isSgExprListExp(expr);
     ROSE_ASSERT(expr_list != NULL);
  /* code inserted from specification */
  
     SgExpressionPtrList::iterator i = expr_list->get_expressions().begin();

     if (i != expr_list->get_expressions().end())
        {
          while (true)
             {
               SgUnparse_Info newinfo(info);
               newinfo.set_SkipBaseType();
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
     curprint(directive->get_directiveString());
  // unp->u_sage->curprint_newline();
     unp->cur.insert_newline(1);
   }

void 
UnparseLanguageIndependentConstructs::unparseDefineDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgDefineDirectiveStatement* directive = isSgDefineDirectiveStatement(stmt);
     ROSE_ASSERT(directive != NULL);
     curprint(directive->get_directiveString());
  // unp->u_sage->curprint_newline();
     unp->cur.insert_newline(1);
   }

void 
UnparseLanguageIndependentConstructs::unparseUndefDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgUndefDirectiveStatement* directive = isSgUndefDirectiveStatement(stmt);
     ROSE_ASSERT(directive != NULL);
     curprint(directive->get_directiveString());
     unp->u_sage->curprint_newline();
   }

void 
UnparseLanguageIndependentConstructs::unparseIfdefDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgIfdefDirectiveStatement* directive = isSgIfdefDirectiveStatement(stmt);
     ROSE_ASSERT(directive != NULL);
     curprint(directive->get_directiveString());
     unp->u_sage->curprint_newline();
   }

void 
UnparseLanguageIndependentConstructs::unparseIfndefDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgIfndefDirectiveStatement* directive = isSgIfndefDirectiveStatement(stmt);
     ROSE_ASSERT(directive != NULL);
     curprint(directive->get_directiveString());
     unp->u_sage->curprint_newline();
   }

void 
UnparseLanguageIndependentConstructs::unparseDeadIfDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgDeadIfDirectiveStatement* directive = isSgDeadIfDirectiveStatement(stmt);
     ROSE_ASSERT(directive != NULL);
     curprint(directive->get_directiveString());
     unp->u_sage->curprint_newline();
   }

void 
UnparseLanguageIndependentConstructs::unparseIfDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgIfDirectiveStatement* directive = isSgIfDirectiveStatement(stmt);
     ROSE_ASSERT(directive != NULL);
     curprint(directive->get_directiveString());
     unp->u_sage->curprint_newline();
   }

void 
UnparseLanguageIndependentConstructs::unparseElseDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgElseDirectiveStatement* directive = isSgElseDirectiveStatement(stmt);
     ROSE_ASSERT(directive != NULL);
     curprint(directive->get_directiveString());
     unp->u_sage->curprint_newline();
   }

void 
UnparseLanguageIndependentConstructs::unparseElseifDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgElseifDirectiveStatement* directive = isSgElseifDirectiveStatement(stmt);
     ROSE_ASSERT(directive != NULL);
     curprint(directive->get_directiveString());
     unp->u_sage->curprint_newline();
   }

void 
UnparseLanguageIndependentConstructs::unparseEndifDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgEndifDirectiveStatement* directive = isSgEndifDirectiveStatement(stmt);
     ROSE_ASSERT(directive != NULL);
     curprint(directive->get_directiveString());
     unp->u_sage->curprint_newline();
   }

void 
UnparseLanguageIndependentConstructs::unparseLineDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgLineDirectiveStatement* directive = isSgLineDirectiveStatement(stmt);
     ROSE_ASSERT(directive != NULL);
     curprint(directive->get_directiveString());
     unp->u_sage->curprint_newline();
   }

void 
UnparseLanguageIndependentConstructs::unparseWarningDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgWarningDirectiveStatement* directive = isSgWarningDirectiveStatement(stmt);
     ROSE_ASSERT(directive != NULL);
     curprint(directive->get_directiveString());
     unp->u_sage->curprint_newline();
   }

void 
UnparseLanguageIndependentConstructs::unparseErrorDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgErrorDirectiveStatement* directive = isSgErrorDirectiveStatement(stmt);
     ROSE_ASSERT(directive != NULL);
     curprint(directive->get_directiveString());
     unp->u_sage->curprint_newline();
   }

void 
UnparseLanguageIndependentConstructs::unparseEmptyDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgEmptyDirectiveStatement* directive = isSgEmptyDirectiveStatement(stmt);
     ROSE_ASSERT(directive != NULL);
     curprint(directive->get_directiveString());
     unp->u_sage->curprint_newline();
   }

void 
UnparseLanguageIndependentConstructs::unparseIdentDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgIdentDirectiveStatement* directive = isSgIdentDirectiveStatement(stmt);
     ROSE_ASSERT(directive != NULL);
     curprint(directive->get_directiveString());
     unp->u_sage->curprint_newline();
   }

void 
UnparseLanguageIndependentConstructs::unparseIncludeNextDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgIncludeNextDirectiveStatement* directive = isSgIncludeNextDirectiveStatement(stmt);
     ROSE_ASSERT(directive != NULL);
     curprint(directive->get_directiveString());
     unp->u_sage->curprint_newline();
   }

void 
UnparseLanguageIndependentConstructs::unparseLinemarkerDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info)
   {
     SgLinemarkerDirectiveStatement* directive = isSgLinemarkerDirectiveStatement(stmt);
     ROSE_ASSERT(directive != NULL);
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
      break;
  }    
  curprint(string(")"));
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
        cerr<<"Error: unhandled operator type ReductionOperatorToString():"<< ro <<endl;
        ROSE_ASSERT(false);
      }
  }
  return result;
}
#endif

//! Unparse an OpenMP clause with a variable list
void UnparseLanguageIndependentConstructs::unparseOmpVariablesClause(SgOmpClause* clause, SgUnparse_Info& info)
{
  ROSE_ASSERT(clause != NULL);
  SgOmpVariablesClause* c= isSgOmpVariablesClause (clause);  
  ROSE_ASSERT(c!= NULL);
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
    case V_SgOmpReductionClause:
      {
        curprint(string(" reduction("));
        //reductionOperatorToString() will handle language specific issues 
        curprint(reductionOperatorToString(isSgOmpReductionClause(c)->get_operation()));
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

  //unparse variable list then
  SgVarRefExpPtrList::iterator p = c->get_variables().begin();
  while ( p != c->get_variables().end() )
  {
    SgInitializedName* init_name = (*p)->get_symbol()->get_declaration();           
    SgName tmp_name  = init_name->get_name();
    curprint( tmp_name.str());

    // Move to the next argument
    p++;

    // Check if this is the last argument (output a "," separator if not)
    if (p != c->get_variables().end())
    {
      curprint( ",");
    }
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
  if (isSgOmpCollapseClause(c))
    curprint(string(" collapse("));
  else if (isSgOmpIfClause(c))
    curprint(string(" if("));
  else if (isSgOmpNumThreadsClause(c))
    curprint(string(" num_threads("));
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
    case V_SgOmpNowaitClause:
      {
        curprint(string(" nowait"));
        break;
      }
    case V_SgOmpOrderedClause:
      {
        curprint(string(" ordered"));
        break;
      }
    case V_SgOmpUntiedClause:
      {
        curprint(string(" untied"));
        break;
      }
    case V_SgOmpScheduleClause:
      {
        unparseOmpScheduleClause(isSgOmpScheduleClause(clause), info);
        break;
      }
    case V_SgOmpCollapseClause:
    case V_SgOmpIfClause:  
    case V_SgOmpNumThreadsClause:  
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
    case V_SgOmpSharedClause:
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
    ROSE_ASSERT (stmt->variantT() == V_SgOmpAtomicStatement || stmt->variantT() == V_SgOmpSectionStatement);
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


