#if 0
class Domain_s {};
class string 
   {
     public:
          string (char*);
   };
#endif

#if 1
#include <string>

struct Domain_s;
#endif

class DomainGraphViz
{
public:

#if 1
   explicit DomainGraphViz(Domain_s* domain, std::string prefix = "");
#else
   explicit DomainGraphViz(Domain_s* domain, std::string prefix);
#endif

   void graphIndicesAndMixSlots(Domain_s* domain, std::string prefix);
   void graphSpecFrac(Domain_s* domain, std::string prefix);
   void graphIreg(Domain_s* domain, std::string prefix);

   virtual ~DomainGraphViz();

  // private:

};
