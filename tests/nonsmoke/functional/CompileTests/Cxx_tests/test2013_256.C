#if 0
enum Protocols
   {
     File
   };

const int Protocols_Count = 1;
#else
   #include "test2013_256.h"
#endif

struct ProtoEntry
   {
     Protocols   protocol;
     unsigned int        defPort;
   };

// Unparsed as: 
// static struct ProtoEntry gProtoList[Protocols_Count] = 
// {
//    {(0)}, 
// };


static ProtoEntry gProtoList[Protocols_Count] =
   {
     { File , 0  }
   };
