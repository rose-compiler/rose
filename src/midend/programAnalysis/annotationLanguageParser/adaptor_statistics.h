#ifndef BDWY_ADAPTOR_STATISTICS_H
#define BDWY_ADAPTOR_STATISTICS_H

#include "broadway.h"

class Adaptor_Statistics {

  typedef enum { Complicit = 0x1,
                 Control_flow = 0x2,
                 Parameter_pass = 0x4,
                 Weak_update = 0x8,
                 Additive = 0x10 } ReasonKind;

  typedef pair<procedureInfo*,memoryBlock*>   CS_Block;
  typedef map<procedureInfo*,ReasonKind>      CS_Reasons;
  typedef map<CS_Block,ReasonKind>            CS_Block_Reasons;
  typedef map<memoryBlock*,ReasonKind>        FS_Reasons;

  typedef set<Location*>                      Locations;
  typedef map<memoryBlock*,Locations>         Destructions;

  typedef pair<Location*,memoryBlock*>        Complicit_assign;
  typedef set<Complicit_assign>               Complicit_assigns;
  typedef map<memoryBlock*,Complicit_assigns> Complicits;

  typedef map<procedureInfo*,memoryblock_set> CS_blocks;

  typedef map<memoryBlock*,basicblockNode*>   CF_Sites;

  // FS
  memoryblock_set   fs_blocks;
  FS_Reasons        fs_reasons;
  Destructions      fs_destructions;
  Complicits        fs_complicits;

  // CS
  procedureinfo_set cs_procs;
  CS_Reasons        cs_reasons;
  CS_Block_Reasons  cs_block_reasons;
  CS_blocks         cs_blocks;
  Destructions      cs_destructions;
  Complicits        cs_complicits;

  typedef pair<int,int>                         int_pair;
  typedef set<unsigned long>                    enumvalue_sets;
  typedef map<int_pair, enumvalue_sets>         Arg_Property;
  typedef map<stmtLocation*,Arg_Property>       Site_Property;
  typedef map<procedureInfo*,Site_Property>     Proc_Site_Property;
  typedef set<stmtLocation*>                    stmtLocations;
  typedef pair<int_pair, enumvalue_sets>        Arg_Property_Pair;
  typedef map<Arg_Property_Pair,stmtLocations>  Site_Partition;
  
  Proc_Site_Property proc_site_property;

  // control-flow
  memoryblock_set   cf_blocks;
  CF_Sites          cf_sites;

  //REF
  Annotations *_ann;
  Analyzer    *_analyzer;

public:
  Adaptor_Statistics(Annotations *ann, Analyzer *analyzer)
  : _ann(ann), _analyzer(analyzer) {}

  void make_CS(memoryBlock *block, procedureInfo *proc, 
               Location *site,
               memoryBlock::DestructiveKind reason);

  void make_CS(memoryBlock *block, procedureInfo *proc, 
               Location *site,
               memoryBlock *complicit);

  void print_statistics(ostream & out);

  // void reset();

private:
  void print_cs_statistics(ostream & out);
  void print_fs_statistics(ostream & out);
  void print_cf_statistics(ostream & out);
  void print_unify_statistics(ostream & out);

  void print_block_chain(memoryBlock *block, Destructions & destructions,
                         Complicits & complicits,
                         memoryblock_set & printed,
                         string indent, string block_indent,
                         ostream & out);

  void print_call_args(stmtLocation *site, procedureInfo *caller,
                       procedureInfo *callee, int arg, declNode *formal,
                       ostream & out);
  void print_call_arg(declNode *formal, int arg, operandNode *parameter,
                      int dereference, pointerValue & parameter_value,
                      stmtLocation *site, procedureInfo *caller,
                      procedureInfo *callee, const enum_property_map & props,
                      ostream & out);

  string reason_str(ReasonKind);
};

#endif
