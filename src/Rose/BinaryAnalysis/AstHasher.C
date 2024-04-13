#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/AstHasher.h>

#include <Rose/Affirm.h>
#include <Rose/AST/Traversal.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>

#include <SgAsmConstantExpression.h>
#include <SgAsmInstruction.h>
#include <SgAsmRegisterReferenceExpression.h>

#include <Cxx_GrammarDowncast.h>

namespace Rose {
namespace BinaryAnalysis {

AstHasher::~AstHasher() {}

AstHasher::AstHasher(const std::shared_ptr<Combinatorics::Hasher> &hasher)
    : hasher_(hasher) {
    ASSERT_not_null(hasher);
}

bool
AstHasher::hashingConstants() const {
    return hashingConstants_;
}

void
AstHasher::hashingConstants(bool b) {
    hashingConstants_ = b;
}

bool
AstHasher::hashingAddresses() const {
    return hashingAddresses_;
}

void
AstHasher::hashingAddresses(bool b) {
    hashingAddresses_ = b;
}

std::shared_ptr<Combinatorics::Hasher>
AstHasher::hasher() const {
    return notnull(hasher_);
}

void
AstHasher::hash(SgNode *ast) {
    if (ast) {
        ASSERT_not_null(hasher_);
        AST::Traversal::forwardPre<SgNode>(ast, [this](SgNode *node) {
            // Always include the type of each node in the hash. We include the type name rather than the enum constant because we
            // want the hash to be as stable as possible across different ROSE versions. The type name never changes, but the enum
            // constant can change whenever someone adds a new SgNode type even if that type is completely unrelated to binary
            // analysis.
            hasher_->insert(node->class_name());
        
            if (auto asmInstruction = isSgAsmInstruction(node)) {
                // Include the mnemonic, and maybe the address.
                hasher_->insert(asmInstruction->get_mnemonic());
                if (hashingAddresses_)
                    hasher_->insert(asmInstruction->get_address());

            } else if (auto regRef = isSgAsmRegisterReferenceExpression(node)) {
                // Always include register references
                hasher_->insert(regRef->get_descriptor().hash());

            } else if (auto constExpr = isSgAsmConstantExpression(node)) {
                // Maybe include constants (integers, floats, pointers)
                if (hashingConstants_)
                    hasher_->insert(constExpr->get_bitVector().toHex());
            }
        });
    }
}

void
AstHasher::hash(const Partitioner2::BasicBlockPtr &bb) {
    if (bb) {
        for (SgAsmInstruction *insn: bb->instructions())
            hash(insn);
    }
}

} // namespace
} // namespace

#endif
