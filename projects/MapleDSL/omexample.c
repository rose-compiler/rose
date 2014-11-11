/* ***********************************************************************
 * OpenMaple Example Program
 *
 * Copyright (c) Maplesoft, a division of Waterloo Maple Inc. 2004.
 * You are permitted to copy, modify and distribute this code, as long as
 * this copyright notice is prominently included and remains intact. If any
 * modifications were done, a prominent notice of the fact that the code has
 * been modified, as well as a list of the modifications, must also be
 * included. To the maximum extent permitted by applicable laws, this
 * material is provided "as is" without any warranty or condition of any kind.
 *
 * This example program illustrates how to use the OpenMaple API
 * to initialize the Maple kernel, evaluate expressions, access help, 
 * and interrupt computations.  Users are encouraged to use and modify
 * this code as a starting point for learning the OpenMaple API.  
 *
 *********************************************************************** */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>

#include "maplec.h"

#ifdef _MSC_VER
#  define CDECL __cdecl
#else
#  define CDECL
#endif

/* global variable used by queryInterrupt() */
static int Interrupted = 0;

/* interrupt signal handler: sets global variable when user hits Ctrl-C */
void CDECL catch_intr( int signo )
{
    Interrupted = TRUE;
    signal(SIGINT,catch_intr);
#ifdef _MSC_VER
    signal(SIGBREAK,catch_intr);
#endif
}

/* interrupt callback: stops computation when `Interrupted' is true */
static M_BOOL M_DECL queryInterrupt( void *data )
{
    if( Interrupted ) {
	Interrupted = 0;
  	return( TRUE );
    }
    return( FALSE );
}

/* callback used for directing help output */
static M_BOOL M_DECL writeHelpChar( void *data, int c )
{
    putchar(c);
    return( FALSE );
}

/* callback used for directing result output */
static void M_DECL textCallBack( void *data, int tag, char *output )
{
    printf("%s\n",output);
}

/* simple program to print a prompt, get input, evaluate it, 
   and display results 
*/
int main( int argc, char *argv[] )
{
    char expr[1000], err[2048];  /* command input and error string buffers */
    MKernelVector kv;  /* Maple kernel handle */
    MCallBackVectorDesc cb = {  textCallBack, 
				0,   /* errorCallBack not used */
				0,   /* statusCallBack not used */
				0,   /* readLineCallBack not used */
				0,   /* redirectCallBack not used */
				0,   /* streamCallBack not used */
			        queryInterrupt, 
				0    /* callBackCallBack not used */
			    };
    ALGEB dag;  /* eval result (Maple data-structure) */
    int len;

    /* initialize Maple */
    if( (kv=StartMaple(argc,argv,&cb,NULL,NULL,err)) == NULL ) {
	printf("Fatal error, %s\n",err);
	return( 1 );
    }
 
    /* catch ^C */
    signal(SIGINT,catch_intr);

    printf("    |\\^/|     Maple (Example Program)\n");
    printf("._|\\|   |/|_. Copyright (c) Maplesoft, a division of Waterloo Maple Inc. 2004\n");
    printf(" \\OPENMAPLE/  All rights reserved. Maple and OpenMaple are trademarks of\n");
    printf(" <____ ____>  Waterloo Maple Inc.\n");
    printf("      |       Type ? for help.\n");

    /* Print a prompt, get a Maple expression, evaluate it,
       print a prompt, get a Maple expression, evaluate it, ... */
    for( ;; ) {
        printf("> ");
        if( !fgets(expr,sizeof(expr),stdin) ) break;

        /* Strip off trailing whitespace (including CR and/or LF). */
        for( len = strlen(expr); len > 0 && isspace(expr[len-1]); --len )
            ;
        expr[len] = '\0';

	if( expr[0] == '?' ) {
	    MapleHelp(kv,expr+1,NULL,writeHelpChar,NULL,80,NULL);
	}
	else {
	    dag = EvalMapleStatement(kv,expr);
	    if( dag && IsMapleStop(kv,dag) ) 
		break;
	}
    }

    return( 0 );
}
