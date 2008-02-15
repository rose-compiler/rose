#ifndef TYPED_FUSION
#define TYPED_FUSION

class GraphNode;
class GraphEdge;
class TypedFusionOperator 
{
  public:
    virtual int GetNodeType( GraphNode *n) = 0;
    virtual void MarkFuseNodes( GraphNode *n1, GraphNode *n2) = 0;
 // virtual Boolean PreventFusion( GraphNode *src, GraphNode *snk, 
    virtual int PreventFusion( GraphNode *src, GraphNode *snk, 
                                   GraphEdge *e) = 0;
};
 
class GraphAccess;
class TypedFusion
{
  public:
    void operator()(GraphAccess *dg, TypedFusionOperator &op, int fusetype);
};

#endif
