#define USE_PPP
//#define TEST_INDIRECT

//define TEST_ARRAY_OVERFLOW
//================================================================
// A++ library test P++ code.  Version 1.2.9 
//================================================================

//#define BOUNDS_CHECK

#include <A++.h>
#include <iostream.h>

#define TYPE float
#define TYPEArray floatArray
#define TYPESerialArray floatSerialArray

#if 0




#endif

//Range APP_Unit_Range(0,0);

int main(int argc,char * * argv)
{ 
   MemoryManagerType Memory_Manager; 

   //Index::setBoundsCheck(on);

   // ... this is an attempt to put a stop in the code for mpi ...
  int pid_num=getpid(); 
  cout<<("pid=")<<(pid_num)<<(endl); 

  int wait1; 
  cout<<("Type first number to continue.")<<(endl); 
  cin>>(wait1); 

  //================= P++ startup =============================

  printf("This is the new testppp, with 2 processors. \n"); 

  // ... leave this in with A++ also for debugging ...

  int Number_Of_Processors=1; 

  Optimization_Manager::Initialize_Virtual_Machine("",Number_Of_Processors,argc,argv); 


  printf("Run P++ code(Number Of Processors=%d) \n",Number_Of_Processors); 


  //===========================================================

  //int pid_num = getpid();
  pid_num = getpid(); 
  cout<<("pid=")<<(pid_num)<<(endl); 

#if defined(MPI)
  // ... this is an attempt to put a stop in the code for mpi for debugging
  //  but this isn't necessary and doesn't work for pvm ...



#endif

  int ghost_cell_width=0; 
  //int ghost_cell_width = 1;
  Partitioning_Type::SpecifyDefaultInternalGhostBoundaryWidths(ghost_cell_width); 


  Range Ispan(2,12); 
  Range Jspan(-2,8); 
  Range Kspan(-6,-1); 
  Range Lspan(100,105); 

 floatArray A(Ispan,Jspan,Kspan,Lspan); 
 floatArray B(Ispan,Jspan,Kspan); 
 floatArray C(Ispan,Jspan); 
  C=-5; 
 floatArray D(Ispan,Jspan,Kspan); 

  Index Ispan1(4,6); 
  Index Jspan1(0,6); 
  Index Kspan1(-5,4); 
  Index Lspan1(101,4); 

  Index Ispan2(6,2,2); 
  Index Jspan2(2,2,2); 
  Index Kspan2(-4,3); 
  Index Lspan2(102,3); 

  Index all; 

  int i; int j; int k; 
  int gc; 

  //============================================================

 floatArray ZZ(Ispan); 
  ZZ=-15; 
  //ZZ.display("ZZ");

  printf("This is the new testppp. \n"); 

#if defined(MPI)
  // ... this is an attempt to put a stop in the code for mpi for debugging
  //  but this isn't necessary and doesn't work for pvm ...



#endif

  A=1; 
  A(Ispan1,Jspan1,Kspan1,Lspan1)=4; 
  A(Ispan2,Jspan2,Kspan2,Lspan2)=9; 


  Optimization_Manager::Optimize_Scalar_Indexing = 1; 
  /*
  TYPESerialArray* BBloc = B.getSerialArrayPointer();
  for (k=BBloc->getBase(2);k<=BBloc->getBound(2);k++)
    for (j=BBloc->getBase(1);j<=BBloc->getBound(1);j++)
      for (i=BBloc->getBase(0);i<=BBloc->getBound(0);i++)
       	(*BBloc)(i,j,k) = (TYPE)i;
	*/
#if 0




#endif

  C=-5; 
  /*
  TYPESerialArray* DDloc = D.getSerialArrayPointer();
  int DBound = D.getBound(0);
  for (k=BBloc->getBase(2);k<=BBloc->getBound(2);k++)
    for (j=BBloc->getBase(1);j<=BBloc->getBound(1);j++)
      for (i=BBloc->getBase(0);i<=BBloc->getBound(0);i++)
	(*DDloc)(i,j,k) = (TYPE)(DBound -i);
	*/
#if 0




#endif
  Optimization_Manager::Optimize_Scalar_Indexing = 0; 

  //============================================================
  // ... Tests abstract operator:
  //    TYPE TYPEArray::Abstract_Unary_Operator 
  //      (const TYPEArray & X ,
  //       Prototype_14 Operation_Array_Data , int Operation_Type )
  //
  //   Test with friend function, x = max(X) ...
  //
  //============================================================

 float m1; float m2; float m3; float m4; 
  m1 = max(A);   // A has values 1,4 and 9
  m2 = max(B);   // B has values up to 12
  m3 = max(C);   // all of C is -5

  if ((m1 != 9.0 || m2 != 12.0) || m3 != -5.0)
  { 
    cout<<("ERROR in max(X), test1")<<(endl)<<("  m1,m2,m3=")<<(m1)<<(" ")<<(m2)<<(" ")
    <<(m3)<<(endl); 
    cout<<("  correct answers are: m1=9., m2=12.,m3=-5.")<<(endl); 
  } 
  else 
    { cout<<("max(X) test1 ok")<<(endl); } 

    //intArray Mask = (A>(TYPE)1. && A<(TYPE)9.);
    //Mask.display("Mask");

    //where (Mask) m1 = max(A);  // only value besides 1 
  where (A>1&&A<9){ m1 = max(A); }   // only value besides 1 
  // and 9 is 4
  where (B<7){ m2 = max(B); }   // B has all integer vals between
  // 2 and 12
  where (C<0){ m3 = max(C); }   // this is all of C
  where (B>6){ m4 = max(D); }   // vals in D are 12 - vals in B so
  // the max is where D is 0 to 5

  if (((m1 != 4.0 || m2 != 6.0) || m3 != -5.0) || m4 != 5.0)
  { 
    cout<<("ERROR in max(X), test2")<<(endl)<<("  m1,m2,m3,m4=")<<(m1)<<(" ")<<(m2)<<(" ")
    <<(m3)<<(" ")<<(m4)<<(endl); 
    cout<<("  correct answers are: m1=4, m2=6,m3=-5,m4=5 ")<<(endl); 
  } 
  else 
    { cout<<("max(X) test2 ok")<<(endl); } 


  Index Ispan3(7,2,2); 
  Index Jspan3(2,2,2); 
  m1 = max(A(all,all,Kspan1,all));   // all of A, tests mix of
  // different Index types
  m2 = max(A(all,Jspan2,all,all));   // subset of A that includes
  // elements set to 9
  m3 = max(B(Ispan2,all,all,all));   // middle section of B in
  // direction 0, includes vals
  // 6, 7 and 8
  m4 = max(A(Ispan3,Jspan3,all,all));   // Ispan3 chooses values
  // between those set to 9


  if (((m1 != 9.0 || m2 != 9.0) || m3 != 8.0) || m4 != 4.0)
  { 
    cout<<("ERROR in max(X), test3")<<(endl)<<("  m1,m2,m3,m4=")<<(m1)<<(" ")<<(m2)<<(" ")
    <<(m3)<<(" ")<<(m4)<<(endl); 
    cout<<("  correct answers are: m1=9,m2=9,m3=8,m4=4")<<(endl); 
  } 
  else 
    { cout<<("max(X) test3 ok")<<(endl); } 


  where (B<9){ m1 = max((B+D)); }   // D is 12 -B so B+D is 12 everywhere

  if (m1 != 12.0)
  { 
    cout<<("ERROR in max(X), test4")<<(endl)<<("  m1=")<<(m1)<<(endl); 
    cout<<("  correct answer is m1=12")<<(endl); 
  } 
  else 
    { cout<<("max(X) test4 ok")<<(endl); } 


    // INDIRECT TEST 1
    //#if defined(USE_PPP)

    //#else
#if defined (TEST_INDIRECT)















    // A(4,-1,-3,103) = 1
    // A(7,2,-3,103) = 4
    // A(11,6,-3,103) = 1
    // view is 1d array with (1,4,1)














#endif
    //#endif
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

  Partitioning_Type * Single_Processor_Partition=new Partitioning_Type (Range(0,0)); 


  //int proc_num = 0;
  int proc_num=Communication_Manager::My_Process_Number; 


  //============================================================

 float * datptr; 
  int * idatptr; 
 float sum1; float sum2; 
  int isum1; int isum2; 
  int size1; int size2; int size3; int size4; 

  size1 = 11 + 2 * ghost_cell_width; 
  size2 = (11 + 2 * ghost_cell_width) * 11; 
  size3 = ((11 + 2 * ghost_cell_width) * 11) * 6; 
  size4 = (((11 + 2 * ghost_cell_width) * 11) * 6) * 6; 

  //============================================================

 floatArray T1; floatArray T2; floatArray T3; floatArray T4; floatArray T5; 
 floatArray locT1; floatArray locT2; floatArray locT3; floatArray locT4; floatArray locT5; 
  locT1.partition(*Single_Processor_Partition); 
  locT2.partition(*Single_Processor_Partition); 
  locT3.partition(*Single_Processor_Partition); 
  locT4.partition(*Single_Processor_Partition); 
  locT5.partition(*Single_Processor_Partition); 

  T1=-(B+D);   // all values are -12, size is 11 x 11 x 6
  // (include ghost cells also)
  locT1.redim(T1); 
  locT1=T1; 

  if (proc_num == 0)
  { 
    datptr = (locT1.getDataPointer)(); 
    for (j = 0; j < 66; j++)
    { 
      for (gc = 0; gc < ghost_cell_width; gc++)
      { 
        datptr[(j * size1 + gc)] = 0; 
        datptr[(((j + 1) * size1 - 1) - gc)] = 0; 
      } 
    } 


    sum1 = 0; 
    for (i = 0; i < size3; i++){ sum1 += datptr[i]; } 

    m1 = -8712; 
    if (sum1 != m1)
    { 
      cout<<("ERROR in A = -X, test1")<<(endl)<<("  sum1=")<<(sum1)<<(endl); 
      cout<<("  correct answer is ")<<(m1)<<(endl); 
    } 
    else 
      { cout<<("A =-X test1 ok")<<(endl); } 
  } 


      // ... test A = sqrt(X) (A and X can't be intArrays) ...

      // 2*2*3*3 elements set to 9 in A, 6*6*4*4 - 36 elements set to 4,
      //   11*11*6*6 - 540 - 36 elements are set to 1
  T2=sqrt(A); 
  locT2.redim(T2); 
  locT2=T2; 

  if (proc_num == 0)
  { 
    datptr = (locT2.getDataPointer)(); 
    for (j = 0; j < 396; j++)
    { 
      for (gc = 0; gc < ghost_cell_width; gc++)
      { 
        datptr[(j * size1 + gc)] = 0; 
        datptr[(((j + 1) * size1 - 1) - gc)] = 0; 
      } 
    } 
    sum1 = 0; 
    for (i = 0; i < size4; i++){ sum1 += datptr[i]; } 

    m1 = 4968; 
    if (sum1 != m1)
    { 
      cout<<("ERROR in sqrt(X), test1")<<(endl)<<("  sum1=")<<(sum1)<<(endl); 
      cout<<("  correct answer is: sum1= ")<<(m1)<<(endl); 
    } 
    else 
      { cout<<("A = sqrt(X) test1 ok")<<(endl); } 
  } 


      // ... all entries should be 3 ...
  T3=sqrt(A(Ispan2,Jspan2,Internal_Index(-3),Internal_Index(103))); 
  //A(Ispan2,Jspan2,-3,103).display("A(Ispan2,Jspan2,-3,103)");
  //sqrt(A(Ispan2,Jspan2,-3,103)).display("sqrt(A(Ispan2,Jspan2,-3,103))");
  //T3.display("T3");

#if defined(USE_PPP)
  // ... redim makes test easier because otherwise locT3 will be 
  //  11 x 11 x 6 x 6 and a view with P++ ...
  locT3.redim(2,2,1,1); 
#endif
  locT3=T3; 
  //locT3.display("locT3");

  if (proc_num == 0)
  { 
    datptr = (locT3.getDataPointer)(); 
    for (j = 0; j < 2; j++)
    { 
      for (gc = 0; gc < ghost_cell_width; gc++)
      { 
        datptr[(j * (2 + 2 * ghost_cell_width) + gc)] = 0; 
        datptr[(((j + 1) * (2 + 2 * ghost_cell_width) - 1) - gc)] = 0; 
      } 
    } 
    sum1 = 0; 
    for (i = 0; i < 2 * (2 + 2 * ghost_cell_width); i++){ sum1 += datptr[i]; } 
  } 

    // ... all entries in T4 should be 2 (because 7 and 3 are not
    //      included in Ispan2,Jspan2 )... 
  T4=sqrt(A(Internal_Index(7),Internal_Index(3),Kspan2,Lspan2)); 
#if defined(USE_PPP)
  // ... redim makes test easier ...
  locT4.redim(1,1,3,3); 
#endif
  locT4=T4; 

  if (proc_num == 0)
  { 
    datptr = (locT4.getDataPointer)(); 
    for (j = 0; j < 9; j++)
    { 
      for (gc = 0; gc < ghost_cell_width; gc++)
      { 
        datptr[(j * (1 + 2 * ghost_cell_width) + gc)] = 0; 
        datptr[(((j + 1) * (1 + 2 * ghost_cell_width) - 1) - gc)] = 0; 
      } 
    } 
    sum2 = 0; 
    for (i = 0; i < 9 * (1 + 2 * ghost_cell_width); i++){ sum2 += datptr[i]; } 

    m1 = 12; 
    m2 = 18; 

    if (sum1 != m1 || sum2 != m2)
    { 
      cout<<("ERROR in sqrt(X), test2")<<(endl)<<("  sum1=")<<(sum1)<<(" ")<<(", sum2=")
      <<(sum2)<<(endl); 
      cout<<("  correct answers are: sum1=12, sum2=18")<<(endl); 
    } 
    else 
      { cout<<("A = sqrt(X) test2 ok")<<(endl); } 
  } 

      /*------------------------------------------------------------*/
      // INDIRECT TEST 2
      //#ifndef USE_PPP 
#if defined (TEST_INDIRECT)

      // all elements for k fixed at -6 are 1
























      //#endif
#endif


 floatArray T6(A); 
 floatArray locT6; 
  locT6.partition(*Single_Processor_Partition); 

  T6=0; 
  where (A>2&&A<7){ T6=sqrt(A); }   // where masks all elements of
  // A except 4
  locT6.redim(T6); 
  locT6=T6; 

  if (proc_num == 0)
  { 
    datptr = (locT6.getDataPointer)(); 
    for (j = 0; j < 396; j++)
    { 
      for (gc = 0; gc < ghost_cell_width; gc++)
      { 
        datptr[(j * size1 + gc)] = 0; 
        datptr[(((j + 1) * size1 - 1) - gc)] = 0; 
      } 
    } 
    sum1 = 0; 
    for (i = 0; i < size4; i++){ sum1 += datptr[i]; } 

    m1 = 1080;     // 540 elements of A are 4 so 540 elements of T6 are 2,
    // the rest are 0
    if (sum1 != m1)
    { 
      cout<<("ERROR in sqrt(X), test4")<<(endl)<<("  sum1=")<<(sum1)<<(endl); 
      cout<<("  correct answer is sum1=1080")<<(endl); 
    } 
    else 
      { cout<<("A = sqrt(X) test4 ok")<<(endl); } 
  } 


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

      /*
  TYPEArray F1 = A(all,all,-6,100)-C; // view of A includes only
                                      // elements that are set to 1
  TYPEArray locF1;
  locF1.partition(*Single_Processor_Partition);
  locF1.redim(F1);
  locF1 = F1;

  if (proc_num == 0)
  {
    datptr = locF1.getDataPointer();
    for (j=0;j<11;j++)
    {
      for (gc=0;gc<ghost_cell_width;gc++)
      {
        datptr[j*size1+gc] = 0.;
        datptr[(j+1)*size1-1-gc] = 0.;
      }
    }
    sum1=0.;
    for (i=0;i<size2;i++) sum1+=datptr[i];

    m1 = 726; // 726 = (1--5)*121
    if (sum1 != m1)
    {
      cout<<"ERROR in Lhs-Rhs,test1"<<endl<<"  sum1="<<sum1<<endl;
      cout<<"  correct answer is: sum1 = 726"<<endl;
    }
    else
      cout<<"A = Lhs - Rhs, test1 ok"<<endl;
  }


  TYPEArray F2(Ispan,Jspan,Kspan);
  F2=0.;
  where (B>(TYPE)6.) F2 = B-D; // B-D = B-(12-B) = 2*B-12 so masked 
			// elements of F2 are 2*7-12 to 2*12 -12
			// which is 2,4, ..., 12.  There
			// are 11*6 of each of these

  TYPEArray locF2;
  locF2.partition(*Single_Processor_Partition);
  locF2.redim(F2);
  locF2 = F2;

  if (proc_num == 0)
  {
    datptr = locF2.getDataPointer();
    for (j=0;j<66;j++)
    {
      for (gc=0;gc<ghost_cell_width;gc++)
      {
        datptr[j*size1+gc] = 0.;
        datptr[(j+1)*size1-1-gc] = 0.;
      }
    }
    sum1=0.;
    for (i=0;i<size3;i++) sum1+=datptr[i];

    m1 = 42.*11*6; // (2+4+...+12)*11*6
    if (sum1 != m1)
    {
      cout<<"ERROR in Lhs-Rhs,test2"<<endl<<"  sum1="<<sum1<<endl;
      cout<<"  correct answer is: sum1 ="<<m1<<endl;
    }
    else
      cout<<"A = Lhs - Rhs, test2 ok"<<endl;
  }

  Index Is(Ispan);
  Index Js(Jspan);
  Index Ks(Kspan);

  TYPEArray F3(Ispan,Jspan,Kspan);
  F3 = A(Is,Js,Ks,103) -(B+D) ;

  TYPEArray locF3;
  locF3.partition(*Single_Processor_Partition);
  locF3.redim(F3);
  locF3 = F3;

  if (proc_num == 0)
  {
    datptr = locF3.getDataPointer();
    for (j=0;j<66;j++)
    {
      for (gc=0;gc<ghost_cell_width;gc++)
      {
        datptr[j*size1+gc] = 0.;
        datptr[(j+1)*size1-1-gc] = 0.;
      }
    }
    sum1=0.;
    for (i=0;i<size3;i++) sum1+=datptr[i];


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
  }


  */

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

      /*
  TYPEArray G1;
  G1.redim(A(all,all,all,103));
  G1=0.;
  where (A(all,all,all,102)>(TYPE)7.) G1 = A(all,all,all,104)+ B;

  TYPEArray locG1;
  locG1.partition(*Single_Processor_Partition);
  locG1.redim(G1);
  locG1 = G1;

  if (proc_num == 0)
  {
    datptr = locG1.getDataPointer();
    for (j=0;j<66;j++)
    {
      for (gc=0;gc<ghost_cell_width;gc++)
      {
        datptr[j*size1+gc] = 0.;
        datptr[(j+1)*size1-1-gc] = 0.;
      }
    }
    sum1=0.;
    for (i=0;i<size3;i++) sum1+=datptr[i];

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
  }

  */

      // ... test A = min (X,Y) ...

 floatArray G2; 
  G2=min(B(all,all,Internal_Index(-6)),D(all,all,Internal_Index(-5))+C); 

 floatArray locG2; 
  locG2.partition(*Single_Processor_Partition); 
  locG2.redim(G2); 
  locG2=G2; 

  if (proc_num == 0)
  { 
    datptr = (locG2.getDataPointer)(); 
    for (j = 0; j < 11; j++)
    { 
      for (gc = 0; gc < ghost_cell_width; gc++)
      { 
        datptr[(j * size1 + gc)] = 0; 
        datptr[(((j + 1) * size1 - 1) - gc)] = 0; 
      } 
    } 
    sum1 = 0; 
    for (i = 0; i < size2; i++){ sum1 += datptr[i]; } 

    // ... D = 12-B so D+C = 7-B which ranges from 5 to -5 (7-2 to 7-12).
    //    B is minimal only for i=2 and i=3. D+3 is minimal for i=4 to
    //    i=12 where D+C has elements 3 to -5. Elements in B and D
    //    only vary with i.  The j direction is dimension 11. ...

    m1 = -4; 
    m1 *= 11; 
    if (sum1 != m1)
    { 
      cout<<("ERROR in min(Lhs,Rhs),test1")<<(endl)<<("  sum1=")<<(sum1)<<(endl); 

      cout<<("  correct answer is: sum1=")<<(m1)<<(endl); 
    } 
    else 
      { cout<<("A = min(Lhs,Rhs), test1 ok")<<(endl); } 
  } 

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

 floatArray P1; 
  P1=B(all,all,Internal_Index(-1)); 
  P1=min(P1,5); 

 floatArray locP1; 
  locP1.partition(*Single_Processor_Partition); 
  locP1.redim(P1); 
  locP1=P1; 

  if (proc_num == 0)
  { 
    datptr = (locP1.getDataPointer)(); 
    for (j = 0; j < 11; j++)
    { 
      for (gc = 0; gc < ghost_cell_width; gc++)
      { 
        datptr[(j * size1 + gc)] = 0; 
        datptr[(((j + 1) * size1 - 1) - gc)] = 0; 
      } 
    } 
    sum1 = 0; 
    for (i = 0; i < size2; i++){ sum1 += datptr[i]; } 

    // ... B(all,all,-1) is a 2d array varying only with i
    //   with values 2 to 12.  The min of 5 and B(all,all,-1)
    //   has values 2,3,4,5,..,5 with 8 5's. The sum in the
    //   i direction is 49 and the j direction has dimension 11 ...

    m1 = 539; 
    if (sum1 != m1)
    { 
      cout<<("ERROR in min(X,y),test1")<<(endl)<<("  sum1=")<<(sum1)<<(endl); 

      cout<<("  correct answer is: sum1=")<<(m1)<<(endl); 
    } 
    else 
      { cout<<("A = min(X,y), test1 ok")<<(endl); } 
  } 


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

  TYPEArray locP3;
  locP3.partition(*Single_Processor_Partition);
  locP3.redim(P3);
  locP3 = P3;

  if (proc_num == 0)
  {
    datptr = locP3.getDataPointer();
    sum1=0.;
    for (i=0;i<3+2*ghost_cell_width;i++) sum1+=datptr[i]; 

    m1 = 12;
    if (sum1 != m1)
    {
      cout<<"ERROR in min(X,y),test2"<<endl<<
      "  sum1="<<sum1<<endl;
      cout<<"  correct answer is: sum1="<<m1<<endl;
    }
    else
      cout<<"A = min(X,y), test2 ok"<<endl;
  }


  TYPEArray P4;
  P4.copy(B);

  TYPEArray P5(11,11,6);
  P5 = D;

  TYPEArray P6;
  P6 = -12. + (P5+P4)(all,all,3);

  TYPEArray locP6;
  locP6.partition(*Single_Processor_Partition);
  locP6.redim(P6);
  locP6 = P6;

  if (proc_num == 0)
  {
    datptr = locP6.getDataPointer();
    sum1=0.;
    for (i=0;i<size2;i++) sum1+=datptr[i];

    m1 = 0;
    if (sum1 != m1)
    {
      cout<<"ERROR in A=x+Rhs,test1"<<endl<<
      "  sum1="<<sum1<<endl;
      cout<<"  correct answer is: sum1="<<m1<<endl;
    }
    else
      cout<<"A=x+Rhs, test1 ok"<<endl;
  }

  */

      //============================================================
      // ... Test abstract function:
      //  TYPEArray & TYPEArray::Abstract_Modification_Operator 
      //   (const intArray & Lhs , const TYPEArray & Rhs ,
      //    Prototype_6 Operation_Array_Data,
      //    int Operation_Type )
      //
      //   A = replace(ILhs,Rhs) (A can't be an intArray)
      //============================================================

 floatArray M1(Ispan,Jspan); 
  M1=5; 

  intArray ILhs(Ispan,Jspan); 
  ILhs=1; 
  where (B(all,all,Internal_Index(-3))>2){ ILhs=0; }   // ILhs is 0 except for j-k
  // plane at i=2
 floatArray M2; 
  M2=M1.replace(ILhs,B(all,all,Internal_Index(-2))); 
  // j=k plane at i=2 of M1 is replaced with
  // same plane from B which is filled
  // with 2

 floatArray locM2; 
  locM2.partition(*Single_Processor_Partition); 
  locM2.redim(M2); 
  locM2=M2; 

  if (proc_num == 0)
  { 
    datptr = (locM2.getDataPointer)(); 
    for (j = 0; j < 11; j++)
    { 
      for (gc = 0; gc < ghost_cell_width; gc++)
      { 
        datptr[(j * size1 + gc)] = 0; 
        datptr[(((j + 1) * size1 - 1) - gc)] = 0; 
      } 
    } 
    sum1 = 0; 
    for (i = 0; i < size2; i++){ sum1 += datptr[i]; } 

    // ... 2*11 + 5*10*11 ...
    m1 = 572; 
    if (sum1 != m1)
    { 
      cout<<("ERROR in A.replace(ILhs,Rhs),test2")<<(endl)<<("  sum1=")<<(sum1)<<(endl); 

      cout<<("  correct answer is: sum1=")<<(m1)<<(endl); 
    } 
    else 
      { cout<<("A.replace(ILhs,Rhs), test2 ok")<<(endl); } 
  } 

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

  TYPEArray locM3;
  locM3.partition(*Single_Processor_Partition);
  locM3.redim(M3);
  locM3 = M3;

  if (proc_num == 0)
  {
    datptr = locM3.getDataPointer();
    for (j=0;j<11;j++)
    {
      for (gc=0;gc<ghost_cell_width;gc++)
      {
        datptr[j*size1+gc] = 0.;
        datptr[(j+1)*size1-1-gc] = 0.;
      }
    }
    sum1=0.;
    for (i=0;i<size2;i++) sum1+=datptr[i];

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
  }
 
  intArray Ibig = (M3>3).indexMap();
  intArray Isml = (M3<=3).indexMap();
 
  if (proc_num == 0)
  {
    sum2=0.;
    for(i=0;i<Ibig.getLength(0)+2*ghost_cell_width;i++)
    {
      sum2+=datptr[Ibig(i)];
    }
    for(i=0;i<Isml.getLength(0)+2*ghost_cell_width;i++)
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
  }
  */

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

 floatArray N1(Ispan,11); 
  N1=7; 
  N1.copy(C); 
#if defined (USE_PPP)
 floatArray N2(N1.getLength(0),N1.getLength(1),N1.getLength(2),N1.getLength(3)); 

  N2=N1;   // N2 is -5
#else

  // N2 is -5
#endif
 floatArray N3; 
  N3.reference(N2);   // N3 is -5
  N3=N2+N1;   // N3 and N2 both are filled with -10

  N1.replace(1,N2);   // N1 is filled with -10
  N3.breakReference(); 
  N3=0;   // N2 stays at -10 
  N1=N2-N1;   // N1 is 10 --10

 floatArray locN1; 
  locN1.partition(*Single_Processor_Partition); 
  locN1.redim(N1); 
  locN1=N1; 

  // ... set ghost cells to 0 first ...
  if (proc_num == 0)
  { 
    datptr = (locN1.getDataPointer)(); 
    for (j = 0; j < 11; j++)
    { 
      for (gc = 0; gc < ghost_cell_width; gc++)
      { 
        datptr[(j * size1 + gc)] = 0; 
        datptr[(((j + 1) * size1 - 1) - gc)] = 0; 
      } 
    } 
    sum1 = 0; 
    for (i = 0; i < size2; i++){ sum1 += datptr[i]; } 

    m1 = 0; 
    if (sum1 != m1)
    { 
      cout<<("ERROR in A.replace(ix,Lhs),test1")<<(endl)<<("  sum1=")<<(sum1)<<(endl); 

      cout<<("  correct answer is: sum1=")<<(m1)<<(endl); 
    } 
    else 
      { cout<<("A.replace(ix,Lhs), test1 ok")<<(endl); } 
  } 

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

 floatArray Q1; 
  Q1=B(all,all,Internal_Index(-6));   // B(all,all,-6) ranges from 2 to 12 only
  // changing in the i direction
  Q1+=(C);   // Q1 ranges from -3 to 7

 floatArray locQ1; 
  locQ1.partition(*Single_Processor_Partition); 
  locQ1.redim(Q1); 
  locQ1=Q1; 

  if (proc_num == 0)
  { 
    datptr = (locQ1.getDataPointer)(); 
    for (j = 0; j < 11; j++)
    { 
      for (gc = 0; gc < ghost_cell_width; gc++)
      { 
        datptr[(j * size1 + gc)] = 0; 
        datptr[(((j + 1) * size1 - 1) - gc)] = 0; 
      } 
    } 
    sum1 = 0; 
    for (i = 0; i < size2; i++){ sum1 += datptr[i]; } 

    // ... (2+3+...+12) = 77 so B(all,all,-6) elements sum to
    //    77*11 before adding -5 to each of the 121 elements ...

    m1 = 242; 
    if (sum1 != m1)
    { 
      cout<<("ERROR in A+=Rhs,test1")<<(endl)<<("  sum1=")<<(sum1)<<(endl); 

      cout<<("  correct answer is: sum1=")<<(m1)<<(endl); 
    } 
    else 
      { cout<<("A+=Rhs, test2 ok")<<(endl); } 
  } 

      /* ... TEMPORARILY TURN OFF FOR 8 PROCESSORS ... */
 floatArray Q2; 
  Q2=B+D;   // Q2 is set to 12 everywhere
  where (B(all,all,Internal_Index(-5))<2){ Q2(all,all,Internal_Index(3))/=(C); } 
  // no elements of B are < 2 so Q2 isn't
  // changed

 floatArray locQ2; 
  locQ2.partition(*Single_Processor_Partition); 
  locQ2.redim(Q2); 
  locQ2=Q2; 

  if (proc_num == 0)
  { 
    datptr = (locQ2.getDataPointer)(); 
    for (j = 0; j < 11; j++)
    { 
      for (gc = 0; gc < ghost_cell_width; gc++)
      { 
        datptr[(j * size1 + gc)] = 0; 
        datptr[(((j + 1) * size1 - 1) - gc)] = 0; 
      } 
    } 
    sum1 = 0; 
    for (i = 0; i < size2; i++){ sum1 += datptr[i]; } 

    m1 = 1452; 
    if (sum1 != m1)
    { 
      cout<<("ERROR in A/=Rhs,test1")<<(endl)<<("  sum1=")<<(sum1)<<(endl); 

      cout<<("  correct answer is: sum1=")<<(m1)<<(endl); 
    } 
    else 
      { cout<<("A/=Rhs , test1 ok")<<(endl); } 
  } 
      /* ... END TEMPORARILY TURNED OFF SECTION FOR 8 PROCESSORS ... */

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

 float * bptr=new float [B.elementCount()]; 

#if defined(USE_PPP)
 floatArray BB(Ispan,Jspan,Kspan); 
#else

#endif

  BB.fill(7); 

 floatArray DD; 
  DD=C; 
  // ... set bases to 0 because operator= has changed and no the
  //  base of DD is set to the base of C instead of 0 making the
  //  resize intersection different ...
  DD.setBase(0,0); 
  DD.setBase(0,1); 
  DD.resize(D.getLength(0),D.getLength(1),D.getLength(2)); 
  Index most(1,5); 
  DD(all,all,most)=0; 
  DD++;   // DD(all,all,0) is -4, DD(all,all,most) is 1

 floatArray EE(Ispan,Jspan); 
  EE=0; 
  EE=BB(all,all,Internal_Index(-3))+DD(all,all,Internal_Index(0));   // EE is filled with 3 
  EE/=(3); 

 floatArray locEE; 
  locEE.partition(*Single_Processor_Partition); 
  locEE.redim(EE); 
  locEE=EE; 

  if (proc_num == 0)
  { 
    datptr = (locEE.getDataPointer)(); 
    for (j = 0; j < 11; j++)
    { 
      for (gc = 0; gc < ghost_cell_width; gc++)
      { 
        datptr[(j * size1 + gc)] = 0; 
        datptr[(((j + 1) * size1 - 1) - gc)] = 0; 
      } 
    } 
    sum1 = 0; 
    for (i = 0; i < size2; i++){ sum1 += datptr[i]; } 

    // there are 121 elements filled with 1 in EE
    m1 = 121; 
    if (sum1 != m1)
    { 
      cout<<("ERROR in A/=x,test1")<<(endl)<<("  sum1=")<<(sum1)<<(endl); 

      cout<<("  correct answer is: sum1=")<<(m1)<<(endl); 
    } 
    else 
      { cout<<("A/=x , test1 ok")<<(endl); } 
  } 

  delete bptr; 
  //delete fptr;


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
 floatArray H1; floatArray H2; 

  int im1; 

  /* ... TEMPORARILY TURN OFF FOR EIGHT ... */
  H1.reference((B-D)(all,all,Internal_Index(0)));   // H1 has elements -8,-6,...,10,12
  //H1.setBase(3,1);  put this in when bug is fixed
  int h1base1=H1.getBase(1); 
  H2=H1(all,Internal_Index(h1base1 + 10));   // H1 only varies with i, H2 is 1d
  // with even values -8 to 12 
  //H2 = H1(all,13); non bug version
 floatArray H3(Ispan); 
  H3=2; 
  IA1=H2<H3;   // H2 is less than H3 for H2 values -8,-6,-4,-2
  // and 0

  intArray locIA1; 
  locIA1.partition(*Single_Processor_Partition); 
  locIA1.redim(IA1); 
  locIA1=IA1; 

  if (proc_num == 0)
  { 
    idatptr = (locIA1.getDataPointer)(); 
    for (gc = 0; gc < ghost_cell_width; gc++)
    { 
      idatptr[gc] = 0; 
      idatptr[((size1 - 1) - gc)] = 0; 
    } 
    isum1 = 0; 
    for (i = 0; i < size1; i++){ isum1 += idatptr[i]; } 

    // ... there are 5 elements in IA1 set to 1 corresponding to
    //    -8,...,0 ...

    im1 = 5; 
    if (isum1 != im1)
    { 
      cout<<("ERROR in IA = Lhs < Rhs,test1")<<(endl)<<("  isum1=")<<(isum1)<<(endl); 

      cout<<("  correct answer is isum1=")<<(im1)<<(endl); 
    } 
    else 
      { cout<<("IA = Lhs < Rhs , test1 ok")<<(endl); } 
  } 

      /* ... END TEMPORARILY TURNED OFF SECTION ... */

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
 floatArray O1(Ispan); 
  O1=0; 
  O1(2) = -2; 
  O1(3) = -1; 
  O1(4) = 0; 
  O1(5) = 1; 
  O1(6) = 2; 

 float w=0; 
  IA=w||O1;   // IA is set to 1 only where O1 is nonzero
  // at i=2,3,5 and 6

  intArray locIA; 
  locIA.partition(*Single_Processor_Partition); 
  locIA.redim(IA); 
  locIA=IA; 

  if (proc_num == 0)
  { 
    idatptr = (locIA.getDataPointer)(); 
    for (gc = 0; gc < ghost_cell_width; gc++)
    { 
      idatptr[gc] = 0; 
      idatptr[((size1 - 1) - gc)] = 0; 
    } 
    isum1 = 0; 
    for (i = 0; i < size1; i++){ isum1 += idatptr[i]; } 

    im1 = 4; 
    if (isum1 != im1)
    { 
      cout<<("ERROR in IA=x||A,test1")<<(endl)<<("  isum1=")<<(isum1)<<(endl); 

      cout<<("  correct answer is: isum1=4")<<(endl); 
    } 
    else 
      { cout<<("IA= X||A , test1 ok")<<(endl); } 
  } 


      //============================================================
      // ... Test abstract operator:
      //  TYPEArray & TYPEArray::operator= ( const TYPEArray & Rhs )
      //
      // Test only function that calls this, A=Rhs
      //============================================================

      //TYPEArray R1;
      //where (B==(TYPE)2.) R1 = B+D;


      //============================================================
      // ... Test abstract operator:
      //  TYPEArray & TYPEArray::operator= ( TYPE x )
      //
      // test with only function from a) A=x
      //============================================================

 floatArray U1(Ispan,Jspan); 
 float x; float y; 
  x = 3; 
  y = 7; 
  where (A(all,all,Internal_Index(-6),Internal_Index(100))<2){ U1=x + y; }   // A(all,all,-6,100) has
  // all 121 elements set to 1
  // so U1 has 121 elements
  // set  to 10.

 floatArray locU1; 
  locU1.partition(*Single_Processor_Partition); 
  locU1.redim(U1); 
  locU1=U1; 

  if (proc_num == 0)
  { 
    datptr = (locU1.getDataPointer)(); 
    for (j = 0; j < 11; j++)
    { 
      for (gc = 0; gc < ghost_cell_width; gc++)
      { 
        datptr[(j * size1 + gc)] = 0; 
        datptr[(((j + 1) * size1 - 1) - gc)] = 0; 
      } 
    } 
    sum1 = 0; 
    for (i = 0; i < size2; i++){ sum1 += datptr[i]; } 

    m1 = 1210; 
    if (sum1 != m1)
    { 
      cout<<("ERROR in A=x,test1")<<(endl)<<("  sum1=")<<(sum1)<<(endl); 

      cout<<("  correct answer is: sum1=1210")<<(endl); 
    } 
    else 
      { cout<<("A=x , test1 ok")<<(endl); } 
  } 

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

 floatArray X1(Ispan,Jspan,Kspan); 
  X1=1; 
 floatArray X2; 
  X2=min(X1,B-D,B+D);   // B-D ranges from -8 to 12, B+D is 12
  // so X2 has elements -8,-6,-4,-2,0,
  // and then 6 1's for each j-k plane.

 floatArray locX2; 
  locX2.partition(*Single_Processor_Partition); 
  locX2.redim(X2); 
  locX2=X2; 

  if (proc_num == 0)
  { 
    datptr = (locX2.getDataPointer)(); 
    for (j = 0; j < 66; j++)
    { 
      for (gc = 0; gc < ghost_cell_width; gc++)
      { 
        datptr[(j * size1 + gc)] = 0; 
        datptr[(((j + 1) * size1 - 1) - gc)] = 0; 
      } 
    } 
    sum1 = 0; 
    for (i = 0; i < size3; i++){ sum1 += datptr[i]; } 

    // ... -8+-6+-4+-2+0+1+1+1+1+1+1=-14, each j-k plane has 11*6
    //     elements ...

    m1 = -924; 
    if (sum1 != m1)
    { 
      cout<<("ERROR in A=min(X,X1,Z),test1")<<(endl)<<("  sum1=")<<(sum1)<<(endl); 

      cout<<("  correct answer is: sum1=")<<(m1)<<(endl); 
    } 
    else 
      { cout<<("A=min(X,X1,Z) , test1 ok")<<(endl); } 
  } 



      // ... test A = min (X,y,Z) ...

  y = 10; 
 floatArray Z1; 
  where (B>9){ Z1=min(B,y,D); }   // B>9 at i=10,11,12 where elements
  // have value i. D has elements 
  // 2,1 and 0 at these positions.
  // Z1 will have elements 2,1 and 0
  // where i=10,11 and 12.
  where (B<=9){ Z1=0; }   // the rest of Z1 is set to 0.

 floatArray locZ1; 
  locZ1.partition(*Single_Processor_Partition); 
  locZ1.redim(Z1); 
  locZ1=Z1; 

  if (proc_num == 0)
  { 
    datptr = (locZ1.getDataPointer)(); 
    for (j = 0; j < 66; j++)
    { 
      for (gc = 0; gc < ghost_cell_width; gc++)
      { 
        datptr[(j * size1 + gc)] = 0; 
        datptr[(((j + 1) * size1 - 1) - gc)] = 0; 
      } 
    } 
    sum1 = 0; 
    for (i = 0; i < size3; i++){ sum1 += datptr[i]; } 

    // ... each j=k plane with 11*6 elements sums to (0+1+2)=3 ...
    m1 = 198; 
    if (sum1 != m1)
    { 
      cout<<("ERROR in A=min(X,y,Z),test1")<<(endl)<<("  sum1=")<<(sum1)<<(endl); 

      cout<<("  correct answer is: sum1=")<<(m1)<<(endl); 
    } 
    else 
      { cout<<("A=min(X,y,Z) , test1 ok")<<(endl); } 
  } 


      // ... test A = sum (X,Axis) ... 

#ifndef USE_PPP 
      // ... sum along an axis doesn't work with P++ ...

      // Sum(B,0) is a 1 x 11 x 6 array with
      // each element set to 2+3+...+12=77,
      // sum(sum(B,0),1) is a 1 x 1 x 11 array
      // with each element set to 11*77 

      // displaying sum(sum(B,0),1) should make this but the data in
      // the wrong place (looks like sum(B,0) too)
      //sum(B,0).display("sum(B,0)");
      //sum(sum(B,0),1).display("sum(sum(B,0),1)");
      //W1.display("W1");



#if defined(USE_PPP)
      // ... redim to make testing easier ...

#endif


      // displaying sum(sum(B,0),1) should make this but the data in
      // the wrong place
      //sum(B,0).display("sum(B,0)");
      //sum(sum(B,0),1).display("sum(sum(B,0),1)");
      //W1.display("W1");

























      // Sum(B(all,JJ,-3)*ONE(all,JJ,-3),1) is an 
      // 11 x 1 x 1 with each element set
      // to i*3 for B(i,j,k)




























#endif

      // ... test A = transpose (X) (only works for 2D arrays) ...

      /*
  TYPEArray V1,V2;
  V1 = transpose (B(all,all,-6)); // V1 is an 11 x 11 array with
				  // elements varying with j ranging
				  // from 2 to 12
  V2 = V1(all,0); // V2 is an 11 x1 array that has 2 for its
                  // elements

  TYPEArray locV2;
  locV2.partition(*Single_Processor_Partition);
  locV2.redim(V2);
  locV2 = V2;

  if (proc_num == 0)
  {
    datptr = locV2.getDataPointer();
    for (gc=0;gc<ghost_cell_width;gc++)
    {
      datptr[gc] = 0.;
      datptr[size1-1-gc] = 0.;
    }
    sum1=0.;
    for (i=0;i<size1;i++) sum1+=datptr[i];
  
    m1 = 22.;
    if (sum1 != m1)
    {
      cout<<"ERROR in A=transpose(X),test1"<<endl<<
      "  sum1="<<sum1<<endl;
      cout<<"  correct answer is: sum1=22"<<endl;
    }
    else
      cout<<"A=transpose(X) , test1 ok"<<endl;
  }

  */
      //===========================================================
      // ... test setting a single value with ranges (Bill H.'s bug) ...

  intArray Num(2,3); 
  Num=0; 
  Num(Range(0,0),Range(1,1))=5; 

  intArray locNum; 
  locNum.partition(*Single_Processor_Partition); 
  locNum.redim(Num); 
  locNum=Num; 

  if (proc_num == 0)
  { 
    idatptr = (locNum.getDataPointer)(); 
    if (idatptr[(2 + 3 * ghost_cell_width)] != 5)
    { 
      Num.display("Num"); 
      cout<<("ERROR in Num(RANGE(0,0),Range(1,1))=5.")<<(endl); 
    } 
    else 
    { 
      cout<<("Num(RANGE(0,0),Range(1,1))=5. test okay")<<(endl); 
    } 
  } 

      //===========================================================
      // ... test filling a view of the whole array ...

 floatArray V3(Ispan); 
 floatArray V4(Ispan); 
 floatArray V5(Ispan); 
  V4=4; 
  V5=5; 

  V3(Ispan)=V4*V5; 

 floatArray locV3; 
  locV3.partition(*Single_Processor_Partition); 
  locV3.redim(V3); 
  locV3=V3; 

  if (proc_num == 0)
  { 
    datptr = (locV3.getDataPointer)(); 
    for (gc = 0; gc < ghost_cell_width; gc++)
    { 
      datptr[gc] = 0; 
      datptr[((size1 - 1) - gc)] = 0; 
    } 
    sum1 = 0; 
    for (i = 0; i < size1; i++){ sum1 += datptr[i]; } 

    m1 = 220; 
    if (sum1 != m1)
    { 
      cout<<("ERROR in view of whole array test, sum1=")<<(sum1)<<(endl); 
      cout<<("  correct answer is: sum1=220.")<<(endl); 
    } 
    else 
      { cout<<("View of whole array test okay ")<<(endl); } 
  } 


      //===========================================================
      // ... make some scalar indexing tests ...

 floatArray AA; 
  AA.reference(A(Ispan2,Jspan2,Internal_Index(-4),Internal_Index(102))); 
  sum1 = 0; 
  int jbase=Jspan2.getBase(); 
  int ibase=Ispan2.getBase(); 
  for (j = jbase; j < (Jspan2.getCount)() + jbase; j++)
     { for (i = ibase; i < (Ispan2.getCount)() + ibase; i++)
       { sum1 += AA(i,j,-4,102); } 
  } 
  if (sum1 != 36.0)
  { 
    cout<<("ERROR in scalar indexing test1")<<(endl)<<(" sum1 = ")<<(sum1)<<(", correct answer = 36.")
    <<(endl); 
  } 
  else 
  { 
    cout<<("scalar indexing test1 okay")<<(endl); 
  } 

 floatArray CC(0); 
  CC.resize(11,11); 
  CC(5,5) = 7; 
  CC.redim(Ispan,Jspan); 
  CC=C; 
  CC.redim(Jspan,Ispan); 

  for (i = ibase; i < (Ispan2.getCount)() + ibase; i++)
  { 
    CC(Internal_Index(2),Ispan2)(2,i) = -4; 
  } 

  CC.resize(Ispan,Jspan); 

  if (CC(2,6) != -4.0 || CC(2,8) != -4.0)
  { 
    cout<<("ERROR in scalar indexing test2,")<<(endl); 
    cout<<("  CC(2,6) = ")<<(C(2,6))<<(", CC(2,8) = ")<<(C(2,8))<<(endl); 
    cout<<("  both should be -4.")<<(endl); 
  } 
  else 
  { 
    cout<<("scalar indexing test2 okay")<<(endl); 
  } 


    // ... temp test ...
 floatArray WW(Range(Ispan2)); 
  WW=-17; 



  //TYPEArray ZZ(Ispan);
  //ZZ = -15.;

  Index I1(4,5,2); 
  Index I2(6,2,2); 

  ZZ(I1)(I2)(6) = -1; 
  ZZ(I1)(I2)(7) = -1; 

  if (ZZ(8) != -1.0 || ZZ(12) != -1.0)
  { 
    cout<<("ERROR in scalar indexing test3")<<(endl); 
    cout<<("  ZZ(8) = ")<<(ZZ(8))<<(", ZZ(12) = ")<<(ZZ(12))<<(endl); 
    cout<<("  both should be -1.")<<(endl); 
  } 
  else 
  { 
    cout<<("scalar indexing test3 okay")<<(endl); 
  } 

  Index I3(5,5); 
 floatArray AAA(Range(I3)); 
  AAA=9; 
  AAA=sqrt(AAA); 

  if (AAA(5) != 3.0)
  { 
    cout<<("ERROR in scalar indexing test4")<<(endl); 
    cout<<("  AAA(5) = ")<<(AAA(5))<<(endl); 
    cout<<("  it should be 3.")<<(endl); 
  } 
  else 
  { 
    cout<<("scalar indexing test4 okay")<<(endl); 
  } 


    /* ... turn this section off for now because it just does a display 
      that makes the rest of the code output harder to read.  This
      should be replaced with a test okay kind of output. ...

  TYPESerialArray ZSerial = ZZ.getLocalArray();
  ZSerial = 22.;
  ZZ.display("ZZ");

  TYPESerialArray ZZSerial = ZZ.getLocalArrayWithGhostBoundaries();
  ZZSerial = 15.;
  ZZSerial.display("ZZSerial");
  ZZ.display("ZZ");

  TYPESerialArray ZZSerialView = ZZ(Range(4,9)).getLocalArrayWithGhostBoundaries();
  ZZSerialView = -7.;
  ZZSerialView.display("ZZSerialView");
  ZZ.display("ZZ");

  */

    //============================================================
    // ... test overlapping boundary model when ghost cells need to be 
    //  updated ... 

  Range II(2,11); 
  ZZ=-15; 
  ZZ(II)=ZZ(II)+ZZ((II+1)); 

 floatArray locZZ; 
  locZZ.partition(*Single_Processor_Partition); 
  locZZ.redim(ZZ); 
  locZZ=ZZ; 

  if (proc_num == 0)
  { 
    datptr = (locZZ.getDataPointer)(); 

    for (gc = 0; gc < ghost_cell_width; gc++)
    { 
      datptr[gc] = 0; 
      datptr[((size1 - 1) - gc)] = 0; 
    } 
    sum1 = 0; 
    for (i = 0; i < size1; i++){ sum1 += datptr[i]; } 

    // ... ZZ should be -30,-30,...,-30,-15 with 10 -30's ...
    if (sum1 != -315.0)
    { 
       cout<<("ERROR in overlapping boundary test")<<(endl); 
       cout<<("   sum1 = ")<<(sum1)<<(", correct answer is -300.")<<(endl); 
    } 
    else 
    { 
      cout<<("overlapping boundary model test ok")<<(endl); 
    } 
  } 

      //============================================================
      // ... test intSerialArray as a mask for where ...

 floatSerialArray ASerial(5,5); 
 floatSerialArray BSerial(5,5); 
  for (i = 0; i < 5; i++){ ASerial(Internal_Index(i),all)=(float )i; } 

  where (ASerial>2){ BSerial=7; } 
  where (ASerial<=2){ BSerial=1; } 

  datptr = (BSerial.getDataPointer)(); 
  size1 = 25; 
  sum1 = 0; 
  for (i = 0; i < size1; i++){ sum1 += datptr[i]; } 

  // ... 10 7's and 15 1's ...
  if (sum1 != 85)
  { 
     cout<<("ERROR in intSerialArray test1")<<(endl); 
     cout<<("  sum1 = ")<<(sum1)<<(", correct answer is 85")<<(endl); 
  } 
  else 
  { 
     cout<<("intSerialArray test1 ok")<<(endl); 
  } 

     //============================================================
     // ... test modification of partitioning object ...

  if (Number_Of_Processors > 2)
  { 
     int Size=10; 
     Partitioning_Type Partition(Range(0,0)); 
    floatArray AAA(Size,Partition); 
    floatArray BBB(Size * 2,Partition); 
    floatArray CCC(Size / 2,Partition); 
    floatArray DDD(Size * 2,Partition); 
    floatArray EEE(Size / 2,Partition); 
    floatArray FFF(Size,Partition); 

     int i; 
     for (i = 0; i < Number_Of_Processors; i++)
     { 
 Partition.SpecifyProcessorRange(Range(0,i)); 
     } 
     for (i = 0; i < Number_Of_Processors; i++)
     { 
 Partition.SpecifyProcessorRange(Range(i,Number_Of_Processors - 1)); 
     } 

    floatArray GGG(Size,Partition); 
    floatArray HHH(Size * 2,Partition); 
    floatArray III(Size / 2,Partition); 
    floatArray JJJ(Size,Partition); 

    // ... to get here tests must have passed or code would have
    //  crashed ...
     cout<<("Partitioning Object modification test okay")<<(endl); 
  } 

     //============================================================
     // ... test reshape, reference and resize bugs reprorted by Bill ...

     // ... this tests reshaping a 2 x 2 x 2 x 2 array into a 4 x 1 x 2 x 2
     //   array ...
 floatArray xx(Range(0,1),Range(0,1),Range(0,1),Range(2,3)); 
  Partitioning_Type * partition=new Partitioning_Type (); 
  int ghost_boundary_width=0; 
  partition->partitionAlongAxis(0,0,ghost_boundary_width); 
  partition->partitionAlongAxis(1,0,ghost_boundary_width); 
  partition->partitionAlongAxis(2,0,ghost_boundary_width); 
  partition->partitionAlongAxis(3,1,1); 
  xx.partition(*partition); 
  xx=3; 
  xx.reshape(Range(0,3),Range(0,0),Range(0,1),Range(0,1)); 

  cout<<("Reshape test 1 okay")<<(endl); 


  // ... this test whether an array gets added to the list associated
  //  with its partition correctly by reference.  If there is an error
  //  it might not show up until cleanup at end ...

  Partitioning_Type * P=new Partitioning_Type (Number_Of_Processors); 

  int numberOfGhostPoints=0;   // ******************************

  int kd; 
  for (kd = 0; kd < 2; kd++)
  { 
     P->partitionAlongAxis(kd,1,numberOfGhostPoints); 
  } 
  for (kd = 2; kd < 4; kd++)
  { 
     P->partitionAlongAxis(kd,0,0); 
  } 

 floatArray aa(3,3,2,2,*P); floatArray bb; 
 //TYPEArray aa(3,3,2,2),bb;
  bb.reference(aa); 
  aa=1; 
  bb.reference(aa); 

  cout<<("Reference with partition test1 okay")<<(endl); 


  // ... test resize with a partition before Bill finds this bug ...
  aa.resize(3,3,4); 

  // ... if code doesn't crash it is probably working ...
  cout<<("resize with partition test1 okay")<<(endl); 

  delete partition; 
  delete P; 

  //============================================================
  // ... this code is slow so the user might want to turn it off ...
#if defined(TEST_ARRAY_OVERFLOW)
  // ... use modified Bill H.'s memory leak code ...






















#endif 


  //============================================================


  delete Single_Processor_Partition; 

  //================= P++ finish ==============================

  printf("Program Terminated Normally! \n"); 

  // ... leave these 2 lines in for A++ also for debugging ...

  Optimization_Manager::Exit_Virtual_Machine(); 
  printf("Virtual Machine exited! \n"); 

  //===========================================================

  return 0;
} 
