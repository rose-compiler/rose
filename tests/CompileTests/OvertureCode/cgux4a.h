//
// CGUX statement functions for fourth-order difference approximations
//

#define RX(I1,I2,I3) inverseVertexDerivative(I1,I2,I3,0)
#define SX(I1,I2,I3) inverseVertexDerivative(I1,I2,I3,1)
#define TX(I1,I2,I3) inverseVertexDerivative(I1,I2,I3,2)
#define RY(I1,I2,I3) inverseVertexDerivative(I1,I2,I3,0+numberOfDimensions)
#define SY(I1,I2,I3) inverseVertexDerivative(I1,I2,I3,1+numberOfDimensions)
#define TY(I1,I2,I3) inverseVertexDerivative(I1,I2,I3,5)
#define RZ(I1,I2,I3) inverseVertexDerivative(I1,I2,I3,6)
#define SZ(I1,I2,I3) inverseVertexDerivative(I1,I2,I3,7)
#define TZ(I1,I2,I3) inverseVertexDerivative(I1,I2,I3,8)

#define UR4A(I1,I2,I3,KD) ( (8.*(U(I1+1,I2,I3,KD)-U(I1-1,I2,I3,KD))  \
                               -(U(I1+2,I2,I3,KD)-U(I1-2,I2,I3,KD)))*d14(axis1) )
#define US4A(I1,I2,I3,KD) ( (8.*(U(I1,I2+1,I3,KD)-U(I1,I2-1,I3,KD))  \
                               -(U(I1,I2+2,I3,KD)-U(I1,I2-2,I3,KD)))*d14(axis2) )
#define UT4A(I1,I2,I3,KD) ( (8.*(U(I1,I2,I3+1,KD)-U(I1,I2,I3-1,KD)) \
                               -(U(I1,I2,I3+2,KD)-U(I1,I2,I3-2,KD)))*d14(axis3) )

#define RXR4(I1,I2,I3) ( (8.*(RX(I1+1,I2,I3)-RX(I1-1,I2,I3))    \
                            -(RX(I1+2,I2,I3)-RX(I1-2,I2,I3)))*d14(axis1) )
#define RYR4(I1,I2,I3) ( (8.*(RY(I1+1,I2,I3)-RY(I1-1,I2,I3))    \
                            -(RY(I1+2,I2,I3)-RY(I1-2,I2,I3)))*d14(axis1) )
#define RZR4(I1,I2,I3) ( (8.*(RZ(I1+1,I2,I3)-RZ(I1-1,I2,I3))    \
                            -(RZ(I1+2,I2,I3)-RZ(I1-2,I2,I3)))*d14(axis1) )
#define RXS4(I1,I2,I3) ( (8.*(RX(I1,I2+1,I3)-RX(I1,I2-1,I3))    \
                            -(RX(I1,I2+2,I3)-RX(I1,I2-2,I3)))*d14(axis2) )
#define RYS4(I1,I2,I3) ( (8.*(RY(I1,I2+1,I3)-RY(I1,I2-1,I3))    \
                            -(RY(I1,I2+2,I3)-RY(I1,I2-2,I3)))*d14(axis2) )
#define RZS4(I1,I2,I3) ( (8.*(RZ(I1,I2+1,I3)-RZ(I1,I2-1,I3))    \
                            -(RZ(I1,I2+2,I3)-RZ(I1,I2-2,I3)))*d14(axis2) )
#define RXT4(I1,I2,I3) ( (8.*(RX(I1,I2,I3+1)-RX(I1,I2,I3-1))    \
                            -(RX(I1,I2,I3+2)-RX(I1,I2,I3-2)))*d14(axis3) )
#define RYT4(I1,I2,I3) ( (8.*(RY(I1,I2,I3+1)-RY(I1,I2,I3-1))    \
                            -(RY(I1,I2,I3+2)-RY(I1,I2,I3-2)))*d14(axis3) )
#define RZT4(I1,I2,I3) ( (8.*(RZ(I1,I2,I3+1)-RZ(I1,I2,I3-1))    \
                            -(RZ(I1,I2,I3+2)-RZ(I1,I2,I3-2)))*d14(axis3) )
#define SXR4(I1,I2,I3) ( (8.*(SX(I1+1,I2,I3)-SX(I1-1,I2,I3))    \
                            -(SX(I1+2,I2,I3)-SX(I1-2,I2,I3)))*d14(axis1) )
#define SYR4(I1,I2,I3) ( (8.*(SY(I1+1,I2,I3)-SY(I1-1,I2,I3))    \
                            -(SY(I1+2,I2,I3)-SY(I1-2,I2,I3)))*d14(axis1) )
#define SZR4(I1,I2,I3) ( (8.*(SZ(I1+1,I2,I3)-SZ(I1-1,I2,I3))    \
                            -(SZ(I1+2,I2,I3)-SZ(I1-2,I2,I3)))*d14(axis1) )
#define SXS4(I1,I2,I3) ( (8.*(SX(I1,I2+1,I3)-SX(I1,I2-1,I3))    \
                            -(SX(I1,I2+2,I3)-SX(I1,I2-2,I3)))*d14(axis2) )
#define SYS4(I1,I2,I3) ( (8.*(SY(I1,I2+1,I3)-SY(I1,I2-1,I3))    \
                            -(SY(I1,I2+2,I3)-SY(I1,I2-2,I3)))*d14(axis2) )
#define SZS4(I1,I2,I3) ( (8.*(SZ(I1,I2+1,I3)-SZ(I1,I2-1,I3))    \
                            -(SZ(I1,I2+2,I3)-SZ(I1,I2-2,I3)))*d14(axis2) )
#define SXT4(I1,I2,I3) ( (8.*(SX(I1,I2,I3+1)-SX(I1,I2,I3-1))    \
                            -(SX(I1,I2,I3+2)-SX(I1,I2,I3-2)))*d14(axis3) )
#define SYT4(I1,I2,I3) ( (8.*(SY(I1,I2,I3+1)-SY(I1,I2,I3-1))    \
                            -(SY(I1,I2,I3+2)-SY(I1,I2,I3-2)))*d14(axis3) )
