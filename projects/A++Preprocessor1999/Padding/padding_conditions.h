//-*-Mode: C++;-*-
#ifndef _padding_conditions_h_
#define _padding_conditions_h_

#include "cacheInfo.h"
#include "arrayInfo.h"
#include "bool.h"

// CW: this constant are defined in the articles
// I guess their value is based on benchmark results 
#define PADLITE_M 4
#define LINPAD_MAX_J 129

// ============================================================
// CW: inter array padding conditions:
// ============================================================

// CW: no restrictions
bool condition_interpadlite(const cacheInfo& cInfo, const arrayInfo& aInfo, const arrayInfo& bInfo);

// CW: this works only with conforming arrays!
bool condition_interpad(const cacheInfo& cInfo, const arrayInfo& aInfo, const arrayInfo& bInfo);

// ============================================================
// CW: intra array padding conditions:
// ============================================================

// CW: linpad1 and linpad2 work only for 2D so far
// restrictions:
// if elemSize>cacheLineSize and elemSize%cacheLineSize==0 it is very likely that the condition
// is always true. I'm not sure when exactly this happens. If the above condition is
// TRUE you should not use linpad together with other conditions because if linpad is always
// TRUE no good padding will be found nevertheless the other conditions specify
bool condition_linpad1(const cacheInfo& cInfo, const arrayInfo& aInfo);
bool condition_linpad2(const cacheInfo& cInfo, const arrayInfo& aInfo);

// CW: restrictions: cacheSize > 3*PADLITE_M
bool condition_intrapadlite(const cacheInfo& cInfo, const arrayInfo& aInfo);

// CW: no restrictions but will only give accurate results if the array info
// contains references
bool condition_intrapad(const cacheInfo& cInfo, const arrayInfo& aInfo);

// CW: define padding condition types:
typedef bool (*intra_padding_condition)(const cacheInfo&,const arrayInfo&);
typedef bool (*inter_padding_condition)(const cacheInfo&,const arrayInfo&,const arrayInfo&);

#endif
