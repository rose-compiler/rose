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

#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

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
  assert(0);
  return 0;
}

LocalVar* HandleMap:: NewBodyHandle()
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

LocalVar* HandleMap::
GetLoopHandle(AstInterface &fa, const AstNodePtr& loop)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

LocalVar* HandleMap:: GetBodyHandle(LoopTreeNode* loop, LocalVar* loophandle)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

void HandleMap::
ObserveCopyAst(AstInterfaceImpl& fa,
      const AstNodePtr& orig, const AstNodePtr& n)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
}

void HandleMap::UpdateCodeGen(const LoopTreeCodeGenInfo& info)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
}

void HandleMap::UpdateDeleteNode(LoopTreeNode* n)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
}

HandleMap:: ~HandleMap()
{
}

LocalVar* HandleMap:: GetLoopHandle(LoopTreeNode* loop)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

void HandleMap::GenTraceHandles(POETProgram& poet, AstInterface& fa)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
}

void HandleMap::GenTraceHandles(POETProgram& poet, LoopTreeNode* r)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
}

LocalVar* HandleMap:: GetTraceTop(const std::string& handleName)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

LocalVar* HandleMap:: GetTraceDecl(LocalVar* top)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

LocalVar* HandleMap:: GetTracePrivate(LocalVar* top)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

LocalVar* HandleMap:: FindTracePrivate(LocalVar* top)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

LocalVar* HandleMap:: GetTraceInclude()
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

LocalVar* HandleMap:: FindTraceInclude()
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

LocalVar* HandleMap:: GetTraceTarget()
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

LocalVar* HandleMap::DeclareTraceInclude(POETProgram& poet, int& lineNo)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

LocalVar* HandleMap:: GetTraceCleanup(LocalVar* top)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

LocalVar* HandleMap:: FindTraceCleanup(LocalVar* top)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

std::string HandleMap:: to_string() const
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return "";
}

LocalVar* HandleMap::
GenTraceCommand(POETProgram& poet,const std::vector<LocalVar*>& handles,
                LocalVar* target,int &lineNo)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
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

POETCode* AutoTuningInterface:: CreateArrayRef(POETProgram& poet, POETCode* arr, POETCode* subscript, int dim)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}
POETCode* AutoTuningInterface:: Access2Array(POETProgram& poet, POETCode* ref,int dim)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

void AutoTuningInterface:: BuildPOETProgram()
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
}

void AutoTuningInterface:: Gen_POET_opt()
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
}

void AutoTuningInterface::GenOutput()
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
}

bool AutoTuningInterface::ApplyOpt(LoopTreeNode* r)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return true;
}

bool AutoTuningInterface::ApplyOpt(AstInterface& fa)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return true;
}

void AutoTuningInterface::UnrollLoop(AstInterface& fa, const AstNodePtr& loop, int unrollsize)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return;
}

void AutoTuningInterface::
ParallelizeLoop(LoopTreeNode* outerLoop, int bsize)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
}

void AutoTuningInterface::
BlockLoops(LoopTreeNode* outerLoop, LoopTreeNode* innerLoop,
      LoopBlocking* config, const std::vector<FuseLoopInfo>* nonperfect)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
}
AutoTuningInterface:: ~AutoTuningInterface()
     { }

BlockSpec* AutoTuningInterface:: LoopBlocked(LocalVar* loop, unsigned *index)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

void AutoTuningInterface::
CopyArray( CopyArrayConfig& config, LoopTreeNode* repl)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
}

/***********************************************************************/
/******QY: finite differencing  impl **********************************/
/***********************************************************************/

/***********************************************************************/
/******QY: loop unrolling  impl **********************************/
/***********************************************************************/

LocalVar* UnrollSpec:: get_unrollSizeVar(const std::string& handleName)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

std::string UnrollSpec:: to_string(OptLevel level)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return "";
}

UnrollSpec :: UnrollSpec (LocalVar* handle, int unrollSize)
  : OptSpec(0,"")
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
}


POETCode* UnrollSpec::
insert_paramDecl(AutoTuningInterface& tune, POETProgram& poet,
                 OptLevel optLevel, int& lineNo)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

POETCode* UnrollSpec::
gen_xformEval(POETProgram& poet, LocalVar* top,
                   POETCode* traceMod, OptLevel optLevel, int& lineNo)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

/***********************************************************************/
/******QY: loop parallelization impl **********************************/
/***********************************************************************/

std::string ParLoopSpec:: to_string(OptLevel level)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return "";
}

ParLoopSpec :: ParLoopSpec(LocalVar* handle, LoopTreeNode* outerLoop,int bsize)
   : OptSpec(0, "")
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
}

POETCode* ParLoopSpec::
insert_paramDecl(AutoTuningInterface& tune, POETProgram& poet,
                 OptLevel optLevel, int& lineNo)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

void ParLoopSpec::
insert_xformDecl(POETProgram& poet, LocalVar* top, POETCode*& traceMod, int& lineNo)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
}


POETCode* ParLoopSpec::
gen_xformEval(POETProgram& poet, LocalVar* top,
                   POETCode* traceMod, OptLevel optLevel, int& lineNo)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

/***********************************************************************/
/******QY: loop blocking  impl **********************************/
/***********************************************************************/

