// This is the file containing the C++ versions of the JNI functions written to interface with ECJ.
// These functions are called from the JAVA code (within the Java based traversal over the ECJ AST) 
// and construct the ROSE AST.

// Support for the internal ROSE IR classes (Precompiled header rules require it to go first).
#include "sage3basic.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

// **********************************************************************************
// WARNING: If this header file is not present, the source file will compile but ROSE
//          will generate an empty AST (no global declarations).
// This is generated from "$(JAVAH) -jni -classpath ${srcdir}/ecjROSE.jar:. JavaParser" 
// called within the src/3rdPartyLibraries/java-parser/Makefile.am and generated only 
// in the build tree (not the source tree).  $(JAVAH) evaluates to "javah" which 
// automatically generates header files (the implimentations of these functions are 
// defined in this file below.
// **********************************************************************************
#include "JavaParser.h"


// Support functions so that this file can be restricted to be just parser (AST traversal) rules.
#include "java_support.h"
#include "jni_utils.h"
#include "VisitorContext.h"

// This is needed so that we can call the FixupAstSymbolTablesToSupportAliasedSymbols::injectSymbolsFromReferencedScopeIntoCurrentScope() function.
#include "fixupCxxSymbolTablesToSupportAliasingSymbols.h"


using namespace std;


// ***************************************************************************
// ***************************************************************************
// Implementation of functions generated from JNI specifications and matching
// prototype declarations found in the auto-generated file JavaParser.h in 
// the build tree at: src/3rdPartyLibraries/java-parser.
// ***************************************************************************
// ***************************************************************************


JNIEXPORT void JNICALL Java_JavaParser_cactionCompilationUnitList (JNIEnv *env, jobject, jint, jobjectArray)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Inside of Java_JavaParser_cactionCompilationUnitList \n");

  // This is already setup by ROSE as part of basic file initialization before calling ECJ.
     ROSE_ASSERT(OpenFortranParser_globalFilePointer != NULL);
     if (SgProject::get_verbose() > 0)
          printf ("OpenFortranParser_globalFilePointer = %s \n",OpenFortranParser_globalFilePointer->class_name().c_str());

     SgSourceFile* sourceFile = isSgSourceFile(OpenFortranParser_globalFilePointer);
     ROSE_ASSERT(sourceFile != NULL);

     if (SgProject::get_verbose() > 0)
          printf ("sourceFile->getFileName() = %s \n",sourceFile->getFileName().c_str());

  // We don't use the SgProject but since it should have already been built, we can verify that it is present.
     SgProject* project = sourceFile->get_project();
     ROSE_ASSERT(project != NULL);

  // Get the pointer to the global scope and push it onto the astJavaScopeStack.
     SgGlobal* globalScope = sourceFile->get_globalScope();
     ROSE_ASSERT(globalScope != NULL);

  // Push the global scope onto the stack (each file has a single global scope).
     ROSE_ASSERT(astJavaScopeStack.empty() == true);
     astJavaScopeStack.push_front(globalScope);
     ROSE_ASSERT(astJavaScopeStack.empty() == false);

  // Verify that the parent is set, these AST nodes are already setup by ROSE before calling this function.
     ROSE_ASSERT(astJavaScopeStack.front()->get_parent() != NULL);

     if (SgProject::get_verbose() > 0)
          printf ("Leaving Java_JavaParser_cactionCompilationUnitList \n");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionCompilationUnitDeclaration (JNIEnv *env, jobject xxx, jstring compilationUnitFilename, jobject jToken)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Inside of Java_JavaParser_cactionCompilationUnitDeclaration() \n");

     outputJavaState("At TOP of cactionTypeDeclaration");

  // Example of how to get the string...but we don't really use the absolutePathFilename in this function.
     const char* absolutePathFilename = env->GetStringUTFChars(compilationUnitFilename, NULL);
     ROSE_ASSERT(absolutePathFilename != NULL);
  // printf ("Inside of Java_JavaParser_cactionCompilationUnitDeclaration absolutePathFilename = %s \n",absolutePathFilename);
     env->ReleaseStringUTFChars(compilationUnitFilename, absolutePathFilename);

  // This is already setup by ROSE as part of basic file initialization before calling ECJ.
     ROSE_ASSERT(OpenFortranParser_globalFilePointer != NULL);

     ROSE_ASSERT(astJavaScopeStack.empty() == false);
     ROSE_ASSERT(astJavaScopeStack.front()->get_parent() != NULL);
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionCompilationUnitDeclarationEnd (JNIEnv *env, jobject xxx, jint java_numberOfStatements, jobject jToken)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Inside of Java_JavaParser_cactionCompilationUnitDeclarationEnd() \n");

     outputJavaState("At TOP of cactionCompilationUnitDeclarationEnd");

     ROSE_ASSERT(astJavaScopeStack.empty() == false);

  // DQ (8/13/2011): Add more precise handling of the statement stack.
     int numberOfStatements = java_numberOfStatements;

     if (SgProject::get_verbose() > 0)
          printf ("cactionCompilationUnitDeclarationEnd(): numberOfStatements = %d astJavaStatementStack.size() = %zu \n",numberOfStatements,astJavaStatementStack.size());

     appendStatementStack(numberOfStatements);

     if (SgProject::get_verbose() > 0)
          printf ("Now we pop off the global scope! \n");

     astJavaScopeStack.pop_front();

     ROSE_ASSERT(astJavaScopeStack.empty() == true);

     outputJavaState("At BOTTOM of cactionCompilationUnitDeclarationEnd");

     if (SgProject::get_verbose() > 0)
          printf ("Leaving Java_JavaParser_cactionCompilationUnitDeclarationEnd() \n");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionTypeDeclaration (JNIEnv *env, jobject xxx, jstring java_string, jobject jToken)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Build a SgClassDeclaration \n");

  // We could provide a constructor for "SgName" that takes a "jstring".  This might help support a simpler interface.
     SgName name = convertJavaStringToCxxString(env,java_string);

     if (SgProject::get_verbose() > 0)
          printf ("Build class type: name = %s \n",name.str());

     outputJavaState("At TOP of cactionTypeDeclaration");

     Token_t * token = create_token(env, jToken);
     ROSE_ASSERT(token != NULL);

  // This builds the associated class in the curren scope as defined by the astJavaScopeStack.
     buildClass(name,token);

     ROSE_ASSERT(astJavaScopeStack.front() != NULL);

     SgClassDefinition* classDefinition = isSgClassDefinition(astJavaScopeStack.front());
     ROSE_ASSERT(classDefinition != NULL);

     setJavaSourcePosition(classDefinition,env,jToken);
     ROSE_ASSERT(classDefinition->get_declaration() != NULL);
     setJavaSourcePosition(classDefinition->get_declaration(),env,jToken);

     if (SgProject::get_verbose() > 0)
          astJavaScopeStack.front()->get_file_info()->display("source position in Java_JavaParser_cactionTypeDeclaration(): debug");

     outputJavaState("At BOTTOM of cactionTypeDeclaration");

     if (SgProject::get_verbose() > 0)
          printf ("Leaving Java_JavaParser_cactionTypeDeclaration() \n");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionTypeDeclarationEnd (JNIEnv *env, jobject xxx, jstring java_string, jint java_numberOfStatements, jobject jToken)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Build a SgClassDeclaration (cactionTypeDeclarationEnd) \n");

  // We could provide a constructor for "SgName" that takes a "jstring".  This might help support a simpler interface.
     SgName name = convertJavaStringToCxxString(env,java_string);

     if (SgProject::get_verbose() > 0)
          printf ("Build class type: name = %s \n",name.str());

     outputJavaState("At TOP of cactionTypeDeclarationEnd");

  // DQ (8/13/2011): Add more precise handling of the statement stack.
     int numberOfStatements = java_numberOfStatements;

     if (SgProject::get_verbose() > 0)
          printf ("cactionTypeDeclarationEnd(): numberOfStatements = %d astJavaStatementStack.size() = %zu \n",numberOfStatements,astJavaStatementStack.size());

     appendStatementStack(numberOfStatements);

     if (SgProject::get_verbose() > 0)
          printf ("We might have to be popping off the existing scope for class type: name = %s \n",name.str());

     ROSE_ASSERT(astJavaScopeStack.front() != NULL);
     SgClassDefinition* classDefinition = isSgClassDefinition(astJavaScopeStack.front());
     ROSE_ASSERT(classDefinition != NULL);

     astJavaScopeStack.pop_front();

     SgClassDeclaration* classDeclaration = classDefinition->get_declaration();
     ROSE_ASSERT(classDeclaration != NULL);

  // Push the class declaration onto the statement stack.
     astJavaStatementStack.push_front(classDeclaration);

     ROSE_ASSERT(astJavaScopeStack.front() != NULL);

     if (SgProject::get_verbose() > 0)
          astJavaScopeStack.front()->get_file_info()->display("source position in Java_JavaParser_cactionTypeDeclarationEnd(): debug");

     outputJavaState("At BOTTOM of cactionTypeDeclarationEnd");

     if (SgProject::get_verbose() > 0)
          printf ("Leaving Java_JavaParser_cactionTypeDeclarationEnd() (cactionTypeDeclarationEnd) \n");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionConstructorDeclaration (JNIEnv *env, jobject xxx, jstring java_string, jboolean java_is_native, jobject jToken)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Build a SgMemberFunctionDeclaration (constructor) \n");

     outputJavaState("At TOP of cactionConstructorDeclaration");

     SgName name   = convertJavaStringToCxxString(env,java_string);
     bool isNative = java_is_native;

     SgClassDefinition* classDefinition = isSgClassDefinition(astJavaScopeStack.front());
     ROSE_ASSERT(classDefinition != NULL);

  // DQ (3/25/2011): changed to reflect requirement to build defining declaration.
  // SgMemberFunctionDeclaration* functionDeclaration = buildSimpleMemberFunction(name,classDefinition);
     SgMemberFunctionDeclaration* functionDeclaration = buildDefiningMemberFunction(name,classDefinition);

     setJavaSourcePosition(functionDeclaration,env,jToken);

     SgFunctionDefinition* functionDefinition = functionDeclaration->get_definition();
     ROSE_ASSERT(functionDefinition != NULL);

     setJavaSourcePosition(functionDefinition,env,jToken);

     astJavaScopeStack.push_front(functionDefinition);
     ROSE_ASSERT(astJavaScopeStack.front()->get_parent() != NULL);

     ROSE_ASSERT(functionDefinition->get_body() != NULL);
     astJavaScopeStack.push_front(functionDefinition->get_body());
     ROSE_ASSERT(astJavaScopeStack.front()->get_parent() != NULL);

     setJavaSourcePosition(functionDefinition->get_body(),env,jToken);

  // Since this is a constructor, set it explicitly as such.
     functionDeclaration->get_specialFunctionModifier().setConstructor();

  // Set the Java specific modifiers
     if (isNative == true)
          functionDeclaration->get_functionModifier().setJavaNative();

     outputJavaState("At BOTTOM of cactionConstructorDeclaration");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionConstructorDeclarationEnd (JNIEnv *env, jobject xxx, jint java_numberOfStatements, jobject jToken)
   {
     if (SgProject::get_verbose() > 0)
          printf ("End of SgMemberFunctionDeclaration (constructor) \n");

     outputJavaState("At TOP of cactionConstructorDeclarationEnd");

  // DQ (7/31/2011): Add more precise handling of the statement stack.
  // This does not count (include) explicit constructor calls...
     int numberOfStatements = java_numberOfStatements;

     if (SgProject::get_verbose() > 0)
          printf ("cactionConstructorDeclarationEnd(): numberOfStatements = %d astJavaStatementStack.size() = %zu \n",numberOfStatements,astJavaStatementStack.size());

     appendStatementStack(numberOfStatements);

  // Pop the constructor body...
     ROSE_ASSERT(astJavaScopeStack.empty() == false);
     astJavaScopeStack.pop_front();

  // Check for the fuction definition...
     ROSE_ASSERT(astJavaScopeStack.empty() == false);

     SgFunctionDefinition* memberFunctionDefinition = isSgFunctionDefinition(astJavaScopeStack.front());
     ROSE_ASSERT(memberFunctionDefinition != NULL);

  // Pop the fuction definition...
     astJavaScopeStack.pop_front();

     ROSE_ASSERT(memberFunctionDefinition->get_declaration() != NULL);
     SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(memberFunctionDefinition->get_declaration());
     ROSE_ASSERT(memberFunctionDeclaration != NULL);

     astJavaStatementStack.push_front(memberFunctionDeclaration);

     outputJavaState("At BOTTOM of cactionConstructorDeclarationEnd");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionExplicitConstructorCall (JNIEnv *env, jobject xxx, jstring java_string, jobject jToken)
   {
  // Build a member function call...
     if (SgProject::get_verbose() > 0)
          printf ("Build a explicit constructor function call \n");

     outputJavaState("At TOP of cactionExplicitConstructorCall");

  // ****************************************************
  // ****************************************************
     printf ("This function is not called anywhere! \n");
     ROSE_ASSERT(false);
  // ****************************************************
  // ****************************************************

/*
  // Should this be a SgBasicBlock or just a SgScopeStatement?
     SgBasicBlock* basicBlock = isSgBasicBlock(astJavaScopeStack.front());
     ROSE_ASSERT(basicBlock != NULL);
     ROSE_ASSERT(basicBlock->get_parent() != NULL);

     SgName name = convertJavaStringToCxxString(env,java_string);

     if (SgProject::get_verbose() > 0)
          printf ("building function call: name = %s \n",name.str());

     SgExprListExp* parameters = NULL;
     SgExprStatement* expressionStatement = SageBuilder::buildFunctionCallStmt(name,SgTypeVoid::createType(),parameters,astJavaScopeStack.front());
     ROSE_ASSERT(expressionStatement != NULL);

     ROSE_ASSERT(astJavaScopeStack.empty() == false);
     appendStatement(expressionStatement);

     ROSE_ASSERT(expressionStatement->get_parent() != NULL);

     printf ("In cactionExplicitConstructorCall: Number of statements in current scope = %zu \n",astJavaScopeStack.front()->generateStatementList().size());
     outputJavaState("At BOTTOM of cactionExplicitConstructorCall");
*/
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionExplicitConstructorCallEnd (JNIEnv *env, jobject xxx, jstring java_string, jobject jToken)
   {
  // Build a member function call...
     if (SgProject::get_verbose() > 0)
          printf ("Build a explicit constructor function call END \n");

     outputJavaState("At TOP of cactionExplicitConstructorCallEnd");

  // ROSE_ASSERT(astJavaExpressionStack.empty() == false);
     if (astJavaExpressionStack.empty() == false)
        {
          SgExpression* expr = astJavaExpressionStack.front();
          ROSE_ASSERT(expr != NULL);
          astJavaExpressionStack.pop_front();

          SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(expr);
          ROSE_ASSERT(functionCallExp != NULL);

          SgExprStatement* expressionStatement = SageBuilder::buildExprStatement(functionCallExp);
          ROSE_ASSERT(expressionStatement != NULL);

          setJavaSourcePosition(functionCallExp,env,jToken);
          setJavaSourcePosition(expressionStatement,env,jToken);

       // DQ (7/31/2011): This should be left on the stack instead of being added to the current scope before the end of the scope.
       // printf ("Previously calling appendStatement in cactionExplicitConstructorCallEnd() \n");
       // appendStatement(expressionStatement);
          astJavaStatementStack.push_front(expressionStatement);
        }

     outputJavaState("At BOTTOM of cactionExplicitConstructorCallEnd");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionMethodDeclaration (JNIEnv *env, jobject xxx, jstring java_string, 
     jboolean java_is_abstract, jboolean java_is_native, jboolean java_is_static, jboolean java_is_final,
     jboolean java_is_synchronized, jboolean java_is_public, jboolean java_is_protected, jboolean java_is_private, jboolean java_is_strictfp, jobject jToken)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Build a SgMemberFunctionDeclaration \n");

     outputJavaState("At TOP of cactionMethodDeclaration");

     SgName name = convertJavaStringToCxxString(env,java_string);

     bool isAbstract     = java_is_abstract;
     bool isNative       = java_is_native;
     bool isStatic       = java_is_static;
     bool isFinal        = java_is_final;
     bool isSynchronized = java_is_synchronized;
     bool isPublic       = java_is_public;
     bool isProtected    = java_is_protected;
     bool isPrivate      = java_is_private;
     bool isStrictfp     = java_is_strictfp;

     SgClassDefinition* classDefinition = isSgClassDefinition(astJavaScopeStack.front());
     ROSE_ASSERT(classDefinition != NULL);

  // DQ (3/25/2011): Changed this to a non-defining declaration.
  // SgMemberFunctionDeclaration* functionDeclaration = buildSimpleMemberFunction(name,classDefinition);
  // SgMemberFunctionDeclaration* functionDeclaration = buildNonDefiningMemberFunction(name,classDefinition);
     SgMemberFunctionDeclaration* functionDeclaration = buildDefiningMemberFunction(name,classDefinition);
     ROSE_ASSERT(functionDeclaration != NULL);

     setJavaSourcePosition(functionDeclaration,env,jToken);

  // This is not a defining function declaration so we can't identify the SgFunctionDefinition and push it's body onto the astJavaScopeStack.
     SgFunctionDefinition* functionDefinition = functionDeclaration->get_definition();
     ROSE_ASSERT(functionDefinition != NULL);

     setJavaSourcePosition(functionDefinition,env,jToken);

     astJavaScopeStack.push_front(functionDefinition);
     ROSE_ASSERT(astJavaScopeStack.front()->get_parent() != NULL);

     ROSE_ASSERT(functionDefinition->get_body() != NULL);
     astJavaScopeStack.push_front(functionDefinition->get_body());
     ROSE_ASSERT(astJavaScopeStack.front()->get_parent() != NULL);

     setJavaSourcePosition(functionDefinition->get_body(),env,jToken);

  // Set the Java specific modifiers
     if (isAbstract == true)
          functionDeclaration->get_declarationModifier().setJavaAbstract();

  // Set the Java specific modifiers
     if (isNative == true)
          functionDeclaration->get_functionModifier().setJavaNative();

  // Set the specific modifier, this modifier is common to C/C++.
     if (isStatic == true)
          functionDeclaration->get_declarationModifier().get_storageModifier().setStatic();

  // Set the modifier (shared between PHP and Java).
     if (isFinal == true)
          functionDeclaration->get_declarationModifier().setFinal();

  // DQ (8/13/2011): Added more modifiers.
     if (isSynchronized == true)
        {
          if (SgProject::get_verbose() > 2)
               printf ("Setting modifier as Synchronized \n");
          functionDeclaration->get_functionModifier().setJavaSynchronized();
        }

  // Set the access modifiers (shared between C++ and Java).
     if (isPublic == true)
        {
          if (SgProject::get_verbose() > 2)
               printf ("Setting modifier as Public \n");
          functionDeclaration->get_declarationModifier().get_accessModifier().setPublic();
        }

     if (isProtected == true)
        {
          if (SgProject::get_verbose() > 2)
               printf ("Setting modifier as Protected \n");
          functionDeclaration->get_declarationModifier().get_accessModifier().setProtected();
        }

     if (isPrivate == true)
        {
          if (SgProject::get_verbose() > 2)
               printf ("Setting modifier as Private \n");
          functionDeclaration->get_declarationModifier().get_accessModifier().setPrivate();
        }

  // Set the Java specific modifier for strict floating point (defined for functions).
     if (isStrictfp == true)
        {
          if (SgProject::get_verbose() > 2)
               printf ("Setting modifier as strictfp \n");
          functionDeclaration->get_functionModifier().setJavaStrictfp();
        }

     outputJavaState("At BOTTOM of cactionMethodDeclaration");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionMethodDeclarationEnd (JNIEnv *env, jobject xxx, int java_numberOfStatements /* , jstring java_string */ , jobject jToken)
   {
     if (SgProject::get_verbose() > 0)
          printf ("End of SgMemberFunctionDeclaration (method) \n");

     outputJavaState("At TOP of cactionMethodDeclarationEnd");

  // Pop the constructor body...
     ROSE_ASSERT(astJavaScopeStack.empty() == false);

  // appendStatementStack();
     int numberOfStatements = java_numberOfStatements;

     if (SgProject::get_verbose() > 0)
          printf ("In cactionMethodDeclarationEnd(): numberOfStatements = %d astJavaStatementStack.size() = %zu \n",numberOfStatements,astJavaStatementStack.size());

     appendStatementStack(numberOfStatements);

     astJavaScopeStack.pop_front();

  // Check for the fuction definition...
     ROSE_ASSERT(astJavaScopeStack.empty() == false);

     SgFunctionDefinition* memberFunctionDefinition = isSgFunctionDefinition(astJavaScopeStack.front());
     ROSE_ASSERT(memberFunctionDefinition != NULL);

  // Pop the fuction definition...
     astJavaScopeStack.pop_front();

     ROSE_ASSERT(memberFunctionDefinition->get_declaration() != NULL);
     SgMemberFunctionDeclaration* memberFunctionDeclaration = isSgMemberFunctionDeclaration(memberFunctionDefinition->get_declaration());
     ROSE_ASSERT(memberFunctionDeclaration != NULL);

     astJavaStatementStack.push_front(memberFunctionDeclaration);

     outputJavaState("At BOTTOM of cactionMethodDeclarationEnd");

  // DQ (7/31/2011): We can't assert this since there could be a import statement on the stack and that will invalidate this rule.
  // ROSE_ASSERT (astJavaStatementStack.empty() == true);
   }



JNIEXPORT void JNICALL Java_JavaParser_cactionSingleTypeReference (JNIEnv *, jobject, jstring, jobject jToken)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Build a type \n");

     outputJavaState("At TOP of cactionSingleTypeReference");

  // Build a type and put it onto the type stack.
  // ...OR...
  // Build a type and add it to the declaration on the declaration stack.
   }

JNIEXPORT void JNICALL Java_JavaParser_cactionArgument (JNIEnv *env, jobject xxx, jstring java_argument_name, jint java_modifiers, jobject jToken)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Build a function argument \n");

     SgName argument_name = convertJavaStringToCxxString(env,java_argument_name);

     if (SgProject::get_verbose() > 0)
          printf ("argument argument_name = %s \n",argument_name.str());

     int modifiers = convertJavaIntegerToCxxInteger(env,java_modifiers);

     if (SgProject::get_verbose() > 0)
          printf ("modifiers = %d \n",modifiers);

     ROSE_ASSERT(astJavaTypeStack.empty() == false);
     SgType* type = astJavaTypeStack.front();
     astJavaTypeStack.pop_front();

  // Until we attached this to the AST, this will generate an error in the AST consistancy tests.
     SgInitializedName* initializedName = SageBuilder::buildInitializedName(argument_name,type,NULL);
     ROSE_ASSERT(initializedName != NULL);

     setJavaSourcePosition(initializedName,env,jToken);

  // DQ (4/6/2011): Instead of assuming there is a function declaration available, we 
  // want to put each SgInitializedName onto the stack so that they can be assembled
  // into the parameter list as part of building the function declaration afterwards.
  // This also allows for the function type to be computed from the function
  // parameter list (the types match the types on the function parameters) and
  // then the SgFunctionSymbol can be computed and inserted into the function 
  // declaration's scope (current scope) as part of building the function declaration.
  // To support this we use the astJavaInitializedNameStack.
     astJavaInitializedNameStack.push_front(initializedName);

#if 0
     printf ("Exiting in Java_JavaParser_cactionArgument() \n");
     ROSE_ASSERT(false);
#endif
   }

JNIEXPORT void JNICALL Java_JavaParser_cactionArrayTypeReference (JNIEnv *, jobject, jstring, jobject jToken)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Build a array type \n");
   }

