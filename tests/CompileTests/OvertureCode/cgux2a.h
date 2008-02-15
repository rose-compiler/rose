//
// Define 2nd-order difference approximations
//
//    needs d12(kd) = 1/(2*h(kd))
//          d22(kd) = 1/(h(kd)**2)
//
// To use these defines you must first define
//      U(i1,i2,i3,kd)
//      UR2, US2, UT2



#define UR2A(I1,I2,I3,KD) ( (U(I1+1,I2,I3,KD)-U(I1-1,I2,I3,KD))*d12(axis1) )
#define US2A(I1,I2,I3,KD) ( (U(I1,I2+1,I3,KD)-U(I1,I2-1,I3,KD))*d12(axis2) )
#define UT2A(I1,I2,I3,KD) ( (U(I1,I2,I3+1,KD)-U(I1,I2,I3-1,KD))*d12(axis3) )

#undef  RX
#define RX(I1,I2,I3) inverseVertexDerivative(I1,I2,I3,0)
#define SX(I1,I2,I3) inverseVertexDerivative(I1,I2,I3,1)
#define TX(I1,I2,I3) inverseVertexDerivative(I1,I2,I3,2)
#define RY(I1,I2,I3) inverseVertexDerivative(I1,I2,I3,0+numberOfDimensions)
#define SY(I1,I2,I3) inverseVertexDerivative(I1,I2,I3,1+numberOfDimensions)
#define TY(I1,I2,I3) inverseVertexDerivative(I1,I2,I3,5)
#define RZ(I1,I2,I3) inverseVertexDerivative(I1,I2,I3,6)
#define SZ(I1,I2,I3) inverseVertexDerivative(I1,I2,I3,7)
#define TZ(I1,I2,I3) inverseVertexDerivative(I1,I2,I3,8)



#define RXR2(I1,I2,I3) ( (RX(I1+1,I2,I3)-RX(I1-1,I2,I3))*d12(axis1) )
#define RYR2(I1,I2,I3) ( (RY(I1+1,I2,I3)-RY(I1-1,I2,I3))*d12(axis1) )
#define RZR2(I1,I2,I3) ( (RZ(I1+1,I2,I3)-RZ(I1-1,I2,I3))*d12(axis1) )
#define RXS2(I1,I2,I3) ( (RX(I1,I2+1,I3)-RX(I1,I2-1,I3))*d12(axis2) )
#define RYS2(I1,I2,I3) ( (RY(I1,I2+1,I3)-RY(I1,I2-1,I3))*d12(axis2) )
#define RZS2(I1,I2,I3) ( (RZ(I1,I2+1,I3)-RZ(I1,I2-1,I3))*d12(axis2) )
#define RXT2(I1,I2,I3) ( (RX(I1,I2,I3+1)-RX(I1,I2,I3-1))*d12(axis3) )
#define RYT2(I1,I2,I3) ( (RY(I1,I2,I3+1)-RY(I1,I2,I3-1))*d12(axis3) )
#define RZT2(I1,I2,I3) ( (RZ(I1,I2,I3+1)-RZ(I1,I2,I3-1))*d12(axis3) )
#define SXR2(I1,I2,I3) ( (SX(I1+1,I2,I3)-SX(I1-1,I2,I3))*d12(axis1) )
#define SYR2(I1,I2,I3) ( (SY(I1+1,I2,I3)-SY(I1-1,I2,I3))*d12(axis1) )
#define SZR2(I1,I2,I3) ( (SZ(I1+1,I2,I3)-SZ(I1-1,I2,I3))*d12(axis1) )
#define SXS2(I1,I2,I3) ( (SX(I1,I2+1,I3)-SX(I1,I2-1,I3))*d12(axis2) )
#define SYS2(I1,I2,I3) ( (SY(I1,I2+1,I3)-SY(I1,I2-1,I3))*d12(axis2) )
#define SZS2(I1,I2,I3) ( (SZ(I1,I2+1,I3)-SZ(I1,I2-1,I3))*d12(axis2) )
#define SXT2(I1,I2,I3) ( (SX(I1,I2,I3+1)-SX(I1,I2,I3-1))*d12(axis3) )
#define SYT2(I1,I2,I3) ( (SY(I1,I2,I3+1)-SY(I1,I2,I3-1))*d12(axis3) )
#define SZT2(I1,I2,I3) ( (SZ(I1,I2,I3+1)-SZ(I1,I2,I3-1))*d12(axis3) )
#define TXR2(I1,I2,I3) ( (TX(I1+1,I2,I3)-TX(I1-1,I2,I3))*d12(axis1) )
#define TYR2(I1,I2,I3) ( (TY(I1+1,I2,I3)-TY(I1-1,I2,I3))*d12(axis1) )
#define TZR2(I1,I2,I3) ( (TZ(I1+1,I2,I3)-TZ(I1-1,I2,I3))*d12(axis1) )
#define TXS2(I1,I2,I3) ( (TX(I1,I2+1,I3)-TX(I1,I2-1,I3))*d12(axis2) )
#define TYS2(I1,I2,I3) ( (TY(I1,I2+1,I3)-TY(I1,I2-1,I3))*d12(axis2) )
#define TZS2(I1,I2,I3) ( (TZ(I1,I2+1,I3)-TZ(I1,I2-1,I3))*d12(axis2) )
#define TXT2(I1,I2,I3) ( (TX(I1,I2,I3+1)-TX(I1,I2,I3-1))*d12(axis3) )
#define TYT2(I1,I2,I3) ( (TY(I1,I2,I3+1)-TY(I1,I2,I3-1))*d12(axis3) )
#define TZT2(I1,I2,I3) ( (TZ(I1,I2,I3+1)-TZ(I1,I2,I3-1))*d12(axis3) )

