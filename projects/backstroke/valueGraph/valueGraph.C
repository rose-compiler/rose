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
	SSA ssa(SageInterface::getProject());
	ssa.run();
	//SSA::NodeReachingDefTable defTable = getUsesAtNode(funcDef);

	// Start to build the value graph.
	//map<ValueGraphNode*, SgNode*> nodeMap;
	map<SgNode*, Vertex> nodeVertexMap;
	map<VariableWithVersion, Vertex> varVertexMap;

	vector<SgNode*> nodes = BackstrokeUtility::querySubTree<SgNode>(funcDef);
	foreach (SgNode* node, nodes)
	{
		// Statement case: variable declaration.
		if (SgStatement* stmt = isSgStatement(node))
		{
			if (SgVariableDeclaration* varDecl = isSgVariableDeclaration(stmt))
			{
				foreach (SgInitializedName* initName, varDecl->get_variables())
				{
					SgInitializer* initalizer = initName->get_initializer();

					if (SgAssignInitializer* assignInit = isSgAssignInitializer(initalizer))
					{
						SgExpression* operand = assignInit->get_operand();

						ROSE_ASSERT(nodeVertexMap.count(operand) > 0);

						SSA::VarName varName = SSA::getVarName(initName);

						SSA::NodeReachingDefTable defTable = ssa.getReachingDefsAtNode(initName);
						ROSE_ASSERT(defTable.count(varName) > 0);

						int version = defTable[varName]->getRenamingNumber();

						Vertex v = nodeVertexMap[operand];
						VariableWithVersion varWithVersion(varName, version);
						varVertexMap[varWithVersion] = v;

						// Assign the node with the variable name and version.
						ValueGraphNode* node = (*this)[v];
						node->setVariable(varName, version);
					}
					else
					{
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
				VarName varName = SSA::getVarName(expr);

				SSA::NodeReachingDefTable defTable = ssa.getUsesAtNode(expr);
				ROSE_ASSERT(defTable.count(varName) > 0);

				if (defTable[varName]->isPhiFunction())
				{
					int version = defTable[varName]->getRenamingNumber();
					VariableWithVersion varWithVersion(varName, version);

					if (varVertexMap.find(varWithVersion) != varVertexMap.end())
					{
						// Add the phi node.
						ValueGraphNode* newNode = new PhiNode();
						newNode->setVariable(varName, version);
						
						Vertex v = add_vertex(*this);
						(*this)[v] = newNode;

						// Add the var -> vertex to varVertexMap
						varVertexMap[varWithVersion] = v;
					}

					ROSE_ASSERT(varVertexMap.find(varWithVersion) != varVertexMap.end());

					// Add the node -> vertex to nodeVertexMap
					nodeVertexMap[expr] = varVertexMap.find(varWithVersion)->second;
				}

				//VariableWithVersion var(varName, defTable[varName]->getRenamingNumber());
				//varVertexMap[var] = add_vertex(*this);
				//(*this)[newNode] = new ValueGraphNode(varName, defTable[varName]->getRenamingNumber());
				//(*this)[newNode] = newNode;
				//varVertexMap[var] = v;
			}

			else if (SgValueExp* valueExp = isSgValueExp(expr))
			{
				ValueGraphNode* newNode = new ValueNode(valueExp);
				Vertex v = add_vertex(*this);
				(*this)[v] = newNode;

				nodeVertexMap[expr] = v;
			}

			else if (SgBinaryOp* binOp = isSgBinaryOp(expr))
			{
				SgExpression* lhs = binOp->get_lhs_operand();
				SgExpression* rhs = binOp->get_rhs_operand();

				ValueGraphNode* newNode = NULL;

				OperaterType opType;

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
							ROSE_ASSERT(nodeVertexMap.count(rhs) > 0);

							SSA::VarName varName = SSA::getVarName(lhs);
							//nodeMap[rhs].varName = varName;

							SSA::NodeReachingDefTable defTable = ssa.getReachingDefsAtNode(lhs);
							ROSE_ASSERT(defTable.count(varName) > 0);

							int version = defTable[varName]->getRenamingNumber();

							Vertex v = nodeVertexMap[rhs];
							VariableWithVersion varWithVersion(varName, version);
							varVertexMap[varWithVersion] = v;
							
							// Assign the node with the variable name and version.
							ValueGraphNode* node = (*this)[v];
							node->setVariable(varName, version);
						}
						else
						{
							ROSE_ASSERT(!"Only variable can be assigned now.");
						}

						break;
					}

					case V_SgAddOp:
					{
						opType = otAdd;
						
						ValueGraphNode* newNode = new OperaterNode(opType);
						Vertex v = add_vertex(*this);
						(*this)[v] = newNode;

						nodeVertexMap[expr] = v;

						ROSE_ASSERT(nodeVertexMap.count(lhs) > 0);
						ROSE_ASSERT(nodeVertexMap.count(rhs) > 0);

						Edge e1 = add_edge(v, nodeVertexMap[lhs], *this).first;
						Edge e2 = add_edge(v, nodeVertexMap[rhs], *this).first;

						(*this)[e1] = new ValueGraphEdge;
						(*this)[e2] = new ValueGraphEdge;

						break;
					}

					case V_SgPlusAssignOp:
					{
						newNode = new OperaterNode(otAdd);
						break;
					}

					default:
						break;
				}
			}
		}
	}
}

void ValueGraph::toDot(const std::string& filename) const
{
	ofstream ofile(filename.c_str(), std::ios::out);
	write_graphviz(ofile, *this,
		bind(&ValueGraph::writeGraphNode, this, ::_1, ::_2));
		//bind(&ValueGraph::writeGraphEdge, this, ::_1, ::_2));
}

}