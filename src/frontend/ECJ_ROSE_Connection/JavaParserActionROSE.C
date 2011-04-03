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

using namespace std;

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

JNIEXPORT void JNICALL Java_JavaParser_cactionArrayTypeReference (JNIEnv *, jobject, jstring)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Build a array type \n");
   }

JNIEXPORT void JNICALL Java_JavaParser_cactionMessageSend (JNIEnv *env, jobject xxx, jstring functionName, jstring associatedClassName)
   {
  // This code is the same as that in cactionExplicitConstructorCall
     if (SgProject::get_verbose() > -1)
          printf ("Build a member function call (message send) \n");

     outputJavaState("At TOP of cactionMessageSend");

  // Should this be a SgBasicBlock or just a SgScopeStatement?
     SgBasicBlock* basicBlock = isSgBasicBlock(astJavaScopeStack.front());
     ROSE_ASSERT(basicBlock != NULL);
     ROSE_ASSERT(basicBlock->get_parent() != NULL);

     SgName name      = convertJavaStringToCxxString(env,functionName);
     SgName className = convertJavaStringToCxxString(env,associatedClassName);

     if (SgProject::get_verbose() > -1)
          printf ("building function call: name = %s from class name = %s \n",name.str(),className.str());

#if 1
  // Refactored this code to "lookupSymbolFromQualifiedName()" so it could be used to generate class types.
     SgClassSymbol* targetClassSymbol = lookupSymbolFromQualifiedName(className);
     ROSE_ASSERT(targetClassSymbol != NULL);
     SgClassDeclaration* classDeclaration = isSgClassDeclaration(targetClassSymbol->get_declaration()->get_definingDeclaration());
     ROSE_ASSERT(classDeclaration != NULL);
     SgScopeStatement* targetClassScope = classDeclaration->get_definition();
     ROSE_ASSERT(targetClassScope != NULL);
#else
     list<SgName> qualifiedClassName = generateQualifierList(className);
     SgClassSymbol* previousClassSymbol = NULL;
     SgScopeStatement* previousClassScope = astJavaScopeStack.front();
     ROSE_ASSERT(previousClassScope != NULL);

  // Traverse all of the classes to get to the class containing the functionName.
     for (list<SgName>::iterator i = qualifiedClassName.begin(); i != qualifiedClassName.end(); i++)
        {
       // Get the class from the current scope of the nearest outer most scope.
       // SgClassSymbol* classSymbol = astJavaScopeStack.back()->lookupSymbolInParentScopes(*i);

          SgSymbol* tmpSymbol = SageInterface::lookupSymbolInParentScopes(*i,previousClassScope);
          ROSE_ASSERT(tmpSymbol != NULL);
          printf ("Found a symbol tmpSymbol = %s = %s \n",tmpSymbol->class_name().c_str(),tmpSymbol->get_name().str());

       // This is either a proper class or an alias to a class where the class is implicit or included via an import statement.
          SgClassSymbol* classSymbol = isSgClassSymbol(tmpSymbol);
          SgVariableSymbol* variableSymbol = isSgVariableSymbol(tmpSymbol);
          SgAliasSymbol* aliasSymbol = isSgAliasSymbol(tmpSymbol);

          if (classSymbol == NULL && aliasSymbol != NULL)
             {
               printf ("Trace through the alias to the proper symbol in another scope. \n");
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
                    ROSE_ASSERT(classType != NULL);

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
                    ROSE_ASSERT(classSymbol != NULL);
                  }
               ROSE_ASSERT(aliasSymbol == NULL);
             }

          ROSE_ASSERT(classSymbol != NULL);

          if (SgProject::get_verbose() > -1)
               printf ("classSymbol = %p for class name = %s \n",classSymbol,(*i).str());

          previousClassSymbol = classSymbol;
          SgClassDeclaration* classDeclaration = isSgClassDeclaration(classSymbol->get_declaration()->get_definingDeclaration());
          ROSE_ASSERT(classDeclaration != NULL);
       // previousClassScope = classSymbol->get_declaration()->get_scope();
          previousClassScope = classDeclaration->get_definition();
          ROSE_ASSERT(previousClassScope != NULL);
        }

  // Use a different name to make the code more clear.
     SgClassSymbol* targetClassSymbol = previousClassSymbol;
     ROSE_ASSERT(targetClassSymbol != NULL);
     SgScopeStatement* targetClassScope = previousClassScope;
     ROSE_ASSERT(targetClassScope != NULL);
