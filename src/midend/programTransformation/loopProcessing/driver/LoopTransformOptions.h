
#ifndef SliceOptions_h
#define SliceOptions_h

#include <iostream>
#include <string>
#include <vector>
#include <SinglyLinkedList.h>
#include <AstInterface.h>

class ArrangeNestingOrder;
class LoopNestFusion;
class LoopBlocking;
class LoopPar;
class CopyArrayOperator;
class AstNodePtr;
class LoopTransformInterface;
class LoopTransformOptions 
{
 public:
  class OptRegistryType
        { 
          std::string name, expl;
        public:
          virtual void operator ()(LoopTransformOptions &opt, unsigned& index, 
                                   const std::vector<std::string>& argv)=0; 
          OptRegistryType( const std::string &s1, const std::string &s2) : name(s1), expl(s2) {}
          std::string GetName() const { return name; }
          std::string GetExpl() const { return expl; }
          virtual ~OptRegistryType() {}
         };
 private:
  static LoopTransformOptions *inst;

  SinglyLinkedListWrap <OptRegistryType*> reg; 

  ArrangeNestingOrder *icOp;
  LoopNestFusion *fsOp;
  LoopBlocking *bkOp;
  LoopPar * parOp;
  CopyArrayOperator* cpOp;
  unsigned cacheline, reuseDist, splitlimit, defaultblocksize, parblocksize;
  LoopTransformOptions();
  ~LoopTransformOptions();

 public:
  static LoopTransformOptions* GetInstance () ;

  void PrintUsage(std::ostream& stream) const ;
  void RegisterOption( OptRegistryType* t);

  LoopBlocking* GetBlockSel() const  { return bkOp; }
  LoopPar* GetParSel() const  { return parOp; }
  CopyArrayOperator* GetCopyArraySel() const { return cpOp; }
  ArrangeNestingOrder* GetInterchangeSel() const  { return icOp; }
  LoopNestFusion* GetFusionSel() const { return fsOp; }
  unsigned GetCacheLineSize() const { return cacheline; }
  unsigned GetReuseDistance() const { return reuseDist; }
  unsigned GetTransAnalSplitLimit() const { return splitlimit; }
  unsigned GetDefaultBlockSize() const { return defaultblocksize; }
  unsigned GetParBlockSize() const { return parblocksize; }
  void SetDefaultBlockSize(unsigned size) { defaultblocksize = size; }
  void SetParBlockSize(unsigned size) { parblocksize = size; }
  bool DoDynamicTuning() const;
  unsigned GetDynamicTuningIndex() const;

  typedef enum {NO_OPT = 0, LOOP_NEST_OPT = 1, INNER_MOST_OPT = 2, MULTI_LEVEL_OPT = 3, LOOP_OPT = 3, DATA_OPT = 4, LOOP_DATA_OPT = 7, PAR_OPT=8, PAR_LOOP_OPT=11, PAR_LOOP_DATA_OPT=15} OptType;

  OptType GetOptimizationType();
 
  void SetOptions  (const std::vector<std::string>& argvList, std::vector<std::string>* known_opt=0);

  void SetBlockSel( LoopBlocking* sel); 
  void SetParSel( LoopPar* sel); 
  void SetCopySel( CopyArrayOperator* sel); 
  void SetInterchangeSel( ArrangeNestingOrder* sel);
  void SetFusionSel( LoopNestFusion* sel);
  void SetCacheLineSize( unsigned sel) { cacheline = sel; }
  void SetReuseDistance( unsigned sel) { reuseDist = sel; }
  void SetTransAnalSplitLimit( unsigned sel) { splitlimit = sel; }
};


#endif


