// This is a copy of test2013_85.C (so that we can work on a simpler version).
// Note also that test2013_85.C is a copy of test2013_84.C (only much simpler).

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
            // This specifies that Handlier is a member function pointer.
               typedef void (LayoutParser::*Handler)(LayoutDocEntry::Kind kind);

            // This specifies that Handling is a non-member function pointer.
            // typedef void (*Handler)(LayoutDocEntry::Kind kind);
        };

     class StartElementHandlerNavEntry
        {
          private:
            // This specifies that Handlier is a member function pointer.
               typedef void (LayoutParser::*Handler)(LayoutNavEntry::Kind kind);

            // This specifies that Handling is a non-member function pointer.
            // typedef void (*Handler)(LayoutNavEntry::Kind kind);
        };

   };
