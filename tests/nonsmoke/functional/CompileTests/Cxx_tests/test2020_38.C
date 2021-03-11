// This is a bug in the output of the extern keyword.
extern "C" 
   {

     typedef struct
        {
          int id;
        } *XYZ;

  // Original code:  extern XYZ *functionReturningXYZ(void);
  // Generated code: XYZ *functionReturningXYZ(void);
     extern XYZ *functionReturningXYZ(void);
   }
