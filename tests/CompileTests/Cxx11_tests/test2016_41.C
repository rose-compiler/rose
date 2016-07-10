#include <iostream>
#include <vector>
#include <tuple>
#include <typeinfo>

namespace ET {

////////////////////////////////////////////////
// Basic expressions used in Expression Template
////////////////////////////////////////////////
template <typename Op, typename T1>                           using LatticeUnaryExpression  = std::pair<Op,std::tuple<T1> > ;
template <typename Op, typename T1, typename T2>              using LatticeBinaryExpression = std::pair<Op,std::tuple<T1,T2> > ;
template <typename Op, typename T1, typename T2, typename T3> using LatticeTrinaryExpression= std::pair<Op,std::tuple<T1,T2,T3> > ;

template<class obj> 
class Lattice { 
 public:
  int _osites;
  std::vector<obj> _odata;

  Lattice(int sites) : _osites(sites) , _odata(sites){};

  inline Lattice<obj> & operator=(const obj & splatme)
  {
    for(int ss=0;ss<_osites;ss++){
      _odata[ss] = splatme;
    }
    return *this;
  }
  template <typename Op, typename T1>                         inline Lattice<obj> & operator=(const LatticeUnaryExpression<Op,T1> &expr)
  {
    for(int ss=0;ss<_osites;ss++){
      _odata[ss] = eval(ss,expr);
    }
    return *this;
  }
  template <typename Op, typename T1,typename T2>             inline Lattice<obj> & operator=(const LatticeBinaryExpression<Op,T1,T2> &expr)
  {
    for(int ss=0;ss<_osites;ss++){
      _odata[ss] = eval(ss,expr);
    }
    return *this;
  }
  template <typename Op, typename T1,typename T2,typename T3> inline Lattice<obj> & operator=(const LatticeTrinaryExpression<Op,T1,T2,T3> &expr)
  {
    for(int ss=0;ss<_osites;ss++){
      _odata[ss] = eval(ss,expr);
    }
    return *this;
  }
};

