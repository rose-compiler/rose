// Template zur Anzeige von Sequenzen
// Template for the display of sequences (file include/showseq)
#ifndef SHOWSEQ_H
#define SHOWSEQ_H

#include<iostream>

namespace br_stl {

template<class Container>
void showSequence(const Container& s, const char* sep = " ",
                  std::ostream& where = std::cout) {
   typename Container::const_iterator iter = s.begin();
   while(iter != s.end())
      where << *iter++ << sep;
   where << std::endl;
}


} // namespace br_stl

#endif

