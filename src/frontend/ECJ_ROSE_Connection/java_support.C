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
     maxStackSize = astJavaExpressionStack.size() > maxStackSize ? astJavaExpressionStack.size() : maxStackSize;
     maxStackSize = astJavaNodeStack.size()       > maxStackSize ? astJavaNodeStack.size()       : maxStackSize;

     printf ("\n");
     printf ("\n");
     printf ("In outputState (%s): maxStackSize = %ld \n",label.c_str(),(long)maxStackSize);

     std::list<SgScopeStatement*>      ::reverse_iterator astScopeStack_iterator                = astJavaScopeStack.rbegin();
     std::list<SgExpression*>          ::reverse_iterator astExpressionStack_iterator           = astJavaExpressionStack.rbegin();
     std::list<SgNode*>                ::reverse_iterator astNodeStack_iterator                 = astJavaNodeStack.rbegin();

     const int NumberOfStacks = 3;
     struct
        { std::string name;
          int fieldWidth;
     } stackNames[NumberOfStacks] = { {"astScopeStack", 40} ,    {"astExpressionStack",30} ,   {"astNodeStack",30} };

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

          outputJavaStateSupport(s,stackNames[0].fieldWidth);

          if (astExpressionStack_iterator != astJavaExpressionStack.rend())
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

          outputJavaStateSupport(s,stackNames[1].fieldWidth);

          if (astNodeStack_iterator != astJavaNodeStack.rend())
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

          outputJavaStateSupport(s,stackNames[2].fieldWidth);

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


