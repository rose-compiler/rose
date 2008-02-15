// DQ (1/1/2006): This is OK if not declared in a header file
using namespace std;

#include <general.h>
#include <ArrayInterface.h>
#include <CPPAstInterface.h>

// DQ (3/13/2006): If we have to have this at lease it is 
// not in the header files seen by ALL ROSE applications.
#define Boolean int

extern bool DebugAliasAnal();

void ArrayInterface::
initialize( AstInterface& fa, const AstNodePtr& h)
{
   aliasCollect(fa, h);
   valueCollect.build( fa, h, *this, ArrayAnnotation::get_inst());
}

void ArrayInterface::observe( AstInterface& fa) 
{
  fa.AttachObserver( &valueCollect.get_value_map());
  fa.AttachObserver(this);
}

void ArrayInterface::
ObserveCopyAst( AstInterface& fa, const AstNodePtr& orig, const AstNodePtr& copy)
{
  map <AstNodePtr, int>::const_iterator p1 = dimmap.find(orig);
  if (p1 != dimmap.end())
    dimmap[copy] = (*p1).second;
  map <AstNodePtr, SymbolicFunctionDeclarationGroup>::const_iterator p2 = lenmap.find(orig);
  if (p2 != lenmap.end())
    lenmap[copy] = (*p2).second;
}

void ArrayInterface :: 
stop_observe( AstInterface& fa) 
{
  fa.DetachObserver( &valueCollect.get_value_map());
  fa.DetachObserver(this);
}

bool ArrayInterface ::
may_alias(AstInterface& fa, const AstNodePtr& r1, const AstNodePtr& r2)
{
  AstNodePtr array1, array2;
  ArrayAnnotation* annot = ArrayAnnotation::get_inst();
  bool elem1 = annot->is_access_array_elem( r1, &array1) ;
  bool len1 = annot->is_access_array_length( r1, &array1);
  bool elem2 = annot->is_access_array_elem( r2, &array2);
  bool len2 = annot->is_access_array_length( r2, &array2);

  if ( (elem1 && len2) || (len1 && elem2))  {
     return false;
  } 
  else if ( (elem1 && elem2)  || (len1 && len2)) {
     if (may_alias(fa, array1, array2)) {
        if (DebugAliasAnal())
            cerr << "has alias between " << fa.AstToString(r1) << " and " << fa.AstToString(r2) << endl;
        return true;
     }
  }
  else if (elem1 || len1) {
     if (may_alias(fa, array1, r2)) {
        if (DebugAliasAnal())
            cerr << "has alias between " << fa.AstToString(r1) << " and " << fa.AstToString(r2) << endl;
        return true;
     }
  }
  else if (elem2 || len2) {
     if (may_alias(fa, r1, array2)) {
        if (DebugAliasAnal())
            cerr << "has alias between " << fa.AstToString(r1) << " and " << fa.AstToString(r2) << endl;
        return true;
     }
  }
  else {
     AstInterface::AstNodeList args;
     if (annot->is_array_construct_op( r1, &args)) {
        for (AstInterface::AstNodeListIterator p = fa.GetAstNodeListIterator(args);
             !p.ReachEnd(); ++p) {
          AstNodePtr cur = *p;
          if (may_alias( fa, cur, r2)) {
            if (DebugAliasAnal())
               cerr << "has alias between " << fa.AstToString(r1) << " and " << fa.AstToString(r2) << endl;
             return true;
          }
        }
        return false;    
     }
     else if (annot->is_array_construct_op( r2, &args)) {
        for (AstInterface::AstNodeListIterator p = fa.GetAstNodeListIterator(args);
             !p.ReachEnd(); ++p) {
          AstNodePtr cur = *p;
          if (may_alias( fa, cur, r1)) {
            if (DebugAliasAnal())
               cerr << "has alias between " << fa.AstToString(r1) << " and " << fa.AstToString(r2) << endl;
             return true;
          }
        }
        return false;
     }
     return aliasCollect.may_alias( fa, r1, r2);
  }
  return false;
}

