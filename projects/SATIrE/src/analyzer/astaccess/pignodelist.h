#ifndef H_PIGNODELIST
#define H_PIGNODELIST

#include "cfg_support.h"

typedef std::list<void *> nodelist;

class PigNodeList
    : public std::pair<nodelist *, nodelist::const_iterator>
{
public:
    template <class T, class T2> PigNodeList(const std::list<T, T2> &l)
    {
        nodelist *newlist = new nodelist();
        typename std::list<T, T2>::const_iterator i, j;
        j = l.end();
        for (i = l.begin(); i != j; ++i)
            newlist->push_front(*i);
        first = newlist;
        second = newlist->begin();
    }
    template <class T, class T2> PigNodeList(
            std::list<T, T2> *l, typename std::list<T, T2>::const_iterator i)
    {
        first = l;
        second = i;
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
    PigNodeList();
};

#endif
