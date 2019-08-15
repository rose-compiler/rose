#include <rose.h>
#include <bROwSE/InstructionListModel.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;

namespace bROwSE {

void
InstructionListModel::changeInstructions(const std::vector<SgAsmInstruction*> &insns) {
    layoutAboutToBeChanged().emit();
    insns_ = insns;
    layoutChanged().emit();
}

void
InstructionListModel::clear() {
    layoutAboutToBeChanged().emit();
    insns_.clear();
    layoutChanged().emit();
}
    
int
InstructionListModel::rowCount(const Wt::WModelIndex &parent) const {
    return parent.isValid() ? 0 : insns_.size();
}

int
InstructionListModel::columnCount(const Wt::WModelIndex &parent) const {
    return parent.isValid() ? 0 : C_NCOLS;
}

boost::any
InstructionListModel::headerData(int column, Wt::Orientation orientation, int role) const {
    if (Wt::Horizontal == orientation && Wt::DisplayRole == role) {
        switch (column) {
            case C_ADDR:       return Wt::WString("Address");
            case C_BYTES:      return Wt::WString("Bytes");
            case C_CHARS:      return Wt::WString("ASCII");
            case C_STACKDELTA: return Wt::WString("StackDelta");
            case C_NAME:       return Wt::WString("Mnemonic");
            case C_ARGS:       return Wt::WString("Operands");
            case C_COMMENT:    return Wt::WString("Comments");
            default:           ASSERT_not_reachable("invalid column");
        }
    }
    return boost::any();
}

boost::any
InstructionListModel::data(const Wt::WModelIndex &index, int role) const {
    ASSERT_require(index.isValid());
    ASSERT_require(index.row()>=0 && (size_t)index.row() < insns_.size());
    SgAsmInstruction *insn = insns_[index.row()];
    if (Wt::DisplayRole == role) {
        switch (index.column()) {
            case C_ADDR: {
                return Wt::WString(StringUtility::addrToString(insn->get_address()));
            }
            case C_BYTES: {
                std::string s;
                for (size_t i=0; i<insn->get_raw_bytes().size(); ++i) {
                    uint8_t byte = insn->get_raw_bytes()[i];
                    char buf[32];
                    sprintf(buf, "%02x", byte);
                    s += std::string(i?" ":"") + buf;
                }
                return Wt::WString(s);
            }
            case C_CHARS: {
                std::string s;
                for (size_t i=0; i<insn->get_raw_bytes().size(); ++i) {
                    char ch = insn->get_raw_bytes()[i];
                    s += std::string(i?" ":"") + (isgraph(ch) ? std::string(1, ch) : std::string(" "));
                }
                return Wt::WString(s);
            }
            case C_STACKDELTA: {
                int64_t delta = insn->get_stackDeltaIn();
                if (delta == SgAsmInstruction::INVALID_STACK_DELTA)
                    return Wt::WString("");
                std::string s = (delta >= 0 ? "+" : "") + boost::lexical_cast<std::string>(delta);
                return Wt::WString(s);
            }
            case C_NAME: {
                return Wt::WString(unparseMnemonic(insn));
            }
            case C_ARGS: {
                std::string s;
                const RegisterDictionary *regs = ctx_.partitioner.instructionProvider().registerDictionary();
                const SgAsmExpressionPtrList &operands = insn->get_operandList()->get_operands();
                for (size_t i=0; i<operands.size(); ++i)
                    s += (i?", ":"") + unparseExpression(operands[i], NULL, regs);
                return Wt::WString(s);
            }
            case C_COMMENT: {
                return Wt::WString(insn->get_comment());
            }
            default:
                ASSERT_not_reachable("invalid column number");
        }
    }
    return boost::any();
}

} // namespace
