#include "sage3basic.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

#include "fortran_support.h"

// FMZ: Location of global variables
#include "FortranParserState.h"

SgSourceFile* OpenFortranParser_globalFilePointer = NULL;

using namespace std;


std::list<SgInterfaceStatement*> astInterfaceStack;

#include "token.h"

Token_t *create_token(int line, int col, int type, const char *text)
  {
	 Token_t *tmp_token = NULL;

	 tmp_token = (Token_t*) malloc(sizeof(Token_t));
	 tmp_token->line = line;
	 tmp_token->col = col;
	 tmp_token->type = type;
 /* Make a copy of our own to make sure it isn't freed on us.  */
    if (text != NULL)
         tmp_token->text = strdup(text);
      else
         tmp_token->text = NULL;

	 return tmp_token;
  }


string
getCurrentFilename()
   {
  // DQ (12/18/2008): Added comment:  This function supports the Fortran "include" 
  // mechanism and is independent of the CPP specific "#include" mechanism.

     string filename;
  // ROSE_ASSERT (astIncludeStack.empty() == false);

     if (astIncludeStack.size() <= 1)
        {
       // Note that the original source file in OFP does not have to use an absolute file name so use the one from ROSE.
          filename = OpenFortranParser_globalFilePointer->get_sourceFileNameWithPath();

          if (OpenFortranParser_globalFilePointer->get_requires_C_preprocessor() == true)
             {
            // This source file requires CPP processing, so this would be the generated file with the "_preprocessed.f*" suffix.
               filename = OpenFortranParser_globalFilePointer->generate_C_preprocessor_intermediate_filename(filename);

            // printf ("##### Using filename = %s for the name in the file_info for a CPP generated file \n",filename.c_str());
             }
        }
       else
        {
       // If the astIncludeStack.size() > 1 then we are in an include file and we need to get the filename so that 
       // we can associate the source position with the correct file.
          filename = astIncludeStack.back();
        }

     return filename;
   }

void
setSourcePosition( SgLocatedNode* locatedNode )
   {
  // This function sets the source position to be marked as not available (since we often don't have token information)
  // These nodes WILL be unparsed in the code generation phase.

  // The SgLocatedNode has both a startOfConstruct and endOfConstruct source position.
     ROSE_ASSERT(locatedNode != NULL);

  // Make sure we never try to reset the source position of the global scope (set elsewhere in ROSE).
     ROSE_ASSERT(isSgGlobal(locatedNode) == NULL);

  // Check the endOfConstruct first since it is most likely NULL (helpful in debugging)
     ROSE_ASSERT(locatedNode->get_endOfConstruct()   == NULL);
     ROSE_ASSERT(locatedNode->get_startOfConstruct() == NULL);

  // Call a mechanism defined in the SageInterface support
     SageInterface::setSourcePosition(locatedNode);
   }

void
setSourceEndPosition  ( SgLocatedNode* locatedNode, Token_t* token )
   {
     string filename = getCurrentFilename();
     locatedNode->set_endOfConstruct(new Sg_File_Info(filename,token->line, token->col));
     locatedNode->get_endOfConstruct()->set_parent(locatedNode);
   }

void
setSourcePositionCompilerGenerated( SgLocatedNode* locatedNode )
   {
  // This function sets the source position to be marked as compiler generated.

  // The SgLocatedNode has both a startOfConstruct and endOfConstruct source position.
     ROSE_ASSERT(locatedNode != NULL);

  // Check the endOfConstruct first since it is most likely NULL (helpful in debugging)
     ROSE_ASSERT(locatedNode->get_endOfConstruct()   != NULL);
     ROSE_ASSERT(locatedNode->get_startOfConstruct() != NULL);

     locatedNode->get_startOfConstruct()->setCompilerGenerated();
     locatedNode->get_endOfConstruct()->setCompilerGenerated();
   }

void
setSourcePosition( SgInitializedName* initializedName )
   {
  // This function sets the source position to be marked as not available (since we often don't have token information)
  // These nodes WILL be unparsed in the conde generation phase.

  // Specifically, these will be marked as:
  //    1) isSourcePositionUnavailableInFrontend = true
  //    2) isOutputInCodeGeneration = true

  // The SgInitializedName only has a startOfConstruct source position.
     ROSE_ASSERT(initializedName != NULL);
  // ROSE_ASSERT(initializedName->get_startOfConstruct() == NULL);
  // initializedName->set_startOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  // initializedName->set_startOfConstruct(Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode());

     if (initializedName->get_startOfConstruct() == NULL)
        {
          Sg_File_Info* fileInfo = Sg_File_Info::generateDefaultFileInfo();
          fileInfo->setSourcePositionUnavailableInFrontend();

       // This is required for the unparser to output the code from the AST.
          fileInfo->setOutputInCodeGeneration();

          initializedName->set_startOfConstruct(fileInfo);

          initializedName->get_startOfConstruct()->set_parent(initializedName);
       
        }
       else
        {
          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
               printf ("Unnecessary call to setSourcePosition(SgInitializedName = %p = %s ) for start info. \n",initializedName,SageInterface::get_name(initializedName).c_str());
        }

       // Liao 11/5/2010, add end file info
     if (initializedName->get_endOfConstruct() == NULL)
        {
          Sg_File_Info* fileInfo = Sg_File_Info::generateDefaultFileInfo();
          fileInfo->setSourcePositionUnavailableInFrontend();

       // This is required for the unparser to output the code from the AST.
          fileInfo->setOutputInCodeGeneration();

          initializedName->set_endOfConstruct(fileInfo);

          initializedName->get_endOfConstruct()->set_parent(initializedName);
        }
       else
        {
          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
               printf ("Unnecessary call to setSourcePosition(SgInitializedName = %p = %s ) for end info. \n",initializedName,SageInterface::get_name(initializedName).c_str());
        }


   }

void
setSourcePosition( SgLocatedNode* locatedNode, const TokenListType & tokenList )
   {
  // printf ("In setSourcePosition locatedNode = %p = %s tokenList.size() = %ld \n",locatedNode,locatedNode->class_name().c_str(),tokenList.size());

  // The SgLocatedNode has both a startOfConstruct and endOfConstruct source position.
     ROSE_ASSERT(locatedNode != NULL);

  // Get the first and last tokens from the token list (information about middle tokens 
  // is held in the token strean to be attached to the AST).
     const Token_t* firstToken = tokenList.front();
     const Token_t* lastToken  = tokenList.back();

     ROSE_ASSERT(firstToken != NULL);
     ROSE_ASSERT(lastToken  != NULL);

     ROSE_ASSERT(firstToken->text != NULL);
     ROSE_ASSERT(lastToken->text  != NULL);

  // printf ("firstToken->text = %s \n",firstToken->text);
  // printf ("lastToken->text  = %s \n",lastToken->text);

     ROSE_ASSERT(firstToken->line > 0);
     ROSE_ASSERT(lastToken->line  > 0);

  // This is required to handle both the Fortran specific "include" files and the CPP specific "#include" files.
     string filename = getCurrentFilename();

     if ( SgProject::get_verbose() > 0 )
          printf ("In setSourcePosition(%p = %s) line = %d column = %d filename = %s \n",locatedNode,locatedNode->class_name().c_str(),firstToken->line,firstToken->col,filename.c_str());

     ROSE_ASSERT(filename.empty() == false);

  // DQ (1/23/2008): New assertions
     if (locatedNode->get_startOfConstruct() != NULL)
        {
          printf ("WARNING: removing predefined START Sg_File_Info object in locatedNode = %p = %s = %s \n",locatedNode,locatedNode->class_name().c_str(),SageInterface::get_name(locatedNode).c_str());
          delete locatedNode->get_startOfConstruct();
          locatedNode->set_startOfConstruct(NULL);
        }

  // DQ (1/23/2008): New assertions
     if (locatedNode->get_endOfConstruct() != NULL)
        {
          printf ("WARNING: removing predefined END Sg_File_Info object in locatedNode = %p = %s = %s \n",locatedNode,locatedNode->class_name().c_str(),SageInterface::get_name(locatedNode).c_str());
          delete locatedNode->get_endOfConstruct();
          locatedNode->set_endOfConstruct(NULL);
        }

  // DQ (1/23/2008): New assertions
     ROSE_ASSERT(locatedNode->get_startOfConstruct() == NULL);
     ROSE_ASSERT(locatedNode->get_endOfConstruct()   == NULL);

  // Set these based on the source position information from the tokens
     locatedNode->set_startOfConstruct (new Sg_File_Info(filename,firstToken->line,firstToken->col));
     locatedNode->set_endOfConstruct   (new Sg_File_Info(filename,lastToken->line, lastToken->col));

     locatedNode->get_startOfConstruct()->set_parent(locatedNode);
     locatedNode->get_endOfConstruct  ()->set_parent(locatedNode);
   }

void
setSourcePosition  ( SgInitializedName* initializedName, Token_t* token )
   {
     if ( SgProject::get_verbose() > 1 )
          printf ("In setSourcePosition initializedName = %p = %s token = %p line = %d \n",initializedName,initializedName->get_name().str(),token,token != NULL ? token->line : -1);

  // The SgLocatedNode has both a startOfConstruct and endOfConstruct source position.
     ROSE_ASSERT(initializedName != NULL);

     ROSE_ASSERT(token != NULL);

     ROSE_ASSERT(token->line > 0);

  // This is required to handle both the Fortran specific "include" files and the CPP specific "#include" files.
     string filename = getCurrentFilename();

     if ( SgProject::get_verbose() > 0 )
          printf ("In setSourcePosition(SgInitializedName %p = %s) line = %d column = %d filename = %s \n",initializedName,initializedName->get_name().str(),token->line,token->col,filename.c_str());

     ROSE_ASSERT(filename.empty() == false);

  // Set these based on the source position information from the tokens
     initializedName->set_startOfConstruct (new Sg_File_Info(filename,token->line,token->col));

     initializedName->get_startOfConstruct()->set_parent(initializedName);

   // Liao 11/5/2010, assuming the end info should be the same for now
     initializedName->set_endOfConstruct (new Sg_File_Info(filename,token->line,token->col));

     initializedName->get_endOfConstruct()->set_parent(initializedName);
   
   }

void
setSourcePosition  ( SgLocatedNode* locatedNode, Token_t* token )
   {
     if ( SgProject::get_verbose() > 0 )
        {
       // DQ (1/26/2009): Output additional information as to what file we are in now that we support
       // more complex file include handling and we are testing more complex multi-file support.
       // printf ("In setSourcePosition locatedNode = %p = %s token = %p line = %d \n",locatedNode,locatedNode->class_name().c_str(),token,token != NULL ? token->line : -1);
          printf ("In setSourcePosition locatedNode = %p = %s token = %p (file = %s) line = %d \n",locatedNode,locatedNode->class_name().c_str(),token,getCurrentFilename().c_str(),token != NULL ? token->line : -1);
        }

  // The SgLocatedNode has both a startOfConstruct and endOfConstruct source position.
     ROSE_ASSERT(locatedNode != NULL);

     ROSE_ASSERT(token != NULL);

  // DQ (12/11/2007): Modified to permit tokens to be built (as in R1219)
     ROSE_ASSERT(token->line > 0);
  // ROSE_ASSERT(token->line >= 0);

     if (locatedNode->get_startOfConstruct() != NULL)
        {
          printf ("WARNING: removing predefined START Sg_File_Info object in locatedNode = %p = %s = %s \n",locatedNode,locatedNode->class_name().c_str(),SageInterface::get_name(locatedNode).c_str());
          delete locatedNode->get_startOfConstruct();
          locatedNode->set_startOfConstruct(NULL);
        }

     if (locatedNode->get_endOfConstruct() != NULL)
        {
          printf ("WARNING: removing predefined END Sg_File_Info object in locatedNode = %p = %s = %s \n",locatedNode,locatedNode->class_name().c_str(),SageInterface::get_name(locatedNode).c_str());
          delete locatedNode->get_endOfConstruct();
          locatedNode->set_endOfConstruct(NULL);
        }

  // DQ (1/23/2008): New assertions
     ROSE_ASSERT(locatedNode->get_startOfConstruct() == NULL);
     ROSE_ASSERT(locatedNode->get_endOfConstruct() == NULL);

  // This is required to handle both the Fortran specific "include" files and the CPP specific "#include" files.
     string filename = getCurrentFilename();

     if ( SgProject::get_verbose() > 0 )
          printf ("In setSourcePosition(SgInitializedName %p = %s) line = %d column = %d filename = %s \n",locatedNode,locatedNode->class_name().c_str(),token->line,token->col,filename.c_str());

     ROSE_ASSERT(filename.empty() == false);

  // Set these based on the source position information from the tokens
     locatedNode->set_startOfConstruct (new Sg_File_Info(filename,token->line,token->col));
     locatedNode->set_endOfConstruct   (new Sg_File_Info(filename,token->line, token->col));

     locatedNode->get_startOfConstruct()->set_parent(locatedNode);
     locatedNode->get_endOfConstruct  ()->set_parent(locatedNode);
   }

void
setSourcePosition  ( SgInitializedName* initializedName, const TokenListType & tokenList )
   {
  // printf ("In setSourcePosition initializedName = %p = %s tokenList.size() = %ld \n",initializedName,initializedName->get_name().str(),tokenList.size());

  // The SgLocatedNode has both a startOfConstruct and endOfConstruct source position.
     ROSE_ASSERT(initializedName != NULL);

  // Get the first and last tokens from the token list (information about middle tokens 
  // is held in the token strean to be attached to the AST).
  // const SgToken* firstToken = tokenList.front();
  // const SgToken* lastToken  = tokenList.back();
     const Token_t* firstToken = tokenList.front();
     const Token_t* lastToken  = tokenList.back();

     ROSE_ASSERT(firstToken != NULL);
     ROSE_ASSERT(lastToken  != NULL);

     ROSE_ASSERT(firstToken->line > 0);
     ROSE_ASSERT(lastToken->line  > 0);

  // This is required to handle both the Fortran specific "include" files and the CPP specific "#include" files.
     string filename = getCurrentFilename();

     if ( SgProject::get_verbose() > 0 )
          printf ("In setSourcePosition(SgInitializedName %p = %s) line = %d column = %d filename = %s \n",initializedName,initializedName->get_name().str(),firstToken->line,firstToken->col,filename.c_str());

     ROSE_ASSERT(filename.empty() == false);

  // Set these based on the source position information from the tokens
  // locatedNode->set_startOfConstruct (new Sg_File_Info(*(firstToken->get_startOfConstruct())));
  // locatedNode->set_endOfConstruct   (new Sg_File_Info(*(lastToken ->get_endOfConstruct  ())));
     initializedName->set_startOfConstruct (new Sg_File_Info(filename,firstToken->line,firstToken->col));

     initializedName->get_startOfConstruct()->set_parent(initializedName);
   }


void
setOperatorSourcePosition  ( SgExpression* expr, Token_t* token )
   {
  // printf ("In setOperatorSourcePosition expression = %p = %s token = %p \n",expr,expr->class_name().c_str(),token);

  // The SgLocatedNode has both a startOfConstruct and endOfConstruct source position.
     ROSE_ASSERT(expr != NULL);

     ROSE_ASSERT(token != NULL);

     ROSE_ASSERT(token->line > 0);

  // This is required to handle both the Fortran specific "include" files and the CPP specific "#include" files.
     string filename = getCurrentFilename();

     if ( SgProject::get_verbose() > 0 )
          printf ("In setOperatorSourcePosition(SgExpression %p = %s) line = %d column = %d filename = %s \n",expr,expr->class_name().c_str(),token->line,token->col,filename.c_str());

     ROSE_ASSERT(filename.empty() == false);

  // Set these based on the source position information from the operator token
     expr->set_operatorPosition (new Sg_File_Info(filename,token->line,token->col));

     expr->get_operatorPosition()->set_parent(expr);
   }



void
resetSourcePosition( SgLocatedNode* locatedNode, const TokenListType & tokenList )
   {
  // printf ("In resetSourcePosition locatedNode = %p = %s tokenList.size() = %ld \n",locatedNode,locatedNode->class_name().c_str(),tokenList.size());

  // The SgLocatedNode has both a startOfConstruct and endOfConstruct source position.
     ROSE_ASSERT(locatedNode != NULL);

     ROSE_ASSERT(locatedNode->get_startOfConstruct() != NULL);
     ROSE_ASSERT(locatedNode->get_endOfConstruct() != NULL);

  // Remove the existing Sg_File_Info objects, they will be reset below
     delete locatedNode->get_startOfConstruct();
     delete locatedNode->get_endOfConstruct();

  // Get the first and last tokens from the token list (information about middle tokens 
  // is held in the token strean to be attached to the AST).
  // const SgToken* firstToken = tokenList.front();
  // const SgToken* lastToken  = tokenList.back();
     const Token_t* firstToken = tokenList.front();
     const Token_t* lastToken  = tokenList.back();

     ROSE_ASSERT(firstToken != NULL);
     ROSE_ASSERT(lastToken != NULL);

     ROSE_ASSERT(firstToken->line > 0);
     ROSE_ASSERT(lastToken->line > 0);

  // This is required to handle both the Fortran specific "include" files and the CPP specific "#include" files.
     string filename = getCurrentFilename();

  // printf ("In resetSourcePosition(%p = %s) filename = %s \n",locatedNode,locatedNode->class_name().c_str(),filename.c_str());
     ROSE_ASSERT(filename.empty() == false);

  // Set these based on the source position information from the tokens
  // locatedNode->set_startOfConstruct (new Sg_File_Info(*(firstToken->get_startOfConstruct())));
  // locatedNode->set_endOfConstruct   (new Sg_File_Info(*(lastToken ->get_endOfConstruct  ())));
     locatedNode->set_startOfConstruct (new Sg_File_Info(filename,firstToken->line,firstToken->col));
     locatedNode->set_endOfConstruct   (new Sg_File_Info(filename,lastToken->line, lastToken->col));

     locatedNode->get_startOfConstruct()->set_parent(locatedNode);
     locatedNode->get_endOfConstruct  ()->set_parent(locatedNode);
   }


void
resetSourcePosition( SgLocatedNode* targetLocatedNode, const SgLocatedNode* sourceLocatedNode )
   {
#if 0
     printf ("In resetSourcePosition targetLocatedNode = %p = %s sourceLocatedNode = %p = %s \n",
          targetLocatedNode,targetLocatedNode->class_name().c_str(),
          sourceLocatedNode,sourceLocatedNode->class_name().c_str());
#endif

  // The SgLocatedNode has both a startOfConstruct and endOfConstruct source position.
     ROSE_ASSERT(targetLocatedNode != NULL);
     ROSE_ASSERT(sourceLocatedNode != NULL);

     ROSE_ASSERT(targetLocatedNode->get_startOfConstruct() != NULL);
     ROSE_ASSERT(targetLocatedNode->get_endOfConstruct() != NULL);

     ROSE_ASSERT(sourceLocatedNode->get_startOfConstruct() != NULL);
     ROSE_ASSERT(sourceLocatedNode->get_endOfConstruct() != NULL);

     if (sourceLocatedNode->get_startOfConstruct()->get_filenameString() == "NULL_FILE")
        {
          printf ("resetSourcePosition: sourceLocatedNode = %p = %s = %s \n",sourceLocatedNode,sourceLocatedNode->class_name().c_str(),SageInterface::get_name(sourceLocatedNode).c_str());
          sourceLocatedNode->get_startOfConstruct()->display("get_filenameString() == NULL_FILE");
        }
  // ROSE_ASSERT(sourceLocatedNode->get_startOfConstruct()->get_filenameString() != "NULL_FILE");

  // Remove the existing Sg_File_Info objects, they will be reset below
     delete targetLocatedNode->get_startOfConstruct();
     delete targetLocatedNode->get_endOfConstruct();

  // This is required to handle both the Fortran specific "include" files and the CPP specific "#include" files.
     string filename = getCurrentFilename();
  // printf ("In resetSourcePosition(%p = %s) filename = %s \n",targetLocatedNode,targetLocatedNode->class_name().c_str(),filename.c_str());
     ROSE_ASSERT(filename.empty() == false);

  // Set these based on the source position information from the tokens
  // locatedNode->set_startOfConstruct (new Sg_File_Info(*(firstToken->get_startOfConstruct())));
  // locatedNode->set_endOfConstruct   (new Sg_File_Info(*(lastToken ->get_endOfConstruct  ())));
     targetLocatedNode->set_startOfConstruct (new Sg_File_Info(*(sourceLocatedNode->get_startOfConstruct())));
     targetLocatedNode->set_endOfConstruct   (new Sg_File_Info(*(sourceLocatedNode->get_endOfConstruct  ())));

     targetLocatedNode->get_startOfConstruct()->set_parent(targetLocatedNode);
     targetLocatedNode->get_endOfConstruct  ()->set_parent(targetLocatedNode);
   }

// void resetEndingSourcePosition( SgLocatedNode* targetLocatedNode, Token_t* token )
void
resetEndingSourcePosition( SgLocatedNode* targetLocatedNode, Token_t* token )
   {
  // DQ (10/10/2010): This function is added to support resetting the end 
  // of the blocks (required to get comments woven into the AST properly).
     ROSE_ASSERT(token != NULL);
     int newLineNumber = token->line;
  // printf ("Resetting the end of the target statement = %s to line = %d \n",targetLocatedNode->class_name().c_str(),newLineNumber);
     resetEndingSourcePosition(targetLocatedNode,newLineNumber);

     SgFunctionDefinition* functionDefinition = isSgFunctionDefinition(targetLocatedNode);
     if (functionDefinition != NULL)
        {
       // Also set the ending position of the function declaration.
       // printf ("In resetEndingSourcePosition(): Set the ending position of the related function declaration \n");
          SgDeclarationStatement* functionDeclaration = functionDefinition->get_declaration();
          resetEndingSourcePosition(functionDeclaration,token);
        }

  // If this is the top level scope then iterate over the outer scopes to reset the end of each scope on the stack.
     if (astScopeStack.front() == targetLocatedNode)
        {
          list<SgScopeStatement*>::iterator i = astScopeStack.begin();
          ROSE_ASSERT(targetLocatedNode == *i);
          while (i != astScopeStack.end())
             {
#if 0
               printf ("In resetEndingSourcePosition(): Resetting the end of the block = %s to line = %d and file = %s \n",(*i)->class_name().c_str(),newLineNumber,getCurrentFilename().c_str());
#endif
               resetEndingSourcePosition(*i,newLineNumber);

               ROSE_ASSERT((*i)->isCaseInsensitive() == true);

               i++;
             }
        }
   }

void resetEndingSourcePosition( SgLocatedNode* targetLocatedNode, SgStatement* sourceStatement )
   {
  // If this is not the same file then the line numbers will not make any sense.
     ROSE_ASSERT(targetLocatedNode->get_endOfConstruct() != NULL);
     ROSE_ASSERT(sourceStatement->get_endOfConstruct()   != NULL);
     if (targetLocatedNode->get_endOfConstruct()->isSameFile(sourceStatement->get_endOfConstruct()) == true)
        {
          int newLineNumber = sourceStatement->get_endOfConstruct()->get_line();
          resetEndingSourcePosition(targetLocatedNode,newLineNumber);
        }
       else
        {
       // Increment the position by "1" since we have at least processed a Fortran include file on it's one line.
#if 0
          printf ("Processing special case of source statement not in same file as the start of the scope. \n");
#endif
          resetEndingSourcePosition(astScopeStack.front(),astScopeStack.front()->get_endOfConstruct()->get_line()+1);
        }
   }

void resetEndingSourcePosition( SgLocatedNode* targetLocatedNode, int newLineNumber )
   {
  // This function is called by the other "resetEndingSourcePosition()" functions.

#if 0
     printf ("targetLocatedNode = %s get_startOfConstruct()->get_line() = %d \n",targetLocatedNode->class_name().c_str(),targetLocatedNode->get_startOfConstruct()->get_line());
     printf ("targetLocatedNode = %s get_endOfConstruct()->get_line()   = %d \n",targetLocatedNode->class_name().c_str(),targetLocatedNode->get_endOfConstruct()->get_line());
#endif

     int oldLineNumber = targetLocatedNode->get_endOfConstruct()->get_line();
     if (newLineNumber > oldLineNumber)
        {
       // printf ("Resetting the ending line number from %d to %d \n",oldLineNumber,newLineNumber);
          targetLocatedNode->get_endOfConstruct()->set_line(newLineNumber);

       // If this is a different filename then change the filename as well.
          string currentFilename = getCurrentFilename();
          if (targetLocatedNode->get_endOfConstruct()->get_filenameString() != currentFilename)
             {
#if 0
               printf ("##### currentFilename = %s \n",currentFilename.c_str());
               printf ("##### targetLocatedNode->get_endOfConstruct()->get_filenameString() = %s \n",targetLocatedNode->get_endOfConstruct()->get_filenameString().c_str());
#endif
               targetLocatedNode->get_startOfConstruct()->set_filenameString(currentFilename);
               targetLocatedNode->get_endOfConstruct()->set_filenameString(currentFilename);
             }
        }

  // DQ (10/10/2010): See example test2007_17.f90 of if statment on a single line for were we can't enforce this.
  // ROSE_ASSERT(targetLocatedNode->get_endOfConstruct()->get_line() != targetLocatedNode->get_startOfConstruct()->get_line());
   }


SgType*
createType(int typeCode)
   {
  // This builds SgType IR nodes given the type code as input.
     SgType* result = NULL;
     switch(typeCode)
        {
          case IntrinsicTypeSpec_INTEGER:         result = SgTypeInt::createType();     break;
          case IntrinsicTypeSpec_REAL:            result = SgTypeFloat::createType();   break;
          case IntrinsicTypeSpec_DOUBLEPRECISION: result = SgTypeDouble::createType();  break;
          case IntrinsicTypeSpec_DOUBLECOMPLEX:   result = SgTypeComplex::createType(SgTypeDouble::createType()); break;
          case IntrinsicTypeSpec_COMPLEX:         result = SgTypeComplex::createType(SgTypeFloat::createType());  break;
          case IntrinsicTypeSpec_CHARACTER:       result = SgTypeChar::createType();    break;
          case IntrinsicTypeSpec_LOGICAL:         result = SgTypeBool::createType();    break;
 
#if ROSE_OFP_MINOR_VERSION_NUMBER >= 8 & ROSE_OFP_PATCH_VERSION_NUMBER >= 0
          // FMZ (2/2/2009): generate SgCAFTeamType for the image_type
          case IntrinsicTypeSpec_TEAM:       result = SgTypeCAFTeam::createType();    break;
          case IntrinsicTypeSpec_CRAYPOINTER:     result = SgTypeCrayPointer::createType();    break;
          // Laksono 2009.10.19: add a new intrinsic type: Topology
          case IntrinsicTypeSpec_TOPOLOGY:       result = SgTypeInt::createType();    break;
          // Laksono 2009.10.20: add a new intrinsic type: Event 
          // FMZ:: currently set event/lock/lockset with type complex(8)
          case IntrinsicTypeSpec_EVENT:
          case IntrinsicTypeSpec_LOCK: 
          case IntrinsicTypeSpec_LOCKSET:  
#endif
               {
                   SgType * tmpType = SgTypeComplex::createType(SgTypeDouble::createType());
                   SgModifierType* tmpM = new SgModifierType(tmpType);
                   ROSE_ASSERT(tmpM!=NULL);
                   SgIntVal* int8 = new SgIntVal(8,"8");
                   setSourcePosition(int8);
                   tmpM->set_type_kind(int8);
                   result = tmpM;
                   break;
               }

          default:
             {
               printf ("Default reached in creatType: typeCode = %d \n",typeCode);
               ROSE_ASSERT(false);
             }
       }

     ROSE_ASSERT(result != NULL);
     return result;
   }


