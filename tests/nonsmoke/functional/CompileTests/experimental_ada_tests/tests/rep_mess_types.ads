package rep_mess_Types is
   type M_Integer is new Integer;
   for  M_Integer'size use 32;

   type N_Int is private;
private
   type N_Int is new M_Integer;
end rep_mess_Types;
