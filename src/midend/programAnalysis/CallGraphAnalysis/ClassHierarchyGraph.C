// tps : Switching from rose.h to sage3 changed size from 17,7 MB to 7,3MB
#include "sage3basic.h"

// DQ (10/14/2010):  This should only be included by source files that require it.
// This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
// Interestingly it must be at the top of the list of include files.
#include "rose_config.h"

#include "CallGraph.h"
#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>

#define foreach BOOST_FOREACH
using namespace std;
using namespace boost;


ClassHierarchyWrapper::ClassHierarchyWrapper(SgNode *node)
{
    ROSE_ASSERT(isSgProject(node));

    Rose_STL_Container<SgNode *> allCls;
    allCls = NodeQuery::querySubTree(node, V_SgClassDefinition);

    // build the class hierarchy
    // start by iterating through all the classes
    for (Rose_STL_Container<SgNode *>::iterator it = allCls.begin(); it != allCls.end(); it++)
    {
        SgClassDefinition *clsDescDef = isSgClassDefinition(*it);
        SgBaseClassPtrList &baseClses = clsDescDef->get_inheritances();

        ClassDefSet& classParents = directParents[clsDescDef->get_declaration()->get_mangled_name()];
        
        // for each iterate through their parents and add parent - child relationship to the graph
        for (SgBaseClassPtrList::iterator it = baseClses.begin(); it != baseClses.end(); it++)
        {
            //AS (032806) Added fix to get the defining class declaration
            SgClassDeclaration *baseCls = isSgClassDeclaration((*it)->get_base_class()->get_definingDeclaration());
            ROSE_ASSERT(baseCls != NULL);
            SgClassDefinition *baseClsDef = baseCls->get_definition();
            ROSE_ASSERT(baseClsDef != NULL);
            
            classParents.insert(baseClsDef);
            directChildren[baseCls->get_mangled_name()].insert(clsDescDef);
        }
    }
    
   
    //Now populate the ancestor/all subclasses maps
    buildAncestorsMap(directParents, ancestorClasses);
    buildAncestorsMap(directChildren, subclasses);
}

const ClassHierarchyWrapper::ClassDefSet& ClassHierarchyWrapper::getSubclasses( SgClassDefinition *cls ) const
{
    const ClassDefSet* result = NULL;
    MangledNameToClassDefsMap::const_iterator children = subclasses.find(cls->get_declaration()->get_mangled_name());
    if (children == subclasses.end())
    {
        static ClassDefSet emptySet;
        result = &emptySet;
    }
    else
    {
        result = &children->second;
    }
    
    return *result;
}


const ClassHierarchyWrapper::ClassDefSet& ClassHierarchyWrapper::getAncestorClasses( SgClassDefinition *cls ) const
{
    const ClassDefSet* result = NULL;
    MangledNameToClassDefsMap::const_iterator children = ancestorClasses.find(cls->get_declaration()->get_mangled_name());
    if (children == ancestorClasses.end())
    {
        static ClassDefSet emptySet;
        result = &emptySet;
    }
    else
    {
        result = &children->second;
    }

    return *result;
}

const ClassHierarchyWrapper::ClassDefSet& ClassHierarchyWrapper::getDirectSubclasses(SgClassDefinition * cls) const
{
    const ClassDefSet* result = NULL;

    MangledNameToClassDefsMap::const_iterator children = directChildren.find(cls->get_declaration()->get_mangled_name());
    if (children == directChildren.end())
    {
        static ClassDefSet emptySet;
        result = &emptySet;
    }
    else
    {
        result = &children->second;
    }

    return *result;
}


void ClassHierarchyWrapper::buildAncestorsMap(const MangledNameToClassDefsMap& parents, MangledNameToClassDefsMap& transitiveParents)
{
    transitiveParents.clear();

    //Iterate over all the classes and perform DFS for each one
    MangledNameToClassDefsMap::const_iterator it = parents.begin();
    for(; it != parents.end(); ++it)
    {
        const ClassDefSet& directParents = it->second;
        ClassDefSet& currentAncestors = transitiveParents[it->first];

        //Use DFS to find all ancestors
        vector<SgClassDefinition*> worklist;
        worklist.insert(worklist.end(), directParents.begin(), directParents.end());
        
        while (!worklist.empty())
        {
            SgClassDefinition* ancestor = worklist.back();
            worklist.pop_back();
            currentAncestors.insert(ancestor);
            
            MangledNameToClassDefsMap::const_iterator deeperAncestors = 
                    parents.find(ancestor->get_declaration()->get_mangled_name());
            if (deeperAncestors != parents.end())
            {
                worklist.insert(worklist.end(), deeperAncestors->second.begin(), deeperAncestors->second.end());
            }
        }
    }
}
