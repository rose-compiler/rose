generic
   type Parent_Type is tagged private;            -- Actual is parent
package Record_extends_formal_type is                                      -- tagged type.
   type Node_Type is new Parent_Type with record  -- Record extension
      null;
   end record;
end Record_extends_formal_type;
