// It is currently an issue that these codes are mutually exclusive to the new argument-based function lookup.

// RC-72:

namespace conduit 
   {
     struct Node {};
     void about(Node&);
     namespace relay 
        {
          void about(conduit::Node&);
          void about() 
             {
               Node n;
            // Original code: relay::about(n);
            // Unparsed code: about(n);
               relay::about(n);
             }
        }
   }



// RC-127

namespace Namespace_And_Class_Name_1 
   {
     class Namespace_And_Class_Name_1 
        {
        };

     enum Enum_Type_1 
        {
          ENUM_VALUE_1
        };

     void proc_2(Enum_Type_1 parm_1);

     class Class_1 
        {
          void proc_1();
        };
   }

using namespace Namespace_And_Class_Name_1;

void Class_1::proc_1() 
   {
  // Original code:                              proc_2(ENUM_VALUE_1);
  // Generated code: Namespace_And_Class_Name_1::proc_2(ENUM_VALUE_1);
     proc_2(ENUM_VALUE_1);
   }

