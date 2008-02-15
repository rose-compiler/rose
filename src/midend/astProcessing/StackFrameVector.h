// Author: Gergo Barany
// $Id: StackFrameVector.h,v 1.1 2008/01/08 02:56:39 dquinlan Exp $

/*
   StackFrameVector is a template container class meant to be used for the
   stack of SythesizedAttributes in the SgTreeTraversal class; to the concrete
   traversal classes (Ast*Processing) it is hidden behind the
   SynthesizedAttributesList typedef, which used to be a std::vector.
   Therefore StackFrameVector implements much of the interface of std::vector
   in the hope that this will cause minimal breakage of existing user code.

   As it was never a useful operation to alter the size of the
   SynthesizedAttributesList, or to change its contents, and this would be
   impossible to support efficiently with StackFrameVector, member functions
   such as push_back(), erase(), resize() etc. are not implemented. Code using
   these, for whatever strange reason, should make a copy of the container (a
   conversion to std::vector is provided) to continue working.

   At the moment we don't provide comparison operators either.

   StackFrameVector is basically implemented as a pointer to a vector, which
   contains the stack elements, and a pair of iterators into this vector
   denoting the start and end of a stack frame. Some special stack operations
   are provided: push(const T &) to push an element to the top of the stack,
   and setFrameSize(BufferType::size_type) to define how many elements will
   comprise the stack's top frame (i.e., how many elements there will be
   between begin() and end()). A push() always pops off the current frame
   before the new element is added.

   The function debugSize() tells you the total size of the stack, and pop()
   pops and returns the topmost element, which is mainly useful if the stack
   contains exactly that one element. All these special functions should only
   be used by code that knows what it's doing.
*/

#ifndef STACKFRAMEVECTOR_H
#define STACKFRAMEVECTOR_H

#include <vector>
#include <ostream>

template <class T>
class StackFrameVector
{
public:
    // typedef to the underlying container type; this is provided for
    // generality, but it will probably not make much sense to change it
    typedef std::vector<T> BufferType;

    // typedefs required for std::vector, just use those defined for the
    // buffer type
    typedef typename BufferType::reference              reference;
    typedef typename BufferType::const_reference        const_reference;
    typedef typename BufferType::iterator               iterator;
    typedef typename BufferType::const_iterator         const_iterator;
    typedef typename BufferType::size_type              size_type;
    typedef typename BufferType::difference_type        difference_type;
    typedef typename BufferType::value_type             value_type;
    typedef typename BufferType::allocator_type         allocator_type;
    typedef typename BufferType::pointer                pointer;
    typedef typename BufferType::const_pointer          const_pointer;
    typedef typename BufferType::reverse_iterator       reverse_iterator;
    typedef typename BufferType::const_reverse_iterator const_reverse_iterator;

    // constructors required for std::vector are not all implemented
    // because user code is not supposed to construct a StackFrameVector
    StackFrameVector();
    StackFrameVector(const StackFrameVector &);
    StackFrameVector(size_type n);
    StackFrameVector(size_type n, value_type initValue);

    ~StackFrameVector();

    // deep copy operation, returns a new instance with a copy of this one's
    // stack up to the stackPtr (the iterators are set correctly in the copy)
    StackFrameVector *deepCopy() const;

    // iterator functions required for std::vector
    iterator               begin();
    const_iterator         begin() const;
    iterator               end();
    const_iterator         end() const;
    reverse_iterator       rbegin();
    const_reverse_iterator rbegin() const;
    reverse_iterator       rend();
    const_reverse_iterator rend() const;

    // various size-related operations required for std::vector; these do
    // not return the size of the underlying buffer (stack), but only the
    // size of the current stack frame
    // size(), max_size(), capacity() return the same value, no resizing
    // functions are provided.
    size_type size() const;
    size_type max_size() const;
    size_type capacity() const;
    bool empty() const;
    
    // element access functions required for std::vector
    reference       operator[](size_type);
    const_reference operator[](size_type) const;
    reference       at(size_type);
    const_reference at(size_type) const;
    reference       front();
    const_reference front() const;
    reference       back();
    const_reference back() const;

    // type conversion to underlying type (this results in a copy, so it
    // will cost you)
    operator std::vector<T>();

    // modifiers; these are deliberately different from what std::vector
    // provides! use at your own risk, and only if you know what you're doing
    void push(const T &);
    void setFrameSize(difference_type);

    // function used for debugging, returns the overall number of objects
    // on the stack, i.e. not just in the current frame
    size_type debugSize() const;

    // Clear the stack; deliberately not named clear. This should only be
    // called by the traversal code at the beginning of a traversal, and in
    // fact it is only needed to support tutorial/traversalShortCircuit.C
    // which calls a traversal, exits from it by throwing an exception,
    // leaving junk on the stack, then uses that traversal object again.
    void resetStack();

    // Pops the top element off the stack, returns it, and adjusts the
    // stack pointer accordingly. Note that this makes sense primarily if
    // the stack (not just the current frame!) contains exactly one element.
    value_type pop();

    // do some debug output; this should not be called
    void debugDump(std::ostream &s);

protected:
    // the buffer to hold all stack elements
    BufferType *buffer;
    // the top stack frame is denoted by these iterators, framePtr plays the
    // role of begin, stackPtr the role of end
    iterator framePtr, stackPtr;
    // flag to indicate whether the destructor should delete the buffer; this
    // must be false for shallow copies (shallow copying is the default
    // behavior)
    bool deleteBufferWhenDone;

private:
    explicit StackFrameVector(const BufferType &otherBuffer, difference_type framePtrOffset);
};

#include "StackFrameVector.C"

#endif
