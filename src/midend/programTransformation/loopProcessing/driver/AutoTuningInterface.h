#ifndef TUNING_H
#define TUNING_H

#include <map>
#include <string>
#include <list>
#include <AstInterface.h>
#include <LoopTree.h>
#include <LoopTreeTransform.h>
class POETProgram;
class XformVar;
class CodeVar;
class LocalVar;
class POETCode;
class LoopTreeNode;
class LoopBlocking; 
class ParallelizeLoop;
class ArrayAbstractionInterface;
class CopyArrayConfig;

/*QY: map loop tree nodes and AST nodes to tracing variables in POET */
class LoopTreeCodeGenInfo;
class HandleMap : public LoopTreeObserver, public AstObserver
{
  public: typedef std::map<const void*, LocalVar*>  AstMapType;

  private:
static XformVar* const modifyHandle;
static LocalVar* const modify_trace;
   int loopindex, bodyindex;
   std::vector<LocalVar*> topHandles;

   std::map<const LoopTreeNode*,LocalVar*> loopMap;
   std::map<LocalVar*,LocalVar*> bodyMap;
   AstMapType astMap;

   LocalVar* NewLoopHandle();
   LocalVar* NewBodyHandle();
   virtual void UpdateCodeGen(const LoopTreeCodeGenInfo& info);
   virtual void UpdateDeleteNode(LoopTreeNode* n);
   virtual void ObserveCopyAst(AstInterfaceImpl& fa, const AstNodePtr& orig, const AstNodePtr& n);

 public:
   ~HandleMap();
    HandleMap() :loopindex(0),bodyindex(0) {}

   /*QY: return the number of loop handles created*/
   int NumOfLoops() { return loopindex; }
   int NumOfBodies() { return bodyindex; }

   /*QY:return a POET trace handle for the given AST or loop tree node*/
   LocalVar* GetLoopHandle(AstInterface& fa, const AstNodePtr& loop);
   LocalVar* GetLoopHandle(LoopTreeNode* loop);
   LocalVar* GetBodyHandle(LoopTreeNode* loop, LocalVar* loopHandle);
   /*QY:sort all the loop nest handles with AST root; return the top handles*/
   const std::vector<LocalVar*>& GetTopTraceHandles() { return topHandles; }
   void GenTraceHandles(POETProgram& poet, AstInterface& fa);
   void GenTraceHandles(POETProgram& poet, LoopTreeNode* r);
   LocalVar* HasBodyHandle(LocalVar* loopHandle)
      { std::map<LocalVar*,LocalVar*>::const_iterator p = 
              bodyMap.find(loopHandle);
        if (p == bodyMap.end()) return 0;
        return (*p).second;
      }
   /*QY: return trace handle for extra-top and decl of transformations*/
   static LocalVar* GetTraceTarget();
   static LocalVar* GetTraceTop(const std::string& handleName);
   static LocalVar* GetTraceDecl(LocalVar* top);
   static LocalVar* GetTracePrivate(LocalVar* top);
   static LocalVar* FindTracePrivate(LocalVar* top);
   static LocalVar* GetTraceInclude();
   static LocalVar* FindTraceInclude();
   static LocalVar* GetTraceCleanup(LocalVar* top);
   static LocalVar* FindTraceCleanup(LocalVar* top);

   static LocalVar* DeclareTraceInclude(POETProgram& poet, int& lineNo);

   /*QY: generate commands to start tracing; return the top trace handle */ 
   static LocalVar* GenTraceCommand(POETProgram& poet, 
        const std::vector<LocalVar*>& handles, LocalVar* target, int& lineNo); 
};

/*QY: specification of program transformations to be implemented in POET*/
class OptSpec {
 protected:
   LocalVar* handle;
   std::string handleName;
   OptSpec(LocalVar* _handle);

