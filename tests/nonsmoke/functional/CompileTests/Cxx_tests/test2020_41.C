// RC-60:  (from Kripke)

typedef struct x_t * X;

void bar(X v);

void foo() 
   {
     bar(X());
   }

