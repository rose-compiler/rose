/* Jeremiah reports that if you run inlineEverything on the following code:

int f() {return 3, 4;}
void g() {int a = f();}

the unparsed output is not legal C++ code, because no parentheses are placed 
around the expression 3,4 in the inlined definition of f(). Changing the code 
slightly can lead to many other kinds of errors (duplicate variable definitions, 
various kinds of syntax errors in the declaration, incorrect behavior because 
the unparsed code has different scoping behavior than the original).  If you 
write the inlined declaration by hand, you need to add the parentheses explicitly, 
and ROSE will preserve them in its output.  Creating them in the AST, however, 
leads to the unparsing bug.
-- Jeremiah Willcock 

This generates:

int f() {return 3, 4;}
void g() {int a = (3,4);}

*/

int f() {return 3, 4;}
void g() {int a = f();}

int a = (2,3);
int b((2,3));
int x[2] = {1,(2,3) };

class X
   {
     public:
          X & operator++();
          X & operator--();
          X & operator++(int);
          X & operator--(int);
          X & operator=(const X & x);
          X (const X & x);
          X ();
   };

void foo()
   {
     int a,b,c,d,e,f,g;
     (a=b<c)?d=e:f=g;

     a+++1;

     b = (++a)--;
     b = (--a)++;

     X x1,x2;
     x1 = ++x2--;

     int* p = &a;
     *p++;

     x[a+b];
     (a+b)[x];
     (x+1)[a+b];
     (a+b)[x+1];
   }