std::string BlockSpec:: to_string(OptLevel level)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return "";
}

LocalVar* BlockSpec::get_blockDimVar(const std::string& handleName)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}


LocalVar* BlockSpec::get_blockSizeVar(const std::string& handleName)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}


POETCode* BlockSpec:: get_blockSize(const std::string& handleName, int level)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

LocalVar* BlockSpec::get_unrollJamSizeVar(const std::string& handleName)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}


POETCode* BlockSpec:: get_ujSize(const std::string& handleName, int level)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}


POETCode* FuseLoopInfo::toPOET(HandleMap& handleMap, const FuseLoopInfo& info)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

BlockSpec::BlockSpec(HandleMap& _handleMap,
            LocalVar* outerHandle, LoopTreeNode* _innerLoop,
            LoopBlocking* config,
            const std::vector<FuseLoopInfo>* _nonperfect)
   : OptSpec(0, ""), handleMap(_handleMap)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
}

LocalVar* BlockSpec:: get_blockTileVar(const std::string& handleName)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}


LocalVar* BlockSpec:: get_blockSplitVar(const std::string& handleName)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}


POETCode* BlockSpec::compute_blockDim(LocalVar* paramVar)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

POETCode* BlockSpec::
insert_paramDecl(AutoTuningInterface& tune, POETProgram& poet,
                 OptLevel optLevel, int& lineNo)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

void BlockSpec::
insert_xformDecl(POETProgram& poet, LocalVar* top, POETCode*& traceMod, int& lineNo)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
}

POETCode* BlockSpec::gen_xformEval(POETProgram& poet, LocalVar* top,
                   POETCode* traceMod, OptLevel optLevel, int& lineNo)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

/***********************************************************************/
/******QY: array copying  impl **********************************/
/***********************************************************************/

std::string CopyArraySpec:: to_string(OptLevel level)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return "";
}

int CopyArraySpec::get_loopLevel(const SelectArray::ArrayDim& cur)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

CopyArraySpec::
CopyArraySpec(HandleMap& handleMap, POETCode* target, const std::string& targetName, CopyArrayConfig& config, LoopTreeNode* root)
     : OptSpec(target,targetName), sel(config.get_arr()), opt(config.get_opt()),permute(0)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
}

/* QY: return variable name  used to trace array dimension to be copied */
LocalVar* CopyArraySpec::
get_dimVar (POETProgram& poet, const std::string& arrname)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}


/* QY: trace handle for array name */
LocalVar* CopyArraySpec:: get_arrVar
(POETProgram& poet, const std::string& arrname)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}


void CopyArraySpec::
insert_xformDecl(POETProgram& poet, LocalVar* top, POETCode*& traceMod, int& lineNo)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
}

void CopyArraySpec:: compute_copySubscript(LocalVar* dimVar)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
}

POETCode* CopyArraySpec::
compute_copySubscript(POETProgram& poet, bool afterCopy)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

POETCode* CopyArraySpec::
compute_copyDim(POETProgram& poet, bool scalar)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

/*QY: compute internal configuration, including cur_id, permute and placeVec*/
void CopyArraySpec:: compute_config()
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
}

POETCode* CopyArraySpec:: gen_cpIvarDecl
  (POETProgram& poet, LocalVar* top,
  const std::string& arrname, int dim, bool cpblock)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

POETCode* CopyArraySpec::
gen_copyInvoke(POETProgram& poet, POETCode* cploc, LocalVar* top,
        const std::string& arrname,
        POETCode* arrelemType, CopyArrayOpt opt,
        POETCode* cpDim, POETCode* cpblock, bool scalar, POETCode* traceMod)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

LocalVar* BlockCopyArraySpec::scalarRepl_handle(POETProgram& poet)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

BlockCopyArraySpec::
BlockCopyArraySpec(LocalVar* handle, CopyArrayConfig& config,
                     BlockSpec& _block)
     : CopyArraySpec(0,"",config)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
}

POETCode* BlockCopyArraySpec::
compute_copyBlock(POETProgram& poet)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

POETCode* CopyArraySpec::
insert_paramDecl(AutoTuningInterface& tune, POETProgram& poet,
                 OptLevel optLevel, int& lineNo)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

POETCode* CopyArraySpec::
gen_copyInvoke(POETProgram& poet, POETCode* cphandle, LocalVar* top,
        POETCode* cpblock, bool scalar, POETCode* traceMod, int& lineNo)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

POETCode* CopyArraySpec::
gen_xformEval(POETProgram& poet, LocalVar* top,
                   POETCode* traceMod, OptLevel optLevel, int& lineNo)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

POETCode* BlockCopyArraySpec::
gen_xformEval(POETProgram& poet, LocalVar* top,
                   POETCode* traceMod, OptLevel optLevel, int& lineNo)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}

int BlockCopyArraySpec:: compute_fdConfig
(POETProgram& poet, POETCode* handle, bool scalar,
       std::vector<POETCode*>& expDimVec_cp, /*QY:copy+strength reduction*/
       std::vector<POETCode*>& expDimVec_nocp) /*QY:no copy,just strength reduction*/
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
}


POETCode* BlockCopyArraySpec::
AfterCopy_dimSize(POETProgram& poet)
{
  std::cerr << "POET needs to be installed for this to work!\n";
  assert(0);
  return 0;
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

