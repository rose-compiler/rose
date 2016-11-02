
template <typename T>
class X
   {
     public:
          void foo();

          X<T> & operator=( X<T> & i )
             {
               return *this;
             }

          friend X<T> & operator+( X<T> & i, X<T> & j)
             {
               return i;
             }

         X<T> & operator-()
             {
               return *this;
             }

         X<T> & operator++()
             {
               return *this;
             }

         X<T> & operator++(int x)
             {
               return *this;
             }

         X<T> & operator->()
             {
               return *this;
             }
   };

X<int>* foobar()
   {
     return 0;
   }

template <typename T>
T & operator+( T & i, T & j)
   {
     return i;
   }

int main()
   {
     X<int>* xPtr;
     X<int>** xPtrPtr;
     X<int> x,y,z;

  // Error used to be unparses as: "x = (+< X< int > > (y,z));"
  // x = y + z;

#if 1
  // Both of the following statements normalize to the following: xPtr -> foo();
     xPtr->foo();
     (*xPtr).foo();

  // Both of these normalize to the following: (*(*xPtrPtr)).foo();
     (*xPtrPtr)->foo();
     (**xPtrPtr).foo();
#endif

#if 1
  // This is unparsed as: "x = -xptr;"
  // x = xptr->operator-();
     x = (*xPtr).operator-();
#endif
#if 1
  // Postfix operators
     x = (*xPtr)++;
     x = (*xPtr).operator++(1);
#endif
#if 1
  // Prefix operators (error same as for "x = (*xPtr).operator-();")
     x = ++(*xPtr);
     x = (*xPtr).operator++();
     x = xPtr->operator++();
#endif

     int separatorDeclaration;

#if 1
  // Postfix operator using a non-SgVerRefExp 
     x = (*(foobar()))++;
     x = (*(foobar())).operator++(1);
#endif
#if 1
  // Prefix operator using a non-SgVerRefExp 
     x = ++(*(foobar()));
     x = (*(foobar())).operator++();
     x = foobar()->operator++();
#endif
#if 1
     x = y.operator-();

     x = y + z;
#endif

     return 0;
   }


