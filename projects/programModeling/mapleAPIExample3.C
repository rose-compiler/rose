#include <math.h>
#include "maplec.h"
  
   ALGEB M_DECL NewtonsMethod( MKernelVector kv, ALGEB *args )
   {
       M_INT i;
       FLOAT64 guess[2], tolerance;
       ALGEB f, fprime, x;
  
       if( 3 != MapleNumArgs(kv, (ALGEB)args) ) {
           MapleRaiseError(kv, "three arguments expected");
           return( NULL );
       }
  
       /* Make sure the first arg (f) is a Maple procedure */
       if( IsMapleProcedure(kv, args[1]) ) {
           f = args[1];
       }
       else {
           ALGEB indets;
           indets = EvalMapleProc(kv, ToMapleName(kv, "indets", TRUE), 1, args[1]);
           if( !IsMapleSet(kv, indets) || MapleNumArgs(kv, indets) != 1 ) {
               MapleRaiseError(kv, "unable to find roots");
               return( NULL );
           }
           i = 1;
           f = EvalMapleProc(kv, ToMapleName(kv, "unapply", TRUE), 2, args[1],
                        MapleSelectIndexed(kv, indets, 1, &i));
           if( !f || !IsMapleProcedure(kv, f) ) {
               MapleRaiseError(kv, "unable to convert first arg to a procedure");
               return( NULL );
           }
       }
  
       /* Compute the derivative of f symbolically */
       x = ToMapleName(kv, "x", FALSE);
       fprime = EvalMapleProc(kv, ToMapleName(kv, "unapply", TRUE), 2,
                   EvalMapleProc(kv, ToMapleName(kv, "diff", TRUE), 2,
                       ToMapleFunction(kv, f, 1, x), x), x);
       if( !fprime || !IsMapleProcedure(kv, fprime) ) {
           MapleRaiseError(kv, "unable to compute derivative");
           return( NULL );
       }
  
       guess[1] = MapleEvalhf(kv, args[2]);
       tolerance = MapleEvalhf(kv, args[3]);
  
       /* Find a root (abort after 500 iterations) */
       for( i=0; i<500; ++i ) {
           /* Check whether current guess is close enough */
           if( fabs(EvalhfMapleProc(kv, f, 1, guess)) <= tolerance )
               break;
  
           /* Apply Newton iteration x - f(x)/f'(x) */
           guess[1] = guess[1] - EvalhfMapleProc(kv, f, 1, guess) /
                       EvalhfMapleProc(kv, fprime, 1, guess);
       }
  
       if( i == 500 ) {
           MapleRaiseError(kv, "unable to find root after 500 iterations");
           return( NULL );
       }
  
       return( ToMapleFloat(kv, guess[1]) );
   }

int main( int argc, char *argv[] )
   {
       char err[2048];  /* Command input and error string buffers */
       MKernelVector kv;  /* Maple kernel vector - needed for all calls to Maple  */
       MCallBackVectorDesc cb = {  textCallBack,
                                   0,   /* errorCallBack not used */
                                   0,   /* statusCallBack not used */
                                   0,   /* readLineCallBack not used */
                                   0,   /* redirectCallBack not used */
                                   0,   /* streamCallBack not used */
                                   0,   /* queryInterrupt not used */
                                   0    /* callBackCallBack not used */
                               };
       ALGEB f, root;  /* Maple data-structures */
       double x0, tolerance;
                             
       /* Initialize Maple */
       if( ( kv=StartMaple(argc,argv,&cb,NULL,NULL,err )) == NULL ) {
           printf( "Error starting Maple, %s\n",err );
           return( 1 );
       }
                                                                                            
       f = EvalMapleStatement( kv, "x^4-1" ); /* Define f as the Maple expression x^4-1  */
       x0 = 2.0;
       tolerance = .001;
                        
       root = NewtonsMethod( kv, f, x0, tolerance );
                                                     
       /* display the result */
       if( r ) {
   MapleALGEB_Printf(kv,"Root found is x=%a \n", root);
       }
                                                    
       return( 0 );
}
