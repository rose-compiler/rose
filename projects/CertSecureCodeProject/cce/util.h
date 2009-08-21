#ifndef UTIL_H
#define UTIL_H

/*
 * Copyright (c) 2007 Carnegie Mellon University.
 * All rights reserved.

 * Permission to use this software and its documentation for any purpose is hereby granted,
 * provided that the above copyright notice appear and that both that copyright notice and
 * this permission notice appear in supporting documentation, and that the name of CMU not
 * be used in advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.
 *
 * CMU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL CMU BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, RISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <complex.h>
#include <time.h>
#include <float.h>
#include <unistd.h>
#include <time.h>
#include <wchar.h>
#include <ctype.h>
#include <inttypes.h>
#include <stddef.h>
#include <assert.h>
#include <float.h>
#include <fenv.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <setjmp.h>
#include <errno.h>
#include <fcntl.h>

extern char **environ;

/* This is for the case where we just want to run it through a static
 * analysis tool and don't care about runtime errors. This is not 
 * specified as a #define to avoid being ignored by the tools.
*/
#define RUNTIME_IGNORE 1

#endif
