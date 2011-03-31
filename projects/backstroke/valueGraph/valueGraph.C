#include "valueGraph.h"
#include <sageBuilder.h>
#include <utilities/utilities.h>
#include <boost/foreach.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/timer.hpp>

namespace Backstroke
{

using namespace std;

#define foreach BOOST_FOREACH

void EventReverser::buildBasicValueGraph()
{
    // Build the SSA form of the given function.
    //SSA::NodeReachingDefTable defTable = getUsesAtNode(funcDef);

    // Build a vertex which is the start point of the search.
    //root_ = addValueGraphNode(new ValueGraphNode);
    root_ = addValueGraphNode(new ValueGraphNode);

    // First, add all parameters of the event to the value graph.
    SgInitializedNamePtrList& paraList = funcDef_->get_declaration()->get_args();
    foreach (SgInitializedName* var, paraList)
    {
        createValueNode(var);

        // FIXME State variable may not be parameters.
        // Add the variable into wanted set.
        valuesToRestore_.push_back(nodeVertexMap_[var]);
        stateVariables_.insert(VarName(1, var));
    }

    vector<SgNode*> nodes = BackstrokeUtility::querySubTree<SgNode>(funcDef_);
    foreach (SgNode* node, nodes)
    {
        cout << node->class_name() << endl;

        // Statement case: variable declaration.
        if (SgStatement* stmt = isSgStatement(node))
        {
            if (SgVariableDeclaration* varDecl = isSgVariableDeclaration(stmt))
            {
                foreach (SgInitializedName* initName, varDecl->get_variables())
                {
                    SgInitializer* initalizer = initName->get_initializer();

                    // A declaration without definition
                    if (initalizer == NULL)
                    {
                        createValueNode(initName);
                    }
                    else if (SgAssignInitializer* assignInit =
                            isSgAssignInitializer(initalizer))
                    {
                        SgExpression* operand = assignInit->get_operand();

                        ROSE_ASSERT(nodeVertexMap_.count(operand) > 0);
                        VGVertex rhsVertex = nodeVertexMap_[operand];

                        addVariableToNode(rhsVertex, initName);
                    }
                    else
                    {
                        //cout << initalizer->class_name() << endl;
                        ROSE_ASSERT(!"Can only deal with assign initializer now!");
                    }
                }
            }
        }

        // Expression case.
        if (SgExpression* expr = isSgExpression(node))
        {
            //cout << ">>>>>>>" << expr->unparseToString() << " " << SageInterface::get_name(expr->get_type()) << endl;

            // For a variable reference, if its def is a phi node, we build this phi node here.
            if (BackstrokeUtility::isVariableReference(expr))
            {
                // Get the var name and version for lhs.
                // We don't know if this var is a use or def now.
                VersionedVariable var = getVersionedVariable(expr);

#if 1
                cout << "Variable node:" << var << endl;
#endif

                if (var.isPseudoDef)
                {
                    // If there is no node of var in the VG.
                    if (varVertexMap_.find(var) == varVertexMap_.end())
                    {
                        addValueGraphPhiNode(var);
                    }

                    ROSE_ASSERT(varVertexMap_.find(var) != varVertexMap_.end());

                    // Add the node -> vertex to nodeVertexMap_
                    nodeVertexMap_[expr] = varVertexMap_.find(var)->second;
                }
                else
                {
                    //cout << var.version << " " << var.isPseudoDef << endl;

                    // If the variable is a use, it should already exist in the var->vertex table.
                    // Find it and update the node->vertex table.
                    if (varVertexMap_.count(var) > 0)
                        nodeVertexMap_[expr] = varVertexMap_[var];
                }

                //VersionedVariable var(varName, defTable[varName]->getRenamingNumber());
                //varVertexMap_[var] = boost::add_vertexvalueGraph_;
                //valueGraph_[newNode] = new ValueGraphNode(varName, defTable[varName]->getRenamingNumber());
                //valueGraph_[newNode] = newNode;
                //varVertexMap_[var] = v;
            }

            // Value expression.
            else if (SgValueExp* valueExp = isSgValueExp(expr))
            {
                createValueNode(valueExp);
                //addValueGraphNode(new ValueNode(valueExp), expr);
            }

            // Cast expression.
            else if (SgCastExp* castExp = isSgCastExp(expr))
            {
                ROSE_ASSERT(nodeVertexMap_.count(castExp->get_operand()));
                nodeVertexMap_[castExp] = nodeVertexMap_[castExp->get_operand()];
            }

            // Unary expressions.
            else if (SgUnaryOp* unaryOp = isSgUnaryOp(expr))
            {
                SgExpression* operand = unaryOp->get_operand();

                VariantT t = node->variantT();
                switch (t)
                {
                case V_SgPlusPlusOp:
                case V_SgMinusMinusOp:
                    {
                        VersionedVariable use = getVersionedVariable(operand);
                        //VersionedVariable def = getVersionedVariable(operand, false);

                        ROSE_ASSERT(varVertexMap_.count(use) > 0);

                        createOperatorNode(t, createValueNode(operand),
                                varVertexMap_[use], createValueOneNode());
                        break;
                    }
                
                default:
                    break;
                }
            }

            // Binary expressions.
            else if (SgBinaryOp* binOp = isSgBinaryOp(expr))
            {
                SgExpression* lhs = binOp->get_lhs_operand();
                SgExpression* rhs = binOp->get_rhs_operand();

                VariantT t = node->variantT();
                switch (t)
                {
                // For assign op, we assign the node which is assigned to with a variable name.
                case V_SgAssignOp:
                    {
                        if (BackstrokeUtility::isVariableReference(lhs))
                        {
                            ROSE_ASSERT(nodeVertexMap_.count(rhs) > 0);
                            VGVertex rhsVertex = nodeVertexMap_.find(rhs)->second;
                            addVariableToNode(rhsVertex, lhs);
                        }
                        else
                        {
                            ROSE_ASSERT(!"Only variable can be assigned now.");
                        }

                        break;
                    }

                    // The following three operations are constructive ones, and we create three operation
                    // nodes for them, since the all three operands can be restored from the other two.
                case V_SgAddOp:
                case V_SgSubtractOp:
                    {
                        // FIXME Check if the variable type is integet type here.

                        ROSE_ASSERT(nodeVertexMap_.count(lhs) > 0);
                        ROSE_ASSERT(nodeVertexMap_.count(rhs) > 0);

                        VGVertex result = createValueNode(expr);
                        VGVertex lhsv = nodeVertexMap_[lhs];
                        VGVertex rhsv = nodeVertexMap_[rhs];

                        VariantT t2 = (t == V_SgAddOp) ? V_SgSubtractOp : V_SgAddOp;
                        createOperatorNode(t, result, lhsv, rhsv);
                        //createOperatorNode(t2, lhsv, result, rhsv);
                        //createOperatorNode(V_SgSubtractOp, rhsv, result, lhsv);

                        break;
                    }

                case V_SgMultiplyOp:
                case V_SgDivideOp:
                case V_SgGreaterThanOp:
                case V_SgLessThanOp:
                    {
                        ROSE_ASSERT(nodeVertexMap_.count(lhs) > 0);
                        ROSE_ASSERT(nodeVertexMap_.count(rhs) > 0);

                        createOperatorNode(t, createValueNode(expr),
                                nodeVertexMap_[lhs], nodeVertexMap_[rhs]);

                        break;
                    }

                case V_SgPlusAssignOp:
                    {
                        VersionedVariable use = getVersionedVariable(lhs);

                        ROSE_ASSERT(varVertexMap_.count(use) > 0);
                        ROSE_ASSERT(nodeVertexMap_.count(rhs) > 0);

                        createOperatorNode(t, createValueNode(expr),
                                varVertexMap_[use], nodeVertexMap_[rhs]);
                        break;
                    }


                default:
                    break;
                }
            }
        }
    }
}


void EventReverser::buildValueGraph()
{
    // First, build the basic part of the value graph.
    buildBasicValueGraph();

    // At the end of the event, find the versions of all variables, 
    // and determine which variables are avaiable during the search of VG.
    typedef SSA::NodeReachingDefTable::value_type VarNameDefPair;
    foreach (const VarNameDefPair& nameDef, ssa_.getLastVersions(funcDef_->get_declaration()))
    {
        VarName name = nameDef.first;

        if (isStateVariable(name))
        {
            VersionedVariable var(name, nameDef.second->getRenamingNumber());

            ROSE_ASSERT(varVertexMap_.count(var) || (cout << var, 0));
            availableValues_.insert(varVertexMap_[var]);
        }
    }

    // Add a reverse edge for every non-ordered edge.
    vector<VGEdge> edges;
    VGEdgeIter e, f;
    for (boost::tie(e, f) = boost::edges(valueGraph_); e != f; ++e)
        edges.push_back(*e);

    foreach (VGEdge edge, edges)
    {
        VGVertex src = boost::source(edge, valueGraph_);
        VGVertex tar = boost::target(edge, valueGraph_);

        // If the edge is not connected to an operator node, make a reverse copy.
        if (isOperatorNode(valueGraph_[src]) == NULL &&
                isOperatorNode(valueGraph_[tar]) == NULL)
            addValueGraphEdge(src, tar, 0);
    }

    // Add all state saving edges.
    addStateSavingEdges();

    cout << "Number of nodes: " << boost::num_vertices(valueGraph_) << endl;
}

EventReverser::VGVertex EventReverser::addValueGraphPhiNode(VersionedVariable& var)
{
    // Add the phi node.
    VGVertex v = addValueGraphNode(new PhiNode(var));
    varVertexMap_[var] = v;

    ROSE_ASSERT(varReachingDefMap_.count(var) > 0);
    SSA::ReachingDefPtr reachingDef = varReachingDefMap_[var];

    foreach (SSA::ReachingDefPtr def, reachingDef->getJoinedDefs())
    {
        int v = def->getRenamingNumber();
        // If this def is also a phi node, add a varWithVersin entry
        // to the varReachingDefMap_ table.
        if (def->isPhiFunction())
        {
            VersionedVariable phiVar(var.name, v, true);
            varReachingDefMap_[phiVar] = def;
        }
        // If this def is not in the value graph, add it.

        var.phiVersions.push_back(PhiNodeDependence(v));
    }

    // Connect the pseudo def to real defs.
    foreach (const PhiNodeDependence& def, var.phiVersions)
    {
        // We cannot build a var like this since it may be another phi node.
        VersionedVariable varInPhi(var.name, def.version);

        cout << "Phi node dependence:" << varInPhi << endl;

        // If the node which defines this phi node is not in the table, it should be another phi node.
        if (varVertexMap_.find(varInPhi) == varVertexMap_.end())
        {
            if (varReachingDefMap_.count(varInPhi) > 0)
            {
                varInPhi.isPseudoDef = true;
                addValueGraphPhiNode(varInPhi);
            }
            else
                ROSE_ASSERT(false);
            ROSE_ASSERT(varVertexMap_.find(varInPhi) != varVertexMap_.end());
        }

        addValueGraphEdge(v, varVertexMap_[varInPhi], 0);
    }

    return v;
}

EventReverser::VGVertex EventReverser::addValueGraphNode(ValueGraphNode* newNode)
{
#if 0
    if (VariableNode* varNode = isVariableNode(newNode))
        cout << "New var added:" << varNode->var << endl;
#endif

    VGVertex v = boost::add_vertex(valueGraph_);
    valueGraph_[v] = newNode;
    //	if (sgNode)
    //		nodeVertexMap_[sgNode] = v;
    return v;
}

EventReverser::VGEdge EventReverser::addValueGraphEdge(
        EventReverser::VGVertex src, EventReverser::VGVertex tar, int cost)
{
    //VGEdge e = boost::add_edge(src, tar, valueGraph_).first;
    VGEdge e = boost::add_edge(src, tar, valueGraph_).first;
    valueGraph_[e] = new ValueGraphEdge(cost);
    return e;
}

EventReverser::VGEdge EventReverser::addValueGraphOrderedEdge(
        EventReverser::VGVertex src, EventReverser::VGVertex tar, int index)
{
    //VGEdge e = boost::add_edge(src, tar, valueGraph_).first;
    VGEdge e = boost::add_edge(src, tar, valueGraph_).first;
    valueGraph_[e] = new OrderedEdge(index);
    return e;
}

//void EventReverser::setNewDefNode(SgNode* defNode, VGVertex useVertex)
//{
//	// Get the var name and version for lhs.
//	VersionedVariable var = getVersionedVariable(defNode, false);
//
//	//// Find the node of rhs in the VG which should exist now.
//	//ROSE_ASSERT(nodeVertexMap_.count(rhs) > 0);
//	//Vertex rhsVertex = nodeVertexMap_.find(rhs)->second;
//
//	// Assign the node with the variable name and version.
//	//ROSE_ASSERT(node);
//
//	// If the node contains a temporary variable, replace it with a variable node,
//	// else build a new node and connect them.
//	VariableNode* varNode = isVariableNode(valueGraph_[useVertex]);
//	if (varNode && varNode->isTemp)
//	{
////			delete valueGraph_[useVertex];
////			valueGraph_[useVertex] = new VariableNode(var);
//		// If the node is a variable node, assign the var name to it.
//		varNode->setVariable(var);
//
//		// Update the node to vertex table.
//		nodeVertexMap_[defNode] = useVertex;
//		// Update the var to vertex map.
//		varVertexMap_[var] = useVertex;
//	}
////		else if (ValueNode* valNode = isValueNode(node))
////		{
////			// If the node is a value node, assign the var name to it.
////			valNode->var = var;
////
////			// Update the node to vertex table.
////			nodeVertexMap_[defNode] = useVertex;
////			// Update the var to vertex map.
////			varVertexMap_[var] = useVertex;
////		}
//	else
//	{
//		VGVertex newVertex = addValueGraphNode(new VariableNode(var), defNode);
//		addValueGraphEdge(newVertex, useVertex);
//
//		// Update tables.
//		nodeVertexMap_[defNode] = newVertex;
//		varVertexMap_[var] = newVertex;
//	}
//}

EventReverser::VGVertex EventReverser::createValueOneNode()
{
    SgValueExp* valueOne = SageBuilder::buildIntVal(1);
    return addValueGraphNode(new ValueNode(valueOne));
}


EventReverser::VGVertex EventReverser::createValueNode(SgNode* node)
{
    VGVertex newVertex; // = boost::add_vertex(valueGraph_);

    // If the AST node is a value node, create a new value node with the value.
    if (SgValueExp* valExp = isSgValueExp(node))
    {
        newVertex = addValueGraphNode(new ValueNode(valExp));
    }
    // If the AST node is not a variable node, create a empty value node.
    else if (isSgVarRefExp(node) == NULL && isSgInitializedName(node) == NULL)
    {
        SgExpression* expr = isSgExpression(node);
        ROSE_ASSERT(expr);
        
        newVertex = addValueGraphNode(new ValueNode(expr->get_type()));
    }
    else
    {
        // Get the var name and version for lhs.
        VersionedVariable var = getVersionedVariable(node, false);

        cout << "New var detected: " << var << endl;

        ValueNode* valNode = new ValueNode;
        valNode->vars.push_back(var);
        newVertex = addValueGraphNode(valNode);

        // Update tables.
        varVertexMap_[var] = newVertex;
    }

    nodeVertexMap_[node] = newVertex;

    return newVertex;
}

void EventReverser::addVariableToNode(EventReverser::VGVertex v, SgNode* node)
{
    ValueNode* valNode = isValueNode(valueGraph_[v]);
    ROSE_ASSERT(valNode);

    VersionedVariable var = getVersionedVariable(node, false);
    valNode->vars.push_back(var);

    varVertexMap_[var] = nodeVertexMap_[node] = v;
}

EventReverser::VGVertex EventReverser::createOperatorNode(
        VariantT t,
        EventReverser::VGVertex result,
        EventReverser::VGVertex lhs,
        EventReverser::VGVertex rhs)
{

    VGVertex op = addValueGraphNode(new OperatorNode(t));

    addValueGraphEdge(result, op, 0);
    addValueGraphOrderedEdge(op, lhs, 0);
    addValueGraphOrderedEdge(op, rhs, 1);

    return op;
}

void EventReverser::addStateSavingEdges()
{
    VGVertexIter v, w;
    for (boost::tie(v, w) = boost::vertices(valueGraph_); v != w; ++v)
    {
        ValueGraphNode* node = valueGraph_[*v];

        // Phi node does not have cost (we don't need it in search process).
        if (isPhiNode(node))
        {
        }
        else if (ValueNode* valNode = isValueNode(node))
        {
            int cost = 0;
            if (!valNode->isAvailable() && availableValues_.count(*v) == 0)
            {
                if (valNode->isTemp())
                    cost = getCost(valNode->type);
                else
                {
                    // The cost is the minimum one in all variables inside.
                    cost = INT_MAX;
                    foreach (const VersionedVariable& var, valNode->vars)
                        cost = min(cost, getCost(var.name.back()->get_type()));
                }
            }
            valNode->cost = cost;
            addValueGraphEdge(*v, root_, cost);
        }
    }
}

int EventReverser::getCost(SgType* t)
{
    ROSE_ASSERT(t);
    
    if (SgReferenceType* refType = isSgReferenceType(t))
    {
        t = refType->get_base_type();
    }

    switch (t->variantT())
    {
        case V_SgTypeBool:
            return sizeof(bool);
        case V_SgTypeInt:
            return sizeof(int);
        case V_SgTypeChar:
            return sizeof(char);
        case V_SgTypeShort:
            return sizeof(short);
        case V_SgTypeFloat:
            return sizeof(float);
        case V_SgTypeDouble:
            return sizeof(double);
        case V_SgClassType:
            ROSE_ASSERT(false);
            return 100;
        default:
            ROSE_ASSERT(!"Unknow type.");
            return 100;
    }

    ROSE_ASSERT(false);
    return 0;
}

VersionedVariable EventReverser::getVersionedVariable(SgNode* node, bool isUse)
{
    VarName varName = SSA::getVarName(node);
    int version;

    // First, check if there is any use at the given node. If not, it should be a def.
    const SSA::NodeReachingDefTable& defTable = ssa_.getUsesAtNode(node);
    SSA::NodeReachingDefTable::const_iterator iter = defTable.find(varName);
    if (iter != defTable.end())
    {
        //ROSE_ASSERT(defTable.count(varName) > 0);
        SSA::ReachingDefPtr reachingDef = defTable.find(varName)->second;
        version = reachingDef->getRenamingNumber();

        // If its reaching def is a phi function, it's a pseudo def.
        if (reachingDef->isPhiFunction())
        {
            VersionedVariable var(varName, version, true);
            varReachingDefMap_[var] = reachingDef;
            return var;
        }
    }
    else
    {
        const SSA::NodeReachingDefTable& defTable = ssa_.getReachingDefsAtNode(node->get_parent());
        iter = defTable.find(varName);
        if (iter != defTable.end())
        {
            SSA::ReachingDefPtr reachingDef = defTable.find(varName)->second;
            version = reachingDef->getRenamingNumber();
        }
        else
        {
            ROSE_ASSERT(!"Cannot find the given node in the def table.");
        }
    }

    return VersionedVariable(varName, version);
}

void EventReverser::valueGraphToDot(const std::string& filename) const
{
    // Since the vetices are stored in a list, we have to give each vertex
    // a unique id here.
    int counter = 0;
    map<VGVertex, int> vertexIDs;
    VGVertexIter v, w;
    for (boost::tie(v, w) = boost::vertices(valueGraph_); v != w; ++v)
        vertexIDs[*v] = counter++;

    // Turn a std::map into a property map.
    boost::associative_property_map<map<VGVertex, int> > vertexIDMap(vertexIDs);

    ofstream ofile(filename.c_str(), std::ios::out);
    boost::write_graphviz(ofile, valueGraph_,
            boost::bind(&EventReverser::writeValueGraphNode, this, ::_1, ::_2),
            boost::bind(&EventReverser::writeValueGraphEdge, this, ::_1, ::_2),
            boost::default_writer(), vertexIDMap);
}



#if 0


struct VertexInfo
{
    VertexInfo(EventReverser::VGVertex ver = 0)
        : v(ver), dist(INT_MAX / 2), visited(false), joinNode(false)
    {
        predDist[0] = predDist[1] = INT_MAX / 2;
    }

