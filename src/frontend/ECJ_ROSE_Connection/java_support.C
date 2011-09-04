#include "sage3basic.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

// Java support for calling C/C++.
#include <jni.h>

// Support functions declaration of function defined in this file.
#include "java_support.h"
#include "jni_token.h"

using namespace std;

#if 0
// DQ (8/15/2011): Moved to openJavaParser_main.C to
// separate the work on Java from the rest of ROSE and support the ROSE
// configuration language only options.
// DQ (10/21/2010): If Fortran is being supported then there will
// be a definition of this pointer there.  Note that we currently
// use only one pointer so that we can simplify how the JVM support 
// is used for either Fortran or Java language support.
#ifndef ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
SgSourceFile* OpenFortranParser_globalFilePointer = NULL;
#endif
#endif

// DQ (8/15/2011): This declaration was moved to openJavaParser_main.C to
// separate the work on Java from the rest of ROSE and support the ROSE
// configuration language only options.
// Global stack of scopes
// list<SgScopeStatement*> astJavaScopeStack;
extern list<SgScopeStatement*> astJavaScopeStack;

// Global stack of expressions 
list<SgExpression*> astJavaExpressionStack;

// Global stack of types
list<SgType*> astJavaTypeStack;

// Global stack of statements
list<SgStatement*> astJavaStatementStack;

// Simplifying type for the setSourcePosition() functions
// typedef std::vector<Token_t*> TokenListType;

// Global stack of IR nodes
list<SgNode*> astJavaNodeStack;

// Attribute spec for holding attributes
// std::list<int> astAttributeSpecStack;

// Global list of implicit classes
list<SgName> astJavaImplicitClassList;

// Global stack of SgInitializedName IR nodes (used for processing function parameters)
list<SgInitializedName*> astJavaInitializedNameStack;

// Global stack of source code positions. The advantage of a stack is that we could 
// always reference the top of the stack, and monitor the depth of the stack, and make
// sure that we never deleted the last entry in the stack until the end of the program.
list<JavaSourceCodePosition*> astJavaSourceCodePositionStack;

// Global stack of context used by the ast visitor to store information
// when visiting a node that may need to be remember on the end visit
list<VisitorContext *> astVisitorContextStack;



SgGlobal*
getGlobalScope()
   {
     ROSE_ASSERT(astJavaScopeStack.empty() == false);
     SgScopeStatement* bottomOfStack = astJavaScopeStack.back();
     ROSE_ASSERT(bottomOfStack != NULL);

     SgGlobal* globalScope = isSgGlobal(bottomOfStack);
     ROSE_ASSERT(globalScope != NULL);

     return globalScope;
   }

string
getCurrentJavaFilename()
   {
  // DQ (8/16/2011): Generate the filename of the current file being processed.
     string filename;

  // Look on the scope stack and trace through parent to find the SgSourceFile.
     ROSE_ASSERT(astJavaScopeStack.empty() == false);

     SgGlobal* globalScope = getGlobalScope();
     ROSE_ASSERT(globalScope != NULL);
     SgSourceFile* sourceFile = isSgSourceFile(globalScope->get_parent());
     ROSE_ASSERT(sourceFile != NULL);

     filename = sourceFile->getFileName();

     return filename;
   }

/*
 * Wrapper to create an Sg_File_Info from line/col info
 */
Sg_File_Info *
createSgFileInfo(int line, int col)
   {
  // Sg_File_Info* sg_fi = Sg_File_Info::generateDefaultFileInfo();
     Sg_File_Info* sg_fi = new Sg_File_Info(getCurrentJavaFilename(),line,col);
  // sg_fi->set_line(line);
  // sg_fi->set_col(col);

     ROSE_ASSERT(sg_fi->isTransformation()    == false);
     ROSE_ASSERT(sg_fi->isCompilerGenerated() == false);

     if (line == 0 && col == 0)
        {
          if (SgProject::get_verbose() > 2)
               printf ("Found source position info (line == 0 && col == 0) indicating compiler generated code \n");

          sg_fi->setCompilerGenerated();

       // sg_fi->display("Found source position info (line == 0 && col == 0) indicating compiler generated code");
        }

     return sg_fi;
   }

Token_t *
create_token(JNIEnv * env, jobject jToken)
   {
  // if (SgProject::get_verbose() > -1)
  //      printf ("Create C-based representation of a JavaToken\n");

     return convert_Java_token(env, jToken);
   }

/**
 * 1) Converts java source code position from java to c type
 * 2) Pushes position info on the astJavaSourceCodePositionStack
 * 3) Transfer java source code position information to the SgNode's Sg_FileInfo
 */
void
pushAndSetSourceCodePosition(JavaSourceCodePosition * pos, SgLocatedNode * sgnode)
   {
    // This function is called by the ECJ Java traversal and pushes a JavaSourceCodePosition object onto
    // the astJavaSourceCodePositionStack.  The processing of ROSE IR nodes uses and removes these objects
    // from the top of the stack.  The stack depth should never be more than 1, if everything is working.
    // But initally we can support a more flexible mechanism in cases wehre we are not catching all
    // the source code position date in ROSE that is generated by the ECJ traversal.  We can even
    // reuse the last (bottom) element of the stack repeatedly if we have not yet generated enough
    // calls to in the ECJ traversal.  Thus a stack could be a flexible way to support the source code
    // position information and tightening the constrains to have it always only be a small depth
    // (e.g. size == 1) would be possible in an iterative way as the implementation develops.

  // outputJavaState("At TOP of pushAndSetSourceCodePosition");
     ROSE_ASSERT(pos != NULL);

     if (SgProject::get_verbose() > 4)
        {
          printf ("Process the source position to set the source code position... \n");
          printf ("pos : line_start = %d line_end = %d col_start = %d col_end = %d \n",
               pos->getLineStart(),pos->getLineEnd(),pos->getColStart(),pos->getColEnd());
        }

  // DQ (8/14/2011): Skipping push to stack since we have not implemented any support to pop from the stack.
  // Update the global source code position information. Or we could push it onto a stack...
  // The advantage of a stack is that we could always reference the top of the stack, and
  // monitor the depth of the stack, and make sure that we never deleted the last entry in
  // the stack until the end of the program.
  // astJavaSourceCodePositionStack.push_front(pos);

     setJavaSourcePosition(sgnode, pos);

  // Check if there are too many on the stack (ROSE processing should clear them as they are used).
     ROSE_ASSERT(astJavaSourceCodePositionStack.size() < 10);

  // outputJavaState("At BOTTOM of pushAndSetSourceCodePosition");
   }

void
setJavaSourcePosition( SgLocatedNode* locatedNode)
   {
     setJavaSourcePosition(locatedNode, (JavaSourceCodePosition*)NULL);
   }

void
setJavaSourcePosition( SgLocatedNode* locatedNode, JavaSourceCodePosition * posInfo)
   {
  // This function sets the source position if java position information has been provided
  // (posInfo != NULL), otherwise it is marked as not available.
  // These nodes WILL be unparsed in the code generation phase.

  // The SgLocatedNode has both a startOfConstruct and endOfConstruct source position.
     ROSE_ASSERT(locatedNode != NULL);

  // Make sure we never try to reset the source position of the global scope (set elsewhere in ROSE).
     ROSE_ASSERT(isSgGlobal(locatedNode) == NULL);

  // Check the endOfConstruct first since it is most likely NULL (helpful in debugging)
     if (locatedNode->get_endOfConstruct() != NULL || locatedNode->get_startOfConstruct() != NULL)
        {
          if (SgProject::get_verbose() > 1)
             {
               printf ("In setSourcePosition(SgLocatedNode* locatedNode): Warning about existing file info data at locatedNode = %p = %s \n",locatedNode,locatedNode->class_name().c_str());
             }

          if (locatedNode->get_startOfConstruct() != NULL)
             {
               delete locatedNode->get_startOfConstruct();
               locatedNode->set_startOfConstruct(NULL);
             }

          if (locatedNode->get_endOfConstruct() != NULL)
             {
               delete locatedNode->get_endOfConstruct();
               locatedNode->set_endOfConstruct(NULL);
             }

        }

  // DQ (8/16/2011): Added support for setting the operator source code position 
  // (Note that for expressions get_file_info() returns get_operatorPosition()).
     SgExpression* expression = isSgExpression(locatedNode);
     if (expression != NULL)
        {
          if (expression->get_operatorPosition() != NULL)
             {
               delete expression->get_operatorPosition();
               expression->set_operatorPosition(NULL);
             }
        }

     if (posInfo == NULL)
        {
       // Call a mechanism defined in the SageInterface support

          printf ("ERROR: JavaSourceCodePosition * posInfo == NULL triggering use of SageInterface::setSourcePosition() (locatedNode = %p = %s) \n",locatedNode,locatedNode->class_name().c_str());
          ROSE_ASSERT(false);

          SageInterface::setSourcePosition(locatedNode);
        } 
       else
        {
       // java position info is available
          Sg_File_Info* start_fileInfo = createSgFileInfo(posInfo->getLineStart(), posInfo->getColStart());
          Sg_File_Info* end_fileInfo   = createSgFileInfo(posInfo->getLineEnd(), posInfo->getColEnd());

          ROSE_ASSERT(start_fileInfo->isTransformation() == false);
          ROSE_ASSERT(end_fileInfo->isTransformation() == false);

       // updating the sgnode
          locatedNode->set_startOfConstruct(start_fileInfo);
          locatedNode->set_endOfConstruct(end_fileInfo);

          ROSE_ASSERT(start_fileInfo->isTransformation() == false);
          ROSE_ASSERT(end_fileInfo->isTransformation() == false);

          ROSE_ASSERT(locatedNode->get_startOfConstruct()->isTransformation() == false);
          ROSE_ASSERT(locatedNode->get_endOfConstruct()->isTransformation() == false);

       // DQ (8/16/2011): Added support for setting the operator source code position 
       // (Note that for expressions get_file_info() returns get_operatorPosition()).
       // SgExpression* expression = isSgExpression(locatedNode);
          if (expression != NULL)
             {
               Sg_File_Info* operator_fileInfo = createSgFileInfo(posInfo->getLineStart(), posInfo->getColStart());
               expression->set_operatorPosition(operator_fileInfo);

               ROSE_ASSERT(locatedNode->get_file_info()->isTransformation() == false);
             }

          ROSE_ASSERT(locatedNode->get_file_info()->isTransformation() == false);
        }

     ROSE_ASSERT(locatedNode->get_file_info()->isTransformation() == false);
     ROSE_ASSERT(locatedNode->get_startOfConstruct()->isTransformation() == false);
     ROSE_ASSERT(locatedNode->get_endOfConstruct()->isTransformation() == false);
   }


