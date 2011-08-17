/* 
 * File:   CustomFilteredCFG.h
 * Author: rahman2
 *
 * Created on July 20, 2011, 3:41 PM
 */

#ifndef CUSTOMFILTEREDCFG_H
#define CUSTOMFILTEREDCFG_H

#include "staticCFG.h"

namespace StaticCFG 
{

template <typename _Filter>
class CustomFilteredCFG : public CFG {
    
public:
        CustomFilteredCFG(SgNode *node) : CFG(node, true) {
        }
        ~CustomFilteredCFG() {
        }
        virtual void buildFilteredCFG();
        
        
protected:        
        virtual void printEdge(std::ostream & o, SgDirectedGraphEdge* edge, bool isInEdge) {
        
                AstAttribute* attr = edge->getAttribute("info");
            
                if (CFGEdgeAttribute<VirtualCFG::FilteredCFGEdge<_Filter> >* edge_attr = dynamic_cast<CFGEdgeAttribute<VirtualCFG::FilteredCFGEdge<_Filter> >*>(attr))  {
                        VirtualCFG::FilteredCFGEdge<_Filter>  e = edge_attr->getEdge();
                        o << e.source().id() << " -> " << e.target().id() << " [label=\"" << escapeString(e.toString()) <<
                        "\", style=\"" << (isInEdge ? "dotted" : "solid") << "\"];\n";
                }
                else
                        ROSE_ASSERT(false);

        }
private:        
        template <class NodeT, class EdgeT>
        void buildTemplatedCFG(NodeT n, std::map<NodeT, SgGraphNode*>& all_nodes, std::set<NodeT>& explored);

    };

}
#endif  /* CUSTOMFILTEREDCFG_H */

