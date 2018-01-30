#include <vector>
typedef int    Index_t ; 

struct Domain
{
  public:
    Index_t&  numNode()            { return m_numNode ; }

    void AllocateNodeElemIndexes()
    {
      Index_t numNode = this->numNode() ;
    } 

#if 0  // the best inline result should look like the following
    void AllocateNodeElemIndexes_inlined()
    {
      Index_t numNode = m_numNode; // call site 1 inlined
    } 
#endif

  private:
    Index_t   m_numNode ;
} domain; 


