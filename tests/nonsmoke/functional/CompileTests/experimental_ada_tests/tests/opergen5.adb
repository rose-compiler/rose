
function opergen5 return boolean is
  package Y is -- Interpreter

     type Num is mod 10;

     zeronum : Num := 0;
  end Y;


begin
    return Y."/="( Y.zeronum, 7 );
end opergen5;
