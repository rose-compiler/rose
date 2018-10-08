// ROSE-42 (Kull)
// ROSE-1307

typedef unsigned long size_t;

typedef size_t required_type;

template< typename _Key >
class _Rb_tree
   {
     public:
          typedef size_t size_type;

       // We need to use the "const size_type" type.
          const size_type __old_size;
   };

template< typename _Tp >
class vector 
   {
     public:
       // NEEDED:
          typedef size_t size_type;
   };

void func1() 
   {
  // We are using the wrong class to support the name qualification.  
  // The use of "const" appears to make all the difference. 
  // This should be unparsed as: 
  // const std::vector<Class1*>::size_type local1 = 0;

#if 1
  // const vector<int>::size_type local1 = 0;
  // const vector::size_type local1 = 0;
     const vector<int>::size_type local1 = 0;
#else
     vector<int>::size_type local1 = 0;
#endif
   }

