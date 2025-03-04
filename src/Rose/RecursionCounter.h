#ifndef ROSE_RecursionCounter_H
#define ROSE_RecursionCounter_H

namespace Rose {

/** Track recursion depth in a function.
 *
 * Use it like this:
 *
 * @code
 *  boost::tribool someAnalysis() {
 *      static size_t depth = 0;
 *      RecursionCounter recursion(depth);
 *      if (depth > 5) // some arbitrary limit
 *          return boost::tribool::indeterminate;
 *      ...
 *  }
 * @endcode
 *
 * You might ask, "Why use this class instead of just saying `++depth` and `--depth`?". If you use this
 * class you don't have to remember to decrement the depth, especially if you (or someone else) adds additional
 * `return` statements later, or if anything can throw and exception. */
struct RecursionCounter {
    size_t &depth;

    RecursionCounter(size_t &depth): depth(depth) {
        ++depth;
    }

    ~RecursionCounter() {
        --depth;
    }
};

} // namespace

#endif
