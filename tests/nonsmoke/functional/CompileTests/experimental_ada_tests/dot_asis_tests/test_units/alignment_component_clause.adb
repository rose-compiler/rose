-- This test shows the usage of various alinment and size clauses.
-- It was taken from an online example, but heavily modified.

with System;

procedure Alignment_Component_Clause is
   Word : constant := 4;  --  storage element is byte, 4 bytes per word
   type State         is (A,M,W,P);
   type Mode          is (Fix, Dec, Exp, Signif);
   type Byte_Mask     is array (0..7)  of Boolean;
   type State_Mask    is array (State) of Boolean;
   type Mode_Mask     is array (Mode)  of Boolean;
   type Program_Status_Word is
      record
         System_Mask        : Integer range 0 .. 4;
         Protection_Key     : Integer range 0 .. 3;
         Machine_State      : Integer range 0 .. 3;
         Interrupt_Cause    : Integer range 0 .. 65535;
         Ilc                : Integer range 0 .. 3;
         Cc                 : Integer range 0 .. 3;
         Program_Mask       : Integer range 0 .. 3;
         Inst_Address       : Integer range 0 .. 65535;
      end record;
   for Program_Status_Word use
      record
         System_Mask      at 0*Word range 0  .. 7;
         Protection_Key   at 0*Word range 10 .. 11; -- bits 8,9 unused
         Machine_State    at 0*Word range 12 .. 15;
         Interrupt_Cause  at 0*Word range 16 .. 31;
         Ilc              at 1*Word range 0  .. 1;  -- second word
         Cc               at 1*Word range 2  .. 3;
         Program_Mask     at 1*Word range 4  .. 7;
         Inst_Address     at 1*Word range 8  .. 31;
      end record;
      for Program_Status_Word'Size use 24*System.Storage_Unit;
      for Program_Status_Word'Alignment use 8;
begin
  null;

end Alignment_Component_Clause;
