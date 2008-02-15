//-*-Mode: C++;-*-
#ifndef _padding_tools_h_
#define _padding_tool_h_

#include "arrayInfo.h"
#include "cacheInfo.h"
#include "bool.h"
#include "tlist.h"
#include "padding_conditions.h"

// CW: allowed maximum size of a padding in elements
#define PADDING_SIZE_MAX 500

// CW: if you are using padding conditions be aware of there limitations
// and restrictions described in padding_conditions.h!!!

// CW: returns new arrayInfo for array A with (eventually) padded columns
arrayInfo getNeededIntraPadding(const cacheInfo& cInfo, const arrayInfo& aInfo, const List<intra_padding_condition>& ipcList);

// CW: returns new arrayInfo for array B with (eventually) changed base address
arrayInfo getNeededInterPadding(const cacheInfo& cInfo, const arrayInfo& aInfo, const arrayInfo& bInfo, const List<inter_padding_condition>& ipcList);

// CW: Test all padding conditions and return FALSE if non of it is TRUE and TRUE otherwise
bool testIntraPaddingConditions(const cacheInfo& cInfo, const arrayInfo& aInfo,const List<intra_padding_condition>& ipcList);
bool testInterPaddingConditions(const cacheInfo& cInfo, const arrayInfo& aInfo, const arrayInfo& bInfo,const List<inter_padding_condition>& ipcList);

// CW: returns TRUE is the two arrays are conforming (column sizes have to be
// identical except the size of the higest dimension).
bool conformingArrays(const arrayInfo& aInfo, const arrayInfo& bInfo);

#endif
