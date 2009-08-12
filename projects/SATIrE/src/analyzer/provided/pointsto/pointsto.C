// pointsto.C -- first attempt at a simple, efficient, and powerful
// unification-based flow- and context-insensitive points-to analysis for C.
// 
// Reference: M. Das. Unification-based Pointer Analysis with Directional
// Assignments. PLDI 2000.
// But first, this is just Steensgaard's algorithm.

#include <memory>
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

#if HAVE_PAG
PointsToAnalysis::Location *
PointsToAnalysis::expressionLocation(
        SgExpression *expr, const ContextInformation::Context &context)
{
    return location_representative(p_impl->expressionLocation(expr, context));
}
#endif

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

#if HAVE_PAG
PointsToAnalysis::Location *
PointsToAnalysis::symbol_location(
        SgSymbol *sym, const ContextInformation::Context &context)
{
    return location_representative(p_impl->symbol_location(sym, context));
}

bool
PointsToAnalysis::symbol_has_location(
        SgSymbol *sym, const ContextInformation::Context &context)
{
    return p_impl->symbol_has_location(sym, context);
}
#endif

bool
PointsToAnalysis::mayBeAliased(PointsToAnalysis::Location *loc) const
{
    return p_impl->mayBeAliased(loc);
}

bool
PointsToAnalysis::isArrayLocation(PointsToAnalysis::Location *loc) const
{
    return p_impl->isArrayLocation(loc);
}

const std::list<SgSymbol *> &
PointsToAnalysis::location_symbols(PointsToAnalysis::Location *loc) const
{
    return p_impl->location_symbols(loc);
}

const std::list<SgFunctionSymbol *> &
PointsToAnalysis::location_funcsymbols(PointsToAnalysis::Location *loc) const
{
    return p_impl->location_funcsymbols(loc);
}

unsigned long
PointsToAnalysis::location_id(PointsToAnalysis::Location *loc) const
{
    return p_impl->location_id(loc);
}

PointsToAnalysis::Location *
PointsToAnalysis::location_representative(PointsToAnalysis::Location *loc) const
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
#if VERBOSE_DEBUG
    if (loc != NULL)
    {
        std::cout
            << "loc " << loc->id
            << " dummy: " << (loc->dummy ? "true" : "false")
            << " repr: " << location_representative(loc)->id
            ;
    }
    else
        std::cout << "loc 0!";
#endif
    return (loc != NULL && !loc->dummy);
}

void
PointsToAnalysis::interesting_locations(std::vector<Location *> &locs) const
{
    locs.clear();
    const std::vector<Location *> &locations = get_locations();
    std::vector<Location *>::const_iterator loc;
    for (loc = locations.begin(); loc != locations.end(); ++loc)
    {
        if(p_impl->interesting(*loc))
            locs.push_back(*loc);
    }
}

} // namespace Analyses

} // namespace SATIrE
