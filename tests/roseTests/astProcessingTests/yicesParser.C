#include <iostream>
#include <fstream>
//#include <rose.h>
#include <string>
#include <err.h>
#include "SgGraphTemplate.h"
#include "graphProcessing.h"
#include "staticCFG.h"
#include "yices_c.h"
/* Testing the graph traversal mechanism now implementing in AstProcessing.h (inside src/midend/astProcessing/)*/


using namespace std;
using namespace boost;




bool forFlag;

std::vector<int> breakN(std::vector<SgGraphNode*> expr, int n);

string getType(SgNode* n);

int nvars;
std::map<SgName, string> nameOf;
bool noAssert;
std::map<SgNode*, int> forsts;

typedef myGraph CFGforT;


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


std::map<SgFunctionDefinition*, std::vector<std::vector<SgGraphNode*> > > FuncPathMap;


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


std::map<SgName, yices_expr> getExpr;

class visitorTraversal : public SgGraphTraversal<CFGforT>
   {
     public:
          int tltnodes;
          int paths;
          //std::map<SgName, yices_expr> getExpr;
          std::set<SgNode*> knownNodes;     
    //      std::vector<std::vector<SgGraphNode*> > pathstore;
          void analyzePath(std::vector<VertexID>& pth);
          SgIncidenceDirectedGraph* g;
          myGraph* orig;
          StaticCFG::CFG* cfg;
          std::vector<std::vector<SgGraphNode*> > inconsistents;
          //std::map<SgVariableSymbol, string> nameOf;
          //int nvars;
   };

class visitorTraversalFunc : public SgGraphTraversal<CFGforT>
   {
     public:
          int tltnodes;
          std::vector<std::vector<SgGraphNode*> > paths;
          void analyzePath(std::vector<VertexID>& pth);
          CFGforT* orig;
          //std::map<SgVariableSymbol, string> nameOf;
          //int nvars;
   };

newGraph* nGraph;


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