void
setJavaSourcePosition( SgLocatedNode* locatedNode, JNIEnv *env, jobject jToken)
   {
     Token_t* token = create_token(env, jToken);

     ROSE_ASSERT(locatedNode != NULL);
     pushAndSetSourceCodePosition(token->getSourcecodePosition(), locatedNode);

  // printf ("In setJavaSourcePosition(): locatedNode->get_file_info()->isTransformation() = %s \n",locatedNode->get_file_info()->isTransformation() ? "true" : "false");
     ROSE_ASSERT(locatedNode->get_file_info()->isTransformation() == false);
   }


void
setJavaSourcePositionUnavailableInFrontend( SgLocatedNode* locatedNode )
   {
     ROSE_ASSERT(locatedNode != NULL);
     ROSE_ASSERT(locatedNode->get_startOfConstruct() != NULL);
     ROSE_ASSERT(locatedNode->get_endOfConstruct()   != NULL);

  // This is redundant for non-expression IR nodes.
     ROSE_ASSERT(locatedNode->get_file_info() != NULL);

     locatedNode->get_startOfConstruct()->setSourcePositionUnavailableInFrontend();
     locatedNode->get_endOfConstruct()->setSourcePositionUnavailableInFrontend();

     locatedNode->get_startOfConstruct()->setOutputInCodeGeneration();
     locatedNode->get_endOfConstruct()->setOutputInCodeGeneration();

     locatedNode->get_startOfConstruct()->unsetTransformation();
     locatedNode->get_endOfConstruct()->unsetTransformation();

     locatedNode->get_startOfConstruct()->unsetCompilerGenerated();
     locatedNode->get_endOfConstruct()->unsetCompilerGenerated();

  // DQ (8/16/2011): Added support for setting the operator source code position 
  // (Note that for expressions get_file_info() returns get_operatorPosition()).
     SgExpression* expression = isSgExpression(locatedNode);
     if (expression != NULL)
        {
          ROSE_ASSERT(expression->get_operatorPosition() != NULL);
          expression->get_operatorPosition()->setSourcePositionUnavailableInFrontend();
          expression->get_operatorPosition()->setOutputInCodeGeneration();

          expression->get_operatorPosition()->unsetTransformation();
          expression->get_operatorPosition()->unsetCompilerGenerated();
        }
   }

void
setJavaCompilerGenerated( SgLocatedNode* locatedNode )
   {
     ROSE_ASSERT(locatedNode != NULL);
     ROSE_ASSERT(locatedNode->get_startOfConstruct() != NULL);
     ROSE_ASSERT(locatedNode->get_endOfConstruct()   != NULL);

  // This is redundant for non-expression IR nodes.
     ROSE_ASSERT(locatedNode->get_file_info() != NULL);

     locatedNode->get_startOfConstruct()->unsetOutputInCodeGeneration();
     locatedNode->get_endOfConstruct()->unsetOutputInCodeGeneration();

     locatedNode->get_startOfConstruct()->setCompilerGenerated();
     locatedNode->get_endOfConstruct()->setCompilerGenerated();

     locatedNode->get_startOfConstruct()->unsetTransformation();
     locatedNode->get_endOfConstruct()->unsetTransformation();

     locatedNode->get_startOfConstruct()->unsetSourcePositionUnavailableInFrontend();
     locatedNode->get_endOfConstruct()->unsetSourcePositionUnavailableInFrontend();

  // DQ (8/16/2011): Added support for setting the operator source code position 
  // (Note that for expressions get_file_info() returns get_operatorPosition()).
     SgExpression* expression = isSgExpression(locatedNode);
     if (expression != NULL)
        {
          ROSE_ASSERT(expression->get_operatorPosition() != NULL);
          expression->get_operatorPosition()->setCompilerGenerated();

          expression->get_operatorPosition()->unsetOutputInCodeGeneration();
          expression->get_operatorPosition()->unsetTransformation();
          expression->get_operatorPosition()->unsetSourcePositionUnavailableInFrontend();
        }
   }

void
setJavaFrontendSpecific( SgLocatedNode* locatedNode )
   {
     ROSE_ASSERT(locatedNode != NULL);
     ROSE_ASSERT(locatedNode->get_startOfConstruct() != NULL);
     ROSE_ASSERT(locatedNode->get_endOfConstruct()   != NULL);

#if 0
  // These IR nodes (marked this way) will not be visualized.
  // So we want to be able to alternatively mark the to be
  // visualized for some debugging.

  // This is redundant for non-expression IR nodes.
     ROSE_ASSERT(locatedNode->get_file_info() != NULL);

     locatedNode->get_startOfConstruct()->setFrontendSpecific();
     locatedNode->get_endOfConstruct()->setFrontendSpecific();

     locatedNode->get_startOfConstruct()->unsetOutputInCodeGeneration();
     locatedNode->get_endOfConstruct()->unsetOutputInCodeGeneration();

     locatedNode->get_startOfConstruct()->unsetCompilerGenerated();
     locatedNode->get_endOfConstruct()->unsetCompilerGenerated();

     locatedNode->get_startOfConstruct()->unsetTransformation();
     locatedNode->get_endOfConstruct()->unsetTransformation();

     locatedNode->get_startOfConstruct()->unsetSourcePositionUnavailableInFrontend();
     locatedNode->get_endOfConstruct()->unsetSourcePositionUnavailableInFrontend();

  // DQ (8/16/2011): Added support for setting the operator source code position 
  // (Note that for expressions get_file_info() returns get_operatorPosition()).
     SgExpression* expression = isSgExpression(locatedNode);
     if (expression != NULL)
        {
          ROSE_ASSERT(expression->get_operatorPosition() != NULL);
          expression->get_operatorPosition()->setFrontendSpecific();

          expression->get_operatorPosition()->unsetOutputInCodeGeneration();
          expression->get_operatorPosition()->unsetCompilerGenerated();
          expression->get_operatorPosition()->unsetTransformation();
          expression->get_operatorPosition()->unsetSourcePositionUnavailableInFrontend();
        }
#else
  // This will cause implicit classes (Java specific) to be marks to permit visualization.
     setJavaCompilerGenerated(locatedNode);
#endif

  // locatedNode->get_startOfConstruct()->display("In setJavaFrontendSpecific():debug");
   }

VisitorContext * getCurrentContext() {
        return astVisitorContextStack.front();
}

void pushContextStack(VisitorContext * ctx) {
        printf("KK Pushing\n");
        astVisitorContextStack.push_front(ctx);
}

VisitorContext * popContextStack() {
        printf("KK Pop\n");
        VisitorContext * ctx = getCurrentContext();
        ROSE_ASSERT(ctx != NULL);
        astVisitorContextStack.pop_front();
        return ctx;
}

bool isStatementContext(VisitorContext * ctx) {
        // This is just to make sure that the call fails
        // if we add a new type of visitor context but don't
        // update this function
        ROSE_ASSERT((dynamic_cast<CallVisitorContext*>(ctx) != NULL) ||
                        (dynamic_cast<IfVisitorContext*>(ctx) != NULL) ||
                        (dynamic_cast<MethodVisitorContext*>(ctx) != NULL) ||
                        (dynamic_cast<BlockVisitorContext*>(ctx) != NULL));
        // NOTE: we could store the type as an enum
        // to avoid the dynamic_cast call
        return (dynamic_cast<CallVisitorContext*>(ctx) == NULL);
}

#if 0
// DQ (8/15/2011): These function were moved to openJavaParser_main.C to
// separate the work on Java from the rest of ROSE and support the ROSE
// configuration language only options.
bool
emptyJavaStateStack()
   {
  // Use the scope stack to indicate if we have a value scope available as part of Java lanaguage processing.
     return astJavaScopeStack.empty();
   }


SgScopeStatement*
getTopOfJavaScopeStack()
   {
     ROSE_ASSERT(astJavaScopeStack.empty() == false);
     SgScopeStatement* topOfStack = astJavaScopeStack.front();

     return topOfStack;
   }
#endif


void
outputJavaStateSupport( const std::string & s, int fieldWidth )
   {
     printf ("(%s)",s.c_str());
     for (int j=s.length(); j < fieldWidth; j++)
        {
          printf (" ");
        }
   }

