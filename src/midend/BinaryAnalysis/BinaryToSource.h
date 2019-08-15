#ifndef ROSE_BinaryAnalysis_BinaryToSource_H
#define ROSE_BinaryAnalysis_BinaryToSource_H

#include <Diagnostics.h>
#include <Partitioner2/Partitioner.h>
#include <RoseException.h>
#include <Sawyer/CommandLine.h>
#include <SourceAstSemantics2.h>
#include <TraceSemantics2.h>

namespace Rose {
namespace BinaryAnalysis {

/** Convert binary to low-level C source code.
 *
 *  This analysis generates very low-level C source code that is as close as possible to the binary. This is not a decompiler;
 *  the source that gets generated is hardly human readable, but it can be fed into source code analysis functions. */
class BinaryToSource {
public:

    /** Settings to control this translator's behavior. */
    struct Settings {
        /** Trace the RISC operators for debugging. This causes diagnostic output to be emitted as the source code is being
         *  generated. */        
        bool traceRiscOps;

        /** Generate code that prints instructions.  This causes the generated source to contain calls to printf so that each
         *  instruction is printed to standard error as it is executed. */
        bool traceInsnExecution;

        /** Optional initial value for the instruction pointer. */
        Sawyer::Optional<rose_addr_t> initialInstructionPointer;

        /** Optional initial value for the stack pointer. */
        Sawyer::Optional<rose_addr_t> initialStackPointer;

        /** Whether to allocate the global memory array.  If this setting has no value (the default) then the global "mem"
         *  array is not allocated, but rather declared "extern".  If the setting is zero, then the array is allocated to be
         *  just large enough to hold the maximum initialized memory address. Otherwise, the array is allocated to the
         *  specified size. */
        Sawyer::Optional<rose_addr_t> allocateMemoryArray;

        /** Whether to zero the memory array, or just allocated with malloc. */
        bool zeroMemoryArray;

        /** Constructs the default settings. */
        Settings()
            : traceRiscOps(false), traceInsnExecution(false), allocateMemoryArray(false), zeroMemoryArray(false) {}
    };

    /** Exceptions thrown by this analysis. */
    class Exception: public Rose::Exception {
    public:
        /** Constructs an exception with the specified message. */
        Exception(const std::string &mesg): Rose::Exception(mesg) {}
    };

private:
    static Diagnostics::Facility mlog;
    Settings settings_;
    Disassembler *disassembler_;
    InstructionSemantics2::SourceAstSemantics::RiscOperatorsPtr raisingOps_;
    InstructionSemantics2::TraceSemantics::RiscOperatorsPtr tracingOps_;
    InstructionSemantics2::BaseSemantics::DispatcherPtr raisingCpu_;
    
public:
    /** Default constructor.
     *
     *  Constructs an analysis object that is not tied to any particular architecture yet, and which uses default
     *  settings. */
    BinaryToSource()
        : disassembler_(NULL) {}

    /** Construct the analyzer with specified settings.
     *
     *  Constructs an analysis object that is not tied to any particular architecture yet, but which uses the specified
     *  settings. */
    explicit BinaryToSource(const Settings &settings)
        : settings_(settings), disassembler_(NULL) {}

    /** Command-line switch parsing. */
    static Sawyer::CommandLine::SwitchGroup commandLineSwitches(Settings&);

    /** Initialize diagnostic streams.
     *
     *  This is called automatically by @ref Rose::Diagnostics::initialize. */
    static void initDiagnostics();

    /** Property: Configuration settings.
     *
     *  This property is read-only. The settings must be specified in the constructor. */
    const Settings& settings() const { return settings_; }

    /** Generate source code as text.
     *
     *  Emits C source code to the specified output stream.  The output will be one C compilation unit that represents the
     *  entire binary specimen contained in the partitioner. This method takes a partitioner rather than a binary AST because
     *  the partitioner's data structures are more efficient for anaysis. A partitioner can be constructed from an AST if
     *  necessary. */
    void generateSource(const Partitioner2::Partitioner&, std::ostream&);

#if 0 // [Robb P. Matzke 2015-10-06]: not implemented yet
    /** Build a C source AST from a binary AST.
     *
     *  Given a disassembled and partitioned binary, create a corresponding C source code AST.  This method takes a partitioner
     *  rather than a binary AST because the partitioner's data structures are more efficient for anaysis. A partitioner can be
     *  constructed from an AST if necessary.  */
    void generateSource(const Partitioner2::Partitioner&, SgSourceFile *sourceAst);
#endif

private:
    // (Re)initialize the instruction semantics machinery so it is suitable for this architecture.
    void init(const Partitioner2::Partitioner&);

    // Emit the file prologue, #include statements, extern declarations, etc.
    void emitFilePrologue(const Partitioner2::Partitioner&, std::ostream&);

    // Declare the global register variables
    void declareGlobalRegisters(std::ostream&);

    // Define interrupt handlers
    void defineInterrupts(std::ostream&);

    // Emit accumulated side effects and/or SSA. */
    void emitEffects(std::ostream&);

    // Emit code for one instruction
    void emitInstruction(SgAsmInstruction*, std::ostream&);

    // Emit code for one basic block
    void emitBasicBlock(const Partitioner2::Partitioner&, const Partitioner2::BasicBlock::Ptr&, std::ostream&);

    // Emit code for one function
    void emitFunction(const Partitioner2::Partitioner&, const Partitioner2::Function::Ptr&, std::ostream&);

    // Emit code for all functions
    void emitAllFunctions(const Partitioner2::Partitioner&, std::ostream&);

    // Emit the function_call dispatcher that decides which C function should be called next.
    void emitFunctionDispatcher(const Partitioner2::Partitioner&, std::ostream&);

    // Emit the initialize_memory function.
    void emitMemoryInitialization(const Partitioner2::Partitioner&, std::ostream&);

    // Emit the "main" function.
    void emitMain(const Partitioner2::Partitioner&, std::ostream&);
};

} // namespace
} // namespace

#endif
