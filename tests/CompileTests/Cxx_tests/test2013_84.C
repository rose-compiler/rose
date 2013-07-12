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
          QCString trPublicSlots(); // = 0;
   };

extern Translator *theTranslator;
extern bool setTranslator(const char *languageName);

enum MemberListType
{
  MemberListType_privateLists       = 0x0800,
  MemberListType_detailedLists      = 0x1000,
  MemberListType_declarationLists   = 0x2000,
  MemberListType_documentationLists = 0x4000,

  MemberListType_pubMethods              = 0,
  MemberListType_proMethods              = 1,
  MemberListType_pacMethods              = 2,
  MemberListType_priMethods              = 3 + MemberListType_privateLists,
  MemberListType_pubStaticMethods        = 4,
  MemberListType_proStaticMethods        = 5,
  MemberListType_pacStaticMethods        = 6,
  MemberListType_priStaticMethods        = 7 + MemberListType_privateLists,
  MemberListType_pubSlots                = 8,
  MemberListType_proSlots                = 9,
  MemberListType_priSlots                = 10 + MemberListType_privateLists,
  MemberListType_pubAttribs              = 11,
  MemberListType_proAttribs              = 12,
  MemberListType_pacAttribs              = 13,
  MemberListType_priAttribs              = 14 + MemberListType_privateLists,
  MemberListType_pubStaticAttribs        = 15,
  MemberListType_proStaticAttribs        = 16,
  MemberListType_pacStaticAttribs        = 17,
  MemberListType_priStaticAttribs        = 18 + MemberListType_privateLists,
  MemberListType_pubTypes                = 19,
  MemberListType_proTypes                = 20,
  MemberListType_pacTypes                = 21,
  MemberListType_priTypes                = 22 + MemberListType_privateLists,
  MemberListType_related                 = 23,
  MemberListType_signals                 = 24,
  MemberListType_friends                 = 25,
  MemberListType_dcopMethods             = 26,
  MemberListType_properties              = 27,
  MemberListType_events                  = 28,

  MemberListType_typedefMembers          = 29 + MemberListType_detailedLists,
  MemberListType_enumMembers             = 30 + MemberListType_detailedLists,
  MemberListType_enumValMembers          = 31 + MemberListType_detailedLists,
  MemberListType_functionMembers         = 32 + MemberListType_detailedLists,
  MemberListType_relatedMembers          = 33 + MemberListType_detailedLists,
  MemberListType_variableMembers         = 34 + MemberListType_detailedLists,
  MemberListType_propertyMembers         = 35 + MemberListType_detailedLists,
  MemberListType_eventMembers            = 36 + MemberListType_detailedLists,
  MemberListType_constructors            = 37 + MemberListType_detailedLists,

  MemberListType_allMembersList          = 38,

  MemberListType_decDefineMembers        = 39 + MemberListType_declarationLists,
  MemberListType_decProtoMembers         = 40 + MemberListType_declarationLists, 
  MemberListType_decTypedefMembers       = 41 + MemberListType_declarationLists,
  MemberListType_decEnumMembers          = 42 + MemberListType_declarationLists,
  MemberListType_decFuncMembers          = 43 + MemberListType_declarationLists,
  MemberListType_decVarMembers           = 44 + MemberListType_declarationLists,
  MemberListType_decEnumValMembers       = 45 + MemberListType_declarationLists,
  MemberListType_decPubSlotMembers       = 46 + MemberListType_declarationLists,
  MemberListType_decProSlotMembers       = 47 + MemberListType_declarationLists,
  MemberListType_decPriSlotMembers       = 48 + MemberListType_declarationLists,
  MemberListType_decSignalMembers        = 49 + MemberListType_declarationLists,
  MemberListType_decEventMembers         = 50 + MemberListType_declarationLists,
  MemberListType_decFriendMembers        = 51 + MemberListType_declarationLists,
  MemberListType_decPropMembers          = 52 + MemberListType_declarationLists,

  MemberListType_docDefineMembers        = 53 + MemberListType_documentationLists,
  MemberListType_docProtoMembers         = 54 + MemberListType_documentationLists,
  MemberListType_docTypedefMembers       = 55 + MemberListType_documentationLists,
  MemberListType_docEnumMembers          = 56 + MemberListType_documentationLists,
  MemberListType_docFuncMembers          = 57 + MemberListType_documentationLists,
  MemberListType_docVarMembers           = 58 + MemberListType_documentationLists,
  MemberListType_docEnumValMembers       = 59 + MemberListType_documentationLists,
  MemberListType_docPubSlotMembers       = 60 + MemberListType_documentationLists,
  MemberListType_docProSlotMembers       = 61 + MemberListType_documentationLists,
  MemberListType_docPriSlotMembers       = 62 + MemberListType_documentationLists,
  MemberListType_docSignalMembers        = 63 + MemberListType_documentationLists,
  MemberListType_docEventMembers         = 64 + MemberListType_documentationLists,
  MemberListType_docFriendMembers        = 65 + MemberListType_documentationLists,
  MemberListType_docPropMembers          = 66 + MemberListType_documentationLists,

