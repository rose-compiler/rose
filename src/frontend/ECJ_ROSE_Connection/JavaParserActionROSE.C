// This is the file containing the C++ versions of the JNI functions written to interface with ECJ.
// These functions are called from the JAVA code (within the Java based traversal over the ECJ AST) 
// and construct the ROSE AST.

// Support for calling the Java <--> C/C++ code.
#include <jni.h>

// What was this?
#include "JavaParser.h"

// Support for the internal ROSE IR classes.
#include "sage3basic.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

// Support functions so that this file can be restricted to be just parser (AST traversal) rules.
#include "java_support.h"

/*
 * Class:     JavaParser
 * Method:    cactionCompilationUnitList
 * Signature: (java/lang/String;)
 */
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
   }


/*
 * Class:     JavaParser
 * Method:    cactionCompilationUnitDeclaration
 * Signature: (java/lang/String;)
 */
JNIEXPORT void JNICALL Java_JavaParser_cactionCompilationUnitDeclaration (JNIEnv *env, jobject xxx, jstring compilationUnitFilename)
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


JNIEXPORT void JNICALL Java_JavaParser_cactionTypeDeclaration (JNIEnv *env, jobject xxx, jstring java_string)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Build a SgClassDeclaration \n");

  // We could provide a constructor for "SgName" that takes a "jstring".  This might help support a simpler interface.
     SgName name = convertJavaStringToCxxString(env,java_string);

     if (SgProject::get_verbose() > 0)
          printf ("Build class type: name = %s \n",name.str());

     outputJavaState("At TOP of cactionTypeDeclaration");

  // This builds the associated class in the curren scope as defined by the astJavaScopeStack.
     buildClass(name);

     ROSE_ASSERT(astJavaScopeStack.front() != NULL);

     if (SgProject::get_verbose() > 0)
          astJavaScopeStack.front()->get_file_info()->display("source position in Java_JavaParser_cactionTypeDeclaration(): debug");

     outputJavaState("At BOTTOM of cactionTypeDeclaration");

     if (SgProject::get_verbose() > 0)
          printf ("Leaving Java_JavaParser_cactionTypeDeclaration() \n");
   }




JNIEXPORT void JNICALL Java_JavaParser_cactionConstructorDeclaration (JNIEnv *env, jobject xxx, jstring java_string)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Build a SgMemberFunctionDeclaration (constructor) \n");

     outputJavaState("At TOP of cactionConstructorDeclaration");

#if 1
  // Comment out to test simpler useage.

  // SgMemberFunctionDeclaration* buildDefiningMemberFunctionDeclaration (const SgName & name, SgType* return_type, SgFunctionParameterList *parlist, SgScopeStatement* scope=NULL);
  // SgName name = "ConstructorDeclaration_abc";
     SgName name = convertJavaStringToCxxString(env,java_string);

     SgClassDefinition* classDefinition = isSgClassDefinition(astJavaScopeStack.front());
     ROSE_ASSERT(classDefinition != NULL);

  // DQ (3/25/2011): changed to reflect requirement to build defining declaration.
  // SgMemberFunctionDeclaration* functionDeclaration = buildSimpleMemberFunction(name,classDefinition);
     SgMemberFunctionDeclaration* functionDeclaration = buildDefiningMemberFunction(name,classDefinition);

     SgFunctionDefinition* functionDefinition = functionDeclaration->get_definition();
     ROSE_ASSERT(functionDefinition != NULL);

     astJavaScopeStack.push_front(functionDefinition);
     ROSE_ASSERT(astJavaScopeStack.front()->get_parent() != NULL);

     ROSE_ASSERT(functionDefinition->get_body() != NULL);
     astJavaScopeStack.push_front(functionDefinition->get_body());
     ROSE_ASSERT(astJavaScopeStack.front()->get_parent() != NULL);