#define SZT4(I1,I2,I3) ( (8.*(SZ(I1,I2,I3+1)-SZ(I1,I2,I3-1))    \
                            -(SZ(I1,I2,I3+2)-SZ(I1,I2,I3-2)))*d14(axis3) )
#define TXR4(I1,I2,I3) ( (8.*(TX(I1+1,I2,I3)-TX(I1-1,I2,I3))    \
                            -(TX(I1+2,I2,I3)-TX(I1-2,I2,I3)))*d14(axis1) )
#define TYR4(I1,I2,I3) ( (8.*(TY(I1+1,I2,I3)-TY(I1-1,I2,I3))    \
                            -(TY(I1+2,I2,I3)-TY(I1-2,I2,I3)))*d14(axis1) )
#define TZR4(I1,I2,I3) ( (8.*(TZ(I1+1,I2,I3)-TZ(I1-1,I2,I3))    \
                            -(TZ(I1+2,I2,I3)-TZ(I1-2,I2,I3)))*d14(axis1) )
#define TXS4(I1,I2,I3) ( (8.*(TX(I1,I2+1,I3)-TX(I1,I2-1,I3))    \
                            -(TX(I1,I2+2,I3)-TX(I1,I2-2,I3)))*d14(axis2) )
#define TYS4(I1,I2,I3) ( (8.*(TY(I1,I2+1,I3)-TY(I1,I2-1,I3))    \
                            -(TY(I1,I2+2,I3)-TY(I1,I2-2,I3)))*d14(axis2) )
#define TZS4(I1,I2,I3) ( (8.*(TZ(I1,I2+1,I3)-TZ(I1,I2-1,I3))    \
                            -(TZ(I1,I2+2,I3)-TZ(I1,I2-2,I3)))*d14(axis2) )
#define TXT4(I1,I2,I3) ( (8.*(TX(I1,I2,I3+1)-TX(I1,I2,I3-1))    \
                            -(TX(I1,I2,I3+2)-TX(I1,I2,I3-2)))*d14(axis3) )
#define TYT4(I1,I2,I3) ( (8.*(TY(I1,I2,I3+1)-TY(I1,I2,I3-1))    \
                            -(TY(I1,I2,I3+2)-TY(I1,I2,I3-2)))*d14(axis3) )
#define TZT4(I1,I2,I3) ( (8.*(TZ(I1,I2,I3+1)-TZ(I1,I2,I3-1))    \
                            -(TZ(I1,I2,I3+2)-TZ(I1,I2,I3-2)))*d14(axis3) )

#define UX41(I1,I2,I3,KD)  (RX(I1,I2,I3)*UR4(I1,I2,I3,KD))

#define UX42(I1,I2,I3,KD)  ( RX(I1,I2,I3)*UR4(I1,I2,I3,KD)   \
                           +SX(I1,I2,I3)*US4(I1,I2,I3,KD) )
#define UY42(I1,I2,I3,KD)  ( RY(I1,I2,I3)*UR4(I1,I2,I3,KD)   \
                           +SY(I1,I2,I3)*US4(I1,I2,I3,KD) )
#define UX43(I1,I2,I3,KD) ( RX(I1,I2,I3)*UR4(I1,I2,I3,KD)   \
                           +SX(I1,I2,I3)*US4(I1,I2,I3,KD)   \
                           +TX(I1,I2,I3)*UT4(I1,I2,I3,KD) )
#define UY43(I1,I2,I3,KD) ( RY(I1,I2,I3)*UR4(I1,I2,I3,KD)  \
                           +SY(I1,I2,I3)*US4(I1,I2,I3,KD)  \
                           +TY(I1,I2,I3)*UT4(I1,I2,I3,KD) )
#define UZ43(I1,I2,I3,KD) ( RZ(I1,I2,I3)*UR4(I1,I2,I3,KD)  \
                           +SZ(I1,I2,I3)*US4(I1,I2,I3,KD)  \
                           +TZ(I1,I2,I3)*UT4(I1,I2,I3,KD) )

#define RXX4(I1,I2,I3) ( RX(I1,I2,I3)*RXR4(I1,I2,I3)     \
                        +SX(I1,I2,I3)*RXS4(I1,I2,I3) )
#define RXY4(I1,I2,I3) ( RX(I1,I2,I3)*RYR4(I1,I2,I3)     \
                        +SX(I1,I2,I3)*RYS4(I1,I2,I3) )
#define RYY4(I1,I2,I3) ( RY(I1,I2,I3)*RYR4(I1,I2,I3)     \
                        +SY(I1,I2,I3)*RYS4(I1,I2,I3) )
#define SXX4(I1,I2,I3) ( RX(I1,I2,I3)*SXR4(I1,I2,I3)     \
                        +SX(I1,I2,I3)*SXS4(I1,I2,I3) )
#define SXY4(I1,I2,I3) ( RX(I1,I2,I3)*SYR4(I1,I2,I3)     \
                        +SX(I1,I2,I3)*SYS4(I1,I2,I3) )
#define SYY4(I1,I2,I3) ( RY(I1,I2,I3)*SYR4(I1,I2,I3)     \
                        +SY(I1,I2,I3)*SYS4(I1,I2,I3) )

#define RXX43(I1,I2,I3) ( RX(I1,I2,I3)*RXR4(I1,I2,I3)     \
                         +SX(I1,I2,I3)*RXS4(I1,I2,I3)     \
                         +TX(I1,I2,I3)*RXT4(I1,I2,I3) )
#define RXY43(I1,I2,I3) ( RX(I1,I2,I3)*RYR4(I1,I2,I3)     \
                         +SX(I1,I2,I3)*RYS4(I1,I2,I3)     \
                         +TX(I1,I2,I3)*RYT4(I1,I2,I3) )
#define RXZ43(I1,I2,I3) ( RX(I1,I2,I3)*RZR4(I1,I2,I3)     \
                         +SX(I1,I2,I3)*RZS4(I1,I2,I3)     \
                         +TX(I1,I2,I3)*RZT4(I1,I2,I3) )
#define RYY43(I1,I2,I3) ( RY(I1,I2,I3)*RYR4(I1,I2,I3)     \
                         +SY(I1,I2,I3)*RYS4(I1,I2,I3)     \
                         +TY(I1,I2,I3)*RYT4(I1,I2,I3) )
