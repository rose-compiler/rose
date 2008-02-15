/*
** This software is copyright (C) by the Lawrence Berkeley National
** Laboratory.  Permission is granted to reproduce this software for
** non-commercial purposes provided that this notice is left intact.
**  
** It is acknowledged that the U.S. Government has rights to this
** software under Contract DE-AC03-765F00098 between the U.S.  Department
** of Energy and the University of California.
**  
** This software is provided as a professional and academic contribution
** for joint exchange. Thus it is experimental, is provided ``as is'',
** with no warranties of any kind whatsoever, no support, no promise of
** updates, or printed documentation. By using this software, you
** acknowledge that the Lawrence Berkeley National Laboratory and Regents
** of the University of California shall have no liability with respect
** to the infringement of other copyrights by any part of this software.
**  
** For further information about this software, contact:
** 
**         Dr. John Bell
**         Bldg. 50D, Rm. 129,
**         Lawrence Berkeley National Laboratory
**         Berkeley, CA, 94720
**         jbbell@lbl.gov
*/

//
// $Id: List.C,v 1.1 2004/07/07 10:27:07 dquinlan Exp $
//

#include <Assert.H>

//
// List members
//

template <class T>
List<T>::List (const List<T>& source)
    : head(0),
      tail(0)
{
    if (source.isEmpty())
        tail = head = 0;
    else
        for (ListIterator<T> li(source); li; ++li)
            append(li());
}

//
// This isn't inlined as it's declared virtual.
//

template <class T>
void
List<T>::add (const T& value)
{
    append(value);
}

template <class T>
int
List<T>::length () const
{
    int len = 0;
    for (ListIterator<T> li(*this); li; ++li)
        len++;
    return len;
}

template <class T>
List<T>&
List<T>::operator= (const List<T>& source)
{
    if (!(this == &source))
    {
        clear();
        for (ListIterator<T> li(source); li; ++li)
            append(li());
    }
    return *this;
}

template <class T>
ListLink<T> *
List<T>::addBefore (ListLink<T>* ln,
                    const T&     val)
{
    boxAssert(ln != 0 || head == 0);

    ListLink<T>* newlink;

    if (ln == head)
    {
        head = newlink = new ListLink<T>(val, 0, head);

        if (tail == 0)
            tail = head;
        else
            head->suc->pre = newlink;
    }
    else
    {
        newlink = new ListLink<T>(val, ln->pre, ln);

        ln->pre->suc = newlink;
        ln->pre = newlink;
    }

    if (newlink == 0)
        BoxLib::OutOfMemory(__FILE__, __LINE__);

    return newlink;
}

template <class T>
ListLink<T>*
List<T>::addAfter (ListLink<T>* ln,
                   const T&     val)
{
    boxAssert(ln != 0 || tail == 0);

    ListLink<T>* newlink;

    if (ln == tail)
    {
        tail = newlink = new ListLink<T>(val,tail,0);

        if (head == 0)
            head = tail;
        else
            tail->pre->suc = newlink;
    }
    else
    {
        newlink = new ListLink<T>(val, ln, ln->suc);

        ln->suc->pre = newlink;
        ln->suc = newlink;
    }

    if (newlink == 0)
        BoxLib::OutOfMemory(__FILE__, __LINE__);

    return newlink;
}

template <class T>
void
List<T>::join (const List<T>& list2)
{
    for (ListIterator<T> li2(list2); li2; ++li2)
        append(li2());
}

template <class T>
void
List<T>::catenate (List<T>& list2)
{
    if (list2.isEmpty())
        //
        // Do nothing.
        //
        ;
    else if (isEmpty())
    {
       head = list2.head;
       tail = list2.tail;
       list2.head = 0;
       list2.tail = 0;
    }
    else
    {
        tail->suc = list2.head;
        list2.head->pre = tail;
        tail = list2.tail;
        list2.head = 0;
        list2.tail = 0;
    }
}

template <class T>
void
List<T>::clear ()
{
    ListLink<T>* next = 0;

    for (ListLink<T>* p = head; p != 0; p = next)
    {
        next = p->suc;
        p->suc = 0;
        delete p;
    }
    tail = head = 0;
}

template <class T>
bool
List<T>::includes (const T& v) const
{
    bool rc = false;
    for (ListIterator<T> li(*this); li && !rc; ++li)
        if (v == li())
            rc = true;
    return rc;
}

template<class T>
bool
List<T>::operator== (const List<T>& rhs) const
{
    if (length() == rhs.length())
    {
        for (ListIterator<T> li(*this), ri(rhs); li; ++li, ++ri)
            if (li() != ri())
                return false;
        return true;
    }

    return false;
}

template<class T>
bool
List<T>::operator!= (const List<T>& rhs) const
{
    return !operator==(rhs);
}

template <class T>
void
List<T>::remove (ListIterator<T>& li)
{
    ListLink<T> *np = li.p->suc;
    remove(li.p);
    li.p = np;
}

template <class T>
void
List<T>::remove (const T& _v)
{
    for (ListIterator<T> litr(*this); litr; ++litr)
        if (litr() == _v)
            remove(litr);
}

template <class T>
void
List<T>::remove (const List<T>& _lv)
{
    for (ListIterator<T> litr(_lv); litr; ++litr)
        remove(litr());
}

template <class T>
void
List<T>::remove (ListLink<T>* ln)
{
    boxAssert(head !=0 && tail != 0);

    if (head == ln && tail == ln)
        head = tail = 0;
    else if (head == ln)
    {
        boxAssert(ln->pre == 0);
        head = ln->suc;
        head->pre = 0;
    }
    else if (tail == ln)
    {
        boxAssert(ln->suc == 0);
        tail = ln->pre;
        tail->suc  = 0;
    }
    else
    {
        boxAssert(ln->suc != 0 && ln->pre != 0);
        ln->suc->pre = ln->pre;
        ln->pre->suc = ln->suc;
    }
    delete ln;
    ln = 0;
}
