#include <bROwSE/FunctionListModel.h>
#include <bROwSE/FunctionUtil.h>

namespace bROwSE {

P2::Function::Ptr
FunctionListModel::functionAt(size_t idx) {
    return idx < functions_.size() ? functions_[idx] : P2::Function::Ptr();
}

Wt::WModelIndex
FunctionListModel::functionIdx(const P2::Function::Ptr &function) const {
    for (size_t i=0; i<functions_.size(); ++i) {
        if (functions_[i] == function)
            return index(i, 0);
    }
    return Wt::WModelIndex();
}

int
FunctionListModel::rowCount(const Wt::WModelIndex &parent) const {
    return parent.isValid() ? 0 : functions_.size();
}

int
FunctionListModel::columnCount(const Wt::WModelIndex &parent) const {
    return parent.isValid() ? 0 : C_NCOLS;
}

boost::any
FunctionListModel::headerData(int column, Wt::Orientation orientation, int role) const {
    if (Wt::Horizontal == orientation && Wt::DisplayRole == role) {
        switch (column) {
            case C_ENTRY:      return Wt::WString("Entry");
            case C_NAME:       return Wt::WString("Name");
            case C_SIZE:       return Wt::WString("Size");
            case C_IMPORT:     return Wt::WString("Import");
            case C_EXPORT:     return Wt::WString("Export");
            case C_NCALLERS:   return Wt::WString("NCalls");
            case C_NRETURNS:   return Wt::WString("NReturns");
            case C_MAYRETURN:  return Wt::WString("MayReturn");
            case C_STACKDELTA: return Wt::WString("StackDelta");
            default:
                ASSERT_not_reachable("column not implemented");
        }
    }
    return boost::any();
}

boost::any
FunctionListModel::data(const Wt::WModelIndex &index, int role) const {
    ASSERT_require(index.isValid());
    ASSERT_require(index.row()>=0 && (size_t)index.row() < functions_.size());
    P2::Function::Ptr function = functions_[index.row()];
    if (Wt::DisplayRole == role) {
        switch (index.column()) {
            case C_ENTRY:
                return Wt::WString(StringUtility::addrToString(function->address()));
            case C_NAME:
                return Wt::WString(StringUtility::cEscape(function->name()));
            case C_SIZE:
                return functionNBytes(ctx_.partitioner, function);
            case C_IMPORT:
                return Wt::WString((function->reasons() & SgAsmFunction::FUNC_IMPORT)!=0 ? "yes" : "no");
            case C_EXPORT:
                return Wt::WString((function->reasons() & SgAsmFunction::FUNC_EXPORT)!=0 ? "yes" : "no");
            case C_NCALLERS:
                return functionNCallers(ctx_.partitioner, function);
            case C_NRETURNS:
                return functionNReturns(ctx_.partitioner, function);
            case C_MAYRETURN:
                switch (functionMayReturn(ctx_.partitioner, function)) {
                    case MAYRETURN_YES:     return Wt::WString("yes");
                    case MAYRETURN_NO:      return Wt::WString("no");
                    case MAYRETURN_UNKNOWN: return Wt::WString("");
                }
                ASSERT_not_reachable("invalid may-return value");
            case C_STACKDELTA: {
                int64_t delta = functionStackDelta(ctx_.partitioner, function);
                if (delta != SgAsmInstruction::INVALID_STACK_DELTA)
                    return Wt::WString(boost::lexical_cast<std::string>(delta));
                if (functionMayReturn(ctx_.partitioner, function) == MAYRETURN_NO)
                    return Wt::WString("NA");
                return Wt::WString("");
            }
            default:
                ASSERT_not_reachable("invalid column number");
        }
    }
    return boost::any();
}

static bool sortByAscendingAddress(const P2::Function::Ptr &a, const P2::Function::Ptr &b) {
    return a->address() < b->address();
}
static bool sortByDescendingAddress(const P2::Function::Ptr &a, const P2::Function::Ptr &b) {
    return a->address() > b->address();
}
static bool sortByAscendingName(const P2::Function::Ptr &a, const P2::Function::Ptr &b) {
    return a->name() < b->name();
}
static bool sortByDescendingName(const P2::Function::Ptr &a, const P2::Function::Ptr &b) {
    return a->name() > b->name();
}
static bool sortByAscendingSize(const P2::Function::Ptr &a, const P2::Function::Ptr &b) {
    return a->attr<size_t>(ATTR_NBYTES).orElse(0) < b->attr<size_t>(ATTR_NBYTES).orElse(0);
}
static bool sortByDescendingSize(const P2::Function::Ptr &a, const P2::Function::Ptr &b) {
    return a->attr<size_t>(ATTR_NBYTES).orElse(0) > b->attr<size_t>(ATTR_NBYTES).orElse(0);
}
static bool sortByAscendingImport(const P2::Function::Ptr &a, const P2::Function::Ptr &b) {
    unsigned aa = a->reasons() & SgAsmFunction::FUNC_IMPORT;
    unsigned bb = b->reasons() & SgAsmFunction::FUNC_IMPORT;
    return aa < bb;
}
static bool sortByDescendingImport(const P2::Function::Ptr &a, const P2::Function::Ptr &b) {
    unsigned aa = a->reasons() & SgAsmFunction::FUNC_IMPORT;
    unsigned bb = b->reasons() & SgAsmFunction::FUNC_IMPORT;
    return aa > bb;
}
static bool sortByAscendingExport(const P2::Function::Ptr &a, const P2::Function::Ptr &b) {
    unsigned aa = a->reasons() & SgAsmFunction::FUNC_EXPORT;
    unsigned bb = b->reasons() & SgAsmFunction::FUNC_EXPORT;
    return aa < bb;
}
static bool sortByDescendingExport(const P2::Function::Ptr &a, const P2::Function::Ptr &b) {
    unsigned aa = a->reasons() & SgAsmFunction::FUNC_EXPORT;
    unsigned bb = b->reasons() & SgAsmFunction::FUNC_EXPORT;
    return aa > bb;
}
static bool sortByAscendingCallers(const P2::Function::Ptr &a, const P2::Function::Ptr &b) {
    return a->attr<size_t>(ATTR_NCALLERS).orElse(0) < b->attr<size_t>(ATTR_NCALLERS).orElse(0);
}
static bool sortByDescendingCallers(const P2::Function::Ptr &a, const P2::Function::Ptr &b) {
    return a->attr<size_t>(ATTR_NCALLERS).orElse(0) > b->attr<size_t>(ATTR_NCALLERS).orElse(0);
}
static bool sortByAscendingReturns(const P2::Function::Ptr &a, const P2::Function::Ptr &b) {
    return a->attr<size_t>(ATTR_NRETURNS).orElse(0) < b->attr<size_t>(ATTR_NRETURNS).orElse(0);
}
static bool sortByDescendingReturns(const P2::Function::Ptr &a, const P2::Function::Ptr &b) {
    return a->attr<size_t>(ATTR_NRETURNS).orElse(0) > b->attr<size_t>(ATTR_NRETURNS).orElse(0);
}
static bool sortByAscendingMayReturn(const P2::Function::Ptr &a, const P2::Function::Ptr &b) {
    return (a->attr<MayReturn>(ATTR_MAYRETURN).orElse(MAYRETURN_UNKNOWN) <
            b->attr<MayReturn>(ATTR_MAYRETURN).orElse(MAYRETURN_UNKNOWN));
}
static bool sortByDescendingMayReturn(const P2::Function::Ptr &a, const P2::Function::Ptr &b) {
    return (a->attr<MayReturn>(ATTR_MAYRETURN).orElse(MAYRETURN_UNKNOWN) >
            b->attr<MayReturn>(ATTR_MAYRETURN).orElse(MAYRETURN_UNKNOWN));
}
static bool sortByAscendingStackDelta(const P2::Function::Ptr &a, const P2::Function::Ptr &b) {
    return (a->attr<int64_t>(ATTR_STACKDELTA).orElse(SgAsmInstruction::INVALID_STACK_DELTA) <
            b->attr<int64_t>(ATTR_STACKDELTA).orElse(SgAsmInstruction::INVALID_STACK_DELTA));
}
static bool sortByDescendingStackDelta(const P2::Function::Ptr &a, const P2::Function::Ptr &b) {
    return (a->attr<int64_t>(ATTR_STACKDELTA).orElse(SgAsmInstruction::INVALID_STACK_DELTA) >
            b->attr<int64_t>(ATTR_STACKDELTA).orElse(SgAsmInstruction::INVALID_STACK_DELTA));
}

static double heatZero(P2::Partitioner &p, const P2::Function::Ptr &function) {
    return 0.0;
}
static double heatName(P2::Partitioner &p, const P2::Function::Ptr &function) {
    return function->name().empty() ? 0 : 1;
}
static double heatSize(P2::Partitioner &p, const P2::Function::Ptr &function) {
    return functionNBytes(p, function);
}
static double heatImport(P2::Partitioner &p, const P2::Function::Ptr &function) {
    return 0==(function->reasons() & SgAsmFunction::FUNC_IMPORT) ? 0 : 1;
}
static double heatExport(P2::Partitioner &p, const P2::Function::Ptr &function) {
    return 0==(function->reasons() & SgAsmFunction::FUNC_EXPORT) ? 0 : 1;
}
static double heatNCallers(P2::Partitioner &p, const P2::Function::Ptr &function) {
    return functionNCallers(p, function);
}
static double heatNReturns(P2::Partitioner &p, const P2::Function::Ptr &function) {
    return functionNReturns(p, function);
}
static double heatMayReturn(P2::Partitioner &p, const P2::Function::Ptr &function) {
    switch (functionMayReturn(p, function)) {
        case MAYRETURN_YES: return 1.0;
        case MAYRETURN_NO: return 0.5;
        case MAYRETURN_UNKNOWN: return NAN;
    }
    ASSERT_not_reachable("unhandled may-return result");
}
static double heatStackDelta(P2::Partitioner &p, const P2::Function::Ptr &function) {
    int64_t delta = functionStackDelta(p, function);
    return SgAsmInstruction::INVALID_STACK_DELTA==delta ? NAN : (double)delta;
}

void
FunctionListModel::sort(int column, Wt::SortOrder order) {
    bool(*sorter)(const P2::Function::Ptr&, const P2::Function::Ptr&) = NULL;
    double(*heat)(P2::Partitioner&, const P2::Function::Ptr&) = NULL;
    switch (column) {
        case C_ENTRY:
            sorter = Wt::AscendingOrder==order ? sortByAscendingAddress : sortByDescendingAddress;
            heat = heatZero;
            break;
        case C_NAME:
            sorter = Wt::AscendingOrder==order ? sortByAscendingName : sortByDescendingName;
            heat = heatName;
            break;
        case C_SIZE:
            BOOST_FOREACH (const P2::Function::Ptr &function, functions_)
                (void) functionNBytes(ctx_.partitioner, function); // make sure sizes are cached for all functions
            sorter = Wt::AscendingOrder==order ? sortByAscendingSize : sortByDescendingSize;
            heat = heatSize;
            break;
        case C_IMPORT:
            sorter = Wt::AscendingOrder==order ? sortByAscendingImport : sortByDescendingImport;
            heat = heatImport;
            break;
        case C_EXPORT:
            sorter = Wt::AscendingOrder==order ? sortByAscendingExport : sortByDescendingExport;
            heat = heatExport;
            break;
        case C_NCALLERS:
            BOOST_FOREACH (const P2::Function::Ptr &function, functions_)
                (void) functionNCallers(ctx_.partitioner, function); // make sure they're all cached before sorting
            sorter = Wt::AscendingOrder==order ? sortByAscendingCallers : sortByDescendingCallers;
            heat = heatNCallers;
            break;
        case C_NRETURNS:
            BOOST_FOREACH (const P2::Function::Ptr &function, functions_)
                (void) functionNReturns(ctx_.partitioner, function); // make sure they're all cached
            sorter = Wt::AscendingOrder==order ? sortByAscendingReturns : sortByDescendingReturns;
            heat = heatNReturns;
            break;
        case C_MAYRETURN:
            BOOST_FOREACH (const P2::Function::Ptr &function, functions_)
                (void) functionMayReturn(ctx_.partitioner, function); // make sure they're all cached
            sorter = Wt::AscendingOrder==order ? sortByAscendingMayReturn : sortByDescendingMayReturn;
            heat = heatMayReturn;
            break;
        case C_STACKDELTA:
            BOOST_FOREACH (const P2::Function::Ptr &function, functions_)
                (void) functionStackDelta(ctx_.partitioner, function); // make sure they're all cached
            sorter = Wt::AscendingOrder==order ? sortByAscendingStackDelta : sortByDescendingStackDelta;
            heat = heatStackDelta;
            break;
        default:
            ASSERT_not_reachable("invalid column number");
    }
    if (sorter) {
        layoutAboutToBeChanged().emit();
        std::sort(functions_.begin(), functions_.end(), sorter);
        layoutChanged().emit();
    }

    // If there's a way to calculate the heat map for a function, then call it and cache the results in the ATTR_HEAT.
    ASSERT_not_null(heat);
    std::vector<double> dv;
    dv.reserve(functions_.size());
    heatStats_ = FpStatistics();
    BOOST_FOREACH (const P2::Function::Ptr &function, functions_) {
        double d = (heat)(ctx_.partitioner, function);
        function->attr(ATTR_HEAT, d);
        if (!isnan(d)) {
            heatStats_.insert(d);
            dv.push_back(d);
        }
    }
    heatStats_.computeRanks(dv);
}

} // namespace
