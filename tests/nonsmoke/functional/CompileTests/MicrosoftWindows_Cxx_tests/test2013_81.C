template <typename T>
class QList
   {
     public:
          T* last();
   };

class DBusXMLHandler
   {
     public:
          void getCurrentScope()
             {
            // m_scopeStack.last()->scope;
            // m_scopeStack.last();
             }

       // This is unparsed as a union (a bug).
       // class ScopeData
          struct ScopeData
             {
            // public:
               ScopeData(int s, int c)
                  : scope(s),
                    count(c)
                  { }

               int scope;
               int count;
             };

          QList<ScopeData> m_scopeStack;
   };



