
class wxAppBase
   {
     public:
       // wxAppBase();
       // virtual ~wxAppBase();

#if 0
          void SetExitOnFrameDelete(bool flag)
             { 
               m_exitOnFrameDelete = flag ? Yes : No; 
             }
#endif
#if 1
          bool GetExitOnFrameDelete() const 
             { 
               return m_exitOnFrameDelete == Yes;
             }
#endif
     // protected:

          enum
             {
               Later = -1,
               No,
               Yes
             } m_exitOnFrameDelete;
   };

