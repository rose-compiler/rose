#include "SDG.h"
#include "util.h"
#include <VariableRenaming.h>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/graph/graphviz.hpp>


#define foreach BOOST_FOREACH

using namespace std;
using namespace boost;


namespace SDG
{



void SystemDependenceGraph::build(SgProject* project)
{
    map<CFGVertex, Vertex> cfgVerticesToSdgVertices;
    map<SgFunctionCallExp*, vector<SDGNode*> > funcCallToArgs;
    
    vector<SgFunctionDefinition*> funcDefs = SageInterface::querySubTree<SgFunctionDefinition>(project, V_SgFunctionDefinition);
    foreach (SgFunctionDefinition* funcDef, funcDefs)
    {
        ControlFlowGraph* cfg = new ControlFlowGraph(funcDef, cfgNodefilter_);
        functionsToCFGs_[funcDef] = cfg;
        
        // For each function, build an entry node for it.
        SDGNode* entry = new SDGNode(SDGNode::Entry);
        entry->funcDef = funcDef;
        functionsToEntries_[funcDef] = entry;
        Vertex entryVertex = addVertex(entry);
        
        // Add all CFG vertices to SDG.
        foreach (CFGVertex cfgVertex, boost::vertices(*cfg))
        {
            if (cfgVertex == cfg->getEntry() || cfgVertex == cfg->getExit())
                continue;

            SgNode* astNode = (*cfg)[cfgVertex]->getNode();
            
            // Add a new node to SDG.
            SDGNode* newSdgNode = new SDGNode(SDGNode::ASTNode);
            newSdgNode->cfgNode = (*cfg)[cfgVertex];
            newSdgNode->astNode = astNode;
            Vertex sdgVertex = addVertex(newSdgNode);
            cfgVerticesToSdgVertices[cfgVertex] = sdgVertex;
            

            // If this CFG node contains a function call expression, extract its all parameters
            // and make them as actual-in nodes.
            
            if (SgFunctionCallExp* funcCallExpr = isSgFunctionCallExp(astNode))
            {
                // Change the node type.
                newSdgNode->type = SDGNode::FunctionCall;
                vector<SDGNode*> argsNodes;
                
                SgExprListExp* args = funcCallExpr->get_args();
                foreach (SgExpression* arg, args->get_expressions())
                {
                    SDGNode* paraNode = new SDGNode(SDGNode::ActualIn);
                    paraNode->astNode = arg;
                    argsNodes.push_back(paraNode);
                    
                    // Add a actual-in parameter node.
                    Vertex paraVertex = addVertex(paraNode);
                    
                    // Add a CD edge from call node to this actual-in node.
                    SDGEdge* paraInEdge = new SDGEdge(SDGEdge::ControlDependence);
                    paraInEdge->setTrue();
                    addEdge(sdgVertex, paraVertex, paraInEdge);
                }
                funcCallToArgs[funcCallExpr] = argsNodes;
            }
        }
        
        // Add control dependence edges.
        addControlDependenceEdges(cfgVerticesToSdgVertices, *cfg, entryVertex);
    
        // Add data dependence edges.
        addDataDependenceEdges(cfgVerticesToSdgVertices, *cfg);
        
    }
    
    // Add call edges.
    
    // Add parameter-in edges.
    
    // Add parameter-out edges.
    
    // Compute summary edges and add them.
}

void SystemDependenceGraph::addControlDependenceEdges(
        const std::map<CFGVertex, Vertex>& cfgVerticesToSdgVertices,
        const ControlFlowGraph& cfg,
        Vertex entry)
{
	// Build the dominance frontiers of the reverse CFG, which represents the CDG
	// of the original CFG.
    ControlFlowGraph rvsCfg = cfg.makeReverseCopy();
	DominanceFrontiersT domFrontiers = buildDominanceFrontiers(rvsCfg);
    
    foreach (const DominanceFrontiersT::value_type& vertices, domFrontiers)
	{
		Vertex src, tar;
        
		CFGVertex from = vertices.first;
        
        if (from == cfg.getEntry() || from == cfg.getExit())
            continue;
        
        ROSE_ASSERT(cfgVerticesToSdgVertices.count(from));
        src = cfgVerticesToSdgVertices.find(from)->second;

		typedef std::pair<CFGVertex, std::vector<CFGEdge> > VertexEdges;
		foreach (const VertexEdges& vertexEdges, vertices.second)
		{            
			CFGVertex to = vertexEdges.first;
			const std::vector<CFGEdge>& cdEdges = vertexEdges.second;
            
            ROSE_ASSERT(cfgVerticesToSdgVertices.count(to));
            tar = cfgVerticesToSdgVertices.find(to)->second;

			foreach (const CFGEdge& cdEdge, cdEdges)
			{
				// Add the edge.
				Edge edge = boost::add_edge(tar, src, *this).first;
                //(*this)[edge].cfgEdge   = edgeTable[rvsCfg[cdEdge]];
                (*this)[edge] = new SDGEdge(SDGEdge::ControlDependence);
				(*this)[edge]->condition = rvsCfg[cdEdge]->condition();
				(*this)[edge]->caseLabel = rvsCfg[cdEdge]->caseLabel();
			}
		}
	}
    
    // Connect an edge from the entry to every node which does not have a control dependence.
    typedef pair<CFGVertex, Vertex> T;
    foreach (const T& v, cfgVerticesToSdgVertices)
    {
        Vertex sdgVertex = v.second;
        
        if (sdgVertex == entry) continue;
        
        if (boost::in_degree(sdgVertex, *this) == 0)
        {
            Edge edge = boost::add_edge(entry, sdgVertex, *this).first;
            //(*this)[edge].cfgEdge   = edgeTable[rvsCfg[cdEdge]];
            (*this)[edge] = new SDGEdge(SDGEdge::ControlDependence);
            (*this)[edge]->condition = VirtualCFG::eckTrue;
        }
    }
}


void SystemDependenceGraph::addDataDependenceEdges(
        const std::map<CFGVertex, Vertex>& cfgVerticesToSdgVertices,
        const ControlFlowGraph& cfg)
{

}


void SystemDependenceGraph::toDot(const std::string& filename) const
{
	std::ofstream ofile(filename.c_str(), std::ios::out);
	boost::write_graphviz(ofile, *this,
		boost::bind(&SystemDependenceGraph::writeGraphNode, this, ::_1, ::_2),
		boost::bind(&SystemDependenceGraph::writeGraphEdge, this, ::_1, ::_2));
}

void SystemDependenceGraph::writeGraphNode(std::ostream& out, const Vertex& vertex) const
{
    //if (node == entry_)
    //{
    //    out << "[label=\"ENTRY\"]";
    //}
    //else
    //    writeCFGNode(out, *(*this)[node]);
    
    std::string nodeColor = "black";
    std::string label;
            
    SDGNode* sdgNode = (*this)[vertex];
    switch (sdgNode->type)
    {
    case SDGNode::Entry:
        label = "Entry\\n";
        ROSE_ASSERT(sdgNode->funcDef);
        label += sdgNode->funcDef->get_declaration()->get_name();
        nodeColor = "orange";
        break;

    case SDGNode::ASTNode:
    {
        SgNode* node = sdgNode->astNode;
        ROSE_ASSERT(node);

        if (isSgStatement(node))
            nodeColor = "blue";
        else if (isSgExpression(node))
            nodeColor = "green";
        else if (isSgInitializedName(node))
            nodeColor = "red";

        if (!isSgScopeStatement(node) && !isSgCaseOptionStmt(node) && !isSgDefaultOptionStmt(node))
        {
            std::string content = node->unparseToString();
            boost::replace_all(content, "\"", "\\\"");
            boost::replace_all(content, "\\n", "\\\\n");
            label += content;
        }
        else
            label += "<" + node->class_name() + ">";

        if (label == "")
            label += "<" + node->class_name() + ">";
    }
        break;
      
    case SDGNode::FunctionCall:
        label = "CALL\\n";
        ROSE_ASSERT(isSgFunctionCallExp(sdgNode->astNode));
        label += sdgNode->astNode->unparseToString();
        nodeColor = "purple";
        break;
        
    case SDGNode::ActualIn:
        label = "ActualIn";
        break;
        
    case SDGNode::ActualOut:
        label = "ActualOut";
        break;
        
    case SDGNode::FormalIn:
        label = "FormalIn";
        break;
        
    case SDGNode::FormalOut:
        label = "FormalOut";
        break;
        
    default:
        break;
    }
	
	out << "[label=\""  << label << "\", color=\"" << nodeColor << "\"]";
}

void SystemDependenceGraph::writeGraphEdge(std::ostream& out, const Edge& edge) const
{
	std::string str, style;
	SDGEdge* sdgEdge = (*this)[edge];
	if (sdgEdge->type == SDGEdge::ControlDependence)
	{
        std::string label;
        switch (sdgEdge->condition)
        {
            case VirtualCFG::eckTrue:
                label = "T";
                break;
            case VirtualCFG::eckFalse:
                label = "F";
                break;
            case VirtualCFG::eckCaseLabel:
                label = "case " + sdgEdge->caseLabel->unparseToString();
                break;
            case VirtualCFG::eckDefault:
                label = "default";
                break;
            default:
                break;
        }
        out << "[label=\"" << label << "\", style=\"" << "solid" << "\"]";
	}
	else
	{
		foreach (const VarName& varName, (*this)[edge]->varNames)
			str += VariableRenaming::keyToString(varName) + " ";
		style = "dotted";
		out << "[label=\"" << str << "\", style=\"" << style << "\"]";
	}
}


} // end of namespace SystemDependenceGraph
