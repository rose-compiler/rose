package Discriminant_Constraint is
   type Unconstrained_Type (Size : Natural) is null record;
   Constrained_Object : Unconstrained_Type (10);
end Discriminant_Constraint;
