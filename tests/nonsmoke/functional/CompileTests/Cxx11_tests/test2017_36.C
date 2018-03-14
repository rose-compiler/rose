// Dan,

// 260+ files have this unparsing error. I have created a test case to reproduce it.

//---------------------- input code----------------
#include <utility>
#include <cmath>

namespace RAJA {
  class IndexSet;
  typedef int Index_type;
}

typedef RAJA::Index_type IndexType;
namespace policy {
  struct invalid {};
  struct undefined {};
  struct serial {};
  struct parstream {};
  struct parwork {};
}

template < typename exec_policy >
struct policy_traits_base;

template <>
struct policy_traits_base< policy::invalid > {
  typedef void raja_policy;
  typedef void reduction_policy;
  typedef void raja_indexset_policy;
  typedef void raja_2d_policy;
  typedef void raja_3d_policy;
  static const bool valid = false;
  static const bool async = false;
  static const bool impl_serial = true;
};

namespace RAJA {

  struct PolicyBase { };

}
namespace RAJA
{
  struct seq_exec : public PolicyBase {
  };

  struct seq_segit : public seq_exec {
  };
  struct seq_reduce {
  };

}


template <>
struct policy_traits_base< policy::serial > {
  typedef RAJA::seq_exec raja_policy;
  typedef RAJA::seq_reduce reduction_policy;
//  typedef RAJA::IndexSet::ExecPolicy<RAJA::seq_segit,RAJA::seq_exec> raja_indexset_policy;
//  typedef typename forallN_policies::raja_2d_ser_policy::type raja_2d_policy;
//  typedef typename forallN_policies::raja_3d_ser_policy::type raja_3d_policy;
  static const bool valid = true;
  static const bool async = false;
  static const bool impl_serial = true;
};
template <>
struct policy_traits_base< policy::parwork > : policy_traits_base< policy::serial > { };

template < typename exec_policy >
struct policy_traits_impl;

template <>
struct policy_traits_impl< policy::invalid > :
policy_traits_base< policy::invalid > {
  static const bool pol_serial = true;
};

template <>
struct policy_traits_impl< policy::serial > :
policy_traits_base< policy::serial > {
  static const bool pol_serial = true;
};

template <>
struct policy_traits_impl< policy::undefined > :

policy_traits_base< policy::serial > {

  static const bool pol_serial = false;
};

template <>
struct policy_traits_impl< policy::parstream > :

policy_traits_base< policy::serial > {

  static const bool pol_serial = false;
};

template <>
struct policy_traits_impl< policy::parwork > :
policy_traits_base< policy::parwork > {
  static const bool pol_serial = false;
};

template < typename my_policy = policy::invalid >
using policy_traits = policy_traits_impl<my_policy>;
template < typename my_policy, typename my_kernel >
struct ares_raja_api_kernel_type_check {
  using policy_type = typename std::decay<my_policy>::type;
  using kernel_type = typename std::decay<my_kernel>::type;
  static constexpr bool is_parallel_kernel =

    true;

  static constexpr bool is_serial_kernel =

    true;

  static constexpr bool is_parallel_policy = !policy_traits<policy_type>::impl_serial;
  static constexpr bool is_serial_policy = policy_traits<policy_type>::impl_serial;
  static constexpr bool value = ( is_serial_policy && is_serial_kernel ) ||
    ( is_parallel_policy && is_parallel_kernel );
};


namespace xargs {

  struct node_index_only {};
  struct index_only {};
  struct zone_index_only {};
  struct slot_index_only {};

  struct node {};
  struct zone {};
  struct index {};
  struct ij {};
  struct ijk {};

}

namespace Ares {
  namespace core {

    template < typename my_policy = policy::invalid,
             typename loop_type,
             typename kernel_type >
               inline void for_all_2d( loop_type,
                   const IndexType&,
                   const IndexType&,
                   const IndexType&,
                   const IndexType&,
                   kernel_type&& )
               {
                   static_assert( !policy_traits< my_policy >::valid,
                       "for_all_2d: invocation does not match overloaded prototype" );
               }
    template < typename my_policy = policy::invalid,
             typename kernel_type >
               inline void
               for_all_2d( xargs::index_only,
                   const IndexType& begin_i,
                   const IndexType& end_i,
                   const IndexType& begin_j,
                   const IndexType& end_j,
                   kernel_type&& kernel )
               {
                   static_assert( ares_raja_api_kernel_type_check<my_policy, kernel_type>::value,
                       "for_all: policy, lambda mismatch!" );

                 ;

                 for(IndexType jj = begin_j; jj < end_j; ++jj) {
                   for(IndexType ii = begin_i; ii < end_i; ++ii) {
                     kernel( ii, jj );
                   }
                 }

                 ;
               }

  }

}

