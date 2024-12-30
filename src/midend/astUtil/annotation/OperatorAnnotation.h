
#ifndef OPERATOR_ANNOTATION_H
#define OPERATOR_ANNOTATION_H

#include "AnnotCollect.h"
#include "AnalysisInterface.h"
#include "AstInterface.h"
#include <iostream>
#include <list>
#include "CommandOptions.h"

template <class Descriptor>
class OperatorAnnotCollection : 
    public AnnotCollectionBase<OperatorDeclaration>,
    public TypeCollection<Descriptor>
{
 public:
  using TypeCollection<Descriptor>::known_type;
  using TypeCollection<Descriptor>::add_annot;
  using TypeCollection<Descriptor>::Dump;
  void read_descriptor( const OperatorDeclaration& target,
                        const std::string&/*annot*/, std::istream& in)
  {
     Descriptor d;
     d.read(in, target);
     add_annot( target, d);
  }
  Descriptor* get_annot_descriptor(const OperatorDeclaration& op, bool insert_if_false = false) {
      DebugLog debugAnnot("-debugannot");
      auto* result = known_type(op);
      if (result == 0 && insert_if_false) {
         Descriptor new_annot;
         new_annot.get_param_decl() = op.get_param_info();
         result = add_annot(op, new_annot);
         assert(result != 0 && result->get_param_decl().num_of_params() == op.get_param_info().num_of_params());
      } else {
         if (result != 0 && 
             result->get_param_decl().num_of_params() < op.get_param_info().num_of_params()) {
            debugAnnot([](){return "Unexpected Error: mismatching parameters and arguments in found annotation! Return 0 to be safe."; });
            // Return 0 to be safe.
            return 0;
         }
      }
      return result;
  }
  template <class CollectObject>
  bool CollectAnnotation(AstInterface& fa, const AstNodePtr& fc,
                CollectObject* collect_f,
                Map2Object<AstInterface*, AstNodePtr, AstNodePtr>* astcodegen = 0) {
    DebugLog debugAnnot("-debugannot");
    AstInterface::AstNodeList args;
    OperatorDeclaration op(fa, fc, &args);
    // QY: This is not supposed to happen, but skip this case for now.
    if (op.get_param_info().num_of_params() != args.size()) 
    {
      debugAnnot([](){return "Error: mismatching parameters and arguments."; });
      return false;
    }
    Descriptor *annot = get_annot_descriptor(op);
    if (annot != 0) {
       debugAnnot([&op](){return "Found recognized operation annotation:" + op.ToString(); });
       if (collect_f != 0) {
          annot->collect(fa, args, *collect_f, astcodegen);
       }
       return true;
    }
    return false;
  }
  bool known_operator( AstInterface& fa, 
                       const AstNodePtr& exp, AstInterface::AstNodeList* argp=0, 
                       Descriptor* desc= 0, bool replpar = false,
                       Map2Object<AstInterface*, AstNodePtr, AstNodePtr>* astcodegen =0) 
  {
     DebugLog debugAnnot("-debugannot");
     AstInterface::AstNodeList args;
     OperatorDeclaration op( fa, exp, &args);
     Descriptor *original_descriptor = get_annot_descriptor(op);
     if (original_descriptor == 0) {
       debugAnnot([&op](){return "Did not recognize operation:" + op.ToString(); });
       return false;
     }
     if (argp != 0)
         *argp = args;
     if (desc != 0) {
         *desc = *original_descriptor;
         if (replpar) {
           ReplaceParams repl = desc->GenReplaceParams( args, astcodegen);
           desc->replace_val( repl);
         }
     }
     debugAnnot([&op](){return "Recognized operation:" + op.ToString(); });
     return true;
   }
};

class OperatorInlineAnnotation
: public OperatorAnnotCollection<OperatorInlineDescriptor>
{
  virtual bool read_annot_name( const std::string& annotName) const
    { return annotName == "inline"; }
  static OperatorInlineAnnotation* inst;
  OperatorInlineAnnotation() {}
 public:
  static OperatorInlineAnnotation* get_inst() 
    { if (inst == 0) inst = new OperatorInlineAnnotation();
      return inst; }
  void Dump() const
    {
      std::cerr << "inline: \n";
      OperatorAnnotCollection<OperatorInlineDescriptor>::Dump();
    }
  bool known_operator( AstInterface& fa, const AstNodePtr& exp, SymbolicVal* val = 0); 
  void register_annot()
    { ReadAnnotation::get_inst()->add_OperatorCollection(this); }
  //Check if an operator has a 'inline {this.operator_2(dim)' record. 
  //If yes, store the semantically equivalent operator into val
  bool get_inline( AstInterface& fa, const AstNodePtr& h, SymbolicVal* val = 0);
};

