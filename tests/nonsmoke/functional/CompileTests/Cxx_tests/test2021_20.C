void foo(const char* f, ...);

class A
   {
     public: int baValue(void) const;
   };

template <typename T1, typename T2>
class Map
   {
     public:
          class iterator
             {
               public: T2* operator->() const;
             };
          void insert(int,int);
   };

void foobar()
   {
     Map<int,A>::iterator iter;
     foo("%d\t ",iter->baValue());
   };