 protected:
   struct LoopInfo { LocalVar* handle; std::string ivarname;
                     LoopInfo(LocalVar* _handle=0, const std::string& name="")
                      : handle(_handle),ivarname(name) {}
                   };
 public:
  typedef enum {PARLOOP, BLOCK, UNROLL, COPY_ARRAY, BLOCK_COPY_ARRAY, FINITE_DIFF,OPT_ENUM_SIZE} OptEnum;
  typedef enum {OPT_NONE=0,OPT_PAR_LEVEL = 1,OPT_CACHE_LEVEL = 2, OPT_PAR_CACHE_LEVEL=3, OPT_REG_LEVEL = 4, OPT_CACHE_REG_LEVEL=6, OPT_PROC_LEVEL=8, OPT_POST_PAR_LEVEL=16, OPT_CLEANUP_LEVEL=32, OPT_CACHE_CLEANUP_LEVEL=34, OPT_CACHE_REG_CLEANUP_LEVEL=38, OPT_CACHE_PROC_CLEANUP_LEVEL=42, OPT_ALL=63, OPT_LEVEL_MAX=32} OptLevel;
 typedef std::vector<OptSpec*>::const_iterator OptIterator;

  LocalVar* get_handle() const { return handle; }
  std::string get_handleName() const { return handleName; }
  /*QY loop-based strength reduction optimization; return the invocation */
  POETCode* gen_fdInvoke(POETProgram& poet, LocalVar* top,
                   const std::string& nvarName, POETCode* exp,
                   POETCode* expType, const std::vector<POETCode*>& dimVec, 
                   POETCode* permute=0, POETCode* traceMod=0,
                   bool is_scalar=true); 


  virtual ~OptSpec() {}
  virtual OptEnum get_enum() const = 0;
  virtual OptLevel get_opt_level() const = 0;
  virtual std::string get_opt_prefix(OptLevel optLevel) = 0;

  /*QY: insert parameter decl; modify lineNo with new line number */
  virtual void insert_paramDecl(POETProgram& poet, OptLevel optLevel,
                                int& lineNo) {}
  /*QY: return xform declaration; modify lineNo;
        append traceMod with variables that need to be kept track of; */
  virtual void insert_xformDecl(POETProgram& poet, LocalVar* top, POETCode*& traceMod, int& lineNo) {}
  /*QY: return xform evaluation; modify lineNo with new line number */
  virtual POETCode* gen_xformEval(POETProgram& poet, LocalVar* top, 
                   POETCode* traceMod, OptLevel optLevel, int& lineNo) = 0;


  /*QY: return the permutation configuration for strength reduction */
  static POETCode* gen_permute(std::vector<int>& permuteVec);
friend class AutoTuningInterface;

  protected:
   static CodeVar* const Nest;
   static CodeVar* const Loop;
   static CodeVar* const LoopBound;
   static CodeVar* const IntType;
   static POETCode* const Nest_ctrl;
   static POETCode* const Nest_body;
   static POETCode* const Loop_ivar;
   static POETCode* const Loop_step;
   static POETCode* const Loop_ub;
   static POETCode* const LoopBound_ivar;
   static POETCode* const LoopBound_step;
   static XformVar* const appendDecl;

   static XformVar* const finiteDiff;
   static LocalVar* const fd_exp_type;
   static LocalVar* const fd_trace;
   static LocalVar* const fd_traceVar;
   static LocalVar* const fd_traceDecl;
   static LocalVar* const fd_is_scalar;
   static LocalVar* const fd_mod;
   static LocalVar* const fd_permute;
  
};

/*QY: used for specifying non-perfect loops in loop blocking */
struct FuseLoopInfo
{
  typedef std::pair<LoopTreeNode*,int> Entry;
  std::vector<Entry> loops; /*QY: the loops being fused*/
  LoopTreeNode *pivotLoop; /*QY: the pivot loop*/
  
  FuseLoopInfo(LoopTreeNode* _pivot=0) : pivotLoop(_pivot) {}
  static POETCode* toPOET(HandleMap& handleMap, const FuseLoopInfo* info);
};

class BlockSpec;
/*QY: interface for POET-code generation*/
class AutoTuningInterface 
{
   HandleMap handleMap; 
   std::vector<OptSpec*> optvec;
   std::string inputName;
   POETProgram* poet;
   int lineNo;
   LocalVar* target;

   static ArrayAbstractionInterface* arrayInfo;
   static POETCode* arrayAccess;
   static CodeVar* const Array;
   static POETCode* const Array_var;
   static POETCode* const Array_sub;
   static CodeVar* const FunctionCall;
   static POETCode* const FunctionCall_arg;