class OperatorModInfoCollection 
  : public OperatorAnnotCollection<OperatorSideEffectDescriptor>
{
  virtual bool read_annot_name( const std::string& annotname) const
   { return annotname == "modify"; }
 public:
  void Dump() const
    { 
      std::cerr << "modify: \n"; 
      OperatorAnnotCollection<OperatorSideEffectDescriptor>::Dump(); 
    }
};

class OperatorReadInfoCollection
  : public OperatorAnnotCollection<OperatorSideEffectDescriptor>
{
  virtual bool read_annot_name( const std::string& annotname) const
   { return annotname == "read"; }
 public:
  void Dump() const
    {
      std::cerr << "read: \n";
      OperatorAnnotCollection<OperatorSideEffectDescriptor>::Dump();
    }
};

class OperatorCallInfoCollection
  : public OperatorAnnotCollection<OperatorSideEffectDescriptor>
{
  virtual bool read_annot_name( const std::string& annotname) const
   { return annotname == "call"; }
 public:
  void Dump() const
    {
      std::cerr << "call: \n";
      OperatorAnnotCollection<OperatorSideEffectDescriptor>::Dump();
    }
};

class OperatorKillInfoCollection
  : public OperatorAnnotCollection<OperatorSideEffectDescriptor>
{
  virtual bool read_annot_name( const std::string& annotname) const
   { return annotname == "kill"; }
 public:
  void Dump() const
    {
      std::cerr << "kill: \n";
      OperatorAnnotCollection<OperatorSideEffectDescriptor>::Dump();
    }
};


class OperatorSideEffectAnnotation : public FunctionSideEffectInterface
{
  OperatorModInfoCollection modInfo;
  OperatorReadInfoCollection readInfo;
  OperatorCallInfoCollection callInfo;
  OperatorKillInfoCollection killInfo;
  static OperatorSideEffectAnnotation* inst;
  OperatorSideEffectAnnotation() {}
 public:
  typedef OperatorAnnotCollection<OperatorSideEffectDescriptor>::const_iterator const_iterator;

  static OperatorSideEffectAnnotation* get_inst() 
    { if (inst == 0) inst = new OperatorSideEffectAnnotation();
      return inst; }
  void register_annot()
    {
      ReadAnnotation* op = ReadAnnotation::get_inst();
      op->add_OperatorCollection(&modInfo);
      op->add_OperatorCollection(&readInfo);
      op->add_OperatorCollection(&callInfo);
      op->add_OperatorCollection(&killInfo);
    }
  OperatorSideEffectDescriptor* get_modify_descriptor( AstInterface& fa, const AstNodePtr& fc, bool insert_if_missing = false) { 
      return modInfo.get_annot_descriptor(OperatorDeclaration(fa, fc), insert_if_missing); 
   }
  OperatorSideEffectDescriptor* get_modify_descriptor( const std::string& signature, bool insert_if_missing = false) { 
      return modInfo.get_annot_descriptor(OperatorDeclaration(signature), insert_if_missing); 
   }
  bool get_modify( AstInterface& fa, const AstNodePtr& fc, CollectObject<AstNodePtr>* collect = 0) override
  { return modInfo.CollectAnnotation(fa, fc, collect); }

  OperatorSideEffectDescriptor* get_read_descriptor( AstInterface& fa, const AstNodePtr& fc, bool insert_if_missing = false)
  { return readInfo.get_annot_descriptor(OperatorDeclaration(fa, fc), insert_if_missing); }
  OperatorSideEffectDescriptor* get_read_descriptor( const std::string& signature, bool insert_if_missing = false)
  { return readInfo.get_annot_descriptor(OperatorDeclaration(signature), insert_if_missing); }
  bool get_read( AstInterface& fa, const AstNodePtr& fc, CollectObject<AstNodePtr>* collect = 0) override
  { return readInfo.CollectAnnotation(fa, fc, collect); }

