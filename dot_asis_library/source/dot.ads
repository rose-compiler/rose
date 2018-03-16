-- This package provides support for creating and printing graphs in the
-- DOT language, described at www.graphviz.org.  From the website:
--
-- The following is an abstract grammar defining the DOT language.  Literal
-- characters and keywords are given in single quotes. Parentheses ( and )
-- indicate grouping when needed. Square brackets [ and ] enclose optional items.
-- Vertical bars | separate alternatives.
--
-- graph        : ['strict'] ('graph'|'digraph') [ID] '{' stmt_list '}'
-- stmt_list    : [stmt [';'] stmt_list]
-- stmt         : node_stmt
--              | edge_stmt
--              | attr_stmt
-- 	        | ID '=' ID
--              | subgraph
-- attr_stmt    : ('graph'|'node'|'edge') attr_list
-- attr_list    : '[' [ a_list] ']' [attr_list]
-- a_list       : ID '=' ID [(';'|',')] [a_list]
-- edge_stmt    : (node_id | subgraph) edgeRHS [attr_list]
-- edgeRHS      : edgeop (node_id | subgraph) [edgeRHS]
-- node_stmt 	: node_id [attr_list]
-- node_id 	: ID [port]
-- port         : ':' ID [':'compass_pt]
--              | ':' compass_pt
-- subgraph     : ['subgraph' [ID]] '{' stmt_list '}'
-- compass_pt   : ('n'|'ne'|'e'|'se'|'s'|'sw'|'w'|'nw'|'c'|'_')
--
-- The keywords node, edge, graph, digraph, subgraph, and strict are
-- case-independent. Note also that the allowed compass point values are not
-- keywords, so these strings can be used elsewhere as ordinary identifiers and,
-- conversely, the parser will actually accept any identifier.
--
-- An ID is one of the following:
-- - Any string of alphabetic ([a-zA-Z\200-\377]) characters, underscores ('_')
--   or digits ([0-9]), not beginning with a digit
-- - a numeral [-]?(.[0-9]+ | [0-9]+(.[0-9]*)? )
-- - any double-quoted string ("...") possibly containing escaped quotes ("\"")
-- - an HTML string (<...>)
--
-- An ID is just a string; the lack of quote characters in the first two forms
-- is just for simplicity. There is no semantic difference between abc_2 and
-- "abc_2", or between 2.34 and "2.34". To use a keyword as an ID, it must be
-- quoted.

with Ada.Containers.Doubly_Linked_Lists;
with Ada.Strings.Unbounded;
with Ada.Text_IO;

