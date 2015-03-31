AST to GraphViz
===============

A programmable GraphViz (dot) generator for ROSE Compiler, it uses ROSE's [AstTopDownProcessing](http://rosecompiler.org/ROSE_HTML_Reference/classAstTopDownProcessing.html).

## Prototype of the top-down GraphViz Generator

```c++
struct NodeDesc {
  std::string label;
  std::string shape;
  std::string color;
  std::string fillcolor;
};

struct EdgeDesc {
  std::string label;
  std::string color;
  bool constraint;
};

template <class DownAttr_tpl>
class TopDown : public AstTopDownProcessing<DownAttr<DownAttr_tpl> > {
  public:
    TopDown();
    void toDot(std::ostream & out) const;
    void clear();

    NodeDesc & addNode(SgNode * node);
    EdgeDesc & addEdge(SgNode * node, SgNode * parent);

  protected:
    virtual bool skipNode(SgNode * node, const DownAttr_tpl & attr_in);
    virtual void computeAttr(SgNode * node, const DownAttr_tpl & attr_in, DownAttr_tpl & attr_out);

    virtual bool stopAtNode(SgNode * node, const DownAttr_tpl & attr_in, const DownAttr_tpl & attr_out);

    virtual void editNodeDesc(SgNode * node, const DownAttr_tpl & attr_in, const DownAttr_tpl & attr_out, NodeDesc & node_desc) = 0;
    virtual void editEdgeDesc(SgNode * node, const DownAttr_tpl & attr_in, const DownAttr_tpl & attr_out, SgNode * parent, EdgeDesc & edge_desc) = 0;

    DownAttr<DownAttr_tpl> evaluateInheritedAttribute(SgNode * node, DownAttr<DownAttr_tpl> attr_in);
};
```
A specialization AST2GraphViz::Basic is provided. It is demonstrated in whole-ast-to-graphviz.cpp and extract-api-fragment.cpp.

## Make your own implementation

### Minimal implementation

```c++
struct EmptyDownAttr {};

class Basic : public AST2GraphViz::TopDown<EmptyDownAttr> {
  public:
    typedef AST2GraphViz::DownAttr<EmptyDownAttr> DownAttr;
    typedef AST2GraphViz::NodeDesc NodeDesc;
    typedef AST2GraphViz::EdgeDesc EdgeDesc;

  protected:
    void editNodeDesc(SgNode * node, const EmptyDownAttr & attr_in, const EmptyDownAttr & attr_out, NodeDesc & node_desc) {
      node_desc.label = node->class_name().getString();
      node_desc.shape = "box";
      node_desc.color = "black";
      node_desc.fillcolor = "white";
    }

    void editEdgeDesc(SgNode * node, const EmptyDownAttr & attr_in, const EmptyDownAttr & attr_out, SgNode * parent, EdgeDesc & edge_desc) {
      edge_desc.label = "";
      edge_desc.color = "black";
      edge_desc.constraint = true;
    }
};
```
Using ROSE's interface, you can extract information from the Sage nodes.
You can then use these information to edit the descriptor.

### Controling the traversal

More virtual methods can be reimplemented for more control:
* skipNode: skips node in the traversal. In the resulting graph, the children of the skipped node will have the parent of the skipped node for parent.
* stopAtNode: the children of the current node will no be included in the graph (the traversal continue). 

### Embeding a top-down analysis of the AST

AST2GraphViz::TopDown is a template of the attribute passed down the AST during the traversal.
You can use this function in association with the computeAttr to graft your top-down analysis of the AST.
It takes the input attribute, inherited from the parent node, and produce an output attribute, forwarded to the children.
computeAttr is called before all other overloadable methods. These methods receive both input and output attributes.

Using this one can output on the graph the steps of the analysis.



