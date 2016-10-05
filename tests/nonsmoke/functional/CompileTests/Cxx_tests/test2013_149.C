// This is a copy of test2013_85.C (so that we can work on a simpler version).
// Note also that test2013_85.C is a copy of test2013_84.C (only much simpler).

struct LayoutDocEntry
   {
  // enum Kind { MemberGroups };
     typedef float Kind;
   };

struct LayoutNavEntry
   {
     public:
       // enum Kind { MainPage };
          typedef int Kind;
   };

class LayoutParser {
     class StartElementHandlerSection
        {
          private:
            // typedef void (LayoutParser::*Handler)(LayoutDocEntry::Kind kind);
               typedef void (LayoutParser::*Handler)(LayoutDocEntry::Kind kind);
#if 1
               void foobar_A() 
                  {
                    (m_parent->*m_handler)(m_kind);
                  }

               LayoutParser *m_parent;
               Handler m_handler;
               LayoutDocEntry::Kind m_kind;
#endif
        };

     class StartElementHandlerNavEntry
        {
          private:
            // typedef void (LayoutParser::*Handler)(LayoutNavEntry::Kind kind);
               typedef void (LayoutParser::*Handler)(LayoutNavEntry::Kind kind);
#if 1
               void foobar_B() 
                  {
                    (m_parent->*m_handler)(m_kind);
                  }

               LayoutParser *m_parent;
               Handler m_handler;
               LayoutNavEntry::Kind m_kind;
#endif
        };

   };

