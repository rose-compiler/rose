// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: testcfg.h,v 1.1 2007-09-20 09:25:32 adrian Exp $

/*
 * TestCFG: consistency test for a cfg and its PAG-interface
 * version 0.0.19
 */

#ifndef TEST_CFG_H__
#define TEST_CFG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "iface.h"

int kfg_testit (KFG kfg, int quiet_mode, char *gdl_name);

#ifdef __cplusplus
}
#endif

#endif
