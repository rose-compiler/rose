// This is the location of all the name qualification support functions
// required for code generation (only applicable to C++)

// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "unparser.h"
// #include "name_qualification_support.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// DQ (4/16/2007): Commented out.
// #define OUTPUT_DEBUGGING_FUNCTION_BOUNDARIES 1
// #define OUTPUT_DEBUGGING_FUNCTION_INTERNALS  0
// #define OUTPUT_DEBUGGING_UNPARSE_INFO        0

#if 0
       // DQ (5/11/2011): OLD API.
bool
Unparser_Nameq::isAnOuterScope( SgScopeStatement* currentScope, SgScopeStatement* targetScope )
   {
  // DQ (6/4/2007): This is true iff targetScope is an outer enclosing scope of currentScope.
  // In this case we need name qualification to resolve the identifier (name).

     bool result = false;

  // DQ (6/27/2007): Allow the special case of where we just don't know the current scope!
     if (currentScope == NULL)
        {
       // If we can't identify an associated scope we assume the name is not visible in an outer scope, triggering fully qualified names.
#if PRINT_DEVELOPER_WARNINGS
          printf ("In Unparser::isAnOuterScope(): Allow the special case of where we just don't know the current scope \n");
#endif
          return false;
        }

     ROSE_ASSERT(currentScope != NULL);
     ROSE_ASSERT(targetScope  != NULL);
#if 0
     printf ("currentScope = %p = %s = %s \n",currentScope,currentScope->class_name().c_str(),SageInterface::get_name(currentScope).c_str());
     printf ("targetScope  = %p = %s = %s \n",targetScope,targetScope->class_name().c_str(),SageInterface::get_name(targetScope).c_str());
#endif

     SgScopeStatement* tempScope = currentScope;
     ROSE_ASSERT(tempScope != NULL);
     while ( (tempScope != NULL) && (tempScope != targetScope) && (isSgGlobal(tempScope) == NULL) )
        {
          ROSE_ASSERT(tempScope != NULL);
       // printf ("tempScope = %p = %s = %s \n",tempScope,tempScope->class_name().c_str(),SageInterface::get_name(tempScope).c_str());
          tempScope = tempScope->get_scope();
        }

     if (tempScope == targetScope)
        result = true;

  // printf ("Unparser_Nameq::isAnOuterScope() = %s \n",(result == true) ? "true" : "false");

     return result;
   }
#endif


#if 0
       // DQ (5/11/2011): OLD API.
void
generateDeclarationAndName( SgScopeStatement* inputScope, SgName & name, SgDeclarationStatement* & declaration )
   {
  // This function generates a name and the associated declaration for any input scope (if such declaration exists).

  // Only a restricted set of scopes are allowed here, those that are named
  // ROSE_ASSERT(inputScope->supportsDefiningFunctionDeclaration() == true);
     if (inputScope->supportsDefiningFunctionDeclaration() == false)
        {
       // This could be a function definition as in test2005_06.C.
#if PRINT_DEVELOPER_WARNINGS
          printf ("Warning: specification of a input SgScopeStatement = %p = %s for which inputScope->supportsDefiningFunctionDeclaration() == false (return)\n",inputScope,inputScope->class_name().c_str());
#endif
          return;
        }

     switch (inputScope->variantT())
        {
          case V_SgGlobal:
             {
            // global scope does not have an associated declaration (or a name)
            // name        = "::";
               name        = "";
               declaration = NULL;
               break;
             }

          case V_SgNamespaceDefinitionStatement:
             {
            // A namespace usually has an associated declaration (though "std" can sometimes be builtin and can be a problem)
               SgNamespaceDefinitionStatement* scope = isSgNamespaceDefinitionStatement(inputScope);
               SgNamespaceDeclarationStatement* namespaceDeclaration = scope->get_namespaceDeclaration();
               ROSE_ASSERT(namespaceDeclaration != NULL);

               name        = namespaceDeclaration->get_name();
               declaration = namespaceDeclaration;
               break;
             }

          case V_SgClassDefinition:
             {
            // A class always  has an associated declaration
               SgClassDefinition* scope = isSgClassDefinition(inputScope);
               SgClassDeclaration* classDeclaration = scope->get_declaration();
               ROSE_ASSERT(classDeclaration != NULL);
               ROSE_ASSERT(isSgTemplateInstantiationDecl(classDeclaration) == NULL);

               name        = classDeclaration->get_name();
            // printf ("case V_SgClassDefinition: name = %s \n",name.str());
               declaration = classDeclaration;
               break;
             }

          case V_SgTemplateInstantiationDefn:
             {
            // A namespace usually has an associated declaration (though "std" can sometimes be builtin and can be a problem)
               SgTemplateInstantiationDefn* scope = isSgTemplateInstantiationDefn(inputScope);
               SgTemplateInstantiationDecl* templateInstantiationDeclaration = isSgTemplateInstantiationDecl(scope->get_declaration());
               ROSE_ASSERT(templateInstantiationDeclaration != NULL);

            // DQ (6/9/2007): get_name() returns the name with template arguments while get_templateName() returns only the template name.
            // name        = templateInstantiationDeclaration->get_templateName();
               name        = templateInstantiationDeclaration->get_name();
            // printf ("case V_SgTemplateInstantiationDefn: name = %s \n",name.str());
            // printf ("case V_SgTemplateInstantiationDefn: templateInstantiationDeclaration->get_name() = %s \n",templateInstantiationDeclaration->get_name().str());
            // printf ("case V_SgTemplateInstantiationDefn: templateInstantiationDeclaration->get_templateName() = %s \n",templateInstantiationDeclaration->get_templateName().str());
               declaration = templateInstantiationDeclaration;
               break;
             }

          default:
             {
               printf ("Default reached in generateNameQualifier( SgInitializedName*,,)  initializedName->get_scope() = %p = %s \n",inputScope,inputScope->class_name().c_str());
               ROSE_ASSERT(false);
             }
        }
   }
