// tps : Switching from rose.h to sage3 changed size from 17,7 MB to 7,3MB
#include "sage3basic.h"

#include "CallGraph.h"
#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>

#define foreach BOOST_FOREACH
using namespace std;
using namespace boost;

ClassHierarchyWrapper::ClassHierarchyWrapper(SgNode *node)
   {
     ROSE_ASSERT(isSgProject(node));

     Rose_STL_Container<SgNode*> allCls;
     allCls = NodeQuery::querySubTree(node, V_SgClassDefinition);

  // DQ (10/12/2012): I think we have to exclude class template defintions since template should not be a 
  // part of the class hierarchy (only instantations of templated).  The new IR design has developed
  // SgTemplateClassDefinition (derived from SgClassDefinition), and so template have gotten into the mix 
  // (returned from the AST query).  so we have to filter them out to get a semilar handling for the class 
  // hierarchy graph as before when template were a well represented in the AST.
     for (Rose_STL_Container<SgNode *>::iterator it = allCls.begin(); it != allCls.end(); it++)
        {
          SgTemplateClassDefinition* templateClassDefinition = isSgTemplateClassDefinition(*it);
          if (templateClassDefinition != NULL)
             {
#if 0
               printf ("In ClassHierarchyWrapper(): SgTemplateClassDefinition %p included into class hierarchy analysis (mistake) \n",templateClassDefinition);
               SgClassDeclaration* classDeclaration = templateClassDefinition->get_declaration();
               classDeclaration->get_file_info()->display("In ClassHierarchyWrapper(): SgTemplateClassDefinition included into class hierarchy analysis (mistake): debug");
#endif
            // Set to NULL so that we can remove all NULL values in the next step.
               *it = NULL;
             }
       // ROSE_ASSERT(templateClassDefinition == NULL);
        }

#if 0
     size_t vectorSizeBeforeRemovingTemplates = allCls.size();
#endif
  // printf ("In ClassHierarchyWrapper(): Size before removing SgTemplateClassDefinition = %" PRIuPTR " \n",vectorSizeBeforeRemovingTemplates);
     allCls.erase(std::remove(allCls.begin(), allCls.end(), (SgNode*)NULL), allCls.end());
  // size_t vectorSizeAfterRemovingTemplates = allCls.size();
  // printf ("In ClassHierarchyWrapper(): Size after removing SgTemplateClassDefinition = %" PRIuPTR " \n",vectorSizeAfterRemovingTemplates);
#if 0
     printf ("In ClassHierarchyWrapper(): Modified size of vector of class definitions before = %" PRIuPTR " after = %" PRIuPTR " \n",vectorSizeBeforeRemovingTemplates,vectorSizeAfterRemovingTemplates);
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

  // build the class hierarchy
  // start by iterating through all the classes
     for (Rose_STL_Container<SgNode *>::iterator it = allCls.begin(); it != allCls.end(); it++)
        {
          SgClassDefinition *clsDescDef = isSgClassDefinition(*it);
          SgBaseClassPtrList & baseClses = clsDescDef->get_inheritances();

          ClassDefSet & classParents = directParents[clsDescDef->get_declaration()->get_mangled_name().getString()];

       // for each iterate through their parents and add parent - child relationship to the graph
          for (SgBaseClassPtrList::iterator it = baseClses.begin(); it != baseClses.end(); it++)
             {
            // AS (032806) Added fix to get the defining class declaration
               SgClassDeclaration *baseCls = isSgClassDeclaration((*it)->get_base_class()->get_definingDeclaration());
               ROSE_ASSERT(baseCls != NULL);
               SgClassDefinition *baseClsDef = baseCls->get_definition();
               ROSE_ASSERT(baseClsDef != NULL);

               classParents.insert(baseClsDef);
               directChildren[baseCls->get_mangled_name().getString()].insert(clsDescDef);
             }
        }

  // Now populate the ancestor/all subclasses maps
     buildAncestorsMap(directParents, ancestorClasses);
     buildAncestorsMap(directChildren, subclasses);
   }


const ClassHierarchyWrapper::ClassDefSet& ClassHierarchyWrapper::getSubclasses(SgClassDefinition *cls) const
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

const ClassHierarchyWrapper::ClassDefSet& ClassHierarchyWrapper::getAncestorClasses(SgClassDefinition *cls) const
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

void findParents(const string& classMangledName,
        const ClassHierarchyWrapper::MangledNameToClassDefsMap& parents,
        ClassHierarchyWrapper::MangledNameToClassDefsMap& transitiveParents,
        set<SgClassDefinition*>& processed)
{
    ClassHierarchyWrapper::MangledNameToClassDefsMap::const_iterator currentParents = parents.find(classMangledName);

    if (currentParents == parents.end())
    {
        ROSE_ASSERT(transitiveParents.find(classMangledName) == transitiveParents.end());
        return;
    }

    ClassHierarchyWrapper::ClassDefSet& currentTransitiveParents = transitiveParents[classMangledName];

    //Our transitive parents are simply the union of our parents' transitive parents
    foreach(SgClassDefinition* parent, currentParents->second)
    {
        std::string parentName = parent->get_declaration()->get_mangled_name();

        if (processed.find(parent) == processed.end())
        {
            findParents(parentName, parents, transitiveParents, processed);
            processed.insert(parent);
        }

        ClassHierarchyWrapper::MangledNameToClassDefsMap::const_iterator grandparents = transitiveParents.find(parentName);
        if (grandparents != transitiveParents.end())
        {
            currentTransitiveParents.insert(grandparents->second.begin(), grandparents->second.end());
        }

        currentTransitiveParents.insert(parent);
    }
}

void ClassHierarchyWrapper::buildAncestorsMap(const MangledNameToClassDefsMap& parents, MangledNameToClassDefsMap& transitiveParents)
{
    transitiveParents.clear();

    //Iterate over all the classes and calculate the transitive parents for each one
    set<SgClassDefinition*> processedNodes;
    MangledNameToClassDefsMap::const_iterator it = parents.begin();
    for (; it != parents.end(); ++it)
    {
        findParents(it->first, parents, transitiveParents, processedNodes);
    }
}
