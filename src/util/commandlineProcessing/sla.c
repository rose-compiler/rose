/*

String list assignment (sla) functions to process assignment instructions and flags.
Copyright (C) 1998,1999 Brian T. N. Gunney

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA  02111-1307, USA.

Brian T. N. Gunney, brlynn@minn.net

$Id: sla.c,v 1.4 2008/01/08 02:56:44 dquinlan Exp $



These functions currently use POSIX regex functions.
Code using GNU regex functions and BSD regex functions
are not written yet.

Arguments:
  argc(io), argv(io): Count and string variable, usually arguments of main.
  flind(i): String of flag indicator character, usually "-".
  assop(i): String of assignment characters, usually "=".
	This string sits between the parameter name and its value.
	Set assop to "$" to specify no value.
	Set assop to " " to specify detatched value.
	Any other assop (such as "=") specifies attached value.
	Set assop to "" to specify attached value without delimiter
	(this may create ambiguities for which the user must accept
	responsibility).
  pname(i): Parameter name.  Precede with '*' if it is to be a list
  value(io): Value(s) to assign to the parameter.
    Points to an array of strings.
    value must have enough room to accept all values to be stored
    (a safe guess is argc).  If pname does not indicate a list,
    only one item (the last one) will be assigned to value.
    If value is set to NULL, sla will NOT set its values
    and also NOT remove the corresponding argv, so that only a count of
    the places needed is returned.  When assop is "$" (end of string),
    there is no assignment to value (of course) and value is completely ignored,
    even if it is not NULL (it can be any pointer, even wild).
  
Return: The number of value found.  If pname is not a list, the number of
  value found is still returned, but only the last value is used.
  On error, returns < 0.

*/

#include <stdio.h>
#ifdef __cplusplus
#include <iostream.h>
#include <iomanip.h>
#endif

#include <strings.h>
#include <unistd.h>

#if defined(J90)/*Cray*/ || defined(C90)/*Cray*/ || defined(IP)/*SGI*/
#include <sys/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
#include <regex.h>
#ifdef __cplusplus
}
#endif

#include "sla.h"

// #ifdef HAVE_CONFIG_H
#include "rose_config.h"
// #endif




static int debug=0;
void sla_set_debug( int d ) {
  debug = d;
  printf( "SLA debug is set to %d\n", debug );
}

#include <stdlib.h>
#ifdef __cplusplus
#define NEW_MEMORY(T,N) new T [N]
#define DEL_MEMORY(P) delete [] P
#else
#define NEW_MEMORY(T,N) (T*)malloc( (N)*sizeof(T) )
#define DEL_MEMORY(P) free(P)
#endif