#endif

#if 0
     SgClassSymbol* classSymbol = astJavaScopeStack.front()->lookup_class_symbol(className);
     if (targetClassSymbol != NULL)
        {
          printf ("WARNING: className = %s could not be found in the symbol table \n",className.str());
        }
     ROSE_ASSERT(targetClassSymbol != NULL);
#endif

     printf ("Looking for the function = %s in class parent scope = %p = %s \n",name.str(),targetClassScope,targetClassScope->class_name().c_str());
     SgFunctionSymbol* functionSymbol = targetClassScope->lookup_function_symbol(name);
     ROSE_ASSERT(functionSymbol != NULL);

     if (functionSymbol != NULL)
        {
          printf ("FOUND function symbol = %p \n",functionSymbol);

  // This is OK for now, but might not be good enough for a non-statement function call expression (not clear yet in ECJ AST).
     SgExprListExp* parameters = NULL;
  // SgExprStatement* expressionStatement = SageBuilder::buildFunctionCallStmt(name,SgTypeVoid::createType(),parameters,astJavaScopeStack.front());

  // SgFunctionCallExp* func_call_expr     = SageBuilder::buildFunctionCallExp(name,SgTypeVoid::createType(),parameters,astJavaScopeStack.front());
     SgFunctionCallExp* func_call_expr     = SageBuilder::buildFunctionCallExp(functionSymbol,parameters);
     SgExprStatement * expressionStatement = SageBuilder::buildExprStatement(func_call_expr);

  // We might want to build the expression directly and put it onto the astJavaExpressionStack..
  // SgFunctionCallExp* buildFunctionCallExp(SgFunctionSymbol* sym, SgExprListExp* parameters=NULL);

     ROSE_ASSERT(expressionStatement != NULL);

  // Push the expression onto the stack so that the cactionMessageSendEnd can find it 
  // and any function arguments and add the arguments to the function call expression.
     astJavaExpressionStack.push_front(func_call_expr);

  // Not clear yet if a MessageSend is an expression function call or a statement function call!!!
     ROSE_ASSERT(astJavaScopeStack.empty() == false);
     astJavaScopeStack.front()->append_statement(expressionStatement);
        }
       else
        {
          printf ("ERROR: functionSymbol == NULL \n");
          ROSE_ASSERT(false);
        }
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionMessageSendEnd (JNIEnv *env, jobject xxx)
   {
  // This code is the same as that in cactionExplicitConstructorCall
     if (SgProject::get_verbose() > -1)
          printf ("Inside of Java_JavaParser_cactionMessageSendEnd() \n");

     outputJavaState("At TOP of cactionMessageSendEnd");

     ROSE_ASSERT(astJavaExpressionStack.empty() == false);

  // The astJavaExpressionStack has all of the arguments to the function call.
     vector<SgExpression*> arguments;
     while (isSgFunctionCallExp(astJavaExpressionStack.front()) == NULL)
        {
          ROSE_ASSERT(astJavaExpressionStack.empty() == false);
          arguments.push_back(astJavaExpressionStack.front());
          astJavaExpressionStack.pop_front();
        }

     SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(astJavaExpressionStack.front());
     ROSE_ASSERT(functionCallExp != NULL);

     printf ("functionCallExp = %p args = %p \n",functionCallExp,functionCallExp->get_args());

  // Are we traversing this the correct direction to get the argument order correct?
     printf ("Number of arguments to the function call expression = %zu \n",arguments.size());
     for (size_t i = 0; i < arguments.size(); i++)
        {
          ROSE_ASSERT(arguments[i] != NULL);
          functionCallExp->append_arg(arguments[i]);
        }

     outputJavaState("At BOTTOM of cactionMessageSendEnd");

#if 0
     printf ("Exiting as a test \n");
     ROSE_ASSERT(false);
#endif
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionQualifiedNameReference (JNIEnv * env, jobject xxx, jstring java_string)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Build a qualified name reference \n");
   }

JNIEXPORT void JNICALL Java_JavaParser_cactionStringLiteral (JNIEnv *env, jobject xxx, jstring java_string)
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
     setJavaSourcePosition(stringValue);

     astJavaExpressionStack.push_front(stringValue);
     ROSE_ASSERT(astJavaExpressionStack.empty() == false);
   }





