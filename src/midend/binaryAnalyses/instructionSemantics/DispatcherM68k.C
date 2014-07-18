#include "sage3basic.h"
#include "BaseSemantics2.h"
#include "DispatcherM68k.h"

namespace BinaryAnalysis {
namespace InstructionSemantics2 {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Functors that handle individual M68k instruction kinds
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace M68k {

// An intermediate class that reduces the amount of typing in all that follows.  Its process() method does some up-front
// checking, dynamic casting, and pointer dereferencing and then calls the p() method that does the real work.
class P: public BaseSemantics::InsnProcessor {
public:
    typedef DispatcherM68k *D;
    typedef BaseSemantics::RiscOperators *Ops;
    typedef SgAsmM68kInstruction *I;
    typedef const SgAsmExpressionPtrList &A;
    virtual void p(D, Ops, I, A) = 0;

    virtual void process(const BaseSemantics::DispatcherPtr &dispatcher_, SgAsmInstruction *insn_)/*override*/ {
        DispatcherM68kPtr dispatcher = DispatcherM68k::promote(dispatcher_);
        BaseSemantics::RiscOperatorsPtr operators = dispatcher->get_operators();
        SgAsmM68kInstruction *insn = isSgAsmM68kInstruction(insn_);
        ASSERT_not_null(insn);
        ASSERT_require(insn == operators->get_insn());
        operators->writeRegister(dispatcher->REG_PC, operators->add(operators->number_(32, insn->get_address()),
                                                                    operators->number_(32, insn->get_size())));
        SgAsmExpressionPtrList &operands = insn->get_operandList()->get_operands();
        p(dispatcher.get(), operators.get(), insn, operands);
    }

    void assert_args(I insn, A args, size_t nargs) {
        if (args.size()!=nargs) {
            std::string mesg = "instruction must have " + StringUtility::plural(nargs, "arguments");
            throw BaseSemantics::Exception(mesg, insn);
        }
    }
};

struct IP_abcd: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_add: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_adda: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_addi: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_addq: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_addx: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_and: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_andi: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_asl: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_asr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_bra: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_bsr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_bhi: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_bls: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_bcc: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_bcs: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_bne: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_beq: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_bvc: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_bvs: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_bpl: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_bmi: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_bge: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_blt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_bgt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_ble: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_bchg: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_bclr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_bfchg: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_bfclr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_bfexts: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_bfextu: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_bfins: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 4);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_bfset: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_bftst: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_bkpt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_bset: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_btst: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_callm: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_cas: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_cas2: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 6);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_chk: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_chk2: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_clr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_cmp: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_cmpa: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_cmpi: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_cmpm: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_cmp2: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_cpushl: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_cpushp: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_cpusha: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_dbt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_dbf: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_dbhi: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_dbls: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_dbcc: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_dbcs: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_dbne: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_dbeq: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_dbvc: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_dbvs: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_dbpl: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_dbmi: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_dbge: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_dblt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_dbgt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_dble: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_divs: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_divrem: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.size()==2) {
            assert_args(insn, args, 2);
            throw BaseSemantics::Exception("semantics not implemented", insn);
        } else {
            assert_args(insn, args, 3);
            throw BaseSemantics::Exception("semantics not implemented", insn);
        }
    }
};

struct IP_divu: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_eor: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_eori: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_exg: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_ext: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_extb: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fabs: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fsabs: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fdabs: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fadd: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fsadd: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fdadd: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbeq: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbne: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbgt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbngt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbge: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbnge: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fblt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbnlt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fble: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbnle: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbgl: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbngl: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbgle: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbngle: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbogt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbule: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fboge: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbult: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbolt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbuge: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbole: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbugt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbogl: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbueq: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbor: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbun: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbf: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbsf: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbst: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbseq: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fbsne: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fcmp: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fdiv: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fsdiv: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fddiv: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fint: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fintrz: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fmove: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fsmove: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fdmove: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fmovem: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fmul: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fsmul: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fdmul: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fneg: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fsneg: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fdneg: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fnop: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fsqrt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fssqrt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fdsqrt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fsub: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fssub: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_fdsub: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_ftst: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_illegal: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_jmp: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_jsr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_lea: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_link: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_lsl: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_lsr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_mac: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 6);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_mov3q: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_move: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_move16: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_movea: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_movem: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_movep: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_moveq: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_muls: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_mulu: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_mvs: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_mvz: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_nbcd: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_neg: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_negx: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_nop: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_not: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_or: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_ori: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_pack: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_pea: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_rems: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_remu: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_rol: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_ror: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_roxl: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_roxr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_rtd: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_rtm: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_rtr: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_rts: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_sbcd: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_st: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_sf: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_shi: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_sls: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_scc: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_scs: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_sne: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_seq: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_svc: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_svs: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_spl: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_smi: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_sge: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_slt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_sgt: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_sle: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_sub: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_suba: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_subi: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_subq: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_subx: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 2);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_swap: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_tas: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_trap: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_trapt: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.empty()) {
            throw BaseSemantics::Exception("semantics not implemented", insn);
        } else {
            assert_args(insn, args, 1);
            throw BaseSemantics::Exception("semantics not implemented", insn);
        }
    }
};

