-- This package provides support for creating and printing graphs in the
-- DOT language, described at www.graphviz.org.  From the website:
--
-- The following is an abstract grammar defining the DOT language.  Literal
-- characters and keywords are given in single quotes. Parentheses ( and )
-- indicate grouping when needed. Square brackets [ and ] enclose optional items.
-- Vertical bars | separate alternatives.
--
-- graph        : [ 'strict' ] ( 'graph' | 'digraph' ) [ ID ] '{' stmt_list '}'
-- stmt_list    : [ stmt [ ';' ] stmt_list ]
-- stmt 	    : node_stmt
--              | edge_stmt
--              | attr_stmt
-- 	            | ID '=' ID
--              | subgraph
-- attr_stmt    : ( 'graph' | 'node' | 'edge' ) attr_list
-- attr_list    : '[' [ a_list ] ']' [ attr_list ]
-- a_list       : ID '=' ID [ (';' | ',') ] [ a_list ]
-- edge_stmt    : ( node_id | subgraph ) edgeRHS [ attr_list ]
-- edgeRHS      : edgeop (node_id | subgraph) [ edgeRHS ]
-- node_stmt 	: node_id [ attr_list ]
-- node_id 	    : ID [ port ]
-- port         : ':' ID [ ':' compass_pt ]
--              | ':' compass_pt
-- subgraph     : [ 'subgraph' [ ID ] ] '{' stmt_list '}'
-- compass_pt   : ( 'n' | 'ne' | 'e' | 'se' | 's' | 'sw' | 'w' | 'nw' | 'c' | '_' )
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

with Ada.Strings.Unbounded;
with Ada.Text_IO;

package Dot is

   type ID_Type is new Ada.Strings.Unbounded.Unbounded_String;

   function To_ID_Type (Item : in String) return ID_Type
                        renames To_Unbounded_String;


private

   package ASU renames Ada.Strings.Unbounded;
   package ATI renames Ada.Text_IO;

   procedure Put (Item : in String) renames ATI.Put;
   procedure New_Line (Spacing : ATI.Positive_Count := 1) renames ATI.New_Line;

   -- Puts with a trailing space:
   procedure Put_Spaced (Item : in String);

   -- Puts nothing if ID is empty, else with a trailing space:
   procedure Put (Item : in ID_Type);

end Dot;
