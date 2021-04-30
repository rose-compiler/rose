// Example translator to generate dot files of virtual, interprocedural control flow graphs

#include <iostream>
#include <fstream>
#include <memory>
//#include <rose.h>
#include <string>
#ifndef _MSC_VER
#include <err.h>
#endif
#include "SgGraphTemplate.h"
#include "graphProcessing.h"
#include "staticCFG.h"
/* Testing the graph traversal mechanism now implementing in AstProcessing.h (inside src/midend/astProcessing/)*/
#include <sys/time.h>
#include <sys/resource.h>
using namespace std;
using namespace boost;
using namespace Rose;







typedef myGraph CFGforT;





class visitorTraversal : public SgGraphTraversal<CFGforT>
   {
     public:
          int tltnodes;
          int paths;
          //std::vector<std::vector<VertexID> > pathstore;
          void analyzePath(vector<VertexID>& pth);
   };


void visitorTraversal::analyzePath(vector<VertexID>& pathR) {
       // ROSE_ASSERT(find(pathstore.begin(), pathstore.end(), pth) == pathstore.end());
       // pathstore.push_back(pth);
       // #pragma omp atomic
       // paths++;

    //std::vector<SgGraphNode*> exprPath;
    //ss << "vector<string> sss;\n";
    //std::cout << "path" << std::endl;
    //for (unsigned int j = 0; j < pathR.size(); j++) {
    //   SgGraphNode* R = getGraphNode[pathR[j]];
    //  CFGNode cf = gcfg->toCFGNode(R);
   //    path.push_back(R->get_name());
    //  string str = cf.toString();
     // std::cout << str << ", ";
    // }
    // std::cout << std::endl;
        paths++;
        //std::cout << "paths: " << paths << std::endl;

}
/*

#include "rose.h"
#include "interproceduralCFG.h"
#include <string>
using namespace std;
*/
//int main(int argc, char *argv[]) 
//{
  // Build the AST used by ROSE
 // SgProject* proj = frontend(argc,argv);
 // ROSE_ASSERT (proj != NULL); 
        

//#include "rose.h"

//using namespace std;


 // Function querySolverAccessFunctions() 
 // find access functions (function name starts with "get_" or "set_")
/*
NodeQuerySynthesizedAttributeType
querySolverAccessFunctions (SgNode * astNode)
   {
     ROSE_ASSERT (astNode != 0);
     NodeQuerySynthesizedAttributeType returnNodeList;

     SgFunctionDeclaration* funcDecl = isSgFunctionDeclaration(astNode);

     if (funcDecl != NULL)
        {
          string functionName = funcDecl->get_name().str();
          if ( (functionName.length() >= 4) && ((functionName.substr(0,4) == "get_") || (functionName.substr(0,4) == "set_")) )
               returnNodeList.push_back (astNode);
        }

     return returnNodeList;
   }
*/
// Function printFunctionDeclarationList will print all function names in the list
void printFunctionDeclarationList(Rose_STL_Container<SgNode*> functionDeclarationList)
   {
     int counter = 0;
     for (Rose_STL_Container<SgNode*>::iterator i = functionDeclarationList.begin(); i != functionDeclarationList.end(); i++)
        {
       // Build a pointer to the current type so that we can call the get_name() member function.
          SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(*i);
          ROSE_ASSERT(functionDeclaration != NULL);

       // output the function number and the name of the function
          printf ("function name #%d is %s at line %d \n",
               counter++,functionDeclaration->get_name().str(),
               functionDeclaration->get_file_info()->get_line());
        }
   }

