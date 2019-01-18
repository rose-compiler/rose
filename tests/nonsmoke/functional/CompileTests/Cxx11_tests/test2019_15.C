
#if 1
#include<initializer_list>
#include<set>
#else
namespace std {
    template<class _E>
    class initializer_list {
    public:
        typedef const _E* const_iterator;
    private:
         initializer_list(const_iterator __a, int __l) {}
    };

    template<typename _Key>
    class set {
    public:
        set(initializer_list<_Key> __l) {}
    };
}
#endif

class Class1 {
public:
    std::set<int> func1();
};

std::set<int> Class1::func1() {
  // BUG: should be unparsed as: return (std::set<int>({1,2}));
  return (std::set<int>({1,2}));
}
