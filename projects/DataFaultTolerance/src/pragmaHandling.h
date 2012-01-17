/*
 * pragmaHandling.h
 *
 *  Created on: Nov 1, 2011
 *      Author: kamalsharma
 */

#ifndef PRAGMAHANDLING_H_
#define PRAGMAHANDLING_H_

#include "arrayBase.h"

void parseChunksizeValue(ArrayList* arrayList);
void parseArrayDimension(ArrayList* arrayList, ArrayInfo* arrayInfo, string variableName, int index, bool* moreDimensions);
void parseArrayInfo(ArrayList* arrayList);
bool recognizePragma(SgPragmaDeclaration *pragmaDec, ArrayList* arrayList);


#endif /* PRAGMAHANDLING_H_ */
