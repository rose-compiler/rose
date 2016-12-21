#ifndef ROSE_BinaryAnalysis_UnparserBase_H
#define ROSE_BinaryAnalysis_UnparserBase_H

#include <BinaryUnparser.h>
#include <Partitioner2/BasicTypes.h>
#include <Partitioner2/FunctionCallGraph.h>
#include <Sawyer/SharedObject.h>
#include <Registers.h>

namespace rose {
namespace BinaryAnalysis {

namespace Unparser {


/** Unparses binary functions, blocks, and instructions. */
class UnparserBase: public Sawyer::SharedObject {
public:
    typedef Sawyer::SharedPointer<UnparserBase> Ptr;

protected:
    struct State {
        Partitioner2::FunctionPtr currentFunction;
        Partitioner2::BasicBlockPtr currentBasicBlock;
        typedef Sawyer::Container::Map<rose_addr_t, std::string> AddrString;
        AddrString basicBlockLabels;
    };

    class FunctionGuard {
        UnparserBase::State &state;
        Partitioner2::FunctionPtr prev;
    public:
        FunctionGuard(State &state, const Partitioner2::FunctionPtr &f);
        ~FunctionGuard();
    };

    class BasicBlockGuard {
        UnparserBase::State &state;
        Partitioner2::BasicBlockPtr prev;
    public:
        BasicBlockGuard(State &state, const Partitioner2::BasicBlockPtr &bb);
        ~BasicBlockGuard();
    };

private:
    const Partitioner2::Partitioner *partitioner_;
    Partitioner2::FunctionCallGraph cg_;
    RegisterNames registerNames_;

protected:
    UnparserBase();
    explicit UnparserBase(const Partitioner2::Partitioner &p);

public:
    virtual Ptr create(const Partitioner2::Partitioner &p) const = 0;

    virtual ~UnparserBase();

    const Partitioner2::Partitioner& partitioner() const;
    const Partitioner2::FunctionCallGraph& cg() const { return cg_; }

    virtual const SettingsBase& settings() const = 0;
    virtual SettingsBase& settings() = 0;

    const RegisterNames& registerNames() const /*final*/ { return registerNames_; }
    void registerNames(const RegisterNames &r) /*final*/ { registerNames_ = r; }

    void operator()(std::ostream &out, SgAsmInstruction *insn) const /*final*/ { unparse(out, insn); }
    void operator()(std::ostream &out, const Partitioner2::BasicBlockPtr &bb) const /*final*/ { unparse(out, bb); }
    void operator()(std::ostream &out, const Partitioner2::DataBlockPtr &db) const /*final*/ { unparse(out, db); }
    void operator()(std::ostream &out, const Partitioner2::FunctionPtr &f) const /*final*/ { unparse(out, f); }

    std::string operator()(SgAsmInstruction *insn) const /*final*/;
    std::string operator()(const Partitioner2::BasicBlockPtr &bb) const /*final*/;
    std::string operator()(const Partitioner2::DataBlockPtr &db) const /*final*/;
    std::string operator()(const Partitioner2::FunctionPtr &f) const /*final*/;

    

protected:
    virtual void unparse(std::ostream&, SgAsmInstruction*) const;
    virtual void unparse(std::ostream&, const Partitioner2::BasicBlockPtr&) const;
    virtual void unparse(std::ostream&, const Partitioner2::DataBlockPtr&) const;
    virtual void unparse(std::ostream&, const Partitioner2::FunctionPtr&) const;

    virtual void emitFunction(std::ostream&, const Partitioner2::FunctionPtr&, State&) const;
    virtual void emitFunctionPrologue(std::ostream&, const Partitioner2::FunctionPtr&, State&) const;
    virtual void emitFunctionBody(std::ostream&, const Partitioner2::FunctionPtr&, State&) const;
    virtual void emitFunctionEpilogue(std::ostream&, const Partitioner2::FunctionPtr&, State&) const;

    virtual void emitFunctionReasons(std::ostream&, const Partitioner2::FunctionPtr&, State&) const;
    virtual void emitFunctionCallers(std::ostream&, const Partitioner2::FunctionPtr&, State&) const;
    virtual void emitFunctionCallees(std::ostream&, const Partitioner2::FunctionPtr&, State&) const;
    virtual void emitFunctionComment(std::ostream&, const Partitioner2::FunctionPtr&, State&) const;
    virtual void emitFunctionStackDelta(std::ostream&, const Partitioner2::FunctionPtr&, State&) const;
    virtual void emitFunctionCallingConvention(std::ostream&, const Partitioner2::FunctionPtr&, State&) const;
    virtual void emitFunctionNoopAnalysis(std::ostream&, const Partitioner2::FunctionPtr&, State&) const;
    virtual void emitFunctionMayReturn(std::ostream&, const Partitioner2::FunctionPtr&, State&) const;

    virtual void emitDataBlock(std::ostream&, const Partitioner2::DataBlockPtr&, State&) const;
    virtual void emitDataBlockPrologue(std::ostream&, const Partitioner2::DataBlockPtr&, State&) const;
    virtual void emitDataBlockBody(std::ostream&, const Partitioner2::DataBlockPtr&, State&) const;
    virtual void emitDataBlockEpilogue(std::ostream&, const Partitioner2::DataBlockPtr&, State&) const;
    
    virtual void emitBasicBlock(std::ostream&, const Partitioner2::BasicBlockPtr&, State&) const;
    virtual void emitBasicBlockPrologue(std::ostream&, const Partitioner2::BasicBlockPtr&, State&) const;
    virtual void emitBasicBlockBody(std::ostream&, const Partitioner2::BasicBlockPtr&, State&) const;
    virtual void emitBasicBlockEpilogue(std::ostream&, const Partitioner2::BasicBlockPtr&, State&) const;

    virtual void emitBasicBlockComment(std::ostream&, const Partitioner2::BasicBlockPtr&, State&) const;
    virtual void emitBasicBlockSharing(std::ostream&, const Partitioner2::BasicBlockPtr&, State&) const;
    virtual void emitBasicBlockPredecessors(std::ostream&, const Partitioner2::BasicBlockPtr&, State&) const;
    virtual void emitBasicBlockSuccessors(std::ostream&, const Partitioner2::BasicBlockPtr&, State&) const;

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
