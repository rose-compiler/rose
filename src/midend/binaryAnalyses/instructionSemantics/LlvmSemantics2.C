#include "sage3basic.h"
#include "LlvmSemantics2.h"
#include "RoseAst.h"
#include "AsmUnparser_compat.h"
#include "integerOps.h"

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
    variables.clear();
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

// FIXME[Robb P. Matzke 2014-01-07]: This is x86 specific.
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
            if (InternalNodePtr inode = node->isInternalNode()) {
                if (InsnSemanticsExpr::OP_READ==inode->get_operator()) {
                    assert(2==inode->size());
                    ops->emit_assignment(o, ops->emit_memory_read(o, inode->child(1), width));
                }
            } else if (LeafNodePtr leaf = node->isLeafNode()) {
                if (leaf->is_variable()) {
                    std::string comment = leaf->get_comment();
                    if (comment.size()>2 && 0==comment.substr(comment.size()-2).compare("_0"))
                        ops->add_variable(leaf);        // becomes a global variable if not already
                    LeafNodePtr t1 = ops->emit_expression(o, leaf);// handles local vars, global vars, and undefs
                }
            }
        }
    } t1(this, o, regs, dictionary);

    // Prerequisites for the registers
    RegisterStatePtr regstate = RegisterState::promote(get_state()->get_register_state());
    for (size_t i=0; i<regs.size(); ++i) {
        SValuePtr value = SValue::promote(regstate->readRegister(regs[i], this));
        value->get_expression()->depth_first_visit(&t1);
    }

    // Prerequisites for memory writes
    for (TreeNodes::const_iterator mwi=mem_writes.begin(); mwi!=mem_writes.end(); ++mwi) {
        const TreeNodePtr mem_write = *mwi;
        mem_write->depth_first_visit(&t1);
    }

    // Prerequisites for the instruction pointer.
    get_instruction_pointer()->get_expression()->depth_first_visit(&t1);
}

void
RiscOperators::emit_register_declarations(std::ostream &o, const RegisterDescriptors &regs)
{
    const RegisterDictionary *dictionary = get_state()->get_register_state()->get_register_dictionary();
    for (size_t i=0; i<regs.size(); ++i) {
        const std::string &name = dictionary->lookup(regs[i]);
        assert(!name.empty());
        o <<prefix() <<"@" <<name <<" = external global " <<llvm_integer_type(regs[i].get_nbits()) <<"\n";
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
        o <<prefix() <<"store " <<llvm_integer_type(t1->get_nbits()) <<" " <<llvm_term(t1)
          <<", " <<llvm_integer_type(t1->get_nbits()) <<"* @" <<name <<"\n";
    }
}

