package Variant_Record is

   type Record_1 (Discriminant_1 : Boolean) is record
      case Discriminant_1 is
         when True =>
            Component_2 : Integer;
         when False =>
            Component_3 : Float;
      end case;
   end record;

   type Record_With_Others (Discriminant_1 : Boolean) is record
      case Discriminant_1 is
         when True =>
            Component_2 : Integer;
         when others =>
            Component_3 : Float;
      end case;
   end record;

   type Record_With_Default_Discriminant
     (Discriminant_1 : Boolean := True)
   is record
      case Discriminant_1 is
         when True =>
            Component_2 : Integer;
         when False =>
            Component_3 : Float;
      end case;
   end record;

   type Record_With_Two_Discriminants
     (Discriminant_1 : Boolean;
      Discriminant_2 : Boolean)
   is record
      case Discriminant_1 is
         when True =>
            Component_2 : Integer;
         when False =>
            Component_3 : Float;
      end case;
   end record;

   type Record_With_Non_Variant (Discriminant_1 : Boolean) is record
      Component_1 : Natural;
      case Discriminant_1 is
         when True =>
            Component_2 : Integer;
         when False =>
            Component_3 : Float;
      end case;
   end record;

   type Record_With_Multiple_Variant_Components (Discriminant_1 : Boolean) is record
      case Discriminant_1 is
         when True =>
            Component_2  : Integer;
            Component_21 : Integer;
         when False =>
            Component_3  : Float;
            Component_31 : Float;
      end case;
   end record;



end Variant_Record;
