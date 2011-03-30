#include "sage3basic.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

// Java support for callinging C/C++.
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


void
setJavaSourcePosition( SgLocatedNode* locatedNode )
   {
  // This function sets the source position to be marked as not available (since we often don't have token information)
  // These nodes WILL be unparsed in the code generation phase.

  // The SgLocatedNode has both a startOfConstruct and endOfConstruct source position.
     ROSE_ASSERT(locatedNode != NULL);

  // Make sure we never try to reset the source position of the global scope (set elsewhere in ROSE).
     ROSE_ASSERT(isSgGlobal(locatedNode) == NULL);

  // Check the endOfConstruct first since it is most likely NULL (helpful in debugging)
     if (locatedNode->get_endOfConstruct() != NULL || locatedNode->get_startOfConstruct() != NULL)
        {
          printf ("In setSourcePosition(SgLocatedNode* locatedNode): locatedNode = %p = %s \n",locatedNode,locatedNode->class_name().c_str());
        }
     ROSE_ASSERT(locatedNode->get_endOfConstruct()   == NULL);
     ROSE_ASSERT(locatedNode->get_startOfConstruct() == NULL);

  // Call a mechanism defined in the SageInterface support
     SageInterface::setSourcePosition(locatedNode);
   }


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

     if ( SgProject::get_verbose() <= 2 )
        {
       // Skip output of stack data for verbose levels less than or equal to 2
          return;
        }

     size_t maxStackSize = astJavaScopeStack.size();
     maxStackSize = astJavaStatementStack.size()  > maxStackSize ? astJavaStatementStack.size()  : maxStackSize;
     maxStackSize = astJavaExpressionStack.size() > maxStackSize ? astJavaExpressionStack.size() : maxStackSize;
     maxStackSize = astJavaTypeStack.size()       > maxStackSize ? astJavaTypeStack.size()       : maxStackSize;
     maxStackSize = astJavaNodeStack.size()       > maxStackSize ? astJavaNodeStack.size()       : maxStackSize;

     printf ("\n");
     printf ("\n");
     printf ("In outputState (%s): maxStackSize = %ld \n",label.c_str(),(long)maxStackSize);

     std::list<SgScopeStatement*>      ::reverse_iterator astScopeStack_iterator                = astJavaScopeStack.rbegin();
     std::list<SgStatement*>           ::reverse_iterator astStatementStack_iterator            = astJavaStatementStack.rbegin();
     std::list<SgExpression*>          ::reverse_iterator astExpressionStack_iterator           = astJavaExpressionStack.rbegin();
     std::list<SgType*>                ::reverse_iterator astTypeStack_iterator                 = astJavaTypeStack.rbegin();
     std::list<SgNode*>                ::reverse_iterator astNodeStack_iterator                 = astJavaNodeStack.rbegin();

     const int NumberOfStacks = 5;
     struct
        { std::string name;
          int fieldWidth;
     } stackNames[NumberOfStacks] = { {"astScopeStack", 40},   {"astStatementStack",50} ,    {"astExpressionStack",50} ,    {"astTypeStack",30},   {"astNodeStack",30} };

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

          if (astNodeStack_iterator != astJavaNodeStack.rend())
             {
               s = (*astNodeStack_iterator)->class_name() + " : " + SageInterface::get_name(*astNodeStack_iterator);

               astNodeStack_iterator++;
             }
            else
             {
               s = " No Node ";
             }

          outputJavaStateSupport(s,stackNames[4].fieldWidth);

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

  // return str;
  // return string(str);
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