#define RYZ43(I1,I2,I3) ( RY(I1,I2,I3)*RZR4(I1,I2,I3)     \
                         +SY(I1,I2,I3)*RZS4(I1,I2,I3)     \
                         +TY(I1,I2,I3)*RZT4(I1,I2,I3) )
#define RZZ43(I1,I2,I3) ( RZ(I1,I2,I3)*RZR4(I1,I2,I3)     \
                         +SZ(I1,I2,I3)*RZS4(I1,I2,I3)     \
                         +TZ(I1,I2,I3)*RZT4(I1,I2,I3) )
#define SXX43(I1,I2,I3) ( RX(I1,I2,I3)*SXR4(I1,I2,I3)     \
                         +SX(I1,I2,I3)*SXS4(I1,I2,I3)     \
                         +TX(I1,I2,I3)*SXT4(I1,I2,I3) )
#define SXY43(I1,I2,I3) ( RX(I1,I2,I3)*SYR4(I1,I2,I3)     \
                         +SX(I1,I2,I3)*SYS4(I1,I2,I3)     \
                         +TX(I1,I2,I3)*SYT4(I1,I2,I3) )
#define SXZ43(I1,I2,I3) ( RX(I1,I2,I3)*SZR4(I1,I2,I3)     \
                         +SX(I1,I2,I3)*SZS4(I1,I2,I3)     \
                         +TX(I1,I2,I3)*SZT4(I1,I2,I3) )
#define SYY43(I1,I2,I3) ( RY(I1,I2,I3)*SYR4(I1,I2,I3)     \
                         +SY(I1,I2,I3)*SYS4(I1,I2,I3)     \
                         +TY(I1,I2,I3)*SYT4(I1,I2,I3) )
#define SYZ43(I1,I2,I3) ( RY(I1,I2,I3)*SZR4(I1,I2,I3)     \
                         +SY(I1,I2,I3)*SZS4(I1,I2,I3)     \
                         +TY(I1,I2,I3)*SZT4(I1,I2,I3) )
#define SZZ43(I1,I2,I3) ( RZ(I1,I2,I3)*SZR4(I1,I2,I3)     \
                         +SZ(I1,I2,I3)*SZS4(I1,I2,I3)     \
                         +TZ(I1,I2,I3)*SZT4(I1,I2,I3) )
#define TXX43(I1,I2,I3) ( RX(I1,I2,I3)*TXR4(I1,I2,I3)     \
                         +SX(I1,I2,I3)*TXS4(I1,I2,I3)     \
                         +TX(I1,I2,I3)*TXT4(I1,I2,I3) )
#define TXY43(I1,I2,I3) ( RX(I1,I2,I3)*TYR4(I1,I2,I3)     \
                         +SX(I1,I2,I3)*TYS4(I1,I2,I3)     \
                         +TX(I1,I2,I3)*TYT4(I1,I2,I3) )
#define TXZ43(I1,I2,I3) ( RX(I1,I2,I3)*TZR4(I1,I2,I3)     \
                         +SX(I1,I2,I3)*TZS4(I1,I2,I3)     \
                         +TX(I1,I2,I3)*TZT4(I1,I2,I3) )
#define TYY43(I1,I2,I3) ( RY(I1,I2,I3)*TYR4(I1,I2,I3)     \
                         +SY(I1,I2,I3)*TYS4(I1,I2,I3)     \
                         +TY(I1,I2,I3)*TYT4(I1,I2,I3) )
#define TYZ43(I1,I2,I3) ( RY(I1,I2,I3)*TZR4(I1,I2,I3)     \
                         +SY(I1,I2,I3)*TZS4(I1,I2,I3)     \
                         +TY(I1,I2,I3)*TZT4(I1,I2,I3) )
#define TZZ43(I1,I2,I3) ( RZ(I1,I2,I3)*TZR4(I1,I2,I3)     \
                         +SZ(I1,I2,I3)*TZS4(I1,I2,I3)     \
                         +TZ(I1,I2,I3)*TZT4(I1,I2,I3) )

#define URR4(I1,I2,I3,KD)   (   \
       ( -30.*U(I1,I2,I3,KD)     \
        +16.*(U(I1+1,I2,I3,KD)+U(I1-1,I2,I3,KD))     \
            -(U(I1+2,I2,I3,KD)+U(I1-2,I2,I3,KD)) )*d24(axis1)  \
                            )
#define USS4(I1,I2,I3,KD)   (    \
      +( -30.*U(I1,I2,I3,KD)     \
        +16.*(U(I1,I2+1,I3,KD)+U(I1,I2-1,I3,KD))     \
            -(U(I1,I2+2,I3,KD)+U(I1,I2-2,I3,KD)) )*d24(axis2) \
                            )
#define URS4(I1,I2,I3,KD)   (   \
        (8.*(UR4(I1,I2+1,I3,KD)-UR4(I1,I2-1,I3,KD))     \
           -(UR4(I1,I2+2,I3,KD)-UR4(I1,I2-2,I3,KD)))*d14(axis2) \
                            )
#define UTT4(I1,I2,I3,KD)   (   \
       ( -30.*U(I1,I2,I3,KD)     \
        +16.*(U(I1,I2,I3+1,KD)+U(I1,I2,I3-1,KD))     \
            -(U(I1,I2,I3+2,KD)+U(I1,I2,I3-2,KD)) )*d24(axis3) \
                            )
#define URT4(I1,I2,I3,KD)   (   \
         (8.*(UR4(I1,I2,I3+1,KD)-UR4(I1,I2,I3-1,KD))     \
            -(UR4(I1,I2,I3+2,KD)-UR4(I1,I2,I3-2,KD)))*d14(axis3) \
                            )
#define UST4(I1,I2,I3,KD)   (   \
        (8.*(US4(I1,I2,I3+1,KD)-US4(I1,I2,I3-1,KD))     \
           -(US4(I1,I2,I3+2,KD)-US4(I1,I2,I3-2,KD)))*d14(axis3) \
                            )

#define UXX41(I1,I2,I3,KD) (SQR(RX(I1,I2,I3))*URR4(I1,I2,I3,KD))