#define UX21(I1,I2,I3,KD)  ( RX(I1,I2,I3)*UR2(I1,I2,I3,KD) )

#define UX22(I1,I2,I3,KD)  ( RX(I1,I2,I3)*UR2(I1,I2,I3,KD)  \
                            +SX(I1,I2,I3)*US2(I1,I2,I3,KD) )
#define UY22(I1,I2,I3,KD)  ( RY(I1,I2,I3)*UR2(I1,I2,I3,KD)  \
                           +SY(I1,I2,I3)*US2(I1,I2,I3,KD) )

#define UX23(I1,I2,I3,KD) ( RX(I1,I2,I3)*UR2(I1,I2,I3,KD)  \
                           +SX(I1,I2,I3)*US2(I1,I2,I3,KD)  \
                           +TX(I1,I2,I3)*UT2(I1,I2,I3,KD) )
#define UY23(I1,I2,I3,KD) ( RY(I1,I2,I3)*UR2(I1,I2,I3,KD)  \
                           +SY(I1,I2,I3)*US2(I1,I2,I3,KD)  \
                           +TY(I1,I2,I3)*UT2(I1,I2,I3,KD) )
#define UZ23(I1,I2,I3,KD) ( RZ(I1,I2,I3)*UR2(I1,I2,I3,KD)  \
                           +SZ(I1,I2,I3)*US2(I1,I2,I3,KD)  \
                           +TZ(I1,I2,I3)*UT2(I1,I2,I3,KD) )

#define RXX2(I1,I2,I3) ( RX(I1,I2,I3)*RXR2(I1,I2,I3)  \
                        +SX(I1,I2,I3)*RXS2(I1,I2,I3) )
#define RXY2(I1,I2,I3) ( RX(I1,I2,I3)*RYR2(I1,I2,I3)  \
                        +SX(I1,I2,I3)*RYS2(I1,I2,I3) )
#define RYY2(I1,I2,I3) ( RY(I1,I2,I3)*RYR2(I1,I2,I3)  \
                        +SY(I1,I2,I3)*RYS2(I1,I2,I3) )
#define SXX2(I1,I2,I3) ( RX(I1,I2,I3)*SXR2(I1,I2,I3)  \
                        +SX(I1,I2,I3)*SXS2(I1,I2,I3) )
#define SXY2(I1,I2,I3) ( RX(I1,I2,I3)*SYR2(I1,I2,I3)  \
                        +SX(I1,I2,I3)*SYS2(I1,I2,I3) )
#define SYY2(I1,I2,I3) ( RY(I1,I2,I3)*SYR2(I1,I2,I3)  \
                        +SY(I1,I2,I3)*SYS2(I1,I2,I3) )

#define RXX23(I1,I2,I3) ( RX(I1,I2,I3)*RXR2(I1,I2,I3)  \
                         +SX(I1,I2,I3)*RXS2(I1,I2,I3)  \
                         +TX(I1,I2,I3)*RXT2(I1,I2,I3) )
#define RXY23(I1,I2,I3) ( RX(I1,I2,I3)*RYR2(I1,I2,I3)  \
                         +SX(I1,I2,I3)*RYS2(I1,I2,I3)  \
                         +TX(I1,I2,I3)*RYT2(I1,I2,I3) )
#define RXZ23(I1,I2,I3) ( RX(I1,I2,I3)*RZR2(I1,I2,I3)  \
                         +SX(I1,I2,I3)*RZS2(I1,I2,I3)  \
                         +TX(I1,I2,I3)*RZT2(I1,I2,I3) )
#define RYY23(I1,I2,I3) ( RY(I1,I2,I3)*RYR2(I1,I2,I3)  \
                         +SY(I1,I2,I3)*RYS2(I1,I2,I3)  \
                         +TY(I1,I2,I3)*RYT2(I1,I2,I3) )
#define RYZ23(I1,I2,I3) ( RY(I1,I2,I3)*RZR2(I1,I2,I3)  \
                         +SY(I1,I2,I3)*RZS2(I1,I2,I3)  \
                         +TY(I1,I2,I3)*RZT2(I1,I2,I3) )
#define RZZ23(I1,I2,I3) ( RZ(I1,I2,I3)*RZR2(I1,I2,I3)  \
                         +SZ(I1,I2,I3)*RZS2(I1,I2,I3)  \
                         +TZ(I1,I2,I3)*RZT2(I1,I2,I3) )
#define SXX23(I1,I2,I3) ( RX(I1,I2,I3)*SXR2(I1,I2,I3)  \
                         +SX(I1,I2,I3)*SXS2(I1,I2,I3)  \
                         +TX(I1,I2,I3)*SXT2(I1,I2,I3) )
#define SXY23(I1,I2,I3) ( RX(I1,I2,I3)*SYR2(I1,I2,I3)  \
                         +SX(I1,I2,I3)*SYS2(I1,I2,I3)  \
                         +TX(I1,I2,I3)*SYT2(I1,I2,I3) )
