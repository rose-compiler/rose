// tps (1/14/2010) : Switching from rose.h to sage3 changed size from 20,9 MB to 10,7MB
#include "sage3basic.h"

#define SIMPLE_FILTER

#include "DependenceGraph.h"
#include<iostream>
using namespace std;
const char *DependenceNode::typeNames[DependenceNode::NUM_NODE_TYPES] = {
    "CONTROL",
    "SGNODE",
    "CALLSITE",
    "ACTUALIN",
    "ACTUALOUT",
    "FORMALIN",
    "FORMALOUT",
    "ENTRY",
    "ACTUALRETURN",
    "FORMALRETURN"
};

const char *DependenceGraph::edgeNameArray[8] = {
/*    "", "CONTROL", "DATA", "", "SUMMARY", "", "", "", "CALL", "", "", "", "",
    "", "", "", "RETURN"*/
    "UNKNOWN",
                "CONTROL", 
                "DATA", 
                "SUMMARY",
                "CALL",
                "RETURN",
                "PARAMETER_IN",
                "PARAMETER_OUT"
};

const char *DependenceGraph::getEdgeName(EdgeType type)
{
        // int offset=0;
        switch(type)
        {
    case SYNTACTIC:
      return "SYNTACTIC";
                case CONTROL:
                        return "CONTROL";
                case DATA:
                        return "DATA";
                case SUMMARY:
                        return "SUMMARY";
                case CALL:
                        return "CALL";
//              case RETURN:
//                      return "RETURN";
                case PARAMETER_IN:
                        return "PARAMETER_IN";
                case PARAMETER_OUT:
                        return "PARAMETER_OUT";
                case DATA_HELPER:
                        return "DATA_HELPER";
                case CONTROL_HELPER:
                        return "CONTROL_HELPER";
                case GLOBALVAR_HELPER:
                        return "GLOBALVAR_HELPER";
    case COMPLETENESS_HELPER:
      return "COMPLETENESS_HELPER";
                case BELONGS_TO:
                        return "BELONGS_TO";
                default:
                                return "UNKNOWN";
        }
}

/*
DependenceNode *DependenceGraph::createNode(DependenceNode * node)
{
    DependenceNode *newNode = getNode(node);

    // If the node isn't in the map, create it.
    if (newNode == NULL)
    {
        newNode = new DependenceNode(node);
        _depnode_map[node] = newNode;
        addNode(newNode);
    }

    return newNode;
}
*/
DependenceNode *DependenceGraph::createNode(SgNode * node)
{
        DependenceNode *depNode = new DependenceNode(DependenceNode::SGNODE,node);
        nodeTypeToDepNodeMapMap[DependenceNode::SGNODE][node]=depNode;
//      DependenceNode *depNode = new DependenceNode(node);
//      sgNodeToDepNodeMap[node]=depNode;
        addNode(depNode);
  return depNode;
}
/*
    edgeTypeMap[e].insert(Edge(from, to));
    edgeMap[Edge(from, to)].insert(e);*/
void DependenceGraph::deleteNode(DependenceNode * node)
{
        // to be save, check for edges...
        std::set<SimpleDirectedGraphNode *> tmp=node->getSuccessors();
        for (std::set<SimpleDirectedGraphNode *>::iterator i=tmp.begin();i!=tmp.end();i++)
        {
                removeLink(node,*i);
        }
        tmp=node->getPredecessors();
        for (std::set<SimpleDirectedGraphNode *>::iterator i=tmp.begin();i!=tmp.end();i++)
        {
                removeLink(*i,node);
        }
        // now it is safe to remove this node
        DependenceNode::NodeType type=node->getType();
        SgNode * identifyingNode = node->getSgNode();
        nodeTypeToDepNodeMapMap[type].erase(identifyingNode);
        removeNode(node);
        free(node);
}

