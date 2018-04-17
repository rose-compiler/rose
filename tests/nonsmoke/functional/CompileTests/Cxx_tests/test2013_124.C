#define REFERENCE_BEFORE_DECLARATION 1

class QTextStreamPrivate 
   {
     public:
#if (REFERENCE_BEFORE_DECLARATION == 0)
          enum SourceType { NotSet };
          SourceType sourceType;
#endif

          QTextStreamPrivate() 
             : sourceType( NotSet )
             {
             }

#if (REFERENCE_BEFORE_DECLARATION == 1)
          enum SourceType { NotSet };
          SourceType sourceType;
#endif
   };
