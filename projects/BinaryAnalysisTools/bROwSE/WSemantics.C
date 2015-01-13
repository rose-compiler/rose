#include <sage3basic.h>

#include <bROwSE/FunctionUtil.h>
#include <bROwSE/WSemantics.h>
#include <bROwSE/WToggleButton.h>
#include <InsnSemanticsExpr.h>                          // ROSE
#include <Wt/WAbstractTableModel>
#include <Wt/WHBoxLayout>
#include <Wt/WPanel>
#include <Wt/WTableView>

using namespace rose::BinaryAnalysis::InstructionSemantics2;

namespace bROwSE {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Model that provides names and values for abstract locations based on a semantic state so they can be displayed in a table
// view.
class AbstractLocationModel: public Wt::WAbstractTableModel {
public:
    enum ColumnNumber { LocationColumn, ValueColumn, NColumns };

    struct LocationValue {
        std::string location;
        std::string value;
        LocationValue() {}
        LocationValue(const std::string &location, const std::string &value): location(location), value(value) {}
    };

    typedef std::vector<LocationValue> LocValPairs;

    Context &ctx_;
    LocValPairs locValPairs_;

    AbstractLocationModel(Context &ctx): ctx_(ctx) {}

    void clear() {
        if (!locValPairs_.empty()) {
            layoutAboutToBeChanged().emit();
            locValPairs_.clear();
            layoutChanged().emit();
        }
    }

    std::string toString(const BaseSemantics::SValuePtr &valueBase) {
        using namespace rose::BinaryAnalysis::InsnSemanticsExpr;
        SymbolicSemantics::SValuePtr value = SymbolicSemantics::SValue::promote(valueBase);
        LeafNodePtr leaf = value->get_expression()->isLeafNode();
        if (leaf && leaf->is_known()) {
            if (1==leaf->get_nbits())
                return leaf->get_value() ? "true" : "false";
            return leaf->get_bits().toHex();
        }
        return "?";
    }

    void reload(const P2::DataFlow::State::Ptr &state, WSemantics::Mode mode, const FunctionDataFlow &dfInfo) {
        using namespace rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;
        layoutAboutToBeChanged().emit();
        locValPairs_.clear();
        if (state) {
            switch (mode) {
                case WSemantics::REG_INIT:
                case WSemantics::REG_FINAL: {
                    RegisterStateGenericPtr regState =
                        RegisterStateGeneric::promote(state->semanticState()->get_register_state());
                    RegisterNames regName(regState->get_register_dictionary());
                    BOOST_FOREACH (const RegisterStateGeneric::RegPair &reg_val, regState->get_stored_registers()) {
                        const RegisterDescriptor &reg = reg_val.desc;

                        // No need to show the instruction pointer since it's obvious; no need to show the stack pointer since the
                        // deltas are already shown.
                        if (reg == ctx_.partitioner.instructionProvider().stackPointerRegister() ||
                            reg == ctx_.partitioner.instructionProvider().instructionPointerRegister())
                            continue;

                        std::string valStr = toString(SymbolicSemantics::SValue::promote(reg_val.value));
                        if (!valStr.empty())
                            locValPairs_.push_back(LocationValue(regName(reg), valStr));
                    }
                    break;
                }

                case WSemantics::MEM_INIT:
                case WSemantics::MEM_FINAL: {
                    // RiscOperators for accessing multi-byte memory. We access a clone of the memory state so we're not
                    // affecting things like latestWriter addresses in the real state.
                    RiscOperatorsPtr ops = ctx_.partitioner.newOperators();
                    ops->set_state(state->semanticState()->clone());

                    // Stack pointer at the very start of this function.
                    const RegisterDescriptor SP = ctx_.partitioner.instructionProvider().stackPointerRegister();
                    SValuePtr stackPtr = dfInfo.initialStates[0]->semanticState()->readRegister(SP, ops.get());
                    const RegisterDescriptor SS = ctx_.partitioner.instructionProvider().stackSegmentRegister();

                    // Function arguments
                    BOOST_FOREACH (const P2::DataFlow::StackVariable &var, state->findFunctionArguments(stackPtr)) {
                        char name[64];
                        sprintf(name, "arg_%"PRIx64, var.offset);
                        SValuePtr value = ops->undefined_(8*var.nBytes);
                        value = ops->readMemory(SS, var.address, value, ops->boolean_(true));
                        std::string valueStr = toString(value);
                        if (!valueStr.empty())
                            locValPairs_.push_back(LocationValue(name, valueStr));
                    }

                    // Function local variables
                    BOOST_FOREACH (const P2::DataFlow::StackVariable &var, state->findLocalVariables(stackPtr)) {
                        char name[64];
                        sprintf(name, "var_%"PRIx64, -var.offset);
                        SValuePtr value = ops->undefined_(8*var.nBytes);
                        value = ops->readMemory(SS, var.address, value, ops->boolean_(true));
                        std::string valueStr = toString(value);
                        if (!valueStr.empty())
                            locValPairs_.push_back(LocationValue(name, valueStr));
                    }

                    // Global variables
                    // FIXME[Robb P. Matzke 2015-01-13]: not implemented yet
                    break;
                }
            }
        }
        layoutChanged().emit();
    }
    
    virtual int rowCount(const Wt::WModelIndex &parent=Wt::WModelIndex()) const ROSE_OVERRIDE {
        return parent.isValid() ? 0 : locValPairs_.size();
    }

    virtual int columnCount(const Wt::WModelIndex &parent=Wt::WModelIndex()) const ROSE_OVERRIDE {
        return parent.isValid() ? 0 : NColumns;
    }

