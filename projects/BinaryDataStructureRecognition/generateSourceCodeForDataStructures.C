#include "rose.h"

#include "astStructRecognition.h"

using namespace SageBuilder;
using namespace SageInterface;
using namespace std;

void
buildSourceCode( SgProject* project )
   {
  // Generate a new file to hold the C++ classes that are equivalent to those 
  // defined/used within the binary. An important point will be to define our 
  // concept of equivalence.

#if 0
  // Absolute filename should also work, a relative location will be with respect 
  // to the current working directory (the build tree when used in ROSE).
  // string fileName="/home/dquinlan/ROSE/ROSE_CompileTree/svn-LINUX-64bit-4.2.4/developersScratchSpace/Dan/binaryDataStructureRecognition_tests/rose_class_definition.C";
  // string fileName="rose_class_definition.C";
     string fileName = generateProjectName(project,/* supressSuffix = */ false);
     printf ("Generate source file: fileName = %s \n",fileName.c_str());

  // Remove any previous existing file (from previous run).
  // This file should already be removed.
     int status = remove (fileName.c_str());
     ROSE_ASSERT (status == 0);
#endif

#if 0
  // Generate the SgSourceFile using the existing SgProject where it does not currently exist.
     SgSourceFile *file = isSgSourceFile( buildFile(fileName,fileName,project) );
#else
  // Use the file that was already generated in the global variable: GlobalScopeAnalysisAttribute
     ROSE_ASSERT(globalScopeAttribute != NULL);
     SgSourceFile *file = globalScopeAttribute->generatedSourceFile;
     ROSE_ASSERT(file != NULL);
#endif

#if 0
  // Example code that adds something into the new file (as a test)
     SgGlobal* global = file->get_globalScope();
     SgVariableDeclaration *varDecl = buildVariableDeclaration("example_global_variable_0", buildIntType());
     appendStatement (varDecl,isSgScopeStatement(global));
#endif

     printf ("Generating code for classes \n");
     generateClassStructures(file);

     printf ("Generating code for global variables \n");
     generateGlobalVariables(file);

     printf ("Generating code for functions \n");
     generateFunctions(file);

  // Test the AST
     AstTests::runAllTests(project);
   }


#if 0
void
GenerateClassStructuresTraversal::visit ( SgNode* n )
   {
   }
#endif

void
generateClassStructures ( SgSourceFile* file )
   {
  // Traverse the AST and build classes for each detected C++ class.
     ROSE_ASSERT(file != NULL);

     SgAsmElfSection* virtualTableSection = getVirtualTableSection(file->get_project());
     ROSE_ASSERT(virtualTableSection != NULL);

     VirtualTableSection* virtualTableSectionAttribute = dynamic_cast<VirtualTableSection*>(virtualTableSection->getAttribute("VirtualFunctionTableSectionAttribute"));
     ROSE_ASSERT(virtualTableSectionAttribute != NULL);

     printf ("Build the C++ classes associated with the virtual function tables...\n");

  // Get the global scope
     SgGlobal* globalScope = file->get_globalScope();

  // This is the list of virtual function tables, we know that there is a one-to-one 
  // association of vTables with C++ classes that have at least one virtual function.
     vector<VirtualFunctionTable*> & virtualFunctionTableList = virtualTableSectionAttribute->virtualFunctionTableList;
     for (size_t i = 0; i < virtualFunctionTableList.size(); i++)
        {
       // This is a C++ class with at least one virtual member function.

          printf ("Virtual function table: i = %zu name = %s \n",i,virtualFunctionTableList[i]->name.c_str());

          SgClassDeclaration* decl = buildVirtualClass(globalScope,virtualFunctionTableList[i]);

          std::vector<VirtualFunction*> virtualFunctionList = virtualFunctionTableList[i]->virtualFunctionList;
          for (size_t j = 0; j < virtualFunctionList.size(); j++)
             {
            // This is a virtual function

               printf ("Virtual function: j = %zu \n",j);

               printf ("virtual function name = %s \n",virtualFunctionList[j]->name.c_str());

               buildVirtualFunction(decl,virtualFunctionList[j]);
             }
        }

   }



