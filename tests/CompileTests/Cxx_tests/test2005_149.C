// template<class T> class Y;

// Skip version 4.x gnu compilers
// TOO (2/15/2011): error for Thrifty g++ 3.4.4
#if ( __GNUC__ == 3 && __GNUC_MINOR__ != 4)

template<class T>
class X
   {
          T *tt;
     public:
          inline X() : tt(0) { Y<int> y; y = 2; }
          inline ~X() { if (tt) delete tt; }
          inline X& operator=(const T& t) { return *this; }
          inline operator T&() const { return *tt; }
          inline T *operator&() { return tt; }
   };

template<class T>
class Y
   {
          T *tt;
     public:
          inline Y() : tt(0) { X<int> x; x= 1; }
          inline ~Y() { if (tt) delete tt; }
          inline Y& operator=(const T& t) { return *this; }
          inline operator T&() const { return *tt; }
          inline T *operator&() { return tt; }
   };

// Template Instantiation Directive
// template X<int>;

#if 1
int main()
   {
     X<int> x;
     Y<int> y;
     x = 7;
     y = 42;
   }
#endif

#else
  #warning "Not tested on gnu 4.0 or greater versions"
#endif

