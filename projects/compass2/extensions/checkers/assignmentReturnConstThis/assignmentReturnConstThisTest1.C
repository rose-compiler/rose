class jewel
{
public: 
  const jewel& operator=(const jewel &other);
};

const jewel& jewel::operator=(const jewel &other)
{
  return other;
}

class fred
{
  const fred& operator=(const fred &rightSide) {int x = 5; x ++;}
};

class wilma
{
  wilma& operator=(const wilma &rightSid) {if (&rightSid == this) return *this; return *this;}
};

class bambam
{
  const bambam& operator=(const bambam &rightSid) {int x = 5; if (&rightSid == this) return *this; if(x) x++;}
};

class pebbles
{
  const pebbles& operator=(const pebbles &rightSid) {while (true) {if (&rightSid == this) return *this; break;} return *this;}
};



int main ()
{
  jewel *rack = new jewel();
  jewel *other = new jewel();
  *other = *rack = *other; 
  return 0;
}

/*
Correct Output

AssignmentReturnConstThisChecker:/assignmentReturnConstThisCheckerTest1.C:7.1: jewel's operator= returns something not *this or has no explicit returns
AssignmentReturnConstThisChecker:/assignmentReturnConstThisCheckerTest1.C:14.3: fred's operator= returns something not *this or has no explicit returns
AssignmentReturnConstThisChecker:/assignmentReturnConstThisCheckerTest1.C:19.3: wilma's operator= does not have return type const wilma&
*/
