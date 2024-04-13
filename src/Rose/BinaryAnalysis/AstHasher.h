#ifndef ROSE_BinaryAnalysis_AstHasher_H
#define ROSE_BinaryAnalysis_AstHasher_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>
#include <Combinatorics.h>                              // rose

class SgNode;

namespace Rose {
namespace BinaryAnalysis {

/** Compute the hash for an AST.
 *
 *  The user supplies a low-level hasher when this object is created, then hashes an AST, basic block, or any other supported
 *  object, then obtains the hash from the low-level hasher. The specifics of how various data structures are recursively hashed
 *  is controlled by properties of this object. */
class AstHasher {
    bool hashingConstants_ = false;
    bool hashingAddresses_ = false;
    std::shared_ptr<Combinatorics::Hasher> hasher_;

public:
    ~AstHasher();
    AstHasher() = delete;
    AstHasher& operator=(const AstHasher&) = delete;

    /** Create a new AST hasher using the specified low-level hasher. */
    explicit AstHasher(const std::shared_ptr<Combinatorics::Hasher>&);

    /** Property: Whether to hash constants.
     *
     * @{ */
    bool hashingConstants() const;
    void hashingConstants(bool);
    /** @} */

    /** Property: Whether to hash addresses.
     *
     *  @{ */
    bool hashingAddresses() const;
    void hashingAddresses(bool);
    /** @} */

    /** Property: The associated low-level hasher. */
    std::shared_ptr<Combinatorics::Hasher> hasher() const;

    /** Hash an AST.
     *
     *  Add the specified AST (recursively) to the associated low-level hasher. */
    void hash(SgNode*);

    /** Hash a basic block.
     *
     *  Add the specified basic block (recursively) to the associated low-level hasher. */
    void hash(const Partitioner2::BasicBlockPtr &bb);
};

} // namespace
} // namespace

#endif
#endif
