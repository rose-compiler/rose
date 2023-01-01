int main(int argc, char* argv[])
{
  unsigned int ivar;
  unsigned int ibbr;
  ivar = 0;
  ibbr = 1;
/*1 before FOR */
  for (ivar=0; ivar < 25 /*2 EOS FOR */; ivar++)
/*3 before BEGIN 1 */
/*4 before BEGIN 2 */
    {
/*5 after BEGIN */
      ibbr = ibbr + 0;
      ibbr = ibbr + 2;
/*6 before END */
    }
/*7 after END FOR block 1 */
/*8 after END FOR block 2 */
}
/*9 after END PROGRAN block */
