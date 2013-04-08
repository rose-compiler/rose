// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// rose.C: Example (default) ROSE Preprocessor: used for testing ROSE infrastructure



#ifndef AST_ATTRIBUTEDOT_H
#define AST_ATTRIBUTEDOT_H

/**************************************************************************************
 *
 *  The class
 *       class AstAttributeDOT
 *  is the base class implementation for DOTGraphNode and DOTGraphEdge. It is meant
 *  to implement convenience functions to help building the string representing 
 *  DOT options and information like if the node should be filtered out in a graph.
 *  
 **************************************************************************************/
class ROSE_DLL_API AstAttributeDOT:public AstAttribute
   {
     public:
          enum DOTStyles
             {
               UnknownListElementDOTStyles = 0,
               filled     = 1,
               not_filled = 2,
               END_OF_NODE_TYPE_LIST_DOTStyles
             };

     private:
       // label the node with auxiliary infomation (is not used by default)
          std::string label;
       // Font used for text. 
          std::string fontname;
       // Color used for text.
          std::string fontcolor;
       // Basic drawing color for graphics, not text.
          std::string color;
       // Set style for node or edge. For cluster subgraph, if "filled", the cluster box's background is filled.
          DOTStyles style; 
       // List of user specific options which is dot options the user can just add
          std::list<std::string> extraOptionsList;
       // If true keep if false filter out from graph
          bool p_keep;

     public:
          AstAttributeDOT ();

       // keep node if value is true (default) if false filter out
          bool get_keep();
          void set_keep(bool);
 
          std::string get_label();
          void set_label(std::string);
 
       // DOT options
          void set_color (std::string);
          void set_extra_options(std::string);
          void set_fontcolor(std::string);
          void set_fontname(std::string);
          void set_style (DOTStyles);

          std::string get_color ();
          std::string get_fontcolor();
          std::string get_fontname();
          std::list<std::string> get_extra_options();
          DOTStyles get_style ();
          std::string get_style_string ();

          std::vector<std::string> get_dot_options_list();
          std::string get_dot_options_string();
   };



/**************************************************************************************
 *
 *  The class
 *       class DOTGraphNode
 *  implements convenience functions specific to a graph node for building a dot options
 *  string. It also contains information about which node in the AST we operate on.  
 **************************************************************************************/

class ROSE_DLL_API DOTGraphNode : public AstAttributeDOT
   {
     public:
          enum DOTShapeTypes
             {
               UnknownListElementDOTShapeTypes = 0,
               polygon = 1,
               ellipse = 2,
               record = 3,
               mrecord = 4,
               END_OF_NODE_TYPE_LIST_DOTShapeTypes
             };

     private:
       // Current node operated upon
          SgNode*     node;
       // Color used to fill the background of a node or cluster assuming style=filled.
       // If fillcolor is not defined, color is used.
          std::string fillcolor;
       // Possible DOT shapes: polygon, ellipse, record, mrecord ...
          DOTShapeTypes shape;

     public:
          DOTGraphNode( SgNode* n );

          SgNode* get_node();

          void set_fillcolor (std::string);
          void set_shape (DOTShapeTypes a_shape);
   
          std::string get_fillcolor ();
          DOTShapeTypes get_shape ();
          std::string get_shape_string ();

          std::vector<std::string> get_dot_options_list();
          std::string get_dot_options_string();
   };


/**************************************************************************************
 *
 *  The class
 *       class DOTGraphEdge
 *  implements convenience functions specific to a graph edge for building a dot options
 *  string. It also contains information about which edge in the AST we operate on.  
 **************************************************************************************/

class ROSE_DLL_API DOTGraphEdge : public AstAttributeDOT
   {
     public:
          enum DOTArrowTypes
             {
               UnknownListElementArrowTypes = 0,
               normal = 1,
               inv = 2,
               dot = 3,
               invdot = 4,
               odot = 5,
               invodot = 6,
               none = 7,
               tee = 8,
               empty = 9,
               invempty = 10,
               diamond = 11,
               odiamond = 12,
               ediamond = 13,
               crow = 14,
               box = 15,
               obox = 16,
               open = 17,
               halfopen = 18,
               vee = 19,
               END_OF_NODE_TYPE_LIST_DOTArrowTypes
             };

     private:
       // pair of nodes for an edge. The edge points from pair<>.first to pair<>.second
          std::pair<SgNode*,SgNode*> edgeNodes;
       // Possible types of arrows
          DOTArrowTypes arrowType;

     public:
         ~DOTGraphEdge(){};

          DOTGraphEdge ( SgNode*, SgNode*);
          void set_arrow_type (DOTArrowTypes a_arrow);

          DOTArrowTypes get_arrow_type ();
          std::string   get_arrow_type_string ();
          std::pair<SgNode*,SgNode*> get_edge();

          std::vector<std::string> get_dot_options_list();
          std::string get_dot_options_string();
   };

#endif
