#include "staticSingleAssignment.h"
#include "rose.h"
#include "VariableRenaming.h"
#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/operators.hpp>
#include <new>
#include "CallGraph.h"

#define foreach BOOST_FOREACH
using namespace std;
using namespace boost;

/** Return all the reaching definitions from a VariableRenaming rename table. */
set<SgNode*> renameTableToDefNodes(const VariableRenaming::NumNodeRenameEntry& entry);

/** Print a set of nodes, on one line. */
void printNodeSet(set<SgNode*> nodes);

class ComparisonTraversal : public AstSimpleProcessing
{
public:
	
	StaticSingleAssignment* ssa;
	VariableRenaming* varRenaming;
	
	virtual void visit(SgNode* node)
	{
		/** Compare reaching defs at node. */
		StaticSingleAssignment::NodeReachingDefTable newReachingDefs = ssa->getReachingDefsAtNode(node);
		VariableRenaming::NumNodeRenameTable oldReachingDefs = varRenaming->getReachingDefsAtNode(node);

		StaticSingleAssignment::ReachingDefPtr reachingDef;
		StaticSingleAssignment::VarName var;
		foreach (tie(var, reachingDef), newReachingDefs)
		{
			set<SgNode*> newReachingDefNodes = reachingDef->getActualDefinitions();
			set<SgNode*> oldReachingDefNodes = renameTableToDefNodes(oldReachingDefs[var]);

			//The set of definition nodes should be the same
			if (newReachingDefNodes != oldReachingDefNodes)
			{
				printf("ERROR: Reaching defs don't match for variable %s\n", StaticSingleAssignment::varnameToString(var).c_str());
				printf("SSA Defs:\n");
				printNodeSet(newReachingDefNodes);
				printf("\nVarRenaming Defs:\n");
				printNodeSet(oldReachingDefNodes);
				printf("\nNode is %s@%d: %s\n", node->class_name().c_str(), node->get_file_info()->get_line(),
						node->unparseToString().c_str());
				ROSE_ASSERT(false);
			}

			//If the reaching def is a phi node, it should have at least two parents
			if (reachingDef->isPhiFunction() && reachingDef->getJoinedDefs().size() < 2)
			{
				printf("Found a phi definition with less than two reaching definitions\n");
				printf("Node is %s@%d: %s\n", node->class_name().c_str(), node->get_file_info()->get_line(),
						node->unparseToString().c_str());
				printf("Variable is %s\n", StaticSingleAssignment::varnameToString(var).c_str());
				ROSE_ASSERT(false);
			}
		}

		/** Compare uses at node */
		StaticSingleAssignment::NodeReachingDefTable newUses = ssa->getUsesAtNode(node);
		VariableRenaming::NumNodeRenameTable oldUses = varRenaming->getUsesAtNode(node);

		if (newUses.size() != oldUses.size())
		{
			printf("SSA uses at node %s:%d\n", node->class_name().c_str(), node->get_file_info()->get_line());
			foreach(const StaticSingleAssignment::NodeReachingDefTable::value_type& varDefPair, newUses)
			{
				printf("%s, ", StaticSingleAssignment::varnameToString(varDefPair.first).c_str());
			}
			printf("\nVarRenaming uses at node:\n");
			foreach(const VariableRenaming::NumNodeRenameTable::value_type& varDefsPair, oldUses)
			{
				printf("%s, ", VariableRenaming::keyToString(varDefsPair.first).c_str());
			}
			printf("\n");

			printf("\nVarRenaming uses:\n");
			varRenaming->printUses(node);
			printf("\nVarRenaming defs:\n");
			varRenaming->printDefs(node);
			printf("\nSSA defs:\n");
			foreach (StaticSingleAssignment::NodeReachingDefTable::value_type x, ssa->getReachingDefsAtNode(node))
			{
				printf("%s: ", StaticSingleAssignment::varnameToString(x.first).c_str());
				printNodeSet(x.second->getActualDefinitions());
			}

			ROSE_ASSERT(false);
		}

		foreach (tie(var, reachingDef), newUses)
		{
			set<SgNode*> newUseNodes = reachingDef->getActualDefinitions();
			set<SgNode*> oldUseNodes = renameTableToDefNodes(oldUses[var]);

			if (newUseNodes != oldUseNodes)
			{
				printf("\n---------Reaching defs for use mismatch at node %s:%d for variable %s\n", node->class_name().c_str(),
						node->get_file_info()->get_line(), StaticSingleAssignment::varnameToString(var).c_str());
				printf("SSA Reaching defs for %s:\n", StaticSingleAssignment::varnameToString(var).c_str());
				printNodeSet(newUseNodes);
				printf("VariableRenaming Reaching defs for %s:\n", StaticSingleAssignment::varnameToString(var).c_str());
				printNodeSet(oldUseNodes);

				printf("\n---------SSA uses at node %s:%d \n", node->class_name().c_str(),
						node->get_file_info()->get_line());
				foreach(const StaticSingleAssignment::NodeReachingDefTable::value_type& varDefPair, newUses)
				{
					printf("%s, ", StaticSingleAssignment::varnameToString(varDefPair.first).c_str());
				}
				printf("\nVarRenaming uses:\n");
				foreach(const VariableRenaming::NumNodeRenameTable::value_type& varDefsPair, oldUses)
				{
					printf("%s, ", VariableRenaming::keyToString(varDefsPair.first).c_str());
				}
				printf("\n");

				ROSE_ASSERT(false);
			}

			//Shouldn't have use with no reaching defs
			ROSE_ASSERT(!newUseNodes.empty());
		}
	}
};