SLA_FCN_BEGIN( sla_str, ARGVCONST char *) {

#ifdef SLA_DEBUG
#ifdef __cplusplus
  if ( debug >= 16 )
    cout << "Entered sla."
	 << '\n' << "  argc:  " << *argc
	 << '\n' << "  argv:  " << argv
	 << '\n' << "  flind: " << flind
	 << '\n' << "  assop: " << assop
	 << '\n' << "  pname: " << pname
	 << '\n' << "  value:   " << value
	 << '\n' << "  argd:  " << argd
	 << endl;
#endif
#endif
  register int i, j, k;
  int nvalue=0;
  const char *cptr;
  regex_t flind_re, pname_re, assop_re, sup_assop_re;
  regmatch_t pm;
  int errf;
  int prefer_detached_over_absent = 0;
  register unsigned long n_pname_exp;
  char *pname_exp;
  register unsigned long n_assop_exp;
  char *assop_exp;
  register unsigned long nsup_assop_exp;
  char *sup_assop_exp;
  int assopmatchlength;
  ARGVCONST char *attachedargbegin;
  char *tstr;
  int assop_ends_str;

#ifdef SLA_DEBUG
#ifdef __cplusplus
  if ( debug >= 16 ) cout << "Forming search string." << endl;
#endif
#endif

  /* form flind_exp one piece at a time */
  register unsigned long n_flind_exp;
  char *flind_exp;
  n_flind_exp = strlen( (void*) flind) + 10;
  flind_exp = NEW_MEMORY(char,n_flind_exp);
  n_flind_exp = 0;
  flind_exp[n_flind_exp++] = '^';
  cptr = flind; while ( *cptr ) flind_exp[n_flind_exp++] = *(cptr++);
  flind_exp[n_flind_exp] = '\0';
#ifdef SLA_DEBUG
#ifdef __cplusplus
  if ( debug >= 2 ) cout << "flind expression: '" << flind_exp << "'" << endl;
#endif
#endif
  /* compile regular expression flind_re for flind */
  errf = regcomp(&flind_re, flind_exp, REG_EXTENDED);
  if (errf != 0) {
    char buf[256];
    (void)regerror(errf,&flind_re,buf,sizeof(buf));
#ifdef __cplusplus
    cout << buf << endl;
#endif
    return(0); /* report error */
  }

  /* form pname_exp one piece at a time */
  n_pname_exp = strlen( (void*)pname) + 10;
  pname_exp = NEW_MEMORY(char,n_pname_exp);
  n_pname_exp = 0;
  pname_exp[n_pname_exp++] = '^';
  cptr = pname + (*pname=='*'); while ( *cptr ) pname_exp[n_pname_exp++] = *(cptr++);
  pname_exp[n_pname_exp] = '\0';
#ifdef SLA_DEBUG
#ifdef __cplusplus
  if ( debug >= 2 ) cout << "pname expression: '" << pname_exp << "'" << endl;
#endif
#endif
  /* compile regular expression flind_re for flind */
  errf = regcomp(&pname_re, pname_exp, REG_EXTENDED);
  if (errf != 0) {
    char buf[256];
    (void)regerror(errf,&pname_re,buf,sizeof(buf));
#ifdef __cplusplus
    cout << buf << endl;
#endif
    return(0); /* report error */
  }

  /* form assop_exp one piece at a time */
  n_assop_exp = strlen( (void*)assop) + 10;
  assop_exp = NEW_MEMORY(char,n_assop_exp);
  n_assop_exp = 0;
  assop_exp[n_assop_exp++] = '^';
  cptr = assop; while ( *cptr ) assop_exp[n_assop_exp++] = *(cptr++);
  if ( assop_exp[n_assop_exp-1] == '^' ) {
    prefer_detached_over_absent = 1;
    assop_exp[--n_assop_exp] = '\0';
  }
  else assop_exp[n_assop_exp] = '\0';
#ifdef SLA_DEBUG
#ifdef __cplusplus
  if ( debug >= 2 ) cout << "assop expression: '" << assop_exp << "'" << endl;
#endif
#endif
  /* compile regular expression flind_re for flind */
  errf = regcomp(&assop_re, assop_exp, REG_EXTENDED);
  if (errf != 0) {
    char buf[256];
    (void)regerror(errf,&assop_re,buf,sizeof(buf));
#ifdef __cplusplus
    cout << buf << endl;
#endif
    return(0); /* report error */
  }

  /* form sup_assop_exp one piece at a time */
  nsup_assop_exp = strlen( (void*)assop) + 10;
  sup_assop_exp = NEW_MEMORY(char,nsup_assop_exp);
  nsup_assop_exp = 0;
  sup_assop_exp[nsup_assop_exp++] = '^';
  cptr = assop; while ( *cptr ) sup_assop_exp[nsup_assop_exp++] = *(cptr++);
  if ( sup_assop_exp[nsup_assop_exp-1] == '^' ) {
    nsup_assop_exp--;
  }
  sup_assop_exp[nsup_assop_exp++] = 'x';
  sup_assop_exp[nsup_assop_exp] = '\0';
#ifdef SLA_DEBUG
#ifdef __cplusplus
  if ( debug >= 2 ) cout << "sup_assop expression: '" << sup_assop_exp << "'" << endl;
#endif
#endif
  /* compile regular expression flind_re for flind */
  errf = regcomp(&sup_assop_re, sup_assop_exp, REG_EXTENDED);
  if (errf != 0) {
    char buf[256];
    (void)regerror(errf,&sup_assop_re,buf,sizeof(buf));
#ifdef __cplusplus
    cout << buf << endl;
#endif
    return(0); /* report error */
  }


  /* Delete found assignment instructions or not. */
  /* argd  > 0 -> delete.
     argd == 0 -> chose automatically.
     argd  < 0 -> don't delete.
  */
  if ( argd == 0 ) {	/* Determine automatically. */
    if ( *assop=='$' && *(assop+1)=='\0' ) argd = 1;	/* Delete any found. */
    else argd = value != NULL ? 1 : -1;	/* Delete as assigned to value. */
  }
#ifdef SLA_DEBUG
#ifdef __cplusplus
  if ( debug >= 16 ) {
    cout << "Variable argd is " << argd << "\n";
  }
  if ( debug >= 4 ) {
    cout << "Found assignment instructions will " << (argd>0?"":"NOT ") << "be deleted\n";
  }
#endif
#endif


  for ( i=0; i<*argc; i++ ) {

#ifdef SLA_DEBUG
#ifdef __cplusplus
    if ( debug >= 4 ) cout << "trying: argv["<<i<<"] = '" << argv[i] << "'" << endl;
#endif
#endif

    int offset;

    offset = 0;

    pm.rm_so = pm.rm_eo = -1;
    errf = regexec(&flind_re, argv[i]+offset, (size_t)1, &pm, 0);
    if ( errf == REG_NOMATCH ) continue; /* no match */
    if ( errf == REG_ESPACE ) { /* error */
#ifdef __cplusplus
      cerr << "**** Out of memory in regexec: " << __LINE__ << " of " << __FILE__;
#endif
      return -1;
    }
    offset += pm.rm_eo;

    pm.rm_so = pm.rm_eo = -1;
    errf = regexec(&pname_re, argv[i]+offset, (size_t)1, &pm, 0);
    if ( errf == REG_NOMATCH ) continue; /* no match */
    if ( errf == REG_ESPACE ) { /* error */
#ifdef __cplusplus
      cerr << "**** Out of memory in regexec: " << __LINE__ << " of " << __FILE__;
#endif
      return -1;
    }
    offset += pm.rm_eo;

    pm.rm_so = pm.rm_eo = -1;
    errf = regexec(&assop_re, argv[i]+offset, (size_t)1, &pm, 0);
    if ( errf == REG_NOMATCH ) continue; /* no match */
    if ( errf == REG_ESPACE ) { /* error */
#ifdef __cplusplus
      cerr << "**** Out of memory in regexec: " << __LINE__ << " of " << __FILE__;
#endif
      return -1;
    }
    assopmatchlength = pm.rm_eo - pm.rm_so;
#ifdef SLA_DEBUG
#ifdef __cplusplus
    if ( debug >= 4 ) cout << "assop match length is " << assopmatchlength << endl;
#endif
#endif
    attachedargbegin = argv[i]+offset+pm.rm_eo;

    /*** did assop match and end-of-string? ***/
    /* There must be a better way to check this. */
    tstr = NEW_MEMORY(char,assopmatchlength+2);
    for ( k=0; k<assopmatchlength; k++ ) tstr[k] = *(argv[i]+offset+k);
    tstr[k] = 'x'; tstr[k+1] = '\0';
    pm.rm_so = pm.rm_eo = -1;
    errf = regexec(&sup_assop_re, tstr, (size_t)1, &pm, 0);
    if ( errf == REG_ESPACE ) { /* error */
#ifdef __cplusplus
      cerr << "**** Out of memory in regexec: " << __LINE__ << " of " << __FILE__;
#endif
      return -1;
    }
    assop_ends_str = ( errf == REG_NOMATCH );
#ifdef SLA_DEBUG
#ifdef __cplusplus
    if ( debug >= 4 ) cout << "assop ends str is " << assop_ends_str << endl;
#endif
#endif
    DEL_MEMORY(tstr);

#ifdef SLA_DEBUG
#ifdef __cplusplus
    if ( debug >= 4 ) cout << "matched with " << argv[i] << endl;
#endif
#endif


#ifdef SLA_DEBUG
#ifdef __cplusplus
    if ( debug >= 4 ) {
      if ( !assop_ends_str ) {	/* attached or no value */
	cout << "using ATtached value '" << attachedargbegin << "'" << endl;
      }
      else {	/* detached value */
	cout << "using absent or DEtached value '" << argv[i+1] << "'" << endl;
      }
    }
#endif
#endif

    
    /* The appropriate assignment instructions has been found.
       Take appropriate assignment and delete actions.
    */

    /*** Assignment actions. ***/
    if ( value ) {
      if ( assop_ends_str ) {	/* either detached value or no value */
	if ( assopmatchlength	/* assop$ case */
	  || prefer_detached_over_absent	/* special case */
	  ) {			/* detached value */
	  value[*pname == '*' ? nvalue : 0] = argv[i+1];
	}
      }
      else {	/* attached value (even if assop is "") */
	value[*pname == '*' ? nvalue : 0] = attachedargbegin;
      }
    }

    /*** Delete actions ***/
    if ( assop_ends_str ) {	/* either detached value or no value */
      if ( assopmatchlength	/* assop$ case */
	|| prefer_detached_over_absent	/* special case */
	) {			/* detached value */
	if ( ( argd > 0 ) || ( ( argd == 0 ) && value ) ) {
	  /* Delete argv[i] and argv[i+1] from list. */
#ifdef SLA_DEBUG
#ifdef __cplusplus
	  if ( debug >= 4 )
	    cout << "Deleting " << i << ':' << argv[i]
		 << " and " << i+1 << ':' << argv[i+1] << " from argv" << endl;
#endif
#endif
	  (*argc) -= 2;
	  for ( j=i; j<(*argc); j++ ) argv[j] = argv[j+2];
	  i--;	/* Cause the new argv[i] to be processed. */
	}
	else i++;	/* skip next argv because it is an value. */
      }
      else {	/* no value */
	if ( argd >= 0 ) {
	  /* Delete argv[i] from list. */
#ifdef SLA_DEBUG
#ifdef __cplusplus
	  if ( debug >= 4 )
	    cout << "Deleting " << i << ':' << argv[i] << " from argv" << endl;
#endif
#endif
	  (*argc)--;
	  for ( j=i; j<(*argc); j++ ) argv[j] = argv[j+1];
	  i--;	/* Cause the new argv[i] to be processed. */
	}
      }
    }
    else {	/* attached value */
      if ( value && argd >= 0 ) {
	/* Delete argv[i] from list. */
#ifdef SLA_DEBUG
#ifdef __cplusplus
	if ( debug >= 4 )
	  cout << "Deleting " << i << ':' << argv[i] << " from argv" << endl;
#endif
#endif
	(*argc)--;
	for ( j=i; j<(*argc); j++ ) argv[j] = argv[j+1];
	i--;	/* Cause the new argv[i] to be processed. */
      }
    }


    nvalue++;	/* Count number of value found. */

#ifdef SLA_DEBUG
#ifdef __cplusplus
    if ( debug >= 8 ) for ( j=1; j<*argc; j++ )
      cout << "remaining:argv["<<j<<"]="<<argv[j] << endl;
#endif
#endif

    }

  regfree(&flind_re);
  regfree(&pname_re);
  regfree(&assop_re);
  regfree(&sup_assop_re);
  DEL_MEMORY(flind_exp);
  DEL_MEMORY(pname_exp);
  DEL_MEMORY(assop_exp);

  return nvalue;
}



