with Asis.Elements;
with Asis.Statements;
with Ada.Text_IO;

package body Asis_Tool_2.Element.Statements is

   ------------
   -- EXPORTED:
   ------------
   procedure Do_Pre_Child_Processing
     (Element : in Asis.Element; State : in out Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Do_Pre_Child_Processing";
      Result : a_nodes_h.Statement_Struct :=
        a_nodes_h.Support.Default_Statement_Struct;

      Statement_Kind : constant Asis.Statement_Kinds :=
        Asis.Elements.Statement_Kind (Element);

      -- Supporting procedures are in alphabetical order:
      procedure Add_Aborted_Tasks is
      begin
         Add_Element_List
           (This           => State,
            Elements_In    => Asis.Statements.Aborted_Tasks (Element),
            Dot_Label_Name => "Aborted_Tasks",
            List_Out       => Result.Aborted_Tasks,
            Add_Edges      => True);
      end;


      procedure Add_Accept_Body_Exception_Handlers is
      begin
         Add_Element_List
           (This           => State,
            Elements_In    => Asis.Statements.Accept_Body_Exception_Handlers (Element),
            Dot_Label_Name => "Accept_Body_Exception_Handlers",
            List_Out       => Result.Accept_Body_Exception_Handlers,
            Add_Edges      => True);
      end;

      procedure Add_Accept_Body_Statements is
      begin
         Add_Element_List
           (This           => State,
            Elements_In    => Asis.Statements.Accept_Body_Statements (Element),
            Dot_Label_Name => "Accept_Body_Statements",
            List_Out       => Result.Accept_Body_Statements,
            Add_Edges      => True);
      end;

      procedure Add_Accept_Entry_Direct_Name is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Statements.Accept_Entry_Direct_Name (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Accept_Entry_Direct_Name", ID);
         Result.Accept_Entry_Direct_Name := ID;
      end;

      procedure Add_Accept_Entry_Index is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Statements.Accept_Entry_Index (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Accept_Entry_Index", ID);
         Result.Accept_Entry_Index := ID;
      end;

      procedure Add_Accept_Parameters is
      begin
         Add_Element_List
           (This           => State,
            Elements_In    => Asis.Statements.Accept_Parameters (Element),
            Dot_Label_Name => "Accept_Parameters",
            List_Out       => Result.Accept_Parameters,
            Add_Edges      => True);
      end;

      procedure Add_Assignment_Expression is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Statements.Assignment_Expression (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Assignment_Expression", ID);
         Result.Assignment_Expression := ID;
      end;

      procedure Add_Assignment_Variable_Name is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Statements.Assignment_Variable_Name (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Assignment_Variable_Name", ID);
         Result.Assignment_Variable_Name := ID;
      end;

      procedure Add_Associated_Message is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Statements.Associated_Message (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Associated_Message", ID);
         Result.Associated_Message := ID;
      end;

      procedure Add_Block_Declarative_Items is
      begin
         Add_Element_List
           (This           => State,
            Elements_In    => Asis.Statements.Block_Declarative_Items (Element),
            Dot_Label_Name => "Block_Declarative_Items",
            List_Out       => Result.Block_Declarative_Items,
            Add_Edges      => True);
      end;

      procedure Add_Block_Exception_Handlers is begin
         Add_Element_List
           (This           => State,
            Elements_In    => Asis.Statements.Block_Exception_Handlers (Element),
            Dot_Label_Name => "Block_Exception_Handlers",
            List_Out       => Result.Block_Exception_Handlers,
            Add_Edges      => True);
      end;

      procedure Add_Block_Statements is
      begin
         Add_Element_List
           (This           => State,
            Elements_In    => Asis.Statements.Block_Statements (Element),
            Dot_Label_Name => "Block_Statements",
            List_Out       => Result.Block_Statements,
            Add_Edges      => True);
      end;


      procedure Add_Call_Statement_Parameters is
      begin
         Add_Element_List
           (This           => State,
            Elements_In    => Asis.Statements.Call_Statement_Parameters (Element),
            Dot_Label_Name => "Call_Statement_Parameters",
            List_Out       => Result.Call_Statement_Parameters,
            Add_Edges      => True);
      end;

      procedure Add_Called_Name is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Statements.Called_Name (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Called_Name", ID);
         Result.Called_Name := ID;
      end;

      procedure Add_Case_Expression is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Statements.Case_Expression (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Case_Expression", ID);
         Result.Case_Expression := ID;
      end;

      procedure Add_Corresponding_Called_Entity is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Statements.Corresponding_Called_Entity (Element));
      begin
         State.Add_To_Dot_Label ("Corresponding_Called_Entity", To_String (ID));
         Result.Corresponding_Called_Entity := ID;
      end;


      procedure Add_Corresponding_Destination_Statement is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Statements.Corresponding_Destination_Statement(Element));
      begin
         State.Add_To_Dot_Label ("Corresponding_Destination_Statement", To_String (ID));
         Result.Corresponding_Destination_Statement := ID;
      end;

      procedure Add_Corresponding_Entry is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Statements.Corresponding_Entry (Element));
      begin
         State.Add_To_Dot_Label ("Corresponding_Entry", To_String (ID));
         Result.Corresponding_Entry := ID;
      end;

      procedure Add_Corresponding_Loop_Exited is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Statements.Corresponding_Loop_Exited (Element));
      begin
         State.Add_To_Dot_Label ("Corresponding_Loop_Exited", To_String (ID));
         Result.Corresponding_Loop_Exited := ID;
      end;

      procedure Add_Delay_Expression is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Statements.Delay_Expression (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Delay_Expression", ID);
         Result.Delay_Expression := ID;
      end;

      procedure Add_Exit_Condition is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Statements.Exit_Condition (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Exit_Condition", ID);
         Result.Exit_Condition := ID;
      end;

      procedure Add_Exit_Loop_Name is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Statements.Exit_Loop_Name (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Exit_Loop_Name", ID);
         Result.Exit_Loop_Name := ID;
      end;

      procedure Add_For_Loop_Parameter_Specification is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Statements.For_Loop_Parameter_Specification (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("For_Loop_Parameter_Specification", ID);
         Result.For_Loop_Parameter_Specification := ID;
      end;

      procedure Add_Goto_Label is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Statements.Goto_Label (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Goto_Label", ID);
         Result.Goto_Label := ID;
      end;

      procedure Add_Is_Declare_Block is
         Value : constant Boolean := Asis.Statements.Is_Declare_Block (Element);
      begin
         State.Add_To_Dot_Label ("Is_Declare_Block", Value);
         Result.Is_Declare_Block := a_nodes_h.Support.To_bool (Value);
      end;

      procedure Add_Is_Dispatching_Call is
         Value : constant Boolean := Asis.Statements.Is_Dispatching_Call (Element);
      begin
         State.Add_To_Dot_Label ("Is_Dispatching_Call", Value);
         Result.Is_Dispatching_Call := a_nodes_h.Support.To_bool (Value);
      end;

      procedure Add_Is_Call_On_Dispatching_Operation is
         Value : constant Boolean := Asis.Statements.Is_Call_On_Dispatching_Operation (Element);
      begin
         State.Add_To_Dot_Label ("Is_Call_On_Dispatching_Operation", Value);
         Result.Is_Call_On_Dispatching_Operation := a_nodes_h.Support.To_bool (Value);
      end;

      --In a declaration this says if the block name is repeated on the
      --end line or not.  e.g. end MyBlock;
      procedure Add_Is_Name_Repeated is
         Value : constant Boolean := Asis.Statements.Is_Name_Repeated (Element);
      begin
         State.Add_To_Dot_Label ("Is_Name_Repeated", Value);
         Result.Is_Name_Repeated := a_nodes_h.Support.To_bool (Value);
      end;

      procedure Add_Label_Names is
      begin
         Add_Element_List
           (This           => State,
            Elements_In    => Asis.Statements.Label_Names (Element),
            Dot_Label_Name => "Label_Names",
            List_Out       => Result.Label_Names,
            Add_Edges      => True);
      end;

      procedure Add_Loop_Statements is
      begin
         Add_Element_List
           (This           => State,
            Elements_In    => Asis.Statements.Loop_Statements (Element),
            Dot_Label_Name => "Loop_Statements",
            List_Out       => Result.Loop_Statements,
            Add_Edges      => True);
      end;

      procedure Add_Qualified_Expression is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Statements.Qualified_Expression (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Qualified_Expression", ID);
         Result.Qualified_Expression := ID;
      end;

      procedure Add_Raised_Exception is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Statements.Raised_Exception (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Raised_Exception", ID);
         Result.Raised_Exception := ID;
      end;

      procedure Add_Requeue_Entry_Name is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Statements.Requeue_Entry_Name (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Requeue_Entry_Name", ID);
         Result.Requeue_Entry_Name := ID;
      end;

      procedure Add_Return_Expression is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Statements.Return_Expression (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Return_Expression", ID);
         Result.Return_Expression := ID;
      end;

      procedure Add_Statement_Identifier is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Statements.Statement_Identifier (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Statement_Identifier", ID);
         Result.Statement_Identifier := ID;
      end;

      procedure Add_Statement_Paths is
      begin
         Add_Element_List
           (This           => State,
            Elements_In    => Asis.Statements.Statement_Paths (Element),
            Dot_Label_Name => "Statement_Paths",
            List_Out       => Result.Statement_Paths,
            Add_Edges      => True);
      end;

      procedure Add_While_Condition is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Statements.While_Condition (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("While_Condition", ID);
         Result.While_Condition := ID;
      end;

      procedure Add_Common_Items is
      begin
         State.Add_To_Dot_Label ("Statement_Kind", Statement_Kind'Image);
         Result.Statement_Kind := anhS.To_Statement_Kinds (Statement_Kind);
         Add_Label_Names;
      end Add_Common_Items;

      procedure Add_Common_Loop_Items is
      begin
         Add_Statement_Identifier;
         Add_Is_Name_Repeated;
         Add_Loop_Statements;
      end Add_Common_Loop_Items;


      use all type Asis.Statement_Kinds;
   begin
      If Statement_Kind /= Not_A_Statement then
         Add_Common_Items;
      end if;

      case Statement_Kind is
         when Not_A_Statement =>
            raise Program_Error with
            Module_Name & " called with: " & Statement_Kind'Image;

         when A_Null_Statement =>
            null; -- No more info.

         when An_Assignment_Statement =>
            Add_Assignment_Variable_Name;
            Add_Assignment_Expression;

         when An_If_Statement =>
            Add_Statement_Paths;

         when A_Case_Statement =>
            Add_Case_Expression;
            Add_Statement_Paths;

         when A_Loop_Statement =>
            Add_Common_Loop_Items;

         when A_While_Loop_Statement =>
            Add_While_Condition;
            Add_Common_Loop_Items;

         when A_For_Loop_Statement =>
            Add_For_Loop_Parameter_Specification;
            Add_Common_Loop_Items;

         when A_Block_Statement =>
            Add_Statement_Identifier;
            Add_Is_Name_Repeated;
            Add_Is_Declare_Block;
            Add_Block_Declarative_Items;
            Add_Block_Statements;
            Add_Block_Exception_Handlers;

         when An_Exit_Statement =>
            Add_Exit_Condition;
            Add_Exit_Loop_Name;
            Add_Corresponding_Loop_Exited;

         when A_Goto_Statement =>
            Add_Goto_Label;
            Add_Corresponding_Destination_Statement;

         when A_Procedure_Call_Statement =>
            Add_Called_Name;
            Add_Corresponding_Called_Entity;
            Add_Call_Statement_Parameters;
            Add_Is_Dispatching_Call;
            Add_Is_Call_On_Dispatching_Operation;

         when A_Return_Statement =>
            Add_Return_Expression;

         when An_Extended_Return_Statement => -- A2005
            State.Add_Not_Implemented (Ada_2005);

         when An_Accept_Statement =>
            --I think this works, but it cannot be tested until task declaration work
            Add_Accept_Entry_Index;
            Add_Accept_Entry_Direct_Name;
            Add_Accept_Parameters;
            Add_Accept_Body_Statements;
            Add_Accept_Body_Exception_Handlers;
            Add_Corresponding_Entry;

         when An_Entry_Call_Statement =>
            --I think this works, but it cannot be tested until task declaration work
            Add_Called_Name;
            Add_Corresponding_Called_Entity;
            Add_Call_Statement_Parameters;

         when A_Requeue_Statement =>
            --I think this works, but it cannot be tested until attributes work
            --Ada.Text_IO.Put_Line("----  Requeue----");
            Add_Requeue_Entry_Name;

         when A_Requeue_Statement_With_Abort =>
            --I think this works, but it cannot be tested until attributes work
            --Ada.Text_IO.Put_Line("----  Requeue----");
            Add_Requeue_Entry_Name;

         when A_Delay_Until_Statement =>
            Add_Delay_Expression;

         when A_Delay_Relative_Statement =>
            Add_Delay_Expression;

         when A_Terminate_Alternative_Statement =>
            null; -- No more info

         when A_Selective_Accept_Statement =>
            --I think this works, but it cannot be tested until task declaration work
            --Ada.Text_IO.Put_Line("----  Selective Accept----");
            Add_Statement_Paths;

         when A_Timed_Entry_Call_Statement =>
            --I think this works, but it cannot be tested until task declaration work
            --Ada.Text_IO.Put_Line("----  Timed Entry Call----");
            Add_Statement_Paths;

         when A_Conditional_Entry_Call_Statement =>
            --I think this works, but it cannot be tested until task declaration work
            --Ada.Text_IO.Put_Line("----  Conditional Entry Call----");
            Add_Statement_Paths;

         when An_Asynchronous_Select_Statement =>
            --I think this works, but it cannot be tested until task declaration work
            --Ada.Text_IO.Put_Line("----  Asynchronous Select----");
            Add_Statement_Paths;

         when An_Abort_Statement =>
            --I think this works, but it cannot be tested until task declaration work
            Add_Aborted_Tasks;

         when A_Raise_Statement =>
            Add_Raised_Exception;
            Add_Associated_Message;

         when A_Code_Statement =>
            -- TODO: Untested.  I can't figure out how to get this statement
            -- The standard example is unintelligble, and everywhere else says
            -- "Don't do this, use gcc style asm instead."
            Add_Qualified_Expression;

      end case;

      State.A_Element.Element_Kind := a_nodes_h.A_Statement;
      State.A_Element.the_union.statement := Result;
   end Do_Pre_Child_Processing;

end Asis_Tool_2.Element.Statements;
