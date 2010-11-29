#include "staticSingleAssignment.h"
#include "rose.h"
#include "VariableRenaming.h"
#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>

#define foreach BOOST_FOREACH
using namespace std;
using namespace boost;

class ComparisonTraversal : public AstSimpleProcessing
{
public:
	
	StaticSingleAssignment* ssa;
	VariableRenaming* varRenaming;
	
	virtual void visit(SgNode* node)
	{
		if (ssa->getOriginalDefsAtNode(node) != varRenaming->getOriginalDefsAtNode(node))
		{
			printf("Found difference between original defs at node %s, line %d: %s\n", node->class_name().c_str(),
					node->get_file_info()->get_line(), node->unparseToString().c_str());
			
			printf("VariableRenaming Original Defs:");
			StaticSingleAssignment::printRenameTable(varRenaming->getOriginalDefsAtNode(node));
			printf("SSA Original Defs:");
			StaticSingleAssignment::printRenameTable(ssa->getOriginalDefsAtNode(node));
			printf("VariableRenaming Expanded Defs:");
			StaticSingleAssignment::printRenameTable(varRenaming->getExpandedDefsAtNode(node));
			printf("SSA Expanded Defs:");
			StaticSingleAssignment::printRenameTable(ssa->getExpandedDefsAtNode(node));
			ROSE_ASSERT(false);
		}

		StaticSingleAssignment::NodeReachingDefTable newReachingDefs = ssa->ssa_getReachingDefsAtNode(node);
		StaticSingleAssignment::NumNodeRenameTable oldReachingDefs = ssa->getReachingDefsAtNode(node);

		if (newReachingDefs.size() != oldReachingDefs.size())
		{
			printf("SSA Reaching defs at node %s:%d\n", node->class_name().c_str(), node->get_file_info()->get_line());
			foreach(const StaticSingleAssignment::NodeReachingDefTable::value_type& varDefPair, newReachingDefs)
			{
				printf("%s, ", StaticSingleAssignment::varnameToString(varDefPair.first).c_str());
			}
			printf("\nVarRenaming reaching defs at node:\n");
			foreach(const StaticSingleAssignment::NumNodeRenameTable::value_type& varDefsPair, oldReachingDefs)
			{
				printf("%s, ", StaticSingleAssignment::varnameToString(varDefsPair.first).c_str());
			}
			printf("\n");

			ROSE_ASSERT(false);
		}


		StaticSingleAssignment::ReachingDefPtr reachingDef;
		StaticSingleAssignment::VarName var;
		foreach (tie(var, reachingDef), newReachingDefs)
		{
			set<SgNode*> newReachingDefNodes = reachingDef->getActualDefinitions();

			set<SgNode*> oldReachingDefNodes;
			foreach(StaticSingleAssignment::NumNodeRenameEntry::value_type& numNodePair, oldReachingDefs[var])
			{
				oldReachingDefNodes.insert(numNodePair.second);
			}

			ROSE_ASSERT(newReachingDefNodes == oldReachingDefNodes);
		}

		ROSE_ASSERT(varRenaming->getExpandedDefsAtNode(node) == ssa->getExpandedDefsAtNode(node));
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

	//Compare the sizes of the use tables
	pair<SgNode*, StaticSingleAssignment::TableEntry> entry;
	if (ssa.getUseTable().size() != varRenaming.getUseTable().size())
	{
		foreach (entry, ssa.getUseTable())
		{
			if (varRenaming.getUseTable().count(entry.first) == 0 && entry.second.size() > 0)
			{
				printf("Node %s, %d: %s --- in SSA but not varRenaming use Table\n", entry.first->class_name().c_str(),
						entry.first->get_file_info()->get_line(), entry.first->unparseToString().c_str());
				ssa.printUses(entry.first);
				ROSE_ASSERT(false);
			}
		}
		
		foreach (entry, varRenaming.getUseTable())
		{
			if (ssa.getUseTable().count(entry.first) == 0 && entry.second.size() > 0)
			{
				printf("Node %s, %d: %s --- in varRenaming but not SSA use Table\n", entry.first->class_name().c_str(),
						entry.first->get_file_info()->get_line(), entry.first->unparseToString().c_str());
				varRenaming.printUses(entry.first);
				ROSE_ASSERT(false);
			}
		}
	}

	foreach(entry, ssa.getUseTable())
	{
		SgNode* node = entry.first;
		const StaticSingleAssignment::TableEntry& usedVarsSSA = entry.second;

		const StaticSingleAssignment::TableEntry& usedVarsRenaming = varRenaming.getUseTable()[node];

		if (usedVarsSSA.size() == 0)
			continue;

		if (usedVarsSSA != usedVarsRenaming)
		{
			printf("The different node is %s, line %d: %s\n", node->class_name().c_str(), node->get_file_info()->get_line(),
					node->unparseToString().c_str());
			printf("---VarRenaming--- Result\n");
			StaticSingleAssignment::printUses(usedVarsRenaming);
			printf("---SSA--- Result\n");
			StaticSingleAssignment::printUses(usedVarsSSA);


			ROSE_ASSERT(false);
		}
	}

	//Compare the sizes of the reaching def tables
	if (ssa.getPropDefTable().size() != varRenaming.getPropDefTable().size())
	{
		foreach (entry, ssa.getPropDefTable())
		{
			if (varRenaming.getPropDefTable().count(entry.first) == 0 && entry.second.size() > 0)
			{
				printf("Node %s, %d: %s --- in SSA but not varRenaming reaching def Table\n", entry.first->class_name().c_str(),
						entry.first->get_file_info()->get_line(), entry.first->unparseToString().c_str());
				ssa.printUses(entry.first);
				return 1;
			}
		}

		foreach (entry, varRenaming.getPropDefTable())
		{
			if (ssa.getPropDefTable().count(entry.first) == 0 && entry.second.size() > 0)
			{
				printf("Node %s, %d: %s --- in varRenaming but not SSA reaching def Table\n", entry.first->class_name().c_str(),
						entry.first->get_file_info()->get_line(), entry.first->unparseToString().c_str());
				varRenaming.printUses(entry.first);
				return 1;
			}
		}
	}

	foreach(entry, ssa.getPropDefTable())
	{
		SgNode* node = entry.first;
		const StaticSingleAssignment::TableEntry& reachingVarsSSA = entry.second;

		const StaticSingleAssignment::TableEntry& reachingVarsRenaming = varRenaming.getPropDefTable()[node];

		if (reachingVarsSSA != reachingVarsRenaming)
		{
			printf("Node has different reaching defs: %s, line %d : %s\n", node->class_name().c_str(),
					node->get_file_info()->get_line(), node->unparseToString().c_str());
			StaticSingleAssignment::printDefs(reachingVarsRenaming);
			StaticSingleAssignment::printDefs(reachingVarsSSA);
			ROSE_ASSERT(false);
		}
	}

	return 0;
}
