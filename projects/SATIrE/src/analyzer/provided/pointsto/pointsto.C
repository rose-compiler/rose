// pointsto.C -- first attempt at a simple, efficient, and powerful
// unification-based flow- and context-insensitive points-to analysis for C.
// 
// Reference: M. Das. Unification-based Pointer Analysis with Directional
// Assignments. PLDI 2000.
// But first, this is just Steensgaard's algorithm.

// Immediate TODOs:
// - Implement a "may be aliased" check for variables; by setting a flag
//   whenever a node becomes the target of another node? Does that work
//   correctly with unification? (For the latter, we could always or the two
//   unificands' flags.)
// - Implement a function to find the location referenced by some
//   expression. In the first version, it should suffice to handle variable
//   references, dereferences (*, [], ->), and struct accesses (->, .). Make
//   the function complain if some expression is not as expected.
// - Implement a usable "dereference" function.

#include <boost/pending/disjoint_sets.hpp>
// Relevant functions provided by Boost's disjoint sets data structure:
// make_set, find_set, union_set, link; link is on class representatives,
// union_set on any members of the respective classes

#include "pointsto.h"
#include "pointsto_implementation.h"

// Use these macros to control how much debug output is printed.
#define DEBUG 1
// #define VERBOSE_DEBUG 1

namespace SATIrE {

namespace Analyses {

std::string
PointsToAnalysis::identifier() const
{
    return "pointsto";
}

std::string
PointsToAnalysis::description() const
{
    return "A flow-insensitive, context-insensitive points-to analysis "
           "in the spirit of Steensgaard.";
}

void
PointsToAnalysis::processResults(Program *)
{
    doDot("pointsto");
}

PointsToAnalysis::PointsToAnalysis(bool contextSensitive)
  : p_impl(new Implementation(contextSensitive))
{
}

PointsToAnalysis::~PointsToAnalysis()
{
}

// ========== implementation forwarding methods ==========

void
PointsToAnalysis::run(Program *program)
{
    TimingPerformance timer("Steensgaard-style points-to analysis:");
    p_impl->run(program);
}

void
PointsToAnalysis::run(CFG *icfg)
{
    TimingPerformance timer("Steensgaard-style points-to analysis on ICFG:");
    p_impl->run(icfg);
}

void
PointsToAnalysis::print()
{
    p_impl->print();
}

void
PointsToAnalysis::doDot(std::string filename)
{
    p_impl->doDot(filename);
}

PointsToAnalysis::Location *
PointsToAnalysis::expressionLocation(SgExpression *expr)
{
    return location_representative(p_impl->expressionLocation(expr));
}

std::string
PointsToAnalysis::locationAttributeString(PointsToAnalysis::Location *location)
{
    return p_impl->locationAttributeString(location);
}

const PointsToAnalysis::CallGraph &
PointsToAnalysis::getCallGraph()
{
    return p_impl->getCallGraph();
}

const std::vector<PointsToAnalysis::Location *> &
PointsToAnalysis::get_locations() const
{
    return p_impl->get_locations();
}

PointsToAnalysis::Location *
PointsToAnalysis::symbol_location(SgSymbol *sym)
{
    return location_representative(p_impl->symbol_location(sym));
}

bool
PointsToAnalysis::mayBeAliased(PointsToAnalysis::Location *loc) const
{
    return p_impl->mayBeAliased(loc);
}

const std::list<SgSymbol *> &
PointsToAnalysis::location_symbols(PointsToAnalysis::Location *loc) const
{
    return p_impl->location_symbols(loc);
}

unsigned long
PointsToAnalysis::location_id(PointsToAnalysis::Location *loc) const
{
    return p_impl->location_id(loc);
}

PointsToAnalysis::Location *
PointsToAnalysis::location_representative(PointsToAnalysis::Location *loc)
{
    return p_impl->location_representative(loc);
}

PointsToAnalysis::Location *
PointsToAnalysis::base_location(PointsToAnalysis::Location *loc)
{
    return location_representative(loc->baseLocation());
}

bool
PointsToAnalysis::valid_location(Location *loc) const
{
    return (loc != NULL && !loc->dummy);
}

} // namespace Analyses

} // namespace SATIrE
