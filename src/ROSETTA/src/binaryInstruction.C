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
//       DECLARE_LEAF_CLASS(MyClass);
//       #ifdef DOCUMENTATION
//       class SgMyClass: public ...base-classes... {
//       #endif
//
//
//   DECLARE_HEADERS is used to indicate what header files need to be included. Note that due to limitations of ROSETTA
//   (specifically, not having any portable regular expression library due to prohibition against using boost), the #ifdef and
//   #endif lines must be *exactly* as written here -- they are sensitive to white space.
//       DECLARE_HEADERS(MyClass);
//       #if defined(SgMyClass_HEADERS) || defined(DOCUMENTATION)
//       #include <someHeader>
//       #endif // SgMyClass_HEADERS
//
//
//   DECLARE_OTHERS is for declaring other class members that don't need to be processed by ROSETTA. Due to limitations of
//   ROSETTA (specifically, not having any portable regular expression library due to prohibition against using boost), the
//   #ifdef and #endif lines must be *exactly* as written here -- they are sensitive to white space.
//       DECLARE_OTHERS(MyClass);
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

#ifdef DOCUMENTATION
#define IS_SERIALIZABLE() /*void*/
#else
#define IS_SERIALIZABLE(CLASS_WITHOUT_Sg) \
    CLASS_WITHOUT_Sg.isBoostSerializable(true)
#endif

// Since ROSETTA builds classes from the leaves up to the base, and C++ builds classes from the base down to the leaves, we
// need to make sure that doxygen sees the base classes before the derived classes. So just list all the non-leaf classes here.
#ifdef DOCUMENTATION
class SgAsmBinaryExpression;
class SgAsmConstantExpression;
class SgAsmDwarfConstruct;
class SgAsmDwarfInformation;
class SgAsmElfSection;
class SgAsmExecutableFileFormat;
class SgAsmExpression;
class SgAsmGenericHeader;
class SgAsmGenericSection;
class SgAsmGenericString;
class SgAsmGenericStrtab;
class SgAsmGenericSymbol;
class SgAsmInstruction;
class SgAsmNode;
class SgAsmPESection;
class SgAsmRegisterReferenceExpression;
class SgAsmScalarType;
class SgAsmStatement;
class AsmSynthesizedFieldDeclaration;
class SgAsmType;
class SgAsmUnaryExpression;
class SgAsmValueExpression;
class SgNode;
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
    IS_SERIALIZABLE(AsmArmInstruction);

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
         *  Returns an enum constant describing the ARM instruction. These enum constants correspond roughly 1:1 with
         *  instruction mnemonics. Each architecture has its own set of enum constants. See also, getAnyKind.
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
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmInstruction);
            s & BOOST_SERIALIZATION_NVP(p_kind);
            s & BOOST_SERIALIZATION_NVP(p_condition);
            s & BOOST_SERIALIZATION_NVP(p_positionOfConditionInMnemonic);
        }
#endif

    public:
        // Overrides are documented in the base class
        virtual std::string description() const ROSE_OVERRIDE;
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
    IS_SERIALIZABLE(AsmX86Instruction);
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

#ifdef DOCUMENTATION
        /** Property: Instruction kind.
         *
         *  Returns an enum constant describing the x86 instruction. These enum constants correspond roughly 1:1 with
         *  instruction mnemonics. Each architecture has its own set of enum constants. See also, getAnyKind.
         *
         * @{ */
        X86InstructionKind get_kind() const;
        void set_kind(X86InstructionSize);
        /** @} */