DependenceNode *DependenceGraph::createNode(DependenceNode::NodeType type,SgNode * identifyingNode)
{
        DependenceNode *depNode = new DependenceNode(type,identifyingNode);
        nodeTypeToDepNodeMapMap[type][identifyingNode]=depNode;
        addNode(depNode);
        return depNode;
}
DependenceNode *DependenceGraph::getNode(DependenceNode::NodeType type,SgNode * identifyingNode)
{
        if(nodeTypeToDepNodeMapMap.count(type) && nodeTypeToDepNodeMapMap[type].count(identifyingNode))
                return nodeTypeToDepNodeMapMap[type][identifyingNode];  
        else
                return createNode(type,identifyingNode);                
}
                                                                               
DependenceNode *DependenceGraph::getExistingNode(DependenceNode::NodeType type,SgNode * identifyingNode)
{
  if (identifyingNode==NULL) return NULL;
        if(nodeTypeToDepNodeMapMap.count(type) && nodeTypeToDepNodeMapMap[type].count(identifyingNode))
                return nodeTypeToDepNodeMapMap[type][identifyingNode];  
        else return NULL;
}

DependenceNode *DependenceGraph::getExistingNode(SgNode * identifyingNode)
{
        return getExistingNode(DependenceNode::SGNODE,identifyingNode);
}


/*
DependenceNode *DependenceGraph::getNode(DependenceNode * node)
{
        if (sgNodeToDepNodeMap.count(node))
        {
                return sgNodeToDepNodeMap[node];
        }
        else return createNode(node);
}

*/
DependenceNode *DependenceGraph::getNode(SgNode * node)
{

        if (node==NULL) ROSE_ASSERT(false);
        return getNode(DependenceNode::SGNODE,node);
                DependenceNode * retVal;
//              cout << "\tgetNode for "<<node->unparseToString()<<"\t";
    if (sgNodeToDepNodeMap.count(node))
    {
                                retVal=sgNodeToDepNodeMap[node];
//                              cout << "returning existing node Ptr("<<retVal<<")\n";
        return retVal;
    }
    else
    {   
                        retVal=createNode(node);
//                              cout << "creating new node Ptr("<<retVal<<")\n";
        return retVal;
    }
}
void DependenceGraph::removeEdge(DependenceNode * from, DependenceNode * to, EdgeType e)
{
        if (!edgeTypeMap.count(e))
        {       
                std::cerr<<"EdgeType does not exist"<<endl;
                exit(-1);
        }
        if (!edgeTypeMap[e].count(Edge(from, to)))
        {
                std::cerr<<"Edge does not exist"<<endl;
                exit(-1);
        }
        // cout <<"removing in edgeTypeMap"<<endl;
        edgeTypeMap[e].erase(Edge(from, to));
        // cout <<"removing in edgeMap"<<endl;
        edgeMap[Edge(from, to)].erase(e);
}
void DependenceGraph::establishEdge(DependenceNode * from, DependenceNode * to, EdgeType e)
{
                // internal Book-keeping
    edgeTypeMap[e].insert(Edge(from, to));
    edgeMap[Edge(from, to)].insert(e);
                // link those two nodes using the SimpleGraphInterface
    addLink(from, to);
}

bool DependenceGraph::edgeExists(DependenceNode * from, DependenceNode * to, EdgeType e)
{
        return edgeTypeMap[e].count(Edge(from, to));
}
bool DependenceGraph::hasOutgingEdge(DependenceNode * src,EdgeType compare)
{
  set<SimpleDirectedGraphNode *> successors=src->getSuccessors();
  for (set<SimpleDirectedGraphNode *>::iterator i=successors.begin();
       i!=successors.end();
       i++)
  {
    DependenceNode * dst=dynamic_cast < DependenceNode * >(*i);
    if (edgeType(src,dst).count(compare)>0)
    {
        return true;
    }
  }
  return false;
}

