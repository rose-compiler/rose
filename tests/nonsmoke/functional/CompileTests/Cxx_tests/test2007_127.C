// This demonstrates a problem with constant folding.

// The x_boolalpha on the last line is in the AST as an integer value with an original
// expression tree child (the variable reference to x_boolalpha); this is correct.
// However, the parent of the variable reference is set to the SgCastExp (implicit in the
// function call), not the SgIntVal which is its actual parent.  This incorrectly set
// parent causes problems with the control flow graph implementation.  I would appreciate
// you looking in to this to see if it is in the current version of ROSE.  Thank you,


enum _Ios_Fmtflags {_S_boolalpha};
typedef _Ios_Fmtflags fmtflags;
const fmtflags x_boolalpha =  _S_boolalpha;
void setf(fmtflags __fmtfl) {}
void boolalpha() {setf(x_boolalpha);}
