#include <featureTests.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

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
 * UPDATE: Instead of splitting class declarations into five separate places, we can now use the macros defined below to
 *        put everything but the node name into one place in this file. The node name still needs a line in the
 *        "astNodeList" file.  My goal is to eventually remove the src/ROSETTA/Grammar/BinaryInstruction.code file and
 *        the related docs/testDoxygen/xxx.docs files and to add documentation here for all property accessors.
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
//       class SgAsmArm64Instruction: public SgAsmInstruction {
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
    CLASS_WITHOUT_Sg.setPredeclarationString("Sg" #CLASS_WITHOUT_Sg "_HEADERS", \
                                             ROSE_AUTOMAKE_ABSOLUTE_PATH_TOP_SRCDIR + "/src/ROSETTA/src/binaryInstruction.C")
#endif

#ifdef DOCUMENTATION
#define DECLARE_OTHERS(CLASS_WITHOUT_Sg) /*void*/
#else
#define DECLARE_OTHERS(CLASS_WITHOUT_Sg) \
    CLASS_WITHOUT_Sg.setFunctionPrototype("Sg" #CLASS_WITHOUT_Sg "_OTHERS", \
                                          ROSE_AUTOMAKE_ABSOLUTE_PATH_TOP_SRCDIR + "/src/ROSETTA/src/binaryInstruction.C")
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

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**************************************************************************************************************************
     *                                  Instructions.
     * Base class (SgAsmInstruction) and various subclasses, one per architecture.
     **************************************************************************************************************************/

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef ROSE_ENABLE_ASM_A64
    DECLARE_LEAF_CLASS(AsmArm64Instruction);
    IS_SERIALIZABLE(AsmArm64Instruction);

    DECLARE_HEADERS(AsmArm64Instruction);
#if defined(SgAsmArm64Instruction_HEADERS) || defined(DOCUMENTATION)
    #include <InstructionEnumsArm64.h>
#endif // SgAsmArm64Instruction_HEADERS

#ifdef DOCUMENTATION
    /** Represents one ARM A64 machine instruction. */
    class SgAsmArm64Instruction: public SgAsmInstruction {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Instruction kind.
         *
         *  Returns an enum constant describing the AArch64 A64 instruction. These enum constants correspond roughly 1:1 with
         *  instruction mnemonics. Each architecture has its own set of enum constants. See also, getAnyKind.
         *
         * @{ */
        Rose::BinaryAnalysis::Arm64InstructionKind get_kind() const;
        void set_kind(Rose::BinaryAnalysis::Arm64InstructionKind);
        /** @} */
#else
        AsmArm64Instruction.setDataPrototype("Rose::BinaryAnalysis::Arm64InstructionKind", "kind",
                                             "= Rose::BinaryAnalysis::Arm64InstructionKind::ARM64_INS_INVALID",
                                             CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
#endif

#ifdef DOCUMENTATION
        /** Property: ARM A64 instruction condition.
         *
         *  @{ */
        Rose::BinaryAnalysis::Arm64InstructionCondition get_condition() const;
        void set_condition(Rose::BinaryAnalysis::Arm64InstructionCondition);
        /** @} */
#else
        AsmArm64Instruction.setDataPrototype("Rose::BinaryAnalysis::Arm64InstructionCondition", "condition",
                                             "= Rose::BinaryAnalysis::Arm64InstructionCondition::ARM64_CC_INVALID",
                                             CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
#endif

        DECLARE_OTHERS(AsmArm64Instruction);
#if defined(SgAsmArm64Instruction_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmInstruction);
            s & BOOST_SERIALIZATION_NVP(p_kind);
            s & BOOST_SERIALIZATION_NVP(p_condition);
        }
#endif

    public:
        // Overrides are documented in the base class
        virtual std::string description() const $ROSE_OVERRIDE;
        virtual bool terminatesBasicBlock() $ROSE_OVERRIDE;
        virtual Rose::BinaryAnalysis::AddressSet getSuccessors(bool &complete) $ROSE_OVERRIDE;
        virtual bool isUnknown() const $ROSE_OVERRIDE;
        virtual unsigned get_anyKind() const $ROSE_OVERRIDE;
#endif // SgAsmArm64Instruction_OTHERS
#ifdef DOCUMENTATION
    };
#endif
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmX86Instruction);
    IS_SERIALIZABLE(AsmX86Instruction);
    DECLARE_HEADERS(AsmX86Instruction);
#if defined(SgAsmX86Instruction_HEADERS) || defined(DOCUMENTATION)
    #include <InstructionEnumsX86.h>
    namespace Rose { namespace BinaryAnalysis { class RegisterDictionary; } }
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
        Rose::BinaryAnalysis::X86InstructionKind get_kind() const;
        void set_kind(Rose::BinaryAnalysis::X86InstructionSize);
        /** @} */
#else
        AsmX86Instruction.setDataPrototype("Rose::BinaryAnalysis::X86InstructionKind", "kind",
                                           "= Rose::BinaryAnalysis::x86_unknown_instruction",
                                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
#endif

#ifdef DOCUMENTATION
        /** Property: An enum constant describing the base size of an x86 instruction.
         *
         * @{ */
        Rose::BinaryAnalysis::X86InstructionSize get_baseSize() const;
        void set_baseSize(Rose::BinaryAnalysis::X86InstructionSize);
        /** @} */
#else
        AsmX86Instruction.setDataPrototype("Rose::BinaryAnalysis::X86InstructionSize", "baseSize",
                                           "= Rose::BinaryAnalysis::x86_insnsize_none",
                                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
#endif

#ifdef DOCUMENTATION
        /** Property: An enum describing the x86 instruction operand size.
         *
         * @{ */
        Rose::BinaryAnalysis::X86InstructionSize get_operandSize() const;
        void set_operandSize(Rose::BinaryAnalysis::X86InstructionSize);
        /** @} */
#else
        AsmX86Instruction.setDataPrototype("Rose::BinaryAnalysis::X86InstructionSize", "operandSize",
                                           "= Rose::BinaryAnalysis::x86_insnsize_none",
                                           CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
#endif

#ifdef DOCUMENTATION
        /** Property: An enum describing the x86 address size.
         *
         *  @{ */
        Rose::BinaryAnalysis::X86InstructionSize get_addressSize() const;
        void set_addressSize(Rose::BinaryAnalysis::X86InstructionSize);
        /** @} */
#else
        AsmX86Instruction.setDataPrototype("Rose::BinaryAnalysis::X86InstructionSize", "addressSize",
                                           "= Rose::BinaryAnalysis::x86_insnsize_none",
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
        Rose::BinaryAnalysis::X86RepeatPrefix get_repeatPrefix() const;
        void set_repeatPrefix(Rose::BinaryAnalysis::X86RepeatPrefix);
        /** @} */
#else
        AsmX86Instruction.setDataPrototype("Rose::BinaryAnalysis::X86RepeatPrefix", "repeatPrefix",
                                           "= Rose::BinaryAnalysis::x86_repeat_none",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
#endif

#ifdef DOCUMENTATION
        /** Property: An enum constant describing branch prediction.
         *
         * @{ */
        Rose::BinaryAnalysis::X86BranchPrediction get_branchPrediction() const;
        void set_branchPrediction(Rose::BinaryAnalysis::X86BranchPrediction);
        /** @} */
#else
        AsmX86Instruction.setDataPrototype("Rose::BinaryAnalysis::X86BranchPrediction", "branchPrediction",
                                           "= Rose::BinaryAnalysis::x86_branch_prediction_none",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
#endif

#ifdef DOCUMENTATION
        /** Property: The segment override register.
         *
         *  Returns an enum describing the segment register override. Note that this is not a register descriptor, but an enum
         *  constant.
         *
         *  @{ */
        Rose::BinaryAnalysis::X86SegmentRegister get_segmentOverride() const;
        void set_segmentOverride(Rose::BinaryAnalysis::X86SegmentRegister);
        /** @} */
#else
        AsmX86Instruction.setDataPrototype("Rose::BinaryAnalysis::X86SegmentRegister", "segmentOverride",
                                           "= Rose::BinaryAnalysis::x86_segreg_none",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
#endif


        DECLARE_OTHERS(AsmX86Instruction);
#if defined(SgAsmX86Instruction_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
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
        static Rose::BinaryAnalysis::X86InstructionSize instructionSizeForWidth(size_t);

        /** Converts a size enum constant to a size.
         *
         *  Given a size enum constant, return the number of bits that enum represents. */
        static size_t widthForInstructionSize(Rose::BinaryAnalysis::X86InstructionSize);

        /** Return the register dictionary for an x86 architecture.
         *
         *  Given an instruction size enum constant return the register dictionary that describes the x86 architecture with
         *  the specified word size.  See also, @ref registersForWidth. */
        static const Rose::BinaryAnalysis::RegisterDictionary* registersForInstructionSize(Rose::BinaryAnalysis::X86InstructionSize);

        /** Return the register dictionary for an x86 architecture.
         *
         *  Given an instruction size of 16, 32, or 64 return the register dictionary that describes the x86 architecture with
         *  the specified word size.  See also, @ref registersForInstructionSize. */
        static const Rose::BinaryAnalysis::RegisterDictionary* registersForWidth(size_t);

        // Overrides are documented in the base class
        virtual bool terminatesBasicBlock() $ROSE_OVERRIDE;
        virtual bool isFunctionCallFast(const std::vector<SgAsmInstruction*>&,
                                        rose_addr_t *target, rose_addr_t *ret) $ROSE_OVERRIDE;
        virtual bool isFunctionCallSlow(const std::vector<SgAsmInstruction*>&,
                                        rose_addr_t *target, rose_addr_t *ret) $ROSE_OVERRIDE;
        virtual bool isFunctionReturnFast(const std::vector<SgAsmInstruction*>&) $ROSE_OVERRIDE;
        virtual bool isFunctionReturnSlow(const std::vector<SgAsmInstruction*>&) $ROSE_OVERRIDE;
        virtual bool getBranchTarget(rose_addr_t *target/*out*/) $ROSE_OVERRIDE;
        virtual Rose::BinaryAnalysis::AddressSet getSuccessors(bool &complete) $ROSE_OVERRIDE;
        virtual Rose::BinaryAnalysis::AddressSet getSuccessors(const std::vector<SgAsmInstruction*>&,
                                                               bool &complete,
                                                               const Rose::BinaryAnalysis::MemoryMap::Ptr &initial_memory =
                                                               Rose::BinaryAnalysis::MemoryMap::Ptr()) $ROSE_OVERRIDE;
        virtual bool isUnknown() const $ROSE_OVERRIDE;
        virtual unsigned get_anyKind() const $ROSE_OVERRIDE;
#endif // SgAsmX86Instruction_OTHERS
#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmPowerpcInstruction);
    IS_SERIALIZABLE(AsmPowerpcInstruction);
    DECLARE_HEADERS(AsmPowerpcInstruction);
#if defined(SgAsmPowerpcInstruction_HEADERS) || defined(DOCUMENTATION)
    #include <InstructionEnumsPowerpc.h>
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
        Rose::BinaryAnalysis::PowerpcInstructionKind get_kind() const;
        void set_kind(Rose::BinaryAnalysis::PowerpcInstructionKind);
        /** @} */
#else
        AsmPowerpcInstruction.setDataPrototype("Rose::BinaryAnalysis::PowerpcInstructionKind", "kind",
                                               "= Rose::BinaryAnalysis::powerpc_unknown_instruction",
                                               CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE,
                                               COPY_DATA);
#endif

        DECLARE_OTHERS(AsmPowerpcInstruction);
#if defined(SgAsmPowerpcInstruction_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmInstruction);
            s & BOOST_SERIALIZATION_NVP(p_kind);
        }
#endif

    public:
        /** Description of conditional branch BO constant.
         *
         *  Given a BO constant from a PowerPC conditional branch instruction, convert it to a description of the condition
         *  that causes the branch to be taken. "BO" is the term used in the PowerPC documentation to denote the first argument
         *  of the conditional instruction; it is a 5-bit integer constant. */
        std::string conditionalBranchDescription() const;

        /** Return the register dictionary for a PowerPC architecture.
         *
         *  Given an instruction size of 32 or 64 return the register dictionary that describes the PowerPC architecture with
         *  the specified word size. */
        static const Rose::BinaryAnalysis::RegisterDictionary* registersForWidth(size_t);

        // Overrides are documented in the base class
        virtual std::string description() const $ROSE_OVERRIDE;
        virtual bool terminatesBasicBlock() $ROSE_OVERRIDE;
        virtual Rose::BinaryAnalysis::AddressSet getSuccessors(bool &complete) $ROSE_OVERRIDE;
        virtual bool isUnknown() const $ROSE_OVERRIDE;
        virtual unsigned get_anyKind() const $ROSE_OVERRIDE;
        virtual bool isFunctionCallFast(const std::vector<SgAsmInstruction*>&,
                                        rose_addr_t *target, rose_addr_t *retva) $ROSE_OVERRIDE;
        virtual bool isFunctionCallSlow(const std::vector<SgAsmInstruction*>&,
                                        rose_addr_t *target, rose_addr_t *retva) $ROSE_OVERRIDE;
        virtual bool isFunctionReturnFast(const std::vector<SgAsmInstruction*>&) $ROSE_OVERRIDE;
        virtual bool isFunctionReturnSlow(const std::vector<SgAsmInstruction*>&) $ROSE_OVERRIDE;
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
        Rose::BinaryAnalysis::MipsInstructionKind get_kind() const;
        void set_kind(Rose::BinaryAnalysis::MipsInstructionKind);
        /** @} */
#else
        AsmMipsInstruction.setDataPrototype("Rose::BinaryAnalysis::MipsInstructionKind", "kind",
                                            "= Rose::BinaryAnalysis::mips_unknown_instruction",
                                            CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE,
                                            COPY_DATA);
#endif

        DECLARE_OTHERS(AsmMipsInstruction);
#if defined(SgAsmMipsInstruction_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmInstruction);
            s & BOOST_SERIALIZATION_NVP(p_kind);
        }
#endif

    public:
        // Overrides are documented in the base class
        virtual std::string description() const $ROSE_OVERRIDE;
        virtual bool terminatesBasicBlock() $ROSE_OVERRIDE;
        virtual bool isFunctionCallFast(const std::vector<SgAsmInstruction*> &insns,
                                        rose_addr_t *target/*out*/, rose_addr_t *ret/*out*/) $ROSE_OVERRIDE;
        virtual bool isFunctionCallSlow(const std::vector<SgAsmInstruction*>&,
                                        rose_addr_t *target, rose_addr_t *ret) $ROSE_OVERRIDE;
        virtual bool isFunctionReturnFast(const std::vector<SgAsmInstruction*> &insns) $ROSE_OVERRIDE;
        virtual bool isFunctionReturnSlow(const std::vector<SgAsmInstruction*> &insns) $ROSE_OVERRIDE;
        virtual Rose::BinaryAnalysis::AddressSet getSuccessors(bool &complete) $ROSE_OVERRIDE;
        virtual bool isUnknown() const $ROSE_OVERRIDE;
        virtual bool getBranchTarget(rose_addr_t *target) $ROSE_OVERRIDE;
        virtual unsigned get_anyKind() const $ROSE_OVERRIDE;
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
         *  Returns an enum constant describing the Motorola m68k instruction. These enum constants correspond roughly 1:1 with
         *  instruction mnemonics. Each architecture has its own set of enum constants. See also, getAnyKind.
         *
         * @{ */
        Rose::BinaryAnalysis::M68kInstructionKind get_kind() const;
        void set_kind(Rose::BinaryAnalysis::M68kInstructionKind);
        /** @} */
#else
        AsmM68kInstruction.setDataPrototype("Rose::BinaryAnalysis::M68kInstructionKind", "kind",
                                            " = Rose::BinaryAnalysis::m68k_unknown_instruction",
                                            CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE,
                                            COPY_DATA);
#endif

        DECLARE_OTHERS(AsmM68kInstruction);
#if defined(SgAsmM68kInstruction_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmInstruction);
            s & BOOST_SERIALIZATION_NVP(p_kind);
        }
#endif

    public:
        // Overrides are documented in the base class
        virtual std::string description() const $ROSE_OVERRIDE;
        virtual bool terminatesBasicBlock() $ROSE_OVERRIDE;
        virtual bool isFunctionCallFast(const std::vector<SgAsmInstruction*> &insns,
                                        rose_addr_t *target/*out*/, rose_addr_t *ret/*out*/) $ROSE_OVERRIDE;
        virtual bool isFunctionCallSlow(const std::vector<SgAsmInstruction*>&,
                                        rose_addr_t *target, rose_addr_t *ret) $ROSE_OVERRIDE;
        virtual bool isFunctionReturnFast(const std::vector<SgAsmInstruction*> &insns) $ROSE_OVERRIDE;
        virtual bool isFunctionReturnSlow(const std::vector<SgAsmInstruction*> &insns) $ROSE_OVERRIDE;
        virtual bool getBranchTarget(rose_addr_t *target) $ROSE_OVERRIDE;
        virtual Rose::BinaryAnalysis::AddressSet getSuccessors(bool &complete) $ROSE_OVERRIDE;
        virtual Rose::BinaryAnalysis::AddressSet getSuccessors(const std::vector<SgAsmInstruction*>&,
                                                               bool &complete,
                                                               const Rose::BinaryAnalysis::MemoryMap::Ptr &initial_memory =
                                                               Rose::BinaryAnalysis::MemoryMap::Ptr()) $ROSE_OVERRIDE;
        virtual bool isUnknown() const $ROSE_OVERRIDE;
        virtual unsigned get_anyKind() const $ROSE_OVERRIDE;
#endif // SgAsmM68kInstruction_OTHERS
#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NEW_NONTERMINAL_MACRO(AsmInstruction,
                          AsmX86Instruction
#ifdef ROSE_ENABLE_ASM_A64
                          | AsmArm64Instruction
#endif
                          | AsmPowerpcInstruction | AsmMipsInstruction |
                          AsmM68kInstruction,
                          "AsmInstruction", "AsmInstructionTag", true);
    AsmInstruction.setCppCondition("!defined(DOCUMENTATION)");
    IS_SERIALIZABLE(AsmInstruction);

    DECLARE_HEADERS(AsmInstruction);
#if defined(SgAsmInstruction_HEADERS) || defined(DOCUMENTATION)
    #include <MemoryMap.h>
#endif // SgAsmInstruction_HEADERS

#ifdef DOCUMENTATION
    /** Base class for machine instructions.
     *
     *  @li Each instruction is represented by one or more instances of SgAsmInstruction.
     *
     *  @li An instruction obtained from a full AST will have a parent pointer. The instruction's first ancestor of type @ref
     *  SgAsmBlock is the basic block in which the instruction appears, and its first ancestor of type @ref SgAsmFunction is
     *  the function in which the instruction appears. There may be intervening AST nodes having other types.
     *
     *  @li An instruction obtained from a @ref Rose::BinaryAnalysis::Partitioner2::Partitioner "Partitioner" will not have a
     *  parent pointer. You can find ownership information using the Partitioner API.
     *
     *  @li An instruction's bytes will always be contiguous in the virtual address space from which the instruction was
     *  decoded, but might not be contiguous in the file (if any) where the instruction was stored. In fact, there's no
     *  guarantee that the instruction even exists entirely within one file.
     *
     *  @li Two distinct instructions (with different encodings) can start at the same virtual address if the specimen is
     *  self-modifying. Most ROSE analysis assumes that specimens are not self-modifying and uses the instruction's starting
     *  virtual address to uniquely identify the instruction.
     *
     *  @li Two distinct instructions (with different encodings) can occupy overlapping bytes in the virtual address space, and
     *  are guaranteed to have different starting addresses unless the specimen is self-modifying. */
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
            if (version >= 1)
                s & BOOST_SERIALIZATION_NVP(semanticFailure_);
        }
#endif

    private:
        struct SemanticFailure {
            size_t n;
            SemanticFailure(): n(0) {}

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
        private:
            friend class boost::serialization::access;
            template<class S>
            void serialize(S &s, const unsigned /*version*/) {
                s & BOOST_SERIALIZATION_NVP(n);
            }
#endif
        };
        SemanticFailure semanticFailure_;

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

        // FIXME[Robb P Matzke 2017-02-13]: unused?
        void appendSources( SgAsmInstruction* instruction );

        /** Number of operands. */
        size_t nOperands() const;

        /** Nth operand.
         *
         *  If the operand index is out of range, then null is returned. */
        SgAsmExpression* operand(size_t) const;

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
         *  This is mostly a wrapper around the @ref Rose::BinaryAnalysis::NoOperation "NoOperation" analysis. The @p
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
         *  This is mostly a wrapper around the @ref Rose::BinaryAnalysis::NoOperation "NoOperation" analysis. The @p
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
        virtual Rose::BinaryAnalysis::AddressSet getSuccessors(bool &complete); /*subclasses must redefine*/

        /** Control flow successors for a basic block.
         *
         *  The @p basicBlock argument is a vector of instructions that is assumed to be a basic block that is entered only at
         *  the first instruction and exits only at the last instruction.  A memory map can supply initial values for the
         *  analysis' memory state.  The return value is a set of control flow successor virtual addresses, and the @p complete
         *  argument return value indicates whether the returned set is known to be complete (aside from interrupts, faults,
         *  etc).  The base class implementation just calls the single-instruction version, so architecture-specific subclasses
         *  might want to override this to do something more sophisticated. */
        virtual Rose::BinaryAnalysis::AddressSet getSuccessors(const std::vector<SgAsmInstruction*> &basicBlock,
                                                               bool &complete,
                                                               const Rose::BinaryAnalysis::MemoryMap::Ptr &initial_memory =
                                                               Rose::BinaryAnalysis::MemoryMap::Ptr());

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

        /** Converts the instruction to a string.
         *
         *  The return value is an address, colon, mnemonic, and arguments. Only one space is used between the parts. */
        virtual std::string toString() const;

        /** Explicit constants.
         *
         *  Return the set of integer constants that appear explicitly in the instruction's operands. These are called
         *  "immediates" for some architectures such as X86. */
        virtual std::set<rose_addr_t> explicitConstants() const;

        /** Property: Whether instruction semantics failed at this location.
         *
         *  This property is incremented by various analyses that evaluate instructions semantically when semantics fails
         *  in a way that is not recoverable.  Some analyses can work around failed semantics by operating in a degraded
         *  mode, and it is up to the analysis whether to increment this property.
         *
         *  Thread safety: This method is thread safe.
         *
         * @{ */
        size_t semanticFailure() const;
        void semanticFailure(size_t);
        void incrementSemanticFailure();
        /** @} */

#endif // SgAsmInstruction_OTHERS

#ifdef DOCUMENTATION
    };
#endif



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
        void serialize(S &s, const unsigned /*version*/) {
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
        void serialize(S &s, const unsigned /*version*/) {
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
        void serialize(S &s, const unsigned /*version*/) {
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
        void serialize(S &s, const unsigned /*version*/) {
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
        void serialize(S &s, const unsigned /*version*/) {
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
        void serialize(S &s, const unsigned /*version*/) {
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
        void serialize(S &s, const unsigned /*version*/) {
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
        void serialize(S &s, const unsigned /*version*/) {
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
        void serialize(S &s, const unsigned /*version*/) {
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
        void serialize(S &s, const unsigned /*version*/) {
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
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmBinaryExpression);
        }
#endif
#endif // SgAsmBinaryLsl_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmBinaryMsl);
    IS_SERIALIZABLE(AsmBinaryMsl);

#ifdef DOCUMENTATION
    /** Expression that performs a logical left shift operation filling low-order bits with one.
     *
     *  This is identical to the Lsl operation except instead of low-order bits being cleared they are set. */
    class SgAsmBinaryMsl: public SgAsmBinaryExpression {
    public:
#endif

        DECLARE_OTHERS(AsmBinaryMsl);
#if defined(SgAsmBinaryMsl_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmBinaryExpression);
        }
#endif
#endif // SgAsmBinaryMsl_OTHERS

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
        void serialize(S &s, const unsigned /*version*/) {
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
        void serialize(S &s, const unsigned /*version*/) {
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
        void serialize(S &s, const unsigned /*version*/) {
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
                          AsmBinaryRor               | AsmBinaryMsl,
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
        void serialize(S &s, const unsigned /*version*/) {
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
        void serialize(S &s, const unsigned /*version*/) {
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
        void serialize(S &s, const unsigned /*version*/) {
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
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmUnaryExpression);
        }
#endif
#endif // SgAsmUnaryRrx_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmUnaryTruncate);
    IS_SERIALIZABLE(AsmUnaryTruncate);

#ifdef DOCUMENTATION
    /** Expression representing truncation.
     *
     *  The amount of truncation is based on the sizes of the types for the operand and the result. There is no second argument
     *  that says how large the result should be since this would be redundant and possibly inconsistent with the type for the
     *  resulting expression. */
    class SgAsmUnaryTruncation: public SgAsmUnaryExpression {
    public:
#endif

        DECLARE_OTHERS(AsmUnaryTruncate);
#if defined(SgAsmUnaryTruncate_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmUnaryExpression);
        }
#endif
#endif // SgAsmUnaryTruncate_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmUnarySignedExtend);
    IS_SERIALIZABLE(AsmUnarySignedExtend);

#ifdef DOCUMENTATION
    /** Expression representing sign extending.
     *
     *  The size of the result is based on the sizes of the types for the operand and the result. There is no second argument
     *  that says how large the result should be since this would be redundant and possibly inconsistent with the type for the
     *  resulting expression. */
    class SgAsmUnarySignedExtend: public SgAsmUnaryExpression {
    public:
#endif

        DECLARE_OTHERS(AsmUnarySignedExtend);
#if defined(SgAsmUnarySignedExtend_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmUnaryExpression);
        }
#endif
#endif // SgAsmUnarySignedExtend_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmUnaryUnsignedExtend);
    IS_SERIALIZABLE(AsmUnaryUnsignedExtend);

#ifdef DOCUMENTATION
    /** Expression representing unsigned extending.
     *
     *  The size of the result is based on the sizes of the types for the operand and the result. There is no second argument
     *  that says how large the result should be since this would be redundant and possibly inconsistent with the type for the
     *  resulting expression. */
    class SgAsmUnaryUnsignedExtend: public SgAsmUnaryExpression {
    public:
#endif

        DECLARE_OTHERS(AsmUnaryUnsignedExtend);
#if defined(SgAsmUnaryUnsignedExtend_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmUnaryExpression);
        }
#endif
#endif // SgAsmUnaryUnsignedExtend_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef ROSE_ENABLE_ASM_A64
    DECLARE_LEAF_CLASS(AsmArm64AtOperand);
    IS_SERIALIZABLE(AsmArm64AtOperand);

#ifdef DOCUMENTATION
    /** Operand for an ARM AArch64 A64 AT instruction. */
    class SgAsmArm64AtOperand: public SgAsmUnaryExpression {
    public:
#endif

        DECLARE_OTHERS(AsmArm64AtOperand);
#if defined(SgAsmArm64AtOperand_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmUnaryExpression);
            s & BOOST_SERIALIZATION_NVP(operation_);
        }
#endif

    private:
        Rose::BinaryAnalysis::Arm64AtOperation operation_;

    public:
        /** Construct a unary expression for the AT instruction's operand. */
        explicit SgAsmArm64AtOperand(Rose::BinaryAnalysis::Arm64AtOperation op)
            : operation_(op) {}

        /** Property: AT Operation.
         *
         *  An enum representing the operation to be performed.
         *
         *  @{ */
        Rose::BinaryAnalysis::Arm64AtOperation operation() const {
            return operation_;
        }
        void operation(Rose::BinaryAnalysis::Arm64AtOperation op) {
            operation_ = op;
        }
        /** @} */
#endif // SgAsmArm64AtOperand_OTHERS

#ifdef DOCUMENTATION
    };
#endif
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef ROSE_ENABLE_ASM_A64
    DECLARE_LEAF_CLASS(AsmArm64PrefetchOperand);
    IS_SERIALIZABLE(AsmArm64PrefetchOperand);

#ifdef DOCUMENTATION
    /** Operand for an ARM AArch64 A64 prefetch instruction. */
    class SgAsmArm64PrefetchOperand: public SgAsmUnaryExpression {
    public:
#endif

        DECLARE_OTHERS(AsmArm64PrefetchOperand);
#if defined(SgAsmArm64PrefetchOperand_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmUnaryExpression);
            s & BOOST_SERIALIZATION_NVP(operation_);
        }
#endif

    private:
        Rose::BinaryAnalysis::Arm64PrefetchOperation operation_;

    public:
        /** Construct a unary expression for a prefetch instruction's prefetch operand. */
        explicit SgAsmArm64PrefetchOperand(Rose::BinaryAnalysis::Arm64PrefetchOperation op)
            : operation_(op) {}

        /** Property: Prefetch operation.
         *
         *  An enum representing the operation to be performed.
         *
         *  @{ */
        Rose::BinaryAnalysis::Arm64PrefetchOperation operation() const {
            return operation_;
        }
        void operation(Rose::BinaryAnalysis::Arm64PrefetchOperation op) {
            operation_ = op;
        }
        /** @} */
#endif // SgAsmArm64PrefetchOperand_OTHERS

#ifdef DOCUMENTATION
    };
#endif
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef ROSE_ENABLE_ASM_A64
    DECLARE_LEAF_CLASS(AsmArm64SysMoveOperand);
    IS_SERIALIZABLE(AsmArm64SysMoveOperand);

#ifdef DOCUMENTATION
    /** Describes a system register for the ARM AArch64 A64 MRS and MSR instructions. */
    class SgAsmArm64SysMoveOperand: public SgAsmUnaryExpression {
    public:
#endif

        DECLARE_OTHERS(AsmArm64SysMoveOperand);
#if defined(SgAsmArm64SysMoveOperand_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmUnaryExpression);
            s & BOOST_SERIALIZATION_NVP(access_);
        }
#endif

    private:
        unsigned access_;

    public:
        /** Construct a unary expression for the system register access. */
        explicit SgAsmArm64SysMoveOperand(unsigned access)
            : access_(access) {}

        /** Property: system register access bits.
         *
         *  The bits describing how to access a system register. These come directly from the encoded instruction.
         *
         *  @{ */
        unsigned access() const {
            return access_;
        }
        void access(unsigned ac) {
            access_ = ac;
        }
        /** @} */
#endif // SgAsmArm64SysMoveOperand_OTHERS

#ifdef DOCUMENTATION
    };
#endif
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef ROSE_ENABLE_ASM_A64
    DECLARE_LEAF_CLASS(AsmArm64CImmediateOperand);
    IS_SERIALIZABLE(AsmArm64CImmediateOperand);

#ifdef DOCUMENTATION
    /** C-Immediate operand for SYS, AT, CFP, CPP, DC, DVP, IC, and TLBI instructions. */
    class SgAsmArm64CImmediateOperand: public SgAsmUnaryExpression {
    public:
#endif

        DECLARE_OTHERS(AsmArm64CImmediateOperand);
#if defined(SgAsmArm64CImmediateOperand_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmUnaryExpression);
            s & BOOST_SERIALIZATION_NVP(imm_);
        }
#endif

    private:
        unsigned imm_;

    public:
        /** Construct a unary expression for the C-immediate. */
        explicit SgAsmArm64CImmediateOperand(unsigned imm)
            : imm_(imm) {}

        /** Property: C-immediate value.
         *
         *  The C-immediate value for the instruction.
         *
         *  @{ */
        unsigned immediate() const {
            return imm_;
        }
        void immediate(unsigned imm) {
            imm_ = imm;
        }
        /** @} */
#endif // SgAsmArm64CImmediateOperand_OTHERS

#ifdef DOCUMENTATION
    };
#endif
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NEW_NONTERMINAL_MACRO(AsmUnaryExpression,
                          AsmUnaryPlus | AsmUnaryMinus | AsmUnaryRrx | AsmUnaryTruncate | AsmUnarySignedExtend
                          | AsmUnaryUnsignedExtend
#ifdef ROSE_ENABLE_ASM_A64
                          | AsmArm64AtOperand | AsmArm64PrefetchOperand | AsmArm64SysMoveOperand | AsmArm64CImmediateOperand
#endif
                          , "AsmUnaryExpression", "AsmUnaryExpressionTag", false);
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
        void serialize(S &s, const unsigned /*version*/) {
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

        DECLARE_OTHERS(AsmDirectRegisterExpression);
#if defined(SgAsmDirectRegisterExpression_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmRegisterReferenceExpression);
        }
#endif

    private:
        // Default c'tor needed for serialization
        SgAsmDirectRegisterExpression() {}
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
        Rose::BinaryAnalysis::RegisterDescriptor get_stride() const;
        void set_stride(Rose::BinaryAnalysis::RegisterDescriptor);
        /** @} */
#else
        AsmIndirectRegisterExpression.setDataPrototype("Rose::BinaryAnalysis::RegisterDescriptor", "stride", "",
                                                       CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Register descriptor offset.
         *
         *  This is the "offset" referred to in the documentation for this class.
         *
         * @{ */
        Rose::BinaryAnalysis::RegisterDescriptor get_offset() const;
        void set_offset(Rose::BinaryAnalysis::RegisterDescriptor);
        /** @} */
#else
        AsmIndirectRegisterExpression.setDataPrototype("Rose::BinaryAnalysis::RegisterDescriptor", "offset", "",
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
        void serialize(S &s, const unsigned /*version*/) {
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
        Rose::BinaryAnalysis::RegisterDescriptor get_descriptor() const;
        void set_descriptor(Rose::BinaryAnalysis::RegisterDescriptor);
        /** @} */
#else
        AsmRegisterReferenceExpression.setDataPrototype("Rose::BinaryAnalysis::RegisterDescriptor", "descriptor", "",
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
        void serialize(S &s, const unsigned /*version*/) {
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
        void serialize(S &s, const unsigned /*version*/) {
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
        void serialize(S &s, const unsigned /*version*/) {
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
        void serialize(S &s, const unsigned /*version*/) {
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
        void serialize(S &s, const unsigned /*version*/) {
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
     *  Assembly instruction (@ref SgAsmInstruction) operands are represented by @ref SgAsmExpression nodes in the AST. If the
     *  expression has a numeric value then an @ref SgAsmValueExpression is used.  Values of various types (integers and
     *  floating-point values of various sizes) are represented by subclasses of @ref SgAsmValueExpression.  Values that are
     *  addresses or references to data will have symbols in a function symbol table.  All other values are assumed to be
     *  literals and will not have associated symbols. */
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
         *  the architectures, it will be set to zero and the "bit_size" property will also be zero.  Bits are numbered so that
         *  bits zero through seven are in the first byte, bits eight through 15 are in the second byte, etc. Within a byte,
         *  bits are numbered so that lower indexes are less significant bits.
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
        void serialize(S &s, const unsigned /*version*/) {
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
        void serialize(S &s, const unsigned /*version*/) {
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
        void serialize(S &s, const unsigned /*version*/) {
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
        void serialize(S &s, const unsigned /*version*/) {
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
         *  Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics::RiscOperators "RiscOperators".
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
            OP_peekRegister,
            OP_writeRegister,
            OP_readMemory,                              /**< Three or four args depending on whether segment reg is present. */
            OP_peekMemory,
            OP_writeMemory,                             /**< Three or four args depending on whether segment reg is present. */
            OP_N_OPERATORS                              /**< Number of operators in this enum. */ // MUST BE LAST!
        };

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
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
        void serialize(S &s, const unsigned /*version*/) {
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
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmNode);
            s & BOOST_SERIALIZATION_NVP(p_type);
            s & BOOST_SERIALIZATION_NVP(p_comment);
        }
#endif

    public:
        /** Property: Width of expression in bits.
         *
         *  Returns the width of the expression in bits according to its data type. The "type" property must be non-null. */
        size_t get_nBits() const;

        /** Return a constant if possible.
         *
         *  If this expression is an integer expression with a constant that fits in a 64-bit unsigned type, then return it,
         *  otherwise return nothing. */
        Sawyer::Optional<uint64_t> asUnsigned() const;

        /** Return a signed constant if possible.
         *
         *  If this expression is an integer expression with a constant that fits in a 64-bit signed type, then return it,
         *  otherwise return nothing. */
        Sawyer::Optional<int64_t> asSigned() const;

#endif // SgAsmExpression_OTHERS

#ifdef DOCUMENTATION
    };
#endif




    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
        void serialize(S &s, const unsigned /*version*/) {
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
        virtual void check() const $ROSE_OVERRIDE;
        virtual std::string toString() const $ROSE_OVERRIDE;
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
    #include <BitFlags.h>
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
        void serialize(S &s, const unsigned /*version*/) {
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
        /** Individual bit flags for this floating-point type. */
        enum Flag {
            GRADUAL_UNDERFLOW           = 0x00000001,   /**< De-normalized signifand when exponent field is clear. */
            IMPLICIT_BIT_CONVENTION     = 0x00000002    /**< Use IEEE 754 implicit bit convention for signicand. */
        };

        /** Collective bit flags for this floating-point type. */
        typedef Rose::BitFlags<Flag> Flags;

        /** Range of bits used for various purposes within the values of this type. */
        typedef Sawyer::Container::BitVector::BitRange BitRange;

        /** Construct a new floating-point type. */
        SgAsmFloatType(ByteOrder::Endianness, size_t nBits,
                       const BitRange &significandBits, const BitRange exponentBits, size_t signBit,
                       uint64_t exponentBias, Flags flags);

        /** Property: Offset to significand least significant bit. */
        BitRange significandBits() const;

        /** Property: Number of bits in the exponent. */
        BitRange exponentBits() const;

        /** Property: Offset to significand sign bit. */
        size_t signBit() const;

        /** Property: Zero-point of exponent. */
        uint64_t exponentBias() const;

        /** Property: Bit vector of all boolean properties. */
        Flags flags() const;

        /** Default IEEE 754 flags.
         *
         *  These flags are the most common types and include the implicit bit convention for the significand and the
         *  gradual underflow capability. */
        static Flags ieeeFlags();

        /** Property: Whether type has gradual underflow.
         *
         *  If the type supports gradual underflow, then when the exponent field's bits are all clear then the integer part (a
         *  single bit implied or explicit depending on @ref implicitBitConvention) of the significand is zero instead of
         *  one. */
        bool gradualUnderflow() const;

        /** Property: Whether the type follows the IEEE 754 significand implicit bit convention.
         *
         *  If true, then the leading set bit of the significand is not stored but rather an implied. If the exponent field
         *  contains any non-zero bits then the integer part of the significand is one, otherwise when the exponent field
         *  is all clear the integer part is zero.
         *
         * @{ */
        bool implicitBitConvention() const;

        /** @} */

        // Overrides documented in base class
        virtual void check() const $ROSE_OVERRIDE;
        virtual std::string toString() const $ROSE_OVERRIDE;
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
        void serialize(S &s, const unsigned /*version*/) {
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
        virtual size_t get_nBits() const $ROSE_OVERRIDE;

        /** Property: Minor byte order. This is the usual notion of byte order. */
        ByteOrder::Endianness get_minorOrder() const;

        /** Property: Major byte order for mixed-order types. */
        ByteOrder::Endianness get_majorOrder() const;

        /** Property: Stride of major byte order for mixed order types. */
        size_t get_majorNBytes() const;

        // Overrides documented in base class
        virtual void check() const $ROSE_OVERRIDE;
        virtual std::string toString() const $ROSE_OVERRIDE;
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
        void serialize(S &s, const unsigned /*version*/) {
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
        virtual void check() const $ROSE_OVERRIDE;
        virtual std::string toString() const $ROSE_OVERRIDE;
        virtual size_t get_nBits() const $ROSE_OVERRIDE;
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
        void serialize(S &s, const unsigned /*version*/) {
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



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**************************************************************************************************************************
     *                                  Collections of Instructions
     **************************************************************************************************************************/

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmFunction);
    IS_SERIALIZABLE(AsmFunction);

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
        /** Property: Additional function existance reason comment.
         *
         *  This reason comment gets filled in automatically by certain function analyses. It's a free-form string that
         *  contains additional information about why this function exists and is used in conjunction with the @ref get_reason
         *  property.
         *
         * @{ */
        const std::string& get_reasonComment() const;
        void set_reasonComment(const std::string&);
        /** @} */
#else
        AsmFunction.setDataPrototype("std::string", "reasonComment", "",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
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
         *  This is the name of the primary calling convention for this function. When the semantics of the function match
         *  multiple calling convention definitions, this property holds the "best" one for some definition of "best". It is
         *  also possible for the semantics to not match any calling convention definition in which case this property is
         *  empty. It is also empty if the calling convention analysis was not performed.
         *
         * @{ */
        const std::string& get_callingConvention() const;
        void set_callingConvention(const std::string&);
        /** @} */
#else
        AsmFunction.setDataPrototype("std::string", "callingConvention", "",
                                     NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmFunction);
#if defined(SgAsmFunction_OTHERS) || defined(DOCUMENTATION)

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmSynthesizedDeclaration);
            s & BOOST_SERIALIZATION_NVP(p_name);
            s & BOOST_SERIALIZATION_NVP(p_reason);
            s & BOOST_SERIALIZATION_NVP(p_reasonComment);
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
        /** Adds statement to end of statement list. */
        void append_statement(SgAsmStatement*);

        /** Erases statement from statement list.
         *
         *  If the specified statement is found in the list of statements then it is erased without being deleted. */
        void remove_statement(SgAsmStatement* statement);

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
                FUNC_THUNK_TARGET= 0x00004000,  /**< Function is the target of a thunk. */
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
                FUNC_INTERPADFUNC = 1,          /**< Detected by Partitioner::FindInterPadFunctions, which looks for unassigned
                                                 *   space between two inter-function padding blocks and makes the first such
                                                 *   address the beginning of one of these functions. */
                FUNC_PESCRAMBLER_DISPATCH = 2, /**<  Dispatcher function for code generated by pescrambler. */
                FUNC_CONFIGURED = 3,           /**<  Function is defined in a configuration file. */
                FUNC_CMDLINE = 4,              /**<  Function mentioned on the command-line. */
                FUNC_SCAN_RO_DATA = 5,         /**<  Address was found in read-only data area by scanning the data. */
                FUNC_INSN_RO_DATA = 6,         /**<  Address was found in read-only data referenced by an existing
                                                *    instruction. */
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
     *  A SgAsmBlock usually represents a sequence of instructions. It's also used for grouping other things such as
     *  functions. A SgAsmBlock represents a basic block if and only if it has at least one descendant of type @ref
     *  SgAsmInstruction and it has no descendants of type SgAsmBlock.
     *
     *  In the absence of interrupt handling, the instructions of a basic block are executed entirely.  In the absense of
     *  multi-threading, no other instructions intervene.
     *
     *  The instructions of a basic block need not be contiguous in virtual memory. They also do not need to be at increasing
     *  virtual addresses.
     *
     *  If the basic block has a parent pointer, then the closest @ref SgAsmFunction ancestor is the one to which this basic
     *  block belongs. In the @ref Rose::BinaryAnalysis::Partitioner2::Partitioner "Partitioner" API, a basic block can be
     *  owned by more than one function.
     *
     *  An AST may have multiple SgAsmBlock objects that represent the same basic block. This happens when a single block
     *  belongs to more than one function.
     *
     *  See also, @ref Rose::BinaryAnalysis::Partitioner2::BasicBlock, which represents a basic block outside the AST. */
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
         *  Rose::BinaryAnalysis::Partitioner2 "Partitioner2" name space has a more useful definition of control flow graph
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
         *  The @ref Rose::BinaryAnalysis::Partitioner2 "Partitioner2" name space has a more useful definition of control flow
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
         *  The @ref Rose::BinaryAnalysis::Partitioner2 "Partitioner2" name space has a more useful definition of control flow
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
         *  The @ref Rose::BinaryAnalysis::Partitioner2 "Partitioner2" name space has a more useful definition of control flow
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
        void serialize(S &s, const unsigned /*version*/) {
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
        void serialize(S &s, const unsigned /*version*/) {
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
#if defined(SgAsmSynthesizedDataStructureDeclaration_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S & s, const unsigned /*version*/) {
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
        void serialize(S &s, const unsigned /*version*/) {
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
        void serialize(S &s, const unsigned /*version*/) {
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
    /** Base class for statement-like subclasses.
     *
     *  This is a base class for those binary analysis entities, such as instructions and basic blocks, that have a starting
     *  address in the virtual address space. */
    class SgAsmStatement: public SgAsmNode {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Starting virtual address.
         *
         *  Virtual address of first byte of instruction, block, or whatever, depending on subclass.
         *
         *  @{ */
        rose_addr_t get_address() const;
        void set_address(rose_addr_t);
        /** @} */
#else
        AsmStatement.setDataPrototype("rose_addr_t", "address", "= 0",
                                      CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
#endif


#ifdef DOCUMENTATION
        /** Property: Commentary.
         *
         *  @{ */
        const std::string& get_comment() const;
        void set_comment(const std::string&);
        /** @} */
#else
        AsmStatement.setDataPrototype("std::string", "comment", "= \"\"",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, COPY_DATA);
#endif

        DECLARE_OTHERS(AsmStatement);
#if defined(SgAsmStatement_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgNode);
            s & BOOST_SERIALIZATION_NVP(p_address);
            s & BOOST_SERIALIZATION_NVP(p_comment);
        }
#endif
#endif // SgAsmStatement_OTHERS

#ifdef DOCUMENTATION
    };
#endif




    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*************************************************************************************************************************
     *                                         Binary Interpretations
     * Some file formats have more than one thing in them: PE files have a DOS and Windows executable both in the same file;
     * Apple OS X files can have multiple architectures represented in a single file, etc.  The interpretation is the root of
     * an AST that represents a single, coherent sub-part of the file.
     *************************************************************************************************************************/

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmInterpretationList);
    IS_SERIALIZABLE(AsmInterpretationList);

#ifdef DOCUMENTATION
    /** List of binary interpretations. */
    class SgAsmInterpretationList: public SgAsmNode {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Interpretation list.
         *
         *  The actual list of pointers to interpretations. The only reason this list is in its own node instead of being
         *  contained directly in the node that needs it is because of limitations of ROSETTA.
         *
         * @{ */
        const SgAsmInterpretationList& get_interpretations() const;
        void set_interpretations(const SgAsmInterpretationList&);
        /** @} */
#else
        AsmInterpretationList.setDataPrototype("SgAsmInterpretationPtrList", "interpretations", "",
                                               NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL,
                                               NO_DELETE);
#endif

        DECLARE_OTHERS(AsmInterpretationList);
#if defined(SgAsmInterpretationList_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmNode);
            s & BOOST_SERIALIZATION_NVP(p_interpretations);
        }
#endif
#endif // SgAsmInterpretationList_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmInterpretation);
    IS_SERIALIZABLE(AsmInterpretation);

    DECLARE_HEADERS(AsmInterpretation);
#if defined(SgAsmInterpretation_HEADERS) || defined(DOCUMENTATION)
    #include <MemoryMap.h>
    namespace Rose { namespace BinaryAnalysis { class RegisterDictionary; } }
#endif // SgAsmInterpretation_HEADERS

#ifdef DOCUMENTATION
    /** Represents an interpretation of a binary container.
     *
     *  An interpretation is a collection of the parts of a binary specimen that represent a coherent program, library, core
     *  dump, etc. For instance, a Windows PE executable has a DOS interpretation and a Windows interpretation--really two
     *  executables in the one container. All the DOS-related stuff will be under one SgAsmInterpretation AST and all the
     *  Windows-related stuff will be under another SgAsmInterpretation AST. */
    class SgAsmInterpretation: public SgAsmNode {
    public:
#endif

#ifdef DOCUMENTATION
        // documentation and definition are below
#else
        AsmInterpretation.setAutomaticGenerationOfConstructor(false);
#endif

#ifdef DOCUMENTATION
        /** Property: File headers.
         *
         *  List of pointers to the file headers that compose this interpretation.
         *
         *  These headers are not considered to be children of this interpretation in the AST--they are reached from other
         *  traversal paths.
         *
         * @{ */
        SgAsmGenericHeaderList* get_headers() const;
        void set_headers(SgAsmGenericHeaderList*);
        /** @} */
#else
        AsmInterpretation.setDataPrototype("SgAsmGenericHeaderList*", "headers", "= NULL",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Global block.
         *
         *  The global block is the top of the AST for this interpretation's functions, basic blocks, and instructions.
         *
         * @{ */
        SgAsmBlock* get_global_block() const;
        void set_global_block(SgAsmBlock*);
        /** @} */
#else
        AsmInterpretation.setDataPrototype("SgAsmBlock*", "global_block", "= NULL",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmInterpretation);
#if defined(SgAsmInterpretation_OTHERS) || defined(DOCUMENTATION)
    private:
        Rose::BinaryAnalysis::MemoryMap::Ptr p_map;
        const Rose::BinaryAnalysis::RegisterDictionary *p_registers;
        bool coverageComputed;                          // true iff percentageCoverage has been computed
        mutable InstructionMap instruction_map;         // cached instruction map

        //! The percentage of an interpretation where each section is marked as executable and identified for
        //  disassembly into instructions.
        double percentageCoverage;

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmNode);
            s & BOOST_SERIALIZATION_NVP(p_headers);
            s & BOOST_SERIALIZATION_NVP(p_global_block);
            s & BOOST_SERIALIZATION_NVP(p_map);
            s & BOOST_SERIALIZATION_NVP(p_registers);
            s & BOOST_SERIALIZATION_NVP(coverageComputed);
            s & BOOST_SERIALIZATION_NVP(instruction_map);
            s & BOOST_SERIALIZATION_NVP(percentageCoverage);
        }
#endif

    public:
        /** Default constructor. */
        SgAsmInterpretation()
            : p_registers(NULL), coverageComputed(false), percentageCoverage(0.0), p_headers(NULL), p_global_block(NULL) {
            ctor();
        }

        /** Returns a list of all files referenced by an interpretation.
         *
         *  It does this by looking at the file headers referenced by the interpretation, following their parent pointers up to
         *  an SgAsmGenericFile node, and returning a vector of those nodes with duplicate files removed. */
        SgAsmGenericFilePtrList get_files() const;

        /** Property: Memory map.
         *
         *  This is the memory map representing the entire interpretation.
         *
         * @{ */
        Rose::BinaryAnalysis::MemoryMap::Ptr get_map() const {return p_map;}
        void set_map(const Rose::BinaryAnalysis::MemoryMap::Ptr &m) {p_map=m;}
        /** @} */

        /** Property: Register dictionary.
         *
         *  The register dictionary is a list of all register descriptors and how register names map to descriptors. The side
         *  effect of descriptors is to describe how registers overlap with each other.
         *
         * @{ */
        const Rose::BinaryAnalysis::RegisterDictionary *get_registers() const;
        void set_registers(const Rose::BinaryAnalysis::RegisterDictionary*);
        /** @} */

        /** Property: Map of instructions by address.
         *
         *  Returns the @ref InstructionMap associated with an interpretation. The instruction map is recomputed if the
         *  currently cached map is empty or if the @p recompute argument is true. Otherwise this just returns the existing
         *  map. No attempt is made to make sure that the map is up-to-date with respect to the current state of the AST.
         *
         * @{ */
        InstructionMap& get_instruction_map(bool recompute=false);
        void set_instruction_map(const InstructionMap&);
        /** @} */

        /** Populate a map of instructions indexed by their virtual addresses.
         *
         *  This function traverses the AST rooted at the @ref get_global_block "global_block" and inserts each encountered
         *  instruction into the provided @ref InstructionMap based on its starting virtual address. */
        void insert_instructions(InstructionMap&/*in,out*/);

        /** Erase instructions from a map.
         *
         *  This function traverses the AST rooted at the @ref get_global_block "global_block" and erases each encountered
         *  instruction from the provided @ref InstructionMap based on its starting virtual address. */
        void erase_instructions(InstructionMap&/*in,out*/);

        /** Property: code coverage percent.
         *
         *  Results of a code coverage analysis.
         *
         * @{ */
        void set_coverageComputed(bool x) { coverageComputed = x; }
        void set_percentageCoverage(double x) { percentageCoverage = x; }
        /** @} */

    private:
        void ctor();                                    // finalize construction
#endif // SgAsmInterpretation_OTHERS

#ifdef DOCUMENTATION
    };
#endif



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*************************************************************************************************************************
     *                                         ELF File Header
     *************************************************************************************************************************/

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfFileHeader);
    IS_SERIALIZABLE(AsmElfFileHeader);

#ifdef DOCUMENTATION
    /** Represents the file header of an ELF binary container.
     *
     *  The file header contains information that the operating system uses to find the various parts within the
     *  container. Most of the object properties are defined in the official ELF specification and their documentation is not
     *  replicated here.
     *
     *  ROSE does not require or use an ELF support library or headers and is thus able to parse ELF files on systems where ELF
     *  is not normally employed. */
    class SgAsmElfFileHeader: public SgAsmGenericHeader {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: File class.
         *
         *  See ELF specification for details.
         *
         * @{ */
        unsigned char get_e_ident_file_class() const;
        void set_e_ident_file_class(unsigned char);
        /** @} */
#else
        AsmElfFileHeader.setDataPrototype("unsigned char", "e_ident_file_class", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Data encoding.
         *
         *  See ELF specification for details.
         *
         * @{ */
        unsigned char get_e_ident_data_encoding() const;
        void set_e_ident_data_encoding(unsigned char);
        /** @} */
#else
        AsmElfFileHeader.setDataPrototype("unsigned char", "e_ident_data_encoding", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: File version.
         *
         *  See ELF specification for details.
         *
         * @{ */
        unsigned char get_e_ident_file_version() const;
        void set_e_ident_file_version(unsigned char*);
        /** @} */
#else
        AsmElfFileHeader.setDataPrototype("unsigned char", "e_ident_file_version", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Padding.
         *
         *  See ELF specification for details.
         *
         * @{ */
        const SgUnsignedCharList& get_e_ident_padding() const;
        void set_e_ident_padding(const SgUnsignedCharList&);
        /** @} */
#else
        AsmElfFileHeader.setDataPrototype("SgUnsignedCharList", "e_ident_padding", "",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Type.
         *
         *  See ELF specification for details.
         *
         * @{ */
        unsigned long get_e_type() const;
        void set_e_type(unsigned long);
        /** @} */
#else
        AsmElfFileHeader.setDataPrototype("unsigned long", "e_type", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Machine.
         *
         *  See ELF specification for details.
         *
         * @{ */
        unsigned long get_e_machine() const;
        void set_e_machine(unsigned long);
        /** @} */
#else
        AsmElfFileHeader.setDataPrototype("unsigned long", "e_machine", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Flags.
         *
         *  See ELF specification for details.
         *
         * @{ */
        unsigned long get_e_flags() const;
        void set_e_flags(unsigned long);
        /** @} */
#else
        AsmElfFileHeader.setDataPrototype("unsigned long", "e_flags", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: ehsize.
         *
         *  See ELF specification for details.
         *
         * @{ */
        unsigned long get_e_ehsize() const;
        void set_e_ehsize(unsigned long);
        /** @} */
#else
        AsmElfFileHeader.setDataPrototype("unsigned long", "e_ehsize", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: phextrasz.
         *
         *  See ELF specification for details.
         *
         * @{ */
        unsigned long get_phextrasz() const;
        void set_phextrasz(unsigned long);
        /** @} */
#else
        AsmElfFileHeader.setDataPrototype("unsigned long", "phextrasz", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: phnum.
         *
         *  See ELF specification for details.
         *
         * @{ */
        unsigned long get_e_phnum() const;
        void set_e_phnum(unsigned long);
        /** @} */
#else
        AsmElfFileHeader.setDataPrototype("unsigned long", "e_phnum", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: shextrasz.
         *
         *  See ELF specification for details.
         *
         * @{ */
        unsigned long get_shextrasz() const;
        void set_shextrasz(unsigned long);
        /** @} */
#else
        AsmElfFileHeader.setDataPrototype("unsigned long", "shextrasz", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: shnum.
         *
         *  See ELF specification for details.
         *
         * @{ */
        unsigned long get_e_shnum() const;
        void set_e_shnum(unsigned long);
        /** @} */
#else
        AsmElfFileHeader.setDataPrototype("unsigned long", "e_shnum", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: shstrndx.
         *
         *  See ELF specification for details.
         *
         * @{ */
        unsigned long get_e_shstrndx() const;
        void set_e_shstrndx(unsigned long);
        /** @} */
#else
        AsmElfFileHeader.setDataPrototype("unsigned long", "e_shstrndx", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Section table.
         *
         *  Points to the AST node that represents the ELF section table that describes each section of the file. ELF sections
         *  are generally those parts of the file that are of interest to linkers, debuggers, etc. but not needed by the
         *  program loader.
         *
         * @{ */
        SgAsmElfSectionTable* get_section_table() const;
        void set_section_table(SgAsmElfSectionTable*);
        /** @} */
#else
        AsmElfFileHeader.setDataPrototype("SgAsmElfSectionTable*", "section_table", "= NULL",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Segment table.
         *
         *  Points to the AST node that represents the ELF segment table that describes each segment of the file. Segments
         *  describe how parts of the file are mapped into virtual memory by the loader.
         *
         * @{ */
        SgAsmElfSegmentTable* get_segment_table() const;
        void set_segment_table(SgAsmElfSegmentTable*);
        /** @} */
#else
        AsmElfFileHeader.setDataPrototype("SgAsmElfSegmentTable*", "segment_table", "= NULL",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmElfFileHeader);
#if defined(SgAsmElfFileHeader_OTHERS) || defined(DOCUMENTATION)

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmGenericHeader);
            s & BOOST_SERIALIZATION_NVP(p_e_ident_file_class);
            s & BOOST_SERIALIZATION_NVP(p_e_ident_data_encoding);
            s & BOOST_SERIALIZATION_NVP(p_e_ident_file_version);
            s & BOOST_SERIALIZATION_NVP(p_e_ident_padding);
            s & BOOST_SERIALIZATION_NVP(p_e_type);
            s & BOOST_SERIALIZATION_NVP(p_e_machine);
            s & BOOST_SERIALIZATION_NVP(p_e_flags);
            s & BOOST_SERIALIZATION_NVP(p_e_ehsize);
            s & BOOST_SERIALIZATION_NVP(p_phextrasz);
            s & BOOST_SERIALIZATION_NVP(p_e_phnum);
            s & BOOST_SERIALIZATION_NVP(p_shextrasz);
            s & BOOST_SERIALIZATION_NVP(p_e_shnum);
            s & BOOST_SERIALIZATION_NVP(p_e_shstrndx);
            s & BOOST_SERIALIZATION_NVP(p_section_table);
            s & BOOST_SERIALIZATION_NVP(p_segment_table);
        }
#endif

    public:
        /** Enum for the @ref get_e_type "e_type" property. */
        enum ObjectType {
            ET_NONE         = 0                         /**< No file type */
            ,ET_REL          = 1                        /**< Relocatable file */
            ,ET_EXEC         = 2                        /**< Executable file */
            ,ET_DYN          = 3                        /**< Shared object file */
            ,ET_CORE         = 4                        /**< Core file */

            ,ET_LOOS         = 0xfe00                   /**< OS-specific range start */
            ,ET_HIOS         = 0xfeff                   /**< OS-specific range end */
            ,ET_LOPROC       = 0xff00                   /**< Processor-specific range start */
            ,ET_HIPROC       = 0xffff                   /**< Processor-specific range end */
        };

        // Some structures are used to represent certain things whose layout is very precise in binary files, thus we need to
        // make sure the compiler doesn't insert alignment padding between the struct members.  ROSE can be compiled on an
        // architecture that has different alignment constraints than the architecture that these structs describe. GNU
        // compilers have long used the attribute mechanism. Microsoft compilers on the other hand use pragmas. GCC versions
        // 4.0 and earlier do not recognize the Microsoft pragmas and issue compiler errors when one is encountered.
#ifdef _MSC_VER
# pragma pack (1)
#endif

        /** File format of an ELF header.
         *
         *  Byte order of members depends on e_ident value. This code comes directly from "System V Application Binary
         *  Interface, Edition 4.1" and the FreeBSD elf(5) man page, and the "Executable and Linkable Format (ELF) Portable
         *  Formats Specifications, Version 1.2" and not from any header file. */
        struct Elf32FileHeader_disk {
            unsigned char       e_ident_magic[4];       /**< 0x7f, 'E', 'L', 'F' */
            unsigned char       e_ident_file_class;     /**< 1=>32-bit; 2=>64-bit; other is error */
            unsigned char       e_ident_data_encoding;  /**< 1=>LSB; 2=>MSB; other is error */
            unsigned char       e_ident_file_version;   /**< Format version number (same as e_version); must be 1 */
            unsigned char       e_ident_padding[9];     /**< Padding to byte 16; must be zero */
            uint16_t            e_type;                 /**< Object file type: relocatable, executable, lib, core */
            uint16_t            e_machine;              /**< Required architecture for an individual file */
            uint32_t            e_version;              /**< Object file version, currently zero or one */
            uint32_t            e_entry;                /**< Entry virtual address or zero if none */
            uint32_t            e_phoff;                /**< File offset of program header table or zero if none */
            uint32_t            e_shoff;                /**< File offset of section header table or zero if none */
            uint32_t            e_flags;                /**< Processor-specific flags (EF_* constants in docs) */
            uint16_t            e_ehsize;               /**< Size of ELF header in bytes */
            uint16_t            e_phentsize;            /**< Size of each entry in the program header table */
            uint16_t            e_phnum;                /**< Number of program headers, or PN_XNUM, or zero */
            uint16_t            e_shentsize;            /**< Size of each entry in the section header table */
            uint16_t            e_shnum;                /**< Number of section headers, or zero for extended entries */
            uint16_t            e_shstrndx;             /**< Index of name section, or SHN_UNDEF, or SHN_XINDEX */
        }
#if !defined(SWIG) && !defined(_MSC_VER)
        __attribute__((packed))
#endif
        ;

        struct Elf64FileHeader_disk {
            unsigned char       e_ident_magic[4];
            unsigned char       e_ident_file_class;
            unsigned char       e_ident_data_encoding;
            unsigned char       e_ident_file_version;
            unsigned char       e_ident_padding[9];
            uint16_t            e_type;
            uint16_t            e_machine;
            uint32_t            e_version;
            uint64_t            e_entry;
            uint64_t            e_phoff;
            uint64_t            e_shoff;
            uint32_t            e_flags;
            uint16_t            e_ehsize;
            uint16_t            e_phentsize;
            uint16_t            e_phnum;
            uint16_t            e_shentsize;
            uint16_t            e_shnum;
            uint16_t            e_shstrndx;
        }
#if !defined(SWIG) && !defined(_MSC_VER)
        __attribute__((packed))
#endif
        ;

#ifdef _MSC_VER
# pragma pack ()
#endif

        /** Construct a new ELF File Header with default values.
         *
         *  The new section is placed at file offset zero and the size is initially one byte (calling @ref parse will extend it
         *  as necessary). Setting the initial size of non-parsed sections to a positive value works better when adding
         *  sections to the end-of-file since the sections will all have different starting offsets and therefore @ref
         *  SgAsmGenericFile::shift_extend will know what order the sections should be in when they are eventually resized. */
        explicit SgAsmElfFileHeader(SgAsmGenericFile *f)
            : SgAsmGenericHeader(f) , p_e_ident_file_class(0), p_e_ident_file_version(1), p_e_type(0),
              p_e_machine(0), p_e_flags(0), p_e_ehsize(0), p_phextrasz(0), p_e_phnum(0), p_shextrasz(0),
              p_e_shnum(0), p_e_shstrndx(0), p_section_table(NULL), p_segment_table(NULL) {
            ctor();
        }

        /** Maximum page size according to the ABI.
         *
         *  This is used by the loader when calculating the program base address. Since parts of the file are mapped into the
         *  process address space those parts must be aligned (both in the file and in memory) on the largest possible page
         *  boundary so that any smaller page boundary will also work correctly. */
        uint64_t max_page_size();

        /** Convert ELF "machine" identifier to generic instruction set architecture value. */
        static SgAsmExecutableFileFormat::InsSetArchitecture machine_to_isa(unsigned machine);

        /** Convert architecture value to an ELF "machine" value. */
        unsigned isa_to_machine(SgAsmExecutableFileFormat::InsSetArchitecture isa) const;

        /** Parse header from file.
         *
         *  Initialize this header with information parsed from the file and construct and parse everything that's reachable
         *  from the header. Since the size of the ELF File Header is determined by the contents of the ELF File Header as
         *  stored in the file, the size of the ELF File Header will be adjusted upward if necessary. */
        virtual SgAsmElfFileHeader *parse() $ROSE_OVERRIDE;

        /** Update prior to unparsing */
        virtual bool reallocate() $ROSE_OVERRIDE;

        /** Write ELF contents back to a file. */
        virtual void unparse(std::ostream&) const $ROSE_OVERRIDE;

        /** Print some debugging info */
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const $ROSE_OVERRIDE;

        /** Return true if the file looks like it might be an ELF file according to the magic number. */
        static bool is_ELF(SgAsmGenericFile*);

        /** Get the list of sections defined in the ELF Section Table */
        SgAsmGenericSectionPtrList get_sectab_sections();

        /** Get the list of sections defined in the ELF Segment Table */
        SgAsmGenericSectionPtrList get_segtab_sections();

        // Overrides documented in base class
        virtual const char *format_name() const $ROSE_OVERRIDE;

    private:
        void ctor();                                    // called by constructors
        void *encode(ByteOrder::Endianness, SgAsmElfFileHeader::Elf32FileHeader_disk*) const;
        void *encode(ByteOrder::Endianness, SgAsmElfFileHeader::Elf64FileHeader_disk*) const;
#endif // SgAsmElfFileHeader_OTHERS

#ifdef DOCUMENTATION
    };
#endif



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*************************************************************************************************************************
     *                                         ELF Section Tables
     *************************************************************************************************************************/

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfSectionTable);
    IS_SERIALIZABLE(AsmElfSectionTable);

#ifdef DOCUMENTATION
    /** Represents an ELF section table.
     *
     *  The ELF Section Table is itself a section.  The entries of the table are stored with the section they describe rather
     *  than storing them all in the SgAsmSectionTable node.  We can reconstruct the ELF Section Table since sections have
     *  unique ID numbers that are their original indices in the ELF Section Table. */
    class SgAsmElfSectionTable: public SgAsmGenericSection {
    public:
#endif

        DECLARE_OTHERS(AsmElfSectionTable);
#if defined(SgAsmElfSectionTable_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmGenericSection);
        }
#endif

    public:
        /** Constructor creates section table within the AST. */
        explicit SgAsmElfSectionTable(SgAsmElfFileHeader *fhdr)
            : SgAsmGenericSection(fhdr->get_file(), fhdr) {
            ctor();
        }

        /** Parses an ELF Section Table.
         *
         *  Parses an ELF section table and constructs and parses all sections reachable from the table. The section is
         *  extended as necessary based on the number of entries and the size of each entry. Returns a pointer to this
         *  object. */
        virtual SgAsmElfSectionTable *parse() $ROSE_OVERRIDE;

        /** Attaches a previously unattached ELF Section to the section table.
         *
         *  If @p section is an ELF String Section (SgAsmElfStringSection) that contains an ELF String Table
         *  (SgAsmElfStringTable) and the ELF Section Table has no associated string table then the @p section will be used as
         *  the string table to hold the section names.
         *
         *  This method complements SgAsmElfSection::init_from_section_table. This method initializes the section table from
         *  the section while init_from_section_table() initializes the section from the section table.
         *
         *  Returns the new section table entry linked into the AST. */
        SgAsmElfSectionTableEntry *add_section(SgAsmElfSection*);

        /** Returns info about the size of the entries based on information already available.
         *
         *  Any or all arguments may be null pointers if the caller is not interested in the value. */
        rose_addr_t calculate_sizes(size_t *entsize, size_t *required, size_t *optional, size_t *entcount) const;

        /** Update prior to unparsing. */
        virtual bool reallocate() $ROSE_OVERRIDE;

        /** Write the section table section back to disk */
        virtual void unparse(std::ostream&) const $ROSE_OVERRIDE;

        /** Print some debugging info */
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const $ROSE_OVERRIDE;

    private:
        void ctor();
#endif // SgAsmElfSectionTable_OTHERS

#ifdef DOCUMENTATION
    };
#endif


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
        void serialize(S &s, const unsigned /*version*/) {
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
#if !defined(SWIG) && !defined(_MSC_VER)
        __attribute__((packed))
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
#if !defined(SWIG) && !defined(_MSC_VER)
        __attribute__((packed))
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

        // Use Rose::stringify... function instead.
        static std::string to_string(SgAsmElfSectionTableEntry::SectionType);
        static std::string to_string(SgAsmElfSectionTableEntry::SectionFlags);

    private:
        void ctor(ByteOrder::Endianness, const SgAsmElfSectionTableEntry::Elf32SectionTableEntry_disk*);
        void ctor(ByteOrder::Endianness, const SgAsmElfSectionTableEntry::Elf64SectionTableEntry_disk*);
#endif // SgAsmElfSectionTableEntry_OTHERS

#ifdef DOCUMENTATION
    };
#endif



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*************************************************************************************************************************
     *                                         ELF Segment Tables
     *************************************************************************************************************************/

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfSegmentTable);
    IS_SERIALIZABLE(AsmElfSegmentTable);

#ifdef DOCUMENTATION
    /** Represents an ELF segment table.
     *
     * The ELF Segment Table is an ELF Section that has entries describing the various segments of the ELF file.  Each segment
     * is also an SgAsmElfSection and the entries of the ELF Segment Table are associated with the SgAsmElfSection they
     * describe.  The ELF Segment Table can be reconstructed by traversing the AST and finding the SgAsmElfSegmentTableEntry
     * nodes. */
    class SgAsmElfSegmentTable: public SgAsmGenericSection {
    public:
#endif

        DECLARE_OTHERS(AsmElfSegmentTable);
#if defined(SgAsmElfSegmentTable_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmGenericSection);
        }
#endif

    public:
        /** Constuct segment table linked into the AST. */
        explicit SgAsmElfSegmentTable(SgAsmElfFileHeader *fhdr)
            : SgAsmGenericSection(fhdr->get_file(), fhdr) {
            ctor();
        }

        /** Parses an ELF Segment (Program Header) Table.
         *
         *  Parses an ELF segment table and constructs and parses all segments reachable from the table. The section is
         *  extended as necessary based on the number of entries and teh size of each entry. */
        virtual SgAsmElfSegmentTable *parse() $ROSE_OVERRIDE;

        /** Attaches new segments to the segment table.
         *
         *  Attaches a previously unattached ELF Segment (@ref SgAsmElfSection) to the ELF Segment Table (@ref
         *  SgAsmElfSegmentTable). This method complements @ref SgAsmElfSection::init_from_segment_table. This method
         *  initializes the segment table from the segment while init_from_segment_table initializes the segment from the
         *  segment table.
         *
         *  ELF Segments are represented by @ref SgAsmElfSection objects since ELF Segments and ELF Sections overlap very much
         *  in their features and thus should share an interface. An @ref SgAsmElfSection can appear in the ELF Section Table
         *  and/or the ELF Segment Table and you can determine where it was located by calling @ref get_section_entry and
         *  @ref get_segment_entry.
         *
         *  Returns the new segment table entry linked into the AST. */
        SgAsmElfSegmentTableEntry *add_section(SgAsmElfSection*);

        /** Returns info about the size of the entries based on information already available.
         *
         *  Any or all arguments may be null pointers if the caller is not interested in the value. */
        rose_addr_t calculate_sizes(size_t *entsize, size_t *required, size_t *optional, size_t *entcount) const;

        /** Pre-unparsing updates */
        virtual bool reallocate() $ROSE_OVERRIDE;

        /** Write the segment table to disk. */
        virtual void unparse(std::ostream&) const $ROSE_OVERRIDE;

        /** Print some debugging info */
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const $ROSE_OVERRIDE;

        private:
                void ctor();
#endif // SgAsmElfSegmentTable_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfSegmentTableEntryList);
    IS_SERIALIZABLE(AsmElfSegmentTableEntryList);

#ifdef DOCUMENTATION
    class SgAsmElfSegmentTableEntryList: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Segment table entries.
         *
         *  List of entries in this segment table.  The reason we have a whole AST node dedicated to holding this list rather
         *  than just storing the list directly in the nodes that need it is due to limitations with ROSETTA.
         *
         * @{ */
        const SgAsmElfSegmentTableEntryPtrList& get_entries();
        void set_entries(const SgAsmElfSegmentTableEntryPtrList&);
        /** @} */
#else
        AsmElfSegmentTableEntryList.setDataPrototype("SgAsmElfSegmentTableEntryPtrList", "entries", "",
                                                     NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL,
                                                     NO_DELETE);
#endif

        DECLARE_OTHERS(AsmElfSegmentTableEntryList);
#if defined(SgAsmElfSegmentTableEntryList_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
        }
#endif
#endif // SgAsmElfSegmentTableEntryList_OTHERS

#ifdef DOCUMENTATION
    };
#endif


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfSegmentTableEntry);
    IS_SERIALIZABLE(AsmElfSegmentTableEntry);

#ifdef DOCUMENTATION
    /** Represents one entry of a segment table. */
    class SgAsmElfSegmentTableEntry: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Index into table.
         *
         *  This is the index of this entry within the ELF segment table.
         *
         * @{ */
        size_t get_index() const;
        void set_index(size_t);
        /** @} */
#else
        AsmElfSegmentTableEntry.setDataPrototype("size_t", "index", "= 0",
                                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Segment type.
         *
         * @{ */
        SegmentType get_type() const;
        void set_type(SegmentType);
        /** @} */
#else
        AsmElfSegmentTableEntry.setDataPrototype("SgAsmElfSegmentTableEntry::SegmentType", "type",
                                                 "= SgAsmElfSegmentTableEntry::PT_LOAD",
                                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Segment flags.
         *
         * @{ */
        SegmentFlags get_flags() const;
        void set_flags(SegmentFlags);
        /** @} */
#else
        AsmElfSegmentTableEntry.setDataPrototype("SgAsmElfSegmentTableEntry::SegmentFlags", "flags",
                                                 "= SgAsmElfSegmentTableEntry::PF_NONE",
                                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Offset of segment in the file.
         *
         *  This is the starting byte offset of the segment within the file.
         *
         * @{ */
        rose_addr_t get_offset() const;
        void set_offset(rose_addr_t);
        /** @} */
#else
        AsmElfSegmentTableEntry.setDataPrototype("rose_addr_t", "offset", "= 0",
                                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Virtual address.
         *
         *  This is the virtual address for the start of the segment as stored in the segment table. This is only a hint to the
         *  loader, which may map the segment to some other virtual address.
         *
         * @{ */
        rose_addr_t get_vaddr() const;
        void set_vaddr(rose_addr_t);
        /** @} */
#else
        AsmElfSegmentTableEntry.setDataPrototype("rose_addr_t", "vaddr", "= 0",
                                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: ELF paddr field.
         *
         *  See official ELF specification.
         *
         * @{ */
        rose_addr_t get_paddr() const;
        void set_paddr(rose_addr_t);
        /** @} */
#else
        AsmElfSegmentTableEntry.setDataPrototype("rose_addr_t", "paddr", "= 0",
                                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Size of segment in file.
         *
         *  Size of the segment in bytes as it is stored in the file.
         *
         * @{ */
        rose_addr_t get_filesz() const;
        void set_filesz(rose_addr_t);
        /** @} */
#else
        AsmElfSegmentTableEntry.setDataPrototype("rose_addr_t", "filesz", "= 0",
                                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property:  Size of segment in memory.
         *
         *  Size of the segment in bytes after it is loaded into virtual memory.
         *
         * @{ */
        rose_addr_t get_memsz() const;
        void set_memsz(rose_addr_t);
        /** @} */
#else
        AsmElfSegmentTableEntry.setDataPrototype("rose_addr_t", "memsz", "= 0",
                                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Alignment.
         *
         *  Alignment in memory in bytes. Zero means the same thing as one, namely no alignment.
         *
         * @{ */
        rose_addr_t get_align() const;
        void set_align(rose_addr_t);
        /** @} */
#else
        AsmElfSegmentTableEntry.setDataPrototype("rose_addr_t", "align", "= 0",
                                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Extra bytes.
         *
         *  These are bytes from the table entry that are not assigned any specific purpose by the ELF specification.
         *
         * @{ */
        const SgUnsignedCharList& get_extra() const;
        void set_extra(const SgUnsignedCharList&);
        /** @} */
#else
        AsmElfSegmentTableEntry.setDataPrototype("SgUnsignedCharList", "extra", "",
                                                 NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmElfSegmentTableEntry);
#if defined(SgAsmElfSegmentTableEntry_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_index);
            s & BOOST_SERIALIZATION_NVP(p_type);
            s & BOOST_SERIALIZATION_NVP(p_flags);
            s & BOOST_SERIALIZATION_NVP(p_offset);
            s & BOOST_SERIALIZATION_NVP(p_vaddr);
            s & BOOST_SERIALIZATION_NVP(p_paddr);
            s & BOOST_SERIALIZATION_NVP(p_filesz);
            s & BOOST_SERIALIZATION_NVP(p_memsz);
            s & BOOST_SERIALIZATION_NVP(p_align);
            s & BOOST_SERIALIZATION_NVP(p_extra);
        }
#endif

    public:
        /** Segment types (host order). All other values are reserved. */
        enum SegmentType {
            PT_NULL         = 0,                        /**< Ignored entry. Other values of entry are undefined. */
            PT_LOAD         = 1,                        /**< Loadable by mapping file contents into memory. */
            PT_DYNAMIC      = 2,                        /**< Dynamic linking information. */
            PT_INTERP       = 3,                        /**< Segment contains NUL-terminated path name of interpreter. */
            PT_NOTE         = 4,                        /**< Auxiliary information. */
            PT_SHLIB        = 5,                        /**< Reserved w/unspecified semantics. Such a file is nonconforming. */
            PT_PHDR         = 6,                        /**< Segment contains the segment table itself (program header array) */
            PT_TLS          = 7,                        /**< Thread local storage. */

            // OS- and Processor-specific ranges
            PT_LOOS         = 0x60000000,               /**< Values reserved for OS-specific semantics */
            PT_HIOS         = 0x6fffffff,
            PT_LOPROC       = 0x70000000,               /**< Values reserved for processor-specific semantics */
            PT_HIPROC       = 0x7fffffff,

            // OS-specific values for GNU/Linux
            PT_GNU_EH_FRAME = 0x6474e550,               /**< GCC .eh_frame_hdr segment */
            PT_GNU_STACK    = 0x6474e551,               /**< Indicates stack executability */
            PT_GNU_RELRO    = 0x6474e552,               /**< Read-only after relocation */
            PT_PAX_FLAGS    = 0x65041580,               /**< Indicates PaX flag markings */

            // OS-specific values for Sun
            PT_SUNWBSS      = 0x6ffffffa,               /**< Sun Specific segment */
            PT_SUNWSTACK    = 0x6ffffffb                /**< Stack segment */
        };

        /** Segment bit flags */
        enum SegmentFlags {
            PF_NONE         = 0,                        /**< Initial value in c'tor */
            PF_RESERVED     = 0x000ffff8,               /**< Reserved bits */
            PF_XPERM        = 0x00000001,               /**< Execute permission */
            PF_WPERM        = 0x00000002,               /**< Write permission */
            PF_RPERM        = 0x00000004,               /**< Read permission */
            PF_OS_MASK      = 0x0ff00000,               /**< os-specific bits */
            PF_PROC_MASK    = 0xf0000000                /**< Processor-specific bits */
        };

#ifdef _MSC_VER
# pragma pack (1)
#endif
        /** File format of an ELF Segment header.
         *
         * Byte order of members depends on e_ident value in file header. This code comes directly from "Executable and
         * Linkable Format (ELF)", Portable Formats Specification, Version 1.1, Tool Interface Standards (TIS) and not from any
         * header file. The 64-bit structure is gleaned from the Linux elf(5) man page. Segment table entries (a.k.a., ELF
         * program headers) either describe process segments or give supplementary info which does not contribute to the
         * process image. */
        struct Elf32SegmentTableEntry_disk {
            uint32_t        p_type;                  /**< 0x00 kind of segment */
            uint32_t        p_offset;                /**< 0x04 file offset */
            uint32_t        p_vaddr;                 /**< 0x08 desired mapped address of segment */
            uint32_t        p_paddr;                 /**< 0x0c physical address where supported (unused by System V) */
            uint32_t        p_filesz;                /**< 0x20 bytes in file (may be zero or other value smaller than p_memsz) */
            uint32_t        p_memsz;                 /**< 0x24 number of bytes when mapped (may be zero) */
            uint32_t        p_flags;                 /**< 0x28 */
            uint32_t        p_align;                 /**< 0x2c alignment for file and memory (0,1=>none); power of two */
        }                                            /* 0x30 */
#if !defined(SWIG) && !defined(_MSC_VER)
        __attribute__((packed))
#endif
        ;

        struct Elf64SegmentTableEntry_disk {
            uint32_t        p_type;         /* 0x00 */
            uint32_t        p_flags;        /* 0x04 */
            uint64_t        p_offset;       /* 0x08 */
            uint64_t        p_vaddr;        /* 0x10 */
            uint64_t        p_paddr;        /* 0x18 */
            uint64_t        p_filesz;       /* 0x20 */
            uint64_t        p_memsz;        /* 0x28 */
            uint64_t        p_align;        /* 0x30 */
        }                                       /* 0x38 */
#if !defined(SWIG) && !defined(_MSC_VER)
        __attribute__((packed))
#endif
        ;
#ifdef _MSC_VER
# pragma pack ()
#endif

        /** Construct node from 32-bit file data. */
        SgAsmElfSegmentTableEntry(ByteOrder::Endianness sex,
                                  const SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk *disk);

        /** Construct node from 64-bit file data. */
        SgAsmElfSegmentTableEntry(ByteOrder::Endianness sex,
                                  const SgAsmElfSegmentTableEntry::Elf64SegmentTableEntry_disk *disk);

        /** Converts segment table entry back into disk structure.
         *
         * @{ */
        void *encode(ByteOrder::Endianness, SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk*) const;
        void *encode(ByteOrder::Endianness, SgAsmElfSegmentTableEntry::Elf64SegmentTableEntry_disk*) const;
        /** @} */

        /** Update this segment table entry with newer information from the section */
        void update_from_section(SgAsmElfSection*);

        /** Print some debugging info */
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const;

        /** Convert segment type to string. */
        static std::string to_string(SgAsmElfSegmentTableEntry::SegmentType);

        /** Convert segment flags to string. */
        static std::string to_string(SgAsmElfSegmentTableEntry::SegmentFlags);

    private:
        void ctor(ByteOrder::Endianness, const SgAsmElfSegmentTableEntry::Elf32SegmentTableEntry_disk*);
        void ctor(ByteOrder::Endianness, const SgAsmElfSegmentTableEntry::Elf64SegmentTableEntry_disk*);
#endif // SgAsmElfSegmentTableEntry_OTHERS

#ifdef DOCUMENTATION
    };
#endif



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*************************************************************************************************************************
     *                                         ELF Symbol Tables
     *************************************************************************************************************************/

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfSymbolSection);
    IS_SERIALIZABLE(AsmElfSymbolSection);

#ifdef DOCUMENTATION
    /** ELF file section containing symbols. */
    class SgAsmElfSymbolSection: public SgAsmElfSection {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Whether this section represents dynamic linking symbols.
         *
         * @{ */
        bool get_is_dynamic() const;
        void set_is_dynamic(bool);
        /** @} */
#else
        AsmElfSymbolSection.setDataPrototype("bool", "is_dynamic", "= false",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Symbols.
         *
         *  List of symbols contained in this symbol table.  The acual list is stored in a separate AST instead of being stored
         *  directly in this node due to limitations of ROSETTA.
         *
         * @{ */
        SgAsmElfSymbolList* get_symbols() const;
        void set_symbols(SgAsmElfSymbolList*);
        /** @} */
#else
        AsmElfSymbolSection.setDataPrototype("SgAsmElfSymbolList*", "symbols", "= NULL",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmElfSymbolSection);
#if defined(SgAsmElfSymbolSection_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmElfSection);
            s & BOOST_SERIALIZATION_NVP(p_is_dynamic);
            s & BOOST_SERIALIZATION_NVP(p_symbols);
        }
#endif

    public:
        /** Non-parsing constructor */
        SgAsmElfSymbolSection(SgAsmElfFileHeader *fhdr, SgAsmElfStringSection *strsec)
            : SgAsmElfSection(fhdr), p_is_dynamic(false) {
            ctor(strsec);
        }

        /** Initialize by parsing a file. */
        virtual SgAsmElfSymbolSection* parse() $ROSE_OVERRIDE;

        /** Update section pointers for locally-bound symbols.
         *
         *  Now that the section table has been read and all non-synthesized sections have been created, we can update
         *  pointers to other things.
         *
         *  The st_shndx is the index (ID) of the section to which the symbol is bound. Special values are:
         *
         *  @li 0x0000: no section (section table entry zero should be all zeros anyway)
         *  @li 0xff00-0xffff: reserved values, not an index
         *  @li 0xff00-0xff1f: processor specific values
         *  @li 0xfff1: symbol has absolute value not affected by relocation
         *  @li 0xfff2: symbol is fortran common or unallocated C extern */
        virtual void finish_parsing() $ROSE_OVERRIDE;

        /** Given a symbol, return its index in this symbol table. */
        size_t index_of(SgAsmElfSymbol*);

        using SgAsmElfSection::calculate_sizes;
        /** Return sizes for various parts of the table.
         *
         *  See documentation for @ref SgAsmElfSection::calculate_sizes. */
        virtual rose_addr_t calculate_sizes(size_t *total, size_t *required, size_t *optional, size_t *nentries) const $ROSE_OVERRIDE;

        /** Called prior to unparsing.
         *
         *  Updates symbol entries with name offsets. */
        virtual bool reallocate() $ROSE_OVERRIDE;

        /** Write symbol table sections back to disk. */
        virtual void unparse(std::ostream&) const $ROSE_OVERRIDE;

        /** Print some debugging info */
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const $ROSE_OVERRIDE;

    private:
        void ctor(SgAsmElfStringSection*);
#endif // SgAsmElfSymbolSection_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfSymbolList);
    IS_SERIALIZABLE(AsmElfSymbolList);

#ifdef DOCUMENTATION
    class SgAsmElfSymbolList: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Symbol list.
         *
         *  This points to an AST node that contains the actual symbol list. The reason that the list is not held directly in
         *  the nodes that need it is due to ROSETTA limitations.
         *
         * @{ */
        const SgAsmElfSymbolPtrList& get_symbols() const;
        void set_symbols(const SgAsmElfSymbolPtrList&);
        /** @} */
#else
        AsmElfSymbolList.setDataPrototype("SgAsmElfSymbolPtrList", "symbols", "",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmElfSymbolList);
#if defined(SgAsmElfSymbolList_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_symbols);
        }
#endif
#endif // SgAsmElfSymbolList_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfSymbol);
    IS_SERIALIZABLE(AsmElfSymbol);

#ifdef DOCUMENTATION
    /** Represents a single ELF symbol.
     *
     *  Most of the properties of this node correspond directly with properties defined by the ELF specification. Their
     *  documentation is not replicated here -- refer to the specification. */
    class SgAsmElfSymbol: public SgAsmGenericSymbol {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Info.
         *
         *  See ELF specification for details.
         *
         * @{ */
        unsigned char get_st_info() const;
        void set_st_info(unsigned char);
        /** @} */
#else
        AsmElfSymbol.setDataPrototype("unsigned char", "st_info", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Reserved byte.
         *
         *  See ELF specification for details.
         *
         * @{ */
        unsigned char get_st_res1() const;
        void set_st_res1(unsigned char);
        /** @} */
#else
        AsmElfSymbol.setDataPrototype("unsigned char", "st_res1", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: shndx.
         *
         *  See ELF specification for details.
         *
         * @{ */
        unsigned get_st_shndx() const;
        void set_st_shndx(unsigned);
        /** @} */
#else
        AsmElfSymbol.setDataPrototype("unsigned", "st_shndx", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: size.
         *
         *  See ELF specification for details.
         *
         * @{ */
        rose_addr_t get_st_size() const;
        void set_st_size(rose_addr_t);
        /** @} */
#else
        AsmElfSymbol.setDataPrototype("rose_addr_t", "st_size", "= 0",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Extra data.
         *
         *  Bytes that are not part of the symbol but which appear in the table as reserved or padding.
         *
         * @{ */
        const SgUnsignedCharList& get_extra() const;
        void set_extra(const SgUnsignedCharList&);
        /** @} */
#else
        AsmElfSymbol.setDataPrototype("SgUnsignedCharList", "extra", "",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmElfSymbol);
#if defined(SgAsmElfSymbol_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmGenericSymbol);
            s & BOOST_SERIALIZATION_NVP(p_st_info);
            s & BOOST_SERIALIZATION_NVP(p_st_res1);
            s & BOOST_SERIALIZATION_NVP(p_st_shndx);
            s & BOOST_SERIALIZATION_NVP(p_st_size);
            s & BOOST_SERIALIZATION_NVP(p_extra);
        }
#endif

    public:
        enum ElfSymBinding {
            STB_LOCAL=0,
            STB_GLOBAL=1,
            STB_WEAK=2
        };

        enum ElfSymType {
            STT_NOTYPE      = 0,                    /**< Unspecified type */
            STT_OBJECT      = 1,                    /**< Data object */
            STT_FUNC        = 2,                    /**< Code object */
            STT_SECTION     = 3,                    /**< Associated with a section */
            STT_FILE        = 4,                    /**< Name of a file */
            STT_COMMON      = 5,                    /**< Common data object */
            STT_TLS         = 6,                    /**< Thread-local data object */
            STT_IFUNC       = 10                    /**< Indirect function. Function call w/out args results in reloc value. */
        };

#ifdef _MSC_VER
# pragma pack (1)
#endif

        /** 32-bit format of an ELF symbol. */
        struct Elf32SymbolEntry_disk {
            uint32_t      st_name;                      /**< Name offset into string table */
            uint32_t      st_value;                     /**< Value: absolute value, address, etc. depending on sym type */
            uint32_t      st_size;                      /**< Symbol size in bytes */
            unsigned char st_info;                      /**< Type and binding attributes */
            unsigned char st_res1;                      /**< Reserved; always zero */
            uint16_t      st_shndx;                     /**< Section index or special meaning */
        }
#if !defined(SWIG) && !defined(_MSC_VER)
        __attribute__((packed))
#endif
        ;

        struct Elf64SymbolEntry_disk {
            uint32_t      st_name;
            unsigned char st_info;
            unsigned char st_res1;
            uint16_t      st_shndx;
            uint64_t      st_value;
            uint64_t      st_size;
        }
#if !defined(SWIG) && !defined(_MSC_VER)
        __attribute__((packed))
#endif
        ;

#ifdef _MSC_VER
# pragma pack ()
#endif

        /** Constructor that adds the symbol to a symbol table. */
        explicit SgAsmElfSymbol(SgAsmElfSymbolSection *symtab) { ctor(symtab); }

        /** Initialize symbol by parsing a symbol table entry.
         *
         *  An ELF String Section must be supplied in order to get the symbol name. */
        void parse(ByteOrder::Endianness, const SgAsmElfSymbol::Elf32SymbolEntry_disk*);

        /** Initialize symbol by parsing a symbol table entry.
         *
         *  An ELF String Section must be supplied in order to get the symbol name. */
        void parse(ByteOrder::Endianness, const SgAsmElfSymbol::Elf64SymbolEntry_disk*);

        /** Encode a symbol into disk format.
         *
         * @{ */
        void *encode(ByteOrder::Endianness, SgAsmElfSymbol::Elf32SymbolEntry_disk*) const;
        void *encode(ByteOrder::Endianness, SgAsmElfSymbol::Elf64SymbolEntry_disk*) const;
        /** @} */

        /** Print some debugging info.
         *
         *  The 'section' is an optional section pointer for the st_shndx member.
         *
         * @{ */
        virtual void dump(FILE *f, const char *prefix, ssize_t idx) const $ROSE_OVERRIDE;
        void dump(FILE*, const char *prefix, ssize_t idx, SgAsmGenericSection*) const;
        /** @} */

        /** Returns binding as an enum constant. */
        SgAsmElfSymbol::ElfSymBinding get_elf_binding() const;

        /** Returns type as an enum constant. */
        SgAsmElfSymbol::ElfSymType get_elf_type() const;

        /** Converts enum constant to string. */
        static std::string to_string(SgAsmElfSymbol::ElfSymBinding);

        /** Converts enum constant to string. */
        static std::string to_string(SgAsmElfSymbol::ElfSymType);

    private:
        void ctor(SgAsmElfSymbolSection*);
        void parse_common();                            // initialization common to all parse() methods
#endif // SgAsmElfSymbol_OTHERS

#ifdef DOCUMENTATION
    };
#endif



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*************************************************************************************************************************
     *                                         ELF Symbol Version Tables
     *************************************************************************************************************************/

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfSymverSection);
    IS_SERIALIZABLE(AsmElfSymverSection);

#ifdef DOCUMENTATION
    /** The ELF symbol version table.
     *
     *  Often named ".gnu.version section", this section points to an @ref SgAsmElfSymverEntryPtrList containing a list of
     *  SgAsmElfSymverEntry objects. */
    class SgAsmElfSymverSection: public SgAsmElfSection {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: List of table entries.
         *
         *  The list of entries in this section. The only reason the list is stored in a separate AST node instead of being
         *  stored here directly is due to ROSETTA limitations.
         *
         * @{ */
        SgAsmElfSymverEntryList* get_entries() const;
        void set_entries(SgAsmElfSymverEntryList*);
        /** @} */
#else
        AsmElfSymverSection.setDataPrototype("SgAsmElfSymverEntryList*", "entries", "= NULL",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmElfSymverSection);
#if defined(SgAsmElfSymverSection_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmElfSection);
            s & BOOST_SERIALIZATION_NVP(p_entries);
        }
#endif

    public:
        /** Construct section and link it into the AST. */
        explicit SgAsmElfSymverSection(SgAsmElfFileHeader *fhdr)
            : SgAsmElfSection(fhdr) {
            ctor();
        }

        /** Initializes section by parsing the file. */
        virtual SgAsmElfSymverSection* parse() $ROSE_OVERRIDE;

        using SgAsmElfSection::calculate_sizes;
        /** Return sizes for various parts of the table.
         *
         *  See documentation for @ref SgAsmElfSection::calculate_sizes. */
        virtual rose_addr_t calculate_sizes(size_t *total, size_t *required, size_t *optional, size_t *nentries) const $ROSE_OVERRIDE;

        /** Write symver table sections back to disk */
        virtual void unparse(std::ostream&) const $ROSE_OVERRIDE;

        /** Print some debugging info */
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const $ROSE_OVERRIDE;
    private:
        void ctor();
#endif // SgAsmElfSymverSection_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfSymverEntry);
    IS_SERIALIZABLE(AsmElfSymverEntry);

#ifdef DOCUMENTATION
    /** Entry in an ELF symbol version table. */
    class SgAsmElfSymverEntry: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Value.
         *
         *  Numeric value of this symbol.
         *
         * @{ */
        size_t get_value() const;
        void set_value(size_t);
        /** @} */
#else
        AsmElfSymverEntry.setDataPrototype("size_t", "value", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmElfSymverEntry);
#if defined(SgAsmElfSymverEntry_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_value);
        }
#endif

    public:
        /** Constructor that links new entry into the AST. */
        explicit SgAsmElfSymverEntry(SgAsmElfSymverSection *symver)
            : p_value(0) {
            ctor(symver);
        }

        /** Prints some debugging info. */
        virtual void dump(FILE *f, const char *prefix, ssize_t idx) const;

    private:
        void ctor(SgAsmElfSymverSection*);
#endif // SgAsmElfSymverEntry_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    DECLARE_LEAF_CLASS(AsmElfSymverEntryList);
    IS_SERIALIZABLE(AsmElfSymverEntryList);

#ifdef DOCUMENTATION
    /** List of entries from a symbol version table.
     *
     *  The only reason this node exists instead of storing the entries directly in the parent node that needs them is due to
     *  ROSETTA limitations. */
    class SgAsmElfSymverEntryList: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: List of symbol version table entries.
         *
         * @{ */
        const SgAsmElfSymverEntryPtrList& get_entries() const;
        void set_entries(const SgAsmElfSymverEntryPtrList&);
        /** @} */
#else
        AsmElfSymverEntryList.setDataPrototype("SgAsmElfSymverEntryPtrList", "entries", "",
                                               NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL,
                                               NO_DELETE);
#endif

        DECLARE_OTHERS(AsmElfSymverEntryList);
#if defined(SgAsmElfSymverEntryList_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_entries);
        }
#endif
#endif // SgAsmElfSymverEntryList_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfSymverDefinedSection);
    IS_SERIALIZABLE(AsmElfSymverDefinedSection);

#ifdef DOCUMENTATION
    /** The GNU symbol version definitions.
     *
     *  This section is usually named ".gnu.version_d".  The @ref SgAsmElfSymverDefinedSection points to a list of
     * entries (@ref SgAsmSymverDefinedEntry), which in turn point to a list of auxilliary members (@ref
     * SgAsmSymverDefinedAux). See @ref SgAsmSymverDefinedSection::parse for a good description of the disk format. */
    class SgAsmElfSymverDefinedSection: public SgAsmElfSection {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: List of entries.
         *
         *  This is a pointer to an AST node which contains the list. The reason the list isn't stored here directly is due to
         *  limitations of ROSETTA.
         *
         * @{ */
        SgAsmElfSymverDefinedEntryList* get_entries() const;
        void set_entries(SgAsmElfSymverDefinedEntryList*);
        /** @} */
#else
        AsmElfSymverDefinedSection.setDataPrototype("SgAsmElfSymverDefinedEntryList*", "entries", "= NULL",
                                                    NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL,
                                                    NO_DELETE);
#endif

        DECLARE_OTHERS(AsmElfSymverDefinedSection);
#if defined(SgAsmElfSymverDefinedSection_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmElfSection);
            s & BOOST_SERIALIZATION_NVP(p_entries);
        }
#endif

    public:
        /** Constructor linking object into AST. */
        SgAsmElfSymverDefinedSection(SgAsmElfFileHeader *fhdr, SgAsmElfStringSection *strsec)
            : SgAsmElfSection(fhdr) {
            ctor(strsec);
        }

        /** Initializes this ELF SymverDefined Section by parsing a file.
         *
         *  The structure is nominally the following (where n is from DT_VERDEFNUM - 1 in .dynamic)
         *
         *  @code
         *   [0]ElfSymverDefinedEntry_disk
         *        vd_next------------------------------------+
         *        vd_aux ---------------------------------+  |
         *        vd_cnt                                  |  |
         *      [0]       ElfSymverDefinedAux_disk <------+  |
         *                  vda_next -----------------+      |
         *      [1]       ElfSymverDefinedAux_disk <--+      |
         *         ...                                       |
         *      [vd_cnt-1]ElfSymverDefinedAux_disk           |
         *                  vda_next = 0 <== null term       |
         *   [1]ElfSymverDefinedEntry_disk <-----------------+
         *      ...
         *   [n]ElfSymverDefinedEntry_disk
         *        vd_next = 0
         *        vd_aux
         *        vd_cnt
         *      [0]       ElfSymverDefinedAux_disk
         *      [1]       ElfSymverDefinedAux_disk
         *         ...
         *      [vd_cnt-1]ElfSymverDefinedAux_disk
         *  @endcode
         *
         *  However, the spec doesn't specify any actual relationship to the layout of anything...so it could just as easily
         *  be:
         *
         *  @code
         *    [0]ElfSymverDefinedEntry_disk ---+---+
         *    [1]ElfSymverDefinedEntry_disk <--+   |
         *    ...                                  |
         *    [n]ElfSymverDefinedEntry_disk -------|---+
         *                                         |   |
         *    [0]ElfSymverDefinedAux_disk   <------+   |
         *    ...                                      |
         *    [x]ElfSymverDefinedAux_disk   <----------+
         *    [.]ElfSymverDefinedAux_disk
         *  @endcode
         *
         *  There is also nothing in particular that says Aux entries need to be next to each other.  So, the code handles the
         *  most rigidly compliant case, which is to use only the offsets and make no assumptions about layouts.
         *
         *  Also note the number of entries is specified in two ways -- via null termination on the "linked list", as well as
         *  the number from the .dynamic section [DT_VERDEFNUM].  For now, we'll support the null terminator, restricted by
         *  ensuring we don't exceed the size of the section (to keep from running away on a bad file).
         *
         *  We have a similar problem with the number of Aux's per Entry (vd_cnt versus vda_aux=0). However, in this case, we
         *  respect the min of the two (i.e. we assume cnt is right, but if vda_aux is zero earlier than expected, we stop).
         *  This is necessary because the spec allows two or more entries to point into (possibly different places) of a shared
         *  aux array.  This parser creates a new @ref SgAsmElfSymverDefinedAux object every time an element of the aux array
         *  is read from disk, ensuring that each @ref SgAsmElfSymverDefinedEntry points to its own copies.
         *
         *  All offsets are relative to the start of the struct they were specified in. I.e.,
         *
         *  @code
         *    Entry* entry=(0x0100);
         *    Aux* firstAux=(0x100 + entry->vd_aux)
         *    Aux* secondAux=(0x100 + entry->vd_aux + firstAux->vda_next)
         *
         *    Entry* secondEntry=(0x0100 + entry->vd_next);
         *  @endcode
         *
         *  Because of this rather complex structure, the section itself (@ref SgAsmElfSymverDefinedSection) manages all of the
         *  data related to structure (vd_next,vd_aux,vd_cnt, vda_next) -- the subclasses can simply ignore all of that.  The
         *  section also takes care of creating both *Entries and *Auxes and tying them together correctly. */
        virtual SgAsmElfSymverDefinedSection* parse() $ROSE_OVERRIDE;

        using SgAsmElfSection::calculate_sizes;
        /** Return sizes for various parts of the table. See doc for SgAsmElfSection::calculate_sizes. */
        virtual rose_addr_t calculate_sizes(size_t *total, size_t *required, size_t *optional, size_t *nentries) const $ROSE_OVERRIDE;

        /** Write SymverDefined section back to disk.
         *
         *  For more information about encoding, see @ref parse. */
        virtual void unparse(std::ostream&) const $ROSE_OVERRIDE;

        /** Print some debugging info. */
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const $ROSE_OVERRIDE;

    private:
        void ctor(SgAsmElfStringSection*);
#endif // SgAsmElfSymverDefinedSection_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfSymverDefinedEntryList);
    IS_SERIALIZABLE(AsmElfSymverDefinedEntryList);

#ifdef DOCUMENTATION
    /** List of entries for the ELF symbol version definition table.
     *
     *  The only reason we have a dedicated AST node type for this information instead of storing it directly in the nodes that
     *  need it is due to limitations of ROSETTA. */
    class SgAsmElfSymverDefinedEntryList: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: List of table entries.
         *
         * @{ */
        const SgAsmElfSymverDefinedEntryPtrList& get_entries() const;
        void set_entries(const SgAsmElfSymverDefinedPtrList&);
        /** @} */
#else
        AsmElfSymverDefinedEntryList.setDataPrototype("SgAsmElfSymverDefinedEntryPtrList", "entries", "",
                                                      NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL,
                                                      NO_DELETE);
#endif

        DECLARE_OTHERS(AsmElfSymverDefinedEntryList);
#if defined(SgAsmElfSymverDefinedEntryList_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_entries);
        }
#endif
#endif // SgAsmElfSymverDefinedEntryList_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfSymverDefinedEntry);
    IS_SERIALIZABLE(AsmElfSymverDefinedEntry);

#ifdef DOCUMENTATION
    /** One entry from an ELF symbol version definition table. */
    class SgAsmElfSymverDefinedEntry: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Version.
         *
         *  See ELF specification for details.
         *
         * @{ */
        size_t get_version() const;
        void set_version(size_t);
        /** @} */
#else
        AsmElfSymverDefinedEntry.setDataPrototype("size_t", "version", "= 0",
                                                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Flags.
         *
         *  See ELF specification for details.
         *
         * @{ */
        int get_flags() const;
        void set_flags(int);
        /** @} */
#else
        AsmElfSymverDefinedEntry.setDataPrototype("int", "flags", "= 0",
                                                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Index.
         *
         *  See ELF specification for details.
         *
         * @{ */
        size_t get_index() const;
        void set_index(size_t);
        /** @} */
#else
        AsmElfSymverDefinedEntry.setDataPrototype("size_t", "index", "= 0",
                                                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Hash.
         *
         *  See ELF specification for details.
         *
         * @{ */
        uint32_t get_hash() const;
        void set_hash(uint32_t);
        /** @} */
#else
        AsmElfSymverDefinedEntry.setDataPrototype("uint32_t", "hash", "= 0",
                                                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Entries.
         *
         *  List of auxilliary entries for this version definition.  Rather than storing the list here directly, we point to a
         *  node whose only purpose is to hold the list. This is due to limitations of ROSETTA.
         *
         * @{ */
        SgAsmElfSymverDefinedAuxList* get_entries() const;
        void set_entries(SgAsmElfSymverDefinedAuxList*);
        /** @} */
#else
        AsmElfSymverDefinedEntry.setDataPrototype("SgAsmElfSymverDefinedAuxList*", "entries", "",
                                                  NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmElfSymverDefinedEntry);
#if defined(SgAsmElfSymverDefinedEntry_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_version);
            s & BOOST_SERIALIZATION_NVP(p_flags);
            s & BOOST_SERIALIZATION_NVP(p_index);
            s & BOOST_SERIALIZATION_NVP(p_hash);
            s & BOOST_SERIALIZATION_NVP(p_entries);
        }
#endif

        public:
#ifdef _MSC_VER
# pragma pack (1)
#endif
        /** Disk format. 32- and 64-bit formats are both the same. */
        struct ElfSymverDefinedEntry_disk {
            uint16_t      vd_version;                   /**< version of this struct: This field shall be set to 1 */
            uint16_t      vd_flags;                     /**< Version information flag bitmask */
            uint16_t      vd_ndx;                       /**< Version index of this entry */
            uint16_t      vd_cnt;                       /**< Number of verdaux entries @see SgAsmElfSymverDefinedAux */
            uint32_t      vd_hash;                      /**< Hash of version name */
            uint32_t      vd_aux;                       /**< Offset (in bytes) to start of array of verdaux entries */
            uint32_t      vd_next;                      /**< Offset (in bytes) to next verdef entry */
        }
#if !defined(SWIG) && !defined(_MSC_VER)
        __attribute__((packed))
#endif
        ;

#ifdef _MSC_VER
# pragma pack ()
#endif

        /** Constructor linking ojbec into the AST. */
        explicit SgAsmElfSymverDefinedEntry(SgAsmElfSymverDefinedSection *symver_defined) {
            ctor(symver_defined);
        }

        /** Initialize by parsing information from the file. */
        void parse(ByteOrder::Endianness, const SgAsmElfSymverDefinedEntry::ElfSymverDefinedEntry_disk*);

        /** Convert to the disk format. */
        void *encode(ByteOrder::Endianness, SgAsmElfSymverDefinedEntry::ElfSymverDefinedEntry_disk*) const;

        /** Print some debugging info. */
        virtual void dump(FILE *f, const char *prefix, ssize_t idx) const;

    private:
        void ctor(SgAsmElfSymverDefinedSection*);
#endif // SgAsmElfSymverDefinedEntry_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfSymverDefinedAuxList);
    IS_SERIALIZABLE(AsmElfSymverDefinedAuxList);

#ifdef DOCUMENTATION
    /** List of symbol version aux entries.
     *
     *  The only purpose of this node is to hold the list of pointers, which can't be contained in the classes that need the
     *  list due to limitations of ROSETTA. */
    class SgAsmElfSymverDefinedAuxList: public SgAsmExecutableFileFormat {
    pbulic:
#endif

#ifdef DOCUMENTATION
        /** Property: List of entries.
         *
         * @{ */
        const SgAsmElfSymverDefinedAuxPtrList& get_entries() const;
        void set_entries(const SgAsmElfSymverDefinedAuxPtrList&);
        /** @} */
#else
        AsmElfSymverDefinedAuxList.setDataPrototype("SgAsmElfSymverDefinedAuxPtrList", "entries", "",
                                                    NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL,
                                                    NO_DELETE);
#endif

        DECLARE_OTHERS(AsmElfSymverDefinedAuxList);
#if defined(SgAsmElfSymverDefinedAuxList_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_entries);
        }
#endif
#endif // SgAsmElfSymverDefinedAuxList_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfSymverDefinedAux);
    IS_SERIALIZABLE(AsmElfSymverDefinedAux);

#ifdef DOCUMENTATION
    class SgAsmAsmElfSymverDefinedAux: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Name.
         *
         * @{ */
        SgAsmGenericString* get_name() const;
        void set_name(SgAsmGenericString*);
        /** @} */
#else
        AsmElfSymverDefinedAux.setDataPrototype("SgAsmGenericString*", "name", "= 0",
                                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmElfSymverDefinedAux);
#if defined(SgAsmElfSymverDefinedAux_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_name);
        }
#endif

    public:
#ifdef _MSC_VER
# pragma pack (1)
#endif

        /** Disk format. The format is the same for 32bit and 64bit. */
        struct ElfSymverDefinedAux_disk {
            uint32_t      vda_name;                     /**< Offset (in bytes) to strings table to name string */
            uint32_t      vda_next;                     /**< Offset (in bytes) to next verdaux entry */
        }
#if !defined(SWIG) && !defined(_MSC_VER)
        __attribute__((packed))
#endif
        ;

#ifdef _MSC_VER
# pragma pack ()
#endif

        /** Constructor linking object into AST.
         *
         *  This constructor links this new Aux into the specified Entry of the specified Symbol Version Definition Table. */
        explicit SgAsmElfSymverDefinedAux(SgAsmElfSymverDefinedEntry *symver_def_entry,
                                          SgAsmElfSymverDefinedSection *symver_def_sec)
            : p_name(NULL) {
            ctor(symver_def_entry,symver_def_sec);
        }

        /** Initialize this object with data parsed from a file. */
        void parse(ByteOrder::Endianness, const SgAsmElfSymverDefinedAux::ElfSymverDefinedAux_disk*);

        /** Convert this object into the disk format record to be written back to the Symbol Version Definition Table. */
        void *encode(ByteOrder::Endianness, SgAsmElfSymverDefinedAux::ElfSymverDefinedAux_disk*) const;

        /** Print debugging information.
         *
         *  Shows information about the specified auxiliary data for an entry in the Symbol Version Definition Table. Note that
         *  in order to have a more compact output, @ref SgAsmElfSymverDefinedEntry::dump prints the @ref
         *  SgAsmElfSymverDefinedAux objects explicitly rather than calling this method. */
        virtual void dump(FILE *f, const char *prefix, ssize_t idx) const;

    private:
        void ctor(SgAsmElfSymverDefinedEntry*,SgAsmElfSymverDefinedSection *symver_def_sec);
#endif // SgAsmElfSymverDefinedAux_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfSymverNeededSection);
    IS_SERIALIZABLE(AsmElfSymverNeededSection);

#ifdef DOCUMENTATION
    /** GNU symbol version requirements table.
     *
     *  This section is usually named ".gnu.version_r".  The format of this table is similar to the GNU Symbol Version
     *  Definitions Table, namey that the table object (@ref SgAsmElfSymverNeededSection) points to a list of entries (@ref
     *  SgAsmElfSymverNeededEntry), which of which point to a list of auxilliary information (@ref SgAsmElfSymverNeededAux). */
    class SgAsmElfSymverNeededSection: public SgAsmElfSection {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: List of entries.
         *
         *  This property points to an AST node whose only purpose is to hold the list of entries. The only reason the entries
         *  are not contained directly in this node where they're needed is due to ROSETTA limitations.
         *
         * @{ */
        SgAsmElfSymverNeededEntryList* get_entries() const;
        void set_entries(SgAsmElfSymverNeededEntryList*);
        /** @} */
#else
        AsmElfSymverNeededSection.setDataPrototype("SgAsmElfSymverNeededEntryList*", "entries", "= NULL",
                                                   NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmElfSymverNeededSection);
#if defined(SgAsmElfSymverNeededSection_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmElfSection);
            s & BOOST_SERIALIZATION_NVP(p_entries);
        }
#endif

    public:
        /** Constructor that links objec into AST.
         *
         *  This constructor adds this entry to the specified GNU Symbol Version Requirements Table. */
        SgAsmElfSymverNeededSection(SgAsmElfFileHeader *fhdr, SgAsmElfStringSection *strsec)
            : SgAsmElfSection(fhdr) {
            ctor(strsec);
        }

        /** Parse a GNU Symbol Version Requirements Table.
         *
         *  The layout of this table is very similar to the layout of the GNU Symbol Version Definition Table and users should
         *  refer to @ref SgAsmElfSymverDefinedSection::parse for details.  Different data structures are used between the
         *  Definition and Requirements tables:
         *
         *  @li @ref SgAsmElfSymverNeededSection corresponds to SgAsmElfSymverDefinedSection.
         *  @li @ref SgAsmElfSymverNeededEntry corresponds to SgAsmElfSymverDefinedEntry.
         *  @li @ref SgAsmElfSymverNeededAux corresponds to SgAsmElfSymverDefinedAux. */
        virtual SgAsmElfSymverNeededSection* parse() $ROSE_OVERRIDE;

        using SgAsmElfSection::calculate_sizes;
        /** Return sizes for various parts of the table.
         *
         *  See documentation for @ref SgAsmElfSection::calculate_sizes. */
        virtual rose_addr_t calculate_sizes(size_t *total, size_t *required, size_t *optional, size_t *nentries) const $ROSE_OVERRIDE;

        /** Write SymverNeeded section back to disk.
         *
         *  For more information about encoding, see @ref SgAsmElfSymverNeededSection::parse. */
        virtual void unparse(std::ostream&) const $ROSE_OVERRIDE;

        /** Print some debugging info */
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const $ROSE_OVERRIDE;

    private:
        void ctor(SgAsmElfStringSection*);
#endif // SgAsmElfSymverNeededSection_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfSymverNeededEntryList);
    IS_SERIALIZABLE(AsmElfSymverNeededEntryList);

#ifdef DOCUMENTATION
    /** List of symbol version needed entries.
     *
     *  The only reason this node type exists is to hold the list of pointers. The list cannot be contained in the nodes that
     *  actually need it due to limitations of ROSETTA. */
    class SgAsmElfSymverNeededEntryList: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: List of entries.
         *
         * @{ */
        const SgAsmElfSymverNeededEntryPtrList& get_entries() void;
        void set_entries(const SgAsmElfSymverNeededEntryPtrList&);
        /** @} */
#else
        AsmElfSymverNeededEntryList.setDataPrototype("SgAsmElfSymverNeededEntryPtrList", "entries", "",
                                                     NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL,
                                                     NO_DELETE);
#endif

        DECLARE_OTHERS(AsmElfSymverNeededEntryList);
#if defined(SgAsmElfSymverNeededEntryList_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_entries);
        }
#endif
#endif // SgAsmElfSymverNeededEntryList_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfSymverNeededEntry);
    IS_SERIALIZABLE(AsmElfSymverNeededEntry);

#ifdef DOCUMENTATION
    /** One entry of the ELF symbol version needed table. */
    class SgAsmElfSymverNeededEntry: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Version.
         *
         *  See ELF specification for details.
         *
         * @{ */
        size_t get_version() const;
        void set_version(size_t);
        /** @} */
#else
        AsmElfSymverNeededEntry.setDataPrototype("size_t", "version", "= 0",
                                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: File name.
         *
         * @{ */
        SgAsmGenericString* get_file_name() const;
        void set_file_name(SgAsmGenericString*);
        /** @} */
#else
        AsmElfSymverNeededEntry.setDataPrototype("SgAsmGenericString*", "file_name", "= 0",
                                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: List of entries.
         *
         *  This is a pointer to an AST node whose only purpose is to hold the list. The reason the list cannot be contained
         *  directly in this node where it's needed is due to ROSETTA limitations.
         *
         * @{ */
        SgAsmElfSymverNeededAuxList* get_entries() const;
        void set_entries(SgAsmElfSymverNeededAuxList*);
        /** @} */
#else
        AsmElfSymverNeededEntry.setDataPrototype("SgAsmElfSymverNeededAuxList*", "entries", "",
                                                 NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmElfSymverNeededEntry);
#if defined(SgAsmElfSymverNeededEntry_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_version);
            s & BOOST_SERIALIZATION_NVP(p_file_name);
            s & BOOST_SERIALIZATION_NVP(p_entries);
        }
#endif

    public:
#ifdef _MSC_VER
# pragma pack (1)
#endif

        /** Disk format. Same for 32bit and 64bit. */
        struct ElfSymverNeededEntry_disk {
            uint16_t      vn_version;                   /**< version of this struct: This field shall be set to 1 */
            uint16_t      vn_cnt;                       /**< Number of vernaux entries @see SgAsmElfSymverNeededAux */
            uint32_t      vn_file;                      /**< Offset (in bytes) to strings table to file string */
            uint32_t      vn_aux;                       /**< Offset (in bytes) to start of array of vernaux entries */
            uint32_t      vn_next;                      /**< Offset (in bytes) to next verneed entry */
        }
#if !defined(SWIG) && !defined(_MSC_VER)
        __attribute__((packed))
#endif
        ;

#ifdef _MSC_VER
# pragma pack ()
#endif

        /** Constructor that links object into the AST. */
        explicit SgAsmElfSymverNeededEntry(SgAsmElfSymverNeededSection *symver_needed)
            : p_file_name(NULL) {
            ctor(symver_needed);
        }

        /** Initialize object by parsing file. */
        void parse(ByteOrder::Endianness, const SgAsmElfSymverNeededEntry::ElfSymverNeededEntry_disk*);

        /** Encode object to disk representation. */
        void *encode(ByteOrder::Endianness, SgAsmElfSymverNeededEntry::ElfSymverNeededEntry_disk*) const;

        /** Print debugging information. */
        virtual void dump(FILE *f, const char *prefix, ssize_t idx) const;

    private:
        void ctor(SgAsmElfSymverNeededSection*);
#endif // SgAsmElfSymverNeededEntry_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfSymverNeededAuxList);
    IS_SERIALIZABLE(AsmElfSymverNeededAuxList);

#ifdef DOCUMENTATION
    /** Hods a list of symbol version aux entries.
     *
     *  The only purpose of this node is to work around a limitation of ROSETTA that prevents this list from being contained
     *  directly in the class that needs it. */
    class SgAsmElfSymverNeededAuxList: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: List of entries.
         *
         * @{ */
        const SgAsmElfSymverNeededAuxPtrList& get_entries() const;
        void set_entries(const SgAsmElfSymverNeededAuxPtrList&);
        /** @} */
#else
        AsmElfSymverNeededAuxList.setDataPrototype("SgAsmElfSymverNeededAuxPtrList", "entries", "",
                                                   NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL,
                                                   NO_DELETE);
#endif

        DECLARE_OTHERS(AsmElfSymverNeededAuxList);
#if defined(SgAsmElfSymverNeededAuxList_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_entries);
        }
#endif
#endif // SgAsmElfSymverNeededAuxList_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfSymverNeededAux);
    IS_SERIALIZABLE(AsmElfSymverNeededAux);

#ifdef DOCUMENTATION
    /** Auxiliary info for needed symbol version. */
    class SgAsmElfSymverNeededAux: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Hash.
         *
         *  See ELF specification for details.
         *
         * @{ */
        uint32_t get_hash() const;
        void set_hash(uint32_t);
        /** @} */
#else
        AsmElfSymverNeededAux.setDataPrototype("uint32_t", "hash", "= 0",
                                               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Flags.
         *
         *  See ELF specification for details.
         *
         * @{ */
        int get_flags() const;
        void set_flags(int);
        /** @} */
#else
        AsmElfSymverNeededAux.setDataPrototype("int", "flags", "= 0",
                                               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Other.
         *
         *  See ELF specification for details.
         *
         * @{ */
        size_t get_other() const;
        void set_other(size_t);
        /** @} */
#else
        AsmElfSymverNeededAux.setDataPrototype("size_t", "other", "= 0",
                                               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Name.
         *
         * @{ */
        SgAsmGenericString* get_name() const;
        void set_name(SgAsmGenericString*);
        /** @} */
#else
        AsmElfSymverNeededAux.setDataPrototype("SgAsmGenericString*", "name", "= 0",
                                               NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmElfSymverNeededAux);
#if defined(SgAsmElfSymverNeededAux_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_hash);
            s & BOOST_SERIALIZATION_NVP(p_flags);
            s & BOOST_SERIALIZATION_NVP(p_other);
            s & BOOST_SERIALIZATION_NVP(p_name);
        }
#endif

    public:
#ifdef _MSC_VER
# pragma pack (1)
#endif

        /** Disk format. Same for 32bit and 64bit. */
        struct ElfSymverNeededAux_disk {
            uint32_t      vna_hash;                     /**< Hash of version name */
            uint16_t      vna_flags;                    /**< Version information flag bitmask */
            uint16_t      vna_other;                    /**< Version index of this entry (bit 15 is special) */
            uint32_t      vna_name;                     /**< Offset (in bytes) to strings table to name string */
            uint32_t      vna_next;                     /**< Offset (in bytes) to next vernaux entry */
        }
#if !defined(SWIG) && !defined(_MSC_VER)
        __attribute__((packed))
#endif
        ;

#ifdef _MSC_VER
# pragma pack ()
#endif

        /** Constructor linking object into AST.
         *
         *  This constructor adds this auxiliary information object to the specified entry of the specified GNU Symbol Version
         *  Requirements Table. */
        SgAsmElfSymverNeededAux(SgAsmElfSymverNeededEntry *symver_needed_entry, SgAsmElfSymverNeededSection *symver_needed_sec)
            : p_name(NULL) {
            ctor(symver_needed_entry,symver_needed_sec);
        }

        /** Initialize this auxiliary record by parsing data from the file. */
        void parse(ByteOrder::Endianness, const SgAsmElfSymverNeededAux::ElfSymverNeededAux_disk*);

        /** Encode this auxiliary record into a format that can be written to a file. */
        void *encode(ByteOrder::Endianness, SgAsmElfSymverNeededAux::ElfSymverNeededAux_disk*) const;

        /** Print debugging information.
         *
         *  Prints debugging information about this auxiliary record of an entry of the GNU Symbol Version Requirements
         *  Table. Note that this method is not normally called since @ref SgAsmElfSymverNeededEntry::dump prints the auxiliary
         *  information explicitly for a more compact listing. */
        virtual void dump(FILE *f, const char *prefix, ssize_t idx) const;

    private:
        void ctor(SgAsmElfSymverNeededEntry*,SgAsmElfSymverNeededSection*);
#endif // SgAsmElfSymverNeededAux_OTHERS

#ifdef DOCUMENTATION
    };
#endif




    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*************************************************************************************************************************
     *                                         ELF Relocation Tables
     *************************************************************************************************************************/

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfRelocSection);
    IS_SERIALIZABLE(AsmElfRelocSection);

#ifdef DOCUMENTATION
    /** Represents an ELF relocation section. */
    class SgAsmElfRelocSection: public SgAsmElfSection {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Whether entries in this section use the addend format.
         *
         * @{ */
        bool get_uses_addend() const;
        void set_uses_addend(bool);
        /** @} */
#else
        AsmElfRelocSection.setDataPrototype("bool", "uses_addend", "= true",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Section targeted by these relocations.
         *
         * @{ */
        SgAsmElfSection* get_target_section() const;
        void set_target_section(SgAsmElfSection*);
        /** @} */
#else
        AsmElfRelocSection.setDataPrototype("SgAsmElfSection*", "target_section", "= NULL",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: List of entries.
         *
         *  This is an AST node whose only purpose is to hold the list. It's done this way due to limitations of ROSETTA.
         *
         * @{ */
        SgAsmElfRelocEntryList* get_entries();
        void set_entries(SgAsmElfRelocEntryList*);
        /** @} */
#else
        AsmElfRelocSection.setDataPrototype("SgAsmElfRelocEntryList*", "entries", "= NULL",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmElfRelocSection);
#if defined(SgAsmElfRelocSection_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmElfSection);
            s & BOOST_SERIALIZATION_NVP(p_uses_addend);
            s & BOOST_SERIALIZATION_NVP(p_target_section);
            s & BOOST_SERIALIZATION_NVP(p_entries);
        }
#endif

    public:
        SgAsmElfRelocSection(SgAsmElfFileHeader *fhdr, SgAsmElfSymbolSection *symsec,SgAsmElfSection* targetsec)
            : SgAsmElfSection(fhdr) {
            ctor(symsec,targetsec);
        }

        using SgAsmElfSection::calculate_sizes;
        /** Parse an existing ELF Rela Section */
        virtual SgAsmElfRelocSection *parse() $ROSE_OVERRIDE;

        /** Return sizes for various parts of the table. See doc for SgAsmElfSection::calculate_sizes. */
        virtual rose_addr_t calculate_sizes(size_t *total, size_t *required, size_t *optional, size_t *entcount) const $ROSE_OVERRIDE;

        /** Pre-unparsing adjustments */
        virtual bool reallocate() $ROSE_OVERRIDE;

        /** Write section back to disk */
        virtual void unparse(std::ostream&) const $ROSE_OVERRIDE;

        /** Print some debugging info */
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const $ROSE_OVERRIDE;

    private:
        void ctor(SgAsmElfSymbolSection*,SgAsmElfSection*);
#endif // SgAsmElfRelocSection_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfRelocEntryList);
    IS_SERIALIZABLE(AsmElfRelocEntryList);

#ifdef DOCUMENTATION
    /** List of ELF relocation entries.
     *
     *  The only purpose of this node is to hold a list of the actual relocation entry nodes since ROSETTA limitations prevent
     *  that list from being contained in the nodes where it's needed. */
    class SgAsmElfRelocEntryList: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: List of relocation entries.
         *
         * @{ */
        const SgAsmElfRelocEntryPtrList& get_entries() const;
        void set_entries(const SgAsmElfRelocEntryPtrList&);
        /** @} */
#else
        AsmElfRelocEntryList.setDataPrototype("SgAsmElfRelocEntryPtrList", "entries", "",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL,
                                              NO_DELETE);
#endif

        DECLARE_OTHERS(AsmElfRelocEntryList);
#if defined(SgAsmElfRelocEntryList_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_entries);
        }
#endif
#endif // SgAsmElfRelocEntryList_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfRelocEntry);
    IS_SERIALIZABLE(AsmElfRelocEntry);

#ifdef DOCUMENTATION
    /** One entry of an ELF relocation table. */
    class SgAsmElfRelocEntry: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Offset.
         *
         *  See ELF specification for details.
         *
         * @{ */
        rose_addr_t get_r_offset() const;
        void set_r_offset(rose_addr_t);
        /** @} */
#else
        AsmElfRelocEntry.setDataPrototype("rose_addr_t", "r_offset", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Addend.
         *
         *  See ELF specification for details.
         *
         * @{ */
        rose_addr_t get_r_addend() const;
        void set_r_addend(rose_addr_t);
        /** @} */
#else
        AsmElfRelocEntry.setDataPrototype("rose_addr_t", "r_addend", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Sym.
         *
         *  See ELF specification for details.
         *
         * @{ */
        unsigned long get_sym() const;
        void set_sym(unsigned long);
        /** @} */
#else
        AsmElfRelocEntry.setDataPrototype("unsigned long", "sym", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Type.
         *
         *  See ELF specification for details.
         *
         * @{ */
        RelocType get_type() const;
        void set_type(RelocType);
        /** @} */
#else
        AsmElfRelocEntry.setDataPrototype("SgAsmElfRelocEntry::RelocType", "type", "= R_386_NONE",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Value of padding bytes.
         *
         * @{ */
        const SgUnsignedCharList& get_extra() const;
        void set_extra(const SgUnsignedCharList&);
        /** @} */
#else
        AsmElfRelocEntry.setDataPrototype("SgUnsignedCharList", "extra", "",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmElfRelocEntry);
#if defined(SgAsmElfRelocEntry_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_r_offset);
            s & BOOST_SERIALIZATION_NVP(p_r_addend);
            s & BOOST_SERIALIZATION_NVP(p_sym);
            s & BOOST_SERIALIZATION_NVP(p_type);
            s & BOOST_SERIALIZATION_NVP(p_extra);
        }
#endif

    public:
        /** Relocation Type. In host order.  All other values are reserved. */
        enum RelocType{
            // Intel 80386 specific definitions.
            R_386_NONE         =0,  /**< No reloc */
            R_386_32           =1,  /**< Direct 32 bit  */
            R_386_PC32         =2,  /**< PC relative 32 bit */
            R_386_GOT32        =3,  /**< 32 bit GOT entry */
            R_386_PLT32        =4,  /**< 32 bit PLT address */
            R_386_COPY         =5,  /**< Copy symbol at runtime */
            R_386_GLOB_DAT     =6,  /**< Create GOT entry */
            R_386_JMP_SLOT     =7,  /**< Create PLT entry */
            R_386_RELATIVE     =8,  /**< Adjust by program base */
            R_386_GOTOFF       =9,  /**< 32 bit offset to GOT */
            R_386_GOTPC        =10, /**< 32 bit PC relative offset to GOT */
            R_386_32PLT        =11,
            R_386_TLS_TPOFF    =14, /**< Offset in static TLS block */
            R_386_TLS_IE       =15, /**< Address of GOT entry for static TLS block offset */
            R_386_TLS_GOTIE    =16, /**< GOT entry for static TLS block offset */
            R_386_TLS_LE       =17, /**< Offset relative to static TLS block */
            R_386_TLS_GD       =18, /**< Direct 32 bit for GNU version of general dynamic thread local data */
            R_386_TLS_LDM      =19, /**< Direct 32 bit for GNU version of local dynamic thread local data in LE code */
            R_386_16           =20,
            R_386_PC16         =21,
            R_386_8            =22,
            R_386_PC8          =23,
            R_386_TLS_GD_32    =24, /**< Direct 32 bit for general dynamic thread local data */
            R_386_TLS_GD_PUSH  =25, /**< Tag for pushl in GD TLS code */
            R_386_TLS_GD_CALL  =26, /**< Relocation for call to __tls_get_addr() */
            R_386_TLS_GD_POP   =27, /**< Tag for popl in GD TLS code */
            R_386_TLS_LDM_32   =28, /**< Direct 32 bit for local dynamic thread local data in LE code */
            R_386_TLS_LDM_PUSH =29, /**< Tag for pushl in LDM TLS code */
            R_386_TLS_LDM_CALL =30, /**< Relocation for call to __tls_get_addr() in LDM code */
            R_386_TLS_LDM_POP  =31, /**< Tag for popl in LDM TLS code */
            R_386_TLS_LDO_32   =32, /**< Offset relative to TLS block */
            R_386_TLS_IE_32    =33, /**< GOT entry for negated static TLS block offset */
            R_386_TLS_LE_32    =34, /**< Negated offset relative to static TLS block */
            R_386_TLS_DTPMOD32 =35, /**< ID of module containing symbol */
            R_386_TLS_DTPOFF32 =36, /**< Offset in TLS block */
            R_386_TLS_TPOFF32  =37, /**< Negated offset in static TLS block */

            // First Entry for X86-64
            R_X86_64_NONE     =100, /**<  No reloc */
            R_X86_64_64       =101, /**<  Direct 64 bit  */
            R_X86_64_PC32     =102, /**<  PC relative 32 bit signed */
            R_X86_64_GOT32    =103, /**<  32 bit GOT entry */
            R_X86_64_PLT32    =104, /**<  32 bit PLT address */
            R_X86_64_COPY     =105, /**<  Copy symbol at runtime */
            R_X86_64_GLOB_DAT =106, /**<  Create GOT entry */
            R_X86_64_JUMP_SLOT=107, /**<  Create PLT entry */
            R_X86_64_RELATIVE =108, /**<  Adjust by program base */
            R_X86_64_GOTPCREL =109, /**<  32 bit signed PC relative offset to GOT */
            R_X86_64_32       =110, /**<  Direct 32 bit zero extended */
            R_X86_64_32S      =111, /**<  Direct 32 bit sign extended */
            R_X86_64_16       =112, /**<  Direct 16 bit zero extended */
            R_X86_64_PC16     =113, /**<  16 bit sign extended pc relative */
            R_X86_64_8        =114, /**<  Direct 8 bit sign extended  */
            R_X86_64_PC8      =115, /**<  8 bit sign extended pc relative */
            R_X86_64_DTPMOD64 =116, /**<  ID of module containing symbol */
            R_X86_64_DTPOFF64 =117, /**<  Offset in module's TLS block */
            R_X86_64_TPOFF64  =118, /**<  Offset in initial TLS block */
            R_X86_64_TLSGD    =119, /**<  32 bit signed PC relative offset to two GOT entries for GD symbol */
            R_X86_64_TLSLD    =120, /**<  32 bit signed PC relative offset to two GOT entries for LD symbol */
            R_X86_64_DTPOFF32 =121, /**<  Offset in TLS block */
            R_X86_64_GOTTPOFF =122, /**<  32 bit signed PC relative offset to GOT entry for IE symbol */
            R_X86_64_TPOFF32  =123  /**<  Offset in initial TLS block */
        };

#ifdef _MSC_VER
# pragma pack (1)
#endif

        struct Elf32RelaEntry_disk {
            uint32_t        r_offset;
            uint32_t        r_info;
            uint32_t        r_addend;
        }
#if !defined(SWIG) && !defined(_MSC_VER)
        __attribute__((packed))
#endif
        ;

        struct Elf64RelaEntry_disk {
            uint64_t        r_offset;
            uint64_t        r_info;
            uint64_t        r_addend;
        }
#if !defined(SWIG) && !defined(_MSC_VER)
        __attribute__((packed))
#endif
        ;

        struct Elf32RelEntry_disk {
            uint32_t        r_offset;
            uint32_t        r_info;
        }
#if !defined(SWIG) && !defined(_MSC_VER)
        __attribute__((packed))
#endif
        ;

        struct Elf64RelEntry_disk {
            uint64_t        r_offset;
            uint64_t        r_info;
        }
#if !defined(SWIG) && !defined(_MSC_VER)
        __attribute__((packed))
#endif
        ;

#ifdef _MSC_VER
# pragma pack ()
#endif

        /** Constructor that adds the new entry to the relocation table. */
        SgAsmElfRelocEntry(SgAsmElfRelocSection *section)
            : p_r_offset(0), p_r_addend(0), p_sym(0), p_type(R_386_NONE) {
            ctor(section);
        }

        /** Initialize object by parsing from file.
         *
         * @{ */
        void parse(ByteOrder::Endianness sex, const SgAsmElfRelocEntry::Elf32RelaEntry_disk *disk);
        void parse(ByteOrder::Endianness sex, const SgAsmElfRelocEntry::Elf64RelaEntry_disk *disk);
        void parse(ByteOrder::Endianness sex, const SgAsmElfRelocEntry::Elf32RelEntry_disk *disk);
        void parse(ByteOrder::Endianness sex, const SgAsmElfRelocEntry::Elf64RelEntry_disk *disk);
        /** @} */

        /** Convert object to on-disk format.
         *
         * @{ */
        void *encode(ByteOrder::Endianness, SgAsmElfRelocEntry::Elf32RelaEntry_disk*) const;
        void *encode(ByteOrder::Endianness, SgAsmElfRelocEntry::Elf64RelaEntry_disk*) const;
        void *encode(ByteOrder::Endianness, SgAsmElfRelocEntry::Elf32RelEntry_disk*) const;
        void *encode(ByteOrder::Endianness, SgAsmElfRelocEntry::Elf64RelEntry_disk*) const;
        /** @} */

        /** Print debugging information.
         *
         * @{ */
        void dump(FILE *f, const char *prefix, ssize_t idx, SgAsmElfSymbolSection *symtab) const;
        virtual void dump(FILE *f, const char *prefix, ssize_t idx) const {
            dump(f, prefix, idx, NULL);
        }
        /** @} */

        /** Convert relocation to string for debugging. */
        std::string reloc_name() const;

    private:
        void ctor(SgAsmElfRelocSection*);
#endif // SgAsmElfRelocEntry_OTHERS

#ifdef DOCUMENTATION
    };
#endif



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*************************************************************************************************************************
     *                                         ELF Dynamic Linking
     *************************************************************************************************************************/

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfDynamicSection);
    IS_SERIALIZABLE(AsmElfDynamicSection);

#ifdef DOCUMENTATION
    /** ELF section containing dynamic linking information. */
    class SgAsmElfDynamicSection: public SgAsmElfSection {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: List of entries.
         *
         *  This points to a node whose only purpose is to hold the list. The list cannot be contained directly here where it's
         *  needed due to limitations of ROSETTA.
         *
         * @{ */
        SgAsmElfDynamicEntryList* get_entries() const;
        void set_entries(SgAsmElfDynamicEntryList*);
        /** @} */
#else
        AsmElfDynamicSection.setDataPrototype("SgAsmElfDynamicEntryList*", "entries", "= NULL",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmElfDynamicSection);
#if defined(SgAsmElfDynamicSection_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmElfSection);
            s & BOOST_SERIALIZATION_NVP(p_entries);
        }
#endif

    public:
        /** Constructor that links object into the AST. */
        SgAsmElfDynamicSection(SgAsmElfFileHeader *fhdr, SgAsmElfStringSection *strsec)
            : SgAsmElfSection(fhdr) {
            ctor(strsec);
        }

        /** Initialize object by parsing file. */
        virtual SgAsmElfDynamicSection* parse() $ROSE_OVERRIDE;

        /** Finish initializing the section entries. */
        virtual void finish_parsing() $ROSE_OVERRIDE;

        using SgAsmElfSection::calculate_sizes;
        /** Return sizes for various parts of the table. See documentation for @ref SgAsmElfSection::calculate_sizes. */
        virtual rose_addr_t calculate_sizes(size_t *total, size_t *required, size_t *optional, size_t *entcount) const $ROSE_OVERRIDE;

        /** Called prior to unparse to make things consistent. */
        virtual bool reallocate() $ROSE_OVERRIDE;

        /** Write the dynamic section back to disk */
        virtual void unparse(std::ostream&) const $ROSE_OVERRIDE;

        /** Print some debugging info */
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const $ROSE_OVERRIDE;

    private:
        void ctor(SgAsmElfStringSection*);
#endif // SgAsmElfDynamicSection_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfDynamicEntryList);
    IS_SERIALIZABLE(AsmElfDynamicEntryList);

#ifdef DOCUMENTATION
    /** List of dynamic linking section entries.
     *
     *  The only purpose of this node is to hold a list which, due to ROSETTA limitations, cannot be contained in the objects
     *  that actually need it. */
    class SgAsmElfDynamicEntryList: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: List of entries.
         *
         * @{ */
        const SgAsmElfDynamicEntryPtrList& get_entries() const;
        void set_entries(const SgAsmElfDynamicEntryPtrList&);
        /** @} */
#else
        AsmElfDynamicEntryList.setDataPrototype("SgAsmElfDynamicEntryPtrList", "entries", "",
                                                NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL,
                                                NO_DELETE);
#endif

        DECLARE_OTHERS(AsmElfDynamicEntryList);
#if defined(SgAsmElfDynamicEntryList_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_entries);
        }
#endif
#endif // SgAsmElfDynamicEntryList_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfDynamicEntry);
    IS_SERIALIZABLE(AsmElfDynamicEntry);

#ifdef DOCUMENTATION
    /** One entry from the dynamic linking table. */
    class SgAsmElfDynamicEntry: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Tag.
         *
         *  See ELF specification for details.
         *
         * @{ */
        EntryType get_d_tag() const;
        void set_d_tag(EntryType);
        /** @} */
#else
        AsmElfDynamicEntry.setDataPrototype("SgAsmElfDynamicEntry::EntryType", "d_tag", "= SgAsmElfDynamicEntry::DT_NULL",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Value.
         *
         *  See ELF specification for details.
         *
         * @{ */
        rose_rva_t get_d_val() const;
        void set_d_val(rose_rva_t);
        /** @} */
#else
        AsmElfDynamicEntry.setDataPrototype("rose_rva_t", "d_val", "",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        // declared and documented below
#else
        AsmElfDynamicEntry.setDataPrototype("SgAsmGenericString*", "name", "= NULL",
                                            NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Padding byte values.
         *
         * @{ */
        const SgUnsignedCharList& get_extra() const;
        void set_extra(const SgUnsignedCharList&);
        /** @} */
#else
        AsmElfDynamicEntry.setDataPrototype("SgUnsignedCharList", "extra", "",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmElfDynamicEntry);
#if defined(SgAsmElfDynamicEntry_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_d_tag);
            s & BOOST_SERIALIZATION_NVP(p_d_val);
            s & BOOST_SERIALIZATION_NVP(p_name);
            s & BOOST_SERIALIZATION_NVP(p_extra);
        }
#endif

    public:
        enum EntryType {                /* Type    Executable SharedObj Purpose */
            DT_NULL     = 0,        /* ignored mandatory  mandatory Marks end of dynamic array */
            DT_NEEDED   = 1,        /* value   optional   optional  Name of needed library */
            DT_PLTRELSZ = 2,        /* value   optional   optional  Size of reloc entries associated with PLT */
            DT_PLTGOT   = 3,        /* pointer optional   optional  PLT and/or GOT */
            DT_HASH     = 4,        /* pointer mandatory  mandatory Symbol hash table */
            DT_STRTAB   = 5,        /* pointer mandatory  mandatory String table for symbols, lib names, etc. */
            DT_SYMTAB   = 6,        /* pointer mandatory  mandatory Symbol table */
            DT_RELA     = 7,        /* pointer mandatory  optional  Relocation table */
            DT_RELASZ   = 8,        /* value   mandatory  optional  Size of RELA relocation table */
            DT_RELAENT  = 9,        /* value   mandatory  optional  Size of each RELA table entry */
            DT_STRSZ    = 10,       /* value   mandatory  mandatory Size of string table */
            DT_SYMENT   = 11,       /* value   mandatory  mandatory Size of symbol table entry */
            DT_INIT     = 12,       /* pointer optional   optional  Initialization function */
            DT_FINI     = 13,       /* pointer optional   optional  Termination function */
            DT_SONAME   = 14,       /* value   ignored    optional  Name of shared object */
            DT_RPATH    = 15,       /* value   optional   ignored   NUL-term library search path */
            DT_SYMBOLIC = 16,       /* ignored ignored    optional  Bool determines dynamic linker symbol resolution */
            DT_REL      = 17,       /* pointer mandatory  optional  Relocation table */
            DT_RELSZ    = 18,       /* value   mandatory  optional  Size of REL relocation table */
            DT_RELENT   = 19,       /* value   mandatory  optional  Size of each REL table entry */
            DT_PLTREL   = 20,       /* value   optional   optional  Reloc type for PLT; value is DT_RELA or DT_REL */
            DT_DEBUG    = 21,       /* pointer optional   ignored   Contents are not specified at ABI level */
            DT_TEXTREL  = 22,       /* ignored optional   optional  presence => relocs to nonwritable segments OK */
            DT_JMPREL   = 23,       /* pointer optional   optional  Addr of relocation entries for PLT */
            DT_BIND_NOW = 24,       /* ignored optional   optional  Shall dynlinker do relocs before xfering control?*/
            DT_INIT_ARRAY = 25,     /* pointer optional   ?         Array with addresses of init fct */
            DT_FINI_ARRAY = 26,     /* pointer optional   ?         Array with address of fini fct */
            DT_INIT_ARRAYSZ = 27,   /* value   optional   ?         Size in bytes of DT_INIT_ARRAY */
            DT_FINI_ARRAYSZ = 28,   /* value   optional   ?         Size in bytes of DT_FINI_ARRAY */
            DT_RUNPATH  = 29,       /* ?       optional   ?         Library search path (how diff from DT_RPATH?) */
            DT_FLAGS    = 30,       /* value   optional   ?         Bit flags */
            DT_PREINIT_ARRAY = 32,  /* pointer optional   ?         Array with addrs of preinit fct (aka DT_ENCODING)*/
            DT_PREINIT_ARRAYSZ = 33,/* value   optional   ?         size in bytes of DT_PREINIT_ARRAY */
            DT_NUM      = 34,       /* ?       ?          ?         "number used"? */

            DT_GNU_PRELINKED =0x6ffffdf5,/*value ?          ?         Prelinking time stamp */
            DT_GNU_CONFLICTSZ=0x6ffffdf6,/*value ?          ?         Size of conflict section */
            DT_GNU_LIBLISTSZ=0x6ffffdf7,/*value  ?          ?         Size of library list */
            DT_CHECKSUM = 0x6ffffdf8, /* value   ?          ?         ? */
            DT_PLTPADSZ = 0x6ffffdf9, /* value   ?          ?         ? */
            DT_MOVEENT  = 0x6ffffdfa, /* value   ?          ?         ? */
            DT_MOVESZ   = 0x6ffffdfb, /* value   ?          ?         ? */
            DT_FEATURE_1= 0x6ffffdfc, /* value   ?          ?         Feature selection (DTF_*) */
            DT_POSFLAG_1= 0x6ffffdfd, /* value   ?          ?         Flag for DT_* entries affecting next entry */
            DT_SYMINSZ  = 0x6ffffdfe, /* value   ?          ?         Size of syminfo table in bytes */
            DT_SYMINENT = 0x6ffffdff, /* value   ?          ?         Size of each syminfo table entry */

            DT_GNU_HASH = 0x6ffffef5, /* pointer ?          ?         GNU-style hash table */
            DT_TLSDESC_PLT=0x6ffffef6,/* pointer ?          ?         ? */
            DT_TLSDESC_GOT=0x6ffffef7,/* pointer ?          ?         ? */
            DT_GNU_CONFLICT=0x6ffffef8,/*pointer ?          ?         Start of conflict section */
            DT_GNU_LIBLIST=0x6ffffef9,/* pointer ?          ?         Library list */
            DT_CONFIG   = 0x6ffffefa, /* pointer ?          ?         Configuration information */
            DT_DEPAUDIT = 0x6ffffefb, /* pointer ?          ?         Dependency auditing */
            DT_AUDIT    = 0x6ffffefc, /* pointer ?          ?         Object auditing */
            DT_PLTPAD   = 0x6ffffefd, /* pointer ?          ?         PLT padding */
            DT_MOVETAB  = 0x6ffffefe, /* pointer ?          ?         Move table */
            DT_SYMINFO  = 0x6ffffeff, /* pointer ?          ?         Syminfo table */

            DT_VERSYM   = 0x6ffffff0, /* pointer ?          ?         ? */
            DT_RELACOUNT= 0x6ffffff9, /* value   ?          ?         ? */
            DT_RELCOUNT = 0x6ffffffa, /* value   ?          ?         ? */
            DT_FLAGS_1  = 0x6ffffffb, /* value   ?          ?         Sun state flags */
            DT_VERDEF   = 0x6ffffffc, /* pointer ?          ?         Sun version definition table */
            DT_VERDEFNUM= 0x6ffffffd, /* value   ?          ?         Sun number of version definitions */
            DT_VERNEED  = 0x6ffffffe, /* pointer ?          ?         Sun needed versions table */
            DT_VERNEEDNUM=0x6fffffff, /* value   ?          ?         Sun number of needed versions */

            DT_AUXILIARY= 0x7ffffffd, /* pointer ?          ?         Sun shared obj to load before self */
            DT_FILTER   = 0x7fffffff  /* pointer ?          ?         Shared object ot get values from */
        };

#ifdef _MSC_VER
# pragma pack (1)
#endif

        /** Disk format. */
        struct Elf32DynamicEntry_disk {
            uint32_t            d_tag;                  /**< Entry type, one of the DT_* constants */
            uint32_t            d_val;                  /**< Tag's value */
        }
#if !defined(SWIG) && !defined(_MSC_VER)
        __attribute__((packed))
#endif
        ;

        struct Elf64DynamicEntry_disk {
            uint64_t            d_tag;                  /**< Entry type, one of the DT_* constants */
            uint64_t            d_val;                  /**< Tag's value */
        }
#if !defined(SWIG) && !defined(_MSC_VER)
        __attribute__((packed))
#endif
        ;

#ifdef _MSC_VER
# pragma pack ()
#endif

        /** Constructor that links the object into the AST. */
        explicit SgAsmElfDynamicEntry(SgAsmElfDynamicSection *dynsec)
            : p_d_tag(DT_NULL), p_name(NULL) {
            ctor(dynsec);
        }

        /** Initialize object by parsing the file.
         *
         * @{ */
        void parse(ByteOrder::Endianness, const SgAsmElfDynamicEntry::Elf32DynamicEntry_disk*);
        void parse(ByteOrder::Endianness, const SgAsmElfDynamicEntry::Elf64DynamicEntry_disk*);
        /** @} */

        /** Convert object to disk representation.
         *
         * @{ */
        void *encode(ByteOrder::Endianness, SgAsmElfDynamicEntry::Elf32DynamicEntry_disk*) const;
        void *encode(ByteOrder::Endianness, SgAsmElfDynamicEntry::Elf64DynamicEntry_disk*) const;
        /** @} */

        /** Print some debugging info */
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const;

        /** Convert Dynamic Entry Tag to a string */
        static std::string to_string(SgAsmElfDynamicEntry::EntryType);

        /** Property: Name.
         *
         * @{ */
        SgAsmGenericString* get_name() const { return p_name; }
        void set_name(SgAsmGenericString*);
        /** @} */

    private:
        void ctor(SgAsmElfDynamicSection*);
#endif // SgAsmElfDynamicEntry_OTHERS

#ifdef DOCUMENTATION
    };
#endif




    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*************************************************************************************************************************
     *                                         ELF String Tables
     *************************************************************************************************************************/

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfStringSection);
    IS_SERIALIZABLE(AsmElfStringSection);
    AsmElfStringSection.setAutomaticGenerationOfDestructor(false);

#ifdef DOCUMENTATION
    /** ELF string table section.
     *
     *  A file section that holds string literals such as symbol names. */
    class SgAsmElfStringSection: public SgAsmElfSection {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: String table.
         *
         *  Pointer to the actual string table for this section.
         *
         * @{ */
        SgAsmElfStrtab* get_strtab() const;
        void set_strtab(SgAsmElfStrtab*);
        /** @} */
#else
        AsmElfStringSection.setDataPrototype("SgAsmElfStrtab*", "strtab", "= NULL",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmElfStringSection);
#if defined(SgAsmElfStringSection_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmElfSection);
            s & BOOST_SERIALIZATION_NVP(p_strtab);
        }
#endif

    public:
        /** Constructor that links new object into AST. */
        explicit SgAsmElfStringSection(SgAsmElfFileHeader *fhdr)
            : SgAsmElfSection(fhdr), p_strtab(NULL) {
            ctor();
        }

        /** Initialize object by parsing binary specimen. */
        virtual SgAsmElfStringSection *parse() $ROSE_OVERRIDE;

        /** Dump debugging information. */
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const $ROSE_OVERRIDE;

        /** Adjust size of table.
         *
         * Augments superclass to make sure free list and such are adjusted properly. Any time the ELF String Section size is
         * changed we adjust the free list in the ELF String Table contained in this section. */
        virtual void set_size(rose_addr_t newsize) $ROSE_OVERRIDE;

        /** Reallocate space for the string section if necessary.
         *
         *  Note that reallocation is lazy here -- we don't shrink the section, we only enlarge it (if you want the section to
         *  shrink then call SgAsmGenericStrtab::reallocate(bool) with a true value rather than calling this
         *  function. SgAsmElfStringSection::reallocate is called in response to unparsing a file and gives the string table a
         *  chance to extend its container section if it needs to allocate more space for strings. */
        virtual bool reallocate() $ROSE_OVERRIDE;

        /** Unparse an ElfStringSection by unparsing the ElfStrtab */
        virtual void unparse(std::ostream&) const $ROSE_OVERRIDE;

    private:
        void ctor();
        void ctor(SgAsmElfSectionTable*);
#endif // SgAsmElfStringSection_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfStrtab);
    IS_SERIALIZABLE(AsmElfStrtab);
    AsmElfStrtab.setAutomaticGenerationOfDestructor(false);

#ifdef DOCUMENTATION
    /** ELF string table. */
    class SgAsmElfStrtab: public SgAsmGenericStrtab {
    public:
#endif

        DECLARE_OTHERS(AsmElfStrtab);
#if defined(SgAsmElfStrtab_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmGenericStrtab);
        }
#endif

    public:
        /** Non-parsing constructor.
         *
         *  The table is created to be at least one byte long and having a NUL character as the first byte. */
        explicit SgAsmElfStrtab(class SgAsmElfSection *containing_section)
            : SgAsmGenericStrtab(containing_section) {
            ctor();
        }

        /** Free StringStorage objects associated with this string table.
         *
         *  It may not be safe to blow them away yet since other objects may still have @ref SgAsmStoredStrings pointing to
         *  these storage objects. So instead, we will mark all this strtab's storage objects as no longer being associated
         *  with a string table. This allows the @ref SgAsmStoredString objects to still function properly and their
         *  destructors will free their storage. */
        virtual ~SgAsmElfStrtab();

        /** Parses the string table.
         *
         *  All that actually happens at this point is we look to see if the table begins with an empty string. */
        virtual SgAsmElfStrtab *parse() $ROSE_OVERRIDE;

        /** Write string table back to disk.
         *
         *  Free space is zeroed out; holes are left as they are. */
        virtual void unparse(std::ostream&) const;

        /** Creates the storage item for the string at the specified offset.
         *
         *  If @p shared is true then attempt to re-use a previous storage object, otherwise always create a new one. Each
         *  storage object is considered a separate string, therefore when two strings share the same storage object, changing
         *  one string changes the other. */
        virtual SgAsmStringStorage *create_storage(rose_addr_t offset, bool shared) $ROSE_OVERRIDE;

        /** Returns the number of bytes required to store the string in the string table.
         *
         *  This is the length of the string plus one for the NUL terminator. */
        virtual rose_addr_t get_storage_size(const SgAsmStringStorage*) $ROSE_OVERRIDE;

        /** Find offset for a string.
         *
         *  Tries to find a suitable offset for a string such that it overlaps with some other string already allocated. If the
         *  new string is the same as the end of some other string (new="main", existing="domain") then we just use an offset
         *  into that string since the space is already allocated for the existing string. If the new string ends with an
         *  existing string (new="domain", existing="main") and there's enough free space before the existing string (two bytes
         *  in this case) then we allocate some of that free space and use a suitable offset. In any case, upon return
         *  <code>storege->get_offset()</code> will return the allocated offset if successful, or
         *  @ref SgAsmGenericString::unallocated if we couldn't find an overlap. */
        virtual void allocate_overlap(SgAsmStringStorage*) $ROSE_OVERRIDE;

        /** Similar to create_storage() but uses a storage object that's already been allocated. */
        virtual void rebind(SgAsmStringStorage*, rose_addr_t) $ROSE_OVERRIDE;

    private:
        void ctor();
#endif // SgAsmElfStrtab_OTHERS

#ifdef DOCUMENTATION
    };
#endif




    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*************************************************************************************************************************
     *                                         ELF Notes
     *************************************************************************************************************************/

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfNoteSection);
    IS_SERIALIZABLE(AsmElfNoteSection);

#ifdef DOCUMENTATION
    class SgAsmElfNoteSection: public SgAsmElfSection {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: List of note entries.
         *
         *  This property points to an AST node that contains the list rather than being a list directly because of limitations
         *  of ROSETTA.
         *
         * @{ */
        SgAsmElfNoteEntryList* get_entries() const;
        void set_entries(SgAsmElfNoteEntryList*);
        /** @} */
#else
        AsmElfNoteSection.setDataPrototype("SgAsmElfNoteEntryList*", "entries", "= NULL",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmElfNoteSection);
#if defined(SgAsmElfNoteSection_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmElfSection);
            s & BOOST_SERIALIZATION_NVP(p_entries);
        }
#endif

    public:
        /** Non-parsing constructor */
        explicit SgAsmElfNoteSection(SgAsmElfFileHeader *fhdr)
            : SgAsmElfSection(fhdr) {
            ctor();
        }
        virtual SgAsmElfNoteSection *parse() $ROSE_OVERRIDE;

        /** Pre-unparsing adjustments */
        virtual bool reallocate() $ROSE_OVERRIDE;

        /** Write data to note section */
        virtual void unparse(std::ostream&) const $ROSE_OVERRIDE;

        /** Print some debugging information */
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const $ROSE_OVERRIDE;

    private:
        void ctor();
#endif // SgAsmElfNoteSection_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfNoteEntryList);
    IS_SERIALIZABLE(AsmElfNoteEntryList);

#ifdef DOCUMENTATION
    /** Node to hold list of ELF note entries.
     *
     *  This node's only purpose is to hold the list of pointers to note entries, which must be done like this because of
     *  limitations of ROSETTA. */
    class SgAsmElfNoteEntryList: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: List of ELF not entries.
         *
         * @{ */
        const SgAsmElfNoteEntryPtrList& get_entries() const;
        void set_entries(const SgAsmElfNoteEntryPtrList&);
        /** @} */
#else
        AsmElfNoteEntryList.setDataPrototype("SgAsmElfNoteEntryPtrList", "entries", "",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmElfNoteEntryList);
#if defined(SgAsmElfNoteEntryList_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_entries);
        }
#endif
#endif // SgAsmElfNoteEntryList_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfNoteEntry);
    IS_SERIALIZABLE(AsmElfNoteEntry);

#ifdef DOCUMENTATION
    /** One entry of an ELF notes table. */
    class SgAsmElfNoteEntry: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Type of note.
         *
         *  See official ELF specification.
         *
         * @{ */
        unsigned get_type() const;
        void set_type(unsigned);
        /** @} */
#else
        AsmElfNoteEntry.setDataPrototype("unsigned", "type", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        // documented below
#else
        AsmElfNoteEntry.setDataPrototype("SgAsmGenericString*", "name", "= NULL",
                                         NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Note payload.
         *
         *  This is the data associated with the note.
         *
         * @{ */
        const SgUnsignedCharList& get_payload() const;
        void set_payload(const SgUnsignedCharList&);
        /** @} */
#else
        AsmElfNoteEntry.setDataPrototype("SgUnsignedCharList", "payload", "",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmElfNoteEntry);
#if defined(SgAsmElfNoteEntry_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_type);
            s & BOOST_SERIALIZATION_NVP(p_name);
            s & BOOST_SERIALIZATION_NVP(p_payload);
        }
#endif

    public:
        /** Constructor adds the new note to the list of notes for the note section. */
        SgAsmElfNoteEntry(SgAsmElfNoteSection *section)
            : p_type(0), p_name(NULL) {
            ctor(section);
        }

        /** Property: Note name.
         *
         *  A string note name stored in an ELF string table in the binary specimen. Changing the name of a note also changes
         *  the contents of the string table.
         *
         * @{ */
        SgAsmGenericString *get_name() const;
        void set_name(SgAsmGenericString *name);
        /** @} */

        /** Initialize a note by parsing it from the specified location in the note section.
         *
         *  Return value is the offset to the beginning of the next note. */
        rose_addr_t parse(rose_addr_t starting_offset);

        /** Write a note at the specified offset to the section containing the note.
         *
         *  Returns the offset for the first byte past the end of the note. */
        rose_addr_t unparse(std::ostream &f, rose_addr_t starting_offset);

        /** Print some debugging information */
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const;

        /** Property: Note payload.
         *
         *  This is the data associated with the note. */
        void set_payload(const void*, size_t nbytes);

        /** Returns the number of bytes needed to store this note. */
        rose_addr_t calculate_size() const;

    private:
        void ctor(SgAsmElfNoteSection *section);
#endif // SgAsmElfNoteEntry_OTHERS

#ifdef DOCUMENTATION
    };
#endif



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*************************************************************************************************************************
     *                                         ELF Exception Handling
     *************************************************************************************************************************/

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfEHFrameSection);
    IS_SERIALIZABLE(AsmElfEHFrameSection);

#ifdef DOCUMENTATION
    /** Represents an ELF EH frame section. */
    class SgAsmElfEHFrameSection: public SgAsmElfSection {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: CI entries.
         *
         *  See official ELF specification.  This property points to an AST node containing the list rather than the direct
         *  list due to limitations of ROSETTA.
         *
         * @{ */
        SgAsmElfEHFrameEntryCIList* get_ci_entries() const;
        void set_ci_entries(SgAsmElfEHFrameEntryCIList*);
        /** @} */
#else
        AsmElfEHFrameSection.setDataPrototype("SgAsmElfEHFrameEntryCIList*", "ci_entries", "= NULL",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmElfEHFrameSection);
#if defined(SgAsmElfEHFrameSection_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmElfSection);
            s & BOOST_SERIALIZATION_NVP(p_ci_entries);
        }
#endif

    public:
        /** Non-parsing constructor. */
        explicit SgAsmElfEHFrameSection(SgAsmElfFileHeader *fhdr)
            : SgAsmElfSection(fhdr), p_ci_entries(NULL) {
            ctor();
        }

        /** Initialize by parsing a file. */
        virtual SgAsmElfEHFrameSection *parse() $ROSE_OVERRIDE;

        /** Return sizes for various parts of the table.
         *
         *  See documentation for @ref SgAsmElfSection::calculate_sizes. Since EH Frame Sections are run-length encoded, we
         *  need to actually unparse the section in order to determine its size. */
        virtual rose_addr_t calculate_sizes(size_t *total, size_t *required, size_t *optional, size_t *entcount) const $ROSE_OVERRIDE;

        /** Write data to .eh_frame section */
        virtual void unparse(std::ostream&) const $ROSE_OVERRIDE;

        /** Unparses the section into the optional output stream and returns the number of bytes written.
         *
         *  If there is no output stream we still go through the actions but don't write anything. This is the only way to
         *  determine the amount of memory required to store the section since the section is run-length encoded. */
        rose_addr_t unparse(std::ostream*) const;

        /** Print some debugging info */
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const $ROSE_OVERRIDE;

    private:
        void ctor();
#endif // SgAsmElfEHFrameSection_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfEHFrameEntryCIList);
    IS_SERIALIZABLE(AsmElfEHFrameEntryCIList);

#ifdef DOCUMENTATION
    /** List of ELF EH frame CI entries.
     *
     *  See official ELF specification. The only reason this node exists is because of ROSETTA limitations which prevent the
     *  list from being stored directly in the class that needs it. */
    class SgAsmElfEHFrameEntryCIList: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: List of pointers to ELF EH frame CI entries.
         *
         * @{ */
        const SgAsmElfEHFrameEntryCIPtrList& get_entries() const;
        void set_entries(const SgAsmElfEHFrameEntryCIPtrList&);
        /** @} */
#else
        AsmElfEHFrameEntryCIList.setDataPrototype("SgAsmElfEHFrameEntryCIPtrList", "entries", "",
                                                  NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL,
                                                  NO_DELETE);
#endif

        DECLARE_OTHERS(AsmElfEHFrameEntryCIList);
#if defined(SgAsmElfEHFrameEntryCIList_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_entries);
        }
#endif
#endif // SgAsmElfEHFrameEntryCIList_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfEHFrameEntryCI);
    IS_SERIALIZABLE(AsmElfEHFrameEntryCI);

#ifdef DOCUMENTATION
    /** ELF error handling frame entry, common information entry.
     *
     *  Most of the properties of this class are documented in the official ELF specification. */
    class SgAsmElfEHFrameEntryCI: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Version number.
         *
         *  See official ELF specification.
         *
         * @{ */
        int get_version() const;
        void set_version(int);
        /** @} */
#else
        AsmElfEHFrameEntryCI.setDataPrototype("int", "version", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Augmentation string.
         *
         *  See official ELF specification.
         *
         * @{ */
        const std::string& get_augmentation_string() const;
        void set_augmentation_string(const std::string&);
        /** @} */
#else
        AsmElfEHFrameEntryCI.setDataPrototype("std::string", "augmentation_string", "= \"\"",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Data value.
         *
         *  See official ELF specification.
         *
         * @{ */
        uint64_t get_eh_data() const;
        void set_eh_data(uint64_t);
        /** @} */
#else
        AsmElfEHFrameEntryCI.setDataPrototype("uint64_t", "eh_data", "=0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Code alignment factor.
         *
         *  See official ELF specification.
         *
         *  @{ */
        uint64_t get_code_alignment_factor() const;
        void set_code_alignment_factor(uint64_t);
        /** @} */
#else
        AsmElfEHFrameEntryCI.setDataPrototype("uint64_t", "code_alignment_factor", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Data alignment factor.
         *
         *  See official ELF specification.
         *
         * @{ */
        int64_t get_data_alignment_factor() const;
        void set_data_alignment_factor(int64_t);
        /** @} */
#else
        AsmElfEHFrameEntryCI.setDataPrototype("int64_t", "data_alignment_factor", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Augmentation data length.
         *
         *  See official ELF specification.
         *
         * @{ */
        uint64_t get_augmentation_data_length() const;
        void set_augmentation_data_length(uint64_t);
        /** @} */
#else
        AsmElfEHFrameEntryCI.setDataPrototype("uint64_t", "augmentation_data_length", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: LSDA encoding.
         *
         *  See official ELF specification.
         *
         * @{ */
        int get_lsda_encoding() const;
        void set_lsda_encoding(int);
        /** @} */
#else
        AsmElfEHFrameEntryCI.setDataPrototype("int", "lsda_encoding", "= -1",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: PRH encoding.
         *
         *  See official ELF specification.
         *
         * @{ */
        int get_prh_encoding() const;
        void set_prh_encoding(int);
        /** @} */
#else
        AsmElfEHFrameEntryCI.setDataPrototype("int", "prh_encoding", "= -1",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: PRH argument.
         *
         *  See official ELF specification.
         *
         * @{ */
        unsigned get_prh_arg() const;
        void set_prh_arg(unsigned);
        /** @} */
#else
        AsmElfEHFrameEntryCI.setDataPrototype("unsigned", "prh_arg", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: PRH address.
         *
         *  See official ELF specification.
         *
         * @{ */
        rose_addr_t get_prh_addr() const;
        void set_prh_addr(rose_addr_t);
        /** @} */
#else
        AsmElfEHFrameEntryCI.setDataPrototype("rose_addr_t", "prh_addr", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Address encoding.
         *
         *  See official ELF specification.
         *
         * @{ */
        int get_addr_encoding() const;
        void set_addr_encoding(int);
        /** @} */
#else
        AsmElfEHFrameEntryCI.setDataPrototype("int", "addr_encoding", "= -1",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Signal frame.
         *
         *  See official ELF specification.
         *
         * @{ */
        bool get_sig_frame() const;
        void set_sig_frame(bool);
        /** @} */
#else
        AsmElfEHFrameEntryCI.setDataPrototype("bool", "sig_frame", "= false",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Instructions.
         *
         *  See official ELF specification.
         *
         * @{ */
        const SgUnsignedCharList& get_instructions() const;
        void set_instructions(const SgUnsignedCharList);
        /** @} */
#else
        AsmElfEHFrameEntryCI.setDataPrototype("SgUnsignedCharList", "instructions", "",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: FD entries.
         *
         *  See official ELF specification. This points to an AST node that holds the list of pointers to the actual entry
         *  nodes due to limitations of ROSETTA.
         *
         * @{ */
        SgAsmElfEHFrameEntryFDList* get_fd_entries() const;
        void set_fd_entries(SgAsmElfEHFrameEntryFDList*);
        /** @} */
#else
        AsmElfEHFrameEntryCI.setDataPrototype("SgAsmElfEHFrameEntryFDList*", "fd_entries", "= NULL",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmElfEHFrameEntryCI);
#if defined(SgAsmElfEHFrameEntryCI_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_version);
            s & BOOST_SERIALIZATION_NVP(p_augmentation_string);
            s & BOOST_SERIALIZATION_NVP(p_eh_data);
            s & BOOST_SERIALIZATION_NVP(p_code_alignment_factor);
            s & BOOST_SERIALIZATION_NVP(p_data_alignment_factor);
            s & BOOST_SERIALIZATION_NVP(p_augmentation_data_length);
            s & BOOST_SERIALIZATION_NVP(p_lsda_encoding);
            s & BOOST_SERIALIZATION_NVP(p_prh_encoding);
            s & BOOST_SERIALIZATION_NVP(p_prh_arg);
            s & BOOST_SERIALIZATION_NVP(p_addr_encoding);
            s & BOOST_SERIALIZATION_NVP(p_sig_frame);
            s & BOOST_SERIALIZATION_NVP(p_instructions);
            s & BOOST_SERIALIZATION_NVP(p_fd_entries);
        }
#endif

    public:
        /** Non-parsing constructor */
        explicit SgAsmElfEHFrameEntryCI(SgAsmElfEHFrameSection *ehframe)
            : p_version(0), p_eh_data(0), p_code_alignment_factor(0), p_data_alignment_factor(0),
              p_augmentation_data_length(0), p_lsda_encoding(-1), p_prh_encoding(-1), p_prh_arg(0), p_prh_addr(0),
              p_addr_encoding(-1), p_sig_frame(false), p_fd_entries(NULL) {
            ctor(ehframe);
        }

        /** Print some debugging info */
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const;

        /** Unparse an entry.
         *
         *  Unparse one Common Information Entry (CIE) without unparsing the Frame Description Entries (FDE) to which it
         *  points. The initial length fields are not included in the result string. */
        std::string unparse(const SgAsmElfEHFrameSection*) const;

    private:
        void ctor(SgAsmElfEHFrameSection*);
#endif // SgAsmElfEHFrameEntryCI_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfEHFrameEntryFDList);
    IS_SERIALIZABLE(AsmElfEHFrameEntryFDList);

#ifdef DOCUMENTATION
    /** List of ELF error handling frame descriptor entries.
     *
     *  The only purpose of this node is to hold the list of pointers to FD entries, and is necesssary due to limitations of
     *  ROSETTA. */
    class AsmElfEHFrameEntryFDList: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: List of pointers to error handling frame descriptor entries.
         *
         * @{ */
        const SgAsmElfEHFrameEntryFDPtrList& get_entries() const;
        void set_entries(const SgAsmElfEHFrameEntryFDPtrList&);
        /** @} */
#else
        AsmElfEHFrameEntryFDList.setDataPrototype("SgAsmElfEHFrameEntryFDPtrList", "entries", "",
                                                  NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL,
                                                  NO_DELETE);
#endif

        DECLARE_OTHERS(AsmElfEHFrameEntryFDList);
#if defined(SgAsmElfEHFrameEntryFDList_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_entries);
        }
#endif
#endif // SgAsmElfEHFrameEntryFDList_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmElfEHFrameEntryFD);
    IS_SERIALIZABLE(AsmElfEHFrameEntryFD);

#ifdef DOCUMENTATION
    /** ELF error handling frame entry frame description entry. */
    class SgAsmElfEHFrameEntryFD: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Beginning relative virtual address.
         *
         *  See official ELF specification.
         *
         * @{ */
        const rose_rva_t& get_begin_rva() const;
        void set_begin_rva(const rose_rva_t&);
        /** @} */
#else
        AsmElfEHFrameEntryFD.setDataPrototype("rose_rva_t", "begin_rva", "",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Size in bytes.
         *
         *  See official ELF specification.
         *
         * @{ */
        rose_addr_t get_size() const;
        void set_size(rose_addr_t);
        /** @} */
#else
        AsmElfEHFrameEntryFD.setDataPrototype("rose_addr_t", "size", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Augmentation data.
         *
         *  See official ELF specification.
         *
         * @{ */
        const SgUnsignedCharList& get_augmentation_data() const;
        void set_augmentation_data(const SgUnsignedCharList&);
        /** @} */
#else
        AsmElfEHFrameEntryFD.setDataPrototype("SgUnsignedCharList", "augmentation_data", "",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Instructions.
         *
         *  See official ELF specification.
         *
         * @{ */
        const SgUnsignedCharList& get_instructions() const;
        void set_instructions(const SgUnsignedCharList&);
        /** @} */
#else
        AsmElfEHFrameEntryFD.setDataPrototype("SgUnsignedCharList", "instructions", "",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmElfEHFrameEntryFD);
#if defined(SgAsmElfEHFrameEntryFD_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_begin_rva);
            s & BOOST_SERIALIZATION_NVP(p_size);
            s & BOOST_SERIALIZATION_NVP(p_augmentation_data);
            s & BOOST_SERIALIZATION_NVP(p_instructions);
        }
#endif

    public:
        /** Non-parsing constructor */
        explicit SgAsmElfEHFrameEntryFD(SgAsmElfEHFrameEntryCI *cie)
            : p_size(0) {
            ctor(cie);
        }

        /** Print some debugging info */
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const;

        /** Unparse to string.
         *
         *  Unparses the frame description entry (FDE) into a string but do not include the leading length field(s) or the CIE
         *  back pointer. */
        std::string unparse(const SgAsmElfEHFrameSection*, SgAsmElfEHFrameEntryCI*) const;

    private:
        void ctor(SgAsmElfEHFrameEntryCI*);
#endif // SgAsmElfEHFrameEntryFD_OTHERS

#ifdef DOCUMENTATION
    };
#endif




    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*************************************************************************************************************************
     *                                         ELF Sections
     *************************************************************************************************************************/

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NEW_NONTERMINAL_MACRO(AsmElfSection,
                          AsmElfSymbolSection | AsmElfRelocSection         | AsmElfDynamicSection       |
                          AsmElfStringSection | AsmElfNoteSection          | AsmElfEHFrameSection       |
                          AsmElfSymverSection | AsmElfSymverDefinedSection | AsmElfSymverNeededSection,
                          "AsmElfSection", "AsmElfSectionTag", true);
    AsmElfSection.setCppCondition("!defined(DOCUMENTATION)");
    IS_SERIALIZABLE(AsmElfSection);

#ifdef DOCUMENTATION
    /** Base class for ELF file sections. */
    class SgAsmElfSection: public SgAsmGenericSection {
    public:
#endif

#ifdef DOCUMENTATION
        // Documentation below
        // DQ (8/22/2008): These are not automatically generated since one of them must be virtual.
#else
        AsmElfSection.setDataPrototype("SgAsmElfSection*", "linked_section", "= NULL", //accessors must be virtual
                                       NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: The section table entry corresponding to this section.
         *
         * @{ */
        SgAsmElfSectionTableEntry* get_section_entry() const;
        void set_section_entry(SgAsmElfSectionTableEntry*);
        /** @} */
#else
        AsmElfSection.setDataPrototype("SgAsmElfSectionTableEntry*", "section_entry", "= NULL",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: The segment table entry corresponding to this section.
         *
         * @{ */
        SgAsmElfSegmentTableEntry* get_segment_entry() const;
        void set_segment_entry(SgAsmElfSegmentTableEntry*);
        /** @} */
#else
        AsmElfSection.setDataPrototype("SgAsmElfSegmentTableEntry*", "segment_entry", "= NULL",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmElfSection);
#if defined(SgAsmElfSection_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmGenericSection);
            s & BOOST_SERIALIZATION_NVP(p_linked_section);
            s & BOOST_SERIALIZATION_NVP(p_section_entry);
            s & BOOST_SERIALIZATION_NVP(p_segment_entry);
        }
#endif

    public:
        /** Constructor for sections not yet in a table.
         *
         *  This constructs a section that is in neither the ELF Section Table nor the ELF Segment Table yet, but eventually
         *  will be. */
        explicit SgAsmElfSection(SgAsmGenericHeader *fhdr)
            : SgAsmGenericSection(fhdr->get_file(), fhdr), p_linked_section(NULL), p_section_entry(NULL),
              p_segment_entry(NULL) {
            ctor();
        }

        /** Initializes the section from data parsed from the ELF Section Table.
         *
         *  This includes the section name, offset, size, memory mapping, and alignments. The @p id is the index into the
         *  section table. This function complements @ref SgAsmElfSectionTable::add_section in that this function initializes
         *  this section from the section table while @ref SgAsmElfSectionTable::add_section "add_section" initializes the
         *  section table from the section. */
        SgAsmElfSection *init_from_section_table(SgAsmElfSectionTableEntry*, SgAsmElfStringSection*, int id);

        /** Initializes the section from data parse from the ELF Segment Table.
         *
         *  This is similar to @ref init_from_section_table but for segments instead of sections. */
        SgAsmElfSection *init_from_segment_table(SgAsmElfSegmentTableEntry*, bool mmap_only=false);

        /** Returns info about the size of the entries based on information already available.
         *
         *  Any or all arguments may be null pointers if the caller is not interested in the value. Return values are:
         *
         *  @li @p entsize is the size of each entry, sum of required and optional parts. This comes from the sh_entsize member
         *  of this section's ELF Section Table Entry, adjusted upward to be large enough to hold the required part of each
         *  entry (see "required").
         *
         *  @li @p required is the size of the required (leading) part of each entry. The size of the required part is based
         *  on the ELF word size.
         *
         *  @li @p optional is the size of the optional (trailing) part of each entry. If the section has been parsed then
         *  the optional size will be calculated from the entry with the largest "extra" (aka, optional) data. Otherwise this
         *  is calculated as the difference between the @p entsize" and the @p required" sizes.
         *
         *  @li entcount is the total number of entries in this section. If the section has been parsed then this is the
         *  actual number of parsed entries, otherwise its the section size divided by the @p entsize.
         *
         *  Return value is the total size needed for the section. In all cases, it is the product of @p entsize and @p
         *  entcount. */
        rose_addr_t calculate_sizes(size_t r32size, size_t r64size, const std::vector<size_t> &optsizes,
                                    size_t *entsize, size_t *required, size_t *optional, size_t *entcount) const;

        virtual void finish_parsing() {}

        /** Base implementation for calculating sizes.
         *
         *  Most subclasses will override this virtual function in order to return more useful values. This implementation
         *  returns the following values:
         *
         *  @li @p entsize is size stored in the ELF Section Table's sh_entsize member, or size of entire section if not a
         *  table.
         *
         *  @li @p required is the same as @p entsize.
         *
         *  @li @p optional is zero.
         *
         *  @li @p entcount is the number of entries, each of size entsize, that can fit in the section.
         *
         *  The return size is the product of @p entsize and @p entcount, which, if this section is a table (nonzero
         *  sh_entsize), could be smaller than the total size of the section. */
        virtual rose_addr_t calculate_sizes(size_t *entsize, size_t *required, size_t *optional, size_t *entcount) const;

        /** Called prior to unparse to make things consistent. */
        virtual bool reallocate() $ROSE_OVERRIDE;

        /** Print some debugging info */
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const $ROSE_OVERRIDE;

        /** Make this section's name to be stored in the specified string table. */
        void allocate_name_to_storage(SgAsmElfStringSection*);

        /** Obtain ELF header.
         *
         *  This is just a convenience function so we don't need to constantly cast the return value from @ref get_header. */
        SgAsmElfFileHeader *get_elf_header() const;

        /** Property: Linked section.
         *
         *  Points to an optional related section. See official ELF specification.
         *
         * @{ */
        SgAsmElfSection* get_linked_section () const;
        virtual void set_linked_section(SgAsmElfSection*);
        /** @} */

    private:
        void ctor();
#endif // SgAsmElfSection_OTHERS

#ifdef DOCUMENTATION
    };
#endif



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*************************************************************************************************************************
     *                                         DOS File Header
     *************************************************************************************************************************/

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmDOSFileHeader);
    IS_SERIALIZABLE(AsmDOSFileHeader);

#ifdef DOCUMENTATION
    /** Represents the file header for DOS executables.
     *
     *  Many of the properties for this node come directly from the DOS specification and have similar names to that
     *  documentation. Such properties are not documented in ROSE. */
    class SgAsmDOSFileHeader: public SgAsmGenericHeader {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Last page size.
         *
         *  See DOS specification.
         *
         * @{ */
        uint16_t get_e_last_page_size() const;
        void set_e_last_page_size(uint16_t);
        /** @} */
#else
        AsmDOSFileHeader.setDataPrototype("uint16_t", "e_last_page_size", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Total number of pages.
         *
         *  See DOS specification.
         *
         * @{ */
        uint16_t get_e_total_pages() const;
        void set_e_total_pages(uint16_t);
        /** @} */
#else
        AsmDOSFileHeader.setDataPrototype("uint16_t", "e_total_pages", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Number of relocations.
         *
         *  See DOS specification.
         *
         * @{ */
        uint16_t get_e_nrelocs() const;
        void set_e_nrelocs(uint16_t);
        /** @} */
#else
        AsmDOSFileHeader.setDataPrototype("uint16_t", "e_nrelocs", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Number of header paragraphs.
         *
         *  See DOS specification.
         *
         * @{ */
        uint16_t get_e_header_paragraphs() const;
        void set_e_header_paragraphs(uint16_t);n
        /** @} */
#else
        AsmDOSFileHeader.setDataPrototype("uint16_t", "e_header_paragraphs", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Min alloc.
         *
         *  See DOS specification.
         *
         * @{ */
        uint16_t get_e_minalloc() const;
        void set_e_minalloc(uint16_t);
        /** @} */
#else
        AsmDOSFileHeader.setDataPrototype("uint16_t", "e_minalloc", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Max alloc.
         *
         *  See DOS specification.
         *
         * @{ */
        uint16_t get_e_maxalloc() const;
        void set_e_maxalloc(uint16_t);
        /** @} */
#else
        AsmDOSFileHeader.setDataPrototype("uint16_t", "e_maxalloc", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: SS.
         *
         *  See DOS specification.
         *
         * @{ */
        uint16_t get_e_ss() const;
        void set_e_ss(uint16_t);
        /** @} */
#else
        AsmDOSFileHeader.setDataPrototype("uint16_t", "e_ss", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: SP.
         *
         *  See DOS specification.
         *
         * @{ */
        uint16_t get_e_sp() const;
        void set_e_sp(uint16_t);
        /** @} */
#else
        AsmDOSFileHeader.setDataPrototype("uint16_t", "e_sp", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Checksum.
         *
         *  See DOS specification.
         *
         * @{ */
        uint16_t get_e_cksum() const;
        void set_e_cksum(uint16_t);
        /** @} */
#else
        AsmDOSFileHeader.setDataPrototype("uint16_t", "e_cksum", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: IP.
         *
         *  See DOS specification.
         *
         * @{ */
        uint16_t get_e_ip() const;
        void set_e_ip(uint16_t);
        /** @} */
#else
        AsmDOSFileHeader.setDataPrototype("uint16_t", "e_ip", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: CS.
         *
         *  See DOS specification.
         *
         * @{ */
        uint16_t get_e_cs() const;
        void set_e_cs(uint16_t);
        /** @} */
#else
        AsmDOSFileHeader.setDataPrototype("uint16_t", "e_cs", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Overlay.
         *
         *  See DOS specification.
         *
         * @{ */
        uint16_t get_e_overlay() const;
        void set_e_overlay(uint16_t);
        /** @} */
#else
        AsmDOSFileHeader.setDataPrototype("uint16_t", "e_overlay", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Relocs offset.
         *
         *  See DOS specification.
         *
         * @{ */
        rose_addr_t get_e_relocs_offset() const;
        void set_e_relocs_offset(rose_addr_t);
        /** @} */
#else
        AsmDOSFileHeader.setDataPrototype("rose_addr_t", "e_relocs_offset", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Reserved field 1.
         *
         *  See DOS specification.
         *
         * @{ */
        unsigned get_e_res1() const;
        void set_e_res1(unsigned);
        /** @} */
#else
        AsmDOSFileHeader.setDataPrototype("unsigned", "e_res1", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Section containing relocation information.
         *
         * @{ */
        SgAsmGenericSection* get_relocs() const;
        void set_relocs(SgAsmGenericSection*);
        /** @} */
#else
        AsmDOSFileHeader.setDataPrototype("SgAsmGenericSection*", "relocs", "= NULL",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Real mode section.
         *
         * @{ */
        SgAsmGenericSection* get_rm_section() const;
        void set_rm_section(SgAsmGenericSection*);
        /** @} */
#else
        AsmDOSFileHeader.setDataPrototype("SgAsmGenericSection*", "rm_section", "= NULL", //real-mode section
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmDOSFileHeader);
#if defined(SgAsmDOSFileHeader_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmGenericHeader);
            s & BOOST_SERIALIZATION_NVP(p_e_last_page_size);
            s & BOOST_SERIALIZATION_NVP(p_e_total_pages);
            s & BOOST_SERIALIZATION_NVP(p_e_nrelocs);
            s & BOOST_SERIALIZATION_NVP(p_e_header_paragraphs);
            s & BOOST_SERIALIZATION_NVP(p_e_minalloc);
            s & BOOST_SERIALIZATION_NVP(p_e_maxalloc);
            s & BOOST_SERIALIZATION_NVP(p_e_ss);
            s & BOOST_SERIALIZATION_NVP(p_e_sp);
            s & BOOST_SERIALIZATION_NVP(p_e_cksum);
            s & BOOST_SERIALIZATION_NVP(p_e_ip);
            s & BOOST_SERIALIZATION_NVP(p_e_cs);
            s & BOOST_SERIALIZATION_NVP(p_e_overlay);
            s & BOOST_SERIALIZATION_NVP(p_e_relocs_offset);
            s & BOOST_SERIALIZATION_NVP(p_e_res1);
            s & BOOST_SERIALIZATION_NVP(p_relocs);
            s & BOOST_SERIALIZATION_NVP(p_rm_section);
        }
#endif

    public:
#ifdef _MSC_VER
# pragma pack (1)
#endif
        /** On-disk format of DOS file header. */
        struct DOSFileHeader_disk {
            unsigned char e_magic[2];                /**< 0x00 "MZ" */
            uint16_t      e_last_page_size;          /**< 0x02 bytes used on last page of file (1 page == 512 bytes);
                                                      *        zero implies if last page is full. */ //
            uint16_t      e_total_pages;             /**< 0x04 number of pages (inc. last possibly partial page) in file. */
            uint16_t      e_nrelocs;                 /**< 0x06 number of relocation entries stored after this header. */
            uint16_t      e_header_paragraphs;       /**< 0x08 header size in paragraphs (16-byte blocks) inc. relocations. */
            uint16_t      e_minalloc;                /**< 0x0a number of extra paragraphs needed, similar to BSS in Unix. */
            uint16_t      e_maxalloc;                /**< 0x0c max paragraphs to allocate for BSS. */
            uint16_t      e_ss;                      /**< 0x0e initial value of SS register relative to program load segment. */
            uint16_t      e_sp;                      /**< 0x10 initial value for SP register. */
            uint16_t      e_cksum;                   /**< 0x12 checksum; 16-bit sum of all words in file should be zero
                                                      *        (usually not filled in). */
            uint16_t      e_ip;                      /**< 0x14 initial value for IP register. */
            uint16_t      e_cs;                      /**< 0x16 initial value for CS register relative to program load segment. */
            uint16_t      e_relocs_offset;           /**< 0x18 file address of relocation table. */
            uint16_t      e_overlay;                 /**< 0x1a overlay number (zero indicates main program). */
            uint32_t      e_res1;                    /**< 0x1c unknown purpose. */
        }                                            /* 0x20 */
#if !defined(SWIG) && !defined(_MSC_VER)
        __attribute__((packed))
#endif
        ;

        struct DOSRelocEntry_disk {
            uint16_t      offset;                       /* 0x00 */
            uint16_t      segment;                      /* 0x02 */
        }                                               /* 0x04 */
#if !defined(SWIG) && !defined(_MSC_VER)
        __attribute__((packed))
#endif
        ;

#ifdef _MSC_VER
# pragma pack ()
#endif

        explicit SgAsmDOSFileHeader(SgAsmGenericFile *f)
            : SgAsmGenericHeader(f), p_e_last_page_size(0), p_e_total_pages(0), p_e_nrelocs(0), p_e_header_paragraphs(0),
              p_e_minalloc(0), p_e_maxalloc(0), p_e_ss(0), p_e_sp(0), p_e_cksum(0), p_e_ip(0), p_e_cs(0), p_e_overlay(0),
              p_e_relocs_offset(0), p_e_res1(0), p_relocs(NULL), p_rm_section(NULL) {
            ctor();
        }
        virtual SgAsmDOSFileHeader *parse() $ROSE_OVERRIDE {return parse(true);}
        SgAsmDOSFileHeader *parse(bool define_rm_section);
        virtual bool reallocate() $ROSE_OVERRIDE;
        virtual void unparse(std::ostream&) const $ROSE_OVERRIDE;
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const $ROSE_OVERRIDE;
        virtual const char *format_name() const $ROSE_OVERRIDE {return "DOS";}

        /** Parses the DOS real-mode text+data section and adds it to the AST.
         *
         *  If max_offset is non-zero then use that as the maximum offset of the real-mode section. If the DOS header indicates
         *  a zero sized section then return NULL. If the section exists or is zero size due to the max_offset then return the
         *  section. See also, update_from_rm_section(). */
        SgAsmGenericSection *parse_rm_section(rose_addr_t max_offset=0);

        /** Update DOS header with data from real-mode section.
         *
         *  The DOS real-mode data+text section is assumed to appear immediately after the DOS Extended Header, which appears
         *  immediately after the DOS File Header, which appears at the beginning of the file. These assumptions are not
         *  checked until SgAsmDOSFileHeader::unparse() is called. See also, @ref parse_rm_section. */
        void update_from_rm_section();

        /** Returns true if a cursory look at the file indicates that it could be a DOS executable file. */
        static bool is_DOS(SgAsmGenericFile*);

    private:
        void ctor();
        void *encode(SgAsmDOSFileHeader::DOSFileHeader_disk*) const;
#endif // SgAsmDOSFileHeader_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmDOSExtendedHeader);
    IS_SERIALIZABLE(AsmDOSExtendedHeader);

#ifdef DOCUMENTATION
    /** DOS exteded header.
     *
     *  Most of the properties correspond to those defined in the PE/DOS specifications and that documentation is not
     *  repeated here. */
    class SgAsmDOSExtendedHeader: public SgAsmGenericSection {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Rerserved area 1.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_res1() const;
        void set_e_res1(unsigned);
        /** @} */
#else
        AsmDOSExtendedHeader.setDataPrototype("unsigned", "e_res1", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: OEM ID.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_oemid() const;
        void set_e_oemid(unsigned);
        /** @} */
#else
        AsmDOSExtendedHeader.setDataPrototype("unsigned", "e_oemid", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: OEM info.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_res1() const;
        void set_e_res1(unsigned);
        /** @} */
#else
        AsmDOSExtendedHeader.setDataPrototype("unsigned", "e_oeminfo", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Rerserved area 2.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_res2() const;
        void set_e_res2(unsigned);
        /** @} */
#else
        AsmDOSExtendedHeader.setDataPrototype("unsigned", "e_res2", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Rerserved area 3.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_res3() const;
        void set_e_res3(unsigned);
        /** @} */
#else
        AsmDOSExtendedHeader.setDataPrototype("unsigned", "e_res3", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Rerserved area 4.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_res4() const;
        void set_e_res4(unsigned);
        /** @} */
#else
        AsmDOSExtendedHeader.setDataPrototype("unsigned", "e_res4", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Rerserved area 5.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_res5() const;
        void set_e_res5(unsigned);
        /** @} */
#else
        AsmDOSExtendedHeader.setDataPrototype("unsigned", "e_res5", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Rerserved area 6.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_res6() const;
        void set_e_res6(unsigned);
        /** @} */
#else
        AsmDOSExtendedHeader.setDataPrototype("unsigned", "e_res6", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: lfanew.
         *
         *  See PE specification.
         *
         * @{ */
        rose_addr_t get_e_lfanew() const;
        void set_e_lfanew(rose_addr_t);
        /** @} */
#else
        AsmDOSExtendedHeader.setDataPrototype("rose_addr_t", "e_lfanew", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmDOSExtendedHeader);
#if defined(SgAsmDOSExtendedHeader_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmGenericSection);
            s & BOOST_SERIALIZATION_NVP(p_e_res1);
            s & BOOST_SERIALIZATION_NVP(p_e_oemid);
            s & BOOST_SERIALIZATION_NVP(p_e_oeminfo);
            s & BOOST_SERIALIZATION_NVP(p_e_res2);
            s & BOOST_SERIALIZATION_NVP(p_e_res3);
            s & BOOST_SERIALIZATION_NVP(p_e_res4);
            s & BOOST_SERIALIZATION_NVP(p_e_res5);
            s & BOOST_SERIALIZATION_NVP(p_e_res6);
            s & BOOST_SERIALIZATION_NVP(p_e_lfanew);
        }
#endif

    public:
#ifdef _MSC_VER
# pragma pack (1)
#endif
        struct DOSExtendedHeader_disk {
            uint32_t      e_res1;     /* 0x00 reserved */
            uint16_t      e_oemid;    /* 0x04 OEM Identifier */
            uint16_t      e_oeminfo;  /* 0x06 other OEM information; oemid specific */
            uint32_t      e_res2;     /* 0x08 reserved */
            uint32_t      e_res3;     /* 0x0c reserved */
            uint32_t      e_res4;     /* 0x10 reserved */
            uint32_t      e_res5;     /* 0x14 reserved */
            uint32_t      e_res6;     /* 0x18 reserved */
            uint32_t      e_lfanew;   /* 0x1c file offset of new exe (PE) header */
        }                                 /* 0x20 */
#if !defined(SWIG) && !defined(_MSC_VER)
        __attribute__((packed))
#endif
        ;
#ifdef _MSC_VER
# pragma pack ()
#endif

        explicit SgAsmDOSExtendedHeader(SgAsmDOSFileHeader *fhdr)
            : SgAsmGenericSection(fhdr->get_file(), fhdr), p_e_res1(0), p_e_oemid(0), p_e_oeminfo(0), p_e_res2(0),
              p_e_res3(0), p_e_res4(0), p_e_res5(0), p_e_res6(0), p_e_lfanew(0) {
            ctor();
        }
        virtual SgAsmDOSExtendedHeader *parse() $ROSE_OVERRIDE;
        void *encode(SgAsmDOSExtendedHeader::DOSExtendedHeader_disk*) const;
        virtual void unparse(std::ostream&) const $ROSE_OVERRIDE;
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const $ROSE_OVERRIDE;

    private:
        void ctor();
#endif // SgAsmDOSExtendedHeader_OTHERS

#ifdef DOCUMENTATION
    };
#endif


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*************************************************************************************************************************
     *                                         PE File Header
     *************************************************************************************************************************/

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmPEFileHeader);
    IS_SERIALIZABLE(AsmPEFileHeader);

#ifdef DOCUMENTATION
    /** Windows PE file header.
     *
     *  Most of the properties come directly from the PE specification and their documentation is not repeated here. */
    class SgAsmPEFileHeader: public SgAsmGenericHeader {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: CPU type.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_cpu_type() const;
        void set_e_cpu_type(unsigned);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("unsigned", "e_cpu_type", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Number of sections.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_nsections() const;
        void set_e_nsections(unsigned);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("unsigned", "e_nsections", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Time.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_time() const;
        void set_e_time(unsigned);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("unsigned", "e_time", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: COFF symbol table.
         *
         *  See PE specification.
         *
         * @{ */
        rose_addr_t get_e_coff_symtab() const;
        void set_e_coff_symtab(rose_addr_t);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("rose_addr_t", "e_coff_symtab", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Size of NT header.
         *
         *  See PE specification.
         *
         * @{ */
        rose_addr_t get_e_nt_hdr_size() const;
        void set_e_nt_hdr_size(rose_addr_t);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("rose_addr_t", "e_nt_hdr_size", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Number of COFF symbols.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_coff_nsyms() const;
        void set_e_coff_nsyms(unsigned);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("unsigned", "e_coff_nsyms", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Flags
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_flags() const;
        void set_e_flags(unsigned);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("unsigned", "e_flags", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Magic.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_opt_magic() const;
        void set_e_opt_magic(unsigned);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("unsigned", "e_opt_magic", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: lmajor.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_lmajor() const;
        void set_e_lmajor(unsigned);

#else/** @} */
        AsmPEFileHeader.setDataPrototype("unsigned", "e_lmajor", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: lminor.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_lminor() const;
        void set_e_lminor(unsigned);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("unsigned", "e_lminor", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Code size.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_code_size() const;
        void set_e_code_size(unsigned);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("unsigned", "e_code_size", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Data size.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_data_size() const;
        void set_e_data_size(unsigned);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("unsigned", "e_data_size", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: BSS size.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_bss_size() const;
        void set_e_bss_size(unsigned);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("unsigned", "e_bss_size", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Code RVA.
         *
         *  See PE specification.
         *
         * @{ */
        rose_rva_t get_e_code_rva() const;
        void set_e_code_rva(rose_rva_t);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("rose_rva_t", "e_code_rva", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Data RVA.
         *
         *  See PE specification.
         *
         * @{ */
        rose_rva_t get_e_data_rva() const;
        void set_e_data_rva(rose_rva_t);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("rose_rva_t", "e_data_rva", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Section alignment.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_section_align() const;
        void set_e_section_align(unsigned);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("unsigned", "e_section_align", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: File alignment.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_file_align() const;
        void set_e_file_align(unsigned);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("unsigned", "e_file_align", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: OS major number.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_os_major() const;
        void set_e_os_major(unsigned);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("unsigned", "e_os_major", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: OS minor number.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_os_minor() const;
        void set_e_os_minor(unsigned);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("unsigned", "e_os_minor", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: User major number.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_user_major() const;
        void set_e_user_major(unsigned);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("unsigned", "e_user_major", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: User minor number.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_user_minor() const;
        void set_e_user_minor(unsigned);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("unsigned", "e_user_minor", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Subsystem major number.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_subsys_major() const;
        void set_e_subsys_major(unsigned);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("unsigned", "e_subsys_major", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Subsystem minor number.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_subsys_minor() const;
        void set_e_subsys_minor(unsigned);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("unsigned", "e_subsys_minor", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Reserved area #9.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_reserved9() const;
        void set_e_reserved9(unsigned);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("unsigned", "e_reserved9", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Image size.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_image_size() const;
        void set_e_image_size(unsigned);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("unsigned", "e_image_size", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Header size.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_header_size() const;
        void set_e_header_size(unsigned);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("unsigned", "e_header_size", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: File checksum.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_file_checksum() const;
        void set_e_file_checksum(unsigned);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("unsigned", "e_file_checksum", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Subsystem.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_subsystem() const;
        void set_e_subsystem(unsigned);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("unsigned", "e_subsystem", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: DLL flags.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_dll_flags() const;
        void set_e_dll_flags(unsigned);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("unsigned", "e_dll_flags", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Stack reserve size.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_stack_reserve_size() const;
        void set_e_stack_reserve_size(unsigned);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("unsigned", "e_stack_reserve_size", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Stack commit size.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_stack_commit_size() const;
        void set_e_stack_commit_size(unsigned);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("unsigned", "e_stack_commit_size", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Heap reserve size.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_heap_reserve_size() const;
        void set_e_heap_reserve_size(unsigned);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("unsigned", "e_heap_reserve_size", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Heap commit size.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_heap_commit_size() const;
        void set_e_heap_commit_size(unsigned);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("unsigned", "e_heap_commit_size", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Loader flags.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_loader_flags() const;
        void set_e_loader_flags(unsigned);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("unsigned", "e_loader_flags", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Number of RVA/size pairs.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_e_num_rvasize_pairs() const;
        void set_e_num_rvasize_pairs(unsigned);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("unsigned", "e_num_rvasize_pairs", "= 0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: RVA/size pairs.
         *
         *  See PE specification.
         *
         * @{ */
        SgAsmPERVASizePairList* get_rvasize_pairs() const;
        void set_rvasize_pairs(SgAsmPERVASizePairList*);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("SgAsmPERVASizePairList*", "rvasize_pairs", "= NULL",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Section table.
         *
         *  See PE specification.
         *
         * @{ */
        SgAsmPESectionTable* get_section_table() const;
        void set_section_table(SgAsmPESectionTable*);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("SgAsmPESectionTable*", "section_table", "= NULL",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: COFF symbol table.
         *
         *  See PE specification.
         *
         * @{ */
        SgAsmCoffSymbolTable* get_coff_symtab() const;
        void set_coff_symtab(SgAsmCoffSymbolTable*);
        /** @} */
#else
        AsmPEFileHeader.setDataPrototype("SgAsmCoffSymbolTable*", "coff_symtab", "= NULL",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmPEFileHeader);
#if defined(SgAsmPEFileHeader_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmGenericHeader);
            s & BOOST_SERIALIZATION_NVP(p_e_cpu_type);
            s & BOOST_SERIALIZATION_NVP(p_e_nsections);
            s & BOOST_SERIALIZATION_NVP(p_e_time);
            s & BOOST_SERIALIZATION_NVP(p_e_coff_symtab);
            s & BOOST_SERIALIZATION_NVP(p_e_nt_hdr_size);
            s & BOOST_SERIALIZATION_NVP(p_e_coff_nsyms);
            s & BOOST_SERIALIZATION_NVP(p_e_flags);
            s & BOOST_SERIALIZATION_NVP(p_e_opt_magic);
            s & BOOST_SERIALIZATION_NVP(p_e_lmajor);
            s & BOOST_SERIALIZATION_NVP(p_e_lminor);
            s & BOOST_SERIALIZATION_NVP(p_e_code_size);
            s & BOOST_SERIALIZATION_NVP(p_e_data_size);
            s & BOOST_SERIALIZATION_NVP(p_e_bss_size);
            s & BOOST_SERIALIZATION_NVP(p_e_code_rva);
            s & BOOST_SERIALIZATION_NVP(p_e_data_rva);
            s & BOOST_SERIALIZATION_NVP(p_e_section_align);
            s & BOOST_SERIALIZATION_NVP(p_e_file_align);
            s & BOOST_SERIALIZATION_NVP(p_e_os_major);
            s & BOOST_SERIALIZATION_NVP(p_e_os_minor);
            s & BOOST_SERIALIZATION_NVP(p_e_user_major);
            s & BOOST_SERIALIZATION_NVP(p_e_user_minor);
            s & BOOST_SERIALIZATION_NVP(p_e_subsys_major);
            s & BOOST_SERIALIZATION_NVP(p_e_subsys_minor);
            s & BOOST_SERIALIZATION_NVP(p_e_reserved9);
            s & BOOST_SERIALIZATION_NVP(p_e_image_size);
            s & BOOST_SERIALIZATION_NVP(p_e_header_size);
            s & BOOST_SERIALIZATION_NVP(p_e_file_checksum);
            s & BOOST_SERIALIZATION_NVP(p_e_subsystem);
            s & BOOST_SERIALIZATION_NVP(p_e_dll_flags);
            s & BOOST_SERIALIZATION_NVP(p_e_stack_reserve_size);
            s & BOOST_SERIALIZATION_NVP(p_e_stack_commit_size);
            s & BOOST_SERIALIZATION_NVP(p_e_heap_reserve_size);
            s & BOOST_SERIALIZATION_NVP(p_e_heap_commit_size);
            s & BOOST_SERIALIZATION_NVP(p_e_loader_flags);
            s & BOOST_SERIALIZATION_NVP(p_e_num_rvasize_pairs);
            s & BOOST_SERIALIZATION_NVP(p_rvasize_pairs);
            s & BOOST_SERIALIZATION_NVP(p_section_table);
            s & BOOST_SERIALIZATION_NVP(p_coff_symtab);
        }
#endif

    public:
#ifdef _MSC_VER
# pragma pack (1)
#endif
        /* File format of a PE File Header. All fields are little endian. */
        struct PEFileHeader_disk {
            unsigned char e_magic[4];       /* 0x00 magic number "PE\0\0" */
            uint16_t    e_cpu_type;         /* 0x04 e.g., 0x014c = Intel 386 */
            uint16_t    e_nsections;        /* 0x06 number of sections defined in the Section Table */
            uint32_t    e_time;             /* 0x08 time and date file was created or modified by the linker */
            uint32_t    e_coff_symtab;      /* 0x0c offset to COFF symbol table */
            uint32_t    e_coff_nsyms;       /* 0x10 number of symbols in COFF symbol table */
            uint16_t    e_nt_hdr_size;      /* 0x14 num remaining bytes in the header following the 'flags' field */
            uint16_t    e_flags;            /* 0x16 Bit flags: exe file, program/library image, fixed address, etc. */
        }                                       /* 0x18 */
#if !defined(SWIG) && !defined(_MSC_VER)
        __attribute__((packed))
#endif
        ;

        struct PE32OptHeader_disk {
            uint16_t    e_opt_magic;        /* 0x18 magic number */
            uint16_t    e_lmajor;           /* 0x1a linker version */
            uint16_t    e_lminor;           /* 0x1c */
            uint16_t    e_code_size;        /* 0x1e Size of .text or sum of all code sections */
            uint32_t    e_data_size;        /* 0x20 Sum size of initialized data */
            uint32_t    e_bss_size;         /* 0x24 Sum size of uninitialized data */
            uint32_t    e_entrypoint_rva;   /* 0x28 RVA="relative virtual address"; relative to 'image_base', below */
            uint32_t    e_code_rva;         /* 0x2c Addr relative to image base for code section when memory mapped */
            uint32_t    e_data_rva;         /* 0x30 Address relative to image base for data section */
            uint32_t    e_image_base;       /* 0x34 Virt base of image (first byte of file, DOS header). 64k aligned */
            uint32_t    e_section_align;    /* 0x38 Alignment of sections in memory. Power of two 512<=x<=256M */
            uint32_t    e_file_align;       /* 0x3c Alignment factor (in bytes) for image pages */
            uint16_t    e_os_major;         /* 0x40 OS version number required to run this image */
            uint16_t    e_os_minor;         /* 0x42 */
            uint16_t    e_user_major;       /* 0x44 User-specified for differentiating between image revs */
            uint16_t    e_user_minor;       /* 0x46 */
            uint16_t    e_subsys_major;     /* 0x48 Subsystem version number */
            uint16_t    e_subsys_minor;     /* 0x4a */
            uint32_t    e_reserved9;        /* 0x4c */
            uint32_t    e_image_size;       /* 0x50 Virtual size of the image inc. all headers; section_align */
            uint32_t    e_header_size;      /* 0x54 Total header size (DOS Header + PE Header + Section table */
            uint32_t    e_file_checksum;    /* 0x58 Checksum for entire file; Set to zero by the linker */
            uint16_t    e_subsystem;        /* 0x5c Native, WindowsGUI, WindowsCharacter, OS/2 Character, etc. */
            uint16_t    e_dll_flags;        /* 0x5e Bit flags for library init/terminate per process or thread */
            uint32_t    e_stack_reserve_size;/*0x60 Virtual mem reserved for stack; non-committed pages are guards */
            uint32_t    e_stack_commit_size;/* 0x64 Size of valid stack; other pages are guards; <=stack_reserve_size*/
            uint32_t    e_heap_reserve_size;/* 0x68 Size (bytes) of local heap to reserve */
            uint32_t    e_heap_commit_size; /* 0x6c Size (bytes) of valid local heap */
            uint32_t    e_loader_flags;     /* 0x70 Reserved, must be zero */
            uint32_t    e_num_rvasize_pairs;/* 0x74 Num RVASizePair entries that follow this member; part of header */
        }                                       /* 0x78 */
#if !defined(SWIG) && !defined(_MSC_VER)
        __attribute__((packed))
#endif
        ;

        struct PE64OptHeader_disk {
            uint16_t    e_opt_magic;        /* 0x18 */
            uint16_t    e_lmajor;           /* 0x1a */
            uint16_t    e_lminor;           /* 0x1c */
            uint16_t    e_code_size;        /* 0x1e */
            uint32_t    e_data_size;        /* 0x20 */
            uint32_t    e_bss_size;         /* 0x24 */
            uint32_t    e_entrypoint_rva;   /* 0x28 */
            uint32_t    e_code_rva;         /* 0x2c */
            // uint32_t  e_data_rva;             /* Not present in PE32+ */
            uint64_t    e_image_base;       /* 0x30 */
            uint32_t    e_section_align;    /* 0x38 */
            uint32_t    e_file_align;       /* 0x3c */
            uint16_t    e_os_major;         /* 0x40 */
            uint16_t    e_os_minor;         /* 0x42 */
            uint16_t    e_user_major;       /* 0x44 */
            uint16_t    e_user_minor;       /* 0x46 */
            uint16_t    e_subsys_major;     /* 0x48 */
            uint16_t    e_subsys_minor;     /* 0x4a */
            uint32_t    e_reserved9;        /* 0x4c */
            uint32_t    e_image_size;       /* 0x50 */
            uint32_t    e_header_size;      /* 0x54 */
            uint32_t    e_file_checksum;    /* 0x58 */
            uint16_t    e_subsystem;        /* 0x5c */
            uint16_t    e_dll_flags;        /* 0x5e */
            uint64_t    e_stack_reserve_size;/*0x60 */
            uint64_t    e_stack_commit_size;/* 0x68 */
            uint64_t    e_heap_reserve_size;/* 0x70 */
            uint64_t    e_heap_commit_size; /* 0x78 */
            uint32_t    e_loader_flags;     /* 0x80 */
            uint32_t    e_num_rvasize_pairs;/* 0x84 */
        }                                       /* 0x88 */
#if !defined(SWIG) && !defined(_MSC_VER)
        __attribute__((packed))
#endif
            ;
#ifdef _MSC_VER
# pragma pack ()
#endif

        /* Bit flags for the PE header 'flags' member */
        enum HeaderFlags {
            HF_PROGRAM          = 0x0000,       /* Program image (no non-reserved bits set) */
            HF_EXECUTABLE       = 0x0002,       /* Clear indicates can't load: link errors or incrementally linked */
            HF_FIXED            = 0x0200,       /* Image *must* be loaded at image_base address or error */
            HF_LIBRARY          = 0x2000,       /* Library image */
            HF_RESERVED_MASK    = 0xddfd        /* Reserved bits */
        };

        /* Values for the PE header 'subsystem' member */
        enum Subsystem {
            HF_SPEC_UNKNOWN     = 0x0000,       /* Specified as 'unknown' in the file */
            HF_NATIVE           = 0x0001,       /* Native */
            HF_WINGUI           = 0x0002,       /* Windows GUI */
            HF_WINCHAR          = 0x0003,       /* Windows character */
            HF_OS2CHAR          = 0x0005,       /* OS/2 character */
            HF_POSIX            = 0x0007        /* POSIX character */
        };

        /* Bit flags for the PE header 'dll_flags' member */
        enum DLLFlags {
            DLL_PROC_INIT       = 0x0001,       /* Per-process library initialization */
            DLL_PROC_TERM       = 0x0002,       /* Per-process library termination */
            DLL_THRD_INIT       = 0x0004,       /* Per-thread library initialization */
            DLL_THRD_TERM       = 0x0008,       /* Per-thread library termination */
            DLL_RESERVED_MASK   = 0xfff0        /* Reserved bits */
        };

        /** Reason for each rva/size pair in the PE header. */
        enum PairPurpose {              // Values are important
            PAIR_EXPORTS            = 0,
            PAIR_IMPORTS            = 1,
            PAIR_RESOURCES          = 2,
            PAIR_EXCEPTIONS         = 3,
            PAIR_CERTIFICATES       = 4,
            PAIR_BASERELOCS         = 5,
            PAIR_DEBUG              = 6,
            PAIR_ARCHITECTURE       = 7,
            PAIR_GLOBALPTR          = 8,
            PAIR_TLS                = 9,    // Thread local storage
            PAIR_LOADCONFIG         = 10,
            PAIR_BOUNDIMPORT        = 11,
            PAIR_IAT                = 12,   // Import address table
            PAIR_DELAYIMPORT        = 13,   // Delay import descriptor
            PAIR_CLRRUNTIME         = 14,   // CLR(?) runtime header
            PAIR_RESERVED15         = 15    // Reserved (always zero according to specification)
        };

    public:
        explicit SgAsmPEFileHeader(SgAsmGenericFile *f)
            : SgAsmGenericHeader(f), p_e_cpu_type(0), p_e_nsections(0), p_e_time(0), p_e_coff_symtab(0), p_e_nt_hdr_size(0),
              p_e_coff_nsyms(0), p_e_flags(0), p_e_opt_magic(0), p_e_lmajor(0), p_e_lminor(0), p_e_code_size(0), p_e_data_size(0),
              p_e_bss_size(0), p_e_code_rva(0), p_e_data_rva(0), p_e_section_align(0), p_e_file_align(0), p_e_os_major(0),
              p_e_os_minor(0), p_e_user_major(0), p_e_user_minor(0), p_e_subsys_major(0), p_e_subsys_minor(0), p_e_reserved9(0),
              p_e_image_size(0), p_e_header_size(0), p_e_file_checksum(0), p_e_subsystem(0), p_e_dll_flags(0),
              p_e_stack_reserve_size(0), p_e_stack_commit_size(0), p_e_heap_reserve_size(0), p_e_heap_commit_size(0),
              p_e_loader_flags(0), p_e_num_rvasize_pairs(0), p_rvasize_pairs(NULL), p_section_table(NULL), p_coff_symtab(NULL) {
            ctor();
        }

        virtual const char *format_name() const $ROSE_OVERRIDE {return "PE";}

        /** Return true if the file looks like it might be a PE file according to the magic number.
         *
         *  The file must contain what appears to be a DOS File Header at address zero, and what appears to be a PE File Header
         *  at a file offset specified in part of the DOS File Header (actually, in the bytes that follow the DOS File
         *  Header). */
        static bool is_PE (SgAsmGenericFile*);

        /** Convert an RVA/Size Pair index number into a section name.
         *
         *  This is different than @ref stringifySgAsmPEFileHeaderPairPurpose because it returns a section name rather than an
         *  enum name. */
        std::string rvasize_pair_name(PairPurpose, const char **short_name);

        /** Define an RVA/Size pair in the PE file header. */
        void set_rvasize_pair(PairPurpose, SgAsmPESection*);

        /** Update all the RVA/Size pair info from the section to which it points. */
        void update_rvasize_pairs();

        void add_rvasize_pairs();

        virtual SgAsmPEFileHeader *parse() $ROSE_OVERRIDE;
        virtual bool reallocate() $ROSE_OVERRIDE;
        virtual void unparse(std::ostream&) const $ROSE_OVERRIDE;
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const $ROSE_OVERRIDE;
        void create_table_sections();

        /* Loader memory maps */
        Rose::BinaryAnalysis::MemoryMap::Ptr get_loader_map() const {return p_loader_map;}
        void set_loader_map(const Rose::BinaryAnalysis::MemoryMap::Ptr &map) {p_loader_map=map;}

    private:
        void ctor();
        void *encode(SgAsmPEFileHeader::PEFileHeader_disk*) const;
        void *encode(SgAsmPEFileHeader::PE32OptHeader_disk*) const;
        void *encode(SgAsmPEFileHeader::PE64OptHeader_disk*) const;
        Rose::BinaryAnalysis::MemoryMap::Ptr p_loader_map;
#endif // SgAsmPEFileHeader_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmPERVASizePairList);
    IS_SERIALIZABLE(AsmPERVASizePairList);

#ifdef DOCUMENTATION
    /** List of SgAsmPERVASizePair AST nodes.
     *
     *  The only reason this AST node exists rather than storing the list directly in the nodes that need it is due to ROSETTA
     *  limitations. */
    class SgAsmPERVASizePairList: public SgAsmExecutableFileFormat {
    public:
#endif


#ifdef DOCUMENTATION
        /** Property: List of pointers to other nodes.
         *
         * @{ */
        const SgAsmPERVASizePairPtrList& get_pairs() const;
        void set_pairs(const SgAsmPERVASizePairPtrList&);
        /** @} */
#else
        AsmPERVASizePairList.setDataPrototype("SgAsmPERVASizePairPtrList", "pairs", "",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmPERVASizePairList);
#if defined(SgAsmPERVASizePairList_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_pairs);
        }
#endif

    public:
        explicit SgAsmPERVASizePairList(SgAsmPEFileHeader *parent) {
            set_parent(parent);
        }
#endif // SgAsmPERVASizePairList_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmPERVASizePair);
    IS_SERIALIZABLE(AsmPERVASizePair);

#ifdef DOCUMENTATION
    /** RVA/size pair. */
    class SgAsmPERVASizePair: public SgAsmExecutableFileFormat {
    public:
#endif


#ifdef DOCUMENTATION
        /** Property: RVA.
         *
         *  See PE specification.
         *
         * @{ */
        rose_rva_t get_e_rva() const;
        void set_e_rva(rose_rva_t);
        /** @} */
#else
        AsmPERVASizePair.setDataPrototype("rose_rva_t", "e_rva", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Size.
         *
         *  See PE specification.
         *
         * @{ */
        rose_addr_t get_e_size() const;
        void set_e_size(rose_addr_t);
        /** @} */
#else
        AsmPERVASizePair.setDataPrototype("rose_addr_t", "e_size", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Section.
         *
         *  See PE specification.
         *
         * @{ */
        SgAsmGenericSection* get_section() const;
        void set_section(SgAsmGenericSection*);
        /** @} */
#else
        AsmPERVASizePair.setDataPrototype("SgAsmGenericSection*", "section", "= NULL",
                                          NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmPERVASizePair);
#if defined(SgAsmPERVASizePair_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_e_rva);
            s & BOOST_SERIALIZATION_NVP(p_e_size);
            s & BOOST_SERIALIZATION_NVP(p_section);
        }
#endif


    public:
#ifdef _MSC_VER
# pragma pack (1)
#endif
        /** File format for an RVA/Size pair.
         *
         *  Such pairs are considered to be part of the PE file header.  All fields are little endian. */
        struct RVASizePair_disk {
            uint32_t    e_rva;
            uint32_t    e_size;
        }
#if !defined(SWIG) && !defined(_MSC_VER)
        __attribute__((packed))
#endif
        ;
#ifdef _MSC_VER
# pragma pack ()
#endif

        SgAsmPERVASizePair(SgAsmPERVASizePairList *parent, const SgAsmPERVASizePair::RVASizePair_disk *disk)
            : p_e_rva(0), p_e_size(0), p_section(NULL) {
            ctor(parent, disk);
        }

        SgAsmPERVASizePair(SgAsmPERVASizePairList *parent, rose_addr_t rva, rose_addr_t size)
            : p_e_rva(0), p_e_size(0), p_section(NULL) {
            ctor(parent, rva, size);
        }

        /** Sets or removes the section associated with an RVA/size pair.
         *
         *  Setting or removing the section also updates the RVA and size according to the preferred mapping address and mapped
         *  size of the section. */
        void set_section(SgAsmGenericSection *section);

        /** Returns the section associated with an RVA/size pair.
         *
         *  This is the same as the ROSETTA-generated accessor, but we need a custom version of set_section(). */
        SgAsmGenericSection *get_section() const;

        void *encode(SgAsmPERVASizePair::RVASizePair_disk *disk) const;
    private:
        void ctor(SgAsmPERVASizePairList *parent, const SgAsmPERVASizePair::RVASizePair_disk*);
        void ctor(SgAsmPERVASizePairList *parent, rose_addr_t rva, rose_addr_t size);
#endif // SgAsmPERVASizePair_OTHERS

#ifdef DOCUMENTATION
    };
#endif



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*************************************************************************************************************************
     *                                         PE Import Tables
     *************************************************************************************************************************/

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmPEImportItem);
    IS_SERIALIZABLE(AsmPEImportItem);

#ifdef DOCUMENTATION
    /** A single imported object.
     *
     *  Each of these nodes is created from a combination of the PE Import Lookup Table and the PE Import Address Table,
     *  pointers to which are stored in the Import Directory (@ref SgAsmPEImportDirectory). This node represents a single
     *  import object described by data structures referenced by a PE Import Directory.  Such a node represents data from two,
     *  possibly three, distinct data structures in the PE file: (1) An entry in the Import Lookup Table, (2) an entry in the
     *  Import Address Table, and (3) an optional Hint/Name pair in the (implicit) Hint/Name Table. */
    class SgAsmPEImportItem: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Whether ordinal number is stored.
         *
         *  See PE specification.
         *
         * @{ */
        bool get_by_ordinal() const;
        void set_by_ordinal(bool);
        /** @} */
#else
        AsmPEImportItem.setDataPrototype("bool", "by_ordinal", "=true",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Ordinal number
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_ordinal() const;
        void set_ordinal(unsigned);
        /** @} */
#else
        AsmPEImportItem.setDataPrototype("unsigned", "ordinal", "=0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Export name table hint if not ordinal.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_hint() const;
        void set_hint(unsigned);
        /** @} */
#else
        AsmPEImportItem.setDataPrototype("unsigned", "hint", "=0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Name if not by ordinal.
         *
         *  See PE specification.
         *
         * @{ */
        SgAsmGenericString* get_name() const;
        void set_name(SgAsmGenericString*);
        /** @} */
#else
        AsmPEImportItem.setDataPrototype("SgAsmGenericString*", "name", "",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: RVA of hint/name pair.
         *
         *  See PE specification.
         *
         * @{ */
        rose_rva_t get_hintname_rva() const;
        void set_hintname_rva(rose_rva_t);
        /** @} */
#else
        AsmPEImportItem.setDataPrototype("rose_rva_t", "hintname_rva", "=0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: File bytes allocated for hint/name pair.
         *
         *  See PE specification.
         *
         * @{ */
        size_t get_hintname_nalloc() const;
        void set_hintname_nalloc(size_t);
        /** @} */
#else
        AsmPEImportItem.setDataPrototype("size_t", "hintname_nalloc", "=0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Bound RVA from IAT if not a copy of ILT.
         *
         *  See PE specification.
         *
         * @{ */
        rose_rva_t get_bound_rva() const;
        void set_bound_rva(rose_rva_t);
        /** @} */
#else
        AsmPEImportItem.setDataPrototype("rose_rva_t", "bound_rva", "=0",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmPEImportItem);
#if defined(SgAsmPEImportItem_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_by_ordinal);
            s & BOOST_SERIALIZATION_NVP(p_ordinal);
            s & BOOST_SERIALIZATION_NVP(p_hint);
            s & BOOST_SERIALIZATION_NVP(p_name);
            s & BOOST_SERIALIZATION_NVP(p_hintname_rva);
            s & BOOST_SERIALIZATION_NVP(p_hintname_nalloc);
            s & BOOST_SERIALIZATION_NVP(p_bound_rva);
        }
#endif

    public:
        explicit SgAsmPEImportItem(SgAsmPEImportItemList *parent) {
            ctor(parent);
        }
        explicit SgAsmPEImportItem(SgAsmPEImportDirectory *idir) {
            ctor(idir);
        }
        SgAsmPEImportItem(SgAsmPEImportDirectory *idir, const std::string &name, unsigned hint=0) {
            ctor(idir, name, hint);
        }
        SgAsmPEImportItem(SgAsmPEImportDirectory *idir, const unsigned ordinal) {
            ctor(idir, ordinal);
        }
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const;

        /** Bytes needed to store hint/name pair.
         *
         *  A hint/name pair consists of a two-byte, little endian, unsigned hint and a NUL-terminated ASCII string.  An
         *  optional zero byte padding appears after the string's NUL terminator if necessary to make the total size of the
         *  hint/name pair a multiple of two. */
        size_t hintname_required_size() const;

        /** Virtual address of an IAT entry.
         *
         *  Returns the virtual address of the IAT slot for this import item.  This import item must be linked into the AST in
         *  order for this method to succeed. */
        rose_addr_t get_iat_entry_va() const;

    private:
        void ctor(SgAsmPEImportItemList *parent);
        void ctor(SgAsmPEImportDirectory*);
        void ctor(SgAsmPEImportDirectory*, const std::string &name, unsigned hint);
        void ctor(SgAsmPEImportDirectory*, unsigned ordinal);
#endif // SgAsmPEImportItem_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmPEImportItemList);
    IS_SERIALIZABLE(AsmPEImportItemList);

#ifdef DOCUMENTATION
    /** A list of imported items.
     *
     *  The only purpose of this node is to hold a list of pointers to other nodes. This list cannot be contained directly in
     *  the nodes that need it due to ROSETTA limitations. */
    class SgAsmPEImportItemList: public SgAsmExecutableFileFormat {
    public:
#endif


#ifdef DOCUMENTATION
        /** Property: List of pointers to other nodes.
         *
         * @{ */
        const SgAsmPEImportItemPtrList& get_vector() const;
        void set_vector(const SgAsmPEImportItemPtrList&);
        /** @} */
#else
        AsmPEImportItemList.setDataPrototype("SgAsmPEImportItemPtrList", "vector", "",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmPEImportItemList);
#if defined(SgAsmPEImportItemList_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_vector);
        }
#endif
#endif // SgAsmPEImportItemList_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmPEImportDirectory);
    IS_SERIALIZABLE(AsmPEImportDirectory);

#ifdef DOCUMENTATION
    /** One import directory per library.
     *
     *  The import directory points to the Import Lookup Table and Import Address Table. ROSE no longer represents the ILT and
     *  IAT with separate IR nodes. */
    class SgAsmPEImportDirectory: public SgAsmExecutableFileFormat {
    public:
#endif


#ifdef DOCUMENTATION
        /** Property: Name of library from which to import.
         *
         *  See PE specification.
         *
         * @{ */
        SgAsmGenericString* get_dll_name() const;
        void set_dll_name(SgAsmGenericString*);
        /** @} */
#else
        AsmPEImportDirectory.setDataPrototype("SgAsmGenericString*", "dll_name", "=0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Address of the import library name.
         *
         *  See PE specification.
         *
         * @{ */
        rose_rva_t get_dll_name_rva() const;
        void set_dll_name_rva(rose_rva_t);
        /** @} */
#else
        AsmPEImportDirectory.setDataPrototype("rose_rva_t", "dll_name_rva", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Bytes allocated in the file for the name.
         *
         *  See PE specification.
         *
         * @{ */
        size_t get_dll_name_nalloc() const;
        void set_dll_name_nalloc(size_t);
        /** @} */
#else
        AsmPEImportDirectory.setDataPrototype("size_t", "dll_name_nalloc", "=0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Time.
         *
         *  See PE specification.
         *
         * @{ */
        time_t get_time() const;
        void set_time(time_t);
        /** @} */
#else
        AsmPEImportDirectory.setDataPrototype("time_t", "time", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Forwarder chain.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_forwarder_chain() const;
        void set_forwarder_chain(unsigned);
        /** @} */
#else
        AsmPEImportDirectory.setDataPrototype("unsigned", "forwarder_chain", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Starting address of the ILT.
         *
         *  See PE specification.
         *
         * @{ */
        rose_rva_t get_ilt_rva() const;
        void set_ilt_rva(rose_rva_t);
        /** @} */
#else
        AsmPEImportDirectory.setDataPrototype("rose_rva_t", "ilt_rva", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Bytes allocated in the file for the ILT.
         *
         *  See PE specification.
         *
         * @{ */
        size_t get_ilt_nalloc() const;
        void set_ilt_nalloc(size_t);
        /** @} */
#else
        AsmPEImportDirectory.setDataPrototype("size_t", "ilt_nalloc", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: IAT RVA.
         *
         *  See PE specification.
         *
         * @{ */
        rose_rva_t get_iat_rva() const;
        void set_iat_rva(rose_rva_t);
        /** @} */
#else
        AsmPEImportDirectory.setDataPrototype("rose_rva_t", "iat_rva", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Bytes allocated in the file for the IAT.
         *
         *  See PE specification.
         *
         * @{ */
        size_t get_iat_nalloc() const;
        void set_iat_nalloc(size_t);
        /** @} */
#else
        AsmPEImportDirectory.setDataPrototype("size_t", "iat_nalloc", "=0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: List of imports.
         *
         *  See PE specification.
         *
         * @{ */
        SgAsmPEImportItemList* get_imports() const;
        void set_imports(SgAsmPEImportItemList*);
        /** @} */
#else
        AsmPEImportDirectory.setDataPrototype("SgAsmPEImportItemList*", "imports", "=NULL",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmPEImportDirectory);
#if defined(SgAsmPEImportDirectory_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_dll_name);
            s & BOOST_SERIALIZATION_NVP(p_dll_name_rva);
            s & BOOST_SERIALIZATION_NVP(p_dll_name_nalloc);
            s & BOOST_SERIALIZATION_NVP(p_time);
            s & BOOST_SERIALIZATION_NVP(p_forwarder_chain);
            s & BOOST_SERIALIZATION_NVP(p_ilt_rva);
            s & BOOST_SERIALIZATION_NVP(p_ilt_nalloc);
            s & BOOST_SERIALIZATION_NVP(p_iat_rva);
            s & BOOST_SERIALIZATION_NVP(p_imports);
        }
#endif

    public:
#ifdef _MSC_VER
# pragma pack (1)
#endif
        struct PEImportDirectory_disk {
            uint32_t ilt_rva;               /* 0x00 Import Lookup Table RVA */
            uint32_t time;                  /* 0x04 Zero until the image is bound, then time stamp of the DLL */
            uint32_t forwarder_chain;       /* 0x08 Index of the first forwarder chain */
            uint32_t dll_name_rva;          /* 0x0c address of NUL-terminated DLL name */
            uint32_t iat_rva;               /* 0x10 Import Address Table (Thunk Table) RVA */
        }                                       /* 0x14 */
#if !defined(SWIG) && !defined(_MSC_VER)
        __attribute__((packed))
#endif
        ;
#ifdef _MSC_VER
# pragma pack ()
#endif

        explicit SgAsmPEImportDirectory(SgAsmPEImportSection *isec, std::string dll_name="")
            : p_dll_name(NULL), p_dll_name_rva(0), p_dll_name_nalloc(0), p_time(0), p_forwarder_chain(0),
              p_ilt_rva(0), p_ilt_nalloc(0), p_iat_rva(0), p_iat_nalloc(0), p_imports(NULL) {
            ctor(isec, dll_name);
        }

        /** Parse an import directory.
         *
         *  The import directory is parsed from the specified virtual address via the PE header's loader map. Return value is
         *  this directory entry on success, or the null pointer if the entry is all zero (which marks the end of the directory
         *  list). The @p isLastEntry is true if the caller thinks this should be an all-zero entry. */
        SgAsmPEImportDirectory *parse(rose_addr_t va, bool isLastEntry);

        /** Allocates space for this import directory's name, import lookup table, and import address table.
         *
         *  The items are allocated beginning at the specified relative virtual address. Items are reallocated if they are not
         *  allocated or if they are allocated in the same section to which start_rva points (the import section).  They are
         *  not reallocated if they already exist in some other section. The return value is the number of bytes allocated in
         *  the import section.  Upon return, this directory's address data members are initialized with possibly new
         *  values. */
        size_t reallocate(rose_rva_t starting_rva);

        /** Encode an import directory entry back into disk format */
        void *encode(SgAsmPEImportDirectory::PEImportDirectory_disk*) const;
        virtual void unparse(std::ostream&, const SgAsmPEImportSection*, size_t idx) const;
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const;

        /** Number of bytes required for the table.
         *
         *  Returns the number of bytes required for the entire IAT or ILT (including the zero terminator) as it is currently
         *  defined in the Import Directory.  The returned size does not include space required to store any Hint/Name pairs,
         *  which are outside the ILT/IAT but pointed to by the ILT/IAT. */
        size_t iat_required_size() const;

        /** Find an import item in an import directory.
         *
         *  Returns the index of the specified import item in this directory, or -1 if the import item is not a child of this
         *  directory.  The hint index is checked first. */
        int find_import_item(const SgAsmPEImportItem *item, int hint=0) const;

        /** Obtains the virtual address of the Hint/Name Table.
         *
         *  The Hint/Name Table is an implicit table--the PE file format specification talks about such a table, but it is not
         *  actually defined anywhere in the PE file.  Instead, various Import Lookup Table and Import Address Table entries
         *  might point to individual Hint/Name pairs, which collectively form an implicit Hint/Name Table.  There is no
         *  requirement that the Hint/Name pairs are contiguous in the address space, and indeed they often are not.
         *  Therefore, the only way to describe the location of the Hint/Name Table is by a list of addresses.
         *
         *  This function will scan this Import Directory's import items, observe which items make references to Hint/Name
         *  pairs that have known addresses, and add those areas of virtual memory to the specified extent map.  This function
         *  returns the number of ILT entries that reference a Hint/Name pair. */
        size_t hintname_table_extent(AddressIntervalSet &extent/*in,out*/) const;

    private:
        void ctor(SgAsmPEImportSection*, const std::string &dll_name);
        void parse_ilt_iat(const rose_rva_t &table_start, bool assume_bound);
        void unparse_ilt_iat(std::ostream&,const rose_rva_t &table_start, bool assume_bound, size_t nalloc) const;
#endif // SgAsmPEImportDirectory_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmPEImportDirectoryList);
    IS_SERIALIZABLE(AsmPEImportDirectoryList);

#ifdef DOCUMENTATION
    /** A list of PE Import Directories.
     *
     *  The only purpose of this class is to store a list of node pointers which cannot be stored in the nodes that need it due
     *  to ROSETTA limitations. */
    class SgAsmPEImportDirectoryList: public SgAsmExecutableFileFormat {
    public:
#endif


#ifdef DOCUMENTATION
        /** Property: List of nodes.
         *
         *  See PE specification.
         *
         * @{ */
        const SgAsmPEImportDirectoryPtrList& get_vector() const;
        void set_vector(const SgAsmPEImportDirectoryPtrList&);
        /** @} */
#else
        AsmPEImportDirectoryList.setDataPrototype("SgAsmPEImportDirectoryPtrList", "vector", "",
                                                  NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmPEImportDirectoryList);
#if defined(SgAsmPEImportDirectoryList_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_vector);
        }
#endif
#endif // SgAsmPEImportDirectoryList_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmPEImportSection);
    IS_SERIALIZABLE(AsmPEImportSection);

#ifdef DOCUMENTATION
   /** Portable Executable Import Section.
     *
     *  Constructs an SgAsmPEImportSection that represents either a PE ".idata" section as defined by the PE Section Table, or
     *  a PE Import Table as described by the RVA/Size pairs at the end of the NT Optional Header. The ".idata" section and PE
     *  Import Table both have the same format (only important fields shown):
     *
     *  @par Import Section
     *  An Import Section consists of a list of Import Directory Entries ("Directories"), one per dynamically linked library,
     *  followed by an all-zero Directory entry that marks the end of the list.  ROSE does not explicitly store the terminating
     *  entry, and wherever "Directories" appears in the following description it does not include this null directory.
     *
     *  @par Import Directory
     *  Each directory points to (by relative virtual address (RVA)) both an Import Lookup Table (ILT) and Import Address Table
     *  (IAT).
     *
     *  @par Import Lookup Table (and Import Address Table)
     *  The Import Lookup Table (ILT) and Import Address Table (IAT) have identical structure.  ROSE represents them as a list
     *  of SgAsmPEImportItem in the Import Directory.  The ILT and IAT are parallel arrays of 32- or 64-bit (PE32 or PE32+)
     *  entries terminated with an all-zero entry.  The terminating entry is not stored explicitly by ROSE.  The entries are
     *  identical for both ILTs and IATs.
     *
     *  @par Import Lookup Table Entry (and Import Address Table Entry)
     *  Entries for ILTs and IATs are structurally identical.  They are 32- or 64-bit vectors.  The most significant bit
     *  (31/63) indicates whether the remaining bits are an Ordinal (when set) or Hint/Name address (when clear).  Ordinals are
     *  represented by the low-order 16 bits and Hint/Name addresses are stored in the low-order 31 bits.  All other bits must
     *  be zero according to the PE specification.  Hint/Name addresses are relative virtual addresses of entries in the
     *  (implicit) Hint/Name Table. When a function is bound by the dynamic linkter, its IAT Entry within process memory is
     *  overwritten with the virtual address of the bound function.
     *
     *  @par Hint/Name Table
     *  Some Import Lookup Table (and Import Address Table) entries contain a Hint/Name Table Entry RVA.  The Hint/Name Table
     *  Entries collectively form the Hint/Name Table, but there is no requirement that the entries appear in any particular
     *  order or even that they appear contiguously in memory.  In other words, the Hint/Name Table is a conceptual object
     *  rather than a true table in the PE file.
     *
     *
     * @verbatim
        +------------ Import Section -------------+                         (SgAsmPEImportSection)
        |                                         |
        |                                         |
        |  +------- Import Directory #0 ------+   |                         (SgAsmPEImportDirectory)
        |  |   1. Import Lookup Table RVA     |   |
        |  |   2. Date/time stamp             |   |
        |  |   3. Forwarder chain index       |   |
        |  |   4. Name RVA                    |   |
        |  |   5. Import Address Table RVA    |   |
        |  +----------------------------------+   |
        |                                         |
        |                                         |
        |  +------- Import Directory #1 ------+   |
        |  |   1. Import Lookup Table RVA     |--------+
        |  |   2. Date/time stamp             |   |    |
        |  |   3. Forwarder chain index       |   |    |
        |  |   4. Name RVA                    |   |    |
        |  |   5. Import Address Table RVA    |------- | -------+
        |  +----------------------------------+   |    |        |
        |                                         |    |        |
        |         . . .                           |    |        |
        |                                         |    |        |
        |  +------- Import Directory #N ------+   |    |        |
        |  |                                  |   |    |        |
        |  |   Terminating directory is       |   |    |        |
        |  |   zero filled.                   |   |    |        |
        |  |                                  |   |    |        |
        |  |                                  |   |    |        |
        |  +----------------------------------+   |    |        |
        |                                         |    |        |
        +-----------------------------------------+    |        |           (Entries of the ILT and IAT are combined into
                                                       |        |            SgAsmPEImportItem objects.)
                                                       |        |
                                                       |        |
        +----------- Import Lookup Table ---------+ <--+        +-->  +----------- Import Address Table --------+
        | #0  32/64-bit vector                    |                   | #0  32/64-bit vector or VA when bound   |
        |                                         |   These arrays    |                                         |
        | #1  32/64-bit vector                    |   are parallel    | #1  32/64-bit vector or VA when bound   |
        |                      \                  |                   |                                         |
        |     ...               \when used as     |                   |     ...                                 |
        |                        \a Hint/Name     |                   |                                         |
        | #N  32/64-bit zero      \RVA            |                   | #N  32/64-bit zero                      |
        +--------------------------\--------------+                   +-----------------------------------------+
                                    \
                                     \
                                      |
        + - - - - -  Hint/Name Table  | - - - - - +           The Hint/Name Table doesn't actually
                                      v                       exist explicitly--there is no pointer
        |  +------ Hint/Name ----------------+    |           to the beginning of the table and no
           |  1. 2-byte index ENPT           |                requirement that the entries be in any
        |  |  2. NUL-terminated name         |    |           particular order, or even contiguous.
           |  3. Optional extran NUL         |
        |  +---------------------------------+    |           "ENPT" means Export Name Pointer Table,
                                                              which is a table in the linked-to
        |          . . .                          |           shared library.

        |  +------ Hint/Name ----------------+    |
           |  1. 2-byte index ENPT           |                              (SgAsmPEImportHNTEntry)
        |  |  2. NUL-terminated name         |    |
           |  3. Optional extran NUL         |
        |  +---------------------------------+    |

        + - - - - - - - - - - - - - - - - - - - - +
    @endverbatim
     *
     * When parsing an Import Directory, ROSE assumes that the IAT contains ordinals and/or hint/name addresses rather than
     * bound addresses.  ROSE checks that the IAT entries are compatible with the ILT entries there were already parsed and if
     * an inconsistency is detected then a warning is issued and ROSE assumes that the IAT entry is a bound value instead.
     * Passing true as the @p assume_bound argument for the parser will cause ROSE to not issue such warnings and immediately
     * assume that all IAT entries are bound addresses.  One can therefore find the conflicting entries by looking for
     * SgAsmImportItem objects that are created with a non-zero bound address.
     *
     * The IAT is often required to be allocated at a fixed address, often the beginning of the ".rdata" section.  Increasing
     * the size of the IAT by adding more items to the import list(s) can be problematic because ROSE is unable to safely write
     * beyond the end of the original IAT.  We require the user to manually allocate space for the new IAT and tell the
     * SgAsmPEImportDirectory object the location and size of the allocated space before unparsing.  On a related note, due to
     * ROSE allocators being section-local, reallocation of an Import Section does not cause reallocation of ILTs, Hint/Name
     * pairs, or DLL names that have addresses outside the Import Section.  If these items' sizes increase, the items will be
     * truncated when written back to disk.  The reallocation happens automatically for all import-related objects that are
     * either bound to the import section or have a null RVA, so one method of getting things reallocated is to traverse the
     * AST and null their RVAs:
     *
     * @code
     *  struct Traversal: public AstSimpleTraversal {
     *      void visit(SgNode *node) {
     *          SgAsmPEImportDirectory *idir = isSgAsmPEImportDirectory(node);
     *          SgAsmPEImportItem *import = isSgAsmPEImportItem(node);
     *          static const rose_rva_t nil(0);
     *
     *          if (idir) {
     *              idir->set_dll_name_rva(nil);
     *              idir->set_ilt_rva(nil);
     *              idir->set_iat_rva(nil);
     *          }
     *
     *          if (import)
     *              idir->set_hintname_rva(nil);
     *     }
     *  };
     * @endcode
     *
     * @sa
     *      SgAsmPEImportDirectory
     *      SgAsmPEImportItem
     */
    class SgAsmPEImportSection: public SgAsmPESection {
    public:
#endif


#ifdef DOCUMENTATION
        /** Property: List of import directories.
         *
         *  See PE specification.
         *
         * @{ */
        SgAsmPEImportDirectoryList* get_import_directories() const;
        void set_import_directories(SgAsmPEImportDirectoryList*);
        /** @} */
#else
        AsmPEImportSection.setDataPrototype ("SgAsmPEImportDirectoryList*", "import_directories", "= NULL",
                                             NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmPEImportSection);
#if defined(SgAsmPEImportSection_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmPESection);
            s & BOOST_SERIALIZATION_NVP(p_import_directories);
        }
#endif

    public:
        explicit SgAsmPEImportSection(SgAsmPEFileHeader *fhdr)
            : SgAsmPESection(fhdr) {
            ctor();
        }
        virtual SgAsmPEImportSection *parse() $ROSE_OVERRIDE;
        virtual bool reallocate() $ROSE_OVERRIDE;
        virtual void unparse(std::ostream&) const $ROSE_OVERRIDE;
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const $ROSE_OVERRIDE;

        /** Add an import directory to the end of the import directory list. */
        void add_import_directory(SgAsmPEImportDirectory*);

        /** Remove an import directory from the import directory list. Does not delete it. */
        void remove_import_directory(SgAsmPEImportDirectory*);
        static bool show_import_mesg();
        static void import_mesg_reset() { mesg_nprinted=0; }

        /** Reallocate space for all Import Address Table.
         *
         *  This method traverses the AST beginning at this PE Import Section and assigns addresses and sizes to all Import
         *  Address Tables (IATs).  The first IAT is given the @p start_at RVA and its size is reset to what ever size is
         *  needed to store the entire table.  Each subsequent IAT is given the next available address and it's size is also
         *  updated.  The result is that all the IATs under this Import Section are given addresses and sizes that make them
         *  contiguous in memory. This method returns the total number of bytes required for all the IATs. */
        size_t reallocate_iats(rose_rva_t start_at);

    private:
        static size_t mesg_nprinted; //counter for import_mesg()
        void ctor();
#endif // SgAsmPEImportSection_OTHERS

#ifdef DOCUMENTATION
    };
#endif



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*************************************************************************************************************************
     *                                         PE Export Tables
     *************************************************************************************************************************/

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmPEExportSection);
    IS_SERIALIZABLE(AsmPEExportSection);


#ifdef DOCUMENTATION
    /** Export file section. */
    class SgAsmPEExportSection: public SgAsmPESection {
    public:
#endif


#ifdef DOCUMENTATION
        /** Property: Export directory.
         *
         *  See PE specification.
         *
         * @{ */
        SgAsmPEExportDirectory* get_export_dir() const;
        void set_export_dir(SgAsmPEExportDirectory*);
        /** @} */
#else
        AsmPEExportSection.setDataPrototype("SgAsmPEExportDirectory*", "export_dir", "= NULL",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: List of export entries.
         *
         *  See PE specification.
         *
         * @{ */
        SgAsmPEExportEntryList* get_exports() const;
        void set_exports(SgAsmPEExportEntryList*);
        /** @} */
#else
        AsmPEExportSection.setDataPrototype("SgAsmPEExportEntryList*", "exports", "= NULL",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmPEExportSection);
#if defined(SgAsmPEExportSection_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmPESection);
            s & BOOST_SERIALIZATION_NVP(p_export_dir);
            s & BOOST_SERIALIZATION_NVP(p_exports);
        }
#endif

    public:
        /** The PE Export Address Table is an array of expaddr_n 4-byte RVAs.
         *
         *  If the address is not in the export section (as defined by the address and length that are indicated in the NT
         *  Optional Header) then the RVA is an actual address in code or data. Otherwise its a Forwarder RVA that names a
         *  symbol in another DLL. */
        typedef uint32_t ExportAddress_disk;

        /** The PE Export Name Pointer Table is an array of nameptr_n 4-byte RVAs pointing into the Export Name Table. The
         *  pointers are ordered lexically to allow binary searches.  An export name is defined only if the export name pointer
         *  table contains a pointer to it. */
        typedef uint32_t ExportNamePtr_disk;

        /** The PE Export Ordinal Table is an array of nameptr_n (yes) 2-byte indices into the Export Address Table biased by
         *  ord_base. In other words, the ord_base must be subtracted from the ordinals to obtain true indices into the Export
         *  Address Table. */
        typedef uint16_t ExportOrdinal_disk;

        SgAsmPEExportSection(SgAsmPEFileHeader *fhdr)
            : SgAsmPESection(fhdr), p_export_dir(NULL), p_exports(NULL) {
            ctor();
        }
        virtual SgAsmPEExportSection *parse() $ROSE_OVERRIDE;
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const $ROSE_OVERRIDE;
        void add_entry(SgAsmPEExportEntry*);

    private:
        void ctor();
#endif // SgAsmPEExportSection_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmPEExportDirectory);
    IS_SERIALIZABLE(AsmPEExportDirectory);


#ifdef DOCUMENTATION
    /** Export directory. */
    class SgAsmPEExportDirectory: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Reserved aread #1.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_res1() const;
        void set_res1(unsigned);
        /** @} */
#else
        AsmPEExportDirectory.setDataPrototype("unsigned", "res1", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Time stamp.
         *
         *  See PE specification.
         *
         * @{ */
        time_t get_timestamp() const;
        void set_timestamp(time_t);
        /** @} */
#else
        AsmPEExportDirectory.setDataPrototype("time_t", "timestamp", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: VMajor.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_vmajor() const;
        void set_vmajor(unsigned);
        /** @} */
#else
        AsmPEExportDirectory.setDataPrototype("unsigned", "vmajor", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: VMinor.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_vminor() const;
        void set_vminor(unsigned);
        /** @} */
#else
        AsmPEExportDirectory.setDataPrototype("unsigned", "vminor", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: RVA for name.
         *
         *  See PE specification.
         *
         * @{ */
        rose_rva_t get_name_rva() const;
        void set_name_rva(rose_rva_t);
        /** @} */
#else
        AsmPEExportDirectory.setDataPrototype("rose_rva_t", "name_rva", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Ordinal base.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_ord_base() const;
        void set_ord_base(unsigned);
        /** @} */
#else
        AsmPEExportDirectory.setDataPrototype("unsigned", "ord_base", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Expaddr_n.
         *
         *  See PE specification.
         *
         * @{ */
        size_t get_expaddr_n() const;
        void set_expaddr_n(size_t);
        /** @} */
#else
        AsmPEExportDirectory.setDataPrototype("size_t", "expaddr_n", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Nameptr_n.
         *
         *  See PE specification.
         *
         * @{ */
        size_t get_nameptr_n() const;
        void set_nameptr_n(size_t);
        /** @} */
#else
        AsmPEExportDirectory.setDataPrototype("size_t", "nameptr_n", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Expaddr RVA.
         *
         *  See PE specification.
         *
         * @{ */
        rose_rva_t get_expaddr_rva() const;
        void set_expaddr_rva(rose_rva_t);
        /** @} */
#else
        AsmPEExportDirectory.setDataPrototype("rose_rva_t", "expaddr_rva", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Name pointer RVA.
         *
         *  See PE specification.
         *
         * @{ */
        rose_rva_t get_nameptr_rva() const;
        void set_nameptr_rva(rose_rva_t);
        /** @} */
#else
        AsmPEExportDirectory.setDataPrototype("rose_rva_t", "nameptr_rva", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Ordinals RVA.
         *
         *  See PE specification.
         *
         * @{ */
        rose_rva_t get_ordinals_rva() const;
        void set_ordinals_rva(rose_rva_t);
        /** @} */
#else
        AsmPEExportDirectory.setDataPrototype("rose_rva_t", "ordinals_rva", "= 0",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Name.
         *
         *  See PE specification.
         *
         * @{ */
        SgAsmGenericString* get_name() const;
        void set_name(SgAsmGenericString*);
        /** @} */
#else
        AsmPEExportDirectory.setDataPrototype("SgAsmGenericString*", "name", "= NULL",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmPEExportDirectory);
#if defined(SgAsmPEExportDirectory_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_res1);
            s & BOOST_SERIALIZATION_NVP(p_timestamp);
            s & BOOST_SERIALIZATION_NVP(p_vmajor);
            s & BOOST_SERIALIZATION_NVP(p_vminor);
            s & BOOST_SERIALIZATION_NVP(p_name_rva);
            s & BOOST_SERIALIZATION_NVP(p_ord_base);
            s & BOOST_SERIALIZATION_NVP(p_expaddr_n);
            s & BOOST_SERIALIZATION_NVP(p_nameptr_n);
            s & BOOST_SERIALIZATION_NVP(p_expaddr_rva);
            s & BOOST_SERIALIZATION_NVP(p_nameptr_rva);
            s & BOOST_SERIALIZATION_NVP(p_ordinals_rva);
            s & BOOST_SERIALIZATION_NVP(p_name);
        }
#endif

    public:
#ifdef _MSC_VER
# pragma pack (1)
#endif
        /* An Export Section begins with the Export Directory */
        struct PEExportDirectory_disk {
            uint32_t res1;         /* 0x00 Reserved, must be zero */
            uint32_t timestamp;    /* 0x04 Time that export data was created */
            uint16_t vmajor;       /* 0x08 Major version number (user defined) */
            uint16_t vminor;       /* 0x0a Minor version number (user defined) */
            uint32_t name_rva;     /* 0x0c Location of name of DLL */
            uint32_t ord_base;     /* 0x10 Starting ordinal for exports in this image (usually 1) */
            uint32_t expaddr_n;    /* 0x14 Number of entries in the export address table */
            uint32_t nameptr_n;    /* 0x18 Number of entries in the name pointer table and ordinal table */
            uint32_t expaddr_rva;  /* 0x1c Location of Export Address Table */
            uint32_t nameptr_rva;  /* 0x20 Location of Export Name Pointer Table */
            uint32_t ordinals_rva; /* 0x24 Location of Ordinal Table */
        }                               /* 0x28 */
#if !defined(SWIG) && !defined(_MSC_VER)
        __attribute__((packed))
#endif
        ;
#ifdef _MSC_VER
# pragma pack ()
#endif

        explicit SgAsmPEExportDirectory(SgAsmPEExportSection *s) {
            ctor(s);
        }
        void dump(FILE *f, const char *prefix, ssize_t idx) const;

    private:
        void ctor(SgAsmPEExportSection*);
#endif // SgAsmPEExportDirectory_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmPEExportEntryList);
    IS_SERIALIZABLE(AsmPEExportEntryList);

#ifdef DOCUMENTATION
    /** List of pointers to other AST nodes.
     *
     *  The only purpose of this node is to hold a list of pointers to other nodes. We cannot store the list directly in the
     *  nodes that need it due to ROSETTA limitations. */
    class SgAsmPEExportEntryList: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: List of pointers to nodes.
         *
         * @{ */
        const SgAsmPEExportEntryPtrList& get_exports() const;
        void set_exports(const SgAsmPEExportEntryPtrList&);
        /** @} */
#else
        AsmPEExportEntryList.setDataPrototype("SgAsmPEExportEntryPtrList", "exports", "",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmPEExportEntryList);
#if defined(SgAsmPEExportEntryList_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_exports);
        }
#endif
#endif // SgAsmPEExportEntryList_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmPEExportEntry);
    IS_SERIALIZABLE(AsmPEExportEntry);

#ifdef DOCUMENTATION
    /** Export entry. */
    class SgAsmPEExportEntry: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Name.
         *
         *  See PE specification.
         *
         * @{ */
        SgAsmGenericString* get_name() const;
        void set_name(SgAsmGenericString*);
        /** @} */
#else
        AsmPEExportEntry.setDataPrototype("SgAsmGenericString*", "name", "= NULL",
                                          NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Ordinal.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_ordinal() const;
        void set_ordinal(unsigned);
        /** @} */
#else
        AsmPEExportEntry.setDataPrototype("unsigned", "ordinal", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Export RVA.
         *
         *  See PE specification.
         *
         * @{ */
        rose_rva_t get_export_rva() const;
        void set_export_rva(rose_rva_t);
        /** @} */
#else
        AsmPEExportEntry.setDataPrototype("rose_rva_t", "export_rva", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Forwarder.
         *
         *  See PE specification.
         *
         * @{ */
        SgAsmGenericString* get_forwarder() const;
        void set_forwarder(SgAsmGenericString*);
        /** @} */
#else
        AsmPEExportEntry.setDataPrototype("SgAsmGenericString*", "forwarder", "= NULL",
                                          NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmPEExportEntry);
#if defined(SgAsmPEExportEntry_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_name);
            s & BOOST_SERIALIZATION_NVP(p_ordinal);
            s & BOOST_SERIALIZATION_NVP(p_export_rva);
            s & BOOST_SERIALIZATION_NVP(p_forwarder);
        }
#endif

    public:
        SgAsmPEExportEntry(SgAsmGenericString *name, unsigned ordinal, rose_rva_t export_rva,
                           SgAsmGenericString *forwarder) {
            ctor(name, ordinal, export_rva, forwarder);
        }

        void dump(FILE *f,const char *prefix,ssize_t idx) const;
        /* Accessors. Override ROSETTA because we adjust parents. */
        SgAsmGenericString *get_name() const {return p_name;}
        void set_name(SgAsmGenericString*);
        SgAsmGenericString *get_forwarder() const {return p_forwarder;}
        void set_forwarder(SgAsmGenericString*);

    private:
        void ctor(SgAsmGenericString *name, unsigned ordinal, rose_rva_t export_rva, SgAsmGenericString *forwarder);
#endif // SgAsmPEExportEntry_OTHERS

#ifdef DOCUMENTATION
    };
#endif



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*************************************************************************************************************************
     *                                         PE String Tables
     *************************************************************************************************************************/

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmPEStringSection);
    IS_SERIALIZABLE(AsmPEStringSection);
    AsmPEStringSection.setAutomaticGenerationOfDestructor(false);

#ifdef DOCUMENTATION
    /** String section. */
    class SgAsmPEStringSection: public SgAsmPESection {
    public:
#endif


#ifdef DOCUMENTATION
        /** Property: String table.
         *
         *  See PE specification.
         *
         * @{ */
        SgAsmCoffStrtab* get_strtab() const;
        void set_strtab(SgAsmCoffStrtab*);
        /** @} */
#else
        AsmPEStringSection.setDataPrototype("SgAsmCoffStrtab*", "strtab", "= NULL",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmPEStringSection);
#if defined(SgAsmPEStringSection_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmPESection);
            s & BOOST_SERIALIZATION_NVP(p_strtab);
        }
#endif

    public:
        SgAsmPEStringSection(SgAsmPEFileHeader *fhdr)
            : SgAsmPESection(fhdr), p_strtab(NULL) {
            ctor();
        }
        virtual ~SgAsmPEStringSection() {}
        virtual SgAsmPEStringSection* parse() $ROSE_OVERRIDE;
        virtual bool reallocate() $ROSE_OVERRIDE;
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const $ROSE_OVERRIDE;
        virtual void set_size(rose_addr_t newsize) $ROSE_OVERRIDE;
        virtual void unparse(std::ostream&) const $ROSE_OVERRIDE;

    private:
        void ctor();
#endif // SgAsmPEStringSection_OTHERS

#ifdef DOCUMENTATION
    };
#endif



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*************************************************************************************************************************
     *                                         PE Section Tables
     *************************************************************************************************************************/

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmPESectionTable);
    IS_SERIALIZABLE(AsmPESectionTable);

#ifdef DOCUMENTATION
    /** Section table. */
    class SgAsmPESectionTable: public SgAsmGenericSection {
    public:
#endif

        DECLARE_OTHERS(AsmPESectionTable);
#if defined(SgAsmPESectionTable_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmGenericSection);
        }
#endif

    public:
        explicit SgAsmPESectionTable(SgAsmPEFileHeader *fhdr)
            : SgAsmGenericSection(fhdr->get_file(), fhdr) {
            ctor();
        }
        virtual SgAsmPESectionTable* parse() $ROSE_OVERRIDE;

        /** Attaches a previously unattached PE Section to the PE Section Table.
         *
         *  This method complements SgAsmPESection::init_from_section_table. This method initializes the section table from the
         *  section while init_from_section_table() initializes the section from the section table. */
        void add_section(SgAsmPESection *section);
        virtual bool reallocate() $ROSE_OVERRIDE;
        virtual void unparse(std::ostream&) const $ROSE_OVERRIDE;
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const $ROSE_OVERRIDE;

    private:
        void ctor();
#endif // SgAsmPESectionTable_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmPESectionTableEntry);
    IS_SERIALIZABLE(AsmPESectionTableEntry);

#ifdef DOCUMENTATION
    /** Section table entry. */
    class SgAsmPESectionTableEntry: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Name.
         *
         *  See PE specification.
         *
         * @{ */
        const std::string& get_name() const;
        void set_name(const std::string&);
        /** @} */
#else
        AsmPESectionTableEntry.setDataPrototype("std::string", "name", "= \"\"",
                                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Virtual size.
         *
         *  See PE specification.
         *
         * @{ */
        rose_addr_t get_virtual_size() const;
        void set_virtual_size(rose_addr_t);
        /** @} */
#else
        AsmPESectionTableEntry.setDataPrototype("rose_addr_t", "virtual_size", "= 0",
                                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: RVA.
         *
         *  See PE specification.
         *
         * @{ */
        rose_addr_t get_rva() const;
        void set_rva(rose_addr_t);
        /** @} */
#else
        AsmPESectionTableEntry.setDataPrototype("rose_addr_t", "rva", "= 0",
                                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Physical size.
         *
         *  See PE specification.
         *
         * @{ */
        rose_addr_t get_physical_size() const;
        void set_physical_size(rose_addr_t);
        /** @} */
#else
        AsmPESectionTableEntry.setDataPrototype("rose_addr_t", "physical_size", "= 0",
                                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Physical offset.
         *
         *  See PE specification.
         *
         * @{ */
        rose_addr_t get_physical_offset() const;
        void set_physical_offset(rose_addr_t);
        /** @} */
#else
        AsmPESectionTableEntry.setDataPrototype("rose_addr_t", "physical_offset", "= 0",
                                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: COFF line numbers.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_coff_line_nums() const;
        void set_coff_line_nums(unsigned);
        /** @} */
#else
        AsmPESectionTableEntry.setDataPrototype("unsigned", "coff_line_nums", "= 0",
                                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Number of relocations.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_n_relocs() const;
        void set_n_relocs(unsigned);
        /** @} */
#else
        AsmPESectionTableEntry.setDataPrototype("unsigned", "n_relocs", "= 0",
                                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Number of COFF line numbers.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_n_coff_line_nums() const;
        void set_n_coff_line_nums(unsigned);
        /** @} */
#else
        AsmPESectionTableEntry.setDataPrototype("unsigned", "n_coff_line_nums", "= 0",
                                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Flags.
         *
         *  See PE specification.
         *
         * @{ */
        unsigned get_flags() const;
        void set_flags(unsigned);
        /** @} */
#else
        AsmPESectionTableEntry.setDataPrototype("unsigned", "flags", "= 0",
                                                NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmPESectionTableEntry);
#if defined(SgAsmPESectionTableEntry_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_name);
            s & BOOST_SERIALIZATION_NVP(p_virtual_size);
            s & BOOST_SERIALIZATION_NVP(p_rva);
            s & BOOST_SERIALIZATION_NVP(p_physical_size);
            s & BOOST_SERIALIZATION_NVP(p_physical_offset);
            s & BOOST_SERIALIZATION_NVP(p_coff_line_nums);
            s & BOOST_SERIALIZATION_NVP(p_n_relocs);
            s & BOOST_SERIALIZATION_NVP(p_coff_line_nums);
            s & BOOST_SERIALIZATION_NVP(p_flags);
        }
#endif
    public:
#ifdef _MSC_VER
# pragma pack (1)
#endif
        /* File format of a section table entry. All fields are little endian. Sections are ordered by RVA. */
        struct PESectionTableEntry_disk {
            char     name[8];          /* NUL-padded */
            uint32_t virtual_size;     /* virtual memory size, >= physical_size and difference is zero filled */
            uint32_t rva;              /* relative virt addr wrt Image Base; multiple of section_align; dense space */
            uint32_t physical_size;    /* bytes of initialized data on disk; multiple of file_align & <= virtual_size*/
            uint32_t physical_offset;  /* location of initialized data on disk; multiple of file_align */
            uint32_t coff_line_nums;   /* file offset of COFF line number info or zero */
            uint32_t n_relocs;         /* number of relocation entries; should be zero for executables */
            uint32_t n_coff_line_nums; /* number of COFF line number entries */
            uint32_t flags;            /* PESectionFlags bits: code, data, caching, paging, shared, permissions, etc.*/
        }
#if !defined(SWIG) && !defined(_MSC_VER)
        __attribute__((packed))
#endif
        ;
#ifdef _MSC_VER
# pragma pack ()
#endif

        /* These come from the windows PE documentation and
         * http://en.wikibooks.org/wiki/X86_Disassembly/Windows_Executable_Files */
        enum PESectionFlags {
            OF_CODE             = 0x00000020,   /* section contains code */
            OF_IDATA            = 0x00000040,   /* initialized data */
            OF_UDATA            = 0x00000080,   /* uninitialized data */
            OF_INFO             = 0x00000200,   /* comments or some other type of info */
            OF_REMOVE           = 0x00000800,   /* section will not become part of image */
            OF_COMDAT           = 0x00001000,   /* section contains comdat */
            OF_NO_DEFER_SPEC_EXC= 0x00004000,   /* reset speculative exception handling bits in the TLB entires for
                                                 * this section */
            OF_GPREL            = 0x00008000,   /* section content can be access relative to GP */
            OF_ALIGN_1          = 0x00100000,   /* no alignment */
            OF_ALIGN_2          = 0x00200000,   /* 2-byte alignment */
            OF_ALIGN_4          = 0x00300000,   /* 4-byte alignment */
            OF_ALIGN_8          = 0x00400000,   /* 8-byte alignment */
            OF_ALIGN_16         = 0x00500000,   /* 16-byte alignment (default if not other alignment specified) */
            OF_ALIGN_32         = 0x00600000,   /* 32-byte alignment */
            OF_ALIGN_64         = 0x00700000,   /* 64-byte alignment */
            OF_ALIGN_128        = 0x00800000,   /* 128-byte alignment */
            OF_ALIGN_256        = 0x00900000,   /* 256-byte alignment */
            OF_ALIGN_512        = 0x00a00000,   /* 512-byte alignment */
            OF_ALIGN_1k         = 0x00b00000,   /* 1024-byte alignment */
            OF_ALIGN_2k         = 0x00c00000,   /* 2048-byte alignment */
            OF_ALIGN_4k         = 0x00d00000,   /* 4096-byte alignment */
            OF_ALIGN_8k         = 0x00e00000,   /* 8192-byte alignment */
            OF_ALIGN_MASK       = 0x00f00000,   /* mask for alignment value */
            OF_NRELOC_OVFL      = 0x01000000,   /* section contains extended relocations */
            OF_DISCARDABLE      = 0x02000000,   /* can be discarded */
            OF_NO_CACHE         = 0x04000000,   /* section must not be cached */
            OF_NO_PAGING        = 0x08000000,   /* section is not pageable */
            OF_SHARED           = 0x10000000,   /* section is shared */
            OF_EXECUTABLE       = 0x20000000,   /* execute permission */
            OF_READABLE         = 0x40000000,   /* read permission */
            OF_WRITABLE         = 0x80000000    /* write permission */
        };

        explicit SgAsmPESectionTableEntry(const SgAsmPESectionTableEntry::PESectionTableEntry_disk *disk);

        /** Update this section table entry with newer information from the section */
        void update_from_section(SgAsmPESection *section);
        void *encode(SgAsmPESectionTableEntry::PESectionTableEntry_disk*) const;
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const;

    private:
        void ctor(const SgAsmPESectionTableEntry::PESectionTableEntry_disk*);
#endif // SgAsmPESectionTableEntry_OTHERS

#ifdef DOCUMENTATION
    };
#endif



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*************************************************************************************************************************
     *                                         PE Sections
     *************************************************************************************************************************/

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NEW_NONTERMINAL_MACRO(AsmPESection,
                          AsmPEImportSection | AsmPEExportSection | AsmPEStringSection,
                          "AsmPESection", "AsmPESectionTag", true);
    AsmPESection.setCppCondition("!defined(DOCUMENTATION)");
    IS_SERIALIZABLE(AsmPESection);

#ifdef DOCUMENTATION
    /** Base class for PE sections. */
    class SgAsmPESection: public SgAsmGenericSection {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Section table entry.
         *
         *  See PE specification.
         *
         * @{ */
        SgAsmPESectionTableEntry* get_section_entry() const;
        void set_section_entry(SgAsmPESectionTableEntry*);
        /** @} */
#else
        AsmPESection.setDataPrototype("SgAsmPESectionTableEntry*", "section_entry", "= NULL",
                                      NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmPESection);
#if defined(SgAsmPESection_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmGenericSection);
            s & BOOST_SERIALIZATION_NVP(p_section_entry);
        }
#endif

    public:
        explicit SgAsmPESection(SgAsmPEFileHeader *fhdr)
            : SgAsmGenericSection(fhdr->get_file(), fhdr), p_section_entry(NULL) {}

        /** Initializes the section from data parsed from the PE Section Table.
         *
         *  This includes the section offset, size, memory mapping, alignments, permissions, etc. This function complements
         *  SgAsmPESectionTable::add_section(): this function initializes this section from the section table while
         *  add_section() initializes the section table from the section. */
        SgAsmPESection *init_from_section_table(SgAsmPESectionTableEntry *entry, int id);
        virtual bool reallocate() $ROSE_OVERRIDE;
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const $ROSE_OVERRIDE;
#endif // SgAsmPESection_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*************************************************************************************************************************
     *                                         COFF Symbol Tables
     * The COFF format is used by Microsoft PE files.
     *************************************************************************************************************************/

    NEW_TERMINAL_MACRO(AsmCoffSymbolTable, "AsmCoffSymbolTable", "AsmCoffSymbolTableTag");
    AsmCoffSymbolTable.setCppCondition("!defined(DOCUMENTATION)");
    IS_SERIALIZABLE(AsmCoffSymbolTable);

#ifdef DOCUMENTATION
    /** COFF symbol table.
     *
     *  This is a symbol table used by Microsoft PE format. */
    class SgAsmCoffSymbolTable: public SgAsmGenericSection {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: String table.
         *
         *  Table that holds the strings for the symbol names.
         *
         * @{ */
        SgAsmGenericSection* get_strtab() const;
        void set_strtab(SgAsmGenericSection*);
        /** @} */
#else
        AsmCoffSymbolTable.setDataPrototype("SgAsmGenericSection*", "strtab", "= NULL",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: List of symbols.
         *
         * @{ */
        SgAsmCoffSymbolList* get_symbols() const;
        void set_symbols(SgAsmCoffSymbolList*);
        /** @} */
#else
        AsmCoffSymbolTable.setDataPrototype("SgAsmCoffSymbolList*", "symbols", "= NULL",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmCoffSymbolTable);
#if defined(SgAsmCoffSymbolTable_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmGenericSection);
            s & BOOST_SERIALIZATION_NVP(p_strtab);
            s & BOOST_SERIALIZATION_NVP(p_symbols);
        }
#endif

    public:
        explicit SgAsmCoffSymbolTable(SgAsmPEFileHeader *fhdr)
            : SgAsmGenericSection(fhdr->get_file(), fhdr) {ctor();}
        size_t get_nslots() const;
        virtual SgAsmCoffSymbolTable *parse() $ROSE_OVERRIDE;
        virtual void unparse(std::ostream&) const $ROSE_OVERRIDE;
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const $ROSE_OVERRIDE;

    private:
        void ctor();
#endif // SgAsmCoffSymbolTable_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NEW_TERMINAL_MACRO(AsmCoffSymbolList, "AsmCoffSymbolList", "AsmCoffSymbolListTag");
    AsmCoffSymbolList.setCppCondition("!defined(DOCUMENTATION)");
    IS_SERIALIZABLE(AsmCoffSymbolList);

#ifdef DOCUMENTATION
    /** List of COFF symbols. */
    class SgAsmCoffSymbolList: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: List of symbol pointers.
         *
         * @{ */
        const SgAsmCoffSymbolPtrList& get_symbols() const;
        void set_symbols(const SgAsmCoffSymbolPtrList&);
        /** @} */
#else
        AsmCoffSymbolList.setDataPrototype("SgAsmCoffSymbolPtrList", "symbols", "",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmCoffSymbolList);
#if defined(SgAsmCoffSymbolList_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;


        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_symbols);
        }
#endif
#endif // SgAsmCoffSymbolList_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NEW_TERMINAL_MACRO(AsmCoffSymbol, "AsmCoffSymbol", "AsmCoffSymbolTag");
    AsmCoffSymbol.setCppCondition("!defined(DOCUMENTATION)");
    IS_SERIALIZABLE(AsmCoffSymbol);
    AsmCoffSymbol.setFunctionSource("SOURCE_PE_COFF_SYMBOL", "../Grammar/BinaryInstruction.code");

#ifdef DOCUMENTATION
    /** COFF symbol. */
    class SgAsmCoffSymbol: public SgAsmGenericSymbol {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Symbol name.
         *
         *  @{ */
        const std::string& get_st_name() const;
        void set_st_name(const std::string&)
        /** @} */
#else
            AsmCoffSymbol.setDataPrototype("std::string", "st_name", "= \"\"",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Symbol name offset.
         *
         * @{ */
        rose_addr_t get_st_name_offset() const;
        void set_st_name_offset(rose_addr_t);
        /** @} */
#else
        AsmCoffSymbol.setDataPrototype("rose_addr_t", "st_name_offset", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Section number.
         *
         * @{ */
        int get_st_section_num() const;
        void set_st_section_num(int);
        /** @} */
#else
        AsmCoffSymbol.setDataPrototype("int", "st_section_num", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Symbol type constant.
         *
         * @{ */
        unsigned get_st_type() const;
        void set_st_type(unsigned);
        /** @} */
#else
        AsmCoffSymbol.setDataPrototype("unsigned", "st_type", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Symbol storage class.
         *
         * @{ */
        unsigned get_st_storage_class() const;
        void set_st_storage_class(unsigned);
        /** @} */
#else
        AsmCoffSymbol.setDataPrototype("unsigned", "st_storage_class", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Number of auxilliary entries.
         *
         * @{ */
        unsigned get_st_num_aux_entries() const;
        void set_st_num_aux_entries(unsigned);
        /** @} */
#else
        AsmCoffSymbol.setDataPrototype("unsigned", "st_num_aux_entries", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Auxilliary data.
         *
         * @{ */
        const SgUnsignedCharList& get_aux_data() const;
        void set_aux_data(const SgUnsignedCharList&);
        /** @} */
#else
        AsmCoffSymbol.setDataPrototype("SgUnsignedCharList", "aux_data", "",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmCoffSymbol);
#if defined(SgAsmCoffSymbol_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmGenericSymbol);
            s & BOOST_SERIALIZATION_NVP(p_st_name);
            s & BOOST_SERIALIZATION_NVP(p_st_name_offset);
            s & BOOST_SERIALIZATION_NVP(p_st_section_num);
            s & BOOST_SERIALIZATION_NVP(p_st_type);
            s & BOOST_SERIALIZATION_NVP(p_st_storage_class);
            s & BOOST_SERIALIZATION_NVP(p_st_num_aux_entries);
            s & BOOST_SERIALIZATION_NVP(p_aux_data);
        }
#endif

    public:

        static const unsigned int COFFSymbol_disk_size = 18;

#ifdef _MSC_VER
# pragma pack (1)
#endif
        struct COFFSymbol_disk {
            union {
                char            st_name[8];
                struct {
                    uint32_t    st_zero;
                    uint32_t    st_offset;
                };
            };
            uint32_t            st_value;
            int16_t             st_section_num;
            uint16_t            st_type;
            unsigned char       st_storage_class;
            unsigned char       st_num_aux_entries;
        }
// DQ (3/7/2013): Adding support to restrict visability to SWIG.
#if !defined(SWIG) && !defined(_MSC_VER)
        __attribute__((packed))
#endif
        ;
#ifdef _MSC_VER
# pragma pack ()
#endif

        SgAsmCoffSymbol(SgAsmPEFileHeader *fhdr, SgAsmGenericSection *symtab, SgAsmGenericSection *strtab, size_t idx);
        void *encode(SgAsmCoffSymbol::COFFSymbol_disk*) const;
        virtual void dump(FILE *f, const char *prefix, ssize_t idx) const $ROSE_OVERRIDE;

    private:
        void ctor(SgAsmPEFileHeader*, SgAsmGenericSection *symtab, SgAsmGenericSection *strtab, size_t idx);
#endif // SgAsmCoffSymbol_OTHERS

#ifdef DOCUMENTATION
    };
#endif


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NEW_TERMINAL_MACRO(AsmCoffStrtab, "AsmCoffStrtab", "AsmCoffStrtabTag");
    AsmCoffStrtab.setCppCondition("!defined(DOCUMENTATION)");
    AsmCoffStrtab.setAutomaticGenerationOfDestructor(false);
    IS_SERIALIZABLE(AsmCoffStrtab);

#ifdef DOCUMENTATION
    /** COFF symbol string table. */
    class SgAsmCoffStrtab: public SgAsmGenericStrtab {
    public:
#endif

        DECLARE_OTHERS(AsmCoffStrtab);
#if defined(SgAsmCoffStrtab_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmGenericStrtab);
        }
#endif

    public:
        explicit SgAsmCoffStrtab(class SgAsmPESection *containing_section)
            : SgAsmGenericStrtab(containing_section) {}
        virtual ~SgAsmCoffStrtab();
        virtual void unparse(std::ostream&) const;
        virtual SgAsmStringStorage *create_storage(rose_addr_t offset, bool shared) $ROSE_OVERRIDE;
        virtual rose_addr_t get_storage_size(const SgAsmStringStorage*) $ROSE_OVERRIDE;
#endif // SgAsmCoffStrtab_OTHERS

#ifdef DOCUMENTATION
    };
#endif

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

    DECLARE_LEAF_CLASS(AsmStringStorage);
    IS_SERIALIZABLE(AsmStringStorage);

#ifdef DOCUMENTATION
    /** Strings stored in an ELF or PE container. */
    class SgAsmStringStorage: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: String table holding the string.
         *
         * @{ */
        SgAsmGenericStrtab* get_strtab() const;
        void set_strtab(SgAsmGenericStrtab*);
        /** @} */
#else
        AsmStringStorage.setDataPrototype("SgAsmGenericStrtab*", "strtab", "= NULL",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: String value.
         *
         * @{ */
        const std::string& get_string() const;
        void set_string(const std::string&);
        /** @} */
#else
        AsmStringStorage.setDataPrototype("std::string", "string", "= \"\"",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Location of string in storage table.
         *
         * @{ */
        rose_addr_t get_offset() const;
        void set_offset(rose_addr_t);
        /** @} */
#else
        AsmStringStorage.setDataPrototype("rose_addr_t", "offset", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmStringStorage);
#if defined(SgAsmStringStorage_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_strtab);
            s & BOOST_SERIALIZATION_NVP(p_string);
            s & BOOST_SERIALIZATION_NVP(p_offset);
        }
#endif

    public:
        SgAsmStringStorage(SgAsmGenericStrtab *strtab, const std::string &string, rose_addr_t offset)
            : p_strtab(strtab), p_string(string), p_offset(offset) {}

        void dump(FILE *s, const char *prefix, ssize_t idx) const;

        /* Accessors. The set_* accessors are private because we don't want anyone messing with them. These data members are
         * used to control string allocation in ELF string tables and must only be modified by allocators in closely related
         * classes.  For instance, to change the value of the string one should call SgAsmGenericString::set_string()
         * instead. */

     private:
          friend class SgAsmStoredString;                     /*allowed to set private data members*/
          friend class SgAsmStoredStrtab;                     /*allowed to set private data members*/
#endif // SgAsmStringStorage_OTHERS

#ifdef DOCUMENTATION
    };
#endif

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
        void serialize(S &s, const unsigned /*version*/) {
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
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const $ROSE_OVERRIDE;
        virtual std::string get_string(bool escape=false) const $ROSE_OVERRIDE;
        virtual void set_string(const std::string&) $ROSE_OVERRIDE;
        virtual void set_string(rose_addr_t) $ROSE_OVERRIDE;

    private:
        void ctor();
#endif // SgAsmBasicString_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmStoredString);
    IS_SERIALIZABLE(AsmStoredString);

#ifdef DOCUMENTATION
    /** Strings stored in an ELF or PE container. */
    class SgAsmStoredString: public SgAsmGenericString {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Storage area for this string.
         *
         * @{ */
        SgAsmStringStorage* get_storage() const;
        void set_storage(SgAsmStringStorage*);
        /** @} */
#else
        AsmStoredString.setDataPrototype("SgAsmStringStorage*", "storage", "= NULL",
                                         NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmStoredString);
#if defined(SgAsmStoredString_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmGenericString);
            s & BOOST_SERIALIZATION_NVP(p_storage);
        }
#endif

    public:
        /** Construct a string existing in a string table. */
        SgAsmStoredString(SgAsmGenericStrtab *strtab, rose_addr_t offset) {
            ctor(strtab, offset, false);
        }

        /** Construct a new string in a string table. */
        SgAsmStoredString(SgAsmGenericStrtab *strtab, const std::string &s) {
            ctor(strtab, s);
        }

        /** Construct a string that shares storage with another. */
        explicit SgAsmStoredString(class SgAsmStringStorage *storage) {
            ctor(storage);
        }

        /** Print some debugging info */
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const $ROSE_OVERRIDE;

        /** Returns the string table that holds this string.
         *
         *  Returns the table even if the string value isn't currently allocated in the table. */
        SgAsmGenericStrtab *get_strtab();

        /** Returns the std::string associated with the SgAsmStoredString. */
        virtual std::string get_string(bool escape=false) const $ROSE_OVERRIDE;

        /** Give the string a new value.
         *
         *  This also deallocates the previous value. */
        virtual void set_string(const std::string&) $ROSE_OVERRIDE;

        /** Give the string a new value.
         *
         *  The string is given a new value by specifying the offset of a string that already exists in the string table. */
        virtual void set_string(rose_addr_t) $ROSE_OVERRIDE;

        /** Returns the offset into the string table where the string is allocated.
         *
         *  If the string is not allocated then this call triggers an allocation. */
        virtual rose_addr_t get_offset() const $ROSE_OVERRIDE;

    private:
        void ctor(class SgAsmGenericStrtab*, rose_addr_t offset, bool shared);
        void ctor(class SgAsmGenericStrtab*, const std::string&);
        void ctor(class SgAsmStringStorage*);
        void ctor(const std::string &s);
#endif // SgAsmStoredString_OTHERS

#ifdef DOCUMENTATION
    };
#endif

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
        void serialize(S &s, const unsigned /*version*/) {
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


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*************************************************************************************************************************
     *                                         Generic Binary IR Nodes
     * These are mostly base classes for the container-specific nodes defined above.
     *************************************************************************************************************************/

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmGenericHeaderList);
    IS_SERIALIZABLE(AsmGenericHeaderList);


#ifdef DOCUMENTATION
    /** List of generic file headers.
     *
     *  The only purpose of this AST node is to hold a list of pointers which cannot be contained directly in other nodes
     *  because of ROSETTA limitations. */
    class SgAsmGenericHeaderList: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: List of file header nodes.
         *
         * @{ */
        const SgAsmGenericHeaderPtrList& get_headers() const;
        void set_headers(const SgAsmGenericHeaderPtrList&);
        /** @} */
#else
        AsmGenericHeaderList.setDataPrototype("SgAsmGenericHeaderPtrList", "headers", "",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL,
                                              NO_DELETE);
#endif

        DECLARE_OTHERS(AsmGenericHeaderList);
#if defined(SgAsmGenericHeaderList_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_headers);
        }
#endif
#endif // SgAsmGenericHeaderList_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NEW_NONTERMINAL_MACRO(AsmGenericHeader,
                          AsmPEFileHeader | AsmLEFileHeader | AsmNEFileHeader | AsmDOSFileHeader | AsmElfFileHeader,
                          "AsmGenericHeader", "AsmGenericHeaderTag", true);
    AsmGenericHeader.setCppCondition("!defined(DOCUMENTATION)");
    AsmGenericHeader.setAutomaticGenerationOfDestructor(false);
    IS_SERIALIZABLE(AsmGenericHeader);

#ifdef DOCUMENTATION
    /** Base class for container file headers. */
    class SgAsmGenericHeader: public SgAsmGenericSection {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: General info about the executable format.
         *
         * @{ */
        SgAsmGenericFormat* get_exec_format() const;
        void set_exec_format(SgAsmGenericFormat*);
        /** @} */
#else
        AsmGenericHeader.setDataPrototype("SgAsmGenericFormat*", "exec_format", "= NULL",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Optional magic number in file byte order.
         *
         * @{ */
        const SgCharList& get_magic() const;
        void set_magic(const SgCharList&);
        /** @} */
#else
        AsmGenericHeader.setDataPrototype("SgCharList", "magic", "",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Machine for which this header and its sections, etc. was compiled.
         *
         * @{ */
        SgAsmGenericFormat::InsSetArchitecture get_isa() const;
        void set_isa(SgAsmGenericFormat::InsSetArchitecture);
        /** @} */
#else
        AsmGenericHeader.setDataPrototype("SgAsmGenericFormat::InsSetArchitecture", "isa",
                                          "= SgAsmGenericFormat::ISA_UNSPECIFIED",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Base virtual address used by all relative virtual addresses.
         *
         * @{ */
        rose_addr_t get_base_va() const;
        void set_base_va(rose_addr_t);
        /** @} */
#else
        AsmGenericHeader.setDataPrototype("rose_addr_t", "base_va", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Code entry point wrt base virtual address.
         *
         * @{ */
        const SgRVAList& get_entry_rvas() const;
        void set_entry_rvas(const SgRVAList&);
        /** @} */
#else
        AsmGenericHeader.setDataPrototype("SgRVAList", "entry_rvas", "",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: List of dynamically linked libraries.
         *
         * @{ */
        SgAsmGenericDLLList* get_dlls() const;
        void set_dlls(SgAsmGenericDLLList*);
        /** @} */
#else
        AsmGenericHeader.setDataPrototype("SgAsmGenericDLLList*", "dlls", "= NULL",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: List of file sections.
         *
         * @{ */
        SgAsmGenericSectionList* get_sections() const;
        void set_sections(SgAsmGenericSectionList*);
        /** @} */
#else
        AsmGenericHeader.setDataPrototype("SgAsmGenericSectionList*", "sections", "= NULL",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmGenericHeader);
#if defined(SgAsmGenericHeader_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmGenericSection);
            s & BOOST_SERIALIZATION_NVP(p_exec_format);
            s & BOOST_SERIALIZATION_NVP(p_magic);
            s & BOOST_SERIALIZATION_NVP(p_isa);
            s & BOOST_SERIALIZATION_NVP(p_base_va);
            s & BOOST_SERIALIZATION_NVP(p_entry_rvas);
            s & BOOST_SERIALIZATION_NVP(p_dlls);
            s & BOOST_SERIALIZATION_NVP(p_sections);
        }
#endif

    public:
        /** Constructor.
         *
         *  Headers (@ref SgAsmGenericHeader and derived classes) set the file/header relationship--a bidirectional link
         *  between this new header and the single file that contains this new header. This new header points to its file and
         *  the file contains a list that points to this new header. The header-to-file half of the link is deleted by the
         *  default destructor by virtue of being a simple pointer, but we also need to delete the other half of the link in
         *  the destructors. */
        explicit SgAsmGenericHeader(SgAsmGenericFile *ef)
            : SgAsmGenericSection(ef, NULL), p_exec_format(NULL), p_isa(ISA_OTHER), p_base_va(0), p_dlls(NULL),
              p_sections(NULL) {
            ctor();
        }

        virtual ~SgAsmGenericHeader();

        /** Allow all sections to reallocate themselves */
        virtual bool reallocate() $ROSE_OVERRIDE;

        /** Unparse headers and all they point to */
        virtual void unparse(std::ostream&) const $ROSE_OVERRIDE;

        /** Print some debugging info. */
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const $ROSE_OVERRIDE;

        /** Returns the name of the file format. */
        virtual const char *format_name() const;

        /** Add a new DLL to the header DLL list */
        void add_dll(SgAsmGenericDLL *dll);

        /** Vector of dynamically loaded libraries. */
        std::vector<SgAsmGenericDLL*>& get_dlls() {
            ROSE_ASSERT(p_dlls != NULL);
            return p_dlls->get_dlls();
        }

        std::vector<unsigned char>& get_magic() {return p_magic;}

        /** Returns the RVA of the first entry point.
         *
         *  The return value is relative to the header's base virtual address. If there are no entry points defined then
         *  returns a zero RVA. */
        rose_addr_t get_entry_rva() const;

        /** Append an RVA to the list of entry points. */
        void add_entry_rva(const rose_rva_t &rva) {
            p_entry_rvas.push_back(rva);
        }

        /* Convenience functions */
        ByteOrder::Endianness get_sex() const;
        size_t get_word_size() const;

        /** Adds a new section to the header.
         *
         *  This is called implicitly by the section constructor. */
        void add_section(SgAsmGenericSection*);

        /** Removes a secton from the header's section list. */
        void remove_section(SgAsmGenericSection*);

        /** Returns the list of sections that are memory mapped */
        SgAsmGenericSectionPtrList get_mapped_sections() const;

        /** Returns sections in this header that have the specified ID. */
        SgAsmGenericSectionPtrList get_sections_by_id(int id) const;

        /** Returns sections in this header that have the specified name.
         *
         *  If @p sep is a non-null string then ignore any part of name at and after @p sep. */
        SgAsmGenericSectionPtrList get_sections_by_name(std::string, char sep=0) const;

        /** Returns sectons in this header that contain all of the specified portion of the file. */
        SgAsmGenericSectionPtrList get_sections_by_offset(rose_addr_t offset, rose_addr_t size) const;

        /** Returns sections that have a preferred mapping that includes the specified relative virtual address. */
        SgAsmGenericSectionPtrList get_sections_by_rva(rose_addr_t rva) const;

        /** Returns sections having a preferred or actual mapping that includes the specified virtual address.
         *
         *  If @p use_preferred is set, then the condition is evaluated by looking at the section's preferred mapping,
         *  otherwise the actual mapping is used.  If an actual mapping is used, the specified virtual address must be part of
         *  the actual mapped section, not merely in the memory region that was also mapped to satisfy alignment
         *  constraints. */
        SgAsmGenericSectionPtrList get_sections_by_va(rose_addr_t va, bool use_preferred) const;

        /** Returns single section in this header that has the specified ID. */
        SgAsmGenericSection *get_section_by_id(int id, size_t *nfound=0) const;

        /** Returns single section in this header that has the specified name. */
        SgAsmGenericSection *get_section_by_name(const std::string&, char sep=0, size_t *nfound=0) const;

        /** Returns single section in this header that contains all of the specified portion of the file. */
        SgAsmGenericSection *get_section_by_offset(rose_addr_t offset, rose_addr_t size, size_t *nfound=0) const;

        /** Returns the single section having a preferred mapping that includes the specified relative virtual address.
         *
         *  If there are no sections or multiple sections satisfying this condition then a null pointer is returned. */
        SgAsmGenericSection *get_section_by_rva(rose_addr_t rva, size_t *nfound=0) const;

        /** Returns the section having a preferred or actual mapping that includes the specified virtual address.
         *
         *  If @p use_preferred is set, then the condition is evaluated by looking at the section's preferred mapping,
         *  otherwise the actual mapping is used. If an actual mapping is used, the specified virtual address must be part of
         *  the actual mapped section, not merely in the memory region that was also mapped to satisfy alignment constraints.
         *  If there are no sections or multiple sections satisfying this condition then a null pointer is returned. */
        SgAsmGenericSection *get_section_by_va(rose_addr_t va, bool use_preferred, size_t *nfound=0) const;

        /** Like SgAsmGenericFile::get_best_section_by_va() except considers only sections defined in this header. */
        SgAsmGenericSection *get_best_section_by_va(rose_addr_t va, bool use_preferred, size_t *nfound=0) const;

    private:
        void ctor();
#endif // SgAsmGenericHeader_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmGenericSymbolList);
    IS_SERIALIZABLE(AsmGenericSymbolList);

#ifdef DOCUMENTATION
    /** Node to hold a list of symbol node pointers.
     *
     *  The only reason this node exists is because a ROSETTA limitation prevents us from storing the list directly in the
     *  nodes that need it. */
    class SgAsmGenericSymbolList: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: List of symbol nodes.
         *
         * @{ */
        const SgAsmGenericSymbolPtrList& get_symbols() const;
        void set_symbols(const SgAsmGenericSymbolPtrList&);
        /** @} */
#else
        AsmGenericSymbolList.setDataPrototype("SgAsmGenericSymbolPtrList", "symbols", "",
                                              NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL,
                                              NO_DELETE);
#endif

        DECLARE_OTHERS(AsmGenericSymbolList);
#if defined(SgAsmGenericSymbolList_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_symbols);
        }
#endif
#endif // SgAsmGenericSymbolList_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NEW_NONTERMINAL_MACRO(AsmGenericSymbol,
                          AsmCoffSymbol | AsmElfSymbol,
                          "AsmGenericSymbol", "AsmGenericSymbolTag", false);
    AsmGenericSymbol.setCppCondition("!defined(DOCUMENTATION)");
    AsmGenericSymbol.setAutomaticGenerationOfConstructor(false);
    IS_SERIALIZABLE(AsmGenericSymbol);

#ifdef DOCUMENTATION
    /* Base class for symbol tables. */
    class SgAsmGenericSymbol: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Definition state.
         *
         * @{ */
        SymbolDefState get_def_state() const;
        void set_def_state(SymbolDefState);
        /** @} */
#else
        AsmGenericSymbol.setDataPrototype("SgAsmGenericSymbol::SymbolDefState", "def_state",
                                          "= SgAsmGenericSymbol::SYM_UNDEFINED",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Symbol binding.
         *
         * @{ */
        SymbolBinding get_binding() const;
        void set_binding(SymbolBinding);
        /** @} */
#else
        AsmGenericSymbol.setDataPrototype("SgAsmGenericSymbol::SymbolBinding", "binding", "= SgAsmGenericSymbol::SYM_NO_BINDING",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Symbol type.
         *
         * @{ */
        SymbolType get_type() const;
        void set_type(SymbolType);
        /** @} */
#else
        AsmGenericSymbol.setDataPrototype("SgAsmGenericSymbol::SymbolType", "type", "= SgAsmGenericSymbol::SYM_NO_TYPE",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Symbol value.
         *
         * @{ */
        rose_addr_t get_value() const;
        void set_value(rose_addr_t);
        /** @} */
#else
        AsmGenericSymbol.setDataPrototype("rose_addr_t", "value", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Symbol size in bytes.
         *
         * @{ */
        rose_addr_t get_size() const;
        void set_size(rose_addr_t);
        /** @} */
#else
        AsmGenericSymbol.setDataPrototype("rose_addr_t", "size", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Associated file section.
         *
         * @{ */
        SgAsmGenericSection* get_bound() const;
        void set_bound(SgAsmGenericSection*);
        /** @} */
#else
        AsmGenericSymbol.setDataPrototype("SgAsmGenericSection*", "bound", "= NULL",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        // Documented below
#else
        AsmGenericSymbol.setDataPrototype("SgAsmGenericString*", "name", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmGenericSymbol);
#if defined(SgAsmGenericSymbol_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_def_state);
            s & BOOST_SERIALIZATION_NVP(p_binding);
            s & BOOST_SERIALIZATION_NVP(p_type);
            s & BOOST_SERIALIZATION_NVP(p_value);
            s & BOOST_SERIALIZATION_NVP(p_size);
            s & BOOST_SERIALIZATION_NVP(p_bound);
            s & BOOST_SERIALIZATION_NVP(p_name);
        }
#endif

    public:
        /** Symbol definition state. */
        enum SymbolDefState {
            SYM_UNDEFINED,          /**< Symbol has not been defined yet. */
            SYM_TENTATIVE,          /**< Does not have size/value yet. E.g., uninitialized C or Fortran common blks. */
            SYM_DEFINED             /**< Created and assigned storage. */
        };

        /** Symbol type. */
        enum SymbolType {
            SYM_NO_TYPE,            /**< No type or type is unknown. */
            SYM_DATA,               /**< Normal variable definitions. */
            SYM_FUNC,               /**< Function or other code. */
            SYM_SECTION,            /**< Section of a file. */
            SYM_FILE,               /**< Name of a file. */
            SYM_ARRAY,              /**< Array of some other type. */
            SYM_TLS,                /**< Thread-local storage. */
            SYM_REGISTER,           /**< CPU register value. See Sparc. */
            SYM_COMMON,             /**< Common block. */
            SYM_IFUNC               /**< Indirection function. */
        };

        /** Symbol binding. */
        enum SymbolBinding {
            SYM_NO_BINDING,         /**< Unknown binding. */
            SYM_LOCAL,              /**< Local binding. */
            SYM_GLOBAL,             /**< Global binding. */
            SYM_WEAK                /**< Weak binding. */
        };

        /** Default constructor. */
        SgAsmGenericSymbol()
            : p_def_state(SYM_UNDEFINED), p_binding(SYM_NO_BINDING), p_type(SYM_NO_TYPE), p_value(0), p_size(0),
              p_bound(NULL), p_name(NULL) {
            ctor();
        }

        /** Print some debugging info. */
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const;

        // [Robb P Matzke 2017-03-22]: These are deprecated because the stringify facility is better.
        std::string stringifyDefState() const ROSE_DEPRECATED("use stringifySgAsmGenericSymbolSymbolDefState");
        std::string stringifyType() const ROSE_DEPRECATED("use stringifySgAsmGenericSymbolSymbolType");
        std::string stringifyBinding() const ROSE_DEPRECATED("use stringifySgAsmGenericSymbolSymbolBinding");

        /** Property: Name.
         *
         * @{ */
        SgAsmGenericString* get_name() const;
        void set_name(SgAsmGenericString*);
        /** @} */

    private:
        void ctor();
#endif // SgAsmGenericSymbol_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NEW_NONTERMINAL_MACRO(AsmGenericStrtab,
                          AsmElfStrtab | AsmCoffStrtab,
                          "AsmGenericStrtab", "AsmGenericStrtabTag", false);
    AsmGenericStrtab.setCppCondition("!defined(DOCUMENTATION)");
    AsmGenericStrtab.setAutomaticGenerationOfDestructor(false);
    IS_SERIALIZABLE(AsmGenericStrtab);

#ifdef DOCUMENTATION
    /** Base class for string tables. */
    class AsmGenericStrtab: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Section storing this string table.
         *
         * @{ */
        SgAsmGenericSection* get_container() const;
        void set_container(SgAsmGenericSection*);
        /** @} */
#else
        AsmGenericStrtab.setDataPrototype("SgAsmGenericSection*", "container", "= NULL",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: String storage list.
         *
         * @{ */
        referenced_t get_storage_list() const;
        void set_storage_list(referenced_t);
        /** @} */
#else
        AsmGenericStrtab.setDataPrototype("SgAsmGenericStrtab::referenced_t", "storage_list", "",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        // Documented below
#else
        AsmGenericStrtab.setDataPrototype("AddressIntervalSet", "freelist", "",
                                          NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Space that should never be freed.
         *
         * @{ */
        SgAsmStringStorage* get_dont_free() const;
        void set_dont_free(SgAsmStringStorage*);
        /** @} */
#else
        AsmGenericStrtab.setDataPrototype("SgAsmStringStorage*", "dont_free", "= NULL",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Number of strings freed thus far.
         *
         * @{ */
        size_t get_num_freed() const;
        void set_num_freed(size_t);
        /** @} */
#else
        AsmGenericStrtab.setDataPrototype("size_t", "num_freed", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmGenericStrtab);
#if defined(SgAsmGenericStrtab_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_container);
            s & BOOST_SERIALIZATION_NVP(p_storage_list);
            s & BOOST_SERIALIZATION_NVP(p_freelist);
            s & BOOST_SERIALIZATION_NVP(p_dont_free);
            s & BOOST_SERIALIZATION_NVP(p_num_freed);
        }
#endif

    public:
        typedef std::vector<class SgAsmStringStorage*> referenced_t;

        /** Constructs table inside file section. */
        explicit SgAsmGenericStrtab(SgAsmGenericSection *container)
            : p_container(container), p_dont_free(0), p_num_freed(0) {}

        virtual ~SgAsmGenericStrtab() {}

        // documented in base class
        virtual SgAsmGenericStrtab* parse() {return this;}

        /** Print some debugging info. */
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const;

        /** Constructs an SgAsmStoredString from an offset into this string table. */
        SgAsmStoredString *create_string(rose_addr_t offset, bool shared);

        /** Free area of this string table that corresponds to the string currently stored.
         *
         *  Use this in preference to the offset/size version of free() when possible. */
        void free(SgAsmStringStorage*);

        /** Add a range of bytes to the free list after subtracting areas that are referenced by other strings.
         *
         *  For instance, an ELF string table can have "main" and "domain" sharing storage. If we free the "domain" string then
         *  only "do" should be added to the free list. */
        void free(rose_addr_t offset, rose_addr_t size); /*mark part of table as free*/

        /** Free all strings so they will be reallocated later.
         *
         *  This is more efficient than calling @ref free for each storage object. If @p blow_away_holes is true then any areas
         *  that are unreferenced in the string table will be marked as referenced and added to the free list. */
        void free_all_strings(bool blow_away_holes=false);

        virtual void allocate_overlap(SgAsmStringStorage*) {};

        /** Allocates storage for strings that have been modified but not allocated.
         *
         *  First try to fit unallocated strings into free space. Any that are left will cause the string table to be
         *  extended. Returns true if the reallocation would potentially affect some other section. If @p shrink is true then
         *  release address space that's no longer needed at the end of the table. */
        bool reallocate(bool shrink);

        /** Property: Free space list.
         *
         *  This list stores space which is available for new strings.
         *
         * @{ */
        const AddressIntervalSet& get_freelist() const;
        AddressIntervalSet& get_freelist();
        /** @} */

        //These should be pure virtual but ROSETTA apparently doesn't support that (RPM 2008-10-03)
        virtual SgAsmStringStorage *create_storage(rose_addr_t /*offset*/, bool /*shared*/) {abort(); return NULL;}
        virtual rose_addr_t get_storage_size(const SgAsmStringStorage*) {abort(); return 0;}
        virtual void rebind(SgAsmStringStorage*, rose_addr_t) {abort();}

    private:
        void ctor();
#endif // SgAsmGenericStrtab_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmGenericSectionList);
    IS_SERIALIZABLE(AsmGenericSectionList);

#ifdef DOCUMENTATION
    /** List of pointers to file sections.
     *
     *  The only reason this node type exists is because ROSETTA limitations prevent the list from being stored directly in the
     *  nodes that need it. */
    class SgAsmGenericSectionList: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: List of section pointers.
         *
         * @{ */
        const SgAsmGenericSectionPtrList& get_sections() const;
        void set_sections(SgAsmGenericSectionPtrList&);
        /** @} */
#else
        AsmGenericSectionList.setDataPrototype("SgAsmGenericSectionPtrList", "sections", "",
                                               NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmGenericSectionList);
#if defined(SgAsmGenericSectionList_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_sections);
        }
#endif
#endif // SgAsmGenericSectionList_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    NEW_NONTERMINAL_MACRO(AsmGenericSection,
                          AsmGenericHeader | AsmElfSection | AsmElfSectionTable | AsmElfSegmentTable | AsmPESection |
                          AsmPESectionTable | AsmDOSExtendedHeader | AsmCoffSymbolTable | AsmNESection | AsmNESectionTable |
                          AsmNENameTable | AsmNEModuleTable | AsmNEStringTable | AsmNEEntryTable | AsmNERelocTable |
                          AsmLESection | AsmLESectionTable | AsmLENameTable | AsmLEPageTable | AsmLEEntryTable | AsmLERelocTable,
                          "AsmGenericSection", "AsmGenericSectionTag", true);
    AsmGenericSection.setCppCondition("!defined(DOCUMENTATION)");
    AsmGenericSection.setAutomaticGenerationOfDestructor(false);
    IS_SERIALIZABLE(AsmGenericSection);

    DECLARE_HEADERS(AsmGenericSection);
#if defined(SgAsmGenericSection_HEADERS) || defined(DOCUMENTATION)
#   include <MemoryMap.h>
#endif // SgAsmGenericSection_HEADERS

#ifdef DOCUMENTATION
    /** Contiguous region of a file.
     *
     *  This class represents any contiguous region of a file that serves a particular purpose. Not only are ELF and PE
     *  Sections represented by @ref SgAsmGenericSection, but also such things as file headers, segment and section tables,
     *  etc. */
    class SgAsmGenericSection: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: File to which this section belongs.
         *
         * @{ */
        SgAsmGenericFile* get_file() const;
        void set_file(SgAsmGenericFile*);
        /** @} */
#else
        AsmGenericSection.setDataPrototype("SgAsmGenericFile*", "file", "= NULL",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: File header that owns this section.
         *
         * @{ */
        SgAsmGenericHeader* get_header() const;
        void set_header(SgAsmGenericHeader*);
        /** @} */
#else
        AsmGenericSection.setDataPrototype("SgAsmGenericHeader*", "header", "= NULL",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        // Documented below
#else
        AsmGenericSection.setDataPrototype("rose_addr_t", "size", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        // Documented below
#else
        AsmGenericSection.setDataPrototype("rose_addr_t", "offset", "= 0", // Starting file offset of the section
                                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Required file alignment.
         *
         *  Zero and one both imply byte alignment.
         *
         * @{ */
        rose_addr_t get_file_alignment() const;
        void set_file_alignment(rose_addr_t);
        /** @} */
#else
        AsmGenericSection.setDataPrototype("rose_addr_t", "file_alignment", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Original content of just this section.
         *
         *  Points into file's content.
         *
         * @{ */
        const SgFileContentList& get_data() const;
        void set_data(const SgFileContentList&);
        /** @} */
#else
        AsmGenericSection.setDataPrototype("SgFileContentList", "data", "",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: General contents of the section.
         *
         * @{ */
        SectionPurpose get_purpose() const;
        void set_purpose(SectionPurpose);
        /** @} */
#else
        AsmGenericSection.setDataPrototype("SgAsmGenericSection::SectionPurpose", "purpose",
                                           "= SgAsmGenericSection::SP_UNSPECIFIED",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Whether section really exists.
         *
         *  If true, then this section was created by the format reader and not actually present in the file.
         *
         * @{ */
        bool get_synthesized() const;
        void set_synthesized(bool);
        /** @} */
#else
        AsmGenericSection.setDataPrototype("bool", "synthesized", "= false",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Non-unique section ID or negative.
         *
         *  ELF files have unique id numbers.
         *
         * @{ */
        int get_id() const;
        void set_id(int);
        /** @} */
#else
        AsmGenericSection.setDataPrototype("int", "id", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        // Documented below
#else
        // Non-unique name of section (may be empty string).  The name is not part of the AST traversal because if we made it
        // part of the traversal then we'd need to fix some of the subclasses which contain a solitary list data member (ROSE
        // cannot have IR nodes that contain a list and other data members).  Therefore, the SgAsmGenericSection destructor
        // will need to explicitly delete the name.
        AsmGenericSection.setDataPrototype("SgAsmGenericString*", "name", "= NULL",
                                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        // Documented below
#else
        AsmGenericSection.setDataPrototype("std::string", "short_name", "",
                                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        // Documented below
#else
        AsmGenericSection.setDataPrototype("rose_addr_t", "mapped_preferred_rva", "= 0", // DOXYGEN
                                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        // Documented below
#else
        AsmGenericSection.setDataPrototype("rose_addr_t", "mapped_size", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Alignment in virtual memory.
         *
         * @{ */
        rose_addr_t get_mapped_alignment() const;
        void set_mapped_alignment(rose_addr_t);
        /** @} */
#else
        AsmGenericSection.setDataPrototype("rose_addr_t", "mapped_alignment", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Whether mapped with read permission.
         *
         * @{ */
        bool get_mapped_rperm() const;
        void set_mapped_rperm(bool);
        /** @} */
#else
        AsmGenericSection.setDataPrototype("bool", "mapped_rperm", "= false",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Whether mapped with write permission.
         *
         * @{ */
        bool get_mapped_wperm() const;
        void set_mapped_wperm(bool);
        /** @} */
#else
        AsmGenericSection.setDataPrototype("bool", "mapped_wperm", "= false",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Whether mapped with execute permission.
         *
         * @{ */
        bool get_mapped_xperm() const;
        void set_mapped_xperm(bool);
        /** @} */
#else
        AsmGenericSection.setDataPrototype("bool", "mapped_xperm", "= false",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Whether a section is known to contain code.
         *
         *  If a section is known to contain code then this data member is set.  Part of the process of parsing a binary file
         *  is to look at the symbol tables and if any function symbol points into a section then the entire section is marked
         *  as containing code.  This is necessary because ELF Object Files (*.o files) contain one ELF Section per function,
         *  none of which have a memory mapping specification.  By marking those sections as containing code, the LoaderELFObj
         *  class, knows that the section should be mapped to virtual memory for disassembly.
         *
         * @{ */
        bool get_contains_code() const;
        void set_contains_code(bool);
        /** @} */
#else
        AsmGenericSection.setDataPrototype("bool", "contains_code", "= false",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Virtual address where ROSE maps this section.
         *
         *  The actual mapping is sometimes different than the preferred mapping indicated in the section table due to file
         *  and/or memory alignment constraints or conflicts with other sections.  The only place values are assigned to this
         *  data member is in the @ref BinaryLoader class and subclasses thereof.
         *
         *  The address corresponds to the latest call into the @ref BinaryLoader classes.  Depending on the loader employed,
         *  it's possible for a section to be mapped, this @c mapped_actual_va value to be set, and then some other section to
         *  be mapped over the top of all or part of the first section. In that case, the @c mapped_actual_va of the first
         *  section is not reset to zero.  The return value is not conditional upon @ref is_mapped since that predicate applies
         *  only to preferred mapping attributes.
         *
         * @{ */
        rose_addr_t get_mapped_actual_va() const;
        void set_mapped_actual_va(rose_addr_t);
        /** @} */
#else
        AsmGenericSection.setDataPrototype("rose_addr_t", "mapped_actual_va", "= 0",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmGenericSection);
#if defined(SgAsmGenericSection_OTHERS) || defined(DOCUMENTATION)
    private:
        /* This is an optional local, writable pool for the p_data member. Normally a section will point into the pool
         * for its SgAsmGenericFile which is memory-mapped (read-only) from the actual file being parsed. The default
         * unparsing action is to write the original data back to the file. By allowing a section to allocate its own
         * pool for p_data we create a very easy way to get data into the unparsed file (the alternative is to derive
         * a new class and override the unparse() method). */
        unsigned char *local_data_pool;

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_file);
            s & BOOST_SERIALIZATION_NVP(p_header);
            s & BOOST_SERIALIZATION_NVP(p_size);
            s & BOOST_SERIALIZATION_NVP(p_offset);
            s & BOOST_SERIALIZATION_NVP(p_file_alignment);
            s & BOOST_SERIALIZATION_NVP(p_data);
            s & BOOST_SERIALIZATION_NVP(p_purpose);
            s & BOOST_SERIALIZATION_NVP(p_synthesized);
            s & BOOST_SERIALIZATION_NVP(p_id);
            s & BOOST_SERIALIZATION_NVP(p_name);
            s & BOOST_SERIALIZATION_NVP(p_short_name);
            s & BOOST_SERIALIZATION_NVP(p_mapped_preferred_rva);
            s & BOOST_SERIALIZATION_NVP(p_mapped_size);
            s & BOOST_SERIALIZATION_NVP(p_mapped_alignment);
            s & BOOST_SERIALIZATION_NVP(p_mapped_rperm);
            s & BOOST_SERIALIZATION_NVP(p_mapped_wperm);
            s & BOOST_SERIALIZATION_NVP(p_mapped_xperm);
            s & BOOST_SERIALIZATION_NVP(p_contains_code);
            s & BOOST_SERIALIZATION_NVP(p_mapped_actual_va);
            // s & BOOST_SERIALIZATION_NVP(local_data_pool); -- not serialized, initialized to null
        }
#endif

    public:
        /** Reason for section's existence.
         *
         *  This is a union of all the section purposes from the various supported file formats.  However, at this time we're
         *  only interested in a few kinds of sections and therefore most will fall into the SP_OTHER category. */
        enum SectionPurpose {
            SP_UNSPECIFIED,                 /**< File format did not specify a reason and none could be determined. */
            SP_PROGRAM,                     /**< Program-supplied data, code, etc. */
            SP_HEADER,                      /**< Section contains a header for the executable file format. */
            SP_SYMTAB,                      /**< Symbol table. */
            SP_OTHER                        /**< File-specified purpose other than any given in this enum. */
        };

    public:
        /** Constructor linking object into ASt.
         *
         *  Section constructors set the optional section header relationship--a bidirectional link between this new section
         *  and its optional, single header.  This new section points to its header and the header contains a list that points
         *  to this new section.  The section-to-header part of the link is deleted by the default destructor by virtue of
         *  being a simple pointer, but we also need to delete the other half of the link in the destructors. */
        SgAsmGenericSection(SgAsmGenericFile *f, SgAsmGenericHeader *fhdr)
            : local_data_pool(NULL), p_file(f), p_header(NULL), p_size(1), p_offset(f->get_current_size()),
              p_file_alignment(0), p_purpose(SP_UNSPECIFIED), p_synthesized(false), p_id(-1), p_name(0),
              p_mapped_preferred_rva(0), p_mapped_size(0), p_mapped_alignment(0), p_mapped_rperm(false),
              p_mapped_wperm(false), p_mapped_xperm(false), p_contains_code(false), p_mapped_actual_va(0) {
            ctor(f, fhdr);
        }

        // This destructor modifies the SgAsmGenericFile data to remove its section from the section list!
        virtual ~SgAsmGenericSection();

        /** Prints info about offsets into known sections. */
        static void dump_containing_sections(FILE*, const std::string &prefix, rose_rva_t, const SgAsmGenericSectionPtrList&);

        /** Saves a reference to the original file data for a section based on the section's current offset and size.
         *
         *  Once this happens, changing the offset or size of the file will not affect the original data. The original data can
         *  be extended, however, by calling @ref extend, which is typically done during parsing. */
        void grab_content();

        // DQ (10/20/2010): Moved this function's definition to the source file.
        virtual SgAsmGenericSection* parse();

        /** Print some debugging info. */
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const;

        // Implemented in subclasses
        virtual bool reallocate() { return false; }

        /** Write a section back to the file.
         *
         *  This is the generic version that simply writes the content. Subclasses should override this. */
        virtual void unparse(std::ostream&) const;

        /** Write just the specified regions back to the file */
        void unparse(std::ostream&, const ExtentMap&) const;

        /** Write holes (unreferenced areas) back to the file */
        void unparse_holes(std::ostream&) const;

        /** Predicate determining whether this section is also a top-level file header.
         *
         *  Returns true (the associated @ref SgAsmGenericHeader pointer) if this section is a top-level file header, false
         *  (NULL) otherwise. */
        SgAsmGenericHeader *is_file_header();

        /** Property: Size of section in file in bytes.
         *
         *  This is the current file size of the section in bytes as it exists in the file.. The original size of the
         *  section (available when @ref parse is called for the function, but possibly updated while parsing) is available
         *  through the size of the original @ref p_data "data" property.
         *
         *  When ths size is set, some types of sections may do additional work. That additional work must not adjust the size
         *  of other sections or the mapping of any section (use @ref SgAsmGenericFile::resize to do that).
         *
         * @{ */
        rose_addr_t get_size() const;
        virtual void set_size(rose_addr_t);
        /** @} */

        /** Property: Offset to start of section in file.
         *
         *  This property holds the current file offset in bytes of this section. When setting the property, some types of
         *  sections may need to do perform additional work. That additional work must not adjust the offset of other sections,
         *  or the mapping of any section.
         *
         * @{ */
        rose_addr_t get_offset() const;
        virtual void set_offset(rose_addr_t);
        /** @} */

        /** File offset for end of section. */
        rose_addr_t   get_end_offset() const;

        /** Extend a section by some number of bytes during the construction and/or parsing phase.
         *
         *  This is function is considered to be part of the parsing and construction of a section--it changes the part of the
         *  file that's considered the "original size" of the section. To adjust the size of a section after the executable
         *  file is parsed, see @ref SgAsmGenericFile::resize.  Sections are allowed to extend beyond the end of the file and
         *  the original data (the @ref get_data "data" property) is extended only up to the end of the file. */
        void extend(rose_addr_t nbytes);

        /** Write data to a file section.
         *
         *  @param f       Output steam to which to write
         *  @param offset  Byte offset relative to start of this section
         *  @param bufsize Size of @p buf in bytes
         *  @param buf     Buffer of bytes to be written
         *
         *  @returns Returns the section-relative byte offset for the first byte beyond what would have been written if all
         *  bytes of the buffer were written.
         *
         *  The buffer is allowed to extend past the end of the section as long as the part that extends beyond is all
         *  zeros. The zeros will not be written to the output file.  Furthermore, any trailing zeros that extend beyond the
         *  end of the file will not be written (end-of-file is determined by @ref SgAsmGenericFile::get_orig_size).
         *
         * @{ */
        rose_addr_t   write(std::ostream &f, rose_addr_t offset, size_t bufsize, const void *buf) const;
        rose_addr_t   write(std::ostream &f, rose_addr_t offset, const std::string &str) const;
        rose_addr_t   write(std::ostream &f, rose_addr_t offset, char c) const;
        rose_addr_t   write(std::ostream &f, rose_addr_t offset, const SgFileContentList &buf) const;
        rose_addr_t   write(std::ostream &f, rose_addr_t offset, const SgUnsignedCharList &buf) const;
        /** @} */

        /** Write an unsigned little-endian 128-bit value.
         *
         *  Encode an unsigned value as LEB128 and return the next offset. */
        rose_addr_t   write_uleb128(unsigned char*, rose_addr_t offset, uint64_t) const;

        /** Write a signed little-endian 128-bit value.
         *
         *  Encode an signed value as LEB128 and return the next offset. */
        rose_addr_t   write_sleb128(unsigned char*, rose_addr_t offset, int64_t) const;

        /** Reads data from a file.
         *
         *  Reads up to @p size bytes of data beginning at byte @p start_offset from the beginning of the file, placing the
         *  results in @p dst_buf and returning the number of bytes read. The return value could be smaller than @p size if the
         *  end-of-file is reached. If the return value is smaller than @p size then one of two things happen: if @p strict is
         *  set (the default) then an @ref SgAsmExecutableFileFormat::ShortRead exception is thrown; otherwise the @p dst_buf
         *  will be padded with zero bytes so that exactly @p size bytes of @p dst_buf are always initialized. */
        size_t read_content(rose_addr_t abs_offset, void *dst_buf, rose_addr_t size, bool strict=true);

        /** Reads data from a file.
         *
         *  Reads up to @p size bytes of data beginning at byte @p start (absolute or relative virtual address) in the mapped
         *  address space and placing the results in @p dst_buf and returning the number of bytes read. The return value could
         *  be smaller than @p size if the reading encounters virtual addresses that are not mapped.  When an unmapped virtual
         *  address is encountered the reading stops (even if subsequent virtual addresses are defined) and one of two things
         *  happen: if @p strict is set (the default) then an @ref MemoryMap::NotMapped exception is thrown, otherwise the @p
         *  dst_buf is padded with zeros so that all @p size bytes are initialized. The @p map is used to map virtual addresses
         *  to file offsets; if @p map is NULL then the map defined in the underlying file is used.
         *
         * @{ */
        size_t read_content(const Rose::BinaryAnalysis::MemoryMap::Ptr&, rose_addr_t start,  void *dst_buf,
                            rose_addr_t size, bool strict=true);
        size_t read_content(const Rose::BinaryAnalysis::MemoryMap::Ptr&, const rose_rva_t &start, void *dst_buf,
                            rose_addr_t size, bool strict=true);
        /** @} */

        /** Reads data from a file.
         *
         *  This behaves the same as @ref read_content except the starting offset is relative to the beginning of this section.
         *  Reading past the end of the section is not allowed and treated as a short read, and one of two things happen: if @p
         *  strict is set (the default) then an @ref SgAsmExecutableFileFormat::ShortRead exception is thrown, otherwise the
         *  result is zero padded so as to contain exactly @p size bytes. */
        size_t read_content_local(rose_addr_t rel_offset, void *dst_buf, rose_addr_t size, bool strict=true);

        /** Reads a string from the file.
         *
         *  The string begins at the specified virtual address and continues until the first NUL byte or until we reach an
         *  address that is not mapped. However, if @p strict is set (the default) and we reach an unmapped address then an
         *  @ref MemoryMap::NotMapped exception is thrown. The @p map defines the mapping from virtual addresses to file
         *  offsets; if @p map is NULL then the map defined in the underlying file is used. */
        std::string read_content_str(const Rose::BinaryAnalysis::MemoryMap::Ptr&, rose_addr_t va, bool strict=true);

        /** Reads a string from the file.
         *
         *  The string begins at the specified absolute or header-relative file offset and continues until the first NUL byte
         *  or end of file is reached. However, if @p strict is set (the default) and we reach the end-of-file then an @ref
         *  SgAsmExecutableFileFormat::ShortRead exception is thrown.
         *
         * @{ */
        std::string read_content_str(rose_addr_t abs_offset, bool strict=true);
        std::string read_content_str(const Rose::BinaryAnalysis::MemoryMap::Ptr &map, rose_rva_t rva, bool strict=true) {
            return read_content_str(map, rva.get_va(), strict);
        }
        /** @} */

        /** Reads a string from the file.
         *
         *  The string begins at the specified file offset relative to the start of this section and continues until the first
         *  NUL byte or the end of section is reached. However, if @p strict is set (the default) and we reach the
         *  end-of-section then an @ref SgAsmExecutableFileFormat::ShortRead exception is thrown. */
        std::string read_content_local_str(rose_addr_t rel_offset, bool strict=true);

        /** Reads content of a section and returns it as a container.
         *
         *  The returned container will always have exactly @p size byte.  If @p size bytes are not available in this section
         *  at the specified offset then the container will be zero padded. This method always behaves as a non-strict read. */
        SgUnsignedCharList read_content_local_ucl(rose_addr_t rel_offset, rose_addr_t size);

        /** Read a signed little-endian 128-bit value.
         *
         *  Extract a signed LEB128 value and adjust @p rel_offset according to how many bytes it occupied. If @p strict is set
         *  (the default) and the end of the section is reached then throw an @ref SgAsmExecutableFileFormat::ShortRead
         *  exception. Upon return, the @p rel_offset will be adjusted to point to the first byte after the LEB128 value. */
        int64_t read_content_local_sleb128(rose_addr_t *rel_offset, bool strict=true);

        /** Read an unsigned little-endian 128-bit value.
         *
         *  Extract an unsigned LEB128 value and adjust @p rel_offset according to how many bytes it occupied.  If @p strict is
         *  set (the default) and the end of the section is reached then throw an @ref SgAsmExecutableFileFormat::ShortRead
         *  exception. Upon return, the @p rel_offset will be adjusted to point to the first byte after the LEB128 value. */
        uint64_t read_content_local_uleb128(rose_addr_t *rel_offset, bool strict=true);

        /** Obtain a local, writable pool to hold content.
         *
         *  Sections typically point into the memory mapped, read-only file stored in the SgAsmGenericFile parent initialized
         *  by calling @ref grab_content (or indirectly by calling @ref parse).  This is also the same data which is, by
         *  default, written back out to the new file during @ref unparse.  Programs modify section content by either
         *  overriding the @ref unparse method or by modifying the @ref p_data "data" property. But in order to modify @ref
         *  p_data "data" we have to make sure that it's pointing to a read/write memory pool. This function replaces the
         *  read-only memory pool with a new one containing @p nbytes bytes of zeros. */
        unsigned char *writable_content(size_t nbytes);

        /** Returns a list of parts of a single section that have been referenced.
         *
         *  The offsets are relative to the start of the section. The tracking actually happens at the entire file level (see
         *  @ref SgAsmGenericFile::get_referenced_extents) and this function returns that same information but limits the
         *  results to this section, and returns section offsets rather than file offsets. */
        AddressIntervalSet get_referenced_extents() const;

        /** Returns a list of parts of a single section that have not been referenced.
         *
         *  The offsets are relative to the start of the section. The tracking actually happens at the entire file level
         *  (see @ref SgAsmGenericFile::get_unreferenced_extents) and this function returns that same information but
         *  limits the results to this section, and returns section offsets rather than file offsets. */
        AddressIntervalSet get_unreferenced_extents() const;

        /** Whether section desires to be mapped to memory.
         *
         *  This predicate is true iff this section has a non-zero mapped address and size. */
        bool is_mapped() const;

        /** Causes section to not be mapped to memory.
         *
         *  This method sets the mapped address and size to zero. */
        void clear_mapped();

        /** Property: Mapped size.
         *
         *  Size of section in bytes when it's mapped into virtual memory.
         *
         * @{ */
        rose_addr_t get_mapped_size() const;
        virtual void set_mapped_size(rose_addr_t);
        /** @} */

        /** Property: Relative virtual address where section prefers to be mapped.
         *
         *  This is the relative virtual address where the section @em prefers to be mapped. The RVA where the section will
         *  ultimately be mapped by the loader might be different due to file and/or memory alignment constraints and conflicts
         *  with other sections.
         *
         *  The virtual address is relative to the base address stored in the file header.
         *
         * @{ */
        rose_addr_t get_mapped_preferred_rva() const;
        virtual void set_mapped_preferred_rva(rose_addr_t);
        /** @} */

        /** Base virtual address for a section.
         *
         *  Returns  zero if the section is not associated with a header.  This is just a convenience method to get the base
         *  virtual address of the file header that owns this section. */
        rose_addr_t get_base_va() const;

        /** Virtual address where section prefers to be mapped.
         *
         *  Returns (non-relative) virtual address if mapped, zero otherwise. See also, the @ref get_mapped_preferred_rva
         *  "mapped_preferred_rva" property. */
        rose_addr_t get_mapped_preferred_va() const;

        /** File offset for specified virtual address.
         *
         *  Returns the file offset associated with the virtual address of a mapped section. The @ref MemoryMap class is a
         *  better interface to this same information. */
        rose_addr_t get_va_offset(rose_addr_t va) const;

        /** File offset for specified relative virtual address.
         *
         *  Returns the file offset associated with the relative virtual address of a mapped section.  The @ref MemoryMap class
         *  is a better interface to this same information. */
        rose_addr_t get_rva_offset(rose_addr_t rva) const;

        /** Property: Non-unique name of section.
         *
         *  The name may be the empty string. If you just want to change the name of a section use the existing name node and
         *  change its string value.  Assigning a new @ref SgAsmGenericString to the section also changes the parent of the
         *  specified string node.
         *
         * @{ */
        SgAsmGenericString *get_name() const;
        void set_name(SgAsmGenericString *s);
        /** @} */

        /** Property: Abbreviated name.
         *
         *  Some sections have long names like "Import Address Table" that are cumbersome when they appear in assembly
         *  listings.  Therefore, each section may also have a short name.  Reading this property returns the short name
         *  if it's non-empty, otherwise the full name.
         *
         * @{ */
        std::string get_short_name() const;
        void set_short_name(const std::string&);
        /** @} */

        /** Returns the file extent for the section.
         *
         *  The extent end points are determined by calling @ref get_offset and @ref get_size. */
        Extent get_file_extent() const;

        /** Returns the memory extent for a mapped section.
         *
         *  If the section is not mapped then offset and size will be zero. The return value is computed from the @ref
         *  get_mapped_preferred_rva "mapped_preferred_rva" and @ref get_mapped_size "mapped_size" properties. */
        Extent get_mapped_preferred_extent() const;

        /** Increase file offset and mapping address to satisfy alignment constraints.
         *
         *  This is typically done when initializing a new section. The constructor places the new section at the end of the
         *  file before it knows what the alignment constraints will be. The user should then set the alignment constraints
         *  (see @ref set_file_alignment "file_alignment" and @ref set_mapped_alignment "mapped_alignment" properties) and call
         *  this method.  This method must be called before any additional sections are appended to the file.
         *
         *  The file offset and memory mapping address are adjusted independently.
         *
         *  On the other hand, if additional sections are in the way, they must first be moved out of the way with the
         *  @ref SgAsmGenericFile::shift_extend method.
         *
         *  Returns true if the file offset and/or mapping address changed as a result of this call. */
        bool align();

    protected:
        void ctor(SgAsmGenericFile*, SgAsmGenericHeader*);
#endif // SgAsmGenericSection_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmGenericDLLList);
    IS_SERIALIZABLE(AsmGenericDLLList);

#ifdef DOCUMENTATION
    /** List of pointers to other nodes.
     *
     *  The only reason this node type exists is because ROSETTA limitations prevent the list from being included directly in
     *  the nodes that need the list. */
    class SgAsmGenericDLLList: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: List of AST node pointers.
         *
         * @{ */
        const SgAsmGenericDLLPtrList& get_dlls() const;
        void set_dlls(const SgAsmGenericDLLPtrList&);
        /** @} */
#else
        AsmGenericDLLList.setDataPrototype("SgAsmGenericDLLPtrList", "dlls", "",
                                           NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmGenericDLLList);
#if defined(SgAsmGenericDLLList_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_dlls);
        }
#endif
#endif // SgAsmGenericDLLList_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmGenericDLL);
    IS_SERIALIZABLE(AsmGenericDLL);

#ifdef DOCUMENTATION
    /** Base class for dynamically linked library information. */
    class SgAsmGenericDLL: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        // Documented below
#else
        AsmGenericDLL.setDataPrototype("SgAsmGenericString*", "name", "= 0",
                                       NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
#endif

#ifdef DOCUMENTATION
        /** Property: Symbols.
         *
         * @{ */
        const SgStringList& get_symbols();
        const set_symbols(const SgStringList&);
        /** @} */
#else
        AsmGenericDLL.setDataPrototype("SgStringList", "symbols", "",
                                       NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
#endif

        DECLARE_OTHERS(AsmGenericDLL);
#if defined(SgAsmGenericDLL_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_name);
            s & BOOST_SERIALIZATION_NVP(p_symbols);
        }
#endif

    public:
        /** Constructor that initializes the name. */
        explicit SgAsmGenericDLL(SgAsmGenericString *s)
            : p_name(s) {
            ctor();
        }

        /** Print some debugging info. */
        virtual void dump(FILE*, const char *prefix, ssize_t idx) const;

        /** Add a needed symbol to the import list for this DLL. */
        void add_symbol(const std::string &s) {
            p_symbols.push_back(s);
        }

        /** Property: Name.
         *
         * @{ */
        SgAsmGenericString *get_name() const;
        void set_name(SgAsmGenericString *s);
        /** @} */

    private:
        void ctor();
#endif // SgAsmGenericDLL_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmGenericFormat);
    IS_SERIALIZABLE(AsmGenericFormat);

#ifdef DOCUMENTATION
    /** Basic information about an executable container. */
    class SgAsmGenericFormat: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: Family.
         *
         * @{ */
        ExecFamily get_family() const;
        void set_family(ExecFamily);
        /** @} */
#else
        AsmGenericFormat.setDataPrototype("SgAsmGenericFormat::ExecFamily", "family", "= SgAsmGenericFormat::FAMILY_UNSPECIFIED",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Purpose.
         *
         * @{ */
        ExecPurpose get_purpose() const;
        void set_purpose(ExecPurpose);
        /** @} */
#else
        AsmGenericFormat.setDataPrototype("SgAsmGenericFormat::ExecPurpose", "purpose",
                                          "= SgAsmGenericFormat::PURPOSE_EXECUTABLE",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Byte order.
         *
         * @{ */
        ByteOrder::Endianness get_sex() const;
        void set_sex(ByteOrder::Endianness);
        /** @} */
#else
        AsmGenericFormat.setDataPrototype("ByteOrder::Endianness", "sex", "= ByteOrder::ORDER_UNSPECIFIED",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: File format version number.
         *
         * @{ */
        unsigned get_version() const;
        void set_version(unsigned);
        /** @} */
#else
        AsmGenericFormat.setDataPrototype("unsigned", "version", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Whether version is considered current.
         *
         *  True if the @ref get_version "version" property is considered to be the current, supported version for this file
         *  format.
         *
         * @{ */
        bool get_is_current_version() const;
        void set_is_current_version(bool);
        /** @} */
#else
        AsmGenericFormat.setDataPrototype("bool", "is_current_version", "= false",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Application binary interface.
         *
         * @{ */
        ExecABI get_abi() const;
        void set_abi(ExecABI);
        /** @} */
#else
        AsmGenericFormat.setDataPrototype("SgAsmGenericFormat::ExecABI", "abi", "= SgAsmGenericFormat::ABI_UNSPECIFIED",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Application binary interface version.
         *
         * @{ */
        unsigned get_abi_version() const;
        void set_abi_version(unsigned);
        /** @} */
#else
        AsmGenericFormat.setDataPrototype("unsigned", "abi_version", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Natural word size in bytes.
         *
         * @{ */
        size_t get_word_size() const;
        void set_word_size(size_t);
        /** @} */
#else
        AsmGenericFormat.setDataPrototype("size_t", "word_size", "= 0",
                                          NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmGenericFormat);
#if defined(SgAsmGenericFormat_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_family);
            s & BOOST_SERIALIZATION_NVP(p_purpose);
            s & BOOST_SERIALIZATION_NVP(p_sex);
            s & BOOST_SERIALIZATION_NVP(p_version);
            s & BOOST_SERIALIZATION_NVP(p_is_current_version);
            s & BOOST_SERIALIZATION_NVP(p_abi);
            s & BOOST_SERIALIZATION_NVP(p_abi_version);
            s & BOOST_SERIALIZATION_NVP(p_word_size);
        }
#endif

    public:
        /** Print some debugging info. */
        void dump(FILE*, const char *prefix, ssize_t idx) const;
#endif // SgAsmGenericFormat_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    DECLARE_LEAF_CLASS(AsmGenericFileList);
    IS_SERIALIZABLE(AsmGenericFileList);

#ifdef DOCUMENTATION
    /** List of AST file node pointers.
     *
     *  The only reason this node type exists is because ROSETTA limitations prevent the list from being contained directly in
     *  the nodes that need it. */
    class SgAsmGenericFileList: public SgAsmNode {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: List of AST node pointers.
         *
         * @{ */
        const SgAsmGenericFilePtrList& get_files() const;
        void set_files(const SgAsmGenericFilePtrList&);
        /** @} */
#else
        AsmGenericFileList.setDataPrototype("SgAsmGenericFilePtrList", "files", "",
                                            NO_CONSTRUCTOR_PARAMETER, BUILD_LIST_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmGenericFileList);
#if defined(SgAsmGenericFileList_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmNode);
            s & BOOST_SERIALIZATION_NVP(p_files);
        }
#endif
#endif // SgAsmGenericFileList_OTHERS

#ifdef DOCUMENTATION
    };
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    DECLARE_LEAF_CLASS(AsmGenericFile);
    IS_SERIALIZABLE(AsmGenericFile);
    AsmGenericFile.setAutomaticGenerationOfConstructor(false);
    AsmGenericFile.setAutomaticGenerationOfDestructor(false);

    DECLARE_HEADERS(AsmGenericFile);
#if defined(SgAsmGenericFile_HEADERS) || defined(DOCUMENTATION)
    #include "MemoryMap.h"
    #include "DataConversion.h"
    #include "StatSerializer.h"                         // non-intrusive serialization of struct stat
#endif // SgAsmGenericFile_HEADERS

#ifdef DOCUMENTATION
    /** Base class for binary files. */
    class SgAsmGenericFile: public SgAsmExecutableFileFormat {
    public:
#endif

#ifdef DOCUMENTATION
        /** Property: DWARF debugging hiearchy.
         *
         * @{ */
        SgAsmDwarfCompilationUnitList* get_dwarf_info() const;
        void set_dwarf_info(SgAsmDwarfCompilationUnitList*);
        /** @} */
#else
        // The "dwarf_info" member is first as an optimization: a single AST traversal will see the dwarf information first and
        // therefore have the necessary debugging information already available when the traversal gets to the rest of the AST.
        AsmGenericFile.setDataPrototype("SgAsmDwarfCompilationUnitList*", "dwarf_info", "= NULL",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Name of file.
         *
         *  @{ */
        const std::string& get_name() const;
        void set_name(const std::string&);
        /** @} */
#else
        AsmGenericFile.setDataPrototype("std::string", "name", "= \"\"",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Read-only file descriptor.
         *
         *  Negative represents lack of valid file descriptor.
         *
         * @{ */
        int get_fd() cont;
        void set_fd(int);
        /** @} */
#else
        AsmGenericFile.setDataPrototype("int", "fd", "= -1",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        // Documented below
#else
        // File attributes at time of file open (valid if fd>=0)
        AsmGenericFile.setDataPrototype("SgAsmGenericFormat::fileDetails", "sb", "",
                                        NO_CONSTRUCTOR_PARAMETER, NO_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Contents of the file.
         *
         * @{ */
        const SgFileContentList& get_data() const;
        void set_data(const SgFileContentList&);
        /** @} */
#else
        AsmGenericFile.setDataPrototype("SgFileContentList", "data", "",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: List of all headers in file.
         *
         *  This is stored as a pointer to another node that contains the list due to limitations of ROSETTA.
         *
         * @{ */
        SgAsmGenericHeaderList* get_headers() const;
        void set_headers(SgAsmGenericHeaderList*);
        /** @} */
#else
        AsmGenericFile.setDataPrototype("SgAsmGenericHeaderList*", "headers", "= NULL",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Addresses unreferenced during parsing.
         *
         *  This is a pointer to a node that contains a list of pointers to the holes. It is done this way rather than storing
         *  the list directly, because of limitations of ROSETTA.
         *
         * @{ */
        SgAsmGenericSectionList* get_holes() const;
        void set_holes(SgAsmGenericSectionList*);
        /** @} */
#else
        AsmGenericFile.setDataPrototype("SgAsmGenericSectionList*", "holes", "= NULL",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, DEF_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Whether to truncate zeros on writes.
         *
         * @{ */
        bool get_truncate_zeros() const;
        void set_truncate_zeros(bool);
        /** @} */
#else
        AsmGenericFile.setDataPrototype("bool", "truncate_zeros", "= false",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_FLAG_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Whether to track referenced areas during parsing.
         *
         * @{ */
        bool get_tracking_references() const;
        void set_tracking_references(bool);
        /** @} */
#else
        AsmGenericFile.setDataPrototype("bool", "tracking_references", "= true",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: Addresses referenced during parsing.
         *
         * @{ */
        const AddressIntervalSet& get_referenced_extents() const;
        void set_referenced_extents(const AddressIntervalSet&);
        /** @} */
#else
        AsmGenericFile.setDataPrototype("AddressIntervalSet", "referenced_extents", "",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

#ifdef DOCUMENTATION
        /** Property: If set, refuse to unparse file.
         *
         * @{ */
        bool get_neuter() const;
        void set_neuter(true);
        /** @} */
#else
        AsmGenericFile.setDataPrototype("bool", "neuter", "= false",
                                        NO_CONSTRUCTOR_PARAMETER, BUILD_ACCESS_FUNCTIONS, NO_TRAVERSAL, NO_DELETE);
#endif

        DECLARE_OTHERS(AsmGenericFile);
#if defined(SgAsmGenericFile_OTHERS) || defined(DOCUMENTATION)
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;

        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(SgAsmExecutableFileFormat);
            s & BOOST_SERIALIZATION_NVP(p_dwarf_info);
            s & BOOST_SERIALIZATION_NVP(p_name);
            // s & BOOST_SERIALIZATION_NVP(p_fd); -- not serialized
            s & BOOST_SERIALIZATION_NVP(p_sb);
            s & BOOST_SERIALIZATION_NVP(p_data);
            s & BOOST_SERIALIZATION_NVP(p_headers);
            s & BOOST_SERIALIZATION_NVP(p_holes);
            s & BOOST_SERIALIZATION_NVP(p_truncate_zeros);
            s & BOOST_SERIALIZATION_NVP(p_tracking_references);
            s & BOOST_SERIALIZATION_NVP(p_referenced_extents);
            s & BOOST_SERIALIZATION_NVP(p_neuter);
            s & BOOST_SERIALIZATION_NVP(p_unreferenced_cache);
            // s & BOOST_SERIALIZATION_NVP(p_data_converter); -- function pointer not serialized
        }
#endif
    private:
        mutable AddressIntervalSet *p_unreferenced_cache;
        DataConverter *p_data_converter;

    public:
        /** Section modification functions for @ref shift_extend. */
        enum AddressSpace {
            ADDRSP_MEMORY = 0x0001,
            ADDRSP_FILE   = 0x0002,
            ADDRSP_ALL    = 0x0003
        };

        /** Elasticity argument for @ref shift_extend. */
        enum Elasticity {
            ELASTIC_NONE  = 0,                          /**< Nothing is elastic; other parts of space are shifted. */
            ELASTIC_UNREF = 1,                          /**< Unreferenced address space is elastic. */
            ELASTIC_HOLE  = 2                           /**< Unreferenced and "hole" sections are elastic. */
        };
    public:
        /** Non-parsing constructor.
         *
         *  If you're creating an executable from scratch then call this function and you're done. But if you're parsing an
         *  existing file then call @ref parse in order to map the file's contents into memory for parsing. */
        SgAsmGenericFile()
            : p_unreferenced_cache(NULL), p_data_converter(NULL), p_dwarf_info(NULL), p_fd(-1), p_headers(NULL),
              p_holes(NULL), p_truncate_zeros(false), p_tracking_references(true), p_neuter(false) {
            ctor();
        }

        /** Destructor deletes children and unmaps/closes file. */
        virtual ~SgAsmGenericFile();

        /** Loads file contents into memory. */
        SgAsmGenericFile* parse(std::string file_name);

        /** Call this before unparsing to make sure everything is consistent. */
        void reallocate();

        /** Mirror image of parsing an executable file.
         *
         *  The result (unless the AST has been modified) should be identical to the original file.  If the file's neuter
         *  property is true, then rather than creating a binary file, the output will contain a note indicating that the
         *  neuter property is set.  This is intended to prevent ASTs that represent malicious binaries from accidently being
         *  used to create the binary. */
        void unparse(std::ostream&) const;

        /** Extend the output file by writing the last byte if it hasn't been written yet. */
        void extend_to_eof(std::ostream&) const;

        /** Print basic info about the sections of a file. */
        void dump(FILE*) const;

        /** Print text file containing all known information about a binary file.
         *
         *  If in_cwd is set, then the file is created in the current working directory rather than the directory containing
         *  the binary file (the default is to create the file in the current working directory).  If @p ext is non-null then
         *  these characters are added to the end of the binary file name. The default null pointer causes the string ".dump"
         *  to be appended to the file name. */
        void dump_all(bool in_cwd=true, const char *ext=NULL);

        /** Print text file containing all known information about a binary file. */
        void dump_all(const std::string& dumpname);

        /** Returns the parts of the file that have never been referenced. */
        const AddressIntervalSet& get_unreferenced_extents() const;

        /** Marks part of a file as having been referenced if tracking references. */
        void mark_referenced_extent(rose_addr_t start_rva, rose_addr_t size);

        /** Property: Data converter.
         *
         *  Function to encode/decode data as it's transferred to/from disk.  The default is to do no transformation.
         *
         * @{ */
        void set_data_converter(DataConverter* dc) {p_data_converter=dc;}
        DataConverter* get_data_converter() const {return p_data_converter;}
        /** @} */

        /** Returns current size of file based on section with highest ending address. */
        rose_addr_t get_current_size() const;

        /** Returns original size of file, based on file system. */
        rose_addr_t get_orig_size() const;

        /** Reads data from a file.
         *
         *  Reads up to @p size bytes of data from the file beginning at the specified byte offset (measured from the beginning
         *  of the file), placing the result in @p dst_buf, and returning the number of bytes read. If the number of bytes read
         *  is less than @p size then one of two things happen: if @p strict is true then an @ref
         *  SgAsmExecutableFileFormat::ShortRead exception is thrown; otherwise @p dst_buf is zero padded so that exactly @p
         *  size bytes are always initialized. */
        size_t read_content(rose_addr_t offset, void *dst_buf, rose_addr_t size, bool strict=true);

        /** Reads data from a file.
         *
         *  Reads up to @p size bytes of data starting at the specified (absolute) virtual address. The @p map specifies how
         *  virtual addresses are mapped to file offsets.  As bytes are read, if we encounter a virtual address that is not
         *  mapped we stop reading and do one of two things: if @p strict is set then a @ref MemoryMap::NotMapped exception is
         *  thrown; otherwise the rest of the @p dst_buf is zero filled and the number of bytes read (not filled) is
         *  returned. */
        size_t read_content(const Rose::BinaryAnalysis::MemoryMap::Ptr&, rose_addr_t va, void *dst_buf,
                            rose_addr_t size, bool strict=true);

        /** Reads a string from a file.
         *
         *  Returns the string stored at the specified (absolute) virtual address. The returned string contains the bytes
         *  beginning at the starting virtual address and continuing until we reach a NUL byte or an address which is not
         *  mapped. If we reach an address which is not mapped then one of two things happen: if @p strict is set then a @ref
         *  MemoryMap::NotMapped exception is thrown; otherwise the string is simply terminated. The returned string does not
         *  include the NUL byte. */
        std::string read_content_str(const Rose::BinaryAnalysis::MemoryMap::Ptr&, rose_addr_t va, bool strict=true);

        /** Reads a string from a file.
         *
         *  Returns the NUL-terminated string stored at the specified relative virtual address. The returned string contains
         *  the bytes beginning at the specified starting file offset and continuing until we reach a NUL byte or an invalid
         *  file offset. If we reach an invalid file offset one of two things happen: if @p strict is set (the default) then an
         *  @ref SgAsmExecutableFileFormat::ShortRead exception is thrown; otherwise the string is simply terminated. The
         *  returned string does not include the NUL byte. */
        std::string read_content_str(rose_addr_t abs_offset, bool strict=true);

        /** Property: Entire file contents. */
        const SgFileContentList& content() { return p_data; }

        /** Returns a vector that points to part of the file.
         *
         *  Returns a vector that points to part of the file content without actually ever reading or otherwise referencing the
         *  file content until the vector elements are referenced. If the desired extent falls entirely or partially outside
         *  the range of data known to the file then throw an @ref SgAsmExecutableFileFormat::ShortRead exception. This
         *  function never updates reference tracking lists for the file. */
        SgFileContentList content(rose_addr_t offset, rose_addr_t size);

        /** Returns list of all sections in the file that are memory mapped, including headers and holes. */
        SgAsmGenericSectionPtrList get_mapped_sections() const;

        /** Returns list of all sections in the file (including headers, holes, etc). */
        SgAsmGenericSectionPtrList get_sections(bool include_holes=true) const;

        /** Returns sections having specified ID across all headers, including headers and holes. */
        SgAsmGenericSectionPtrList get_sections_by_id(int id) const;

        /** Returns all sections having specified name across all headers, including headers and holes. */
        SgAsmGenericSectionPtrList get_sections_by_name(std::string, char sep='\0') const;

        /** Find sections by their offset.
         *
         *  Returns all sections that contain all of the specified portion of the file across all headers, including headers
         *  and holes. */
        SgAsmGenericSectionPtrList get_sections_by_offset(rose_addr_t offset, rose_addr_t size) const;

        /** Find sections by address.
         *
         *  Returns all sections that are mapped to include the specified relative virtual address across all headers,
         *  including headers and holes. This uses the preferred mapping of the section rather than the actual mapping. */
        SgAsmGenericSectionPtrList get_sections_by_rva(rose_addr_t rva) const;

        /** Find sections by address.
         *
         *  Returns all sections that are mapped to include the specified virtual address across all headers, including headers
         *  and holes. This uses the preferred mapping rather than the actual mapping. */
        SgAsmGenericSectionPtrList get_sections_by_va(rose_addr_t va) const;

        /** Find section with specified ID.
         *
         *  Returns the pointer to section with the specified ID across all headers only if there's exactly one match. Headers
         *  and holes are included in the results. */
        SgAsmGenericSection *get_section_by_id(int id, size_t *nfound=0) const;

        /** Find section by name.
         *
         *  Returns pointer to the section with the specified name, or NULL if there isn't exactly one match. Any characters in
         *  the name after the first occurrence of SEP are ignored (default is NUL). For instance, if sep=='$' then the
         *  following names are all equivalent: .idata, .idata$, and .idata$1 */
        SgAsmGenericSection *get_section_by_name(const std::string&, char sep=0, size_t *nfound=0) const;

        /** Find section by file offset.
         *
         *  Returns single section that contains all of the specified portion of the file across all headers, including headers
         *  and holes. */
        SgAsmGenericSection *get_section_by_offset(rose_addr_t offset, rose_addr_t size, size_t *nfound=0) const;

        /** Find section by address.
         *
         *  Returns single section that is mapped to include the specified relative virtual file address across all headers,
         *  including headers and holes. */
        SgAsmGenericSection *get_section_by_rva(rose_addr_t rva, size_t *nfound=0) const;

        /** Find section by address.
         *
         *  Returns single section that is mapped to include the specified virtual address across all headers. See also
         *  @ref get_best_section_by_va. */
        SgAsmGenericSection *get_section_by_va(rose_addr_t va, size_t *nfound=0) const;

        /** Find section by address.
         *
         *  Similar to @ref get_section_by_va except when more than one section contains the specified virtual address this
         *  choose the "best" one. All candidates must map the virtual address to the same file address or else we fail (return
         *  null and number of candidates). See @ref best_section_by_va for definition of "best". */
        SgAsmGenericSection *get_best_section_by_va(rose_addr_t va, size_t *nfound=0) const;

        /** Definition for "best".
         *
         *  This is the definition of "best" as used by @ref get_best_section_by_va and @ref
         *  SgAsmGenericHeader::get_best_section_by_va.  The specified list of sections is scanned and the best one
         *  containing the specified virtual address is returned.  The operation is equivalent to the successive elimination of
         *  bad sections: first eliminate all sections that do not contain the virtual address.  If more than one remains,
         *  eliminate all but the smallest.  If two or more are tied in size and at least one has a name, eliminate those that
         *  don't have names.  If more than one section remains, return the section that is earliest in the specified list of
         *  sections.  Return the null pointer if no section contains the specified virtual address, or if any two sections
         *  that contain the virtual address map it to different parts of the underlying binary file. */
        static SgAsmGenericSection *best_section_by_va(const SgAsmGenericSectionPtrList &sections, rose_addr_t va);

        /** Moves and enlarges a section.
         *
         *  Shifts (to a higher offset) and/or enlarges the specified section, S, taking all other sections into account. The
         *  positions of sections are based on their preferred virtual mappings rather than the actual mapping.
         *
         *  The neighborhood(S) is S itself and the set of all sections that overlap or are adjacent to the neighborhood of S,
         *  recursively.
         *
         *  The address space can be partitioned into three categories:
         *  @li Section: part of an address space that is referenced by an SgAsmGenericSection other than a "hole" section.
         *  @li Hole:    part of an address space that is referenced only by a "hole" section.
         *  @li Unref:   part of an address space that is not used by any section, including any "hole" section.
         *
         *  The last two categories define parts of the address space that can be optionally elastic--they expand or contract
         *  to take up slack or provide space for neighboring sections. This is controlled by the "elasticity" argument.
         *
         *  Note that when elasticity is ELASTIC_HOLE we simply ignore the "hole" sections, effectively removing their
         *  addresses from the range of addresses under consideration. This avoids complications that arise when a "hole"
         *  overlaps with a real section (due to someone changing offsets in an incompatible manner), but causes the hole
         *  offset and size to remain fixed.  (FIXME RPM 2008-10-20)
         *
         *  When section S is shifted by 'Sa' bytes and/or enlarged by 'Sn' bytes, other sections are affected as follows:
         *  @li Cat L:  Not affected
         *  @li Cat R: Shifted by Sa+Sn if they are in neighborhood(S). Otherwise the amount of shifting depends on the size of
         *      the hole right of neighborhood(S).
         *  @li Cat C:  Shifted Sa and enlarged Sn.
         *  @li Cat O:  If starting address are the same: Shifted Sa. If starting address not equal: Englarged Sa+Sn
         *  @li Cat I:  Shifted Sa, not enlarged
         *  @li Cat B:  Not shifted, but enlarged Sn
         *  @li Cat E:  Shifted Sa and enlarged Sn
         *
         *  Generally speaking, the AddressSpace argument should be SgAsmGenericFile::ADDRSP_ALL in order to adjust both file
         *  and memory offsets and sizes in a consistent manner.
         *
         *  To change the address and/or size of S without regard to other sections in the same file, use set_offset() and
         *  set_size() (for file address space) or set_mapped_preferred_rva() and set_mapped_size() (for memory address
         *  space).
         *
         * @{ */
        void shift_extend(SgAsmGenericSection*, rose_addr_t sa, rose_addr_t sn, AddressSpace, Elasticity);
        void shift_extend(SgAsmGenericSection *s, rose_addr_t sa, rose_addr_t sn) {
            shift_extend(s, sa, sn, ADDRSP_ALL, ELASTIC_UNREF);
        }
        /** @} */

        /** File offset of next section.
         *
         *  Given a file address, return the file offset of the following section.  If there is no following section then
         *  return an address of -1 (when signed) */
        rose_addr_t get_next_section_offset(rose_addr_t offset);

        /** Adds a new hole to the file.
         *
         *  This is called implicitly by the hole constructor. */
        void add_hole(SgAsmGenericSection*);

        /** Removes a hole from the list of holes in a file. */
        void remove_hole(SgAsmGenericSection*);

        /** Find holes in file and create sections to fill them.
         *
         *  Synthesizes "hole" sections to describe the parts of the file that are not yet referenced by other sections.  Note
         *  that holes are used to represent parts of the original file data, before sections were modified by walking the AST
         *  (at this time it is not possible to create a hole outside the original file content). */
        void fill_holes();

        /** Deletes "hole" sections.
         *
         *  Undoes what @ref fill_holes did. */
        void unfill_holes();

        /** Adds a new header to the file.
         *
         *  This is called implicitly by the header constructor */
        void add_header(SgAsmGenericHeader*);

        /** Removes a header from the header list in a file. */
        void remove_header(SgAsmGenericHeader*);

        /** Returns the header for the specified format. */
        SgAsmGenericHeader *get_header(SgAsmGenericFormat::ExecFamily);

        /** Return a string describing the file format.
         *
         *  This uses the last header so that files like PE, NE, LE, LX, etc. which also have a DOS header report the format of
         *  the second (PE, etc.) header rather than the DOS header. */
        const char *format_name() const;

        const SgAsmGenericFormat::fileDetails &get_sb() {
            return p_sb;
        }
        void set_sb(const SgAsmGenericFormat::fileDetails&) {
            printf("set_sb() not implemented!\n");
            ROSE_ASSERT(false);
        }

    private:
        void ctor();
#endif // SgAsmGenericFile_OTHERS

#ifdef DOCUMENTATION
    };
#endif


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
#if defined(SgAsmExecutableFileFormat_HEADERS) || defined(DOCUMENTATION)
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
        void serialize(S &s, const unsigned /*version*/) {
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
        class FormatError: public std::runtime_error {
        public:
            FormatError(const std::string &mesg): std::runtime_error(mesg) {}
            FormatError(const char *mesg): std::runtime_error(mesg) {}
            ~FormatError() throw () {}
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
            ISA_ARM_A64                 = 0x090f,       /**< ARM AArch64 A64 instruction set. */

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

        /** Dump debugging information into a named text file.
         *
         *  Writes a new file from the IR node for a parsed executable file. Warning: This function might modify the AST by
         *  calling @ref reallocate, which makes sure all parts of the AST are consistent with respect to each other. */
        static void unparseBinaryFormat(const std::string &name, SgAsmGenericFile*);

        /** Dump debugging information to specified stream.
         *
         *  Unparses an executable file into the supplied output stream. Warning: This function might modify the AST by calling
         *  reallocate(), which makes sure all parts of the AST are consistent with respect to each other. */
        static void unparseBinaryFormat(std::ostream&, SgAsmGenericFile*);

        /** Diagnostic stream. */
        static Sawyer::Message::Facility mlog;

        /** Initialize diagnostic streams.
         *
         *  This is called automatically by @ref Rose::initializeLibrary. */
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
        // Rose::stringifySgAsmExecutableFileFormatInsnSetArchitecture, etc. */
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
        void serialize(S &s, const unsigned /*version*/) {
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

#endif
