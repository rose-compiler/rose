#if 1
void snafu_1 (int x);
int a;
void snafu_1 (int x = 1);
#endif

#if 1
void snafu_2 (char *label);
char* b;
void snafu_2 (char *label = b);
#endif

void snafu_3 (char *label);
void snafu_3 (char *label = "abc");

#if 1
void snafu_4 (float x);
float c;
void snafu_4 (float x = 1.0);
#endif

#if 1
void snafu_5 (float *x);
float d;
void snafu_5 (float *x = &d);
#endif

#if 1
void snafu_6 (float *x);
void snafu_6 (float *x = 0L);
#endif

#if 1
void foobar (const char *label);
void foobar (const char *label = "");
// void foobar (const char *label);

// Here the defining declaration can't use the default parameter since it was specified in the function prototype.
void foobar (const char *label)
   {
   }
#endif

#if 1
// Here the default parameter is a part of the defining function declaration (because there was no prototype).
void foo (const char *label = "")
   {
   }
#endif

#if 1
class X
   {
     public:
          void foo( int x, const char *label = "" );
   };

// ROSE gets this case correct, likely because the defining declaration outside of the class must rely on
// a previously declared member function declaration where the default parameter must have been used.
void X::foo ( int x, const char *label )
   {
   }
#endif

