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

/** State for unparsing.
 *
 *  This object stores the current state for unparsing. The state is kept separate from the unparser class so that (1) the
 *  unparser can be a const reference, and (2) multiple threads can be unparsing with the same unparser object. */
class State {
public:
    typedef Sawyer::Container::Map<rose_addr_t, std::string> AddrString;/**< Map from address to string. */

private:
    const Partitioner2::Partitioner &partitioner_;
    Partitioner2::FunctionCallGraph cg_;
    Partitioner2::FunctionPtr currentFunction_;
    Partitioner2::BasicBlockPtr currentBasicBlock_;
    AddrString basicBlockLabels_;
    RegisterNames registerNames_;
    const Base &frontUnparser_;

public:
    State(const Partitioner2::Partitioner&, const Settings&, const Base &frontUnparser);
    virtual ~State();

    const Partitioner2::Partitioner& partitioner() const;
    const Partitioner2::FunctionCallGraph& cg() const;
    Partitioner2::FunctionPtr currentFunction() const;
    void currentFunction(const Partitioner2::FunctionPtr&);
    Partitioner2::BasicBlockPtr currentBasicBlock() const;
    void currentBasicBlock(const Partitioner2::BasicBlockPtr&);
    const RegisterNames& registerNames() const;
    void registerNames(const RegisterNames &r);
    const AddrString& basicBlockLabels() const;
    AddrString& basicBlockLabels();
    const Base& frontUnparser() const;
};
    
/** Abstract base class for unparsers.
 *
 *  This defines the interface only. All data that's used during unparsing is provided to each function so that this interface
 *  is thread-safe. Only the configuration settings are stored in the unparser itself.
 *
 *  End users generally inovke the high-level output methods, which are provided by non-virtual function operators that are
 *  overloaded on the type of object being unparsed.
 *
 *  The high-level function operators each call a corresponding overloaded virtual @ref unparse function that kicks things off
 *  by creating an unparse @ref State object, setting the front parser to the one whose @ref unparse method was called, and
 *  invoking an appropriate mid-level "emit" function in that front parser.
 *
 *  Mid-level "emit" functions use a combination of C++ virtual functions and unparser object chaining as described in the
 *  documentation for those functions. This permits two ways to override or augment behavior: behavior modification based on
 *  architecture (virtual functions) and behavior modification based on a filtering concept to support things like tables and
 *  HTML (chaining). */
class Base: public Sawyer::SharedObject {
public:
    typedef Sawyer::SharedPointer<Base> Ptr;

private:
    Ptr nextUnparser_;

protected:
    Base();
    explicit Base(const Ptr &nextUnparser);
    
public:
    virtual Ptr copy() const = 0;
    virtual ~Base();

    virtual const Settings& settings() const = 0;
    virtual Settings& settings() = 0;

    Ptr nextUnparser() const { return nextUnparser_; }

    /** Emit the entity to an output stream.
     *
     *  Renders the third argument as text and sends it to the stream indicated by the first argument.  The @p partitioner
     *  argument provides additional details about the thing being printed.  The version of this function that takes only two
     *  arguments causes all functions to be emitted.
     *
     *  @{ */
    void operator()(std::ostream &out, const Partitioner2::Partitioner &p) const /*final*/ {
        unparse(out, p);
    }
    void operator()(std::ostream &out, const Partitioner2::Partitioner &p,
                    SgAsmInstruction *insn) const /*final*/ {
        unparse(out, p, insn);
    }
    void operator()(std::ostream &out, const Partitioner2::Partitioner &p,
                    const Partitioner2::BasicBlockPtr &bb) const /*final*/{
        unparse(out, p, bb);
    }
    void operator()(std::ostream &out, const Partitioner2::Partitioner &p,
                    const Partitioner2::DataBlockPtr &db) const /*final*/ {
        unparse(out, p, db);
    }
    void operator()(std::ostream &out, const Partitioner2::Partitioner &p,
                    const Partitioner2::FunctionPtr &f) const /*final*/ {
        unparse(out, p, f);
    }
    /** @} */

    /** Emit the entity to a string.
     *
     *  This is just a convenience wrapper around the three-argument form.
     *
     * @{ */
    std::string operator()(const Partitioner2::Partitioner &p) const /*final*/ {
        return unparse(p);
    }
    std::string operator()(const Partitioner2::Partitioner &p, SgAsmInstruction *insn) const /*final*/ {
        return unparse(p, insn);
    }
    std::string operator()(const Partitioner2::Partitioner &p, const Partitioner2::BasicBlockPtr &bb) const /*final*/ {
        return unparse(p, bb);
    }
    std::string operator()(const Partitioner2::Partitioner &p, const Partitioner2::DataBlockPtr &db) const /*final*/ {
        return unparse(p, db);
    }
    std::string operator()(const Partitioner2::Partitioner &p, const Partitioner2::FunctionPtr &f) const /*final*/ {
        return unparse(p, f);
    }
    /** @} */


    

public:
    /** High-level unparsing function.
     *
     *  This function does the same thing as the function operator that has the same arguments.
     *
     * @{ */
    virtual void unparse(std::ostream&, const Partitioner2::Partitioner&) const;
    virtual void unparse(std::ostream&, const Partitioner2::Partitioner&, SgAsmInstruction*) const;
    virtual void unparse(std::ostream&, const Partitioner2::Partitioner&, const Partitioner2::BasicBlockPtr&) const;
    virtual void unparse(std::ostream&, const Partitioner2::Partitioner&, const Partitioner2::DataBlockPtr&) const;
    virtual void unparse(std::ostream&, const Partitioner2::Partitioner&, const Partitioner2::FunctionPtr&) const;

