/******************************************************************************
 *
 * DOT Graph output, similar to DOTGraphRepresentation
 * also supports subgraph clustering - inc constrast to above it
 * does buffer the graph information until writeDotoutStream is 
 * called
 *
 *****************************************************************************/
// Author: Markus Schordan, Nils Thuerey
// $Id: DOTSubgraphRepresentation.C,v 1.6 2008/01/08 02:56:19 dquinlan Exp $

#include <iostream>
#include <fstream>
#include <sstream>
#include <typeinfo>
#include <assert.h>
#include "sage3basic.h"
#include <inttypes.h>
#include "DOTSubgraphRepresentation.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

template<class NodeType>
DOTSubgraphRepresentation<NodeType>::DOTSubgraphRepresentation() {
   dotout = new ostringstream();
   assert(dotout != NULL);
	mSubgraphIds = -1;
	mAutoSubgraphIds   = false;
	mManualSubgraphIds = false;
}

template<class NodeType>
DOTSubgraphRepresentation<NodeType>::~DOTSubgraphRepresentation() {
  // DQ (8/14/2004): 
  // This fixes an long outstanding bug in the use of the DOT interface classes with Qing's graph 
  // interface. The DOTSubgraphRepresentation<> class does not have a dotout data member
  // the DOTRepresentation<> (the base class) does have a dotout data member and direived class
  // should not delete the base class's data member.  It would work if we had set the pointer to
  // NULL after deleting it (a habit that I very much like and which would have avoided the
  // bug, but it is better that the derived class not delete the base class data members.
  // delete dotout;
}

template<class NodeType>
void
DOTSubgraphRepresentation<NodeType>::resetWriteFlags( void )
   {
  // clear flags
     for (typename vector<NodeStorageType>::iterator i=mNodes.begin(); i!=mNodes.end(); i++)
        {
          (*i).written = false;
        }
     for (typename vector<EdgeStorageType>::iterator i=mEdges.begin(); i!=mEdges.end(); i++)
        {
          (*i).written = false;
        }
     for (typename vector<NodeAdditionalStorage>::iterator i=mAdds.begin(); i!=mAdds.end(); i++)
        {
          (*i).written = false;
        }
   }


template<class NodeType>
void
DOTSubgraphRepresentation<NodeType>::writeOutSubgraph( typename map<int,string>::iterator i )
   {
     int sid             = (*i).first;
     string internalName = (*i).second;

  // default look for subgraphs
  // char *defaultSubgraphStyle =  " color=lightgrey; ";
	  string defaultSubgraphStyle =  " color=blue; ";

     printf ("Handling subgraph %s ... (sid=%d) \n",internalName.c_str(),sid);

  // remove possible . from filenames
     for (size_t l=0; l<internalName.length(); l++)
        {
          if (internalName[l] == '.')
             {
               printf ("replacing . with _ in %s \n",internalName.c_str());
               internalName[l] = '_';
             }
        }

     (*dotout) << "subgraph " << "cluster_"<<sid <<" { " << endl;	
     (*dotout) << "label=\"" << (*i).second << "\"; " << endl;
  // (*dotout) << "color=lightgrey; " << endl;
     (*dotout) << defaultSubgraphStyle << endl;
  // write all components of this subgraph
     for (typename vector<NodeStorageType>::iterator i=mNodes.begin(); i!=mNodes.end(); i++)
        {
          if ( (*i).subgraph == sid )
             {
               (*dotout) << this->nodeName( (*i).node ) << " [label=\"BLA" <<  (*i).label  << "\" " <<  (*i).option  << "];" << endl;
               (*i).written = true;
             }
        }
     for (typename vector<EdgeStorageType>::iterator i=mEdges.begin(); i!=mEdges.end(); i++)
        {
       // check if both nodes are in this subgraph
          bool isinthis = true;
          for (typename vector<NodeStorageType>::iterator j=mNodes.begin(); j!=mNodes.end(); j++)
             {
               if ( (*j).node == (*i).node1 )
                  {
                    if ((*j).subgraph != sid) isinthis = false;
                  }
             }

          for (typename vector<NodeStorageType>::iterator j=mNodes.begin(); j!=mNodes.end(); j++)
             {
               if ( (*j).node == (*i).node2 )
                  {
                    if ((*j).subgraph != sid) isinthis = false;
                  }
             }

          if( isinthis  )
             {
               (*dotout) << this->nodeName( (*i).node1 )
                         << " -> "
                         << this->nodeName( (*i).node2 )
                         << " [label=\"" <<  (*i).label << "\" " <<  (*i).option  << " ];" << endl;
               (*i).written = true;
             }

          for (typename vector<NodeAdditionalStorage>::iterator i=mAdds.begin(); i!=mAdds.end(); i++)
             {
            // check if the node is in this subgraph
               bool isinthis = true;
               for (typename vector<NodeStorageType>::iterator j=mNodes.begin(); j!=mNodes.end(); j++)
                  {
                    if ( (*j).node == (*i).node )
                       {
                         if ((*j).subgraph != sid) isinthis = false;
                       }
                  }
               if ( isinthis  )
                  {
                    (*dotout) << (*i).add;
                    (*i).written = true;
                  }
             }
        }

       // end this subgraph
     (*dotout) << "} " << endl;
   }