bool ArrayInterface::
get_array_opt(AstInterface& fa, const AstNodePtr& array, ArrayOptDescriptor& r)
{
  string name;
  if (!fa.IsVarRef(array, 0, &name))
    return false;
  
  map <string, ArrayOptDescriptor>::const_iterator p = optmap.find(name);
  if (p != optmap.end()) 
  {
    r = (*p).second;
    return true;
  }
  ArrayOptDescriptor desc;
  if (!ArrayAnnotation::get_inst()->has_array_opt( array, &desc))
    return false;
  
  // int dim;
  for (ArrayOptDescriptor::InitVarIterator p = desc.init_var_begin();
       p != desc.init_var_end(); ++p) {
    DefineVariableDescriptor& cur = *p;
    ExtendibleParamDescriptor &par = cur.get_var();
    string parname = par.get_param_name();
    par.get_param() = SymbolicVar( name + parname, 0);
    SymbolicVal newpar = new SymbolicVar(name + parname, 0);
    desc.replace_var(parname, newpar);
  }
  optmap[name] = desc;
  r = desc;
  return true;
}
  
void ArrayInterface ::
set_array_dimension( const AstNodePtr& arrayexp, int dim)
{
  map <AstNodePtr, int>::const_iterator p = dimmap.find(arrayexp);
  if (p != dimmap.end()) {
     int olddim = (*p).second;
     assert(olddim == dim);
  }
  else
     dimmap[arrayexp] = dim;
}

bool ArrayInterface :: 
is_array_exp( const AstNodePtr& array, 
	      int *dimp, SymbolicFunctionDeclarationGroup *lenp, bool *changep)
{
  ArrayDefineDescriptor desc;
  if (!ArrayAnnotation::get_inst()->known_array( array, &desc))
    return false;
  if (lenp != 0) 
  {
    map <AstNodePtr, SymbolicFunctionDeclarationGroup>::const_iterator p = lenmap.find(array);
    if (p != lenmap.end()) 
    {
      *lenp = (*p).second;
      lenp = 0;
    }
  }
  int dim=0;
  if (dimp != 0 || lenp != 0) 
  {
    map <AstNodePtr, int>::const_iterator p = dimmap.find(array);
    if (p != dimmap.end()) 
    {
      dim = (*p).second;
      if (dimp != 0) {
         *dimp = dim;
         dimp = 0;
      }
    }
  }
  if (dimp == 0 && lenp == 0) 
  {
    assert( changep == 0);
    return true;
  }
    
  if (changep != 0)
    *changep = true;
  HasValueDescriptor valdesc;
  bool hasval =  valueCollect.known_value( array, &valdesc, changep);
  if (changep != 0 && !AstInterface::IsVarRef(array))
    *changep = false; 
    
  if (dim == 0) 
  {
    SymbolicValDescriptor dimval;
    if (!hasval || !valdesc.has_value("dimension", &dimval)  
        || dimval.get_val().GetValType() != VAL_CONST 
	|| dimval.get_val().GetTypeName() != "int")  {
      dimval = desc.get_dimension();
    }
    if (!dimval.get_val().ToInt(dim))
        assert(false);
    if (dimp != 0)
      *dimp = dim;
    dimmap[array] = dim;
  }
  if (lenp != 0) 
  {
    SymbolicFunctionDeclarationGroup len;
    char buf[20];
    for (int i = 0; i < dim; ++i) {
      sprintf(buf, "length_%d", i);
      SymbolicValDescriptor parval(i);
      ExtendibleParamDescriptor par_i(parval);
      SymbolicValDescriptor tmp;
      if (hasval && valdesc.has_value( string(buf), &tmp)
	  && !tmp.is_bottom() && !tmp.is_top()) 
	len.push_back( SymbolicFunctionDeclaration( par_i, tmp));
    }
    len.insert(len.end(), lenp->begin(), lenp->end());
    *lenp = len;
    lenmap[array] = len;
  }
  return true;
}

