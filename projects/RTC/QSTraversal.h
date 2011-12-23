#ifndef QSTRAVERSAL_H
#define QSTRAVERSAL_H
#include "CheckBase.h"

class TraversalBase;

class QSTraversal : public TraversalBase {

	public:
	virtual void nodeCollection(SgProject*);

	virtual void nodeInstrumentation(SgProject*);

};

#endif
