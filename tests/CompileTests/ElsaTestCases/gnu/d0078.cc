//  /home/ballAruns/tmpfiles/./Guppi-0.40.3-13/gnan-1Njx.i:3823:64:
//  error: there is no member called `__d' in union /*anonymous*/

int main() {
  (
   (union {int c;}) {c:0}
  ) .c ;

  (
   (union {int d;}) {d:0}
  ) .d ;

  // for now this typechecks the E_compoundLit twice due to the way []
  // is implemented as lowering and re-typechecking
  (
   (union {int i[2];}) {i:{0}}
  )
  .i[1];
}
