#ifndef ROSE_BinaryAnalysis_Variables_H
#define ROSE_BinaryAnalysis_Variables_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/Variables/BaseVariable.h>
#include <Rose/BinaryAnalysis/Variables/BasicTypes.h>
#include <Rose/BinaryAnalysis/Variables/GlobalVariable.h>
#include <Rose/BinaryAnalysis/Variables/InstructionAccess.h>
#include <Rose/BinaryAnalysis/Variables/StackVariable.h>
#include <Rose/BinaryAnalysis/Variables/VariableFinder.h>

namespace Rose {
namespace BinaryAnalysis {

/** Finding and describing source-level variables.
 *
 *  This namespace contains facilities for finding source-level variables in binaries and describing those variables. For instance,
 *  it can analyze a function and detect stack locations that are likely to represent variables that appear in the source language
 *  from which the binary was compiled.
 *
 *  Binaries generally don't represent source-level variables in a straightforward manner. Some things to watch out for are:
 *
 *  @item The compiler can elide a source-level variable if it determines that it's not necessary in the binary. I.e., the compiler
 *  can completely optimize away a variable that is written to but never read. In GDB one often sees the string "<optimized out>"
 *  in this kind of situation.
 *
 *  @item The compiler can give the same address to two or more source-level variables if one or more of them is read without being
 *  initialized. This is undefined behavior (UB) in C and C++ and the compiler is free to do anything it wants in these situations.
 *
 *  @item The compiler might not allocate stack storage for a local variable. E.g., if enough general purpose registers are
 *  available the compiler might use a register to store the local variable.
 *
 *  @item The scope of the local variable in the source language does not generally match the lifetime of the storage location
 *  for that variable in the binary. For instance, the compiler might decide that the same storage location can be used for two
 *  or more variables with non-overlapping lifetimes. The binary analysis sees this as a single variable.
 *
 *  @item Variables that the compiler might initially store in registers (such as function arguments) are often spilled to the
 *  stack in order to make those registers available for other purposes within the function.
 *
 *  @item A source-level variable might exist at more than one location in the binary. A common situation is when the compiler
 *  decides to move the variable from a stack location into a register for part of the function and then optionally moves it back
 *  to the stack.
 *
 *  @item Writes to a stack location that are not followed by reads can be elided, in which case the analysis would indicate that
 *  there was no subsequent write.
 *
 *  @item Many other caveats. */
namespace Variables {}

} // namespace
} // namespace


#endif
#endif