#define UXX42(I1,I2,I3,KD)   (   \
       (SQR(RX(I1,I2,I3))                )*URR4(I1,I2,I3,KD)     \
      +2.*(RX(I1,I2,I3)*SX(I1,I2,I3)                           )     \
                                        *URS4(I1,I2,I3,KD)     \
      +(SQR(SX(I1,I2,I3))                )*USS4(I1,I2,I3,KD)     \
           +(RXX4(I1,I2,I3)              )*UR4(I1,I2,I3,KD)     \
           +(SXX4(I1,I2,I3)              )*US4(I1,I2,I3,KD)   \
                            )
#define UXX42A(I1,I2,I3,KD)   (   \
       (SQR(RX(I1,I2,I3))                )*URR4(I1,I2,I3,KD)     \
                            )
#define UXX42B(I1,I2,I3,KD)   (   \
      +2.*(RX(I1,I2,I3)*SX(I1,I2,I3)                           )     \
                                        *URS4(I1,I2,I3,KD)     \
                            )
#define UXX42C(I1,I2,I3,KD)   (   \
      +(SQR(SX(I1,I2,I3))                )*USS4(I1,I2,I3,KD)     \
                            )
#define UXX42D(I1,I2,I3,KD)   (   \
           +(RXX4(I1,I2,I3)              )*UR4(I1,I2,I3,KD)     \
                            )
#define UXX42E(I1,I2,I3,KD)   (   \
           +(SXX4(I1,I2,I3)              )*US4(I1,I2,I3,KD)   \
                            )



#define UYY42(I1,I2,I3,KD)   (   \
       (              SQR(RY(I1,I2,I3)))*URR4(I1,I2,I3,KD)     \
      +2.*(                           RY(I1,I2,I3)*SY(I1,I2,I3))     \
                                        *URS4(I1,I2,I3,KD)     \
      +(              SQR(SY(I1,I2,I3)))*USS4(I1,I2,I3,KD)     \
           +(              RYY4(I1,I2,I3))*UR4(I1,I2,I3,KD)     \
           +(              SYY4(I1,I2,I3))*US4(I1,I2,I3,KD)  \
                            )
#define UYY42A(I1,I2,I3,KD)   (   \
       (SQR(RY(I1,I2,I3))                )*URR4(I1,I2,I3,KD)     \
                            )
#define UYY42B(I1,I2,I3,KD)   (   \
      +2.*(RY(I1,I2,I3)*SY(I1,I2,I3)                           )     \
                                        *URS4(I1,I2,I3,KD)     \
                            )
#define UYY42C(I1,I2,I3,KD)   (   \
      +(SQR(SY(I1,I2,I3))                )*USS4(I1,I2,I3,KD)     \
                            )
#define UYY42D(I1,I2,I3,KD)   (   \
           +(RYY4(I1,I2,I3)              )*UR4(I1,I2,I3,KD)     \
                            )
#define UYY42E(I1,I2,I3,KD)   (   \
           +(SYY4(I1,I2,I3)              )*US4(I1,I2,I3,KD)   \
                            )

#define UXY42(I1,I2,I3,KD)   (   \
          RX(I1,I2,I3)*RY(I1,I2,I3)*URR4(I1,I2,I3,KD)     \
      +(RX(I1,I2,I3)*SY(I1,I2,I3)+RY(I1,I2,I3)*SX(I1,I2,I3))     \
                                   *URS4(I1,I2,I3,KD)     \
      +   SX(I1,I2,I3)*SY(I1,I2,I3)*USS4(I1,I2,I3,KD)     \
        +RXY4(I1,I2,I3)              *UR4(I1,I2,I3,KD)     \
        +SXY4(I1,I2,I3)              *US4(I1,I2,I3,KD)  \
                            )
#define UXX43(I1,I2,I3,KD)  (    \
       SQR(RX(I1,I2,I3)) *URR4(I1,I2,I3,KD)     \
      +SQR(SX(I1,I2,I3)) *USS4(I1,I2,I3,KD)     \
      +SQR(TX(I1,I2,I3)) *UTT4(I1,I2,I3,KD)     \
      +2.*RX(I1,I2,I3)*SX(I1,I2,I3)*URS4(I1,I2,I3,KD)     \
      +2.*RX(I1,I2,I3)*TX(I1,I2,I3)*URT4(I1,I2,I3,KD)     \
      +2.*SX(I1,I2,I3)*TX(I1,I2,I3)*UST4(I1,I2,I3,KD)     \
      +RXX43(I1,I2,I3)*UR4(I1,I2,I3,KD)     \
      +SXX43(I1,I2,I3)*US4(I1,I2,I3,KD)     \
      +TXX43(I1,I2,I3)*UT4(I1,I2,I3,KD)     \
                            )
#define UXX43A(I1,I2,I3,KD)  (    \
       SQR(RX(I1,I2,I3)) *URR4(I1,I2,I3,KD)     \
                            )
#define UXX43B(I1,I2,I3,KD)  (    \
      +SQR(SX(I1,I2,I3)) *USS4(I1,I2,I3,KD)     \
                            )
#define UXX43C(I1,I2,I3,KD)  (    \
      +SQR(TX(I1,I2,I3)) *UTT4(I1,I2,I3,KD)     \
                            )
#define UXX43D(I1,I2,I3,KD)  (    \
      +2.*RX(I1,I2,I3)*SX(I1,I2,I3)*URS4(I1,I2,I3,KD)     \
                            )
#define UXX43E(I1,I2,I3,KD)  (    \
      +2.*RX(I1,I2,I3)*TX(I1,I2,I3)*URT4(I1,I2,I3,KD)     \
                            )
#define UXX43F(I1,I2,I3,KD)  (    \
      +2.*SX(I1,I2,I3)*TX(I1,I2,I3)*UST4(I1,I2,I3,KD)     \
                            )
#define UXX43G(I1,I2,I3,KD)  (    \
      +RXX43(I1,I2,I3)*UR4(I1,I2,I3,KD)     \
                            )
#define UXX43H(I1,I2,I3,KD)  (    \
      +SXX43(I1,I2,I3)*US4(I1,I2,I3,KD)     \
                            )
#define UXX43I(I1,I2,I3,KD)  (    \
      +TXX43(I1,I2,I3)*UT4(I1,I2,I3,KD)     \
                            )

