#ifndef QSTRAVERSAL_C
#define QSTRAVERSAL_C
#include "QSTraversal.h"

using namespace SageInterface;

void QSTraversal::nodeCollection(SgProject* project) {

	BopContainer EligibleNodes = querySubTree<SgBinaryOp>(project, V_SgBinaryOp);

	QSTraversal::processBinaryOps(EligibleNodes);
}

void QSTraversal::nodeInstrumentation(SgProject* project) {
	
	QSTraversal::naiveInstrumentation();
}

#endif
