/*
Contributed by Jeff Keasler

Liao 1/27/2009
*/
class Domain {
public:
   Domain() { } ;
   ~Domain() { } ;
  int notUsed ;
} ;

class Decomposition {
public:
     Decomposition() { numLocDom = 20 ; } ;
     ~Decomposition() { } ;

     class Iterator
     {
     public:
       Iterator ( const Decomposition* d );
       virtual              ~Iterator ( );

       int                   first ( );
       int                   isValid ( ) const;
       int                   next ( );
       Domain*               value ( ) const;

     private:
       Iterator ( );

       int                   m_currentPosition;
       const Decomposition*  m_theDecomp;
     };

     Domain *domains[20] ;
     int numLocDom ;
} ;


Decomposition::Iterator::Iterator ( const Decomposition* d
                                  ) : m_currentPosition(-1)
                                    , m_theDecomp(d)
{
   first() ;
}

Decomposition::Iterator::~Iterator ( )
{
}

int Decomposition::Iterator::first ( )
{
   if (1 <= m_theDecomp->numLocDom)
   {
      m_currentPosition = 0;
      return 1;
   }
   else
   {
      m_currentPosition = -1;
      return 0;
   }
}

inline int Decomposition::Iterator::isValid ( ) const
{
   if (0 <= m_currentPosition && m_currentPosition < m_theDecomp->numLocDom)
      return 1;
   else
      return 0;
}

int Decomposition::Iterator::next ( )
{
   if (0 <= m_currentPosition)
   {
      m_currentPosition += 1;

      if (m_currentPosition < m_theDecomp->numLocDom)
      {
         return 1;
      }
      else
      {
         m_currentPosition = -1;
         return 0;
      }
   }
   else
      return 0;
}

Domain* Decomposition::Iterator::value ( ) const
{
   if (0 <= m_currentPosition && m_currentPosition < m_theDecomp->numLocDom)
   {
      Domain* d   = m_theDecomp->domains[m_currentPosition];
      return d;
   }
   else
   {
      return 0;
   }
}

int main(int argc, char *argv[])
{
   Decomposition decomp ;
   for (Decomposition::Iterator ditr(&decomp); ditr.isValid(); ditr.next()) {
      Domain* domain = ditr.value();
   }
  return 0;
} 
