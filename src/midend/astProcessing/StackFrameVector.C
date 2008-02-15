// Author: Gergo Barany
// $Id: StackFrameVector.C,v 1.1 2008/01/08 02:56:39 dquinlan Exp $

#ifndef STACKFRAMEVECTOR_C
#define STACKFRAMEVECTOR_C

#include "StackFrameVector.h"

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
