// This example if from sigaction.h:
// This example demonstrates where a macro name is expanded twice (first 
// in the AST and then again in the processing of the unparsed file).

// #undef abc
struct sigaction
   {
     union
      {
        int sa_handler;
      } union_in_X;

// This is the problem macro that will be expaned twice.
# define sa_handler union_in_X.sa_handler
   };

void foo()
   {
     struct sigaction x1;

  // The only solution I can think of is to generate a macro such as: "#undef abc" so 
  // that we can avoid this being expaned twice.  Note: rewrapping macros is still not 
  // ready for use in ROSE yet.
  // #undef abc
     x1.sa_handler = 42;
   }
