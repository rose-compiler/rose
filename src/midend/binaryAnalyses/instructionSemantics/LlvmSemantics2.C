#include "sage3basic.h"
#include "LlvmSemantics2.h"
#include "RoseAst.h"
#include "AsmUnparser_compat.h"

namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace LlvmSemantics {

BaseSemantics::SValuePtr
RiscOperators::readMemory(const RegisterDescriptor &segreg, const BaseSemantics::SValuePtr &addr_,
                          const BaseSemantics::SValuePtr &cond, size_t nbits)
{
    SValuePtr addr = SValue::promote(addr_);
    return svalue_expr(InternalNode::create(nbits, InsnSemanticsExpr::OP_READ,
                                            LeafNode::create_memory(nbits), addr->get_expression()));
}

void
RiscOperators::writeMemory(const RegisterDescriptor &segreg, const BaseSemantics::SValuePtr &addr_,
                           const BaseSemantics::SValuePtr &data_, const BaseSemantics::SValuePtr &cond)
{
    SValuePtr addr = SValue::promote(addr_);
    SValuePtr data = SValue::promote(data_);
    mem_writes.push_back(InternalNode::create(data->get_width(), InsnSemanticsExpr::OP_WRITE,
                                              LeafNode::create_memory(data->get_width()),
                                              addr->get_expression(), data->get_expression())->isInternalNode());
}

void
RiscOperators::reset()
{
    BaseSemantics::StatePtr state = get_state();
    BaseSemantics::RegisterStatePtr regs = state->get_register_state();
    BaseSemantics::MemoryStatePtr mem = state->get_memory_state();

    BaseSemantics::RegisterStatePtr new_regs = regs->create(get_protoval(), regs->get_register_dictionary());
    BaseSemantics::MemoryStatePtr new_mem = mem->create(get_protoval());
    BaseSemantics::StatePtr new_state = state->create(new_regs, new_mem);

    set_state(new_state);
    rewrites.clear();
    prev_regstate.reset();
    mem_writes.clear();
}

void
RiscOperators::emit_changed_state(std::ostream &o)
{
    const RegisterDictionary *dictionary = get_state()->get_register_state()->get_register_dictionary();
    RegisterDescriptors modified_registers = get_modified_registers();
    emit_prerequisites(o, modified_registers, dictionary);
    emit_register_definitions(o, modified_registers);
    emit_memory_writes(o);
    make_current();
}

std::string
RiscOperators::prefix() const 
{
    std::string retval = "";
    for (size_t i=0; i<indent_level; ++i)
        retval += indent_string;
    return retval;
}

// FIXME[Robb P. Matzke 2014-01-07]: this is x86 specific.
const RegisterDescriptors &
RiscOperators::get_important_registers()
{
    if (important_registers.empty()) {
        assert(get_state()!=NULL);
        const RegisterDictionary *dictionary = get_state()->get_register_state()->get_register_dictionary();

        // General-purpose registers
        important_registers.push_back(*dictionary->lookup("eax"));
        important_registers.push_back(*dictionary->lookup("ebx"));
        important_registers.push_back(*dictionary->lookup("ecx"));
        important_registers.push_back(*dictionary->lookup("edx"));
        important_registers.push_back(*dictionary->lookup("esp"));
        important_registers.push_back(*dictionary->lookup("ebp"));
        important_registers.push_back(*dictionary->lookup("esi"));
        important_registers.push_back(*dictionary->lookup("edi"));

        // Segment registers
        important_registers.push_back(*dictionary->lookup("cs"));
        important_registers.push_back(*dictionary->lookup("ds"));
        important_registers.push_back(*dictionary->lookup("ss"));
        important_registers.push_back(*dictionary->lookup("es"));
        important_registers.push_back(*dictionary->lookup("fs"));
        important_registers.push_back(*dictionary->lookup("gs"));

        // Treat EFLAGS as individual bits
        important_registers.push_back(*dictionary->lookup("cf"));
        important_registers.push_back(*dictionary->lookup("pf"));
        important_registers.push_back(*dictionary->lookup("af"));
        important_registers.push_back(*dictionary->lookup("zf"));
        important_registers.push_back(*dictionary->lookup("sf"));
        important_registers.push_back(*dictionary->lookup("tf"));
        important_registers.push_back(*dictionary->lookup("if"));
        important_registers.push_back(*dictionary->lookup("df"));
        important_registers.push_back(*dictionary->lookup("of"));
        important_registers.push_back(*dictionary->lookup("nt"));
        important_registers.push_back(*dictionary->lookup("iopl"));
        important_registers.push_back(*dictionary->lookup("rf"));
        important_registers.push_back(*dictionary->lookup("vm"));
        important_registers.push_back(*dictionary->lookup("vif"));
        important_registers.push_back(*dictionary->lookup("vip"));
        important_registers.push_back(*dictionary->lookup("id"));
    }
    return important_registers;
}

// Important registers that have a value
RegisterDescriptors
RiscOperators::get_stored_registers()
{
    RegisterDescriptors retval;
    RegisterStatePtr regstate = RegisterState::promote(get_state()->get_register_state());
    const RegisterDictionary *dictionary = regstate->get_register_dictionary();
    const std::vector<RegisterDescriptor> &regs = get_important_registers();
    for (size_t i=0; i<regs.size(); ++i) {
        if (regstate->is_partly_stored(regs[i])) {
            const std::string &name = dictionary->lookup(regs[i]);
            assert(!name.empty());
            SValuePtr value = SValue::promote(regstate->readRegister(regs[i], this));

            // Sometimes registers only have a value because they've been read.  There is no need to emit a definition for
            // these variables.  By convention, the RegisterStateGeneric will add the register name + "_0" to all registers
            // initialized this way.
            std::string comment = value->get_comment();
            if (0!=comment.compare(name + "_0"))
                retval.push_back(regs[i]);
        }
    }
    return retval;
}

// Important registers whose value has changed recently
RegisterDescriptors
RiscOperators::get_modified_registers()
{
    RegisterDescriptors retval;
    RegisterStatePtr cur_regstate = RegisterState::promote(get_state()->get_register_state());
    const RegisterDictionary *dictionary = cur_regstate->get_register_dictionary();
    const std::vector<RegisterDescriptor> &regs = get_important_registers();
    for (size_t i=0; i<regs.size(); ++i) {
        if (cur_regstate->is_partly_stored(regs[i])) {
            const std::string &name = dictionary->lookup(regs[i]);
            assert(!name.empty());
            SValuePtr cur_value = SValue::promote(cur_regstate->readRegister(regs[i], this));
            if (0==cur_value->get_comment().compare(name + "_0")) {
                // This register has it's initial value, probably because it was read (registers that have never been read or
                // written won't even get this far in the loop due to the is_partly_stored() check above.
                continue;
            } else if (prev_regstate!=NULL && prev_regstate->is_partly_stored(regs[i])) {
                SValuePtr prev_value = SValue::promote(prev_regstate->readRegister(regs[i], this));
                if (cur_value->must_equal(prev_value))
                    continue;
            }
            retval.push_back(regs[i]);
        }
    }
    return retval;
}

// FIXME[ROBB P. MATZKE 2014-01-07]: This is x86 specific.
SValuePtr
RiscOperators::get_instruction_pointer()
{
    const RegisterDictionary *dictionary = get_state()->get_register_state()->get_register_dictionary();
    RegisterDescriptor EIP = *dictionary->lookup("eip");
    return SValue::promote(get_state()->get_register_state()->readRegister(EIP, this));
}

// Create temporary LLVM variables for all definers of the specified registers.
void
RiscOperators::emit_prerequisites(std::ostream &o, const RegisterDescriptors &regs, const RegisterDictionary *dictionary)
{
    struct T1: InsnSemanticsExpr::Visitor {
        RiscOperators *ops;
        std::ostream &o;
        const RegisterDescriptors &regs;
        const RegisterDictionary *dictionary;
        std::set<uint64_t> seen;
        T1(RiscOperators *ops, std::ostream &o, const RegisterDescriptors &regs, const RegisterDictionary *dictionary)
            : ops(ops), o(o), regs(regs), dictionary(dictionary) {}
        void operator()(const TreeNodePtr &node) {
            if (!seen.insert(node->hash()).second)
                return;                                 // already processed this same expression
            size_t width = node->get_nbits();
            const std::string &comment = node->get_comment();
            if (InternalNodePtr inode = node->isInternalNode()) {
                if (InsnSemanticsExpr::OP_READ==inode->get_operator()) {
                    assert(2==inode->size());
                    ops->emit_assignment(o, ops->emit_memory_read(o, inode->child(1), width));
                }
            } else if (LeafNodePtr leaf = node->isLeafNode()) {
                if (leaf->is_variable() && comment.size()>2 && 0==comment.substr(comment.size()-2).compare("_0")) {
                    std::string register_name = comment.substr(0, comment.size()-2);
                    const RegisterDescriptor *reading = dictionary->lookup(register_name);
                    if (reading && std::find(regs.begin(), regs.end(), *reading)!=regs.end()) {
                        // We're reading from a register which is also one of the registers whose definition will eventually be
                        // emitted, so treat this read as a prerequisite.  On the other hand, if we're reading a register that
                        // is not one of the specified registers then there's no point in emitting a temporary variable for
                        // this register.
                        ops->emit_assignment(o, leaf);
                    }
                }
            }
        }
    } t1(this, o, regs, dictionary);

    RegisterStatePtr regstate = RegisterState::promote(get_state()->get_register_state());
    for (size_t i=0; i<regs.size(); ++i) {
        SValuePtr value = SValue::promote(regstate->readRegister(regs[i], this));
        value->get_expression()->depth_first_visit(&t1);
    }
}

void
RiscOperators::emit_register_declarations(std::ostream &o, const RegisterDescriptors &regs)
{
    const RegisterDictionary *dictionary = get_state()->get_register_state()->get_register_dictionary();
    for (size_t i=0; i<regs.size(); ++i) {
        const std::string &name = dictionary->lookup(regs[i]);
        assert(!name.empty());
        o <<prefix() <<"@" <<name <<"\n";
    }
}

void
RiscOperators::emit_register_definitions(std::ostream &o, const RegisterDescriptors &regs)
{
    RegisterStatePtr regstate = RegisterState::promote(get_state()->get_register_state());
    const RegisterDictionary *dictionary = regstate->get_register_dictionary();
    for (size_t i=0; i<regs.size(); ++i) {
        const std::string &name = dictionary->lookup(regs[i]);
        assert(!name.empty());
        SValuePtr value = SValue::promote(regstate->readRegister(regs[i], this));
        o <<prefix() <<"; register " <<name <<" = " <<*value <<"\n";
        TreeNodePtr t1 = emit_expression(o, value);
        o <<prefix() <<"@" <<name <<" = " <<llvm_integer_type(t1->get_nbits()) <<" " << llvm_term(t1) <<"\n";
    }
}

void
RiscOperators::emit_next_eip(std::ostream &o)
{
    SValuePtr eip = get_instruction_pointer();
    if (eip->is_number()) {
        o <<prefix() <<"br label " <<addr_label(eip->get_number()) <<"\n";
        return;
    }

    InternalNodePtr inode = eip->get_expression()->isInternalNode();
    if (inode && InsnSemanticsExpr::OP_ITE==inode->get_operator()) {
        LeafNodePtr true_leaf = inode->child(1)->isLeafNode();
        LeafNodePtr false_leaf = inode->child(2)->isLeafNode();
        if (true_leaf!=NULL && true_leaf->is_known() && false_leaf!=NULL && false_leaf->is_known()) {
            LeafNodePtr t1 = emit_expression(o, inode->child(0));
            o <<prefix() <<"br i1 " <<llvm_term(t1)
              <<", label " <<addr_label(true_leaf->get_value())
              <<", label " <<addr_label(false_leaf->get_value()) <<"\n";
            return;
        }
    }

    o <<prefix() <<"; EIP = " <<*eip <<"\n";
    LeafNodePtr t1 = emit_expression(o, eip);
    o <<prefix() <<"indirectbr i8* " <<llvm_term(t1) <<"\n";
}

void
RiscOperators::emit_memory_writes(std::ostream &o)
{
    for (size_t i=0; i<mem_writes.size(); ++i) {
        InternalNodePtr inode = mem_writes[i]->isInternalNode();
        assert(inode!=NULL);
        assert(inode->get_operator() == InsnSemanticsExpr::OP_WRITE);
        assert(inode->size()==3);
        LeafNodePtr addr = emit_expression(o, inode->child(1));
        LeafNodePtr value = emit_expression(o, inode->child(2));
        std::string type = llvm_integer_type(value->get_nbits());
        o <<prefix() <<"store " <<type <<" " <<llvm_term(value) <<", " <<type <<"* " <<llvm_term(addr) <<"\n";
    }
}

void
RiscOperators::make_current()
{
    prev_regstate = RegisterState::promote(get_state()->get_register_state()->clone());
    mem_writes.clear();
}

LeafNodePtr
RiscOperators::emit_expression(std::ostream &o, const SValuePtr &value)
{
    assert(value!=NULL);
    LeafNodePtr result = emit_expression(o, value->get_expression());
    assert(result!=NULL);
    return result;
}

std::string
RiscOperators::llvm_integer_type(size_t width)
{
    assert(width>0);
    return "i" + StringUtility::numberToString(width);
}

std::string
RiscOperators::llvm_term(const TreeNodePtr &expr)
{
    LeafNodePtr leaf = expr->isLeafNode();
    assert(leaf!=NULL);
    if (leaf->is_known())
        return StringUtility::numberToString(leaf->get_value());
    if (leaf->is_variable()) {
        std::string comment = leaf->get_comment();
        if (comment.size()>2 && 0==comment.substr(comment.size()-2).compare("_0")) {
            // This is an initial value for a register, so we'll treat it like a global variable.
            return "@" + comment.substr(0, comment.size()-2);
        }

        return "%" + StringUtility::numberToString(leaf->get_name());
    }

    assert(!"not a number or variable");
    abort();
}

LeafNodePtr
RiscOperators::next_temporary(size_t nbits)
{
    return LeafNode::create_variable(nbits);
}

std::string
RiscOperators::next_label()
{
    static size_t nlabels = 0;
    return "L" + StringUtility::numberToString(nlabels++);
}

std::string
RiscOperators::addr_label(rose_addr_t addr)
{
    return "L_" + StringUtility::addrToString(addr);
}

// Emit an LLVM instruction to zero-extend a value if necessary.  If the value is already the specified width then this is a
// no-op.
TreeNodePtr
RiscOperators::emit_zero_extend(std::ostream &o, const TreeNodePtr &value, size_t nbits)
{
    assert(value!=NULL);
    assert(value->get_nbits() <= nbits);
    if (value->get_nbits() == nbits)
        return value;

    TreeNodePtr t1 = emit_expression(o, value);
    std::string t1_type = llvm_integer_type(t1->get_nbits());
    TreeNodePtr t2 = next_temporary(nbits);
    std::string t2_type = llvm_integer_type(nbits);
    o <<prefix() <<llvm_term(t2) <<" = zext " <<t1_type <<" " <<llvm_term(t1) <<" to " <<t2_type <<"\n";
    return t2;
}

// Emit an LLVM instruction to sign-extend a value.  If the value is already the specified width then this is a no-op.
TreeNodePtr
RiscOperators::emit_sign_extend(std::ostream &o, const TreeNodePtr &value, size_t nbits)
{
    assert(value!=NULL);
    assert(value->get_nbits() <= nbits);
    if (value->get_nbits() == nbits)
        return value;

    TreeNodePtr t1 = emit_expression(o, value);
    std::string t1_type = llvm_integer_type(t1->get_nbits());
    TreeNodePtr t2 = next_temporary(nbits);
    std::string t2_type = llvm_integer_type(nbits);
    o <<prefix() <<llvm_term(t2) <<" = sext " <<t1_type <<" " <<llvm_term(t1) <<" to " <<t2_type <<"\n";
    return t2;
}

// Emit an LLVM instruction to truncate a value if necessary.  If the value is already the specified width then this is
// a no-op.
TreeNodePtr
RiscOperators::emit_truncate(std::ostream &o, const TreeNodePtr &value, size_t nbits)
{
    assert(value!=NULL);
    assert(value->get_nbits() >= nbits);
    if (value->get_nbits() == nbits)
        return value;

    TreeNodePtr t1 = emit_expression(o, value);
    std::string t1_type = llvm_integer_type(t1->get_nbits());
    LeafNodePtr t2 = next_temporary(nbits);
    std::string t2_type = llvm_integer_type(nbits);
    o <<prefix() <<llvm_term(t2) <<" = trunc " <<t1_type <<" " <<llvm_term(t1) <<" to " <<t2_type <<"\n";
    return t2;
}

// Emit LLVM to make value the specified size by zero extending or truncating as necessary.  This is a no-op if the value
// is already the desired size.
TreeNodePtr
RiscOperators::emit_unsigned_resize(std::ostream &o, const TreeNodePtr &value, size_t nbits)
{
    if (value->get_nbits()==nbits)
        return value;
    if (value->get_nbits() < nbits)
        return emit_zero_extend(o, value, nbits);
    return emit_truncate(o, value, nbits);
}

// Emits an LLVM binary operator.  Both operands must be the same width.
TreeNodePtr
RiscOperators::emit_binary(std::ostream &o, const std::string &llvm_operator, const TreeNodePtr &a, const TreeNodePtr &b)
{
    assert(a!=NULL && b!=NULL);
    assert(a->get_nbits() == b->get_nbits());
    std::string type = llvm_integer_type(a->get_nbits());
    TreeNodePtr t1 = emit_expression(o, a);
    TreeNodePtr t2 = emit_expression(o, b);
    TreeNodePtr t3 = next_temporary(a->get_nbits());
    o <<prefix() <<llvm_term(t3) <<" = " <<llvm_operator
      <<" " <<type <<" " <<llvm_term(t1)
      <<", " <<type <<" " <<llvm_term(t2) <<"\n";
    return t3;
}

// Emits an LLVM binary operator.  The width of the result is the maximum width of the operands. The narrower of the two
// operands is sign extended to the same width as the result.
TreeNodePtr
RiscOperators::emit_signed_binary(std::ostream &o, const std::string &llvm_operator, const TreeNodePtr &a, const TreeNodePtr &b)
{
    assert(a!=NULL && b!=NULL);
    size_t width = std::max(a->get_nbits(), b->get_nbits());
    TreeNodePtr t1 = emit_sign_extend(o, a, width);
    TreeNodePtr t2 = emit_sign_extend(o, b, width);
    return emit_binary(o, llvm_operator, t1, t2);
}

// Emits an LLVM binary operator.  The width of the result is the maximum width of the operands. The narrower of the two
// operands is zero extended to the same width as the result.
TreeNodePtr
RiscOperators::emit_unsigned_binary(std::ostream &o, const std::string &llvm_operator, const TreeNodePtr &a, const TreeNodePtr &b)
{
    assert(a!=NULL && b!=NULL);
    size_t width = std::max(a->get_nbits(), b->get_nbits());
    TreeNodePtr t1 = emit_zero_extend(o, a, width);
    TreeNodePtr t2 = emit_zero_extend(o, b, width);
    return emit_binary(o, llvm_operator, t1, t2);
}

// Emit an LLVM logical left shift expression if necessary.  If the shift amount is the constant zero then this is a no-op.
// LLVM requires that the shift amount be the same width as the value being shifted.  The shift amount is interpreted as
// unsigned.
TreeNodePtr
RiscOperators::emit_logical_right_shift(std::ostream &o, const TreeNodePtr &value, const TreeNodePtr &amount)
{
    if (LeafNodePtr amount_leaf = amount->isLeafNode()) {
        if (amount_leaf->is_known()) {
            if (amount_leaf->get_value() == 0)
                return value;
            if (amount_leaf->get_value() >= value->get_nbits())
                return LeafNode::create_integer(value->get_nbits(), 0);
        }
    }
    return emit_binary(o, "lshr", value, emit_unsigned_resize(o, amount, value->get_nbits()));
}

// Emit the LLVM instructions for a right shift that inserts set bits rather than zeros.  The amount could be non-constant,
// so we need to be careful about how we do this.
TreeNodePtr
RiscOperators::emit_logical_right_shift_ones(std::ostream &o, const TreeNodePtr &value, const TreeNodePtr &amount)
{
    TreeNodePtr t1 = emit_logical_right_shift(o, value, amount);
    size_t width = std::max(value->get_nbits(), amount->get_nbits());
    TreeNodePtr ones = InternalNode::create(width, InsnSemanticsExpr::OP_ADD,
                                            LeafNode::create_integer(width, value->get_nbits()),
                                            InternalNode::create(width, InsnSemanticsExpr::OP_NEGATE,
                                                                 InternalNode::create(width, InsnSemanticsExpr::OP_UEXTEND,
                                                                                      LeafNode::create_integer(8, width),
                                                                                      amount)));
    return emit_binary(o, "or", t1, ones);
}

// Emit an LLVM expression for arithmetic right shift.  If the shift amount is the constant zero then this is a no-op.  LLVM
// requires that the shift amount be the same width as the value being shifted.  The shift amount is interpretted as unsigned.
TreeNodePtr
RiscOperators::emit_arithmetic_right_shift(std::ostream &o, const TreeNodePtr &value, const TreeNodePtr &amount)
{
    if (LeafNodePtr amount_leaf = amount->isLeafNode()) {
        if (amount_leaf->is_known()) {
            if (amount_leaf->get_value() == 0)
                return value;
            if (amount_leaf->get_value() >= value->get_nbits())
                return LeafNode::create_integer(value->get_nbits(), 0);
        }
    }
    return emit_binary(o, "ashr", value, emit_unsigned_resize(o, amount, value->get_nbits()));
}

// Emit an LLVM left-shift expression if necessary.  If the shift amount is the constant zero then this is a no-op. LLVM
// requires that the shift amount be the same width as the value being shifted.  The shift amount is interpretted as unsigned.
TreeNodePtr
RiscOperators::emit_left_shift(std::ostream &o, const TreeNodePtr &value, const TreeNodePtr &amount)
{
    if (LeafNodePtr amount_leaf = amount->isLeafNode()) {
        if (amount_leaf->is_known()) {
            if (amount_leaf->get_value() == 0)
                return value;
            if (amount_leaf->get_value() >= value->get_nbits())
                return LeafNode::create_integer(value->get_nbits(), 0);
        }
    }
    return emit_binary(o, "shl", value, emit_unsigned_resize(o, amount, value->get_nbits()));
}

// Emits the LLVM equivalent of ROSE's OP_SHL1, which shifts 1 bits into the LSB side of the value. LLVM doesn't have an
// operator that shifts set bits into the left side of a value, so we use the zero-inserting left shift operator, the invert
// operator, and the bitwise OR operator instead.
TreeNodePtr
RiscOperators::emit_left_shift_ones(std::ostream &o, const TreeNodePtr &value, const TreeNodePtr &amount)
{
    size_t width = value->get_nbits();
    TreeNodePtr t1 = emit_left_shift(o, value, amount);
    TreeNodePtr ones = emit_invert(o, emit_left_shift(o, LeafNode::create_integer(width, -1), amount));
    return emit_binary(o, "or", t1, ones);
}

// Emit LLVM instructions for an extract operator.  LLVM doesn't have a dedicated extract instruction, so we right shift
// and truncate.
TreeNodePtr
RiscOperators::emit_extract(std::ostream &o, const TreeNodePtr &value, const TreeNodePtr &from, size_t result_nbits)
{
    return emit_truncate(o, emit_logical_right_shift(o, value, from), result_nbits);
}

// Emit LLVM to invert all bits of a value.  LLVM doesn't have a dedicated invert operator, so we use xor instead.
TreeNodePtr
RiscOperators::emit_invert(std::ostream &o, const TreeNodePtr &value)
{
    return emit_binary(o, "xor", value, LeafNode::create_integer(value->get_nbits(), -1));
}

// Emit LLVM instructions for a left-associative binary operator. If only one operand is given, then simply return that operand
// without doing anything.  When more than one operand is given they must all be the same width.
TreeNodePtr
RiscOperators::emit_left_associative(std::ostream &o, const std::string &llvm_operator, const TreeNodes &operands)
{
    assert(!operands.empty());
    const size_t width = operands[0]->get_nbits();
    std::string type = llvm_integer_type(width);
    TreeNodePtr result = operands[0];

    for (size_t i=1; i<operands.size(); ++i) {
        assert(operands[i]->get_nbits() == width);
        result = emit_binary(o, llvm_operator, result, operands[i]);
    }
    return result;
}

// Emits LLVM to concatenate operands. LLVM doesn't have a dedicated concatenation operator, so we must build the result with
// left shift and bit-wise OR operators.  Operands are given from most significant to least significant.
TreeNodePtr
RiscOperators::emit_concat(std::ostream &o, TreeNodes operands)
{
    assert(!operands.empty());
    if (1==operands.size())
        return operands[0];

    size_t result_width = 0;
    for (size_t i=0; i<operands.size(); ++i)
        result_width += operands[i]->get_nbits();

    std::reverse(operands.begin(), operands.end());     // we want least-significant to most-significant
    TreeNodePtr result = operands[0];
    size_t shift = operands[0]->get_nbits();
    for (size_t i=1; i<operands.size(); ++i) {
        TreeNodePtr t1 = emit_zero_extend(o, result, result_width);
        TreeNodePtr t2 = emit_zero_extend(o, operands[i], result_width);
        TreeNodePtr t3 = emit_left_shift(o, t2, LeafNode::create_integer(result_width, shift));
        result = emit_binary(o, "or", t1, t3);
        shift += operands[i]->get_nbits();
    }
    return result;
}

// Emits LLVM to compute a ratio.  The width of the result is the same width as the numerator in both LLVM and ROSE.
TreeNodePtr
RiscOperators::emit_divide(std::ostream &o, const std::string llvm_operator,
                           const TreeNodePtr &numerator, const TreeNodePtr &denominator)
{
    TreeNodePtr t1 = emit_expression(o, numerator);
    std::string t1_type = llvm_integer_type(t1->get_nbits());
    TreeNodePtr t2 = emit_expression(o, denominator);
    std::string t2_type = llvm_integer_type(t2->get_nbits());
    TreeNodePtr t3 = next_temporary(t1->get_nbits());
    o <<prefix() <<llvm_term(t3) <<" = " <<llvm_operator
      <<" " <<t1_type <<" " <<llvm_term(t1)
      <<", " <<t2_type <<" " <<llvm_term(t2) <<"\n";
    return t3;
}


// Emits LLVM to compute a remainder.  In LLVM the width of the result is the same as the width of the numerator, but in ROSE
// the width of the result is the same as the width of the denominator.
TreeNodePtr
RiscOperators::emit_signed_modulo(std::ostream &o, const TreeNodePtr &numerator, const TreeNodePtr &denominator)
{
    TreeNodePtr t1 = emit_sign_extend(o, numerator, denominator->get_nbits());
    TreeNodePtr t2 = emit_binary(o, "srem", t1, denominator);
    return emit_truncate(o, t2, denominator->get_nbits());
}

// Emits LLVM to compute a remainder.  In LLVM the width of the result is the same as the width of the numerator, but in ROSE
// the width of the result is the same as the width of the denominator.
TreeNodePtr
RiscOperators::emit_unsigned_modulo(std::ostream &o, const TreeNodePtr &numerator, const TreeNodePtr &denominator)
{
    TreeNodePtr t1 = emit_zero_extend(o, numerator, denominator->get_nbits());
    TreeNodePtr t2 = emit_binary(o, "srem", t1, denominator);
    return emit_truncate(o, t2, denominator->get_nbits());
}

// Emits LLVM to compute the product of all the operands.  Multiply operations in LLVM are always sign-independent since the
// product has the same width as the operands (which must all be the same width).  In ROSE, multiply takes any number of
// operands and returns a product whose width is the sum of the operand widths, so we have to sign extend everything.
TreeNodePtr
RiscOperators::emit_signed_multiply(std::ostream &o, const TreeNodes &operands)
{
    assert(!operands.empty());

    size_t result_width = 0;
    for (size_t i=0; i<operands.size(); ++i)
        result_width += operands[i]->get_nbits();

    TreeNodePtr result = operands[0];
    for (size_t i=1; i<operands.size(); ++i) {
        TreeNodePtr t1 = emit_sign_extend(o, result, result_width);
        TreeNodePtr t2 = emit_sign_extend(o, operands[i], result_width);
        result = emit_binary(o, "mul", t1, t2);
    }

    return result;
}

// Emits LLVM to compute the product of all the operands.  Multiply operations in LLVM are always sign-independent since the
// product has the same width as the operands (which must all be the same width).  In ROSE, multiply takes any number of
// operands and returns a product whose width is the sum of the operand widths, so we have to zero extend everything.
TreeNodePtr
RiscOperators::emit_unsigned_multiply(std::ostream &o, const TreeNodes &operands)
{
    assert(!operands.empty());

    size_t result_width = 0;
    for (size_t i=0; i<operands.size(); ++i)
        result_width += operands[i]->get_nbits();

    TreeNodePtr result = operands[0];
    for (size_t i=1; i<operands.size(); ++i) {
        TreeNodePtr t1 = emit_zero_extend(o, result, result_width);
        TreeNodePtr t2 = emit_zero_extend(o, operands[i], result_width);
        result = emit_binary(o, "mul", t1, t2);
    }

    return result;
}

// Emits LLVM for an if-then-else construct.
TreeNodePtr
RiscOperators::emit_ite(std::ostream &o, const TreeNodePtr &cond, const TreeNodePtr &a, const TreeNodePtr &b)
{
    assert(cond!=NULL && a!=NULL && b!=NULL);
    assert(cond->get_nbits()==1);
    assert(a->get_nbits()==b->get_nbits());

    // Condition
    std::string true_label = next_label();
    std::string false_label = next_label();
    std::string end_label = next_label();
    TreeNodePtr result = next_temporary(a->get_nbits());
    TreeNodePtr t1 = emit_expression(o, cond);
    o <<prefix() <<"br i1 " <<llvm_term(t1) <<", label " <<true_label <<", label " <<false_label <<"\n";

    // True body
    o <<true_label <<":\n";
    {
        Indent indent(this);
        TreeNodePtr t2 = emit_expression(o, a);
        o <<llvm_term(result) <<" = " <<llvm_term(t2) <<"\n";
        o <<prefix() <<"br i1 1, label " <<end_label <<"\n";
    }

    // False body
    o <<prefix() <<false_label <<":\n";
    {
        Indent indent(this);
        TreeNodePtr t3 = emit_expression(o, b);
        o <<llvm_term(result) <<" = " <<llvm_term(t3) <<"\n";
        o <<prefix() <<"br i1 1, label " <<end_label <<"\n";
    }

    // End
    o <<prefix() <<end_label <<":\n";
    return result;
}

TreeNodePtr
RiscOperators::emit_memory_read(std::ostream &o, const TreeNodePtr &addr, size_t nbits)
{
    assert(addr!=NULL);
    LeafNodePtr t1 = emit_expression(o, addr);
    std::string type = llvm_integer_type(nbits);
    LeafNodePtr t2 = next_temporary(nbits);
    o <<prefix() <<llvm_term(t2) <<" = load " <<type <<"* " <<llvm_term(t1) <<"\n";
    return t2;
}

// Emits LLVM for an expression and returns a terminal (variable or constant).  If the expression is already a terminal then
// this method is a no-op.
LeafNodePtr
RiscOperators::emit_expression(std::ostream &o, const TreeNodePtr &expr)
{
    assert(expr!=NULL);
    LeafNodePtr retval = expr->isLeafNode();

    // If we've seen this expression already, then use the replacement value (an LLVM variable).
    if (rewrites.exists(expr->hash()))
        retval = rewrites.get_one(expr->hash());

    // Emit LLVM for symbolic operators until the result is a leaf node (LLVM variable or constant). This causes recursive
    // calls to emit_expression().
    if (retval==NULL && expr->isInternalNode()) {
        TreeNodePtr result = expr->isInternalNode();
        while (InternalNodePtr inode = result->isInternalNode()) {
            TreeNodes operands = inode->get_children();
            switch (inode->get_operator()) {
                case InsnSemanticsExpr::OP_ADD:
                    result = emit_left_associative(o, "add", operands);
                    break;
                case InsnSemanticsExpr::OP_AND:
                    result = emit_left_associative(o, "and", operands);
                    break;
                case InsnSemanticsExpr::OP_ASR:
                    assert(2==operands.size());
                    result = emit_arithmetic_right_shift(o, operands[1], operands[0]);
                    break;
                case InsnSemanticsExpr::OP_BV_AND:
                    result = emit_left_associative(o, "and", operands);
                    break;
                case InsnSemanticsExpr::OP_BV_OR:
                    result = emit_left_associative(o, "or", operands);
                    break;
                case InsnSemanticsExpr::OP_BV_XOR:
                    result = emit_left_associative(o, "xor", operands);
                    break;
                case InsnSemanticsExpr::OP_CONCAT:
                    result = emit_concat(o, operands);
                    break;
                case InsnSemanticsExpr::OP_EQ:
                    assert(2==operands.size());
                    result = emit_binary(o, "icmp eq", operands[0], operands[1]);
                    break;
                case InsnSemanticsExpr::OP_EXTRACT:
                    assert(3==operands.size());
                    result = emit_extract(o, operands[2], operands[0], inode->get_nbits());
                    break;
                case InsnSemanticsExpr::OP_INVERT:
                    assert(1==operands.size());
                    result = emit_invert(o, operands[0]);
                    break;
                case InsnSemanticsExpr::OP_ITE:
                    assert(3==operands.size());
                    result = emit_ite(o, operands[0], operands[1], operands[2]);
                    break;
                case InsnSemanticsExpr::OP_NE:
                    assert(2==operands.size());
                    result = emit_binary(o, "icmp ne", operands[0], operands[1]);
                    break;
                case InsnSemanticsExpr::OP_NEGATE:
                    assert(1==operands.size());
                    result = emit_binary(o, "sub", LeafNode::create_integer(operands[0]->get_nbits(), 0), operands[0]);
                    break;
                case InsnSemanticsExpr::OP_OR:
                    result = emit_left_associative(o, "or", operands);
                    break;
                case InsnSemanticsExpr::OP_READ:
                    assert(2==operands.size());
                    result = emit_memory_read(o, operands[1], inode->get_nbits());
                    break;
                case InsnSemanticsExpr::OP_SDIV:
                    assert(2==operands.size());
                    result = emit_divide(o, "sdiv", operands[0], operands[1]);
                    break;
                case InsnSemanticsExpr::OP_SEXTEND:
                    assert(2==operands.size());
                    result = emit_sign_extend(o, operands[1], inode->get_nbits());
                    break;
                case InsnSemanticsExpr::OP_SGE:
                    assert(2==operands.size());
                    result = emit_binary(o, "icmp sge", operands[0], operands[1]);
                    break;
                case InsnSemanticsExpr::OP_SGT:
                    assert(2==operands.size());
                    result = emit_binary(o, "icmp sgt", operands[0], operands[1]);
                    break;
                case InsnSemanticsExpr::OP_SHL0:
                    assert(2==operands.size());
                    result = emit_left_shift(o, operands[1], operands[0]);
                    break;
                case InsnSemanticsExpr::OP_SHL1:
                    assert(2==operands.size());
                    result = emit_left_shift_ones(o, operands[1], operands[0]);
                    break;
                case InsnSemanticsExpr::OP_SHR0:
                    assert(2==operands.size());
                    result = emit_logical_right_shift(o, operands[1], operands[0]);
                    break;
                case InsnSemanticsExpr::OP_SHR1:
                    assert(2==operands.size());
                    result = emit_logical_right_shift_ones(o, operands[1], operands[0]);
                    break;
                case InsnSemanticsExpr::OP_SLE:
                    assert(2==operands.size());
                    result = emit_binary(o, "icmp sle", operands[0], operands[1]);
                    break;
                case InsnSemanticsExpr::OP_SLT:
                    assert(2==operands.size());
                    result = emit_binary(o, "icmp slt", operands[0], operands[1]);
                    break;
                case InsnSemanticsExpr::OP_SMOD:
                    assert(2==operands.size());
                    result = emit_signed_modulo(o, operands[0], operands[1]);
                    break;
                case InsnSemanticsExpr::OP_SMUL:
                    result = emit_signed_multiply(o, operands);
                    break;
                case InsnSemanticsExpr::OP_UDIV:
                    assert(2==operands.size());
                    result = emit_divide(o, "udiv", operands[0], operands[1]);
                    break;
                case InsnSemanticsExpr::OP_UEXTEND:
                    assert(2==operands.size());
                    result = emit_zero_extend(o, operands[1], inode->get_nbits());
                    break;
                case InsnSemanticsExpr::OP_UGE:
                    assert(2==operands.size());
                    result = emit_binary(o, "icmp uge", operands[0], operands[1]);
                    break;
                case InsnSemanticsExpr::OP_UGT:
                    assert(2==operands.size());
                    result = emit_binary(o, "icmp ugt", operands[0], operands[1]);
                    break;
                case InsnSemanticsExpr::OP_ULE:
                    assert(2==operands.size());
                    result = emit_binary(o, "icmp ule", operands[0], operands[1]);
                    break;
                case InsnSemanticsExpr::OP_ULT:
                    assert(2==operands.size());
                    result = emit_binary(o, "icmp ult", operands[0], operands[1]);
                    break;
                case InsnSemanticsExpr::OP_UMOD:
                    assert(2==operands.size());
                    result = emit_unsigned_modulo(o, operands[0], operands[1]);
                    break;
                case InsnSemanticsExpr::OP_UMUL:
                    result = emit_unsigned_multiply(o, operands);
                    break;
                case InsnSemanticsExpr::OP_ZEROP:
                    assert(1==operands.size());
                    result = emit_binary(o, "icmp eq", operands[0], LeafNode::create_integer(operands[0]->get_nbits(), 0));
                    break;

                case InsnSemanticsExpr::OP_LSSB:
                case InsnSemanticsExpr::OP_MSSB:
                case InsnSemanticsExpr::OP_NOOP:
                case InsnSemanticsExpr::OP_ROL:
                case InsnSemanticsExpr::OP_ROR:
                case InsnSemanticsExpr::OP_WRITE:
                    assert(!"not implemented yet");
                    abort();

                // no default because we want warnings when a new operator is added
            }
            assert(result!=inode);                      // no case was executed
        }
        retval = result->isLeafNode();
        assert(retval!=NULL);
    }

    // Add a rewrite rule so that next time we're asked to emit the same expression we can just emit the LLVM variable (or
    // constant) to which the expression was translated.
    assert(retval!=NULL);
    assert(retval->is_known() || retval->is_variable());
    if (expr->hash()!=retval->hash())
        rewrites.insert(std::make_pair(expr->hash(), retval));
    return retval;
}

LeafNodePtr
RiscOperators::emit_assignment(std::ostream &o, const TreeNodePtr &rhs)
{
    assert(rhs!=NULL);
    LeafNodePtr lhs = next_temporary(rhs->get_nbits());
    LeafNodePtr t1 = emit_expression(o, rhs);
    o <<prefix() <<llvm_term(lhs) <<" = " <<llvm_integer_type(rhs->get_nbits()) <<" " <<llvm_term(t1) <<"\n";
    rewrites.insert(std::make_pair(rhs->hash(), lhs));
    return lhs;
}

void
Transcoder::transcodeInstruction(SgAsmInstruction *insn, std::ostream &o)
{
    abort();
}

std::string
Transcoder::transcodeInstruction(SgAsmInstruction *insn)
{
    std::ostringstream ss;
    transcodeInstruction(insn, ss);
    return ss.str();
}

void
Transcoder::transcodeBasicBlock(SgAsmBlock *bb, std::ostream &o)
{
    assert(this!=NULL);
    if (!bb)
        return;
    o <<operators->prefix() <<operators->addr_label(bb->get_address()) <<":\n";
    RiscOperators::Indent indent(operators);
    RoseAst ast(bb);
    operators->reset();
    bool emit_eip = false;
    for (RoseAst::iterator ai=ast.begin().withoutNullValues(); ai!=ast.end(); ++ai) {
        if (SgAsmInstruction *insn = isSgAsmInstruction(*ai)) {
            ai.skipChildrenOnForward();
            o <<operators->prefix() <<"; " <<StringUtility::addrToString(insn->get_address())
              <<": " <<unparseInstruction(insn) <<"\n";
            dispatcher->processInstruction(insn);
            emit_eip = true;
            // In order to avoid issues of memory aliasing, whenever a write occurs we dump the current machine state to
            // LLVM. We're assuming that a machine instruction performs at most one write and if it performs a memory write
            // then it doesn't also perform a memory read.
            if (!operators->get_memory_writes().empty())
                operators->emit_changed_state(o);
        } else if (SgAsmStaticData *data = isSgAsmStaticData(*ai)) {
            SgUnsignedCharList bytes = data->get_raw_bytes();
            o <<operators->prefix() <<"@" + StringUtility::addrToString(data->get_address())
              <<" = [" <<bytes.size() <<" x i8] [";
            for (size_t i=0; i<bytes.size(); ++i)
                o <<(i?", ":"") <<"i8 " <<StringUtility::unsignedToHex2(bytes[i], 8);
            o <<"]\n";
        }
    }
    operators->emit_changed_state(o);
    if (emit_eip)
        operators->emit_next_eip(o);
}

std::string
Transcoder::transcodeBasicBlock(SgAsmBlock *bb)
{
    std::ostringstream ss;
    transcodeBasicBlock(bb, ss);
    return ss.str();
}

void
Transcoder::transcodeFunction(SgAsmFunction *func, std::ostream &o)
{
    assert(this!=NULL);
    if (!func)
        return;

    o <<operators->prefix() <<"define void @" <<operators->addr_label(func->get_entry_va()) <<"() {\n";
    RiscOperators::Indent func_body_indentation(operators);

    // Note that in LLVM the first basic block of a function cannot be the target of a branch instruction.  Therefore, we emit
    // a no-op as the first basic block.  We indent two levels because the bodies of the other basic blocks are indented two
    // levels (one for the label and another for the instructions).
    {
        RiscOperators::Indent bb_indentation(operators);
        o <<operators->prefix() <<"br i1 1, label " <<operators->addr_label(func->get_entry_va()) <<"\n";
    }

    const SgAsmStatementPtrList &bbs = func->get_statementList();
    for (SgAsmStatementPtrList::const_iterator bbi=bbs.begin(); bbi!=bbs.end(); ++bbi) {
        SgAsmBlock *bb = isSgAsmBlock(*bbi);
        assert(bb!=NULL);
        if (bbi!=bbs.begin())
            o <<"\n";
        transcodeBasicBlock(bb, o);
    }

    o <<"}\n";
}

std::string
Transcoder::transcodeFunction(SgAsmFunction *func)
{
    std::ostringstream ss;
    transcodeFunction(func, ss);
    return ss.str();
}

} // namespace
} // namespace
} // namespace