SgExpression*
createUnaryOperator ( SgExpression* exp, string name, bool is_user_defined_operator )
   {
     ROSE_ASSERT(exp != NULL);
     SgExpression* result = NULL;

  // DQ (10/9/2008): Added support for user defined operators
     if (is_user_defined_operator == true)
        {
       // Get the function symbol for the function defined by "name"
          SgScopeStatement* currentScope = astScopeStack.front();

       // The name in the symbol table uses the form:
          name = "operator(" + name + ")";

          printf ("name = %s \n",name.c_str());
          printf ("currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());

       // currentScope->print_symboltable ("In createBinaryOperator()");

          SgFunctionSymbol* functionSymbol = trace_back_through_parent_scopes_lookup_function_symbol(name,currentScope);
          ROSE_ASSERT(functionSymbol != NULL);

          result = new SgUserDefinedUnaryOp(exp,NULL,name,functionSymbol);

          ROSE_ASSERT(result != NULL);
          return result;
        }

     int stringLength = name.length();
     if (stringLength == 1)
        {
          switch (name[0])
             {
               case '!':
                  {
                 // printf ("Building a SgLessThanOp binary operator \n");
                    result = new SgNotOp(exp,NULL);
                    break;
                  }

               default:
                  {
                    printf ("Error: default reached relOp = %s \n",name.c_str());
                    ROSE_ASSERT(false);
                  }
             }
        }
       else
        {
       // ROSE_ASSERT(stringLength == 2);
          if (stringLength == 2)
             {
            // Not sure what these would be.
               printf ("2 character unary operators not implemented relOp = %s \n",name.c_str());
               ROSE_ASSERT(false);
             }
            else
             {
               if (stringLength == 4)
                  {
                    if (matchingName(name,".XX.") == true)
                       {
                       }
                      else
                       {
                         printf ("Error: not sure what the operator is: %s \n",name.c_str());
                         ROSE_ASSERT(false);
                       }
                  }
                 else
                  {
                    if (stringLength == 5)
                       {
                      // This is the case for ".OR." etc.
                         if (matchingName(name,".NOT.") == true)
                            {
                              result = new SgNotOp(exp,NULL);
                            }
                           else
                            {
                              printf ("n != 1,2,4,5 character operators not implemented relOp = %s \n",name.c_str());
                              ROSE_ASSERT(false);
                            }
                       }
                  }
             }
        }

     ROSE_ASSERT(result != NULL);
     return result;
   }

SgExpression*
createBinaryOperator ( SgExpression* lhs, SgExpression* rhs, string name, bool is_user_defined_operator )
   {
     ROSE_ASSERT(lhs != NULL);
     ROSE_ASSERT(rhs != NULL);
     SgExpression* result = NULL;

  // DQ (10/9/2008): Added support for user defined operators
     if (is_user_defined_operator == true)
        {
       // Get the function symbol for the function defined by "name"
          SgScopeStatement* currentScope = astScopeStack.front();

          printf ("name = %s \n",name.c_str());
          printf ("currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());

       // currentScope->print_symboltable ("In createBinaryOperator()");

          SgFunctionSymbol* functionSymbol = trace_back_through_parent_scopes_lookup_function_symbol(name,currentScope);
          ROSE_ASSERT(functionSymbol != NULL);

          result = new SgUserDefinedBinaryOp(lhs,rhs,NULL,name,functionSymbol);

          ROSE_ASSERT(result != NULL);
          return result;
        }

     int stringLength = name.length();
     if (stringLength == 1)
        {
          switch (name[0])
             {
               case '<':
                  {
                 // printf ("Building a SgLessThanOp binary operator \n");
                    result = new SgLessThanOp(lhs,rhs,NULL);
                    break;
                  }

               case '>':
                  {
                 // printf ("Building a SgLessThanOp binary operator \n");
                    result = new SgGreaterThanOp(lhs,rhs,NULL);
                    break;
                  }

               default:
                  {
                    printf ("Error: default reached relOp = %s \n",name.c_str());
                    ROSE_ASSERT(false);
                  }
             }
        }
       else
        {
       // ROSE_ASSERT(stringLength == 2);
          if (stringLength == 2)
             {
            // This is likely the case of ">=", "<=", "/=", "=="
            // printf ("2 character operators not implemented relOp = %s \n",name.c_str());
            // ROSE_ASSERT(false);

               if (name == "<=")
                  {
                    result = new SgLessOrEqualOp(lhs,rhs,NULL);
                  }
                 else
                  {
                    if (name == ">=")
                       {
                         result = new SgGreaterOrEqualOp(lhs,rhs,NULL);
                       }
                      else
                       {
                         if (name == "==")
                            {
                              result = new SgEqualityOp(lhs,rhs,NULL);
                            }
                           else
                            {
                           // In C this would be the "!=" operator, in fortran it is "/="
                              if (name == "/=")
                                 {
                                   result = new SgNotEqualOp(lhs,rhs,NULL);
                                 }
                                else
                                 {
                                   if (name == "//")
                                      {
                                        result = new SgConcatenationOp(lhs,rhs,NULL);
                                      }
                                     else
                                      {
                                        printf ("2 character operator not implemented relOp = %s \n",name.c_str());
                                        ROSE_ASSERT(false);
                                      }
                                 }
                            }
                       }
                  }
             }
            else
             {
               if (stringLength == 4)
                  {
                 // These are all case insensitive.
                 // This is the case for ".LT.", ".LE.", ".EQ.", ".NE.", ".GT.", ".GE.", and ".OR." etc.
                    if (matchingName(name,".LT.") == true)
                       {
                         result = new SgLessThanOp(lhs,rhs,NULL);
                       }
                      else
                       {
                         if (matchingName(name,".LE.") == true)
                            {
                              result = new SgLessOrEqualOp(lhs,rhs,NULL);
                            }
                           else
                            {
                              if (matchingName(name,".EQ.") == true)
                                 {
                                   result = new SgEqualityOp(lhs,rhs,NULL);
                                 }
                                else
                                 {
                                   if (matchingName(name,".NE.") == true)
                                      {
                                        result = new SgNotEqualOp(lhs,rhs,NULL);
                                      }
                                     else
                                      {
                                        if (matchingName(name,".GT.") == true)
                                           {
                                             result = new SgGreaterThanOp(lhs,rhs,NULL);
                                           }
                                          else
                                           {
                                             if (matchingName(name,".GE.") == true)
                                                {
                                                  result = new SgGreaterOrEqualOp(lhs,rhs,NULL);
                                                }
                                               else
                                                {
                                                  if (matchingName(name,".OR.") == true)
                                                     {
                                                       result = new SgOrOp(lhs,rhs,NULL);
                                                     }
                                                    else
                                                     {
                                                       printf ("4 character operator not implemented relOp = %s \n",name.c_str());
                                                       ROSE_ASSERT(false);
                                                     }
                                                }
                                           }
                                      }
                                 }
                            }
                       }
                  }
                 else
                  {
                    if (stringLength == 5)
                       {
                      // These are all case insensitive.
                      // This is the case for ".AND." etc.
                         if (matchingName(name,".AND.") == true)
                            {
                              result = new SgAndOp(lhs,rhs,NULL);
                            }
                           else
                            {
                              if (matchingName(name,".EQV.") == true)
                                 {
                                   result = new SgEqualityOp(lhs,rhs,NULL);
                                 }
                                else
                                 {
                                   printf ("n != 2,4,5 character operators not implemented relOp = %s \n",name.c_str());
                                   ROSE_ASSERT(false);
                                 }
                            }
                       }
                      else
                       {
                         if (stringLength == 6)
                            {
                           // This is the case for ".NEQV." etc.
                              if (matchingName(name,".NEQV.") == true)
                                 {
                                // DQ (8/5/2010): This might output .NE. instead of .NEQV. and the two are .NEQV.! 
                                   result = new SgNotEqualOp(lhs,rhs,NULL);
                                 }
                                else
                                 {
                                   printf ("n != 2,4,5,6 character operators not implemented relOp = %s \n",name.c_str());
                                   ROSE_ASSERT(false);
                                 }
                            }
                       }
                 // printf ("n != 2,4 character operators not implemented relOp = %s \n",name.c_str());
                 // ROSE_ASSERT(false);
                  }
             }
        }

     ROSE_ASSERT(result != NULL);
     return result;
   }


void
outputStateSupport( const std::string & s, int fieldWidth )
   {
     printf ("(%s)",s.c_str());
     for (int j=s.length(); j < fieldWidth; j++)
        {
          printf (" ");
        }
   }

void outputState( const std::string label )
   {
  // This function is used for debugging and outputs the data in the different 
  // stacks used to accumulate intermeditate data as part of building the AST.
  // Output the stack information for: 
  //      astScopeStack, 
  //      astExpressionStack, 
  //      astNodeStack, 
  //      astNameStack, 
  //      astTypeStack, 
  //      astIntentSpecStack, 
  //      astAttributeSpecStack,
  //      astDeclarationStatementStack,
  //      astInitializerStack, 

     if ( SgProject::get_verbose() <= 2 )
        {
       // Skip output of stack data for verbose levels less than or equal to 2
          return;
        }

     size_t maxStackSize = astScopeStack.size();
     maxStackSize = astExpressionStack.size()           > maxStackSize ? astExpressionStack.size()    : maxStackSize;
     maxStackSize = astNodeStack.size()                 > maxStackSize ? astNodeStack.size()          : maxStackSize;
     maxStackSize = astNameStack.size()                 > maxStackSize ? astNameStack.size()          : maxStackSize;
     maxStackSize = astBaseTypeStack.size()             > maxStackSize ? astBaseTypeStack.size()      : maxStackSize;
     maxStackSize = astTypeStack.size()                 > maxStackSize ? astTypeStack.size()          : maxStackSize;
     maxStackSize = astIntentSpecStack.size()           > maxStackSize ? astIntentSpecStack.size()    : maxStackSize;
     maxStackSize = astAttributeSpecStack.size()        > maxStackSize ? astAttributeSpecStack.size() : maxStackSize;
  // maxStackSize = astInitializerStack.size()          > maxStackSize ? astInitializerStack.size()   : maxStackSize;
     maxStackSize = astTypeKindStack.size()             > maxStackSize ? astTypeKindStack.size()      : maxStackSize;
     maxStackSize = astTypeParameterStack.size()        > maxStackSize ? astTypeParameterStack.size() : maxStackSize;
     maxStackSize = astLabelSymbolStack.size()          > maxStackSize ? astLabelSymbolStack.size()   : maxStackSize;

  // maxStackSize = astDeclarationStatementStack.size() > maxStackSize ? astDeclarationStatementStack.size() : maxStackSize;

     printf ("\n");
     printf ("\n");
     printf ("In outputState (%s): maxStackSize = %ld \n",label.c_str(),(long)maxStackSize);

     std::list<SgScopeStatement*>      ::reverse_iterator astScopeStack_iterator                = astScopeStack.rbegin();
     std::list<SgExpression*>          ::reverse_iterator astExpressionStack_iterator           = astExpressionStack.rbegin();
     std::list<SgNode*>                ::reverse_iterator astNodeStack_iterator                 = astNodeStack.rbegin();
     std::list<AstNameType*>           ::reverse_iterator astNameStack_iterator                 = astNameStack.rbegin();
     std::list<SgType*>                ::reverse_iterator astBaseTypeStack_iterator             = astBaseTypeStack.rbegin();
     std::list<SgType*>                ::reverse_iterator astTypeStack_iterator                 = astTypeStack.rbegin();
     std::list<int>                    ::reverse_iterator astIntentSpecStack_iterator           = astIntentSpecStack.rbegin();
     std::list<int>                    ::reverse_iterator astAttributeSpecStack_iterator        = astAttributeSpecStack.rbegin();
  // std::list<SgExpression*>          ::reverse_iterator astInitializerStack_iterator          = astInitializerStack.rbegin();
     std::list<SgExpression*>          ::reverse_iterator astTypeKindStack_iterator             = astTypeKindStack.rbegin();
     std::list<SgExpression*>          ::reverse_iterator astTypeParameterStack_iterator        = astTypeParameterStack.rbegin();
     std::list<SgLabelSymbol*>         ::reverse_iterator astLabelSymbolStack_iterator          = astLabelSymbolStack.rbegin();

  // std::list<SgDeclarationStatement*>::reverse_iterator astDeclarationStatementStack_iterator = astDeclarationStatementStack.rbegin();

  // printf ("     astScopeStack : astExpressionStack : astNodeStack : astDeclarationStatementStack : astTypeStack : astIntentSpecStack : astAttributeSpecStack \n");
     const int NumberOfStacks = 11;
  // string stackNames[NumberOfStacks] = { "astScopeStack", "astExpressionStack", "astNodeStack", "astNameStack", "astTypeStack", "astIntentSpecStack", "astAttributeSpecStack" };
     struct
        { std::string name;
          int fieldWidth;
        } stackNames[NumberOfStacks] = { {"astScopeStack", 40} ,    {"astExpressionStack",30} ,   {"astNodeStack",30},
                                         {"astNameStack",30} ,      {"astBaseTypeStack",30},      {"astTypeStack",30}, 
                                         {"astIntentSpecStack",30}, {"astAttributeSpecStack",20}, /* {"astInitializerStack",20}, */
                                         {"astTypeKindStack",20},   {"astTypeParameterStack",20}, {"astLabelSymbolStack",20} };

     for (int k=0; k < NumberOfStacks; k++)
        {
          std::string s  = stackNames[k].name;
          int fieldWidth = stackNames[k].fieldWidth;
          outputStateSupport(s,fieldWidth);
        }
     printf ("\n");

  // printf ("-------------------------------------------------------------------------------------------------------------------------------------------------------\n");
     int fieldWidth = 25;
     for (int j=0; j < fieldWidth*NumberOfStacks; j++)
        {
          printf ("-");
        }
     printf ("\n");

     for (size_t i=0; i < maxStackSize; i++)
        {
          std::string s;
          if (astScopeStack_iterator != astScopeStack.rend())
             {
            // printf ("     %p = %s = %s :",*astScopeStack_iterator,(*astScopeStack_iterator)->class_name().c_str(),SageInterface::get_name(*astScopeStack_iterator).c_str());
            // printf ("     %p ",*astScopeStack_iterator);
            // printf (" %s ",(*astScopeStack_iterator)->class_name().c_str());
            // printf ("= %s ",SageInterface::get_name(*astScopeStack_iterator).c_str());
            // printf (":");

               if (isSgBasicBlock(*astScopeStack_iterator) != NULL || isSgAssociateStatement(*astScopeStack_iterator) != NULL)
                  {
                 // If this is the SgBasicBlock or SgAssociateStatement then output the address instead 
                 // of the "default_name" generated by SageInterface::get_name().
                    s = (*astScopeStack_iterator)->class_name() + " : " + StringUtility::numberToString(*astScopeStack_iterator);
                  }
                 else
                  {
                    s = (*astScopeStack_iterator)->class_name() + " : " + SageInterface::get_name(*astScopeStack_iterator);
                  }

               astScopeStack_iterator++;
             }
            else
             {
               s = " No Scope ";
             }

          outputStateSupport(s,stackNames[0].fieldWidth);

          if (astExpressionStack_iterator != astExpressionStack.rend())
             {
            // printf ("     %p = %s = %s :",*astExpressionStack_iterator,(*astExpressionStack_iterator)->class_name().c_str(),SageInterface::get_name(*astExpressionStack_iterator).c_str());
            // printf ("     %p ",*astExpressionStack_iterator);
            // printf (" %s ",(*astExpressionStack_iterator)->class_name().c_str());
            // printf ("= %s ",SageInterface::get_name(*astExpressionStack_iterator).c_str());
            // printf (":");
               s = (*astExpressionStack_iterator)->class_name() + " : " + SageInterface::get_name(*astExpressionStack_iterator);

               astExpressionStack_iterator++;
             }
            else
             {
               s = " No Expression ";
             }

          outputStateSupport(s,stackNames[1].fieldWidth);

          if (astNodeStack_iterator != astNodeStack.rend())
             {
            // printf ("     %p = %s = %s :",*astExpressionStack_iterator,(*astExpressionStack_iterator)->class_name().c_str(),SageInterface::get_name(*astExpressionStack_iterator).c_str());
            // printf ("     %p ",*astNodeStack_iterator);
            // printf (" %s ",(*astNodeStack_iterator)->class_name().c_str());
            // printf ("= %s ",SageInterface::get_name(*astNodeStack_iterator).c_str());
            // printf (":");
               s = (*astNodeStack_iterator)->class_name() + " : " + SageInterface::get_name(*astNodeStack_iterator);

               astNodeStack_iterator++;
             }
            else
             {
               s = " No Node ";
             }

          outputStateSupport(s,stackNames[2].fieldWidth);

          if (astNameStack_iterator != astNameStack.rend())
             {
               s = (*astNameStack_iterator)->text;

               astNameStack_iterator++;
             }
            else
             {
               s = " No Token ";
             }

          outputStateSupport(s,stackNames[3].fieldWidth);

          if (astBaseTypeStack_iterator != astBaseTypeStack.rend())
             {
               s = (*astBaseTypeStack_iterator)->class_name() + " : " + SageInterface::get_name(*astBaseTypeStack_iterator);

               astBaseTypeStack_iterator++;
             }
            else
             {
               s = " No Type ";
             }

          outputStateSupport(s,stackNames[4].fieldWidth);

          if (astTypeStack_iterator != astTypeStack.rend())
             {
            // printf ("     %p = %s = %s :",*astExpressionStack_iterator,(*astExpressionStack_iterator)->class_name().c_str(),SageInterface::get_name(*astExpressionStack_iterator).c_str());
            // printf ("     %p ",*astTypeStack_iterator);
            // printf (" %s ",(*astTypeStack_iterator)->class_name().c_str());
            // printf ("= %s ",SageInterface::get_name(*astTypeStack_iterator).c_str());
            // printf (":");
               s = (*astTypeStack_iterator)->class_name() + " : " + SageInterface::get_name(*astTypeStack_iterator);

               astTypeStack_iterator++;
             }
            else
             {
               s = " No Type ";
             }

          outputStateSupport(s,stackNames[5].fieldWidth);

          if (astIntentSpecStack_iterator != astIntentSpecStack.rend())
             {
            // printf (" %d ",*astIntentSpecStack_iterator);
            // printf (":");
               s = StringUtility::numberToString(*astIntentSpecStack_iterator);

               astIntentSpecStack_iterator++;
             }
            else
             {
               s = " No IntentSpec ";
             }

          outputStateSupport(s,stackNames[6].fieldWidth);

          if (astAttributeSpecStack_iterator != astAttributeSpecStack.rend())
             {
            // printf (" %d ",*astAttributeSpecStack_iterator);
            // printf (":");
               s = StringUtility::numberToString(*astAttributeSpecStack_iterator);

               astAttributeSpecStack_iterator++;
             }
            else
             {
               s = " No AttributeSpec ";
             }

          outputStateSupport(s,stackNames[7].fieldWidth);

          if (astTypeKindStack_iterator != astTypeKindStack.rend())
             {
               s = (*astTypeKindStack_iterator)->class_name() + " : " + SageInterface::get_name(*astTypeKindStack_iterator);

               astTypeKindStack_iterator++;
             }
            else
             {
               s = " No Expression ";
             }

       // outputStateSupport(s,stackNames[9].fieldWidth);
          outputStateSupport(s,stackNames[8].fieldWidth);

          if (astTypeParameterStack_iterator != astTypeParameterStack.rend())
             {
               s = (*astTypeParameterStack_iterator)->class_name() + " : " + SageInterface::get_name(*astTypeParameterStack_iterator);

               astTypeParameterStack_iterator++;
             }
            else
             {
               s = " No Expression ";
             }

       // outputStateSupport(s,stackNames[10].fieldWidth);
          outputStateSupport(s,stackNames[9].fieldWidth);

          if (astLabelSymbolStack_iterator != astLabelSymbolStack.rend())
             {
               s = (*astLabelSymbolStack_iterator)->class_name() + " : " + SageInterface::get_name(*astLabelSymbolStack_iterator);

               astLabelSymbolStack_iterator++;
             }
            else
             {
               s = " No Symbol ";
             }

       // outputStateSupport(s,stackNames[11].fieldWidth);
          outputStateSupport(s,stackNames[10].fieldWidth);

          printf ("\n");
        }

     printf ("\n");
     printf ("\n");
   }


SgScopeStatement* getTopOfScopeStack()
   {
  // This function has to do into go into a different source file than fortran_support.C 
  // since that is linked to librose while the astScopeStack variable is declared in this 
  // file (so they at least have to stay together).

     ROSE_ASSERT(astScopeStack.empty() == false);
  // SgScopeStatement* topOfStack = *(astScopeStack.begin());
  // printf ("In getTopOfScopeStack() topOfStack = %p = %s \n",topOfStack,topOfStack->class_name().c_str());
     SgScopeStatement* topOfStack = astScopeStack.front();

  // Testing the scope stack...
  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     std::list<SgScopeStatement*>::iterator scopeInterator = astScopeStack.begin();
     while (scopeInterator != astScopeStack.end())
        {
          if ((*scopeInterator)->isCaseInsensitive() == false)
             {
               printf ("##### Error: the scope handling is set to case sensitive scopeInterator = %p = %s \n",*scopeInterator,(*scopeInterator)->class_name().c_str());
            // (*scopeInterator)->setCaseInsensitive(true);
             }
          ROSE_ASSERT((*scopeInterator)->isCaseInsensitive() == true);
          scopeInterator++;
        }

     return topOfStack;
   }

SgType* getTopOfTypeStack()
   {
  // This function has to do into go into a different source file than fortran_support.C 
  // since that is linked to librose while the astScopeStack variable is declared in this 
  // file (so they at least have to stay together).

     ROSE_ASSERT(astTypeStack.empty() == false);
     SgType* topOfStack = astTypeStack.front();

     return topOfStack;
   }

AstNameType* getTopOfNameStack()
   {
  // This function has to do into go into a different source file than fortran_support.C 
  // since that is linked to librose while the astScopeStack variable is declared in this 
  // file (so they at least have to stay together).

     ROSE_ASSERT(astNameStack.empty() == false);
  // AstNameType* topOfStack = *(astNameStack.begin());
     AstNameType* topOfStack = astNameStack.front();
  // printf ("In getTopOfNameStack() topOfStack = %p \n",topOfStack);

     return topOfStack;
   }

SgExpression* getTopOfExpressionStack()
   {
  // This function has to do into go into a different source file than fortran_support.C 
  // since that is linked to librose while the astScopeStack variable is declared in this 
  // file (so they at least have to stay together).

     ROSE_ASSERT(astExpressionStack.empty() == false);
  // SgExpression* topOfStack = *(astExpressionStack.begin());
     SgExpression* topOfStack = astExpressionStack.front();
  // printf ("In getTopOfExpressionStack() topOfStack = %p = %s \n",topOfStack,topOfStack->class_name().c_str());

     return topOfStack;
   }


SgExpression* 
buildLabelRefExp(SgExpression* expression)
   {
     SgExpression* returnExpression = expression;

  // If this is an integer it is a format label and we want to generate a SgLabelRefExp
     SgIntVal* integerValue = isSgIntVal(expression);
     if (integerValue != NULL)
        {
       // If this is an integer, then generate a SgLabelRefExp.
          SgName name = StringUtility::numberToString(integerValue->get_value());

          Token_t* format_label = create_token(1,0,0,name.str());
          SgLabelSymbol* labelSymbol = buildNumericLabelSymbol(format_label);

          delete format_label;
          format_label = NULL;

          labelSymbol->set_label_type(SgLabelSymbol::e_start_label_type);

          SgLabelRefExp* labelRefExp = new SgLabelRefExp(labelSymbol);
       // printf ("################## In buildLabelRefExp(): labelRefExp = %p value = %d \n",labelRefExp,labelRefExp->get_symbol()->get_numeric_label_value());

          setSourcePosition(labelRefExp);

       // DQ (1/26/2009): Set the parent of the SgIntVal IR node.
          integerValue->set_parent(labelRefExp);

          returnExpression = labelRefExp;
        }

     return returnExpression;
   }


SgFunctionDefinition*
getFunctionDefinitionFromScopeStack()
   {
  // New function to refactor code used to get the SgFunctionDefinition off of the scope stack.
     SgFunctionDefinition* functionDefinition = NULL;

     std::list<SgScopeStatement*>::iterator i = astScopeStack.begin();
  // printf ("Defining iterator i scope = %p = %s \n",*i,(*i)->class_name().c_str());

     while ( i != astScopeStack.end() && isSgFunctionDefinition(*i) == NULL )
        {
       // printf ("Looping iterator (before ++) i scope = %p = %s \n",*i,(*i)->class_name().c_str());
          i++;
       // printf ("Looping iterator i scope = %p = %s \n",*i,(*i)->class_name().c_str());
        }

     if (i != astScopeStack.end())
        {
          functionDefinition = isSgFunctionDefinition(*i);
          ROSE_ASSERT(functionDefinition != NULL);
        }

     return functionDefinition;
   }



// SgLabelRefExp* buildNumericLabelSymbol(Token_t* label)
SgLabelSymbol*
buildNumericLabelSymbol(Token_t* label)
   {
  // This is the function we use to create the label that might refer to a 
  // preivously seen statement or a statement we will see in the future.

     ROSE_ASSERT(label != NULL);
     ROSE_ASSERT(label->text != NULL);

  // DQ (11/22/2010): This is a bug in OFP, but I have to work around it for now.
     if (label->line == 0)
        {
          printf ("Error (OFP bug): label->line == 0 for label->text = %s \n",label->text);
        }
  // ROSE_ASSERT(label->line > 0);

     SgLabelSymbol* returnSymbol = NULL;

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of buildNumericLabelSymbol()");
#endif

     if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
          printf ("This code can be replaced with a call to getFunctionDefinitionFromScopeStack() \n");

     std::list<SgScopeStatement*>::iterator i = astScopeStack.begin();
  // printf ("Defining iterator i scope = %p = %s \n",*i,(*i)->class_name().c_str());
  // while ( astScopeStack.empty() == false && i != astScopeStack.end() && isSgFunctionDefinition(*i) == NULL )
     while ( i != astScopeStack.end() && isSgFunctionDefinition(*i) == NULL )
        {
       // printf ("Looping iterator (before ++) i scope = %p = %s \n",*i,(*i)->class_name().c_str());
          i++;
       // printf ("Looping iterator i scope = %p = %s \n",*i,(*i)->class_name().c_str());
        }

  // A label on a program statment will not have a scope in which to be identified. "10 program main"
  // It is not clear what scope such a label should be added (global scope?).
     if (i != astScopeStack.end())
        {
       // printf ("Looking for SgLabelSymbol i scope = %p = %s \n",*i,(*i)->class_name().c_str());
          ROSE_ASSERT(isSgFunctionDefinition(*i) != NULL);

       // ROSE_ASSERT( i != astScopeStack.end() );
          SgName name = label->text;
       // SgLabelSymbol* returnSymbol = (*i)->lookup_label_symbol(name);
          returnSymbol = (*i)->lookup_label_symbol(name);

       // printf ("In buildNumericLabelSymbol(): returnSymbol = %p \n",returnSymbol);
          if (returnSymbol == NULL)
             {
            // The symbol was not found, create a symbol so that statements can reference
            // it then we can fixup the statement in the symbol later (when we see it).
               int label_value = atoi(label->text);
            // printf ("Building a SgLabelSymbol for a numeric label that we have not see yet: label_value = %d = %s \n",label_value,label->text);

               returnSymbol = new SgLabelSymbol(NULL);
               ROSE_ASSERT(returnSymbol != NULL);
               returnSymbol->set_fortran_statement(NULL);
               returnSymbol->set_numeric_label_value(label_value);

               SgStatement* label_statement = new SgNullStatement();
            // printf ("Building SgNullStatement label_statement = %p \n",label_statement);

               returnSymbol->set_fortran_statement(label_statement);

            // DQ (1/20/2008): The parent of a statement can't be set to a SgSymbol, so make it point to the current scope for now!
            // label_statement->set_parent(returnSymbol);
               label_statement->set_parent(astScopeStack.front());
               ROSE_ASSERT(label_statement->get_parent() != NULL);

            // DQ (1/20/2008): If the label is not present, but is referenced then this has to be set.
            // Note that test2007_175.f demonstrates that if the lable is not present 
            // then this label_statement fails because the source position is not set.
               setSourcePosition(label_statement);

               ROSE_ASSERT(isSgFunctionDefinition(*i) != NULL);
            // Insert the symbol into the function definition's symbol table so it will be found next time.
               (*i)->insert_symbol(name,returnSymbol);
             }

          ROSE_ASSERT(returnSymbol != NULL);
        }
       else
        {
          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
               printf ("WARNING: label attached to construct not in a function scope! \n");
#if 0
       // Output debugging information about saved state (stack) information.
          outputState("WARNING: At BOTTOM of buildNumericLabelSymbol()");
#endif
        }

  // printf ("Returning from buildNumericLabelSymbol() returnSymbol = %p \n",returnSymbol);
     return returnSymbol;
   }

SgLabelSymbol* 
buildNumericLabelSymbolAndAssociateWithStatement(SgStatement* stmt, Token_t* label)
   {
  // This is the function we use to associate a label with a statement.
  // The label may be been previously build if there was a forward 
  // reference to the statement's label. This might also be the label for
  // the statements asociated end of construct (for the label associated
  // with "end" for example).

     ROSE_ASSERT(label != NULL);
     ROSE_ASSERT(label->line > 0);
     ROSE_ASSERT(label->text != NULL);

  // Assumes simple string based representation of integer
  // I used atoi(), even though strtol() is suggested.  We store the 
  // numeric lables in the AST as numeric values.
  // int numericLabel = atoi(label->text);
  // printf ("label->text = %s numericLabel = %d \n",label->text,numericLabel);
  // stmt->set_numeric_label(numericLabel);

     std::list<SgScopeStatement*>::iterator i = astScopeStack.begin();
  // printf ("Starting at the scope = %s \n",(*i)->class_name().c_str());

  // DQ (12/9/2007): Added support for numeric lables to be SgLabelSymbols.
  // It is unclear if we want to use the function scope of the global scope 
  // (I have use the function scope, where the function is not explicit this 
  // is still using the compiler generated function).
     while ( i != astScopeStack.end() && isSgFunctionDefinition(*i) == NULL )
        {
          i++;
        }

     ROSE_ASSERT( i != astScopeStack.end() );
     ROSE_ASSERT(isSgFunctionDefinition(*i) != NULL);

  // printf ("Looking for SgLabelSymbol i scope = %p = %s \n",*i,(*i)->class_name().c_str());

     SgName name = label->text;
     SgLabelSymbol* label_symbol = (*i)->lookup_label_symbol(name);
     if (label_symbol == NULL)
        {
       // If this does not exist then build the associated label symbol and put it into the function's symbol table.
          label_symbol = new SgLabelSymbol(NULL);
          label_symbol->set_fortran_statement(stmt);

       // DQ (12/24/2007): The new design stores the numeric label value in the SgLabelSymbol.
          int label_value = atoi(label->text);
       // printf ("Building a SgLabelSymbol for a numeric label that we have not see yet: label_value = %d = %s \n",label_value,label->text);
          label_symbol->set_numeric_label_value(label_value);

       // printf ("Warning in buildNumericLabelSymbolAndAssociateWithStatement(): setting label_type using SgLabelSymbol::e_end_label_type \n");
          label_symbol->set_label_type(SgLabelSymbol::e_end_label_type);

          ROSE_ASSERT(isSgFunctionDefinition(*i) != NULL);
          (*i)->insert_symbol(name,label_symbol);

          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
               printf ("Built a new SgLabelSymbol = %p (numeric value = %d) for name = %s associated with statement %p \n",label_symbol,label_value,name.str(),stmt);
        }
       else
        {
       // If this exists then it was build with a SgNullStatement and we have to replace the statement.
          SgStatement* tempStatement = label_symbol->get_fortran_statement();

          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
              printf ("Found an existing SgLabelSymbol = %p for name = %s associated with statement %p (deleted tempStatement = %p) \n",label_symbol,name.str(),stmt,tempStatement);

       // DQ (12/24/2007): New implementation does not use SgNullStatement internally. NOT TRUE, we need these for forward references to numeric labels.
       // ROSE_ASSERT(isSgNullStatement(tempStatement) == NULL);

       // DQ (12/24/2007): If this is associated with a SgFortranDo statement, then don't delete it.
       // Only delete it if it is a SgNullStatement. This should solve the problem with test2007_233.f 
       // (fixed this problem, but not finished yet).
          if (isSgNullStatement(tempStatement) != NULL)
             {
               delete tempStatement;
               tempStatement = NULL;
             }

          label_symbol->set_fortran_statement(NULL);
          label_symbol->set_fortran_statement(stmt);
        }

     ROSE_ASSERT(label_symbol != NULL);
     return label_symbol;
   }


// DQ (12/8/2010): This is a new function that should likely not be implemented!
void
processLabelOnStack( SgStatement* statement )
   {
  // The label functionality should be handled via the label toke passed as a function parameter and
  // we should disable the generation of SgLabelSymbol and pushing them onto the stack via R313
  // (a reference for this handling is the email with Scott 12/8/2010).

     ROSE_ASSERT(statement != NULL);

     if (astLabelSymbolStack.empty() == false)
        {
          SgLabelSymbol* labelSymbol = astLabelSymbolStack.front();
          ROSE_ASSERT(labelSymbol != NULL);

          astLabelSymbolStack.pop_front();
        }
   }


// DQ (12/8/2010): This is a new function that should likely not be implemented!
void
specialFixupForLabelOnStackAndNotPassedAsParameter( SgStatement* statement )
   {
// This is to make up for a bug in OFP where the label is pushed onto the stack
// but not also passed as a c_action function function argument.  So this is
// the only way to detect and process the label.

     ROSE_ASSERT(statement != NULL);

     if (astLabelSymbolStack.empty() == false)
        {
          SgLabelSymbol* labelSymbol = astLabelSymbolStack.front();
          ROSE_ASSERT(labelSymbol != NULL);

          astLabelSymbolStack.pop_front();

       // DQ (12/9/2010): We only want to pop the stack, the label is processed using
       // only the token from the c_action function parameter as suggested by Scott.

       // SgVarRefExp* labelVarRef = SageBuilder::buildVarRefExp(labelSymbol);
          SgLabelRefExp* labelRefExp = new SgLabelRefExp(labelSymbol);
          setSourcePosition(labelRefExp);

          statement->set_numeric_label(labelRefExp);
        }
   }


void
setStatementNumericLabelUsingStack(SgStatement* statement)
   {
  // DQ (12/9/2010): To provide consistant handling of labels we want to only process 
  // labels passed as arguments to the appropriate c_action function.  So this processing
  // should be redundant with that and disallowed.

  // Set the label using the stack 
     if (astLabelSymbolStack.empty() == false)
        {
          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
               printf ("There is a label on the stack \n");

       // Get the label info from the astLabelSymbolStack
          SgLabelSymbol* labelSymbol = astLabelSymbolStack.front();
          astLabelSymbolStack.pop_front();

       // The label should not overwrite an existing label (built using a label 
       // Token_t passed as a function argument to a c_action_ function).
       // ROSE_ASSERT(statement->get_numeric_label() == NULL);
          if (statement->get_numeric_label() != NULL)
             {
               if (statement->get_numeric_label()->get_numeric_label_value() != labelSymbol->get_numeric_label_value())
                  {
                    printf ("Error: Overwriting an existing label on a statement = %p = %s \n",statement,statement->class_name().c_str());
                    printf ("   labelSymbol->get_numeric_label_value()                    = %d \n",labelSymbol->get_numeric_label_value());
                    printf ("   statement->get_numeric_label()->get_numeric_label_value() = %d \n",statement->get_numeric_label()->get_numeric_label_value());
                    ROSE_ASSERT(statement->get_numeric_label()->get_numeric_label_value() == labelSymbol->get_numeric_label_value());
                  }
             }

          if (statement->get_numeric_label() == NULL)
             {
            // Set the label in the statement
            // statement->set_numeric_label(labelSymbol);

               SgLabelRefExp* labelRefExp = new SgLabelRefExp(labelSymbol);
            // printf ("################## In setStatementNumericLabelUsingStack(): statement = %p labelRefExp = %p value = %d \n",statement,labelRefExp,labelRefExp->get_symbol()->get_numeric_label_value());

            // printf ("Exiting as a test! \n");
            // ROSE_ASSERT(false);

               statement->set_numeric_label(labelRefExp);
               labelRefExp->set_parent(statement);
               setSourcePosition(labelRefExp);
             }
        }
       else
        {
          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
               printf ("astLabelSymbolStack.empty() == true, no label on stack for statement = %p = %s \n",statement,statement->class_name().c_str());
        }
   }


void
setStatementNumericLabel(SgStatement* stmt, Token_t* label)
   {
     if (label != NULL)
        {
          ROSE_ASSERT(label->line > 0);
          ROSE_ASSERT(label->text != NULL);

       // DQ (2/18/2008): There are two mechanisms for setting labels, make sure that if 
       // this one is being used that we don't leave labels on the astLabelSymbolStack
       // ROSE_ASSERT (astLabelSymbolStack.empty() == true);

          SgLabelSymbol* labelSymbol = buildNumericLabelSymbolAndAssociateWithStatement(stmt,label);
          ROSE_ASSERT(labelSymbol != NULL);

       // printf ("In setStatementNumericLabel(): Found statement =%p with label = %d \n",labelSymbol->get_fortran_statement(),labelSymbol->get_fortran_statement()->get_numeric_label());
          labelSymbol->set_label_type(SgLabelSymbol::e_start_label_type);
       // stmt->set_numeric_label(labelSymbol);

          SgLabelRefExp* labelRefExp = new SgLabelRefExp(labelSymbol);

       // This providea a crude from of trace through the problem so that we can 
       // map failures back to the relevant lines of code in a large application.
       // printf ("################## In setStatementNumericLabel(): statement = %p labelRefExp = %p value = %d \n",stmt,labelRefExp,labelRefExp->get_symbol()->get_numeric_label_value());

          stmt->set_numeric_label(labelRefExp);
          labelRefExp->set_parent(stmt);
          setSourcePosition(labelRefExp,label);
        }
   }

void
setStatementEndNumericLabel(SgStatement* stmt, Token_t* label)
   {
#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of setStatementEndNumericLabel()");
#endif

     ROSE_ASSERT(stmt->has_end_numeric_label() == true);

     if (label != NULL)
        {
          ROSE_ASSERT(label->line > 0);
          ROSE_ASSERT(label->text != NULL);

          SgLabelSymbol* labelSymbol = buildNumericLabelSymbolAndAssociateWithStatement(stmt,label);

       // Mark the label as associated with the end of the statement (for block constructs)
          ROSE_ASSERT(labelSymbol != NULL);

       // printf ("In setStatementEndNumericLabel(): Found statement =%p with label = %d \n",labelSymbol->get_fortran_statement(),labelSymbol->get_fortran_statement()->get_numeric_label());
          labelSymbol->set_label_type(SgLabelSymbol::e_end_label_type);
       // int numeric_label = atoi(label->text);
       // stmt->set_end_numeric_label(labelSymbol);

          SgLabelRefExp* labelRefExp = new SgLabelRefExp(labelSymbol);
          stmt->set_end_numeric_label(labelRefExp);
          labelRefExp->set_parent(stmt);
          setSourcePosition(labelRefExp,label);
        }
   }

void
setStatementElseNumericLabel(SgStatement* stmt, Token_t* label)
   {
#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of setStatementElseNumericLabel()");
#endif

  // For now just trap the special case.  At some point this might also apply to the SgWhereStatement
  // ROSE_ASSERT(stmt->has_else_numeric_label() == true);

     SgIfStmt* ifStatement = isSgIfStmt(stmt);
     ROSE_ASSERT(ifStatement != NULL);

     if (label != NULL)
        {
          ROSE_ASSERT(label->line > 0);
          ROSE_ASSERT(label->text != NULL);

          SgLabelSymbol* labelSymbol = buildNumericLabelSymbolAndAssociateWithStatement(stmt,label);

       // Mark the label as associated with the end of the statement (for block constructs)
          ROSE_ASSERT(labelSymbol != NULL);

       // printf ("In setStatementElseNumericLabel(): Found statement =%p with label = %d \n",labelSymbol->get_fortran_statement(),labelSymbol->get_fortran_statement()->get_numeric_label());
          labelSymbol->set_label_type(SgLabelSymbol::e_else_label_type);
       // int numeric_label = atoi(label->text);
       // ifStatement->set_else_numeric_label(labelSymbol);

          SgLabelRefExp* labelRefExp = new SgLabelRefExp(labelSymbol);
       // stmt->set_else_numeric_label(labelRefExp);
          ifStatement->set_else_numeric_label(labelRefExp);
          labelRefExp->set_parent(ifStatement);
          setSourcePosition(labelRefExp,label);
        }
   }

void
setStatementStringLabel(SgStatement* stmt, Token_t* label)
   {
     if (label != NULL)
        {
          ROSE_ASSERT(label->line > 0);
          ROSE_ASSERT(label->text != NULL);

       // Note that an alternative to this switch would be to have the 
       // apropriate virtual dunctioned defined in the IR nodes.
          switch (stmt->variantT())
             {
               case V_SgWhereStatement:
                  {
                    SgWhereStatement* statement = isSgWhereStatement(stmt);
                    statement->set_string_label(label->text);
                    break;
                  }

               case V_SgFortranDo:
                  {
                    SgFortranDo* statement = isSgFortranDo(stmt);
                    statement->set_string_label(label->text);
                    break;
                  }

               case V_SgIfStmt:
                  {
                    SgIfStmt* statement = isSgIfStmt(stmt);
                    statement->set_string_label(label->text);
                    break;
                  }

               case V_SgForStatement:
                  {
                    SgForStatement* statement = isSgForStatement(stmt);
                    statement->set_string_label(label->text);
                    break;
                  }

               case V_SgSwitchStatement:
                  {
                    SgSwitchStatement* statement = isSgSwitchStatement(stmt);
                    statement->set_string_label(label->text);
                    break;
                  }

               case V_SgWhileStmt:
                  {
                    SgWhileStmt* statement = isSgWhileStmt(stmt);
                    statement->set_string_label(label->text);
                    break;
                  }

               default:
                  {
                    printf ("Error: set_string_label() not defined for this statement = %p = %s \n",stmt,stmt->class_name().c_str());
                    ROSE_ASSERT(false);
                  }
             }
        }
   }


void
trace_back_through_parent_scopes_searching_for_module (const SgName & moduleName, SgScopeStatement* currentScope, SgClassSymbol* & moduleSymbol )
   {
  // This function traces back through the parent scopes to search for the named module symbol in an outer scope
  // It returns NULL if it is not found in any scope.  It also chases all modules included via SgUseStatements.

     if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
          printf ("In trace_back_through_parent_scopes_searching_for_module(): moduleName = %s currentScope = %p \n",moduleName.str(),currentScope);

  // Testing the scope stack...
  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     std::list<SgScopeStatement*>::iterator scopeInterator = astScopeStack.begin();
     while (scopeInterator != astScopeStack.end())
        {
          if ((*scopeInterator)->isCaseInsensitive() == false)
             {
               printf ("##### Error (in initialize_global_scope_if_required): the scope handling is set to case sensitive scopeInterator = %p = %s \n",*scopeInterator,(*scopeInterator)->class_name().c_str());
            // (*scopeInterator)->setCaseInsensitive(true);
             }
          ROSE_ASSERT((*scopeInterator)->isCaseInsensitive() == true);
          scopeInterator++;
        }

     SgScopeStatement* tempScope = currentScope;

  // DQ (12/12/2007): Added test for if this is a function!
     while (moduleSymbol == NULL && tempScope != NULL)
        {
       // Note that modules are represented at classes.
          moduleSymbol = tempScope->lookup_class_symbol(moduleName);
#if 0
          printf ("In trace_back_through_parent_scopes_searching_for_module(): tempScope = %p = %s moduleSymbol = %p \n",
               tempScope,tempScope->class_name().c_str(),moduleSymbol);
#endif
       // If we have processed the global scope then we can stop (if we have not found the symbol at this
       // point then it is not available (or it is only availalbe through a USE statment.
          tempScope = isSgGlobal(tempScope) ? NULL : tempScope->get_scope();
        }

  // This function could have returned a NULL pointer if there was no symbol found ???
     if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
          printf ("Leaving trace_back_through_parent_scopes_searching_for_module(): moduleSymbol = %p \n",moduleSymbol);
   }

void
trace_back_through_parent_scopes_lookup_variable_symbol_but_do_not_build_variable(const SgName & variableName, SgScopeStatement* currentScope, SgVariableSymbol* & variableSymbol, SgFunctionSymbol* & functionSymbol, SgClassSymbol* & classSymbol)
   {
  // This function traces back through the parent scopes to search for the named symbol in an outer scope
  // It returns NULL if it is not found in any scope.  Is does not look in any scopes specified using a 
  // C++ using declaration (SgUsingDeclarationStatement), using directives (SgUsingDirectiveStatement), a
  // Fortran use statement (SgUseStatement).  This will be done in another function, not yet implemented.

  // DQ (4/29/2008): Added support for detecting SgClassSymbol IR nodes (fro drived types).

     if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
          printf ("In trace_back_through_parent_scopes_lookup_variable_symbol_but_do_not_build_variable(): variableName = %s currentScope = %p = %s \n",variableName.str(),currentScope,currentScope->class_name().c_str());

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("In trace_back_through_parent_scopes_lookup_variable_symbol_but_do_not_build_variable()");
#endif

  // printf ("############### Could this variable = %s be a data member in a module? ############## \n",variableName.str());

  // Testing the scope stack...(debugging code for case insensitive symbol table handling)
  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     std::list<SgScopeStatement*>::iterator scopeInterator = astScopeStack.begin();
     while (scopeInterator != astScopeStack.end())
        {
          if ((*scopeInterator)->isCaseInsensitive() == false)
             {
               printf ("##### Error (in initialize_global_scope_if_required): the scope handling is set to case sensitive scopeInterator = %p = %s \n",*scopeInterator,(*scopeInterator)->class_name().c_str());
            // (*scopeInterator)->setCaseInsensitive(true);
             }
          ROSE_ASSERT((*scopeInterator)->isCaseInsensitive() == true);
          scopeInterator++;
        }

  // DQ (12/21/2010): Reset these to NULL before evaluating the current scoep for a variable, function, or class with the target name.
     variableSymbol = NULL;
     functionSymbol = NULL;
     classSymbol    = NULL;

  // SgVariableSymbol* variableSymbol = NULL;
  // SgFunctionSymbol* functionSymbol = NULL;
     SgScopeStatement* tempScope = currentScope;

  // DQ (12/12/2007): Added test for if this is a function!
  // while (variableSymbol == NULL && tempScope != NULL)
     while (variableSymbol == NULL && functionSymbol == NULL && classSymbol == NULL && tempScope != NULL)
        {
#if 0
          printf ("Searching in scope = %p = %s \n",tempScope,tempScope->class_name().c_str());
          tempScope->get_startOfConstruct()->display("Searching in scope");
#endif
       // DQ (11/26/2010): The variable name that we will search for needs to be case normalized (see test2010_112.f90).
          variableSymbol = tempScope->lookup_variable_symbol(variableName);
          functionSymbol = tempScope->lookup_function_symbol(variableName);
          classSymbol    = tempScope->lookup_class_symbol(variableName);
#if 0
          printf ("In trace_back_through_parent_scopes_lookup_variable_symbol_but_do_not_build_variable(): tempScope = %p = %s variableSymbol = %p functionSymbol = %p classSymbol = %p \n",
               tempScope,tempScope->class_name().c_str(),variableSymbol,functionSymbol,classSymbol);
#endif

#if 0
          tempScope->print_symboltable("In trace_back_through_parent_scopes_lookup_variable_symbol_but_do_not_build_variable()");
#endif

       // If we have processed the global scope then we can stop (if we have not found the symbol at this
       // point then it is not available (or it is only available through a USE statment and we have not 
       // implemented that support yet.
          tempScope = isSgGlobal(tempScope) ? NULL : tempScope->get_scope();
        }

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of trace_back_through_parent_scopes_lookup_variable_symbol_but_do_not_build_variable()");
#endif

#if 1
  // This function could have returned a NULL pointer if there was no symbol found ???
     if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
          printf ("Leaving trace_back_through_parent_scopes_lookup_variable_symbol_but_do_not_build_variable(): variableSymbol = %p functionSymbol = %p \n",variableSymbol,functionSymbol);
#endif
   }

// void build_implicit_program_statement_if_required();

SgVariableSymbol*
trace_back_through_parent_scopes_lookup_variable_symbol(const SgName & variableName, SgScopeStatement* currentScope )
   {
  // This function traces back through the parent scopes to search for the named symbol in an outer scope
  // Semantics of this function:
  //    It returns NULL if the named variable is not found in any scope.
  //    If the variable should be implicitly defined then it is built, but the function still returns NULL.
  // Note that this means that using it to get the symbol for an implicitly defined variable that has not 
  // been used required calling the function twice.

  // Whe looking at scopes this function does not look in any scopes specified using a 
  // C++ using declaration (SgUsingDeclarationStatement), using directives (SgUsingDirectiveStatement), a
  // Fortran use statement (SgUseStatement).  This is handled by having SgAliasSymbol to represent such 
  // symbols that have been imported into the associated scope of the using declarations (or use statement 
  // in fortran).

#if 1
     if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
          printf ("In trace_back_through_parent_scopes_lookup_variable_symbol(): variableName = %s currentScope = %p \n",variableName.str(),currentScope);
#endif

  // An AttributeSpecification statement can be the first statement in a program
  // (see test2007_213.f, if so then this can be one of the first c_action statements called.
  // build_implicit_program_statement_if_required();

     SgVariableSymbol* variableSymbol = NULL;
     SgFunctionSymbol* functionSymbol = NULL;
     SgClassSymbol*    classSymbol    = NULL;

  // trace_back_through_parent_scopes_lookup_variable_symbol_but_do_not_build_variable(variableName,currentScope,variableSymbol,functionSymbol);
     trace_back_through_parent_scopes_lookup_variable_symbol_but_do_not_build_variable(variableName,currentScope,variableSymbol,functionSymbol,classSymbol);

#if 0
     printf ("In trace_back_through_parent_scopes_lookup_variable_symbol(): variableSymbol = %p \n",variableSymbol);
     printf ("In trace_back_through_parent_scopes_lookup_variable_symbol(): functionSymbol = %p \n",functionSymbol);
     printf ("In trace_back_through_parent_scopes_lookup_variable_symbol(): classSymbol    = %p \n",classSymbol);
#endif

#if 0
     printf ("Output the symbol table at the current scope (debugging): \n");
     currentScope->get_symbol_table()->print("Output the symbol table at the current scope");
#endif

     if ( (variableSymbol == NULL) && functionSymbol == NULL && classSymbol == NULL && (matchAgainstIntrinsicFunctionList(variableName.str()) == false) )
        {
       // FMZ(1/8/2010)
       // if the Id name is "team_world" or "team_default", generate team,external:: declarations
          {
            string teamName = variableName.str();
            std::transform(teamName.begin(),teamName.end(),teamName.begin(),::tolower);

            string teamWorld =   "team_world";
            string teamDefault = "team_default";
            if (teamName == teamWorld || teamName == teamDefault)
               {
                 return   add_external_team_decl(teamName);
               }
          }

          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
               printf ("Warning: trace_back_through_parent_scopes_lookup_variable_symbol(): could not locate the specified variable (for %s) in any outer symbol table: astNameStack.size() = %zu \n",variableName.str(),astNameStack.size());
       // printf ("astNameStack.front() = %p = %s = %s \n",astNameStack.front(),astNameStack.front()->class_name().c_str(),SageInterface::get_name(astNameStack.front()).c_str());

          if (astNameStack.empty() == false)
             {
               if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                    printf ("astNameStack.front() = %p = %s \n",astNameStack.front(),astNameStack.front()->text);
             }

       // Check if this is a scope using implicit none rules, however for now even if we know it is function we 
       // first build it as a variable and later convert it to either an array or a function (or a derived type).
          bool isAnImplicitNoneScope = isImplicitNoneScope();
          if (isAnImplicitNoneScope == true)
             {
            // This has to be a function call since we can't be building a variable to represent an implicitly type variable. 
            // See jacobi.f for an example of this. Also, currently test2010_45.f90 demonstrates a case of "integer x = 42, y = x" 
            // where the initializer to "y" is not built as a symbol yet as part of the construction of the variable declaration.
#if 1
               outputState("scope marked as implicit none, so we must construct function in trace_back_through_parent_scopes_lookup_variable_symbol()");
#endif
            // DQ (9/11/2010): Since this must be a function, we can build a function reference expression and a function declaration, but in what scope.
            // Or we could define a list of unresolved functions and fix them up at the end of the module or global scope.  This is a general problem
            // and demonstrated by test2010_46.f90 
               if (astNameStack.empty() != false)
                    cerr << "Error: name stack is empty when handling variable:"<<variableName.str() << endl;
               ROSE_ASSERT(astNameStack.empty() == false);
               ROSE_ASSERT(astNameStack.front()->text != NULL);

               SgName name = astNameStack.front()->text;
            // astNameStack.pop_front();

            // Define this as a function returning void type for now, but it will have to be fixed when we finally see the function definition.
               SgFunctionType* functionType             = new SgFunctionType (SgTypeVoid::createType());
               SgFunctionDefinition* functionDefinition = NULL;

               SgProcedureHeaderStatement* functionDeclaration = new SgProcedureHeaderStatement(name,functionType,functionDefinition);
               ROSE_ASSERT(functionDeclaration != NULL);

            // We have not yet seen the declaration for this function so it can only be marked as compiler gnerated for now!
               setSourcePosition(functionDeclaration);
               setSourcePosition(functionDeclaration->get_parameterList());

               ROSE_ASSERT(functionDeclaration->get_endOfConstruct()   != NULL);
               ROSE_ASSERT(functionDeclaration->get_startOfConstruct() != NULL);

               setSourcePositionCompilerGenerated(functionDeclaration);
               setSourcePositionCompilerGenerated(functionDeclaration->get_parameterList());

               ROSE_ASSERT(functionDeclaration->get_endOfConstruct()   != NULL);
               ROSE_ASSERT(functionDeclaration->get_startOfConstruct() != NULL);

               ROSE_ASSERT(functionSymbol == NULL);
               functionSymbol = new SgFunctionSymbol(functionDeclaration);
               ROSE_ASSERT(functionSymbol != NULL);

               ROSE_ASSERT(astScopeStack.empty() == false);
               SgScopeStatement* scope = astScopeStack.back();
               ROSE_ASSERT(scope != NULL);
               SgGlobal* globalScope = isSgGlobal(scope);
               ROSE_ASSERT(globalScope != NULL);

            // Set the scope to be global scope since we have not yet seen the function definition!
            // If it was from a module, then the module should have been included. If we were in a 
            // module then this should be fixed up at the end of the module scope (and we will have 
            // seen the function definition by then).  If this needs to be fixed up in global scope 
            // then we will see the function definition by then (at the end of the translation unit).
               functionDeclaration->set_scope(globalScope);

            // We also have to set the first non-defining declaration.
               functionDeclaration->set_firstNondefiningDeclaration(functionDeclaration);

            // We also have to set the parent...
               ROSE_ASSERT(functionDeclaration->get_parent() == NULL);
               functionDeclaration->set_parent(globalScope);
               ROSE_ASSERT(functionDeclaration->get_parent() != NULL);

            // printf ("Adding function name = %s to the global scope (even though we have not seen the definition yet) \n",name.str());
               globalScope->insert_symbol(name,functionSymbol);

            // Add this function to the list of unresolved functions so that we can fixup the AST afterward (close of module scope or close of global scope).
               astUnresolvedFunctionsList.push_front(functionDeclaration);
#if 0
               printf ("Error: scope marked as implict none, so we must construct name = %s as a function (function reference expression) \n",variableName.str());
               ROSE_ASSERT(false);
#endif
             }
            else
             {
            // printf ("NOTE: CANCLED SEMANTICS THAT BUILD IMPLICIT VARIABLE SYMBOLS \n");
             }
        }

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of trace_back_through_parent_scopes_lookup_variable_symbol()");
#endif

#if 1
  // This function could have returned a NULL pointer if there was no symbol found ???
     if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
          printf ("Leaving trace_back_through_parent_scopes_lookup_variable_symbol(): variableSymbol = %p functionSymbol = %p \n",variableSymbol,functionSymbol);
#endif

     return variableSymbol;
   }


// DQ (12/27/2010): We have to maek the return type std::vector<SgSymbol*> so that we have 
// include SgFunctionSymbols to handle function return value initialization for defined types.
// DQ (12/14/2010): New support for structure members.
// std::vector<SgVariableSymbol*>
// std::vector<SgSymbol*> trace_back_through_parent_scopes_lookup_member_variable_symbol(const std::vector<std::string> & qualifiedNameList, SgScopeStatement* currentScope )
std::vector<SgSymbol*>
trace_back_through_parent_scopes_lookup_member_variable_symbol(const std::vector<MultipartReferenceType> & qualifiedNameList, SgScopeStatement* currentScope )
   {
  // This function takes an array of names representing multi-part references generated from repeated calls to R613.
  // This function permits structured data member lookup to be uniform with simple variable lookup.
  // The non-recursive implementation that this allows for R612 makes it easer to debug this code.

     SgVariableSymbol* variableSymbol = NULL;
     SgFunctionSymbol* functionSymbol = NULL;
     SgClassSymbol*    classSymbol    = NULL;

  // std::vector<SgVariableSymbol*> returnSymbolList;
     std::vector<SgSymbol*> returnSymbolList;

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of trace_back_through_parent_scopes_lookup_member_variable_symbol(const std::vector<std::string>,SgScopeStatement*)");
#endif

#if 0
     printf ("qualifiedNameList name = %s \n",generateQualifiedName(qualifiedNameList).c_str());
#endif

        {
       // This is the more sophisticated case (multi-part references)...
       // We actually want to call the version of the function that will not create a new implicit variable.
          SgScopeStatement* structureScope = currentScope;
          ROSE_ASSERT(structureScope != NULL);

          string name;
       // size_t i = 0;
       // while (i < qualifiedNameList.size())
          for (size_t i = 0; i < qualifiedNameList.size(); i++)
             {
            // name = qualifiedNameList[i];
               name = qualifiedNameList[i].name;
#if 0
               printf ("structureScope = %p = %s name = %s \n",structureScope,structureScope->class_name().c_str(),name.c_str());
#endif
#if 0
               printf ("Output the symbol table at the structureScope (debugging i = %zu): \n",i);
               structureScope->get_symbol_table()->print("Output the symbol table at the current scope");
#endif

            // variableSymbol = trace_back_through_parent_scopes_lookup_variable_symbol(qualifiedNameList[i],structureScope);
            // trace_back_through_parent_scopes_lookup_variable_symbol_but_do_not_build_variable(qualifiedNameList[i],structureScope,variableSymbol,functionSymbol,classSymbol);
               trace_back_through_parent_scopes_lookup_variable_symbol_but_do_not_build_variable(name,structureScope,variableSymbol,functionSymbol,classSymbol);

#if 0
               printf ("In trace_back_through_parent_scopes_lookup_member_variable_symbol(): variableSymbol = %p \n",variableSymbol);
               printf ("In trace_back_through_parent_scopes_lookup_member_variable_symbol(): functionSymbol = %p \n",functionSymbol);
               printf ("In trace_back_through_parent_scopes_lookup_member_variable_symbol(): classSymbol    = %p \n",classSymbol);
#endif

               if (variableSymbol != NULL)
                  {
                 // ROSE_ASSERT(variableSymbol->get_type() != NULL);
#if 0
                    printf ("variable type = %s \n",variableSymbol->get_type()->class_name().c_str());
#endif
                 // This is a reference to a variable (perhaps a structure), and we can return variableSymbol as NULL.
                 // name = qualifiedNameList[i];
#if 0
                    printf ("Found a variableSymbol = %p Next variable name = %s \n",variableSymbol,name.c_str());
#endif
                    SgType* type = variableSymbol->get_type();
                    ROSE_ASSERT(type != NULL);
#if 0
                    printf ("associated variable type = %s \n",type->class_name().c_str());
#endif
                 // DQ (12/23/2010): I think a better implementation would strip off SgArrayType and SgPointerType as needed to get the the SgClassType
                 // I am unclear how many cases we will have to handle here.

                 // DQ (12/29/2010): I promise to refactor this code as soon as possible (but maybe after vacation)!
                 // printf ("Refactor this code!!! \n");

                    switch (type->variantT())
                       {
                      // This type will continue the search fo multi-type references.
                         case V_SgClassType:
                            {
                              SgClassType* classType = isSgClassType(type);
                              ROSE_ASSERT(classType->get_declaration() != NULL);
                              SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
                              ROSE_ASSERT(classDeclaration != NULL);

                           // Get the defining declaration!
                              SgClassDeclaration* definingClassDeclaration = isSgClassDeclaration(classDeclaration->get_definingDeclaration());
                              ROSE_ASSERT(definingClassDeclaration != NULL);
                              SgClassDefinition* classDefinition = definingClassDeclaration->get_definition();
                              ROSE_ASSERT(classDefinition != NULL);

                              structureScope = classDefinition;
                           // printf ("Set structureScope to %p (case of class type) \n",structureScope);
                              break;
                            }

                         case V_SgArrayType:
                            {
                              SgArrayType* arrayType = isSgArrayType(type);
                              SgType* baseType = arrayType->get_base_type();
                              ROSE_ASSERT(baseType != NULL);
#if 0
                              printf ("baseType = %p = %s \n",baseType,baseType->class_name().c_str());
#endif
                              SgClassType* classType = isSgClassType(baseType);
                              if (classType != NULL)
                                 {
                                   ROSE_ASSERT(classType->get_declaration() != NULL);
                                   SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
                                   ROSE_ASSERT(classDeclaration != NULL);

                                // Get the defining declaration!
                                   SgClassDeclaration* definingClassDeclaration = isSgClassDeclaration(classDeclaration->get_definingDeclaration());
                                   ROSE_ASSERT(definingClassDeclaration != NULL);
                                   SgClassDefinition* classDefinition = definingClassDeclaration->get_definition();
                                   ROSE_ASSERT(classDefinition != NULL);

                                   structureScope = classDefinition;
                                 }
                                else
                                 {
                                   SgPointerType* pointerType = isSgPointerType(baseType);
                                   if (pointerType != NULL)
                                      {
                                     // printf ("Case of array of pointers \n");
                                     // ROSE_ASSERT(false);

                                        SgType* baseType = pointerType->get_base_type();
                                        ROSE_ASSERT(baseType != NULL);
#if 0
                                        printf ("baseType = %p = %s \n",baseType,baseType->class_name().c_str());
#endif
                                        SgClassType* classType = isSgClassType(baseType);
                                        if (classType != NULL)
                                           {
                                             ROSE_ASSERT(classType->get_declaration() != NULL);
                                             SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
                                             ROSE_ASSERT(classDeclaration != NULL);

                                          // Get the defining declaration!
                                             SgClassDeclaration* definingClassDeclaration = isSgClassDeclaration(classDeclaration->get_definingDeclaration());
                                             ROSE_ASSERT(definingClassDeclaration != NULL);
                                             SgClassDefinition* classDefinition = definingClassDeclaration->get_definition();
                                             ROSE_ASSERT(classDefinition != NULL);

                                             structureScope = classDefinition;
                                           }
                                          else
                                           {
                                             structureScope = NULL;
                                           }
                                      }
                                     else
                                      {
                                        structureScope = NULL;
                                      }
                                 }
                           // printf ("Set structureScope to %p (case of array type) \n",structureScope);
                              break;
                            }

                         case V_SgPointerType:
                            {
                              SgPointerType* pointerType = isSgPointerType(type);
                              SgType* baseType = pointerType->get_base_type();
                              ROSE_ASSERT(baseType != NULL);
#if 0
                              printf ("baseType = %p = %s \n",baseType,baseType->class_name().c_str());
#endif
                           // This is the same code for handling the class type as in the "case V_SgArrayType:" above.
                              SgClassType* classType = isSgClassType(baseType);
                              if (classType != NULL)
                                 {
                                   ROSE_ASSERT(classType->get_declaration() != NULL);
                                   SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
                                   ROSE_ASSERT(classDeclaration != NULL);

                                // Get the defining declaration!
                                   SgClassDeclaration* definingClassDeclaration = isSgClassDeclaration(classDeclaration->get_definingDeclaration());
                                   ROSE_ASSERT(definingClassDeclaration != NULL);
                                   SgClassDefinition* classDefinition = definingClassDeclaration->get_definition();
                                   ROSE_ASSERT(classDefinition != NULL);

                                   structureScope = classDefinition;
                                 }
                                else
                                 {
                                // Note that this could be recursive...
                                // structureScope = NULL;
                                   SgArrayType* arrayType = isSgArrayType(baseType);
                                   if (arrayType != NULL)
                                      {
                                        SgType* baseType = arrayType->get_base_type();
                                        ROSE_ASSERT(baseType != NULL);
#if 0
                                        printf ("In the array: baseType = %p = %s \n",baseType,baseType->class_name().c_str());
#endif
                                        SgClassType* classType = isSgClassType(baseType);
                                        if (classType != NULL)
                                           {
                                             ROSE_ASSERT(classType->get_declaration() != NULL);
                                             SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
                                             ROSE_ASSERT(classDeclaration != NULL);

                                          // Get the defining declaration!
                                             SgClassDeclaration* definingClassDeclaration = isSgClassDeclaration(classDeclaration->get_definingDeclaration());
                                             ROSE_ASSERT(definingClassDeclaration != NULL);
                                             SgClassDefinition* classDefinition = definingClassDeclaration->get_definition();
                                             ROSE_ASSERT(classDefinition != NULL);

                                             structureScope = classDefinition;
                                           }
                                          else
                                           {
                                          // DQ (1/13/2011): Added checking for possible SgTypeDefault that should have been fixed up by fixup_forward_type_declarations().
                                             SgTypeDefault* defaultType = isSgTypeDefault(baseType);
                                             if (defaultType != NULL)
                                                {
                                                  printf ("Error: SgTypeDefault identified in array base-type (result of unfixed up reference to type defined after initial reference). \n");
                                               // ROSE_ASSERT(false);
                                                }
                                               else
                                                {
                                                  structureScope = NULL;
                                                }
                                           }
                                      }
                                     else
                                      {
                                        structureScope = NULL;
                                      }
                                 }
                           // printf ("Set structureScope to %p (case of pointr type) \n",structureScope);
                              break;
                            }

                      // DQ (12/29/2010): Added new types...
                         case V_SgTypeDouble:
                         case V_SgTypeChar:

                      // These types will terminate the search for names from multi-part references.
                         case V_SgTypeString:
                         case V_SgTypeFloat:
                         case V_SgTypeBool:
                         case V_SgTypeInt:
                            {
                              structureScope = NULL;
                           // printf ("Set structureScope to NULL (for primative types) \n");
                              break;
                            }

                      // All currently unhandled types
                         default:
                            {
                              structureScope = NULL;
                           // printf ("Set structureScope to NULL \n");
                           // printf ("Warning: what is this type, associated variable type = %s \n",type->class_name().c_str());
                           // ROSE_ASSERT(false);
                            }
                       }

                 // Returning an empty list (returnSymbolList) is how we would return the equivalant of "variableSymbol == NULL".
                    if (variableSymbol != NULL)
                       {
                         returnSymbolList.push_back(variableSymbol);
                       }
                  }
                 else
                  {
#if 0
                    printf ("In trace_back_through_parent_scopes_lookup_member_variable_symbol(): functionSymbol = %p \n",functionSymbol);
#endif
                    if (functionSymbol != NULL)
                       {
                      // This is a reference to a function (maybe a member function of a module?), and we can return variableSymbol as NULL.
                      // See test2010_176.f90 for an example of this (setting a data member of the return value of a function returning a type!
                      // name = qualifiedNameList[i];
#if 0
                         printf ("Found a functionSymbol = %p Next variable name = %s \n",functionSymbol,name.c_str());
#endif
                         SgType* type = functionSymbol->get_type();
                         ROSE_ASSERT(type != NULL);
                         SgFunctionType* functionType = isSgFunctionType(type);
                         ROSE_ASSERT(functionType != NULL);
                         SgType* functionReturnType = functionType->get_return_type();
                         ROSE_ASSERT(functionReturnType != NULL);
#if 0
                         printf ("functionReturnType = %p = %s \n",functionReturnType,functionReturnType->class_name().c_str());
#endif
                         SgClassType* classType = isSgClassType(functionReturnType);
                         if (classType != NULL)
                            {
                           // printf ("Found a function with SgClassType return type! \n");
                              ROSE_ASSERT(classType->get_declaration() != NULL);
                              SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
                              ROSE_ASSERT(classDeclaration != NULL);

                           // Get the defining declaration!
                              SgClassDeclaration* definingClassDeclaration = isSgClassDeclaration(classDeclaration->get_definingDeclaration());
                              ROSE_ASSERT(definingClassDeclaration != NULL);
                              SgClassDefinition* classDefinition = definingClassDeclaration->get_definition();
                              ROSE_ASSERT(classDefinition != NULL);

                              structureScope = classDefinition;
                           // printf ("Set structureScope to %p \n",structureScope);

                              returnSymbolList.push_back(functionSymbol);
                            }
                           else
                            {
                              structureScope = NULL;

                           // See test2007_07.f90 for where this is required.
                              returnSymbolList.push_back(functionSymbol);
                            }
                       }
                      else
                       {
#if 0
                         printf ("In trace_back_through_parent_scopes_lookup_member_variable_symbol(): classSymbol = %p \n",classSymbol);
#endif
                         if (classSymbol != NULL)
                            {
                           // This is a reference to a type, and we can return variableSymbol as NULL.
                           // printf ("Found a classSymbol = %p \n",classSymbol);
                              structureScope = NULL;
                            }
                           else
                            {
#if 0
                               printf ("########## This is reference has not been seen previously: name = %s qualifiedNameList.size() = %zu \n",name.c_str(),qualifiedNameList.size());
#endif
                           // Nothing was found, so we can return variableSymbol as NULL.
                           // Note: types could be buried (modules defined in modules), but they are not likely a part of multi-part references (in Fortran).
                              structureScope = NULL;

                           // DQ (12/28/2010): Added handling for implicit references.
                           // If nothing was found then this is an implicit reference (could be a variable or array reference, I think).
                           // But to be an implicit reference it must have only one part (qualifiedNameList.size() == 1).
                              if (qualifiedNameList.size() == 1)
                                 {
                                // DQ (1/18/2011): This detects where we have used the semantics of implicitly building symbols for implicit variables.
                                // printf ("WARNING: This use of trace_back_through_parent_scopes_lookup_variable_symbol() used the side effect of building a symbol if the reference is not found! \n");
                                // ROSE_ASSERT(false);

                                // This will build the variable symbol if the variable is not found.
                                // variableSymbol = trace_back_through_parent_scopes_lookup_variable_symbol(qualifiedNameList[0],currentScope);
                                   variableSymbol = trace_back_through_parent_scopes_lookup_variable_symbol(name,currentScope);

                                // Returning an empty list (returnSymbolList) is how we would return the equivalant of "variableSymbol == NULL".
                                   if (variableSymbol != NULL)
                                      {
                                        returnSymbolList.push_back(variableSymbol);
                                      }
                                     else
                                      {
                                     // I think this may be an error...output a message for now.
#if 0
                                        printf ("Warning: variable symbol not built for expected implicit reference = %s \n",name.c_str());
#endif
#if 0
                                     // Returning a SgDefaultSymbol will be used to indicate that the name is not known and can be interpreted later.
                                        SgSymbol* defaultSymbol = new SgDefaultSymbol();
                                        returnSymbolList.push_back(defaultSymbol);
#endif
                                      }
                                 }
                            }
                       }
                  }

            // Error checking...
               if (structureScope == NULL)
                  {
                 // This should be the last iteration!
                    if (i != (qualifiedNameList.size() - 1))
                       {
                         printf ("WARNING: i != (qualifiedNameList.size() - 1) for LANL_POP code only! (i = %zu qualifiedNameList.size() = %zu) \n",i,qualifiedNameList.size());

                      // Debugging...
                         printf ("Leaving trace_back_through_parent_scopes_lookup_member_variable_symbol(): \n");
                         for (size_t i = 0; i < returnSymbolList.size(); i++)
                            {
                               printf ("--- returnSymbolList[%zu] = %p = %s = %s \n",i,returnSymbolList[i],returnSymbolList[i]->class_name().c_str(),returnSymbolList[i]->get_name().str());
                            }
                       }

                 // Note that if this fails is it always because there was some case missed above (and so the structureScope was not set properly to permit the search to be continued resolve a name in a nested type or scope).
                    ROSE_ASSERT(i == (qualifiedNameList.size() - 1));
                  }

            // End of "for loop" over multi-part name parts.
             }
        }

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of trace_back_through_parent_scopes_lookup_member_variable_symbol(const std::vector<std::string>,SgScopeStatement*)");
#endif

#if 1
  // This function could have returned a NULL pointer if there was no symbol found ???
     if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
        {
          printf ("Leaving trace_back_through_parent_scopes_lookup_member_variable_symbol(): \n");
          for (size_t i = 0; i < returnSymbolList.size(); i++)
             {
               printf ("--- returnSymbolList[%zu] = %p = %s \n",i,returnSymbolList[i],returnSymbolList[i]->class_name().c_str());
             }
        }
#endif

#if 0
  // DQ (1/19/2011): I think this is valid debugging code.
     if (returnSymbolList.empty() == true)
        {
          printf ("*** WARNING: returnSymbolList is empty (might be an error) *** \n");
        }
#endif

  // DQ (12/28/2010): Fixed this test to handle case of single part implicit references.
  // DQ (12/27/2010): Can we assert this?  Maybe not for implicitly declared variables (which are by definition only a single part, not multi-part).
  // if (returnSymbolList.size() != qualifiedNameList.size())
     if (qualifiedNameList.size() > 1 && returnSymbolList.size() != qualifiedNameList.size())
        {
          printf ("Error: returnSymbolList.size() = %zu qualifiedNameList.size() = %zu \n",returnSymbolList.size(),qualifiedNameList.size());
        }
  // ROSE_ASSERT(returnSymbolList.size() == qualifiedNameList.size());
     ROSE_ASSERT(qualifiedNameList.size() == 1 || returnSymbolList.size() == qualifiedNameList.size());

     return returnSymbolList;
   }


void
buildImplicitVariableDeclaration( const SgName & variableName )
   {
     Token_t token;
     token.line = 1;
     token.col  = 1;
     token.type = 0;
     token.text = strdup(variableName.str());

  // Push the name onto the stack
     if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
          printf ("Push the name onto the astNameStack \n");

     astNameStack.push_front(&token);
     ROSE_ASSERT(astNameStack.empty() == false);

  // DQ (12/20/2007): The type here must be determined using implicit type rules.
     SgType* intrinsicType = generateImplicitType(variableName.str());
     ROSE_ASSERT(intrinsicType != NULL);

     astTypeStack.push_front(intrinsicType);

     if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
          printf ("Calling buildVariableDeclaration to build an implicitly defined variable: name = %s type = %s astNameStack.size() = %zu \n",variableName.str(),intrinsicType->class_name().c_str(),astNameStack.size());

     ROSE_ASSERT(astTypeStack.empty() == false);
     SgType* type = astTypeStack.front();
     astTypeStack.pop_front();

     SgInitializedName* initializedName = new SgInitializedName(variableName,type,NULL,NULL,NULL);
     setSourcePosition(initializedName);

  // printf ("Built a new SgInitializedName = %p = %s \n",initializedName,variableName.str());
  // DQ (12/14/2007): This will be set in buildVariableDeclaration()
  // initializedName->set_scope(currentScope);

     astNameStack.pop_front();
     astNodeStack.push_front(initializedName);
#if 0
  // Output debugging information about saved state (stack) information.
     outputState("Calling buildVariableDeclaration to build an implicitly defined variable from trace_back_through_parent_scopes_lookup_variable_symbol()");
#endif

  // We need to explicitly specify that the variable is to be implicitly declarated (so that we will know to process only one variable at a time).
     bool buildingImplicitVariable = true;
     SgVariableDeclaration* variableDeclaration = buildVariableDeclaration(NULL,buildingImplicitVariable);
     ROSE_ASSERT(variableDeclaration != NULL);

     if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
          printf ("DONE: Calling buildVariableDeclaration to build an implicitly defined variable \n");

  // ROSE_ASSERT(variableDeclaration->get_file_info()->isCompilerGenerated() == true);

  // Set this implicitly defined variable declaration to be compiler generated.
  // setSourcePositionCompilerGenerated(variableDeclaration);

     ROSE_ASSERT(variableDeclaration->get_startOfConstruct() != NULL);
     ROSE_ASSERT(variableDeclaration->get_endOfConstruct() != NULL);

#if 0
     if (variableDeclaration->get_startOfConstruct()->get_filenameString() == "NULL_FILE")
        {
          variableDeclaration->get_startOfConstruct()->display("Implicit variable declaration within trace_back_through_parent_scopes_lookup_variable_symbol()");
        }
  // ROSE_ASSERT(variableDeclaration->get_startOfConstruct()->get_filenameString() != "NULL_FILE");
#endif

  // DQ (12/17/2007): Make sure the scope was set!
     ROSE_ASSERT(initializedName->get_scope() != NULL);

  // Set the variableSymbol we want to return...
  // ROSE_ASSERT(initializedName->get_symbol_from_symbol_table() != NULL);
     SgSymbol* tempSymbol = initializedName->get_symbol_from_symbol_table();
     ROSE_ASSERT(tempSymbol != NULL);

  // DQ (1/17/2011): Adding an additional test based on debugging test2007_94.f90.
     ROSE_ASSERT(initializedName->get_scope()->lookup_variable_symbol(variableName) != NULL);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of building an implicitly defined variable from trace_back_through_parent_scopes_lookup_variable_symbol()");
#endif
   }



SgClassSymbol*
trace_back_through_parent_scopes_lookup_derived_type_symbol(const SgName & derivedTypeName, SgScopeStatement* currentScope )
   {
  // This function traces back through the parent scopes to search for the named symbol in an outer scope
  // It retuens NULL if it is not found in any scope.  Is does not look in any scopes specified using a 
  // C++ using declaration (SgUsingDeclarationStatement), using directives (SgUsingDirectiveStatement), a
  // Fortran use statement (SgUseStatement).  This will be done in another function, not yet implemented.

     SgClassSymbol* derivedTypeSymbol = NULL;
     SgScopeStatement* tempScope = currentScope;
     while (derivedTypeSymbol == NULL && tempScope != NULL)
        {
          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
               printf ("In trace_back_through_parent_scopes_lookup_derived_type_symbol(): tempScope = %p = %s = %s \n",
                    tempScope,tempScope->class_name().c_str(),SageInterface::get_name(tempScope).c_str());

       // We have to specify the class symbol since there could be other types (or declarations) with the same name, at least in C++ (unclear about Fortran 90).
          derivedTypeSymbol = tempScope->lookup_class_symbol(derivedTypeName);

       // If we have processed the global scope then we can stop (if we have not found the symbol at this
       // point then it is not available (or it is only availalbe through a USE statment and we have not 
       // implemented that support yet.
       // tempScope = tempScope->get_scope();
          tempScope = isSgGlobal(tempScope) ? NULL : tempScope->get_scope();
        }

  // DQ (4/30/2008): I think it is not a problem to return a NULL pointer if no derived type was found (commented out assertion).
     if (derivedTypeSymbol == NULL)
        {
          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
               printf ("Warning: trace_back_through_parent_scopes_lookup_derived_type_symbol(): could not locate the specified derived type %s in any outer symbol table \n",derivedTypeName.str());
       // ROSE_ASSERT(false);
        }

     return derivedTypeSymbol;
   }

SgFunctionSymbol*
trace_back_through_parent_scopes_lookup_function_symbol(const SgName & functionName, SgScopeStatement* currentScope )
   {
  // DQ (11/24/2007): This function can return NULL.  It returns NULL when the function symbol is not found.
  // This can happen when a function is referenced before it it defined (no prototype mechanism in Fortran is required).

  // This function was moved to the SageInteface so that the astPostProcessing could call it too.
  // return SageInterface::lookupFunctionSymbolInParentScopes(functionName,currentScope);
  // printf ("Calling SageInterface::lookupFunctionSymbolInParentScopes(%s,%p = %s) \n",functionName.str(),currentScope,currentScope->class_name().c_str());
     SgFunctionSymbol* functionSymbol = SageInterface::lookupFunctionSymbolInParentScopes(functionName,currentScope);

     if (functionSymbol != NULL)
        {
          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
               printf ("Found a function for functionName = %s functionSymbol = %p \n",functionName.str(),functionSymbol);
        }
       else
        {
       // printf ("In trace_back_through_parent_scopes_lookup_function_symbol(): Could not find function symbol for name = %s \n",functionName.str());
        }

     return functionSymbol;
   }


SgModuleStatement*
buildModuleStatementAndDefinition (string name, SgScopeStatement* scope)
   {
  // This function builds a class declaration and definition 
  // (both the defining and nondefining declarations as required).

  // This is the class definition (the fileInfo is the position of the opening brace)
     SgClassDefinition* classDefinition   = new SgClassDefinition();
     assert(classDefinition != NULL);

  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     classDefinition->setCaseInsensitive(true);

  // classDefinition->set_endOfConstruct(SOURCE_POSITION);
     setSourcePosition(classDefinition);

  // Set the end of construct explictly (where not a transformation this is the location of the closing brace)
  // classDefinition->set_endOfConstruct(SOURCE_POSITION);

  // This is the defining declaration for the class (with a reference to the class definition)
     SgModuleStatement* classDeclaration = new SgModuleStatement(name.c_str(),SgClassDeclaration::e_struct,NULL,classDefinition);
     assert(classDeclaration != NULL);
  // classDeclaration->set_endOfConstruct(SOURCE_POSITION);

  // This is set later when the source position is more accurately known
  // setSourcePosition(classDeclaration);

  // Set the defining declaration in the defining declaration!
     classDeclaration->set_definingDeclaration(classDeclaration);

  // Set the non defining declaration in the defining declaration (both are required)
     SgModuleStatement* nondefiningClassDeclaration = new SgModuleStatement(name.c_str(),SgClassDeclaration::e_struct,NULL,NULL);
     assert(classDeclaration != NULL);
  // nondefiningClassDeclaration->set_endOfConstruct(SOURCE_POSITION);
     setSourcePosition(nondefiningClassDeclaration);

 // Liao 10/30/2009. we now ask for explicit creation of SgClassType. The constructor will not create it by default
     if (nondefiningClassDeclaration->get_type () == NULL)
          nondefiningClassDeclaration->set_type (SgClassType::createType(nondefiningClassDeclaration));
     classDeclaration->set_type(nondefiningClassDeclaration->get_type());

  // Set the internal reference to the non-defining declaration
     classDeclaration->set_firstNondefiningDeclaration(nondefiningClassDeclaration);

  // Set the parent explicitly
     nondefiningClassDeclaration->set_parent(scope);

  // Set the defining and no-defining declarations in the non-defining class declaration!
     nondefiningClassDeclaration->set_firstNondefiningDeclaration(nondefiningClassDeclaration);
     nondefiningClassDeclaration->set_definingDeclaration(classDeclaration);

  // Set the nondefining declaration as a forward declaration!
     nondefiningClassDeclaration->setForward();

  // Don't forget the set the declaration in the definition (IR node constructors are side-effect free!)!
     classDefinition->set_declaration(classDeclaration);

  // set the scope explicitly (name qualification tricks can imply it is not always the parent IR node!)
     classDeclaration->set_scope(scope);
     nondefiningClassDeclaration->set_scope(scope);

  // Set the parent explicitly
     classDeclaration->set_parent(scope);

  // A type should have been build at this point, since we will need it later!
     ROSE_ASSERT(classDeclaration->get_type() != NULL);

  // We use the nondefiningClassDeclaration, though it might be that for Fortran the rules that cause this to be important are not so complex as for C/C++.
     SgClassSymbol* classSymbol = new SgClassSymbol(nondefiningClassDeclaration);

  // Add the symbol to the current scope (the specified input scope)
     scope->insert_symbol(name,classSymbol);

     ROSE_ASSERT(scope->lookup_class_symbol(name) != NULL);

  // some error checking
     assert(classDeclaration->get_definingDeclaration() != NULL);
     assert(classDeclaration->get_firstNondefiningDeclaration() != NULL);
     assert(classDeclaration->get_definition() != NULL);

     ROSE_ASSERT(classDeclaration->get_definition()->get_parent() != NULL);

     return classDeclaration;
   }


SgDerivedTypeStatement*
buildDerivedTypeStatementAndDefinition (string name, SgScopeStatement* scope)
   {
  // This function builds a class declaration and definition 
  // (both the defining and nondefining declarations as required).

  // This is the class definition (the fileInfo is the position of the opening brace)
     SgClassDefinition* classDefinition   = new SgClassDefinition();
     assert(classDefinition != NULL);

  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     classDefinition->setCaseInsensitive(true);

  // classDefinition->set_endOfConstruct(SOURCE_POSITION);
     setSourcePosition(classDefinition);

  // Set the end of construct explictly (where not a transformation this is the location of the closing brace)
  // classDefinition->set_endOfConstruct(SOURCE_POSITION);

  // This is the defining declaration for the class (with a reference to the class definition)
     SgDerivedTypeStatement* classDeclaration = new SgDerivedTypeStatement(name.c_str(),SgClassDeclaration::e_struct,NULL,classDefinition);
     assert(classDeclaration != NULL);
  // classDeclaration->set_endOfConstruct(SOURCE_POSITION);

  // This will be set later when the source position is known
  // setSourcePosition(classDeclaration);

  // Set the defining declaration in the defining declaration!
     classDeclaration->set_definingDeclaration(classDeclaration);

  // Set the non defining declaration in the defining declaration (both are required)
     SgDerivedTypeStatement* nondefiningClassDeclaration = new SgDerivedTypeStatement(name.c_str(),SgClassDeclaration::e_struct,NULL,NULL);
     assert(classDeclaration != NULL);

  // DQ (12/27/2010): Set the parent before calling the SgClassType::createType() since then name mangling will require it.
  // Set the parent explicitly
     nondefiningClassDeclaration->set_parent(scope);

  // Liao 10/30/2009. we now ask for explicit creation of SgClassType. The constructor will not create it by default
     if (nondefiningClassDeclaration->get_type () == NULL) 
          nondefiningClassDeclaration->set_type (SgClassType::createType(nondefiningClassDeclaration));
     classDeclaration->set_type(nondefiningClassDeclaration->get_type());

  // nondefiningClassDeclaration->set_endOfConstruct(SOURCE_POSITION);

  // Leave the nondefining declaration without a specific source code position.
     setSourcePosition(nondefiningClassDeclaration);

  // Set the internal reference to the non-defining declaration
     classDeclaration->set_firstNondefiningDeclaration(nondefiningClassDeclaration);

  // DQ (12/27/2010): Moved to before call to SgClassType::createType().
  // Set the parent explicitly
  // nondefiningClassDeclaration->set_parent(scope);

  // Set the defining and no-defining declarations in the non-defining class declaration!
     nondefiningClassDeclaration->set_firstNondefiningDeclaration(nondefiningClassDeclaration);
     nondefiningClassDeclaration->set_definingDeclaration(classDeclaration);

  // Set the nondefining declaration as a forward declaration!
     nondefiningClassDeclaration->setForward();

  // Don't forget the set the declaration in the definition (IR node constructors are side-effect free!)!
     classDefinition->set_declaration(classDeclaration);

  // set the scope explicitly (name qualification tricks can imply it is not always the parent IR node!)
     classDeclaration->set_scope(scope);
     nondefiningClassDeclaration->set_scope(scope);

  // Set the parent explicitly
     classDeclaration->set_parent(scope);

  // A type should have been build at this point, since we will need it later!
     ROSE_ASSERT(classDeclaration->get_type() != NULL);

  // We use the nondefiningClassDeclaration, though it might be that for Fortran the rules that cause this to be important are not so complex as for C/C++.
     SgClassSymbol* classSymbol = new SgClassSymbol(nondefiningClassDeclaration);

  // Add the symbol to the current scope (the specified input scope)
     scope->insert_symbol(name,classSymbol);

     ROSE_ASSERT(scope->lookup_class_symbol(name) != NULL);

  // some error checking
     assert(classDeclaration->get_definingDeclaration() != NULL);
     assert(classDeclaration->get_firstNondefiningDeclaration() != NULL);
     assert(classDeclaration->get_definition() != NULL);

     ROSE_ASSERT(classDeclaration->get_definition()->get_parent() != NULL);

  // DQ (8/28/2010): Save the attributes used and clear the astAttributeSpecStack for this declaration (see test2010_34.f90).
     while (astAttributeSpecStack.empty() == false)
        {
#if 0
          printf ("In buildDerivedTypeStatementAndDefinition(): Process attribute spec %d \n",astAttributeSpecStack.front());
#endif
          setDeclarationAttributeSpec(classDeclaration,astAttributeSpecStack.front());

          if (astAttributeSpecStack.front() == AttrSpec_PUBLIC || astAttributeSpecStack.front() == AttrSpec_PRIVATE)
             {
            // printf ("astNameStack.size() = %zu \n",astNameStack.size());
               if (astNameStack.empty() == false)
                  {
                    string type_attribute_string = astNameStack.front()->text;
                 // printf ("type_attribute_string = %s \n",type_attribute_string.c_str());
                    astNameStack.pop_front();
                  }
             }

          astAttributeSpecStack.pop_front();
        }

     return classDeclaration;
   }


bool
isImplicitNoneScope()
   {
  // This function works by traversing the scopes to the SgFunctionDefinition scope and looking
  // for an implicit none statement.  We can make this more efficient at a later point.

  // Default is that we are NOT in an implicit none scope.
     bool isImplicitNoneScope = false;

     bool foundFunctionDefinition = false;

     std::list<SgScopeStatement*>::iterator i = astScopeStack.begin();
     while ( i != astScopeStack.end() )
        {
       // Find the function definition, or where else an implicit statement can be!
       // module statement, interface statement

          SgFunctionDefinition* functionDefinition = isSgFunctionDefinition(*i);
          if (functionDefinition != NULL)
             {
               foundFunctionDefinition = true;
               std::vector<SgStatement*>::iterator j = functionDefinition->get_body()->get_statements().begin();
               while ( j != functionDefinition->get_body()->get_statements().end() )
                  {
                 // Look for the SgImplicitStatement, then test if it was specificed using "none"
                    SgImplicitStatement* implicitStatement = isSgImplicitStatement(*j);
                    if (implicitStatement != NULL && implicitStatement->get_implicit_none() == true)
                       isImplicitNoneScope = true;

                    j++;
                  }
             }

          i++;
        }

  // DQ (5/21/2008): for the case of a module we will not find a SgFunctionDefinition (see test2008_35.f90)
  // ROSE_ASSERT(foundFunctionDefinition == true);

     return isImplicitNoneScope;
   }


SgVariableDeclaration* 
buildVariableDeclaration (Token_t * label, bool buildingImplicitVariable )
   {
#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of buildVariableDeclaration()");
#endif

  // DQ (5/17/2008): If we are building a variable using implicit type rules, 
  // then make sure this is not in a scope (or in a nested scope) that has 
  // been marked as "implicit none".
     if (buildingImplicitVariable == true)
        {
          bool isAnImplicitNoneScope = isImplicitNoneScope();

          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
               printf ("In buildVariableDeclaration(): isAnImplicitNoneScope = %s \n",(isAnImplicitNoneScope == true) ? "true" : "false");
#if 0
          if (isAnImplicitNoneScope == true)
             {
               outputState("Warning: isAnImplicitNoneScope == true buildVariableDeclaration()");
             }
#endif
       // First we build it as a variable then we build it as a function (a few rules later)
       // So although we know at this point that this is going to be a function, we have to
       // build it as a varialbe so that it can be evaluated for converstion from a variable 
       // into a function or array a bit later.
       // ROSE_ASSERT(isAnImplicitNoneScope == false);
        }

  // printf ("buildingImplicitVariable = %s \n",buildingImplicitVariable ? "true" : "false");

  // Currently I am skipping initializers
     SgVariableDeclaration* variableDeclaration = new SgVariableDeclaration();

  // Set the position of the definition (not always the same, but most often the same)
     ROSE_ASSERT(variableDeclaration != NULL);

     if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
          printf ("In buildVariableDeclaration(): variableDeclaration = %p being built astNodeStack.size() = %zu \n",variableDeclaration,astNodeStack.size());

  // Now the toke list has all the tokens from the start to the end (except the type, which never was a token, not sure what to do about that).
  // setSourcePosition(variableDeclaration,tokenList);
  // setSourcePosition(variableDeclaration,label);
     setSourcePosition(variableDeclaration);

  // DQ (9/20/2007): Set the parent and definingDeclaration explicitly, the scopes is not defined.
     SgScopeStatement* currentScope = getTopOfScopeStack();
     variableDeclaration->set_parent(currentScope);
     variableDeclaration->set_definingDeclaration(variableDeclaration);

  // Setup the label on the statement if it is available.
     setStatementNumericLabel(variableDeclaration,label);

  // This should be the default, so I am not sure why we have to set it!
     variableDeclaration->set_variableDeclarationContainsBaseTypeDefiningDeclaration(false);

  // Get the first SgInitializedName object and set its members
     ROSE_ASSERT(variableDeclaration->get_variables().empty() == true);
     ROSE_ASSERT(astNodeStack.empty() == false);

  // printf ("astNodeStack.size() = %zu \n",astNodeStack.size());

     SgInitializedName* firstInitializedNameForSourcePosition = NULL;
     SgInitializedName* lastInitializedNameForSourcePosition  = NULL;

     do {
#if 0
       // Output debugging information about saved state (stack) information.
          outputState("In loop over variables in buildVariableDeclaration()");
#endif
       // printf ("At TOP of processing list of variables declared in a single declaration \n");

          SgInitializedName* initializedName = isSgInitializedName(astNodeStack.front());

       // These are used to set the source position of the SgVariableDeclaration
          if (firstInitializedNameForSourcePosition == NULL)
               firstInitializedNameForSourcePosition = initializedName;
          lastInitializedNameForSourcePosition = initializedName;

       // printf ("In buildVariableDeclaration(): Processing initializedName = %p = %s \n",initializedName,initializedName->get_name().str());
       // setSourcePosition(initializedName);

       // initializedName->get_startOfConstruct()->display("buildVariableDeclaration(): initializedName");

          SgName variableName = initializedName->get_name();
          initializedName->set_declptr(variableDeclaration);

          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
               printf ("In buildVariableDeclaration(): initializedName = %p = %s initializer = %p \n",initializedName,initializedName->get_name().str(),initializedName->get_initializer());

       // DQ (11/29/2007): This is an odd API in ROSE, the initializer (already a part of the SgInitializedName, must be provide seperately to the append_variable() member function).
       // variableDeclaration->append_variable(initializedName,initializedName->get_initializer());
          variableDeclaration->prepend_variable(initializedName,initializedName->get_initializer());

          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
             {
               printf ("After variableDeclaration->prepend_variable(): initializedName = %p = %s initializer = %p \n",initializedName,initializedName->get_name().str(),initializedName->get_initializer());
               if (initializedName->get_initializer() != NULL)
                  {
                    SgExpression* initializer = initializedName->get_initializer();
                    printf ("--- initializedName->get_initializer() = %p = %s = %s \n",initializer,initializer->class_name().c_str(),SageInterface::get_name(initializer).c_str());
                  }
             }

       // setSourcePosition(initializedName->get_definition());
          ROSE_ASSERT(initializedName->get_definition()->get_startOfConstruct() != NULL);

          ROSE_ASSERT(astScopeStack.empty() == false);
          ROSE_ASSERT(astScopeStack.front()->get_parent() != NULL);
          SgFunctionDefinition* functionDefinition = isSgFunctionDefinition(astScopeStack.front()->get_parent());

       // DQ (5/15/2008): This is allowed since the variable may have been previous built from a common block declaration.
       // ROSE_ASSERT(initializedName->get_scope() == NULL);

          SgVariableSymbol* variableSymbol = NULL;
          if (functionDefinition != NULL)
             {
               variableSymbol = functionDefinition->lookup_variable_symbol(variableName);
               if (variableSymbol != NULL)
                  {
                 // This variable symbol has already been placed into the function definition's symbol table
                 // Link the SgInitializedName in the variable declaration with it's intry in the function parameter list.
                    initializedName->set_prev_decl_item(variableSymbol->get_declaration());

                 // Set the referenced type in the function parameter to be the same as that in the declaration being processed.
                    variableSymbol->get_declaration()->set_type(initializedName->get_type());

                 // Function parameters are in the scope of the function definition (same for C/C++)
                    initializedName->set_scope(functionDefinition);
                  }
             }

       // If not just set above then this is not a function parameter, but it could have been built in a common block
          if (variableSymbol == NULL)
             {
            // ROSE_ASSERT(getTopOfScopeStack()->lookup_variable_symbol(variableName) == NULL);

            // Check the current scope (function body)
               variableSymbol = getTopOfScopeStack()->lookup_variable_symbol(variableName);

               initializedName->set_scope(astScopeStack.front());

               if (variableSymbol == NULL)
                  {
                    variableSymbol = new SgVariableSymbol(initializedName);
                    //printf ("Debug, built a var symbol %p\n",variableSymbol);
                    astScopeStack.front()->insert_symbol(variableName,variableSymbol);
                    ROSE_ASSERT (initializedName->get_symbol_from_symbol_table () != NULL);
                    //SgSymbolTable * parent = isSgSymbolTable(variableSymbol->get_parent());
                    //ROSE_ASSERT (parent != NULL);
                   // ROSE_ASSERT (parent->find_variable(initializedName->get_name()) == variableSymbol);

                  }
             }

          ROSE_ASSERT(variableSymbol != NULL);
          ROSE_ASSERT(initializedName->get_scope() != NULL);

       // DQ (12/14/2007): Error checking... this should not have been specified as built in global scope!
          if (isSgGlobal(initializedName->get_scope()) != NULL)
             {
               printf ("Error: isSgGlobal(initializedName->get_scope()) != NULL *** initializedName = %p = %s \n",initializedName,initializedName->get_name().str());
             }
          ROSE_ASSERT(isSgGlobal(initializedName->get_scope()) == NULL);

       // DQ (1/25/2011): Failing for test2011_33.f90 (because the variables being added to the function were first 
       // defined as function parameters.  This is I think an issue that the buildInterface tries to fix in the 
       // SageBuilder::buildVariableDeclaration() function.
       // DQ (1/24/2011): I think that this test should pass.
       // ROSE_ASSERT(initializedName->get_symbol_from_symbol_table() != NULL);

#if 0
       // Make sure that the variable does not already exist in this current scope!
          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
               printf ("Looking for symbol for initializedName = %s scope = %p = %s \n",initializedName->get_name().str(),initializedName->get_scope(),initializedName->get_scope()->class_name().c_str());
#endif 
       // Make sure we can find the newly added symbol!
       // ROSE_ASSERT(getTopOfScopeStack()->lookup_variable_symbol(variableName) != NULL);

          setSourcePosition(initializedName);
#if 0
          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
               printf ("In buildVariableDeclaration(): initializedName = %p \n",initializedName);
#endif
          ROSE_ASSERT(astNodeStack.empty() == false);
          astNodeStack.pop_front();

       // We should have at least one element in the variable list.
          ROSE_ASSERT(variableDeclaration->get_variables().empty() == false);

       // printf ("At BOTTOM of processing list of variables declared in a single declaration \n");
        }
     while ( (buildingImplicitVariable == false) && (astNodeStack.empty() == false) );

  // DQ (11/24/2007): Set the default to be undefined (new setting added to support Fortran)
  // Note that undefined access (niether PUBLIC nor PRIVATE) default to PUBLIC if there is not 
  // PRIVATE statement specifically naming the variable or no PRIVATE statement with an empty list.
  // Note that either a PUBLIC or PRIVATE statement with no list changes the default setting!
  // So we default to an undefined setting.
     variableDeclaration->get_declarationModifier().get_accessModifier().setUndefined();

  // DQ (11/18/2007): Save the attributes used and clear the astAttributeSpecStack for this declaration
     while (astAttributeSpecStack.empty() == false)
        {
       // printf ("In buildVariableDeclaration(): Process attribute spec %d ",astAttributeSpecStack.front());
          setDeclarationAttributeSpec(variableDeclaration,astAttributeSpecStack.front());

          if (astAttributeSpecStack.front() == AttrSpec_PUBLIC || astAttributeSpecStack.front() == AttrSpec_PRIVATE)
             {
            // printf ("astNameStack.size() = %zu \n",astNameStack.size());
               if (astNameStack.empty() == false)
                  {
                    string type_attribute_string = astNameStack.front()->text;
                 // printf ("type_attribute_string = %s \n",type_attribute_string.c_str());
                    astNameStack.pop_front();
                  }
             }

          astAttributeSpecStack.pop_front();
        }

  // Set the source position to be more precise than just: isCompilerGenerated = false position = 0:0 filename = NULL_FILE
  // Set the value using the firstInitializedNameForSourcePosition.
     ROSE_ASSERT(variableDeclaration->get_startOfConstruct() != NULL);
     ROSE_ASSERT(firstInitializedNameForSourcePosition->get_startOfConstruct() != NULL);
     ROSE_ASSERT(lastInitializedNameForSourcePosition->get_startOfConstruct() != NULL);
     *(variableDeclaration->get_startOfConstruct()) = *(firstInitializedNameForSourcePosition->get_startOfConstruct());
     *(variableDeclaration->get_endOfConstruct())   = *(lastInitializedNameForSourcePosition->get_startOfConstruct());

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of buildVariableDeclaration()");
#endif

  // DQ (12/1/2007): It is better to not clear the stack here and instead clear it in R501

     return variableDeclaration;
   }


void
initialize_global_scope_if_required()
   {
  // First we have to get the global scope initialized (and pushed onto the stack).

  // printf ("In initialize_global_scope_if_required(): astScopeStack.empty() = %s \n",astScopeStack.empty() ? "true" : "false");
     if (astScopeStack.empty() == true)
        {
          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
               printf ("In initialize_global_scope_if_required(): OpenFortranParser_globalFilePointer = %p \n",OpenFortranParser_globalFilePointer);

          ROSE_ASSERT(OpenFortranParser_globalFilePointer != NULL);
          SgSourceFile* file = OpenFortranParser_globalFilePointer;
          SgGlobal* globalScope = file->get_globalScope();
          ROSE_ASSERT(globalScope != NULL);
          ROSE_ASSERT(globalScope->get_parent() != NULL);

       // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
          globalScope->setCaseInsensitive(true);

       // DQ (8/21/2008): endOfConstruct is not set to be consistant with startOfConstruct.
          ROSE_ASSERT(globalScope->get_endOfConstruct()   != NULL);
          ROSE_ASSERT(globalScope->get_startOfConstruct() != NULL);

       // Scopes should be pushed onto the front of the stack (we define the top of the stack to
       // be the front).  I have used a vector instead of a stack for now, this might change later.
          ROSE_ASSERT(astScopeStack.empty() == true);
          astScopeStack.push_front(globalScope);

       // DQ (10/10/2010): Set the start position of global scope to "1".
          globalScope->get_startOfConstruct()->set_line(1);

       // DQ (10/10/2010): Set this position to the same value so that if we increment 
       // by "1" the start and end will not be the same value.
          globalScope->get_endOfConstruct()->set_line(1);
#if 0
          astScopeStack.front()->get_startOfConstruct()->display("In initialize_global_scope_if_required(): start");
          astScopeStack.front()->get_endOfConstruct  ()->display("In initialize_global_scope_if_required(): end");
#endif
        }

  // Testing the scope stack...
  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     std::list<SgScopeStatement*>::iterator scopeInterator = astScopeStack.begin();
     while (scopeInterator != astScopeStack.end())
        {
          if ((*scopeInterator)->isCaseInsensitive() == false)
             {
               printf ("##### Error (in initialize_global_scope_if_required): the scope handling is set to case sensitive scopeInterator = %p = %s \n",*scopeInterator,(*scopeInterator)->class_name().c_str());
            // (*scopeInterator)->setCaseInsensitive(true);
             }
          ROSE_ASSERT((*scopeInterator)->isCaseInsensitive() == true);
          scopeInterator++;
        }
   }

bool
matchingName ( string x, string y )
   {
  // This function checks a case insensitive match of x against y.
  // This is required because Fortran is case insensitive.

     size_t x_length = x.length();
     size_t y_length = y.length();

     return (x_length == y_length) ? strncasecmp(x.c_str(),y.c_str(),x_length) == 0 : false;
   }

// bool matchAgainstImplicitFunctionList( std::string s )
bool
matchAgainstIntrinsicFunctionList( std::string s )
   {
  // This function should likely break out the subroutines and procedures that would be associated with a "call" statement.

     bool resultValue = false;

  // Need caseinsensitive version: strncasecmp(keyword1->text,"real",4) == 0

  // bool isTypeInqueryFunction      = (s == "associated") || (s == "present")  || (s == "kind");
     bool isTypeInqueryFunction      = matchingName(s,"associated") || matchingName(s,"present")  || matchingName(s,"kind");

     bool isElementalNumericFunction = 
          matchingName(s,"abs") || matchingName(s,"aimag")  || matchingName(s,"aint") || matchingName(s,"anint") || matchingName(s,"ceiling") || 
          matchingName(s,"cmplx") || matchingName(s,"floor") || matchingName(s,"int") || matchingName(s,"nint") || matchingName(s,"real");

     bool isElementalNonConversionFunction = 
          matchingName(s,"conjg") || matchingName(s,"dim") || matchingName(s,"max") || matchingName(s,"min") || matchingName(s,"mod") || matchingName(s,"modulo") || matchingName(s,"sign");

     bool isElementalMathFunction = 
          matchingName(s,"acos") || matchingName(s,"asin") || matchingName(s,"atan") || matchingName(s,"atan2") || matchingName(s,"cos") || 
          matchingName(s,"cosh") || matchingName(s,"exp")  || matchingName(s,"log")  || matchingName(s,"log10") || matchingName(s,"sin") || 
          matchingName(s,"sinh") || matchingName(s,"sqrt") || matchingName(s,"tan")  || matchingName(s,"tanh");

     bool isCharacterConversionFunction = 
          matchingName(s,"achar") || matchingName(s,"char") || matchingName(s,"iachar") || matchingName(s,"ichar");

     bool isLexicalCompareFunction = matchingName(s,"lge") || matchingName(s,"lgt") || matchingName(s,"lle") || matchingName(s,"llt");

     bool isStringHandlingElementalFunction = 
          matchingName(s,"adjustl") || matchingName(s,"adjustr") || matchingName(s,"index") || matchingName(s,"len_trim") || matchingName(s,"scan") || matchingName(s,"verify");

     bool isLogicalConversionFunction = matchingName(s,"logical");

     bool isStringInqueryFunction = matchingName(s,"len");

     bool isStringTransformationFunction = matchingName(s,"repeat") || matchingName(s,"trim");

     bool isNumericInqueryFunction = 
          matchingName(s,"digits") || matchingName(s,"epsilon") || matchingName(s,"huge") || matchingName(s,"maxexponent") || matchingName(s,"minexponent") || matchingName(s,"precision") || matchingName(s,"radix") || matchingName(s,"range") || matchingName(s,"tiny");

     bool isElementalRealFunction = 
          matchingName(s,"exponent") || matchingName(s,"fraction") || matchingName(s,"nearest") || matchingName(s,"rrspacing") || matchingName(s,"scale") || matchingName(s,"set_exponent") || matchingName(s,"spacing");

     bool isKindTransformationFunction = matchingName(s,"selected_int_kind") || matchingName(s,"selected_real_kind");

     bool isBitInqueryFunction = matchingName(s,"bit_size");

     bool isBitElementalFunction = 
          matchingName(s,"btest") || matchingName(s,"iand") || matchingName(s,"ibclr") || matchingName(s,"ibits") || matchingName(s,"ibset") || matchingName(s,"ieor") ||
          matchingName(s,"ior") || matchingName(s,"ishft") || matchingName(s,"ishftc") || matchingName(s,"not");

  // This is a procedure and so should be associated with a call statment and so it is different
     bool isBitElementalSubroutine = matchingName(s,"mvbits");

     bool isTransferFunction = matchingName(s,"transfer");

     bool isVectorMatrixMultipyFunction = matchingName(s,"dot_product") || matchingName(s,"matmul");

  // These can take one argument with an optional "dim" argument, also "maxval", "minval", "product", "sum" can have a 3rd optional mask argument.
     bool isArrayReductionFunction = 
          matchingName(s,"all") || matchingName(s,"any") || matchingName(s,"count") || matchingName(s,"maxval") || matchingName(s,"minval") || matchingName(s,"product") || matchingName(s,"sum");

     bool isArrayInqueryFunction = 
          matchingName(s,"allocated") || matchingName(s,"lbound") || matchingName(s,"shape") || matchingName(s,"size") || matchingName(s,"ubound");

     bool isArrayConstructionAndManipulationFunction = 
          matchingName(s,"merge") || matchingName(s,"pack") || matchingName(s,"unpack") || matchingName(s,"reshape") || matchingName(s,"spread") || matchingName(s,"cshift") || matchingName(s,"eoshift") || matchingName(s,"transpose");

  // These come in two flavors (depending of the use of s "dim" 2nd argument)
     bool isTransformationFunctionForGeometricLocation = 
          matchingName(s,"maxloc") || matchingName(s,"minloc") || matchingName(s,"unpack") || matchingName(s,"reshape") || matchingName(s,"spread") || matchingName(s,"cshift") || matchingName(s,"eoshift") || matchingName(s,"transpose");

     bool isTransformationFunctionForPointerDisassociation = matchingName(s,"null");

  // These are procedures and so are part of a call statement.
     bool isTimeFunction = matchingName(s,"data_and_time") || matchingName(s,"system_clock") || matchingName(s,"cpu_time");

  // These are procedures and so are part of a call statement.
     bool isSupportForRandomNumberFunction = matchingName(s,"random_number") || matchingName(s,"random_seed");

  // DQ (11/25/2007): New Fortran 2003 intrinsic functions
     bool isNewFortran2003Function = matchingName(s,"is_iostat_end") || matchingName(s,"is_iostat_eor") || matchingName(s,"new_line");

  // These are the generic intrinsic function name tests.
     if ( isTypeInqueryFunction || isElementalNumericFunction || isElementalNonConversionFunction || 
          isElementalMathFunction || isCharacterConversionFunction || isLexicalCompareFunction || 
          isStringHandlingElementalFunction || isLogicalConversionFunction || isStringInqueryFunction || 
          isStringTransformationFunction || isNumericInqueryFunction || isElementalRealFunction || 
          isKindTransformationFunction || isBitInqueryFunction || isBitElementalFunction || 
          isBitElementalSubroutine || isTransferFunction || isVectorMatrixMultipyFunction || 
          isArrayReductionFunction || isArrayInqueryFunction || isArrayConstructionAndManipulationFunction || 
          isTransformationFunctionForGeometricLocation || isTransformationFunctionForPointerDisassociation || 
          isTimeFunction || isSupportForRandomNumberFunction || isNewFortran2003Function)
        {
          resultValue = true;
        }

// **************************************
// F77 specific types intrinsic functions
// **************************************

/* Alternative names for abs:
IABS
ABS
DABS
CABS
QABS (sun)
ZABS (sun)
CDABS (sun)
CQABS (sun)
*/
     bool isAlternativeName_abs = matchingName(s,"iabs") || matchingName(s,"dabs") || matchingName(s,"cabs") ||
                                 matchingName(s,"qabs") || matchingName(s,"zabs") || matchingName(s,"cdabs") || matchingName(s,"cqabs");

/* Alternative names for int:
AINT
DINT
QINT (sun)
*/
     bool isAlternativeName_int = matchingName(s,"aint") || matchingName(s,"dint") || matchingName(s,"qint");

/* Alternative names for int (nearest whole number):
ANINT
DNINT
QNINT

// Alternative names for int (nearest integer):
NINT
IDNINT
IQNINT (sun)
*/
     bool isAlternativeName_nint = matchingName(s,"anint") || matchingName(s,"dnint") || matchingName(s,"qnint") || matchingName(s,"idnint") || matchingName(s,"iqnint");

/* Alternative names for mod:
MOD
AMOD
DMOD
QMOD (sun)
*/
     bool isAlternativeName_mod = matchingName(s,"amod") || matchingName(s,"dmod") || matchingName(s,"qmod");

/* Alternative names for sign:
ISIGN
SIGN
DSIGN
QSIGN (sun)
*/
     bool isAlternativeName_sign = matchingName(s,"isign") || matchingName(s,"dsign") || matchingName(s,"qsign");

/* Alternative names for dim:
IDIM
DIM
DDIM
QDIM (sun)
*/
     bool isAlternativeName_dim = matchingName(s,"idim") || matchingName(s,"ddim") || matchingName(s,"qdim");

/* Alternative names for * (product):
DPROD
QPROD (sun)
*/
     bool isAlternativeName_prod = matchingName(s,"dprod") || matchingName(s,"qprod");

/* Alternative names for max:
MAX0
AMAX1
DMAX1
QMAX1 (sun)
AMAX0
MAX1
*/
     bool isAlternativeName_max = matchingName(s,"max0") || matchingName(s,"amax0") || matchingName(s,"max1") || matchingName(s,"amax1") || matchingName(s,"dmax1") || matchingName(s,"qmax1");

/* Alternative names for min:
MIN0
AMIN1
DMIN1
QMIN1 (sun)
AMIN0
MIN1
*/
     bool isAlternativeName_min = matchingName(s,"min0") || matchingName(s,"amin0") || matchingName(s,"min1") || matchingName(s,"amin1") || matchingName(s,"dmin1") || matchingName(s,"qmin1");

/* Alternative names for int (conversion):
INT
IFIX
IDINT
IQINT (sun)
*/
     bool isAlternativeName_conversion_int = matchingName(s,"ifix") || matchingName(s,"idint") || matchingName(s,"iqint");

/* Alternative names for real (conversion):
REAL
FLOAT
SNGL
SNGLQ (sun)
FLOATK
*/
     bool isAlternativeName_conversion_real = matchingName(s,"float") || matchingName(s,"sngl") || matchingName(s,"snglq") || matchingName(s,"floatk");

/* Alternative names for double (conversion):
DBLE
DFLOAT
DFLOATK
DREAL (sun)
DBLEQ (sun)
*/
     bool isAlternativeName_conversion_double = matchingName(s,"dble") || matchingName(s,"dfloat") || matchingName(s,"dfloatk") || matchingName(s,"dreal") || matchingName(s,"dbleq");

/* Alternative names for real*16:
QREAL (sun)
QFLOAT (sun)
QEXT (sun)
QEXTD (sun)
*/
     bool isAlternativeName_conversion_real_16 = matchingName(s,"qreal") || matchingName(s,"qfloat") || matchingName(s,"qext") || matchingName(s,"qextd");

/* Alternative names for cmplx (conversion) (CMPLX is only name used)
CMPLX
DCMPLX (sun)
QCMPLX (sun)
*/
     bool isAlternativeName_conversion_cmplx = matchingName(s,"dcmplx") || matchingName(s,"qcmplx");

/* Alternative names for sin:
SIN
DSIN
QSIN (sun)
CSIN
ZSIN (sun)
CDSIN (sun)
CQSIN (sun)
*/
     bool isAlternativeName_sin = matchingName(s,"dsin") || matchingName(s,"qsin") || matchingName(s,"csin") || matchingName(s,"zsin") || matchingName(s,"cdsin") || matchingName(s,"cqsin");

/* Alternative names for sind (sun):
SIND (sun)
DSIND (sun)
QSIND (sun)
*/
     bool isAlternativeName_sind = matchingName(s,"dsind") || matchingName(s,"qsind");

/* Alternative names for cos:
COS
DCOS
QCOS (sun)
CCOS
ZCOS (sun)
CDCOS (sun)
CQCOS (sun) 
*/
     bool isAlternativeName_cos = matchingName(s,"dcos") || matchingName(s,"qcos") || matchingName(s,"ccos") || matchingName(s,"zcos") || matchingName(s,"cdcos") || matchingName(s,"cqcos");

/* Alternative names for cosd (sun):
COSD (sun)
DCOSD (sun)
QCOSD (sun)
*/
     bool isAlternativeName_cosd = matchingName(s,"dcosd") || matchingName(s,"qcosd");

/* Alternative names for tan:
TAN
DTAN
QTAN (sin) 
*/
     bool isAlternativeName_tan = matchingName(s,"dtan") || matchingName(s,"qtan");

/* Alternative names for tand (sun):
TAND (sun)
DTAND (sun)
QTAND (sun)
*/
     bool isAlternativeName_tand = matchingName(s,"dtand") || matchingName(s,"qtand");

/* Alternative names for asin:
ASIN
DASIN
QASIN (sun)

// Alternative names for asind (sun):
ASIND (sun)
DASIND (sun)
QASIND (sun)
*/
     bool isAlternativeName_asin = matchingName(s,"dasin") || matchingName(s,"qasin") || matchingName(s,"asind") || matchingName(s,"dasind") || matchingName(s,"qasind");

/* Alternative names for acos:
ACOS
DACOS
QACOS (sun)

// Alternative names for acosd (sun):
ACOSD (sun)
DACOSD (sun)
QACOSD (sun)
*/
     bool isAlternativeName_acos = matchingName(s,"dacos") || matchingName(s,"qacos") || matchingName(s,"acosd") || matchingName(s,"dacosd") || matchingName(s,"qacosd");

/* Alternative names for atan:
ATAN
DATAN
QATAN (sun)

// Alternative names for atand (sun):
ATAND (sun)
DATAND (sun)
QATAND (sun)
*/
     bool isAlternativeName_atan = matchingName(s,"datan") || matchingName(s,"qatan") || matchingName(s,"atand") || matchingName(s,"datand") || matchingName(s,"qatand");

/* Alternative names for atan2:
ATAN2
DATAN2
QATAN2 (sun)

// Alternative names for atan2d (sun):
ATAN2D (sun)
DATAN2D (sun)
QATAN2D (sun)
*/
     bool isAlternativeName_atan2 = matchingName(s,"datan2") || matchingName(s,"qatan2") || matchingName(s,"atan2d") || matchingName(s,"datan2d") || matchingName(s,"qatan2d");

/* Alternative names for sinh (sun):
SINH (sun)
DSINH (sun)
QSINH (sun)
*/
     bool isAlternativeName_sinh = matchingName(s,"dsinh") || matchingName(s,"qsinh");

/* Alternative names for cosh (sun):
COSH (sun)
DCOSH (sun)
QCOSH (sun)
*/
     bool isAlternativeName_cosh = matchingName(s,"dcosh") || matchingName(s,"qcosh");

/* Alternative names for tanh (sun):
TANH (sun)
DTANH (sun)
QTANH (sun)
*/
     bool isAlternativeName_tanh = matchingName(s,"dtanh") || matchingName(s,"qtanh");

/* Alternative names for imag:
AIMAG
DIMAG (sun)
QIMAG (sun)
*/
  // AIMAG is part of the F77 standard listed above
     bool isAlternativeName_imag = matchingName(s,"dimag") || matchingName(s,"qimag");

/* Alternative names for conjg:
CONJG
DCONJG (sun)
QCONJG (sun)
*/
  // conjg is part of the F77 standard listed above
     bool isAlternativeName_conjg = matchingName(s,"dconjg") || matchingName(s,"qconjg");

/* Alternative names for sqrt:
SQRT
DSQRT
QSQRT (sun)
CSQRT
ZSQRT (sun)
CDSQRT (sun)
CQSQRT (sun)
*/
     bool isAlternativeName_sqrt = matchingName(s,"dsqrt") || matchingName(s,"qsqrt") || matchingName(s,"csqrt") || matchingName(s,"zsqrt") || matchingName(s,"cdsqrt") || matchingName(s,"cqsqrt");

/* Alternative names for cbrt (sun):
CBRT (sun)
DCBRT (sun)
QCBRT (sun)
CCBRT (sun)
ZCBRT (sun)
CDCBRT (sun)
CQCBRT (sun)
*/
     bool isAlternativeName_cbrt = matchingName(s,"cbrt") || matchingName(s,"dcbrt") || matchingName(s,"qcbrt") || matchingName(s,"ccbrt") || matchingName(s,"zcbrt") || matchingName(s,"cdcbrt") || matchingName(s,"cqcbrt");

/* Alternative names for exp:
EXP
DEXP
QEXP (sun)
CEXP
ZEXP (sun)
CDEXP (sun)
CQEXP (sun) 
*/
     bool isAlternativeName_exp = matchingName(s,"dexp") || matchingName(s,"qexp") || matchingName(s,"cexp") || matchingName(s,"zexp") || matchingName(s,"cdexp") || matchingName(s,"cqexp");

/* Alternative names for log:
ALOG
DLOG
QLOG (sun)
CLOG
ZLOG (sun)
CDLOG (sun)
CQLOG (sun)
*/

     bool isAlternativeName_log = matchingName(s,"alog") || matchingName(s,"dlog") || matchingName(s,"qlog") || matchingName(s,"clog") || matchingName(s,"zlog") || matchingName(s,"cdlog") || matchingName(s,"cqlog");

/* Alternative names for log10:
ALOG10
DLOG10
QLOG10 (sun)
*/
     bool isAlternativeName_log10 = matchingName(s,"alog10") || matchingName(s,"dlog10") || matchingName(s,"qlog10");

/* Alternative names for erf:
ERF (sun)
DERF (sun)
*/
     bool isAlternativeName_erf = matchingName(s,"erf") || matchingName(s,"derf");

/* Alternative names for erfc:
ERFC (sun)
DERFC (sun)
*/
     bool isAlternativeName_erfc = matchingName(s,"erfc") || matchingName(s,"derfc");

/* Additional intrinsic environmental functions (non-standard):
EPBASE
EPPREC
EPEMIN
EPEMAX
EPTINY
EPHUGE
EPMRSP
*/

     bool environment_functions = matchingName(s,"epbase") || matchingName(s,"epprec") || matchingName(s,"epemin") || matchingName(s,"epemax") || matchingName(s,"eptiny") || matchingName(s,"ephuge") || matchingName(s,"epmrsp");

/* Memory functions
LOC
MALLOC
MALLOC64
FREE
SIZEOF
*/

     bool memory_functions = matchingName(s,"loc") || matchingName(s,"malloc") || matchingName(s,"malloc64") || matchingName(s,"free") || matchingName(s,"sizeof");

/* Bit functions:

// As defined above:
//   bool isBitElementalFunction = 
//        matchingName(s,"btest") || matchingName(s,"iand") || matchingName(s,"ibclr") || matchingName(s,"ibits") || matchingName(s,"ibset") || matchingName(s,"ieor") ||
//        matchingName(s,"ior") || matchingName(s,"ishft") || matchingName(s,"ishftc") || matchingName(s,"not");

AND
OR
XOR
LSHFT
RSHFT
LRSHFT
*/

     bool additional_bit_functions = matchingName(s,"and") || matchingName(s,"or") || matchingName(s,"xor") || matchingName(s,"lshft") || matchingName(s,"rshft") || matchingName(s,"lrshft");


  // DQ (12/14/2007): These are the non-generic intrinsic function name tests.
     if ( resultValue || isAlternativeName_abs || isAlternativeName_int || isAlternativeName_nint ||
          isAlternativeName_mod || isAlternativeName_sign || isAlternativeName_dim || isAlternativeName_prod ||
          isAlternativeName_max || isAlternativeName_min || isAlternativeName_conversion_int ||
          isAlternativeName_conversion_real || isAlternativeName_conversion_double || 
          isAlternativeName_conversion_real_16 || isAlternativeName_conversion_cmplx ||
          isAlternativeName_sin || isAlternativeName_sind || isAlternativeName_cos || isAlternativeName_cosd ||
          isAlternativeName_tan || isAlternativeName_tand || isAlternativeName_asin || isAlternativeName_acos ||
          isAlternativeName_atan || isAlternativeName_atan2 || isAlternativeName_sinh ||  isAlternativeName_cosh ||
          isAlternativeName_tanh || isAlternativeName_imag || isAlternativeName_conjg || isAlternativeName_sqrt ||
          isAlternativeName_cbrt || isAlternativeName_exp || isAlternativeName_log || isAlternativeName_log10 ||
          isAlternativeName_erf || isAlternativeName_erfc || environment_functions || memory_functions ||
          additional_bit_functions)
        {
          resultValue = true;
        }

     return resultValue;
   }

bool
isIntrinsicFunctionReturningNonmatchingType( string s)
   {
  // Later we can figure out exactly which intrinsic function return type different from their
  // input types.  Examples include: sign, modulo (I think).
     return false;
   }


SgType*
generateIntrinsicFunctionReturnType( string s , SgExprListExp* argumentList )
   {
  // Maybe this function should break out the subroutines and procedures that 
  // would be associated with a "call" statement.

  // Intrinsic function return types depend on there arguments and also on the
  // specific intrinsic function.  

     SgType* returnType = NULL;

     bool isIntrinsicFunction = matchAgainstIntrinsicFunctionList(s);
     ROSE_ASSERT(isIntrinsicFunction == true);

     if (argumentList != NULL)
        {
       // Use the type of the arguments in the argumentList to figure out what type to return.
          if (isIntrinsicFunctionReturningNonmatchingType(s) == true)
             {
            // This may have to be handled on a case by case basis.

            // For now, lets just use the implicit type rules, I will fix this later.
               returnType = generateImplicitType(s);
             }
            else
             {
            // If we have an expression list, then I assum it is non-empty, but check to make sure.
               ROSE_ASSERT(argumentList->get_expressions().empty() == false);

            // As I recall all the argument types are the same and the return type of the 
            // implicit function matches the argument type.
               returnType = argumentList->get_expressions()[0]->get_type();
             }
        }
       else
        {
       // I can't think of anything else to do but compute the type using the implicit type rules.
       // If the user changes the implicit type rules then this would be incorrect, so we have to 
       // have something better eventually.
          returnType = generateImplicitType(s);
        }

  // Use the implicit type rules, however that is not likely good enough since 
  // many intrinsic functions have explicitly predefined types.
  // return generateImplicitType(s);

     ROSE_ASSERT(returnType != NULL);
     return returnType;
   }

SgType* 
generateImplicitType( string name )
   {
  // Implement the default implicit type rules.
  // These will have to be modified to account for user defined implicit type rules later.

  // The DEFAULT implicit typing is based on the first letter of the name Implicit type
  // A to H 	REAL
  // I to N 	INTEGER
  // O to Z 	REAL

     SgType* returnType = NULL;

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of generateImplicitType()");
#endif

  // DQ (12/29/2010): Need to search for implicit statement in outer scope (if it exists)
  //    If it exists, then we need to use that information to associate types.
  //    We might have to take the union of the implicit statements to get the
  //    details correct.  This is out outstanding bug in ROSE and show up 
  //    in the latest work reimplementing the R612 and R613 rules.

     ROSE_ASSERT(tolower(name[0]) >= 'a');
     ROSE_ASSERT(tolower(name[0]) <= 'z');

  // printf ("***** In generateImplicitType(): name = %s name[0] = %c tolower(name[0]) = %c \n",name.c_str(),name[0],tolower(name[0]));

     if (tolower(name[0]) < 'i')
        {
          returnType = SgTypeFloat::createType();
        }
       else 
        {
          if (tolower(name[0]) < 'o')
             {
               returnType = SgTypeInt::createType();
             }
            else 
             {
               returnType = SgTypeFloat::createType();
             }
        }

  // printf ("***** name = %s generating type = %s \n",name.c_str(),returnType->class_name().c_str());

     ROSE_ASSERT(returnType != NULL);
     return returnType;
   }



SgFunctionType* generateImplicitFunctionType( string functionName)
   {
  // This function generates a function type that is computed from the name of the function 
  // and the types of the function arguments on the stack (astExpressionStack).

     bool isIntrinsicFunction = matchAgainstIntrinsicFunctionList(functionName);
     bool argumentListOnStack = ( (astExpressionStack.empty() == false) && (isSgExprListExp(astExpressionStack.front()) != NULL) );

     SgType* returnType = NULL;
     SgExprListExp* exprListExp = NULL;

  // printf ("argumentListOnStack = %s \n",argumentListOnStack ? "true" : "false");
  // printf ("isIntrinsicFunction = %s \n",isIntrinsicFunction ? "true" : "false");
     if (argumentListOnStack == true)
        {
       // If there is an argument list, then if it is a non-empty list then the return type of 
       // the function is determined by the argument type.
          exprListExp = isSgExprListExp(astExpressionStack.front());

          if (isIntrinsicFunction == true)
             {
               returnType = generateIntrinsicFunctionReturnType(functionName,exprListExp);
             }
            else
             {
            // We need to lookup the type by looking for a variable in the symbol table and using that type
               returnType = generateImplicitType(functionName);
             }
        }
       else
        {
       // This may be just a request to generate a function type from a function name.
       // In this case the type can not yet be precisely defined so we use some defaults 
       // for the parts that we don't know (and fixup the information later in post-processing 
       // phases).
          if (isIntrinsicFunction == true)
             {
               returnType = generateIntrinsicFunctionReturnType(functionName,NULL);
             }
            else
             {
            // We need to lookup the type by looking for a variable in the symbol table and using that type
               returnType = generateImplicitType(functionName);
             }

        }

     bool has_ellipses = false;
     SgFunctionType* functionType = new SgFunctionType(returnType,has_ellipses);
     ROSE_ASSERT(functionType != NULL);
     ROSE_ASSERT(functionType->get_argument_list() != NULL);

     if (argumentListOnStack == true)
        {
       // If there was a SgExprListExp on the stack, then fillin the SgFunctionType with the types of the arguments.
          ROSE_ASSERT(exprListExp != NULL);
          SgExpressionPtrList & functionArgumentList = exprListExp->get_expressions();
          for (unsigned int i=0; i < functionArgumentList.size(); i++)
             {
               functionType->append_argument(functionArgumentList[i]->get_type());
             }
        }

     return functionType;
   }


void
buildAttributeSpecificationStatement ( SgAttributeSpecificationStatement::attribute_spec_enum kind, Token_t *label, Token_t *sourcePositionToken )
   {
  // We can't call build_implicit_program_statement_if_required() since it is defined in the c_action_<name> functions.
  // An AttributeSpecification statement can be the first statement in a program
  // (see test2007_147.f, the original Fortran I code from the IBM 704 Fortran Manual).
  // build_implicit_program_statement_if_required();

  // printf ("In buildAttributeSpecificationStatement(): kind = %d label = %s \n",kind,label != NULL ? label->text : "NULL");

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of buildAttributeSpecificationStatement()");
#endif

     SgAttributeSpecificationStatement *attributeSpecificationStatement = new SgAttributeSpecificationStatement();

  // DQ (10/6/2008): It seems that we all of a sudden need this to be set!
     attributeSpecificationStatement->set_definingDeclaration(attributeSpecificationStatement);
     attributeSpecificationStatement->set_firstNondefiningDeclaration(attributeSpecificationStatement);

     ROSE_ASSERT(sourcePositionToken != NULL);
     setSourcePosition(attributeSpecificationStatement,sourcePositionToken);

#if 0
  // DQ (1/29/2009): the save statement will be associated with the wrong file if it is the
  // last statement of an include file.  See tests: test2009_12.f test2009_13.f
     attributeSpecificationStatement->get_file_info()->display("In buildAttributeSpecificationStatement()");
#endif

     attributeSpecificationStatement->set_attribute_kind(kind);

  // printf ("In buildAttributeSpecificationStatement(): astNameStack.size() = %zu \n",astNameStack.size());

  // DQ (9/11/2010): Added support for F2003 "protected" statement.
     if (kind == SgAttributeSpecificationStatement::e_protectedStatement)
        {
       // Note that in Fortran "protected" only means that the variable cannot be modified, but it can be read, so this is different from C++.
          printf ("In buildAttributeSpecificationStatement(): kind == SgAttributeSpecificationStatement::e_protectedStatement \n");

          ROSE_ASSERT(astScopeStack.empty() == false);
          ROSE_ASSERT(astScopeStack.front() != NULL);

          ROSE_ASSERT(astNameStack.empty() == false);
          ROSE_ASSERT(astNameStack.front()->text != NULL);
          SgName name = astNameStack.front()->text;
          printf ("building protected statement for variable name = %s \n",name.str());
          SgVariableSymbol* variableSymbol = astScopeStack.front()->lookup_variable_symbol(name);
          ROSE_ASSERT(variableSymbol != NULL);
          SgInitializedName* initializedName = variableSymbol->get_declaration();
          ROSE_ASSERT(initializedName != NULL);

       // Using new support for fortran "protected" specification of variables.
          initializedName->set_protected_declaration(true);

       // printf ("SgAttributeSpecificationStatement::e_protectedStatement is not yet supported \n");
       // ROSE_ASSERT(false);
        }

     if (kind == SgAttributeSpecificationStatement::e_bindStatement)
        {
       // printf ("In buildAttributeSpecificationStatement(): kind == SgAttributeSpecificationStatement::e_bindStatement \n");

       // Build the SgExprListExp in the attributeSpecificationStatement if it has not already been built
          if (attributeSpecificationStatement->get_bind_list() == NULL)
             {
               SgExprListExp* bindList = new SgExprListExp();
               attributeSpecificationStatement->set_bind_list(bindList);
               bindList->set_parent(attributeSpecificationStatement);
               setSourcePosition(bindList);
             }

       // There should be at least one variable specified!
          ROSE_ASSERT(astExpressionStack.empty() == false);

       // Put into local list so that we can reverse the list entries when inserting them into the attributeSpecificationStatement
       // SgExpressionPtrList localList;
       // while (astExpressionStack.empty() == false)
       // while (astExpressionStack.size() > 1)
          while (astNodeStack.empty() == false)
             {
               SgExpression* bindExpression = isSgExpression(astNodeStack.front());
               ROSE_ASSERT(bindExpression != NULL);

            // printf ("Push the expressions onto the bind_list \n");

            // localList.push_back(bindExpression);
            // attributeSpecificationStatement->get_bind_list()->prepend_expression(astExpressionStack.front());
               attributeSpecificationStatement->get_bind_list()->prepend_expression(bindExpression);

            // astExpressionStack.pop_front();
               astNodeStack.pop_front();
             }

       // This function using the names on the astNameStack to find the linkage and binding_label
          processBindingAttribute( attributeSpecificationStatement );
        }

  // DQ (12/19/2007): This has been changed to use SgPntrArrRefExp expressions instead of names.
     if (kind != SgAttributeSpecificationStatement::e_dimensionStatement)
        {
       // This handling of the astNameStack, does not apply to the dimension statement
          while (astNameStack.empty() == false)
             {
               string name = astNameStack.front()->text;
            // printf ("Push %s onto attributeSpecificationStatement name_list \n",name.c_str());
               attributeSpecificationStatement->get_name_list().push_back(name);

            // printf ("In loop: attributeSpecificationStatement->get_name_list().size() = %zu \n",attributeSpecificationStatement->get_name_list().size());

               astNameStack.pop_front();
             }

       // There should at most be a single intent on the stack (I think)
          ROSE_ASSERT(astIntentSpecStack.size() <= 1);
          while (astIntentSpecStack.empty() == false)
             {
            // DQ (4/5/2010): Debugging missing INTENT keyword in ROSE output of test2009_19.f90.
            // printf ("Calling attributeSpecificationStatement->set_intent(intent) \n");

               int intent = astIntentSpecStack.front();
               attributeSpecificationStatement->set_intent(intent);
               astIntentSpecStack.pop_front();
             }
        }

  // DQ (12/19/2007): The dimension statement now uses this case as well.
  // Handle any entities for both the parameter statement and the allocatable statement
  // if (kind == SgAttributeSpecificationStatement::e_parameterStatement)
     if ( (kind == SgAttributeSpecificationStatement::e_parameterStatement) ||
          (kind == SgAttributeSpecificationStatement::e_externalStatement) ||
          (kind == SgAttributeSpecificationStatement::e_dimensionStatement) ||
          (kind == SgAttributeSpecificationStatement::e_allocatableStatement) )
        {
#if 0
       // Output debugging information about saved state (stack) information.
          outputState("At TOP of buildAttributeSpecificationStatement() for (parameter || external || allocatable || dimension) statement");
#endif
       // The parameter expressions are stored on the stack
          while (astExpressionStack.empty() == false)
             {
               SgExpression* parameterExpression = astExpressionStack.front();

            // Build the SgExprListExp in the attributeSpecificationStatement if it has not already been built
               if (attributeSpecificationStatement->get_parameter_list() == NULL)
                  {
                    SgExprListExp* parameterList = new SgExprListExp();
                    attributeSpecificationStatement->set_parameter_list(parameterList);
                    parameterList->set_parent(attributeSpecificationStatement);
                    setSourcePosition(parameterList);
                  }

               attributeSpecificationStatement->get_parameter_list()->prepend_expression(parameterExpression);

               astExpressionStack.pop_front();
             }
#if 0
          ROSE_ASSERT(astScopeStack.empty() == false);
          astScopeStack.front()->print_symboltable("In buildAttributeSpecificationStatement()");
#endif
#if 0
       // Output debugging information about saved state (stack) information.
          outputState("At BASE of buildAttributeSpecificationStatement() for (parameter || external || allocatable || dimension) statement");
#endif
        }

  // Collect the data groups (data-sets) off of the astNodeStack.
     if (kind == SgAttributeSpecificationStatement::e_dataStatement)
        {
       // Put into local list so that we can reverse the list entries when inserting them into the attributeSpecificationStatement
          SgDataStatementGroupPtrList localList;
          while (astNodeStack.empty() == false)
             {
               SgDataStatementGroup* dataGroup = isSgDataStatementGroup(astNodeStack.front());
               ROSE_ASSERT(dataGroup != NULL);

            // attributeSpecificationStatement->get_data_statement_group_list().push_back(dataGroup);
               localList.push_back(dataGroup);
            // ROSE_ASSERT(attributeSpecificationStatement->get_data_statement_group_list().empty() == false);

               astNodeStack.pop_front();
             }

       // Reverse the list
          for (int i = localList.size()-1; i >= 0; i--)
             {
               attributeSpecificationStatement->get_data_statement_group_list().push_back(localList[i]);
             }
        }

  // printf ("attributeSpecificationStatement->get_name_list().size() = %zu \n",attributeSpecificationStatement->get_name_list().size());

     astScopeStack.front()->append_statement(attributeSpecificationStatement);     

  // Set the numeric label if it exists
     setStatementNumericLabel(attributeSpecificationStatement,label);

  // printf ("In buildAttributeSpecificationStatement(): attributeSpecificationStatement: start = %d end = %d \n",attributeSpecificationStatement->get_startOfConstruct()->get_line(),attributeSpecificationStatement->get_endOfConstruct()->get_line());
   }


// void setDeclarationAttributeSpec ( SgVariableDeclaration* variableDeclaration, int astAttributeSpec )
void
setDeclarationAttributeSpec ( SgDeclarationStatement* variableDeclaration, int astAttributeSpec )
   {
  // printf ("In setDeclarationAttributeSpec(): variableDeclaration = %p astAttributeSpec = %d \n",variableDeclaration,astAttributeSpec);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of setDeclarationAttributeSpec()");
#endif

     switch(astAttributeSpec)
        {
          case AttrSpec_none:
               printf ("Error: Unsupported option for astAttributeSpec = %d \n",astAttributeSpec);
               ROSE_ASSERT(false);
               break;

          case AttrSpec_access:
             {
            // DQ (5/20/2008): This is a redundant specifier, it appears to only be used with AttrSpec_PUBLIC or AttrSpec_PRIVATE
            // It should be filtered out so that will not see this case in this function.
               printf ("Error: Unsupported option for astAttributeSpec = %d \n",astAttributeSpec);
               ROSE_ASSERT(false);
               break;
             }

          case AttrSpec_language_binding:
             {
               variableDeclaration->get_declarationModifier().get_typeModifier().setBind();

#if 0
            // Output debugging information about saved state (stack) information.
               outputState("In setDeclarationAttributeSpec()");
#endif

               processBindingAttribute(variableDeclaration);

            // printf ("Exiting after processing AttrSpec_language_binding = %d \n",(int)AttrSpec_language_binding);
            // ROSE_ASSERT(false);
               break;
             }

       // This maps to a C/C++ modifier setting.
          case AttrSpec_PUBLIC:
               variableDeclaration->get_declarationModifier().get_accessModifier().setPublic();
            // printf ("astNameStack.size() = %zu \n",astNameStack.size());

            // DQ (10/24/2010): Verify that the name on the astNameStack is "PUBLIC" and then pop the token from the stack.

#if 0
            // Output debugging information about saved state (stack) information.
               outputState("In setDeclarationAttributeSpec()");
#endif
            // printf ("Exiting after processing AttrSpec_PUBLIC = %d \n",(int)AttrSpec_PUBLIC);
            // ROSE_ASSERT(false);
               break;

          case AttrSpec_PRIVATE:      variableDeclaration->get_declarationModifier().get_accessModifier().setPrivate();   break;

       // DQ (9/11/2010): Fortran protected attribute is not the same a C++ protected attribute, so we use a different mechanism 
       // that can be associated with individual variables if required (as required with the protected statment is used with 
       // subsets of variables that might appear in a variable declaration).
       // case AttrSpec_PROTECTED:    variableDeclaration->get_declarationModifier().get_accessModifier().setProtected(); break;
          case AttrSpec_PROTECTED:
             {
            // variableDeclaration->get_declarationModifier().get_accessModifier().setProtected(); break;
            // Using new support for fortran "protected" specification of variables.
               SgVariableDeclaration* local_variableDeclaration = isSgVariableDeclaration(variableDeclaration);
            // printf ("Setting all the variables in a variable declaration as protected \n");
               SgInitializedNamePtrList::iterator i = local_variableDeclaration->get_variables().begin();
               while (i != local_variableDeclaration->get_variables().end())
                  {
                    (*i)->set_protected_declaration(true);
                    i++;
                  }
#if 0
               printf ("Exiting after setting all the variables in a variable declaration as protected \n");
               ROSE_ASSERT(false);
#endif
               break;
             }

       // These represent special Fortran specific support in ROSE.
          case AttrSpec_ALLOCATABLE:  variableDeclaration->get_declarationModifier().get_typeModifier().setAllocatable();  break;
          case AttrSpec_ASYNCHRONOUS: variableDeclaration->get_declarationModifier().get_typeModifier().setAsynchronous(); break;
          case AttrSpec_DIMENSION:    variableDeclaration->get_declarationModifier().get_typeModifier().setDimension();    break;
          case AttrSpec_INTENT:
             {
               ROSE_ASSERT(astIntentSpecStack.empty() == false);
               switch(astIntentSpecStack.front())
                  {
                    case IntentSpec_IN:    variableDeclaration->get_declarationModifier().get_typeModifier().setIntent_in();    break;
                    case IntentSpec_OUT:   variableDeclaration->get_declarationModifier().get_typeModifier().setIntent_out();   break;
                    case IntentSpec_INOUT: variableDeclaration->get_declarationModifier().get_typeModifier().setIntent_inout(); break;
                    default:
                       {
                         printf ("Error: default reached in switch(astIntentSpecStack.front()) astIntentSpecStack.front() = %d \n",astIntentSpecStack.front());
                         ROSE_ASSERT(false);
                       }
                  }
               astIntentSpecStack.pop_front();
               break;
             }

          case AttrSpec_INTRINSIC:    variableDeclaration->get_declarationModifier().get_typeModifier().setIntrinsic();    break;
          case AttrSpec_BINDC:        variableDeclaration->get_declarationModifier().get_typeModifier().setBind();         break;
          case AttrSpec_OPTIONAL:     variableDeclaration->get_declarationModifier().get_typeModifier().setOptional();     break;
          case AttrSpec_SAVE:         variableDeclaration->get_declarationModifier().get_typeModifier().setSave();         break;
          case AttrSpec_TARGET:       variableDeclaration->get_declarationModifier().get_typeModifier().setTarget();       break;
          case AttrSpec_VALUE:        variableDeclaration->get_declarationModifier().get_typeModifier().setValue();        break;

       // DQ (8/28/2010): Added support required for type-attributes-spec values (TypeAttrSpec_bind does the same as AttrSpec_BINDC).
          case TypeAttrSpec_extends:  variableDeclaration->get_declarationModifier().get_typeModifier().setExtends();      break;
          case TypeAttrSpec_abstract: variableDeclaration->get_declarationModifier().get_typeModifier().setAbstract();     break;
          case TypeAttrSpec_bind:     variableDeclaration->get_declarationModifier().get_typeModifier().setBind();         break;

       // This maps to C/C++ modifier settings.
          case AttrSpec_EXTERNAL:     variableDeclaration->get_declarationModifier().get_storageModifier().setExtern();    break;
          case AttrSpec_VOLATILE:     variableDeclaration->get_declarationModifier().get_typeModifier().get_constVolatileModifier().setVolatile(); break;

          case AttrSpec_PARAMETER:
            // printf ("Error: PARAMETER is an attribute that implies constant value ('const' in C/C++) \n");
               variableDeclaration->get_declarationModifier().get_typeModifier().get_constVolatileModifier().setConst();

            // Output debugging information about saved state (stack) information.
               outputState("In setDeclarationAttributeSpec(): case AttrSpec_PARAMETER");

               if (astBaseTypeStack.empty() == false)
                  {
                    if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                         printf ("Processing AttrSpec_PARAMETER case, astBaseTypeStack not empty: astBaseTypeStack.front() = %p = %s = %s (CLEARING astBaseTypeStack) \n",
                              astBaseTypeStack.front(),astBaseTypeStack.front()->class_name().c_str(),SageInterface::get_name(astBaseTypeStack.front()).c_str());

                    SgModifierType* modifierType = isSgModifierType(astBaseTypeStack.front());
                 // ROSE_ASSERT(modifierType != NULL);
                    if (modifierType != NULL)
                       {
                         modifierType->get_typeModifier().get_constVolatileModifier().setConst();
                       }
                      else
                       {
                      // printf ("This was not a modifier Type \n");
                       }

                 // DQ (4/7/2010): This might be the perfect place to pop the stack, and avoid the warning later!
                 // astBaseTypeStack.pop();
                  }

               break;

          case AttrSpec_POINTER:
            // printf ("Error: POINTER is an attribute specifier that effects the associated type (no flag is provided) \n");
               break;

          case AttrSpec_PASS:
          case AttrSpec_NOPASS:
          case AttrSpec_NON_OVERRIDABLE:
          case AttrSpec_DEFERRED:
            // printf ("Error: Are these F08 attribute specs? astAttributeSpec = %d \n",astAttributeSpec);
               break;

          case ComponentAttrSpec_pointer:
            // DQ (8/29/2010): This should be enabled so that we can at least see that it is not implemented.
            // FMZ 6/15/2009 : this should be ok
               if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                    printf ("Error: POINTER (ComponentAttrSpec_pointer) is an attribute specifier that effects the associated type (no flag is provided) \n");
            // ROSE_ASSERT(false);
               break;

#if ROSE_OFP_MINOR_VERSION_NUMBER == 7
       // DQ (4/5/2010): These have been removed from OFP 0.8.0
          case ComponentAttrSpec_dimension_paren:
#if 0
            // DQ (8/29/2010): This should be enabled so that we can at least see that it is not implemented.
            // FMZ 6/15/2009 : this should be ok
               printf ("Error: ComponentAttrSpec_dimension_paren used as an attribute specifier (unclear how to process this) \n");
               ROSE_ASSERT(false);
#endif
            // Laksono 2009.10.16: This is a Fortran legal syntax to have an array inside a type!
               variableDeclaration->get_declarationModifier().get_typeModifier().setDimension();
            // printf ("Error: ComponentAttrSpec_dimension_paren used as an attribute specifier (unclear how to process this) \n");
            // ROSE_ASSERT(false);
               break;

          case ComponentAttrSpec_dimension_bracket:
               printf ("Error: ComponentAttrSpec_dimension_bracket used as an attribute specifier (unclear how to process this) \n");
               ROSE_ASSERT(false);
               break;
#endif

          case ComponentAttrSpec_allocatable:
               printf ("Error: ComponentAttrSpec_allocatable used as an attribute specifier (unclear how to process this) \n");
               ROSE_ASSERT(false);
               break;

          case ComponentAttrSpec_access_spec:
               printf ("Error: ComponentAttrSpec_access_spec used as an attribute specifier (unclear how to process this) \n");
            // ROSE_ASSERT(false);
               break;

          case ComponentAttrSpec_kind:
               printf ("Error: ComponentAttrSpec_kind used as an attribute specifier (unclear how to process this) \n");
            // ROSE_ASSERT(false);
               break;

          case ComponentAttrSpec_len:
#if 1
            // DQ (8/28/2010): Uncommented this out (problems here are likely due to another bug where the ComponentAttrSpec_len is not set properly.
            // FMZ 6/15/2009 : this should be ok
               printf ("Error: ComponentAttrSpec_len used as an attribute specifier (unclear how to process this) \n");
               ROSE_ASSERT(false);
#endif
               break;

       // DQ (8/29/2010): Added support for new enum values
          case ComponentAttrSpec_codimension:
               printf ("Error: ComponentAttrSpec_codimension used as an attribute specifier (unclear how to process this) \n");
               ROSE_ASSERT(false);
               break;

       // DQ (8/29/2010): Added support for new enum values
          case ComponentAttrSpec_contiguous:
               printf ("Error: ComponentAttrSpec_contiguous used as an attribute specifier (unclear how to process this) \n");
               ROSE_ASSERT(false);
               break;

       // DQ (8/29/2010): Added support for new enum values
          case ComponentAttrSpec_dimension:
               if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                    printf ("Error: ComponentAttrSpec_dimension used as an attribute specifier (unclear how to process this) \n");
            // ROSE_ASSERT(false);
               variableDeclaration->get_declarationModifier().get_typeModifier().setDimension();
               break;

          default:
             {
               printf ("Error: default reached astAttributeSpec = %d \n",astAttributeSpec);
               ROSE_ASSERT(false);
             }
        }
   }

void
processBindingAttribute( SgDeclarationStatement* declaration)
   {
  // This function sets the binding details for functions, variables, and type declarations.

     declaration->get_declarationModifier().setBind();

  // Output debugging information about saved state (stack) information.
  // outputState("Process binding spec in R1232 c_action_subroutine_stmt()");

     string targetLanguage = astNameStack.front()->text;
  // printf ("targetLanguage = %s \n",targetLanguage.c_str());
     astNameStack.pop_front();

  // I think it is always "C", we use the linkage string in ROSE to hold this information
  // (used for C/C++ and now Fortran).
  // declaration->set_bind_language(targetLanguage);
     declaration->set_linkage(targetLanguage);

  // Note that the use of "NAME="c_language_name" is optional.
  // ROSE_ASSERT(astNameStack.empty() == false);

  // printf ("In processBindingAttribute(): astNameStack.empty()       = %s \n",astNameStack.empty() ? "true" : "false");
  // printf ("In processBindingAttribute(): astExpressionStack.empty() = %s \n",astExpressionStack.empty() ? "true" : "false");

     if (astNameStack.empty() == false && astExpressionStack.empty() == false)
        {
       // There is a "NAME" = string_literal to be processed.
          string optionString = astNameStack.front()->text;
       // printf ("optionString = %s \n",optionString.c_str());
          astNameStack.pop_front();

       // SgExpression* stringLiteralExp = isSgStringVal(astExpressionStack.front());
          SgStringVal* stringLiteralExp = isSgStringVal(astExpressionStack.front());
          astExpressionStack.pop_front();
          ROSE_ASSERT(stringLiteralExp != NULL);

          string bindingLabel = stringLiteralExp->get_value();

       // We don't need the string literal IR node that we just got from the stack (we just want the string)
          delete stringLiteralExp;
          stringLiteralExp = NULL;

       // printf ("Binding label = %s \n",bindingLabel.c_str());
          declaration->set_binding_label(bindingLabel);
        }
   }

void
convertVariableSymbolToFunctionCallExp( SgVariableSymbol* variableSymbol, Token_t* nameToken )
   {
     SgName name = variableSymbol->get_name();

     if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
          printf ("Converting a SgVarRefExp to a SgFunctionCallExp \n");

     SgExprListExp* expressionList = new SgExprListExp();
     ROSE_ASSERT(expressionList != NULL);
     setSourcePosition(expressionList);
     astExpressionStack.push_front(expressionList);

  // Sg_File_Info* filePosition = varRefExp->get_file_info();
  // ROSE_ASSERT(filePosition != NULL);
  // filePosition->display("In c_action_procedure_designator()");

  // Token_t* nameToken = create_token(filePosition->get_line(),filePosition->get_col(),0,name.str());

     ROSE_ASSERT(nameToken != NULL);

  // printf ("In convertVariableSymbolToFunctionCallExp(): This function call to generateFunctionCall() is ignoring its new SgFunctionSymbol return type. \n");

     generateFunctionCall(nameToken);
   }

void
convertExpressionOnStackToFunctionCallExp()
   {
  // This function is only called by R1219 c_action_procedure_designator()

  // This function takes a varRefExp on the astExpressionStack and converts it to a function call of the same name.
  // This addresses to need to be able to change the type of construct represented initially with little information 
  // and so for which we assumed it was a variable and only later in the sequence of parsing actions discovered that 
  // it was a function.  In general everything is a function unless there is a hint that it is an array.

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of convertExpressionOnStackToFunctionCallExp()");
#endif

     ROSE_ASSERT(astExpressionStack.empty() == false);
     SgExpression* expression = astExpressionStack.front();
  // printf ("In convertExpressionOnStackToFunctionCallExp(): expression = %p = %s \n",expression,expression->class_name().c_str());
     SgVarRefExp* varRefExp = isSgVarRefExp(expression);
     if (varRefExp != NULL)
        {
       // Take the varRefExp off the expression stack.
          astExpressionStack.pop_front();
          SgVariableSymbol* variableSymbol = varRefExp->get_symbol();

          SgName name = variableSymbol->get_name();
          Sg_File_Info* filePosition = varRefExp->get_file_info();
          ROSE_ASSERT(filePosition != NULL);
          Token_t* nameToken = create_token(filePosition->get_line(),filePosition->get_col(),0,name.str());

          convertVariableSymbolToFunctionCallExp(variableSymbol,nameToken);
        }
       else
        {
       // DQ (1/20/2008): Added support to fix test2007_164.f (calling functions without the "()" syntax)
          SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(expression);
          if (functionRefExp != NULL)
             {
               SgExprListExp* functionArguments = new SgExprListExp();
               setSourcePosition(functionArguments);

            // Take the SgFunctionRefExp off the stack
               astExpressionStack.pop_front();

               SgFunctionCallExp* functionCallExp  = new SgFunctionCallExp(functionRefExp,functionArguments,NULL);
               setSourcePosition(functionCallExp);

            // Now push the function call to the implicit function onto the astExpressionStack
               ROSE_ASSERT(functionCallExp != NULL);
               astExpressionStack.push_front(functionCallExp);
             }
            else
             {
            // printf ("In convertExpressionOnStackToFunctionCallExp(): expression = %p = %s \n",expression,expression->class_name().c_str());
             }
        }
   }


// Note: it might make more sense for "convert" function to not have return types and leave their result on the stack.
SgArrayType*
convertTypeOnStackToArrayType( int count )
   {
  // This function uses the entry on the top of the type stach and the expressions on the astExpressionStack
  // and replaced the top o the typeStack with a SgArrayType.  This conversion of base type to array type is
  // required because we often find out later after having declarated a variable that it is an array (either
  // in the process of building the variable declaration or because an "allocatable statement" is seen after
  // the variable declaration).

     if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
          printf ("In convertTypeOnStackToArrayType(count = %d) \n",count);

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of convertTypeOnStackToArrayType()");
#endif

  // Use the type on the astTypeStack and build an array from it (at least if the type == 700)
  // SgType* baseType = getTopOfTypeStack();
     ROSE_ASSERT(astBaseTypeStack.empty() == false);

  // DQ (1/24/2011): If there is an entry on the astTypeStack then we are supposed to turn it into an array.
  // Most of the time astTypeStack.empty() == true, so this hanppend mostly for multipart 
  // specifications such as "character A(2)*7" array of strings of length 7.
  // SgType* baseType = astBaseTypeStack.front();
     SgType* baseType = NULL;
     if (astTypeStack.empty() == false)
        {
          printf ("Unusual case of declaration such as: character A(2)*7 \n");
          baseType = astTypeStack.front();
        }
       else
        {
       // This is the typical case...
          baseType = astBaseTypeStack.front();
        }

     ROSE_ASSERT(baseType != NULL);

  // Leave this on the stack so that other arrays or references to it can use it as a 
  // base type (e.g. for a differently dimensioned array).
  // astTypeStack.pop_front();

  // At this point we have not seen the "dimension"
#if 0
  // Debugging code
     if (astAttributeSpecStack.empty() == false)
        {
          std::list<int>::iterator i = astAttributeSpecStack.begin();
          while (i != astAttributeSpecStack.end())
             {
               printf ("astAttributeSpecStack i = %d \n",*i);
               i++;
             }
        }
#endif

  // depending on the syntax type used to declare the array).
  // ROSE_ASSERT (astAttributeSpecStack.empty() == true);

  // Generate a NULL expression that we can fixup later when we see a dimension statement or when we see more of the declaration.
  // Actually the p_index is used for C/C++ and the the p_dim_info is used for Fortran, we need to make this more uniform.
     SgExpression* sizeExpression = new SgNullExpression();
     setSourcePosition(sizeExpression);

#if 0
     printf ("I think we need to call the SgArrayType::createType() interface instead of new SgArrayType()\n");
#endif

  // Build the array type
     SgArrayType* arrayType = new SgArrayType(baseType,sizeExpression);
     ROSE_ASSERT(arrayType != NULL);

  // Need to set the parent of the sizeExpression since we don't traverse the types to set such things in the AST postprocessing.
     sizeExpression->set_parent(arrayType);

  // DQ (1/17/2011): We now use R510 to gather the indexing and build the multidimensional array dimension.
     ROSE_ASSERT(arrayType->get_dim_info() == NULL);

     ROSE_ASSERT(astExpressionStack.empty() == false);
     SgExprListExp* expresssionList = isSgExprListExp(astExpressionStack.front());
     ROSE_ASSERT(expresssionList != NULL);
     astExpressionStack.pop_front();
     arrayType->set_dim_info(expresssionList);
  // setSourcePosition(expresssionList);
     expresssionList->set_parent(arrayType);

  // Mark the rank as count, even though we have not seen the dimension expressions yet.
     arrayType->set_rank(expresssionList->get_expressions().size());
  // printf ("arrayType built with rank = %d (set from the input count parameter) \n",arrayType->get_rank());

  // Set the scope so that we can cal unparseToString()
  // sizeExpression->set_scope(getTopOfScopeStack());

  // printf ("sizeExpression->unparseToString() = %s \n",sizeExpression->unparseToString().c_str());

  // This is an error since we never know when it is the base_type and when it is an original type in a variable declaration.
  // Remove the base_type from the astTypeStack, before we push the new arrayType
  // astTypeStack.pop_front();

#if 0
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of convertTypeOnStackToArrayType()");
#endif

  // printf ("Exiting at base of convertTypeOnStackToArrayType \n");
  // ROSE_ASSERT(false);

  // Put the array type onto the type stack
  // astTypeStack.push_front(arrayType);
     return arrayType;
   }


void
processFunctionPrefix( SgFunctionDeclaration* functionDeclaration )
   {
     while (astFunctionAttributeStack.empty() == false)
        {
       // string prefixString = astNameStack.front()->text;
          string prefixString = astFunctionAttributeStack.front()->text;

       // printf ("prefixString = %s \n",prefixString.c_str());
       // astNameStack.pop_front();
          astFunctionAttributeStack.pop_front();

       // This needs to be a case insensitive test for equality (use C since C++ does not have a simple mechanism for this).
          if ( strncasecmp(prefixString.c_str(),"pure",4) == 0 )
             {
            // printf ("Marking this function as PURE \n");
               functionDeclaration->get_functionModifier().setPure();
             }
            else if ( strncasecmp(prefixString.c_str(),"elemental",9) == 0 )
             {
            // printf ("Marking this function as ELEMENTAL \n");
               functionDeclaration->get_functionModifier().setElemental();
             }
            else if ( strncasecmp(prefixString.c_str(),"recursive",9) == 0 )
             {
            // printf ("Marking this function as RECURSIVE \n");
               functionDeclaration->get_functionModifier().setRecursive();
             }
        }
   }



SgFunctionRefExp*
generateFunctionRefExp( Token_t* nameToken )
   {
  // This function will generate a SgFunctionRefExp using just the function name and the current scope.
  // If the function is found in a symbol table (iterating from the current scope to the global scope)
  // then the SgFunctionRefExp is built using the found SgFunctionSymbol.  
  // If no SgFunctionSymbol is found in the symbol tables from iterating through all the scopes, then 
  // a new SgProcedureHeaderStatement (function declaration) is built and an associated SgFunctionSymbol 
  // is inserted into the global scope.  

  // Note that the global scope may not be correct if the function will be seen later in 
  // a different scope (e.g. the current module's scope).  So a fixup is required and is 
  // called from R1106 c_action_end_module_stmt().

     ROSE_ASSERT(nameToken != NULL);
     std::string name = nameToken->text;

     if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
          printf ("Generating a SgFunctionRefExp for name = %s \n",name.c_str());

     SgFunctionType* functionType = generateImplicitFunctionType(name);
     ROSE_ASSERT(functionType != NULL);

  // The next element on the stack is the expression list of function arguments
  // ROSE_ASSERT(astExpressionStack.empty() == false);
  // SgExprListExp* functionArguments = isSgExprListExp(astExpressionStack.front());
  // astExpressionStack.pop_front();

     SgFunctionRefExp* functionRefExp = NULL;
     SgName functionName              = nameToken->text;
     SgFunctionSymbol* functionSymbol = trace_back_through_parent_scopes_lookup_function_symbol(functionName,astScopeStack.front());

#if 0
     printf ("In generateFunctionRefExp(): called trace_back_through_parent_scopes_lookup_function_symbol() -- functionSymbol = %p \n",functionSymbol);
#endif

     if (functionSymbol != NULL)
        {
       // Found the function symbol, so build the function ref expression using the existing symbol...
       // printf ("Found the function symbol, so build the function ref expression using the existing symbol... \n");

          functionRefExp = new SgFunctionRefExp(functionSymbol,NULL);
          setSourcePosition(functionRefExp);
        }
       else
        {
       // If the function has not been seen yet, it does not mean that it is an array.  It could be a function 
       // to be declared later or an intrinsic function.  But the point is that it is a function!

       // For implicit function we build non-defining declarations
          SgFunctionDefinition* functionDefinition = NULL;
       // SgFunctionDeclaration* functionDeclaration = new SgFunctionDeclaration(name,functionType,functionDefinition);
       // SgProgramHeaderStatement* functionDeclaration = new SgProgramHeaderStatement(name,functionType,functionDefinition);
          SgProcedureHeaderStatement* functionDeclaration = new SgProcedureHeaderStatement(name,functionType,functionDefinition);

          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
               printf ("Generated a SgProcedureHeaderStatement for name = %s functionDeclaration = %p \n",name.c_str(),functionDeclaration);

       // This is the first function declaration for this function name so we have to set it as the firstNondefiningDeclaration.
       // Note that the first nondefining declaration is what is always used for building symbols.
          functionDeclaration->set_firstNondefiningDeclaration(functionDeclaration);
          functionDeclaration->set_definingDeclaration(NULL);

       // Set the parent to the global scope, mostly just to have it be non-NULL (checked by internal error checking).
          ROSE_ASSERT(astScopeStack.empty() == false);

       // We really want the nearest module scope or global scope is there is no module scope.
       // See test2010_140.f90
       // SgScopeStatement* currentScope = astScopeStack.front();
          SgScopeStatement* currentScope = isSgClassDefinition(astScopeStack.front());
          if (currentScope == NULL)
             {
               currentScope = TransformationSupport::getClassDefinition(astScopeStack.front());
               if (currentScope == NULL)
                  {
                    currentScope = TransformationSupport::getGlobalScope(astScopeStack.front());
                    ROSE_ASSERT(currentScope != NULL);
                  }
             }
          ROSE_ASSERT(currentScope != NULL);

          functionDeclaration->set_parent(currentScope);
          functionDeclaration->set_scope(currentScope);

          setSourcePosition(functionDeclaration,nameToken);
       // We should not have to set this explicitly!
          setSourcePosition(functionDeclaration->get_parameterList(),nameToken);

       // Now build the function call and use the arguments from the ExprList on the top of the astExpressionStack!
          SgFunctionSymbol* functionSymbol = new SgFunctionSymbol(functionDeclaration);

       // Insert the function into the global scope so that we can find it later.
          currentScope->insert_symbol(functionName,functionSymbol);

          functionRefExp = new SgFunctionRefExp(functionSymbol,NULL);
          setSourcePosition(functionRefExp);
        }

  // Now push the function call to the implicit function onto the astExpressionStack
     ROSE_ASSERT(functionRefExp != NULL);
  // astExpressionStack.push_front(functionRefExp);

     return functionRefExp;
   }


// DQ (12/29/2010): Modified to return the associated SgFunctionSymbol so
// that we can support greater uniformity in handling of R612 and R613.
// void generateFunctionCall( Token_t* nameToken )
SgFunctionSymbol*
generateFunctionCall( Token_t* nameToken )
   {
     ROSE_ASSERT(nameToken != NULL);

     if ( ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL ) || (astExpressionStack.empty() != false) )
          printf ("Inside of generateFunctionCall(): nameToken = %s \n",nameToken->text);

  // The next element on the stack is the expression list of function arguments
  // However, test2010_169.f90 demonstrates that an implicit function can be called 
  // without "()", so it should not be an error to not have a SgExprListExp IR node
  // on the stack.

  // ROSE_ASSERT(astExpressionStack.empty() == false);
  // SgExprListExp* functionArguments = isSgExprListExp(astExpressionStack.front());
  // astExpressionStack.pop_front();
     SgExprListExp* functionArguments = NULL;
     if (astExpressionStack.empty() == false)
        {
          functionArguments = isSgExprListExp(astExpressionStack.front());
          astExpressionStack.pop_front();
        }
       else
        {
       // printf ("Special case of function not called with () \n");
          functionArguments = new SgExprListExp();
          setSourcePosition(functionArguments);
        }

  // DQ (12/11/2010): If the name of this function is not found a function of this 
  // name will be added to the current scope (see details in generateFunctionRefExp()).
     SgFunctionRefExp* functionRefExp = generateFunctionRefExp(nameToken);

     SgFunctionCallExp* functionCallExp  = new SgFunctionCallExp(functionRefExp,functionArguments,NULL);
     setSourcePosition(functionCallExp,nameToken);

  // Now push the function call to the implicit function onto the astExpressionStack
     ROSE_ASSERT(functionCallExp != NULL);
     astExpressionStack.push_front(functionCallExp);


  // DQ (5/14/2008): Now cleanup the SgInitializedName, the SgVariableSymbol, and the 
  // SgVariableDeclarationStatement; since these are now invalid now that the name is 
  // interpreted as a function call.  Note that we see R619 a little bit too late in the
  // parsing (after we have interpreted the name as a variable).

  // Look up the name, 
     SgVariableSymbol* variableSymbol = NULL;
     SgFunctionSymbol* functionSymbol = NULL;
     SgClassSymbol*    classSymbol    = NULL;
     
     SgName variableName            = nameToken->text;
     SgScopeStatement* currentScope = astScopeStack.front();

     ROSE_ASSERT(currentScope != NULL);

     trace_back_through_parent_scopes_lookup_variable_symbol_but_do_not_build_variable(variableName,currentScope,variableSymbol,functionSymbol,classSymbol);

  // DQ (12/29/2010): Can we assert this if we just build a SgFunctionRefExp? (fails in case of test2007_158.f90).
  // ROSE_ASSERT(functionSymbol != NULL);
#if 0
     if (functionSymbol == NULL)
        {
       // In this case the function has no declaration but is being called.  So it can have no symbol.
          printf ("Interesting case of functionSymbol == NULL \n");
        }
#endif

     if (variableSymbol != NULL)
        {
          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
               printf ("Found variableSymbol = %p nameToken = %s (erasing all traces of this variable so it can be supported as a function) \n",variableSymbol,nameToken->text);
       // ROSE_ASSERT(false);

          SgSymbolTable* symbolTable = isSgSymbolTable(variableSymbol->get_parent());
          ROSE_ASSERT(symbolTable != NULL);

          symbolTable->remove(variableSymbol);

          SgInitializedName* initializedName = variableSymbol->get_declaration();
          ROSE_ASSERT(initializedName != NULL);

          SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(initializedName->get_parent());
          ROSE_ASSERT(variableDeclaration != NULL);

          SgVariableDefinition* variableDefinition = isSgVariableDefinition(initializedName->get_declptr());
          ROSE_ASSERT(variableDefinition != NULL);

          delete variableDefinition;
          variableDefinition = NULL;

          delete variableDeclaration;
          variableDeclaration = NULL;

          delete initializedName;
          initializedName = NULL;

          delete variableSymbol;
          variableSymbol = NULL;
        }
       else
        {
       // DQ (8/28/2010): Report more diagnostics.
       // printf ("Warning: In generateFunctionCall() there was no SgVariableSymbol found to convert to a SgFunctionSymbol \n");
        }

  // This works the 2nd time because the function id by default defined in global scope and the variable symbol just removed 
  // was in the function's body (where the function was called).
  // printf ("In generateFunctionCall(): second try to isolate the functionSymbol! \n");
     trace_back_through_parent_scopes_lookup_variable_symbol_but_do_not_build_variable(variableName,currentScope,variableSymbol,functionSymbol,classSymbol);

  // DQ (12/29/2010): Can we assert this if we just build a SgFunctionRefExp? (fails in case of test2007_158.f90).
     ROSE_ASSERT(functionSymbol != NULL);

#if 0
     if (functionSymbol == NULL)
        {
       // In this case the function has no declaration but is being called.  So it can have no symbol.
          printf ("Interesting case #2 of functionSymbol == NULL \n");
        }
#endif

     return functionSymbol;
   }


void
buildProcedureSupport(SgProcedureHeaderStatement* procedureDeclaration, bool hasDummyArgList)
   {
  // This does not do everything required to build a function or subroutine, but it does as much as possible
  // (factors out code so that it can be called for R1117, R1224, and R1232.

  // This will be the defining declaration
     ROSE_ASSERT(procedureDeclaration != NULL);

  // FMZTEST
  // cout <<"NAME::" << procedureDeclaration->get_name().str()<<endl;

     procedureDeclaration->set_definingDeclaration(procedureDeclaration);
     procedureDeclaration->set_firstNondefiningDeclaration(NULL);

     ROSE_ASSERT(astScopeStack.empty() == false);
     SgScopeStatement* currentScopeOfFunctionDeclaration = astScopeStack.front();

     ROSE_ASSERT(currentScopeOfFunctionDeclaration != NULL);

  // This should not be a SgFunctionDefinition, but it can be a SgBasicBlock (currently) if it is declared in an Interface.
  // ROSE_ASSERT(isSgBasicBlock(currentScopeOfFunctionDeclaration) == NULL);
     ROSE_ASSERT(isSgFunctionDefinition(currentScopeOfFunctionDeclaration) == NULL);

     if (astInterfaceStack.empty() == false)
        {
          SgInterfaceStatement* interfaceStatement = astInterfaceStack.front();

       // DQ (10/6/2008): The use of the SgInterfaceBody IR nodes allows the details of if 
       // it was a procedure name or a procedure declaration to be abstracted away and saves 
       // this detail of how it was structured in the source code in the AST (for the unparser).
          SgName name = procedureDeclaration->get_name();
          SgInterfaceBody* interfaceBody = new SgInterfaceBody(name,procedureDeclaration,/*use_function_name*/ false);
          procedureDeclaration->set_parent(interfaceStatement);
          interfaceStatement->get_interface_body_list().push_back(interfaceBody);
          interfaceBody->set_parent(interfaceStatement);
          setSourcePosition(interfaceBody);
        }
       else
        {
       // The function was not processed as part of an interface so add it to the current scope.
          currentScopeOfFunctionDeclaration->append_statement(procedureDeclaration);
        }

  // Go looking for if this was a previously declared function
  // SgFunctionSymbol* functionSymbol = trace_back_through_parent_scopes_lookup_function_symbol(procedureDeclaration->get_name(),astScopeStack.front());
     SgFunctionSymbol* functionSymbol = trace_back_through_parent_scopes_lookup_function_symbol(procedureDeclaration->get_name(),currentScopeOfFunctionDeclaration);

#if 0
     printf ("In buildProcedureSupport(): functionSymbol = %p from trace_back_through_parent_scopes_lookup_function_symbol() \n",functionSymbol);
  // printf ("In buildProcedureSupport(): procedureDeclaration scope = %p = %s \n",procedureDeclaration->get_scope(),procedureDeclaration->get_scope()->class_name().c_str());
     printf ("In buildProcedureSupport(): procedureDeclaration scope = %p \n",procedureDeclaration->get_scope());
     printf ("In buildProcedureSupport(): currentScopeOfFunctionDeclaration = %p = %s \n",currentScopeOfFunctionDeclaration,currentScopeOfFunctionDeclaration->class_name().c_str());
#endif

     if (functionSymbol != NULL)
        {
       // FMZTEST
       // cout <<"FMZTEST:Frontend::" << functionSymbol->get_name().str()<<endl;
          SgFunctionDeclaration* nondefiningDeclaration = functionSymbol->get_declaration();
          ROSE_ASSERT(nondefiningDeclaration != NULL);

          procedureDeclaration->set_firstNondefiningDeclaration(nondefiningDeclaration);

       // And set the defining declaration in the non-defining declaration
          nondefiningDeclaration->set_definingDeclaration(procedureDeclaration);

      // if the procedure is defined in same module, it should be visiable for the non-definingDeclaration.
         if (isSgClassDefinition(currentScopeOfFunctionDeclaration) != NULL) {
                   nondefiningDeclaration->set_scope(currentScopeOfFunctionDeclaration);
          }
        }
       else
        {
       // Build the function symbol and put it into the symbol table for the current scope
       // It might be that we should build a nondefining declaration for use in the symbol.
          functionSymbol = new SgFunctionSymbol(procedureDeclaration);
          currentScopeOfFunctionDeclaration->insert_symbol(procedureDeclaration->get_name(), functionSymbol);
#if 0
          printf ("In buildProcedureSupport(): Added SgFunctionSymbol = %p to scope = %p = %s \n",functionSymbol,currentScopeOfFunctionDeclaration,currentScopeOfFunctionDeclaration->class_name().c_str());
#endif
        }

  // Now push the function definition and the function body (SgBasicBlock) onto the astScopeStack
     SgBasicBlock* procedureBody               = new SgBasicBlock();
     SgFunctionDefinition* procedureDefinition = new SgFunctionDefinition(procedureDeclaration,procedureBody);

     ROSE_ASSERT(procedureDeclaration->get_definition() != NULL);

  // DQ (11/28/2010): Added specification of case insensitivity for Fortran.
     procedureBody->setCaseInsensitive(true);
     procedureDefinition->setCaseInsensitive(true);

  // Set the scope
     procedureDeclaration->set_scope(currentScopeOfFunctionDeclaration);

  // Now push the function definition onto the astScopeStack (so that the function parameters will be build in the correct scope)
     astScopeStack.push_front(procedureDefinition);

  // This code is specific to the case where the procedureDeclaration is a Fortran function (not a subroutine or data block)
  // If there was a result specificed for the function then the SgInitializedName list is returned on the astNodeStack.
     if (astNodeStack.empty() == false)
        {
          SgInitializedName* returnVar = isSgInitializedName(astNodeStack.front());
          ROSE_ASSERT(returnVar != NULL);
       // returnVar->set_scope(functionBody);
          returnVar->set_parent(procedureDeclaration);
          returnVar->set_scope(procedureDefinition);
          procedureDeclaration->set_result_name(returnVar);
          astNodeStack.pop_front();

          SgFunctionType* functionType = procedureDeclaration->get_type();
          returnVar->set_type(functionType->get_return_type());

       // Now build associated SgVariableSymbol and put it into the current scope (function definition scope)
          SgVariableSymbol* returnVariableSymbol = new SgVariableSymbol(returnVar);
          procedureDefinition->insert_symbol(returnVar->get_name(),returnVariableSymbol);

       // printf ("Processing the return var in a function \n");
       // ROSE_ASSERT(false);
        }

     if (hasDummyArgList == true)
        {
#if 0
       // Output debugging information about saved state (stack) information.
          outputState("In buildProcedureSupport(): building the function parameters");
#endif
       // Take the arguments off of the token stack (astNameStack).
          while (astNameStack.empty() == false)
             {
            // Capture the procedure parameters.
               SgName arg_name = astNameStack.front()->text;

            // printf ("arg_name = %s \n",arg_name.str());

            // Build a SgInitializedName with a SgTypeDefault and fixup the type later when we see the declaration inside the procedure.
            // SgInitializedName* initializedName = new SgInitializedName(arg_name,SgTypeDefault::createType());
            // SgInitializedName* initializedName = new SgInitializedName(arg_name,SgTypeDefault::createType(),NULL,NULL,NULL);
            // SgInitializedName* initializedName = new SgInitializedName(arg_name,SgTypeDefault::createType());
            // SgInitializedName* initializedName = new SgInitializedName(arg_name,SgTypeDefault::createType(),NULL,procedureDeclaration,NULL);
               SgInitializedName* initializedName = new SgInitializedName(arg_name,generateImplicitType(arg_name.str()),NULL,procedureDeclaration,NULL);

               procedureDeclaration->append_arg(initializedName);

               initializedName->set_parent(procedureDeclaration->get_parameterList());
               ROSE_ASSERT(initializedName->get_parent() != NULL);

            // DQ (12/17/2007): set the scope
               initializedName->set_scope(astScopeStack.front());

               setSourcePosition(initializedName,astNameStack.front());

               ROSE_ASSERT(astNameStack.empty() == false);
               astNameStack.pop_front();

            // Now build associated SgVariableSymbol and put it into the current scope (function definition scope)
               SgVariableSymbol* variableSymbol = new SgVariableSymbol(initializedName);
               procedureDefinition->insert_symbol(arg_name,variableSymbol);

            // DQ (12/17/2007): Make sure the scope was set!
               ROSE_ASSERT(initializedName->get_scope() != NULL);
             }

          ROSE_ASSERT(procedureDeclaration->get_args().empty() == false);
        }

  // printf ("Added function programName = %s (symbol = %p) to scope = %p = %s \n",tempName.str(),functionSymbol,astScopeStack.front(),astScopeStack.front()->class_name().c_str());

  // Now push the function definition and the function body (SgBasicBlock) onto the astScopeStack
     astScopeStack.push_front(procedureBody);

     procedureBody->set_parent(procedureDefinition);
     procedureDefinition->set_parent(procedureDeclaration);

     ROSE_ASSERT(procedureDeclaration->get_parameterList() != NULL);

  // DQ (1/23/2008): This is not set with the function declaration
  // setSourcePosition(procedureDeclaration->get_parameterList());

  // Unclear if we should use the same token list for resetting the source position in all three IR nodes.
     setSourcePosition(procedureDefinition);
     setSourcePosition(procedureBody);

  // DQ (1/21/2008): This has been set earlier so that it could be done more accurately
  // setSourcePosition(procedureDeclaration);

  // Now setup the function type and reset it in the procedureDeclaration
  // SgFunctionType* functionType = new SgFunctionType(SgTypeVoid::createType(),false);
  // procedureDeclaration->set_type(functionType);

#if 0
  // DQ (10/10/2010): Added tests to make sure that start and end positions are set reasonably.
     ROSE_ASSERT(procedureDeclaration->get_startOfConstruct()->get_line() > 0);
     ROSE_ASSERT(procedureDefinition->get_startOfConstruct()->get_line()  > 0);
     ROSE_ASSERT(procedureBody->get_startOfConstruct()->get_line()        > 0);
     ROSE_ASSERT(procedureDeclaration->get_endOfConstruct()->get_line() >= procedureDeclaration->get_startOfConstruct()->get_line());
     ROSE_ASSERT(procedureDefinition->get_endOfConstruct()->get_line()  >= procedureDefinition->get_startOfConstruct()->get_line());
     ROSE_ASSERT(procedureBody->get_endOfConstruct()->get_line()        >= procedureBody->get_startOfConstruct()->get_line());
#endif
   }


void
markDoLoopAsUsingEndDo()
   {
#if 0
  // Output debugging information about saved state (stack) information.
     outputState("In markDoLoopAsUsingEndDo()");
#endif

  // Look for the last statement (should be a SgFortranDo)
     SgScopeStatement* currentScope = astScopeStack.front();

  // printf ("In markDoLoopAsUsingEndDo: currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());

     SgFortranDo* doStatement = isSgFortranDo(currentScope);
     if (doStatement != NULL)
        {
       // Mark the do-loop to use the "end do" new style syntax.
       // printf ("In markDoLoopAsUsingEndDo: Marking the doStatement as new style syntax \n");

       // DQ (12/26/2007): This field is depricated in favor of the has_end_statement boolean field used uniformally in several IR nodes).
       // doStatement->set_old_style(false);
          doStatement->set_has_end_statement(true);
        }
       else
        {
          SgWhileStmt* whileStatement = isSgWhileStmt(currentScope);
          if (whileStatement != NULL)
             {
            // Mark the do-loop to use the "end do" new style syntax.
            // printf ("I don't think we have to make the while statement as new sytle! \n");
            // doStatement->set_old_style(false);
               whileStatement->set_has_end_statement(true);
             }
            else
             {
               printf ("Error: do-loop or while-loop should be at the top of the astScopeStack! currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
               ROSE_ASSERT(false);
             }
        }
   }


SgExpression*
buildSubscriptExpression ( bool hasLowerBound, bool hasUpperBound, bool hasStride, bool isAmbiguous )
   {
  // DQ (4/26/2008): Note that test2008_21.f90 demonstrates that this function is called to evaluate 
  // the initialization of types in structures (fortran types).  

  // DQ (4/26/2008): Later... This function should not be called for initialization now!

     SgExpression* subscript = NULL;

     if (hasUpperBound == true)
        {
       // SgSubscriptColon* subscript = new SgSubscriptColon();

          if (hasLowerBound == true)
             {
               if (hasStride == true)
                  {
                 // Get three values off of the stack!
                    SgExpression* stride = astExpressionStack.front();
                    astExpressionStack.pop_front();
                    SgExpression* upperBound = astExpressionStack.front();
                    astExpressionStack.pop_front();
                    SgExpression* lowerBound = astExpressionStack.front();
                    astExpressionStack.pop_front();

                    subscript = new SgSubscriptExpression(lowerBound,upperBound,stride);
                  }
                 else
                  {
                 // Case of a single literal specified, the expression is already on the stack!
                 // ROSE_ASSERT(isAmbiguous == true);

                 // SgExpression* stride = new SgNullExpression();
                    SgExpression* stride = new SgIntVal(1,"1");
                    SgExpression* upperBound = astExpressionStack.front();
                    astExpressionStack.pop_front();
                    SgExpression* lowerBound = astExpressionStack.front();
                    astExpressionStack.pop_front();

                    subscript = new SgSubscriptExpression(lowerBound,upperBound,stride);

                    setSourcePosition(stride);
                  }
             }
            else
             {
               SgExpression* lowerBound = new SgNullExpression();

               if (hasStride == true)
                  {
                 // Get three values off oc the stack!
                    SgExpression* stride = astExpressionStack.front();
                    astExpressionStack.pop_front();
                    SgExpression* upperBound = astExpressionStack.front();
                    astExpressionStack.pop_front();

                    subscript = new SgSubscriptExpression(lowerBound,upperBound,stride);
                  }
                 else
                  {
                    SgExpression* stride = new SgIntVal(1,"1");

                    SgExpression* upperBound = astExpressionStack.front();
                    astExpressionStack.pop_front();

                    subscript = new SgSubscriptExpression(lowerBound,upperBound,stride);

                    setSourcePosition(stride);
                  }

               setSourcePosition(lowerBound);
             }
        }
       else
        {
          if (hasLowerBound == true)
             {
               if (hasStride == true)
                  {
                    SgExpression* stride = astExpressionStack.front();
                    astExpressionStack.pop_front();
                    SgExpression* upperBound = new SgNullExpression();
                    SgExpression* lowerBound = astExpressionStack.front();
                    astExpressionStack.pop_front();

                    setSourcePosition(upperBound);

                    subscript = new SgSubscriptExpression(lowerBound,upperBound,stride);
                  }
                 else
                  {
                 // This is the case of scalar indexing, so generate a scalar value instead of a SgSubscriptExpression.

                 // The isAmbiguous flag helps distinguish between the cases of array(1) and array(1:)
                    if (isAmbiguous == false)
                       {
                      // This is the case of array(1:)
                         SgExpression* stride = new SgIntVal(1,"1");

                         SgExpression* lowerBound = astExpressionStack.front();
                         astExpressionStack.pop_front();
                         SgExpression* upperBound = new SgNullExpression();

                         setSourcePosition(upperBound);

                         subscript = new SgSubscriptExpression(lowerBound,upperBound,stride);
                         setSourcePosition(stride);
                       }
                      else
                       {
                      // This is the case of array(1)
                      // See also that this is used for type initialization (see test2008_21.f90)
                         ROSE_ASSERT(astExpressionStack.empty() == false);
                         if (astExpressionStack.empty() == false)
                            {
                              subscript = astExpressionStack.front();
                              astExpressionStack.pop_front();
                            }
                           else
                            {
                           // This branch should not be called now!  This function should not be used in initialization.
                              printf ("subscript evaluation used in type initialization \n");
                              ROSE_ASSERT(false);
                            }
                       }
                  }
             }
            else
             {
               SgExpression* lowerBound = new SgNullExpression();
               SgExpression* upperBound = new SgNullExpression();

               if (hasStride == true)
                  {
                    SgExpression* stride = astExpressionStack.front();
                    astExpressionStack.pop_front();

                 // subscript = new SgSubscriptColon(lowerBound,stride);
                    subscript = new SgSubscriptExpression(lowerBound,upperBound,stride);
                  }
                 else
                  {
                 // DQ (4/30/2008): This case makes no requirement of the astExpressionStack

                 // SgExpression* stride     = new SgNullExpression();
                    SgExpression* stride     = new SgIntVal(1,"1");

                    setSourcePosition(stride);

                 // subscript = new SgSubscriptColon(lowerBound,stride);
                    subscript = new SgSubscriptExpression(lowerBound,upperBound,stride);
                  }

               setSourcePosition(lowerBound);
               setSourcePosition(upperBound);
             }           

        }

     ROSE_ASSERT(subscript != NULL);

  // This could be a integer expression or a SgSubscriptExpression
     SgSubscriptExpression* subscriptExpression = isSgSubscriptExpression(subscript);
     if (subscriptExpression != NULL)
        {
       // Set the parents of all the parts of the SgSubscriptExpression
          subscriptExpression->get_lowerBound()->set_parent(subscript);
          subscriptExpression->get_upperBound()->set_parent(subscript);
          subscriptExpression->get_stride()->set_parent(subscript);

       // Only set the position of the new SgSubscriptExpression, not a previously built integer expression.
          setSourcePosition(subscriptExpression);
        }

     ROSE_ASSERT(subscript != NULL);
     return subscript;
   }



bool
isPubliclyAccessible( SgSymbol* symbol )
   {
     bool returnValue = false;
     SgNode* symbol_basis = symbol->get_symbol_basis();

     SgDeclarationStatement* declaration = isSgDeclarationStatement(symbol_basis);
     if (declaration != NULL)
        {
       // printf ("In isPubliclyAccessible(): declaration = %p = %s symbol = %s \n",declaration,declaration->class_name().c_str(),symbol->get_name().str());

       // Publically accessible is either declared explicitly as public, or not defined as anything (default in Fortran is public).
          if (declaration->get_declarationModifier().get_accessModifier().isPublic() == true ||
              declaration->get_declarationModifier().get_accessModifier().isUndefined() == true)
             {
               returnValue = true;
             }

       // declaration->get_declarationModifier().get_accessModifier().display("In isPubliclyAccessible()");
        }
       else
        {
          SgInitializedName* initializedName = isSgInitializedName(symbol_basis);
          if (initializedName != NULL)
             {
               SgNode* parent = initializedName->get_parent();
               SgDeclarationStatement* declaration = isSgDeclarationStatement(parent);
               if (declaration != NULL)
                  {
#if 0
                 // printf ("declaration (from SgInitializedName) = %p = %s \n",declaration,declaration->class_name().c_str());
                    printf ("In isPubliclyAccessible(): declaration = %p = %s symbol = %s \n",declaration,declaration->class_name().c_str(),symbol->get_name().str());
                    declaration->get_declarationModifier().display("In isPubliclyAccessible()");
#endif
                    if (declaration->get_declarationModifier().get_accessModifier().isPublic() == true ||
                        declaration->get_declarationModifier().get_accessModifier().isUndefined() == true)
                       {
                         returnValue = true;
                       }
                      else
                       {
                      // DQ (9/11/2010): See test2007_176.f03 for an example of the F2003 use of the "protected" keyword.
                      // If it was marked as protected in the variable then either it will be found in a parent scope 
                      // (for module functions) or it should not be put into the current scope as an aliased variable symbol.
                      // printf ("This may still be a protected variable. \n");

                      // DQ (9/11/2010): But make sure this is not marked as protected (is so then we want to include it as an aliased symbol).
                         if (initializedName->get_protected_declaration() == true)
                            {
                           // This should not be considered a publicly accessible variable (don't put it into the synbol table as an aliased symbol.
                              if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                                   printf ("In isPubliclyAccessible(): Note that this variable is marked as protected (so must be included as an aliased symbol \n");

                              returnValue = true;
                            }
                       }

                 // declaration->get_declarationModifier().get_accessModifier().display("In isPubliclyAccessible()");
                  }
             }
            else
             {
            // This case is not handled yet
               printf ("In isPubliclyAccessible(): Uknown symbol_basis = %p = %s \n",symbol_basis,symbol_basis->class_name().c_str());
               ROSE_ASSERT(false);
             }
        }

     return returnValue;
   }

void
cleanupTypeStackAfterDeclaration()
   {
  // This function is called in R501 c_action_type_declaration_stmt().

  // We should have used all the types stored on the stack at this point!
  // Except for the case of an array type which will have pushed the base type 
  // onto the stack and then an array type.  In this case we will still have 
  // the base type on the stack.  If there was a scalar variable then it will 
  // have used the type on the stack, but we should take care of this by always 
  // pushing a type onto the stack for each variable to use and then making 
  // sure that we have the base type still on the stack at this point, so the 
  // stack at this point should never be empty and should have the unused base 
  // type on top (and the stack size should be 1).

     if (astTypeStack.empty() == false)
        {
       // This should be only a base type that has been left on the stack.
       // printf ("WARNING, astTypeStack not empty: astTypeStack.front() = %p = %s \n",astTypeStack.front(),astTypeStack.front()->class_name().c_str());

       // If there was an array declaration then the base type is left on the stack, else 
       // the stack is empty.  Cleanup the stack of the one possible base type left there!
       // astTypeStack.pop_front();

          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
             printf ("WARNING, astTypeStack not empty: astTypeStack.front() = %p = %s = %s (CLEARING astTypeStack) \n",astTypeStack.front(),astTypeStack.front()->class_name().c_str(),SageInterface::get_name(astTypeStack.front()).c_str());

          astTypeStack.clear();
#if 0
          if (astTypeStack.empty() == false)
               outputState("Error: astTypeStack.empty() == false in R502 c_action_declaration_type_spec()");
#endif
          ROSE_ASSERT(astTypeStack.empty() == true);
        }

     if (astBaseTypeStack.empty() == false)
        {
          if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
               printf ("WARNING, astBaseTypeStack not empty: astBaseTypeStack.front() = %p = %s = %s (CLEARING astBaseTypeStack) \n",astBaseTypeStack.front(),astBaseTypeStack.front()->class_name().c_str(),SageInterface::get_name(astBaseTypeStack.front()).c_str());

          astBaseTypeStack.clear();
#if 0
          if (astBaseTypeStack.empty() == false)
               outputState("Error: astBaseTypeStack.empty() == false in R502 c_action_declaration_type_spec()");
#endif
          ROSE_ASSERT(astBaseTypeStack.empty() == true);
        }
   }




void
buildVariableDeclarationAndCleanupTypeStack( Token_t * label )
   {
  // This function is called in R501 c_action_type_declaration_stmt().

  // DQ (1/28/2009): Part of fix for nested include (test2009_14.f).
  // Only try to build a varialbe if there is information to support this on the stack.
  // I am trying to have variable built earlier than before since the Fortran "include"
  // mechanism can be called before this R501 rule and that causes problems.  Basically
  // each new include file needs to be started with an empty stack(s).
     if (astNodeStack.empty() == false && astBaseTypeStack.empty() == false)
        {
#if 1
       // Output debugging information about saved state (stack) information.
          outputState("At TOP of buildVariableDeclarationAndCleanupTypeStack() (before calling buildVariableDeclaration())");
#endif

          SgVariableDeclaration* variableDeclaration = buildVariableDeclaration(label,false);

          ROSE_ASSERT(variableDeclaration->get_file_info()->isCompilerGenerated() == false);

       // DQ (11/29/2007): commented out, we can't assume this (see test2007_133.f03)
       // DQ (9/30/2007):
       // I think this is now uniformally true for all type declarations.  If so then we can
       // remove the conditionaly handling below.  See the note in R504 c_action_entity_decl()
       // for more details.
       // ROSE_ASSERT(astTypeStack.empty() == true);
#if 1
          outputState("In buildVariableDeclarationAndCleanupTypeStack() (after buildVariableDeclaration())");
#endif
       // We should have used all the types stored on the stack at this point!
       // Except for the case of an array type which will have pushed the base type 
       // onto the stack and then an array type.  In this case we will still have 
       // the base type on the stack.  If there was a scalar variable then it will 
       // have used the type on the stack, but we should take care of this by always 
       // pushing a type onto the stack for each variable to use and then making 
       // sure that we have the base type still on the stack at this point, so the 
       // stack at this point should never be empty and should have the unused base 
       // type on top (and the stack size should be 1).
       // ROSE_ASSERT(astTypeStack.empty() == true);
       // ROSE_ASSERT(astTypeStack.size() == 1);
       // astTypeStack.pop_front();

       // DQ (1/27/2009): Refactored code so that I can handle test2009_13.f (nested include files).
          cleanupTypeStackAfterDeclaration();

          ROSE_ASSERT(getTopOfScopeStack()->variantT() == V_SgBasicBlock || getTopOfScopeStack()->variantT() == V_SgClassDefinition);

          getTopOfScopeStack()->append_statement(variableDeclaration);
        }
   }


bool
isARoseModuleFile( string filename )
   {
     bool result = false;

     string targetSuffix = MOD_FILE_SUFFIX;
     size_t filenameLength = filename.size();

  // if ( (filenameLength > 5) && (filename.substr(filenameLength - 5) == ".rmod") )
     if ( (filenameLength > targetSuffix.size()) && (filename.substr(filenameLength - 5) == MOD_FILE_SUFFIX) )
          result = true;
#if 0
     printf ("################ filename = %s result = %s \n",filename.c_str(),(result == true) ? "true" : "false");
#endif

     return result;
   }


void
generateAssignmentStatement(Token_t* label, bool isPointerAssignment )
   {
  // This function builds the SgAssignOp and the SgExprStatement and 
  // inserts it into the current scope.

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of generateAssignmentStatement()");
#endif

     ROSE_ASSERT(astExpressionStack.empty() == false);
     SgExpression* rhs = astExpressionStack.front();
     astExpressionStack.pop_front();

     ROSE_ASSERT(astExpressionStack.empty() == false);
     SgExpression* lhs = astExpressionStack.front();
     astExpressionStack.pop_front();

  // Note that the type provided is NULL, since ROSE will internally (dynamically) evaluate the type 
  // as required.  This avoids state in the AST and better supports transformations on the AST.
  // SgAssignOp* assignmentExpr = new SgAssignOp(lhs,rhs,NULL);
     SgExpression* assignmentExpr = NULL;
     if (isPointerAssignment == true)
        {
       // This is "p => x"
          assignmentExpr = new SgPointerAssignOp(lhs,rhs,NULL);
        }
       else
        {
       // This is "p = x"
          assignmentExpr = new SgAssignOp(lhs,rhs,NULL);
        }

     setSourcePosition(assignmentExpr);

  // DQ (1/22/2008): Set the source position based on the internal expressions
     resetSourcePosition(assignmentExpr,lhs);
  // resetSourcePosition(assignmentExpr,rhs);

  // Now build the expression statement required for insertion into the current 
  // scope (note that expressions cannot be directly inserted into scopes).

     SgExprStatement* expressionStatement = new SgExprStatement(assignmentExpr);

     if (label != NULL)
        {
       // A label was provided so set the label in the expressionStatement

       // Setup the label on the statement if it is available.
          setStatementNumericLabel(expressionStatement,label);

       // If there is a label then at least try to use it to set the source 
       // position approximately (better than nothing).
          setSourcePosition(expressionStatement,label);
        }
       else
        {
       // No source position information is available
          setSourcePosition(expressionStatement);

       // DQ (1/22/2008): Try this
          resetSourcePosition(expressionStatement,assignmentExpr);
        }

     setStatementNumericLabelUsingStack(expressionStatement);

  // DQ (1/31/2009): Moved this to the TOP of the function.
  // Refactored the code to build support function
  // initialize_global_scope_if_required();
  // build_implicit_program_statement_if_required();

     SgScopeStatement* currentScope = getTopOfScopeStack();
     currentScope->append_statement(expressionStatement);

     if ( SgProject::get_verbose() > DEBUG_RULE_COMMENT_LEVEL )
          printf ("Pushing this expressionStatement onto the astNodeStack for optional use by where statement \n");

  // This is needed for test2007_67.f90
     astNodeStack.push_front(expressionStatement);

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of generateAssignmentStatement()");
#endif

  // Error checking for astExpressionStack
     ROSE_ASSERT(astScopeStack.empty() == false);
     ROSE_ASSERT(astScopeStack.front()->get_parent() != NULL);
     SgWhereStatement* whereStatement = isSgWhereStatement(astScopeStack.front()->get_parent());
     SgIfStmt* ifStatement = isSgIfStmt(astScopeStack.front()->get_parent());
     if (whereStatement != NULL || ifStatement != NULL)
        {
       // If in a where statement produced with R 619:section-subscript-list__begin then the 
       // condition is on the stack, else if it was produced with R744:where-construct-stmt 
       // then the condition was used directly and already cleared from the stack.
       // ROSE_ASSERT(astExpressionStack.empty() == false);
          ROSE_ASSERT(astExpressionStack.size() <= 1);
        }
       else
        {
       // If this is NOT a where statement then the stack should be empty.
          ROSE_ASSERT(astExpressionStack.empty() == true);
        }
   }

void
convertBaseTypeOnStackToPointer()
   {
  // DQ (1/20/2011): This function only uses the astBaseTypeStack if there is 
  // not an entry on the astTypeStack.  So now the function is not well named!

     ROSE_ASSERT(astBaseTypeStack.empty() == false);
  // printf ("In convertBaseTypeOnStackToPointer(): astTypeStack.empty() = %s \n",astTypeStack.empty() ? "true" : "false");

  // If there is already en entry on the astTypeStack, then modify it.
  // However, I am not clear if this is always the only semantics that we want.
     if (astTypeStack.empty() == true)
        {
       // Use the type on the astBaseTypeStack, since there is no evolving type on the astBaseTypeStack
          SgType* baseType = astBaseTypeStack.front();
          astBaseTypeStack.pop_front();
          ROSE_ASSERT(astBaseTypeStack.empty() == true);
          SgPointerType* pointerType = new SgPointerType(baseType);
       // astBaseTypeStack.push_front(pointerType);
          astBaseTypeStack.push_front(pointerType);
        }
       else
        {
       // If there is a type being assembled on the astTypeStack, then use it as a base type for the SgPointerType!
          SgType* baseType = astTypeStack.front();
          astTypeStack.pop_front();
          ROSE_ASSERT(astTypeStack.empty() == true);
          SgPointerType* pointerType = new SgPointerType(baseType);
          astTypeStack.push_front(pointerType);
        }
   }

// FMZ(12/21/2009): added support for "team_default"/"team_world"
SgVariableSymbol*
add_external_team_decl(string teamName) {

          // make external team :: team_world
              // create the team "type"
             SgTypeCAFTeam* cafTeamType = SgTypeCAFTeam::createType();
             ROSE_ASSERT( cafTeamType != NULL);

             // build variable 
             SgInitializedName* teamVar= NULL;

             teamVar = new SgInitializedName(teamName,cafTeamType,NULL);

             ROSE_ASSERT(teamVar != NULL);
             setSourcePosition(teamVar);

             SgVariableDeclaration* teamDec = new SgVariableDeclaration();
             ROSE_ASSERT(teamDec!=NULL);
             setSourcePosition(teamDec);
             teamDec->set_parent(astScopeStack.front());
             teamDec->set_definingDeclaration(teamDec);
             teamDec->get_declarationModifier().get_storageModifier().setExtern() ;

             teamDec->append_variable(teamVar,NULL);
             teamVar->set_declptr(teamDec);
             teamVar->set_scope(astScopeStack.front());

             SgVariableSymbol* teamId = new SgVariableSymbol(teamVar);
             ROSE_ASSERT(teamId != NULL);

             astScopeStack.front()->insert_symbol(teamName,teamId);

             return teamId;
 }


void
fixupModuleScope( SgClassDefinition* moduleScope )
   {
  // Fixes up function symbols to be in the correct scope when the function call appears before the function declaration.
     ROSE_ASSERT(moduleScope != NULL);

     ROSE_ASSERT(moduleScope->containsOnlyDeclarations() == true);
     const SgDeclarationStatementPtrList & declarationList = moduleScope->getDeclarationList();

  // printf ("Inside of fixupModuleScope(moduleScope = %p) \n",moduleScope);

     SgDeclarationStatementPtrList::const_iterator i = declarationList.begin();
     while (i != declarationList.end())
        {
          SgProcedureHeaderStatement* functionDeclaration = isSgProcedureHeaderStatement(*i);
          if (functionDeclaration != NULL)
             {
            // Note that at this point where fixupModuleScope() is called the astScopeStack has already poped the module scope.
            // So a valid local_symbol means that the declaration was not placed into the module scope (e.g incorrectly placed
            // into the global scope).

            // Test if there is a valid symbol associated with each function declaration.
               SgSymbol* tempSymbol = functionDeclaration->get_symbol_from_symbol_table();
               if (tempSymbol == NULL)
                  {
                 // These function without a valid symbol need to be fixed up.
                    if ( SgProject::get_verbose() > 0 )
                         printf ("(tempSymbol == NULL): Identified a function declaration = %p = %s = %s \n",functionDeclaration,functionDeclaration->class_name().c_str(),functionDeclaration->get_name().str());

                    SgName variableName              = functionDeclaration->get_name();
                    SgVariableSymbol* variableSymbol = NULL;
                    SgFunctionSymbol* functionSymbol = NULL;
                    SgClassSymbol*    classSymbol    = NULL;

                    trace_back_through_parent_scopes_lookup_variable_symbol_but_do_not_build_variable(variableName,moduleScope,variableSymbol,functionSymbol,classSymbol);

                    if (functionSymbol != NULL)
                       {
                         if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                              printf ("Found functionSymbol = %p function name = %s (erasing all traces of this function in the wrong scope) \n",functionSymbol,variableName.str());

                         SgSymbolTable* symbolTable = isSgSymbolTable(functionSymbol->get_parent());
                         ROSE_ASSERT(symbolTable != NULL);

                      // Remove the symbol from the scope were it was first put (incorrectly).
                         symbolTable->remove(functionSymbol);

                      // Insert the functionSymbol into the moduleScope
                         moduleScope->insert_symbol(variableName,functionSymbol);
                       }
                      else
                       {
                      // DQ (8/28/2010): Report more diagnostics.
                         printf ("Warning: In fixupModuleScope() there was no SgFunctionSymbol found reset into the module scope \n");
                       }
                  }
                 else
                  {
                 // These functions don't require any fixup.
                 // printf ("(local_symbol != NULL): Identified a function declaration = %p = %s = %s \n",functionDeclaration,functionDeclaration->class_name().c_str(),functionDeclaration->get_name().str());
                  }
             }

          i++;
        }
   }



SgClassSymbol* 
buildIntrinsicModule_ISO_C_BINDING()
   {
     SgClassSymbol* moduleSymbol = NULL;

     const string name = "ISO_C_BINDING";

     if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
          printf ("Building the module file for ISO_C_BINDING \n");

     return moduleSymbol;
   }



SgClassSymbol* 
buildIntrinsicModule ( const string & name )
   {
     SgClassSymbol*  moduleSymbol = NULL;

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of buildIntrinsicModule()");
#endif

  // DQ (10/23/2010): Intrinsic module must be filtered out since they are special.
  // These include: ISO_C_BINDING, ISO_FORTRAN_ENV, OMP_LIB, OMP_LIB_KINDS.  There 
  // are no others that I know of.
     if (matchingName(name,"ISO_C_BINDING") == true)
        {
       // This is the ISO_C_BINDING intrisic module and so there is a list of names and types that need to be inserted into the local scope.
       // printf ("Insert types and variables from ISO_C_BINDING into the local scope \n");

       // DQ (10/26/2010): This support has been refactored.
          moduleSymbol = buildIntrinsicModule_ISO_C_BINDING();
        }

     if (matchingName(name,"ISO_FORTRAN_ENV") == true)
        {
       // This is the ISO_FORTRAN_ENV intrisic module and so there is a list of names and types that need to be inserted into the local scope.
          printf ("Insert types and variables from ISO_FORTRAN_ENV into the local scope \n");

          printf ("Sorry: ISO_FORTRAN_ENV intrinsic module not implemented yet. \n");
          ROSE_ASSERT(false);

          ROSE_ASSERT(moduleSymbol != NULL);
        }

     if (matchingName(name,"IEEE_EXCEPTIONS") == true)
        {
       // This is the IEEE_EXCEPTIONS intrisic module and so there is a list of names and types that need to be inserted into the local scope.
          printf ("Insert types and variables from IEEE_EXCEPTIONS into the local scope \n");

          printf ("Sorry: IEEE_EXCEPTIONS intrinsic module not implemented yet. \n");
          ROSE_ASSERT(false);

          ROSE_ASSERT(moduleSymbol != NULL);
        }

     if (matchingName(name,"IEEE_ARITHMETIC") == true)
        {
       // This is the IEEE_ARITHMETIC intrisic module and so there is a list of names and types that need to be inserted into the local scope.
          printf ("Insert types and variables from IEEE_ARITHMETIC into the local scope \n");

          printf ("Sorry: IEEE_ARITHMETIC intrinsic module not implemented yet. \n");
          ROSE_ASSERT(false);

          ROSE_ASSERT(moduleSymbol != NULL);
        }

     if (matchingName(name,"IEEE_FEATURES") == true)
        {
       // This is the IEEE_FEATURES intrisic module and so there is a list of names and types that need to be inserted into the local scope.
          printf ("Insert types and variables from IEEE_FEATURES into the local scope \n");

          printf ("Sorry: IEEE_FEATURES intrinsic module not implemented yet. \n");
          ROSE_ASSERT(false);

          ROSE_ASSERT(moduleSymbol != NULL);
        }

     if (matchingName(name,"OMP_LIB") == true)
        {
       // This is the OMP_LIB intrisic module and so there is a list of names and types that need to be inserted into the local scope.
          printf ("Insert types and variables from OMP_LIB into the local scope \n");

          printf ("Error: OMP_LIB intrinsic module not defined (non-standard intrinsic module). \n");
          ROSE_ASSERT(false);

          ROSE_ASSERT(moduleSymbol != NULL);
        }

     if (matchingName(name,"OMP_LIB_KINDS") == true)
        {
       // This is the OMP_LIB_KINDS intrisic module and so there is a list of names and types that need to be inserted into the local scope.
          printf ("Insert types and variables from OMP_LIB_KINDS into the local scope \n");

          printf ("Error: OMP_LIB_KINDS intrinsic module not defined (non-standard intrinsic module). \n");
          ROSE_ASSERT(false);

          ROSE_ASSERT(moduleSymbol != NULL);
        }

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At BOTTOM of buildIntrinsicModule()");
#endif

  // printf ("Leaving buildIntrinsicModule() \n");

  // It is OK to return NULL, few modules are intrinsic.
  // ROSE_ASSERT(moduleSymbol != NULL);

     return moduleSymbol;
   }

void
push_token(string s)
   {
  // This is the case of an option not being specified, as in "read(1)" instead of "read(UNIT=1)"
  // To make the astExpressionStack match the astNameStack we have to push a default token onto the astNameStack.
  // Token_t* defaultToken = create_token(0,0,0,"fmt");
     Token_t* defaultToken = create_token(0,0,0,s.c_str());
     ROSE_ASSERT(defaultToken != NULL);
     astNameStack.push_front(defaultToken);
   }



void
replace_return_type (SgFunctionType* functionType, SgFunctionDeclaration* functionDeclaration, SgClassSymbol* derivedTypeSymbol)
   {
     bool has_ellipses = functionType->get_has_ellipses();
  // SgFunctionParameterTypeList* argument_list = functionType->get_argument_list();

     ROSE_ASSERT(derivedTypeSymbol->get_declaration() != NULL);
     ROSE_ASSERT(derivedTypeSymbol->get_declaration()->get_type() != NULL);

     SgFunctionType* newFunctionType = new SgFunctionType(derivedTypeSymbol->get_declaration()->get_type(),has_ellipses);
  // printf ("newFunctionType = %p \n",newFunctionType);

  // DQ (12/26/2010): Not clear if this should be allowed or not...I would like to fix it later.
  // printf ("NOTE: Sharing the function type argument list in use_statement_fixup() \n");
  // newFunctionType->set_argument_list(argument_list);

     ROSE_ASSERT(functionType->get_argument_list() != NULL);
     SgTypePtrList & functionArgumentTypeList = functionType->get_arguments();
     for (unsigned int i=0; i < functionArgumentTypeList.size(); i++)
        {
          newFunctionType->append_argument(functionArgumentTypeList[i]);
        }

  // Overwrite the function type for the function we are fixing up.
  // Note that the type we are overwriting may be shared so we can't delete it.
     functionDeclaration->set_type(newFunctionType);

#if 0
     printf ("Resetting the function type may cause this to have the mangled name unloaded from the mangled name cache... \n");
#endif

  // functionDeclaration->get_scope()->print_symboltable ("In use_statement_fixup() (after modifications 1)");

  // I think we have to do this explicitly...double check on this as debugging step...
  // Reinsert the function into the symbol using the new function type of the fixed up function.
  // printf ("Insert the new SgFunctionType into the global function type table! \n");
  // functionDeclaration->get_scope()->insert_function(functionDeclaration);

  // Now build the function call and use the arguments from the ExprList on the top of the astExpressionStack!
     SgFunctionSymbol* newFunctionSymbol = new SgFunctionSymbol(functionDeclaration);

  // Insert the function into the global scope so that we can find it later.
     functionDeclaration->get_scope()->insert_symbol(functionDeclaration->get_name(),newFunctionSymbol);

  // functionDeclaration->get_scope()->print_symboltable ("In use_statement_fixup() (after modifications 2)");
   }


void
fixup_possible_incomplete_function_return_type()
   {
  // DQ (12/26/2010): Factor out the fixup required for where use statements are used (e.g. function return types).

#if 1
  // Output debugging information about saved state (stack) information.
     outputState("At TOP of fixup_possible_incomplete_function_return_type()");
#endif

  // SgFunctionDefinition* functionDefinition   = TransformationSupport::getFunctionDefinition(astScopeStack.front());
     SgFunctionDeclaration* functionDeclaration = TransformationSupport::getFunctionDeclaration(astScopeStack.front());

  // printf ("functionDeclaration = %p \n",functionDeclaration);
     if (functionDeclaration != NULL)
        {
       // printf ("functionDeclaration->get_name() = %s \n",functionDeclaration->get_name().str());

          SgFunctionType* functionType = isSgFunctionType(functionDeclaration->get_type());
          ROSE_ASSERT(functionType != NULL);
       // printf ("functionType = %p = %s \n",functionType,functionType->class_name().c_str());

          SgType* returnType = functionType->get_return_type();
          ROSE_ASSERT(returnType != NULL);

       // printf ("returnType = %p = %s \n",returnType,returnType->class_name().c_str());
          SgTypeDefault* defaultType = isSgTypeDefault(returnType);
          if (defaultType != NULL)
             {
            // The default type has to be fixed up...

               SgName derivedTypeName = defaultType->get_name();
            // printf ("derivedTypeName = %s \n",derivedTypeName.str());

            // functionDeclaration->get_scope()->print_symboltable ("In use_statement_fixup() (before modifications)");

               SgClassSymbol* derivedTypeSymbol = trace_back_through_parent_scopes_lookup_derived_type_symbol( derivedTypeName, getTopOfScopeStack() );
            // printf ("derivedTypeSymbol = %p \n",derivedTypeSymbol);
            // ROSE_ASSERT(derivedTypeSymbol != NULL);
               if (derivedTypeSymbol != NULL)
                  {
                    replace_return_type (functionType,functionDeclaration,derivedTypeSymbol);
                  }
             }
            else
             {
            // Handle case of test2010_180.f90 (if a type name in the current scope matches the name of 
            // the specified return type then we want to fixup the function return type!

            // printf ("returnType = %p = %s \n",returnType,returnType->class_name().c_str());
               SgClassType* classType = isSgClassType(returnType);
               if (classType != NULL)
                  {
                 // The current SgClassType might have to be swapped for another in the function scope (another kind of fixup)...

                 // printf ("The current SgClassType might have to be swapped for another in the function scope (another kind of fixup)... \n");

                    SgName derivedTypeName = classType->get_name();
                 // printf ("derivedTypeName = %s \n",derivedTypeName.str());

                    SgClassSymbol* derivedTypeSymbol = trace_back_through_parent_scopes_lookup_derived_type_symbol( derivedTypeName, getTopOfScopeStack() );

                    replace_return_type (functionType,functionDeclaration,derivedTypeSymbol);
                  }
             }
        }

  // printf ("Exiting at bottom of use_statement_fixup() \n");
  // ROSE_ASSERT(false);

  // printf ("Leaving fixup_possible_incomplete_function_return_type() \n");
   }

void
use_statement_fixup()
   {
  // printf ("Inside of use_statement_fixup() \n");

  // Refactored this code to be called in R433 and R1109.
     fixup_possible_incomplete_function_return_type();

  // printf ("Leaving use_statement_fixup() \n");
   }


string 
generateQualifiedName(const std::vector<MultipartReferenceType> & qualifiedNameList)
   {
  // DQ (12/29/2010): This function isolates some of the R612 and R613 handling and support.
     string qualifiedNameString;

     size_t numberOfParts = qualifiedNameList.size();
     for (size_t i = 0; i < numberOfParts; i++)
        {
          qualifiedNameString = qualifiedNameString + qualifiedNameList[i].name;

          if (i < numberOfParts-1)
               qualifiedNameString = qualifiedNameString + '%';
        }

  // printf ("In generateQualifiedName(): qualifiedNameString = %s \n",qualifiedNameString.c_str());

     return qualifiedNameString;
   }

void
fixup_forward_type_declarations()
   {
#if 0
     printf ("Inside of fixup_forward_type_declarations() \n");
#endif
     SgScopeStatement* currentScope = astScopeStack.front();
#if 0
     printf ("Searching declarations in currentScope = %p = %s \n",currentScope,currentScope->class_name().c_str());
#endif
     Rose_STL_Container<SgNode*> typeList = NodeQuery::generateListOfTypes(currentScope);
#if 0
     printf ("typeList.size() = %zu \n",typeList.size());
#endif

     for (size_t i = 0; i < typeList.size(); i++)
        {
#if 0
          printf ("typeList[%zu] = %p = %s \n",i,typeList[i],typeList[i]->class_name().c_str());
#endif
          SgTypeDefault* defaultType = isSgTypeDefault(typeList[i]);
          if (defaultType != NULL)
             {
            // This uses the order of the vector generated from NodeQuery::generateListOfTypes(), this might be a problem.
               size_t parentTypeIndex = i-1;
               SgType* parentType = isSgType(typeList[parentTypeIndex]);
               ROSE_ASSERT(parentType != NULL);
            // ROSE_ASSERT(parentType->containsInternalTypes() == true);
               if (parentType->containsInternalTypes() == true)
                  {
                    string nameOfIntendedType = defaultType->get_name();
#if 0
                    printf ("Reset the base type in parentType = %p = %s to nameOfIntendedType = %s \n",parentType,parentType->class_name().c_str(),nameOfIntendedType.c_str());
#endif
                    SgSymbol* symbolOfIntendedType = currentScope->lookup_symbol(nameOfIntendedType);
                    ROSE_ASSERT(symbolOfIntendedType != NULL);

                    SgType* intendedType = symbolOfIntendedType->get_type();
                    ROSE_ASSERT(intendedType != NULL);

                    parentType->reset_base_type(intendedType);
                  }
             }
        }
#if 0
     printf ("Leaving fixup_forward_type_declarations() \n");
#endif
   }


void 
processAttributeSpecStack(bool hasArraySpec, bool hasInitialization)
   {
  // DQ (1/16/2011): Now is the latest point where we can process the type.  We have now seen all of the
  // attributes and they are on the astAttributeSpecStack stack (which we can use to construct the
  // correct base type (from the current base type).  Note that if there is an initializer for the
  // variable it will be on top of the stack.  so this might be a better thing to handle AFTER the
  // initializer has been consumed...
     vector<int> savedAttributes;
  // printf ("In processAttributeSpecStack(): astAttributeSpecStack.size() = %zu \n",astAttributeSpecStack.size());

     std::list<int>::iterator i = astAttributeSpecStack.begin();
  // while (astAttributeSpecStack.empty() == false)
     while (i != astAttributeSpecStack.end())
        {
       // Some attributes that are not order dependent (are there any in Fortran) should maybe we saved and put back on the stack.

#if 1
       // Output debugging information about saved state (stack) information.
          outputState("In loop over attributes in processAttributeSpecStack()");
#endif

       // int attr = astAttributeSpecStack.front();
          int attr = *i;
       // printf ("attribute spec on stack = %d \n",attr);

       // Note that we need to handle just those attributes that effect the base type of the declaration.
       // But since all attributes are stored on the stack we have to pop them all off, find the relevant 
       // ones that effect the type, and then rebuild the stack.
          switch(attr)
             {
            // DQ (1/20/2011): Added case of ComponentAttrSpec_dimension
               case AttrSpec_DIMENSION:
               case ComponentAttrSpec_dimension:
                  {
                 // Handle astAttributeSpecStack.push_front(AttrSpec_DIMENSION); done previously.
                    if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                         printf ("found a DIMENSION spec \n");

                    ROSE_ASSERT(astBaseTypeStack.empty() == false);
                 // printf ("Here is where we need to build an array (where the base type has finally be correctly computed = %s \n",astBaseTypeStack.front()->class_name().c_str());

                 // At this point the array type is already built, but is built using the base type, also it is on the astTypeStack, and not the astBaseTypeStack.
                 // so we move it to the astBaseTypeStack stack.

                 // We don't know the count yet, not clear where we get that (assume 1 for initial test).
                 // Note that this parameter is no longer used, so it should be removed!
                 // printf ("REMOVE USE OF count PARAMETER IN convertTypeOnStackToArrayType() \n");

                 // If the array spec is a part of the multi-part reference then process it as such and build the type on the astTypeStack (specific for that variable), else build it for the astBaseTypeStack for all variables.
                 // if (hasArraySpec == true)
                 // printf ("In processAttributeSpecStack(): hasArraySpec = %s hasInitialization = %s \n",hasArraySpec ? "true" : "false", hasInitialization ? "true" : "false");

                 // DQ (1/22/2011): Not clear now to handle hasInitialization...
                 // if (hasArraySpec == true || hasInitialization == false)
                 // if (hasArraySpec == true)
                 // if (astTypeStack.empty() == true || hasArraySpec == true)

                    if (hasInitialization == false)
                       {
                         if ((astTypeStack.empty() == true) || (hasArraySpec == true) ) // This is a problem for test2011_16.f90
                            {
                              if (hasArraySpec == true)
                                 {
                                // DQ (1/18/2011): 
                                   int count = 1;
                                   SgArrayType* arrayType = convertTypeOnStackToArrayType(count);
                                   ROSE_ASSERT(arrayType != NULL);

                                // DQ (1/17/2011): Fixup the call to convertTypeOnStackToArrayType() to prepare a value on the correct type stack. See test2007_101.f90 for an example.
                                   ROSE_ASSERT(astBaseTypeStack.empty() == false);

                                // This is false for test2011_23.f90 ("character :: A(2)*7)") but true nearly everywhere else...
                                // ROSE_ASSERT(astTypeStack.empty() == true);
                                   if (astTypeStack.empty() == false)
                                      {
                                     // When multiple specifiers are used (e.g. arrays of strings) to construct non-base types we
                                     // do so in place at the front of the astTypeStack.  There should be at most a single entry.
                                     // printf ("Handling case of declarations such as: character :: A(2)*7) \n");
                                        astTypeStack.pop_front();
                                      }
                                   ROSE_ASSERT(astTypeStack.empty() == true);
                                   
                                   astTypeStack.push_front(arrayType);
                                 }
                                else
                                 {
                                // We want to use the type in astTypeStack, so push the base type from astBaseTypeStack
                                   ROSE_ASSERT(astTypeStack.empty() == true);
                                // ROSE_ASSERT(astBaseTypeStack.empty() == true);

                                   if (astExpressionStack.empty() == false)
                                      {
                                        int count = 1;
                                        SgArrayType* arrayType = convertTypeOnStackToArrayType(count);
                                        ROSE_ASSERT(arrayType != NULL);
                                        astBaseTypeStack.pop_front();
                                        astBaseTypeStack.push_front(arrayType);
                                        astTypeStack.push_front(astBaseTypeStack.front());
                                      }
                                     else
                                      {
                                     // See test2011_07.f90 for an example of why this is required.
                                        astTypeStack.push_front(astBaseTypeStack.front());
                                      }
                                 }

                           // printf ("Error, we want to build or modify only the the type in the astTypeStack and not touch the type in the astBaseTypeStack \n");
                           // ROSE_ASSERT(false);
                            }
                           else
                            {
                           // Nothing do do here since the array type was processed previously!
                              ROSE_ASSERT(astBaseTypeStack.empty() == false);
                           // ROSE_ASSERT(astTypeStack.empty() == false);
                           // ROSE_ASSERT(astTypeStack.empty() == true);
                            }
                       }
                      else
                       {
                      // This is the case of test2011_16.f90
                         ROSE_ASSERT(hasInitialization == true);

                      // DQ (1/23/2011): In test2011_19.f90 ("integer, dimension(2,kmax0) :: X(2,kmax1) = reshape( (/5,6/), (/2,kmax2/) )")
                      // since hasInitialization is true when processing X and now with R469 list c_action_ac_value_list__begin() now
                      // processing array types (e.g. for X) using convertTypeOnStackToArrayType() we don't want to process it here.

                         if (hasArraySpec == true)
                            {
                           // printf ("############ Sometimes we WANT to process this into an array ############## \n");

                           // DQ (1/23/2011): If hasInitialization == true then the initializer is on the top of the stack and the array bound expression is beneath it.
                           // Save the initializer and put it back on the stack after we process the array expression.
                              ROSE_ASSERT(astExpressionStack.empty() == false);
                              SgExpression* initializer = astExpressionStack.front();
                              astExpressionStack.pop_front();

                           // This is code need to handle test2011_16.f90 (but might make some other code fail, e.g. test2010_136.f90 and test2011_19.f90)!
                              SgExprListExp* exprListExp = astExpressionStack.empty() == false ? isSgExprListExp(astExpressionStack.front()) : NULL;
                              if (exprListExp != NULL)
                                 {
                                   int count = 1;
                                   ROSE_ASSERT(astExpressionStack.empty() == false);
                                   SgArrayType* arrayType = convertTypeOnStackToArrayType(count);
                                   ROSE_ASSERT(arrayType != NULL);

                                // DQ (1/17/2011): Fixup the call to convertTypeOnStackToArrayType() to prepare a value on the correct type stack. See test2007_101.f90 for an example.
                                   ROSE_ASSERT(astBaseTypeStack.empty() == false);
                                   ROSE_ASSERT(astTypeStack.empty() == true);

                                   astTypeStack.push_front(arrayType);
                                 }
                                else
                                 {
                                // DQ (1/23/2011): This is the case for test2011_19.f90 and test2010_136.f90
                                // printf ("This array attribute has previously been turned into an array, so we don't have to worry about it... \n");
                                   ROSE_ASSERT(astTypeStack.empty() == false);
                                   ROSE_ASSERT(isSgArrayType(astTypeStack.front()) != NULL);
                                 }

                           // Put the initializer back
                              astExpressionStack.push_front(initializer);
                            }
                           else
                            {
                           // printf ("############ Sometimes we DON'T want to process this into an array ############## \n");
                            }
                         ROSE_ASSERT(astExpressionStack.empty() == false);
                       }
#if 1
                 // Output debugging information about saved state (stack) information.
                    outputState("In processAttributeSpecStack(): After processing type for AttrSpec_DIMENSION");
#endif
                 // Increment as many times as required past this dimension case.
                    while ((*i == AttrSpec_DIMENSION || *i == ComponentAttrSpec_dimension) )
                       {
                         i++;
                       }
#if 1
                 // Output debugging information about saved state (stack) information.
                    outputState("After processing AttrSpec_DIMENSION in loop over attributes in R504 R503-F2008 c_action_entity_decl()");
#endif
                    break;
                  }

            // DQ (1/20/2011): Added case of ComponentAttrSpec_pointer
               case AttrSpec_POINTER:
               case ComponentAttrSpec_pointer:
                  {
                    if ( SgProject::get_verbose() > DEBUG_COMMENT_LEVEL )
                         printf ("found a POINTER spec \n");

                    if (astTypeStack.empty() == false)
                       {
                      // Because the pointer attribute specification applies to the base type we have to force this to be applied to the base type.
                      // So clear the astTypeStack so that the convertBaseTypeOnStackToPointer() will leave the generate type on the astBaseTypeStack.
                      // Since the convertBaseTypeOnStackToPointer() is only called from this function we could change the semantics of that function
                      // to include this semantics.
                      // printf ("Clear the type on the astTypeStack so that it can be recreated! \n");
                         astTypeStack.pop_front();
                       }

                 // DQ (2/1/2009): Change the type on the astBaseTypeStack 
                 // to be a pointer with that base type. This attribute 
                 // really should have an immediate effect.
                    convertBaseTypeOnStackToPointer();

                 // Increment once past this pointer case.
                    i++;

                 // astAttributeSpecStack.pop_front();
                    break;
                  }

               default:
                  {
                 // This is really an error because it means that we are ignoring all of attributes except 
                 // for the specific cases handled above. so we have to save these attributes and use them
                 // to rebuild the stack without the attributes processed above.  This is because some
                 // attributes effect the type and some are just attributes to the save in the declaration.

                 // savedAttributes.push_back(attr);
                 // astAttributeSpecStack.pop_front();

                 // Increment once past this default case.
                    i++;
                  }
             }

       // DQ (1/16/2011): We can't cal this since we don't yet have a declaration to use.
       // So we will have to save the entries that we don't process in the stack and push 
       // those back onto the stack so that the variable declaration can be set properly later.
       // setDeclarationAttributeSpec(variableDeclaration,astAttributeSpecStack.front());

       // astAttributeSpecStack.pop_front();

       // printf ("At bottom of loop over the attrubutes (next attribute = %d = %s ) \n",i != astAttributeSpecStack.end() ? *i : -1,i != astAttributeSpecStack.end() ? "valid" : "end of list");

#if 1
       // Output debugging information about saved state (stack) information.
          outputState("In BOTTOM of loop over attributes in processAttributeSpecStack()");
#endif
       // i++;
        }

  // DQ (1/21/2011): Handling code such as "integer :: a(5), b(7)"
  // if (hasArraySpec == true)
        {
       // If this is a declarations such as "integer :: a(5)" then "hasArraySpec" is TRUE and we want to
       // remove the dimension attribute so that an additional veriable will use either the correct base 
       // type or a new dimension attribute (and associated array length expression).

       // Remove the dimension spec AND any pointer!!!
          std::vector<int> savedAttributes;
          std::list<int>::iterator j = astAttributeSpecStack.begin();
          while (j != astAttributeSpecStack.end())
             {
            // DQ (1/22/2011): The base type has already been computed and it will have include both pointer 
            // and array concepts already. We will not want to repeat these in another array type used for 
            // subsequent variables.
            // if ( (*j != AttrSpec_DIMENSION) && (*j != ComponentAttrSpec_dimension) )
               if ( (*j != AttrSpec_DIMENSION) && (*j != ComponentAttrSpec_dimension) &&
                    (*j != AttrSpec_POINTER)   && (*j != ComponentAttrSpec_pointer) )
                  {
                 // printf ("Save the attribute = %d \n",*j);
                    savedAttributes.push_back(*j);
                  }
                 else
                  {
                 // printf ("Found a dimension attribute \n");
                 // printf ("Found a dimension or pointer attribute (already incorporated into the base type) \n");
                  }

               j++;
             }

       // Clear the astAttributeSpecStack, so that we avoid redundant entries when we put back 
       // the saved entries.  See test2011_11.f90 for an example of where this is required.
       // astAttributeSpecStack.clear();
          while (astAttributeSpecStack.empty() == false)
             {
               astAttributeSpecStack.pop_front();
             }
          ROSE_ASSERT(astAttributeSpecStack.empty() == true);

       // Replace the saved attributes on the stack (preserve the order as in the original stack).
          vector<int>::reverse_iterator k = savedAttributes.rbegin();
          while (k != savedAttributes.rend())
             {
            // printf ("Adding *k = %d saved from first trip back onto the astAttributeSpecStack \n",*k);
               astAttributeSpecStack.push_front(*k);
               k++;
             }
        }

  // printf ("Leaving processAttributeSpecStack(): astAttributeSpecStack.size() = %zu \n",astAttributeSpecStack.size());
   }


void
processMultidimensionalSubscriptsIntoExpressionList(int count)
   {
  // DQ (1/18/2011): Refactored this code so it could be called in R510 and R443.

  // printf ("In processMultidimensionalSubscriptsIntoExpressionList( count = %d ) \n",count);

  // DQ (1/17/2011): Moved the code to build the array index expression list from convertTypeOnStackToArrayType().
     SgExprListExp* expresssionList = new SgExprListExp();
  // arrayType->set_dim_info(expresssionList);
     setSourcePosition(expresssionList);
  // expresssionList->set_parent(arrayType);

     for (int i=0; i < count; i++)
        {
          ROSE_ASSERT(astExpressionStack.empty() == false);
       // printf ("Adding an expression to the array type dimension information = %s \n",SageInterface::get_name(astExpressionStack.front()).c_str());

       // ROSE_ASSERT(arrayType->get_dim_info() != NULL);
       // arrayType->get_dim_info()->prepend_expression(astExpressionStack.front());
          expresssionList->prepend_expression(astExpressionStack.front());
          astExpressionStack.pop_front();
        }

  // Put the SgExprListExp onto the stack
     astExpressionStack.push_front(expresssionList);
   }



void
convertBaseTypeToArrayWhereAppropriate()
   {
  // This is the latest point for building the base type to be used in the declaration of multiple variables.
  // DQ (1/20/2011): Refactored the code below so it could be called from R443 as well as R504.
     bool hasArraySpec      = false;
     bool hasInitialization = false;
  // printf ("In convertBaseTypeToArrayWhereAppropriate (called by R504 (list__begin) c_action_entity_decl_list__begin()) calling processAttributeSpecStack(false,false): astAttributeSpecStack.size() = %zu \n",astAttributeSpecStack.size());
     processAttributeSpecStack(hasArraySpec,hasInitialization);

  // Note that if there are array or pointer attributes to process then processAttributeSpecStack() 
  // sets up the astBaseTypeStack and astTypeStack, and we don't want an entry on the astTypeStack.
  // ROSE_ASSERT(astTypeStack.empty() == false);
     if (astTypeStack.empty() == false)
        {
       // printf ("In convertBaseTypeToArrayWhereAppropriate (called by R504 (list__begin) c_action_entity_decl_list__begin()): Clearing the top entry on the astTypeStack \n");
          astTypeStack.pop_front();
          ROSE_ASSERT(astTypeStack.empty() == true);
        }
   }