std::set < DependenceGraph::EdgeType >
    DependenceGraph::edgeType(DependenceNode * from, DependenceNode * to)
{
// DQ (8/30/2009): Debugging ROSE compiling ROSE (this statement does not compile using ROSE).
// This is part of an error in DependenceGraph.h that is also commented out when ROSE is used to compile ROSE.
#ifndef USE_ROSE
    return edgeMap[Edge(from, to)];
#else
    return std::set < DependenceGraph::EdgeType >();
#endif
}

void DependenceGraph::writeDot(char *filename)
{

                std::vector<std::string> styles;
    std::ofstream f(filename);

    f << "digraph \"G" << filename << "\" {" << std::endl;
    // output all of the nodes
    std::set < SimpleDirectedGraphNode * >::iterator i;
    for (i = _nodes.begin(); i != _nodes.end(); i++)
    {
                                styles.clear();
        DependenceNode *node = dynamic_cast < DependenceNode * >(*i);
                
        SimpleDirectedGraphNode *d = *i;
        char buf[sizeof(SimpleDirectedGraphNode *) * 2 + 3];

        sprintf(buf, "%p", d);
        f << "\"" << buf << "\" [";
                                // label
                                f<<"label = \"";
        _displayData(d, f);
        f << "\"";
                                if (node->isHighlighted())
                                {
                                        f<<",color=red";
                                        styles.push_back(string("bold"));
                                }
                                
                                //append a shape
                                switch(node->getType())
                                {
                                        case DependenceNode::ENTRY:
                                                f<<",shape=hexagon";
                                                f<<",fillcolor=deepskyblue";
                                                styles.push_back(string("filled"));
                                                break;
                                        case DependenceNode::ACTUALIN:
                                        case DependenceNode::ACTUALOUT:
                                        case DependenceNode::FORMALIN:
                                        case DependenceNode::FORMALOUT:
                                        case DependenceNode::FORMALRETURN:
          case DependenceNode::ACTUALRETURN:
                                                f<<",shape=box";
                                                break;                                  
                                        default:
                                        //      f<<"shape=box";
                                                break;
                                        
                                }
                                f<<",style=\"";
                                for (unsigned int st=0;st<styles.size();st++)
                                {
                                        if(st!=0)
                                         f<<",";
                                                f<<styles[st];                                  
                                }
                                f<<"\"";
                                // end the properties for that node
                                f<<"];" << std::endl;
    }

    // output all of the edges (we'll just use successor edges
    for (i = _nodes.begin(); i != _nodes.end(); i++)
    {
        DependenceNode *d1 = dynamic_cast < DependenceNode * >(*i);

        std::set < SimpleDirectedGraphNode * >succs = d1->getSuccessors();
        std::set < SimpleDirectedGraphNode * >::iterator j;
        for (j = succs.begin(); j != succs.end(); j++)
        {
            DependenceNode *d2 = dynamic_cast < DependenceNode * >(*j);

            char buf1[sizeof(DependenceNode *) * 2 + 3];
            char buf2[sizeof(DependenceNode *) * 2 + 3];

            sprintf(buf1, "%p", d1);
            sprintf(buf2, "%p", d2);

            f << "\"" << buf1 << "\" -> \"" << buf2 << "\"";
            std::set < EdgeType > ets = edgeType(d1, d2);
            EdgeType et = *(ets.begin());

                                                bool highlightEdge=false;
                                                if (d1->isHighlighted() && d2->isHighlighted())
                                                {
                                                        highlightEdge=true;
                                                }

            f << "[";
                                                f<<"label=\"" << getEdgeName(et) << "\"";
                                                if (!highlightEdge)
                                                switch(et)
                                                {
                                                        case SUMMARY:
                                                                f<<",color=cyan";
                                                                break;
                                                        case CONTROL:
                                                                f<<",color=blue";
                                                                break;
                                                        case DATA:
                                                                f<<",color=green";
                                                                break;
                                                        case CALL:
                                                        f<<",color=gold1";
                break;
              case SYNTACTIC:
              f<<",color=gray";
                                                        default:
                                                        break;
                                                }
                                                else
                                                {
                                                        f<<",style=bold";
                                                        f<<",color=red";
                                                }
                                                f<<"];" << std::endl;
        }
    }

    f << "}" << std::endl;
}