#endif


#if 0
       // DQ (5/11/2011): OLD API.

SgName 
Unparser_Nameq::generateNameQualifierForType( SgType* type, const SgUnparse_Info & info )
   {
     SgName nameQualifier;     

     return nameQualifier;
   }
#endif


#if 0
       // DQ (5/11/2011): OLD API.

SgName
Unparser_Nameq::generateNameQualifier( SgDeclarationStatement* declarationStatement, const SgUnparse_Info & info, bool nestedQualification )
   {
  // DQ (11/11/2004) This function encapsulates the details of determining the qualifier 
  // for a qualified name.  It is useful for references to named types, variables, 
  // functions, and member functions.
  // There are a few C++ constructs where name qualification is forced (the context 
  // requires name qualification and the function can determine the context, I think). 
  // Here, the function "info.set_forceQualifiedNames()" is used, these cases are:
  //    1) function parameter types
  //    2) template arguments
  //    3) types appearing in variable declarations
  // In addition there are a number of cases where the name qualification is skipped depending 
  // on context (calling "info.set_SkipQualifiedNames()"), these are:
  //    1) output of type definitions in variable declarations (e.g. "struct X { int i; } X_variable;")
  //    2) output of type definitions in typedef declarations (e.g. "typedef struct X { int i; } X_type;")
  //    3)

     ROSE_ASSERT(declarationStatement != NULL);

     bool outputQualifiedName = !info.SkipQualifiedNames() && (SageInterface::is_Cxx_language() == true);

#define TRACE_NAME_QUALIFICATION 0

#if TRACE_NAME_QUALIFICATION
     printf ("\n***** In generateNameQualifier declaration = %p = %s = %s : This branch should output a %s name? \n",
          declarationStatement,declarationStatement->class_name().c_str(),
          SageInterface::get_name(declarationStatement).c_str(),outputQualifiedName ? "QUALIFIED" : "NON-QUALIFIED");
#endif

//#if TRACE_NAME_QUALIFICATION && false
#if TRACE_NAME_QUALIFICATION 
#if 0
     cur << "/* In generateNameQualifier info.SkipQualifiedNames()                         = " << (info.SkipQualifiedNames() == true ? "true" : "false") << " */\n ";
     cur << "/* In generateNameQualifier declarationStatement                              = " << declarationStatement->class_name() << " */\n ";
     cur << "/* In generateNameQualifier info.get_current_scope()                          = " << StringUtility::numberToString(info.get_current_scope()) << " */\n ";
     cur << "/* In generateNameQualifier info.get_declstatement_ptr()                      = " << StringUtility::numberToString(info.get_declstatement_ptr()) << " */\n ";
     cur << "/* In generateNameQualifier info.get_current_namespace()                      = " << (info.get_current_namespace() != NULL ? info.get_current_namespace()->get_name().str() : "NO NAMESPACE") << " */\n ";
     cur << "/* In generateNameQualifier info.get_current_function_call()                  = " << (info.get_current_function_call() != NULL ? SageInterface::get_name(info.get_current_function_call()) : "NO FUNCTION CALL") << " */\n ";
     cur << "/* In generateNameQualifier declarationStatement->get_scope()                 = " << declarationStatement->get_scope()->class_name() << " */\n ";
  // cur << "/* In generateNameQualifier declarationStatement->get_definingDeclaration()   = " << declarationStatement->get_definingDeclaration() << " */\n ";
     cur << "/* In generateNameQualifier declarationStatement->get_definingDeclaration()   = " << (declarationStatement->get_definingDeclaration() != NULL ? "VALID" : "NULL") << " */\n ";
     cur << "/* In generateNameQualifier declarationStatement->get_parent() == get_scope() = " << ((declarationStatement->get_parent() == declarationStatement->get_scope()) ? "true" : "false") << " */\n ";

  // info.display("In generateNameQualifier(): The C++ support is more complex and can require qualified names");
#endif
#endif

  // DQ (6/6/2007): If the scope supports defining declaration then the scope can be named
  // and thus such declarations can be name qualified, else name qualification is not defined.
  // For example a class defined in a function can't be refered to with name qualification 
  // from another scope (and it visible only in the function body and any nested scopes of 
  // the function). However, a declaration in namespace can be refed to using name qualification
  // through the namespace and a declaration in global scope is always visible through global 
  // qualification.
     ROSE_ASSERT(declarationStatement->get_scope() != NULL);
     outputQualifiedName = (declarationStatement->get_scope()->supportsDefiningFunctionDeclaration() == true);

#if TRACE_NAME_QUALIFICATION
  // cur << "/* In generateNameQualifier outputQualifiedName  = " << (outputQualifiedName ? "true" : "false") << " */\n ";
     printf ("In generateNameQualifier outputQualifiedName  = %s \n",(outputQualifiedName ? "true" : "false"));
#endif

  // DQ (6/9/2007): Make an exception for compiler generated variables (e.g."__PRETTY_FUNCTION__")
  // if (initializedName->get_startOfConstruct()->isCompilerGenerated() == true)
     string builtinFunctionName = "__builtin";
     if ( (declarationStatement->get_startOfConstruct()->isCompilerGenerated() == true) && (SageInterface::get_name(declarationStatement).substr(0,builtinFunctionName.size()) == builtinFunctionName) )
        {
#if 0
          printf ("declarationStatement = %p = %s is compiler generated so it should not be qualified \n",declarationStatement,SageInterface::get_name(declarationStatement).c_str());
          declarationStatement->get_startOfConstruct()->display("builtin function: compiler generated so it should not be qualified");
#endif
          outputQualifiedName = false;
        }

  // Return value for this function
     SgName  qualifier;

  // DQ (1/7/2007): This make sure that we don't use name qualifification for "C" language code.
     if (outputQualifiedName == true)
        {
       // DQ (6/3/2007): In the scope where the variable is declared name qualification is never required, so don't
       // test that scope.  Look at where the variable declaration is used instead (get from SgUnparse_Info object).
       // SgScopeStatement* scopeStatement = initializedName->get_scope();
          SgScopeStatement* scopeStatement = info.get_current_scope();

#if TRACE_NAME_QUALIFICATION
       // ROSE_ASSERT(info.get_current_scope() != NULL);
       // printf ("Current scope: info.get_current_scope() = %p = %s \n",info.get_current_scope(),info.get_current_scope()->class_name().c_str());
          printf ("Current scope: info.get_current_scope() = %p = %s \n",scopeStatement,(scopeStatement != NULL ? scopeStatement->class_name().c_str() : "NULL"));
          if (scopeStatement != NULL)
             {
               printf ("Current scope: info.get_current_scope() = %p = %s = %s \n",scopeStatement,scopeStatement->class_name().c_str(),SageInterface::get_name(scopeStatement).c_str());
             }
          ROSE_ASSERT(declarationStatement->get_scope() != NULL);
          printf ("declarationStatement->get_scope()       = %p = %s \n",declarationStatement->get_scope(),declarationStatement->get_scope()->class_name().c_str());
#endif

       // If this is NULL then assume that this is being called from the unparseToString function 
       // (where it is difficult to establish the scope for SgType an SgSymbol IR node). We can't 
       // get the qualification correct in this case since we often can't even compute the calling 
       // context.  This will have to be handled later.
          if (scopeStatement == NULL)
             {
#if PRINT_DEVELOPER_WARNINGS
               printf ("info.get_current_scope() is NULL, likely part of a transformation, OK (skip generation of qualified name) \n");
#endif
             }
       // ROSE_ASSERT(scopeStatement != NULL);

       // In general the whole name qualification mechanism should be recursive to generate minimally qualified names.

       // DQ (9/8/2007): This will be checked in isAnOuterScope(), so let's catch such errors as early as possible.
          ROSE_ASSERT(declarationStatement->get_scope() != NULL);

       // DQ (7/4/2007): We sometimes require name qualification independent of the hidden list mechanism. For example
       // when qualification is required to resolve a name sepearately from when we are only trying to disambiguate between names.
          bool requiredNameIsVisible = unp->u_name->isAnOuterScope(scopeStatement,declarationStatement->get_scope());

#if TRACE_NAME_QUALIFICATION
          printf ("##### Visability rules also require tracking down where using directives and declarations have made names visible (not implemented yet) ##### \n");
          printf ("declarationStatement requiredNameIsVisible = %s \n",(requiredNameIsVisible == true) ? "true" : "false");
#endif
          if (requiredNameIsVisible == true)
             {
            // This case is to disambiguate between names, and if they are hidden by other names we have to use name qualification to do this.

            // Make this a reference later
               set<SgSymbol*> hidden_declaration_list = scopeStatement->get_hidden_declaration_list();
               set<SgSymbol*> hidden_type_list        = scopeStatement->get_hidden_type_list();

            // List of symbols who's declaration require type elaboration when uses as names (e.g. "class X; int X; class X* xptr;")
               set<SgSymbol*> hidden_elaboration_list = scopeStatement->get_type_elaboration_list();

#if TRACE_NAME_QUALIFICATION
            // DQ (6/6/2007): Currently there is a bug which causes all the types to be put into the hidden_elaboration_list
            // This test fails for test2005_144.C (I don't know why)
            // ROSE_ASSERT(hidden_type_list.empty() == true);
               if (hidden_type_list.empty() == false)
                  {
                    printf ("########## WARNING: ADDING symbols into hidden_type_list using symbols in hidden_elaboration_list ########## \n");
                  }
#endif

            // DQ (6/10/2007): concatinate the lists instead of overwriting the hidden_type_list.
            // hidden_type_list = hidden_elaboration_list;
               hidden_type_list.insert(hidden_elaboration_list.begin(),hidden_elaboration_list.end());
               hidden_elaboration_list.erase(hidden_elaboration_list.begin(),hidden_elaboration_list.end());
#if 0
               outputHiddenListData (scopeStatement);
#endif
            // DQ (6/6/2007): We have to check the hidden list of symbols to check if name qualification is required.  
            // To do this we need to get the symbol associated with the input declaration. To do this we have to identify 
            // the declaration to use to search for the symbol in the symbol table.  Different types of declarations
            // us either the defining or nodefining (or don't care) and associate them with symbols.  It is not practical
            // to have all types of symbols use either the non-defining or defining declarations.  Eg. namespaces use the 
            // non-defining since there could be multiple defining declarations; functions can use either defining or first 
            // non-defining declaration; while class symbols always use the first non-defining declaration with their symbols.
            // Enums typically use the defining declaration, but only if there are no forward enum declarations (which is 
            // non-standard C but implemented by most compilers and so supported in ROSE).
               SgDeclarationStatement* declarationAssociatedWithSymbol = NULL;

            // DQ (6/8/2007): Simpler implementation
            // A SgFunctionSymbol uses the firstNondefiningDeclaration if it is available.
            // declarationAssociatedWithSymbol = declarationStatement->get_firstNondefiningDeclaration();
            // DQ (7/4/2007): New function to refactor code and provide basis for internal testing.
#if 0
               printf ("In generateNameQualifier(): declarationStatement = %p = %s = %s \n",declarationStatement,declarationStatement->class_name().c_str(),SageInterface::get_name(declarationStatement).c_str());
               printf ("In generateNameQualifier(): declarationStatement->get_scope() = %p = %s \n",declarationStatement->get_scope(),declarationStatement->get_scope()->class_name().c_str());
#endif
               declarationAssociatedWithSymbol = declarationStatement->get_declaration_associated_with_symbol();
               if (declarationAssociatedWithSymbol == NULL)
                  {
                    cerr << "Fatal error: declaration without symbol for " << SageInterface::get_name(declarationStatement) << endl;
                    ROSE_ASSERT(declarationAssociatedWithSymbol != NULL);
                  }

               if (declarationAssociatedWithSymbol->get_symbol_from_symbol_table() == NULL)
                  {
                    printf ("declarationAssociatedWithSymbol = %p = %s = %s \n",declarationAssociatedWithSymbol,declarationAssociatedWithSymbol->class_name().c_str(),SageInterface::get_name(declarationAssociatedWithSymbol).c_str());
                    SgScopeStatement* scope = declarationAssociatedWithSymbol->get_scope();
                    printf ("declarationAssociatedWithSymbol scope = %p = %s = %s \n",scope,scope->class_name().c_str(),SageInterface::get_name(scope).c_str());
                    scope->get_symbol_table()->print("declarationAssociatedWithSymbol");
                  }
               ROSE_ASSERT(declarationAssociatedWithSymbol->get_symbol_from_symbol_table() != NULL);

            // Get the symbol associated with a specific declaration
               SgSymbol* associatedSymbol = declarationAssociatedWithSymbol->get_symbol_from_symbol_table();
               if (associatedSymbol == NULL)
                  {
                    printf ("Could not find associatedSymbol using declarationAssociatedWithSymbol = %p = %s \n",
                         declarationAssociatedWithSymbol,declarationAssociatedWithSymbol->class_name().c_str());
                  }
               ROSE_ASSERT(associatedSymbol != NULL);

#if TRACE_NAME_QUALIFICATION
               printf ("associatedSymbol = %p = %s \n",associatedSymbol,associatedSymbol->class_name().c_str());
#endif
               SgScopeStatement* declarationStatementScope = declarationStatement->get_scope();
               ROSE_ASSERT(declarationStatementScope != NULL);

            // Handle the case of a hidden declaration (function, variable, namespace, etc.)
               set<SgSymbol*>::iterator declarationIterator = hidden_declaration_list.find(associatedSymbol);

#if TRACE_NAME_QUALIFICATION
               printf ("info.forceQualifiedNames() = %s \n",info.forceQualifiedNames() ? "true" : "false");
#endif
            // ROSE_ASSERT(info.forceQualifiedNames() == false);

            // DQ (8/1/2007): Work with Andreas has pointed out that unparse to string does not respect where we want to force the 
            // generation of qualified names (which is NOT the default for unparseToString(), but can be set optionally using an 
            // optional SgUnparse_Info object).
            // if (declarationIterator != hidden_declaration_list.end())
               if (declarationIterator != hidden_declaration_list.end() || info.forceQualifiedNames() == true)
                  {
                 // If the associatedSymbolOfHiddenDeclaration exists then the associated declaration is hidden and any reference to it should be qualified.
                 // SgSymbol* associatedSymbolOfHiddenDeclaration = *declarationIterator;

#if TRACE_NAME_QUALIFICATION
                    printf ("Matching symbol in hidden_declaration_list: declarationStatement = %p = %s = %s \n",declarationStatement,declarationStatement->class_name().c_str(),SageInterface::get_name(declarationStatement).c_str());
                 // printf ("hiddenDeclaration    = %p = %s = %s \n",hiddenDeclaration,hiddenDeclaration->class_name().c_str(),SageInterface::get_name(hiddenDeclaration).c_str());
                    printf ("Matching symbol in hidden_declaration_list: declarationStatement->get_firstNondefiningDeclaration() = %p \n",declarationStatement->get_firstNondefiningDeclaration());
                 // printf ("hiddenDeclaration->get_firstNondefiningDeclaration()    = %p \n",hiddenDeclaration->get_firstNondefiningDeclaration());
#endif
                    SgName name;
                    SgDeclarationStatement* declaration = NULL;
#if TRACE_NAME_QUALIFICATION
                    printf ("Calling generateDeclarationAndName using declarationStatementScope = %p = %s \n",declarationStatementScope,declarationStatementScope->class_name().c_str());
#endif
                    generateDeclarationAndName (declarationStatementScope,name,declaration);

#if TRACE_NAME_QUALIFICATION
                    printf ("Output of generateDeclarationAndName(): declaration = %p \n",declaration);
#endif
                    SgName baseName;
                    if (declaration != NULL)
                         baseName = generateNameQualifier(declaration,info,/* nestedQualifiaction */ true);

                    if (isSgGlobal(declarationStatementScope) != NULL)
                         name = "::";

                 // Process a few exceptions:
                 //  1) function declarations never have global qualification (their use in function calls may).

                 // Use the context information provide in the SgUnparse_Info object. However friend functions can have name qualification
                    SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(info.get_declstatement_ptr());
                    if ( (functionDeclaration != NULL) && (functionDeclaration->get_declarationModifier().isFriend() == false) )
                       {
#if TRACE_NAME_QUALIFICATION
                         printf ("This is part of the unparsing of a friend function declaration so remove the global qualification if present \n");
#endif
                         if (name == "::")
                              name = "";
                       }

#if TRACE_NAME_QUALIFICATION
                    printf ("After class declaration test: baseName = %s name = %s \n",baseName.str(),name.str());
#endif
                 // Use the context information provide in the SgUnparse_Info object.
                 // printf ("info.get_current_scope() = %p \n",info.get_current_scope());
                    ROSE_ASSERT(info.get_current_scope() != NULL);
                    if (declarationStatementScope == info.get_current_scope())
                       {
#if TRACE_NAME_QUALIFICATION
                         printf ("This is part of the unparsing of a definition defined in the current scope remove the qualification if present \n");
#endif
                         name = "";

                      // DQ (8/6/2007): Reset the baseName as well to prevent any qualification (see test2004_42.C and test2007_119.C)
                         baseName = "";
                       }

#if TRACE_NAME_QUALIFICATION
                    printf ("After declaration in current scope test: baseName = %s name = %s \n",baseName.str(),name.str());
#endif
                    qualifier = SgName::assembleQualifiedName(baseName,name);
                  }

            // Handle the case of a hidden type (class, enum, typedef)
               set<SgSymbol*>::iterator typeIterator = hidden_type_list.find(associatedSymbol);
               if (typeIterator != hidden_type_list.end())
                  {
                 // If the associatedSymbolOfHiddenType exists then the associated type is hidden and any reference to it should be qualified.
                 // SgSymbol* associatedSymbolOfHiddenType = *typeIterator;
                 // printf ("associatedSymbolOfHiddenType = %p = %s \n",associatedSymbolOfHiddenType,associatedSymbolOfHiddenType->class_name().c_str());

                    SgName name;
                    SgDeclarationStatement* declaration = NULL;
                 // printf ("Matching symbol in hidden_type_list: Calling generateDeclarationAndName using declarationStatementScope = %p = %s \n",declarationStatementScope,declarationStatementScope->class_name().c_str());
                    generateDeclarationAndName (declarationStatementScope,name,declaration);
                 // printf ("Matching symbol in hidden_type_list: Output of generateDeclarationAndName(): declaration = %p \n",declaration);

                    SgName baseName;
                    if (declaration != NULL)
                         baseName = generateNameQualifier(declaration,info,/* nestedQualifiaction */ true);

                    if (isSgGlobal(declarationStatementScope) != NULL)
                         name = "::";
#if 1
                 // Process a few exceptions:
                 //  1) function declarations never have global qualification (their use in function calls may).

                 // Use the context information provide in the SgUnparse_Info object.
                 // printf ("info.get_declstatement_ptr() = %p \n",info.get_declstatement_ptr());
                 // ROSE_ASSERT(info.get_declstatement_ptr() != NULL);
                    ROSE_ASSERT(info.get_declstatement_ptr() != NULL || nestedQualification == true);
                 // if (isSgClassDeclaration(info.get_declstatement_ptr()) != NULL)
                    if (isSgClassDeclaration(info.get_declstatement_ptr()) == declarationStatement)
                       {
#if TRACE_NAME_QUALIFICATION
                         printf ("This is part of the unparsing of a class declaration so remove the global qualification if present \n");
#endif
                      // if (name == "::")
                              name = "";

                      // ROSE_ASSERT(false);
                       }
#endif
#if TRACE_NAME_QUALIFICATION
                    printf ("After class declaration test: baseName = %s name = %s \n",baseName.str(),name.str());
#endif
                 // Use the context information provide in the SgUnparse_Info object.
                 // printf ("info.get_current_scope() = %p \n",info.get_current_scope());
                    ROSE_ASSERT(info.get_current_scope() != NULL);
                 // printf ("declarationStatement     = %p = %s = %s \n",declarationStatement,declarationStatement->class_name().c_str(),SageInterface::get_name(declarationStatement).c_str());
                 // printf ("info.get_current_scope() = %p = %s = %s \n",info.get_current_scope(),info.get_current_scope()->class_name().c_str(),SageInterface::get_name(info.get_current_scope()).c_str());
                    if (declarationStatementScope == info.get_current_scope())
                       {
#if TRACE_NAME_QUALIFICATION
                         printf ("This is part of the unparsing of a type defined in the current scope remove the qualification if present \n");
#endif
                         name = "";
                       }

#if TRACE_NAME_QUALIFICATION
                    printf ("After declaration in current scope test: baseName = %s name = %s \n",baseName.str(),name.str());
#endif
                    qualifier = SgName::assembleQualifiedName(baseName,name);
                  }
             }
            else
             {
            // This case is where a name is not visible unless we use name qualification. In this case we will not
            // require global qualification unless the top level name requires it.  We will have to make this
            // recursive to handle this case.

            // DQ (6/4/2007): Need to get the qualified name of the scope to which the initializedName is associated.
            // SgName basename = "";
            // SgName basename = initializedName->get_scope()->get_qualified_name();
            // qualifier = SgName::assembleQualifiedName(scopeStatement->get_qualified_name(),basename);
            // qualifier = SgName::assembleQualifiedName(declarationStatement->get_scope()->get_qualified_name(),basename);

               SgName name;
               SgDeclarationStatement* declaration = NULL;
               SgScopeStatement* declarationStatementScope = declarationStatement->get_scope();

            // DQ (6/27/2007): I think we should assert this explicitly or handle this case explicitly!
               ROSE_ASSERT(declarationStatementScope != NULL);

               generateDeclarationAndName (declarationStatementScope,name,declaration);

            // This permits a recursive evaluation of qualified names back to the global scope
            // This global qualification
               SgName baseName;
               if (declaration != NULL)
                    baseName = generateNameQualifier(declaration,info,/* nestedQualifiaction */ true);

#if TRACE_NAME_QUALIFICATION
               printf ("baseName = %s name = %s \n",baseName.str(),name.str());
#endif
               qualifier = SgName::assembleQualifiedName(baseName,name);
             }

          string qualifierString = qualifier.str();
          string::size_type qualifierStringLength = qualifierString.size();
       // printf ("qualifierString = %s qualifierStringLength = %ld \n",qualifierString.c_str(),qualifierStringLength);

       // Only add a trailing "::" if this is not a nested call to generateNameQualifier
          if (nestedQualification == false && qualifier.is_null() == false && ( (qualifierStringLength == 1) || ( (qualifierStringLength >= 2) && (qualifierString.rfind("::") != qualifierStringLength-2) ) ) )
             {
#if TRACE_NAME_QUALIFICATION
               printf ("In generateNameQualifier( SgDeclarationStatement*,,): Adding :: to qualifier = %s \n",qualifier.str());
#endif
               qualifier += "::";
             }

        } // end of block for outputQualifiedName == true

#if TRACE_NAME_QUALIFICATION
     printf ("Leaving generateNameQualifier( SgDeclarationStatement*,,): qualifier = %s \n",(qualifier.is_null() == false) ? qualifier.str() : "NULL");
#endif

     return qualifier;
   }
