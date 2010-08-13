

#include "AnnotExpr.h"
#include "SymbolicSelect.h"
#include "SymbolicPlus.h"
#include "SymbolicMultiply.h"
#include <string.h>

using namespace std;

template class CloseDescriptor< SymbolicValDescriptor, '{', '}'>;

// DQ (8/30/2009): Debugging ROSE compiling ROSE (this statement does not compile using ROSE).
#ifndef USE_ROSE

// DQ (1/9/2010): This is a problem for the Intel icpc compiler (which uses EDG same as ROSE).
#ifndef __INTEL_COMPILER
template bool CloseDescriptor< SymbolicValDescriptor, '{', '}'>::read(istream&);
#endif

#endif

void SymbolicValDescriptor :: set_val( const SymbolicVal& v) 
{
  if (v.IsNIL() || FindVal(v, get_top()) )
    val = get_top();
  else if (FindVal( v, get_bottom()) )
    val = get_bottom();
  else
    val = v;
}

void SymbolicValDescriptor::write(ostream& out) const
 { 
         string r= val.toString();
         if (r != "")
            out << r;
 }
void SymbolicValDescriptor::Dump() const
{
     if (val.IsNIL())
        cerr << "nil";
     else if (val == get_top())
          cerr << "top";
     else if (val == get_bottom())
         cerr << "bottom";
     else {
         string r= val.toString(); 
         if (r != "")
            cerr << r;
     }
}

bool SymbolicValDescriptor:: is_top() const
{
 return val == get_top();
}

bool SymbolicValDescriptor:: is_bottom() const
{
  return val == get_bottom();
}

SymbolicConst SymbolicValDescriptor :: get_top() 
{
  return SymbolicConst("", "top");
}

SymbolicConst SymbolicValDescriptor :: get_bottom() 
{
  return SymbolicConst("", "bottom");
}

void SymbolicValDescriptor::
replace_var( const string& varname, const SymbolicVal& n)
{
  SymbolicValDescriptor that(n);
  SymbolicVal repl = that.get_val();
  SymbolicVal r = ReplaceVal( val, SymbolicVar(varname, AST_NULL), repl);
  set_val(r);
}

void SymbolicValDescriptor::
replace_val( MapObject<SymbolicVal, SymbolicVal>& repl)
{
  SymbolicVal r = ReplaceVal( val, repl);
  set_val(r);
}

bool SymbolicValDescriptor:: find( const SymbolicVal& v)
{
  return FindVal( val, v);
}


bool SymbolicValDescriptor:: merge( const SymbolicValDescriptor& that)
{
  if (that.is_top() || is_bottom())
     return false;
  else if (that.is_bottom() || is_top())
      val = that;
  else if (val != that.val) 
        val = get_bottom(); 
  else
       return false;
  return true;
}

string SymbolicExtendVar :: get_varname( string name, int index)
{
        char buf[20];
        sprintf(buf, "%d", index);
        return name + "_" + buf;
}

AstNodePtr SymbolicExtendVar :: CodeGen( AstInterface& fa) const
     {  
        string name;
        assert(first_arg().GetValType() == VAL_VAR && last_arg().GetValType() == VAL_CONST);
        SymbolicVar var( first_arg().toString() + "_" + last_arg().toString(), AST_NULL );
        return var.CodeGen(fa);
     }

AstNodePtr SymbolicFunctionPtrCall :: CodeGen(AstInterface& fa) const
{
  const_iterator i = args_begin(); 
  AstNodePtr func = (*i).CodeGen(fa);
  AstInterface::AstNodeList l;
  for ( ++i ; i != args_end(); ++i) {
     SymbolicVal cur = *i;
     AstNodePtr curast = cur.CodeGen(fa); 
     l.push_back(curast);
  }
  return fa.CreateFunctionCall(func,l); 
}


AstNodePtr SymbolicDotExp:: CodeGen( AstInterface& fa) const
     {  
         assert(NumOfArgs() == 2);
         AstNodePtr obj = first_arg().CodeGen(fa);
         AstNodeType objtype;
         if ( fa.IsExpression(obj, &objtype) == AST_NULL)
           assert(false);
         string objtypename;
         fa.GetTypeInfo(objtype, 0, &objtypename);

         string val = last_arg().toString(), type = last_arg().GetTypeName();
         SymbolicConst tmp( objtypename + "::" + val, type);
         return fa.CreateBinaryOP(AstInterface::BOP_DOT_ACCESS,obj, tmp.CodeGen(fa)); 
     }

SymbolicVal ReadSymbolicExpr( int level, istream& in);

