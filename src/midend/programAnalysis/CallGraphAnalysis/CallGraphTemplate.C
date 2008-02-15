
template <class Node, class Edge> 
DAGCreate<Node, Edge>::DAGCreate () : VirtualGraphCreateTemplate<Node, Edge> (NULL) 
  {}

template <class Node, class Edge> 
void
DAGCreate<Node, Edge>::DeleteNode(Node* n) 
  {
   VirtualGraphCreateTemplate<Node, Edge>::DeleteNode(n);
  }

template <class Node, class Edge>
DAGCreate<Node, Edge>::~DAGCreate() 
   {
  // printf ("Inside of ~CallGraphCreate() \n"); 
   }

template <class Node, class Edge> void
DAGCreate<Node, Edge>::addNode ( Node* node )
   { VirtualGraphCreateTemplate<Node,Edge>::AddNode ( node ); }

template <class Node, class Edge> void
DAGCreate<Node, Edge>::addEdge ( Node *src, Node *snk, Edge* edge )
   { VirtualGraphCreateTemplate<Node,Edge>::AddEdge ( src, snk, edge ); }


template <class Node, class Edge> bool
DAGCreate<Node, Edge>::edgeExist ( Node *src, Node *snk)
   { 
     bool edge_exist = false;     
     for (typename DAGCreate<Node, Edge>::EdgeIterator i = this->GetNodeEdgeIterator(src,GraphAccess::EdgeOut); !i.ReachEnd(); ++i) 
        {
	      Edge* currentEdge = i.Current();
              Node* endPoint= GetEdgeEndPoint(currentEdge, GraphAccess::EdgeIn);
	      
	      if(snk == endPoint)
            {
           // std::cout << "NODE EXISTS" << std::endl;
              edge_exist = true;
		        break;
	         }
           else
            {
           // std::cout << "NODE DO NOT EXIST" << std::endl;
            }
        }

     
     
     return edge_exist; 
   }


#if 0
// DQ (9/12/2007): Redundant declaration, I think.
template <class Node, class Edge> bool
DAGCreate<Node, Edge>::edgeExist ( Node *src, Node *snk)
   { 
     bool edge_exist = false;     
     for (typename DAGCreate<Node, Edge>::EdgeIterator i = this->GetNodeEdgeIterator(src,GraphAccess::EdgeOut); !i.ReachEnd(); ++i) 
        {
	      Edge* currentEdge = i.Current();
              Node* endPoint= GetEdgeEndPoint(currentEdge, GraphAccess::EdgeIn);
	      
	      if(snk == endPoint){
		   std::cout << "NODE EXISTS" << std::endl;
		   edge_exist = true;
		   break;
	      }else
                   std::cout << "NODE DO NOT EXIST" << std::endl;
        }

     
     
     return edge_exist; 
   }
#endif

template <class Node, class Edge> int
DAGCreate<Node, Edge>::size()
   {
     int numberOfNodes = 0;
     for (typename DAGCreate<Node, Edge>::NodeIterator i = this->GetNodeIterator(); !i.ReachEnd(); ++i) 
        {
          numberOfNodes++;
        }

     return numberOfNodes;
   }

template <class Node, class Edge> std::map<std::string, int> & 
DAGCreate<Node, Edge>::getSubGraphMap()
   {
     return subgraphMap;
   }

