class QGDictIterator {};

template <class T>
class QList 
   {
     public:
          QList &operator=(const QList &list);
   };

class QGDItList : public QList<QGDictIterator>
   {
     public:
          QGDItList &operator=(const QGDItList &list)
	          {
            // This unparses to: return (class QGDItList )( = list);
            // return (QGDItList&)QList<QGDictIterator>::operator=(list);
               return (QGDItList &) QList<QGDictIterator>::operator= ( list );
             }
   };

