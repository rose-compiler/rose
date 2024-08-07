/* 
 * File:   SDG.C
 * Author: Cong Hou [hou_cong@gatech.edu]
 */

#include "sage3basic.h"                                 // every librose .C file must start with this

#include "SDG.h"
#include "util.h"
#include <VariableRenaming.h>
#include <boost/algorithm/string.hpp>
#include <boost/bind/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/graphviz.hpp>

using namespace std;

namespace SDG
{

//! A DFS visitor used in depth first search.
template <typename VertexT> 
    struct DFSVisitor : public boost::default_dfs_visitor
{
    DFSVisitor(set<VertexT>& vertices) : vertices_(vertices) {}
    template <typename Vertex, typename Graph>
        void discover_vertex(Vertex u, const Graph & g)
        { vertices_.insert(u); }
    set<VertexT>& vertices_;
};


void SDGEdge::setCondition(VirtualCFG::EdgeConditionKind cond, SgExpression* expr)
{
    switch (cond)
    {
        case VirtualCFG::eckTrue:
            condition = cdTrue;
            break;
        case VirtualCFG::eckFalse:
            condition = cdFalse;
            break;
        case VirtualCFG::eckCaseLabel:
            condition = cdCase;
            caseLabel = expr;
            break;
        case VirtualCFG::eckDefault:
            condition = cdDefault;
            break;
        default:
            break;
    }
}

void SystemDependenceGraph::build()
{
    boost::unordered_map<CFGVertex, Vertex> cfgVerticesToSdgVertices;
    boost::unordered_map<SgNode*, Vertex>   astNodesToSdgVertices;

    //map<SgFunctionCallExp*, vector<SDGNode*> > funcCallToArgs;

    vector<CallSiteInfo> functionCalls;


    map<SgNode*, vector<Vertex> > actualInParameters;
    map<SgNode*, vector<Vertex> > actualOutParameters;

    map<SgNode*, Vertex> formalInParameters;
    map<SgNode*, Vertex> formalOutParameters;

    vector<SgFunctionDefinition*> funcDefs = 
        SageInterface::querySubTree<SgFunctionDefinition>(project_, V_SgFunctionDefinition);
    for (SgFunctionDefinition* funcDef : funcDefs)
    {
        SgFunctionDeclaration* funcDecl = funcDef->get_declaration();

        CFG* cfg = new CFG(funcDef, cfgNodefilter_);
        functionsToCFGs_[funcDecl] = cfg;

        // For each function, build an entry node for it.
        SDGNode* entry = new SDGNode(SDGNode::Entry);
        entry->astNode = funcDef;
        //entry->funcDef = funcDef;
        Vertex entryVertex = addVertex(entry);
        functionsToEntries_[funcDecl] = entryVertex;

        // Add all out formal parameters to SDG.
        const SgInitializedNamePtrList& formalArgs = funcDecl->get_args();
        for (SgInitializedName* initName : formalArgs)
        {
            // If the parameter is passed by reference, create a formal-out node.
            if (isParaPassedByRef(initName->get_type()))
            {
                SDGNode* formalOutNode = new SDGNode(SDGNode::FormalOut);
                formalOutNode->astNode = initName;
                Vertex formalOutVertex = addVertex(formalOutNode);
                formalOutParameters[initName] = formalOutVertex;

                // Add a CD edge from call node to this formal-out node.
                addTrueCDEdge(entryVertex, formalOutVertex);
            }
        }

        // A vertex representing the returned value.
        Vertex returnVertex;

        // If the function returns something, build a formal-out node.
        if (!isSgTypeVoid(funcDecl->get_type()->get_return_type()))
        {
            SDGNode* formalOutNode = new SDGNode(SDGNode::FormalOut);
            // Assign the function declaration to the AST node of this vertex to make
            // it possible to classify this node into the subgraph of this function.
            formalOutNode->astNode = funcDecl;
            returnVertex = addVertex(formalOutNode);
            formalOutParameters[funcDecl] = returnVertex;

            // Add a CD edge from call node to this formal-out node.
            addTrueCDEdge(entryVertex, returnVertex);
        }

        // Add all CFG vertices to SDG.
        for (CFGVertex cfgVertex : boost::vertices(*cfg))
        {
            if (cfgVertex == cfg->getEntry() || cfgVertex == cfg->getExit())
                continue;

            SgNode* astNode = (*cfg)[cfgVertex]->getNode();

            // If this node is an initialized name and it is a parameter, make it 
            // as a formal in node.
            SgInitializedName* initName = isSgInitializedName(astNode);
            if (initName && isSgFunctionParameterList(initName->get_parent()))
            {
                SDGNode* formalInNode = new SDGNode(SDGNode::FormalIn);
                formalInNode->astNode = initName;
                Vertex formalInVertex = addVertex(formalInNode);
                formalInParameters[initName] = formalInVertex;

                cfgVerticesToSdgVertices[cfgVertex] = formalInVertex;
                astNodesToSdgVertices[astNode] = formalInVertex;

                // Add a CD edge from call node to this formal-in node.
                addTrueCDEdge(entryVertex, formalInVertex);
                continue;
            }

            // Add a new node to SDG.
            SDGNode* newSdgNode = new SDGNode(SDGNode::ASTNode);
            //newSdgNode->cfgNode = (*cfg)[cfgVertex];
            newSdgNode->astNode = astNode;
            Vertex sdgVertex = addVertex(newSdgNode);

            cfgVerticesToSdgVertices[cfgVertex] = sdgVertex;
            astNodesToSdgVertices[astNode] = sdgVertex;


            // Connect a vertex containing the return statement to the formal-out return vertex.
            if (isSgReturnStmt(astNode)
                    || isSgReturnStmt(astNode->get_parent()))
            {
                SDGEdge* newEdge = new SDGEdge(SDGEdge::DataDependence);
                addEdge(sdgVertex, returnVertex, newEdge);                
            }

            // If this CFG node contains a function call expression, extract its all parameters
            // and make them as actual-in nodes.

            if (SgFunctionCallExp* funcCallExpr = isSgFunctionCallExp(astNode))
            {
                CallSiteInfo callInfo;
                callInfo.funcCall = funcCallExpr;
                callInfo.vertex = sdgVertex;

                // Change the node type.
                newSdgNode->type = SDGNode::FunctionCall;
                vector<SDGNode*> argsNodes;

                // Get the associated function declaration.
                SgFunctionDeclaration* funcDecl = funcCallExpr->getAssociatedFunctionDeclaration();
                
                if (funcDecl == NULL) 
                    continue;
                    
                ROSE_ASSERT(funcDecl);
                const SgInitializedNamePtrList& formalArgs = funcDecl->get_args();

                SgExprListExp* args = funcCallExpr->get_args();
                const SgExpressionPtrList& actualArgs = args->get_expressions();
                
                if (formalArgs.size() != actualArgs.size())
                {
                    cout << "The following function has variadic arguments:\n";
                    cout << funcDecl->get_file_info()->get_filename() << endl;
                    cout << funcDecl->get_name() << formalArgs.size() << " " << actualArgs.size() << endl;
                    continue;
                }

                for (int i = 0, s = actualArgs.size(); i < s; ++i)
                {
                    // Make sure that this parameter node is added to SDG then we
                    // change its node type from normal AST node to a ActualIn arg.
                    ROSE_ASSERT(astNodesToSdgVertices.count(actualArgs[i]));

                    Vertex paraInVertex = astNodesToSdgVertices.at(actualArgs[i]);
                    SDGNode* paraInNode = (*this)[paraInVertex]; 
                    paraInNode->type = SDGNode::ActualIn;

                    actualInParameters[formalArgs[i]].push_back(paraInVertex);
                    callInfo.inPara.push_back(paraInVertex);

                    // Add a CD edge from call node to this actual-in node.
                    addTrueCDEdge(sdgVertex, paraInVertex);

                    // If the parameter is passed by reference, create a parameter-out node.
                    if (isParaPassedByRef(formalArgs[i]->get_type()))
                    {
                        SDGNode* paraOutNode = new SDGNode(SDGNode::ActualOut);
                        paraOutNode->astNode = actualArgs[i];
                        //argsNodes.push_back(paraInNode);

                        // Add an actual-out parameter node.
                        Vertex paraOutVertex = addVertex(paraOutNode);
                        actualOutParameters[formalArgs[i]].push_back(paraOutVertex);
                        callInfo.outPara.push_back(paraOutVertex);

                        // Add a CD edge from call node to this actual-out node.
                        addTrueCDEdge(sdgVertex, paraOutVertex);
                    }
                }

                if (!isSgTypeVoid(funcDecl->get_type()->get_return_type())) 
                {
                    // If this function returns a value, create a actual-out vertex.
                    SDGNode* paraOutNode = new SDGNode(SDGNode::ActualOut);
                    paraOutNode->astNode = funcCallExpr;

                    // Add an actual-out parameter node.
                    Vertex paraOutVertex = addVertex(paraOutNode);
                    actualOutParameters[funcDecl].push_back(paraOutVertex);
                    callInfo.outPara.push_back(paraOutVertex);
                    callInfo.isVoid = false;
                    callInfo.returned = paraOutVertex;

                    // Add a CD edge from call node to this actual-out node.
                    addTrueCDEdge(sdgVertex, paraOutVertex);
                }

                functionCalls.push_back(callInfo);
                //funcCallToArgs[funcCallExpr] = argsNodes;
            }
        }

        // Add control dependence edges.
        addControlDependenceEdges(cfgVerticesToSdgVertices, *cfg, entryVertex);
    }


    // Add data dependence edges.
    addDataDependenceEdges(astNodesToSdgVertices, functionCalls, formalOutParameters);


    //=============================================================================================//
    // Add call edges.
    for (const CallSiteInfo& callInfo : functionCalls)
    {
        SgFunctionDeclaration* funcDecl = isSgFunctionDeclaration(callInfo.funcCall->getAssociatedFunctionDeclaration());
        ROSE_ASSERT(funcDecl);
        if (functionsToEntries_.count(funcDecl))
            addEdge(callInfo.vertex, functionsToEntries_[funcDecl], new SDGEdge(SDGEdge::Call));
        else
            ;//ROSE_ASSERT(false);
    }

    //=============================================================================================//
    // Add parameter-in edges.
    typedef pair<SgNode*, Vertex> NodeVertex;

    for (const NodeVertex& nodeToVertex : formalInParameters)
    {
        for (Vertex vertex : actualInParameters[nodeToVertex.first])
        {
            addEdge(vertex, nodeToVertex.second, new SDGEdge(SDGEdge::ParameterIn));
        }
    }

    //=============================================================================================//
    // Add parameter-out edges.
    for (const NodeVertex& nodeToVertex : formalOutParameters)
    {
        for (Vertex vertex : actualOutParameters[nodeToVertex.first])
        {
            addEdge(nodeToVertex.second, vertex, new SDGEdge(SDGEdge::ParameterOut));
        }
    }


    //=============================================================================================//
    // Compute summary edges and add them.

    size_t verticesNum = boost::num_vertices(*this);


    // Check if this Actual-In vertex has any out-going edges. If not, the corresponding
    // function definition of this function does not exit. To be conservative, we have to
    // assume that each Actual-In parameter can affect the value of all Actual-Out parameters.
    for (const CallSiteInfo& callInfo : functionCalls)
    {
        if (callInfo.inPara.empty())
            continue;

        int outDegree = boost::out_degree(callInfo.inPara[0], *this);
        if (outDegree > 0)
            continue;

        for (Vertex actualIn : callInfo.inPara)
        {
            for (Vertex actualOut : callInfo.outPara)
                addEdge(actualIn, actualOut, new SDGEdge(SDGEdge::Summary));
        }
    }

    for (const CallSiteInfo& callInfo : functionCalls)
    {
        for (Vertex actualIn : callInfo.inPara)
        {
            set<Vertex> vertices;

            // Create a DFS visitor in which we add all nodes to vertices set.
            DFSVisitor<Vertex> dfsVisitor(vertices);
            // Build a vector of colors and set all initial colors to white.
            std::vector<boost::default_color_type> colors(verticesNum, boost::white_color);
            // Do a DFS.
            boost::depth_first_visit(*this, actualIn, dfsVisitor, &colors[0]);

            for (Vertex actualOut : callInfo.outPara)
            {
                if (vertices.count(actualOut))
                {
                    if (!boost::edge(actualIn, actualOut, *this).second)
                        addEdge(actualIn, actualOut, new SDGEdge(SDGEdge::Summary));
                }
            }
        }
    }

}

void SystemDependenceGraph::addTrueCDEdge(Vertex src, Vertex tgt)
{
    SDGEdge* newEdge = new SDGEdge(SDGEdge::ControlDependence);
    newEdge->setTrue();
    addEdge(src, tgt, newEdge);
}


void SystemDependenceGraph::addControlDependenceEdges(
        const boost::unordered_map<CFGVertex, Vertex>& cfgVerticesToSdgVertices,
        const CFG& cfg,
        Vertex entry)
{
    // Build the dominance frontiers of the reverse CFG, which represents the CDG
    // of the original CFG.
    CFG rvsCfg = cfg.makeReverseCopy();
    DominanceFrontiersT domFrontiers = buildDominanceFrontiers(rvsCfg);

    for (const DominanceFrontiersT::value_type& vertices : domFrontiers)
    {
        Vertex src, tar;

        CFGVertex from = vertices.first;

        if (from == cfg.getEntry() || from == cfg.getExit())
            continue;

        ROSE_ASSERT(cfgVerticesToSdgVertices.count(from));
        src = cfgVerticesToSdgVertices.find(from)->second;

        typedef pair<CFGVertex, vector<CFGEdge> > VertexEdges;
        for (const VertexEdges& vertexEdges : vertices.second)
        {            
            CFGVertex to = vertexEdges.first;
            const vector<CFGEdge>& cdEdges = vertexEdges.second;

            ROSE_ASSERT(cfgVerticesToSdgVertices.count(to));
            tar = cfgVerticesToSdgVertices.find(to)->second;

            for (const CFGEdge& cdEdge : cdEdges)
            {
                // Add the edge.
                Edge edge = boost::add_edge(tar, src, *this).first;
                (*this)[edge] = new SDGEdge(SDGEdge::ControlDependence);
                (*this)[edge]->setCondition(rvsCfg[cdEdge]->condition(), rvsCfg[cdEdge]->caseLabel());
            }
        }
    }

    // Connect an edge from the entry to every node which does not have a control dependence.
    typedef pair<CFGVertex, Vertex> T;
    for (const T& v : cfgVerticesToSdgVertices)
    {
        Vertex sdgVertex = v.second;

        if (sdgVertex == entry) continue;

        if (boost::in_degree(sdgVertex, *this) == 0)
        {
            Edge edge = boost::add_edge(entry, sdgVertex, *this).first;
            //(*this)[edge].cfgEdge   = edgeTable[rvsCfg[cdEdge]];
            (*this)[edge] = new SDGEdge(SDGEdge::ControlDependence);
            (*this)[edge]->setTrue();
        }
    }
}

namespace 
{
    inline bool isBasicStatement(SgNode* node)
    {
        if (isSgFunctionCallExp(node))
            return false;
        return node;
        //return isSgExpression(node) || isSgDeclarationStatement(node);
    }
}

void SystemDependenceGraph::addDataDependenceEdges(
        const boost::unordered_map<SgNode*, Vertex>& astNodesToSdgVertices,
        const vector<CallSiteInfo>& callSiteInfo,
        const map<SgNode*, Vertex>& formalOutPara)
{
    // Get the def-use chains from the generator.
    ROSE_ASSERT(!defUseChainGenerator_.empty());
    DefUseChains defUseChains;
    defUseChainGenerator_(project_, defUseChains);

    // Once we have Def-Use chains, we can add data dependence edges to SDG.
    // We only add edges between basic statements like expressions and declarations.

    // A table mapping each AST node to all SDG vertices which contains basic statements of it.
    typedef boost::unordered_map<SgNode*, vector<Vertex> > ASTNodeToVertices;
    typedef ASTNodeToVertices::iterator Iter;
    ASTNodeToVertices nodesToVerticesTable;

    // Build the table above.

    for (const DefUseChains::value_type& defUse : defUseChains)
    {
        set<SgNode*> defUses = defUse.second;
        defUses.insert(defUse.first);

        for (SgNode* use : defUses)
        {
            Iter iter = nodesToVerticesTable.find(use);
            if (iter == nodesToVerticesTable.end())
            {     
                vector<Vertex>& nodes = nodesToVerticesTable[use];
                while (isBasicStatement(use))
                {
                    boost::unordered_map<SgNode*, Vertex>::const_iterator it = 
                        astNodesToSdgVertices.find(use);
                    if (it != astNodesToSdgVertices.end())
                        nodes.push_back(it->second);
                    use = use->get_parent();
                }
            }
        }
    }

    // Add data dependence edges.

    map<pair<Vertex, Vertex>, set<VarName> > dataDependenceEdges;

    // First collect all defs on each DD edge.
    for (const DefUseChains::value_type& defUse : defUseChains)
    {
        SgNode* def = defUse.first;

        for (Vertex src : nodesToVerticesTable.at(def))
        {
            for (SgNode* use : defUse.second)
            {
                for (Vertex tgt : nodesToVerticesTable.at(use))
                {
                    // If the target is a formal-in node, we should retarget it to a formal-out node.
                    SDGNode* sdgNode = (*this)[tgt];
                    if (sdgNode->type == SDGNode::FormalIn)
                    {
                        ROSE_ASSERT(formalOutPara.count(sdgNode->astNode));
                        tgt = formalOutPara.find(sdgNode->astNode)->second;
                    }
                    dataDependenceEdges[make_pair(src, tgt)];
                    //SDGEdge* newEdge = new SDGEdge(SDGEdge::DataDependence);
                    //addEdge(src, tgt, newEdge);
                }
            }
        } 
    }

    // Add an edge from returned result of each function call to all uses of this function call.
    for (const CallSiteInfo& callInfo : callSiteInfo)
    {
        // Functions of void type does not return anything.
        if (callInfo.isVoid)
            continue;
            
        SgNode* node = (*this)[callInfo.returned]->astNode;
        if (isSgFunctionCallExp(node))
        {
            do
            {
                node = node->get_parent();
                if (astNodesToSdgVertices.count(node))
                    dataDependenceEdges[make_pair(callInfo.returned, astNodesToSdgVertices.at(node))];
            } 
            while (isBasicStatement(node));
        }
    }

    // Add those edges.
    typedef map<pair<Vertex, Vertex>, set<VarName> >::value_type T2;
    for (const T2& edges : dataDependenceEdges)
    {
        SDGEdge* newEdge = new SDGEdge(SDGEdge::DataDependence);
        addEdge(edges.first.first, edges.first.second, newEdge);
    }

}


void SystemDependenceGraph::toDot(const string& filename) const
{
    using namespace boost::placeholders; // for _1, _2, ... below

    ofstream ofile(filename.c_str(), ios::out);
    boost::write_graphviz(ofile, *this,
            boost::bind(&SystemDependenceGraph::writeGraphNode, this, ::_1, ::_2),
            boost::bind(&SystemDependenceGraph::writeGraphEdge, this, ::_1, ::_2),
            boost::bind(&SystemDependenceGraph::writeGraphProperty, this, ::_1));
}

void SystemDependenceGraph::writeGraphProperty(ostream& out) const
{
    map<string, pair<string, vector<int> > > subgraphNodes;

    for (Vertex vertex : boost::vertices(*this))
    {
        SDGNode* sdgNode = (*this)[vertex];

        if (sdgNode->astNode)
        {
            SgFunctionDeclaration* funcDecl = isSgFunctionDeclaration(sdgNode->astNode);
            if (!funcDecl)
                funcDecl = SageInterface::getEnclosingFunctionDeclaration(sdgNode->astNode);
            if (funcDecl)
            {
                string name = funcDecl->get_name();
                string mangledName = funcDecl->get_mangled_name();
                subgraphNodes[mangledName].first = name;
                subgraphNodes[mangledName].second.push_back(vertex);
            }
        }
    }

    typedef pair<string, pair<string, vector<int> > > T;
    for (const T& subgraph : subgraphNodes)
    {
        out << "subgraph cluster_" << subgraph.first << " {label=\"" << subgraph.second.first << "\";";
        for (int i : subgraph.second.second)
        {
            out << i << "; "; 
        }
        out << "}\n";
    }
}


void SystemDependenceGraph::writeGraphNode(ostream& out, const Vertex& vertex) const
{
    //if (node == entry_)
    //{
    //    out << "[label=\"ENTRY\"]";
    //}
    //else
    //    writeCFGNode(out, *(*this)[node]);

    string nodeColor = "black";
    string label;

    SDGNode* sdgNode = (*this)[vertex];        

    switch (sdgNode->type)
    {
        case SDGNode::Entry:
            label = "Entry\\n";
            ROSE_ASSERT(isSgFunctionDefinition(sdgNode->astNode));
            label += isSgFunctionDefinition(sdgNode->astNode)->get_declaration()->get_name();
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
                    string content = node->unparseToString();
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
            label = "Actual-In\\n";
            ROSE_ASSERT(sdgNode->astNode);
            label += sdgNode->astNode->unparseToString();
            nodeColor = "sienna";
            break;

        case SDGNode::ActualOut:
            label = "Actual-Out\\n";
            ROSE_ASSERT(sdgNode->astNode);
            label += sdgNode->astNode->unparseToString();
            nodeColor = "turquoise";
            break;

        case SDGNode::FormalIn:
            label = "Formal-In\\n";
            ROSE_ASSERT(sdgNode->astNode);
            label += sdgNode->astNode->unparseToString();
            nodeColor = "sienna";
            break;

        case SDGNode::FormalOut:
            label = "Formal-Out\\n";
            // The return formal-out node has a SgFunctionDeclaration inside.
            if (sdgNode->astNode)
            {
                if (isSgFunctionDeclaration(sdgNode->astNode))
                    label += "Return";
                else
                    label += sdgNode->astNode->unparseToString();
            }
            nodeColor = "turquoise";
            break;

        default:
            break;
    }

    out << "[label=\""  << label << "\", color=\"" << nodeColor << "\"]";

    // Draw subgraphs.

}

void SystemDependenceGraph::writeGraphEdge(ostream& out, const Edge& edge) const
{
    string label, style;
    SDGEdge* sdgEdge = (*this)[edge];

    switch (sdgEdge->type)
    {
        case SDGEdge::ControlDependence:
            switch (sdgEdge->condition)
            {
                case SDGEdge::cdTrue:
                    label = "T";
                    break;
                case SDGEdge::cdFalse:
                    label = "F";
                    break;
                case SDGEdge::cdCase:
                    label = "case " + sdgEdge->caseLabel->unparseToString();
                    break;
                case SDGEdge::cdDefault:
                    label = "default";
                    break;
                default:
                    break;
            }
            break;

        case SDGEdge::DataDependence:
            for (const VarName& varName : (*this)[edge]->varNames)
                label += VariableRenaming::keyToString(varName) + " ";
            style = "dotted";
            break;

        case SDGEdge::Call:
            //style = "invis";
            break;

        case SDGEdge::ParameterIn:
        case SDGEdge::ParameterOut:
            style = "dashed";
            break;

        case SDGEdge::Summary:
            style = "bold";
            break;

        default:
            break;
    }

    out << "[label=\"" << label << "\", style=\"" << style << "\"]";
}


} // end of namespace SystemDependenceGraph