#define SXZ23(I1,I2,I3) ( RX(I1,I2,I3)*SZR2(I1,I2,I3)  \
                         +SX(I1,I2,I3)*SZS2(I1,I2,I3)  \
                         +TX(I1,I2,I3)*SZT2(I1,I2,I3) )
#define SYY23(I1,I2,I3) ( RY(I1,I2,I3)*SYR2(I1,I2,I3)  \
                         +SY(I1,I2,I3)*SYS2(I1,I2,I3)  \
                         +TY(I1,I2,I3)*SYT2(I1,I2,I3) )
#define SYZ23(I1,I2,I3) ( RY(I1,I2,I3)*SZR2(I1,I2,I3)  \
                         +SY(I1,I2,I3)*SZS2(I1,I2,I3)  \
                         +TY(I1,I2,I3)*SZT2(I1,I2,I3) )
#define SZZ23(I1,I2,I3) ( RZ(I1,I2,I3)*SZR2(I1,I2,I3)  \
                         +SZ(I1,I2,I3)*SZS2(I1,I2,I3)  \
                         +TZ(I1,I2,I3)*SZT2(I1,I2,I3) )
#define TXX23(I1,I2,I3) ( RX(I1,I2,I3)*TXR2(I1,I2,I3)  \
                         +SX(I1,I2,I3)*TXS2(I1,I2,I3)  \
                         +TX(I1,I2,I3)*TXT2(I1,I2,I3) )
#define TXY23(I1,I2,I3) ( RX(I1,I2,I3)*TYR2(I1,I2,I3)  \
                         +SX(I1,I2,I3)*TYS2(I1,I2,I3)  \
                         +TX(I1,I2,I3)*TYT2(I1,I2,I3) )
#define TXZ23(I1,I2,I3) ( RX(I1,I2,I3)*TZR2(I1,I2,I3)  \
                         +SX(I1,I2,I3)*TZS2(I1,I2,I3)  \
                         +TX(I1,I2,I3)*TZT2(I1,I2,I3) )
#define TYY23(I1,I2,I3) ( RY(I1,I2,I3)*TYR2(I1,I2,I3)  \
                         +SY(I1,I2,I3)*TYS2(I1,I2,I3)  \
                         +TY(I1,I2,I3)*TYT2(I1,I2,I3) )
#define TYZ23(I1,I2,I3) ( RY(I1,I2,I3)*TZR2(I1,I2,I3)  \
                         +SY(I1,I2,I3)*TZS2(I1,I2,I3)  \
                         +TY(I1,I2,I3)*TZT2(I1,I2,I3) )
#define TZZ23(I1,I2,I3) ( RZ(I1,I2,I3)*TZR2(I1,I2,I3)  \
                         +SZ(I1,I2,I3)*TZS2(I1,I2,I3)  \
                         +TZ(I1,I2,I3)*TZT2(I1,I2,I3) )

#define URR2(I1,I2,I3,KD)                                          \
       ( ( -2.*U(I1,I2,I3,KD)                                      \
          +   (U(I1+1,I2,I3,KD)+U(I1-1,I2,I3,KD))  )*d22(axis1) )
#define USS2(I1,I2,I3,KD)                                          \
       ( ( -2.*U(I1,I2,I3,KD)                                      \
          +    (U(I1,I2+1,I3,KD)+U(I1,I2-1,I3,KD)) )*d22(axis2) )
#define URS2(I1,I2,I3,KD)                                          \
       (  (UR2(I1,I2+1,I3,KD)-UR2(I1,I2-1,I3,KD))*d12(axis2) )
#define UTT2(I1,I2,I3,KD)                                          \
       ( ( -2.*U(I1,I2,I3,KD)                                      \
          +   (U(I1,I2,I3+1,KD)+U(I1,I2,I3-1,KD))  )*d22(axis3) )
#define URT2(I1,I2,I3,KD)                                          \
       (   (UR2(I1,I2,I3+1,KD)-UR2(I1,I2,I3-1,KD))*d12(axis3) )
#define UST2(I1,I2,I3,KD)                                          \
       (  (US2(I1,I2,I3+1,KD)-US2(I1,I2,I3-1,KD))*d12(axis3) )

#define UXX21(I1,I2,I3,KD) (SQR(RX(I1,I2,I3))*URR2(I1,I2,I3,KD))

#define UXX22(I1,I2,I3,KD)  (                                       \
        (SQR(RX(I1,I2,I3))              )*URR2(I1,I2,I3,KD)        \
        +2.*(RX(I1,I2,I3)*SX(I1,I2,I3)                           )  \
                                          *URS2(I1,I2,I3,KD)        \
        +(SQR(SX(I1,I2,I3))                )*USS2(I1,I2,I3,KD)        \
             +(RXX2(I1,I2,I3)              )*UR2(I1,I2,I3,KD)       \
             +(SXX2(I1,I2,I3)              )*US2(I1,I2,I3,KD)      \
                            )
#define UXX22A(I1,I2,I3,KD)  (                                       \
        (SQR(RX(I1,I2,I3))              )*URR2(I1,I2,I3,KD)        \
                            )
