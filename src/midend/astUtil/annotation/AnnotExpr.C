#include <general.h>
#include <AnnotExpr.h>
#include <SymbolicSelect.h>
#include <SymbolicPlus.h>
#include <SymbolicMultiply.h>

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

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
         string r= val.ToString();
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
         string r= val.ToString(); 
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
  SymbolicVal r = ReplaceVal( val, SymbolicVar(varname, 0), repl);
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
        assert(GetArg(0).GetValType() == VAL_VAR && GetArg(1).GetValType() == VAL_CONST);
        SymbolicVar var( GetArg(0).ToString() + "_" + GetArg(1).ToString(), 0 );
        return var.CodeGen(fa);
     }

AstNodePtr SymbolicDotExp:: CodeGen( AstInterface& fa) const
     {  
         assert(NumOfArgs() == 2);
         AstNodePtr obj = GetArg(0).CodeGen(fa);
         AstNodeType objtype;
         bool succ = fa.IsExpression(obj, &objtype);
         assert(succ);
         string objtypename;
         fa.GetTypeInfo(objtype, 0, &objtypename);

         string val = GetArg(1).ToString(), type = GetArg(1).GetTypeName();
         SymbolicConst tmp( objtypename + "::" + val, type);
         return fa.CreateBinaryOP(".",obj, tmp.CodeGen(fa)); 
     }

AstNodePtr SymbolicSubscriptExp:: CodeGen( AstInterface& fa) const
     {
         assert(NumOfArgs() == 2);
         AstNodePtr array = GetArg(0).CodeGen(fa);
         AstNodePtr sub = GetArg(1).CodeGen(fa);
         return fa.CreateBinaryOP("[]", array, sub);
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
         if (val.find('.') != string::npos)
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
               result = new SymbolicVar(id, 0);
               assert( id != "none");
            }
            else 
               result = new SymbolicConst(id, "function");
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
           result = new SymbolicFunction( op, args);
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
        c = peek_ch(in);
        if (c != ')') {
          ReadContainerWrap<SymbolicValDescriptor, vector<SymbolicVal> > accu( args);
          read_list(in, accu, ',');
        }
        read_ch(in, ')');
        result = new SymbolicFunction( result, args);
     }
     else if (c == '[') {
       read_ch(in,c);
       SymbolicVal r2 = ReadSymbolicExpr( 0, in);
       read_ch(in,']'); 
       SymbolicFunction::Arguments args;
       args.push_back(result);
       args.push_back(r2);
       result = new SymbolicSubscriptExp( args);
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
        else {
           SymbolicFunction::Arguments args;
           args.push_back(result);
           args.push_back(r2);
           if (op == "$") 
               result = result = new SymbolicExtendVar(args);
           else
               result = new SymbolicFunction( op, args);
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
     l = atoi(lb.ToString().c_str());
     u = atoi(ub.ToString().c_str());
     return true;
  }
  return false;
}

void SymbolicParamListDescriptor::
replace_var( const string& varname, const SymbolicVal& repl)
{
  for (unsigned int i = 0; i < size(); ++i) {
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
  const SymbolicFunction::Arguments parList;

  SymbolicVal orig;
  SymbolicFunction::Arguments result;
  
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
    for (unsigned int i = cur; i < result.size(); ++i) {
      r->ApplyOpd( result[i]);
    }

    result.resize(cur);
    result.push_back( GetExprVal(r));
  }

  void VisitFunction( const SymbolicFunction& u)
  {
    string buf = u.GetOp().ToString();
    if (buf == "$" && u.GetArg(0).ToString() == basename) {
	assert(u.NumOfArgs() == 2);
        string curext = u.GetArg(1).ToString();
        if ( curext == extname) {
	    for (int i = 0; i <= extub-extlb; ++i) {
	      result.push_back(parList[parstart + i]);
	    } 
	}
	else if (u.GetArg(1).GetValType() == VAL_CONST && 
                 u.GetArg(1).GetTypeName() == "int") {
            int index = atoi(curext.c_str());
	    assert( index >= extlb && index <= extub);
	    result.push_back( parList[parstart + index - extlb] );
	}
	else {
            cerr << "Error: expecting integer const instead of ";
            u.GetArg(1).Dump();
            cerr << endl;
	    assert(false);
        }
    }
   else if (buf == "repeat") {
	assert(u.NumOfArgs() == 4);
	buf = u.GetArg(0).ToString();
	int lb = 0, ub = 0;
        lb = atoi( u.GetArg(1).ToString().c_str());
        ub = atoi( u.GetArg(2).ToString().c_str());
	for (int i = lb; i <= ub; ++i) {
	  SymbolicVal cur = ReplaceVal( u.GetArg(3), SymbolicVar(buf, 0), SymbolicConst(i));
          orig = cur;
	  orig.Visit(this);
	}
   }
   else {
	SymbolicFunction::Arguments tmp = result;
	result.clear();

        SymbolicVal op = u.GetOp();
        orig = op;
        orig.Visit(this);
        assert(result.size() == 1);
        SymbolicVal nop = result[0];

        result.clear();
	for (unsigned int i = 0; i < u.NumOfArgs(); ++i) {
          SymbolicVal cur = u.GetArg(i);
          orig = cur;
	  orig.Visit(this);
	}

	SymbolicVal val = u.CloneFunction( nop, result);
	result = tmp;
        result.push_back(val);
    }
  }
 public:
   ReplaceExtendibleParam( const string& bn, const string& en, int lb, int ub, 
                           int start, const SymbolicFunction::Arguments& l)
     : basename(bn), extname(en), extlb(lb), extub(ub), parstart(start), parList(l)
    {}
   SymbolicVal visit ( const SymbolicVal& o )
    { 
      orig = o;
      result.clear();
      orig.Visit(this);
      assert(result.size() == 1);
      return result[0];
    }
};

bool SymbolicFunctionDeclarationGroup::
get_val(AstInterface& fa, AstInterface::AstNodeList& argList, AstNodePtr& r) const
{
  SymbolicFunction::Arguments argVal;
  int index = 0;
  for (AstInterface::AstNodeListIterator p = fa.GetAstNodeListIterator(argList);
       !p.ReachEnd(); ++p, ++index) {
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

bool SymbolicFunctionDeclaration:: 
get_val(const SymbolicFunction::Arguments& parList, SymbolicVal& r) const
{
  vector< pair<SymbolicVar, SymbolicVal> > params;

  unsigned int j = 0;
  r = second;
  for (unsigned int i = 0; i < first.size(); ++i) {
    const ExtendibleParamDescriptor& cur = first[i];
    const SymbolicValDescriptor& cur1 = cur.get_param();
    SymbolicValType curtype = cur1.get_val().GetValType();
    if (curtype != VAL_VAR) {
      if ( parList[j] != cur1)
	return false;
      ++j;
    }
    else {
      string basename = cur1.get_val().ToString();
      string extname = cur.get_extend_var();
      if (extname == "") {
	r = ReplaceVal(r, SymbolicVar(basename, 0), parList[j]);
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
  if ( j < parList.size()) {
    cerr << "Error: given more than needed arguments *********" << j << "<" << parList.size() << endl;
    assert(false);
  }
  return true;
}

bool SymbolicFunctionDeclarationGroup :: 
get_val( const SymbolicFunction::Arguments& parList, SymbolicVal& r) const
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
#include <AnnotDescriptors.h>
template class WriteContainer<list<SymbolicFunctionDeclaration>, ',', '(', ')'>;
template class pair<SymbolicValDescriptor, SymbolicValDescriptor>;
#endif
