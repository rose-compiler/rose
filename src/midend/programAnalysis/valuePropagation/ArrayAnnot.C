#include <ArrayAnnot.h>
#include <fstream>
#include <CommandOptions.h>
#include <iostream>

bool DebugArrayAnnot()
{
  static int r = 0;
  if (r == 0) {
     if (CmdOptions::GetInstance()->HasOption("-debugarrayannot"))
         r = 1;
     else
         r = -1;
  }
  return r == 1;
}


bool ArrayShapeDescriptor:: get_dimension( int& val) const
{ 
   SymbolicVal dimval = dimension.get_val();
   return dimval.isConstInt( val);
}


bool ArrayShapeDescriptor::read( std::istream& in)
{
  return ReadContainer<ArrayShapeDescriptor, ReadSymbolicFunctionDeclaration,
                        ';', '{','}'>::
    read(*this, in);
}

bool ArrayElemDescriptor::read( std::istream& in)
{
  return ReadContainer<ArrayElemDescriptor, ReadSymbolicFunctionDeclaration,';', '{','}'>::
    read(*this, in);
}

bool ArrayDescriptor::read( std::istream& in)
{
  return ReadContainer<ArrayDescriptor, ReadSymbolicFunctionDeclaration,';', '{','}'>::
    read(*this, in);
}

void ArrayShapeDescriptor:: push_back( const ReadSymbolicFunctionDeclaration& cur)
  {
    std::string annot = cur.first.first;
    if (annot == "dimension") {
        assert(cur.first.second.size() == 0);
        dimension = cur.second;
    }
    else if (annot == "length") {
        length.push_back( SymbolicFunctionDeclaration( cur.first.second, cur.second) );
    }
    else {
      std::cerr << "Error: non-recognized annotation: " << annot << std::endl;
      ROSE_ABORT();
    }
  }

void ArrayElemDescriptor:: push_back( const ReadSymbolicFunctionDeclaration& cur)
{
  std::string annot = cur.first.first;
  if (annot == "elem") {
    elem.push_back( SymbolicFunctionDeclaration( cur.first.second, cur.second) );
  }
  else{
      std::cerr << "Error: non-recognized annotation: " << annot << std::endl;
      ROSE_ABORT();
    }

}
  
void ArrayDescriptor:: push_back( const ReadSymbolicFunctionDeclaration& cur)
{
  std::string annot = cur.first.first;
  if (annot == "elem") {
     ArrayElemDescriptor::push_back( cur);
  }
  else 
    ArrayShapeDescriptor::push_back(cur) ;
}
void ArrayShapeDescriptor::write( std::ostream& out) const
{
  out << "dimension=";
  dimension.write(out); 
  out << ";  length=" ;
  length.write(out);
}

void ArrayShapeDescriptor :: Dump() const
{
  write(std::cerr);
}

void ArrayElemDescriptor::write( std::ostream& out) const
{
  out << ";  elem=";
  elem.write(out);
}

void ArrayElemDescriptor :: Dump() const
{
  write(std::cerr);
}

void ArrayDescriptor::write( std::ostream& out) const
{
  out << "{";
  ArrayShapeDescriptor::write(out);
  ArrayElemDescriptor::write(out);
  out << " } ";
}

void ArrayDescriptor :: Dump() const
{
  write(std::cerr);
}

void ArrayDefineDescriptor ::
replace_var(  const std::string& varname, const SymbolicVal& repl)
{ 
  ArrayDescriptor::replace_var( varname, repl);
  reshape.replace_var( varname, repl);
}

void ArrayDefineDescriptor ::
replace_val(MapObject<SymbolicVal, SymbolicVal>& repl)
{ 
  ArrayDescriptor :: replace_val(repl);
  reshape.replace_val(repl);
}

void ArrayDefineDescriptor::push_back( const ReadSymbolicFunctionDeclaration& cur)
{
  std::string annot = cur.first.first;
  if (annot == "reshape")  {
    if ( cur.first.second.size() > 1) {
      cur.Dump();
      ROSE_ABORT();
    }
    reshape.push_back( SymbolicFunctionDeclaration( cur.first.second, cur.second) );
  }
  else
     ArrayDescriptor::push_back(cur);
}

bool ArrayDefineDescriptor :: read( std::istream& in)
{
  return ReadContainer< ArrayDefineDescriptor, ReadSymbolicFunctionDeclaration,';', '{','}'>::
               read(*this, in);
}

void ArrayDefineDescriptor :: write( std::ostream& out) const
{ 
  ArrayDescriptor::write(out);
  out << "reshape = ";
  reshape.write(out);
}

