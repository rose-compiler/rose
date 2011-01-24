#include "valueGraph.h"
#include <utilities/utilities.h>
#include <boost/foreach.hpp>
#include <boost/graph/graphviz.hpp>

using namespace std;
using namespace boost;

#define foreach BOOST_FOREACH


namespace Backstroke
{

void ValueGraph::build(SgFunctionDefinition* funcDef)
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
	//map<VariableWithVersion, Vertex> varVertexMap_;

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
						Vertex rhsVertex = nodeVertexMap_.find(operand)->second;
						
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
			// For a variable reference, if its def is a phi node, we build this phi node here.
			if (BackstrokeUtility::isVariableReference(expr))
			{
				// Get the var name and version for lhs.
				// We don't know if this var is a use or def now.
				VariableWithVersion var = getVariableWithVersion(expr);

				if (var.isPseudoDef)
				{
					// If there is no node of var in the VG.
					if (varVertexMap_.find(var) == varVertexMap_.end())
					{
						// Add the phi node.
						Vertex v = addVertex(new PhiNode(var));
						varVertexMap_[var] = v;

						// Connect the pseudo def to real defs.
						foreach (const PhiNodeDependence& def, var.phiVersions)
						{
							VariableWithVersion varInPhi(var.name, def.version);
							ROSE_ASSERT(varVertexMap_.find(varInPhi) != varVertexMap_.end());
							
							Edge e = add_edge(v, varVertexMap_[varInPhi], *this).first;
							(*this)[e] = new ValueGraphEdge;
						}
					}

					ROSE_ASSERT(varVertexMap_.find(var) != varVertexMap_.end());

					// Add the node -> vertex to nodeVertexMap_
					nodeVertexMap_[expr] = varVertexMap_.find(var)->second;
				}
				else
				{
					cout << var.version << var.isPseudoDef << endl;
					// If the reaching def of this variable is not a phi node, its node should already be added
					// into the value graph. Find it and update the node->vertex table.
					ROSE_ASSERT(varVertexMap_.count(var) > 0);
					nodeVertexMap_[expr] = varVertexMap_[var];
				}

				//VariableWithVersion var(varName, defTable[varName]->getRenamingNumber());
				//varVertexMap_[var] = add_vertex(*this);
				//(*this)[newNode] = new ValueGraphNode(varName, defTable[varName]->getRenamingNumber());
				//(*this)[newNode] = newNode;
				//varVertexMap_[var] = v;
			}

			else if (SgValueExp* valueExp = isSgValueExp(expr))
			{
				addVertex(new ValueNode(valueExp), expr);
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

				switch (node->variantT())
				{
					// For assign op, we assign the node which is assigned to with a variable name.
					case V_SgAssignOp:
					{
						if (BackstrokeUtility::isVariableReference(lhs))
						{
							ROSE_ASSERT(nodeVertexMap_.count(rhs) > 0);
							Vertex rhsVertex = nodeVertexMap_.find(rhs)->second;
							setNewDefNode(lhs, rhsVertex);
						}
						else
						{
							ROSE_ASSERT(!"Only variable can be assigned now.");
						}

						break;
					}

					case V_SgAddOp:
					{
						Vertex v = addVertex(new OperaterNode(OperaterNode::otAdd), expr);

						ROSE_ASSERT(nodeVertexMap_.count(lhs) > 0);
						ROSE_ASSERT(nodeVertexMap_.count(rhs) > 0);

						Edge e1 = add_edge(v, nodeVertexMap_[lhs], *this).first;
						Edge e2 = add_edge(v, nodeVertexMap_[rhs], *this).first;

						(*this)[e1] = new ValueGraphEdge;
						(*this)[e2] = new ValueGraphEdge;

						break;
					}

					case V_SgPlusAssignOp:
					{
						Vertex v = addVertex(new OperaterNode(OperaterNode::otAdd), expr);

						VariableWithVersion use = getVariableWithVersion(lhs);

						ROSE_ASSERT(varVertexMap_.count(use) > 0);
						ROSE_ASSERT(nodeVertexMap_.count(rhs) > 0);

						Edge e1 = add_edge(v, varVertexMap_[use], *this).first;
						Edge e2 = add_edge(v, nodeVertexMap_[rhs], *this).first;

						(*this)[e1] = new ValueGraphEdge;
						(*this)[e2] = new ValueGraphEdge;

						// Set the def node.
						setNewDefNode(lhs, v);

						break;
					}

					default:
						break;
				}
			}
		}
	}
}

void ValueGraph::setNewDefNode(SgNode* defNode, Vertex useVertex)
{
	// Get the var name and version for lhs.
	VariableWithVersion var = getVariableWithVersion(defNode, false);

	//// Find the node of rhs in the VG which should exist now.
	//ROSE_ASSERT(nodeVertexMap_.count(rhs) > 0);
	//Vertex rhsVertex = nodeVertexMap_.find(rhs)->second;

	if (useVertex != nullVertex())
	{
		// Assign the node with the variable name and version.
		ValueGraphNode* node = (*this)[useVertex];

		// If the node contains a non-temporary variable, assign it with the
		// variable name and version, else build a new node and connect them.
		if (node->isTemp)
		{
			node->setVariable(var);

			// Update tables.
			nodeVertexMap_[defNode] = useVertex;
			varVertexMap_[var] = useVertex;
		}
		else
		{
			Vertex newVertex = addVertex(new ValueGraphNode(var), defNode);

			// Update the var to vertex map.
			varVertexMap_[var] = newVertex;

			Edge e = add_edge(newVertex, useVertex, *this).first;
			(*this)[e] = new ValueGraphEdge;
		}
	}
	else
	{
		// A variable is declared but not defined.
		Vertex newVertex = addVertex(new ValueGraphNode(var), defNode);

		// Update the var to vertex map.
		varVertexMap_[var] = newVertex;
	}
}

VariableWithVersion ValueGraph::getVariableWithVersion(SgNode* node, bool isUse) const
{
	VarName varName = SSA::getVarName(node);
	int version;

	if (isUse)
	{
		const SSA::NodeReachingDefTable& defTable = ssa_.getUsesAtNode(node);
		ROSE_ASSERT(defTable.count(varName) > 0);
		SSA::ReachingDefPtr reachingDef = defTable.find(varName)->second;
		version = reachingDef->getRenamingNumber();

		// If its reaching def is a phi function, it's a pseudo def.
		if (reachingDef->isPhiFunction())
		{
			VariableWithVersion var(varName, version, true);
			foreach (SSA::ReachingDefPtr def, reachingDef->getJoinedDefs())
			{
				var.phiVersions.push_back(PhiNodeDependence(def->getRenamingNumber()));
			}
			return var;
		}
	}
	else
	{
		const SSA::NodeReachingDefTable& defTable = ssa_.getReachingDefsAtNode(node->get_parent());
		ROSE_ASSERT(defTable.count(varName) > 0);
		SSA::ReachingDefPtr reachingDef = defTable.find(varName)->second;
		version = reachingDef->getRenamingNumber();
	}
	
	return VariableWithVersion(varName, version);
}

void ValueGraph::toDot(const std::string& filename) const
{
	ofstream ofile(filename.c_str(), std::ios::out);
	write_graphviz(ofile, *this,
		bind(&ValueGraph::writeGraphNode, this, ::_1, ::_2));
		//bind(&ValueGraph::writeGraphEdge, this, ::_1, ::_2));
}

}