    virtual std::string unparse(const Partitioner2::Partitioner&) const;
    virtual std::string unparse(const Partitioner2::Partitioner&, SgAsmInstruction*) const;
    virtual std::string unparse(const Partitioner2::Partitioner&, const Partitioner2::BasicBlockPtr&) const;
    virtual std::string unparse(const Partitioner2::Partitioner&, const Partitioner2::DataBlockPtr&) const;
    virtual std::string unparse(const Partitioner2::Partitioner&, const Partitioner2::FunctionPtr&) const;
    /** @} */

public:
    /** Mid-level unparser function.
     *
     *  This function emits some entity to the specified output stream. All call-specific state is supplied in the State
     *  reference so that this function can be called with a const parser references and is thread safe per state object. The
     *  high-level functions create the state -- states are not intended for end users -- before calling this mid-level
     *  function.
     *
     *  This mid-level function uses a combination of C++ virtual function calls and object chaining. The virtual functions
     *  allow subclasses to override or augment behavior from the base class, so that, e.g., an x86 operand expression can be
     *  output in a different format than an m68k expression.  The object chaining allows one unparser to override or augment
     *  the behavior of another function and is used, for example, to provide HTML wrapping around various entities.
     *
     *  The object chaining is implemented in two parts: every unparser object has a @ref nextUnparser pointer and the final
     *  unparser in the list has a null pointer for this property; the state object has a @ref State::frontUnparser
     *  "frontUnparser" method that returns the first unparser in this list.  This mid-level function is expected to always
     *  invoke functions on the front unparser in order to give every parser in the chain a chance to influence behavior. The
     *  base implementation of this mid-level function defers to the next parser in the chain if there is one, otherwise it
     *  performs some default output that's hopefully appropriate for most unparsers.
     *
     *  All such chained and virtual emitters begin with the letters "emit".
     *
     * @{ */
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
    virtual void emitInstructionPrologue(std::ostream&, SgAsmInstruction*, State&) const;
    virtual void emitInstructionBody(std::ostream&, SgAsmInstruction*, State&) const;
    virtual void emitInstructionEpilogue(std::ostream&, SgAsmInstruction*, State&) const;

    virtual void emitInstructionAddress(std::ostream&, SgAsmInstruction*, State&) const;
    virtual void emitInstructionBytes(std::ostream&, SgAsmInstruction*, State&) const;
    virtual void emitInstructionStackDelta(std::ostream&, SgAsmInstruction*, State&) const;
    virtual void emitInstructionMnemonic(std::ostream&, SgAsmInstruction*, State&) const;
    virtual void emitInstructionOperands(std::ostream&, SgAsmInstruction*, State&) const;
    virtual void emitInstructionComment(std::ostream&, SgAsmInstruction*, State&) const;

    virtual void emitOperand(std::ostream&, SgAsmExpression*, State&) const;
    virtual void emitOperandPrologue(std::ostream&, SgAsmExpression*, State&) const;
    virtual void emitOperandBody(std::ostream&, SgAsmExpression*, State&) const;
    virtual void emitOperandEpilogue(std::ostream&, SgAsmExpression*, State&) const;

    virtual void emitRegister(std::ostream&, const RegisterDescriptor&, State&) const;
    virtual std::vector<std::string> emitUnsignedInteger(std::ostream&, const Sawyer::Container::BitVector&, State&) const;
    virtual std::vector<std::string> emitSignedInteger(std::ostream&, const Sawyer::Container::BitVector&, State&) const;
    virtual std::vector<std::string> emitInteger(std::ostream&, const Sawyer::Container::BitVector&, State&,
                                                 bool isSigned) const;
    virtual bool emitAddress(std::ostream&, rose_addr_t, State&, bool always=true) const;
    virtual bool emitAddress(std::ostream&, const Sawyer::Container::BitVector&, State&, bool always=true) const;
    virtual void emitCommentBlock(std::ostream&, const std::string&, State&, const std::string &prefix = ";;; ") const;
    virtual void emitTypeName(std::ostream&, SgAsmType*, State&) const;
    /** @} */

    //-----  Utility functions -----
public:
    static std::string leftJustify(const std::string&, size_t nchars);

    static std::string juxtaposeColumns(const std::vector<std::string> &content, const std::vector<size_t> &minWidths,
                                        const std::string &columnSeparator = " ");
};


} // namespace
} // namespace
} // namespace

#endif