#endif

  // Push this statement onto the stack so that we can add arguments, etc.
     astJavaStatementStack.push_front(functionDeclaration);

     outputJavaState("At BOTTOM of cactionConstructorDeclaration");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionConstructorDeclarationEnd (JNIEnv *env, jobject xxx)
   {
     if (SgProject::get_verbose() > 0)
          printf ("End of SgMemberFunctionDeclaration (constructor) \n");

  // Pop the constructor body...
     ROSE_ASSERT(astJavaScopeStack.empty() == false);
     astJavaScopeStack.pop_front();
 
  // Pop the fuction definition...
     ROSE_ASSERT(astJavaScopeStack.empty() == false);
     astJavaScopeStack.pop_front();
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionExplicitConstructorCall (JNIEnv *env, jobject xxx, jstring java_string)
   {
  // Build a member function call...
     if (SgProject::get_verbose() > 0)
          printf ("Build a explicit constructor function call \n");

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
     astJavaScopeStack.front()->append_statement(expressionStatement);
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionMethodDeclaration (JNIEnv *env, jobject xxx, jstring java_string)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Build a SgMemberFunctionDeclaration \n");

     outputJavaState("At TOP of cactionMethodDeclaration");

     SgName name = convertJavaStringToCxxString(env,java_string);

     SgClassDefinition* classDefinition = isSgClassDefinition(astJavaScopeStack.front());
     ROSE_ASSERT(classDefinition != NULL);

  // DQ (3/25/2011): Changed this to a non-defining declaration.
  // SgMemberFunctionDeclaration* functionDeclaration = buildSimpleMemberFunction(name,classDefinition);
  // SgMemberFunctionDeclaration* functionDeclaration = buildNonDefiningMemberFunction(name,classDefinition);
     SgMemberFunctionDeclaration* functionDeclaration = buildDefiningMemberFunction(name,classDefinition);
     ROSE_ASSERT(functionDeclaration != NULL);

  // This is handled in the buildSimpleMemberFunction() function (though perhaps that is not the semantics we want).
  // astJavaScopeStack.front()->append_statement(functionDeclaration);

#if 1
  // This is not a defining function declaration so we can't identify the SgFunctionDefinition and push it's body onto the astJavaScopeStack.
     SgFunctionDefinition* functionDefinition = functionDeclaration->get_definition();
     ROSE_ASSERT(functionDefinition != NULL);

     astJavaScopeStack.push_front(functionDefinition);
     ROSE_ASSERT(astJavaScopeStack.front()->get_parent() != NULL);

     ROSE_ASSERT(functionDefinition->get_body() != NULL);
     astJavaScopeStack.push_front(functionDefinition->get_body());
     ROSE_ASSERT(astJavaScopeStack.front()->get_parent() != NULL);
#endif

  // Push the declaration onto the declaration stack.
  // Push this statement onto the stack so that we can add arguments, etc.
     astJavaStatementStack.push_front(functionDeclaration);

     outputJavaState("At BOTTOM of cactionMethodDeclaration");
   }

JNIEXPORT void JNICALL Java_JavaParser_cactionMethodDeclarationEnd (JNIEnv *env, jobject xxx /* , jstring java_string */ )
   {
     if (SgProject::get_verbose() > 0)
          printf ("End of SgMemberFunctionDeclaration (method) \n");

  // SgName name = convertJavaStringToCxxString(env,java_string);

  // Pop the constructor body...
     ROSE_ASSERT(astJavaScopeStack.empty() == false);
     astJavaScopeStack.pop_front();
 
  // Pop the fuction definition...
     ROSE_ASSERT(astJavaScopeStack.empty() == false);
     astJavaScopeStack.pop_front();
   }



JNIEXPORT void JNICALL Java_JavaParser_cactionSingleTypeReference (JNIEnv *, jobject, jstring)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Build a type \n");

  // Build a type and put it onto the type stack.
  // ...OR...
  // Build a type and add it to the declaration on the declaration stack.
   }

JNIEXPORT void JNICALL Java_JavaParser_cactionArgument (JNIEnv *env, jobject xxx, jstring java_argument_name, jint java_modifiers)
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

#if 1
  // Until we attached this to the AST, this will generate an error in the AST consistancy tests.
     SgInitializedName* initializedName = SageBuilder::buildInitializedName(argument_name,type,NULL);
     ROSE_ASSERT(initializedName != NULL);
