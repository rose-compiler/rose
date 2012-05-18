#ifndef __APIREADER_H__
#define __APIREADER_H__

#include "APISpec.h"

APISpec readAPISpec(std::string fname);
APISpecs readAPISpecCollection(std::string fname);

#endif // __APIREADER_H__
