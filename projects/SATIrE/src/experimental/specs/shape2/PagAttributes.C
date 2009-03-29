#include "ShapeAnalyzerOptions.h"

extern ShapeAnalyzerOptions *opt;

// remove temporary variables introduced in the normalisation process
PAG_BOOL get_universal_attribute__option_kill_normalisation_temps() {
    return true;
}

// remove empty graphs from the set of graphs
PAG_BOOL get_universal_attribute__option_kill_empty_graph() {
    return false;
}

// remove nodes that are not reachable
PAG_BOOL get_universal_attribute__option_kill_unreachable_nodes() {
    return false;
}

