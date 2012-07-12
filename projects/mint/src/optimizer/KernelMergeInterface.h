/*
 * KernelMergeInterface.h
 *
 *  Created on: Jul 4, 2010
 *      Author: didem
 */

#ifndef KERNELMERGEINTERFACE_H_
#define KERNELMERGEINTERFACE_H_

#include "rose.h"


#include <map>
#include <algorithm>

using namespace SageBuilder;
using namespace SageInterface;


class KernelMergeInterface
{

public:
	KernelMergeInterface();
	virtual ~KernelMergeInterface();

	static bool mergeKernels(SgFunctionDeclaration* kernel1,
				 SgFunctionDeclaration* kernel2);



 private:

};

#endif /* KERNELMERGEINTERFACE_H_ */
