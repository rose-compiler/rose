
class X
   {
     public:
          bool foo() { return m_exitOnFrameDelete == Yes; }

       // This does not unparse properly.
          typedef enum
             {
               Yes
             } exit_type;

          exit_type m_exitOnFrameDelete;
   };