#else
        AsmX86Instruction.setDataPrototype("X86InstructionKind", "kind", "= x86_unknown_instruction",
                                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
#endif

#ifdef DOCUMENTATION
        /** Property: An enum constant describing the base size of an x86 instruction.
         *
         * @{ */
        X86InstructionSize get_baseSize() const;
        void set_baseSize(X86InstructionSize);
        /** @} */
#else
        AsmX86Instruction.setDataPrototype("X86InstructionSize", "baseSize", "= x86_insnsize_none",
                                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
#endif

#ifdef DOCUMENTATION
        /** Property: An enum describing the x86 instruction operand size.
         *
         * @{ */
        X86InstructionSize get_operandSize() const;
        void set_operandSize(X86InstructionSize);
        /** @} */
#else
        AsmX86Instruction.setDataPrototype("X86InstructionSize", "operandSize", "= x86_insnsize_none",
                                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
#endif

#ifdef DOCUMENTATION
        /** Property: An enum describing the x86 address size.
         *
         *  @{ */
        X86InstructionSize get_addressSize() const;
        void set_addressSize(X86InstructionSize);
        /** @} */
#else
        AsmX86Instruction.setDataPrototype("X86InstructionSize", "addressSize", "= x86_insnsize_none",
                                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
#endif

#ifdef DOCUMENTATION
        /** Property: Whether the x86 lock prefix was present.
         *
         *  Returns true if this instruction had the x86 lock prefix byte; false otherwise.
         *
         *  @{ */
        bool get_lockPrefix() const;
        void set_lockPrefix(bool);
        /** @} */
#else
        AsmX86Instruction.setDataPrototype("bool", "lockPrefix", "= false",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
#endif

#ifdef DOCUMENTATION
        /** Property: An enum constant describing whether the instruction is repeated.
         *
         * @{ */
        X86RepeatPrefix get_repeatPrefix() const;
        void set_repeatPrefix(X86RepeatPrefix);
        /** @} */
#else
        AsmX86Instruction.setDataPrototype("X86RepeatPrefix", "repeatPrefix", "= x86_repeat_none",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
#endif

#ifdef DOCUMENTATION
        /** Property: An enum constant describing branch prediction.
         *
         * @{ */
        X86BranchPrediction get_branchPrediction() const;
        void set_branchPrediction(X86BranchPrediction);
        /** @} */
#else
        AsmX86Instruction.setDataPrototype("X86BranchPrediction", "branchPrediction", "= x86_branch_prediction_none",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
#endif

#ifdef DOCUMENTATION
        /** Property: The segment override register.
         *
         *  Returns an enum describing the segment register override. Note that this is not a register descriptor, but an enum
         *  constant.
         *
         *  @{ */
        X86SegmentRegister get_segmentOverride() const;
        void set_segmentOverride(X86SegmentRegister);
        /** @} */
#else
        AsmX86Instruction.setDataPrototype("X86SegmentRegister", "segmentOverride", "= x86_segreg_none",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
#endif


        DECLARE_OTHERS(AsmX86Instruction);
#if defined(SgAsmX86Instruction_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmInstruction);
            s & BOOST_SERIALIZATION_NVP(p_kind);
            s & BOOST_SERIALIZATION_NVP(p_baseSize);
            s & BOOST_SERIALIZATION_NVP(p_operandSize);
            s & BOOST_SERIALIZATION_NVP(p_addressSize);
            s & BOOST_SERIALIZATION_NVP(p_lockPrefix);
            s & BOOST_SERIALIZATION_NVP(p_repeatPrefix);
            s & BOOST_SERIALIZATION_NVP(p_branchPrediction);
            s & BOOST_SERIALIZATION_NVP(p_segmentOverride);
        }
#endif

    public:
        /** Converts a size to an instruction size enum.
         *
         *  Given a size in bits, which must be 16, 32, or 64, return the corresponding enum constant. */
        static X86InstructionSize instructionSizeForWidth(size_t);

        /** Converts a size enum constant to a size.
         *
         *  Given a size enum constant, return the number of bits that enum represents. */
        static size_t widthForInstructionSize(X86InstructionSize);

        /** Return the register dictionary for an x86 architecture.
         *
         *  Given an instruction size enum constant return the register dictionary that describes the x86 architecture with
         *  the specified word size.  See also, @ref registersForWidth. */
        static const RegisterDictionary* registersForInstructionSize(X86InstructionSize);

        /** Return the register dictionary for an x86 architecture.
         *
         *  Given an instruction size of 16, 32, or 64 return the register dictionary that describes the x86 architecture with
         *  the specified word size.  See also, @ref registersForInstructionSize. */
        static const RegisterDictionary* registersForWidth(size_t);

        // Overrides are documented in the base class
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
        virtual bool isUnknown() const ROSE_OVERRIDE;
        virtual unsigned get_anyKind() const ROSE_OVERRIDE;
#endif // SgAsmX86Instruction_OTHERS
#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmPowerpcInstruction);
    IS_SERIALIZABLE(AsmPowerpcInstruction);
    DECLARE_HEADERS(AsmPowerpcInstruction);
#if defined(SgAsmPowerpcInstruction_HEADERS) || defined(DOCUMENTATION)
    #include <powerpcInstructionEnum.h>
#endif // SgAsmPowerpcInstruction_HEADERS

#ifdef DOCUMENTATION
    /** Represents one PowerPC machine instruction. */
    class SgAsmPowerpcInstruction: public SgAsmInstruction {
    public:
#endif


#ifdef DOCUMENTATION
        /** Property: Instruction kind.
         *
         *  Returns an enum constant describing the PowerPC instruction. These enum constants correspond roughly 1:1 with
         *  instruction mnemonics. Each architecture has its own set of enum constants. See also, getAnyKind.
         *
         * @{ */
        PowerpcInstructionKind get_kind() const;
        void set_kind(PowerpcInstructionKind);
        /** @} */
#else
        AsmPowerpcInstruction.setDataPrototype("PowerpcInstructionKind", "kind", "= powerpc_unknown_instruction",
                                               CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE,
                                               COPY_DATA);
#endif

        DECLARE_OTHERS(AsmPowerpcInstruction);
#if defined(SgAsmPowerpcInstruction_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmInstruction);
            s & BOOST_SERIALIZATION_NVP(p_kind);
        }
#endif

    public:
        // Overrides are documented in the base class
        virtual std::string description() const ROSE_OVERRIDE;
        virtual bool terminatesBasicBlock() ROSE_OVERRIDE;
        virtual std::set<rose_addr_t> getSuccessors(bool* complete) ROSE_OVERRIDE;
        virtual bool isUnknown() const ROSE_OVERRIDE;
        virtual unsigned get_anyKind() const ROSE_OVERRIDE;
#endif // SgAsmPowerpcInstruction_OTHERS
#ifdef DOCUMENTATION
    };
#endif


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmMipsInstruction);
    IS_SERIALIZABLE(AsmMipsInstruction);
    DECLARE_HEADERS(AsmMipsInstruction);
#if defined(SgAsmMipsInstruction_HEADERS) || defined(DOCUMENTATION)
    #include <InstructionEnumsMips.h>
#endif // SgAsmMipsInstruction_HEADERS

#ifdef DOCUMENTATION
    /** Represents one MIPS machine instruction. */
    class SgAsmMipsInstruction: public SgAsmInstruction {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Instruction kind.
         *
         *  Returns an enum constant describing the MIPS instruction. These enum constants correspond roughly 1:1 with
         *  instruction mnemonics. Each architecture has its own set of enum constants. See also, getAnyKind.
         *
         * @{ */
        MipsInstructionKind get_kind() const;
        void set_kind(MipsInstructionKind);
        /** @} */
#else
        AsmMipsInstruction.setDataPrototype("MipsInstructionKind", "kind", "= mips_unknown_instruction",
                                            CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE,
                                            COPY_DATA);
#endif

        DECLARE_OTHERS(AsmMipsInstruction);
#if defined(SgAsmMipsInstruction_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmInstruction);
            s & BOOST_SERIALIZATION_NVP(p_kind);
        }
#endif

    public:
        // Overrides are documented in the base class
        virtual std::string description() const ROSE_OVERRIDE;
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
    IS_SERIALIZABLE(AsmM68kInstruction);
    DECLARE_HEADERS(AsmM68kInstruction);
#if defined(SgAsmM68kInstruction_HEADERS) || defined(DOCUMENTATION)
    #include "InstructionEnumsM68k.h"
#endif // SgAsmM68kInstruction_HEADERS

#ifdef DOCUMENTATION
    class SgAsmM68kInstruction: public SgAsmInstruction {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Instruction kind.
         *
         *  Returns an enum constant describing the ARM instruction. These enum constants correspond roughly 1:1 with
         *  instruction mnemonics. Each architecture has its own set of enum constants. See also, getAnyKind.
         *
         * @{ */
        M68kInstructionKind get_kind() const;
        void set_kind(M68kInstructionKind);
        /** @} */
#else
        AsmM68kInstruction.setDataPrototype("M68kInstructionKind", "kind", " = m68k_unknown_instruction",
                                            CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE,
                                            COPY_DATA);
#endif

        DECLARE_OTHERS(AsmM68kInstruction);
#if defined(SgAsmM68kInstruction_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmInstruction);
            s & BOOST_SERIALIZATION_NVP(p_kind);
        }
#endif

    public:
        // Overrides are documented in the base class
        virtual std::string description() const ROSE_OVERRIDE;
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
    AsmInstruction.setCppCondition("!defined(DOCUMENTATION)");
    IS_SERIALIZABLE(AsmInstruction);

    DECLARE_HEADERS(AsmInstruction);
#if defined(SgAsmInstruction_HEADERS) || defined(DOCUMENTATION)
    #include <MemoryMap.h>
#endif // SgAsmInstruction_HEADERS

#ifdef DOCUMENTATION
    class SgAsmInstruction: public SgAsmStatement {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Instruction mnemonic string.
         *
         *  The short string that describes the instruction. When comparing instructions, it's faster to use the @ref get_kind
         *  or @ref get_anyKind methods instead of comparing mnemonic strings. But be aware that some architectures have
         *  mnemonics that include information about the instruction operands and this information is typically not represented
         *  by the instruction kind enum constants.
         *
         * @{ */
        const std::string& get_mnemonic() const;
        void set_mnemonic(const std::string&);
        /** @} */
#else
        AsmInstruction.setDataPrototype("std::string", "mnemonic", "= \"\"",
                                        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
#endif

#ifdef DOCUMENTATION
        /** Property: Raw bytes of an instruction.
         *
         *  These are the bytes that were actually decoded to obtain the instruction AST.
         *
         * @{ */
        const SgUnsignedList& get_raw_bytes() const;
        void set_raw_bytes(const SgUnsignedList&);
        /** @} */
#else
        AsmInstruction.setDataPrototype("SgUnsignedCharList", "raw_bytes", "",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
#endif

#ifdef DOCUMENTATION
        /** Property: AST node that holds all operands.
         *
         *  This is the @ref SgAsmOperandList AST node that holds all the operands of this instruction. A separate node is
         *  necessary (rather than storing the operand list directly in the instruction node) due to limitations of ROSETTA.
         *
         * @{ */
        SgAsmOperandList* get_operandList() const;
        void set_operandList(SgAsmOperandList*);
        /** @} */
#else
        AsmInstruction.setDataPrototype("SgAsmOperandList*", "operandList", "= NULL",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, COPY_DATA);
#endif

#ifdef DOCUMENTATION
        // FIXME[Robb P Matzke 2017-02-13]: unused?
#else
        AsmInstruction.setDataPrototype("SgAsmStatementPtrList", "sources", "",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Stack pointer at start of instruction relative to start of instruction's function.
         *
         *  If the stack delta was not computed, or could not be computed, or is a non-numeric value then the special value
         *  @ref INVALID_STACK_DELTA is used.
         *
         *  @{ */
        int64_t get_stackDeltaIn() const;
        void set_stackDeltaIn(int64_t);
        /** @} */
#else
        AsmInstruction.setDataPrototype("int64_t", "stackDeltaIn", "= SgAsmInstruction::INVALID_STACK_DELTA",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        // FIXME[Robb P Matzke 2017-02-13]: unused?
#else
        AsmInstruction.setDataPrototype("SgAsmExprListExp*", "semantics", "= NULL",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, COPY_DATA);
#endif

        DECLARE_OTHERS(AsmInstruction);
#if defined(SgAsmInstruction_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmStatement);
            s & BOOST_SERIALIZATION_NVP(p_mnemonic);
            s & BOOST_SERIALIZATION_NVP(p_raw_bytes);
            s & BOOST_SERIALIZATION_NVP(p_operandList);
            s & BOOST_SERIALIZATION_NVP(p_sources);
        }
#endif

    public:
        /** Represents an invalid stack delta.
         *
         *  This value is used for the result of a stack delta analysis stored in the instruction AST if the stack delta
         *  analysis was not run or did not produce a numeric result. */
        static const int64_t INVALID_STACK_DELTA;

        /** Return a description of this instruction.
         *
         *  Descriptions are useful for generating comments in the disassembly listing to say what each instruction does when
         *  the audience is not well versed in that instruction set architecture.  The base implementation always returns an
         *  empty string. */
        virtual std::string description() const { return ""; }

        // [Robb P Matzke 2017-02-13]: deprecating this old API
        SgAsmInstruction* cfgBinFlowOutEdge(const VirtualBinCFG::AuxiliaryInformation* info)
            ROSE_DEPRECATED("cfgBin is deprecated");
        std::vector<VirtualBinCFG::CFGEdge> cfgBinOutEdges(const VirtualBinCFG::AuxiliaryInformation* info)
            ROSE_DEPRECATED("cfgBin is deprecated");
        std::vector<VirtualBinCFG::CFGEdge> cfgBinInEdges(const VirtualBinCFG::AuxiliaryInformation* info)
            ROSE_DEPRECATED("cfgBin is deprecated");

        // FIXME[Robb P Matzke 2017-02-13]: unused?
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
    IS_SERIALIZABLE(AsmOperandList);

#ifdef DOCUMENTATION
    /** List of operands for an instruction. */
    class SgAsmOperandList: public SgAsmNode {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Ordered list of instruction operands.
         *
         *  The operand list is its own Sage node type (rather than operands being stored directly in the instruction node)
         *  because of limitations of ROSETTA.
         *
         * @{ */
        const SgAsmExpressionPtrList& get_operands() const;
        void set_oerands(const SgAsmExpressionPtrList&);
        /** @} */
#else
        AsmOperandList.setDataPrototype("SgAsmExpressionPtrList", "operands", "",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmOperandList);
#if defined(SgAsmOperandList_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmNode);
            s & BOOST_SERIALIZATION_NVP(p_operands);
        }
#endif

    public:
        /** Append another operand expression to this node. */
        void append_operand(SgAsmExpression* operand);
#endif // SgAsmOperandList_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    // FIXME[Robb P Matzke 2016-10-31]
    AsmOperandList.setFunctionSource("SOURCE_BINARY_OPERAND_LIST", "../Grammar/BinaryInstruction.code");

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmBinaryAdd);
    IS_SERIALIZABLE(AsmBinaryAdd);

#ifdef DOCUMENTATION
    /** Expression that adds two operands. */
    class SgAsmBinaryAdd: public SgAsmBinaryExpression {
    public:
#endif

        DECLARE_OTHERS(AsmBinaryAdd);
#if defined(SgAsmBinaryAdd_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmBinaryExpression);
        }
#endif
#endif // SgAsmBinaryAdd_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmBinarySubtract);
    IS_SERIALIZABLE(AsmBinarySubtract);

#ifdef DOCUMENTATION
    /** Expression that subtracts the second operand from the first. */
    class SgAsmBinarySubtract: public SgAsmBinaryExpression {
    public:
#endif

        DECLARE_OTHERS(AsmBinarySubtract);
#if defined(SgAsmBinarySubtract_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmBinaryExpression);
        }
#endif
#endif // SgAsmBinarySubtract_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmBinaryMultiply);
    IS_SERIALIZABLE(AsmBinaryMultiply);

#ifdef DOCUMENTATION
    /** Expression that multiplies two operands. */
    class SgAsmBinaryMultiply: public SgAsmBinaryExpression {
    public:
#endif

        DECLARE_OTHERS(AsmBinaryMultiply);
#if defined(SgAsmBinaryMultiply_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmBinaryExpression);
        }
#endif
#endif // SgAsmBinaryMultiply_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmBinaryDivide);
    IS_SERIALIZABLE(AsmBinaryDivide);

#ifdef DOCUMENTATION
    /** Expression that divides the first operand by the second. */
    class SgAsmBinaryDivide: public SgAsmBinaryExpression {
    public:
#endif

        DECLARE_OTHERS(AsmBinaryDivide);
#if defined(SgAsmBinaryDivide_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmBinaryExpression);
        }
#endif
#endif // SgAsmBinaryDivide_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmBinaryMod);
    IS_SERIALIZABLE(AsmBinaryMod);

#ifdef DOCUMENTATION
    /** Expression that returns the remainder when dividing the first operand by the second. */
    class SgAsmBinaryMod: public SgAsmBinaryExpression {
    public:
#endif

        DECLARE_OTHERS(AsmBinaryMod);
#if defined(SgAsmBinaryMod_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmBinaryExpression);
        }
#endif
#endif // SgAsmBinaryMod_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmBinaryAddPreupdate);
    IS_SERIALIZABLE(AsmBinaryAddPreupdate);

#ifdef DOCUMENTATION
    /** Expression that performs a pre-increment operation. */
    class SgAsmBinaryAddPreupdate: public SgAsmBinaryExpression {
    public:
#endif

        DECLARE_OTHERS(AsmBinaryAddPreupdate);
#if defined(SgAsmBinaryAddPreupdate_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmBinaryExpression);
        }
#endif
#endif // SgAsmBinaryAddPreupdate_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmBinarySubtractPreupdate);
    IS_SERIALIZABLE(AsmBinarySubtractPreupdate);

#ifdef DOCUMENTATION
    /** Expression that performs a pre-decrement operation. */
    class SgAsmBinarySubtractPreupdate: public SgAsmBinaryExpression {
    public:
#endif

        DECLARE_OTHERS(AsmBinarySubtractPreupdate);
#if defined(SgAsmBinarySubtractPreupdate_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmBinaryExpression);
        }
#endif
#endif // SgAsmBinarySubtractPreupdate_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmBinaryAddPostupdate);
    IS_SERIALIZABLE(AsmBinaryAddPostupdate);

#ifdef DOCUMENTATION
    /** Expression that performs a post-increment operation. */
    class SgAsmBinaryAddPostupdate: public SgAsmBinaryExpression {
    public:
#endif

        DECLARE_OTHERS(AsmBinaryAddPostupdate);
#if defined(SgAsmBinaryAddPostupdate_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmBinaryExpression);
        }
#endif
#endif // SgAsmBinaryAddPostupdate_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmBinarySubtractPostupdate);
    IS_SERIALIZABLE(AsmBinarySubtractPostupdate);

#ifdef DOCUMENTATION
    /** Expression that performs a post-decrement operation. */
    class SgAsmBinarySubtractPostupdate: public SgAsmBinaryExpression {
    public:
#endif

        DECLARE_OTHERS(AsmBinarySubtractPostupdate);
#if defined(SgAsmBinarySubtractPostupdate_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmBinaryExpression);
        }
#endif
#endif // SgAsmBinarySubtractPostupdate_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmBinaryLsl);
    IS_SERIALIZABLE(AsmBinaryLsl);

#ifdef DOCUMENTATION
    /** Expression that performs a logical left shift operation. */
    class SgAsmBinaryLsl: public SgAsmBinaryExpression {
    public:
#endif

        DECLARE_OTHERS(AsmBinaryLsl);
#if defined(SgAsmBinaryLsl_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmBinaryExpression);
        }
#endif
#endif // SgAsmBinaryLsl_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmBinaryLsr);
    IS_SERIALIZABLE(AsmBinaryLsr);

#ifdef DOCUMENTATION
    /** Expression that performs a logical, sign-bit non-preserving right shift. */
    class SgAsmBinaryLsr: public SgAsmBinaryExpression {
    public:
#endif

        DECLARE_OTHERS(AsmBinaryLsr);
#if defined(SgAsmBinaryLsr_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmBinaryExpression);
        }
#endif
#endif // SgAsmBinaryLsr_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmBinaryAsr);
    IS_SERIALIZABLE(AsmBinaryAsr);

#ifdef DOCUMENTATION
    /** Expression that performs an arithmetic, sign-bit preserving right shift. */
    class SgAsmBinaryAsr: public SgAsmBinaryExpression {
    public:
#endif

        DECLARE_OTHERS(AsmBinaryAsr);
#if defined(SgAsmBinaryAsr_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmBinaryExpression);
        }
#endif
#endif // SgAsmBinaryAsr_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmBinaryRor);
    IS_SERIALIZABLE(AsmBinaryRor);

#ifdef DOCUMENTATION
    /** Expression that performs a right rotate. */
    class SgAsmBinaryRor: public SgAsmBinaryExpression {
    public:
#endif

        DECLARE_OTHERS(AsmBinaryRor);
#if defined(SgAsmBinaryRor_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmBinaryExpression);
        }
#endif
#endif // SgAsmBinaryRor_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NEW_NONTERMINAL_MACRO(AsmBinaryExpression,
                          AsmBinaryAdd               | AsmBinarySubtract      | AsmBinaryMultiply           |
                          AsmBinaryDivide            | AsmBinaryMod           | AsmBinaryAddPreupdate       |
                          AsmBinarySubtractPreupdate | AsmBinaryAddPostupdate | AsmBinarySubtractPostupdate |
                          AsmBinaryLsl               | AsmBinaryLsr           | AsmBinaryAsr                |
                          AsmBinaryRor,
                          "AsmBinaryExpression", "AsmBinaryExpressionTag", false);
    AsmBinaryExpression.setCppCondition("!defined(DOCUMENTATION)");
    IS_SERIALIZABLE(AsmBinaryExpression);

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

        DECLARE_OTHERS(AsmBinaryExpression);
#if defined(SgAsmBinaryExpression_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExpression);
            s & BOOST_SERIALIZATION_NVP(p_lhs);
            s & BOOST_SERIALIZATION_NVP(p_rhs);
        }
#endif
#endif // SgAsmBinaryExpression_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmUnaryPlus);
    IS_SERIALIZABLE(AsmUnaryPlus);

#ifdef DOCUMENTATION
    /** Expression representing a (no-op) unary plus operation. */
    class SgAsmUnaryPlus: public SgAsmUnaryExpression {
    public:
#endif

        DECLARE_OTHERS(AsmUnaryPlus);
#if defined(SgAsmUnaryPlus_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmUnaryExpression);
        }
#endif
#endif // SgAsmUnaryPlus_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmUnaryMinus);
    IS_SERIALIZABLE(AsmUnaryMinus);

#ifdef DOCUMENTATION
    /** Expression represting negation. */
    class SgAsmUnaryMinus: public SgAsmUnaryExpression {
    public:
#endif

        DECLARE_OTHERS(AsmUnaryMinus);
#if defined(SgAsmUnaryMinus_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmUnaryExpression);
        }
#endif
#endif // SgAsmUnaryMinus_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmUnaryRrx);
    IS_SERIALIZABLE(AsmUnaryRrx);

#ifdef DOCUMENTATION
    // FIXME[Robb P Matzke 2016-10-31]: no idea what this is
    class SgAsmUnaryRrx: public SgAsmUnaryExpression {
    public:
#endif

        DECLARE_OTHERS(AsmUnaryRrx);
#if defined(SgAsmUnaryRrx_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmUnaryExpression);
        }
#endif
#endif // SgAsmUnaryRrx_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmUnaryArmSpecialRegisterList);
    IS_SERIALIZABLE(AsmUnaryArmSpecialRegisterList);

#ifdef DOCUMENTATION
    // FIXME[Robb P Matzke 2016-10-31]: no idea what this is
    class SgAsmUnaryArmSpecialRegisterList: public SgAsmUnaryExpression {
    public:
#endif

        DECLARE_OTHERS(AsmUnaryArmSpecialRegisterList);
#if defined(SgAsmUnaryArmSpecialRegisterList_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmUnaryExpression);
        }
#endif
#endif // SgAsmUnaryArmSpecialRegisterList_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NEW_NONTERMINAL_MACRO(AsmUnaryExpression,
                          AsmUnaryPlus | AsmUnaryMinus | AsmUnaryRrx | AsmUnaryArmSpecialRegisterList,
                          "AsmUnaryExpression", "AsmUnaryExpressionTag", false);
    AsmUnaryExpression.setCppCondition("!defined(DOCUMENTATION)");
    IS_SERIALIZABLE(AsmUnaryExpression);

#ifdef DOCUMENTATION
    /** Base class for unary expressions. */
    class SgAsmUnaryExpression: public SgAsmExpression {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Operand for a unary expression.
         *
         * @{ */
        SgAsmExpression* get_operand() const;
        void set_operand(SgAsmExpression*);
        /** @} */
#else
        AsmUnaryExpression.setDataPrototype("SgAsmExpression*", "operand", "= NULL",
                                            CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmUnaryExpression);
#if defined(SgAsmUnaryExpression_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExpression);
            s & BOOST_SERIALIZATION_NVP(p_operand);
        }
#endif
#endif // SgAsmUnaryExpression_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmDirectRegisterExpression);
    IS_SERIALIZABLE(AsmDirectRegisterExpression);

#ifdef DOCUMENTATION
    /** Expression representing a machine register. */
    class SgAsmDirectRegisterExpression: public SgAsmRegisterReferenceExpression {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: PSR mask for ARM architectures.
         *
         * @{ */
        unsigned get_psr_mask() const;
        void set_psr_mask(unsigned);
        /** @} */
#else
        AsmDirectRegisterExpression.setDataPrototype("unsigned", "psr_mask", "=0", // for ARM
                                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL,
                                                     NO_DELETE);
#endif

        DECLARE_OTHERS(AsmDirectRegisterExpression);
#if defined(SgAsmDirectRegisterExpression_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmRegisterReferenceExpression);
            s & BOOST_SERIALIZATION_NVP(p_psr_mask);
        }
#endif

    private:
        // Default c'tor needed for serialization
        SgAsmDirectRegisterExpression()
            : p_psr_mask(0) {}
#endif // SgAsmDirectRegisterExpression_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmIndirectRegisterExpression);
    IS_SERIALIZABLE(AsmIndirectRegisterExpression);

#ifdef DOCUMENTATION
    /** Registers accessed indirectly.
     *
     *  An indirect register is a register whose descriptor is computed dynamically.  The dynamic descriptor is computed by
     *  reading "offset" register to obtain an integral value and adding it to the integral "index" modulo the specified
     *  "modulus" to obtain an integer @ref i. (These are all properties of this node.)  Then, the final register descriptor
     *  is calculated by adding @ref i times "stride" to the base register descriptor (@ref get_descriptor). The @ref i times
     *  "stride" multiplies each member of "stride" by @em i, and addition of two register descriptors is defined as the
     *  pair-wise addition of their elements.
     *
     *  An example of an indirect register is x86 ST(1) which has base register "st", stride={0,1,0,0}, offset
     *  register "fpstatus_top", index is 1, and modulus is 8. Thus, the dynamically-computed register is:
     *
     *  @code
     *   i = (read("fpstatus_top") + index) % modulo;
     *   result = descriptor("st") + {0,1,0,0} * i;
     *  @endcode */
    class SgAsmIndirectRegisterExpression: public SgAsmRegisterReferenceExpression {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Register descriptor stride.
         *
         *  This is the "stride" referred to in the documentation for this class.  This is not an actual register.
         *
         * @{ */
        const RegisterDescriptor& get_stride() const;
        void set_stride(const RegisterDescriptor&);
        /** @} */
#else
        AsmIndirectRegisterExpression.setDataPrototype("RegisterDescriptor", "stride", "",
                                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Register descriptor offset.
         *
         *  This is the "offset" referred to in the documentation for this class.
         *
         * @{ */
        const RegisterDescriptor& get_offset() const;
        void set_offset(const RegisterDescriptor&);
        /** @} */
#else
        AsmIndirectRegisterExpression.setDataPrototype("RegisterDescriptor", "offset", "",
                                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Register descriptor index.
         *
         *  This is the "index" referred to in the documentation for this class.
         *
         * @{ */
        size_t get_index() const;
        void set_index(size_t);
        /** @} */
#else
        AsmIndirectRegisterExpression.setDataPrototype("size_t", "index", "",
                                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Register descriptor modulus.
         *
         *  This is the "modulus" referred to in the documentation for this class.
         *
         * @{ */
        size_t get_modulus() const;
        void set_modulus(size_t);
        /** @} */
#else
        AsmIndirectRegisterExpression.setDataPrototype("size_t", "modulus", "",
                                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmIndirectRegisterExpression);
#if defined(SgAsmIndirectRegisterExpression_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmRegisterReferenceExpression);
            s & BOOST_SERIALIZATION_NVP(p_stride);
            s & BOOST_SERIALIZATION_NVP(p_offset);
            s & BOOST_SERIALIZATION_NVP(p_index);
            s & BOOST_SERIALIZATION_NVP(p_modulus);
        }
#endif

    private:
        // Default c'tor needed for serialization
        SgAsmIndirectRegisterExpression()
            : p_index(0), p_modulus(0) {}
#endif // SgAsmIndirectRegisterExpression_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NEW_NONTERMINAL_MACRO(AsmRegisterReferenceExpression, AsmDirectRegisterExpression|AsmIndirectRegisterExpression,
                          "AsmRegisterReferenceExpression", "AsmRegisterReferenceExpressionTag", false);
    AsmRegisterReferenceExpression.setCppCondition("!defined(DOCUMENTATION)");
    IS_SERIALIZABLE(AsmRegisterReferenceExpression);

#ifdef DOCUMENTATION
    /** Base class for references to a machine register. */
    class SgAsmRegisterReferenceExpression: public SgAsmExpression {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Descriptor for accessed register.
         *
         *  @{ */
        const RegisterDescriptor& get_descriptor() const;
        void set_descriptor(const RegisterDescriptor&);
        /** @} */
#else
        AsmRegisterReferenceExpression.setDataPrototype("RegisterDescriptor", "descriptor", "",
                                                        CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Post-increment or pre-decrement amount.
         *
         *  This is a value that's added or subtracted from a register each time the containing instruction is executed.
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

        DECLARE_OTHERS(AsmRegisterReferenceExpression);
#if defined(SgAsmRegisterReferenceExpression_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExpression);
            s & BOOST_SERIALIZATION_NVP(p_descriptor);
            s & BOOST_SERIALIZATION_NVP(p_adjustment);
        }
#endif

    protected:
        // Default c'tor needed for serialization
        SgAsmRegisterReferenceExpression()
            : p_adjustment(0) {}
#endif // SgAsmRegisterReferenceExpression_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmRegisterNames);
    IS_SERIALIZABLE(AsmRegisterNames);

#ifdef DOCUMENTATION
    /** An ordered list of registers. */
    class SgAsmRegisterNames: public SgAsmExpression {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: List of registers.
         *
         *  These are the actual registers contained in this node.  Registers are stored in this node instead of directly in a
         *  @ref SgAsmRegisterNames node because of limitations of ROSETTA.
         *
         * @{ */
        const SgAsmRegisterReferenceExpressionPtrList& get_registers() const;
        void set_registers(const SgAsmRegisterReferenceExpressionPtrList&);
        /** @} */
#else
        AsmRegisterNames.setDataPrototype("SgAsmRegisterReferenceExpressionPtrList", "registers", "",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Bit mask.
         *
         *  This is an optional bit mask representing the registers. It's used by the m68k disassembler and represents the mask
         *  that appeared in the encoded instruction.
         *
         * @{ */
        unsigned get_mask() const;
        void set_mask(unsigned);
        /** @} */
#else
        AsmRegisterNames.setDataPrototype("unsigned", "mask", "=0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmRegisterNames);
#if defined(SgAsmRegisterNames_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExpression);
            s & BOOST_SERIALIZATION_NVP(p_registers);
            s & BOOST_SERIALIZATION_NVP(p_mask);
        }
#endif
#endif // SgAsmRegisterNames_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmIntegerValueExpression);
    IS_SERIALIZABLE(AsmIntegerValueExpression);

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
         * The base node is not considered to be a child of this node in the AST.
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
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmConstantExpression);
            s & BOOST_SERIALIZATION_NVP(p_baseNode);
        }
#endif

    public:
        /** Construct a new value of specified type.
         *
         *  Creates a new AST node having value @p n of type @p type. See also, the constructor that takes a bit vector as the
         *  first argument, which is useful when the value is non-integral or is too large to fit in a @c uint64_t. */
        SgAsmIntegerValueExpression(uint64_t n, SgAsmType *type);

        /** Construct a new value of specified type.
         *
         *  Creates a new AST node having value @p bv of type @p type.  See also, the constructor that takes a @c uint64_t as
         *  the first argument which might be simpler to call in situations where the value is an integral type not more than
         *  64 bits. */
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
    IS_SERIALIZABLE(AsmFloatValueExpression);

#ifdef DOCUMENTATION
    /** Floating-point value.
     *
     *  The bits are stored in the super-class (SgAsmConstantExpression) and interpretted as various kinds of floating-point
     *  values. */
    class SgAsmFloatValueExpression: public SgAsmConstantExpression {
    public:
#endif

        AsmFloatValueExpression.setAutomaticGenerationOfConstructor(false); // so p_nativeValue cache can be initialized

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

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmConstantExpression);
            s & BOOST_SERIALIZATION_NVP(p_nativeValue);
            s & BOOST_SERIALIZATION_NVP(p_nativeValueIsValid);
        }
#endif

    public:
        /** Default constructor.
         *
         *  Creates a new floating-point value of unspecified type initialized to positive zero.  We recommend using a
         *  constructor that takes a @ref SgAsmType argument instead. */
        SgAsmFloatValueExpression(): p_nativeValue(0.0), p_nativeValueIsValid(true) {}

        /** Construct specified floating-point value.
         *
         *  Creates a new floating-point constant AST node having the specified native value and type. See also, the
         *  constructor that takes a bit vector as its first argument, which is useful in cases where the native representation
         *  of a floating point value does not match the target machine's representation. */
        SgAsmFloatValueExpression(double nativeValue, SgAsmType*);

        /** Construct specified floating-point value.
         *
         *  Creates a new floating-point constant AST node having the specified value and type.  See also, the constructor that
         *  takes a @c double argument for those cases when the native representation matches the target machine's
         *  representation. */
        SgAsmFloatValueExpression(const Sawyer::Container::BitVector&,SgAsmType*);

        /** Set AST node value to specified native value. */
        void set_nativeValue(double);

        /** Get value in native format. */
        double get_nativeValue() const;

        /** Update bit vector from native representation.
         *
         *  Resets the bit vector using the cached native representation.  The bit vector is considered to be the canonical
         *  value of this AST node. */
        void updateBitVector();

        /** Update cached native value from bit vector.
         *
         *  Resets the cached native value from the bit vector. The bit vector is considered to be the canonical value of this
         *  AST node, and the native format value is a cached representation that can be used in arithmetic. */
        void updateNativeValue() const;
#endif // SgAsmFloatValueExpression_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NEW_NONTERMINAL_MACRO(AsmConstantExpression,
                          AsmIntegerValueExpression | AsmFloatValueExpression,
                          "AsmConstantExpression", "AsmConstantExpressionTag", false);
    AsmConstantExpression.setCppCondition("!defined(DOCUMENTATION)");
    IS_SERIALIZABLE(AsmConstantExpression);

    DECLARE_HEADERS(AsmConstantExpression);
#if defined(SgAsmConstantExpression_HEADERS) || defined(DOCUMENTATION)
    #include <Sawyer/BitVector.h>
#endif // SgAsmConstantExpression_HEADERS

#ifdef DOCUMENTATION
    /** Base class for constants.
     *
     *  Represents integer values, floating-point values, etc. This class holds the actual bits for the constant value.
     *  Subclasses provide the intepretation of those bits. */
    class SgAsmConstantExpression: public SgAsmValueExpression {
    public:
#endif

#ifndef DOCUMENTATION
        // Documented below. Implemented below due to ROSETTA limitations.
        AsmConstantExpression.setDataPrototype("Sawyer::Container::BitVector", "bitVector", "",
                                               NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmConstantExpression);
#if defined(SgAsmConstantExpression_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmValueExpression);
            s & BOOST_SERIALIZATION_NVP(p_bitVector);
        }
#endif

    public:
        /** Property: Bits for constant.
         *
         *  This is the canonical value of an AST node; subclasses may cache native representations of this value.
         *
         *  @{ */
        const Sawyer::Container::BitVector& get_bitVector() const { return p_bitVector; }
        Sawyer::Container::BitVector& get_bitVector() { return p_bitVector; }
        void set_bitVector(const Sawyer::Container::BitVector &bv) { p_bitVector = bv; }
        /** @} */
#endif // SgAsmConstantExpression_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NEW_NONTERMINAL_MACRO(AsmValueExpression,
                          AsmConstantExpression,
                          "AsmValueExpression", "AsmValueExpressionTag", false);
    AsmValueExpression.setCppCondition("!defined(DOCUMENTATION)");
    IS_SERIALIZABLE(AsmValueExpression);

#ifdef DOCUMENTATION
    /** Base class for values.
     *
     *  Values that are addresses or references to data will have symbols in a function symbol table.  All other values are
     *  assumed to be literals and will not have associated symbols. */
    class SgAsmValueExpression: public SgAsmExpression {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Alternate expression without constant folding.
         *
         *  This is an optional expression that's equivalent to this expression but without constant folding.
         *
         *  Note: All of the ROSE disassemblers always set this to null regardless of whether they do any constant folding.
         *
         * @{ */
        SgAsmValueExpression* get_unfolded_expression_tree() const;
        void set_unfolded_expression_tree(SgAsmValueExpression*);
        /** @} */
#else
        AsmValueExpression.setDataPrototype("SgAsmValueExpression*", "unfolded_expression_tree", "= NULL",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Where this expression is encoded within the instruction.
         *
         *  This is the bit offset into the instruction's raw bytes where this expression is encoded. If it is not supported by
         *  the architectures, it will be set to zero and the "bit_size" property will also be zero.
         *
         * @{ */
        unsigned short get_bit_offset() const;
        void set_bit_offset(unsigned short);
        /** @} */
#else
        AsmValueExpression.setDataPrototype("unsigned short", "bit_offset", "= 0",         // DOXYGEN
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Where this expression is encoded within the instruction.
         *
         *  This is the size in bits of the encoding for this expression within the instruction.  If it is not supported by
         *  the architecture, it will be set to zero.
         *
         * @{ */
        unsigned short get_bit_size() const;
        void set_bit_size(unsigned short);
        /** @} */
#else
        AsmValueExpression.setDataPrototype("unsigned short", "bit_size", "= 0",           // DOXYGEN
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Symbol corresponding to this expression.
         *
         *  If a symbol refers to this expression then it is linked by this property, otherwise null.  The symbol is not
         *  considered to be a child of this node in the AST.
         *
         * @{ */
        SgSymbol* get_symbol() const;
        void set_symbol(SgSymbol*);
        /** @} */
#else
        AsmValueExpression.setDataPrototype("SgSymbol*", "symbol", "= NULL",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmValueExpression);
#if defined(SgAsmValueExpression_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExpression);
            s & BOOST_SERIALIZATION_NVP(p_unfolded_expression_tree);
            s & BOOST_SERIALIZATION_NVP(p_bit_offset);
            s & BOOST_SERIALIZATION_NVP(p_bit_size);
#if 1
            ASSERT_require2(p_symbol == NULL, "not implemented yet");
#else
            s & BOOST_SERIALIZATION_NVP(p_symbol);
#endif
        }
#endif
#endif // SgAsmValueExpression_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmMemoryReferenceExpression);
    IS_SERIALIZABLE(AsmMemoryReferenceExpression);

#ifdef DOCUMENTATION
    /** Reference to memory locations. */
    class SgAsmMemoryReferenceExpression: public SgAsmExpression {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Memory address expression.
         *
         *  This property stores the address of the memory reference.  If the reference is for multiple bytes of memory, then
         *  only one address is stored and the instruction semantics determine which address it is. Usually multi-byte
         *  references store the lowest address.
         *
         * @{ */
        SgAsmExpression* get_address() const;
        void set_address(SgAsmExpression*);
        /** @} */
#else
        AsmMemoryReferenceExpression.setDataPrototype("SgAsmExpression*", "address", "= NULL",
                                                      CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Optional memory segment register.
         *
         *  If a segment register is specified then the actual memory address is formed by adding the stored memory address to
         *  the current value of the segment register.  The x86 architecture is the only ROSE architecture that uses segment
         *  registers, and most of the time they are initialized to zero by the instruction semantics.
         *
         * @{ */
        SgAsmExpression* get_segment() const;
        void set_segment(SgAsmExpression*);
        /** @} */
#else
        AsmMemoryReferenceExpression.setDataPrototype("SgAsmExpression*", "segment", "= NULL",
                                                      CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmMemoryReferenceExpression);
#if defined(SgAsmMemoryReferenceExpression_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExpression);
            s & BOOST_SERIALIZATION_NVP(p_address);
            s & BOOST_SERIALIZATION_NVP(p_segment);
        }
#endif
#endif // SgAsmMemoryReferenceExpression_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmControlFlagsExpression);
    IS_SERIALIZABLE(AsmControlFlagsExpression);

#ifdef DOCUMENTATION
    // FIXME[Robb P Matzke 2016-10-31]: no idea what this is
    class SgAsmControlFlagsExpression: public SgAsmExpression {
    public:
#endif

#ifndef DOCUMENTATION
        AsmControlFlagsExpression.setDataPrototype("unsigned long", "bit_flags", "= 0",
                                                   NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmControlFlagsExpression);
#if defined(SgAsmControlFlagsExpression_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExpression);
            s & BOOST_SERIALIZATION_NVP(p_bit_flags);
        }
#endif
#endif // SgAsmControlFlagsExpression_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmCommonSubExpression);
    IS_SERIALIZABLE(AsmCommonSubExpression);

#ifdef DOCUMENTATION
    // FIXME[Robb P Matzke 2016-10-31]: no idea what this is
    class SgAsmCommonSubexpression: public SgAsmExpression {
    public:
#endif

#ifndef DOCUMENTATION
        AsmCommonSubExpression.setDataPrototype("SgAsmExpression*", "subexpression", "= 0",
                                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmCommonSubExpression);
#if defined(SgAsmCommonSubExpression_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExpression);
            s & BOOST_SERIALIZATION_NVP(p_subexpression);
        }
#endif
#endif // SgAsmCommonSubExpression_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmRiscOperation);
    IS_SERIALIZABLE(AsmRiscOperation);

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

#ifdef DOCUMENTATION
        /** Property: Low-level semantic operation.
         *
         *  This property is an enum constant that represents an operation in @ref
         *  rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RiscOperators "RiscOperators".
         *
         * @{ */
        RiscOperator get_riscOperator() const;
        void set_riscOperator(RiscOperator);
        /** @} */
#else
        AsmRiscOperation.setDataPrototype("SgAsmRiscOperation::RiscOperator", "riscOperator", "= SgAsmRiscOperation::OP_NONE",
                                          CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
#endif

#ifdef DOCUMENTATION
        /** Property: Operands for the low-level operation.
         *
         *  These are the operands used by the low-level operation. The actual list is stored in a separate @ref
         *  SgAsmExprListExpr AST node instead of directly in this node due to limitations of ROSETTA.
         *
         * @{ */
        SgAsmExprListExp* get_operands() const;
        void set_operands(SgAsmExprListExp*);
        /** @} */
#else
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
            OP_add,                                     /**< Two args + optional carry bit. */
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

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExpression);
            s & BOOST_SERIALIZATION_NVP(p_riscOperator);
            s & BOOST_SERIALIZATION_NVP(p_operands);
        }
#endif
#endif // SgAsmRiscOperation_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmExprListExp);
    IS_SERIALIZABLE(AsmExprListExp);

#ifdef DOCUMENTATION
    /** List of expression nodes. */
    class SgAsmExprListExp: public SgAsmExpression {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Pointers to expressions.
         *
         *  List of expression nodes are stored in their own @ref SgAsmExprListExp node instead of in parent AST nodes due to
         *  limitations of ROSETTA.
         *
         * @{ */
        const SgAsmExpressionPtrList& get_expressions() const;
        void set_expressions(const SgAsmExpressionPtrList&);
        /** @} */
#else
        AsmExprListExp.setDataPrototype("SgAsmExpressionPtrList", "expressions", "",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmExprListExp);
#if defined(SgAsmExprListExp_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExpression);
            s & BOOST_SERIALIZATION_NVP(p_expressions);
        }
#endif
#endif // SgAsmExprListExp_OTHERS

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
    AsmExpression.setCppCondition("!defined(DOCUMENTATION)");
    IS_SERIALIZABLE(AsmExpression);

#ifdef DOCUMENTATION
    /** Base class for expressions. */
    class SgAsmExpression: public SgAsmNode {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Type of expression.
         *
         *  The type node is not considered a child of this node in the AST.
         *
         * @{ */
        SgAsmType* get_type() const;
        void set_type(SgAsmType*);
        /** @} */
#else
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

#ifdef DOCUMENTATION
        /** Property: Comment.
         *
         *  User-defined comment for an expression.
         *
         * @{ */
        const std::string& get_comment() const;
        void set_comment(const std::string&);
        /** @} */
#else
        AsmExpression.setDataPrototype("std::string", "comment", "= \"\"",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmExpression);
#if defined(SgAsmExpression_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmNode);
            s & BOOST_SERIALIZATION_NVP(p_type);
        }
#endif

    public:
        /** Property: Width of expression in bits.
         *
         *  Returns the width of the expression in bits according to its data type. The "type" property must be non-null. */
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
    IS_SERIALIZABLE(AsmIntegerType);

#ifdef DOCUMENTATION
    /** Integer types. */
    class SgAsmIntegerType: public SgAsmScalarType {
    public:
#endif

#ifndef DOCUMENTATION
        // Documented below due to ROSETTA limitations
        AsmIntegerType.setDataPrototype("bool", "isSigned", "=false", // read-only
                                        NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmIntegerType);
#if defined(SgAsmIntegerType_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmScalarType);
            s & BOOST_SERIALIZATION_NVP(p_isSigned);
        }
#endif

    public:
        /** Constructor with specified member values.
         *
         *  Constructs a new integer type and initializes its properties according to the arguments. */
        SgAsmIntegerType(ByteOrder::Endianness, size_t nBits, bool isSigned);

        /** Property: whether the integral type is signed or unsigned.
         *
         *  Returns true if this is a signed type, otherwise false. */
        bool get_isSigned() const;

        // Overrides documented in base class
        virtual void check() const ROSE_OVERRIDE;
        virtual std::string toString() const ROSE_OVERRIDE;
#endif // SgAsmIntegerType_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmFloatType);
    IS_SERIALIZABLE(AsmFloatType);

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
        // Documented below because of ROSETTA limitations (they're read-only)
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
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmScalarType);
            s & BOOST_SERIALIZATION_NVP(p_significandOffset);
            s & BOOST_SERIALIZATION_NVP(p_significandNBits);
            s & BOOST_SERIALIZATION_NVP(p_signBitOffset);
            s & BOOST_SERIALIZATION_NVP(p_exponentOffset);
            s & BOOST_SERIALIZATION_NVP(p_exponentNBits);
            s & BOOST_SERIALIZATION_NVP(p_exponentBias);
            s & BOOST_SERIALIZATION_NVP(p_flags);
        }
#endif

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

        // Overrides documented in base class
        virtual void check() const ROSE_OVERRIDE;
        virtual std::string toString() const ROSE_OVERRIDE;
#endif // SgAsmFloatType_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NEW_NONTERMINAL_MACRO(AsmScalarType,
                          AsmIntegerType | AsmFloatType,
                          "AsmScalarType", "AsmScalarTypeTag", false);
    AsmScalarType.setCppCondition("!defined(DOCUMENTATION)");
    IS_SERIALIZABLE(AsmScalarType);

#ifdef DOCUMENTATION
    /** Base class for scalar types. */
    class SgAsmScalarType: public SgAsmType {
    public:
#endif

#ifndef DOCUMENTATION
        // Documented below due to ROSETTA limitations (read-only)
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
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmType);
            s & BOOST_SERIALIZATION_NVP(p_minorOrder);
            s & BOOST_SERIALIZATION_NVP(p_majorOrder);
            s & BOOST_SERIALIZATION_NVP(p_majorNBytes);
            s & BOOST_SERIALIZATION_NVP(p_nBits);
        }
#endif

    protected:
        /** Construct a new scalar type.
         *
         *  Since scalar types are base classes, one normally does not construct just a scalar type but rather one of the base
         *  classes. */
        SgAsmScalarType(ByteOrder::Endianness, size_t nBits);

    public:
        /** Property: Number of bits. */
        virtual size_t get_nBits() const ROSE_OVERRIDE;

        /** Property: Minor byte order. This is the usual notion of byte order. */
        ByteOrder::Endianness get_minorOrder() const;

        /** Property: Major byte order for mixed-order types. */
        ByteOrder::Endianness get_majorOrder() const;

        /** Property: Stride of major byte order for mixed order types. */
        size_t get_majorNBytes() const;

        // Overrides documented in base class
        virtual void check() const ROSE_OVERRIDE;
        virtual std::string toString() const ROSE_OVERRIDE;
#endif // SgAsmScalarType_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmVectorType);
    IS_SERIALIZABLE(AsmVectorType);

#ifdef DOCUMENTATION
    /** Base class for vector types. */
    class SgAsmVectorType: public SgAsmType {
    public:
#endif

#ifndef DOCUMENTATION
        // Documented below due to ROSETTA limitations (read-only)
        AsmVectorType.setDataPrototype("size_t", "nElmts", "=0", // read-only
                                       NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
        AsmVectorType.setDataPrototype("SgAsmType*", "elmtType", "=NULL", // read-only
                                       NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmVectorType);
#if defined(SgAsmVectorType_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmType);
            s & BOOST_SERIALIZATION_NVP(p_nElmts);
            s & BOOST_SERIALIZATION_NVP(p_elmtType);
        }
#endif

    public:
        /** Construct a new vector type. */
        SgAsmVectorType(size_t nElmts, SgAsmType *elmtType);

        /** Property: Number of elements in vector. */
        size_t get_nElmts() const;

        /** Property: Type of each vector element. */
        SgAsmType* get_elmtType() const;

        // Overrides documented in base class
        virtual void check() const ROSE_OVERRIDE;
        virtual std::string toString() const ROSE_OVERRIDE;
        virtual size_t get_nBits() const ROSE_OVERRIDE;
#endif // SgAsmVectorType_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NEW_NONTERMINAL_MACRO(AsmType, AsmScalarType | AsmVectorType, "AsmType", "AsmTypeTag", false);
    AsmType.setCppCondition("!defined(DOCUMENTATION)");
    IS_SERIALIZABLE(AsmType);

#ifdef DOCUMENTATION
    /** Base class for binary types. */
    class SgAsmType: public SgAsmNode {
    public:
#endif

        DECLARE_OTHERS(AsmType);
#if defined(SgAsmType_OTHERS) || defined(DOCUMENTATION)
    private:
        static Sawyer::Container::Map<std::string, SgAsmType*> p_typeRegistry;

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmNode);
        }
#endif

    public:
        /** Validate properties collectively.
         *
         *  This method collectively validates the properties of a type since validation is not always possible or convenient
         *  when the user modifies an individual property.  For instance, when changing the bit fields in a floating-point
         *  type, it would be very inconvenient to the user if those properties individually validated there settings because
         *  it would require the user to shuffle fields around in such a way that they never overlap. This way, the overlap
         *  validation can occur after the user is all done moving the fields. */
        virtual void check() const;

        /** Convert a type to a string.
         *
         *  The output is intended mostly for debugging since it contains more details than what would be typically useful. For
         *  instance, instead of the word "double", this will probably print all the details about where the various
         *  floating-point fields are located, how the exponent field works, etc. */
        virtual std::string toString() const {
            abort();                                    // ROSETTA limitation: intended pure virtual
            return NULL;                                // Windows limitation: return value required [Too, 2014-08-11]
        }

        /** Width of type in bits. */
        virtual size_t get_nBits() const {
            abort();                                    // ROSETTA limitation: intended pure virtual
            return (size_t)-1;                          // Windows limitation: return value required [Too, 2014-08-11]
        }

        /** Width of type in bytes. */
        virtual size_t get_nBytes() const;

        /** Registers a type with the type system.
         *
         *  This method registers the specified type by its @ref toString value so it can be found later.  If a type by the
         *  same name is already registered then the specified one is deleted.  The return value is the type that is ultimately
         *  in the registry (either one that existed their previously or the specified type). */
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

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmFunction);
    IS_SERIALIZABLE(AsmFunction);

    DECLARE_HEADERS(AsmFunction);
#if defined(SgAsmFunction_HEADERS) || defined(DOCUMENTATION)
    namespace rose {
    namespace BinaryAnalysis {
    namespace CallingConvention {
    class Definition;
    } // namespace
    } // namespace
    } // namespace
#endif // SgAsmFunction_HEADERS

#ifdef DOCUMENTATION
    /** Represents a synthesized function.
     *
     *  A function is a collection of blocks holding instructions (basic blocks) or static data.  Instructions might have
     *  references to addresses or data which are described by symbols (not to be confused with the binary's symbol table) in
     *  the function's symbol table (@ref get_symbol_table).  Functions do not explicitly exist in a binary, but are
     *  synthesized by ROSE as part of the disassembly and partitioning steps. The individual instructions and/or individual
     *  static data areas need not cover a contiguous region of the address space.  Some synthesized functions will likely not
     *  be a "function" or "produceure" in the strict sense of those words due to such factors as compiler optimizations,
     *  hand-coded routines, exception handling, non-local branching, shortcomings of ROSE's partitioning solvers, etc. In any
     *  case, each function will have one primary entry address.  Although the AST requires that every function have its own
     *  basic block children, which have their own instructions (definition of "tree" data structures), logically two functions
     *  might share basic blocks, although this is uncommon.
     *
     *  Warning: Although currently basic blocks are direct children of function nodes in the AST, this may change in a future
     *  version of ROSE as new node types are introduced to describe scopes and control structures. */
    class SgAsmFunction: public SgAsmSynthesizedDeclaration {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Name.
         *
         *  The optional string name of a function.
         *
         * @{ */
        const std::string& get_name() const;
        void set_name(const std::string&);
        /** @} */
#else
        AsmFunction.setDataPrototype("std::string", "name", "= \"\"",
                                     CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Reason that function exists.
         *
         *  This is a bit vector of enum constants that describe why this function was created. See @ref
         *  SgAsmFunction::FunctionReason for details.
         *
         * @{ */
        unsigned get_reason() const;
        void set_reason(unsigned);
        /** @} */
#else
        AsmFunction.setDataPrototype("unsigned", "reason", "= SgAsmFunction::FUNC_NONE", /*bit flags*/
                                     CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Kind of function.
         *
         *  This enum constant describes the kind of function. See @ref SgAsmFunction::function_kind_enum for details.
         *
         * @{ */
        function_kind_enum get_function_kind() const;
        void set_function_kind(function_kind_enum);
        /** @} */
#else
        AsmFunction.setDataPrototype("SgAsmFunction::function_kind_enum", "function_kind", "= SgAsmFunction::e_unknown",
                                     CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
#endif

#ifdef DOCUMENTATION
        /** Property: Whether a function could return to its caller.
         *
         * @{ */
        MayReturn get_may_return() const;
        void set_may_return(MayReturn);
        /** @} */
#else
        AsmFunction.setDataPrototype("SgAsmFunction::MayReturn", "may_return", "= SgAsmFunction::RET_UNKNOWN",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Hash of the function.
         *
         *  This is unused by ROSE, but can be set by users to identify a function by hash string.
         *
         * @{ */
        const std::string& get_name_md5() const;
        void set_name_md5(const std::string&);
        /** @} */
#else
        AsmFunction.setDataPrototype("std::string", "name_md5", "= \"\"",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Statements that make up a function.
         *
         *  The "statements" of a function are things like instructions, static data blocks, etc.
         *
         * @{ */
        const SgAsmStatementPtrList& get_statementList() const;
        void set_statementList(const SgAsmStatementPtrList&);
        /** @} */
#else
        AsmFunction.setDataPrototype("SgAsmStatementPtrList", "statementList", "",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        // FIXME[Robb P Matzke 2017-02-13]: unused?
#else
        AsmFunction.setDataPrototype("SgAsmStatementPtrList", "dest", "",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Primary entry address.
         *
         *  Every function has one primary entry address that uniquely identifies the function in the AST. This is the starting
         *  address of the function's entry instruction.  The abbreviation "va" means "virtual address".
         *
         * @{ */
        rose_addr_t get_entry_va() const;
        void set_entry_va(rose_addr_t);
        /** @} */
#else
        AsmFunction.setDataPrototype("rose_addr_t", "entry_va", "= 0",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Symbol table.
         *
         *  A ROSE symbol table associated with this function. This is not the same as the symbol table that appears in the
         *  binary specimen, such as ELF or PE symbol tables.
         *
         * @{ */
        SgSymbolTable* get_symbol_table() const;
        void set_symbol_table(SgSymbolTable*);
        /** @} */
#else
        AsmFunction.setDataPrototype("SgSymbolTable*", "symbol_table", "= NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, DEF_DELETE, NO_COPY_DATA);
#endif

#ifdef DOCUMENTATION
        // FIXME[Robb P Matzke 2017-02-13]: what is this?
#else
        AsmFunction.setDataPrototype("size_t", "cached_vertex", "= (size_t)(-1)", // see BinaryAnalysis::FunctionCall
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Net effect of function on the stack pointer.
         *
         *  Net effect that this function has on the machine's stack pointer register.  For most x86 Linux ELF specimens the
         *  net effect is to pop the return address from the stack, thus +4 for 32-bit specimens and +8 for 64-bit specimens.
         *
         *  If the stack delta analysis has not run or could not determine a constant stack delta, then the special value @c
         *  SgAsmInstruction::INVALID_STACK_DELTA is used.
         *
         * @{ */
        int64_t get_stackDelta() const;
        void set_stackDelta(int64_t);
        /** @} */
#else
        AsmFunction.setDataPrototype("int64_t", "stackDelta", "= SgAsmInstruction::INVALID_STACK_DELTA",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Primary calling convention.
         *
         *  This is the primary calling convention for this function. When the semantics of the function match multiple
         *  calling convention definitions, this property holds the "best" one for some definition of "best". It is also
         *  possible for the semantics to not match any calling convention definition in which case this property is null. It
         *  is also null if the calling convention analysis was not performed.
         *
         * @{ */
        const rose::BinaryAnalysis::CallingConvention::Definition* get_callingConvention() const;
        void set_callingConvention(const rose::BinaryAnalysis::CallingConventionD::Definition*);
        /** @} */
#else
        AsmFunction.setDataPrototype("const rose::BinaryAnalysis::CallingConvention::Definition*", "callingConvention", "=NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmFunction);
#if defined(SgAsmFunction_OTHERS) || defined(DOCUMENTATION)

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmSynthesizedDeclaration);
            s & BOOST_SERIALIZATION_NVP(p_name);
            s & BOOST_SERIALIZATION_NVP(p_reason);
            s & BOOST_SERIALIZATION_NVP(p_function_kind);
            s & BOOST_SERIALIZATION_NVP(p_may_return);
            s & BOOST_SERIALIZATION_NVP(p_name_md5);
            s & BOOST_SERIALIZATION_NVP(p_statementList);
            s & BOOST_SERIALIZATION_NVP(p_dest);
            s & BOOST_SERIALIZATION_NVP(p_entry_va);
            //s & BOOST_SERIALIZATION_NVP(p_symbol_table); // not implemented yet
            s & BOOST_SERIALIZATION_NVP(p_cached_vertex);
            s & BOOST_SERIALIZATION_NVP(p_stackDelta);
            s & BOOST_SERIALIZATION_NVP(p_callingConvention);
        }
#endif

    public:
        // [Robb P Matzke 2017-02-13]: Deprecated because it uses a very old CFG implementation
        int nrOfValidInstructions(std::vector<SgNode*>& succs) ROSE_DEPRECATED("use modern CFG classes instead");

        /** Adds statement to end of statement list. */
        void append_statement(SgAsmStatement*);

        /** Erases statement from statement list.
         *
         *  If the specified statement is found in the list of statements then it is erased without being deleted. */
        void remove_statement(SgAsmStatement* statement);

        // [Robb P Matzke 2017-02-13]: I don't think this is used anywhere. It appends to the poorly named "dest" property.
        void append_dest(SgAsmStatement* instruction) ROSE_DEPRECATED("apparently not used anywhere");

        // FIXME[Robb P Matzke 2017-02-13]: This is a bad name--it removes only statements, not all AST children.
        /** Removes all statements.
         *
         *  Clears the statement list without deleting any of them. */
        void remove_children();

        /** Function entry basic block.
         *
         *  Returns the basic block that represents the function primary entry point. Returns null for a function
         *  that contains no instructions. */
        SgAsmBlock* get_entry_block() const;

        /** Whether a function returns. */
        enum MayReturn {
                RET_UNKNOWN,                    /**< It is unknown whether this function ever returns or not. */
                RET_NEVER,                      /**< This function is known to never return. */
                RET_SOMETIMES,                  /**< This function may return or not, depending on how it is called. */
                RET_ALWAYS                      /**< This function returns each time it is called. */
        };

        /** Reasons why an instruction might be considered the beginning of a function. These bit flags are also used by
         *  the instruction partitioner (Partitioner class) to determine what heuristics are used when partitioning
         *  instructions into functions. */
        enum FunctionReason {
                // NOTE: If you add more here, then fix Partitioner::parse_switches()
                //       Also fix SgAsmFunction::reason_key()
                FUNC_NONE        = 0x00000000,  /**< Used for initialization; not a bit flag. */
                FUNC_EXCEPTION_HANDLER
                                 = 0x00008000,  /**< Function for handling an exception. */
                FUNC_ENTRY_POINT = 0x00010000,  /**< An entry point specified in the file header. */
                FUNC_CALL_TARGET = 0x00020000,  /**< Target of a function call instruction sequence in the CFG. When used as
                                                 *   a partitioning heuristic, the partitioner will create new functions when
                                                 *   it discovers a call-like sequence while traversing the CFG. */
                FUNC_CALL_INSN   = 0x00040000,  /**< Target of call, possibly not in the CFG (see Partitioner::mark_call_insns).
                                                 *   When used as a partitioning heuristic, the partitioner will search all
                                                 *   available instructions for instances of call-like instructions and define
                                                 *   a function for each target. The function entry points added in this way
                                                 *   become initial nodes of the CFG which is used by some of
                                                 *   the other function discovery methods, including FUNC_CALL_TARGET. */
                FUNC_EH_FRAME    = 0x00080000,  /**< Address mentioned in the ELF .eh_frame section. */
                FUNC_SYMBOL      = 0x00100000,  /**< Address of a function symbol in a symbol table. */
                FUNC_PATTERN     = 0x00200000,  /**< Appears to be a function based on pattern of instructions. When used as
                                                 *   a partitioning heuristic, the partitioner will search through all available
                                                 *   instructions and create function entry points. The function entry points are
                                                 *   added to the CFG which is used by some of the other function discovery
                                                 *   methods. */
                FUNC_GRAPH       = 0x00400000,  /**< Implied by inter-basicblock branching. When used as a partitioning
                                                 *   heuristic, the partitioner creates a new function when it discovers, by
                                                 *   CFG traversal, that two different functions branch to a common basic block.
                                                 *   The block in common becomes a new function under this rule. */
                FUNC_USERDEF     = 0x00800000,  /**< User-defined algorithm. See Partitioner::addFunctionDetector(). */
                FUNC_PADDING     = 0x01000000,  /**< Created to represent NOP padding between other functions. When used as a
                                                 *   partitioning heuristic, the partitioner searches for padding after all
                                                 *   CFG-based analysis has completed.  Padding can consist of either NOP
                                                 *   instructions or zero bytes. The former requires that instructions at the
                                                 *   interfunction addresses have been disassembled. */
                FUNC_DISCONT     = 0x02000000,  /**< Blocks of function are not contiguous in memory. This is not a partitioner
                                                 *   heuristic, but rather only an indication of whether the function's basic
                                                 *   blocks occupy contiguous memory locations. */
                FUNC_INSNHEAD    = 0x04000000,  /**< Initial instructions not in any other function. (UNUSED?) */
                FUNC_IMPORT      = 0x08000000,  /**< Functions dynamically linked. For ELF containers, these are the entries
                                                 *   in the procedure lookup table (PLT). When used as a partitioning heuristic,
                                                 *   the partitioner will scan the PLT and define a function for each entry. */
                FUNC_LEFTOVERS   = 0x10000000,  /**< Generated function to hold blocks that are otherwise not part of
                                                 *   any function.  If this bit is turned off then the instruction
                                                 *   Partitioner will delete instructions that it couldn't assign to
                                                 *   a function. */
                FUNC_INTRABLOCK  = 0x20000000,  /**< Function contains basic blocks that were inserted by searching the
                                                 *   address space between the blocks discovered by analyzing the control flow.
                                                 *   Blocks added by this algorithm do not take control flow into account, and
                                                 *   therefore, the global control flow graph (CFG) may have edges into the
                                                 *   middle of such blocks, or such blocks may have edges into the middle of
                                                 *   other blocks, including inter-function edges.  Also, blocks added by this
                                                 *   method might not represent true instructions, but rather data that was
                                                 *   disassembled as instructions. */
                FUNC_THUNK       = 0x40000000,  /**< Function is a thunk.  Thunks are small pieces of code whose only purpose
                                                 *   is to branch to another function.  Depending on how the disassembler and
                                                 *   partitioner are run, a thunk can exist either as its own function or the
                                                 *   thunk's instructions will be subsumed by the called function.  The
                                                 *   partitioner only sets this flag for functions that are created due to
                                                 *   the thunk recognition pass; other functions that don't have this bit set
                                                 *   might also be thunks, and functions that have this bit set might not
                                                 *   be a thunk.  The Partitioner::is_thunk() method will return true for
                                                 *   functions whose content looks like a thunk. */
                FUNC_EXPORT      = 0x80000000,  /**< Exported function. These are the functions that appear in a PE export
                                                 *   table. */
                FUNC_DEFAULT     = 0xefff80ff,  /**< Default value for Partitioner class. */

                /*========= Miscellaneous Reasons ===========================================================================
                 * The first half of the range (1-127, inclusive) is used for algorithms defined by ROSE.  The second half is
                 * availalble for users to use as they see fit. */
                FUNC_MISCMASK    = 0x000000ff,  /**< Miscellaneous.  These are all the other special purpose function detectors
                                                 *   that are implemented in the Partitioner. Each one is identified by an
                                                 *   eight-bit integer stored in the low order bits.  Only one such reason can
                                                 *   be stored at a time.  These are not used to control which partitioning
                                                 *   heuristics to use, but rather to indicate which one (of possibly many)
                                                 *   that detected the function. */
                FUNC_INTERPADFUNC = 0x00000001  /**< Detected by Partitioner::FindInterPadFunctions, which looks for unassigned
                                                 *   space between two inter-function padding blocks and makes the first such
                                                 *   address the beginning of one of these functions. */
        };

        /** Multi-line description of function reason keys from unparser.
         *
         *  Returns a string that describes what the one-letter function reasons mean in the unparser output. */
        static std::string reason_key(const std::string &prefix="");
        
        /** Returns a very short string describing the reason mask. */
        std::string reason_str(bool pad) const;

        /** Class method that converts a reason bit vector to a human-friendly string.
         *
         *  The second argument is the bit vector of SgAsmFunction::FunctionReason bits. */
        static std::string reason_str(bool pad, unsigned reason);

        /** Selection functor for SgAsmFunction::get_extent(). */
        class NodeSelector {
        public:
                virtual ~NodeSelector() {}
                virtual bool operator()(SgNode*) = 0;
        };

        /** Returns information about the function addresses.
         *
         *  Every non-empty function has a minimum (inclusive) and maximum (exclusive) address which are returned by reference,
         *  but not all functions own all the bytes within that range of addresses. Therefore, the exact bytes are returned by
         *  adding them to the optional ExtentMap argument.  This function returns the number of nodes (instructions and static
         *  data items) in the function.  If the function contains no nodes then @p extents is not modified and the low and
         *  high addresses are both set to zero.
         *
         *  If an @p selector functor is provided, then only nodes for which it returns true are considered part of the
         *  function.  This can be used for such things as filtering out data blocks that are marked as padding.  For example:
         *
         *  @code
         *  class NotPadding: public SgAsmFunction::NodeSelector {
         *  public:
         *      virtual bool operator()(SgNode *node) {
         *          SgAsmStaticData *data = isSgAsmStaticData(node);
         *          SgAsmBlock *block = SageInterface::getEnclosingNode<SgAsmBlock>(data);
         *          return !data || !block || block->get_reason()!=SgAsmBlock::BLK_PADDING;
         *      }
         *  } notPadding;
         *
         *  AddressIntervalSet extents;
         *  function->get_extent(&extents, NULL, NULL, &notPadding);
         *  @endcode
         *
         *  Here's another example that calculates the extent of only the padding data, based on the negation of the filter in
         *  the previous example:
         *
         *  @code
         *  class OnlyPadding: public NotPadding {
         *  public:
         *      virtual bool operator()(SgNode *node) {
         *          return !NotPadding::operator()(node);
         *      }
         *  } onlyPadding;
         *
         *  AddressIntervalSet extents;
         *  function->get_extent(&extents, NULL, NULL, &onlyPadding);
         *  @endcode */
        size_t get_extent(AddressIntervalSet *emap=NULL, rose_addr_t *lo_addr=NULL, rose_addr_t *hi_addr=NULL,
                          NodeSelector *selector=NULL);

        /** Computes the SHA1 message digest for the bytes of a function.
         *
         *  Returns true if the SHA1 is available, false if the message digest cannot be computed because the prerequisite
         *  gcrypt functions are not available. The optional @p selector argument can be used to limit the digest to only
         *  certain nodes of the function; by default, all instructions and static data are accumulated. */
        bool get_sha1(uint8_t digest[20]/*out*/, NodeSelector *selector=NULL);

        /** Constants for the "function_kind" property. */
        enum function_kind_enum {
            e_unknown  = 0,
            e_standard = 1,
            e_library  = 2,
            e_imported = 3,
            e_thunk     = 4,
            e_last
        };

        // Computes the offset of the stack at the end of the call relative the the start of the call (in a perfect function
        // this would be zero, this is used to score properly formed functions).
        // [Robb P Matzke 2017-02-13]: deprecated
        int get_stackNutralityMetric() const ROSE_DEPRECATED("use get_stackDelta instead");
#endif // SgAsmFunction_OTHERS


#ifdef DOCUMENTATION
    };
#endif


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    DECLARE_LEAF_CLASS(AsmBlock);
    IS_SERIALIZABLE(AsmBlock);

#ifdef DOCUMENTATION
    /** Instruction basic block.
     *
     *  One entry point (first instruction) and one exit point (last instruction).  However, SgAsmBlock has also historically
     *  been used for other things, such as collections of functions. */
    class SgAsmBlock: public SgAsmStatement {
    public:
#endif
        
#if 0 // [Robb P Matzke 2017-02-13]: not used anymore? Not serialized.
        // [tps 05Apr07] needed for the control_flow_graph
        AsmBlock.setDataPrototype("rose_addr_t", "next_block_true_address", "= 0",
                                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#if 0 // [Robb P Matzke 2017-02-13]: not used anymore? Not serialized.
        AsmBlock.setDataPrototype("rose_addr_t", "next_block_false_address", "= 0",
                                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Identification.
         *
         *  Block unique identification number. Not used by ROSE.
         *
         * @{ */
        rose_addr_t get_id() const;
        void set_id(rose_addr_t);
        /** @} */
#else
        AsmBlock.setDataPrototype("rose_addr_t", "id", "= 0",
                                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
#endif

#ifdef DOCUMENTATION
        /** Property: Reasons this block was created.
         *
         *  This property holds a bit mask of @ref SgAsmBlock::Reason bits that indicate why this block was created.
         *
         * @{ */
        unsigned get_reason() const;
        void set_reason(unsigned);
        /** @} */
#else
        AsmBlock.setDataPrototype("unsigned", "reason", "= SgAsmBlock::BLK_NONE",
                                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        // statementList and successors should have been pointers to nodes that contain the list rather than being the lists
        // themselves because ROSETTA doesn't allow traversals on multiple list data members--we can traverse either one list or
        // the other, but not both.  It's too late to change how this part of the AST is structured because so much user code
        // already depends on it, therefore we can only traverse statementList and not successors. [Robb Matzke 2016-02-25]
        /** Property: Statements of which this block is composed.
         *
         *  This is the list of "statements" that belong to this block. Statements are usually instructions, but historical
         *  code may have used basic blocks with other children.
         *
         * @{ */
        const SgAsmStatementPtrList& get_statementList() const;
        void set_statementList(const SgAsmStatementPtrList&);
        /** @} */
#else
        AsmBlock.setDataPrototype("SgAsmStatementPtrList", "statementList", "",
                                  NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        // statementList and successors should have been pointers to nodes that contain the list rather than being the lists
        // themselves because ROSETTA doesn't allow traversals on multiple list data members--we can traverse either one list or
        // the other, but not both.  It's too late to change how this part of the AST is structured because so much user code
        // already depends on it, therefore we can only traverse statementList and not successors. [Robb Matzke 2016-02-25]
        /** Property: Control flow successors.
         *
         *  This property holds the list of addresses which are control flow successors of this block.  The @ref
         *  rose::BinaryAnalysis::Partitioner2 "Partitioner2" name space has a more useful definition of control flow graph
         *  that can reference indeterminate addresses and store data in the edges, and which is copiable.
         *
         * @{ */
        const SgAsmIntegerValuePtrList& get_successors() const;
        void set_successors(const SgAsmIntegerValuePtrList&);
        /** @} */
#else
        AsmBlock.setDataPrototype("SgAsmIntegerValuePtrList", "successors", "",
                                  NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Whether the successors list is complete.
         *
         *  This property is true if the "successors" property holds an incomplete list of successors. Since this
         *  representation of a control flow graph is unable to represent edges that point to indeterminate addresses (e.g.,
         *  computed branches), the "successors_complete" property can be used.
         *
         *  The @ref rose::BinaryAnalysis::Partitioner2 "Partitioner2" name space has a more useful definition of control flow
         *  graph that can reference indeterminate addresses and store data in the edges, and which is copiable.
         *
         * @{ */
        bool get_successors_complete() const;
        void set_successors_complete(bool);
        /** @} */
#else
        AsmBlock.setDataPrototype("bool", "successors_complete", "= false",
                                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Holds the immediate dominator block in the control flow graph.
         *
         *  The immediate dominator is the closest block to this one (by following reverse control flow edges) through which
         *  all control paths pass in order to get from the function entry block to this block.
         *
         *  The @ref rose::BinaryAnalysis::Partitioner2 "Partitioner2" name space has a more useful definition of control flow
         *  graph that can reference indeterminate addresses and store data in the edges, and which is copiable.
         *
         * @{ */
        SgAsmBlock* get_immediate_dominator() const;
        void set_immediate_dominator(SgAsmBlock*);
        /** @} */
#else
        AsmBlock.setDataPrototype("SgAsmBlock*", "immediate_dominator", "=NULL",
                                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Cached vertex for control flow graphs.
         *
         *  This property is used by the virtual control flow graph mechanism.
         *
         *  The @ref rose::BinaryAnalysis::Partitioner2 "Partitioner2" name space has a more useful definition of control flow
         *  graph that can reference indeterminate addresses and store data in the edges, and which is copiable.
         *
         * @{ */
        size_t get_cached_vertex() const;
        void set_cached_vertex(size_t);
        /** @} */
#else
        AsmBlock.setDataPrototype("size_t", "cached_vertex", "= (size_t)(-1)", // see BinaryAnalysis::ControlFlow
                                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Likelihood that this block represents real instructions.
         *
         *  This property holds the results of an analysis that determines how likely it is that the memory from which this
         *  basic block was disassembled represents actual instructions that would be executed when the specimen runs.
         *
         * @{ */
        double get_code_likelihood() const;
        void set_code_likelihood(double);
        /** @} */
#else
        AsmBlock.setDataPrototype("double", "code_likelihood", "= 0.0",
                                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Stack pointer at block exit w.r.t. stack pointer at function entry.
         *
         *  This is the difference between the stack pointer after the last instruction of this block executes and the stack
         *  pointer when this block's function was entered.  It stores the result of a stack delta analysis. If stack delta
         *  analysis hasn't run, or was unable to determine a constant delta, then the special value @ref
         *  SgAsmInstruction::INVALID_STACK_DELTA is stored.
         *
         * @{ */
        int64_t get_stackDeltaOut() const;
        void set_stackDeltaOut(int64_t);
        /** @} */
#else
        AsmBlock.setDataPrototype("int64_t", "stackDeltaOut", "= SgAsmInstruction::INVALID_STACK_DELTA",
                                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmBlock);
#if defined(SgAsmBlock_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmStatement);
            s & BOOST_SERIALIZATION_NVP(p_reason);
            s & BOOST_SERIALIZATION_NVP(p_statementList);
            s & BOOST_SERIALIZATION_NVP(p_successors);
            s & BOOST_SERIALIZATION_NVP(p_successors_complete);
            s & BOOST_SERIALIZATION_NVP(p_immediate_dominator);
            s & BOOST_SERIALIZATION_NVP(p_cached_vertex);
            s & BOOST_SERIALIZATION_NVP(p_code_likelihood);
            s & BOOST_SERIALIZATION_NVP(p_stackDeltaOut);
        }
#endif

    public:
        /** Reasons why a basic block might have been assigned to a function. */
        enum Reason {
            // Please update SgAsmBlock::reason_str() if you change this enum!
            BLK_NONE        = 0x00000000,           /**< No particular reason.  Mostly just for initialization. */
            BLK_ENTRY_POINT = 0x00010000,           /**< Block is an entry point for the function. */
            BLK_PADDING     = 0x00020000,           /**< Block is used for padding. */
            BLK_FRAGMENT    = 0x00080000,           /**< This block created because it seems to belong to the function although
                                                     *   CFG traversal did not find it. */
            BLK_CFGHEAD     = 0x00100000,           /**< Block serves as an explicit starting point for CFG analysis. */
            BLK_USERDEF     = 0x00200000,           /**< User says block belongs to the function. */
            BLK_LEFTOVERS   = 0x00400000,           /**< Block is being assigned to a FUNC_LEFTOVERS function because it could
                                                     *   not be assigned to any other function. */
            BLK_JUMPTABLE   = 0x00800000,           /**< Table of code addresses used by indirect branches. */
            BLK_GRAPH1      = 0x01000000,           /**< Block was added by the main CFG analysis. */
            BLK_GRAPH2      = 0x02000000,           /**< Block was added by a second pass of CFG analysis. */
            BLK_GRAPH3      = 0x04000000,           /**< Block was added by a third pass of CFG analysis. */
                
            BLK_DEFAULT     = BLK_NONE,             //NO_STRINGIFY

            // ========= Miscellaneous Reasons ===========================================================================
            // The first half of the range (1-127, inclusive) is used for algorithms defined by ROSE.  The second half is
            // availalble for users to use as they see fit.
            BLK_MISCMASK    = 0x000000ff,           /**< Miscellaneous reasons go here. We can store only one such reason at
                                                     *   a time. */
            BLK_FINDDATA    = 0x00000001,           /**< Added by Partitioner::FindData, which attaches unassigned parts of the
                                                     *   disassembly address space to the preceding function. */
            BLK_POSTFUNC    = 0x00000002            /**< Added by Partitioner::FindPostFunctionInsns, which adds unassigned
                                                     *   instructions to the immediately preceding function. */
        };

        /** Add the specified statement to the end of the statement list.
         *
         *  This is is usually used to add the next instruction to the end of a basic block. */
        void append_statement(SgAsmStatement*);

        /** Erase the specified statement.
         *
         *  If the specified statement exists in the "statementList" property then it is erased but not deleted. */
        void remove_statement(SgAsmStatement*);

        // FIXME[Robb P Matzke 2017-02-13]: wrong name -- erases only statements, not all children
        /** Removes all statements from the block.
         *
         *  This makes the block empty, and not having a unique starting virtual address. It does not erase all children, just
         *  the statement children.  None of the statements that are erased are deleted. */
        void remove_children();

        /** Fall-through virtual address.
         *
         *  A block's fall-through address is the virtual address that follows the last byte of the block's last instruction.
         *  The block must have instructions (e.g., it cannot be a strict data block). */
        rose_addr_t get_fallthrough_va();

        /** Returns the function that owns this block.
         *
         *  This is just a convenience wrapper around @ref SageInterface::getEnclosingNode. */
        SgAsmFunction *get_enclosing_function() const;

        /** Determins if a block contains instructions.
         *
         *  Returns true if the block has instructions, false otherwise. We look only at the immediate descendants of this
         *  block.  See also, @ref SageInterface::querySubTree in order to get the list of all instructions or to consider all
         *  descendants. */
        bool has_instructions() const;

        /** Determine if a block contains instructions.
         *
         *  Returns true if the block has instructions, false otherwise. We look only at the immediate descendants of this
         *  block.  See also, @ref SageInterface::querySubTree in order to get the list of all instructions or to consider all
         *  descendants. */
        bool is_basic_block() const { return has_instructions(); }

        /** Returns true if basic block appears to be a function call.
         *
         *  If the target address is known and is a single value then it is stored in the @p target_va argument, otherwise we
         *  store the maximum 64-bit address.  If the return address for the function call is known then it is stored in the @p
         *  return_va argument, otherwise @p return_va will contain the maximum 64-bit address. The return address is usually
         *  the fall-through address of the basic block.
         *
         * Note: Use this function in preference to SgAsmInstruction::isFunctionCallSlow() because the latter is intended to be
         * used by the Partitioner before an AST is created and might not be as accurate. */
        bool is_function_call(rose_addr_t &target_va/*out*/, rose_addr_t &return_va/*out*/);

        /** Multi-line string describing the letters used for basic block reasons.
         *
         *  The letters are returned by the padding version of @ref reason_str and appear in unparser output. */
        static std::string reason_key(const std::string &prefix="");

        /** Returns reason string for this block.
         *
         *  The reason string is a very short string describing the reason that the block was created. */
        std::string reason_str(bool pad) const;

        /** Converts a reason bit vector to a human-friendly string.
         *
         *  The second argument is the bit vector of @ref SgAsmBlock::Reason bits.  Some of the positions in the padded return
         *  value are used for more than one bit.  For instance, the first character can be "L" for leftovers, "N" for padding,
         *  "E" for entry point, or "-" for none of the above. */
        static std::string reason_str(bool pad, unsigned reason);
#endif // SgAsmBlock_OTHERS

#ifdef DOCUMENTATION
    };
#endif


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    DECLARE_LEAF_CLASS(AsmStaticData);
    IS_SERIALIZABLE(AsmStaticData);

#ifdef DOCUMENTATION
    /** Represents static data in an executable.
     *
     *  For now, we don't associate any type with the data because ROSE's data type infrastructure (source or binary) is not
     *  capable of representing the information we need: multiple interpretations of overlapping parts of memory (i.e., two or
     *  more types for the same bytes); arbitrary offsets and padding in structured types; size-specific integers and
     *  floating-point types; regions of unknown type; ease of improving type information by filling in more details as the
     *  type is discovered; etc. */
    class SgAsmStaticData: public SgAsmStatement {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Raw bytes.
         *
         *  These are the raw memory bytes of static data that appear in the binary specimen. Type information is painted onto
         *  these bytes.
         *
         * @{ */
        const SgUnsignedCharList& get_raw_bytes() const;
        void set_raw_bytes(const SgUnsignedCharList&);
        /** @} */
#else
        AsmStaticData.setDataPrototype("SgUnsignedCharList", "raw_bytes", "",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
#endif

        DECLARE_OTHERS(AsmStaticData);
#if defined(SgAsmStaticData_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmStatement);
            s & BOOST_SERIALIZATION_NVP(p_raw_bytes);
        }
#endif

    public:
        /** Property: Size of static data in bytes.
         *
         *  This returns the number of raw data bytes rather than the size of any data type painted onto those bytes. */
        size_t get_size() const { return p_raw_bytes.size(); }
#endif // SgAsmStaticData_OTHERS

#ifdef DOCUMENTATION
    };
#endif


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    DECLARE_LEAF_CLASS(AsmSynthesizedDataStructureDeclaration);
    IS_SERIALIZABLE(AsmSynthesizedDataStructureDeclaration);

#ifdef DOCUMENTATION
    /** Declaration-like nodes that encapsulate multiple instructions.
     *
     *  Binary ASTs have two sides: the container side that corresponds to the ELF/PE/etc. file formats, and the interpretation
     *  side that corresponds to instructions and data from multiple sources (specimen + dynamic libraries) organized into
     *  multiple SgAsmInterpretation where each interpretation makes a coherent binary entity such as the DOS part of a PE
     *  executable.  The declaration-like nodes that follow appear on the interpretation side of the AST.  We may add other
     *  declaration nodes to the container side of the AST at a later time.
     *
     *  These interpretation-side declaration-like nodes are used by the projects/BinaryDataStructureRecognition even if they
     *  aren't used internally by ROSE. */
    class SgAsmSynthesizedDataStructureDeclaration: public SgAsmSynthesizedDeclaration {
    public:
#endif

        DECLARE_OTHERS(AsmSynthesizedDataStructureDeclaration);
#if defined(SgAsmSynthesizedDataStructureDeclaration) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S & s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmStatement);
        }
#endif
     protected:
          SgAsmSynthesizedDeclarationPtrList p_declarationList;

    public:
        /** Appends another declaration. */
        void append_declaration(SgAsmSynthesizedDeclaration *declaration) {
            p_declarationList.push_back(declaration);
        }
#endif // SgAsmSynthesizedDataStructureDeclaration_OTHERS

#ifdef DOCUMENTATION
    };
#endif


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmSynthesizedFieldDeclaration);
    IS_SERIALIZABLE(AsmSynthesizedFieldDeclaration);

#ifdef DOCUMENTATION
    // FIXME[Robb P Matzke 2017-02-13]: what is this?
    class SgAsmSynthesizedFieldDeclaration: public SgAsmSynthesizedDeclaration {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Name.
         *
         * @{ */
        const std::string& get_name() const;
        void set_name(const std::string&);
        /** @} */
#else
        AsmSynthesizedFieldDeclaration.setDataPrototype("std::string","name","= \"\"",
                                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL,
                                                        NO_DELETE);
#endif

#ifdef DOCUMENTATION
        // FIXME[Robb P Matzke 2017-02-13]: Is this bytes, bits, or what?
        /** Property: Offset.
         *
         * @{ */
        uint64_t get_offset() const;
        void set_ofset(uint64_t);
        /** @} */
#else
        // Not clear if we want to store the offset explicitly
        AsmSynthesizedFieldDeclaration.setDataPrototype("uint64_t","offset","= 0",
                                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL,
                                                        NO_DELETE);
#endif

        DECLARE_OTHERS(AsmSynthesizedFieldDeclaration);
#if defined(SgAsmSynthesizedFieldDeclaration_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmSynthesizedDeclaration);
        }
#endif
#endif // SgAsmSynthesizedFieldDeclaration_OTHERS

#ifdef DOCUMENTATION
    };
#endif


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    NEW_NONTERMINAL_MACRO(AsmSynthesizedDeclaration,
                          AsmSynthesizedDataStructureDeclaration | AsmFunction | AsmSynthesizedFieldDeclaration,
                          "AsmSynthesizedDeclaration", "AsmSynthesizedDeclarationTag", false );
    AsmSynthesizedFieldDeclaration.setCppCondition("!defined(DOCUMENTATION)");
    IS_SERIALIZABLE(AsmSynthesizedDeclaration);

#ifdef DOCUMENTATION
    /** Base class for synthesized declarations.
     *
     *  A synthesized declaration is one created by ROSE which does not appear in the binary specimen. At one point we
     *  considered adding "synthesized" to all such intities, but later decided against it since most declarations are missing
     *  from binary specimens are are synthesized by ROSE, and would therefore lead to a lot of extra letters in many class
     *  names. */
    class SgAsmSynthesizedDeclaration: public SgAsmStatement {
    public:
#endif

        DECLARE_OTHERS(AsmSynthesizedDeclaration);
#if defined(SgAsmSynthesizedDeclaration_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmStatement);
        };
#endif
#endif // SgAsmSynthesizedDeclaration_OTHERS

#ifdef DOCUMENTATION
    };
#endif


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NEW_NONTERMINAL_MACRO(AsmStatement,
                          AsmSynthesizedDeclaration | AsmBlock | AsmInstruction | AsmStaticData,
                          "AsmStatement", "AsmStatementTag", false);
    AsmStatement.setCppCondition("!defined(DOCUMENTATION)");
    IS_SERIALIZABLE(AsmStatement);

#ifdef DOCUMENTATION
    class SgAsmStatement: public SgAsmNode {
    public:
#endif

#ifndef DOCUMENTATION
        AsmStatement.setDataPrototype("rose_addr_t", "address", "= 0",
                                      CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
        AsmStatement.setDataPrototype("std::string", "comment", "= \"\"",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
#endif

        DECLARE_OTHERS(AsmStatement);
#if defined(SgAsmStatement_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgNode);
            s & BOOST_SERIALIZATION_NVP(p_address);
            s & BOOST_SERIALIZATION_NVP(p_comment);
        }
#endif
#endif // SgAsmStatement_OTHERS

#ifdef DOCUMENTATION
    };
#endif





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

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NEW_TERMINAL_MACRO(AsmElfSectionTable, "AsmElfSectionTable", "AsmElfSectionTableTag");
    AsmElfSectionTable.setFunctionPrototype("HEADER_ELF_SECTION_TABLE", "../Grammar/BinaryInstruction.code");


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfSectionTableEntry);
    IS_SERIALIZABLE(AsmElfSectionTableEntry);

#ifdef DOCUMENTATION
    /** Represents one entry in an ELF section table. */
    class SgAsmElfSectionTableEntry: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: sh_name.
         *
         *  The sh_name property of an ELF section table. See official ELF specification.
         *
         * @{ */
        unsigned get_sh_name() const;
        void set_sh_name(unsigned);
        /** @} */
#else
        AsmElfSectionTableEntry.setDataPrototype("unsigned", "sh_name", "= 0",
                                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: sh_type.
         *
         *  The sh_type property of an ELF section table. See official ELF specification.
         *
         * @{ */
        SectionType get_sh_type() const;
        void set_sh_type(SectionType);
        /** @} */
#else
        AsmElfSectionTableEntry.setDataPrototype("SgAsmElfSectionTableEntry::SectionType", "sh_type", "= SHT_PROGBITS",
                                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: sh_link.
         *
         *  The sh_link property of an ELF section table. See official ELF specification.
         *
         * @{ */
        unsigned long get_sh_link() const;
        void set_sh_link(unsigned long);
        /** @} */
#else
        AsmElfSectionTableEntry.setDataPrototype("unsigned long", "sh_link", "= 0",
                                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: sh_info.
         *
         *  The sh_info property of an ELF section table. See official ELF specification.
         *
         * @{ */
        unsigned long get_sh_info() const;
        void set_sh_info(unsigned long);
        /** @} */
#else
        AsmElfSectionTableEntry.setDataPrototype("unsigned long", "sh_info", "= 0",
                                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: sh_flags.
         *
         *  The sh_flags property of an ELF section table.  See official ELF specification.
         *
         * @{ */
        uint64_t get_sh_flags() const;
        void set_sh_flags(uint64_t);
        /** @} */
#else
        AsmElfSectionTableEntry.setDataPrototype("uint64_t", "sh_flags", "= 0",
                                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: sh_addr.
         *
         *  The sh_addr property of an ELF section table. See official ELF specification.
         *
         * @{ */
        rose_addr_t get_sh_addr() const;
        void set_sh_addr(rose_addr_t);
        /** @} */
#else
        AsmElfSectionTableEntry.setDataPrototype("rose_addr_t", "sh_addr", "= 0",
                                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: sh_offset.
         *
         *  The sh_offset property of an ELF section table. See official ELF specification.
         *
         * @{ */
        rose_addr_t get_sh_offset() const;
        void set_sh_offset(rose_addr_t);
        /** @} */
#else
        AsmElfSectionTableEntry.setDataPrototype("rose_addr_t", "sh_offset", "= 0",
                                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: sh_size.
         *
         *  The sh_size property of an ELF section table. See official ELF specification.
         *
         * @{ */
        rose_addr_t get_sh_size() const;
        void set_sh_size(rose_addr_t);
        /** @} */
#else
        AsmElfSectionTableEntry.setDataPrototype("rose_addr_t", "sh_size", "= 0",
                                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: sh_addralign.
         *
         *  The sh_addralign property of an ELF section table. See official ELF specification.
         *
         * @{ */
        rose_addr_t get_sh_addralign() const;
        void set_sh_addralign(rose_addr_t);
        /** @} */
#else
        AsmElfSectionTableEntry.setDataPrototype("rose_addr_t", "sh_addralign", "= 0",
                                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: sh_entsize.
         *
         *  The sh_entsize property of an ELF section table. See official ELF specification.
         *
         * @{ */
        rose_addr_t get_sh_entsize() const;
        void set_sh_entsize(rose_addr_t);
        /** @} */
#else
        AsmElfSectionTableEntry.setDataPrototype("rose_addr_t", "sh_entsize", "= 0",
                                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Extra bytes not officially part of the table entry.
         *
         *  These are the extra bytes that aren't assigned any meaning by the specification.
         *
         * @{ */
        const SgUnsignedCharList& get_extra() const;
        void set_extra(const SgUnsignedCharLit&);
        /** @} */
#else
        AsmElfSectionTableEntry.setDataPrototype("SgUnsignedCharList", "extra", "",
                                                 NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL,
                                                 NO_DELETE);
#endif

        DECLARE_OTHERS(AsmElfSectionTableEntry);
#if defined(SgAsmElfSectionTableEntry_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_sh_name);
            s & BOOST_SERIALIZATION_NVP(p_sh_type);
            s & BOOST_SERIALIZATION_NVP(p_sh_link);
            s & BOOST_SERIALIZATION_NVP(p_sh_info);
            s & BOOST_SERIALIZATION_NVP(p_sh_flags);
            s & BOOST_SERIALIZATION_NVP(p_sh_addr);
            s & BOOST_SERIALIZATION_NVP(p_sh_offset);
            s & BOOST_SERIALIZATION_NVP(p_sh_size);
            s & BOOST_SERIALIZATION_NVP(p_sh_addralign);
            s & BOOST_SERIALIZATION_NVP(p_sh_entsize);
            s & BOOST_SERIALIZATION_NVP(p_extra);
        }
#endif

    public:
        /** Section types (host order). All other values are reserved. */
        enum SectionType {
            SHT_NULL        = 0,                  /**< Section header is inactive */
            SHT_PROGBITS    = 1,                  /**< Info defined by the program; format and meaning determined by prog */
            SHT_SYMTAB      = 2,                  /**< Complete symbol table */
            SHT_STRTAB      = 3,                  /**< String table */
            SHT_RELA        = 4,                  /**< Relocation entries with explicit addends (e.g., Elf32_Rela types) */
            SHT_HASH        = 5,                  /**< Symbol hash table (used by dynamic linking) */
            SHT_DYNAMIC     = 6,                  /**< Information for dynamic linking */
            SHT_NOTE        = 7,                  /**< Information that marks the file in some way */
            SHT_NOBITS      = 8,                  /**< Like SHT_PROGBITS but occupies no file space */
            SHT_REL         = 9,                  /**< Relocation entries without explicit addends (e.g., Elf32_Rel types) */
            SHT_SHLIB       = 10,                 /**< Reserved, unspecified semantics; Present only in non-conforming files */
            SHT_DYNSYM      = 11,                 /**< Minimal set of dynamic linking symbols */

            SHT_LOOS        = 0x60000000,         /**< OS specific semantics */
            SHT_GNU_verdef  = 0x6ffffffd,         /**< Symbol Version Definitions [gnu extension] .gnu.version_d */
            SHT_GNU_verneed = 0x6ffffffe,         /**< Symbol Version Requirements [gnu extension] .gnu.version_r */
            SHT_GNU_versym  = 0x6fffffff,         /**< Symbol Version Table [gnu extension] .gnu.version */
            SHT_HIOS        = 0x6fffffff,         /**< End OS specific semantics */       /*NO_STRINGIFY*/

            SHT_LOPROC      = 0x70000000,         /* Processor specific semantics */
            SHT_HIPROC      = 0x7fffffff,
            SHT_LOUSER      = 0x80000000,         /* Application specific semantics */
            SHT_HIUSER      = 0xffffffff
        };

        /** Section Flags (host order).  All other values are reserved. */
        enum SectionFlags {
            SHF_NULL=                  0,        /**< Invalid section flag (added for rose) */
            SHF_WRITE=           (1 << 0),       /**< Writable */
            SHF_ALLOC=           (1 << 1),       /**< Occupies memory during execution */
            SHF_EXECINSTR=       (1 << 2),       /**< Executable */
            SHF_MERGE=           (1 << 4),       /**< Might be merged */
            SHF_STRINGS=         (1 << 5),       /**< Contains nul-terminated strings */
            SHF_INFO_LINK=       (1 << 6),       /**< 'sh_info' contains SHT index */
            SHF_LINK_ORDER=      (1 << 7),       /**< Preserve order after combining */
            SHF_OS_NONCONFORMING=(1 << 8),       /**< Non-standard OS specific handling required */
            SHF_GROUP=           (1 << 9),       /**< Section is member of a group.  */
            SHF_TLS=             (1 << 10),      /**< Section hold thread-local data.  */
            SHF_MASKOS=          0x0ff00000,     /**< OS-specific.  */
            SHF_MASKPROC=        0xf0000000      /**< Processor-specific */
        };

        /** File format of an ELF Section header.
         *
         *  Byte order of members depends on e_ident value in file header. This code
         * comes directly from "Executable and Linkable Format (ELF)", Portable Formats Specification, Version 1.1, Tool
         * Interface Standards (TIS) and not from any header file. The 64-bit structure is gleaned from the Linux elf(5) man
         * page. */
#ifdef _MSC_VER
# pragma pack (1)
#endif
        struct Elf32SectionTableEntry_disk {
            uint32_t        sh_name;             /* 0x00 Section name; index into section header string table */
            uint32_t        sh_type;             /* 0x04 Section contents and semantics (see SectionType enum) */
            uint32_t        sh_flags;            /* 0x08 Bit flags */
            uint32_t        sh_addr;             /* 0x0c Desired mapped address */
            uint32_t        sh_offset;           /* 0x10 Section location in file unless sh_type==SHT_NOBITS */
            uint32_t        sh_size;             /* 0x14 Section size in bytes */
            uint32_t        sh_link;             /* 0x18 Section ID of another section; meaning depends on section type */
            uint32_t        sh_info;             /* 0x1c Extra info depending on section type */
            uint32_t        sh_addralign;        /* 0x20 Mapped alignment (0 and 1=>byte aligned); sh_addr must be aligned*/
            uint32_t        sh_entsize;          /* 0x24 If non-zero, size in bytes of each array member in the section */
        }                                        /* 0x28 */
// DQ (3/7/2013): Adding support to restrict visability to SWIG.
#ifndef SWIG
#ifndef _MSC_VER
        __attribute__((packed))
#endif
#endif
        ;

        struct Elf64SectionTableEntry_disk {
            uint32_t        sh_name;             /* 0x00 see Elf32SectionTableEntry_disk */
            uint32_t        sh_type;             /* 0x04 */
            uint64_t        sh_flags;            /* 0x08 */
            uint64_t        sh_addr;             /* 0x10 */
            uint64_t        sh_offset;           /* 0x18 */
            uint64_t        sh_size;             /* 0x20 */
            uint32_t        sh_link;             /* 0x28 */
            uint32_t        sh_info;             /* 0x2c */
            uint64_t        sh_addralign;        /* 0x30 */
            uint64_t        sh_entsize;          /* 0x38 */
        }                                        /* 0x40 */
// DQ (3/7/2013): Adding support to restrict visability to SWIG.
#ifndef SWIG
#ifndef _MSC_VER
        __attribute__((packed))
#endif
#endif
        ;
#ifdef _MSC_VER
# pragma pack ()
#endif

        /** Converts 32-bit disk representation to host representation. */
        SgAsmElfSectionTableEntry(ByteOrder::Endianness sex,
                                  const SgAsmElfSectionTableEntry::Elf32SectionTableEntry_disk *disk);

        /** Converts 64-bit disk representation to host representation. */
        SgAsmElfSectionTableEntry(ByteOrder::Endianness sex,
                                  const SgAsmElfSectionTableEntry::Elf64SectionTableEntry_disk *disk);

        /** Encode a section table entry into the disk structure.
         *
         * @{ */
        void *encode(ByteOrder::Endianness sex,
                     SgAsmElfSectionTableEntry::Elf32SectionTableEntry_disk *disk) const;
        void *encode(ByteOrder::Endianness sex,
                     SgAsmElfSectionTableEntry::Elf64SectionTableEntry_disk *disk) const;
        /** @} */

        /** Update this section table entry with newer information from the section. */
        void update_from_section(SgAsmElfSection*);

        /** Print some debugging info */
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const;

        // Use rose::stringify... function instead.
        static std::string to_string(SgAsmElfSectionTableEntry::SectionType);
        static std::string to_string(SgAsmElfSectionTableEntry::SectionFlags);

    private:
        void ctor(ByteOrder::Endianness, const SgAsmElfSectionTableEntry::Elf32SectionTableEntry_disk*);
        void ctor(ByteOrder::Endianness, const SgAsmElfSectionTableEntry::Elf64SectionTableEntry_disk*);
#endif // SgAsmElfSectionTableEntry_OTHERS



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

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NEW_TERMINAL_MACRO(AsmStringStorage, "AsmStringStorage", "AsmStringStorageTag");
    AsmStringStorage.setFunctionPrototype("HEADER_STRING_STORAGE", "../Grammar/BinaryInstruction.code");
    AsmStringStorage.setDataPrototype("SgAsmGenericStrtab*", "strtab", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmStringStorage.setDataPrototype("std::string", "string", "= \"\"",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
    AsmStringStorage.setDataPrototype("rose_addr_t", "offset", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmBasicString);
    IS_SERIALIZABLE(AsmBasicString);

#ifdef DOCUMENTATION
    /** String associated with a binary file.
     *
     *  Basic strings need not be stored in the binary specimen; they can be generated on the fly by the parser. See also, @ref
     *  SgAsmStoredString, which is present in the binary specimen. */
    class SgAsmBasicString: public SgAsmGenericString {
    public:
#endif

#ifndef DOCUMENTATION
        // Documented and declared below due to ROSETTA limitations
        AsmBasicString.setDataPrototype("std::string", "string", "= \"\"",
                                        NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmBasicString);
#if defined(SgAsmBasicString_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmGenericString);
            s & BOOST_SERIALIZATION_NVP(p_string);
        }
#endif

    public:
        /** Constructor that gives a value to the object.
         *
         * @{ */
        explicit SgAsmBasicString(const std::string &s)
            : p_string(s) {ctor();}
        explicit SgAsmBasicString(const char *s)
            : p_string(s) {ctor();}
        /** @} */

        // Overrides documented in base class
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const ROSE_OVERRIDE;
        virtual std::string get_string(bool escape=false) const ROSE_OVERRIDE;
        virtual void set_string(const std::string&) ROSE_OVERRIDE;
        virtual void set_string(rose_addr_t) ROSE_OVERRIDE;
        
    private:
        void ctor();
#endif // SgAsmBasicString_OTHERS

#ifdef DOCUMENTATION
    };
#endif


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    NEW_TERMINAL_MACRO(AsmStoredString, "AsmStoredString", "AsmStoredStringTag");
    AsmStoredString.setFunctionPrototype("HEADER_STORED_STRING", "../Grammar/BinaryInstruction.code");
    AsmStoredString.setDataPrototype("SgAsmStringStorage*", "storage", "= NULL",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NEW_NONTERMINAL_MACRO(AsmGenericString,
                          AsmBasicString | AsmStoredString,
                          "AsmGenericString", "AsmGenericStringTag", false);
    AsmGenericString.setCppCondition("!defined(DOCUMENTATION)");
    IS_SERIALIZABLE(AsmGenericString);

#ifdef DOCUMENTATION
    /** Base class for strings related to binary specimens. */
    class SgAsmGenericString: public SgAsmExecutableFileFormat {
    public:
#endif

        DECLARE_OTHERS(AsmGenericString);
#if defined(SgAsmGenericString_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
        }
#endif

    public:
        /** Constant for addresses of unallocated strings. */
        static const rose_addr_t unallocated = ~(rose_addr_t)0;

        /** Property: String value.
         *
         *  When retrieving the string, if @p escape is true then escape special charactes like the would be in C source code.
         *
         *  The base class implementation cannot be called and exists only due to ROSETTA limitations, otherwise we would have
         *  made them pure virtual.
         *
         * @{ */
        virtual std::string get_string(bool escape=false) const;
        virtual void set_string(const std::string &s);
        virtual void set_string(rose_addr_t);
        /** @} */

        virtual rose_addr_t get_offset() const {return unallocated;}

        virtual void dump(FILE*, const char *prefix, ssize_t idx) const;
#endif // SgAsmGenericString_OTHERS

#ifdef DOCUMENTATION
    };
#endif



        
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


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    AsmExecutableFileFormat.setCppCondition("!defined(DOCUMENTATION)");
    IS_SERIALIZABLE(AsmExecutableFileFormat);

    DECLARE_HEADERS(AsmExecutableFileFormat);
#if defined(SgAsmExecutableFileFormat) || defined(DOCUMENTATION)
#include <Sawyer/Message.h>
#endif // SgAsmExecutableFileFormat_HEADERS

#ifdef DOCUMENTATION
    /** Base class for many binary analysis nodes. */
    class SgAsmExecutableFileFormat: public SgAsmNode {
    public:
#endif

        DECLARE_OTHERS(AsmExecutableFileFormat);
#if defined(SgAsmExecutableFileFormat_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmNode);
        }
#endif

    public:
        /** Exception for reading past the end of something.
         *
         *  This object is thrown when an attempt is made to read past the end of a file, section, header, segment, etc. */
        class ShortRead {
        public:
            const SgAsmGenericSection *section;         /**< Section from which read occurred; null implies file-level write. */
            rose_addr_t offset;                         /**< Byte offset into section (or file). */
            rose_addr_t size;                           /**< Number of bytes of attempted read. */
            std::string mesg;                           /**< Optional message. */

            ShortRead(const class SgAsmGenericSection *section, size_t offset, size_t size)
                : section(section), offset(offset), size(size) {}
            ShortRead(const class SgAsmGenericSection *section, size_t offset, size_t size, const std::string &mesg)
                : section(section), offset(offset), size(size), mesg(mesg) {}
            ShortRead(const class SgAsmGenericSection *section, size_t offset, size_t size, const char *mesg)
                : section(section), offset(offset), size(size), mesg(mesg) {}
        };

        /** Exception for writing past the end of something.
         *
         *  This object is thrown when an attempt is made to write past the end of a file, section, header, segment, etc. */
        class ShortWrite {
        public:
            const SgAsmGenericSection *section;         /**< Section to which write occurred; null implies file-level write. */
            rose_addr_t          offset;                /**< Byte offset into section (or file). */
            rose_addr_t          size;                  /**< Number of bytes of attempted write. */
            std::string          mesg;                  /**< Optional message. */

            ShortWrite(const class SgAsmGenericSection *section, size_t offset, size_t size)
                : section(section), offset(offset), size(size) {}
            ShortWrite(const class SgAsmGenericSection *section, size_t offset, size_t size, const std::string &mesg)
                : section(section), offset(offset), size(size), mesg(mesg) {}
            ShortWrite(const class SgAsmGenericSection *section, size_t offset, size_t size, const char *mesg)
                : section(section), offset(offset), size(size), mesg(mesg) {}
        };

        /** Exception for container syntax errors.
         *
         *  This object is thrown when the file contains an error that prevents ROSE from parsing it. */
        class FormatError {
        public:
            std::string mesg;

            FormatError(const std::string &mesg) {this->mesg=mesg;}
            FormatError(const char *mesg) {this->mesg=mesg;}
        };

        /** Information about the file in the filesystem. */
        typedef struct stat fileDetails;

        /** Architecture family. */
        enum ExecFamily {
            FAMILY_UNSPECIFIED,                         /**< Unspecified family. */
            FAMILY_DOS,                                 /**< Microsoft DOS format. */
            FAMILY_ELF,                                 /**< Unix Executable and Linking Format. */
            FAMILY_LE,                                  /**< Microsft Linear Executable format. */
            FAMILY_LX,                                  /**< OS/2 LX (Windows 9x VxD device drivers, extension of LE). */
            FAMILY_NE,                                  /**< Microsoft New Executable Format. */
            FAMILY_PE                                   /**< Microsoft Portable Executable Format. */
        };

        /** Application binary interface. */
        enum ExecABI {
            ABI_UNSPECIFIED,                            /**< Not specified in file and could not be inferred */
            ABI_OTHER,                                  /**< Anything other than values below */
            ABI_86OPEN,                                 /**< 86Open Common IA32 */
            ABI_AIX,                                    /**< AIX */
            ABI_ARM,                                    /**< ARM architecture */
            ABI_FREEBSD,                                /**< FreeBSD */
            ABI_HPUX,                                   /**< HP/UX */
            ABI_IRIX,                                   /**< IRIX */
            ABI_HURD,                                   /**< GNU/Hurd */
            ABI_LINUX,                                  /**< GNU/Linux */
            ABI_MODESTO,                                /**< Novell Modesto */
            ABI_MONTEREY,                               /**< Monterey project */
            ABI_MSDOS,                                  /**< Microsoft DOS */
            ABI_NT,                                     /**< Windows NT */
            ABI_NETBSD,                                 /**< NetBSD */
            ABI_OS2,                                    /**< OS/2 */
            ABI_SOLARIS,                                /**< Sun Solaris */
            ABI_SYSV,                                   /**< SysV R4 */
            ABI_TRU64,                                  /**< Compaq TRU64 UNIX */
            ABI_WIN386                                  /**< Microsoft Windows */
        };

        /** Instruction sets organized by families */
        enum InsSetArchitecture {
            ISA_UNSPECIFIED             = 0x0000,       /**< File does not specify an architecture */
            ISA_OTHER                   = 0xffff,       /**< Architecture is something other than below */
            ISA_FAMILY_MASK             = 0xff00,       /**< Mask to get family part of ISA */

            ISA_IA32_Family             = 0x0100,       /**< x86 IA-32 family of architectures; Intel, AMD, VIA, ... */
            ISA_IA32_286                = 0x0101,       /**< 80286 */
            ISA_IA32_386                = 0x0102,       /**< MMU with paging */
            ISA_IA32_486                = 0x0103,       /**< risc-like pipelining, integrated FPU, on-chip cache */
            ISA_IA32_Pentium            = 0x0104,       /**< superscalar, 64-bit databus, MMX */
            ISA_IA32_Cyrix6x86          = 0x0105,       /**< register renaming, speculative execution */
            ISA_IA32_AMDK5              = 0x0106,       /**< micro-op translation */
            ISA_IA32_PentiumPro         = 0x0107,       /**< PAE, integrated L2 cache */
            ISA_IA32_PentiumII          = 0x0108,       /**< L3-cache, 3D Now, SSE */
            ISA_IA32_Athlon             = 0x0109,       /**< superscalar FPU, wide design */
            ISA_IA32_Pentium4           = 0x010a,       /**< deeply pipelined, high frequency, SSE2, hyper-threading */
            ISA_IA32_PentiumM           = 0x010b,       /**< low power */

            ISA_X8664_Family            = 0x0200,       /**< x86-64 family of architectures: Intel, AMD, VIA, ... */
            ISA_X8664_Athlon64          = 0x0201,       /**< on-die memory controller, 40-bit phys address space */
            ISA_X8664_Prescott          = 0x0202,       /**< deeply pipelined, high frequency, SSE3 */
            ISA_X8664_IntelCore         = 0x0203,       /**< low power, multi-core, lower clock frequency */
            ISA_X8664_AMDPhenom         = 0x0204,       /**< quad core, 128-bit FPUs, SSE4a, native mem ctrl, on-die L3 cache */

            ISA_SPARC_Family            = 0x0300,       /**< SPARC family of architectures; Sun Microsystems */
            ISA_SPARC_V7                = 0x0301,
            ISA_SPARC_V8                = 0x0302,
            ISA_SPARC_V8E               = 0x0303,
            ISA_SPARC_V9                = 0x0304,
            ISA_SPARC_V9JPS1            = 0x0305,
            ISA_SPARC_V9UA              = 0x0306,
            ISA_SPARC_V9JPS2            = 0x0307,

            ISA_M68K_Family             = 0x0400,       /**< Motorala m68k family */
            ISA_M68K_68000              = 0x0401,       /**< generation one: 16/32 internal; 8-, 16-, 32-bit interface */
            ISA_M68K_68EC000            = 0x0402,
            ISA_M68K_68HC000            = 0x0403,
            ISA_M68K_68008              = 0x0404,
            ISA_M68K_68010              = 0x0405,
            ISA_M68K_68012              = 0x0406,
            ISA_M68K_68020              = 0x0407,       /**< generation two: fully 32-bit */
            ISA_M68K_68EC020            = 0x0408,
            ISA_M68K_68030              = 0x0409,
            ISA_M68K_68EC030            = 0x040a,
            ISA_M68K_68040              = 0x040b,       /**< generation three: pipelined */
            ISA_M68K_68EC040            = 0x040c,
            ISA_M68K_68LC040            = 0x040d,
            ISA_M68K_68060              = 0x040e,       /**< generation four: superscalar */
            ISA_M68K_ColdFire           = 0x040f,       /**< other */
            ISA_M68K_DragonBall         = 0x0410,       /**< other */

            ISA_M88K_Family             = 0x0500,       /**< Motorola m88k family (not very popular) */
            ISA_M88K_88100              = 0x0501,       /**< 32-bit, integrated FPU mated with 88200 MMU and cache controller */
            ISA_M88K_88110              = 0x0502,       /**< single package of 88100+88200 */
            ISA_M88K_88110MP            = 0x0503,       /**< on-chip comm for use in multi-processor systems */
            ISA_M88K_88120              = 0x0504,       /**< superscalar (never actually released) */

            ISA_MIPS_Family             = 0x0600,       /**< 32/64-bit RISC; MIPS Technologies, Inc. */
            ISA_MIPS_MarkI              = 0x0601,       /**< R2000, R3000 */
            ISA_MIPS_MarkII             = 0x0602,       /**< R6000 */
            ISA_MIPS_MarkIII            = 0x0603,       /**< R4000 */
            ISA_MIPS_R2000              = 0x0604,       /**< 32-bit, Big or little endian */
            ISA_MIPS_R3000              = 0x0605,       /**< virtual identical: Pacempi's R3400, IDT's R3500, Toshiba R3900 */
            ISA_MIPS_R4000              = 0x0606,       /**< 64-bit; others in the series had larger caches and bug fixes */
            ISA_MIPS_R4200              = 0x0607,       /**< low-cost version of R4000 */
            ISA_MIPS_R4300              = 0x0608,       /**< low-cost version of R4000 with 32-bit external bus */
            ISA_MIPS_R4600              = 0x0609,       /**< "Orion" by Qauntum Effect Devices (QED); larger caches */
            ISA_MIPS_R4650              = 0x060a,       /**< by QED */
            ISA_MIPS_R4700              = 0x060b,       /**< "Orion" by QED */
            ISA_MIPS_R5000              = 0x060c,       /**< by QED */
            ISA_MIPS_RM7000             = 0x060d,       /**< by PMC-Sierra; 256kB L2 and optional L3 */
            ISA_MIPS_R8000              = 0x060e,       /**< superscalar, fairly rare */
            ISA_MIPS_R10000             = 0x060f,       /**< R8000 on a single chip; 32kB caches; out-of-order */
            ISA_MIPS_R12000             = 0x0610,       /**< R10000 + higher clock rates */
            ISA_MIPS_R14000             = 0x0611,       /**< R12000 + support for DDR SRAM; 200MHz front side bus */
            ISA_MIPS_R16000             = 0x0612,       /**< R14000 + increased freq, more L1, smaller die */
            ISA_MIPS_R16000A            = 0x0613,
            ISA_MIPS_16                 = 0x0614,       /**< Unknown. Windows PE architecture 0x266 "MIPS16" */
            ISA_MIPS_FPU                = 0x0615,       /**< Unknown. Windows PE architecture 0x366 "MIPS with FPU" */
            ISA_MIPS_16FPU              = 0x0616,       /**< Unknown. Windows PE architecture 0x466 "MIPS16 with FPU" */

            ISA_I860_Family             = 0x0700,       /**< Intel i860 family; 1989-mid 90's; RISC VLIW */
            ISA_I860_860XR              = 0x0701,       /**< (code named N10) 25-40MHz */
            ISA_I860_860XP              = 0x0702,       /**< (code named N11) larger caches; 40-50MHz; same IS as XR */

            ISA_IA64_Family             = 0x0800,       /**< Intel 64-bit architecture */
            ISA_IA64_Itanium            = 0x0801,       /**< First generation */
            ISA_IA64_Itanium2           = 0x0802,       /**< Second generation starting Nov 2007 */

            // See http://en.wikipedia.org/wiki/ARM_architecture
            ISA_ARM_Family              = 0x0900,       /**< Acorn RISC Machine, Advanced RISC Machines, ARM Limited */
            ISA_ARM_ARM1                = 0x0901,       /**< ARM evaluation system */
            ISA_ARM_ARM2                = 0x0902,       /**< ARM2, ARM250 cores */
            ISA_ARM_ARM3                = 0x0903,       /**< ARM2a core */
            ISA_ARM_ARM6                = 0x0904,       /**< ARM60, ARM600, ARM610 cores */
            ISA_ARM_ARM7                = 0x0905,       /**< ARM{700,710,710a,7100,7500,7500FE} cores */
            ISA_ARM_ARM7TDMI            = 0x0906,       /**< ARM{7TDMI,7TDMI-S,710T,720T,740T,7EJ-S} cores */
            ISA_ARM_StrongARM           = 0x0907,       /**< SA-110, SA-1110 cores */
            ISA_ARM_ARM8                = 0x0908,       /**< ARM810 core */
            ISA_ARM_ARM9TDMI            = 0x0909,       /**< ARM{9TDMI,920T,922T,940T} cores */
            ISA_ARM_ARM9E               = 0x090a,       /**< ARM{946E-S,966E-S,968E-S,926EJ-S,966HS} cores */
            ISA_ARM_ARM10E              = 0x090b,       /**< ARM{1020E,1022E,1026EJ-S} cores */
            ISA_ARM_XScale              = 0x090c,       /**< 80200, IOP310, IOP315, 80219, IOP321, IOP33x, IOP34x, PXA210,
                                                         *   PXA250, PXA255, PXA26x, PXA27x, PXA800(E)F, Monahans, PXA900,
                                                         *   IXC1100, IXP2400, IXP2800, IXP2850, IXP2325, IXP2350, IXP42x,
                                                         *   IXP460, IXP465 cores */
            ISA_ARM_ARM11               = 0x090d,       /**< ARMv{6,6T2,6KZ,6K} cores */
            ISA_ARM_Cortex              = 0x090e,       /**< Cortex-{A8,A9,A9 MPCore,R4(F),M3,M1} cores */
          
            // Others, not yet incorporated into this enum
            ISA_OTHER_Family            = 0xf000,

            ISA_ATT_WE_32100            = 0xf001,       /**< Sometimes simply "M32" */
            ISA_IBM_System_370          = 0xf002,
            ISA_HPPA                    = 0xf003,
            ISA_Fujitsu_VPP500          = 0xf004,
            ISA_Sun_v8plus              = 0xf005,
            ISA_PowerPC                 = 0xf006,
            ISA_PowerPC_64bit           = 0xf007,
            ISA_IBM_S390                = 0xf008,
            ISA_NEC_V800_series         = 0xf009,
            ISA_Fujitsu_FR20            = 0xf00a,
            ISA_TRW_RH_32               = 0xf00b,
            ISA_Motorola_RCE            = 0xf00c,
            ISA_Digital_Alpha_fake      = 0xf00e,
            ISA_Hitachi_SH              = 0xf00f,
            ISA_Siemens_Tricore         = 0xf010,
            ISA_Argonaut_RISC_Core      = 0xf011,
            ISA_Hitachi_H8_300          = 0xf012,
            ISA_Hitachi_H8_300H         = 0xf013,
            ISA_Hitachi_H8S             = 0xf014,
            ISA_Hitachi_H8_500          = 0xf015,
            ISA_Stanford_MIPS_X         = 0xf016,
            ISA_Motorola_M68HC12        = 0xf017,
            ISA_Fujitsu_MMA_Multimedia_Accelerator=0xf018,
            ISA_Siemens_PCP             = 0xf019,
            ISA_Sony_nCPU_embeeded_RISC = 0xf01a,
            ISA_Denso_NDR1_microprocessor=0xf01b,
            ISA_Motorola_Start_Core_processor=0xf01c,
            ISA_Toyota_ME16_processor   = 0xf01d,
            ISA_STMicroelectronic_ST100_processor=0xf01e,
            ISA_Advanced_Logic_Corp_Tinyj_emb_family=0xf01f,
            ISA_AMD_x86_64_architecture = 0xf020,
            ISA_Sony_DSP_Processor      = 0xf021,
            ISA_Siemens_FX66_microcontroller=0xf022,
            ISA_STMicroelectronics_ST9_plus_8_16_microcontroller=0xf023,
            ISA_STMicroelectronics_ST7_8bit_microcontroller=0xf024,
            ISA_Motorola_MC68HC16_microcontroller=0xf025,
            ISA_Motorola_MC68HC11_microcontroller=0xf026,
            ISA_Motorola_MC68HC08_microcontroller=0xf027,
            ISA_Motorola_MC68HC05_microcontroller=0xf028,
            ISA_Silicon_Graphics_SVx    = 0xf029,
            ISA_STMicroelectronics_ST19_8bit_microcontroller=0xf02a,
            ISA_Digital_VAX             = 0xf02b,
            ISA_Axis_Communications_32bit_embedded_processor=0xf02c,
            ISA_Infineon_Technologies_32bit_embedded_processor=0xf02d,
            ISA_Element_14_64bit_DSP_Processor=0xf02e,
            ISA_LSI_Logic_16bit_DSP_Processor=0xf02f,
            ISA_Donald_Knuths_educational_64bit_processor=0xf030,
            ISA_Harvard_University_machine_independent_object_files=0xf031,
            ISA_SiTera_Prism            = 0xf032,
            ISA_Atmel_AVR_8bit_microcontroller=0xf033,
            ISA_Fujitsu_FR30            = 0xf034,
            ISA_Mitsubishi_D10V         = 0xf035,
            ISA_Mitsubishi_D30V         = 0xf036,
            ISA_NEC_v850                = 0xf037,
            ISA_Mitsubishi_M32R         = 0xf038,
            ISA_Matsushita_MN10300      = 0xf039,
            ISA_Matsushita_MN10200      = 0xf03a,
            ISA_picoJava                = 0xf03b,
            ISA_OpenRISC_32bit_embedded_processor=0xf03c,
            ISA_ARC_Cores_Tangent_A5    = 0xf03d,
            ISA_Tensilica_Xtensa_Architecture=0xf03e,
            ISA_Digital_Alpha         = 0xf03f,
            ISA_Matsushita_AM33         = 0xf040,
            ISA_EFI_ByteCode            = 0xf041
        };

        /** General purpose of a binary executable file. */
        enum ExecPurpose {
            PURPOSE_UNSPECIFIED,                        /**< Purpose is not specified and could not be inferred */
            PURPOSE_OTHER,                              /**< A purpose other than any defined below */
            PURPOSE_EXECUTABLE,                         /**< Executable program */
            PURPOSE_LIBRARY,                            /**< Library (shared or relocatable) */
            PURPOSE_CORE_DUMP,                          /**< Post mortem image */
            PURPOSE_OS_SPECIFIC,                        /**< Some operating system specific purpose */
            PURPOSE_PROC_SPECIFIC                       /**< Some processor specific purpose */
        };

        /** Factory method that parses a binary file. */
        static SgAsmGenericFile *parseBinaryFormat(const char *name);

        /** Dump debugging information into a named text file. */
        static void unparseBinaryFormat(const std::string &name, SgAsmGenericFile*);

        /** Dump debugging information to specified stream. */
        static void unparseBinaryFormat(std::ostream&, SgAsmGenericFile*);

        /** Diagnostic stream. */
        static Sawyer::Message::Facility mlog;

        /** Initialize diagnostic streams.
         *
         *  This is called automatically by @ref rose::initializeLibrary. */
        static void initDiagnostics();

        /** Display binary data.
         *
         *  This function displays binary data in a fashion similar to the "hexdump -C" command in Unix: an address, numeric
         *  byte values, character byte values.  The format of the output is configurable through the HexdumpFormat
         *  argument. There are other versions that output containers of data.  The hexdump comes in three flavors: output to a
         *  C++ stream, output to a C FILE, and output to an std::string.  The FILE and string versions are implemented in
         *  terms of the stream version.
         *
         * @{ */
        static void hexdump(std::ostream&, rose_addr_t base_addr, const unsigned char *data, size_t data_sz,
                            const HexdumpFormat&);
        static void hexdump(std::ostream&, rose_addr_t base_addr, const std::string &prefix, const SgUnsignedCharList& data,
                            bool multiline=true);
        static void hexdump(std::ostream&, rose_addr_t base_addr, const std::string &prefix, const SgFileContentList& data,
                            bool multiline=true);

        // Same, but returning a string instead.
        static std::string hexdump(rose_addr_t base_addr, const unsigned char *data, size_t data_sz, const HexdumpFormat&);
        static std::string hexdump(rose_addr_t base_addr, const std::string &prefix, const SgUnsignedCharList& data,
                                   bool multiline=true);
        static std::string hexdump(rose_addr_t base_addr, const std::string &prefix, const SgFileContentList& data,
                                   bool multiline=true);

        // Same, but output to a FILE* instead.
        static void hexdump(FILE*, rose_addr_t base_addr, const unsigned char *data, size_t data_sz, const HexdumpFormat&);
        static void hexdump(FILE*, rose_addr_t base_addr, const std::string &prefix, const SgUnsignedCharList& data,
                            bool multiline=true);
        static void hexdump(FILE*, rose_addr_t base_addr, const std::string &prefix, const SgFileContentList& data,
                            bool multiline=true);
        /** @} */

        // These convert enums to strings. It is better to use the automatic enum stringification instead. They have names like
        // rose::stringifySgAsmExecutableFileFormatInsnSetArchitecture, etc. */
        static std::string isa_family_to_string(SgAsmExecutableFileFormat::InsSetArchitecture);
        static std::string isa_to_string(SgAsmExecutableFileFormat::InsSetArchitecture);
        static std::string to_string(SgAsmExecutableFileFormat::InsSetArchitecture);
        static std::string to_string(SgAsmExecutableFileFormat::ExecFamily);
        static std::string to_string(SgAsmExecutableFileFormat::ExecABI);
        static std::string to_string(SgAsmExecutableFileFormat::ExecPurpose);
#endif // SgAsmExecutableFileFormat_OTHERS

#ifdef DOCUMENTATION
    };
#endif


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    /*************************************************************************************************************************
     *                                         Root of all binary IR classes
     *************************************************************************************************************************/

    NEW_NONTERMINAL_MACRO(AsmNode,
                          AsmStatement | AsmExpression | AsmInterpretation | AsmOperandList | AsmType |
                          AsmExecutableFileFormat | AsmInterpretationList | AsmGenericFileList,
                          "AsmNode", "AsmNodeTag", false);
    AsmNode.setCppCondition("!defined(DOCUMENTATION)");
    IS_SERIALIZABLE(AsmNode);

#ifdef DOCUMENTATION
    /** Base class for all binary analysis IR nodes. */
    class SgAsmNode: public SgNode {
    public:
#endif

        DECLARE_OTHERS(AsmNode);
#if defined(SgAsmNode_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned version) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgNode);
        }
#endif
#endif // SgAsmNode_OTHERS

#ifdef DOCUMENTATION
    };
#endif

#ifndef DOCUMENTATION
}
#endif
