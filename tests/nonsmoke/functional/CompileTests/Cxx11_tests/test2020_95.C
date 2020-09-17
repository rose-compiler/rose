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
     proc_2(ENUM_VALUE_1);
   }

