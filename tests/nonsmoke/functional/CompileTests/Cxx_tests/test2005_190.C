int x = 1 + 2;

int hex = 0x0001;
struct __ios_flags
   {
     typedef short __int_type;
#if (__GNUC__ >= 3)
     const __int_type _S_boolalpha; // = 0x0001;
#else
     const __int_type _S_boolalpha = 0x0001;
#endif
  // static const __int_type _S_boolalpha = 0x0001;
  // static const __int_type _S_boolalpha = 0x0001 + 0x0002;
   };

float y = 3.14 + 1.00;

bool somethingTrue = !false;

char c1 = 'a' + 1;

// This seems like it should be a bug, but it works fine (in EDG, at least)
char c2 = 'a' + true;