JNIEXPORT void JNICALL Java_JavaParser_cactionMessageSend (JNIEnv *env, jobject xxx, jstring functionName, jstring associatedClassName, jobject jToken)
   {
  // This code is the same as that in cactionExplicitConstructorCall
     if (SgProject::get_verbose() > 1)
          printf ("Build a member function call (message send) \n");

     outputJavaState("At TOP of cactionMessageSend");

  // Should this be a SgBasicBlock or just a SgScopeStatement?
     SgBasicBlock* basicBlock = isSgBasicBlock(astJavaScopeStack.front());
     ROSE_ASSERT(basicBlock != NULL);
     ROSE_ASSERT(basicBlock->get_parent() != NULL);

     SgName name      = convertJavaStringToCxxString(env,functionName);
     SgName className = convertJavaStringToCxxString(env,associatedClassName);

  // For Java 1.5 and greater we need to process raw type names to remove the "#RAW" suffix.
     className = processNameOfRawType(className);

     string classNameString = className;

     if (name == "super")
        {
       // Handle case of super class.
       // printf ("Handle case of super class. \n");

       // Refactored the support for getting the current class scope.
          SgClassDefinition* classDefinition = getCurrentClassDefinition();
          className = classDefinition->get_declaration()->get_name();
        }

     if (SgProject::get_verbose() > 1)
          printf ("building function call: name = %s from class name = %s \n",name.str(),className.str());

  // DQ (8/20/2011): Detect if this is a #RAW type (should have been processed to be "java.lang.<class name>".
     ROSE_ASSERT(classNameString.length() < 4 || classNameString.find("#RAW",classNameString.length()-4) == string::npos);

  // Refactored this code to "lookupSymbolFromQualifiedName()" so it could be used to generate class types.
     SgClassSymbol* targetClassSymbol = lookupSymbolFromQualifiedName(className);

  // ROSE_ASSERT(targetClassSymbol != NULL);

     SgScopeStatement* targetClassScope = NULL;
     if (targetClassSymbol == NULL)
        {
       // This case can happen when in debugging mode where we artifically limit the number of implicit 
       // classes built and so some classes are not available.  We want to have defaults for this mode 
       // as a way of limiting the complexity of problems as they are debugged.

       // printf ("Build a SgTypeInt in the stack since the class was not built (debugging mode): name = %s\n",name.str());
       // astJavaTypeStack.push_front(SgTypeInt::createType());

       // This is not correct, but it allows me to move on and debug other work for now.
       // What this means is that functions from classes that we don't have available will not be resolved as function call expressions.
          printf ("ERROR: we can't find the targetClassScope for className = %s (skipping building this function call -- building int value expression for the stack instead) \n",className.str());
          SgValueExp* dummyValueExpression = SageBuilder::buildIntVal(9999999);
          ROSE_ASSERT(dummyValueExpression != NULL);
          astJavaExpressionStack.push_front(dummyValueExpression);

          return;
        }
       else
        {
       // The associated type was found (after being explicitly built, after not being found the first time)) and is pushed onto the stack.
       // printf ("On the second search for the class = %s (after building it explicitly) it was found! \n",name.str());
       // astJavaTypeStack.push_front(classType);

          SgClassDeclaration* classDeclaration = isSgClassDeclaration(targetClassSymbol->get_declaration()->get_definingDeclaration());
          ROSE_ASSERT(classDeclaration != NULL);

       // SgScopeStatement* targetClassScope = classDeclaration->get_definition();
          targetClassScope = classDeclaration->get_definition();
          ROSE_ASSERT(targetClassScope != NULL);
        }
     ROSE_ASSERT(targetClassScope != NULL);

     if (SgProject::get_verbose() > 1)
          printf ("Looking for the function = %s in class parent scope = %p = %s \n",name.str(),targetClassScope,targetClassScope->class_name().c_str());

     SgFunctionSymbol* functionSymbol = targetClassScope->lookup_function_symbol(name);
  // ROSE_ASSERT(functionSymbol != NULL);

     if (functionSymbol != NULL)
        {
       // printf ("FOUND function symbol = %p \n",functionSymbol);

       // This is OK for now, but might not be good enough for a non-statement function call expression (not clear yet in ECJ AST).
          SgExprListExp* parameters = NULL;

          SgFunctionCallExp* func_call_expr = SageBuilder::buildFunctionCallExp(functionSymbol,parameters);

          setJavaSourcePosition(func_call_expr,env,jToken);

       // Push the expression onto the stack so that the cactionMessageSendEnd can find it 
       // and any function arguments and add the arguments to the function call expression.
          astJavaExpressionStack.push_front(func_call_expr);
        }
       else
        {
       // If this is a function not found (and assuming it is a legal code) then it is because 
       // the associated class's member functions and data members have not been read and 
       // translated into the ROSE AST.

          printf ("ERROR: functionSymbol == NULL \n");
       // ROSE_ASSERT(false);
        }
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionMessageSendEnd (JNIEnv *env, jobject xxx, jobject jToken)
   {
  // Unclear if this should be a function call statement or a function call expression.
  // When it is a statement there does not appear to be any thing to provide a clue 
  // about this, so we will assume it is a statement and do more testing.

  // This code is the same as that in cactionExplicitConstructorCall
     if (SgProject::get_verbose() > 2)
          printf ("Inside of Java_JavaParser_cactionMessageSendEnd() \n");

     outputJavaState("At TOP of cactionMessageSendEnd");

     ROSE_ASSERT(astJavaExpressionStack.empty() == false);

  // The astJavaExpressionStack has all of the arguments to the function call.
     vector<SgExpression*> arguments;
     while (astJavaExpressionStack.empty() == false && isSgFunctionCallExp(astJavaExpressionStack.front()) == NULL)
        {
          ROSE_ASSERT(astJavaExpressionStack.empty() == false);
          arguments.push_back(astJavaExpressionStack.front());

          ROSE_ASSERT(astJavaExpressionStack.front()->get_parent() == NULL);

          astJavaExpressionStack.pop_front();
        }

     SgStatement* statement = NULL;
     if (astJavaExpressionStack.empty() == true)
        {
       // This can happen if we are in a debugging mode and the function was not found and function expression was not built.
       // In this case build a SgNullStatement.
          statement = SageBuilder::buildNullStatement();
          ROSE_ASSERT(statement != NULL);

       // Set the parents of the expressions on the stack to avoid errors in the AST graph generation.
          for (vector<SgExpression*>::iterator i = arguments.begin(); i != arguments.end(); i++)
             {
               ROSE_ASSERT((*i)->get_parent() == NULL);
               (*i)->set_parent(statement);
               ROSE_ASSERT((*i)->get_parent() != NULL);
             }
        }
       else
        {
          ROSE_ASSERT(astJavaExpressionStack.empty() == false);

          SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(astJavaExpressionStack.front());
          ROSE_ASSERT(functionCallExp != NULL);

          astJavaExpressionStack.pop_front();

       // DQ (7/18/2011): Some of these entries in "arguments" from the stack are arguments 
       // and some are associated with the object whose member function is being called.
       // SgExpression* functionRefExp = functionCallExp->get_function();
       // SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(functionCallExp->get_function());
          SgMemberFunctionRefExp* functionRefExp = isSgMemberFunctionRefExp(functionCallExp->get_function());
          ROSE_ASSERT(functionRefExp != NULL);
       // printf ("functionRefExp from functionCallExp = %p = %s \n",functionRefExp,functionRefExp->class_name().c_str());

          setJavaSourcePosition(functionRefExp,env,jToken);

          SgSymbol* symbol = functionRefExp->get_symbol();
          ROSE_ASSERT(symbol != NULL);
       // printf ("symbol from functionRefExp = %p = %s \n",symbol,symbol->class_name().c_str());
          SgMemberFunctionSymbol* functionSymbol = isSgMemberFunctionSymbol(symbol);
          ROSE_ASSERT(functionSymbol != NULL);

          SgFunctionDeclaration* functionDeclaration = functionSymbol->get_declaration();
          ROSE_ASSERT(functionDeclaration != NULL);
       // printf ("functionDeclaration from functionSymbol = %p = %s \n",functionDeclaration,functionDeclaration->class_name().c_str());

          size_t numberOfFunctionParameters = functionDeclaration->get_args().size();

       // printf ("numberOfFunctionParameters = %zu \n",numberOfFunctionParameters);

       // printf ("functionCallExp = %p args = %p \n",functionCallExp,functionCallExp->get_args());

       // Are we traversing this the correct direction to get the argument order correct?
       // printf ("Number of arguments to the function call expression = %zu \n",arguments.size());
       // for (size_t i = 0; i < arguments.size(); i++)
          for (size_t i = 0; i < numberOfFunctionParameters; i++)
             {
               ROSE_ASSERT(arguments[i] != NULL);
               functionCallExp->append_arg(arguments[i]);
             }

          SgExpression* exprForExprStatement = functionCallExp;

       // The remaining arguments are really just the object chain through which the function is called.
          ROSE_ASSERT(arguments.size() >= numberOfFunctionParameters);
          size_t objectChainLength = arguments.size() - numberOfFunctionParameters;
          for (size_t i = 0; i < objectChainLength; i++)
             {
               ROSE_ASSERT(arguments[i] != NULL);
               exprForExprStatement = SageBuilder::buildBinaryExpression<SgDotExp>(arguments[i],exprForExprStatement);
             }

       // ROSE_ASSERT(astJavaStatementStack.empty() == true);
       // SgStatement* statement = SageBuilder::buildExprStatement(functionCallExp);
       // statement = SageBuilder::buildExprStatement(functionCallExp);
          statement = SageBuilder::buildExprStatement(exprForExprStatement);
          ROSE_ASSERT(statement != NULL);
        }

     ROSE_ASSERT(statement != NULL);
     
  // DQ (7/31/2011): This should be left on the stack instead of being added to the current scope before the end of the scope.
  // printf ("Previously calling appendStatement in cactionMessageSendEnd() \n");
  // appendStatement(statement);
     astJavaStatementStack.push_front(statement);

  // printf ("Number of statements in current scope = %zu \n",astJavaScopeStack.front()->generateStatementList().size());

  // DQ (7/18/2011): This will not be true if we are in a "if" statement block.
  // ROSE_ASSERT(astJavaExpressionStack.empty() == true);

     outputJavaState("At BOTTOM of cactionMessageSendEnd");

#if 0
     printf ("Exiting as a test \n");
     ROSE_ASSERT(false);
#endif
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionQualifiedNameReference (JNIEnv * env, jobject xxx, jstring java_string, jobject jToken)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Build a qualified name reference \n");
   }

JNIEXPORT void JNICALL Java_JavaParser_cactionStringLiteral (JNIEnv *env, jobject xxx, jstring java_string, jobject jToken)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Build a SgStringVal \n");

     ROSE_ASSERT(astJavaScopeStack.empty() == false);
     outputJavaState("cactionStringLiteral");

  // string stringLiteral = "stringLiteral_abc";
     SgName stringLiteral = convertJavaStringToCxxString(env,java_string);

     printf ("Building an string value expression = %s \n",stringLiteral.str());

     SgStringVal* stringValue = new SgStringVal(stringLiteral);
     ROSE_ASSERT(stringValue != NULL);

  // Set the source code position (default values for now).
  // setJavaSourcePosition(stringValue);
     setJavaSourcePosition(stringValue,env,jToken);

     astJavaExpressionStack.push_front(stringValue);
     ROSE_ASSERT(astJavaExpressionStack.empty() == false);
   }





