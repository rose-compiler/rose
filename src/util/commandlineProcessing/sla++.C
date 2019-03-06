// This avoids requiring the user to use rose_config.h and follows 
// the automake manual request that we use <> instead of ""
#include <rose_config.h>
#include <assert.h>

// DQ (3/22/2009): Added MSVS support for ROSE.
#include "rose_msvc.h"


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

$Id: sla++.C,v 1.5 2008/01/25 02:25:49 dquinlan Exp $



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

/*
 *
 * Jeremiah J. Willcock, 10-16-2007
 * Changed the C++ version of this code to use std::strings rather than char*
 * to allow better integration with the string version of ROSE command line
 * processing.
 *
 */

// DQ (1/15/2007): Modern C++ compiler can handle these directly!
#include <stdio.h>
#include <iostream>
#include <iomanip>

// These are C language specific files
#include <strings.h>
#include <unistd.h>

// DQ (12/11/2007): Some versions of gnu require this (for example, gnu 4.1.2).
#include <assert.h>

// DQ (1/15/2007): This is no longer required for modern C++ compilers!
#if 0
  #include <stdio.h>
  #ifdef __cplusplus
 // DQ (2/11/2004): Changed name of header file as defined in config.h
    #include IOSTREAM_HEADER_FILE
    #include <iostream>
    #include IOMANIP_HEADER_FILE
    #include <iomanip>
  #endif

  #include <strings.h>
  #include <unistd.h>

  #if defined(J90)/*Cray*/ || defined(C90)/*Cray*/ || defined(IP)/*SGI*/
  #include <sys/types.h>
  #endif
#endif

#if (_WIN32)
 #include <boost/regex.h>
// #include <regex.h>
#else
 #include <regex.h>
#endif
#include <string.h>

// DQ (11/4/2009): This is no longer required for modern C++ compilers!
#if 0
#ifdef __cplusplus
extern "C" {
#endif
// #include <regex.h>
  /* next line added by Bobby, 10/25/2001 */
#ifdef __cplusplus
}
#endif
#endif

#include "sla.h"

// #ifdef HAVE_CONFIG_H
// #include "config.h"
// #endif




static int debug=0;
void sla_set_debug( int d ) {
  debug = d;
  printf( "SLA debug is set to %d\n", debug );
}

#ifdef __cplusplus
#define NEW_MEMORY(T,N) new T [N]
#define DEL_MEMORY(P) delete [] P
#else
#include <malloc.h>
#define NEW_MEMORY(T,N) (T*)malloc( (N)*sizeof(T) )
#define DEL_MEMORY(P) free(P)
#endif