SymbolicVal ReadSymbolicTerm( istream& in)
{
      char c = peek_ch(in);
      SymbolicVal result;
      if (c == '(') {
        read_ch(in,c);
        result = ReadSymbolicExpr( 0, in);
        read_ch(in, ')');
        c = peek_ch(in);
      } 
      else if (is_num(c)) {
         string val = read_num(in);
         if (strchr(val.c_str(), '.') != 0)
            result = new SymbolicConst(val, "float");
         else
            result = new SymbolicConst(val, "int");
         c = 0;
      }
      else if (is_id(c)) {      
         string id = read_id(in);
         if (id == "Min" || id == "min") {
               read_ch(in, '(');
               SymbolicSelect *r = new SymbolicSelect(-1);
               ReadContainerWrap<SymbolicValDescriptor, SymbolicSelect> accu(*r);
               read_list(in, accu, ',');
               read_ch(in, ')');
               result = r;
               c = 0;
          }
         else if (id == "Max" || id == "max") {
              read_ch(in, '(');
              SymbolicSelect *r = new SymbolicSelect(1);
              ReadContainerWrap<SymbolicValDescriptor, SymbolicSelect> accu( *r);
              read_list(in, accu,',');
              read_ch(in, ')');
              result = r;
              c = 0;
         }
         else {
            c  = peek_ch(in);
            if (c != '(') {
               result = new SymbolicVar(id, AST_NULL);
               assert( id != "none");
            }
            else {
              read_ch(in, c);
              SymbolicFunction::Arguments args;
              c = peek_ch(in);
              if (c != ')') {
                 ReadContainerWrap<SymbolicValDescriptor, SymbolicFunction::Arguments > accu( args);
                 read_list(in, accu, ',');
               }
              read_ch(in, ')');
              result = new SymbolicFunction(AstInterface::OP_NONE, id, args);
            }
         }
     }
     else if (is_operator(c)) { // unary operators
         string op = read_operator(in);
         SymbolicVal v = ReadSymbolicTerm( in);
         if (op == "-") {
             result = (-1) * v;
         }
         else {
           SymbolicFunction::Arguments args;
           args.push_back(result);
           result = new SymbolicFunction( AstInterface::OP_NONE, op, args);
         }
         c = peek_ch(in);
     }       
     else {
          // "non recognizable value start " 
          c = 0;
     }
     while (c == '.') {
        read_ch(in, c);
        string id = read_id(in);
        c = peek_ch(in);
        SymbolicFunction::Arguments args;
        args.push_back(result);
        if (c == '(')
          args.push_back( new SymbolicConst(id, "memberfunction"));
        else
          args.push_back( new SymbolicConst( id, "field"));
        result = new SymbolicDotExp( args);
     }
     if (c == '(') {
        read_ch(in, c);
        SymbolicFunction::Arguments args;
        args.push_back(result);
        c = peek_ch(in);
        if (c != ')') {
          ReadContainerWrap<SymbolicValDescriptor, SymbolicFunction::Arguments > accu( args);
          read_list(in, accu, ',');
        }
        read_ch(in, ')');
        result = new SymbolicFunctionPtrCall( args);
     }
     else if (c == '[') {
       read_ch(in,c);
       SymbolicVal r2 = ReadSymbolicExpr( 0, in);
       read_ch(in,']'); 
       result = new SymbolicFunction(AstInterface::OP_ARRAY_ACCESS, "[]", result, r2);
     }

    return result;
}

SymbolicVal ReadSymbolicExpr( int level, istream& in)
{
     SymbolicVal result = ReadSymbolicTerm( in);
     char c = peek_ch(in);
     int level1 = is_operator(c);
     while (level1 > level) { // binary operator
        string op = read_operator(in);
        SymbolicVal r2 = ReadSymbolicExpr( level1, in);
        if (op == "*") { 
           result = result * r2;
         }
        else if (op == "+") {
           result = result + r2;
        }
        else if (op == "-") {
           result = result - r2;
        }
        else if (op == "/") 
           result = new SymbolicFunction( AstInterface::BOP_DIVIDE,op, result,r2);
        else {
           SymbolicFunction::Arguments args;
           args.push_back(result);
           args.push_back(r2);
           if (op == "$") 
               result = result = new SymbolicExtendVar(args);
           else
               result = new SymbolicFunction( AstInterface::OP_NONE, op, result,r2);
        }
        c = peek_ch(in);
        level1 = is_operator(c);
     }
     return result;
}

bool SymbolicValDescriptor::read( istream& in)
{
     val = ReadSymbolicExpr( 0, in);
     
     return !val.IsNIL();
}

bool ExtendibleParamDescriptor :: read( istream& in)
{
  return BaseClass::read(in);
}

