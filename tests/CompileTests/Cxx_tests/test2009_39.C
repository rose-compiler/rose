/*
Hi Dan,

this is a bug report regarding the EDG-Sage connection. When calling the
frontend (e.g., by starting the identityTranslator) on the attached file
func_t.c, ROSE fails with the following message:

identityTranslator: /home/dquinlan/ROSE/svn-test-rose/svn-readonly-rose/src/frontend/CxxFrontend/EDG_SAGE_Connection/sage_gen_be.C:19921: SgFunctionDeclaration* sage_gen_routine_decl(a_boolean, a_boolean*): Assertion `result != __null' failed.
Aborted

The unusual feature of the code example is that a function is declared using
a typedef for its function type. Unfortunately, this construct is common in
code that we are currently trying to analyze.

I'd like to help you debug this issue, but we don't have any current
versions of ROSE with the EDG connection source code. Could you make a
tarball with the full ROSE and EDG source code available for us? Neither
Markus nor I have SVN access anymore.


Thanks,
Gergo
*/

// typedef int (*func_t)(int); --- This works
typedef int func_t(int);

// Fails in this statement
// extern func_t my_function; // Should appear like this in unparsed code.
extern func_t my_function;
