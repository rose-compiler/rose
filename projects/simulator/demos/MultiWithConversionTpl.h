// See MultiWithConversion.h for documentation                                          __THIS_HEADER_IS_USED__
// This header is included at the end of RSIM_Templates.h

#include "YicesSolver.h"
#include "RobinTpl.h"

namespace MultiDomainDemo {

MULTI_DOMAIN_TEMPLATE
void
Policy<State, ValueType>::init()
{
    // We can't call anything here that uses the policy's RSIM_Thread because the thread is not fully initialized yet.
    name = "MultiDomainDemo::Policy";
    std::cerr <<"RSIM is using MultiDomainDemo::Policy (see demos/MultiWithConversion.h)\n";

    // By default, disable all our sub-domains.  Only allow the simulator's domain to be active.
    this->set_active_policies(CONCRETE.mask);

#if 1
    // Do we want to use an SMT solver for the symbolic semantics policy?  Doing so will make it much slower, but will also
    // allow us to answer more complex questions.  Regardless of whether ROSE's SymbolicSemantics uses an SMT solver, you will
    // always be able to use the SMTSolver class (or YicesSolver subclass) to build your own SMT queries. See ROSE doxygen
    // documentation for the SMTSolver class and example code in SymbolicSemantics.h.
    YicesSolver *yices = new YicesSolver;
    //yices->set_linkage(YicesSolver::LM_LIBRARY); // much faster, but has fewer debugging capabilities
    //yices->set_debug(stderr); // will show you solver input and output for LM_EXECUTABLE linkage
    this->get_policy(SYMBOLIC).set_solver(yices);
#endif
}

MULTI_DOMAIN_TEMPLATE
RTS_Message *
Policy<State, ValueType>::trace()
{
    return this->get_policy(CONCRETE).thread->tracing(TRACE_MISC);
}

MULTI_DOMAIN_TEMPLATE
void
Policy<State, ValueType>::trigger(rose_addr_t target_va)
{
    trace()->mesg("%s: triggered; enabling all sub-domains; branching to 0x%"PRIx64, name, target_va);
    this->set_active_policies(CONCRETE.mask | INTERVAL.mask | SYMBOLIC.mask);
    triggered = true;
    this->writeRegister("eip", RSIM_SEMANTICS_VTYPE<32>(target_va));
}

MULTI_DOMAIN_TEMPLATE
void
Policy<State, ValueType>::startInstruction(SgAsmInstruction *insn_)
{
    if (triggered) {
        SgAsmx86Instruction *insn = isSgAsmx86Instruction(insn_);
        assert(insn!=NULL);
        trace()->mesg("%s\n", std::string(80, '-').c_str());
        trace()->mesg("%s: executing: %s", name, unparseInstruction(insn).c_str());

        // We should be executing in at least the simulator's domain (concrete) right now since we haven't started the
        // instruction.
        assert(this->is_active(CONCRETE));

        // Make sure EIP is updated with the instruction's address (most policies assert this).
        this->set_active_policies(0x07);
        this->writeRegister("eip", RSIM_SEMANTICS_VTYPE<32>(insn->get_address()));

        // Activate domains based on instruction.
        unsigned activated = Robin::domains_for_instruction(this, insn);
        assert(activated!=0); // perhaps this should be how we finish an analysis
        this->set_active_policies(activated);
        std::string domains;
        if (0 != (activated & CONCRETE.mask))
            domains += " concrete";
        if (0 != (activated & INTERVAL.mask))
            domains += " interval";
        if (0 != (activated & SYMBOLIC.mask))
            domains += " symbolic";
        trace()->mesg("%s: executing in:%s\n", name, domains.c_str());

        // Warn about no concrete state
        if (!this->is_active(CONCRETE)) {
            static bool warned = false;
            if (!warned) {
                std::cerr <<name <<": turning off the CONCRETE state can have some unintended consequences. For\n"
                          <<"    instance, the concrete EIP value is used in most tracing messages.\n";
                warned = true;
            }
        }
    }
    Super::startInstruction(insn_);
}

MULTI_DOMAIN_TEMPLATE
void
Policy<State, ValueType>::finishInstruction(SgAsmInstruction *insn)
{
    if (triggered) {
        // The CONCRETE domain is driving the fetch-execute loop, so we probably need to make sure that the EIP's concrete value
        // points to the next instruction.  If we evaluated insn without the concrete domain, then EIP would never have been
        // updated.
        CONCRETE_VALUE<32> eip = convert_to_concrete(this->template readRegister<32>("eip"));
        this->get_policy(CONCRETE).writeRegister("eip", eip);

        Robin::after_instruction(this, isSgAsmx86Instruction(insn));
        this->set_active(CONCRETE);
    }

    Super::finishInstruction(insn);
}

MULTI_DOMAIN_TEMPLATE
template<size_t nBits>
ValueType<nBits>
Policy<State, ValueType>::xor_(const ValueType<nBits> &a, const ValueType<nBits> &b)
{
    ValueType<nBits> retval = Super::xor_(a, b);
    if (triggered) {
        // A common optimization is to zero a register by xoring it with itself. However, the IntervalSemantics' xor_() method
        // can't detect this case because all it sees is two operands which, although their ranges are identical, could have
        // different values.  So we use the symbolic semantics: if the symbolic semantics says the answer is zero then we set
        // the interval semantics' value to the singleton set {0}.  Simply looking at the nBit-symbolic answer won't easily
        // tell us if it must be zero (it could be a complex expression), but by calling equalToZero() we cause the SMT solver
        // to be invoked.
        if (this->is_active(SYMBOLIC) && retval.is_valid(SYMBOLIC)) {
            SYMBOLIC_VALUE<1> symbolic_bool = this->get_policy(SYMBOLIC).equalToZero(retval.get_subvalue(SYMBOLIC));
            if (symbolic_bool.is_known() && symbolic_bool.known_value()!=0) {
                trace()->mesg("%s: xor_ optimization triggered for interval domain", name);
                trace()->mesg("%s: instruction: %s", name, unparseInstruction(this->get_policy(CONCRETE).get_insn()).c_str());
#if 0 // FIXME: throws an exception, segfaults, ? [possibly fixed by commit 5cb0fa28, but not verified yet]
                std::ostringstream ss; ss <<this->get_policy(SYMBOLIC);
                trace()->mesg("%s: symbolic state:\n%s", name, StringUtility::prefixLines(ss.str(), "    ").c_str());
#endif
                retval.set_subvalue(INTERVAL, INTERVAL_VALUE<nBits>(0));
            }
        }
    }
    return retval;
}

MULTI_DOMAIN_TEMPLATE
size_t
Policy<State, ValueType>::symbolic_state_complexity()
{
    const RegisterDictionary::Entries &regs = this->get_policy(SYMBOLIC).get_register_dictionary()->get_registers();
    ExprNodeCounter visitor;
    for (RegisterDictionary::Entries::const_iterator ri=regs.begin(); ri!=regs.end(); ++ri) {
        try {
            const RegisterDescriptor &reg = ri->second;
            switch (reg.get_nbits()) { // arghh!  side effect of using templates :-/
                case 1:  symbolic_expr_complexity(this->template readRegister<1>(reg).get_subvalue(SYMBOLIC),  &visitor); break;
                case 8:  symbolic_expr_complexity(this->template readRegister<8>(reg).get_subvalue(SYMBOLIC),  &visitor); break;
                case 16: symbolic_expr_complexity(this->template readRegister<16>(reg).get_subvalue(SYMBOLIC), &visitor); break;
                case 32: symbolic_expr_complexity(this->template readRegister<32>(reg).get_subvalue(SYMBOLIC), &visitor); break;
                default: break; // skip these registers
            }
        } catch (SymbolicPolicy::Exception &e) {
            // register is probably not implemented in the state object, so skip it
        }
    }
    return visitor.nnodes;
}

MULTI_DOMAIN_TEMPLATE
template<size_t nBits>
ValueType<nBits>
Policy<State, ValueType>::readRegister(const char *regname)
{
    const RegisterDescriptor &reg = this->findRegister(regname, nBits);
    return this->template readRegister<nBits>(reg);
}

MULTI_DOMAIN_TEMPLATE
template<size_t nBits>
ValueType<nBits>
Policy<State, ValueType>::readRegister(const RegisterDescriptor &reg)
{
    if (!triggered)
        return Super::template readRegister<nBits>(reg);

    unsigned active_policies = this->get_active_policies();
    this->set_active_policies(0x07); // make them all active so our policy operations work properly in this function.
    ValueType<nBits> retval;
    try {
        switch (nBits) {
            case 1:
                // Only FLAGS/EFLAGS bits have a size of one.  Other registers cannot be accessed at this granularity.
                if (reg.get_major()!=x86_regclass_flags)
                    throw Exception("bit access only valid for FLAGS/EFLAGS register");
                if (reg.get_minor()!=0 || reg.get_offset()>=state.registers.n_flags)
                    throw Exception("register not implemented in semantic policy");
                if (reg.get_nbits()!=1)
                    throw Exception("semantic policy supports only single-bit flags");
                retval = this->template unsignedExtend<1, nBits>(state.registers.flag[reg.get_offset()]);
                break;

            case 8:
                // Only general-purpose registers can be accessed at a byte granularity, and we can access only the low-order
                // byte or the next higher byte.  For instance, "al" and "ah" registers.
                if (reg.get_major()!=x86_regclass_gpr)
                    throw Exception("byte access only valid for general purpose registers");
                if (reg.get_minor()>=state.registers.n_gprs)
                    throw Exception("register not implemented in semantic policy");
                assert(reg.get_nbits()==8); // we had better be asking for a one-byte register (e.g., "ah", not "ax")
                switch (reg.get_offset()) {
                    case 0:
                        retval = this->template extract<0, nBits>(state.registers.gpr[reg.get_minor()]);
                        break;
                    case 8:
                        retval = this->template extract<8, 8+nBits>(state.registers.gpr[reg.get_minor()]);
                        break;
                    default:
                        throw Exception("invalid one-byte access offset");
                }
                break;

            case 16:
                if (reg.get_nbits()!=16)
                    throw Exception("invalid 2-byte register");
                if (reg.get_offset()!=0)
                    throw Exception("policy does not support non-zero offsets for word granularity register access");
                switch (reg.get_major()) {
                    case x86_regclass_segment:
                        if (reg.get_minor()>=state.registers.n_segregs)
                            throw Exception("register not implemented in semantic policy");
                        retval = this->template unsignedExtend<16, nBits>(state.registers.segreg[reg.get_minor()]);
                        break;
                    case x86_regclass_gpr:
                        if (reg.get_minor()>=state.registers.n_gprs)
                            throw Exception("register not implemented in semantic policy");
                        retval = this->template extract<0, nBits>(state.registers.gpr[reg.get_minor()]);
                        break;
                    case x86_regclass_flags:
                        if (reg.get_minor()!=0 || state.registers.n_flags<16)
                            throw Exception("register not implemented in semantic policy");
                        retval = this->template unsignedExtend<16, nBits>(concat(state.registers.flag[0],
                                                                          concat(state.registers.flag[1],
                                                                          concat(state.registers.flag[2],
                                                                          concat(state.registers.flag[3],
                                                                          concat(state.registers.flag[4],
                                                                          concat(state.registers.flag[5],
                                                                          concat(state.registers.flag[6],
                                                                          concat(state.registers.flag[7],
                                                                          concat(state.registers.flag[8],
                                                                          concat(state.registers.flag[9],
                                                                          concat(state.registers.flag[10],
                                                                          concat(state.registers.flag[11],
                                                                          concat(state.registers.flag[12],
                                                                          concat(state.registers.flag[13],
                                                                          concat(state.registers.flag[14],
                                                                                 state.registers.flag[15]))))))))))))))));
                        break;
                    default:
                        throw Exception("word access not valid for this register type");
                }
                break;

            case 32:
                if (reg.get_offset()!=0)
                    throw Exception("policy does not support non-zero offsets for double word granularity register access");
                switch (reg.get_major()) {
                    case x86_regclass_gpr:
                        if (reg.get_minor()>=state.registers.n_gprs)
                            throw Exception("register not implemented in semantic policy");
                        retval = this->template unsignedExtend<32, nBits>(state.registers.gpr[reg.get_minor()]);
                        break;
                    case x86_regclass_ip:
                        if (reg.get_minor()!=0)
                            throw Exception("register not implemented in semantic policy");
                        retval = this->template unsignedExtend<32, nBits>(state.registers.ip);
                        break;
                    case x86_regclass_segment:
                        if (reg.get_minor()>=state.registers.n_segregs || reg.get_nbits()!=16)
                            throw Exception("register not implemented in semantic policy");
                        retval = this->template unsignedExtend<16, nBits>(state.registers.segreg[reg.get_minor()]);
                        break;
                    case x86_regclass_flags: {
                        if (reg.get_minor()!=0 || state.registers.n_flags<32)
                            throw Exception("register not implemented in semantic policy");
                        if (reg.get_nbits()!=32)
                            throw Exception("register is not 32 bits");
                        retval = this->template unsignedExtend<32, nBits>(concat(readRegister<16>("flags"), // no-op sign extension
                                                                          concat(state.registers.flag[16],
                                                                          concat(state.registers.flag[17],
                                                                          concat(state.registers.flag[18],
                                                                          concat(state.registers.flag[19],
                                                                          concat(state.registers.flag[20],
                                                                          concat(state.registers.flag[21],
                                                                          concat(state.registers.flag[22],
                                                                          concat(state.registers.flag[23],
                                                                          concat(state.registers.flag[24],
                                                                          concat(state.registers.flag[25],
                                                                          concat(state.registers.flag[26],
                                                                          concat(state.registers.flag[27],
                                                                          concat(state.registers.flag[28],
                                                                          concat(state.registers.flag[29],
                                                                          concat(state.registers.flag[30],
                                                                                 state.registers.flag[31])))))))))))))))));
                        break;
                    }
                    default:
                        throw Exception("double word access not valid for this register type");
                }
                break;

            default:
                throw Exception("invalid register access width");
        }
    } catch (...) {
        this->set_active_policies(active_policies);
        throw;
    }
    this->set_active_policies(active_policies);
    
    if (this->is_active(CONCRETE) && !retval.is_valid(CONCRETE))
        retval.set_subvalue(CONCRETE, convert_to_concrete(retval));
    if (this->is_active(INTERVAL) && !retval.is_valid(INTERVAL))
        retval.set_subvalue(INTERVAL, convert_to_interval(retval));
    if (this->is_active(SYMBOLIC) && !retval.is_valid(SYMBOLIC))
        retval.set_subvalue(SYMBOLIC, convert_to_symbolic(retval));

    this->writeRegister(reg, retval);
    return retval;
}

MULTI_DOMAIN_TEMPLATE
template<size_t nBits>
void
Policy<State, ValueType>::writeRegister(const char *regname, const ValueType<nBits> &value)
{
    const RegisterDescriptor &reg = this->findRegister(regname, nBits);
    this->template writeRegister(reg, value);
}

MULTI_DOMAIN_TEMPLATE
template<size_t nBits>
void
Policy<State, ValueType>::writeRegister(const RegisterDescriptor &reg, const ValueType<nBits> &value) {
    if (!triggered)
        return Super::template writeRegister<nBits>(reg, value);

    unsigned active_policies = this->get_active_policies();
    this->set_active_policies(0x07); // make them all active so our policy operations work properly in this function.
    Super::template writeRegister<nBits>(reg, value); // also store the value in the subpolicies' states so they have it.
    try {
        switch (nBits) {
            case 1:
                // Only FLAGS/EFLAGS bits have a size of one.  Other registers cannot be accessed at this granularity.
                if (reg.get_major()!=x86_regclass_flags)
                    throw Exception("bit access only valid for FLAGS/EFLAGS register");
                if (reg.get_minor()!=0 || reg.get_offset()>=state.registers.n_flags)
                    throw Exception("register not implemented in semantic policy");
                if (reg.get_nbits()!=1)
                    throw Exception("semantic policy supports only single-bit flags");
                state.registers.flag[reg.get_offset()] = this->template unsignedExtend<nBits, 1>(value);
                break;

            case 8:
                // Only general purpose registers can be accessed at byte granularity, and only for offsets 0 and 8.
                if (reg.get_major()!=x86_regclass_gpr)
                    throw Exception("byte access only valid for general purpose registers.");
                if (reg.get_minor()>=state.registers.n_gprs)
                    throw Exception("register not implemented in semantic policy");
                assert(reg.get_nbits()==8); // we had better be asking for a one-byte register (e.g., "ah", not "ax")
                switch (reg.get_offset()) {
                    case 0:
                        state.registers.gpr[reg.get_minor()] =
                            concat(this->template signExtend<nBits, 8>(value),
                                   this->template extract<8, 32>(state.registers.gpr[reg.get_minor()])); // no-op extend
                        break;
                    case 8:
                        state.registers.gpr[reg.get_minor()] =
                            concat(this->template extract<0, 8>(state.registers.gpr[reg.get_minor()]),
                                   concat(this->template unsignedExtend<nBits, 8>(value),
                                          this->template extract<16, 32>(state.registers.gpr[reg.get_minor()])));
                        break;
                    default:
                        throw Exception("invalid byte access offset");
                }
                break;

            case 16:
                if (reg.get_nbits()!=16)
                    throw Exception("invalid 2-byte register");
                if (reg.get_offset()!=0)
                    throw Exception("policy does not support non-zero offsets for word granularity register access");
                switch (reg.get_major()) {
                    case x86_regclass_segment:
                        if (reg.get_minor()>=state.registers.n_segregs)
                            throw Exception("register not implemented in semantic policy");
                        state.registers.segreg[reg.get_minor()] = this->template unsignedExtend<nBits, 16>(value);
                        break;
                    case x86_regclass_gpr:
                        if (reg.get_minor()>=state.registers.n_gprs)
                            throw Exception("register not implemented in semantic policy");
                        state.registers.gpr[reg.get_minor()] =
                            concat(this->template unsignedExtend<nBits, 16>(value),
                                   this->template extract<16, 32>(state.registers.gpr[reg.get_minor()]));
                        break;
                    case x86_regclass_flags:
                        if (reg.get_minor()!=0 || state.registers.n_flags<16)
                            throw Exception("register not implemented in semantic policy");
                        state.registers.flag[0]  = this->template extract<0,  1 >(value);
                        state.registers.flag[1]  = this->template extract<1,  2 >(value);
                        state.registers.flag[2]  = this->template extract<2,  3 >(value);
                        state.registers.flag[3]  = this->template extract<3,  4 >(value);
                        state.registers.flag[4]  = this->template extract<4,  5 >(value);
                        state.registers.flag[5]  = this->template extract<5,  6 >(value);
                        state.registers.flag[6]  = this->template extract<6,  7 >(value);
                        state.registers.flag[7]  = this->template extract<7,  8 >(value);
                        state.registers.flag[8]  = this->template extract<8,  9 >(value);
                        state.registers.flag[9]  = this->template extract<9,  10>(value);
                        state.registers.flag[10] = this->template extract<10, 11>(value);
                        state.registers.flag[11] = this->template extract<11, 12>(value);
                        state.registers.flag[12] = this->template extract<12, 13>(value);
                        state.registers.flag[13] = this->template extract<13, 14>(value);
                        state.registers.flag[14] = this->template extract<14, 15>(value);
                        state.registers.flag[15] = this->template extract<15, 16>(value);
                        break;
                    default:
                        throw Exception("word access not valid for this register type");
                }
                break;

            case 32:
                if (reg.get_offset()!=0)
                    throw Exception("policy does not support non-zero offsets for double word granularity register access");
                switch (reg.get_major()) {
                    case x86_regclass_gpr:
                        if (reg.get_minor()>=state.registers.n_gprs)
                            throw Exception("register not implemented in semantic policy");
                        state.registers.gpr[reg.get_minor()] = this->template signExtend<nBits, 32>(value);
                        break;
                    case x86_regclass_ip:
                        if (reg.get_minor()!=0)
                            throw Exception("register not implemented in semantic policy");
                        state.registers.ip = this->template unsignedExtend<nBits, 32>(value);
                        break;
                    case x86_regclass_flags:
                        if (reg.get_minor()!=0 || state.registers.n_flags<32)
                            throw Exception("register not implemented in semantic policy");
                        if (reg.get_nbits()!=32)
                            throw Exception("register is not 32 bits");
                        this->template writeRegister<16>("flags", this->template unsignedExtend<nBits, 16>(value));
                        state.registers.flag[16] = this->template extract<16, 17>(value);
                        state.registers.flag[17] = this->template extract<17, 18>(value);
                        state.registers.flag[18] = this->template extract<18, 19>(value);
                        state.registers.flag[19] = this->template extract<19, 20>(value);
                        state.registers.flag[20] = this->template extract<20, 21>(value);
                        state.registers.flag[21] = this->template extract<21, 22>(value);
                        state.registers.flag[22] = this->template extract<22, 23>(value);
                        state.registers.flag[23] = this->template extract<23, 24>(value);
                        state.registers.flag[24] = this->template extract<24, 25>(value);
                        state.registers.flag[25] = this->template extract<25, 26>(value);
                        state.registers.flag[26] = this->template extract<26, 27>(value);
                        state.registers.flag[27] = this->template extract<27, 28>(value);
                        state.registers.flag[28] = this->template extract<28, 29>(value);
                        state.registers.flag[29] = this->template extract<29, 30>(value);
                        state.registers.flag[30] = this->template extract<30, 31>(value);
                        state.registers.flag[31] = this->template extract<31, 32>(value);
                        break;
                    default:
                        throw Exception("double word access not valid for this register type");
                }
                break;

            default:
                throw Exception("invalid register access width");
        }
    } catch (...) {
        this->set_active_policies(active_policies);
        throw;
    }
    this->set_active_policies(active_policies);
}

MULTI_DOMAIN_TEMPLATE
template<size_t nBits>
ValueType<nBits>
Policy<State, ValueType>::readMemory(X86SegmentRegister sr, ValueType<32> addr, const ValueType<1> &cond)
{
    if (!triggered)
        return  Super::template readMemory<nBits>(sr, addr, cond);

    unsigned active_policies = this->get_active_policies();
    SMTSolver *solver = this->get_policy(SYMBOLIC).get_solver();
    SYMBOLIC_VALUE<32> a0 = convert_to_symbolic(addr);
        
    // Read a multi-byte value from memory in little-endian order.
    assert(8==nBits || 16==nBits || 32==nBits);
    ValueType<32> dword = this->concat(ValueType<24>(0), state.mem_read_byte(sr, a0, active_policies, solver));
    if (nBits>=16) {
        SYMBOLIC_VALUE<32> a1 = this->get_policy(SYMBOLIC).add(a0, SYMBOLIC_VALUE<32>(1));
        dword = this->or_(dword, this->concat(ValueType<16>(0),
                                              this->concat(state.mem_read_byte(sr, a1, active_policies, solver),
                                                           ValueType<8>(0))));
    }
    if (nBits>=24) {
        SYMBOLIC_VALUE<32> a2 = this->get_policy(SYMBOLIC).add(a0, SYMBOLIC_VALUE<32>(2));
        dword = this->or_(dword, this->concat(ValueType<8>(0),
                                              this->concat(state.mem_read_byte(sr, a2, active_policies, solver),
                                                           ValueType<16>(0))));
    }
    if (nBits>=32) {
        SYMBOLIC_VALUE<32> a3 = this->get_policy(SYMBOLIC).add(a0, SYMBOLIC_VALUE<32>(3));
        dword = this->or_(dword, this->concat(state.mem_read_byte(sr, a3, active_policies, solver),
                                              ValueType<24>(0)));
    }
    return this->template extract<0, nBits>(dword);
}

MULTI_DOMAIN_TEMPLATE
template<size_t nBits>
void
Policy<State, ValueType>::writeMemory(X86SegmentRegister sr, ValueType<32> addr,
                                      const ValueType<nBits> &data, const ValueType<1> &cond)
{
    if (!triggered)
        return Super::template writeMemory<nBits>(sr, addr, data, cond);

    SYMBOLIC_VALUE<32> a0 = convert_to_symbolic(addr);

    // Add the address/value pair to the mixed-semantics memory state, one byte at a time in little-endian order.
    assert(8==nBits || 16==nBits || 32==nBits);
    ValueType<8> b0 = this->template extract<0, 8>(data);
    state.mem_write_byte(sr, a0, b0);
    if (nBits>=16) {
        SYMBOLIC_VALUE<32> a1 = this->get_policy(SYMBOLIC).add(a0, SYMBOLIC_VALUE<32>(1));
        ValueType<8> b1 = this->template extract<8, 16>(data);
        state.mem_write_byte(sr, a1, b1);
    }
    if (nBits>=24) {
        SYMBOLIC_VALUE<32> a2 = this->get_policy(SYMBOLIC).add(a0, SYMBOLIC_VALUE<32>(2));
        ValueType<8> b2 = this->template extract<16, 24>(data);
        state.mem_write_byte(sr, a2, b2);
    }
    if (nBits>=32) {
        SYMBOLIC_VALUE<32> a3 = this->get_policy(SYMBOLIC).add(a0, SYMBOLIC_VALUE<32>(3));
        ValueType<8> b3 = this->template extract<24, 32>(data);
        state.mem_write_byte(sr, a3, b3);
    }
}

MULTI_DOMAIN_TEMPLATE
void
Policy<State, ValueType>::print(std::ostream &o, bool abbreviated) const
{
    state.print(o, abbreviated?this->get_active_policies() : 0x07);
}

template <template <size_t> class ValueType>
bool
State<ValueType>::must_alias(const SYMBOLIC_VALUE<32> &addr1, const SYMBOLIC_VALUE<32> &addr2, SMTSolver *solver)
{
    assert(solver);
    return addr1.get_expression()->equal_to(addr2.get_expression(), solver);
}

template <template <size_t> class ValueType>
bool
State<ValueType>::may_alias(const SYMBOLIC_VALUE<32> &addr1, const SYMBOLIC_VALUE<32> &addr2, SMTSolver *solver)
{
    using namespace InsnSemanticsExpr;
    if (must_alias(addr1, addr2, solver))
        return true;
    TreeNodePtr assertion = InternalNode::create(1, OP_EQ, addr1.get_expression(), addr2.get_expression());
    return SMTSolver::SAT_NO != solver->satisfiable(assertion);
}

template <template <size_t> class ValueType>
void
State<ValueType>::mem_write_byte(X86SegmentRegister sr, const SYMBOLIC_VALUE<32> &addr, const ValueType<8> &value)
{
    MemoryCells &cells = x86_segreg_ss==sr ? stack_cells : data_cells;
    cells.push_front(MemoryCell(addr, value));
}

template <template <size_t> class ValueType>
ValueType<8>
State<ValueType>::mem_read_byte(X86SegmentRegister sr, const SYMBOLIC_VALUE<32> &addr, unsigned active_policies,
                                SMTSolver *solver/*=NULL*/)
{
    ValueType<8> retval;
    MemoryCells &cells = x86_segreg_ss==sr ? stack_cells : data_cells;

    // Find all values that could be returned.  I.e., those stored at addresses that might be equal to 'addr'
    std::vector<ValueType<8> > found;
    for (typename MemoryCells::iterator ci=cells.begin(); ci!=cells.end(); ++ci) {
        if (may_alias(addr, ci->first, solver)) {
            found.push_back(ci->second);
            if (must_alias(addr, ci->first, solver))
                break;
        }
    }

    // If we're in the concrete domain, return a random found value (or a random value if none found)
    if (0 != (active_policies & CONCRETE.mask)) {
        if (found.empty()) {
            retval.set_subvalue(CONCRETE, CONCRETE_VALUE<8>(rand()%256));
        } else {
            retval.set_subvalue(CONCRETE, convert_to_concrete(found[rand()%found.size()]));
        }
    }

    // If we're in the interval domain, union all the found intervals. Return an interval that covers all values if we didn't
    // find any memory cells.
    if (0 != (active_policies & INTERVAL.mask)) {
        if (found.empty()) {
            retval.set_subvalue(INTERVAL, INTERVAL_VALUE<8>()); // any 8-bit value
        } else {
            BinaryAnalysis::InstructionSemantics::Intervals intervals;
            for (size_t i=0; i<found.size(); ++i) {
                INTERVAL_VALUE<8> iv = convert_to_interval(found[i]);
                intervals.insert_ranges(iv.get_intervals());
            }
            retval.set_subvalue(INTERVAL, INTERVAL_VALUE<8>(intervals));
        }
    }

    // If we're in the symbolic domain, return the set of all found values.  SMTSolver doesn't have a specific set theory, so
    // we use its memory representation.  We construct a memory state whose values are the distinct values we found above, and
    // whose addresses are all unbound variables.  We then apply a "read" operation to this state using an unbound variable. In
    // essence, we're creating a little memory-read operation that could evaluate to any of the values we found.
    if (0 != (active_policies & SYMBOLIC.mask)) {
        if (found.empty()) {
            retval.set_subvalue(SYMBOLIC, SYMBOLIC_VALUE<8>());
        } else if (1==found.size()) {
            retval.set_subvalue(SYMBOLIC, found[0].get_subvalue(SYMBOLIC));
        } else {
            using namespace InsnSemanticsExpr;
            TreeNodePtr expr = LeafNode::create_memory(8, "SET"); // empty set
            std::vector<TreeNodePtr> set_members; // members in the set
            for (size_t i=0; i<found.size(); ++i) {
                TreeNodePtr sv = convert_to_symbolic(found[i]).get_expression();
                bool dup = false; // is 'sv' equivalent to any already in found_symbolic[] ?
                for (size_t j=0; !dup && j<set_members.size(); ++j)
                    dup = sv->equal_to(set_members[j], solver);
                if (!dup) {
                    // Add 'sv' to the set
                    set_members.push_back(sv);
                    expr = InternalNode::create(8, OP_WRITE, expr, LeafNode::create_variable(8), sv);
                }
            }
            expr = InternalNode::create(8, OP_READ, expr, LeafNode::create_variable(8)); // set R-value
            retval.set_subvalue(SYMBOLIC, SYMBOLIC_VALUE<8>(expr));
        }
    }

    // Write the value back to memory so the next read returns the same thing (and returns faster)
    cells.push_front(MemoryCell(addr, retval));
    return retval;
}

template <template <size_t> class ValueType>
template <size_t nBits>
void
State<ValueType>::show_value(std::ostream &o, const std::string &prefix, const ValueType<nBits> &v, unsigned domains) const
{
    std::string prefix2(prefix.size(), ' ');
    const std::string *s = &prefix;

    if (v.is_valid(CONCRETE) && 0!=(domains & CONCRETE.mask)) {
        o <<*s <<"concrete: " <<v.get_subvalue(CONCRETE) <<"\n";
        s = &prefix2;
    }
    if (v.is_valid(INTERVAL) && 0!=(domains & INTERVAL.mask)) {
        o <<*s <<"interval: " <<v.get_subvalue(INTERVAL) <<"\n";
        s = &prefix2;
    }
    if (v.is_valid(SYMBOLIC) && 0!=(domains & SYMBOLIC.mask)) {
        o <<*s <<"symbolic: " <<v.get_subvalue(SYMBOLIC) <<"\n";
    }
}

template <template <size_t> class ValueType>
void
State<ValueType>::print(std::ostream &o, unsigned domains) const
{
    o <<"== General purpose registers ==\n";
    for (size_t i=0; i<registers.n_gprs; ++i) {
        std::ostringstream hdg;
        hdg <<"    " <<std::setw(7) <<std::left <<gprToString((X86GeneralPurposeRegister)i) <<" = ";
        show_value(o, hdg.str(), registers.gpr[i], domains);
    }
    show_value(o, "    ip      = ", registers.ip, domains);
    o <<"== Segment registers ==\n";
    for (size_t i=0; i<registers.n_segregs; ++i) {
        std::ostringstream hdg;
        hdg <<"    " <<std::setw(7) <<std::left <<segregToString((X86SegmentRegister)i) <<" = ";
        show_value(o, hdg.str(), registers.segreg[i], domains);
    }
    o <<"== CPU flags ==\n";
    for (size_t i=0; i<registers.n_flags; ++i) {
        std::ostringstream hdg;
        hdg <<"    " <<std::setw(7) <<std::left <<flagToString((X86Flag)i) <<" = ";
        show_value(o, hdg.str(), registers.flag[i], domains);
    }
    for (size_t i=0; i<2; ++i) {
        const MemoryCells &cells = 0==i ? stack_cells : data_cells;
        o <<"== Multi Memory (" <<(0==i?"stack":"data") <<" segment) ==\n";
        for (typename MemoryCells::const_iterator ci=cells.begin(); ci!=cells.end(); ++ci) {
            o <<"    address symbolic: " <<ci->first <<"\n";
            show_value(o, "      value ", ci->second, domains);
        }
    }
}

template <template <size_t> class ValueType, size_t nBits>
CONCRETE_VALUE<nBits>
convert_to_concrete(const ValueType<nBits> &v)
{
    if (v.is_valid(CONCRETE))
        return v.get_subvalue(CONCRETE);
    if (v.is_valid(INTERVAL) && 1==v.get_subvalue(INTERVAL).get_intervals().size())
        return CONCRETE_VALUE<nBits>(v.get_subvalue(INTERVAL).get_intervals().min());
    if (v.is_valid(SYMBOLIC) && v.get_subvalue(SYMBOLIC).is_known())
        return CONCRETE_VALUE<nBits>(v.get_subvalue(SYMBOLIC).known_value());
    CONCRETE_VALUE<nBits> retval = Robin::convert_to_concrete(v);
    std::cerr <<"Robin: converted " <<v <<" to concrete " <<retval <<"\n";
    return retval;
}

template <template <size_t> class ValueType, size_t nBits>
INTERVAL_VALUE<nBits>
convert_to_interval(const ValueType<nBits> &v)
{
    if (v.is_valid(INTERVAL))
        return v.get_subvalue(INTERVAL);
    if (v.is_valid(CONCRETE))
        return INTERVAL_VALUE<nBits>(v.get_subvalue(CONCRETE).known_value());
    INTERVAL_VALUE<nBits> retval = Robin::convert_to_interval(v);
    std::cerr <<"Robin: converted " <<v <<" to interval " <<retval <<"\n";
    return retval;
}

template <template <size_t> class ValueType, size_t nBits>
SYMBOLIC_VALUE<nBits>
convert_to_symbolic(const ValueType<nBits> &v)
{
    if (v.is_valid(SYMBOLIC))
        return v.get_subvalue(SYMBOLIC);
    if (v.is_valid(CONCRETE))
        return SYMBOLIC_VALUE<nBits>(v.get_subvalue(CONCRETE).known_value());
    if (v.is_valid(INTERVAL) && 1==v.get_subvalue(INTERVAL).get_intervals().size())
        return SYMBOLIC_VALUE<nBits>(v.get_subvalue(INTERVAL).get_intervals().min());
    SYMBOLIC_VALUE<nBits> retval = Robin::convert_to_symbolic(v);
    std::cerr <<"Robin: converted " <<v <<" to symbolic " <<retval <<"\n";
    return retval;
}

} // namespace
