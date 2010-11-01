#include "sage3basic.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

#include <jni.h>

// Support functions declaration of function defined in this file.
#include "java_support.h"


using namespace std;


// DQ (10/21/2010): If Fortran is being supported then there will
// be a definition of this pointer there.  Note that we currently
// use only one pointer so that we can simplify how the JVM support 
// is used for either Fortran or Java language support.
#ifndef ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
SgSourceFile* OpenFortranParser_globalFilePointer = NULL;
#endif


// Global stack of scopes
list<SgScopeStatement*> astJavaScopeStack;

// Global stack of expressions 
list<SgExpression*> astJavaExpressionStack;

// Simplifying type for the setSourcePosition() functions
// typedef std::vector<Token_t*> TokenListType;

// Global stack of IR nodes
list<SgNode*> astJavaNodeStack;

// Attribute spec for holding attributes
// std::list<int> astAttributeSpecStack;




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


string
convertJavaStringToCxxString(JNIEnv *env, const jstring & java_string)
   {
  // Note that "env" can't be passed into this function as "const".
     const char* str = env->GetStringUTFChars(java_string, NULL);
     ROSE_ASSERT(str != NULL);

     string returnString = str;

     printf ("Inside of convertJavaStringToCxxString s = %s \n",str);

  // Note that str is not set to NULL.
     env->ReleaseStringUTFChars(java_string, str);
     ROSE_ASSERT(str != NULL);

  // return str;
  // return string(str);
     return returnString;
   }


SgMemberFunctionDeclaration*
buildSimpleMemberFunction(const SgName & name)
   {
  // This is abstracted so that we can build member functions as require to define Java specific default functions (e.g. super()).

     SgClassDefinition* classDefinition = isSgClassDefinition(astJavaScopeStack.front());
     ROSE_ASSERT(classDefinition != NULL);

     SgFunctionParameterTypeList* typeList = SageBuilder::buildFunctionParameterTypeList();
     ROSE_ASSERT(typeList != NULL);

  // Specify if this is const, volatile, or restrict (0 implies normal member function).
     unsigned int mfunc_specifier = 0;
     SgMemberFunctionType* return_type = SageBuilder::buildMemberFunctionType(SgTypeVoid::createType(), typeList, classDefinition, mfunc_specifier);
     ROSE_ASSERT(return_type != NULL);

     SgFunctionParameterList* parameterlist = SageBuilder::buildFunctionParameterList(typeList);
     ROSE_ASSERT(parameterlist != NULL);

     SgMemberFunctionDeclaration* functionDeclaration = SageBuilder::buildDefiningMemberFunctionDeclaration (name, return_type, parameterlist, astJavaScopeStack.front() );
     ROSE_ASSERT(functionDeclaration != NULL);

     ROSE_ASSERT(functionDeclaration->get_definingDeclaration() != NULL);

  // non-defining declaration not built yet.
     ROSE_ASSERT(functionDeclaration->get_firstNondefiningDeclaration() == NULL);

     SgFunctionDefinition* functionDefinition = functionDeclaration->get_definition();
     ROSE_ASSERT(functionDefinition != NULL);

     return functionDeclaration;
   }
