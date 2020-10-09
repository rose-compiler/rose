with Asis.Statements;

package body Asis_Tool_2.Element.Exception_Handlers is

   -----------------------------
   -- EXPORTED:
   -----------------------------
   procedure Do_Pre_Child_Processing
     (Element : in Asis.Element;
      State : in out Class)
   is
      Parent_Name : constant String := Module_Name;
      Module_Name : constant String := Parent_Name & ".Process_Exception_Handler";
      Result : a_nodes_h.Exception_Handler_Struct :=
        a_nodes_h.Support.Default_Exception_Handler_Struct;

      procedure Add_Choice_Parameter_Specification is
         ID : constant a_nodes_h.Element_ID :=
           Get_Element_ID (Asis.Statements.Choice_Parameter_Specification
                           (Element));
      begin
         State.Add_To_Dot_Label_And_Edge ("Choice_Parameter_Specification", ID);
         Result.Choice_Parameter_Specification :=
           ID;
      end;

      procedure Add_Exception_Choices is
      begin
         Add_Element_List
           (This           => State,
            Elements_In    => Asis.Statements.Exception_Choices (Element),
            Dot_Label_Name => "Exception_Choices",
            List_Out       => Result.Exception_Choices,
            Add_Edges      => True);
      end;

      procedure Add_Handler_Statements is
      begin
         Add_Element_List
           (This           => State,
            Elements_In    => Asis.Statements.Handler_Statements (Element),
            Dot_Label_Name => "Handler_Statements",
            List_Out       => Result.Handler_Statements,
            Add_Edges      => True);
      end;

   begin
      Add_Choice_Parameter_Specification;
      Add_Exception_Choices;
      Add_Handler_Statements;

      State.A_Element.Element_Kind := a_nodes_h.An_Exception_Handler;
      State.A_Element.the_union.exception_handler := Result;
   end Do_Pre_Child_Processing;

end Asis_Tool_2.Element.Exception_Handlers;
