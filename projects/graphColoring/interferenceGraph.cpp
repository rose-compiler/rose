#include "interferenceGraph.h"

using namespace std;

namespace graphColoring
{
  // Return true based on a given p probability, 
  // where p>=0.0 (0% chance) and p <=1.0 (100% chance) 
  bool trueOnProbability (float p, unsigned int seed)
  {
    //float tol = 1.0/(RAND_MAX +1);  this will give a negative value!! max 32-bit integer is RAND_MAX! +1 will overflow it!
    float tol = 1.0/(RAND_MAX); // this is also the smallest granularity of probability srand() can achieve (the accuracy)

    assert (p>=0.0 && p <= 1.0);
    // close or equal to 1
    if ( (1.0 - p)/1.0< tol)
      return true;
    // close or equal to 0
    if ( (p - 0.0) < tol)
      return false;

    int multiplier = p/tol ; // the multiplier of the desired p on top of the building block p

    srand (seed); // seed the generator
    // the chance of return [0, multiplier] is multiplier/RAND_MAX ==> p/tol/RAND_MAX ==> p
    if ( rand()<= multiplier)
      return true;
    else
      return false;
  }

  Adjacent_Matrix::Adjacent_Matrix(size_t node_count):n_count(node_count){
    // Don't FORGET to fill in the elements! Or segmentation fault!
    for (size_t i = 0; i< node_count; i++ )
    {
      // Only need to fill in the lower diagonal of the matrix: i > j: right side is empty 
      for (size_t j = 0; j< i; j++ )
      {
        matrix.insert (make_pair ( make_pair(i,j), false));
      }
    }
  };


  void Adjacent_Matrix::toDotGraph(string file_name)
  {
    ofstream fs(file_name.c_str());
    fs << "graph graphname {\n"; 
    // print nodes
    for (size_t i=0; i<n_count; i++)
      fs<<i<<" [label=\""<<i<<"\", color=\"blue\", style=\"solid\"];\n";
    // print edges
    for (size_t i = 0; i< n_count; i++ )
    {
      for (size_t j = 0; j< i; j++ )
      {
        if (matrix[make_pair(i,j)] == true)
          //fs<<i<<" -> "<<j<<" [label=\"\", style=\"solid\"];\n";
          //fs<<i<<" -- "<<j<<" [label=\"\", style=\"dotted\"];\n";
          fs<<i<<" -- "<<j<<" [label=\"\", style=\"solid\"];\n";
      }
    }
    fs << "}\n";
  }

  void Adjacent_Matrix::print()
  {
    size_t edge_counter=0;
    for (size_t i = 0; i< n_count; i++ )
    {
      cout<<"row id:"<<i<<" ";
      for (size_t j = 0; j< i; j++ )
      {
        if (matrix[make_pair(i,j)])
          edge_counter ++;
        cout<<matrix[make_pair(i,j)]<<" ";
      }
      cout<<endl;
    }
    cout<<"graph edge density ratio="<<((float)edge_counter*2)/((float)(n_count*(n_count -1)))<<endl;
  }
  void Adjacent_Matrix::setEdge(size_t index_i, size_t index_j, bool val)
  {
    assert (index_i != index_j);
    assert (index_i < n_count);
    assert (index_j < n_count);
    // we only save the lower diagonal of the matrix, (i<j)
    if (index_i > index_j)
      matrix[make_pair(index_i, index_j)] = val;
    else
      matrix[make_pair(index_j, index_i)] = val;
  }

  void Adjacent_Matrix::resetEdge(size_t index_i, size_t index_j)
  {
    assert (index_i != index_j);
    assert (index_i < n_count);
    assert (index_j < n_count);
    // we only save the lower diagonal of the matrix, (i<j)
    if (index_i > index_j)
      matrix[make_pair(index_i, index_j)] = false;
    else
      matrix[make_pair(index_j, index_i)] = false;
  }

  void Adjacent_Matrix::flipEdge(size_t index_i, size_t index_j)
  {
    assert (index_i != index_j);
    assert (index_i < n_count);
    assert (index_j < n_count);
    // we only save the lower diagonal of the matrix, (i<j)
    if (index_i > index_j)
    {
      matrix[make_pair(index_i, index_j)] = (matrix[make_pair(index_i, index_j)]==false);
    }
    else
    {
      matrix[make_pair(index_j, index_i)] = (matrix[make_pair(index_j, index_i)]==false);
    }
  }

  bool Adjacent_Matrix::hasEdge(size_t index_i, size_t index_j)
  {
    assert (index_i != index_j);
    assert (index_i < n_count);
    assert (index_j < n_count);
    // we only save the lower diagonal of the matrix, (i<j)
    if (index_i > index_j)
      return matrix[make_pair(index_i, index_j)];
    else
      return matrix[make_pair(index_j, index_i)];
  }

  ostream & operator << (ostream & o, const Node & n)
  {
    o << n.id ;
    return o;
  }

  Graph::Graph(size_t node_count, float edge_complete_rate)
  {
    randomPopulateGraph (node_count, edge_complete_rate);
  }

  Graph::~Graph()
  {
    assert (adj_matrix!= NULL);
    delete adj_matrix; 
  }

  void Graph::randomPopulateGraph(size_t node_count, float edge_complete_rate)
  {
    assert (nodes.size() == 0);
    adj_matrix = new Adjacent_Matrix (node_count);
    // create Nodes
    for (size_t i =0; i< node_count; i++)
    {
      Node * n = new Node(i);
      nodes.push_back(n);
    }

    // decide if there is an edge between two nodes
    // for undirected graph, we only need to populate half of the adjacent matrix
    //#pragma omp parallel for 
    for (size_t i =0; i< node_count; i++)
      //for (size_t j =0; j< node_count; j++)
      for (size_t j =0; j< i; j++)
      {
        adj_matrix->setEdge (i,j,trueOnProbability (edge_complete_rate, time(NULL)+i+j));
      }
  }

