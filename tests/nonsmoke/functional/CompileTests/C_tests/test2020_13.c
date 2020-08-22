// Must be in the if statement
// Must have two uses of the pattern
// Pattern is "( ( union { int v; } ) {} ).v"
//   - originaly: "( ( union { typeof(x) v; int w; } ) { .v=x } ).w )"
//   - it was also using "__extension__ ()" around each instance of the pattern

void foo() {
  if ( ( ( union { int v; } ) {} ).v || ( ( union { int v; } ) {} ).v );
}


