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
			CFGNode definingNode = initName->cfgForBeginning();
			
			cfgNodeToDefinedVars[definingNode].insert(uName->getKey());

			if (StaticSingleAssignment::getDebug())
			{
				cout << "Defined " << StaticSingleAssignment::varnameToString(uName->getKey()) << 
						" at " << definingNode.toStringForDebugging() << endl;
			}
		}

		return ChildUses();
	}
	//Catch all variable references. These are uses
    else if (SgVarRefExp* varRef = isSgVarRefExp(node))
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
		astNodeToUsedVars[varRef].insert(varRef);

		if (StaticSingleAssignment::getDebug())
		{
			cout << "Found use for " << StaticSingleAssignment::varnameToString(uName->getKey()) << 
					" at " << node->cfgForBeginning().toStringForDebugging() << endl;
		}

		//This varref is both the only use in the subtree and the current variable
		return ChildUses(varRef, varRef);
	}
	//Catch all types of Binary Operations
	else if (SgBinaryOp* binaryOp = isSgBinaryOp(node))
	{
		ROSE_ASSERT(attrs.size() == 2 && "Error: BinaryOp without exactly 2 children.");
		ChildUses& lhs = attrs[0];
		ChildUses& rhs = attrs[1];

		//If we have an assigning operation, we want to list everything on the LHS as being defined
		//Otherwise, everything is being used.
		set<SgVarRefExp*> uses;
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
				//All the binary ops have 3 indices in the CFG. The first index is before the lhs operand,
				//the second index is after the lhs operand but before the rhs operand. 
				ROSE_ASSERT(binaryOp->cfgIndexForEnd() == 2);
				
				//All the uses from the RHS are propagated
				uses.insert(rhs.getUses().begin(), rhs.getUses().end());

				//All the uses from the LHS are propagated, unless we're an assign op
				uses.insert(lhs.getUses().begin(), lhs.getUses().end());

				SgVarRefExp* currentVar = lhs.getCurrentVar();

				if (currentVar != NULL)
				{
					set<SgVarRefExp*>::iterator lhsCurrVarUse = lhs.getUses().find(currentVar);
					set<SgVarRefExp*>::iterator rhsCurrVarUse = rhs.getUses().find(currentVar);
					//How can we have an active variable without any uses?
					ROSE_ASSERT(uses.find(currentVar) != uses.end());
					
					//An assign op doesn't use the var it's defining. So, if it's only used in the left-hand side,
					//we should remove it from the list of used vars
					if (isSgAssignOp(binaryOp))
					{
						if (lhsCurrVarUse != lhs.getUses().end() && rhsCurrVarUse == rhs.getUses().end())
						{
							set<SgVarRefExp*>::iterator currVarUse = uses.find(currentVar);
							ROSE_ASSERT(currVarUse != uses.end());
							uses.erase(currVarUse);
							
							//Also remove the use from the varRef node, because it's not really a use.
							//FIXME: this doesn't erase uses in the parents of the SgVarRefExp.
							//		We can end up with false positives for uses.
							astNodeToUsedVars.erase(currentVar);
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
					//The definition actually happens after both sides are evaluated
					CFGNode definingCfgNode = binaryOp->cfgForEnd();
					addDefForVarAtNode(currentVar, definingCfgNode);
				}
				
				return ChildUses(uses, currentVar);
			}
			//Otherwise cover all the non-defining Ops
			default:
			{
				//We want to set all the varRefs as being used here
				std::set<SgVarRefExp*> uses;
				uses.insert(lhs.getUses().begin(), lhs.getUses().end());
				uses.insert(rhs.getUses().begin(), rhs.getUses().end());

				//Set all the uses as being used here.
				addUsesToNode(binaryOp, uses);
				
				SgVarRefExp* newCurrentVar = NULL;
				
				//An assignment to a comma op assigns to its rhs member. 
				//Same is true for arrow/dot
				if (isSgCommaOpExp(binaryOp) || isSgDotExp(binaryOp) || isSgArrowExp(binaryOp))
				{
					newCurrentVar = rhs.getCurrentVar();
				}
				
				return ChildUses(uses, newCurrentVar);
			}
		}
	}
	//Catch all unary operations here.
	else if (SgUnaryOp* unaryOp = isSgUnaryOp(node))
	{
		//Now handle the uses. All unary operators use everything in their operand
		std::set<SgVarRefExp*> uses;
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
				astNodeToUsedVars.erase(successor);
			}
		}
		else
		{
			//We now know that the unary op is a use. We have to merge its child uses
			if (isSgCastExp(unaryOp))
			{
				//Cast expressions can have two children in the AST, because of the original expression tree
				ROSE_ASSERT(attrs.size() == 2);
				uses.insert(attrs[0].getUses().begin(), attrs[0].getUses().end());
			}
			else if (isSgThrowOp(unaryOp))
			{
				//Rethrow is a unary op that can have no children
				ROSE_ASSERT(attrs.size() <= 1);
				if (attrs.size() == 1)
					uses = attrs[0].getUses();
			}
			else
			{
				ROSE_ASSERT(attrs.size() == 1);
				uses = attrs[0].getUses();
			}
		}

		//For these definition operations, we want to insert a def for the operand.
		SgVarRefExp* currentVar = NULL;
		if (isSgMinusMinusOp(unaryOp) || isSgPlusPlusOp(unaryOp))
		{
			currentVar = attrs[0].getCurrentVar();

			//The defs can be empty. For example, foo()++ where foo returns a reference
			if (currentVar != NULL)
			{
				//The increment/decrement ops should have exactly two indices.
				//The first index comes before the operand and the second index comes after the operand
				ROSE_ASSERT(unaryOp->cfgIndexForEnd() == 1);
				
				CFGNode definingNode;
				
				//We always make the definition of a ++ or -- op appear after the operand is evaluated, regardless
				//of whether it's a pre- or post- op. Otherwise, the operand may get the wrong reaching definition
				//if (unaryOp->get_mode() == SgUnaryOp::prefix)
				//	definingNode = unaryOp->cfgForBeginning();
				//else if (unaryOp->get_mode() == SgUnaryOp::postfix)
					definingNode = unaryOp->cfgForEnd();
				//else
				//	ROSE_ASSERT(false);
				
				
				addDefForVarAtNode(currentVar, definingNode);

				//++ and -- always use their operand. Make sure it's part of the uses
				ROSE_ASSERT(uses.find(currentVar) != uses.end());
			}
		}
		//Some other ops also preserve the current var. 
		//FIXME: We don't really distinguish between the pointer variable
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
	else if (SgDeleteExp* deleteExp = isSgDeleteExp(node))
	{
		//Deleting a variable definitely modifies it.
		ROSE_ASSERT(attrs.size() == 1);
		SgVarRefExp* currentVar = attrs.front().getCurrentVar();
		
		if (currentVar != NULL)
		{
			//The delete expression should have exactly two indices. One is invoked before the operand
			//and one after the operand
			ROSE_ASSERT(deleteExp->cfgIndexForEnd() == 1);
			
			CFGNode definingNode = deleteExp->cfgForEnd();
			
			addDefForVarAtNode(currentVar, definingNode);
			return ChildUses(attrs.front().getUses());
		}
        else
        {
            return ChildUses();
        }
	}
	else if (isSgStatement(node) && !isSgDeclarationStatement(node))
	{
		//Don't propagate uses up to the statement level
		return ChildUses();
	}
	else
	{
		//For the default case, we merge the uses of every attribute and pass them upwards
		std::set<SgVarRefExp*> uses;
		for (unsigned int i = 0; i < attrs.size(); i++)
		{
			if (StaticSingleAssignment::getDebug())
			{
				cout << "Merging attr[" << i << "]" << endl;
			}
			uses.insert(attrs[i].getUses().begin(), attrs[i].getUses().end());
		}

		//Set all the uses as being used here.
		addUsesToNode(node, uses);

		//In the default case, we don't propagate the variable up the tree
		return ChildUses(uses, NULL);
	}
}