   void BuildPOETProgram();
   void Gen_POET_opt();
  public:
    AutoTuningInterface(const std::string& _fname) 
       : inputName(_fname), poet(0), lineNo(-1), target(0) {}
    ~AutoTuningInterface() ;

    void set_astInterface(AstInterface& fa);
    static void set_arrayInfo( ArrayAbstractionInterface& arrayInfo);
    static POETCode* CreateArrayRef(POETProgram& poet, POETCode* arr, POETCode* subscript, int dim);
    static POETCode* Access2Subscript(POETProgram& poet, POETCode* ref, int dim);
    static POETCode* Access2Array(POETProgram& poet, POETCode* ref, int dim);

    LocalVar* get_target() { return target; }

    BlockSpec* LoopBlocked(LocalVar* loop); /*QY: whether loop has been blocked*/

    void UnrollLoop(AstInterface& fa, const AstNodePtr& loop, int unrollsize);

    void BlockLoops(LoopTreeNode* outerLoop, LoopTreeNode* innerLoop, LoopBlocking* config, const FuseLoopInfo* nonperfect = 0);

    void ParallelizeLoop(LoopTreeNode* outerLoop, int bsize);

    void CopyArray( CopyArrayConfig& config, LoopTreeNode* repl);


    bool ApplyOpt(LoopTreeNode* root);
    bool ApplyOpt(AstInterface& fa);

    void GenOutput();

};

/*QY: loop unrolling optimization*/
class UnrollSpec : public OptSpec
{
   /*QY: relevant POET invocation names 
         (need to be consistent with POET/lib/opt.pi*/
   static XformVar* const unroll;
   static LocalVar* const unroll_factor;
   static LocalVar* const unroll_cleanup;
   LocalVar* paramVar;

 public:
  UnrollSpec(LocalVar* handle, int unrollSize); 
  virtual OptEnum get_enum() const { return UNROLL; }
  virtual OptLevel get_opt_level() const { return OPT_PROC_LEVEL; }
  virtual std::string get_opt_prefix(OptLevel optLevel) { return "unroll"; }

  /*QY: insert parameter decl; modify lineNo with new line number */
  virtual void insert_paramDecl(POETProgram& poet, OptLevel optLevel,
                                int& lineNo); 

  virtual POETCode* gen_xformEval(POETProgram& poet, LocalVar* top, 
                   POETCode* traceMod, OptLevel optLevel, int& lineNo);

  /*QY: return a unique name that save relevant info. for a loop unroll optimization*/
  static LocalVar* get_unrollSizeVar(const std::string& handleName);
  
};

class ParLoopSpec : public OptSpec
{
  static XformVar* const parloop;
  static LocalVar* const par_trace;
  static LocalVar* const par_thread;
  static LocalVar* const par_private;
  static LocalVar* const par_include;
  POETCode* privateVars;
  POETCode* ivarName, *bvarName;
  LocalVar* parVar, *parblockVar;
 public:
  ParLoopSpec(LocalVar* outerHandle, LoopTreeNode* loop, int bsize);
  virtual OptEnum get_enum() const { return PARLOOP; }
  virtual OptLevel get_opt_level() const { return OPT_PAR_LEVEL; }
  virtual std::string get_opt_prefix(OptLevel optLevel) { return "par"; }

  /*QY: insert parameter decl; modify lineNo with new line number */
  virtual void insert_paramDecl(POETProgram& poet, OptLevel optLevel,
                                int& lineNo); 

  virtual void insert_xformDecl(POETProgram& poet, LocalVar* top, POETCode*& traceMod, int& lineNo); 

  virtual POETCode* gen_xformEval(POETProgram& poet, LocalVar* top, 
                   POETCode* traceMod, OptLevel optLevel, int& lineNo);
};

/*QY: loop blocking optimization*/
class BlockSpec : public OptSpec
{
   /*QY: relevant POET invocation names 
         (need to be consistent with POET/lib/opt.pi*/
   static XformVar* const unrollJam;
   static LocalVar* const unrollJam_cleanup;
   static XformVar* const cleanup;
   static LocalVar* const cleanup_trace;