int main(int argc, char** argv)
{
	SgProject* project = frontend(argc, argv);

	if (project->get_frontendErrorCode() > 3)
	{
		//The frontend failed!
		return 1;
	}

	//Write out basic graphs
	if (SgProject::get_verbose() > 0)
	{
		generateDOT(*project);
		generateWholeGraphOfAST("wholeAST");
		
		//Call graph
		CallGraphBuilder CGBuilder(project);
		CGBuilder.buildCallGraph(ssa_private::FunctionFilter());

		// Output to a dot file
		AstDOTGeneration dotgen;
		SgFilePtrList file_list = project->get_fileList();
		std::string firstFileName = StringUtility::stripPathFromFileName(file_list[0]->getFileName());
		dotgen.writeIncidenceGraphToDOTFile(CGBuilder.getGraph(), firstFileName + "_callGraph.dot");
	}

	//Run the variable renaming on the project
	VariableRenaming varRenaming(project);
	varRenaming.run();

	//Write out CFG graphs
	if (SgProject::get_verbose() > 0)
	{
		varRenaming.toFilteredDOT("varRenaming_filtered.dot");
		varRenaming.toDOT("varRenaming_uniltered.dot");
	}

	if (SgProject::get_verbose() > 0)
	{
		printf("\n\n ***** VariableRenaming Complete ***** \n\n");
	}

	//Run the SSA analysis intraprocedurally
	StaticSingleAssignment ssa(project);
	ssa.run(false);

	if (SgProject::get_verbose() > 0)
	{
		ssa.toFilteredDOT("filteredCFG.dot");
		ssa.toDOT("UNfiltered.dot");
	}

	//Compare original defs and uses
	ComparisonTraversal t;
	t.varRenaming = &varRenaming;
	t.ssa = &ssa;
	t.traverse(project, preorder);

	//Also test the interprocedural analysis
	StaticSingleAssignment ssaInterprocedural(project);
	ssaInterprocedural.run(true);

	if (SgProject::get_verbose() > 0)
	{
		ssaInterprocedural.toFilteredDOT("interprocedural.dot");
	}

	return 0;
}

set<SgNode*> renameTableToDefNodes(const VariableRenaming::NumNodeRenameEntry& entry)
{
	set<SgNode*> reachingDefs;
	foreach(const VariableRenaming::NumNodeRenameEntry::value_type& numNodePair, entry)
	{
		reachingDefs.insert(numNodePair.second);
	}
	return reachingDefs;
}

void printNodeSet(set<SgNode*> nodes)
{
	foreach(SgNode* node, nodes)
	{
		printf("%s@%d, ", node->class_name().c_str(), node->get_file_info()->get_line());
	}
	printf("\n");
}