void
memberFunctionSetup (SgName & name, SgClassDefinition* classDefinition, SgFunctionParameterList* & parameterlist, SgMemberFunctionType* & return_type)
   {
  // Refactored code.

  // This is abstracted so that we can build member functions as require to define Java specific default functions (e.g. super()).

     ROSE_ASSERT(classDefinition != NULL);
     ROSE_ASSERT(classDefinition->get_declaration() != NULL);

     if (SgProject::get_verbose() > 0)
          printf ("Inside of memberFunctionSetup(): name = %s in scope = %p = %s = %s \n",name.str(),classDefinition,classDefinition->class_name().c_str(),classDefinition->get_declaration()->get_name().str());

     SgFunctionParameterTypeList* typeList = SageBuilder::buildFunctionParameterTypeList();
     ROSE_ASSERT(typeList != NULL);

  // Specify if this is const, volatile, or restrict (0 implies normal member function).
     unsigned int mfunc_specifier = 0;
     return_type = SageBuilder::buildMemberFunctionType(SgTypeVoid::createType(), typeList, classDefinition, mfunc_specifier);
     ROSE_ASSERT(return_type != NULL);

     parameterlist = SageBuilder::buildFunctionParameterList(typeList);
     ROSE_ASSERT(parameterlist != NULL);

     SgFunctionType* func_type = SageBuilder::buildFunctionType(return_type,parameterlist);
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
               if (SgProject::get_verbose() > -1)
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
     ROSE_ASSERT(declarationListSize > 0);

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
   }

SgMemberFunctionDeclaration*
buildNonDefiningMemberFunction(const SgName & inputName, SgClassDefinition* classDefinition)
   {
     SgName name = inputName;

     SgFunctionParameterList* parameterlist = NULL;
     SgMemberFunctionType* return_type = NULL;

  // Refactored code.
     memberFunctionSetup (name,classDefinition,parameterlist,return_type);

     ROSE_ASSERT(parameterlist != NULL);
     ROSE_ASSERT(return_type != NULL);

     SgMemberFunctionDeclaration* functionDeclaration = SageBuilder::buildNondefiningMemberFunctionDeclaration (name, return_type, parameterlist, classDefinition );

     ROSE_ASSERT(functionDeclaration != NULL);
     ROSE_ASSERT(functionDeclaration->get_definingDeclaration() == NULL);
     ROSE_ASSERT(functionDeclaration->get_definition() == NULL);

  // Add the member function to the class scope (this is a required second step after building the function declaration).
     classDefinition->append_statement(functionDeclaration);

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
     SgMemberFunctionType* return_type = NULL;

  // Refactored code.
     memberFunctionSetup (name,classDefinition,parameterlist,return_type);

     ROSE_ASSERT(parameterlist != NULL);
     ROSE_ASSERT(return_type != NULL);

     SgMemberFunctionDeclaration* functionDeclaration = SageBuilder::buildDefiningMemberFunctionDeclaration (name, return_type, parameterlist, classDefinition );

     ROSE_ASSERT(functionDeclaration != NULL);
     ROSE_ASSERT(functionDeclaration->get_definingDeclaration() != NULL);
     ROSE_ASSERT(functionDeclaration->get_definition() != NULL);

  // Add the member function to the class scope (this is a required second step after building the function declaration).
     classDefinition->append_statement(functionDeclaration);

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

#if 0
  // Ignore this requirement while we are debugging...

  // DQ (3/25/2011): Changed this to a non-defining declaration.
  // Add "super()" member function.
  // SgMemberFunctionDeclaration* functionDeclaration = buildSimpleMemberFunction("super",classDefinition);
     SgMemberFunctionDeclaration* functionDeclaration = buildNonDefiningMemberFunction("super",classDefinition);
     ROSE_ASSERT(functionDeclaration != NULL);

     size_t declarationListSize = classDefinition->generateStatementList().size();

     if (SgProject::get_verbose() > 0)
          printf ("declarationListSize = %zu \n",declarationListSize);
     ROSE_ASSERT(declarationListSize > 0);
#else
     if (SgProject::get_verbose() > 0)
          printf ("WARNING: Skipping addition of \"super\" member function for each class.\n");
#endif

     return declaration;
   }

