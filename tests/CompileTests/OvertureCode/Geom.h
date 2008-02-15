#ifndef __KKC_GEOM__
#define __KKC_GEOM__

#include "Overture.h"
#include "Face.h"


bool intersect2D(realArray const &a, realArray const &b, realArray const &c, realArray const &d, bool &isParallel);

bool checkFaceAngle(const realArray &a, const realArray &b, const realArray &c);

#include "Geom_inline.C"

#endif