  template<class obj> std::ostream& operator<< (std::ostream& stream, const Lattice<obj> &o){
    int N=o._osites;
    stream<<"{";
    for(int s=0;s<N-1;s++){
      stream<<o._odata[s];
      stream<<",";
    }
    stream<<o._odata[N-1];
    stream<<"}";
    return stream;
  }

////////////////////////////////////////////
//leaf eval of lattice ; should enable if protect using traits
////////////////////////////////////////////
template<class obj>
inline const obj & eval(const unsigned int ss, const Lattice<obj> &arg)
{
    return arg._odata[ss];
}

////////////////////////////////////////////
// recursive evaluation of expressions; Could
// switch to generic approach with variadics, a la
// Antonin's LatSim but the repack to variadic with popped
// from tuple is hideous; C++14 introduces std::make_index_sequence for this
////////////////////////////////////////////
template <typename Op, typename T1>
auto inline eval(const unsigned int ss, const LatticeUnaryExpression<Op,T1 > &expr) 
  -> decltype(expr.first.func(eval(ss,std::get<0>(expr.second))))
{
  return expr.first.func(eval(ss,std::get<0>(expr.second)));
}

template <typename Op, typename T1, typename T2>
auto inline eval(const unsigned int ss, const LatticeBinaryExpression<Op,T1,T2> &expr) 
  -> decltype(expr.first.func(eval(ss,std::get<0>(expr.second)),eval(ss,std::get<1>(expr.second))))
{
  return expr.first.func(eval(ss,std::get<0>(expr.second)),eval(ss,std::get<1>(expr.second)));
}

template <typename Op, typename T1, typename T2, typename T3>
auto inline eval(const unsigned int ss, const LatticeTrinaryExpression<Op,T1,T2,T3 > &expr) 
  -> decltype(expr.first.func(eval(ss,std::get<0>(expr.second)),eval(ss,std::get<1>(expr.second)),eval(ss,std::get<2>(expr.second))))
{
  return expr.first.func(eval(ss,std::get<0>(expr.second)),eval(ss,std::get<1>(expr.second)),eval(ss,std::get<2>(expr.second)) );
}

////////////////////////////////////////////
// Unary operators and funcs
////////////////////////////////////////////
#define UnopClass(name,ret)\
template <class arg> struct name\
{\
  static auto inline func(const arg a)-> decltype(ret) { return ret; } \
};
UnopClass(UnarySub,-a);
UnopClass(UnaryAdj,adj(a));
UnopClass(UnaryConj,conj(a));
UnopClass(UnaryTrace,trace(a));
UnopClass(UnaryTranspose,transpose(a));

////////////////////////////////////////////
// Binary operators
////////////////////////////////////////////
#define BinOpClass(name,combination)\
template <class left,class right>\
struct name\
{\
  static auto inline func(const left &lhs,const right &rhs)-> decltype(combination) const \
    {\
      return combination;\
    }\
}
BinOpClass(BinaryAdd,lhs+rhs);
BinOpClass(BinarySub,lhs-rhs);
BinOpClass(BinaryMul,lhs*rhs);

////////////////////////////////////////////
// Operator syntactical glue
////////////////////////////////////////////

#define UNOP(name)   name<decltype(eval(0, arg))>
#define BINOP(name)  name<decltype(eval(0, lhs)), decltype(eval(0, rhs))>
#define TRINOP(name) name<decltype(eval(0, pred)), decltype(eval(0, lhs)), decltype(eval(0, rhs))>

#define DEFINE_UNOP(op, name)\
template <typename T1> inline auto op(const T1 &arg)	\
  -> decltype(LatticeUnaryExpression<UNOP(name),const T1&>(std::make_pair(UNOP(name)(),\
                                                           std::forward_as_tuple(arg)))) \
{\
 return LatticeUnaryExpression<UNOP(name), const T1 &>(std::make_pair(UNOP(name)(),\
						       std::forward_as_tuple(arg))); \
}

#define DEFINE_BINOP(op, name)\
template <typename T1,typename T2> inline auto op(const T1 &lhs,const T2&rhs)	\
  -> decltype(LatticeBinaryExpression<BINOP(name),const T1&,const T2 &>(std::make_pair(BINOP(name)(),\
								        std::forward_as_tuple(lhs,rhs)))) \
{\
 return LatticeBinaryExpression<BINOP(name), const T1 &, const T2 &>(std::make_pair(BINOP(name)(),\
								     std::forward_as_tuple(lhs, rhs))); \
}

#define DEFINE_TRINOP(op, name)\
template <typename T1,typename T2,typename T3> inline auto op(const T1 &pred,const T2&lhs,const T3 &rhs) \
  -> decltype(LatticeTrinaryExpression<BINOP(name),const T1&,const T2 &,const T3&>(std::make_pair(TRINOP(name)(),\
										   std::forward_as_tuple(pred,lhs,rhs)))) \
{\
  return LatticeTrinaryExpression<BINOP(name), const T1 &, const T2 &,const T3&>(std::make_pair(TRINOP(name)(), \
										 std::forward_as_tuple(pred,lhs, rhs))); \
}

////////////////////////
//Operator definitions
////////////////////////
DEFINE_UNOP(operator -,UnarySub);
DEFINE_UNOP(adj,UnaryAdj);
DEFINE_UNOP(conj,UnaryConj);
DEFINE_UNOP(trace,UnaryTrace);
DEFINE_UNOP(transpose,UnaryTranspose);

DEFINE_BINOP(operator+,BinaryAdd);
DEFINE_BINOP(operator-,BinarySub);
DEFINE_BINOP(operator*,BinaryMul);

}

using namespace ET;
 	      
int main(int argc,char **argv){
   
   const int length = 16;

   Lattice<double> v1(length); v1=1.0;
   Lattice<double> v2(length); v2=2.0;
   Lattice<double> v3(length); v3=10.0;

   BinaryAdd<double,double> tmp;
   LatticeBinaryExpression<BinaryAdd<double,double>,Lattice<double> &,Lattice<double> &> 
     expr(std::make_pair(tmp,
	  std::forward_as_tuple(v1,v2)));
   tmp.func(eval(0,v1),eval(0,v2));

   auto var = v1+v2;
   std::cout<<typeid(var).name()<<std::endl;

   v3=v1+v2;

   std::cout<<"v3 = "<< v3<<std::endl;

   v3=v1+v2+v1*v2;

   std::cout<<"v3 = "<< v3<<std::endl;

};

void testit(Lattice<double> &v1,Lattice<double> &v2,Lattice<double> &v3)
{
   v3=v1+v2+v1*v2;
}

