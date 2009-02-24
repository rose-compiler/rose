
#include "iostream"
#define MPI_AST_ASTATTRIBSTR "CTP_MPI_AST_ATTRIB"

class MPI_ASTAttrib: public AstAttribute
{
	protected:
	bool MPIrelated;
	public:
	MPI_ASTAttrib(){MPIrelated=true;}
	bool isMPI(){return true;}
};


bool isNodeMPI(SgNode * sgNode);
void markNodeAsMPI(SgNode * node);

