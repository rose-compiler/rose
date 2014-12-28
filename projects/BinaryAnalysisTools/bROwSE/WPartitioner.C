#include <bROwSE/WPartitioner.h>

#include <bROwSE/WMemoryMap.h>
#include <Disassembler.h>                               // ROSE
#include <Partitioner2/Modules.h>                       // ROSE
#include <Partitioner2/ModulesPe.h>                     // ROSE
#include <sawyer/Message.h>
#include <sawyer/Stopwatch.h>
#include <Wt/WBreak>
#include <Wt/WCheckBox>
#include <Wt/WComboBox>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WVBoxLayout>

using namespace rose;
using namespace Sawyer::Message::Common;

namespace bROwSE {

void
WPartitioner::init() {

    new Wt::WText("NOTE: This is not the final state of this page. Sorry about the mess here.", this);
    new Wt::WBreak(this);

    wParseSpecimen_ = new Wt::WPushButton("Parse ELF/PE", this);
    wParseSpecimen_->clicked().connect(boost::bind(&WPartitioner::changeState, this, ParsedSpecimen));

    new Wt::WBreak(this);
    new Wt::WText("Which interpretation: ", this);
    wInterpretation_ = new Wt::WComboBox(this);
    wInterpretation_->sactivated().connect(boost::bind(&WPartitioner::chooseInterpretation, this, _1));

    new Wt::WBreak(this);
    wLoadSpecimen_ = new Wt::WPushButton("Load/Link", this);
    wLoadSpecimen_->clicked().connect(boost::bind(&WPartitioner::changeState, this, LoadedSpecimen));

    new Wt::WBreak(this);
    new Wt::WText("Instruction set architecture: ", this);
    wIsaName_ = new Wt::WComboBox(this);
    wIsaName_->addItem("automatic");
    BOOST_FOREACH (const std::string &s, BinaryAnalysis::Disassembler::isaNames())
        wIsaName_->addItem(s);
    wIsaError_ = new Wt::WText(this);
    wIsaError_->setStyleClass("text-error");
    wIsaError_->hide();
    wIsaName_->activated().connect(boost::bind(&WPartitioner::clearIsaError, this));

    new Wt::WBreak(this);
    wUseSemantics_ = new Wt::WCheckBox("Use semantics?", this);
    wUseSemantics_->setCheckState(Wt::Checked);

    new Wt::WBreak(this);
    wFollowGhostEdges_ = new Wt::WCheckBox("Follow ghost edges?", this);
    wFollowGhostEdges_->setCheckState(Wt::Unchecked);

    new Wt::WBreak(this);
    wAllowDiscontiguousBlocks_ = new Wt::WCheckBox("Allow discontiguous basic blocks?", this);
    wAllowDiscontiguousBlocks_->setCheckState(Wt::Checked);

    new Wt::WBreak(this);
    wFindDeadCode_ = new Wt::WCheckBox("Find dead code?", this);
    wFindDeadCode_->setCheckState(Wt::Checked);

    new Wt::WBreak(this);
    wDefeatPeScrambler_ = new Wt::WCheckBox("Defeat PEScrambler?", this);
    wDefeatPeScrambler_->setCheckState(Wt::Unchecked);
    wPeScramblerDispatchVa_ = new WHexValueEdit("&nbsp;&nbsp;&nbsp;&nbsp;PEScrambler dispatcher VA:", this);
    wPeScramblerDispatchVa_->value(0, AddressInterval::whole());

    new Wt::WBreak(this);
    wIntraFunctionCode_ = new Wt::WCheckBox("Find intra-function unreachable code?", this);
    wIntraFunctionCode_->setCheckState(Wt::Checked);

    new Wt::WBreak(this);
    wAssumeFunctionsReturn_ = new Wt::WCheckBox("Assume functions return?", this);
    wAssumeFunctionsReturn_->setCheckState(Wt::Checked);

    new Wt::WBreak(this);
    wPartitionSpecimen_ = new Wt::WPushButton("Disassemble", this);
    wPartitionSpecimen_->clicked().connect(boost::bind(&WPartitioner::changeState, this, PartitionedSpecimen));
    new Wt::WText(" (This may take a while)", this);
}

void
WPartitioner::changeState(State newState) {
    if (newState == state_) {
        return;
    } else if (state_ < newState) {
        while (state_ < newState) {
            State nextState = (State)(state_ + 1);      // state to cause to become in effect
            switch (nextState) {
                case InitialState:
                    ASSERT_not_reachable("InitialState must be first");
                case ParsedSpecimen:
                    parseSpecimen();
                    break;
                case LoadedSpecimen:
                    loadSpecimen();
                    break;
                case PartitionedSpecimen:
                    partitionSpecimen();
                    break;
            }
            state_ = nextState;
        }
    } else {
        // Undo each state_ one at a time.
        while (state_ > newState) {
            switch (state_) {
                case PartitionedSpecimen:
                    undoPartitionSpecimen();
                    break;
                case LoadedSpecimen:
                    undoLoadSpecimen();
                    break;
                case ParsedSpecimen:
                    undoParseSpecimen();
                    break;
                case InitialState:
                    ASSERT_not_reachable("Initial state must be first");
            }
            state_ = (State)(state_ - 1);
        }
    }
}

void
WPartitioner::parseSpecimen() {
    Sawyer::Stopwatch timer;
    Sawyer::Message::Stream info(mlog[INFO] <<"parse ELF/PE containers");
    ctx_.engine.parse(ctx_.specimenNames);
    info <<"; took " <<timer <<" seconds.\n";

    // Offer interpretations (if there are any), but default to the one chosen by the engine
    wInterpretation_->clear();
    if (SgProject *project = SageInterface::getProject()) {
        SgAsmInterpretation *chosen = ctx_.engine.interpretation();
        std::vector<SgAsmInterpretation*> interps = SageInterface::querySubTree<SgAsmInterpretation>(project);
        size_t interpIdx = 0;
        BOOST_FOREACH (SgAsmInterpretation *interp, interps) {
            std::set<std::string> headerTypes;
            BOOST_FOREACH (SgAsmGenericHeader *header, interp->get_headers()->get_headers())
                headerTypes.insert(header->class_name());
            std::string s = StringUtility::numberToString(interpIdx+1) + ":";
            BOOST_FOREACH (const std::string &headerType, headerTypes)
                s += " " + headerType;
            wInterpretation_->addItem(s);
            if (interp == chosen)
                wInterpretation_->setCurrentIndex(interpIdx);
            ++interpIdx;
        }
    }
    
    specimenParsed_.emit(true);
}

void
WPartitioner::undoParseSpecimen() {
    ctx_.engine.interpretation(NULL);
    wInterpretation_->clear();
    specimenParsed_.emit(false);
}

void
WPartitioner::chooseInterpretation(const Wt::WString &text) {
    size_t interpIdx = strtol(text.narrow().c_str(), NULL, 10) - 1;
    if (SgProject *project = SageInterface::getProject()) {
        changeState(ParsedSpecimen);
        std::vector<SgAsmInterpretation*> interps = SageInterface::querySubTree<SgAsmInterpretation>(project);
        ASSERT_require(interpIdx < interps.size());
        ctx_.engine.interpretation(interps[interpIdx]);
    }
}

void
WPartitioner::loadSpecimen() {
    Sawyer::Stopwatch timer;
    Sawyer::Message::Stream info(mlog[INFO] <<"load specimen");
    ctx_.engine.load(ctx_.specimenNames);
    info <<"; took " <<timer <<" seconds\n";
    specimenLoaded_.emit(true);
}

void
WPartitioner::undoLoadSpecimen() {
    if (ctx_.engine.interpretation())
        ctx_.engine.interpretation()->set_map(NULL);
    ctx_.engine.memoryMap(MemoryMap());
    specimenLoaded_.emit(false);
}

void
WPartitioner::clearIsaError() {
    wIsaError_->setText("");
    wIsaError_->hide();
}

void
WPartitioner::partitionSpecimen() {
    SgAsmInterpretation *interp = ctx_.engine.interpretation();// null if no ELF/PE container

    BinaryAnalysis::Disassembler *disassembler = ctx_.engine.obtainDisassembler(isaName());
    if (NULL==disassembler) {
        wIsaError_->setText("ISA must be specified when there is no ELF/PE container.");
        wIsaError_->show();
        return;
    }

    // Adjust some engine settings
    ctx_.engine.opaquePredicateSearch(findDeadCode());
    ctx_.engine.intraFunctionCodeSearch(intraFunctionCode());
    
    // Obtain the memory map which might have been edited by now.
    if (wMemoryMap_)
        ctx_.engine.memoryMap(wMemoryMap_->memoryMap());

    // Create the partitioner
    P2::Partitioner &p = ctx_.partitioner = ctx_.engine.createTunedPartitioner();
    p.enableSymbolicSemantics(useSemantics());
    p.assumeFunctionsReturn(assumeFunctionsReturn());
    if (followGhostEdges())
        p.basicBlockCallbacks().append(P2::Modules::AddGhostSuccessors::instance());
    if (!allowDiscontiguousBlocks())
        p.basicBlockCallbacks().append(P2::Modules::PreventDiscontiguousBlocks::instance());
    if (defeatPeScrambler()) {
        rose_addr_t dispatcherVa = peScramblerDispatcherVa();
        P2::ModulesPe::PeDescrambler::Ptr cb = P2::ModulesPe::PeDescrambler::instance(dispatcherVa);
        cb->nameKeyAddresses(p);                        // give names to certain PEScrambler things
        p.basicBlockCallbacks().append(cb);
        p.attachFunction(P2::Function::instance(dispatcherVa, p.addressName(dispatcherVa), SgAsmFunction::FUNC_USERDEF));
    }
    ctx_.engine.labelAddresses(p, interp);

    // FIXME[Robb P. Matzke 2014-12-27]: we should load configuration files here

    // Run the partitioner (this could take a long time)
    Sawyer::Stopwatch timer;
    Sawyer::Message::Stream info(mlog[INFO] <<"disassembling and partitioning");
    ctx_.engine.runPartitioner(ctx_.partitioner, interp);
    info <<"; took " <<timer <<" seconds\n";

    specimenPartitioned_.emit(true);
}

void
WPartitioner::undoPartitionSpecimen() {
    ctx_.partitioner = P2::Partitioner();
    specimenPartitioned_.emit(false);
}

std::string
WPartitioner::isaName() const {
    Wt::WString s = wIsaName_->currentText();
    if (s == "automatic")
        s = "";
    return s.narrow();
}

bool
WPartitioner::useSemantics() const {
    return wUseSemantics_->checkState() == Wt::Checked;
}

bool
WPartitioner::followGhostEdges() const {
    return wFollowGhostEdges_->checkState() == Wt::Checked;
}

bool
WPartitioner::allowDiscontiguousBlocks() const {
    return wAllowDiscontiguousBlocks_->checkState() == Wt::Checked;
}

bool
WPartitioner::findDeadCode() const {
    return wFindDeadCode_->checkState() == Wt::Checked;
}

bool
WPartitioner::defeatPeScrambler() const {
    return wDefeatPeScrambler_->checkState() == Wt::Checked;
}

rose_addr_t
WPartitioner::peScramblerDispatcherVa() const {
    // FIXME[Robb P. Matzke 2014-12-27]: We should not have to press the SAVE key.
    return wPeScramblerDispatchVa_->value();
}

bool
WPartitioner::intraFunctionCode() const {
    return wIntraFunctionCode_->checkState() == Wt::Checked;
}

bool
WPartitioner::assumeFunctionsReturn() const {
    return wAssumeFunctionsReturn_->checkState() == Wt::Checked;
}

} // namespace