// DQ: Note that the function signature is abby-normal...jclass instead of jobject (because they are 
// declared "public static native" instead of "public native" in the Java side of the JNI interface.
// JNIEXPORT void JNICALL Java_JavaParser_cactionBuildImplicitClassSupportStart (JNIEnv* env, jclass xxx, jstring java_string, jobject jToken)
JNIEXPORT void JNICALL Java_JavaParser_cactionBuildImplicitClassSupportStart (JNIEnv* env, jclass xxx, jstring java_string)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Build support for implicit class (start) \n");

     outputJavaState("At TOP of cactionBuildImplicitClassSupportStart");

  // printf ("In Java_JavaParser_cactionBuildImplicitClassSupportStart(): Exiting as a test! \n");
  // ROSE_ASSERT(false);

     SgName name = convertJavaStringToCxxString(env,java_string);

  // This builds a class to represent the implicit classes that are available by default within Java.
  // Each is built on an as needed basis (driven by references to the class).
     buildImplicitClass(name);

  // DQ (4/15/2011): This code should be refactored ...
     SgName classNameWithQualification = name;
     if (SgProject::get_verbose() > 0)
          printf ("implicit class = %s \n",classNameWithQualification.str());

     SgGlobal* globalScope = getGlobalScope();
     ROSE_ASSERT(globalScope != NULL);

     SgName classNameWithoutQualification = stripQualifiers(classNameWithQualification);
     SgClassSymbol* classSymbol = globalScope->lookup_class_symbol(classNameWithoutQualification);
     if (classSymbol != NULL)
        {
       // Nothing to do.
          if (SgProject::get_verbose() > 0)
               printf ("NOTHING TO DO: class = %s is already in global scope (qualified name = %s) \n",classNameWithoutQualification.str(),classNameWithQualification.str());
        }
       else
        {
          if (SgProject::get_verbose() > 0)
               printf ("class = %s must be placed into global scope (qualified name = %s) \n",classNameWithoutQualification.str(),classNameWithQualification.str());

          classSymbol = lookupSymbolFromQualifiedName(classNameWithQualification);

          ROSE_ASSERT(classSymbol != NULL);

          ROSE_ASSERT(globalScope->symbol_exists(classNameWithoutQualification) == false);
          ROSE_ASSERT(globalScope->symbol_exists(classSymbol) == false);

          SgAliasSymbol* aliasSymbol = new SgAliasSymbol(classSymbol,/* isRenamed */ false);

          if ( SgProject::get_verbose() > 0 )
               printf ("Adding SgAliasSymbol for classNameWithoutQualification = %s \n",classNameWithoutQualification.str());

          globalScope->insert_symbol(classNameWithoutQualification,aliasSymbol);
        }

     outputJavaState("At BOTTOM of cactionBuildImplicitClassSupportStart");

#if 0
     printf ("In Java_JavaParser_cactionBuildImplicitClassSupportStart(): Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }

// DQ: Note that the function signature is abby-normal...jclass instead of jobject (because they are 
// declared "public static native" instead of "public native" in the Java side of the JNI interface.
// JNIEXPORT void JNICALL Java_JavaParser_cactionBuildImplicitClassSupportEnd (JNIEnv* env, jclass xxx, jstring java_string, jobject jToken)
JNIEXPORT void JNICALL Java_JavaParser_cactionBuildImplicitClassSupportEnd (JNIEnv* env, jclass xxx, jint java_numberOfStatements, jstring java_string)
   {
     SgName name = convertJavaStringToCxxString(env,java_string);

     if (SgProject::get_verbose() > 0)
        printf ("Build support for implicit class (end) for class = %s \n",name.str());

     ROSE_ASSERT(astJavaScopeStack.empty() == false);
     outputJavaState("At TOP of cactionBuildImplicitClassSupportEnd");

  // Experiment with ERROR on C++ side...communicated to Java...and back to C++ side where the JVM is called by ROSE...
  // ROSE_ASSERT(false);

     ROSE_ASSERT(astJavaScopeStack.empty() == false);

  // DQ (8/20/2011): This is the class that we just built implicitly
     astJavaScopeStack.pop_front();
     ROSE_ASSERT(astJavaScopeStack.empty() == false);

  // DQ (7/31/2011): Collection up all of the statements and append to the current scope.
  // Later I would like to do this more precisely, but for now collect all statements.
  // printf ("Appending all of the statments on the Statement stack (size = %zu) to the current scope = %p = %s \n",astJavaStatementStack.size(),astJavaScopeStack.front(),astJavaScopeStack.front()->class_name().c_str());
  // int numberOfStatements = astJavaStatementStack.size();
     int numberOfStatements = java_numberOfStatements;
     appendStatementStack(numberOfStatements);

  // Pop the class definition off the scope stack...
     ROSE_ASSERT(astJavaScopeStack.empty() == false);
  // astJavaScopeStack.pop_front();

  // At this point we should still at least have the global scope on the stack.
     ROSE_ASSERT(astJavaScopeStack.empty() == false);

  // Output the list of implicit classes seen so far...
     if (SgProject::get_verbose() > 0)
        {
          printf ("astJavaImplicitClassList: \n");
          for (list<SgName>::iterator i = astJavaImplicitClassList.begin(); i != astJavaImplicitClassList.end(); i++)
             {
               printf ("   --- implicit class = %s \n",(*i).str());
             }
        }

     outputJavaState("At BOTTOM of cactionBuildImplicitClassSupportEnd");
   }


// DQ: Note that the function signature is abby-normal...jclass instead of jobject (because they are 
// declared "public static native" instead of "public native" in the Java side of the JNI interface.
// JNIEXPORT void JNICALL Java_JavaParser_cactionBuildImplicitMethodSupport (JNIEnv* env, jclass xxx, jstring java_string, jobject jToken)
JNIEXPORT void JNICALL Java_JavaParser_cactionBuildImplicitMethodSupport (JNIEnv* env, jclass xxx, jstring java_string)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Build support for implicit class member function (method) \n");

     outputJavaState("At TOP of cactionBuildImplicitMethodSupport");

     SgName name = convertJavaStringToCxxString(env,java_string);

     if (SgProject::get_verbose() > 0)
          printf ("Build support for implicit class member function (method) = %s \n",name.str());

  // Note sure if we want anything specific to implicit class handling to touch the astJavaScopeStack!
     SgClassDefinition* classDefinition = isSgClassDefinition(astJavaScopeStack.front());
     ROSE_ASSERT(classDefinition != NULL);

  // DQ (3/25/2011): Changed this to a non-defining declaration.
  // SgMemberFunctionDeclaration* functionDeclaration = buildSimpleMemberFunction(name, classDefinition);
     SgMemberFunctionDeclaration* functionDeclaration = buildNonDefiningMemberFunction(name, classDefinition);
     ROSE_ASSERT(functionDeclaration != NULL);

  // setJavaCompilerGenerated(functionDeclaration);
     setJavaFrontendSpecific(functionDeclaration);

  // Add the types to the non-defining function.

     outputJavaState("At BOTTOM of cactionBuildImplicitMethodSupport");
   }


// DQ: Note that the function signature is abby-normal...jclass instead of jobject (because they are 
// declared "public static native" instead of "public native" in the Java side of the JNI interface.
JNIEXPORT void JNICALL Java_JavaParser_cactionBuildImplicitFieldSupport (JNIEnv* env, jclass xxx, jstring java_string)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Inside of Java_JavaParser_cactionBuildImplicitFieldSupport (variable declaration for field) \n");

     outputJavaState("At TOP of cactionBuildImplicitFieldSupport");

     SgName name = convertJavaStringToCxxString(env,java_string);

     SgVariableDeclaration* variableDeclaration = buildSimpleVariableDeclaration(name);
     ROSE_ASSERT(variableDeclaration != NULL);

  // setJavaCompilerGenerated(variableDeclaration);
     setJavaFrontendSpecific(variableDeclaration);

     ROSE_ASSERT(astJavaScopeStack.empty() == false);

  // DQ (7/31/2011): This should be left on the stack instead of being added to the current scope before the end of the scope.
  // printf ("Previously calling appendStatement in cactionBuildImplicitFieldSupport() \n");
  // appendStatement(variableDeclaration);
     astJavaStatementStack.push_front(variableDeclaration);

     if (SgProject::get_verbose() > 0)
          variableDeclaration->get_file_info()->display("source position in Java_JavaParser_cactionBuildImplicitFieldSupport(): debug");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionGenerateType (JNIEnv* env, jclass xxx, jstring java_string)
   {
  // This is general support for building types.  Using JNI one to many calls from Java are made to C++ 
  // with enough information to build specific types or parts of types (for aggragate types such as classes).
  // All type information is constructed onto the stack, at the end of any recursion a single type is on
  // the astJavaTypeStack to represent that level of the recursion.

     if (SgProject::get_verbose() > 0)
          printf ("Inside of Java_JavaParser_cactionGenerateType() \n");

     outputJavaState("At TOP of cactionGenerateType");

     SgName name = convertJavaStringToCxxString(env,java_string);
  // printf ("Java_JavaParser_cactionGenerateType(): name = %s \n",name.str());

  // Type details (from http://java.sun.com/docs/books/jls/second_edition/html/jTOC.doc.html):
  // boolean
  //    1-bit. May take on the values true and false only. true and false are defined constants of the language 
  //    and are not the same as True and False, TRUE and FALSE, zero and nonzero, 1 and 0 or any other numeric 
  //    value. Booleans may not be cast into any other type of variable nor may any other variable be cast into 
  //    a boolean.
  // byte
  //    1 signed byte (two's complement). Covers values from -128 to 127.
  // short
  //    2 bytes, signed (two's complement), -32,768 to 32,767
  // int
  //    4 bytes, signed (two's complement). -2,147,483,648 to 2,147,483,647. Like all numeric types ints may be 
  //    cast into other numeric types (byte, short, long, float, double). When lossy casts are done (e.g. int 
  //    to byte) the conversion is done modulo the length of the smaller type.
  // long
  //    8 bytes signed (two's complement). Ranges from -9,223,372,036,854,775,808 to +9,223,372,036,854,775,807.
  // float
  //    4 bytes, IEEE 754. Covers a range from 1.40129846432481707e-45 to 3.40282346638528860e+38 (positive or negative).
  //    Like all numeric types floats may be cast into other numeric types (byte, short, long, int, double). When 
  //    lossy casts to integer types are done (e.g. float to short) the fractional part is truncated and the conversion is done modulo the length of the smaller type.
  // double
  //    8 bytes IEEE 754. Covers a range from 4.94065645841246544e-324d to 1.79769313486231570e+308d (positive or negative). 
  // char
  //    2 bytes, unsigned, Unicode, 0 to 65,535 Chars are not the same as bytes, ints, shorts or Strings.

     if (name == "boolean")
        {
       // This is a logical true/false value (it's bit widths in implementation dependent.
       // astJavaTypeStack.push_front(SgTypeInt::createType());
          astJavaTypeStack.push_front(SgTypeBool::createType());
        }
       else if (name == "byte")
        {
       // DQ (4/3/2011): In Java the type "byte" is signed!
       // Reference: http://www.javamex.com/java_equivalents/unsigned.shtml
       // astJavaTypeStack.push_front(SgTypeUnsignedChar::createType());
          astJavaTypeStack.push_front(SgTypeSignedChar::createType());
        }
       else if (name == "char")
        {
       // In Java, all integers are signed, except for "char". However a "char" is 2 byte unicode so it might be better for it to be SgTypeWchar.
       // astJavaTypeStack.push_front(SgTypeChar::createType());
       // astJavaTypeStack.push_front(SgTypeUnsignedChar::createType());
          astJavaTypeStack.push_front(SgTypeWchar::createType());
        }
       else if (name == "int")
        {
       // This should be a 32-bit type, but ROSE does not specify the bit length explictly (we could us 32-bit field widths, I suppose).
          astJavaTypeStack.push_front(SgTypeInt::createType());
        }
       else if (name == "short")
        {
       // This is a 2 byte signed type.
          astJavaTypeStack.push_front(SgTypeShort::createType());
        }
       else if (name == "float")
        {
       // This is a 4 byte floating point type.
          astJavaTypeStack.push_front(SgTypeFloat::createType());
        }
       else if (name == "long")
        {
       // This is a 8 byte signed type.
          astJavaTypeStack.push_front(SgTypeLong::createType());
        }
       else if (name == "double")
        {
       // This is an 8 byte floating point type.
          astJavaTypeStack.push_front(SgTypeDouble::createType());
        }
       else if (name == "void")
        {
       // DQ (4/5/2011): Added support for "void" type (also used as return type fo constructors).
       // This is the same sort of void type as in C/C++.
          astJavaTypeStack.push_front(SgTypeVoid::createType());
        }
       else if (name == "null")
        {
       // There is also a special null type, the type of the expression null, which has no name. Because the null type has 
       // no name, it is impossible to declare a variable of the null type or to cast to the null type. The null reference 
       // is the only possible value of an expression of null type. The null reference can always be cast to any reference 
       // type. In practice, the programmer can ignore the null type and just pretend that null is merely a special literal 
       // that can be of any reference type.

       // Within ROSE it is not yet clear if I should define a new SgType (SgTypeNull) to represent a null type.  For now it
       // is an error to try to build such a type.

          printf ("Error: SgTypeNull (Java null type) support not implemented (name = %s) \n",name.str());
          ROSE_ASSERT(false);
        }
       else 
        {
       // DQ (8/17/2011): This should be any implicit type defined in Java (so test for the prefix "java.").
       // else if (name == "String")
       // else if (name == "java.lang.String")

       // Strip off the suffix "#RAW" on the class name that is an artifact of Java versions 1.5 and greater???
          name = processNameOfRawType(name);
          string nameString = name;

       // Check if the typename starts with "java."
          if (nameString.find("java.",0) == 0)
             {
            // DQ (4/9/2011): Added support for "String" type.
            // This should maybe be a SgClassType specific to the String class, instead of using the SgTypeString for Java.
            // astJavaTypeStack.push_front(SgTypeString::createType());

               SgClassType* classType = lookupTypeFromQualifiedName(name);

            // printf ("In Java_JavaParser_cactionGenerateType(): classType = %p \n",classType);
               if (classType == NULL)
                  {
                 // If the "String" class was not found then it is likely because we are in a debug mode which limits the number of implecit classes.
                 // printf ("Build a class for java.<class name>.<type name>: name = %s\n",name.str());
                    outputJavaState("In cactionGenerateType case of java.lang.<type name>");
#if 1
                 // We build the class but we need the declarations in the class.
                    buildImplicitClass(name);
                 // printf ("DONE: Build a class for java.<class name>.<type name>: name = %s\n",name.str());
#else
                    printf ("Skipping call to buildImplicitClass(name = %s); \n",name.str());
#endif
                    outputJavaState("DONE: In cactionGenerateType case of java.lang.<type name>");

                 // DQ (8/20/2011): The new class is on the stack, we want to get it's data members into place so that they will be available to be called.
                 // Can we call Java to force the members to be traversed?  Not clear how to do this!

                 // We need to leave a SgType on the astJavaTypeStack, we need to build the class to build 
                 // the SgClassType, but we don't want to leave a SgClassDefinition on the astJavaScopeStack.
                 // printf ("When we just build a type we don't want the new class definition on the stack. \n");
                    astJavaScopeStack.pop_front();

                    SgClassType* classType = lookupTypeFromQualifiedName(name);
                    if (classType == NULL)
                       {
                         printf ("Build a SgTypeInt in the stack since the class was not built (debugging mode): name = %s\n",name.str());
                         astJavaTypeStack.push_front(SgTypeInt::createType());
                       }
                      else
                       {
                      // The associated type was found (after being explicitly built, after not being found the first time)) and is pushed onto the stack.
                      // printf ("On the second search for the class = %s (after building it explicitly) it was found! \n",name.str());
                         astJavaTypeStack.push_front(classType);
                       }
                  }
                 else
                  {
                 // The associated type was found and is pushed onto the stack.
                    astJavaTypeStack.push_front(classType);
                  }

#if 0
               printf ("Exiting as a test in Java_JavaParser_cactionGenerateType() \n");
               ROSE_ASSERT(false);
#endif
             }
            else
             {
            // This is the last chance to resolve the type.
               if (SgProject::get_verbose() > 0)
                    printf ("I think this is a user defined class (so we have to look it up) (name = %s) \n",name.str());

               SgClassType* classType = lookupTypeFromQualifiedName(name);
               ROSE_ASSERT(classType != NULL);

               astJavaTypeStack.push_front(classType);
#if 0
               printf ("Error: default reached in switch in Java_JavaParser_cactionGenerateType() (name = %s) \n",name.str());
               ROSE_ASSERT(false);
#endif
             }
        }

  // There should be a type on the stack!
     ROSE_ASSERT(astJavaTypeStack.empty() == false);

  // printf ("Leaving Java_JavaParser_cactionGenerateType() \n");

     outputJavaState("At BOTTOM of cactionGenerateType");

#if 0
     printf ("Build support for types (not finished) \n");
     ROSE_ASSERT(false);
#endif
   }

