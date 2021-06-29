// An example ROSE plugin

#ifndef CLASS_HIERARCHY_WRITER_H
#define CLASS_HIERARCHY_WRITER_H 1

#include <ostream>

#include "ClassHierarchyAnalysis.h"


namespace CodeThorn
{

/// filter type for class hierarchy writer
///   returns true, if a class should be included in the graph
using ClassFilterFn = std::function<bool (ClassKeyType)>;

/// writes out a class analysis graph for all classes and casts
void writeClassDotFile( std::ostream& os,
                        ClassNameFn& nameOf,
                        ClassFilterFn include,
                        const ClassAnalysis& classes,
                        const CastAnalysis& casts
                      );

/// writes out the virtual functions relationships as text
void writeVirtualFunctions( std::ostream& os,
                            ClassNameFn& className,
                            FuncNameFn& funcName,
                            ClassFilterFn include,
                            const ClassAnalysis& classes,
                            const VirtualFunctionAnalysis& vfuns,
                            bool withOverridden = false
                          );


}
#endif /* CLASS_HIERARCHY_WRITER_H */
