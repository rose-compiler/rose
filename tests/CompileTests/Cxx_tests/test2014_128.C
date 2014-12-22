#if 1
struct Domain_s;

#if 0
class _string 
   {
     public:
          _string (char*);
   };

typedef _string string;
#else
typedef class _string 
   {
     public:
          _string (char*);
   } string;
#endif

#endif

#if 0
#include <string>

struct Domain_s;
#endif

class DomainGraphViz
{
public:

#if 1
   explicit DomainGraphViz(Domain_s* domain, string prefix = "");
#else
   explicit DomainGraphViz(Domain_s* domain, string prefix);
#endif

   void graphIndicesAndMixSlots(Domain_s* domain, string prefix);
   void graphSpecFrac(Domain_s* domain, string prefix);
   void graphIreg(Domain_s* domain, string prefix);

   virtual ~DomainGraphViz();

  // private:

};
