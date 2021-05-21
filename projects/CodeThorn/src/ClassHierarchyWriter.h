// An example ROSE plugin

#ifndef CLASS_HIERARCHY_WRITER_H
#define CLASS_HIERARCHY_WRITER_H 1

#include <ostream>

#include "ClassHierarchyAnalysis.h"


namespace CodeThorn
{

/// filter type for class hierarchy writer
///   returns true, if a class should be included in the graph
using FilterFn = std::function<bool (ClassKeyType)>;

/// returns the name for a class key
using NameFn   = std::function<std::string(ClassKeyType)>;

/// writes out a class analysis graph for all classes and casts
void write( std::ostream& os,
            NameFn nameOf,
            FilterFn include,
            const ClassAnalysis& classes,
            const CastAnalysis& casts
          );

}
#endif /* CLASS_HIERARCHY_WRITER_H */