void ExtendibleParamDescriptor :: 
replace_var( const string& varname, const SymbolicVal& _val)
{
  get_ext_lb().replace_var(varname, _val);
  get_ext_ub().replace_var(varname, _val);
}

void ExtendibleParamDescriptor ::
replace_val(MapObject<SymbolicVal, SymbolicVal>& repl)
{
  get_ext_lb().replace_val(repl);
  get_ext_ub().replace_val(repl);
}

bool ExtendibleParamDescriptor :: get_extension( int& l, int& u) const
{
  SymbolicVal lb = get_ext_lb();
  SymbolicVal ub = get_ext_ub();
  if ( lb.GetValType() == VAL_CONST && lb.GetTypeName() == "int" &&
          ub.GetValType() == VAL_CONST && ub.GetTypeName() == "int") {
     l = atoi(lb.toString().c_str());
     u = atoi(ub.toString().c_str());
     return true;
  }
  return false;
}

void SymbolicParamListDescriptor::
replace_var( const string& varname, const SymbolicVal& repl)
{
  for (size_t i = 0; i < size(); ++i) {
    ExtendibleParamDescriptor& cur = operator[](i);
    cur.replace_var( varname, repl);
  }
}

void SymbolicParamListDescriptor::Dump() const
{ write(cerr); }

void SymbolicFunctionDeclaration::
replace_var( const string& varname, const SymbolicVal& repl)
{
  first.replace_var( varname, repl);
  second.replace_var( varname, repl);
}

void SymbolicFunctionDeclaration::Dump() const
{ write(cerr); }

void SymbolicFunctionDeclaration:: 
replace_val(MapObject<SymbolicVal, SymbolicVal>& repl)
{
  second.replace_val(repl);
}

class ReplaceExtendibleParam : public SymbolicVisitor
{
  string basename, extname;
  int extlb, extub, parstart;
  const vector<SymbolicVal> parList;

  SymbolicVal orig;
  vector<SymbolicVal> result;
  
  void Default() { 
    result.push_back(orig); 
  }

  void VisitExpr( const SymbolicExpr& exp) 
  {
    int cur = result.size();
    SymbolicExpr *r = exp.DistributeExpr(SYMOP_NIL, SymbolicVal());
    for (SymbolicExpr::OpdIterator iter = exp.GetOpdIterator();
         !iter.ReachEnd(); iter.Advance()) {
      SymbolicVal cur = exp.Term2Val(iter.Current());
      orig = cur;
      orig.Visit(this);
    }
    for (size_t i = cur; i < result.size(); ++i) {
      r->ApplyOpd( result[i]);
    }

    result.resize(cur);
    result.push_back( GetExprVal(r));
  }

  void VisitFunction( const SymbolicFunction& u)
  {
    string buf = u.GetOp();
    if (buf == "$" && u.first_arg().toString() == basename) {
	assert(u.NumOfArgs() == 2);
        string curext = u.last_arg().toString();
        if ( curext == extname) {
	    for (int i = 0; i <= extub-extlb; ++i) {
	      result.push_back(parList[parstart + i]);
	    } 
	}
	else if (u.last_arg().GetValType() == VAL_CONST && 
                 u.last_arg().GetTypeName() == "int") {
            int index = atoi(curext.c_str());
	    assert( index >= extlb && index <= extub);
	    result.push_back( parList[parstart + index - extlb] );
	}
	else {
            cerr << "Error: expecting integer const instead of ";
            u.last_arg().Dump();
            cerr << endl;
	    assert(false);
        }
    }
   else if (buf == "repeat") {
        assert(u.NumOfArgs() == 4);
        SymbolicFunction::const_iterator i = u.args_begin();
	buf = (*i).toString();
	int lb = 0, ub = 0;
        lb = atoi( (*(++i)).toString().c_str());
        ub = atoi( (*(++i)).toString().c_str());
        SymbolicVal cur = *(++i);
	for (int j = lb; j <= ub; ++j) {
	  SymbolicVal tmp= ReplaceVal( cur, SymbolicVar(buf, AST_NULL), SymbolicConst(j));
          orig = tmp;
	  orig.Visit(this);
	}
   }
   else {
        int cur = result.size();
        for (SymbolicFunction::const_iterator i = u.args_begin(); 
              i != u.args_end(); ++i) {
          SymbolicVal tmp= *i;
          orig = tmp;
	  orig.Visit(this);
	}
        SymbolicFunction::Arguments args;
        for (size_t i = cur; i < result.size(); ++i) {
          args.push_back(result[i]);
        }
	SymbolicVal val = u.cloneFunction( args);
        result.resize(cur);
        result.push_back(val);
    }
  }
 public:
   ReplaceExtendibleParam( const string& bn, const string& en, int lb, int ub, 
                           int start, const vector<SymbolicVal>& l)
     : basename(bn), extname(en), extlb(lb), extub(ub), parstart(start), parList(l)
    {}
   SymbolicVal visit ( const SymbolicVal& o )
    { 
      orig = o;
      result.clear();
      orig.Visit(this);
      assert(result.size() == 1);
      return result.front();
    }
};