#define UXX22B(I1,I2,I3,KD)  (                                       \
        +2.*(RX(I1,I2,I3)*SX(I1,I2,I3)                           )  \
                                          *URS2(I1,I2,I3,KD)        \
                            )
#define UXX22C(I1,I2,I3,KD)  (                                       \
        +(SQR(SX(I1,I2,I3))                )*USS2(I1,I2,I3,KD)        \
                            )
#define UXX22D(I1,I2,I3,KD)  (                                       \
             +(RXX2(I1,I2,I3)              )*UR2(I1,I2,I3,KD)       \
                            )
#define UXX22E(I1,I2,I3,KD)  (                                       \
             +(SXX2(I1,I2,I3)              )*US2(I1,I2,I3,KD)      \
                            )




#define UYY22(I1,I2,I3,KD)                                           \
       ( (              SQR(RY(I1,I2,I3)))*URR2(I1,I2,I3,KD)        \
        +2.*(                           RY(I1,I2,I3)*SY(I1,I2,I3))  \
                                          *URS2(I1,I2,I3,KD)        \
        +(              SQR(SY(I1,I2,I3)))*USS2(I1,I2,I3,KD)        \
             +(              RYY2(I1,I2,I3))*UR2(I1,I2,I3,KD)       \
             +(              SYY2(I1,I2,I3))*US2(I1,I2,I3,KD)  )

#define UYY22A(I1,I2,I3,KD)  (                                       \
        (SQR(RY(I1,I2,I3))              )*URR2(I1,I2,I3,KD)        \
                            )
#define UYY22B(I1,I2,I3,KD)  (                                       \
        +2.*(RY(I1,I2,I3)*SY(I1,I2,I3)                           )  \
                                          *URS2(I1,I2,I3,KD)        \
                            )
#define UYY22C(I1,I2,I3,KD)  (                                       \
        +(SQR(SY(I1,I2,I3))                )*USS2(I1,I2,I3,KD)        \
                            )
#define UYY22D(I1,I2,I3,KD)  (                                       \
             +(RYY2(I1,I2,I3)              )*UR2(I1,I2,I3,KD)       \
                            )
#define UYY22E(I1,I2,I3,KD)  (                                       \
             +(SYY2(I1,I2,I3)              )*US2(I1,I2,I3,KD)      \
                            )

#define UXY22(I1,I2,I3,KD)                                           \
       (    RX(I1,I2,I3)*RY(I1,I2,I3)*URR2(I1,I2,I3,KD)             \
        +(RX(I1,I2,I3)*SY(I1,I2,I3)+RY(I1,I2,I3)*SX(I1,I2,I3))      \
                                     *URS2(I1,I2,I3,KD)             \
        +   SX(I1,I2,I3)*SY(I1,I2,I3)*USS2(I1,I2,I3,KD)             \
          +RXY2(I1,I2,I3)              *UR2(I1,I2,I3,KD)            \
          +SXY2(I1,I2,I3)              *US2(I1,I2,I3,KD)  )

#define UXX23(I1,I2,I3,KD) (                                        \
         SQR(RX(I1,I2,I3)) *URR2(I1,I2,I3,KD)                        \
        +SQR(SX(I1,I2,I3)) *USS2(I1,I2,I3,KD)                        \
        +SQR(TX(I1,I2,I3)) *UTT2(I1,I2,I3,KD)                        \
        +2.*RX(I1,I2,I3)*SX(I1,I2,I3)*URS2(I1,I2,I3,KD)            \
        +2.*RX(I1,I2,I3)*TX(I1,I2,I3)*URT2(I1,I2,I3,KD)            \
        +2.*SX(I1,I2,I3)*TX(I1,I2,I3)*UST2(I1,I2,I3,KD)            \
        +RXX23(I1,I2,I3)*UR2(I1,I2,I3,KD)                          \
        +SXX23(I1,I2,I3)*US2(I1,I2,I3,KD)                          \
        +TXX23(I1,I2,I3)*UT2(I1,I2,I3,KD)                          \
                           )
#define UXX23A(I1,I2,I3,KD) (                                        \
         SQR(RX(I1,I2,I3)) *URR2(I1,I2,I3,KD)                        \
        +SQR(SX(I1,I2,I3)) *USS2(I1,I2,I3,KD)                        \
        +SQR(TX(I1,I2,I3)) *UTT2(I1,I2,I3,KD)                        \
                           )
#define UXX23B(I1,I2,I3,KD) (                                        \
        +2.*RX(I1,I2,I3)*SX(I1,I2,I3)*URS2(I1,I2,I3,KD)            \
        +2.*RX(I1,I2,I3)*TX(I1,I2,I3)*URT2(I1,I2,I3,KD)            \
        +2.*SX(I1,I2,I3)*TX(I1,I2,I3)*UST2(I1,I2,I3,KD)            \
                           )
#define UXX23C(I1,I2,I3,KD) (                                        \
        +RXX23(I1,I2,I3)*UR2(I1,I2,I3,KD)                          \
                           )
#define UXX23D(I1,I2,I3,KD) (                                        \
        +SXX23(I1,I2,I3)*US2(I1,I2,I3,KD)                          \
                           )
#define UXX23E(I1,I2,I3,KD) (                                        \
        +TXX23(I1,I2,I3)*UT2(I1,I2,I3,KD)                          \
                           )



