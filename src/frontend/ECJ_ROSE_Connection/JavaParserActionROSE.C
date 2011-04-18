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

// DQ (4/16/2011): Support for JNI function for tokens (source code position).
// #include "token.h"
#include "jni_token.h"

using namespace std;



JNIEXPORT void JNICALL Java_JavaParser_cactionSetSourcePosition(JNIEnv *env, jclass xxx, jobject java_token)
   {
     if (SgProject::get_verbose() > -1)
          printf ("Process the token to set the source code position... \n");

     outputJavaState("At TOP of cactionSetSourcePosition");

     Token_t* token = convert_Java_token(env,java_token);
     ROSE_ASSERT(token != NULL);

     printf ("token = %s line = %d col = %d \n",token->getText().c_str(),token->getLine(),token->getCol());

#if 0
     printf ("Exiting as a test in Java_JavaParser_cactionSetSourcePosition! \n");
     ROSE_ASSERT(false);
#endif

     outputJavaState("At BOTTOM of cactionSetSourcePosition");
   }



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

  // This design is no longer required since we push everything required to build a function 
  // declaration onto the stack before we build the declaration, thus it is completely built 
  // at this point.
  // Push this statement onto the stack so that we can add arguments, etc.
  // astJavaStatementStack.push_front(functionDeclaration);

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

     outputJavaState("At TOP of cactionExplicitConstructorCall");

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

     printf ("In cactionExplicitConstructorCall: Number of statements in current scope = %zu \n",astJavaScopeStack.front()->generateStatementList().size());

     outputJavaState("At Bottom of cactionExplicitConstructorCall");
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

  // This design is no longer required since we push everything required to build a function 
  // declaration onto the stack before we build the declaration, thus it is completely built 
  // at this point.
  // Push the declaration onto the declaration stack.
  // Push this statement onto the stack so that we can add arguments, etc.
  // astJavaStatementStack.push_front(functionDeclaration);

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

#if 0
     ROSE_ASSERT(astJavaStatementStack.empty() == false);
     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(astJavaStatementStack.front());
     ROSE_ASSERT(functionDeclaration != NULL);

  // SgInitializedNamePtrList & arg_list = functionDeclaration->get_args();
  // arg_list->append(initializedName);
     functionDeclaration->append_arg(initializedName);
     ROSE_ASSERT(functionDeclaration->get_args().empty() == false);
#else
  // DQ (4/6/2011): Instead of assuming there is a function declaration available, we 
  // want to put each SgInitializedName onto the stack so that they can be assembled
  // into the parameter list as part of building the function declaration afterwards.
  // This also allows for the function type to be computed from the function
  // parameter list (the types match the types on the function parameters) and
  // then the SgFunctionSymbol can be computed and inserted into the function 
  // declaration's scope (current scope) as part of building the function declaration.
  // To support this we use the astJavaInitializedNameStack.
     astJavaInitializedNameStack.push_front(initializedName);
#endif

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
#if 0
     SgExprStatement * expressionStatement = SageBuilder::buildExprStatement(func_call_expr);

  // We might want to build the expression directly and put it onto the astJavaExpressionStack..
  // SgFunctionCallExp* buildFunctionCallExp(SgFunctionSymbol* sym, SgExprListExp* parameters=NULL);

     ROSE_ASSERT(expressionStatement != NULL);

  // Not clear yet if a MessageSend is an expression function call or a statement function call!!!
     ROSE_ASSERT(astJavaScopeStack.empty() == false);
     astJavaScopeStack.front()->append_statement(expressionStatement);
#else
  // Push the expression onto the stack so that the cactionMessageSendEnd can find it 
  // and any function arguments and add the arguments to the function call expression.
     astJavaExpressionStack.push_front(func_call_expr);
#endif
        }
       else
        {
          printf ("ERROR: functionSymbol == NULL \n");
          ROSE_ASSERT(false);
        }
   }


