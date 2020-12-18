package Private_Type_Definition is
   type Type_1 is private;
   type Type_2 is tagged private;
   type Type_3 is abstract tagged limited private;

   type Parent_Type_1 is tagged record
      Component_1 : Integer;
   end record;
   type Type_4 is new Parent_Type_1 with private;

private
   type Type_1 is new Integer;
   type Type_2 is tagged null record;
   type Type_3 is abstract tagged limited null record;

   type Type_4 is new Parent_Type_1 with record
      Component_2 : Float;
   end record;
end Private_Type_Definition;
