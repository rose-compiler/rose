/******************************************************************************************************************************
 * NOTE:  For any given IR class, please keep all its parts as close together as possible.  Its bad enough that we're
 *        defining classes in a non-C-like language -- and it's painful when a definition is scattered all over this file. We
 *        already have four places to look to get the definition of an IR class:
 *              (1) here, in this source file, built with ROSETTA functions
 *              (2) additional declarations between HEADER_* markers in ../Grammar/BinaryInstruction.code (which we can't
 *                  even parse as a C++ file, making IDEs useless there).
 *              (3) function definitions between SOURCE_* markers in ../Grammar/BinaryInstruction.code, which for some
 *                  reason are not next to the corresponding HEADER_ markers.  Please migrate these definitions to *.C files
 *                  whenever you have an opportunity, so that IDEs can see the definitions.
 *              (4) Doxygen documentation in ../../../docs/testDoxygen directory since doxygen can't parse the IR classes
 *                  from this source file.  Documented entities are marked with a "DOXYGEN" comment here to make it more
 *                  obvious when documentation is present.
 *
 * NOTE:  First part of the file is for classes describing instructions.  Middle part is for classes describing binary
 *        containers (organized by container type: ELF, DOS, PE, ...)  Last part is the base classes.  Unlike C++, the classes
 *        defined in this file need to be defined from the most specific (derived classes) to the most general (base class).
 *
 * NOTE:  Please use three blank lines between IR node definitions to help make this file more readable.  Unless those IR
 *        nodes are so closely related to one another that it's better to keep them close.
 *
 * UPDATE: Instead of splitting class declarations into four separate places, we can now use the macros defined below to put
 *        everything into one place in this file.  My goal is to eventually remove the
 *        src/ROSETTA/Grammar/BinaryInstruction.code file and the related docs/testDoxygen/xxx.docs files and to add
 *        documentation here for all property accessors.
 *
 * ROSETTA FAILURE MODES:
 *
 * + If you get thousands of compile errors in Cxx_Grammar.h that seem to have absolutely nothing to do with the node
 *   you just added, then double check that the new node type is listed as the descendant of some other node type in a
 *   NEW_NONTERMINAL_MACRO macro expansion.
 *
 * + If CxxGrammarMetaProgram uses gigabytes and gigabytes of memory and never terminates then check that all the new
 *   IR node types have a correctly spelled entry in the astNodeList file.
 *-----------------------------------------------------------------------------------------------------------------------------*/

#include "ROSETTA_macros.h"
#include "grammar.h"
#include "AstNodeClass.h"

// The following macros try to make it possile to keep all aspects of a Sage node's declaration in this one file. The old way
// of doing things required the class declarations to be split into four places:
//   (1) The #include files necessary to declare the class were at the top of a src/ROSETTA/Grammar/*.code file
//   (2) The class declaration and properties were built in src/ROSETTA/src/*.C
//   (3) The documentation for the class and its properties were in docs/testDoxygen/*.docs files
//   (4) Additional non-ROSETTA members were in src/ROSETTA/Grammar/*.code files
// Implementations were originally also in the src/ROSETTA/Grammar/*.code files but have since been moved to *.C files.  The
// problem with *.code files is no IDE understands them.
//
// The following macros take a CLASS_WITHOUT_Sg parameter, which is the name of the Sage node class but without the leading
// "Sg". I'm not sure why ROSETTA was written this way. For instance, use "AsmInstruction" instead of "SgAsmInstruction". Most
// other things will use the full class name.
//
// The macros are:
//   DOCUMENTATION is never defined when compiling, but will be defined when generating documentation and can be used to help
//   IDE's figure out the indentation and as commentary. We don't use "#if 0" because some IDEs figure out that the code is
//   never possible and don't indent it properly. For instance, most of the classes are defined like this:
//       #ifdef DOCUMENTATION
//       class SgAsmArmInstruction: public SgAsmInstruction {
//       #endif
//
//       ...
//
//       #ifdef DOCUMENTATION
//       };
//       #endif
//
//
//   DECLARE_LEAF_CLASS is the simpler way to declare a Sage class that has no subclasses.  This must be the first macro
//   invoked when starting a new class declaration. Example, to declare the SgMyClass node, say:
//       DECLARE_LEAF_CLASS(MyClass)
//       #ifdef DOCUMENTATION
//       class SgMyClass: public ...base-classes... {
//       #endif
//
//   
//   DECLARE_HEADERS is used to indicate what header files need to be included. Note that due to limitations of ROSETTA
//   (specifically, not having any portable regular expression library due to prohibition against using boost), the #ifdef and
//   #endif lines must be *exactly* as written here -- they are sensitive to white space.
//       DECLARE_HEADERS(MyClass)
//       #if defined(SgMyClass_HEADERS) || defined(DOCUMENTATION)
//       #include <someHeader>
//       #endif // SgMyClass_HEADERS
//
//
//   DECLARE_OTHERS is for declaring other class members that don't need to be processed by ROSETTA. Due to limitations of
//   ROSETTA (specifically, not having any portable regular expression library due to prohibition against using boost), the
//   #ifdef and #endif lines must be *exactly* as written here -- they are sensitive to white space.
//       DECLARE_OTHERS(MyClass)
//       #if defined(SgMyClass_OTHERS) || defined(DOCUMENTATION)
//       // other declarations here
//       #endif // SgMyClass_OTHERS

//#undef DOCUMENTATION -- commented out so IDEs can't figure it out
#ifdef DOCUMENTATION
DOCUMENTATION_should_never_be_defined;
#endif