void visitorTraversalFunc::analyzePath(std::vector<VertexID>& pathR) {
    std::vector<SgGraphNode*> path;
    for (unsigned int j = 0; j < pathR.size(); j++) {
       SgGraphNode* R = (*orig)[pathR[j]].sg;
       path.push_back(R);
    }
    ROSE_ASSERT(isSgFunctionDefinition(path[0]->get_SgNode()));
    int curr = 0;
  /*
    int indie = 0;
    while (!isSgFunctionDefinition(path[curr]->get_SgNode()) || indie != 2) {
        if (isSgFunctionDefinition(path[curr]->get_SgNode())) {
           indie++;
           if (indie == 2) {
               break;
           }
        }
     //   std::cout << "index: " << getIndex(path[curr]) << std::endl;
    //    std::cout << "node: " << path[curr]->get_SgNode()->unparseToString() << std::endl;
        curr++;
    }
    curr++;
    */
    std::vector<SgGraphNode*> newpath;
    newpath.push_back(path[curr]);
    curr++;
    while (!isSgFunctionDefinition(path[curr]->get_SgNode())) {
      //  std::cout << "node: " << path[curr]->get_SgNode()->unparseToString() << std::endl;
        newpath.push_back(path[curr]);
        curr++;
    }
    newpath.push_back(path[curr]);
    paths.push_back(newpath);

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
            std::cout << cf << "expr: " << (*g)[v].exprstr << std::endl;
            str << cf << " expr: " << (*g)[v].exprstr;
            }
            else if ((*g)[v].vardec) {
            std::cout << cf << " vardec: " << (*g)[v].varstr << std::endl;
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
            //const newGraph* gc = g;
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


std::vector<VertexID> exprs;

int ipaths;


std::vector<VertexID> ambientPath;
std::vector<std::vector<SgNode*> > ambientPathAssociations;


void visitorTraversal::analyzePath(std::vector<VertexID>& pathR) {
   //yices_context ctx; 
   unsigned int i = 0;
   paths++;
   ambientPath = pathR; 
    noAssert = false;
    rounds = 0;
    forFlag = false;
    stringstream pathstream;
    //std::set<SgNode*> knownNodes;
    nameOf.clear();
    getExpr.clear();
    std::vector<std::vector<SgGraphNode*> > unsats;
    //VertexID2 start = boost::add_vertex(*nGraph);
    //graphVertex[(*orig)[pathR[0]]] = start;
    //std::cout << "path: " << pathnum << std::endl;
    //for (int i = 0; i < pathR.size(); i++) {
    //    std::cout << vertintmap[pathR[i]] << ", ";
   // }
  //  std::cout << std::endl;
   // std::cout << "pathnum: " << pathnum << std::endl;
   // for (int qw = 0; qw < path.size(); qw++) {
   //     std::cout << path[qw]->get_SgNode()->unparseToString() << std::endl;
    //}
    pathnum++;
    bool inconsistent = false;
    std::vector<std::vector<SgGraphNode*> > paths;
    std::vector<SgGraphNode*> path;
    //std::vector<SgGraphNode*> pathR;
    std::vector<SgGraphNode*> exprPath;
    for (unsigned int j = 0; j < pathR.size(); j++) {
       SgGraphNode* R = (*orig)[pathR[j]].sg;
       path.push_back(R);
    }
    paths.push_back(path);
    //graphVertex[path[0]] = start;
    int ps = 0;
    std::vector<SgGraphNode*> called;
    while (ps < paths.size()) {
    std::cout << "paths.size(): " << paths.size() << std::endl;
    std::cout << "ps: " << ps << std::endl;
    yices_context ctx = yices_mk_context();
    path = paths[ps];
    //std::cout << "path: " << ps << std::endl;
    //for (int qw = 0; qw < path.size(); qw++) {
    //    std::cout << path[qw]->get_SgNode()->unparseToString() << std::endl;
   // }
    std::cout << std::endl;
    while (i < path.size()) {
      /*  if (yices_inconsistent(ctx)) {
            inconsistent = true;
            inconsistents.push_back(path);
//            ipaths++;

            ps++;
            std::cout << "inconsistent: " << inconsistents.size() << std::endl;
            break;
        }
        else*/ if (!yices_check(ctx)) {
            //unsatisfiable = true;
            std::cout << "unsatisfiable" << std::endl;
            unsats.push_back(path);
            ps++;
            break;
        }

        exprPath.clear();
        std::vector<SgGraphNode*> vb12;
       
        if (isSgExprStatement(path[i]->get_SgNode()) && isSgFunctionCallExp(path[i+1]->get_SgNode())) {
            std::cout << "found a function" << std::endl;
            
            if (find(called.begin(), called.end(), path[i+1]) == called.end()) {
            int k = i+1;
            while (!isSgExprListExp(path[k]->get_SgNode())) {
                k++;
            }
            unsigned int cfgEnd = (path[k]->get_SgNode())->cfgIndexForEnd();
            k++;
            int checkindie = 1;
            while (!isSgExprListExp(path[k]->get_SgNode()) || checkindie != cfgEnd) {
                if (isSgExprListExp(path[k]->get_SgNode())) {
                    checkindie++;
                }
                vb12.push_back(path[k]);
                k++;
            }
            std::cout << "vb12: " << std::endl;
            for (int qr = 0; qr < vb12.size(); qr++) {
                std::cout << vb12[qr]->get_SgNode()->unparseToString() << std::endl;
            }
            std::cout << std::endl;
            //std::vector<int> brokenlist = breakN(vb12, cfgEnd+1);
            
          //  SgExprListExp* sele = path[k]->get_SgNode();
            SgFunctionCallExp* sgfce = isSgFunctionCallExp(path[i+1]->get_SgNode());
            //ambientPathAssociations.push_back(sgfce);
            SgFunctionDeclaration* sgfd = isSgFunctionCallExp(path[i+1]->get_SgNode())->getAssociatedFunctionDeclaration();
            SgFunctionParameterList* sfpl = sgfd->get_parameterList();
            SgInitializedNamePtrList sinp = sfpl->get_args();
            SgInitializedNamePtrList::iterator ite = sinp.begin();
            //SgExpressionPtrList sepl = sele->get_expressions();
            //nsigned int seplint = sepl.size();
            SgName svs = (isSgInitializedName((*ite)))->get_qualified_name();
            std::cout << "SgInitializedNamePtrList.size(): " << sinp.size() << std::endl; 
            
            std::vector<int> brokenlist = breakN(vb12, cfgEnd-1);
            yices_expr exprlist[brokenlist.size()-1];
            for (int qq = 1; qq < brokenlist.size(); qq++) {
                int curr = brokenlist[qq-1];
                std::vector<SgGraphNode*> vbi;
                while (curr != brokenlist[qq]) {
                   vbi.push_back(vb12[curr]);
                   curr++;
                }
                yices_expr ei = new yices_expr();
                ei = mainParse(vbi, ctx);
                ROSE_ASSERT(ei != NULL);
                exprlist[qq-1] = ei;
            }
            int expint = 0;
            while (expint < brokenlist.size()-1) {
                SgInitializedName* sin = (*ite);
                SgName svs = sin->get_qualified_name();
                stringstream stst;
                std::vector<SgGraphNode*> vec1;
                SgType* ty = sin->get_type();
                string parsed = getType(ty);
 
        stringstream funN;
        string ss;
            funN << "V" << nvars;
            nameOf[svs] = funN.str();
            nvars++;
        char* fun = (char*) funN.str().c_str();
        char* valTypeCh = (char*) parsed.c_str();
        yices_type ty1 = yices_mk_type(ctx, valTypeCh);
        yices_var_decl vdecl = yices_mk_var_decl(ctx, fun, ty1);
        yices_expr e1 = yices_mk_var_from_decl(ctx, vdecl);
        getExpr[svs] = e1;
        ROSE_ASSERT(e1 != NULL);
        yices_expr e2 = exprlist[expint];
        ROSE_ASSERT(e2 != NULL);
          yices_expr ret = new yices_expr();
            ret = yices_mk_eq(ctx, e1, e2);
            ROSE_ASSERT(ret != NULL);
        yices_assert(ret, ctx);
                ite++;
               expint++;;
   
            }
            SgFunctionDefinition* sgfdef = sgfd->get_definition();
            std::vector<std::vector<SgGraphNode*> > funcPaths = FuncPathMap[sgfdef];
            ROSE_ASSERT(funcPaths.size() > 0);
            
            
            std::vector<SgGraphNode*>::iterator it = path.begin();
            it += i;
            std::vector<SgGraphNode*> oldpath = path;
            std::vector<SgGraphNode*> newpath;
            std::cout << "oldpath.size(): " << oldpath.size() << std::endl;
            for (int qe = 0; qe < i+1; qe++) {
                newpath.push_back(path[qe]);
            }
            for (int qe2 = 0; qe2 < funcPaths[0].size(); qe2++) {
                newpath.push_back(funcPaths[0][qe2]);
            }
            for (int qe3 = i+1; qe3 < path.size(); qe3++) {
                newpath.push_back(path[qe3]);
            }
            path = newpath;
            //path.insert(it, funcPaths[0].begin(), funcPaths[0].end());
            std::cout << "newpath.size(): " << path.size() << std::endl;
            for (int qw = 1; qw < funcPaths.size(); qw++) {
                std::vector<SgGraphNode*> npath = oldpath;
          for (int qe = 0; qe < i+1; qe++) {
                npath.push_back(path[qe]);
            }
            for (int qe2 = 0; qe2 < funcPaths[qw].size(); qe2++) {
                npath.push_back(funcPaths[qw][qe2]);
            }
            for (int qe3 = i+1; qe3 < path.size(); qe3++) {
                npath.push_back(path[qe3]);
            }

                //npath.insert(it, funcPaths[qw].begin(), funcPaths[qw].end());
                paths.push_back(npath);
            }
            called.push_back(path[i+1]);
            i++;
            }
            else {
                i+=1;
            }
       }
                    
            
        if (isSgInitializedName(path[i]->get_SgNode()) /*&& knownNodes.find(path[i]->get_SgNode()) == knownNodes.end()*/) {
            exprPath.push_back(path[i]);
            unsigned int k = i+1;
            while (k < path.size() && (!isSgInitializedName(path[k]->get_SgNode()) || path[k]->get_SgNode() != path[i]->get_SgNode())) {
                exprPath.push_back(path[k]);
                k++;
            }
            if (k < path.size()) {
            exprPath.push_back(path[k]);
            }
            if (k == path.size()) {
                 ps++;
                 break;
           }
            yices_expr yx = mainParse(exprPath, ctx);
            yices_assert(ctx, yx);
            i += exprPath.size();
           k = 0;
        }
        else if (isSgForStatement(path[i]->get_SgNode()) && isSgForInitStatement(path[i+1]->get_SgNode())) {
            forFlag = true;
            std::vector<SgGraphNode*> vec1;
            unsigned int j = i+2;
            unsigned int w = 2;
            while (!isSgInitializedName(path[j]->get_SgNode()) && !isSgAssignOp(path[j]->get_SgNode())) {
                j++;
                w++;
            }
            vec1.push_back(path[j]);
            unsigned int k = j+1;
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
        else if (isSgExprStatement(path[i]->get_SgNode()) /*&& knownNodes.find(path[i]->get_SgNode()) == knownNodes.end()*/) {
            unsigned int j = i+1;
            if (find(exprs.begin(), exprs.end(), pathR[i]) == exprs.end()) {
                exprs.push_back(pathR[i]);
            }
            //exprPath.push_back(path[i]);
            while (j < path.size() && (!isSgExprStatement(path[j]->get_SgNode()) || path[j]->get_SgNode() != path[i]->get_SgNode())) {
                ROSE_ASSERT(j < path.size());
                exprPath.push_back(path[j]);
                j++;
               // std::cout << "in second while" << std::endl;
            }
            //std::cout << "path: " << std::endl;
            //for (int qq = 0; qq < path.size(); qq++) {
            //   std::cout << (path[qq]->get_SgNode())->unparseToString() << std::endl;
            //}
            //std::cout << std::endl;
            if (j == path.size()) {
                ps++;
                break;
            }
            ROSE_ASSERT(j < path.size());
		    //exprPath.push_back(path[j]);
		   // i += exprPath.size();
            //std::vector<SgGraphNode*> exprP;
            //for (int q = 1; q < exprPath.size()-1; q++) {
            //    exprP.push_back(exprPath[q]);
           // }
            yices_expr y2 =  mainParse(exprPath, ctx);
            ROSE_ASSERT(y2 != NULL);
            ROSE_ASSERT(path[j] != NULL);
            ROSE_ASSERT(g != NULL);
            //std::cout << "exprPath.size(): " << exprPath.size() << std::endl; 
            std::set<SgDirectedGraphEdge*> oeds = g->computeEdgeSetOut(path[j]);
            ROSE_ASSERT(oeds.size() == 1);
            SgGraphNode* onn = path[j+1];//(*(oeds.begin()))->get_to();
            //ROSE_ASSERT(onn == path[j+1]);
            std::set<SgDirectedGraphEdge*> ifoeds = g->computeEdgeSetOut(path[j+1]); 
            if ((isSgForStatement(onn->get_SgNode()) || isSgIfStmt(onn->get_SgNode())) && ifoeds.size() >= 2) { 
                CFGNode cn = cfg->toCFGNode(path[j+2]);
                std::vector<CFGEdge> ed = cn.inEdges();
                int qw = 0;
                while (ed[qw].source() != cfg->toCFGNode(path[j+1])) {
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
                    if (isSgForStatement(onn->get_SgNode())) {
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
        else {
            i++;
        }
    
    }
    if (yices_inconsistent(ctx)) {
        ipaths++;
        inconsistent = false;
    }
    yices_del_context(ctx);
    ps++;
}
}

StaticCFG::CFG* cfg;


std::vector<int> breakTriple(std::vector<SgGraphNode*> expr) {
    SgNode* index = expr[0]->get_SgNode();
    std::vector<int> bounds(3, 0);
    bounds[0] = 0;
    int i = 1;
    while (expr[i]->get_SgNode() != index) {
        int er = (int) expr.size();
        ROSE_ASSERT(i < er);
        i++;
    }
    bounds[1] = i;
    bounds[2] = expr.size()-1;

    return bounds;
    }
   

std::vector<int> breakN(std::vector<SgGraphNode*> expr, int n) {
    SgNode* index = expr[0]->get_SgNode();
    std::vector<int> bounds(n, 0);
    bounds[0] = 0;
    int i = 1;
    for (int j = 1; j < n; j++) {
        while (expr[i]->get_SgNode() != index) {
          //  unsigned int er = expr.size();
          //  ROSE_ASSERT(i < er);
            i++;
        }
        bounds[j] = i;
    }
    return bounds;
}


string isAtom(SgNode*);
bool isLogicalSplit(SgNode*);
string getLogicalSplit(SgNode*);
string getBinaryLogicOp(SgNode*);
bool isBinaryLogicOp(SgNode*);
bool isBinaryOp(SgNode*);
string getBinaryOp(SgNode*);
/*
void applyFunctionPaths(SgGraphNode* sgn, yices_context& ctx);

void applyFunctionPaths(std::vector<SgGraphNode*> expr, yices_context& ctx) {
          ROSE_ASSERT(isSgFunctionCallExp(expr[0]->get_SgNode()));
          std::vector<int> broken = breakN(expr, 4);
            SgFunctionRefExp* sgRef = isSgFunctionRefExp(expr[1]->get_SgNode());
            std::vector<SgGraphNode*> vb12;
            for (int q = bounds[1]; q < bounds[2]; q++) {
                vb12.push_back[expr[q]];
            }
            ROSE_ASSERT(isSgExprListExp(expr[bounds[1]+1]->get_SgNode()));
            unsigned int endindex = (isSgExprListExp(expr[bounds[1]+1]->get_SgNode()))->cfgIndexForEnd();
            std::vector<int> brokenlist = breakN(vb12, endindex+1);
            yices_expr exprlist[brokenlist.size()-1];
            for (int i = 1; i < brokenlist.size(); i++) {
                int curr = brokenlist[i-1];
                while (curr != brokenlist[i]) {
                   vbi.push_back(vb12[curr]);
                }
                yices_expr ei;
                ei = mainParse(vbi, ctx);
                exprlist[i-1] = ei;
            }
            yices_expr vars = yices_mk_and(ctx, exprlist, exprlist.size())
            return vars;
}
*/
yices_expr mainParse(vector<SgGraphNode*> expr, yices_context& ctx) {
    rounds++;
    std::stringstream stst;
    string parsed;
    bool unknown_flag = false;
    std::vector<SgGraphNode*> vec1;
    std::vector<SgGraphNode*> vec2;
    stringstream ss;
    yices_expr ret;
    if (expr.size() == 0) {
        yices_expr empty = new yices_expr;
        return empty;
    }
    if (unknown_flag) {
        yices_expr empty = new yices_expr;
        return empty;
    }
   // else if (isSgFunctionCallExp(expr[0]->get_SgNode())) {
   //     //pathAssociates.push_back(expr);
   //     yices_expr ret = yices_mk_true(ctx);
   //     return ret;
   // }  
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
        std::vector<int> bounds = breakTriple(expr);
        for (int i = bounds[0]+1; i < bounds[1]; i++) {
            vec1.push_back(expr[i]);
        }
        for (int j = bounds[1]+1; j < bounds[2]; j++) {
            vec2.push_back(expr[j]);
        }
                yices_expr e1 = yices_mk_fresh_bool_var(ctx);
                yices_expr e2 = yices_mk_fresh_bool_var(ctx);
                e1 = mainParse(vec1, ctx);
                e2 = mainParse(vec2, ctx);
                if (vec1.size() == 0 && ls == "and") { 
                    e1 = yices_mk_false(ctx);
                }
                else if (vec1.size() == 0 && ls == "or") {
                    e1 = yices_mk_false(ctx);
                }
                else if (vec2.size() == 0 && ls == "and") {
                    e2 = yices_mk_false(ctx);
                }
                else if (vec2.size() == 0 && ls == "or") {
                    e2 = yices_mk_false(ctx);
                }
                yices_expr arr[2];
                arr[0] = e1;
                arr[1] = e2;
            if (ls == "or") {
                ret = yices_mk_or(ctx, arr, 2);
            }
            else if (ls == "and") {
                ret = yices_mk_and(ctx, arr, 2);
            }
            else {
                ROSE_ASSERT(false);
            }
            return ret;    
    }
    else if (isBinaryLogicOp(expr[0]->get_SgNode()) || isBinaryOp(expr[0]->get_SgNode())) {
        std::vector<int> bounds = breakTriple(expr);
        for (int i = bounds[0]+1; i < bounds[1]; i++) {
            vec1.push_back(expr[i]);
        }
        for (int j = bounds[1]+1; j < bounds[2]; j++) {
            vec2.push_back(expr[j]);
        }
        if (isBinaryLogicOp(expr[0]->get_SgNode())) {
            parsed = getBinaryLogicOp(expr[0]->get_SgNode());
        }
        else {
            parsed = getBinaryOp(expr[0]->get_SgNode());
        }
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
                return ret;
            }
            ROSE_ASSERT(ret != NULL);
            return ret;
        }
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
            else {
                ROSE_ASSERT(false);
            }
        return ret;
        }
    }
    else if (isSgPlusPlusOp(expr[0]->get_SgNode())) {
        for (unsigned int i = 1; i < expr.size() - 1; i++) {
            vec1.push_back(expr[i]);
        }
        yices_expr e1 = mainParse(vec1, ctx);
        stringstream funN;
        funN << "V" << nvars;
        nvars++;
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
        }
        else if (ty == "double") {
            double dval = isSgDoubleVal(expr[0]->get_SgNode())->get_value();
            ret = yices_mk_num(ctx, dval);    
        }
        else if (ty == "float") {
           float fval = isSgFloatVal(expr[0]->get_SgNode())->get_value();
           ret = yices_mk_num(ctx, fval);
        }
        else if (ty == "short") {
           short sval = isSgShortVal(expr[0]->get_SgNode())->get_value();
           ret = yices_mk_num(ctx, sval);
        }
        else if (ty == "long") {
            long lval = isSgLongIntVal(expr[0]->get_SgNode())->get_value();
            ret = yices_mk_num(ctx, lval);
        }
        else if (ty == "long long int") {
            long long llval = isSgLongLongIntVal(expr[0]->get_SgNode())->get_value();
            ret = yices_mk_num(ctx, llval);
        }
        else if (ty == "long double") {
            long double lldval = isSgLongDoubleVal(expr[0]->get_SgNode())->get_value();
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
            ROSE_ASSERT(false);
        }
        return ret;
    }
    else if (isSgVarRefExp((expr[0])->get_SgNode())) {
        SgName svs = isSgVarRefExp(expr[0]->get_SgNode())->get_symbol()->get_name();;
        ROSE_ASSERT(nameOf.find(svs) != nameOf.end());
        yices_expr e1 = getExpr[svs];
        ret = e1;
        return ret;
        
    }
    else if (isSgInitializedName(expr[0]->get_SgNode())) {
        stringstream stst;
        std::vector<SgGraphNode*> vec1;
        string valType = isAtom((expr[2])->get_SgNode());
        unsigned int p = 2;
        SgName svs = (isSgInitializedName(expr[0]->get_SgNode()))->get_qualified_name();
        while (p < expr.size() && !isSgAssignInitializer(expr[p]->get_SgNode())) {
            vec1.push_back(expr[p]);
            p++;
        }
        stringstream funN;
        string ss;
            funN << "V" << nvars;
            nameOf[svs] = funN.str();
            nvars++;  
        char* fun = (char*) funN.str().c_str();
        char* valTypeCh = (char*) valType.c_str();
        yices_type ty = yices_mk_type(ctx, valTypeCh);
        yices_var_decl vdecl = yices_mk_var_decl(ctx, fun, ty);
        yices_expr e1 = yices_mk_var_from_decl(ctx, vdecl);
        getExpr[svs] = e1;
        yices_expr e2 = mainParse(vec1, ctx);
        if (forFlag) {
            ret = yices_mk_eq(ctx, e1, e1);
        }
         else {
            ret = yices_mk_eq(ctx, e1, e2);
        }
        
        return ret;
    }

     else if (isSgReturnStmt(expr[0]->get_SgNode())) {
            int k = 1;
            std::cout << "returnstmt" << std::endl;
            std::vector<SgGraphNode*> evalpath;
            while (!isSgReturnStmt(expr[k]->get_SgNode())) {
                evalpath.push_back(expr[k]);
                k++;
            }
            yices_expr ret = mainParse(evalpath, ctx);
            return ret;
    }


    else if (isSgAssignOp(expr[0]->get_SgNode())) {
        stringstream stst;
        ROSE_ASSERT(isSgVarRefExp(expr[1]->get_SgNode()));
        SgName svs;
        yices_expr e1;
        yices_expr e2;
        if (isAtom(expr[3]->get_SgNode()) == "") {
            SgNode* sn = expr[3]->get_SgNode();
            //std::cout << "sn name: " << sn->unparseToString() << std::endl;
            int endin = (expr[3]->get_SgNode())->cfgIndexForEnd();
            int indieend = 1;
            std::vector<SgGraphNode*> exprnew;
            int curr = 4;
            exprnew.push_back(expr[3]);
          //  std::cout << "endin: " << endin << std::endl;
            while (expr[curr]->get_SgNode() != expr[3]->get_SgNode() || indieend != endin) /*&& curr < expr.size()-1)*/ {
                if (expr[curr]->get_SgNode() == expr[3]->get_SgNode()) {
                    indieend++;
                }
            //    std::cout << "index: " << getIndex(expr[curr]);
                exprnew.push_back(expr[curr]);
                curr++;
                ROSE_ASSERT(curr < expr.size());
            }
            exprnew.push_back(expr[curr]);
            
            e2 = mainParse(exprnew, ctx);
            svs = (isSgVarRefExp((expr[1]->get_SgNode())))->get_symbol()->get_declaration()->get_qualified_name();
                ROSE_ASSERT(nameOf.find(svs) != nameOf.end());
                stringstream ss;
                ss << "V";
                ss << nvars;
                nvars++;
                //nameOf[svs] = ss.str();
                e1 = getExpr[svs];
                //getExpr[nm] = e2;
                ret = yices_mk_eq(ctx, e1, e2); 
                return ret;
       }
             
             
            
        
        else {
        string valType = isAtom(expr[3]->get_SgNode());
        std::vector<int> bounds = breakTriple(expr);
        svs = (isSgVarRefExp((expr[1]->get_SgNode()))->get_symbol()->get_declaration()->get_qualified_name());
        
        for (int j = bounds[1]+1; j < bounds[2]; j++) {
            vec2.push_back(expr[j]);
        }
        
        if (nameOf.find(svs) != nameOf.end()) {
            stringstream ss;
            ss << "V";
            ss << nvars;
            nvars++;
            yices_type ty1 = yices_mk_type(ctx, (char*) valType.c_str());
            yices_var_decl decl1 = yices_mk_var_decl(ctx, (char*) ss.str().c_str(), ty1);
            e2 = mainParse(vec2, ctx);
            
            yices_expr e1 = yices_mk_var_from_decl(ctx, decl1);
            getExpr[svs] = e1;
            
            
            
            if (forFlag) {
                ret = e1;
            }
            else {
                ret = yices_mk_eq(ctx, e1, e2);
            }
            nameOf[svs] = ss.str();
            getExpr[svs] = e1;
        }
        else {
            stringstream ss;
            ss << "V";
            ss << nvars;
            nvars++;
            char valTypeCh[valType.size()];
            for (unsigned int k = 0; k < valType.size(); k++) {
                valTypeCh[k] = valType[k];
            }
            char nam[ss.str().size()];
            for (unsigned int q = 0; q < ss.str().size(); q++) {
                nam[q] = ss.str()[q];
            }
            string fun = valType;
            yices_type ty = yices_mk_type(ctx, (char*) valType.c_str());
	    yices_var_decl decl1 = yices_mk_var_decl(ctx,nam, ty);
            yices_expr e1 = yices_mk_var_from_decl(ctx, decl1);
            yices_expr e2 = mainParse(vec2, ctx);
            if (forFlag) {
                ret = e1;
            }
            else {
                ret = yices_mk_eq(ctx, e1, e2);
            }
            getExpr[svs] = e1;
            nameOf[svs] = fun;
        }
        }
        return ret;
    }
    else {
        ret = yices_mk_fresh_bool_var(ctx);
        return ret;
    }
    return ret;   
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
        ROSE_ASSERT(false);
    }
    return ss;
}



