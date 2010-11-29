#include "staticSingleAssignment.h"
#include "rose.h"
#include "VariableRenaming.h"
#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/operators.hpp>

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

		if (newReachingDefs.size() != oldReachingDefs.size())
		{
			printf("SSA Reaching defs at node %s:%d\n", node->class_name().c_str(), node->get_file_info()->get_line());
			foreach(const StaticSingleAssignment::NodeReachingDefTable::value_type& varDefPair, newReachingDefs)
			{
				printf("%s, ", StaticSingleAssignment::varnameToString(varDefPair.first).c_str());
			}
			printf("\nVarRenaming reaching defs at node:\n");
			foreach(const VariableRenaming::NumNodeRenameTable::value_type& varDefsPair, oldReachingDefs)
			{
				printf("%s, ", VariableRenaming::keyToString(varDefsPair.first).c_str());
			}
			printf("\n");

			ROSE_ASSERT(false);
		}

		StaticSingleAssignment::ReachingDefPtr reachingDef;
		StaticSingleAssignment::VarName var;
		foreach (tie(var, reachingDef), newReachingDefs)
		{
			set<SgNode*> newReachingDefNodes = reachingDef->getActualDefinitions();
			set<SgNode*> oldReachingDefNodes = renameTableToDefNodes(oldReachingDefs[var]);

			ROSE_ASSERT(newReachingDefNodes == oldReachingDefNodes);
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

				printf("SSA Reaching defs for %s:\n", StaticSingleAssignment::varnameToString(var).c_str());
				printNodeSet(newUseNodes);
				printf("VariableRenaming Reaching defs for %s:\n", StaticSingleAssignment::varnameToString(var).c_str());
				printNodeSet(oldUseNodes);

				ROSE_ASSERT(false);
			}
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

	//Run the variable renaming on the project
	VariableRenaming varRenaming(project);
	varRenaming.run();

	if (SgProject::get_verbose() > 0)
	{
		printf("\n\n ***** VariableRenaming Complete ***** \n\n");
	}

	if (SgProject::get_verbose() > 0)
	{
		generateDOT(*project);
		varRenaming.toFilteredDOT("filteredCFG.dot");
		varRenaming.toDOT("UNfiltered.dot");
	}

	//Run the SSA analysis
	StaticSingleAssignment ssa(project);
	ssa.run();

	//Compare original defs and uses
	ComparisonTraversal t;
	t.varRenaming = &varRenaming;
	t.ssa = &ssa;
	t.traverse(project, preorder);
	
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