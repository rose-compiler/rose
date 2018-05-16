static	int var = 11; /* "lex 1st decl w file scope" says "static" --> internal */
extern int var;	      /* same linkage as previous --> internal */

void main()
   {
  /* BUG: This is unparsed as: "static int var;" */
     extern int var;	/* internal linkage (already defined) */
   }
