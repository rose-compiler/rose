// Demonstration of instrumentation using the TAU performance monitoring tools (University of Oregon)

#include "rose.h"

using namespace std;

#define NEW_FILE_INFO Sg_File_Info::generateDefaultFileInfoForTransformationNode()

SgClassDeclaration*
getProfilerClassDeclaration (SgProject* project)
   {
  // Note that it would be more elegant to look this up in the Symbol table (do this next)

     SgClassDeclaration* returnClassDeclaration = NULL;
     Rose_STL_Container<SgNode*> classDeclarationList = NodeQuery::querySubTree (project,V_SgClassDeclaration);
     for (Rose_STL_Container<SgNode*>::iterator i = classDeclarationList.begin(); i != classDeclarationList.end(); i++)
        {
       // Need to cast *i from SgNode to at least a SgStatement
          SgClassDeclaration* classDeclaration = isSgClassDeclaration(*i);
          ROSE_ASSERT (classDeclaration != NULL);

       // printf ("In getProfilerClassDeclaration(): classDeclaration->get_name() = %s \n",classDeclaration->get_name().str());

          if (classDeclaration->get_name() == "Profiler")
               returnClassDeclaration = classDeclaration;
        }

     ROSE_ASSERT(returnClassDeclaration != NULL);
     return returnClassDeclaration;
   }