// This is the old code which is being refactored to support nested subgraphs
template<class NodeType>
void
DOTSubgraphRepresentation<NodeType>::writeDotoutStream( void )
   {
  // default look for subgraphs
  // char *defaultSubgraphStyle =  " color=lightgrey; ";
	  string defaultSubgraphStyle =  " color=red; ";

  // clear flags
     resetWriteFlags();

     printf ("DOTSubgraphRepresentation<NodeType>::writeDotoutStream(): mSubgraphNames.size() = %" PRIuPTR " \n",mSubgraphNames.size());

	// write normally if there are no subgraphs
     if (mSubgraphNames.size() == 0) 
        {
       // write nodes
          for (typename vector<NodeStorageType>::iterator i=mNodes.begin(); i!=mNodes.end(); i++)
             {
               (*dotout) << this->nodeName( (*i).node ) << "[label=\"" <<  (*i).label  << "\" " <<  (*i).option  << "];" << endl;
               (*i).written = true;
             }
       // write edges
          for (typename vector<EdgeStorageType>::iterator i=mEdges.begin(); i!=mEdges.end(); i++)
             {
               (*dotout) << this->nodeName( (*i).node1 )
                         << " -> "
                         << this->nodeName( (*i).node2 )
                         << "[label=\"" <<  (*i).label << "\" " <<  (*i).option  << " ];" << endl;
               (*i).written = true;
             }

          for (typename vector<NodeAdditionalStorage>::iterator i=mAdds.begin(); i!=mAdds.end(); i++)
             {
               (*dotout) << (*i).add;
               (*i).written = true;
             }

        }
       else
        {
       // print each sbugraph, then other stuff
          for(typename map<int,string>::iterator i=mSubgraphNames.begin(); i!=mSubgraphNames.end(); i++)
             {
			int sid = (*i).first;

         printf ("Handling subgraph ... (sid=%d) \n",sid);

			// remove possible . from filenames
			string internalName = (*i).second;
			for(size_t l=0; l<internalName.length(); l++) {
				if(internalName[l]=='.') internalName[l] = '_';
			}

			(*dotout) << "subgraph " << "cluster_"<<sid <<" { " << endl;	
			(*dotout) << "label=\"" << (*i).second << "\"; " << endl;
      // (*dotout) << "color=lightgrey; " << endl;
         (*dotout) << defaultSubgraphStyle << endl;
			// write all components of this subgraph
			for(typename vector<NodeStorageType>::iterator i=mNodes.begin(); i!=mNodes.end(); i++) {
				if( (*i).subgraph == sid ) {
				(*dotout) << this->nodeName( (*i).node ) << " [label=\"BLA" <<  (*i).label  << "\" " <<  (*i).option  << "];" << endl;
				(*i).written = true; }
			}
			for(typename vector<EdgeStorageType>::iterator i=mEdges.begin(); i!=mEdges.end(); i++) {
				// check if both nodes are in this subgraph
				bool isinthis = true;
				for(typename vector<NodeStorageType>::iterator j=mNodes.begin(); j!=mNodes.end(); j++) {
					if( (*j).node == (*i).node1 ) {
						if((*j).subgraph != sid) isinthis = false; } 
				}
				for(typename vector<NodeStorageType>::iterator j=mNodes.begin(); j!=mNodes.end(); j++) {
					if( (*j).node == (*i).node2 ) {
						if((*j).subgraph != sid) isinthis = false; } 
				}

				if( isinthis  ) {
				(*dotout) << this->nodeName( (*i).node1 )
					<< " -> "
					<< this->nodeName( (*i).node2 )
					<< " [label=\"" <<  (*i).label << "\" " <<  (*i).option  << " ];" << endl;
				(*i).written = true; }
			}
			for(typename vector<NodeAdditionalStorage>::iterator i=mAdds.begin(); i!=mAdds.end(); i++) {
				// check if the node is in this subgraph
				bool isinthis = true;
				for(typename vector<NodeStorageType>::iterator j=mNodes.begin(); j!=mNodes.end(); j++) {
					if( (*j).node == (*i).node ) {
						if((*j).subgraph != sid) isinthis = false; } 
				}
				if( isinthis  ) {
				(*dotout) << (*i).add;
				(*i).written = true; }
			}
			// end this subgraph
			(*dotout) << "} " << endl;
		}

      // DQ (8/16/2004): These nodes will appear outside of the subgraph (as intended by the user)
		// write everything that wasnt written yet
		for(typename vector<NodeStorageType>::iterator i=mNodes.begin(); i!=mNodes.end(); i++) {
			if(!(*i).written) (*dotout) << this->nodeName( (*i).node ) << "[label=\"BLA" <<  (*i).label  << "\" " <<  (*i).option  << "];" << endl;
		}
		// write edges
		for(typename vector<EdgeStorageType>::iterator i=mEdges.begin(); i!=mEdges.end(); i++) {
			if(!(*i).written) (*dotout) << this->nodeName( (*i).node1 )
				<< " -> "
				<< this->nodeName( (*i).node2 )
				<< "[label=\"" <<  (*i).label << "\" " <<  (*i).option  << " ];" << endl;
		}
		for(typename vector<NodeAdditionalStorage>::iterator i=mAdds.begin(); i!=mAdds.end(); i++) {
			if(!(*i).written) (*dotout) << (*i).add;
		}

		// subgraphs done
	}
}