// JNIEXPORT void JNICALL Java_JavaParser_cactionGenerateClassType (JNIEnv* env, jclass xxx, jstring java_string, jobject jToken)
JNIEXPORT void JNICALL Java_JavaParser_cactionGenerateClassType (JNIEnv* env, jclass xxx, jstring java_string)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Inside of Java_JavaParser_cactionGenerateClassType() \n");

     SgName name = convertJavaStringToCxxString(env,java_string);

  // printf ("In Java_JavaParser_cactionGenerateClassType(): Calling lookupSymbolFromQualifiedName(name = %s) \n",name.str());
     SgClassType* classType = lookupTypeFromQualifiedName(name);

  // ROSE_ASSERT(classType != NULL);
     if (classType != NULL)
        {
          astJavaTypeStack.push_front(classType);
        }
       else
        {
          printf ("WARNING: SgClassSymbol NOT FOUND in Java_JavaParser_cactionGenerateClassType(): name = %s (build an integer type and keep going...) \n",name.str());
          astJavaTypeStack.push_front(SgTypeInt::createType());
        }

     outputJavaState("At Bottom of cactionGenerateClassType");

#if 0
     printf ("Build support for class types (not finished) \n");
     ROSE_ASSERT(false);
#endif
   }

JNIEXPORT void JNICALL Java_JavaParser_cactionStatementEnd(JNIEnv *env, jclass xxx, jstring java_string /* JNIEnv *env, jobject xxx */ , jobject jToken)
   {
     string label = convertJavaStringToCxxString(env,java_string);

     if (SgProject::get_verbose() > 0)
        printf ("Closing actions for statements label = %s\n",label.c_str());

     outputJavaState("At Top of cactionStatementEnd()");

  // DQ (7/30/2011): We now process the scopes to collect statements and pop off the exact 
  // number required no more, as a result we need not process each statement separately 
  // (some ere process and some were not and this interfered with the scope based processing 
  // of statments).

     outputJavaState("At Bottom of cactionStatementEnd()");

  // DQ (4/6/2011): these stacks should be cleared now.
     ROSE_ASSERT(astJavaNodeStack.empty() == true);
     ROSE_ASSERT(astJavaInitializedNameStack.empty() == true);

  // DQ (7/16/2011): Note that there could be a predicate on the expression stack 
  // associated with an SgIfStmt that is not finished being constructed.
  // ROSE_ASSERT(astJavaExpressionStack.empty() == true);

  // We want to at some point make this an error, but for now just a warning.
     if (astJavaTypeStack.empty() == false)
        {
          printf ("WARNING: clearing the astJavaTypeStack size = %zu \n",astJavaTypeStack.size());
          astJavaTypeStack.clear();
        }
  // ROSE_ASSERT(astJavaTypeStack.empty() == true);
   }



// JNIEXPORT void JNICALL Java_JavaParser_cactionGenerateArrayType(JNIEnv *env, jclass xxx, jobject jToken)
JNIEXPORT void JNICALL Java_JavaParser_cactionGenerateArrayType(JNIEnv *env, jclass xxx)
   {
     if (SgProject::get_verbose() > 2)
          printf ("Build an array type using the base type found on the type stack \n");

     outputJavaState("At TOP of cactionGenerateArrayType");

     ROSE_ASSERT(astJavaTypeStack.empty() == false);
     SgArrayType* arrayType = SageBuilder::buildArrayType(astJavaTypeStack.front(),NULL);

     ROSE_ASSERT(arrayType != NULL);
     astJavaTypeStack.pop_front();

     astJavaTypeStack.push_front(arrayType);

     outputJavaState("At BOTTOM of cactionGenerateArrayType");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionAllocationExpression(JNIEnv *env, jobject xxx, jobject jToken)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Inside of Java_JavaParser_cactionAllocationExpression() \n");

     outputJavaState("At TOP of cactionAllocationExpression");
   }

JNIEXPORT void JNICALL Java_JavaParser_cactionAllocationExpressionEnd(JNIEnv *env, jobject xxx, jstring java_string, jobject jToken)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Inside of Java_JavaParser_cactionAllocationExpressionEnd() \n");

     outputJavaState("At TOP of cactionAllocationExpressionEnd");

     SgName name = convertJavaStringToCxxString(env,java_string);

     printf ("Build a new operator for type name = %s \n",name.str());

     ROSE_ASSERT(astJavaTypeStack.empty() == false);
     SgType* type = astJavaTypeStack.front();
     astJavaTypeStack.pop_front();

  // For the simple case, we only need the type as input to build SgNewExp.
     SgExprListExp* exprListExp          = NULL;
     SgConstructorInitializer* constInit = NULL;
     SgExpression* expr                  = NULL;
     short int val                       = 0;
     SgFunctionDeclaration* funcDecl     = NULL;

     SgNewExp* newExpression = SageBuilder::buildNewExp(type, exprListExp, constInit, expr, val, funcDecl);
     ROSE_ASSERT(newExpression != NULL);

     setJavaSourcePosition(newExpression,env,jToken);

     astJavaExpressionStack.push_front(newExpression);

     outputJavaState("At BOTTOM of cactionAllocationExpressionEnd");
   }

