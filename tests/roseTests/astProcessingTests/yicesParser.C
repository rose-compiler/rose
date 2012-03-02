#include <iostream>
#include <fstream>
//#include <rose.h>
#include <string>
#include <err.h>
#include <SgGraphTemplate.h>
#include <graphProcessing.h>
#include <staticCFG.h>
#include <yices_c.h>
/* Testing the graph traversal mechanism now implementing in AstProcessing.h (inside src/midend/astProcessing/)*/


using namespace std;
using namespace boost;


typedef myGraph CFGforT;
bool unknown_flag;
std::map<std::vector<SgGraphNode*>, std::set<SgNode*> > calledMap;
std::set<SgNode*> unknownFunctions;

string getGraphNodeType(SgGraphNode* sn);
class visitorTraversalFunc : public SgGraphTraversal<CFGforT>
   {
     public:
          int tltnodes;
          std::vector<std::vector<SgGraphNode*> > vpaths;
          void analyzePath(std::vector<VertexID>& pth);
          CFGforT* orig;
          //std::map<SgVariableSymbol, string> nameOf;
          //int nvars;
   };


void visitorTraversalFunc::analyzePath(std::vector<VertexID>& pathR) {
    //ROSE_ASSERT(pathcheck.find(pathR) == pathcheck.end());
    //pathcheck.insert(pathR);

    //std::cout << "funcAnalyze" << std::endl;
    std::vector<SgGraphNode*> path;
    for (unsigned int j = 0; j < pathR.size(); j++) {
       SgGraphNode* R = (*orig)[pathR[j]].sg;
       path.push_back(R);
    }
    ROSE_ASSERT(isSgFunctionDefinition(path[0]->get_SgNode()));
    int curr = 0;
    //std::cout << "a vpath" << std::endl;
   // for (int q = 0; q < path.size(); q++) {
   //     std::cout << getGraphNodeType(path[q]) << std::endl;
   // }
   // std::cout << "\n\n";
    if (path.back()->get_SgNode() == path.front()->get_SgNode()) {
    vpaths.push_back(path);
    }
}


std::map<SgNode*, std::vector<std::vector<SgGraphNode*> > > FuncPathMap;

bool forFlag;

int nvars;
std::map<SgName, string> nameOf;
bool noAssert;
std::map<SgNode*, int> forsts;

std::map<int, std::vector<SgGraphNode*> > intvecmap;
std::map<std::vector<SgGraphNode*>, int> vecintmap;

typedef myGraph CFGforT;

std::vector<std::vector<SgGraphNode*> > paths;

struct Vertex2 {
    SgGraphNode* sg;
    bool vardec;
    string varstr;
    bool expr;
    string exprstr;
};

struct Edge2 {
    SgDirectedGraphEdge* gedge;
};

typedef boost::adjacency_list<
        boost::vecS,
        boost::vecS,
        boost::bidirectionalS,
        Vertex2,
        Edge2
> newGraph;

typedef newGraph::vertex_descriptor VertexID2;
typedef newGraph::edge_descriptor EdgeID2;

    typedef boost::graph_traits<newGraph>::vertex_iterator vertex_iterator;
    typedef boost::graph_traits<newGraph>::out_edge_iterator out_edge_iterator;
    typedef boost::graph_traits<newGraph>::in_edge_iterator in_edge_iterator;
    typedef boost::graph_traits<newGraph>::edge_iterator edge_iterator;


/**
    These should be unsatisfiable under integer logic
    int main (argc, argv) {
    int x = argc[0];
    int y = argc[1];
    if (x > 0 && y < 0) {
    
    if (x == y) {
    }
    else if (y > x) {
    }
    else if (x*y > 0) {
    }
    else {
    }
    }
    else {
    }
    return 0;

    Therefore there should only be 2 satisfiable paths, the primary if
    statement being false or the primary if statement being true and
    everything else false

    This should translate to

    (set-logic QF_LIA)
    (declare-fun x () Int)
    (declare-fun y () Int)
    (assert (> (x 0)))
    (assert (< (y 0)))
    (assert (= (x y)))
    (check-sat)
    (pop 1)
    (push 1)
    (assert (> (y x)))
    (check-sat)
    (pop 1)
    (push 1)
    (assert (> (* (x y) ) 0))
    (check-sat)
    (pop 1)
    (check-sat)
    (pop 2)
    (check-sat)
    


**/


//Process CFG representation into SMT



void propagateFunctionCall(std::vector<SgGraphNode*> path, int i, int pathnum) {
            SgFunctionDeclaration* sgfd = isSgFunctionCallExp(path[i]->get_SgNode())->getAssociatedFunctionDeclaration();
            SgName nam = sgfd->get_qualified_name();
            //std::cout << "function: " << nam.getString() << std::endl;
            ROSE_ASSERT(sgfd != NULL);
            SgFunctionDefinition* sgfdef = sgfd->get_definition();
            ROSE_ASSERT(sgfdef != NULL);
            int kk = i + 1;
            int indiec = 0;
            std::vector<std::vector<SgGraphNode*> > funcPaths = FuncPathMap[sgfdef];
            ROSE_ASSERT(funcPaths.size() > 0);
            int funcIndEnd = path[i]->get_SgNode()->cfgIndexForEnd();
            if (path.size() > 1) {
              //  std::cout << "funcIndEnd: " << funcIndEnd << std::endl;

                while (indiec < funcIndEnd) {

                if (path[kk]->get_SgNode() == path[i]->get_SgNode()) {
                    indiec++;
                    if (indiec == funcIndEnd) {
                        break;
                    }
                }
                kk++;
            }
            }
            int startingpoint = kk;


            //std::vector<SgGraphNode*>::iterator it = path.begin();
            //it += kk;
            std::vector<SgGraphNode*> oldpath = path;
            std::vector<SgGraphNode*> newpath;
           // std::cout << "oldpath.size(): " << oldpath.size() << std::endl;
            for (int qe = 0; qe < startingpoint; qe++) {
                newpath.push_back(path[qe]);
            }
            for (int qe2 = 0; qe2 < funcPaths[0].size(); qe2++) {
                newpath.push_back(funcPaths[0][qe2]);
            }
            for (int qe3 = startingpoint; qe3 < path.size(); qe3++) {
                newpath.push_back(path[qe3]);
            }
            //path = newpath; 
            paths[pathnum] = newpath;
            calledMap[newpath] = calledMap[path];
            calledMap[newpath].insert(path[i]->get_SgNode());
            //npaths.push_back(newpath);
            //path.insert(it, funcPaths[0].begin(), funcPaths[0].end());
            //std::cout << "newpath.size(): " << newpath.size() << std::endl;
            //std::cout << "funcPaths.size(): " << funcPaths.size() << std::endl;
           if (funcPaths.size() == 1) {
            return;
           }
            for (int qw = 1; qw < funcPaths.size(); qw++) {
            //if (qw != pathnum) {
                std::vector<SgGraphNode*> npath;// = oldpath;
          for (int qe = 0; qe < startingpoint; qe++) {
                npath.push_back(path[qe]);
           }
           for (int qe2 = 0; qe2 < funcPaths[qw].size(); qe2++) {
                npath.push_back(funcPaths[qw][qe2]);
            }
            for (int qe3 = startingpoint; qe3 < path.size(); qe3++) {
                npath.push_back(path[qe3]);
            }
            calledMap[npath] = calledMap[newpath];

             


                //npath.insert(it, funcPaths[qw].begin(), funcPaths[qw].end());
                paths.push_back(npath);
            }
            //}
            //std::cout << "paths.size(): " << paths.size() << std::endl;
            std::vector<std::vector<SgGraphNode*> >::iterator tt = paths.begin();
            //tt += pathnum;
           // (*tt) = npaths[0]; 
           // tt+=1;
           // paths.insert((*tt), 
            //called.push_back(path[i+1]->get_SgNode());
       }



std::map<SgName, yices_expr> getExpr;
//string getGraphNodeType(SgGraphNode* sn);

class visitorTraversal : public SgGraphTraversal<CFGforT>
   {
     public:
          int tltnodes;
      //    int paths;
          //std::map<SgName, yices_expr> getExpr;
          std::set<SgNode*> knownNodes;     
    //      std::vector<std::vector<SgGraphNode*> > pathstore;
          void analyzePath(std::vector<VertexID>& pth);
          SgIncidenceDirectedGraph* g;
          myGraph* orig;
          StaticCFG::CFG* cfg;
          int pathnumber;
          //std::map<SgVariableSymbol, string> nameOf;
          //int nvars;
   };

class visitorTraversal2 : public SgGraphTraversal<newGraph>
   {
     public:
          int tltnodes;
          int paths;
          void analyzePath(std::vector<VertexID>& pth);
          //std::map<SgVariableSymbol, string> nameOf;
          //int nvars;
   };

newGraph* nGraph;
         SgIncidenceDirectedGraph* openg;
          myGraph* openorig;
          StaticCFG::CFG* opencfg;



long getIndex(SgGraphNode* n) {
    unsigned int i = n->get_index(); 
    return i;
}


yices_expr mainParse(vector<SgGraphNode*> expr, yices_context& ctx);

int rounds;
int pathnum;
//std::set<SgGraphNode*> knownGraphNodes;
std::set<std::pair<VertexID2, VertexID2> > knownEdges;
std::map<SgGraphNode*, VertexID2> graphVertex;

void visitorTraversal2::analyzePath(std::vector<VertexID2>& pathR) {
    tltnodes += pathR.size();
    paths++;
    //std::cout << "path: " << paths << std::endl;
}


std::map<int, EdgeID2> intedgemap;
std::map<EdgeID2, int> edgeintmap;
std::map<VertexID2, int> intmap;

