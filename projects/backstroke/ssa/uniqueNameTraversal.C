#include "uniqueNameTraversal.h"
#include "staticSingleAssignment.h"

using namespace std;
using namespace ssa_private;

VariableReferenceSet UniqueNameTraversal::evaluateSynthesizedAttribute(SgNode* node, SynthesizedAttributesList attrs)
{
	if (StaticSingleAssignment::getDebugExtra())
	{
		cout << "Examining " << node->class_name() << node << endl;
	}
	//First we check if this is an initName
	if (isSgInitializedName(node))
	{
		SgInitializedName* name = isSgInitializedName(node);

		//We want to assign this node its unique name, as well as adding it to the defs.
		VarUniqueName* uName = new VarUniqueName(name);
		name->setAttribute(StaticSingleAssignment::varKeyTag, uName);

		return VariableReferenceSet(name);
	}
		//Next, see if it is a varRef
	else if (isSgVarRefExp(node))
	{
		SgVarRefExp* var = isSgVarRefExp(node);

		if (attrs.size() != 0)
		{
			cout << "Error: Found VarRef with children." << endl;
			ROSE_ASSERT(false);
		}

		//We want to assign this node its unique name, as well as adding it to the defs.
		VarUniqueName* uName = new VarUniqueName(var->get_symbol()->get_declaration());
		var->setAttribute(StaticSingleAssignment::varKeyTag, uName);

		return VariableReferenceSet(var);
	}
		//We check if it is a 'this' expression, since we want to be able to version 'this' as well.
		//We don't have an SgInitializedName for 'this', so we use a flag in the unique names
	else if (isSgThisExp(node))
	{
		SgThisExp* thisExp = isSgThisExp(node);

		if (attrs.size() != 0)
		{
			cout << "Error: Found ThisExp with children." << endl;
			ROSE_ASSERT(false);
		}

		return VariableReferenceSet(thisExp);
	}
		//Now we check if we have reached a Dot Expression, where we have to merge names.
	else if (isSgDotExp(node))
	{
		if (attrs.size() != 2)
		{
			cout << "Error: Found dot operator without exactly 2 children." << endl;
			ROSE_ASSERT(false);
		}

		//We want to update the naming for the RHS child
		//Check if the LHS has at least one varRef
		if (attrs[0].getRefs().size() > 0)
		{
			bool thisExp = false;
			//Check if our LHS varRef is the 'this' expression
			if (isSgThisExp(attrs[0].getRefs()[attrs[0].getRefs().size() - 1]))
			{
				thisExp = true;
			}

			//Get the unique name from the highest varRef in the LHS, since this will have the most
			//fully qualified UniqueName.
			VarUniqueName* lhsName;
			if (!thisExp)
			{
				lhsName = dynamic_cast<VarUniqueName*> (attrs[0].getRefs()[attrs[0].getRefs().size() - 1]->getAttribute(StaticSingleAssignment::varKeyTag));
				ROSE_ASSERT(lhsName);
			}

			//Check if the RHS has a single varRef
			if (attrs[1].getRefs().size() == 1)
			{
				SgVarRefExp* varRef = isSgVarRefExp(attrs[1].getRefs()[0]);

				if (varRef)
				{
					if (!thisExp)
					{
						//Create the uniqueName from the uniqueName of the lhs prepended to the rhs uniqueName
						VarUniqueName* uName = new VarUniqueName(lhsName->getKey(), varRef->get_symbol()->get_declaration());
						uName->setUsesThis(lhsName->getUsesThis());
						varRef->setAttribute(StaticSingleAssignment::varKeyTag, uName);

						VarUniqueName* uName2 = new VarUniqueName(*uName);
						node->setAttribute(StaticSingleAssignment::varKeyTag, uName2);
					}
					else
					{
						//Create the UniqueName from the current varRef, and stores that it uses 'this'
						VarUniqueName* uName = new VarUniqueName(varRef->get_symbol()->get_declaration());
						uName->setUsesThis(true);
						varRef->setAttribute(StaticSingleAssignment::varKeyTag, uName);

						VarUniqueName* uName2 = new VarUniqueName(*uName);
						node->setAttribute(StaticSingleAssignment::varKeyTag, uName2);
					}

					//Return the combination of the LHS and RHS varRefs
					return VariableReferenceSet(attrs[0].getRefs(), varRef);
				}
				else
				{
					//Since the RHS has no varRef, we can no longer
					//establish a direct reference chain between the LHS and any varRefs
					//further up the tree.
					return VariableReferenceSet();
				}
			}
			else
			{
				//Since the RHS has no varRef, we can no longer
				//establish a direct reference chain between the LHS and any varRefs
				//further up the tree.
				return VariableReferenceSet();
			}
		}
		else
		{

			//The LHS has no varRefs, so there can be no explicit naming of varRefs above this point.

			//We need to check if the RHS had a name assigned.
			//If so, we need to remove it, because it will be incorrect.
			if (attrs[1].getRefs().size() == 1)
			{
				SgVarRefExp* varRef = isSgVarRefExp(attrs[1].getRefs()[0]);

				if (varRef)
				{
					//If the RHS is a varRef and has the naming attribute set, remove it
					if (varRef->attributeExists(StaticSingleAssignment::varKeyTag))
					{
						VarUniqueName* name = dynamic_cast<VarUniqueName*> (varRef->getAttribute(StaticSingleAssignment::varKeyTag));
						varRef->removeAttribute(StaticSingleAssignment::varKeyTag);
						delete name;
					}
				}
			}

			return VariableReferenceSet();
		}
	}
		//Now we check if we have reached an ArrowExpression, we have to merge names.
	else if (isSgArrowExp(node))
	{
		if (attrs.size() != 2)
		{
			cout << "Error: Found arrow operator without exactly 2 children." << endl;
			ROSE_ASSERT(false);
		}

		//We want to update the naming for the RHS child
		//Check if the LHS has at least one varRef
		if (attrs[0].getRefs().size() > 0)
		{
			bool thisExp = false;
			//Check if our LHS varRef is the 'this' expression
			if (isSgThisExp(attrs[0].getRefs()[attrs[0].getRefs().size() - 1]))
			{
				thisExp = true;
			}

			//Get the unique name from the highest varRef in the LHS, since this will have the most
			//fully qualified UniqueName.
			VarUniqueName* lhsName;
			if (!thisExp)
			{
				lhsName = dynamic_cast<VarUniqueName*> (attrs[0].getRefs()[attrs[0].getRefs().size() - 1]->getAttribute(StaticSingleAssignment::varKeyTag));
				ROSE_ASSERT(lhsName);
			}

			//Check if the RHS has a single varRef
			if (attrs[1].getRefs().size() == 1)
			{
				SgVarRefExp* varRef = isSgVarRefExp(attrs[1].getRefs()[0]);

				if (varRef)
				{
					if (!thisExp)
					{
						//Create the uniqueName from the uniqueName of the lhs prepended to the rhs uniqueName
						VarUniqueName* uName = new VarUniqueName(lhsName->getKey(), varRef->get_symbol()->get_declaration());
						uName->setUsesThis(lhsName->getUsesThis());
						varRef->setAttribute(StaticSingleAssignment::varKeyTag, uName);

						VarUniqueName* uName2 = new VarUniqueName(*uName);
						node->setAttribute(StaticSingleAssignment::varKeyTag, uName2);
					}
					else
					{
						//Create the UniqueName from the current varRef, and stores that it uses 'this'
						VarUniqueName* uName = new VarUniqueName(varRef->get_symbol()->get_declaration());
						uName->setUsesThis(true);
						varRef->setAttribute(StaticSingleAssignment::varKeyTag, uName);

						VarUniqueName* uName2 = new VarUniqueName(*uName);
						node->setAttribute(StaticSingleAssignment::varKeyTag, uName2);
					}

					//Return the combination of the LHS and RHS varRefs
					return VariableReferenceSet(attrs[0].getRefs(), varRef);
				}
				else
				{
					//Since the RHS has no varRef, we can no longer
					//establish a direct reference chain between the LHS and any varRefs
					//further up the tree.
					return VariableReferenceSet();
				}
			}
			else
			{
				//Since the RHS has no varRef, we can no longer
				//establish a direct reference chain between the LHS and any varRefs
				//further up the tree.
				return VariableReferenceSet();
			}
		}
		else
		{
			//The LHS has no varRefs, so there can be no explicit naming of varRefs above this point.

			//We need to check if the RHS had a name assigned.
			//If so, we need to remove it, because it will be incorrect.
			if (attrs[1].getRefs().size() == 1)
			{
				SgVarRefExp* varRef = isSgVarRefExp(attrs[1].getRefs()[0]);

				if (varRef)
				{
					//If the RHS is a varRef and has the naming attribute set, remove it
					if (varRef->attributeExists(StaticSingleAssignment::varKeyTag))
					{
						VarUniqueName* name = dynamic_cast<VarUniqueName*> (varRef->getAttribute(StaticSingleAssignment::varKeyTag));
						varRef->removeAttribute(StaticSingleAssignment::varKeyTag);
						delete name;
					}
				}
			}

			return VariableReferenceSet();
		}
	}
		//Now we hit the default case. We should return a merged list.
	else
	{
		std::vector<SgNode*> names;
		for (unsigned int i = 0; i < attrs.size(); i++)
		{
			names.insert(names.end(), attrs[i].getRefs().begin(), attrs[i].getRefs().end());
		}

		return VariableReferenceSet(names);
	}
}