void
buildClassSupport (const SgName & className, bool implicitClass)
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
          printf ("In buildClass(%s): astJavaScopeStack.front() = %p = %s \n",name.str(),astJavaScopeStack.front(),astJavaScopeStack.front()->class_name().c_str());
          printf ("original_classNameString = %s \n",original_classNameString.c_str());
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
                    SgClassDeclaration* implicitDeclaration = buildJavaClass(parentClassName, outerScope );
                    ROSE_ASSERT(implicitDeclaration != NULL);
                    ROSE_ASSERT(implicitDeclaration->get_definition() != NULL);

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

       // Here we build the class and associate it with the correct code (determined differently based on if this is an implicit or non-implicit class.
          SgClassDeclaration* declaration = buildJavaClass(name, outerScope );

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

          SgClassDeclaration* nonDefiningDeclaration = isSgClassDeclaration(declaration->get_firstNondefiningDeclaration());
          ROSE_ASSERT(nonDefiningDeclaration != NULL);

       // DQ (3/25/2011): Make sure we are using the correct declaration to build the symbol.
          ROSE_ASSERT(nonDefiningDeclaration->get_definingDeclaration() == declaration);
          ROSE_ASSERT(nonDefiningDeclaration->get_firstNondefiningDeclaration() != declaration);

          if (SgProject::get_verbose() > 0)
               printf ("Build a new SgClassSymbol for parentClassName = %s and declaration = %p = %s \n",name.str(),declaration,declaration->class_name().c_str());
       // SgClassSymbol* classSymbol = new SgClassSymbol(declaration);
          SgClassSymbol* classSymbol = new SgClassSymbol(nonDefiningDeclaration);

          if (SgProject::get_verbose() > 0)
               printf ("Insert the new SgClassSymbol = %p into scope = %p = %s \n",classSymbol,outerScope,outerScope->class_name().c_str());

       // DQ (3/24/2011): Make sure there is not already a symbol for this class here already.
          ROSE_ASSERT(outerScope->symbol_exists(name,classSymbol) == false);

          outerScope->insert_symbol(name,classSymbol);

          if (SgProject::get_verbose() > 0)
               printf ("Test to make sure the SgClassSymbol is in the symbol table. \n");
          classSymbol = outerScope->lookup_class_symbol(name);
          ROSE_ASSERT(classSymbol != NULL);

       // At this point we shuld still at least have the global scope on the stack.
          ROSE_ASSERT(astJavaScopeStack.empty() == false);

       // Note that this pushed only the new implicit class definition onto the stack 
       // and none of the parent class scopes. Is this going to be OK?
          ROSE_ASSERT(declaration->get_definition() != NULL);
          astJavaScopeStack.push_front(declaration->get_definition());
          ROSE_ASSERT(astJavaScopeStack.front()->get_parent() != NULL);

          ROSE_ASSERT(declaration->get_parent() != NULL);
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

       // Add the class declaration to the current scope.  This might not be appropriate for implicit classes, but it helps in debugging the AST for now.
          ROSE_ASSERT(outerScope != NULL);

          if (SgProject::get_verbose() > 0)
               printf ("In buildClass(%s : non-implicit class) after building the SgClassDeclaration: astJavaScopeStack.front() = %p = %s \n",name.str(),outerScope,outerScope->class_name().c_str());

          SgClassDefinition* outerScopeClassDefinition = isSgClassDefinition(outerScope);

          if (SgProject::get_verbose() > 0)
               printf ("outerScopeClassDefinition = %p = %s \n",outerScopeClassDefinition,outerScopeClassDefinition == NULL ? "NULL" : outerScopeClassDefinition->get_declaration()->get_name().str());

          outerScope->append_statement(declaration);
          ROSE_ASSERT(outerScope->generateStatementList().size() > 0);

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
     buildClassSupport (className,implicitClass);
   }

void
buildClass (const SgName & className)
   {
     bool implicitClass = false;
     buildClassSupport (className,implicitClass);
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
     SgVariableDeclaration* variable = SageBuilder::buildVariableDeclaration (name, type, NULL, astJavaScopeStack.front() );
     ROSE_ASSERT(variable != NULL);

     return variable;
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
     printf ("Exiting in stripQualifiers(): after computing the className \n");
     ROSE_ASSERT(false);
#endif

     return returnList;
   }


SgName
stripQualifiers (const SgName & classNameWithQualification)
   {
     list<SgName> l = generateQualifierList(classNameWithQualification);
     ROSE_ASSERT(l.empty() == false);

     if (SgProject::get_verbose() > 0)
          printf ("result in stripQualifiers(%s) = %s \n",classNameWithQualification.str(),l.back().str());

     return l.back();
   }

