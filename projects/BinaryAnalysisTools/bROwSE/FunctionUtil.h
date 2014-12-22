#ifndef bROwSE_FunctionUtil_H
#define bROwSE_FunctionUtil_H

#include <bROwSE/bROwSE.h>

namespace bROwSE {

enum MayReturn { MAYRETURN_YES, MAYRETURN_NO, MAYRETURN_UNKNOWN };

struct Box { int x, y, dx, dy; };

typedef Sawyer::Container::Map<rose_addr_t, Box> CfgVertexCoords;

size_t functionNBytes(const P2::Partitioner&, const P2::Function::Ptr&);
size_t functionNInsns(const P2::Partitioner&, const P2::Function::Ptr&);
size_t functionNIntervals(const P2::Partitioner&, const P2::Function::Ptr&);
size_t functionNDiscontiguousBlocks(const P2::Partitioner&, const P2::Function::Ptr&);
boost::filesystem::path functionCfgGraphvizFile(const P2::Partitioner&, const P2::Function::Ptr&);
boost::filesystem::path functionCfgImage(const P2::Partitioner&, const P2::Function::Ptr&);
CfgVertexCoords functionCfgVertexCoords(const P2::Partitioner&, const P2::Function::Ptr&);
P2::FunctionCallGraph* functionCallGraph(P2::Partitioner&);
size_t functionNCallers(P2::Partitioner&, const P2::Function::Ptr&);
size_t functionNReturns(P2::Partitioner&, const P2::Function::Ptr&);
MayReturn functionMayReturn(P2::Partitioner&, const P2::Function::Ptr&);
int64_t functionStackDelta(P2::Partitioner&, const P2::Function::Ptr&);
SgAsmFunction *functionAst(P2::Partitioner&, const P2::Function::Ptr&);

/** Interface for computing some property of a function. */
class FunctionAnalyzer: public Sawyer::SharedObject {
public:
    typedef Sawyer::SharedPointer<FunctionAnalyzer> Ptr;

protected:
    // Use subclass "instance" methods instead.
    FunctionAnalyzer() {}

public:
    /** Short name for a property.  This is the name that will appear as the table header and should be capitalized and very
     * short. */
    virtual Wt::WString header() const = 0;

    /** Longer name for a property.  This name is suitable for tables where the heading is on the left rather than the top. */
    virtual Wt::WString name() const {
        return header();
    }

    /** Full description for a property. */
    virtual Wt::WString toolTip() const {
        return "";
    }

    /** Natural width for the column. */
    virtual Wt::WLength naturalWidth() const {
        return Wt::WLength(5, Wt::WLength::FontEm);
    }

    /** Return data for the table cell. */
    virtual boost::any data(P2::Partitioner&, const P2::Function::Ptr&) const = 0;

    /** Return data as a formatted string. */
    virtual Wt::WString toString(const boost::any &value) const {
        return boost::any_cast<Wt::WString>(value);
    }

    /** Sorts two analysis results in ascending order. Should return true if and only if @p a is less than @p b. Since some of
     *  the analyses are expensive to compute, it is customary to cache the result as an attribute of the function. In fact,
     *  caching is required for any analysis that requires more than just a function pointer because when sorting a table
     *  column only the function pointers are available (thus the two arguments for the @ref isDescending method). */
    virtual bool isAscending(const P2::Function::Ptr &a, const P2::Function::Ptr &b) const = 0;

    /** Sorts two analysis results in descending order. Should return true if and only if @p a is greater than @p b. */
    virtual bool isDescending(const P2::Function::Ptr &a, const P2::Function::Ptr &b) const {
        return !isAscending(a, b) && isAscending(b, a);
    }

