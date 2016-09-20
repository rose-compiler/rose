class DependenceGraph // : public SimpleDirectedGraph
{
 protected:
  bool debugme;
 public:
  DependenceGraph() {debugme=false;}
  virtual ~DependenceGraph(){};
  void debugCoutNodeList()
  {
#if 0
    std::set<SimpleDirectedGraphNode *>::iterator i;
    for (i=_nodes.begin();i!=_nodes.end();i++)
      {
        std::cout<<"\t\t"<<(*i)<<"\n";
      }
#endif
  }

  /* ! \brief This enum marks what type of edge connects two DependenceNodes

     This enum is used in conjunction with bit vector representations, so
     some of the values are powers of two */
  enum EdgeType
  {
    // control information
    CONTROL         = 0x1,          /* !< A control dependence edge */
    CALL            = 0x4,             /* !< An edge between a call site and a function entry, or from actual-in to formal-in nodes */
    CALL_RETURN     = 0x5,        /* !return to the call-site */
    // data information
    DATA            = 0x2,             /* !< A data dependence edge */
    SUMMARY         = 0x3,          /* !< A summary edge between actual-in and actual-out nodes (used for interprocedural */
    PARAMETER_IN    = 0x7,
    PARAMETER_OUT   = 0x8,
    // SYNTACTIC
    SYNTACTIC       = 0xe,
        
    //        RETURN          = 0x6,          /* !< An edge from formal-out nodes to actual-in nodes */
    DATA_HELPER     = 0x9,
    CONTROL_HELPER  = 0xa,
    GLOBALVAR_HELPER= 0xb,
    COMPLETENESS_HELPER=0xc,
    // nice, but completely useless
    BELONGS_TO      = 0xd, /* shows for floating nodes, to which statement/node they belong to*/
    //      NUM_EDGE_TYPES  = 0x11   /* !< Set to be 1 more than the last entry, to fix size of the name array */

    // Jim Leek, 2009/6/3: DO_NOT_FOLLOW is A special type of edge for controlling reachability. It can keep an edge from
    // being traversed by _getReachable without changing the type of the edge
    DO_NOT_FOLLOW   = 0x10,
  };
  /* ! \brief an array of C-strings for each EdgeType
     This array is initialized in DependenceGraph.C */
  static const char *edgeNameArray[8];
  const char *getEdgeName(EdgeType type);
  /*
    static char *getEdgeName(EdgeType type)
    {
    int offset=0;
    switch(type)
    {
    case CONTROL:
    return "CONTROL";
    case DATA:
    return "DATA";
    case SUMMARY:
    return "SUMMARY";
    `case CALL:
    return "CALL";
    case RETURN:
    return "RETURN";
    case PARAMETER_IN:
    return "PARAMETER_IN";
    case PARAMETER_OUT:
    return "PARAMETER_OUT";
    case DATA_HELPER:
    return "DATA_HELPER
    default:
    return "UNKNOWN"
    }
    }*/
  //    static char *edgeNames[NUM_EDGE_TYPES];

  /* ! \brief create a new DependenceNode that is a copy of node

     Params: - DependenceNode * node: the node we want to copy

     Return: Either a newly created copy, or, if this has been copied
     before, the previous copy

     Side effects: If we created a new copy, we insert a mapping from node
     to the newly created copy in _depnode_map. */
  //    DependenceNode *createNode(DependenceNode * node);

  /* ! \brief retrieve the DependenceNode associated with node

     Params: - DependenceNode * node: the node we want to find a copy for

     Return: If node exists in _depnode_map, we return the associated copy,
     otherwise we return NULL. */
  //    DependenceNode *getNode(DependenceNode * node);

  /* ! \brief create a new DependenceNode which contains node

     Params: - SgNode * node: the SgNode we want to wrap in a DependenceNode

     Return: Either a new DependenceNode or, if we've already wrapped this
     SgNode, the existing DependenceNode containing node

     Side effects: If we created a new DependenceNode, we insert a mapping
     from node to the newly created DependenceNode in _sgnode_map. */
  DependenceNode *createNode(DependenceNode::NodeType type,SgNode * identifyingNode);
  DependenceNode *createNode(SgNode * node);
  void deleteNode(DependenceNode * node);

  /* ! \brief retrieve the DependenceNode that wraps node

     Params: - SgNode * node: the SgNode for which we want to find the
     associated DependenceNode

     Return: If there is a wrapper DependenceNode in _sgnode_map, we return
     it. Otherwise, we return NULL. */
  DependenceNode *getNode(SgNode * node);
  // (NodeType type, SgNode * node = NULL, std::string depName= "")
  DependenceNode *getNode(DependenceNode::NodeType type,SgNode * identifyingNode);

  DependenceNode * getExistingNode(SgNode * node);
  DependenceNode * getExistingNode(DependenceNode::NodeType type,SgNode * identifyingNode);

  // ! return the InterproceduralInfo object associated with the
  // DependenceGraph
  InterproceduralInfo *getInterprocedural()
  {
    return NULL;
    //!*!        return _interprocedural;
  }

  /* ! \brief create an edge of type e between from and to

     Params: - DependenceNode * from: the source of the edge -
     DependenceNode * to: the sink of the edge - EdgeType e: the type of the 
     edge

     Side effects: Inserts the Edge (from, to) into the set associated with
     e by _edgetype_map. Inserts e into the set associated with Edge(from,
     to) by _edge_map.

  */
  virtual void establishEdge(DependenceNode * from, DependenceNode * to, EdgeType e=CONTROL);
  virtual void removeEdge(DependenceNode * from, DependenceNode * to, EdgeType e=CONTROL);
  /* ! \brief determine if there is an edge of type e between from and to

     Params: - DependenceNode * from: the source of the edge -
     DependenceNode * to: the sink of the edge - EdgeType e: the type of the 
     edge

     Return: true if e is in the set associated with Edge(from, to) by
     _edge_map. */
  bool edgeExists(DependenceNode * from, DependenceNode * to, EdgeType e);
  bool hasOutgingEdge(DependenceNode * src,EdgeType compare);

  /* ! \brief returns all edges between from and to

     Params: - DependenceNode * from: the source of the edge -
     DependenceNode * to: the sink of the edge

     Return: the set of EdgeTypes associated with Edge(from, to) by
     _edge_map.

  */
  std::set < EdgeType > edgeType(DependenceNode * from, DependenceNode * to);
  /*    std::list <DependenceNode*> getParents(DependenceNode * current)
	{
	std::list <DependenceNode*> parentList;
	return parentList;
	}*/
  // ! writes a dot file representing this dependence graph to filename
  virtual void writeDot(char *filename);

 protected:
  // ! Maps a DependenceNode to a copy unique to this DependenceGraph
  //!*!    std::map < DependenceNode *, DependenceNode * >_depnode_map;
  // ! Maps an SgNode to a DependenceNode unique to this DependenceGraph
  //!*!    std::map < SgNode *, DependenceNode * >_sgnode_map;
  std::map<SgNode*,DependenceNode*> sgNodeToDepNodeMap;
  std::map<DependenceNode::NodeType,std::map<SgNode*,DependenceNode*> > nodeTypeToDepNodeMapMap;

  // ! The InterproceduralInfo associated with this DependenceGraph
  //!*!    InterproceduralInfo *_interprocedural;

  typedef std::pair < DependenceNode *, DependenceNode * >Edge;

  // ! a map from EdgeType to all the edges of that type
  std::map < EdgeType, std::set < Edge > >edgeTypeMap;

// DQ (8/30/2009): Debugging ROSE compiling ROSE (this statement does not compile using ROSE). The error is:
// sage_gen_be.C:5286: SgEnumDeclaration* sage_gen_enum_definition(a_type*, SgDeclarationStatement*&, DataRequiredForComputationOfSourcePostionInformation*): Assertion `forwardDeclaration->get_parent() != __null' failed.
// #ifndef USE_ROSE

  // ! a map from an edge to all the variants of that edge in the graph
  std::map < Edge, std::set < EdgeType > >edgeMap;

// #endif

  bool isLibraryFunction(SgFunctionDeclaration * sgFD) const
  {
    if (sgFD == NULL) return false;
    else if (sgFD->get_definition() != NULL) return true;
    else return false;
  };
};
