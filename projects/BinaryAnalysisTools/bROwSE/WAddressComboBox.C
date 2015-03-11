#include <bROwSE/WAddressComboBox.h>
#include <Wt/WComboBox>
#include <Wt/WPushButton>
#include <Wt/WText>

namespace bROwSE {

void
WAddressComboBox::init(const Wt::WString &title) {
    if (!title.empty())
        wTitle_ = new Wt::WText(title, this);
    wComboBox_ = new Wt::WComboBox(this);
    wComboBox_->sactivated().connect(this, &WAddressComboBox::handleAddressActivated);
}

size_t
WAddressComboBox::addButton(const Wt::WString &label) {
    size_t id = buttons_.size();
    buttons_.push_back(ButtonInfo(label, id));
    return id;
}

void
WAddressComboBox::clear() {
    map_.clear();
}

void
WAddressComboBox::insertAddress(rose_addr_t va, Wt::WString label) {
    if (label.empty())
        label = StringUtility::addrToString(va);
    map_.insert(label, va);
}

void
WAddressComboBox::insertAst(SgNode *ast, const AddressIntervalSet &restrict) {
    struct T1: AstSimpleProcessing {
        WAddressComboBox *self;
        const AddressIntervalSet &restrict;
        T1(WAddressComboBox *self, const AddressIntervalSet &restrict): self(self), restrict(restrict) {}
        void visit(SgNode *node) {
            if (SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(node)) {
                rose_addr_t va = ival->get_absoluteValue();
                if (!restrict.isEmpty() && restrict.contains(va))
                    self->insertAddress(va);
            }
        }
    } t1(this, restrict);
    t1.traverse(ast, preorder);
}

void
WAddressComboBox::insertBasicBlock(const P2::Partitioner &partitioner, const P2::BasicBlock::Ptr &bblock) {
    // Restrict addresses to those which are mapped.
    AddressIntervalSet mapped(partitioner.memoryMap());

    // Addresses for the basic block instructions (these should all be mapped, so no need to check)
    AddressIntervalSet bblockVas = partitioner.basicBlockInstructionExtent(bblock);
    BOOST_FOREACH (const AddressInterval &interval, bblockVas.intervals())
        insertAddress(interval.least());

    // Addresses for constants mentioned in those instructions
    BOOST_FOREACH (SgAsmInstruction *insn, bblock->instructions())
        insertAst(insn, mapped);
}

void
WAddressComboBox::redraw() {
    if (!buttonsCreated_) {
        buttonsCreated_ = true;
        for (size_t i=0; i<buttons_.size(); ++i) {
            if (!buttons_[i].wButton) {
                if (buttons_[i].label.empty())
                    buttons_[i].label = "Go";
                buttons_[i].wButton = new Wt::WPushButton(buttons_[i].label, this);
            }
            buttons_[i].wButton->clicked().connect(boost::bind(&WAddressComboBox::handleButtonClicked, this, i));
        }
    }
    wComboBox_->clear();
    BOOST_FOREACH (const Wt::WString &label, map_.keys())
        wComboBox_->addItem(label);
}

void
WAddressComboBox::handleButtonClicked(size_t buttonIdx) {
    rose_addr_t va = 0;
    Wt::WString label = wComboBox_->currentText();
    if (map_.getOptional(label).assignTo(va))
        clicked_.emit(va, label, buttonIdx);
}

void
WAddressComboBox::handleAddressActivated(const Wt::WString &label) {
    rose_addr_t va = 0;
    if (map_.getOptional(label).assignTo(va))
        clicked_.emit(va, label, (size_t)-1);
}


} // namespace
