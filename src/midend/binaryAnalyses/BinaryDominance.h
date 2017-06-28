#ifndef ROSE_BinaryAnalysis_Dominance_H
#define ROSE_BinaryAnalysis_Dominance_H

namespace Rose {
namespace BinaryAnalysis {

// This dominator API, which was based on the Boost Graph Library (BGL) has been removed because we're gradually transitioning
// all types of graphs to the Sawyer Graph API which is easier for non-expert C++ programmers (it uses templates more like the
// STL), is faster than BGL, and follows the ROSE naming scheme.
//
// To find the dominators for all vertices in any Sawyer graph, call @c graphDominators, defined in the
// @c Sawyer::Container::Algorithm namespace.
//
// Documentation for Sawyer can be find in the official ROSE documentation, or here: https://hoosierfocus.com/sawyer

}
}

#endif
