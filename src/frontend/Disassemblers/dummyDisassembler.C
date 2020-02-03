// ROSETTA has no way to exclude certain IR node definitions based on the user's choice of languages.  Therefore, we need to
// provide (stub) definitions for those nodes' virtual methods in order to get things to compile when those nodes aren't
// actually being used anywhere.
#include "sage3basic.h"
#include "Diagnostics.h"
#include "Disassembler.h"

using namespace Rose::BinaryAnalysis;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmExecutableFileFormat

void SgAsmExecutableFileFormat::initDiagnostics() {}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmFloatValueExpression

double SgAsmFloatValueExpression::get_nativeValue() const { abort(); }
void SgAsmFloatValueExpression::set_nativeValue(double) { abort(); }
void SgAsmFloatValueExpression::updateBitVector() { abort(); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmIntegerValueExpression

uint64_t SgAsmIntegerValueExpression::virtualAddress(SgNode*) { abort(); }
std::string SgAsmIntegerValueExpression::get_label(bool) const { abort(); }
size_t SgAsmIntegerValueExpression::get_significantBits() const { abort(); }
void SgAsmIntegerValueExpression::makeRelativeTo(SgNode*) { abort(); }
uint64_t SgAsmIntegerValueExpression::get_baseAddress() const { abort(); }
uint64_t SgAsmIntegerValueExpression::get_absoluteValue(size_t) const { abort(); }
int64_t SgAsmIntegerValueExpression::get_signedValue() const { abort(); }
void SgAsmIntegerValueExpression::set_absoluteValue(uint64_t) { abort(); }
int64_t SgAsmIntegerValueExpression::get_relativeValue() const { abort(); }
void SgAsmIntegerValueExpression::set_relativeValue(int64_t, size_t) { abort(); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmInstruction

const int64_t SgAsmInstruction::INVALID_STACK_DELTA = (uint64_t)1 << 63;
size_t SgAsmInstruction::get_size() const { return 0; }
bool SgAsmInstruction::terminatesBasicBlock() { return false; }
bool SgAsmInstruction::isUnknown() const { return false; }
bool SgAsmInstruction::hasEffect() { return false; }
bool SgAsmInstruction::hasEffect(const std::vector<SgAsmInstruction*>&, bool, bool) { return false; }
bool SgAsmInstruction::isFunctionCallFast(const std::vector<SgAsmInstruction*>&, rose_addr_t*, rose_addr_t*) { return false; }
bool SgAsmInstruction::isFunctionCallSlow(const std::vector<SgAsmInstruction*>&, rose_addr_t*, rose_addr_t*) { return false; }
bool SgAsmInstruction::isFunctionReturnFast(const std::vector<SgAsmInstruction*>&) { return false; }
bool SgAsmInstruction::isFunctionReturnSlow(const std::vector<SgAsmInstruction*>&) { return false; }
bool SgAsmInstruction::getBranchTarget(rose_addr_t*) { return false; }
std::set<rose_addr_t> SgAsmInstruction::getSuccessors(bool* complete) { return std::set<rose_addr_t>();}
unsigned SgAsmInstruction::get_anyKind() const { return 0; }
std::string SgAsmInstruction::toString() const { return ""; }

std::set<rose_addr_t>
SgAsmInstruction::getSuccessors(const std::vector<SgAsmInstruction*>&, bool*, const MemoryMap::Ptr&) {
    return std::set<rose_addr_t>();
}

std::vector<std::pair<size_t,size_t> >
SgAsmInstruction::findNoopSubsequences(const std::vector<SgAsmInstruction*>&, bool, bool) {
    return std::vector<std::pair<size_t,size_t> >();
}

std::set<rose_addr_t>
SgAsmInstruction::explicitConstants() const {
    return std::set<rose_addr_t>();
}

size_t SgAsmInstruction::semanticFailure() const { abort(); }
void SgAsmInstruction::semanticFailure(size_t) { abort(); }
void SgAsmInstruction::incrementSemanticFailure() { abort(); }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmArmInstruction

bool SgAsmArmInstruction::terminatesBasicBlock() { return false; }
bool SgAsmArmInstruction::isUnknown() const { return false; }
std::set<rose_addr_t> SgAsmArmInstruction::getSuccessors(bool* complete) { return std::set<rose_addr_t>(); }
unsigned SgAsmArmInstruction::get_anyKind() const { return 0; }
std::string SgAsmArmInstruction::description() const { return ""; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmM68kInstruction

bool SgAsmM68kInstruction::terminatesBasicBlock() { return false; }
bool SgAsmM68kInstruction::isFunctionCallFast(const std::vector<SgAsmInstruction*>&, rose_addr_t*, rose_addr_t*) { return false; }
bool SgAsmM68kInstruction::isFunctionCallSlow(const std::vector<SgAsmInstruction*>&, rose_addr_t*, rose_addr_t*) { return false; }
bool SgAsmM68kInstruction::isFunctionReturnFast(const std::vector<SgAsmInstruction*>&) { return false; }
bool SgAsmM68kInstruction::isFunctionReturnSlow(const std::vector<SgAsmInstruction*>&) { return false; }
bool SgAsmM68kInstruction::isUnknown() const { return false; }
bool SgAsmM68kInstruction::getBranchTarget(rose_addr_t*) { return false; }
std::set<rose_addr_t> SgAsmM68kInstruction::getSuccessors(bool* complete) { return std::set<rose_addr_t>();}
unsigned SgAsmM68kInstruction::get_anyKind() const { return 0; }
std::string SgAsmM68kInstruction::description() const { return ""; }

std::set<rose_addr_t>
SgAsmM68kInstruction::getSuccessors(const std::vector<SgAsmInstruction*>&, bool*, const MemoryMap::Ptr&) {
    return std::set<rose_addr_t>();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmMipsInstruction

bool SgAsmMipsInstruction::terminatesBasicBlock() { return false; }
bool SgAsmMipsInstruction::isUnknown() const { return false; }
bool SgAsmMipsInstruction::isFunctionCallFast(const std::vector<SgAsmInstruction*>&, rose_addr_t*, rose_addr_t*) { return false; }
bool SgAsmMipsInstruction::isFunctionCallSlow(const std::vector<SgAsmInstruction*>&, rose_addr_t*, rose_addr_t*) { return false; }
bool SgAsmMipsInstruction::isFunctionReturnFast(const std::vector<SgAsmInstruction*>&) { return false; }
bool SgAsmMipsInstruction::isFunctionReturnSlow(const std::vector<SgAsmInstruction*>&) { return false; }
bool SgAsmMipsInstruction::getBranchTarget(rose_addr_t*) { return false; }
std::set<rose_addr_t> SgAsmMipsInstruction::getSuccessors(bool*) { return std::set<rose_addr_t>();}
unsigned SgAsmMipsInstruction::get_anyKind() const { return 0; }
std::string SgAsmMipsInstruction::description() const { return ""; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmPowerpcInstruction

bool SgAsmPowerpcInstruction::terminatesBasicBlock() { return false; }
bool SgAsmPowerpcInstruction::isUnknown() const { return false; } 
std::set<rose_addr_t> SgAsmPowerpcInstruction::getSuccessors(bool*) { return std::set<rose_addr_t>();}
unsigned SgAsmPowerpcInstruction::get_anyKind() const { return 0; }
std::string SgAsmPowerpcInstruction::description() const { return 0; }
bool SgAsmPowerpcInstruction::isFunctionCallFast(const std::vector<SgAsmInstruction*>&, rose_addr_t*, rose_addr_t*) { return false; }
bool SgAsmPowerpcInstruction::isFunctionCallSlow(const std::vector<SgAsmInstruction*>&, rose_addr_t*, rose_addr_t*) { return false; }
bool SgAsmPowerpcInstruction::isFunctionReturnFast(const std::vector<SgAsmInstruction*>&) { return false; }
bool SgAsmPowerpcInstruction::isFunctionReturnSlow(const std::vector<SgAsmInstruction*>&) { return false; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmX86Instruction

bool SgAsmX86Instruction::terminatesBasicBlock() { return false; }
bool SgAsmX86Instruction::isUnknown() const { return false; }
bool SgAsmX86Instruction::isFunctionCallFast(const std::vector<SgAsmInstruction*>&, rose_addr_t*, rose_addr_t*) { return false; }
bool SgAsmX86Instruction::isFunctionCallSlow(const std::vector<SgAsmInstruction*>&, rose_addr_t*, rose_addr_t*) { return false; }
bool SgAsmX86Instruction::isFunctionReturnFast(const std::vector<SgAsmInstruction*>&) { return false; }
bool SgAsmX86Instruction::isFunctionReturnSlow(const std::vector<SgAsmInstruction*>&) { return false; }
bool SgAsmX86Instruction::getBranchTarget(rose_addr_t*) { return false; }
unsigned SgAsmX86Instruction::get_anyKind() const { return 0; }
std::set<rose_addr_t> SgAsmX86Instruction::getSuccessors(bool* complete) { return std::set<rose_addr_t>();}

std::set<rose_addr_t>
SgAsmX86Instruction::getSuccessors(const std::vector<SgAsmInstruction*>&, bool*, const MemoryMap::Ptr&) {
    return std::set<rose_addr_t>();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmType

void SgAsmType::check() const {}
size_t SgAsmType::get_nBytes() const { return 0; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmScalarType

void SgAsmScalarType::check() const {}
size_t SgAsmScalarType::get_nBits() const { return 0; }
std::string SgAsmScalarType::toString() const { return std::string(); }
size_t SgAsmScalarType::get_majorNBytes() const { return 0; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmIntegerType

void SgAsmIntegerType::check() const {}
std::string SgAsmIntegerType::toString() const { return std::string(); }
bool SgAsmIntegerType::get_isSigned() const { return false; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmFloatType

void SgAsmFloatType::check() const {}
std::string SgAsmFloatType::toString() const { return std::string(); }
SgAsmFloatType::BitRange SgAsmFloatType::significandBits() const { return BitRange(); }
SgAsmFloatType::BitRange SgAsmFloatType::exponentBits() const { return BitRange(); }
size_t SgAsmFloatType::signBit() const { return 0; }
uint64_t SgAsmFloatType::exponentBias() const { return 0; }
SgAsmFloatType::Flags SgAsmFloatType::flags() const { return 0; }
bool SgAsmFloatType::gradualUnderflow() const { return false; }
bool SgAsmFloatType::implicitBitConvention() const { return false; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmVectorType

void SgAsmVectorType::check() const {}
std::string SgAsmVectorType::toString() const { return std::string(); }
size_t SgAsmVectorType::get_nBits() const { return 0; }
size_t SgAsmVectorType::get_nElmts() const { return 0; }
SgAsmType* SgAsmVectorType::get_elmtType() const { return 0; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Disassembler
namespace Rose {
namespace BinaryAnalysis{
void Disassembler::initDiagnostics() {}
} // namespace
} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SgAsmBlock

bool
SgAsmBlock::has_instructions() const
{
    const SgAsmStatementPtrList &stmts = get_statementList();
    for (SgAsmStatementPtrList::const_iterator si=stmts.begin(); si!=stmts.end(); ++si) {
        if (isSgAsmInstruction(*si))
            return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RegisterDescriptor
namespace Rose {
namespace BinaryAnalysis {
void RegisterDescriptor::majorNumber(unsigned) { abort(); }
void RegisterDescriptor::minorNumber(unsigned) { abort(); }
void RegisterDescriptor::offset(size_t) { abort(); }
void RegisterDescriptor::nBits(size_t) { abort(); }
void RegisterDescriptor::setOffsetWidth(size_t, size_t) { abort(); }
std::ostream& operator<<(std::ostream&, RegisterDescriptor) { abort(); }
} // namespace
} // namespace