#define UYY43(I1,I2,I3,KD)  (    \
       SQR(RY(I1,I2,I3)) *URR4(I1,I2,I3,KD)     \
      +SQR(SY(I1,I2,I3)) *USS4(I1,I2,I3,KD)     \
      +SQR(TY(I1,I2,I3)) *UTT4(I1,I2,I3,KD)     \
      +2.*RY(I1,I2,I3)*SY(I1,I2,I3)*URS4(I1,I2,I3,KD)     \
      +2.*RY(I1,I2,I3)*TY(I1,I2,I3)*URT4(I1,I2,I3,KD)     \
      +2.*SY(I1,I2,I3)*TY(I1,I2,I3)*UST4(I1,I2,I3,KD)     \
      +RYY43(I1,I2,I3)*UR4(I1,I2,I3,KD)     \
      +SYY43(I1,I2,I3)*US4(I1,I2,I3,KD)     \
      +TYY43(I1,I2,I3)*UT4(I1,I2,I3,KD)  \
                            )

#define UYY43A(I1,I2,I3,KD)  (    \
       SQR(RY(I1,I2,I3)) *URR4(I1,I2,I3,KD)     \
                            )
#define UYY43B(I1,I2,I3,KD)  (    \
      +SQR(SY(I1,I2,I3)) *USS4(I1,I2,I3,KD)     \
                            )
#define UYY43C(I1,I2,I3,KD)  (    \
      +SQR(TY(I1,I2,I3)) *UTT4(I1,I2,I3,KD)     \
                            )
#define UYY43D(I1,I2,I3,KD)  (    \
      +2.*RY(I1,I2,I3)*SY(I1,I2,I3)*URS4(I1,I2,I3,KD)     \
                            )
#define UYY43E(I1,I2,I3,KD)  (    \
      +2.*RY(I1,I2,I3)*TY(I1,I2,I3)*URT4(I1,I2,I3,KD)     \
                            )
#define UYY43F(I1,I2,I3,KD)  (    \
      +2.*SY(I1,I2,I3)*TY(I1,I2,I3)*UST4(I1,I2,I3,KD)     \
                            )
#define UYY43G(I1,I2,I3,KD)  (    \
      +RYY43(I1,I2,I3)*UR4(I1,I2,I3,KD)     \
                            )
#define UYY43H(I1,I2,I3,KD)  (    \
      +SYY43(I1,I2,I3)*US4(I1,I2,I3,KD)     \
                            )
#define UYY43I(I1,I2,I3,KD)  (    \
      +TYY43(I1,I2,I3)*UT4(I1,I2,I3,KD)     \
                             )

#define UZZ43(I1,I2,I3,KD)  (    \
       SQR(RZ(I1,I2,I3)) *URR4(I1,I2,I3,KD)     \
      +SQR(SZ(I1,I2,I3)) *USS4(I1,I2,I3,KD)     \
      +SQR(TZ(I1,I2,I3)) *UTT4(I1,I2,I3,KD)     \
      +2.*RZ(I1,I2,I3)*SZ(I1,I2,I3)*URS4(I1,I2,I3,KD)     \
      +2.*RZ(I1,I2,I3)*TZ(I1,I2,I3)*URT4(I1,I2,I3,KD)     \
      +2.*SZ(I1,I2,I3)*TZ(I1,I2,I3)*UST4(I1,I2,I3,KD)     \
      +RZZ43(I1,I2,I3)*UR4(I1,I2,I3,KD)     \
      +SZZ43(I1,I2,I3)*US4(I1,I2,I3,KD)     \
      +TZZ43(I1,I2,I3)*UT4(I1,I2,I3,KD)     \
                            )

#define UZZ43A(I1,I2,I3,KD)  (    \
       SQR(RZ(I1,I2,I3)) *URR4(I1,I2,I3,KD)     \
                            )
#define UZZ43B(I1,I2,I3,KD)  (    \
      +SQR(SZ(I1,I2,I3)) *USS4(I1,I2,I3,KD)     \
                            )
#define UZZ43C(I1,I2,I3,KD)  (    \
      +SQR(TZ(I1,I2,I3)) *UTT4(I1,I2,I3,KD)     \
                            )
#define UZZ43D(I1,I2,I3,KD)  (    \
      +2.*RZ(I1,I2,I3)*SZ(I1,I2,I3)*URS4(I1,I2,I3,KD)     \
                            )
#define UZZ43E(I1,I2,I3,KD)  (    \
      +2.*RZ(I1,I2,I3)*TZ(I1,I2,I3)*URT4(I1,I2,I3,KD)     \
                            )
#define UZZ43F(I1,I2,I3,KD)  (    \
      +2.*SZ(I1,I2,I3)*TZ(I1,I2,I3)*UST4(I1,I2,I3,KD)     \
                            )
#define UZZ43G(I1,I2,I3,KD)  (    \
      +RZZ43(I1,I2,I3)*UR4(I1,I2,I3,KD)     \
                            )
#define UZZ43H(I1,I2,I3,KD)  (    \
      +SZZ43(I1,I2,I3)*US4(I1,I2,I3,KD)     \
                            )
#define UZZ43I(I1,I2,I3,KD)  (    \
      +TZZ43(I1,I2,I3)*UT4(I1,I2,I3,KD)     \
                             )


#define UXY43(I1,I2,I3,KD)  (    \
        RX(I1,I2,I3)*RY(I1,I2,I3)*URR4(I1,I2,I3,KD)     \
       +SX(I1,I2,I3)*SY(I1,I2,I3)*USS4(I1,I2,I3,KD)     \
       +TX(I1,I2,I3)*TY(I1,I2,I3)*UTT4(I1,I2,I3,KD)     \
       +(RX(I1,I2,I3)*SY(I1,I2,I3)+RY(I1,I2,I3)*SX(I1,I2,I3))     \
                                        *URS4(I1,I2,I3,KD)     \
       +(RX(I1,I2,I3)*TY(I1,I2,I3)+RY(I1,I2,I3)*TX(I1,I2,I3))     \
                                        *URT4(I1,I2,I3,KD)     \
       +(SX(I1,I2,I3)*TY(I1,I2,I3)+SY(I1,I2,I3)*TX(I1,I2,I3))     \
                                        *UST4(I1,I2,I3,KD)     \
       +RXY43(I1,I2,I3)*UR4(I1,I2,I3,KD)     \
       +SXY43(I1,I2,I3)*US4(I1,I2,I3,KD)     \
       +TXY43(I1,I2,I3)*UT4(I1,I2,I3,KD)     \
                            )
