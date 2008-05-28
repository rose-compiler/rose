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

// Author: Gergo Barany
// $Id: StackFrameVector.C,v 1.1 2008/01/08 02:56:39 dquinlan Exp $

template <class T>
StackFrameVector<T>::StackFrameVector()
  : buffer(new BufferType()),
    framePtr(buffer->begin()),
    stackPtr(buffer->end()),
    deleteBufferWhenDone(true)
{
}

template <class T>
StackFrameVector<T>::StackFrameVector(const StackFrameVector<T> &v)
  : buffer(NULL),
    framePtr(v.framePtr),
    stackPtr(v.stackPtr),
    deleteBufferWhenDone(false) // don't delete the pointer, it's not ours
{
}

template <class T>
StackFrameVector<T>::StackFrameVector(
        typename StackFrameVector<T>::size_type n)
  : buffer(new BufferType(n)),
    framePtr(buffer->begin()),
    stackPtr(buffer->end()),
    deleteBufferWhenDone(true)
{
}

template <class T>
StackFrameVector<T>::StackFrameVector(
        typename StackFrameVector<T>::size_type n,
        typename StackFrameVector<T>::value_type initValue)
  : buffer(new BufferType(n, initValue)),
    framePtr(buffer->begin()),
    stackPtr(buffer->end()),
    deleteBufferWhenDone(true)
{
}

template <class T>
StackFrameVector<T>::StackFrameVector(
        const typename StackFrameVector<T>::BufferType &otherBuffer,
        typename StackFrameVector<T>::difference_type framePtrOffset)
  : buffer(new BufferType(otherBuffer)),
    framePtr(buffer->begin() + framePtrOffset),
    stackPtr(buffer->end()),
    deleteBufferWhenDone(true)
{
}

template <class T>
StackFrameVector<T>::~StackFrameVector()
{
    ROSE_ASSERT(deleteBufferWhenDone == (buffer != NULL));
    if (deleteBufferWhenDone)
    {
        delete buffer;
        buffer = NULL;
    }
}

template <class T>
StackFrameVector<T> *
StackFrameVector<T>::deepCopy() const
{
    // return a pointer to a new instance having a deep copy of all the live
    // elements on the stack (those up to stackPtr). The second argument is
    // the offset of the framePtr that needs to be set correctly in the copy
    // (the stackPtr can just be set to the end of the newly copied buffer).
    return new StackFrameVector(BufferType(buffer->begin(), stackPtr), framePtr - buffer->begin());
}

template <class T>
typename StackFrameVector<T>::iterator
StackFrameVector<T>::begin()
{
    return framePtr;
}

template <class T>
typename StackFrameVector<T>::const_iterator
StackFrameVector<T>::begin() const
{
    return framePtr;
}

template <class T>
typename StackFrameVector<T>::iterator
StackFrameVector<T>::end()
{
    return stackPtr;
}

template <class T>
typename StackFrameVector<T>::const_iterator
StackFrameVector<T>::end() const
{
    return stackPtr;
}

template <class T>
typename StackFrameVector<T>::reverse_iterator
StackFrameVector<T>::rbegin()
{
    return reverse_iterator(end());
}

template <class T>
typename StackFrameVector<T>::const_reverse_iterator
StackFrameVector<T>::rbegin() const
{
    return reverse_iterator(end());
}

template <class T>
typename StackFrameVector<T>::reverse_iterator
StackFrameVector<T>::rend()
{
    return reverse_iterator(begin());
}

template <class T>
typename StackFrameVector<T>::const_reverse_iterator
StackFrameVector<T>::rend() const
{
    return reverse_iterator(begin());
}

template <class T>
typename StackFrameVector<T>::size_type
StackFrameVector<T>::size() const
{
    return end() - begin();
}

template <class T>
typename StackFrameVector<T>::size_type
StackFrameVector<T>::max_size() const
{
    return size();
}

