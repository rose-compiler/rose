#ifndef ASTATTRIBUTEDOT_C
#define ASTATTRIBUTEDOT_C
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "AstAttributeDOT.h"


AstAttributeDOT::AstAttributeDOT ()
  {
    style  = UnknownListElementDOTStyles;
    p_keep   = true;  
  }


std::list<std::string> AstAttributeDOT::get_extra_options(){
   return extraOptionsList;


};


std::string
AstAttributeDOT::get_color(){
   return color;
}

std::string
AstAttributeDOT::get_fontcolor(){
   return fontcolor;
}

std::string
AstAttributeDOT::get_fontname(){
   return fontname;
}

void
AstAttributeDOT::set_label(std::string l){
     label =l;

};

std::string
AstAttributeDOT::get_label(){
     return label;

};

bool AstAttributeDOT::get_keep(){
   return p_keep;
}

void AstAttributeDOT::set_keep(bool p_k){
   p_keep = p_k;
}

void
AstAttributeDOT::set_style (DOTStyles a_style)
{

  switch (a_style)
    {
    case filled:
    case not_filled:
      style = a_style;
      break;
    default:
      ROSE_ASSERT (false);      //Default case exiting
      break;

    }
};



AstAttributeDOT::DOTStyles AstAttributeDOT::get_style ()
{
  return style;
}

std::string AstAttributeDOT::get_style_string ()
{
  std::string returnString;

  switch (style)
    {
    case filled:
          returnString = "filled";
          break;
    case not_filled:
          returnString = "not_filled";
          break;
    default:
      ROSE_ASSERT (false);      //Default case exiting
      break;

    }

  return returnString;
}

void
AstAttributeDOT::set_color (std::string v_color)
{
  color = v_color;   
}

void
AstAttributeDOT::set_fontcolor (std::string v_fontcolor)
{
  fontcolor = v_fontcolor;   
}


void
AstAttributeDOT::set_fontname (std::string v_fontname)
{
  fontname = v_fontname;   
}


void AstAttributeDOT::set_extra_options(std::string extraOptions)
{
   ROSE_ASSERT(extraOptions.find(",") == std::string::npos); // one option at a time, should not contain comma
   extraOptionsList.push_back(extraOptions);
};

std::string AstAttributeDOT::get_dot_options_string(){
     std::string optionsString;

     //"shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"Blue\",fillcolor=red,fontname=\"7x13bold\",fontcolor=black,style=filled"
     std::vector<std::string> optionsStringVector = get_dot_options_list();
 
     for(unsigned int i = 0; i < optionsStringVector.size(); i++){

        if(i == 0)
           optionsString = optionsStringVector[0];
        else 
           optionsString += ", " + optionsStringVector[i];
     }

   

     return optionsString;
  

};

std::vector<std::string> AstAttributeDOT::get_dot_options_list(){
     std::vector<std::string> optionsStringVector;

     if(color != "")
        optionsStringVector.push_back("color=\""+color+"\"");

     if(fontname != "")
        optionsStringVector.push_back("fontname=\""+fontname+"\"");

     if(fontcolor != "")
        optionsStringVector.push_back("fontcolor=\""+fontcolor+"\"");

      switch(style){
        case UnknownListElementDOTStyles:
        case END_OF_NODE_TYPE_LIST_DOTStyles:
              break;
        default:
              optionsStringVector.push_back("style=" + get_style_string());
              break;

     }

      std::copy (extraOptionsList.begin(), extraOptionsList.end(),
             std::inserter(optionsStringVector, optionsStringVector.end()));



     return optionsStringVector;

};


//#ASR: 07/07/10
//Fixed non-standard invocation of base class constructor for gcc 4.5.0
DOTGraphEdge::DOTGraphEdge ( SgNode* from, SgNode* to) : AstAttributeDOT() { 
    edgeNodes.first  = from;
    edgeNodes.second = to;
    arrowType = UnknownListElementArrowTypes;
//    AstAttributeDOT::AstAttributeDOT();

};



std::pair<SgNode*,SgNode*> DOTGraphEdge::get_edge(){
     return edgeNodes;
};

void
DOTGraphEdge::set_arrow_type (DOTArrowTypes a_arrow)
{
  arrowType = a_arrow;

};



DOTGraphEdge::DOTArrowTypes DOTGraphEdge::get_arrow_type ()
{
  return arrowType;
}

