enum { I_RING_SIZE = 4 };

// Currently unparses as: extern int (*_gl_verify_function18())[!(!(sizeof(struct )))];
extern int (*_gl_verify_function18 (void)) [(!!sizeof (struct ABC { unsigned int bitField: (1 <= I_RING_SIZE) ? 1 : -1; }))];

struct XYZ
   {
     float value;
   };

extern int (*function_pointer__array (void)) [(!!sizeof (struct XYZ))];