JNIEXPORT void JNICALL Java_JavaParser_cactionMessageSendEnd (JNIEnv *env, jobject xxx)
   {
  // Unclear if this should be a function call statement or a function call expression.
  // When it is a statement there does not appear to be any thing to provide a clue 
  // about this, so we will assume it is a statement and do more testing.

  // This code is the same as that in cactionExplicitConstructorCall
     if (SgProject::get_verbose() > -1)
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

          printf ("functionCallExp = %p args = %p \n",functionCallExp,functionCallExp->get_args());

       // Are we traversing this the correct direction to get the argument order correct?
          printf ("Number of arguments to the function call expression = %zu \n",arguments.size());
          for (size_t i = 0; i < arguments.size(); i++)
             {
               ROSE_ASSERT(arguments[i] != NULL);
               functionCallExp->append_arg(arguments[i]);
             }

       // ROSE_ASSERT(astJavaStatementStack.empty() == true);
       // SgStatement* statement = SageBuilder::buildExprStatement(functionCallExp);
          statement = SageBuilder::buildExprStatement(functionCallExp);
          ROSE_ASSERT(statement != NULL);
        }

     ROSE_ASSERT(statement != NULL);
     
  // astJavaStatementStack.push_front(statement);
     astJavaScopeStack.front()->append_statement(statement);

     printf ("Number of statements in current scope = %zu \n",astJavaScopeStack.front()->generateStatementList().size());

     ROSE_ASSERT(astJavaExpressionStack.empty() == true);
     
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

#if 0
  // DQ (4/15/2011): This more expensive approach is no longer required.

  // We need to pick out the SgClassDeclaration from the AST to build the SgAliasSysmbol.
  // So we build a list and find the one that matches the name, however this is a global 
  // operation and could be made much more efficient and direct by parsing the qualified 
  // name and finding the symbol more directly.  So this is simple to implement and should
  // be replaced later.
     printf ("##### Replace global search of AST to find name = %s more directly #####\n",classNameWithQualification.str());
     NodeQuerySynthesizedAttributeType nodeList = NodeQuery::querySubTree(globalScope,V_SgClassDeclaration);

     if (SgProject::get_verbose() > 0)
        {
          printf ("Output the nodeList (current classes in AST) = \n");
       // for (list<SgName>::iterator i = astJavaImplicitClassList.begin(); i != astJavaImplicitClassList.end(); i++)
          for (NodeQuerySynthesizedAttributeType::iterator i = nodeList.begin(); i != nodeList.end(); i++)
             {
            // SgName classNameWithQualification = *i;
            // if (SgProject::get_verbose() > -1)
            //      printf ("   --- implicit class = %s \n",classNameWithQualification.str());

               SgClassDeclaration* temp_classDeclaration = isSgClassDeclaration(*i);
               ROSE_ASSERT(temp_classDeclaration != NULL);
               printf ("   --- nodeList: %s \n",temp_classDeclaration->get_name().str());
             }
        }
#endif

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

#if 0
       // DQ (4/15/2011): This more expensive approach is no longer required.

       // DQ (4/15/2011): The class might not have been put into global scope, but it might be present because of a reference.
          SgClassSymbol* alt_classSymbol = lookupSymbolFromQualifiedName(classNameWithQualification);
          ROSE_ASSERT(alt_classSymbol != NULL);

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

          if (classDeclarationList.size() != 1)
             {
               printf ("In cactionBuildImplicitClassSupportEnd(): classDeclarationList.size() = %zu \n",classDeclarationList.size());
               outputJavaState("In buildImplicitClass(): Pop the class built implicitly from the stack.");
             }
          ROSE_ASSERT(classDeclarationList.size() == 1);

          classSymbol = isSgClassSymbol(targetClassDeclaration->search_for_symbol_from_symbol_table());
          if (classSymbol == NULL)
               classSymbol = isSgClassSymbol(targetClassDeclaration->get_firstNondefiningDeclaration()->search_for_symbol_from_symbol_table());
#else
          classSymbol = lookupSymbolFromQualifiedName(classNameWithQualification);