bool SymbolicFunctionDeclarationGroup::
get_val(AstInterface& fa, AstInterface::AstNodeList& argList, AstNodePtr& r) const
{
  std::vector<SymbolicVal> argVal;
  int index = 0;
  for (AstInterface::AstNodeList::iterator p = argList.begin();
       p != argList.end(); ++p, ++index) {
    AstNodePtr cur = *p;
    argVal.push_back( SymbolicAstWrap(cur));
  }
  SymbolicVal result;
  if (! get_val(argVal, result))
     return false;

  r = result.CodeGen(fa);
  return true;
}

void SymbolicFunctionDeclarationGroup::Dump() const
{ write(cerr); }

//! store the function name in r ??  Store the result in 'r' ?
bool SymbolicFunctionDeclaration:: 
get_val(const std::vector<SymbolicVal>& parList, SymbolicVal& r) const
{
  vector< pair<SymbolicVar, SymbolicVal> > params;

  int j = 0;
  r = second;
  // For each parameter in this function declaration's parameter list
  for (size_t i = 0; i < first.size(); ++i) {
    const ExtendibleParamDescriptor& cur = first[i];
    const SymbolicValDescriptor& cur1 = cur.get_param();
    SymbolicValType curtype = cur1.get_val().GetValType();
    if (curtype != VAL_VAR) {
      if ( parList[j] != cur1)
	return false;
      ++j;
    }
    else { // variable parameter may have range information
      string basename = cur1.get_val().toString(); // parameter name
      string extname = cur.get_extend_var();    // extension name
      if (extname == "") {
	r = ReplaceVal(r, SymbolicVar(basename, AST_NULL), parList[j]);
	++j;
      }
      else {
        int lb = 0, ub = 0;
        if (!cur.get_extension(lb, ub) || ub-lb >= (int)parList.size() - j) {
           cerr << "lb = " << lb << "; ub = " << ub << "; parList.size = " << parList.size() << "; j = " << j << endl; 
           assert(false);
        }
        ReplaceExtendibleParam op(basename, extname, lb, ub, j, parList);
        r = op.visit( r);
        j += (ub-lb)+1;
      }
    }
  }
  if ( (size_t)j < parList.size()) {
    cerr << "Error: given more than needed arguments *********" << j << "<" << parList.size() << endl;
    assert(false);
  }
  return true;
}

bool SymbolicFunctionDeclarationGroup :: 
get_val( const std::vector<SymbolicVal>& parList, SymbolicVal& r) const
{
  for (const_iterator p = begin(); p != end(); ++p) {
    const SymbolicFunctionDeclaration& cur = *p;
    if (cur.get_val(parList, r))
      return true;
  }
  return false;
}

void SymbolicFunctionDeclarationGroup::
replace_var(const string& varname, const SymbolicVal& repl)

{
  for (iterator p = begin(); p != end(); ++p) {
    SymbolicFunctionDeclaration& cur = *p;
    cur.replace_var( varname, repl);
  }
}

void SymbolicFunctionDeclarationGroup::
replace_val(MapObject<SymbolicVal, SymbolicVal>& repl)
{
  for (iterator p = begin(); p != end(); ++p) {
    SymbolicFunctionDeclaration& cur = *p;
    cur.replace_val(repl);
  }
}

void DefineVariableDescriptor::
replace_var( const string& varname, const SymbolicVal& repl)

{
  first.second.replace_var( varname, repl);
  second.replace_var(varname, repl);
}

void DefineVariableDescriptor::
replace_val( MapObject<SymbolicVal, SymbolicVal>& repl)
{
  first.second.replace_val(repl);
  second.replace_val(repl);
}

void DefineVariableDescriptor::Dump() const
{
  write(cerr);
}


#ifndef TEMPLATE_ONLY
#define TEMPLATE_ONLY
#include <AnnotDescriptors.C>
template class WriteContainer<list<SymbolicFunctionDeclaration>, ',', '(', ')'>;
template class WriteContainer<vector<SymbolicFunctionDeclaration>, ',', '(', ')'>;
template class pair<SymbolicValDescriptor, SymbolicValDescriptor>;
#endif
