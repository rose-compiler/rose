procedure Enum_Subtype is

  type Rainbow is (Red, Orange, Yellow, Green, Blue, Indigo, Violet);
  type ColdColors is new Rainbow range Blue .. Violet;

  BlueColor : ColdColors := Blue;
begin
  null;
end Enum_Subtype;
