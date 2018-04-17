#define LEAP_YEAR 0

int main (int argc, char* argv[])
{
  for (int i = 1; i < argc; ++i)
    {
      char* month = argv[i];
      int days = 0;
      if (month == "January"
	  || month == "March"
	  || month == "May"
	  || month == "July"
	  || month == "August"
	  || month == "October"
	  || month == "December")
         days = 31;
#if LEAP_YEAR
      else if (month == "February")
         days = 29;
#else
      else if (month == "February")
         days = 28;
#endif
      else if (month == "April"
	       || month == "June"
	       || month == "September"
	       || month == "November")
         days = 30;
  // cout << argv[i] << " " << days << endl;
    }
  return 0;
}
