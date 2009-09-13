/*

Dan:
	Thanks. I'm not sure I've explained the problem clearly. The
issue is that in ROSE 0.8.0b the GNU_EXTENSIONS_ALLOWED macro within
EDG is turned off.  I turned it on but then ROSE would not compile.

../../../../ROSE-0.8.0b/src/frontend/EDG_SAGE_Connection/sage_gen_be.C:73:2: #error "GNU_EXTENSIONS_ALLOWED is not currently defined in SAGE III (later)!" 

	I have a copy of EDG (3.3) separate from ROSE that I obtained early
in my work before we knew about ROSE.  Setting the GNU_EXTENSIONS_ALLOWED
macro to TRUE (and using the --gcc command line option) in that code allows EDG 
(again, separate from ROSE, via the eccp command) to parse the following 
source files without error.

	So I dont think this is a case of extending EDG, but allowing
the EDG that is used with ROSE to enable the GNU_EXTENSIONS_ALLOWED macro.

thanks and sorry for the confusion.
chadd


TEST CASES:

Here is the command I use to run rose over each of these files:
mytool -rose:C --edg:c99 --edg:extended_variadic_macros -c filename.c

Here are each of the files I have been using as test cases, all are
derived from source code I have seen in either Wine (winehq.com)
or Apaches httpd.

Each of these files compile with gcc 3.3.3
*/

// DQ (1/8/2009): This was commented out, uncommented it so it can be tested with the new EDG/SageIII front-end.
#if 1
/*********asm.c*****************/
static int inb( int port )
   {
     int b;

  // DQ (1/10/2009): Changed "inb" to "in" for at least the 64bit case.
  // __asm__ __volatile__ ( "inb %w1,%0" : "=a" (b) : "d" (port) );
     __asm__ __volatile__ ( "in %w1,%0" : "=a" (b) : "d" (port) );

     return b;
   }

/******FUNCTION.c********************/
#define xxx(len,wanted) CLIENT_ProtocolError( __FUNCTION__ ": len %d != %d\n", (len), (wanted) );

extern void CLIENT_ProtocolError( const char *err, ... );

/******Macro.c************/
#include <stdio.h>
#define macroMacro(x,y) printf(" TEST  %d %d %s" ,x,y, __FUNCTION__);

int bar(int x, int z){

	return macroMacro(x,z);
}
/**********stdcall.c****************/
void __attribute__((__stdcall__)) foo(){
}

/*********typeof.c*****************/
#include <stdio.h>

int x;

typeof(x)  y;


main(){

	x=100;
	y=x;

	printf("%d %d\n", x,y);
}
/*******volatile.c********************/
__volatile__ int i;

/*******zeroarray.c*************/
#ifdef ROSE_USE_NEW_EDG_INTERFACE
// DQ (1/11/2009): both the old and new interface appear to generate the correct (same) code 
// for a zero length array, but it only compiles with the backend compiler when using the 
// new interface (even when it is the same backend comiler).
// Example of zero length array declaration: char XXX[0];

char XXX[0];
#endif
/******************************/

#endif


