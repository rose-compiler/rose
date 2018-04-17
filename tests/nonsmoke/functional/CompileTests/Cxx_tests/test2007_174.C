// This is a minimal subset of test2004_30.C which demonstrated a bug
// with how scopes were held in the SgUnparse_Info object in the unparser.
// Causing protectedDoubleValue to be unparsed as classType::protectedDoubleValue.
// Which was an error for the g++ 4.2 compiler.
class classType
   {
     public:
          friend void publicFriendMemberFunction () {};

     private:
          double protectedDoubleValue;
   };







