#ifndef CLASS_HIERARCHY_GRAPH_H
#define CLASS_HIERARCHY_GRAPH_H

#include <vector>
#include <map>
#include <boost/unordered_set.hpp>

class ROSE_DLL_API ClassHierarchyWrapper
{
public:
    typedef boost::unordered_set<SgClassDefinition*> ClassDefSet;

    typedef boost::unordered_map<std::string, ClassDefSet> MangledNameToClassDefsMap;

private:


    /** Map from each class to all its immediate superclasses. */
    MangledNameToClassDefsMap directParents;

    /** Map from each class to all its immediate subclasses. */
    MangledNameToClassDefsMap directChildren;

    /** Map from class to all (strict) ancestors. */
    MangledNameToClassDefsMap ancestorClasses;

    /** Map from class to all (strict) subclasses. */
    MangledNameToClassDefsMap subclasses;

    SgIncidenceDirectedGraph* classGraph;

public:
    explicit
    ClassHierarchyWrapper(SgNode* node);

    const ClassDefSet& getSubclasses(SgClassDefinition *) const;
    const ClassDefSet& getDirectSubclasses(SgClassDefinition *) const;
    const ClassDefSet& getAncestorClasses(SgClassDefinition *) const;

private:

    /** Computes the transitive closure of the child-parent class relationship.
     * @param parents map from each class to its parents.
     * @param transitiveParents map from each class to all its ancestors */
    static void buildAncestorsMap(const MangledNameToClassDefsMap& parents, MangledNameToClassDefsMap& transitiveParents);
};


#endif