void outputJavaState( const std::string label )
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

  // if ( SgProject::get_verbose() <= 3 )
     if ( SgProject::get_verbose() <= 3 && label.find("debug") == string::npos )
        {
       // Skip output of stack data for verbose levels less than or equal to 2
          return;
        }

     size_t maxStackSize = astJavaScopeStack.size();
     maxStackSize = astJavaStatementStack.size()       > maxStackSize ? astJavaStatementStack.size()       : maxStackSize;
     maxStackSize = astJavaExpressionStack.size()      > maxStackSize ? astJavaExpressionStack.size()      : maxStackSize;
     maxStackSize = astJavaTypeStack.size()            > maxStackSize ? astJavaTypeStack.size()            : maxStackSize;
     maxStackSize = astJavaInitializedNameStack.size() > maxStackSize ? astJavaInitializedNameStack.size() : maxStackSize;
     maxStackSize = astJavaNodeStack.size()            > maxStackSize ? astJavaNodeStack.size()            : maxStackSize;

     printf ("\n");
     printf ("\n");
     printf ("In outputState (%s): maxStackSize = %ld \n",label.c_str(),(long)maxStackSize);

     std::list<SgScopeStatement*>      ::reverse_iterator astScopeStack_iterator                = astJavaScopeStack.rbegin();
     std::list<SgStatement*>           ::reverse_iterator astStatementStack_iterator            = astJavaStatementStack.rbegin();
     std::list<SgExpression*>          ::reverse_iterator astExpressionStack_iterator           = astJavaExpressionStack.rbegin();
     std::list<SgType*>                ::reverse_iterator astTypeStack_iterator                 = astJavaTypeStack.rbegin();
     std::list<SgInitializedName*>     ::reverse_iterator astInitializedNameStack_iterator      = astJavaInitializedNameStack.rbegin();
     std::list<SgNode*>                ::reverse_iterator astNodeStack_iterator                 = astJavaNodeStack.rbegin();

     const int NumberOfStacks = 6;
     struct
        { std::string name;
          int fieldWidth;
     } stackNames[NumberOfStacks] = { {"astScopeStack", 40},   {"astStatementStack",50} ,    {"astExpressionStack",60} ,    {"astTypeStack",30} ,    {"astInitializedNameStack",30},   {"astNodeStack",30} };

     for (int k=0; k < NumberOfStacks; k++)
        {
          std::string s  = stackNames[k].name;
          int fieldWidth = stackNames[k].fieldWidth;
          outputJavaStateSupport(s,fieldWidth);
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
          if (astScopeStack_iterator != astJavaScopeStack.rend())
             {
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

          outputJavaStateSupport(s,stackNames[0].fieldWidth);




          if (astStatementStack_iterator != astJavaStatementStack.rend())
             {
               s = (*astStatementStack_iterator)->class_name() + " : " + SageInterface::get_name(*astStatementStack_iterator);

               astStatementStack_iterator++;
             }
            else
             {
               s = " No Statement ";
             }

          outputJavaStateSupport(s,stackNames[1].fieldWidth);




          if (astExpressionStack_iterator != astJavaExpressionStack.rend())
             {
               s = (*astExpressionStack_iterator)->class_name() + " : " + SageInterface::get_name(*astExpressionStack_iterator);

               astExpressionStack_iterator++;
             }
            else
             {
               s = " No Expression ";
             }

          outputJavaStateSupport(s,stackNames[2].fieldWidth);




          if (astTypeStack_iterator != astJavaTypeStack.rend())
             {
               s = (*astTypeStack_iterator)->class_name() + " : " + SageInterface::get_name(*astTypeStack_iterator);

               astTypeStack_iterator++;
             }
            else
             {
               s = " No Type ";
             }

          outputJavaStateSupport(s,stackNames[3].fieldWidth);




          if (astInitializedNameStack_iterator != astJavaInitializedNameStack.rend())
             {
            // Since this is the SgInitializedName stack we don't have to output the class_name for each list element.
            // s = (*astInitializedNameStack_iterator)->class_name() + " : " + SageInterface::get_name(*astInitializedNameStack_iterator);
               s = SageInterface::get_name(*astInitializedNameStack_iterator);

               astInitializedNameStack_iterator++;
             }
            else
             {
               s = " No Type ";
             }

          outputJavaStateSupport(s,stackNames[4].fieldWidth);




          if (astNodeStack_iterator != astJavaNodeStack.rend())
             {
               s = (*astNodeStack_iterator)->class_name() + " : " + SageInterface::get_name(*astNodeStack_iterator);

               astNodeStack_iterator++;
             }
            else
             {
               s = " No Node ";
             }

          outputJavaStateSupport(s,stackNames[5].fieldWidth);

          printf ("\n");
        }

     printf ("\n");
     printf ("\n");
   }



string
convertJavaStringToCxxString(JNIEnv *env, const jstring & java_string)
   {
  // Note that "env" can't be passed into this function as "const".
     const char* str = env->GetStringUTFChars(java_string, NULL);
     ROSE_ASSERT(str != NULL);

     string returnString = str;

  // printf ("Inside of convertJavaStringToCxxString s = %s \n",str);

  // Note that str is not set to NULL.
     env->ReleaseStringUTFChars(java_string, str);
     ROSE_ASSERT(str != NULL);

     return returnString;
   }

int
convertJavaIntegerToCxxInteger(JNIEnv *env, const jint & java_integer)
   {
  // The case of an integer is more trivial than I expected!

  // Note that "env" can't be passed into this function as "const".
  // const int returnValue = env->CallIntMethod(java_integer,NULL);
     const int returnValue = java_integer;

     return returnValue;
   }

bool
convertJavaBooleanToCxxBoolean(JNIEnv *env, const jboolean & java_boolean)
   {
  // Note that "env" can't be passed into this function as "const".
  // const unsigned char returnValueChar = env->CallBooleanMethod(java_boolean,NULL);
  // const bool returnValue = (returnValueChar != 0);

  // const bool returnValue = (bool) (env->CallBooleanMethod(java_boolean,NULL) == 1);
  // return returnValue;

     return false;
   }

// void memberFunctionSetup (SgName & name, SgClassDefinition* classDefinition, SgFunctionParameterList* & parameterlist, SgMemberFunctionType* & return_type)
void
memberFunctionSetup (SgName & name, SgClassDefinition* classDefinition, SgFunctionParameterList* & parameterlist, SgMemberFunctionType* & memberFunctionType)
   {
  // Refactored code.

  // This is abstracted so that we can build member functions as require to define Java specific default functions (e.g. super()).

     ROSE_ASSERT(classDefinition != NULL);
     ROSE_ASSERT(classDefinition->get_declaration() != NULL);

     if (SgProject::get_verbose() > 0)
          printf ("Inside of memberFunctionSetup(): name = %s in scope = %p = %s = %s \n",name.str(),classDefinition,classDefinition->class_name().c_str(),classDefinition->get_declaration()->get_name().str());

     SgFunctionParameterTypeList* typeList = SageBuilder::buildFunctionParameterTypeList();
     ROSE_ASSERT(typeList != NULL);

     ROSE_ASSERT(parameterlist == NULL);
     parameterlist = SageBuilder::buildFunctionParameterList();
     ROSE_ASSERT(parameterlist != NULL);

  // DQ (8/16/2011): Mark this as implicit (we may later find more source position infroamtion for this).
  // setJavaCompilerGenerated(parameterlist);
     setJavaSourcePositionUnavailableInFrontend(parameterlist);

     ROSE_ASSERT(astJavaTypeStack.empty() == false);

  // This is the return type for the member function (top of the stack).
     SgType* return_type = astJavaTypeStack.front();
     ROSE_ASSERT(return_type != NULL);

     astJavaTypeStack.pop_front();

  // Loop over the types in the astJavaTypeStack (the rest of the stack).
     while (astJavaInitializedNameStack.empty() == false)
        {
          SgInitializedName* initializedName = astJavaInitializedNameStack.front();
          ROSE_ASSERT(initializedName != NULL);

          setJavaSourcePositionUnavailableInFrontend(initializedName);

          SgType* parameterType = initializedName->get_type();
          ROSE_ASSERT(parameterType != NULL);

       // Note certain this is the correct order (we might need to insert instead of append).
          typeList->append_argument(parameterType);

          parameterlist->append_arg(initializedName);
          initializedName->set_parent(parameterlist);

          astJavaInitializedNameStack.pop_front();
        }

  // Specify if this is const, volatile, or restrict (0 implies normal member function).
     unsigned int mfunc_specifier = 0;
  // SgMemberFunctionType* memberFunctionType = SageBuilder::buildMemberFunctionType(SgTypeVoid::createType(), typeList, classDefinition, mfunc_specifier);
  // SgMemberFunctionType* memberFunctionType = SageBuilder::buildMemberFunctionType(return_type, typeList, classDefinition, mfunc_specifier);
     memberFunctionType = SageBuilder::buildMemberFunctionType(return_type, typeList, classDefinition, mfunc_specifier);
     ROSE_ASSERT(memberFunctionType != NULL);

  // parameterlist = SageBuilder::buildFunctionParameterList(typeList);
     ROSE_ASSERT(parameterlist != NULL);

  // SgFunctionType* func_type = SageBuilder::buildFunctionType(return_type,parameterlist);
     SgFunctionType* func_type = memberFunctionType;
     ROSE_ASSERT(func_type != NULL);

  // DQ (3/24/2011): Currently we am introducing a mechanism to make sure that overloaded function will have 
  // a unique name. It is temporary until we can handle correct mangled name support using the argument types.
     SgFunctionSymbol* func_symbol = NULL;
     bool func_symbol_found = true;
     while (func_symbol_found == true)
        {
       // DQ (3/24/2011): This function should not already exist (else it should be an error).
          func_symbol = classDefinition->lookup_function_symbol(name,func_type);
       // ROSE_ASSERT(func_symbol == NULL);

          if (func_symbol != NULL)
             {
               func_symbol_found = true;

            // This is a temporary mean to force overloaded functions to have unique names.
               name += "_overloaded_";
               if (SgProject::get_verbose() > 0)
                    printf ("Using a temporary mean to force overloaded functions to have unique names (name = %s) \n",name.str());
             }
            else
             {
               func_symbol_found = false;
             }
        }
   }

