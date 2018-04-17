// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// rose.C: Example (default) ROSE Preprocessor: used for testing ROSE infrastructure

#include "rose.h"

// DQ (1/1/2006): This is OK if not declared in a header file
using namespace std;

class MyTraversal
   : public SgSimpleProcessing
   {
     public:
          string targetString;
          string newSourceString;
          MiddleLevelRewrite::PlacementPositionEnum locationInScope;

         ~MyTraversal () {};
          MyTraversal () {};

          void setTargetString ( string s ) 
             {
               targetString = s;
             }

          void setLocationInScope ( MiddleLevelRewrite::PlacementPositionEnum location ) 
             {
               locationInScope = location;
             }

          void setSourceString ( string s, bool beforeStatement ) 
             {
               newSourceString = s;
#if 0
            // Possible values for locationInScope
               PreamblePositionInScope
               TopOfCurrentScope
               BeforeCurrentPosition
               ReplaceCurrentPosition
               AfterCurrentPosition
               BottomOfCurrentScope
#endif
               locationInScope = MidLevelCollectionTypedefs::BeforeCurrentPosition;
             }

       // Function required by the global tree traversal mechanism
          void visit ( SgNode* astNode );
   };


void
MyTraversal::visit ( SgNode* astNode )
   {
     SgStatement* statement = isSgStatement(astNode);

#if 0
     printf ("astNode->sage_class_name() = %s statement = %p \n",astNode->sage_class_name(),statement);
     if (statement != NULL)
        {
          printf ("Found a statement: %s \n",statement->unparseToString().c_str());

          printf ("Calling AstTests::runAllTests on each statement BEFORE rewrite mechanism \n");
          SgProject* project = TransformationSupport::getProject(statement);
          ROSE_ASSERT(project != NULL);
          AstTests::runAllTests(project);
        }
#endif

     switch (astNode->variantT())
        {
#if 1
          case V_SgVariableDeclaration:
             {
            // Look for a target string so that we avoid adding more than one string
               string currentDeclarationStatement = statement->unparseToString();
            // printf ("Found a declaration: currentDeclarationStatement = %s \n",currentDeclarationStatement.c_str());
               if (currentDeclarationStatement == targetString)
                  {
                    printf ("Found a variable declaration statement (%s) (adding newSourceString = %s) \n",
                         currentDeclarationStatement.c_str(),newSourceString.c_str());

                 // MiddleLevelRewrite::ScopeIdentifierEnum scope = MidLevelCollectionTypedefs::StatementScope;
                    MiddleLevelRewrite::ScopeIdentifierEnum scope = MidLevelCollectionTypedefs::SurroundingScope;
                    MiddleLevelRewrite::insert(statement,newSourceString,scope,locationInScope);
                  }
             }
#endif

#if 0
          case V_SgGlobal:
             { 
               string topOfScopeString = "/* Top of Global Scope */";
//             MiddleLevelRewrite::insert(statement,topOfScopeString,MidLevelCollectionTypedefs::TopOfCurrentScope);

               topOfScopeString = "int topOfGlobalScope;";
//             MiddleLevelRewrite::insert(statement,topOfScopeString,MidLevelCollectionTypedefs::TopOfCurrentScope);

               string bottomOfScopeString = "/* Bottom of Global Scope */";
               MiddleLevelRewrite::insert(statement,bottomOfScopeString,MidLevelCollectionTypedefs::BottomOfCurrentScope);

               bottomOfScopeString = "int bottomOfGlobalScope;";
//             MiddleLevelRewrite::insert(statement,bottomOfScopeString,MidLevelCollectionTypedefs::BottomOfCurrentScope);
             }
#endif

#if 0
          case V_SgBasicBlock:
             { 
               topOfScopeComment = "/* Top of Scope */";
               MiddleLevelRewrite::insert(statement,topOfScopeComment,TopOfScope);
             }
#endif
          default:
             {
            // Nothing to do here ...
             }
        }
   }

