class A
   {
     public:
#if 1
         A();
#else
         A() {}
#endif
   };

#if 1
A::A()
   {
   }
#endif

int 
main()
   {
     return 0;
   }


