// Bug from Rich: I'm guessing the following is fixed in your to-be-checked-in version of ROSE, 
// but just so you know, this code snippet caused an assertion failure on line 19364 of 
// src/frontend/EDG_SAGE_Connection/sage_gen_be.C in ROSE 0.8.4c (latest in CVS):

class X
{
public:
 X () {}  // if this constructor is commented out, everything is OK
};

void
foo (void)
{
 X* y = new X[10];
}
