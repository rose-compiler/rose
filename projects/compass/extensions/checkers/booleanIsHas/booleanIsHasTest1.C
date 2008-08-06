
bool fresh()
{
  bool has_jimmy, is_nancy, somethingelse, blah;
  bool mike = is_nancy = somethingelse = has_jimmy = blah = true;
  if (mike and is_nancy and somethingelse and blah and has_jimmy)
    {
      return 1;
    }
  return 0;
}


bool is_fast()
{
  return false;
}

bool ahas_normal()
{
  return true;
}

int main ()
{
  return 0;
}


/* PROPER SOLUTIONS

BooleanIsHasChecker:/home/byrd11/rose/checkers/oldcheckers/booleanIsHas/booleanIsHasCheckerTest1.C:2.1:fresh is a bool-returning function that is not named is_ or has_
BooleanIsHasChecker:/home/byrd11/rose/checkers/oldcheckers/booleanIsHas/booleanIsHasCheckerTest1.C:4.3:somethingelse is a boolean variable that is not named is_ or has_
BooleanIsHasChecker:/home/byrd11/rose/checkers/oldcheckers/booleanIsHas/booleanIsHasCheckerTest1.C:4.3:blah is a boolean variable that is not named is_ or has_
BooleanIsHasChecker:/home/byrd11/rose/checkers/oldcheckers/booleanIsHas/booleanIsHasCheckerTest1.C:5.3:mike is a boolean variable that is not named is_ or has_
BooleanIsHasChecker:/home/byrd11/rose/checkers/oldcheckers/booleanIsHas/booleanIsHasCheckerTest1.C:19.1:ahas_normal is a bool-returning function that is not named is_ or has_
*/