#define UXZ43(I1,I2,I3,KD)  (    \
        RX(I1,I2,I3)*RZ(I1,I2,I3)*URR4(I1,I2,I3,KD)     \
       +SX(I1,I2,I3)*SZ(I1,I2,I3)*USS4(I1,I2,I3,KD)     \
       +TX(I1,I2,I3)*TZ(I1,I2,I3)*UTT4(I1,I2,I3,KD)     \
       +(RX(I1,I2,I3)*SZ(I1,I2,I3)+RZ(I1,I2,I3)*SX(I1,I2,I3))     \
                                        *URS4(I1,I2,I3,KD)     \
       +(RX(I1,I2,I3)*TZ(I1,I2,I3)+RZ(I1,I2,I3)*TX(I1,I2,I3))     \
                                        *URT4(I1,I2,I3,KD)     \
       +(SX(I1,I2,I3)*TZ(I1,I2,I3)+SZ(I1,I2,I3)*TX(I1,I2,I3))     \
                                        *UST4(I1,I2,I3,KD)     \
       +RXZ43(I1,I2,I3)*UR4(I1,I2,I3,KD)     \
       +SXZ43(I1,I2,I3)*US4(I1,I2,I3,KD)     \
       +TXZ43(I1,I2,I3)*UT4(I1,I2,I3,KD)     \
                            )
#define UYZ43(I1,I2,I3,KD)  (    \
        RY(I1,I2,I3)*RZ(I1,I2,I3)*URR4(I1,I2,I3,KD)     \
       +SY(I1,I2,I3)*SZ(I1,I2,I3)*USS4(I1,I2,I3,KD)     \
       +TY(I1,I2,I3)*TZ(I1,I2,I3)*UTT4(I1,I2,I3,KD)     \
       +(RY(I1,I2,I3)*SZ(I1,I2,I3)+RZ(I1,I2,I3)*SY(I1,I2,I3))     \
                                        *URS4(I1,I2,I3,KD)     \
       +(RY(I1,I2,I3)*TZ(I1,I2,I3)+RZ(I1,I2,I3)*TY(I1,I2,I3))     \
                                        *URT4(I1,I2,I3,KD)     \
       +(SY(I1,I2,I3)*TZ(I1,I2,I3)+SZ(I1,I2,I3)*TY(I1,I2,I3))     \
                                        *UST4(I1,I2,I3,KD)     \
       +RYZ43(I1,I2,I3)*UR4(I1,I2,I3,KD)     \
       +SYZ43(I1,I2,I3)*US4(I1,I2,I3,KD)     \
       +TYZ43(I1,I2,I3)*UT4(I1,I2,I3,KD)     \
                            )
                                                                             
#define LAPLACIAN41(I1,I2,I3,KD)  UXX41(I1,I2,I3,KD)

#define LAPLACIAN42(I1,I2,I3,KD)  (                                \
      (SQR(RX(I1,I2,I3))+SQR(RY(I1,I2,I3)))*URR4(I1,I2,I3,KD)     \
     +2.*(RX(I1,I2,I3)*SX(I1,I2,I3)+ RY(I1,I2,I3)*SY(I1,I2,I3))   \
                                       *URS4(I1,I2,I3,KD)         \
     +(SQR(SX(I1,I2,I3))+SQR(SY(I1,I2,I3)))*USS4(I1,I2,I3,KD)     \
          +(RXX4(I1,I2,I3)+RYY4(I1,I2,I3))*UR4(I1,I2,I3,KD)       \
          +(SXX4(I1,I2,I3)+SYY4(I1,I2,I3))*US4(I1,I2,I3,KD)       \
                                 )
#define LAPLACIAN42A(I1,I2,I3,KD)  (                                \
      (SQR(RX(I1,I2,I3))+SQR(RY(I1,I2,I3)))*URR4(I1,I2,I3,KD)     \
                                 )
#define LAPLACIAN42B(I1,I2,I3,KD)  (                                \
     +2.*(RX(I1,I2,I3)*SX(I1,I2,I3)+ RY(I1,I2,I3)*SY(I1,I2,I3))   \
                                       *URS4(I1,I2,I3,KD)         \
                                 )
#define LAPLACIAN42C(I1,I2,I3,KD)  (                                \
     +(SQR(SX(I1,I2,I3))+SQR(SY(I1,I2,I3)))*USS4(I1,I2,I3,KD)     \
                                 )
#define LAPLACIAN42D(I1,I2,I3,KD)  (                                \
          +(RXX4(I1,I2,I3)+RYY4(I1,I2,I3))*UR4(I1,I2,I3,KD)       \
                                 )
#define LAPLACIAN42E(I1,I2,I3,KD)  (                                \
          +(SXX4(I1,I2,I3)+SYY4(I1,I2,I3))*US4(I1,I2,I3,KD)       \
                                 )
            
