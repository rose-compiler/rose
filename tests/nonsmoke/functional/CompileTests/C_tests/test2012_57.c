enum { I_RING_SIZE = 4 };


// Currently unparses as: extern int (*_gl_verify_function18())[!(!(sizeof(struct )))];
// Note that these are always un-named, but they could be named and if so it is less clear how to
// know when to unparse them into the correct place.  So let this test code be restricted to
// testing un-named cases more similar to what I see in real applications.
// extern int (*_gl_verify_function18 (void)) [(!!sizeof (struct ABC { unsigned int bitField: (1 <= I_RING_SIZE) ? 1 : -1; }))];
extern int (*_gl_verify_function18 (void)) [(!!sizeof (struct { unsigned int bitField: (1 <= I_RING_SIZE) ? 1 : -1; }))];

struct XYZ
   {
     float value;
   };

extern int (*function_pointer__array (void)) [(!!sizeof (struct XYZ))];
