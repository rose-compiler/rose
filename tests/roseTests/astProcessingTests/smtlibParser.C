#include <iostream>
#include <fstream>
//#include <rose.h>
#include <string>
#ifndef _MSC_VER
#include <err.h>
#endif
#include "SgGraphTemplate.h"
#include "graphProcessing.h"
#include "staticCFG.h"
/* Testing the graph traversal mechanism now implementing in AstProcessing.h (inside src/midend/astProcessing/)*/


using namespace std;
using namespace boost;
using namespace rose;



int nvars;
std::map<SgName, string> nameOf;
bool noAssert;

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


class visitorTraversal : public SgGraphTraversal<CFGforT>
   {
     public:
          int tltnodes;
          int paths;
          //std::set<SgNode*> knownNodes;     
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


string mainParse(vector<SgGraphNode*> expr);

int rounds;
int pathnum;
//std::set<SgGraphNode*> knownGraphNodes;
std::set<std::pair<VertexID2, VertexID2> > knownEdges;
std::map<SgGraphNode*, VertexID2> graphVertex;

void visitorTraversal2::analyzePath(std::vector<VertexID2>& pathR) {
    tltnodes += pathR.size();
    paths++;
    std::cout << "path: " << paths << std::endl;
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
           // const newGraph* gc = g;
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


void visitorTraversal::analyzePath(std::vector<VertexID>& pathR) {
    unsigned int i = 0;
   paths++; 
    noAssert = false;
    rounds = 0;
    stringstream pathstream;
    std::set<SgNode*> knownNodes;
    //VertexID2 start = boost::add_vertex(*nGraph);
    //graphVertex[(*orig)[pathR[0]]] = start;
    std::cout << "path: " << pathnum << std::endl;
    pathnum++;
    std::vector<SgGraphNode*> path;
    //std::vector<SgGraphNode*> pathR;
    std::vector<SgGraphNode*> exprPath;
    for (unsigned int j = 0; j < pathR.size(); j++) {
       SgGraphNode* R = getGraphNode[pathR[j]];
       path.push_back(R);
    }
    //graphVertex[path[0]] = start;
    while (i < path.size()) {
        exprPath.clear(); 
        //VertexID2 v1;
        //VertexID2 v2;
        //std::cout << "in while" << std::endl;
        if (isSgInitializedName(path[i]->get_SgNode()) && knownNodes.find(path[i]->get_SgNode()) == knownNodes.end()) {
           // exprs.push_back(path[i]);
            exprPath.push_back(path[i]);
            unsigned int k = i+1;
            while (k < path.size() && (!isSgInitializedName(path[k]->get_SgNode()) || path[k]->get_SgNode() != path[i]->get_SgNode())) {
                exprPath.push_back(path[k]);
                k++;
            }
            exprPath.push_back(path[k]);
            string ss = mainParse(exprPath);
            pathstream << ss;
      /*      if (knownGraphNodes.find(exprPath.front()) == knownGraphNodes.end()) {
                VertexID2 vt2 = boost::add_vertex(*nGraph);
                //graphVertex[path[i]] = vt2;
                (*nGraph)[vt2].vardec = true;
                (*nGraph)[vt2].varstr = ss;
                knownGraphNodes.insert(exprPath.front());
                graphVertex[exprPath.front()] = vt2;

            }*/
            knownNodes.insert(path[i]->get_SgNode());
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
        else if (isSgExprStatement(path[i]->get_SgNode()) && knownNodes.find(path[i]->get_SgNode()) == knownNodes.end()) {
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
		    //exprPath.push_back(path[j]);
		   // i += exprPath.size();
            //std::vector<SgGraphNode*> exprP;
            //for (int q = 1; q < exprPath.size()-1; q++) {
            //    exprP.push_back(exprPath[q]);
           // }
            string ss = mainParse(exprPath);
            //stringstream sts;
            if (!noAssert) {
            pathstream << "( assert " << ss << ")\n";
            }
            else {
                noAssert = false;
            }
            //exprs[path[i]] = ss;
            //std::cout << "sts: " << sts.str() << std::endl;
            knownNodes.insert(path[i]->get_SgNode());
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
*/            
            i += exprPath.size()-1;
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
    std::cout << pathstream.str() << std::endl;
}

StaticCFG::CFG* cfg;


std::vector<int> breakTriple(std::vector<SgGraphNode*> expr) {
    SgNode* index = expr[0]->get_SgNode();
    std::vector<int> bounds(3, 0);
    bounds[0] = 0;
    unsigned int i = 1;
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

string mainParse(vector<SgGraphNode*> expr) {
    //std::cout << "rounds" << rounds << std::endl;
    rounds++;
    std::stringstream stst;
    string parsed;
    bool unknown_flag = false;
    std::vector<SgGraphNode*> vec1;
    std::vector<SgGraphNode*> vec2;
    stringstream ss;
    if (expr.size() == 0) {
        string empty;
        return empty;
    }
    if (unknown_flag) {
        string empty;
        parsed= empty;
    }
    else if (isLogicalSplit(expr[0]->get_SgNode())) {
        string ls = getLogicalSplit(expr[0]->get_SgNode());
        std::vector<int> bounds = breakTriple(expr);
        for (int i = bounds[0]+1; i < bounds[1]; i++) {
            vec1.push_back(expr[i]);
        }
        for (int j = bounds[1]+1; j < bounds[2]; j++) {
            vec2.push_back(expr[j]);
        }
        stst << "( "<< ls << " " << mainParse(vec1) << " " << mainParse(vec2) << ")";
        parsed = stst.str();
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
        if (isBinaryLogicOp(expr[0]->get_SgNode())) {
            stst << "( " <<  parsed << " " << mainParse(vec1) << " " << mainParse(vec2) << ")";
        }
        else {
            stst << "( " << parsed << " " << mainParse(vec1) << " " << mainParse(vec2) << " ) ";
        }
        parsed = stst.str();
    }
    else if (isAtom(expr[0]->get_SgNode()) != "") {
        string ty = isAtom(expr[0]->get_SgNode());
        if (ty == "int") {
            int ival = isSgIntVal(expr[0]->get_SgNode())->get_value();
            ss << ival;
            parsed = ss.str();
        }
        else if (ty == "double") {
            double dval = isSgDoubleVal(expr[0]->get_SgNode())->get_value();
            ss << dval;
            parsed = ss.str();    
        }
        else if (ty == "float") {
           float fval = isSgFloatVal(expr[0]->get_SgNode())->get_value();
           ss << fval;
           parsed =  ss.str();
        }
        else if (ty == "short") {
           short sval = isSgShortVal(expr[0]->get_SgNode())->get_value();
           ss << sval;
           parsed =  ss.str();
        }
        else if (ty == "long") {
            long lval = isSgLongIntVal(expr[0]->get_SgNode())->get_value();
            ss << lval;
            parsed = ss.str();
        }
        else if (ty == "long long int") {
            long long llval = isSgLongLongIntVal(expr[0]->get_SgNode())->get_value();
            ss << llval;
            parsed = ss.str();
        }
        else if (ty == "long double") {
            long double lldval = isSgLongDoubleVal(expr[0]->get_SgNode())->get_value();
            ss << lldval;
            parsed =  ss.str();
        }
        else if (ty == "bool") {
            bool bval = isSgBoolValExp(expr[0]->get_SgNode())->get_value();
            if (bval == true) {
                parsed = "true";
            }
            else {
                parsed = "false";
            }
        }
        else {
            cout << "unsupported atomic type";
            ROSE_ASSERT(false);
        }
    }
    else if (isSgVarRefExp((expr[0])->get_SgNode())) {
        SgName svs = isSgVarRefExp(expr[0]->get_SgNode())->get_symbol()->get_name();;
        stringstream ss;
        if (nameOf.find(svs) != nameOf.end()) {
            parsed = nameOf[svs];
        }
        else {
            ss <<  "V" << nvars;
            nvars++;
            nameOf[svs] = ss.str();
            parsed = nameOf[svs];
        }
    }
    else if (isSgInitializedName(expr[0]->get_SgNode())) {
        stringstream stst;
        std::vector<SgGraphNode*> vec1;
        ROSE_ASSERT(isAtom((expr[2])->get_SgNode()) != "");
        string valType = isAtom((expr[2])->get_SgNode());
        int p = 2;
        SgName svs = (isSgInitializedName(expr[0]->get_SgNode()))->get_qualified_name();
        while (!isSgAssignInitializer(expr[p]->get_SgNode())) {
            vec1.push_back(expr[p]);
            p++;
        }
        stringstream funN;
        string ss;
        if (nameOf.find(svs) != nameOf.end()) {
            ss = nameOf[svs];
        }
        else {
            funN << "V" << nvars;
            nameOf[svs] = funN.str();
            ss = funN.str();
            nvars++;  
            stst << "(declare-fun " << ss << " () " << valType << ")\n";
        }
        stst << "(let (" << ss << " " << mainParse(vec1) << ")\n";
        parsed =  stst.str();
    }
    else if (isSgAssignOp(expr[0]->get_SgNode())) {
        stringstream stst;
        ROSE_ASSERT(isSgVarRefExp(expr[1]->get_SgNode()));
        std::vector<int> bounds = breakTriple(expr);
        for (int i = bounds[0]+1; i < bounds[1]; i++) {
            vec1.push_back(expr[i]);
        }
        for (int j = bounds[1]+1; j < bounds[2]; j++) {
            vec2.push_back(expr[j]);
        }
        //stringstream stst;
        noAssert = true;
        stst << "(let (" << mainParse(vec1) << " " << mainParse(vec2) << ") )";
        parsed = stst.str();
    }
    else {
        cout << "unknown type" << endl;
        cout << cfg->toCFGNode(expr[0]).toString() << std::endl;
        unknown_flag = true;
    }
    //std::cout << "parsed: " << parsed << std::endl;
    return parsed;   
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
        cout << "not a logicalSplit Operator" << std::endl;
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
        std::cout << "bad eqOp" << std::endl;
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
        std::cout << "unknown op in getBinaryOp" << std::endl;
        ROSE_ASSERT(false);
    }
    return ss;
}




int main(int argc, char *argv[]) {

  //struct timeval t1, t2;
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
    vis->orig = mg;
    vis->cfg = &cfg;
    //vis->firstPrepGraph(constcfg);
    //t1 = getCPUTime();
    vis->constructPathAnalyzer(mg, true);
    //t2 = getCPUTime();
    //std::cout << "took: " << timeDifference(t2, t1) << std::endl;
    //cfg.clearNodesAndEdges();
    std::cout << "finished" << std::endl;
    std::cout << "tltnodes: " << vis->tltnodes << " paths: " << vis->paths << std::endl;
    delete vis;
    //return 1;
}
/*

int main(int argc, char *argv[]) {
  pathnum = 0;
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
    //cfg1.buildFullCFG();
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
    
    vis->constructPathAnalyzer(mg);
    std::cout << "constructed" << std::endl;
   
   // printHotness2(nGraph);
   // std::cout << "mapped" << std::endl;i
   std::vector<std::vector<int> > pts;
   std::vector<int> ptsP;
   // std::vector<SgExpressionStmt*> exprs = SageInterface::querySubTree<SgExpressionStmt>(proj);
   for (int q1 = 0; q1 < exprs.size(); q1++) {
      ptsP.clear();
      for (int q2 = 0; q2 < exprs.size(); q2++) {
          if (q1 != q2) {
              vis->paths = 0;
              vis->tltnodes = 0;
              vis->constructPathAnalyzer(mg, false,exprs[q1], exprs[q2]);
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
    //cfg.clearNodesAndEdges();
    std::cout << "finished" << std::endl;
    std::cout << "tltnodes: " << vis->tltnodes << " paths: " << vis->paths << std::endl;
    delete vis;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                            
*/
/*
std::vector<SgExprStatement*> exprList = SageInterface::querySubTree<SgExprStatement>(project);
for (Rose_STL_Container<SgGraphNode*>::iterator i = exprList.begin(); i != exprList.end(); i++) {
*/

//          SgExprStatement* expr = isSgExprStatement(*i);