int main( int argc, char * argv[] )
   {
  // Build the AST used by ROSE
     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT(project != NULL);

  // Build a list of functions within the AST and find all access functions 
  // (function name starts with "get_" or "set_")

  // Build list using a query of the whole AST
     Rose_STL_Container<SgNode*> functionDefinitionList = NodeQuery::querySubTree (project,V_SgFunctionDefinition);

  // Build list using nested Queries (operating on return result of previous query)
  //   Rose_STL_Container<SgNode*> accessFunctionsList;
  //   accessFunctionsList = NodeQuery::queryNodeList (functionDeclarationList,&querySolverAccessFunctions);
  //   printFunctionDeclarationList(accessFunctionsList);

  // Alternative form of same query building the list using a query of the whole AST
   //  accessFunctionsList = NodeQuery::querySubTree (project,&querySolverAccessFunctions);
   //  printFunctionDeclarationList(accessFunctionsList);
int counter = 0;
for (Rose_STL_Container<SgNode*>::iterator i = functionDefinitionList.begin(); i != functionDefinitionList.end(); i++) {
   SgFunctionDefinition* fnc = isSgFunctionDefinition(*i);
  stringstream ss; 
  SgFunctionDeclaration* functionDeclaration = fnc->get_declaration();
  string fileName= functionDeclaration->get_name().str();//StringUtility::stripPathFromFileName(mainDef->get_file_info()->get_filenameString());
    string dotFileName1;
ss << fileName << "." << counter << ".dot";
    counter++;
    dotFileName1 = ss.str();
    //SgFunctionDefinition* fnc = functionDeclaration->get_definition();
    if (fnc != NULL) 
    {
      StaticCFG::CFG            cfg{fnc};
      visitorTraversal          vis;
      SgIncidenceDirectedGraph* g = cfg.getGraph(); // not owning
      std::unique_ptr<myGraph>  mg{instantiateGraph(g, cfg)};

      vis.tltnodes = 0;
      vis.paths = 0;
      std::cout << dotFileName1 << std::endl;
      cfgToDot(fnc,dotFileName1);
    //vis->firstPrepGraph(constcfg);
    //t1 = getCPUTime();
      vis.constructPathAnalyzer(&*mg, true, 0, 0, true);
    
    //t2 = getCPUTim
      std::cout << "function: " << fileName << std::endl;
      std::cout << "paths: " << vis.paths << std::endl;
    //std::cout << "took: " << timeDifference(t2, t1) << std::endl;
    }
    }
   SgProject* proj = project; 
   SgFunctionDeclaration* mainDefDecl = SageInterface::findMain(proj);
   if (mainDefDecl != NULL) {
     SgFunctionDefinition* mainDef = mainDefDecl->get_definition();
     visitorTraversal vis;
     StaticCFG::CFG cfg(mainDef);
   //cfg.buildFullCFG();
     stringstream ss;
     string fileName= StringUtility::stripPathFromFileName(mainDef->get_file_info()->get_filenameString());
     string dotFileName1=fileName+"."+ mainDef->get_declaration()->get_name() +".dot";

     cfgToDot(mainDef,dotFileName1);
    //cfg->buildFullCFG();
     SgIncidenceDirectedGraph* g = cfg.getGraph();
     std::unique_ptr<myGraph> mg{instantiateGraph(g, cfg)};
     vis.tltnodes = 0;
     vis.paths = 0;
    //vis->firstPrepGraph(constcfg);
     vis.constructPathAnalyzer(&*mg, true, 0, 0, true);
    //std::cout << "took: " << timeDifference(t2, t1) << std::endl;
    //cfg.clearNodesAndEdges();
     std::cout << "finished" << std::endl;
     std::cout << "tltnodes: " << vis.tltnodes << " paths: " << vis.paths << std::endl;
    //std::cout << "ipaths: " << ipaths << std::endl;
    }

// Another way to query for collections of IR nodes
     VariantVector vv1 = V_SgClassDefinition;
     std::cout << "Number of class definitions in the memory pool is: " << NodeQuery::queryMemoryPool(vv1).size() << std::endl;

  // Another way to query for collections of multiple IR nodes.
  // VariantVector(V_SgType) is internally expanded to all IR nodes derived from SgType.
     VariantVector vv2 = VariantVector(V_SgClassDefinition) + VariantVector(V_SgType);
     std::cout << "Number of class definitions AND types in the memory pool is: " << NodeQuery::queryMemoryPool(vv2).size() << std::endl;

  // Note: Show composition of AST queries

     return 0;
   }
/*
if (SageInterface::findMain(proj)) {
  SgFunctionDeclaration* mainDefDecl = SageInterface::findMain(proj);
  ROSE_ASSERT (mainDefDecl != NULL); 

  SgFunctionDefinition* mainDef = mainDefDecl->get_definition();
  ROSE_ASSERT (mainDef != NULL); 

   string fileName= StringUtility::stripPathFromFileName(mainDef->get_file_info()->get_filenameString());
    string dotFileName1=fileName+"."+ mainDef->get_declaration()->get_name() +".dot";


  StaticCFG::InterproceduralCFG cfg(mainDef);

  // Dump out the full CFG, including bookkeeping nodes
  cfg.cfgToDot(mainDef, dotFileName1);
  //}
  }
  return 0;
}
*/