package Dot is

   -- For convenience:
   package ATI renames Ada.Text_IO;

   -- Using the exact spellings from the grammar for the record components
   -- instead of spelling things out:

   type Compass_Pt_Type is (N, NE, E, SE, S, SW, W, NW, C, Underscore);

   type ID_Type is new Ada.Strings.Unbounded.Unbounded_String;
   function To_ID_Type (Item : in String) return ID_Type
                        renames To_Unbounded_String;
   --     function To_ID_Type (Item : in Wide_String) return ID_Type;

   -----------------------------------------------------------------------------
   package Stmt is

      type Class is abstract tagged null record;
      type Access_All_Class is access all Class'Class;

      procedure Put
        (This : in     Class;
         File : in ATI.File_Type) is abstract;

      package Lists is new
        Ada.Containers.Doubly_Linked_Lists (Access_All_Class);
      -- Make primitive operations like "=" visible:
      type List_Of_Access_All_Class is new Lists.List with null record;

      procedure Put
        (These : in List_Of_Access_All_Class;
         File  : in ATI.File_Type);

   end Stmt;
   -----------------------------------------------------------------------------

   -----------------------------------------------------------------------------
   -- Zero or more assigns:
   package Assign is

      type Class is tagged -- Initialized
      record
         L : ID_Type; -- Initialized
         R : ID_Type; -- Initialized
      end record;

      procedure Put
        (This : in Class;
         File : in ATI.File_Type);

      package Lists is new
        Ada.Containers.Doubly_Linked_Lists (Class);
      -- Make primitive operations like "=" visible:
      type List_Of_Class is new Lists.List with null record; -- Initialized

      procedure Put
        (These : in List_Of_Class;
         File  : in ATI.File_Type);

      -- Convenience: converts L, R to ID_Type and appends a Class to the list.
      -- Allows this:
      --      Assign_List.Append ("Today", "Thursday");
      -- Instead of this:
      --      Assign_List.Append ((L => Dot.To_ID_Type ("Today"),
      --                               R => Dot.To_ID_Type ("Thursday")));

      not overriding
      procedure Append
        (These : in out List_Of_Class;
         L, R  : in     String);

      function Empty_List return List_Of_Class;

   end Assign;
   -----------------------------------------------------------------------------

   -----------------------------------------------------------------------------
   -- Zero or more bracketed lists of assigns:
   package Attr is

      type Class is tagged -- Initialized
         record
            A_List : Assign.List_Of_Class; -- Initialized
         end record;

      Null_Class : constant Class;

      procedure Put
        (This : in Class;
         File : in ATI.File_Type);

      package Lists is new
        Ada.Containers.Doubly_Linked_Lists (Class);
      -- Make primitive operations like "=" visible:
      type List_Of_Class is new Lists.List with null record;

      procedure Put
        (These : in List_Of_Class;
         File  : in ATI.File_Type);

   -- Add an assign to the first attr of the list. If there is no first attr,
   -- add one:
      procedure Add_Assign_To_First_Attr
        (Attr_List : in out List_Of_Class;
         Name      : in     String;
         Value     : in     String);

      Empty_List : constant List_Of_Class :=
        List_Of_Class'(Lists.Empty_List with null record);
   private

      Default_Class : Class;
      Null_Class    : constant Class := Default_Class;

   end Attr;
   -----------------------------------------------------------------------------

   -----------------------------------------------------------------------------
   package Attr_Stmt is

      type Kind_Type is (Graph, Node, Edge);

      type Class is new Stmt.Class with record -- Initialized
         Kind  : Kind_Type := Node;
         Attr_List : Dot.Attr.List_Of_Class; -- Initialized
      end record;

      overriding
      procedure Put
        (This : in Class;
         File : in ATI.File_Type);

      -- Creates a Class object on the heap:
      procedure Append_To
        (This      : in Class;
         Stmt_List : in out Stmt.List_Of_Access_All_Class);

   end Attr_Stmt;
   -----------------------------------------------------------------------------

   -----------------------------------------------------------------------------
   package Node_ID is

      type Port_Class is tagged record -- Initialized
         Has_ID         : Boolean := False;
         ID             : ID_Type; -- Initialized
         Has_Compass_Pt : Boolean := False;
         Compass_Pt     : Compass_Pt_Type := C;
      end record;

      procedure Put
        (This : in Port_Class;
         File : in ATI.File_Type);

      Null_Port_Class : constant Port_Class;

      type Class is tagged record -- Initialized
         ID       : ID_Type; -- Initialized
         Port     : Port_Class; -- Initialized
      end record;

      procedure Put
        (This : in Class;
         File : in ATI.File_Type);

   private
      Default_Port_Class : Port_Class;
      Null_Port_Class : constant Port_Class := Default_Port_Class;
   end Node_ID;
   -----------------------------------------------------------------------------

   -----------------------------------------------------------------------------
   package HTML_Like_Labels is
      -- GraphViz HTML-Like Labels support variously formatted nodes, especially
      -- tables.  This package supports node formatting with "this = that" table
      -- rows.
      type Class is tagged private; -- Initialized

      procedure Add_Eq_Row
        (This : in out Class;
         L, R : in String);

      procedure Add_3_Col_Cell
        (This : in out Class;
         Text : in String);

      function To_String (This : in Class) return String;

   private

      use Ada.Strings.Unbounded;

      type LR_Pair is array (1 .. 2) Of Unbounded_String;

      function To_TR (This : in LR_Pair) return Unbounded_String;

      package LR_Pair_Lists is new
        Ada.Containers.Doubly_Linked_Lists (LR_Pair);
      -- Make primitive operations like "=" visible:
      type LR_Pair_List is new LR_Pair_Lists.List with null record; -- Initialized

      function To_Unbounded_String (This : in LR_Pair_List) return Unbounded_String;

      type Class is tagged record -- Initialized
         Rows : LR_Pair_List; -- Initialized
      end record;

   end HTML_Like_Labels;
   -----------------------------------------------------------------------------

    -----------------------------------------------------------------------------
   package Node_Stmt is

      type Class is new Stmt.Class with record -- Initialized
         Node_ID   : Dot.Node_ID.Class; -- Initialized
         Attr_List : Dot.Attr.List_Of_Class; -- Initialized
      end record;

      overriding
      procedure Put
        (This : in Class;
         File : in ATI.File_Type);

      -- Creates a Class object on the heap:
      procedure Append_To
        (This      : in Class;
         Stmt_List : in out Stmt.List_Of_Access_All_Class);

      procedure Add_Label
        (This     : in out Class;
         HL_Label : in HTML_Like_Labels.Class);

   end Node_Stmt;
   -----------------------------------------------------------------------------

  -----------------------------------------------------------------------------
   package Subgraphs is

      type Class is tagged record -- Initialized
         Stmt_List : Stmt.List_Of_Access_All_Class;
         ID        : ID_Type;
      end record;

      procedure Put
        (This : in Class;
         File : in ATI.File_Type);

   end Subgraphs;
   -----------------------------------------------------------------------------


   -----------------------------------------------------------------------------
   package Edges is

      package Terminals is

         type Kind_Type is (Node_Kind, Subgraph_Kind);

         type Class (Kind : Kind_Type := Node_Kind) is record -- Initialized
            case Kind is
               when Node_Kind =>
                  Node_Id  : Dot.Node_ID.Class; -- Initialized
               when Subgraph_Kind =>
                  Subgraph : Subgraphs.Class; -- Initialized
            end case;
         end record;

         procedure Put
           (This : in Class;
            File : in ATI.File_Type);

         package Lists is new
           Ada.Containers.Doubly_Linked_Lists (Class);
         -- Make primitive operations like "=" visible:
         type List_Of_Class is new Lists.List with null record;

         procedure Put
           (These : in List_Of_Class;
            File  : in ATI.File_Type);

      end Terminals;

      package Stmts is

         -- There must be at least one RHS:
         type Class is new Stmt.Class with record -- Initialized
            LHS       : Terminals.Class; -- Initialized
            RHS       : Terminals.Class; -- Initialized
            RHSs      : Terminals.List_Of_Class; -- Initialized
            Attr_List : Dot.Attr.List_Of_Class; -- Initialized
         end record;

         overriding
         procedure Put
           (This : in Class;
            File : in ATI.File_Type);

         -- Creates a Class object on the heap:
         procedure Append_To
           (This      : in Class;
            Stmt_List : in out Stmt.List_Of_Access_All_Class);

      end Stmts;

   end Edges;
   -----------------------------------------------------------------------------

   -----------------------------------------------------------------------------
   package Graphs is

      type Class is tagged private; -- Initialized
      type Access_Class is access Class;

      function Create
        (Is_Digraph : in Boolean;
         Is_Strict  : in Boolean)
        return Access_Class;

      procedure Set_Is_Digraph
        (This : access Class;
         To   : in     Boolean);

      procedure Set_Is_Strict
        (This : access Class;
         To   : in     Boolean);

      procedure Set_ID
        (This : access Class;
         To   : in     String);

      procedure Append_Stmt
        (This     : access Class;
         The_Stmt : in     Stmt.Access_All_Class);

      function Stmt_Count
        (This : access Class)
         return Natural;

      procedure Put
        (This : access Class;
         File : in     ATI.File_Type);

      -- Writes a file <Name>.dot. If Overwrite is true, overwrites an existing
      -- file. If it is not and the file exists, raises Usage_Error:
      procedure Write_File
        (This      : access Class;
         Name      : in     String;
         Overwrite : in     Boolean := False);

      Usage_Error : Exception;

   private

      type Class is tagged -- Initialized
         record
            Digraph   : Boolean := True;
            Strict    : Boolean := True;
            ID        : ID_Type; -- Initialized
            Stmt_List : Stmt.List_Of_Access_All_Class; -- Initialized
         end record;

   end Graphs;
   -----------------------------------------------------------------------------

private
   package ASU renames Ada.Strings.Unbounded;

   package Indented is

      procedure Indent;
      procedure Dedent;

      procedure Put
        (File : in ATI.File_Type;
         Item : in String);
      procedure New_Line
        (File : in ATI.File_Type);
      -- Calls New_Line if the current col is greater than the indent col:
      procedure End_Line_If_Needed
        (File : in ATI.File_Type);
      -- Put nothing if Item is empty, else Put it with a trailing space:
      procedure Put_Spaced
        (File : in ATI.File_Type;
         Item : in String);

   private

      -- If the indent is increased in the middle of a line, this will ensure
      -- that the next put is at that indent or better:
      procedure Put_Indent
        (File : in ATI.File_Type);

   end Indented;


   function To_String (Item : in Compass_Pt_Type)
                       return String;

   -- Returns a quoted string (in case ID is a reserved word), or an empty
   -- string if the ID is empty:
   function To_String (Item : in ID_Type)
                       return String;

   procedure Put
     (This : in ID_Type;
      File : in ATI.File_Type);

end Dot;