JNIEXPORT void JNICALL Java_JavaParser_cactionANDANDExpression(JNIEnv *env, jobject xxx, jobject jToken)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Inside of Java_JavaParser_cactionANDANDExpression() \n");

     outputJavaState("At TOP of cactionANDANDExpression");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionANDANDExpressionEnd(JNIEnv *env, jobject xxx, jobject jToken)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Inside of Java_JavaParser_cactionANDANDExpressionEnd() \n");

     outputJavaState("At TOP of cactionANDANDExpressionEnd");

     binaryExpressionSupport<SgAndOp>();

     ROSE_ASSERT(astJavaExpressionStack.empty() == false);
     setJavaSourcePosition(astJavaExpressionStack.front(),env,jToken);

     outputJavaState("At BOTTOM of cactionANDANDExpressionEnd");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionAnnotationMethodDeclaration(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionArgumentClassScope(JNIEnv *, jobject, jstring, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionArrayAllocationExpression(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionArrayInitializer(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionArrayQualifiedTypeReference(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionArrayQualifiedTypeReferenceClassScope(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionArrayReference(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionArrayTypeReferenceClassScope(JNIEnv *, jobject, jstring, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionAssertStatement(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionAssignment(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionAssignmentEnd(JNIEnv *env, jobject xxx, jobject jToken)
   {
  // This function builds an assignement statement (not an expression).

     if (SgProject::get_verbose() > 2)
          printf ("Build an assignement statement (expression?) \n");

     outputJavaState("At TOP of cactionAssignmentEnd");

     ROSE_ASSERT(astJavaExpressionStack.empty() == false);
     ROSE_ASSERT(astJavaExpressionStack.size() >= 2);

     SgExpression* rhs = astJavaExpressionStack.front();
     ROSE_ASSERT(rhs != NULL);
     astJavaExpressionStack.pop_front();

     SgExpression* lhs = astJavaExpressionStack.front();
     ROSE_ASSERT(lhs != NULL);
     astJavaExpressionStack.pop_front();

  // This is an assignement statement so we have to build a statement and push it onto the stack.

  // DQ (8/16/2011): I had to build a special version of this function to avoid the recursive call
  // in SageBuilder::buildAssignStatement() which will reset the file info as a transformation.
  // This is a bug in the buildAssignStatement() function but will likely breack other code if we 
  // fix it now.  This will have to be revisitied later.
  // SgExprStatement* assignmentStatement = SageBuilder::buildAssignStatement(lhs,rhs);
     SgExprStatement* assignmentStatement = SageBuilder::buildAssignStatement_ast_translate(lhs,rhs);
     ROSE_ASSERT(assignmentStatement != NULL);

     setJavaSourcePosition(assignmentStatement,env,jToken);
     ROSE_ASSERT(assignmentStatement->get_expression() != NULL);
     setJavaSourcePosition(assignmentStatement->get_expression(),env,jToken);

     astJavaStatementStack.push_front(assignmentStatement);

     outputJavaState("At BOTTOM of cactionAssignmentEnd");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionBinaryExpression(JNIEnv *env, jobject xxx, jobject jToken)
   {
  // I don't think we need this function.
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionBinaryExpressionEnd(JNIEnv *env, jobject xxx, jint java_operator_kind, jobject jToken)
   {
     if (SgProject::get_verbose() > 2)
          printf ("Build an Binary Expression End \n");

     outputJavaState("At TOP of cactionBinaryExpressionEnd");

  // These are the operator code values directly from ECJ.
     enum ops
        {
          ERROR_OPERATOR       = 0, // This is not a ECJ value 
          AND                  = 2,
          DIVIDE               = 9, 
          GREATER              = 6,
          GREATER_EQUAL        = 7, 
          LEFT_SHIFT           = 10, 
          LESS                 = 4, 
          LESS_EQUAL           = 5, 
          MINUS                = 13, 
          MULTIPLY             = 15, 
          OR                   = 3,
          PLUS                 = 14,  
          REMAINDER            = 16, 
          RIGHT_SHIFT          = 17, 
          UNSIGNED_RIGHT_SHIFT = 19, 
          XOR                  = 8,
          OR_OR                = 100, // Handled by separate function 
          AND_AND              = 101, // Handled by separate function 
          LAST_OPERATOR };

  // printf ("PLUS = %d \n",PLUS);

     int operator_kind = java_operator_kind;
  // printf ("operator_kind = %d \n",operator_kind);

     switch(operator_kind)
        {
       // Operator codes used by the BinaryExpression in ECJ.
          case LESS:                 binaryExpressionSupport<SgLessThanOp>();       break;
          case LESS_EQUAL:           binaryExpressionSupport<SgLessOrEqualOp>();    break;
          case GREATER:              binaryExpressionSupport<SgGreaterThanOp>();    break;
          case GREATER_EQUAL:        binaryExpressionSupport<SgGreaterOrEqualOp>(); break;
          case AND:                  binaryExpressionSupport<SgBitAndOp>();         break;
          case OR:                   binaryExpressionSupport<SgBitOrOp>();          break;
          case XOR:                  binaryExpressionSupport<SgBitXorOp>();         break;
          case DIVIDE:               binaryExpressionSupport<SgDivideOp>();         break;
          case MINUS:                binaryExpressionSupport<SgSubtractOp>();       break;
          case PLUS:                 binaryExpressionSupport<SgAddOp>();            break;
          case MULTIPLY:             binaryExpressionSupport<SgMultiplyOp>();       break;
          case RIGHT_SHIFT:          binaryExpressionSupport<SgRshiftOp>();         break;
          case LEFT_SHIFT:           binaryExpressionSupport<SgLshiftOp>();         break;
          case REMAINDER:            binaryExpressionSupport<SgModOp>();            break;

       // This may have to handled special in ROSE. ROSE does not represent the semantics,
       // and so this support my require a special operator to support Java in ROSE. For
       // now we will use the more common SgRshiftOp.
          case UNSIGNED_RIGHT_SHIFT: binaryExpressionSupport<SgJavaUnsignedRshiftOp>();         break;

       // These are handled through separate functions (not a BinaryExpression in ECJ).
          case OR_OR:   ROSE_ASSERT(false); break;
          case AND_AND: ROSE_ASSERT(false); break;

          default:
             {
               printf ("Error: default reached in cactionBinaryExpressionEnd() operator_kind = %d \n",operator_kind);
               ROSE_ASSERT(false);
             }
        }

     ROSE_ASSERT(astJavaExpressionStack.empty() == false);
     setJavaSourcePosition(astJavaExpressionStack.front(),env,jToken);

     outputJavaState("At BOTTOM of cactionBinaryExpressionEnd");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionBlock(JNIEnv *env, jobject xxx, jobject jToken)
   {
     if (SgProject::get_verbose() > 2)
          printf ("Build an SgBasicBlock scope \n");

     outputJavaState("At TOP of cactionBlock");

  // There could be a conditional from an IF statement on the stack.
  // ROSE_ASSERT(astJavaExpressionStack.empty() == true);

  // If there is an expression on the expression stack and an SgIfStmt on the scope stack then 
  // this might be a good time to associate the conditional with the SgIfStmt and have a more
  // enforceble rules going forward.  But then there might not be a SgBasicBlock, so don't do this.

  // Since we build the true body when we build the ifStmt, we need to detect and reuse this 
  // SgBasicBlock instead of building a new one.
  // SgBasicBlock* block = SageBuilder::buildBasicBlock();
     SgBasicBlock* block = NULL;
     SgIfStmt* ifStatement = isSgIfStmt(astJavaScopeStack.front());
     if (ifStatement != NULL)
        {
          SgNullStatement* nullStatement = isSgNullStatement(ifStatement->get_true_body());
          if (nullStatement != NULL)
             {
            // block = ifStatement->get_true_body();
               block = SageBuilder::buildBasicBlock();
               ROSE_ASSERT(block != NULL);
               ifStatement->set_true_body(block);

               delete nullStatement;
             }
            else
             {
            // Set the false body
               block = SageBuilder::buildBasicBlock();
               ROSE_ASSERT(block != NULL);
               ifStatement->set_false_body(block);
             }
        }
       else
        {
       // DQ (7/30/2011): Handle the case of a block after a SgForStatement
       // Because we build the SgForStatement on the stack and then the cactionBlock 
       // function is called, we have to detect and fixup the SgForStatement.
          SgForStatement* forStatement = isSgForStatement(astJavaScopeStack.front());
          if (forStatement != NULL)
             {
               SgNullStatement* nullStatement = isSgNullStatement(forStatement->get_loop_body());
               if (nullStatement != NULL)
                  {
                    block = SageBuilder::buildBasicBlock();
                    ROSE_ASSERT(block != NULL);
                    forStatement->set_loop_body(block);
                    delete nullStatement;
                  }
             }
            else
             {
               block = SageBuilder::buildBasicBlock();
               ROSE_ASSERT(block != NULL);
             }
        }
     ROSE_ASSERT(block != NULL);

     setJavaSourcePosition(block,env,jToken);

     block->set_parent(astJavaScopeStack.front());
     ROSE_ASSERT(block->get_parent() != NULL);

     astJavaScopeStack.push_front(block);

     outputJavaState("At BOTTOM of cactionBlock");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionBlockEnd(JNIEnv *env, jobject xxx, jint java_numberOfStatements, jobject jToken)
   {
     if (SgProject::get_verbose() > 2)
          printf ("Pop the current SgBasicBlock scope off the scope stack...\n");

     outputJavaState("At TOP of cactionBlockEnd");

     int numberOfStatements = java_numberOfStatements;

     if (SgProject::get_verbose() > 2)
          printf ("In cactionBlockEnd(): numberOfStatements = %d \n",numberOfStatements);

     ROSE_ASSERT(astJavaScopeStack.empty() == false);

     appendStatementStack(numberOfStatements);

  // DQ (7/30/2011): We only pop a pricise number of statments off the stack, so that can still be statements left.
  // ROSE_ASSERT(astJavaStatementStack.empty() == true);

  // DQ (7/30/2011): Take the block off of the scope stack and put it onto the statement stack so that we can 
  // process either blocks of other statements uniformally.
     SgStatement* body = astJavaScopeStack.front();
     ROSE_ASSERT(body != NULL);
     astJavaStatementStack.push_front(body);

     astJavaScopeStack.pop_front();

     outputJavaState("At BOTTOM of cactionBlockEnd");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionBreakStatement(JNIEnv *env, jobject xxx, jstring java_string, jobject jToken)
   {
     SgBreakStmt* stmt = SageBuilder::buildBreakStmt();
     ROSE_ASSERT(stmt != NULL);

     string label_name = convertJavaStringToCxxString(env, java_string);
     if (label_name.length() > 0) {
         stmt -> set_do_string_label(label_name);
     }

     astJavaStatementStack.push_front(stmt);
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionCaseStatement(JNIEnv *env, jobject xxx, jboolean hasCaseExpression, jobject jToken)
   {
     if (SgProject::get_verbose() > 2)
          printf ("Inside of Java_JavaParser_cactionCaseStatement() \n");

     outputJavaState("At TOP of cactionCaseStatement");

  // 
  // We build on the way down because the scope information and symbol table information is contained
  // in the Ast node.  This AST node is a subclass of SgScopeStatement
  //
     SgStatement *caseStatement = NULL;
     if (hasCaseExpression) {
         caseStatement = SageBuilder::buildCaseOptionStmt(); // the body will be added later
     }
     else {
         caseStatement = SageBuilder::buildDefaultOptionStmt(); // the body will be added later
     }
     ROSE_ASSERT(caseStatement != NULL);

     setJavaSourcePosition(caseStatement,env,jToken);

  // DQ (7/30/2011): For the build interface to work we have to initialize the parent pointer to the SgForStatement.
  // PC (8/23/2011): When and why parent pointers should be set needs to be clarified. Perhaps the SageBuilder
  // functions should be revisited?
     caseStatement->set_parent(astJavaScopeStack.front());

     outputJavaState("At BOTTOM of cactionCaseStatement");
   }

JNIEXPORT void JNICALL Java_JavaParser_cactionCaseStatementEnd(JNIEnv *env, jobject xxx, jboolean hasCaseExpression, jobject jToken)
   {
    if (SgProject::get_verbose() > 2)
         printf ("Inside of Java_JavaParser_cactionCaseStatementEnd() \n");

    outputJavaState("At TOP of cactionCaseStatementEnd");

    SgStatement *case_statement = NULL;
    // update the Case Statement
    if (hasCaseExpression) {
        SgExpression *case_expression = (SgExpression *) astJavaExpressionStack.front();
        astJavaExpressionStack.pop_front();
        case_statement = SageBuilder::buildCaseOptionStmt(case_expression, NULL); // the body will be added later
    }
    else {
        case_statement = SageBuilder::buildDefaultOptionStmt(NULL); // the body will be added later
    }

    // Pushing 'case' on the statement stack
    astJavaStatementStack.push_front(case_statement);
    outputJavaState("At BOTTOM of cactionCaseStatementEnd");
}

JNIEXPORT void JNICALL Java_JavaParser_cactionCastExpression(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionCastExpressionEnd(JNIEnv *env, jobject xxx, jobject jToken)
   {
     if (SgProject::get_verbose() > 2)
          printf ("Inside of Java_JavaParser_cactionCastExpressionEnd() \n");

     outputJavaState("At TOP of cactionCastExpressionEnd");

     ROSE_ASSERT(astJavaTypeStack.empty() == false);
     SgType* castType = astJavaTypeStack.front();
     astJavaTypeStack.pop_front();

     ROSE_ASSERT(astJavaExpressionStack.empty() == false);
     SgExpression* castExpression = astJavaExpressionStack.front();
     astJavaExpressionStack.pop_front();

     SgCastExp* castExp = SageBuilder::buildCastExp(castExpression, castType);
     ROSE_ASSERT(castExp != NULL);

     astJavaExpressionStack.push_front(castExp);

     outputJavaState("At BOTTOM of cactionCastExpressionEnd");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionCharLiteral(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionClassLiteralAccess(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionClinit(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionConditionalExpression(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionConditionalExpressionEnd(JNIEnv *env, jobject xxx, jobject jToken)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Inside of Java_JavaParser_cactionConditionalExpressionEnd() \n");

     outputJavaState("At TOP of cactionConditionalExpressionEnd");

     ROSE_ASSERT(astJavaExpressionStack.empty() == false);
     ROSE_ASSERT(astJavaExpressionStack.size() >= 3);

     SgExpression* false_exp = astJavaExpressionStack.front();
     ROSE_ASSERT(false_exp != NULL);
     astJavaExpressionStack.pop_front();

     SgExpression* true_exp = astJavaExpressionStack.front();
     ROSE_ASSERT(true_exp != NULL);
     astJavaExpressionStack.pop_front();

     SgExpression* test_exp = astJavaExpressionStack.front();
     ROSE_ASSERT(test_exp != NULL);
     astJavaExpressionStack.pop_front();

  // Build the assignment operator and push it onto the stack.
     SgConditionalExp* conditional = SageBuilder::buildConditionalExp(test_exp,true_exp,false_exp);
     ROSE_ASSERT(conditional != NULL);
     astJavaExpressionStack.push_front(conditional);
     ROSE_ASSERT(astJavaExpressionStack.empty() == false);

     outputJavaState("At BOTTOM of cactionConditionalExpressionEnd");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionContinueStatement(JNIEnv *env, jobject xxx, jstring java_string, jobject jToken)
   {
     SgContinueStmt* stmt = SageBuilder::buildContinueStmt();
     ROSE_ASSERT(stmt != NULL);

     string label_name = convertJavaStringToCxxString(env,java_string);
     if (label_name.length() > 0) {
         stmt -> set_do_string_label(label_name);
     }

     astJavaStatementStack.push_front(stmt);
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionCompoundAssignment(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionCompoundAssignmentEnd(JNIEnv *env, jobject xxx, jint java_operator_kind, jobject jToken)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Inside of Java_JavaParser_cactionCompoundAssignmentEnd() \n");

     outputJavaState("At TOP of cactionCompoundAssignmentEnd");

  // These are the operator code values directly from ECJ.
     enum ops
        {
          ERROR_OPERATOR       = 0, // This is not a ECJ value 
          AND                  = 2,
          DIVIDE               = 9, 
          LEFT_SHIFT           = 10, 
          MINUS                = 13, 
          MULTIPLY             = 15, 
          OR                   = 3,
          PLUS                 = 14,  
          REMAINDER            = 16, 
          RIGHT_SHIFT          = 17, 
          UNSIGNED_RIGHT_SHIFT = 19, 
          XOR                  = 8,
          LAST_OPERATOR };

     int operator_kind = java_operator_kind;
  // printf ("operator_kind = %d \n",operator_kind);

     switch(operator_kind)
        {
       // Operator codes used by the CompoundAssignment in ECJ.
          case PLUS:        binaryAssignmentStatementSupport<SgPlusAssignOp>();   break;
          case MINUS:       binaryAssignmentStatementSupport<SgMinusAssignOp>();  break;
          case DIVIDE:      binaryAssignmentStatementSupport<SgDivAssignOp>();    break;
          case MULTIPLY:    binaryAssignmentStatementSupport<SgMultAssignOp>();   break;
          case OR:          binaryAssignmentStatementSupport<SgIorAssignOp>();    break;
          case AND:         binaryAssignmentStatementSupport<SgAndAssignOp>();    break;
          case XOR:         binaryAssignmentStatementSupport<SgXorAssignOp>();    break;
          case REMAINDER:   binaryAssignmentStatementSupport<SgModAssignOp>();    break;
          case RIGHT_SHIFT: binaryAssignmentStatementSupport<SgRshiftAssignOp>(); break;
          case LEFT_SHIFT:  binaryAssignmentStatementSupport<SgLshiftAssignOp>(); break;

       // This may have to handled special in ROSE. ROSE does not represent the semantics,
       // and so this support my require a special operator to support Java in ROSE. For
       // now we will use the more common SgRshiftOp.
          case UNSIGNED_RIGHT_SHIFT: binaryAssignmentStatementSupport<SgJavaUnsignedRshiftAssignOp>(); break;

          default:
             {
               printf ("Error: default reached in cactionCompoundAssignmentEnd() operator_kind = %d \n",operator_kind);
               ROSE_ASSERT(false);
             }
        }

     ROSE_ASSERT(astJavaStatementStack.empty() == false);
     setJavaSourcePosition(astJavaStatementStack.front(),env,jToken);

  // Also set the source position of the nested expression in the expression statement.
     SgExprStatement* exprStatement = isSgExprStatement(astJavaStatementStack.front());
     if (exprStatement != NULL)
        {
          setJavaSourcePosition(exprStatement,env,jToken);
        }

     outputJavaState("At BOTTOM of cactionCompoundAssignmentEnd");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionDoStatement(JNIEnv *env, jobject xxx, jobject jToken)
   {
     if (SgProject::get_verbose() > 2)
          printf ("Inside of Java_JavaParser_cactionDoStatement() \n");

     outputJavaState("At TOP of cactionDoStatement");

     SgNullStatement*  testStatement       = SageBuilder::buildNullStatement();
     SgNullStatement*  bodyStatement       = SageBuilder::buildNullStatement();

  // It might be that we should build this on the way down so that we can have it on the stack 
  // before the body would be pushed onto the scope stack if a block is used.
  // SgForStatement* forStatement = SageBuilder::buildForStatement(assignmentStatement,testStatement,incrementExpression,bodyStatement);
     SgDoWhileStmt* doWhileStatement = SageBuilder::buildDoWhileStmt(bodyStatement,testStatement);
     ROSE_ASSERT(doWhileStatement != NULL);

  // DQ (7/30/2011): For the build interface to work we have to initialize the parent pointer to the SgForStatement.
     doWhileStatement->set_parent(astJavaScopeStack.front());

     astJavaScopeStack.push_front(doWhileStatement);

     outputJavaState("At BOTTOM of cactionDoStatement");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionDoStatementEnd(JNIEnv *env, jobject xxx, jobject jToken)
   {
     if (SgProject::get_verbose() > 2)
          printf ("Inside of Java_JavaParser_cactionDoStatementEnd() \n");

     outputJavaState("At TOP of cactionDoStatementEnd");

  // If we DO put all body's onto the statement stack then we process it this way.
     ROSE_ASSERT(astJavaStatementStack.empty() == false);
     SgStatement* bodyStatement = astJavaStatementStack.front();
     ROSE_ASSERT(bodyStatement != NULL);
     astJavaStatementStack.pop_front();

     ROSE_ASSERT(astJavaExpressionStack.empty() == false);
     SgExpression* testExpression = astJavaExpressionStack.front();
     ROSE_ASSERT(testExpression != NULL);
     astJavaExpressionStack.pop_front();

     SgExprStatement* testStatement = SageBuilder::buildExprStatement(testExpression);

     setJavaSourcePosition(testExpression,env,jToken);
     setJavaSourcePosition(testStatement,env,jToken);

     SgDoWhileStmt* originalDoWhileStatement = isSgDoWhileStmt(astJavaScopeStack.front());
     ROSE_ASSERT(originalDoWhileStatement != NULL);

     originalDoWhileStatement->set_body(bodyStatement);
     bodyStatement->set_parent(originalDoWhileStatement);
     ROSE_ASSERT(bodyStatement->get_parent() == originalDoWhileStatement);

     originalDoWhileStatement->set_condition(testStatement);
     testStatement->set_parent(originalDoWhileStatement);
     ROSE_ASSERT(testStatement->get_parent() == originalDoWhileStatement);

     setJavaSourcePosition(originalDoWhileStatement,env,jToken);

     astJavaStatementStack.push_front(originalDoWhileStatement);

  // Remove the SgWhileStmt on the scope stack...
     astJavaScopeStack.pop_front();

     outputJavaState("At BOTTOM of cactionDoStatementEnd");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionDoubleLiteral(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionEmptyStatement(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionEqualExpression(JNIEnv *env, jobject xxx, jobject jToken)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Inside of Java_JavaParser_cactionEqualExpression() \n");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionEqualExpressionEnd(JNIEnv *env, jobject xxx, jint java_operator_kind, jobject jToken)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Inside of Java_JavaParser_cactionEqualExpressionEnd() \n");

     outputJavaState("At TOP of cactionEqualExpressionEnd");

  // These are the operator code values directly from ECJ.
     enum ops
        {
          ERROR_OPERATOR = 0, // This is not a ECJ value 
          EQUAL_EQUAL    = 18,
          NOT_EQUAL      = 29,
          LAST_OPERATOR };

     int operator_kind = java_operator_kind;
  // printf ("operator_kind = %d \n",operator_kind);

     switch(operator_kind)
        {
       // Operator codes used by the UnaryExpression in ECJ.
          case EQUAL_EQUAL: binaryExpressionSupport<SgEqualityOp>(); break;
          case NOT_EQUAL:   binaryExpressionSupport<SgNotEqualOp>(); break;

          default:
             {
               printf ("Error: default reached in cactionEqualExpressionEnd() operator_kind = %d \n",operator_kind);
               ROSE_ASSERT(false);
             }
        }

     ROSE_ASSERT(astJavaExpressionStack.empty() == false);
     setJavaSourcePosition(astJavaExpressionStack.front(),env,jToken);

     outputJavaState("At BOTTOM of cactionEqualExpressionEnd");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionExtendedStringLiteral(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionFalseLiteral(JNIEnv *env, jobject xxx, jobject jToken)
   {
     SgExpression* expression = SageBuilder::buildBoolValExp(false);
     astJavaExpressionStack.push_front(expression);
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionFieldDeclaration(JNIEnv *env, jobject xxx, jstring variableName, jboolean java_hasInitializer,
     jboolean java_is_final, jboolean java_is_private, jboolean java_is_protected, jboolean java_is_public, 
     jboolean java_is_volatile, jboolean java_is_synthetic, jboolean java_is_static, jboolean java_is_transient, jobject jToken)
   {
     if (SgProject::get_verbose() > 2)
          printf ("Inside of Java_JavaParser_cactionFieldDeclaration() \n");

     outputJavaState("At TOP of cactionFieldDeclaration");

     SgName name = convertJavaStringToCxxString(env,variableName);

  // I think we need this in the endVisit() function (and not here).
     bool hasInitializer = java_hasInitializer;

     if (SgProject::get_verbose() > 2)
          printf ("hasInitializer = %s (but not used except in bottom up processing) \n",hasInitializer ? "true" : "false");

     bool isFinal     = java_is_final;
     bool isPrivate   = java_is_private;
     bool isProtected = java_is_protected;
     bool isPublic    = java_is_public;
     bool isVolatile  = java_is_volatile;
     bool isSynthetic = java_is_synthetic;
     bool isStatic    = java_is_static;
     bool isTransient = java_is_transient;

     if (SgProject::get_verbose() > 2)
          printf ("Building a variable declaration for name = %s \n",name.str());

  // Note that the type shuld have already been built and should be on the astJavaTypeStack.
     SgVariableDeclaration* variableDeclaration = buildSimpleVariableDeclaration(name);
     ROSE_ASSERT(variableDeclaration != NULL);

  // DQ (8/13/2011): Added test for scope
     ROSE_ASSERT(variableDeclaration->get_scope() != NULL);

  // DQ (8/13/2011): This is a test to debug failing test in resetParentPointers.C:1733
     ROSE_ASSERT(SageInterface::is_Fortran_language() == false);
     SgInitializedName* initializedName = variableDeclaration->get_decl_item(name);
     ROSE_ASSERT(initializedName != NULL);
     ROSE_ASSERT(initializedName->get_scope() != NULL);

     setJavaSourcePosition(initializedName,env,jToken);
     setJavaSourcePosition(variableDeclaration,env,jToken);

  // Save it on the stack so that we can add SgInitializedNames to it.
     astJavaStatementStack.push_front(variableDeclaration);

  // Set the modifiers (shared between PHP and Java)
     if (isFinal == true)
          variableDeclaration->get_declarationModifier().setFinal();

  // DQ (8/13/2011): Added modifier support.
     if (isPrivate == true)
        {
          if (SgProject::get_verbose() > 2)
               printf ("Setting modifier as Private \n");
          variableDeclaration->get_declarationModifier().get_accessModifier().setPrivate();
        }

     if (isProtected == true)
        {
          if (SgProject::get_verbose() > 2)
               printf ("Setting modifier as Protected \n");
          variableDeclaration->get_declarationModifier().get_accessModifier().setProtected();
        }

     if (isPublic == true)
        {
          if (SgProject::get_verbose() > 2)
               printf ("Setting modifier as Public \n");
          variableDeclaration->get_declarationModifier().get_accessModifier().setPublic();
        }

     if (isVolatile == true)
        {
          if (SgProject::get_verbose() > 2)
               printf ("Setting modifier as Volatile \n");
          variableDeclaration->get_declarationModifier().get_typeModifier().get_constVolatileModifier().setVolatile();
        }

     if (isSynthetic == true)
        {
       // Synthetic is not a keyword, not clear if we want to record this explicitly.
          printf ("Specification of isSynthetic is not supported in the IR (should it be?) \n");
        }

     if (isStatic == true)
        {
          if (SgProject::get_verbose() > 2)
               printf ("Setting modifier as Static \n");
          variableDeclaration->get_declarationModifier().get_storageModifier().setStatic();
        }

     if (isTransient == true)
        {
          if (SgProject::get_verbose() > 2)
               printf ("Setting modifier as Transient \n");
          variableDeclaration->get_declarationModifier().get_typeModifier().get_constVolatileModifier().setJavaTransient();
        }

     if (SgProject::get_verbose() > 0)
          variableDeclaration->get_file_info()->display("source position in Java_JavaParser_cactionFieldDeclaration(): debug");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionFieldDeclarationEnd(JNIEnv *env, jobject xxx, jstring variableName, jboolean java_hasInitializer, jobject jToken )
   {
  // DQ (8/13/2011): The initializer has to be set in the endVisit() function.

     SgName name = convertJavaStringToCxxString(env,variableName);
     bool hasInitializer = java_hasInitializer;

     ROSE_ASSERT(astJavaStatementStack.empty() == false);
     SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(astJavaStatementStack.front());
     ROSE_ASSERT(variableDeclaration != NULL);

     SgInitializedName* initializedName = variableDeclaration->get_decl_item(name);
     ROSE_ASSERT(initializedName != NULL);

     if (hasInitializer == true)
        {
          ROSE_ASSERT(astJavaExpressionStack.empty() == false);
          SgExpression* expr = astJavaExpressionStack.front();

       // Must wrap this has an initializer.
          SgInitializer* initializer = SageBuilder::buildAssignInitializer(expr);
          ROSE_ASSERT(initializer != NULL);

          setJavaSourcePosition(initializer,env,jToken);

          printf ("In cactionFieldDeclarationEnd(): initializer = %p = %s \n",initializer,initializer->class_name().c_str());
          initializer->get_file_info()->display("cactionFieldDeclarationEnd()");

          initializedName->set_initptr(initializer);

          expr->set_parent(initializedName);

          astJavaExpressionStack.pop_front();
        }
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionFieldReference(JNIEnv *env, jobject xxx, jobject jToken)
   {
     if (SgProject::get_verbose() > 2)
          printf ("Inside of Java_JavaParser_cactionFieldReference() \n");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionFieldReferenceClassScope(JNIEnv *env, jobject xxx, jobject jToken)
   {
     if (SgProject::get_verbose() > 2)
          printf ("Inside of Java_JavaParser_cactionFieldReferenceClassScope() \n");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionFloatLiteral(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionForeachStatement(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionForStatement(JNIEnv *env, jobject xxx, jobject jToken)
   {
     if (SgProject::get_verbose() > 2)
          printf ("Inside of Java_JavaParser_cactionForStatement() \n");

  // outputJavaState("At TOP of cactionForStatement");

     SgNullStatement*  assignmentStatement = SageBuilder::buildNullStatement();
     SgNullStatement*  testStatement       = SageBuilder::buildNullStatement();
     SgNullExpression* incrementExpression = SageBuilder::buildNullExpression();
     SgNullStatement*  bodyStatement       = SageBuilder::buildNullStatement();

  // The SageBuilder::buildForStatement() function works better if we provide a proper SgForInitStatement
  // Else the original SgForInitStatement built by the SgForStatement constructor will be left dangling...
  // and this causes an error in the AST post processing and testing.
     SgStatementPtrList statements;
     statements.push_back(assignmentStatement);
     SgForInitStatement* forInitStatement = SageBuilder::buildForInitStatement_nfi(statements);
     ROSE_ASSERT(forInitStatement != NULL);

  // We need to set the source code position information
     SageInterface::setOneSourcePositionForTransformation(forInitStatement);
     ROSE_ASSERT(forInitStatement->get_startOfConstruct() != NULL);
  // printf ("forInitStatement = %p \n",forInitStatement);

  // It might be that we should build this on the way down so that we can have it on the stack 
  // before the body would be pushed onto the scope stack if a block is used.
  // SgForStatement* forStatement = SageBuilder::buildForStatement(assignmentStatement,testStatement,incrementExpression,bodyStatement);
     SgForStatement* forStatement = SageBuilder::buildForStatement(forInitStatement,testStatement,incrementExpression,bodyStatement);
     ROSE_ASSERT(forStatement != NULL);

     ROSE_ASSERT(forInitStatement->get_startOfConstruct() != NULL);

  // printf ("forStatement->get_for_init_stmt() = %p \n",forStatement->get_for_init_stmt());

  // DQ (7/30/2011): For the build interface to wrk we have to initialize the parent pointer to the SgForStatement.
     forStatement->set_parent(astJavaScopeStack.front());

     astJavaScopeStack.push_front(forStatement);
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionForStatementEnd(JNIEnv *env, jobject xxx, jobject jToken)
   {
     if (SgProject::get_verbose() > 2)
          printf ("Inside of Java_JavaParser_cactionForStatementEnd() \n");

     outputJavaState("At TOP of cactionForStatementEnd");

  // If we DO put all body's onto the statement stack then we process it this way.
     ROSE_ASSERT(astJavaStatementStack.empty() == false);
     SgStatement* bodyStatement = astJavaStatementStack.front();
     ROSE_ASSERT(bodyStatement != NULL);
     astJavaStatementStack.pop_front();

     ROSE_ASSERT(astJavaStatementStack.empty() == false);
     SgStatement* assignmentStatement = astJavaStatementStack.front();
     ROSE_ASSERT(assignmentStatement != NULL);
     astJavaStatementStack.pop_front();

     SgExprStatement* exprStatement = isSgExprStatement(assignmentStatement);
     if (exprStatement != NULL)
        {
          SgExpression* exp = exprStatement->get_expression();
          ROSE_ASSERT(exp != NULL);

       // printf ("exprStatement->get_expression() = %p = %s \n",exp,exp->class_name().c_str());
          ROSE_ASSERT(exp->get_type() != NULL);
       // printf ("exp->get_type() = %p = %s \n",exp->get_type(),exp->get_type()->class_name().c_str());
        }
       else
        {
          printf ("Error: This should be an expression statement for now! \n");
          ROSE_ASSERT(false);
        }

     ROSE_ASSERT(astJavaExpressionStack.empty() == false);
     SgExpression* incrementExpression = astJavaExpressionStack.front();
     ROSE_ASSERT(incrementExpression != NULL);
     astJavaExpressionStack.pop_front();

     ROSE_ASSERT(astJavaExpressionStack.empty() == false);
     SgExpression* testExpression = astJavaExpressionStack.front();
     ROSE_ASSERT(testExpression != NULL);
     astJavaExpressionStack.pop_front();

  // The ROSE IR is based on C which allows a statment for the test. Not clear if Java is the same.
     SgStatement* testStatement = SageBuilder::buildExprStatement(testExpression);
     ROSE_ASSERT(testStatement != NULL);

  // It might be that we should build this on the way down so that we can have it on the stack 
  // before the body would be pushed onto the scope stack if a block is used.
     SgForStatement* originalForStatement = isSgForStatement(astJavaScopeStack.front());
     ROSE_ASSERT(originalForStatement != NULL);
  // printf ("originalForStatement->get_for_init_stmt() = %p \n",originalForStatement->get_for_init_stmt());
     delete originalForStatement->get_for_init_stmt();
  // originalForStatement->set_for_init_stmt(NULL);

     delete originalForStatement;

  // The SageBuilder::buildForStatement() function works better if we provide a proper SgForInitStatement
  // Else the original SgForInitStatement built by the SgForStatement constructor will be left dangling...
  // and this causes an error in the AST post processing and testing.
     SgStatementPtrList statements;
     statements.push_back(assignmentStatement);
     SgForInitStatement* forInitStatement = SageBuilder::buildForInitStatement_nfi(statements);
     ROSE_ASSERT(forInitStatement != NULL);

  // We need to set the source code position information
     SageInterface::setOneSourcePositionForTransformation(forInitStatement);
     ROSE_ASSERT(forInitStatement->get_startOfConstruct() != NULL);
  // printf ("forInitStatement = %p \n",forInitStatement);

  // SgForStatement* forStatement = SageBuilder::buildForStatement(assignmentStatement,testStatement,incrementExpression,bodyStatement);
     SgForStatement* forStatement = SageBuilder::buildForStatement(forInitStatement,testStatement,incrementExpression,bodyStatement);
  // delete forStatement->get_for_init_stmt();
  // forStatement->set_for_init_stmt();
  // printf ("forStatement->get_for_init_stmt() = %p \n",forStatement->get_for_init_stmt());

     ROSE_ASSERT(forStatement != NULL);
  // astJavaScopeStack.push_front(forStatement);
     astJavaStatementStack.push_front(forStatement);

  // Remove the SgForStatement on the scope stack...
     astJavaScopeStack.pop_front();

     ROSE_ASSERT(assignmentStatement->get_parent() != NULL);
     ROSE_ASSERT(assignmentStatement->get_startOfConstruct() != NULL);

     ROSE_ASSERT(testExpression->get_parent() != NULL);
     ROSE_ASSERT(testStatement->get_parent() != NULL);

     ROSE_ASSERT(incrementExpression->get_parent() != NULL);
     ROSE_ASSERT(incrementExpression->get_startOfConstruct() != NULL);

  // printf ("forStatement->get_for_init_stmt() = %p \n",forStatement->get_for_init_stmt());
  // printf ("forStatement->get_for_init_stmt() = %p \n",forStatement->get_for_init_stmt());
     ROSE_ASSERT(forStatement->get_for_init_stmt() != NULL);
     ROSE_ASSERT(forStatement->get_for_init_stmt()->get_parent() != NULL);
     ROSE_ASSERT(forStatement->get_for_init_stmt()->get_startOfConstruct() != NULL);
  // printf ("forStatement->get_for_init_stmt() = %p \n",forStatement->get_for_init_stmt());

     ROSE_ASSERT(forStatement->get_startOfConstruct() != NULL);

     ROSE_ASSERT(forStatement->get_parent() == NULL);

  // SgForInitStatement* forInitStatement = forStatement->get_for_init_stmt();
     ROSE_ASSERT(forInitStatement != NULL);
     ROSE_ASSERT(forInitStatement->get_startOfConstruct() != NULL);
     ROSE_ASSERT(forInitStatement->get_endOfConstruct() != NULL);

     outputJavaState("At BOTTOM of cactionForStatementEnd");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionIfStatement(JNIEnv *env, jobject xxx, jobject jToken)
   {
     if (SgProject::get_verbose() > 2)
          printf ("Inside of Java_JavaParser_cactionIfStatement() \n");

     outputJavaState("At TOP of cactionIfStatement");

  // Build a SgIfStatement and push it onto the stack with a true block.

  // We need a predicate to use to call the SageBuilder::buildIfStmt() function.  So build a SgNullExpression for now. 
     SgNullStatement* temp_conditional = SageBuilder::buildNullStatement();
     SgNullStatement* true_block = SageBuilder::buildNullStatement();
     ROSE_ASSERT(true_block != NULL);

     SgIfStmt* ifStatement = SageBuilder::buildIfStmt(temp_conditional,true_block,NULL);
     ROSE_ASSERT(ifStatement != NULL);

     ifStatement->set_parent(astJavaScopeStack.front());

     setJavaSourcePosition(ifStatement,env,jToken);

  // Push the SgIfStmt onto the stack, but not the true block.
     astJavaScopeStack.push_front(ifStatement);
     ROSE_ASSERT(astJavaScopeStack.front()->get_parent() != NULL);

     outputJavaState("At BOTTOM of cactionIfStatement");
   }

JNIEXPORT void JNICALL Java_JavaParser_cactionIfStatementEnd(JNIEnv *env, jobject xxx, jint java_numberOfStatements, jobject jToken)
   {
     if (SgProject::get_verbose() > 2)
          printf ("Inside of Java_JavaParser_cactionIfStatementEnd() \n");

     outputJavaState("At TOP of cactionIfStatementEnd");

     int numberOfStatements = java_numberOfStatements;

  // There should be a predicate on the stack for us to use as a final step in construction of the SgIfStmt.
     ROSE_ASSERT(astJavaExpressionStack.empty() == false);

     ROSE_ASSERT(astJavaScopeStack.empty() == false);

     SgIfStmt* ifStatement = isSgIfStmt(astJavaScopeStack.front());
     ROSE_ASSERT(ifStatement != NULL);
     ROSE_ASSERT(ifStatement->get_parent() != NULL);

     setJavaSourcePosition(ifStatement,env,jToken);

     SgExpression* condititonalExpr = astJavaExpressionStack.front();
     ROSE_ASSERT(condititonalExpr != NULL);

     SgExprStatement* exprStatement = SageBuilder::buildExprStatement(condititonalExpr);

     setJavaSourcePosition(exprStatement,env,jToken);

     ROSE_ASSERT(exprStatement != NULL);
     ROSE_ASSERT(condititonalExpr->get_parent() != NULL);

     ifStatement->set_conditional(exprStatement);

     ROSE_ASSERT(exprStatement->get_parent() == NULL);
     exprStatement->set_parent(ifStatement);
     ROSE_ASSERT(exprStatement->get_parent() != NULL);

     astJavaExpressionStack.pop_front();

     astJavaScopeStack.pop_front();

     ROSE_ASSERT(astJavaStatementStack.empty() == false);
     ROSE_ASSERT(numberOfStatements == 1 || numberOfStatements == 2);

  // If there are two required then, then the first is for the false branch.
     if (numberOfStatements == 2)
        {
          ROSE_ASSERT(astJavaStatementStack.empty() == false);
       // printf ("Set the false body of the SgIfStmt = %p \n",astJavaStatementStack.front());
          ifStatement->set_false_body(astJavaStatementStack.front());
          astJavaStatementStack.front()->set_parent(ifStatement);
          ROSE_ASSERT(astJavaStatementStack.front()->get_parent() != NULL);
          astJavaStatementStack.pop_front();
        }

  // If there is one or more then this one is for the true branch.
     if (numberOfStatements >= 1)
        {
          ROSE_ASSERT(astJavaStatementStack.empty() == false);
       // printf ("Set the true body of the SgIfStmt = %p \n",astJavaStatementStack.front());
          ifStatement->set_true_body(astJavaStatementStack.front());
          astJavaStatementStack.front()->set_parent(ifStatement);
          ROSE_ASSERT(astJavaStatementStack.front()->get_parent() != NULL);
          astJavaStatementStack.pop_front();
        }

  // DQ (7/30/2011): Take the block off of the scope stack and put it onto the statement stack so that we can 
  // process either blocks of other statements uniformally.
     astJavaStatementStack.push_front(ifStatement);

     outputJavaState("At BOTTOM of cactionIfStatementEnd");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionImportReference(JNIEnv *env, jobject thisObj, jstring java_string, jint java_containsWildcard, jobject jToken)
   {
  // This is the import statement.  The semantics is to include the named file and add its 
  // declarations to the global scope so that they can be referenced by the current file.
  // The import directive tells the compiler where to look for the class definitions 
  // when it comes upon a class that it cannot find in the default java.lang package.

     if (SgProject::get_verbose() > -1)
          printf ("Inside of Java_JavaParser_cactionImportReference() \n");

  // I could not debug passing a Java "Boolean" variable, but "int" works fine.
  // containsWildcard = convertJavaBooleanToCxxBoolean(env,input_containsWildcard);
  // containsWildcard = (bool) (env->CallBooleanMethod(xxx,input_containsWildcard) == 1);
  // containsWildcard = (bool) input_containsWildcard;
  // containsWildcard = (bool) (env->CallStaticBooleanMethod(xxx,java_containsWildcard) == 1);
  // containsWildcard = (java_containsWildcard == 1);

     SgName name           = convertJavaStringToCxxString(env,java_string);
     bool containsWildcard = java_containsWildcard;

     printf ("import name = %s containsWildcard = %s \n",name.str(),containsWildcard ? "true" : "false");

     SgJavaImportStatement* importStatement = new SgJavaImportStatement(name,containsWildcard);
     ROSE_ASSERT(importStatement != NULL);

     ROSE_ASSERT(astJavaScopeStack.empty() == false);

  // DQ (7/31/2011): This should be left on the stack instead of being added to the current scope before the end of the scope.
  // printf ("Previously calling appendStatement in cactionImportReference() \n");
  // appendStatement(importStatement);
     astJavaStatementStack.push_front(importStatement);

  // We also have to set the parent so that the stack debugging output will work.
     importStatement->set_parent(astJavaScopeStack.front());

  // importStatement->set_parent(astJavaScopeStack.front());
  // setJavaSourcePosition(importStatement);
     setJavaSourcePosition(importStatement,env,jToken);

  // DQ (8/22/2011): The Java generics support is tied to the handling of the import statement so that we can find
  // the parameterized class from the name when it appears without name qualification.
  // The import statement should act like the using namespace directive in C++ to bring in a class or set of classes
  // so that they will be visible in the current scope.  On the Java side the classes have all been read.  Now we
  // just have to build the SgAliasSymbol in the current scope (do this tomorrow morning).
     printf ("Now build the SgAliasSymbol in the current scope \n");

  // DQ (8/23/2011): This is part of the AST post-processing, but it has to be done as we process the Java import 
  // statements (top down) so that the symbol tables will be correct and variable, function, and type references 
  // will be resolved correctly.
  // FixupAstSymbolTablesToSupportAliasedSymbols::injectSymbolsFromReferencedScopeIntoCurrentScope ( SgScopeStatement* referencedScope, SgScopeStatement* currentScope, SgAccessModifier::access_modifier_enum accessLevel );
  // FixupAstSymbolTablesToSupportAliasedSymbols::injectSymbolsFromReferencedScopeIntoCurrentScope ( SgScopeStatement* referencedScope, SgScopeStatement* currentScope, SgAccessModifier::access_modifier_enum accessLevel );

#if 0
     printf ("Exiting as a test in cactionImportReference() \n");
     ROSE_ASSERT(false);
#endif

     if (SgProject::get_verbose() > 1)
          printf ("Leaving Java_JavaParser_cactionImportReference() \n");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionInitializer(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionInstanceOfExpression(JNIEnv *env, jobject xxx, jobject jToken)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Inside of Java_JavaParser_cactionInstanceOfExpression() \n");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionInstanceOfExpressionEnd(JNIEnv *env, jobject xxx, jobject jToken)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Inside of Java_JavaParser_cactionInstanceOfExpressionEnd() \n");

     outputJavaState("At TOP of cactionInstanceOfExpressionEnd()");

     SgExpression* exp = astJavaExpressionStack.front();
     astJavaExpressionStack.pop_front();

  // The generation of this type is not yet supported.
     SgType* type = NULL;

  // Warn that this support in not finished.
     printf ("WARNING: Support for SgJavaInstanceOfOp is incomplete, type not specified! \n");

     SgExpression* result = SageBuilder::buildJavaInstanceOfOp(exp,type);
     astJavaExpressionStack.push_front(result);

     outputJavaState("At BOTTOM of cactionInstanceOfExpressionEnd()");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionIntLiteral(JNIEnv *env, jobject xxx, jint java_int_value, jobject jToken)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Build support for implicit class (end) \n");

     ROSE_ASSERT(astJavaScopeStack.empty() == false);
     outputJavaState("cactionIntLiteral");

     int value          = java_int_value;

  // Later we want to take the string from the token, but this is fine for now.
     string valueString = StringUtility::numberToString(value);

  // printf ("Building an integer value expression = %d = %s \n",value,valueString.c_str());

     SgIntVal* integerValue = new SgIntVal(value,valueString);
     ROSE_ASSERT(integerValue != NULL);

     setJavaSourcePosition(integerValue,env,jToken);

     astJavaExpressionStack.push_front(integerValue);
     ROSE_ASSERT(astJavaExpressionStack.empty() == false);
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadoc(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocClassScope(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocAllocationExpression(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocAllocationExpressionClassScope(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocArgumentExpression(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocArgumentExpressionClassScope(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocArrayQualifiedTypeReference(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocArrayQualifiedTypeReferenceClassScope(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocArraySingleTypeReference(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocArraySingleTypeReferenceClassScope(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocFieldReference(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocFieldReferenceClassScope(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocImplicitTypeReference(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocImplicitTypeReferenceClassScope(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocMessageSend(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocMessageSendClassScope(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocQualifiedTypeReference(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocQualifiedTypeReferenceClassScope(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocReturnStatement(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocReturnStatementClassScope(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocSingleNameReference(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocSingleNameReferenceClassScope(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocSingleTypeReference(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocSingleTypeReferenceClassScope(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionLabeledStatement(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionLocalDeclaration(JNIEnv *env, jobject xxx, jstring variableName, jboolean java_is_final, jobject jToken)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Inside of Java_JavaParser_cactionLocalDeclaration() \n");

     outputJavaState("At TOP of cactionLocalDeclaration");

     SgName name = convertJavaStringToCxxString(env,variableName);

     bool isFinal = java_is_final;

     if (SgProject::get_verbose() > 2)
          printf ("Building a variable declaration for name = %s \n",name.str());

  // Note that the type shuld have already been built and should be on the astJavaTypeStack.
     SgVariableDeclaration* variableDeclaration = buildSimpleVariableDeclaration(name);
     ROSE_ASSERT(variableDeclaration != NULL);

     setJavaSourcePosition(variableDeclaration,env,jToken);

  // DQ (7/16/2011): Added test for scope
     ROSE_ASSERT(variableDeclaration->get_scope() != NULL);

  // DQ (7/16/2011): This is a test to debug failing test in resetParentPointers.C:1733
     ROSE_ASSERT(SageInterface::is_Fortran_language() == false);
     SgInitializedName* initializedName = variableDeclaration->get_decl_item (name);
     ROSE_ASSERT(initializedName != NULL);
     ROSE_ASSERT(initializedName->get_scope() != NULL);

     setJavaSourcePosition(initializedName,env,jToken);
     setJavaSourcePosition(variableDeclaration,env,jToken);

  // Save it on the stack so that we can add SgInitializedNames to it.
     astJavaStatementStack.push_front(variableDeclaration);

  // We don't want to add the statement to the current scope until it is finished being built.
  // ROSE_ASSERT(astJavaScopeStack.empty() == false);
  // astJavaScopeStack.front()->append_statement(variableDeclaration);

  // Set the modifiers (shared between PHP and Java)
     if (isFinal == true)
        {
          variableDeclaration->get_declarationModifier().setFinal();
        }

  // Local declarations have no definition for this (default is public so we need to explicity mark this as undefined).
     variableDeclaration->get_declarationModifier().get_accessModifier().setUndefined();

  // DQ (8/21/2011): Debugging declarations in local function should (should not be marked as public).
     ROSE_ASSERT(variableDeclaration->get_declarationModifier().get_accessModifier().isPublic() == false);

     if (SgProject::get_verbose() > 0)
          variableDeclaration->get_file_info()->display("source position in Java_JavaParser_cactionLocalDeclaration(): debug");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionLocalDeclarationInitialization(JNIEnv *env, jobject xxx, jobject jToken)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Inside of Java_JavaParser_cactionLocalDeclarationInitialization() \n");

     outputJavaState("At TOP of cactionLocalDeclarationInitialization");

     ROSE_ASSERT(astJavaStatementStack.empty() == false);
     ROSE_ASSERT(astJavaExpressionStack.empty() == false);

     SgExpression* expression = astJavaExpressionStack.front();
     astJavaExpressionStack.pop_front();

     SgStatement* statement = astJavaStatementStack.front();
     ROSE_ASSERT(statement != NULL);
     SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(statement);
     ROSE_ASSERT(variableDeclaration != NULL);

  // variableDeclaration->set_initializer(initializer);
     SgInitializedName* initializedName = variableDeclaration->get_variables()[0];
     ROSE_ASSERT(initializedName != NULL);

     SgAssignInitializer* initializer = SageBuilder::buildAssignInitializer(expression,initializedName->get_type());
     ROSE_ASSERT(initializer != NULL);

     setJavaSourcePosition(initializer,env,jToken);
     setJavaSourcePosition(expression,env,jToken);

     initializedName->set_initptr(initializer);
     initializer->set_parent(initializedName);

     ROSE_ASSERT(initializer->get_parent() != NULL);
     ROSE_ASSERT(initializer->get_parent() == initializedName);

  // This can't be enforced since the scope might include a SgIfStmt with the predicate still on the stack.
  // ROSE_ASSERT(astJavaExpressionStack.empty() == true);
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionLongLiteral(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionMarkerAnnotation(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionMemberValuePair(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionStringLiteralConcatenation(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionNormalAnnotation(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionNullLiteral(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionORORExpression(JNIEnv *env, jobject xxx, jobject jToken)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Inside of Java_JavaParser_cactionORORExpression() \n");

     outputJavaState("At TOP of cactionORORExpression");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionORORExpressionEnd(JNIEnv *env, jobject xxx, jobject jToken)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Inside of Java_JavaParser_cactionORORExpressionEnd() \n");

     outputJavaState("At TOP of cactionORORExpressionEnd");

     binaryExpressionSupport<SgOrOp>();

     ROSE_ASSERT(astJavaExpressionStack.empty() == false);
     setJavaSourcePosition(astJavaExpressionStack.front(),env,jToken);

     outputJavaState("At BOTTOM of cactionORORExpressionEnd");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionParameterizedQualifiedTypeReference(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionParameterizedQualifiedTypeReferenceClassScope(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionParameterizedSingleTypeReference(JNIEnv *env, jobject xxx, jobject jToken)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Inside of Java_JavaParser_cactionParameterizedSingleTypeReference() \n");

     outputJavaState("At TOP of cactionParameterizedSingleTypeReference");

  // outputJavaState("At BOTTOM of cactionParameterizedSingleTypeReference");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionParameterizedSingleTypeReferenceEnd(JNIEnv *env, jobject xxx, int java_numberOfTypeArguments, jobject jToken)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Inside of Java_JavaParser_cactionParameterizedSingleTypeReferenceEnd() \n");

     outputJavaState("At TOP of cactionParameterizedSingleTypeReferenceEnd");

     int numberOfTypeArguments = java_numberOfTypeArguments;

     for (int i = 0; i < numberOfTypeArguments; i++)
        {
          ROSE_ASSERT(astJavaTypeStack.empty() == false);

          astJavaTypeStack.pop_front();
        }

  // ROSE_ASSERT(astJavaExpressionStack.empty() == false);
  // setJavaSourcePosition(astJavaExpressionStack.front(),env,jToken);

     outputJavaState("At BOTTOM of cactionParameterizedSingleTypeReferenceEnd");

     printf ("Exiting as a test in cactionParameterizedSingleTypeReferenceEnd() \n");
     exit(1);
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionParameterizedSingleTypeReferenceClassScope(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionPostfixExpression(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }

JNIEXPORT void JNICALL Java_JavaParser_cactionPostfixExpressionEnd(JNIEnv *env, jobject xxx, jint java_operator_kind, jobject jToken)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Inside of Java_JavaParser_cactionPostfixExpressionEnd() \n");

     outputJavaState("At TOP of cactionPostfixExpressionEnd");

  // These are the operator code values directly from ECJ.
     enum ops
        {
          ERROR_OPERATOR = 0, // This is not a ECJ value 
          MINUS          = 13,
          PLUS           = 14,
          LAST_OPERATOR };

     int operator_kind = java_operator_kind;
  // printf ("operator_kind = %d \n",operator_kind);

     switch(operator_kind)
        {
       // Operator codes used by the UnaryExpression in ECJ.
          case PLUS:  unaryExpressionSupport<SgPlusPlusOp>(); break;
          case MINUS: unaryExpressionSupport<SgMinusMinusOp>(); break;

          default:
             {
               printf ("Error: default reached in cactionPostfixExpressionEnd() operator_kind = %d \n",operator_kind);
               ROSE_ASSERT(false);
             }
        }

  // Mark this a a postfix operator
     SgUnaryOp* unaryOp = isSgUnaryOp(astJavaExpressionStack.front());
     ROSE_ASSERT(unaryOp != NULL);
     unaryOp->set_mode(SgUnaryOp::postfix);

     ROSE_ASSERT(astJavaExpressionStack.empty() == false);
     setJavaSourcePosition(astJavaExpressionStack.front(),env,jToken);

     outputJavaState("At BOTTOM of cactionPostfixExpressionEnd");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionPrefixExpression(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionPrefixExpressionEnd(JNIEnv *env, jobject xxx, jint java_operator_kind, jobject jToken)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Inside of Java_JavaParser_cactionPrefixExpressionEnd() \n");

     outputJavaState("At TOP of cactionPrefixExpressionEnd");

  // These are the operator code values directly from ECJ.
     enum ops
        {
          ERROR_OPERATOR = 0, // This is not a ECJ value 
          MINUS          = 13,
          PLUS           = 14,
          LAST_OPERATOR };

     int operator_kind = java_operator_kind;
  // printf ("operator_kind = %d \n",operator_kind);

     switch(operator_kind)
        {
       // Operator codes used by the UnaryExpression in ECJ.
          case PLUS:  unaryExpressionSupport<SgPlusPlusOp>(); break;
          case MINUS: unaryExpressionSupport<SgMinusMinusOp>(); break;

          default:
             {
               printf ("Error: default reached in cactionPrefixExpressionEnd() operator_kind = %d \n",operator_kind);
               ROSE_ASSERT(false);
             }
        }

  // Mark this a a prefix operator
     SgUnaryOp* unaryOp = isSgUnaryOp(astJavaExpressionStack.front());
     ROSE_ASSERT(unaryOp != NULL);
     unaryOp->set_mode(SgUnaryOp::prefix);

     ROSE_ASSERT(astJavaExpressionStack.empty() == false);
     setJavaSourcePosition(astJavaExpressionStack.front(),env,jToken);

     outputJavaState("At BOTTOM of cactionPrefixExpressionEnd");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionQualifiedAllocationExpression(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionQualifiedSuperReference(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionQualifiedSuperReferenceClassScope(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionQualifiedThisReference(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionQualifiedThisReferenceClassScope(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionQualifiedTypeReference(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionQualifiedTypeReferenceClassScope(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionReturnStatement(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionSingleMemberAnnotation(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionSingleNameReference(JNIEnv *env, jobject xxx, jstring variableName, jobject jToken)
   {
     outputJavaState("At TOP of cactionSingleNameReference");

     SgName name = convertJavaStringToCxxString(env,variableName);

     if (SgProject::get_verbose() > 0)
          printf ("Building a variable reference for name = %s \n",name.str());

  // We have to provide the starting scope to trigger the name to be looked up in parent scopes.
  // SgVarRefExp* varRefExp = SageBuilder::buildVarRefExp(name,astJavaScopeStack.front());
  // ROSE_ASSERT(varRefExp != NULL);
     SgVarRefExp* varRefExp = NULL;
     SgClassSymbol* className = SageInterface::lookupClassSymbolInParentScopes(name,astJavaScopeStack.front());

     if (SgProject::get_verbose() > 0)
          printf ("In cactionSingleNameReference(): className = %p \n",className);

     if (className != NULL)
        {
       // DQ (7/18/2011): test2011_24.java demonstrates that this can be a type.  So check for a type first...
          SgType* type = className->get_type();
          ROSE_ASSERT(type != NULL);
          astJavaTypeStack.push_front(type);
        }
       else
        {
       // What is the type!
          varRefExp = SageBuilder::buildVarRefExp(name,astJavaScopeStack.front());
          ROSE_ASSERT(varRefExp != NULL);

          if (SgProject::get_verbose() > 0)
               printf ("In cactionSingleNameReference(): varRefExp = %p type = %p = %s \n",varRefExp,varRefExp->get_type(),varRefExp->get_type()->class_name().c_str());

          setJavaSourcePosition(varRefExp,env,jToken);

          ROSE_ASSERT(varRefExp->get_file_info()->isTransformation()    == false);
          ROSE_ASSERT(varRefExp->get_file_info()->isCompilerGenerated() == false);

          astJavaExpressionStack.push_front(varRefExp);
        }

     outputJavaState("At BOTTOM of cactionSingleNameReference");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionSingleNameReferenceClassScope(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionSuperReference(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionSwitchStatement(JNIEnv *env, jobject xxx, jobject jToken)
   {
     if (SgProject::get_verbose() > 2)
          printf ("Inside of Java_JavaParser_cactionSwitchStatement() \n");

     outputJavaState("At TOP of cactionSwitchStatement");

  // 
  // We build on the way down because the scope information and symbol table information is contained
  // in the Ast node.  This AST node is a subclass of SgScopeStatement
  //
     SgSwitchStatement* switchStatement = SageBuilder::buildSwitchStatement();
     ROSE_ASSERT(switchStatement != NULL);

     setJavaSourcePosition(switchStatement,env,jToken);

  // DQ (7/30/2011): For the build interface to work we have to initialize the parent pointer to the SgForStatement.
  // PC (8/23/2011): When and why parent pointers should be set needs to be clarified. Perhaps the SageBuilder
  // functions should be revisited?
     switchStatement->set_parent(astJavaScopeStack.front());

     astJavaScopeStack.push_front(switchStatement);

     outputJavaState("At BOTTOM of cactionSwitchStatement");
   }

JNIEXPORT void JNICALL Java_JavaParser_cactionSwitchStatementEnd(JNIEnv *env, jobject thisObj, jint numCases, jboolean hasDefaultCase, jobject jToken)
   {
    if (SgProject::get_verbose() > 2)
        printf ("Inside of Java_JavaParser_cactionSwitchStatementEnd() \n");

    outputJavaState("At TOP of cactionSwitchStatementEnd");

    // Get the conditional expr and transform it into a statement
    SgExpression* item_selector = astJavaExpressionStack.front();
    ROSE_ASSERT(item_selector != NULL);
    astJavaExpressionStack.pop_front();

    SgBasicBlock *switch_block = SageBuilder::buildBasicBlock();
    switch_block -> set_parent(astJavaScopeStack.front());
    ROSE_ASSERT(isSgSwitchStatement(astJavaScopeStack.front()));

    // read 'nb_stmt' elements from the stmt stack
    // they should be every direct statement children the block has
    printf("Block Number of statement in stack %d\n", astJavaStatementStack.size());
    SgDefaultOptionStmt *default_stmt = NULL;
    for (int i = (hasDefaultCase ? numCases + 1 : numCases); i > 0; i--) {
        SgBasicBlock *case_block = SageBuilder::buildBasicBlock();
        case_block -> set_parent(switch_block);

        SgStatement *sg_stmt = astJavaStatementStack.front();
        astJavaStatementStack.pop_front();

        while (! (isSgCaseOptionStmt(sg_stmt) || isSgDefaultOptionStmt(sg_stmt))) {
            case_block -> prepend_statement(sg_stmt);

            sg_stmt = astJavaStatementStack.front();
            astJavaStatementStack.pop_front();
        }

        if  (isSgCaseOptionStmt(sg_stmt)) {
            SgCaseOptionStmt *case_stmt = (SgCaseOptionStmt *) sg_stmt;
            case_stmt -> set_body(case_block);
            case_stmt -> set_parent(switch_block);
            switch_block -> prepend_statement(case_stmt);
        }
        else {
            ROSE_ASSERT(default_stmt == NULL); // only onde default section is expected!
            default_stmt = (SgDefaultOptionStmt *) sg_stmt;
            default_stmt->set_body(case_block);
            default_stmt -> set_parent(switch_block);
            switch_block->prepend_statement(default_stmt);
        }
    }

    printf("AFTER Block Number of statement in stack %d\n", astJavaStatementStack.size());

    // Build the final Switch Statement
    SgSwitchStatement *switch_statement = (SgSwitchStatement *) astJavaScopeStack.front();
    astJavaScopeStack.pop_front();
    ROSE_ASSERT(switch_statement != NULL && isSgSwitchStatement(switch_statement));

    switch_statement -> set_item_selector(SageBuilder::buildExprStatement(item_selector));
    switch_statement -> set_body(switch_block);

     // Pushing 'switch' on the statement stack
     astJavaStatementStack.push_front(switch_statement);
     outputJavaState("At BOTTOM of cactionSwitchStatementEnd");
   }

JNIEXPORT void JNICALL Java_JavaParser_cactionSynchronizedStatement(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionThisReference(JNIEnv *env, jobject xxx, jobject jToken)
   {
     SgClassDefinition* classDefinition = getCurrentClassDefinition();
     string className = classDefinition->get_declaration()->get_name();
     printf ("Current class for ThisReference is: %s \n",className.c_str());

  // SgClassSymbol* classSymbol = classDefinition->get_declaration()->get_symbol();
     SgClassSymbol* classSymbol = isSgClassSymbol(classDefinition->get_declaration()->search_for_symbol_from_symbol_table());
     ROSE_ASSERT(classSymbol != NULL);

     SgThisExp* thisExp = SageBuilder::buildThisExp(classSymbol);
     ROSE_ASSERT(thisExp != NULL);

     astJavaExpressionStack.push_front(thisExp);
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionThisReferenceClassScope(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionThrowStatement(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionTrueLiteral(JNIEnv *env, jobject xxx, jobject jToken)
   {
     SgExpression* expression = SageBuilder::buildBoolValExp(true);
     astJavaExpressionStack.push_front(expression);
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionTryStatement(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionTypeParameter(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionTypeParameterClassScope(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionUnaryExpression(JNIEnv *env, jobject xxx, jobject jToken)
   {
     if (SgProject::get_verbose() > 2)
          printf ("Build an Unary Expression \n");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionUnaryExpressionEnd(JNIEnv *env, jobject xxx, jint java_operator_kind, jobject jToken)
   {
     if (SgProject::get_verbose() > 2)
          printf ("Build an Unary Expression End \n");

     outputJavaState("At TOP of cactionUnaryExpressionEnd");

  // These are the operator code values directly from ECJ.
     enum ops
        {
          ERROR_OPERATOR = 0, // This is not a ECJ value 
          NOT            = 11,
          TWIDDLE        = 12,
          MINUS          = 13,
          PLUS           = 14,
          LAST_OPERATOR };

     int operator_kind = java_operator_kind;
  // printf ("operator_kind = %d \n",operator_kind);

     switch(operator_kind)
        {
       // Operator codes used by the UnaryExpression in ECJ.
          case NOT:     unaryExpressionSupport<SgNotOp>();           break;
          case TWIDDLE: unaryExpressionSupport<SgBitComplementOp>(); break;
          case MINUS:   unaryExpressionSupport<SgMinusOp>();         break;
          case PLUS:    unaryExpressionSupport<SgUnaryAddOp>();      break;

          default:
             {
               printf ("Error: default reached in cactionUnaryExpressionEnd() operator_kind = %d \n",operator_kind);
               ROSE_ASSERT(false);
             }
        }

     ROSE_ASSERT(astJavaExpressionStack.empty() == false);
     setJavaSourcePosition(astJavaExpressionStack.front(),env,jToken);

     outputJavaState("At BOTTOM of cactionUnaryExpressionEnd");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionWhileStatement(JNIEnv *env, jobject xxx, jobject jToken)
   {
     if (SgProject::get_verbose() > 2)
          printf ("Inside of Java_JavaParser_cactionWhileStatement() \n");

     outputJavaState("At TOP of cactionForStatement");

     SgNullStatement*  testStatement       = SageBuilder::buildNullStatement();
     SgNullStatement*  bodyStatement       = SageBuilder::buildNullStatement();

  // It might be that we should build this on the way down so that we can have it on the stack 
  // before the body would be pushed onto the scope stack if a block is used.
  // SgForStatement* forStatement = SageBuilder::buildForStatement(assignmentStatement,testStatement,incrementExpression,bodyStatement);
     SgWhileStmt* whileStatement = SageBuilder::buildWhileStmt(testStatement,bodyStatement,NULL);
     ROSE_ASSERT(whileStatement != NULL);

  // DQ (7/30/2011): For the build interface to work we have to initialize the parent pointer to the SgForStatement.
     whileStatement->set_parent(astJavaScopeStack.front());

     astJavaScopeStack.push_front(whileStatement);

     outputJavaState("At BOTTOM of cactionForStatement");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionWhileStatementEnd(JNIEnv *env, jobject xxx, jobject jToken)
   {
     if (SgProject::get_verbose() > 2)
          printf ("Inside of Java_JavaParser_cactionWhileStatementEnd() \n");

     outputJavaState("At TOP of cactionWhileStatementEnd");

  // If we DO put all body's onto the statement stack then we process it this way.
     ROSE_ASSERT(astJavaStatementStack.empty() == false);
     SgStatement* bodyStatement = astJavaStatementStack.front();
     ROSE_ASSERT(bodyStatement != NULL);
     astJavaStatementStack.pop_front();

     ROSE_ASSERT(astJavaExpressionStack.empty() == false);
     SgExpression* testExpression = astJavaExpressionStack.front();
     ROSE_ASSERT(testExpression != NULL);
     astJavaExpressionStack.pop_front();


     SgExprStatement* testStatement = SageBuilder::buildExprStatement(testExpression);

     SgWhileStmt* originalWhileStatement = isSgWhileStmt(astJavaScopeStack.front());
     ROSE_ASSERT(originalWhileStatement != NULL);

     originalWhileStatement->set_body(bodyStatement);
     bodyStatement->set_parent(originalWhileStatement);
     ROSE_ASSERT(bodyStatement->get_parent() == originalWhileStatement);

     originalWhileStatement->set_condition(testStatement);
     testStatement->set_parent(originalWhileStatement);
     ROSE_ASSERT(testStatement->get_parent() == originalWhileStatement);

     setJavaSourcePosition(originalWhileStatement,env,jToken);

     astJavaStatementStack.push_front(originalWhileStatement);

  // Remove the SgWhileStmt on the scope stack...
     astJavaScopeStack.pop_front();

     outputJavaState("At BOTTOM of cactionWhileStatementEnd");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionWildcard(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionWildcardClassScope(JNIEnv *env, jobject xxx, jobject jToken)
   {
   }


