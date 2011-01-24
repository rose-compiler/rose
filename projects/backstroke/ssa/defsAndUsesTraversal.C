#include "defsAndUsesTraversal.h"
#include "staticSingleAssignment.h"
#include <boost/foreach.hpp>

using namespace std;
using namespace ssa_private;

#define foreach BOOST_FOREACH

ChildUses DefsAndUsesTraversal::evaluateSynthesizedAttribute(SgNode* node, SynthesizedAttributesList attrs)
{
	if (StaticSingleAssignment::getDebug())
	{
		cout << "---------<" << node->class_name() << node << ">-------" << node << endl;
	}

	//We want to propagate the def/use information up from the varRefs to the higher expressions.
	if (SgInitializedName* initName = isSgInitializedName(node))
	{
		VarUniqueName * uName = StaticSingleAssignment::getUniqueName(node);
		ROSE_ASSERT(uName);

		//Add this as a def, unless this initialized name is a preinitialization of a parent class. For example,
		//in the base of B : A(3) { ... } where A is a superclass of B, an initialized name for A appears.
		//Clearly, the initialized name for the parent class is not a real variable
		if (initName->get_preinitialization() != SgInitializedName::e_virtual_base_class
				&& initName->get_preinitialization() != SgInitializedName::e_nonvirtual_base_class)
		{
			ssa->getOriginalDefTable()[node].insert(uName->getKey());

			if (StaticSingleAssignment::getDebug())
			{
				cout << "Defined " << uName->getNameString() << endl;
			}
		}

		return ChildUses();
	}
	//Catch all variable references
	else if (isSgVarRefExp(node))
	{
		//Get the unique name of the def.
		VarUniqueName * uName = StaticSingleAssignment::getUniqueName(node);

		//In some cases, a varRef isn't actually part of a variable name. For example,
		//foo().x where foo returns a structure. x is an SgVarRefExp, but is not part of a variable name.
		if (uName == NULL)
		{
			return ChildUses();
		}

		//Add this as a use. If it's not a use (e.g. target of an assignment), we'll fix it up later.
		ssa->getLocalUsesTable()[node].insert(uName->getKey());

		if (StaticSingleAssignment::getDebug())
		{
			cout << "Found use for " << uName->getNameString() << " at " << node->cfgForBeginning().toStringForDebugging() << endl;
		}

		//This varref is both the only use in the subtree and the current variable
		return ChildUses(node, isSgVarRefExp(node));
	}
	//Catch all types of Binary Operations
	else if (SgBinaryOp* binaryOp = isSgBinaryOp(node))
	{
		ROSE_ASSERT(attrs.size() == 2 && "Error: BinaryOp without exactly 2 children.");
		ChildUses& lhs = attrs[0];
		ChildUses& rhs = attrs[1];

		//If we have an assigning operation, we want to list everything on the LHS as being defined
		//Otherwise, everything is being used.
		vector<SgNode*> uses;
		switch (binaryOp->variantT())
		{
			//All the binary ops that define the LHS
			case V_SgAndAssignOp:
			case V_SgDivAssignOp:
			case V_SgIorAssignOp:
			case V_SgLshiftAssignOp:
			case V_SgMinusAssignOp:
			case V_SgModAssignOp:
			case V_SgMultAssignOp:
			case V_SgPlusAssignOp:
			case V_SgPointerAssignOp:
			case V_SgRshiftAssignOp:
			case V_SgXorAssignOp:
			case V_SgAssignOp:
			{
				//All the uses from the RHS are propagated
				uses.insert(uses.end(), rhs.getUses().begin(), rhs.getUses().end());

				//All the uses from the LHS are propagated, unless we're an assign op
				uses.insert(uses.end(), lhs.getUses().begin(), lhs.getUses().end());

				SgVarRefExp* currentVar = lhs.getCurrentVar();

				if (currentVar != NULL)
				{
					vector<SgNode*>::iterator currVarUse = find(uses.begin(), uses.end(), currentVar);

					//An assign op doesn't use the var it's defining. So, remove that var from the uses
					if (isSgAssignOp(binaryOp))
					{
						if (currVarUse != uses.end())
						{
							uses.erase(currVarUse);
						}

						//Also remove the use from the varRef node, because it's not really a use.
						ssa->getLocalUsesTable()[currentVar].clear();
					}
					//All the other ops always use the var they're defining (+=, -=, /=, etc)
					else
					{
						if (currVarUse == uses.end())
						{
							uses.push_back(currentVar);
						}
					}
				}

				//Set all the uses as being used at this node
				addUsesToNode(binaryOp, uses);

				//Set the current var as being defined here
				//It's possible that the LHS has no variable references. For example,
				//foo() = 3, where foo() returns a reference
				if (currentVar != NULL)
				{
					addDefForVarAtNode(currentVar, binaryOp);
				}

				return ChildUses(uses, currentVar);
			}
			//Otherwise cover all the non-defining Ops
			default:
			{
				//We want to set all the varRefs as being used here
				std::vector<SgNode*> uses;
				uses.insert(uses.end(), lhs.getUses().begin(), lhs.getUses().end());
				uses.insert(uses.end(), rhs.getUses().begin(), rhs.getUses().end());

				//Set all the uses as being used here.
				addUsesToNode(binaryOp, uses);

				//Propagate the current variable up. The rhs variable is the one that could be potentially defined up the tree
				return ChildUses(uses, rhs.getCurrentVar());
			}
		}
	}
	//Catch all unary operations here.
	else if (isSgUnaryOp(node))
	{
		SgUnaryOp* unaryOp = isSgUnaryOp(node);

		//Now handle the uses. All unary operators use everything in their operand
		std::vector<SgNode*> uses;
		if (isSgAddressOfOp(unaryOp) && isSgPointerMemberType(unaryOp->get_type()))
		{
			//SgAddressOfOp is special; it's not always a use of its operand. When creating a reference to a member variable,
			//we create reference without providing a variable instance. For example,
			//		struct foo { int bar; };
			//
			//		void test()
			//		{
			//			int foo::*v = &foo::bar;  <---- There is no use of foo.bar on this line
			//			foo b;
			//			b.*v = 3;
			//		}
			//In this case, there are no uses in the operand. We also want to delete any uses for the children
			vector<SgNode*> successors = SageInterface::querySubTree<SgNode>(unaryOp);
			foreach(SgNode* successor, successors)
			{
				ssa->getLocalUsesTable()[successor].clear();
			}
		}
		else
		{
			//Guard agains unary ops that have no children (exception rethrow statement)
			if (attrs.size() > 0)
			{
				uses.insert(uses.end(), attrs[0].getUses().begin(), attrs[0].getUses().end());
			}
		}

		//For these two definition operations, we want to insert a def for the operand
		SgVarRefExp* currentVar = NULL;
		if (isSgMinusMinusOp(unaryOp) || isSgPlusPlusOp(unaryOp))
		{
			currentVar = attrs[0].getCurrentVar();

			//The defs can be empty. For example, foo()++ where foo returns a reference
			if (currentVar != NULL)
			{
				addDefForVarAtNode(currentVar, unaryOp);

				//++ and -- always use their operand. Make sure it's part of the uses
				if (find(uses.begin(), uses.end(), currentVar) == uses.end())
				{
					uses.push_back(currentVar);
				}
			}
		}
		//Some other ops also preserve the current var. We don't really distinguish between the pointer variable
		//and the value to which it points
		else if (isSgCastExp(unaryOp) || isSgPointerDerefExp(unaryOp) || isSgAddressOfOp(unaryOp))
		{
			currentVar = attrs[0].getCurrentVar();
		}

		//Set all the uses as being used here.
		addUsesToNode(unaryOp, uses);

		//Return the combined uses
		return ChildUses(uses, currentVar);
	}
	else if (isSgStatement(node))
	{
		//Don't propogate uses and defs up to the statement level
		return ChildUses();
	}
	else
	{
		//For the default case, we merge the uses of every attribute and pass them upwards
		std::vector<SgNode*> uses;
		for (unsigned int i = 0; i < attrs.size(); i++)
		{
			if (StaticSingleAssignment::getDebug())
			{
				cout << "Merging attr[" << i << "]" << endl;
			}
			uses.insert(uses.end(), attrs[i].getUses().begin(), attrs[i].getUses().end());
		}

		//Set all the uses as being used here.
		addUsesToNode(node, uses);

		//In the default case, we don't propagate the variable up the tree
		return ChildUses(uses, NULL);
	}
}

