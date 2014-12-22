// template<typename T> 
class Class_A
   {
     public:
          Class_A();
   };

#if 1
template<typename T> 
class Template_Class_A
   {
     public:
          T xxx;
          Template_Class_A();
   };
#endif

// struct xyz
struct
   {
#if 1
  // This presence of a type that calls a constructor causes a constructor to be 
  // generated for the struct, this constructor will be a function with out a name.
     Class_A var_0;
#endif
#if 1
     Template_Class_A<int> var_1;
#endif
   } tcl;