void ArrayDefineDescriptor :: Dump() const
{
  write(std::cerr);
}


bool ArrayOptDescriptor :: read( std::istream& in)
{
  read_ch(in, '{');
  if (peek_id(in) == "define") { 
     read_id(in,"define");
     defs.read(in);
  }
  return ReadContainer< ArrayDescriptor, ReadSymbolicFunctionDeclaration,';', 0,'}'>::
               read(*this, in);
}

void ArrayOptDescriptor :: write( std::ostream& out) const
{ 
  defs.write(out);
  ArrayDescriptor::write(out);
}

void ArrayOptDescriptor :: 
replace_var(  const std::string& varname, const SymbolicVal& repl)
{ 
  ArrayDescriptor::replace_var( varname, repl);
  for (DefContainer::iterator p = defs.begin();
       p != defs.end(); ++p) {
    DefineVariableDescriptor& cur = *p;
    cur.replace_var(varname, repl);
  }
}

void ArrayOptDescriptor :: 
replace_val(MapObject<SymbolicVal, SymbolicVal>& repl)
{ 
  ArrayDescriptor :: replace_val(repl);
  for (DefContainer::iterator p = defs.begin();
       p != defs.end(); ++p) {
    DefineVariableDescriptor& cur = *p;
    cur.replace_val(repl);
  }
}

void ArrayOptDescriptor :: Dump() const
{
  write(std::cerr);
}

ArrayAnnotation* ArrayAnnotation::inst = 0;
ArrayAnnotation* ArrayAnnotation::get_inst()
{
  if (inst == 0)
    inst = new ArrayAnnotation();
  return inst;
}

void ArrayAnnotation:: register_annot()
{
   OperatorSideEffectAnnotation::get_inst()->register_annot();
   OperatorInlineAnnotation::get_inst()->register_annot();
   OperatorAliasAnnotation::get_inst()->register_annot();

   ValueAnnotation::get_inst()->register_annot();
   ReadAnnotation* op = ReadAnnotation::get_inst();
   op->add_TypeCollection(&arrays);
   op->add_TypeCollection(&unique_arrays);
   op->add_TypeCollection(&arrayopt);
   op->add_OperatorCollection(&arrayConstruct);
   op->add_OperatorCollection(&arrayModify);
}

void ArrayAnnotation :: Dump() const
{
   OperatorSideEffectAnnotation::get_inst()->Dump();
   OperatorAliasAnnotation::get_inst()->Dump();

   ValueAnnotation::get_inst()->Dump();
   arrays.Dump();
   arrayopt.Dump();
   arrayConstruct.Dump();
   arrayModify.Dump();
}

bool ArrayAnnotation ::
known_array_type( CPPAstInterface& fa, const AstNodeType& type, ArrayDefineDescriptor* r)
{
  return arrays.known_type( fa, type, r);
}


bool ArrayAnnotation :: 
known_unique_array( CPPAstInterface& fa, const AstNodePtr& array, ArrayDefineDescriptor* d)
{
   std::string varname; 
   AstNodePtr scope;
   if (!fa.IsVarRef(array,0,&varname,&scope, 0, /*use_global_unique_name=*/true)) return false;
   if (unique_arrays.known_type(varname, d)) {
      return true;
   }
  return false;
}

bool ArrayAnnotation ::
known_array( CPPAstInterface& fa, const AstNodePtr& array, ArrayDefineDescriptor* r)
{
  if (arrays.known_type( fa, array, r)) {
    if (DebugArrayAnnot()) 
       std::cerr << "Recognized array: " << AstInterface::AstToString(array) << "\n";
    return true;
  }
  if (DebugArrayAnnot()) 
       std::cerr << "Not recognizing array: " << AstInterface::AstToString(array) << "\n";
  return false;
}

bool ArrayAnnotation ::
has_array_opt( CPPAstInterface& fa, const AstNodePtr array, ArrayOptDescriptor* r)
{
  return arrayopt.known_type( fa, array, r);
}

bool ArrayAnnotation::
is_array_mod_op( CPPAstInterface& fa, const AstNodePtr& arrayExp,
                 AstNodePtr* arrayp, ArrayDescriptor* descp, bool* reshape,
                 ReplaceParams* /*repl*/)
{
  CPPAstInterface::AstNodeList args;
  ArrayModifyDescriptor desc;
  if (!arrayModify.known_operator( fa, arrayExp, &args, &desc, true)) {
     if (DebugArrayAnnot()) {
        std::cerr << "NOT mod-array operator: ";
        std::cerr << AstInterface::AstToString(arrayExp);
        std::cerr << std::endl;
     }
    return false;
  }
  if (arrayp != 0) {
    if (!desc.first.get_val().isAstWrap(*arrayp))
       ROSE_ABORT( );
  }
  if (descp != 0) {
    *descp = desc.second;
  }

  if (reshape != 0) {
    *reshape = !desc.second.get_dimension().is_top() || desc.second.get_length().size();
  }

  if (DebugArrayAnnot()) {
     std::cerr << "recognized mod-array operator: ";
     std::cerr << AstInterface::AstToString(arrayExp);
     std::cerr << std::endl;

     return true;
  }
  return true;
}