void
RiscOperators::emit_next_eip(std::ostream &o, SgAsmInstruction *latest_insn)
{
    SgAsmBlock *bb = SageInterface::getEnclosingNode<SgAsmBlock>(latest_insn);
    SgAsmFunction *func = SageInterface::getEnclosingNode<SgAsmFunction>(bb);
    SgAsmInterpretation *interp = SageInterface::getEnclosingNode<SgAsmInterpretation>(func);
    assert(interp!=NULL);                           // instructions must be part of the global AST
    const InstructionMap &insns = interp->get_instruction_map();
    SValuePtr eip = get_instruction_pointer();

    // If EIP is a constant then it is one of the following cases:
    //    1. It points to an instruction that's in a different function than the last executed instruction, in which
    //       case we treat it like a function call.  LLVM requires us to treat all inter-function control flow as a
    //       function call even though the same restriction isn't present in the binary. FIXME[Robb P. Matzke 2014-01-09]
    //    2. It is an unconditional intra-function branch which can be translated to an LLVM unconditional "br" instruction.
    if (eip->is_number()) {
        SgAsmInstruction *dst_insn = insns.get_value_or(eip->get_number(), NULL);
        SgAsmFunction *dst_func = SageInterface::getEnclosingNode<SgAsmFunction>(dst_insn);
        if (func!=dst_func) {                       // one or both could be null
            std::string funcname = function_label(dst_func);
            std::string ret_label = addr_label(latest_insn->get_address() + latest_insn->get_size());
            o <<prefix() <<"call void " <<funcname <<"()\n"
              <<prefix() <<"br label %" <<ret_label <<"\n";
        } else {
            o <<prefix() <<"br label %" <<addr_label(eip->get_number()) <<"\n";
        }
        return;
    }

    o <<prefix() <<"; register eip = " <<*eip <<"\n";

    // If EIP is a symbolic if-then-else ("ite") and both operands are constants then the binary has a conditional branch
    // instruction (like an x86 "je", "jne", etc.) and we can emit an LLVM conditional "br" with true and false parts.
    InternalNodePtr inode = eip->get_expression()->isInternalNode();
    if (inode && InsnSemanticsExpr::OP_ITE==inode->get_operator()) {
        LeafNodePtr true_leaf = inode->child(1)->isLeafNode();
        LeafNodePtr false_leaf = inode->child(2)->isLeafNode();
        if (true_leaf!=NULL && true_leaf->is_known() && false_leaf!=NULL && false_leaf->is_known()) {
            LeafNodePtr t1 = emit_expression(o, inode->child(0));
            o <<prefix() <<"br i1 " <<llvm_term(t1)
              <<", label %" <<addr_label(true_leaf->get_value())
              <<", label %" <<addr_label(false_leaf->get_value()) <<"\n";
            return;
        }
    }

    // If the last executed instruction is some kind of function return instruction, then emit an LLVM "ret" instruction.
    // The stack used by LLVM is distinct from the stack used by the binary. The latter's stack is implemented via the @ebp (or
    // similar) global pointer.  FIXME[Robb P. Matzke 2014-01-09]: This is architecture dependent.
    if (SgAsmx86Instruction *insn_x86 = isSgAsmx86Instruction(latest_insn)) {
        if (insn_x86->get_kind() == x86_ret || insn_x86->get_kind() == x86_retf) {
            o <<prefix() <<"ret void\n";
            return;
        }
    }

    // If this function is a thunk, then we need to treat it as an LLVM function call (because LLVM doesn't allow
    // inter-function branches).  FIXME[Robb P. Matzke 2014-01-09]: This is architecture dependent.
    if (SgAsmx86Instruction *insn_x86 = isSgAsmx86Instruction(latest_insn)) {
        std::vector<SgAsmInstruction*> func_insns = SageInterface::querySubTree<SgAsmInstruction>(func);
        if (func_insns.size()==1 && func_insns.front()==insn_x86 &&
            (insn_x86->get_kind() == x86_jmp || insn_x86->get_kind() == x86_farjmp)) {
            LeafNodePtr t1 = emit_expression(o, eip);
            o <<prefix() <<"call void()* " <<llvm_term(t1) <<"\n"
              <<prefix() <<"ret void\n";
            return;
        }
    }

    // If we don't know the target address and this is an indirect function call, then the successors are the entry points of
    // all known functions.  FIXME[Robb P. Matzke 2014-01-09]: Detection of a function call is architecture dependent.
    if (SgAsmx86Instruction *insn_x86 = isSgAsmx86Instruction(latest_insn)) {
        if (insn_x86->get_kind() == x86_call || insn_x86->get_kind() == x86_farcall) {
            LeafNodePtr t1 = emit_expression(o, eip);
            std::string ret_label = addr_label(latest_insn->get_address() + latest_insn->get_size());
            o <<prefix() <<"call void()* " <<llvm_term(t1) <<"\n"
              <<prefix() <<"br label %" <<ret_label <<"\n";
            return;
        }
    }

    // Catch-all: this must be an intra-function indirect branch.  LLVM requires us to enumerate all possible targets, but
    // since we don't actually know them we must enumerate all basic blocks of the function.
    {
        LeafNodePtr t1 = emit_expression(o, eip);
        o <<prefix() <<"indirectbr i8* " <<llvm_term(t1) <<" [";
        size_t nblocks = 0;
        for (InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
            if (SageInterface::getEnclosingNode<SgAsmFunction>(ii->second)==func && ii->second->is_first_in_block())
                o <<(nblocks++ ? ", " : "") <<"label %" <<addr_label(ii->first);
        }
        o <<"]\n";
    }
}

