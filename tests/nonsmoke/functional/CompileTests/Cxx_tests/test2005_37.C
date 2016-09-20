
// #define PD_inquire_type(file, name) (defstr *) SC_def_lookup(name, file->chart_hashed)
//   if (PD_inquire_type(fp,const_cast<char*>(expectedType)))

//   if ((SC_def_lookup((expectedType),fp -> chart_hashed)))

// #define PD_inquire_type(file, name) (defstr *) SC_def_lookup(name, file->chart_hashed)
#define PD_inquire_type(file, name) SC_def_lookup(name, file)

int SC_def_lookup(void*, int);

void foo(const char *expectedType)
   {
  // if (PD_inquire_type(fp,const_cast<char*>(expectedType)))
  // if (SC_def_lookup(const_cast<char*>(expectedType),fp->chart_hashed)))

     int fp = 0;
  // if (PD_inquire_type(fp,const_cast<char*>(expectedType)))
  // unparses to: if (SC_def_lookup((expectedType),fp))
     if (PD_inquire_type(fp,((char*)expectedType)))
        {
        }
   }


// (int *[3])&x" becomes "x", not "&x". (See test2005_106.C)

class X
   {
     public:
          char* array[10];
          void foo ( const char* c ) const;
   };

int main()
   {
     X x;

  // Generates warning: "Warning: compiler generated cast not explicit in Sage translation (skipped)"
     x.foo (x.array [0]);
   }

#if 0
void foobar()
   {
     int *a;
     int x[3];

  // EDG presents the case "(int *[3])&x" and that 
  // it would be nice if this becomes "x", not "&x".
  // the EDG/Sage III translation appears to track this case 
  // "is_array_decay_cast(expr)" but I can't seem to generate
  // source code that will demonstrate it.  The example below 
  // is close to what we want and is translated to multiple casts 
  // (SgCastExp objects) as one might expect (non-normalized form)
  // The unparser then outputs each cast operator as expected.
  // The unparsed codes appears as: "a = ((int *)(&x));"
     a = (int *)&x;
   }
#endif
