
// The input code has the following line for those constant number:
// #define REAL float
float error_ref= 9.212767E-04, resid_ref = 2.355429E-08;
float tol,relax=1.0,alpha=0.0543;

/* For EDG 4.7, It has the following output:
float error_ref = 9.212767E-04;
float resid_ref = 2.355429E-08;
float relax = 1.0;
float alpha = 0.0543;

For EDG 4.9, it has the following output:
float error_ref = 0.000921277F;
float resid_ref = 2.35543e-08F;
float relax = 1.00000F;
float alpha = 0.0543000F;
*/

