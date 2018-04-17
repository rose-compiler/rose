// This test code demonstrates output of the template member functions
// some of which require special handling 

template<class T> class SwigValueWrapper {
    T *tt;
public:
    inline SwigValueWrapper() : tt(0) { }
    inline ~SwigValueWrapper() { if (tt) delete tt; } 
    inline SwigValueWrapper& operator=(const T& t) { tt = new T(t); return *this; }
    inline operator T&() const { return *tt; }
    inline T *operator&() { return tt; }
};

// Template Instantiation Directive
// template X<int>;

#if 1
int main()
   {
     SwigValueWrapper<int> b;
     b = 42;
   }
#endif