void
memberFunctionTest (const SgName & name, SgClassDefinition* classDefinition, SgMemberFunctionDeclaration* functionDeclaration)
   {
  // Refactored code.

     ROSE_ASSERT(functionDeclaration != NULL);

     size_t declarationListSize = classDefinition->generateStatementList().size();
     if (SgProject::get_verbose() > 0)
          printf ("declarationListSize = %zu \n",declarationListSize);
  // ROSE_ASSERT(declarationListSize > 0);
  // ROSE_ASSERT(declarationListSize == 0);

     if (SgProject::get_verbose() > 0)
          printf ("Test to make sure the SgFunctionSymbol is in the symbol table. \n");
     SgFunctionSymbol* memberFunctionSymbol = classDefinition->lookup_function_symbol(name);
     ROSE_ASSERT(memberFunctionSymbol != NULL);

  // DQ (3/24/2011): Added tests.
  // ROSE_ASSERT(classDefinition->get_symbol_from_symbol_table() != NULL);
     ROSE_ASSERT(classDefinition->get_declaration() != NULL);
     ROSE_ASSERT(classDefinition->get_declaration()->get_scope() != NULL);

  // DQ (3/24/2011): Unclear if this should be NULL, I think it should be NULL since only the nondefining declaration should have an associated symbol.
  // ROSE_ASSERT(classDefinition->get_declaration()->get_symbol_from_symbol_table() != NULL);
     ROSE_ASSERT(classDefinition->get_declaration()->get_symbol_from_symbol_table() == NULL);

     ROSE_ASSERT(classDefinition->get_declaration()->get_firstNondefiningDeclaration() != NULL);
     ROSE_ASSERT(classDefinition->get_declaration()->get_firstNondefiningDeclaration()->get_symbol_from_symbol_table() != NULL);
     ROSE_ASSERT(functionDeclaration->get_symbol_from_symbol_table() != NULL);

#if 0
  // DQ (8/16/2011): Mark this as implicit (compiler generated, to support the type system).
     setJavaCompilerGenerated(functionDeclaration);
     ROSE_ASSERT(functionDeclaration->get_parameterList() != NULL);
     setJavaCompilerGenerated(functionDeclaration->get_parameterList());
     ROSE_ASSERT(functionDeclaration->get_CtorInitializerList() != NULL);
     setJavaCompilerGenerated(functionDeclaration->get_CtorInitializerList());
#else
  // DQ (8/16/2011): Mark this as implicit (compiler generated, to support the type system).
     setJavaFrontendSpecific(functionDeclaration);
     ROSE_ASSERT(functionDeclaration->get_parameterList() != NULL);
     setJavaFrontendSpecific(functionDeclaration->get_parameterList());
     ROSE_ASSERT(functionDeclaration->get_CtorInitializerList() != NULL);
     setJavaFrontendSpecific(functionDeclaration->get_CtorInitializerList());
#endif
   }

SgMemberFunctionDeclaration*
buildNonDefiningMemberFunction(const SgName & inputName, SgClassDefinition* classDefinition)
   {
     SgName name = inputName;

     SgFunctionParameterList* parameterlist = NULL;
     SgMemberFunctionType* memberFunctionType = NULL;
  // SgType* return_type = NULL;

  // printf("Build non defining member function %s\n", inputName.str());
  // Refactored code.
     memberFunctionSetup (name,classDefinition,parameterlist,memberFunctionType);

     ROSE_ASSERT(parameterlist != NULL);
     ROSE_ASSERT(memberFunctionType != NULL);

  // SgMemberFunctionDeclaration* functionDeclaration = SageBuilder::buildNondefiningMemberFunctionDeclaration (name, return_type, parameterlist, classDefinition );
     SgMemberFunctionDeclaration* functionDeclaration = SageBuilder::buildNondefiningMemberFunctionDeclaration (name, memberFunctionType, parameterlist, classDefinition );

     ROSE_ASSERT(functionDeclaration != NULL);
     ROSE_ASSERT(functionDeclaration->get_definingDeclaration() == NULL);
     ROSE_ASSERT(functionDeclaration->get_definition() == NULL);

  // DQ (8/21/2011): We would like to have this end up on the statement stack and be appended at the end of the processing for the scope.
  // Add the member function to the class scope (this is a required second step after building the function declaration).
  // classDefinition->append_statement(functionDeclaration);

  // Refactored code.
     memberFunctionTest(name,classDefinition,functionDeclaration);

  // Push this statement onto the stack so that we can add arguments, etc.
  // astJavaStatementStack.push_front(functionDeclaration);

     return functionDeclaration;
   }

// SgMemberFunctionDeclaration* buildSimpleMemberFunction(const SgName & inputName, SgClassDefinition* classDefinition)
SgMemberFunctionDeclaration*
buildDefiningMemberFunction(const SgName & inputName, SgClassDefinition* classDefinition)
   {
  // This is abstracted so that we can build member functions as require to define Java specific default functions (e.g. super()).

     SgName name = inputName;

     SgFunctionParameterList* parameterlist = NULL;
     SgMemberFunctionType* memberFunctionType = NULL;

  // printf("Build defining member function %s\n", inputName.str());
  // Refactored code.
     memberFunctionSetup (name,classDefinition,parameterlist,memberFunctionType);

     ROSE_ASSERT(parameterlist != NULL);
     ROSE_ASSERT(memberFunctionType != NULL);

     SgMemberFunctionDeclaration* functionDeclaration = SageBuilder::buildDefiningMemberFunctionDeclaration (name, memberFunctionType, parameterlist, classDefinition );

     ROSE_ASSERT(functionDeclaration != NULL);
     ROSE_ASSERT(functionDeclaration->get_definingDeclaration() != NULL);
     ROSE_ASSERT(functionDeclaration->get_definition() != NULL);

  // DQ (8/21/2011): We would like to have this end up on the statement stack and be appended at the end of the processing for the scope.
  // Add the member function to the class scope (this is a required second step after building the function declaration).
  // classDefinition->append_statement(functionDeclaration);

  // Refactored code.
     memberFunctionTest(name,classDefinition,functionDeclaration);

  // Push this statement onto the stack so that we can add arguments, etc.
  // astJavaStatementStack.push_front(functionDeclaration);

     return functionDeclaration;
   }


SgClassDeclaration*
buildJavaClass (const SgName & className, SgScopeStatement* scope )
   {
  // This is a low level function to build the class specific to Java requirements 
  // (not very different from C++, but with a "super" function).

     ROSE_ASSERT(scope != NULL);

  // Note that this will also build the non-defining declaration.
     SgClassDeclaration* declaration = SageBuilder::buildDefiningClassDeclaration ( className, scope );
     ROSE_ASSERT(declaration != NULL);

     ROSE_ASSERT(declaration->get_type() != NULL);
     ROSE_ASSERT(declaration->get_scope() != NULL);

     ROSE_ASSERT(declaration->get_firstNondefiningDeclaration() != NULL);

  // DQ (3/24/2011): I think we should be able to assect this since the scope was valid.
     ROSE_ASSERT(declaration->get_symbol_from_symbol_table() == NULL);
     ROSE_ASSERT(declaration->get_firstNondefiningDeclaration()->get_symbol_from_symbol_table() != NULL);

  // Make sure that the new class has been added to the correct synbol table.
     ROSE_ASSERT (scope->lookup_class_symbol(declaration->get_name()) != NULL);

  // Set the source code position...
  // setSourcePosition(declaration);
  // setSourcePositionCompilerGenerated(declaration);

  // void setSourcePosition  ( SgLocatedNode* locatedNode );
  // void setSourcePositionCompilerGenerated( SgLocatedNode* locatedNode );

     ROSE_ASSERT(astJavaScopeStack.empty() == false);
     SgClassDefinition* classDefinition = SageBuilder::buildClassDefinition(declaration);
     ROSE_ASSERT(classDefinition != NULL);

  // Set the source code position...
  // setSourcePosition(definition);
  // setSourcePositionCompilerGenerated(definition);

  // This lowest level function should not touch the astJavaScopeStack!
  // astJavaScopeStack.push_front(definition);
  // ROSE_ASSERT(astJavaScopeStack.front()->get_parent() != NULL);

  // DQ (3/25/2011): Added testing.
     ROSE_ASSERT(classDefinition->get_declaration() == declaration);
     ROSE_ASSERT(classDefinition->get_declaration()->get_symbol_from_symbol_table() == NULL);
     ROSE_ASSERT(classDefinition->get_declaration()->get_firstNondefiningDeclaration()->get_symbol_from_symbol_table() != NULL);

#if 1
  // Ignore this requirement while we are debugging...

  // DQ (3/25/2011): Changed this to a non-defining declaration.
  // Add "super()" member function.

  // Push a dummy type to stand for the return type of the member function to be built.
  // This allows us to use a common member function support for constrcutors and the "super" function.
     SgTypeVoid* voidType = SgTypeVoid::createType();
     astJavaTypeStack.push_front(voidType);

  // SgMemberFunctionDeclaration* functionDeclaration = buildSimpleMemberFunction("super",classDefinition);
     SgMemberFunctionDeclaration* functionDeclaration = buildNonDefiningMemberFunction("super",classDefinition);
     ROSE_ASSERT(functionDeclaration != NULL);

#if 0
  // DQ (8/16/2011): Mark this as implicit (compiler generated, to support the type system).
     setJavaCompilerGenerated(functionDeclaration);
     ROSE_ASSERT(functionDeclaration->get_parameterList() != NULL);
     setJavaCompilerGenerated(functionDeclaration->get_parameterList());
     ROSE_ASSERT(functionDeclaration->get_CtorInitializerList() != NULL);
     setJavaCompilerGenerated(functionDeclaration->get_CtorInitializerList());
#else
     setJavaFrontendSpecific(functionDeclaration);
     ROSE_ASSERT(functionDeclaration->get_parameterList() != NULL);
     setJavaFrontendSpecific(functionDeclaration->get_parameterList());
     ROSE_ASSERT(functionDeclaration->get_CtorInitializerList() != NULL);
     setJavaFrontendSpecific(functionDeclaration->get_CtorInitializerList());
#endif

     size_t declarationListSize = classDefinition->generateStatementList().size();

     if (SgProject::get_verbose() > 0)
          printf ("declarationListSize = %zu \n",declarationListSize);
  // ROSE_ASSERT(declarationListSize > 0);
     ROSE_ASSERT(declarationListSize == 0);
#else
     if (SgProject::get_verbose() > 0)
          printf ("WARNING: Skipping addition of \"super\" member function for each class.\n");
#endif

     return declaration;
   }