int
getSource(int& edge, newGraph*& g)
{
    EdgeID2 e = intedgemap[edge];
    VertexID2 v = boost::source(e, *g);
    return(intmap[v]);
}



int getTarget(int& edge, newGraph*& g)
{
    EdgeID2 e = intedgemap[edge];
    VertexID2 v = boost::target(e, *g);
    return (intmap[v]);
}

        void printCFGNode2(int& cf, VertexID2 v, newGraph*& g, std::ofstream& o)
        {
            stringstream str;
            if ((*g)[v].expr) {
            //std::cout << cf << "expr: " << (*g)[v].exprstr << std::endl;
            str << cf << " expr: " << (*g)[v].exprstr;
            }
            else if ((*g)[v].vardec) {
            //std::cout << cf << " vardec: " << (*g)[v].varstr << std::endl;
            str << cf << " vardec: " << (*g)[v].varstr;
            }
            else {
               str << cf;
            }
            std::string nodeColor = "black";
            o << cf << " [label=\"" << " num:" << str.str() << "\", color=\"" << nodeColor << "\", style=\"" << "solid" << "\"];\n";
        }

        void printCFGEdge2(int& cf, newGraph*& cfg, std::ofstream& o)
        {
            int src = getSource(cf, cfg);
            int tar = getTarget(cf, cfg);
            o << src << " -> " << tar << " [label=\"" << src << " " << tar << "\", style=\"" << "solid" << "\"];\n";
        }

        void printHotness2(newGraph*& g)
        {
            const newGraph* gc = g;
            int currhot = 0;
            
            std::ofstream mf;
            std::stringstream filenam;
            filenam << "hotness2" << currhot << ".dot";
            std::string fn = filenam.str();
            mf.open(fn.c_str());

            mf << "digraph defaultName { \n";
            vertex_iterator v, vend;
            edge_iterator e, eend;
            int intcurr = 1;
            int intcurr2 = 1;
            for (tie(v, vend) = vertices(*g); v != vend; ++v)
            {
                intmap[*v] = intcurr;
                printCFGNode2(intcurr, *v, g,  mf);
                intcurr++;
            }
            for (tie(e, eend) = edges(*g); e != eend; ++e)
            {
                edgeintmap[*e] = intcurr2;
                intedgemap[intcurr2] = *e;
                printCFGEdge2(intcurr2, g, mf);
                intcurr2++;
            }
            mf.close();
        }


string getType(SgNode* n) {
    if (isSgTypeInt(n)) {
        return "int";
    }
    else if (isSgTypeDouble(n)) {
        return "double";
    }
    else if (isSgTypeFloat(n)) {
        return "float";
    }
    else if (isSgTypeShort(n)) {
        return "short";
    }
    else if (isSgTypeLong(n)) {
        return "long";
    }
    else if (isSgTypeLongLong(n)) {
        return "long long int";
    }
    else if (isSgTypeLongDouble(n)) {
        return "long double";
    }
    else if (isSgTypeBool(n)) {
        return "bool";
    }
    return "";
}

//string getSentence(SgGraphNode* n, std::vector<SgGraphNode*> nodesentence) {
   

yices_expr evalFunction(std::vector<SgGraphNode*> path, yices_context& ctx, bool mainFlag);

std::vector<VertexID> exprs;

int ipaths;


//std::vector<std::vector<SgGraphNode*> > paths;

std::set<std::vector<SgGraphNode*> > globalPaths;

//std::map<std::vector<SgGraphNode*>, std::set<SgNode*> > calledMap;

void visitorTraversal::analyzePath(std::vector<VertexID>& pathR) {
   //ROSE_ASSERT(globalPaths.find(pathR) == globalPaths.end());
   //globalPaths.insert(pathR);
   //yices_context ctx;
   paths.clear();
   //FuncPathMap.clear();
   openg = g;
   opencfg = cfg;
   openorig = orig; 
   unsigned int i = 0;
    noAssert = false;
    rounds = 0;
    forFlag = false;
    stringstream pathstream;
    //std::set<SgNode*> knownNodes;
    nameOf.clear();
    getExpr.clear();
    //VertexID2 start = boost::add_vertex(*nGraph);
    //graphVertex[(*orig)[pathR[0]]] = start;
    //std::cout << "path: " << pathnum << std::endl;
    //for (int i = 0; i < pathR.size(); i++) {
    //    std::cout << vertintmap[pathR[i]] << ", ";
   // }
    //std::cout << std::endl;
    pathnum++;
    bool inconsistent = false;
    std::vector<SgGraphNode*> path;
    //std::vector<SgGraphNode*> pathR;
    std::vector<SgGraphNode*> exprPath;
    for (unsigned int j = 0; j < pathR.size(); j++) {
       SgGraphNode* R = (*orig)[pathR[j]].sg;
       path.push_back(R);
    }
    if (path.back()->get_SgNode() != path.front()->get_SgNode()) {
        return;
    }
    //ROSE_ASSERT(globalPaths.find(path) == globalPaths.end());
    //globalPaths.insert(path);
    //std::cout << "path: " << std::endl;
    //for (int qr = 0; qr < path.size(); qr++) {
    //    std::cout << getGraphNodeType(path[qr]) << std::endl;
   // }
   // std::cout << std::endl; 
    //graphVertex[path[0]] = start;
    yices_context ctx = yices_mk_context();
bool noadd = false;
int jjf = 0;
paths.push_back(path);
std::vector<SgNode*> called;
    while (jjf != paths.size()) {
        //std::cout << "propagating" << std::endl;
        std::vector<SgGraphNode*> pathc = paths[jjf];
        int jj = 0;
        while (jj != pathc.size()) {
            if (isSgFunctionCallExp(pathc[jj]->get_SgNode()) && calledMap[pathc].find(pathc[jj]->get_SgNode()) == calledMap[pathc].end()) { //find(called.begin(), called.end(), pathc[jj]->get_SgNode()) == called.end()) {
             SgFunctionDeclaration* sgfd = isSgFunctionCallExp(pathc[jj]->get_SgNode())->getAssociatedFunctionDeclaration();
            SgName nam = sgfd->get_qualified_name();
          //  std::cout << "function: " << nam.getString() << std::endl;
            ROSE_ASSERT(sgfd != NULL);
            SgFunctionDefinition* sgfdef = sgfd->get_definition();
            //ROSE_ASSERT(sgfdef != NULL);

                if (sgfdef != NULL) {
                propagateFunctionCall(pathc, jj, jjf);
                
                //called.push_back(pathc[jj]->get_SgNode());
               //jjf = 0;
                noadd = true;
               //jj = 0;
                break;
                }
                else {
                std::cout << "ufunc: " << nam.getString() << std::endl;
                unknownFunctions.insert(pathc[jj]->get_SgNode());
                jj++;
                }
            }
            else {
                jj++;
            }
       }
       if (noadd) {
       noadd = false;
       }

       else {
       jjf++;
       }
       }

    //std::cout << "paths.size(): " << paths.size() << std::endl;
    //ROSE_ASSERT(false); 
    pathnumber += paths.size();
    std::vector<SgNode*> ncalled;

for (int q = 0; q < paths.size(); q++) {
   stringstream y;
   y << "yices" << q << ".txt";
  
  //yices_enable_log_file((char*) y.str().c_str());

  nameOf.clear();
  getExpr.clear();
  std::cout << "q=" << q << std::endl;
  //std::cout << "path: " << std::endl;
  //for (int q1 = 0; q1 < paths[q].size(); q1++) {
  //    std::cout << getGraphNodeType(paths[q][q1]);
 // }
 // std::cout << "endpath" << std::endl;
 // std::cout << "evalFunction" << std::endl;
  std::vector<SgGraphNode*> path = paths[q];
   //yices_reset(ctx);
    ROSE_ASSERT(path.front()->get_SgNode() == path.back()->get_SgNode());
    yices_context ctx = yices_mk_context();
    yices_expr ye = evalFunction(path, ctx, true);
    nameOf.clear();
    getExpr.clear();
}
}

bool inconsistent;