template <class T>
typename StackFrameVector<T>::size_type
StackFrameVector<T>::capacity() const
{
    return size();
}

template <class T>
bool
StackFrameVector<T>::empty() const
{
    return size() == 0;
}

template <class T>
typename StackFrameVector<T>::reference
StackFrameVector<T>::operator[](typename StackFrameVector<T>::size_type n)
{
    return framePtr[n];
}

template <class T>
typename StackFrameVector<T>::const_reference
StackFrameVector<T>::operator[](typename StackFrameVector<T>::size_type n) const
{
    return framePtr[n];
}

#include <stdexcept>

template <class T>
typename StackFrameVector<T>::reference
StackFrameVector<T>::at(typename StackFrameVector<T>::size_type n)
{
    if (n >= size())
        throw std::out_of_range("StackFrameVector: index out of range");
    return (*this)[n];
}

template <class T>
typename StackFrameVector<T>::const_reference
StackFrameVector<T>::at(typename StackFrameVector<T>::size_type n) const
{
    if (n >= size())
        throw std::out_of_range("StackFrameVector: index out of range");
    return (*this)[n];
}

template <class T>
typename StackFrameVector<T>::reference
StackFrameVector<T>::front()
{
    return *framePtr;
}

template <class T>
typename StackFrameVector<T>::const_reference
StackFrameVector<T>::front() const
{
    return *framePtr;
}

template <class T>
typename StackFrameVector<T>::reference
StackFrameVector<T>::back()
{
    return *(stackPtr - 1);
}

template <class T>
typename StackFrameVector<T>::const_reference
StackFrameVector<T>::back() const
{
    return *(stackPtr - 1);
}

template <class T>
StackFrameVector<T>::operator std::vector<T>()
{
    // return a copy of the objects in the current stack frame
    return BufferType(begin(), end());
}

template <class T>
void
StackFrameVector<T>::push(const T &x)
{
    ROSE_ASSERT(buffer != NULL);

    if (framePtr == buffer->end())
    {
        buffer->push_back(x);
        // push_back may have caused a resize, invalidating iterators;
        // compute the new iterator values
        framePtr = stackPtr = buffer->end();
        // note that no user should have iterators into our buffer lying
        // around (push is only called by the traversal code, no user function
        // is alive at that point), so hopefully we haven't invalidated
        // anything else
    }
    else
    {
        *framePtr++ = x;
        stackPtr = framePtr;
    }
}

template <class T>
void
StackFrameVector<T>::setFrameSize(difference_type frameSize)
{
    // set the frame size to the desired value by adjusting the frame
    // pointer; the next push() (which writes through the frame pointer)
    // will in effect pop off this whole frame
    framePtr = stackPtr - frameSize;
}

template <class T>
typename StackFrameVector<T>::size_type
StackFrameVector<T>::debugSize() const
{
    ROSE_ASSERT(buffer != NULL);
    return stackPtr - buffer->begin();
}

template <class T>
void
StackFrameVector<T>::resetStack()
{
    ROSE_ASSERT(buffer != NULL);
    framePtr = stackPtr = buffer->begin();
}

template <class T>
typename StackFrameVector<T>::value_type
StackFrameVector<T>::pop()
{
    // pop off a single element, this is intended to be used if the whole
    // stack contains just this element (the final result of the computation);
    // if your stack contains more than one element at this point, your
    // pointers will be messed up
    --framePtr;
    return *--stackPtr;
}

template <class T>
void
StackFrameVector<T>::debugDump(std::ostream &s)
{
    // this function should only be called for debugging
    s << "\n"
        << "framePtr offset: " << framePtr - buffer->begin()
        << " stackPtr offset: " << stackPtr - buffer->begin()
        << " size: " << size()
        << " buffer size: " << buffer->size()
        << "\n";
    s << "buffer:" << "\n";
    const_iterator i;
    for (i = buffer->begin(); i != buffer->end(); ++i)
        s << (void *) *i << ' ';
    s << "\n" << "\n";
}

#endif
