#ifndef BINARYSIMPLETRAVERSAL_H
#define BINARYSIMPLETRAVERSAL_H
#include "CheckBase.h"

class TraversalBase;



class BinarySimpleTraversal : public TraversalBase , public AstSimpleProcessing {

    private:
	BopContainer SelectedNodes; 


	public:

	virtual void nodeCollection(SgProject*);

	virtual void nodeInstrumentation(SgProject*);

    protected:

	virtual void visit(SgNode* astNode);

	void atTraversalStart();

	void atTraversalEnd();

	bool isUseful(SgBinaryOp*); 
};

#endif