SgClassDeclaration *
buildVirtualClass ( SgGlobal* globalScope, VirtualFunctionTable* vTable )
   {
     pushScopeStack (isSgScopeStatement (globalScope));
     ROSE_ASSERT (topScopeStack());

  // build a struct declaration with implicit scope information from the scope stack
#if 0
     SgClassDeclaration * decl = buildStructDeclaration(vTable->name);
#else
  // For now add the globalScope, but then make the interface the same as for buildStructDeclaration()
     SgClassDeclaration * decl = buildClassDeclaration(vTable->name,globalScope);
#endif

  // build member variables inside the structure
     SgClassDefinition *def = decl->get_definition();
     pushScopeStack (isSgScopeStatement (def));
     ROSE_ASSERT (topScopeStack());

     SgVariableDeclaration *varDecl = buildVariableDeclaration(SgName ("example_data_member_0"), buildIntType());

  // Insert the  member variable
     appendStatement (varDecl);

#if 0
  // build a member function prototype of the construct
     SgInitializedName* arg1 = buildInitializedName(SgName("x"), buildIntType());
     SgFunctionParameterList * paraList = buildFunctionParameterList();
     appendArg(paraList,arg1);

     SgMemberFunctionDeclaration * funcdecl = buildNondefiningMemberFunctionDeclaration("bar",buildVoidType(), paraList);
     appendStatement(funcdecl);

  // build a defining member function 
     SgFunctionParameterList * paraList2 = isSgFunctionParameterList(deepCopy(paraList)); 
     ROSE_ASSERT(paraList2);
     SgMemberFunctionDeclaration* funcdecl_2 = buildDefiningMemberFunctionDeclaration("bar2",buildVoidType(),paraList2);
     appendStatement(funcdecl_2);                         
#endif

     popScopeStack ();
  // insert the struct declaration
     appendStatement (decl);

  // pop the final scope after all AST insertion
     popScopeStack ();

     return decl;
   }

void
generateGlobalVariables ( SgSourceFile* file )
   {
  // Traverse the AST and build classes for each detected C++ class.
     ROSE_ASSERT(file != NULL);

  // Get the global scope
     SgGlobal* globalScope = file->get_globalScope();

  // Verify that the global variable is initialized properly.
     ROSE_ASSERT(globalScopeAttribute != NULL);

  // Add this variable's type to the list of variable types with addresses on the stack.
     vector<TypeAnalysisAttribute*> & stackTypeList = globalScopeAttribute->stackTypeList;

     printf ("Build the C++ global variables associated with the binary...size = %zu \n",stackTypeList.size());

     pushScopeStack (globalScope);

     for (size_t i = 0; i < stackTypeList.size(); i++)
        {
       // This is a C++ class with at least one virtual member function.
          ROSE_ASSERT(stackTypeList[i]->asmType != NULL);
          printf ("   Global variable: i = %zu address = %p type = %s \n",i,(void*)(stackTypeList[i]->address),stackTypeList[i]->asmType->class_name().c_str());
       // printf ("   Global variable: i = %zu address = %p \n",i,(void*)(stackTypeList[i]->address));
       // printf ("   Global variable: type = %s \n",stackTypeList[i]->asmType->class_name().c_str());

#if 1
       // SgVariableDeclaration* buildVariableDeclaration(const std::string & name, SgType *type, SgInitializer *varInit=NULL, SgScopeStatement* scope=NULL);
          string name             = stackTypeList[i]->name;
       // SgType* type            = getTypeMapping(stackTypeList[i]->asmType);
       // string name             = "var_1";
          SgType* type            = SgTypeInt::createType();
          SgInitializer *varInit  = NULL;

       // Only insert variables not already present
          if (globalScope->lookup_variable_symbol(name) == NULL)
             {
               SgVariableDeclaration* vdecl = buildVariableDeclaration(name,type,varInit,globalScope);
               appendStatement(vdecl);
             }
#endif
        }

  // pop the final scope after all AST insertion
     popScopeStack ();
   }


