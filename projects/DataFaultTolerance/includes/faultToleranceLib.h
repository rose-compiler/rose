/*
 * faultToleranceLib.h
 *
 *  Created on: May 24, 2011
 *      Author: sharma9
 */


#include <string>

void updateCheckBits(void *memLoc, std::string type);
void verifyCheckBits(void *memLoc, std::string type);

