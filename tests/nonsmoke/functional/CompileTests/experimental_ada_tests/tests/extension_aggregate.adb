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

   Parent_Var : Parent := (C1 => 1.0, C2 => 2.0);
   Exten_Var  : Extension;
begin
   -- Simple aggregate
   -- (See ACES V2.0, test "a9_ob_simp_aggregate_02")
   Exten_Var := (C1 => 1.0, C2 => 2.0,
                 C3 => 3.0, C4 => 4.0);
   -- Extension aggregate
   -- (See ACES V2.0, test "a9_ob_ext_aggregate_02")
   Exten_Var := (Parent_Var with C3 => 3.3, C4 => 4.4);
  null;
end;
