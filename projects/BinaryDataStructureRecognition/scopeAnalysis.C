#include "rose.h"

#include "astStructRecognition.h"

using namespace std;

GlobalScopeAnalysisAttribute* globalScopeAttribute = NULL;

GlobalScopeAnalysisAttribute::GlobalScopeAnalysisAttribute ( string n, SgAsmInterpretation* s, SgProject* p )
   : name(n), asmInterpretation(s), project(p), generatedSourceFile(NULL)
   {
  // Do any initial analysis here!

     using namespace SageBuilder;
     using namespace SageInterface;
     using namespace std;

  // Generate the SgSourceFile using the existing SgProject where it does not currently exist.
  // string fileName = "rose_class_definition.C";
  // string fileName = generateProjectName(p, /* supressSuffix = */ false);
  // string fileName = "rose.C";
     string fileName = "class_design_" + generateProjectName(p, /* supressSuffix = */ false) + ".C";

  // Remove any previous existing file (from previous run). Uses standard C function call "remove".
     printf ("removing file = %s if it exists \n",fileName.c_str());
     int status = remove (fileName.c_str());
     printf ("remove function call status = %d \n",status);
  // This will fail if the file does not already exist.
     ROSE_ASSERT (status == 0 || status == -1);

  // If the file exists then it seems that the build function will read it (so it should have been deleted).
  // generatedSourceFile = isSgSourceFile( buildFile(fileName,fileName,project) );
     generatedSourceFile = isSgSourceFile( buildFile(fileName,"",project) );
     p->get_fileList().push_back(generatedSourceFile);

  // Test the AST
     AstTests::runAllTests(p);
   }

bool
GlobalScopeAnalysisAttribute::isVirtualMemberFunction ( SgAsmFunctionDeclaration* asmFunction )
   {
  // ROSE_ASSERT(virtualFunctionTableList.empty() == false);

     bool result = false;

  // For now loop over the whole table.
     for (size_t i = 0; i < virtualFunctionTableList.size(); i++)
        {
           if (virtualFunctionTableList[i]->isVirtualMemberFunction(asmFunction) == true)
             {
               result = true;
             }
        }

     return result;
   }

VirtualFunctionTable*
GlobalScopeAnalysisAttribute::associatedVirtualFunctionTable ( SgAsmFunctionDeclaration* asmFunction )
   {
     ROSE_ASSERT(virtualFunctionTableList.empty() == false);

  // The semantics of this function is that this should be at least a verified virtual function.
     ROSE_ASSERT(isVirtualMemberFunction(asmFunction) == true);

     VirtualFunctionTable* result = NULL;

  // For now loop over the whole table.
     for (size_t i = 0; i < virtualFunctionTableList.size(); i++)
        {
          if (virtualFunctionTableList[i]->isVirtualMemberFunction(asmFunction) == true)
             {
               result = virtualFunctionTableList[i];
             }
        }

  // Since this is a virtual function is should exist in a table (so we can assert).
     ROSE_ASSERT(result != NULL);

     return result;
   }


void
addGlobalScopeAnalysisAttributes( SgProject* project )
   {
     struct Visitor: public AstSimpleProcessing
        {
       // Save the project so that we can use it in the visit function
          SgProject* project;

          Visitor( SgProject* p ) : project(p) {}

          virtual void visit(SgNode* astNode)
             {
               SgAsmInterpretation* asmGlobalScope = isSgAsmInterpretation(astNode);
               if (asmGlobalScope != NULL)
                  {
                    ROSE_ASSERT(project != NULL);

                    string name = "GlobalScope";
                    GlobalScopeAnalysisAttribute* globalScopeAnalysisAttribute = new GlobalScopeAnalysisAttribute(name,asmGlobalScope,project);
                    ROSE_ASSERT(globalScopeAnalysisAttribute != NULL);

                 // Save this as a global variable so that the rest of the analysis can refer to it easily.
                    globalScopeAttribute = globalScopeAnalysisAttribute;
                    ROSE_ASSERT(globalScopeAttribute != NULL);

                 // Add it to the AST (so it can be found later in another pass over the AST)
                    asmGlobalScope->addNewAttribute("GlobalScopeAnalysisAttribute",globalScopeAnalysisAttribute);
                  }
             }
        };

     Visitor v(project);

  // Verify that the global variable is noy yet initialized properly.
     ROSE_ASSERT(globalScopeAttribute == NULL);

     v.traverse(project, preorder);

  // Verify that the global variable is initialized properly.
     ROSE_ASSERT(globalScopeAttribute != NULL);
   }


void
addScopeAnalysisAttributes( SgProject* project )
   {
  // Handle the global scope
     addGlobalScopeAnalysisAttributes(project);

  // TODO handle local scopes for each function, if required.
  // An analysis could also try to detect common patterns of usage and
  // use this to build up namespaces (matching namespaces of the original
  // program would I think not be possible, but this would provide 
  // organization for a large scale program.

   }
