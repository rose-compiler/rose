package Package_Pragma is
   function Add (A, B : Integer) return Integer;
   pragma Export (C, Add, "add");
end Package_Pragma;
