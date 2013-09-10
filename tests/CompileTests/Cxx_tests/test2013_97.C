#define Q_UINT32 int

template<class type> class QArray
   {
     public:
          QArray() {}
   };

class QCString 
   {
     public:
          QCString() {}
   };

class QDataStream
   {
     public:
          QDataStream();
          QDataStream &operator>>( Q_UINT32 &i );
   };

QDataStream & operator>>( QDataStream &s, QArray<char> &a )
// QDataStream &operator>>( QDataStream &s, int &a )
   {
     Q_UINT32 len;

     s >> len;

     return s;
   }










