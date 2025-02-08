#include <fstream>
#include <AutoTuningInterface.h>
#include <LoopTransformInterface.h>
#include <LoopTree.h>
#include <BlockingAnal.h>
#include <SymbolicExpr.h>

#ifndef _MSC_VER
#include <sys/time.h>
#else
#include "timing.h"
#endif

#include <ROSE_ASSERT.h>

#define DEBUG

ArrayAbstractionInterface* AutoTuningInterface::arrayInfo = 0;
POETCode* AutoTuningInterface::arrayAccess = 0;

int CopyArraySpec::index=0;

/***********************************************************************/
/******QY: Loop Handle Map **********************************/
/***********************************************************************/

LocalVar* HandleMap:: NewLoopHandle()
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

LocalVar* HandleMap:: NewBodyHandle()
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

LocalVar* HandleMap::
GetLoopHandle(AstInterface&, const AstNodePtr&)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

LocalVar* HandleMap:: GetBodyHandle(LoopTreeNode*, LocalVar*)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

void HandleMap::
ObserveCopyAst(AstInterfaceImpl&, const AstNodePtr&, const AstNodePtr&)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

void HandleMap::UpdateCodeGen(const LoopTreeCodeGenInfo&)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

void HandleMap::UpdateDeleteNode(const LoopTreeNode*)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

HandleMap:: ~HandleMap()
{
}

LocalVar* HandleMap:: GetLoopHandle(LoopTreeNode*)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

void HandleMap::GenTraceHandles(POETProgram&, AstInterface&)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

void HandleMap::GenTraceHandles(POETProgram&, LoopTreeNode*)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

LocalVar* HandleMap:: GetTraceTop(const std::string&)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

LocalVar* HandleMap:: GetTraceDecl(LocalVar*)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

LocalVar* HandleMap:: GetTracePrivate(LocalVar*)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

LocalVar* HandleMap:: FindTracePrivate(LocalVar*)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

LocalVar* HandleMap:: GetTraceInclude()
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

LocalVar* HandleMap:: FindTraceInclude()
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

LocalVar* HandleMap:: GetTraceTarget()
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

LocalVar* HandleMap::DeclareTraceInclude(POETProgram&, int&)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

LocalVar* HandleMap:: GetTraceCleanup(LocalVar*)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

LocalVar* HandleMap:: FindTraceCleanup(LocalVar*)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

std::string HandleMap:: to_string() const
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

LocalVar* HandleMap::
GenTraceCommand(POETProgram&,const std::vector<LocalVar*>&,
                LocalVar*,int&)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

/***********************************************************************/
/******QY: autotuning interface impl **********************************/
/***********************************************************************/

void AutoTuningInterface::
set_astInterface(AstInterface& fa)
{ fa.AttachObserver(&handleMap); }

void AutoTuningInterface::
set_arrayInfo( ArrayAbstractionInterface& _arrayInfo)
{
  arrayInfo = &_arrayInfo;
}

POETCode* AutoTuningInterface:: CreateArrayRef(POETProgram&, POETCode*, POETCode*, int)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}
POETCode* AutoTuningInterface:: Access2Array(POETProgram&, POETCode*, int)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

void AutoTuningInterface:: BuildPOETProgram()
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

void AutoTuningInterface:: Gen_POET_opt()
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

void AutoTuningInterface::GenOutput()
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

bool AutoTuningInterface::ApplyOpt(LoopTreeNode*)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

bool AutoTuningInterface::ApplyOpt(AstInterface&)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

void AutoTuningInterface::UnrollLoop(AstInterface&, const AstNodePtr&, int)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

void AutoTuningInterface::
ParallelizeLoop(LoopTreeNode*, int)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

void AutoTuningInterface::
BlockLoops(LoopTreeNode*, LoopTreeNode*, LoopBlocking*, const std::vector<FuseLoopInfo>*)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}
AutoTuningInterface:: ~AutoTuningInterface()
     { }

BlockSpec* AutoTuningInterface:: LoopBlocked(LocalVar*, unsigned *)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

void AutoTuningInterface::
CopyArray( CopyArrayConfig&, LoopTreeNode*)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

/***********************************************************************/
/******QY: finite differencing  impl **********************************/
/***********************************************************************/

/***********************************************************************/
/******QY: loop unrolling  impl **********************************/
/***********************************************************************/

LocalVar* UnrollSpec:: get_unrollSizeVar(const std::string&)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

std::string UnrollSpec:: to_string(OptLevel)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

UnrollSpec :: UnrollSpec (LocalVar*, int)
  : OptSpec(0,"")
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

POETCode* UnrollSpec::
insert_paramDecl(AutoTuningInterface&, POETProgram&, OptLevel, int&)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

POETCode* UnrollSpec::
gen_xformEval(POETProgram&, LocalVar*, POETCode*, OptLevel, int&)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

/***********************************************************************/
/******QY: loop parallelization impl **********************************/
/***********************************************************************/

std::string ParLoopSpec:: to_string(OptLevel)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

ParLoopSpec :: ParLoopSpec(LocalVar*, LoopTreeNode*,int)
   : OptSpec(0, "")
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

POETCode* ParLoopSpec::
insert_paramDecl(AutoTuningInterface&, POETProgram&, OptLevel, int&)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

