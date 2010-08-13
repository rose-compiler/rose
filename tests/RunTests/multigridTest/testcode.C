
//================================================================
// A++ library test code for multiple dimensions.  Version 1.2.7 
//================================================================

#define BOUNDS_CHECK

#include <A++.h>
#include <iostream.h>

#define TYPE float
#define TYPEArray floatArray

#if 0
// These can not be defined here since they are defined within the A++/P++ library
// The KCC compiler has the strange behavior of not returning from "main"
// when these are defined (multiply defined)
Range APP_Unit_Range(0,0);
int APP_Global_Array_Base = 0;
void APP_Null_Initialization_Function(void) {};
#endif

// comment out the whole of the main problem as a link test for KCC with expression templates!
#if 1
int main(int argc, char** argv)
{
// MemoryManagerType  Memory_Manager;
// Array_Descriptor_Type::smartReleaseOfInternalMemory = TRUE;
// APP_smartReleaseOfInternalMemory = TRUE;
   Array_Domain_Type::smartReleaseOfInternalMemory = TRUE;

#if 1
  Index::setBoundsCheck(on);
#endif

  //================= P++ startup =============================

#if defined(USE_PPP)
  int Number_Of_Processors = 1;
  Optimization_Manager::Initialize_Virtual_Machine
    ("/n/c3served/kdb/A++P++/P++/TEST/testcode",
     Number_Of_Processors,argc,argv);
  printf("Run P++ code(Number Of Processors=%d) \n",
    Number_Of_Processors);
#endif

  //===========================================================
  
  Range Ispan(2,12);
  Range Jspan(-2,8);
  Range Kspan(-6,-1);
  Range Lspan(100,105);
  Range Mspan(100,105);
  Range Nspan(100,105);

  TYPEArray A(Ispan,Jspan,Kspan,Lspan);
  TYPEArray B(Ispan,Jspan,Kspan);
  TYPEArray C(Ispan,Jspan);
  TYPEArray D(Ispan,Jspan,Kspan);
  TYPEArray E(Ispan,Jspan,Kspan,Lspan,Mspan);
  TYPEArray F(Ispan,Jspan,Kspan,Lspan,Mspan,Nspan);

  Index Ispan1(4,6);
  Index Jspan1(0,6);
  Index Kspan1(-5,4);
  Index Lspan1(101,4);

  Index Ispan2(6,2,2);
  Index Jspan2(2,2,2);
  Index Kspan2(-4,3);
  Index Lspan2(102,3);

  Index all;

  int i,j,k;

  //============================================================

  A = 1.;
//APP_DEBUG = 8;
  A(Ispan1,Jspan1,Kspan1,Lspan1) = 4.;
//APP_DEBUG = 0;
//printf("Exit until this part is working! \n");
//APP_ABORT();

  A(Ispan2,Jspan2,Kspan2,Lspan2) = 9.;

  for (k=B.getBase(2);k<=B.getBound(2);k++)
    for (j=B.getBase(1);j<=B.getBound(1);j++)
      for (i=B.getBase(0);i<=B.getBound(0);i++)
        B(i,j,k) = i;

  C = -5.;

  for (k=B.getBase(2);k<=B.getBound(2);k++)
    for (j=B.getBase(1);j<=B.getBound(1);j++)
      for (i=B.getBase(0);i<=B.getBound(0);i++)
       	D(i,j,k) = D.getBound(0)-i;

  E = 7.;
  F = 8.;

//B.display("B");
//B(all,all,0).display("B(all,all,0)");
//D.display("D");

  //============================================================
  // ... Tests abstract operator:
  //    TYPE TYPEArray::Abstract_Unary_Operator 
  //      (const TYPEArray & X ,
  //       Prototype_14 Operation_Array_Data , int Operation_Type )
  //
  //   Test with friend function, x = max(X) ...
  //
  //============================================================

  TYPE m1,m2,m3,m4,m5,m6;
//TYPE m1,m2,m3,m4,m5;

#if 1
  m1 = max(A); // A has values 1,4 and 9
  m2 = max(B); // B has values up to 12
  m3 = max(C); // all of C is -5
//C.display("C");
  m4 = max(E); // all of E is 7

  if (m1 != 9. || m2 != 12. || m3 != -5. || m4 != 7.) 
  {
    cout<<"ERROR in max(X), test1"<<endl<<
    "  m1,m2,m3,m4="<<m1<<" "<<m2<<" "<<m3<<" "<<m4<<endl;
    cout<<"  correct answers are: m1=9., m2=12.,m3=-5.,m4=7"<<endl;
  }
  else
    cout<<"max(X) test1 ok"<<endl;

//A.display("A");

//printf("Exit until this part is working! \n");
//APP_ABORT();

  APP_DEBUG = 0;
  where (A>(TYPE)1. && A<(TYPE)9.) m1 = max(A);  // only value besides 1 
                                     // and 9 is 4
  APP_DEBUG = 0;
//printf("Exit until this part is working! \n");
//APP_ABORT();
  where (B < (TYPE)7.) m2 = max(B); // B has all integer vals between
			      // 2 and 12
  where (C < (TYPE)0.) m3 = max(C); // this is all of C
  where (B > (TYPE)6.) m4 = max(D); // vals in D are 12 - vals in B so
			      // the max is where D is 0 to 5

  F(7,5,-3,101,100,103) = (TYPE)9.;
  where (B > (TYPE)6.) m5 = max(F(all,all,all,101,100,103)); 
			      // vals in F are 8 except 1 at 9 
			      // so max is 9

  if (m1 != 4. || m2!= 6. || m3 != -5. || m4 != 5. || m5 != 9.)
  {
    cout<<"ERROR in max(X), test2"<<endl<<
    "  m1,m2,m3,m4,m5="<<m1<<" "<<m2<<" "<<m3<<" "<<m4<<" "<<m5<<endl;
    cout<<"  correct answers are: m1=4, m2=6,m3=-5,m4=5,m5=9 "<<endl;
  }
  else
    cout<<"max(X) test2 ok"<<endl;

//printf("Exit until this part is working! \n");
//APP_ABORT();
//exit(1);

  Index Ispan3(7,2,2);
  Index Jspan3(2,2,2);
  m1 = max(A(all,all,Kspan1,all)); // all of A, tests mix of
                                   // different Index types
  //A(all,Jspan2,Kspan2,Lspan2).display("A(all,Jspan2,Kspan2,Lspan2)");

  m2 = max(A(all,Jspan2,all,all)); // subset of A that includes
                                   // elements set to 9
  m3 = max(B(Ispan2,all,all,all)); // middle section of B in
                                   // direction 0, includes vals
				   // 6, 7 and 8
  m4 = max(A(Ispan3,Jspan3,all,all)); // Ispan3 chooses values
                                      // between those set to 9


  if (m1 != 9. || m2 != 9. || m3 != 8. || m4 != 4.)
  {
    cout<<"ERROR in max(X), test3"<<endl<<
    "  m1,m2,m3,m4="<<m1<<" "<<m2<<" "<<m3<<" "<<m4<<endl;
    cout<<"  correct answers are: m1=9,m2=9,m3=8,m4=4"<<endl;
  }
  else
    cout<<"max(X) test3 ok"<<endl;

//APP_DEBUG = 4;
  where (B<(TYPE)9.) m1 = max(B+D); // D is 12 -B so B+D is 12 everywhere
//APP_DEBUG = 0;

//printf("Exit until this part is working! \n");
//APP_ABORT();

  if (m1 != 12.) 
  {
    cout<<"ERROR in max(X), test4"<<endl<<"  m1="<<m1<<endl;
    cout<<"  correct answer is m1=12"<<endl;
  }
  else
    cout<<"max(X) test4 ok"<<endl;
#endif

#if defined(USE_PPP)

#else
  intArray I(3),J(3);

  I(0) = 4;
  I(1) = 7;
  I(2) = 11;

  J(0) = -1;
  J(1) = 2;
  J(2) = 6;

  TYPEArray FF(3);
  FF(0) = 5.;
  FF(1) = 6.;
  FF(2) = 7.;

  // A(4,-1,-3,103) = 1
  // A(7,2,-3,103) = 4
  // A(11,6,-3,103) = 1
  // view is 1d array with (1,4,1)

//FF.display("FF");
//A(I,J,-3,103).display("A(I,J,-3,103)");
//A(all,all,-3,103).display("A(all,all,-3,103)");
//A(I,J,-3,103).view("A(I,J,-3,103)");

//printf ("max(A(I,J,-3,103)) = %f \n",max(A(I,J,-3,103)) );
//printf ("max(A(I,J,-3,103) + (TYPE)2.0) = %f \n",max(A(I,J,-3,103) + (TYPE)2.0) );

//printf("Exit until this part is working! \n");
//APP_ABORT();

//APP_DEBUG = 6;
//printf ("Call where (FF<(TYPE)7.) m1 = max(A(I,J,-3,103)+(TYPE)2.) \n");
  where (FF<(TYPE)7.) m1 = max(A(I,J,-3,103)+(TYPE)2.);
//printf ("DONE: Call where (FF<(TYPE)7.) m1 = max(A(I,J,-3,103)+(TYPE)2.) \n");
//APP_DEBUG = 0;

//printf("Exit until this part is working! \n");
//APP_ABORT();

// We want to test the first statement seperately!
#if 1
//printf ("where (FF>(TYPE)6.) m2 = max(A(I,J,-3,103)+(TYPE)2.) \n");
  where (FF>(TYPE)6.) m2 = max(A(I,J,-3,103)+(TYPE)2.);
//printf ("DONE: where (FF>(TYPE)6.) m2 = max(A(I,J,-3,103)+(TYPE)2.) \n");
#else
  m2 = 3;
#endif

  if (m1 != 6 || m2 != 3)
  {
    cout<<"ERROR in max(X), test5"<<endl<<"  m1,m2="<<m1<<
      " "<<m2<<endl;
    cout<<"  correct answers are: m1=6, m2=3"<<endl;
  }
  else
    cout<<"max(X) test5 ok"<<endl;

//printf("Exit until this part is working! \n");
//APP_ABORT();

#endif
  //============================================================
  // ... test with abstract operator:
  //   TYPEArray & TYPEArray::Abstract_Unary_Operator 
  //     (const TYPEArray & X , 
  //      Prototype_0 Operation_Array_Data ,
  //      Prototype_1 Operation_Array_Data_Accumulate_To_Existing_Operand,
  //      int Operation_Type )
  //		   
  //   member function 
  //     A = -B
  //   friend functions 
  //     A = sqrt  (X) (A and X can't be intArrays)
  //============================================================

  TYPEArray T1,T2,T3,T4,T5;
  TYPE* datptr;
  TYPE sum1,sum2;

  T1 = -(B+D); // all values are -12, size is 11 x 11 x 6
//T1.display("T1");
  datptr = T1.getDataPointer();
  sum1=0.;
  for (i=0;i<726;i++) 
    sum1+=datptr[i];

  m1 = -12.*726;
  if (sum1 != m1)
  {
    cout<<"ERROR in A = -X, test1"<<endl<<"  sum1="<<sum1<<endl;
    cout<<"  correct answer is "<<m1<<endl;
  }
  else
    cout<<"A =-X test1 ok"<<endl;

//printf("Exit until this part is working! \n");
//APP_ABORT();

  // ... test A = sqrt(X) (A and X can't be intArrays) ...

  // 2*2*3*3 elements set to 9 in A, 6*6*4*4 - 36 elements set to 4,
  //   11*11*6*6 - 540 - 36 elements are set to 1
  T2 = sqrt(A);    
  datptr = T2.getDataPointer();
  sum1=0.;
  for (i=0;i<4356;i++) 
    sum1+=datptr[i];

  m1 = 540*2. +36*3. +3780.;
  if (sum1 != m1)
  {
    cout<<"ERROR in sqrt(X), test1"<<endl<<"  sum1="<<sum1<<endl;
    cout<<"  correct answer is: sum1= "<<m1<<endl;
  }
  else
    cout<<"A = sqrt(X) test1 ok"<<endl;

//printf("Exit until this part is working! \n");
//APP_ABORT();

  // ... all entries should be 3 ...

//A(all,all,-3,103).display("A(all,all,-3,103)");
//A(Ispan2,Jspan2,-3,103).display("A(Ispan2,Jspan2,-3,103)");
//A(Ispan2,Jspan2,-3,103).view("A(Ispan2,Jspan2,-3,103)");

  T3 = sqrt(A(Ispan2,Jspan2,-3,103)); 
//APP_DEBUG = 0;
//T3 = A(Ispan2,Jspan2,-3,103); 
//APP_DEBUG = 0;

//T3.display("T3");
  datptr = T3.getDataPointer();
  sum1=0.;
  for (i=0;i<4;i++) 
    sum1+=datptr[i];

//printf ("T3.getDataPointer() - T3.Array_View_Pointer5 = %d \n",T3.getDataPointer() - T3.Array_View_Pointer5);
//printf ("T3.Array_View_Pointer2 - T3.Array_View_Pointer5 = %d \n",T3.Array_View_Pointer2 - T3.Array_View_Pointer5);
//printf ("sum1 = %f (should be 12) \n",sum1);
//printf("Exit until this part is working! \n");
//APP_ABORT();

  // ... all entries in T4 should be 2 (because 7 and 3 are not
  //      included in Ispan2,Jspan2 )... 
  T4 = sqrt(A(7,3,Kspan2,Lspan2));
//T4.display("T4");
  datptr = T4.getDataPointer();
  sum2=0.;
  for (i=0;i<9;i++) 
    sum2+=datptr[i];

  m1 = 12;
  m2 = 18.;

  if (sum1 != m1 || sum2 != m2)
  {
    cout<<"ERROR in sqrt(X), test2"<<endl<<
    "  sum1="<<sum1<<" "<<", sum2="<<sum2<<endl;
    cout<<"  correct answers are: sum1=12, sum2=18"<<endl;
  }
  else
    cout<<"A = sqrt(X) test2 ok"<<endl;

//printf("Exit until this part is working! \n");
//APP_ABORT();

  /*------------------------------------------------------------*/
  T5 = sqrt(A(I,J,-6,100)); // all elements for k fixed at -6 are 1
  datptr = T5.getDataPointer();
  sum1=0.;
  for (i=0;i<3;i++) 
    sum1+=datptr[i];

  m1 = 3.;
  if (sum1 != m1)
  {
    cout<<"ERROR in sqrt(X), test3"<<endl<<"  sum1="<<sum1<<endl;
    cout<<"  correct answer is: sum1 = 3"<<endl;
  }
  else
    cout<<"A = sqrt(X) test3 ok"<<endl;

//printf("Exit until this part is working! \n");
//APP_ABORT();

  TYPEArray T6(A);
  T6 = 0;
  where (A>(TYPE)2. && A<(TYPE)7.) T6 = sqrt(A); // where masks all elements of
                                     // A except 4
  datptr = T6.getDataPointer();
  sum1=0.;
  for (i=0;i<4356;i++) 
    sum1+=datptr[i];

  m1 = 1080; // 540 elements of A are 4 so 540 elements of T6 are 2,
             // the rest are 0
  if (sum1 != m1)
  {
    cout<<"ERROR in sqrt(X), test4"<<endl<<"  sum1="<<sum1<<endl;
    cout<<"  correct answer is sum1=1080"<<endl;
  }
  else
    cout<<"A = sqrt(X) test4 ok"<<endl;

//printf("Exit until this part is working! \n");
//APP_ABORT();

  //============================================================
  //  ... Test abstract operator:
  //   TYPEArray & TYPEArray::Abstract_Binary_Operator_Non_Commutative 
  //   (const TYPEArray & Lhs , const TYPEArray & Rhs ,
  //    Prototype_2 Operation_Array_Data ,
  //    Prototype_3 Operation_Array_Data_Accumulate_To_Existing_Operand,
  //    int Operation_Type )
  //
  //  A = Lhs - Rhs
  //============================================================

  TYPEArray F1 = A(all,all,-6,100)-C; // view of A includes only
                                       // elements that are set to 1
  datptr = F1.getDataPointer();
  sum1=0.;
  for (i=0;i<121;i++) 
    sum1+=datptr[i];

  m1 = 726; // 726 = (1--5)*121
  if (sum1 != m1)
  {
    cout<<"ERROR in Lhs-Rhs,test1"<<endl<<"  sum1="<<sum1<<endl;
    cout<<"  correct answer is: sum1 = 726"<<endl;
  }
  else
    cout<<"A = Lhs - Rhs, test1 ok"<<endl;


  TYPEArray F2(Ispan,Jspan,Kspan);
  F2=0;
  where (B>(TYPE)6.) F2 = B-D; // B-D = B-(12-B) = 2*B-12 so masked 
			// elements of F2 are 2*7-12 to 2*12 -12
			// which is 2,4, ..., 12.  There
			// are 11*6 of each of these

  datptr = F2.getDataPointer();
  sum1=0.;
  for (i=0;i<726;i++) 
    sum1+=datptr[i];

  m1 = 42.*11*6; // (2+4+...+12)*11*6
  if (sum1 != m1)
  {
    cout<<"ERROR in Lhs-Rhs,test2"<<endl<<"  sum1="<<sum1<<endl;
    cout<<"  correct answer is: sum1 ="<<m1<<endl;
  }
  else
    cout<<"A = Lhs - Rhs, test2 ok"<<endl;

  Index Is(Ispan);
  Index Js(Jspan);
  Index Ks(Kspan);

  TYPEArray F3(Ispan,Jspan,Kspan);
  F3 = A(Is,Js,Ks,103) -(B+D) ;

  datptr = F3.getDataPointer();
  sum1=0.;
  for (i=0;i<726;i++) 
    sum1+=datptr[i];


  // ... In A(Is,Js,Ks,103) there are 4 values set to 9 in i-j plane 
  //    at (6,2),(8,2),(6,4) and (8,4) for 3 i-j planes at k=102 to 104 
  //    making a total of 12 elements at 9. 6 by 6 by 4 elements set 
  //    to 4 or 9 is 144-12 elements set to 4. 11*11*6 - (132+12) 
  //    elements set to 1. B+D=12 for all 726 elements. ...

  m1 = 582. + (132*4.) + (12*9.);
  m1 -= 12.*726;
  if (sum1 != m1)
  {
    cout<<"ERROR in Lhs-Rhs,test3"<<endl<<
    "  sum1="<<sum1<<endl;
    cout<<"  correct answer is: sum1="<<m1<<endl;
  }
  else
    cout<<"A = Lhs - Rhs, test3 ok"<<endl;

//printf("Exit until this part is working! \n");
//APP_ABORT();
#if 1
  //============================================================
  // ... Test abstract operator:
  //  TYPEArray & TYPEArray::Abstract_Binary_Operator 
  //   (const TYPEArray & Lhs , const TYPEArray & Rhs ,
  //    Prototype_2 Operation_Array_Data ,
  //    Prototype_3 Operation_Array_Data_Accumulate_To_Existing_Operand,
  //    int Operation_Type )
  //
  // ... test A =  Lhs + Rhs ...
  //============================================================

  TYPEArray G1;
  G1.redim(A(all,all,all,103));
  G1=0.;
//printf ("G1.Array_ID() = %d \n",G1.Array_ID());
//printf (" A.Array_ID() = %d \n",A.Array_ID());
//printf (" B.Array_ID() = %d \n",B.Array_ID());
//printf ("START: where (A(all,all,all,102)>(TYPE)7.) G1 = A(all,all,all,104)+ B; \n");
//APP_DEBUG = 4;
  where (A(all,all,all,102)>(TYPE)7.) G1 = A(all,all,all,104)+ B;
//APP_DEBUG = 0;
//printf ("DONE:  where (A(all,all,all,102)>(TYPE)7.) G1 = A(all,all,all,104)+ B; \n");

  datptr = G1.getDataPointer();
  sum1=0.;
  for (i=0;i<726;i++) 
    sum1+=datptr[i];

  // ... A(all,all,all,102) has elements set to 9 at i=6,8, 
  //    j=2,4 and k=-4 to -2. These are the only places where
  //    A > 7.  Elements of B are set to i so G1 has (-2--4)*2
  //    = 6 elements set to 6+9 and 6 elements set to 8+9. ...

  m1 = (14.+18)*6;
  if (sum1 != m1)
  {
    cout<<"ERROR in Lhs+Rhs,test1"<<endl<<
    "  sum1="<<sum1<<endl;
    cout<<"  correct answer is: sum1="<<m1<<endl;
  }
  else
    cout<<"A = Lhs + Rhs, test1 ok"<<endl;

//printf("Exit until this part is working! \n");
//APP_ABORT();
#endif
  // ... test A = min (X,Y) ...

  TYPEArray G2;
  G2 = min(B(all,all,-6),D(all,all,-5)+C);

  datptr = G2.getDataPointer();
  sum1=0.;
  for (i=0;i<121;i++) 
    sum1+=datptr[i];

  // ... D = 12-B so D+C = 7-B which ranges from 5 to -5 (7-2 to 7-12).
  //    B is minimal only for i=2 and i=3. D+3 is minimal for i=4 to
  //    i=12 where D+C has elements 3 to -5. Elements in B and D
  //    only vary with i.  The j direction is dimension 11. ...

  m1 = 2.+3.+3.+2.+1.-1.-2.-3.-4.-5.;
  m1*=11;
  if (sum1 != m1)
  {
    cout<<"ERROR in min(Lhs,Rhs),test1"<<endl<<
    "  sum1="<<sum1<<endl;
    cout<<"  correct answer is: sum1="<<m1<<endl;
  }
  else
    cout<<"A = min(Lhs,Rhs), test1 ok"<<endl;

  //============================================================
  // ... Test abstract operator:
  //  TYPEArray & TYPEArray::Abstract_Binary_Operator 
  //   (const TYPEArray & Lhs , TYPE x ,
  //    Prototype_4 Operation_Array_Data ,
  //    Prototype_5 Operation_Array_Data_Accumulate_To_Existing_Operand,
  //    int Operation_Type )
  //
  // member functions
  //  A = x + Rhs
  //  A = Lhs / x
  //
  // friend functions calling this
  //  A = min (X,y);
  //============================================================

  TYPEArray P1;
  P1=B(all,all,-1);

  P1=min(P1,(TYPE)5.);

  datptr = P1.getDataPointer();
  sum1=0.;
  for (i=0;i<121;i++) 
    sum1+=datptr[i];

  // ... B(all,all,-1) is a 2d array varying only with i
  //   with values 2 to 12.  The min of 5 and B(all,all,-1)
  //   has values 2,3,4,5,..,5 with 8 5's. The sum in the
  //   i direction is 49 and the j direction has dimension 11 ...

  m1 = 49. *11;
  if (sum1 != m1)
  {
    cout<<"ERROR in min(X,y),test1"<<endl<<
    "  sum1="<<sum1<<endl;
    cout<<"  correct answer is: sum1="<<m1<<endl;
  }
  else
    cout<<"A = min(X,y), test1 ok"<<endl;

//printf("Exit until this part is working! \n");
//APP_ABORT();

  
  /* STILL DOESN'T WORK, #2 */
  /*
  intArray IN(3);
  IN(0) = 5;
  IN(1) = 7;
  IN(2) = 9;
  TYPEArray P2;
  P2 = D(IN+1,-2,-6); // P2 is a 1d array with elements 6,4,2

  TYPEArray P3;
  P3 = min(P2,6); // P3 is the same as P2

  datptr = P3.getDataPointer();
  sum1=0.;
  for (i=0;i<3;i++) 
    sum1+=datptr[i];

  m1 = 12;
  if (sum1 != m1)
  {
    cout<<"ERROR in min(X,y),test2"<<endl<<
    "  sum1="<<sum1<<endl;
    cout<<"  correct answer is: sum1="<<m1<<endl;
  }
  else
    cout<<"A = min(X,y), test2 ok"<<endl;
  */


  TYPEArray P4;
  P4.copy(B);

  TYPEArray P5(11,11,6);
  P5 = D;

  TYPEArray P6;
//P6 = -12 + (P5+P4)(all,all,3);
  P6 = TYPE(-12.0) + TYPEArray(P5+P4)(all,all,3);

  datptr = P6.getDataPointer();
  sum1=0.;
  for (i=0;i<121;i++) 
    sum1+=datptr[i];

  m1 = 0;
  if (sum1 != m1)
  {
    cout<<"ERROR in A=x+Rhs,test1"<<endl<<
    "  sum1="<<sum1<<endl;
    cout<<"  correct answer is: sum1="<<m1<<endl;
  }
  else
    cout<<"A=x+Rhs, test1 ok"<<endl;


//printf("Exit until this part is working! \n");
//APP_ABORT();

  //============================================================
  // ... Test abstract function:
  //  TYPEArray & TYPEArray::Abstract_Modification_Operator 
  //   (const intArray & Lhs , const TYPEArray & Rhs ,
  //    Prototype_6 Operation_Array_Data,
  //    int Operation_Type )
  //
  //   A = replace(ILhs,Rhs) (A can't be an intArray)
  //============================================================

  TYPEArray M1(Ispan,Jspan);
  M1 = 5.;

  intArray ILhs(Ispan,Jspan);
  ILhs = 1;
  where (B(all,all,-3) > (TYPE)2.) ILhs = 0; // ILhs is 0 except for j-k
				      // plane at i=2

//ILhs.display("ILhs");
//B.display("B");
//M1.display("M1 (before)");

  TYPEArray M2;
//M2.display("M2 (before)");

  M2 = M1.replace(ILhs, B(all,all,-2)); 
			    // j=k plane at i=2 of M1 is replaced with
			    // same plane from B which is filled
			    // with 2

//M1.display("M1 (after)");
//M2.display("M2 (after)");

  datptr = M2.getDataPointer();
  sum1=0.;
  for (i=0;i<121;i++) 
    sum1+=datptr[i];

  // ... 2*11 + 5*10*11 ...
  m1 = 5.*110 + 22.;
  if (sum1 != m1)
  {
    cout<<"ERROR in A.replace(ILhs,Rhs),test2"<<endl<<
    "  sum1="<<sum1<<endl;
    cout<<"  correct answer is: sum1="<<m1<<endl;
  }
  else
    cout<<"A.replace(ILhs,Rhs), test2 ok"<<endl;

//printf("Exit until this part is working! \n");
//APP_ABORT();

  //============================================================
  // ... Test with abstract function:
  //  TYPEArray & TYPEArray::Abstract_Modification_Operator 
  //   (const intArray & Lhs , TYPE x,
  //    Prototype_7 Operation_Array_Data,
  //    int Operation_Type )
  //
  // Test with function A = replace (ILhs,x)
  //============================================================

  /* seqAdd DOESN'T WORK ON T3D, TURN OFF THIS SECTION */
  /*
  int csize = C.rows()*C.cols();

#if defined(USE_PPP)
  TYPE* fptr = new TYPE[csize];
  for (i=0;i<csize;i++) fptr[i]=1.;
  TYPEArray M3(csize); 
  M3.fill(1);
#else
  TYPE* fptr = new TYPE[csize];
  for (i=0;i<csize;i++) fptr[i]=1.;
  TYPEArray M3; 
  M3.adopt(fptr,csize);
#endif

  M3.reshape(C.getLength(0),C.getLength(1));
  M3.setBase(-1);
  M3(Ispan1,Jspan1).seqAdd(5);

  intArray Ilhs(Ispan,Jspan);
  Ilhs = 0;
  where (M3>2) Ilhs=1;

  M3.replace(Ilhs,5);

  datptr = M3.getDataPointer();
  sum1=0.;
  for (i=0;i<121;i++) 
    sum1+=datptr[i];

  // ... 11*11 -(6*6*1)+(6*6*5) ...
  m1 = 121+36*4;
  if (sum1 != m1)
  {
    cout<<"ERROR in A.replace(ILhs,x),test1"<<endl<<
    "  sum1="<<sum1<<endl;
    cout<<"  correct answer is: sum1="<<m1<<endl;
  }
  else
    cout<<"A.replace(ILhs,x), test1 ok"<<endl;

  intArray Ibig = (M3>3).indexMap();
  intArray Isml = (M3<=3).indexMap();

  sum2=0.;
  for(i=0;i<Ibig.getLength(0);i++)
  {
    sum2+=datptr[Ibig(i)];
  }
  for(i=0;i<Isml.getLength(0);i++)
  {
    sum2+=datptr[Isml(i)];
  }

  // ... this should be the same as m1 above ...
  if (sum2 != m1)
  {
    cout<<"ERROR in I.indexMap(), test1"<<endl<<
    "  sum2="<<sum2<<endl;
    cout<<"  correct answer is: sum2="<<m1<<endl;
  }
  else
    cout<<"I.indexMap(), test1 ok"<<endl;

  */

//B(all,all,0).display("B(all,all,0)");

  //============================================================
  // ... Test abstract operator:
  //  TYPEArray & TYPEArray::Abstract_Modification_Operator 
  //    const TYPEArray & Lhs , int x,
  //    Prototype_8 Operation_Array_Data,
  //    int Operation_Type )
  //
  // Test with function A = replace (ix, Lhs) (A can't be an
  // intArray)
  //============================================================

  TYPEArray N1(Ispan,11);
  N1=7;
  N1.copy(C);
#if defined (USE_PPP)
  TYPEArray N2(N1.getLength(0),N1.getLength(1),
    N1.getLength(2),N1.getLength(3)); 
    N2 = N1;  // N2 is -5
#else
  TYPEArray N2(N1.getDataPointer(),N1.getLength(0),N1.getLength(1),
    N1.getLength(2),N1.getLength(3)); // N2 is -5
#endif
  TYPEArray N3;
  N3.reference(N2); // N3 is -5
//printf ("START: N3 = N2+N1; \n");
  N3 = N2+N1;  // N3 and N2 both are filled with -10
//printf ("DONE: N3 = N2+N1; \n");

//N1.display("N1 (before breakReference)");
//N2.display("N2 (before breakReference)");
//N3.display("N3 (before breakReference)");

  N1.replace(1,N2); // N1 is filled with -10
  N3.breakReference();
  N3 = 0; // N2 stays at -10 

//N1.display("N1 (after breakReference)");
//N2.display("N2 (after breakReference)");
//N3.display("N3 (after breakReference)");

//printf ("START: N1 = N2 - N1; \n");
//APP_DEBUG = 4;
  N1 = N2 - N1; // N1 is 10 --10
//APP_DEBUG = 0;
//printf ("DONE: N1 = N2 - N1; \n");

  datptr = N1.getDataPointer();
  sum1=0.;
  for (i=0;i<121;i++) 
    sum1+=datptr[i];

  m1 = 0;
  if (sum1 != m1)
  {
    cout<<"ERROR in A.replace(ix,Lhs),test1"<<endl<<
    "  sum1="<<sum1<<endl;
    cout<<"  correct answer is: sum1="<<m1<<endl;
  }
  else
    cout<<"A.replace(ix,Lhs), test1 ok"<<endl;

//printf("Exit until this part is working! \n");
//APP_ABORT();

  //============================================================
  // ... Test abstract operator:
  //  void TYPEArray::Abstract_Operator_Operation_Equals 
  //   (const TYPEArray & Lhs , const TYPEArray & Rhs ,
  //    Prototype_3 Operation_Array_Data_Accumulate_To_Existing_Operand ,
  //    int Operation_Type )
  // 
  // Test functions
  //   A+=Rhs
  //   A/=Rhs
  //============================================================

  TYPEArray Q1;
  Q1 = B(all,all,-6); // B(all,all,-6) ranges from 2 to 12 only
		      // changing in the i direction
  Q1+=C; // Q1 ranges from -3 to 7

  datptr = Q1.getDataPointer();
  sum1=0.;
  for (i=0;i<121;i++) 
    sum1+=datptr[i];

  // ... (2+3+...+12) = 77 so B(all,all,-6) elements sum to
  //    77*11 before adding -5 to each of the 121 elements ...

  m1 = 77.*11 - (5.*121);
  if (sum1 != m1)
  {
    cout<<"ERROR in A+=Rhs,test1"<<endl<<
    "  sum1="<<sum1<<endl;
    cout<<"  correct answer is: sum1="<<m1<<endl;
  }
  else
    cout<<"A+=Rhs, test2 ok"<<endl;

  TYPEArray Q2;
  Q2 = B+D; // Q2 is set to 12 everywhere
//Q2.view("Q2");
  where (B(all,all,-5)< (TYPE)2.) Q2(all,all,3)/=C; 
                     // no elements of B are < 2 so Q2 isn't
		     // changed

  datptr = Q2.getDataPointer();
  sum1=0.;
  for (i=0;i<121;i++) 
    sum1+=datptr[i];

  m1 = 12.*121;
  if (sum1 != m1)
  {
    cout<<"ERROR in A/=Rhs,test1"<<endl<<
    "  sum1="<<sum1<<endl;
    cout<<"  correct answer is: sum1="<<m1<<endl;
  }
  else
    cout<<"A/=Rhs , test1 ok"<<endl;

  //============================================================
  // ... Test abstract operator:
  //  void TYPEArray::Abstract_Operator_Operation_Equals 
  //    const TYPEArray & Lhs , TYPE x ,
  //    Prototype_5 Operation_Array_Data_Accumulate_To_Existing_Operand ,
  //    int Operation_Type )
  //
  // Test functions
  //   A++
  //   A/=x
  //============================================================

  TYPE* bptr = new TYPE[B.elementCount()];

#if defined(USE_PPP)
  TYPEArray BB(Ispan,Jspan,Kspan);
#else
  TYPEArray BB(bptr,Ispan,Jspan,Kspan);
#endif

  BB.fill(7.);

  TYPEArray DD;
  DD=C;
  DD.resize(D.getLength(0),D.getLength(1),D.getLength(2));
  Index most(1,5);
  // ... (10/18/96,kdb) resize function has changed so test must change.
  //  Now intersection that is set doesn't contain 0 and 1 in first dimension
  //  of DD and 9 and 10 in second dimension so these are now only 81 
  //  values set to -5 ...
  Index firstEdge(0,2);
  Index secondEdge(9,2);
  DD(all,all,most)=0;
  DD(firstEdge,all,all) = 0.;
  DD(all,secondEdge,all) = 0.;
  DD++; // DD(all,all,0) is -4, DD(all,all,most) is 1

  TYPEArray EE(Ispan,Jspan);
  EE = 0.;
  EE = BB(all,all,-3)+DD(all,all,0); // EE is filled with 3 
  EE/=1.;

  datptr = EE.getDataPointer();
  sum1=0.;
  for (i=0;i<121;i++) 
    sum1+=datptr[i];

  // ... there are 81 elements set to -4 + 7 and 20 elements set to 
  //  1 + 7 so total is 3 * 81 + 40 * 8 = 563.
  m1 = 563.;
  if (sum1 != m1)
  {
    cout<<"ERROR in A/=x,test1"<<endl<<
    "  sum1="<<sum1<<endl;
    cout<<"  correct answer is: sum1="<<m1<<endl;
  }
  else
    cout<<"A/=x , test1 ok"<<endl;

  delete bptr;

//printf("Exit until this part is working! \n");
//APP_ABORT();

  //============================================================
  // ... Test abstract operator:
  //  intArray & TYPEArray::Abstract_Unary_Operator_Returning_IntArray 
  //   (const TYPEArray & X ,
  //    Prototype_9  Operation_Array_Data ,
  //    Prototype_10 Operation_Array_Data_Accumulate_To_Existing_Operand ,
  //    int Operation_Type )
  //
  // Test with only function using this, IA = !A
  //============================================================

#if 1
   // UPPER main if for tracing destructor bug

  //============================================================
  // ... Test abstract operator:
  //  intArray & TYPEArray::Abstract_Binary_Operator_Returning_IntArray 
  //   (const TYPEArray & Lhs , const TYPEArray & Rhs ,
  //    Prototype_11 Operation_Array_Data ,
  //    Prototype_9  Operation_Array_Data_Accumulate_To_Existing_Operand ,
  //    int Operation_Type )
  //
  //   IA = Lhs == Rhs
  // ... test IA = Lhs < Rhs ...
  //============================================================

  intArray IA1;
  TYPEArray H1,H2;

  int isum1,im1;
  int* idatptr;

//B(all,all,0).display("B(all,all,0)");
//D(all,all,0).display("D(all,all,0)");
//H1.reference((B-D)(all,all,0)); // H1 has elements -8,-6,...,10,12
// H1.reference(TYPEArray(B-D)(all,all,0)); // H1 has elements -8,-6,...,10,12
  H1.reference(TYPEArray(B-D)(all,all,-6)); // H1 has elements -8,-6,...,10,12
  //H1.setBase(3,1);  put this in when bug is fixed
  int h1base1 = H1.getBase(1);
  H2 = H1(all,h1base1+10); // H1 only varies with i, H2 is 1d
                           // with even values -8 to 12 
  //H2 = H1(all,13); non bug version
  TYPEArray H3(Ispan);
  H3 = 2.;
//IA1.display("IA1");
//H1.display("H1");
//H2.display("H2");
//H3.display("H3");
  IA1 = H2 < H3; // H2 is less than H3 for H2 values -8,-6,-4,-2
                 // and 0
//IA1.display("IA1");

  idatptr = IA1.getDataPointer();
  isum1=0;
  for (i=0;i<11;i++) 
    isum1+=idatptr[i];

  // ... there are 5 elements in IA1 set to 1 corresponding to
  //    -8,...,0 ...

  im1 = 5;
  if (isum1 != im1)
  {
    cout<<"ERROR in IA = Lhs < Rhs,test1"<<endl<<
    "  isum1="<<isum1<<endl;
    cout<<"  correct answer is isum1="<<im1<<endl; 
  }
  else
    cout<<"IA = Lhs < Rhs , test1 ok"<<endl;

//printf("Exit until this part is working! \n");
//APP_ABORT();

#else
  int isum1,im1;
  int* idatptr;
#endif
  // UPPER main if to trace destructor problem

#if 1
  // main if to trace destructor problem

  //============================================================
  // ... Test abstract operator:
  //  intArray & TYPEArray::Abstract_Binary_Operator_Returning_IntArray 
  //   (const TYPEArray & Lhs , TYPE x ,
  //    Prototype_12 Operation_Array_Data ,
  //    Prototype_13 Operation_Array_Data_Accumulate_To_Existing_Operand ,
  //    int Operation_Type )
  //
  // Test with functions
  //   IA = Lhs >= x
  //   IA = x || Rhs
  //============================================================

  intArray IA;
  TYPEArray O1(Ispan);
  O1 = 0.;
  O1(2) = -2.;
  O1(3) = -1.;
  O1(4) = 0.;
  O1(5) = 1.;
  O1(6) = 2.;

  TYPE w=0.;
  IA = w || O1; // IA is set to 1 only where O1 is nonzero
		// at i=2,3,5 and 6

  idatptr = IA.getDataPointer();
  isum1=0;
  for (i=0;i<11;i++) 
    isum1+=idatptr[i];

  im1=4;
  if (isum1 != im1)
  {
    cout<<"ERROR in IA=x||A,test1"<<endl<<
    "  isum1="<<isum1<<endl;
    cout<<"  correct answer is: isum1=4"<<endl;
  }
  else
    cout<<"IA= X||A , test1 ok"<<endl;

#endif
#if 1
  //============================================================
  // ... Test abstract operator:
  //  TYPEArray & TYPEArray::operator= ( const TYPEArray & Rhs )
  //
  // Test only function that calls this, A=Rhs
  //============================================================

  //TYPEArray R1;
  //where (B==2) R1 = B+D;


  //============================================================
  // ... Test abstract operator:
  //  TYPEArray & TYPEArray::operator= ( TYPE x )
  //
  // test with only function from a) A=x
  //============================================================

  TYPEArray U1(Ispan,Jspan);
  TYPE x,y;
  x=3.;
  y=7.;
  where (A(all,all,-6,100) < (TYPE)2.) U1 = x+y; // A(all,all,-6,100) has
                                         // all 121 elements set to 1
					 // so U1 has 121 elements
					 // set  to 10.

  datptr = U1.getDataPointer();
  sum1=0.;
  for (i=0;i<121;i++) 
    sum1+=datptr[i];

  m1=1210.;
  if (sum1 != m1)
  {
    cout<<"ERROR in A=x,test1"<<endl<<
    "  sum1="<<sum1<<endl;
    cout<<"  correct answer is: sum1=1210"<<endl;
  }
  else
    cout<<"A=x , test1 ok"<<endl;

  //============================================================
  // ... Test friend functions using abstract operators ...
  //
  //     (all friend functions)
  //       A = min (X,Y,Z)
  //       A = min (X,y,Z)
  //       A = sum (X,Axis) 
  //       A = transpose (X) (only works for 2D arrays)
  //============================================================

  // ... test A = min (X,Y,Z) ...

  TYPEArray X1(Ispan,Jspan,Kspan);
  X1 = 1.;
  TYPEArray X2;
  X2 = min(X1,B-D,B+D); // B-D ranges from -8 to 12, B+D is 12
			// so X2 has elements -8,-6,-4,-2,0,
			// and then 6 1's for each j-k plane.

  datptr = X2.getDataPointer();
  sum1=0.;
  for (i=0;i<726;i++) 
    sum1+=datptr[i];

  // ... -8+-6+-4+-2+0+1+1+1+1+1+1=-14, each j-k plane has 11*6
  //     elements ...

  m1 = -14.*11*6;
  if (sum1 != m1)
  {
    cout<<"ERROR in A=min(X,X1,Z),test1"<<endl<<
    "  sum1="<<sum1<<endl;
    cout<<"  correct answer is: sum1="<<m1<<endl;
  }
  else
    cout<<"A=min(X,X1,Z) , test1 ok"<<endl;



  // ... test A = min (X,y,Z) ...

  y = 10.;
  TYPEArray Z1;
  where (B> (TYPE)9.) Z1 = min(B,y,D); // B>9 at i=10,11,12 where elements
				// have value i. D has elements 
				// 2,1 and 0 at these positions.
				// Z1 will have elements 2,1 and 0
				// where i=10,11 and 12.
  where (B<= (TYPE)9.) Z1 = 0.;  // the rest of Z1 is set to 0.

  datptr = Z1.getDataPointer();
  sum1=0.;
  for (i=0;i<726;i++) 
    sum1+=datptr[i];

  // ... each j=k plane with 11*6 elements sums to (0+1+2)=3 ...
  m1 = 3.*11*6;
  if (sum1 != m1)
  {
    cout<<"ERROR in A=min(X,y,Z),test1"<<endl<<
    "  sum1="<<sum1<<endl;
    cout<<"  correct answer is: sum1="<<m1<<endl;
  }
  else
    cout<<"A=min(X,y,Z) , test1 ok"<<endl;


  // ... test A = sum (X,Axis) ... 

#if !defined(USE_EXPRESSION_TEMPLATES)
// Skip this test since there is not expression template version of the
// sum along axis function!

  TYPEArray W1;
   W1 = sum(sum(B,0),1); // Sum(B,0) is a 1 x 11 x 6 array with
                        // each element set to 2+3+...+12=77,
			// sum(sum(B,0),1) is a 1 x 1 x 11 array
			// with each element set to 11*77 

  datptr = W1.getDataPointer();
  sum1=0.;
  for (i=0;i<6;i++) 
    sum1+=datptr[i];
  m1 = 77.*11.*6;


  TYPEArray ONE(Ispan,Jspan,Kspan);
  ONE = 1.;

  TYPEArray W2;
  Index JJ(0,3);
//W2 = sum(B(all,JJ,-3)*ONE(all,JJ,-3),1); 
// We need to specify the type since there are sum functions 
// taking each of doubleArray, floatArray, and intArray
  W2 = sum(TYPEArray(B(all,JJ,-3)*ONE(all,JJ,-3)),1); 
                             // Sum(B(all,JJ,-3)*ONE(all,JJ,-3),1) is an 
			     // 11 x 1 x 1 with each element set
			     // to i*3 for B(i,j,k)
  datptr = W2.getDataPointer();
  sum2=0.;
  for (i=0;i<11;i++) 
    sum2+=datptr[i];
  m2 = 77. * 3.;


  if (sum1 != m1 || sum2 != m2)
  {
    cout<<"ERROR in A=sum(X,axis),test1"<<endl<<
    "  sum1="<<sum1<<", sum2="<<sum2<<endl;
    cout<<"  correct answer is: sum1="<<m1<<", sum2="<<m2<<endl;
  }
  else
    cout<<"Not implemented in expression template version: A=sum(X,axis) , test1 ok"<<endl;
#endif

  // ... test A = transpose (X) (only works for 2D arrays) ...

  TYPEArray V1,V2;
  V1 = transpose (B(all,all,-6)); // V1 is an 11 x 11 array with
				  // elements varying with j ranging
				  // from 2 to 12
  V2 = V1(all,0); // V2 is an 11 x1 array that has 2 for its
                  // elements

  datptr = V2.getDataPointer();
  sum1=0.;
  for (i=0;i<11;i++) 
    sum1+=datptr[i];

  m1 = 22.;
  if (sum1 != m1)
  {
    cout<<"ERROR in A=transpose(X),test1"<<endl<<
    "  sum1="<<sum1<<endl;
    cout<<"  correct answer is: sum1=22"<<endl;
  }
  else
    cout<<"A=transpose(X) , test1 ok"<<endl;

  //===========================================================
  // ... test setting a single value with ranges (Bill H.'s bug) ...

  intArray num(2,3);
  num = 0;
  num(Range(0,0),Range(1,1)) = 5.;
  idatptr = num.getDataPointer();
  if (idatptr[2]!=5)
  {
    num.display("num");
    cout<<"ERROR in num(RANGE(0,0),Range(1,1))=5."<<endl;
  }
  else
  {
    cout<<"num(RANGE(0,0),Range(1,1))=5. test okay"<<endl;
  }

  //===========================================================
  // ... make some scalar indexing tests ...

  TYPEArray AA;
  AA.reference(A(Ispan2,Jspan2,-4,102));
  sum1 = 0.;
  int jbase = Jspan2.getBase();
  int ibase = Ispan2.getBase();
  for (j=jbase;j<Jspan2.getCount()+jbase;j++)
     for (i=ibase;i<Ispan2.getCount()+ibase;i++)
       sum1 += AA(i,j,-4,102);

  if (sum1 != 36.)
  {
    cout<<"ERROR in scalar indexing test1"<<endl<<
       " sum1 = "<<sum1<<", correct answer = 36."<<endl; 
  }
  else
  {
    cout<<"scalar indexing test1 okay"<<endl;
  }


  TYPEArray CC;
  TYPEArray CCC(0);
  CC.resize(11,11);
  CCC.reference(CC(Range(5,7),7));
  CCC.redim(0);
  CC(5,5) = 7.;
  CC.redim(Ispan,Jspan);
  CC = C;
  //CC.redim(11,11);
  CC.redim(Jspan,Ispan);

//Ispan.display("Ispan");
//Jspan.display("Jspan");
//Ispan2.display("Ispan2");
//CC = 1;
//CC.display("CC (after redim before asignment)");

  for (i=ibase;i<Ispan2.getCount()+ibase;i++)
  {
    CC(2,Ispan2)(2,i) = -4.;
  }

//CC(2,6) = 8;
//CC(2,8) = 8;

//CC.display("CC (before resize)");

//CC.view("CC (before resize)");
  CC.resize(Ispan,Jspan);

//CC(2,6) = 9;
//CC(2,8) = 9;

  printf ("CC(2,6) = %f  CC(2,8) = %f \n",CC(2,6),CC(2,8));
  cout << "  CC(2,6) = " << C(2,6) << ", CC(2,8) = " << C(2,8) << endl;
  float x1 = CC(2,6);
  float x2 = CC(2,8);
  cout << "  CC(2,6) = " << x1 << ", CC(2,8) = " << x2 << endl;
  cout << "  CC(2,6) = " << C(2,6) << ", CC(2,8) = " << C(2,8) << endl;

//CC.display("CC (after resize)");
//CC.view("CC (after resize)");

  if (CC(2,6) != -4. || CC(2,8) != -4.)
  {
    cout<<"ERROR in scalar indexing test2,"<<endl;
    cout<<"  CC(2,6) = "<<C(2,6)<<", CC(2,8) = "<<C(2,8)<<endl;
    cout<<"  both should be -4."<<endl;
  }
  else
  {
    cout<<"scalar indexing test2 okay"<<endl;
  }

//printf("Exit until this part is working! \n");
//APP_ABORT();

  // ... temp test ...
  TYPEArray WW(Ispan2);
  WW = -17.;



  TYPEArray ZZ(Ispan);
  ZZ = -15.;

  Index I1(4,5,2);
  Index I2(6,2,2);

  ZZ(I1)(I2)(6) = -1.;
  ZZ(I1)(I2)(7) = -1.;

  if (ZZ(8) != -1. || ZZ(12) != -1.)
  {
    cout<<"ERROR in scalar indexing test3"<<endl;
    cout<<"  ZZ(8) = "<<ZZ(8)<<", ZZ(12) = "<<ZZ(12)<<endl;
    cout<<"  both should be -1."<<endl;
  }
  else
  {
    cout<<"scalar indexing test3 okay"<<endl;
  }


  TYPEArray a(Range(-2,2),Range(-2,2));
  for( j=-2; j<=2; j++ )
     for( i=-2; i<=2; i++ )
        a(i,j)=i+5*j;
  
  TYPEArray b;
  b.redim(5,5); b.setBase(-2);
  for( j=-2; j<=2; j++ )
     for( i=-2; i<=2; i++ )
        b(i,j)=i+5*j;

  int testok;
  TYPE *aptr;
  aptr = a.getDataPointer();
  bptr = b.getDataPointer();
  for (i=0;i<25;i++)
    testok = (aptr[i] == bptr[i]);

  if (testok)
    cout<<"scalar indexing test4 okay"<<endl;
  else
  {
    cout<<"ERROR in scalar indexing test4"<<endl;
    cout<<"   global setBase probably didn't work correctly"<<endl;
  }

  TYPEArray ZSerial = ZZ.getLocalArray();
  ZSerial = 22.;
  ZZ.display("ZZ");

  TYPEArray ZZSerial = ZZ.getLocalArrayWithGhostBoundaries();
  ZSerial = 15.;
  ZZSerial.display("ZZSerial");
  ZZ.display("ZZ");

  //================= P++ finish ==============================
  //Communication_Manager::sync();

  printf ("Program Terminated Normally! \n");
#if defined(USE_PPP)
  Optimization_Manager::Exit_Virtual_Machine ();
  printf ("Virtual Machine exited! \n");
#endif

  //===========================================================

   // Main endif to trace destructor problems!
#endif
  return (0);

}


#else
int main ()
   {
  // Index::setBoundsCheck(on);

#if 1
     floatArray A(10);
     floatArray B(10);
     A = 3;
     B = 5;
     A.display("A");
     B.display("B");
     floatArray C(A+B);
     C.display("C");
     C = sin(A);
     C.display("C");
#endif

     printf ("Alternate main program terminated normally! \n");
     return 0;
  // APP_ABORT();
   }
#endif