struct IP_trapf: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.empty()) {
            throw BaseSemantics::Exception("semantics not implemented", insn);
        } else {
            assert_args(insn, args, 1);
            throw BaseSemantics::Exception("semantics not implemented", insn);
        }
    }
};

struct IP_traphi: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.empty()) {
            throw BaseSemantics::Exception("semantics not implemented", insn);
        } else {
            assert_args(insn, args, 1);
            throw BaseSemantics::Exception("semantics not implemented", insn);
        }
    }
};

struct IP_trapls: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.empty()) {
            throw BaseSemantics::Exception("semantics not implemented", insn);
        } else {
            assert_args(insn, args, 1);
            throw BaseSemantics::Exception("semantics not implemented", insn);
        }
    }
};

struct IP_trapcc: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.empty()) {
            throw BaseSemantics::Exception("semantics not implemented", insn);
        } else {
            assert_args(insn, args, 1);
            throw BaseSemantics::Exception("semantics not implemented", insn);
        }
    }
};

struct IP_trapcs: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.empty()) {
            throw BaseSemantics::Exception("semantics not implemented", insn);
        } else {
            assert_args(insn, args, 1);
            throw BaseSemantics::Exception("semantics not implemented", insn);
        }
    }
};

struct IP_trapne: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.empty()) {
            throw BaseSemantics::Exception("semantics not implemented", insn);
        } else {
            assert_args(insn, args, 1);
            throw BaseSemantics::Exception("semantics not implemented", insn);
        }
    }
};

struct IP_trapeq: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.empty()) {
            throw BaseSemantics::Exception("semantics not implemented", insn);
        } else {
            assert_args(insn, args, 1);
            throw BaseSemantics::Exception("semantics not implemented", insn);
        }
    }
};

struct IP_trapvc: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.empty()) {
            throw BaseSemantics::Exception("semantics not implemented", insn);
        } else {
            assert_args(insn, args, 1);
            throw BaseSemantics::Exception("semantics not implemented", insn);
        }
    }
};

struct IP_trapvs: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.empty()) {
            throw BaseSemantics::Exception("semantics not implemented", insn);
        } else {
            assert_args(insn, args, 1);
            throw BaseSemantics::Exception("semantics not implemented", insn);
        }
    }
};

struct IP_trappl: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.empty()) {
            throw BaseSemantics::Exception("semantics not implemented", insn);
        } else {
            assert_args(insn, args, 1);
            throw BaseSemantics::Exception("semantics not implemented", insn);
        }
    }
};

struct IP_trapmi: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.empty()) {
            throw BaseSemantics::Exception("semantics not implemented", insn);
        } else {
            assert_args(insn, args, 1);
            throw BaseSemantics::Exception("semantics not implemented", insn);
        }
    }
};

struct IP_trapge: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.empty()) {
            throw BaseSemantics::Exception("semantics not implemented", insn);
        } else {
            assert_args(insn, args, 1);
            throw BaseSemantics::Exception("semantics not implemented", insn);
        }
    }
};

struct IP_traplt: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.empty()) {
            throw BaseSemantics::Exception("semantics not implemented", insn);
        } else {
            assert_args(insn, args, 1);
            throw BaseSemantics::Exception("semantics not implemented", insn);
        }
    }
};

struct IP_trapgt: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.empty()) {
            throw BaseSemantics::Exception("semantics not implemented", insn);
        } else {
            assert_args(insn, args, 1);
            throw BaseSemantics::Exception("semantics not implemented", insn);
        }
    }
};

