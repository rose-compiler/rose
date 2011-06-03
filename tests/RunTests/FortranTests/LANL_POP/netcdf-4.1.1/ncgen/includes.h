#ifdef HAVE_CONFIG_H
#include <config.h>
#else
#include "config.h"
#endif


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>	/* for isprint() */
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef __hpux
#include <locale.h>
#endif

#include "list.h"
#include "bytebuffer.h"

/* Local Configuration flags*/
#define ENABLE_BINARY
#define ENABLE_C
#define ENABLE_F77
#define ENABLE_JAVA

#include "netcdf.h"
#include "odom.h"
#include "data.h"
#include "ncgen.h"
#include "genlib.h"
#include "util.h"
#include "debug.h"

#ifdef HAVE_STRDUP
#define nulldup(s) ((s)==NULL?NULL:strdup(s))
#else
extern char* nulldup(const char*);
#endif
