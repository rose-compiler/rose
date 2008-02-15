
template <class Node, class Edge> 
DAGCreate<Node, Edge>::DAGCreate () : IDGraphCreateTemplate<Node, Edge> (NULL) 
  {}

template <class Node, class Edge> 
void
DAGCreate<Node, Edge>::DeleteNode(Node* n) 
  {
   IDGraphCreateTemplate<Node, Edge>::DeleteNode(n);
  }

template <class Node, class Edge>
DAGCreate<Node, Edge>::~DAGCreate() 
   {
  // printf ("Inside of ~CallGraphCreate() \n"); 
   }

template <class Node, class Edge> void
DAGCreate<Node, Edge>::addNode ( Node* node )
   { CreateBaseNode ( node ); }

template <class Node, class Edge> void
DAGCreate<Node, Edge>::addEdge ( Node *src, Node *snk, Edge* edge )
   { CreateBaseEdge ( src, snk, edge ); }


template <class Node, class Edge> bool
DAGCreate<Node, Edge>::edgeExist ( Node *src, Node *snk)
   { 
     bool edge_exist = false;     
     for (typename DAGCreate<Node, Edge>::IDEdgeIterator i = this->GetIDNodeEdgeIterator(src,GraphAccess::EdgeOut); !i.ReachEnd(); ++i) 
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
     for (typename DAGCreate<Node, Edge>::IDEdgeIterator i = this->GetIDNodeEdgeIterator(src,GraphAccess::EdgeOut); !i.ReachEnd(); ++i) 
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