std::string DOTGraphEdge::get_arrow_type_string ()
{
  std::string returnString;
  switch (arrowType)
    {
    case normal:
      returnString = "normal";
      break;
    case inv:
      returnString = "inv";
      break;

    case dot:
      returnString = "dot";
      break;

    case invdot:
      returnString = "invdot";
      break;

    case odot:
      returnString = "odot";
      break;

    case invodot:
      returnString = "invdot";
      break;

    case none:
      returnString = "none";
      break;

    case tee:
      returnString = "tee";
      break;

    case empty:
      returnString = "empty";
      break;

    case invempty:
      returnString = "invempty";
      break;

    case diamond:
      returnString = "diamond";
      break;

    case odiamond:
      returnString = "odiamond";
      break;

    case ediamond:
      returnString = "ediamond";
      break;

    case crow:
      returnString = "crow";
      break;

    case box:
      returnString = "box";
      break;

    case obox:
      returnString = "obox";
      break;

    case open:
      returnString = "open";
      break;

    case halfopen:
      returnString = "halfopen";
      break;

    case vee:
      returnString = "vee";
      break;
    default:
      ROSE_ASSERT (false);      //Default case exiting
      break;

    }

  return returnString;
}


std::vector<std::string> DOTGraphEdge::get_dot_options_list(){
     std::vector<std::string> returnList = AstAttributeDOT::get_dot_options_list();

     switch(arrowType){
        case UnknownListElementArrowTypes:
        case END_OF_NODE_TYPE_LIST_DOTArrowTypes:
              break;
        default:
            returnList.push_back("arrowhead=\""+get_arrow_type_string()+"\""); 
            break; 
     }

     return returnList;

};

std::string DOTGraphEdge::get_dot_options_string(){
     std::string optionsString;

     //"shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"Blue\",fillcolor=red,fontname=\"7x13bold\",fontcolor=black,style=filled"
     std::vector<std::string> optionsStringVector = get_dot_options_list();
 
      for(unsigned int i = 0; i< optionsStringVector.size(); i++){

        if(i == 0)
           optionsString = optionsStringVector[0];
        else 
           optionsString += ", " + optionsStringVector[i];
     }
   

     return optionsString;
  

};



//#ASR: 07/07/2010
//Modified constructor to call base class constructor for the standard 
DOTGraphNode::DOTGraphNode(SgNode* n) : AstAttributeDOT() {
      node = n;
      shape     = UnknownListElementDOTShapeTypes;
//      AstAttributeDOT::AstAttributeDOT();
      set_label(n->class_name());
};


SgNode*
DOTGraphNode::get_node(){
     return node;

}

void
DOTGraphNode::set_fillcolor (std::string v_fillcolor)
{
  fillcolor = v_fillcolor;   
}

std::string
DOTGraphNode::get_fillcolor ()
{
  return fillcolor;   
}


void
DOTGraphNode::set_shape (DOTShapeTypes a_shape)
{

  switch (a_shape)
    {
    case polygon:
    case ellipse:
    case record:
    case mrecord:
      shape = a_shape;
      break;
    default:
      ROSE_ASSERT (false);      //Default case exiting
      break;

    }
};

DOTGraphNode::DOTShapeTypes DOTGraphNode::get_shape ()
{
  return shape;
}


std::string DOTGraphNode::get_shape_string ()
{
  std::string returnString;
  switch (shape)
    {
    case polygon:
      returnString = "polygon";
      break;
    case ellipse:
      returnString = "ellipse";
      break;
    case record:
      returnString = "record";
      break;
    case mrecord:
      returnString = "mrecord";
      break;
    default:
      ROSE_ASSERT (false);      //Default case exiting
      break;

    }

  return returnString;
}


std::vector<std::string>
DOTGraphNode::get_dot_options_list()
   {
     std::vector<std::string> optionsStringVector = AstAttributeDOT::get_dot_options_list();
 
     switch(shape)
        {
          case UnknownListElementDOTShapeTypes:
          case END_OF_NODE_TYPE_LIST_DOTShapeTypes:
               break;
          default:
               optionsStringVector.push_back("shape=" + get_shape_string());
               break;

        }

     if(fillcolor != "")
        optionsStringVector.push_back("fillcolor=\""+fillcolor+"\"");


     return optionsStringVector;

   }

std::string
DOTGraphNode::get_dot_options_string()
   {
     std::string optionsString;

  // "shape=polygon,regular=0,URL=\"\\N\",tooltip=\"more info at \\N\",sides=6,peripheries=1,color=\"Blue\",fillcolor=red,fontname=\"7x13bold\",fontcolor=black,style=filled"
     std::vector<std::string> optionsStringVector = get_dot_options_list();
 
     for(unsigned int i = 0; i < optionsStringVector.size(); i++)
        {
          if(i == 0)
               optionsString = optionsStringVector[0];
            else 
               optionsString += ", " + optionsStringVector[i];
        }

     return optionsString;
   }

#endif
