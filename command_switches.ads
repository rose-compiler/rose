-- This package parses the program's command line for switches (e.g -d) and
-- parameters.  (any token not starting with "-")  It associates each parameter
-- with the preceding switch.
--
-- Clients may query for the presence of a switch
-- and for the parameters that may be associated with a switch.
-- This query is done with the "Strings_Match_Well_Enough" equality test.
--
-- The command line is parsed at package elaboration time.
--
-- Any parameters that precede the first switch will be associated with the
-- No_Switch switch.
--
-- NOTE:  Duplicate switches and their parameters will be ignored.
--

with Ada.Calendar;

package Command_Switches is
    pragma Elaborate_Body;

    No_Switch : constant String;

    function Command_Name return String;

    -- Works for POSIX names only.
    function Simple_Command_Name return String;

    -- Returns the last modifcation date of the command's executable file:
    function Command_Date return Ada.Calendar.Time;

    function Is_Present (Switch : in String) return Boolean;

    procedure Check_Is_Present (Switch : in String);
    -- May raise Error

    function Is_Present_One_Parm (Switch : in String) return Boolean;

    procedure Check_Is_Present_One_Parm (Switch : in String);
    -- May raise Error

    function Get_One_Parm (Switch : in String) return String;

    function Parameter_Count (Switch : in String) return Natural;
    -- May raise Error

    -- Raises the exception if the parameter count does not match
    procedure Parameter_Count_Should_Be (Switch : in String; Count : Natural);
    -- May raise Error

    function Parameter_Of
                (Switch : in String; Position : in Positive := 1) return String;
    -- May raise Error

    -- Returns true if the parameter is found in the switch:
    function Has_Parameter
                (Switch : in String; Parameter : in String) return Boolean;
    -- May raise Error

    Error : exception;

    -- Used to interactively enter "command" switches
    procedure Prompt_For_Switches;

private

    No_Switch : constant String := "No_Switch";

end Command_Switches;
