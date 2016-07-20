#ifndef RERS_SPECIALIZATION
#define RERS_SPECIALIZATION

// RERS-binary-binding-specific declarations
#define STR_VALUE(arg) #arg

// integer variables
#define INIT_GLOBALVAR(VARNAME) VARNAME = new int[numberOfThreads];
#define COPY_PSTATEVAR_TO_GLOBALVAR(VARNAME) VARNAME[thread_id] = pstate[analyzer->globalVarIdByName(STR_VALUE(VARNAME))].getIntValue();
//cout<<"PSTATEVAR:"<<pstate[analyzer->globalVarIdByName(STR_VALUE(VARNAME))].toString()<<"="<<pstate[analyzer->globalVarIdByName(STR_VALUE(VARNAME))].toString()<<endl;
#define COPY_GLOBALVAR_TO_PSTATEVAR(VARNAME) pstate[analyzer->globalVarIdByName(STR_VALUE(VARNAME))]=CodeThorn::AType::ConstIntLattice(VARNAME[thread_id]);

// pointers to integer variables
#define INIT_GLOBALPTR(VARNAME) VARNAME = new int*[numberOfThreads]; 
#define COPY_PSTATEPTR_TO_GLOBALPTR(VARNAME) VARNAME[thread_id] = analyzer->mapGlobalVarAddress[analyzer->getVarNameByIdCode(pstate[analyzer->globalVarIdByName(STR_VALUE(VARNAME))].getIntValue())]
#define COPY_GLOBALPTR_TO_PSTATEPTR(VARNAME) pstate[analyzer->globalVarIdByName(STR_VALUE(VARNAME))]=CodeThorn::AType::ConstIntLattice(analyzer->globalVarIdByName(analyzer->mapAddressGlobalVar[VARNAME[thread_id]]).getIdCode());

// create an entry in the mapping    <var_address>  <-->  <var_name>
#define REGISTER_GLOBAL_VAR_ADDRESS(VARNAME) analyzer->mapGlobalVarInsert(STR_VALUE(VARNAME), (int*) &VARNAME);

namespace RERS_Problem {
  void rersGlobalVarsCallInit(CodeThorn::Analyzer* analyzer, CodeThorn::PState& pstate, int thread_id);
  void rersGlobalVarsCallReturnInit(CodeThorn::Analyzer* analyzer, CodeThorn::PState& pstate, int thread_id);
  void rersGlobalVarsArrayInit(int numberOfThreads);
  void createGlobalVarAddressMaps(CodeThorn::Analyzer* analyzer);

  void calculate_output(int numberOfThreads);
  extern int* output;
}
// END OF RERS-binary-binding-specific declarations
#endif
