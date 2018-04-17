

class QString {};
class QCString {};

class StartElementHandler
{
};

class EndElementHandler {};

class Translator 
   {
     public:
       // virtual QCString trPublicSlots(); // = 0;
          virtual QCString trPublicSlots() = 0;
          virtual QCString trPublicTypes() = 0;
   };

extern Translator *theTranslator;
extern bool setTranslator(const char *languageName);

enum MemberListType
{
  MemberListType_pubSlots                = 8,
  MemberListType_pubTypes                = 19,
  MemberListType_memberGroup             = 69
};

template<class type> class QDict // : public QGDict
   {
     public:
          void insert( const QString &k, const type *d ); //	{ QGDict::look_string(k,(Item)d,1); }
   };

class QXmlAttributes {};

struct LayoutDocEntry
   {
     virtual ~LayoutDocEntry() {}
     enum Kind { MemberGroups, MemberDeclStart };
     virtual Kind kind() const = 0;
   };

class LayoutNavEntry
   {
     public:
          enum Kind { MainPage,Pages,Modules,UserGroup };
   };



//---------------------------------------------------------------------------------

class LayoutParser // : public QXmlDefaultHandler
{
  private:
    class StartElementHandler
    {
    };

    class StartElementHandlerMember : public StartElementHandler
    {
        typedef void (LayoutParser::*Handler)(const QXmlAttributes &attrib,
                                              MemberListType type,
                                              const QCString &title,
                                              const QCString &subtitle); 
      public:
#if 1
     // This falis because the source sequence position for the "const QCString &ss = QCString()" is too far off from the function's source sequence entry.
        StartElementHandlerMember(LayoutParser *parent, 
                                  Handler h,
                                  MemberListType type,
                                  const QCString &tl,
                                  const QCString &ss = QCString()
                                 ) 
          : m_parent(parent), m_handler(h), m_type(type),
            m_title(tl), m_subscript(ss) {}
#else
     // This works, because the source sequence position is closer to being correct in this case.
        StartElementHandlerMember(LayoutParser *parent, Handler h,MemberListType type,const QCString &tl,const QCString &ss = QCString()) : m_parent(parent), m_handler(h), m_type(type),m_title(tl), m_subscript(ss) {}
#endif
        void operator()(const QXmlAttributes &attrib) 
        { 
          (m_parent->*m_handler)(attrib,m_type,m_title,m_subscript); 
        }

      private:
        LayoutParser *m_parent;
        Handler m_handler;
        MemberListType m_type;
        QCString m_title;
        QCString m_subscript;
    };


  public:

    void init()
    {
      StartElementHandlerMember* XXX = new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,MemberListType_pubTypes,theTranslator->trPublicTypes());

   // This version of the code will work...because the default parameter specification is not required here.
   // StartElementHandlerMember* XXX = new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,MemberListType_pubTypes,theTranslator->trPublicTypes(),QCString());
    }


    void startMemberDeclEntry(const QXmlAttributes &attrib,MemberListType type,const QCString &title,const QCString &subscript)
    {
    }



};


