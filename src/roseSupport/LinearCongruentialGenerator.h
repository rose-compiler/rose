#ifndef ROSE_LinearCongruentialGenerator_H
#define ROSE_LinearCongruentialGenerator_H

#include <stdint.h>

/** Linear congruential generator.  Generates a repeatable sequence of pseudo-random numbers. */
class LinearCongruentialGenerator {
public:
    /** Initialize the generator with a random seed. */
    LinearCongruentialGenerator(): seed_(rand()) { value_=seed_; }

    /** Initialize the generator with a seed. The seed determines which sequence of numbers is returned. */
    LinearCongruentialGenerator(int seed): seed_(seed), value_(seed) {}

    /** Reset the sequence back to the first value. */
    void reset() { value_=seed_; }

    /** Start a new sequence of random values. The seed identifies which sequence is returned. */
    void reseed(int seed) { value_ = seed_ = seed; }

    /** Return the seed for the current sequence. */
    int seed() const { return seed_; }

    /** Return the last returned value again. */
    uint64_t again() const { return value_; }

    /** Return the maximum possible value. */
    uint64_t max();

    /** Return the next value in the sequence.
     * @{ */
    uint64_t next();
    uint64_t operator()() { return next(); }
    /** @} */

protected:
    int seed_;
    uint64_t value_;
};

typedef LinearCongruentialGenerator LCG;

#endif
