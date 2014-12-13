#include <bROwSE/WFunctionSummary.h>

#include <bROwSE/FunctionUtil.h>
#include <Wt/WTable>
#include <Wt/WText>

namespace bROwSE {

static
Wt::WText* field(std::vector<Wt::WText*> &fields, const std::string &toolTip) {
    Wt::WText *wValue = new Wt::WText();
    if (toolTip!="")
        wValue->setToolTip(toolTip);
    fields.push_back(wValue);
    return wValue;
}

void
WFunctionSummary::init() {
    std::vector<Wt::WText*> fields;
    wName_       = field(fields, "Function name");
    wEntry_      = field(fields, "Primary entry virtual address");
    wBBlocks_    = field(fields, "Number of basic blocks");
    wDBlocks_    = field(fields, "Number of data blocks");
    wInsns_      = field(fields, "Number of instructions");
    wBytes_      = field(fields, "Number of distinct addresses with code and/or data");
    wDiscontig_  = field(fields, "Number of discontiguous regions in the address space");
    wCallers_    = field(fields, "Number of distinct functions that call this function");
    wCallsInto_  = field(fields, "Number of call sites calling this function");
    wCallees_    = field(fields, "Number of distinct functions this function calls.");
    wCallsOut_   = field(fields, "Number of function call sites in this function");
    wRecursive_  = field(fields, "Does this function call itself directly?");
    wMayReturn_  = field(fields, "Might this function return its caller?");
    wStackDelta_ = field(fields, "Net effect on stack pointer");

    table_ = new Wt::WTable(this);
    table_->setWidth("100%");
    static const size_t ncols = 4;
    size_t nrows = (fields.size()+ncols-1) / ncols;
    for (size_t col=0, i=0; col<ncols; ++col) {
        for (size_t row=0; row<nrows && i<fields.size(); ++row)
            table_->elementAt(row, col)->addWidget(fields[i++]);
    }
}

void
WFunctionSummary::changeFunction(const P2::Function::Ptr &function) {
    if (function == function_)
        return;
    function_ = function;
    if (function) {
        size_t nInsns = functionNInsns(ctx_.partitioner, function);
        size_t nBytes = functionNBytes(ctx_.partitioner, function);
        size_t nIntervals = ctx_.partitioner.functionExtent(function).nIntervals();
        const P2::FunctionCallGraph *cg = functionCallGraph(ctx_.partitioner);
        MayReturn mayReturn = functionMayReturn(ctx_.partitioner, function);
        std::string mayReturnStr = MAYRETURN_NO==mayReturn?"never returns":
                                   (MAYRETURN_YES==mayReturn?"may return" : "may return is unknown");
        int64_t stackDelta = functionStackDelta(ctx_.partitioner, function);
        char stackDeltaStr[64];
        if (stackDelta == SgAsmInstruction::INVALID_STACK_DELTA) {
            strcpy(stackDeltaStr, "stack delta is unknown");
        } else {
            sprintf(stackDeltaStr, "stack delta is %+"PRId64, stackDelta);
        }

        ASSERT_not_null(cg);
        wName_      ->setText(function->name()==""?std::string("(no name)"):function->name());
        wEntry_     ->setText("entry "+StringUtility::addrToString(function->address()));
        wBBlocks_   ->setText(StringUtility::plural(function->basicBlockAddresses().size(), "basic blocks"));
        wDBlocks_   ->setText(StringUtility::plural(function->dataBlocks().size(), "data blocks"));
        wInsns_     ->setText(StringUtility::plural(nInsns, "instructions"));
        wBytes_     ->setText(StringUtility::plural(nBytes, "bytes"));
        wDiscontig_ ->setText(StringUtility::plural(nIntervals, "contiguous intervals"));
        wCallers_   ->setText(StringUtility::plural(cg->nCallers(function), "distinct callers"));
        wCallsInto_ ->setText(StringUtility::plural(cg->nCallsIn(function), "calls")+" incoming");
        wCallees_   ->setText(StringUtility::plural(cg->nCallees(function), "distinct callees"));
        wCallsOut_  ->setText(StringUtility::plural(cg->nCallsOut(function), "calls")+" outgoing");
        wRecursive_ ->setText(cg->nCalls(function, function)?"recursive":"non-recursive");
        wMayReturn_ ->setText(mayReturnStr);
        wStackDelta_->setText(stackDeltaStr);
        table_->show();
    } else {
        wName_      ->setText("");
        wEntry_     ->setText("");
        wBBlocks_   ->setText("");
        wDBlocks_   ->setText("");
        wInsns_     ->setText("");
        wBytes_     ->setText("");
        wDiscontig_ ->setText("");
        wCallers_   ->setText("");
        wCallsInto_ ->setText("");
        wCallees_   ->setText("");
        wCallsOut_  ->setText("");
        wRecursive_ ->setText("");
        wMayReturn_ ->setText("");
        wStackDelta_->setText("");
        table_->hide();
    }
}

} // namespace