yices_expr evalFunction(std::vector<SgGraphNode*> path, yices_context& ctx, bool mainFlag) {
    int i = 0;
    bool noAssert = false;
    int rounds = 0;
    bool forFlag = false;
    int defscount = 0;
    bool inconsistent = false;
    std::vector<SgGraphNode*> exprPath;
   /* if (!mainFlag && isSgFunctionCallExp(path.front()->get_SgNode())) {
        path.pop_back();
        int qq = 0;
        while (defscount < 2) {
        while (path[qq]->get_SgNode() != path[0]->get_SgNode()) {
           qq++;
        }
        defscount++;
        }
        std::vector<SgGraphNode*> npath;
        for (int qw = qq; qw < path.size(); qw++) {
            npath.push_back(path[qw]);
        }
        path = npath;
       
    }*/
/*
    if (unknownFunctions.find(isSgFunctionCallExp(path[0]->get_SgNode())) != unknownFunctions.end()) {
        SgFunctionDeclaration* afd = (isSgFunctionCallExp(path[0]->get_SgNode()))->getAssociatedFunctionDeclaration();
        SgType* ty = afd.get_orig_return_type();
        string ty_str = getType(ty);
        SgInitializedNamePtrList* sipl = afd->get_args();
        yices_type dom[sipl->size()]
        int iic = 0;;
        for (SgInitializedNamePtrList::iterator ii = sipl.begin(); ii != sipl.end(); ii++) {
            dom[iic] = (*ii)->get_type();
            
            iic++;
        }
        int ds = iic;
        yices_type fty = yices_mk_function_type(ctx, dom, ds, ty);
        yices_var_decl ftydecl = (ctx, afd->get_qualified_name()->getString(),fty);
        yices_expr f = yices_mk_var_from_decl(ctx, ftydecl);




    }
*/
    while (i < path.size()) {
        //std::cout << "in evalFunction" << std::endl;
        //std::cout << "ith node: " << getGraphNodeType(path[i]) << std::endl;
/*
        if (inconsistent) {
            if (!mainFlag) {
                yices_expr wrong;
                return wrong;
            }
            else {
                inconsistent = false;
                yices_expr zrong;
                return zrong;
            }
        }*/
        if (yices_inconsistent(ctx)) {
            inconsistent = true;
            std::cout << "inconsistent" << std::endl;
            if (mainFlag) {
            ipaths++;
            }
            yices_expr ywrong = yices_mk_fresh_bool_var(ctx);
            return ywrong;
        }
        //std::cout << "i: " << i << std::endl;
        exprPath.clear(); 
        //VertexID2 v1;
        //VertexID2 v2;
        //std::cout << "in while" << std::endl;
        if (isSgReturnStmt(path[i]->get_SgNode())) {
          // std::cout << "retstmt" << std::endl;
          // std::cout << "i: " << i << ", path.size(): " << path.size() << std::endl;
           std::vector<SgGraphNode*> retpath;
           //retpath.push_back(path[i]);
            
           int j = i+1;
           while (path[j]->get_SgNode() != path[i]->get_SgNode()) {
              retpath.push_back(path[j]);
            //  std::cout << "path[j]: " << getGraphNodeType(path[j]) << std::endl;
              j++;
              if (j == path.size()) {
                  break;
              }
           }
           //retpath.push_back(path[j]);
           yices_expr retparse = mainParse(retpath, ctx);
           if (!mainFlag) {
               return retparse;
               //yices_assert(retparse, ctx);
           }
           i += retpath.size()+2;
        }
        if (isSgInitializedName(path[i]->get_SgNode()) /*&& knownNodes.find(path[i]->get_SgNode()) == knownNodes.end()*/) {
           // exprs.push_back(path[i]);
            exprPath.clear();
            exprPath.push_back(path[i]);
            if (path[i]->get_SgNode()->cfgIndexForEnd() == 0) {
                    
                    SgName svs = (isSgInitializedName(exprPath[0]->get_SgNode()))->get_qualified_name();
                    if (nameOf.find(svs) == nameOf.end()) {
                    SgType* typ = (isSgInitializedName(exprPath[0]->get_SgNode()))->get_type();
                    string typ_str = getType(typ);
                    stringstream funN;
                    //funN << "V" << nvars;
                    nvars++; 
                    funN << svs.getString() << nvars;
                    char* fun = (char*) funN.str().c_str();
                    char* valTypeCh = (char*) typ_str.c_str();
                    yices_type ty = yices_mk_type(ctx, valTypeCh);
                    yices_var_decl vdecl = yices_mk_var_decl(ctx, fun, ty);
                    yices_expr e1 = yices_mk_var_from_decl(ctx, vdecl);
                    getExpr[svs] = e1;
                    nameOf[svs] = funN.str();
                    }
                    i++; 
            }
            else { 
            unsigned int k = i+1;
            //while (k < path.size() && (!isSgInitializedName(path[k]->get_SgNode()) || path[k]->get_SgNode() != path[i]->get_SgNode())) {
            //    exprPath.push_back(path[k]);
            //    k++;
           // }
           // exprPath.push_back(path[i]);
            int check = 0;
            while (check < path[i]->get_SgNode()->cfgIndexForEnd()) {//path[k]->get_SgNode() != path[i]->get_SgNode()) {
                if (path[i]->get_SgNode() == path[k]->get_SgNode()) {
                    check++;
                    if (check == path[i]->get_SgNode()->cfgIndexForEnd()) {
                        break;
                    }
                }
                exprPath.push_back(path[k]);
                k++;
            }
            exprPath.push_back(path[k]);
           // std::cout << "EXPRPATH: " << std::endl;
           // for (int oo = 0; oo < exprPath.size(); oo++) {
           //     std::cout << getGraphNodeType(exprPath[oo]) << std::endl;
           // }
            //std::cout << std::endl;
            //SE_ASSERT(y1 != NULL);
            std::cout << "exprPath.size(): " << exprPath.size() << std::endl; 
            yices_expr y1 = mainParse(exprPath, ctx);
            ROSE_ASSERT(y1 != NULL);
            if (!unknown_flag) {
            //if (y1 != NULL) {
            
            yices_assert(ctx, y1);
            //}
            }
            else {
                unknown_flag = false;
            }
            i += exprPath.size()+1;
            exprPath.clear();
            k = 0;
        }
        }
        else if (isSgForStatement(path[i]->get_SgNode()) && isSgForInitStatement(path[i+1]->get_SgNode())) {
            forFlag = true;
            std::vector<SgGraphNode*> vec1;
            unsigned int j = i+2;
            int w = 2;
            while (!isSgInitializedName(path[j]->get_SgNode()) && !isSgAssignOp(path[j]->get_SgNode())) {
                j++;
                w++;
            }
            vec1.push_back(path[j]);
            int k = j+1;
            while (k < path.size() && !isSgInitializedName(path[k]->get_SgNode())) {
                vec1.push_back(path[k]);
                k++;
            }
            vec1.push_back(path[k]);
            int q = 0;
            while (k < path.size() && !isSgForInitStatement(path[k]->get_SgNode())) {
                q++;
                k++;
            }
            yices_expr y1 = mainParse(vec1, ctx);
            yices_assert(ctx, y1);
            forFlag = false;
            i += vec1.size() + w + q;           
        } 
        else if (isSgIfStmt(path[i]->get_SgNode())) {
            //std::cout << "ifstmt" << std::endl;
            //for (int wq = i; wq < path.size(); wq++) {
            //std::cout << getGraphNodeType(path[wq]) << std::endl;
           // }
           // std::cout << "endifstmt" << std::endl;
          //  std::cout << "next node: " << getGraphNodeType(path[i+1]) << std::endl; 
            while (isSgBasicBlock(path[i+1]->get_SgNode())) {
                i++;
            }
            if (!isSgExprStatement(path[i+1]->get_SgNode())) {
                i++;
            }
            else {
            ROSE_ASSERT(isSgExprStatement(path[i+1]->get_SgNode()));
            int k = i+2;
            std::vector<SgGraphNode*> fpath;
            while (path[k]->get_SgNode() != path[i+1]->get_SgNode()) {
                if (!isSgBasicBlock(path[k]->get_SgNode())) {
                fpath.push_back(path[k]);
                }
                k++;
            }
            //fpath.push_back(path[k]);
            ROSE_ASSERT(isSgExprStatement(path[k]->get_SgNode()));;
           // std::cout << "fpath: " << std::endl;
           // for (int xx = 0; xx < fpath.size(); xx++) {
           //     std::cout << getGraphNodeType(fpath[xx]) << std::endl;
           // }
            //std::cout << "endfpath" << std::endl;
            yices_expr y1 = mainParse(fpath, ctx);
            int kk = k+1;
            ROSE_ASSERT(isSgIfStmt(path[kk]->get_SgNode()));
            std::set<SgDirectedGraphEdge*> ifoeds = openg->computeEdgeSetOut(path[kk]);
            CFGNode cn = opencfg->toCFGNode(path[kk+1]);
            std::vector<CFGEdge> ed = cn.inEdges();
            int qw = 0;
            while (isSgBasicBlock(path[kk+1]->get_SgNode())) {
                kk++;
           }
            std::cout << "currnode: " << getGraphNodeType(path[kk+1]) << std::endl;
            for (int q2 = 0; q2 < path.size(); q2++) {
            while (ed[qw].source() != opencfg->toCFGNode(path[q2])) {
                qw++;
                if (qw >= ed.size()) {
                    break;
                }
            }
            qw = 0;
            }
            CFGEdge needEdge = ed[qw];
            if (unknown_flag) {
                unknown_flag = false;
                yices_expr uf;
                return uf;
            }
            EdgeConditionKind kn = needEdge.condition();
            if (kn == eckTrue) {
                yices_assert(ctx, y1);
            }
            else {
                yices_expr ynot = yices_mk_not(ctx,y1);
                yices_assert(ctx,ynot);
            }
            int kk2 = kk+1;
            while (isSgBasicBlock(path[kk2]->get_SgNode())) {
                kk2++;
            }//int kk2 = kk+1;
            //std::cout << "kk: " << getGraphNodeType(path[kk]) << std::endl;
            //std::cout << "kk2: " << getGraphNodeType(path[kk2]) << std::endl;
            //std::cout << "path.back(): " << getGraphNodeType(path.back()) << std::endl;
            std::vector<SgGraphNode*> fpath2;
            while (path[kk2]->get_SgNode() != path[kk]->get_SgNode() && kk2 < path.size()) {
                if (!isSgBasicBlock(path[kk2]->get_SgNode())) {
                fpath2.push_back(path[kk2]);
                }
                kk2++;
                if (kk2 == path.size()) {
                    break;
                }
                
            }
            if (kk2 == path.size()) {
                i = kk+1;
            }
            else {
            int kk3 = kk2+1;
           // while (path[kk3]->get_SgNode() != path[kk2]->get_SgNode()) {
           //    kk3++;
           // }
            if (fpath2.size() != 0) {
            //std::cout << "fpath2" << std::endl;
            //for (int qp = 0; qp < fpath2.size(); qp++) {
            //    std::cout << getGraphNodeType(fpath2[qp]) << std::endl;
            //}
            //std::cout << "fpath2end" << std::endl;
            if (isSgExprStatement(fpath2[0]->get_SgNode())) {
                std::vector<SgGraphNode*> fpathnew;
              //  ROSE_ASSERT(isSgExprStatement(fpath2.back()->get_SgNode()));
                for (int qq = 1; qq < fpath2.size()-1; qq++) {
                    fpathnew.push_back(fpath2[qq]);
                }
                fpath2 = fpathnew;
            }
            //if (fpath2.size() != 0) {
            SgGraphNode* pathj = fpath2.front();
            if (isSgIfStmt(pathj->get_SgNode()) || isSgForStatement(pathj->get_SgNode()) || isSgInitializedName(pathj->get_SgNode()) /*|| isSgReturnStmt(pathj->get_SgNode())*/) {
            yices_expr y3 = evalFunction(fpath2, ctx, false);
            }
            else if (isSgReturnStmt(pathj->get_SgNode())) {
            yices_expr y3 = evalFunction(fpath2, ctx, false);
           return y3;
            }
            else {
            //std::cout << "fpath2: " << std::endl;
            //for (int wq = 0; wq < fpath2.size(); wq++) {
            //    std::cout << getGraphNodeType(fpath2[wq]) << std::endl;
          ///  }
          //  std::cout << "\n\n" << std::endl;
            yices_expr y3 = mainParse(fpath2, ctx);
            yices_assert(ctx, y3);
            }
            }
            //else {
            //    ROSE_ASSERT(false);
           // }
            
            i = kk2;
            }
       }
       }        
        else if (isSgExprStatement(path[i]->get_SgNode()) /*&& knownNodes.find(path[i]->get_SgNode()) == knownNodes.end()*/) {
            i = i+1;
        }
/*
            unsigned int j = i+1;
            while (path[j]->get_SgNode() != path[i]->get_SgNode()) {
                exprPath.push_back(path[j]);
                j++;
            }
            if (isSgIfStmt(path[j]->get_SgNode()) || isSgForStatement(path[j]->get_SgNode()) || isSgInitializedName(path[j]->get_SgNode()) || isSgReturnStmt(path[j]->get_SgNode())) {
                i = j;
            }
            else {
            //    std::cout << "Exprpath: " << std::endl;
            //    for (int qq = 0; qq < exprPath.size(); qq++) {
              //      std::cout << getGraphNodeType(exprPath[qq]) << std::endl;
              //  }
                //std::cout << std::endl;
              //  if (!unknown_flag) {
                yices_expr y1 = mainParse(exprPath, ctx);
                if (!unknown_flag) {
                yices_assert(ctx,y1);
                }
                else {
                unknown_flag = false;
                }
                i += exprPath.size()+2;
            }
       }
*/
        
/*        
            ROSE_ASSERT(j < path.size());
            yices_expr y2 =  mainParse(exprPath, ctx);
            ROSE_ASSERT(y2 != NULL);
            //std::cout << "exprPath.size(): " << exprPath.size() << std::endl; 
            std::set<SgDirectedGraphEdge*> oeds = openg->computeEdgeSetOut(path[j]);
            ROSE_ASSERT(oeds.size() == 1);
            SgGraphNode* onn = (*(oeds.begin()))->get_to();
               
            
            ROSE_ASSERT(onn == path[j+1]);
            std::set<SgDirectedGraphEdge*> ifoeds = openg->computeEdgeSetOut(path[j+1]); 
            if ((isSgForStatement(onn->get_SgNode()) || (isSgIfStmt(onn->get_SgNode())) && ifoeds.size() >= 2)) { 
                //std::cout << "got a for or if" << std::endl;

                CFGNode cn = opencfg->toCFGNode(path[j+2]);
                std::vector<CFGEdge> ed = cn.inEdges();
                //ROSE_ASSERT(ed.size() == 1);
                int qw = 0;
                while (ed[qw].source() != opencfg->toCFGNode(path[j+1])) {
                    qw++;
                } 
                CFGEdge needEdge =  ed[qw];
                EdgeConditionKind kn = needEdge.condition();
                ROSE_ASSERT(kn == eckTrue || kn == eckFalse);
                if (kn == eckFalse) {
                    yices_expr y2n = yices_mk_not(ctx, y2);

                        yices_assert(ctx, y2n);
                }
                else {
                    ROSE_ASSERT(kn == eckTrue);
                    //std::cout << "got a eckTrue" << std::endl;
                    if (isSgForStatement(onn->get_SgNode())) {
                   //     int yr = yices_assert_retractable(ctx, y2);
                   //     forsts[onn->get_SgNode()] = yr;
                    }
                    else {
                        yices_assert(ctx, y2);
                    }
                }
            }
            else {      
                yices_assert(ctx, y2);
            }
            i += exprPath.size()+2;
            j = 0;

 
      }
*/
        else {
            i++;
        }
    
    }
    //if (yices_inconsistent(ctx)) {
     //   std::cout << "inconsistent path: " << ipaths << std::endl;
     //   ipaths++;
     //   inconsistent = false;
        
   // }
    if (mainFlag) {
    yices_del_context(ctx);
    }
}