#ifdef __cplusplus
/*** Sla without value (it is presumed to be zero). ***/
/* Note: this function is just a matter of convenience, not truly
   necessary.  It calls the general sla with value=(char const**)0.
   Its primary purpose is to save the calling code from having to
   specify the last argument when it is set to the NULL pointer.
   Because sla is overloaded in c++ and uses the type of value to resolve
   the call, one can't simply pass NULL as the last argument.
   */
int sla_none( int *argc, ARGVCONST char **argv, const char *flind, const char *assop, const char *pname, int argd ) {
  return sla_str( argc, argv, flind, assop, pname, (ARGVCONST char **)0, argd );
}
#endif

/*
  Generate additional sla for various primitive types.
  The use of the SLA macro makes it
  difficult to see the prototypes of the functions created.
  To find the prototypes, try this:
  > CC -P sla.cc
  > pgrep 's/^(int sla\s*\([^\{]*).*$/$1;/' sla.i -s
 */
SLA_FCN_BEGIN( sla_float, float )		{ SLA_CNV(atof) }
SLA_FCN_BEGIN( sla_double, double )		{ SLA_CNV(atof) }
SLA_FCN_BEGIN( sla_int, int )			{ SLA_CNV(atoi) }
SLA_FCN_BEGIN( sla_short, short )		{ SLA_CNV(atoi) }
SLA_FCN_BEGIN( sla_long, long )		{ SLA_CNV(atoi) }
SLA_FCN_BEGIN( sla_uint, unsigned int )	{ SLA_CNV(atoi) }
SLA_FCN_BEGIN( sla_ushort, unsigned short )	{ SLA_CNV(atoi) }
SLA_FCN_BEGIN( sla_ulong, unsigned long )	{ SLA_CNV(atoi) }
SLA_FCN_BEGIN( sla_char, char )		{ SLA_CNV(*) }


#undef NEW_MEMORY
#undef DEL_MEMORY
