#include "uniqueNameTraversal.h"
#include "staticSingleAssignment.h"
#include <boost/foreach.hpp>

#define foreach BOOST_FOREACH

using namespace std;
using namespace ssa_private;

string UniqueNameTraversal::varKeyTag = "ssa_varname_KeyTag";

VariableReferenceSet UniqueNameTraversal::evaluateSynthesizedAttribute(SgNode* node, SynthesizedAttributesList attrs)
{
    if (StaticSingleAssignment::getDebugExtra())
    {
        cout << "Examining " << node->class_name() << node << endl;
    }
    //First we check if this is an initName
    if (isSgInitializedName(node))
    {
        SgInitializedName* name = resolveTemporaryInitNames(isSgInitializedName(node));

        //We want to assign this node its unique name, as well as adding it to the defs.
        VarUniqueName* uName = new VarUniqueName(name);
        node->setAttribute(varKeyTag, uName);

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

        //We want to assign this node its unique name, as well as adding it to the uses.
        VarUniqueName* uName = new VarUniqueName(resolveTemporaryInitNames(var->get_symbol()->get_declaration()));
        var->setAttribute(varKeyTag, uName);

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
    else if (isSgDotExp(node) || (isSgArrowExp(node) && treatPointersAsStructs))
    {
        if (attrs.size() != 2)
        {
            cout << "Error: Found dot or arrow operator without exactly 2 children." << endl;
            ROSE_ASSERT(false);
        }

        SgNode* lhsVar = attrs[0].getCurrentVar();
        SgNode* rhsVar = attrs[1].getCurrentVar();

        //We want to update the naming for the RHS child
        //Check if the LHS has at least one varRef
        if (lhsVar != NULL)
        {
            //Check if our LHS varRef is the 'this' expression
            SgThisExp* thisExp = isSgThisExp(lhsVar);

            //Get the unique name from the varRef in the LHS
            VarUniqueName* lhsName = NULL;
            if (thisExp == NULL)
            {
                lhsName = dynamic_cast<VarUniqueName*> (lhsVar->getAttribute(varKeyTag));
                ROSE_ASSERT(lhsName);
            }

            //Check if the RHS has a single varRef
            if (rhsVar != NULL)
            {
                SgVarRefExp* varRef = isSgVarRefExp(rhsVar);

                if (varRef != NULL)
                {
                    if (thisExp == NULL)
                    {
                        //Create the uniqueName from the uniqueName of the lhs prepended to the rhs uniqueName
                        VarUniqueName* uName = new VarUniqueName(lhsName->getKey(),
                                resolveTemporaryInitNames(varRef->get_symbol()->get_declaration()));
                        uName->setUsesThis(lhsName->getUsesThis());
                        varRef->setAttribute(varKeyTag, uName);

                        VarUniqueName* uName2 = new VarUniqueName(*uName);
                        node->setAttribute(varKeyTag, uName2);
                    }
                    else
                    {
                        //Create the UniqueName from the current varRef, and stores that it uses 'this'
                        VarUniqueName* uName = new VarUniqueName(resolveTemporaryInitNames(varRef->get_symbol()->get_declaration()));
                        uName->setUsesThis(true);
                        varRef->setAttribute(varKeyTag, uName);

                        VarUniqueName* uName2 = new VarUniqueName(*uName);
                        node->setAttribute(varKeyTag, uName2);

                        //Add a varName to the this expression also
                        uName2 = new VarUniqueName(*uName);
                        thisExp->setAttribute(varKeyTag, uName2);
                    }

                    //Return the combination of the LHS and RHS varRefs
                    return VariableReferenceSet(node);
                }
                else
                {
                    //Since the RHS has no varRef, we can no longer
                    //establish a direct reference chain between the LHS and any varRefs
                    //further up the tree.
                    return VariableReferenceSet(NULL);
                }
            }
            else
            {
                //Since the RHS has no varRef, we can no longer
                //establish a direct reference chain between the LHS and any varRefs
                //further up the tree.
                return VariableReferenceSet(NULL);
            }
        }
        else
        {
            //The LHS has no varRefs, so there can be no explicit naming of varRefs above this point.

            //We need to check if the RHS had a name assigned.
            //If so, we need to remove it, because it will be incorrect.
            if (rhsVar != NULL)
            {
                SgVarRefExp* varRef = isSgVarRefExp(rhsVar);
                ROSE_ASSERT(varRef);

                //If the RHS is a varRef and has the naming attribute set, remove it
                if (varRef->attributeExists(varKeyTag))
                {
                    VarUniqueName* name = dynamic_cast<VarUniqueName*> (varRef->getAttribute(varKeyTag));
                    varRef->removeAttribute(varKeyTag);
                    delete name;
                }
            }

            return VariableReferenceSet(NULL);
        }
    }
    
    //If we have an arrow expression and we don't treat pointers as structs, delete
    //the variable from the right-hand side
    else if (isSgArrowExp(node) && !treatPointersAsStructs)
    {
        ROSE_ASSERT(attrs.size() == 2);

        SgNode* rhsVar = attrs[1].getCurrentVar();
        if (rhsVar != NULL)
        {
            SgVarRefExp* varRef = isSgVarRefExp(rhsVar);
            ROSE_ASSERT(varRef);

            //If the RHS is a varRef and has the naming attribute set, remove it
            if (varRef->attributeExists(varKeyTag))
            {
                VarUniqueName* name = dynamic_cast<VarUniqueName*> (varRef->getAttribute(varKeyTag));
                ROSE_ASSERT(name != NULL);
                varRef->removeAttribute(varKeyTag);
                delete name;
            }
        }
    }

    else if (isSgCastExp(node))
    {
        ROSE_ASSERT(attrs.size() > 0);
        return attrs.front();
    }

    //If we don't distinguish between arrow and dot operations, then dereferencing preserves the current variable.
    else if (isSgPointerDerefExp(node) && treatPointersAsStructs)
    {
        ROSE_ASSERT(attrs.size() == 1);
        return attrs.front();
    }

    //This allows us to treat (a, b).x as b.x
    else if (isSgCommaOpExp(node) && propagateNamesThroughComma)
    {
        ROSE_ASSERT(attrs.size() == 2);
        return attrs.back();
    }

    //Now we hit the default case. Names should not propagate up
    else
    {
        return VariableReferenceSet(NULL);
    }
}

SgInitializedName* UniqueNameTraversal::resolveTemporaryInitNames(SgInitializedName* name)
{
    //Initialized names are children of varRefs when names are used before they are declared (possible in class definitions)
    if (isSgVarRefExp(name->get_parent()))
    {

        foreach(SgInitializedName* otherName, allInitNames)
        {
            if (otherName->get_prev_decl_item() == name)
                return otherName;
        }
    }

    //Class variables defined in constructor pre-initializer lists have an extra initialized name there (so there can be an assign
    // initializer). Track down the real initialized name corresponding to the declaration of the variable inside a class scope
    if (isSgCtorInitializerList(name->get_declaration()))
    {
        if (name->get_prev_decl_item() != NULL)
        {
            return name->get_prev_decl_item();
        }
    }

    return name;
}
