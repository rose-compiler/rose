package Asis_Adapter.Element.Definitions is

   procedure Do_Pre_Child_Processing
     (Element : in     Asis.Element;
      State   : in out Class);

   procedure Do_Post_Child_Processing
     (Element : in     Asis.Element;
      State   : in out Class);

private

   -- For debuggng:
   Parent_Name : constant String := Module_Name;
   Module_Name : constant String := Parent_Name & "Definitions";

end Asis_Adapter.Element.Definitions;
