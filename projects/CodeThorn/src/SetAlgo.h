// include/setalgo.h
#ifndef SETALGO_H
#define SETALGO_H
#include<cassert>

namespace br_stl {

template<class set_type>
bool Includes(const set_type& s1, const set_type& s2) {
    // Is s2 contained in s1?
    if(&s1 == &s2)       // save time if the sets are identical
       return true;

    typename set_type::const_iterator i = s2.begin();

    while(i != s2.end()) {
        if(s1.find(*i++) == s1.end())     // not found
           return false;
    }
    return true;
}

template<class set_type>
void Union(const set_type& s1, const set_type& s2,
           set_type& result) {
    set_type temp(s2);
    if(&s1 != &s2) {
       typename set_type::const_iterator i = s1.begin();
       while(i != s1.end()) 
            temp.insert(*i++);
    }
    temp.swap(result);
}

template<class set_type>
void Intersection(const set_type& s1, const set_type& s2,
                  set_type& result) {
    set_type temp;
    typename set_type::const_iterator i1 = s1.begin(), i2;

    // An identity check makes no sense, because in case
    // of identity, temp must be filled anyway.

    while(i1 != s1.end()) {
        i2 = s2.find(*i1++);
        if(i2 != s2.end())
           temp.insert(*i2);
    }
    temp.swap(result);
}

template<class set_type>
void Difference(const set_type& s1, const set_type& s2,
                set_type& result) {
    set_type temp;
    typename set_type::const_iterator i = s1.begin();

    if(&s1 != &s2)
       while(i != s1.end()) {
           if(s2.find(*i) == s2.end())   // not found
               temp.insert(*i);
           ++i;
       }
    temp.swap(result);
}

template<class set_type>
void Symmetric_Difference(const set_type& s1,
                          const set_type& s2,
                          set_type& result) {
    set_type temp;
    typename set_type::const_iterator i = s1.begin();

    if(&s1 != &s2) {
       while(i != s1.end()) {
           if(s2.find(*i) == s2.end())  // not found
              temp.insert(*i);
           ++i;
       }

       i = s2.begin();
       while(i != s2.end()) {
           if(s1.find(*i) == s1.end())  // not found
              temp.insert(*i);
           ++i;
       }
    }
    temp.swap(result);
}


} // namespace br_stl 

#endif  //  setalgo

