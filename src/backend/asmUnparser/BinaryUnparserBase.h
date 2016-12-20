#ifndef ROSE_BinaryAnalysis_UnparserBase_H
#define ROSE_BinaryAnalysis_UnparserBase_H

#include <Partitioner2/BasicBlock.h>
#include <Partitioner2/DataBlock.h>
#include <Partitioner2/Function.h>
#include <Partitioner2/FunctionCallGraph.h>

namespace rose {
namespace BinaryAnalysis {

namespace Unparser {

struct SettingsBase {
    virtual ~SettingsBase() {}

    struct {
        bool showingReasons;                            /**< Show reasons for function existing. */
        struct {
            bool showing;                               /**< Show function call graph? */
        } cg;                                           /**< Settings for function call graphs. */
        struct {
            bool showing;                               /**< Show stack delta? */
            bool concrete;                              /**< Show concrete or symbolic deltas. */
        } stackDelta;                                   /**< Settings for function stack deltas. */
        struct {
            bool showing;                               /**< Show calling convention? */
        } callconv;                                     /**< Settings for function calling convention. */
        struct {
            bool showing;                               /**< Show no-op analysis results? */
        } noop;                                         /**< Settings for no-op analysis. */
        struct {
            bool showing;                               /**< Show results of may-return analysis? */
        } mayReturn;                                    /**< Settings for may-return analysis. */
    } function;                                         /**< Settings for functions. */

    struct {
        struct {
            bool showingPredecessors;                   /**< Show basic block predecessors? */
            bool showingSuccessors;                     /**< Show basic block successors? */
            bool showingSharing;                        /**< Show functions when block is shared? */
        } cfg;                                          /**< Settings for control flow graphs. */
    } bblock;                                           /**< Settings for basic blocks. */

    struct {
        struct {
            bool showing;                               /**< Show instruction addresses? */
            size_t fieldWidth;                          /**< Min characters to use per insn address. */
        } address;                                      /**< Settings for instruction starting addresses. */

        struct {
            bool showing;                               /**< Show instruction bytes? */
            size_t perLine;                             /**< Max number of bytes to show per line of output. */
            size_t fieldWidth;                          /**< Min characters to use for the bytes field. */
        } bytes;                                        /**< Settings for the bytes that make up an instruction. */

        struct {
            bool showing;                               /**< Show stack deltas? */
            size_t fieldWidth;                          /**< Min characters to use for the stack delta field. */
        } stackDelta;                                   /**< Settings for stack deltas. */

        struct {
            size_t fieldWidth;                          /**< Min characters to use for the instruction mnemonic. */
        } mnemonic;                                     /**< Settings for instruction mnemonics. */

        struct {
            std::string separator;                      /**< How to separate one operand from another. */
            size_t fieldWidth;                          /**< Min characters to use for the operand list. */
        } operands;                                     /**< Settings for the operand list. */

        struct {
            bool showing;                               /**< Show instruction comments? */
            std::string pre;                            /**< String to introduce a comment. */
            std::string post;                           /**< String to terminate a comment. */
            size_t fieldWidth;                          /**< Min characters to use for the comment field. */
        } comment;                                      /**< Settings for instruction comments. */
    } insn;                                             /**< Settings for instructions. */

    SettingsBase() {
        function.showingReasons = true;
        function.cg.showing = true;
        function.stackDelta.showing = true;
        function.stackDelta.concrete = true;
        function.callconv.showing = true;
        function.noop.showing = true;
        function.mayReturn.showing = true;

        bblock.cfg.showingPredecessors = true;
        bblock.cfg.showingSuccessors = true;
        bblock.cfg.showingSharing = true;

        insn.address.showing = true;
        insn.address.fieldWidth = 10;
        insn.bytes.showing = true;
        insn.bytes.perLine = 8;
        insn.bytes.fieldWidth = 25;
        insn.stackDelta.showing = true;
        insn.stackDelta.fieldWidth = 2;
        insn.mnemonic.fieldWidth = 1;
        insn.operands.separator = ", ";
        insn.operands.fieldWidth = 40;
        insn.comment.showing = true;
        insn.comment.pre = "; ";
        insn.comment.fieldWidth = 1;
    }
};

/** Unparses binary functions, blocks, and instructions. */
class UnparserBase {
protected:
    struct State {
        Partitioner2::Function::Ptr currentFunction;
        Partitioner2::BasicBlock::Ptr currentBasicBlock;
    };

