/*******
  Author: Craig Rasmussen (Pei-Hung Lin originated Fortran_to_C from which this started)
  Contact: rasmussen17@llnl.gov

  Date Created       : December 10, 2017
 
 This is a source-to-source compiler for Jovial

  Jovial to C translator 
  * input  : Jovial source code
  * output : C source code

 *******/  

#include "rose.h"
#include "j2c.h"
#include "CommandOptions.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace Jovial_to_C;

vector<SgVariableDeclaration*> variableDeclList;
vector<SgStatement*> statementList;
vector<SgNode*> removeList;
stack<SgStatement*> insertList;

// Memory pool traversal for variable declarations
//
#ifdef USE_PLACEHOLDER_TRAVERSAL
class VariableDeclTraversal : public ROSE_VisitorPattern
{
  public:
    void visit(SgVariableDeclaration* varDecl);
};

void VariableDeclTraversal::visit(SgVariableDeclaration* decl)
{
    SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(decl);
    ROSE_ASSERT(variableDeclaration);
    SgInitializedNamePtrList initializedNameList = variableDeclaration->get_variables();
    for (SgInitializedNamePtrList::iterator i=initializedNameList.begin(); i!=initializedNameList.end();++i)
       {
          SgInitializedName* initializedName = isSgInitializedName(*i);
       }

    variableDeclList.push_back(decl);
}
#endif

// Simple traversal for general language translation
//
class Jovial2cTraversal : public AstSimpleProcessing
{
  public:
    virtual void visit(SgNode* n);
};

void Jovial2cTraversal::visit(SgNode* n)
{
  Sg_File_Info* fileInfo = n->get_file_info();
  fileInfo->set_isPartOfTransformation(true);

  /***
    1. The following switch statement searches for Jovial-specific
       AST nodes and transform thems into C nodes. 
    2. The new C nodes are created first.  Attributes and details
       are then copied from original Jovial nodes.  After the 
       copy, original Jovial nodes are deleted. 
  ***/
  switch(n->variantT())
  {
    case V_SgSourceFile:
      {
         SgFile* fileNode = isSgFile(n);
         translateFileName(fileNode);
         break;
      }
    case V_SgGlobal:
      {
         SgGlobal* global = isSgGlobal(n);
         ROSE_ASSERT(global);
         break;
      }
    case V_SgProgramHeaderStatement:
      {
         SgProgramHeaderStatement* ProgramHeaderStatement = isSgProgramHeaderStatement(n);
         ROSE_ASSERT(ProgramHeaderStatement);
         translateProgramHeaderStatement(ProgramHeaderStatement);
      // Deep delete the original Jovial SgProgramHeaderStatement
         removeList.push_back(ProgramHeaderStatement);
         break;
      }
    case V_SgExprStatement:
      {
         SgExprStatement* exprStmt = isSgExprStatement(n);
         SgFunctionCallExp* funcCallExp = isSgFunctionCallExp(exprStmt->get_expression());
         while(insertList.size() > 0 && funcCallExp)
           {
              insertStatement(exprStmt,deepCopy(insertList.top()),true);
              insertList.pop();
           }
         break;
      }
    default:
       break;
  }
}

int main( int argc, char * argv[] )
{
  // Introduces tracking of performance of ROSE at the top most level.                                                                 
     TimingPerformance timer ("AST translation (main): time (sec) = ",true);

  // Build a vector of strings to represent the command line arguments.                                                                
     std::vector<std::string> sourceCommandline = std::vector<std::string>(argv, argv + argc);
     sourceCommandline.push_back("-rose:exit_after_parser");

  // Parse the file and create original language AST
     SgProject* project = frontend(sourceCommandline);

  // Check the AST for consistency
     AstTests::runAllTests(project);

     std::cout << std::endl;
     std::cout << "STARTING translation to C ..." << std::endl;
     std::cout << std::endl;

  // Simple traversal, bottom-up, to translate the rest
     Jovial2cTraversal j2c;
     j2c.traverseInputFiles(project, postorder);

  // Remove all unused statements from the AST
     for (vector<SgStatement*>::iterator i=statementList.begin(); i!=statementList.end(); ++i)
       {
          std::cout << "... removing statement \n";
          removeStatement(*i);
          (*i)->set_parent(NULL);
       }
    
  // Deep delete of the removed nodes 
     for (vector<SgNode*>::iterator i=removeList.begin(); i!=removeList.end(); ++i)
       {
          std::cout << "... deleting node of type: " << (*i)->class_name() << std::endl;
          deepDelete(*i);
       }

     project->set_C_only(true);
     project->set_Fortran_only(false);
      
/*
  1. There should be no Jovial-specific AST nodes in the whole
     AST graph after the translation. 
  
  TODO: make sure translator generates clean AST 
*/

     // FIXME: broken
     // generateDOT(*project);
     if (SgProject::get_verbose() > 2)
       {
          // FIXME: broken for now
          generateAstGraph(project,8000);
       }

  // Output statistics about how ROSE was used...                                                                                      
     if (project->get_verbose() > 1)
       {
          std::cout << AstNodeStatistics::traversalStatistics(project);
          std::cout << AstNodeStatistics::IRnodeUsageStatistics();
       }

  // Just set the project, the report will be generated upon calling the destructor for "timer"                                        
  // Use option "-rose:verbose 2" to see the report.                                                                                   
     timer.set_project(project);

  // Finished with translation
     std::cout << std::endl;

     return backend(project);
}

