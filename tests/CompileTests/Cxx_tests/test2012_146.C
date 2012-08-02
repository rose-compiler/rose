// This demonstrates a bug in ROSE having to do with function declarations
// If the include file is used to hold the code if-def'd below then
// the error is demonstrated.  Else the code works fine!

// This was a bug independent of the use of extern C
extern "C" {

#if 1
typedef struct A {
    int            id;          /*identifier of the compound array */
} *XYZ;
#endif

// extern XYZ *functionReturningXYZ(void);
}

// If this is output as: 
//      extern "C" { extern XYZ *functionReturningXYZ(void); } 
// then it is an error because the source position is not correct by column number.
#if 1
extern "C" { extern XYZ *functionReturningXYZ(void); }
#else
extern "C" { extern int *functionReturningXYZ(void); }
#endif