    class FunctionGuard {
        UnparserBase::State &state;
        Partitioner2::Function::Ptr prev;
    public:
        FunctionGuard(State &state, const Partitioner2::Function::Ptr &f)
            : state(state) {
            prev = state.currentFunction;
            state.currentFunction = f;
        }

        ~FunctionGuard() {
            state.currentFunction = prev;
        }
    };

    class BasicBlockGuard {
        UnparserBase::State &state;
        Partitioner2::BasicBlock::Ptr prev;
    public:
        BasicBlockGuard(State &state, const Partitioner2::BasicBlock::Ptr &bb)
            : state(state) {
            prev = state.currentBasicBlock;
            state.currentBasicBlock = bb;
        }

        ~BasicBlockGuard() {
            state.currentBasicBlock = prev;
        }
    };

private:
    const Partitioner2::Partitioner &partitioner_;
    Partitioner2::FunctionCallGraph cg_;
    RegisterNames registerNames_;

protected:
    explicit UnparserBase(const Partitioner2::Partitioner &p)
        : partitioner_(p) {
        init();
    }

public:
    virtual ~UnparserBase() {}

    const Partitioner2::Partitioner& partitioner() const { return partitioner_; }
    const Partitioner2::FunctionCallGraph& cg() const { return cg_; }

    virtual const SettingsBase& settings() const = 0;
    virtual SettingsBase& settings() = 0;

    const RegisterNames& registerNames() const /*final*/ { return registerNames_; }
    void registerNames(const RegisterNames &r) /*final*/ { registerNames_ = r; }

    void operator()(std::ostream &out, SgAsmInstruction *insn) const /*final*/ { unparse(out, insn); }
    void operator()(std::ostream &out, const Partitioner2::BasicBlock::Ptr &bb) const /*final*/ { unparse(out, bb); }
    void operator()(std::ostream &out, const Partitioner2::DataBlock::Ptr &db) const /*final*/ { unparse(out, db); }
    void operator()(std::ostream &out, const Partitioner2::Function::Ptr &f) const /*final*/ { unparse(out, f); }

    std::string operator()(SgAsmInstruction *insn) const /*final*/;
    std::string operator()(const Partitioner2::BasicBlock::Ptr &bb) const /*final*/;
    std::string operator()(const Partitioner2::DataBlock::Ptr &db) const /*final*/;
    std::string operator()(const Partitioner2::Function::Ptr &f) const /*final*/;

    

protected:
    virtual void unparse(std::ostream&, SgAsmInstruction*) const;
    virtual void unparse(std::ostream&, const Partitioner2::BasicBlock::Ptr&) const;
    virtual void unparse(std::ostream&, const Partitioner2::DataBlock::Ptr&) const;
    virtual void unparse(std::ostream&, const Partitioner2::Function::Ptr&) const;

    virtual void emitFunction(std::ostream&, const Partitioner2::Function::Ptr&, State&) const;
    virtual void emitFunctionPrologue(std::ostream&, const Partitioner2::Function::Ptr&, State&) const;
    virtual void emitFunctionBody(std::ostream&, const Partitioner2::Function::Ptr&, State&) const;
    virtual void emitFunctionEpilogue(std::ostream&, const Partitioner2::Function::Ptr&, State&) const;

    virtual void emitFunctionReasons(std::ostream&, const Partitioner2::Function::Ptr&, State&) const;
    virtual void emitFunctionCallers(std::ostream&, const Partitioner2::Function::Ptr&, State&) const;
    virtual void emitFunctionCallees(std::ostream&, const Partitioner2::Function::Ptr&, State&) const;
    virtual void emitFunctionComment(std::ostream&, const Partitioner2::Function::Ptr&, State&) const;
    virtual void emitFunctionStackDelta(std::ostream&, const Partitioner2::Function::Ptr&, State&) const;
    virtual void emitFunctionCallingConvention(std::ostream&, const Partitioner2::Function::Ptr&, State&) const;
    virtual void emitFunctionNoopAnalysis(std::ostream&, const Partitioner2::Function::Ptr&, State&) const;
    virtual void emitFunctionMayReturn(std::ostream&, const Partitioner2::Function::Ptr&, State&) const;

