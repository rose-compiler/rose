/*  -*-Mode: c++; -*-  */
// MERGE0 : use for A++ operations when the first index is a scalar
//      a(i0,I1,I2,I3)=
// you must define the following in your code
//  int dum;
//  Range aR0,aR1,aR2,aR3;
//		from Bill Henshaw's /n/c3servet/henshaw/res/oges/discreteVertex.C

#undef MERGE0
#define MERGE0(a,i0,I1,I2,I3) \
  for(  \
      aR0=Range(a.getBase(0),a.getBound(0)),   \
      aR1=Range(a.getBase(1),a.getBound(1)),   \
      aR2=Range(a.getBase(2),a.getBound(2)),   \
      aR3=Range(a.getBase(3),a.getBound(3)),   \
      a.reshape(Range(0,aR0.length()*aR1.length()-1),aR2,aR3), \
      dum=0; dum<1; dum++,  \
      a.reshape(aR0,aR1,aR2,aR3) ) \
    a(Index(i0-aR0.getBase()+aR0.length()*(I1.getBase()-aR1.getBase()),   \
      I1.length(),aR0.length()),I2,I3)

#undef RESHAPE
#define RESHAPE(a) \
      aR0=Range(a.getBase(0),a.getBound(0));   \
      aR1=Range(a.getBase(1),a.getBound(1));   \
      aR2=Range(a.getBase(2),a.getBound(2));   \
      aR3=Range(a.getBase(3),a.getBound(3));   \
      a.reshape(Range(0,aR0.length()*aR1.length()-1),aR2,aR3) 

#undef MERGE1
#define MERGE1(a,i0,I1,I2,I3) \
    a(Index(i0-aR0.getBase()+aR0.length()*(I1.getBase()-aR1.getBase()),   \
      I1.length(),aR0.length()),I2,I3)

#undef UNRESHAPE
#define UNRESHAPE(a) a.reshape(aR0,aR1,aR2,aR3)


