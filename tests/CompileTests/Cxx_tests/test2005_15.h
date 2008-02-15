extern "C" {

typedef struct {
    int            id;          /* Identifier for this object */
} DBcurve;

typedef struct {
    int            id;          /*identifier of the compound array */
} DBcompoundarray;

extern DBcompoundarray *DBAllocCompoundarray(void);
extern DBcurve *DBAllocCurve(void);

}
