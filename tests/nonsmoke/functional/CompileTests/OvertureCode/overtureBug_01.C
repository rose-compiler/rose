// this is a very old test code that demonstrated a bug in compiling Overture.
// We need to get an updated version of Overture headers and revisit the 
// testing of Overture.

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

#if MAKE_SAGE_FAIL
          typedef GenericGridData RCData;
          RCData* rcData;
#else
          GenericGridData* rcData;
#endif

#if ALTERNATE_FIX
          void computedGeometry()
             { rcData->computedGeometry = 0; }
#endif
   };

