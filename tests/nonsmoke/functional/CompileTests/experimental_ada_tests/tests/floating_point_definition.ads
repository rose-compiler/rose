package Floating_Point_Definition is
   type Floating_Type_1 is digits 5;
   type Floating_Type_2 is digits 6 range 0.0 .. 1.0;
   type Floating_Type_3 is new Float digits 7 range -3.4E38 .. +3.4E38; -- JACCEL-267
   type Floating_Type_4 is new Float range -1.0E10 .. +1.0E10; 
   type Floating_Type_5 is new Float digits 8;
end Floating_Point_Definition;
