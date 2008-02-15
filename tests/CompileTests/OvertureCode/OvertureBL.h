/* This include file will define Boxlib define's in terms of Overture defines */
/* wdh 971016 */
#ifndef OVERTURE_BL_H 
#define OVERTURE_BL_H 

#include "OvertureDefine.h"

#ifdef OV_USE_DOUBLE
#undef BL_USE_DOUBLE
#define BL_USE_DOUBLE
#else
#undef BL_USE_FLOAT
#define BL_USE_FLOAT
#endif

#endif
