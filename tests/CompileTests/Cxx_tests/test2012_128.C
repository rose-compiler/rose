class MutableNodeRefList 
   {
     public:

          void	swap()
             {
               eOrder temp = m_order;
             }

          enum eOrder { eUnknownOrder };

          eOrder m_order;
   };