   static LocalVar* const unrollJam_factor;
   static LocalVar* const unrollJam_trace;

   std::vector<LoopInfo> loopVec; /*QY: the loops to block */ 
   POETCode* nonperfect; /*QY: the non-perfect loops*/
   LocalVar* blockPar, *ujPar;
   HandleMap& handleMap;
   unsigned loopnum;

   /*QY: compute the blocking dimension configuration */
   POETCode* compute_blockDim(LocalVar* paramVar);

 public:
  BlockSpec(HandleMap& handleMap, LocalVar* outerHandle, LoopTreeNode* innerLoop, LoopBlocking* config, const FuseLoopInfo* nonperfect=0); 
  virtual OptEnum get_enum() const { return BLOCK; }
  virtual OptLevel get_opt_level() const { return OPT_CACHE_REG_CLEANUP_LEVEL; }
  virtual std::string get_opt_prefix(OptLevel optLevel) 
   { 
     switch (optLevel) {
       case OPT_CACHE_LEVEL: return "block";
       case OPT_REG_LEVEL: return "unrolljam";
       case OPT_CLEANUP_LEVEL: return "cleanup";
       default: return "";
     }
   }

  unsigned get_loopnum() const { return loopVec.size(); }
  const LoopInfo& get_loop(int i) const { return loopVec[i]; }

  void insert_paramDecl(POETProgram& poet, OptLevel optLevel, int& lineNo);

  /*QY: return xform declaration; modify lineNo;
        append traceMod with variables that need to be kept track of; */
  virtual void insert_xformDecl(POETProgram& poet, LocalVar* top, POETCode*& traceMod, int& lineNo); 

  virtual POETCode* gen_xformEval(POETProgram& poet, LocalVar* top, 
                   POETCode* traceMod, OptLevel optLevel, int& lineNo);

  /*QY: return unique names that save relevant info. for a blocking opt*/
  static LocalVar* get_blockSizeVar(const std::string& handleName);
  static LocalVar* get_unrollJamSizeVar(const std::string& handleName);
  static LocalVar* get_blockDimVar(const std::string& handleName);
  static LocalVar* get_blockTileVar(const std::string& handleName);
  static LocalVar* get_blockSplitVar(const std::string& handleName);

  static POETCode* get_blockSize(const std::string& handleName, int level);
};

/*QY: array copy */
class CopyArraySpec : public OptSpec
{
 protected:
   /*QY: relevant POET invocation names 
         (need to be consistent with POET/lib/opt.pi*/
   static XformVar* const copyarray;
   static LocalVar* const bufname;
   static LocalVar* const init_loc;
   static LocalVar* const save_loc;
   static LocalVar* const delete_loc;
   static LocalVar* const elem_type;
   static LocalVar* const trace_decl;
   static LocalVar* const trace_mod;
   static LocalVar* const trace_vars;
   static LocalVar* const trace;
   static LocalVar* const is_scalar;
   static LocalVar* const block_spec;
   static int index;

   SelectArray sel; /*QY: which array elements to copy*/
   CopyArrayOpt opt; /*QY: alloc/init/save/free configurations */
   std::vector<LoopInfo> loopVec; /*QY: the surrounding loops*/ 

   std::vector<int> placeVec; /* QY: placement of loop dim*/
   POETCode* permute; /* permutation of array dimension*/
   std::string cur_id;

   int get_loopLevel(const SelectArray::ArrayDim& cur);

   /* QY: return the subscript pattern for the array references. 
          beforeCopy: before or after the copy optimization is done */
   POETCode* compute_copySubscript(POETProgram& poet, bool afterCopy);
   /* QY: return a list of copy dimensions */
   POETCode* compute_copyDim(POETProgram& poet, bool scalar);
   /* QY: compute internal info for copy; need to be done in constructor*/
   void compute_config();

   POETCode* gen_copyInvoke(POETProgram& poet, POETCode* cphandle, 
            LocalVar* top, POETCode* cpblock, bool scalar, 
            POETCode* traceMod, int& lineNo);
   void insert_paramDecl(POETProgram& poet, OptLevel optLevel, int& lineNo);
  CopyArraySpec(LocalVar* handle, CopyArrayConfig& config)
     : OptSpec(handle), sel(config.get_arr()), opt(config.get_opt()),permute(0)
       {}

