#include "valueGraph.h"
#include <utilities/utilities.h>
#include <boost/foreach.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/timer.hpp>
#include "steinerTree.h"

namespace Backstroke
{

using namespace std;
using namespace boost;

#define foreach BOOST_FOREACH


void EventReverser::buildValueGraph(SgFunctionDefinition* funcDef)
{
	// First, build a SSA CFG for the given function definition.
	//CFG cfg(funcDef);

	// Then build a CDG to find all control dependence.
	//CDG cdg(cfg);

	// Build the SSA form of the given function.
	//SSA ssa(SageInterface::getProject());
	//ssa_.run();
	//SSA::NodeReachingDefTable defTable = getUsesAtNode(funcDef);

	// Start to build the value graph.
	//map<ValueGraphNode*, SgNode*> nodeMap;
	
	//map<SgNode*, Vertex> nodeVertexMap_;
	//map<VersionedVariable, Vertex> varVertexMap_;


	// Build a vertex which is the start point of the search.
	//root_ = addValueGraphNode(new ValueGraphNode);
	root_ = addValueGraphNode(new ValueGraphNode);

	// First, add all parameters of the event to the value graph.
	SgInitializedNamePtrList& paraList = funcDef->get_declaration()->get_args();
	foreach (SgInitializedName* var, paraList)
	{
		creatValueNode(var);

		// FIXME State variable may not be parameters.
		// Add the variable into wanted set.
		valuesToRestore_.push_back(nodeVertexMap_[var]);
		stateVariables_.insert(VarName(1, var));
	}

	vector<SgNode*> nodes = BackstrokeUtility::querySubTree<SgNode>(funcDef);
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

					if (initalizer == NULL)
					{
						creatValueNode(initName);
					}
					else if (SgAssignInitializer* assignInit = isSgAssignInitializer(initalizer))
					{
						SgExpression* operand = assignInit->get_operand();

						ROSE_ASSERT(nodeVertexMap_.count(operand) > 0);
						VGVertex rhsVertex = nodeVertexMap_[operand];
						
						addVariable(rhsVertex, initName);
					}
					else
					{
						cout << initalizer->class_name() << endl;
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

				cout << "Variable node:" << var << endl;

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
				//varVertexMap_[var] = add_vertexvalueGraph_;
				//valueGraph_[newNode] = new ValueGraphNode(varName, defTable[varName]->getRenamingNumber());
				//valueGraph_[newNode] = newNode;
				//varVertexMap_[var] = v;
			}

			// Value expression.
			else if (SgValueExp* valueExp = isSgValueExp(expr))
			{
				creatValueNode(valueExp);
				//addValueGraphNode(new ValueNode(valueExp), expr);
			}

			// Cast expression.
			else if (SgCastExp* castExp = isSgCastExp(expr))
			{
				ROSE_ASSERT(nodeVertexMap_.count(castExp->get_operand()));
				nodeVertexMap_[castExp] = nodeVertexMap_[castExp->get_operand()];
			}

			else if (SgBinaryOp* binOp = isSgBinaryOp(expr))
			{
				SgExpression* lhs = binOp->get_lhs_operand();
				SgExpression* rhs = binOp->get_rhs_operand();

//				ValueGraphNode* newNode = NULL;

				//OperaterType opType;

//				map<VariantT, OperaterType> nodeOpTypeTable;
//				nodeOpTypeTable[V_SgAddOp] = otAdd;
//				nodeOpTypeTable[V_SgAddOp] = otAdd;
//				nodeOpTypeTable[V_SgAddOp] = otAdd;

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
							addVariable(rhsVertex, lhs);
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

						VGVertex result = creatValueNode(expr);
						VGVertex lhsv = nodeVertexMap_[lhs];
						VGVertex rhsv = nodeVertexMap_[rhs];

						VariantT t2 = (t == V_SgAddOp) ? V_SgSubtractOp : V_SgAddOp;
						creatOperatorNode(t, result, lhsv, rhsv);
						creatOperatorNode(t2, lhsv, result, rhsv);
						creatOperatorNode(V_SgSubtractOp, rhsv, result, lhsv);

						break;
					}

					case V_SgMultiplyOp:
					case V_SgDivideOp:
					case V_SgGreaterThanOp:
					case V_SgLessThanOp:
					{
						ROSE_ASSERT(nodeVertexMap_.count(lhs) > 0);
						ROSE_ASSERT(nodeVertexMap_.count(rhs) > 0);

						creatOperatorNode(t, creatValueNode(expr),
								nodeVertexMap_[lhs], nodeVertexMap_[rhs]);

						break;
					}

					case V_SgPlusAssignOp:
					{
						VersionedVariable use = getVersionedVariable(lhs);

						ROSE_ASSERT(varVertexMap_.count(use) > 0);
						ROSE_ASSERT(nodeVertexMap_.count(rhs) > 0);
						
						creatOperatorNode(t, creatValueNode(expr),
								varVertexMap_[use], nodeVertexMap_[rhs]);
						break;
					}


					default:
						break;
				}
			}
		}
	}

	// At the end of the event, find the versions of all variables, and determine which variables
	// are avaiable during the search of VG.
	typedef SSA::NodeReachingDefTable::value_type VarNameDefPair;
	foreach (const VarNameDefPair& nameDef, ssa_.getLastVersions(funcDef->get_declaration()))
	{
		VarName name = nameDef.first;
		
		if (isStateVariable(name))
		{
			VersionedVariable var(name, nameDef.second->getRenamingNumber());

			ROSE_ASSERT(varVertexMap_.count(var));
			availableValues_.insert(varVertexMap_[var]);
		}
	}

	// Add a reverse edge for every non-ordered edge.
	vector<VGEdge> edges;
	VGEdgeIter e, f;
	for (tie(e, f) = boost::edges(valueGraph_); e != f; ++e)
		edges.push_back(*e);

	foreach (VGEdge edge, edges)
	{
		VGVertex src = source(edge, valueGraph_);
		VGVertex tar = target(edge, valueGraph_);

		// If the edge is not connected to an operator node, make a reverse copy.
		if (isOperatorNode(valueGraph_[src]) == NULL &&
				isOperatorNode(valueGraph_[tar]) == NULL)
			addValueGraphEdge(src, tar, 0);
	}

	// Add all state saving edges.
	addStateSavingEdges();

	cout << "Number of nodes: " << num_vertices(valueGraph_) << endl;
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

	VGVertex v = add_vertex(valueGraph_);
	valueGraph_[v] = newNode;
