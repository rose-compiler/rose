// This demonstrates a bug in ROSE having to do with function declarations
// If the include file is used to hold the code if-def'd below then
// the error is demonstrated.  Else the code works fine!

// This was a bug independent of the use of extern C
extern "C" {
typedef struct {
    int            id;          /* Identifier for this object */
} DBcurve;

typedef struct {
    int            id;          /*identifier of the compound array */
} DBcompoundarray;

typedef struct {
    int            id;          /*identifier of the compound array */
} *XYZ;

extern DBcompoundarray *DBAllocCompoundarray(void);
extern DBcurve *DBAllocCurve(void);
extern XYZ *functionReturningXYZ(void);
}

// This pointer initialization better demonstrates the error
// caused by the function return type being output incorrectly.
DBcompoundarray *pointer = DBAllocCompoundarray();

extern "C" { extern DBcompoundarray *DBAllocCompoundarray(void); }
extern "C" { extern DBcurve *DBAllocCurve(void); }

// If this is output as: 
//      extern "C" { extern XYZ *functionReturningXYZ(void); } 
// then it is an error because the source position is not correct by column number.
extern "C" { extern XYZ *functionReturningXYZ(void); }

