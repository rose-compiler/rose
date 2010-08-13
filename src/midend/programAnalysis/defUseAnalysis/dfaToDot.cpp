// tps (1/14/2010) : Switching from rose.h to sage3 changed size from 18,5 MB to 8,1MB

#include "sage3basic.h"
#include "dfaToDot.h"
#include "DFAFilter.h"
#include "LivenessAnalysis.h"


using namespace std;



static string escapeStringDFA(const string& s) {
  string result;
  for (unsigned int i = 0; i < s.length(); ++i) {
    if (isprint(s[i]) && s[i] != '"' && s[i] != '\\') {
	//    if (isprint(s[i])) {
      result.push_back(s[i]);
    } else if (s[i] == '\a') {
      result += "\\a";
    } else if (s[i] == '\f') {
      result += "\\f";
    } else if (s[i] == '\n') {
      result += "\\n";
    } else if (s[i] == '\r') {
      result += "\\r";
    } else if (s[i] == '\t') {
      result += "\\t";
    } else if (s[i] == '\v') {
      result += "\\v";
    } else {
      ostringstream stream;
      stream << '\\';
      stream << setw(3) << setfill('0') << (unsigned int)(s[i]);
      result += stream.str();
    }
  }
  return result;
}


namespace VirtualCFG {

template<typename T>
std::string ToString(T t) {
  std::ostringstream myStream; //creates an ostringstream object
  myStream << t << std::flush;
  return(myStream.str()); //returns the string form of the stringstream object
}

  template <typename NodeT>
  std::string getNodeString(const NodeT& cfg, DefUseAnalysis* dfa) {
    std::string retStr = "";
    SgNode* n = cfg.getNode();
    retStr = " ( " + ToString(dfa->getIntForSgNode(n)) + " )  - [";
    retStr += RoseBin_support::ToString(n);
    retStr +="] ";
    SgInitializedName* init = isSgInitializedName(n);
    if (init) {
      retStr += " initVar : ";
      retStr += init->get_qualified_name().str();
    }
    SgVarRefExp* var = isSgVarRefExp(n);
    if (var) {
      retStr += " varRef : ";
      retStr += var->get_symbol()->get_name().str();
    }
    SgVariableDeclaration* varD = isSgVariableDeclaration(n);
    if (varD) {
      retStr += " varDecl : ";
      SgInitializedNamePtrList & list = varD->get_variables();
      SgInitializedNamePtrList::iterator it = list.begin();
      for (;it!=list.end();++it) {
	SgInitializedName* init = isSgInitializedName(*it);
	retStr += init->get_qualified_name().str();
	retStr += ",";
      }
    }
    return retStr;
  }

  template <typename EdgeT>
  std::string getEdgeString(const EdgeT& e, DefUseAnalysis* dfa,
			    LivenessAnalysis* live) {
    std::string retStr = "";
    SgNode* n = e.source().getNode();
    SgNode* t = e.target().getNode();

    if (live==NULL) {
      typedef std::vector < std::pair < SgInitializedName* , SgNode*>  > multitype;
      std::vector < std::pair <SgInitializedName*, SgNode*> > multi = dfa->getDefMultiMapFor(n);
      for (multitype::const_iterator j = multi.begin(); j != multi.end(); ++j) {
	SgInitializedName* sgInitMM = (*j).first;
	string name = sgInitMM->get_qualified_name().str() ;
	SgNode* sgNodeMM = (*j).second;
	string temp = "DEF: " +name + " ( " + ToString(dfa->getIntForSgNode(sgInitMM)) +
	  " )  - " + ToString(dfa->getIntForSgNode(sgNodeMM)) + " \n";
	retStr += temp;
      }

      multi = dfa->getUseMultiMapFor(n);
      for (multitype::const_iterator j = multi.begin(); j != multi.end(); ++j) {
	SgInitializedName* sgInitMM = (*j).first;
	string name = sgInitMM->get_qualified_name().str() ;
	SgNode* sgNodeMM = (*j).second;
	string temp = "USE: " +name + " ( " + ToString(dfa->getIntForSgNode(sgInitMM)) +
	  " )  - " + ToString(dfa->getIntForSgNode(sgNodeMM)) + " \n";
	retStr += temp;
      }
    } else {
      ROSE_ASSERT(n);
      ROSE_ASSERT(t);
      std::vector<SgInitializedName*> in = live->getIn(t);
      std::vector<SgInitializedName*> out = live->getOut(n);

      std::vector<SgInitializedName*>::iterator it = in.begin();
      std::string inS = "in : ";
      for (;it!=in.end();++it) {
	SgInitializedName* init = isSgInitializedName(*it);
	ROSE_ASSERT(init);
	inS += init->get_name().str();
	inS += ",";
      }

      it = out.begin();
      std::string outS = "out : ";
      for (;it!=out.end();++it) {
	SgInitializedName* init = isSgInitializedName(*it);
	ROSE_ASSERT(init);
	outS += init->get_name().str();
	outS += ",";
      }
      retStr = outS+"\n   visited : "+
	RoseBin_support::ToString(live->getVisited(n))+"\n"+inS+"\n";

      typedef std::vector < std::pair < SgInitializedName* , SgNode*>  > multitype;
      std::vector < std::pair <SgInitializedName*, SgNode*> > multi = dfa->getDefMultiMapFor(n);
      for (multitype::const_iterator j = multi.begin(); j != multi.end(); ++j) {
	SgInitializedName* sgInitMM = (*j).first;
	string name = sgInitMM->get_qualified_name().str() ;
	SgNode* sgNodeMM = (*j).second;
	string temp = "DEF: " +name + " ( " + ToString(dfa->getIntForSgNode(sgInitMM)) +
	  " )  - " + ToString(dfa->getIntForSgNode(sgNodeMM)) + " \n";
	if (sgNodeMM==n)
	  retStr += temp;
      }

      multi = dfa->getUseMultiMapFor(n);
      for (multitype::const_iterator j = multi.begin(); j != multi.end(); ++j) {
	SgInitializedName* sgInitMM = (*j).first;
	string name = sgInitMM->get_qualified_name().str() ;
	SgNode* sgNodeMM = (*j).second;
	string temp = "USE: " +name + " ( " + ToString(dfa->getIntForSgNode(sgInitMM)) +
	  " )  - " + ToString(dfa->getIntForSgNode(sgNodeMM)) + " \n";
	if (sgNodeMM==n)
	  retStr += temp;
      }

    }

    return retStr;
  }