#ifdef DOCUMENTATION
#define DECLARE_LEAF_CLASS(CLASS_WITHOUT_Sg) /*void*/
#else
#define DECLARE_LEAF_CLASS(CLASS_WITHOUT_Sg) \
    NEW_TERMINAL_MACRO(CLASS_WITHOUT_Sg, #CLASS_WITHOUT_Sg, #CLASS_WITHOUT_Sg "Tag"); \
    CLASS_WITHOUT_Sg.setCppCondition("!defined(DOCUMENTATION)")
#endif

#ifdef DOCUMENTATION
#define DECLARE_HEADERS(CLASS_WITHOUT_Sg) /*void*/
#else
#define DECLARE_HEADERS(CLASS_WITHOUT_Sg) \
    CLASS_WITHOUT_Sg.setPredeclarationString("Sg" #CLASS_WITHOUT_Sg "_HEADERS", __FILE__)
#endif

#ifdef DOCUMENTATION
#define DECLARE_OTHERS(CLASS_WITHOUT_Sg) /*void*/
#else
#define DECLARE_OTHERS(CLASS_WITHOUT_Sg) \
    CLASS_WITHOUT_Sg.setFunctionPrototype("Sg" #CLASS_WITHOUT_Sg "_OTHERS", __FILE__)
#endif

// Since ROSETTA builds classes from the leaves up to the base, and C++ builds classes from the base down to the leaves, we
// need to make sure that doxygen sees the base classes before the derived classes. So just list all the non-leaf classes here.
#ifdef DOCUMENTATION
class SgAsmBinaryExpression;
class SgAsmConstantExpression;
class SgAsmExpression;
class SgAsmInstruction;
class SgAsmNode;
class SgAsmRegisterReferenceExpression;
class SgAsmScalarType;
class SgAsmStatement;
class SgAsmType;
class SgAsmUnaryExpression;
class SgAsmValueExpression;
#endif

#ifndef DOCUMENTATION
void Grammar::setUpBinaryInstructions() {
#endif
    
    /**************************************************************************************************************************
     *                                  Instructions.
     * Base class (SgAsmInstruction) and various subclasses, one per architecture.
     **************************************************************************************************************************/

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmArmInstruction);

    DECLARE_HEADERS(AsmArmInstruction);
#if defined(SgAsmArmInstruction_HEADERS) || defined(DOCUMENTATION)
    #include <armInstructionEnum.h>
#endif // SgAsmArmInstruction_HEADERS

#ifdef DOCUMENTATION
    /** Represents one ARM machine instruction. */
    class SgAsmArmInstruction: public SgAsmInstruction {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Instruction kind.
         *
         *  Every instruction of every architecture has a @c kind property which is an enum for the particular kind of
         *  instruction, similar to the mnemonic for the instruction.
         *
         * @{ */
        ArmInstructionKind get_kind() const;
        void set_kind(ArmInstructionKind);
        /** @} */
#else
        AsmArmInstruction.setDataPrototype("ArmInstructionKind", "kind", "= arm_unknown_instruction",
                                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
#endif

#ifdef DOCUMENTATION
        /** Property: Arm instruction condition.
         *
         *  @{ */
        ArmInstructionCondition get_condition() const;
        void set_condition(ArmInstructionCondition);
        /** @} */
#else
        AsmArmInstruction.setDataPrototype("ArmInstructionCondition", "condition", "= arm_cond_unknown",
                                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
#endif

#ifdef DOCUMENTATION
        /** Property: Bit position of condition bits in instruction menmonic.
         *
         * @{ */
        int get_positionOfConditionInMnemonic() const;
        void set_positionOfConditionInMnemonic(int);
        /** @} */
#else
        AsmArmInstruction.setDataPrototype("int", "positionOfConditionInMnemonic", "= -1",
                                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
#endif

        DECLARE_OTHERS(AsmArmInstruction);
#if defined(SgAsmArmInstruction_OTHERS) || defined(DOCUMENTATION)
    public:
        virtual bool terminatesBasicBlock() ROSE_OVERRIDE;
        virtual std::set<rose_addr_t> getSuccessors(bool* complete) ROSE_OVERRIDE;
        virtual bool isUnknown() const ROSE_OVERRIDE;
        virtual unsigned get_anyKind() const ROSE_OVERRIDE;
#endif // SgAsmArmInstruction_OTHERS
#ifdef DOCUMENTATION
    };
#endif


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmX86Instruction);
    DECLARE_HEADERS(AsmX86Instruction);
#if defined(SgAsmX86Instruction_HEADERS) || defined(DOCUMENTATION)
    #include <InstructionEnumsX86.h>
    class RegisterDictionary;
#endif // SgAsmX86Instruction_HEADERS

#ifdef DOCUMENTATION
    /** Represents one Intel x86 machine instruction. */
    class SgAsmX86Instruction: public SgAsmInstruction {
    public:
#endif

#ifndef DOCUMENTATION
        AsmX86Instruction.setDataPrototype("X86InstructionKind", "kind", "= x86_unknown_instruction",
                                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
        AsmX86Instruction.setDataPrototype("X86InstructionSize", "baseSize", "= x86_insnsize_none",
                                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
        AsmX86Instruction.setDataPrototype("X86InstructionSize", "operandSize", "= x86_insnsize_none",
                                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
        AsmX86Instruction.setDataPrototype("X86InstructionSize", "addressSize", "= x86_insnsize_none",
                                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
        AsmX86Instruction.setDataPrototype("bool", "lockPrefix", "= false",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
        AsmX86Instruction.setDataPrototype("X86RepeatPrefix", "repeatPrefix", "= x86_repeat_none",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
        AsmX86Instruction.setDataPrototype("X86BranchPrediction", "branchPrediction", "= x86_branch_prediction_none",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
        AsmX86Instruction.setDataPrototype("X86SegmentRegister", "segmentOverride", "= x86_segreg_none",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
#endif


        DECLARE_OTHERS(AsmX86Instruction);
#if defined(SgAsmX86Instruction_OTHERS) || defined(DOCUMENTATION)
    public:
        static X86InstructionSize instructionSizeForWidth(size_t);
        static size_t widthForInstructionSize(X86InstructionSize);
        static const RegisterDictionary* registersForInstructionSize(X86InstructionSize);
        static const RegisterDictionary* registersForWidth(size_t);

        virtual bool terminatesBasicBlock() ROSE_OVERRIDE;
        virtual bool isFunctionCallFast(const std::vector<SgAsmInstruction*>&,
                                        rose_addr_t *target, rose_addr_t *ret) ROSE_OVERRIDE;
        virtual bool isFunctionCallSlow(const std::vector<SgAsmInstruction*>&,
                                        rose_addr_t *target, rose_addr_t *ret) ROSE_OVERRIDE;
        virtual bool isFunctionReturnFast(const std::vector<SgAsmInstruction*>&) ROSE_OVERRIDE;
        virtual bool isFunctionReturnSlow(const std::vector<SgAsmInstruction*>&) ROSE_OVERRIDE;
        virtual bool getBranchTarget(rose_addr_t *target/*out*/) ROSE_OVERRIDE;
        virtual std::set<rose_addr_t> getSuccessors(bool* complete) ROSE_OVERRIDE;
        virtual std::set<rose_addr_t> getSuccessors(const std::vector<SgAsmInstruction*>&,
                                                    bool* complete,
                                                    const MemoryMap *initial_memory=NULL) ROSE_OVERRIDE;

        /** Determines whether this instruction is the special x86 "unknown" instruction. */
        virtual bool isUnknown() const;

        virtual unsigned get_anyKind() const;
#endif // SgAsmX86Instruction_OTHERS
#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmPowerpcInstruction);
    DECLARE_HEADERS(AsmPowerpcInstruction);
#if defined(SgAsmPowerpcInstruction_HEADERS) || defined(DOCUMENTATION)
    #include <powerpcInstructionEnum.h>
#endif // SgAsmPowerpcInstruction_HEADERS

#ifdef DOCUMENTATION
    /** Represents one PowerPC machine instruction. */
    class SgAsmPowerpcInstruction: public SgAsmInstruction {
    public:
#endif


#ifndef DOCUMENTATION
        AsmPowerpcInstruction.setDataPrototype("PowerpcInstructionKind", "kind", "= powerpc_unknown_instruction",
                                               CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE,
                                               COPY_DATA);
#endif

        DECLARE_OTHERS(AsmPowerpcInstruction);
#if defined(SgAsmPowerpcInstruction_OTHERS) || defined(DOCUMENTATION)
    public:
        virtual bool terminatesBasicBlock() ROSE_OVERRIDE;
        virtual std::set<rose_addr_t> getSuccessors(bool* complete) ROSE_OVERRIDE;
        virtual bool isUnknown() const ROSE_OVERRIDE;
        virtual unsigned get_anyKind() const;
#endif // SgAsmPowerpcInstruction_OTHERS
#ifdef DOCUMENTATION
    };
#endif


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmMipsInstruction);
    DECLARE_HEADERS(AsmMipsInstruction);
#if defined(SgAsmMipsInstruction_HEADERS) || defined(DOCUMENTATION)
    #include <InstructionEnumsMips.h>
#endif // SgAsmMipsInstruction_HEADERS

#ifdef DOCUMENTATION
    /** Represents one MIPS machine instruction. */
    class SgAsmMipsInstruction: public SgAsmInstruction {
    public:
#endif

#ifndef DOCUMENTATION
        AsmMipsInstruction.setDataPrototype("MipsInstructionKind", "kind", "= mips_unknown_instruction",
                                            CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE,
                                            COPY_DATA);
#endif

        DECLARE_OTHERS(AsmMipsInstruction);
#if defined(SgAsmMipsInstruction_OTHERS) || defined(DOCUMENTATION)
    public:
        virtual bool terminatesBasicBlock() ROSE_OVERRIDE;
        virtual bool isFunctionCallFast(const std::vector<SgAsmInstruction*> &insns,
                                        rose_addr_t *target/*out*/, rose_addr_t *ret/*out*/) ROSE_OVERRIDE;
        virtual bool isFunctionCallSlow(const std::vector<SgAsmInstruction*>&,
                                        rose_addr_t *target, rose_addr_t *ret) ROSE_OVERRIDE;
        virtual bool isFunctionReturnFast(const std::vector<SgAsmInstruction*> &insns) ROSE_OVERRIDE;
        virtual bool isFunctionReturnSlow(const std::vector<SgAsmInstruction*> &insns) ROSE_OVERRIDE;
        virtual std::set<rose_addr_t> getSuccessors(bool* complete) ROSE_OVERRIDE;
        virtual bool isUnknown() const ROSE_OVERRIDE;
        virtual bool getBranchTarget(rose_addr_t *target) ROSE_OVERRIDE;
        virtual unsigned get_anyKind() const ROSE_OVERRIDE;
#endif // SgAsmMipsInstruction_OTHERS
#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmM68kInstruction);
    DECLARE_HEADERS(AsmM68kInstruction);
#if defined(SgAsmM68kInstruction_HEADERS) || defined(DOCUMENTATION)
    #include "InstructionEnumsM68k.h"
#endif // SgAsmM68kInstruction_HEADERS

#ifdef DOCUMENTATION
    class SgAsmM68kInstruction: public SgAsmInstruction {
    public:
#endif

#ifndef DOCUMENTATION
        AsmM68kInstruction.setDataPrototype("M68kInstructionKind", "kind", " = m68k_unknown_instruction",
                                            CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE,
                                            COPY_DATA);
#endif

        DECLARE_OTHERS(AsmM68kInstruction);
#if defined(SgAsmM68kInstruction_OTHERS) || defined(DOCUMENTATION)
    public:
        virtual bool terminatesBasicBlock() ROSE_OVERRIDE;
        virtual bool isFunctionCallFast(const std::vector<SgAsmInstruction*> &insns,
                                        rose_addr_t *target/*out*/, rose_addr_t *ret/*out*/) ROSE_OVERRIDE;
        virtual bool isFunctionCallSlow(const std::vector<SgAsmInstruction*>&,
                                        rose_addr_t *target, rose_addr_t *ret) ROSE_OVERRIDE;
        virtual bool isFunctionReturnFast(const std::vector<SgAsmInstruction*> &insns) ROSE_OVERRIDE;
        virtual bool isFunctionReturnSlow(const std::vector<SgAsmInstruction*> &insns) ROSE_OVERRIDE;
        virtual bool getBranchTarget(rose_addr_t *target) ROSE_OVERRIDE;
        virtual std::set<rose_addr_t> getSuccessors(bool* complete) ROSE_OVERRIDE;
        virtual std::set<rose_addr_t> getSuccessors(const std::vector<SgAsmInstruction*>&,
                                                    bool* complete,
                                                    const MemoryMap *initial_memory=NULL) ROSE_OVERRIDE;
        virtual bool isUnknown() const ROSE_OVERRIDE;
        virtual unsigned get_anyKind() const ROSE_OVERRIDE;
#endif // SgAsmM68kInstruction_OTHERS
#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NEW_NONTERMINAL_MACRO(AsmInstruction,
                          AsmX86Instruction | AsmArmInstruction | AsmPowerpcInstruction | AsmMipsInstruction |
                          AsmM68kInstruction,
                          "AsmInstruction", "AsmInstructionTag", true);

    DECLARE_HEADERS(AsmInstruction);
#if defined(SgAsmInstruction_HEADERS) || defined(DOCUMENTATION)
    #include <MemoryMap.h>
#endif // SgAsmInstruction_HEADERS

#ifdef DOCUMENTATION
    class SgAsmInstruction: public SgAsmStatement {
    public:
#endif

#ifndef DOCUMENTATION
        AsmInstruction.setDataPrototype("std::string", "mnemonic", "= \"\"",
                                        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
        AsmInstruction.setDataPrototype("SgUnsignedCharList", "raw_bytes", "",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
        AsmInstruction.setDataPrototype("SgAsmOperandList*", "operandList", "= NULL",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, COPY_DATA);
        AsmInstruction.setDataPrototype("SgAsmStatementPtrList", "sources", "",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Stack pointer at start of instruction relative to start of instruction's function.
         *
         *  @{ */
        int64_t get_stackDeltaIn() const;
        void set_stackDeltaIn(int64_t);
        /** @} */
#else
        AsmInstruction.setDataPrototype("int64_t", "stackDeltaIn", "= SgAsmInstruction::INVALID_STACK_DELTA",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifndef DOCUMENTATION
        AsmInstruction.setDataPrototype("SgAsmExprListExp*", "semantics", "= NULL",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, COPY_DATA);
#endif

        DECLARE_OTHERS(AsmInstruction);
#if defined(SgAsmInstruction_OTHERS) || defined(DOCUMENTATION)
    public:
        static const int64_t INVALID_STACK_DELTA;

        SgAsmInstruction* cfgBinFlowOutEdge(const VirtualBinCFG::AuxiliaryInformation* info);
        std::vector<VirtualBinCFG::CFGEdge> cfgBinOutEdges(const VirtualBinCFG::AuxiliaryInformation* info);
        std::vector<VirtualBinCFG::CFGEdge> cfgBinInEdges(const VirtualBinCFG::AuxiliaryInformation* info);
        void appendSources( SgAsmInstruction* instruction );

        /** Determines if this instruction normally terminates a basic block.
         *
         *  The analysis only looks at the individual instruction and therefore is not very sophisticated.  For instance, a
         *  conditional branch will always terminate a basic block by this method even if its condition is opaque.  The base
         *  class implementation always aborts; architecture-specific subclasses should override this to do something useful
         *  (pure virtual is not possible due to ROSETTA). */
        virtual bool terminatesBasicBlock();

        /** Returns true if the specified basic block looks like a function call.
         *
         *  This instruction object is only used to select the appropriate virtual method; the basic block to be analyzed is
         *  the first argument to the function.  If the basic block looks like a function call then this method returns true.
         *  If (and only if) the target address is known (i.e., the address of the called function) then @p target is set to
         *  this address (otherwise @p target is unmodified). If the return address is known or can be guessed, then return_va
         *  is initialized to the return address, which is normally the fall-through address of the last instruction; otherwise
         *  the return_va is unmodified.
         *
         *  The "fast" and "slow" versions differ only in what kind of anlysis they do.  The "fast" version typically looks
         *  only at instruction patterns while the slow version might incur more expense by looking at instruction semantics.
         *
         * @{ */
        virtual bool isFunctionCallFast(const std::vector<SgAsmInstruction*>&, rose_addr_t *target, rose_addr_t *ret);
        virtual bool isFunctionCallSlow(const std::vector<SgAsmInstruction*>&, rose_addr_t *target, rose_addr_t *ret);
        /** @} */

        /** Returns true if the specified basic block looks like a function return.
         *
         *  This instruction object is only used to select the appropriate virtual method; the basic block to be analyzed is
         *  the first argument to the function.
         *  
         *  The "fast" and "slow" versions differ only in what kind of anlysis they do.  The "fast" version typically looks
         *  only at instruction patterns while the slow version might incur more expense by looking at instruction semantics.
         *
         * @{ */
        virtual bool isFunctionReturnFast(const std::vector<SgAsmInstruction*>&);
        virtual bool isFunctionReturnSlow(const std::vector<SgAsmInstruction*>&);
        /** @} */

        /** Returns true if this instruction is the first instruction in a basic block.
         *
         *  This method looks only at the AST to make this determination. */
        bool isFirstInBlock();

        /** Returns true if this instruction is the last instruction in a basic block.
         *
         *  This method looks only at the AST to make this determination. */
        bool isLastInBlock();

        /** Obtains the virtual address for a branching instruction.
         *
         *  Returns true if this instruction is a branching instruction and the target address is known; otherwise, returns
         *  false and @p target is not modified. */
        virtual bool getBranchTarget(rose_addr_t *target/*out*/);

        /** Determines whether a single instruction has an effect.
         *
         *  An instruction has an effect if it does anything other than setting the instruction pointer to a concrete
         *  value. Instructions that have no effect are called "no-ops".  The x86 NOP instruction is an example of a no-op, but
         *  there are others also.
         *
         *  The following information about x86 no-ops is largely from Cory Cohen at CMU/SEI. In the discussion that follows,
         *  we are careful to distinguish between NOP (the mneumonic for instructions 90, and 0f1f) and "no-op" (any
         *  instruction whose only effect is to advance the instruction pointer).
         *
         * @code
         *  Opcode bytes         Intel assembly syntax
         *  -------------------- ---------------------- 
         *  90                   nop
         *
         *  89c0                 mov eax,eax            Intel's old recommended two-byte no-op was to
         *  89c9                 mov ecx,ecx            move a register to itself...  The second byte of these are mod/rm
         *  89d2                 mov edx,edx            bytes, and can generally be substituded wherever you see 0xc0 in
         *  89db                 mov ebx,ebx            subsequent examples.
         *  89e4                 mov esp,esp
         *  89ed                 mov ebp,ebp
         *  89f6                 mov esi,esi
         *  89ff                 mov edi,edi
         *
         *  88c0                 mov al,al              The above are also available in 8-bit form with a leading byte of 0x88
         *  6689c0               mov ax,ax              and with an operand size prefix (0x66).
         *
         *  66666689c0           mov ax,ax              The prefixes can be repeated. One source seemed to imply that up to
         *                                              three are reliably supported by the actual Intel processors. ROSE
         *                                              supports any number up to the maximum instruction size (varies by mode).
         *
         *  6688c0               mov al,al              The operand size prefix can even be nonsensical.
         *
         *  8ac0                 mov al,al              These are also presumabely no-ops.  As with most instructions, these
         *  8bc0                 mov eax,eax            will accept operand size prefixes as well.
         *
         *  f090                 lock nop               Most of these instructions will accept a lock prefix as well, which does
         *  f0f090               lock nop               not materially affect the result. As before, they can occur repeatedly,
         *  f066f090             lock nop               and even in wacky combinations.
         *  f066f06666f0f066f090 lock nop
         *  
         *  f290                 repne nop              Cory Cohen strongly suspects that the other instruction prefixes are
         *  f390                 rep nop                ignored as well, although to be complete, we might want to conduct a
         *  2690                 es nop                 few tests into the behavior of common processors.
         *  2e90                 cs nop
         *  3690                 ss nop
         *  3e90                 ds nop
         *  6490                 fs nop
         *  6590                 gs nop
         *  6790                 nop
         *  
         *  8d00                 lea eax,[eax]          Intel's old recommendation for larger no-ops was to use the LEA
         *  8d09                 lea ecx,[ecx]          instruction in various dereferencing modes.
         *  8d12                 lea edx,[edx]
         *  8d1b                 lea ebx,[ebx]
         *  8d36                 lea esi,[esi]
         *  8d3f                 lea edi,[edi]
         *  
         *  8d4000               lea eax,[eax+0x0]
         *  8d4900               lea ecx,[ecx+0x0]
         *  8d5200               lea edx,[edx+0x0]
         *  8d5b00               lea ebx,[ebx+0x0]
         *  8d7600               lea esi,[esi+0x0]
         *  8d7f00               lea edi,[edi+0x0]
         *  
         *  8d8000000000         lea eax,[eax+0x0]      This last block is really the [reg*0x1+0x0] dereferencing mode.
         *  8d8900000000         lea ecx,[ecx+0x0]
         *  8d9200000000         lea edx,[edx+0x0]
         *  8d9b00000000         lea ebx,[ebx+0x0]
         *  8db600000000         lea esi,[esi+0x0]
         *  8dbf00000000         lea edi,[edi+0x0]
         *
         *  8d0420               lea eax,[eax]          Then there's funky equivalents involving SIB bytes.
         *  8d0c21               lea ecx,[ecx]
         *  8d1422               lea edx,[edx]
         *  8d1c23               lea ebx,[ebx]
         *  8d2424               lea esp,[esp]
         *  8d3426               lea esi,[esi]
         *  8d3c27               lea edi,[edi]
         *  
         *  8d442000             lea eax,[eax+0x0]
         *  8d4c2100             lea ecx,[ecx+0x0]
         *  8d542200             lea edx,[edx+0x0]
         *  8d5c2300             lea ebx,[ebx+0x0]
         *  8d642400             lea esp,[esp+0x0]
         *  8d742600             lea esi,[esi+0x0]
         *  8d7c2700             lea edi,[edi+0x0]
         *  
         *  8d842000000000       lea eax,[eax+0x0]
         *  8d8c2100000000       lea ecx,[ecx+0x0]
         *  8d942200000000       lea edx,[edx+0x0]
         *  8d9c2300000000       lea ebx,[ebx+0x0]
         *  8da42400000000       lea esp,[esp+0x0]
         *  8db42600000000       lea esi,[esi+0x0]
         *  8dbc2700000000       lea edi,[edi+0x0]
         *  
         *  8d2c2d00000000       lea ebp,[ebp+0x0]      The EBP variants don't exactly follow the pattern above.
         *  8d6c2500             lea ebp,[ebp+0x0]
         *  8dac2500000000       lea ebp,[ebp+0x0]
         *
         *  0f1f00               nop [eax]              P4+ adds the 0f1f instruction. Each of these can be prefixed with the
         *  0f1f4000             nop [eax+0x0]          0x66 operand size prefix. In fact, Intel recommends doing this now
         *  0f1f440000           nop [eax+0x0]          for the optimally efficient 6- and 9-byte sequences.
         *  0f1f8000000000       nop [eax+0x0]
         *  0f1f840000000000     nop [eax+0x0]
         *
         *  0f0dxx               nop [xxx]              The latest version of the manual implies that this sequence is also
         *                                              reserved for NOP, although I can find almost no references to it except
         *                                              in the latest instruction manual on page A-13 of volume 2B. It's also
         *                                              mentioned on x86asm.net. [CORY 2010-04]
         *                                              
         *  d9d0                 fnop                   These aren't really no-ops on the chip, but are no-ops from the
         *  9b                   wait                   program's perspective. Most of these instructions are related to
         *  0f08                 invd                   improving cache efficiency and performance, but otherwise do not
         *  0f09                 wbinvd                 affect the program behavior.
         *  0f01c9               mwait
         *  0f0138               invlpg [eax]
         *  0f01bf00000000       invlpg [edi+0x0]       and more...
         *  0f18 /0              prefetchnta [xxx]
         *  0f18 /1              prefetch0 [xxx]
         *  0f18 /2              prefetch1 [xxx]
         *  0f18 /3              prefetch2 [xxx]
         *  0fae /5              lfence [xxx]
         *  0fae /6              mfence [xxx]
         *  0fae /7              sfence [xxx]
         *
         *  0f18xx through 0f1exx                       This opcode rante is officially undefined but is probably reserved
         *                                              for no-ops as well.  Any instructions encountered in this range are
         *                                              probably consequences of bad code and should be ingored.
         *                                              
         *  JMP, Jcc, PUSH/RET, etc.                    Branches are considered no-ops if they can be proven to always branch
         *                                              to the fall-through address.
         * @endcode
         */
        virtual bool hasEffect();

        /** Determine if an instruction sequence has an effect.
         *
         *  A sequence of instructions has an effect if it does something other than setting the instruction pointer to a
         *  concrete value.
         *
         *  This is mostly a wrapper around the @ref rose::BinaryAnalysis::NoOperation "NoOperation" analysis. The @p
         *  allow_branch and @p relax_stack_semantics are no longer supported but perhaps will be added eventually to the
         *  NoOperation analysis. */
        virtual bool hasEffect(const std::vector<SgAsmInstruction*>&, bool allow_branch=false,
                               bool relax_stack_semantics=false);

        /** Determines what subsequences of an instruction sequence have no cumulative effect.
         *
         *  The return value is a vector of pairs where each pair is the starting index and length of subsequence.  The
         *  algorithm we use is to compute the machine state after each instruction and then look for pairs of states that are
         *  identical except for the instruction pointer.
         *
         *  This is mostly a wrapper around the @ref rose::BinaryAnalysis::NoOperation "NoOperation" analysis. The @p
         *  allow_branch and @p relax_stack_semantics are no longer supported but perhaps will be added eventually to the
         *  NoOperation analysis. */
        virtual std::vector<std::pair<size_t,size_t> >
        findNoopSubsequences(const std::vector<SgAsmInstruction*>& insns, bool allow_branch=false,
                             bool relax_stack_semantics=false);

        /** Control flow successors for a single instruction.
         *
         *  The return value does not consider neighboring instructions, and therefore is quite naive.  It returns only the
         *  information it can glean from this single instruction.  If the returned set of virtual instructions is fully known
         *  then the @p complete argument will be set to true, otherwise false.  The base class implementation always
         *  aborts()--it must be defined in an architecture-specific subclass (pure virtual is not possible due to ROSETTA). */
        virtual std::set<rose_addr_t> getSuccessors(bool* complete); /*subclasses must redefine*/

        /** Control flow successors for a basic block.
         *
         *  The @p basicBlock argument is a vector of instructions that is assumed to be a basic block that is entered only at
         *  the first instruction and exits only at the last instruction.  A memory map can supply initial values for the
         *  analysis' memory state.  The return value is a set of control flow successor virtual addresses, and the @p complete
         *  argument return value indicates whether the returned set is known to be complete (aside from interrupts, faults,
         *  etc).  The base class implementation just calls the single-instruction version, so architecture-specific subclasses
         *  might want to override this to do something more sophisticated. */
        virtual std::set<rose_addr_t> getSuccessors(const std::vector<SgAsmInstruction*> &basicBlock,
                                                    bool *complete,
                                                    const MemoryMap *initial_memory=NULL);

        /** Returns the size of an instruction in bytes.
         *
         *  This is only a convenience function that returns the size of the instruction's raw byte vector.  If an instruction
         *  or its arguments are modified, then the size returned by this function might not reflect the true size of the
         *  modified instruction if it were to be reassembled. */
        virtual size_t get_size() const;

        /** Returns true if this instruction is the special "unknown" instruction.
         *
         *  Each instruction architecture in ROSE defines an "unknown" instruction to be used when the disassembler is unable
         *  to create a real instruction.  This can happen, for instance, if the bit pattern does not represent a valid
         *  instruction for the architecture. */
        virtual bool isUnknown() const;

        /** Returns instruction kind for any architecture.
         *
         *  Instruction kinds are specific to the architecture so it doesn't make sense to compare an instruction kind from x86
         *  with an instruction kind from m68k.  However, this virtual function exists so that we don't need to implement
         *  switch statements every time we want to compare two instructions from the same architecture.  For instance, instead
         *  of code like this:
         *
         * @code
         *  bool areSame(SgAsmInstruction *a, SgAsmInstruction *b) {
         *      if (a->variantT() != b->variantT())
         *          return false;
         *      if (SgAsmM68kInstruction *aa = isSgAsmM68kInstruction(a)) {
         *          SgAsmM68kInstruction *bb = isSgAsmM68kInstruction(b);
         *          return aa->get_kind() == bb->get_kind();
         *      }
         *      if (SgAsmMipsInstruction *aa = isSgAsmMipsInstruction(a)) {
         *          SgAsmMipsInstruction *bb = isSgAsmMipsInstruction(b);
         *          return aa->get_kind() == bb->get_kind();
         *      }
         *      ...
         *      ... // and many others
         *      ...
         *      ASSERT_not_reachable("architecture is not implemented yet");
         *  }
         * @endcode
         *
         *  we can write future-proof code:
         *
         * @code
         *  bool areSame(SgAsmInstruction *a, SgAsmInstruction *b) {
         *      return a->variantT()==b->variantT() && a->get_anyKind()==b->get_anyKind();
         *  }
         * @endcode */
        virtual unsigned get_anyKind() const;
#endif // SgAsmInstruction_OTHERS

#ifdef DOCUMENTATION
    };
#endif





    /**************************************************************************************************************************
     *                                  Instruction Expressions
     * Related functions and documentation can be found in src/frontend/Disassemblers/Expressions.C
     **************************************************************************************************************************/



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmOperandList);

#ifdef DOCUMENTATION
    /** List of operands for an instruction. */
    class SgAsmOperandList: public SgAsmNode {
    public:
#endif
        
#ifndef DOCUMENTATION
        AsmOperandList.setDataPrototype("SgAsmExpressionPtrList", "operands", "",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmOperandList);
#if defined(SgAsmOperandList_OTHERS) || defined(DOCUMENTATION)
    public:
        void append_operand(SgAsmExpression* operand);
#endif // SgAsmOperandList_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    // FIXME[Robb P Matzke 2016-10-31]
    AsmOperandList.setFunctionSource("SOURCE_BINARY_OPERAND_LIST", "../Grammar/BinaryInstruction.code");

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmBinaryAdd);

#ifdef DOCUMENTATION
    /** Expression that adds two operands. */
    class SgAsmBinaryAdd: public SgAsmBinaryExpression {};
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmBinarySubtract);
    
#ifdef DOCUMENTATION
    /** Expression that subtracts the second operand from the first. */
    class SgAsmBinarySubtract: public SgAsmBinaryExpression {};
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmBinaryMultiply);

#ifdef DOCUMENTATION
    /** Expression that multiplies two operands. */
    class SgAsmBinaryMultiply: public SgAsmBinaryExpression {};
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmBinaryDivide);


#ifdef DOCUMENTATION
    /** Expression that divides the first operand by the second. */
    class SgAsmBinaryDivide: public SgAsmBinaryExpression {};
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    DECLARE_LEAF_CLASS(AsmBinaryMod);

#ifdef DOCUMENTATION
    /** Expression that returns the remainder when dividing the first operand by the second. */
    class SgAsmBinaryMod: public SgAsmBinaryExpression {};
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmBinaryAddPreupdate);

#ifdef DOCUMENTATION
    /** Expression that performs a pre-increment operation. */
    class SgAsmBinaryAddPreupdate: public SgAsmBinaryExpression {};
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmBinarySubtractPreupdate);

#ifdef DOCUMENTATION
    /** Expression that performs a pre-decrement operation. */
    class SgAsmBinarySubtractPreupdate: public SgAsmBinaryExpression {};
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    DECLARE_LEAF_CLASS(AsmBinaryAddPostupdate);

#ifdef DOCUMENTATION
    /** Expression that performs a post-increment operation. */
    class SgAsmBinaryAddPostupdate: public SgAsmBinaryExpression {};
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmBinarySubtractPostupdate);

#ifdef DOCUMENTATION
    /** Expression that performs a post-decrement operation. */
    class SgAsmBinarySubtractPostupdate: public SgAsmBinaryExpression {};
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmBinaryLsl);

#ifdef DOCUMENTATION
    /** Expression that performs a logical left shift operation. */
    class SgAsmBinaryLsl: public SgAsmBinaryExpression {};
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmBinaryLsr);

#ifdef DOCUMENTATION
    /** Expression that performs a logical, sign-bit non-preserving right shift. */
    class SgAsmBinaryLsr: public SgAsmBinaryExpression {};
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmBinaryAsr);

#ifdef DOCUMENTATION
    /** Expression that performs an arithmetic, sign-bit preserving right shift. */
    class SgAsmBinaryAsr: public SgAsmBinaryExpression {};
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmBinaryRor);

#ifdef DOCUMENTATION
    /** Expression that performs a right rotate. */
    class SgAsmBinaryRor: public SgAsmBinaryExpression {};
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    NEW_NONTERMINAL_MACRO(AsmBinaryExpression,
                          AsmBinaryAdd               | AsmBinarySubtract      | AsmBinaryMultiply           |
                          AsmBinaryDivide            | AsmBinaryMod           | AsmBinaryAddPreupdate       |
                          AsmBinarySubtractPreupdate | AsmBinaryAddPostupdate | AsmBinarySubtractPostupdate |
                          AsmBinaryLsl               | AsmBinaryLsr           | AsmBinaryAsr                |
                          AsmBinaryRor,
                          "AsmBinaryExpression", "AsmBinaryExpressionTag", false);

#ifdef DOCUMENTATION
    /** Base class for binary expressions. */
    class SgAsmBinaryExpression: public SgAsmExpression {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Left-hand side operand.
         *
         *  @{ */
        SgAsmExpression* get_lhs() const;
        void set_lhs(SgAsmExpression*);
        /** @} */
#else
        AsmBinaryExpression.setDataPrototype("SgAsmExpression*", "lhs", "= NULL",
                                             CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Right-hand side operand.
         *
         *  @{ */
        SgAsmExpression* get_rhs() const;
        void set_rhs(SgAsmExpression*);
        /** @} */
#else
        AsmBinaryExpression.setDataPrototype("SgAsmExpression*", "rhs", "= NULL",
                                             CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmUnaryPlus);

#ifdef DOCUMENTATION
    /** Expression representing a (no-op) unary plus operation. */
    class SgAsmUnaryPlus: public SgAsmUnaryExpression {};
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    DECLARE_LEAF_CLASS(AsmUnaryMinus);

#ifdef DOCUMENTATION
    /** Expression represting negation. */
    class SgAsmUnaryMinus: public SgAsmUnaryExpression {};
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    DECLARE_LEAF_CLASS(AsmUnaryRrx);

#ifdef DOCUMENTATION
    // FIXME[Robb P Matzke 2016-10-31]: no idea what this is
    class SgAsmUnaryRrx: public SgAsmUnaryExpression {};
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmUnaryArmSpecialRegisterList);

#ifdef DOCUMENTATION
    // FIXME[Robb P Matzke 2016-10-31]: no idea what this is
    class SgAsmUnaryArmSpecialRegisterList: public SgAsmUnaryExpression {};
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NEW_NONTERMINAL_MACRO(AsmUnaryExpression,
                          AsmUnaryPlus | AsmUnaryMinus | AsmUnaryRrx | AsmUnaryArmSpecialRegisterList,
                          "AsmUnaryExpression", "AsmUnaryExpressionTag", false);

#ifdef DOCUMENTATION
    /** Base class for unary expressions. */
    class SgAsmUnaryExpression: public SgAsmExpression {
    public:
#endif

#ifndef DOCUMENTATION
        AsmUnaryExpression.setDataPrototype("SgAsmExpression*", "operand", "= NULL",
                                            CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmDirectRegisterExpression);

#ifdef DOCUMENTATION
    /** Expression representing a machine register. */
    class SgAsmDirectRegisterExpression: public SgAsmRegisterReferenceExpression {
    public:
#endif

#ifndef DOCUMENTATION
        AsmDirectRegisterExpression.setDataPrototype("unsigned", "psr_mask", "=0", // for ARM
                                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL,
                                                     NO_DELETE);
#endif

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmIndirectRegisterExpression);

#ifdef DOCUMENTATION
    /** Registers accessed indirectly.  For instance, x86 ST(1) which has base register "st", stride={0,1,0,0}, and offset
     *  register fpstatus_top, index is 1, and modulus is 8. */
    class SgAsmIndirectRegisterExpression: public SgAsmRegisterReferenceExpression {
    public:
#endif

#ifndef DOCUMENTATION
        AsmIndirectRegisterExpression.setDataPrototype("RegisterDescriptor", "stride", "",
                                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
        AsmIndirectRegisterExpression.setDataPrototype("RegisterDescriptor", "offset", "",
                                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
        AsmIndirectRegisterExpression.setDataPrototype("size_t", "index", "",
                                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
        AsmIndirectRegisterExpression.setDataPrototype("size_t", "modulus", "",
                                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NEW_NONTERMINAL_MACRO(AsmRegisterReferenceExpression, AsmDirectRegisterExpression|AsmIndirectRegisterExpression,
                          "AsmRegisterReferenceExpression", "AsmRegisterReferenceExpressionTag", false);

#ifdef DOCUMENTATION
    class SgAsmRegisterReferenceExpression: public SgAsmExpression {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Descriptor for accessed register.
         *
         *  @{ */
        RegisterDescriptor get_descriptor() const;
        void set_descriptor(RegisterDescriptor);
        /** @} */
#else
        AsmRegisterReferenceExpression.setDataPrototype("RegisterDescriptor", "descriptor", "",
                                                        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Post-increment or pre-decrement amount.
         *
         *  @{ */
        int get_adjustment() const;
        void set_adjustment(int);
        /** @} */
#else
        AsmRegisterReferenceExpression.setDataPrototype("int", "adjustment", "=0",
                                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL,
                                                        NO_DELETE);
#endif

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    DECLARE_LEAF_CLASS(AsmRegisterNames);

#ifdef DOCUMENTATION
    /** An ordered list of registers. */
    class SgAsmRegisterNames: public SgAsmExpression {
    public:
#endif

#ifndef DOCUMENTATION
        AsmRegisterNames.setDataPrototype("SgAsmRegisterReferenceExpressionPtrList", "registers", "",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
        AsmRegisterNames.setDataPrototype("unsigned", "mask", "=0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmIntegerValueExpression);

#ifdef DOCUMENTATION
    /**  Base class for integer values.
     *
     *  An integer value consists of an offset from an optional base node.  The base node must be some kind of object with a
     *  virtual address, such as an instruction, symbol, segment, etc.  If no base node is associated with the
     *  SgAsmIntegerValueExpression (the default situation), then a zero base address is used.
     *
     *  When a (new) base object is associated with an SgAsmIntegerValueExpression via the makeRelativeTo() method, the value
     *  of the expression does not change.  However, the value does change when the address of the associated base node
     *  changes.  For instance, one way to represent a function call to "main" is to have a CALL instruction whose operand is
     *  an SgAsmIntegerValueExpression that has a base which is either the entry instruction of "main" or the symbol for
     *  "main".  That way, if the address of "main" changes then the target address in the CALL instruction also changes.
     *
     *  The base class stores the bits that are interpretted as the signed offset. The offset is accessed with
     *  get_relativeValue() and set_relativeValue() methods. The class also defines get_absoluteValue() and
     *  set_aabsoluteValue() methods that operate on the absolute value (which isn't actually stored anywhere). */
    class SgAsmIntegerValueExpression: public SgAsmConstantExpression {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Base node associated with an integer.
         *
         * When setting this property, the base node is changed without updating this object's relative value, thus this
         * object's absolute value changes.  The value returned by get_absoluteValue() will probably differ from what it would
         * have returned before calling set_baseNode().  If this is not the behavior that's needed, see the makeRelativeTo()
         * method.
         *
         * @{ */
        SgNode* get_baseNode() const;
        void set_baseNode(SgNode*);
        /** @} */
#else
        AsmIntegerValueExpression.setDataPrototype("SgNode*", "baseNode", "=NULL",
                                                   NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmIntegerValueExpression);
#if defined(SgAsmIntegerValueExpression_OTHERS) || defined(DOCUMENTATION)
    public:
        SgAsmIntegerValueExpression(uint64_t n, SgAsmType *type);
        SgAsmIntegerValueExpression(const Sawyer::Container::BitVector &bv, SgAsmType *type);

        /** Returns the base address of an addressable IR node. */
        static uint64_t virtualAddress(SgNode*);

        /** Returns a label for the value.
         *
         *  The label consists of the base object name (if available) or address, followed by a plus sign or minus sign,
         *  followed by the offset from that object.  The empty string is returned if this integer value expression has no base
         *  object (i.e., it's absolute).
         *
         *  If the base object has no name and the integer value points directly at the object (offset=0) then one of two
         *  things happen: if @p quiet is true, the empty string is returned, otherwise the label is the name of the node type
         *  enclosed in an extra set of angle brackets.  This is useful to indicate that a value is relative rather than
         *  absolute.  For instance, the instruction listing "call 0x004126bb" is ambiguous as to whether 0x004126bb points to
         *  a known, unnamed function, a non-entry instruction within a function, or some memory location we didn't
         *  disassemble.  But when labeled with @p quiet being false, the output will be:
         *
         *  <ul>
         *    <li>call 0x004126bb<main>; points to a function with a name</li>
         *    <li>call 0x004126bb<<Func>>; points to a function without a name</li>
         *    <li>call 0x004126bb<<Insn>>; points to an instruction that's not a function entry point</li>
         *    <li>call 0x004126bb; points to something that's not been disassembled</li>
         *  </ul> */
        std::string get_label(bool quiet=false) const;

        /** Return the number of significant bits in the value. */
        size_t get_significantBits() const;

        /** Makes the value of this integer relative to some other addressable node.
         *
         *  The absolute value of this expression is unchanged by this operation. The @p baseNode must be a type of IR node
         *  that has a virtual address, such as another instruction.  If @p baseNode is the null pointer, then the
         *  "relativeness" of this constant is removed (i.e., it will be relative to zero). */
        void makeRelativeTo(SgNode *baseNode);

        /** Returns the base address.
         *
         *  The base address is the virtual address of the associated IR node, or zero if no IR node is associated with this
         *  integer value. */
        uint64_t get_baseAddress() const;

        /** Returns the current absolute value zero filled to 64 bits.
         *
         *  The absolute value is the 64-bit sum of the 64-bit address of the base node (or zero if no base node is associated
         *  with this object) and the 64-bit offset. However, this function returns only the specified number of low-order bits
         *  zero extended to the 64-bit return type.  If @p nbits is zero, then get_significantBits() is called. */
        uint64_t get_absoluteValue(size_t nbits=0) const;

        /** Set absolute value.
         *
         *  Changes the absolute value of this integer expression without changing the base node. */
        void set_absoluteValue(uint64_t);

        /** Returns the current absolute value (base+offset) as a signed value. */
        int64_t get_signedValue() const;

        /** Get relative value.
         *
         *  Interprets the bit vector as a signed value, sign extends it to 64-bits if necessary, and returns it. */
        int64_t get_relativeValue() const;

        /** Set relative value without changing the base value.
         *
         *  The relative value is interpretted as a signed value of the specified
         *  width (defaulting to 64-bits). */
        void set_relativeValue(int64_t v, size_t nbits=64);

        uint64_t get_value() const { return get_absoluteValue(); }

        // These are deprecated; use CamelCase versions instead [Robb P. Matzke 2014-07-21]
        size_t get_significant_bits() const ROSE_DEPRECATED("use get_significantBits");
        void make_relative_to(SgNode*) ROSE_DEPRECATED("use makeRelativeTo");
        uint64_t get_base_address() const ROSE_DEPRECATED("use get_baseAddress");
        uint64_t get_absolute_value(size_t nbits=0) const ROSE_DEPRECATED("use get_absoluteValue");
        void set_absolute_value(uint64_t) ROSE_DEPRECATED("use set_absoluteValue");
        int64_t get_signed_value() const ROSE_DEPRECATED("use set_signedValue");
        int64_t get_relative_value() const ROSE_DEPRECATED("use get_relativeValue");
        void set_relative_value(int64_t, size_t nbits=64) ROSE_DEPRECATED("use set_relativeValue");
#endif // SgAsmIntegerValueExpression_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmFloatValueExpression);

#ifdef DOCUMENTATION
    /** Floating-point value.
     *
     *  The bits are stored in the super-class (SgAsmConstantExpression) and interpretted as various kinds of floating-point
     *  values. */
    class SgAsmFloatValueExpression: public SgAsmConstantExpression {
    public:
#endif

        AsmFloatValueExpression.setAutomaticGenerationOfConstructor(false); // so cache can be initialized

        DECLARE_OTHERS(AsmFloatValueExpression);
#if defined(SgAsmFloatValueExpression_OTHERS) || defined(DOCUMENTATION)
    private:
        // This node stores its primary representation of the value in the p_bitVector of a parent class.  However, since we
        // often access the value as a native "double", and since converting to/from a bit vector is costly, we want to cache
        // the native double value whenever we compute it.  ROSETTA does not need to be aware of the cached value since it can
        // be recomputed from the bit vector, and in fact, ROSETTA can't handle cache data members because it doesn't
        // understand "mutable".
        mutable double p_nativeValue;
        mutable bool p_nativeValueIsValid;
    public:
        SgAsmFloatValueExpression(): p_nativeValue(0.0), p_nativeValueIsValid(true) {}
        SgAsmFloatValueExpression(double nativeValue, SgAsmType*);
        SgAsmFloatValueExpression(const Sawyer::Container::BitVector&,SgAsmType*);
        void set_nativeValue(double);
        double get_nativeValue() const;
        void updateBitVector();
        void updateNativeValue() const;
#endif // SgAsmFloatValueExpression_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NEW_NONTERMINAL_MACRO(AsmConstantExpression,
                          AsmIntegerValueExpression | AsmFloatValueExpression,
                          "AsmConstantExpression", "AsmConstantExpressionTag", false);

    DECLARE_HEADERS(AsmConstantExpression);
#if defined(SgAsmConstantExpression_HEADERS) || defined(DOCUMENTATION)
    #include <Sawyer/BitVector.h>
#endif // SgAsmConstantExpression_HEADERS

#ifdef DOCUMENTATION
    /** Base class for constants.  Represents integer values, floating-point values, etc. This class holds the actual bits for
    // the constant value.  Subclasses provide the intepretation of those bits. */
    class SgAsmConstantExrpression: public SgAsmValueExression {
    public:
#endif
        
#ifdef DOCUMENTATION
        /** Property: Bits for constant.
         *
         *  @{ */
        Sawyer::Container::BitVector get_bitVector() const;
        void set_bitVector(Sawyer::Container::BitVector);
        /** @} */
#else
        AsmConstantExpression.setDataPrototype("Sawyer::Container::BitVector", "bitVector", "",
                                               NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmConstantExpression);
#if defined(SgAsmConstantExpression_OTHERS) || defined(DOCUMENTATION)
    public:
        const Sawyer::Container::BitVector& get_bitVector() const { return p_bitVector; }
        Sawyer::Container::BitVector& get_bitVector() { return p_bitVector; }
        void set_bitVector(const Sawyer::Container::BitVector &bv) { p_bitVector = bv; }
#endif // SgAsmConstantExpression_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NEW_NONTERMINAL_MACRO(AsmValueExpression,
                          AsmConstantExpression,
                          "AsmValueExpression", "AsmValueExpressionTag", false);

#ifdef DOCUMENTATION
    /** Base class for values.
     *
     *  Values that are addresses or references to data will have symbols in a function symbol table.  All other values are
     *  assumed to be literals and will not have associated symbols. */
    class SgAsmValueExpression: public SgAsmExpression {
    public:
#endif

#ifndef DOCUMENTATION
        AsmValueExpression.setDataPrototype("SgAsmValueExpression*", "unfolded_expression_tree", "= NULL",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
        AsmValueExpression.setDataPrototype("unsigned short", "bit_offset", "= 0",         // DOXYGEN
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
        AsmValueExpression.setDataPrototype("unsigned short", "bit_size", "= 0",           // DOXYGEN
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
        AsmValueExpression.setDataPrototype("SgSymbol*", "symbol", "= NULL",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmMemoryReferenceExpression);

#ifdef DOCUMENTATION
    /** Reference to memory locations. */
    class SgAsmMemoryReferenceExpression: public SgAsmExpression {
    public:
#endif
        
#ifndef DOCUMENTATION
        AsmMemoryReferenceExpression.setDataPrototype("SgAsmExpression*", "address", "= NULL",
                                                      CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
        AsmMemoryReferenceExpression.setDataPrototype("SgAsmExpression*", "segment", "= NULL",
                                                      CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmControlFlagsExpression);

#ifdef DOCUMENTATION
    // FIXME[Robb P Matzke 2016-10-31]: no idea what this is
    class SgAsmControlFlagsExpression: public SgAsmExpression {
    public:
#endif
        
#ifndef DOCUMENTATION
        AsmControlFlagsExpression.setDataPrototype("unsigned long", "bit_flags", "= 0",
                                                   NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmCommonSubExpression);

#ifdef DOCUMENTATION
    // FIXME[Robb P Matzke 2016-10-31]: no idea what this is
    class SgAsmCommonSubexpression: public SgAsmExpression {
    public:
#endif

#ifndef DOCUMENTATION
        AsmCommonSubExpression.setDataPrototype("SgAsmExpression*", "subexpression", "= 0",
                                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmRiscOperation);

#ifdef DOCUMENTATION
    /** Static representation of instruction semantics.
     *
     *  Instruction semantics are not added to the AST by default since this would make it very, very large.  Instead, ROSE has
     *  a non-traditional approach: instead of ROSE having C++ code to generate a data-centric representation of semantics (a
     *  tree like data structure similar to a syntax tree) and then users writing analyses on that data structure, ROSE's C++
     *  code can be hooked into directly by users via C++ class derivation. If a user really wants a data-centric view they can
     *  either have ROSE create @ref SgAsmRiscOperation nodes in the AST, or they can hook into ROSE's instruction semantics
     *  API and build whatever kind of data-centric representation that suites their need. */
    class SgAsmRiscOperation: public SgAsmExpression {
    public:
#endif

#ifndef DOCUMENTATION
        AsmRiscOperation.setDataPrototype("SgAsmRiscOperation::RiscOperator", "riscOperator", "= SgAsmRiscOperation::OP_NONE",
                                          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
        AsmRiscOperation.setDataPrototype("SgAsmExprListExp*", "operands", "= NULL",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmRiscOperation);
#if defined(SgAsmRiscOperation_OTHERS) || defined(DOCUMENTATION)
    public:
        /** One enum per RISC operator.
         *
         * The names are the same as the RISC operator.  Note that some of these operators aren't normally part of the base
         * set. For instance, "subtract" and comparison operations are usually implemented in terms of more basic operations
         * like add_, invert, and equalToZero. */
        enum RiscOperator {
            OP_NONE,
            OP_bottom,
            OP_undefined,
            OP_unspecified,
            OP_filterCallTarget,
            OP_filterReturnTarget,
            OP_filterIndirectJumpTarget,
            OP_hlt,
            OP_cpuid,
            OP_rdtsc,
            OP_and_,
            OP_or_,
            OP_xor_,
            OP_invert,
            OP_extract,
            OP_concat,
            OP_leastSignificantSetBit,
            OP_mostSignificantSetBit,
            OP_rotateLeft,
            OP_rotateRight,
            OP_shiftLeft,
            OP_shiftRight,
            OP_shiftRightArithmetic,
            OP_equalToZero,
            OP_ite,
            OP_isEqual,
            OP_isNotEqual,
            OP_isUnsignedLessThan,
            OP_isUnsignedLessThanOrEqual,
            OP_isUnsignedGreaterThan,
            OP_isUnsignedGreaterThanOrEqual,
            OP_isSignedLessThan,
            OP_isSignedLessThanOrEqual,
            OP_isSignedGreaterThan,
            OP_isSignedGreaterThanOrEqual,
            OP_unsignedExtend,
            OP_signExtend,
            OP_add,		                        /**< Two args + optional carry bit. */
            OP_addCarries,                              /**< Carries from a 3-arg add operation. */
            OP_subtract,
            OP_negate,
            OP_signedDivide,
            OP_signedModulo,
            OP_signedMultiply,
            OP_unsignedDivide,
            OP_unsignedModulo,
            OP_unsignedMultiply,
            OP_interrupt,
            OP_readRegister,
            OP_writeRegister,
            OP_readMemory,                              /**< Three or four args depending on whether segment reg is present. */
            OP_writeMemory,                             /**< Three or four args depending on whether segment reg is present. */
            OP_N_OPERATORS                              /**< Number of operators in this enum. */ // MUST BE LAST!
        };
#endif // SgAsmRiscOperation_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmExprListExp);

#ifdef DOCUMENTATION
    /** List of expression nodes. */
    class SgAsmExprListExp: public SgAsmExpression {
    public:
#endif
        
#ifndef DOCUMENTATION
        AsmExprListExp.setDataPrototype("SgAsmExpressionPtrList", "expressions", "",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NEW_NONTERMINAL_MACRO(AsmExpression,
                          AsmValueExpression           | AsmBinaryExpression            | AsmUnaryExpression        |
                          AsmMemoryReferenceExpression | AsmRegisterReferenceExpression | AsmControlFlagsExpression |
                          AsmCommonSubExpression       | AsmExprListExp                 | AsmRegisterNames          |
                          AsmRiscOperation,
                          "AsmExpression", "AsmExpressionTag", false);

#ifdef DOCUMENTATION
    /** Base class for expressions. */
    class SgAsmExpression: public SgAsmNode {
    public:
#endif

#ifndef DOCUMENTATION
    AsmExpression.setDataPrototype("SgAsmType*", "type", "= NULL",
                                   NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Replacement string for RoseBin_IDAPRO_buildTree.
         *
         *  The @p replacement is only set by @ref RoseBin_IDAPRO_buildTree::resolveRecursivelyExpression and appears to be
         *  used only in a couple of files in src/midend/binaryAnalsyses (and elsewhere only for converting a @ref
         *  SgAsmExpression to a string). It seems to hold the name of a function, such as "_malloc" or "malloc@plt" for branch
         *  instructions. It should be possible to obtain the function name by looking up the instruction at the branch target
         *  and then following parent links in the AST until we reach the SgAsmFunction node, which has a get_name() method.
         *
         * @{ */
        std::string get_replacement() const;
        void set_replacement(std::string);
        /** @} */
#else
        AsmExpression.setDataPrototype("std::string", "replacement", "= \"\"",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifndef DOCUMENTATION
        AsmExpression.setDataPrototype("std::string", "comment", "= \"\"",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmExpression);
#if defined(SgAsmExpression_OTHERS) || defined(DOCUMENTATION)
    public:
        size_t get_nBits() const;
#endif // SgAsmExpression_OTHERS

#ifdef DOCUMENTATION
    };
#endif



        

    /***************************************************************************************************************************
     *                                  Data Types (new interface 2014-07)
     *
     * Data types are generally read-only objects because we need to be able to test type equality by testing pointer
     * equality.  This is a basic features of ROSE: two types are equal if their pointers are equal.  The inverse should also
     * be true, which means that when we ask for a particular type we get the same pointer as before.  This can only work if we
     * don't allow types to be modified after they're created.
     ***************************************************************************************************************************/

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmIntegerType);

#ifdef DOCUMENTATION
    /** Integer types. */
    class SgAsmIntegerType: public SgAsmScalarType {
    public:
#endif

#ifndef DOCUMENTATION
        // Repeated below because they're read-only
        AsmIntegerType.setDataPrototype("bool", "isSigned", "=false", // read-only
                                        NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmIntegerType);
#if defined(SgAsmIntegerType_OTHERS) || defined(DOCUMENTATION)
    public:
        SgAsmIntegerType(ByteOrder::Endianness, size_t nBits, bool isSigned);
        virtual void check() const;
        virtual std::string toString() const;
        bool get_isSigned() const;
#endif // SgAsmIntegerType_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmFloatType);
    DECLARE_HEADERS(AsmFloatType);
#if defined(SgAsmFloatType_HEADERS) || defined(DOCUMENTATION)
    #include <Sawyer/BitVector.h>
#endif // SgAsmFloatType_HEADERS

#ifdef DOCUMENTATION
    /** Floating point types. */
    class SgAsmFloatType: public SgAsmScalarType {
    public:
#endif

#ifndef DOCUMENTATION
        // Repeated below because they're read-only
        AsmFloatType.setDataPrototype("size_t", "significandOffset", "=(size_t)(-1)", // read-only
                                      NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
        AsmFloatType.setDataPrototype("size_t", "significandNBits", "=(size_t)(-1)", // read-only
                                      NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
        AsmFloatType.setDataPrototype("size_t", "signBitOffset", "=(size_t)(-1)", // read-only
                                      NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
        AsmFloatType.setDataPrototype("size_t", "exponentOffset", "=(size_t)(-1)", // read-only
                                      NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
        AsmFloatType.setDataPrototype("size_t", "exponentNBits", "=(size_t)(-1)", // read-only
                                      NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
        AsmFloatType.setDataPrototype("uint64_t", "exponentBias", "=0", // read-only
                                      NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
        AsmFloatType.setDataPrototype("unsigned", "flags", "=0", // read-only
                                      NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmFloatType);
#if defined(SgAsmFloatType_OTHERS) || defined(DOCUMENTATION)
    public:
        enum {
            GRADUAL_UNDERFLOW      = 0x00000001,
            NORMALIZED_SIGNIFICAND = 0x00000002
        };

        typedef Sawyer::Container::BitVector::BitRange BitRange;

        /** Construct a new floating-point type. */
        SgAsmFloatType(ByteOrder::Endianness, size_t nBits,
                       const BitRange &significandBits, const BitRange exponentBits, size_t signBit,
                       uint64_t exponentBias, unsigned flags);

        virtual void check() const;
        virtual std::string toString() const;
                
        /** Property: Offset to significand least significant bit. */
        BitRange significandBits() const;

        /** Property: Number of bits in the exponent. */
        BitRange exponentBits() const;

        /** Property: Offset to significand sign bit. */
        size_t signBit() const;

        /** Property: Zero-point of exponent. */
        uint64_t exponentBias() const;

        /** Property: Bit vector of all boolean properties. */
        unsigned flags() const;

        /** Property: Whether type has gradual underflow. */
        bool gradualUnderflow() const;

        /** Property: Whether type has normalized significand. */
        bool normalizedSignificand() const;
#endif // SgAsmFloatType_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
    NEW_NONTERMINAL_MACRO(AsmScalarType,
                          AsmIntegerType | AsmFloatType,
                          "AsmScalarType", "AsmScalarTypeTag", false);

#ifdef DOCUMENTATION
    /** Base class for scalar types. */
    class SgAsmScalarType: public SgAsmType {
    public:
#endif
        
#ifndef DOCUMENTATION
        // Repeated below since these are read-only
        AsmScalarType.setDataPrototype("ByteOrder::Endianness", "minorOrder", "= ByteOrder::ORDER_UNSPECIFIED", // read-only
                                       NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
        AsmScalarType.setDataPrototype("ByteOrder::Endianness", "majorOrder", "= ByteOrder::ORDER_UNSPECIFIED", // read-only
                                       NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
        AsmScalarType.setDataPrototype("size_t", "majorNBytes", "=0", // read-only
                                       NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
        AsmScalarType.setDataPrototype("size_t", "nBits", "=0", // read-only
                                       NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmScalarType);
#if defined(SgAsmScalarType_OTHERS) || defined(DOCUMENTATION)
    protected:
        /** Construct a new scalar type.
         *
         *  Since scalar types are base classes, one normally does not construct just a scalar type but rather one of the base
         *  classes. */
        SgAsmScalarType(ByteOrder::Endianness, size_t nBits);

    public:
        virtual void check() const;
        virtual std::string toString() const;

        /** Property: Number of bits. */
        virtual size_t get_nBits() const ROSE_OVERRIDE;

        /** Property: Minor byte order. This is the usual notion of byte order. */
        ByteOrder::Endianness get_minorOrder() const;

        /** Property: Major byte order for mixed-order types. */
        ByteOrder::Endianness get_majorOrder() const;

        /** Property: Stride of major byte order for mixed order types. */
        size_t get_majorNBytes() const;
#endif // SgAsmScalarType_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmVectorType);

#ifdef DOCUMENTATION
    /** Base class for vector types. */
    class SgAsmVectorType: public SgAsmType {
    public:
#endif

#ifndef DOCUMENTATION
        // Repeated below since these are read-only
        AsmVectorType.setDataPrototype("size_t", "nElmts", "=0", // read-only
                                       NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
        AsmVectorType.setDataPrototype("SgAsmType*", "elmtType", "=NULL", // read-only
                                       NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmVectorType);
#if defined(SgAsmVectorType_OTHERS) || defined(DOCUMENTATION)
    public:
        /** Construct a new vector type. */
        SgAsmVectorType(size_t nElmts, SgAsmType *elmtType);

        size_t get_nElmts() const;
        SgAsmType* get_elmtType() const;
        virtual void check() const ROSE_OVERRIDE;
        virtual std::string toString() const ROSE_OVERRIDE;
        virtual size_t get_nBits() const ROSE_OVERRIDE;
#endif // SgAsmVectorType_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    NEW_NONTERMINAL_MACRO(AsmType, AsmScalarType | AsmVectorType, "AsmType", "AsmTypeTag", false);

#ifdef DOCUMENTATION
    /** Base class for binary types. */
    class SgAsmType: public SgAsmNode {
    public:
#endif

        DECLARE_OTHERS(AsmType);
#if defined(SgAsmType_OTHERS) || defined(DOCUMENTATION)
    private:
        static Sawyer::Container::Map<std::string, SgAsmType*> p_typeRegistry;

    public:
        virtual void check() const;
        virtual std::string toString() const { abort(); return NULL; }       // ROSETTA doesn't support pure virtual methods; (TOO1, 2014-08-11): Windows requires return value
        virtual size_t get_nBits() const { abort(); return (size_t)-1; }           // ROSETTA doesn't support pure virtual methods; (TOO1, 2014-08-11): Windows requires return value
        virtual size_t get_nBytes() const;

        template<class Type>                                    // Type is a subclass of SgAsmType
        static Type* registerOrDelete(Type *toInsert) {
            ASSERT_not_null(toInsert);
            std::string key = toInsert->toString();
            Type *retval = dynamic_cast<Type*>(p_typeRegistry.insertMaybe(key, toInsert));
            ASSERT_not_null(retval);
            if (retval!=toInsert)
                delete toInsert;
            return retval;
        }
#endif // SgAsmType_OTHERS

#ifdef DOCUMENTATION
    };
#endif




    /**************************************************************************************************************************
     *                                  Collections of Instructions
     **************************************************************************************************************************/

    // A function is a collection of blocks holding instructions (basic blocks) or static data.  Instructions might have
    // references to addresses or data which are described by symbols (not to be confused with the binary's symbol table) in
    // the function's symbol table (the SgAsmFunction::symbol_table member).
    NEW_TERMINAL_MACRO(AsmFunction, "AsmFunction", "AsmFunctionTag");
    AsmFunction.setPredeclarationString("HEADER_BINARY_FUNCTION_PREDECLARATION", "../Grammar/BinaryInstruction.code");
    AsmFunction.setFunctionPrototype("HEADER_BINARY_FUNCTION_DECLARATION", "../Grammar/BinaryInstruction.code");
    AsmFunction.setFunctionSource("SOURCE_BINARY_FUNCTION_DECLARATION", "../Grammar/BinaryInstruction.code");
    AsmFunction.setDataPrototype("std::string", "name", "= \"\"",
                                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmFunction.setDataPrototype("unsigned", "reason", "= SgAsmFunction::FUNC_NONE", /*bit flags*/
                                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmFunction.setDataPrototype("SgAsmFunction::function_kind_enum", "function_kind", "= SgAsmFunction::e_unknown",
                                 CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
    AsmFunction.setDataPrototype("SgAsmFunction::MayReturn", "may_return", "= SgAsmFunction::RET_UNKNOWN",
                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmFunction.setDataPrototype("std::string", "name_md5", "= \"\"",
                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmFunction.setDataPrototype("SgAsmStatementPtrList", "statementList", "",
                                 NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AsmFunction.setDataPrototype("SgAsmStatementPtrList", "dest", "",
                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmFunction.setDataPrototype("rose_addr_t", "entry_va", "= 0",  /*entry point virtual address*/
                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmFunction.setDataPrototype("SgSymbolTable*", "symbol_table", "= NULL",
                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, NO_COPY_DATA);
    AsmFunction.setDataPrototype("size_t", "cached_vertex", "= (size_t)(-1)", // see BinaryAnalysis::FunctionCall
                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // net effect of function on the stack pointer
    AsmFunction.setDataPrototype("int64_t", "stackDelta", "= SgAsmInstruction::INVALID_STACK_DELTA",
                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmFunction.setDataPrototype("const rose::BinaryAnalysis::CallingConvention::Definition*", "callingConvention", "=NULL",
                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    // Instruction basic block. One entry point (first instruction) and one exit point (last instruction).  However,
    // SgAsmBlock is also used for other things, such as collections of functions.
    //
    // statementList and successors should have been pointers to nodes that contain the list rather than being the lists
    // themselves because ROSETTA doesn't allow traversals on multiple list data members--we can traverse either one list or
    // the other, but not both.  It's too late to change how this part of the AST is structured because so much user code
    // already depends on it, therefore we can only traverse statementList and not successors. [Robb Matzke 2016-02-25]
    NEW_TERMINAL_MACRO(AsmBlock, "AsmBlock", "AsmBlockTag");
    AsmBlock.setFunctionPrototype("HEADER_BINARY_BLOCK", "../Grammar/BinaryInstruction.code");
    AsmBlock.setFunctionSource("SOURCE_BINARY_BLOCK", "../Grammar/BinaryInstruction.code");
    AsmBlock.setDataPrototype("rose_addr_t", "next_block_true_address", "= 0", // [tps 05Apr07] needed for the control_flow_graph
                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmBlock.setDataPrototype("rose_addr_t", "next_block_false_address", "= 0",
                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmBlock.setDataPrototype("rose_addr_t", "id", "= 0",
                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
    AsmBlock.setDataPrototype("unsigned", "reason", "= SgAsmBlock::BLK_NONE", // why this block exists
                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmBlock.setDataPrototype("SgAsmStatementPtrList", "statementList", "", //in order of execution
                              NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AsmBlock.setDataPrototype("SgAsmIntegerValuePtrList", "successors", "",
                              NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmBlock.setDataPrototype("bool", "successors_complete", "= false",
                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmBlock.setDataPrototype("SgAsmBlock*", "immediate_dominator", "=NULL",
                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmBlock.setDataPrototype("size_t", "cached_vertex", "= (size_t)(-1)", // see BinaryAnalysis::ControlFlow
                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmBlock.setDataPrototype("double", "code_likelihood", "= 0.0",
                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // stack pointer at end of basic block relative to start of block's function
    AsmBlock.setDataPrototype("int64_t", "stackDeltaOut", "= SgAsmInstruction::INVALID_STACK_DELTA",
                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    // Represents static data in an executable.  For now, we don't associate any type with the data because ROSE's data type
    // infrastructure (source or binary) is not capable of representing the information we need: multiple interpretations of
    // overlapping parts of memory (i.e., two or more types for the same bytes); arbitrary offsets and padding in structured
    // types; size-specific integers and floating-point types; regions of unknown type; ease of improving type information by
    // filling in more details as the type is discovered; etc.
    NEW_TERMINAL_MACRO(AsmStaticData, "AsmStaticData", "AsmStaticDataTag");
    AsmStaticData.setFunctionPrototype("HEADER_STATIC_DATA", "../Grammar/BinaryInstruction.code");
    AsmStaticData.setDataPrototype("SgUnsignedCharList", "raw_bytes", "",
                                   NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);



    // Declaration-like nodes that encapsulate multiple instructions.  Binary ASTs have two sides: the container side that
    // corresponds to the ELF/PE/etc. file formats, and the interpretation side that corresponds to instructions and data from
    // multiple sources (specimen + dynamic libraries) organized into multiple SgAsmInterpretation where each interpretation
    // makes a coherent binary entity such as the DOS part of a PE executable.  The declaration-like nodes that follow appear
    // on the interpretation side of the AST.  We may add other declaration nodes to the container side of the AST at a later
    // time.
    //
    // These interpretation-side declaration-like nodes are used by the projects/BinaryDataStructureRecognition even if they
    // aren't used internally by ROSE.
    NEW_TERMINAL_MACRO(AsmSynthesizedDataStructureDeclaration,
                       "AsmSynthesizedDataStructureDeclaration", "AsmSynthesizedDataStructureDeclarationTag");
    AsmSynthesizedDataStructureDeclaration.setFunctionPrototype("HEADER_BINARY_DATA_STRUCTURE",
                                                                "../Grammar/BinaryInstruction.code");
    AsmSynthesizedDataStructureDeclaration.setFunctionSource("SOURCE_BINARY_DATA_STRUCTURE",
                                                             "../Grammar/BinaryInstruction.code");
#if 0
    // DQ (3/15/2007): I can't seem to get this to compile so I will leave it out for now!
    // Binaries have some easily resolved data structures so we use this to represent these
    AsmDataStructureDeclaration.setDataPrototype("std::list<SgAsmDeclaration*>","declarationList","",
                                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL,
                                                 NO_DELETE, COPY_DATA);
#endif



    NEW_TERMINAL_MACRO(AsmSynthesizedFieldDeclaration, "AsmSynthesizedFieldDeclaration", "AsmSynthesizedFieldDeclarationTag");
    // These are used as data members in AsmDataStructureDeclaration
    AsmSynthesizedFieldDeclaration.setDataPrototype("std::string","name","= \"\"",
                                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // Not clear if we want to store the offset explicitly
    AsmSynthesizedFieldDeclaration.setDataPrototype("uint64_t","offset","= 0",
                                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    // DQ (9/2/2013): We may later wish to change "AsmFunction" to "AsmSynthesizedFunction", since functions are
    // philosophically a synthesized concept within a binary (except where Binary API standards must be followed to permit
    // seperate compilation).
    // RPM (9/18/2013): On the other hand, most things on the interpretation side of the AST will be synthesized: basic
    // blocks, functions, code vs. data, thunk tables, trampolines, exception handling structures, data types, CFG structures
    // like switch statements, ... do users want or need "synthesized" in all those node type names?
    NEW_NONTERMINAL_MACRO(AsmSynthesizedDeclaration,
                          AsmSynthesizedDataStructureDeclaration | AsmFunction | AsmSynthesizedFieldDeclaration,
                          "AsmSynthesizedDeclaration", "AsmSynthesizedDeclarationTag", false );
    AsmSynthesizedDeclaration.setFunctionPrototype("HEADER_BINARY_DECLARATION", "../Grammar/BinaryInstruction.code");



    NEW_NONTERMINAL_MACRO(AsmStatement,
                          AsmSynthesizedDeclaration | AsmBlock | AsmInstruction | AsmStaticData,
                          "AsmStatement", "AsmStatementTag", false);
    AsmStatement.setDataPrototype("rose_addr_t", "address", "= 0",
                                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
    AsmStatement.setDataPrototype("std::string", "comment", "= \"\"",
                                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);



    /*************************************************************************************************************************
     *                                         Binary Interpretations
     * Some file formats have more than one thing in them: PE files have a DOS and Windows executable both in the same file;
     * Apple OS X files can have multiple architectures represented in a single file, etc.  The interpretation is the root of
     * an AST that represents a single, coherent sub-part of the file.
     *************************************************************************************************************************/

    NEW_TERMINAL_MACRO(AsmInterpretationList, "AsmInterpretationList", "AsmInterpretationListTag");
    AsmInterpretationList.setDataPrototype("SgAsmInterpretationPtrList", "interpretations", "",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmInterpretation, "AsmInterpretation", "AsmInterpretationTag");
    AsmInterpretation.setFunctionPrototype("HEADER_INTERPRETATION", "../Grammar/BinaryInstruction.code");
    AsmInterpretation.setPredeclarationString("HEADER_INTERPRETATION_PREDECLARATION", "../Grammar/BinaryInstruction.code");
    AsmInterpretation.setAutomaticGenerationOfConstructor(false);
    AsmInterpretation.setDataPrototype("SgAsmGenericHeaderList*", "headers", "= NULL",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmInterpretation.setDataPrototype("SgAsmBlock*", "global_block", "= NULL",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    /*************************************************************************************************************************
     *                                         ELF File Header
     *************************************************************************************************************************/

    NEW_TERMINAL_MACRO(AsmElfFileHeader, "AsmElfFileHeader", "AsmElfFileHeaderTag");
    AsmElfFileHeader.setFunctionPrototype("HEADER_ELF_HEADER", "../Grammar/BinaryInstruction.code");
    AsmElfFileHeader.setFunctionSource("SOURCE_ELF_HEADER", "../Grammar/BinaryInstruction.code");
    AsmElfFileHeader.setDataPrototype("unsigned char", "e_ident_file_class", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfFileHeader.setDataPrototype("unsigned char", "e_ident_data_encoding", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfFileHeader.setDataPrototype("unsigned char", "e_ident_file_version", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfFileHeader.setDataPrototype("SgUnsignedCharList", "e_ident_padding", "",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfFileHeader.setDataPrototype("unsigned long", "e_type", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfFileHeader.setDataPrototype("unsigned long", "e_machine", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfFileHeader.setDataPrototype("unsigned long", "e_flags", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfFileHeader.setDataPrototype("unsigned long", "e_ehsize", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfFileHeader.setDataPrototype("unsigned long", "phextrasz", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfFileHeader.setDataPrototype("unsigned long", "e_phnum", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfFileHeader.setDataPrototype("unsigned long", "shextrasz", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfFileHeader.setDataPrototype("unsigned long", "e_shnum", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfFileHeader.setDataPrototype("unsigned long", "e_shstrndx", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfFileHeader.setDataPrototype("SgAsmElfSectionTable*", "section_table", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfFileHeader.setDataPrototype("SgAsmElfSegmentTable*", "segment_table", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    /*************************************************************************************************************************
     *                                         ELF Section Tables
     * The ELF Section Table is itself a section.  The entries of the table are stored with the section they describe rather
     * than storing them all in the SgAsmSectionTable node.  We can reconstruct the ELF Section Table since sections have
     * unique ID numbers that are their original indices in the ELF Section Table.
     *************************************************************************************************************************/

    NEW_TERMINAL_MACRO(AsmElfSectionTable, "AsmElfSectionTable", "AsmElfSectionTableTag");
    AsmElfSectionTable.setFunctionPrototype("HEADER_ELF_SECTION_TABLE", "../Grammar/BinaryInstruction.code");



    NEW_TERMINAL_MACRO(AsmElfSectionTableEntry, "AsmElfSectionTableEntry", "AsmElfSectionTableEntryTag");
    AsmElfSectionTableEntry.setFunctionPrototype("HEADER_ELF_SECTION_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
    AsmElfSectionTableEntry.setFunctionSource("SOURCE_ELF_SECTION_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
    AsmElfSectionTableEntry.setDataPrototype("unsigned", "sh_name", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfSectionTableEntry.setDataPrototype("SgAsmElfSectionTableEntry::SectionType", "sh_type", "= SHT_PROGBITS",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfSectionTableEntry.setDataPrototype("unsigned long", "sh_link", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfSectionTableEntry.setDataPrototype("unsigned long", "sh_info", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfSectionTableEntry.setDataPrototype("uint64_t", "sh_flags", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfSectionTableEntry.setDataPrototype("rose_addr_t", "sh_addr", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfSectionTableEntry.setDataPrototype("rose_addr_t", "sh_offset", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfSectionTableEntry.setDataPrototype("rose_addr_t", "sh_size", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfSectionTableEntry.setDataPrototype("rose_addr_t", "sh_addralign", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfSectionTableEntry.setDataPrototype("rose_addr_t", "sh_entsize", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfSectionTableEntry.setDataPrototype("SgUnsignedCharList", "extra", "",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    /*************************************************************************************************************************
     *                                         ELF Segment Tables
     * The ELF Segment Table is an ELF Section that has entries describing the various segments of the ELF file.  Each segment
     * is also an SgAsmElfSection and the entries of the ELF Segment Table are associated with the SgAsmElfSection they
     * describe.  The ELF Segment Table can be reconstructed by traversing the AST and finding the SgAsmElfSegmentTableEntry
     * nodes.
     *************************************************************************************************************************/

    NEW_TERMINAL_MACRO(AsmElfSegmentTable, "AsmElfSegmentTable", "AsmElfSegmentTableTag");
    AsmElfSegmentTable.setFunctionPrototype("HEADER_ELF_SEGMENT_TABLE", "../Grammar/BinaryInstruction.code");



    NEW_TERMINAL_MACRO(AsmElfSegmentTableEntryList, "AsmElfSegmentTableEntryList", "AsmElfSegmentTableEntryListTag");
    AsmElfSegmentTableEntryList.setDataPrototype("SgAsmElfSegmentTableEntryPtrList", "entries", "",
                                                 NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmElfSegmentTableEntry, "AsmElfSegmentTableEntry", "AsmElfSegmentTableEntryTag");
    AsmElfSegmentTableEntry.setFunctionPrototype("HEADER_ELF_SEGMENT_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
    AsmElfSegmentTableEntry.setFunctionSource("SOURCE_ELF_SEGMENT_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
    AsmElfSegmentTableEntry.setDataPrototype("size_t", "index", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfSegmentTableEntry.setDataPrototype("SgAsmElfSegmentTableEntry::SegmentType", "type",
                                             "= SgAsmElfSegmentTableEntry::PT_LOAD",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfSegmentTableEntry.setDataPrototype("SgAsmElfSegmentTableEntry::SegmentFlags", "flags",
                                             "= SgAsmElfSegmentTableEntry::PF_NONE",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfSegmentTableEntry.setDataPrototype("rose_addr_t", "offset", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfSegmentTableEntry.setDataPrototype("rose_addr_t", "vaddr", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfSegmentTableEntry.setDataPrototype("rose_addr_t", "paddr", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfSegmentTableEntry.setDataPrototype("rose_addr_t", "filesz", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfSegmentTableEntry.setDataPrototype("rose_addr_t", "memsz", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfSegmentTableEntry.setDataPrototype("rose_addr_t", "align", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfSegmentTableEntry.setDataPrototype("SgUnsignedCharList", "extra", "",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    /*************************************************************************************************************************
     *                                         ELF Symbol Tables
     *************************************************************************************************************************/

    NEW_TERMINAL_MACRO(AsmElfSymbolSection, "AsmElfSymbolSection", "AsmElfSymbolSectionTag");
    AsmElfSymbolSection.setFunctionPrototype("HEADER_ELF_SYMBOL_SECTION", "../Grammar/BinaryInstruction.code");
    AsmElfSymbolSection.setDataPrototype("SgAsmElfSymbolList*", "symbols", "= NULL",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AsmElfSymbolSection.setDataPrototype("bool", "is_dynamic", "= false",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmElfSymbolList, "AsmElfSymbolList", "AsmElfSymbolListTag");
    AsmElfSymbolList.setDataPrototype("SgAsmElfSymbolPtrList", "symbols", "",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmElfSymbol, "AsmElfSymbol", "AsmElfSymbolTag");
    AsmElfSymbol.setFunctionPrototype("HEADER_ELF_SYMBOL", "../Grammar/BinaryInstruction.code");
    AsmElfSymbol.setDataPrototype("unsigned char", "st_info", "= 0",
                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfSymbol.setDataPrototype("unsigned char", "st_res1", "= 0",
                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfSymbol.setDataPrototype("unsigned", "st_shndx", "= 0",
                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfSymbol.setDataPrototype("rose_addr_t", "st_size", "= 0",
                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfSymbol.setDataPrototype("SgUnsignedCharList", "extra", "",
                          NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    /*************************************************************************************************************************
     *                                         ELF Symbol Version Tables
     *************************************************************************************************************************/

    // The ELF Symbol Version Table (.gnu.version section).  The section points to an SgAsmElfSymverEntryPtrList containing a
    // list of SgAsmElfSymverEntry objects.
    NEW_TERMINAL_MACRO(AsmElfSymverSection, "AsmElfSymverSection", "AsmElfSymverSection");
    AsmElfSymverSection.setFunctionPrototype("HEADER_ELF_SYMVER_SECTION", "../Grammar/BinaryInstruction.code");
    AsmElfSymverSection.setDataPrototype("SgAsmElfSymverEntryList*", "entries", "= NULL",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmElfSymverEntry, "AsmElfSymverEntry", "AsmElfSymverEntryTag");
    AsmElfSymverEntry.setFunctionPrototype("HEADER_ELF_SYMVER_ENTRY", "../Grammar/BinaryInstruction.code");
    AsmElfSymverEntry.setDataPrototype("size_t", "value", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmElfSymverEntryList, "AsmElfSymverEntryList", "AsmElfSymverEntryListTag");
    AsmElfSymverEntryList.setDataPrototype("SgAsmElfSymverEntryPtrList", "entries", "",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    /* The GNU Symbol Version Definitions (.gnu.version_d section).  The SgAsmElfSymverDefinedSection points to a list of
     * entries (SgAsmSymverDefinedEntry), which in turn point to a list of auxilliary members (SgAsmSymverDefinedAux). See
     * SgAsmSymverDefinedSection::parse() for a good description of the disk format. */
    NEW_TERMINAL_MACRO(AsmElfSymverDefinedSection, "AsmElfSymverDefinedSection", "AsmElfSymverDefinedSection");
    AsmElfSymverDefinedSection.setFunctionPrototype("HEADER_ELF_SYMVER_DEFINED_SECTION", "../Grammar/BinaryInstruction.code");
    AsmElfSymverDefinedSection.setDataPrototype("SgAsmElfSymverDefinedEntryList*", "entries", "= NULL",
                                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmElfSymverDefinedEntryList, "AsmElfSymverDefinedEntryList", "AsmElfSymverDefinedEntryListTag");
    AsmElfSymverDefinedEntryList.setDataPrototype("SgAsmElfSymverDefinedEntryPtrList", "entries", "",
                                                  NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL,
                                                  NO_DELETE);



    NEW_TERMINAL_MACRO(AsmElfSymverDefinedEntry, "AsmElfSymverDefinedEntry", "AsmElfSymverDefinedEntryTag");
    AsmElfSymverDefinedEntry.setFunctionPrototype("HEADER_ELF_SYMVER_DEFINED_ENTRY", "../Grammar/BinaryInstruction.code");
    AsmElfSymverDefinedEntry.setDataPrototype("size_t", "version", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfSymverDefinedEntry.setDataPrototype("int", "flags", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfSymverDefinedEntry.setDataPrototype("size_t", "index", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfSymverDefinedEntry.setDataPrototype("uint32_t", "hash", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfSymverDefinedEntry.setDataPrototype("SgAsmElfSymverDefinedAuxList*", "entries", "",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmElfSymverDefinedAuxList, "AsmElfSymverDefinedAuxList", "AsmElfSymverDefinedAuxListTag");
    AsmElfSymverDefinedAuxList.setDataPrototype("SgAsmElfSymverDefinedAuxPtrList", "entries", "",
                                                NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmElfSymverDefinedAux, "AsmElfSymverDefinedAux", "AsmElfSymverDefinedAuxTag");
    AsmElfSymverDefinedAux.setFunctionPrototype("HEADER_ELF_SYMVER_DEFINED_AUX", "../Grammar/BinaryInstruction.code");
    AsmElfSymverDefinedAux.setDataPrototype("SgAsmGenericString*", "name", "= 0",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    /* The GNU Symbol Version Requirements Table (.gnu.version_r section).  The format of this table is similar to the GNU
     * Symbol Version Definitions Table described above, namey that the table object (SgAsmElfSymverNeededSection) points to
     * a list of entries (SgAsmElfSymverNeededEntry), which of which point to a list of auxilliary information
     * (SgAsmElfSymverNeededAux). */
    NEW_TERMINAL_MACRO(AsmElfSymverNeededSection, "AsmElfSymverNeededSection", "AsmElfSymverNeededSection");
    AsmElfSymverNeededSection.setFunctionPrototype("HEADER_ELF_SYMVER_NEEDED_SECTION", "../Grammar/BinaryInstruction.code");
    AsmElfSymverNeededSection.setDataPrototype("SgAsmElfSymverNeededEntryList*", "entries", "= NULL",
                                               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmElfSymverNeededEntryList, "AsmElfSymverNeededEntryList", "AsmElfSymverNeededEntryListTag");
    AsmElfSymverNeededEntryList.setDataPrototype("SgAsmElfSymverNeededEntryPtrList", "entries", "",
                                                 NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmElfSymverNeededEntry, "AsmElfSymverNeededEntry", "AsmElfSymverNeededEntryTag");
    AsmElfSymverNeededEntry.setFunctionPrototype("HEADER_ELF_SYMVER_NEEDED_ENTRY", "../Grammar/BinaryInstruction.code");
    AsmElfSymverNeededEntry.setDataPrototype("size_t", "version", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfSymverNeededEntry.setDataPrototype("SgAsmGenericString*", "file_name", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AsmElfSymverNeededEntry.setDataPrototype("SgAsmElfSymverNeededAuxList*", "entries", "",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmElfSymverNeededAuxList, "AsmElfSymverNeededAuxList", "AsmElfSymverNeededAuxListTag");
    AsmElfSymverNeededAuxList.setDataPrototype("SgAsmElfSymverNeededAuxPtrList", "entries", "",
                                               NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmElfSymverNeededAux, "AsmElfSymverNeededAux", "AsmElfSymverNeededAuxTag");
    AsmElfSymverNeededAux.setFunctionPrototype("HEADER_ELF_SYMVER_NEEDED_AUX", "../Grammar/BinaryInstruction.code");
    AsmElfSymverNeededAux.setDataPrototype("uint32_t", "hash", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfSymverNeededAux.setDataPrototype("int", "flags", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfSymverNeededAux.setDataPrototype("size_t", "other", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfSymverNeededAux.setDataPrototype("SgAsmGenericString*", "name", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    /*************************************************************************************************************************
     *                                         ELF Relocation Tables
     *************************************************************************************************************************/

    NEW_TERMINAL_MACRO(AsmElfRelocSection, "AsmElfRelocSection", "AsmElfRelocSectionTag");
    AsmElfRelocSection.setFunctionPrototype("HEADER_ELF_RELOC_SECTION", "../Grammar/BinaryInstruction.code");
    AsmElfRelocSection.setDataPrototype("bool", "uses_addend", "= true",
                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfRelocSection.setDataPrototype("SgAsmElfSection*", "target_section", "= NULL",
                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfRelocSection.setDataPrototype("SgAsmElfRelocEntryList*", "entries", "= NULL",
                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmElfRelocEntryList, "AsmElfRelocEntryList", "AsmElfRelocEntryListTag");
    AsmElfRelocEntryList.setDataPrototype("SgAsmElfRelocEntryPtrList", "entries", "",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmElfRelocEntry, "AsmElfRelocEntry", "AsmElfRelocEntryTag");
    AsmElfRelocEntry.setFunctionPrototype("HEADER_ELF_RELOC_ENTRY", "../Grammar/BinaryInstruction.code");
    AsmElfRelocEntry.setDataPrototype("rose_addr_t", "r_offset", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfRelocEntry.setDataPrototype("rose_addr_t", "r_addend", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfRelocEntry.setDataPrototype("unsigned long", "sym", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfRelocEntry.setDataPrototype("SgAsmElfRelocEntry::RelocType", "type", "= R_386_NONE",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfRelocEntry.setDataPrototype("SgUnsignedCharList", "extra", "",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    /*************************************************************************************************************************
     *                                         ELF Dynamic Linking
     *************************************************************************************************************************/

    NEW_TERMINAL_MACRO(AsmElfDynamicSection, "AsmElfDynamicSection", "AsmElfDynamicSectionTag");
    AsmElfDynamicSection.setFunctionPrototype("HEADER_ELF_DYNAMIC_SECTION", "../Grammar/BinaryInstruction.code");
    AsmElfDynamicSection.setDataPrototype("SgAsmElfDynamicEntryList*", "entries", "= NULL",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmElfDynamicEntryList, "AsmElfDynamicEntryList", "AsmElfDynamicEntryListTag");
    AsmElfDynamicEntryList.setDataPrototype("SgAsmElfDynamicEntryPtrList", "entries", "",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmElfDynamicEntry, "AsmElfDynamicEntry", "AsmElfDynamicEntryTag");
    AsmElfDynamicEntry.setFunctionPrototype("HEADER_ELF_DYNAMIC_ENTRY", "../Grammar/BinaryInstruction.code");
    AsmElfDynamicEntry.setDataPrototype("SgAsmElfDynamicEntry::EntryType", "d_tag", "= SgAsmElfDynamicEntry::DT_NULL",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfDynamicEntry.setDataPrototype("rose_rva_t", "d_val", "",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfDynamicEntry.setDataPrototype("SgAsmGenericString*", "name", "= NULL",
                                        NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AsmElfDynamicEntry.setDataPrototype("SgUnsignedCharList", "extra", "",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    /*************************************************************************************************************************
     *                                         ELF String Tables
     *************************************************************************************************************************/

    // String table section
    NEW_TERMINAL_MACRO(AsmElfStringSection, "AsmElfStringSection", "AsmElfStringSectionTag");
    AsmElfStringSection.setFunctionPrototype("HEADER_ELF_STRING_SECTION", "../Grammar/BinaryInstruction.code");
    AsmElfStringSection.setFunctionSource("SOURCE_ELF_STRING_SECTION", "../Grammar/BinaryInstruction.code");
    AsmElfStringSection.setAutomaticGenerationOfDestructor(false);
    AsmElfStringSection.setDataPrototype("SgAsmElfStrtab*", "strtab", "= NULL",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmElfStrtab, "AsmElfStrtab", "AsmElfStrtabTag");
    AsmElfStrtab.setFunctionPrototype("HEADER_ELF_STRING_TABLE", "../Grammar/BinaryInstruction.code");
    AsmElfStrtab.setAutomaticGenerationOfDestructor(false);



    /*************************************************************************************************************************
     *                                         ELF Notes
     *************************************************************************************************************************/

    NEW_TERMINAL_MACRO(AsmElfNoteSection, "AsmElfNoteSection", "AsmElfNoteSection");
    AsmElfNoteSection.setFunctionPrototype("HEADER_ELF_NOTE_SECTION", "../Grammar/BinaryInstruction.code");
    AsmElfNoteSection.setDataPrototype("SgAsmElfNoteEntryList*", "entries", "= NULL",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmElfNoteEntryList, "AsmElfNoteEntryList", "AsmElfNoteEntryListTag");
    AsmElfNoteEntryList.setDataPrototype("SgAsmElfNoteEntryPtrList", "entries", "",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmElfNoteEntry, "AsmElfNoteEntry", "AsmElfNoteEntryTag");
    AsmElfNoteEntry.setFunctionPrototype("HEADER_ELF_NOTE_ENTRY", "../Grammar/BinaryInstruction.code");
    AsmElfNoteEntry.setDataPrototype("unsigned", "type", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfNoteEntry.setDataPrototype("SgAsmGenericString*", "name", "= NULL",
                                     NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AsmElfNoteEntry.setDataPrototype("SgUnsignedCharList", "payload", "",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    /*************************************************************************************************************************
     *                                         ELF Exception Handling
     *************************************************************************************************************************/

    NEW_TERMINAL_MACRO(AsmElfEHFrameSection, "AsmElfEHFrameSection", "AsmElfEHFrameSection");
    AsmElfEHFrameSection.setFunctionPrototype("HEADER_ELF_EH_FRAME_SECTION", "../Grammar/BinaryInstruction.code");
    AsmElfEHFrameSection.setDataPrototype("SgAsmElfEHFrameEntryCIList*", "ci_entries", "= NULL",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmElfEHFrameEntryCIList, "AsmElfEHFrameEntryCIList", "AsmElfEHFrameEntryCIListTag");
    AsmElfEHFrameEntryCIList.setDataPrototype("SgAsmElfEHFrameEntryCIPtrList", "entries", "",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    // ELF Error Handling Frame Entry, Common Information Entry Format
    NEW_TERMINAL_MACRO(AsmElfEHFrameEntryCI, "AsmElfEHFrameEntryCI", "AsmElfEHFrameEntryCITag");
    AsmElfEHFrameEntryCI.setFunctionPrototype("HEADER_ELF_EH_FRAME_ENTRY_CI", "../Grammar/BinaryInstruction.code");
    AsmElfEHFrameEntryCI.setDataPrototype("int", "version", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfEHFrameEntryCI.setDataPrototype("std::string", "augmentation_string", "= \"\"",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfEHFrameEntryCI.setDataPrototype("uint64_t", "eh_data", "=0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfEHFrameEntryCI.setDataPrototype("uint64_t", "code_alignment_factor", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfEHFrameEntryCI.setDataPrototype("int64_t", "data_alignment_factor", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfEHFrameEntryCI.setDataPrototype("uint64_t", "augmentation_data_length", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfEHFrameEntryCI.setDataPrototype("int", "lsda_encoding", "= -1",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfEHFrameEntryCI.setDataPrototype("int", "prh_encoding", "= -1",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfEHFrameEntryCI.setDataPrototype("unsigned", "prh_arg", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfEHFrameEntryCI.setDataPrototype("rose_addr_t", "prh_addr", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfEHFrameEntryCI.setDataPrototype("int", "addr_encoding", "= -1",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfEHFrameEntryCI.setDataPrototype("bool", "sig_frame", "= false",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfEHFrameEntryCI.setDataPrototype("SgUnsignedCharList", "instructions", "",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfEHFrameEntryCI.setDataPrototype("SgAsmElfEHFrameEntryFDList*", "fd_entries", "= NULL",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmElfEHFrameEntryFDList, "AsmElfEHFrameEntryFDList", "AsmElfEHFrameEntryFDListTag");
    AsmElfEHFrameEntryFDList.setDataPrototype("SgAsmElfEHFrameEntryFDPtrList", "entries", "",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    /* ELF Error Handling Frame Entry, Frame Description Entry Format */
    NEW_TERMINAL_MACRO(AsmElfEHFrameEntryFD, "AsmElfEHFrameEntryFD", "AsmElfEHFrameEntryFDTag");
    AsmElfEHFrameEntryFD.setFunctionPrototype("HEADER_ELF_EH_FRAME_ENTRY_FD", "../Grammar/BinaryInstruction.code");
    AsmElfEHFrameEntryFD.setDataPrototype("rose_rva_t", "begin_rva", "",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfEHFrameEntryFD.setDataPrototype("rose_addr_t", "size", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfEHFrameEntryFD.setDataPrototype("SgUnsignedCharList", "augmentation_data", "",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfEHFrameEntryFD.setDataPrototype("SgUnsignedCharList", "instructions", "",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    /*************************************************************************************************************************
     *                                         ELF Sections
     *************************************************************************************************************************/

    NEW_NONTERMINAL_MACRO(AsmElfSection,
                          AsmElfSymbolSection | AsmElfRelocSection         | AsmElfDynamicSection       |
                          AsmElfStringSection | AsmElfNoteSection          | AsmElfEHFrameSection       |
                          AsmElfSymverSection | AsmElfSymverDefinedSection | AsmElfSymverNeededSection,
                          "AsmElfSection", "AsmElfSectionTag", true);
    AsmElfSection.setFunctionPrototype("HEADER_ELF_SECTION", "../Grammar/BinaryInstruction.code");
    AsmElfSection.setFunctionSource("SOURCE_ELF_SECTION", "../Grammar/BinaryInstruction.code");
    AsmElfSection.setDataPrototype("SgAsmElfSection*", "linked_section", "= NULL", //accessors must be virtual
                                   NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmElfSection.setDataPrototype("SgAsmElfSectionTableEntry*", "section_entry", "= NULL",
                                   NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AsmElfSection.setDataPrototype("SgAsmElfSegmentTableEntry*", "segment_entry", "= NULL",
                                   NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    /*************************************************************************************************************************
     *                                         DOS File Header
     *************************************************************************************************************************/

    NEW_TERMINAL_MACRO(AsmDOSFileHeader, "AsmDOSFileHeader", "AsmDOSFileHeaderTag");
    AsmDOSFileHeader.setFunctionPrototype("HEADER_DOS_HEADER", "../Grammar/BinaryInstruction.code");
    AsmDOSFileHeader.setDataPrototype("uint16_t", "e_last_page_size", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDOSFileHeader.setDataPrototype("uint16_t", "e_total_pages", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDOSFileHeader.setDataPrototype("uint16_t", "e_nrelocs", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDOSFileHeader.setDataPrototype("uint16_t", "e_header_paragraphs", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDOSFileHeader.setDataPrototype("uint16_t", "e_minalloc", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDOSFileHeader.setDataPrototype("uint16_t", "e_maxalloc", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDOSFileHeader.setDataPrototype("uint16_t", "e_ss", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDOSFileHeader.setDataPrototype("uint16_t", "e_sp", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDOSFileHeader.setDataPrototype("uint16_t", "e_cksum", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDOSFileHeader.setDataPrototype("uint16_t", "e_ip", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDOSFileHeader.setDataPrototype("uint16_t", "e_cs", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDOSFileHeader.setDataPrototype("uint16_t", "e_overlay", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDOSFileHeader.setDataPrototype("rose_addr_t", "e_relocs_offset", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDOSFileHeader.setDataPrototype("unsigned", "e_res1", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDOSFileHeader.setDataPrototype("SgAsmGenericSection*", "relocs", "= NULL", //relocation info
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AsmDOSFileHeader.setDataPrototype("SgAsmGenericSection*", "rm_section", "= NULL", //real-mode section
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmDOSExtendedHeader, "AsmDOSExtendedHeader", "AsmDOSExtendedHeaderTag");
    AsmDOSExtendedHeader.setFunctionPrototype("HEADER_DOS_EXTENDED_HEADER", "../Grammar/BinaryInstruction.code");
    AsmDOSExtendedHeader.setDataPrototype("unsigned", "e_res1", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDOSExtendedHeader.setDataPrototype("unsigned", "e_oemid", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDOSExtendedHeader.setDataPrototype("unsigned", "e_oeminfo", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDOSExtendedHeader.setDataPrototype("unsigned", "e_res2", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDOSExtendedHeader.setDataPrototype("unsigned", "e_res3", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDOSExtendedHeader.setDataPrototype("unsigned", "e_res4", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDOSExtendedHeader.setDataPrototype("unsigned", "e_res5", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDOSExtendedHeader.setDataPrototype("unsigned", "e_res6", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDOSExtendedHeader.setDataPrototype("rose_addr_t", "e_lfanew", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    /*************************************************************************************************************************
     *                                         PE File Header
     *************************************************************************************************************************/

    NEW_TERMINAL_MACRO(AsmPEFileHeader, "AsmPEFileHeader", "AsmPEFileHeaderTag");
    AsmPEFileHeader.setFunctionPrototype("HEADER_PE_FILE_HEADER", "../Grammar/BinaryInstruction.code");
    AsmPEFileHeader.setDataPrototype("unsigned", "e_cpu_type", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("unsigned", "e_nsections", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("unsigned", "e_time", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("rose_addr_t", "e_coff_symtab", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("rose_addr_t", "e_nt_hdr_size", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("unsigned", "e_coff_nsyms", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("unsigned", "e_flags", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("unsigned", "e_opt_magic", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("unsigned", "e_lmajor", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("unsigned", "e_lminor", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("unsigned", "e_code_size", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("unsigned", "e_data_size", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("unsigned", "e_bss_size", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("rose_rva_t", "e_code_rva", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("rose_rva_t", "e_data_rva", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("unsigned", "e_section_align", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("unsigned", "e_file_align", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("unsigned", "e_os_major", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("unsigned", "e_os_minor", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("unsigned", "e_user_major", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("unsigned", "e_user_minor", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("unsigned", "e_subsys_major", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("unsigned", "e_subsys_minor", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("unsigned", "e_reserved9", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("unsigned", "e_image_size", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("unsigned", "e_header_size", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("unsigned", "e_file_checksum", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("unsigned", "e_subsystem", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("unsigned", "e_dll_flags", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("unsigned", "e_stack_reserve_size", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("unsigned", "e_stack_commit_size", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("unsigned", "e_heap_reserve_size", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("unsigned", "e_heap_commit_size", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("unsigned", "e_loader_flags", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("unsigned", "e_num_rvasize_pairs", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("SgAsmPERVASizePairList*", "rvasize_pairs", "= NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("SgAsmPESectionTable*", "section_table", "= NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEFileHeader.setDataPrototype("SgAsmCoffSymbolTable*", "coff_symtab", "= NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmPERVASizePairList, "AsmPERVASizePairList", "AsmPERVASizePairListTag");
    AsmPERVASizePairList.setFunctionPrototype("HEADER_PE_RVA_SIZE_PAIR_LIST", "../Grammar/BinaryInstruction.code");
    AsmPERVASizePairList.setDataPrototype("SgAsmPERVASizePairPtrList", "pairs", "",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmPERVASizePair, "AsmPERVASizePair", "AsmPERVASizePairTag");
    AsmPERVASizePair.setFunctionPrototype("HEADER_PE_RVA_SIZE_PAIR", "../Grammar/BinaryInstruction.code");
    AsmPERVASizePair.setDataPrototype("rose_rva_t", "e_rva", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPERVASizePair.setDataPrototype("rose_addr_t", "e_size", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPERVASizePair.setDataPrototype("SgAsmGenericSection*", "section", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    /*************************************************************************************************************************
     *                                         PE Import Tables
     *************************************************************************************************************************/

    // A single imported object.  Each of these nodes is created from a combination of the PE Import Lookup Table and the PE
    // Import Address Table, pointers to which are stored in the Import Directory (SgAsmPEImportDirectory).
    NEW_TERMINAL_MACRO(AsmPEImportItem, "AsmPEImportItem", "AsmPEImportItemTag");
    AsmPEImportItem.setFunctionPrototype("HEADER_PE_IMPORT_ITEM", "../Grammar/BinaryInstruction.code");
    AsmPEImportItem.setDataPrototype("bool", "by_ordinal", "=true",      // store ordinal number if ILT/IAT?
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEImportItem.setDataPrototype("unsigned", "ordinal", "=0",        // ordinal number
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEImportItem.setDataPrototype("unsigned", "hint", "=0",           // export name table hint if not by_ordinal
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEImportItem.setDataPrototype("SgAsmGenericString*", "name", "",  // name if not by ordinal
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AsmPEImportItem.setDataPrototype("rose_rva_t", "hintname_rva", "=0", // rva of hint/name pair
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEImportItem.setDataPrototype("size_t", "hintname_nalloc", "=0",  // file bytes allocated for hint/name pair
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEImportItem.setDataPrototype("rose_rva_t", "bound_rva", "=0",    // bound rva from IAT if not a copy of ILT
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    // A list of imported items.
    NEW_TERMINAL_MACRO(AsmPEImportItemList, "AsmPEImportItemList", "AsmPEImportItemListTag");
    AsmPEImportItemList.setDataPrototype("SgAsmPEImportItemPtrList", "vector", "",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    // One import directory per library.  The import directory points to the Import Lookup Table and Import Address Table.
    // ROSE no longer represents the ILT and IAT with separate IR nodes.
    NEW_TERMINAL_MACRO(AsmPEImportDirectory, "AsmPEImportDirectory", "AsmPEImportDirectoryTag");
    AsmPEImportDirectory.setFunctionPrototype("HEADER_PE_IMPORT_DIRECTORY", "../Grammar/BinaryInstruction.code");
    AsmPEImportDirectory.setDataPrototype("SgAsmGenericString*", "dll_name", "=0", // name of library from which to import
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AsmPEImportDirectory.setDataPrototype("rose_rva_t", "dll_name_rva", "= 0", // address of the name
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEImportDirectory.setDataPrototype("size_t", "dll_name_nalloc", "=0",   // bytes allocated in the file for the name
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEImportDirectory.setDataPrototype("time_t", "time", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEImportDirectory.setDataPrototype("unsigned", "forwarder_chain", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEImportDirectory.setDataPrototype("rose_rva_t", "ilt_rva", "= 0", // starting address of the ILT
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEImportDirectory.setDataPrototype("size_t", "ilt_nalloc", "= 0",  // bytes allocated in the file for the ILT
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEImportDirectory.setDataPrototype("rose_rva_t", "iat_rva", "= 0", // usually in the .rdata section
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEImportDirectory.setDataPrototype("size_t", "iat_nalloc", "=0",   // bytes allocated in the file for the IAT
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEImportDirectory.setDataPrototype("SgAsmPEImportItemList*", "imports", "=NULL",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);


    // A list of PE Import Directories.
    NEW_TERMINAL_MACRO(AsmPEImportDirectoryList, "AsmPEImportDirectoryList", "AsmPEImportDirectoryListTag");
    AsmPEImportDirectoryList.setDataPrototype("SgAsmPEImportDirectoryPtrList", "vector", "",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    // A file section containing a list of PE Import Directories.  Documentation is in PeImportSection.C */
    NEW_TERMINAL_MACRO(AsmPEImportSection, "AsmPEImportSection", "AsmPEImportSectionTag");
    AsmPEImportSection.setFunctionPrototype("HEADER_PE_IMPORT_SECTION", "../Grammar/BinaryInstruction.code");
    AsmPEImportSection.setDataPrototype ("SgAsmPEImportDirectoryList*", "import_directories", "= NULL",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    /*************************************************************************************************************************
     *                                         PE Export Tables
     *************************************************************************************************************************/

    NEW_TERMINAL_MACRO(AsmPEExportSection, "AsmPEExportSection", "AsmPEExportSectionTag");
    AsmPEExportSection.setFunctionPrototype("HEADER_PE_EXPORT_SECTION", "../Grammar/BinaryInstruction.code");
    AsmPEExportSection.setDataPrototype("SgAsmPEExportDirectory*", "export_dir", "= NULL",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AsmPEExportSection.setDataPrototype("SgAsmPEExportEntryList*", "exports", "= NULL",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmPEExportDirectory, "AsmPEExportDirectory", "AsmPEExportDirectoryTag");
    AsmPEExportDirectory.setFunctionPrototype("HEADER_PE_EXPORT_DIRECTORY", "../Grammar/BinaryInstruction.code");
    AsmPEExportDirectory.setDataPrototype("unsigned", "res1", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEExportDirectory.setDataPrototype("time_t", "timestamp", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEExportDirectory.setDataPrototype("unsigned", "vmajor", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEExportDirectory.setDataPrototype("unsigned", "vminor", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEExportDirectory.setDataPrototype("rose_rva_t", "name_rva", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEExportDirectory.setDataPrototype("unsigned", "ord_base", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEExportDirectory.setDataPrototype("size_t", "expaddr_n", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEExportDirectory.setDataPrototype("size_t", "nameptr_n", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEExportDirectory.setDataPrototype("rose_rva_t", "expaddr_rva", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEExportDirectory.setDataPrototype("rose_rva_t", "nameptr_rva", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEExportDirectory.setDataPrototype("rose_rva_t", "ordinals_rva", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEExportDirectory.setDataPrototype("SgAsmGenericString*", "name", "= NULL",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmPEExportEntryList, "AsmPEExportEntryList", "AsmPEExportEntryListTag");
    AsmPEExportEntryList.setDataPrototype("SgAsmPEExportEntryPtrList", "exports", "",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmPEExportEntry, "AsmPEExportEntry", "AsmPEExportEntryTag");
    AsmPEExportEntry.setFunctionPrototype("HEADER_PE_EXPORT_ENTRY", "../Grammar/BinaryInstruction.code");
    AsmPEExportEntry.setDataPrototype("SgAsmGenericString*", "name", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AsmPEExportEntry.setDataPrototype("unsigned", "ordinal", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEExportEntry.setDataPrototype("rose_rva_t", "export_rva", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPEExportEntry.setDataPrototype("SgAsmGenericString*", "forwarder", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    /*************************************************************************************************************************
     *                                         PE String Tables
     *************************************************************************************************************************/

    NEW_TERMINAL_MACRO(AsmPEStringSection, "AsmPEStringSection", "AsmPEStringSectionTag");
    AsmPEStringSection.setFunctionPrototype("HEADER_PE_STRING_SECTION", "../Grammar/BinaryInstruction.code");
    AsmPEStringSection.setAutomaticGenerationOfDestructor(false);
    AsmPEStringSection.setDataPrototype("SgAsmCoffStrtab*", "strtab", "= NULL",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    /*************************************************************************************************************************
     *                                         PE Section Tables
     *************************************************************************************************************************/

    NEW_TERMINAL_MACRO(AsmPESectionTable, "AsmPESectionTable", "AsmPESectionTableTag");
    AsmPESectionTable.setFunctionPrototype("HEADER_PE_SECTION_TABLE", "../Grammar/BinaryInstruction.code");



    NEW_TERMINAL_MACRO(AsmPESectionTableEntry, "AsmPESectionTableEntry", "AsmPESectionTableEntryTag");
    AsmPESectionTableEntry.setFunctionPrototype("HEADER_PE_SECTION_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
    AsmPESectionTableEntry.setFunctionSource("SOURCE_PE_SECTION_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
    AsmPESectionTableEntry.setDataPrototype("std::string", "name", "= \"\"",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPESectionTableEntry.setDataPrototype("rose_addr_t", "virtual_size", "= 0",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPESectionTableEntry.setDataPrototype("rose_addr_t", "rva", "= 0",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPESectionTableEntry.setDataPrototype("rose_addr_t", "physical_size", "= 0",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPESectionTableEntry.setDataPrototype("rose_addr_t", "physical_offset", "= 0",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPESectionTableEntry.setDataPrototype("unsigned", "coff_line_nums", "= 0",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPESectionTableEntry.setDataPrototype("unsigned", "n_relocs", "= 0",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPESectionTableEntry.setDataPrototype("unsigned", "n_coff_line_nums", "= 0",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmPESectionTableEntry.setDataPrototype("unsigned", "flags", "= 0",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    /*************************************************************************************************************************
     *                                         PE Sections
     *************************************************************************************************************************/

    NEW_NONTERMINAL_MACRO(AsmPESection,
                          AsmPEImportSection | AsmPEExportSection | AsmPEStringSection,
                          "AsmPESection", "AsmPESectionTag", true);
    AsmPESection.setFunctionPrototype("HEADER_PE_SECTION", "../Grammar/BinaryInstruction.code");
    AsmPESection.setDataPrototype("SgAsmPESectionTableEntry*", "section_entry", "= NULL",
                                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    /*************************************************************************************************************************
     *                                         COFF Symbol Tables
     * The COFF format is used by Microsoft PE files.
     *************************************************************************************************************************/

    NEW_TERMINAL_MACRO(AsmCoffSymbolTable, "AsmCoffSymbolTable", "AsmCoffSymbolTableTag");
    AsmCoffSymbolTable.setFunctionPrototype("HEADER_PE_COFF_SYMBOL_TABLE", "../Grammar/BinaryInstruction.code");
    AsmCoffSymbolTable.setDataPrototype("SgAsmGenericSection*", "strtab", "= NULL",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmCoffSymbolTable.setDataPrototype("SgAsmCoffSymbolList*", "symbols", "= NULL",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmCoffSymbolList, "AsmCoffSymbolList", "AsmCoffSymbolListTag");
    AsmCoffSymbolList.setDataPrototype("SgAsmCoffSymbolPtrList", "symbols", "",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmCoffSymbol, "AsmCoffSymbol", "AsmCoffSymbolTag");
    AsmCoffSymbol.setFunctionPrototype("HEADER_PE_COFF_SYMBOL", "../Grammar/BinaryInstruction.code");
    AsmCoffSymbol.setFunctionSource("SOURCE_PE_COFF_SYMBOL", "../Grammar/BinaryInstruction.code");
    AsmCoffSymbol.setDataPrototype("std::string", "st_name", "= \"\"",
                                   NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmCoffSymbol.setDataPrototype("rose_addr_t", "st_name_offset", "= 0",
                                   NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmCoffSymbol.setDataPrototype("int", "st_section_num", "= 0",
                                   NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmCoffSymbol.setDataPrototype("unsigned", "st_type", "= 0",
                                   NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmCoffSymbol.setDataPrototype("unsigned", "st_storage_class", "= 0",
                                   NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmCoffSymbol.setDataPrototype("unsigned", "st_num_aux_entries", "= 0",
                                   NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmCoffSymbol.setDataPrototype("SgUnsignedCharList", "aux_data", "",
                                   NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmCoffStrtab, "AsmCoffStrtab", "AsmCoffStrtabTag");
    AsmCoffStrtab.setFunctionPrototype("HEADER_COFF_STRING_TABLE", "../Grammar/BinaryInstruction.code");
    AsmCoffStrtab.setAutomaticGenerationOfDestructor(false);



    /*************************************************************************************************************************
     *                                         NE File Header
     *************************************************************************************************************************/

    NEW_TERMINAL_MACRO(AsmNEFileHeader, "AsmNEFileHeader", "AsmNEFileHeaderTag");
    AsmNEFileHeader.setFunctionPrototype("HEADER_NE_FILE_HEADER", "../Grammar/BinaryInstruction.code");
    AsmNEFileHeader.setDataPrototype("unsigned", "e_linker_major", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNEFileHeader.setDataPrototype("unsigned", "e_linker_minor", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNEFileHeader.setDataPrototype("unsigned", "e_checksum", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNEFileHeader.setDataPrototype("unsigned", "e_flags1", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNEFileHeader.setDataPrototype("unsigned", "e_autodata_sn", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNEFileHeader.setDataPrototype("unsigned", "e_bss_size", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNEFileHeader.setDataPrototype("unsigned", "e_stack_size", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNEFileHeader.setDataPrototype("unsigned", "e_csip", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNEFileHeader.setDataPrototype("unsigned", "e_sssp", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNEFileHeader.setDataPrototype("unsigned", "e_nsections", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNEFileHeader.setDataPrototype("unsigned", "e_nmodrefs", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNEFileHeader.setDataPrototype("unsigned", "e_nnonresnames", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNEFileHeader.setDataPrototype("unsigned", "e_nmovable_entries", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNEFileHeader.setDataPrototype("unsigned", "e_sector_align", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNEFileHeader.setDataPrototype("unsigned", "e_nresources", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNEFileHeader.setDataPrototype("unsigned", "e_exetype", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNEFileHeader.setDataPrototype("unsigned", "e_flags2", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNEFileHeader.setDataPrototype("unsigned", "e_res1", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNEFileHeader.setDataPrototype("unsigned", "e_winvers", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNEFileHeader.setDataPrototype("rose_addr_t", "e_entrytab_rfo", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNEFileHeader.setDataPrototype("rose_addr_t", "e_entrytab_size", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNEFileHeader.setDataPrototype("rose_addr_t", "e_sectab_rfo", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNEFileHeader.setDataPrototype("rose_addr_t", "e_rsrctab_rfo", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNEFileHeader.setDataPrototype("rose_addr_t", "e_resnametab_rfo", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNEFileHeader.setDataPrototype("rose_addr_t", "e_modreftab_rfo", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNEFileHeader.setDataPrototype("rose_addr_t", "e_importnametab_rfo", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNEFileHeader.setDataPrototype("rose_addr_t", "e_nonresnametab_offset", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNEFileHeader.setDataPrototype("rose_addr_t", "e_fastload_sector", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNEFileHeader.setDataPrototype("rose_addr_t", "e_fastload_nsectors", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNEFileHeader.setDataPrototype("SgAsmDOSExtendedHeader*", "dos2_header", "= NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AsmNEFileHeader.setDataPrototype("SgAsmNESectionTable*", "section_table", "= NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNEFileHeader.setDataPrototype("SgAsmNENameTable*", "resname_table", "= NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AsmNEFileHeader.setDataPrototype("SgAsmNENameTable*", "nonresname_table", "= NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AsmNEFileHeader.setDataPrototype("SgAsmNEModuleTable*", "module_table", "= NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AsmNEFileHeader.setDataPrototype("SgAsmNEEntryTable*", "entry_table", "= NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmNEEntryTable, "AsmNEEntryTable", "AsmNEEntryTableTag");
    AsmNEEntryTable.setFunctionPrototype("HEADER_NE_ENTRY_TABLE", "../Grammar/BinaryInstruction.code");
    AsmNEEntryTable.setDataPrototype("SgSizeTList", "bundle_sizes", "",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNEEntryTable.setDataPrototype("SgAsmNEEntryPointPtrList", "entries", "",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmNEEntryPoint, "AsmNEEntryPoint", "AsmNEEntryPointTag");
    AsmNEEntryPoint.setFunctionPrototype("HEADER_NE_ENTRY_POINT", "../Grammar/BinaryInstruction.code");
    AsmNEEntryPoint.setFunctionSource("SOURCE_NE_ENTRY_POINT", "../Grammar/BinaryInstruction.code");
    AsmNEEntryPoint.setDataPrototype("SgAsmNEEntryPoint::NEEntryFlags", "flags", "= SgAsmNEEntryPoint::EF_ZERO",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNEEntryPoint.setDataPrototype("unsigned", "int3f", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNEEntryPoint.setDataPrototype("unsigned", "section_idx", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNEEntryPoint.setDataPrototype("unsigned", "section_offset", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    /*************************************************************************************************************************
     *                                         NE Section Table
     *************************************************************************************************************************/

    NEW_TERMINAL_MACRO(AsmNESectionTable, "AsmNESectionTable", "AsmNESectionTableTag");
    AsmNESectionTable.setFunctionPrototype("HEADER_NE_SECTION_TABLE", "../Grammar/BinaryInstruction.code");
    AsmNESectionTable.setDataPrototype("unsigned", "flags", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNESectionTable.setDataPrototype("unsigned", "sector", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNESectionTable.setDataPrototype("rose_addr_t", "physical_size", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNESectionTable.setDataPrototype("rose_addr_t", "virtual_size", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmNESectionTableEntry, "AsmNESectionTableEntry", "AsmNESectionTableEntryTag");
    AsmNESectionTableEntry.setFunctionPrototype("HEADER_NE_SECTION_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
    AsmNESectionTableEntry.setFunctionSource("SOURCE_NE_SECTION_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
    AsmNESectionTableEntry.setDataPrototype("unsigned", "flags", "= 0",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNESectionTableEntry.setDataPrototype("unsigned", "sector", "= 0",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNESectionTableEntry.setDataPrototype("rose_addr_t", "physical_size", "= 0",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNESectionTableEntry.setDataPrototype("rose_addr_t", "virtual_size", "= 0",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    /*************************************************************************************************************************
     *                                         NE Relocation Table
     *************************************************************************************************************************/

    NEW_TERMINAL_MACRO(AsmNERelocTable, "AsmNERelocTable", "AsmNERelocTableTag");
    AsmNERelocTable.setFunctionPrototype("HEADER_NE_RELOC_TABLE", "../Grammar/BinaryInstruction.code");
    AsmNERelocTable.setDataPrototype("SgAsmNERelocEntryPtrList", "entries", "",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmNERelocEntry, "AsmNERelocEntry", "AsmNERelocEntryTag");
    AsmNERelocEntry.setFunctionPrototype("HEADER_NE_RELOC_ENTRY", "../Grammar/BinaryInstruction.code");
    AsmNERelocEntry.setFunctionSource("SOURCE_NE_RELOC_ENTRY", "../Grammar/BinaryInstruction.code");
    AsmNERelocEntry.setDataPrototype("SgAsmNERelocEntry::NERelocSrcType", "src_type", "= SgAsmNERelocEntry::RF_SRCTYPE_8OFF",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNERelocEntry.setDataPrototype("SgAsmNERelocEntry::NERelocModifiers", "modifier", "= SgAsmNERelocEntry::RF_MODIFIER_SINGLE",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNERelocEntry.setDataPrototype("SgAsmNERelocEntry::NERelocTgtType", "tgt_type", "= SgAsmNERelocEntry::RF_TGTTYPE_IREF",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNERelocEntry.setDataPrototype("SgAsmNERelocEntry::NERelocFlags", "flags", "= SgAsmNERelocEntry::RF_ADDITIVE",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNERelocEntry.setDataPrototype("rose_addr_t", "src_offset", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNERelocEntry.setDataPrototype("SgAsmNERelocEntry::iref_type", "iref", "",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNERelocEntry.setDataPrototype("SgAsmNERelocEntry::iord_type", "iord", "",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNERelocEntry.setDataPrototype("SgAsmNERelocEntry::iname_type", "iname", "",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNERelocEntry.setDataPrototype("SgAsmNERelocEntry::osfixup_type", "osfixup", "",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    /*************************************************************************************************************************
     *                                         NE Sections
     *************************************************************************************************************************/

    NEW_TERMINAL_MACRO(AsmNENameTable, "AsmNENameTable", "AsmNENameTableTag");
    AsmNENameTable.setFunctionPrototype("HEADER_NE_NAME_TABLE", "../Grammar/BinaryInstruction.code");
    AsmNENameTable.setDataPrototype("SgStringList", "names", "",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNENameTable.setDataPrototype("SgUnsignedList", "ordinals", "",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmNEModuleTable, "AsmNEModuleTable", "AsmNEModuleTableTag");
    AsmNEModuleTable.setFunctionPrototype("HEADER_NE_MODULE_TABLE", "../Grammar/BinaryInstruction.code");
    AsmNEModuleTable.setDataPrototype("SgAsmNEStringTable*", "strtab", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AsmNEModuleTable.setDataPrototype("SgAddressList", "name_offsets", "",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmNEModuleTable.setDataPrototype("SgStringList", "names", "",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmNEStringTable, "AsmNEStringTable", "AsmNEStringTableTag");
    AsmNEStringTable.setFunctionPrototype("HEADER_NE_STRING_TABLE", "../Grammar/BinaryInstruction.code");



    NEW_TERMINAL_MACRO(AsmNESection, "AsmNESection", "AsmNESectionTag");
    AsmNESection.setFunctionPrototype("HEADER_NE_SECTION", "../Grammar/BinaryInstruction.code");
    AsmNESection.setDataPrototype("SgAsmNESectionTableEntry*", "st_entry", "= NULL",
                                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AsmNESection.setDataPrototype("SgAsmNERelocTable*", "reloc_table", "= NULL",
                                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    /*************************************************************************************************************************
     *                                         LE File Header
     *************************************************************************************************************************/

    NEW_TERMINAL_MACRO(AsmLEFileHeader, "AsmLEFileHeader", "AsmLEFileHeaderTag");
    AsmLEFileHeader.setFunctionPrototype("HEADER_LE_FILE_HEADER", "../Grammar/BinaryInstruction.code");
    AsmLEFileHeader.setDataPrototype("unsigned", "e_byte_order", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("unsigned", "e_word_order", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("unsigned", "e_format_level", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("unsigned", "e_cpu_type", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("unsigned", "e_os_type", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("unsigned", "e_module_version", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("unsigned", "e_flags", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("unsigned", "e_eip_section", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("unsigned", "e_esp_section", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("unsigned", "e_last_page_size", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("unsigned", "e_page_offset_shift", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("unsigned", "e_fixup_sect_cksum", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("unsigned", "e_loader_sect_cksum", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("unsigned", "e_secttab_nentries", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("unsigned", "e_rsrctab_nentries", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("unsigned", "e_fmtdirtab_nentries", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("unsigned", "e_import_modtab_nentries", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("unsigned", "e_preload_npages", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("unsigned", "e_nonresnametab_size", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("unsigned", "e_nonresnametab_cksum", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("unsigned", "e_auto_ds_section", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("unsigned", "e_debug_info_size", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("unsigned", "e_num_instance_preload", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("unsigned", "e_num_instance_demand", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("unsigned", "e_heap_size", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_npages", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_eip", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_esp", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_page_size", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_fixup_sect_size", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_loader_sect_size", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_secttab_rfo", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_pagetab_rfo", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_iterpages_offset", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_rsrctab_rfo", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_resnametab_rfo", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_entrytab_rfo", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_fmtdirtab_rfo", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_fixup_pagetab_rfo", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_fixup_rectab_rfo", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_import_modtab_rfo", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_import_proctab_rfo", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_ppcksumtab_rfo", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_data_pages_offset", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_nonresnametab_offset", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("rose_addr_t", "e_debug_info_rfo", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("SgAsmDOSExtendedHeader*", "dos2_header", "= NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("SgAsmLESectionTable*", "section_table", "= NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("SgAsmLEPageTable*", "page_table", "= NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("SgAsmLENameTable*", "resname_table", "= NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("SgAsmLENameTable*", "nonresname_table", "= NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("SgAsmLEEntryTable*", "entry_table", "= NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AsmLEFileHeader.setDataPrototype("SgAsmLERelocTable*", "reloc_table", "= NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmLEEntryTable, "AsmLEEntryTable", "AsmLEEntryTableTag");
    AsmLEEntryTable.setFunctionPrototype("HEADER_LE_ENTRY_TABLE", "../Grammar/BinaryInstruction.code");
    AsmLEEntryTable.setDataPrototype("SgSizeTList", "bundle_sizes", "",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEEntryTable.setDataPrototype("SgAsmLEEntryPointPtrList", "entries", "",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmLEEntryPoint, "AsmLEEntryPoint", "AsmLEEntryPointTag");
    AsmLEEntryPoint.setFunctionPrototype("HEADER_LE_ENTRY_POINT", "../Grammar/BinaryInstruction.code");
    AsmLEEntryPoint.setFunctionSource("SOURCE_LE_ENTRY_POINT", "../Grammar/BinaryInstruction.code");
    AsmLEEntryPoint.setDataPrototype("SgAsmLEEntryPointPtrList", "entries", "",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AsmLEEntryPoint.setDataPrototype("unsigned", "flags", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEEntryPoint.setDataPrototype("unsigned", "objnum", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEEntryPoint.setDataPrototype("unsigned", "entry_type", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEEntryPoint.setDataPrototype("unsigned", "res1", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEEntryPoint.setDataPrototype("rose_addr_t", "entry_offset", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    /*************************************************************************************************************************
     *                                         LE Section Table
     *************************************************************************************************************************/

    NEW_TERMINAL_MACRO(AsmLESectionTable, "AsmLESectionTable", "AsmLESectionTableTag");
    AsmLESectionTable.setFunctionPrototype("HEADER_LE_SECTION_TABLE", "../Grammar/BinaryInstruction.code");



    NEW_TERMINAL_MACRO(AsmLESectionTableEntry, "AsmLESectionTableEntry", "AsmLESectionTableEntryTag");
    AsmLESectionTableEntry.setFunctionPrototype("HEADER_LE_SECTION_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
    AsmLESectionTableEntry.setFunctionSource("SOURCE_LE_SECTION_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
    AsmLESectionTableEntry.setDataPrototype("unsigned", "flags", "= 0",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLESectionTableEntry.setDataPrototype("unsigned", "pagemap_index", "= 0",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLESectionTableEntry.setDataPrototype("unsigned", "pagemap_nentries", "= 0",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLESectionTableEntry.setDataPrototype("unsigned", "res1", "= 0",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLESectionTableEntry.setDataPrototype("rose_addr_t", "mapped_size", "= 0",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLESectionTableEntry.setDataPrototype("rose_addr_t", "base_addr", "= 0",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    /*************************************************************************************************************************
     *                                         LE Relocation Table
     *************************************************************************************************************************/

    NEW_TERMINAL_MACRO(AsmLERelocTable, "AsmLERelocTable", "AsmLERelocTableTag");
    AsmLERelocTable.setFunctionPrototype("HEADER_LE_RELOC_TABLE", "../Grammar/BinaryInstruction.code");
    AsmLERelocTable.setDataPrototype("SgAsmLERelocEntryPtrList", "entries", "",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    /*************************************************************************************************************************
     *                                         LE Sections
     *************************************************************************************************************************/

    NEW_TERMINAL_MACRO(AsmLENameTable, "AsmLENameTable", "AsmLENameTableTag");
    AsmLENameTable.setFunctionPrototype("HEADER_LE_NAME_TABLE", "../Grammar/BinaryInstruction.code");
    AsmLENameTable.setDataPrototype("SgStringList", "names", "",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLENameTable.setDataPrototype("SgUnsignedList", "ordinals", "",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmLEPageTable, "AsmLEPageTable", "AsmLEPageTableTag");
    AsmLEPageTable.setFunctionPrototype("HEADER_LE_PAGE_TABLE", "../Grammar/BinaryInstruction.code");
    AsmLEPageTable.setDataPrototype("SgAsmLEPageTableEntryPtrList", "entries", "",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmLEPageTableEntry, "AsmLEPageTableEntry", "AsmLEPageTableEntryTag");
    AsmLEPageTableEntry.setFunctionPrototype("HEADER_LE_PAGE_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
    AsmLEPageTableEntry.setFunctionSource("SOURCE_LE_PAGE_TABLE_ENTRY", "../Grammar/BinaryInstruction.code");
    AsmLEPageTableEntry.setDataPrototype("unsigned", "pageno", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmLEPageTableEntry.setDataPrototype("unsigned", "flags", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmLESection, "AsmLESection", "AsmLESectionTag");
    AsmLESection.setFunctionPrototype("HEADER_LE_SECTION", "../Grammar/BinaryInstruction.code");
    AsmLESection.setDataPrototype("SgAsmLESectionTableEntry*", "st_entry", "= NULL",
                                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    /*************************************************************************************************************************
     *                                         DWARF
     * These are a subset (but most of) the possible Dwarf Tags, I [DQ] have skipped some of the architecture specific tags
     * (except for GNU).  There are about 63 new IR nodes here. These are kinds of language constructs that are stored in the
     * ".debug_info" section when dwarf debug information is available (generated by the compiler) in the binary executable.
     *************************************************************************************************************************/

    NEW_TERMINAL_MACRO(AsmDwarfArrayType, "AsmDwarfArrayType", "AsmDwarfArrayTypeTag");
    AsmDwarfArrayType.setFunctionPrototype("HEADER_DWARF_ARRAY_TYPE", "../Grammar/BinaryInstruction.code");
    AsmDwarfArrayType.setFunctionSource("SOURCE_DWARF_ARRAY_TYPE", "../Grammar/BinaryInstruction.code");
    AsmDwarfArrayType.setDataPrototype("SgAsmDwarfConstructList*", "body", "= NULL",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmDwarfClassType, "AsmDwarfClassType", "AsmDwarfClassTypeTag");
    AsmDwarfClassType.setFunctionPrototype("HEADER_DWARF_CLASS_TYPE", "../Grammar/BinaryInstruction.code");
    AsmDwarfClassType.setFunctionSource("SOURCE_DWARF_CLASS_TYPE", "../Grammar/BinaryInstruction.code");
    AsmDwarfClassType.setDataPrototype("SgAsmDwarfConstructList*", "body", "= NULL",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmDwarfEntryPoint, "AsmDwarfEntryPoint", "AsmDwarfEntryPointTag");



    NEW_TERMINAL_MACRO(AsmDwarfEnumerationType, "AsmDwarfEnumerationType", "AsmDwarfEnumerationTypeTag");
    AsmDwarfEnumerationType.setFunctionPrototype("HEADER_DWARF_ENUMERATION_TYPE", "../Grammar/BinaryInstruction.code");
    AsmDwarfEnumerationType.setFunctionSource("SOURCE_DWARF_ENUMERATION_TYPE", "../Grammar/BinaryInstruction.code");
    AsmDwarfEnumerationType.setDataPrototype("SgAsmDwarfConstructList*", "body", "= NULL",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmDwarfFormalParameter, "AsmDwarfFormalParameter", "AsmDwarfFormalParameterTag");



    NEW_TERMINAL_MACRO(AsmDwarfImportedDeclaration, "AsmDwarfImportedDeclaration", "AsmDwarfImportedDeclarationTag");



    NEW_TERMINAL_MACRO(AsmDwarfLabel, "AsmDwarfLabel", "AsmDwarfLabelTag");



    NEW_TERMINAL_MACRO(AsmDwarfLexicalBlock, "AsmDwarfLexicalBlock", "AsmDwarfLexicalBlockTag");
    AsmDwarfLexicalBlock.setFunctionPrototype("HEADER_DWARF_LEXICAL_BLOCK", "../Grammar/BinaryInstruction.code");
    AsmDwarfLexicalBlock.setFunctionSource("SOURCE_DWARF_LEXICAL_BLOCK", "../Grammar/BinaryInstruction.code");
    AsmDwarfLexicalBlock.setDataPrototype("SgAsmDwarfConstructList*", "body", "= NULL",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmDwarfMember, "AsmDwarfMember", "AsmDwarfMemberTag");



    NEW_TERMINAL_MACRO(AsmDwarfPointerType, "AsmDwarfPointerType", "AsmDwarfPointerTypeTag");



    NEW_TERMINAL_MACRO(AsmDwarfReferenceType, "AsmDwarfReferenceType", "AsmDwarfReferenceTypeTag");



    NEW_TERMINAL_MACRO(AsmDwarfCompilationUnit, "AsmDwarfCompilationUnit", "AsmDwarfCompilationUnitTag");
    AsmDwarfCompilationUnit.setFunctionPrototype("HEADER_DWARF_COMPILATION_UNIT", "../Grammar/BinaryInstruction.code");
    AsmDwarfCompilationUnit.setFunctionSource("SOURCE_DWARF_COMPILATION_UNIT", "../Grammar/BinaryInstruction.code");
    //AsmDwarfCompilationUnit.setDataPrototype("std::string", "name", "= \"\"",
    //                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDwarfCompilationUnit.setDataPrototype("std::string", "producer", "= \"\"",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDwarfCompilationUnit.setDataPrototype("std::string", "language", "= \"\"",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDwarfCompilationUnit.setDataPrototype("uint64_t", "low_pc", "= 0x0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDwarfCompilationUnit.setDataPrototype("uint64_t", "hi_pc", "= 0x0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDwarfCompilationUnit.setDataPrototype("int", "version_stamp", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDwarfCompilationUnit.setDataPrototype("uint64_t", "abbrev_offset", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDwarfCompilationUnit.setDataPrototype("uint64_t", "address_size", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDwarfCompilationUnit.setDataPrototype("uint64_t", "offset_length", "= 0",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDwarfCompilationUnit.setDataPrototype("SgAsmDwarfLineList*", "line_info", "= NULL",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AsmDwarfCompilationUnit.setDataPrototype("SgAsmDwarfConstructList*", "language_constructs", "= NULL",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AsmDwarfCompilationUnit.setDataPrototype("SgAsmDwarfMacroList*", "macro_info", "= NULL",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmDwarfStringType, "AsmDwarfStringType", "AsmDwarfStringTypeTag");



    NEW_TERMINAL_MACRO(AsmDwarfStructureType, "AsmDwarfStructureType", "AsmDwarfStructureTypeTag");
    AsmDwarfStructureType.setFunctionPrototype("HEADER_DWARF_STRUCTURE_TYPE", "../Grammar/BinaryInstruction.code");
    AsmDwarfStructureType.setFunctionSource("SOURCE_DWARF_STRUCTURE_TYPE", "../Grammar/BinaryInstruction.code");
    AsmDwarfStructureType.setDataPrototype("SgAsmDwarfConstructList*", "body", "= NULL",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmDwarfSubroutineType, "AsmDwarfSubroutineType", "AsmDwarfSubroutineTypeTag");
    AsmDwarfSubroutineType.setFunctionPrototype("HEADER_DWARF_SUBROUTINE_TYPE", "../Grammar/BinaryInstruction.code");
    AsmDwarfSubroutineType.setFunctionSource("SOURCE_DWARF_SUBROUTINE_TYPE", "../Grammar/BinaryInstruction.code");
    AsmDwarfSubroutineType.setDataPrototype("SgAsmDwarfConstructList*", "body", "= NULL",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmDwarfTypedef, "AsmDwarfTypedef", "AsmDwarfTypedefTag");



    NEW_TERMINAL_MACRO(AsmDwarfUnionType, "AsmDwarfUnionType", "AsmDwarfUnionTypeTag");
    AsmDwarfUnionType.setFunctionPrototype("HEADER_DWARF_UNION_TYPE", "../Grammar/BinaryInstruction.code");
    AsmDwarfUnionType.setFunctionSource("SOURCE_DWARF_UNION_TYPE", "../Grammar/BinaryInstruction.code");
    AsmDwarfUnionType.setDataPrototype("SgAsmDwarfConstructList*", "body", "= NULL",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmDwarfUnspecifiedParameters, "AsmDwarfUnspecifiedParameters", "AsmDwarfUnspecifiedParametersTag");



    NEW_TERMINAL_MACRO(AsmDwarfVariant, "AsmDwarfVariant", "AsmDwarfVariantTag");



    NEW_TERMINAL_MACRO(AsmDwarfCommonBlock, "AsmDwarfCommonBlock", "AsmDwarfCommonBlockTag");
    AsmDwarfCommonBlock.setFunctionPrototype("HEADER_DWARF_COMMON_BLOCK", "../Grammar/BinaryInstruction.code");
    AsmDwarfCommonBlock.setFunctionSource("SOURCE_DWARF_COMMON_BLOCK", "../Grammar/BinaryInstruction.code");
    AsmDwarfCommonBlock.setDataPrototype("SgAsmDwarfConstructList*", "body", "= NULL",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmDwarfCommonInclusion, "AsmDwarfCommonInclusion", "AsmDwarfCommonInclusionTag");



    NEW_TERMINAL_MACRO(AsmDwarfInheritance, "AsmDwarfInheritance", "AsmDwarfInheritanceTag");



    NEW_TERMINAL_MACRO(AsmDwarfInlinedSubroutine, "AsmDwarfInlinedSubroutine", "AsmDwarfInlinedSubroutineTag");
    AsmDwarfInlinedSubroutine.setFunctionPrototype("HEADER_DWARF_INLINED_SUBROUTINE", "../Grammar/BinaryInstruction.code");
    AsmDwarfInlinedSubroutine.setFunctionSource("SOURCE_DWARF_INLINED_SUBROUTINE", "../Grammar/BinaryInstruction.code");
    AsmDwarfInlinedSubroutine.setDataPrototype("SgAsmDwarfConstructList*", "body", "= NULL",
                                               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmDwarfModule, "AsmDwarfModule", "AsmDwarfModuleTag");



    NEW_TERMINAL_MACRO(AsmDwarfPtrToMemberType, "AsmDwarfPtrToMemberType", "AsmDwarfPtrToMemberTypeTag");



    NEW_TERMINAL_MACRO(AsmDwarfSetType, "AsmDwarfSetType", "AsmDwarfSetTypeTag");



    NEW_TERMINAL_MACRO(AsmDwarfSubrangeType, "AsmDwarfSubrangeType", "AsmDwarfSubrangeTypeTag");



    NEW_TERMINAL_MACRO(AsmDwarfWithStmt, "AsmDwarfWithStmt", "AsmDwarfWithStmtTag");



    NEW_TERMINAL_MACRO(AsmDwarfAccessDeclaration, "AsmDwarfAccessDeclaration", "AsmDwarfAccessDeclarationTag");



    NEW_TERMINAL_MACRO(AsmDwarfBaseType, "AsmDwarfBaseType", "AsmDwarfBaseTypeTag");



    NEW_TERMINAL_MACRO(AsmDwarfCatchBlock, "AsmDwarfCatchBlock", "AsmDwarfCatchBlockTag");



    NEW_TERMINAL_MACRO(AsmDwarfConstType, "AsmDwarfConstType", "AsmDwarfConstTypeTag");



    NEW_TERMINAL_MACRO(AsmDwarfConstant, "AsmDwarfConstant", "AsmDwarfConstantTag");



    NEW_TERMINAL_MACRO(AsmDwarfEnumerator, "AsmDwarfEnumerator", "AsmDwarfEnumeratorTag");



    NEW_TERMINAL_MACRO(AsmDwarfFileType, "AsmDwarfFileType", "AsmDwarfFileTypeTag");



    NEW_TERMINAL_MACRO(AsmDwarfFriend, "AsmDwarfFriend", "AsmDwarfFriendTag");



    NEW_TERMINAL_MACRO(AsmDwarfNamelist, "AsmDwarfNamelist", "AsmDwarfNamelistTag");



    NEW_TERMINAL_MACRO(AsmDwarfNamelistItem, "AsmDwarfNamelistItem", "AsmDwarfNamelistItemTag");



    NEW_TERMINAL_MACRO(AsmDwarfPackedType, "AsmDwarfPackedType", "AsmDwarfPackedTypeTag");



    NEW_TERMINAL_MACRO(AsmDwarfSubprogram, "AsmDwarfSubprogram", "AsmDwarfSubprogramTag");
    AsmDwarfSubprogram.setFunctionPrototype("HEADER_DWARF_SUBPROGRAM", "../Grammar/BinaryInstruction.code");
    AsmDwarfSubprogram.setFunctionSource("SOURCE_DWARF_SUBPROGRAM", "../Grammar/BinaryInstruction.code");
    AsmDwarfSubprogram.setDataPrototype("SgAsmDwarfConstructList*", "body", "= NULL",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmDwarfTemplateTypeParameter, "AsmDwarfTemplateTypeParameter", "AsmDwarfTemplateTypeParameterTag");



    NEW_TERMINAL_MACRO(AsmDwarfTemplateValueParameter, "AsmDwarfTemplateValueParameter", "AsmDwarfTemplateValueParameterTag");



    NEW_TERMINAL_MACRO(AsmDwarfThrownType, "AsmDwarfThrownType", "AsmDwarfThrownTypeTag");



    NEW_TERMINAL_MACRO(AsmDwarfTryBlock, "AsmDwarfTryBlock", "AsmDwarfTryBlockTag");



    NEW_TERMINAL_MACRO(AsmDwarfVariantPart, "AsmDwarfVariantPart", "AsmDwarfVariantPartTag");



    NEW_TERMINAL_MACRO(AsmDwarfVariable, "AsmDwarfVariable", "AsmDwarfVariableTag");



    NEW_TERMINAL_MACRO(AsmDwarfVolatileType, "AsmDwarfVolatileType", "AsmDwarfVolatileTypeTag");



    NEW_TERMINAL_MACRO(AsmDwarfDwarfProcedure, "AsmDwarfDwarfProcedure", "AsmDwarfDwarfProcedureTag");



    NEW_TERMINAL_MACRO(AsmDwarfRestrictType, "AsmDwarfRestrictType", "AsmDwarfRestrictTypeTag");



    NEW_TERMINAL_MACRO(AsmDwarfInterfaceType, "AsmDwarfInterfaceType", "AsmDwarfInterfaceTypeTag");



    NEW_TERMINAL_MACRO(AsmDwarfNamespace, "AsmDwarfNamespace", "AsmDwarfNamespaceTag");
    AsmDwarfNamespace.setFunctionPrototype("HEADER_DWARF_NAMESPACE", "../Grammar/BinaryInstruction.code");
    AsmDwarfNamespace.setFunctionSource("SOURCE_DWARF_NAMESPACE", "../Grammar/BinaryInstruction.code");
    AsmDwarfNamespace.setDataPrototype("SgAsmDwarfConstructList*", "body", "= NULL",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmDwarfImportedModule, "AsmDwarfImportedModule", "AsmDwarfImportedModuleTag");



    NEW_TERMINAL_MACRO(AsmDwarfUnspecifiedType, "AsmDwarfUnspecifiedType", "AsmDwarfUnspecifiedTypeTag");



    NEW_TERMINAL_MACRO(AsmDwarfPartialUnit, "AsmDwarfPartialUnit", "AsmDwarfPartialUnitTag");



    NEW_TERMINAL_MACRO(AsmDwarfImportedUnit, "AsmDwarfImportedUnit", "AsmDwarfImportedUnitTag");



    NEW_TERMINAL_MACRO(AsmDwarfMutableType, "AsmDwarfMutableType", "AsmDwarfMutableTypeTag");



    NEW_TERMINAL_MACRO(AsmDwarfCondition, "AsmDwarfCondition", "AsmDwarfConditionTag");



    NEW_TERMINAL_MACRO(AsmDwarfSharedType, "AsmDwarfSharedType", "AsmDwarfSharedTypeTag");



    NEW_TERMINAL_MACRO(AsmDwarfFormatLabel, "AsmDwarfFormatLabel", "AsmDwarfFormatLabelTag");



    NEW_TERMINAL_MACRO(AsmDwarfFunctionTemplate, "AsmDwarfFunctionTemplate", "AsmDwarfFunctionTemplateTag");



    NEW_TERMINAL_MACRO(AsmDwarfClassTemplate, "AsmDwarfClassTemplate", "AsmDwarfClassTemplateTag");



    NEW_TERMINAL_MACRO(AsmDwarfUpcSharedType, "AsmDwarfUpcSharedType", "AsmDwarfUpcSharedTypeTag");



    NEW_TERMINAL_MACRO(AsmDwarfUpcStrictType, "AsmDwarfUpcStrictType", "AsmDwarfUpcStrictTypeTag");



    NEW_TERMINAL_MACRO(AsmDwarfUpcRelaxedType, "AsmDwarfUpcRelaxedType", "AsmDwarfUpcRelaxedTypeTag");



    NEW_TERMINAL_MACRO(AsmDwarfUnknownConstruct, "AsmDwarfUnknownConstruct", "AsmDwarfUnknownConstructTag");



    NEW_NONTERMINAL_MACRO(AsmDwarfConstruct,
                          AsmDwarfArrayType | AsmDwarfClassType | AsmDwarfEntryPoint | AsmDwarfEnumerationType |
                          AsmDwarfFormalParameter | AsmDwarfImportedDeclaration | AsmDwarfLabel | AsmDwarfLexicalBlock |
                          AsmDwarfMember | AsmDwarfPointerType | AsmDwarfReferenceType | AsmDwarfCompilationUnit |
                          AsmDwarfStringType | AsmDwarfStructureType | AsmDwarfSubroutineType | AsmDwarfTypedef |
                          AsmDwarfUnionType | AsmDwarfUnspecifiedParameters | AsmDwarfVariant | AsmDwarfCommonBlock |
                          AsmDwarfCommonInclusion | AsmDwarfInheritance | AsmDwarfInlinedSubroutine | AsmDwarfModule |
                          AsmDwarfPtrToMemberType | AsmDwarfSetType | AsmDwarfSubrangeType | AsmDwarfWithStmt |
                          AsmDwarfAccessDeclaration | AsmDwarfBaseType | AsmDwarfCatchBlock | AsmDwarfConstType |
                          AsmDwarfConstant | AsmDwarfEnumerator | AsmDwarfFileType | AsmDwarfFriend | AsmDwarfNamelist |
                          AsmDwarfNamelistItem | AsmDwarfPackedType | AsmDwarfSubprogram | AsmDwarfTemplateTypeParameter |
                          AsmDwarfTemplateValueParameter | AsmDwarfThrownType | AsmDwarfTryBlock | AsmDwarfVariantPart |
                          AsmDwarfVariable | AsmDwarfVolatileType | AsmDwarfDwarfProcedure | AsmDwarfRestrictType |
                          AsmDwarfInterfaceType | AsmDwarfNamespace | AsmDwarfImportedModule | AsmDwarfUnspecifiedType |
                          AsmDwarfPartialUnit | AsmDwarfImportedUnit | AsmDwarfMutableType | AsmDwarfCondition |
                          AsmDwarfSharedType | AsmDwarfFormatLabel | AsmDwarfFunctionTemplate | AsmDwarfClassTemplate |
                          AsmDwarfUpcSharedType | AsmDwarfUpcStrictType | AsmDwarfUpcRelaxedType | AsmDwarfUnknownConstruct,
                          "AsmDwarfConstruct", "AsmDwarfConstructTag", false);
    AsmDwarfConstruct.setFunctionPrototype("HEADER_DWARF_CONSTRUCT", "../Grammar/BinaryInstruction.code");
    AsmDwarfConstruct.setFunctionSource("SOURCE_DWARF_CONSTRUCT", "../Grammar/BinaryInstruction.code");
    // For now we will store the nesting level (just to support debugging), then it will be removed.
    AsmDwarfConstruct.setDataPrototype("int", "nesting_level", "= 0",
                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDwarfConstruct.setDataPrototype("uint64_t", "offset", "= 0",
                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDwarfConstruct.setDataPrototype("uint64_t", "overall_offset", "= 0",
                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // A lot of constructs have a name so put the name into the base class
    AsmDwarfConstruct.setDataPrototype("std::string", "name", "= \"\"",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // This is a data member that can be filled in via a separate analysis (we would have to read the line information before
    // hand).
    AsmDwarfConstruct.setDataPrototype("SgAsmDwarfLine*", "source_position", "= NULL",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);




    NEW_TERMINAL_MACRO(AsmDwarfMacroList, "AsmDwarfMacroList", "AsmDwarfMacroListTag");
    AsmDwarfMacroList.setDataPrototype("SgAsmDwarfMacroPtrList", "macro_list", "",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmDwarfMacro, "AsmDwarfMacro", "AsmDwarfMacroTag");
    AsmDwarfMacro.setDataPrototype("std::string", "macro_string", "= \"\"",
                                   NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmDwarfLine, "AsmDwarfLine", "AsmDwarfLineTag");
    AsmDwarfLine.setDataPrototype("uint64_t", "address", "= 0",
                                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDwarfLine.setDataPrototype("int", "file_id", "= Sg_File_Info::NULL_FILE_ID",
                                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDwarfLine.setDataPrototype("int", "line", "= 0",
                                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDwarfLine.setDataPrototype("int", "column", "= 0",
                                  CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmDwarfLineList, "AsmDwarfLineList", "AsmDwarfLineListTag");
    AsmDwarfLineList.setFunctionPrototype("HEADER_DWARF_LINE_LIST", "../Grammar/BinaryInstruction.code");
    AsmDwarfLineList.setFunctionSource("SOURCE_DWARF_LINE_LIST", "../Grammar/BinaryInstruction.code");
    AsmDwarfLineList.setDataPrototype("SgAsmDwarfLinePtrList", "line_list", "",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#if 0
    // I [DQ] am having trouble making these proper data members so just use function to return them, so that they are
    // computed dynamically.
    AsmDwarfLineList.setDataPrototype("SgInstructionAddressSourcePositionMapPtrList", "instructionToSourceMap", "",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmDwarfLineList.setDataPrototype("SgSourcePositionInstructionAddressMapPtrList", "sourceToInstructionMap", "",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif



    NEW_TERMINAL_MACRO(AsmDwarfConstructList, "AsmDwarfConstructList", "AsmDwarfConstructListTag");
    AsmDwarfConstructList.setDataPrototype("SgAsmDwarfConstructPtrList", "list", "",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmDwarfCompilationUnitList, "AsmDwarfCompilationUnitList", "AsmDwarfCompilationUnitListTag");
    AsmDwarfCompilationUnitList.setDataPrototype("SgAsmDwarfCompilationUnitPtrList", "cu_list", "",
                                                 NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_NONTERMINAL_MACRO(AsmDwarfInformation,
                          AsmDwarfMacro | AsmDwarfMacroList | AsmDwarfLine | AsmDwarfLineList | AsmDwarfCompilationUnitList |
                          AsmDwarfConstruct | AsmDwarfConstructList,
                          "AsmDwarfInformation", "AsmDwarfInformationTag", false);



    /*************************************************************************************************************************
     *                                         Strings
     * These nodes describe how character strings (symbol names, section names, etc) are stored in a binary file.
     *************************************************************************************************************************/

    NEW_TERMINAL_MACRO(AsmStringStorage, "AsmStringStorage", "AsmStringStorageTag");
    AsmStringStorage.setFunctionPrototype("HEADER_STRING_STORAGE", "../Grammar/BinaryInstruction.code");
    AsmStringStorage.setDataPrototype("SgAsmGenericStrtab*", "strtab", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmStringStorage.setDataPrototype("std::string", "string", "= \"\"",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmStringStorage.setDataPrototype("rose_addr_t", "offset", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmBasicString, "AsmBasicString", "AsmBasicStringTag");
    AsmBasicString.setFunctionPrototype("HEADER_BASIC_STRING", "../Grammar/BinaryInstruction.code");
    AsmBasicString.setDataPrototype("std::string", "string", "= \"\"",
                                    NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmStoredString, "AsmStoredString", "AsmStoredStringTag");
    AsmStoredString.setFunctionPrototype("HEADER_STORED_STRING", "../Grammar/BinaryInstruction.code");
    AsmStoredString.setDataPrototype("SgAsmStringStorage*", "storage", "= NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    NEW_NONTERMINAL_MACRO(AsmGenericString,
                          AsmBasicString | AsmStoredString,
                          "AsmGenericString", "AsmGenericStringTag", false);
    AsmGenericString.setFunctionPrototype("HEADER_GENERIC_STRING", "../Grammar/BinaryInstruction.code");






    /*************************************************************************************************************************
     *                                         Generic Binary IR Nodes
     * These are mostly base classes for the container-specific nodes defined above.
     *************************************************************************************************************************/

    NEW_TERMINAL_MACRO(AsmGenericHeaderList, "AsmGenericHeaderList", "AsmGenericHeaderListTag");
    AsmGenericHeaderList.setDataPrototype("SgAsmGenericHeaderPtrList", "headers", "",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_NONTERMINAL_MACRO(AsmGenericHeader,
                          AsmPEFileHeader | AsmLEFileHeader | AsmNEFileHeader | AsmDOSFileHeader | AsmElfFileHeader,
                          "AsmGenericHeader", "AsmGenericHeaderTag", true);
    AsmGenericHeader.setFunctionPrototype("HEADER_GENERIC_HEADER", "../Grammar/BinaryInstruction.code");
    AsmGenericHeader.setFunctionSource("SOURCE_GENERIC_HEADER", "../Grammar/BinaryInstruction.code");
    AsmGenericHeader.setAutomaticGenerationOfDestructor(false);
    // General info about the executable format
    AsmGenericHeader.setDataPrototype("SgAsmGenericFormat*", "exec_format", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    // Optional magic number in file byte order
    AsmGenericHeader.setDataPrototype("SgCharList", "magic", "",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // Machine for which this header and its sections, etc. was compiled
    AsmGenericHeader.setDataPrototype("SgAsmGenericFormat::InsSetArchitecture", "isa", "= SgAsmGenericFormat::ISA_UNSPECIFIED",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // Base virtual address used by all "relative virtual addresses" (RVA)
    AsmGenericHeader.setDataPrototype("rose_addr_t", "base_va", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // Code entry point wrt base_va
    AsmGenericHeader.setDataPrototype("SgRVAList", "entry_rvas", "",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmGenericHeader.setDataPrototype("SgAsmGenericDLLList*", "dlls", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AsmGenericHeader.setDataPrototype("SgAsmGenericSectionList*", "sections", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmGenericSymbolList, "AsmGenericSymbolList", "AsmGenericSymbolListTag");
    AsmGenericSymbolList.setDataPrototype("SgAsmGenericSymbolPtrList", "symbols", "",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    NEW_NONTERMINAL_MACRO(AsmGenericSymbol,
                          AsmCoffSymbol | AsmElfSymbol,
                          "AsmGenericSymbol", "AsmGenericSymbolTag", false);
    AsmGenericSymbol.setFunctionPrototype("HEADER_GENERIC_SYMBOL", "../Grammar/BinaryInstruction.code");
    AsmGenericSymbol.setFunctionSource("SOURCE_GENERIC_SYMBOL", "../Grammar/BinaryInstruction.code");
    AsmGenericSymbol.setAutomaticGenerationOfConstructor(false);
    AsmGenericSymbol.setDataPrototype("SgAsmGenericSymbol::SymbolDefState", "def_state", "= SgAsmGenericSymbol::SYM_UNDEFINED",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmGenericSymbol.setDataPrototype("SgAsmGenericSymbol::SymbolBinding", "binding", "= SgAsmGenericSymbol::SYM_NO_BINDING",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmGenericSymbol.setDataPrototype("SgAsmGenericSymbol::SymbolType", "type", "= SgAsmGenericSymbol::SYM_NO_TYPE",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmGenericSymbol.setDataPrototype("rose_addr_t", "value", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmGenericSymbol.setDataPrototype("rose_addr_t", "size", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmGenericSymbol.setDataPrototype("SgAsmGenericSection*", "bound", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmGenericSymbol.setDataPrototype("SgAsmGenericString*", "name", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_NONTERMINAL_MACRO(AsmGenericStrtab,
                          AsmElfStrtab | AsmCoffStrtab,
                          "AsmGenericStrtab", "AsmGenericStrtabTag", false);
    AsmGenericStrtab.setFunctionPrototype("HEADER_GENERIC_STRTAB", "../Grammar/BinaryInstruction.code");
    AsmGenericStrtab.setAutomaticGenerationOfDestructor(false);
    AsmGenericStrtab.setDataPrototype("SgAsmGenericSection*", "container", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmGenericStrtab.setDataPrototype("SgAsmGenericStrtab::referenced_t", "storage_list", "",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmGenericStrtab.setDataPrototype("AddressIntervalSet", "freelist", "", // space avail. for new strings
                                      NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmGenericStrtab.setDataPrototype("SgAsmStringStorage*", "dont_free", "= NULL", // never free this storage
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmGenericStrtab.setDataPrototype("size_t", "num_freed", "= 0", // number of strings freed thus far
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmGenericSectionList, "AsmGenericSectionList", "AsmGenericSectionListTag");
    AsmGenericSectionList.setDataPrototype("SgAsmGenericSectionPtrList", "sections", "",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_NONTERMINAL_MACRO(AsmGenericSection,
                          AsmGenericHeader | AsmElfSection | AsmElfSectionTable | AsmElfSegmentTable | AsmPESection |
                          AsmPESectionTable | AsmDOSExtendedHeader | AsmCoffSymbolTable | AsmNESection | AsmNESectionTable |
                          AsmNENameTable | AsmNEModuleTable | AsmNEStringTable | AsmNEEntryTable | AsmNERelocTable |
                          AsmLESection | AsmLESectionTable | AsmLENameTable | AsmLEPageTable | AsmLEEntryTable | AsmLERelocTable,
                          "AsmGenericSection", "AsmGenericSectionTag", true);
    AsmGenericSection.setFunctionPrototype("HEADER_GENERIC_SECTION", "../Grammar/BinaryInstruction.code");
    AsmGenericSection.setFunctionSource("SOURCE_GENERIC_SECTION", "../Grammar/BinaryInstruction.code");
    AsmGenericSection.setPredeclarationString("HEADER_GENERIC_SECTION_PREDECLARATION", "../Grammar/BinaryInstruction.code");
    AsmGenericSection.setAutomaticGenerationOfDestructor(false);
    AsmGenericSection.setDataPrototype("SgAsmGenericFile*", "file", "= NULL", // The file to which this section belongs
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmGenericSection.setDataPrototype("SgAsmGenericHeader*", "header", "= NULL",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // Current size of section in bytes (may be different than original size, which is kept in p_data.size())
    AsmGenericSection.setDataPrototype("rose_addr_t", "size", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmGenericSection.setDataPrototype("rose_addr_t", "offset", "= 0", // Starting file offset of the section
                                       NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // Required file alignment; zero and one both imply byte alignment
    AsmGenericSection.setDataPrototype("rose_addr_t", "file_alignment", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // Original content of just this section; points into file's content
    AsmGenericSection.setDataPrototype("SgFileContentList", "data", "",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // General contents of the section
    AsmGenericSection.setDataPrototype("SgAsmGenericSection::SectionPurpose", "purpose", "= SgAsmGenericSection::SP_UNSPECIFIED",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // Section was created by the format reader; not specified in file
    AsmGenericSection.setDataPrototype("bool", "synthesized", "= false",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // Non-unique section ID (unique for ELF) or negative
    AsmGenericSection.setDataPrototype("int", "id", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // Non-unique name of section (may be empty string).  The name is not part of the AST traversal because if we
    // made it part of the traversal then we'd need to fix some of the subclasses which contain a solitary list
    // data member (ROSE cannot have IR nodes that contain a list and other data members).  Therefore, the SgAsmGenericSection
    // destructor will need to explicitly delete the name.
    AsmGenericSection.setDataPrototype("SgAsmGenericString*", "name", "= NULL",
                                       NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmGenericSection.setDataPrototype("std::string", "short_name", "",
                                       NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmGenericSection.setDataPrototype("rose_addr_t", "mapped_preferred_rva", "= 0", // DOXYGEN
                                       NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmGenericSection.setDataPrototype("rose_addr_t", "mapped_size", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmGenericSection.setDataPrototype("rose_addr_t", "mapped_alignment", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmGenericSection.setDataPrototype("bool", "mapped_rperm", "= false",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmGenericSection.setDataPrototype("bool", "mapped_wperm", "= false",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmGenericSection.setDataPrototype("bool", "mapped_xperm", "= false",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmGenericSection.setDataPrototype("bool", "contains_code", "= false", // DOXYGEN
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmGenericSection.setDataPrototype("rose_addr_t", "mapped_actual_va", "= 0", // DOXYGEN
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmGenericDLLList, "AsmGenericDLLList", "AsmGenericDLLListTag");
    AsmGenericDLLList.setDataPrototype("SgAsmGenericDLLPtrList", "dlls", "",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmGenericDLL, "AsmGenericDLL", "AsmGenericDLLTag");
    AsmGenericDLL.setFunctionPrototype("HEADER_GENERIC_DLL", "../Grammar/BinaryInstruction.code");
    AsmGenericDLL.setFunctionSource("SOURCE_GENERIC_DLL", "../Grammar/BinaryInstruction.code");
    AsmGenericDLL.setDataPrototype("SgAsmGenericString*", "name", "= 0",
                                   NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
    AsmGenericDLL.setDataPrototype("SgStringList", "symbols", "",
                                   NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);



    NEW_TERMINAL_MACRO(AsmGenericFormat, "AsmGenericFormat", "AsmGenericFormatTag");
    AsmGenericFormat.setFunctionPrototype("HEADER_GENERIC_FORMAT", "../Grammar/BinaryInstruction.code");
    AsmGenericFormat.setDataPrototype("SgAsmGenericFormat::ExecFamily", "family", "= SgAsmGenericFormat::FAMILY_UNSPECIFIED",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmGenericFormat.setDataPrototype("SgAsmGenericFormat::ExecPurpose", "purpose", "= SgAsmGenericFormat::PURPOSE_EXECUTABLE",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmGenericFormat.setDataPrototype("ByteOrder::Endianness", "sex", "= ByteOrder::ORDER_UNSPECIFIED",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // actual file format version number stored in file
    AsmGenericFormat.setDataPrototype("unsigned", "version", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // is 'version' considered to be the current, supported version
    AsmGenericFormat.setDataPrototype("bool", "is_current_version", "= false",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // application binary interface
    AsmGenericFormat.setDataPrototype("SgAsmGenericFormat::ExecABI", "abi", "= SgAsmGenericFormat::ABI_UNSPECIFIED",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // version of the ABI targeted by this file
    AsmGenericFormat.setDataPrototype("unsigned", "abi_version", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // Initialize to ZERO first and then set as required, in bytes (e.g., Elf32 is 4; Elf64 is 8)
    AsmGenericFormat.setDataPrototype("size_t", "word_size", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmGenericFileList, "AsmGenericFileList", "AsmGenericFileListTag");
    AsmGenericFileList.setDataPrototype("SgAsmGenericFilePtrList", "files", "",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);



    NEW_TERMINAL_MACRO(AsmGenericFile, "AsmGenericFile", "AsmGenericFileTag");
    AsmGenericFile.setFunctionPrototype("HEADER_GENERIC_FILE", "../Grammar/BinaryInstruction.code");
    AsmGenericFile.setPredeclarationString("HEADER_GENERIC_FILE_PREDECLARATION", "../Grammar/BinaryInstruction.code");
    AsmGenericFile.setAutomaticGenerationOfConstructor(false);
    AsmGenericFile.setAutomaticGenerationOfDestructor(false);
    // The "dwarf_info" member is first as an optimization: a single AST traversal will see the dwarf information first and
    // therefore have the necessary debugging information already available when the traversal gets to the rest of the AST.
    AsmGenericFile.setDataPrototype("SgAsmDwarfCompilationUnitList*", "dwarf_info", "= NULL",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AsmGenericFile.setDataPrototype("std::string", "name", "= \"\"",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // File descriptor opened for read-only (or negative)
    AsmGenericFile.setDataPrototype("int", "fd", "= -1",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // File attributes at time of file open (valid if fd>=0)
    AsmGenericFile.setDataPrototype("SgAsmGenericFormat::fileDetails", "sb", "",
                                    NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // Content of file mapped into memory
    AsmGenericFile.setDataPrototype("SgFileContentList", "data", "",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    // All known header sections for this file
    AsmGenericFile.setDataPrototype("SgAsmGenericHeaderList*", "headers", "= NULL",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    // All known holes (areas otherwise unreferenced by parsing)
    AsmGenericFile.setDataPrototype("SgAsmGenericSectionList*", "holes", "= NULL",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
    AsmGenericFile.setDataPrototype("bool", "truncate_zeros", "= false",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmGenericFile.setDataPrototype("bool", "tracking_references", "= true",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmGenericFile.setDataPrototype("AddressIntervalSet", "referenced_extents", "",
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmGenericFile.setDataPrototype("bool", "neuter", "= false", // if set, refuse to unparse file
                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    /*************************************************************************************************************************
     *                                         Generic Binary File
     * This is the root of binary file format container classes.
     *************************************************************************************************************************/

    NEW_NONTERMINAL_MACRO(AsmExecutableFileFormat,
                          AsmGenericDLL | AsmGenericFormat | AsmGenericDLLList | AsmElfEHFrameEntryFD | AsmGenericFile |
                          AsmGenericSection | AsmGenericSymbol | AsmGenericStrtab | AsmGenericSymbolList |
                          AsmGenericSectionList | AsmGenericHeaderList | AsmGenericString | AsmElfSectionTableEntry |
                          AsmElfSegmentTableEntry | AsmElfSymbolList | AsmElfRelocEntry |
                          AsmElfRelocEntryList | AsmPEExportEntry | AsmPEExportEntryList | AsmElfDynamicEntry |
                          AsmElfDynamicEntryList | AsmElfSegmentTableEntryList | AsmStringStorage | AsmElfNoteEntry |
                          AsmElfNoteEntryList | AsmElfSymverEntry | AsmElfSymverEntryList | AsmElfSymverDefinedEntry |
                          AsmElfSymverDefinedEntryList | AsmElfSymverDefinedAux | AsmElfSymverDefinedAuxList |
                          AsmElfSymverNeededEntry | AsmElfSymverNeededEntryList | AsmElfSymverNeededAux |
                          AsmElfSymverNeededAuxList | AsmPEImportDirectory | AsmPESectionTableEntry |
                          AsmPEExportDirectory | AsmPERVASizePair | AsmCoffSymbolList | AsmPERVASizePairList |
                          AsmElfEHFrameEntryCI |
                          AsmPEImportDirectoryList | AsmNEEntryPoint | AsmNERelocEntry | AsmNESectionTableEntry |
                          AsmElfEHFrameEntryCIList | AsmLEPageTableEntry | AsmLEEntryPoint | AsmLESectionTableEntry |
                          AsmElfEHFrameEntryFDList | AsmDwarfInformation | AsmPEImportItem | AsmPEImportItemList,
                          "AsmExecutableFileFormat", "AsmExecutableFileFormatTag", false);
    AsmExecutableFileFormat.setFunctionPrototype("HEADER_EXECUTABLE_FILE_FORMAT", "../Grammar/BinaryInstruction.code");
    AsmExecutableFileFormat.setPredeclarationString("HEADER_EXECUTABLE_FILE_FORMAT_PREDECLARATION",
                                               "../Grammar/BinaryInstruction.code");



    /*************************************************************************************************************************
     *                                         Root of all binary IR classes
     *************************************************************************************************************************/

    NEW_NONTERMINAL_MACRO(AsmNode,
                          AsmStatement | AsmExpression | AsmInterpretation | AsmOperandList | AsmType |
                          AsmExecutableFileFormat | AsmInterpretationList | AsmGenericFileList,
                          "AsmNode", "AsmNodeTag", false);
    AsmNode.setFunctionPrototype("HEADER_BINARY", "../Grammar/BinaryInstruction.code");
    AsmNode.setFunctionSource("SOURCE_BINARY", "../Grammar/BinaryInstruction.code");
    AsmNode.setDataPrototype("AttachedPreprocessingInfoType*", "attachedPreprocessingInfoPtr", "= NULL",
                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, COPY_DATA);
    AsmNode.setFunctionPrototype("HEADER_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");
    AsmNode.setFunctionSource("SOURCE_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");
    AsmNode.setFunctionPrototype("HEADER_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");
    AsmNode.setFunctionSource("SOURCE_ATTRIBUTE_SUPPORT", "../Grammar/Support.code");
    AsmNode.setDataPrototype("AstAttributeMechanism*", "attributeMechanism", "= NULL",
                             NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, CLONE_PTR);

#ifndef DOCUMENTATION
}
#endif
