/* This is the in10.c example */
typedef struct {
    int nLine;
    double eThresh;
    double MultSh;
    shared double *eLine;    /* [nLine] */
    shared double *cpLine;  /* [nLine] */
    shared double *pShell;  /* [maxEG] */
} FluorShell;

typedef struct {
    int nShell;
    shared FluorShell *FlShell;  /* [nShell]  */
} FluorData;

void foo(shared FluorData *fptr){
	double d;
	d = fptr->FlShell->MultSh;
}
