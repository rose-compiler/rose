#if 0
// The following (strange, but legal) code, from <URL:http://www.cs.utah.edu/classes/cs7968/lectures/cil.pdf>, 
// is represented in the ROSE AST (and thus unparsed) without any of the dereference operators.  This is fine 
// in terms of the meaning of the program, but doesn't exactly represent the actual source code.  This probably 
// doesn't need to be fixed, but it's good to know about.

int f(void) {
  return (*******f)();
}

-- Jeremiah Willcock 
#endif



// This appears to be a normalization (in EDG first and then not represented in ROSE).
int f(void) {
  return (*******f)();
}
