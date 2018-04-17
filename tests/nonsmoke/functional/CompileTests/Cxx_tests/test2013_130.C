// This is a copy of test2013_85.C (so that we can work on a simpler version).
// Note also that test2013_85.C is a copy of test2013_84.C (only much simpler).

struct LayoutDocEntry
   {
     enum Kind { MemberGroups };
   };

class X 
   {
     class StartElementHandlerSection
        {
          private:
#if 0
              typedef void (X::*Handler)(LayoutDocEntry::Kind kind);
#else
              typedef void (*Handler)(LayoutDocEntry::Kind kind);
#endif
            // LayoutDocEntry::Kind m_kind;
        };
   };

