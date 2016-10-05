#if 0
extern int *pcre_compile(const char *, int, const char **, int *,
                  const unsigned char *);
#endif

// int pcre_compile(const char *, int, const char **, int *,const unsigned char *);

// This function declaration's parameters are not named, so no source position information 
// will be available in EDG.
void foobar(int,int);

// The use of this declaration is not enough in EDG to define source position information to the 
// function parameters (because the previous declaration's function parameters were not named).
void foobar(int x,int y);

// The use of this defining declaration will cause source position information to be generated 
// for the function parameters.
// void foobar(int x,int y) {}



