-- ----------------------------------------------------------------------
-- This is the GNAT variant of Command_Switches.Os_Access
-- ----------------------------------------------------------------------

-- Note: This package is used in the elaboration of Debug_Support, so it
-- cannot with Debug_Support or Debug_Utility.

with Ada.Calendar;
with Ada.Command_Line;
with Gnat.Os_Lib;
with Io_Conventions;

separate (Command_Switches)
package body Os_Access is

    Package_Name : constant String := "Command_Switches.Os_Access";

    -- The following internal procedures are copied from Debug_Support:

    function Format_Message (Exception_Name : in String;
                             Message        : in String;
                             Module_Name    : in String) return String is
        Lf : constant String := Io_Conventions.Line_Terminator;
    begin
        return Exception_Name &
                  " -- Caught in: " & Module_Name &
                  " -- Message: " & Lf & Message;
    end Format_Message;


    function Get_Message (Occurrence : in Ada.Exceptions.Exception_Occurrence)
                         return String is
        The_Message : constant String :=
           Ada.Exceptions.Exception_Message (Occurrence);
    begin
        if The_Message = "" then
            return "(no message)";
        else
            return The_Message;
        end if;
    end Get_Message;


    function To_String (X           : in Ada.Exceptions.Exception_Occurrence;
                        Module_Name : in String) return String is
    begin
        return Format_Message (Exception_Name =>
                                  Ada.Exceptions.Exception_Name (X),
                               Module_Name    => Module_Name,
                               Message        => Get_Message (X));
    end To_String;


    procedure Raise_Internal_Error (X           : in
                                       Ada.Exceptions.Exception_Occurrence;
                                    Module_Name : in String) is
    begin
        Ada.Exceptions.Raise_Exception (E       => Internal_Error'Identity,
                                         Message => To_String (X, Module_Name));
    end Raise_Internal_Error;


    ------------
    -- EXPORTED:
    ------------
    function Argument_Count return Natural is
    begin
        return Ada.Command_Line.Argument_Count;
    end Argument_Count;


    ------------
    -- EXPORTED:
    ------------
    function Argument (Number : in Positive) return String is
    begin
        return Ada.Command_Line.Argument (Number);
    end Argument;


    ------------
    -- EXPORTED:
    ------------
    function Command_Name return String is
    begin
        return Ada.Command_Line.Command_Name;
    end Command_Name;


    ------------
    -- EXPORTED:
    ------------
    function Command_Date return Ada.Calendar.Time is
        Module_Name : constant String := Package_Name & ".Command_Date";
        Gnat_Time : Gnat.Os_Lib.Os_Time;
        Result : Ada.Calendar.Time;
    begin
        Gnat_Time := Gnat.Os_Lib.File_Time_Stamp (Command_Name);
        Result := Ada.Calendar.Time_Of
                     (Year => Gnat.Os_Lib.Gm_Year (Gnat_Time),
                      Month => Gnat.Os_Lib.Gm_Month (Gnat_Time),
                      Day => Gnat.Os_Lib.Gm_Day (Gnat_Time),
                      Seconds => Ada.Calendar.Day_Duration
                                    (Gnat.Os_Lib.Gm_Hour (Gnat_Time) * 3600) +
                                 Ada.Calendar.Day_Duration
                                    (Gnat.Os_Lib.Gm_Minute (Gnat_Time) * 60) +
                                 Ada.Calendar.Day_Duration
                                    (Gnat.Os_Lib.Gm_Second (Gnat_Time)));
        return Result;
    exception
        when X: others =>
         Raise_Internal_Error (X, Module_Name);
         -- to avoid GNAT Warning:
         raise;
    end Command_Date;

end Os_Access;
