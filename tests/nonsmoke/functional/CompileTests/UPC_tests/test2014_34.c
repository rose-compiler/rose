/* This is the in11.c example */
typedef struct FluorShell_s {
    int nLine;
    double eThresh;
    double MultSh;
    shared double *eLine;    /* [nLine] */
    shared double *cpLine;  /* [nLine] */
    shared double *pShell;  /* [maxEG] */
} FluorShell;

typedef struct FluorData_s {
    int nShell;
    shared struct FluorShell_s *FlShell;  /* [nShell]  */
} FluorData;

void foo(shared struct FluorData_s *fptr){
	double d;
	d = fptr->FlShell->MultSh;
	d += 1;
}
