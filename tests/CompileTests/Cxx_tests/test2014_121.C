struct MS_Matmodel;

namespace XXX {
  namespace Materials {

template <typename T>
class HistVarSet 
   {
     public:
          HistVarSet (MS_Matmodel* msModel) {}
#if 1
          void packVars( HistVarSet<T*> const & hist_list, HistVarSet<bool> const &log_flags, int index );
#endif
   };

  }  // closing brace for Materials namespace statement
}  // closing brace for XXX namespace statement

struct Domain_t;
struct State_t;

typedef float real8;

namespace XXX {
  namespace Materials {

     void MSLib_Material_Driver( State_t*   state, const int* ndx, const int  len, Domain_t*  domain )
        {
          MS_Matmodel* msModel = 0L;

          HistVarSet<real8>  hist(msModel);
          HistVarSet<real8>  hist_updt(msModel);
          HistVarSet<real8*> hist_list(msModel);
          HistVarSet<bool>   log_flags(msModel);
        }

  }  // closing brace for Materials namespace statement
}  // closing brace for XXX namespace statement


