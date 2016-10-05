struct LayoutDocEntry
   {
     enum Kind { MemberGroups };
   };

struct LayoutNavEntry
   {
     public:
          enum Kind { MainPage };
   };

class LayoutParser
   {
     class StartElementHandlerSection
        {
          private:
               typedef void (LayoutParser::*Handler)(LayoutDocEntry::Kind kind);

               void foobar_A() 
                  {
                    (m_parent->*m_handler)(m_kind);
                  }

               LayoutParser *m_parent;
               Handler m_handler;
               LayoutDocEntry::Kind m_kind;
        };

     class StartElementHandlerNavEntry
        {
          private:
               typedef void (LayoutParser::*Handler)(LayoutNavEntry::Kind kind);

               void foobar_B() 
                  {
                    (m_parent->*m_handler)(m_kind);
                  }

               LayoutParser *m_parent;
               Handler m_handler;
               LayoutNavEntry::Kind m_kind;

        };

   };

   
