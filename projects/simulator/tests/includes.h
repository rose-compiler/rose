/*
 *   Copyright (C) International Business Machines  Corp., 2004,2005,2006
 *
 *   This program is free software;  you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY;  without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 *   the GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program;  if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 ******************************************************************************
 * File: includes.h
 *
 * Overview:
 *	This file contains all #defines for 32-bit system calls which might
 *	not be defined on systems which should support them. If they are
 *	not defined, then we define them so as to make use of them in the
 *	syscall testcases.
 *
 ********************************** HISTORY ***********************************
 *
 * DATE         NAME                    DESCRIPTION
 * 02/17/2006   mcthomps@us.ibm.com     Updated to work within new framework
 * 12/17/2004	ashok-atm@in.ibm.com	Originated CAPP/EAL4 includes.h
 *
 *****************************************************************************/

#ifndef _INCLUDES_H
#define _INCLUDES_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <unistd.h>

/* if NR_chown16 not defined AND x86_64 or s390x */
#if !defined(__NR_chown16) && ( defined(__X86_64) || defined(__S390X) ) 
#define __NR_chown16 182
#endif

/* if NR_chown32 not defined AND x86_64_32 or s390 or s390x */
#if !defined(__NR_chown32) && ( (defined(__X86_64) ) || defined(__S390) || defined(__S390X) )
#define __NR_chown32  212
#endif

/* if NR_fchown16 not defined AND x86_64 or s390x  */
#if !defined(__NR_fchown16) && ( defined(__X86_64) || defined(__S390X) ) 
#define __NR_fchown16 95
#endif

/* if NR_fchown32 not defined AND x86_64_32 or s390 or s390x */
#if !defined(__NR_fchown32) && ( (defined(__X86_64) ) || defined(__S390) || defined(__S390X) )
#define __NR_fchown32  207
#endif

/* if NR_lchown16 not defined AND x86_64 or s390x  */
#if !defined(__NR_lchown16) && ( defined(__X86_64) || defined(__S390X) ) 
#define __NR_lchown16 16
#endif

/* if NR_lchown32 not defined AND x86_64_32 or s390 or s390x */
#if !defined(__NR_lchown32) && ( (defined(__X86_64) ) || defined(__S390) || defined(__S390X) )
#define __NR_lchown32  198
#endif

/* if NR_setfsgid16 not defined AND x86_64 or s390x  */
#if !defined(__NR_setfsgid16) && ( defined(__X86_64) || defined(__S390X) ) 
#define __NR_setfsgid16 139
#endif

/* if NR_setfsgid32 not defined AND x86_64_32 or s390 or s390x */
#if !defined(__NR_setfsgid32) && ( (defined(__X86_64) ) || defined(__S390) || defined(__S390X) )
#define __NR_setfsgid32 216
#endif

/* if NR_setfsuid16 not defined AND x86_64 or s390x  */
#if !defined(__NR_setfsuid16) && ( defined(__X86_64) || defined(__S390X) ) 
#define __NR_setfsuid16 138
#endif

/* if NR_setfsuid32 not defined AND x86_64_32 or s390 or s390x */
#if !defined(__NR_setfsuid32) && ( (defined(__X86_64) ) || defined(__S390) || defined(__S390X) )
#define __NR_setfsuid32  215
#endif

/* if NR_setgid16 not defined AND x86_64 or s390x  */
#if !defined(__NR_setgid16) && ( defined(__X86_64) || defined(__S390X) ) 
#define __NR_setgid16 46
#endif

/* if NR_setgid32 not defined AND x86_64_32 or s390 or s390x */
#if !defined(__NR_setgid32) && ( (defined(__X86_64) ) || defined(__S390) || defined(__S390X) )
#define __NR_setgid32 214
#endif

/* if NR_setgroups16 not defined AND x86_64 or s390x  */
#if !defined(__NR_setgroups16) && ( defined(__X86_64) || defined(__S390X) ) 
#define __NR_setgroups16 81
#endif

/* if NR_setgroups32 not defined AND x86_64_32 or s390 or s390x */
#if !defined(__NR_setgroups32) && ( (defined(__X86_64) ) || defined(__S390) || defined(__S390X) )
#define __NR_setgroups32  206
#endif

/* if NR_setregid16 not defined AND x86_64 or s390x  */
#if !defined(__NR_setregid16) && ( defined(__X86_64) || defined(__S390X) ) 
#define __NR_setregid16 71
#endif

/* if NR_setregid32 not defined AND x86_64_32 or s390 or s390x */
#if !defined(__NR_setregid32) && ( (defined(__X86_64) ) || defined(__S390) || defined(__S390X) )
#define __NR_setregid32  204
#endif

/* if NR_setresgid16 not defined AND x86_64 or s390x  */
#if !defined(__NR_setresgid16) && ( defined(__X86_64) || defined(__S390X) ) 
#define __NR_setresgid16 170
#endif

/* if NR_setresgid32 not defined AND x86_64_32 or s390 or s390x */
#if !defined(__NR_setresgid32) && ( (defined(__X86_64) ) || defined(__S390) || defined(__S390X) )
#define __NR_setresgid32  210
#endif

/* if NR_setresuid16 not defined AND x86_64 or s390x  */
#if !defined(__NR_setresuid16) && ( defined(__X86_64) || defined(__S390X) ) 
#define __NR_setresuid16 164
#endif

/* if NR_setresuid32 not defined AND x86_64_32 or s390 or s390x */
#if !defined(__NR_setresuid32) && ( (defined(__X86_64) ) || defined(__S390) || defined(__S390X) )
#define __NR_setresuid32  208
#endif

/* if NR_setreuid16 not defined AND x86_64 or s390x  */
#if !defined(__NR_setreuid16) && ( defined(__X86_64) || defined(__S390X) ) 
#define __NR_setreuid16 70
#endif

/* if NR_setreuid32 not defined AND x86_64_32 or s390 or s390x */
#if !defined(__NR_setreuid32) && ( (defined(__X86_64) ) || defined(__S390) || defined(__S390X) )
#define __NR_setreuid32  203
#endif

/* if NR_setuid16 not defined AND x86_64 or s390x  */
#if !defined(__NR_setuid16) && ( defined(__X86_64) || defined(__S390X) ) 
#define __NR_setuid16 23
#endif

/* if NR_setuid32 not defined AND x86_64_32 or s390 or s390x */
#if !defined(__NR_setuid32) && ( (defined(__X86_64) ) || defined(__S390) || defined(__S390X) )
#define __NR_setuid32 213
#endif

#define XATTR_TEST_VALUE "text/plain"
#endif