void
buildClassSupport (const SgName & className, bool implicitClass, Token_t* token)
   {
  // This is mid level support for building a Java class. It is used by both the 
  // buildClass() and buildImplicitClass() functions.  This function calls the 
  // low level buildJavaClass() function to handle Java specific details and to
  // factor out the details similar to both normal java classes and implicit classes.

  // Note that if an implicit class is built, then all of the required classes to
  // support it being in the correct class hierarchy of classes (e.g. java.lang.xxx.yyy).
  // We may at some point use namespaces to supprt this, but this design point is not 
  // clear presently (namespaces are not really a part of Java).  I expect that we will
  // define something similar to a implicit use statement to support the java implicit
  // classes to be present in the global name space (using the alias symbol support in 
  // ROSE).  Though it is not clear if implicit Java function can appear without their
  // name qualification, so we will see.

  // Names of implicitly defined classes have names that start with "java." and these have to be translated.
     string original_classNameString = className.str();
     string classNameString = className.str();

  // Also replace '.' with '_'
     replace(classNameString.begin(), classNameString.end(),'.','_');

  // Also replace '$' with '_' (not clear on what '$' means yet (something related to inner and outer class nesting).
     replace(classNameString.begin(), classNameString.end(),'$','_');

     SgName name = classNameString;

  // We should not have a '.' in the class name.  Or it will fail the current ROSE name mangling tests.
     ROSE_ASSERT(classNameString.find('.') == string::npos);

  // DQ (3/20/2011): Detect use of '$' in class names. Current best reference 
  // is: http://www.java-forums.org/new-java/27577-specific-syntax-java-util-regex-pattern-node.html
     ROSE_ASSERT(classNameString.find('$') == string::npos);

     ROSE_ASSERT(astJavaScopeStack.empty() == false);

     if (SgProject::get_verbose() > 0)
        {
          printf ("In buildClassSupport(%s): astJavaScopeStack.front() = %p = %s \n",name.str(),astJavaScopeStack.front(),astJavaScopeStack.front()->class_name().c_str());
          printf ("   --- original_classNameString = %s \n",original_classNameString.c_str());
        }

     if (implicitClass == true)
        {
       // This branch makes a recursive call to this function (and computes the outerScope explicitly not using the astJavaScopeStack.
       // Note that we prepend the implicit classes since they should be defined before traversing the AST of the input program.
       // We also might want to mark the classes that are added as part of this implicit class support since it is Java specific.

       // Implicit classes are put into the global scope at the top of the scope.
          SgScopeStatement* outerScope = getGlobalScope();
          ROSE_ASSERT(outerScope != NULL);

       // Parse the original_classNameString to a list of what will be classes.
          size_t lastPosition = 0;
          size_t position = original_classNameString.find('.',lastPosition);
          while (position != string::npos)
             {
               string parentClassName = original_classNameString.substr(lastPosition,position-lastPosition);
               if (SgProject::get_verbose() > 0)
                    printf ("parentClassName = %s \n",parentClassName.c_str());

               lastPosition = position+1;
               position = original_classNameString.find('.',lastPosition);
               if (SgProject::get_verbose() > 0)
                    printf ("lastPosition = %zu position = %zu \n",lastPosition,position);

            // Build the "java" and the "lang" classes if they don't already exist.
               SgClassSymbol * classSymbol = outerScope->lookup_class_symbol(parentClassName);

               if (SgProject::get_verbose() > 0)
                    printf ("parentClassName = %s classSymbol = %p \n",parentClassName.c_str(),classSymbol);

               if (classSymbol == NULL)
                  {
                 // This parent class does not exist so add it (must be added to the correct scope).
                 // buildImplicitClass(parentClassName);
                    if (SgProject::get_verbose() > 0)
                         printf ("Building parent class = %s for implicit class = %s \n",parentClassName.c_str(),name.str());

                    outputJavaState("Building a new class (inside of loop)");

                 // At this point we could not find the class and so we will build one (just the class, not its definitions).
                 // However, we don't want to leave it on the astJavaScopeStack!
                    SgClassDeclaration* implicitDeclaration = buildJavaClass(parentClassName, outerScope );
                    ROSE_ASSERT(implicitDeclaration != NULL);
                    ROSE_ASSERT(implicitDeclaration->get_definition() != NULL);

                 // DQ (8/16/2011): Mark this as implicit (compiler generated, to support the type system).
#if 0
                    setJavaCompilerGenerated(implicitDeclaration);
                    ROSE_ASSERT(implicitDeclaration->get_definition() != NULL);
                    setJavaCompilerGenerated(implicitDeclaration->get_definition());
#else
                    setJavaFrontendSpecific(implicitDeclaration);
                    ROSE_ASSERT(implicitDeclaration->get_definition() != NULL);
                    setJavaFrontendSpecific(implicitDeclaration->get_definition());
#endif

                    outputJavaState("DONE: Building a new class (inside of loop)");

                    if (SgProject::get_verbose() > 0)
                         printf ("In buildClass(%s : parent of implicit class) after building the SgClassDeclaration: outerScope = %p = %s \n",parentClassName.c_str(),outerScope,outerScope->class_name().c_str());

                    SgClassDefinition* outerScopeClassDefinition = isSgClassDefinition(outerScope);

                    if (SgProject::get_verbose() > 0)
                         printf ("outerScopeClassDefinition = %p = %s \n",outerScopeClassDefinition,outerScopeClassDefinition == NULL ? "NULL" : outerScopeClassDefinition->get_declaration()->get_name().str());

                    outerScope->prepend_statement(implicitDeclaration);
                    ROSE_ASSERT(outerScope->generateStatementList().size() > 0);

                    ROSE_ASSERT(implicitDeclaration->get_parent() != NULL);
                 // declaration->set_parent(outerScope);

                 // DQ (3/25/2011): Make sure we are using the correct declaration to build the symbol.
                    ROSE_ASSERT(implicitDeclaration->get_definingDeclaration() == implicitDeclaration);
                    ROSE_ASSERT(implicitDeclaration->get_firstNondefiningDeclaration() != implicitDeclaration);

                    SgClassDeclaration* nonDefiningImplicitDeclaration = isSgClassDeclaration(implicitDeclaration->get_firstNondefiningDeclaration());
                    ROSE_ASSERT(nonDefiningImplicitDeclaration != NULL);

                 // DQ (3/25/2011): Make sure we are using the correct declaration to build the symbol.
                    ROSE_ASSERT(nonDefiningImplicitDeclaration->get_definingDeclaration() == implicitDeclaration);
                    ROSE_ASSERT(nonDefiningImplicitDeclaration->get_firstNondefiningDeclaration() != implicitDeclaration);

                    if (SgProject::get_verbose() > 0)
                         printf ("Build a new SgClassSymbol for parentClassName = %s and implicitDeclaration = %p = %s \n",parentClassName.c_str(),implicitDeclaration,implicitDeclaration->class_name().c_str());
                 // classSymbol = new SgClassSymbol(implicitDeclaration);
                    classSymbol = new SgClassSymbol(nonDefiningImplicitDeclaration);

                    if (SgProject::get_verbose() > 0)
                         printf ("Insert the new SgClassSymbol = %p into scope = %p = %s \n",classSymbol,outerScope,outerScope->class_name().c_str());
                    outerScope->insert_symbol(parentClassName,classSymbol);

                    if (SgProject::get_verbose() > 0)
                         printf ("Test to make sure the SgClassSymbol is in the symbol table. \n");
                    classSymbol = outerScope->lookup_class_symbol(parentClassName);
                    ROSE_ASSERT(classSymbol != NULL);
                  }
                 else
                  {
                 // Nothing to do here!
                  }

               ROSE_ASSERT(classSymbol != NULL);
               ROSE_ASSERT(classSymbol->get_declaration() != NULL);
               ROSE_ASSERT(classSymbol->get_declaration()->get_definingDeclaration() != NULL);
               ROSE_ASSERT(isSgClassDeclaration(classSymbol->get_declaration()->get_definingDeclaration()) != NULL);
               ROSE_ASSERT(isSgClassDeclaration(classSymbol->get_declaration()->get_definingDeclaration())->get_definition() != NULL);

               outerScope = isSgClassDeclaration(classSymbol->get_declaration()->get_definingDeclaration())->get_definition();
               ROSE_ASSERT(outerScope != NULL);
             }

          string className = original_classNameString.substr(lastPosition,position-lastPosition);

          if (SgProject::get_verbose() > 0)
               printf ("className for implicit (leaf) class = %s \n",className.c_str());

       // Reset the name for the most inner nested implicit class.  This allows a class such as "java.lang.System" 
       // to be build as "System" inside of "class "lang" inside of class "java" (without resetting the name we 
       // would have "java.lang.System" inside of "class "lang" inside of class "java").
          name = className;
#if 0
          printf ("Exiting after computing the className \n");
          ROSE_ASSERT(false);
#endif

       // DQ (4/15/2011): Since we build even implicit classes on demand, they might have already been built 
       // from a previous type reference and so we should check to make sure we don't build it again.
          SgClassSymbol * classSymbol = outerScope->lookup_class_symbol(className);

          if (SgProject::get_verbose() > 0)
               printf ("className = %s classSymbol = %p \n",className.c_str(),classSymbol);

          SgClassDeclaration* declaration = NULL;
          SgClassDeclaration* nonDefiningDeclaration = NULL;
          if (classSymbol == NULL)
             {
            // This parent class does not exist so add it (must be added to the correct scope).
            // buildImplicitClass(parentClassName);
               if (SgProject::get_verbose() > 0)
                    printf ("Building leaf class = %s for implicit class = %s \n",className.c_str(),name.str());

               outputJavaState("Building a new class (outside of loop)");

            // Here we build the class and associate it with the correct code (determined differently based on if this is an implicit or non-implicit class.
            // SgClassDeclaration* declaration = buildJavaClass(name, outerScope );
               declaration = buildJavaClass(name, outerScope );
               ROSE_ASSERT(declaration != NULL);

            // DQ (8/16/2011): Mark this as implicit (compiler generated, to support the type system).
#if 0
               setJavaCompilerGenerated(declaration);
               ROSE_ASSERT(declaration->get_definition() != NULL);
               setJavaCompilerGenerated(declaration->get_definition());
#else
               setJavaFrontendSpecific(declaration);
               ROSE_ASSERT(declaration->get_definition() != NULL);
               setJavaFrontendSpecific(declaration->get_definition());
#endif

               outputJavaState("DONE: Building a new class (outside of loop)");

               if (SgProject::get_verbose() > 0)
                    printf ("In buildClass(%s : implicit class) after building the SgClassDeclaration: outerscope = %p = %s \n",name.str(),outerScope,outerScope->class_name().c_str());

               SgClassDefinition* outerScopeClassDefinition = isSgClassDefinition(outerScope);

               if (SgProject::get_verbose() > 0)
                    printf ("outerScopeClassDefinition = %p = %s \n",outerScopeClassDefinition,outerScopeClassDefinition == NULL ? "NULL" : outerScopeClassDefinition->get_declaration()->get_name().str());

               outerScope->prepend_statement(declaration);
               ROSE_ASSERT(outerScope->generateStatementList().size() > 0);

            // DQ (3/25/2011): Make sure we are using the correct declaration to build the symbol.
               ROSE_ASSERT(declaration->get_definingDeclaration() == declaration);
               ROSE_ASSERT(declaration->get_firstNondefiningDeclaration() != declaration);

            // SgClassDeclaration* nonDefiningDeclaration = isSgClassDeclaration(declaration->get_firstNondefiningDeclaration());
               nonDefiningDeclaration = isSgClassDeclaration(declaration->get_firstNondefiningDeclaration());
               ROSE_ASSERT(nonDefiningDeclaration != NULL);

            // DQ (3/25/2011): Make sure we are using the correct declaration to build the symbol.
               ROSE_ASSERT(nonDefiningDeclaration->get_definingDeclaration() == declaration);
               ROSE_ASSERT(nonDefiningDeclaration->get_firstNondefiningDeclaration() != declaration);

               if (SgProject::get_verbose() > 0)
                    printf ("Build a new SgClassSymbol for parentClassName = %s and declaration = %p = %s \n",name.str(),declaration,declaration->class_name().c_str());
            // SgClassSymbol* classSymbol = new SgClassSymbol(declaration);
            // SgClassSymbol* classSymbol = new SgClassSymbol(nonDefiningDeclaration);
               classSymbol = new SgClassSymbol(nonDefiningDeclaration);

               if (SgProject::get_verbose() > 0)
                    printf ("Insert the new SgClassSymbol = %p into scope = %p = %s \n",classSymbol,outerScope,outerScope->class_name().c_str());

            // DQ (3/24/2011): Make sure there is not already a symbol for this class here already.
               ROSE_ASSERT(outerScope->symbol_exists(name,classSymbol) == false);

               outerScope->insert_symbol(name,classSymbol);

#if 0
            // DQ (9/4/2011): Moved this code to avoid leaving a declaration on the stack when 
            // this is a class that has been previously handled (see test2011_48.java).
               ROSE_ASSERT(declaration->get_definition() != NULL);
               astJavaScopeStack.push_front(declaration->get_definition());
#endif
             }
            else
             {
            // Classes that already exist were likely built previously due to a previous type reference.
               printf ("@@@@@@@@@@@@@@@ NOTE: this leaf class %s already exists... @@@@@@@@@@@@@@@@@\n",className.c_str());
            // nonDefiningDeclaration = isSgClassDeclaration(declaration->get_firstNondefiningDeclaration());
               nonDefiningDeclaration = classSymbol->get_declaration();
               ROSE_ASSERT(nonDefiningDeclaration != NULL);
               declaration = isSgClassDeclaration(nonDefiningDeclaration->get_definingDeclaration());
               ROSE_ASSERT(declaration != NULL);
             }

          if (SgProject::get_verbose() > 0)
               printf ("Test to make sure the SgClassSymbol is in the symbol table. \n");
       // classSymbol = outerScope->lookup_class_symbol(name);
       // ROSE_ASSERT(classSymbol != NULL);
          ROSE_ASSERT(outerScope->lookup_class_symbol(name) != NULL);

       // At this point we shuld still at least have the global scope on the stack.
          ROSE_ASSERT(astJavaScopeStack.empty() == false);

       // Note that this pushed only the new implicit class definition onto the stack 
       // and none of the parent class scopes. Is this going to be OK?
          ROSE_ASSERT(declaration->get_definition() != NULL);
#if 1
          astJavaScopeStack.push_front(declaration->get_definition());
#endif
          ROSE_ASSERT(astJavaScopeStack.front()->get_parent() != NULL);

          ROSE_ASSERT(declaration->get_parent() != NULL);

#if 0
       // DQ (4/14/2011): This is a test, if it works then we should not have pushed the scope onto the stack in the first place!
          astJavaScopeStack.pop_front();
          outputJavaState("In buildClassSupport(): Pop the class built implicitly from the stack.");
#endif

#if 0
          printf ("Exiting as a test of implicitClass handling \n");
          ROSE_ASSERT(false);
#endif
        }
       else
        {
       // Not implicit case: build the class and add it to the specified scope (append).
          SgScopeStatement* outerScope = astJavaScopeStack.front();

       // Here we build the class and associate it with the correct code (determined differently based on if this is an implicit or non-implicit class.
          SgClassDeclaration* declaration = buildJavaClass(name, outerScope );

       // Handle the token to support setting the source position.
          ROSE_ASSERT(token != NULL);
          pushAndSetSourceCodePosition(token->getSourcecodePosition(), declaration);

       // Add the class declaration to the current scope.  This might not be appropriate for implicit classes, but it helps in debugging the AST for now.
          ROSE_ASSERT(outerScope != NULL);

          if (SgProject::get_verbose() > 0)
               printf ("In buildClass(%s : non-implicit class) after building the SgClassDeclaration: astJavaScopeStack.front() = %p = %s \n",name.str(),outerScope,outerScope->class_name().c_str());

          SgClassDefinition* outerScopeClassDefinition = isSgClassDefinition(outerScope);

          if (SgProject::get_verbose() > 0)
               printf ("outerScopeClassDefinition = %p = %s \n",outerScopeClassDefinition,outerScopeClassDefinition == NULL ? "NULL" : outerScopeClassDefinition->get_declaration()->get_name().str());

       // DQ (8/21/2011): Fixup to be more bottom up now that we have better support (but set the parent explicitly).
       // outerScope->append_statement(declaration);
       // ROSE_ASSERT(outerScope->generateStatementList().size() > 0);
          declaration->set_parent(outerScope);

          ROSE_ASSERT(declaration->get_parent() != NULL);
       // declaration->set_parent(outerScope);

       // Make sure that the new class has been added to the correct synbol table.
          ROSE_ASSERT (outerScope->lookup_class_symbol(declaration->get_name()) != NULL);

       // If this is NOT an implicit class then use the class definition as the new current scope.
          ROSE_ASSERT(declaration->get_definition() != NULL);
          astJavaScopeStack.push_front(declaration->get_definition());
          ROSE_ASSERT(astJavaScopeStack.front()->get_parent() != NULL);

          ROSE_ASSERT(declaration->get_parent() != NULL);
        }
   }


