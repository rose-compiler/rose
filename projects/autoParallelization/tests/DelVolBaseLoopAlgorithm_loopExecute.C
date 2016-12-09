// A test case extracted from loop suite
// Representing unparsing error after loop normalization

// ((LoopAlgorithm *)(this)) -> m_Domain -> m_kmax 
// complains about protected member is accessed illegally. 

namespace loopsuite {

  class StructuredDomain
  {
    public:
      int m_npnl, m_kmax, m_jmax;
  };

  class DelVolBaseLoopAlgorithm
  {
    public: 
      void loopExecute();

    private:
      int m_jp, m_kp;

    protected:
      const StructuredDomain* m_Domain;

  };


  void DelVolBaseLoopAlgorithm::loopExecute()
  {
    int j,k;

    for ( k=m_Domain->m_npnl; k < m_Domain->m_kmax; k++ ) {
      for ( j=m_Domain->m_npnl; j < m_Domain->m_jmax; j++ ) {                                                                                        
        int off = j * m_jp + k * m_kp ;                         
      }
    }
  }

}
