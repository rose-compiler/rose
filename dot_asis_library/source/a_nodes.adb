with Ada.Exceptions;
with Ada.Text_IO;
-- For int."+":
with Interfaces.C;

package body A_Nodes is

   Module_Name : constant String := "A_Nodes";

   package AEX renames Ada.Exceptions;
   package ATI renames Ada.Text_IO;

   -- Checks to be sure no Unit with this ID has already been pushed.  Raises
   -- Usage_Error if so.
   procedure Check_Unit_Node
     (This : access Class;
      Unit : in a_nodes_h.Unit_Struct)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Check_Unit_Node";
      ID          : constant a_nodes_h.Unit_ID := Unit.ID;
      use type Interfaces.C.int;
   begin
      if This.Unit_IDs.Contains (ID) then
         raise Usage_Error with Module_Name &
           ": Tried to push second Unit with ID => " & ID'Image;
      else
         This.Unit_IDs.Insert (ID);
         if ID > This.Highest_Unit_ID then
            This.Highest_Unit_ID := ID;
         end if;
      end if;
   end Check_Unit_Node;

   -- Checks to be sure no Element with this ID has already been pushed.  Raises
   -- Usage_Error if so.
   procedure Check_Element_Node
     (This : access Class;
      Element : in a_nodes_h.Element_Struct)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Check_Element_Node";
      ID          : constant a_nodes_h.Element_ID := Element.ID;
      use type Interfaces.C.int;
   begin
      if This.Element_IDs.Contains (ID) then
         raise Usage_Error with Module_Name &
           ": Tried to push second Element with ID => " & ID'Image;
      else
         This.Element_IDs.Insert (ID);
         if ID > This.Highest_Element_ID then
            This.Highest_Element_ID := ID;
         end if;
      end if;
   end Check_Element_Node;

   procedure Print_Exception_Info
     (Module_Name : in String;
      X           : in AEX.Exception_Occurrence) is
   begin
      ATI.Put_Line ((1 .. 40 => '#'));
      ATI.Put_Line (Module_Name & ": ***EXCEPTION*** " & Aex.Exception_Information (X));
      ATI.Put_Line ((1 .. 40 => '#'));
   end Print_Exception_Info;

   ------------
   -- EXPORTED:
   ------------
   procedure Set
     (This    : access Class;
      Context : in     a_nodes_h.Context_Struct) is
   begin
      This.Nodes.Context := Context;
   end Set;

   ------------
   -- EXPORTED:
   ------------
   procedure Push
     (This : access Class;
      Unit : in     a_nodes_h.Unit_Struct)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Push";
      List_Node : a_nodes_h.Unit_Struct_List_Struct :=
        a_nodes_h.Support.Default_Unit_Struct_List_Struct;
      use type Interfaces.C.int;
      use type a_nodes_h.Unit_Structs_Ptr;
   begin
      begin
         Check_Unit_Node (This, Unit);
      exception
         when X : Usage_Error =>
            Print_Exception_Info (Module_Name, X);
            ATI.Put_Line ("Continuing...");
      end;
      List_Node.Unit := Unit;
      if This.Nodes.Units = null then
         List_Node.Next := null;
         List_Node.Next_Count := 0;
      else
         List_Node.Next := This.Nodes.Units;
         List_Node.Next_Count := This.Nodes.Units.Next_Count + 1;
      end if;
      This.Nodes.Units := new a_nodes_h.Unit_Struct_List_Struct'(List_Node);
   end Push;

   ------------
   -- EXPORTED:
   ------------
   procedure Push
     (This    : access Class;
      Element : in     a_nodes_h.Element_Struct)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Push";
      List_Node : a_nodes_h.Element_Struct_List_Struct :=
        a_nodes_h.Support.Default_Element_Struct_List_Struct;
      use type Interfaces.C.int;
      use type a_nodes_h.Element_Structs_Ptr;
   begin
      begin
         Check_Element_Node (This, Element);
      exception
         when X : Usage_Error =>
            Print_Exception_Info (Module_Name, X);
            ATI.Put_Line ("Continuing...");
      end;
      List_Node.Element := Element;
      if This.Nodes.Elements = null then
         List_Node.Next := null;
         List_Node.Next_Count := 0;
      else
         List_Node.Next := This.Nodes.Elements;
         List_Node.Next_Count := This.Nodes.Elements.Next_Count + 1;
      end if;
      This.Nodes.Elements := new a_nodes_h.Element_Struct_List_Struct'(List_Node);
   end Push;

   ------------
   -- EXPORTED:
   ------------
   procedure Add_Not_Implemented
     (This : access Class) is
   begin
      This.Not_Implemented := This.Not_Implemented + 1;
   end Add_Not_Implemented;

   ------------
   -- EXPORTED:
   ------------
   function Get_Nodes
     (This : access Class)
      return a_nodes_h.Nodes_Struct is
   begin
      return This.Nodes;
   end Get_Nodes;

   ------------
   -- EXPORTED:
   ------------
   procedure Print_Stats
     (This : access Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Print_Stats";
      procedure Put_Line (Message : in String)is
      begin
         ATI.Put_Line (Module_Name & ": " & Message);
      end Put_Line;
   begin
      Put_Line ("Total Units        :" & This.Unit_Ids.Length'Image);
      Put_Line ("Highest Unit ID    :" & This.Highest_Unit_ID'Image);
      Put_Line ("Total Elements     :" & This.Element_Ids.Length'Image);
      Put_Line ("Highest Element ID :" & This.Highest_Element_ID'Image);
      Put_Line ("Not Implemented    :" & This.Not_Implemented'Image);
   end Print_Stats;


end A_Nodes;
