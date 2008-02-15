
#ifndef SliceOptions_h
#define SliceOptions_h

#include <iostream>
#include <string>
#include <SinglyLinkedList.h>
#include <AstInterface.h>

class ArrangeNestingOrder;
class LoopNestFusion;
class LoopBlockingAnal;
class CopyArrayOperator;
class AstNodePtr;
class LoopTransformInterface;
class LoopTransformOptions 
{
 public:
  class OptRegistryType
        { 
          STD string name, expl;
        public:
          virtual void operator () ( LoopTransformOptions &opt, unsigned &index, 
                                     char* argv[])=0; 
          OptRegistryType( const STD string &s1, const STD string &s2) : name(s1), expl(s2) {}
          STD string GetName() const { return name; }
          STD string GetExpl() const { return expl; }
         };
 private:
  static LoopTransformOptions *inst;

  SinglyLinkedListWrap <OptRegistryType*> reg; 

  ArrangeNestingOrder *icOp;
  LoopNestFusion *fsOp;
  LoopBlockingAnal *bkOp;
  CopyArrayOperator* cpOp;
  unsigned cacheline, reuseDist, splitlimit, defaultblocksize;
  LoopTransformOptions();
  ~LoopTransformOptions();

 public:
  static LoopTransformOptions* GetInstance () ;

  void PrintUsage(STD ostream& stream) const ;
  void RegisterOption( OptRegistryType* t);

  LoopBlockingAnal* GetBlockSel() const  { return bkOp; }
  CopyArrayOperator* GetCopyArraySel() const { return cpOp; }
  ArrangeNestingOrder* GetInterchangeSel() const  { return icOp; }
  LoopNestFusion* GetFusionSel() const { return fsOp; }
  unsigned GetCacheLineSize() const { return cacheline; }
  unsigned GetReuseDistance() const { return reuseDist; }
  unsigned GetTransAnalSplitLimit() const { return splitlimit; }
  unsigned GetDefaultBlockSize() const { return defaultblocksize; }
  void SetDefaultBlockSize(unsigned size) { defaultblocksize = size; }
  bool DoDynamicTuning() const;
  unsigned GetDynamicTuningIndex() const;

  typedef enum {NO_OPT = 0, LOOP_NEST_OPT = 1, INNER_MOST_OPT = 2, MULTI_LEVEL_OPT = 3, LOOP_OPT = 3, DATA_OPT = 4, LOOP_DATA_OPT = 7} OptType;

  OptType GetOptimizationType();
 
  int SetOptions  (int argc, char* argv[]);

  void SetBlockSel( LoopBlockingAnal* sel); 
  void SetCopySel( CopyArrayOperator* sel); 
  void SetInterchangeSel( ArrangeNestingOrder* sel);
  void SetFusionSel( LoopNestFusion* sel);
  void SetCacheLineSize( unsigned sel) { cacheline = sel; }
  void SetReuseDistance( unsigned sel) { reuseDist = sel; }
  void SetTransAnalSplitLimit( unsigned sel) { splitlimit = sel; }
};


#endif


