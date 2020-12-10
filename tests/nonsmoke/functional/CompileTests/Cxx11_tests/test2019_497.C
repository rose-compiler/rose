
// This corresponds to the assertion failing at line 36494 of the Cxx_Grammar.C file.
// This is tied to be the 3rd most numerous issue in testing (causing failure in 3 files).

namespace std
   {
     struct __true_type {};
     struct __false_type {};

     template<typename _Tp>
     struct __is_void
        {
          enum { __value = 0 };
          typedef __false_type __type;
        };

     template<>
     struct __is_void<void>
        {
          enum { __value = 1 };
          typedef __true_type __type;
        };
   }

class Ba_A
   {
     public:
          Ba_A(void);
   };

class Ba_B
   {
     public:
          Ba_B();
   };

class C
   {
     public:
          Ba_B getList() const;
     private:
          int getValue(int index) const;
   };

class Ba_D
   {
     public:
          static Ba_D & getObject(void);
          Ba_A baGetValue(int id);
   };


Ba_B C::getList() const
   {
     Ba_D & m = Ba_D::getObject();

     Ba_A data;
     Ba_B list;

#if 1
     {
    // Demonstrate error in Cxx_Grammar.C:36494
       data = m.baGetValue(getValue(42));
     }
#endif

#if 0
    // Demonstrate error in sageInterface.C:19475
       data = m.baGetValue(getValue(42));
#endif
   }
