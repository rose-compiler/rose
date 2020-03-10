#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
#include "sage3basic.h"

#include "Diagnostics.h"
#include "LlvmSemantics2.h"
#include "AsmUnparser_compat.h"
#include "integerOps.h"
#include "stringify.h"

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace LlvmSemantics {

using namespace Rose::Diagnostics;

static unsigned nVersionWarnings = 0;

BaseSemantics::SValuePtr
RiscOperators::readMemory(RegisterDescriptor segreg, const BaseSemantics::SValuePtr &addr_,
                          const BaseSemantics::SValuePtr &dflt, const BaseSemantics::SValuePtr &cond)
{
    if (cond->is_number() && !cond->get_number())
        return dflt;
    size_t nbits = dflt->get_width();
    SValuePtr addr = SValue::promote(addr_);
    return svalue_expr(SymbolicExpr::makeRead(SymbolicExpr::makeMemoryVariable(addr->get_width(), nbits), addr->get_expression(),
                                              solver()));
}

void
RiscOperators::writeMemory(RegisterDescriptor segreg, const BaseSemantics::SValuePtr &addr_,
                           const BaseSemantics::SValuePtr &data_, const BaseSemantics::SValuePtr &cond)
{
    if (cond->is_number() && !cond->get_number())
        return;
    SValuePtr addr = SValue::promote(addr_);
    SValuePtr data = SValue::promote(data_);
    mem_writes.push_back(SymbolicExpr::makeWrite(SymbolicExpr::makeMemoryVariable(addr->get_width(), data->get_width()),
                                                 addr->get_expression(), data->get_expression(), solver())
                         ->isInteriorNode());
}

void
RiscOperators::reset()
{
    BaseSemantics::StatePtr state = currentState();
    BaseSemantics::RegisterStatePtr regs = state->registerState();
    BaseSemantics::MemoryStatePtr mem = state->memoryState();

    RegisterStatePtr new_regs = RegisterState::promote(regs->create(protoval(), regs->get_register_dictionary()));
    BaseSemantics::MemoryStatePtr new_mem = mem->create(mem->get_addr_protoval(), mem->get_val_protoval());
    BaseSemantics::StatePtr new_state = state->create(new_regs, new_mem);

    new_regs->initialize_nonoverlapping(get_important_registers(), false);

    currentState(new_state);
    rewrites.clear();
    variables.clear();
    prev_regstate.reset();
    mem_writes.clear();
}

void
RiscOperators::emit_changed_state(std::ostream &o)
{
    const RegisterDictionary *dictionary = currentState()->registerState()->get_register_dictionary();
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
    for (int i=0; i<indent_level; ++i)
        retval += indent_string;
    return retval;
}

// FIXME[Robb P. Matzke 2014-01-07]: this is x86 specific.
const RegisterDescriptors &
RiscOperators::get_important_registers()
{
    if (important_registers.empty()) {
        ASSERT_not_null(currentState());
        const RegisterDictionary *dictionary = currentState()->registerState()->get_register_dictionary();

        // General-purpose registers
        important_registers.push_back(*dictionary->lookup("eax"));
        important_registers.push_back(*dictionary->lookup("ebx"));
        important_registers.push_back(*dictionary->lookup("ecx"));
        important_registers.push_back(*dictionary->lookup("edx"));
        important_registers.push_back(*dictionary->lookup("esp"));
        important_registers.push_back(*dictionary->lookup("ebp"));
        important_registers.push_back(*dictionary->lookup("esi"));
        important_registers.push_back(*dictionary->lookup("edi"));
        important_registers.push_back(*dictionary->lookup("eip"));

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

        // Floating point stuff, probably not handled too well yet.
        important_registers.push_back(*dictionary->lookup("fpstatus"));
        important_registers.push_back(*dictionary->lookup("fpctl"));
        important_registers.push_back(*dictionary->lookup("mxcsr"));
        important_registers.push_back(*dictionary->lookup("mm0"));
        important_registers.push_back(*dictionary->lookup("mm1"));
        important_registers.push_back(*dictionary->lookup("mm2"));
        important_registers.push_back(*dictionary->lookup("mm3"));
        important_registers.push_back(*dictionary->lookup("mm4"));
        important_registers.push_back(*dictionary->lookup("mm5"));
        important_registers.push_back(*dictionary->lookup("mm6"));
        important_registers.push_back(*dictionary->lookup("mm7"));
        important_registers.push_back(*dictionary->lookup("xmm0"));
        important_registers.push_back(*dictionary->lookup("xmm1"));
        important_registers.push_back(*dictionary->lookup("xmm2"));
        important_registers.push_back(*dictionary->lookup("xmm3"));
        important_registers.push_back(*dictionary->lookup("xmm4"));
        important_registers.push_back(*dictionary->lookup("xmm5"));
        important_registers.push_back(*dictionary->lookup("xmm6"));
        important_registers.push_back(*dictionary->lookup("xmm7"));
    }
    return important_registers;
}

// Important registers that have a value
RegisterDescriptors
RiscOperators::get_stored_registers()
{
    RegisterDescriptors retval;
    RegisterStatePtr regstate = RegisterState::promote(currentState()->registerState());
    const RegisterDictionary *dictionary = regstate->get_register_dictionary();
    const std::vector<RegisterDescriptor> &regs = get_important_registers();
    for (size_t i=0; i<regs.size(); ++i) {
        if (regstate->is_partly_stored(regs[i])) {
            const std::string &name = dictionary->lookup(regs[i]);
            ASSERT_require(!name.empty());
            BaseSemantics::SValuePtr dflt = undefined_(regs[i].nBits());
            SValuePtr value = SValue::promote(regstate->readRegister(regs[i], dflt, this));

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
    RegisterStatePtr cur_regstate = RegisterState::promote(currentState()->registerState());
    const RegisterDictionary *dictionary = cur_regstate->get_register_dictionary();
    const std::vector<RegisterDescriptor> &regs = get_important_registers();
    for (size_t i=0; i<regs.size(); ++i) {
        if (cur_regstate->is_partly_stored(regs[i])) {
            const std::string &name = dictionary->lookup(regs[i]);
            ASSERT_require(!name.empty());
            BaseSemantics::SValuePtr dflt = undefined_(regs[i].nBits());
            SValuePtr cur_value = SValue::promote(cur_regstate->readRegister(regs[i], dflt, this));
            if (0==cur_value->get_comment().compare(name + "_0")) {
                // This register has it's initial value, probably because it was read (registers that have never been read or
                // written won't even get this far in the loop due to the is_partly_stored() check above.
                continue;
            } else if (prev_regstate!=NULL && prev_regstate->is_partly_stored(regs[i])) {
                SValuePtr prev_value = SValue::promote(prev_regstate->readRegister(regs[i], dflt, this));
                if (cur_value->must_equal(prev_value))
                    continue;
            }
            retval.push_back(regs[i]);
        }
    }
    return retval;
}

// FIXME[Robb P. Matzke 2014-01-07]: This is x86 specific.
RegisterDescriptor
RiscOperators::get_insn_pointer_register()
{
    const RegisterDictionary *dictionary = currentState()->registerState()->get_register_dictionary();
    return *dictionary->lookup("eip");
}

SValuePtr
RiscOperators::get_instruction_pointer()
{
    RegisterDescriptor EIP = get_insn_pointer_register();
    BaseSemantics::SValuePtr dflt = undefined_(EIP.nBits());
    return SValue::promote(currentState()->registerState()->readRegister(EIP, dflt, this));
}

// Create temporary LLVM variables for all definers of the specified registers.
void
RiscOperators::emit_prerequisites(std::ostream &o, const RegisterDescriptors &regs, const RegisterDictionary *dictionary)
{
    struct T1: SymbolicExpr::Visitor {
        RiscOperators *ops;
        std::ostream &o;
        const RegisterDescriptors &regs;
        const RegisterDictionary *dictionary;
        std::set<SymbolicExpr::Hash> seen;
        T1(RiscOperators *ops, std::ostream &o, const RegisterDescriptors &regs, const RegisterDictionary *dictionary)
            : ops(ops), o(o), regs(regs), dictionary(dictionary) {}
        virtual SymbolicExpr::VisitAction preVisit(const ExpressionPtr &node) ROSE_OVERRIDE {
            if (!seen.insert(node->hash()).second)
                return SymbolicExpr::TRUNCATE; // already processed this same expression
            size_t width = node->nBits();
            if (InteriorPtr inode = node->isInteriorNode()) {
                if (SymbolicExpr::OP_READ==inode->getOperator()) {
                    ASSERT_require(2==inode->nChildren());
                    ops->emit_assignment(o, ops->emit_memory_read(o, inode->child(1), width));
                }
            } else {
                LeafPtr leaf = node->isLeafNode();
                ASSERT_not_null(leaf);
                if (leaf->isIntegerVariable()) {
                    std::string comment = leaf->comment();
                    if (comment.size()>2 && 0==comment.substr(comment.size()-2).compare("_0"))
                        ops->add_variable(leaf);        // becomes a global variable if not already
                    LeafPtr t1 = ops->emit_expression(o, leaf);// handles local vars, global vars, and undefs
                }
            }
            return SymbolicExpr::CONTINUE;
        }
        virtual SymbolicExpr::VisitAction postVisit(const ExpressionPtr&) ROSE_OVERRIDE {
            return SymbolicExpr::CONTINUE;
        }
    } t1(this, o, regs, dictionary);

    // Prerequisites for the registers
    RegisterStatePtr regstate = RegisterState::promote(currentState()->registerState());
    for (size_t i=0; i<regs.size(); ++i) {
        BaseSemantics::SValuePtr dflt = undefined_(regs[i].nBits());
        SValuePtr value = SValue::promote(regstate->readRegister(regs[i], dflt, this));
        value->get_expression()->depthFirstTraversal(t1);
    }

    // Prerequisites for memory writes
    for (TreeNodes::const_iterator mwi=mem_writes.begin(); mwi!=mem_writes.end(); ++mwi) {
        const ExpressionPtr mem_write = *mwi;
        mem_write->depthFirstTraversal(t1);
    }

    // Prerequisites for the instruction pointer.
    get_instruction_pointer()->get_expression()->depthFirstTraversal(t1);
}

void
RiscOperators::emit_register_declarations(std::ostream &o, const RegisterDescriptors &regs)
{
    const RegisterDictionary *dictionary = currentState()->registerState()->get_register_dictionary();
    for (size_t i=0; i<regs.size(); ++i) {
        const std::string &name = dictionary->lookup(regs[i]);
        ASSERT_require(!name.empty());
        o <<prefix() <<"@" <<name <<" = external global " <<llvm_integer_type(regs[i].nBits()) <<"\n";
    }
}

void
RiscOperators::emit_register_definitions(std::ostream &o, const RegisterDescriptors &regs)
{
    RegisterStatePtr regstate = RegisterState::promote(currentState()->registerState());
    const RegisterDictionary *dictionary = regstate->get_register_dictionary();
    for (size_t i=0; i<regs.size(); ++i) {
        const std::string &name = dictionary->lookup(regs[i]);
        ASSERT_require(!name.empty());
        BaseSemantics::SValuePtr dflt = undefined_(regs[i].nBits());
        SValuePtr value = SValue::promote(regstate->readRegister(regs[i], dflt, this));
        o <<prefix() <<"; register " <<name <<" = " <<*value <<"\n";
        ExpressionPtr t1 = emit_expression(o, value);
        o <<prefix() <<"store " <<llvm_integer_type(t1->nBits()) <<" " <<llvm_term(t1)
          <<", " <<llvm_integer_type(t1->nBits()) <<"* @" <<name <<"\n";
    }
}

void
RiscOperators::emit_next_eip(std::ostream &o, SgAsmInstruction *latest_insn)
{
    using namespace SageInterface;

    SgAsmBlock *bb = getEnclosingNode<SgAsmBlock>(latest_insn);
    SgAsmFunction *func = getEnclosingNode<SgAsmFunction>(bb);
    SgAsmInterpretation *interp = getEnclosingNode<SgAsmInterpretation>(func);
    ASSERT_not_null(interp);                            // instructions must be part of the global AST
    const InstructionMap &insns = interp->get_instruction_map();
    SValuePtr eip = get_instruction_pointer();
    rose_addr_t fallthrough_va = latest_insn->get_address() + latest_insn->get_size();

    // If EIP is a constant then it is one of the following cases:
    //    1. It points to an instruction that's in a different function than the last executed instruction, in which
    //       case we treat it like a function call.  LLVM requires us to treat all inter-function control flow as a
    //       function call even though the same restriction isn't present in the binary. FIXME[Robb P. Matzke 2014-01-09]
    //    2. It is an unconditional intra-function branch which can be translated to an LLVM unconditional "br" instruction.
    //    3. It is the fall-through address added by transcodeBasicBlock for an instruction for which semantics failed when
    //       quiet-errors mode is enabled, and therefore might be completely invalid.
    if (eip->is_number()) {
        SgAsmInstruction *dst_insn = insns.get_value_or(eip->get_number(), NULL);
        SgAsmFunction *dst_func = getEnclosingNode<SgAsmFunction>(dst_insn);
        if (!dst_func) {
            o <<prefix() <<"unreachable\n";
        } else if (func!=dst_func) {                    // func could be null
            std::string funcname = function_label(dst_func);
            o <<prefix() <<"call void " <<funcname <<"()\n";
            rose_addr_t ret_addr = fallthrough_va;
            SgAsmFunction *ret_func = getEnclosingNode<SgAsmFunction>(insns.get_value_or(ret_addr, NULL));
            if (ret_func!=func) {
                // The fall through address might be invalid or in a different function if the call never returns.
                o <<prefix() <<"unreachable\n";
            } else {
                o <<prefix() <<"br label %" <<addr_label(ret_addr) <<"\n";
            }
        } else {
            o <<prefix() <<"br label %" <<addr_label(eip->get_number()) <<"\n";
        }
        return;
    }

    o <<prefix() <<"; register eip = " <<*eip <<"\n";

    // If EIP is a symbolic if-then-else ("ite") and both operands are constants then the binary has a conditional branch
    // instruction (like an x86 "je", "jne", etc.) and we can emit an LLVM conditional "br" with true and false parts. However,
    // we must watch out for the case when the ROSE disassembler determined that the predicate is opaque and one of the target
    // addresses isn't valid.  This can happen because the ROSE disassembler might be using a more advanced analysis than we
    // use here.
    InteriorPtr inode = eip->get_expression()->isInteriorNode();
    if (inode && SymbolicExpr::OP_ITE==inode->getOperator()) {
        LeafPtr leaf1 = inode->child(1)->isLeafNode();
        LeafPtr leaf2 = inode->child(2)->isLeafNode();
        if (leaf1!=NULL && leaf1->isIntegerConstant() && leaf2!=NULL && leaf2->isIntegerConstant()) {
            rose_addr_t true_va = leaf1->toUnsigned().get();
            rose_addr_t false_va = leaf2->toUnsigned().get();
            if (false_va != fallthrough_va)
                std::swap(true_va, false_va);
            SgAsmFunction *true_func = getEnclosingNode<SgAsmFunction>(insns.get_value_or(true_va, NULL));
            SgAsmFunction *false_func = getEnclosingNode<SgAsmFunction>(insns.get_value_or(false_va, NULL));
            const SgAsmIntegerValuePtrList &succs = bb->get_successors();
            std::vector<rose_addr_t> succs_va;
            for (SgAsmIntegerValuePtrList::const_iterator si=succs.begin(); si!=succs.end(); ++si)
                succs_va.push_back((*si)->get_absoluteValue());

            if (succs.size()==2 && true_func==func && false_func==func &&
                std::min(succs_va[0], succs_va[1])==std::min(true_va, false_va) &&
                std::max(succs_va[0], succs_va[1])==std::max(true_va, false_va)) {
                // This is a normal intra-function conditional branch that can be translated directly to an LLVM "br"
                LeafPtr t1 = emit_expression(o, inode->child(0));
                o <<prefix() <<"br i1 " <<llvm_term(t1)
                  <<", label %" <<addr_label(true_va) <<", label %" <<addr_label(false_va) <<"\n";
                return;
            } else if (succs.size()==1 && (succs_va[0]==true_va || succs_va[0]==false_va) &&
                       getEnclosingNode<SgAsmFunction>(insns.get_value_or(succs_va[0], NULL))==func) {
                // An intra-function conditional branch with opaque predicate.
                o <<prefix() <<"br label %" <<addr_label(succs_va[0]) <<"\n";
                return;
            } else if (true_func==func && false_func==func) {
                // CFG succs info is fishy, but both values of the "ite" are valid intra-function blocks
                LeafPtr t1 = emit_expression(o, inode->child(0));
                o <<prefix() <<"br i1 " <<llvm_term(t1)
                  <<", label %" <<addr_label(true_va) <<", label %" <<addr_label(false_va) <<"\n";
                return;
            }
        }
    }

    // If the last executed instruction is some kind of function return instruction, then emit an LLVM "ret" instruction.
    // The stack used by LLVM is distinct from the stack used by the binary. The latter's stack is implemented via the @ebp (or
    // similar) global pointer.  FIXME[Robb P. Matzke 2014-01-09]: This is architecture dependent.
    if (SgAsmX86Instruction *insn_x86 = isSgAsmX86Instruction(latest_insn)) {
        if (insn_x86->get_kind() == x86_ret || insn_x86->get_kind() == x86_retf) {
            o <<prefix() <<"ret void\n";
            return;
        }
    }

    // If this function is a thunk, then we need to treat it as an LLVM function call (because LLVM doesn't allow
    // inter-function branches).  FIXME[Robb P. Matzke 2014-01-09]: This is architecture dependent.
    if (SgAsmX86Instruction *insn_x86 = isSgAsmX86Instruction(latest_insn)) {
        std::vector<SgAsmInstruction*> func_insns = querySubTree<SgAsmInstruction>(func);
        if (func_insns.size()==1 && func_insns.front()==insn_x86 &&
            (insn_x86->get_kind() == x86_jmp || insn_x86->get_kind() == x86_farjmp)) {
            LeafPtr t1 = emit_expression(o, eip);
            LeafPtr t2 = next_temporary(32);        // pointer to the function
            o <<prefix() <<llvm_lvalue(t2) <<" = inttoptr "
              <<llvm_integer_type(t1->nBits()) <<" " <<llvm_term(t1) <<" to void()*\n";
            o <<prefix() <<"call void " <<llvm_term(t2) <<"()\n";
            o <<prefix() <<"ret void\n";
            return;
        }
    }

    // If we don't know the target address and this is an indirect function call, then the successors are the entry points of
    // all known functions.  FIXME[Robb P. Matzke 2014-01-09]: Detection of a function call is architecture dependent.
    if (SgAsmX86Instruction *insn_x86 = isSgAsmX86Instruction(latest_insn)) {
        if (insn_x86->get_kind() == x86_call || insn_x86->get_kind() == x86_farcall) {
            LeafPtr t1 = emit_expression(o, eip);
            LeafPtr t2 = next_temporary(32);        // pointer to the function
            std::string ret_label = addr_label(latest_insn->get_address() + latest_insn->get_size());
            o <<prefix() <<llvm_lvalue(t2) <<" = inttoptr "
              <<llvm_integer_type(t1->nBits()) <<" " <<llvm_term(t1) <<" to void()*\n";
            o <<prefix() <<"call void " <<llvm_term(t2) <<"()\n";
            o <<prefix() <<"br label %" <<ret_label <<"\n";
            return;
        }
    }

    // Catch-all: this must be an intra-function indirect branch.  LLVM requires us to enumerate all possible targets, but
    // since we don't actually know them we must enumerate all basic blocks of the function.  Note that LLVM 2.5 does not have
    // the "indirectbr" instruction, so we need to use the "switch" instruction.
    {
        LeafPtr t1 = emit_expression(o, eip);
        std::string type = llvm_integer_type(t1->nBits());
        std::string dflt_label = next_label();
        o <<prefix() <<"switch " <<type <<" " <<llvm_term(t1) <<", label %" <<dflt_label <<" [";
        for (InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
            if (getEnclosingNode<SgAsmFunction>(ii->second)==func && ii->second->isFirstInBlock())
                o <<" " <<type <<" " <<ii->first <<", label %" <<addr_label(ii->first);
        }
        o <<" ]\n";
        {
            Indent label_undent(this, -1);
            o <<prefix() <<dflt_label <<":\n";
        }
        o <<prefix() <<"unreachable\n";
        return;
    }
}

void
RiscOperators::emit_memory_writes(std::ostream &o)
{
    for (size_t i=0; i<mem_writes.size(); ++i) {
        InteriorPtr inode = mem_writes[i]->isInteriorNode();
        ASSERT_not_null(inode);
        ASSERT_require(inode->getOperator() == SymbolicExpr::OP_WRITE);
        ASSERT_require(inode->nChildren()==3);
        ExpressionPtr addr = inode->child(1);
        ExpressionPtr value = inode->child(2);
        o <<prefix() <<"; store value=" <<*value <<" at address=" <<*addr <<"\n";
        emit_memory_write(o, addr, value);
    }
}

void
RiscOperators::make_current()
{
    prev_regstate = RegisterState::promote(currentState()->registerState()->clone());
    mem_writes.clear();
}

LeafPtr
RiscOperators::emit_expression(std::ostream &o, const SValuePtr &value)
{
    ASSERT_not_null(value);
    LeafPtr result = emit_expression(o, value->get_expression());
    ASSERT_not_null(result);
    return result;
}

std::string
RiscOperators::llvm_integer_type(size_t width)
{
    ASSERT_require(width>0);
    return "i" + StringUtility::numberToString(width);
}

std::string
RiscOperators::llvm_lvalue(const LeafPtr &var)
{
    ASSERT_require(var && var->isIntegerVariable());
    ASSERT_require(!variables.exists(var->nameId()));         // LLVM assembly is SSA
    return add_variable(var);
}

std::string
RiscOperators::llvm_term(const ExpressionPtr &expr)
{
    LeafPtr leaf = expr->isLeafNode();
    ASSERT_not_null(leaf);
    leaf = rewrites.get_value_or(leaf->hash(), leaf);

    if (leaf->isIntegerConstant()) {
        int64_t sv = leaf->toSigned().get();
        return StringUtility::numberToString(sv);
    }

    std::string name = get_variable(leaf);
    ASSERT_require(!name.empty());
    return name;
}

LeafPtr
RiscOperators::next_temporary(size_t nbits)
{
    return SymbolicExpr::makeIntegerVariable(nbits);
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

// Label for a function. Some ROSE functions don't have names. Sometimes two functions have the same name.  Therefore, we
// generate the return value as a combination of unique address and optional, non-unique name.
std::string
RiscOperators::function_label(SgAsmFunction *func)
{
    ASSERT_not_null(func);
    std::string retval = "L_" + StringUtility::addrToString(func->get_entry_va());
    std::string fname = func->get_name();
    if (!fname.empty())
        retval += "_" + fname;

    for (size_t i=0; i<retval.size(); ++i) {
        if ('_'!=retval[i] && !isalnum(retval[i]))
            retval[i] = '.';
    }
    return "@" + retval;
}

// Emit an LLVM instruction to zero-extend a value if necessary.  If the value is already the specified width then this is a
// no-op.
ExpressionPtr
RiscOperators::emit_zero_extend(std::ostream &o, const ExpressionPtr &value, size_t nbits)
{
    ASSERT_not_null(value);
    ASSERT_require(value->nBits() <= nbits);
    if (value->nBits() == nbits)
        return value;

    LeafPtr t1 = emit_expression(o, value);
    std::string t1_type = llvm_integer_type(t1->nBits());
    LeafPtr t2 = next_temporary(nbits);
    std::string t2_type = llvm_integer_type(nbits);
    o <<prefix() <<llvm_lvalue(t2) <<" = zext " <<t1_type <<" " <<llvm_term(t1) <<" to " <<t2_type <<"\n";
    return t2;
}

// Emit an LLVM instruction to sign-extend a value.  If the value is already the specified width then this is a no-op.
ExpressionPtr
RiscOperators::emit_sign_extend(std::ostream &o, const ExpressionPtr &value, size_t nbits)
{
    ASSERT_not_null(value);
    ASSERT_require(value->nBits() <= nbits);
    if (value->nBits() == nbits)
        return value;

    ExpressionPtr t1 = emit_expression(o, value);
    std::string t1_type = llvm_integer_type(t1->nBits());
    LeafPtr t2 = next_temporary(nbits);
    std::string t2_type = llvm_integer_type(nbits);
    o <<prefix() <<llvm_lvalue(t2) <<" = sext " <<t1_type <<" " <<llvm_term(t1) <<" to " <<t2_type <<"\n";
    return t2;
}

// Emit an LLVM instruction to truncate a value if necessary.  If the value is already the specified width then this is
// a no-op.
ExpressionPtr
RiscOperators::emit_truncate(std::ostream &o, const ExpressionPtr &value, size_t nbits)
{
    ASSERT_not_null(value);
    ASSERT_require(value->nBits() >= nbits);
    if (value->nBits() == nbits)
        return value;

    ExpressionPtr t1 = emit_expression(o, value);
    std::string t1_type = llvm_integer_type(t1->nBits());
    LeafPtr t2 = next_temporary(nbits);
    std::string t2_type = llvm_integer_type(nbits);
#if 1 /*DEBUGGING [Robb P. Matzke 2014-01-15]*/
    ASSERT_require(0!=t1_type.compare(t2_type));
#endif
    o <<prefix() <<llvm_lvalue(t2) <<" = trunc " <<t1_type <<" " <<llvm_term(t1) <<" to " <<t2_type <<"\n";
    return t2;
}

// Emit LLVM to make value the specified size by zero extending or truncating as necessary.  This is a no-op if the value
// is already the desired size.
ExpressionPtr
RiscOperators::emit_unsigned_resize(std::ostream &o, const ExpressionPtr &value, size_t nbits)
{
    if (value->nBits()==nbits)
        return value;
    if (value->nBits() < nbits)
        return emit_zero_extend(o, value, nbits);
    return emit_truncate(o, value, nbits);
}

// Emits an LLVM binary operator.  Both operands must be the same width.
ExpressionPtr
RiscOperators::emit_binary(std::ostream &o, const std::string &llvm_operator, const ExpressionPtr &a, const ExpressionPtr &b)
{
    ASSERT_not_null(a);
    ASSERT_not_null(b);
    ASSERT_require(a->nBits() == b->nBits());
    std::string type = llvm_integer_type(a->nBits());
    ExpressionPtr t1 = emit_expression(o, a);
    ExpressionPtr t2 = emit_expression(o, b);
    LeafPtr t3 = next_temporary(a->nBits());
    o <<prefix() <<llvm_lvalue(t3) <<" = " <<llvm_operator
      <<" " <<type <<" " <<llvm_term(t1) <<", " <<llvm_term(t2) <<"\n";
    return t3;
}

// Emits an LLVM binary operator.  The width of the result is the maximum width of the operands. The narrower of the two
// operands is sign extended to the same width as the result.
ExpressionPtr
RiscOperators::emit_signed_binary(std::ostream &o, const std::string &llvm_operator, const ExpressionPtr &a, const ExpressionPtr &b)
{
    ASSERT_not_null(a);
    ASSERT_not_null(b);
    size_t width = std::max(a->nBits(), b->nBits());
    ExpressionPtr t1 = emit_sign_extend(o, a, width);
    ExpressionPtr t2 = emit_sign_extend(o, b, width);
    return emit_binary(o, llvm_operator, t1, t2);
}

// Emits an LLVM binary operator.  The width of the result is the maximum width of the operands. The narrower of the two
// operands is zero extended to the same width as the result.
ExpressionPtr
RiscOperators::emit_unsigned_binary(std::ostream &o, const std::string &llvm_operator,
                                    const ExpressionPtr &a, const ExpressionPtr &b)
{
    ASSERT_not_null(a);
    ASSERT_not_null(b);
    size_t width = std::max(a->nBits(), b->nBits());
    ExpressionPtr t1 = emit_zero_extend(o, a, width);
    ExpressionPtr t2 = emit_zero_extend(o, b, width);
    return emit_binary(o, llvm_operator, t1, t2);
}

// Emit an LLVM logical left shift expression if necessary.  If the shift amount is the constant zero then this is a no-op.
// LLVM requires that the shift amount be the same width as the value being shifted.  The shift amount is interpreted as
// unsigned.
ExpressionPtr
RiscOperators::emit_logical_right_shift(std::ostream &o, const ExpressionPtr &value, const ExpressionPtr &amount)
{
    if (LeafPtr amount_leaf = amount->isLeafNode()) {
        if (amount_leaf->isIntegerConstant()) {
            if (amount_leaf->toUnsigned().get() == 0)
                return value;
            if (amount_leaf->toUnsigned().get() >= value->nBits())
                return SymbolicExpr::makeIntegerConstant(value->nBits(), 0);
        }
    }
    return emit_binary(o, "lshr", value, emit_unsigned_resize(o, amount, value->nBits()));
}

// Emit the LLVM instructions for a right shift that inserts set bits rather than zeros.  The amount could be non-constant,
// so we need to be careful about how we do this.
ExpressionPtr
RiscOperators::emit_logical_right_shift_ones(std::ostream &o, const ExpressionPtr &value, const ExpressionPtr &amount)
{
    ExpressionPtr t1 = emit_logical_right_shift(o, value, amount);
    size_t width = std::max(value->nBits(), amount->nBits());
    ExpressionPtr ones = SymbolicExpr::makeAdd(SymbolicExpr::makeIntegerConstant(width, value->nBits()),
                                               SymbolicExpr::makeNegate(SymbolicExpr::makeExtend(SymbolicExpr::makeIntegerConstant(8, width),
                                                                                                 amount, solver()),
                                                                        solver()),
                                               solver());
    return emit_binary(o, "or", t1, ones);
}

// Emit an LLVM expression for arithmetic right shift.  If the shift amount is the constant zero then this is a no-op.  LLVM
// requires that the shift amount be the same width as the value being shifted.  The shift amount is interpretted as unsigned.
ExpressionPtr
RiscOperators::emit_arithmetic_right_shift(std::ostream &o, const ExpressionPtr &value, const ExpressionPtr &amount)
{
    if (LeafPtr amount_leaf = amount->isLeafNode()) {
        if (amount_leaf->isIntegerConstant()) {
            if (amount_leaf->toUnsigned().get() == 0)
                return value;
            if (amount_leaf->toUnsigned().get() >= value->nBits())
                return SymbolicExpr::makeIntegerConstant(value->nBits(), 0);
        }
    }
    return emit_binary(o, "ashr", value, emit_unsigned_resize(o, amount, value->nBits()));
}

// Emit an LLVM left-shift expression if necessary.  If the shift amount is the constant zero then this is a no-op. LLVM
// requires that the shift amount be the same width as the value being shifted.  The shift amount is interpretted as unsigned.
ExpressionPtr
RiscOperators::emit_left_shift(std::ostream &o, const ExpressionPtr &value, const ExpressionPtr &amount)
{
    if (LeafPtr amount_leaf = amount->isLeafNode()) {
        if (amount_leaf->isIntegerConstant()) {
            if (amount_leaf->toUnsigned().get() == 0)
                return value;
            if (amount_leaf->toUnsigned().get() >= value->nBits())
                return SymbolicExpr::makeIntegerConstant(value->nBits(), 0);
        }
    }
    return emit_binary(o, "shl", value, emit_unsigned_resize(o, amount, value->nBits()));
}

// Emits the LLVM equivalent of ROSE's OP_SHL1, which shifts 1 bits into the LSB side of the value. LLVM doesn't have an
// operator that shifts set bits into the left side of a value, so we use the zero-inserting left shift operator, the invert
// operator, and the bitwise OR operator instead.
ExpressionPtr
RiscOperators::emit_left_shift_ones(std::ostream &o, const ExpressionPtr &value, const ExpressionPtr &amount)
{
    size_t width = value->nBits();
    ExpressionPtr t1 = emit_left_shift(o, value, amount);
    ExpressionPtr ones = emit_invert(o, emit_left_shift(o, SymbolicExpr::makeIntegerConstant(width, -1), amount));
    return emit_binary(o, "or", t1, ones);
}

// Emits the LLVM equivalent of ROSE's OP_LSSB, which returns the zero-origin index of the least significant set bit. The return
// value is zero if the least significant bit is set or if no bits are set.  LLVM doesn't have an instruction like this, but it
// has a related instruction that counts the number of trailing zero bits (llvm.cttz.*).  We generate the following code for
// the call emit_lssb(o, i32 %1):
//    %2 = icmp eq i32 %1, 0                     ; is zero?
//    %3 = call i32 @llvm.cttz.i32(i32 %1)       ; number of trailing zeros (not used if %1 is zero)
//    %4 = select i1 %2, i32 0, i32 %3           ; result
ExpressionPtr
RiscOperators::emit_lssb(std::ostream &o, const ExpressionPtr &value)
{
    size_t width = value->nBits();
    LeafPtr zero = SymbolicExpr::makeIntegerConstant(width, 0);
    LeafPtr t1 = emit_expression(o, value);
    ExpressionPtr t2 = emit_compare(o, "icmp eq", t1, zero);
    LeafPtr t3 = next_temporary(width);
    o <<prefix() <<llvm_lvalue(t3) <<" = call " <<llvm_integer_type(width)
      <<" @llvm.cttz.i" <<StringUtility::numberToString(width) <<"(" <<llvm_integer_type(width) <<" " <<llvm_term(t1) <<")\n";
    ExpressionPtr t4 = emit_ite(o, t2, zero, t3);
    return t4;
}

// Emits the LLVM equivalent of ROSE's OP_MSSB, which returns the zero-origin index of the most significant set bit. The return
// value is zero if the least significant bit is set or if no bits are set.  LLVM doesn't have an instruction like this, but it
// has a related instruction that counts the number of leading zero bits (llvm.ctlz.*).  We generate the following code for
// the call emit_mssb(o, i32 %1):
//    %2 = icmp eq i32 %1, 0                     ; is zero?
//    %3 = call i32 @llvm.ctlz.i32(i32 %1, i1 0) ; number of leading zeros (not used if %1 is zero)
//    %4 = sub i32 31, %3                        ; result if value is non-zero
//    %5 = select i1 %2, i32 0, i32 %4           ; final result
ExpressionPtr
RiscOperators::emit_mssb(std::ostream &o, const ExpressionPtr &value)
{
    size_t width = value->nBits();
    LeafPtr zero = SymbolicExpr::makeIntegerConstant(width, 0);
    LeafPtr t1 = emit_expression(o, value);
    ExpressionPtr t2 = emit_compare(o, "icmp eq", t1, zero);
    LeafPtr t3 = next_temporary(width);
    o <<prefix() <<llvm_lvalue(t3) <<" = call " <<llvm_integer_type(width)
      <<" @llvm.ctlz.i" <<StringUtility::numberToString(width) <<"(" <<llvm_integer_type(width) <<" " <<llvm_term(t1) <<")\n";
    ExpressionPtr t4 = emit_binary(o, "sub", SymbolicExpr::makeIntegerConstant(width, width-1), t3);
    ExpressionPtr t5 = emit_ite(o, t2, zero, t4);
    return t5;
}

// Emit LLVM instructions for an extract operator.  LLVM doesn't have a dedicated extract instruction, so we right shift
// and truncate.
ExpressionPtr
RiscOperators::emit_extract(std::ostream &o, const ExpressionPtr &value, const ExpressionPtr &from, size_t result_nbits)
{
    return emit_truncate(o, emit_logical_right_shift(o, value, from), result_nbits);
}

// Emit LLVM to invert all bits of a value.  LLVM doesn't have a dedicated invert operator, so we use xor instead.
ExpressionPtr
RiscOperators::emit_invert(std::ostream &o, const ExpressionPtr &value)
{
    return emit_binary(o, "xor", value, SymbolicExpr::makeIntegerConstant(value->nBits(), -1));
}

// Emit LLVM instructions for a left-associative binary operator. If only one operand is given, then simply return that operand
// without doing anything.  When more than one operand is given they must all be the same width.
ExpressionPtr
RiscOperators::emit_left_associative(std::ostream &o, const std::string &llvm_operator, const TreeNodes &operands)
{
    ASSERT_require(!operands.empty());
    const size_t width = operands[0]->nBits();
    std::string type = llvm_integer_type(width);
    ExpressionPtr result = operands[0];

    for (size_t i=1; i<operands.size(); ++i) {
        ASSERT_require(operands[i]->nBits() == width);
        result = emit_binary(o, llvm_operator, result, operands[i]);
    }
    return result;
}

// Emits LLVM to concatenate operands. LLVM doesn't have a dedicated concatenation operator, so we must build the result with
// left shift and bit-wise OR operators.  Operands are given from most significant to least significant.
ExpressionPtr
RiscOperators::emit_concat(std::ostream &o, TreeNodes operands)
{
    ASSERT_require(!operands.empty());
    if (1==operands.size())
        return operands[0];

    size_t result_width = 0;
    for (size_t i=0; i<operands.size(); ++i)
        result_width += operands[i]->nBits();

    std::reverse(operands.begin(), operands.end());     // we want least-significant to most-significant
    ExpressionPtr result = operands[0];
    size_t shift = operands[0]->nBits();
    for (size_t i=1; i<operands.size(); ++i) {
        ExpressionPtr t1 = emit_zero_extend(o, result, result_width);
        ExpressionPtr t2 = emit_zero_extend(o, operands[i], result_width);
        ExpressionPtr t3 = emit_left_shift(o, t2, SymbolicExpr::makeIntegerConstant(result_width, shift));
        result = emit_binary(o, "or", t1, t3);
        shift += operands[i]->nBits();
    }
    return result;
}

// Emits LLVM to compute a ratio.  LLVM requires that the numerator and denominator have the same width; ROSE only stipulates
// that the return value has the same width as the numerator.
ExpressionPtr
RiscOperators::emit_signed_divide(std::ostream &o, const ExpressionPtr &numerator, const ExpressionPtr &denominator)
{
    size_t width = std::max(numerator->nBits(), denominator->nBits());
    ExpressionPtr t1 = emit_sign_extend(o, numerator, width);
    ExpressionPtr t2 = emit_sign_extend(o, denominator, width);
    ExpressionPtr t3 = emit_binary(o, "sdiv", t1, t2);
    return emit_truncate(o, t3, numerator->nBits());
}

// Emits LLVM to compute a ratio.  LLVM requires that the numerator and denominator have the same width; ROSE only stipulates
// that the return value has the same width as the numerator.
ExpressionPtr
RiscOperators::emit_unsigned_divide(std::ostream &o, const ExpressionPtr &numerator, const ExpressionPtr &denominator)
{
    size_t width = std::max(numerator->nBits(), denominator->nBits());
    ExpressionPtr t1 = emit_zero_extend(o, numerator, width);
    ExpressionPtr t2 = emit_zero_extend(o, denominator, width);
    ExpressionPtr t3 = emit_binary(o, "udiv", t1, t2);
    return emit_truncate(o, t3, numerator->nBits());
}

// Emits LLVM to compute a remainder.  In LLVM the width of the result is the same as the width of the numerator, but in ROSE
// the width of the result is the same as the width of the denominator.
ExpressionPtr
RiscOperators::emit_signed_modulo(std::ostream &o, const ExpressionPtr &numerator, const ExpressionPtr &denominator)
{
    size_t width = std::max(numerator->nBits(), denominator->nBits());
    ExpressionPtr t1 = emit_sign_extend(o, numerator, width);
    ExpressionPtr t2 = emit_sign_extend(o, denominator, width);
    ExpressionPtr t3 = emit_binary(o, "srem", t1, t2);
    return emit_truncate(o, t3, denominator->nBits());
}

// Emits LLVM to compute a remainder.  In LLVM the width of the result is the same as the width of the numerator, but in ROSE
// the width of the result is the same as the width of the denominator.
ExpressionPtr
RiscOperators::emit_unsigned_modulo(std::ostream &o, const ExpressionPtr &numerator, const ExpressionPtr &denominator)
{
    size_t width = std::max(numerator->nBits(), denominator->nBits());
    ExpressionPtr t1 = emit_zero_extend(o, numerator, width);
    ExpressionPtr t2 = emit_zero_extend(o, denominator, width);
    ExpressionPtr t3 = emit_binary(o, "urem", t1, t2);
    return emit_truncate(o, t3, denominator->nBits());
}

// Emits LLVM to compute the product of all the operands.  Multiply operations in LLVM are always sign-independent since the
// product has the same width as the operands (which must all be the same width).  In ROSE, multiply takes any number of
// operands and returns a product whose width is the sum of the operand widths, so we have to sign extend everything.
ExpressionPtr
RiscOperators::emit_signed_multiply(std::ostream &o, const TreeNodes &operands)
{
    ASSERT_require(!operands.empty());

    size_t result_width = 0;
    for (size_t i=0; i<operands.size(); ++i)
        result_width += operands[i]->nBits();

    ExpressionPtr result = operands[0];
    for (size_t i=1; i<operands.size(); ++i) {
        ExpressionPtr t1 = emit_sign_extend(o, result, result_width);
        ExpressionPtr t2 = emit_sign_extend(o, operands[i], result_width);
        result = emit_binary(o, "mul", t1, t2);
    }

    return result;
}

// Emits LLVM to compute the product of all the operands.  Multiply operations in LLVM are always sign-independent since the
// product has the same width as the operands (which must all be the same width).  In ROSE, multiply takes any number of
// operands and returns a product whose width is the sum of the operand widths, so we have to zero extend everything.
ExpressionPtr
RiscOperators::emit_unsigned_multiply(std::ostream &o, const TreeNodes &operands)
{
    ASSERT_require(!operands.empty());

    size_t result_width = 0;
    for (size_t i=0; i<operands.size(); ++i)
        result_width += operands[i]->nBits();

    ExpressionPtr result = operands[0];
    for (size_t i=1; i<operands.size(); ++i) {
        ExpressionPtr t1 = emit_zero_extend(o, result, result_width);
        ExpressionPtr t2 = emit_zero_extend(o, operands[i], result_width);
        result = emit_binary(o, "mul", t1, t2);
    }

    return result;
}

// Rotate the bits of "value" by "amount" bits.  LLVM doesn't have a rotate instruction so we use left and right shifting and
// bitwise OR.  If called like emit_rotate_left(o, i32 %1, i32 %2) the output will be:
//     %3 = shl i32 %1, %2      ; result high bits
//     %4 = sub i32 32, %2      ; right shift amount
//     %5 = lshr i32 %1, %4     ; result low bits
//     %6 = or i32 %3, %5       ; result
ExpressionPtr
RiscOperators::emit_rotate_left(std::ostream &o, const ExpressionPtr &value, const ExpressionPtr &amount)
{
    ExpressionPtr t3 = emit_left_shift(o, value, amount);
    ExpressionPtr t4 = emit_unsigned_binary(o, "sub",
                                            SymbolicExpr::makeIntegerConstant(amount->nBits(), 32),
                                            amount);
    ExpressionPtr t5 = emit_arithmetic_right_shift(o, value, t4);
    ExpressionPtr t6 = emit_unsigned_binary(o, "or", t3, t5);
    return t6;
}

// Rotate the bits of "value" by "amount" bits.  LLVM doesn't have a rotate instruction so we use left and right shifting and
// bitwise OR.  If called like emit_rotate_right(o, i32 %1, i32 %2) the output will be:
//     %3 = lshr i32 %1, %2     ; result low bits
//     %4 = sub i32 32, %2      ; left shift amount
//     %5 = shl i32 %1, %4      ; result high bits
//     %6 = or i32 %3, %5       ; result
ExpressionPtr
RiscOperators::emit_rotate_right(std::ostream &o, const ExpressionPtr &value, const ExpressionPtr &amount)
{
    ExpressionPtr t3 = emit_arithmetic_right_shift(o, value, amount);
    ExpressionPtr t4 = emit_unsigned_binary(o, "sub",
                                            SymbolicExpr::makeIntegerConstant(amount->nBits(), 32),
                                            amount);
    ExpressionPtr t5 = emit_left_shift(o, value, t4);
    ExpressionPtr t6 = emit_unsigned_binary(o, "or", t3, t5);
    return t6;
}

// Emits a comparison operation.  E.g., emit_compare(o, "icmp eq", %1, %2) will emit:
//    %3 = icmp eq i32 %1, i32 %1; typeof(%3) == i1
ExpressionPtr
RiscOperators::emit_compare(std::ostream &o, const std::string &llvm_op, const ExpressionPtr &a, const ExpressionPtr &b)
{
    LeafPtr t1 = emit_expression(o, a);
    LeafPtr t2 = emit_expression(o, b);
    LeafPtr t3 = next_temporary(1);
    o <<prefix() <<llvm_lvalue(t3) <<" = " <<llvm_op
      <<" " <<llvm_integer_type(t1->nBits()) <<" " <<llvm_term(t1) <<", " <<llvm_term(t2) <<"\n";
    return t3;
}

// Emits LLVM for an if-then-else construct.
ExpressionPtr
RiscOperators::emit_ite(std::ostream &o, const ExpressionPtr &cond, const ExpressionPtr &a, const ExpressionPtr &b)
{
    ASSERT_not_null(cond);
    ASSERT_not_null(a);
    ASSERT_not_null(b);
    ASSERT_require(cond->nBits()==1);
    ASSERT_require(a->nBits()==b->nBits());

    size_t width = a->nBits();
    LeafPtr t1 = emit_expression(o, cond);
    LeafPtr t2 = emit_expression(o, a);
    LeafPtr t3 = emit_expression(o, b);
    LeafPtr t4 = next_temporary(width);
    o <<prefix() <<llvm_lvalue(t4) <<" = select i1 " <<llvm_term(t1)
      <<", " <<llvm_integer_type(width) <<" " <<llvm_term(t2)
      <<", " <<llvm_integer_type(width) <<" " <<llvm_term(t3) <<"\n";
    return t4;
}

ExpressionPtr
RiscOperators::emit_memory_read(std::ostream &o, const ExpressionPtr &addr, size_t nbits)
{
    ASSERT_not_null(addr);

    // Convert ADDR to a pointer T2. The pointer type is "iNBITS*"
    LeafPtr t1 = emit_expression(o, addr);
    LeafPtr t2 = next_temporary(32);                // a 32-bit address
    o <<prefix() <<llvm_lvalue(t2) <<" = inttoptr " <<llvm_integer_type(t1->nBits()) <<" " <<llvm_term(t1)
      <<" to " <<llvm_integer_type(nbits) <<"*\n";

    // Dereference pointer T2 to get the return value.
    LeafPtr t3 = next_temporary(nbits);
    if (llvmVersion_ < 3007000) {                      // just a guess
        if (0 == llvmVersion_ && 0 == nVersionWarnings++)
            mlog[WARN] <<"LLVM version number is unknown; assuming 1-argument \"load\" instructions\n";
        o <<prefix() <<llvm_lvalue(t3) <<" = load " <<llvm_integer_type(nbits) <<"* " <<llvm_term(t2) <<"\n";
    } else {
        o <<prefix() <<llvm_lvalue(t3) <<" = load " <<llvm_integer_type(nbits) <<", "
          <<llvm_integer_type(nbits) <<"* " <<llvm_term(t2) <<"\n";
    }
    return t3;
}

// Reads a global variable. For instance,
//     emit_global_read(o, "@ebp", 32)
//         %1 = load i32* @ebp ; return value
ExpressionPtr
RiscOperators::emit_global_read(std::ostream &o, const std::string &varname, size_t nbits)
{
    ASSERT_require(!varname.empty() && varname[0]=='@');
    LeafPtr t1 = next_temporary(nbits);
    if (llvmVersion_ < 3007000) {
        if (0 == llvmVersion_ && 0 == nVersionWarnings++)
            mlog[WARN] <<"LLVM version number is unknown; assuming 1-argument \"load\" instructions\n";
        o <<prefix() <<llvm_lvalue(t1) <<" = load " <<llvm_integer_type(nbits) <<"* " <<varname <<"\n";
    } else {
        o <<prefix() <<llvm_lvalue(t1) <<" = load " <<llvm_integer_type(nbits) <<", "
          <<llvm_integer_type(nbits) <<"* " <<varname <<"\n";
    }
    return t1;
}

// Write a value to an address. The address is an integer expression.  For instance,
//     emit_memory_write(o, i32 %1, i16 %2)
// emits
//     %3 = inttoptr i32 %1 to i16*
//     store i16 %2, i16* %3
void
RiscOperators::emit_memory_write(std::ostream &o, const ExpressionPtr &addr, const ExpressionPtr &value)
{
    LeafPtr t1 = emit_expression(o, value);
    LeafPtr t2 = emit_expression(o, addr);

    LeafPtr t3 = next_temporary(value->nBits());
    o <<prefix() <<llvm_lvalue(t3) <<" = inttoptr " <<llvm_integer_type(addr->nBits()) <<" " <<llvm_term(t2)
      <<" to " <<llvm_integer_type(value->nBits()) <<"*\n";
    o <<prefix() <<"store " <<llvm_integer_type(value->nBits()) <<" " <<llvm_term(t1)
      <<", " <<llvm_integer_type(value->nBits()) <<"* " <<llvm_term(t3) <<"\n";
}

LeafPtr
RiscOperators::emit_expression(std::ostream &o, const LeafPtr &leaf)
{
    ExpressionPtr x = leaf;
    return emit_expression(o, x);
}

// Emits LLVM for an expression and returns a terminal (variable or constant).  If the expression is already a terminal then
// this method is a no-op.
LeafPtr
RiscOperators::emit_expression(std::ostream &o, const ExpressionPtr &orig_expr)
{
    ASSERT_not_null(orig_expr);
    ExpressionPtr cur_expr = orig_expr;

    // If we've seen this expression already, then use the replacement value (an LLVM variable).
    if (rewrites.exists(cur_expr->hash()))
        cur_expr = rewrites.get_one(cur_expr->hash());

    // Handle leaf nodes
    if (LeafPtr leaf = cur_expr->isLeafNode()) {
        if (leaf->isVariable2()) {
            std::string varname = get_variable(leaf);
            if (varname.empty()) {
                // This is a reference to a ROSE variable that has no corresponding LLVM variable.  This can happen for things
                // like the x86 TEST instruction, which leaves the AF register in an undefined state--if we then try to print
                // the definition of AF we will encounter a ROSE variable with no LLVM variable.
                LeafPtr t1 = next_temporary(leaf->nBits());
                o <<prefix() <<llvm_lvalue(t1) <<" = add " <<llvm_integer_type(leaf->nBits()) <<" undef, undef\n";
                cur_expr = t1;
            } else if ('@'==varname[0]) {
                cur_expr = emit_global_read(o, varname, leaf->nBits());
            }
        }
    }
    
    // Emit LLVM for symbolic operators until the result is a leaf node (LLVM variable or constant). This causes recursive
    // calls to emit_expression().
    while (InteriorPtr inode = cur_expr->isInteriorNode()) {
        ExpressionPtr operator_result;
        TreeNodes operands = inode->children();
        switch (inode->getOperator()) {
            case SymbolicExpr::OP_NONE:
                ASSERT_not_reachable("cannot happen for an interior node");
            case SymbolicExpr::OP_ADD:
                operator_result = emit_left_associative(o, "add", operands);
                break;
            case SymbolicExpr::OP_AND:
                operator_result = emit_left_associative(o, "and", operands);
                break;
            case SymbolicExpr::OP_ASR:
                ASSERT_require(2==operands.size());
                operator_result = emit_arithmetic_right_shift(o, operands[1], operands[0]);
                break;
            case SymbolicExpr::OP_XOR:
                operator_result = emit_left_associative(o, "xor", operands);
                break;
            case SymbolicExpr::OP_CONCAT:
                operator_result = emit_concat(o, operands);
                break;
            case SymbolicExpr::OP_EQ:
                ASSERT_require(2==operands.size());
                operator_result = emit_compare(o, "icmp eq", operands[0], operands[1]);
                break;
            case SymbolicExpr::OP_EXTRACT:
                ASSERT_require(3==operands.size());
                operator_result = emit_extract(o, operands[2], operands[0], inode->nBits());
                break;
            case SymbolicExpr::OP_INVERT:
                ASSERT_require(1==operands.size());
                operator_result = emit_invert(o, operands[0]);
                break;
            case SymbolicExpr::OP_ITE:
                ASSERT_require(3==operands.size());
                operator_result = emit_ite(o, operands[0], operands[1], operands[2]);
                break;
            case SymbolicExpr::OP_LSSB:
                ASSERT_require(1==operands.size());
                operator_result = emit_lssb(o, operands[0]);
                break;
            case SymbolicExpr::OP_MSSB:
                ASSERT_require(1==operands.size());
                operator_result = emit_mssb(o, operands[0]);
                break;
            case SymbolicExpr::OP_NE:
                ASSERT_require(2==operands.size());
                operator_result = emit_compare(o, "icmp ne", operands[0], operands[1]);
                break;
            case SymbolicExpr::OP_NEGATE:
                ASSERT_require(1==operands.size());
                operator_result = emit_binary(o, "sub", SymbolicExpr::makeIntegerConstant(operands[0]->nBits(), 0), operands[0]);
                break;
            case SymbolicExpr::OP_OR:
                operator_result = emit_left_associative(o, "or", operands);
                break;
            case SymbolicExpr::OP_READ:
                ASSERT_require(2==operands.size());
                operator_result = emit_memory_read(o, operands[1], inode->nBits());
                break;
            case SymbolicExpr::OP_ROL:
                ASSERT_require(2==operands.size());
                operator_result = emit_rotate_left(o, operands[1], operands[0]);
                break;
            case SymbolicExpr::OP_ROR:
                ASSERT_require(2==operands.size());
                operator_result = emit_rotate_right(o, operands[1], operands[0]);
                break;
            case SymbolicExpr::OP_SDIV:
                ASSERT_require(2==operands.size());
                operator_result = emit_signed_divide(o, operands[0], operands[1]);
                break;
            case SymbolicExpr::OP_SEXTEND:
                ASSERT_require(2==operands.size());
                operator_result = emit_sign_extend(o, operands[1], inode->nBits());
                break;
            case SymbolicExpr::OP_SGE:
                ASSERT_require(2==operands.size());
                operator_result = emit_compare(o, "icmp sge", operands[0], operands[1]);
                break;
            case SymbolicExpr::OP_SGT:
                ASSERT_require(2==operands.size());
                operator_result = emit_compare(o, "icmp sgt", operands[0], operands[1]);
                break;
            case SymbolicExpr::OP_SHL0:
                ASSERT_require(2==operands.size());
                operator_result = emit_left_shift(o, operands[1], operands[0]);
                break;
            case SymbolicExpr::OP_SHL1:
                ASSERT_require(2==operands.size());
                operator_result = emit_left_shift_ones(o, operands[1], operands[0]);
                break;
            case SymbolicExpr::OP_SHR0:
                ASSERT_require(2==operands.size());
                operator_result = emit_logical_right_shift(o, operands[1], operands[0]);
                break;
            case SymbolicExpr::OP_SHR1:
                ASSERT_require(2==operands.size());
                operator_result = emit_logical_right_shift_ones(o, operands[1], operands[0]);
                break;
            case SymbolicExpr::OP_SLE:
                ASSERT_require(2==operands.size());
                operator_result = emit_compare(o, "icmp sle", operands[0], operands[1]);
                break;
            case SymbolicExpr::OP_SLT:
                ASSERT_require(2==operands.size());
                operator_result = emit_compare(o, "icmp slt", operands[0], operands[1]);
                break;
            case SymbolicExpr::OP_SMOD:
                ASSERT_require(2==operands.size());
                operator_result = emit_signed_modulo(o, operands[0], operands[1]);
                break;
            case SymbolicExpr::OP_SMUL:
                operator_result = emit_signed_multiply(o, operands);
                break;
            case SymbolicExpr::OP_UDIV:
                ASSERT_require(2==operands.size());
                operator_result = emit_unsigned_divide(o, operands[0], operands[1]);
                break;
            case SymbolicExpr::OP_UEXTEND:
                ASSERT_require(2==operands.size());
                operator_result = emit_zero_extend(o, operands[1], inode->nBits());
                break;
            case SymbolicExpr::OP_UGE:
                ASSERT_require(2==operands.size());
                operator_result = emit_compare(o, "icmp uge", operands[0], operands[1]);
                break;
            case SymbolicExpr::OP_UGT:
                ASSERT_require(2==operands.size());
                operator_result = emit_compare(o, "icmp ugt", operands[0], operands[1]);
                break;
            case SymbolicExpr::OP_ULE:
                ASSERT_require(2==operands.size());
                operator_result = emit_compare(o, "icmp ule", operands[0], operands[1]);
                break;
            case SymbolicExpr::OP_ULT:
                ASSERT_require(2==operands.size());
                operator_result = emit_compare(o, "icmp ult", operands[0], operands[1]);
                break;
            case SymbolicExpr::OP_UMOD:
                ASSERT_require(2==operands.size());
                operator_result = emit_unsigned_modulo(o, operands[0], operands[1]);
                break;
            case SymbolicExpr::OP_UMUL:
                operator_result = emit_unsigned_multiply(o, operands);
                break;
            case SymbolicExpr::OP_ZEROP:
                ASSERT_require(1==operands.size());
                operator_result = emit_compare(o, "icmp eq", operands[0],
                                               SymbolicExpr::makeIntegerConstant(operands[0]->nBits(), 0));
                break;

            case SymbolicExpr::OP_LET:
            case SymbolicExpr::OP_NOOP:
            case SymbolicExpr::OP_WRITE:
            case SymbolicExpr::OP_SET:
            case SymbolicExpr::OP_FP_ABS:
            case SymbolicExpr::OP_FP_NEGATE:
            case SymbolicExpr::OP_FP_ADD:
            case SymbolicExpr::OP_FP_MUL:
            case SymbolicExpr::OP_FP_DIV:
            case SymbolicExpr::OP_FP_MULADD:
            case SymbolicExpr::OP_FP_SQRT:
            case SymbolicExpr::OP_FP_MOD:
            case SymbolicExpr::OP_FP_ROUND:
            case SymbolicExpr::OP_FP_MIN:
            case SymbolicExpr::OP_FP_MAX:
            case SymbolicExpr::OP_FP_LE:
            case SymbolicExpr::OP_FP_LT:
            case SymbolicExpr::OP_FP_GE:
            case SymbolicExpr::OP_FP_GT:
            case SymbolicExpr::OP_FP_EQ:
            case SymbolicExpr::OP_FP_ISNORM:
            case SymbolicExpr::OP_FP_ISSUBNORM:
            case SymbolicExpr::OP_FP_ISZERO:
            case SymbolicExpr::OP_FP_ISINFINITE:
            case SymbolicExpr::OP_FP_ISNAN:
            case SymbolicExpr::OP_FP_ISNEG:
            case SymbolicExpr::OP_FP_ISPOS:
            case SymbolicExpr::OP_CONVERT:
            case SymbolicExpr::OP_REINTERPRET:
                throw BaseSemantics::Exception("LLVM translation for " +
                                               stringifyBinaryAnalysisSymbolicExprOperator(inode->getOperator()) +
                                               " is not implemented yet", NULL);

            // no default because we want warnings when a new operator is added
        }
        ASSERT_not_null(operator_result);               // no case was executed
        cur_expr = operator_result;
    }

    // The return value must be a constant or variable
    LeafPtr retval = cur_expr->isLeafNode();
    ASSERT_require(retval!=NULL && (retval->isIntegerConstant() || retval->isIntegerVariable()));

    // Add a rewrite rule so that next time we're asked to emit the same expression we can just emit the result without going
    // through all this work again.
    add_rewrite(orig_expr, retval);
    return retval;
}

void
RiscOperators::add_rewrite(const ExpressionPtr &from, const LeafPtr &to)
{
    ASSERT_not_null(from);
    ASSERT_not_null(to);
    if (from==to) {
        rewrites.erase(from->hash());
    } else {
        rewrites.insert(std::make_pair(from->hash(), to));
        if (to->isIntegerVariable())
            add_variable(to);
    }
}

std::string
RiscOperators::add_variable(const LeafPtr &var)
{
    ASSERT_require(var!=NULL && var->isIntegerVariable());
    std::string name = get_variable(var);
    if (name.empty()) {
        name = var->comment();
        if (name.empty()) {
            name = "%v" + StringUtility::numberToString(var->nameId());
        } else if (name.size()>2 && 0==name.substr(name.size()-2).compare("_0")) {
            name = "@" + name.substr(0, name.size()-2);
        } else {
            name = "@" + name;
        }
        variables.insert(std::make_pair(var->nameId(), name));
    }
    return name;
}

std::string
RiscOperators::get_variable(const LeafPtr &var)
{
    ASSERT_require(var!=NULL && var->isIntegerVariable());
    return variables.get_value_or(var->nameId(), "");
}

LeafPtr
RiscOperators::emit_assignment(std::ostream &o, const ExpressionPtr &rhs)
{
    ASSERT_not_null(rhs);
    LeafPtr t1 = emit_expression(o, rhs);

    if (t1->isIntegerVariable() && t1->comment().empty())
        return t1;

    LeafPtr lhs = next_temporary(rhs->nBits());
    o <<prefix() <<llvm_lvalue(lhs) <<" = " <<llvm_integer_type(rhs->nBits()) <<" " <<llvm_term(t1) <<"\n";
    add_rewrite(rhs, lhs);
    return lhs;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Transcoder
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int
Transcoder::llvmVersion() const {
    return operators->llvmVersion();
}

void
Transcoder::llvmVersion(int v) {
    operators->llvmVersion(v);
}

void
Transcoder::emitFilePrologue(std::ostream &o)
{
    operators->emit_register_declarations(o, operators->get_important_registers());

    // This function is apparently not declared like it should be.  Hopefully we only need these versions.
    o <<"\n"
      <<operators->prefix() <<"; These LLVM functions don't seem to be defined as advertised in documentation.\n"
      <<operators->prefix() <<"declare i8  @llvm.ctlz.i8 (i8)\n"
      <<operators->prefix() <<"declare i16 @llvm.ctlz.i16(i16)\n"
      <<operators->prefix() <<"declare i32 @llvm.ctlz.i32(i32)\n"
      <<operators->prefix() <<"declare i8  @llvm.cttz.i8 (i8)\n"
      <<operators->prefix() <<"declare i16 @llvm.cttz.i16(i16)\n"
      <<operators->prefix() <<"declare i32 @llvm.cttz.i32(i32)\n";
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
    ASSERT_this();
    if (!bb)
        return 0;
    std::vector<SgAsmInstruction*> insns = SageInterface::querySubTree<SgAsmInstruction>(bb);
#if 0 // [Robb Matzke 2015-12-23]: doesn't handle HLT, INT, INT3, REP, etc.
    operators->reset();
    for (size_t i=0; i<insns.size(); ++i) {
        SgAsmInstruction *insn = insns[i];
        if (0==i)
            o <<"\n" <<operators->prefix() <<operators->addr_label(bb->get_address()) <<":\n";
        o <<operators->prefix() <<"; " <<StringUtility::addrToString(insn->get_address())
          <<": " <<unparseInstruction(insn) <<"\n";
        try {
            dispatcher->processInstruction(insn);
        } catch (const BaseSemantics::Exception &e) {
            if (quiet_errors) {
                // Try to make sure that the LLVM is still valid.  That means we need to make sure the instruction pointer is a
                // reasonable value otherwise we might try to branch to the failed instruction, which could be in the middle of
                // a basic block.  It's more likely that the fall-through address will be a valid basic block, although not
                // guaranteed.
                o <<operators->prefix() <<";;ERROR: " <<e <<"\n";
                RegisterDescriptor IP_REG = operators->get_insn_pointer_register();
                BaseSemantics::SValuePtr fallthrough_va = operators->number_(IP_REG.nBits(),
                                                                             insn->get_address() + insn->get_size());
                operators->currentState()->registerState()->writeRegister(IP_REG, fallthrough_va, operators.get());
            } else {
                throw;
            }
        }
            
        // In order to avoid issues of memory aliasing, whenever a write occurs we dump the current machine state to
        // LLVM. We're assuming that a machine instruction performs at most one write and if it performs a memory write then it
        // doesn't also perform a memory read (or it performs the memory read first, as in test-and-set).
        if (!operators->get_memory_writes().empty()) {
            RiscOperators::Indent indent2(operators);
            operators->emit_changed_state(o);
        }

        // x86 HLT and INT need special handling
        if (isSgAsmX86Instruction(insn) &&
            (isSgAsmX86Instruction(insn)->get_kind() == x86_hlt ||
             isSgAsmX86Instruction(insn)->get_kind() == x86_int ||
             isSgAsmX86Instruction(insn)->get_kind() == x86_int3)) {
            {
                RiscOperators::Indent indent2(operators);
                o <<operators->prefix() <<"br label %L_" <<StringUtility::addrToString(insn->get_address()) <<"_insn\n";
            }
            o <<operators->prefix() <<"L_" <<StringUtility::addrToString(insn->get_address()) <<"_insn:\n";
            {
                RiscOperators::Indent indent2(operators);
                o <<operators->prefix() <<"br label %L_" <<StringUtility::addrToString(insn->get_address()) <<"_insn\n";
            }
            return insns.size();
        }
    }

    if (!insns.empty()) {
        RiscOperators::Indent indent2(operators);
        operators->emit_changed_state(o);
        operators->emit_next_eip(o, insns.back());
    }
#else
    o <<"\n" <<operators->prefix() <<"; Basic block " <<StringUtility::addrToString(bb->get_address()) <<"\n";
    BOOST_FOREACH (SgAsmInstruction *insn, insns) {
        o <<operators->prefix() <<operators->addr_label(insn->get_address()) <<":    ; " <<unparseInstruction(insn) <<"\n";
        try {
            operators->reset();
            dispatcher->processInstruction(insn);
        } catch (const BaseSemantics::Exception &e) {
            if (quiet_errors) {
                o <<operators->prefix() <<";;ERROR: " <<e <<"\n";
            } else {
                throw;
            }
        }
        {
            RiscOperators::Indent indent2(operators);
            ExpressionPtr t1 = SymbolicExpr::makeIntegerConstant(32, insn->get_address());
            o <<operators->prefix() <<"store " <<operators->llvm_integer_type(32) <<" " <<operators->llvm_term(t1)
              <<", " <<operators->llvm_integer_type(32) <<"* @eip\n";
            operators->emit_changed_state(o);
            operators->emit_next_eip(o, insn);
        }
    }
#endif
    return insns.size();
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
    ASSERT_this();
    if (!func)
        return 0;
    size_t nbbs = 0;                                    // number of basic blocks emitted

    o <<operators->prefix() <<"define void " <<operators->function_label(func) <<"() {\n";
    RiscOperators::Indent func_body_indentation(operators);

    // Function prologue. Note that in LLVM the first basic block of a function cannot be the target of a branch instruction.
    // Therefore, we emit a no-op as the first basic block so that ROSE's first basic block becomes the second basic block for
    // LLVM.
    {
        RiscOperators::Indent insn_indentation(operators);
        std::string label = operators->addr_label(func->get_entry_va());
        o <<operators->prefix() <<"br label %" <<label <<"\n";
    }

    // Function body.
    const SgAsmStatementPtrList &bbs = func->get_statementList();
    for (SgAsmStatementPtrList::const_iterator bbi=bbs.begin(); bbi!=bbs.end(); ++bbi) {
        SgAsmBlock *bb = isSgAsmBlock(*bbi);
        ASSERT_not_null(bb);
        if (emit_funcfrags || 0==(bb->get_reason() & SgAsmBlock::BLK_FRAGMENT)) {
            int ninsns = transcodeBasicBlock(bb, o);
            if (ninsns>0)
                ++nbbs;
        } else {
            // We still must emit the basic block because we might need the address.  The RiscOperators don't have any
            // mechanism for filtering out basic blocks and so might produce a reference to such a block.
            o <<operators->prefix() <<operators->addr_label(bb->get_address()) <<":\n";
            RiscOperators::Indent insn_indentation(operators);
            o <<operators->prefix() <<"unreachable\n";
            ++nbbs;
        }
    }

    // Function epilogue.  This is to handle ROSE functions that have no basic blocks--they still need to be valid functions in
    // LLVM.
    if (0==nbbs) {
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

#if 0
    // [Robb Matzke 2015-12-22]: Doesn't seem to be needed for LLVM 3.5.0, and in fact generates errors like:
    //   invalid redefinition of function 'L_0x08048278__init'
    // for each line emitted here.
    o <<"\n; Function declarations\n";
    emitFunctionDeclarations(interp, o);
#endif

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
    transcodeInterpretation(interp, ss);
    return ss.str();
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