#endif


#if 0
       // DQ (5/11/2011): OLD API.

SgName
Unparser_Nameq::generateNameQualifier( SgInitializedName* initializedName, const SgUnparse_Info& info, bool nestedQualification )
   {
  // Return value for this function
     SgName  qualifier;

  // DQ (11/30/2004): If the prev_decl_item is available then use its variable declaration, else we 
  // are the prev_decl_item and we are in the class in which case we don't need a qualified name!

#define TRACE_NAME_QUALIFICATION_FOR_INITIALIZED_NAME 0

#if TRACE_NAME_QUALIFICATION_FOR_INITIALIZED_NAME
     printf ("\nIn generateNameQualifier( SgInitializedName*,,): initializedName = %p = %s initializedName->get_prev_decl_item() = %p \n",initializedName,initializedName->get_name().str(),initializedName->get_prev_decl_item());
#endif

     bool outputQualifiedName = !info.SkipQualifiedNames() && (SageInterface::is_Cxx_language() == true);

#if TRACE_NAME_QUALIFICATION_FOR_INITIALIZED_NAME
     printf ("***** This branch should output a %s name? \n",outputQualifiedName ? "QUALIFIED" : "NON-QUALIFIED");
#endif

  // DQ (1/11/2009): Added support for __FUNCTION__ (see test2004_153.C) and __BASE_FILE__. 
  // DQ (6/9/2007): Make an exception for compiler generated variables (e.g."__PRETTY_FUNCTION__")
  // if (initializedName->get_startOfConstruct()->isCompilerGenerated() == true)
  // if ( (initializedName->get_startOfConstruct()->isCompilerGenerated() == true) && (initializedName->get_name() == "__PRETTY_FUNCTION__" || initializedName->get_name() == "__func__") )
     if ( (initializedName->get_startOfConstruct()->isCompilerGenerated() == true) && 
          ( (initializedName->get_name() == "__PRETTY_FUNCTION__") || 
            (initializedName->get_name() == "__func__")            || 
            (initializedName->get_name() == "__FUNCTION__")        || 
            (initializedName->get_name() == "__BASE_FILE__") ) )
        {
#if 0
          printf ("initializedName = %p = %s is compiler generated so it should not be qualified \n",initializedName,initializedName->get_name().str());
          initializedName->get_startOfConstruct()->display("compiler generated so it should not be qualified");
#endif
          outputQualifiedName = false;
        }

  // DQ (6/3/2007): In the scope where the variable is declared name qualification is never required, so don't
  // test that scope.  Look at where the variable declaration is used instead (get from SgUnparse_Info object).
  // SgScopeStatement* scopeStatement = initializedName->get_scope();
     SgScopeStatement* scopeStatement = info.get_current_scope();

       // If this is NULL then assume that this is being called from the unparseToString function 
       // (where it is difficult to establish the scope for SgType an SgSymbol IR node).
     if (scopeStatement == NULL)
        {
#if PRINT_DEVELOPER_WARNINGS
       // printf ("initializedName->get_scope() is NULL, likely part of a transformation, OK (skip generation of qualified name) \n");
          printf ("info.get_current_scope() is NULL, likely part of a transformation, OK (skip generation of qualified name) \n");
#endif
          outputQualifiedName = false;
        }
  // ROSE_ASSERT(scopeStatement != NULL);

  // DQ (1/7/2007): This make sure that we don't use name qualifification for "C" language code.
     if (outputQualifiedName == true)
        {
       // In general the whole name qualification mechanism should be recursive to generate minimally qualified names.

       // DQ (9/8/2007): This will be checked in isAnOuterScope(), so let's catch such errors as early as possible.
          ROSE_ASSERT(initializedName->get_scope() != NULL);

       // DQ (7/4/2007): We sometimes require name qualification independent of the hidden list mechanism. For example
       // when qualification is required to resolve a name sepearately from when we are only trying to disambiguate between names.
          bool requiredNameIsVisible = unp->u_name->isAnOuterScope(scopeStatement,initializedName->get_scope());
#if TRACE_NAME_QUALIFICATION_FOR_INITIALIZED_NAME
          printf ("initializedName requiredNameIsVisible = %s \n",(requiredNameIsVisible == true) ? "true" : "false");
#endif
          if (requiredNameIsVisible == true)
             {
            // This case is to disambiguate between names, and if they are hidden by other names we have to use name qualification to do this.

#if 0
               outputHiddenListData (scopeStatement);
#endif

            // Make this a reference later
               set<SgSymbol*> hidden_declaration_list = scopeStatement->get_hidden_declaration_list();

            // Get the symbol associated with a specific declaration
               SgSymbol* associatedSymbol = initializedName->get_symbol_from_symbol_table();

#if TRACE_NAME_QUALIFICATION_FOR_INITIALIZED_NAME
               printf ("In generateNameQualifier(): initializedName = %p initializedName->get_scope() = %p = %s \n",
                    initializedName,initializedName->get_scope(),initializedName->get_scope()->class_name().c_str());
#endif
            // initializedName->get_scope()->get_symbol_table()->find(initializedName);

            // DQ (6/10/2007): static data members can have multiple SgInitializedName objects 
            // and so we need to figure out which one was used to build the symbol.
               if (associatedSymbol == NULL)
                  {
                    SgInitializedName* alt_initializedName = initializedName->get_prev_decl_item();
                    if (alt_initializedName == NULL)
                       {
                         printf ("Error: initializedName->get_symbol_from_symbol_table() returned NULL, no symbol for variable found in the variables explicitly stored scope -- name: %s \n",initializedName->get_name().str());
                         initializedName->get_startOfConstruct()->display("Error: initializedName->get_symbol_from_symbol_table() returned NULL, no symbol for variable found in the variables explicitly stored scope");
                       }
                    ROSE_ASSERT(alt_initializedName != NULL);

#if TRACE_NAME_QUALIFICATION_FOR_INITIALIZED_NAME
                    printf ("Could not find associatedSymbol using initializedName = %p = %s using alt_initializedName = %p = %s \n",
                         initializedName,initializedName->get_name().str(),alt_initializedName,alt_initializedName->get_name().str());
                    printf ("Warning: There maybe should be an associated symbol for the specified initializedName \n");
#endif
                    associatedSymbol = alt_initializedName->get_symbol_from_symbol_table();
                    ROSE_ASSERT(associatedSymbol != NULL);
                  }

#if TRACE_NAME_QUALIFICATION_FOR_INITIALIZED_NAME
               if (associatedSymbol == NULL)
                  {
                    printf ("Could not find associatedSymbol using initializedName = %p = %s \n",
                         initializedName,initializedName->get_name().str());
                    printf ("scopeStatement = %p = %s \n",scopeStatement,scopeStatement->class_name().c_str());
                    printf ("initializedName->get_scope()->get_symbol_table() = %p \n",initializedName->get_scope()->get_symbol_table());
                    initializedName->get_scope()->get_symbol_table()->print("initializedName scope symbol table",V_SgVariableSymbol);
                    initializedName->get_startOfConstruct()->display("initializedName");
                  }
#endif
               ROSE_ASSERT(associatedSymbol != NULL);

               SgScopeStatement* initializedNameScope = initializedName->get_scope();
               ROSE_ASSERT(initializedNameScope != NULL);

            // Handle the case of a hidden declaration (function, variable, namespace, etc.)
               set<SgSymbol*>::iterator symbolIterator = hidden_declaration_list.find(associatedSymbol);
               if (symbolIterator != hidden_declaration_list.end())
                  {
                 // If the associatedSymbolOfHiddenDeclaration exists then the associated declaration is hidden and any reference to it should be qualified.
                 // SgSymbol* associatedSymbolOfHiddenDeclaration = *declarationIterator;

                 // printf ("initializedName = %p = %s \n",initializedName,SageInterface::get_name(initializedName).c_str());

                    SgName name;
                    SgDeclarationStatement* declaration = NULL;
                 // printf ("Calling generateDeclarationAndName using initializedNameScope = %p = %s \n",initializedNameScope,initializedNameScope->class_name().c_str());
                    generateDeclarationAndName (initializedNameScope,name,declaration);
                 // printf ("Output of generateDeclarationAndName(): declaration = %p \n",declaration);

                    SgName baseName;
                    if (declaration != NULL)
                         baseName = generateNameQualifier(declaration,info,/* nestedQualifiaction */ true);

                    if (isSgGlobal(initializedNameScope) != NULL)
                         name = "::";

                 // Use the context information provide in the SgUnparse_Info object.
                 // printf ("info.get_current_scope() = %p \n",info.get_current_scope());
                    ROSE_ASSERT(info.get_current_scope() != NULL);
                    if (initializedNameScope == info.get_current_scope())
                       {
                      // printf ("This is part of the unparsing of a definition defined in the current scope remove the qualification if present \n");
                         name = "";
                       }

                 // printf ("After initializedName in current scope test: baseName = %s name = %s \n",baseName.str(),name.str());

                    qualifier = SgName::assembleQualifiedName(baseName,name);
                  }
             }
            else
             {
            // This case is where a name is not visible unless we use name qualification. In this case we will not
            // require global qualification unless the top level name requires it.  We will have to make this
            // recursive to handle this case.

               SgName name;
               SgDeclarationStatement* declaration = NULL;
               SgScopeStatement* initailizedNameScope = initializedName->get_scope();
               generateDeclarationAndName (initailizedNameScope,name,declaration);

            // This could be a function definition as in test2005_06.C.
            // ROSE_ASSERT(declaration != NULL);

            // Also this can not be global scope, else it would have been visible!
               ROSE_ASSERT(isSgGlobal(declaration) == NULL);

            // This permits a recursive evaluation of qualified names back to the global scope
            // This global qualification
            // SgName baseName = generateNameQualifier(declaration,info,/* nestedQualifiaction */ true);
               SgName baseName;
               if (declaration != NULL)
                  {
                 // If this was a function definition as in test2005_06.C then generateDeclarationAndName() will return declaration == NULL.
                    baseName = generateNameQualifier(declaration,info,/* nestedQualifiaction */ true);
                  }
#if 0
            // If this is a non-empty base name then generate the glue to use it with the declaration name
               if (baseName.is_null() == false)
                  {
                    baseName += "::";
                  }
#endif
#if TRACE_NAME_QUALIFICATION_FOR_INITIALIZED_NAME
               printf ("baseName = %s name = %s \n",baseName.str(),name.str());
#endif
               qualifier = SgName::assembleQualifiedName(baseName,name);
             }

          string qualifierString = qualifier.str();
          string::size_type qualifierStringLength = qualifierString.size();
       // printf ("qualifierString = %s qualifierStringLength = %ld \n",qualifierString.c_str(),qualifierStringLength);

       // Only add a trailing "::" if this is not a nested call to generateNameQualifier
       // if (nestedQualification == false && qualifier.is_null() == false && qualifier != "::")
          if (nestedQualification == false && qualifier.is_null() == false && ( (qualifierStringLength == 1) || ( (qualifierStringLength >= 2) && (qualifierString.rfind("::") != qualifierStringLength-2) ) ) )
             {
#if TRACE_NAME_QUALIFICATION_FOR_INITIALIZED_NAME
               printf ("In generateNameQualifier( SgInitializedName*,,): Adding :: to qualifier = %s \n",qualifier.str());
#endif
               qualifier += "::";
             }

        } // end of block for outputQualifiedName == true

#if TRACE_NAME_QUALIFICATION_FOR_INITIALIZED_NAME
     printf ("Leaving generateNameQualifier( SgInitializedName*,,): qualifier = %s \n",(qualifier.is_null() == false) ? qualifier.str() : "NULL");
#endif

     return qualifier;
   }