#define UYY23(I1,I2,I3,KD)   \
       ( SQR(RY(I1,I2,I3)) *URR2(I1,I2,I3,KD)   \
        +SQR(SY(I1,I2,I3)) *USS2(I1,I2,I3,KD)   \
        +SQR(TY(I1,I2,I3)) *UTT2(I1,I2,I3,KD)   \
        +2.*RY(I1,I2,I3)*SY(I1,I2,I3)*URS2(I1,I2,I3,KD)   \
        +2.*RY(I1,I2,I3)*TY(I1,I2,I3)*URT2(I1,I2,I3,KD)   \
        +2.*SY(I1,I2,I3)*TY(I1,I2,I3)*UST2(I1,I2,I3,KD)   \
        +RYY23(I1,I2,I3)*UR2(I1,I2,I3,KD)   \
        +SYY23(I1,I2,I3)*US2(I1,I2,I3,KD)   \
        +TYY23(I1,I2,I3)*UT2(I1,I2,I3,KD) )

#define UYY23A(I1,I2,I3,KD) (                                        \
         SQR(RY(I1,I2,I3)) *URR2(I1,I2,I3,KD)                        \
        +SQR(SY(I1,I2,I3)) *USS2(I1,I2,I3,KD)                        \
        +SQR(TY(I1,I2,I3)) *UTT2(I1,I2,I3,KD)                        \
                           )
#define UYY23B(I1,I2,I3,KD) (                                        \
        +2.*RY(I1,I2,I3)*SY(I1,I2,I3)*URS2(I1,I2,I3,KD)            \
        +2.*RY(I1,I2,I3)*TY(I1,I2,I3)*URT2(I1,I2,I3,KD)            \
        +2.*SY(I1,I2,I3)*TY(I1,I2,I3)*UST2(I1,I2,I3,KD)            \
                           )
#define UYY23C(I1,I2,I3,KD) (                                        \
        +RYY23(I1,I2,I3)*UR2(I1,I2,I3,KD)                          \
                           )
#define UYY23D(I1,I2,I3,KD) (                                        \
        +SYY23(I1,I2,I3)*US2(I1,I2,I3,KD)                          \
                           )
#define UYY23E(I1,I2,I3,KD) (                                        \
        +TYY23(I1,I2,I3)*UT2(I1,I2,I3,KD)                          \
                           )

#define UZZ23(I1,I2,I3,KD)   \
       ( SQR(RZ(I1,I2,I3)) *URR2(I1,I2,I3,KD)   \
        +SQR(SZ(I1,I2,I3)) *USS2(I1,I2,I3,KD)   \
        +SQR(TZ(I1,I2,I3)) *UTT2(I1,I2,I3,KD)   \
        +2.*RZ(I1,I2,I3)*SZ(I1,I2,I3)*URS2(I1,I2,I3,KD)   \
        +2.*RZ(I1,I2,I3)*TZ(I1,I2,I3)*URT2(I1,I2,I3,KD)   \
        +2.*SZ(I1,I2,I3)*TZ(I1,I2,I3)*UST2(I1,I2,I3,KD)   \
        +RZZ23(I1,I2,I3)*UR2(I1,I2,I3,KD)   \
        +SZZ23(I1,I2,I3)*US2(I1,I2,I3,KD)   \
        +TZZ23(I1,I2,I3)*UT2(I1,I2,I3,KD) )

#define UZZ23A(I1,I2,I3,KD) (                                        \
         SQR(RZ(I1,I2,I3)) *URR2(I1,I2,I3,KD)                        \
        +SQR(SZ(I1,I2,I3)) *USS2(I1,I2,I3,KD)                        \
        +SQR(TZ(I1,I2,I3)) *UTT2(I1,I2,I3,KD)                        \
                           )
#define UZZ23B(I1,I2,I3,KD) (                                        \
        +2.*RZ(I1,I2,I3)*SZ(I1,I2,I3)*URS2(I1,I2,I3,KD)            \
        +2.*RZ(I1,I2,I3)*TZ(I1,I2,I3)*URT2(I1,I2,I3,KD)            \
        +2.*SZ(I1,I2,I3)*TZ(I1,I2,I3)*UST2(I1,I2,I3,KD)            \
                           )
#define UZZ23C(I1,I2,I3,KD) (                                        \
        +RZZ23(I1,I2,I3)*UR2(I1,I2,I3,KD)                          \
                           )
#define UZZ23D(I1,I2,I3,KD) (                                        \
        +SZZ23(I1,I2,I3)*US2(I1,I2,I3,KD)                          \
                           )
#define UZZ23E(I1,I2,I3,KD) (                                        \
        +TZZ23(I1,I2,I3)*UT2(I1,I2,I3,KD)                          \
                           )