struct IP_traple: P {
    void p(D d, Ops ops, I insn, A args) {
        if (args.empty()) {
            throw BaseSemantics::Exception("semantics not implemented", insn);
        } else {
            assert_args(insn, args, 1);
            throw BaseSemantics::Exception("semantics not implemented", insn);
        }
    }
};

struct IP_trapv: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 0);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_tst: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_unlk: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 1);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

struct IP_unpk: P {
    void p(D d, Ops ops, I insn, A args) {
        assert_args(insn, args, 3);
        throw BaseSemantics::Exception("semantics not implemented", insn);
    }
};

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      DispatcherM68k
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
DispatcherM68k::iproc_init()
{
    iproc_set(m68k_abcd,        new M68k::IP_abcd);
    iproc_set(m68k_add,         new M68k::IP_add);
    iproc_set(m68k_adda,        new M68k::IP_adda);
    iproc_set(m68k_addi,        new M68k::IP_addi);
    iproc_set(m68k_addq,        new M68k::IP_addq);
    iproc_set(m68k_addx,        new M68k::IP_addx);
    iproc_set(m68k_and,         new M68k::IP_and);
    iproc_set(m68k_andi,        new M68k::IP_andi);
    iproc_set(m68k_asl,         new M68k::IP_asl);
    iproc_set(m68k_asr,         new M68k::IP_asr);
    iproc_set(m68k_bra,         new M68k::IP_bra);
    iproc_set(m68k_bsr,         new M68k::IP_bsr);
    iproc_set(m68k_bhi,         new M68k::IP_bhi);
    iproc_set(m68k_bls,         new M68k::IP_bls);
    iproc_set(m68k_bcc,         new M68k::IP_bcc);
    iproc_set(m68k_bcs,         new M68k::IP_bcs);
    iproc_set(m68k_bne,         new M68k::IP_bne);
    iproc_set(m68k_beq,         new M68k::IP_beq);
    iproc_set(m68k_bvc,         new M68k::IP_bvc);
    iproc_set(m68k_bvs,         new M68k::IP_bvs);
    iproc_set(m68k_bpl,         new M68k::IP_bpl);
    iproc_set(m68k_bmi,         new M68k::IP_bmi);
    iproc_set(m68k_bge,         new M68k::IP_bge);
    iproc_set(m68k_blt,         new M68k::IP_blt);
    iproc_set(m68k_bgt,         new M68k::IP_bgt);
    iproc_set(m68k_ble,         new M68k::IP_ble);
    iproc_set(m68k_bchg,        new M68k::IP_bchg);
    iproc_set(m68k_bclr,        new M68k::IP_bclr);
    iproc_set(m68k_bfchg,       new M68k::IP_bfchg);
    iproc_set(m68k_bfclr,       new M68k::IP_bfclr);
    iproc_set(m68k_bfexts,      new M68k::IP_bfexts);
    iproc_set(m68k_bfextu,      new M68k::IP_bfextu);
    iproc_set(m68k_bfins,       new M68k::IP_bfins);
    iproc_set(m68k_bfset,       new M68k::IP_bfset);
    iproc_set(m68k_bftst,       new M68k::IP_bftst);
    iproc_set(m68k_bkpt,        new M68k::IP_bkpt);
    iproc_set(m68k_bset,        new M68k::IP_bset);
    iproc_set(m68k_btst,        new M68k::IP_btst);
    iproc_set(m68k_callm,       new M68k::IP_callm);
    iproc_set(m68k_cas,         new M68k::IP_cas);
    iproc_set(m68k_cas2,        new M68k::IP_cas2);
    iproc_set(m68k_chk,         new M68k::IP_chk);
    iproc_set(m68k_chk2,        new M68k::IP_chk2);
    iproc_set(m68k_clr,         new M68k::IP_clr);
    iproc_set(m68k_cmp,         new M68k::IP_cmp);
    iproc_set(m68k_cmpa,        new M68k::IP_cmpa);
    iproc_set(m68k_cmpi,        new M68k::IP_cmpi);
    iproc_set(m68k_cmpm,        new M68k::IP_cmpm);
    iproc_set(m68k_cmp2,        new M68k::IP_cmp2);
    iproc_set(m68k_cpushl,      new M68k::IP_cpushl);
    iproc_set(m68k_cpushp,      new M68k::IP_cpushp);
    iproc_set(m68k_cpusha,      new M68k::IP_cpusha);
    iproc_set(m68k_dbt,         new M68k::IP_dbt);
    iproc_set(m68k_dbf,         new M68k::IP_dbf);
    iproc_set(m68k_dbhi,        new M68k::IP_dbhi);
    iproc_set(m68k_dbls,        new M68k::IP_dbls);
    iproc_set(m68k_dbcc,        new M68k::IP_dbcc);
    iproc_set(m68k_dbcs,        new M68k::IP_dbcs);
    iproc_set(m68k_dbne,        new M68k::IP_dbne);
    iproc_set(m68k_dbeq,        new M68k::IP_dbeq);
    iproc_set(m68k_dbvc,        new M68k::IP_dbvc);
    iproc_set(m68k_dbvs,        new M68k::IP_dbvs);
    iproc_set(m68k_dbpl,        new M68k::IP_dbpl);
    iproc_set(m68k_dbmi,        new M68k::IP_dbmi);
    iproc_set(m68k_dbge,        new M68k::IP_dbge);
    iproc_set(m68k_dblt,        new M68k::IP_dblt);
    iproc_set(m68k_dbgt,        new M68k::IP_dbgt);
    iproc_set(m68k_dble,        new M68k::IP_dble);
    iproc_set(m68k_divs,        new M68k::IP_divs);
    iproc_set(m68k_divu,        new M68k::IP_divu);
    iproc_set(m68k_eor,         new M68k::IP_eor);
    iproc_set(m68k_eori,        new M68k::IP_eori);
    iproc_set(m68k_exg,         new M68k::IP_exg);
    iproc_set(m68k_ext,         new M68k::IP_ext);
    iproc_set(m68k_extb,        new M68k::IP_extb);
    iproc_set(m68k_fabs,        new M68k::IP_fabs);
    iproc_set(m68k_fsabs,       new M68k::IP_fsabs);
    iproc_set(m68k_fdabs,       new M68k::IP_fdabs);
    iproc_set(m68k_fadd,        new M68k::IP_fadd);
    iproc_set(m68k_fsadd,       new M68k::IP_fsadd);
    iproc_set(m68k_fdadd,       new M68k::IP_fdadd);
    iproc_set(m68k_fbeq,        new M68k::IP_fbeq);
    iproc_set(m68k_fbne,        new M68k::IP_fbne);
    iproc_set(m68k_fbgt,        new M68k::IP_fbgt);
    iproc_set(m68k_fbngt,       new M68k::IP_fbngt);
    iproc_set(m68k_fbge,        new M68k::IP_fbge);
    iproc_set(m68k_fbnge,       new M68k::IP_fbnge);
    iproc_set(m68k_fblt,        new M68k::IP_fblt);
    iproc_set(m68k_fbnlt,       new M68k::IP_fbnlt);
    iproc_set(m68k_fble,        new M68k::IP_fble);
    iproc_set(m68k_fbnle,       new M68k::IP_fbnle);
    iproc_set(m68k_fbgl,        new M68k::IP_fbgl);
    iproc_set(m68k_fbngl,       new M68k::IP_fbngl);
    iproc_set(m68k_fbgle,       new M68k::IP_fbgle);
    iproc_set(m68k_fbngle,      new M68k::IP_fbngle);
    iproc_set(m68k_fbogt,       new M68k::IP_fbogt);
    iproc_set(m68k_fbule,       new M68k::IP_fbule);
    iproc_set(m68k_fboge,       new M68k::IP_fboge);
    iproc_set(m68k_fbult,       new M68k::IP_fbult);
    iproc_set(m68k_fbolt,       new M68k::IP_fbolt);
    iproc_set(m68k_fbuge,       new M68k::IP_fbuge);
    iproc_set(m68k_fbole,       new M68k::IP_fbole);
    iproc_set(m68k_fbugt,       new M68k::IP_fbugt);
    iproc_set(m68k_fbogl,       new M68k::IP_fbogl);
    iproc_set(m68k_fbueq,       new M68k::IP_fbueq);
    iproc_set(m68k_fbor,        new M68k::IP_fbor);
    iproc_set(m68k_fbun,        new M68k::IP_fbun);
    iproc_set(m68k_fbf,         new M68k::IP_fbf);
    iproc_set(m68k_fbt,         new M68k::IP_fbt);
    iproc_set(m68k_fbsf,        new M68k::IP_fbsf);
    iproc_set(m68k_fbst,        new M68k::IP_fbst);
    iproc_set(m68k_fbseq,       new M68k::IP_fbseq);
    iproc_set(m68k_fbsne,       new M68k::IP_fbsne);
    iproc_set(m68k_fcmp,        new M68k::IP_fcmp);
    iproc_set(m68k_fdiv,        new M68k::IP_fdiv);
    iproc_set(m68k_fsdiv,       new M68k::IP_fsdiv);
    iproc_set(m68k_fddiv,       new M68k::IP_fddiv);
    iproc_set(m68k_fint,        new M68k::IP_fint);
    iproc_set(m68k_fintrz,      new M68k::IP_fintrz);
    iproc_set(m68k_fmove,       new M68k::IP_fmove);
    iproc_set(m68k_fsmove,      new M68k::IP_fsmove);
    iproc_set(m68k_fdmove,      new M68k::IP_fdmove);
    iproc_set(m68k_fmovem,      new M68k::IP_fmovem);
    iproc_set(m68k_fmul,        new M68k::IP_fmul);
    iproc_set(m68k_fsmul,       new M68k::IP_fsmul);
    iproc_set(m68k_fdmul,       new M68k::IP_fdmul);
    iproc_set(m68k_fneg,        new M68k::IP_fneg);
    iproc_set(m68k_fsneg,       new M68k::IP_fsneg);
    iproc_set(m68k_fdneg,       new M68k::IP_fdneg);
    iproc_set(m68k_fnop,        new M68k::IP_fnop);
    iproc_set(m68k_fsqrt,       new M68k::IP_fsqrt);
    iproc_set(m68k_fssqrt,      new M68k::IP_fssqrt);
    iproc_set(m68k_fdsqrt,      new M68k::IP_fdsqrt);
    iproc_set(m68k_fsub,        new M68k::IP_fsub);
    iproc_set(m68k_fssub,       new M68k::IP_fssub);
    iproc_set(m68k_fdsub,       new M68k::IP_fdsub);
    iproc_set(m68k_ftst,        new M68k::IP_ftst);
    iproc_set(m68k_illegal,     new M68k::IP_illegal);
    iproc_set(m68k_jmp,         new M68k::IP_jmp);
    iproc_set(m68k_jsr,         new M68k::IP_jsr);
    iproc_set(m68k_lea,         new M68k::IP_lea);
    iproc_set(m68k_link,        new M68k::IP_link);
    iproc_set(m68k_lsl,         new M68k::IP_lsl);
    iproc_set(m68k_lsr,         new M68k::IP_lsr);
    iproc_set(m68k_mac,         new M68k::IP_mac);
    iproc_set(m68k_mov3q,       new M68k::IP_mov3q);
    iproc_set(m68k_move,        new M68k::IP_move);
    iproc_set(m68k_move16,      new M68k::IP_move16);
    iproc_set(m68k_movea,       new M68k::IP_movea);
    iproc_set(m68k_movem,       new M68k::IP_movem);
    iproc_set(m68k_movep,       new M68k::IP_movep);
    iproc_set(m68k_moveq,       new M68k::IP_moveq);
    iproc_set(m68k_muls,        new M68k::IP_muls);
    iproc_set(m68k_mulu,        new M68k::IP_mulu);
    iproc_set(m68k_mvs,         new M68k::IP_mvs);
    iproc_set(m68k_mvz,         new M68k::IP_mvz);
    iproc_set(m68k_nbcd,        new M68k::IP_nbcd);
    iproc_set(m68k_neg,         new M68k::IP_neg);
    iproc_set(m68k_negx,        new M68k::IP_negx);
    iproc_set(m68k_nop,         new M68k::IP_nop);
    iproc_set(m68k_not,         new M68k::IP_not);
    iproc_set(m68k_or,          new M68k::IP_or);
    iproc_set(m68k_ori,         new M68k::IP_ori);
    iproc_set(m68k_pack,        new M68k::IP_pack);
    iproc_set(m68k_pea,         new M68k::IP_pea);
    iproc_set(m68k_rems,        new M68k::IP_rems);
    iproc_set(m68k_remu,        new M68k::IP_remu);
    iproc_set(m68k_rol,         new M68k::IP_rol);
    iproc_set(m68k_ror,         new M68k::IP_ror);
    iproc_set(m68k_roxl,        new M68k::IP_roxl);
    iproc_set(m68k_roxr,        new M68k::IP_roxr);
    iproc_set(m68k_rtd,         new M68k::IP_rtd);
    iproc_set(m68k_rtm,         new M68k::IP_rtm);
    iproc_set(m68k_rtr,         new M68k::IP_rtr);
    iproc_set(m68k_rts,         new M68k::IP_rts);
    iproc_set(m68k_sbcd,        new M68k::IP_sbcd);
    iproc_set(m68k_st,          new M68k::IP_st);
    iproc_set(m68k_sf,          new M68k::IP_sf);
    iproc_set(m68k_shi,         new M68k::IP_shi);
    iproc_set(m68k_sls,         new M68k::IP_sls);
    iproc_set(m68k_scc,         new M68k::IP_scc);
    iproc_set(m68k_scs,         new M68k::IP_scs);
    iproc_set(m68k_sne,         new M68k::IP_sne);
    iproc_set(m68k_seq,         new M68k::IP_seq);
    iproc_set(m68k_svc,         new M68k::IP_svc);
    iproc_set(m68k_svs,         new M68k::IP_svs);
    iproc_set(m68k_spl,         new M68k::IP_spl);
    iproc_set(m68k_smi,         new M68k::IP_smi);
    iproc_set(m68k_sge,         new M68k::IP_sge);
    iproc_set(m68k_slt,         new M68k::IP_slt);
    iproc_set(m68k_sgt,         new M68k::IP_sgt);
    iproc_set(m68k_sle,         new M68k::IP_sle);
    iproc_set(m68k_sub,         new M68k::IP_sub);
    iproc_set(m68k_suba,        new M68k::IP_suba);
    iproc_set(m68k_subi,        new M68k::IP_subi);
    iproc_set(m68k_subq,        new M68k::IP_subq);
    iproc_set(m68k_subx,        new M68k::IP_subx);
    iproc_set(m68k_swap,        new M68k::IP_swap);
    iproc_set(m68k_tas,         new M68k::IP_tas);
    iproc_set(m68k_trap,        new M68k::IP_trap);
    iproc_set(m68k_trapt,       new M68k::IP_trapt);
    iproc_set(m68k_trapf,       new M68k::IP_trapf);
    iproc_set(m68k_traphi,      new M68k::IP_traphi);
    iproc_set(m68k_trapls,      new M68k::IP_trapls);
    iproc_set(m68k_trapcc,      new M68k::IP_trapcc);
    iproc_set(m68k_trapcs,      new M68k::IP_trapcs);
    iproc_set(m68k_trapne,      new M68k::IP_trapne);
    iproc_set(m68k_trapeq,      new M68k::IP_trapeq);
    iproc_set(m68k_trapvc,      new M68k::IP_trapvc);
    iproc_set(m68k_trapvs,      new M68k::IP_trapvs);
    iproc_set(m68k_trappl,      new M68k::IP_trappl);
    iproc_set(m68k_trapmi,      new M68k::IP_trapmi);
    iproc_set(m68k_trapge,      new M68k::IP_trapge);
    iproc_set(m68k_traplt,      new M68k::IP_traplt);
    iproc_set(m68k_trapgt,      new M68k::IP_trapgt);
    iproc_set(m68k_traple,      new M68k::IP_traple);
    iproc_set(m68k_trapv,       new M68k::IP_trapv);
    iproc_set(m68k_tst,         new M68k::IP_tst);
    iproc_set(m68k_unlk,        new M68k::IP_unlk);
    iproc_set(m68k_unpk,        new M68k::IP_unpk);
}

void
DispatcherM68k::regcache_init()
{
    if (regdict) {
        for (int i=0; i<8; ++i) {
            REG_D[i] = findRegister("d"+StringUtility::numberToString(i), 32);
            REG_A[i] = findRegister("a"+StringUtility::numberToString(i), 32);
            REG_FP[i] = findRegister("fp"+StringUtility::numberToString(i));
            ASSERT_require2(REG_FP[i].get_nbits()==64 || REG_FP[i].get_nbits()==80, "invalid floating point register size");
        }
        REG_PC = findRegister("pc", 32);
    }
}

void
DispatcherM68k::set_register_dictionary(const RegisterDictionary *regdict)
{
    BaseSemantics::Dispatcher::set_register_dictionary(regdict);
    regcache_init();
}



} // namespace
} // namespace
