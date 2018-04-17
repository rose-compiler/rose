// This example is similar to test2012_17.c

// This example if from sigaction.h:
// This example demonstrates where a macro name is expanded twice (first 
// in the AST and then again in the processing of the unparsed file).

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

void foo()
   {
     sigaction_t x1;
     sigaction_t x2[2];

  // The only solution I can think of is to generate a macro such as: "#undef abc" so 
  // that we can avoid this being expaned twice.  Note: rewrapping macros is still not 
  // ready for use in ROSE yet.
  // #undef abc
     x1.sa_handler = 42;
     x1.sa_sigaction = 0L;
     x2[0].sa_sigaction = 0L;
   }