 public:
  CopyArraySpec(HandleMap& handleMap,LocalVar* handle,CopyArrayConfig& config, LoopTreeNode* root);
  virtual OptEnum get_enum() const { return COPY_ARRAY; }
  virtual OptLevel get_opt_level() const { return OPT_REG_LEVEL; }

  /*QY: insert xform declaration; append traceMod with variables that need to be kept track of;
        return the new line number */
  virtual void insert_xformDecl(POETProgram& poet, LocalVar* top, 
                               POETCode*& traceMod, int& lineNo);
  virtual POETCode* gen_xformEval(POETProgram& poet, LocalVar* top, 
                   POETCode* traceMod, OptLevel optLevel, int& lineNo);

  static bool do_scalarRepl(OptLevel optLevel)
  {
    switch (optLevel) {
    case OPT_CACHE_LEVEL: return false;
    case OPT_REG_LEVEL: return true;
    default: assert(0);
    }
  }
  std::string get_opt_prefix(OptLevel optLevel)
  {
    switch (optLevel) {
    case OPT_CACHE_LEVEL: return "copy"+cur_id;
    case OPT_REG_LEVEL: return "scalar"+cur_id;
    default: assert(0);
    }
  }

  /* QY: global variable for tracing array dimension and ref exp */
  LocalVar* get_dimVar(POETProgram& poet, const std::string& arrname);
  LocalVar* get_arrVar(POETProgram& poet, const std::string& arrname);

  /*copy buffer name*/
  std::string get_bufName(const std::string& arrname, bool scalar) 
     { return (scalar)? arrname+"_"+cur_id+"_scalar" : arrname+"_buf"; } 
  /* QY: copy induction variable name */
  std::string get_cpIvarName(const std::string& arrname, int sel_index) 
    {  
       assert( sel_index >= 0);
       std::string copyIvar = handleName +  "_" + cur_id+"_"+arrname + "_cp";
       copyIvar.push_back('0' + sel_index);
       return copyIvar;
    }
  /*QY: declaring copy induction variables*/
  POETCode* gen_cpIvarDecl(POETProgram& poet, LocalVar* top, 
               const std::string& arrname, int dim, bool cpblock);

  /*QY: the copy invocation*/
  POETCode* 
  gen_copyInvoke(POETProgram& poet, POETCode* handle,LocalVar* top, 
        const std::string& arrname, POETCode* arrelemType, CopyArrayOpt opt, 
        POETCode* cpDim, POETCode*  cpblock, bool scalar, POETCode* traceMod);
};

/*QY: array copy associated with blocking */
class BlockCopyArraySpec : public CopyArraySpec
{
   /*QY: relevant POET invocation names 
         (need to be consistent with POET/lib/opt.pi*/

   POETCode* AfterCopy_dimSize(POETProgram& poet);
   POETCode* compute_copyBlock(POETProgram& poet);
    /*QY: return handle for scalar repl*/
   LocalVar* scalarRepl_handle(POETProgram& poet);

  static std::string get_fdName(const std::string& arrname)
        { return arrname+"_fd"; } /*finite difference var name*/

   /*QY:compute strenghth reduction dimension configuration
       : return outermost loop level to start reduction */
   int compute_fdConfig(POETProgram& poet, POETCode* handle, bool scalar,
           std::vector<POETCode*>& expDimVec_cp, /*QY: to go with copying */
           std::vector<POETCode*>& expDimVec_nocp);/*QY: to go without copying*/
 public:
  BlockCopyArraySpec(LocalVar* handle, CopyArrayConfig& config, 
                     BlockSpec& _block);

  virtual OptEnum get_enum() const { return BLOCK_COPY_ARRAY; }
  virtual OptLevel get_opt_level() const { return OPT_CACHE_REG_LEVEL; }
  virtual POETCode* gen_xformEval(POETProgram& poet, LocalVar* top, 
                   POETCode* traceMod, OptLevel optLevel, int& lineNo);
};

#endif
