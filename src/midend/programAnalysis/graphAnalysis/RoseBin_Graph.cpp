/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 27 Sep07
 * Decription : DotGraph Visualization
 ****************************************************/
// tps (1/14/2010) : Switching from rose.h to sage3 changed size from 17,3 MB to 6,9MB
#include "sage3basic.h"
#include "RoseBin_Graph.h"

using namespace std;




void
RoseBin_Graph::createUniqueEdges() {
  //SgGraphEdgeList* gedges = get_edges();
  rose_graph_integer_edge_hash_multimap edges =get_node_index_to_edge_multimap_edgesOut();
  if (unique_edges.size()==0) {
    //cerr << " Creating unique edge map. Edges : " << RoseBin_support::ToString(edges.size()) << endl;
    rose_graph_integer_edge_hash_multimap::const_iterator it2 = edges.begin();
    for (;it2!=edges.end();it2++) {
      SgDirectedGraphEdge* edgeIt = isSgDirectedGraphEdge(it2->second);
      SgGraphNode* from = isSgGraphNode(edgeIt->get_from());
      SgGraphNode* to = isSgGraphNode(edgeIt->get_to());

      rose_graph_integer_edge_hash_multimap::iterator it = unique_edges.find(from->get_index());
      if (it==unique_edges.end())
        unique_edges.insert(pair<int,SgDirectedGraphEdge*>( from->get_index(), edgeIt)) ;
      else {
        bool found=false;
        while (it!=unique_edges.end()) {
          SgDirectedGraphEdge* edge = isSgDirectedGraphEdge(it->second);
          SgGraphNode* source = isSgGraphNode(edge->get_from());
          SgGraphNode* target = isSgGraphNode(edge->get_to());
          if (target==to && source==from)
            found=true;
          it++;
          if (source!=from)
            break;
        }
        if (!found)
          unique_edges.insert(pair<int,SgDirectedGraphEdge*>( from->get_index(), edgeIt)) ;
      } // else
    } // for
  } // if
}











