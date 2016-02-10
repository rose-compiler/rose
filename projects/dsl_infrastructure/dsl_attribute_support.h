
// We need the rose.h so that we can get the defining declaration of AstAttribute
// Which we require to be a base class to all of the generated attributes.
#ifndef SKIP_ROSE_HEADERS
// We mostly need this to be able to compile this file because of the reference to AstAttribute.
   #include "rose.h"
#else
// Declearations we need to process this file using ROSE in C++11 mode (also simpler then processing "rose.h").
#if 1
#include <string>
#include <vector>
#include <map>
#else
namespace std
   {
     class string { public: string(const char* s); };
     template<typename _Tp> struct vector { public: vector(const _Tp & x); };
#if 1
     template <typename T1, typename T2> class pair { public: pair(T1 a, T2 b); };
     template <class Key, class T> class map { public: map(const pair<Key,T> & x); };
#endif
   }
#endif

class AstAttribute {};

// Note clear if we need this
// class dsl_attribute : public AstAttribute {};

#endif

