--  This is the test unit to try out the ASIS applications built
--  in the framework of the ASIS tutorial.

with Ada.Text_IO; use Ada.Text_IO;

package body Test_Unit is

   procedure Swap (A, B : in out T) is
     C : T;
   begin
      if A = B then
         return;
      else
         C := A;
         A := B;
         B := C;
      end if;
   end Swap;

   procedure Swap_Integer is new Swap (Integer);

   procedure Use_Swap_Integer is
      X, Y : Integer;
      package Int_IO is new Integer_IO (Integer);
      use Int_IO;
   begin
      X :=  7;
      Y := 13;

      Put ("X = ");
      Put (X);
      New_Line;

      Put ("Y = ");
      Put (Y);
      New_Line;

      Swap_Integer (X, Y);

      New_Line;
      Put ("X = ");
      Put (X);
      New_Line;

      Put ("Y = ");
      Put (Y);
      New_Line;

   end Use_Swap_Integer;


end Test_Unit;