/** Mark all the uses as occurring at the specified node. */
void DefsAndUsesTraversal::addUsesToNode(SgNode* node, std::set<SgVarRefExp*> uses)
{
	astNodeToUsedVars[node].insert(uses.begin(), uses.end());
}


void DefsAndUsesTraversal::addDefForVarAtNode(SgVarRefExp* var, const CFGNode& node)
{
	const StaticSingleAssignment::VarName& varName = StaticSingleAssignment::getVarName(var);
	ROSE_ASSERT(varName != StaticSingleAssignment::emptyName);
	
	cfgNodeToDefinedVars[node].insert(varName);
	
	if (StaticSingleAssignment::getDebug())
	{
		cout << "Found def for " << StaticSingleAssignment::varnameToString(varName)
				<< " at " << node.toStringForDebugging() << endl;
	}
}

void DefsAndUsesTraversal::CollectDefsAndUses(SgNode* traversalRoot, DefsAndUsesTraversal::CFGNodeToVarsMap& defs, 
		std::map<SgNode*, std::set<SgVarRefExp*> >& uses)
{
	DefsAndUsesTraversal traversal;
	traversal.traverse(traversalRoot);
	
	defs.insert(traversal.cfgNodeToDefinedVars.begin(), traversal.cfgNodeToDefinedVars.end());
	uses.insert(traversal.astNodeToUsedVars.begin(), traversal.astNodeToUsedVars.end());
}