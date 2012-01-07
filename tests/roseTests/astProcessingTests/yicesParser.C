#include <iostream>
#include <fstream>
//#include <rose.h>
#include <string>
#include <err.h>
#include <SgGraphTemplate.h>
#include <graphProcessing.h>
#include <staticCFG.h>
#include "yices_c.h"
/* Testing the graph traversal mechanism now implementing in AstProcessing.h (inside src/midend/astProcessing/)*/


using namespace std;
using namespace boost;




bool forFlag;

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

void visitorTraversal::analyzePath(std::vector<VertexID>& pathR) {
   //yices_context ctx; 
   unsigned int i = 0;
   paths++; 
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
    //graphVertex[path[0]] = start;
    yices_context ctx = yices_mk_context();
    while (i < path.size()) {
        if (yices_inconsistent(ctx)) {
            inconsistent = true;
            //std::cout << "inconsistent" << std::endl;
            break;
        }
        exprPath.clear();
       // if (isSgFunctionDefinition(path[i]->get_SgNode())) {
        //    exprPath.push_back(path[i]);
        //    int k = i+1;
            //ROSE_ASSERT(isSgFunctionParameterList(path[k]->get_SgNode()));
        //    while (!isSgFunctionDefinition(path[k])) {
            
        //std::cout << "i: " << i << std::endl;
        //VertexID2 v1;
        //VertexID2 v2;
        if (isSgFunctionRefExp(path[i]->get_SgNode())) {
            SgFunctionDeclaration* sgfd = getAssociatedFunctionDeclaration(isSgFunctionRefExp(path[i]->get_SgNode()));
            SgFunctionDefinition* sgfdef = getAssociatedFunctionDefinition(sgfd);
            ROSE_ASSERT(isSgFunctionParameterList)path[i+1]->get_SgNode())
        //std::cout << "in while" << std::endl;
        if (isSgInitializedName(path[i]->get_SgNode()) /*&& knownNodes.find(path[i]->get_SgNode()) == knownNodes.end()*/) {
           // exprs.push_back(path[i]);
           //std::cout << "initialized name" << std::endl;
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
                 return;
           }
            //string ss = mainParse(exprPath);
            //ROSE_ASSERT(y1 != NULL);
            //std::cout << "exprPath.size(): " << exprPath.size() << std::endl;
            //std::cout << "k: " << k << std::endl;
            //std::cout << "i: " << i << std::endl;
            yices_expr yx = mainParse(exprPath, ctx);
            yices_assert(ctx, yx);
            //std::cout << "successful assert" << std::endl;
            //pathstream << ss;
      /*      if (knownGraphNodes.find(exprPath.front()) == knownGraphNodes.end()) {
                VertexID2 vt2 = boost::add_vertex(*nGraph);
                //graphVertex[path[i]] = vt2;
                (*nGraph)[vt2].vardec = true;
                (*nGraph)[vt2].varstr = ss;
                knownGraphNodes.insert(exprPath.front());
                graphVertex[exprPath.front()] = vt2;

            }*/
            //knownNodes.insert(path[i]->get_SgNode());
            //ROSE_ASSERT(graphVertex.find(path[i]) != graphVertex.end());
            //VertexID2 v1 = graphVertex[exprPath.front()];
            i += exprPath.size();
            /*if (knownGraphNodes.find(exprPath.back()) == knownGraphNodes.end()) {
                VertexID2 vt3 = boost::add_vertex(*nGraph);
                graphVertex[exprPath.back()] = vt3;
                knownGraphNodes.insert(exprPath.back());
            }*/
            /*std::pair<VertexID2, VertexID2> pr;
            VertexID2 v2 = graphVertex[exprPath.back()];
            pr.first = v1;
            pr.second = v2;
            if (knownEdges.find(pr) == knownEdges.end()) {
                EdgeID2 edge;
                bool ok;
                boost::tie(edge, ok) = boost::add_edge(v1, v2, *nGraph);
                ROSE_ASSERT(ok);
                knownEdges.insert(pr);
            }
           */
           k = 0;
            //ROSE_ASSERT(knownNodes.find((exprPath.back())->get_SgNode) != knownNodes.end());
           // knownNodes.insert(path[i]->get_SgNode());
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
                exprPath.push_back(path[j]);
                j++;
               // std::cout << "in second while" << std::endl;
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
            SgGraphNode* onn = (*(oeds.begin()))->get_to();
               
            
            ROSE_ASSERT(onn == path[j+1]);
            std::set<SgDirectedGraphEdge*> ifoeds = g->computeEdgeSetOut(path[j+1]); 
            if ((isSgForStatement(onn->get_SgNode()) || isSgIfStmt(onn->get_SgNode())) && ifoeds.size() >= 2) { 
                //std::cout << "got a for or if" << std::endl;

                //if (isSgForStatement(onn->get_SgNode())) {
                //    if (forsts.find(onn->get_SgNode()) != forsts.end()) {
                //        yices_retract(ctx,forsts[onn->get_SgNode()]);
                //    }
               // }
                //std::cout << "problem node: " << cfg->toCFGNode(path[j+2]).toString() << std::endl;
                CFGNode cn = cfg->toCFGNode(path[j+2]);
                std::vector<CFGEdge> ed = cn.inEdges();
                //ROSE_ASSERT(ed.size() == 1);
                int qw = 0;
                while (ed[qw].source() != cfg->toCFGNode(path[j+1])) {
                    qw++;
                } 
                CFGEdge needEdge =  ed[qw];
                EdgeConditionKind kn = needEdge.condition();
                ROSE_ASSERT(kn == eckTrue || kn == eckFalse);
                if (kn == eckFalse) {
                    //yices_expr y2n = yices_mk_fresh_bool_var(ctx);
                    yices_expr y2n = yices_mk_not(ctx, y2);

                   //if (isSgForStatement(onn->get_SgNode())) {
                  //      int yr = yices_assert_retractable(ctx, y2n);
                  //      forsts[onn->get_SgNode()] = yr;
                   // }
                   // else {
                   //     ROSE_ASSERT(y2n != NULL);
                        yices_assert(ctx, y2n);
                    //}

//                    yices_assert(ctx, y2n);
                    //if (yices_inconsistent(ctx)) {
                  //      std::cout << "inconsistent" << std::endl;
                  //      std::cout << "at: " << cfg->toCFGNode(path[i]).toString() << std::endl;
                    //}
                    //else {
                    //    std::cout << "consistent" << std::endl;
                    //}
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
                    //if (yices_inconsistent(ctx)) {
                      //  std::cout << "inconsistent" << std::endl;
                      //  std::cout << "at: " << cfg->toCFGNode(path[i]).toString() << std::endl;

                    //}
                    //else {
                      //  std::cout << "consistent" << std::endl;
                    //}

                   // }
                }
            }
            else {      
                yices_assert(ctx, y2);
            }
            //std::cout << "successful assert" << std::endl;
            //assertion_id aid = yices_assert_retractable(y2, ctx);
            
            //stringstream sts;
            //if (!noAssert) {
            //pathstream << "( assert " << ss << ")\n";
           // }
           // else {
           //     noAssert = false;
           // }
            //exprs[path[i]] = ss;
            //std::cout << "sts: " << sts.str() << std::endl;
            //knownNodes.insert(path[i]->get_SgNode());
           /* if (knownGraphNodes.find(exprPath.front()) == knownGraphNodes.end()) {
                VertexID2 vt2 = boost::add_vertex(*nGraph);
                //graphVertex[path[i]] = vt2;
                (*nGraph)[vt2].expr = true;
                (*nGraph)[vt2].exprstr = ss;
                graphVertex[exprPath.front()] = vt2;
                knownGraphNodes.insert(exprPath.front());
                VertexID2 vTest = graphVertex[exprPath.front()];
                //std::cout << (*nGraph)[vt2].exprstr << std::endl;
            }
*/
/*
            ROSE_ASSERT(graphVertex.find(exprPath.front()) != graphVertex.end());
            VertexID v1 = graphVertex[exprPath.front()];
*/          //std::cout << "next node: " << cfg->toCFGNode(path[i]).toString() << std::endl; 
            i += exprPath.size()+2;
/*
            if (knownGraphNodes.find(exprPath.back()) == knownGraphNodes.end()) {
                VertexID2 vt3 = boost::add_vertex(*nGraph);
                graphVertex[exprPath.back()] = vt3;
                knownGraphNodes.insert(exprPath.back());
            }
            std::pair<VertexID2, VertexID2> pr;
            ROSE_ASSERT(graphVertex.find(exprPath.back()) != graphVertex.end());
            VertexID v2 = graphVertex[exprPath.back()];
            pr.first = v1;
            pr.second = v2;
            if (knownEdges.find(pr) == knownEdges.end()) {
                bool ok;
                EdgeID2 edge;
                boost::tie(edge, ok) = boost::add_edge(v1, v2, *nGraph);
                ROSE_ASSERT(ok);
                knownEdges.insert(pr);
            }
*/           
            j = 0;

 
      }
        else {
/*
            //if (knownGraphNodes.find(path[i]) == knownGraphNodes.end()) {
                 if (knownGraphNodes.find(path[i-1]) == knownGraphNodes.end()) {
                     VertexID2 vi = boost::add_vertex(*nGraph);
                     knownGraphNodes.insert(path[i-1]);
                     graphVertex[path[i-1]] = intmap[vi];
                 }
                 ROSE_ASSERT(graphVertex.find(path[i-1]) != graphVertex.end());
                 if (knownGraphNodes.find(path[i]) == knownGraphNodes.end()) {
                     //std::cout << "i: " << i << std::endl;
                     VertexID2 vi1 = boost::add_vertex(*nGraph);
                     knownGraphNodes.insert(path[i]);
                     graphVertex[path[i]] = intmap[vi1];
                 }
                 ROSE_ASSERT(graphVertex.find(path[i]) != graphVertex.end());
           // }
            //if (i != 0) {
            int vi2 = graphVertex[path[i-1]]; 
            int vi3 = graphVertex[path[i]];
            std::pair<int, int> pr;
            pr.first = vi2;
            pr.second = vi3;
            if (knownEdges.find(pr) == knownEdges.end()) {
                 bool ok;
                 EdgeID2 edge;
                 boost::tie(edge, ok) = boost::add_edge(intvertmap[vi2], intvertmap[vi3], *nGraph);
                 ROSE_ASSERT(ok);
                 knownEdges.insert(pr);
            }
                   */
            i++;
        }
    //    std::cout << pathstream.str() << std::endl;
       // i += exprPath.size();
    
    }
    if (yices_inconsistent(ctx)) {
        //std::cout << "inconsistent path: " << ipaths << std::endl;
        ipaths++;
        //for (int i = 0; i < path.size(); i++) {
        //   std::cout <<  cfg->toCFGNode(path[i]).toStringForDebugging() << ", ";
       // }
        //std::cout << std::endl;
        inconsistent = false;
    }
    //std::cout << pathstream.str() << std::endl;
    yices_del_context(ctx);
}

