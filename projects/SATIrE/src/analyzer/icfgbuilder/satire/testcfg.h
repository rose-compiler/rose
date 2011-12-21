// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: testcfg.h,v 1.3 2008-09-29 12:33:37 gergo Exp $

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

  void outputGdlIcfg(KFG kfg, const char* gdl_name);
  void outputDotIcfg(KFG kfg, const char* dot_name);
  int kfg_testit(KFG kfg, int quiet_mode);

#ifdef __cplusplus
}
#endif

#endif