//	if (sgNode)
//		nodeVertexMap_[sgNode] = v;
	return v;
}

EventReverser::VGEdge EventReverser::addValueGraphEdge(
		EventReverser::VGVertex src, EventReverser::VGVertex tar, int cost)
{
	//VGEdge e = add_edge(src, tar, valueGraph_).first;
	VGEdge e = add_edge(tar, src, valueGraph_).first;
	valueGraph_[e] = new ValueGraphEdge(cost);
	return e;
}

EventReverser::VGEdge EventReverser::addValueGraphOrderedEdge(
		EventReverser::VGVertex src, EventReverser::VGVertex tar, int index)
{
	//VGEdge e = add_edge(src, tar, valueGraph_).first;
	VGEdge e = add_edge(tar, src, valueGraph_).first;
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

EventReverser::VGVertex EventReverser::creatValueNode(SgNode* node)
{
	VGVertex newVertex; // = add_vertex(valueGraph_);

	if (SgValueExp* valExp = isSgValueExp(node))
	{
		newVertex = addValueGraphNode(new ValueNode(valExp));
	}
	else if (isSgVarRefExp(node) == NULL && isSgInitializedName(node) == NULL)
	{
		newVertex = addValueGraphNode(new ValueNode);
	}
	else
	{
		// Get the var name and version for lhs.
		VersionedVariable var = getVersionedVariable(node, false);

		ValueNode* valNode = new ValueNode;
		valNode->vars.push_back(var);
		newVertex = addValueGraphNode(valNode);

		// Update tables.
		varVertexMap_[var] = newVertex;
	}

	nodeVertexMap_[node] = newVertex;

	return newVertex;
}

void EventReverser::addVariable(EventReverser::VGVertex v, SgNode* node)
{
	ValueNode* valNode = isValueNode(valueGraph_[v]);
	ROSE_ASSERT(valNode);
	
	VersionedVariable var = getVersionedVariable(node, false);
	valNode->vars.push_back(var);

	varVertexMap_[var] = nodeVertexMap_[node] = v;
}

EventReverser::VGVertex EventReverser::creatOperatorNode(
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
	for (tie(v, w) = boost::vertices(valueGraph_); v != w; ++v)
	{
		ValueGraphNode* node = valueGraph_[*v];

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
                    cost = INT_MAX;
                    foreach (const VersionedVariable& var, valNode->vars)
                        cost = min(cost, getCost(var.name.back()->get_type()));
                }
            }
			addValueGraphEdge(*v, root_, cost);
		}
	}
}

int EventReverser::getCost(SgType* t)
{
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
	ofstream ofile(filename.c_str(), std::ios::out);
	write_graphviz(ofile, valueGraph_,
		bind(&EventReverser::writeValueGraphNode, this, ::_1, ::_2),
		bind(&EventReverser::writeValueGraphEdge, this, ::_1, ::_2));
}



#if 1

