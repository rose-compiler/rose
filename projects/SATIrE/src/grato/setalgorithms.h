// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: setalgorithms.h,v 1.2 2007-03-08 15:36:49 markus Exp $

// Author: Markus Schordan

#ifndef SETALGORITHMS_H
#define SETALGORITHMS_H

template<typename set_type>
void intersection(const set_type& s1, const set_type& s2, set_type& result) {
  set_type temp;
  typename set_type::iterator i1=s1.begin();
  typename set_type::iterator i2;

  while(i1!=s1.end()) {
    i2=s2.find(*i1++);
    if(i2!=s2.end())
      temp.insert(*i2);
  }
  temp.swap(result);
}

// result=s1-s2;
template<typename set_type>
void set_difference(const set_type& s1, const set_type& s2, set_type& result) {
  set_type temp;
  typename set_type::iterator i1=s1.begin();
  typename set_type::iterator i2;

  while(i1!=s1.end()) { // foreach *i1 in s1
    i2=s2.find(*i1);
    if(i2==s2.end())    // if *i1 notin s2
      temp.insert(*i1); // then temp:=temp+*i1
    i1++;
  }
  temp.swap(result);
}


#endif