StaticCFG::CFG* cfg;


std::vector<int> breakTriple(std::vector<SgGraphNode*> expr) {
    SgNode* index = expr[0]->get_SgNode();
    std::vector<int> bounds(3, 0);
    bounds[0] = 0;
    int i = 1;
    while (expr[i]->get_SgNode() != index) {
        //std::cout << "expr[i]: " << cfg->toCFGNode(expr[i]).toString() << std::endl;
        ROSE_ASSERT(i < expr.size());
        i++;
    }
    bounds[1] = i;
    bounds[2] = expr.size()-1;

    return bounds;
    }
   


//string mainParse(vector<SgGraphNode*> expr);
string isAtom(SgNode*);
bool isLogicalSplit(SgNode*);
string getLogicalSplit(SgNode*);
string getBinaryLogicOp(SgNode*);
bool isBinaryLogicOp(SgNode*);
bool isBinaryOp(SgNode*);
string getBinaryOp(SgNode*);


std::vector<SgGraphNode*> getSlice(std::vector<SgGraphNode*> vv, int i) {
    int cfgEnd = vv[i]->get_SgNode()->cfgIndexForEnd();
    int ind = 0;
    std::vector<SgGraphNode*> slice;
    if (cfgEnd == 0) {
        //std::cout << "nullend" << std::endl;
        slice.push_back(vv[i]);
        return slice;
    }
    slice.push_back(vv[i]);
    int k = i+1;
    while (true) {
        ROSE_ASSERT(cfgEnd != ind);
        //ROSE_ASSERT(k < vv.size());
        if (vv[i]->get_SgNode() == vv[k]->get_SgNode()) {
            ind++;
            if (ind == cfgEnd) {
                slice.push_back(vv[k]);
                return slice;
            }
         }
         slice.push_back(vv[k]);
         k++;
    }
}

//yices_expr evalFunction(vector<SgGraphNode*> funcLine, yices_context& ctx) {
    
string getGraphNodeType(SgGraphNode* sn) {
                   CFGNode cf = opencfg->toCFGNode(sn);
                     string str = cf.toString();
                     return str;
}



