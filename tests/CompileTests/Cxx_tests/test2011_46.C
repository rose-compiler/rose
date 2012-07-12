namespace A
   {
     class baseClass
        {
          public:
               int publicBaseClassMemberData;
        };

     class Bclass : baseClass
        {
          public:
            // If we use a using declaration then we can't have a data member with the same name.
            // int publicBaseClassMemberData;

       // This is a using declaration ... and redundant because baseClass 
       // is a base class of Bclass and there is only a public data member.
          using baseClass::publicBaseClassMemberData;
        };
   }

