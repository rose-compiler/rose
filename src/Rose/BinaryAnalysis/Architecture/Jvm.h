#ifndef ROSE_BinaryAnalysis_Architecture_Jvm_H
#define ROSE_BinaryAnalysis_Architecture_Jvm_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Architecture/Base.h>

namespace Rose {
namespace BinaryAnalysis {
namespace Architecture {

/** Architecture-specific information for the Java virtual machine.
 *
 *  A Java virtual machine (JVM) is a virtual machine that enables a computer to run Java programs as well as programs written in
 *  other languages that are also compiled to Java bytecode. The JVM is detailed by a specification that formally describes what is
 *  required in a JVM implementation. Having a specification ensures interoperability of Java programs across different
 *  implementations so that program authors using the Java Development Kit (JDK) need not worry about idiosyncrasies of the
 *  underlying hardware platform.
 *
 *  The JVM reference implementation is developed by the OpenJDK project as open source code and includes a JIT compiler called
 *  HotSpot. The commercially supported Java releases available from Oracle are based on the OpenJDK runtime. Eclipse OpenJ9 is
 *  another open source JVM for OpenJDK. */
class Jvm: public Base {
public:
    using Ptr = JvmPtr;

protected:
    Jvm();                                              // use `instance` instead
public:
    ~Jvm();

public:
    /** Allocating constructor. */
    static Ptr instance();

public:
    RegisterDictionary::Ptr registerDictionary() const override;
    bool matchesHeader(SgAsmGenericHeader*) const override;

    Sawyer::Container::Interval<size_t> bytesPerInstruction() const override;
    Alignment instructionAlignment() const override;
    std::string instructionDescription(const SgAsmInstruction*) const override;
    bool isUnknown(const SgAsmInstruction*) const override;
    bool terminatesBasicBlock(SgAsmInstruction*) const override;
    bool isFunctionCallFast(const std::vector<SgAsmInstruction*>&, rose_addr_t *target, rose_addr_t *ret) const override;
    bool isFunctionReturnFast(const std::vector<SgAsmInstruction*>&) const override;
    Sawyer::Optional<rose_addr_t> branchTarget(SgAsmInstruction*) const override;
    AddressSet getSuccessors(SgAsmInstruction*, bool &complete) const override;

    Disassembler::BasePtr newInstructionDecoder() const override;
    Unparser::BasePtr newUnparser() const override;
};

} // namespace
} // namespace
} // namespace

#endif
#endif
