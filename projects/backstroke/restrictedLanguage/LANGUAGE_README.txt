            Initial restricted design space (Language 1)

   To simplify the initial development we will restrict the applications
to which our work will apply (the programs that we will accept). The
resctrictions will be enforced and this will provide experience with
general restrictions that we expect in the final support for
automated rollback code generation.

Restrictions:
*  1) Variables that are part of the simulation state will be global.
*  2) Variables will be integers (only one type supported)
   3) function take arguments only by value, but may have
      side effects on global variables and return integer values.
   4) functions can have local (stack) variables
*  5) Functions have a single scope
   6) No jump structures (no goto, break (except in switch), continue)
   7) No threading
   8) No I/O
   9) No dynamic memory allocations
  10) No composite types
* 11) No boolean operators
* 12) No relational operators
  13) Only control structures allowed are:
        function call
  14) No recursion will be permitted.
  15) No ellipse arguments
* 16) Limited use of pointers, as required by the simulator only
* 17) No constructors whose arguments have side effects
* 18) In any expression, between two sequence points only one value can be modified

Initial work will support +,-,*, and \.  Zero divide will be our proxy
for special cases that will likely come up in other operators.

To be more precise, we explicitly declare some constructs to
be allowed:
   1) Comments
   3) Assignment, and arithemetic operators.

The input language is restricted, but language that we use for
the generated code has not restrictions.