  OperatorSideEffectDescriptor* get_call_descriptor( AstInterface& fa, const AstNodePtr& fc, bool insert_if_missing = false)
  { return callInfo.get_annot_descriptor(OperatorDeclaration(fa, fc), insert_if_missing); }
  bool get_call( AstInterface& fa, const AstNodePtr& fc, CollectObject<AstNodePtr>* collect = 0) override
  { return callInfo.CollectAnnotation(fa, fc, collect); }

  OperatorSideEffectDescriptor* get_kill_descriptor( AstInterface& fa, const AstNodePtr& fc, bool insert_if_missing = false)
  { return killInfo.get_annot_descriptor(OperatorDeclaration(fa, fc), insert_if_missing); }
  virtual bool get_kill( AstInterface& fa, const AstNodePtr& fc, CollectObject<AstNodePtr>* collect = 0) 
  { return killInfo.CollectAnnotation(fa, fc, collect); }

  

  void add_modify(const OperatorDeclaration& op, OperatorSideEffectDescriptor& d) {
    modInfo.add_annot( op, d); 
  }
  void add_read(const OperatorDeclaration& op, OperatorSideEffectDescriptor& d) {
    readInfo.add_annot( op, d); 
  }
  void add_call(const OperatorDeclaration& op, OperatorSideEffectDescriptor& d) {
    callInfo.add_annot( op, d); 
  }
  void add_kill(const OperatorDeclaration& op, OperatorSideEffectDescriptor& d) {
    killInfo.add_annot( op, d); 
  }
  void write(std::ostream& out) const {
      modInfo.write(out); readInfo.write(out);
      callInfo.write(out); killInfo.write(out);
  }
  void Dump() const
    { write(std::cerr); }
};

class OperatorAliasCollection 
: public OperatorAnnotCollection <OperatorAliasDescriptor>
{
  virtual bool read_annot_name( const std::string& annotname) const
   { return annotname == "alias"; }
 public:
  void Dump() const
    { std::cerr << "alias: \n"; 
      OperatorAnnotCollection<OperatorAliasDescriptor>::Dump(); }
};

class OperatorAllowAliasCollection
: public OperatorAnnotCollection <OperatorAliasDescriptor>
{
  virtual bool read_annot_name( const std::string& annotname) const
   { return annotname == "allow_alias"; }
 public:
  void Dump() const
    { std::cerr << "allow_alias: \n";
      OperatorAnnotCollection<OperatorAliasDescriptor>::Dump(); }
};

class OperatorAliasAnnotation : public FunctionAliasInterface
{
  OperatorAliasCollection aliasInfo;
  OperatorAllowAliasCollection allowaliasInfo;

  virtual bool read_annot_name( const std::string& annotname) const
   { return annotname == "alias" || annotname == "allow_alias"; }
  static OperatorAliasAnnotation* inst;
  OperatorAliasAnnotation() {}
 public:
  static OperatorAliasAnnotation* get_inst() 
    { if (inst == 0) inst = new OperatorAliasAnnotation();
      return inst; }
  //! Inherited from FunctionAliasInterface    
  bool may_alias(AstInterface& fa, const AstNodePtr& fc, const AstNodePtr& result, 
               CollectObject<std::pair<AstNodePtr,int> >& collectalias);
  //! Inherited from FunctionAliasInterface
  bool allow_alias( AstInterface& fa, const AstNodePtr& fd, 
                    CollectObject<std::pair<AstNodePtr,int> >& collectalias);
  void Dump() const
    { aliasInfo.Dump(); allowaliasInfo.Dump(); }
  void register_annot()
    { ReadAnnotation* op = ReadAnnotation::get_inst();
      op->add_OperatorCollection(&aliasInfo); 
      op->add_OperatorCollection(&allowaliasInfo); }
};
//! Replace an operator with another equivalent operation, which is specified by "inline" annotation
//e.g: operator floatArray::operator() (int index) {  inline { this.elem(index) }; }
//     All ..array(index).. will be replaced by array.elem(index) in the code
class OperatorInlineRewrite : public TransformAstTree
{
 public:
  virtual bool operator()( AstInterface& fa, const AstNodePtr& n,
                           AstNodePtr& result) 
  {
    SymbolicVal r;
    if (OperatorInlineAnnotation::get_inst()->get_inline( fa, n, &r)) {
       result = r.CodeGen(fa);
       return true;
    }
    return false;
  }
  AstNodePtr operator() ( AstInterface& fa, const AstNodePtr& r) 
   {
     return TransformAstTraverse( fa, r, *this);
   }
};

#endif
