// Example showing use of builtin variables (in older version of C they were 
// strings, in later versions of C (in gnu 3.4.x and greater, they are variables
// instead of values)).

extern "C" {
     extern int printf (char *, ...);
}
     
class a
   {
     public:
          void sub (int i)
             {
               printf ("__FUNCTION__ = %s\n", __FUNCTION__);
               printf ("__PRETTY_FUNCTION__ = %s\n", __PRETTY_FUNCTION__);
             }
   };
     
int
main (void)
   {
     a ax;
     ax.sub (0);
     return 0;
   }