#define UXY23(I1,I2,I3,KD)      \
     ( RX(I1,I2,I3)*RY(I1,I2,I3)*URR2(I1,I2,I3,KD)    \
      +SX(I1,I2,I3)*SY(I1,I2,I3)*USS2(I1,I2,I3,KD)    \
      +TX(I1,I2,I3)*TY(I1,I2,I3)*UTT2(I1,I2,I3,KD)    \
      +(RX(I1,I2,I3)*SY(I1,I2,I3)+RY(I1,I2,I3)*SX(I1,I2,I3))    \
                                       *URS2(I1,I2,I3,KD)    \
      +(RX(I1,I2,I3)*TY(I1,I2,I3)+RY(I1,I2,I3)*TX(I1,I2,I3))    \
                                       *URT2(I1,I2,I3,KD)    \
      +(SX(I1,I2,I3)*TY(I1,I2,I3)+SY(I1,I2,I3)*TX(I1,I2,I3))    \
                                       *UST2(I1,I2,I3,KD)    \
      +RXY23(I1,I2,I3)*UR2(I1,I2,I3,KD)    \
      +SXY23(I1,I2,I3)*US2(I1,I2,I3,KD)    \
      +TXY23(I1,I2,I3)*UT2(I1,I2,I3,KD) )                  


#define UXZ23(I1,I2,I3,KD)   \
       (  RX(I1,I2,I3)*RZ(I1,I2,I3)*URR2(I1,I2,I3,KD)   \
         +SX(I1,I2,I3)*SZ(I1,I2,I3)*USS2(I1,I2,I3,KD)   \
         +TX(I1,I2,I3)*TZ(I1,I2,I3)*UTT2(I1,I2,I3,KD)   \
         +(RX(I1,I2,I3)*SZ(I1,I2,I3)+RZ(I1,I2,I3)*SX(I1,I2,I3))   \
                                          *URS2(I1,I2,I3,KD)   \
         +(RX(I1,I2,I3)*TZ(I1,I2,I3)+RZ(I1,I2,I3)*TX(I1,I2,I3))   \
                                          *URT2(I1,I2,I3,KD)   \
         +(SX(I1,I2,I3)*TZ(I1,I2,I3)+SZ(I1,I2,I3)*TX(I1,I2,I3))   \
                                          *UST2(I1,I2,I3,KD)   \
         +RXZ23(I1,I2,I3)*UR2(I1,I2,I3,KD)   \
         +SXZ23(I1,I2,I3)*US2(I1,I2,I3,KD)   \
         +TXZ23(I1,I2,I3)*UT2(I1,I2,I3,KD)  )

#define UYZ23(I1,I2,I3,KD)   \
       (  RY(I1,I2,I3)*RZ(I1,I2,I3)*URR2(I1,I2,I3,KD)   \
         +SY(I1,I2,I3)*SZ(I1,I2,I3)*USS2(I1,I2,I3,KD)   \
         +TY(I1,I2,I3)*TZ(I1,I2,I3)*UTT2(I1,I2,I3,KD)   \
         +(RY(I1,I2,I3)*SZ(I1,I2,I3)+RZ(I1,I2,I3)*SY(I1,I2,I3))   \
                                          *URS2(I1,I2,I3,KD)   \
         +(RY(I1,I2,I3)*TZ(I1,I2,I3)+RZ(I1,I2,I3)*TY(I1,I2,I3))   \
                                          *URT2(I1,I2,I3,KD)   \
         +(SY(I1,I2,I3)*TZ(I1,I2,I3)+SZ(I1,I2,I3)*TY(I1,I2,I3))   \
                                          *UST2(I1,I2,I3,KD)   \
         +RYZ23(I1,I2,I3)*UR2(I1,I2,I3,KD)   \
         +SYZ23(I1,I2,I3)*US2(I1,I2,I3,KD)   \
         +TYZ23(I1,I2,I3)*UT2(I1,I2,I3,KD)  )


#define LAPLACIAN21(I1,I2,I3,KD)  UXX21(I1,I2,I3,KD)

#define LAPLACIAN22(I1,I2,I3,KD)  (                                \
      (SQR(RX(I1,I2,I3))+SQR(RY(I1,I2,I3)))*URR2(I1,I2,I3,KD)     \
     +2.*(RX(I1,I2,I3)*SX(I1,I2,I3)+ RY(I1,I2,I3)*SY(I1,I2,I3))   \
                                       *URS2(I1,I2,I3,KD)         \
     +(SQR(SX(I1,I2,I3))+SQR(SY(I1,I2,I3)))*USS2(I1,I2,I3,KD)     \
          +(RXX2(I1,I2,I3)+RYY2(I1,I2,I3))*UR2(I1,I2,I3,KD)       \
          +(SXX2(I1,I2,I3)+SYY2(I1,I2,I3))*US2(I1,I2,I3,KD)       \
                                 )

#define LAPLACIAN22A(I1,I2,I3,KD)  (                                \
      (SQR(RX(I1,I2,I3))+SQR(RY(I1,I2,I3)))*URR2(I1,I2,I3,KD)     \
                                 )
#define LAPLACIAN22B(I1,I2,I3,KD)  (                                \
     +2.*(RX(I1,I2,I3)*SX(I1,I2,I3)+ RY(I1,I2,I3)*SY(I1,I2,I3))   \
                                       *URS2(I1,I2,I3,KD)         \
                                 )
#define LAPLACIAN22C(I1,I2,I3,KD)  (                                \
     +(SQR(SX(I1,I2,I3))+SQR(SY(I1,I2,I3)))*USS2(I1,I2,I3,KD)     \
                                 )
#define LAPLACIAN22D(I1,I2,I3,KD)  (                                \
          +(RXX2(I1,I2,I3)+RYY2(I1,I2,I3))*UR2(I1,I2,I3,KD)       \
                                 )
