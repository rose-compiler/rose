// we can add arbitrary new state to the lambda object, because each capture 
// creates a new type-deduced local variable inside the lambda:

int x = 4;
int z = [&r = x, y = x+1] 
        {
          r += 2;         // set x to 6; "R is for Renamed Ref"
          return y+2;     // return 7 to initialize z
        }(); // invoke lambda