template < typename my_policy = policy::invalid,
         typename loop_type = xargs::index_only,
  typename kernel_type >
inline void for_all_2d( const IndexType& begin_i,
    const IndexType& end_i,
    const IndexType& begin_j,
    const IndexType& end_j,
    kernel_type&& kernel )
{
  if (begin_i < end_i && begin_j < end_j) {
    Ares::core::for_all_2d< my_policy >(
        loop_type(),
        begin_i,
        end_i,
        begin_j,
        end_j,
        std::forward<kernel_type>(kernel) );
  }
}

typedef struct NodalAdv_s {

  int imax;
  int jmax;
  int kmax;

  int ip;
  int jp;
  int kp;

  int *nactive;
  int *first;

  double *dxmom;
  double *dymom;
  double *dzmom;
  double *mvsq;
  double *massflux;

  double *slope;

} NodalAdv_t;


typedef struct Domain_s {
  int *ndxa ;
  int *ndxd ;
  int *ndxn ;

  int nslide ;

  double *nmass ;

} Domain_t;


void calcndx2d(NodalAdv_t *nl, Domain_t *domain)
{
  int imax = nl->imax;
  int jmax = nl->jmax;
  int ip = nl->ip;
  int jp = nl->jp;


  int *ndxa = domain->ndxa;
  int *ndxd = domain->ndxd;
  int *ndxn = domain->ndxn;

  double *massflux = nl->massflux;
  double *nmass = domain->nmass;

  if ( domain->nslide == 0 ) {
    for_all_2d< policy::parstream > (
        2 +1, imax + 1,
        2, jmax + 1,
        [=] (int i, int j) {

        int off = j * jp ;
        int iz = i * ip + off ;

        double signflo = 0.0;

        if ( massflux[iz] >= 0.0 ) {
        signflo = 0.999999 ;

        ndxa[iz] = iz ;
        ndxd[iz] = iz - ip ;
        ndxn[iz] = iz - 2 * ip ;
        }
        else
        {
        signflo = -0.999999 ;

        ndxa[iz] = iz - ip ;
        ndxd[iz] = iz ;
        ndxn[iz] = iz + ip ;

        }
        if ( fabs(massflux[iz]) > nmass[ndxd[iz]] ) {
          massflux[iz] = signflo * nmass[ndxd[iz]] ;
        }

        });
  }
}

#if 0
-------command line to reproduce the error----------
identityTranslator -c -std=c++11 ares_calcndx2d_smallest.cc
rose_ares_calcndx2d_smallest.cc: In function 'void calcndx2d(NodalAdv_t*, Domain_t*)':
rose_ares_calcndx2d_smallest.cc:267:3: error: parse error in template argument list
 ::for_all_2d< class policy::parstream  , class xargs::index_only  , {
   ^
rose_ares_calcndx2d_smallest.cc:267:69: error: expected ';' before '{' token
 ::for_all_2d< class policy::parstream  , class xargs::index_only  , {
                                                                     ^
rose_ares_calcndx2d_smallest.cc:274:15: error: statement cannot resolve address of overloaded function
 double *nmass;} > (2 + 1,imax + 1,2,jmax + 1, [=] (int i,int j)
               ^
rose_ares_calcndx2d_smallest.cc:274:17: error: expected primary-expression before '>' token
 double *nmass;} > (2 + 1,imax + 1,2,jmax + 1, [=] (int i,int j)
                 ^

-----unparsed code : portion----------

void calcndx2d(NodalAdv_t *nl,Domain_t *domain)
{
int imax = nl -> imax;
int jmax = nl -> jmax;
int ip = nl -> ip;
int jp = nl -> jp;
int *ndxa = domain -> ndxa;
int *ndxd = domain -> ndxd;
int *ndxn = domain -> ndxn;
double *massflux = nl -> massflux;
double *nmass = domain -> nmass;
if (domain -> nslide == 0) {
::for_all_2d< class policy::parstream  , class xargs::index_only  , {
public: int jp;
int ip;
double *massflux;
int *ndxa;
int *ndxd;
int *ndxn;
double *nmass;} > (2 + 1,imax + 1,2,jmax + 1, [=] (int i,int j)
{
int off = j *  jp;
int iz = i *  ip + off;
double signflo = 0.0;
if ( massflux[iz] >= 0.0) {
signflo = 0.999999;
 ndxa[iz] = iz;
 ndxd[iz] = iz -  ip;
 ndxn[iz] = iz - 2 *  ip;
}
 else {
signflo = - 0.999999;
 ndxa[iz] = iz -  ip;
 ndxd[iz] = iz;
 ndxn[iz] = iz +  ip;
}
if (fabs( massflux[iz]) >  nmass[ ndxd[iz]]) {
 massflux[iz] = signflo *  nmass[ ndxd[iz]];
}
});
}
}

#endif
