class EdgeType
   {
  // EdgeType holdes information required to specify an edge to DOT

     public:
          SgNode* start;
          SgNode* end;
          string optionString;

          EdgeType (SgNode* n1, SgNode* n2, string options )
             : start(n1), end(n2), optionString(options) {}
   };

class NodeType
   {
  // NodeType holdes information required to specify additional info for any node to DOT

     public:
          SgNode* target;
          string labelString;
          string optionString;

          NodeType (SgNode* node, string label, string options )
             : target(node), labelString(label), optionString(options) {}

       // bool operator= (const NodeType & node) const { return node.target == target; }
          bool operator!=(const NodeType & node) const { return node.target != target; }
          bool operator==(const NodeType & node) const { return node.target == target; }
   };
