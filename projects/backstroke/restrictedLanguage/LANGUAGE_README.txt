					Restricted Language Revision 2

1) No variables of pointer or array types are declared inside the function. Arguments
	can be of pointer types.
	REASON: We would like to be able to statically determine which values are modified,
	without getting into alias analysis. We need to allow arguments of pointer type because
	ROSS events take pointers to the simulation state.

2) Variables of pointer types (function arguments) are never modified.
	REASON: We want to allow pointers
	because they are used extensively in ROSS, but we would like to treat them as simply
	representatives to their original rvalue, rather than arbitrary references to memory.

3) The state of a function consists of the rvalues referred by arguments of a pointer type,
	and any variables outside of its scope that are modified.
	REASON: In ROSS, the state is passed as a pointer to a struct. In SPEEDES, the
	state consists of member variables of the class of the event function.

4) Function arguments are never passed by reference
	REASON: We want a guarantee that function calls never modify local variables,
	only state variables.

5) Only types allowed are scalar types and composite types containing only scalars.
	Note: No arrays
	REASON: Simplicity of reverse code generation. Permissive enough to accommodate
	ROSS's model where the state is a struct.

6) Pointer variables can only have arrow operator and dereference operator applied.
	No bracket operator [] (i.e. arrays)
	REASON: We don't want pointers to be used as a proxy for arrays.

7) No jump structures (goto, break outside of switch statement, continue)
	REASON: Simplify handling of loops.

8) No dynamic memory allocation
	REASON: We would like to statically determine the state to be saved.

9) No ellipse arguments (varargs)

10) No threading

11) No I/O
	REASON: I/O is irreversible an has to be handled by a commit method.

12) No calling functions through function pointers or virtual functions.
	REASON: We cannot call the correct reverse function in those cases.

13) No throw or catch statements

14) Limited use of standard library functions
	Pure functions such as abs, max, cos, sin are ok.
	In the first implementation, we'll have an explicit whitelist of standard library
	functions allowed





            Initial restricted design space (Language 1)

   To simplify the initial development we will restrict the applications
to which our work will apply (the programs that we will accept). The
restrictions will be enforced and this will provide experience with
general restrictions that we expect in the final support for
automated rollback code generation.

Restrictions:
   1) Variables that are part of the simulation state will be global.
   2) Variables will be integers (only one type supported)
   3) function take arguments only by value, but may have
      side effects on global variables and return integer values.
   4) functions can have local (stack) variables
   5) Functions have a single scope
   6) No jump structures (no goto, break (except in switch), continue)
   7) No threading
   8) No I/O
   9) No dynamic memory allocations
  10) No composite types
  11) No boolean operators
  12) No relational operators
  13) Only control structures allowed are:
        function call
  14) No recursion will be permitted.
  15) No ellipse arguments


Initial work will support +,-,*, and \.  Zero divide will be our proxy
for special cases that will likely come up in other operators.

To be more precise, we explicitly declare some constructs to
be allowed:
   1) Comments
   3) Assignment, and arithmetic operators.

The input language is restricted, but language that we use for
the generated code has no restrictions.