  template <typename NodeT>
  inline void printDFANode(ostream& o, const NodeT& n, DefUseAnalysis* dfa) {
    string id = n.id();
    string nodeColor = "black";
    std::string funcName ="";
    if (isSgUnaryOp(n.getNode())) nodeColor = "orange";
    else if (isSgAssignInitializer(n.getNode())) nodeColor = "red";
    else if (isSgBinaryOp(n.getNode())) nodeColor = "red";
    else if (isSgFunctionCallExp(n.getNode())) nodeColor = "yellow";
    else if (isSgInitializedName(n.getNode())) nodeColor = "blue";
    else if (isSgVarRefExp(n.getNode())) nodeColor = "green";
    if (isSgFunctionDefinition(n.getNode())) {
    	funcName = isSgFunctionDefinition(n.getNode())->get_declaration()->get_qualified_name();
    }
    o << id << " [label=\"" << funcName << " : " << escapeStringDFA(getNodeString(n, dfa) + n.toStringForDebugging() ) <<
      "\", color=\"" << nodeColor <<
      "\", style=\"" << (n.isInteresting() ? "solid" : "dotted") << "\"];\n";
  }

  template <typename EdgeT>
  inline void printDFAEdge(ostream& o, const EdgeT& e, bool isInEdge, DefUseAnalysis* dfa,
                           LivenessAnalysis* live) {
    //cout << " printDFAEdge: label: " << getEdgeString(e) << endl;
    string nodeColor = "black";
    SgNode* ns = e.source().getNode();
    if (isSgIfStmt(ns)) {
      EdgeConditionKind kk = e.condition();
      if (kk==eckTrue)
	nodeColor = "green";
      if (kk==eckFalse)
	nodeColor = "red";
    }
    o << e.source().id() << " -> " << e.target().id() << " [label=\"" <<
      escapeString(getEdgeString(e, dfa, live) ) <<
      "\", color=\"" << nodeColor <<
      "\", style=\"" << (isInEdge ? "dotted" : "solid") << "\"];\n";
  }


  template <typename NodeT, typename EdgeT>
  void printDFANodePlusEdges(ostream& o, NodeT n, DefUseAnalysis* dfa,
                             LivenessAnalysis* live) {
    printDFANode<NodeT>(o, n, dfa);
    vector<EdgeT> outEdges = n.outEdges();
    for (unsigned int i = 0; i < outEdges.size(); ++i) {
      printDFAEdge<EdgeT>(o, outEdges[i], false, dfa, live);
    }
      if (/* Debug */ isSgFunctionCallExp(n.getNode()) && n.getIndex() == 3) {
	vector<EdgeT> inEdges = n.inEdges();
	for (unsigned int i = 0; i < inEdges.size(); ++i) {
	  printDFAEdge<EdgeT>(o, inEdges[i], false, dfa, live);
	}
	printDFAEdge<EdgeT>(o, CFGPath(CFGEdge(CFGNode(n.getNode(), 2), n.toNode())), false, dfa, live);
      }
  }

