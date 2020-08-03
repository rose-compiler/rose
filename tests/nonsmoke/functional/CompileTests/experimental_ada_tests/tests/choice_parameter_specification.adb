procedure Choice_Parameter_Specification is
begin
   null;
exception
   when Choice_Parameter : Constraint_Error =>
      null;
end Choice_Parameter_Specification;