yices_expr mainParse(vector<SgGraphNode*> expr, yices_context& ctx) {
    //std::cout << "rounds" << rounds << std::endl;
    string typ = getGraphNodeType(expr[0]);
    std::cout << "nodetype: " << typ << std::endl;
    std::cout << "mainParse" << std::endl;
     rounds++;
    bool yices = true;
    std::stringstream stst;
    string parsed;
    //bool unknown_flag = false;
    std::vector<SgGraphNode*> vec1;
    std::vector<SgGraphNode*> vec2;
    stringstream ss;
    yices_expr ret;
    if (expr.size() == 0) {
        yices_expr empty = new yices_expr;
        return empty;
    }
    if (unknown_flag) {
        yices_expr empty;
         return empty;
    }
    else if (isSgReturnStmt(expr[0]->get_SgNode())) {
        ROSE_ASSERT(isSgReturnStmt(expr.back()));
        std::vector<SgGraphNode*> toSolve;
        for (int j = 1; j < expr.size()-1; j++) {
		    toSolve.push_back(expr[j]);
        }
        yices_expr ts = mainParse(toSolve, ctx);
        return ts;
    }
    else if (isSgFunctionCallExp(expr[0]->get_SgNode())) {
        //yices_type ty;
        yices_type fty;
        yices_var_decl ftydecl;
        yices_expr f;
        bool ufunc = false;
        if (unknownFunctions.find(expr[0]->get_SgNode()) != unknownFunctions.end()) {
           //yices_expr unknown;;
           //return unknown;
        
        SgFunctionDeclaration* afd = (isSgFunctionCallExp(expr[0]->get_SgNode()))->getAssociatedFunctionDeclaration();
        SgType* ty = afd->get_orig_return_type();
        string ty_str = getType(ty);
        const char* ty_const_char = ty_str.c_str();
        yices_type rty = yices_mk_type(ctx,(char*)ty_const_char);
        SgInitializedNamePtrList sipl = afd->get_args();
        yices_type dom[sipl.size()];
        int iic = 0;;
        for (SgInitializedNamePtrList::iterator ii = sipl.begin(); ii != sipl.end(); ii++) {
            string domY = getType((*ii)->get_type());
            yices_type typdom = yices_mk_type(ctx,(char*)domY.c_str());
            dom[iic] = typdom;

            iic++;
        }
        int ds = iic;
        fty = yices_mk_function_type(ctx, dom, ds, rty);;
        ftydecl = yices_mk_var_decl(ctx, (char*) afd->get_qualified_name().getString().c_str(),fty);
        f = yices_mk_var_from_decl(ctx, ftydecl);

 //yices_type fty = yices_mk_function_type(ctx, domain, 1, ty);
 // yices_var_decl fdecl = yices_mk_var_decl(ctx, "f", fty);

           ufunc = true;
        }
       int i = 1;
       while (!isSgExprListExp(expr[i]->get_SgNode())) {
           i++;
           if (i > expr.size()) {
               ROSE_ASSERT(false);
               yices_expr empty = new yices_expr;
               return empty;
           }
       }
       int j = i+1;
       int checks = 0;
       std::vector<yices_expr> argsyices;
       std::vector<SgGraphNode*> yexp;
       while (checks != expr[i]->get_SgNode()->cfgIndexForEnd()) {
       //std::vector<SgGraphNode*> yexp;
       while (expr[j]->get_SgNode() != expr[i]->get_SgNode()) {
           yexp.push_back(expr[j]);
           j++;
           if (j >= expr.size()) {
               ROSE_ASSERT(false);
               yices_expr empty = new yices_expr;
               return empty;
           }
      }
      j++;
      //yices_expr argsaryices[argsyices.size()];
      //std::cout << "yexp: " << std::endl;
      //for (int qy = 0; qy < yexp.size(); qy++) {
      //    std::cout << getGraphNodeType(yexp[qy]) << std::endl;
     // }
      //ROSE_ASSERT(false);
     // std::cout << std::endl;
      yices_expr yex = mainParse(yexp,ctx);
      //yices_assert(ctx, yex);
      ROSE_ASSERT(yex != NULL);
      argsyices.push_back(yex);
      checks++;
      }
      if (ufunc) {
          //yices_expr argsaryyices[argsyices.size()];
       //   std::cout << "ufunc" << std::endl;
          yices_expr argsaryices[argsyices.size()];
          for (int tt = 0; tt < argsyices.size(); tt++) {
          argsaryices[tt] = argsyices[tt];
          }
       
          yices_expr app = yices_mk_app(ctx,f,argsaryices,argsyices.size());
          return app;
      }
      SgFunctionDeclaration* sgfd = isSgFunctionCallExp(expr[0]->get_SgNode())->getAssociatedFunctionDeclaration();
      SgFunctionParameterList* sfpl = sgfd->get_parameterList();
      SgInitializedNamePtrList sinp = sfpl->get_args();
      SgInitializedNamePtrList::iterator ite = sinp.begin();
      int argnum = 0;
      for (ite = sinp.begin(); ite != sinp.end(); ite++) {
          SgName svs = (isSgInitializedName((*ite)))->get_qualified_name();
          stringstream funN;
          nvars++;
          funN << svs.getString() << nvars;
          //funN << "V" << nvars;
          nameOf[svs] = funN.str();//funN.str();
          string valType = getType(isSgInitializedName(*ite)->get_type());
            yices_type ty1 = yices_mk_type(ctx, (char*) valType.c_str());
            yices_var_decl decl1 = yices_mk_var_decl(ctx, (char*) funN.str().c_str(), ty1);
            yices_expr e1 = yices_mk_var_from_decl(ctx, decl1);
            //yices_expr e2 = mainParse(vec2, ctx);

          //if (isSgVarRefExp(yexp[0]->get_SgNode())) {
          //getExpr[svs] = getExpr[isSgVarRefExp(yexp[0]->get_SgNode())->get_symbol()->get_name()];//argsyices[argnum];
         // }
         // else {
          ROSE_ASSERT(argsyices[argnum] != NULL); 
          getExpr[svs] = argsyices[argnum];
          
          yices_expr exp = yices_mk_eq(ctx,e1,argsyices[argnum]);
          yices_assert(ctx,exp);
         
        //      ROSE_ASSERT(false);
          //}
          argnum++;
       }
       //std::cout << "argnum: " << argnum << std::endl;
       //std::cout << std::endl;
       //for (int ww = 0; ww < expr.size(); ww++) {
       //    std::cout << getGraphNodeType(expr[ww]) << std::endl;
      // }
      // std::cout << std::endl;
       ROSE_ASSERT(isSgFunctionCallExp(expr[j]->get_SgNode()));
      // std::cout << "graphnodeafterexp: " << getGraphNodeType(expr[j+1]) << std::endl;
       //ROSE_ASSERT(isSgFunctionCallExp(expr[j+1]->get_SgNode()));
       int k = j;
       //j+=2;
       //int k = j-2;
       std::vector<SgGraphNode*> funcLine;
       int check2 = 2;
      // std::cout << "k-1: " << getGraphNodeType(expr[k]) << std::endl;
       j++;
       //std::cout << "funcLinePath: " << std::endl;
       //for (int qt = 0; qt < expr.size(); qt++) {
       //    std::cout << getGraphNodeType(expr[qt]) << std::endl;
      // }
       //std::cout << "endpath" << std::endl;
       //std::cout << std::endl;
       while (check2 < expr[k]->get_SgNode()->cfgIndexForEnd()) {
       if (expr[k]->get_SgNode() == expr[j]->get_SgNode()) {
           check2++;
           //if (check2 >= expr[k]->get_SgNode()->cfgIndexForEnd()) {
               break;
           //}
           //check2++;
        //   funcLine.push_back(expr[j]);
         //  j++;
       }
       //check2++;
       funcLine.push_back(expr[j]);
       j++;
       }
       //std::cout << "formed funcLine" << std::endl;
       std::vector<SgGraphNode*> funcLine2;
       for (int kk = 1; kk < funcLine.size()-1; kk++) {
           funcLine2.push_back(funcLine[kk]);
       }
       //std::cout << std::endl;
       std::cout << "funcLine2 size: " << funcLine2.size() << std::endl;
       for (int ww2 = 0; ww2 < funcLine2.size(); ww2++) {
           std::cout << getGraphNodeType(funcLine2[ww2]) << std::endl;
       }
       std::cout << std::endl;
       ROSE_ASSERT(!unknown_flag);
        
       yices_expr funcexp = evalFunction(funcLine2, ctx, false); 
       ROSE_ASSERT(funcexp != NULL);
       return funcexp;   


 
    }
    else if (isSgNotOp(expr[0]->get_SgNode())) {
        ret = yices_mk_fresh_bool_var(ctx);
        int i = 1;
        SgGraphNode* curr = expr[1];
        while (curr->get_SgNode() != expr[0]->get_SgNode()) {
            vec1.push_back(curr);
            i++;
            curr = expr[i];
        }
        yices_expr e1 = yices_mk_fresh_bool_var(ctx);
        e1 = mainParse(vec1, ctx);
        ret = yices_mk_not(ctx, e1);
        return ret;
    }     
    else if (isLogicalSplit(expr[0]->get_SgNode())) {
        ret = yices_mk_fresh_bool_var(ctx);
        string ls = getLogicalSplit(expr[0]->get_SgNode());
       // std::vector<int> bounds = breakTriple(expr);
        std::map<int, std::vector<SgGraphNode*> > vec;
        std::vector<SgGraphNode*> vecX;
        int qt = 1;
        int curr = 0;
        //std::cout << "expr logical split: " << std::endl;
        //for (int qy = 0; qy < expr.size(); qy++) {
         //   std::cout << getGraphNodeType(expr[qy]) << std::endl;
       // }
       // std::cout << std::endl;
        while (curr < 2) {
            if (expr[qt]->get_SgNode() == expr[0]->get_SgNode()) {
                vec[curr] = vecX;
                vecX.clear();
                curr++;
            }
            else {
            vecX.push_back(expr[qt]);
            }
            qt++;
        }
        vec1 = vec[0];
        vec2 = vec[1];
/*
        for (int i = bounds[0]+1; i < bounds[1]; i++) {
            vec1.push_back(expr[i]);
        }
        for (int j = bounds[1]+1; j < bounds[2]; j++) {
            vec2.push_back(expr[j]);
        }
*/
        //if (yices) {
                yices_expr e1 = yices_mk_fresh_bool_var(ctx);
                yices_expr e2 = yices_mk_fresh_bool_var(ctx);
            //    std::cout << "vec1: " << std::endl;
                //for (int qw = 0; qw < vec1.size(); qw++) {
                //    std::cout << getGraphNodeType(vec1[qw]) << std::endl;
               // }
              //  std::cout << "vec2: " << std::endl;
               // for (int qw2 = 0; qw2 < vec2.size(); qw2++) {
                //    std::cout << getGraphNodeType(vec2[qw2]) << std::endl;
               // }
              //  std::cout << "\n\n";
                e1 = mainParse(vec1, ctx);
                if (vec2.size() != 0) {
                e2 = mainParse(vec2, ctx);
                } 
                //std::cout << "vec1.size()" << vec1.size() << " vec2.size(): " << vec2.size() << std::endl;
                
                if (vec1.size() == 0 && ls == "and") { 
                    e1 = yices_mk_false(ctx);
                }
                else if (vec1.size() == 0 && ls == "or") {
                    e1 = yices_mk_true(ctx);
                }
                else if (vec2.size() == 0 && ls == "and") {
                    e2 = yices_mk_false(ctx);
                }
                else if (vec2.size() == 0 && ls == "or") {
                    e2 = yices_mk_true(ctx);
                }
               
                yices_expr arr[2];
                arr[0] = e1;
                arr[1] = e2;
               // yices_expr ret = yices_mk_fresh_bool_var(ctx);
            if (ls == "or") {
                ret = yices_mk_or(ctx, arr, 2);
            }
            else if (ls == "and") {
                ret = yices_mk_and(ctx, arr, 2);
            }
            else {
                //std::cout << "bad logical command" << std::endl;
                ROSE_ASSERT(false);
            }
            //yices_assert(ctx, ret);
            return ret;    
        //}
        //stst << "( "<< ls << " " << mainParse(vec1) << " " << mainParse(vec2) << ")";
        //parsed = stst.str();
        //stst << "and " << mainParse(vec2) << ")\n";
        
    }
    else if (isBinaryLogicOp(expr[0]->get_SgNode()) || isBinaryOp(expr[0]->get_SgNode())) {
        //std::vector<int> bounds = breakTriple(expr);
        int i = 1;
        int check = 0;
       // std::cout << "binarylogic vec: " << std::endl;
       // for (int ws = 0; ws < expr.size(); ws++) {
       //     std::cout << getGraphNodeType(expr[ws]) << std::endl;
       // }
       // std::cout << "endlogic" << std::endl;
       // std::cout << "\n";
        std::vector<SgGraphNode*> vecX;
        std::map<int, std::vector<SgGraphNode*> > vec;
        while (check < expr[0]->get_SgNode()->cfgIndexForEnd()) {
            if (expr[0]->get_SgNode() == expr[i]->get_SgNode()) { 
                vec[check] = vecX;
                vecX.clear();
                check++;
                if (check == expr[0]->get_SgNode()->cfgIndexForEnd()) {
                    break;
                }
             }
             if (expr[0]->get_SgNode() != expr[i]->get_SgNode()) {
             vecX.push_back(expr[i]);
             }
             i++;
        }
        vec1 = vec[0];
        vec2 = vec[1];
/*
        for (int i = bounds[0]+1; i < bounds[1]; i++) {
            vec1.push_back(expr[i]);
        }
        for (int j = bounds[1]+1; j < bounds[2]; j++) {
            vec2.push_back(expr[j]);
        }
*/
        if (isBinaryLogicOp(expr[0]->get_SgNode())) {
            parsed = getBinaryLogicOp(expr[0]->get_SgNode());
        }
        else {
            parsed = getBinaryOp(expr[0]->get_SgNode());
        }
        //yices_expr ret;
        if (isBinaryLogicOp(expr[0]->get_SgNode())) {
            ret = yices_mk_fresh_bool_var(ctx);
            yices_expr e1 = mainParse(vec1, ctx);
            yices_expr e2 = mainParse(vec2, ctx);
            if (parsed == ">") {
                ret = yices_mk_gt(ctx,e1, e2); 
            }
            else if (parsed == "<") {
                ret = yices_mk_lt(ctx, e1, e2);
            }
            else if (parsed == "=") {
                ret = yices_mk_eq(ctx, e1, e2);
            }
            else if (parsed == "!=") {
                ret = yices_mk_diseq(ctx, e1, e2);
            }
            else if (parsed == "<=") {
                ret = yices_mk_le(ctx, e1, e2);
            }
            else if (parsed == ">=") {
                ret = yices_mk_ge(ctx, e1, e2);
            }
            else {
                //std::cout << "unknown binary logic op" << std::endl;
                return ret;
            }
            //std::cout << "parsed: " << parsed << std::endl;
            ROSE_ASSERT(ret != NULL);
            //yices_assert(ctx, ret);
            return ret;
        }
         //   stst << "( " <<  parsed << " " << mainParse(vec1) << " " << mainParse(vec2) << ")";
        else {
            yices_expr e1 = mainParse(vec1, ctx);
            yices_expr e2 = mainParse(vec2, ctx);
            yices_expr yicesarr[2];
            yicesarr[0] = e1;
            yicesarr[1] = e2; 
            string bop = getBinaryOp(expr[0]->get_SgNode());
            if (bop == "+") {
                
                ret = yices_mk_sum(ctx, yicesarr, 2);
            }
            else if (bop == "-") {
                ret = yices_mk_sub(ctx, yicesarr, 2);
            }
            else if (bop == "*") {
                ret = yices_mk_mul(ctx, yicesarr, 2);
            }
            //else if (bop == "/") {
            //    ret = yices_mk_div(ctx, e1, e2);
           // }
            else {
                //std::cout << "bad binary op: " << bop << endl;
                ROSE_ASSERT(false);
            }
        return ret;
            //stst << "( " << parsed << " " << mainParse(vec1) << " " << mainParse(vec2) << " ) ";
        }
        //parsed = stst.str();
    }
    else if (isSgPlusPlusOp(expr[0]->get_SgNode())) {
        for (int i = 1; i < expr.size() - 1; i++) {
            vec1.push_back(expr[i]);
        }
        yices_expr e1 = mainParse(vec1, ctx);
        stringstream funN;
        funN << "V" << nvars;
        nvars++;
        char* fun = (char*) funN.str().c_str();
        yices_type ty = yices_mk_type(ctx, "int");
        yices_var_decl vdecl = yices_mk_var_decl(ctx, fun, ty);
        yices_expr e2 = yices_mk_var_from_decl(ctx, vdecl);
        yices_expr arr[2];
        yices_expr en = yices_mk_num(ctx, 1);
        arr[0] = e1;
        arr[1] = en; 
        ret = yices_mk_sum(ctx, arr, 2);
        return ret;
    }    
    else if (isAtom(expr[0]->get_SgNode()) != "") {
        string ty = isAtom(expr[0]->get_SgNode());
        if (ty == "int") {
            int ival = isSgIntVal(expr[0]->get_SgNode())->get_value();
            ret = yices_mk_num(ctx, ival);
            //std::cout << "ival: " <<  ival << std::endl;
            
            //parsed = ss.str();
        }
        else if (ty == "double") {
            double dval = isSgDoubleVal(expr[0]->get_SgNode())->get_value();
            //ss << dval;
            //parsed = ss.str();
            ret = yices_mk_num(ctx, dval);    
        }
        else if (ty == "float") {
           float fval = isSgFloatVal(expr[0]->get_SgNode())->get_value();
          // ss << fval;
          // parsed =  ss.str();
           ret = yices_mk_num(ctx, fval);
        }
        else if (ty == "short") {
           short sval = isSgShortVal(expr[0]->get_SgNode())->get_value();
           //ss << sval;
           //parsed =  ss.str();
           ret = yices_mk_num(ctx, sval);
        }
        else if (ty == "long") {
            long lval = isSgLongIntVal(expr[0]->get_SgNode())->get_value();
            //ss << lval;
            //parsed = ss.str();
            ret = yices_mk_num(ctx, lval);
        }
        else if (ty == "long long int") {
            long long llval = isSgLongLongIntVal(expr[0]->get_SgNode())->get_value();
            //ss << llval;
            //parsed = ss.str();
            ret = yices_mk_num(ctx, llval);
        }
        else if (ty == "long double") {
            long double lldval = isSgLongDoubleVal(expr[0]->get_SgNode())->get_value();
            //ss << lldval;
            //parsed =  ss.str();
            ret = yices_mk_num(ctx, lldval);
        }
        else if (ty == "bool") {
            bool bval = isSgBoolValExp(expr[0]->get_SgNode())->get_value();
            if (bval == true) {
                parsed = "true";
                ret = yices_mk_true(ctx);
            }
            else {
                parsed = "false";
                ret = yices_mk_false(ctx);
            }
        }
        else {
            //cout << "unsupported atomic type";
            ROSE_ASSERT(false);
        }
        return ret;
    }
    else if (isSgVarRefExp((expr[0])->get_SgNode())) {
        SgName svs = isSgVarRefExp(expr[0]->get_SgNode())->get_symbol()->get_declaration()->get_qualified_name();
        //stringstream ss;
        ROSE_ASSERT(nameOf.find(svs) != nameOf.end());
        //    parsed = nameOf[svs];
        //}
        //else {
        //    ss <<  "V" << nvars;
        //    nvars++;
        //    nameOf[svs] = ss.str();
        //    parsed = nameOf[svs];
        //}
        //std::cout << "nameOf[svs]: " << nameOf[svs] << std::endl;
        yices_expr e1 = getExpr[svs];
        ret = e1;
        return ret;
        
    }
    else if (isSgInitializedName(expr[0]->get_SgNode())) {
        stringstream stst;
        std::vector<SgGraphNode*> vec1;
        //ROSE_ASSERT(isAtom((expr[2])->get_SgNode()) != "");
      //  string valType = isAtom((expr[2])->get_SgNode());
        int p = 3;
         

              SgName svs = (isSgInitializedName(expr[0]->get_SgNode()))->get_qualified_name();
                    SgType* typ = (isSgInitializedName(expr[0]->get_SgNode()))->get_type();
                    string valType = getType(typ);
                    //stringstream funN;
/*
                    funN << "V" << nvars;
                    nvars++;
                    char* fun = (char*) funN.str().c_str();
                    char* valTypeCh = (char*) typ_str.c_str();
                    yices_type ty = yices_mk_type(ctx, valTypeCh);
                    yices_var_decl vdecl = yices_mk_var_decl(ctx, fun, ty);
                    yices_expr e1 = yices_mk_var_from_decl(ctx, vdecl);
                    getExpr[svs] = e1;
                    nameOf[svs] = fun;
*/



         // SgName svs = (isSgInitializedName(expr[0]->get_SgNode()))->get_qualified_name();

      //  if (isAtom(expr[2]) == "") {
//        SgName svs = (isSgInitializedName(expr[0]->get_SgNode()))->get_qualified_name();
        int check = 0;
        vec1.push_back(expr[2]);
        std::cout << "expr[2]: " << getGraphNodeType(expr[2]) << std::endl;
        while (/*expr[2]->get_SgNode() != expr[p]->get_SgNode()) { &&*/ check < expr[2]->get_SgNode()->cfgIndexForEnd()) {
            if (expr[2]->get_SgNode() == expr[p]->get_SgNode()) {
                check++;
                if (check >= expr[2]->get_SgNode()->cfgIndexForEnd()) {
                    break;
                }
            }
            vec1.push_back(expr[p]);
            p++;
            //vec1.push_back(expr[p]);
         //   p++;
        }
        vec1.push_back(expr[p]);
        stringstream funN;
        string ss;
        //if (nameOf.find(svs) != nameOf.end()) {
        //    ss = nameOf[svs];
        //}
        //else {
            nvars++;
            funN << svs.getString() << nvars;
            nameOf[svs] = funN.str();
            //ss = funN.str();
            //nvars++;  
            //stst << "(declare-fun " << ss << " () " << valType << ")\n";
        //}i
        char* fun = (char*) funN.str().c_str();//funN.str().c_str();
        //for (int i = 0; i < funN.str().size(); i++) {
        //    fun[i] = funN.str()[i];
        //}
        //std::cout << "fun: " << fun << std::endl;
        char* valTypeCh = (char*) valType.c_str();
	//for (int j = 0; j < valType.size(); j++) {
        //    valTypeCh[j] = valType[j];
        //}
        //std::cout << "valTypeCh: " << valTypeCh << std::endl;
        //std::cout << "fun" << fun << std::endl;
        //char* fun = (char*)(funN.str().c_str());
        yices_type ty = yices_mk_type(ctx, valTypeCh);
        yices_var_decl vdecl = yices_mk_var_decl(ctx, fun, ty);
        yices_expr e1 = yices_mk_var_from_decl(ctx, vdecl);
        //getType[e1] = valType;
        //getExpr[svs] = e1;
        std::cout << "vec1.size(): " << vec1.size() << std::endl;
        std::cout << "vec1[0]: " << getGraphNodeType(vec1[0]) << std::endl;

        yices_expr e2 = mainParse(vec1, ctx);
        getExpr[svs] = e2;
        ROSE_ASSERT(e2 != NULL);
        if (forFlag || unknown_flag) {
            unknown_flag = false;
            ret = yices_mk_eq(ctx, e1, e1);
        }
         else {
            ret = yices_mk_eq(ctx, e1, e2);
        }
        //yices_assert(ctx, ret); 
        //stst << "(let (" << ss << " " << mainParse(vec1) << ")\n";
        //parsed =  stst.str();
        
        return ret;
    }
    else if (isSgAssignOp(expr[0]->get_SgNode())) {
        stringstream stst;
        ROSE_ASSERT(isSgVarRefExp(expr[1]->get_SgNode()));
        //ROSE_ASSERT(isAtom(expr[3]->get_SgNode()) != "");
        //string valType = isAtom(expr[3]->get_SgNode());
        std::vector<int> bounds = breakTriple(expr);
        //for (int i = bounds[0]+1; i < bounds[1]; i++) {
        //    vec1.push_back(expr[i]);
        //}
        SgName svs = (isSgVarRefExp((expr[1]->get_SgNode()))->get_symbol()->get_declaration()->get_qualified_name());
        SgType* typ = isSgVarRefExp((expr[1]->get_SgNode()))->get_symbol()->get_declaration()->get_type();
        string valType = getType(typ);
        for (int j = bounds[1]+1; j < bounds[2]; j++) {
            vec2.push_back(expr[j]);
        }
        if (nameOf.find(svs) != nameOf.end()) {
            stringstream ss;
            nvars++;
            ss << svs.getString();
            ss << nvars;
            //nvars++;
            yices_type ty1 = yices_mk_type(ctx, (char*) valType.c_str());
            yices_var_decl decl1 = yices_mk_var_decl(ctx, (char*) ss.str().c_str(), ty1);
            yices_expr e1 = yices_mk_var_from_decl(ctx, decl1);
            yices_expr e2 = mainParse(vec2, ctx);
            if (forFlag || unknown_flag) {
                unknown_flag = false;
                ret = e1;
            }
            else {
                ret = yices_mk_eq(ctx, e1, e2);
            }
            nameOf[svs] = ss.str();
            getExpr[svs] = e2;
        }
        else {
            stringstream ss;
           // ss << "V";
           // ss << nvars;
            nvars++;
            ss << svs.getString();
            ss << nvars;
            char valTypeCh[valType.size()];
            for (int k = 0; k < valType.size(); k++) {
                valTypeCh[k] = valType[k];
            }
            char nam[ss.str().size()];
            for (int q = 0; q < ss.str().size(); q++) {
                nam[q] = ss.str()[q];
            }
            string fun = valType;
            //char* funC = fun.c_str();
            yices_type ty = yices_mk_type(ctx, (char*) valType.c_str());
	    yices_var_decl decl1 = yices_mk_var_decl(ctx,(char*) ss.str().c_str(), ty);
            yices_expr e1 = yices_mk_var_from_decl(ctx, decl1);
            yices_expr e2 = mainParse(vec2, ctx);
            if (forFlag) {
                ret = e1;
            }
            else {
                ret = yices_mk_eq(ctx, e1, e2);
            }
            getExpr[svs] = e1;
            nameOf[svs] = ss.str();//svs.getString();//fun;
        }
        //yices_assert(ctx, ret);
        //getExpr[svs] = e1;
        //stringstream stst;
        //noAssert = true;
        
       // stst << "(let (" << mainParse(vec1) << " " << mainParse(vec2) << ") )";
        //parsed = stst.str();
        return ret;
    }
    else {
        //cout << "unknown type" << endl;
        //cout << cfg->toCFGNode(expr[0]).toString() << std::endl;
        //ROSE_ASSERT(false);
        unknown_flag = true;
        yices_expr y1 = yices_mk_fresh_bool_var(ctx);
        return y1;
    }
    //std::cout << "parsed: " << parsed << std::endl;
    return ret;   
}