void
buildImplicitClass (const SgName & className)
   {
     bool implicitClass = true;
     Token_t* token = NULL;
     buildClassSupport (className,implicitClass,token);
   }

void
buildClass (const SgName & className, Token_t* token)
   {
     bool implicitClass = false;
     buildClassSupport (className,implicitClass,token);
   }


SgVariableDeclaration*
buildSimpleVariableDeclaration(const SgName & name)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Building a variable (%s) within scope = %p = %s \n",name.str(),astJavaScopeStack.front(),astJavaScopeStack.front()->class_name().c_str());

  // SgType* type = SgTypeInt::createType();
     ROSE_ASSERT(astJavaTypeStack.empty() == false);
     SgType* type = astJavaTypeStack.front();

  // Not clear if we should pop this off at this point or in an alternative step.
     astJavaTypeStack.pop_front();

  // We are not supporting an initialized at this point in the implementation of the Java support.
     SgVariableDeclaration* variableDeclaration = SageBuilder::buildVariableDeclaration (name, type, NULL, astJavaScopeStack.front() );
     ROSE_ASSERT(variableDeclaration != NULL);

  // DQ (8/21/2011): Note that the default access permission is default, but this is the same enum value as public.
  // Most language support ignores this in the unparser, but we might want to set it better than this.

  // DQ (8/21/2011): Debugging declarations in local function should (should not be marked as public).
  // ROSE_ASSERT(variableDeclaration->get_declarationModifier().get_accessModifier().isPublic() == false);

  // DQ (7/16/2011): This is a test to debug failing test in resetParentPointers.C:1733
     ROSE_ASSERT(SageInterface::is_Fortran_language() == false);
     SgInitializedName* initializedName = variableDeclaration->get_decl_item (name);
     ROSE_ASSERT(initializedName != NULL);
     ROSE_ASSERT(initializedName->get_scope() != NULL);

     return variableDeclaration;
   }

