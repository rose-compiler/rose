#include <bROwSE/WPartitioner.h>

#include <boost/thread.hpp>
#include <bROwSE/WBusy.h>
#include <bROwSE/WMemoryMap.h>
#include <Disassembler.h>                               // ROSE
#include <Partitioner2/Modules.h>                       // ROSE
#include <Partitioner2/ModulesPe.h>                     // ROSE
#include <rose_strtoull.h>
#include <sawyer/Message.h>
#include <sawyer/Stopwatch.h>
#include <Wt/WBreak>
#include <Wt/WBorder>
#include <Wt/WCheckBox>
#include <Wt/WComboBox>
#include <Wt/WLineEdit>
#include <Wt/WPanel>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WVBoxLayout>

using namespace rose;
using namespace Sawyer::Message::Common;

namespace bROwSE {

static const Color::HSV panelDone(1/3.0, 1, 0.8);       // light green when completed
static const Color::HSV panelNotDone(2/3.0, 1, 0.8);    // light blue if not completed

void
WPartitioner::init() {

    new Wt::WText("This tab controls the major steps needed to disassemble a specimen.  Each panel contains "
                  "some configuration items and a final action that triggers the step.  Steps are generally "
                  "performed in the order listed here, but clicking on a later step will cause all earlier "
                  "steps to also run, and clicking on an earlier step after a later step has already run will "
                  "undo the effect of the later step.", this);
    new Wt::WBreak(this);
    Wt::WString tip;

    //-------------------
    // Container parsing
    //-------------------
    {
        Wt::WContainerWidget *c = makePanel(ParsedSpecimen, "Parsing container", this);
        new Wt::WText("These steps are for parsing any container files such as ELF and PE.  If the specimen has no "
                      "container files then performing this action will have no effect.", c);

        new Wt::WBreak(c);
        wParseSpecimen_ = new Wt::WPushButton("Parse", c);
        wParseSpecimen_->clicked().connect(boost::bind(&WPartitioner::redoState, this, ParsedSpecimen));
    }

    //--------------- 
    // Load and link
    //---------------
    {
        Wt::WContainerWidget *c = makePanel(LoadedSpecimen, "Loading into memory", this);
        new Wt::WText("These steps cause data to be mapped into specimen memory.  The data comes from places such as the "
                      "segments and sections of ELF or PE containers, from files containing raw data, or from the memory "
                      "of other processes.  Once the memory is initialized one can navigate to the \"Memory Map\" tab "
                      "to make adjustments before proceeding to the next step.", c);

        tip = "Some containers hold multiple binary interpretations, such as a DOS or PE interpretation in "
              "Microsoft Windows binary. The disassembler operates on only one interpretation at a time.";
        new Wt::WBreak(c);
        new Wt::WText("Interpretation: ", c);
        wInterpretation_ = new Wt::WComboBox(c);
        wInterpretation_->setToolTip(tip);
        wInterpretation_->sactivated().connect(boost::bind(&WPartitioner::chooseInterpretation, this, _1));

        new Wt::WBreak(c);
        wLoadSpecimen_ = new Wt::WPushButton("Load/Link", c);
        wLoadSpecimen_->clicked().connect(boost::bind(&WPartitioner::redoState, this, LoadedSpecimen));
    }

    //------------- 
    // Disassemble
    //-------------
    {
        Wt::WContainerWidget *c = makePanel(PartitionedSpecimen, "Disassembling memory", this);
        new Wt::WText("These steps disassemble memory and partition instructions into basic blocks and basic blocks into "
                      "functions.", c);

        // FIXME[Robb P. Matzke 2014-12-30]: not fully implemented
        tip = "A configuration file can be specified on the command-line when the server is started. This item is mostly "
              "just a placeholder for when we eventually allow configuration files to be uploaded from the browser.";
        new Wt::WBreak(c);
        wUseConfiguration_ =
            new Wt::WCheckBox("Use configuration \"" +
                              StringUtility::htmlEscape(StringUtility::cEscape(ctx_.settings.configurationName)) + "\"?", c);
        wUseConfiguration_->setToolTip(tip);
        wUseConfiguration_->setCheckState(Wt::Checked);
        wUseConfiguration_->setHidden(ctx_.settings.configurationName.empty());

        tip = "Overrides the ISA found in an ELF/PE container, or provides an ISA if the specimen has "
              "no container (e.g., raw data).";
        new Wt::WBreak(c);
        new Wt::WText("Instruction set architecture: ", c);
        wIsaName_ = new Wt::WComboBox(c);
        wIsaName_->setToolTip(tip);
        wIsaName_->addItem("automatic");
        BOOST_FOREACH (const std::string &s, BinaryAnalysis::Disassembler::isaNames())
            wIsaName_->addItem(s);
        wIsaError_ = new Wt::WText(c);
        wIsaError_->setStyleClass("text-error");
        wIsaError_->hide();
        wIsaName_->activated().connect(boost::bind(&WPartitioner::clearIsaError, this));

        tip = "Instruction semantics enable the partitioner to reason about the effect of instructions or blocks of "
              "instructions without needing to match certain patterns.  For instance, opaque predicates can be found "
              "when semantics are enabled. Turning off semantics can make the partitioner much faster but with a slight "
              "loss of accuracy.";
        new Wt::WBreak(c);
        wUseSemantics_ = new Wt::WCheckBox("Use semantics?", c);
        wUseSemantics_->setToolTip(tip);
        wUseSemantics_->setCheckState(Wt::Checked);
        wUseSemantics_->checked().connect(boost::bind(&WPartitioner::handleUseSemantics, this, true));
        wUseSemantics_->unChecked().connect(boost::bind(&WPartitioner::handleUseSemantics, this, false));

        tip = "ROSE's definition of basic block is more permissive than many disassemblers, and can allow instructions "
              "that are not located adjacent to each other in memory, or instructions that overlap.  Disabling this will "
              "result in basic blocks whose instructions are contiguous and non-overlapping.";
        new Wt::WBreak(c);
        wAllowDiscontiguousBlocks_ = new Wt::WCheckBox("Allow discontiguous basic blocks?", c);
        wAllowDiscontiguousBlocks_->setToolTip(tip);
        wAllowDiscontiguousBlocks_->setCheckState(Wt::Checked);

        tip = "Ghost edges are those edges occluded by an opaque predicate.  Following such edges is one method by "
              "which dead code can be found. The edges can either be followed immediately as if the predicate was not "
              "opaque, or they can be followed later when more information is known about their target address. In any "
              "case, ghost edges are real edges when instruction semantics is disabled.";
        new Wt::WBreak(c);
        new Wt::WText("Follow ghost edges? ", c);
        wFollowGhostEdges_ = new Wt::WComboBox(c);
        wFollowGhostEdges_->setToolTip(tip);
        wFollowGhostEdges_->addItem("Never");           ASSERT_require(FOLLOW_NEVER==0); // or fix followGhostEdges()
        wFollowGhostEdges_->addItem("Immediately");     ASSERT_require(FOLLOW_NOW==1);
        wFollowGhostEdges_->addItem("Later");           ASSERT_require(FOLLOW_LATER==2);
        wFollowGhostEdges_->setCurrentIndex(FOLLOW_LATER);
        wFollowGhostEdgesWarning_ = new Wt::WText("&nbsp;&nbsp;(no ghost edges possible; requires semantics)", c);
        wFollowGhostEdgesWarning_->hide();

        tip = "If checked, search for dead code by looking for regions of memory that were not disassembled by other means. "
              "This finds more code than ghost-edge-following because it does not need an edge to find the code, but it is"
              "more likely to inadvertently disassemble non-code areas.";
        new Wt::WBreak(c);
        wFindDeadCode_ = new Wt::WCheckBox("Find disconnected dead code?", c);
        wFindDeadCode_->setToolTip(tip);
        wFindDeadCode_->setCheckState(Wt::Checked);

        tip = "PEScrambler is \"a tool to obfuscate win32 binaries automatically. It can relocate portions of code and protect "
              "them with anti-disassembly code. It also defeats static program flow analysis by re-routing all function calls "
              "through a central dispatcher function.\" ROSE is largely immune to these techniques by default, and supplying "
              "the address of the central dispatcher function here will immunize it against the dispatcher. The dispatcher "
              "address can be easily found by looking for the function that has the most calls.";
        new Wt::WBreak(c);
        wDefeatPeScrambler_ = new Wt::WCheckBox("Defeat PEScrambler?", c);
        wDefeatPeScrambler_->setToolTip(tip);
        wDefeatPeScrambler_->setCheckState(Wt::Unchecked);
        new Wt::WText("&nbsp;&nbsp;&nbsp;&nbsp;PEScrambler dispatcher VA:", c);
        wPeScramblerDispatchVa_ = new Wt::WLineEdit(c);
        wPeScramblerDispatchVa_->setToolTip(tip);

        tip = "Assume functions return if nothing can be proven about whether they return.  The \"returnability\" of "
              "a function can influence whether the disassembler looks for code following its call sites.";
        new Wt::WBreak(c);
        wAssumeFunctionsReturn_ = new Wt::WCheckBox("Assume functions return?", c);
        wAssumeFunctionsReturn_->setToolTip(tip);
        wAssumeFunctionsReturn_->setCheckState(Wt::Checked);

        new Wt::WBreak(c);
        wPartitionSpecimen_ = new Wt::WPushButton("Disassemble", c);
        wPartitionSpecimen_->clicked().connect(boost::bind(&WPartitioner::redoState, this, PartitionedSpecimen));
        new Wt::WText(" (This may take a while)", c);
    }

    adjustPanelBorders();
}

// Adjust the panel border color based on WPartitioner current state
void
WPartitioner::adjustPanelBorders() {
    for (int i=0; i<NStates; ++i) {
        if (panels_[i]) {
            Color::HSV color = i <= state_ ? panelDone : panelNotDone;
            Wt::WBorder border(Wt::WBorder::Solid, Wt::WLength(4, Wt::WLength::Pixel), toWt(color));
            panels_[i]->decorationStyle().setBorder(border);
        }
    }
}

// Makes a panel and returns its central widget. The panel itself is stored in a vector of panels.
Wt::WContainerWidget *
WPartitioner::makePanel(State state, const std::string &title, Wt::WContainerWidget *parent) {
    Wt::WPanel *panel = new Wt::WPanel(parent);
    if ((size_t)state >= panels_.size())
        panels_.resize(state+1, NULL);
    panels_[state] = panel;
    panel->setTitle(title);
    Wt::WContainerWidget *central = new Wt::WContainerWidget;
    panel->setCentralWidget(central);
    return central;
}

void
WPartitioner::changeState(State newState) {
    if (newState == state_) {
        return;
    } else if (state_ < newState) {
        while (state_ < newState) {
            bool success = false;
            State nextState = (State)(state_ + 1);      // state to cause to become in effect
            switch (nextState) {
                case InitialState:
                    ASSERT_not_reachable("InitialState must be first");
                case ParsedSpecimen:
                    success = parseSpecimen();
                    break;
                case LoadedSpecimen:
                    success = loadSpecimen();
                    break;
                case PartitionedSpecimen:
                    success = partitionSpecimen();
                    break;
                default:
                    ASSERT_not_reachable("invalid state");
            }
            if (!success)
                break;
            state_ = nextState;
        }
        adjustPanelBorders();
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
                default:
                    ASSERT_not_reachable("invalid state");
            }
            state_ = (State)(state_ - 1);
        }
        adjustPanelBorders();
    }
}