#endif

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
JNIEXPORT void JNICALL Java_JavaParser_cactionBuildImplicitClassSupportEnd (JNIEnv* env, jclass xxx, jstring java_string)
   {
     SgName name = convertJavaStringToCxxString(env,java_string);

     if (SgProject::get_verbose() > 0)
        printf ("Build support for implicit class (end) for class = %s \n",name.str());



     ROSE_ASSERT(astJavaScopeStack.empty() == false);
     outputJavaState("cactionBuildImplicitClassSupportEnd");

  // Experiment with ERROR on C++ side...communicated to Java...and back to C++ side where the JVM is called by ROSE...
  // ROSE_ASSERT(false);

  // Pop the class definition off the scope stack...
     ROSE_ASSERT(astJavaScopeStack.empty() == false);
     astJavaScopeStack.pop_front();

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

#if 0
     SgGlobal* globalScope = getGlobalScope();
     NodeQuerySynthesizedAttributeType nodeList = NodeQuery::querySubTree(globalScope,V_SgClassDeclaration);

     if (SgProject::get_verbose() > 0)
        {
          printf ("Output the nodeList (current classes in AST) = \n");
       // for (list<SgName>::iterator i = astJavaImplicitClassList.begin(); i != astJavaImplicitClassList.end(); i++)
          for (NodeQuerySynthesizedAttributeType::iterator i = nodeList.begin(); i != nodeList.end(); i++)
             {
            // SgName classNameWithQualification = *i;
            // if (SgProject::get_verbose() > -1)
            //      printf ("   --- implicit class = %s \n",classNameWithQualification.str());

               SgClassDeclaration* temp_classDeclaration = isSgClassDeclaration(*i);
               ROSE_ASSERT(temp_classDeclaration != NULL);
               printf ("   --- nodeList: %s \n",temp_classDeclaration->get_name().str());
             }
        }

  // We can't wait until the end of the processing for a class to build the SgAliasSymbol for the class.
  // Else all the references that are found via recursion will not be resolved.  So this functionality
  // to build the SgAliasSymbols is moved to Java_JavaParser_cactionBuildImplicitClassSupportStart().

     printf ("In Java_JavaParser_cactionBuildImplicitClassSupportEnd(): Exiting as a test! \n");
     ROSE_ASSERT(false);

     if (SgProject::get_verbose() > 0)
          printf ("Process the list of classes (nodeList) \n");

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

               if (classDeclarationList.size() != 1)
                  {
                    printf ("In cactionBuildImplicitClassSupportEnd(): classDeclarationList.size() = %zu \n",classDeclarationList.size());
                    outputJavaState("In buildImplicitClass(): Pop the class built implicitly from the stack.");
                  }
            // ROSE_ASSERT(classDeclarationList.size() == 1);

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
#endif
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
    // else if (name == "String")
       else if (name == "java.lang.String")
        {
       // DQ (4/9/2011): Added support for "String" type.
       // This should maybe be a SgClassType specific to the String class, instead of using the SgTypeString for Java.
       // astJavaTypeStack.push_front(SgTypeString::createType());

          SgClassType* classType = lookupTypeFromQualifiedName(name);
          if (classType == NULL)
             {
            // If the "String" class was not found then it is likely because we are in a debug mode which limits the number of implecit classes.
               printf ("Build a class for String: name = %s\n",name.str());
               outputJavaState("In cactionGenerateType case of java.lang.String");

               buildImplicitClass(name);
               printf ("DONE: Build a class for String: name = %s\n",name.str());

               outputJavaState("DONE: In cactionGenerateType case of java.lang.String");

            // We need to leave a SgType on the astJavaTypeStack, we need to build the class to build 
            // the SgClassType, but we don't want to leave a SgClassDefinition on the astJavaScopeStack.
               printf ("When we just build a type we don't want the new class definition on the stack. \n");
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
                    printf ("On the second search for the class = %s (after building it explicitly) it was found! \n",name.str());
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
          printf ("Error: default reached in switch in Java_JavaParser_cactionGenerateType() (name = %s) \n",name.str());
          ROSE_ASSERT(false);
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

JNIEXPORT void JNICALL Java_JavaParser_cactionGenerateClassType (JNIEnv* env, jclass xxx, jstring java_string)
   {
     if (SgProject::get_verbose() > 0)
          printf ("Inside of Java_JavaParser_cactionGenerateClassType() \n");

     SgName name = convertJavaStringToCxxString(env,java_string);

  // printf ("In Java_JavaParser_cactionGenerateClassType(): Calling lookupSymbolFromQualifiedName(name = %s) \n",name.str());
#if 0
  // DQ (4/9/2011): Refactored this code to lookupTypeFromQualifiedName().

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
#else
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
#endif

     outputJavaState("At Bottom of cactionGenerateClassType");

#if 0
     printf ("Build support for class types (not finished) \n");
     ROSE_ASSERT(false);
#endif
   }

JNIEXPORT void JNICALL Java_JavaParser_cactionStatementEnd(JNIEnv *env, jclass xxx, jstring java_string /* JNIEnv *env, jobject xxx */ )
   {
     string label = convertJavaStringToCxxString(env,java_string);

     if (SgProject::get_verbose() > 0)
        printf ("Closing actions for statements label = %s\n",label.c_str());

     outputJavaState("At Top of cactionStatementEnd()");

  // When there is a statement left of the stack, append it to the current scope and pop 
  // it off the stack.  There should be at most a single statement on the stack (I think).
     if (astJavaStatementStack.empty() == false)
        {
          SgStatement* statement = astJavaStatementStack.front();
          ROSE_ASSERT(statement != NULL);

          astJavaStatementStack.pop_front();

          astJavaScopeStack.front()->append_statement(statement);
          ROSE_ASSERT(astJavaStatementStack.empty() == true);
        }
     ROSE_ASSERT(astJavaStatementStack.empty() == true);

     outputJavaState("At Bottom of cactionStatementEnd()");

  // DQ (4/6/2011): these stacks should be cleared now.
     ROSE_ASSERT(astJavaNodeStack.empty() == true);
     ROSE_ASSERT(astJavaInitializedNameStack.empty() == true);
     ROSE_ASSERT(astJavaStatementStack.empty() == true);
     ROSE_ASSERT(astJavaExpressionStack.empty() == true);

  // We want to at some point make this an error, but for now just a warning.
     if (astJavaTypeStack.empty() == false)
        {
          printf ("WARNING: clearing the astJavaTypeStack size = %zu \n",astJavaTypeStack.size());
          astJavaTypeStack.clear();
        }
  // ROSE_ASSERT(astJavaTypeStack.empty() == true);
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


// I could not make these work...
// JNIEXPORT void JNICALL Java_JavaParser_cactionImportReference(JNIEnv *env, jobject xxx, jstring java_string, jboolean java_containsWildcard)
// JNIEXPORT void JNICALL Java_JavaParser_cactionImportReference(JNIEnv *env, jclass xxx, jstring java_string, jboolean java_containsWildcard)
// JNIEXPORT void JNICALL Java_JavaParser_cactionImportReference(JNIEnv *env, jobject xxx, jstring java_string, jboolean java_containsWildcard)
// JNIEXPORT void JNICALL Java_JavaParser_cactionImportReference(JNIEnv *env, jobject xxx, jstring java_string)
JNIEXPORT void JNICALL Java_JavaParser_cactionImportReference(JNIEnv *env, jobject xxx, jstring java_string, jint java_containsWildcard)
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
     astJavaScopeStack.front()->append_statement(importStatement);
  // importStatement->set_parent(astJavaScopeStack.front());
     setJavaSourcePosition(importStatement);
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

     printf ("Building a variable declaration for name = %s \n",name.str());

  // Note that the type shuld have already been built and should be on the astJavaTypeStack.
     SgVariableDeclaration* variableDeclaration = buildSimpleVariableDeclaration(name);
     ROSE_ASSERT(variableDeclaration != NULL);

  // Save it on the stack so that we can add SgInitializedNames to it.
     astJavaStatementStack.push_front(variableDeclaration);

  // We don't want to add the statment to the current scpe until it is finished being built.
  // ROSE_ASSERT(astJavaScopeStack.empty() == false);
  // astJavaScopeStack.front()->append_statement(variableDeclaration);

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