list<SgName>
generateQualifierList (const SgName & classNameWithQualification)
   {
  // This function can be used to refactor the similar code in:
  //    void buildClassSupport (const SgName & className, bool implicitClass).

     list<SgName> returnList;
     SgName classNameWithoutQualification;

     classNameWithoutQualification = classNameWithQualification;

  // Names of implicitly defined classes have names that start with "java." and these have to be translated.
     string original_classNameString = classNameWithQualification.str();
     string classNameString = classNameWithQualification.str();

  // Also replace '.' with '_'
     replace(classNameString.begin(), classNameString.end(),'.','_');

  // Also replace '$' with '_' (not clear on what '$' means yet (something related to inner and outer class nesting).
     replace(classNameString.begin(), classNameString.end(),'$','_');

     SgName name = classNameString;

  // We should not have a '.' in the class name.  Or it will fail the current ROSE name mangling tests.
     ROSE_ASSERT(classNameString.find('.') == string::npos);

  // DQ (3/20/2011): Detect use of '$' in class names. Current best reference 
  // is: http://www.java-forums.org/new-java/27577-specific-syntax-java-util-regex-pattern-node.html
     ROSE_ASSERT(classNameString.find('$') == string::npos);

  // Parse the original_classNameString to a list of what will be classes.
     size_t lastPosition = 0;
     size_t position = original_classNameString.find('.',lastPosition);
     while (position != string::npos)
        {
          string parentClassName = original_classNameString.substr(lastPosition,position-lastPosition);
          if (SgProject::get_verbose() > 0)
               printf ("parentClassName = %s \n",parentClassName.c_str());

          returnList.push_back(parentClassName);

          lastPosition = position+1;
          position = original_classNameString.find('.',lastPosition);
          if (SgProject::get_verbose() > 0)
               printf ("lastPosition = %zu position = %zu \n",lastPosition,position);

        }

     string className = original_classNameString.substr(lastPosition,position-lastPosition);

     if (SgProject::get_verbose() > 0)
          printf ("className for implicit (leaf) class = %s \n",className.c_str());

  // Reset the name for the most inner nested implicit class.  This allows a class such as "java.lang.System" 
  // to be build as "System" inside of "class "lang" inside of class "java" (without resetting the name we 
  // would have "java.lang.System" inside of "class "lang" inside of class "java").
     name = className;

     if (SgProject::get_verbose() > 0)
          printf ("last name = %s \n",name.str());

  // Push the last name onto the list.
     returnList.push_back(name);

     if (SgProject::get_verbose() > 0)
          printf ("returnList.size() = %zu \n",returnList.size());

#if 0
     int counter = 0;
     list<SgName>::iterator i = returnList.begin();
     while (i != returnList.end())
        {
          printf ("generateQualifierList(): returnList[%d] = %s \n",counter,(*i).str());

          i++;
          counter++;
        }
     printf ("Leaving generateQualifierList() \n");
#endif

#if 0
     printf ("Exiting in generateQualifierList(): after computing the className \n");
     ROSE_ASSERT(false);
#endif

     return returnList;
   }


SgName
stripQualifiers (const SgName & classNameWithQualification)
   {
  // printf ("Calling generateQualifierList(): classNameWithQualification = %s \n",classNameWithQualification.str());
     list<SgName> l = generateQualifierList(classNameWithQualification);

  // printf ("DONE: calling generateQualifierList(): classNameWithQualification = %s \n",classNameWithQualification.str());

     ROSE_ASSERT(l.empty() == false);

     if (SgProject::get_verbose() > 0)
          printf ("result in stripQualifiers(%s) = %s \n",classNameWithQualification.str(),l.back().str());

     return l.back();
   }


SgClassSymbol* 
lookupSymbolFromQualifiedName(string className)
   {
  // Java qualified names are separate by "." and can refer to classes that
  // are implicit (not appearing in the source code).  ROSE determines all
  // referenced implicit classes (recursively) and includes them in the AST
  // to support a proper AST with full type resolution, etc.  This can make 
  // the AST for even a trivial Java program rather large.

     list<SgName> qualifiedClassName = generateQualifierList(className);

  // printf ("DONE: generateQualifierList() called from lookupSymbolFromQualifiedName(%s) \n",className.c_str());

     SgClassSymbol* previousClassSymbol = NULL;
     SgScopeStatement* previousClassScope = astJavaScopeStack.front();
     ROSE_ASSERT(previousClassScope != NULL);

  // Traverse all of the classes to get to the class containing the functionName.
     for (list<SgName>::iterator i = qualifiedClassName.begin(); i != qualifiedClassName.end(); i++)
        {
       // Get the class from the current scope of the nearest outer most scope.
       // SgClassSymbol* classSymbol = astJavaScopeStack.back()->lookupSymbolInParentScopes(*i);

          ROSE_ASSERT(previousClassScope != NULL);

          if (SgProject::get_verbose() > 2)
               printf ("Lookup SgSymbol for name = %s in scope = %p = %s = %s \n",(*i).str(),previousClassScope,previousClassScope->class_name().c_str(),SageInterface::get_name(previousClassScope).c_str());

          SgSymbol* tmpSymbol = SageInterface::lookupSymbolInParentScopes(*i,previousClassScope);

       // ROSE_ASSERT(tmpSymbol != NULL);
          if (tmpSymbol != NULL)
             {
               if (SgProject::get_verbose() > 2)
                    printf ("Found a symbol tmpSymbol = %s = %s \n",tmpSymbol->class_name().c_str(),tmpSymbol->get_name().str());

            // This is either a proper class or an alias to a class where the class is implicit or included via an import statement.
               SgClassSymbol*    classSymbol    = isSgClassSymbol(tmpSymbol);
               SgVariableSymbol* variableSymbol = isSgVariableSymbol(tmpSymbol);
               SgAliasSymbol*    aliasSymbol    = isSgAliasSymbol(tmpSymbol);

               if (classSymbol == NULL && aliasSymbol != NULL)
                  {
                 // printf ("Trace through the alias to the proper symbol in another scope. \n");
                    classSymbol = isSgClassSymbol(aliasSymbol->get_alias());
                  }
                 else
                  {
                 // This could be a call to "System.out.println();" (see test2011_04.java) in which case
                 // this is a variableSymbol and the type of the variable is the class which has the 
                 // "println();" function.
                    if (classSymbol == NULL && variableSymbol != NULL)
                       {
                      // Find the class associated with the type of the variable (this could be any expression so this get's messy!)
                         SgType* tmpType = variableSymbol->get_type();
                         ROSE_ASSERT(tmpType != NULL);

                         printf ("variable type = %p = %s \n",tmpType,tmpType->class_name().c_str());

                      // This should be a SgClassType but currently all variables are build with SgTypeInt.
                      // So this is the next item to fix in the AST.
                         SgClassType* classType = isSgClassType(tmpType);

                      // ROSE_ASSERT(classType != NULL);
                         if (classType != NULL)
                            {
                              ROSE_ASSERT(classType->get_declaration() != NULL);
                              SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
                              ROSE_ASSERT(classDeclaration != NULL);

                              SgSymbol* tmpSymbol = classDeclaration->search_for_symbol_from_symbol_table();
                              ROSE_ASSERT(tmpSymbol != NULL);
                              classSymbol = isSgClassSymbol(tmpSymbol);
                              ROSE_ASSERT(classSymbol != NULL);
                            }
                           else
                            {
                           // This case happens when we are debugging the Java support and we have not built all the 
                           // implicit classes and yet we discover a type used in a function argument list or retun 
                           // type that is missing. In this case return NULL and it will be handled by the calling function.
                              printf ("WARNING: lookupSymbolFromQualifiedName(name = %s) is returning NULL since the class type was not found (debug mode) \n",className.c_str());
                              return NULL;
                            }
                       }
                      else
                       {
                      // DQ (7/17/2011): This is not from a variable, it can be associated with a function when we started inside of the class.  See test2011_21.java.
                         SgFunctionSymbol* functionSymbol = isSgFunctionSymbol(tmpSymbol);
                         if (functionSymbol != NULL)
                            {
                           // printf ("This could/should the constructor for the class we want, we just want the class... \n");

                           // Get the class directly since it is likely a parent class of the current scope.
                              classSymbol = SageInterface::lookupClassSymbolInParentScopes(*i,previousClassScope);
                              ROSE_ASSERT(classSymbol != NULL);
                            }

                         ROSE_ASSERT(classSymbol != NULL);
                       }

                    ROSE_ASSERT(aliasSymbol == NULL);
                  }

               ROSE_ASSERT(classSymbol != NULL);

               if (SgProject::get_verbose() > 2)
                    printf ("classSymbol = %p for class name = %s \n",classSymbol,(*i).str());

               previousClassSymbol = classSymbol;
               SgClassDeclaration* classDeclaration = isSgClassDeclaration(classSymbol->get_declaration()->get_definingDeclaration());
               ROSE_ASSERT(classDeclaration != NULL);
            // previousClassScope = classSymbol->get_declaration()->get_scope();
               previousClassScope = classDeclaration->get_definition();
               ROSE_ASSERT(previousClassScope != NULL);
             }
            else
             {
            // This is OK when we are only processing a small part of the implicit class space (debugging mode) and have not built all the SgClassDeclarations. 
            // printf ("WARNING: SgClassSymbol NOT FOUND in lookupSymbolFromQualifiedName(): name = %s \n",className.c_str());
               previousClassSymbol = NULL;
             }
        }

     return previousClassSymbol;
   }



SgClassType* 
lookupTypeFromQualifiedName(string className)
   {
  // Lookup the name, find the symbol, build a SgClassType, and push it onto the astJavaTypeStack.
     SgClassSymbol* targetClassSymbol = lookupSymbolFromQualifiedName(className);

     SgClassType* classType = NULL;

     if (SgProject::get_verbose() > 2)
          printf ("DONE: In lookupTypeFromQualifiedName(): Calling lookupSymbolFromQualifiedName(name = %s) targetClassSymbol = %p \n",className.c_str(),targetClassSymbol);

  // printf ("DONE: In lookupTypeFromQualifiedName(): Calling lookupSymbolFromQualifiedName(name = %s) targetClassSymbol = %p \n",className.c_str(),targetClassSymbol);
  // ROSE_ASSERT(targetClassSymbol != NULL);

     if (targetClassSymbol != NULL)
        {
          if (SgProject::get_verbose() > 2)
               printf ("In Java_JavaParser_cactionGenerateClassType(): Get the SgClassDeclaration \n");

          SgClassDeclaration* classDeclaration = isSgClassDeclaration(targetClassSymbol->get_declaration());
          ROSE_ASSERT(classDeclaration != NULL);

       // printf ("In Java_JavaParser_cactionGenerateClassType(): Get the SgClassType \n");
       // SgClassType* classType = SgClassType::createType(classDeclaration);
          classType = SgClassType::createType(classDeclaration);
          ROSE_ASSERT(classType != NULL);

      // astJavaTypeStack.push_front(classType);
        }
       else
        {
       // This is OK when we are only processing a small part of the implicit class space (debugging mode) and have not built all the SgClassDeclaration IR nodes. 
       // printf ("WARNING: SgClassSymbol NOT FOUND in lookupTypeFromQualifiedName(): className = %s (returning NULL) \n",className.c_str());
#if 0
          printf ("ERROR: type not found \n");
          ROSE_ASSERT(false);
#endif
        }

     return classType;
   }



