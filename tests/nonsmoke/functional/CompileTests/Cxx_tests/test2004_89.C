// This is an example of the failure to find a symbol in the case of dik_constructor 
// within the handling of expressions in sage_gen_expression() within the EDG/Sage 
// connection code.  The example appears in Boost, but in a more complex form!
// The same error appears to occure in the handling of STL (see test2004_35.C).

// The problem and the fix:
// This code demonstrates the case of using a compiler generated function.
// Previous to the fix, without the default constructor explicitly defined 
// a symbol would not be generated and the use of the function would fail.
// The fix was to detect the use of compiler generated functions and allow
// the symbol to be constructed upon first use (applies to template cases as well).

class use_count_is_zero // : public std::exception
   {
     public:
#if 0
       // Defining the default constructor causes the program to compile properly
          use_count_is_zero() {}
#endif
          virtual char const * what()
             {
               return ""; // boost::use_count_is_zero";
             }
   };

class counted_base
   {
     public:

          void add_ref()
             {
               throw (use_count_is_zero());
             }
   };