/** Mark all the uses as occurring at the specified node. */
void DefsAndUsesTraversal::addUsesToNode(SgNode* node, std::vector<SgNode*> uses)
{
	foreach(SgNode* useNode, uses)
	{
		//Get the unique name of the def.
		VarUniqueName * uName = StaticSingleAssignment::getUniqueName(useNode);
		ROSE_ASSERT(uName);

		//Add the varRef as a def at the current node of the ref's uniqueName
		//We will correct the reference later.
		ssa->getLocalUsesTable()[node].insert(uName->getKey());

		if (StaticSingleAssignment::getDebug())
		{
			cout << "Found use for " << uName->getNameString() << " at " << node->cfgForBeginning().toStringForDebugging() << endl;
		}
	}
}

void DefsAndUsesTraversal::addDefForVarAtNode(SgVarRefExp* currentVar, SgNode* defNode)
{
	const StaticSingleAssignment::VarName& varName = StaticSingleAssignment::getVarName(currentVar);
	ROSE_ASSERT(varName != StaticSingleAssignment::emptyName);

	//Add the varRef as a definition at the current node of the ref's uniqueName
	ssa->getOriginalDefTable()[defNode].insert(varName);

	if (StaticSingleAssignment::getDebug())
	{
		cout << "Found def for " << StaticSingleAssignment::varnameToString(varName)
				<< " at " << defNode->cfgForBeginning().toStringForDebugging() << endl;
	}
}