void ParLoopSpec::
insert_xformDecl(POETProgram&, LocalVar*, POETCode*&, int&)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}


POETCode* ParLoopSpec::
gen_xformEval(POETProgram&, LocalVar*, POETCode*, OptLevel, int&)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

/***********************************************************************/
/******QY: loop blocking  impl **********************************/
/***********************************************************************/

std::string BlockSpec:: to_string(OptLevel)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

LocalVar* BlockSpec::get_blockDimVar(const std::string&)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

LocalVar* BlockSpec::get_blockSizeVar(const std::string&)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

POETCode* BlockSpec:: get_blockSize(const std::string&, int)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

LocalVar* BlockSpec::get_unrollJamSizeVar(const std::string&)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

POETCode* BlockSpec:: get_ujSize(const std::string&, int)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

POETCode* FuseLoopInfo::toPOET(HandleMap&, const FuseLoopInfo&)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

BlockSpec::BlockSpec(HandleMap& /*_handleMap*/,
                     LocalVar*, LoopTreeNode*, LoopBlocking*,
                     const std::vector<FuseLoopInfo>*)
  : OptSpec(0, "") /*, handleMap(_handleMap)*/ // compiler warning: private field 'handleMap' is not used
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

LocalVar* BlockSpec:: get_blockTileVar(const std::string&)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

LocalVar* BlockSpec:: get_blockSplitVar(const std::string&)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}


POETCode* BlockSpec::compute_blockDim(LocalVar*)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

POETCode* BlockSpec::
insert_paramDecl(AutoTuningInterface&, POETProgram&, OptLevel, int&)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

void BlockSpec::
insert_xformDecl(POETProgram&, LocalVar*, POETCode*&, int&)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

POETCode* BlockSpec::gen_xformEval(POETProgram&, LocalVar*, POETCode*, OptLevel, int&)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

/***********************************************************************/
/******QY: array copying  impl **********************************/
/***********************************************************************/

std::string CopyArraySpec:: to_string(OptLevel)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

int CopyArraySpec::get_loopLevel(const SelectArray::ArrayDim&)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

CopyArraySpec::
CopyArraySpec(HandleMap&, POETCode* target, const std::string& targetName, CopyArrayConfig& config, LoopTreeNode*)
     : OptSpec(target,targetName), sel(config.get_arr()), opt(config.get_opt()), permute(0)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

/* QY: return variable name  used to trace array dimension to be copied */
LocalVar* CopyArraySpec::
get_dimVar (POETProgram&, const std::string&)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}


/* QY: trace handle for array name */
LocalVar* CopyArraySpec:: get_arrVar
(POETProgram&, const std::string&)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}


void CopyArraySpec::
insert_xformDecl(POETProgram&, LocalVar*, POETCode*&, int&)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

void CopyArraySpec:: compute_copySubscript(LocalVar*)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

POETCode* CopyArraySpec::
compute_copySubscript(POETProgram&, bool)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

POETCode* CopyArraySpec::
compute_copyDim(POETProgram&, bool)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

/*QY: compute internal configuration, including cur_id, permute and placeVec*/
void CopyArraySpec:: compute_config()
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

POETCode* CopyArraySpec:: gen_cpIvarDecl
  (POETProgram&, LocalVar*, const std::string&, int, bool)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

POETCode* CopyArraySpec::
gen_copyInvoke(POETProgram&, POETCode*, LocalVar*,
        const std::string&, POETCode*, CopyArrayOpt,
        POETCode*, POETCode*, bool, POETCode*)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

LocalVar* BlockCopyArraySpec::scalarRepl_handle(POETProgram&)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

BlockCopyArraySpec::
BlockCopyArraySpec(LocalVar*, CopyArrayConfig& config, BlockSpec&)
     : CopyArraySpec(0,"",config)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

POETCode* BlockCopyArraySpec::
compute_copyBlock(POETProgram&)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

POETCode* CopyArraySpec::
insert_paramDecl(AutoTuningInterface&, POETProgram&, OptLevel, int&)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

POETCode* CopyArraySpec::
gen_copyInvoke(POETProgram&, POETCode*, LocalVar*,
               POETCode*, bool, POETCode*, int&)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

POETCode* CopyArraySpec::
gen_xformEval(POETProgram&, LocalVar*, POETCode*, OptLevel, int&)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

POETCode* BlockCopyArraySpec::
gen_xformEval(POETProgram&, LocalVar*, POETCode*, OptLevel, int&)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

int BlockCopyArraySpec:: compute_fdConfig
(POETProgram&, POETCode*, bool,
       std::vector<POETCode*>&, /*QY:copy+strength reduction*/
       std::vector<POETCode*>&) /*QY:no copy,just strength reduction*/
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}


POETCode* BlockCopyArraySpec::
AfterCopy_dimSize(POETProgram&)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  ROSE_ABORT();
}

double GetWallTime(void)
   {
      struct timeval tp;
      static long start=0, startu;
      if (!start)
      {
         gettimeofday(&tp, NULL);
         start = tp.tv_sec;
         startu = tp.tv_usec;
         return(0.0);
      }
      gettimeofday(&tp, NULL);
      return( ((double) (tp.tv_sec - start)) + (tp.tv_usec-startu)/1000000.0 );
   }

