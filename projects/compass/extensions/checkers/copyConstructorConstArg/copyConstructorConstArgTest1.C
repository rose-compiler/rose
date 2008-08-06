class ok
{

public:
  
  ok (const ok &other) {};
};


class willFail
{
public:
  // This constructer is not const and will fail.
  willFail (willFail &other) {};
};

int main()
{
  return 0;
}

/*
PROPER OUTPUT

CopyConstructerConstArgChecker:/home/byrd11/rose/checkers/oldcheckers/copyConstructerConstArg/copyConstructerConstArgCheckerTest1.C:16.1: Class: "willFail" --  Copy Constructor Arg Not Const
*/
