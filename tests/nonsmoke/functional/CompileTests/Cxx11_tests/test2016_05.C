
namespace std
   {
     class string { public: string(const char* s); };
     template <typename T1, typename T2> class pair { public: pair(T1 a, T2 b); };
  // template<typename _Tp> struct vector { public: vector(const _Tp & x); };
     template <class Key, class T> class map { public: map(const pair<Key,T> & x); };
   }

// class AstAttribute {};
// class dsl_attribute : public AstAttribute {};
class dsl_attribute {};
class array_dsl_attribute : public dsl_attribute {};

std::map<std::string,dsl_attribute> dsl_attribute_map = { {"array", array_dsl_attribute()} };
// std::map<std::string,dsl_attribute> dsl_attribute_map = { {"array", array_dsl_attribute()} , {"array", array_dsl_attribute()} };