int
main( int argc, char * argv[] )
   {
  // This test code tests the AST rewrite mechanism to add TAU Instrumention to the AST.

  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

     SgProject* project = frontend(argc,argv);

  // Output the source code file (as represented by the SAGE AST) as a PDF file (with bookmarks)
  // generatePDF(*project);

  // Output the source code file (as represented by the SAGE AST) as a DOT file (graph)
  // generateDOT(*project);

  // Allow ROSE translator options to influence if we transform the AST
     if ( project->get_skip_transformation() == false )
        {
       // NOTE: There can be multiple files on the command line and each file has a global scope

          MiddleLevelRewrite::ScopeIdentifierEnum   scope           = MidLevelCollectionTypedefs::StatementScope;
          MiddleLevelRewrite::PlacementPositionEnum locationInScope = MidLevelCollectionTypedefs::TopOfCurrentScope;

       // Add a TAU include directive to the top of the global scope
          Rose_STL_Container<SgNode*> globalScopeList = NodeQuery::querySubTree (project,V_SgGlobal);
          for (Rose_STL_Container<SgNode*>::iterator i = globalScopeList.begin(); i != globalScopeList.end(); i++)
             {
            // Need to cast *i from SgNode to at least a SgStatement
               SgGlobal* globalScope = isSgGlobal(*i);
               ROSE_ASSERT (globalScope != NULL);

            // TAU does not seem to compile using EDG or g++ (need to sort this out with Brian)
            // MiddleLevelRewrite::insert(globalScope,"#define PROFILING_ON \n#include<TAU.h> \n",scope,locationInScope);
            // MiddleLevelRewrite::insert(globalScope,"#include<TAU.h> \n",scope,locationInScope);
               MiddleLevelRewrite::insert(globalScope,"#define PROFILING_ON 1 \n#define TAU_STDCXXLIB 1 \n#include<TAU.h> \n",scope,locationInScope);
             }
#if 1
       // Now get the class declaration representing the TAU type with which to build variable declarations
          SgClassDeclaration* tauClassDeclaration = getProfilerClassDeclaration(project);
          ROSE_ASSERT(tauClassDeclaration != NULL);
          SgClassType* tauType = tauClassDeclaration->get_type();
          ROSE_ASSERT(tauType != NULL);

       // Get a constructor to use with the variable declaration (anyone will due for code generation)
          SgMemberFunctionDeclaration* memberFunctionDeclaration = SageInterface::getDefaultConstructor(tauClassDeclaration);
          ROSE_ASSERT(memberFunctionDeclaration != NULL);

       // Add the instrumentation to each function
          Rose_STL_Container<SgNode*> functionDeclarationList = NodeQuery::querySubTree (project,V_SgFunctionDeclaration);
          for (Rose_STL_Container<SgNode*>::iterator i = functionDeclarationList.begin(); i != functionDeclarationList.end(); i++)
             {
               SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(*i);
               ROSE_ASSERT(functionDeclaration != NULL);
               SgFunctionDeclaration* definingFunctionDeclaration = isSgFunctionDeclaration(functionDeclaration->get_definingDeclaration());

            // The defining declaration is unique and we only want to process the function body once
               if (definingFunctionDeclaration == functionDeclaration)
                  {
                    ROSE_ASSERT(functionDeclaration->get_definition() != NULL);
                    SgBasicBlock* functionBody = functionDeclaration->get_definition()->get_body();
                    ROSE_ASSERT(functionBody != NULL);
#if 0
                 // Use AST string-based rewrite mechanism (not ready for production use)
                    string tauInstumentationString = "TauTimer tauTimerInstrumentation;\n";
                    MiddleLevelRewrite::insert(functionBody,tauInstumentationString,scope,locationInScope);
#else
                 // Build the function declaration as a lower level (directly within the AST)
                    if (functionBody->get_statements().size() > 0)
                       {
                      // Build this to be an integer type until we can figure out how TAU works
                      // SgType* type = new SgTypeInt();
                         SgType* type = tauType;
                         ROSE_ASSERT(type != NULL);

                         SgName name = "tauTimerInstrumentation";

                         SgVariableDeclaration* variableDeclaration = new SgVariableDeclaration(NEW_FILE_INFO,name,type);
                         ROSE_ASSERT(variableDeclaration != NULL);

                         ROSE_ASSERT(variableDeclaration->get_variables().size() == 1);
                         SgInitializedName* initializedName = (*variableDeclaration->get_variables().begin());

                      // Get the 1st parameter for the Tau constructor call
                         SgExprListExp* expressionList = new SgExprListExp(NEW_FILE_INFO);
                         SgStringVal* functionNameStringValue = new SgStringVal(NEW_FILE_INFO,functionDeclaration->get_name().getString());
                         expressionList->append_expression(functionNameStringValue);

                      // Get the 2nd parameter for the Tau constructor call
                         ROSE_ASSERT(functionDeclaration->get_orig_return_type() != NULL);

                      // printf ("Processing functionDeclaration = %p = %s = %s \n",functionDeclaration,
                      //      functionDeclaration->class_name().c_str(),SageInterface::get_name(functionDeclaration).c_str());
                      // functionDeclaration->get_startOfConstruct()->display("Processing functionDeclaration tauTimerInstrumentation: debug");

                      // DQ (9/9/2007): The MiddleLevelRewrite::insert() has to call the AST merge to add in the symbols and 
                      // declarations required for the new function to be properly defined.  Until this happends there will be
                      // some types (e.g. those in "std") which the unparser will not be able to resolve using the new name 
                      // qualification mechanism (which make heavy use of symbols that would not be found until the AST merge is 
                      // used in the MiddleLevelRewrite::insert(). When we have a less precise name qualifiacation this worked!
                      // string typeNameString = functionDeclaration->get_orig_return_type()->unparseToString();
                         string typeNameString = "function return type unavailable (type unparsing mechanism requires AST merge use in MiddleLevelRewrite::insert())";

                         SgStringVal* functionTypeStringValue = new SgStringVal(NEW_FILE_INFO,typeNameString);
                         expressionList->append_expression(functionTypeStringValue);

                         SgConstructorInitializer* constructorInitializer = 
                           // new SgConstructorInitializer(NEW_FILE_INFO,memberFunctionDeclaration,expressionList,NULL,true,true,true,true);
                              new SgConstructorInitializer(NEW_FILE_INFO,memberFunctionDeclaration,expressionList,tauClassDeclaration->get_type(),false,false,true,false);
                         initializedName->set_initptr(constructorInitializer);

                      // Note that insert is not defined for empty body, since the iterator is not defined
                         ROSE_ASSERT(functionBody->get_statements().empty() == false);
                         functionBody->get_statements().insert(functionBody->get_statements().begin(),variableDeclaration);
                         variableDeclaration->set_parent(functionBody);

                         initializedName->set_scope(functionBody);
                         SgVariableSymbol* variableSymbol = new SgVariableSymbol(initializedName);
                         functionBody->insert_symbol(initializedName->get_name(),variableSymbol);
                         ROSE_ASSERT(functionBody->lookup_variable_symbol(initializedName->get_name()) != NULL);
#endif
                       }
                  }
             }
#endif
        }
       else
        {
          printf ("project->get_skip_transformation() == true \n");
        }

#if 0
  // Call the ROSE backend (unparse to generate transformed 
  // source code and compile it with vendor compiler).
     return backend(project);
#else
     project->unparse();
#endif
   }

