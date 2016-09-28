// This demonstrates a bug in ROSE having to do with function declarations
// If the include file is used to hold the code if-def'd below then
// the error is demonstrated.  Else the code works fine!

// This was a bug independent of the use of extern C
extern "C" {

// If this is given a name (as in "typedef struct ABC {int id;} *XYZ;")
// Then the last statement need not appear in the ectern "C" block.
// Now that I have fixed the handling of the un-named structs, this code 
// is not a bug (because the extern "C" block was likely never working
// properly to encolse the last statement.  This is because the 
// comments and CPP directives (which also handled the extern "C" directive
// has likely never worked properly.  So tomorrow I have to fix
// the comment/cpp directive weaving to use column numbers correctly
// (and see if I can fix the source position information in the secondary
// declaration of functionReturningXYZ).
typedef struct {
    int id;          /*identifier of the compound array */
} *XYZ;

// ROSE will compute the column numbers correctly here.
extern XYZ *functionReturningXYZ(void);
}

// Column number for this is wrong in ROSE (it has the column numbers of the first declaration).
extern "C" { extern XYZ *functionReturningXYZ(void); }