bool ArrayAnnotation::
is_array_construct_op( CPPAstInterface& fa, const AstNodePtr& arrayExp, CPPAstInterface::AstNodeList* alias,
                       ArrayDescriptor* descp, ReplaceParams* /*repl*/)
{
  CPPAstInterface::AstNodeList args;
  ArrayConstructDescriptor desc;
  if (!arrayConstruct.known_operator( fa, arrayExp, &args, &desc, true))
    return false;

  if (descp != 0) {
    *descp = desc.second;
  }
  if (alias != 0) {
    for (std::list<SymbolicValDescriptor>::const_iterator p = desc.first.begin(); 
         p != desc.first.end(); ++p) {
       SymbolicValDescriptor cur = *p;
       AstNodePtr curarg;
       if (!cur.get_val().isAstWrap(curarg))
           ROSE_ABORT();
       alias->push_back(curarg.get_ptr());
    }
  }
  return true;
}

AstNodePtr ArrayAnnotation::
create_access_array_elem( CPPAstInterface& fa, const AstNodePtr& array,
                          const AstInterface::AstNodeList& args)
{
  return arrays.create_known_member_function( fa, array, "elem", args);
}

SymbolicVal  ArrayAnnotation ::
create_access_array_elem( const AstNodePtr& array, const SymbolicFunction::Arguments& args)
{
  return arrays.create_known_member_function( array, "elem", args);
}

AstNodePtr ArrayAnnotation ::
create_reshape_array( CPPAstInterface& fa, const AstNodePtr& array,
                          const CPPAstInterface::AstNodeList& args)
{
  return arrays.create_known_member_function( fa, array, "reshape", args);
}

SymbolicVal ArrayAnnotation::
create_access_array_length( const AstNodePtr& array, const SymbolicVal& dim)
{
  SymbolicFunction::Arguments args;
  args.push_back( dim);
  return arrays.create_known_member_function( array, "length", args);
}

AstNodePtr ArrayAnnotation::
create_access_array_length( CPPAstInterface& fa, const AstNodePtr& array, 
                            int dim)
{
  CPPAstInterface::AstNodeList args;
  args.push_back( fa.CreateConstInt(dim).get_ptr());
  return arrays.create_known_member_function( fa, array, "length", args);
}

bool ArrayAnnotation ::
is_access_array_length( CPPAstInterface& fa, const SymbolicVal& orig, 
                         AstNodePtr* arrayp, SymbolicVal *dim)
{
  SymbolicFunction::Arguments args;
  if (arrays.is_known_member_function( fa,orig, arrayp, &args) == "length") {
     assert(args.size() == 1);
     if (dim != 0)
        *dim = args.front();
     if (DebugArrayAnnot()) {
        std::cerr << "Recognized access-array-length operator: " << orig.toString() << "\n";
        }
     return true;
  }
  if (DebugArrayAnnot()) {
        std::cerr << "NOT access-array-length operator: " << orig.toString();
        std::cerr << std::endl;
  }
  return false;
}

bool ArrayAnnotation ::
is_access_array_length( CPPAstInterface& fa, const AstNodePtr& orig, AstNodePtr* arrayp,
                        AstNodePtr* dimAst, int *dim)
{
   CPPAstInterface::AstNodeList args;
   if (arrays.is_known_member_function( fa, orig, arrayp, &args) == "length") {
      assert (args.size() == 1);
      AstNodePtr cur = *args.begin();
      if (dimAst != 0)
            *dimAst = cur;
      if (dim != 0) {
          if (!fa.IsConstInt(cur, dim))
              ROSE_ABORT();
      }
      if (DebugArrayAnnot()) {
        std::cerr << "Recognized access-array-length operator: ";
        std::cerr << AstInterface::AstToString(orig);
        std::cerr << std::endl;
      }
      return true;
   }
  if (DebugArrayAnnot()) {
        std::cerr << "NOT access-array-length operator: ";
        std::cerr << AstInterface::AstToString(orig);
        std::cerr << std::endl;
  }
   return false;
}

