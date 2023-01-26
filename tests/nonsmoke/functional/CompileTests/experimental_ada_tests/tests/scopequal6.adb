

procedure ScopeQual6 is

  package Y is
     type Button is tagged private;
     type Button_Callback is access procedure(B : Button'Class);

     procedure Press (B : in Button; C : in Button_Callback);
  private
    type Button is tagged null record;
  end Y;

  package Z is
    type Color_Button is new Y.Button with null record;
  end Z;

  package body Y is
     procedure Press (B : in Button; C : in Button_Callback) is
     begin
       null;
     end Press;
  end Y;

  procedure Display (B : Y.Button'Class) is
  begin
    null;
  end Display;

  Green_Button : Z.Color_Button;

begin
  Z.Press(Green_Button, Display'Access);
end ScopeQual6;