int main(int argc, char *argv[]) {

  string fileSaver = "saviorStuff";
  ofstream fout;
    fout.open(fileSaver.c_str(),ios::app);

  SgProject* proj = frontend(argc,argv);
  ROSE_ASSERT (proj != NULL);
 
SgFunctionDeclaration* mainDefDecl = SageInterface::findMain(proj);
  Rose_STL_Container<SgNode*> functionDeclarationList = NodeQuery::querySubTree(proj,V_SgFunctionDeclaration);
  std::vector<SgNode*> funcs;
  for (Rose_STL_Container<SgNode*>::iterator i = functionDeclarationList.begin(); i != functionDeclarationList.end(); i++) {

          SgFunctionDeclaration* fni = isSgFunctionDeclaration(*i);
          ROSE_ASSERT(fni != NULL);
          funcs.push_back(fni);
  }
  for (unsigned int i = 0; i < funcs.size(); i++) {
      if (funcs[i] != mainDefDecl) {
      visitorTraversalFunc* visfunc = new visitorTraversalFunc();
      SgFunctionDeclaration* sfd = isSgFunctionDeclaration(funcs[i]);
      SgFunctionDefinition* sfdd = sfd->get_definition();
  
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
    FuncPathMap[sfdd] = visfunc->paths;

}
}
}
/*
    if (ipaths > 0) {
    string fN = StringUtility::stripPathFromFileName(fnc->get_file_info()->get_filenameString());
    fout << "filename: " << fN << std::endl;
    fout << "function: " << fileName << std::endl;
    fout << "paths: " << vis->paths << std::endl;
    fout << "ipaths: " << ipaths << std::endl;
    }
    }
    }
*/


