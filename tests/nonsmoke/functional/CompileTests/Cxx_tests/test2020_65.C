
extern "C" int foobar(void(*f)(void));

extern "C"
   {
     int foobar(void (*f)(void));
   }

extern "C"
   {
     int foobar(void (*f)(void)) { return 42; };
   }

