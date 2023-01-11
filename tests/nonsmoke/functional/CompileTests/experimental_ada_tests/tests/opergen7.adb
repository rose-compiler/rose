
procedure opergen7 is

  package Y is
    type Color is (White);

    type Widget is tagged
      record
         fld : Color;
      end record;
  end Y;

  package Z is
    type ExtWidget is new Y.Widget with null record;
  end Z;

  m : Z.ExtWidget;

begin
   if Y."/=" (m.fld, Y.White) then
     null;
   end if;
end opergen7;
