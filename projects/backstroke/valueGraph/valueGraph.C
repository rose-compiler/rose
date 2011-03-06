#include "valueGraph.h"
#include <utilities/utilities.h>
#include <boost/foreach.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/johnson_all_pairs_shortest.hpp>

using namespace std;
using namespace boost;

#define foreach BOOST_FOREACH


namespace Backstroke
{

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
	root_ = addValueGraphNode(new ValueGraphNode);

	// First, add all parameters of the event to the value graph.
	SgInitializedNamePtrList& paraList = funcDef->get_declaration()->get_args();
	foreach (SgInitializedName* var, paraList)
	{
		setNewDefNode(var, nullVertex());

		// FIXME State variable may not be parameters.
		// Add the variable into wanted set.
		valuesToRestore_.push_back(nodeVertexMap_[var]);
		stateVariables_.insert(VarName(1, var));
	}

	vector<SgNode*> nodes = BackstrokeUtility::querySubTree<SgNode>(funcDef);
	foreach (SgNode* node, nodes)
	{
		cout << "Build value graph node for: " << node->class_name() << endl;
		
		// Statement case: variable declaration.
		if (SgStatement* stmt = isSgStatement(node))
		{
			if (SgVariableDeclaration* varDecl = isSgVariableDeclaration(stmt))
			{
				foreach (SgInitializedName* initName, varDecl->get_variables())
				{
					SgInitializer* initalizer = initName->get_initializer();

					// If the variable is not initialized.
					if (initalizer == NULL)
					{
						setNewDefNode(initName, nullVertex());
					}
					else if (SgAssignInitializer* assignInit = isSgAssignInitializer(initalizer))
					{
						SgExpression* operand = assignInit->get_operand();

						// Find the correct vertex which defines the variable.
						ROSE_ASSERT(nodeVertexMap_.count(operand) > 0);
						VGVertex rhsVertex = nodeVertexMap_.find(operand)->second;
						
						setNewDefNode(initName, rhsVertex);
					}
					else
					{
						cout << "The initializer: " << initalizer->class_name() << endl;
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
					cout << var.version << " " << var.isPseudoDef << endl;

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

			else if (SgValueExp* valueExp = isSgValueExp(expr))
			{
				addValueGraphNode(new ValueNode(valueExp), expr);
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
							setNewDefNode(lhs, rhsVertex);
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

						VGVertex var = addValueGraphNode(new TempVariableNode(expr->get_type()), expr);
						VGVertex lhsv = nodeVertexMap_[lhs];
						VGVertex rhsv = nodeVertexMap_[rhs];

						VGVertex op1;
						VGVertex op2;
						VGVertex op3;

						if (t == V_SgAddOp)
						{
							op1 = addValueGraphNode(new OperatorNode(V_SgAddOp));
							op2 = addValueGraphNode(new OperatorNode(V_SgSubtractOp));
							op3 = addValueGraphNode(new OperatorNode(V_SgSubtractOp));
						}
						else if (t == V_SgSubtractOp)
						{
							op1 = addValueGraphNode(new OperatorNode(V_SgSubtractOp));
							op2 = addValueGraphNode(new OperatorNode(V_SgSubtractOp));
							op3 = addValueGraphNode(new OperatorNode(V_SgAddOp));
						}

						addValueGraphEdge(var, op1);
						addValueGraphOrderedEdge(op1, lhsv, 0);
						addValueGraphOrderedEdge(op1, rhsv, 1);
						
						addValueGraphEdge(lhsv, op2);
						addValueGraphOrderedEdge(op2, var, 0);
						addValueGraphOrderedEdge(op2, rhsv, 1);
						
						addValueGraphEdge(rhsv, op3);
						addValueGraphOrderedEdge(op3, var, 0);
						addValueGraphOrderedEdge(op3, lhsv, 1);

						break;
					}

					case V_SgMultiplyOp:
					case V_SgDivideOp:
					case V_SgGreaterThanOp:
					case V_SgLessThanOp:
					{
						VGVertex var = addValueGraphNode(new TempVariableNode(expr->get_type()), expr);
						VGVertex op = addValueGraphNode(new OperatorNode(t));

						ROSE_ASSERT(nodeVertexMap_.count(lhs) > 0);
						ROSE_ASSERT(nodeVertexMap_.count(rhs) > 0);

						addValueGraphEdge(var, op);
						addValueGraphOrderedEdge(op, nodeVertexMap_[lhs], 0);
						addValueGraphOrderedEdge(op, nodeVertexMap_[rhs], 1);

						break;
					}

					case V_SgPlusAssignOp:
					{
						VGVertex var = addValueGraphNode(new VariableNode(), expr);
						VGVertex op = addValueGraphNode(new OperatorNode(t));

						VersionedVariable use = getVersionedVariable(lhs);

						ROSE_ASSERT(varVertexMap_.count(use) > 0);
						ROSE_ASSERT(nodeVertexMap_.count(rhs) > 0);

						addValueGraphEdge(var, op);
						addValueGraphOrderedEdge(op, varVertexMap_[use], 0);
						addValueGraphOrderedEdge(op, nodeVertexMap_[rhs], 1);

						// Set the def node.
						setNewDefNode(lhs, var);

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
	boost::graph_traits<ValueGraph>::edge_iterator e, f;
	for (tie(e, f) = boost::edges(valueGraph_); e != f; ++e)
		edges.push_back(*e);

	foreach (VGEdge edge, edges)
	{
		VGVertex src = source(edge, valueGraph_);
		VGVertex tar = target(edge, valueGraph_);

		// If the edge is not connected to an operator node, make a reverse copy.
		if (isOperatorNode(valueGraph_[src]) == NULL &&
				isOperatorNode(valueGraph_[tar]) == NULL)
			addValueGraphEdge(target(edge, valueGraph_), source(edge, valueGraph_));
	}

	// Add all state saving edges.
	addStateSavingEdges();
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

		addValueGraphEdge(v, varVertexMap_[varInPhi]);
	}

	return v;
}

EventReverser::VGVertex EventReverser::addValueGraphNode(ValueGraphNode* newNode, SgNode* sgNode)
{
	VGVertex v = add_vertex(valueGraph_);
	valueGraph_[v] = newNode;
	if (sgNode)
		nodeVertexMap_[sgNode] = v;
	return v;
}

EventReverser::VGEdge EventReverser::addValueGraphEdge(EventReverser::VGVertex src, EventReverser::VGVertex tar, int cost)
{
	VGEdge e = add_edge(src, tar, valueGraph_).first;
	valueGraph_[e] = new ValueGraphEdge(cost);
	return e;
}

EventReverser::VGEdge EventReverser::addValueGraphOrderedEdge(EventReverser::VGVertex src, EventReverser::VGVertex tar, int index)
{
	VGEdge e = add_edge(src, tar, valueGraph_).first;
	valueGraph_[e] = new OrderedEdge(index);
	return e;
}

void EventReverser::setNewDefNode(SgNode* defNode, VGVertex useVertex)
{
	// Get the var name and version for lhs.
	VersionedVariable var = getVersionedVariable(defNode, false);

	cout << "New var added:" << var << endl;

	//// Find the node of rhs in the VG which should exist now.
	//ROSE_ASSERT(nodeVertexMap_.count(rhs) > 0);
	//Vertex rhsVertex = nodeVertexMap_.find(rhs)->second;

	if (useVertex != nullVertex())
	{
		// Assign the node with the variable name and version.
		ValueGraphNode* node = valueGraph_[useVertex];

		//ROSE_ASSERT(node);

		// If the node contains a temporary variable, replace it with a variable node,
		// else build a new node and connect them.
		if (isTempVariableNode(node))
		{
			delete valueGraph_[useVertex];
			valueGraph_[useVertex] = new VariableNode(var);

			// Update the node to vertex table.
			nodeVertexMap_[defNode] = useVertex;
			// Update the var to vertex map.
			varVertexMap_[var] = useVertex;
		}
		else if (ValueNode* valNode = isValueNode(node))
		{
			// If the node is a value node, assign the var name to it.
			valNode->var = var;

			// Update the node to vertex table.
			nodeVertexMap_[defNode] = useVertex;
			// Update the var to vertex map.
			varVertexMap_[var] = useVertex;
		}
		else
		{
			VGVertex newVertex = addValueGraphNode(new VariableNode(var), defNode);
			addValueGraphEdge(newVertex, useVertex);

			// Update the var to vertex map.
			varVertexMap_[var] = newVertex;
		}
	}
	else
	{
		// A variable is declared but not defined.
		VGVertex newVertex = addValueGraphNode(new VariableNode(var), defNode);

		// Update the var to vertex map.
		varVertexMap_[var] = newVertex;
	}
}

void EventReverser::addStateSavingEdges()
{
	boost::graph_traits<ValueGraph>::vertex_iterator v, w;
	for (tie(v, w) = boost::vertices(valueGraph_); v != w; ++v)
	{
		ValueGraphNode* node = valueGraph_[*v];

		if (isValueNode(node))
		{
			addValueGraphEdge(*v, root_);
		}
		else if (isPhiNode(node))
		{
		}
		else if (VariableNode* varNode = isVariableNode(node))
		{
			if (availableValues_.count(*v) > 0)
				addValueGraphEdge(*v, root_, 0);
			else
				addValueGraphEdge(*v, root_, getCost(varNode->var.name.back()->get_type()));
		}
		else if (TempVariableNode* tempNode = isTempVariableNode(node))
		{
			addValueGraphEdge(*v, root_, getCost(tempNode->type));
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
// Generates all n combinations from m.
vector<vector<int> > GenerateCombination(int m, int n)
{
	ROSE_ASSERT(m >= n && m > 0 && n >= 0);
	vector<vector<int> > result;

	if (n == 0)
	{
		result.resize(1);
		return result;
	}

	if (n == 1)
	{
		for (int i = 0; i < m; ++i)
		{
			vector<int> r(1, i);
			result.push_back(r);
		}
		return result;
	}

	if (m == n)
	{
		vector<int> r;
		for (int i = 0; i < m; ++i)
			r.push_back(i);
		result.push_back(r);
		return result;
	}

	for (int i = m - 1; i >= n - 1; --i)
	{
		vector<vector<int> > r = GenerateCombination(i, n - 1);
		foreach (vector<int>& comb, r)
		{
			comb.push_back(i);
			result.push_back(comb);
		}
	}

	return result;
}

template <class Graph, class Vertex, class Map>
int SteinerTree(const ValueGraph& g, Vertex r, const set<Vertex>& terms, const Map& weightMap)
{
	const int INTMAX = INT_MAX / 3;

	// Get all-pair shortest path.
	int num = num_vertices(g);
	vector<Vertex> terminals(terms.begin(), terms.end());

	vector<vector<int> > distMatrix(num);
	foreach (vector<int>& vec, distMatrix)
		vec.resize(num);
	johnson_all_pairs_shortest_paths(g, distMatrix, weight_map(weightMap));

	foreach (vector<int>& vec, distMatrix)
	{
		foreach (int& d, vec)
		{
			if (d == INT_MAX)
				d = INTMAX;
		}
	}

	vector<Vertex> allVertices;
	typename boost::graph_traits<Graph>::vertex_iterator v, w;
	for (tie(v, w) = boost::vertices(g); v != w; ++v)
	{
		if (distMatrix[*v][r] < INTMAX)
			allVertices.push_back(*v);
	}

	cout << "allVertices:" << allVertices.size() << endl;

	map<pair<Vertex, vector<Vertex> >, int> S;
	foreach (Vertex t, terminals)
	{
		foreach (Vertex j, allVertices)
		{
			vector<Vertex> s(1, t);
			S[make_pair(j, s)] = distMatrix[t][j];
		}
	}

	for (size_t m = 2; m < terminals.size(); ++m)
	{
		vector<vector<int> > indices = GenerateCombination(terminals.size(), m);
		foreach (const vector<int>& idx, indices)
		{
			vector<Vertex> D;
			foreach (int i, idx)
				D.push_back(terminals[i]);

			foreach (Vertex v, allVertices)
			{
				S[make_pair(v, D)] = INTMAX;

				int u = INTMAX;

				// Produce all Es
				vector<vector<Vertex> > Es;
				for (size_t i = 0; i < m - 1; ++i)
				{
					vector<vector<int> > indices = GenerateCombination(m - 1, i);
					foreach (const vector<int> idx, indices)
					{
						vector<Vertex> vers(1, D[0]);
						foreach (int j, idx)
							vers.push_back(D[j + 1]);
						Es.push_back(vers);
					}
				}

				foreach (const vector<Vertex>& E, Es)
				{
					vector<Vertex> diff(D.size() - E.size());
					set_difference(D.begin(), D.end(), E.begin(), E.end(), diff.begin());
					u = min(u, S[make_pair(v, E)] + S[make_pair(v, diff)]);
				}

				foreach (Vertex w, allVertices)
				{
					S[make_pair(w, D)] = min(S[make_pair(w, D)], distMatrix[w][v] + u);
				}

			}
		}
	}

//	typedef pair<pair<Vertex, vector<Vertex> >, int> P;
//	foreach (const P& p, S)
//		cout << "* " << p.second << endl;

	int result = INTMAX;
	foreach (Vertex v, allVertices)
	{
		int u = INTMAX;

		// Produce all Es
		vector<vector<Vertex> > Es;
		for (size_t i = 0; i < terminals.size() - 1; ++i)
		{
			vector<vector<int> > indices = GenerateCombination(terminals.size() - 1, i);
			foreach (const vector<int> idx, indices)
			{
				vector<Vertex> vers(1, terminals[0]);
				foreach (int j, idx)
					vers.push_back(terminals[j + 1]);
				Es.push_back(vers);
			}
		}

		foreach (const vector<Vertex>& E, Es)
		{
			vector<Vertex> diff(terminals.size() - E.size());
			set_difference(terminals.begin(), terminals.end(), E.begin(), E.end(), diff.begin());
			u = min(u, S[make_pair(v, E)] + S[make_pair(v, diff)]);
		}

		result = min(result, distMatrix[v][r] + u);
	}

	return result;
}


void EventReverser::searchValueGraph()
{
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
			dependencesMap[*v].first = source(*(in_edges(*v, valueGraph_).first), valueGraph_);
			boost::graph_traits<ValueGraph>::out_edge_iterator e, f;
			for (tie(e, f) = out_edges(*v, valueGraph_); e != f; ++e)
				dependencesMap[*v].second.insert(target(*e, valueGraph_));
		}
	}

	for (size_t i = opNodes.size(); i >= 0 ; --i)
	//for (size_t i = 0; i <= opNodes.size(); ++i)
	{
		vector<vector<int> > combs = GenerateCombination(opNodes.size(), i);

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
				boost::graph_traits<ValueGraph>::out_edge_iterator e, f;
				for (tie(e, f) = out_edges(opNodes[k], vg); e != f; ++e)
					terminals.insert(target(*e, vg));
			}

			for (size_t j = 0; j < opNodes.size(); ++j)
			{
				if (find(comb.begin(), comb.end(), j) != comb.end())
				{
					VGEdge e = *(in_edges(opNodes[j], vg).first);
					vg[add_edge(source(e, vg), root_, vg).first] = vg[e];
				}
				clear_vertex(opNodes[j], vg);
				//remove_vertex(opNodes[j], vg);
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

			int res = SteinerTree<ValueGraph, VGVertex, associative_property_map<map<VGEdge, int> > >
				(vg, root_, terminals, associative_property_map<map<VGEdge, int> >(weightMap));

			cout << i << ' ' << comb.size() << ' ' << res << endl;
			//getchar();
		}
	}
}

#endif

}