SgMemberFunctionDeclaration*
buildSimpleMemberFunction(const SgName & name)
   {
  // This is abstracted so that we can build member functions as require to define Java specific default functions (e.g. super()).

     printf ("Inside of buildSimpleMemberFunction(): name = %s \n",name.str());

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


void
buildClassSupport (const SgName & className, bool implicitClass)
   {
#if 1
     SgScopeStatement* outerScope = NULL;

  // Names of implicitly defined classes have names that start with "java." and these have to be translated.
     string original_classNameString = className.str();
     string classNameString = className.str();

  // Also replace '.' with '_'
     replace(classNameString.begin(), classNameString.end(),'.','_');

  // Also replace '$' with '_' (not clear on what '$' means yet (something related to inner and outer class nesting).
     replace(classNameString.begin(), classNameString.end(),'$','_');

     if (implicitClass == true)
        {
       // Nothing to do here.
        }
       else
        {
       // This is proper class.
          outerScope = astJavaScopeStack.front();
        }

     SgName name = classNameString;
#else
     SgName name = className;
#endif

  // We should not have a '.' in the class name.  Or it will fail the current ROSE name mangling tests.
     ROSE_ASSERT(classNameString.find('.') == string::npos);

  // DQ (3/20/2011): Detect use of '$' in class names. Current best reference 
  // is: http://www.java-forums.org/new-java/27577-specific-syntax-java-util-regex-pattern-node.html
     ROSE_ASSERT(classNameString.find('$') == string::npos);

     ROSE_ASSERT(astJavaScopeStack.empty() == false);

     printf ("In buildClass(%s): astJavaScopeStack.front() = %p = %s \n",name.str(),astJavaScopeStack.front(),astJavaScopeStack.front()->class_name().c_str());

     printf ("original_classNameString = %s \n",original_classNameString.c_str());

#if 0
  // SgScopeStatement* outerScope = astJavaScopeStack.front();
     SgScopeStatement* outerScope = NULL;
     bool implicitClass = false;
     if (original_classNameString.substr(0,5) == "java.")
        {
       // This is an implicit class (default class available in Java) (set to global scope).
          implicitClass = true;
       // outerScope = getGlobalScope(); // astJavaScopeStack.front();
        }
       else
        {
       // This is proper class.
          outerScope = astJavaScopeStack.front();
        }
#endif

  // ROSE_ASSERT(outerScope != NULL);

     SgClassDeclaration* declaration = SageBuilder::buildDefiningClassDeclaration ( name, astJavaScopeStack.front() );

     ROSE_ASSERT(declaration->get_type() != NULL);

     ROSE_ASSERT(declaration->get_firstNondefiningDeclaration() != NULL);

  // Set the source code position...
  // setSourcePosition(declaration);
  // setSourcePositionCompilerGenerated(declaration);

  // void setSourcePosition  ( SgLocatedNode* locatedNode );
  // void setSourcePositionCompilerGenerated( SgLocatedNode* locatedNode );

     ROSE_ASSERT(astJavaScopeStack.empty() == false);
     SgClassDefinition* definition = SageBuilder::buildClassDefinition(declaration);

  // Set the source code position...
  // setSourcePosition(definition);
  // setSourcePositionCompilerGenerated(definition);

     astJavaScopeStack.push_front(definition);
     ROSE_ASSERT(astJavaScopeStack.front()->get_parent() != NULL);

  // Add "super()" member function.
     SgMemberFunctionDeclaration* functionDeclaration = buildSimpleMemberFunction("super");
     ROSE_ASSERT(functionDeclaration != NULL);

     if (implicitClass == true)
        {
       // This branch makes a recursive call to this function (and computes the outerScope explicitly not using the astJavaScopeStack.

       // Implicit classes are put into the global scope at the top of the scope.
          SgScopeStatement* outerScope = getGlobalScope();
          ROSE_ASSERT(outerScope != NULL);

       // Parse the original_classNameString to a list of what will be classes.
          size_t lastPosition = 0;
          size_t position = original_classNameString.find('.',lastPosition);
          while (position != string::npos)
             {
               string parentClassName = original_classNameString.substr(lastPosition,position-lastPosition);
               printf ("parentClassName = %s \n",parentClassName.c_str());

               lastPosition = position+1;
               position = original_classNameString.find('.',lastPosition);
               printf ("lastPosition = %zu position = %zu \n",lastPosition,position);

            // Build the "java" and the "lang" classes if they don't already exist.
               SgClassSymbol * classSymbol = outerScope->lookup_class_symbol(parentClassName);
               if (classSymbol == NULL)
                  {
                 // This parent class does not exist so add it (must be added to the correct scope).
                    buildImplicitClass(parentClassName);
                    classSymbol = outerScope->lookup_class_symbol(parentClassName);
                    ROSE_ASSERT(classSymbol != NULL);
                  }
                 else
                  {
                 // Nothing to do here!
                  }

               outerScope = classSymbol->get_declaration()->get_definition();
               ROSE_ASSERT(outerScope != NULL);
             }

          printf ("In buildClass(%s) after building the SgClassDeclaration: astJavaScopeStack.front() = %p = %s \n",name.str(),outerScope,outerScope->class_name().c_str());
          outerScope->prepend_statement(declaration);
          ROSE_ASSERT(outerScope->generateStatementList().size() > 0);

#if 1
          printf ("Exiting as a test of implicitClass handling \n");
          ROSE_ASSERT(false);
#endif
        }
       else
        {
       // Add the class declaration to the current scope.  This might not be appropriate for implicit classes, but it helps in debugging the AST for now.
          ROSE_ASSERT(outerScope != NULL);
          printf ("In buildClass(%s) after building the SgClassDeclaration: astJavaScopeStack.front() = %p = %s \n",name.str(),outerScope,outerScope->class_name().c_str());
          outerScope->append_statement(declaration);
          ROSE_ASSERT(outerScope->generateStatementList().size() > 0);

       // Make sure that the new class has been added to the correct synbol table.
          ROSE_ASSERT (outerScope->lookup_class_symbol(declaration->get_name()) != NULL);
        }

     ROSE_ASSERT(declaration->get_parent() != NULL);
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
     SgVariableDeclaration* variable = NULL;

     variable = SageBuilder::buildVariableDeclaration (name, SgTypeInt::createType(), NULL, astJavaScopeStack.front() );
     ROSE_ASSERT(variable != NULL);

     return variable;
   }

