/* This is a copy of in14.c */
#ifdef NOTDEF
#define SHARED /**/
#else
#define SHARED shared
#endif
typedef struct FluorShell_s {
    int nLine;
    double eThresh;
    double MultSh;
    SHARED double *eLine;    /* [nLine] */
    SHARED double *cpLine;  /* [nLine] */
    SHARED double *pShell;  /* [maxEG] */
} FluorShell;

typedef struct FluorData_s {
    int nShell;
    SHARED struct FluorShell_s *FlShell;  /* [nShell]  */
} FluorData;

void foo(SHARED struct FluorData_s *fptr){
	SHARED struct FluorShell_s *lFlShell;
	lFlShell = fptr->FlShell;
	/* lFlShell = ((struct FluorData_s *)(((char *)fptr) + MPISMOFFSET)) -> FlShell; */
}