string isAtom(SgNode* n) {
    if (isSgIntVal(n)) {
        return "int";
    }
    else if (isSgDoubleVal(n)) {
        return "double";
    }
    else if (isSgFloatVal(n)) {
        return "float";
    }
    else if (isSgShortVal(n)) {
        return "short";
    }
    else if (isSgLongIntVal(n)) {
        return "long";
    }
    else if (isSgLongLongIntVal(n)) {
        return "long long int";
    }
    else if (isSgLongDoubleVal(n)) {
        return "long double";
    }
    else if (isSgBoolValExp(n)) {
        return "bool";
    }
    return "";
}

bool isLogicalSplit(SgNode* n) {
    if (isSgAndOp(n) || isSgOrOp(n) || isSgNotOp(n)) {
        return true;
    }
    return false;
}

std::string getLogicalSplit(SgNode* n) {
    if (isSgAndOp(n)) {
        return "and";
    }
    else if (isSgOrOp(n)) {
        return "or";
    }
    else if (isSgNotOp(n)) {
        return "not";
    }
    else {
       // cout << "not a logicalSplit Operator" << std::endl;
        ROSE_ASSERT(false);
    }
}

std::string getBinaryLogicOp(SgNode* n) {
    std::string ss;
    if (isSgEqualityOp(n)) {
        ss = "=";
    }
    else if (isSgLessThanOp(n)) {
        ss = "<";
    }
    else if (isSgGreaterThanOp(n)) {
        ss = ">";
    }
    else if (isSgNotEqualOp(n)) {
       ss = "/=";
    }
    else {
        //std::cout << "bad eqOp" << std::endl;
        ROSE_ASSERT(false);
    }
    return ss;
}

