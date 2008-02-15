#include <stdio.h>

int five(){
  return 5;
}

int zero(){
  return 0;
}

int main(int, char**) {
  int a = 999;
  int b =                                                                                                                                  140;
 

  // line 17 should not show, it has a default within the if statement block
  switch(a)
    {
    case 1:
      break;
    case 3:
      five();
      if (five() and zero())
	{
    default:
      printf("in default!\n");
      break;
	}
    }


  // just trying to make it ugly, nested cases with a embedded loop... kinda like the ugly thing mentioned in meeting last week.  
  // line 35 should not show up
  int i = a;
  switch(a)
    {
    case 1:
      while (i > 0)
	{
	  i += 1;
	  
	case 5:
	  i -= 2;
	  //line 45 has a default, it's nested though, but still should not show up.
	  switch(b)
		{
		case 1:
		  break;
		default:
		  break;
		}
	  break;
	case 4:
	  break;
	}
    case 777:
      printf("did you break out far enough?\n");

    default:
      printf("%d nuttin honey\n", i);
	
    }
    

  //just trying to keep the default in the loop, line 66 should not show up
  switch (a+b)
    {
      while(true)
	{
	case 12:
	  break;
	case 14:
	  break;
	default:
	  break;
	}
    }

  //in loop without default, line 80 should show
  switch(a+b)
    {
      while(true)
	{
	case 1:
	  break;
	    }
    }



  //nested madness line 95 should show up, has no default, although one nested within it did.
  // line 100 nested within has a default, should not show
  // line 105 does not have one, though nested in one that does, should show up.

  switch(a+b)
    {
    case 1:
      break;
    case 2:
      switch(3)
	{
	case 3:
	  break;
	case 4:
	  switch(7)
	    {
	    case 3:
	      break;
	    }
	  break;
	  
	default:
	  break;

	}
    }

  return a + b;
}


//hence correct answer should show lines:  80,95,105