  MemberListType_redefinedBy             = 67,
  MemberListType_enumFields              = 68,
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


class LayoutParser // : public QXmlDefaultHandler
   {
     class StartElementHandlerSection : public StartElementHandler
        {
          typedef void (LayoutParser::*Handler)(LayoutDocEntry::Kind kind,const QXmlAttributes &attrib,const QCString &title); 
          public:
               StartElementHandlerSection(LayoutParser *parent, LayoutDocEntry::Kind k,Handler h,const QCString &title) : m_parent(parent), m_kind(k), m_handler(h), m_title(title) {}
               void operator()(const QXmlAttributes &attrib) 
                  {
                    (m_parent->*m_handler)(m_kind,attrib,m_title); 
                  }
          private:
               LayoutParser *m_parent;
               LayoutDocEntry::Kind m_kind;
               Handler m_handler;
               QCString m_title;
        };


     class StartElementHandlerMember // : public StartElementHandler
        {
          private:
            // typedef void (LayoutParser::*Handler)(const QXmlAttributes &attrib, MemberListType type, const QCString &title, const QCString &subtitle); 
               typedef void (LayoutParser::*Handler)(const QXmlAttributes &attrib, MemberListType type); 

          public:
            // StartElementHandlerMember(LayoutParser *parent, Handler h, MemberListType type, const QCString &tl,const QCString &ss = QCString() ) : m_parent(parent), m_handler(h), m_type(type),m_title(tl), m_subscript(ss) {}
            // StartElementHandlerMember(LayoutParser *parent, Handler h, MemberListType type, const QCString &tl ) {}
            // StartElementHandlerMember(LayoutParser *parent, Handler h ) {}
               StartElementHandlerMember(LayoutParser *parent, Handler h, const QCString &ss = QCString() ) {}

               void operator()(const QXmlAttributes &attrib) 
                  {
                 // (m_parent->*m_handler)(attrib,m_type,m_title,m_subscript);
                    (m_parent->*m_handler)(attrib,m_type);
                  }

          private:
               LayoutParser *m_parent;
               Handler m_handler;
               MemberListType m_type;
               QCString m_title;
               QCString m_subscript;
        };


     class StartElementHandlerNavEntry // : public StartElementHandler
        {
          private:
               typedef void (LayoutParser::*Handler)(LayoutNavEntry::Kind kind,const QXmlAttributes &attrib,const QCString &title); 

          public:
               StartElementHandlerNavEntry(LayoutParser *parent,LayoutNavEntry::Kind kind,Handler h,const QCString &tl) : m_parent(parent), m_kind(kind), m_handler(h), m_title(tl) {}

               void operator()(const QXmlAttributes &attrib) 
                  {
                 // DQ (3/31/2013): This unparses as: ((this) -> m_parent ->* (this) -> m_handler)((this) -> m_kind,attrib,(this) -> m_title);
                    (m_parent->*m_handler)(m_kind,attrib,m_title); 
                  }

          private:
               LayoutParser *m_parent;
               LayoutNavEntry::Kind m_kind;
               Handler m_handler;
               QCString m_title;
        };

     public:
       // void startMemberDeclEntry(const QXmlAttributes &attrib,MemberListType type,const QCString &title,const QCString &subscript) {}
          void startMemberDeclEntry(const QXmlAttributes &attrib,MemberListType type) {}

          void init()
             {
            // m_sHandler.insert("class/memberdecl/publicslots", new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,MemberListType_pubSlots,theTranslator->trPublicSlots())); 
            // StartElementHandlerMember* XXX = new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry,MemberListType_pubSlots,theTranslator->trPublicSlots());
               StartElementHandlerMember* XXX = new StartElementHandlerMember(this,&LayoutParser::startMemberDeclEntry);
             }

     private:
       // LayoutParser() : m_sHandler(163), m_eHandler(17), m_invalidEntry(FALSE) { }
          LayoutParser() { }

          QDict<StartElementHandler> m_sHandler;
          QDict<EndElementHandler>   m_eHandler;
          QCString m_scope;
          int m_part;
          LayoutNavEntry *m_rootNav;
          bool m_invalidEntry;
   };
