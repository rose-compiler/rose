#include "DataflowCFG.h"

using namespace std;

#define SgNULL_FILE Sg_File_Info::generateDefaultFileInfoForTransformationNode()

namespace VirtualCFG {
        
        std::string DataflowNode::str(std::string indent) const
        {
                ostringstream outs;
                outs << "<" << getNode() << " | " << getNode()->class_name() << " | " << getNode()->unparseToString() << " | " << getIndex() << ">";
                return outs.str();
        }

  // XXX: This code is duplicated from frontend/SageIII/virtualCFG/virtualCFG.C
  vector<DataflowEdge> makeClosureDF(const vector<CFGEdge>& orig,
                                      vector<CFGEdge> (CFGNode::*closure)() const,
                                      CFGNode (CFGPath::*otherSide)() const,
                                      CFGPath (*merge)(const CFGPath&, const CFGPath&)) {
    vector<CFGPath> rawEdges(orig.begin(), orig.end());
    // cerr << "makeClosure starting with " << orig.size() << endl;
    while (true) {
      top:
      // cerr << "makeClosure loop: " << rawEdges.size() << endl;
      
// CH (5/27/2010): 'push_back' may invalidate iterators of a vector.
// Using index instead to fix this subtle bug.
#if 0 
      for (vector<CFGPath>::iterator i = rawEdges.begin(); i != rawEdges.end(); ++i) {
        if (!((*i).*otherSide)().isInteresting()) {
          unsigned int oldSize = rawEdges.size();
          vector<CFGEdge> rawEdges2 = (((*i).*otherSide)().*closure)();
          for (unsigned int j = 0; j < rawEdges2.size(); ++j) {
            CFGPath merged = (*merge)(*i, rawEdges2[j]);
            if (std::find(rawEdges.begin(), rawEdges.end(), merged) == rawEdges.end()) {
              rawEdges.push_back(merged);
            }
          }
          if (rawEdges.size() != oldSize) goto top; // To restart iteration
        }
      }
#else
      for (size_t i = 0; i < rawEdges.size(); ++i) {
        if (!(rawEdges[i].*otherSide)().isInteresting()) {
          unsigned int oldSize = rawEdges.size();
          vector<CFGEdge> rawEdges2 = ((rawEdges[i].*otherSide)().*closure)();
          for (unsigned int j = 0; j < rawEdges2.size(); ++j) {
            CFGPath merged = (*merge)(rawEdges[i], rawEdges2[j]);
            if (std::find(rawEdges.begin(), rawEdges.end(), merged) == rawEdges.end()) {
              rawEdges.push_back(merged);
            }
          }
          if (rawEdges.size() != oldSize) goto top; // To restart iteration
        }
      }
#endif
      break; // If the iteration got all of the way through
    }
    // cerr << "makeClosure loop done: " << rawEdges.size() << endl;
    vector<DataflowEdge> edges;
    for (vector<CFGPath>::const_iterator i = rawEdges.begin(); i != rawEdges.end(); ++i) {
      if (((*i).*otherSide)().isInteresting())
        edges.push_back(DataflowEdge(*i));
    }
    //cout << "makeClosure done: #edges=" << edges.size() << endl;
    //for(vector<DataflowEdge>::iterator e=edges.begin(); e!=edges.end(); e++)
    //    printf("Current Node %p<%s | %s>\n", e.target().getNode(), e.target().getNode()->unparseToString().c_str(), e.target().getNode()->class_name().c_str());
    return edges;
  }
        
        vector<DataflowEdge> DataflowNode::outEdges() const {
                return makeClosureDF(n.outEdges(), &CFGNode::outEdges, &CFGPath::target, &mergePaths);
        }
        
        vector<DataflowEdge> DataflowNode::inEdges() const {
                return makeClosureDF(n.inEdges(), &CFGNode::inEdges, &CFGPath::source, &mergePathsReversed);
        }

        bool DataflowNode::isInteresting() const {
                return (n.getNode())->cfgIsIndexInteresting(n.getIndex());
                //return isDataflowInteresting(n);
        }
        
        bool isDataflowInteresting(CFGNode cn) {
                ROSE_ASSERT (cn.getNode());
                return (cn.getNode()->cfgIsIndexInteresting(cn.getIndex()) && 
                       //!isSgFunctionRefExp(cn.getNode()) &&
                       !isSgExprListExp(cn.getNode()) &&
                       !isSgForInitStatement(cn.getNode()) &&
                       //!isSgVarRefExp(cn.getNode()) &&
                       //!isSgValueExp(cn.getNode()) &&
                       //!isSgExprStatement(cn.getNode()) &&
                       !(isSgInitializedName(cn.getNode()) && cn.getIndex()==0)) 
                       ||
                       (isSgIfStmt(cn.getNode()) &&
                        cn.getIndex()==1 || cn.getIndex()==2);
        }
        
}
