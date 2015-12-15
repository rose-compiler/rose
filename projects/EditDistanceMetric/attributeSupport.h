
class IsomorphicNode : public AstAttribute
   {
  // This is the persistant attribute used to mark locations in the AST where subtrees are not isomorphic.

     public:
       // I am not sure that we will really use this value
          SgNode* thisNode;
          SgNode* thatNode;

          IsomorphicNode (SgNode* n, SgNode* m) : thisNode(n), thatNode(m) {}

       // int get_node() { return node; }

          virtual std::string additionalNodeOptions();

          virtual std::vector<AstAttribute::AttributeEdgeInfo> additionalEdgeInfo();
          virtual std::vector<AstAttribute::AttributeNodeInfo> additionalNodeInfo();

          virtual AstAttribute* copy() const;
   };

class NonIsomorphicNode : public AstAttribute
   {
  // This is the persistant attribute used to mark locations in the AST where subtrees are not isomorphic.

     public:
       // I am not sure that we will really use this value
          SgNode* thisNode;
          SgNode* thatNode;

          NonIsomorphicNode (SgNode* n, SgNode* m) : thisNode(n), thatNode(m) {}

          virtual std::string additionalNodeOptions();

          virtual std::vector<AstAttribute::AttributeEdgeInfo> additionalEdgeInfo();
          virtual std::vector<AstAttribute::AttributeNodeInfo> additionalNodeInfo();

          virtual AstAttribute* copy() const;
   };


