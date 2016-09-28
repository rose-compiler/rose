// This is a simpler version of test2012_134.c

// This is the essential bug in ROSE from the module.c file of the zsh C application.
// This is the same as the issue with params.c from zsh also.

typedef int (*Hookfn) (int);

struct LinkNode
   {
     void *dat;
   };

void
runhookdef()
   {
     struct LinkNode* p;
     int r;

  // The calls to the function might need the cast to Hookfn.

  // Unparses to: r = (p -> dat)(42);
     r = ((Hookfn)((p)->dat))(42);
   }


