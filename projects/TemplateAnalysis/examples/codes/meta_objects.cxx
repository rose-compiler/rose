
struct sentinel {};

template<typename Te, class Tn=sentinel>
struct list {
  typedef Te element;
  typedef Tn next;
};

template<class Tl>
struct len {
  enum { value = len<typename Tl::next>::value + 1 };
};

template<>
struct len<sentinel> {
  enum { value = 0 };
};

template<class Te, class Tl>
struct append { 
  typedef list< typename Tl::element, typename append<Te, typename Tl::next>::result> result;
};

template<class Te>
struct append<Te, sentinel> {
  typedef list<Te,sentinel> result;
};



#ifdef RUNTEST
template <int n> 
struct intval {
  enum { value = n };
};

typedef list<intval<3>, list<intval<1>, list<intval<2>, list<intval<4>, sentinel> > > > alist;

int main() {
#if RUNTEST == 0
  return len< alist >::value;
#else
  return len< append< intval<7> , alist >::result >::value;
#endif
}
#endif

