package Test_Unit is
   type Range_Type is new Integer range 1 .. 10;
   procedure Do_It (This : in Range_Type);
   Dont_Like_5 : Exception;
   package Parent_Class is
      type Object is abstract tagged record
         Component_1 : Integer := 2;
      end record;
      procedure Method_1 (This : in out Object);
   end Parent_Class;
   package Child_Class is
      type Object is new Parent_Class.Object with record
         Component_2 : Integer := 10;
      end record;
      overriding procedure Method_1 (This : in out Object);
   end Child_Class;
end Test_Unit;
