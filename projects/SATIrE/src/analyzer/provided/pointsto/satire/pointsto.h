// pointsto.h -- first attempt at a simple, efficient, and powerful
// unification-based flow- and context-insensitive points-to analysis for C.

#ifndef H_POINTSTO
#define H_POINTSTO

#include "cfg_support.h"
#include <satire.h>

namespace SATIrE {

namespace Analyses {

class PointsToAnalysis: public Analysis
{
public:
 // Implementation of the SATIrE::Analysis interface.
    virtual std::string identifier() const;
    virtual std::string description() const;

    virtual void run(Program *program);
    virtual void run(CFG *icfg);

    virtual void processResults(Program *program);

 // Dump a representation of the shape data structure to std::cout.
    void print();
 // Dump a dot representation of the shape data structure to a file with the
 // given name, and invoke dot on it.
    void doDot(std::string filename);

 // Opaque type representing the "abstract location" of an expression. This
 // "abstract location" is a representation of a memory region; expressions
 // referring to the same "location", i.e., region, *may* be aliased.
 // Expressions referring to distinct locations are definitely not aliased.
    struct Location;

 // This type encapsualtes the "points-to information", i.e., the analysis
 // results. Not sure right now whether this should be public or not; if its
 // interface is not public, it might not make too much sense.
    class PointsToInformation;
    PointsToInformation *getPointsToInformation() const;

 // This function returns an expression's location. For now, the expression
 // may be composed of dereference and member access operations and may use
 // variables (and constants as array subscripts). Two expressions may be
 // aliased iff this function returns the same pointer for both.
    Location *expressionLocation(SgExpression *expr);
 // Compute some unique string from the given location. There is a one-to-one
 // correspondence between valid Location pointers and these strings, so again
 // two expressions may be aliased iff the locationAttributeStrings for their
 // locations compare equal.
    std::string locationAttributeString(Location *location);

 // The points-to analysis computes a call graph of the program as it runs.
 // This type, and the associated function, give access to this call graph.
 // The graph is represented by two data structures: The
 // CallSiteExpressionMap maps each function (represented by its symbol) to
 // the list of function expressions at all call sites inside the function.
 // The list is in an order which does not necessarily agree with the the
 // lexical order of function expressions in the source file.
 // The CallCandidateMap maps each of the expressions from the first map to
 // the set of functions that may be referenced by that expression, based on
 // points-to information.
 // Thus, by composing the maps in the obvious way, you can get a mapping
 // from each function to every function it may call.
    struct CallGraph
    {
        typedef std::map<SgFunctionSymbol *, std::list<SgExpression *> >
            CallSiteExpressionMap;
        typedef std::map<SgExpression *, std::set<SgFunctionSymbol *> >
            CallCandidateMap;

        CallSiteExpressionMap callSiteExpressionMap;
        CallCandidateMap callCandidateMap;
    };
 // This function returns a reference to the internal call graph. Users
 // might want to make a copy, as this changes if the analysis is run again,
 // and is lost when the analyzer object is destructed.
    const CallGraph &getCallGraph();

    const std::vector<Location *> &get_locations() const;
    Location *symbol_location(SgSymbol *sym);
    bool mayBeAliased(Location *loc) const;
    const std::list<SgSymbol *> &location_symbols(Location *loc) const;
    unsigned long location_id(Location *loc) const;
    Location *location_representative(Location *loc);
    Location *base_location(Location *loc);
    bool valid_location(Location *loc) const;

    PointsToAnalysis();
    ~PointsToAnalysis();

private:
 // Forbid copying.
    PointsToAnalysis(const PointsToAnalysis &);
    PointsToAnalysis &operator=(const PointsToAnalysis &);

 // The "pimpl" idiom ensures that clients of this class need not be
 // recompiled if internal details are changed.
    struct Implementation;
    const std::auto_ptr<Implementation> p_impl;
};

} // namespace Analyses

} // namespace SATIrE

#endif