bool IsImportantForSliceSgFilter(SgNode * n){
#if 0
                // get rid of all beginning nodes     if (!cfgn.isInteresting())       return false;
                SgNode *n = cfgn.getNode();     if (isSgExprStatement(n))       return true;
                if (isSgReturnStmt(n))       return true;     // the following should not be necessary
                if (isSgFunctionDefinition(n))       return true;     // if (isSgStatement(n)) return true;
                // check for for-loop incremental expression
                if ((isSgBinaryOp(n) || isSgUnaryOp(n)) && isSgExpression(n))     {
                        // if the parent is a for statement, then true else false
                        SgNode *current = n;
                        while (isSgExpression(current->get_parent()))
                        {
                                current = current->get_parent();
                        }
                        if (isSgForStatement(current->get_parent()))
                                return true;

                }
                return false;
#endif


                // statements are intersting, but not the FunctionparameterList or SgExprStatements
    // Jim Leek: 2/21/2009: Defuse now ignores SgExprStatements, so we should too.  
    // Added to fix problem with dupicate nodes appearing in Dominance Tree.  
    if(isSgExprStatement(n)) {
      return false;
    }
                // function calls are now always interesting, since we ignore sgExprStatements.
                if (isSgFunctionCallExp(n))
                {
      return true;
                }

                // get rid of all beginning nodes
#ifdef SIMPLE_FILTER    
                if (isSgStatement(n))
                        return true;            
                // JJW
                if (isSgExpression(n) && !isSgExpression(n->get_parent()))
                  return true;
                return false;   
#else                   
/*              if (isSgForStatement(n))
                        return false;*/
                // statements are intersting, but not the FunctionparameterList
                if (isSgStatement(n))
                        if (!isSgFunctionParameterList(n))
                                return true;
                if (isSgInitializedName(n))
                {
                        if (!isSgVariableDeclaration(n->get_parent()))
                        return true;
                }
                // function calls are interesting, but ONLY if they are not yet shown in another way..
                if (isSgFunctionCallExp(n))
                {
                        if (!isSgExprStatement(n->get_parent()))
//                      if (!isSgStatement(n->get_parent()))
                                return true;
                }
                // check for for-loop incremental expression
                if ((isSgBinaryOp(n) || isSgUnaryOp(n)) && isSgExpression(n))
                {
                        // if the parent is a for statement, then true else false
                        SgNode *current = n;
                        while (isSgExpression(current->get_parent()))
                        {
                                current = current->get_parent();
                        }
                        if (isSgForStatement(current->get_parent()))
                                return true;
                }
                
                return false;
#endif          
                
                /*
                if (isSgExprStatement(n))
                        return true;
                if (isSgReturnStmt(n))
                        return true;
                // the following should not be necessary
                if (isSgFunctionDefinition(n))
                        return true;
                // if (isSgStatement(n)) return true;
                return false;
*/
        }

NodeQuerySynthesizedAttributeType queryIsImportantForSliceType(SgNode * astNode)
{
        NodeQuerySynthesizedAttributeType retVal;
        if (IsImportantForSliceSgFilter(astNode))
                retVal.push_back(astNode);
        return retVal;
}

NodeQuerySynthesizedAttributeType queryIsImportantForSliceTypeWithCalls(SgNode * astNode)
{
        NodeQuerySynthesizedAttributeType retVal;
        if (IsImportantForSliceSgFilter(astNode) || isSgFunctionCallExp(astNode))
                retVal.push_back(astNode);
        return retVal;
}