  template <typename NodeT, typename EdgeT>
  class DfaToDotImpl {
    multimap<SgNode*, NodeT> exploredNodes;
    set<SgNode*> nodesPrinted;
    ostream& o;
    DefUseAnalysis* dfa;
    LivenessAnalysis* live;

  public:
    DfaToDotImpl(ostream& o, DefUseAnalysis* dfa_p, LivenessAnalysis* live_p):
      exploredNodes(), nodesPrinted(), o(o), dfa(dfa_p), live(live_p) {}
    void explore(NodeT n);
    void processNodes(SgNode* top);
  };

  template <typename NodeT, typename EdgeT>
  void DfaToDotImpl<NodeT, EdgeT>::explore(NodeT n) {
    ROSE_ASSERT (n.getNode());
    //std::cerr << n.toStringForDebugging() << std::endl;
    pair<typename multimap<SgNode*, NodeT>::const_iterator,
      typename multimap<SgNode*, NodeT>::const_iterator> ip = exploredNodes.equal_range(n.getNode());
    for (typename multimap<SgNode*, NodeT>::const_iterator i = ip.first; i != ip.second; ++i) {
      if (i->second == n) return;
    }
    exploredNodes.insert(make_pair(n.getNode(), n));
    vector<EdgeT> outEdges = n.outEdges();
    for (unsigned int i = 0; i < outEdges.size(); ++i) {
      ROSE_ASSERT (outEdges[i].source() == n);
      explore(outEdges[i].target());
    }
    vector<EdgeT> inEdges = n.inEdges();
    for (unsigned int i = 0; i < inEdges.size(); ++i) {
      ROSE_ASSERT (inEdges[i].target() == n);
      explore(inEdges[i].source());
    }
    if (getenv("BUGGY_INTERPROCEDURAL_CFG") && isSgFunctionCallExp(n.getNode()) && n.getIndex() == 2) explore(CFGNode(n.getNode(), 3));
  }

  template <typename NodeT, typename EdgeT>
  void DfaToDotImpl<NodeT, EdgeT>::processNodes(SgNode*) {
    for (typename multimap<SgNode*, NodeT>::const_iterator it = exploredNodes.begin();
         it != exploredNodes.end(); ++it) {
      printDFANodePlusEdges<NodeT, EdgeT>(o, it->second, dfa, live);
    }
  }


  template < typename FilterFunction >
  std::ostream & dfaToDot(std::ostream & o,
			  std::string graphName,
			  std::vector < FilteredCFGNode < FilterFunction > > start,
			  DefUseAnalysis* dfa) {
    o << "digraph " << graphName << " {\n";
    DfaToDotImpl < FilteredCFGNode < FilterFunction >,
      FilteredCFGEdge < FilterFunction > > impl(o, dfa, NULL);
    typename  std::vector < FilteredCFGNode < FilterFunction > >::const_iterator it;
    for (it=start.begin(); it!=start.end(); ++it) {
      o << "// Writing graph for " << (*it).toStringForDebugging() << std::endl;
      FilteredCFGNode < FilterFunction> cfgnode = *it;
      impl.explore(cfgnode);
    }
    impl.processNodes(0);
    o << "}\n";
    return o;
  }

  template < typename FilterFunction >
  std::ostream & dfaToDot(std::ostream & o,
			  std::string graphName,
			  std::vector < FilteredCFGNode < FilterFunction > > start,
			  DefUseAnalysis* dfa,
			  LivenessAnalysis* live) {
    o << "digraph " << graphName << " {\n";
    DfaToDotImpl < FilteredCFGNode < FilterFunction >,
      FilteredCFGEdge < FilterFunction > > impl(o, dfa, live);
    typename  std::vector < FilteredCFGNode < FilterFunction > >::const_iterator it;
    for (it=start.begin(); it!=start.end(); ++it) {
      o << "// Writing graph for " << (*it).toStringForDebugging() << std::endl;
      FilteredCFGNode < FilterFunction> cfgnode = *it;
      impl.explore(cfgnode);
    }
    impl.processNodes(0);
    o << "}\n";
    return o;
  }


  template std::ostream& dfaToDot(std::ostream&, std::string,
  				  std::vector <FilteredCFGNode<IsDFAFilter> >,
  				  DefUseAnalysis* dfa);
  template std::ostream& dfaToDot(std::ostream&, std::string,
  				  std::vector <FilteredCFGNode<IsDFAFilter> >,
  				  DefUseAnalysis* dfa,
				  LivenessAnalysis* live);
}
