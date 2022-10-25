with Rep_Common_Types.Menu;

package Rep_Common_Types.Data is
  package Menu renames Rep_Common_Types.Menu;

  subtype Menu_Item_Size_Limit_Type is Menu.Menu_Item_Type
  range Menu.Small .. Menu.Medium;

end Rep_Common_Types.Data;
