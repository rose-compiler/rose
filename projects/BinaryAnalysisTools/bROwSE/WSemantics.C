#include <rose.h>

#include <BinaryStackVariable.h>                        // ROSE
#include <BinarySymbolicExpr.h>                         // ROSE
#include <bROwSE/FunctionUtil.h>
#include <bROwSE/WSemantics.h>
#include <bROwSE/WToggleButton.h>
#include <MemoryCellList.h>                             // ROSE
#include <Wt/WAbstractTableModel>
#include <Wt/WHBoxLayout>
#include <Wt/WPanel>
#include <Wt/WTableView>
#include <Wt/WText>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Rose::BinaryAnalysis::InstructionSemantics2;

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
        SymbolicSemantics::SValuePtr value = SymbolicSemantics::SValue::promote(valueBase);
        SymbolicExpr::LeafPtr leaf = value->get_expression()->isLeafNode();
        if (leaf && leaf->isNumber()) {
            if (1==leaf->nBits()) {
                return leaf->toInt() ? "true" : "false";
            } else if (leaf->nBits()<=64) {
                uint64_t v = leaf->toInt();
                return StringUtility::toHex2(v, leaf->nBits());
            } else {
                return leaf->bits().toHex();
            }
        }
        return "? " + StringUtility::plural(valueBase->get_width(), "bits");
    }

    void reload(const BaseSemantics::StatePtr &state, WSemantics::Mode mode, const FunctionDataFlow &dfInfo) {
        using namespace Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;
        layoutAboutToBeChanged().emit();
        locValPairs_.clear();
        if (state) {
            switch (mode) {
                case WSemantics::REG_INIT:
                case WSemantics::REG_FINAL: {
                    RegisterStateGenericPtr regState = RegisterStateGeneric::promote(state->registerState());
                    RegisterNames regName(regState->get_register_dictionary());
                    BOOST_FOREACH (const RegisterStateGeneric::RegPair &reg_val, regState->get_stored_registers()) {
                        RegisterDescriptor reg = reg_val.desc;

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
                    ops->currentState(state->clone());

#if 1 // DEBUGGING [Robb Matzke 2015-01-13]
                    {
                        std::cerr <<"ROBB: full memory state (reverse chronological order):\n";
                        MemoryCellListPtr memState = MemoryCellList::promote(state->memoryState());
                        BOOST_FOREACH (const MemoryCellPtr &cell, memState->get_cells()) {
                            std::ostringstream s1, s2;
                            s1 <<*cell->get_address();
                            s2 <<*cell->get_value();
                            std::cerr <<"        " <<s1.str() <<" = " <<s2.str() <<"\n";
                        }
                    }
#endif

                    // Stack pointer at the very start of this function.
                    const RegisterDescriptor SP = ctx_.partitioner.instructionProvider().stackPointerRegister();
                    SValuePtr stackPtr = dfInfo.initialStates[0]->readRegister(SP, ops->undefined_(SP.nBits()), ops.get());
                    const RegisterDescriptor SS = ctx_.partitioner.instructionProvider().stackSegmentRegister();

                    // Function arguments
                    BOOST_FOREACH (const StackVariable &var, P2::DataFlow::findFunctionArguments(ops, stackPtr)) {
                        char name[64];
                        sprintf(name, "arg_%" PRIx64, var.location.offset);
                        SValuePtr value = ops->undefined_(8*var.location.nBytes);
                        value = ops->readMemory(SS, var.location.address, value, ops->boolean_(true));
                        std::string valueStr = toString(value);
                        if (!valueStr.empty())
                            locValPairs_.push_back(LocationValue(name, valueStr));
                    }

                    // Function local variables
                    BOOST_FOREACH (const StackVariable &var, P2::DataFlow::findLocalVariables(ops, stackPtr)) {
                        char name[64];
                        sprintf(name, "var_%" PRIx64, -var.location.offset);
                        SValuePtr value = ops->undefined_(8*var.location.nBytes);
                        value = ops->readMemory(SS, var.location.address, value, ops->boolean_(true));
                        std::string valueStr = toString(value);
                        if (!valueStr.empty())
                            locValPairs_.push_back(LocationValue(name, valueStr));
                    }

                    // Global variables
                    ASSERT_require(SP.nBits() % 8 == 0);
                    const size_t wordNBytes = SP.nBits() / 8;
                    BOOST_FOREACH (const AbstractLocation &var, P2::DataFlow::findGlobalVariables(ops, wordNBytes)) {
                        if (var.isAddress() && var.nBytes()>0 &&
                            var.getAddress()->is_number() && var.getAddress()->get_width()<=64) {
                            rose_addr_t va = var.getAddress()->get_number();
                            std::string name = ctx_.partitioner.addressName(va);
                            if (name.empty())
                                name = StringUtility::addrToString(va);
                            SValuePtr value = ops->undefined_(8*var.nBytes());
                            value = ops->readMemory(SS, var.getAddress(), value, ops->boolean_(true));
                            std::string valueStr = toString(value);
                            if (!valueStr.empty())
                                locValPairs_.push_back(LocationValue(name, valueStr));
                        }
                    }
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
                        return Wt::WString("Location");
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

    Wt::WContainerWidget *wPanelCenter = new Wt::WContainerWidget;

    // Info about what's being displayed
    wAddress_ = new Wt::WText("No block", wPanelCenter);

    // The buttons to select which semantics are displayed.
    Wt::WContainerWidget *buttons = new Wt::WContainerWidget(wPanelCenter);
    Wt::WHBoxLayout *buttonBox = new Wt::WHBoxLayout;
    buttons->setLayout(buttonBox);
    wRegInit_ = new WToggleButton("/images/semantics-reg-top-24x24.png", "/images/semantics-reg-top-pressed-24x24.png");
    wRegInit_->clicked().connect(boost::bind(&WSemantics::changeMode, this, REG_INIT));
    buttonBox->addWidget(wRegInit_);

    wRegFinal_ = new WToggleButton("/images/semantics-reg-bottom-24x24.png", "/images/semantics-reg-bottom-pressed-24x24.png");
    wRegFinal_->clicked().connect(boost::bind(&WSemantics::changeMode, this, REG_FINAL));
    buttonBox->addWidget(wRegFinal_);

    wMemInit_ = new WToggleButton("/images/semantics-mem-top-24x24.png", "/images/semantics-mem-top-pressed-24x24.png");
    wMemInit_->clicked().connect(boost::bind(&WSemantics::changeMode, this, MEM_INIT));
    buttonBox->addWidget(wMemInit_);
    
    wMemFinal_ = new WToggleButton("/images/semantics-mem-bottom-24x24.png", "/images/semantics-mem-bottom-pressed-24x24.png");
    wMemFinal_->clicked().connect(boost::bind(&WSemantics::changeMode, this, MEM_FINAL));
    buttonBox->addWidget(wMemFinal_);

    // The table showing the semantics
    wTableView_ = new Wt::WTableView(wPanelCenter);
    wTableView_->setModel(model_);
    wTableView_->setRowHeaderCount(1);                  // must be first
    wTableView_->setHeaderHeight(28);
    wTableView_->setSortingEnabled(false);
    wTableView_->setAlternatingRowColors(true);
    wTableView_->setColumnResizeEnabled(true);
    wTableView_->setEditTriggers(Wt::WAbstractItemView::NoEditTrigger);
    wTableView_->setColumnWidth(0, Wt::WLength(10, Wt::WLength::FontEm));
    wTableView_->setColumnWidth(1, Wt::WLength(15, Wt::WLength::FontEm));

    // A frame to hold all the semantics info
    Wt::WPanel *wPanel = new Wt::WPanel(this);
    wPanel->setTitle("Semantics");
    wPanel->resize(300, Wt::WLength::Auto);
    wPanel->setCentralWidget(wPanelCenter);
}

void
WSemantics::changeBasicBlock(const P2::BasicBlock::Ptr &bblock, Mode mode) {
    if (bblock_ == bblock && mode_ == mode)
        return;
    bblock_ = bblock;
    mode_ = mode;

#if 0 // [Robb Matzke 2015-12-14]: old code, assuming basic blocks are not shared
    function_ = bblock_ ? ctx_.partitioner.basicBlockFunctionOwner(bblock) : P2::Function::Ptr();
#else // Partly fixed code, still assuming basic blocks are not shared.
    if (function_) {
        if (bblock_ == NULL || !function_->ownsBasicBlock(bblock_->address()))
            function_ = P2::Function::Ptr();
    } else {
        BOOST_FOREACH (const P2::Function::Ptr &f, ctx_.partitioner.functionsOwningBasicBlock(bblock)) {
            function_ = f;                              // chosen arbitrarily as the first one, if any
            break;
        }
    }
#endif
        
    if (bblock && function_) {
        typedef BaseSemantics::RegisterStateGeneric RegState;
        typedef BaseSemantics::MemoryCellList MemState;
        FunctionDataFlow df = functionDataFlow(ctx_.partitioner, function_);
        if (!df.error.empty()) {
            wAddress_->setText("Error: " + df.error);
        } else {
            wAddress_->setText("No block");
        }
        
        RegisterDescriptor SP = ctx_.partitioner.instructionProvider().stackPointerRegister();
        BaseSemantics::RiscOperatorsPtr ops = ctx_.partitioner.newOperators();
        BaseSemantics::SValuePtr initialStackPointer =
            df.initialStates[0]->readRegister(SP, ops->undefined_(SP.nBits()), ops.get());

        BOOST_FOREACH (const P2::DataFlow::DfCfg::Vertex &vertex, df.dfCfg.vertices()) {
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
                if (df.error.empty())
                    wAddress_->setText(bblock->printableName());
                return;                                 // no need to search for futher vertex
            }
        }
    }

    model_->clear();
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
    wRegInit_ ->setState(REG_INIT ==mode ? WToggleButton::Depressed : WToggleButton::Normal);
    wRegFinal_->setState(REG_FINAL==mode ? WToggleButton::Depressed : WToggleButton::Normal);
    wMemInit_ ->setState(MEM_INIT ==mode ? WToggleButton::Depressed : WToggleButton::Normal);
    wMemFinal_->setState(MEM_FINAL==mode ? WToggleButton::Depressed : WToggleButton::Normal);
    changeBasicBlock(bblock_, mode);
}

} // namespace
