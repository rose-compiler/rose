/*
Contributed by Jeff Keasler
Liao 1/27/2009
*/

class Domain 
{
  

  public: inline Domain()
{
  }
  

  inline ~Domain()
{
  }
  int notUsed;
}
;

class Decomposition 
{
  

  public: inline Decomposition()
{
    (this) -> numLocDom = 20;
  }
  

  inline ~Decomposition()
{
  }
  

  class Iterator 
{
    public: Iterator(const class Decomposition *d);
    virtual ~Iterator();
    int first();
    inline int isValid() const;
    int next();
    Domain *value() const;
    private: Iterator();
    int m_currentPosition;
    const class Decomposition *m_theDecomp;
  }
;
  class Domain *domains[20];
  int numLocDom;
}
;

Decomposition::Iterator::Iterator(const class Decomposition *d) : m_currentPosition(- 1), m_theDecomp(d)
{
  (this) ->  first ();
}

Decomposition::Iterator::~Iterator()
{
}

int Decomposition::Iterator::first()
{
  if (1 <= (this) -> m_theDecomp -> numLocDom) {
    (this) -> m_currentPosition = 0;
    return 1;
  }
   else {
    (this) -> m_currentPosition = - 1;
    return 0;
  }
}

inline int Decomposition::Iterator::isValid() const
{
  if (0 <= (this) -> m_currentPosition && (this) -> m_currentPosition < (this) -> m_theDecomp -> numLocDom) 
    return 1;
   else 
    return 0;
}

int Decomposition::Iterator::next()
{
  if (0 <= (this) -> m_currentPosition) {
    (this) -> m_currentPosition += 1;
    if ((this) -> m_currentPosition < (this) -> m_theDecomp -> numLocDom) {
      return 1;
    }
     else {
      (this) -> m_currentPosition = - 1;
      return 0;
    }
  }
   else 
    return 0;
}

Domain *Decomposition::Iterator::value() const
{
  if (0 <= (this) -> m_currentPosition && (this) -> m_currentPosition < (this) -> m_theDecomp -> numLocDom) {
    class Domain *d = (this) -> m_theDecomp -> domains[(this) -> m_currentPosition];
    return d;
  }
   else {
    return 0;
  }
}

int main(int argc,char *argv[])
{
  class Decomposition decomp;
  for (class Decomposition::Iterator ditr((&decomp)); (ditr .  isValid ()); ditr .  next ()) {
    class Domain *domain = ditr .  value ();
  }
  return 0;
}