bool isBinaryLogicOp(SgNode* n) {
    if (isSgEqualityOp(n) || isSgLessThanOp(n) || isSgGreaterThanOp(n) || isSgNotEqualOp(n)) {
        return true;
    }
    else {
        return false;
    }
}

bool isBinaryOp(SgNode* n) {
    if (isSgAddOp(n) || isSgSubtractOp(n) || isSgMultiplyOp(n) || isSgDivideOp(n)) {
        return true;
    }
    else {
        return false;
    }
}

std::string getBinaryOp(SgNode* n) {
    std::string ss;
    if (isSgAddOp(n)) {
        ss = "+";
    }
    else if (isSgSubtractOp(n)) {
        ss = "-";
    }
    else if (isSgMultiplyOp(n)) {
        ss = "*";
    }
    else if (isSgDivideOp(n)) {
        ss = "/";
    }
    else {
        //std::cout << "unknown op in getBinaryOp" << std::endl;
        ROSE_ASSERT(false);
    }
    return ss;
}


/*
int main(int argc, char *argv[]) {

  struct timeval t1, t2;
  SgProject* proj = frontend(argc,argv);
  ROSE_ASSERT (proj != NULL);

  SgFunctionDeclaration* mainDefDecl = SageInterface::findMain(proj);

  SgFunctionDefinition* mainDef = mainDefDecl->get_definition();
   visitorTraversal* vis = new visitorTraversal();
    StaticCFG::CFG cfg(mainDef);
   //cfg.buildFullCFG();
    stringstream ss;
    string fileName= StringUtility::stripPathFromFileName(mainDef->get_file_info()->get_filenameString());
    string dotFileName1=fileName+"."+ mainDef->get_declaration()->get_name() +".dot";

    cfgToDot(mainDef,dotFileName1);
    //cfg->buildFullCFG();
    SgIncidenceDirectedGraph* g = new SgIncidenceDirectedGraph();
    g = cfg.getGraph();
    myGraph* mg = new myGraph();
    mg = instantiateGraph(g, cfg);
    vis->tltnodes = 0;
    vis->paths = 0;
    ipaths = 0;
    vis->orig = mg;
    vis->g = g;
    //vis->firstPrepGraph(constcfg);
    //t1 = getCPUTime();
    vis->constructPathAnalyzer(mg, true);
    //t2 = getCPUTime();
    //std::cout << "took: " << timeDifference(t2, t1) << std::endl;
    //cfg.clearNodesAndEdges();
    std::cout << "finished" << std::endl;
    std::cout << "tltnodes: " << vis->tltnodes << " paths: " << vis->paths << " ipaths: " << ipaths <<  std::endl;
    //delete vis;
    return 0;
}
*/


