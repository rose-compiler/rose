with Command_Switches;

with Ada.Exceptions;
with Ada.Text_Io;
use Ada.Text_Io;

procedure Test_Command_Switches is
begin

    Put_Line ("BEGIN Test_Command_Switches");


    Put_Line ("----------------------------------------");
    Put_Line ("Command name is: """ & Command_Switches.Command_Name & """");
    Put_Line ("Simple command name is: """ &
              Command_Switches.Simple_Command_Name & """");


    Put_Line ("----------------------------------------");
    Put_Line ("Count of parameters with no switch:" &
              Integer'Image (Command_Switches.Parameter_Count
                                (Command_Switches.No_Switch)));
    for Index in 1 .. Command_Switches.Parameter_Count
                         (Command_Switches.No_Switch) loop
        Put_Line
           ("#" & Integer'Image (Index) & ":""" &
            Command_Switches.Parameter_Of (Command_Switches.No_Switch, Index) &
            """");
    end loop;


    Put_Line ("----------------------------------------");
    Put_Line ("Checking for switch ""a"" using Is_Present");
    if Command_Switches.Is_Present ("a") then
        Put_Line ("Found");
    else
        Put_Line ("Not found");
    end if;


    Put_Line ("----------------------------------------");
    Put_Line ("Checking for switch ""a"" using Check_Is_Present");
    Put_Line ("An exception should be raised if the switch is not present");
    Command_Switches.Check_Is_Present ("a");


    Put_Line ("----------------------------------------");
    Put_Line ("Checking for parameter ""foo"" in switch ""a""");
    if Command_Switches.Has_Parameter ("a", "foo") then
        Put_Line ("Found");
    else
        Put_Line ("Not found");
    end if;


    Put_Line ("----------------------------------------");
    Put_Line
       ("Checking that switch ""a"" has exactly one parameter using Is_Present_One_Parm.");
    if Command_Switches.Is_Present_One_Parm ("a") then
        Put_Line ("""a"" is present, has one parm");
    else
        Put_Line ("""a"" is not present, or does not have exactly one parm");
    end if;


    Put_Line ("----------------------------------------");
    Put_Line
       ("Checking that switch ""a"" has exactly one parameter using Check_Is_Present_One_Parm.");
    Put_Line
       ("An exception should be raised if the switch does not have exactly one parm.");
    Command_Switches.Check_Is_Present_One_Parm ("a");


    Put_Line ("----------------------------------------");
    Put_Line ("Parameter count for switch ""b"":");
    Put_Line ("(An exception should be raised if there is no switch ""b"".)");
    Put_Line (Integer'Image (Command_Switches.Parameter_Count ("b")));
    Put_Line ("(No exception raised.)");


    Put_Line ("----------------------------------------");
    Put_Line ("Parameters for switch ""b"" are:");
    for Index in 1 .. Command_Switches.Parameter_Count ("b") loop
        Put_Line ("#" & Integer'Image (Index) & ":""" &
                  Command_Switches.Parameter_Of ("b", Index) & """");
    end loop;


    Put_Line ("----------------------------------------");
    Put_Line ("Getting the second parameter of ""b"" with Parameter_Of.");
    Put_Line
       ("If there are not AT LEAST 2 parameters for ""b"", an exception should be raised below.");
    Put_Line ("#" & Integer'Image (2) & ":""" &
              Command_Switches.Parameter_Of ("b", 2) & """");
    Put_Line ("(No exception raised.)");


    Put_Line ("----------------------------------------");
    Put_Line
       ("Checking the parameter count of ""b"" with Parameter_Count_Should_Be.");
    Put_Line
       ("If there are not EXACTLY 2 parameters for ""b"", an exception should be raised below.");
    Command_Switches.Parameter_Count_Should_Be ("b", 2);
    Put_Line ("(No exception raised.)");


    Put_Line ("----------------------------------------");
    Put_Line ("END Test_Command_Switches");

exception

    when Occurrence: others =>
        Put_Line ("EXCEPTION " & Ada.Exceptions.Exception_Name (Occurrence) &
                  " " & Ada.Exceptions.Exception_Message (Occurrence));
        raise;

end Test_Command_Switches;
