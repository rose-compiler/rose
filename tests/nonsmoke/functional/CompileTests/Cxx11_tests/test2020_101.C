// RC-72:

namespace conduit 
   {
     struct Node {};
     void about(Node&);
     namespace relay 
        {
          void about(conduit::Node&);
          void xxx_about() 
             {
               Node n;
            // Original code: relay::about(n);
            // Unparsed code: about(n);
               relay::about(n);
             }
        }
   }


