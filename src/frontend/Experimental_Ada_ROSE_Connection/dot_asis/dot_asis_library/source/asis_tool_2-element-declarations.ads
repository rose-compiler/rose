package Asis_Tool_2.Element.Declarations is

   procedure Do_Pre_Child_Processing
     (Element : in     Asis.Element;
      State   : in out Class);

private

   -- For debuggng:
   Parent_Name : constant String := Module_Name;
   Module_Name : constant String := Parent_Name & "Declarations";

end Asis_Tool_2.Element.Declarations;