#endif


// DQ (5/11/2011): New name qualification for ROSE (the 4th try).
// This is a part of a rewrite of the name qualification support in ROSE with the follwoing properties:
//    1) It is exact (no over qualification).
//    2) It handled visibility of names constructs
//    3) It resolves ambiguity of named constructs.
//    4) It resolves where type elaboration is required.
//    5) The inputs are carried in the SgUnparse_Info object for uniform handling.
//    6) The the values in the SgUnparse_Info object are copied from the AST references to the named 
//       constructs to avoid where named constructs are referenced from multiple locations and the 
//       name qulification might be different.
//
//    7) What about base class qualification? I might have forgotten this one! No, this work, but might not generate the minimum length qualified name.

SgName
Unparser_Nameq::generateNameQualifier( SgInitializedName* initializedName, const SgUnparse_Info& info )
   {
  // This support for name qualification is C++ specific, this might be a problem that should generate a refactoring of this code.

     ROSE_ASSERT(initializedName != NULL);
     return generateNameQualifierSupport(initializedName->get_scope(),info);
   }

SgName
Unparser_Nameq::generateNameQualifier( SgDeclarationStatement* declarationStatement, const SgUnparse_Info & info )
   {
  // This support for name qualification is C++ specific, this might be a problem that should generate a refactoring of this code.
#if 1
     printf ("In Unparser_Nameq::generateNameQualifier(): info.get_name_qualification_length()     = %d \n",info.get_name_qualification_length());
     printf ("In Unparser_Nameq::generateNameQualifier(): info.get_type_elaboration_required()     = %s \n",info.get_type_elaboration_required() ? "true" : "false");
     printf ("In Unparser_Nameq::generateNameQualifier(): info.get_global_qualification_required() = %s \n",info.get_global_qualification_required() ? "true" : "false");
#endif

     ROSE_ASSERT(declarationStatement != NULL);
     return generateNameQualifierSupport(declarationStatement->get_scope(),info);
   }


