
#if 1
#include<initializer_list>
#include<set>
#else
namespace std {
    template<class _E>
    class rose_initializer_list {
    public:
        typedef const _E* const_iterator;
    private:
         rose_initializer_list(const_iterator __a, int __l) {}
    };

    template<typename _Key>
    class set {
    public:
        set(_Key __l, _Key y) {}
        set(rose_initializer_list<_Key> __l) {}
    };
}
#endif

class Class1 {
public:
    std::set<int> func1();
};

std::set<int> Class1::func1() {
  int abcdefg;
  // BUG: should be unparsed as: return (std::set<int>({1,2}));
  return (std::set<int>({1,2}));
}
