// This test code tests a number of different ways that templates can be used

class ClassType
   {
     public:
          template < class T > void publicTemplateFunction() {}
          template < class T > friend void publicTemplateFriendFunction() {}
   };


