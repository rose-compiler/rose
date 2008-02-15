
// extern int s11;

static int s11 = 11;

// extern int s11;
// extern int s11;
// extern int s11;

void foo()
   {
  // This causes an error in ROSE: Error: add failed. Attribute: test1 exists already.
     extern int s11;
   }

