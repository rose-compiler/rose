class X
   {
     public:
          bool foo() { return m_exitOnFrameDelete.Yes; }

          int aaaaaaaaaaaaaaaaaaaa;

          class Y
             {
               public:
                    bool Yes;
             } m_exitOnFrameDelete;

       // bool foo() { return m_exitOnFrameDelete.Yes; }
   };

