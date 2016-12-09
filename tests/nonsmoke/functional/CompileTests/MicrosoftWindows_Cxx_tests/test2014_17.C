// int* __declspec( restrict ) foobar();

struct X
   {
  // int* __declspec( restrict ) foobar();
  // int* foobar() restrict;
  // int* foobar() __restrict__;

  // This will compile, however the restrict keyword is lost (unavailable in EDG).
     int* __declspec( restrict ) foobar();
   };