bool ArrayInterface::
is_array_mod_op( const AstNodePtr& arrayExp, AstNodePtr* arrayp, int *dimp, 
		 SymbolicFunctionDeclarationGroup *len, SymbolicFunctionDeclarationGroup* elem, 
		 bool *reshape)
{
  AstNodePtr array;
  ArrayDescriptor desc;
  if (!ArrayAnnotation::get_inst()->is_array_mod_op( arrayExp, &array, &desc, reshape)) 
    return false;
  HasValueMapReplace repl( valueCollect.get_value_map(), true);
  desc.replace_val(repl);

  if (arrayp != 0)
    *arrayp = array;

  int dim = 0, dim1 = 0;
  if (elem != 0 && dimp == 0)
     dimp = &dim;
  if ( dimp != 0 && desc.get_dimension(*dimp)) {
      dim1 = *dimp;
  }
  if (len != 0)
    *len = desc.get_length();
  if (dimp != 0 || len != 0) 
  {
    if (!is_array_exp( array, dimp, len, reshape))
      assert(false);
    if (dimp != 0 && dim1 != 0 && *dimp > dim1)
       *dimp = dim1;
  }
  else
    assert( reshape == 0);
  if (elem != 0) {
    assert( dimp != 0);
    *elem = desc.get_elem();
    elem->replace_var("dimension", *dimp);
  }
  return true;
}

bool ArrayInterface::
is_array_construct_op( const AstNodePtr& arrayExp, AstInterface::AstNodeList* alias,int *dimp, 
 			      SymbolicFunctionDeclarationGroup *len, SymbolicFunctionDeclarationGroup* elem)
{
  ArrayDescriptor desc;
  if (ArrayAnnotation::get_inst()->is_array_construct_op( arrayExp, alias, &desc) ) {
    HasValueMapReplace repl( valueCollect.get_value_map(), true);
    desc.replace_val(repl);

    int dim = 0, dim1 = 0;
    if (elem != 0 && dimp == 0)
       dimp = &dim;
    if ( dimp != 0 && desc.get_dimension(*dimp)) {
      dim1 = *dimp;
    }

    if (len != 0)
      *len = desc.get_length();
    if (dimp != 0 || len != 0)
    {
      if (!is_array_exp( arrayExp, dimp, len))
	assert(false);
      if (dimp != 0 && dim1 != 0 && *dimp > dim1)
        *dimp = dim1;
    }
    if (elem != 0)  {
      assert( dimp != 0);
      *elem = desc.get_elem();
      elem->replace_var("dimension", *dimp);
    }
    return true;
  } 
  return false;
}


AstNodePtr ArrayInterface::
impl_array_opt_init( AstInterface& fa, const AstNodePtr& array, bool insertInit)
{
  ArrayOptDescriptor desc;

  if (!get_array_opt( fa, array, desc))
    return 0;  

  int dim;
  if (!is_array_exp( array, &dim))
    assert(false);
  AstNodePtr result = insertInit? 0 : fa.CreateBasicBlock();
  for (ArrayOptDescriptor::InitVarIterator p = desc.init_var_begin();
       p != desc.init_var_end(); ++p) {
    DefineVariableDescriptor& cur = *p;
    cur.replace_var( "this", SymbolicAstWrap(array));
    cur.replace_var( "dimension", dim);

    const ExtendibleParamDescriptor& par = cur.get_var();
    string extname = par.get_extend_var();
    string parname = par.get_param_name();
    int lb = -1, ub = -1;
    par.get_extension( lb, ub);
    string vartype = cur.get_var_type();
    for (int i = lb; i <= ub; ++i) {
      string varname = parname;
      SymbolicValDescriptor initval = cur.get_var_init();
      // if (i >= 0) {
	varname = SymbolicExtendVar::get_varname( varname, i);
	initval.replace_var(extname, i); 
      // }
      AstNodePtr init = initval.get_val().CodeGen(fa);
      if (insertInit) {
	string varname1 = fa.NewVar( fa.GetType(vartype), varname, false, 0, init);
	assert( varname1 == varname);
      }
      else {
	string varname1 = fa.NewVar( fa.GetType(vartype), varname);
	assert( varname1 == varname);
	AstNodePtr var = fa.CreateVarRef( varname);
	AstNodePtr assign = fa.CreateAssignment( var, init);
	fa.BasicBlockAppendStmt(result, assign);
      }
    }
  }
  return result;
}

