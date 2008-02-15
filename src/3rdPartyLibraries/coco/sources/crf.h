#ifndef CRF_MODULE
#define CRF_MODULE

void GenCode(FILE *out, char *fmt, ...);
void SymCharName(unsigned char c, char *asciiname);

FILE *OpenFile(char *name, char *opt, int GenError);

void InitFrameVars(void);
void SetupFrameVars(void);
void GenScanner(void);
void GenParser(void);
void GenHeaders(void);
void GenCompiler(void);

extern char compiler_name[];
extern char module_prefix[];
extern char source_name[];

#if !(__MSDOS__ || MSDOS || WIN32 || __WIN32__)
#ifndef stricmp
#define stricmp strcasecmp
#endif
#if 0
int stricmp(char *s1,char *s2);
#endif
#endif

#endif