    //	bool operator < (const VertexInfo& info) const
    //	{ return dist > info.dist; }

    EventReverser::VGVertex v;
    int dist;
    //vector<EventReverser::VGVertex> predecessors;
    vector<EventReverser::VGEdge> edges;
    bool visited;

    bool joinNode;
    int predDist[2];
};

void EventReverser::shortestPath()
{
    VGVertexIter v, w;
    vector<VertexInfo> verticesInfos(boost::num_vertices(valueGraph_));
    //priority_queue<VertexInfo> workList;
    typedef pair<int, VGVertex> DistVertexPair;
    priority_queue<DistVertexPair, vector<DistVertexPair>, greater<DistVertexPair> > workList;

    for (boost::tie(v, w) = vertices(valueGraph_); v != w; ++v)
    {
        VertexInfo info(*v);
        if (*v == root_)
            info.dist = 0;
        if (isOperatorNode(valueGraph_[*v]))
            info.joinNode = true;

        verticesInfos[info.v] = info;
    }
    workList.push(make_pair(0, root_));

    while (!workList.empty())
    {
        VGVertex r = workList.top().second;
        workList.pop();

        if (verticesInfos[r].visited)
            continue;

        VGOutEdgeIter e, f;
        for (boost::tie(e, f) = boost::out_edges(r, valueGraph_); e != f; ++e)
        {
            VGVertex v = boost::target(*e, valueGraph_);
            VertexInfo& info = verticesInfos[v];
            if (info.visited)
                continue;

            int newDist = verticesInfos[r].dist + valueGraph_[*e]->cost;
            if (!info.joinNode)
            {
                if (info.dist > newDist)
                {
                    info.dist = newDist;
                    info.edges = verticesInfos[r].edges;
                    info.edges.push_back(*e);
                    //					info.predecessors = verticesInfos[r].predecessors;
                    //					info.predecessors.push_back(r);
                }
            }
            else
            {
                ROSE_ASSERT(isOrderedEdge(valueGraph_[*e]));
                int idx = isOrderedEdge(valueGraph_[*e])->index;
                ROSE_ASSERT(idx >= 0 && idx <= 1);

                info.predDist[idx] = newDist;
                info.dist = info.predDist[0] + info.predDist[1];

                ROSE_ASSERT(!verticesInfos[r].edges.empty());

                info.edges.insert(info.edges.end(),
                        verticesInfos[r].edges.begin(), verticesInfos[r].edges.end());
                info.edges.push_back(*e);
            }

            workList.push(make_pair(info.dist, v));
        }
        verticesInfos[r].visited = true;
    }

    //	foreach (VertexInfo& info, verticesInfos)
    //	{
    //		cout << valueGraph_[info.v]->toString() << endl << info.v << ':' << info.dist << endl;
    //	}

    foreach (VGVertex v, valuesToRestore_)
        dagEdges_.insert(dagEdges_.end(),
                verticesInfos[v].edges.begin(), verticesInfos[v].edges.end());

    foreach (VGEdge e, dagEdges_)
        cout << e << endl;
}

namespace // anonymous namespace
{

// A local structure to help find all paths.
struct PathInfo
{
    set<EventReverser::VGVertex> nodes;
    set<OperatorNode*> operations;
    int cost;
    set<EventReverser::VGVertex> nodesRestored;
};

bool increaseIndex(vector<int>& indices, const vector<int>& maxIndices)
{
    int i = 0;
    while(1)
    {
        if (indices[i] == maxIndices[i])
        {
            if (i == indices.size() - 1)
                return false;
            indices[i] = 0;
            ++i;
        }
        else
        {
            ++indices[i];
            return true;
        }
    }
    return true;
}

} // end of anonymous

void EventReverser::getPath()
{
    // A map from each state variable to all its possible paths.
    map<VGVertex, vector<PathInfo> > allPaths;

    int maxCost = 0;
    foreach (VGVertex s, valuesToRestore_)
        maxCost += isValueNode(valueGraph_[s])->cost;

    foreach (VGVertex s, valuesToRestore_)
    {
        ValueNode* valNode = isValueNode(valueGraph_[s]);
        ROSE_ASSERT(valNode);

        vector<PathInfo> paths;
        PathInfo path;
        path.nodes.insert(s);
        path.cost = valNode->cost;
        path.nodesRestored.insert(s);

        paths.push_back(path);

        // Start to search all possible paths for this variable.

        for (size_t i = 0; i < paths.size(); ++i)
        {
            // Here we try to spread each node in the list.
            // Copy the list to avoid invalidation due to the push_back().
            set<VGVertex> nodes = paths[i].nodes;
            foreach (VGVertex v, nodes)
            {
                ROSE_ASSERT(isValueNode(valueGraph_[v]));

                // If the cost is 0, it is not necessary to go through this node.
                if (isValueNode(valueGraph_[v])->cost == 0)
                    continue;

                PathInfo p = paths[i];

                // Remove this node from the list, then spread through it.
                p.nodes.erase(p.nodes.find(v));
                p.nodesRestored.insert(v);

                VGOutEdgeIter e, f;
                for (boost::tie(e, f) = boost::out_edges(v, valueGraph_); e != f; ++e)
                {
                    VGVertex tar = boost::target(*e, valueGraph_);
                    if (OperatorNode* operNode = isOperatorNode(valueGraph_[tar]))
                    {
                        PathInfo newPath = p;

                        VGOutEdgeIter g, h;
                        vector<VGVertex> operands;
                        for (boost::tie(g, h) = boost::out_edges(tar, valueGraph_); g != h; ++g)
                        {
                            VGVertex operand = boost::target(*g, valueGraph_);
                            if (p.nodesRestored.count(operand) > 0)
                                goto NEXT;
                            operands.push_back(operand);
                        }
                        ROSE_ASSERT(operands.size() == 2);

                        foreach (VGVertex oper, operands)
                            newPath.nodes.insert(oper);
                        newPath.operations.insert(operNode);

                        // If the cost of this path is greater than the max cost, discard it.
                        newPath.cost = 0;
                        foreach (VGVertex n, newPath.nodes)
                            newPath.cost += isValueNode(valueGraph_[n])->cost;
                        if (newPath.cost >= maxCost)
                            continue;

                        paths.push_back(newPath);
                    }
NEXT:
                    ;
                }
            }
        }

        cout << "All paths for " << s << endl;
        foreach (const PathInfo& info, paths)
        {
            foreach (VGVertex v, info.nodes)
                cout << v << ' ';
            cout << info.cost << endl;
        }
        cout << endl << endl;


        allPaths[s].swap(paths);
    }

    // Find the optimal combination of paths of all state variables.
    vector<int> indices(valuesToRestore_.size(), 0);
    vector<int> maxIndices;
    foreach (VGVertex v, valuesToRestore_)
        maxIndices.push_back(allPaths[v].size() - 1);

    vector<int> results;
    int minCost = INT_MAX;
    do
    {
        set<VGVertex> allNodes;
        int i = 0;
        int cost = 0;
        foreach (VGVertex v, valuesToRestore_)
        {
            const set<VGVertex>& nodes = allPaths[v][indices[i++]].nodes;
            allNodes.insert(nodes.begin(), nodes.end());

            cost = 0;
            foreach (VGVertex w, allNodes)
                cost += isValueNode(valueGraph_[w])->cost;
            if (cost >= maxCost)
                goto NEXT2;
        }
        if (cost < minCost)
        {
            cout << cost << endl;
            results = indices;
            minCost = cost;
        }
NEXT2:
        ;
    } while (increaseIndex(indices, maxIndices));
}



#endif

} // End of namespace Backstroke
