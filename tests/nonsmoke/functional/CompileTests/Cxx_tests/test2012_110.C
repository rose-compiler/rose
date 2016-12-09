// Nested typedef using the tag of the autonomous type
#if 1
typedef struct Atag4
   {
     typedef Atag4 Btag;
   } A4;
#else
struct Atag4
   {
     typedef Atag4 Btag;
   };

struct Atag4 A4;
#endif

