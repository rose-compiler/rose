// include/hashfun.h
// Standard function object
#ifndef HASH_FUNCTION_H
#define HASH_FUNCTION_H

#include "ConstraintRepresentation.h"

namespace br_stl {

template<class IndexType>
class HashFun {
   public:
    HashFun(long prime=1009) : tabSize(prime) {}
    long operator()(IndexType p) const {
       return long(p) % tabSize;
    }
    long tableSize() const { return tabSize;}
   private:
    long tabSize;
};



} // namespace br_stl


#endif

