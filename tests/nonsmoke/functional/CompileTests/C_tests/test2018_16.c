/*
 *  Bug: array type had incomplete element type for double v[][][][5]
 * */
// Must have this prototype to reproduce the bug
void l2norm (int ldx, int ldy, double v[][ldy/2*2+1][ldx/2*2+1][5]);

void l2norm (int ldx, int ldy, double v[][ldy/2*2+1][ldx/2*2+1][5])
   {
   }