// Similar to changeState except when the state is already selected go back one additional level and then forward so the state
// change gets triggered and the action happens again.  This is typically bound to state-changing buttons.
void
WPartitioner::redoState(State newState) {
    if (newState <= state_)
        changeState((State)(newState - 1));
    changeState(newState);
}

bool
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
    return true;
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
        std::vector<SgAsmInterpretation*> interps = SageInterface::querySubTree<SgAsmInterpretation>(project);
        ASSERT_require(interpIdx < interps.size());
        ctx_.engine.interpretation(interps[interpIdx]);
    }
}

bool
WPartitioner::loadSpecimen() {
    Sawyer::Stopwatch timer;
    Sawyer::Message::Stream info(mlog[INFO] <<"load specimen");
    ctx_.engine.load(ctx_.specimenNames);
    info <<"; took " <<timer <<" seconds\n";
    specimenLoaded_.emit(true);
    return true;
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

// Updates the global WBusy widget's progress whenever a basic block is inserted into the partitioner's CFG.
class BusyUpdater: public P2::CfgAdjustmentCallback {
    WBusy *busy_;
    double lastUpdateTime_;
protected:
    BusyUpdater(WBusy *busy): busy_(busy), lastUpdateTime_(0.0) {}
public:
    typedef Sawyer::SharedPointer<BusyUpdater> Ptr;
    static Ptr instance(WBusy *busy) {
        return Ptr(new BusyUpdater(busy));
    }
    virtual bool operator()(bool chain, const AttachedBasicBlock &args) ROSE_OVERRIDE {
        static const double minimumUpdateInterval = 1.5;    // seconds
        double now = Sawyer::Message::now();
        if (now - lastUpdateTime_ >= minimumUpdateInterval) {
            lastUpdateTime_ = now;
            busy_->setValue(args.partitioner->nBytes());
        }
        return chain;
    }
    virtual bool operator()(bool chain, const DetachedBasicBlock&) ROSE_OVERRIDE { return chain; }
};

// The thread in which to run the very expensive partitioning step.  The caller must have already incremented the global WBusy
// counter, which we'll decrement when we finish.
class LaunchPartitioner {
    Context *ctx_;
    Wt::Signal<bool> *finished_;
    SgAsmInterpretation *interp_;
public:
    LaunchPartitioner(Context *ctx, Wt::Signal<bool> *finished, SgAsmInterpretation *interp)
        : ctx_(ctx), finished_(finished), interp_(interp) {}

    void operator()() {
        Sawyer::Message::Stream info(mlog[INFO]);

        // Load configuration information from files
        if (!ctx_->settings.configurationName.empty()) {
            info <<"loading configuration files";
            ctx_->busy->replaceWork("Loading configuration files...", 0);
            Sawyer::Stopwatch timer;
            size_t nItems = ctx_->engine.configureFromFile(ctx_->partitioner, ctx_->settings.configurationName);
            info <<"; " <<StringUtility::plural(nItems, "items") <<" took " <<timer <<" seconds\n";
        }

        // Disassemble and partition
        info <<"disassembling and partitioning";
        size_t expectedTotal = 0;
        BOOST_FOREACH (const MemoryMap::Node &node, ctx_->engine.memoryMap().nodes()) {
            if (0 != (node.value().accessibility() & MemoryMap::EXECUTABLE))
                expectedTotal += node.key().size();
        }
        ctx_->busy->replaceWork("Disassembling and partitioning...", expectedTotal);
        Sawyer::Stopwatch timer;
        ctx_->engine.runPartitioner(ctx_->partitioner, interp_);
        info <<"; took " <<timer <<" seconds\n";

        // Post-partitioning analysis
        info <<"running post-partitioning analyses";
        ctx_->busy->replaceWork("Post-partitioning anlaysis...", 0);
        ctx_->engine.postPartitionAnalyses(true);
        timer.start(true /*reset*/);
        ctx_->engine.updateAnalysisResults(ctx_->partitioner);
        info <<"; took " <<timer <<" seconds\n";

        // All done, update app, which is in some other thread.
        Wt::WApplication::UpdateLock lock(ctx_->application);
        if (lock) {
            finished_->emit(true);
            ctx_->busy->popWork();
        }
    }
};

bool
WPartitioner::partitionSpecimen() {
    SgAsmInterpretation *interp = ctx_.engine.interpretation();// null if no ELF/PE container

    BinaryAnalysis::Disassembler *disassembler = ctx_.engine.obtainDisassembler(isaName());
    if (NULL==disassembler) {
        wIsaError_->setText("ISA must be specified when there is no ELF/PE container.");
        wIsaError_->show();
        return false;
    }

    // Adjust some engine settings
    ctx_.engine.opaquePredicateSearch(findDeadCode());
    ctx_.engine.intraFunctionCodeSearch(followGhostEdges()==FOLLOW_LATER);
    
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
    ctx_.engine.postPartitionAnalyses(false);           // we do them explicitly in order to get timing info

    // Configure the progress bar. We need to add a work item here in the parent thread, although the child will quickly
    // override it with something more appropriate.
    ctx_.busy->pushWork("Disassembling and partitioning...", 0);
    BusyUpdater::Ptr bupper = BusyUpdater::instance(ctx_.busy);
    ctx_.partitioner.cfgAdjustmentCallbacks().prepend(bupper);

    // Run the partitioner (this could take a long time)
    LaunchPartitioner launcher(&ctx_, &specimenPartitioned_, interp);
    boost::thread thread(launcher);

    return true;
}

void
WPartitioner::undoPartitionSpecimen() {
    ctx_.partitioner = P2::Partitioner();
    specimenPartitioned_.emit(false);
}

std::string
WPartitioner::useConfiguration() const {
    if (wUseConfiguration_->checkState() != Wt::Checked)
        return "";
    return ctx_.settings.configurationName;
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

void
WPartitioner::handleUseSemantics(bool isChecked) {
    wFollowGhostEdges_->setEnabled(isChecked);
    wFollowGhostEdgesWarning_->setHidden(isChecked);
}

WPartitioner::GhostEdgeFollowing
WPartitioner::followGhostEdges() const {
    return (GhostEdgeFollowing)wFollowGhostEdges_->currentIndex();
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
    std::string str = wPeScramblerDispatchVa_->text().narrow();
    rose_addr_t va = rose_strtoull(str.c_str(), NULL, 16);
    return va;
}

bool
WPartitioner::assumeFunctionsReturn() const {
    return wAssumeFunctionsReturn_->checkState() == Wt::Checked;
}

} // namespace