void
generateStackVariables ( SgFunctionDeclaration* functionDeclaration, FunctionAnalysisAttribute* functionAnalysisAttribute )
   {
     ROSE_ASSERT(functionAnalysisAttribute != NULL);
     ROSE_ASSERT(functionDeclaration != NULL);

  // Add this variable's type to the list of variable types with addresses on the stack.
     vector<TypeAnalysisAttribute*> & stackTypeList = functionAnalysisAttribute->stackTypeList;

     printf ("Build the C++ stack variables associated with each functions...size = %zu \n",stackTypeList.size());

     SgFunctionDefinition* functionScope = functionDeclaration->get_definition();
     ROSE_ASSERT(functionScope != NULL);

     SgScopeStatement* functionBodyScope = isSgScopeStatement(functionScope->get_body());
     ROSE_ASSERT(functionBodyScope != NULL);

     pushScopeStack(functionBodyScope);

     for (size_t i = 0; i < stackTypeList.size(); i++)
        {
       // This is a C++ class with at least one virtual member function.
          ROSE_ASSERT(stackTypeList[i]->asmType != NULL);
          printf ("   Stack variable: i = %zu address = %p type = %s \n",i,(void*)(stackTypeList[i]->address),stackTypeList[i]->asmType->class_name().c_str());

#if 1
       // SgVariableDeclaration* buildVariableDeclaration(const std::string & name, SgType *type, SgInitializer *varInit=NULL, SgScopeStatement* scope=NULL);
          string name             = stackTypeList[i]->name;
       // SgType* type            = getTypeMapping(stackTypeList[i]->asmType);
       // string name             = "var_1";
          SgType* type            = SgTypeInt::createType();
          SgInitializer *varInit  = NULL;

       // Only insert variables not already present
          if (functionScope->lookup_variable_symbol(name) == NULL)
             {
               SgVariableDeclaration* vdecl = buildVariableDeclaration(name,type,varInit,functionScope);
               appendStatement(vdecl);
             }
#endif
        }

  // pop the final scope after all AST insertion
     popScopeStack ();
   }

SgMemberFunctionDeclaration*
buildVirtualFunction ( SgClassDeclaration * decl, VirtualFunction* vFunction )
   {
     SgClassDefinition *def = decl->get_definition();
     ROSE_ASSERT(def != NULL);

  // build member variables inside the structure
     pushScopeStack (isSgScopeStatement (def));
     ROSE_ASSERT (topScopeStack());

#if 0
     SgVariableDeclaration *varDecl = buildVariableDeclaration(SgName ("example_data_member_1"), buildIntType());

  // Insert the  member variable
     appendStatement (varDecl);
#endif

  // build a member function prototype of the construct
     SgInitializedName* arg1 = buildInitializedName(SgName("x"), buildIntType());
     SgFunctionParameterList * paraList = buildFunctionParameterList();
     appendArg(paraList,arg1);

     string name = vFunction->name;
     SgMemberFunctionDeclaration * funcdecl = buildNondefiningMemberFunctionDeclaration(name,buildVoidType(), paraList);
     appendStatement(funcdecl);

#if 0
  // build a defining member function 
     SgFunctionParameterList * paraList2 = isSgFunctionParameterList(deepCopy(paraList)); 
     ROSE_ASSERT(paraList2);
     SgMemberFunctionDeclaration* funcdecl_2 = buildDefiningMemberFunctionDeclaration("virtual_function_bar",buildVoidType(),paraList2);
     appendStatement(funcdecl_2);                         
#endif

  // pop the final scope after all AST insertion
     popScopeStack ();

     return funcdecl;
   }