void EventReverser::searchValueGraph()
{
	timer t;

	set<VGVertex> terminals;
	foreach (VGVertex v, valuesToRestore_)
		terminals.insert(v);
	SteinerTreeBuilder builder;
	builder.buildSteinerTree(valueGraph_, root_, terminals);


#if 0
	
	// First, collect all operator nodes.
	vector<VGVertex> opNodes;
	map<VGVertex, pair<VGVertex, set<VGVertex> > > dependencesMap;
	boost::graph_traits<ValueGraph>::vertex_iterator v, w;
	for (tie(v, w) = boost::vertices(valueGraph_); v != w; ++v)
	{
		if (isOperatorNode(valueGraph_[*v]))
		{
			opNodes.push_back(*v);

			// Find the dependence.
			dependencesMap[*v].first = target(*(out_edges(*v, valueGraph_).first), valueGraph_);
			boost::graph_traits<ValueGraph>::in_edge_iterator e, f;
			for (tie(e, f) = in_edges(*v, valueGraph_); e != f; ++e)
				dependencesMap[*v].second.insert(source(*e, valueGraph_));
		}
	}

	for (int i = opNodes.size(); i >= 0 ; --i)
	//for (size_t i = 0; i <= opNodes.size(); ++i)
	{
		vector<vector<int> > combs = generateCombination(opNodes.size(), i);

		// for each combination
		foreach (vector<int>& comb, combs)
		{
			// First, check if this combination is valid (no cross dependence).
			map<VGVertex, set<VGVertex> > dependences;
			bool flag = false;
			foreach (int j, comb)
			{
				pair<VGVertex, set<VGVertex> >& vp = dependencesMap[opNodes[j]];

				// A value can only be restored by one operator.
				if (dependences.count(vp.first) > 0)
				{
					flag = true;
					break;
				}
				
				//cout << "::" << vp.first << ':';
				foreach (VGVertex v, vp.second)
				{
					//cout << v << ' ';
					if (dependences[v].count(vp.first) > 0)
					{
						flag = true;
						//cout << "\n\n\n";
						goto NEXT;
					}
				}
				//cout << endl;
				dependences.insert(vp);
			}
NEXT:
			if (flag)
				continue;

			ValueGraph vg = valueGraph_;

			set<VGVertex> terminals;
			foreach (VGVertex v, valuesToRestore_)
				terminals.insert(v);
			foreach (int k, comb)
			{
				boost::graph_traits<ValueGraph>::in_edge_iterator e, f;
				for (tie(e, f) = in_edges(opNodes[k], vg); e != f; ++e)
					terminals.insert(source(*e, vg));
			}

			for (size_t j = 0; j < opNodes.size(); ++j)
			{
				if (find(comb.begin(), comb.end(), j) != comb.end())
				{
					VGEdge e = *(out_edges(opNodes[j], vg).first);
					vg[add_edge(root_, target(e, vg), vg).first] = vg[e];
				}
				clear_vertex(opNodes[j], vg);
			}

#if 0
			boost::graph_traits<ValueGraph>::vertex_iterator v, w;
			for (tie(v, w) = boost::vertices(vg); v != w;)
			{
				if (isOperatorNode(vg[*v]))
				{
					clear_vertex(*v, vg);
					remove_vertex(*v, vg);
					tie(v, w) = boost::vertices(vg);
				}
				else
					++v;
			}
#endif

#if 0
			cout << "Draw graph ..." << endl;
			ofstream ofile("VG.dot", std::ios::out);
			write_graphviz(ofile, vg,
				bind(&EventReverser::writeValueGraphNode, this, ::_1, ::_2),
				bind(&EventReverser::writeValueGraphEdge, this, ::_1, ::_2));
#endif

			// Start the search.
			map<VGEdge, int> weightMap;
			vector<VGEdge> edges;
			boost::graph_traits<ValueGraph>::edge_iterator e, f;
			for (tie(e, f) = boost::edges(vg); e != f; ++e)
				weightMap[*e] = vg[*e]->cost;

#if 1
//			int res = SteinerTree<ValueGraph, VGVertex, associative_property_map<map<VGEdge, int> > >
//				(vg, root_, terminals, associative_property_map<map<VGEdge, int> >(weightMap));
			SteinerTreeBuilder builder;
			builder.buildSteinerTree(valueGraph_, root_, terminals);

			//cout << i << ' ' << comb.size() << ' ' << dist << endl;
#else
			typedef SteinerTreeFinder<ValueGraph, associative_property_map<map<VGEdge, int> > > STFinder;
			STFinder finder(vg, root_, terminals, (int)(log((float)terminals.size()) / log(2.f) + 0.5f), weightMap);
			STFinder::SteinerTreeType steinerTree;
			int dist;
			tie(steinerTree, dist) = finder.GetSteinerTree();
			cout << i << ' ' << comb.size() << ' ' << dist << endl;
#endif
			//getchar();
		}
	}

#endif

	cout << "Time used: " << t.elapsed() << endl;
}

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
	vector<VertexInfo> verticesInfos(num_vertices(valueGraph_));
	//priority_queue<VertexInfo> workList;
	typedef pair<int, VGVertex> DistVertexPair;
	priority_queue<DistVertexPair, vector<DistVertexPair>, greater<DistVertexPair> > workList;

	for (tie(v, w) = vertices(valueGraph_); v != w; ++v)
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
		for (tie(e, f) = out_edges(r, valueGraph_); e != f; ++e)
		{
			VGVertex v = target(*e, valueGraph_);
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
		dagEdges_.insert(dagEdges_.end(), verticesInfos[v].edges.begin(), verticesInfos[v].edges.end());

	foreach (VGEdge e, dagEdges_)
		cout << e << endl;
}


#endif

} // End of namespace Backstroke
