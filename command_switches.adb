with Ada.Calendar;
with Ada.Exceptions;
with Ada.Strings.Unbounded;
with Ada.Text_Io;


with List_Generic;
with Strings_Match_Well_Enough;

package body Command_Switches is

    package Asu renames Ada.Strings.Unbounded;
    package Aex renames Ada.Exceptions;

    package Parameter_Lists is new List_Generic (Asu.Unbounded_String);

    type Switch_Data_Record is
        record
            Switch : Asu.Unbounded_String;
            Parameters : Parameter_Lists.List;
        end record;

    package Switch_Lists is new List_Generic (Switch_Data_Record);

    The_Switches : Switch_Lists.List;

    package Os_Access is
        function Argument_Count return Natural;
        -- raises Internal_Error

        function Argument (Number : in Positive) return String;
        -- raises Internal_Error

        function Command_Name return String;
        -- raises no exceptions

        function Command_Date return Ada.Calendar.Time;
        -- raises Internal_Error

        Internal_Error : exception;
    end Os_Access;

    ------------
    -- EXPORTED:
    ------------
    function Command_Name return String is
    begin
        return Os_Access.Command_Name;
    end Command_Name;

    ------------
    -- EXPORTED:
    -- Works for POSIX names only.
    ------------
    function Simple_Command_Name return String is
        Full_Name : String := Command_Name;
        Simple_Pos : Positive := Full_Name'First;
    begin
        for Index in reverse Full_Name'Range loop
            if Full_Name (Index) = '/' then
                Simple_Pos := Index + 1;
                exit;
            end if;
        end loop;
        return Full_Name (Simple_Pos .. Full_Name'Last);
    end Simple_Command_Name;

    ------------
    -- EXPORTED:
    ------------
    function Command_Date return Ada.Calendar.Time is
    begin
        return Os_Access.Command_Date;
    end Command_Date;


    ----------------------------------------------------------------------
    procedure Look_For_Switch
                 (Switch : in String;
                  Switch_Data : out Switch_Data_Record;
                  Found : out Boolean) is
        use Switch_Lists;
        Current_Switch : Switch_Data_Record;
        Place : Listiter := Makelistiter (The_Switches);
    begin
        -- Explicitly initialize out params
        Switch_Data := (Switch => Asu.Null_Unbounded_String,
                        Parameters => Parameter_Lists.Create);
        Found := False;

        -- Loop through the list and check each one until we find it:
        while More (Place) loop
            Next (Place, Current_Switch);
            if Strings_Match_Well_Enough
                  (Asu.To_String (Current_Switch.Switch), Switch) then
                Switch_Data := Current_Switch;
                Found := True;
                exit;
            end if;
        end loop;
    end Look_For_Switch;

    ----------------------------------------------------------------------
    function Get_Switch (Switch : in String) return Switch_Data_Record is
        Switch_Data : Switch_Data_Record;
        Found : Boolean;
    begin
        Look_For_Switch (Switch, Switch_Data, Found);
        if not Found then
            Aex.Raise_Exception (Error'Identity,
                                 "Couldn't find switch """ & Switch & """");
        end if;
        -- No exception:
        return Switch_Data;
    end Get_Switch;

    ------------
    -- EXPORTED:
    ------------
    function Is_Present (Switch : in String) return Boolean is
        Switch_Data : Switch_Data_Record;
        Found : Boolean;
    begin
        Look_For_Switch (Switch, Switch_Data, Found);
        return Found;
    end Is_Present;

    ------------
    -- EXPORTED:
    ------------
    procedure Check_Is_Present (Switch : in String) is
    begin
        if not Is_Present (Switch) then
            Aex.Raise_Exception (Error'Identity,
                                 "Couldn't find switch """ & Switch & """");
        end if;
    end Check_Is_Present;

    ------------
    -- EXPORTED:
    ------------
    function Is_Present_One_Parm (Switch : in String) return Boolean is
    begin
        return Is_Present (Switch) and then Parameter_Count (Switch) = 1;
    end Is_Present_One_Parm;

    ------------
    -- EXPORTED:
    ------------
    procedure Check_Is_Present_One_Parm (Switch : in String) is
    begin
        -- Be sure there is a switch first:
        Check_Is_Present (Switch);
        if not Is_Present_One_Parm (Switch) then
            Aex.Raise_Exception
               (Error'Identity,
                "The """ & Switch &
                   """ switch does not have exactly one parameter.");
        end if;
    end Check_Is_Present_One_Parm;

    ------------
    -- EXPORTED:
    ------------
    function Get_One_Parm (Switch : in String) return String is
    begin
        Check_Is_Present_One_Parm (Switch);
        return Parameter_Of (Switch, 1);
    end Get_One_Parm;

    ------------
    -- EXPORTED:
    ------------
    function Parameter_Count (Switch : in String) return Natural is
        This_Switch : Switch_Data_Record;
    begin
        This_Switch := Get_Switch (Switch);
        return Parameter_Lists.Length (This_Switch.Parameters);
    end Parameter_Count;

    ------------
    -- EXPORTED:
    ------------
    function Parameter_Of (Switch : in String; Position : in Positive := 1)
                          return String is
        This_Switch : Switch_Data_Record;
        Parameter_Count : Natural;
        use Parameter_Lists;
        Current_Parameter : Asu.Unbounded_String;
        Place : Listiter;
    begin
        This_Switch := Get_Switch (Switch);
        Parameter_Count := Parameter_Lists.Length (This_Switch.Parameters);

        if Position > Parameter_Count then
            Aex.Raise_Exception (Error'Identity,
                                 "Couldn't find parameter#" &
                                    Positive'Image (Position) &
                                    " in parameters 1.." &
                                    Natural'Image (Parameter_Count) &
                                    " of switch """ & Switch & """");
        else
            -- Step to the parameter:
            Place := Makelistiter (This_Switch.Parameters);
            for Index in 1 .. Position loop
                Next (Place, Current_Parameter);
            end loop;
        end if;

        return Asu.To_String (Current_Parameter);
    end Parameter_Of;


    ------------
    -- EXPORTED:
    ------------
    procedure Parameter_Count_Should_Be (Switch : in String; Count : Natural) is
    begin
        if not (Parameter_Count (Switch) = Count) then
            Aex.Raise_Exception (Error'Identity,
                                 "Parameter count of """ &
                                    Switch & """ should be " &
                                    Natural'Image (Count) & ", not " &
                                    Natural'Image (Parameter_Count (Switch)));
        end if;
    end Parameter_Count_Should_Be;

    ------------
    -- EXPORTED:
    ------------
    -- Returns true if the parameter is found in the switch:
    function Has_Parameter
                (Switch : in String; Parameter : in String) return Boolean is
        Parm_Count : Natural := Parameter_Count (Switch);
    begin
        for Index in 1 .. Parm_Count loop
            if Strings_Match_Well_Enough
                  (Parameter_Of (Switch, Index), Parameter) then
                return True;
            end if;
        end loop;

        return False;

    end Has_Parameter;


    function Is_A_Switch (It : in Asu.Unbounded_String) return Boolean is
    begin
        return (Asu.Element (It, 1) = '-');
    end Is_A_Switch;

    ----------------------------------------------------------------------
    procedure Initialize is
        This_Token : Asu.Unbounded_String;
        This_Switch : Switch_Data_Record;
        use type Asu.Unbounded_String;
        ----------------------------------------------------------------------
    begin
        -- At elaboration time, parse all the tokens.
        -- Any parameters that precede the first switch will be associated
        -- with the No_Switch switch.
        -- NOTE:  Duplicate switches and their parameters will be ignored.

        The_Switches := Switch_Lists.Create;

        -- Start a new switch for any parameters without a switch:
        This_Switch.Switch := Asu.To_Unbounded_String (No_Switch);
        This_Switch.Parameters := Parameter_Lists.Create;

        for Index in 1 .. Os_Access.Argument_Count loop
            This_Token := Asu.To_Unbounded_String (Os_Access.Argument (Index));

            -- Don't do anything with a null token:
            if This_Token /= "" then
                if Is_A_Switch (This_Token) then
                    -- Store the previous switch
                    Switch_Lists.Attach (The_Switches, This_Switch);

                    -- Start a new switch:
                    -- (trim the leading '-'):
                    This_Switch.Switch := Asu.Delete (This_Token, 1, 1);
                    This_Switch.Parameters := Parameter_Lists.Create;
                else
                    -- Add a parameter to the current switch:
                    Parameter_Lists.Attach (This_Switch.Parameters, This_Token);
                end if;
            end if;

        end loop;

        -- End the last switch:
        Switch_Lists.Attach (The_Switches, This_Switch);

    end Initialize;

    package Natural_Io is new Ada.Text_Io.Integer_Io (Natural);

    ------------
    -- EXPORTED:
    ------------
    -- Interactively obtains switches from the user
    procedure Prompt_For_Switches is

        Argument_Count : Natural;
        Token_Str : String (1 .. 64);
        Token_Last : Natural;

        This_Token : Asu.Unbounded_String;
        This_Switch : Switch_Data_Record;

        use Ada.Text_Io;
        use type Asu.Unbounded_String;
    begin
        -- Start a new switch for any parameters without a switch:
        This_Switch.Switch := Asu.To_Unbounded_String (No_Switch);
        This_Switch.Parameters := Parameter_Lists.Create;

        -- Get argc
        Put_Line ("Enter the number of arguments ");
        Natural_Io.Get (Argument_Count);

        for Index in 1 .. Argument_Count loop
            Put_Line ("Enter token(" & Integer'Image (Index) & ") ");

            --Kludge - flush out the buffered new_line
            if Index = 1 then
                Get_Line (Token_Str, Token_Last);
            end if;

            Get_Line (Token_Str, Token_Last);
            This_Token := Asu.To_Unbounded_String (Token_Str (1 .. Token_Last));

            -- Don't do anything with a null token:
            if This_Token /= "" then
                if Is_A_Switch (This_Token) then
                    -- Store the previous switch
                    Switch_Lists.Attach (The_Switches, This_Switch);

                    -- Start a new switch:
                    -- (trim the leading '-'):
                    This_Switch.Switch := Asu.Delete (This_Token, 1, 1);
                    This_Switch.Parameters := Parameter_Lists.Create;
                else
                    -- Add a parameter to the current switch:
                    Parameter_Lists.Attach (This_Switch.Parameters, This_Token);
                end if;
            end if;
        end loop;

        -- End the last switch:
        Switch_Lists.Attach (The_Switches, This_Switch);
    end Prompt_For_Switches;

    -- This package body is separate to minimize target-dependent code
    -- (different histories):
    package body Os_Access is separate;
begin

    Initialize;

end Command_Switches;