    virtual boost::any headerData(int column, Wt::Orientation orientation=Wt::Horizontal,
                                  int role=Wt::DisplayRole) const ROSE_OVERRIDE {
        if (Wt::Horizontal == orientation) {
            if (Wt::DisplayRole == role) {
                switch (column) {
                    case LocationColumn:
                        return Wt::WString("ALoc");
                    case ValueColumn:
                        return Wt::WString("Value");
                    default:
                        ASSERT_not_reachable("invalid column number");
                }
            }
        }
        return boost::any();
    }

    virtual boost::any data(const Wt::WModelIndex &index, int role=Wt::DisplayRole) const ROSE_OVERRIDE {
        ASSERT_require(index.isValid());
        ASSERT_require(index.row()>=0 && (size_t)index.row()<locValPairs_.size());
        if (Wt::DisplayRole == role) {
            switch (index.column()) {
                case LocationColumn:
                    return Wt::WString(locValPairs_[index.row()].location);
                case ValueColumn:
                    return Wt::WString(locValPairs_[index.row()].value);
                default:
                    ASSERT_not_reachable("invalid column number");
            }
        }
        return boost::any();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Widget
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
WSemantics::init() {
    model_ = new AbstractLocationModel(ctx_);

    Wt::WContainerWidget *wPanelCenter = new Wt::WContainerWidget(this);

    // The buttons to select which semantics are displayed.
    Wt::WContainerWidget *buttons = new Wt::WContainerWidget(wPanelCenter);
    Wt::WHBoxLayout *buttonBox = new Wt::WHBoxLayout;
    buttons->setLayout(buttonBox);
    WToggleButton *wRegInit = new WToggleButton("/images/semantics-reg-init-16x16.png");
    wRegInit->clicked().connect(boost::bind(&WSemantics::changeMode, this, REG_INIT));
    buttonBox->addWidget(wRegInit);
    WToggleButton *wRegFinal = new WToggleButton("/images/semantics-reg-final-16x16.png");
    wRegFinal->clicked().connect(boost::bind(&WSemantics::changeMode, this, REG_FINAL));
    buttonBox->addWidget(wRegFinal);
    WToggleButton *wMemInit = new WToggleButton("/images/semantics-mem-init-16x16.png");
    wMemInit->clicked().connect(boost::bind(&WSemantics::changeMode, this, MEM_INIT));
    buttonBox->addWidget(wMemInit);
    WToggleButton *wMemFinal = new WToggleButton("/images/semantics-mem-final-16x16.png");
    wMemFinal->clicked().connect(boost::bind(&WSemantics::changeMode, this, MEM_FINAL));
    buttonBox->addWidget(wMemFinal);

    // The table showing the semantics
    wTableView_ = new Wt::WTableView(wPanelCenter);
    wTableView_->setModel(model_);
    wTableView_->setRowHeaderCount(1);                  // must be first
    wTableView_->setHeaderHeight(28);
    wTableView_->setSortingEnabled(false);
    wTableView_->setAlternatingRowColors(true);
    wTableView_->setColumnResizeEnabled(true);
    wTableView_->setEditTriggers(Wt::WAbstractItemView::NoEditTrigger);
    wTableView_->setColumnWidth(0, Wt::WLength(3, Wt::WLength::FontEm));
    wTableView_->setColumnWidth(1, Wt::WLength(5, Wt::WLength::FontEm));

    // A frame to hold all the semantics info
    Wt::WPanel *wPanel = new Wt::WPanel(this);
    wPanel->setTitle("Semantics");
    wPanel->resize(200, Wt::WLength::Auto);
    wPanel->setCentralWidget(wPanelCenter);
}

void
WSemantics::changeBasicBlock(const P2::BasicBlock::Ptr &bblock, Mode mode) {
    if (bblock_ == bblock && mode_ == mode)
        return;
    bblock_ = bblock;
    mode_ = mode;
    function_ = bblock_ ? ctx_.partitioner.basicBlockFunctionOwner(bblock) : P2::Function::Ptr();

    if (function_) {
        typedef BaseSemantics::RegisterStateGeneric RegState;
        typedef BaseSemantics::MemoryCellList MemState;
        FunctionDataFlow df = functionDataFlow(ctx_.partitioner, function_);

        const RegisterDescriptor &SP = ctx_.partitioner.instructionProvider().stackPointerRegister();
        BaseSemantics::RiscOperatorsPtr ops = ctx_.partitioner.newOperators();
        BaseSemantics::SValuePtr initialStackPointer = df.initialStates[0]->semanticState()->readRegister(SP, ops.get());

        BOOST_FOREACH (const P2::DataFlow::DfCfg::VertexNode &vertex, df.dfCfg.vertices()) {
            if (vertex.value().type() == P2::DataFlow::DfCfgVertex::BBLOCK && vertex.value().bblock() == bblock) {
                switch (mode) {
                    case REG_INIT:
                    case MEM_INIT:
                        model_->reload(df.initialStates[vertex.id()], mode, df);
                        break;
                    case REG_FINAL:
                    case MEM_FINAL:
                        model_->reload(df.finalStates[vertex.id()], mode, df);
                        break;
                }
                break;                                  // no need to search for futher vertex
            }
        }
    } else {
        model_->clear();
    }
}

void
WSemantics::changeBasicBlock(const P2::BasicBlock::Ptr &bblock) {
    changeBasicBlock(bblock, mode_);
}

void
WSemantics::changeFunction(const P2::Function::Ptr &function) {
    changeBasicBlock(P2::BasicBlock::Ptr());
}

void
WSemantics::changeMode(Mode mode) {
    changeBasicBlock(bblock_, mode);
}

} // namespace