#define LAPLACIAN22E(I1,I2,I3,KD)  (                                \
          +(SXX2(I1,I2,I3)+SYY2(I1,I2,I3))*US2(I1,I2,I3,KD)       \
                                 )
            
#define LAPLACIAN23(I1,I2,I3,KD)    (                             \
      (SQR(RX(I1,I2,I3))+SQR(RY(I1,I2,I3))+SQR(RZ(I1,I2,I3)))     \
                                       *URR2(I1,I2,I3,KD)         \
     +(SQR(SX(I1,I2,I3))+SQR(SY(I1,I2,I3))+SQR(SZ(I1,I2,I3)))     \
                                       *USS2(I1,I2,I3,KD)         \
     +(SQR(TX(I1,I2,I3))+SQR(TY(I1,I2,I3))+SQR(TZ(I1,I2,I3)))    \
                                       *UTT2(I1,I2,I3,KD)         \
     +2.*(RX(I1,I2,I3)*SX(I1,I2,I3)+ RY(I1,I2,I3)*SY(I1,I2,I3)    \
         +RZ(I1,I2,I3)*SZ(I1,I2,I3))   *URS2(I1,I2,I3,KD)         \
     +2.*(RX(I1,I2,I3)*TX(I1,I2,I3)+ RY(I1,I2,I3)*TY(I1,I2,I3)    \
         +RZ(I1,I2,I3)*TZ(I1,I2,I3))   *URT2(I1,I2,I3,KD)         \
     +2.*(SX(I1,I2,I3)*TX(I1,I2,I3)+ SY(I1,I2,I3)*TY(I1,I2,I3)    \
         +SZ(I1,I2,I3)*TZ(I1,I2,I3))   *UST2(I1,I2,I3,KD)         \
          +(RXX23(I1,I2,I3)+RYY23(I1,I2,I3)+RZZ23(I1,I2,I3))      \
                                       *UR2(I1,I2,I3,KD)          \
          +(SXX23(I1,I2,I3)+SYY23(I1,I2,I3)+SZZ23(I1,I2,I3))      \
                                       *US2(I1,I2,I3,KD)          \
          +(TXX23(I1,I2,I3)+TYY23(I1,I2,I3)+TZZ23(I1,I2,I3))      \
                                       *UT2(I1,I2,I3,KD)          \
                                  )                            
#define LAPLACIAN23A(I1,I2,I3,KD)    (                             \
      (SQR(RX(I1,I2,I3))+SQR(RY(I1,I2,I3))+SQR(RZ(I1,I2,I3)))     \
                                       *URR2(I1,I2,I3,KD)         \
     +(SQR(SX(I1,I2,I3))+SQR(SY(I1,I2,I3))+SQR(SZ(I1,I2,I3)))     \
                                       *USS2(I1,I2,I3,KD)         \
     +(SQR(TX(I1,I2,I3))+SQR(TY(I1,I2,I3))+SQR(TZ(I1,I2,I3)))    \
                                       *UTT2(I1,I2,I3,KD)         \
                                  )                            
#define LAPLACIAN23B(I1,I2,I3,KD)    (                             \
     +2.*(RX(I1,I2,I3)*SX(I1,I2,I3)+ RY(I1,I2,I3)*SY(I1,I2,I3)    \
         +RZ(I1,I2,I3)*SZ(I1,I2,I3))   *URS2(I1,I2,I3,KD)         \
     +2.*(RX(I1,I2,I3)*TX(I1,I2,I3)+ RY(I1,I2,I3)*TY(I1,I2,I3)    \
         +RZ(I1,I2,I3)*TZ(I1,I2,I3))   *URT2(I1,I2,I3,KD)         \
     +2.*(SX(I1,I2,I3)*TX(I1,I2,I3)+ SY(I1,I2,I3)*TY(I1,I2,I3)    \
         +SZ(I1,I2,I3)*TZ(I1,I2,I3))   *UST2(I1,I2,I3,KD)         \
                                  )                            
#define LAPLACIAN23C(I1,I2,I3,KD)    (                             \
          +(RXX23(I1,I2,I3)+RYY23(I1,I2,I3)+RZZ23(I1,I2,I3))      \
                                       *UR2(I1,I2,I3,KD)          \
                                  )                            
#define LAPLACIAN23D(I1,I2,I3,KD)    (                             \
          +(SXX23(I1,I2,I3)+SYY23(I1,I2,I3)+SZZ23(I1,I2,I3))      \
                                       *US2(I1,I2,I3,KD)          \
                                  )                            
#define LAPLACIAN23E(I1,I2,I3,KD)    (                             \
          +(TXX23(I1,I2,I3)+TYY23(I1,I2,I3)+TZZ23(I1,I2,I3))      \
                                       *UT2(I1,I2,I3,KD)          \
                                  )                            

// Define these even though they shouldn't be used
#define UY21(I1,I2,I3,KD)  0.
#define UZ21(I1,I2,I3,KD)  0.
#define UXY21(I1,I2,I3,KD) 0.
#define UXZ21(I1,I2,I3,KD) 0.
#define UYY21(I1,I2,I3,KD) 0.
#define UYZ21(I1,I2,I3,KD) 0.
#define UZZ21(I1,I2,I3,KD) 0.

