/*
The following code:

int f(int x __attribute__((unused))) {
  x = 0;
  return 1;
}

causes sage_gen_be.C to segmentation fault during parsing.  The problem line appears to be:
write_variable_attributes(param_var);
around line 6658 of sage_gen_be.C; the file descriptor used to write the attributes is NULL (presumably because ROSE is not supposed to be generating C++ code like the original EDG unparser was doing).

-- 
Jeremiah Willcock        Lawrence Livermore National Laboratory
Phone: (925)-422-7790    Mail: L-550 
*/

int f(int x __attribute__((unused))) {
  x = 0;
  return 1;
}
