
#if 1
   #include <vector>
#else

#include <initializer_list>
namespace std
   {
  // class string { public: string(const char* s); };
  // template <typename T1, typename T2> class pair { public: pair(T1 a, T2 b); };
  // template<typename _Tp> struct vector { public: vector(const _Tp & x); };
     template<typename _Tp> struct vector { public: vector(std::initializer_list<int> list); };

  // template <class Key, class T> class map { public: map(const pair<Key,T> & x); };
   }

#endif

class dsl_attribute 
   {
     public:
          dsl_attribute();
          dsl_attribute(const dsl_attribute & X);
   };

// std::vector<dsl_attribute> abc1 = { dsl_attribute };  // Error in frontend
std::vector<dsl_attribute> abc2 = { dsl_attribute() }; // Correct code
std::vector<dsl_attribute> abc3 = { dsl_attribute{} }; // Correct code, try to figure out the difference.
std::vector<dsl_attribute> abc4 = { dsl_attribute{dsl_attribute{}} }; // Strange but correct.

std::vector<dsl_attribute> abc5 = { dsl_attribute() }; // Correct code
std::vector<dsl_attribute> abc6 = {}; // Correct code
