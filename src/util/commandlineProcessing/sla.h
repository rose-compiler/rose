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

$Id: sla.h,v 1.3 2008/01/08 02:56:44 dquinlan Exp $

*/

/*
 *
 * Jeremiah J. Willcock, 10-16-2007
 * Changed the C++ version of this code to use std::strings rather than char*
 * to allow better integration with the string version of ROSE command line
 * processing.
 *
 */

#ifndef __INCLUDED_sla_hh__
#define __INCLUDED_sla_hh__
/*
$Header: /nfs/casc/overture/ROSE/ROSE2_Repository/ROSE/src/util/commandlineProcessing/sla.h,v 1.3 2008/01/08 02:56:44 dquinlan Exp $
*/

#ifdef __cplusplus
#include <vector>
#include <string>
#endif

#ifndef ARGVCONST
#define ARGVCONST
#endif

void sla_set_debug( int d );

#if 1
#ifdef __cplusplus
#define sla_str		sla
#define sla_none	sla
#define sla_float	sla
#define sla_double	sla
#define sla_int		sla
#define sla_short	sla
#define sla_long	sla
#define sla_uint	sla
#define sla_ushort	sla
#define sla_ulong	sla
#define sla_char	sla
#endif
#endif

#ifdef __cplusplus
int sla_none( std::vector<std::string> &argv, const std::string& flind, const std::string& assop, const std::string& pname, int argd=0 );
#else
int sla_none( int *argc, ARGVCONST char **argv, const char *flind, const char *assop, const char *pname, int argd );
#endif

/*** generates code for sla prototypes ***/
#ifndef SLA_FCN_PROTO
#ifdef __cplusplus
#define SLA_FCN_PROTO(N,T) \
int N( std::vector<std::string>& argv	\
     , const std::string& flind \
     , const std::string& assop \
     , const std::string& pname \
     , T *value	\
     , int argd	= 0 /* default argument */	\
     )
#else
#define SLA_FCN_PROTO(N,T) \
int N( int *argc \
     , ARGVCONST char **argv \
     , const char *flind \
     , const char *assop \
     , const char *pname \
     , T *value \
     , int argd /* no default argument */ \
     )
#endif
#endif

/*** generates code for sla function headers ***/
#ifndef SLA_FCN_BEGIN
#ifdef __cplusplus
#define SLA_FCN_BEGIN(N,T) \
int N( std::vector<std::string>& argv	\
     , const std::string& flind \
     , const std::string& assop \
     , const std::string& pname \
     , T *value	\
     , int argd	\
     )
#else
#define SLA_FCN_BEGIN(N,T) SLA_FCN_PROTO(N,T)
#endif
#endif

/*** For allocating and deleting memory ***/
#include <stdlib.h>

/*** Sla for type T (automatically uses function M to modify). ***/
#ifndef SLA_MOD
#ifdef __cplusplus
#define SLA_MOD(M) \
  std::vector<std::string> rr(argv.size()); \
  int i, nvalue; \
  std::string ppname = pname; \
  if ( !pname.empty() && pname[0] != '*' ) { \
    ppname = "*" + ppname; \
  } \
  nvalue = sla_str( argv, flind, assop, ppname, rr, argd ); \
  if ( nvalue > 0 && value != NULL ) { \
    if ( !pname.empty() && pname[0] == '*' ) { \
      for ( i=0; i<nvalue; i++ ) M( value+i, rr[i] ); \
    } \
    else { \
      for ( i=0; i<nvalue; i++ ) M( value  , rr[i] ); \
    } \
  } \
  return nvalue;
#else
#define SLA_MOD(M) \
  ARGVCONST char **rr, eol='\0'; \
  int i, nvalue; \
  rr = (ARGVCONST char**)malloc( (*argc)*sizeof(ARGVCONST char*) ); \
  for ( i=0; i<*argc; i++ ) rr[i] = &eol; \
  nvalue = sla_str( argc, argv, flind, assop, pname, rr, argd ); \
  if ( nvalue > 0 && value != NULL ) { \
    if ( *pname == '*' ) { \
      for ( i=0; i<nvalue; i++ ) M( value+i, rr[i] ); \
    } \
    else { \
      for ( i=0; i<nvalue; i++ ) M( value  , rr[i] ); \
    } \
  } \
  free(rr); \
  return nvalue;
#endif
#endif

/*** Sla for type T (automatically uses function C to convert). ***/
#ifndef SLA_CNV
#ifdef __cplusplus
#define SLA_CNV(C) \
  std::vector<std::string> rr(argv.size()); \
  int i, nvalue; \
  std::string ppname = pname; \
  if ( pname.empty() || pname[0] != '*' ) { \
    ppname = "*" + ppname; \
  } \
  nvalue = sla_str( argv, flind, assop, ppname, &rr[0], argd ); \
  if ( nvalue > 0 && value != NULL ) { \
    if ( !pname.empty() && pname[0] == '*' ) { \
      for ( i=0; i<nvalue; i++ ) value[i] = C(rr[i].c_str()); \
    } \
    else { \
      for ( i=0; i<nvalue; i++ ) value[0] = C(rr[i].c_str()); \
    } \
  } \
  return nvalue;
#else
#define SLA_CNV(C) \
  ARGVCONST char **rr, eol='\0'; \
  int i, nvalue; \
  rr = (ARGVCONST char**)malloc( (*argc)*sizeof(ARGVCONST char*) ); \
  for ( i=0; i<*argc; i++ ) rr[i] = &eol; \
  nvalue = sla_str( argc, argv, flind, assop, pname, rr, argd ); \
  if ( nvalue > 0 && value != NULL ) { \
    if ( *pname == '*' ) { \
      for ( i=0; i<nvalue; i++ ) value[i] = C(rr[i]); \
    } \
    else { \
      for ( i=0; i<nvalue; i++ ) value[0] = C(rr[i]); \
    } \
  } \
  free(rr); \
  return nvalue;
#endif
#endif


/*
  Generate additional sla for various primitive types.
  The use of the SLA macro makes it
  difficult to see the prototypes of the functions created.
  To find the prototypes, try this:
  > CC -P sla.cc
  > pgrep 's/^(int sla\s*\([^\{]*).*$/$1;/' sla.i -s
 */
#ifdef __cplusplus
SLA_FCN_PROTO( sla_str,		std::string	);
#else
SLA_FCN_PROTO( sla_str,		ARGVCONST char *	);
#endif
SLA_FCN_PROTO( sla_float,	float			);
SLA_FCN_PROTO( sla_double,	double			);
SLA_FCN_PROTO( sla_int,		int			);
SLA_FCN_PROTO( sla_short,	short 			);
SLA_FCN_PROTO( sla_long,		long			);
SLA_FCN_PROTO( sla_uint,		unsigned int		);
SLA_FCN_PROTO( sla_ushort,	unsigned short		);
SLA_FCN_PROTO( sla_ulong,	unsigned long		);
SLA_FCN_PROTO( sla_char,		char			);




#endif
