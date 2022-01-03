procedure deref_test is
  type PString is access String;
  
  a,b   : String (1 .. 3);
  res   : String := "a";
  PtrArr: array(1 .. 2) of PString;

  function "<"(x,y: in String) return PString is
  begin
    return PtrArr(1);
  end "<";

begin
  PtrArr(1) := new String'("<");
  PtrArr(2) := new String'(">=");

  res := "<"(a, b).all;

  PtrArr(2) := a < b;
end deref_test;
