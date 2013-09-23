class QString;
class QDataStream;

template<class T> class QValueList {};
template<class T> QDataStream& operator>>( QDataStream& s, QValueList<T>& l );

class QStringList : public QValueList<QString> {};

void foo(QDataStream & s, QStringList& l)
   {
     s >> (QValueList<QString>&)l;
   }
