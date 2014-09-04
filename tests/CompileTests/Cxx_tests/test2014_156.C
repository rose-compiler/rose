namespace std
   {
     template<typename _Tp>
     class allocator
        {
        };

     template<typename _Key , typename _Compare , typename _Alloc = std::allocator<_Key> >
     class set
        {
          public:
               set(const _Compare& __comp) { }
        };
   }

bool compareSolutions(int x);

void foobar()
   {
  // This is a variable declaration.
     std::set<int, bool(*)(int)> XXX_triedSolutions(compareSolutions);

  // This is a simple function call.
     compareSolutions(1);

  // This is a function declaration (not a variable declaration).
     std::set<int, bool(*)(int)> YYY_triedSolutions();
   }