#define LAPLACIAN43(I1,I2,I3,KD)    (                             \
      (SQR(RX(I1,I2,I3))+SQR(RY(I1,I2,I3))+SQR(RZ(I1,I2,I3)))     \
                                       *URR4(I1,I2,I3,KD)         \
     +(SQR(SX(I1,I2,I3))+SQR(SY(I1,I2,I3))+SQR(SZ(I1,I2,I3)))     \
                                       *USS4(I1,I2,I3,KD)         \
     +(SQR(TX(I1,I2,I3))+SQR(TY(I1,I2,I3))+SQR(TZ(I1,I2,I3)))    \
                                       *UTT4(I1,I2,I3,KD)         \
     +2.*(RX(I1,I2,I3)*SX(I1,I2,I3)+ RY(I1,I2,I3)*SY(I1,I2,I3)    \
         +RZ(I1,I2,I3)*SZ(I1,I2,I3))   *URS4(I1,I2,I3,KD)         \
     +2.*(RX(I1,I2,I3)*TX(I1,I2,I3)+ RY(I1,I2,I3)*TY(I1,I2,I3)    \
         +RZ(I1,I2,I3)*TZ(I1,I2,I3))   *URT4(I1,I2,I3,KD)         \
     +2.*(SX(I1,I2,I3)*TX(I1,I2,I3)+ SY(I1,I2,I3)*TY(I1,I2,I3)    \
         +SZ(I1,I2,I3)*TZ(I1,I2,I3))   *UST4(I1,I2,I3,KD)         \
          +(RXX43(I1,I2,I3)+RYY43(I1,I2,I3)+RZZ43(I1,I2,I3))      \
                                       *UR4(I1,I2,I3,KD)          \
          +(SXX43(I1,I2,I3)+SYY43(I1,I2,I3)+SZZ43(I1,I2,I3))      \
                                       *US4(I1,I2,I3,KD)          \
          +(TXX43(I1,I2,I3)+TYY43(I1,I2,I3)+TZZ43(I1,I2,I3))      \
                                       *UT4(I1,I2,I3,KD)          \
                                  )                            
#define LAPLACIAN43A(I1,I2,I3,KD)    (                             \
      (SQR(RX(I1,I2,I3))+SQR(RY(I1,I2,I3))+SQR(RZ(I1,I2,I3)))     \
                                       *URR4(I1,I2,I3,KD)         \
                                  )                            
#define LAPLACIAN43B(I1,I2,I3,KD)    (                             \
     +(SQR(SX(I1,I2,I3))+SQR(SY(I1,I2,I3))+SQR(SZ(I1,I2,I3)))     \
                                       *USS4(I1,I2,I3,KD)         \
                                  )                            
#define LAPLACIAN43C(I1,I2,I3,KD)    (                             \
     +(SQR(TX(I1,I2,I3))+SQR(TY(I1,I2,I3))+SQR(TZ(I1,I2,I3)))    \
                                       *UTT4(I1,I2,I3,KD)         \
                                  )                            
#define LAPLACIAN43D(I1,I2,I3,KD)    (                             \
     +2.*(RX(I1,I2,I3)*SX(I1,I2,I3)+ RY(I1,I2,I3)*SY(I1,I2,I3)    \
         +RZ(I1,I2,I3)*SZ(I1,I2,I3))   *URS4(I1,I2,I3,KD)         \
                                  )                            
#define LAPLACIAN43E(I1,I2,I3,KD)    (                             \
     +2.*(RX(I1,I2,I3)*TX(I1,I2,I3)+ RY(I1,I2,I3)*TY(I1,I2,I3)    \
         +RZ(I1,I2,I3)*TZ(I1,I2,I3))   *URT4(I1,I2,I3,KD)         \
                                  )                            
#define LAPLACIAN43F(I1,I2,I3,KD)    (                             \
     +2.*(SX(I1,I2,I3)*TX(I1,I2,I3)+ SY(I1,I2,I3)*TY(I1,I2,I3)    \
         +SZ(I1,I2,I3)*TZ(I1,I2,I3))   *UST4(I1,I2,I3,KD)         \
                                  )                            
#define LAPLACIAN43G(I1,I2,I3,KD)    (                             \
          +(RXX43(I1,I2,I3)+RYY43(I1,I2,I3)+RZZ43(I1,I2,I3))      \
                                       *UR4(I1,I2,I3,KD)          \
                                  )                            
#define LAPLACIAN43H(I1,I2,I3,KD)    (                             \
          +(SXX43(I1,I2,I3)+SYY43(I1,I2,I3)+SZZ43(I1,I2,I3))      \
                                       *US4(I1,I2,I3,KD)          \
                                  )                            
#define LAPLACIAN43I(I1,I2,I3,KD)    (                             \
          +(TXX43(I1,I2,I3)+TYY43(I1,I2,I3)+TZZ43(I1,I2,I3))      \
                                       *UT4(I1,I2,I3,KD)          \
                                  )                            

// Define these even though they shouldn't be used
#define UY41(I1,I2,I3,KD)  0.
#define UZ41(I1,I2,I3,KD)  0.
#define UXY41(I1,I2,I3,KD) 0.
#define UXZ41(I1,I2,I3,KD) 0.
#define UYY41(I1,I2,I3,KD) 0.
#define UYZ41(I1,I2,I3,KD) 0.
#define UZZ41(I1,I2,I3,KD) 0.

#define UZ42(I1,I2,I3,KD)  0.
#define UXZ42(I1,I2,I3,KD) 0.
#define UYZ42(I1,I2,I3,KD) 0.
#define UZZ42(I1,I2,I3,KD) 0.

#define UZZ42A(I1,I2,I3,KD) 0.
#define UZZ42B(I1,I2,I3,KD) 0.
#define UZZ42C(I1,I2,I3,KD) 0.
#define UZZ42D(I1,I2,I3,KD) 0.
#define UZZ42E(I1,I2,I3,KD) 0.

//============================================================================================
// Define derivatives for a rectangular grid
//
// These definitions assume that the follwoing values are defined:
//    h41(axis) = 1./(12.*deltaX(axis))    : 41=4th order, first derivative
//    h42(axis) = 1./(12.*deltaX(axis)^2)
//============================================================================================

// 42 means 4th order, 2D:

#define UX42R(I1,I2,I3,KD) ( (8.*(U(I1+1,I2,I3,KD)-U(I1-1,I2,I3,KD))  \
                                -(U(I1+2,I2,I3,KD)-U(I1-2,I2,I3,KD)))*h41(axis1) )
#define UY42R(I1,I2,I3,KD) ( (8.*(U(I1,I2+1,I3,KD)-U(I1,I2-1,I3,KD))  \
                                -(U(I1,I2+2,I3,KD)-U(I1,I2-2,I3,KD)))*h41(axis2) )
#define UZ42R(I1,I2,I3,KD) ( (8.*(U(I1,I2,I3+1,KD)-U(I1,I2,I3-1,KD)) \
                                -(U(I1,I2,I3+2,KD)-U(I1,I2,I3-2,KD)))*h41(axis3) )

