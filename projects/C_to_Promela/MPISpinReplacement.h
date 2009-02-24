#include <map>
#include <string>

#include "SpinPStructContainer.h"
#include "MPICallListHelper.h"

class MPISpinReplacement
{
	typedef enum
	{
		NO_MPI_CALL=-1,
		MPI_DUMMY_CALL=0,
		MPI_ENUM_LIST
		/*
		MPI_Init_CALL=1,
		MPI_Finalize_CALL,
		MPI_Comm_rank_CALL,
		MPI_Comm_size_CALL,
		MPI_Send_CALL,
		MPI_Recv_CALL,
		MPI_Bcast_CALL,
		MPI_Reduce_CALL,
		MPI_Initialized_CALL,
		MPI_Finalized_CALL*/
//		#include "mpiEnum.inc"
	}MPICallType;
	
	typedef enum
	{	
		NotSupportedByMPISpin,
		NotImplemented,
		NotSuported
	}NotImplementedReason;
	
	std::map<std::string,MPICallType> mpiCallToIdMap;
	std::map<MPICallType,std::string> idMapToMpiCall;
	void initMPICallToIdMap();
	MPICallType mapFunctionCall(SgFunctionCallExp * call);
	
	public:	
	MPISpinReplacement();
//old:	int replaceIn(SgNode * node,SgVariableDeclaration* var);
	int replaceIn(SgNode * node,SpinPStructContainer * spsc);
	int getNrOfMPICalls(){return foundMPICalls;};
	int getNrOfMPICallsChanged(){changedMPICalls;};
	protected:
	int foundMPICalls;
	int changedMPICalls;

	
	// MPI-Handler functions
	void processMPI_Init(SgFunctionCallExp *call,SpinPStructContainer* spsc);
	void processMPI_Finalize(SgFunctionCallExp *call,SpinPStructContainer * spsc);
	void processMPI_Comm_rank(SgFunctionCallExp *call,SpinPStructContainer * spsc);
	void processMPI_Comm_size(SgFunctionCallExp *call,SpinPStructContainer * spsc);
	void processMPI_Send(SgFunctionCallExp *call,SpinPStructContainer * spsc);
	void processMPI_Recv(SgFunctionCallExp *call,SpinPStructContainer * spsc);
	
//	void processMPI_Bcast(SgFunctionCallExp *call,SpinPStructContainer * spsc);
//	void processMPI_Reduce(SgFunctionCallExp *call,SpinPStructContainer * spsc);

	void processMPI_Initialized(SgFunctionCallExp *call,SpinPStructContainer * spsc);
	void processMPI_Finalized(SgFunctionCallExp *call,SpinPStructContainer * spsc);
	void processMPI_ISend(SgFunctionCallExp *call,SpinPStructContainer * spsc);

	void genericISend(SgFunctionCallExp *call,SpinPStructContainer * spsc);
//	void process(SgFunctionCallExp *call,SpinPStructContainer * spsc);

	SgFunctionCallExp * createNamedFunctioncall(std::string name,SgExprListExp * exprList);
	SgVarRefExp * MPISpinReplacement::createNamedVarRef(std::string name);
	void notImplemented(MPICallType type,NotImplementedReason reason);
	SgScopeStatement * globalScope;
	private:
	void registerMPICall(std::string,MPICallType);
	void markCallStatement(SgFunctionCallExp*call);
};
