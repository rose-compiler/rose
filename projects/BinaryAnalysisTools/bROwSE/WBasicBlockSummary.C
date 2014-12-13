#include <bROwSE/WBasicBlockSummary.h>
#include <BaseSemantics2.h>                             // ROSE

using namespace ::rose::BinaryAnalysis;

namespace bROwSE {

static Wt::WText*
field(std::vector<Wt::WText*> &fields, const std::string &toolTip) {
    Wt::WText *wValue = new Wt::WText();
    if (toolTip!="")
        wValue->setToolTip(toolTip);
    fields.push_back(wValue);
    return wValue;
}

void
WBasicBlockSummary::init(Context &ctx) {
    std::vector<Wt::WText*> fields;
    wHasOpaquePredicates_   = field(fields, "Block has conditional branches that are never taken?");
    wDataBlocks_            = field(fields, "Number of data blocks owned by this basic block.");
    wIsFunctionCall_        = field(fields, "Does this block appear to call a function?");
    wIsFunctionReturn_      = field(fields, "Does this block appear to return from a function call?");
    wStackDeltaIn_          = field(fields, "Stack pointer delta at block entrance w.r.t. function.");
    wStackDeltaOut_         = field(fields, "Stack pointer delta at block exit w.r.t. function.");

    table_ = new Wt::WTable(this);
    table_->setWidth("100%");
    static const size_t ncols = 1;
    size_t nrows = (fields.size()+ncols-1) / ncols;
    for (size_t col=0, i=0; col<ncols; ++col) {
        for (size_t row=0; row<nrows && i<fields.size(); ++row)
            table_->elementAt(row, col)->addWidget(fields[i++]);
    }
}

void
WBasicBlockSummary::changeBasicBlock(const P2::BasicBlock::Ptr &bblock) {
    if (bblock == bblock_)
        return;
    bblock_ = bblock;
    if (bblock) {
        std::set<rose_addr_t> ghostSuccessors = ctx_.partitioner.basicBlockGhostSuccessors(bblock);
        wHasOpaquePredicates_->setText(StringUtility::plural(ghostSuccessors.size(), "opaque predicates"));

        wDataBlocks_->setText(StringUtility::plural(bblock->dataBlocks().size(), "data blocks"));

        wIsFunctionCall_->setText(ctx_.partitioner.basicBlockIsFunctionCall(bblock) ?
                                  "appears to have function call semantics" : "lacks function call semantics");

        wIsFunctionReturn_->setText(ctx_.partitioner.basicBlockIsFunctionReturn(bblock) ?
                                    "appears to be a function return" : "lacks function return semantics");

        InstructionSemantics2::BaseSemantics::SValuePtr stackDeltaIn = ctx_.partitioner.basicBlockStackDeltaIn(bblock);
        if (stackDeltaIn!=NULL) {
            std::ostringstream stackDeltaStream;
            stackDeltaStream <<*stackDeltaIn;
            wStackDeltaIn_->setText("stack delta in: " + stackDeltaStream.str());
        } else {
            wStackDeltaIn_->setText("stack delta in: not computed");
        }

        InstructionSemantics2::BaseSemantics::SValuePtr stackDeltaOut = ctx_.partitioner.basicBlockStackDeltaOut(bblock);
        if (stackDeltaOut!=NULL) {
            std::ostringstream stackDeltaStream;
            stackDeltaStream <<*stackDeltaOut;
            wStackDeltaOut_->setText("stack delta out: " + stackDeltaStream.str());
        } else {
            wStackDeltaOut_->setText("stack delta out: not computed");
        }

        table_->show();
    } else {
        wHasOpaquePredicates_->setText("");
        wStackDeltaIn_->setText("");
        table_->hide();
    }
}

} // namespace
