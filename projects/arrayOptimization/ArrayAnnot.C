// DQ (1/1/2006): This is OK if not declared in a header file
using namespace std;

#include <general.h>
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
   return dimval.ToInt( val);
}


bool ArrayShapeDescriptor::read( istream& in)
{
  return ReadContainer<ArrayShapeDescriptor, ReadSymbolicFunctionDeclaration,
                        ';', '{','}'>::
    read(*this, in);
}

bool ArrayElemDescriptor::read( istream& in)
{
  return ReadContainer<ArrayElemDescriptor, ReadSymbolicFunctionDeclaration,';', '{','}'>::
    read(*this, in);
}

bool ArrayDescriptor::read( istream& in)
{
  return ReadContainer<ArrayDescriptor, ReadSymbolicFunctionDeclaration,';', '{','}'>::
    read(*this, in);
}

void ArrayShapeDescriptor:: push_back( const ReadSymbolicFunctionDeclaration& cur)
  {
    string annot = cur.first.first;
    if (annot == "dimension") {
        assert(cur.first.second.size() == 0);
        dimension = cur.second;
    }
    else if (annot == "length") {
        length.push_back( SymbolicFunctionDeclaration( cur.first.second, cur.second) );
    }
    else {
      cerr << "Error: non-recognized annotation: " << annot << endl;
      assert(false);
    }
  }

void ArrayElemDescriptor:: push_back( const ReadSymbolicFunctionDeclaration& cur)
{
  string annot = cur.first.first;
  if (annot == "elem") {
    elem.push_back( SymbolicFunctionDeclaration( cur.first.second, cur.second) );
  }
  else{
      cerr << "Error: non-recognized annotation: " << annot << endl;
      assert(false);
    }

}
  
void ArrayDescriptor:: push_back( const ReadSymbolicFunctionDeclaration& cur)
{
  string annot = cur.first.first;
  if (annot == "elem") {
     ArrayElemDescriptor::push_back( cur);
  }
  else 
    ArrayShapeDescriptor::push_back(cur) ;
}
void ArrayShapeDescriptor::write( ostream& out) const
{
  out << "dimension=";
  dimension.write(out); 
  out << ";  length=" ;
  length.write(out);
}

void ArrayShapeDescriptor :: Dump() const
{
  write(cerr);
}

void ArrayElemDescriptor::write( ostream& out) const
{
  out << ";  elem=";
  elem.write(out);
}

void ArrayElemDescriptor :: Dump() const
{
  write(cerr);
}

void ArrayDescriptor::write( ostream& out) const
{
  out << "{";
  ArrayShapeDescriptor::write(out);
  ArrayElemDescriptor::write(out);
  out << " } ";
}

void ArrayDescriptor :: Dump() const
{
  write(cerr);
}

void ArrayDefineDescriptor ::
replace_var(  const string& varname, const SymbolicVal& repl)
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
  string annot = cur.first.first;
  if (annot == "reshape")  {
    if ( cur.first.second.size() > 1) {
      cur.Dump();
      assert(false);
    }
    reshape.push_back( SymbolicFunctionDeclaration( cur.first.second, cur.second) );
  }
  else
     ArrayDescriptor::push_back(cur);
}

bool ArrayDefineDescriptor :: read( istream& in)
{
  return ReadContainer< ArrayDefineDescriptor, ReadSymbolicFunctionDeclaration,';', '{','}'>::
               read(*this, in);
}

void ArrayDefineDescriptor :: write( ostream& out) const
{ 
  ArrayDescriptor::write(out);
  out << "reshape = ";
  reshape.write(out);
}

void ArrayDefineDescriptor :: Dump() const
{
  write(cerr);
}


bool ArrayOptDescriptor :: read( istream& in)
{
  read_ch(in, '{');
  if (peek_id(in) == "define") { 
     read_id(in,"define");
     defs.read(in);
  }
  return ReadContainer< ArrayDescriptor, ReadSymbolicFunctionDeclaration,';', 0,'}'>::
               read(*this, in);
}

