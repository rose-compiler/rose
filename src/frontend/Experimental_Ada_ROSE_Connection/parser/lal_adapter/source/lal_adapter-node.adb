with Ada.Tags;
with Generic_Logging;
with Langkit_Support.Slocs;
with Langkit_Support.Text;

package body Lal_Adapter.Node is

   package Slocs renames Langkit_Support.Slocs;
   package Text renames Langkit_Support.Text;

   ------------
   -- EXPORTED:
   ------------
   function To_String
     (This : in a_nodes_h.Element_ID)
      return String
   is
     (To_String (This, Element_ID_Kind));

   ----------------------
   -- EXPORTED (private):
   ----------------------

   procedure Add_To_Dot_Label
     (This  : in out Class;
      Value : in     String) is
   begin
      Add_To_Dot_Label (Dot_Label => This.Dot_Label,
                        Outputs   => This.Outputs,
                        Value     => Value);
   end Add_To_Dot_Label;

   procedure Add_To_Dot_Label
     (This  : in out Class;
      Name  : in     String;
      Value : in     String) is
   begin
      Add_To_Dot_Label (Dot_Label => This.Dot_Label,
                        Outputs   => This.Outputs,
                        Name      => Name,
                        Value     => Value);
   end Add_To_Dot_Label;

   procedure Add_To_Dot_Label
     (This  : in out Class;
      Name  : in     String;
      Value : in     a_nodes_h.Element_ID) is
   begin
      This.Add_To_Dot_Label (Name, To_String (Value));
   end Add_To_Dot_Label;

   procedure Add_To_Dot_Label
     (This  : in out Class;
      Name  : in     String;
      Value : in     Boolean) is
   begin
      Add_To_Dot_Label (Dot_Label => This.Dot_Label,
                        Outputs   => This.Outputs,
                        Name      => Name,
                        Value     => Value);
   end Add_To_Dot_Label;

   procedure Add_Dot_Edge
     (This  : in out Class;
      From  : in     a_nodes_h.Element_ID;
      To    : in     a_nodes_h.Element_ID;
      Label : in     String)
   is
   begin
      Add_Dot_Edge (Outputs   => This.Outputs,
                    From      => From,
                    From_Kind => Element_ID_Kind,
                    To        => To,
                    To_Kind   => Element_ID_Kind,
                    Label     => Label);
   end Add_Dot_Edge;

   procedure Add_To_Dot_Label_And_Edge
     (This  : in out Class;
      Label : in     String;
      To    : in     a_nodes_h.Element_ID) is
   begin
      This.Add_To_Dot_Label (Label, To_String (To));
      This.Add_Dot_Edge (From  => This.Element_IDs.First_Element,
                         To    => To,
                         Label => Label);
   end Add_To_Dot_Label_And_Edge;

   procedure Add_Not_Implemented
     (This        : in out Class;
      Ada_Version : in     Ada_Versions := Supported_Ada_Version) is
   begin
      if Ada_Version <= Supported_Ada_Version then
         This.Add_To_Dot_Label
           ("LIBADALANG_PROCESSING", String'("NOT_IMPLEMENTED_COMPLETELY"));
         This.Outputs.A_Nodes.Add_Not_Implemented;
      else
         This.Add_To_Dot_Label
           ("LIBADALANG_PROCESSING",
            Ada_Version'Image & "_FEATURE_NOT_IMPLEMENTED_IN_" &
             Supported_Ada_Version'Image);
      end if;
   end Add_Not_Implemented;

   ------------
   -- Exported:
   ------------
   procedure Process_Ada_Stmt
     (This    : in out Class;
      --  Node    : in     LAL.Stmt'Class;
      Node    : in     LAL.Ada_Node'Class;
      --  Options : in     Options_Record;
      Outputs : in     Output_Accesses_Record)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process_Ada_Stmt";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      --  Auto : Logging.Auto_Logger; -- Logs BEGIN and END

      -- Will raise Constraint_Error if Node.Kind is not in Ada_Stmt:
      Kind             : constant LALCO.Ada_Stmt := Node.Kind;
      use LALCO; -- For subtype names in case stmt
   begin -- Process_Ada_Stmt
      This.Outputs := Outputs;

      case Kind is
         when Ada_Accept_Stmt =>
            This.Add_Not_Implemented;
         when Ada_Accept_Stmt_With_Stmts =>
            This.Add_Not_Implemented;
         when Ada_For_Loop_Stmt =>
            This.Add_Not_Implemented;
         when Ada_Loop_Stmt =>
            This.Add_Not_Implemented;
         when Ada_While_Loop_Stmt =>
            This.Add_Not_Implemented;
         when Ada_Begin_Block =>
            This.Add_Not_Implemented;
         when Ada_Decl_Block =>
            This.Add_Not_Implemented;
         when Ada_Case_Stmt =>
            This.Add_Not_Implemented;
         when Ada_Extended_Return_Stmt =>
            This.Add_Not_Implemented;
         when Ada_If_Stmt =>
            This.Add_Not_Implemented;
         when Ada_Named_Stmt =>
            This.Add_Not_Implemented;
         when Ada_Select_Stmt =>
            This.Add_Not_Implemented;
         when Ada_Error_Stmt =>
            This.Add_Not_Implemented;
         when Ada_Abort_Stmt =>
            This.Add_Not_Implemented;
         when Ada_Assign_Stmt =>
            This.Add_Not_Implemented;
         when Ada_Call_Stmt =>
            This.Add_Not_Implemented;

         when Ada_Delay_Stmt =>
            declare
               Delay_Stmt_Node : constant LAL.Delay_Stmt := LAL.As_Delay_Stmt (Node);
               Has_Until       : constant Boolean := LAL.F_Has_Until (Delay_Stmt_Node);
               Seconds         : constant LAL.Expr := LAL.F_Expr (Delay_Stmt_Node);
            begin
               Log ("F_Has_Until: " & Boolean'Image (Has_Until));
               Log ("Seconds: " & Seconds.Debug_Text);
            end;

            This.Add_Not_Implemented;

         when Ada_Exit_Stmt =>
            This.Add_Not_Implemented;
         when Ada_Goto_Stmt =>
            This.Add_Not_Implemented;
         when Ada_Label =>
            This.Add_Not_Implemented;
         when Ada_Null_Stmt =>
            This.Add_Not_Implemented;
         when Ada_Raise_Stmt =>
            This.Add_Not_Implemented;
         when Ada_Requeue_Stmt =>
            This.Add_Not_Implemented;
         when Ada_Return_Stmt =>
            This.Add_Not_Implemented;
         when Ada_Terminate_Alternative =>
            This.Add_Not_Implemented;
      end case;

   exception
      when X : External_Error | Internal_Error | Usage_Error =>
         raise;
      when X: others =>
         Log_Exception (X);
         Log ("No handler for this exception.  Raising Internal_Error");
         raise Internal_Error;
   end Process_Ada_Stmt;

   ------------
   -- Exported:
   ------------
   procedure Process
     (This    : in out Class;
      Node    : in     LAL.Ada_Node'Class;
      --  Options : in     Options_Record;
      Outputs : in     Output_Accesses_Record)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      --  Auto : Logging.Auto_Logger; -- Logs BEGIN and END

      Sloc_Range_Image : constant string := Slocs.Image (Node.Sloc_Range);
      Kind             : constant LALCO.Ada_Node_Kind_Type := Node.Kind;
      Kind_Image       : constant String := LALCO.Ada_Node_Kind_Type'Image (Kind);
      Kind_Name        : constant String := Node.Kind_Name;
      Debug_Text       : constant String := Node.Debug_Text;
      use LALCO; -- For subtype names in case stmt
      --  use type LALCO.Ada_Node_Kind_Type; -- For "="
   begin -- Process
      This.Outputs := Outputs;
      --  Log ("Line" & Start_Line_Image & ": " & Kind_Image  & ": " & Debug_Text);
      --  if Node.Kind /= LALCO.Ada_Compilation_Unit then
      Log ("Kind enum: " & Kind_Image & "; Kind name: " & Kind_Name & " at " & Sloc_Range_Image);
      --  Log (LAL.Image(Node));
      --  if Kind in LALCO.Ada_Stmt then
      --     Log ("Statement");
      --  else
      --     Log ("NOT a statement");
      --  end if;
      --
      case Kind is
         -- 74 included kinds:
         when Ada_Basic_Decl'First .. Ada_Basic_Decl'Last =>
            This.Add_Not_Implemented;
         -- 60 included kinds:
         when Ada_Expr'First .. Ada_Expr'Last =>
            This.Add_Not_Implemented;
         -- 31 (25?) included kinds:
         when Ada_Stmt'First .. Ada_Stmt'Last =>
            --  Log ("Tag: " & Ada.Tags.Expanded_Name (Node'Tag));
            --  This.Process_Ada_Stmt (LAL.Stmt'Class (Node), Outputs);
            This.Process_Ada_Stmt (Node, Outputs);
         -- 17 included kinds:
         when Ada_Type_Def'First .. Ada_Type_Def'Last =>
            This.Add_Not_Implemented;


         when Ada_Abort_Node'First .. Ada_Abort_Node'Last =>
            This.Add_Not_Implemented;
         when Ada_Abstract_Node'First .. Ada_Abstract_Node'Last =>
            This.Add_Not_Implemented;
         when Ada_Ada_List'First .. Ada_Ada_List'Last =>
            This.Add_Not_Implemented;
         when Ada_Aliased_Node'First .. Ada_Aliased_Node'Last =>
            This.Add_Not_Implemented;
         when Ada_All_Node'First .. Ada_All_Node'Last =>
            This.Add_Not_Implemented;
         when Ada_Array_Indices'First .. Ada_Array_Indices'Last =>
            This.Add_Not_Implemented;
         when Ada_Aspect_Assoc_Range'First .. Ada_Aspect_Assoc_Range'Last =>
            This.Add_Not_Implemented;
         when Ada_Aspect_Clause'First .. Ada_Aspect_Clause'Last =>
            This.Add_Not_Implemented;
         when Ada_Aspect_Spec_Range'First .. Ada_Aspect_Spec_Range'Last =>
            This.Add_Not_Implemented;
         when Ada_Base_Assoc'First .. Ada_Base_Assoc'Last =>
            This.Add_Not_Implemented;
         when Ada_Base_Formal_Param_Holder'First .. Ada_Base_Formal_Param_Holder'Last =>
            This.Add_Not_Implemented;
         when Ada_Base_Record_Def'First .. Ada_Base_Record_Def'Last =>
            This.Add_Not_Implemented;
         when Ada_Basic_Assoc'First .. Ada_Basic_Assoc'Last =>
            This.Add_Not_Implemented;
         --  Moved to top:
         --  when Ada_Basic_Decl'First .. Ada_Basic_Decl'Last =>
         when Ada_Case_Stmt_Alternative_Range'First .. Ada_Case_Stmt_Alternative_Range'Last =>
            This.Add_Not_Implemented;
         when Ada_Compilation_Unit_Range'First .. Ada_Compilation_Unit_Range'Last =>
            This.Add_Not_Implemented;
         when Ada_Component_Clause_Range'First .. Ada_Component_Clause_Range'Last =>
            This.Add_Not_Implemented;
         when Ada_Component_Def_Range'First .. Ada_Component_Def_Range'Last =>
            This.Add_Not_Implemented;
         when Ada_Constraint'First .. Ada_Constraint'Last =>
            This.Add_Not_Implemented;
         when Ada_Constant_Node'First .. Ada_Constant_Node'Last =>
            This.Add_Not_Implemented;
         when Ada_Declarative_Part_Range'First .. Ada_Declarative_Part_Range'Last =>
            This.Add_Not_Implemented;
         when Ada_Elsif_Expr_Part_Range'First .. Ada_Elsif_Expr_Part_Range'Last =>
            This.Add_Not_Implemented;
         when Ada_Elsif_Stmt_Part_Range'First .. Ada_Elsif_Stmt_Part_Range'Last =>
            This.Add_Not_Implemented;
         --  Moved to top:
         --  when Ada_Expr'First .. Ada_Expr'Last =>
         when Ada_Handled_Stmts_Range'First .. Ada_Handled_Stmts_Range'Last =>
            This.Add_Not_Implemented;
         when Ada_Interface_Kind'First .. Ada_Interface_Kind'Last =>
            This.Add_Not_Implemented;
         when Ada_Iter_Type'First .. Ada_Iter_Type'Last =>
            This.Add_Not_Implemented;
         when Ada_Library_Item_Range'First .. Ada_Library_Item_Range'Last =>
            This.Add_Not_Implemented;
         when Ada_Limited_Node'First .. Ada_Limited_Node'Last =>
            This.Add_Not_Implemented;
         when Ada_Loop_Spec'First .. Ada_Loop_Spec'Last =>
            This.Add_Not_Implemented;
         when Ada_Mode'First .. Ada_Mode'Last =>
            This.Add_Not_Implemented;
         when Ada_Not_Null'First .. Ada_Not_Null'Last =>
            This.Add_Not_Implemented;
         when Ada_Null_Component_Decl_Range'First .. Ada_Null_Component_Decl_Range'Last =>
            This.Add_Not_Implemented;
         when Ada_Others_Designator_Range'First .. Ada_Others_Designator_Range'Last =>
            This.Add_Not_Implemented;
         when Ada_Overriding_Node'First .. Ada_Overriding_Node'Last =>
            This.Add_Not_Implemented;
         when Ada_Params_Range'First .. Ada_Params_Range'Last =>
            This.Add_Not_Implemented;
         when Ada_Pragma_Node_Range'First .. Ada_Pragma_Node_Range'Last =>
            This.Add_Not_Implemented;
         when Ada_Prim_Type_Accessor_Range'First .. Ada_Prim_Type_Accessor_Range'Last =>
            This.Add_Not_Implemented;
         when Ada_Private_Node'First .. Ada_Private_Node'Last =>
            This.Add_Not_Implemented;
         when Ada_Protected_Node'First .. Ada_Protected_Node'Last =>
            This.Add_Not_Implemented;
         when Ada_Protected_Def_Range'First .. Ada_Protected_Def_Range'Last =>
            This.Add_Not_Implemented;
         when Ada_Quantifier'First .. Ada_Quantifier'Last =>
            This.Add_Not_Implemented;
         when Ada_Range_Spec_Range'First .. Ada_Range_Spec_Range'Last =>
            This.Add_Not_Implemented;
         when Ada_Renaming_Clause_Range'First .. Ada_Renaming_Clause_Range'Last =>
            This.Add_Not_Implemented;
         when Ada_Reverse_Node'First .. Ada_Reverse_Node'Last =>
            This.Add_Not_Implemented;
         when Ada_Select_When_Part_Range'First .. Ada_Select_When_Part_Range'Last =>
            This.Add_Not_Implemented;
         when Ada_Subp_Kind'First .. Ada_Subp_Kind'Last =>
            This.Add_Not_Implemented;
         when Ada_Subunit_Range'First .. Ada_Subunit_Range'Last =>
            This.Add_Not_Implemented;
         --  Moved to top:
         --  when Ada_Stmt'First .. Ada_Stmt'Last =>
         when Ada_Synchronized_Node'First .. Ada_Synchronized_Node'Last =>
            This.Add_Not_Implemented;
         when Ada_Tagged_Node'First .. Ada_Tagged_Node'Last =>
            This.Add_Not_Implemented;
         --  Moved to top:
         --  when Ada_Type_Def'First .. Ada_Type_Def'Last =>
         when Ada_Task_Def_Range'First .. Ada_Task_Def_Range'Last =>
            This.Add_Not_Implemented;
         when Ada_Type_Expr'First .. Ada_Type_Expr'Last =>
            This.Add_Not_Implemented;
         when Ada_Unconstrained_Array_Index_Range'First .. Ada_Unconstrained_Array_Index_Range'Last =>
            This.Add_Not_Implemented;
         when Ada_Until_Node'First .. Ada_Until_Node'Last =>
            This.Add_Not_Implemented;
         when Ada_Use_Clause'First .. Ada_Use_Clause'Last =>
            This.Add_Not_Implemented;
         when Ada_Variant_Range'First .. Ada_Variant_Range'Last =>
            This.Add_Not_Implemented;
         when Ada_Variant_Part_Range'First .. Ada_Variant_Part_Range'Last =>
            This.Add_Not_Implemented;
         when Ada_With_Clause_Range'First .. Ada_With_Clause_Range'Last =>
            This.Add_Not_Implemented;
         when Ada_With_Private'First .. Ada_With_Private'Last =>
            This.Add_Not_Implemented;
      end case;
      --  end if;
   exception
      when X : External_Error | Internal_Error | Usage_Error =>
         raise;
      when X: others =>
         Log_Exception (X);
         Log ("No handler for this exception.  Raising Internal_Error");
         raise Internal_Error;
   end Process;

end Lal_Adapter.Node;
