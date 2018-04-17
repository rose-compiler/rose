/*
Here is a minimal example of a bug in the EDG/SAGE connection (again
extracted from the IU C++ bindings for MPI).  The default constructor for
iter is necessary to reproduce the bug, even though it is never used.  The
use of a constructor assignment for pos is also necessary, although p can
also be a local variable and the bug will still appear.

Thank you,
Jeremiah Willcock
*/

struct iter {iter();};
void insert(iter p) {iter pos(p);}
