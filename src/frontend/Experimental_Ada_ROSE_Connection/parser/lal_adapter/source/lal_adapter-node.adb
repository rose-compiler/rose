with Generic_Logging;
with Langkit_Support.Slocs;
with Langkit_Support.Text;

package body Lal_Adapter.Node is
   package Slocs renames Langkit_Support.Slocs;
   package Text renames Langkit_Support.Text;

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
   begin -- processing
      This.Outputs := Outputs;
      --  Log ("Line" & Start_Line_Image & ": " & Kind_Image  & ": " & Debug_Text);
      --  if Node.Kind /= LALCO.Ada_Compilation_Unit then
      Log (Kind_Image & " " & Kind_Name & " at " & Sloc_Range_Image);
      Log (LAL.Image(Node));
      case Kind is
         when Ada_Basic_Decl'First .. Ada_Basic_Decl'Last =>
            This.Add_Not_Implemented;
         when Ada_Expr'First .. Ada_Expr'Last =>
            null;
         when Ada_Stmt'First .. Ada_Stmt'Last =>
            null;
         when Ada_Type_Def'First .. Ada_Type_Def'Last =>
            null;


         when Ada_Abort_Node'First .. Ada_Abort_Node'Last =>
            null;
         when Ada_Abstract_Node'First .. Ada_Abstract_Node'Last =>
            null;
         when Ada_Ada_List'First .. Ada_Ada_List'Last =>
            null;
         when Ada_Aliased_Node'First .. Ada_Aliased_Node'Last =>
            null;
         when Ada_All_Node'First .. Ada_All_Node'Last =>
            null;
         when Ada_Array_Indices'First .. Ada_Array_Indices'Last =>
            null;
         when Ada_Aspect_Assoc_Range'First .. Ada_Aspect_Assoc_Range'Last =>
            null;
         when Ada_Aspect_Clause'First .. Ada_Aspect_Clause'Last =>
            null;
         when Ada_Aspect_Spec_Range'First .. Ada_Aspect_Spec_Range'Last =>
            null;
         when Ada_Base_Assoc'First .. Ada_Base_Assoc'Last =>
            null;
         when Ada_Base_Formal_Param_Holder'First .. Ada_Base_Formal_Param_Holder'Last =>
            null;
         when Ada_Base_Record_Def'First .. Ada_Base_Record_Def'Last =>
            null;
         when Ada_Basic_Assoc'First .. Ada_Basic_Assoc'Last =>
            null;
         when Ada_Case_Stmt_Alternative_Range'First .. Ada_Case_Stmt_Alternative_Range'Last =>
            null;
         when Ada_Compilation_Unit_Range'First .. Ada_Compilation_Unit_Range'Last =>
            null;
         when Ada_Component_Clause_Range'First .. Ada_Component_Clause_Range'Last =>
            null;
         when Ada_Component_Def_Range'First .. Ada_Component_Def_Range'Last =>
            null;
         when Ada_Constraint'First .. Ada_Constraint'Last =>
            null;
         when Ada_Constant_Node'First .. Ada_Constant_Node'Last =>
            null;
            --  when Ada_Contract_Cases_Range'First .. Ada_Contract_Cases_Range'Last =>
            null;
         when Ada_Declarative_Part_Range'First .. Ada_Declarative_Part_Range'Last =>
            null;
         when Ada_Elsif_Expr_Part_Range'First .. Ada_Elsif_Expr_Part_Range'Last =>
            null;
         when Ada_Elsif_Stmt_Part_Range'First .. Ada_Elsif_Stmt_Part_Range'Last =>
            null;
         when Ada_Handled_Stmts_Range'First .. Ada_Handled_Stmts_Range'Last =>
            null;
         when Ada_Interface_Kind'First .. Ada_Interface_Kind'Last =>
            null;
         when Ada_Iter_Type'First .. Ada_Iter_Type'Last =>
            null;
         when Ada_Library_Item_Range'First .. Ada_Library_Item_Range'Last =>
            null;
         when Ada_Limited_Node'First .. Ada_Limited_Node'Last =>
            null;
         when Ada_Loop_Spec'First .. Ada_Loop_Spec'Last =>
            null;
         when Ada_Mode'First .. Ada_Mode'Last =>
            null;
         when Ada_Not_Null'First .. Ada_Not_Null'Last =>
            null;
         when Ada_Null_Component_Decl_Range'First .. Ada_Null_Component_Decl_Range'Last =>
            null;
         when Ada_Others_Designator_Range'First .. Ada_Others_Designator_Range'Last =>
            null;
         when Ada_Overriding_Node'First .. Ada_Overriding_Node'Last =>
            null;
         when Ada_Params_Range'First .. Ada_Params_Range'Last =>
            null;
         when Ada_Pragma_Node_Range'First .. Ada_Pragma_Node_Range'Last =>
            null;
         when Ada_Prim_Type_Accessor_Range'First .. Ada_Prim_Type_Accessor_Range'Last =>
            null;
         when Ada_Private_Node'First .. Ada_Private_Node'Last =>
            null;
         when Ada_Protected_Node'First .. Ada_Protected_Node'Last =>
            null;
         when Ada_Protected_Def_Range'First .. Ada_Protected_Def_Range'Last =>
            null;
         when Ada_Quantifier'First .. Ada_Quantifier'Last =>
            null;
         when Ada_Range_Spec_Range'First .. Ada_Range_Spec_Range'Last =>
            null;
         when Ada_Renaming_Clause_Range'First .. Ada_Renaming_Clause_Range'Last =>
            null;
         when Ada_Reverse_Node'First .. Ada_Reverse_Node'Last =>
            null;
         when Ada_Select_When_Part_Range'First .. Ada_Select_When_Part_Range'Last =>
            null;
         when Ada_Subp_Kind'First .. Ada_Subp_Kind'Last =>
            null;
         when Ada_Subunit_Range'First .. Ada_Subunit_Range'Last =>
            null;
         when Ada_Synchronized_Node'First .. Ada_Synchronized_Node'Last =>
            null;
         when Ada_Tagged_Node'First .. Ada_Tagged_Node'Last =>
            null;
         when Ada_Task_Def_Range'First .. Ada_Task_Def_Range'Last =>
            null;
         when Ada_Type_Expr'First .. Ada_Type_Expr'Last =>
            null;
         when Ada_Unconstrained_Array_Index_Range'First .. Ada_Unconstrained_Array_Index_Range'Last =>
            null;
         when Ada_Until_Node'First .. Ada_Until_Node'Last =>
            null;
         when Ada_Use_Clause'First .. Ada_Use_Clause'Last =>
            null;
         when Ada_Variant_Range'First .. Ada_Variant_Range'Last =>
            null;
         when Ada_Variant_Part_Range'First .. Ada_Variant_Part_Range'Last =>
            null;
         when Ada_With_Clause_Range'First .. Ada_With_Clause_Range'Last =>
            null;
         when Ada_With_Private'First .. Ada_With_Private'Last =>
            null;
      end case;
      if Kind in LALCO.Ada_Stmt then
         Log ("Statement");
      else
         Log ("NOT a statement");
      end if;

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
