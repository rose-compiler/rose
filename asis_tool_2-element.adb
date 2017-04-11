with Ada.Characters.Handling;
with Ada.Wide_Text_IO;
with Asis.Clauses;
with Asis.Declarations;
with Asis.Elements;
with Asis.Expressions;
with Asis.Iterator;

package body Asis_Tool_2.Element is

   -- Processing of current element aborted.  Processing of remaining elements
   -- may proceed:
   Not_Implemented : exception;

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
        (Element :        Asis.Element;
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
         State.Add_Attribute
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
         State.Add_Attribute (Name => "Defining_Name_Kind",
                              Value => Defining_Name_Kind'Image);
         State.Add_Attribute (Name => "Name",
                              Value => Name (Element));
         case Defining_Name_Kind is
            when Asis.A_Defining_Operator_Symbol =>
               State.Add_Attribute (Name => "Operator_Kind",
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
         State.Add_Attribute (Name => "Declaration_Kind",
                              Value => Declaration_Kind'Image);
         State.Add_Attribute (Name => "Declaration_Origin",
                              Value => Asis.Elements.Declaration_Origin (Element)'Image);
         case Declaration_Kind is
            when A_Parameter_Specification |
                 A_Formal_Object_Declaration =>
               State.Add_Attribute (Name => "Mode_Kind",
                                    Value => Asis.Elements.Mode_Kind (Element)'Image);
            when others =>
               null;
         end case;
         case Declaration_Kind is
            when A_Formal_Function_Declaration |
                 A_Formal_Procedure_Declaration =>
               State.Add_Attribute (Name => "Subprogram_Default_Kind",
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
               State.Add_Attribute (Name => "Trait_Kind",
                                    Value => Asis.Elements.Trait_Kind (Element)'Image);
            when others =>
               null;
         end case;
      end Process_Declaration;

      procedure Process_Definition
        (Element : in     Asis.Element;
         State   : in out Class)
      is
      begin
         --        A_Definition          -> Definition_Kinds
         --                                         -> Trait_Kinds
         --                                         -> Type_Kinds
         --                                         -> Formal_Type_Kinds
         --                                         -> Access_Type_Kinds
         --                                         -> Root_Type_Kinds
         --                                         -> Constraint_Kinds
         --                                         -> Discrete_Range_Kinds
         State.Add_Not_Implemented;
      end Process_Definition;

      procedure Process_Expression
        (Element : in     Asis.Element;
         State   : in out Class)
      is
         Expression_Kind : constant Asis.Expression_Kinds :=
           Asis.Elements.Expression_Kind (Element);
         use all type Asis.Expression_Kinds;
      begin
         --        An_Expression         -> Expression_Kinds
         --                                         -> Operator_Kinds
         --                                         -> Attribute_Kinds
         --
         State.Add_Attribute ("Expression_Kind", Expression_Kind'Image);
         -- Kind ordering and grouping is from Asis.Expression_Kinds.
         -- "when" comment is Ada LRM section.
         case Expression_Kind is
            when Not_An_Expression =>                         -- An unexpected element
               raise Program_Error with "Element.Pre_Children.Process_Expression called with: " & Expression_Kind'Image;

            when A_Box_Expression =>                          --  Ada 2005
                                                              --  4.3.1(4), 4.3.3(3,6)
               null;
               -- No more info.

            when An_Integer_Literal |                         -- 2.4
                 A_Real_Literal |                             -- 2.4.1
                 A_String_Literal =>                          -- 2.6
               State.Add_Not_Implemented;
               -- Value_Image

            when An_Identifier |                              -- 4.1
                 An_Operator_Symbol |                         -- 4.1
                 A_Character_Literal |                        -- 4.1
                 An_Enumeration_Literal =>                    -- 4.1
               State.Add_Attribute ("Name_Image",
                                    '"' & Asis.Expressions.Name_Image (Element) & '"');
               -- Name_Image
               -- Corresponding_Name_Definition
               -- Corresponding_Name_Definition_List
               -- Corresponding_Name_Declaration
               -- Subpool_Name
               case Expression_Kind is
                  when An_Operator_Symbol =>
                     State.Add_Attribute ("Operator_Kind",
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
               State.Add_Attribute ("Attribute_Kind",
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
               null;
               -- No more info.

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
      begin
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
         use all type Asis.Statement_Kinds;
      begin
         --        A_Statement           -> Statement_Kinds
         --
         State.Add_Attribute ("Statement_Kind", Statement_Kind'Image);
         -- All Statements can have:
         -- Label_Names

         -- Kind ordering and grouping is from Asis.Statement_Kinds.
         -- "when" comment is Ada LRM section.
         case Statement_Kind is
            when Not_A_Statement =>
               raise Program_Error with "Element.Pre_Children.Process_Statement called with: " & Statement_Kind'Image;

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
               State.Add_Not_Implemented;
               -- Statement_Identifier
               -- Is_Name_Repeated
               -- Loop_Statements
            when A_While_Loop_Statement =>              -- 5.5
               State.Add_Not_Implemented;
               -- Statement_Identifier
               -- While_Condition
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
         State.Add_Attribute ("Clause_Kind", Clause_Kind'Image);
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

      ------------
      -- EXPORTED:
      ------------
      procedure Process_Element
        (Element :        Asis.Element;
         Control : in out Asis.Traverse_Control;
         State   : in out Class)
      is
         Element_Kind : constant Asis.Element_Kinds :=
           Asis.Elements.Element_Kind (Element);
         New_Node : Dot.Node_Stmt.Class; -- Initialized
      begin
         State.Current_Node := New_Node;
         State.Add_Attribute ("Element_Kind", Element_Kind'Image);
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
         State.Text.End_Line;
         State.Text.Indent;
         State.Graph.Append_Stmt (new Dot.Node_Stmt.Class'(State.Current_Node));
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
         -- TODO: Add edges?
         State.Text.Dedent;
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
      Graph   : in     Dot.Graphs.Access_Class)
   is
      Process_Control : Asis.Traverse_Control := Asis.Continue;
   begin
      This.The_Element := Element;
      This.Graph := Graph;
      Traverse_Element
        (Element => Element,
         Control => Process_Control,
         State   => This);
   end Process_Element_Tree;

   -----------
   -- PRIVATE:
   -----------
   procedure Add_Attribute
     (This  : in out Class;
      Name  : in     String;
      Value : in     String)
   is
   begin
      This.Text.Put_Indented_Line (Name & " => """ & Value & """");
      This.Current_Node.Attrs.Add_Assign_To_First_Attr
        (Name  => Name,
         Value => Value);
   end;

   -----------
   -- PRIVATE:
   -----------
   procedure Add_Attribute
     (This  : in out Class;
      Name  : in     String;
      Value : in     Wide_String)
   is
   begin
      This.Add_Attribute (Name, To_String (Value));
   end;

   procedure Add_Not_Implemented
     (This  : in out Class)
   is
   begin
      This.Add_Attribute ("Traversal", String'("***NOT_IMPLEMENTED***"));
   end Add_Not_Implemented;


end Asis_Tool_2.Element;