bool ArrayAnnotation ::
is_access_array_elem( CPPAstInterface& fa, const SymbolicVal& orig, AstNodePtr* array, SymbolicFunction::Arguments* args)
{
  if (arrays.is_known_member_function( fa, orig, array, args) == "elem") {
     if (DebugArrayAnnot()) {
        std::cerr << "Recognized access-array-elem operator: " << orig.toString();
        std::cerr << std::endl;
     }
    return true;
  }
  if (DebugArrayAnnot()) {
        std::cerr << "NOT access-array-elem operator: " << orig.toString();
        std::cerr << std::endl;
  }
  return false;
}

bool ArrayAnnotation ::
is_access_array_elem( CPPAstInterface& fa, const AstNodePtr& orig, 
                      AstNodePtr* arrayp,
                      CPPAstInterface::AstNodeList* args)
{
  if (arrays.is_known_member_function( fa, orig, arrayp, args) == "elem") {
     if (DebugArrayAnnot()) {
        std::cerr << "Recognized access-array-elem operator: " << AstInterface::AstToString(orig);
        std::cerr << std::endl;
     }
    return true;
  }
  if (DebugArrayAnnot()) {
        std::cerr << "NOT access-array-elem operator: " << AstInterface::AstToString(orig);
        std::cerr << std::endl;
  }
  return false;
}

bool ArrayAnnotation ::
is_reshape_array( CPPAstInterface& fa, const AstNodePtr& orig, 
                  AstNodePtr* arrayp,
                  CPPAstInterface::AstNodeList* args)
{
  return arrays.is_known_member_function( fa, orig, arrayp, args) == "reshape" ;
}

bool ArrayAnnotation ::
may_alias(AstInterface& _fa, const AstNodePtr& fc, const AstNodePtr& result,
                         CollectObject< std::pair<AstNodePtr, int> >& collectalias)
{
  CPPAstInterface& fa = static_cast<CPPAstInterface&>(_fa);
  if (is_access_array_elem( fa, fc) || is_access_array_length(fa, fc) ||
      is_reshape_array( fa, fc))
      return true;
  return OperatorAliasAnnotation::get_inst()->may_alias(fa, fc, result, collectalias);
}

bool ArrayAnnotation ::
allow_alias(AstInterface& fa, const AstNodePtr& fc, 
                         CollectObject< std::pair<AstNodePtr, int> >& collectalias)
{
  return OperatorAliasAnnotation::get_inst()->allow_alias(fa, fc, collectalias);
}

bool ArrayAnnotation ::
get_modify(AstInterface& _fa, const AstNodePtr& fc,
                               CollectObject<AstNodePtr>* collect)
{ CPPAstInterface& fa = static_cast<CPPAstInterface&>(_fa);
   if ( is_access_array_elem(fa, fc) || is_access_array_length(fa, fc)) 
      return true;
   AstNodePtr array;
   if (is_reshape_array( fa,fc, &array)) {
      if (collect != 0)
         (*collect)(array);
      return true;
   } 
   return OperatorSideEffectAnnotation::get_inst()->get_modify(fa, fc, collect);
}

bool ArrayAnnotation ::
get_read(AstInterface& _fa, const AstNodePtr& fc, CollectObject<AstNodePtr>* collect)
{
  CPPAstInterface& fa = static_cast<CPPAstInterface&>(_fa);
   AstNodePtr dim;
   if (is_access_array_length( fa, fc, 0, &dim)) {
       if (collect != 0)
           (*collect)(dim);
       return true;
   }
   CPPAstInterface::AstNodeList args;
   if (is_access_array_elem( fa, fc, 0, &args)) {
      if (collect != 0) {
           for (CPPAstInterface::AstNodeList::iterator p = args.begin();
                  p != args.end(); ++p) 
              (*collect)(*p);
      }
      return true;
   }
   return OperatorSideEffectAnnotation::get_inst()->get_read(fa, fc, collect);
}


#define TEMPLATE_ONLY
#include <TypeAnnotation.C>
#include <OperatorDescriptors.h>
template class TypeCollection<ArrayModifyDescriptor>;
template class TypeCollection<ArrayConstructDescriptor>;
template class OperatorAnnotCollection<ArrayModifyDescriptor>;
template class OperatorAnnotCollection<ArrayConstructDescriptor>;
template class TypeAnnotCollection<ArrayDefineDescriptor>;
template class TypeAnnotCollection<ArrayOptDescriptor>;
#include <AnnotDescriptors.C>
template struct CollectPair<TypeDescriptor, ArrayDescriptor, 0>;
