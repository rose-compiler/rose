// Modification of ROSE-42 (Kull)
// ROSE-1307

// This modified version of ROSE-42 (test2018_134.C) is to explore
// how we want to handle sharing of types declared between different 
// classes.

typedef unsigned long* size_t;

typedef size_t* required_type;

template< typename _Key >
class _Rb_tree
   {
     public:
       // typedef size_t size_type;
          size_t* size_type;

       // We need to use the "const size_type" type.
       // const size_type __old_size;
          const size_type __old_size;
   };

template< typename _Tp >
class vector 
   {
     public:
       // NEEDED:
       // typedef size_t size_type;
          size_t* size_type;
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
     const vector<int>::size_type local1 = 0L;
#else
     vector<int>::size_type local1 = 0L;
#endif
   }