void
generateFunctions ( SgSourceFile* file )
   {
  // In order to get get stack references to data structures correct we generate
  // a source code representation of each function in the binary so that we can
  // add data structures as required.  At the moment all member functions are
  // generated as functions.

  // Traverse the AST and build functions for each detected function in the binary.
     ROSE_ASSERT(file != NULL);

     SgProject* project = file->get_project();
     ROSE_ASSERT(project != NULL);

     SgGlobal* globalScope = file->get_globalScope();
     ROSE_ASSERT(globalScope != NULL);

     struct Visitor: public AstSimpleProcessing
        {
       // Save the project so that we can use it in the visit function
          SgGlobal* globalScope;

          Visitor( SgGlobal* g ) : globalScope(g) {}

          virtual void visit(SgNode* astNode)
             {
            // DQ (8/30/2013): This type was renamed and so the original function is not available (changed).
            // SgAsmFunctionDeclaration* asmFunction = isSgAsmFunctionDeclaration(astNode);
               SgAsmFunctionDeclaration* asmFunction = isSgAsmFunction(astNode);
               if (asmFunction != NULL)
                  {
                    ROSE_ASSERT(globalScope != NULL);

                 // Add it to the AST (so it can be found later in another pass over the AST)
                    FunctionAnalysisAttribute* functionAnalysisAttribute = dynamic_cast<FunctionAnalysisAttribute*>(asmFunction->getAttribute("FunctionAnalysisAttribute"));
                    ROSE_ASSERT(functionAnalysisAttribute != NULL);

                    SgFunctionDeclaration* functionDeclaration = buildFunction(globalScope,functionAnalysisAttribute);

                 // Add the referenced stack variables.
                    generateStackVariables(functionDeclaration,functionAnalysisAttribute);
                  }
             }
        };

      Visitor v(globalScope);

      v.traverse(project, preorder);
   }

string 
filter_name_to_form_valid_identifier ( string name )
   {
     string s = name;

  // Note that this could lead to ambiguity
     replace(s.begin(),s.end(),'.','_');

  // DQ (3/27/2010): Make sure we don't generate function names that violate the C/C++ language standard.
  // Handle names like: __cxa_atexit@plt, __gmon_start__@plt, _ZdlPv@plt, etc.
     replace(s.begin(),s.end(),'@','_');

  // DQ (3/27/2010): Make sure we don't generate function names that violate the C/C++ language standard.
  // Handle names like: ***unassigned blocks***
     replace(s.begin(),s.end(),'*','_');
     replace(s.begin(),s.end(),' ','_');

  // printf ("Convert name = %s to s = %s \n",name.c_str(),s.c_str());

     return s;
   }


// void
SgFunctionDeclaration*
buildFunction ( SgGlobal* globalScope, FunctionAnalysisAttribute* function )
   {
     ROSE_ASSERT(globalScope != NULL);
     ROSE_ASSERT(function != NULL);

     printf ("adding function = %s \n",function->name.c_str());

     string name = filter_name_to_form_valid_identifier(function->name);

  // DQ (3/27/2010): Make sure we don't generate function names that violate the C/C++ language standard.
     ROSE_ASSERT(name.find('@') == string::npos);

     pushScopeStack (isSgScopeStatement (globalScope));

  // defining  int foo(int x, float)
  // build parameter list first
     SgInitializedName* arg1 = buildInitializedName(SgName("example_parameter_0"),buildIntType());
     SgInitializedName* arg2 = buildInitializedName(SgName("example_parameter_1"),buildFloatType());
     SgFunctionParameterList * paraList = buildFunctionParameterList();
     appendArg(paraList, arg1);
     appendArg(paraList, arg2);

  // build defining function declaration
     SgType* functionReturnType   = (name == "main") ? dynamic_cast<SgType*>(buildIntType()) : dynamic_cast<SgType*>(buildVoidType());
     SgFunctionDeclaration* func1 = buildDefiningFunctionDeclaration (name,functionReturnType,paraList);

  // build a statement inside the function body
     SgBasicBlock *func_body = func1->get_definition ()->get_body ();
     ROSE_ASSERT (func_body);
     pushScopeStack (isSgScopeStatement (func_body));

  // If there are any associated comments for the function in the Binary AST, then add them to the generated source code
     ROSE_ASSERT(function->function != NULL);
     if (function->function->get_comment().empty() == false)
        {
          string comment = function->function->get_comment();
       // SageInterface::attachComment(func1,comment);

       // Put the comments inside the function bodies.
          SageInterface::attachComment(func_body,comment,PreprocessingInfo::inside);
        }

#if 0
     SgVariableDeclaration *varDecl = buildVariableDeclaration(SgName ("example_variable_declaration_0"), buildIntType());
  // Insert the statement
     appendStatement (varDecl);
#endif

     popScopeStack ();
  // insert the defining function
     appendStatement (func1);

  // pop the final scope after all AST insertion
     popScopeStack ();

     return func1;
   }

