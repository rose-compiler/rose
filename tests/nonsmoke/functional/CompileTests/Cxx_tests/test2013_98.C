class QList 
   {
     public:
          QList & operator=(const QList &list);
   };

class QGDItList : public QList
   {
     public:
          QList &operator=(const QList &list)
	          {
            // This unparses to: "return  = list;"
               return QList::operator= ( list );
             }
   };

