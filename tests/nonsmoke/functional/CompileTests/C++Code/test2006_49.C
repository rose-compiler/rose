/*
Duff's device     n.     
The most dramatic use yet seen of fall through in C, invented by Tom Duff when he was at Lucasfilm. Trying to bum all
the instructions he could out of an inner loop that copied data serially onto an output port, he decided to unroll it. He then realized that the
unrolled version could be implemented by interlacing the structures of a switch and a loop: 
*/

/*
Shocking though it appears to all who encounter it for the first time, the device is actually perfectly valid, legal C. C's default fall through in
case statements has long been its most controversial single feature; Duff observed that "This code forms some sort of argument in that
debate, but I'm not sure whether it's for or against." Duff has discussed the device in detail at http://www.lysator.liu.se/c/duffs-device.html.
Note that the omission of postfix ++ from *to was intentional (though confusing). Duff's device can be used to implement memory copy, but the
original aim was to copy values serially into a magic IO register. 

[For maximal obscurity, the outermost pair of braces above could actually be removed -- GLS] 

*/

   register n = (count + 7) / 8;      /* count > 0 assumed */

   switch (count % 8)
   {
   case 0:        do {  *to = *from++;
   case 7:              *to = *from++;
   case 6:              *to = *from++;
   case 5:              *to = *from++;
   case 4:              *to = *from++;
   case 3:              *to = *from++;
   case 2:              *to = *from++;
   case 1:              *to = *from++;
                      } while (--n > 0);
   }

