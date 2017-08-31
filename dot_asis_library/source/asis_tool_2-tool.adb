with Asis.Implementation;

-- GNAT specific:
with Asis.Extensions;

package body Asis_Tool_2.Tool is

   ------------
   -- EXPORTED:
   ------------
   procedure Process
     (This      : in out Class;
      File_Name : in     String;
      GNAT_Home : in     String;
      Debug     : in     Boolean)
   is
      procedure Log (Message : in String) is
      begin
         Put_Line ("Asis_Tool_2.Tool.Process:  " & message);
      end;

      procedure Init_And_Process_Context is
      begin
         Asis.Implementation.Initialize;
         This.Outputs.Text := new Indented_Text.Class;
         This.Outputs.Graph := Dot.Graphs.Create (Is_Digraph => True,
                                                  Is_Strict  => False);
         This.Outputs.A_Nodes := new A_Nodes.Class;
         -- TODO: use File_Name:
         This.My_Context.Process (This.Outputs);
         This.Outputs.Graph.Write_File (File_Name);
         Asis.Implementation.Finalize;
      end Init_And_Process_Context;

      Compile_Succeeded : Boolean := False;
      File_Name_Aliased : aliased String := File_Name;
      -- Need an aliased string for the Asis.Extensions.Compile GCC parm:
      GCC_String        : aliased String := GNAT_Home & "/bin/gcc";
   begin
      Asis_Tool_2.Trace_On := Debug;
      Log ("BEGIN - File_Name => """ & File_Name & """");
      Log ("Calling Asis.Extensions.Compile (...GCC => " & GCC_String & "...):");
      -- Using gprbuild to avoid calling the wrong (non-GNAT) gcc below:
      Asis.Extensions.Compile
        (Source_File  => File_Name_Aliased'Unchecked_Access,
         Args         => (1..0 => null),
         Success      => Compile_Succeeded,
         GCC          => GCC_String'Unchecked_Access,
         Use_GPRBUILD => False,
         Display_Call => True);
      if Compile_Succeeded then
         Log ("Asis.Extensions.Compile succeeded");
         Init_And_Process_Context;
      else
         Log ("*** Asis.Extensions.Compile FAILED. NOT calling Init_And_Process_Context");
      end if;
      Log ("END");
   end Process;

   ------------
   -- EXPORTED:
   ------------
   function Get_Nodes
     (This      : in out Class)
      return a_nodes_h.Node_List_Ptr is
   begin
      return This.Outputs.A_Nodes.Get_Head;
   end Get_Nodes;

end Asis_Tool_2.Tool;
