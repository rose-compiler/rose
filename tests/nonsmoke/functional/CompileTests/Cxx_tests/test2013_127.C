class QString;
class QDataStream;

template <class T> class QValueList {};

template<class T>
inline QDataStream& operator>>( QDataStream& s, QValueList<T>& l )
   {
     return s;
   }

template<class T>
inline QDataStream& operator<<( QDataStream& s, const QValueList<T>& l )
   {
     return s;
   }

class QStringList : public QValueList<QString> {};

extern QDataStream &operator>>( QDataStream &, QStringList& );
extern QDataStream &operator<<( QDataStream &, const QStringList& );

QDataStream &operator>>( QDataStream & s, QStringList& l )
   {
  // Should be unparsed as: return s >> (QValueList<QString>&)l;
     return s >> (QValueList<QString>&)l;
   }

QDataStream &operator<<( QDataStream & s, const QStringList& l )
   {
  // Should be unparsed as: return s << (const QValueList<QString>&)l;
     return s << (const QValueList<QString>&)l;
   }