void ArrayOptDescriptor :: write( ostream& out) const
{ 
  defs.write(out);
  ArrayDescriptor::write(out);
}

void ArrayOptDescriptor :: 
replace_var(  const string& varname, const SymbolicVal& repl)
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
  write(cerr);
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
   op->add_TypeCollection(&arrayopt);
   op->add_OperatorCollection(&arrayConstruct);
   op->add_OperatorCollection(&arrayModify);
}

void ArrayAnnotation :: Dump() const
{
   OperatorSideEffectAnnotation::get_inst()->Dump();
   ValueAnnotation::get_inst()->Dump();
   arrays.Dump();
   arrayopt.Dump();
   arrayConstruct.Dump();
   arrayModify.Dump();
}

bool ArrayAnnotation ::
known_array_type( const AstNodeType& type, ArrayDefineDescriptor* r)
{
  return arrays.known_type( type, r);
}

bool ArrayAnnotation ::
known_array( const AstNodePtr& array, ArrayDefineDescriptor* r)
{
  return arrays.known_type( array, r);
}

bool ArrayAnnotation ::
has_array_opt( const AstNodePtr array, ArrayOptDescriptor* r)
{
  return arrayopt.known_type( array, r);
}

bool ArrayAnnotation::
is_array_mod_op( const AstNodePtr& arrayExp,
                 AstNodePtr* arrayp, ArrayDescriptor* descp, bool* reshape,
                 ReplaceParams* repl)
{
  AstInterface::AstNodeList args;
  ArrayModifyDescriptor desc;
  if (!arrayModify.known_operator( arrayExp, &args, &desc, true))
    return false;

  if (arrayp != 0) {
    *arrayp = desc.first.get_val().ToAst();
    assert( *arrayp != 0);
  }
  if (descp != 0) {
    *descp = desc.second;
  }

  if (reshape != 0) {
    *reshape = !desc.second.get_dimension().is_top() || desc.second.get_length().size();
  }

  if (DebugArrayAnnot()) {
     cerr << "recognized mod-array operator: ";
     arrayExp.Dump();
     cerr << endl;
  }
  return true;
}

bool ArrayAnnotation::
is_array_construct_op( const AstNodePtr& arrayExp, AstInterface::AstNodeList* alias,
                       ArrayDescriptor* descp, ReplaceParams* repl)
{
  AstInterface::AstNodeList args;
  ArrayConstructDescriptor desc;
  if (!arrayConstruct.known_operator( arrayExp, &args, &desc, true))
    return false;

  if (descp != 0) {
    *descp = desc.second;
  }
  if (alias != 0) {
    for (list<SymbolicValDescriptor>::const_iterator p = desc.first.begin(); 
         p != desc.first.end(); ++p) {
       SymbolicValDescriptor cur = *p;
       AstNodePtr curarg = cur.get_val().ToAst();
       assert( curarg != 0);
       assert( curarg != 0);
       AstInterfaceBase::ListAppend(*alias, curarg);
    }
  }
  return true;
}

