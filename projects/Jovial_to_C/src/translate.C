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

#define DEBUG_JOVIAL_TRANSLATION 1

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace Jovial_to_C;

vector<SgVariableDeclaration*> variableDeclList;
vector<SgStatement*> statementList;
vector<SgNode*> removeList;
stack<SgStatement*> insertList;

// Simple traversal for general language translation
//
class Jovial2cTraversal : public AstSimpleProcessing
{
  public:
    virtual void visit(SgNode* n);

  private:
   SgSourceFile* src_file{nullptr};

  private:
   PreprocessingInfo* preprocess_directive{nullptr};
};

void Jovial2cTraversal::visit(SgNode* n)
{
#if DEBUG_JOVIAL_TRANSLATION
   std::cout << "... visit node: classname is " << n->class_name() << std::endl;
#endif

  // Apparently source files don't want to be part of the transformation
   if (isSgSourceFile(n) == NULL)
      {
         Sg_File_Info* fileInfo = n->get_file_info();
         fileInfo->set_isPartOfTransformation(true);
      }
   else
      {
         src_file = isSgSourceFile(n);
         ROSE_ASSERT(src_file);
         ROSE_ASSERT(src_file->get_startOfConstruct()->get_file_id() >= 0);
      }

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

      // will contain modified/transformed nodes
         global->set_isModified(true);
         global->set_containsTransformation(true);

         break;
      }
    case V_SgJovialDefineDeclaration:
      {
         SgJovialDefineDeclaration* defineDeclaration = isSgJovialDefineDeclaration(n);
         ROSE_ASSERT(defineDeclaration);
         preprocess_directive = translateJovialDefineDeclaration(defineDeclaration);
      // Deep delete the original SgJovialDefineDeclaration
         removeList.push_back(defineDeclaration);
         break;
      }
    case V_SgProgramHeaderStatement:
      {
         SgProgramHeaderStatement* programHeaderStatement = isSgProgramHeaderStatement(n);
         ROSE_ASSERT(programHeaderStatement);
         translateProgramHeaderStatement(programHeaderStatement);
      // Deep delete the original Jovial SgProgramHeaderStatement
         removeList.push_back(programHeaderStatement);
         break;
      }
    case V_SgJovialTableStatement:
      {
         SgJovialTableStatement* tableStatement = isSgJovialTableStatement(n);
         ROSE_ASSERT(tableStatement);
         translateJovialTableStatement(tableStatement);
      // Deep delete the original Jovial SgProgramHeaderStatement
      // removeList.push_back(compoolStatement);
         break;
      }
    case V_SgJovialCompoolStatement:
      {
         SgJovialCompoolStatement* compoolStatement = isSgJovialCompoolStatement(n);
         ROSE_ASSERT(compoolStatement);
         translateJovialCompoolStatement(compoolStatement);
      // Deep delete the original Jovial SgProgramHeaderStatement
      // removeList.push_back(compoolStatement);
         break;
      }
    case V_SgInitializedName:
      {
         SgInitializedName* name = isSgInitializedName(n);
         ROSE_ASSERT(name);
         translateInitializedName(name);
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
    case V_SgStopOrPauseStatement:
      {
         SgStopOrPauseStatement* stopOrPauseStmt = isSgStopOrPauseStatement(n);
         ROSE_ASSERT(stopOrPauseStmt);
         translateStopOrPauseStatement(stopOrPauseStmt);
         break;
      }

    default:
       break;
  }

  // Attach any dangling preprocessor directives
  // TODO - this should be a list
  if (preprocess_directive && (n->variantT() == V_SgVariableDeclaration))
     {
        SgLocatedNode* located_node = isSgLocatedNode(n);
        if (located_node)
           {
              located_node->addToAttachedPreprocessingInfo(preprocess_directive);
              preprocess_directive = nullptr;
           }
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

     ROSE_ASSERT(project->numberOfFiles() == 1);
     SgFilePtrList file_list = project->get_files();
     ROSE_ASSERT(file_list.size() == 1);
     SgFile* file = file_list[0];

     SgSourceFile* src_file = isSgSourceFile(file);
     ROSE_ASSERT(src_file);

     std::cout << std::endl;
     std::cout << "STARTING translation to C ..." << std::endl;
     std::cout << std::endl;

  // Simple traversal, bottom-up, to translate the rest
     Jovial2cTraversal j2c;
     j2c.traverseInputFiles(project, postorder);

  // Remove all unused statements from the AST
     for (vector<SgStatement*>::iterator i=statementList.begin(); i!=statementList.end(); ++i)
       {
#if DEBUG_JOVIAL_TRANSLATION
          std::cout << "... removing statement " << *i << " : finfo is " << (*i)->get_file_info() << endl;;
#endif
          removeStatement(*i);
          (*i)->set_parent(NULL);
       }
    
  // Deep delete of the removed nodes 
     for (vector<SgNode*>::iterator i=removeList.begin(); i!=removeList.end(); ++i)
       {
#if DEBUG_JOVIAL_TRANSLATION
          std::cout << "... deleting statement " << *i << " : finfo is " << (*i)->get_file_info() << endl;;
#endif
          deepDelete(*i);
       }

     std::cout << "... # of files is " << project->numberOfFiles() << std::endl;

     project->set_Cxx_only(true);
     project->set_Fortran_only(false);
     project->set_Jovial_only(false);

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
  // std::cout << "finished translation \n" << std::endl;

     return backend(project);
}