AstNodePtr ArrayInterface::
impl_reshape_array( AstInterface& fa, 
		    const AstNodePtr& array,
		    AstInterface::AstNodeList& ivarAst)
{
  ArrayDefineDescriptor desc;
  if (!ArrayAnnotation::get_inst()->known_array( array, &desc))
    return false;

  SymbolicFunctionDeclarationGroup reshape = desc.get_reshape();
  reshape.replace_var( "this", SymbolicAstWrap(array));
  reshape.replace_var( "dimension", ivarAst.size());

  AstNodePtr r;
  if (!reshape.get_val( fa, ivarAst, r)) {
     cerr << "Error: cannot extract value from reshape spec: \n";
     reshape.write(cerr);
     cerr << endl;
     assert(false);
  }
  return r;
}


AstNodePtr ArrayInterface::
impl_access_array_elem (AstInterface& fa, const AstNodePtr& array,
			   AstInterface::AstNodeList& ivarAst)
{
  SymbolicFunctionDeclarationGroup elem;

  ArrayOptDescriptor desc;
  if (get_array_opt(fa, array, desc)) 
  {
    elem = desc.get_elem();
  }
  else 
  {
    ArrayDefineDescriptor desc1;
    if (!ArrayAnnotation::get_inst()->known_array( array, &desc1))
      assert(false);
    elem = desc1.get_elem();
  }
  elem.replace_var("this", SymbolicAstWrap(array));
  elem.replace_var( "dimension", ivarAst.size());
  AstNodePtr r;
  if (! elem.get_val(fa, ivarAst, r))
     assert(false);
  return r;
}


AstNodePtr ArrayInterface::
impl_access_array_length( AstInterface& fa, const AstNodePtr& array,
			 int dim, int plus)
{
  SymbolicVal rval;
  ArrayOptDescriptor desc;
  if (get_array_opt(fa, array, desc)) 
  {
    if (!desc.get_length(dim, rval))
      assert(false);
    
  }
  else 
  {
    ArrayDefineDescriptor desc1;
    if (!ArrayAnnotation::get_inst()->known_array( array, &desc1))
      return false;
    if (! desc1.get_length(dim, rval))
      assert(false);
  }
  ReplaceVal(rval, SymbolicVar("this",0), SymbolicAstWrap(array));
  if (plus != 0)
     rval = rval + plus;
  return rval.CodeGen(fa);
}

Boolean ArrayInterface ::
IsArray( AstInterface& fa, const AstNodePtr& s)
{ 
  if ( ArrayAnnotation::get_inst()->known_array( s))
     return true;
  return false;
}

Boolean ArrayInterface ::
IsArrayRef( AstInterface& fa, const AstNodePtr& t)
{
  if ( ArrayAnnotation::get_inst()->known_array_type(fa.GetExpressionType(t)))
     return true;
  return false;
}
Boolean ArrayInterface :: 
IsArrayAccess( AstInterface& fa, const AstNodePtr& s, AstNodePtr* arrayp,
                                 AstInterface::AstNodeList* index)
{  
   AstNodePtr array;
   if ( ArrayAnnotation::get_inst()->is_access_array_elem( s, &array, index)) {
     if (arrayp != 0)
        *arrayp = array; 
     return true;
   }
   return false;
}

AstNodePtr ArrayInterface::
CreateArrayAccess(AstInterface& fa, const AstNodePtr& arr, 
                          AstInterface::AstNodeList& index)
{
  return impl_access_array_elem(  fa, arr, index); 
}

Boolean ArrayInterface ::
GetArrayBound( AstInterface& fa, const AstNodePtr& array,
                                 int dim, int &lb, int &ub) 
{ 
  SymbolicFunctionDeclarationGroup len;
  if (!is_array_exp( array, 0, &len))
    assert(false);

  SymbolicFunction::Arguments pars;
  pars.push_back( SymbolicConst(dim));

  SymbolicVal rval;
  if (!len.get_val( pars, rval)) 
     return false;
  if (!rval.ToInt(ub))
     return false;
  lb = 0;
  return true;
}


