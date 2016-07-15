
#include "SgGraphTemplate.h"
#include "graphProcessing.h"
#include "staticCFG.h"
#include <iostream>
#include <fstream>

/* Testing the graph traversal mechanism now implementing in AstProcessing.h (inside src/midend/astProcessing/)*/
#include <sys/time.h>
#include <sys/resource.h>
using namespace std;
using namespace boost;
using namespace rose;







typedef myGraph CFGforT;

stringstream ss;

class visitorTraversal : public SgGraphTraversal<CFGforT>
   {
     public:
          void analyzePath(std::vector<VertexID>& pth);
          SgIncidenceDirectedGraph* g;
          myGraph* orig;
          stringstream sso;
          StaticCFG::CFG* cfg;
          //std::set<std::vector<string> > paths;
   };

void visitorTraversal::analyzePath(std::vector<VertexID>& pathR) {
    std::vector<SgGraphNode*> exprPath;
    std::vector<string> path;
    //ss << "vector<string> sss;\n";
    for (unsigned int j = 0; j < pathR.size(); j++) {
       SgGraphNode* R = getGraphNode[pathR[j]];
      CFGNode cf = cfg->toCFGNode(R);
   //    path.push_back(R->get_name());
      string str = cf.toString();
      str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
       //ss << "string str = \"" << str << "\";\n";
       ss << "sss.push_back(\"" << str << "\");\n";
    }
    ss << "sssv.insert(sss);\n";
    ss << "sss.clear();\n";
   // paths.insert(path);
    
}


int main(int argc, char *argv[]) {
  ss << "#include \"SgGraphTemplate.h\"\n";
  ss << "#include \"graphProcessing.h\"\n";
  ss << "#include \"staticCFG.h\"\n";
  ss << "using namespace std;\n";
  ss << "using namespace boost;\n";






  //ss << "set<vector<string> >  sssv;\n";
  //ss << "vector<string> sss;\n";
  ss << "typedef myGraph CFGforT;\n";

  
  SgProject* proj = frontend(argc,argv);
  ROSE_ASSERT (proj != NULL);

  SgFunctionDeclaration* mainDefDecl = SageInterface::findMain(proj);

  SgFunctionDefinition* mainDef = mainDefDecl->get_definition();
   visitorTraversal* vis = new visitorTraversal();
    StaticCFG::CFG cfg(mainDef);


//    stringstream ss;
    string fileName= StringUtility::stripPathFromFileName(mainDef->get_file_info()->get_filenameString());
    string Cfilename = fileName+"."+ mainDef->get_declaration()->get_name() +"test.C";
    
   // cfgToDot(mainDef,dotFileName1);
    //cfg->buildFullCFG();
    SgIncidenceDirectedGraph* g = new SgIncidenceDirectedGraph();
    g = cfg.getGraph();
    myGraph* mg = new myGraph();
    mg = instantiateGraph(g, cfg);
    //vis->constructPathAnalyzer(mg, true, 0, 0, true);

   // std::cout << "took: " << timeDifference(t2, t1) << std::endl;
    //cfg.clearNodesAndEdges();
    //std::cout << "finished" << std::endl;
 //   std::cout << "tltnodes: " << vis->tltnodes << " paths: " << vis->paths << std::endl;
 //   delete vis;
//}


ss << "class visitorTraversal : public SgGraphTraversal<CFGforT>\n";
ss << "  {\n";
ss << "     public:\n";
ss << "     vector<string> sss;\n";
ss << "     set<vector<string> > sssv;\n";
//vis->constructPathAnalyzer(mg, true, 0, 0, true);
ss << "         void analyzePath(std::vector<VertexID>& pth);\n";
ss << "         SgIncidenceDirectedGraph* g;\n";
ss << "          myGraph* orig;\n";
ss << "          StaticCFG::CFG* cfg;\n";
ss << "          std::vector<std::vector<string> > paths;\n";
ss << "   };\n";

ss << "void visitorTraversal::analyzePath(std::vector<VertexID>& pathR) {\n";
ss << "    std::vector<string> path;\n";
ss << "   for (unsigned int j = 0; j < pathR.size(); j++) {\n";
ss << "       SgGraphNode* R = getGraphNode[pathR[j]];\n";
ss << "      CFGNode cf = cfg->toCFGNode(R);\n";
ss << "       string str = cf.toString();";
ss << "str.erase(std::remove(str.begin(), str.end(), '\\n'), str.end());\n";

ss << "       path.push_back(str);\n";
ss << "    }\n";
ss << "    paths.push_back(path);\n";
ss << "   // ROSE_ASSERT(sssv.find(path) != sssv.end());\n";

ss << "}\n";

//ss << "set<vector<string> > sssv;\n";

ss << "int main(int argc, char *argv[]) {\n";
ss << "SgProject* proj = frontend(argc,argv);\n";
ss << "ROSE_ASSERT (proj != NULL);\n";
ss << "SgFunctionDeclaration* mainDefDecl = SageInterface::findMain(proj);\n";
ss << "SgFunctionDefinition* mainDef = mainDefDecl->get_definition();\n";
ss << "visitorTraversal* vis = new visitorTraversal();\n";
ss << "StaticCFG::CFG cfg(mainDef);\n";
ss << "stringstream ss;\n";
ss << "string fileName= StringUtility::stripPathFromFileName(mainDef->get_file_info()->get_filenameString());\n";
ss << "string dotFileName1=fileName+\".\"+ mainDef->get_declaration()->get_name() +\".dot\";\n";
ss << "cfgToDot(mainDef,dotFileName1);\n";
ss << "SgIncidenceDirectedGraph* g = new SgIncidenceDirectedGraph();\n";
ss << "g = cfg.getGraph();\n";
ss << "myGraph* mg = new myGraph();\n";
ss << "mg = instantiateGraph(g, cfg);\n";
//ss << "vis->tltnodes = 0;\n";
//ss << "vis->paths = 0;\n";
ss << "std::set<std::vector<string> > sssv;\n";
ss << "std::vector<string> sss;\n";
vis->constructPathAnalyzer(mg, true, 0, 0, true);
ss << "vis->sssv = sssv;\n";
ss << "vis->constructPathAnalyzer(mg, true, 0, 0, true);\n";
ss << "ROSE_ASSERT(vis->sssv.size() == vis->paths.size());\n";
ss << "std::cout << \"finished\" << std::endl;\n";
ss << "std::cout << \" paths: \" << vis->paths.size() << std::endl;\n";
ss << "delete vis;\n";
ss << "}\n";
string sst = ss.str();
ofstream myfile;
myfile.open(Cfilename.c_str());
myfile << sst;
myfile.close();
delete vis;
return 0;
}

