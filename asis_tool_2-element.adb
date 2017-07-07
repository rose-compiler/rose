with Asis.Compilation_Units;
with Asis.Declarations;
with Asis.Elements;
with Asis.Expressions;
with Asis.Iterator;
with Asis.Statements;
-- GNAT-specific:
with Asis.Set_Get;
with Asis.Text;
with Types;

package body Asis_Tool_2.Element is

   -- Return the name image for declarations:
   function Name
     (Element : in Asis.Element)
      return Wide_String
   is
   begin
      case Asis.Elements.Element_Kind (Element) is
         when Asis.A_Defining_Name =>
            return '"' & Asis.Declarations.Defining_Name_Image (Element) & '"';
         when others =>
            return "";
      end case;
   end Name;


   package Pre_Children is

      procedure Process_Element
        (Element : in     Asis.Element;
         Control : in out Asis.Traverse_Control;
         State   : in out Class);

   end Pre_Children;

   package body Pre_Children is

      procedure Process_Pragma
        (Element : in     Asis.Element;
         State   : in out Class)
      is
         Pragma_Kind : Asis.Pragma_Kinds :=
           Asis.Elements.Pragma_Kind (Element);
      begin
         --        A_Pragma              -> Pragma_Kinds
         --
         State.Add_To_Dot_Label
           (Name => "Pragma_Kind",
            Value => Pragma_Kind'Image);
         State.Add_Not_Implemented;
      end Process_Pragma;

      procedure Process_Defining_Name
        (Element : in     Asis.Element;
         State   : in out Class)
      is
         Defining_Name_Kind : Asis.Defining_Name_Kinds :=
           Asis.Elements.Defining_Name_Kind (Element);
      begin
         --        A_Defining_Name       -> Defining_Name_Kinds
         --                                         -> Operator_Kinds
         State.Add_To_Dot_Label (Name => "Defining_Name_Kind",
                              Value => Defining_Name_Kind'Image);
         State.Add_To_Dot_Label (Name => "Name",
                              Value => Name (Element));
         case Defining_Name_Kind is
            when Asis.A_Defining_Operator_Symbol =>
               State.Add_To_Dot_Label (Name => "Operator_Kind",
                                    Value => Asis.Elements.Operator_Kind (Element)'Image);
            when others => null;
         end case;
      end Process_Defining_Name;

      procedure Process_Declaration
        (Element : in     Asis.Element;
         State   : in out Class)
      is
         Declaration_Kind : Asis.Declaration_Kinds :=
           Asis.Elements.Declaration_Kind (Element);
         -- Tired of typing "Asis." in front of enum values:
         use Asis;
      begin
         --        A_Declaration         -> Declaration_Kinds
         --                                         -> Declaration_Origin
         --                                         -> Mode_Kinds
         --                                         -> Subprogram_Default_Kinds
         State.Add_To_Dot_Label (Name => "Declaration_Kind",
                              Value => Declaration_Kind'Image);
         State.Add_To_Dot_Label (Name => "Declaration_Origin",
                              Value => Asis.Elements.Declaration_Origin (Element)'Image);
         case Declaration_Kind is
            when A_Parameter_Specification |
                 A_Formal_Object_Declaration =>
               State.Add_To_Dot_Label (Name => "Mode_Kind",
                                    Value => Asis.Elements.Mode_Kind (Element)'Image);
            when others =>
               null;
         end case;
         case Declaration_Kind is
            when A_Formal_Function_Declaration |
                 A_Formal_Procedure_Declaration =>
               State.Add_To_Dot_Label (Name => "Subprogram_Default_Kind",
                                    Value => Asis.Elements.Default_Kind (Element)'Image);
            when others =>
               null;
         end case;
         case Declaration_Kind is
            when A_Private_Type_Declaration |
                 A_Private_Extension_Declaration |
                 A_Variable_Declaration |
                 A_Constant_Declaration |
                 A_Deferred_Constant_Declaration |
                 A_Discriminant_Specification |
                 A_Loop_Parameter_Specification |
                 A_Generalized_Iterator_Specification |
                 An_Element_Iterator_Specification |
                 A_Procedure_Declaration |
                 A_Function_Declaration =>
               State.Add_To_Dot_Label (Name => "Trait_Kind",
                                    Value => Asis.Elements.Trait_Kind (Element)'Image);
            when others =>
               null;
         end case;
      end Process_Declaration;

      procedure Process_Definition
        (Element : in     Asis.Element;
         State   : in out Class)
      is
         Definition_Kind : Asis.Definition_Kinds :=
           Asis.Elements.Definition_Kind (Element);
         A_Definition : a_nodes_h.Definition_Struct :=
           a_nodes_h.Support.Default_Definition_Struct;
         -- Tired of typing "Asis." in front of enum values:
         use all type Asis.Definition_Kinds;
      begin
         State.Add_To_Dot_Label ("Definition_Kind", Definition_Kind'Image);
         A_Definition.kind := anhS.To_Definition_Kinds (Definition_Kind);

         case Definition_Kind is
            when A_Type_Definition =>
--                 declare
--                    Type_Kind : constant Asis.Type_Kinds :=
--                      Asis.Elements.Type_Kind (A_Definition);
--                 begin
--                    State.Add_To_Dot_Label ("Type_Kind", Type_Kind'Image);
--                    A_Definition.type_kind := anhS.To_Type_Kinds (Type_Kind);
--                 end;
               State.Add_Not_Implemented;
            when A_Constraint =>
               -- Constraint_Kinds
               State.Add_Not_Implemented;
            when A_Component_Definition |
                 A_Private_Type_Definition |
                 A_Tagged_Private_Type_Definition |
                 A_Private_Extension_Definition =>
               -- Trait_Kinds
               State.Add_Not_Implemented;
            when A_Discrete_Subtype_Definition |
                 A_Discrete_Range =>
               -- Discrete_Range_Kinds
               State.Add_Not_Implemented;
            when An_Access_Definition =>
               -- Access_Definition_Kinds
               State.Add_Not_Implemented;
            when A_Formal_Type_Definition =>
               -- Formal_Type_Kinds
               State.Add_Not_Implemented;
            when others =>
               null;
               -- TODO: Root_Type_Kinds??
         end case;

         --        A_Definition          -> Definition_Kinds
         --                                         -> Trait_Kinds
         --                                         -> Type_Kinds
         --                                         -> Formal_Type_Kinds
         --                                         -> Access_Type_Kinds
         --                                         -> Root_Type_Kinds
         --                                         -> Constraint_Kinds
         --                                         -> Discrete_Range_Kinds
      end Process_Definition;

      procedure Process_Expression
        (Element : in     Asis.Element;
         State   : in out Class)
      is
        Expression_Kind : Asis.Expression_Kinds :=
           Asis.Elements.Expression_Kind (Element);
         A_Expression : a_nodes_h.Expression_Struct :=
           a_nodes_h.Support.Default_Expression_Struct;
         use all type Asis.Expression_Kinds;
      begin
         State.Add_To_Dot_Label ("Expression_Kind", Expression_Kind'Image);
         A_Expression.kind := anhS.To_Expression_Kinds (Expression_Kind);
         --        An_Expression         -> Expression_Kinds
         --                                         -> Operator_Kinds
         --                                         -> Attribute_Kinds
         --
         -- Kind ordering and grouping is from Asis.Expression_Kinds.
         -- "when" comment is Ada LRM section.
         case Expression_Kind is
            when Not_An_Expression =>                         -- An unexpected element
               raise Program_Error with
                 "Element.Pre_Children.Process_Expression called with: " &
                 Expression_Kind'Image;
            when A_Box_Expression =>                          --  Ada 2005
                                                              --  4.3.1(4), 4.3.3(3,6)
               null;
               -- No more info.

            when An_Integer_Literal |                         -- 2.4
                 A_Real_Literal |                             -- 2.4.1
                 A_String_Literal =>                          -- 2.6
               declare
                  Value_Image : constant Wide_String :=
                    Asis.Expressions.Value_Image (Element);
               begin
                  State.Add_To_Dot_Label ("Value_Image", Value_Image);
                  A_Expression.value_image := To_Chars_Ptr (Value_Image);
               end;

            when An_Identifier |                              -- 4.1
                 An_Operator_Symbol |                         -- 4.1
                 A_Character_Literal |                        -- 4.1
                 An_Enumeration_Literal =>                    -- 4.1
               State.Add_To_Dot_Label ("Name_Image",
                                    '"' & Asis.Expressions.Name_Image (Element) & '"');
               -- Name_Image
               -- Corresponding_Name_Definition
               -- Corresponding_Name_Definition_List
               -- Corresponding_Name_Declaration
               -- Subpool_Name
               case Expression_Kind is
                  when An_Operator_Symbol =>
                     State.Add_To_Dot_Label ("Operator_Kind",
                                          Asis.Elements.Operator_Kind (Element)'Image);
                  when others =>
                     null;
               end case;
            when An_Explicit_Dereference =>                   -- 4.1
               State.Add_Not_Implemented;
               -- Prefix
            when A_Function_Call =>                           -- 4.1
               State.Add_Not_Implemented;
               -- Prefix
               -- Is_Prefix_Call
               -- Corresponding_Called_Function
               -- Function_Call_Parameters

            when An_Indexed_Component =>                      -- 4.1.1
               State.Add_Not_Implemented;
               -- Prefix
               -- Index_Expressions
               -- Corresponding_Called_Function (Is_Generalized_Indexing)  --ASIS 2012
               -- Is_Generalized_Indexing
            when A_Slice =>                                   -- 4.1.2
               State.Add_Not_Implemented;
               -- Prefix
               -- Slice_Range
            when A_Selected_Component =>                      -- 4.1.3
               State.Add_Not_Implemented;
               -- Prefix
               -- Selector
            when An_Attribute_Reference =>                    -- 4.1.4  -> Attribute_Kinds
               State.Add_To_Dot_Label ("Attribute_Kind",
                                    Asis.Elements.Attribute_Kind (Element)'Image);
               -- Prefix
               -- Attribute_Designator_Identifier
               -- Attribute_Designator_Expressions

            when A_Record_Aggregate =>                        -- 4.3
               State.Add_Not_Implemented;
               -- Record_Component_Associations
            when An_Extension_Aggregate =>                    -- 4.3
               State.Add_Not_Implemented;
               -- Record_Component_Associations
               -- Extension_Aggregate_Expression
            when A_Positional_Array_Aggregate |               -- 4.3
                 A_Named_Array_Aggregate =>                   -- 4.3
               State.Add_Not_Implemented;
               -- Array_Component_Associations

            when An_And_Then_Short_Circuit |                  -- 4.4
                 An_Or_Else_Short_Circuit =>                  -- 4.4
               State.Add_Not_Implemented;
               -- Short_Circuit_Operation_Left_Expression
               -- Short_Circuit_Operation_Right_Expression

            when An_In_Membership_Test |                      -- 4.4  Ada 2012
                 A_Not_In_Membership_Test =>                  -- 4.4  Ada 2012
               State.Add_Not_Implemented;
               -- Membership_Test_Expression
               -- Membership_Test_Choices

            when A_Null_Literal =>                            -- 4.4
               null;
               -- No more info.
            when A_Parenthesized_Expression =>                -- 4.4
               State.Add_Not_Implemented;
               -- Expression_Parenthesized
            when A_Raise_Expression =>                        -- 4.4 Ada 2012 (AI12-0022-1)
               -- No subprograms in Asis.Elements for this element type.  Should we call something else?
               State.Add_Not_Implemented;

            when A_Type_Conversion =>                         -- 4.6
               State.Add_Not_Implemented;
               -- Converted_Or_Qualified_Subtype_Mark
               -- Converted_Or_Qualified_Expression
            when A_Qualified_Expression =>                    -- 4.7
               State.Add_Not_Implemented;
               -- Converted_Or_Qualified_Subtype_Mark
               -- Converted_Or_Qualified_Expression
               -- Predicate

            when An_Allocation_From_Subtype =>                -- 4.8
               State.Add_Not_Implemented;
               -- Allocator_Subtype_Indication
               -- Subpool_Name
            when An_Allocation_From_Qualified_Expression =>   -- 4.8
               State.Add_Not_Implemented;
               -- Allocator_Qualified_Expression
               -- Subpool_Name
            when A_Case_Expression |                          -- Ada 2012
                 An_If_Expression =>                          -- Ada 2012
               State.Add_Not_Implemented;
               -- Expression_Paths
            when A_For_All_Quantified_Expression |            -- Ada 2012
                 A_For_Some_Quantified_Expression =>          -- Ada 2012
               State.Add_Not_Implemented;
               -- Iterator_Specification
         end case;
      end Process_Expression;

      procedure Process_Association
        (Element : in     Asis.Element;
         State   : in out Class)
      is
         Association_Kind : Asis.Association_Kinds :=
           Asis.Elements.Association_Kind (Element);
         A_Association : a_nodes_h.Association_Struct :=
           a_nodes_h.Support.Default_Association_Struct;
      begin
         State.Add_To_Dot_Label ("Association_Kind", Association_Kind'Image);
         A_Association.kind := anhS.To_Association_Kinds (Association_Kind);

--    // An_Array_Component_Association,        // 4.3.3
--    Expression_List Array_Component_Choices;
--    // A_Record_Component_Association,        // 4.3.1
--    Expression_List Record_Component_Choices;
--    // An_Array_Component_Association,        // 4.3.3
--    // A_Record_Component_Association,        // 4.3.1
--    Expression_ID   Component_Expression;
--    // A_Pragma_Argument_Association,         // 2.8
--    // A_Parameter_Association,               // 6.4
--    // A_Generic_Association                  // 12.3
--    Expression_ID   Formal_Parameter;
--    Expression_ID   Actual_Parameter;
--    // A_Discriminant_Association,            // 3.7.1
--    Expression_List Discriminant_Selector_Names;
--    Expression_ID   Discriminant_Expression;
--    // A_Discriminant_Association,            // 3.7.1
--    // A_Record_Component_Association,        // 4.3.1
--    // A_Parameter_Association,               // 6.4
--    // A_Generic_Association                  // 12.3
--    bool            Is_Normalized;
--    // A_Parameter_Association
--    // A_Generic_Association
--    //  //|A2005 start
--    // A_Record_Component_Association
--    //  //|A2005 end
--    bool            Is_Defaulted_Association;



         State.A_Element.kind := a_nodes_h.An_Association;
         State.A_Element.the_union.association := A_Association;
         --        An_Association        -> Association_Kinds
         --
         State.Add_Not_Implemented;
      end Process_Association;

      procedure Process_Statement
        (Element : in     Asis.Element;
         State   : in out Class)
      is
         Statement_Kind : constant Asis.Statement_Kinds :=
           Asis.Elements.Statement_Kind (Element);
         A_Statement : a_nodes_h.Statement_Struct :=
           a_nodes_h.Support.Default_Statement_Struct;
         use all type Asis.Statement_Kinds;
      begin
         --        A_Statement           -> Statement_Kinds
         --
         State.Add_To_Dot_Label ("Statement_Kind", Statement_Kind'Image);
         A_Statement.kind := anhS.To_Statement_Kinds (Statement_Kind);

         -- All Statements can have:
         -- Label_Names

         -- Kind ordering and grouping is from Asis.Statement_Kinds.
         -- "when" comment is Ada LRM section.
         case Statement_Kind is
            when Not_A_Statement =>
               raise Program_Error with
                 "Element.Pre_Children.Process_Statement called with: " &
                 Statement_Kind'Image;
            when A_Null_Statement =>                    -- 5.1
               null;
               -- No more info.
            when An_Assignment_Statement =>             -- 5.2
               State.Add_Not_Implemented;
               -- Assignment_Variable_Name
               -- Assignment_Expression
            when An_If_Statement =>                     -- 5.3
               State.Add_Not_Implemented;
               -- Statement_Paths
            when A_Case_Statement =>                    -- 5.4
               State.Add_Not_Implemented;
               -- Case_Expression
            when A_Loop_Statement =>                    -- 5.5
               declare
               Statement_Identifier : constant Asis.Defining_Name :=
                    Asis.Statements.Statement_Identifier (Element);
                  Statement_Identifier_ID : constant Types.Node_Id :=
                    Asis.Set_Get.Node (Statement_Identifier);
               -- While_Condition
               -- Loop_Statements
               begin
                  State.Add_To_Dot_Label
                    ("Statement_Identifier", To_String (Statement_Identifier_ID));
                  A_Statement.Statement_Identifier := a_nodes_h.Node_ID
                    (Statement_Identifier_ID);
                  end;
               State.Add_Not_Implemented;
            when A_While_Loop_Statement =>              -- 5.5
               State.Add_Not_Implemented;
               -- Statement_Identifier
               -- Is_Name_Repeated
               -- Loop_Statements
            when A_For_Loop_Statement =>                -- 5.5
               State.Add_Not_Implemented;
               -- Statement_Identifier
               -- For_Loop_Parameter_Specification
               -- Loop_Statements
            when A_Block_Statement =>                   -- 5.6
               State.Add_Not_Implemented;
               -- Statement_Identifier
               -- Is_Name_Repeated
               -- Is_Declare_Block
               -- Block_Declarative_Items
               -- Block_Statements
               -- Block_Exception_Handlers
            when An_Exit_Statement =>                   -- 5.7
               State.Add_Not_Implemented;
               -- Exit_Loop_Name
               -- Exit_Condition
               -- Corresponding_Loop_Exited
            when A_Goto_Statement =>                    -- 5.8
               State.Add_Not_Implemented;
               -- Goto_Label
               -- Corresponding_Destination_Statement

            when A_Procedure_Call_Statement =>          -- 6.4
               State.Add_Not_Implemented;
               -- Called_Name
               -- Corresponding_Called_Entity
               -- Call_Statement_Parameters
            when A_Return_Statement =>                  -- 6.5
               State.Add_Not_Implemented;
               -- Return_Expression
            when An_Extended_Return_Statement =>        -- 6.5 (Ada 2005)
               State.Add_Not_Implemented;
               -- Return_Object_Declaration
               -- Extended_Return_Statements
               -- Extended_Return_Exception_Handlers

            when An_Accept_Statement =>                 -- 9.5.2
               State.Add_Not_Implemented;
               -- Accept_Entry_Index
               -- Accept_Entry_Direct_Name
               -- Accept_Parameters
               -- Accept_Body_Statements
               -- Accept_Body_Exception_Handlers
               -- Corresponding_Entry
            when An_Entry_Call_Statement =>             -- 9.5.3
               State.Add_Not_Implemented;
               -- Called_Name
               -- Corresponding_Called_Entity
               -- Call_Statement_Parameters

            when A_Requeue_Statement =>                 -- 9.5.4
               State.Add_Not_Implemented;
               -- Requeue_Entry_Name
            when A_Requeue_Statement_With_Abort =>      -- 9.5.4
               State.Add_Not_Implemented;
               -- Requeue_Entry_Name

            when A_Delay_Until_Statement =>             -- 9.6
               State.Add_Not_Implemented;
               -- Delay_Expression
            when A_Delay_Relative_Statement =>          -- 9.6
               State.Add_Not_Implemented;
               -- Delay_Expression

            when A_Terminate_Alternative_Statement =>   -- 9.7.1
               null;
               -- No more info.
            when A_Selective_Accept_Statement =>        -- 9.7.1
               State.Add_Not_Implemented;
               -- Statement_Paths
            when A_Timed_Entry_Call_Statement =>        -- 9.7.2
               State.Add_Not_Implemented;
               -- Statement_Paths
            when A_Conditional_Entry_Call_Statement =>  -- 9.7.3
               State.Add_Not_Implemented;
               -- Statement_Paths
            when An_Asynchronous_Select_Statement =>    -- 9.7.4
               State.Add_Not_Implemented;
               -- Statement_Paths

            when An_Abort_Statement =>                  -- 9.8
               State.Add_Not_Implemented;
               -- Aborted_Tasks
            when A_Raise_Statement =>                   -- 11.3
               State.Add_Not_Implemented;
               -- Raised_Exception
               -- Associated_Message
            when A_Code_Statement =>                    -- 13.8
               State.Add_Not_Implemented;
               -- Qualified_Expression
         end case;
      end Process_Statement;

      procedure Process_Path
        (Element : in     Asis.Element;
         State   : in out Class)
      is
      begin
         --        A_Path                -> Path_Kinds
         --
         State.Add_Not_Implemented;
      end Process_Path;

      procedure Process_Clause
        (Element : in     Asis.Element;
         State   : in out Class)
      is
         Clause_Kind : constant Asis.Clause_Kinds :=
           Asis.Elements.Clause_Kind (Element);
      begin
         --        A_Clause              -> Clause_Kinds
         --                                         -> Representation_Clause_Kinds
         State.Add_To_Dot_Label ("Clause_Kind", Clause_Kind'Image);
         case Clause_Kind is
            when Asis.Not_A_Clause =>
               raise Program_Error with "Element.Pre_Children.Process_Clause called with: " & Clause_Kind'Image;
            when Asis.A_Use_Package_Clause =>
               State.Add_Not_Implemented;
            when Asis.A_Use_Type_Clause =>
               State.Add_Not_Implemented;
            when Asis.A_Use_All_Type_Clause =>
               State.Add_Not_Implemented;
            when Asis.A_With_Clause =>
               State.Add_Not_Implemented;
            when Asis.A_Representation_Clause =>
               State.Add_Not_Implemented;
            when Asis.A_Component_Clause =>
               State.Add_Not_Implemented;
         end case;
      end Process_Clause;

      procedure Process_Exception_Handler
        (Element : in     Asis.Element;
         State   : in out Class)
      is
      begin
         --        An_Exception_Handler
         State.Add_Not_Implemented;
      end Process_Exception_Handler;

      function Get_Enclosing_ID
        (Element : in Asis.Element)
         return Dot.ID_Type
      is
         Result : Dot.ID_Type; -- Initilaized
         Enclosing_Element_Id : constant Types.Node_Id :=
           Asis.Set_Get.Node_Value (Asis.Elements.Enclosing_Element (Element));
         Enclosing_Unit_Id : constant A4G.A_Types.Unit_Id :=
           Asis.Set_Get.Encl_Unit_Id (Element);
         function Enclosing_Is_Element return boolean
           is (Types."/=" (Enclosing_Element_Id, Types.Empty));
      begin
         if Enclosing_Is_Element then
            Result := To_Dot_ID_Type (Enclosing_Element_Id);
         else
            Result := To_Dot_ID_Type (Enclosing_Unit_Id);
         end if;
         return Result;
      end Get_Enclosing_ID;

      function Get_Enclosing_ID
        (Element : in Asis.Element)
         return a_nodes_h.Node_ID is
      begin
         return a_nodes_h.Node_ID
           (Asis.Set_Get.Node_Value
              (Asis.Elements.Enclosing_Element (Element)));
      end Get_Enclosing_ID;

      function Source_Location_Image
        (Element : in Asis.Element)
         return String
      is
         Unit : constant Asis.Compilation_Unit :=
           Asis.Elements.Enclosing_Compilation_Unit (Element);
         Unit_Name : constant String := To_String
           (Asis.Compilation_Units.Unit_Full_Name (Unit));
         Unit_Class : constant Asis.Unit_Classes := Asis.Compilation_Units.Unit_Class (Unit);
         Span : constant Asis.Text.Span := Asis.Text.Element_Span (Element);
         function Spec_Or_Body return String is
            use all type Asis.Unit_Classes;
         begin
            case Unit_Class is
               when Not_A_Class =>
                  return "()";
               when A_Public_Declaration |
                    A_Private_Declaration =>
                  return "(spec)";
               when A_Public_Body |
                    A_Private_Body =>
                  return "(body)";
               when A_Public_Declaration_And_Body =>
                  return "(spec and body)";
               when A_Separate_Body =>
                  return "(separate body)";
            end case;
         end Spec_Or_Body;
      begin
         return Unit_Name & Spec_Or_Body & ":" &
           NLB_Image (Span.First_Line) & ":" & NLB_Image (Span.First_Column) &
           ".." &
           NLB_Image (Span.Last_Line) & ":" & NLB_Image (Span.Last_Column);
      end Source_Location_Image;

      ------------
      -- EXPORTED:
      ------------
      procedure Process_Element
        (Element : in     Asis.Element;
         Control : in out Asis.Traverse_Control;
         State   : in out Class)
      is
         Element_Kind : constant Asis.Element_Kinds :=
           Asis.Elements.Element_Kind (Element);
         Element_Id   : constant Types.Node_Id := Asis.Set_Get.Node (Element);

         procedure Start_Output
         is
            Default_Node  : Dot.Node_Stmt.Class; -- Initialized
            Default_Label : Dot.HTML_Like_Labels.Class; -- Initialized
         begin
            State.Text.Indent;
            State.Text.End_Line;
            State.Dot_Node := Default_Node;
            State.Dot_Label := Default_Label;
            State.A_Element := a_nodes_h.Support.Default_Element_Struct;

            State.Dot_Node.Node_ID.ID := To_Dot_ID_Type (Element_Id);
            State.A_Element.id := a_nodes_h.Node_ID (Element_Id);

            State.Add_To_Dot_Label ("Element_Kind", Element_Kind'Image);
            State.A_Element.kind := anhS.To_Element_Kinds (Element_Kind);

            State.Add_To_Dot_Label ("ID", To_String (Element_Id));
            -- ID is in the Dot node twice, but not in the a_node.

            State.Add_To_Dot_Label ("Source", Source_Location_Image (Element));
            State.A_Element.source_location :=
              To_Chars_Ptr (Source_Location_Image (Element));
         end;

         procedure Add_Enclosing_Edge
         is
            Edge_Stmt : Dot.Edges.Stmts.Class; -- Initialized
            Enclosing_Element : constant Asis.Element :=
              Asis.Elements.Enclosing_Element (Element);
            Enclosing_Element_Id : constant Types.Node_Id :=
              Asis.Set_Get.Node (Enclosing_Element);
--    enum Enclosing_Kinds   enclosing_kind;
         begin
            Edge_Stmt.LHS.Node_Id.ID := To_Dot_ID_Type (Enclosing_Element_Id);
            State.A_Element.enclosing_id := a_nodes_h.Node_ID (Enclosing_Element_Id);

            Edge_Stmt.RHS.Node_Id.ID := To_Dot_ID_Type (Element_Id);

            State.Outputs.Graph.Append_Stmt (new Dot.Edges.Stmts.Class'(Edge_Stmt));
         end;

         procedure Finish_Output
         is
            A_Node    : a_nodes_h.Node_Struct := anhS.Default_Node_Struct;
         begin
            Add_Enclosing_Edge;
            State.Dot_Node.Add_Label (State.Dot_Label);

            State.Outputs.Graph.Append_Stmt
              (new Dot.Node_Stmt.Class'(State.Dot_Node));

            A_Node.kind := a_nodes_h.An_Element_Node;
            A_Node.the_union.element := State.A_Element;
            State.Outputs.A_Nodes.Push (A_Node);

            State.Text.End_Line;
            State.Text.Dedent;
         end;

      begin
         Start_Output;
         case Element_Kind is
         when Asis.Not_An_Element =>
            Null;
         when Asis.A_Pragma =>
            Process_Pragma (Element, State);
         when Asis.A_Defining_Name =>
            Process_Defining_Name (Element, State);
         when Asis.A_Declaration =>
            Process_Declaration (Element, State);
         when Asis.A_Definition =>
            Process_Definition (Element, State);
         when Asis.An_Expression =>
            Process_Expression (Element, State);
         when Asis.An_Association =>
            Process_Association (Element, State);
         when Asis.A_Statement =>
            Process_Statement (Element, State);
         when Asis.A_Path =>
            Process_Path (Element, State);
         when Asis.A_Clause =>
            Process_Clause (Element, State);
         when Asis.An_Exception_Handler =>
            Process_Exception_Handler (Element, State);
         end case;
         Finish_Output;
      end Process_Element;

   end Pre_Children;


   package Post_Children is

      procedure Process_Element
        (Element :        Asis.Element;
         Control : in out Asis.Traverse_Control;
         State   : in out Class);

   end Post_Children;

   package body Post_Children is

      ------------
      -- EXPORTED:
      ------------
      procedure Process_Element
        (Element :        Asis.Element;
         Control : in out Asis.Traverse_Control;
         State   : in out Class) is
      begin
         Null;
      end Process_Element;

   end Post_Children;


   -- Call Pre_Operation on ths element, call Traverse_Element on all children,
   -- then call Post_Operation on this element:
   procedure Traverse_Element is new
     Asis.Iterator.Traverse_Element
       (State_Information => Class,
        Pre_Operation     => Pre_Children.Process_Element,
        Post_Operation    => Post_Children.Process_Element);

   ------------
   -- EXPORTED:
   ------------
   procedure Process_Element_Tree
     (This    : in out Class;
      Element : in     Asis.Element;
      Outputs : in     Output_Accesses_Record)
   is
      Process_Control : Asis.Traverse_Control := Asis.Continue;
   begin
      This.The_Element := Element;
      -- I like to just pass Outputs through and not store it in the object,
      -- since it is all pointers and we doesn't need to store their values
      -- between calls to Process_Element_Tree. Outputs has to go into
      -- State_Information in the Traverse_Element instatiation, though,
      -- so we'll put it in the object and pass that:
      This.Outputs := Outputs;
      Traverse_Element
        (Element => Element,
         Control => Process_Control,
         State   => This);
   end Process_Element_Tree;

   -----------
   -- PRIVATE:
   -----------
   procedure Add_To_Dot_Label
     (This  : in out Class;
      Name  : in     String;
      Value : in     String) is
   begin
-- Instead of this, put the "attribute" in the label:
--        This.Node.Attr_List.Add_Assign_To_First_Attr
--          (Name  => Name,
--           Value => Value);
      This.Dot_Label.Add_Eq_Row(L => Name, R => Value);
      This.Text.Put_Indented_Line (Name & " => """ & Value & """");
   end;

   -----------
   -- PRIVATE:
   -----------
   procedure Add_To_Dot_Label
     (This  : in out Class;
      Name  : in     String;
      Value : in     Wide_String) is
   begin
      This.Add_To_Dot_Label (Name, To_String (Value));
   end;

   procedure Add_Not_Implemented
     (This  : in out Class) is
   begin
      This.Add_To_Dot_Label ("ASIS_PROCESSING", String'("NOT_COMPLETELY_IMPLEMENTED"));
   end Add_Not_Implemented;


end Asis_Tool_2.Element;