AstNodePtr ArrayAnnotation::
create_access_array_elem( AstInterface& fa, const AstNodePtr& array,
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
create_reshape_array( AstInterface& fa, const AstNodePtr& array,
			  const AstInterface::AstNodeList& args)
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
create_access_array_length( AstInterface& fa, const AstNodePtr& array, 
			    int dim)
{
  AstInterface::AstNodeList args = fa.CreateList();
  fa.ListAppend( args, fa.CreateConstInt(dim));
  return arrays.create_known_member_function( fa, array, "length", args);
}

bool ArrayAnnotation ::
is_access_array_length( const SymbolicVal& orig, AstNodePtr* arrayp, SymbolicVal *dim)
{
  SymbolicFunction::Arguments args;
  if (arrays.is_known_member_function( orig, arrayp, &args) == "length") {
     assert(args.size() == 1);
     if (dim != 0)
        *dim = args[0];
     return true;
  }
  return false;
}

bool ArrayAnnotation ::
is_access_array_length( const AstNodePtr& orig, AstNodePtr* arrayp,
                        AstNodePtr* dimAst, int *dim)
{
   AstInterface::AstNodeList args;
   if (arrays.is_known_member_function( orig, arrayp, &args) == "length") {
      assert (args.size() == 1);
      AstNodePtr cur = *AstInterface::GetAstNodeListIterator(args);
      if (dimAst != 0)
            *dimAst = cur;
      if (dim != 0) {
          if (!AstInterface::IsConstInt(cur, dim))
              assert(false);
      }
      return true;
   }
   return false;
}

bool ArrayAnnotation ::
is_access_array_elem( const SymbolicVal& orig, AstNodePtr* array, SymbolicFunction::Arguments* args)
{
  if (arrays.is_known_member_function( orig, array, args) == "elem") {
    return true;
  }
  return false;
}

bool ArrayAnnotation ::
is_access_array_elem( const AstNodePtr& orig, AstNodePtr* arrayp,
                      AstInterface::AstNodeList* args)
{
  if (arrays.is_known_member_function( orig, arrayp, args) == "elem") {
    return true;
  }
  return false;
}

bool ArrayAnnotation ::
is_reshape_array( const AstNodePtr& orig, AstNodePtr* arrayp,
                  AstInterface::AstNodeList* args)
{
  return arrays.is_known_member_function( orig, arrayp, args) == "reshape" ;
}

bool ArrayAnnotation ::
may_alias(AstInterface& fa, const AstNodePtr& fc, const AstNodePtr& result,
                         CollectObject< pair<AstNodePtr, int> >& collectalias)
{
  if (is_access_array_elem( fc) || is_access_array_length(fc) ||
      is_reshape_array( fc))
      return true;
  return OperatorAliasAnnotation::get_inst()->may_alias(fa, fc, result, collectalias);
}

bool ArrayAnnotation ::
allow_alias(AstInterface& fa, const AstNodePtr& fc, 
                         CollectObject< pair<AstNodePtr, int> >& collectalias)
{
  return OperatorAliasAnnotation::get_inst()->allow_alias(fa, fc, collectalias);
}

bool ArrayAnnotation ::
get_modify(AstInterface& fa, const AstNodePtr& fc,
                               CollectObject<AstNodePtr>* collect)
{
   if ( is_access_array_elem( fc) || is_access_array_length( fc)) 
      return true;
   AstNodePtr array;
   if (is_reshape_array( fc, &array)) {
      if (collect != 0)
         (*collect)(array);
      return true;
   } 
   return OperatorSideEffectAnnotation::get_inst()->get_modify(fa, fc, collect);
}

bool ArrayAnnotation ::
get_read(AstInterface& fa, const AstNodePtr& fc, CollectObject<AstNodePtr>* collect)
{
   AstNodePtr dim;
   if (is_access_array_length( fc, 0, &dim)) {
       if (collect != 0)
           (*collect)(dim);
       return true;
   }
   AstInterface::AstNodeList args;
   if (is_access_array_elem( fc, 0, &args)) {
      if (collect != 0) {
           for (AstInterface::AstNodeListIterator p =
                  fa.GetAstNodeListIterator(args); !p.ReachEnd(); ++p) 
              (*collect)(*p);
      }
      return true;
   }
   return OperatorSideEffectAnnotation::get_inst()->get_read(fa, fc, collect);
}


#define TEMPLATE_ONLY
#include <TypeAnnotation.C>
#include <OperatorDescriptors.C>
template class TypeCollection<ArrayModifyDescriptor>;
template class TypeCollection<ArrayConstructDescriptor>;
template class OperatorAnnotCollection<ArrayModifyDescriptor>;
template class OperatorAnnotCollection<ArrayConstructDescriptor>;
template class TypeAnnotCollection<ArrayDefineDescriptor>;
template class TypeAnnotCollection<ArrayOptDescriptor>;
#include <AnnotDescriptors.C>
template class CollectPair<TypeDescriptor, ArrayDescriptor, 0>;

