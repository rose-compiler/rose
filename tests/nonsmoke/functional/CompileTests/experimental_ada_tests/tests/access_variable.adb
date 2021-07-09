procedure Access_Variable is
type Button is null record;
type ButtonRef is access all Button;

procedure Press (B : ButtonRef) is
begin
null;
end Press;

Red_Button : aliased Button;

begin
Press(Red_Button'Access);
end Access_Variable;