int
main( int argc, char * argv[] )
   {
  // This test code tests the AST rewrite mechanism using an arbitrary targetCodeString string and
  // an arbitrary newCodeString string.  The targetCodeString is searched for in the AST (the
  // recognition is dependent upon the unparsed AST matching the target string, so this would be
  // problematic if this were not a test code).  The new code (newCodeString) is then inserted into
  // the AST at the relative position specified (scope and position in scope). All possible
  // positions are tested by this test code.

#if 0
     int i,j;
     for (i = 0; i < MidLevelInterfaceNodeCollection::LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG; i++)
          for (j = 0; j < 2; j++)
             {
               printf ("In main(): MidLevelInterfaceNodeCollection::markerStrings[%d][%d] = %s \n",i,j,MidLevelInterfaceNodeCollection::markerStrings[i][j].c_str());
             }

     printf ("Exiting after output of markerStrings \n");
     ROSE_ABORT();
#endif

  // SgProject project(argc,argv);
     SgProject* project = frontend(argc,argv);

  // Output the source code file (as represented by the SAGE AST) as a PDF file (with bookmarks)
     generatePDF(*project);

  // Output the source code file (as represented by the SAGE AST) as a DOT file (with bookmarks)
     generateDOT(*project);

     printf ("Calling AstTests::runAllTests in main program BEFORE rewrite mechanism \n");
     AstTests::runAllTests(project);

  // Allow compiler options to influence if we operate on the AST
     if ( project->get_skip_transformation() == false )
        {
          MyTraversal traversal;

       // Part of a temporary fix to explicitly clear all the flags
       // AstPreorderTraversal cleanTreeFlags;
       // AstClearVisitFlags cleanTreeFlags;

#if 0
       // Example of how to define a string containing source code
       // cleanTreeFlags.traverse(project);
          traversal.setTargetString("int local_x;");
          traversal.setSourceString("int abc;",true);
          traversal.setLocationInScope(MidLevelCollectionTypedefs::BeforeCurrentPosition);
          traversal.traverse(project,preorder);
#endif

#if 0
       // Example of how to define a string containing source code (containing comments)
       // cleanTreeFlags.traverse(project);
          traversal.setTargetString("int global_x;");
          traversal.setSourceString("#include \"stdio.h\" \n",true);
          traversal.traverse(project,postorder);
#endif

#if 1
       // Example of how to define a string containing source code (containing comments)
       // cleanTreeFlags.traverse(project);
          traversal.setTargetString("int global_x;");
          traversal.setSourceString("#include \"variableDeclaration.h\" \n",true);
          traversal.traverse(project,postorder);
#endif

#if 0
       // Test the use of the variable just added via an include file through an include directive
          traversal.setTargetString("int local_x;");
       // traversal.setSourceString("printf (\"help me!\"\n);",true);
          traversal.setSourceString("variable++;",true);
          traversal.traverse(project,postorder);
#endif

#if 0
       // Example of how to define a comment or CPP directive
       // cleanTreeFlags.traverse(project);
          traversal.setTargetString("int x;");
          traversal.setSourceString("/* comment only (before) */",true);
          traversal.traverse(project,preorder);
#endif

#if 0
       // Example of how to define a string containing source code
       // cleanTreeFlags.traverse(project);
          traversal.setTargetString("int x;");
          traversal.setSourceString("int abc;",true);
          traversal.traverse(project,preorder);
#endif

#if 0
       // Example of how to define a comment or CPP directive
       // cleanTreeFlags.traverse(project);
          traversal.setTargetString("int x;");
          traversal.setSourceString("/* comment only (after) */",false);
          traversal.traverse(project,preorder);
#endif

#if 0
       // Example of how to define a string containing source code
       // cleanTreeFlags.traverse(project);
          traversal.setTargetString("int x;");
          traversal.setSourceString("int xyz;",false);
          traversal.traverse(project,preorder);
#endif

#if 0
       // Example of how to define a macro
       // cleanTreeFlags.traverse(project);
          traversal.setTargetString("int x;");
          traversal.setSourceString("#define MIN(a,b) ((a)<(b)?(a):(b))",true);
          traversal.traverse(project,preorder);
#endif

#if 0
       // Example of how to define a macro definition
       // cleanTreeFlags.traverse(project);
          traversal.setTargetString("int x;");
          traversal.setSourceString("#define STATEMENT_MACRO(name) int name;",true);
          traversal.traverse(project,preorder);
#endif

#if 0
       // Example of how to define a macro call
       // cleanTreeFlags.traverse(project);
          traversal.setTargetString("int x;");
          traversal.setSourceString(MiddleLevelRewrite::postponeMacroExpansion("STATEMENT_MACRO(x1)"),true);
          traversal.traverse(project,preorder);
#endif

#if 0
       // Example of how to define a macro call
       // cleanTreeFlags.traverse(project);
          traversal.setTargetString("int x;");
          traversal.setSourceString(MiddleLevelRewrite::postponeMacroExpansion("STATEMENT_MACRO(x2)"),true);
          traversal.traverse(project,preorder);
#endif

#if 0
       // Example of how to define a string containing source code (containing comments)
       // cleanTreeFlags.traverse(project);
          traversal.setTargetString("int local_x;");
          traversal.setSourceString("int abcdedf;",true);
          traversal.traverse(project,preorder);
#endif
#if 0
       // Example of how to define a string containing source code (containing comments)
       // cleanTreeFlags.traverse(project);
          traversal.setTargetString("int global_x;");
          traversal.setSourceString("int a1; int a2;",true);
          traversal.traverse(project,postorder);
#endif

#if 0
       // Example of how to define a string containing source code (containing comments)
       // cleanTreeFlags.traverse(project);
          traversal.setTargetString("int local_x;");
          traversal.setSourceString("int ijkl;",true);
          traversal.traverse(project,postorder);
#endif

#if 0
       // Example of how to define a string containing source code (containing comments)
       // cleanTreeFlags.traverse(project);
          traversal.setTargetString("int local_x;");
          traversal.setSourceString("/* Starting Comment */ \n int y; { int y; } int zz; for (y=0; y < 10; y++){z = 1;} \n/* Ending Comment */\n",true);
          traversal.traverse(project,preorder);
#endif

#if 0
       // Example of how to define a macro definition
       // cleanTreeFlags.traverse(project);
          traversal.setTargetString("int global_x;");
//        traversal.setSourceString("void myprintf(const char* s);\n#define PRINT_MACRO(name) myprintf(name);",true);
          traversal.setSourceString("#define PRINT_MACRO(name) name;",true);
          traversal.traverse(project,preorder);

       // Example of how to define a string containing source code (containing comments)
       // cleanTreeFlags.traverse(project);
          traversal.setTargetString("int local_x;");
//        traversal.setSourceString(MiddleLevelRewrite::postponeMacroExpansion("PRINT_MACRO(\"Hello Word!\")"),true);
          traversal.setSourceString(postponeMacroExpansion("PRINT_MACRO(\"Hello World!\")"),true);
          traversal.traverse(project,postorder);
#endif

#if 0
       // Example of how to define a macro definition
       // cleanTreeFlags.traverse(project);
          traversal.setTargetString("int global_x;");

       // Note that additional "\n\n\" linefeeds in the string are ignored since only those
       // elements (comments directives and C++ language elements) are picked up in the
       // AST fragement generated from the compilation of the intermediate file where 
       // this string is placed.

          traversal.setSourceString("\n\
#ifdef USE_ROSE\n\
// If using a translator built using ROSE process the simpler tauProtos.h header \n\
// file instead of the more complex TAU.h header file (until ROSE is more robust) \n\
   #include \"tauProtos.h\"\n\n\
// This macro definition could be placed into the tauProtos.h header file \n\
   #define TAU_PROFILE(name, type, group) \\\n\
        static TauGroup_t tau_gr = group; \\\n\
        static FunctionInfo tauFI(name, type, tau_gr, #group); \\\n\
        Profiler tauFP(&tauFI, tau_gr); \n\
#else\n\
   #include \"TAU.h\"\n\
#endif",true);
          traversal.traverse(project,preorder);

       // Example of how to define a use of a macro
       // cleanTreeFlags.traverse(project);
          traversal.setTargetString("int local_x;");
          traversal.setSourceString(MiddleLevelRewrite::postponeMacroExpansion("TAU_PROFILE(\"main\",\"\",TAU_USER)"),true);
          traversal.traverse(project,postorder);
#endif

#if 0
       // Example of how to define a macro definition
       // cleanTreeFlags.traverse(project);
          traversal.setTargetString("int local_x;");
          traversal.setSourceString("#pragma omp parallel for \n",true);
          traversal.traverse(project,preorder);
#endif

        }
       else
        {
          printf ("project->get_skip_transformation() == true \n");
        }
     

  // Call the ROSE backend (unparse to generate transformed 
  // source code and compile it with vendor compiler).
     return backend(project);
   }