//  SgFunctionDeclaration* mainDefDecl = SageInterface::findMain(proj);
  ROSE_ASSERT(mainDefDecl != NULL);
  if (mainDefDecl != NULL) {
  SgFunctionDefinition* mainDef = mainDefDecl->get_definition();
   visitorTraversal* vis = new visitorTraversal();
    StaticCFG::InterproceduralCFG* cfg = new StaticCFG::InterproceduralCFG(mainDef);
    
    stringstream ss;
    string fileName= StringUtility::stripPathFromFileName(mainDef->get_file_info()->get_filenameString());
    string dotFileName1=fileName+"."+ mainDef->get_declaration()->get_name() +".dot";

    SgIncidenceDirectedGraph* g = new SgIncidenceDirectedGraph();
    g = cfg->getGraph();
    myGraph* mg = new myGraph();
    mg = instantiateGraph(g, *cfg, mainDef);
    vis->tltnodes = 0;
    vis->paths = 0;
    ipaths = 0;
    vis->orig = mg;
    vis->g = g;
    vis->cfg = cfg;
    
    vis->constructPathAnalyzer(mg, true);
    //if (ipaths > 0) {
    cout << "filename: " << fileName << std::endl;
    cout << "finished" << std::endl;
    cout << "tltnodes: " << vis->tltnodes << " paths: " << vis->paths << " ipaths: " << ipaths <<  std::endl;
   //} 
    }
    fout.close();
    return 0;
}

