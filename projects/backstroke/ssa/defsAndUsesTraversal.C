#include "defsAndUsesTraversal.h"
#include "staticSingleAssignment.h"
#include <boost/foreach.hpp>

using namespace std;
using namespace ssa_private;

#define foreach BOOST_FOREACH

ChildDefsAndUses DefsAndUsesTraversal::evaluateSynthesizedAttribute(SgNode* node, SynthesizedAttributesList attrs)
{
	if (StaticSingleAssignment::getDebug())
	{
		cout << "---------<" << node->class_name() << node << ">-------" << node << endl;
	}
	//We want to propogate the def/use information up from the varRefs to the higher expressions.
	if (isSgInitializedName(node))
	{
		SgInitializedName* name = isSgInitializedName(node);
		ROSE_ASSERT(name);

		VarUniqueName * uName = StaticSingleAssignment::getUniqueName(name);
		ROSE_ASSERT(uName);

		//Add this as a def. [node][uniqueName]
		ssa->getOriginalDefTable()[name].insert(uName->getKey());

		if (StaticSingleAssignment::getDebug())
		{
			cout << "Defined " << uName->getNameString() << endl;
		}

		//An SgInitializedName should count as a def, since it is the initial definition.
		return ChildDefsAndUses(name, NULL);
	}
	//Catch all variable references
	else if (isSgVarRefExp(node))
	{
		SgVarRefExp* varRef = isSgVarRefExp(node);
		ROSE_ASSERT(varRef);

		//Get the unique name of the def.
		VarUniqueName * uName = StaticSingleAssignment::getUniqueName(varRef);
		ROSE_ASSERT(uName);

		//Add this as a use.
		ssa->getLocalUsesTable()[varRef].insert(uName->getKey());

		if (StaticSingleAssignment::getDebug())
		{
			cout << "Found use for " << uName->getNameString() << " at " << varRef->cfgForBeginning().toStringForDebugging() << endl;
		}

		//A VarRef is always a use, it only becomes defined by the parent assignment.
		return ChildDefsAndUses(NULL, varRef);
	}
	//Catch all types of Binary Operations
	else if (isSgBinaryOp(node))
	{
		SgBinaryOp* binaryOp = isSgBinaryOp(node);
		ROSE_ASSERT(attrs.size() == 2 && "Error: BinaryOp without exactly 2 children.");
		ChildDefsAndUses& lhs = attrs[0];
		ChildDefsAndUses& rhs = attrs[1];

		//If we have an assigning operation, we want to list everything on the LHS as being defined
		//Otherwise, everything is being used.
		std::vector<SgNode*> uses;
		switch (binaryOp->variantT())
		{
			//All the following ops both use and define the left-hand side
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
			{
				//All the uses from the LHS are propagated
				uses.insert(uses.end(), lhs.getDefs().begin(), lhs.getDefs().end());
				uses.insert(uses.end(), lhs.getUses().begin(), lhs.getUses().end());
			}

			//The assign op defines, but does not use the LHS. Notice that the other assignments also fall through,
			//as they also define the LHS
			case V_SgAssignOp:
			{
				//All the uses from the RHS are propagated
				uses.insert(uses.end(), rhs.getDefs().begin(), rhs.getDefs().end());
				uses.insert(uses.end(), rhs.getUses().begin(), rhs.getUses().end());

				//Set all the rhs uses as being used at this node
				addUsesToNode(binaryOp, uses);

				//We want to set all the right-most varRef from LHS as being defined
				std::vector<SgNode*> defs;
				defs.insert(defs.end(), lhs.getDefs().begin(), lhs.getDefs().end());
				defs.insert(defs.end(), lhs.getUses().begin(), lhs.getUses().end());

				//It's possible that the LHS has no variable references. For example,
				//foo() = 3, where foo() returns a reference
				SgNode* def = NULL;
				if (!defs.empty())
				{
					//Set only the last def as being defined here.
					def = defs.back();
					//Get the unique name of the def.
					VarUniqueName * uName = StaticSingleAssignment::getUniqueName(def);
					ROSE_ASSERT(uName);

					//Add the varRef as a definition at the current node of the ref's uniqueName
					ssa->getOriginalDefTable()[binaryOp].insert(uName->getKey());

					if (StaticSingleAssignment::getDebug())
					{
						cout << "Found def for " << uName->getNameString() << " at " << binaryOp->cfgForBeginning().toStringForDebugging() << endl;
					}
				}

				//Cut off the uses here. We will only pass up the defs.
				return ChildDefsAndUses(def, NULL);
			}
			//Otherwise cover all the non-defining Ops
			default:
			{
				//We want to set all the varRefs as being used here
				std::vector<SgNode*> uses;
				uses.insert(uses.end(), lhs.getDefs().begin(), lhs.getDefs().end());
				uses.insert(uses.end(), lhs.getUses().begin(), lhs.getUses().end());
				uses.insert(uses.end(), rhs.getDefs().begin(), rhs.getDefs().end());
				uses.insert(uses.end(), rhs.getUses().begin(), rhs.getUses().end());

				//Set all the uses as being used here.
				addUsesToNode(binaryOp, uses);

				//Return all the uses.
				return ChildDefsAndUses(NULL, uses);
			}
		}
	}
	//Catch all unary operations here.
	else if (isSgUnaryOp(node))
	{
		SgUnaryOp* op = isSgUnaryOp(node);

		//If we have an assigning operation, we want to list everything as being defined and used
		//Otherwise, everything is being used.
		VariantT type = op->variantT();

		std::vector<SgNode*> defs, uses;
		if (type == V_SgMinusMinusOp || type == V_SgPlusPlusOp)
		{
			defs.insert(defs.end(), attrs[0].getDefs().begin(), attrs[0].getDefs().end());
			defs.insert(defs.end(), attrs[0].getUses().begin(), attrs[0].getUses().end());

			//The defs can be empty. For example, foo()++ where foo returns a reference
			if (!defs.empty())
			{
				//Set only the last def as being defined here.
				SgNode* def = defs.back();
				//Get the unique name of the def.
				VarUniqueName * uName = StaticSingleAssignment::getUniqueName(def);
				ROSE_ASSERT(uName);

				//Add the varRef as a definition at the current node of the ref's uniqueName
				ssa->getOriginalDefTable()[op].insert(uName->getKey());

				if (StaticSingleAssignment::getDebug())
				{
					cout << "Found def for " << uName->getNameString() << " at " << op->cfgForBeginning().toStringForDebugging() << endl;
				}
			}
		}

		//Now handle the uses. Note that the defining ops fall through
		if (type == V_SgAddressOfOp && isSgPointerMemberType(op->get_type()))
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
			vector<SgNode*> successors = SageInterface::querySubTree<SgNode>(op);
			foreach(SgNode* successor, successors)
			{
				ssa->getLocalUsesTable()[successor].clear();
			}
		}
		else
		{
			//For all non-defining Unary Ops, add all of them as uses
			//We want to set all the varRefs as being used here

			//Guard agains unary ops that have no children (exception rethrow statement)
			if (attrs.size() > 0)
			{
				uses.insert(uses.end(), attrs[0].getDefs().begin(), attrs[0].getDefs().end());
				uses.insert(uses.end(), attrs[0].getUses().begin(), attrs[0].getUses().end());
			}

			//Set all the uses as being used here.
			addUsesToNode(op, uses);
		}

		//Return the combined defs and uses.
		return ChildDefsAndUses(defs, uses);
	}
	else if (isSgStatement(node))
	{
		//Don't propogate uses and defs up to the statement level
		return ChildDefsAndUses();
	}

	//For the default case, we merge the defs and uses of every attribute and pass them upwards
	std::vector<SgNode*> defs;
	std::vector<SgNode*> uses;
	for (unsigned int i = 0; i < attrs.size(); i++)
	{
		if (StaticSingleAssignment::getDebug())
		{
			cout << "Merging attr[" << i << "]" << endl;
		}
		//defs.insert(defs.end(), attrs[i].getDefs().begin(), attrs[i].getDefs().end());
		//George Vulov 9/13/2010: We don't propagate defs up the tree by default, just uses.
		//If we propagate defs up the tree for arbitrary nodes, e.g. (SgInitializer), then we get spurious defs
		uses.insert(uses.end(), attrs[i].getDefs().begin(), attrs[i].getDefs().end());
		uses.insert(uses.end(), attrs[i].getUses().begin(), attrs[i].getUses().end());
	}

	//George Vulov 9/13/2010: We don't propagate defs up the tree by default, just uses.
	//If we propagate defs up the tree for arbitrary nodes, e.g. (SgInitializer), then we get spurious defs

	//Set all the uses as being used here.
	addUsesToNode(node, uses);

	return ChildDefsAndUses(defs, uses);
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