int main(int argc, char *argv[]) {
  string y = "yices.txt";
  yices_enable_log_file((char*) y.c_str());
  string fileSaver = "saviorStuff";
  ofstream fout;
    fout.open(fileSaver.c_str(),ios::app);

  SgProject* proj = frontend(argc,argv);
  ROSE_ASSERT (proj != NULL);

SgFunctionDefinition* mainDef = SageInterface::findMain(proj)->get_definition();
  Rose_STL_Container<SgNode*> functionDeclarationList = NodeQuery::querySubTree(proj,V_SgFunctionDeclaration);
  Rose_STL_Container<SgNode*> functionDefinitionList = NodeQuery::querySubTree(proj, V_SgFunctionDefinition);
 // std::cout << "functionDeclarationList.size(): " << functionDeclarationList.size() << std::endl;
 // std::cout << "functionDefinitionList.size(): " << functionDefinitionList.size() << std::endl;
  //ROSE_ASSERT(false);
  std::vector<SgNode*> funcs;
  for (Rose_STL_Container<SgNode*>::iterator i = functionDefinitionList.begin(); i != functionDefinitionList.end(); i++) {
          if (isSgFunctionDefinition(*i) != mainDef) {
          SgFunctionDefinition* fni = isSgFunctionDefinition(*i);
          ROSE_ASSERT(fni != NULL);
          //ROSE_ASSERT(find(funcs.begin(), funcs.end(), fni) == funcs.end());
          funcs.push_back(fni);
          }
  }
  //std::cout << "funcs.size(): " << funcs.size() << std::endl;
  //ROSE_ASSERT(false);
    int jj = 0;
  for (unsigned int i = 0; i < funcs.size(); i++) {
     // if (funcs[i] != mainDef) {
      visitorTraversalFunc* visfunc = new visitorTraversalFunc();
      //SgFunctionDeclaration* sfd = isSgFunctionDeclaration(funcs[i]);
      SgFunctionDefinition* sfdd = isSgFunctionDefinition(funcs[i]);
     //}
int counter = i;
   SgFunctionDefinition* fnc = isSgFunctionDefinition(sfdd);
   if (fnc != NULL) {
  stringstream ss;
  SgFunctionDeclaration* functionDeclaration = fnc->get_declaration();

  string fileName= functionDeclaration->get_name().str();//StringUtility::stripPathFromFileName(mainDef->get_file_info()->get_filenameString());
    string dotFileName1;
ss << fileName << "." << counter << ".dot";
    counter++;
    dotFileName1 = ss.str();
    StaticCFG::InterproceduralCFG* cfg = new StaticCFG::InterproceduralCFG(fnc);
    SgIncidenceDirectedGraph* g = new SgIncidenceDirectedGraph();
//     visitorTraversalFunc* vis = new visitorTraversalFunc();
    g = cfg->getGraph();
    CFGforT* mg = new CFGforT();
    mg = instantiateGraph(g, *cfg, fnc);
    visfunc->tltnodes = 0;
    //visfunc->paths = 0;
    //std::vector<std::vector<VertexID> > pt;
    //visfunc->paths = pt;
    visfunc->orig = mg;
    //visfunc->cfg = cfg;
    //visfunc->g = g;
    visfunc->constructPathAnalyzer(mg, true, 0, 0, true);
    std::vector<std::vector<SgGraphNode*> > vp = visfunc->vpaths;
    ROSE_ASSERT(sfdd != NULL);
    FuncPathMap[sfdd] = vp;
    //std::cout << "vp.size(): " << vp.size() << std::endl;

}
}
/*
int jjf = 0;
std::vector<SgGraphNode*> paths;
paths.push_back(path);
std::vector<SgNode*> called;
    while (jjf != paths.size()) {
        std::cout << "propagating" << std::endl;
        path = paths[jjf];
        int jj = 0;
        while (jj != path.size()) {
            if (isSgFunctionCallExp(path[jj]->get_SgNode()) && find(called.begin(), called.end(), path[jj]->get_SgNode()) == called.end()) {
                propagateFunctionCall(path, jj, jjf);
                called.push_back(path[jj]->get_SgNode());
               jjf = 0;
             //   noadd = true;
               jj = 0;
                break;
            }
            else {
                jj++;
            }
       }
       if (noadd) {
       noadd = false;
       }

       else {
       jjf++;
       }
       }

    std::cout << "paths.size(): " << paths.size() << std::endl;
    //ROSE_ASSERT(false); 
    pathnumber += paths.size();
    std::vector<SgNode*> ncalled;

for (int q = 0; q < paths.size(); q++) {  
  std::vector<SgGraphNode*> path = path[q]; 
  ROSE_ASSERT(mainDef != NULL);
  //if (mainDefDecl != NULL) {
*/
  //SgFunctionDefinition* mainDef = mainDefDecl->get_definition();
   visitorTraversal* vis = new visitorTraversal();
    StaticCFG::CFG* cfg = new StaticCFG::CFG(mainDef);
    vis->pathnumber = 0;
    stringstream ss;
    string fileName= StringUtility::stripPathFromFileName(mainDef->get_file_info()->get_filenameString());
    string dotFileName1=fileName+"."+ mainDef->get_declaration()->get_name() +".dot";

    SgIncidenceDirectedGraph* g = new SgIncidenceDirectedGraph();
    g = cfg->getGraph();
    myGraph* mg = new myGraph();
    mg = instantiateGraph(g, *cfg);
    vis->tltnodes = 0;
    //vis->pathnumber = 0;
    ipaths = 0;
    vis->orig = mg;
    //openorig = mg;
    vis->g = g;
    //openg = g;
    vis->cfg = cfg;
    //opencfg = cfg;

    vis->constructPathAnalyzer(mg, true);
    //if (ipaths > 0) {
    cout << "filename: " << fileName << std::endl;
    cout << "finished" << std::endl;
    cout << "tltnodes: " << vis->tltnodes << " paths: " << vis->pathnumber << " ipaths: " << ipaths <<  std::endl;
   //} 
   // }
    fout.close();
    return 0;
}



/*

int main(int argc, char *argv[]) {
  pathnum = 0;
  ipaths = 0;
  SgProject* proj = frontend(argc,argv);
  ROSE_ASSERT (proj != NULL);

  SgFunctionDeclaration* mainDefDecl = SageInterface::findMain(proj);

  SgFunctionDefinition* mainDef = mainDefDecl->get_definition();
   visitorTraversal* vis = new visitorTraversal();
   visitorTraversal* vis2 = new visitorTraversal();
   nGraph = new newGraph();
   //vis->nGraph = nGraph;
   //newGraph* nnGraph = new newGraph();
    StaticCFG::CFG* cfg1 = new StaticCFG::CFG(mainDef);
   //cfg.buildFullCFG();
    stringstream ss;
    string fileName= StringUtility::stripPathFromFileName(mainDef->get_file_info()->get_filenameString());
    string dotFileName1=fileName+"."+ mainDef->get_declaration()->get_name() +".dot";

    cfgToDot(mainDef,dotFileName1);
    //cfg->buildFullCFG();
    //SgIncidenceDirectedGraph* cf = new SgIncidenceDirectedGraph();
    SgIncidenceDirectedGraph* cf = cfg1->getGraph();
    myGraph* mg = new myGraph();
    mg = instantiateGraph(cf, *cfg1);
    vis2->tltnodes = 0;
    vis2->paths = 0;
    vis->tltnodes = 0;
    vis->paths = 0;
    //vis->firstPrepGraph(constcfg);
    vis->g = cf;
    vis2->g = cf;
    vis2->orig = mg;
    cfg = cfg1;
    vis->orig = mg;
    vis->constructPathAnalyzer(mg, true);
    std::cout << "constructed" << std::endl;
    std::cout << "ipaths: " << ipaths << std::endl; 
   // printHotness2(nGraph);
   // std::cout << "mapped" << std::endl;i
   std::vector<std::vector<int> > pts;
   std::vector<int> ptsP;
    //std::vector<SgExpressionStmt*> exprs = SageInterface::querySubTree<SgExpressionStmt>(proj);
/*  
 for (int q1 = 0; q1 < exprs.size(); q1++) {
      ptsP.clear();
      for (int q2 = 0; q2 < exprs.size(); q2++) {
          if (q1 != q2) {
              vis->paths = 0;
              vis->tltnodes = 0;
              vis->constructPathAnalyzer(mg, exprs[q1], exprs[q2]);
              std::cout << vis->paths << " between expr" << q1 << " and expr" << q2 << std::endl;
              ptsP.push_back(vis->paths);
          }
          pts.push_back(ptsP);
      }
    }
    for (int i = 0; i < pts.size(); i++) {
        for (int j = 0; j < pts[i].size(); j++) {
            std::cout << "between expr" << i << "and expr" << j << " there are " << pts[i][j] << std::endl;
        }
    }
*/
    //cfg.clearNodesAndEdges();
    //std::cout << "finished" << std::endl;
    //std::cout << "tltnodes: " << vis->tltnodes << " paths: " << vis->paths << std::endl;
    //delete vis;
//}
                                                                                                                                                                                                                                                                                                                                                                                                                                                            

/*
std::vector<SgExprStatement*> exprList = SageInterface::querySubTree<SgExprStatement>(project);
for (Rose_STL_Container<SgGraphNode*>::iterator i = exprList.begin(); i != exprList.end(); i++) {
*/

//          SgExprStatement* expr = isSgExprStatement(*i);

