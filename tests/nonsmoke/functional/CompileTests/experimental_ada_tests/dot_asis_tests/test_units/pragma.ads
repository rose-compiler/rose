package Unit1 is
   function Add (A, B : Integer) return Integer;
   pragma Export (C, Add, "add");
end Unit1;
