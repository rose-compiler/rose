// number #31

#define ALTERNATE_FIX 0

class GenericGridData
   {
     public:
          int computedGeometry;
   };

class GenericGrid
   {
     public:
#if !ALTERNATE_FIX
          void computedGeometry()
             { rcData->computedGeometry = 0; }
#endif

          typedef GenericGridData RCData;
       // Note that currently the unparse reports that the parent of rcData
          RCData* rcData;

#if ALTERNATE_FIX
          void computedGeometry()
             { rcData->computedGeometry = 0; }
#endif
   };
