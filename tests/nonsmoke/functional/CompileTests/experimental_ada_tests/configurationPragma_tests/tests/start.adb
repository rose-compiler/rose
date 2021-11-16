with with_dot_issue.sub;

procedure start is

  myConst : constant := with_dot_issue.sub.Bytes_In_Word;
  myInt : integer;

begin

  myInt := myConst;

end start;

