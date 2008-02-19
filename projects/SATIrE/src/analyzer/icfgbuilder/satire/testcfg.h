// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: testcfg.h,v 1.2 2008-02-19 19:08:00 markus Exp $

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

  void outputIcfg(KFG kfg, char* gdl_name);
  int kfg_testit(KFG kfg, int quiet_mode);

#ifdef __cplusplus
}
#endif

#endif
