/*
	Copyright 04 Aug 1993 Sun Microsystems, Inc. All Rights Reserved
*/
/*  new.h -- declarations for operator new and new_handler

    Copyright 1991,1993 by TauMetric Corporation      ALL RIGHTS RESERVED

    @(#)new.h	1.4  04 Aug 1993 14:08:32
*/

#ifndef _NEW_H_
#define _NEW_H_

#include <stddef.h>	// to get size_t

typedef void (*_Pvf)();
_Pvf set_new_handler(_Pvf);

void * operator new(size_t, void *);
void * operator new(size_t);

#endif /* _NEW_H_ */
