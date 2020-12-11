procedure Extension_Aggregate is
  type Parent is tagged
      record
         C1 : Float;
         C2 : Float;
      end record;

   type Extension is new Parent with
      record
         C3 : Float;
         C4 : Float;
      end record;

   type Parent_Ptr is access Parent;
   type Extension_Ptr is access Extension;

   ParentA : Parent_Ptr;
   ExtenA  : Extension_Ptr;
begin
   ParentA := new Parent;
   ExtenA  := new Extension'(C1 => 1.0, C2 => 2.0, C3 => 3.0, C4 => 4.0);
  null;
end;