#endif

     ROSE_ASSERT(astJavaStatementStack.empty() == false);
     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(astJavaStatementStack.front());
     ROSE_ASSERT(functionDeclaration != NULL);

  // SgInitializedNamePtrList & arg_list = functionDeclaration->get_args();
  // arg_list->append(initializedName);
     functionDeclaration->append_arg(initializedName);
     ROSE_ASSERT(functionDeclaration->get_args().empty() == false);

#if 0
     printf ("Exiting in Java_JavaParser_cactionArgument() \n");
     ROSE_ASSERT(false);
#endif
   }

#if 0
JNIEXPORT void JNICALL Java_JavaParser_cactionArgumentName(JNIEnv *env, jobject, jstring java_string)
   {
     SgName name = convertJavaStringToCxxString(env,java_string);
     printf ("argument name = %s \n",name.str());
   }

#error "DEAD CODE!"

JNIEXPORT void JNICALL Java_JavaParser_cactionArgumentModifiers(JNIEnv *env, jobject, jint java_modifiers)
   {
     int modifiers = convertJavaIntegerToCxxInteger(env,java_modifiers);

  // Now push this integer onto the stack so that we can build the SgInitializedName 
  // as the argument in Java_JavaParser_cactionArgument_end().

     printf ("Modifiers not pushed onto the AST stack. \n");
   }

#error "DEAD CODE!"

JNIEXPORT void JNICALL Java_JavaParser_cactionArgumentEnd(JNIEnv *env, jobject xxx)
   {
  // Now build the SgInitializedName object to be the argument.
     printf ("Inside of Java_JavaParser_cactionArgumentEnd() \n");

     ROSE_ASSERT(astJavaTypeStack.empty() == false);
     SgType* type = astJavaTypeStack.front();
     astJavaTypeStack.pop_front();

     ROSE_ASSERT(astJavaNameStack.empty() == false);
     SgName* name = astJavaNameStack.front();
     astJavaNameStack.pop_front();

     SgInitializedName* initializedName = buildInitializedName(name,type,NULL);
     ROSE_ASSERT(initializedName != NULL);

     ROSE_ASSERT(astJavaStatementStack.empty() == false);
     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(astJavaStatementStack.front());
     ROSE_ASSERT(functionDeclaration != NULL);

  // SgInitializedNamePtrList & arg_list = functionDeclaration->get_args();
  // arg_list->append(initializedName);
     functionDeclaration->append_arg(initializedName);
     ROSE_ASSERT(functionDeclaration->get_args().empty() == false);

#if 1
     printf ("Inside of Java_JavaParser_cactionArgumentEnd(): exiting ... \n");
     ROSE_ASSERT(false);
#endif
   }
#endif

JNIEXPORT void JNICALL Java_JavaParser_cactionArrayTypeReference (JNIEnv *, jobject, jstring)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Build a array type \n");
   }

JNIEXPORT void JNICALL Java_JavaParser_cactionMessageSend (JNIEnv *env, jobject xxx, jstring functionName, jstring associatedClassName)
   {
  // This code is the same as that in cactionExplicitConstructorCall
     if (SgProject::get_verbose() > 0)
          printf ("Build a member function call (message send) \n");

  // Should this be a SgBasicBlock or just a SgScopeStatement?
     SgBasicBlock* basicBlock = isSgBasicBlock(astJavaScopeStack.front());
     ROSE_ASSERT(basicBlock != NULL);
     ROSE_ASSERT(basicBlock->get_parent() != NULL);

     SgName name = convertJavaStringToCxxString(env,functionName);
     SgName className = convertJavaStringToCxxString(env,associatedClassName);

     if (SgProject::get_verbose() > 0)
          printf ("building function call: name = %s from class name = %s \n",name.str(),className.str());

     SgClassSymbol* classSymbol = astJavaScopeStack.front()->lookup_class_symbol(className);
  // ROSE_ASSERT(classSymbol != NULL);
     if (classSymbol != NULL)
        {
          printf ("WARNING: className = %s could not be found in the symbol table \n",className.str());
        }

  // This is OK for now, but might not be good enough for a non-statement function call expression (not clear yet in ECJ AST).
     SgExprListExp* parameters = NULL;
     SgExprStatement* expressionStatement = SageBuilder::buildFunctionCallStmt(name,SgTypeVoid::createType(),parameters,astJavaScopeStack.front());

  // We might want to build the expression directly and put it onto the astJavaExpressionStack..
  // SgFunctionCallExp* buildFunctionCallExp(SgFunctionSymbol* sym, SgExprListExp* parameters=NULL);

     ROSE_ASSERT(expressionStatement != NULL);

     ROSE_ASSERT(astJavaScopeStack.empty() == false);
     astJavaScopeStack.front()->append_statement(expressionStatement);
   }

