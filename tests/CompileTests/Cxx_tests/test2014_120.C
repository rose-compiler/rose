#include <vector>

namespace XXX {
  namespace Materials {

template <typename T>
class HistVarSet 
   {
     public:
#if 1
          void packVars( HistVarSet<T*> const & hist_list, HistVarSet<bool> const &log_flags, int index );
#endif

          std::vector<T>  elas;
   };

  }  // closing brace for Materials namespace statement
}  // closing brace for XXX namespace statement


struct Domain_s;
struct State_s;

namespace XXX {
  namespace Materials {

class HistVarManager 
   {
     public:
          int setHistList( HistVarSet<double*> &hist_list, HistVarSet<bool> &log_flags, /* MS_Matmodel const *msModel, */ State_s const *state, Domain_s *domain);
     private:
          int setHistListPtrs( std::vector<double*> &histList, std::vector<bool> &logFlag , /* MS_Model_t model, */ State_s const *state, Domain_s*  domain);
   };

  }  // namespace Materials
}  // namespace XXX


namespace XXX {
  namespace Materials {

int HistVarManager::setHistList( HistVarSet<double*> &hist_list, HistVarSet<bool> &log_flags, State_s const *state, Domain_s *domain)
   {
     int status = 0;

  // BUG: This is unparsed as:
  // status = (this) ->  setHistListPtrs (hist_list . HistVarSet< double *> ::elas,log_flags . HistVarSet< bool > ::elas,msModel -> _ms_matmodel::elasModel,state,domain);
  // status = setHistListPtrs( hist_list.elas, log_flags.elas, msModel->elasModel, state, domain);
     status = setHistListPtrs( hist_list.elas, log_flags.elas, state, domain);

     return status;
   }

  }  // closing brace for Materials namespace statement
}  // closing brace for XXX namespace statement