template<class NodeType>
void
DOTSubgraphRepresentation<NodeType>::writeToFileAsGraph(string filename) {
	writeDotoutStream();
	DOTRepresentation<NodeType>::writeToFileAsGraph(filename);
}

template<class NodeType>
void
DOTSubgraphRepresentation<NodeType>::writeToFile(string filename) {
	writeDotoutStream();
	DOTRepresentation<NodeType>::writeToFile(filename);
}

template<class NodeType>
void 
DOTSubgraphRepresentation<NodeType>::addNode(NodeType node, string nodelabel, string option) {
	addNode(node,nodelabel,option, -1);
}

template<class NodeType>
void 
DOTSubgraphRepresentation<NodeType>::addNode(NodeType node, string nodelabel, string option, int subgraph) {
	NodeStorageType sn;
	sn.node = node;
	sn.label = nodelabel;
	sn.option = option;
	sn.subgraph = subgraph;
	mNodes.push_back( sn );
	return;
}

template<class NodeType>
void DOTSubgraphRepresentation<NodeType>::
addEdge(NodeType node1, TraceType downtrace, TraceType uptrace, string edgelabel, NodeType node2, string option) {
	EdgeStorageType edge;
	edge.node1 = node1;
	edge.node2 = node2;
	ostringstream strs;
        strs << downtrace << ":" << uptrace << ":"<< edgelabel;
	edge.label = strs.str();
	string diropt(" dir=both ");
	edge.option = option + diropt;
	mEdges.push_back(edge);
	return;
}
 
// for edges to revisited nodes (there is no uptrace)
template<class NodeType>
void DOTSubgraphRepresentation<NodeType>::
addEdge(NodeType node1, TraceType downtrace, string edgelabel, NodeType node2, string option) {
	EdgeStorageType edge;
	edge.node1 = node1;
	edge.node2 = node2;
	ostringstream strs;
        strs << downtrace << ":" << edgelabel;
	edge.label = strs.str();
	string diropt(" arrowhead=odot ");
	edge.option = option + diropt;
	mEdges.push_back(edge);
	return;
}
 
// for edges to revisited nodes (there is no uptrace)
template<class NodeType>
void DOTSubgraphRepresentation<NodeType>::
addEdge(NodeType node1, string edgelabel, NodeType node2, string option) {
	EdgeStorageType edge;
	edge.node1 = node1;
	edge.node2 = node2;
	edge.label = edgelabel;
	edge.option = option;
	mEdges.push_back(edge);
	return;
}

template<class NodeType>
void DOTSubgraphRepresentation<NodeType>::
addNullValue(NodeType node, TraceType trace, string varname, string option) {
	NodeAdditionalStorage add;
	add.node = node;
	ostringstream strs;
  // a null value is represented by an edge to a diamond node, with the variable name as edge label
  // edge
  (strs) << "n_" << node
	    << " -> "
	    << "n_" << node << "__" << varname << "__null"
	    << "[label=\"" << trace << ":" << varname << "\" " << "dir=none "<< option << "];" << endl;
  // node
  (strs) << "n_" << node << "__" << varname << "__null"
	    << "[label=\""<< trace << ":\" shape=diamond "<< option <<"];" << endl;
	add.add = strs.str();
	mAdds.push_back( add );
	return;
}
 
template<class NodeType>
void DOTSubgraphRepresentation<NodeType>::
addEmptyContainer(NodeType node, TraceType trace, string varname, string option) {
	NodeAdditionalStorage add;
	add.node = node;
	ostringstream strs;
  (strs) << "n_" << node // node: holding null-reference to STL container, using [] to represent container-reference 
	    << " -> "
	    << "n_" << node << "__" << varname << "__null"
	    << "["<< "label=\"" << trace << ":" << varname << "[]\"" << " dir=none ];" << endl;
  (strs) << "n_" << node << "__" << varname << "__null"
	    << "[label=\"\" shape=diamond ];" << endl; // dot-null node
	add.add = strs.str();
	mAdds.push_back( add );
	return;
}

// add new subgraph, returns id of the subgraph
template<class NodeType>
int DOTSubgraphRepresentation<NodeType>::addSubgraph(string name)
{
	assert(!mManualSubgraphIds);
	mAutoSubgraphIds = true;
	mSubgraphIds++;
	mSubgraphNames[ mSubgraphIds ] = name;
	return mSubgraphIds;	
}

template<class NodeType>
void DOTSubgraphRepresentation<NodeType>::addSubgraph(int id, string name)
      {
        assert(!mAutoSubgraphIds);
        mManualSubgraphIds = true;
        mSubgraphNames[id] = name;
      };

