// This demonstrates a bug in ROSE having to do with function declarations
// If the include file is used to hold the code if-def'd below then
// the error is demonstrated.  Else the code works fine!

// This was a bug independent of the use of extern C
extern "C" {

// ROSE will compute the column numbers correctly here.
extern int *functionReturningXYZ(void);
}

// Column number for this is wrong in ROSE (it has the column numbers of the first declaration).
extern "C" { extern int *functionReturningXYZ(void); }