StaticCFG::CFG* cfg;


std::vector<int> breakTriple(std::vector<SgGraphNode*> expr) {
    SgNode* index = expr[0]->get_SgNode();
    std::vector<int> bounds(3, 0);
    bounds[0] = 0;
    int i = 1;
    while (expr[i]->get_SgNode() != index) {
        int er = (int) expr.size();
        //std::cout << "expr[i]: " << cfg->toCFGNode(expr[i]).toString() << std::endl;
        ROSE_ASSERT(i < er);
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


yices_expr mainParse(vector<SgGraphNode*> expr, yices_context& ctx) {
    //std::cout << "rounds" << rounds << std::endl;
    rounds++;
    //bool yices = true;
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
        string empty;
        parsed= empty;
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
        std::vector<int> bounds = breakTriple(expr);
        for (int i = bounds[0]+1; i < bounds[1]; i++) {
            vec1.push_back(expr[i]);
        }
        for (int j = bounds[1]+1; j < bounds[2]; j++) {
            vec2.push_back(expr[j]);
        }
        //if (yices) {
                yices_expr e1 = yices_mk_fresh_bool_var(ctx);
                yices_expr e2 = yices_mk_fresh_bool_var(ctx);
                e1 = mainParse(vec1, ctx);
                e2 = mainParse(vec2, ctx);
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
        for (unsigned int i = 1; i < expr.size() - 1; i++) {
            vec1.push_back(expr[i]);
        }
        yices_expr e1 = mainParse(vec1, ctx);
        stringstream funN;
        funN << "V" << nvars;
        nvars++;
        //char* fun = (char*) funN.str().c_str();
        //yices_type ty = yices_mk_type(ctx, "int");
        //yices_var_decl vdecl = yices_mk_var_decl(ctx, fun, ty);
        //yices_expr e2 = yices_mk_var_from_decl(ctx, vdecl);
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
        SgName svs = isSgVarRefExp(expr[0]->get_SgNode())->get_symbol()->get_name();;
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
        string valType = isAtom((expr[2])->get_SgNode());
        unsigned int p = 2;
        SgName svs = (isSgInitializedName(expr[0]->get_SgNode()))->get_qualified_name();
        while (p < expr.size() && !isSgAssignInitializer(expr[p]->get_SgNode())) {
            vec1.push_back(expr[p]);
            p++;
        }
        stringstream funN;
        string ss;
        //if (nameOf.find(svs) != nameOf.end()) {
        //    ss = nameOf[svs];
        //}
        //else {
            funN << "V" << nvars;
            nameOf[svs] = funN.str();
            //ss = funN.str();
            nvars++;  
            //stst << "(declare-fun " << ss << " () " << valType << ")\n";
        //}i
        char* fun = (char*) funN.str().c_str();
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
        getExpr[svs] = e1;
        yices_expr e2 = mainParse(vec1, ctx);
        if (forFlag) {
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
        ROSE_ASSERT(isAtom(expr[3]->get_SgNode()) != "");
        string valType = isAtom(expr[3]->get_SgNode());
        std::vector<int> bounds = breakTriple(expr);
        //for (int i = bounds[0]+1; i < bounds[1]; i++) {
        //    vec1.push_back(expr[i]);
        //}
        SgName svs = (isSgVarRefExp((expr[1]->get_SgNode()))->get_symbol()->get_declaration()->get_qualified_name());
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
            yices_expr e1 = yices_mk_var_from_decl(ctx, decl1);
            yices_expr e2 = mainParse(vec2, ctx);
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
            //char* funC = fun.c_str();
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
        //unknown_flag = true;
        ret = yices_mk_fresh_bool_var(ctx);
        return ret;
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



int main(int argc, char *argv[]) {

  string fileSaver = "saviorStuff";
  ofstream fout;
    fout.open(fileSaver.c_str(),ios::app);

  //struct timeval t1, t2;
  SgProject* proj = frontend(argc,argv);
  ROSE_ASSERT (proj != NULL);


  Rose_STL_Container<SgNode*> functionDeclarationList = NodeQuery::querySubTree(proj,V_SgFunctionDeclaration);
  std::vector<SgNode*> forsA;
  //int counter = 0;
  for (Rose_STL_Container<SgNode*>::iterator i = functionDeclarationList.begin(); i != functionDeclarationList.end(); i++) {

          SgFunctionDeclaration* fni = isSgFunctionDeclaration(*i);
          ROSE_ASSERT(fni != NULL);
          forsA.push_back(fni);
  }
  for (unsigned int i = 0; i < forsA.size(); i++) {
      SgFunctionDeclaration* sfd = isSgFunctionDeclaration(forsA[i]);
      SgFunctionDefinition* sfdd = sfd->get_definition();
      //visitorTraversal* visInter = new visitorTraversal();
  
int counter = i;
   //std::cout << "counter: " << counter << std::endl;
   SgFunctionDefinition* fnc = isSgFunctionDefinition(sfdd);
   if (fnc != NULL) {
  stringstream ss;
  SgFunctionDeclaration* functionDeclaration = fnc->get_declaration();
  string fileName= functionDeclaration->get_name().str();//StringUtility::stripPathFromFileName(mainDef->get_file_info()->get_filenameString());
    string dotFileName1;
ss << fileName << "." << counter << ".dot";
    counter++;
    dotFileName1 = ss.str();
    //SgFunctionDefinition* fnc = functionDeclaration->get_definition();
    StaticCFG::InterproceduralCFG* cfg = new StaticCFG::InterproceduralCFG(fnc);
    SgIncidenceDirectedGraph* g = new SgIncidenceDirectedGraph();
     visitorTraversal* vis = new visitorTraversal();
    g = cfg->getGraph();
    myGraph* mg = new myGraph();
    mg = instantiateGraph(g, *cfg, fnc);
    vis->tltnodes = 0;
    vis->paths = 0;
    vis->orig = mg;
    vis->cfg = cfg;
    vis->g = g;
 //std::cout << dotFileName1 << std::endl;
 //cfgToDot(fnc,dotFileName1);
    //vis->firstPrepGraph(constcfg);
    //t1 = getCPUTime();
    vis->constructPathAnalyzer(mg, true, 0, 0, true);

    //t2 = getCPUTim
    if (ipaths > 0) {
    string fN = StringUtility::stripPathFromFileName(fnc->get_file_info()->get_filenameString());
    fout << "filename: " << fN << std::endl;
    fout << "function: " << fileName << std::endl;
    fout << "paths: " << vis->paths << std::endl;
    fout << "ipaths: " << ipaths << std::endl;
    }
    //delete vis;
   //delete cfg;
    //delete g;
    //delete mg;
    //std::cout << "took: " << timeDifference(t2, t1) << std::endl;
    }
    }



  SgFunctionDeclaration* mainDefDecl = SageInterface::findMain(proj);
  if (mainDefDecl != NULL) {
  SgFunctionDefinition* mainDef = mainDefDecl->get_definition();
   visitorTraversal* vis = new visitorTraversal();
    StaticCFG::InterproceduralCFG* cfg = new StaticCFG::InterproceduralCFG(mainDef);
    
   //cfg.buildFullCFG();
    stringstream ss;
    string fileName= StringUtility::stripPathFromFileName(mainDef->get_file_info()->get_filenameString());
    string dotFileName1=fileName+"."+ mainDef->get_declaration()->get_name() +".dot";

    //cfgToDot(mainDef,dotFileName1);
    //cfg->buildFullCFG();
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
    
    //vis->firstPrepGraph(constcfg);
    //t1 = getCPUTime();
    vis->constructPathAnalyzer(mg, true);
    //t2 = getCPUTime();
    //std::cout << "took: " << timeDifference(t2, t1) << std::endl;
    //cfg.clearNodesAndEdges(;
    if (ipaths > 0) {
    fout << "filename: " << fileName << std::endl;
    fout << "finished" << std::endl;
    fout << "tltnodes: " << vis->tltnodes << " paths: " << vis->paths << " ipaths: " << ipaths <<  std::endl;
   } 
   //delete vis;
    }
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