SgName
Unparser_Nameq::generateNameQualifierSupport( SgScopeStatement* scope, const SgUnparse_Info& info )
   {
  // This support for name qualification is C++ specific, this might be a problem that should generate a refactoring of this code.

     printf ("In Unparser_Nameq::generateNameQualifierSupport(): info.get_name_qualification_length()     = %d \n",info.get_name_qualification_length());
     printf ("In Unparser_Nameq::generateNameQualifierSupport(): info.get_type_elaboration_required()     = %s \n",info.get_type_elaboration_required() ? "true" : "false");
     printf ("In Unparser_Nameq::generateNameQualifierSupport(): info.get_global_qualification_required() = %s \n",info.get_global_qualification_required() ? "true" : "false");

     SgName qualifiedName;

     SgName name;
     for (int i = 0; i < info.get_name_qualification_length(); i++)
        {
       // Get the name of each named scope from the associated declaration and put it out.
          printf ("In generateNameQualifierSupport(): i = %d scope = %p = %s qualifiedName = %s \n",i,scope,scope->class_name().c_str(),qualifiedName.str());

          if (scope->isNamedScope() == false)
             {
            // Iterate to the next named scope
            // while (scope->isNamedScope() == false && isSgGlobal(scope) != NULL)
               while (scope->isNamedScope() == false && isSgGlobal(scope) == NULL)
                  {
                    scope = scope->get_scope();
                    ROSE_ASSERT(scope != NULL);

                    if (isSgGlobal(scope) != NULL)
                       {
                         printf ("Found the global scope! \n");
                       }
                  }

               ROSE_ASSERT(scope != NULL);
             }

       // Generate the name of this named scope.
          name = scope->associatedScopeName();

          printf ("In generateNameQualifierSupport(): scope = %p = %s name = %s \n",scope,scope->class_name().c_str(),name.str());

       // Sometimes the name can be empty (e.g. un-named namespace).
          if (name.is_null() == false)
             {
            // qualifiedName += name + "::";
               qualifiedName = name + "::" + qualifiedName;
             }
            else
             {
               printf ("ERROR: empty name for named scope = %s \n",scope->class_name().c_str());
#if 0
               ROSE_ASSERT(false);
#endif
             }

          scope = scope->get_scope();
          ROSE_ASSERT(scope != NULL);


        }

  // Add global qualification if it is specified.
     if (info.get_global_qualification_required() == true)
        {
       // qualifiedName += "::";
          qualifiedName = "::" + qualifiedName;
        }

     return qualifiedName;
   }
