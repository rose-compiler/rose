#include "rose.h"
#include "Rose/AST/NodeId.h"
#include "stdio.h"

#ifdef _MSC_VER
#include <Windows.h>
#define sleep Sleep
#endif

//Preorder traversal that prints out every nodeId and its nodetype
//if this matches between runs that indicates that the memory pool is stable IMO.
class ClassPrintPre: public AstPrePostProcessing
{
public:
  ClassPrintPre()  { }
  ~ClassPrintPre()  {  }
protected:
    virtual void preOrderVisit(SgNode *node)
    {
      //1. Get the NodeId from the SgNode
      const Rose::AST::NodeId nId = Rose::AST::NodeId::getId(node);
      printf("%s %s\n", nId.toString().c_str(), nId.getNode()->class_name().c_str());

      //1. verify we get the same SgNode back from the generated nodeId String
      SgNode *retNode(Rose::AST::NodeId::getNode(nId.toString()));
      ROSE_ASSERT(retNode == node);
    }
    virtual void postOrderVisit(SgNode *node)
    {
      ;
    }

private:
  //Each file will have a set of function names it contains references to.
  //We don't want duplicates (hence a set of function names)
  //And files will be added lazily.  (ie, only if they contain a lowercase function)
  std::map<std::string, std::set<std::string>* > filesToFunctions;

};

int
main ( int argc, char* argv[] )
   {
     TimingPerformance timer ("main() execution time (sec) = ");
     
     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);
     ClassPrintPre traversal;
     traversal.traverseInputFiles(project);

     
  // Output any saved performance data (see ROSE/src/astDiagnostics/AstPerformance.h)
  // AstPerformance::generateReportToFile(project->get_file(0).get_sourceFileNameWithPath(),project->get_compilationPerformanceFile());
  // AstPerformance::generateReportToFile(project);
     timer.generateReportToFile(project);
   }
