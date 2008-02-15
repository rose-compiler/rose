/* $Id: p_icclib.c,v 1.1 2005/10/11 17:18:14 vuduc2 Exp $
 *
 * ICClib routines for PDFlib, slightly modified from the original ICClib.
 * (see below).
 *
 * $Log: p_icclib.c,v $
 * Revision 1.1  2005/10/11 17:18:14  vuduc2
 * see ChangeLog
 *
 * Revision 1.17.2.1  2004/07/30 16:14:30  kurt
 * icc_read: all free statements in the error case removed
 *           (because of program crash in icc_delete)
 * icc_delete: more security checks
 * new public function: icc_get_errmsg
 *
 */

/*
 * International Color Consortium Format Library (icclib)
 * For ICC profile version 3.4
 *
 * Author:  Graeme W. Gill
 * Date:    2002/04/22
 * Version: 2.02
 *
 * Copyright 1997 - 2002 Graeme W. Gill
 * See Licence.txt file for conditions of use.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#ifdef __sun
#include <unistd.h>
#endif
#if defined(__IBMC__) && defined(_M_IX86)
#include <float.h>
#endif

/* PDFlib */
#include "pc_util.h"
#include "pc_core.h"