// DQ: Note that the function signature is abby-normal...jclass instead of jobject (because they are 
// declared "public static native" instead of "public native" in the Java side of the JNI interface.
JNIEXPORT void JNICALL Java_JavaParser_cactionBuildImplicitClassSupportStart (JNIEnv* env, jclass xxx, jstring java_string)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Build support for implicit class (start) \n");

     outputJavaState("At TOP of cactionBuildImplicitClassSupportStart");

     SgName name = convertJavaStringToCxxString(env,java_string);

  // This builds a class to represent the implicit classes that are available by default within Java.
  // Each is built on an as needed basis (driven by references to the class).
     buildImplicitClass(name);

  // Save the list of implicitly build classes so that within cactionBuildImplicitClassSupportEnd() we can
  // process the list to support the symbol table fixup required to allow implicit named methods and
  // data members to be properly referenced with name qulaification.
     astJavaImplicitClassList.push_back(name);

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

  // Output the list of implicit classes seen so far...
     if (SgProject::get_verbose() > 0)
        {
          printf ("astJavaImplicitClassList: \n");
          for (list<SgName>::iterator i = astJavaImplicitClassList.begin(); i != astJavaImplicitClassList.end(); i++)
             {
               printf ("   --- implicit class = %s \n",(*i).str());
             }
        }

     SgGlobal* globalScope = getGlobalScope();
     NodeQuerySynthesizedAttributeType nodeList = NodeQuery::querySubTree(globalScope,V_SgClassDeclaration);

     if (SgProject::get_verbose() > 0)
        {
          printf ("Output the nodeList = \n");
          for (list<SgName>::iterator i = astJavaImplicitClassList.begin(); i != astJavaImplicitClassList.end(); i++)
             {
               SgName classNameWithQualification = *i;
               if (SgProject::get_verbose() > -1)
                    printf ("   --- implicit class = %s \n",classNameWithQualification.str());
             }
        }

  // printf ("Process the list of classes (nodeList) \n");
  // Process the classes in the list to support an implicit "use" statment of the parent of the class so that
  // each of the implicit classes will be represented in the current scope.
     for (list<SgName>::iterator i = astJavaImplicitClassList.begin(); i != astJavaImplicitClassList.end(); i++)
        {
          SgName classNameWithQualification = *i;
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

               SgClassDeclaration* targetClassDeclaration = NULL;
               list<SgClassDeclaration*> classDeclarationList;
               for (NodeQuerySynthesizedAttributeType::iterator i = nodeList.begin(); i != nodeList.end(); i++)
                  {
                    SgClassDeclaration* temp_classDeclaration = isSgClassDeclaration(*i);
                    if (temp_classDeclaration->get_name() == classNameWithoutQualification)
                       {
                         targetClassDeclaration = temp_classDeclaration;
                         classDeclarationList.push_back(targetClassDeclaration);
                       }
                  }
               ROSE_ASSERT(classDeclarationList.empty() == false);
               ROSE_ASSERT(classDeclarationList.size() == 1);

               classSymbol = isSgClassSymbol(targetClassDeclaration->search_for_symbol_from_symbol_table());
               if (classSymbol == NULL)
                    classSymbol = isSgClassSymbol(targetClassDeclaration->get_firstNondefiningDeclaration()->search_for_symbol_from_symbol_table());

               ROSE_ASSERT(classSymbol != NULL);

               ROSE_ASSERT(globalScope->symbol_exists(classNameWithoutQualification) == false);
               ROSE_ASSERT(globalScope->symbol_exists(classSymbol) == false);

               SgAliasSymbol* aliasSymbol = new SgAliasSymbol(classSymbol,/* isRenamed */ false);

               if ( SgProject::get_verbose() > 0 )
                    printf ("Adding SgAliasSymbol for classNameWithoutQualification = %s \n",classNameWithoutQualification.str());

               globalScope->insert_symbol(classNameWithoutQualification,aliasSymbol);
             }
        }
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

     ROSE_ASSERT(astJavaScopeStack.empty() == false);
     astJavaScopeStack.front()->append_statement(variableDeclaration);

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
  // printf ("Java_JavaParser_cactionGenerateType(): name = %s \n",name.str());

