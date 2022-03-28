with Ada.Tags;
with Generic_Logging;
with Langkit_Support.Slocs;
with Langkit_Support.Text;
With System.Address_Image;

package body Lal_Adapter.Node is

   package Slocs renames Langkit_Support.Slocs;
   package Text renames Langkit_Support.Text;

   -----------------------------------------------------------------------------
   -- Element_ID support

   -- Unlike ASIS Elements, libadalang Nodes have no unique ID as far as I can tell.
   -- Until we can come up with something better, we will just use an incrementing
   -- counter.  THIS GIVES A DIFFERENT ANSWER EVERY TIME Get_Element_ID IS CALLED.
   -- This is good to keep all nodes from having the same ID, but it is bad for
   -- determinining if two nodes are the same.
   -- TODO: Implement by storing and hashing Nodes?
   Last_Node_ID : Natural := anhS.Empty_ID;

   Node_Map : Node_ID_Map.Map := Node_ID_Map.Empty_Map; 

   ------------
   -- EXPORTED:
   ------------
   function Get_Element_ID
     (Node : in LAL.Ada_Node'Class)
      return Element_ID is
-- Tried to use address for mapping but got nodes with same address.
-- Use Image for now till we have better option for the mapping.
     Node_Image : String :=  LAL.Image(Node);
     C : constant Node_ID_Map.Cursor := Node_Map.Find (Node_Image);
     use type Node_ID_Map.Cursor;
     Node_Id : Integer := 0; 
   begin
      -- Put_Line("Node: " & Node_Image);
      if LAL.Is_Null (Node) then
         return No_Element_ID;
      else
         if C = Node_ID_Map.No_Element then
            Last_Node_ID := Last_Node_ID + 1;
            Node_Map.Insert (Node_Image, Last_Node_ID);
            Node_Id := Last_Node_ID;
         else
            Node_Id := Node_ID_Map.Element (C);
         end if; 

         return (Node_ID => Node_Id,
                 Kind    => Node.Kind);
      end if;
   end Get_Element_ID;

   ------------
   -- EXPORTED:
   ------------
   function To_Element_ID
     (This : in Element_ID)
      return a_nodes_h.Element_ID
   is
      Result : Integer;
   begin
      Result := Integer (This.Node_ID) * 1000 +
        LALCO.Ada_Node_Kind_Type'Pos(This.Kind);
      return a_nodes_h.Element_ID (Result);
   end To_Element_ID;

   ------------
   -- EXPORTED:
   ------------
   function Get_Element_ID
     (Element : in LAL.Ada_Node'Class)
      return a_nodes_h.Element_ID
   is
      (To_Element_ID (Get_Element_ID (Element)));

   ------------
   -- EXPORTED:
   ------------
   function To_String
     (This : in a_nodes_h.Element_ID)
      return String
   is
     (To_String (This, Element_ID_Kind));

   -- END Element_ID support
   -----------------------------------------------------------------------------
   ----------------------
   -- EXPORTED (private):
   ----------------------

   procedure Add_To_Dot_Label
     (This  : in out Class;
      Value : in     String) is
   begin
      This.Add_To_Dot_Label 
        (Dot_Label => This.Dot_Label,
         Value     => Value);
   end Add_To_Dot_Label;

   procedure Add_To_Dot_Label
     (This  : in out Class;
      Name  : in     String;
      Value : in     String) is
   begin
      This.Add_To_Dot_Label 
        (Dot_Label => This.Dot_Label,
         Name      => Name,
         Value     => Value);
   end Add_To_Dot_Label;

   procedure Add_To_Dot_Label
     (This  : in out Class;
      Name  : in     String;
      Value : in     a_nodes_h.Element_ID) is
   begin
      This.Add_To_Dot_Label 
        (Name  => Name, 
         Value => To_String (Value));
   end Add_To_Dot_Label;

   procedure Add_To_Dot_Label
     (This  : in out Class;
      Name  : in     String;
      Value : in     Boolean) is
   begin
      This.Add_To_Dot_Label 
        (Dot_Label => This.Dot_Label,
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
      This.Add_Dot_Edge 
        (From      => From,
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
      This.Add_To_Dot_Label 
        (Name  => Label, 
         Value => To_String (To));
      This.Add_Dot_Edge 
        (From  => This.Element_IDs.First_Element,
         To    => To,
         Label => Label);
   end Add_To_Dot_Label_And_Edge;

   procedure Add_Not_Implemented
     (This        : in out Class;
      Ada_Version : in     Ada_Versions := Supported_Ada_Version) is
   begin
      if Ada_Version <= Supported_Ada_Version then
         This.Add_To_Dot_Label
           (Name  => "LIBADALANG_PROCESSING", 
            Value => String'("NOT_IMPLEMENTED_COMPLETELY"));
         This.Outputs.A_Nodes.Add_Not_Implemented;
      else
         This.Add_To_Dot_Label
           (Name  => "LIBADALANG_PROCESSING",
            Value => Ada_Version'Image & "_FEATURE_NOT_IMPLEMENTED_IN_" &
             Supported_Ada_Version'Image);
      end if;
   end Add_Not_Implemented;

   ------------
   -- Exported:
   ------------
   procedure Process_Ada_Abort_Node
     (This    : in out Class;
      --  Node    : in     LAL.Stmt'Class;
      Node    : in     LAL.Ada_Node'Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process_Abort_Node";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      --  Auto : Logging.Auto_Logger; -- Logs BEGIN and END

      -- Will raise Constraint_Error if Node.Kind is not in Ada_Stmt:
      Kind             : constant LALCO.Ada_Abort_Node := Node.Kind;
      use LALCO; -- For subtype names in case stmt
   begin -- Process_Ada_Abort_Node
      case Kind is

         when Ada_Abort_Absent =>
            declare
               Abort_Absent_Node : constant LAL.Abort_Absent := LAL.As_Abort_Absent (Node);
            begin
               NULL; 
            end;

         when Ada_Abort_Present =>
            declare
               Abort_Present_Node : constant LAL.Abort_Present := LAL.As_Abort_Present (Node);
            begin
               NULL; 
            end;

      end case;
   end Process_Ada_Abort_Node;

   procedure Process_Ada_Abstract_Node
     (This    : in out Class;
      --  Node    : in     LAL.Stmt'Class;
      Node    : in     LAL.Ada_Node'Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process_Abstract_Node";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      --  Auto : Logging.Auto_Logger; -- Logs BEGIN and END

      -- Will raise Constraint_Error if Node.Kind is not in Ada_Stmt:
      Kind             : constant LALCO.Ada_Abstract_Node := Node.Kind;
      use LALCO; -- For subtype names in case stmt
   begin -- Process_Ada_Abstract_Node
      case Kind is

         when Ada_Abstract_Absent =>
            declare
              Abstract_Absent_Node : constant LAL.Abstract_Absent := LAL.As_Abstract_Absent (Node);
            begin
               NULL; 
            end;

         when Ada_Abstract_Present =>
            declare
              Abstract_Present_Node : constant LAL.Abstract_Present := LAL.As_Abstract_Present (Node);
            begin
               NULL; 
            end;

      end case;
   end Process_Ada_Abstract_Node;

   procedure Process_Ada_Ada_List
     (This    : in out Class;
      --  Node    : in     LAL.Stmt'Class;
      Node    : in     LAL.Ada_Node'Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process_Ada_List";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      --  Auto : Logging.Auto_Logger; -- Logs BEGIN and END

      -- Will raise Constraint_Error if Node.Kind is not in Ada_Stmt:
      Kind             : constant LALCO.Ada_Ada_List := Node.Kind;
      use LALCO; -- For subtype names in case stmt
   begin -- Process_Ada_Ada_List
      case Kind is

         when Ada_Ada_Node_List =>
            declare
               Ada_Node_List_Node : constant LAL.Ada_Node_List := LAL.As_Ada_Node_List (Node);
               NodeListFirst : constant Positive := LAL.Ada_Node_List_First (Ada_Node_List_Node);
            begin
               Log ("NodeListFirst: " & NodeListFirst'Image);
            end;

         when Ada_Abstract_State_Decl_List =>
            This.Add_Not_Implemented;
            
         when Ada_Alternatives_List =>
            declare
               Alternatives_List_Node : constant LAL.Alternatives_List := LAL.As_Alternatives_List (Node);
            begin
              NULL;
            end;

         when Ada_Constraint_List =>
            declare
               Constraint_List_Node : constant LAL.Constraint_List := LAL.As_Constraint_List (Node);
            begin
              NULL;
            end;

         when Ada_Decl_List =>
            declare
               Decl_List_Node : constant LAL.Decl_List := LAL.As_Decl_List (Node);
            begin
              NULL;
            end;

         when Ada_Stmt_List =>
            declare
               Stmt_List_Node : constant LAL.Stmt_List := LAL.As_Stmt_List (Node);
            begin
              NULL;
            end;

         when Ada_Aspect_Assoc_List =>
            declare
               Aspect_Assoc_List_Node : constant LAL.Aspect_Assoc_List := LAL.As_Aspect_Assoc_List (Node);
               AspectAsoocListFirst : constant Positive := LAL.Aspect_Assoc_List_First (Aspect_Assoc_List_Node);
            begin
               Log ("AspectAsoocListFirst: " & AspectAsoocListFirst'Image);
            end;

         when Ada_Base_Assoc_List =>
            declare
               Base_Assoc_List_Node : constant LAL.Base_Assoc_List := LAL.As_Base_Assoc_List (Node);
               BaseAsoocListFirst : constant Positive := LAL.Base_Assoc_List_First (Base_Assoc_List_Node);
            begin
               Log ("BaseAsoocListFirst: " & BaseAsoocListFirst'Image);
            end;

         when Ada_Assoc_List =>
            declare
               Assoc_list_Node : constant LAL.Assoc_list := LAL.As_Assoc_list (Node);
            begin
              NULL; 
            end;

         when Ada_Basic_Decl_List =>
            This.Add_Not_Implemented;
            
         when Ada_Case_Expr_Alternative_List =>
            declare
               Case_Expr_Alternative_List_Node : constant LAL.Case_Expr_Alternative_List := LAL.As_Case_Expr_Alternative_List (Node);
               CaseExprAlternativeListFirst : constant Positive := LAL.Case_Expr_Alternative_List_First (Case_Expr_Alternative_List_Node);
            begin
               Log ("CaseExprAlternativeListFirst: " & CaseExprAlternativeListFirst'Image);
            end;

         when Ada_Case_Stmt_Alternative_List =>
            declare
               Case_Stmt_Alternative_List_Node : constant LAL.Case_Stmt_Alternative_List := LAL.As_Case_Stmt_Alternative_List (Node);
               CaseStmtAlernativeListFirst : constant Positive := LAL.Case_Stmt_Alternative_List_First (Case_Stmt_Alternative_List_Node);
            begin
               Log ("CaseStmtAlernativeListFirst: " & CaseStmtAlernativeListFirst'Image);
            end;

         when Ada_Compilation_Unit_List =>
            declare
               Compilation_Unit_List_Node : constant LAL.Compilation_Unit_List := LAL.As_Compilation_Unit_List (Node);
               CompilationUnitListFirst : constant Positive := LAL.Compilation_Unit_List_First (Compilation_Unit_List_Node);
            begin
               Log ("CompilationUnitListFirst: " & CompilationUnitListFirst'Image);
            end;

         when Ada_Contract_Case_Assoc_List =>
            declare
               Contract_Case_Assoc_List_Node : constant LAL.Contract_Case_Assoc_List := LAL.As_Contract_Case_Assoc_List (Node);
               ContractCastAssocListFirst : constant Positive := LAL.Contract_Case_Assoc_List_First (Contract_Case_Assoc_List_Node);
            begin
               Log ("ContractCastAssocListFirst: " & ContractCastAssocListFirst'Image);
            end;

         when Ada_Defining_Name_List =>
            declare
               Defining_Name_List_Node : constant LAL.Defining_Name_List := LAL.As_Defining_Name_List (Node);
               DefiningNameListFirst : constant Positive := LAL.Defining_Name_List_First (Defining_Name_List_Node);
            begin
               Log ("DefiningNameListFirst: " & DefiningNameListFirst'Image);
            end;

         when Ada_Discriminant_Spec_List =>
            declare
               Discriminant_Spec_List_Node : constant LAL.Discriminant_Spec_List := LAL.As_Discriminant_Spec_List (Node);
               DiscriminantSpecListNodeFirst : constant Positive := LAL.Discriminant_Spec_List_First (Discriminant_Spec_List_Node);
            begin
               Log ("DiscriminantSpecListNodeFirst: " & DiscriminantSpecListNodeFirst'Image);
            end;

         when Ada_Elsif_Expr_Part_List =>
            declare
               Elsif_Expr_Part_List_Node : constant LAL.Elsif_Expr_Part_List := LAL.As_Elsif_Expr_Part_List (Node);
               ElsifExprPartListNodeFirst : constant Positive := LAL.Elsif_Expr_Part_List_First (Elsif_Expr_Part_List_Node);
            begin
               Log ("ElsifExprPartListNodeFirst: " & ElsifExprPartListNodeFirst'Image);
            end;

         when Ada_Elsif_Stmt_Part_List =>
            declare
               Elsif_Stmt_Part_List_Node : constant LAL.Elsif_Stmt_Part_List := LAL.As_Elsif_Stmt_Part_List (Node);
               ElsifStmtPartListNodeFirst : constant Positive := LAL.Elsif_Stmt_Part_List_First (Elsif_Stmt_Part_List_Node);
            begin
               Log ("ElsifExprPartListNodeFirst: " & ElsifStmtPartListNodeFirst'Image);
            end;

         when Ada_Enum_Literal_Decl_List =>
            declare
               Enum_Literal_Decl_List_Node : constant LAL.Enum_Literal_Decl_List := LAL.As_Enum_Literal_Decl_List (Node);
               EnumLiteralDeclListNodeFirst : constant Positive := LAL.Enum_Literal_Decl_List_First (Enum_Literal_Decl_List_Node);
            begin
               Log ("EnumLiteralDeclListNodeFirst: " & EnumLiteralDeclListNodeFirst'Image);
            end;

         when Ada_Expr_Alternatives_List =>
            declare
               Expr_Alternatives_List_Node : constant LAL.Expr_Alternatives_List := LAL.As_Expr_Alternatives_List (Node);
            begin
              NULL; 
            end;

         when Ada_Discriminant_Choice_List =>
            declare
               Discriminant_Choice_List_Node : constant LAL.Discriminant_Choice_List := LAL.As_Discriminant_Choice_List (Node);
            begin
              NULL; 
            end;

         when Ada_Name_List =>
            declare
               Name_List_Node : constant LAL.Name_List := LAL.As_Name_List (Node);
               NameListFirst : constant Positive := LAL.Name_List_First (Name_List_Node);
            begin
               Log ("NameListFirst: " & NameListFirst'Image);
            end;

         when Ada_Parent_List =>
            declare
               Parent_List_Node : constant LAL.Parent_List := LAL.As_Parent_List (Node);
            begin
              NULL; 
            end;

         when Ada_Param_Spec_List =>
            declare
               Param_Spec_List_Node : constant LAL.Param_Spec_List := LAL.As_Param_Spec_List (Node);
               ParamSpecListFirst : constant Positive := LAL.Param_Spec_List_First (Param_Spec_List_Node);
            begin
               Log ("ParamSpecListFirst: " & ParamSpecListFirst'Image);
            end;

         when Ada_Pragma_Node_List =>
            declare
               Pragma_Node_List_Node : constant LAL.Pragma_Node_List := LAL.As_Pragma_Node_List (Node);
               PragmaNodeListFirst : constant Positive := LAL.Pragma_Node_List_First (Pragma_Node_List_Node);
            begin
               Log ("PragmaNodeListFirst: " & PragmaNodeListFirst'Image);
            end;

         when Ada_Select_When_Part_List =>
            declare
               Select_When_Part_List_Node : constant LAL.Select_When_Part_List := LAL.As_Select_When_Part_List (Node);
               SelectWhenPartListFirst : constant Positive := LAL.Select_When_Part_List_First (Select_When_Part_List_Node);
            begin
               Log ("SelectWhenPartListFirst: " & SelectWhenPartListFirst'Image);
            end;

         when Ada_Unconstrained_Array_Index_List =>
            declare
               Unconstrained_Array_Index_List_Node : constant LAL.Unconstrained_Array_Index_List := LAL.As_Unconstrained_Array_Index_List (Node);
               UnconstrainedArrayIndexListFirst : constant Positive := LAL.Unconstrained_Array_Index_List_First (Unconstrained_Array_Index_List_Node);
            begin
               Log ("UnconstrainedArrayIndexListFirst: " & UnconstrainedArrayIndexListFirst'Image);
            end;

         when Ada_Variant_List =>
            declare
               Variant_List_Node : constant LAL.Variant_List := LAL.As_Variant_List (Node);
               VariantListFirst : constant Positive := LAL.Variant_List_First (Variant_List_Node);
            begin
               Log ("VariantListFirst: " & VariantListFirst'Image);
            end;

      end case;
   end Process_Ada_Ada_List;

   procedure Process_Ada_Aliased_Node
     (This    : in out Class;
      --  Node    : in     LAL.Stmt'Class;
      Node    : in     LAL.Ada_Node'Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process_Aliased_Node";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      --  Auto : Logging.Auto_Logger; -- Logs BEGIN and END

      -- Will raise Constraint_Error if Node.Kind is not in Ada_Stmt:
      Kind             : constant LALCO.Ada_Aliased_Node := Node.Kind;
      use LALCO; -- For subtype names in case stmt
   begin -- Process_Ada_Aliased_Node
      case Kind is

         when Ada_Aliased_Absent =>
            declare
               Aliased_Absent_Node : constant LAL.Aliased_Absent := LAL.As_Aliased_Absent (Node);
            begin
              NULL; 
            end;

         when Ada_Aliased_Present =>
            declare
               Aliased_Present_Node : constant LAL.Aliased_Present := LAL.As_Aliased_Present (Node);
            begin
              NULL; 
            end;

      end case;
   end Process_Ada_Aliased_Node;

   procedure Process_Ada_All_Node
     (This    : in out Class;
      --  Node    : in     LAL.Stmt'Class;
      Node    : in     LAL.Ada_Node'Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process_All_Node";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      --  Auto : Logging.Auto_Logger; -- Logs BEGIN and END

      -- Will raise Constraint_Error if Node.Kind is not in Ada_Stmt:
      Kind             : constant LALCO.Ada_All_Node := Node.Kind;
      use LALCO; -- For subtype names in case stmt
   begin -- Process_Ada_All_Node
      case Kind is

         when Ada_All_Absent =>
            declare
               All_Absent_Node : constant LAL.All_Absent := LAL.As_All_Absent (Node);
            begin
              NULL; 
            end;

         when Ada_All_Present =>
            declare
               All_Present_Node : constant LAL.All_Present := LAL.As_All_Present (Node);
            begin
              NULL; 
            end;

      end case;
   end Process_Ada_All_Node;

   procedure Process_Ada_Array_Indices
     (This    : in out Class;
      --  Node    : in     LAL.Stmt'Class;
      Node    : in     LAL.Ada_Node'Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process_Array_Indices";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      --  Auto : Logging.Auto_Logger; -- Logs BEGIN and END

      -- Will raise Constraint_Error if Node.Kind is not in Ada_Stmt:
      Kind             : constant LALCO.Ada_Array_Indices := Node.Kind;
      use LALCO; -- For subtype names in case stmt
   begin -- Process_Ada_Array_Indices
      case Kind is

         when Ada_Constrained_Array_Indices =>
            declare
               Constrained_Array_Indices_Node : constant LAL.Constrained_Array_Indices := LAL.As_Constrained_Array_Indices (Node);
               ConstraintList : constant LAL.Constraint_List := LAL.F_List (Constrained_Array_Indices_Node);
            begin
               Log ("ConstraintList: " & ConstraintList.Image);
            end;

         when Ada_Unconstrained_Array_Indices =>
            declare
               Unconstrained_Array_Indices_Node : constant LAL.Unconstrained_Array_Indices := LAL.As_Unconstrained_Array_Indices (Node);
               UnconstrainedArrayIndexList : constant LAL.Unconstrained_Array_Index_List := LAL.F_Types (Unconstrained_Array_Indices_Node);
            begin
               Log ("UnconstrainedArrayIndexList: " & UnconstrainedArrayIndexList.Image);
            end;

      end case;
   end Process_Ada_Array_Indices;

   procedure Process_Ada_Aspect_Assoc_Range
     (This    : in out Class;
      --  Node    : in     LAL.Stmt'Class;
      Node    : in     LAL.Ada_Node'Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process_Aspect_Assoc_Range";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      --  Auto : Logging.Auto_Logger; -- Logs BEGIN and END

      -- Will raise Constraint_Error if Node.Kind is not in Ada_Stmt:
      Kind             : constant LALCO.Ada_Aspect_Assoc_Range := Node.Kind;
      use LALCO; -- For subtype names in case stmt
   begin -- Process_Ada_Aspect_Assoc_Range
      case Kind is

         when Ada_Aspect_Assoc =>
            declare
               Aspect_Assoc_Node : constant LAL.Aspect_Assoc := LAL.As_Aspect_Assoc (Node);
               name : constant LAL.Name := LAL.F_Id (Aspect_Assoc_Node);
               expr : constant LAL.Expr := LAL.F_Expr (Aspect_Assoc_Node);
            begin
               Log ("name: " & name.Image);
               if not expr.Is_Null then
                 Log ("expr: " & expr.Image);
               end if;
            end;

      end case;
   end Process_Ada_Aspect_Assoc_Range;

   procedure Process_Ada_Aspect_Clause
     (This    : in out Class;
      --  Node    : in     LAL.Stmt'Class;
      Node    : in     LAL.Ada_Node'Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process_Aspect_Clause";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      --  Auto : Logging.Auto_Logger; -- Logs BEGIN and END

      -- Will raise Constraint_Error if Node.Kind is not in Ada_Stmt:
      Kind             : constant LALCO.Ada_Aspect_Clause := Node.Kind;
      use LALCO; -- For subtype names in case stmt
   begin -- Process_Ada_Aspect_Clause
      case Kind is

         when Ada_At_Clause =>
            declare
               At_Clause_Node : constant LAL.At_Clause := LAL.As_At_Clause (Node);
               baseID : constant LAL.Base_Id := LAL.F_Name (At_Clause_Node);
               expr : constant LAL.Expr := LAL.F_Expr (At_Clause_Node);
            begin
               Log ("baseID: " & baseID.Image);
               Log ("expr: " & expr.Image);
            end;

         when Ada_Attribute_Def_Clause =>
            declare
               Attribute_Def_Clause_Node : constant LAL.Attribute_Def_Clause := LAL.As_Attribute_Def_Clause (Node);
               name : constant LAL.Name := LAL.F_Attribute_Expr (Attribute_Def_Clause_Node);
               expr : constant LAL.Expr := LAL.F_Expr (Attribute_Def_Clause_Node);
            begin
               Log ("name: " & name.Image);
               Log ("expr: " & expr.Image);
            end;

         when Ada_Enum_Rep_Clause =>
            declare
               Enum_Rep_Clause_Node : constant LAL.Enum_Rep_Clause := LAL.As_Enum_Rep_Clause (Node);
               name : constant LAL.Name := LAL.F_Type_Name (Enum_Rep_Clause_Node);
               baseAggregate : constant LAL.Base_Aggregate := LAL.F_Aggregate (Enum_Rep_Clause_Node);
            begin
               Log ("name: " & name.Image);
               Log ("baseAggregate: " & baseAggregate.Image);
            end;

         when Ada_Record_Rep_Clause =>
            declare
               Record_Rep_Clause_Node : constant LAL.Record_Rep_Clause := LAL.As_Record_Rep_Clause (Node);
               name : constant LAL.Name := LAL.F_Name (Record_Rep_Clause_Node);
               expr : constant LAL.Expr := LAL.F_At_Expr (Record_Rep_Clause_Node);
               nodeList : constant LAL.Ada_Node_List := LAL.F_Components (Record_Rep_Clause_Node);
            begin
               Log ("name: " & name.Image);
               if not expr.Is_Null then
                 Log ("expr: " & expr.Image);
               end if;
               Log ("nodeList: " & nodeList.Image);
            end;

      end case;
   end Process_Ada_Aspect_Clause;

   procedure Process_Ada_Aspect_Spec_Range
     (This    : in out Class;
      --  Node    : in     LAL.Stmt'Class;
      Node    : in     LAL.Ada_Node'Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process_Aspect_Spec_Range";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      --  Auto : Logging.Auto_Logger; -- Logs BEGIN and END

      -- Will raise Constraint_Error if Node.Kind is not in Ada_Stmt:
      Kind             : constant LALCO.Ada_Aspect_Spec_Range := Node.Kind;
      use LALCO; -- For subtype names in case stmt
   begin -- Process_Ada_Aspect_Spec_Range
      case Kind is

         when Ada_Aspect_Spec =>
            declare
               Aspect_Spec_Node : constant LAL.Aspect_Spec := LAL.As_Aspect_Spec (Node);
               aspectAssocList : constant LAL.Aspect_Assoc_List := LAL.F_Aspect_Assocs (Aspect_Spec_Node);
            begin
               Log ("aspectAssocList: " & aspectAssocList.Image);
            end;

      end case;
   end Process_Ada_Aspect_Spec_Range;


   procedure Process_Ada_Base_Assoc
     (This    : in out Class;
      --  Node    : in     LAL.Stmt'Class;
      Node    : in     LAL.Ada_Node'Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process_Base_Assoc";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      --  Auto : Logging.Auto_Logger; -- Logs BEGIN and END

      -- Will raise Constraint_Error if Node.Kind is not in Ada_Stmt:
      Kind             : constant LALCO.Ada_Base_Assoc := Node.Kind;
      use LALCO; -- For subtype names in case stmt
   begin -- Process_Ada_Base_Assoc
      case Kind is

         when Ada_Contract_Case_Assoc =>
            declare
               Contract_Case_Assoc_Node : constant LAL.Contract_Case_Assoc := LAL.As_Contract_Case_Assoc (Node);
               guard : constant LAL.Ada_Node := LAL.F_Guard (Contract_Case_Assoc_Node);
               consequence : constant LAL.Expr := LAL.F_Consequence (Contract_Case_Assoc_Node);
            begin
               Log ("guard: " & guard.Image);
               Log ("consequence: " & consequence.Image);
            end;

         when Ada_Pragma_Argument_Assoc =>
            declare
               Pragma_Argument_Assoc_Node : constant LAL.Pragma_Argument_Assoc := LAL.As_Pragma_Argument_Assoc (Node);
               id : constant LAL.Identifier := LAL.F_Id (Pragma_Argument_Assoc_Node);
               expr : constant LAL.Expr := LAL.F_Expr (Pragma_Argument_Assoc_Node);
            begin
               if not id.Is_Null then
                 Log ("id: " & id.Image);
               end if;
               if not expr.Is_Null then
                 Log ("expr: " & expr.Image);
               end if;
            end;

      end case;
   end Process_Ada_Base_Assoc;

   procedure Process_Ada_Base_Formal_Param_Holder
     (This    : in out Class;
      --  Node    : in     LAL.Stmt'Class;
      Node    : in     LAL.Ada_Node'Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process_Base_Formal_Param_Holder";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      --  Auto : Logging.Auto_Logger; -- Logs BEGIN and END

      -- Will raise Constraint_Error if Node.Kind is not in Ada_Stmt:
      Kind             : constant LALCO.Ada_Base_Formal_Param_Holder := Node.Kind;
      use LALCO; -- For subtype names in case stmt
   begin -- Process_Ada_Base_Formal_Param_Holder
      case Kind is

         when Ada_Entry_Spec =>
            declare
               Entry_Spec_Node : constant LAL.Entry_Spec := LAL.As_Entry_Spec (Node);
               entryName : constant LAL.Defining_Name := LAL.F_Entry_Name (Entry_Spec_Node);
               familyType : constant LAL.Ada_Node := LAL.F_Family_Type (Entry_Spec_Node);
               entryParams : constant LAL.Params := LAL.F_Entry_Params (Entry_Spec_Node);
            begin
               Log ("entryName: " & entryName.Image);
               if not familyType.Is_Null then
                 Log ("familyType: " & familyType.Image);
               end if;
               if not entryParams.Is_Null then
                 Log ("entryParams: " & entryParams.Image);
               end if;
            end;

         when Ada_Enum_Subp_Spec =>
            declare
               Enum_Subp_Spec_Node : constant LAL.Enum_Subp_Spec := LAL.As_Enum_Subp_Spec (Node);
            begin
              NULL;
            end;

         when Ada_Subp_Spec =>
            declare
               Subp_Spec_Node : constant LAL.Subp_Spec := LAL.As_Subp_Spec (Node);
               subpKind : constant LAL.Subp_Kind := LAL.F_Subp_Kind (Subp_Spec_Node);
               subpName : constant LAL.Defining_Name := LAL.F_Subp_Name (Subp_Spec_Node);
               subpParams : constant LAL.Params := LAL.F_Subp_Params (Subp_Spec_Node);
               subpReturn : constant LAL.Type_Expr := LAL.F_Subp_Returns (Subp_Spec_Node);
            begin
               Log ("subpKind: " & subpKind.Image);
               if not subpName.Is_Null then
                 Log ("subpName: " & subpName.Image);
               end if;
               if not subpParams.Is_Null then
                 Log ("subpParams: " & subpParams.Image);
               end if;
               if not subpReturn.Is_Null then
                 Log ("subpReturn: " & subpReturn.Image);
               end if;
            end;

         when Ada_Component_List =>
            declare
               Component_List_Node : constant LAL.Component_List := LAL.As_Component_List (Node);
               components : constant LAL.Ada_Node_List := LAL.F_Components (Component_List_Node);
               variantPart : constant LAL.Variant_Part := LAL.F_Variant_Part (Component_List_Node);
            begin
               Log ("components: " & components.Image);
               if not variantPart.Is_Null then
                 Log ("variantPart: " & variantPart.Image);
               end if;
            end;

         when Ada_Known_Discriminant_Part =>
            declare
               Known_Discriminant_Part_Node : constant LAL.Known_Discriminant_Part := LAL.As_Known_Discriminant_Part (Node);
               discrSpecs : constant LAL.Discriminant_Spec_List := LAL.F_Discr_Specs (Known_Discriminant_Part_Node);
            begin
               Log ("discrSpecs: " & discrSpecs.Image);
            end;

         when Ada_Unknown_Discriminant_Part =>
            declare
               Unknown_Discriminant_Part_Node : constant LAL.Unknown_Discriminant_Part := LAL.As_Unknown_Discriminant_Part (Node);
            begin
               NULL; 
            end;

         when Ada_Entry_Completion_Formal_Params =>
            declare
               Entry_Completion_Formal_Params_Node : constant LAL.Entry_Completion_Formal_Params := LAL.As_Entry_Completion_Formal_Params (Node);
               params : constant LAL.Params := LAL.F_Params (Entry_Completion_Formal_Params_Node);
            begin
               if not params.Is_Null then
                 Log ("params: " & params.Image);
               end if;
            end;

         when Ada_Generic_Formal_Part =>
            declare
               Generic_Formal_Part_Node : constant LAL.Generic_Formal_Part := LAL.As_Generic_Formal_Part (Node);
               decls : constant LAL.Ada_Node_List := LAL.F_Decls (Generic_Formal_Part_Node);
            begin
               Log ("decls: " & decls.Image);
            end;

      end case;
   end Process_Ada_Base_Formal_Param_Holder;

   procedure Process_Ada_Base_Record_Def
     (This    : in out Class;
      --  Node    : in     LAL.Stmt'Class;
      Node    : in     LAL.Ada_Node'Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process_Base_Record_Def";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      --  Auto : Logging.Auto_Logger; -- Logs BEGIN and END

      -- Will raise Constraint_Error if Node.Kind is not in Ada_Stmt:
      Kind             : constant LALCO.Ada_Base_Record_Def := Node.Kind;
      use LALCO; -- For subtype names in case stmt
   begin -- Process_Ada_Base_Record_Def
      case Kind is

         when Ada_Null_Record_Def =>
            declare
               Null_Record_Def_Node : constant LAL.Null_Record_Def := LAL.As_Null_Record_Def (Node);
            begin
               NULL; 
            end;

         when Ada_Record_Def =>
            declare
               Record_Def_Node : constant LAL.Record_Def := LAL.As_Record_Def (Node);
            begin
               NULL; 
            end;

      end case;
   end Process_Ada_Base_Record_Def;

   procedure Process_Ada_Basic_Assoc
     (This    : in out Class;
      --  Node    : in     LAL.Stmt'Class;
      Node    : in     LAL.Ada_Node'Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process_Basic_Assoc";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      --  Auto : Logging.Auto_Logger; -- Logs BEGIN and END

      -- Will raise Constraint_Error if Node.Kind is not in Ada_Stmt:
      Kind             : constant LALCO.Ada_Basic_Assoc := Node.Kind;
      use LALCO; -- For subtype names in case stmt
   begin -- Process_Ada_Basic_Assoc
      case Kind is

         when Ada_Aggregate_Assoc =>
            declare
               Aggregate_Assoc_Node : constant LAL.Aggregate_Assoc := LAL.As_Aggregate_Assoc (Node);
               designators : constant LAL.Alternatives_List := LAL.F_Designators (Aggregate_Assoc_Node);
               rExpr : constant LAL.Expr := LAL.F_R_Expr (Aggregate_Assoc_Node);
            begin
               Log ("designators: " & designators.Image);
               Log ("rExpr: " & rExpr.Image);
            end;

         when Ada_Multi_Dim_Array_Assoc =>
            declare
               Multi_Dim_Array_Assoc_Node : constant LAL.Multi_Dim_Array_Assoc := LAL.As_Multi_Dim_Array_Assoc (Node);
            begin
               NULL;
            end;

         when Ada_Discriminant_Assoc =>
            declare
               Discriminant_Assoc_Node : constant LAL.Discriminant_Assoc := LAL.As_Discriminant_Assoc (Node);
               ids : constant LAL.Discriminant_Choice_List := LAL.F_Ids (Discriminant_Assoc_Node);
               discrExpr : constant LAL.Expr := LAL.F_Discr_Expr (Discriminant_Assoc_Node);
            begin
               Log ("ids: " & ids.Image);
               Log ("discrExpr: " & discrExpr.Image);
            end;

         when Ada_Param_Assoc =>
            declare
               Param_Assoc_Node : constant LAL.Param_Assoc := LAL.As_Param_Assoc (Node);
               designators : constant LAL.Ada_Node := LAL.F_Designator (Param_Assoc_Node);
               rExpr : constant LAL.Expr := LAL.F_R_Expr (Param_Assoc_Node);
            begin
               if not designators.Is_Null then
                 Log ("designators: " & designators.Image);
               end if;
               if not rExpr.Is_Null then
                 Log ("rExpr: " & rExpr.Image);
               end if;
            end;
            
         when Ada_Iterated_Assoc =>
            This.Add_Not_Implemented;

      end case;
   end Process_Ada_Basic_Assoc;

   procedure Process_Ada_Basic_Decl
     (This    : in out Class;
      --  Node    : in     LAL.Stmt'Class;
      Node    : in     LAL.Ada_Node'Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process_Ada_Basic_Decl";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      --  Auto : Logging.Auto_Logger; -- Logs BEGIN and END

      -- Will raise Constraint_Error if Node.Kind is not in Ada_Stmt:
      Kind             : constant LALCO.Ada_Basic_Decl := Node.Kind;
      use LALCO; -- For subtype names in case stmt
   begin -- Process_Ada_Basic_Decl
      case Kind is
         when Ada_Abstract_State_Decl =>
            This.Add_Not_Implemented;
            
         when Ada_Anonymous_Expr_Decl =>
            This.Add_Not_Implemented;

         --when Ada_Base_Formal_Param_Decl =>
         --   This.Add_Not_Implemented;
         when Ada_Component_Decl =>
            declare
               Component_Decl_Node : constant LAL.Component_Decl := LAL.As_Component_Decl (Node);
               NameList : constant LAL.Defining_Name_List := LAL.F_Ids (Component_Decl_Node);
               Component_Def : constant LAL.Component_Def := LAL.F_Component_Def (Component_Decl_Node);
               Expr : constant LAL.Expr := LAL.F_Default_Expr (Component_Decl_Node);
            begin
               Log ("NameList: " & NameList.Image);
               Log ("Component_Def: " & Component_Def.Image);
               if not Expr.Is_Null then
                 Log ("Expr: " & Expr.Image);
               end if;
            end;

            This.Add_Not_Implemented;
         when Ada_Discriminant_Spec =>
            declare
               Discriminant_Spec_Node : constant LAL.Discriminant_Spec := LAL.As_Discriminant_Spec (Node);
               NameList : constant LAL.Defining_Name_List := LAL.F_Ids (Discriminant_Spec_Node);
               TypeExpr : constant LAL.Type_Expr := LAL.F_Type_Expr (Discriminant_Spec_Node);
               DefaultExpr : constant LAL.Expr := LAL.F_Default_Expr (Discriminant_Spec_Node);
            begin
               Log ("NameList: " & NameList.Image);
               Log ("TypeExpr: " & TypeExpr.Image);
               if not DefaultExpr.Is_Null then
                 Log ("DefaultExpr: " & DefaultExpr.Image);
               end if;
            end;

            This.Add_Not_Implemented;
         --when Ada_Generic_Formal =>
         --   This.Add_Not_Implemented;
         when Ada_Generic_Formal_Obj_Decl =>
            declare
               Generic_Formal_Obj_Decl_Node : constant LAL.Generic_Formal_Obj_Decl := LAL.As_Generic_Formal_Obj_Decl (Node);
            begin
              NULL;
            end;

            This.Add_Not_Implemented;
         when Ada_Generic_Formal_Package =>
            declare
               Generic_Formal_Package_Node : constant LAL.Generic_Formal_Package := LAL.As_Generic_Formal_Package (Node);
            begin
              NULL;
            end;

            This.Add_Not_Implemented;
         when Ada_Generic_Formal_Subp_Decl =>
            declare
               Generic_Formal_Subp_Decl_Node : constant LAL.Generic_Formal_Subp_Decl := LAL.As_Generic_Formal_Subp_Decl (Node);
            begin
              NULL;
            end;

            This.Add_Not_Implemented;
         when Ada_Generic_Formal_Type_Decl =>
            declare
               Generic_Formal_Type_Decl_Node : constant LAL.Generic_Formal_Type_Decl := LAL.As_Generic_Formal_Type_Decl (Node);
            begin
              NULL;
            end;

            This.Add_Not_Implemented;
         when Ada_Param_Spec =>
            declare
               Param_Spec_Node : constant LAL.Param_Spec := LAL.As_Param_Spec (Node);
               NameList : constant LAL.Defining_Name_List := LAL.F_Ids (Param_Spec_Node);
               Has_Aliased : constant Boolean := LAL.F_Has_Aliased (Param_Spec_Node);
               Mode : constant LAL.Mode := LAL.F_Mode (Param_Spec_Node);
            begin
               Log ("NameList: " & NameList.Image);
               Log ("Has_Alias: " & Boolean'Image (Has_Aliased));
               Log ("Mode: " & Mode.Image);
            end;

            This.Add_Not_Implemented;
         --when Ada_Base_Package_Decl =>
         --   This.Add_Not_Implemented;
         when Ada_Generic_Package_Internal =>
            declare
               Generic_Package_Internal_Node : constant LAL.Generic_Package_Internal := LAL.As_Generic_Package_Internal (Node);
            begin
              NULL;
            end;

            This.Add_Not_Implemented;
         when Ada_Package_Decl =>
            declare
               Package_Decl_Node : constant LAL.Package_Decl := LAL.As_Package_Decl (Node);
            begin
              NULL;
            end;

            This.Add_Not_Implemented;
         --when Ada_Base_Type_Decl =>
         --   This.Add_Not_Implemented;
         --when Ada_Base_Subtype_Decl =>
         --   This.Add_Not_Implemented;
         when Ada_Discrete_Base_Subtype_Decl =>
            declare
               Discrete_Base_Subtype_Decl_Node : constant LAL.Discrete_Base_Subtype_Decl := LAL.As_Discrete_Base_Subtype_Decl (Node);
            begin
              NULL;
            end;

            This.Add_Not_Implemented;
         when Ada_Subtype_Decl =>
            declare
               Subtype_Decl_Node : constant LAL.Subtype_Decl := LAL.As_Subtype_Decl (Node);
               bareSubtype : constant LAL.Subtype_Indication := LAL.F_Subtype (Subtype_Decl_Node);
            begin
               Log ("bareSubtype: " & bareSubtype.Image);
            end;

            This.Add_Not_Implemented;
         when Ada_Classwide_Type_Decl =>
            declare
               Classwide_Type_Decl_Node : constant LAL.Classwide_Type_Decl := LAL.As_Classwide_Type_Decl (Node);
            begin
              NULL;
            end;

            This.Add_Not_Implemented;
         when Ada_Incomplete_Type_Decl =>
            declare
               Incomplete_Type_Decl_Node : constant LAL.Incomplete_Type_Decl := LAL.As_Incomplete_Type_Decl (Node);
               Discriminants : constant LAL.Discriminant_Part := LAL.F_Discriminants (Incomplete_Type_Decl_Node);
            begin
               if not Discriminants.Is_Null then
                 Log ("Discriminants: " & Discriminants.Image);
               end if;
            end;

            This.Add_Not_Implemented;
         when Ada_Incomplete_Tagged_Type_Decl =>
            declare
               Incomplete_Tagged_Type_Decl_Node : constant LAL.Incomplete_Tagged_Type_Decl := LAL.As_Incomplete_Tagged_Type_Decl (Node);
               Has_Abstract : constant Boolean := LAL.F_Has_Abstract (Incomplete_Tagged_Type_Decl_Node);
            begin
               Log ("Has_Abstract: " & Boolean'Image (Has_Abstract));
            end;

            This.Add_Not_Implemented;
         when Ada_Protected_Type_Decl =>
            declare
               Protected_Type_Decl_Node : constant LAL.Protected_Type_Decl := LAL.As_Protected_Type_Decl (Node);
               Discriminants : constant LAL.Discriminant_Part := LAL.F_Discriminants (Protected_Type_Decl_Node);
               Definition : constant LAL.Protected_Def := LAL.F_Definition (Protected_Type_Decl_Node);
            begin
               if not Discriminants.Is_Null then
                 Log ("Discriminants: " & Discriminants.Image);
               end if;
               if not Definition.Is_Null then
                 Log ("Definition: " & Definition.Image);
               end if;
            end;

            This.Add_Not_Implemented;
         when Ada_Task_Type_Decl =>
            declare
               Task_Type_Decl_Node : constant LAL.Task_Type_Decl := LAL.As_Task_Type_Decl (Node);
               Discriminants : constant LAL.Discriminant_Part := LAL.F_Discriminants (Task_Type_Decl_Node);
               Definition : constant LAL.Task_Def := LAL.F_Definition (Task_Type_Decl_Node);
            begin
               if not Discriminants.Is_Null then
                 Log ("Discriminants: " & Discriminants.Image);
               end if;
               if not Definition.Is_Null then
                 Log ("Definition: " & Definition.Image);
               end if;
            end;

            This.Add_Not_Implemented;
         when Ada_Single_Task_Type_Decl =>
            declare
               Single_Task_Type_Decl_Node : constant LAL.Single_Task_Type_Decl := LAL.As_Single_Task_Type_Decl (Node);
            begin
              NULL; 
            end;

         when Ada_Type_Decl =>
            declare
               Type_Decl_Node : constant LAL.Type_Decl := LAL.As_Type_Decl (Node);
               Discriminants : constant LAL.Discriminant_Part := LAL.F_Discriminants (Type_Decl_Node);
               typeDef : constant LAL.Type_Def := LAL.F_Type_Def (Type_Decl_Node);
            begin
               if not Discriminants.Is_Null then
                 Log ("Discriminants: " & Discriminants.Image);
               end if;
               Log ("typeDef: " & typeDef.Image);
            end;

            This.Add_Not_Implemented;
         when Ada_Anonymous_Type_Decl =>
            declare
               Anonymous_Type_Decl_Node : constant LAL.Anonymous_Type_Decl := LAL.As_Anonymous_Type_Decl (Node);
            begin
              NULL;
            end;

            This.Add_Not_Implemented;
         when Ada_Synth_Anonymous_Type_Decl =>
            declare
               Synth_Anonymous_Type_Decl_Node : constant LAL.Synth_Anonymous_Type_Decl := LAL.As_Synth_Anonymous_Type_Decl (Node);
            begin
              NULL;
            end;

         when Ada_Abstract_Subp_Decl =>
            declare
               Abstract_Subp_Decl_Node : constant LAL.Abstract_Subp_Decl := LAL.As_Abstract_Subp_Decl (Node);
            begin
              NULL; 
            end;

            This.Add_Not_Implemented;
         --when Ada_Formal_Subp_Decl =>
         --   This.Add_Not_Implemented;
         when Ada_Abstract_Formal_Subp_Decl =>
            declare
               Abstract_Formal_Subp_Decl_Node : constant LAL.Abstract_Formal_Subp_Decl := LAL.As_Abstract_Formal_Subp_Decl (Node);
            begin
              NULL; 
            end;

            This.Add_Not_Implemented;
         when Ada_Concrete_Formal_Subp_Decl =>
            declare
               Concrete_Formal_Subp_Decl_Node : constant LAL.Concrete_Formal_Subp_Decl := LAL.As_Concrete_Formal_Subp_Decl (Node);
            begin
              NULL; 
            end;

            This.Add_Not_Implemented;
         when Ada_Subp_Decl =>
            declare
               Subp_Decl_Node : constant LAL.Subp_Decl := LAL.As_Subp_Decl (Node);
            begin
              NULL; 
            end;

            This.Add_Not_Implemented;
         when Ada_Entry_Decl =>
            declare
               Entry_Decl_Node : constant LAL.Entry_Decl := LAL.As_Entry_Decl (Node);
               overridding : constant LAL.Overriding_Node := LAL.F_Overriding (Entry_Decl_Node);
               spec : constant LAL.Entry_Spec := LAL.F_Spec (Entry_Decl_Node);
            begin
               Log ("overridding: " & overridding.Image);
               Log ("spec: " & spec.Image);
            end;

            This.Add_Not_Implemented;
         when Ada_Enum_Literal_Decl =>
            declare
               Enum_Literal_Decl_Node : constant LAL.Enum_Literal_Decl := LAL.As_Enum_Literal_Decl (Node);
               name : constant LAL.Defining_Name := LAL.F_Name (Enum_Literal_Decl_Node);
               enumType : constant LAL.Type_Decl := LAL.P_Enum_Type (Enum_Literal_Decl_Node);
            begin
               Log ("name: " & name.Image);
               Log ("enumType: " & enumType.Image);
            end;

            This.Add_Not_Implemented;
         when Ada_Generic_Subp_Internal =>
            declare
               Generic_Subp_Internal_Node : constant LAL.Generic_Subp_Internal := LAL.As_Generic_Subp_Internal (Node);
               subpSpec : constant LAL.Subp_Spec := LAL.F_Subp_Spec (Generic_Subp_Internal_Node);
            begin
               Log ("subpSpec: " & subpSpec.Image);
            end;

            This.Add_Not_Implemented;
         --when Ada_Body_Node =>
         --   This.Add_Not_Implemented;
         --when Ada_Base_Subp_Body =>
         --   This.Add_Not_Implemented;
         when Ada_Expr_Function =>
            declare
               Expr_Function_Node : constant LAL.Expr_Function := LAL.As_Expr_Function (Node);
               expr : constant LAL.Expr := LAL.F_Expr (Expr_Function_Node);
            begin
               Log ("expr: " & expr.Image);
            end;

            This.Add_Not_Implemented;
         when Ada_Null_Subp_Decl =>
            declare
               Null_Subp_Decl_Node : constant LAL.Null_Subp_Decl := LAL.As_Null_Subp_Decl (Node);
            begin
              NULL; 
            end;

            This.Add_Not_Implemented;
         when Ada_Subp_Body =>
            declare
               Subp_Body_Node : constant LAL.Subp_Body := LAL.As_Subp_Body (Node);
               decl : constant LAL.Declarative_Part := LAL.F_Decls (Subp_Body_Node);
               stmt : constant LAL.Handled_Stmts := LAL.F_Stmts (Subp_Body_Node);
               endname : constant LAL.End_Name := LAL.F_End_Name (Subp_Body_Node);
            begin
               Log ("decl: " & decl.Image);
               Log ("stmt: " & stmt.Image);
               if not endname.Is_Null then
                 Log ("endname: " & endname.Image);
               end if;
            end;

            This.Add_Not_Implemented;
         when Ada_Subp_Renaming_Decl =>
            declare
               Subp_Renaming_Decl_Node : constant LAL.Subp_Renaming_Decl := LAL.As_Subp_Renaming_Decl (Node);
               rename : constant LAL.Renaming_Clause := LAL.F_Renames (Subp_Renaming_Decl_Node);
            begin
               Log ("rename: " & rename.Image);
            end;

            This.Add_Not_Implemented;
         --when Ada_Body_Stub =>
         --   This.Add_Not_Implemented;
         when Ada_Package_Body_Stub =>
            declare
               Package_Body_Stub_Node : constant LAL.Package_Body_Stub := LAL.As_Package_Body_Stub (Node);
               name : constant LAL.Defining_Name := LAL.F_Name (Package_Body_Stub_Node);
            begin
               Log ("name: " & name.Image);
            end;

            This.Add_Not_Implemented;
         when Ada_Protected_Body_Stub =>
            declare
               Protected_Body_Stub_Node : constant LAL.Protected_Body_Stub := LAL.As_Protected_Body_Stub (Node);
               name : constant LAL.Defining_Name := LAL.F_Name (Protected_Body_Stub_Node);
            begin
               Log ("name: " & name.Image);
            end;

            This.Add_Not_Implemented;
         when Ada_Subp_Body_Stub =>
            declare
               Subp_Body_Stub_Node : constant LAL.Subp_Body_Stub := LAL.As_Subp_Body_Stub (Node);
               overridding : constant LAL.Overriding_Node := LAL.F_Overriding (Subp_Body_Stub_Node);
               subSpec : constant LAL.Subp_Spec := LAL.F_Subp_Spec (Subp_Body_Stub_Node);
            begin
               Log ("overridding: " & overridding.Image);
               Log ("subSpec: " & subSpec.Image);
            end;

            This.Add_Not_Implemented;
         when Ada_Task_Body_Stub =>
            declare
               Task_Body_Stub_Node : constant LAL.Task_Body_Stub := LAL.As_Task_Body_Stub (Node);
               name : constant LAL.Defining_Name := LAL.F_Name (Task_Body_Stub_Node);
            begin
               Log ("name: " & name.Image);
            end;

            This.Add_Not_Implemented;
         when Ada_Entry_Body =>
            declare
               Entry_Body_Node : constant LAL.Entry_Body := LAL.As_Entry_Body (Node);
               params : constant LAL.Entry_Completion_Formal_Params := LAL.F_Params (Entry_Body_Node);
               barrier : constant LAL.Expr := LAL.F_Barrier (Entry_Body_Node);
               decls : constant LAL.Declarative_Part := LAL.F_Decls (Entry_Body_Node);
               stmts : constant LAL.Handled_Stmts := LAL.F_Stmts (Entry_Body_Node);
               endname : constant LAL.End_Name := LAL.F_End_Name (Entry_Body_Node);
            begin
               Log ("params: " & params.Image);
               Log ("barrier: " & barrier.Image);
               Log ("decls: " & decls.Image);
               Log ("stmts: " & stmts.Image);
               if not endname.Is_Null then
                 Log ("endname: " & endname.Image);
               end if;
            end;

            This.Add_Not_Implemented;
         when Ada_Package_Body =>
            declare
               Package_Body_Node : constant LAL.Package_Body := LAL.As_Package_Body (Node);
               name : constant LAL.Defining_Name := LAL.F_Package_Name (Package_Body_Node);
               decls : constant LAL.Declarative_Part := LAL.F_Decls (Package_Body_Node);
               stmts : constant LAL.Handled_Stmts := LAL.F_Stmts (Package_Body_Node);
               endname : constant LAL.End_Name := LAL.F_End_Name (Package_Body_Node);
            begin
               Log ("name: " & name.Image);
               Log ("decls: " & decls.Image);
               if not stmts.Is_Null then
                 Log ("stmts: " & stmts.Image);
               end if;
               if not endname.Is_Null then
                 Log ("endname: " & endname.Image);
               end if;
            end;

            This.Add_Not_Implemented;
         when Ada_Protected_Body =>
            declare
               Protected_Body_Node : constant LAL.Protected_Body := LAL.As_Protected_Body (Node);
               name : constant LAL.Defining_Name := LAL.F_Name (Protected_Body_Node);
               decls : constant LAL.Declarative_Part := LAL.F_Decls (Protected_Body_Node);
               endname : constant LAL.End_Name := LAL.F_End_Name (Protected_Body_Node);
            begin
               Log ("name: " & name.Image);
               Log ("decls: " & decls.Image);
               Log ("endname: " & endname.Image);
            end;

            This.Add_Not_Implemented;
         when Ada_Task_Body =>
            declare
               Task_Body_Node : constant LAL.Task_Body := LAL.As_Task_Body (Node);
               name : constant LAL.Defining_Name := LAL.F_Name (Task_Body_Node);
               decls : constant LAL.Declarative_Part := LAL.F_Decls (Task_Body_Node);
               stmts : constant LAL.Declarative_Part := LAL.F_Decls (Task_Body_Node);
               endname : constant LAL.End_Name := LAL.F_End_Name (Task_Body_Node);
            begin
               Log ("name: " & name.Image);
               Log ("decls: " & decls.Image);
               Log ("stmts: " & stmts.Image);
               Log ("endname: " & endname.Image);
            end;

            This.Add_Not_Implemented;
         when Ada_Entry_Index_Spec =>
            declare
               Entry_Index_Spec_Node : constant LAL.Entry_Index_Spec := LAL.As_Entry_Index_Spec (Node);
               id : constant LAL.Defining_Name := LAL.F_Id (Entry_Index_Spec_Node);
               sub_type : constant LAL.Ada_Node := LAL.F_Subtype (Entry_Index_Spec_Node);
            begin
               Log ("id: " & id.Image);
               Log ("sub_type: " & sub_type.Image);
            end;

            This.Add_Not_Implemented;
         when Ada_Error_Decl =>
            declare
               Error_Decl_Node : constant LAL.Error_Decl := LAL.As_Error_Decl (Node);
            begin
               NULL;
            end;

            This.Add_Not_Implemented;
         when Ada_Exception_Decl =>
            declare
               Exception_Decl_Node : constant LAL.Exception_Decl := LAL.As_Exception_Decl (Node);
               ids : constant LAL.Defining_Name_List := LAL.F_Ids (Exception_Decl_Node);
               rename : constant LAL.Renaming_Clause := LAL.F_Renames (Exception_Decl_Node);
            begin
               Log ("ids: " & ids.Image);
               if not rename.Is_Null then
                 Log ("rename: " & rename.Image);
               end if;
            end;

            This.Add_Not_Implemented;
         when Ada_Exception_Handler =>
            declare
               Exception_Handler_Node : constant LAL.Exception_Handler := LAL.As_Exception_Handler (Node);
               exceptionName : constant LAL.Defining_Name := LAL.F_Exception_Name (Exception_Handler_Node);
               handledExceptions : constant LAL.Alternatives_List := LAL.F_Handled_Exceptions (Exception_Handler_Node);
               stmts : constant LAL.Stmt_List := LAL.F_Stmts (Exception_Handler_Node);
            begin
               if not exceptionName.Is_Null then
                 Log ("exceptionName: " & exceptionName.Image);
               end if;
               Log ("handledExceptions: " & handledExceptions.Image);
               Log ("stmts: " & stmts.Image);
            end;

            This.Add_Not_Implemented;
         when Ada_For_Loop_Var_Decl =>
            declare
               For_Loop_Var_Decl_Node : constant LAL.For_Loop_Var_Decl := LAL.As_For_Loop_Var_Decl (Node);
               id : constant LAL.Defining_Name := LAL.F_Id (For_Loop_Var_Decl_Node);
               idType : constant LAL.Subtype_Indication := LAL.F_Id_Type (For_Loop_Var_Decl_Node);
            begin
               Log ("id: " & id.Image);
               if not idType.Is_Null then
                 Log ("idType: " & idType.Image);
               end if;
            end;

            This.Add_Not_Implemented;
         --when Ada_Generic_Decl =>
         --   This.Add_Not_Implemented;
         when Ada_Generic_Package_Decl =>
            declare
               Generic_Package_Decl_Node : constant LAL.Generic_Package_Decl := LAL.As_Generic_Package_Decl (Node);
               packageDecl : constant LAL.Generic_Package_Internal := LAL.F_Package_Decl (Generic_Package_Decl_Node);
               bodyPart : constant LAL.Package_Body := LAL.P_Body_Part (Generic_Package_Decl_Node);
            begin
               Log ("packageDecl: " & packageDecl.Image);
               if not bodyPart.Is_Null then
                 Log ("bodyPart: " & bodyPart.Image);
               end if;
            end;

            This.Add_Not_Implemented;
         when Ada_Generic_Subp_Decl =>
            declare
               Generic_Subp_Decl_Node : constant LAL.Generic_Subp_Decl := LAL.As_Generic_Subp_Decl (Node);
               subpDecl : constant LAL.Generic_Subp_Internal := LAL.F_Subp_Decl (Generic_Subp_Decl_Node);
--               bodyPart : constant LAL.Base_Subp_Body := LAL.P_Body_Part (Generic_Subp_Decl_Node);
            begin
               Log ("subpDecl: " & subpDecl.Image);
--               Log ("bodyPart: " & bodyPart.Image);
            end;

            This.Add_Not_Implemented;
         --when Ada_Generic_Instantiation =>
         --   This.Add_Not_Implemented;
         when Ada_Generic_Package_Instantiation =>
            declare
               Generic_Package_Instantiation_Node : constant LAL.Generic_Package_Instantiation := LAL.As_Generic_Package_Instantiation (Node);
               name : constant LAL.Defining_Name := LAL.F_Name (Generic_Package_Instantiation_Node);
               gericPackageName : constant LAL.Name := LAL.F_Generic_Pkg_Name (Generic_Package_Instantiation_Node);
               params : constant LAL.Assoc_List := LAL.F_Params (Generic_Package_Instantiation_Node);
            begin
               Log ("name: " & name.Image);
               Log ("gericPackageName: " & gericPackageName.Image);
               Log ("params: " & params.Image);
            end;

            This.Add_Not_Implemented;
         when Ada_Generic_Subp_Instantiation =>
            declare
               Generic_Subp_Instantiation_Node : constant LAL.Generic_Subp_Instantiation := LAL.As_Generic_Subp_Instantiation (Node);
               --kind : constant Ada_Subp_Kind := LAL.F_Kind (Generic_Subp_Instantiation_Node);
               subpName : constant LAL.Defining_Name := LAL.F_Subp_Name (Generic_Subp_Instantiation_Node);
               genericSubpName : constant LAL.Name := LAL.F_Generic_Subp_Name (Generic_Subp_Instantiation_Node);
               params : constant LAL.Assoc_List := LAL.F_Params (Generic_Subp_Instantiation_Node);
               designatedSubp : constant LAL.Ada_Node := LAL.P_Designated_Subp (Generic_Subp_Instantiation_Node);
            begin
               --Log ("kind: " & kind.Image);
               Log ("subpName: " & subpName.Image);
               Log ("genericSubpName: " & genericSubpName.Image);
               Log ("params: " & params.Image);
               Log ("designatedSubp: " & designatedSubp.Image);
            end;

            This.Add_Not_Implemented;
         --when Ada_Generic_Renaming_Decl =>
         --   This.Add_Not_Implemented;
         when Ada_Generic_Package_Renaming_Decl =>
            declare
               Generic_Package_Renaming_Decl_Node : constant LAL.Generic_Package_Renaming_Decl := LAL.As_Generic_Package_Renaming_Decl (Node);
               name : constant LAL.Defining_Name := LAL.F_Name (Generic_Package_Renaming_Decl_Node);
               rename : constant LAL.Name := LAL.F_Renames (Generic_Package_Renaming_Decl_Node);
            begin
               Log ("name: " & name.Image);
               Log ("rename: " & rename.Image);
            end;

            This.Add_Not_Implemented;
         when Ada_Generic_Subp_Renaming_Decl =>
            declare
               Generic_Subp_Renaming_Decl_Node : constant LAL.Generic_Subp_Renaming_Decl := LAL.As_Generic_Subp_Renaming_Decl (Node);
               kind : constant LAL.Subp_Kind := LAL.F_Kind (Generic_Subp_Renaming_Decl_Node);
               name : constant LAL.Defining_Name := LAL.F_Name (Generic_Subp_Renaming_Decl_Node);
               rename : constant LAL.Name := LAL.F_Renames (Generic_Subp_Renaming_Decl_Node);
            begin
               --Log ("kind: " & kind.Image);
               Log ("kind: " & kind.Image);
               Log ("name: " & name.Image);
               Log ("rename: " & rename.Image);
            end;

            This.Add_Not_Implemented;
         when Ada_Label_Decl =>
            declare
               Label_Decl_Node : constant LAL.Label_Decl := LAL.As_Label_Decl (Node);
               name : constant LAL.Defining_Name := LAL.F_Name (Label_Decl_Node);
            begin
               --Log ("kind: " & kind.Image);
               Log ("name: " & name.Image);
            end;

            This.Add_Not_Implemented;
         when Ada_Named_Stmt_Decl =>
            declare
               Named_Stmt_Decl_Node : constant LAL.Named_Stmt_Decl := LAL.As_Named_Stmt_Decl (Node);
               name : constant LAL.Defining_Name := LAL.F_Name (Named_Stmt_Decl_Node);
            begin
               --Log ("kind: " & kind.Image);
               Log ("name: " & name.Image);
            end;

            This.Add_Not_Implemented;
         when Ada_Number_Decl =>
            declare
               Number_Decl_Node : constant LAL.Number_Decl := LAL.As_Number_Decl (Node);
               ids : constant LAL.Defining_Name_List := LAL.F_Ids (Number_Decl_Node);
               expr : constant LAL.Expr := LAL.F_Expr (Number_Decl_Node);
            begin
               --Log ("kind: " & kind.Image);
               Log ("ids: " & ids.Image);
               Log ("expr: " & expr.Image);
            end;

            This.Add_Not_Implemented;
         when Ada_Object_Decl =>
            declare
               Object_Decl_Node : constant LAL.Object_Decl := LAL.As_Object_Decl (Node);
               FIds         : constant LAL.Defining_Name_List := LAL.F_Ids (Object_Decl_Node);
               Has_Aliased       : constant Boolean := LAL.F_Has_Aliased (Object_Decl_Node);
               Has_Constant       : constant Boolean := LAL.F_Has_Constant (Object_Decl_Node);
               mode         : constant LAL.Mode := LAL.F_Mode (Object_Decl_Node);
               typeExpr         : constant LAL.Type_Expr := LAL.F_Type_Expr (Object_Decl_Node);
               defaultExpr         : constant LAL.Expr := LAL.F_Default_Expr (Object_Decl_Node);
               renamingClause         : constant LAL.Renaming_Clause := LAL.F_Renaming_Clause (Object_Decl_Node);
               publicPartDecl         : constant LAL.Basic_Decl := LAL.P_Public_Part_Decl (Object_Decl_Node);
            begin
               Log ("FIds: " & FIds.Image);
               Log ("Has_Aliased: " & Boolean'Image (Has_Constant));
               Log ("F_Has_Constant: " & Boolean'Image (Has_Constant));
               Log ("mode: " & mode.Image);
               if not typeExpr.Is_Null then
                 Log ("typeExpr: " & typeExpr.Image);
               end if;
               if not defaultExpr.Is_Null then
                 Log ("defaultExpr: " & defaultExpr.Image);
               end if;
               if not renamingClause.Is_Null then
                 Log ("renamingClause: " & renamingClause.Image);
               end if;
               if not publicPartDecl.Is_Null then
                 Log ("publicPartDecl: " & publicPartDecl.Image);
               end if;
            end;

            This.Add_Not_Implemented;
         when Ada_Extended_Return_Stmt_Object_Decl =>
            declare
               Extended_Return_Stmt_Object_Decl_Node : constant LAL.Extended_Return_Stmt_Object_Decl := LAL.As_Extended_Return_Stmt_Object_Decl (Node);
            begin
              NULL;
            end;

            This.Add_Not_Implemented;
         when Ada_Package_Renaming_Decl =>
            declare
               Package_Renaming_Decl_Node : constant LAL.Package_Renaming_Decl := LAL.As_Package_Renaming_Decl (Node);
               name         : constant LAL.Defining_Name := LAL.F_Name (Package_Renaming_Decl_Node);
               rename         : constant LAL.Renaming_Clause := LAL.F_Renames (Package_Renaming_Decl_Node);
               renamedPackage         : constant LAL.Basic_Decl := LAL.P_Renamed_Package (Package_Renaming_Decl_Node);
               finalRenamedPackage         : constant LAL.Basic_Decl := LAL.P_Final_Renamed_Package (Package_Renaming_Decl_Node);
            begin
               Log ("name: " & name.Image);
               Log ("rename: " & rename.Image);
               Log ("renamedPackage: " & renamedPackage.Image);
               Log ("finalRenamedPackage: " & finalRenamedPackage.Image);
            end;

            This.Add_Not_Implemented;
         when Ada_Single_Protected_Decl =>
            declare
               Single_Protected_Decl_Node : constant LAL.Single_Protected_Decl := LAL.As_Single_Protected_Decl (Node);
               name         : constant LAL.Defining_Name := LAL.F_Name (Single_Protected_Decl_Node);
               interfaces         : constant LAL.Parent_List := LAL.F_Interfaces (Single_Protected_Decl_Node);
               definition         : constant LAL.Protected_Def := LAL.F_Definition (Single_Protected_Decl_Node);
            begin
               Log ("name: " & name.Image);
               Log ("interfaces: " & interfaces.Image);
               Log ("definition: " & definition.Image);
            end;

            This.Add_Not_Implemented;
         when Ada_Single_Task_Decl =>
            declare
               Single_Task_Decl_Node : constant LAL.Single_Task_Decl := LAL.As_Single_Task_Decl (Node);
               taskType         : constant LAL.Single_Task_Type_Decl := LAL.F_Task_Type (Single_Task_Decl_Node);
            begin
               Log ("taskType: " & taskType.Image);
            end;

            This.Add_Not_Implemented;
      end case;
   end Process_Ada_Basic_Decl;

   procedure Process_Ada_Case_Stmt_Alternative_Range
     (This    : in out Class;
      --  Node    : in     LAL.Stmt'Class;
      Node    : in     LAL.Ada_Node'Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process_Case_Stmt_Alternative_Range";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      --  Auto : Logging.Auto_Logger; -- Logs BEGIN and END

      -- Will raise Constraint_Error if Node.Kind is not in Ada_Stmt:
      Kind             : constant LALCO.Ada_Case_Stmt_Alternative_Range := Node.Kind;
      use LALCO; -- For subtype names in case stmt
   begin -- Process_Ada_Case_Stmt_Alternative_Range
      case Kind is

         when Ada_Case_Stmt_Alternative =>
            declare
               Case_Stmt_Alternative_Node : constant LAL.Case_Stmt_Alternative := LAL.As_Case_Stmt_Alternative (Node);
               choices         : constant LAL.Alternatives_List := LAL.F_Choices (Case_Stmt_Alternative_Node);
               stmts         : constant LAL.Stmt_List := LAL.F_Stmts (Case_Stmt_Alternative_Node);
            begin
               Log ("choices: " & choices.Image);
               Log ("stmts: " & stmts.Image);
            end;

      end case;
   end Process_Ada_Case_Stmt_Alternative_Range;

   procedure Process_Ada_Compilation_Unit_Range
     (This    : in out Class;
      --  Node    : in     LAL.Stmt'Class;
      Node    : in     LAL.Ada_Node'Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process_Compilation_Unit_Range";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      --  Auto : Logging.Auto_Logger; -- Logs BEGIN and END

      -- Will raise Constraint_Error if Node.Kind is not in Ada_Stmt:
      Kind             : constant LALCO.Ada_Compilation_Unit_Range := Node.Kind;
      use LALCO; -- For subtype names in case stmt
   begin -- Process_Ada_Compilation_Unit_Range
      case Kind is

         when Ada_Compilation_Unit =>
            declare
               Compilation_Unit_Node : constant LAL.Compilation_Unit := LAL.As_Compilation_Unit (Node);
               prelude         : constant LAL.Ada_Node_List := LAL.F_Prelude (Compilation_Unit_Node);
               bodyunit         : constant LAL.Ada_Node := LAL.F_Body (Compilation_Unit_Node);
               pragmas         : constant LAL.Pragma_Node_List := LAL.F_Pragmas (Compilation_Unit_Node);
               syntaticQualifiedName         : constant LAL.Unbounded_Text_Type_Array := LAL.P_Syntactic_Fully_Qualified_Name (Compilation_Unit_Node);
               unitKind         : constant LALCO.Analysis_Unit_Kind := LAL.P_Unit_Kind (Compilation_Unit_Node);
            begin
               Log ("prelude: " & prelude.Image);
               Log ("bodyunit: " & bodyunit.Image);
               Log ("pragmas: " & pragmas.Image);
               -- Log ("syntaticQualifiedName: " & syntaticQualifiedName.Image);
               -- Log ("unitKind: " & unitKind.Image);
            end;

      end case;
   end Process_Ada_Compilation_Unit_Range;

   procedure Process_Ada_Component_Clause_Range
     (This    : in out Class;
      --  Node    : in     LAL.Stmt'Class;
      Node    : in     LAL.Ada_Node'Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process_Component_Clause_Range";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      --  Auto : Logging.Auto_Logger; -- Logs BEGIN and END

      -- Will raise Constraint_Error if Node.Kind is not in Ada_Stmt:
      Kind             : constant LALCO.Ada_Component_Clause_Range := Node.Kind;
      use LALCO; -- For subtype names in case stmt
   begin -- Process_Ada_Component_Clause_Range
      case Kind is

         when Ada_Component_Clause =>
            declare
               Component_Clause_Node : constant LAL.Component_Clause := LAL.As_Component_Clause (Node);
               id         : constant LAL.Identifier := LAL.F_Id (Component_Clause_Node);
               position         : constant LAL.Expr := LAL.F_Position (Component_Clause_Node);
               ranges         : constant LAL.Range_Spec := LAL.F_Range (Component_Clause_Node);
            begin
               Log ("id: " & id.Image);
               Log ("position: " & position.Image);
               Log ("ranges: " & ranges.Image);
            end;

      end case;
   end Process_Ada_Component_Clause_Range;

   procedure Process_Ada_Component_Def_Range
     (This    : in out Class;
      --  Node    : in     LAL.Stmt'Class;
      Node    : in     LAL.Ada_Node'Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process_Component_Def_Range";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      --  Auto : Logging.Auto_Logger; -- Logs BEGIN and END

      -- Will raise Constraint_Error if Node.Kind is not in Ada_Stmt:
      Kind             : constant LALCO.Ada_Component_Def_Range := Node.Kind;
      use LALCO; -- For subtype names in case stmt
   begin -- Process_Ada_Component_Def_Range
      case Kind is

         when Ada_Component_Def =>
            declare
               Component_Def_Node : constant LAL.Component_Def := LAL.As_Component_Def (Node);
               Has_Aliased         : constant Boolean := LAL.F_Has_Aliased (Component_Def_Node);
               Has_Constant         : constant Boolean := LAL.F_Has_Constant (Component_Def_Node);
            begin
               Log ("Has_Aliased: " & Boolean'Image (Has_Aliased));
               Log ("Has_Constant: " & Boolean'Image (Has_Constant));
            end;

      end case;
   end Process_Ada_Component_Def_Range;

   procedure Process_Ada_Constraint
     (This    : in out Class;
      --  Node    : in     LAL.Stmt'Class;
      Node    : in     LAL.Ada_Node'Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process_Constraint";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      --  Auto : Logging.Auto_Logger; -- Logs BEGIN and END

      -- Will raise Constraint_Error if Node.Kind is not in Ada_Stmt:
      Kind             : constant LALCO.Ada_Constraint := Node.Kind;
      use LALCO; -- For subtype names in case stmt
   begin -- Process_Ada_Constraint
      case Kind is

         when Ada_Delta_Constraint =>
            declare
               Delta_Constraint_Node : constant LAL.Delta_Constraint := LAL.As_Delta_Constraint (Node);
               Digit         : constant LAL.Expr := LAL.F_Digits (Delta_Constraint_Node);
               ranges         : constant LAL.Range_Spec := LAL.F_Range (Delta_Constraint_Node);
            begin
               Log ("Digit: " & Digit.Image);
               Log ("ranges: " & ranges.Image);
            end;

         when Ada_Digits_Constraint =>
            declare
               Digits_Constraint_Node : constant LAL.Digits_Constraint := LAL.As_Digits_Constraint (Node);
               Digit         : constant LAL.Expr := LAL.F_Digits (Digits_Constraint_Node);
               ranges         : constant LAL.Range_Spec := LAL.F_Range (Digits_Constraint_Node);
            begin
               Log ("Digit: " & Digit.Image);
               Log ("ranges: " & ranges.Image);
            end;

         when Ada_Discriminant_Constraint =>
            declare
               Discriminant_Constraint_Node : constant LAL.Discriminant_Constraint := LAL.As_Discriminant_Constraint (Node);
               constraints         : constant LAL.Assoc_List := LAL.F_Constraints (Discriminant_Constraint_Node);
            begin
               Log ("constraints: " & constraints.Image);
            end;

         when Ada_Index_Constraint =>
            declare
               Index_Constraint_Node : constant LAL.Index_Constraint := LAL.As_Index_Constraint (Node);
               constraints         : constant LAL.Constraint_List := LAL.F_Constraints (Index_Constraint_Node);
            begin
               Log ("constraints: " & constraints.Image);
            end;

         when Ada_Range_Constraint =>
            declare
               Range_Constraint_Node : constant LAL.Range_Constraint := LAL.As_Range_Constraint (Node);
               ranges         : constant LAL.Range_Spec := LAL.F_Range (Range_Constraint_Node);
            begin
               Log ("Range: " & ranges.Image);
            end;

      end case;
   end Process_Ada_Constraint;

   procedure Process_Ada_Constant_Node
     (This    : in out Class;
      --  Node    : in     LAL.Stmt'Class;
      Node    : in     LAL.Ada_Node'Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process_Constant_Node";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      --  Auto : Logging.Auto_Logger; -- Logs BEGIN and END

      -- Will raise Constant_Node_Error if Node.Kind is not in Ada_Stmt:
      Kind             : constant LALCO.Ada_Constant_Node := Node.Kind;
      use LALCO; -- For subtype names in case stmt
   begin -- Process_Ada_Constant_Node
      case Kind is

         when Ada_Constant_Absent =>
            declare
               Constant_Absent_Node : constant LAL.Constant_Absent := LAL.As_Constant_Absent (Node);
            begin
               NULL; 
            end;

         when Ada_Constant_Present =>
            declare
               Constant_Present_Node : constant LAL.Constant_Present := LAL.As_Constant_Present (Node);
            begin
               NULL; 
            end;

      end case;
   end Process_Ada_Constant_Node;

   procedure Process_Ada_Declarative_Part_Range
     (This    : in out Class;
      --  Node    : in     LAL.Stmt'Class;
      Node    : in     LAL.Ada_Node'Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process_Declarative_Part_Range";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      --  Auto : Logging.Auto_Logger; -- Logs BEGIN and END

      -- Will raise Declarative_Part_Error if Node.Kind is not in Ada_Stmt:
      Kind             : constant LALCO.Ada_Declarative_Part_Range := Node.Kind;
      use LALCO; -- For subtype names in case stmt
   begin -- Process_Ada_Declarative_Part_Range
      case Kind is

         when Ada_Declarative_Part =>
            declare
               Declarative_Part_Node : constant LAL.Declarative_Part := LAL.As_Declarative_Part (Node);
               decls         : constant LAL.Ada_Node_List := LAL.F_Decls (Declarative_Part_Node);
            begin
               Log ("decls: " & decls.Image);
            end;

         when Ada_Private_Part =>
            declare
               Private_Part_Node : constant LAL.Private_Part := LAL.As_Private_Part (Node);
            begin
               NULL; 
            end;

         when Ada_Public_Part =>
            declare
               Public_Part_Node : constant LAL.Public_Part := LAL.As_Public_Part (Node);
            begin
               NULL; 
            end;

      end case;
   end Process_Ada_Declarative_Part_Range;

   procedure Process_Ada_Elsif_Expr_Part_Range
     (This    : in out Class;
      --  Node    : in     LAL.Stmt'Class;
      Node    : in     LAL.Ada_Node'Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process_Elsif_Expr_Part_Range";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      --  Auto : Logging.Auto_Logger; -- Logs BEGIN and END

      -- Will raise Declarative_Part_Error if Node.Kind is not in Ada_Stmt:
      Kind             : constant LALCO.Ada_Elsif_Expr_Part_Range := Node.Kind;
      use LALCO; -- For subtype names in case stmt
   begin -- Process_Ada_Elsif_Expr_Part_Range
      case Kind is

         when Ada_Elsif_Expr_Part =>
            declare
               Elsif_Expr_Part_Node : constant LAL.Elsif_Expr_Part := LAL.As_Elsif_Expr_Part (Node);
               Cond_Expr         : constant LAL.Expr := LAL.F_Cond_Expr (Elsif_Expr_Part_Node);
               Then_Expr         : constant LAL.Expr := LAL.F_Then_Expr (Elsif_Expr_Part_Node);
            begin
               Log ("Cond_Expr: " & Cond_Expr.Image);
               Log ("Then_Expr: " & Then_Expr.Image);
            end;

      end case;
   end Process_Ada_Elsif_Expr_Part_Range;

   procedure Process_Ada_Elsif_Stmt_Part_Range
     (This    : in out Class;
      --  Node    : in     LAL.Stmt'Class;
      Node    : in     LAL.Ada_Node'Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process_Elsif_Stmt_Part_Range";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      --  Auto : Logging.Auto_Logger; -- Logs BEGIN and END

      -- Will raise Declarative_Part_Error if Node.Kind is not in Ada_Stmt:
      Kind             : constant LALCO.Ada_Elsif_Stmt_Part_Range := Node.Kind;
      use LALCO; -- For subtype names in case stmt
   begin -- Process_Ada_Elsif_Stmt_Part_Range
      case Kind is

         when Ada_Elsif_Stmt_Part =>
            declare
               Elsif_Stmt_Part_Node : constant LAL.Elsif_Stmt_Part := LAL.As_Elsif_Stmt_Part (Node);
               Cond_Expr         : constant LAL.Expr := LAL.F_Cond_Expr (Elsif_Stmt_Part_Node);
               Stmts         : constant LAL.Stmt_List := LAL.F_Stmts (Elsif_Stmt_Part_Node);
            begin
               Log ("Cond_Expr: " & Cond_Expr.Image);
               Log ("Stmts: " & Stmts.Image);
            end;

      end case;
   end Process_Ada_Elsif_Stmt_Part_Range;

   procedure Process_Ada_Expr
     (This    : in out Class;
      --  Node    : in     LAL.Stmt'Class;
      Node    : in     LAL.Ada_Node'Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process_Ada_Expr";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      --  Auto : Logging.Auto_Logger; -- Logs BEGIN and END

      -- Will raise Constraint_Error if Node.Kind is not in Ada_Stmt:
      Kind             : constant LALCO.Ada_Expr := Node.Kind;
      use LALCO; -- For subtype names in case stmt
   begin -- Process_Ada_Expr
      case Kind is
         when Ada_Abstract_State_Decl_Expr =>
            This.Add_Not_Implemented;

         when Ada_Allocator =>
            declare
               Allocator_Node : constant LAL.Allocator := LAL.As_Allocator (Node);
               Subpool : constant LAL.Name := LAL.F_Subpool (Allocator_Node);
               Type_Or_Expr : constant LAL.Ada_Node := LAL.F_Type_Or_Expr (Allocator_Node);
               Get_Allocated_Type : constant LAL.Base_Type_Decl := LAL.P_Get_Allocated_Type (Allocator_Node);
            begin
               if not Subpool.Is_Null then
                 Log ("Subpool: " & Subpool.Image);
               end if;
               if not Type_Or_Expr.Is_Null then
                 Log ("Type_Or_Expr: " & Type_Or_Expr.Image);
               end if;
               if not Get_Allocated_Type.Is_Null then
                 Log ("Get_Allocated_Type: " & Get_Allocated_Type.Image);
               end if;
            end;

         when Ada_Aggregate =>
            declare
               Aggregate_Node : constant LAL.Aggregate := LAL.As_Aggregate (Node);
            begin
               NULL;
            end;
            
         when Ada_Bracket_Aggregate =>
            This.Add_Not_Implemented;

         when Ada_Delta_Aggregate =>
            This.Add_Not_Implemented; 

         when Ada_Bracket_Delta_Aggregate =>
            This.Add_Not_Implemented;
            
         when Ada_Null_Record_Aggregate =>
            declare
               Null_Record_Aggregate_Node : constant LAL.Null_Record_Aggregate := LAL.As_Null_Record_Aggregate (Node);
            begin
               NULL;
            end;

         when Ada_Bin_Op =>
            declare
               Bin_Op_Node : constant LAL.Bin_Op := LAL.As_Bin_Op (Node);
               Left : constant LAL.Expr := LAL.F_Left (Bin_Op_Node);
               Op : constant LAL.Op := LAL.F_Op (Bin_Op_Node);
               Right : constant LAL.Expr := LAL.F_Right (Bin_Op_Node);
            begin
               Log ("Left: " & Left.Image);
               Log ("Op: " & Op.Image);
               Log ("Right: " & Right.Image);
            end;

         when Ada_Relation_Op =>
            declare
               Relation_Op_Node : constant LAL.Relation_Op := LAL.As_Relation_Op (Node);
            begin
               NULL;
            end;

         when Ada_Box_Expr =>
            declare
               Box_Expr_Node : constant LAL.Box_Expr := LAL.As_Box_Expr (Node);
            begin
               NULL;
            end;

         when Ada_Case_Expr_Alternative =>
            declare
               Case_Expr_Alternative_Node : constant LAL.Case_Expr_Alternative := LAL.As_Case_Expr_Alternative (Node);
               choices : constant LAL.Alternatives_List := LAL.F_Choices (Case_Expr_Alternative_Node);
               expr : constant LAL.Expr := LAL.F_Expr (Case_Expr_Alternative_Node);
            begin
               Log ("choices: " & choices.Image);
               Log ("expr: " & expr.Image);
            end;

         when Ada_Case_Expr =>
            declare
               Case_Expr_Node : constant LAL.Case_Expr := LAL.As_Case_Expr (Node);
               Expr : constant LAL.Expr := LAL.F_Expr (Case_Expr_Node);
               Cases : constant LAL.Case_Expr_Alternative_List := LAL.F_Cases (Case_Expr_Node);
            begin
               Log ("Expr: " & Expr.Image);
               Log ("Cases: " & Cases.Image);
            end;

         when Ada_If_Expr =>
            declare
               If_Expr_Node : constant LAL.If_Expr := LAL.As_If_Expr (Node);
               Cond_Expr : constant LAL.Expr := LAL.F_Cond_Expr (If_Expr_Node);
               Then_Expr : constant LAL.Expr := LAL.F_Then_Expr (If_Expr_Node);
               Alternatives : constant LAL.Elsif_Expr_Part_List := LAL.F_Alternatives (If_Expr_Node);
               Else_Expr : constant LAL.Expr := LAL.F_Else_Expr (If_Expr_Node);
            begin
               Log ("Cond_Expr: " & Cond_Expr.Image);
               Log ("Then_Expr: " & Then_Expr.Image);
               Log ("Alternatives: " & Alternatives.Image);
               if not Else_Expr.Is_Null then
                 Log ("Else_Expr: " & Else_Expr.Image);
               end if;
            end;

         when Ada_Contract_Cases =>
            declare
               Contract_Cases_Node : constant LAL.Contract_Cases := LAL.As_Contract_Cases (Node);
               contract_cases : constant LAL.Contract_Case_Assoc_List := LAL.F_Contract_Cases (Contract_Cases_Node);
            begin
               Log ("contract_cases: " & contract_cases.Image);
            end;
            
         when Ada_Decl_Expr =>
            This.Add_Not_Implemented;
            
         when Ada_Membership_Expr =>
            declare
               Membership_Expr_Node : constant LAL.Membership_Expr := LAL.As_Membership_Expr (Node);
               Expr : constant LAL.Expr := LAL.F_Expr (Membership_Expr_Node);
               Op : constant LAL.Op := LAL.F_Op (Membership_Expr_Node);
               Membership_Exprs : constant LAL.Expr_Alternatives_List := LAL.F_Membership_Exprs (Membership_Expr_Node);
            begin
               Log ("Expr: " & Expr.Image);
               Log ("Op: " & Op.Image);
               Log ("Membership_Exprs: " & Membership_Exprs.Image);
            end;

         when Ada_Attribute_Ref =>
            declare
               Attribute_Ref_Node : constant LAL.Attribute_Ref := LAL.As_Attribute_Ref (Node);
               Prefix : constant LAL.Name := LAL.F_Prefix (Attribute_Ref_Node);
               Attribute : constant LAL.Identifier := LAL.F_Attribute (Attribute_Ref_Node);
               Args : constant LAL.Ada_Node := LAL.F_Args (Attribute_Ref_Node);
            begin
               Log ("Prefix: " & Prefix.Image);
               Log ("Attribute: " & Attribute.Image);
               if not Args.Is_Null then
                 Log ("Args: " & Args.Image);
               end if;
            end;

         when Ada_Update_Attribute_Ref =>
            declare
               Update_Attribute_Ref_Node : constant LAL.Update_Attribute_Ref := LAL.As_Update_Attribute_Ref (Node);
            begin
               NULL;
            end;

         when Ada_Call_Expr =>
            declare
               Call_Expr_Node : constant LAL.Call_Expr := LAL.As_Call_Expr (Node);
               Name : constant LAL.Name := LAL.F_Name (Call_Expr_Node);
               Suffix : constant LAL.Ada_Node := LAL.F_Suffix (Call_Expr_Node);
--               Is_Array_Slice : constant Boolean := LAL.P_Is_Array_Slice (Call_Expr_Node);
            begin
               Log ("Name: " & Name.Image);
               Log ("Suffix: " & Suffix.Image);
--               Log ("Is_Array_Slice: " & Boolean'Image(Is_Array_Slice));
            end;

         when Ada_Defining_Name =>
            declare
               Defining_Name_Node : constant LAL.Defining_Name := LAL.As_Defining_Name (Node);
            begin
               NULL;
            end;

         when Ada_Discrete_Subtype_Name =>
            declare
               Discrete_Subtype_Name_Node : constant LAL.Discrete_Subtype_Name := LAL.As_Discrete_Subtype_Name (Node);
               Sub_Type : constant LAL.Discrete_Subtype_Indication := LAL.F_Subtype (Discrete_Subtype_Name_Node);
            begin
               Log ("Sub_Type: " & Sub_Type.Image);
            end;

         when Ada_Dotted_Name =>
            declare
               Dotted_Name_Node : constant LAL.Dotted_Name := LAL.As_Dotted_Name (Node);
               Prefix : constant LAL.Name := LAL.F_Prefix (Dotted_Name_Node);
               Suffix : constant LAL.Base_Id := LAL.F_Suffix (Dotted_Name_Node);
            begin
               Log ("Prefix: " & Prefix.Image);
               Log ("Suffix: " & Suffix.Image);
            end;

         when Ada_End_Name =>
            declare
               End_Name_Node : constant LAL.End_Name := LAL.As_End_Name (Node);
               Name : constant LAL.Name := LAL.F_Name (End_Name_Node);
               Basic_Decl : constant LAL.Basic_Decl := LAL.P_Basic_Decl (End_Name_Node);
            begin
               Log ("Name: " & Name.Image);
               Log ("Basic_Decl: " & Basic_Decl.Image);
            end;

         when Ada_Explicit_Deref =>
            declare
               Explicit_Deref_Node : constant LAL.Explicit_Deref := LAL.As_Explicit_Deref (Node);
               Prefix : constant LAL.Name := LAL.F_Prefix (Explicit_Deref_Node);
            begin
               Log ("Prefix: " & Prefix.Image);
            end;

         when Ada_Qual_Expr =>
            declare
               Qual_Expr_Node : constant LAL.Qual_Expr := LAL.As_Qual_Expr (Node);
               Prefix : constant LAL.Name := LAL.F_Prefix (Qual_Expr_Node);
               Suffix : constant LAL.Expr := LAL.F_Suffix (Qual_Expr_Node);
            begin
               Log ("Prefix: " & Prefix.Image);
               Log ("Suffix: " & Suffix.Image);
            end;

         when Ada_Char_Literal =>
            declare
               Char_Literal_Node : constant LAL.Char_Literal := LAL.As_Char_Literal (Node);
               -- Denoted_Value : constant LALCO.Character_Type := LAL.P_Denoted_Value (Char_Literal_Node);
            begin
               -- Log ("Denoted_Value: " & Denoted_Value.Image);
               NULL;
            end;

         when Ada_Identifier =>
            declare
               Identifier_Node : constant LAL.Identifier := LAL.As_Identifier (Node);
            begin
               NULL;
            end;

         when Ada_Op_Abs =>
            declare
               Op_Abs_Node : constant LAL.Op_Abs := LAL.As_Op_Abs (Node);
            begin
               NULL;
            end;

         when Ada_Op_And =>
            declare
               Op_And_Node : constant LAL.Op_And := LAL.As_Op_And (Node);
            begin
               NULL;
            end;

         when Ada_Op_And_Then =>
            declare
               Op_And_Then_Node : constant LAL.Op_And_Then := LAL.As_Op_And_Then (Node);
            begin
               NULL;
            end;

         when Ada_Op_Concat =>
            declare
               Op_Concat_Node : constant LAL.Op_Concat := LAL.As_Op_Concat (Node);
            begin
               NULL;
            end;

         when Ada_Op_Div =>
            declare
               Op_Div_Node : constant LAL.Op_Div := LAL.As_Op_Div (Node);
            begin
               NULL;
            end;

         when Ada_Op_Double_Dot =>
            declare
               Op_Double_Dot_Node : constant LAL.Op_Double_Dot := LAL.As_Op_Double_Dot (Node);
            begin
               NULL;
            end;

         when Ada_Op_Eq =>
            declare
               Op_Eq_Node : constant LAL.Op_Eq := LAL.As_Op_Eq (Node);
            begin
               NULL;
            end;

         when Ada_Op_Gt =>
            declare
               Op_Gt_Node : constant LAL.Op_Gt := LAL.As_Op_Gt (Node);
            begin
               NULL;
            end;

         when Ada_Op_Gte =>
            declare
               Op_Gte_Node : constant LAL.Op_Gte := LAL.As_Op_Gte (Node);
            begin
               NULL;
            end;

         when Ada_Op_In =>
            declare
               Op_In_Node : constant LAL.Op_In := LAL.As_Op_In (Node);
            begin
               NULL;
            end;

         when Ada_Op_Lt =>
            declare
               Op_Lt_Node : constant LAL.Op_Lt := LAL.As_Op_Lt (Node);
            begin
               NULL;
            end;

         when Ada_Op_Lte =>
            declare
               Op_Lte_Node : constant LAL.Op_Lte := LAL.As_Op_Lte (Node);
            begin
               NULL;
            end;

         when Ada_Op_Minus =>
            declare
               Op_Minus_Node : constant LAL.Op_Minus := LAL.As_Op_Minus (Node);
            begin
               NULL;
            end;

         when Ada_Op_Mod =>
            declare
               Op_Mod_Node : constant LAL.Op_Mod := LAL.As_Op_Mod (Node);
            begin
               NULL;
            end;

         when Ada_Op_Mult =>
            declare
               Op_Mult_Node : constant LAL.Op_Mult := LAL.As_Op_Mult (Node);
            begin
               NULL;
            end;

         when Ada_Op_Neq =>
            declare
               Op_Neq_Node : constant LAL.Op_Neq := LAL.As_Op_Neq (Node);
            begin
               NULL;
            end;

         when Ada_Op_Not =>
            declare
               Op_Not_Node : constant LAL.Op_Not := LAL.As_Op_Not (Node);
            begin
               NULL;
            end;

         when Ada_Op_Not_In =>
            declare
               Op_Not_In_Node : constant LAL.Op_Not_In := LAL.As_Op_Not_In (Node);
            begin
               NULL;
            end;

         when Ada_Op_Or =>
            declare
               Op_Or_Node : constant LAL.Op_Or := LAL.As_Op_Or (Node);
            begin
               NULL;
            end;

         when Ada_Op_Or_Else =>
            declare
               Op_Or_Else_Node : constant LAL.Op_Or_Else := LAL.As_Op_Or_Else (Node);
            begin
               NULL;
            end;

         when Ada_Op_Plus =>
            declare
               Op_Plus_Node : constant LAL.Op_Plus := LAL.As_Op_Plus (Node);
            begin
               NULL;
            end;

         when Ada_Op_Pow =>
            declare
               Op_Pow_Node : constant LAL.Op_Pow := LAL.As_Op_Pow (Node);
            begin
               NULL;
            end;

         when Ada_Op_Rem =>
            declare
               Op_Rem_Node : constant LAL.Op_Rem := LAL.As_Op_Rem (Node);
            begin
               NULL;
            end;

         when Ada_Op_Xor =>
            declare
               Op_Xor_Node : constant LAL.Op_Xor := LAL.As_Op_Xor (Node);
            begin
               NULL;
            end;

         when Ada_String_Literal =>
            declare
               String_Literal_Node : constant LAL.String_Literal := LAL.As_String_Literal (Node);
               -- Denoted_Value : constant LALCO.Stringacter_Type := LAL.P_Denoted_Value (String_Literal_Node);
            begin
               -- Log ("Denoted_Value: " & Denoted_Value.Image);
               NULL;
            end;

         when Ada_Null_Literal =>
            declare
               Null_Literal_Node : constant LAL.Null_Literal := LAL.As_Null_Literal (Node);
            begin
               NULL;
            end;

         when Ada_Int_Literal =>
            declare
               Int_Literal_Node : constant LAL.Int_Literal := LAL.As_Int_Literal (Node);
               -- Denoted_Value : constant LALCO.Big_Integer := LAL.P_Denoted_Value (Int_Literal_Node);
            begin
               -- Log ("Denoted_Value: " & Denoted_Value.Image);
               NULL;
            end;

         when Ada_Real_Literal =>
            declare
               Real_Literal_Node : constant LAL.Real_Literal := LAL.As_Real_Literal (Node);
            begin
               NULL;
            end;

         when Ada_Target_Name =>
            declare
               Target_Name_Node : constant LAL.Target_Name := LAL.As_Target_Name (Node);
            begin
               NULL;
            end;

         when Ada_Paren_Expr =>
            declare
               Paren_Expr_Node : constant LAL.Paren_Expr := LAL.As_Paren_Expr (Node);
               Expr : constant LAL.Expr := LAL.F_Expr (Paren_Expr_Node);
            begin
               Log ("Expr: " & Expr.Image);
            end;

         when Ada_Quantified_Expr =>
            declare
               Quantified_Expr_Node : constant LAL.Quantified_Expr := LAL.As_Quantified_Expr (Node);
               Quantifier : constant LAL.Quantifier := LAL.F_Quantifier (Quantified_Expr_Node);
               Loop_Spec : constant LAL.For_Loop_Spec := LAL.F_Loop_Spec (Quantified_Expr_Node);
               Expr : constant LAL.Expr := LAL.F_Expr (Quantified_Expr_Node);
            begin
               Log ("Quantifier: " & Quantifier.Image);
               Log ("Loop_Spec: " & Loop_Spec.Image);
               Log ("Expr: " & Expr.Image);
            end;

         when Ada_Raise_Expr =>
            declare
               Raise_Expr_Node : constant LAL.Raise_Expr := LAL.As_Raise_Expr (Node);
               Exception_Name : constant LAL.Name := LAL.F_Exception_Name (Raise_Expr_Node);
               Error_Message : constant LAL.Expr := LAL.F_Error_Message (Raise_Expr_Node);
            begin
               Log ("Exception_Name: " & Exception_Name.Image);
               Log ("Error_Message: " & Error_Message.Image);
            end;

         when Ada_Un_Op =>
            declare
               Un_Op_Node : constant LAL.Un_Op := LAL.As_Un_Op (Node);
               Op : constant LAL.Op := LAL.F_Op (Un_Op_Node);
            begin
               Log ("Op: " & Op.Image);
            end;

      end case;
   end Process_Ada_Expr;


   procedure Process_Ada_Handled_Stmts_Range
     (This    : in out Class;
      --  Node    : in     LAL.Stmt'Class;
      Node    : in     LAL.Ada_Node'Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process_Handled_Stmts_Range";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      --  Auto : Logging.Auto_Logger; -- Logs BEGIN and END

      -- Will raise Declarative_Part_Error if Node.Kind is not in Ada_Stmt:
      Kind             : constant LALCO.Ada_Handled_Stmts_Range := Node.Kind;
      use LALCO; -- For subtype names in case stmt
   begin -- Process_Ada_Handled_Stmts_Range
      case Kind is

         when Ada_Handled_Stmts =>
            declare
               Handled_Stmts_Node : constant LAL.Handled_Stmts := LAL.As_Handled_Stmts (Node);
               Stmts : constant LAL.Stmt_List := LAL.F_Stmts (Handled_Stmts_Node);
               Exceptions : constant LAL.Ada_Node_List := LAL.F_Exceptions (Handled_Stmts_Node);
            begin
               Log ("Stmts: " & Stmts.Image);
               Log ("Exceptions: " & Exceptions.Image);
            end;

      end case;
   end Process_Ada_Handled_Stmts_Range;

   procedure process_ada_interface_kind
     (this    : in out class;
      --  node    : in     lal.stmt'class;
      node    : in     lal.ada_node'class)
   is
      parent_name : constant string := module_name;
      module_name : constant string := parent_name & ".process_Handled_Stmts_Range";
      package logging is new generic_logging (module_name); use logging;
      --  auto : logging.auto_logger; -- logs begin and end

      -- will raise declarative_part_error if node.kind is not in ada_stmt:
      kind             : constant lalco.ada_interface_kind := node.kind;
      use lalco; -- for subtype names in case stmt
   begin -- process_ada_interface_kind
      case kind is

         when ada_interface_kind_limited =>
            declare
               interface_kind_limited_Node : constant LAL.interface_kind_limited := LAL.As_interface_kind_limited (Node);
            begin
               NULL;
            end;

         when ada_interface_kind_protected =>
            declare
               interface_kind_protected_Node : constant LAL.interface_kind_protected := LAL.As_interface_kind_protected (Node);
            begin
               NULL;
            end;

         when ada_interface_kind_synchronized =>
            declare
               interface_kind_synchronized_Node : constant LAL.interface_kind_synchronized := LAL.As_interface_kind_synchronized (Node);
            begin
               NULL;
            end;

         when ada_interface_kind_task =>
            declare
               interface_kind_task_Node : constant LAL.interface_kind_task := LAL.As_interface_kind_task (Node);
            begin
               NULL;
            end;

      end case;
   end process_ada_interface_kind;

   procedure process_ada_Iter_Type
     (this    : in out class;
      --  node    : in     lal.stmt'class;
      node    : in     lal.ada_node'class)
   is
      parent_name : constant string := module_name;
      module_name : constant string := parent_name & ".process_Iter_Type";
      package logging is new generic_logging (module_name); use logging;
      --  auto : logging.auto_logger; -- logs begin and end

      -- will raise declarative_part_error if node.kind is not in ada_stmt:
      kind             : constant lalco.ada_Iter_Type := node.kind;
      use lalco; -- for subtype names in case stmt
   begin -- process_ada_Iter_Type
      case kind is

         when ada_Iter_Type_In =>
            declare
               Iter_Type_In_Node : constant LAL.Iter_Type_In := LAL.As_Iter_Type_In (Node);
            begin
               NULL;
            end;

         when ada_Iter_Type_Of =>
            declare
               Iter_Type_Of_Node : constant LAL.Iter_Type_Of := LAL.As_Iter_Type_Of (Node);
            begin
               NULL;
            end;

      end case;
   end process_ada_Iter_Type;

   procedure process_ada_Library_Item_Range
     (this    : in out class;
      --  node    : in     lal.stmt'class;
      node    : in     lal.ada_node'class)
   is
      parent_name : constant string := module_name;
      module_name : constant string := parent_name & ".process_Library_Item_Range";
      package logging is new generic_logging (module_name); use logging;
      --  auto : logging.auto_logger; -- logs begin and end

      -- will raise declarative_part_error if node.kind is not in ada_stmt:
      kind             : constant lalco.ada_Library_Item_Range := node.kind;
      use lalco; -- for subtype names in case stmt
   begin -- process_ada_Library_Item_Range
      case kind is

         when ada_Library_Item =>
            declare
               Library_Item_Node : constant LAL.Library_Item := LAL.As_Library_Item (Node);
               Has_Private       : constant Boolean := LAL.F_Has_Private (Library_Item_Node);
               item : constant LAL.Basic_Decl := LAL.F_Item (Library_Item_Node);
            begin
               Log ("Has_Private: " & Boolean'Image (Has_Private));
               Log ("item: " & item.Image);
            end;
            this.add_not_implemented;

      end case;
   end process_ada_Library_Item_Range;

   procedure process_ada_Limited_Node
     (this    : in out class;
      --  node    : in     lal.stmt'class;
      node    : in     lal.ada_node'class)
   is
      parent_name : constant string := module_name;
      module_name : constant string := parent_name & ".process_Library_Item_Range";
      package logging is new generic_logging (module_name); use logging;
      --  auto : logging.auto_logger; -- logs begin and end

      -- will raise declarative_part_error if node.kind is not in ada_stmt:
      kind             : constant lalco.ada_Limited_Node := node.kind;
      use lalco; -- for subtype names in case stmt
   begin -- process_ada_Limited_Node
      case kind is

         when ada_Limited_Absent =>
            declare
               Limited_Absent_Node : constant LAL.Limited_Absent := LAL.As_Limited_Absent (Node);
            begin
               NULL;
            end;
            this.add_not_implemented;

         when ada_Limited_Present =>
            declare
               Limited_Present_Node : constant LAL.Limited_Present := LAL.As_Limited_Present (Node);
            begin
               NULL;
            end;
            this.add_not_implemented;

      end case;
   end process_ada_Limited_Node;

   procedure process_ada_Loop_Spec
     (this    : in out class;
      --  node    : in     lal.stmt'class;
      node    : in     lal.ada_node'class)
   is
      parent_name : constant string := module_name;
      module_name : constant string := parent_name & ".process_Loop_Spec";
      package logging is new generic_logging (module_name); use logging;
      --  auto : logging.auto_logger; -- logs begin and end

      -- will raise declarative_part_error if node.kind is not in ada_stmt:
      kind             : constant lalco.ada_Loop_Spec := node.kind;
      use lalco; -- for subtype names in case stmt
   begin -- process_ada_Loop_Spec
      case kind is

         when ada_For_Loop_Spec =>
            declare
               For_Loop_Spec_Node : constant LAL.For_Loop_Spec := LAL.As_For_Loop_Spec (Node);
               Has_Reverse       : constant Boolean := LAL.F_Has_Reverse (For_Loop_Spec_Node);
               var_decl         : constant LAL.For_Loop_Var_Decl := LAL.F_Var_Decl (For_Loop_Spec_Node);
               loop_type         : constant LAL.Iter_Type := LAL.F_Loop_Type (For_Loop_Spec_Node);
            begin
               Log ("F_Has_Reverse: " & Boolean'Image (Has_Reverse));
               Log ("var_decl: " & var_decl.Image);
               Log ("loop_type: " & loop_type.Image);
            end;
            this.add_not_implemented;

         when ada_While_Loop_Spec =>
            declare
               While_Loop_Spec_Node : constant LAL.While_Loop_Spec := LAL.As_While_Loop_Spec (Node);
               expr         : constant LAL.Expr := LAL.F_Expr (While_Loop_Spec_Node);
            begin
               Log ("expr: " & expr.Image);
            end;
            this.add_not_implemented;

      end case;
   end process_ada_Loop_Spec;

   procedure process_ada_Mode
     (this    : in out class;
      --  node    : in     lal.stmt'class;
      node    : in     lal.ada_node'class)
   is
      parent_name : constant string := module_name;
      module_name : constant string := parent_name & ".process_Mode";
      package logging is new generic_logging (module_name); use logging;
      --  auto : logging.auto_logger; -- logs begin and end

      -- will raise declarative_part_error if node.kind is not in ada_stmt:
      kind             : constant lalco.ada_Mode := node.kind;
      use lalco; -- for subtype names in case stmt
   begin -- process_ada_Mode
      case kind is

         when ada_Mode_Default =>
            declare
               Mode_Default_Node : constant LAL.Mode_Default := LAL.As_Mode_Default (Node);
            begin
               NULL;
            end;
            this.add_not_implemented;

         when ada_Mode_In =>
            declare
               Mode_In_Node : constant LAL.Mode_In := LAL.As_Mode_In (Node);
            begin
               NULL;
            end;
            this.add_not_implemented;

         when ada_Mode_In_Out =>
            declare
               Mode_In_Out_Node : constant LAL.Mode_In_Out := LAL.As_Mode_In_Out (Node);
            begin
               NULL;
            end;
            this.add_not_implemented;

         when ada_Mode_Out =>
            declare
               Mode_Out_Node : constant LAL.Mode_Out := LAL.As_Mode_Out (Node);
            begin
               NULL;
            end;
            this.add_not_implemented;

      end case;
   end process_ada_Mode;

   procedure Process_Ada_Multi_Abstract_State_Decl_Range
     (This    : in out Class;
      --  Node    : in     LAL.Stmt'Class;
      Node    : in     LAL.Ada_Node'Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process_Ada_Multi_Abstract_State_Decl_Range";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      --  Auto : Logging.Auto_Logger; -- Logs BEGIN and END

      -- Will raise Constraint_Error if Node.Kind is not in Ada_Stmt:
      Kind : constant LALCO.Ada_Multi_Abstract_State_Decl_Range := Node.Kind;
      use LALCO; -- For subtype names in case stmt
   begin -- Process_Ada_Multi_Abstract_State_Decl_Range
      case Kind is

         when Ada_Multi_Abstract_State_Decl =>
            declare
               Multi_Abstract_State_Decl_Node : constant LAL.Multi_Abstract_State_Decl :=
                 LAL.As_Multi_Abstract_State_Decl (Node);
               F_Decls : constant LAL.Abstract_State_Decl_List := 
                 LAL.F_Decls (Multi_Abstract_State_Decl_Node);
            begin
               this.add_not_implemented;
            end;

      end case;
   end Process_Ada_Multi_Abstract_State_Decl_Range;

   procedure process_ada_Not_Null
     (this    : in out class;
      --  node    : in     lal.stmt'class;
      node    : in     lal.ada_node'class)
   is
      parent_name : constant string := module_name;
      module_name : constant string := parent_name & ".process_Not_Null";
      package logging is new generic_logging (module_name); use logging;
      --  auto : logging.auto_logger; -- logs begin and end

      -- will raise declarative_part_error if node.kind is not in ada_stmt:
      kind             : constant lalco.ada_Not_Null := node.kind;
      use lalco; -- for subtype names in case stmt
   begin -- process_ada_Not_Null
      case kind is

         when ada_Not_Null_Absent =>
            declare
               Not_Null_Absent_Node : constant LAL.Not_Null_Absent := LAL.As_Not_Null_Absent (Node);
            begin
               NULL;
            end;
            this.add_not_implemented;

         when ada_Not_Null_Present =>
            declare
               Not_Null_Present_Node : constant LAL.Not_Null_Present := LAL.As_Not_Null_Present (Node);
            begin
               NULL;
            end;
            this.add_not_implemented;

      end case;
   end process_ada_Not_Null;

   procedure process_ada_Null_Component_Decl_Range
     (this    : in out class;
      --  node    : in     lal.stmt'class;
      node    : in     lal.ada_node'class)
   is
      parent_name : constant string := module_name;
      module_name : constant string := parent_name & ".process_Null_Component_Decl_Range";
      package logging is new generic_logging (module_name); use logging;
      --  auto : logging.auto_logger; -- logs begin and end

      -- will raise declarative_part_error if node.kind is not in ada_stmt:
      kind             : constant lalco.ada_Null_Component_Decl_Range := node.kind;
      use lalco; -- for subtype names in case stmt
   begin -- process_ada_Null_Component_Decl_Range
      case kind is

         when ada_Null_Component_Decl =>
            declare
               Null_Component_Decl_Node : constant LAL.Null_Component_Decl := LAL.As_Null_Component_Decl (Node);
            begin
               NULL;
            end;
            this.add_not_implemented;

      end case;
   end process_ada_Null_Component_Decl_Range;

   procedure process_ada_Others_Designator_Range
     (this    : in out class;
      --  node    : in     lal.stmt'class;
      node    : in     lal.ada_node'class)
   is
      parent_name : constant string := module_name;
      module_name : constant string := parent_name & ".process_Others_Designator_Range";
      package logging is new generic_logging (module_name); use logging;
      --  auto : logging.auto_logger; -- logs begin and end

      -- will raise declarative_part_error if node.kind is not in ada_stmt:
      kind             : constant lalco.ada_Others_Designator_Range := node.kind;
      use lalco; -- for subtype names in case stmt
   begin -- process_ada_Others_Designator_Range
      case kind is

         when ada_Others_Designator =>
            declare
               Others_Designator_Node : constant LAL.Others_Designator := LAL.As_Others_Designator (Node);
            begin
               NULL;
            end;
            this.add_not_implemented;

      end case;
   end process_ada_Others_Designator_Range;

   procedure process_ada_Overriding_Node
     (this    : in out class;
      --  node    : in     lal.stmt'class;
      node    : in     lal.ada_node'class)
   is
      parent_name : constant string := module_name;
      module_name : constant string := parent_name & ".process_Overriding_Node";
      package logging is new generic_logging (module_name); use logging;
      --  auto : logging.auto_logger; -- logs begin and end

      -- will raise declarative_part_error if node.kind is not in ada_stmt:
      kind             : constant lalco.ada_Overriding_Node := node.kind;
      use lalco; -- for subtype names in case stmt
   begin -- process_ada_Overriding_Node
      case kind is

         when ada_Overriding_Not_Overriding =>
            declare
               Overriding_Not_Overriding_Node : constant LAL.Overriding_Not_Overriding := LAL.As_Overriding_Not_Overriding (Node);
            begin
               NULL;
            end;
            this.add_not_implemented;

         when ada_Overriding_Overriding =>
            declare
               Overriding_Overriding_Node : constant LAL.Overriding_Overriding := LAL.As_Overriding_Overriding (Node);
            begin
               NULL;
            end;
            this.add_not_implemented;

         when ada_Overriding_Unspecified =>
            declare
               Overriding_Unspecified_Node : constant LAL.Overriding_Unspecified := LAL.As_Overriding_Unspecified (Node);
            begin
               NULL;
            end;
            this.add_not_implemented;

      end case;
   end process_ada_Overriding_Node;

   procedure process_ada_Params_Range
     (this    : in out class;
      --  node    : in     lal.stmt'class;
      node    : in     lal.ada_node'class)
   is
      parent_name : constant string := module_name;
      module_name : constant string := parent_name & ".process_Params_Range";
      package logging is new generic_logging (module_name); use logging;
      --  auto : logging.auto_logger; -- logs begin and end

      -- will raise declarative_part_error if node.kind is not in ada_stmt:
      kind             : constant lalco.ada_Params_Range := node.kind;
      use lalco; -- for subtype names in case stmt
   begin -- process_ada_Params_Range
      case kind is

         when ada_Params =>
            declare
               Params_Node : constant LAL.Params := LAL.As_Params (Node);
               params : constant LAL.Param_Spec_List := LAL.F_Params (Params_Node);
            begin
               Log ("params: " & params.Image);
            end;
            this.add_not_implemented;

      end case;
   end process_ada_Params_Range;

   procedure Process_Ada_Paren_Abstract_State_Decl_Range
     (This    : in out Class;
      --  Node    : in     LAL.Stmt'Class;
      Node    : in     LAL.Ada_Node'Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process_Ada_Paren_Abstract_State_Decl_Range";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      --  Auto : Logging.Auto_Logger; -- Logs BEGIN and END

      -- Will raise Constraint_Error if Node.Kind is not in Ada_Stmt:
      Kind             : constant LALCO.Ada_Paren_Abstract_State_Decl_Range := Node.Kind;
      use LALCO; -- For subtype names in case stmt
   begin -- Process_Ada_Paren_Abstract_State_Decl_Range
      case Kind is

         when Ada_Paren_Abstract_State_Decl =>
            declare
               Paren_Abstract_State_Decl_Node : constant LAL.Paren_Abstract_State_Decl := LAL.As_Paren_Abstract_State_Decl (Node);
               F_Decl : constant LAL.Ada_Node := LAL.F_Decl (Paren_Abstract_State_Decl_Node);
            begin
               Log ("F_Decl: " & F_Decl.Image);
               this.add_not_implemented;
            end;

      end case;
   end Process_Ada_Paren_Abstract_State_Decl_Range;

   procedure process_ada_Pragma_Node_Range
     (this    : in out class;
      --  node    : in     lal.stmt'class;
      node    : in     lal.ada_node'class)
   is
      parent_name : constant string := module_name;
      module_name : constant string := parent_name & ".process_Pragma_Node_Range";
      package logging is new generic_logging (module_name); use logging;
      --  auto : logging.auto_logger; -- logs begin and end

      -- will raise declarative_part_error if node.kind is not in ada_stmt:
      kind             : constant lalco.ada_Pragma_Node_Range := node.kind;
      use lalco; -- for subtype names in case stmt
   begin -- process_ada_Pragma_Node_Range
      case kind is

         when ada_Pragma_Node =>
            declare
               Pragma_Node_Node : constant LAL.Pragma_Node := LAL.As_Pragma_Node (Node);
               id : constant LAL.Identifier := LAL.F_Id (Pragma_Node_Node);
               args : constant LAL.Base_Assoc_List := LAL.F_Args (Pragma_Node_Node);
--               associated_Decls : constant LAL.Basic_Decl_Array := LAL.P_Associated_Decls (Pragma_Node_Node);
            begin
               Log ("id: " & id.Image);
               Log ("args: " & args.Image);
--               Log ("associated_Decls: " & associated_Decls.Image);
            end;
            this.add_not_implemented;

      end case;
   end process_ada_Pragma_Node_Range;

   procedure process_ada_Private_Node
     (this    : in out class;
      --  node    : in     lal.stmt'class;
      node    : in     lal.ada_node'class)
   is
      parent_name : constant string := module_name;
      module_name : constant string := parent_name & ".process_Private_Node";
      package logging is new generic_logging (module_name); use logging;
      --  auto : logging.auto_logger; -- logs begin and end

      -- will raise declarative_part_error if node.kind is not in ada_stmt:
      kind             : constant lalco.ada_Private_Node := node.kind;
      use lalco; -- for subtype names in case stmt
   begin -- process_ada_Private_Node
      case kind is

         when ada_Private_Absent =>
            declare
               Private_Absent_Node : constant LAL.Private_Absent := LAL.As_Private_Absent (Node);
            begin
               NULL;
            end;
            this.add_not_implemented;

         when ada_Private_Present =>
            declare
               Private_Present_Node : constant LAL.Private_Present := LAL.As_Private_Present (Node);
            begin
               NULL;
            end;
            this.add_not_implemented;

      end case;
   end process_ada_Private_Node;

   procedure process_ada_Protected_Node
     (this    : in out class;
      --  node    : in     lal.stmt'class;
      node    : in     lal.ada_node'class)
   is
      parent_name : constant string := module_name;
      module_name : constant string := parent_name & ".process_Protected_Node";
      package logging is new generic_logging (module_name); use logging;
      --  auto : logging.auto_logger; -- logs begin and end

      -- will raise declarative_part_error if node.kind is not in ada_stmt:
      kind             : constant lalco.ada_Protected_Node := node.kind;
      use lalco; -- for subtype names in case stmt
   begin -- process_ada_Protected_Node
      case kind is

         when ada_Protected_Absent =>
            declare
               Protected_Absent_Node : constant LAL.Protected_Absent := LAL.As_Protected_Absent (Node);
            begin
               NULL;
            end;
            this.add_not_implemented;

         when ada_Protected_Present =>
            declare
               Protected_Present_Node : constant LAL.Protected_Present := LAL.As_Protected_Present (Node);
            begin
               NULL;
            end;
            this.add_not_implemented;

      end case;
   end process_ada_Protected_Node;

   procedure process_ada_Protected_Def_Range
     (this    : in out class;
      --  node    : in     lal.stmt'class;
      node    : in     lal.ada_node'class)
   is
      parent_name : constant string := module_name;
      module_name : constant string := parent_name & ".process_Protected_Def_Range";
      package logging is new generic_logging (module_name); use logging;
      --  auto : logging.auto_logger; -- logs begin and end

      -- will raise declarative_part_error if node.kind is not in ada_stmt:
      kind             : constant lalco.ada_Protected_Def_Range := node.kind;
      use lalco; -- for subtype names in case stmt
   begin -- process_ada_Protected_Def_Range
      case kind is

         when ada_Protected_Def =>
            declare
               Protected_Def_Node : constant LAL.Protected_Def := LAL.As_Protected_Def (Node);
               public_part : constant LAL.Public_Part := LAL.F_Public_Part (Protected_Def_Node);
               private_part : constant LAL.Private_Part := LAL.F_Private_Part (Protected_Def_Node);
               end_name : constant LAL.End_Name := LAL.F_End_Name (Protected_Def_Node);
            begin
               Log ("public_part: " & public_part.Image);
               if not private_part.Is_Null then
                 Log ("private_part: " & private_part.Image);
               end if;
               if not end_name.Is_Null then
                 Log ("end_name: " & end_name.Image);
               end if;
            end;
            this.add_not_implemented;

      end case;
   end process_ada_Protected_Def_Range;

   procedure process_ada_Quantifier
     (this    : in out class;
      --  node    : in     lal.stmt'class;
      node    : in     lal.ada_node'class)
   is
      parent_name : constant string := module_name;
      module_name : constant string := parent_name & ".process_Quantifier";
      package logging is new generic_logging (module_name); use logging;
      --  auto : logging.auto_logger; -- logs begin and end

      -- will raise declarative_part_error if node.kind is not in ada_stmt:
      kind             : constant lalco.ada_Quantifier := node.kind;
      use lalco; -- for subtype names in case stmt
   begin -- process_ada_Quantifier
      case kind is

         when ada_Quantifier_All =>
            declare
               Quantifier_All_Node : constant LAL.Quantifier_All := LAL.As_Quantifier_All (Node);
            begin
               NULL;
            end;
            this.add_not_implemented;

         when ada_Quantifier_Some =>
            declare
               Quantifier_Some_Node : constant LAL.Quantifier_Some := LAL.As_Quantifier_Some (Node);
            begin
               NULL;
            end;
            this.add_not_implemented;

      end case;
   end process_ada_Quantifier;

   procedure process_ada_Range_Spec_Range
     (this    : in out class;
      --  node    : in     lal.stmt'class;
      node    : in     lal.ada_node'class)
   is
      parent_name : constant string := module_name;
      module_name : constant string := parent_name & ".process_Range_Spec_Range";
      package logging is new generic_logging (module_name); use logging;
      --  auto : logging.auto_logger; -- logs begin and end

      -- will raise declarative_part_error if node.kind is not in ada_stmt:
      kind             : constant lalco.ada_Range_Spec_Range := node.kind;
      use lalco; -- for subtype names in case stmt
   begin -- process_ada_Range_Spec_Range
      case kind is

         when ada_Range_Spec =>
            declare
               Range_Spec_Node : constant LAL.Range_Spec := LAL.As_Range_Spec (Node);
               F_Range : constant LAL.Expr := LAL.F_Range (Range_Spec_Node);
            begin
               Log ("F_Range: " & F_Range.Image);
            end;
            this.add_not_implemented;

      end case;
   end process_ada_Range_Spec_Range;

   procedure process_ada_Renaming_Clause_Range
     (this    : in out class;
      --  node    : in     lal.stmt'class;
      node    : in     lal.ada_node'class)
   is
      parent_name : constant string := module_name;
      module_name : constant string := parent_name & ".process_Renaming_Clause_Range";
      package logging is new generic_logging (module_name); use logging;
      --  auto : logging.auto_logger; -- logs begin and end

      -- will raise declarative_part_error if node.kind is not in ada_stmt:
      kind             : constant lalco.ada_Renaming_Clause_Range := node.kind;
      use lalco; -- for subtype names in case stmt
   begin -- process_ada_Renaming_Clause_Range
      case kind is

         when ada_Renaming_Clause =>
            declare
               Renaming_Clause_Node : constant LAL.Renaming_Clause := LAL.As_Renaming_Clause (Node);
               renamed_object : constant LAL.Name := LAL.F_Renamed_Object (Renaming_Clause_Node);
            begin
               Log ("renamed_object: " & renamed_object.Image);
            end;
            this.add_not_implemented;

         when ada_Synthetic_Renaming_Clause =>
            declare
               Synthetic_Renaming_Clause_Node : constant LAL.Synthetic_Renaming_Clause := LAL.As_Synthetic_Renaming_Clause (Node);
            begin
               NULL; 
            end;
            this.add_not_implemented;

      end case;
   end process_ada_Renaming_Clause_Range;

   procedure process_ada_Reverse_Node
     (this    : in out class;
      --  node    : in     lal.stmt'class;
      node    : in     lal.ada_node'class)
   is
      parent_name : constant string := module_name;
      module_name : constant string := parent_name & ".process_Reverse_Node";
      package logging is new generic_logging (module_name); use logging;
      --  auto : logging.auto_logger; -- logs begin and end

      -- will raise declarative_part_error if node.kind is not in ada_stmt:
      kind             : constant lalco.ada_Reverse_Node := node.kind;
      use lalco; -- for subtype names in case stmt
   begin -- process_ada_Reverse_Node
      case kind is

         when ada_Reverse_Absent =>
            declare
               Reverse_Absent_Node : constant LAL.Reverse_Absent := LAL.As_Reverse_Absent (Node);
            begin
               NULL;
            end;
            this.add_not_implemented;

         when ada_Reverse_Present =>
            declare
               Reverse_Present_Node : constant LAL.Reverse_Present := LAL.As_Reverse_Present (Node);
            begin
               NULL;
            end;
            this.add_not_implemented;

      end case;
   end process_ada_Reverse_Node;

   procedure process_ada_Select_When_Part_Range
     (this    : in out class;
      --  node    : in     lal.stmt'class;
      node    : in     lal.ada_node'class)
   is
      parent_name : constant string := module_name;
      module_name : constant string := parent_name & ".process_Select_When_Part_Range";
      package logging is new generic_logging (module_name); use logging;
      --  auto : logging.auto_logger; -- logs begin and end

      -- will raise declarative_part_error if node.kind is not in ada_stmt:
      kind             : constant lalco.ada_Select_When_Part_Range := node.kind;
      use lalco; -- for subtype names in case stmt
   begin -- process_ada_Select_When_Part_Range
      case kind is

         when ada_Select_When_Part =>
            declare
               Select_When_Part_Node : constant LAL.Select_When_Part := LAL.As_Select_When_Part (Node);
               cond_expr : constant LAL.Expr := LAL.F_Cond_Expr (Select_When_Part_Node);
               stmts : constant LAL.Stmt_List := LAL.F_Stmts (Select_When_Part_Node);
            begin
               if not cond_expr.Is_Null then
                 Log ("cond_expr: " & cond_expr.Image);
               end if;
               if not stmts.Is_Null then
                 Log ("stmts: " & stmts.Image);
               end if;
            end;
            this.add_not_implemented;

      end case;
   end process_ada_Select_When_Part_Range;

   procedure Process_Ada_Stmt
     (This    : in out Class;
      --  Node    : in     LAL.Stmt'Class;
      Node    : in     LAL.Ada_Node'Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process_Ada_Stmt";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      --  Auto : Logging.Auto_Logger; -- Logs BEGIN and END

      -- Will raise Constraint_Error if Node.Kind is not in Ada_Stmt:
      Kind             : constant LALCO.Ada_Stmt := Node.Kind;
      use LALCO; -- For subtype names in case stmt
   begin -- Process_Ada_Stmt
      case Kind is
         when Ada_Accept_Stmt =>
            declare
               Accept_Stmt_Node : constant LAL.Accept_Stmt := LAL.As_Accept_Stmt (Node);
               Name : constant LAL.Identifier := LAL.F_Name (Accept_Stmt_Node);
               Entry_Index_Expr : constant LAL.Expr := LAL.F_Entry_Index_Expr (Accept_Stmt_Node);
               Params : constant LAL.Entry_Completion_Formal_Params := LAL.F_Params (Accept_Stmt_Node);
            begin
               Log ("Name: " & Name.Image);
               if not Entry_Index_Expr.Is_Null then
                 Log ("Entry_Index_Expr: " & Entry_Index_Expr.Image);
               end if;
               Log ("Params: " & Params.Image);
            end;

            This.Add_Not_Implemented;
         when Ada_Accept_Stmt_With_Stmts =>
            declare
               Accept_Stmt_With_Stmts_Node : constant LAL.Accept_Stmt_With_Stmts := LAL.As_Accept_Stmt_With_Stmts (Node);
               Stmts : constant LAL.Handled_Stmts := LAL.F_Stmts (Accept_Stmt_With_Stmts_Node);
               End_Name : constant LAL.End_Name := LAL.F_End_Name (Accept_Stmt_With_Stmts_Node);
            begin
               Log ("Stmts: " & Stmts.Image);
               Log ("End_Name: " & End_Name.Image);
            end;

            This.Add_Not_Implemented;
         when Ada_For_Loop_Stmt =>
            declare
               For_Loop_Stmt_Node : constant LAL.For_Loop_Stmt := LAL.As_For_Loop_Stmt (Node);
            begin
               NULL;
            end;

            This.Add_Not_Implemented;
         when Ada_Loop_Stmt =>
            declare
               Loop_Stmt_Node : constant LAL.Loop_Stmt := LAL.As_Loop_Stmt (Node);
            begin
               NULL;
            end;

            This.Add_Not_Implemented;
         when Ada_While_Loop_Stmt =>
            declare
               While_Loop_Stmt_Node : constant LAL.While_Loop_Stmt := LAL.As_While_Loop_Stmt (Node);
            begin
               NULL;
            end;

            This.Add_Not_Implemented;
         when Ada_Begin_Block =>
            declare
               Begin_Block_Node : constant LAL.Begin_Block := LAL.As_Begin_Block (Node);
               Stmt : constant LAL.Handled_Stmts := LAL.F_Stmts (Begin_Block_Node);
               End_Name : constant LAL.End_Name := LAL.F_End_Name (Begin_Block_Node);
            begin
               Log ("Stmt: " & Stmt.Image);
               if not End_Name.Is_Null then
                 Log ("End_Name: " & End_Name.Image);
               end if;
            end;

            This.Add_Not_Implemented;
         when Ada_Decl_Block =>
            declare
               Decl_Block_Node : constant LAL.Decl_Block := LAL.As_Decl_Block (Node);
               Decl : constant LAL.Declarative_Part := LAL.F_Decls (Decl_Block_Node);
               Stmt : constant LAL.Handled_Stmts := LAL.F_Stmts (Decl_Block_Node);
               End_Name : constant LAL.End_Name := LAL.F_End_Name (Decl_Block_Node);
            begin
               Log ("Decl: " & Decl.Image);
               Log ("Stmt: " & Stmt.Image);
               if not End_Name.Is_Null then
                 Log ("End_Name: " & End_Name.Image);
               end if;
            end;

            This.Add_Not_Implemented;
         when Ada_Case_Stmt =>
            declare
               Case_Stmt_Node : constant LAL.Case_Stmt := LAL.As_Case_Stmt (Node);
               Expr : constant LAL.Expr := LAL.F_Expr (Case_Stmt_Node);
               Alternatives : constant LAL.Case_Stmt_Alternative_List := LAL.F_Alternatives (Case_Stmt_Node);
            begin
               Log ("Expr: " & Expr.Image);
               Log ("Alternatives: " & Alternatives.Image);
            end;

            This.Add_Not_Implemented;
         when Ada_Extended_Return_Stmt =>
            declare
               Extended_Return_Stmt_Node : constant LAL.Extended_Return_Stmt := LAL.As_Extended_Return_Stmt (Node);
               Decl_Stmt : constant LAL.Extended_Return_Stmt_Object_Decl := LAL.F_Decl (Extended_Return_Stmt_Node);
               Stmt : constant LAL.Handled_Stmts := LAL.F_Stmts (Extended_Return_Stmt_Node);
            begin
               Log ("Decl_Stmt: " & Decl_Stmt.Image);
               Log ("Stmt: " & Stmt.Image);
            end;

            This.Add_Not_Implemented;
         when Ada_If_Stmt =>
            declare
               If_Stmt_Node : constant LAL.If_Stmt := LAL.As_If_Stmt (Node);
               Then_Stmt : constant LAL.Stmt_List := LAL.F_Then_Stmts (If_Stmt_Node);
               Alternative : constant LAL.Elsif_Stmt_Part_List := LAL.F_Alternatives (If_Stmt_Node);
               Else_Stmt : constant LAL.Stmt_List := LAL.F_Else_Stmts (If_Stmt_Node);
            begin
               Log ("Then_Stmt: " & Then_Stmt.Image);
               Log ("Alternative: " & Alternative.Image);
               Log ("Else_Stmt: " & Else_Stmt.Image);
            end;

            This.Add_Not_Implemented;
         when Ada_Named_Stmt =>
            declare
               Named_Stmt_Node : constant LAL.Named_Stmt := LAL.As_Named_Stmt (Node);
               Decl_Stmt : constant LAL.Named_Stmt_Decl := LAL.F_Decl (Named_Stmt_Node);
               Stmt : constant LAL.Composite_Stmt := LAL.F_Stmt (Named_Stmt_Node);
            begin
               Log ("Decl_Stmt: " & Decl_Stmt.Image);
               Log ("Stmt: " & Stmt.Image);
            end;

            This.Add_Not_Implemented;
         when Ada_Select_Stmt =>
            declare
               Select_Stmt_Node : constant LAL.Select_Stmt := LAL.As_Select_Stmt (Node);
               Guards : constant LAL.Select_When_Part_List := LAL.F_Guards (Select_Stmt_Node);
               Else_Stmt : constant LAL.Stmt_List := LAL.F_Else_Stmts (Select_Stmt_Node);
               Abort_Stmts : constant LAL.Stmt_List := LAL.F_Abort_Stmts (Select_Stmt_Node);
            begin
               Log ("F_Guards: " & Guards.Image);
               Log ("F_Else_Stmts: " & Else_Stmt.Image);
               Log ("F_Abort_Stmts: " & Abort_Stmts.Image);
            end;

            This.Add_Not_Implemented;
         when Ada_Error_Stmt =>
            declare
               Error_Stmt_Node : constant LAL.Error_Stmt := LAL.As_Error_Stmt (Node);
            begin
               NULL;
            end;

            This.Add_Not_Implemented;
         when Ada_Abort_Stmt =>
            declare
               Abort_Stmt_Node : constant LAL.Abort_Stmt := LAL.As_Abort_Stmt (Node);
               Names           : constant LAL.Name_List := LAL.F_Names (Abort_Stmt_Node);
            begin
               Log ("F_Names: " & Names.Image);
            end;

            This.Add_Not_Implemented;

         when Ada_Assign_Stmt =>
            declare
               Assign_Stmt_Node : constant LAL.Assign_Stmt := LAL.As_Assign_Stmt (Node);
               Dest             : constant LAL.Name := LAL.F_Dest (Assign_Stmt_Node);
               Expr             : constant LAL.Expr := LAL.F_Expr (Assign_Stmt_Node);
            begin
               Log ("F_Dest: " & Dest.Image);
               Log ("F_Expr: " & Expr.Image);
            end;

            This.Add_Not_Implemented;

         when Ada_Call_Stmt =>
            declare
               Call_Stmt_Node : constant LAL.Call_Stmt := LAL.As_Call_Stmt (Node);
               Call           : constant LAL.Name := LAL.F_Call (Call_Stmt_Node);
            begin
               Log ("F_Call: " & Call.Image);
            end;

            This.Add_Not_Implemented;

         when Ada_Delay_Stmt =>
            declare
               Delay_Stmt_Node : constant LAL.Delay_Stmt := LAL.As_Delay_Stmt (Node);
               Has_Until       : constant Boolean := LAL.F_Has_Until (Delay_Stmt_Node);
               Seconds         : constant LAL.Expr := LAL.F_Expr (Delay_Stmt_Node);
            begin
               Log ("F_Has_Until: " & Boolean'Image (Has_Until));
               Log ("Seconds: " & Seconds.Image);
            end;

            This.Add_Not_Implemented;

         when Ada_Exit_Stmt =>
            declare
               Exit_Stmt_Node : constant LAL.Exit_Stmt := LAL.As_Exit_Stmt (Node);
               Loop_Name      : constant LAL.Name := LAL.F_Loop_Name (Exit_Stmt_Node);
               Cond_Expr      : constant LAL.Expr := LAL.F_Cond_Expr (Exit_Stmt_Node);
            begin
               if not Loop_Name.Is_Null then
                 Log ("F_Loop_Name: " & Loop_Name.Image);
               end if;
               if not Cond_Expr.Is_Null then
                 Log ("F_Cond_Expr: " & Cond_Expr.Image);
               end if;
            end;

            This.Add_Not_Implemented;
         when Ada_Goto_Stmt =>
            declare
               Goto_Stmt_Node : constant LAL.Goto_Stmt := LAL.As_Goto_Stmt (Node);
               Label          : constant LAL.Name := LAL.F_Label_Name (Goto_Stmt_Node);
            begin
               Log ("F_Label_Name: " & Label.Image);
            end;

            This.Add_Not_Implemented;
         when Ada_Label =>
            declare
               Label_Node : constant LAL.Label := LAL.As_Label (Node);
               Label_Decl : constant LAL.Label_Decl := LAL.F_Decl (Label_Node);
            begin
               Log ("F_Decl: " & Label_Decl.Image);
            end;

            This.Add_Not_Implemented;
         when Ada_Null_Stmt =>
            declare
               Null_Stmt_Node : constant LAL.Null_Stmt := LAL.As_Null_Stmt (Node);
            begin
               NULL;
            end;

            This.Add_Not_Implemented;
         when Ada_Raise_Stmt =>
            declare
               Raise_Stmt_Node : constant LAL.Raise_Stmt := LAL.As_Raise_Stmt (Node);
               Exception_Name  : constant LAL.Name := LAL.F_Exception_Name (Raise_Stmt_Node);
               Error_Message  : constant LAL.Expr := LAL.F_Error_Message (Raise_Stmt_Node);
            begin
               Log ("F_Exception_Name: " & Exception_Name.Image);
               if not Error_Message.Is_Null then
                 Log ("Error_Message: " & Error_Message.Image);
               end if;
            end;

            This.Add_Not_Implemented;
         when Ada_Requeue_Stmt =>
            declare
               Requeue_Stmt_Node : constant LAL.Requeue_Stmt := LAL.As_Requeue_Stmt (Node);
               Call_Name         : constant LAL.Name := LAL.F_Call_Name (Requeue_Stmt_Node);
               Has_Abort         : constant Boolean := LAL.F_Has_Abort (Requeue_Stmt_Node);
            begin
               Log ("F_Call_Name: " & Call_Name.Image);
               Log ("F_Has_Abort: " & Boolean'Image (Has_Abort));
            end;

            This.Add_Not_Implemented;
         when Ada_Return_Stmt =>
            declare
               Return_Stmt_Node : constant LAL.Return_Stmt := LAL.As_Return_Stmt (Node);
               Return_Expr      : constant LAL.Expr := LAL.F_Return_Expr (Return_Stmt_Node);
            begin
               Log ("F_Return_Expr: " & Return_Expr.Image);
            end;

            This.Add_Not_Implemented;
         when Ada_Terminate_Alternative =>
            declare
               Terminate_Alternative_Node : constant LAL.Terminate_Alternative := LAL.As_Terminate_Alternative (Node);
            begin
               NULL;
            end;

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

   procedure process_ada_Subp_Kind
     (this    : in out class;
      --  node    : in     lal.stmt'class;
      node    : in     lal.ada_node'class)
   is
      parent_name : constant string := module_name;
      module_name : constant string := parent_name & ".process_Subp_Kind";
      package logging is new generic_logging (module_name); use logging;
      --  auto : logging.auto_logger; -- logs begin and end

      -- will raise declarative_part_error if node.kind is not in ada_stmt:
      kind             : constant lalco.ada_Subp_Kind := node.kind;
      use lalco; -- for subtype names in case stmt
   begin -- process_ada_Subp_Kind
      case kind is

         when ada_Subp_Kind_Function =>
            declare
               Subp_Kind_Function_Node : constant LAL.Subp_Kind_Function := LAL.As_Subp_Kind_Function (Node);
            begin
               NULL;
            end;

         when ada_Subp_Kind_Procedure =>
            declare
               Subp_Kind_Procedure_Node : constant LAL.Subp_Kind_Procedure := LAL.As_Subp_Kind_Procedure (Node);
            begin
               NULL;
            end;

      end case;
   end process_ada_Subp_Kind;

   procedure process_ada_Subunit_Range
     (this    : in out class;
      --  node    : in     lal.stmt'class;
      node    : in     lal.ada_node'class)
   is
      parent_name : constant string := module_name;
      module_name : constant string := parent_name & ".process_Subunit_Range";
      package logging is new generic_logging (module_name); use logging;
      --  auto : logging.auto_logger; -- logs begin and end

      -- will raise declarative_part_error if node.kind is not in ada_stmt:
      kind             : constant lalco.ada_Subunit_Range := node.kind;
      use lalco; -- for subtype names in case stmt
   begin -- process_ada_Subunit_Range
      case kind is

         when ada_Subunit =>
            declare
               Subunit_Node : constant LAL.Subunit := LAL.As_Subunit (Node);
               name      : constant LAL.Name := LAL.F_Name (Subunit_Node);
               f_body      : constant LAL.Body_Node := LAL.F_Body (Subunit_Node);
               body_root      : constant LAL.Basic_Decl := LAL.P_Body_Root (Subunit_Node);
            begin
               Log ("name: " & name.Image);
               Log ("f_body: " & f_body.Image);
               Log ("body_root: " & body_root.Image);
            end;

      end case;
   end process_ada_Subunit_Range;

   procedure process_ada_Synchronized_Node
     (this    : in out class;
      --  node    : in     lal.stmt'class;
      node    : in     lal.ada_node'class)
   is
      parent_name : constant string := module_name;
      module_name : constant string := parent_name & ".process_Synchronized_Node";
      package logging is new generic_logging (module_name); use logging;
      --  auto : logging.auto_logger; -- logs begin and end

      -- will raise declarative_part_error if node.kind is not in ada_stmt:
      kind             : constant lalco.ada_Synchronized_Node := node.kind;
      use lalco; -- for subtype names in case stmt
   begin -- process_ada_Synchronized_Node
      case kind is

         when ada_Synchronized_Absent =>
            declare
               Synchronized_Absent_Node : constant LAL.Synchronized_Absent := LAL.As_Synchronized_Absent (Node);
            begin
               NULL;
            end;

         when ada_Synchronized_Present =>
            declare
               Synchronized_Present_Node : constant LAL.Synchronized_Present := LAL.As_Synchronized_Present (Node);
            begin
               NULL;
            end;

      end case;
   end process_ada_Synchronized_Node;

   procedure process_ada_Tagged_Node
     (this    : in out class;
      --  node    : in     lal.stmt'class;
      node    : in     lal.ada_node'class)
   is
      parent_name : constant string := module_name;
      module_name : constant string := parent_name & ".process_Tagged_Node";
      package logging is new generic_logging (module_name); use logging;
      --  auto : logging.auto_logger; -- logs begin and end

      -- will raise declarative_part_error if node.kind is not in ada_stmt:
      kind             : constant lalco.ada_Tagged_Node := node.kind;
      use lalco; -- for subtype names in case stmt
   begin -- process_ada_Tagged_Node
      case kind is

         when ada_Tagged_Absent =>
            declare
               Tagged_Absent_Node : constant LAL.Tagged_Absent := LAL.As_Tagged_Absent (Node);
            begin
               NULL;
            end;

         when ada_Tagged_Present =>
            declare
               Tagged_Present_Node : constant LAL.Tagged_Present := LAL.As_Tagged_Present (Node);
            begin
               NULL;
            end;

      end case;
   end process_ada_Tagged_Node;

   procedure process_ada_Task_Def_Range
     (this    : in out class;
      --  node    : in     lal.stmt'class;
      node    : in     lal.ada_node'class)
   is
      parent_name : constant string := module_name;
      module_name : constant string := parent_name & ".process_Task_Def_Range";
      package logging is new generic_logging (module_name); use logging;
      --  auto : logging.auto_logger; -- logs begin and end

      -- will raise declarative_part_error if node.kind is not in ada_stmt:
      kind             : constant lalco.ada_Task_Def_Range := node.kind;
      use lalco; -- for subtype names in case stmt
   begin -- process_ada_Task_Def_Range
      case kind is

         when ada_Task_Def =>
            declare
               Task_Def_Node : constant LAL.Task_Def := LAL.As_Task_Def (Node);
               interfaces      : constant LAL.Parent_List := LAL.F_Interfaces (Task_Def_Node);
               public_part      : constant LAL.Public_Part := LAL.F_Public_Part (Task_Def_Node);
               private_part      : constant LAL.Private_part := LAL.F_Private_Part (Task_Def_Node);
               end_name      : constant LAL.End_Name := LAL.F_End_Name (Task_Def_Node);
            begin
               Log ("interfaces: " & interfaces.Image);
               Log ("public_part: " & public_part.Image);
               if not private_part.Is_Null then
                 Log ("private_part: " & private_part.Image);
               end if;
               Log ("end_name: " & end_name.Image);
            end;

      end case;
   end process_ada_Task_Def_Range;

   procedure process_ada_Type_Def
     (this    : in out class;
      --  node    : in     lal.stmt'class;
      node    : in     lal.ada_node'class)
   is
      parent_name : constant string := module_name;
      module_name : constant string := parent_name & ".process_Def_Range";
      package logging is new generic_logging (module_name); use logging;
      --  auto : logging.auto_logger; -- logs begin and end

      -- will raise declarative_part_error if node.kind is not in ada_stmt:
      kind             : constant lalco.ada_Type_Def := node.kind;
      use lalco; -- for subtype names in case stmt
   begin -- process_ada_Type_Def
      case kind is

         when ada_Access_To_Subp_Def =>
            declare
               Access_To_Subp_Def_Node : constant LAL.Access_To_Subp_Def := LAL.As_Access_To_Subp_Def (Node);
               has_protected      : constant Boolean := LAL.F_Has_Protected (Access_To_Subp_Def_Node);
               sub_spec      : constant LAL.Subp_Spec := LAL.F_Subp_Spec (Access_To_Subp_Def_Node);
            begin
               Log ("has_protected: " & Boolean'Image(has_protected));
               Log ("sub_spec: " & sub_spec.Image);
            end;

         when ada_Anonymous_Type_Access_Def =>
            declare
               Anonymous_Type_Access_Def_Node : constant LAL.Anonymous_Type_Access_Def := LAL.As_Anonymous_Type_Access_Def (Node);
               type_decl      : constant LAL.Base_Type_Decl := LAL.F_Type_Decl (Anonymous_Type_Access_Def_Node);
            begin
               Log ("type_decl: " & type_decl.Image);
            end;

         when ada_Type_Access_Def =>
            declare
               Type_Access_Def_Node : constant LAL.Type_Access_Def := LAL.As_Type_Access_Def (Node);
               has_all      : constant Boolean := LAL.F_Has_All (Type_Access_Def_Node);
               has_constant      : constant Boolean := LAL.F_Has_Constant (Type_Access_Def_Node);
            begin
               Log ("has_all: " & Boolean'Image(has_all));
               Log ("has_constant: " & Boolean'Image(has_constant));
            end;

         when ada_Array_Type_Def =>
            declare
               Array_Type_Def_Node : constant LAL.Array_Type_Def := LAL.As_Array_Type_Def (Node);
               indices      : constant LAL.Array_Indices := LAL.F_Indices (Array_Type_Def_Node);
               component_type      : constant LAL.Component_Def := LAL.F_Component_Type (Array_Type_Def_Node);
            begin
               Log ("indices: " & indices.Image);
               Log ("component_type: " & component_type.Image);
            end;

         when ada_Derived_Type_Def =>
            declare
               Derived_Type_Def_Node : constant LAL.Derived_Type_Def := LAL.As_Derived_Type_Def (Node);
               interfaces      : constant LAL.Parent_List := LAL.F_Interfaces (Derived_Type_Def_Node);
               record_extension      : constant LAL.Base_Record_Def := LAL.F_Record_Extension (Derived_Type_Def_Node);
               has_with_private      : constant Boolean := LAL.F_Has_With_Private (Derived_Type_Def_Node);
            begin
               if not interfaces.Is_Null then
                 Log ("interfaces: " & interfaces.Image);
               end if;
               if not record_extension.Is_Null then
                 Log ("record_extension: " & record_extension.Image);
               end if;
               Log ("has_with_private: " & Boolean'Image(has_with_private));
            end;

         when ada_Enum_Type_Def =>
            declare
               Enum_Type_Def_Node : constant LAL.Enum_Type_Def := LAL.As_Enum_Type_Def (Node);
               enum_literals      : constant LAL.Enum_Literal_Decl_List := LAL.F_Enum_Literals (Enum_Type_Def_Node);
            begin
               Log ("enum_literals: " & enum_literals.Image);
            end;

         when ada_Formal_Discrete_Type_Def =>
            declare
               Formal_Discrete_Type_Def_Node : constant LAL.Formal_Discrete_Type_Def := LAL.As_Formal_Discrete_Type_Def (Node);
            begin
               NULL; 
            end;

         when ada_Interface_Type_Def =>
            declare
               Interface_Type_Def_Node : constant LAL.Interface_Type_Def := LAL.As_Interface_Type_Def (Node);
               interface_kind      : constant LAL.Interface_Kind := LAL.F_Interface_Kind (Interface_Type_Def_Node);
            begin
               Log ("interface_kind: " & interface_kind.Image);
            end;

         when ada_Mod_Int_Type_Def =>
            declare
               Mod_Int_Type_Def_Node : constant LAL.Mod_Int_Type_Def := LAL.As_Mod_Int_Type_Def (Node);
               expr      : constant LAL.Expr := LAL.F_Expr (Mod_Int_Type_Def_Node);
            begin
               Log ("expr: " & expr.Image);
            end;

         when ada_Private_Type_Def =>
            declare
               Private_Type_Def_Node : constant LAL.Private_Type_Def := LAL.As_Private_Type_Def (Node);
               has_abstract      : constant Boolean := LAL.F_Has_Abstract (Private_Type_Def_Node);
               has_tagged      : constant Boolean := LAL.F_Has_Tagged (Private_Type_Def_Node);
               has_limited      : constant Boolean := LAL.F_Has_Limited (Private_Type_Def_Node);
            begin
               Log ("has_abstract: " & Boolean'Image(has_abstract));
               Log ("has_tagged: " & Boolean'Image(has_tagged));
               Log ("has_limited: " & Boolean'Image(has_limited));
            end;

         when ada_Decimal_Fixed_Point_Def =>
            declare
               Decimal_Fixed_Point_Def_Node : constant LAL.Decimal_Fixed_Point_Def := LAL.As_Decimal_Fixed_Point_Def (Node);
               f_delta      : constant LAL.Expr := LAL.F_Delta (Decimal_Fixed_Point_Def_Node);
               f_digits      : constant LAL.Expr := LAL.F_Digits (Decimal_Fixed_Point_Def_Node);
               f_range      : constant LAL.Range_Spec := LAL.F_Range (Decimal_Fixed_Point_Def_Node);
            begin
               Log ("f_delta: " & f_delta.Image);
               Log ("f_digits: " & f_digits.Image);
               if not f_range.Is_Null then
                 Log ("f_range: " & f_range.Image);
               end if;
            end;

         when ada_Floating_Point_Def =>
            declare
               Floating_Point_Def_Node : constant LAL.Floating_Point_Def := LAL.As_Floating_Point_Def (Node);
               num_digits      : constant LAL.Expr := LAL.F_Num_Digits (Floating_Point_Def_Node);
               f_range      : constant LAL.Range_Spec := LAL.F_Range (Floating_Point_Def_Node);
            begin
               Log ("num_digits: " & num_digits.Image);
               if not f_range.Is_Null then
                 Log ("f_range: " & f_range.Image);
               end if;
            end;

         when ada_Ordinary_Fixed_Point_Def =>
            declare
               Ordinary_Fixed_Point_Def_Node : constant LAL.Ordinary_Fixed_Point_Def := LAL.As_Ordinary_Fixed_Point_Def (Node);
               f_delta      : constant LAL.Expr := LAL.F_Delta (Ordinary_Fixed_Point_Def_Node);
               f_range      : constant LAL.Range_Spec := LAL.F_Range (Ordinary_Fixed_Point_Def_Node);
            begin
               Log ("f_delta: " & f_delta.Image);
               Log ("f_range: " & f_range.Image);
            end;

         when ada_Record_Type_Def =>
            declare
               Record_Type_Def_Node : constant LAL.Record_Type_Def := LAL.As_Record_Type_Def (Node);
               has_abstract      : constant Boolean := LAL.F_Has_Abstract (Record_Type_Def_Node);
               has_tagged      : constant Boolean := LAL.F_Has_Tagged (Record_Type_Def_Node);
               has_limited      : constant Boolean := LAL.F_Has_Limited (Record_Type_Def_Node);
            begin
               Log ("has_abstract: " & Boolean'Image(has_abstract));
               Log ("has_tagged: " & Boolean'Image(has_tagged));
               Log ("has_limited: " & Boolean'Image(has_limited));
            end;

         when ada_Signed_Int_Type_Def =>
            declare
               Signed_Int_Type_Def_Node : constant LAL.Signed_Int_Type_Def := LAL.As_Signed_Int_Type_Def (Node);
               f_range      : constant LAL.Range_Spec := LAL.F_Range (Signed_Int_Type_Def_Node);
            begin
               Log ("f_range: " & f_range.Image);
            end;

      end case;
   end process_ada_Type_Def;

   procedure process_ada_Type_Expr
     (this    : in out class;
      --  node    : in     lal.stmt'class;
      node    : in     lal.ada_node'class)
   is
      parent_name : constant string := module_name;
      module_name : constant string := parent_name & ".process_Type_Expr";
      package logging is new generic_logging (module_name); use logging;
      --  auto : logging.auto_logger; -- logs begin and end

      -- will raise declarative_part_error if node.kind is not in ada_stmt:
      kind             : constant lalco.ada_Type_Expr := node.kind;
      use lalco; -- for subtype names in case stmt
   begin -- process_ada_Type_Expr
      case kind is

         when ada_Anonymous_Type =>
            declare
               Anonymous_Type_Node : constant LAL.Anonymous_Type := LAL.As_Anonymous_Type (Node);
               type_Decl      : constant LAL.Anonymous_Type_Decl := LAL.F_Type_Decl (Anonymous_Type_Node);
            begin
               Log ("type_Decl: " & type_Decl.Image);
            end;

         when ada_Enum_Lit_Synth_Type_Expr =>
            declare
               Enum_Lit_Synth_Type_Expr_Node : constant LAL.Enum_Lit_Synth_Type_Expr := LAL.As_Enum_Lit_Synth_Type_Expr (Node);
            begin
               NULL; 
            end;

         when ada_Subtype_Indication =>
            declare
               Subtype_Indication_Node : constant LAL.Subtype_Indication := LAL.As_Subtype_Indication (Node);
               has_not_null      : constant Boolean := LAL.F_Has_Not_Null (Subtype_Indication_Node);
               name      : constant LAL.Name := LAL.F_Name (Subtype_Indication_Node);
               constraint      : constant LAL.Constraint := LAL.F_Constraint (Subtype_Indication_Node);
            begin
               Log ("has_not_null: " & Boolean'Image(has_not_null));
               Log ("name: " & name.Image);
               if not constraint.Is_Null then
                 Log ("constraint: " & constraint.Image);
               end if;
            end;

         when ada_Constrained_Subtype_Indication =>
            declare
               Constrained_Subtype_Indication_Node : constant LAL.Constrained_Subtype_Indication := LAL.As_Constrained_Subtype_Indication (Node);
            begin
               NULL; 
            end;

         when ada_Discrete_Subtype_Indication =>
            declare
               Discrete_Subtype_Indication_Node : constant LAL.Discrete_Subtype_Indication := LAL.As_Discrete_Subtype_Indication (Node);
            begin
               NULL; 
            end;

      end case;
   end process_ada_Type_Expr;

   procedure process_ada_Unconstrained_Array_Index_Range
     (this    : in out class;
      --  node    : in     lal.stmt'class;
      node    : in     lal.ada_node'class)
   is
      parent_name : constant string := module_name;
      module_name : constant string := parent_name & ".process_Unconstrained_Array_Index_Range";
      package logging is new generic_logging (module_name); use logging;
      --  auto : logging.auto_logger; -- logs begin and end

      -- will raise declarative_part_error if node.kind is not in ada_stmt:
      kind             : constant lalco.ada_Unconstrained_Array_Index_Range := node.kind;
      use lalco; -- for subtype names in case stmt
   begin -- process_ada_Unconstrained_Array_Index_Range
      case kind is

         when ada_Unconstrained_Array_Index =>
            declare
               Unconstrained_Array_Index_Node : constant LAL.Unconstrained_Array_Index := LAL.As_Unconstrained_Array_Index (Node);
            begin
               NULL; 
            end;

      end case;
   end process_ada_Unconstrained_Array_Index_Range;

   procedure process_ada_Until_Node
     (this    : in out class;
      --  node    : in     lal.stmt'class;
      node    : in     lal.ada_node'class)
   is
      parent_name : constant string := module_name;
      module_name : constant string := parent_name & ".process_Until_Node";
      package logging is new generic_logging (module_name); use logging;
      --  auto : logging.auto_logger; -- logs begin and end

      -- will raise declarative_part_error if node.kind is not in ada_stmt:
      kind             : constant lalco.ada_Until_Node := node.kind;
      use lalco; -- for subtype names in case stmt
   begin -- process_ada_Until_Node
      case kind is

         when ada_Until_Absent =>
            declare
               Until_Absent_Node : constant LAL.Until_Absent := LAL.As_Until_Absent (Node);
            begin
               NULL; 
            end;

         when ada_Until_Present =>
            declare
               Until_Present_Node : constant LAL.Until_Present := LAL.As_Until_Present (Node);
            begin
               NULL; 
            end;

      end case;
   end process_ada_Until_Node;

   procedure process_ada_Use_Clause
     (this    : in out class;
      --  node    : in     lal.stmt'class;
      node    : in     lal.ada_node'class)
   is
      parent_name : constant string := module_name;
      module_name : constant string := parent_name & ".process_Use_Clause";
      package logging is new generic_logging (module_name); use logging;
      --  auto : logging.auto_logger; -- logs begin and end

      -- will raise declarative_part_error if node.kind is not in ada_stmt:
      kind             : constant lalco.ada_Use_Clause := node.kind;
      use lalco; -- for subtype names in case stmt
   begin -- process_ada_Use_Clause
      case kind is

         when ada_Use_Package_Clause =>
            declare
               Use_Package_Clause_Node : constant LAL.Use_Package_Clause := LAL.As_Use_Package_Clause (Node);
               packages      : constant LAL.Name_List := LAL.F_Packages (Use_Package_Clause_Node);
            begin
               Log ("packages: " & packages.Image);
            end;

         when ada_Use_Type_Clause =>
            declare
               Use_Type_Clause_Node : constant LAL.Use_Type_Clause := LAL.As_Use_Type_Clause (Node);
               has_all      : constant Boolean := LAL.F_Has_All (Use_Type_Clause_Node);
               types      : constant LAL.Name_List := LAL.F_Types (Use_Type_Clause_Node);
            begin
               Log ("has_all: " & Boolean'Image(has_all));
               Log ("types: " & types.Image);
            end;

      end case;
   end process_ada_Use_Clause;

   procedure process_ada_Variant_Range
     (this    : in out class;
      --  node    : in     lal.stmt'class;
      node    : in     lal.ada_node'class)
   is
      parent_name : constant string := module_name;
      module_name : constant string := parent_name & ".process_Variant_Range";
      package logging is new generic_logging (module_name); use logging;
      --  auto : logging.auto_logger; -- logs begin and end

      -- will raise declarative_part_error if node.kind is not in ada_stmt:
      kind             : constant lalco.ada_Variant_Range := node.kind;
      use lalco; -- for subtype names in case stmt
   begin -- process_ada_Variant_Range
      case kind is

         when ada_Variant =>
            declare
               Variant_Node : constant LAL.Variant := LAL.As_Variant (Node);
               choices      : constant LAL.Alternatives_List := LAL.F_Choices (Variant_Node);
               components      : constant LAL.Component_List := LAL.F_Components (Variant_Node);
            begin
               Log ("choices: " & choices.Image);
               Log ("components: " & components.Image);
            end;

      end case;
   end process_ada_Variant_Range;

   procedure process_ada_Variant_Part_Range
     (this    : in out class;
      --  node    : in     lal.stmt'class;
      node    : in     lal.ada_node'class)
   is
      parent_name : constant string := module_name;
      module_name : constant string := parent_name & ".process_Variant_Part_Range";
      package logging is new generic_logging (module_name); use logging;
      --  auto : logging.auto_logger; -- logs begin and end

      -- will raise declarative_part_error if node.kind is not in ada_stmt:
      kind             : constant lalco.ada_Variant_Part_Range := node.kind;
      use lalco; -- for subtype names in case stmt
   begin -- process_ada_Variant_Part_Range
      case kind is

         when ada_Variant_Part =>
            declare
               Variant_Part_Node : constant LAL.Variant_Part := LAL.As_Variant_Part (Node);
               discr_name      : constant LAL.Identifier := LAL.F_Discr_Name (Variant_Part_Node);
               variant      : constant LAL.Variant_List := LAL.F_Variant (Variant_Part_Node);
            begin
               Log ("discr_name: " & discr_name.Image);
               Log ("variant: " & variant.Image);
            end;

      end case;
   end process_ada_Variant_Part_Range;

   procedure process_ada_With_Clause_Range
     (this    : in out class;
      --  node    : in     lal.stmt'class;
      node    : in     lal.ada_node'class)
   is
      parent_name : constant string := module_name;
      module_name : constant string := parent_name & ".process_With_Clause_Range";
      package logging is new generic_logging (module_name); use logging;
      --  auto : logging.auto_logger; -- logs begin and end

      -- will raise declarative_part_error if node.kind is not in ada_stmt:
      kind             : constant lalco.ada_With_Clause_Range := node.kind;
      use lalco; -- for subtype names in case stmt
   begin -- process_ada_With_Clause_Range
      case kind is

         when ada_With_Clause =>
            declare
               With_Clause_Node : constant LAL.With_Clause := LAL.As_With_Clause (Node);
               packages      : constant LAL.Name_List := LAL.F_Packages (With_Clause_Node);
               has_private      : constant Boolean := LAL.F_Has_Private (With_Clause_Node);
               has_limited      : constant Boolean := LAL.F_Has_Limited (With_Clause_Node);
            begin
               Log ("packages: " & packages.Image);
               Log ("has_private: " & Boolean'Image(has_private));
               Log ("has_limited: " & Boolean'Image(has_limited));
            end;

      end case;
   end process_ada_With_Clause_Range;

   procedure process_ada_With_Private
     (this    : in out class;
      --  node    : in     lal.stmt'class;
      node    : in     lal.ada_node'class)
   is
      parent_name : constant string := module_name;
      module_name : constant string := parent_name & ".process_With_Private";
      package logging is new generic_logging (module_name); use logging;
      --  auto : logging.auto_logger; -- logs begin and end

      -- will raise declarative_part_error if node.kind is not in ada_stmt:
      kind             : constant lalco.ada_With_Private := node.kind;
      use lalco; -- for subtype names in case stmt
   begin -- process_ada_With_Private
      case kind is

         when ada_With_Private_Absent =>
            declare
               With_Private_Absent_Node : constant LAL.With_Private_Absent := LAL.As_With_Private_Absent (Node);
            begin
               NULL; 
            end;

         when ada_With_Private_Present =>
            declare
               With_Private_Present_Node : constant LAL.With_Private_Present := LAL.As_With_Private_Present (Node);
            begin
               NULL; 
            end;

      end case;
   end process_ada_With_Private;

   -- Do_Pre_Child_Processing and Do_Post_Child_Processing below are preserved
   -- from Asis_Adapter for familiarity.
   --
   -- Asis_Adapter.Unit.Process indirectly calls Asis_Adapter.Element.
   -- Process_Element_Tree, which calls an instance of generic
   -- Asis.Iterator.Traverse_Element, instantiated with
   -- Do_Pre_Child_Processing and Do_Post_Child_Processing.
   --
   -- Lal_Adapter.Unit.Process indirectly calls LAL.Compilation_Unit.Traverse
   -- with a pointer that indrectly calls Lal_Adapter.Node.Process, which calls
   -- Do_Pre_Child_Processing and Do_Post_Child_Processing.

   procedure Do_Pre_Child_Processing
     (This    : in out Class;
      Node    : in     LAL.Ada_Node'Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Do_Pre_Child_Processing";
      package Logging is new Generic_Logging (Module_Name); use Logging;
      --  Auto : Logging.Auto_Logger; -- Logs BEGIN and END

      Result : a_nodes_h.Element_Struct renames This.A_Element;

      Sloc_Range_Image : constant string := Slocs.Image (Node.Sloc_Range);
      Kind             : constant LALCO.Ada_Node_Kind_Type := Node.Kind;
      Kind_Image       : constant String := LALCO.Ada_Node_Kind_Type'Image (Kind);
      Kind_Name        : constant String := Node.Kind_Name;
      Debug_Text       : constant String := Node.Image;

      procedure Add_Element_ID is begin
         -- ID is in the Dot node twice (once in the Label and once in
         -- Node_ID), but not in the a_node twice.
         This.Add_To_Dot_Label (To_String (This.Element_IDs.First_Element));
         Result.id := This.Element_IDs.First_Element;
      end;

      procedure Add_Node_Kind is begin
         This.Add_To_Dot_Label ("Node_Kind", Kind_Image);
         -- TODO: Result.Element_Kind := anhS.To_Element_Kinds (Element_Kind);
      end;

      procedure Add_Source_Location is
         Unit       : constant LAL.Analysis_Unit := Node.Unit;
         File_Name  : constant String := Unit.Get_Filename;
         Sloc_Range : constant Slocs.Source_Location_Range := Node.Sloc_Range;
         Image      : constant String := To_String (Node.Full_Sloc_Image);
      begin
         This.Add_To_Dot_Label ("Source", Image);
         Result.Source_Location :=
           (Unit_Name    => To_Chars_Ptr (File_Name),
            First_Line   => Interfaces.C.int (Sloc_Range.Start_Line),
            First_Column => Interfaces.C.int (Sloc_Range.Start_Column),
            Last_Line    => Interfaces.C.int (Sloc_Range.End_Line),
            Last_Column  => Interfaces.C.int (Sloc_Range.End_Column));
      end;

      procedure Add_Enclosing_Element is
         Value : constant a_nodes_h.Element_ID :=
           --  Get_Element_ID (Node.P_Semantic_Parent);
           Get_Element_ID (Node.Parent);
      begin
         --              State.Add_Dot_Edge (From  => Enclosing_Element_Id,
         --                              To    => State.Element_Id,
         --                              Label => "Child");
         This.Add_To_Dot_Label ("Enclosing_Element", Value);
         Result.Enclosing_Element_Id := Value;
      end;

      procedure Start_Output is
         Default_Node  : Dot.Node_Stmt.Class; -- Initialized
         Default_Label : Dot.HTML_Like_Labels.Class; -- Initialized
         --  Parent_Name : constant String := Module_Name;
         --  Module_Name : constant String := Parent_Name & ".Start_Output";
         --  package Logging is new Generic_Logging (Module_Name); use Logging;
         --  Auto : Logging.Auto_Logger; -- Logs BEGIN and END
      begin -- Start_Output
         -- Set defaults:
         Result := a_nodes_h.Support.Default_Element_Struct;
         This.Outputs.Text.End_Line;
         -- Element ID comes out on next line via Add_Element_ID:
         This.Outputs.Text.Put_Indented_Line (String'("BEGIN "));
         This.Outputs.Text.Indent;
         This.Dot_Node := Default_Node;
         This.Dot_Label := Default_Label;

         -- Get ID:
         This.Element_IDs.Prepend (Get_Element_ID (Node));
         -- Log ( " Elem ID: " & To_String(Get_Element_ID (Node)));
         This.Dot_Node.Node_ID.ID :=
           To_Dot_ID_Type (This.Element_IDs.First_Element, Element_ID_Kind);

         -- Result.Debug_Image := Debug_Image;
         -- Put_Debug;
         Add_Element_ID;
         Add_Node_Kind;
         Add_Enclosing_Element;
         Add_Source_Location;
      end Start_Output;

      procedure Finish_Output is
      --     Parent_Name : constant String := Module_Name;
      --     Module_Name : constant String := Parent_Name & ".Finish_Output";
      --     package Logging is new Generic_Logging (Module_Name); use Logging;
      --     Auto : Logging.Auto_Logger; -- Logs BEGIN and END
      begin
         This.Dot_Node.Add_Label (This.Dot_Label);
         This.Outputs.Graph.Append_Stmt
           (new Dot.Node_Stmt.Class'(This.Dot_Node));
         -- Depends on unique node ids:
         This.Outputs.A_Nodes.Push (Result);
      end Finish_Output;

      use LALCO; -- For subtype names in case stmt
      --  use type LALCO.Ada_Node_Kind_Type; -- For "="
   begin -- Do_Pre_Child_Processing
      --  Log ("Line" & Start_Line_Image & ": " & Kind_Image  & ": " & Debug_Text);
      --  if Node.Kind /= LALCO.Ada_Compilation_Unit then
      Log ("Kind enum: " & Kind_Image & "; Kind name: " & Kind_Name & " at " & Sloc_Range_Image);
      Start_Output;
      --  Log (LAL.Image(Node));
      --  if Kind in LALCO.Ada_Stmt then
      --     Log ("Statement");
      --  else
      --     Log ("NOT a statement");
      --  end if;
      --
    
      case Kind is
         -- 3 included kinds:
         when Ada_Abort_Node'First .. Ada_Abort_Node'Last =>
            This.Process_Ada_Abort_Node (Node);
         -- 3 included kinds:
         when Ada_Abstract_Node'First .. Ada_Abstract_Node'Last =>
            This.Process_Ada_Abstract_Node (Node);
         -- 30 included kinds:
         when Ada_Ada_List'First .. Ada_Ada_List'Last =>
            This.Process_Ada_Ada_List (Node);
         -- 3 included kinds:
         when Ada_Aliased_Node'First .. Ada_Aliased_Node'Last =>
            This.Process_Ada_Aliased_Node (Node);
         -- 3 included kinds:
         when Ada_All_Node'First .. Ada_All_Node'Last =>
            This.Process_Ada_All_Node (Node);
         -- 3 included kinds:
         when Ada_Array_Indices'First .. Ada_Array_Indices'Last =>
            This.Process_Ada_Array_Indices (Node);
         -- 1 included kinds:
         when Ada_Aspect_Assoc_Range'First .. Ada_Aspect_Assoc_Range'Last =>
            This.Process_Ada_Aspect_Assoc_Range (Node);
         -- 5 included kinds:
         when Ada_Aspect_Clause'First .. Ada_Aspect_Clause'Last =>
            This.Process_Ada_Aspect_Clause (Node);
         -- 2 included kinds:
         when Ada_Aspect_Spec_Range'First .. Ada_Aspect_Spec_Range'Last =>
            this.process_ada_aspect_spec_range (node);
         -- 3 included kinds:
         when Ada_Base_Assoc'First .. Ada_Base_Assoc'Last =>
            this.process_ada_Base_Assoc (node);
         -- 3 included kinds:
         when Ada_Base_Formal_Param_Holder'First .. Ada_Base_Formal_Param_Holder'Last =>
            this.process_ada_Base_Formal_Param_Holder (node);
         -- 3 included kinds:
         when Ada_Base_Record_Def'First .. Ada_Base_Record_Def'Last =>
            this.process_ada_Base_Record_Def (node);
         -- 5 included kinds:
         when Ada_Basic_Assoc'First .. Ada_Basic_Assoc'Last =>
            this.process_ada_Basic_Assoc (node);
         -- 74 included kinds:
         when Ada_Basic_Decl'First .. Ada_Basic_Decl'Last =>
            This.Process_Ada_Basic_Decl (Node);
         -- 1 included kinds:
         when Ada_Case_Stmt_Alternative_Range'First .. Ada_Case_Stmt_Alternative_Range'Last =>
            This.Process_Ada_Case_Stmt_Alternative_Range (Node);
         -- 1 included kinds:
         when Ada_Compilation_Unit_Range'First .. Ada_Compilation_Unit_Range'Last =>
            This.Process_Ada_Compilation_Unit_Range (Node);
         -- 1 included kinds:
         when Ada_Component_Clause_Range'First .. Ada_Component_Clause_Range'Last =>
            This.Process_Ada_Component_Clause_Range (Node);
         -- 1 included kinds:
         when Ada_Component_Def_Range'First .. Ada_Component_Def_Range'Last =>
            This.Process_Ada_Component_Def_Range (Node);
         -- 6 included kinds:
         when Ada_Constraint'First .. Ada_Constraint'Last =>
            This.Process_Ada_Constraint (Node);
         -- 3 included kinds:
         when Ada_Constant_Node'First .. Ada_Constant_Node'Last =>
            This.Process_Ada_Constant_Node (Node);
         -- 3 included kinds:
         when Ada_Declarative_Part_Range'First .. Ada_Declarative_Part_Range'Last =>
            This.Process_Ada_Declarative_Part_Range (Node);
         -- 1 included kinds:
         when Ada_Elsif_Expr_Part_Range'First .. Ada_Elsif_Expr_Part_Range'Last =>
            This.Process_Ada_Elsif_Expr_Part_Range (Node);
         -- 1 included kinds:
         when Ada_Elsif_Stmt_Part_Range'First .. Ada_Elsif_Stmt_Part_Range'Last =>
            This.Process_Ada_Elsif_Stmt_Part_Range (Node);
         -- 60 included kinds:
         when Ada_Expr'First .. Ada_Expr'Last =>
            This.Process_Ada_Expr (Node);
         -- 1 included kinds:
         when Ada_Handled_Stmts_Range'First .. Ada_Handled_Stmts_Range'Last =>
            This.Process_Ada_Handled_Stmts_Range (Node);
         -- 5 included kinds:
         when Ada_Interface_Kind'First .. Ada_Interface_Kind'Last =>
            This.Process_Ada_Interface_Kind (Node);
         -- 3 included kinds:
         when Ada_Iter_Type'First .. Ada_Iter_Type'Last =>
            This.Process_Ada_Iter_Type (Node);
         -- 2 included kinds:
         when Ada_Library_Item_Range'First .. Ada_Library_Item_Range'Last =>
            This.Process_Ada_Library_Item_Range (Node);
         -- 3 included kinds:
         when Ada_Limited_Node'First .. Ada_Limited_Node'Last =>
            This.Process_Ada_Limited_Node (Node);
         -- 3 included kinds:
         when Ada_Loop_Spec'First .. Ada_Loop_Spec'Last =>
            This.Process_Ada_Loop_Spec (Node);
         -- 3 included kinds:
         when Ada_Mode'First .. Ada_Mode'Last =>
            This.Process_Ada_Mode (Node);
         -- 1 included Kind:   
         when Ada_Multi_Abstract_State_Decl_Range'First .. Ada_Multi_Abstract_State_Decl_Range'Last =>
            This.Process_Ada_Multi_Abstract_State_Decl_Range (Node);
         -- 3 included kinds:
         when Ada_Not_Null'First .. Ada_Not_Null'Last =>
            This.Process_Ada_Not_Null (Node);
         -- 2 included kinds:
         when Ada_Null_Component_Decl_Range'First .. Ada_Null_Component_Decl_Range'Last =>
            This.Process_Ada_Null_Component_Decl_Range (Node);
         -- 2 included kinds:
         when Ada_Others_Designator_Range'First .. Ada_Others_Designator_Range'Last =>
            This.Process_Ada_Others_Designator_Range (Node);
         -- 4 included kinds:
         when Ada_Overriding_Node'First .. Ada_Overriding_Node'Last =>
            This.Process_Ada_Overriding_Node (Node);
         -- 4 included kinds:
         when Ada_Params_Range'First .. Ada_Params_Range'Last =>
            This.Process_Ada_Params_Range (Node);
         -- 1 included Kind:   
         when Ada_Paren_Abstract_State_Decl_Range'First .. Ada_Paren_Abstract_State_Decl_Range'Last =>
            This.Process_Ada_Paren_Abstract_State_Decl_Range (Node);
         -- 4 included kinds:
         when Ada_Pragma_Node_Range'First .. Ada_Pragma_Node_Range'Last =>
            This.Process_Ada_Pragma_Node_Range (Node);
         -- 4 included kinds:
         when Ada_Private_Node'First .. Ada_Private_Node'Last =>
            This.Process_Ada_Private_Node (Node);
         -- 4 included kinds:
         when Ada_Protected_Node'First .. Ada_Protected_Node'Last =>
            This.Process_Ada_Protected_Node (Node);
         -- 4 included kinds:
         when Ada_Protected_Def_Range'First .. Ada_Protected_Def_Range'Last =>
            This.Process_Ada_Protected_Def_Range (Node);
         -- 3 included kinds:
         when Ada_Quantifier'First .. Ada_Quantifier'Last =>
            This.Process_Ada_Quantifier (Node);
         -- 2 included kinds:
         when Ada_Range_Spec_Range'First .. Ada_Range_Spec_Range'Last =>
            This.Process_Ada_Range_Spec_Range (Node);
         -- 3 included kinds:
         when Ada_Renaming_Clause_Range'First .. Ada_Renaming_Clause_Range'Last =>
            This.Process_Ada_Renaming_Clause_Range (Node);
         -- 3 included kinds:
         when Ada_Reverse_Node'First .. Ada_Reverse_Node'Last =>
            This.Process_Ada_Reverse_Node (Node);
         -- 2 included kinds:
         when Ada_Select_When_Part_Range'First .. Ada_Select_When_Part_Range'Last =>
            This.Process_Ada_Select_When_Part_Range (Node);
         -- 31 (25?) included kinds:
         when Ada_Stmt'First .. Ada_Stmt'Last =>
            --  Log ("Tag: " & Ada.Tags.Expanded_Name (Node'Tag));
            --  This.Process_Ada_Stmt (LAL.Stmt'Class (Node), Outputs);
            This.Process_Ada_Stmt (Node);
         -- 3 included kinds:
         when Ada_Subp_Kind'First .. Ada_Subp_Kind'Last =>
            This.Process_Ada_Subp_Kind (Node);
         -- 2 included kinds:
         when Ada_Subunit_Range'First .. Ada_Subunit_Range'Last =>
            This.Process_Ada_Subunit_Range (Node);
         -- 3 included kinds:
         when Ada_Synchronized_Node'First .. Ada_Synchronized_Node'Last =>
            This.Process_Ada_Synchronized_Node (Node);
         -- 3 included kinds:
         when Ada_Tagged_Node'First .. Ada_Tagged_Node'Last =>
            This.Process_Ada_Tagged_Node (Node);
         -- 2 included kinds:
         when Ada_Task_Def_Range'First .. Ada_Task_Def_Range'Last =>
            This.Process_Ada_Task_Def_Range (Node);
         -- 17 included kinds:
         when Ada_Type_Def'First .. Ada_Type_Def'Last =>
            This.Process_Ada_Type_Def (Node);
         -- 5 included kinds:
         when Ada_Type_Expr'First .. Ada_Type_Expr'Last =>
            This.Process_Ada_Type_Expr (Node);
         -- 2 included kinds:
         when Ada_Unconstrained_Array_Index_Range'First .. Ada_Unconstrained_Array_Index_Range'Last =>
            This.Process_Ada_Unconstrained_Array_Index_Range (Node);
         -- 3 included kinds:
         when Ada_Until_Node'First .. Ada_Until_Node'Last =>
            This.Process_Ada_Until_Node (Node);
         -- 3 included kinds:
         when Ada_Use_Clause'First .. Ada_Use_Clause'Last =>
            This.Process_Ada_Use_Clause (Node);
         -- 2 included kinds:
         when Ada_Variant_Range'First .. Ada_Variant_Range'Last =>
            This.Process_Ada_Variant_Range (Node);
         -- 2 included kinds:
         when Ada_Variant_Part_Range'First .. Ada_Variant_Part_Range'Last =>
            This.Process_Ada_Variant_Part_Range (Node);
         -- 2 included kinds:
         when Ada_With_Clause_Range'First .. Ada_With_Clause_Range'Last =>
            This.Process_Ada_With_Clause_Range (Node);
         -- 3 included kinds:
         when Ada_With_Private'First .. Ada_With_Private'Last =>
            This.Process_Ada_With_Private (Node);

         -- when Ada_Abort_Node'First .. Ada_Abort_Node'Last =>
         --   This.Add_Not_Implemented;
         -- when Ada_Abstract_Node'First .. Ada_Abstract_Node'Last =>
         --   This.Add_Not_Implemented;
         -- when Ada_Ada_List'First .. Ada_Ada_List'Last =>
         --   This.Add_Not_Implemented;
         -- when Ada_Aliased_Node'First .. Ada_Aliased_Node'Last =>
         --   This.Add_Not_Implemented;
         -- when Ada_All_Node'First .. Ada_All_Node'Last =>
         --   This.Add_Not_Implemented;
         -- when Ada_Array_Indices'First .. Ada_Array_Indices'Last =>
         --   This.Add_Not_Implemented;
         -- when Ada_Aspect_Assoc_Range'First .. Ada_Aspect_Assoc_Range'Last =>
         --   This.Add_Not_Implemented;
         -- when Ada_Aspect_Clause'First .. Ada_Aspect_Clause'Last =>
         --   This.Add_Not_Implemented;
         -- when Ada_Aspect_Spec_Range'First .. Ada_Aspect_Spec_Range'Last =>
         --   This.Add_Not_Implemented;
         -- when Ada_Base_Assoc'First .. Ada_Base_Assoc'Last =>
         --   This.Add_Not_Implemented;
         -- when Ada_Base_Formal_Param_Holder'First .. Ada_Base_Formal_Param_Holder'Last =>
         --   This.Add_Not_Implemented;
         -- when Ada_Base_Record_Def'First .. Ada_Base_Record_Def'Last =>
         --   This.Add_Not_Implemented;
         -- when Ada_Basic_Assoc'First .. Ada_Basic_Assoc'Last =>
         --   This.Add_Not_Implemented;
         --  Moved to top:
         --  when Ada_Basic_Decl'First .. Ada_Basic_Decl'Last =>
         -- when Ada_Case_Stmt_Alternative_Range'First .. Ada_Case_Stmt_Alternative_Range'Last =>
         --   This.Add_Not_Implemented;
         -- when Ada_Compilation_Unit_Range'First .. Ada_Compilation_Unit_Range'Last =>
         --   This.Add_Not_Implemented;
         -- when Ada_Component_Clause_Range'First .. Ada_Component_Clause_Range'Last =>
         --   This.Add_Not_Implemented;
         -- when Ada_Component_Def_Range'First .. Ada_Component_Def_Range'Last =>
         --   This.Add_Not_Implemented;
         -- when Ada_Constraint'First .. Ada_Constraint'Last =>
         --   This.Add_Not_Implemented;
         -- when Ada_Constant_Node'First .. Ada_Constant_Node'Last =>
         --   This.Add_Not_Implemented;
         -- when Ada_Declarative_Part_Range'First .. Ada_Declarative_Part_Range'Last =>
         --   This.Add_Not_Implemented;
         -- when Ada_Elsif_Expr_Part_Range'First .. Ada_Elsif_Expr_Part_Range'Last =>
         --   This.Add_Not_Implemented;
         -- when Ada_Elsif_Stmt_Part_Range'First .. Ada_Elsif_Stmt_Part_Range'Last =>
         --   This.Add_Not_Implemented;
         --  Moved to top:
         --  when Ada_Expr'First .. Ada_Expr'Last =>
         -- when Ada_Handled_Stmts_Range'First .. Ada_Handled_Stmts_Range'Last =>
         --   This.Add_Not_Implemented;
         -- when Ada_Interface_Kind'First .. Ada_Interface_Kind'Last =>
         --   This.Add_Not_Implemented;
         -- when Ada_Iter_Type'First .. Ada_Iter_Type'Last =>
         --   This.Add_Not_Implemented;
         -- when Ada_Library_Item_Range'First .. Ada_Library_Item_Range'Last =>
         --   This.Add_Not_Implemented;
         -- when Ada_Limited_Node'First .. Ada_Limited_Node'Last =>
         --   This.Add_Not_Implemented;
         -- when Ada_Loop_Spec'First .. Ada_Loop_Spec'Last =>
         --   This.Add_Not_Implemented;
         -- when Ada_Mode'First .. Ada_Mode'Last =>
         --   This.Add_Not_Implemented;
         -- when Ada_Not_Null'First .. Ada_Not_Null'Last =>
         --   This.Add_Not_Implemented;
         -- when Ada_Null_Component_Decl_Range'First .. Ada_Null_Component_Decl_Range'Last =>
         --    This.Add_Not_Implemented;
         -- when Ada_Others_Designator_Range'First .. Ada_Others_Designator_Range'Last =>
         --    This.Add_Not_Implemented;
         -- when Ada_Overriding_Node'First .. Ada_Overriding_Node'Last =>
         --    This.Add_Not_Implemented;
         -- when Ada_Params_Range'First .. Ada_Params_Range'Last =>
         --    This.Add_Not_Implemented;
         -- when Ada_Pragma_Node_Range'First .. Ada_Pragma_Node_Range'Last =>
         --    This.Add_Not_Implemented;
         -- when Ada_Prim_Type_Accessor_Range'First .. Ada_Prim_Type_Accessor_Range'Last =>
         --    This.Add_Not_Implemented;
         -- when Ada_Private_Node'First .. Ada_Private_Node'Last =>
         --    This.Add_Not_Implemented;
         -- when Ada_Protected_Node'First .. Ada_Protected_Node'Last =>
         --    This.Add_Not_Implemented;
         -- when Ada_Protected_Def_Range'First .. Ada_Protected_Def_Range'Last =>
         --    This.Add_Not_Implemented;
         -- when Ada_Quantifier'First .. Ada_Quantifier'Last =>
         --    This.Add_Not_Implemented;
         -- when Ada_Range_Spec_Range'First .. Ada_Range_Spec_Range'Last =>
         --    This.Add_Not_Implemented;
        --  when Ada_Renaming_Clause_Range'First .. Ada_Renaming_Clause_Range'Last =>
         --    This.Add_Not_Implemented;
         -- when Ada_Reverse_Node'First .. Ada_Reverse_Node'Last =>
         --   This.Add_Not_Implemented;
         -- when Ada_Select_When_Part_Range'First .. Ada_Select_When_Part_Range'Last =>
         --   This.Add_Not_Implemented;
         -- when Ada_Subp_Kind'First .. Ada_Subp_Kind'Last =>
         --    This.Add_Not_Implemented;
         -- when Ada_Subunit_Range'First .. Ada_Subunit_Range'Last =>
         --    This.Add_Not_Implemented;
         --  Moved to top:
         -- when Ada_Stmt'First .. Ada_Stmt'Last =>
         -- when Ada_Synchronized_Node'First .. Ada_Synchronized_Node'Last =>
         --    This.Add_Not_Implemented;
         -- when Ada_Tagged_Node'First .. Ada_Tagged_Node'Last =>
         --    This.Add_Not_Implemented;
         --  Moved to top:
         --  when Ada_Type_Def'First .. Ada_Type_Def'Last =>
         -- when Ada_Task_Def_Range'First .. Ada_Task_Def_Range'Last =>
         --   This.Add_Not_Implemented;
         -- when Ada_Type_Expr'First .. Ada_Type_Expr'Last =>
         --    This.Add_Not_Implemented;
         -- when Ada_Unconstrained_Array_Index_Range'First .. Ada_Unconstrained_Array_Index_Range'Last =>
         --   This.Add_Not_Implemented;
         -- when Ada_Until_Node'First .. Ada_Until_Node'Last =>
         --    This.Add_Not_Implemented;
         -- when Ada_Use_Clause'First .. Ada_Use_Clause'Last =>
         --    This.Add_Not_Implemented;
         -- when Ada_Variant_Range'First .. Ada_Variant_Range'Last =>
         --    This.Add_Not_Implemented;
         -- when Ada_Variant_Part_Range'First .. Ada_Variant_Part_Range'Last =>
         --    This.Add_Not_Implemented;
         -- when Ada_With_Clause_Range'First .. Ada_With_Clause_Range'Last =>
         --    This.Add_Not_Implemented;
         -- when Ada_With_Private'First .. Ada_With_Private'Last =>
         --    This.Add_Not_Implemented;
      end case;
      Finish_Output;
      --  end if;
   exception
      when X : External_Error | Internal_Error | Usage_Error =>
         raise;
      when X: others =>
         Log_Exception (X);
         Log ("No handler for this exception.  Raising Internal_Error");
         raise Internal_Error;
   end Do_Pre_Child_Processing;

   procedure Do_Post_Child_Processing
     (This : in out Class) is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Do_Post_Child_Processing";
      package Logging is new Generic_Logging (Module_Name); use Logging;
   --     Auto : Logging.Auto_Logger; -- Logs BEGIN and END
   begin -- Do_Post_Child_Processing
      This.Outputs.Text.End_Line;
      This.Outputs.Text.Dedent;
      This.Outputs.Text.Put_Indented_Line
        (String'("END " & To_String (This.Element_IDs.First_Element)));
      This.Element_IDs.Delete_First;
   exception
      when X : External_Error | Internal_Error | Usage_Error =>
         raise;
      when X: others =>
         Log_Exception (X);
         Log ("No handler for this exception.  Raising Internal_Error");
         raise Internal_Error;
   end Do_Post_Child_Processing;

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
   begin
      This.Outputs := Outputs;
      Do_Pre_Child_Processing (This, Node);
      Do_Post_Child_Processing (This);
   end Process;

end Lal_Adapter.Node;
