/*
Hi Dan,

[What better way to return from Europe than with a bug report in your inbox.]

In a simple program using varargs, a SgVarRefExp of type va_list has a
NULL grandparent (i.e., SgVarRefExp->get_parent()->get_parent() is NULL).
This prevents me from invoking its unparseToString() method.

I am including a traversal which queries for SgVarRefExps, looks at their
parent and parent->parent nodes, and prints them using unparseToString().
When run on this program, varArg.C:

#include <stdarg.h>

void ellipsis_int(int i, ...)
{
    int j;
    va_list ap;

    j = va_arg(ap,int);

}

int main()
{
  return 0;
}

The translator works fine under 0.8.7a and returns:

[bwhite@tux83 null-parent-traversal]$ ./translator ./varArg.C
varRefExp: j
varRefExp: ap

But it fails under 0.8.8a:

[bwhite@tux83 null-parent-traversal]$ ./translator ./varArg.C
varRefExp: j
grand parent is NULL
translator:
/usr/casc/overture/bwhite/rose-staging-area/ROSE-0.8.8a/src/roseSupport/utility_functions.C:719:
static std::string rose::getFileNameByTraversalBackToFileNode(SgNode*):
Assertion `parent != __null' failed.
Abort

Thanks,
Brian
*/

#include <stdarg.h>

void ellipsis_int(int i, ...)
{
    int j;
    va_list ap;

    j = va_arg(ap,int);

}

int main()
{
  return 0;
}
