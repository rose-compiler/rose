// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: pignodelist.h,v 1.5 2008-05-13 10:18:32 gergo Exp $

#ifndef H_PIGNODELIST
#define H_PIGNODELIST

#include "cfg_support.h"

// GB (2008-05-13): This class was removed in favor of the new
// Ir::createNodeList interface. That is less clumsy and makes memory
// management easier.
#if 0
typedef std::vector<void *> nodelist;

class PigNodeList
    : public std::pair<nodelist *, nodelist::const_iterator>
{
public:
    template <class T, class T2> PigNodeList(const std::vector<T, T2> &l)
    {
        nodelist *newlist = new nodelist();
        typename std::vector<T, T2>::const_iterator i, j;
        j = l.end();
        for (i = l.begin(); i != j; ++i)
            newlist->push_back(*i);
        first = newlist;
        second = newlist->begin();
    }
    PigNodeList(const SgExprListExp *e)
    {
        PigNodeList(e->get_expressions());
    }

    int empty()
    {
        return first->end() == second;
    }

    void *head()
    {
        return *second;
    }

    void *tail()
    {
        nodelist::const_iterator i = second;
        ++i;
        // return new std::pair<nodelist *, nodelist::const_iterator>(first, i);
        return new PigNodeList(first, i);
    }

private:
    template <class T, class T2> PigNodeList(
            std::vector<T, T2> *l, typename std::vector<T, T2>::const_iterator i)
    {
        first = l;
        second = i;
    }
};
#endif

#endif
