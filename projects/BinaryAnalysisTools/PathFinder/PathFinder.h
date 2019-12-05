#ifndef PathFinder_PathFinder_H
#define PathFinder_PathFinder_H

#include <Diagnostics.h>
#include <Sawyer/Map.h>

namespace PathFinder {

enum PathSelection { NO_PATHS, FEASIBLE_PATHS, ALL_PATHS };
enum SearchMode { SEARCH_SINGLE_DFS, SEARCH_SINGLE_BFS, SEARCH_MULTI };

// Settings from the command-line
struct Settings {
    std::string beginVertex;                            // address or function name where paths should begin
    std::vector<std::string> endVertices;               // addresses or function names where paths should end
    std::vector<std::string> avoidVertices;             // vertices to avoid in any path
    std::vector<std::string> avoidEdges;                // edges to avoid in any path (even number of vertex addresses)
    std::vector<rose_addr_t> summarizeFunctions;        // functions to summarize
    size_t maxRecursionDepth;                           // max recursion depth when expanding function calls
    size_t maxCallDepth;                                // max function call depth when expanding function calls
    size_t maxPathLength;                               // maximum length of any path considered
    size_t vertexVisitLimit;                            // max times a vertex can appear in a path
    bool showInstructions;                              // show instructions in paths?
    bool showConstraints;                               // show path constraints?
    bool showFinalState;                                // show final machine state for each feasible path?
    bool showFunctionSubgraphs;                         // show functions in GraphViz output?
    std::string graphVizPrefix;                         // prefix for GraphViz file names
    PathSelection graphVizOutput;                       // which paths to dump to GraphViz files
    size_t maxPaths;                                    // max number of paths to find (0==unlimited)
    SearchMode searchMode;                              // path finding mode
    size_t maxExprDepth;                                // max depth when printing expressions
    bool showExprType;                                  // show expression data types?
    Sawyer::Optional<rose_addr_t> initialStackPtr;      // concrete value to use for stack pointer register initial value
    size_t nThreads;                                    // number of threads for algorithms that support multi-threading
    std::vector<std::string> postConditionsStr;         // final conditions for registers and memory
    Settings()
        : beginVertex("_start"), maxRecursionDepth(4), maxCallDepth(100), maxPathLength(1600), vertexVisitLimit(1),
          showInstructions(true), showConstraints(false), showFinalState(false), showFunctionSubgraphs(true),
          graphVizPrefix("path-"), graphVizOutput(NO_PATHS), maxPaths(1), searchMode(SEARCH_SINGLE_BFS), maxExprDepth(4),
          showExprType(false), nThreads(1) {}
};

extern Settings settings;

// Maps symbolic variable names to comments, e.g., "v801" => "eax read at path position #6 by 0x08040000 cmp eax, [esp]"
typedef Sawyer::Container::Map<std::string /*name*/, std::string /*comment*/> VarComments;

// Tool diagnostics
extern Rose::Diagnostics::Facility mlog;

} // namespace

#endif