void
RiscOperators::emit_memory_writes(std::ostream &o)
{
    for (size_t i=0; i<mem_writes.size(); ++i) {
        InternalNodePtr inode = mem_writes[i]->isInternalNode();
        assert(inode!=NULL);
        assert(inode->get_operator() == InsnSemanticsExpr::OP_WRITE);
        assert(inode->size()==3);
        TreeNodePtr addr = inode->child(1);
        TreeNodePtr value = inode->child(2);
        o <<prefix() <<"; store value=" <<*value <<" at address=" <<*addr <<"\n";
        emit_memory_write(o, addr, value);
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
RiscOperators::llvm_lvalue(const LeafNodePtr &var)
{
    assert(var && var->is_variable());
    assert(!variables.exists(var->get_name()));         // LLVM assembly is SSA
    return add_variable(var);
}

std::string
RiscOperators::llvm_term(const TreeNodePtr &expr)
{
    LeafNodePtr leaf = expr->isLeafNode();
    assert(leaf!=NULL);
    leaf = rewrites.get_value_or(leaf->hash(), leaf);

    if (leaf->is_known()) {
        int64_t sv = IntegerOps::signExtend2(leaf->get_value(), leaf->get_nbits(), 8*sizeof(int64_t));
        return StringUtility::numberToString(sv);
    }

    std::string name = get_variable(leaf);
    assert(!name.empty());
    return name;
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

std::string
RiscOperators::function_label(SgAsmFunction *func)
{
    std::string fname = func->get_name();
    if (fname.empty())
        fname = "L_" + StringUtility::addrToString(func->get_entry_va());
    for (size_t i=0; i<fname.size(); ++i) {
        if ('_'!=fname[i] && !isalnum(fname[i]))
            fname[i] = '.';
    }
    return "@" + fname;
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

    LeafNodePtr t1 = emit_expression(o, value);
    std::string t1_type = llvm_integer_type(t1->get_nbits());
    LeafNodePtr t2 = next_temporary(nbits);
    std::string t2_type = llvm_integer_type(nbits);
    o <<prefix() <<llvm_lvalue(t2) <<" = zext " <<t1_type <<" " <<llvm_term(t1) <<" to " <<t2_type <<"\n";
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
    LeafNodePtr t2 = next_temporary(nbits);
    std::string t2_type = llvm_integer_type(nbits);
    o <<prefix() <<llvm_lvalue(t2) <<" = sext " <<t1_type <<" " <<llvm_term(t1) <<" to " <<t2_type <<"\n";
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
    o <<prefix() <<llvm_lvalue(t2) <<" = trunc " <<t1_type <<" " <<llvm_term(t1) <<" to " <<t2_type <<"\n";
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
    LeafNodePtr t3 = next_temporary(a->get_nbits());
    o <<prefix() <<llvm_lvalue(t3) <<" = " <<llvm_operator
      <<" " <<type <<" " <<llvm_term(t1) <<", " <<llvm_term(t2) <<"\n";
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

// Emits LLVM to compute a ratio.  LLVM requires that the numerator and denominator have the same width; ROSE only stipulates
// that the return value has the same width as the denominator.
TreeNodePtr
RiscOperators::emit_signed_divide(std::ostream &o, const TreeNodePtr &numerator, const TreeNodePtr &denominator)
{
    size_t width = std::max(numerator->get_nbits(), denominator->get_nbits());
    TreeNodePtr t1 = emit_sign_extend(o, numerator, width);
    TreeNodePtr t2 = emit_sign_extend(o, denominator, width);
    TreeNodePtr t3 = emit_binary(o, "sdiv", t1, t2);
    return emit_truncate(o, t3, denominator->get_nbits());
}

// Emits LLVM to compute a ratio.  LLVM requires that the numerator and denominator have the same width; ROSE only stipulates
// that the return value has the same width as the denominator.
TreeNodePtr
RiscOperators::emit_unsigned_divide(std::ostream &o, const TreeNodePtr &numerator, const TreeNodePtr &denominator)
{
    size_t width = std::max(numerator->get_nbits(), denominator->get_nbits());
    TreeNodePtr t1 = emit_zero_extend(o, numerator, width);
    TreeNodePtr t2 = emit_zero_extend(o, denominator, width);
    TreeNodePtr t3 = emit_binary(o, "udiv", t1, t2);
    return emit_truncate(o, t3, denominator->get_nbits());
}

// Emits LLVM to compute a remainder.  In LLVM the width of the result is the same as the width of the numerator, but in ROSE
// the width of the result is the same as the width of the denominator.
TreeNodePtr
RiscOperators::emit_signed_modulo(std::ostream &o, const TreeNodePtr &numerator, const TreeNodePtr &denominator)
{
    size_t width = std::max(numerator->get_nbits(), denominator->get_nbits());
    TreeNodePtr t1 = emit_sign_extend(o, numerator, width);
    TreeNodePtr t2 = emit_sign_extend(o, denominator, width);
    TreeNodePtr t3 = emit_binary(o, "srem", t1, t2);
    return emit_truncate(o, t3, denominator->get_nbits());
}

// Emits LLVM to compute a remainder.  In LLVM the width of the result is the same as the width of the numerator, but in ROSE
// the width of the result is the same as the width of the denominator.
TreeNodePtr
RiscOperators::emit_unsigned_modulo(std::ostream &o, const TreeNodePtr &numerator, const TreeNodePtr &denominator)
{
    size_t width = std::max(numerator->get_nbits(), denominator->get_nbits());
    TreeNodePtr t1 = emit_zero_extend(o, numerator, width);
    TreeNodePtr t2 = emit_zero_extend(o, denominator, width);
    TreeNodePtr t3 = emit_binary(o, "urem", t1, t2);
    return emit_truncate(o, t3, denominator->get_nbits());
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

// Emits a comparison operation.  E.g., emit_compare(o, "icmp eq", %1, %2) will emit:
//    %3 = icmp eq i32 %1, i32 %1; typeof(%3) == i1
TreeNodePtr
RiscOperators::emit_compare(std::ostream &o, const std::string &llvm_op, const TreeNodePtr &a, const TreeNodePtr &b)
{
    LeafNodePtr t1 = emit_expression(o, a);
    LeafNodePtr t2 = emit_expression(o, b);
    LeafNodePtr t3 = next_temporary(1);
    o <<prefix() <<llvm_lvalue(t3) <<" = " <<llvm_op
      <<" " <<llvm_integer_type(t1->get_nbits()) <<" " <<llvm_term(t1) <<", " <<llvm_term(t2) <<"\n";
    return t3;
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
    LeafNodePtr result = next_temporary(a->get_nbits());
    std::string lvalue = llvm_lvalue(result);
    TreeNodePtr t1 = emit_expression(o, cond);
    o <<prefix() <<"br i1 " <<llvm_term(t1) <<", label %" <<true_label <<", label %" <<false_label <<"\n";

    // True body
    o <<true_label <<":\n";
    {
        Indent indent(this);
        TreeNodePtr t2 = emit_expression(o, a);
        o <<lvalue <<" = " <<llvm_term(t2) <<"\n";
        o <<prefix() <<"br label %" <<end_label <<"\n";
    }

    // False body
    o <<prefix() <<false_label <<":\n";
    {
        Indent indent(this);
        TreeNodePtr t3 = emit_expression(o, b);
        o <<lvalue <<" = " <<llvm_term(t3) <<"\n";
        o <<prefix() <<"br label %" <<end_label <<"\n";
    }

    // End
    o <<prefix() <<end_label <<":\n";
    return result;
}

TreeNodePtr
RiscOperators::emit_memory_read(std::ostream &o, const TreeNodePtr &addr, size_t nbits)
{
    assert(addr!=NULL);

    // Convert ADDR to a pointer T2. The pointer type is "iNBITS*"
    LeafNodePtr t1 = emit_expression(o, addr);
    LeafNodePtr t2 = next_temporary(32);                // a 32-bit address
    o <<prefix() <<llvm_lvalue(t2) <<" = inttoptr " <<llvm_integer_type(t1->get_nbits()) <<" " <<llvm_term(t1)
      <<" to " <<llvm_integer_type(nbits) <<"*\n";

    // Dereference pointer T2 to get the return value.
    LeafNodePtr t3 = next_temporary(nbits);
    o <<prefix() <<llvm_lvalue(t3) <<" = load " <<llvm_integer_type(nbits) <<"* " <<llvm_term(t2) <<"\n";
    return t3;
}

// Reads a global variable. For instance, emit_global_read(o, "@ebp", 32) will produce the following LLVM:
//   %1 = load i32* @ebp
// and returns %1
TreeNodePtr
RiscOperators::emit_global_read(std::ostream &o, const std::string &varname, size_t nbits)
{
    assert(!varname.empty() && varname[0]=='@');
    LeafNodePtr t1 = next_temporary(nbits);
    o <<prefix() <<llvm_lvalue(t1) <<" = load " <<llvm_integer_type(nbits) <<"* " <<varname <<"\n";
    return t1;
}

// Write a value to an address. The address is an integer expression.  For instance,
//     emit_memory_write(o, i32 %1, i16 %2)
// emits
//     %3 = inttoptr i32 %1 to i16*
//     store i16 %2, i16* %3
void
RiscOperators::emit_memory_write(std::ostream &o, const TreeNodePtr &addr, const TreeNodePtr &value)
{
    LeafNodePtr t1 = emit_expression(o, value);
    LeafNodePtr t2 = emit_expression(o, addr);

    LeafNodePtr t3 = next_temporary(value->get_nbits());
    o <<prefix() <<llvm_lvalue(t3) <<" = inttoptr " <<llvm_integer_type(addr->get_nbits()) <<" " <<llvm_term(t2)
      <<" to " <<llvm_integer_type(value->get_nbits()) <<"*\n";
    o <<prefix() <<"store " <<llvm_integer_type(value->get_nbits()) <<" " <<llvm_term(t1)
      <<", " <<llvm_integer_type(value->get_nbits()) <<"* " <<llvm_term(t3) <<"\n";
}

// Emits LLVM for an expression and returns a terminal (variable or constant).  If the expression is already a terminal then
// this method is a no-op.
LeafNodePtr
RiscOperators::emit_expression(std::ostream &o, const TreeNodePtr &orig_expr)
{
    assert(orig_expr!=NULL);
    TreeNodePtr cur_expr = orig_expr;

    // If we've seen this expression already, then use the replacement value (an LLVM variable).
    if (rewrites.exists(cur_expr->hash()))
        cur_expr = rewrites.get_one(cur_expr->hash());

    // Handle leaf nodes
    if (LeafNodePtr leaf = cur_expr->isLeafNode()) {
        if (leaf->is_variable()) {
            std::string varname = get_variable(leaf);
            if (varname.empty()) {
                // This is a reference to a ROSE variable that has no corresponding LLVM variable.  This can happen for things
                // like the x86 TEST instruction, which leaves the AF register in an undefined state--if we then try to print
                // the definition of AF we will encounter a ROSE variable with no LLVM variable.
                LeafNodePtr t1 = next_temporary(leaf->get_nbits());
                o <<prefix() <<llvm_lvalue(t1) <<" = add " <<llvm_integer_type(leaf->get_nbits()) <<" undef, undef\n";
                cur_expr = t1;
            } else if ('@'==varname[0]) {
                cur_expr = emit_global_read(o, varname, leaf->get_nbits());
            }
        }
    }
    
    // Emit LLVM for symbolic operators until the result is a leaf node (LLVM variable or constant). This causes recursive
    // calls to emit_expression().
    while (InternalNodePtr inode = cur_expr->isInternalNode()) {
        TreeNodePtr operator_result;
        TreeNodes operands = inode->get_children();
        switch (inode->get_operator()) {
            case InsnSemanticsExpr::OP_ADD:
                operator_result = emit_left_associative(o, "add", operands);
                break;
            case InsnSemanticsExpr::OP_AND:
                operator_result = emit_left_associative(o, "and", operands);
                break;
            case InsnSemanticsExpr::OP_ASR:
                assert(2==operands.size());
                operator_result = emit_arithmetic_right_shift(o, operands[1], operands[0]);
                break;
            case InsnSemanticsExpr::OP_BV_AND:
                operator_result = emit_left_associative(o, "and", operands);
                break;
            case InsnSemanticsExpr::OP_BV_OR:
                operator_result = emit_left_associative(o, "or", operands);
                break;
            case InsnSemanticsExpr::OP_BV_XOR:
                operator_result = emit_left_associative(o, "xor", operands);
                break;
            case InsnSemanticsExpr::OP_CONCAT:
                operator_result = emit_concat(o, operands);
                break;
            case InsnSemanticsExpr::OP_EQ:
                assert(2==operands.size());
                operator_result = emit_compare(o, "icmp eq", operands[0], operands[1]);
                break;
            case InsnSemanticsExpr::OP_EXTRACT:
                assert(3==operands.size());
                operator_result = emit_extract(o, operands[2], operands[0], inode->get_nbits());
                break;
            case InsnSemanticsExpr::OP_INVERT:
                assert(1==operands.size());
                operator_result = emit_invert(o, operands[0]);
                break;
            case InsnSemanticsExpr::OP_ITE:
                assert(3==operands.size());
                operator_result = emit_ite(o, operands[0], operands[1], operands[2]);
                break;
            case InsnSemanticsExpr::OP_NE:
                assert(2==operands.size());
                operator_result = emit_compare(o, "icmp ne", operands[0], operands[1]);
                break;
            case InsnSemanticsExpr::OP_NEGATE:
                assert(1==operands.size());
                operator_result = emit_binary(o, "sub", LeafNode::create_integer(operands[0]->get_nbits(), 0), operands[0]);
                break;
            case InsnSemanticsExpr::OP_OR:
                operator_result = emit_left_associative(o, "or", operands);
                break;
            case InsnSemanticsExpr::OP_READ:
                assert(2==operands.size());
                operator_result = emit_memory_read(o, operands[1], inode->get_nbits());
                break;
            case InsnSemanticsExpr::OP_SDIV:
                assert(2==operands.size());
                operator_result = emit_signed_divide(o, operands[0], operands[1]);
                break;
            case InsnSemanticsExpr::OP_SEXTEND:
                assert(2==operands.size());
                operator_result = emit_sign_extend(o, operands[1], inode->get_nbits());
                break;
            case InsnSemanticsExpr::OP_SGE:
                assert(2==operands.size());
                operator_result = emit_compare(o, "icmp sge", operands[0], operands[1]);
                break;
            case InsnSemanticsExpr::OP_SGT:
                assert(2==operands.size());
                operator_result = emit_compare(o, "icmp sgt", operands[0], operands[1]);
                break;
            case InsnSemanticsExpr::OP_SHL0:
                assert(2==operands.size());
                operator_result = emit_left_shift(o, operands[1], operands[0]);
                break;
            case InsnSemanticsExpr::OP_SHL1:
                assert(2==operands.size());
                operator_result = emit_left_shift_ones(o, operands[1], operands[0]);
                break;
            case InsnSemanticsExpr::OP_SHR0:
                assert(2==operands.size());
                operator_result = emit_logical_right_shift(o, operands[1], operands[0]);
                break;
            case InsnSemanticsExpr::OP_SHR1:
                assert(2==operands.size());
                operator_result = emit_logical_right_shift_ones(o, operands[1], operands[0]);
                break;
            case InsnSemanticsExpr::OP_SLE:
                assert(2==operands.size());
                operator_result = emit_compare(o, "icmp sle", operands[0], operands[1]);
                break;
            case InsnSemanticsExpr::OP_SLT:
                assert(2==operands.size());
                operator_result = emit_compare(o, "icmp slt", operands[0], operands[1]);
                break;
            case InsnSemanticsExpr::OP_SMOD:
                assert(2==operands.size());
                operator_result = emit_signed_modulo(o, operands[0], operands[1]);
                break;
            case InsnSemanticsExpr::OP_SMUL:
                operator_result = emit_signed_multiply(o, operands);
                break;
            case InsnSemanticsExpr::OP_UDIV:
                assert(2==operands.size());
                operator_result = emit_unsigned_divide(o, operands[0], operands[1]);
                break;
            case InsnSemanticsExpr::OP_UEXTEND:
                assert(2==operands.size());
                operator_result = emit_zero_extend(o, operands[1], inode->get_nbits());
                break;
            case InsnSemanticsExpr::OP_UGE:
                assert(2==operands.size());
                operator_result = emit_compare(o, "icmp uge", operands[0], operands[1]);
                break;
            case InsnSemanticsExpr::OP_UGT:
                assert(2==operands.size());
                operator_result = emit_compare(o, "icmp ugt", operands[0], operands[1]);
                break;
            case InsnSemanticsExpr::OP_ULE:
                assert(2==operands.size());
                operator_result = emit_compare(o, "icmp ule", operands[0], operands[1]);
                break;
            case InsnSemanticsExpr::OP_ULT:
                assert(2==operands.size());
                operator_result = emit_compare(o, "icmp ult", operands[0], operands[1]);
                break;
            case InsnSemanticsExpr::OP_UMOD:
                assert(2==operands.size());
                operator_result = emit_unsigned_modulo(o, operands[0], operands[1]);
                break;
            case InsnSemanticsExpr::OP_UMUL:
                operator_result = emit_unsigned_multiply(o, operands);
                break;
            case InsnSemanticsExpr::OP_ZEROP:
                assert(1==operands.size());
                operator_result = emit_compare(o, "icmp eq", operands[0], LeafNode::create_integer(operands[0]->get_nbits(), 0));
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
        assert(operator_result!=NULL);                  // no case was executed
        cur_expr = operator_result;
    }

    // The return value must be a constant or variable
    LeafNodePtr retval = cur_expr->isLeafNode();
    assert(retval!=NULL && (retval->is_known() || retval->is_variable()));

    // Add a rewrite rule so that next time we're asked to emit the same expression we can just emit the result without going
    // through all this work again.
    add_rewrite(orig_expr, retval);
    return retval;
}

void
RiscOperators::add_rewrite(const TreeNodePtr &from, const LeafNodePtr &to)
{
    assert(from!=NULL && to!=NULL);
    if (from==to) {
        rewrites.erase(from->hash());
    } else {
        rewrites.insert(std::make_pair(from->hash(), to));
        add_variable(to);
    }
}

std::string
RiscOperators::add_variable(const LeafNodePtr &var)
{
    assert(var!=NULL && var->is_variable());
    std::string name = get_variable(var);
    if (name.empty()) {
        name = var->get_comment();
        if (name.empty()) {
            name = "%v" + StringUtility::numberToString(var->get_name());
        } else if (name.size()>2 && 0==name.substr(name.size()-2).compare("_0")) {
            name = "@" + name.substr(0, name.size()-2);
        } else {
            name = "@" + name;
        }
        variables.insert(std::make_pair(var->get_name(), name));
    }
    return name;
}

std::string
RiscOperators::get_variable(const LeafNodePtr &var)
{
    assert(var!=NULL && var->is_variable());
    return variables.get_value_or(var->get_name(), "");
}

LeafNodePtr
RiscOperators::emit_assignment(std::ostream &o, const TreeNodePtr &rhs)
{
    assert(rhs!=NULL);
    LeafNodePtr t1 = emit_expression(o, rhs);

    if (t1->is_variable() && t1->get_comment().empty())
        return t1;

    LeafNodePtr lhs = next_temporary(rhs->get_nbits());
    o <<prefix() <<llvm_lvalue(lhs) <<" = " <<llvm_integer_type(rhs->get_nbits()) <<" " <<llvm_term(t1) <<"\n";
    add_rewrite(rhs, lhs);
    return lhs;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Transcoder
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
Transcoder::emitFilePrologue(std::ostream &o)
{
    operators->emit_register_declarations(o, operators->get_important_registers());
}

std::string
Transcoder::emitFilePrologue()
{
    std::ostringstream ss;
    emitFilePrologue(ss);
    return ss.str();
}

void
Transcoder::emitFunctionDeclarations(SgNode *ast, std::ostream &o)
{
    struct T1: AstSimpleProcessing {
        RiscOperators *ops;
        std::ostream &o;
        T1(RiscOperators *ops, std::ostream &o): ops(ops), o(o) {}
        void visit(SgNode *node) {
            if (SgAsmFunction *func = isSgAsmFunction(node))
                o <<ops->prefix() <<"declare void " <<ops->function_label(func) <<"()\n";
        }
    } t1(operators.get(), o);
    t1.traverse(ast, preorder);
}

std::string
Transcoder::emitFunctionDeclarations(SgNode *ast)
{
    std::ostringstream ss;
    emitFunctionDeclarations(ast, ss);
    return ss.str();
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

size_t
Transcoder::transcodeBasicBlock(SgAsmBlock *bb, std::ostream &o)
{
    assert(this!=NULL);
    if (!bb)
        return 0;
    size_t ninsns = 0;                                  // number of instructions emitted
    RiscOperators::Indent indent(operators);
    RoseAst ast(bb);
    operators->reset();
    SgAsmInstruction *last_insn = NULL;
    for (RoseAst::iterator ai=ast.begin().withoutNullValues(); ai!=ast.end(); ++ai) {
        if (SgAsmInstruction *insn = isSgAsmInstruction(*ai)) {
            ai.skipChildrenOnForward();
            if (0==ninsns++)
                o <<"\n" <<operators->prefix() <<operators->addr_label(bb->get_address()) <<":\n";
            o <<operators->prefix() <<"; " <<StringUtility::addrToString(insn->get_address())
              <<": " <<unparseInstruction(insn) <<"\n";
            last_insn = insn;
            dispatcher->processInstruction(insn);
#if 0 /*DEBUGGING [Robb P. Matzke 2014-01-14]*/
            std::ostringstream ss;
            ss <<*operators->get_state();
            o <<StringUtility::prefixLines(ss.str(), operators->prefix() + "    ;; ");
#endif
            // In order to avoid issues of memory aliasing, whenever a write occurs we dump the current machine state to
            // LLVM. We're assuming that a machine instruction performs at most one write and if it performs a memory write
            // then it doesn't also perform a memory read (or it performs the memory read first, as in test-and-set).
            if (!operators->get_memory_writes().empty()) {
                RiscOperators::Indent indent2(operators);
                operators->emit_changed_state(o);
            }
        }
    }

    if (ninsns>0) {
        RiscOperators::Indent indent2(operators);
        operators->emit_changed_state(o);
        if (last_insn)
            operators->emit_next_eip(o, last_insn);
    }
    return ninsns;
}

std::string
Transcoder::transcodeBasicBlock(SgAsmBlock *bb)
{
    std::ostringstream ss;
    transcodeBasicBlock(bb, ss);
    return ss.str();
}

size_t
Transcoder::transcodeFunction(SgAsmFunction *func, std::ostream &o)
{
    assert(this!=NULL);
    if (!func)
        return 0;
    size_t nbbs = 0;                                    // number of basic blocks emitted

    o <<operators->prefix() <<"define void " <<operators->function_label(func) <<"() {\n";
    RiscOperators::Indent func_body_indentation(operators);

    // Function prologue. Note that in LLVM the first basic block of a function cannot be the target of a branch instruction.
    // Therefore, we emit a no-op as the first basic block so that ROSE's first basic block becomes the second basic block for
    // LLVM.
    {
        RiscOperators::Indent indentation(operators, 2);
        std::string label = operators->addr_label(func->get_entry_va());
        o <<operators->prefix() <<"br label %" <<label <<"\n";
    }

    // Function body.
    const SgAsmStatementPtrList &bbs = func->get_statementList();
    for (SgAsmStatementPtrList::const_iterator bbi=bbs.begin(); bbi!=bbs.end(); ++bbi) {
        SgAsmBlock *bb = isSgAsmBlock(*bbi);
        assert(bb!=NULL);
        int ninsns = transcodeBasicBlock(bb, o);
        if (ninsns>0)
            ++nbbs;
    }

    // Function epilogue.  This is to handle ROSE functions that have no basic blocks--they still need to be valid functions in
    // LLVM.
    if (0==nbbs) {
        RiscOperators::Indent label_indentation(operators);
        std::string label = operators->addr_label(func->get_entry_va());
        o <<"\n" <<operators->prefix() <<label <<"\n";
        {
            RiscOperators::Indent insn_indentation(operators);
            o <<operators->prefix() <<"br label %" <<label <<"\n";
        }
    }
    
    o <<"}\n";
    return nbbs;
}

std::string
Transcoder::transcodeFunction(SgAsmFunction *func)
{
    std::ostringstream ss;
    transcodeFunction(func, ss);
    return ss.str();
}

void
Transcoder::transcodeInterpretation(SgAsmInterpretation *interp, std::ostream &o)
{
    o <<"; Register declarations\n";
    emitFilePrologue(o);

    o <<"\n; Function declarations\n";
    emitFunctionDeclarations(interp, o);

    std::vector<SgAsmFunction*> functions = SageInterface::querySubTree<SgAsmFunction>(interp);
    for (size_t i=0; i<functions.size(); ++i) {
        o <<"\n\n" <<std::string(100, ';') <<"\n";
        transcodeFunction(functions[i], o);
    }
}

std::string
Transcoder::transcodeInterpretation(SgAsmInterpretation *interp)
{
    std::ostringstream ss;
    transcodeInterpretation(interp);
    return ss.str();
}

} // namespace
} // namespace
} // namespace
