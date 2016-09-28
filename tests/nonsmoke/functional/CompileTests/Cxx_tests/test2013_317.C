// This is a copy of overtureBug_01.o
// It demonstrates a suprising problem with the current 
// version of ROSE (which otherwise passes all tests!)

#define MAKE_SAGE_FAIL 1
#define ALTERNATE_FIX 0

class GenericGridData_baseA
   {
     public:
          int computedGeometry;
   };

class GenericGridData_baseB
   {
     public:
          int computedGeometry;
   };

class GenericGridData : public GenericGridData_baseA, public GenericGridData_baseB
   {
     public:
          int computedGeometry;
   };

class GenericGrid
   {
     public:
#if !ALTERNATE_FIX
          void computedGeometry()
             {
               rcData->computedGeometry = 0;
               rcData->GenericGridData_baseA::computedGeometry = 0;
               rcData->GenericGridData_baseB::computedGeometry = 0;
             }
#endif

          typedef GenericGridData RCData;

       // Note that currently the unparser reports that the parent of rcData
          RCData* rcData;

#if ALTERNATE_FIX
          void computedGeometry()
             { rcData->computedGeometry = 0; }
#endif
   };