#define UZ22(I1,I2,I3,KD)  0.
#define UXZ22(I1,I2,I3,KD) 0.
#define UYZ22(I1,I2,I3,KD) 0.
#define UZZ22(I1,I2,I3,KD) 0.

#define UZZ22A(I1,I2,I3,KD) 0.
#define UZZ22B(I1,I2,I3,KD) 0.
#define UZZ22C(I1,I2,I3,KD) 0.
#define UZZ22D(I1,I2,I3,KD) 0.
#define UZZ22E(I1,I2,I3,KD) 0.


//============================================================================================
// Define derivatives for a rectangular grid
//
// These definitions assume that the follwoing values are defined:
//    h12(axis) = 1./(2*deltaX(axis))
//    h22(axis) = 1./(deltaX(axis)^2)
//============================================================================================

// 22 means 2nd order, 2D:

#define UX22R(I1,I2,I3,KD) ( (U(I1+1,I2,I3,KD)-U(I1-1,I2,I3,KD))*h21(axis1) )
#define UY22R(I1,I2,I3,KD) ( (U(I1,I2+1,I3,KD)-U(I1,I2-1,I3,KD))*h21(axis2) )
#define UZ22R(I1,I2,I3,KD) ( (U(I1,I2,I3+1,KD)-U(I1,I2,I3-1,KD))*h21(axis3) )

#define UXX22R(I1,I2,I3,KD) ( (U(I1+1,I2,I3,KD)-2.*U(I1,I2,I3,KD)+U(I1-1,I2,I3,KD))*h22(axis1) )
#define UYY22R(I1,I2,I3,KD) ( (U(I1,I2+1,I3,KD)-2.*U(I1,I2,I3,KD)+U(I1,I2-1,I3,KD))*h22(axis2) )
#define UZZ22R(I1,I2,I3,KD) ( (U(I1,I2,I3+1,KD)-2.*U(I1,I2,I3,KD)+U(I1,I2,I3-1,KD))*h22(axis3) )


#define UXY22R(I1,I2,I3,KD) ( (U(I1+1,I2+1,I3,KD)-U(I1-1,I2+1,I3,KD)                 \
                              -U(I1+1,I2-1,I3,KD)+U(I1-1,I2-1,I3,KD))*(h21(axis1)*h21(axis2)) )

#define UXZ22R(I1,I2,I3,KD) ( (U(I1+1,I2,I3+1,KD)-U(I1-1,I2,I3+1,KD)                 \
                              -U(I1+1,I2,I3-1,KD)+U(I1-1,I2,I3-1,KD))*(h21(axis1)*h21(axis3)) )

#define UYZ22R(I1,I2,I3,KD) ( (U(I1,I2+1,I3+1,KD)-U(I1,I2+1,I3-1,KD)                 \
                              -U(I1,I2-1,I3+1,KD)+U(I1,I2-1,I3-1,KD))*(h21(axis2)*h21(axis3)) )


#define LAPLACIAN22R(I1,I2,I3,KD)  ( UXX22R(I1,I2,I3,KD)+UYY22R(I1,I2,I3,KD) )


// Here are the versions for 3D, most are the same as 2D

#define UX23R(I1,I2,I3,KD) UX22R(I1,I2,I3,KD) 
#define UY23R(I1,I2,I3,KD) UY22R(I1,I2,I3,KD) 
#define UZ23R(I1,I2,I3,KD) UZ22R(I1,I2,I3,KD) 
			   
#define UXX23R(I1,I2,I3,KD) UXX22R(I1,I2,I3,KD)
#define UYY23R(I1,I2,I3,KD) UYY22R(I1,I2,I3,KD)
#define UZZ23R(I1,I2,I3,KD) UZZ22R(I1,I2,I3,KD)
			   
#define UXY23R(I1,I2,I3,KD) UXY22R(I1,I2,I3,KD)
#define UXZ23R(I1,I2,I3,KD) UXZ22R(I1,I2,I3,KD)
#define UYZ23R(I1,I2,I3,KD) UYZ22R(I1,I2,I3,KD)
                           
#define LAPLACIAN23R(I1,I2,I3,KD)  ( UXX22R(I1,I2,I3,KD)+UYY22R(I1,I2,I3,KD)+UZZ22R(I1,I2,I3,KD) )

// Here are the versions for 1D, most are the same as 2D

#define UX21R(I1,I2,I3,KD) UX22R(I1,I2,I3,KD) 
#define UY21R(I1,I2,I3,KD) UY22R(I1,I2,I3,KD) 
#define UZ21R(I1,I2,I3,KD) UZ22R(I1,I2,I3,KD) 
			   
#define UXX21R(I1,I2,I3,KD) UXX22R(I1,I2,I3,KD)
#define UYY21R(I1,I2,I3,KD) UYY22R(I1,I2,I3,KD)
#define UZZ21R(I1,I2,I3,KD) UZZ22R(I1,I2,I3,KD)
			   
#define UXY21R(I1,I2,I3,KD) UXY22R(I1,I2,I3,KD)
#define UXZ21R(I1,I2,I3,KD) UXZ22R(I1,I2,I3,KD)
#define UYZ21R(I1,I2,I3,KD) UYZ22R(I1,I2,I3,KD)
                           
#define LAPLACIAN21R(I1,I2,I3,KD)  UXX22R(I1,I2,I3,KD)

