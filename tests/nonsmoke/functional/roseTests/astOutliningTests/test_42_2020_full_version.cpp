// This is the paper test_42.cpp
// SageInterface::convertRefToInitializedName() on SgFunctionCallExp
#include <iterator>

typedef unsigned int uint32_t;

template < class Key, class T, class Compare = std::less<Key> > class MyMap
    {
      struct N;

      public:
           class iterator : 
std::iterator<std::bidirectional_iterator_tag, T>
              {
                public:
                     T* operator->() const;
              };
    };

namespace A
    {
      namespace B
         {
           enum Type
              {
                ENUM_B = 0,
                ENUM_C = 1
              };
         }
    }

struct C
    {
      bool var_1;
    };

struct D
    {
      C var_2;
    };

class E
    {
      private:
           void foobar(void);
           bool var_3;
    };

using namespace A;

class F
    {
      public:
           enum Type
              {
                AB_ENUM
              };
    };


extern "C" uint32_t var_4;
extern "C" B::Type foo();

void E::foobar(void)
    {
#pragma rose_outline
      switch (foo())
         {
           case B::ENUM_B:
              {
                bool var_5 = false;
                MyMap<uint32_t,D>::iterator var_6;
                if (true)
                  {
                    bool var_7 = var_6->var_2.var_1;
                  }
                if (var_4 == F::AB_ENUM)
                  {
                  }
                break;
              }

           case B::ENUM_C:
                var_3 = false;
                if (var_4 != F::AB_ENUM)
                   {
                   }
                break;

           default:
                break;
         }
    }

