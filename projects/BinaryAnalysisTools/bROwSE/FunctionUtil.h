#ifndef bROwSE_FunctionUtil_H
#define bROwSE_FunctionUtil_H

#include <bROwSE/bROwSE.h>

namespace bROwSE {

enum MayReturn { MAYRETURN_YES, MAYRETURN_NO, MAYRETURN_UNKNOWN };

struct Box { int x, y, dx, dy; };

typedef Sawyer::Container::Map<rose_addr_t, Box> CfgVertexCoords;

size_t functionNBytes(const P2::Partitioner&, const P2::Function::Ptr&);
size_t functionNInsns(const P2::Partitioner&, const P2::Function::Ptr&);
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

    /** Natural width for the column. */
    virtual Wt::WLength naturalWidth() const {
        return Wt::WLength(5, Wt::WLength::FontEm);
    }

    /** Return data for the table cell. */
    virtual boost::any data(P2::Partitioner&, const P2::Function::Ptr&) const = 0;

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
        return Wt::WString("Entry");
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
        return Wt::WString("Name");
    }
    Wt::WLength naturalWidth() const ROSE_OVERRIDE {
        return Wt::WLength(25, Wt::WLength::FontEm);
    }
    boost::any data(P2::Partitioner&, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        return Wt::WString(StringUtility::cEscape(f->name()));
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
        return Wt::WString("Bytes");
    }
    boost::any data(P2::Partitioner &p, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        return functionNBytes(p, f);
    }
    bool isAscending(const P2::Function::Ptr &a, const P2::Function::Ptr &b) const ROSE_OVERRIDE {
        return a->attr<size_t>(ATTR_NBYTES).orElse(0) < b->attr<size_t>(ATTR_NBYTES).orElse(0);
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
        return Wt::WString("Insns");
    }
    boost::any data(P2::Partitioner &p, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        return functionNInsns(p, f);
    }
    bool isAscending(const P2::Function::Ptr &a, const P2::Function::Ptr &b) const ROSE_OVERRIDE {
        return a->attr<size_t>(ATTR_NINSNS).orElse(0) < b->attr<size_t>(ATTR_NINSNS).orElse(0);
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
        return Wt::WString("BBlocks");
    }
    boost::any data(P2::Partitioner &p, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        return f->basicBlockAddresses().size();
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
        return Wt::WString("DBlocks");
    }
    boost::any data(P2::Partitioner &p, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        return f->dataBlocks().size();
    }
    bool isAscending(const P2::Function::Ptr &a, const P2::Function::Ptr &b) const ROSE_OVERRIDE {
        return a->dataBlocks().size() < b->dataBlocks().size();
    }
    double heatValue(P2::Partitioner &p, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        return f->dataBlocks().size();
    }
};

/** Whether function is marked as an import. */
class FunctionImported: public FunctionAnalyzer {
public:
    static Ptr instance() {
        return Ptr(new FunctionImported);
    }
    Wt::WString header() const ROSE_OVERRIDE {
        return Wt::WString("Imported");
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
        return Wt::WString("Exported");
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
        return Wt::WString("NCallers");
    }
    boost::any data(P2::Partitioner &p, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        return functionNCallers(p, f);
    }
    bool isAscending(const P2::Function::Ptr &a, const P2::Function::Ptr &b) const ROSE_OVERRIDE {
        return a->attr<size_t>(ATTR_NCALLERS).orElse(0) < b->attr<size_t>(ATTR_NCALLERS).orElse(0);
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
        return Wt::WString("NRet");
    }
    boost::any data(P2::Partitioner &p, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        return functionNReturns(p, f);
    }
    bool isAscending(const P2::Function::Ptr &a, const P2::Function::Ptr &b) const ROSE_OVERRIDE {
        return a->attr<size_t>(ATTR_NRETURNS).orElse(0) < b->attr<size_t>(ATTR_NRETURNS).orElse(0);
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
        return Wt::WString("MayReturn");
    }
    boost::any data(P2::Partitioner &p, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        switch (functionMayReturn(p, f)) {
            case MAYRETURN_YES:     return Wt::WString("yes");
            case MAYRETURN_NO:      return Wt::WString("no");
            case MAYRETURN_UNKNOWN: return Wt::WString("");
        }
        ASSERT_not_reachable("invalid may-return value");
    }
    bool isAscending(const P2::Function::Ptr &a, const P2::Function::Ptr &b) const ROSE_OVERRIDE {
        return (a->attr<MayReturn>(ATTR_MAYRETURN).orElse(MAYRETURN_UNKNOWN) <
                b->attr<MayReturn>(ATTR_MAYRETURN).orElse(MAYRETURN_UNKNOWN));
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
        return Wt::WString("SDelta");
    }
    boost::any data(P2::Partitioner &p, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        int64_t delta = functionStackDelta(p, f);
        if (delta != SgAsmInstruction::INVALID_STACK_DELTA)
            return Wt::WString(boost::lexical_cast<std::string>(delta));
        if (functionMayReturn(p, f) == MAYRETURN_NO)
            return Wt::WString("N.A.");
        return Wt::WString("");
    }
    bool isAscending(const P2::Function::Ptr &a, const P2::Function::Ptr &b) const ROSE_OVERRIDE {
        return (a->attr<int64_t>(ATTR_STACKDELTA).orElse(SgAsmInstruction::INVALID_STACK_DELTA) <
                b->attr<int64_t>(ATTR_STACKDELTA).orElse(SgAsmInstruction::INVALID_STACK_DELTA));
    }
    double heatValue(P2::Partitioner &p, const P2::Function::Ptr &f) const ROSE_OVERRIDE {
        int64_t delta = functionStackDelta(p, f);
        return SgAsmInstruction::INVALID_STACK_DELTA==delta ? NAN : (double)delta;
    }
};

} // namespace
#endif
