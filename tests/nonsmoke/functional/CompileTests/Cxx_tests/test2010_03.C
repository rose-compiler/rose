// A set of four bugs submitted by Jeff Keasler.
// This test code represent one and the next three 
// test codes represent the other three separately.

////////////////////////////////////////////
//////////// DECLARATIONS for bugs at bottom
////////////////////////////////////////////

typedef double real8 ;

#ifndef GLOBALID_IS_64BIT
#define GLOBALID_IS_64BIT 0
#endif

#ifndef DOIT_THE_NEW_WAY
#  define       ALE_INT64 long long
#else
#  ifdef USE_FOR_64BIT
#     define    ALE_INT64 USE_FOR_64BIT
#  else
#     ifdef _WIN32
#        define ALE_INT64 __int64
#     else
#        define ALE_INT64 int64
#     endif
#  endif
#endif

class globalID {
   friend globalID operator + (const int &, const globalID &) ;
   friend globalID operator - (const int &, const globalID &) ;
   friend globalID operator * (const int &, const globalID &) ;
public:
   inline globalID() {};
   inline globalID(const globalID& g) : m_gid(g.m_gid) {};
#if GLOBALID_IS_64BIT
   inline explicit globalID(const ALE_INT64& i) : m_gid(i) {};
#else
   inline explicit globalID(int i) : m_gid(i) {};
#endif
   inline globalID& operator = (const globalID &g) { m_gid = g.m_gid; return *this ;}
   inline globalID& operator += (const globalID & g) { m_gid += g.m_gid; return *this ;}
   inline globalID& operator -= (const globalID & g) { m_gid -= g.m_gid; return *this ;}
   inline globalID& operator *= (const globalID & g) { m_gid *= g.m_gid; return *this ;}
   inline globalID& operator += (const int & g) { m_gid += g; return *this ;}
   inline globalID& operator -= (const int & g) { m_gid -= g; return *this ;}
   inline globalID& operator *= (const int & g) { m_gid *= g; return *this ;}
   inline globalID& operator ++ () {++m_gid; return *this;}
   inline globalID operator ++ (int) {return globalID(m_gid++) ;}
   inline globalID& operator -- () {--m_gid; return *this;}
   inline globalID& operator - () {m_gid = -m_gid; return *this;}
   inline globalID operator -- (int) {return globalID(m_gid--) ;}
   inline globalID operator + (const globalID &g) const { return globalID(*this) += g ;}
   inline globalID operator - (const globalID &g) const { return globalID(*this) -= g ;}
   inline globalID operator % (const globalID &g) const { return globalID (m_gid % g.m_gid) ;}
   inline globalID operator / (const globalID &g) const { return globalID (m_gid / g.m_gid) ;}
   inline globalID operator / (const int &i) const { return globalID (m_gid / i) ;}
   inline globalID operator * (const globalID &g) const { return globalID (m_gid * g.m_gid) ;}

   inline globalID operator + (const int i) const { return globalID(*this) += i ; }
   inline globalID operator - (const int i) const { return globalID(*this) -= i ;}
   inline globalID operator * (const int i) const { return globalID(m_gid * i) ;}
   inline int operator % (const int &i) const { return static_cast<int>(m_gid % i) ;}

   inline bool operator == (const globalID &g) const {return m_gid == g.m_gid ;}
   inline bool operator != (const globalID &g) const {return !(*this == g) ;}
   inline int operator > (const globalID &g) const {return m_gid > g.m_gid ;}
   inline int operator < (const globalID &g) const {return m_gid < g.m_gid ;}
   inline int operator >= (const globalID &g) const {return m_gid >= g.m_gid ;}
   inline int operator <= (const globalID &g) const {return m_gid <= g.m_gid ;}

   inline real8 Real8() const {return (real8) m_gid ;}
   inline int   Int() const {
      return (int) m_gid ;
   }
   /// convert to intrinsic type
   inline
#if GLOBALID_IS_64BIT
   ALE_INT64
#else
   int
#endif
   Value() const {return m_gid;}

   /// used to reduce number of .Int operations and explain use
   /// we are subtracting the base off a number and returning local
   /// (int) value
   inline int Remove(const globalID &g) const { return (int)(m_gid - g.m_gid) ;}

private:
#if GLOBALID_IS_64BIT
   ALE_INT64 m_gid ;
#else
   int m_gid ;
#endif
} ;

////////////////////////////////////////////
////////////  *BUGS*  *BUGS*  *BUGS* *BUGS*
////////////////////////////////////////////

bool bug1(globalID globalZone)
{
   if (globalZone < globalID(0))
      return false;
   else
      return true;
}

#if 0
// these are enabled separately in the next three test codes

/* Frightening order-of-operations change */
void bug2(int *locDom, globalID div, int mod)
{
   globalID startElem = locDom[0]*div + ((locDom[0] < mod) ? locDom[0] : mod) ;
}

void bug3(globalID len) {
   for (globalID i=globalID(0); i<len; ++i) {
   }
}

struct inner
{
   globalID valA ;
   int valB ;
} ;

struct outer
{
   struct inner g;
} ;

void bug4(struct outer *ptr)
{
   if (ptr->g.valB <= 16 && ptr->g.valA < globalID(200000)) {
     /* stuff */ ;
   }
}
#endif