#if 0
     if (name == "int")
        {
       // Specification of integer type.
          if (SgProject::get_verbose() > 0)
               printf ("Inside of Java_JavaParser_cactionGenerateType(): building an integer type \n");

          astJavaTypeStack.push_front(SgTypeInt::createType());
        }
       else
        {
       // buildImplicitClass(name);
          printf ("Error: type support not implemented for name = %s \n",name.str());
          ROSE_ASSERT(false);
        }
#endif

     if (name == "boolean")
        {
          astJavaTypeStack.push_front(SgTypeInt::createType());
        }
       else if (name == "byte")
        {
          astJavaTypeStack.push_front(SgTypeUnsignedChar::createType());
        }
       else if (name == "char")
        {
          astJavaTypeStack.push_front(SgTypeChar::createType());
        }
       else if (name == "int")
        {
          astJavaTypeStack.push_front(SgTypeInt::createType());
        }
       else if (name == "short")
        {
          astJavaTypeStack.push_front(SgTypeShort::createType());
        }
       else if (name == "float")
        {
          astJavaTypeStack.push_front(SgTypeFloat::createType());
        }
       else if (name == "long")
        {
          astJavaTypeStack.push_front(SgTypeLong::createType());
        }
       else if (name == "double")
        {
          astJavaTypeStack.push_front(SgTypeDouble::createType());
        }
       else if (name == "null")
        {
       // astJavaTypeStack.push_front(SgTypeInt::createType());
          printf ("Java type null is not supported yet in ROSE \n");
          ROSE_ASSERT(false);
        }
       else
        {
          printf ("Error: type support not implemented for name = %s \n",name.str());
          ROSE_ASSERT(false);
        }

  // printf ("Leaving Java_JavaParser_cactionGenerateType() \n");

#if 0
     printf ("Build support for types (not finished) \n");
     ROSE_ASSERT(false);
#endif
   }

JNIEXPORT void JNICALL Java_JavaParser_cactionGenerateClassType (JNIEnv* env, jclass xxx, jstring java_string)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Inside of Java_JavaParser_cactionGenerateClassType() \n");

     SgName name = convertJavaStringToCxxString(env,java_string);

  // printf ("In Java_JavaParser_cactionGenerateClassType(): Calling lookupSymbolFromQualifiedName(name = %s) \n",name.str());

  // Lookup the name, find the symbol, build a SgClassType, and push it onto the astJavaTypeStack.
     SgClassSymbol* targetClassSymbol = lookupSymbolFromQualifiedName(name);

  // printf ("DONE: In Java_JavaParser_cactionGenerateClassType(): Calling lookupSymbolFromQualifiedName(name = %s) \n",name.str());
  // ROSE_ASSERT(targetClassSymbol != NULL);
     if (targetClassSymbol != NULL)
        {
       // printf ("In Java_JavaParser_cactionGenerateClassType(): Get the SgClassDeclaration \n");
          SgClassDeclaration* classDeclaration = isSgClassDeclaration(targetClassSymbol->get_declaration());
          ROSE_ASSERT(classDeclaration != NULL);

       // printf ("In Java_JavaParser_cactionGenerateClassType(): Get the SgClassType \n");
          SgClassType* classType = SgClassType::createType(classDeclaration);
          ROSE_ASSERT(classType != NULL);

          astJavaTypeStack.push_front(classType);
        }
       else
        {
       // This is OK when we are only processing a small part of the implicit class space (debugging mode) and have not built all the SgClassDeclaration IR nodes. 
          printf ("WARNING: SgClassSymbol NOT FOUND in lookupSymbolFromQualifiedName(): name = %s (build an integer type and keep going...) \n",name.str());

          astJavaTypeStack.push_front(SgTypeInt::createType());
        }

     outputJavaState("At Bottom of cactionGenerateClassType");

#if 0
     printf ("Build support for class types (not finished) \n");
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
     if (SgProject::get_verbose() > 0)
          printf ("Build support for implicit class (end) \n");

     ROSE_ASSERT(astJavaScopeStack.empty() == false);
     outputJavaState("cactionIntLiteral");

     int value          = 42;
     string valueString = "42";

     printf ("Building an integer value expression = %d = %s \n",value,valueString.c_str());

     SgIntVal* integerValue = new SgIntVal(value,valueString);
     ROSE_ASSERT(integerValue != NULL);

  // Set the source code position (default values for now).
     setJavaSourcePosition(integerValue);

     astJavaExpressionStack.push_front(integerValue);
     ROSE_ASSERT(astJavaExpressionStack.empty() == false);
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


