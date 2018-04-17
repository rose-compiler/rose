// Bug report from Beata:
//    Previously defined forloop from hell that I make up (with Markus)
//    to make a point about how bizzare C++ code could be.
//    The examples caused an endless loop within ROSE at present,
//    which should teach me something about making up silly examples 
//    and passing them out so proadly to our users :-).
//    We now have it as a test code in ROSE (I will fix it after 
//    templates is further along).

void foobar(){
 
  for ( class X { public: int x; X(int input_x) { x = input_x; }
                          bool operator!=(X input_x) { return x != input_x.x; }
                          int operator++(int flag){x++;} }
        A = 0, B = 1; A != 10; B++)
      {
     // any thing you want goes here
      }
    }
