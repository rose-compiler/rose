#include <vector>
typedef int    Index_t ; 

struct Domain{
  public:
    void AllocateNodeElemIndexes()
    {
      Index_t numNode = this->numNode() ;

      for (Index_t i=0;i<numNode;++i) {
        nodeElemCount(i)=0;
      }   
    } 

#if 0  // the best inline result should look like the following
    void AllocateNodeElemIndexes_inlined()
    {
      Index_t numNode = m_numNode; // call site 1 inlined

      for (Index_t i=0;i<numNode;++i) {
        m_nodeElemCount[i] =0;  // call site 2 inlined
      }   
    } 
#endif

    Index_t& nodeElemCount(Index_t idx) { return m_nodeElemCount[idx] ; }
    Index_t&  numNode()            { return m_numNode ; }

  private:
    std::vector<Index_t> m_nodeElemCount ;
    Index_t   m_numNode ;


} domain; 