  void Graph::printSpanTree (const vector <pair <Node*, Node*> >& span_tree_edges)
  {
    // output the current span tree
    for (size_t i=0; i < span_tree_edges.size(); i++)
    {
      cout<<(*span_tree_edges[i].first)<<"--"<<(*span_tree_edges[i].second) << " ";
    }
    cout<<endl;
  }

  void Graph::initVisited()
  {
    // reset visited flags
    visited.clear();
    for (size_t i =0 ; i<nodes.size(); i++)
    {
      visited.push_back(false);
    }
    assert (visited.size() == nodes.size());
  }

  void Graph::depth_first_traverse (Node* n, vector< pair <Node*, Node*> >& span_tree_edges)
  {
    visited[n->id] = true;
    cout<<n->id <<" visited " <<endl;
    // for each neighbors of n, if not visited, do the same depth first traverse
    vector<Node*> neighbors = n->neighbors;
    //  getNeighbors (*n, neighbors);
    for (size_t i=0; i< neighbors.size(); i++) // this will include a back edge from n
    {
      Node* nb = neighbors[i];
      if (visited[nb->id]!= true)// this can also screen out the incoming node n to nb since n is also one of nb's neighbors.
      {
        // store the current node
        span_tree_edges.push_back(make_pair(n, nb));
        depth_first_traverse (nb, span_tree_edges);
      }
      else
      { // find a node which has been visited before.
        // this node can be the incoming node: n--nb
        // so we must check if this edge is included or not
        // Also due to stack unwinding in recursive call: nb can be a node not connected by a node in the bottom of the span tree
        vector< pair <Node*, Node*> >::iterator iter ;
        // search twice since the graph is undirected
        iter = find (span_tree_edges.begin(), span_tree_edges.end(),make_pair(n,nb));
        if (iter == span_tree_edges.end())
          iter = find(span_tree_edges.begin(), span_tree_edges.end(),make_pair(nb,n));

        if (iter == span_tree_edges.end())
        { // must do this edge search since all neighbors of n is checked, including the predecessor of n: p
          // p --> n,   then every neighbor of n will go back and find p again, but n-->p is alredy in the span tree!

          // output the current span tree
          //cout<<"revisited a node by an edge which is not in the span tree eges:"<< *n <<"--"<< *nb <<endl;
//          printCycle (span_tree_edges, make_pair(n,nb));
          // now we just find a back edge from n-->nb (or nb-->n) which is just part of the cycle. how to print out the cycle??
          // we need to traverse the span tree to find a path in the tree from n to nb (either n or nb can show up first!!)
          // then connect the two ends using n--nb
        }
        else
        {
          //cout<<"revisted a node by an edge within existing span tree:"<<(*nb)<<endl;
        }
      }
    }
  }

  void Graph::DFS()
  {
    initVisited();
    vector <pair <Node*, Node*> > span_tree_edges;
    for (size_t i =0; i<nodes.size(); i++)
    {
      if (visited[nodes[i]->id]!=true)
        depth_first_traverse (nodes[i], span_tree_edges);
    }

    for (size_t i=0; i< span_tree_edges.size(); i++)
    {
      pair <Node*, Node*> edge = span_tree_edges[i];
      cout<<edge.first->id<<"->"<<edge.second->id<<endl;
    }
    Graph tree(getNodeCount(), span_tree_edges);
    tree.toDotGraph("tree.dot");
  }

  // Construct a graph from a span tree
  Graph::Graph(size_t node_count, const vector <pair <Node*, Node*> >& span_tree_edges)
  {
    adj_matrix = new Adjacent_Matrix (node_count);
    // create Nodes
    for (size_t i =0; i< node_count; i++)
    {
      Node * n = new Node(i);
      nodes.push_back(n);
    }
    for (size_t i =0 ; i< span_tree_edges.size(); i++)
    {
      Node* src = span_tree_edges[i].first;
      Node* dest= span_tree_edges[i].second;
      adj_matrix->setEdge (src->id, dest->id, true);
    }
  }

  void Graph::regenerateAdjacentNeighbors ()
  {
    // clean up existing data
    for (size_t i=0; i<nodes.size(); i++)
    {
      nodes[i]->neighbors.clear();
    }

    for (size_t i=0; i<nodes.size(); i++) // go through each element of the diagonal matrix
      for (size_t j =0; j<i; j++)
      {
        // each edge brings two neighbors! 
        if (i!=j && adj_matrix->hasEdge(i,j) == true)
        {
          nodes[i]->neighbors.push_back (nodes[j]);
          nodes[j]->neighbors.push_back (nodes[i]);
        }
      }

    // do some self checking here

    for (size_t i=0; i<nodes.size(); i++)
    {
      size_t size1 = nodes[i]->neighbors.size();
      vector<Node*> neighbors;
      getNeighbors(*(nodes[i]), neighbors);
      size_t size2 = neighbors.size();
      assert (size1 == size2);
    }

  }

  void Graph::getNeighbors(const Node& n, vector<Node*>& neighbors)
  {
    assert (neighbors.size() ==0);
    size_t i = n.id;
    //  for (size_t j=0; j<i; j++) // This is WRONG
    //for (size_t j=0; j<(nodes.size()/2); j++) //  WRONG! must check all pairs!
    for (size_t j=0; j<nodes.size(); j++) // must check every possible pairs
    {
      if (i!=j && adj_matrix->hasEdge(i,j) == true)
        neighbors.push_back(nodes[j]);
    }
  }


} // end of namespace
