/* real8 type */
typedef double real8 ;
#define real8_hasBeenDefinedAs_double

/* These defines are used to select the fundemental datatype used to represent
 * a global id.
 */
#include <stdint.h>
#ifdef __cplusplus
#include <limits.h>
#endif
#define       ALE_INT64 int64_t

#define AGID_MAX INT_MAX
#define GID_FMT "d"
#define GIDTYPE int


#ifdef __cplusplus

class globalID {
   friend globalID operator + (const int &, const globalID &) ;
   friend globalID operator - (const int &, const globalID &) ;
   friend globalID operator * (const int &, const globalID &) ;
public:
   inline globalID() {};
   inline globalID(const globalID& g) : m_gid(g.m_gid) {};
   inline explicit globalID(const real8 r) : m_gid((GIDTYPE)r) {};
   inline explicit globalID(const long long l) : m_gid((GIDTYPE)l) {};
   inline explicit globalID(int i) : m_gid(i) {};
   /// assignment operator
   inline globalID& operator = (const globalID &g) { m_gid = g.m_gid; return *this ;}
   /// sum into operator
   inline globalID& operator += (const globalID & g) { m_gid += g.m_gid; return *this ;}
   /// subtract from operator
   inline globalID& operator -= (const globalID & g) { m_gid -= g.m_gid; return *this ;}
   /// multiply into operator
   inline globalID& operator *= (const globalID & g) { m_gid *= g.m_gid; return *this ;}
   /// sum an int into operator
   inline globalID& operator += (const int & g) { m_gid += g; return *this ;}
   /// subtract an int into operator
   inline globalID& operator -= (const int & g) { m_gid -= g; return *this ;}
   /// multiply an int into operator
   inline globalID& operator *= (const int & g) { m_gid *= g; return *this ;}
   /// postfix increment by 1
   inline globalID& operator ++ () {++m_gid; return *this;}
   /// prefix increment by 1
   inline globalID operator ++ (int) {return globalID(m_gid++) ;}
   /// postfix decrement by 1
   inline globalID& operator -- () {--m_gid; return *this;}
   /// negation
   inline globalID& operator - () {m_gid = -m_gid; return *this;}
   /// prefix decrement by 1
   inline globalID operator -- (int) {return globalID(m_gid--) ;}
   ///  add
   inline globalID operator + (const globalID &g) const { return globalID(*this) += g ;}
   ///  subtract
   inline globalID operator - (const globalID &g) const { return globalID(*this) -= g ;}
   ///  modulo
   inline globalID operator % (const globalID &g) const { return globalID (m_gid % g.m_gid) ;}
   ///  divide
   inline globalID operator / (const globalID &g) const { return globalID (m_gid / g.m_gid) ;}
   ///  divide by int
   inline globalID operator / (const int &i) const { return globalID (m_gid / i) ;}
   ///  multiply
   inline globalID operator * (const globalID &g) const { return globalID (m_gid * g.m_gid) ;}

   ///  add integer
   inline globalID operator + (const int i) const { return globalID(*this) += i ; }
   ///  subtract integer
   inline globalID operator - (const int i) const { return globalID(*this) -= i ;}
   ///  multiply by integer
   inline globalID operator * (const int i) const { return globalID(m_gid * i) ;}
   ///  modulo by an int - returns int
   inline int operator % (const int &i) const { return static_cast<int>(m_gid % i) ;}

   /// equality test
   inline bool operator == (const globalID &g) const {return m_gid == g.m_gid ;}
   /// not equal test
   inline bool operator != (const globalID &g) const {return !(*this == g) ;}
   /// greater than
   inline int operator > (const globalID &g) const {return m_gid > g.m_gid ;}
   /// less than
   inline int operator < (const globalID &g) const {return m_gid < g.m_gid ;}
   /// greater than or equal
   inline int operator >= (const globalID &g) const {return m_gid >= g.m_gid ;}
   /// less than or equal
   inline int operator <= (const globalID &g) const {return m_gid <= g.m_gid ;}
   /// convert to real
   inline real8 Real8() const {return (real8) m_gid ;}
   /// convert to int
   inline int   Int() const {
      return (int) m_gid ;
   }
   /// convert to intrinsic type
   inline
   int
   Value() const {return m_gid;}

   /// used to reduce number of .Int operations and explain use
   /// we are subtracting the base off a number and returning local
   /// (int) value
   inline int Remove(const globalID &g) const { return (int)(m_gid - g.m_gid) ;}

private:
   int m_gid ;
} ;

#endif

int main(int argc, char *argv[])
{
   globalID value = globalID(100)*7 + 5 ;

   return 0 ;
}

