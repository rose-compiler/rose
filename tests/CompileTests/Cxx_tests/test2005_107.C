// This is a copy of overtureBug_01.o
// It demonstrates a suprising problem with the current 
// version of ROSE (which otherwise passes all tests!)

#define MAKE_SAGE_FAIL 1
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