JNIEXPORT void JNICALL Java_JavaParser_cactionQualifiedNameReference (JNIEnv *, jobject, jstring)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Build a qualified name reference \n");
   }

JNIEXPORT void JNICALL Java_JavaParser_cactionStringLiteral (JNIEnv *, jobject, jstring)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Build a SgStringVal \n");
   }





// DQ: Note that the function signature is abby-normal...jclass instead of jobject (because they are 
// declared "public static native" instead of "public native" in the Java side of the JNI interface.
JNIEXPORT void JNICALL Java_JavaParser_cactionBuildImplicitClassSupportStart (JNIEnv* env, jclass xxx, jstring java_string)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Build support for implicit class (start) \n");

     outputJavaState("At TOP of cactionBuildImplicitClassSupportStart");

#if 1
     SgName name = convertJavaStringToCxxString(env,java_string);

  // This builds a class to represent the implicit classes that are available by default within Java.
  // Each is built on an as needed basis (driven by references to the class).
     buildImplicitClass(name);
#endif

     outputJavaState("At BOTTOM of cactionBuildImplicitClassSupportStart");
   }


// DQ: Note that the function signature is abby-normal...jclass instead of jobject (because they are 
// declared "public static native" instead of "public native" in the Java side of the JNI interface.
JNIEXPORT void JNICALL Java_JavaParser_cactionBuildImplicitClassSupportEnd (JNIEnv* env, jclass xxx, jstring java_string)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Build support for implicit class (end) \n");

     ROSE_ASSERT(astJavaScopeStack.empty() == false);
     outputJavaState("cactionBuildImplicitClassSupportEnd");

  // Experiment with ERROR on C++ side...communicated to Java...and back to C++ side where the JVM is called by ROSE...
  // ROSE_ASSERT(false);

  // Pop the class definition off the scope stack...
     ROSE_ASSERT(astJavaScopeStack.empty() == false);
     astJavaScopeStack.pop_front();

  // At this point we shuld still at least have the global scope on the stack.
     ROSE_ASSERT(astJavaScopeStack.empty() == false);
   }


// DQ: Note that the function signature is abby-normal...jclass instead of jobject (because they are 
// declared "public static native" instead of "public native" in the Java side of the JNI interface.
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

     SgName name = convertJavaStringToCxxString(env,java_string);

     if (name == "int")
        {
       // Specification of integer type.
          if (SgProject::get_verbose() > 0)
               printf ("Inside of Java_JavaParser_cactionGenerateType(): building an integer type \n");

          astJavaTypeStack.push_front(SgTypeInt::createType());
        }
       else
        {
          printf ("Error: type support not implemented for name = %s \n",name.str());
          ROSE_ASSERT(false);
        }

#if 0
     printf ("Build support for types (not finished) \n");
     ROSE_ASSERT(false);
#endif
   }



