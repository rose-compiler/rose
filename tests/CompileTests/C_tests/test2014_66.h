// #undef abc
struct sigaction
   {
     union
      {
        int sa_handler;
        void (*sa_sigaction)();
      } union_in_X;

// This is the problem macro that will be expaned twice.
# define sa_handler union_in_X.sa_handler
# define sa_sigaction union_in_X.sa_sigaction
   };

// DQ (11/5/2012): Added typedef hides use of sigaction (so our previous fix will have resolve to base type).
typedef struct sigaction sigaction_t;

// When the declarations, "sigaction_t x1;" are in a header file then we
// have a more complex problem that requires we output the #undef directives
// in the source file where the use of the variable is located.
sigaction_t x1;
sigaction_t x2[2];