void
appendStatement(SgStatement* statement)
   {
  // This support function handles the complexity of handling append where the current scope is a SgIfStmt.
//     SgIfStmt* ifStatement = isSgIfStmt(astJavaScopeStack.front());
//     if (ifStatement != NULL)
//        {
//          SgNullStatement* nullStatement = isSgNullStatement(ifStatement->get_true_body());
//          if (nullStatement != NULL)
//             {
//               ifStatement->set_true_body(statement);
//               delete nullStatement;
//             }
//            else
//             {
//               ifStatement->set_false_body(statement);
//             }
//        }
//       else
//        {
          astJavaScopeStack.front()->append_statement(statement);
//        }


//     ROSE_ASSERT(statement->get_parent() != NULL);
   }
//
//
///*
// * Pop nb_pop elements from a stack and push them on a new stack
// * (the 'n' poped element are pushed in the order they are pop)
// */
std::list<SgStatement*> pop_from_stack_and_reverse(std::list<SgStatement*>& l, int nb_pop) {
        list<SgStatement*> nl;
        while(nb_pop > 0) {
                SgStatement * s = l.front();
                l.pop_front();
                nl.push_front(s);
                nb_pop--;
        }
        return nl;
}
//
//=======
//// void appendStatementStack()
void
appendStatementStack(int numberOfStatements)
   {
  // DQ (9/30/2011): Modified to only pop a precise number of statements off the of the stack.

  // This function is used to dump all statements accumulated on the astJavaStatementStack
  // into the current scope (called as part of closing off the scope where functions that 
  // don't call the function to close off statements).

  // Reverse the list to avoid acesses to the stack from the bottom, 
  // which would be confusing and violate stack semantics.
     int counter = 0;
     list<SgStatement*> reverseStatementList;

  // DQ (7/30/2011): We want to be more exact in the future, if possible.  This allows
  // for the number of statements to be larger than the statck size and if so we take
  // everything on the stack, but don't trigger an error.
     while (astJavaStatementStack.empty() == false && counter < numberOfStatements)
        {
          reverseStatementList.push_front(astJavaStatementStack.front());
          astJavaStatementStack.pop_front();

          counter++;
        }

     while (reverseStatementList.empty() == false)
        {
          appendStatement(reverseStatementList.front());

          ROSE_ASSERT(reverseStatementList.front()->get_parent() != NULL);
          ROSE_ASSERT(reverseStatementList.front()->get_parent()->get_startOfConstruct() != NULL);

          reverseStatementList.pop_front();
        }
   }



SgClassDefinition*
getCurrentClassDefinition()
   {
     SgClassDefinition* classDefinition = NULL;
     std::list<SgScopeStatement*>::reverse_iterator i = astJavaScopeStack.rbegin();
     while (i != astJavaScopeStack.rend() && isSgClassDefinition(*i) == NULL)
        {
          i++;
        }

     if (i != astJavaScopeStack.rend())
        {
          classDefinition = isSgClassDefinition(*i);
          string className = classDefinition->get_declaration()->get_name();
       // printf ("Current class is className = %s \n",className.c_str());
        }
       else
        {
          printf ("Error in getCurrentClassDefinition(): SgClassDefinition not found \n");
          ROSE_ASSERT(false);
        }

     ROSE_ASSERT(classDefinition != NULL);
     return classDefinition;
   }


SgName
processNameOfRawType(SgName name)
   {
     string nameString = name;
  // printf ("nameString = %s \n",nameString.c_str());

  // DQ (8/22/2011): This is not a reasonable assertion (fails for test2011_42.java).
  // DQ (8/20/2011): Detect if this is a #RAW type (should have been processed to be "java.lang.<class name>".
  // ROSE_ASSERT(nameString.length() < 4 || nameString.find("#RAW",nameString.length()-4) == string::npos);

  // ROSE_ASSERT(nameString.length() >= 4);
  // size_t startOfRawSuffix = nameString.find("#RAW",nameString.length()-4);
  // if (startOfRawSuffix != string::npos)
     if (nameString.length() >= 4 && nameString.find("#RAW",nameString.length()-4) != string::npos)
        {
          size_t startOfRawSuffix = nameString.find("#RAW",nameString.length()-4);
          nameString = nameString.substr(0,startOfRawSuffix);

       // List and ArrayList are in java.util class, but we don't want to have to know this.
       // nameString = "java.lang." + nameString;
          nameString = "java.util." + nameString;

          if (SgProject::get_verbose() > 2)
               printf ("Found raw type (generic type without type parameter) nameString = %s \n",nameString.c_str());

          name = nameString;
        }
       else
        {
          if (SgProject::get_verbose() > 2)
               printf ("Not a raw type: nameString = %s \n",nameString.c_str());
        }

     return name;
   }


SgScopeStatement*
get_scope_from_symbol( SgSymbol* returnSymbol )
   {
     SgClassSymbol* classSymbol = isSgClassSymbol(returnSymbol);
     ROSE_ASSERT(classSymbol != NULL);

     SgDeclarationStatement* declarationFromSymbol = classSymbol->get_declaration();
     ROSE_ASSERT(declarationFromSymbol != NULL);

     SgClassDeclaration* definingClassDeclaration  = isSgClassDeclaration(declarationFromSymbol->get_definingDeclaration());
     ROSE_ASSERT(definingClassDeclaration != NULL);

     SgScopeStatement* currentScope = definingClassDeclaration->get_definition();
     ROSE_ASSERT(currentScope != NULL);

     return currentScope;
   }


SgSymbol*
lookupSymbolInParentScopesUsingQualifiedName( SgName qualifiedName, SgScopeStatement* currentScope)
   {
  // The name is likely qualified, and we can't directly look up a qualified name.

     list<SgName> qualifiedNameList = generateQualifierList(qualifiedName);
     ROSE_ASSERT(qualifiedNameList.empty() == false);

     list<SgName>::iterator i = qualifiedNameList.begin();

     printf ("In lookupSymbolInParentScopesUsingQualifiedName(): Seaching for symbol for qualifiedName = %s name = %s (inital name) \n",qualifiedName.str(),(*i).str());

  // Lookup the first name using the parent scopes, but then drill down into the identified scopes only.
     SgSymbol* returnSymbol = SageInterface::lookupSymbolInParentScopes(*i,currentScope);
     ROSE_ASSERT(returnSymbol != NULL);

  // Increment past the first name in the qualified name.
     i++;

  // If there are more names in the list, then drill down in to each to find the next class.
     while (i != qualifiedNameList.end())
        {
          printf ("In lookupSymbolInParentScopesUsingQualifiedName(): Seaching for symbol for name = %s \n",(*i).str());

          currentScope = get_scope_from_symbol(returnSymbol);
          ROSE_ASSERT(currentScope != NULL);

          returnSymbol = currentScope->lookup_class_symbol(*i);
          ROSE_ASSERT(returnSymbol != NULL);

          i++;
        }


     return returnSymbol;
   }



list<SgName>
generateGenericTypeNameList (const SgName & parameterizedTypeName)
   {
  // This function separates the name from the parameters of the type.
  // From: List<java.lang.String>
  // Generate: "List" and "java.lang.String" as separate elements of the returned list.
  //           Additional type parameters are returned as additonal elements in the list.

     list<SgName> returnList;

#if 0
  // Names of implicitly defined classes have names that start with "java." and these have to be translated.
     string original_classNameString = parameterizedTypeName.str();
     string classNameString          = parameterizedTypeName.str();

  // Also replace '.' with '_'
     replace(classNameString.begin(), classNameString.end(),'.','_');

  // Also replace '$' with '_' (not clear on what '$' means yet (something related to inner and outer class nesting).
     replace(classNameString.begin(), classNameString.end(),'$','_');

     SgName name = classNameString;

  // We should not have a '.' in the class name.  Or it will fail the current ROSE name mangling tests.
     ROSE_ASSERT(classNameString.find('.') == string::npos);

  // DQ (3/20/2011): Detect use of '$' in class names. Current best reference 
  // is: http://www.java-forums.org/new-java/27577-specific-syntax-java-util-regex-pattern-node.html
     ROSE_ASSERT(classNameString.find('$') == string::npos);

     size_t starting_position = original_classNameString.find('<',0);
     size_t ending_position   = original_classNameString.find('>',lastPosition);











     while (position != string::npos)
        {
          string parentClassName = original_classNameString.substr(lastPosition,position-lastPosition);
          if (SgProject::get_verbose() > 0)
               printf ("parentClassName = %s \n",parentClassName.c_str());

          returnList.push_back(parentClassName);

          lastPosition = position+1;
          position = original_classNameString.find('.',lastPosition);
          if (SgProject::get_verbose() > 0)
               printf ("lastPosition = %zu position = %zu \n",lastPosition,position);

        }

     string className = original_classNameString.substr(lastPosition,position-lastPosition);

     if (SgProject::get_verbose() > 0)
          printf ("className for implicit (leaf) class = %s \n",className.c_str());

  // Reset the name for the most inner nested implicit class.  This allows a class such as "java.lang.System" 
  // to be build as "System" inside of "class "lang" inside of class "java" (without resetting the name we 
  // would have "java.lang.System" inside of "class "lang" inside of class "java").
     name = className;

     if (SgProject::get_verbose() > 0)
          printf ("last name = %s \n",name.str());

  // Push the last name onto the list.
     returnList.push_back(name);

     if (SgProject::get_verbose() > 0)
          printf ("returnList.size() = %zu \n",returnList.size());

#if 0
     int counter = 0;
     list<SgName>::iterator i = returnList.begin();
     while (i != returnList.end())
        {
          printf ("generateQualifierList(): returnList[%d] = %s \n",counter,(*i).str());

          i++;
          counter++;
        }
     printf ("Leaving generateQualifierList() \n");
#endif

#if 0
     printf ("Exiting in generateQualifierList(): after computing the className \n");
     ROSE_ASSERT(false);
#endif
#endif

     return returnList;
   }