#define UXX42R(I1,I2,I3,KD) ( \
       ( -30.*U(I1,I2,I3,KD)     \
        +16.*(U(I1+1,I2,I3,KD)+U(I1-1,I2,I3,KD))     \
            -(U(I1+2,I2,I3,KD)+U(I1-2,I2,I3,KD)) )*h42(axis1)  \
                            )
#define UYY42R(I1,I2,I3,KD) ( \
       ( -30.*U(I1,I2,I3,KD)     \
        +16.*(U(I1,I2+1,I3,KD)+U(I1,I2-1,I3,KD))     \
            -(U(I1,I2+2,I3,KD)+U(I1,I2-2,I3,KD)) )*h42(axis2) \
                            )
#define UZZ42R(I1,I2,I3,KD) ( \
       ( -30.*U(I1,I2,I3,KD)     \
        +16.*(U(I1,I2,I3+1,KD)+U(I1,I2,I3-1,KD))     \
            -(U(I1,I2,I3+2,KD)+U(I1,I2,I3-2,KD)) )*h42(axis3) \
                            )

#define UXY42R(I1,I2,I3,KD) ( \
   ( (U(I1+2,I2+2,I3,KD)-U(I1-2,I2+2,I3,KD)-U(I1+2,I2-2,I3,KD)+U(I1-2,I2-2,I3,KD)) \
 +8.*(U(I1-1,I2+2,I3,KD)-U(I1-1,I2-2,I3,KD)-U(I1+1,I2+2,I3,KD)+U(I1+1,I2-2,I3,KD)  \
     +U(I1+2,I2-1,I3,KD)-U(I1-2,I2-1,I3,KD)-U(I1+2,I2+1,I3,KD)+U(I1-2,I2+1,I3,KD)) \
+64.*(U(I1+1,I2+1,I3,KD)-U(I1-1,I2+1,I3,KD)-U(I1+1,I2-1,I3,KD)+U(I1-1,I2-1,I3,KD)) \
                     )*(h41(axis1)*h41(axis2)) )

#define UXZ42R(I1,I2,I3,KD) ( \
   ( (U(I1+2,I2,I3+2,KD)-U(I1-2,I2,I3+2,KD)-U(I1+2,I2,I3-2,KD)+U(I1-2,I2,I3-2,KD)) \
 +8.*(U(I1-1,I2,I3+2,KD)-U(I1-1,I2,I3-2,KD)-U(I1+1,I2,I3+2,KD)+U(I1+1,I2,I3-2,KD)  \
     +U(I1+2,I2,I3-1,KD)-U(I1-2,I2,I3-1,KD)-U(I1+2,I2,I3+1,KD)+U(I1-2,I2,I3+1,KD)) \
+64.*(U(I1+1,I2,I3+1,KD)-U(I1-1,I2,I3+1,KD)-U(I1+1,I2,I3-1,KD)+U(I1-1,I2,I3-1,KD)) \
                )*(h41(axis1)*h41(axis3)) )
                            
#define UYZ42R(I1,I2,I3,KD)  ( \
   ( (U(I1,I2+2,I3+2,KD)-U(I1,I2-2,I3+2,KD)-U(I1,I2+2,I3-2,KD)+U(I1,I2-2,I3-2,KD)) \
 +8.*(U(I1,I2-1,I3+2,KD)-U(I1,I2-1,I3-2,KD)-U(I1,I2+1,I3+2,KD)+U(I1,I2+1,I3-2,KD)  \
     +U(I1,I2+2,I3-1,KD)-U(I1,I2-2,I3-1,KD)-U(I1,I2+2,I3+1,KD)+U(I1,I2-2,I3+1,KD)) \
+64.*(U(I1,I2+1,I3+1,KD)-U(I1,I2-1,I3+1,KD)-U(I1,I2+1,I3-1,KD)+U(I1,I2-1,I3-1,KD)) \
                )*(h41(axis2)*h41(axis3)) )


#define LAPLACIAN42R(I1,I2,I3,KD)  ( UXX42R(I1,I2,I3,KD)+UYY42R(I1,I2,I3,KD) )


// Here are the versions for 3D, most are the same as 2D

#define UX43R(I1,I2,I3,KD) UX42R(I1,I2,I3,KD) 
#define UY43R(I1,I2,I3,KD) UY42R(I1,I2,I3,KD) 
#define UZ43R(I1,I2,I3,KD) UZ42R(I1,I2,I3,KD) 
			   
#define UXX43R(I1,I2,I3,KD) UXX42R(I1,I2,I3,KD)
#define UYY43R(I1,I2,I3,KD) UYY42R(I1,I2,I3,KD)
#define UZZ43R(I1,I2,I3,KD) UZZ42R(I1,I2,I3,KD)
			   
#define UXY43R(I1,I2,I3,KD) UXY42R(I1,I2,I3,KD)
#define UXZ43R(I1,I2,I3,KD) UXZ42R(I1,I2,I3,KD)
#define UYZ43R(I1,I2,I3,KD) UYZ42R(I1,I2,I3,KD)
                           
#define LAPLACIAN43R(I1,I2,I3,KD)  ( UXX42R(I1,I2,I3,KD)+UYY42R(I1,I2,I3,KD)+UZZ42R(I1,I2,I3,KD) )


// Here are the versions for 1D, most are the same as 2D

#define UX41R(I1,I2,I3,KD) UX42R(I1,I2,I3,KD) 
#define UY41R(I1,I2,I3,KD) UY42R(I1,I2,I3,KD) 
#define UZ41R(I1,I2,I3,KD) UZ42R(I1,I2,I3,KD) 
			   
#define UXX41R(I1,I2,I3,KD) UXX42R(I1,I2,I3,KD)
#define UYY41R(I1,I2,I3,KD) UYY42R(I1,I2,I3,KD)
#define UZZ41R(I1,I2,I3,KD) UZZ42R(I1,I2,I3,KD)
			   
#define UXY41R(I1,I2,I3,KD) UXY42R(I1,I2,I3,KD)
#define UXZ41R(I1,I2,I3,KD) UXZ42R(I1,I2,I3,KD)
#define UYZ41R(I1,I2,I3,KD) UYZ42R(I1,I2,I3,KD)
                           
#define LAPLACIAN41R(I1,I2,I3,KD)  UXX42R(I1,I2,I3,KD)

