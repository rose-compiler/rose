#ifndef REACHING_DEFINITION_H
#define REACHING_DEFINITION_H

#include <assert.h>
#include <string>
#include "DataFlowAnalysis.h"
#include "BitVectorRepr.h"
#include "AnalysisInterface.h"
#include "StmtInfoCollect.h"
#include "AstInterface.h"
// A bit vector representation for reaching definition information
// Each definition has a position in the vector: a name variable may have multiple definition positions
typedef BitVectorRepr ReachingDefinitions;

// A list of <name,data> with indices corresponding offsets in a bit vector
class ReachingDefinitionBase 
 : private BitVectorReprBase<std::string, std::pair<AstNodePtr, AstNodePtr> >
{
  Ast2StringMap scopemap;
 public:
  typedef BitVectorReprBase<std::string, std::pair<AstNodePtr, AstNodePtr> >::iterator iterator;
  // Collecting definition references within 'h', 
  // after considering additional references from an optional node list 'in'
  void collect_refs ( AstInterface& fa, const AstNodePtr& h,
                      FunctionSideEffectInterface* a=0,
                      AstInterface::AstNodeList* in = 0);
  // Insert an entry into the list, the entry is <scope_name+name, variable_definition_info>		      
  void add_ref( const std::string& name, const AstNodePtr& scope, const std::pair<AstNodePtr,AstNodePtr>& def);
  void add_unknown_def ( const std::pair<AstNodePtr,AstNodePtr>& def)
      { add_data( "unknown", def); }
  void finalize() { BitVectorReprBase<std::string, std::pair<AstNodePtr, AstNodePtr> >::finalize(); }
  iterator begin()  const
       { return BitVectorReprBase<std::string, std::pair<AstNodePtr, AstNodePtr> >::begin();}
  iterator end() const
       { return BitVectorReprBase<std::string, std::pair<AstNodePtr, AstNodePtr> >::end();}
  std::pair<AstNodePtr, AstNodePtr> get_ref ( iterator p) const
    { return BitVectorReprBase<std::string, std::pair<AstNodePtr, AstNodePtr> >::get_data(p); }


 friend class ReachingDefinitionGenerator;
};

class  ReachingDefinitionGenerator 
: private BitVectorReprGenerator<std::string, std::pair<AstNodePtr,AstNodePtr> >
{
  Ast2StringMap scopemap;
 public:
  ReachingDefinitionGenerator( const ReachingDefinitionBase& b)
    : BitVectorReprGenerator<std::string, std::pair<AstNodePtr,AstNodePtr> >(b), scopemap(b.scopemap) {}
    
  void add_unknown_def( ReachingDefinitions& gen, 
                        const std::pair<AstNodePtr,AstNodePtr>& def) const
      { add_member( gen, "unknown", def); }
  void add_def( ReachingDefinitions& repr, const std::string& varname, const AstNodePtr& scope, 
                const std::pair<AstNodePtr,AstNodePtr>& def) const;


  ReachingDefinitions get_unknown_defs() const
     { return get_data_set( "unknown" ); }
  ReachingDefinitions get_empty_set() const 
   { return BitVectorReprGenerator<std::string, std::pair<AstNodePtr,AstNodePtr> >::get_empty_set(); }

  // Return a definition bit vector for a specified variable only, zero out all other bits(definition positions).
  ReachingDefinitions get_def_set( const std::string& varname, const AstNodePtr& scope) const;

  void collect_member( const ReachingDefinitions& repr,
                       CollectObject< std::pair<AstNodePtr,AstNodePtr> >& collect) const
    { BitVectorReprGenerator<std::string, std::pair<AstNodePtr,AstNodePtr> >::collect_member(repr, collect); }

  const ReachingDefinitionBase& get_base() const
   { return static_cast<const ReachingDefinitionBase&>
         (BitVectorReprGenerator<std::string, std::pair<AstNodePtr,AstNodePtr> >::get_base()); }

};
//A reaching definition node is kind of data flow nodes with
// bit vectors for definitions
class ReachingDefNode 
: public DataFlowNode<ReachingDefinitions>
{
  ReachingDefinitions gen, notkill, in, out;
 protected:
  void finalize(AstInterface& fa, const ReachingDefinitionGenerator& g, 
                FunctionSideEffectInterface* a = 0, const ReachingDefinitions* in=0);
 public:
  virtual ReachingDefinitions get_entry_data() const 
    { return in; } 
  virtual void set_entry_data(const ReachingDefinitions& _in)  
    { in = _in; }
  virtual ReachingDefinitions get_exit_data() const 
    { return out; }
  virtual void apply_transfer_function() 
    { 
      out = in;
      out &= notkill;
      out |= gen;
    }
  void Dump() const;

  ReachingDefNode( MultiGraphCreate* c)  
          : DataFlowNode<ReachingDefinitions>(c) {}
  ReachingDefinitions get_entry_defs() const { return in; }
  ReachingDefinitions get_exit_defs() const { return out; }
  friend class ReachingDefinitionAnalysis;
};

class ReachingDefinitionAnalysis 
: public DataFlowAnalysis <ReachingDefNode, ReachingDefinitions>
{
  ReachingDefinitionGenerator* g;
  FunctionSideEffectInterface* a;
  AstInterface::AstNodeList pars;

  virtual ReachingDefinitions get_empty_data() const
    { return g->get_empty_set(); }
  virtual ReachingDefinitions 
    meet_data( const ReachingDefinitions& d1, const ReachingDefinitions& d2)
    {  
       ReachingDefinitions result = d1;
       result |= d2; 
       return result;
    }
  virtual void FinalizeCFG( AstInterface& fa);
 public:
  ReachingDefinitionAnalysis() : g(0) {}
  ~ReachingDefinitionAnalysis()
    {
      if (g != 0)
	delete g;
    }
  void operator() ( AstInterface& fa, const AstNodePtr& h, 
                    FunctionSideEffectInterface* anal = 0);
  //Collecting all definition data represented by a bit vector 'repr', saving the data into 'collect'
  void collect_ast( const ReachingDefinitions& repr, 
		    CollectObject< std::pair<AstNodePtr, AstNodePtr> >& collect);

  const ReachingDefinitionGenerator* get_generator() const
    { return g; }
};


#endif