    virtual void emitDataBlock(std::ostream&, const Partitioner2::DataBlock::Ptr&, State&) const;
    virtual void emitDataBlockPrologue(std::ostream&, const Partitioner2::DataBlock::Ptr&, State&) const;
    virtual void emitDataBlockBody(std::ostream&, const Partitioner2::DataBlock::Ptr&, State&) const;
    virtual void emitDataBlockEpilogue(std::ostream&, const Partitioner2::DataBlock::Ptr&, State&) const;
    
    virtual void emitBasicBlock(std::ostream&, const Partitioner2::BasicBlock::Ptr&, State&) const;
    virtual void emitBasicBlockPrologue(std::ostream&, const Partitioner2::BasicBlock::Ptr&, State&) const;
    virtual void emitBasicBlockBody(std::ostream&, const Partitioner2::BasicBlock::Ptr&, State&) const;
    virtual void emitBasicBlockEpilogue(std::ostream&, const Partitioner2::BasicBlock::Ptr&, State&) const;

    virtual void emitBasicBlockComment(std::ostream&, const Partitioner2::BasicBlock::Ptr&, State&) const;
    virtual void emitBasicBlockSharing(std::ostream&, const Partitioner2::BasicBlock::Ptr&, State&) const;
    virtual void emitBasicBlockPredecessors(std::ostream&, const Partitioner2::BasicBlock::Ptr&, State&) const;
    virtual void emitBasicBlockSuccessors(std::ostream&, const Partitioner2::BasicBlock::Ptr&, State&) const;

    virtual void emitInstruction(std::ostream&, SgAsmInstruction*, State&) const;
    virtual void emitInstructionPrologue(std::ostream&, SgAsmInstruction*, State&) const {}
    virtual void emitInstructionBody(std::ostream&, SgAsmInstruction*, State&) const;
    virtual void emitInstructionEpilogue(std::ostream&, SgAsmInstruction*, State&) const {};

    virtual void emitInstructionAddress(std::ostream&, SgAsmInstruction*, State&) const;
    virtual void emitInstructionBytes(std::ostream&, SgAsmInstruction*, State&) const;
    virtual void emitInstructionStackDelta(std::ostream&, SgAsmInstruction*, State&) const;
    virtual void emitInstructionMnemonic(std::ostream&, SgAsmInstruction*, State&) const;
    virtual void emitInstructionOperands(std::ostream&, SgAsmInstruction*, State&) const;
    virtual void emitInstructionComment(std::ostream&, SgAsmInstruction*, State&) const;

    virtual void emitOperand(std::ostream&, SgAsmExpression*, State&) const;
    virtual void emitOperandPrologue(std::ostream&, SgAsmExpression*, State&) const {};
    virtual void emitOperandBody(std::ostream&, SgAsmExpression*, State&) const = 0;
    virtual void emitOperandEpilogue(std::ostream&, SgAsmExpression*, State&) const {};

    virtual void emitRegister(std::ostream&, const RegisterDescriptor&, State&) const;
    virtual std::vector<std::string> emitUnsignedInteger(std::ostream&, const Sawyer::Container::BitVector&, State&) const;
    virtual std::vector<std::string> emitSignedInteger(std::ostream&, const Sawyer::Container::BitVector&, State&) const;
    virtual std::vector<std::string> emitInteger(std::ostream&, const Sawyer::Container::BitVector&, State&, bool isSigned) const;
    virtual void emitAddress(std::ostream&, rose_addr_t, State&) const;
    virtual void emitAddress(std::ostream&, const Sawyer::Container::BitVector&, State&) const;
    virtual void emitCommentBlock(std::ostream&, const std::string&, State&, const std::string &prefix = ";;; ") const;
    virtual void emitTypeName(std::ostream&, SgAsmType*, State&) const;

    //-----  Utility functions -----
public:
    static std::string leftJustify(const std::string&, size_t nchars);

    static std::string juxtaposeColumns(const std::vector<std::string> &content, const std::vector<size_t> &minWidths,
                                        const std::string &columnSeparator = " ");

private:
    void init();
};


} // namespace
} // namespace
} // namespace

#endif
