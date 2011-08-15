#ifndef CLASS_HIERARCHY_GRAPH_H
#define CLASS_HIERARCHY_GRAPH_H

#include <vector>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

class ClassHierarchyWrapper
{
public:
    typedef boost::unordered_set<SgClassDefinition*> ClassDefSet;
        
private:
    typedef boost::unordered_map<SgClassDefinition*,  ClassDefSet> ClassDefToClassDefsMap;
    
    /** Map from each class to all its immediate superclasses. */
    ClassDefToClassDefsMap directParents;
    
    /** Map from each class to all its immediate subclasses. */
    ClassDefToClassDefsMap directChildren;
    
    /** Map from class to all (strict) ancestors. */
    ClassDefToClassDefsMap ancestorClasses;
    
    /** Map from class to all (strict) subclasses. */
    ClassDefToClassDefsMap subclasses;
    
    SgIncidenceDirectedGraph* classGraph;

public:

    ClassHierarchyWrapper(SgNode *node);
    const ClassDefSet& getSubclasses(SgClassDefinition *) const;
    const ClassDefSet& getDirectSubclasses(SgClassDefinition *) const;
    const ClassDefSet& getDirectAncestorclasses(SgClassDefinition *) const;
    const ClassDefSet& getAncestorClasses(SgClassDefinition *) const;

private:

    /** Computes the transitive closure of the child-parent class relationship.
     * @param parents map from each class to its parents. 
     * @param transitiveParents map from each class to all its ancestors */
    static void buildAncestorsMap(const ClassDefToClassDefsMap& parents, ClassDefToClassDefsMap& transitiveParents);
};


#endif