SLA_FCN_BEGIN( sla_str, std::string ) {

#ifdef SLA_DEBUG
#ifdef __cplusplus
  if ( debug >= 16 )
    std::cout << "Entered sla."
              << '\n' << "  argc:  " << argv.size()
              // << '\n' << "  argv:  " << argv
              << '\n' << "  flind: " << flind
              << '\n' << "  assop: " << assop
              << '\n' << "  pname: " << pname
              << '\n' << "  value:   " << value
              << '\n' << "  argd:  " << argd
              << std::endl;
#endif
#endif
// Rasmussen-2019.01.25: Removed register as it is deprecated and incompatible with C++17
//                       (as reported by clang compiler)
  unsigned int i, k;
  int nvalue=0;
  regex_t flind_re, pname_re, assop_re, sup_assop_re;
  regmatch_t pm;
  int errf;
  int prefer_detached_over_absent = 0;
  unsigned int assopmatchlength;
  const char *attachedargbegin;
  char *tstr;
  int assop_ends_str;

#ifdef SLA_DEBUG
#ifdef __cplusplus
  if ( debug >= 16 ) std::cout << "Forming search string." << std::endl;
#endif
#endif

  /* form flind_exp one piece at a time */
  std::string flind_exp = "^" + flind;
#ifdef SLA_DEBUG
#ifdef __cplusplus
  if ( debug >= 2 ) std::cout << "flind expression: '" << flind_exp << "'" << std::endl;
#endif
#endif
  /* compile regular expression flind_re for flind */
  //printf(" flind_re: %d %s    address: %d \n",flind_re.re_magic, flind_re, &flind_re);
    //printf(" flind_exp: %s  \n",flind_exp.c_str());
    //printf(" REG_EXTENDED: %d \n",REG_EXTENDED);
    // errf != 0;
    //    if (flind_re.re_magic==0)
  errf = regcomp(&flind_re, flind_exp.c_str(), REG_EXTENDED);
  if (errf != 0) {
    char buf[256];
    (void)regerror(errf,&flind_re,buf,sizeof(buf));
#ifdef __cplusplus
    std::cout << buf << std::endl;
#endif
    abort(); /* report error */
  }

  /* form pname_exp one piece at a time */
  std::string pname_exp = "^" + pname.substr((!pname.empty() && pname[0] == '*'));
#ifdef SLA_DEBUG
#ifdef __cplusplus
  if ( debug >= 2 ) std::cout << "pname expression: '" << pname_exp << "'" << std::endl;
#endif
#endif
  /* compile regular expression flind_re for flind */
  errf = regcomp(&pname_re, pname_exp.c_str(), REG_EXTENDED);
  if (errf != 0) {
    char buf[256];
    (void)regerror(errf,&pname_re,buf,sizeof(buf));
#ifdef __cplusplus
    std::cout << buf << std::endl;
#endif
    abort(); /* report error */
  }

  /* form assop_exp one piece at a time */
  std::string assop_exp = "^" + assop;
  if ( assop_exp[assop_exp.size() - 1] == '^' ) {
    prefer_detached_over_absent = 1;
    assop_exp.resize(assop_exp.size() - 1);
  }
#ifdef SLA_DEBUG
#ifdef __cplusplus
  if ( debug >= 2 ) std::cout << "assop expression: '" << assop_exp << "'" << std::endl;
#endif
#endif
  /* compile regular expression flind_re for flind */
  errf = regcomp(&assop_re, assop_exp.c_str(), REG_EXTENDED);
  if (errf != 0) {
    char buf[256];
    (void)regerror(errf,&assop_re,buf,sizeof(buf));
#ifdef __cplusplus
    std::cout << buf << std::endl;
#endif
    abort(); /* report error */
  }

  /* form sup_assop_exp one piece at a time */
  std::string sup_assop_exp = "^" + assop;
  if ( sup_assop_exp[sup_assop_exp.size() - 1] == '^' ) {
    sup_assop_exp.resize(sup_assop_exp.size() - 1);
  }
  sup_assop_exp += 'x';
#ifdef SLA_DEBUG
#ifdef __cplusplus
  if ( debug >= 2 ) std::cout << "sup_assop expression: '" << sup_assop_exp << "'" << std::endl;
#endif
#endif
  /* compile regular expression flind_re for flind */
  errf = regcomp(&sup_assop_re, sup_assop_exp.c_str(), REG_EXTENDED);
  if (errf != 0) {
    char buf[256];
    (void)regerror(errf,&sup_assop_re,buf,sizeof(buf));
#ifdef __cplusplus
    std::cout << buf << std::endl;
#endif
    abort(); /* report error */
  }


  /* Delete found assignment instructions or not. */
  /* argd  > 0 -> delete.
     argd == 0 -> chose automatically.
     argd  < 0 -> don't delete.
  */
  if ( argd == 0 ) {    /* Determine automatically. */
    if ( assop == "$" ) argd = 1;       /* Delete any found. */
    else argd = value != NULL ? 1 : -1; /* Delete as assigned to value. */
  }
#ifdef SLA_DEBUG
#ifdef __cplusplus
  if ( debug >= 16 ) {
    std::cout << "Variable argd is " << argd << "\n";
  }
  if ( debug >= 4 ) {
    std::cout << "Found assignment instructions will " << (argd>0?"":"NOT ") << "be deleted\n";
  }
#endif
#endif


  for ( i=0; i<argv.size(); i++ ) {

#ifdef SLA_DEBUG
#ifdef __cplusplus
    if ( debug >= 4 ) std::cout << "trying: argv["<<i<<"] = '" << argv[i] << "'" << std::endl;
#endif
#endif

    int offset;

    offset = 0;

    pm.rm_so = pm.rm_eo = -1;
    errf = regexec(&flind_re, argv[i].c_str()+offset, (size_t)1, &pm, 0);
    if ( errf == REG_NOMATCH ) continue; /* no match */
    if ( errf == REG_ESPACE ) { /* error */
#ifdef __cplusplus
      std::cerr << "**** Out of memory in regexec: " << __LINE__ << " of " << __FILE__;
#endif
      return -1;
    }
    offset += pm.rm_eo;

    pm.rm_so = pm.rm_eo = -1;
    errf = regexec(&pname_re, argv[i].c_str()+offset, (size_t)1, &pm, 0);
    if ( errf == REG_NOMATCH ) continue; /* no match */
    if ( errf == REG_ESPACE ) { /* error */
#ifdef __cplusplus
      std::cerr << "**** Out of memory in regexec: " << __LINE__ << " of " << __FILE__;
#endif
      return -1;
    }
    offset += pm.rm_eo;

    pm.rm_so = pm.rm_eo = -1;
    errf = regexec(&assop_re, argv[i].c_str()+offset, (size_t)1, &pm, 0);
    if ( errf == REG_NOMATCH ) continue; /* no match */
    if ( errf == REG_ESPACE ) { /* error */
#ifdef __cplusplus
      std::cerr << "**** Out of memory in regexec: " << __LINE__ << " of " << __FILE__;
#endif
      return -1;
    }
    assopmatchlength = pm.rm_eo - pm.rm_so;
#ifdef SLA_DEBUG
#ifdef __cplusplus
    if ( debug >= 4 ) std::cout << "assop match length is " << assopmatchlength << std::endl;
#endif
#endif
    attachedargbegin = argv[i].c_str()+offset+pm.rm_eo;

    /*** did assop match and end-of-string? ***/
    /* There must be a better way to check this. */
    tstr = NEW_MEMORY(char,assopmatchlength+2);
    for ( k=0; k<assopmatchlength; k++ ) tstr[k] = *(argv[i].c_str()+offset+k);
    tstr[k] = 'x'; tstr[k+1] = '\0';
    pm.rm_so = pm.rm_eo = -1;
    errf = regexec(&sup_assop_re, tstr, (size_t)1, &pm, 0);
    if ( errf == REG_ESPACE ) { /* error */
#ifdef __cplusplus
      std::cerr << "**** Out of memory in regexec: " << __LINE__ << " of " << __FILE__;
#endif
      return -1;
    }
    assop_ends_str = ( errf == REG_NOMATCH );
#ifdef SLA_DEBUG
#ifdef __cplusplus
    if ( debug >= 4 ) std::cout << "assop ends str is " << assop_ends_str << std::endl;
#endif
#endif
    DEL_MEMORY(tstr);

#ifdef SLA_DEBUG
#ifdef __cplusplus
    if ( debug >= 4 ) std::cout << "matched with " << argv[i] << std::endl;
#endif
#endif


#ifdef SLA_DEBUG
#ifdef __cplusplus
    if ( debug >= 4 ) {
      if ( !assop_ends_str ) {  /* attached or no value */
        std::cout << "using ATtached value '" << attachedargbegin << "'" << std::endl;
      }
      else {    /* detached value */
        std::cout << "using absent or DEtached value '" << argv[i+1] << "'" << std::endl;
      }
    }
#endif
#endif

    
    /* The appropriate assignment instructions has been found.
       Take appropriate assignment and delete actions.
    */

    /*** Assignment actions. ***/
    if ( value ) {
      if ( assop_ends_str ) {   /* either detached value or no value */
        if ( assopmatchlength   /* assop$ case */
          || prefer_detached_over_absent        /* special case */
          ) {                   /* detached value */
      assert (i + 1 < argv.size());
      value[(!pname.empty() && pname[0] == '*') ? nvalue : 0] = argv[i+1];
        }
      }
      else {    /* attached value (even if assop is "") */
        value[(!pname.empty() && pname[0] == '*') ? nvalue : 0] = attachedargbegin;
      }
    }

    /*** Delete actions ***/
    if ( assop_ends_str ) {     /* either detached value or no value */
      if ( assopmatchlength     /* assop$ case */
        || prefer_detached_over_absent  /* special case */
        ) {                     /* detached value */
        if ( ( argd > 0 ) || ( ( argd == 0 ) && value ) ) {
          /* Delete argv[i] and argv[i+1] from list. */
#ifdef SLA_DEBUG
#ifdef __cplusplus
          if ( debug >= 4 )
            std::cout << "Deleting " << i << ':' << argv[i]
                      << " and " << i+1 << ':' << argv[i+1] << " from argv" << std::endl;
#endif
#endif
          argv.erase(argv.begin() + i, argv.begin() + i + 2);
          i -= 2;       /* Cause the new argv[i] to be processed. */
        }
        else i++;       /* skip next argv because it is an value. */
      }
      else {    /* no value */
        if ( argd >= 0 ) {
          /* Delete argv[i] from list. */
#ifdef SLA_DEBUG
#ifdef __cplusplus
          if ( debug >= 4 )
            std::cout << "Deleting " << i << ':' << argv[i] << " from argv" << std::endl;
#endif
#endif
          argv.erase(argv.begin() + i);
          i--;  /* Cause the new argv[i] to be processed. */
        }
      }
    }
    else {      /* attached value */
      if ( value && argd >= 0 ) {
        /* Delete argv[i] from list. */
#ifdef SLA_DEBUG
#ifdef __cplusplus
        if ( debug >= 4 )
          std::cout << "Deleting " << i << ':' << argv[i] << " from argv" << std::endl;
#endif
#endif
        argv.erase(argv.begin() + i);
        i--;    /* Cause the new argv[i] to be processed. */
      }
    }


    nvalue++;   /* Count number of value found. */

#ifdef SLA_DEBUG
#ifdef __cplusplus
    if ( debug >= 8 ) for ( size_t j=1; j<argv.size(); j++ )
      std::cout << "remaining:argv["<<j<<"]="<<argv[j] << std::endl;
#endif
#endif

    }

  regfree(&flind_re);
  regfree(&pname_re);
  regfree(&assop_re);
  regfree(&sup_assop_re);

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
int sla_none( std::vector<std::string>& argv, const std::string& flind, const std::string& assop, const std::string& pname, int argd ) {
  return sla_str( argv, flind, assop, pname, (std::string*)NULL, argd );
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
#include <stdlib.h>
SLA_FCN_BEGIN( sla_float, float )               { SLA_CNV((float)atof) }
SLA_FCN_BEGIN( sla_double, double )             { SLA_CNV(atof) }
SLA_FCN_BEGIN( sla_int, int )                   { SLA_CNV(atoi) }
SLA_FCN_BEGIN( sla_short, short )               { SLA_CNV((short)atoi) }
SLA_FCN_BEGIN( sla_long, long )         { SLA_CNV((long)atoi) }
SLA_FCN_BEGIN( sla_uint, unsigned int ) { SLA_CNV((unsigned int)atoi) }
SLA_FCN_BEGIN( sla_ushort, unsigned short )     { SLA_CNV((unsigned short)atoi) }
SLA_FCN_BEGIN( sla_ulong, unsigned long )       { SLA_CNV((unsigned long)atoi) }
SLA_FCN_BEGIN( sla_char, char )         { SLA_CNV(*) }


#undef NEW_MEMORY
#undef DEL_MEMORY