    /** Return a double result or NaN. This is the result that's used when computing a heat map. */
    virtual double heatValue(P2::Partitioner&, const P2::Function::Ptr&) const {
        return 0;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Various function analyses

/** Address where the function is entered when called. */
class FunctionEntryAddress: public FunctionAnalyzer {
public:
    static Ptr instance() {
        return Ptr(new FunctionEntryAddress);
    }
    Wt::WString header() const ROSE_OVERRIDE {
        return "Entry";
    }
    Wt::WString name() const ROSE_OVERRIDE {
        return "Entry virtual address";
    }
    Wt::WString toolTip() const ROSE_OVERRIDE {
        return "This is the address by which the function is entered when called.";
    }
    Wt::WLength naturalWidth() const ROSE_OVERRIDE {
        return Wt::WLength(6, Wt::WLength::FontEm);
    }
    boost::any data(P2::Partitioner&, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        return Wt::WString(StringUtility::addrToString(f->address()));
    }
    bool isAscending(const P2::Function::Ptr &a, const P2::Function::Ptr &b) const ROSE_OVERRIDE {
        return a->address() < b->address();
    }
};

/** Name of function escaped as a C string. */
class FunctionName: public FunctionAnalyzer {
public:
    static Ptr instance() {
        return Ptr(new FunctionName);
    }
    Wt::WString header() const ROSE_OVERRIDE {
        return "Name";
    }
    Wt::WString toolTip() const ROSE_OVERRIDE {
        return "Function name if known, escaped as a C string.";
    }
    Wt::WLength naturalWidth() const ROSE_OVERRIDE {
        return Wt::WLength(25, Wt::WLength::FontEm);
    }
    boost::any data(P2::Partitioner&, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        std::string name = f->name();
        if (!name.empty() && (!isgraph(name[0]) || !isgraph(name[name.size()-1]))) {
            // Add double quotes to the string after we escape it, otherwise the user can't tell that the name
            // has leading or trailing non-printable characters.
            name = "\"" + StringUtility::cEscape(name) + "\"";
        } else {
            name = StringUtility::cEscape(name);
        }
        return Wt::WString(name);
    }
    bool isAscending(const P2::Function::Ptr &a, const P2::Function::Ptr &b) const ROSE_OVERRIDE {
        return a->name() < b->name();
    }
    double heatValue(P2::Partitioner&, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        return f->name().empty() ? NAN : 0.0;
    }
};

/** Size of function in bytes. */
class FunctionSizeBytes: public FunctionAnalyzer {
public:
    static Ptr instance() {
        return Ptr(new FunctionSizeBytes);
    }
    Wt::WString header() const ROSE_OVERRIDE {
        return "Bytes";
    }
    Wt::WString name() const ROSE_OVERRIDE {
        return "Number of bytes";
    }
    Wt::WString toolTip() const ROSE_OVERRIDE {
        return ("Total size of function in bytes including code and data. Overlapping instructions and/or data "
                "is counted only once.");
    }
    boost::any data(P2::Partitioner &p, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        return functionNBytes(p, f);
    }
    Wt::WString toString(const boost::any &data) const ROSE_OVERRIDE {
        return boost::lexical_cast<std::string>(boost::any_cast<size_t>(data));
    }
    bool isAscending(const P2::Function::Ptr &a, const P2::Function::Ptr &b) const ROSE_OVERRIDE {
        return a->attr<size_t>(ATTR_NBytes).orElse(0) < b->attr<size_t>(ATTR_NBytes).orElse(0);
    }
    double heatValue(P2::Partitioner &p, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        return functionNBytes(p, f);
    }
};

/** Size of function in instructions. */
class FunctionSizeInsns: public FunctionAnalyzer {
public:
    static Ptr instance() {
        return Ptr(new FunctionSizeInsns);
    }
    Wt::WString header() const ROSE_OVERRIDE {
        return "Insns";
    }
    Wt::WString name() const ROSE_OVERRIDE {
        return "Number of instructions";
    }
    Wt::WString toolTip() const ROSE_OVERRIDE {
        return "Total number of instructions belonging to function.";
    }
    boost::any data(P2::Partitioner &p, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        return functionNInsns(p, f);
    }
    Wt::WString toString(const boost::any &data) const ROSE_OVERRIDE {
        return boost::lexical_cast<std::string>(boost::any_cast<size_t>(data));
    }
    bool isAscending(const P2::Function::Ptr &a, const P2::Function::Ptr &b) const ROSE_OVERRIDE {
        return a->attr<size_t>(ATTR_NInsns).orElse(0) < b->attr<size_t>(ATTR_NInsns).orElse(0);
    }
    double heatValue(P2::Partitioner &p, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        return functionNInsns(p, f);
    }
};

/** Size of function in basic blocks. */
class FunctionSizeBBlocks: public FunctionAnalyzer {
public:
    static Ptr instance() {
        return Ptr(new FunctionSizeBBlocks);
    }
    Wt::WString header() const ROSE_OVERRIDE {
        return "BBlocks";
    }
    Wt::WString name() const ROSE_OVERRIDE {
        return "Number of basic blocks";
    }
    Wt::WString toolTip() const ROSE_OVERRIDE {
        return "Total number of basic blocks belonging to function.";
    }
    boost::any data(P2::Partitioner &p, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        return f->basicBlockAddresses().size();
    }
    Wt::WString toString(const boost::any &data) const ROSE_OVERRIDE {
        return boost::lexical_cast<std::string>(boost::any_cast<size_t>(data));
    }
    bool isAscending(const P2::Function::Ptr &a, const P2::Function::Ptr &b) const ROSE_OVERRIDE {
        return a->basicBlockAddresses().size() < b->basicBlockAddresses().size();
    }
    double heatValue(P2::Partitioner &p, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        return f->basicBlockAddresses().size();
    }
};

/** Number of data blocks owned by function. */
class FunctionSizeDBlocks: public FunctionAnalyzer {
public:
    static Ptr instance() {
        return Ptr(new FunctionSizeDBlocks);
    }
    Wt::WString header() const ROSE_OVERRIDE {
        return "DBlocks";
    }
    Wt::WString name() const ROSE_OVERRIDE {
        return "Number of data blocks";
    }
    Wt::WString toolTip() const ROSE_OVERRIDE {
        return "Total number of data blocks belonging to function.";
    }
    boost::any data(P2::Partitioner &p, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        return f->dataBlocks().size();
    }
    Wt::WString toString(const boost::any &data) const ROSE_OVERRIDE {
        return boost::lexical_cast<std::string>(boost::any_cast<size_t>(data));
    }
    bool isAscending(const P2::Function::Ptr &a, const P2::Function::Ptr &b) const ROSE_OVERRIDE {
        return a->dataBlocks().size() < b->dataBlocks().size();
    }
    double heatValue(P2::Partitioner &p, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        return f->dataBlocks().size();
    }
};

/** Number of discontiguous basic blocks in a function. */
class FunctionNDiscontiguousBlocks: public FunctionAnalyzer {
public:
    static Ptr instance() {
        return Ptr(new FunctionNDiscontiguousBlocks);
    }
    Wt::WString header() const ROSE_OVERRIDE {
        return "BBDis";
    }
    Wt::WString name() const ROSE_OVERRIDE {
        return "Discontiguous basic blocks";
    }
    Wt::WString toolTip() const ROSE_OVERRIDE {
        return ("Number of basic blocks that are not contiguous in memory.  This can indicate opaque predicates, "
                "scattered blocks sewed together by unconditional branches.");
    }
    boost::any data(P2::Partitioner &p, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        return functionNDiscontiguousBlocks(p, f);
    }
    Wt::WString toString(const boost::any &data) const ROSE_OVERRIDE {
        return boost::lexical_cast<std::string>(boost::any_cast<size_t>(data));
    }
    bool isAscending(const P2::Function::Ptr &a, const P2::Function::Ptr &b) const ROSE_OVERRIDE {
        return a->attr<size_t>(ATTR_NDiscontiguousBlocks).orElse(0) < b->attr<size_t>(ATTR_NDiscontiguousBlocks).orElse(0);
    }
    double heatValue(P2::Partitioner &p, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        return functionNDiscontiguousBlocks(p, f);
    }
};
    
/** Number of contiguous intervals in a function's code and data addresses. */
class FunctionNIntervals: public FunctionAnalyzer {
public:
    static Ptr instance() {
        return Ptr(new FunctionNIntervals);
    }
    Wt::WString header() const ROSE_OVERRIDE {
        return "Intervals";
    }
    Wt::WString name() const ROSE_OVERRIDE {
        return "Number of intervals";
    }
    Wt::WString toolTip() const ROSE_OVERRIDE {
        return ("Number of discontiguous regions forming the function's code and data. I.e., one more than the "
                "number of gaps in the function's addresses.");

    }
    boost::any data(P2::Partitioner &p, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        return functionNIntervals(p, f);
    }
    Wt::WString toString(const boost::any &data) const ROSE_OVERRIDE {
        return boost::lexical_cast<std::string>(boost::any_cast<size_t>(data));
    }
    bool isAscending(const P2::Function::Ptr &a, const P2::Function::Ptr &b) const ROSE_OVERRIDE {
        return a->attr<size_t>(ATTR_NIntervals).orElse(0) < b->attr<size_t>(ATTR_NIntervals).orElse(0);
    }
    double heatValue(P2::Partitioner &p, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        return functionNIntervals(p, f);
    }
};
    

/** Whether function is marked as an import. */
class FunctionImported: public FunctionAnalyzer {
public:
    static Ptr instance() {
        return Ptr(new FunctionImported);
    }
    Wt::WString header() const ROSE_OVERRIDE {
        return "Imported";
    }
    Wt::WString toolTip() const ROSE_OVERRIDE {
        return "Whether function is mentioned in import tables. Dynamically linked functions are considered imports.";
    }
    boost::any data(P2::Partitioner &p, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        return Wt::WString((f->reasons() & SgAsmFunction::FUNC_IMPORT)!=0 ? "yes" : "no");
    }
    bool isAscending(const P2::Function::Ptr &a, const P2::Function::Ptr &b) const ROSE_OVERRIDE {
        unsigned aa = a->reasons() & SgAsmFunction::FUNC_IMPORT;
        unsigned bb = b->reasons() & SgAsmFunction::FUNC_IMPORT;
        return aa < bb;
    }
    double heatValue(P2::Partitioner&, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        return (f->reasons() & SgAsmFunction::FUNC_IMPORT) ? 1 : 0;
    }
};

/** Whether a function is marked as an export. */
class FunctionExported: public FunctionAnalyzer {
public:
    static Ptr instance() {
        return Ptr(new FunctionExported);
    }
    Wt::WString header() const ROSE_OVERRIDE {
        return "Exported";
    }
    Wt::WString toolTip() const ROSE_OVERRIDE {
        return "Whether function is an exported symbol.";
    }
    boost::any data(P2::Partitioner &p, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        return Wt::WString((f->reasons() & SgAsmFunction::FUNC_EXPORT)!=0 ? "yes" : "no");
    }
    bool isAscending(const P2::Function::Ptr &a, const P2::Function::Ptr &b) const ROSE_OVERRIDE {
        unsigned aa = a->reasons() & SgAsmFunction::FUNC_EXPORT;
        unsigned bb = b->reasons() & SgAsmFunction::FUNC_EXPORT;
        return aa < bb;
    }
    double heatValue(P2::Partitioner &p, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        return (f->reasons() & SgAsmFunction::FUNC_EXPORT) ? 1 : 0;
    }
};

/** Number of calls to a function. This is the total number of calls, not the number of distinct functions that call this. */
class FunctionNCallers: public FunctionAnalyzer {
public:
    static Ptr instance() {
        return Ptr(new FunctionNCallers);
    }
    Wt::WString header() const ROSE_OVERRIDE {
        return "NCallers";
    }
    Wt::WString name() const ROSE_OVERRIDE {
        return "Number of callers";
    }
    Wt::WString toolTip() const ROSE_OVERRIDE {
        return ("Number of sites from which this function is called.");
    }
    boost::any data(P2::Partitioner &p, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        return functionNCallers(p, f);
    }
    Wt::WString toString(const boost::any &data) const ROSE_OVERRIDE {
        return boost::lexical_cast<std::string>(boost::any_cast<size_t>(data));
    }
    bool isAscending(const P2::Function::Ptr &a, const P2::Function::Ptr &b) const ROSE_OVERRIDE {
        return a->attr<size_t>(ATTR_NCallers).orElse(0) < b->attr<size_t>(ATTR_NCallers).orElse(0);
    }
    double heatValue(P2::Partitioner &p, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        return functionNCallers(p, f);
    }
};

/** Number of returning basic blocks.  The number of basic blocks in the function that return to the caller. */
class FunctionNReturns: public FunctionAnalyzer {
public:
    static Ptr instance() {
        return Ptr(new FunctionNReturns);
    }
    Wt::WString header() const ROSE_OVERRIDE {
        return "NRet";
    }
    Wt::WString name() const ROSE_OVERRIDE {
        return "Number of return points";
    }
    Wt::WString toolTip() const ROSE_OVERRIDE {
        return ("Number of points at which the function returns to a caller. Return points are counted even if they "
                "are not reachable according to the control flow graph.");
    }
    boost::any data(P2::Partitioner &p, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        return functionNReturns(p, f);
    }
    Wt::WString toString(const boost::any &data) const ROSE_OVERRIDE {
        return boost::lexical_cast<std::string>(boost::any_cast<size_t>(data));
    }
    bool isAscending(const P2::Function::Ptr &a, const P2::Function::Ptr &b) const ROSE_OVERRIDE {
        return a->attr<size_t>(ATTR_NReturns).orElse(0) < b->attr<size_t>(ATTR_NReturns).orElse(0);
    }
    double heatValue(P2::Partitioner &p, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        return functionNReturns(p, f);
    }
};

/** May-return analysis. Whether it's possible for a function to return to its caller. */
class FunctionMayReturn: public FunctionAnalyzer {
public:
    static Ptr instance() {
        return Ptr(new FunctionMayReturn);
    }
    Wt::WString header() const ROSE_OVERRIDE {
        return "MayReturn";
    }
    Wt::WString name() const ROSE_OVERRIDE {
        return "May return to caller";
    }
    Wt::WString toolTip() const ROSE_OVERRIDE {
        return "Whether it is possible for a function to return to a caller.";
    }
    boost::any data(P2::Partitioner &p, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        switch (functionMayReturn(p, f)) {
            case MAYRETURN_YES:     return Wt::WString("yes");
            case MAYRETURN_NO:      return Wt::WString("no");
            case MAYRETURN_UNKNOWN: return Wt::WString("unknown");
        }
        ASSERT_not_reachable("invalid may-return value");
    }
    bool isAscending(const P2::Function::Ptr &a, const P2::Function::Ptr &b) const ROSE_OVERRIDE {
        return (a->attr<MayReturn>(ATTR_MayReturn).orElse(MAYRETURN_UNKNOWN) <
                b->attr<MayReturn>(ATTR_MayReturn).orElse(MAYRETURN_UNKNOWN));
    }
    double heatValue(P2::Partitioner &p, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        switch (functionMayReturn(p, f)) {
            case MAYRETURN_NO:      return 0;
            case MAYRETURN_YES:     return 1;
            case MAYRETURN_UNKNOWN: return NAN;
        }
        ASSERT_not_reachable("invalid may-return value");
    }
};

/** Stack delta analysis.  The net effect of the function on the stack pointer. */
class FunctionStackDelta: public FunctionAnalyzer {
public:
    static Ptr instance() {
        return Ptr(new FunctionStackDelta);
    }
    Wt::WString header() const ROSE_OVERRIDE {
        return "SDelta";
    }
    Wt::WString name() const ROSE_OVERRIDE {
        return "Stack delta";
    }
    Wt::WString toolTip() const ROSE_OVERRIDE {
        return ("Net effect on the stack pointer, including popping the return address if that is part of the call-"
                "return convention.");
    }
    boost::any data(P2::Partitioner &p, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        int64_t delta = functionStackDelta(p, f);
        if (delta != SgAsmInstruction::INVALID_STACK_DELTA)
            return Wt::WString(boost::lexical_cast<std::string>(delta));
        if (functionMayReturn(p, f) == MAYRETURN_NO)
            return Wt::WString("inapt");
        return Wt::WString("unknown");
    }
    bool isAscending(const P2::Function::Ptr &a, const P2::Function::Ptr &b) const ROSE_OVERRIDE {
        // order: inapt, unknown, concrete.
        int64_t aDelta = a->attr<int64_t>(ATTR_StackDelta).orElse(SgAsmInstruction::INVALID_STACK_DELTA);
        int64_t bDelta = b->attr<int64_t>(ATTR_StackDelta).orElse(SgAsmInstruction::INVALID_STACK_DELTA);
        if (aDelta == SgAsmInstruction::INVALID_STACK_DELTA) {
            if (aDelta == bDelta) {
                // ATTR_MayReturn should exist since the stack delta is invalid.  See data() method.
                MayReturn aMayRet = a->attr<MayReturn>(ATTR_MayReturn).orElse(MAYRETURN_UNKNOWN);
                MayReturn bMayRet = b->attr<MayReturn>(ATTR_MayReturn).orElse(MAYRETURN_UNKNOWN);
                return aMayRet==MAYRETURN_NO && bMayRet!=MAYRETURN_NO;
            } else {
                return true;                            // b is concrete
            }
        } else if (bDelta == SgAsmInstruction::INVALID_STACK_DELTA) {
            return false;                               // a is concrete
        } else {
            return aDelta < bDelta;                     // a and b are concrete
        }
    }
    double heatValue(P2::Partitioner &p, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        int64_t delta = functionStackDelta(p, f);
        return SgAsmInstruction::INVALID_STACK_DELTA==delta ? NAN : (double)delta;
    }
};

} // namespace
#endif
