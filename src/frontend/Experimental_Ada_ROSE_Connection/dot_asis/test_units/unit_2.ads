with Ada.Strings.Unbounded;

package Unit_2 is

   type Range_Type is new Integer range 1 .. 10;

   procedure Do_It (This : in Range_Type);

   Dont_Like_5 : Exception;

private

   package Parent_Class is

      type Object is abstract tagged record
         Component_1 : Integer := 2;
      end record;

      procedure Method_1
        (This : in out Object);

   end Parent_Class;

   package Child_Class is

      type Object is new Parent_Class.Object with record
         Component_2 : Integer := 10;
      end record;

      overriding
      procedure Method_1
        (This : in out Object);

      function Method_2
        (This    : in out Object;
         Param_1 : in Integer)
         return Integer;

   end Child_Class;

end Unit_2;
