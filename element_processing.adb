with Asis.Iterator;

-- Does actual work:
with Actuals_For_Traversing;

package body Element_Processing is

   procedure Recursive_Construct_Processing is new
     Asis.Iterator.Traverse_Element
       (State_Information => Actuals_For_Traversing.Traversal_State,
        Pre_Operation     => Actuals_For_Traversing.Pre_Op,
        Post_Operation    => Actuals_For_Traversing.Post_Op);

   procedure Process_Construct (The_Element : Asis.Element) is
      Process_Control : Asis.Traverse_Control := Asis.Continue;
      Process_State   : Actuals_For_Traversing.Traversal_State :=
        Actuals_For_Traversing.Initial_Traversal_State;
   begin
      Recursive_Construct_Processing
        (Element => The_Element,
         Control => Process_Control,
         State   => Process_State);
   end Process_Construct;

end Element_Processing;