JNIEXPORT void JNICALL Java_JavaParser_cactionStatementEnd(JNIEnv *env, jclass xxx, jstring java_string /* JNIEnv *env, jobject xxx */ )
   {
     if (SgProject::get_verbose() > 0)
          printf ("Closing actions for statements \n");

     ROSE_ASSERT(astJavaStatementStack.empty() == false);
     astJavaStatementStack.pop_front();
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionAllocationExpression(JNIEnv *env, jobject xxx)
   {
   }

JNIEXPORT void JNICALL Java_JavaParser_cactionAND_AND_Expression(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionAnnotationMethodDeclaration(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionArgumentClassScope(JNIEnv *, jobject, jstring)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionArrayAllocationExpression(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionArrayInitializer(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionArrayQualifiedTypeReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionArrayQualifiedTypeReferenceClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionArrayReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionArrayTypeReferenceClassScope(JNIEnv *, jobject, jstring)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionAssertStatement(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionAssignment(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionBinaryExpression(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionBlock(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionBreakStatement(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionCaseStatement(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionCastExpression(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionCharLiteral(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionClassLiteralAccess(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionClinit(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionConditionalExpression(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionContinueStatement(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionCompoundAssignment(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionDoStatement(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionDoubleLiteral(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionEmptyStatement(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionEqualExpression(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionExtendedStringLiteral(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionFalseLiteral(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionFieldDeclaration(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionFieldReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionFieldReferenceClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionFloatLiteral(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionForeachStatement(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionForStatement(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionIfStatement(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionImportReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionInitializer(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionInstanceOfExpression(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionIntLiteral(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadoc(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocAllocationExpression(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocAllocationExpressionClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocArgumentExpression(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocArgumentExpressionClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocArrayQualifiedTypeReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocArrayQualifiedTypeReferenceClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocArraySingleTypeReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocArraySingleTypeReferenceClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocFieldReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocFieldReferenceClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocImplicitTypeReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocImplicitTypeReferenceClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocMessageSend(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocMessageSendClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocQualifiedTypeReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocQualifiedTypeReferenceClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocReturnStatement(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocReturnStatementClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocSingleNameReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocSingleNameReferenceClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocSingleTypeReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionJavadocSingleTypeReferenceClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionLabeledStatement(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionLocalDeclaration(JNIEnv *env, jobject xxx, jstring variableName)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Inside of Java_JavaParser_cactionLocalDeclaration() \n");

     outputJavaState("At TOP of cactionLocalDeclaration");

     SgName name = convertJavaStringToCxxString(env,variableName);

  // Note that the type shuld have already been built and should be on the astJavaTypeStack.
     SgVariableDeclaration* variableDeclaration = buildSimpleVariableDeclaration(name);
     ROSE_ASSERT(variableDeclaration != NULL);

     astJavaStatementStack.push_front(variableDeclaration);

     ROSE_ASSERT(astJavaScopeStack.empty() == false);
     astJavaScopeStack.front()->append_statement(variableDeclaration);

     if (SgProject::get_verbose() > 0)
          variableDeclaration->get_file_info()->display("source position in Java_JavaParser_cactionLocalDeclaration(): debug");
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionLongLiteral(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionMarkerAnnotation(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionMemberValuePair(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionStringLiteralConcatenation(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionNormalAnnotation(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionNullLiteral(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionOR_OR_Expression(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionParameterizedQualifiedTypeReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionParameterizedQualifiedTypeReferenceClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionParameterizedSingleTypeReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionParameterizedSingleTypeReferenceClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionPostfixExpression(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionPrefixExpression(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionQualifiedAllocationExpression(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionQualifiedSuperReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionQualifiedSuperReferenceClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionQualifiedThisReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionQualifiedThisReferenceClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionQualifiedTypeReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionQualifiedTypeReferenceClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionReturnStatement(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionSingleMemberAnnotation(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionSingleNameReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionSingleNameReferenceClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionSuperReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionSwitchStatement(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionSynchronizedStatement(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionThisReference(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionThisReferenceClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionThrowStatement(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionTrueLiteral(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionTryStatement(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionTypeParameter(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionTypeParameterClassScope(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionUnaryExpression(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionWhileStatement(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionWildcard(JNIEnv *env, jobject xxx)
   {
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionWildcardClassScope(JNIEnv *env, jobject xxx)
   {
   }


