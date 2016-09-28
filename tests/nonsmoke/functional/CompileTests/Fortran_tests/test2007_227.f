C  CVS $Revision: 1.1 $  created $Date: 2008/01/08 02:55:38 $
      SUBROUTINE TRANFT (LINCK, LINMC, LTRAN, LIN, LOUT, MAXTP,
     1                   LIWORK, LRWORK, LCWORK, I, R, C)
C*****precision > double
      IMPLICIT real (A-H, O-Z), INTEGER (I-N)
C*****END precision > double
C*****precision > single
C      IMPLICIT REAL (A-H, O-Z), INTEGER (I-N)
C*****END precision > single
C
      DIMENSION I(LIWORK), R(LRWORK)
      CHARACTER*16 C(LCWORK), PRVERS, PREC, FILVER, PRDATE
      LOGICAL KERR
      COMMON /MCVERS/ PRVERS, PREC, FILVER
      INTEGER CKLSCH
      EXTERNAL CKLSCH
      PARAMETER (MAXFIT=7, NO=4, NFDIM=165, NT=50)
C
      NRANGE = MAXTP - 1
      KERR = .FALSE.
      PRVERS = '3.15'
      FILVER='1.0'
      PRDATE = '00/02/01'
C*****precision > double
      PREC = 'DOUBLE'
C*****END precision > double
C*****precision > single
C      PREC = 'SINGLE'
C*****END precision > single
C
      WRITE (LOUT, '(/A, /1X,A, A, A, A, /A, /A, //)')
     1  ' TRANFIT: CHEMKIN-III Transport property fitting code,',
     2  PREC(1:CKLSCH(PREC)), ' PRECISION Vers. ',
     3  PRVERS(1:CKLSCH(PRVERS)+1), PRDATE,
     4  ' Copyright 1995, Sandia Corporation.',
     5' The U.S. Government retains a limited license in this software.'
C
      CALL CKLEN (LINCK, LOUT, LENICK, LENRCK, LENCCK, IFLAG)
C##ERROR_CONDITION##
      IF (IFLAG .GT. 0) KERR = .TRUE.
      IF (LIWORK .LT. LENICK) THEN
C##ERROR_CONDITION##
         WRITE (LOUT, *)
     1   'TRANFT ERROR: IWORK needs to be at least ', LENICK
         KERR = .TRUE.
      ENDIF
      IF (LRWORK .LT. LENRCK) THEN
C##ERROR_CONDITION##
         WRITE (LOUT, *)
     1   'TRANFT ERROR:  RWORK needs to be at least ', LENRCK
         KERR = .TRUE.
      ENDIF
      IF (LCWORK .LT. LENCCK) THEN
C##ERROR_CONDITION##
         WRITE (LOUT, *)
     1   'TRANFT ERROR:  CWORK needs to be at least ', LENCCK
         KERR =.TRUE.
      ENDIF
      IF (KERR) RETURN
      CALL CKINIT (LENICK, LENRCK, LENCCK, LINCK, LOUT, I,
     1             R, C, IFLAG)
C##ERROR_CONDITION##
      IF (IFLAG .GT. 0) RETURN
      CALL CKINDX (I, R, MM, KK, II, NFIT)
C     integer array space
      IICK   = 1
      INLIN  = IICK  + LENICK
      IKTDIF = INLIN + KK
      INTEMP = IKTDIF + KK
      ITOT   = INTEMP + KK - 1
C     real array space
      IRCK   = 1
      IEPS   = IRCK + LENRCK
      ISIG   = IEPS + KK
      IDIP   = ISIG + KK
      IPOL   = IDIP + KK
      IZROT  = IPOL + KK
      IWT    = IZROT + KK
      ICV    = IWT   + KK
      IFITWT = ICV   + KK
      IFITRS = IFITWT+ NT
      IALOGT = IFITRS+ NT
      IXLA   = IALOGT+ NT
      IXETA  = IXLA  + NT
      IXD    = IXETA + NT
      IFITWK = IXD   + NT
      ICOFLM = IFITWK + NFDIM
      ICOFET = ICOFLM + NO * KK
      ICOFD  = ICOFET + NO * KK
      ICOFTD = ICOFD  + NO * KK * KK
      ITEMP  = ICOFTD + NO * KK * 10
      ITHERM = ITEMP  + MAXTP * KK
      ITFIT  = ITHERM + MAXFIT * NRANGE * KK
      NTOT   = ITFIT + NT - 1
      ICCK = 1
      IKSYM = ICCK + LENCCK
      ICTOT = IKSYM + KK - 1
      IF (LIWORK .LT. ITOT) THEN
C##ERROR_CONDITION##
         WRITE (LOUT, *)
     1   'TRANFT ERROR: IWORK needs to be at least ', ITOT
         KERR = .TRUE.
      ENDIF
      IF (LRWORK .LT. NTOT) THEN
C##ERROR_CONDITION##
         WRITE (LOUT, *)
     1   'TRANFT ERROR:  RWORK needs to be at least ', NTOT
         KERR = .TRUE.
      ENDIF
      IF (LCWORK .LT. ICTOT) THEN
C##ERROR_CONDITION##
         WRITE (LOUT, *)
     1   'TRANFT ERROR:  CWORK needs to be at least ', ICTOT
         KERR =.TRUE.
      ENDIF
      IF (KERR) THEN
         CALL CKDONE (I(IICK), R(IRCK))
         RETURN
      ENDIF
C
      CALL MCFIT
     1(LINCK, LINMC, LTRAN, LIN, LOUT, KK, NFDIM, NO, NT, LENICK,
     2 LENRCK, LENCCK, MAXFIT, MAXTP, NRANGE, R(IEPS), R(ISIG),
     3 R(IDIP), R(IPOL), R(IZROT), I(INLIN), R(IWT), R(ICV),
     4 R(IFITWT), R(IFITRS), R(IALOGT), R(IXLA), R(IXETA), R(IXD),
     5 R(IFITWK), R(ICOFLM), R(ICOFET), R(ICOFD), R(ICOFTD), I(IICK),
     6 R(IRCK), C(ICCK), I(IKTDIF), I(INTEMP), R(ITEMP), R(ITHERM),
     7 R(ITFIT), C(IKSYM))
C
      CALL CKDONE (I(IICK), R(IRCK))
      RETURN
      END
C
      SUBROUTINE MCFIT
     1(LINCK, LINMC, LTRAN, LIN, LOUT, KDIM, NFDIM, NO, NT, LENICK,
     2 LENRCK, LENCCK, MAXFIT, MAXTP, NRANGE, EPS, SIG, DIP, POL,
     3 ZROT, NLIN, WT, CV, FITWT, FITRES, ALOGT, XLA, XETA, XD,
     4 FITWRK, COFLAM, COFETA, COFD, COFTD, ICKWRK, RCKWRK, CCKWRK,
     5 KTDIF, NTEMP, TEMP, THERM, TFIT, KSYM)
C*****precision > double
      IMPLICIT real (A-H, O-Z), INTEGER (I-N)
C*****END precision > double
C*****precision > single
C      IMPLICIT REAL (A-H, O-Z), INTEGER (I-N)
C*****END precision > single
C
      COMMON /MCVERS/ PRVERS, PREC, FILVER
      COMMON /MCCON/ PI, RU, PATM, BOLTZ, EPSIL, TLOW, DT, P, EMAXL,
     1                EMAXE, EMAXD, EMAXTD, FCON
      DIMENSION EPS(KDIM), SIG(KDIM), DIP(KDIM), POL(KDIM), ZROT(KDIM),
     1          NLIN(KDIM), WT(KDIM), CV(KDIM), FITWT(NT), FITRES(NT),
     2          ALOGT(NT), XLA(NT), XETA(NT), XD(NT), FITWRK(NFDIM),
     3          COFLAM(NO,KDIM), COFETA(NO,KDIM), COFD(NO,KDIM,KDIM),
     4          COFTD(NO,KDIM,10), ICKWRK(LENICK), RCKWRK(LENRCK),
     5          VALUE(6), KTDIF(KDIM), NTEMP(KDIM), TEMP(MAXTP,KDIM),
     6          THERM(MAXFIT, NRANGE, KDIM), TFIT(NT)
C
      CHARACTER*80 LINE
      CHARACTER*16 CCKWRK(LENCCK), KSYM(KDIM), FILVER, PRVERS, PREC
      LOGICAL IERR, KERR
      INTEGER CKFRCH, CKSLEN
      EXTERNAL CKFRCH, CKSLEN
C
      PARAMETER (NCHECK=1000)
      DATA KERR/.FALSE./
C
C*****precision > double
      PI = DACOS (-1.0D0)
C*****END precision > double
C*****precision > single
C      PI = ACOS (-1.0)
C*****END precision > single
C
C DQ (12/20/2007): Modified to be "5" instead of "05" (see test2007_224.f)
      DO 5 K = 1, KDIM
         NLIN(K) = NCHECK
    5 CONTINUE
C
      EMAXL = 0.0
      EMAXE = 0.0
      EMAXD = 0.0
      EMAXTD = 0.0
      CALL CKINIT (LENICK, LENRCK, LENCCK, LINCK, LOUT, ICKWRK,
     1             RCKWRK, CCKWRK, IFLAG)
C##ERROR_CONDITION##
      IF (IFLAG .GT. 0) RETURN
      CALL CKINDX (ICKWRK, RCKWRK, MM, KK, II, NFIT)
      CALL CKMXTP (ICKWRK, MXTP)
      IF (MXTP .GT. MAXTP) THEN
C##ERROR_CONDITION##
         WRITE (LOUT, 8299) MAXTP
         RETURN
      ENDIF
      IF (KK .GT. KDIM) THEN
C##ERROR_CONDITION##
         WRITE (LOUT, 8300) KDIM
         RETURN
      ENDIF
C
      CALL CKSYMS (CCKWRK, LOUT, KSYM, IERR)
C##ERROR_CONDITION##
      KERR = KERR.OR.IERR
      CALL CKWT   (ICKWRK, RCKWRK, WT)
      CALL CKRP   (ICKWRK, RCKWRK, RU, RUC, PATM)
      P = PATM
C
C       Value of Avrogadro's number is from 1986 CODATA
C       recommended values.
C        (6.0221367(36)E23 mol-1)
C
C*****precision > double
      BOLTZ = RU / 6.0221367D23
C*****END precision > double
C*****precision > single
C      BOLTZ = RU / 6.0221367E23
C*****END precision > single
      EPSIL  = 0.0
      FDTCGS = 1.0E-18
      FATCM  = 1.0E+08
      FCON = 0.5 * FDTCGS**2 * FATCM**3 / BOLTZ
C
      CALL CKATHM (MAXFIT, NRANGE, ICKWRK, RCKWRK, MAXTP,
     1             NTEMP, TEMP, THERM)
      TLOW  = TEMP(1,1)
      THIGH = TEMP(NTEMP(1),1)
      DO 10 K = 2, KK
         THIGH = MIN (THIGH, TEMP (NTEMP(K), K))
         TLOW  = MAX (TLOW,  TEMP (1,K))
   10 CONTINUE
      DT = (THIGH-TLOW) / (NT-1)
      TN = TLOW
      DO 15 N = 1, NT
C         TFIT(N) = TLOW + (N-1)*DT
C         ALOGT(N) = LOG(TFIT(N))
          TFIT(N) = TN
          ALOGT(N) = LOG(TN)
          TN = TN + DT
   15 CONTINUE
C
      WRITE (LOUT, 20) TLOW, THIGH
   20 FORMAT (/,' DATA HAS BEEN FIT OVER THE RANGE:  TLOW=',F9.2,
     1        ', THIGH=',F9.2)
C
C     Read the transport database
      WRITE (LOUT, 7020)
      LDAT = LTRAN
C
   50 CONTINUE
      READ (LDAT, '(A)', END=500) LINE
      CALL CKDTAB (LINE)
      ILEN = CKSLEN(LINE)
      IF (ILEN .GT. 0) THEN
         K1 = CKFRCH(LINE(1:ILEN))
         K2 = K1 + INDEX(LINE(K1:ILEN),' ') - 1
         CALL CKCOMP (LINE(K1:K2-1), KSYM, KK, KFIND)
C
         IF (KFIND .GT. 0) THEN
            CALL CKXNUM (LINE(K2:ILEN), 6, LOUT, NVAL, VALUE, IERR)
            KERR = KERR.OR.IERR
C##ERROR_CONDITION##
            IF (IERR) WRITE (LOUT, 8000)
            WRITE (LOUT, 7010) LINE
C
            NLIN(KFIND) = INT(VALUE(1))
            EPS(KFIND)  = VALUE(2)
            SIG(KFIND)  = VALUE(3)
            DIP(KFIND)  = VALUE(4)
            POL(KFIND)  = VALUE(5)
            ZROT(KFIND) = VALUE(6)
         ENDIF
      ENDIF
      GO TO 50
C
  500 CONTINUE
C
      IF (LDAT .EQ. LTRAN .AND. LIN .GT. 0) THEN
CEM - can't backspace standard in!
         IF (LIN .NE. 5) THEN
            READ (LIN,'(A)',END=550) LINE
            BACKSPACE(LIN)
         ENDIF
         LDAT = LIN
         WRITE (LOUT, 7021)
         GO TO 50
      ENDIF
C
  550 CONTINUE
C
      DO 600 K = 1, KK
         IF (NLIN(K) .EQ. NCHECK) THEN
            DO 750 J = K, KK
C##ERROR_CONDITION##
               IF (NLIN(J) .EQ. NCHECK) WRITE (LOUT, 8010) KSYM(J)
  750       CONTINUE
            KERR = .TRUE.
         ENDIF
  600 CONTINUE
C
      IF (.NOT. KERR) THEN
C
C        Fit the conductivities and viscosities
         CALL LAMFIT (KK, NT, NFDIM, TFIT, NO, LOUT, WT, SIG, EPS, DIP,
     1                ZROT, NLIN, ALOGT, FITRES, FITWT, FITWRK, XLA,
     2                XETA, CV, ICKWRK, RCKWRK, COFLAM, COFETA, IERR)
         KERR = KERR.OR.IERR
C
C        Fit the diffusion coefficients
         CALL DIFFIT (KK, NT, NFDIM, TFIT, NO, KDIM, LOUT, WT, SIG, EPS,
     1                DIP, POL, ALOGT, FITRES, FITWT, FITWRK, XD, COFD,
     2                IERR)
         KERR = KERR.OR.IERR
C
C        Fit the thermal diffusion ratios
         CALL THMFIT (KK, NT, NFDIM, TFIT, NO, KDIM, LOUT, WT, SIG, EPS,
     1                DIP, POL, FITRES, FITWT, FITWRK, XD, NLITE, KTDIF,
     2                COFTD, IERR)
         KERR = KERR.OR.IERR
C
C        Print the fits
         WRITE (LOUT, 7035)
     1   ' COEFFICIENTS FOR SPECIES CONDUCTIVITIES',EMAXL
         WRITE (LOUT, 8200)
         WRITE (LOUT, 8100) (KSYM(K), (COFLAM(N,K),N=1,NO), K=1,KK)
C
         WRITE (LOUT, 7035)
     1   ' COEFFICIENTS FOR SPECIES VISCOSITIES',EMAXE
         WRITE (LOUT, 8200)
         WRITE (LOUT, 8100) (KSYM(K), (COFETA(N,K),N=1,NO), K=1,KK)
C
         WRITE (LOUT, 7035)
     1   ' COEFFICIENTS FOR SPECIES DIFFUSION COEFFICIENTS',EMAXD
         DO 2300 J = 1, KK
            WRITE (LOUT, 8200)
            WRITE (LOUT, 8110)
     1            (KSYM(J), KSYM(K), (COFD(N,J,K),N=1,NO), K=1,J)
 2300    CONTINUE
C
         WRITE (LOUT, 7035)
     1   ' COEFFICIENTS FOR THERMAL DIFFUSION RATIOS',EMAXTD
         DO 2400 M = 1, NLITE
            K = KTDIF(M)
            WRITE (LOUT, 8200)
            WRITE (LOUT, 8110)
     1            (KSYM(K), KSYM(J), (COFTD(N,J,M),N=1,NO), J=1,KK)
 2400    CONTINUE
      ENDIF
C
      LENIMC = 4*KK + NLITE
      LENRMC = (19 + 2*NO + NO*NLITE)*KK + (15+NO)*KK**2
C*****linkfile (transport) > binary
C       CALL MCBIN
C*****END linkfile (transport) > binary
C*****linkfile (transport) > ascii
      CALL MCFORM
C*****END linkfile (transport) > ascii
     1     (LINMC, LOUT, KERR, NO, KDIM, KK, NLITE, LENIMC,
     2            LENRMC, PATM, WT, EPS, SIG, DIP, POL, ZROT, NLIN,
     3            COFLAM, COFETA, COFD, KTDIF, COFTD)
C
C Write out workspace requirements to output file
C
      WRITE (LOUT, '(/A,3(/A,I6))')
     1      ' WORKING SPACE REQUIREMENTS ARE',
     2      '    INTEGER:   ',LENIMC,
     3      '    REAL:      ',LENRMC
C
 7000 FORMAT (A)
 7010 FORMAT (1X,A)
 7020 FORMAT (///,' TRANSPORT PARAMETERS FROM DATA BASE:',/)
 7021 FORMAT (///,' TRANSPORT PARAMETERS FROM USER INPUT:',/)
 7035 FORMAT (///A/' MAXIMUM FITTING ERROR = ', 1PE12.3)
 8000 FORMAT (10X, 'ERROR IN CKXNUM READING FROM TRANSPORT DATA BASE')
 8010 FORMAT (10X, 'ERROR...TRANSPORT DATA NOT GIVEN FOR ',A10)
 8100 FORMAT (2X, A10, 4E12.3)
 8110 FORMAT (2X, A10, A10, 4E12.3)
 8200 FORMAT (1X, ' ')
 8299 FORMAT (10X, 'ERROR...THE TRANSPORT FITTING CODE IS DIMENSIONED
     1FOR ONLY', I3, ' THERMODYNAMIC PROPERTIES FIT TEMPERATURES')
 8300 FORMAT (10X, 'ERROR...THE TRANSPORT FITTING CODE IS DIMENSIONED
     1FOR ONLY', I3, ' SPECIES')
C
C     end of SUBROUTINE MCFIT
      RETURN
      END
C
      SUBROUTINE TRABS()
C
C     V.3.15 2000/02/01 F. Rupley
C     1. CALL CKDONE to close CHEMKIN
C     V.3.14 2000/01/09 F. Rupley
C     1. allow user input from unit LIN to supplement tran.dat.
C     V.3.13 1998/12/10 E. Meeks
C     1. For phase-one implentation of error handling, removal of
C        STOP statement in XERHLT requires modification of routines
C        that call XERMSG with Level=2.  Added KERR logical keyword
C        to call list of DP1VLU and PVALUE, to return
C        error condition after XERMSG is called.
C     2. Added definition for IERR returned in DPOLFT and POLFIT
C        such that IERR=5 when DP1VLU or PVALUE returns KERR=.TRUE.
C     3. Added KERR logical to call list of DPCOEF and PCOEF, to
C        return fatal-error condition from DP1VLU or PVALUE.
C     4. Added KERR logical to call list of LAMFIT, THMFIT, and
C        DIFFIT to propogate fatal error up to calling routine.
C     5. In MCFIT, set KERR to true if fatal error is returned from
C        LAMFIT, THMFIT, or DIFFIT.
C     V.3.12 1998/10/16 E. Meeks
C     1. In MCFORM and MCBIN, changed dimensioning of KTDIF to
C        KTDIF(KK) instead of KTDIF(NLITE), to be consistent
C        with other routines, and to avoid dimensioning to 0
C        when NLITE=0.
C     V.3.11 98/03/03 E. Meeks
C     1. Action#120: Rename subroutine MCABS to be TRABS to avoid
C        duplication of same-named routine in tranlib.f.
C     2. Action#121: Add printout of workspace requirements at
C        end of output file in MCFIT.
C     V.3.10 97/08/18 F. Rupley
C     1. Fix bug#076:  FUNCTION HORNER arguments (N, A, X, P)
C        should be (N, A, X).
C     V.3.9, 97/03/01 F. Rupley
C     1. isolate comments into SUBROUTINE MCABS
C     2. make new main "driver" program to set up arrays,
C        to satisfy Reaction Design requirement of providing object
C        files instead of source code - move OPENs to driver
C     V.3.8, 97/01/28 F. Rupley
C     1. need to CALL CKMXTP to get fit temperature dimension required.
C     V. 3.7, 96/05/24
C     1. initial sccs version
C     VERSION 3.6 (F. Rupley, May 2, 1996)
C     1.  declare KERR logical in MCBIN, MCFORM
C     VERSION 3.5 (Initial CHEMKIN-III version, January 1996)
C     1.  PROLOGUEs
C     2.  binary/ascii linkfile options
C     3.  separate linkfile version, code version
C     4.  temperature array TFIT for the fit temperatures
C         (instead of calculating locally in each fit subroutine)
C     5.  some SUBROUTINEs become FUNCTIONs
C     CHANGES FOR VERSION 3.4
C     1.  Restructured data in OMEG12
C     2.  Changed REAL*8 to real
C     3.  Changed POLFIT and PCOEF to call single and double precision
C         SLATEC subroutines POLFIT,DPOLFT and PCOEF,DPCOEF
C     4.  Change vms open statements
C     CHANGES FROM VERSION 1.3
C     1.  Change THIGH to 3500
C     2.  Change name to TRANFT to conform to ANSI standard
C     3.  Add "unix" and "snla" change blocks
C     CHANGES FROM VERSION 1.4
C     1.  modify OPEN statements for unix
C     CHANGES FOR VERSION 1.6
C     1.  Find THIGH and TLOW from species thermodynamic data
C     CHANGES FOR VERSION 1.9
C     1.  Change binary file to include version, precision, error
C         status, and required length of work arrays
C     2.  Allow user input from a file.
C     3.  Implement non-formatted transport data input
C     CHANGES FOR VERSION 3.0 (3/15/94 F. Rupley)
C     1.  DOS/PC compatibility effort includes adding file names to
C         OPEN statements, removing unused variables in CALL lists,
C         unusued but possibly initialized variables.
C     CHANGES FOR VERSION 3.1 (6/9/94 H. Moffat)
C     1.  Changed Avrog Number to 1986 CODATA recommendations.
C     2.  PI and variables based on PI
C         is now accurate up to DP machine prec.
C     CHANGES FOR VERSION 3.2 (8/10/94 H.K. Moffat)
C     1.  Got rid of LEXIST variable that was uninitialized.
C     2.  'tran.inp' is no longer openned - there is no need for it.
C     CHANGES FOR VERSION 3.3 (1/19/95 F. Rupley)
C     1.  Add integer error flag to CKINIT call list.
C     2.  Appended "polfit.f", as tranfit is the only code which
C         uses it.
C     CHANGES FOR VERSION 3.4 (2/27/95 F. Rupley)
C     1.  Change character index "(:" to "(1:"
C
      RETURN
      END
C                                                                      C
      SUBROUTINE MCBIN (LINMC, LOUT, KERR, NO, KDIM, KK, NLITE, LENIMC,
     1                  LENRMC, PATM, WT, EPS, SIG, DIP, POL, ZROT,
     2                  NLIN, COFLAM, COFETA, COFD, KTDIF, COFTD)
C
C  START PROLOGUE
C  MCBIN writes transport information into a binary linkfile.
C
C  END PROLOGUE
C
C*****precision > double
      IMPLICIT real (A-H, O-Z), INTEGER (I-N)
C*****END precision > double
C*****precision > single
C      IMPLICIT REAL (A-H, O-Z), INTEGER (I-N)
C*****END precision > single
C
C     Integer arrays
      DIMENSION NLIN(KK), KTDIF(KK)
C     Real arrays
      DIMENSION WT(KK), EPS(KK), SIG(KK), DIP(KK), POL(KK), ZROT(KK),
     1          COFLAM(NO,KK), COFETA(NO,KK), COFD(NO,KDIM,KK),
     2          COFTD(NO,KDIM,10)
C
      LOGICAL KERR
C     Version and Precision common blocks
      CHARACTER*16 PRVERS, PREC, FILVER
      COMMON /MCVERS/ PRVERS, PREC, FILVER
C
C     Linkfile version (should be 1.0)
      WRITE (LINMC, ERR=5003) FILVER

C     Tranfit program version
      WRITE (LINMC, ERR=5003) PRVERS
C
C     Precision
      Write (LINMC, ERR=5003) PREC
C
C     Whether tranfit successfully completed or not
      WRITE (LINMC, ERR=5001) KERR
C
      IF (KERR) THEN
C##ERROR_CONDITION##
         WRITE (LOUT, '(//A,/A)')
     1   ' WARNING...THERE IS AN ERROR IN THE TRANSPORT LINKFILE.'
         RETURN
      ENDIF
C
C     Work space sizes, fixed parameters
      WRITE (LINMC, ERR=5001) LENIMC, LENRMC, NO, KK, NLITE
      WRITE (LINMC, ERR=5002) PATM
C
      WRITE (LINMC, ERR=5002) (WT(K), K = 1, KK)
      WRITE (LINMC, ERR=5002) (EPS(K), K = 1, KK)
      WRITE (LINMC, ERR=5002) (SIG(K), K = 1, KK)
      WRITE (LINMC, ERR=5002) (DIP(K), K = 1, KK)
      WRITE (LINMC, ERR=5002) (POL(K), K = 1, KK)
      WRITE (LINMC, ERR=5002) (ZROT(K), K = 1, KK)
      WRITE (LINMC, ERR=5001) (NLIN(K), K = 1, KK)
      WRITE (LINMC, ERR=5002) ((COFLAM(N,K), N=1,NO), K=1,KK)
      WRITE (LINMC, ERR=5002) ((COFETA(N,K), N=1,NO), K=1,KK)
      WRITE (LINMC, ERR=5002) (((COFD(N,J,K), N=1,NO), J=1,KK), K=1,KK)
      WRITE (LINMC, ERR=5002) (KTDIF(N), N = 1, NLITE)
      WRITE (LINMC, ERR=5002)
     1   (((COFTD(N,J,L), N=1,NO), J=1,KK), L=1,NLITE)
      RETURN
C
C ERROR HANDLING
C
5001  CONTINUE
C##ERROR_CONDITION##
      WRITE (LOUT, '(///A)')
     $   'ERROR: Failure to write binary integer data to tran.bin'
      RETURN
5002  CONTINUE
C##ERROR_CONDITION##
      WRITE (LOUT, '(///A)')
     $   'ERROR: Failure to write binary real data to tran.bin'
      RETURN
5003  CONTINUE
C##ERROR_CONDITION##
      WRITE (LOUT, '(///A)')
     $   'ERROR: Failure to write binary character data to tran.bin'
      RETURN
C
      END
C
      SUBROUTINE MCFORM (LINMC, LOUT, KERR, NO, KDIM, KK, NLITE,
     1                  LENIMC, LENRMC, PATM, WT, EPS, SIG, DIP, POL,
     2                  ZROT, NLIN, COFLAM, COFETA, COFD, KTDIF,
     3                  COFTD)
C
C  START PROLOGUE
C  MCFORM writes transport information into a formatted linkfile.
C
C  END PROLOGUE
C
C*****precision > double
      IMPLICIT real (A-H, O-Z), INTEGER (I-N)
C*****END precision > double
C*****precision > single
C      IMPLICIT REAL (A-H, O-Z), INTEGER (I-N)
C*****END precision > single
C
      LOGICAL KERR
C     Integer arrays
      DIMENSION NLIN(KK), KTDIF(KK)
C     Real arrays
      DIMENSION WT(KK), EPS(KK), SIG(KK), DIP(KK), POL(KK), ZROT(KK),
     1          COFLAM(NO,KK), COFETA(NO,KK), COFD(NO,KDIM,KK),
     2          COFTD(NO,KDIM,10)
C
C     Version and Precision common blocks
      CHARACTER*16 PRVERS, PREC, FILVER, CFMT, IFMT, LFMT, RFMT
C            Write Out Linkfile in ascii format
C
C      Character data - A16
C      Integer data   - I12
C      Logical data   - L8
C      real*8 data    - 1PE24.16
C
      PARAMETER
     1(CFMT='(8A16)', IFMT='(10I12)', LFMT='(L8)', RFMT='(1P,5E24.16)')

      COMMON /MCVERS/ PRVERS, PREC, FILVER
C
C     Linkfile version (should be 1.0)
      WRITE (LINMC, CFMT, ERR=5003) FILVER

C     Tranfit program version
      WRITE (LINMC, CFMT, ERR=5003) PRVERS
C
C     Precision
      Write (LINMC, CFMT, ERR=5003) PREC
C
C     Whether tranfit successfully completed or not
      WRITE (LINMC, LFMT, ERR=5001) KERR
C
      IF (KERR) THEN
C##ERROR_CONDITION##
         WRITE (LOUT, '(//A,/A)')
     1   ' WARNING...THERE IS AN ERROR IN THE TRANSPORT LINKFILE.'
         RETURN
      ENDIF
C
C     Work space sizes, fixed parameters
      WRITE (LINMC, IFMT, ERR=5001) LENIMC, LENRMC, NO, KK, NLITE
      WRITE (LINMC, RFMT, ERR=5002) PATM
C
      WRITE (LINMC, RFMT, ERR=5002) (WT(K), K = 1, KK)
      WRITE (LINMC, RFMT, ERR=5002) (EPS(K), K = 1, KK)
      WRITE (LINMC, RFMT, ERR=5002) (SIG(K), K = 1, KK)
      WRITE (LINMC, RFMT, ERR=5002) (DIP(K), K = 1, KK)
      WRITE (LINMC, RFMT, ERR=5002) (POL(K), K = 1, KK)
      WRITE (LINMC, RFMT, ERR=5002) (ZROT(K), K = 1, KK)
      WRITE (LINMC, IFMT, ERR=5001) (NLIN(K), K = 1, KK)
      WRITE (LINMC, RFMT, ERR=5002) ((COFLAM(N,K), N=1,NO), K=1,KK)
      WRITE (LINMC, RFMT, ERR=5002) ((COFETA(N,K), N=1,NO), K=1,KK)
      WRITE (LINMC, RFMT, ERR=5002)
     1   (((COFD(N,J,K), N=1,NO), J=1,KK), K=1,KK)
      WRITE (LINMC, IFMT, ERR=5002) (KTDIF(N), N = 1, NLITE)
      WRITE (LINMC, RFMT, ERR=5002)
     1     (((COFTD(N,J,L), N=1,NO), J=1,KK), L=1,NLITE)
      RETURN
C
C ERROR HANDLING
C
5001  CONTINUE
C##ERROR_CONDITION##
      WRITE (LOUT, '(///A)')
     $   'ERROR: Failure to write ascii integer data to tran.asc'
      RETURN
5002  CONTINUE
C##ERROR_CONDITION##
      WRITE (LOUT, '(///A)')
     $   'ERROR: Failure to write ascii real data to tran.asc'
      RETURN
5003  CONTINUE
C##ERROR_CONDITION##
      WRITE (LOUT, '(///A)')
     $   'ERROR: Failure to write ascii character data to tran.asc'
      RETURN
C
      END
C                                                                     C
C---------------------------------------------------------------------C
C                                                                     C
      SUBROUTINE LAMFIT (KK, NT, NFDIM, TFIT, NO, LOUT, WT, SIG, EPS,
     1                   DIP, ZROT, NLIN, ALOGT, FITRES, FITWT, FITWRK,
     2                   XLA, XETA, CV, ICKWRK, RCKWRK, COFLAM, COFETA,
     3                   KERR)
C
C  START PROLOGUE
C  SUBROUTINE LAMFIT (KK, NT, NFDIM, TFIT, NO, LOUT, WT, SIG, EPS,
C                     DIP, ZROT, NLIN, ALOGT, FITRES, FITWT, FITWRK,
C                     XLA, XETA, CV, ICKWRK, RCKWRK, COFLAM, COFETA,
C                     KERR)
C
C  KK          -
C  NT          -
C  NFDIM       -
C  TFIT(*)     -
C  NO          -
C  LOUT        -
C  WT(*)       -
C  SIG(*)      -
C  EPS(*)      -
C  DIP(*)      -
C  ZROT(*)     -
C  NLIN(*)     -
C  ALOGT(*)    -
C  FITRES(*)   -
C  FITWRK(*)   -
C  XLA(*)      -
C  XETA(*)     -
C  CV(*)       -
C  ICKWRK(*)   -
C  RCKWRK(*)   -
C  COFLAM(*,*) -
C  COFETA(*,*) -
C         KERR -   logical flag set to true if fatal error occurred
C
C  END PROLOGUE
C
C*****precision > double
      IMPLICIT real (A-H, O-Z), INTEGER (I-N)
C*****END precision > double
C*****precision > single
C      IMPLICIT REAL (A-H, O-Z), INTEGER (I-N)
C*****END precision > single
C
      DIMENSION WT(KK), SIG(KK), EPS(KK), DIP(KK), ZROT(KK), NLIN(KK),
     1          CV(KK), FITRES(NT), FITWRK(NFDIM), ALOGT(NT), FITWT(NT),
     2          XLA(NT), XETA(NT), ICKWRK(*), RCKWRK(*), COFLAM(NO,KK),
     3          COFETA(NO,KK), TFIT(NT)
      LOGICAL KERR
      COMMON /MCCON/ PI, RU, PATM, BOLTZ, EPSIL, TLOW, DT, P, EMAXL,
     1                EMAXE, EMAXD, EMAXTD, FCON
C
      KERR = .FALSE.
      PFAC = PATM / P
      DO 1000 K = 1, KK
         DST = FCON * DIP(K)**2 / (EPS(K) * SIG(K)**3)
         HELPE = 2.6693E-5 * SQRT(WT(K)) / SIG(K)**2
         HELPD = 2.6280E-3 * PFAC / (SQRT(WT(K)) * SIG(K)**2)
C
         DO 300 N = 1, NT
            T = TFIT(N)
            CALL CKCVML (T, ICKWRK, RCKWRK, CV)
            TR = T / EPS(K)
            DII = SQRT(T**3)*HELPD / OMEG12(1,TR,DST)
            XETA(N) = SQRT(T)*HELPE / OMEG12(2,TR,DST)
            PRUT = P / (RU * T)
            RODET = DII * WT(K) * PRUT / XETA(N)
            AA = 2.5 - RODET
            IF (NLIN(K) .EQ. 2) THEN
               BB = ZROT(K)*PARKER(T,EPS(K)) + 2.0*(5.0/2.0 + RODET)/PI
               CROCT = 1.0
            ELSE
               BB = ZROT(K)*PARKER(T,EPS(K)) + 2.0*(5.0/3.0 + RODET)/PI
               CROCT = 2.0/3.0
            ENDIF
            FTRA = 2.5 * (1.0 - 2.0 * CROCT * AA / (BB*PI))
            FROT = RODET * (1.0 + 2.0 * AA / (BB*PI))
            FVIB = RODET
            IF (NLIN(K) .EQ. 0) THEN
               FAKTOR = 5.0/2.0 * 1.5*RU
            ELSE IF (NLIN(K) .EQ. 1) THEN
               FAKTOR = (FTRA*1.5 + FROT)*RU + FVIB*(CV(K)-2.5*RU)
            ELSE IF (NLIN(K) .EQ. 2) THEN
               FAKTOR = (FTRA+FROT)*1.5*RU + FVIB*(CV(K)-3.0*RU)
            ENDIF
            XLA(N)  = LOG( XETA(N)/WT(K) * FAKTOR)
            XETA(N) = LOG(XETA(N))
300      CONTINUE
C
C
C        Fit conductivity
         FITWT(1) = -1.0
         EPSL = EPSIL
C*****precision > double
         CALL DPOLFT
C*****END precision > double
C*****precision > single
C         CALL POLFIT
C*****END precision > single
C
     1   (NT, ALOGT, XLA, FITWT, NO-1, NORD, EPSL, FITRES, IERR, FITWRK)
         IF (IERR .NE. 1) THEN
C##ERROR_CONDITION##
            KERR = .TRUE.
            WRITE (LOUT,7000)
            RETURN
         ENDIF
C
         CCC = 0.0
C*****precision > double
         CALL DPCOEF (NORD, CCC, COFLAM(1,K), FITWRK, KERR)
C*****END precision > double
C*****precision > single
C         CALL PCOEF (NORD, CCC, COFLAM(1,K), FITWRK, KERR)
C*****END precision > single
         IF (KERR) THEN
C##ERROR_CONDITION##
            RETURN
         ENDIF
C
C
C        Fit viscosity
         FITWT(1) = -1.0
         EMAXL = MAX (EMAXL, EPSL)
         EPSL = EPSIL
C*****precision > double
         CALL DPOLFT
C*****END precision > double
C*****precision > single
C         CALL POLFIT
C*****END precision > single
C
     1 (NT, ALOGT, XETA, FITWT, NO-1, NORD, EPSL, FITRES, IERR, FITWRK)
         IF (IERR .NE. 1) THEN
C##ERROR_CONDITION##
            KERR = .TRUE.
            WRITE (LOUT,7000)
            RETURN
         ENDIF
C
         EMAXE = MAX (EMAXE, EPSL)
         CCC = 0.0
C*****precision > double
         CALL DPCOEF (NORD, CCC, COFETA(1,K), FITWRK, KERR)
C*****END precision > double
C*****precision > single
C         CALL PCOEF (NORD, CCC, COFETA(1,K), FITWRK, KERR)
C*****END precision > single
         IF (KERR) THEN
C##ERROR_CONDITION##
            RETURN
         ENDIF
C
1000  CONTINUE
C
7000  FORMAT(9X,'ERROR IN POLFIT WHILE FITTING VISCOSITY OR ',
     1'CONDUCTIVITY')
C
C     end of SUBROUTINE MCBIN
      RETURN
      END
C                                                                     C
C---------------------------------------------------------------------C
C                                                                     C
      SUBROUTINE DIFFIT (KK, NT, NFDIM, TFIT, NO, KDIM, LOUT, WT, SIG,
     1                   EPS, DIP, POL, ALOGT, FITRES, FITWT, FITWRK,
     2                   XD, COFD, KERR)
C
C  START PROLOGUE
C  SUBROUTINE DIFFIT (KK, NT, NFDIM, TFIT, NO, KDIM, LOUT, WT, SIG,
C                     EPS, DIP, POL, ALOGT, FITRES, FITWT, FITWRK,
C                     XD, COFD, KERR)
C
C  KK        -
C  NT        -
C  NFDIM     -
C  TFIT(*)   -
C  NO        -
C  KDIM      -
C  LOUT      -
C  WT(*)     -
C  SIG(*)    -
C  EPS(*)    -
C  DIP(*)    -
C  POL(*)    -
C  ALOGT(*)  -
C  FITRES(*) -
C  FITWT(*)  -
C  FITWRK(*) -
C  XD(*)     -
C  COFD(*,*) -
C         KERR -   logical flag set to true if fatal error occurred
C
C  END PROLOGUE
C
C*****precision > double
      IMPLICIT real (A-H, O-Z), INTEGER (I-N)
C*****END precision > double
C*****precision > single
C      IMPLICIT REAL (A-H, O-Z), INTEGER (I-N)
C*****END precision > single
C
      DIMENSION WT(KK), SIG(KK), EPS(KK), DIP(KK), POL(KK),
     1          FITRES(NT), FITWT(NT), FITWRK(NFDIM), ALOGT(NT),
     2          XD(NT), COFD(NO,KDIM,KK), TFIT(NT)
      LOGICAL KERR
      COMMON /MCCON/ PI, RU, PATM, BOLTZ, EPSIL, TLOW, DT, P, EMAXL,
     1                EMAXE, EMAXD, EMAXTD, FCON
C
      KERR = .FALSE.
      DIPMIN = 1.0E-20
C
      DO 1000 J = 1, KK
         DO 999 K = 1, J
            SIGM = 0.5 * (SIG(J)+SIG(K))
            EPSM = SQRT(EPS(J)*EPS(K))
            IF (DIP(J).LT.DIPMIN .AND. DIP(K).GT.DIPMIN) THEN
C              K is polar, J is nonpolar
C
               DST = 0.
               XI = 1.0 +
     1              POL(J) * FCON * DIP(K)**2 * SQRT(EPS(K)/EPS(J)) /
     2              ( 2.0 * SIG(J)**3 * EPS(K) * SIG(K)**3)
C
            ELSEIF (DIP(J).GT.DIPMIN .AND. DIP(K).LT.DIPMIN) THEN
C              J is polar, K is nonpolar
C
               DST = 0.
               XI = 1.0 +
     1              POL(K) * FCON * DIP(J)**2 * SQRT(EPS(J)/EPS(K)) /
     2              (2.0 * SIG(K)**3 * EPS(J) * SIG(J)**3)
C
C
            ELSE
C              normal case, either both polar or both nonpolar
C
               DST = FCON * DIP(J) * DIP(K) / (EPSM * SIGM**3)
               XI = 1.0
            ENDIF
C
            SIGM = SIGM * XI**(-1.0/6.0)
            EPSM = EPSM * XI**2
            HELP1 = (WT(J)+WT(K)) / (2.0*WT(J)*WT(K))
            DO 500 N = 1, NT
               T = TFIT(N)
               TR = T / EPSM
               HELP2 = 0.002628 * SQRT(HELP1*T**3)
               XD(N) = HELP2*PATM / (OMEG12(1,TR,DST) * SIGM**2 * P)
               XD(N) = XD(N) *
     1            D12(WT(J),WT(K),T,EPS(J),EPS(K),SIG(J),SIG(K),DST)
               XD(N) = LOG(XD(N))
500         CONTINUE
C
            FITWT(1) = -1.0
            EPSL = EPSIL
C
C*****precision > double
            CALL DPOLFT
C*****END precision > double
C*****precision > single
C            CALL POLFIT
C*****END precision > single
     1 (NT, ALOGT, XD, FITWT, NO-1, NORD, EPSL, FITRES, IERR, FITWRK)
C
            IF (IERR .NE. 1) THEN
C##ERROR_CONDITION##
               KERR = .TRUE.
               WRITE (LOUT,7000) J,K
               RETURN
            ENDIF
C
            CCC = 0.0
            EMAXD = MAX (EMAXD, EPSL)
C
C*****precision > double
            CALL DPCOEF (NORD, CCC, COFD(1,K,J), FITWRK, KERR)
C*****END precision > double
C*****precision > single
C            CALL PCOEF (NORD, CCC, COFD(1,K,J), FITWRK, KERR)
C*****END precision > single
            IF (KERR) THEN
C##ERROR_CONDITION##
               RETURN
            ENDIF
 999     CONTINUE
1000  CONTINUE
C
C     Fill out the lower triangle
      DO 2000 K = 1, KK-1
         DO 1999 J = K+1, KK
            DO 1998 N = 1, NO
               COFD(N,J,K) = COFD(N,K,J)
 1998       CONTINUE
 1999    CONTINUE
 2000 CONTINUE
C
7000  FORMAT (10X, 'ERROR IN FITTING', I3, ',', I3,
     1                              'DIFFUSION COEFFICIENT')
C
C     end of SUBROUTINE DIFFIT
      RETURN
      END
C                                                                     C
C---------------------------------------------------------------------C
C                                                                     C
      SUBROUTINE THMFIT (KK, NT, NFDIM, TFIT, NO, KDIM, LOUT, WT, SIG,
     1                   EPS, DIP, POL, FITRES, FITWT, FITWRK, TD,
     2                   NLITE, KTDIF, COFTD, KERR)
C
C  START PROLOGUE
C  SUBROUTINE THMFIT (KK, NT, NFDIM, TFIT, NO, KDIM, LOUT, WT, SIG,
C                     EPS, DIP, POL, FITRES, FITWT, FITWRK, TD,
C                     NLITE, KTDIF, COFTD, KERR)
C
C  KK         -
C  NT         -
C  NFDIM      -
C  TFIT(*)    -
C  NO         -
C  KDIM       -
C  LOUT       -
C  WT(*)      -
C  SIG(*)     -
C  EPS(*)     -
C  DIP(*)     -
C  POL(*)     -
C  FITRES(*)  -
C  FITWT(*)   -
C  FITWRK(*)  -
C  TD(*)      -
C  NLITE      -
C  KTDIF(*)   -
C  COFTD(*,*,*)-
C         KERR -   logical flag set to true if fatal error occurred
C
C  END PROLOGUE
C
C*****precision > double
      IMPLICIT real (A-H, O-Z), INTEGER (I-N)
C*****END precision > double
C*****precision > single
C      IMPLICIT REAL (A-H, O-Z), INTEGER (I-N)
C*****END precision > single
C
      COMMON /MCCON/ PI, RU, PATM, BOLTZ, EPSIL, TLOW, DT, P, EMAXL,
     1                EMAXE, EMAXD, EMAXTD, FCON
      DIMENSION WT(KK), SIG(KK), EPS(KK), DIP(KK), POL(KK),
     1          KTDIF(KK), COFTD(NO,KDIM,10), FITRES(NT),
     2          FITWT(NT), FITWRK(NFDIM), TD(NT),
     3          FITAST(7), FITBST(7), FITCST(7), TFIT(*)
      LOGICAL KERR
      EXTERNAL HORNER
C
      DATA FITAST/ .1106910525E+01, -.7065517161E-02,-.1671975393E-01,
     1             .1188708609E-01,  .7569367323E-03,-.1313998345E-02,
     2             .1720853282E-03/
C
      DATA FITBST/ .1199673577E+01, -.1140928763E+00,-.2147636665E-02,
     1             .2512965407E-01, -.3030372973E-02,-.1445009039E-02,
     2             .2492954809E-03/
C
      DATA FITCST/ .8386993788E+00,  .4748325276E-01, .3250097527E-01,
     1            -.1625859588E-01, -.2260153363E-02, .1844922811E-02,
     2            -.2115417788E-03/
C
      KERR = .FALSE.
      NLITE = 0
C
      DO 1100 J = 1, KK
         IF (WT(J) .GT. 5.0) GO TO 1100
C
         NLITE = NLITE + 1
         KTDIF(NLITE) = J
         EPSJ = EPS(J) * BOLTZ
         SIGJ = SIG(J) * 1.0E-8
         POLJ = POL(J) * 1.0E-24
         POLJST = POLJ / SIGJ**3
C
         DO 1000 K = 1, KK
            EPSK = EPS(K) * BOLTZ
            SIGK = SIG(K) * 1.0E-8
            DIPK = DIP(K) * 1.0E-18
            DIPKST = DIPK / SQRT(EPSK*SIGK**3)
            EKOEJ = EPSK / EPSJ
            TSE = 1.0 + 0.25*POLJST*DIPKST**2*SQRT(EKOEJ)
            EOK = TSE**2 * SQRT(EPS(J)*EPS(K))
            WTKJ= (WT(K)-WT(J)) / (WT(K)+WT(J))
C
            EQLOG = LOG(EOK)
            DO 500 N = 1, NT
C               TSLOG = LOG(TFIT(N) / EOK)
               TSLOG = LOG(TFIT(N)) - EQLOG
               ASTAR = HORNER (7, FITAST, TSLOG)
               BSTAR = HORNER (7, FITBST, TSLOG)
               CSTAR = HORNER (7, FITCST, TSLOG)
               TD(N) = 7.5 * WTKJ * (2.0*ASTAR + 5.0) *
     1                 (6.0*CSTAR - 5.0)/
     2            (ASTAR * (16.0*ASTAR - 12.0*BSTAR + 55.0))
  500       CONTINUE
C
            FITWT(1) = -1.0
            EPSL = EPSIL
C
C*****precision > double
            CALL DPOLFT
C*****END precision > double
C*****precision > single
C            CALL POLFIT
C*****END precision > single
     1   (NT, TFIT, TD, FITWT, NO-1, NORD, EPSL, FITRES, IERR, FITWRK)
C
            IF (IERR .NE. 1) THEN
C##ERROR_CONDITION##
               KERR = .TRUE.
               WRITE (LOUT, 7000) J,K
               RETURN
            ENDIF
C
            EMAXTD = MAX (EMAXTD, EPSL)
            CCC = 0.0
C
C*****precision > double
            CALL DPCOEF (NORD, CCC, COFTD(1,K,NLITE), FITWRK, KERR)
C*****END precision > double
C*****precision > single
C            CALL PCOEF (NORD, CCC, COFTD(1,K,NLITE), FITWRK, KERR)
C*****END precision > single
            IF (KERR) THEN
C##ERROR_CONDITION##
               RETURN
            ENDIF
C
 1000    CONTINUE
 1100 CONTINUE
C
 7000 FORMAT (10X, 'ERROR IN FITTING THE ', I3, ',', I3,
     1                ' THERMAL DIFFUSION RATIO')
C
C     end of SUBROUTINE THMFIT
      RETURN
      END
C                                                                     C
C---------------------------------------------------------------------C
C                                                                     C
C*****precision > double
      real FUNCTION HORNER (N, A, X)
      IMPLICIT real (A-H, O-Z), INTEGER (I-N)
C*****END precision > double
C*****precision > single
C      REAL FUNCTION HORNER (N, A, X)
C      IMPLICIT REAL (A-H, O-Z), INTEGER (I-N)
C*****END precision > single
C
      DIMENSION A(*)
      NM1 = N-1
      B = A(N)
      DO 10 I = 1, NM1
         B = A(N-I) + B*X
   10 CONTINUE
      HORNER = B
C
C     end of FUNCTION HORNER
      RETURN
      END
C
C*****precision > double
      real FUNCTION D12 (W1,W2,T,EPS1,EPS2,SIG1,SIG2,DST)
      IMPLICIT real (A-H, O-Z), INTEGER (I-N)
C*****END precision > double
C*****precision > single
C      REAL FUNCTION D12 (W1, W2, T, EPS1, EPS2, SIG1, SIG2, DST)
C      IMPLICIT REAL (A-H, O-Z), INTEGER (I-N)
C*****END precision > single
C
C        CORRECTION OF D(1,2).
C         REFERENCE: MARRERO AND MASON,J.PHYS CHEM REF DAT.1,3(1972)
C
      SUMW = W1+W2
      SIG12 = 0.5 * (SIG1 + SIG2)
      TR11 = T / EPS1
      TR22 = T / EPS2
      TR12 = T / SQRT(EPS1*EPS2)
C
C//// CALCULATION OF THE REDUCED COLLISION INTEGRALS /////////////////
C
      OM2F11 = OMEG12 (2, TR11, DST)
      OM2F22 = OMEG12 (2, TR22, DST)
      OM1F12 = OMEG12 (1, TR12, DST)
      OM2F12 = OMEG12 (2, TR12, DST)
C
      IF (TR12 .LE. 5.0) THEN
         BST12 = 1.36 - 0.223*TR12 + 0.0613*TR12**2 -0.00554*TR12**3
         CST12 =0.823 + 0.0128*TR12 + 0.0112*TR12**2 -0.00193*TR12**3
      ELSE
         BST12 = 1.095
         CST12 = 0.9483
      ENDIF
      AST12 = OM2F12 / OM1F12
C
      H = (SIG1/SIG2)**2 * SQRT(2.0*W2/SUMW) * 2.0*W1**2/(SUMW*W2)
      P1 = H * OM2F11/OM1F12
C
      H = (SIG2/SIG1)**2 * SQRT(2.0*W1/SUMW) * 2.0*W2**2/(SUMW*W1)
      P2 = H * OM2F22/OM1F12
C
      P12 = 15.0 * ((W1-W2)/SUMW)**2 + 8.0*W1*W2*AST12/SUMW**2
C
      H = 2.0/(W2*SUMW) * SQRT(2.0*W2/SUMW) * OM2F11/OM1F12 *
     1    (SIG1/SIG12)**2
      Q1 = ((2.5-1.2*BST12)*W1**2 +
     1                           3.0*W2**2 + 1.6*W1*W2*AST12)*H
C
      H = 2.0/(W1*SUMW) * SQRT(2.0*W1/SUMW) * OM2F22/OM1F12 *
     1    (SIG2/SIG12)**2
      Q2 = ((2.5-1.2*BST12)*W2**2 +
     1                           3.0*W1**2 + 1.6*W1*W2*AST12)*H
C
      H = ((W1-W2)/SUMW)**2 * (2.5-1.2*BST12)*15.0 +
     1      4.0*W1*W2*AST12/SUMW**2 * (11.0 - 2.4*BST12)
      Q12 = H + 1.6*SUMW*OM2F11*OM2F22 / (SQRT(W1*W2)*OM1F12**2) *
     1      SIG1**2*SIG2**2/SIG12**4
      D12 = ((6.0*CST12-5.0)**2/10.0) * (P1+P2+P12) /
     1      (Q1+Q2+Q12) + 1.0
C
C     end of FUNCTION D12
      RETURN
      END
C                                                                     C
C---------------------------------------------------------------------C
C                                                                     C
C*****precision > double
      real FUNCTION PARKER (T, EPS)
      IMPLICIT real (A-H, O-Z), INTEGER (I-N)
C*****END precision > double
C*****precision > single
C      REAL FUNCTION PARKER (T, EPS)
C      IMPLICIT REAL (A-H, O-Z), INTEGER (I-N)
C*****END precision > single
C
C  START PROLOGUE
C  TEMPERATURE DEPENDENCE OF THE ROTATIONAL COLLISION NUMBERS
C
C  REF: BRAU, C.A., JONKMAN, R.M., "CLASSICAL THEORY
C       OF ROTATIONAL RELAXATION IN DIATOMIC GASES",
C       JCP,VOL52,P477(1970)
C  END PROLOGUE
C
C*****precision > double
      DATA   PI32O2 /2.7841639984158539D+00/,
     $       P2O4P2 /4.4674011002723397D+00/,
     $       PI32   /5.5683279968317078D+00/
C*****END precision > double
C*****precision > single
C      DATA   PI32O2 /2.7841639984158539E+00/,
C     $       P2O4P2 /4.4674011002723397E+00/,
C     $       PI32   /5.5683279968317078E+00/
C*****END precision > single

      D  = EPS / T
      DR = EPS / 298.0
      PARKER = (1.0 + PI32O2*SQRT(DR) + P2O4P2*DR + PI32*DR**1.5) /
     1    (1.0 + PI32O2*SQRT(D)  + P2O4P2*D  + PI32*D**1.5)
C
C     end of FUNCTION PARKER
      RETURN
      END
C                                                                     C
C---------------------------------------------------------------------C
C*****precision > double
      real FUNCTION QINTRP (ARG, X, Y)
      IMPLICIT real (A-H, O-Z), INTEGER (I-N)
C*****END precision > double
C*****precision > single
C      REAL FUNCTION QINTRP (ARG, X, Y)
C      IMPLICIT REAL (A-H, O-Z), INTEGER (I-N)
C*****END precision > single
C
C  START PROLOGUE
C  QUADRATIC INTERPOLATION
C  END PROLOGUE
C
      DIMENSION X(3),Y(3)
      VAL1 = Y(1) + (ARG-X(1))*(Y(2)-Y(1)) / (X(2)-X(1))
      VAL2 = Y(2) + (ARG-X(2))*(Y(3)-Y(2)) / (X(3)-X(2))
      FAC1 = (ARG-X(1)) / (X(2)-X(1)) / 2.0
      FAC2 = (X(3)-ARG) / (X(3)-X(2)) / 2.0
      IF (ARG .GE. X(2)) THEN
         VAL = (VAL1*FAC2+VAL2) / (1.0+FAC2)
      ELSE
         VAL = (VAL1+VAL2*FAC1) / (1.0+FAC1)
      ENDIF
      QINTRP = VAL
C
C     end of FUNCTION QINTRP
      RETURN
      END
C
C*****precision > double
      real FUNCTION OMEG12 (N, TR, DR)
      IMPLICIT real (A-H, O-Z), INTEGER (I-N)
C*****END precision > double
C*****precision > single
C      REAL FUNCTION OMEG12 (N, TR, DR)
C      IMPLICIT REAL (A-H, O-Z), INTEGER (I-N)
C*****END precision > single
C
C  START PROLOGUE
C  CALC. OF COLLISION INTEGRALS FOR A KRIEGER-12-6-3-POTENTIAL
C
C  END PROLOGUE
C
      DIMENSION VERT(3), VAL(3), TSTERN(37), DELTA(8), O(37,8), P(37,8)
      DIMENSION ARG(3)
      DATA TSTERN/.1,.2,.3,.4,.5,.6,.7,.8,.9,1.,1.2,1.4,1.6,1.8,2.,2.5,
     1            3.,3.5,4.,5.,6.,7.,8.,9.,10.,12.,14.,16.,18.,20.,25.,
     2            30.,35.,40.,50.,75.,100./
      DATA DELTA/0.,.25,.5,.75,1.,1.5,2.,2.5/
C
      IF (DR.LT.-.00001 .OR. DR.GT.2.5 .OR. TR.LT..09 .OR. TR.GT.500.
     1 .OR. (ABS(DR).GT.1.0E-5 .AND. TR.GT.75.0)) THEN
C##ERROR_CONDITION##
         WRITE (6,'(A)') 'COLLISION INTEGRAL UNDEFINED'
         RETURN
      ENDIF
C
      IF (TR .GT. 75.0)  THEN
         IF (N .EQ. 1) THEN
            OMEG12 = 0.623 - 0.136E-2*TR + 0.346E-5*TR*TR
     1                         -0.343E-8*TR*TR*TR
         ELSEIF (N .EQ. 2) THEN
            OMEG12 = 0.703 - 0.146E-2*TR + 0.357E-5*TR*TR
     1                         -0.343E-8*TR*TR*TR
         ENDIF
         RETURN
      ENDIF
C
      IF (TR .LE. 0.2) THEN
         II = 2
      ELSE
         II = 37
         DO 30 I = 2, 36
            IF (TSTERN(I-1).LT.TR .AND. TSTERN(I).GE.TR)  II=I
   30    CONTINUE
      ENDIF
C
      IF (ABS(DR) .LT. 1.0E-5) THEN
         DO 80 I = 1, 3
            ARG(I) = TSTERN(II-2+I)
            IF (N .EQ. 1) THEN
               VAL(I) = O(II-2+I, 1)
            ELSEIF (N .EQ. 2) THEN
               VAL(I) = P(II-2+I, 1)
            ENDIF
   80    CONTINUE
         OMEG12 = QINTRP(TR, ARG, VAL)
         RETURN
C DQ (12/20/2007): Replaced tab with spaces (see test2007_225.f)
      ENDIF
C
      IF (DR .LE. 0.25) THEN
         KK = 2
      ELSE
         KK = 7
         DO 10 K = 2, 7
            IF (DELTA(K-1).LT.DR .AND. DELTA(K) .GE. DR)  KK=K
   10    CONTINUE
      ENDIF
C
      DO 50 I = 1, 3
         DO 40 K = 1, 3
            ARG(K) = DELTA(KK-2+K)
            IF (N .EQ. 1)  THEN
               VAL(K) = O(II-2+I, KK-2+K)
            ELSEIF (N .EQ. 2)  THEN
               VAL(K) = P(II-2+I, KK-2+K)
            ENDIF
   40    CONTINUE
         VERT(I) = QINTRP(DR, ARG, VAL)
   50 CONTINUE
      DO 60 I = 1, 3
         ARG(I) = TSTERN(II-2+I)
   60 CONTINUE
      OMEG12 = QINTRP(TR, ARG, VERT)
C
C     end of FUNCTION OMEG12
      RETURN
      END
C                                                                    C
C--------------------------------------------------------------------C
C                                                                    C
C*****precision > double
      SUBROUTINE DP1VLU (L, NDER, X, YFIT, YP, A, KERR)
C***BEGIN PROLOGUE  DP1VLU
C***PURPOSE  Use the coefficients generated by DPOLFT to evaluate the
C            polynomial fit of degree L, along with the first NDER of
C            its derivatives, at a specified point.
C***LIBRARY   SLATEC
C***CATEGORY  K6
C***TYPE      real (PVALUE-S, DP1VLU-D)
C***KEYWORDS  CURVE FITTING, LEAST SQUARES, POLYNOMIAL APPROXIMATION
C***AUTHOR  Shampine, L. F., (SNLA)
C           Davenport, S. M., (SNLA)
C***DESCRIPTION
C
C     Abstract
C
C     The subroutine  DP1VLU  uses the coefficients generated by DPOLFT
C     to evaluate the polynomial fit of degree  L, along with the first
C     NDER  of its derivatives, at a specified point.  Computationally
C     stable recurrence relations are used to perform this task.
C
C     The parameters for  DP1VLU  are
C
C     Input -- ALL TYPE REAL variables are real
C         L -      the degree of polynomial to be evaluated.  L  may be
C                  any non-negative integer which is less than or equal
C                  to  NDEG , the highest degree polynomial provided
C                  by  DPOLFT .
C         NDER -   the number of derivatives to be evaluated.  NDER
C                  may be 0 or any positive value.  If NDER is less
C                  than 0, it will be treated as 0.
C         X -      the argument at which the polynomial and its
C                  derivatives are to be evaluated.
C         A -      work and output array containing values from last
C                  call to  DPOLFT .
C         KERR -   logical flag set to true if fatal error occurred
C     Output -- ALL TYPE REAL variables are real
C         YFIT -   value of the fitting polynomial of degree L at X
C         YP -     array containing the first through NDER derivatives
C                  of the polynomial of degree  L.  YP  must be
C                  dimensioned at least  NDER  in the calling program.
C
C***REFERENCES  L. F. Shampine, S. M. Davenport and R. E. Huddleston,
C                 Curve fitting by polynomials in one variable, Report
C                 SLA-74-0270, Sandia Laboratories, June 1974.
C***ROUTINES CALLED  XERMSG
C***REVISION HISTORY  (YYMMDD)
C   740601  DATE WRITTEN
C   890531  Changed all specific intrinsics to generic.  (WRB)
C   890911  Removed unnecessary intrinsics.  (WRB)
C   891006  Cosmetic changes to prologue.  (WRB)
C   891006  REVISION DATE from Version 3.2
C   891214  Prologue converted to Version 4.0 format.  (BAB)
C   900315  CALLs to XERROR changed to CALLs to XERMSG.  (THJ)
C   900510  Convert XERRWV calls to XERMSG calls.  (RWC)
C   920501  Reformatted the REFERENCES section.  (WRB)
C***END PROLOGUE  DP1VLU
      IMPLICIT real (A-H,O-Z)
      INTEGER I,IC,ILO,IN,INP1,IUP,K1,K1I,K2,K3,K3P1,K3PN,K4,K4P1,K4PN,
     * KC,L,LM1,LP1,MAXORD,N,NDER,NDO,NDP1,NORD
      real A(*),CC,DIF,VAL,X,YFIT,YP(*)
      CHARACTER*8 XERN1, XERN2
      LOGICAL KERR
C***FIRST EXECUTABLE STATEMENT  DP1VLU
      KERR = .FALSE.
      IF (L .LT. 0) GO TO 12
      NDO = MAX(NDER,0)
      NDO = MIN(NDO,L)
      MAXORD = A(1) + 0.5D0
      K1 = MAXORD + 1
      K2 = K1 + MAXORD
      K3 = K2 + MAXORD + 2
      NORD = A(K3) + 0.5D0
      IF (L .GT. NORD) GO TO 11
      K4 = K3 + L + 1
      IF (NDER .LT. 1) GO TO 2
      DO 1 I = 1,NDER
 1      YP(I) = 0.0D0
 2    IF (L .GE. 2) GO TO 4
      IF (L .EQ. 1) GO TO 3
C
C L IS 0
C
      VAL = A(K2+1)
      GO TO 10
C
C L IS 1
C
 3    CC = A(K2+2)
      VAL = A(K2+1) + (X-A(2))*CC
      IF (NDER .GE. 1) YP(1) = CC
      GO TO 10
C
C L IS GREATER THAN 1
C
 4    NDP1 = NDO + 1
      K3P1 = K3 + 1
      K4P1 = K4 + 1
      LP1 = L + 1
      LM1 = L - 1
      ILO = K3 + 3
      IUP = K4 + NDP1
      DO 5 I = ILO,IUP
 5      A(I) = 0.0D0
      DIF = X - A(LP1)
      KC = K2 + LP1
      A(K4P1) = A(KC)
      A(K3P1) = A(KC-1) + DIF*A(K4P1)
      A(K3+2) = A(K4P1)
C
C EVALUATE RECURRENCE RELATIONS FOR FUNCTION VALUE AND DERIVATIVES
C
      DO 9 I = 1,LM1
        IN = L - I
        INP1 = IN + 1
        K1I = K1 + INP1
        IC = K2 + IN
        DIF = X - A(INP1)
        VAL = A(IC) + DIF*A(K3P1) - A(K1I)*A(K4P1)
        IF (NDO .LE. 0) GO TO 8
        DO 6 N = 1,NDO
          K3PN = K3P1 + N
          K4PN = K4P1 + N
 6        YP(N) = DIF*A(K3PN) + N*A(K3PN-1) - A(K1I)*A(K4PN)
C
C SAVE VALUES NEEDED FOR NEXT EVALUATION OF RECURRENCE RELATIONS
C
        DO 7 N = 1,NDO
          K3PN = K3P1 + N
          K4PN = K4P1 + N
          A(K4PN) = A(K3PN)
 7        A(K3PN) = YP(N)
 8      A(K4P1) = A(K3P1)
 9      A(K3P1) = VAL
C
C NORMAL RETURN OR ABORT DUE TO ERROR
C
 10   YFIT = VAL
      RETURN
C
   11 WRITE (XERN1, '(I8)') L
      WRITE (XERN2, '(I8)') NORD
C##ERROR_CONDITION##
      CALL XERMSG ('SLATEC', 'DP1VLU',
     *  'THE ORDER OF POLYNOMIAL EVALUATION, L = ' // XERN1 //
     *  'REQUESTED EXCEEDS THE HIGHEST ORDER FIT, NORD = ' // XERN2 //
     *  ', COMPUTED BY DPOLFT -- EXECUTION TERMINATED.', 8, 2)
      KERR = .TRUE.
      RETURN
C
C##ERROR_CONDITION##
   12 CALL XERMSG ('SLATEC', 'DP1VLU',
     +  'INVALID INPUT PARAMETER.  ORDER OF POLYNOMIAL EVALUATION '//
     +  'REQUESTED IS NEGATIVE.', 2, 2)
C
C     end of SUBROUTINE DP1VLU
      KERR = .TRUE.
      RETURN
      END
C                                                                    C
C--------------------------------------------------------------------C
C                                                                    C
      SUBROUTINE DPCOEF (L, C, TC, A, KERR)
C***BEGIN PROLOGUE  DPCOEF
C***PURPOSE  Convert the DPOLFT coefficients to Taylor series form.
C***LIBRARY   SLATEC
C***CATEGORY  K1A1A2
C***TYPE      real (PCOEF-S, DPCOEF-D)
C***KEYWORDS  CURVE FITTING,DATA FITTING,LEAST SQUARES,POLYNOMIAL FIT
C***AUTHOR  Shampine, L. F., (SNLA)
C           Davenport, S. M., (SNLA)
C***DESCRIPTION
C
C     Abstract
C
C     DPOLFT computes the least squares polynomial fit of degree L as
C     a sum of orthogonal polynomials.  DPCOEF changes this fit to its
C     Taylor expansion about any point  C , i.e. writes the polynomial
C     as a sum of powers of (X-C).  Taking  C=0.  gives the polynomial
C     in powers of X, but a suitable non-zero  C  often leads to
C    polynomials which are better scaled and more accurately evaluated.
C
C     The parameters for  DPCOEF  are
C
C     INPUT -- All TYPE REAL variables are real
C         L -      Indicates the degree of polynomial to be changed to
C                  its Taylor expansion.  To obtain the Taylor
C                  coefficients in reverse order, input  L  as the
C                  negative of the degree desired.  The absolute value
C                  of L must be less than or equal to NDEG, the highest
C                  degree polynomial fitted by  DPOLFT .
C         C -      The point about which the Taylor expansion is to be
C                  made.
C         A -      Work and output array containing values from last
C                  call to  DPOLFT .
C         KERR -   logical flag set to true if fatal error occurred
C
C     OUTPUT -- All TYPE REAL variables are real
C         TC -     Vector containing the first LL+1 Taylor coefficients
C                  where LL=ABS(L).  If  L.GT.0 , the coefficients are
C                  in the usual Taylor series order, i.e.
C                   P(X) = TC(1) + TC(2)*(X-C) + ... + TC(N+1)*(X-C)**N
C                  If L .LT. 0, the coefficients are in reverse order,
C                  i.e.
C                   P(X) = TC(1)*(X-C)**N + ... + TC(N)*(X-C) + TC(N+1)
C
C***REFERENCES  L. F. Shampine, S. M. Davenport and R. E. Huddleston,
C                 Curve fitting by polynomials in one variable, Report
C                 SLA-74-0270, Sandia Laboratories, June 1974.
C***ROUTINES CALLED  DP1VLU
C***REVISION HISTORY  (YYMMDD)
C   740601  DATE WRITTEN
C   890531  Changed all specific intrinsics to generic.  (WRB)
C   891006  Cosmetic changes to prologue.  (WRB)
C   891006  REVISION DATE from Version 3.2
C   891214  Prologue converted to Version 4.0 format.  (BAB)
C   920501  Reformatted the REFERENCES section.  (WRB)
C***END PROLOGUE  DPCOEF
C
      INTEGER I,L,LL,LLP1,LLP2,NEW,NR
      real A(*),C,FAC,SAVE,TC(*)
      LOGICAL KERR
C***FIRST EXECUTABLE STATEMENT  DPCOEF
      KERR = .FALSE.
      LL = ABS(L)
      LLP1 = LL + 1
      CALL DP1VLU (LL,LL,C,TC(1),TC(2),A, KERR)
      IF (LL .LT. 2) GO TO 2
      FAC = 1.0D0
      DO 1 I = 3,LLP1
        FAC = FAC*(I-1)
 1      TC(I) = TC(I)/FAC
 2    IF (L .GE. 0) GO TO 4
      NR = LLP1/2
      LLP2 = LL + 2
      DO 3 I = 1,NR
        SAVE = TC(I)
        NEW = LLP2 - I
        TC(I) = TC(NEW)
 3      TC(NEW) = SAVE
C
C     end of SUBROUTINE DPCOEF
 4    RETURN
      END
C                                                                    C
C--------------------------------------------------------------------C
C                                                                    C
      SUBROUTINE DPOLFT (N, X, Y, W, MAXDEG, NDEG, EPS, R, IERR, A)
C***BEGIN PROLOGUE  DPOLFT
C***PURPOSE  Fit discrete data in a least squares sense by polynomials
C            in one variable.
C***LIBRARY   SLATEC
C***CATEGORY  K1A1A2
C***TYPE      real (POLFIT-S, DPOLFT-D)
C***KEYWORDS  CURVE FITTING, DATA FITTING,LEAST SQUARES,POLYNOMIAL FIT
C***AUTHOR  Shampine, L. F., (SNLA)
C           Davenport, S. M., (SNLA)
C           Huddleston, R. E., (SNLL)
C***DESCRIPTION
C
C     Abstract
C
C     Given a collection of points X(I) and a set of values Y(I) which
C     correspond to some function or measurement at each of the X(I),
C     subroutine  DPOLFT computes the weighted least-squares polynomial
C    fits of all degrees up to some degree either specified by the user
C     or determined by the routine.  The fits thus obtained are in
C     orthogonal polynomial form.  Subroutine  DP1VLU  may then be
C     called to evaluate the fitted polynomials and any of their
C     derivatives at any point.  The subroutine  DPCOEF  may be used to
C     express the polynomial fits as powers of (X-C) for any specified
C     point C.
C
C     The parameters for  DPOLFT  are
C
C     Input -- All TYPE REAL variables are real
C         N -      the number of data points.  The arrays X, Y and W
C                  must be dimensioned at least  N  (N .GE. 1).
C         X -      array of values of the independent variable.  These
C                  values may appear in any order and need not all be
C                  distinct.
C         Y -      array of corresponding function values.
C         W -      array of positive values to be used as weights.  If
C                  W(1) is negative,  DPOLFT  will set all the weights
C                  to 1.0, which means unweighted least squares error
C                  will be minimized.  To minimize relative error, the
C                  user should set the weights to:  W(I) = 1.0/Y(I)**2,
C                  I = 1,...,N .
C         MAXDEG - maximum degree to be allowed for polynomial fit.
C                  MAXDEG  may be any non-negative integer less than N.
C                  Note -- MAXDEG  cannot be equal to  N-1  when a
C                  statistical test is to be used for degree selection,
C                  i.e., when input value of  EPS  is negative.
C         EPS -    specifies the criterion to be used in determining
C                  the degree of fit to be computed.
C                  (1)  If  EPS  is input negative,  DPOLFT chooses the
C                       degree based on a statistical F test of
C                       significance.  One of three possible
C                       significance levels will be used:  .01, .05 or
C                       .10.  If  EPS=-1.0 , the routine will
C                       automatically select one of these levels based
C                       on the number of data points and the maximum
C                       degree to be considered.  If  EPS  is input as
C                       -.01, -.05, or -.10, a significance level of
C                       .01, .05, or .10, respectively, will be used.
C                  (2)  If  EPS  is set to 0.,  DPOLFT  computes the
C                       polynomials of degrees 0 through  MAXDEG .
C                  (3)  If  EPS  is input positive,  EPS  is the RMS
C                       error tolerance which must be satisfied by the
C                       fitted polynomial.  DPOLFT  will increase the
C                       degree of fit until this criterion is met or
C                       until the maximum degree is reached.
C
C     Output -- All TYPE REAL variables are real
C         NDEG -   degree of the highest degree fit computed.
C         EPS -    RMS error of the polynomial of degree  NDEG .
C         R -      vector of dimension at least NDEG containing values
C                  of the fit of degree  NDEG  at each of the  X(I) .
C                  Except when the statistical test is used, these
C                 values are more accurate than results from subroutine
C                  DP1VLU  normally are.
C         IERR -   error flag with the following possible values.
C             1 -- indicates normal execution, i.e., either
C                  (1)  the input value of  EPS  was negative, and the
C                       computed polynomial fit of degree  NDEG
C                       satisfies the specified F test, or
C                 (2)  the input value of  EPS  was 0., and the fits of
C                       all degrees up to  MAXDEG  are complete, or
C                  (3)  the input value of  EPS  was positive, and the
C                       polynomial of degree  NDEG  satisfies the RMS
C                       error requirement.
C             2 -- invalid input parameter.  At least one of the input
C                 parameters has an illegal value and must be corrected
C                  before  DPOLFT  can proceed.  Valid input results
C                  when the following restrictions are observed
C                       N .GE. 1
C                       0 .LE. MAXDEG .LE. N-1  for  EPS .GE. 0.
C                       0 .LE. MAXDEG .LE. N-2  for  EPS .LT. 0.
C                       W(1)=-1.0  or  W(I) .GT. 0., I=1,...,N .
C             3 -- cannot satisfy the RMS error requirement with a
C                  polynomial of degree no greater than  MAXDEG .  Best
C                  fit found is of degree  MAXDEG .
C             4 -- cannot satisfy the test for significance using
C                  current value of  MAXDEG .  Statistically, the
C                  best fit found is of order  NORD .  (In this case,
C                  NDEG will have one of the values:  MAXDEG-2,
C                  MAXDEG-1, or MAXDEG).  Using a higher value of
C                  MAXDEG  may result in passing the test.
C             5 -- fatal error in DP1VLU
C         A -      work and output array having at least 3N+3MAXDEG+3
C                  locations
C
C     Note - DPOLFT  calculates all fits of degrees up to and including
C            NDEG .  Any or all of these fits can be evaluated or
C            expressed as powers of (X-C) using  DP1VLU  and  DPCOEF
C            after just one call to  DPOLFT .
C
C***REFERENCES  L. F. Shampine, S. M. Davenport and R. E. Huddleston,
C                 Curve fitting by polynomials in one variable, Report
C                 SLA-74-0270, Sandia Laboratories, June 1974.
C***ROUTINES CALLED  DP1VLU, XERMSG
C***REVISION HISTORY  (YYMMDD)
C   740601  DATE WRITTEN
C   890531  Changed all specific intrinsics to generic.  (WRB)
C   891006  Cosmetic changes to prologue.  (WRB)
C   891006  REVISION DATE from Version 3.2
C   891214  Prologue converted to Version 4.0 format.  (BAB)
C   900315  CALLs to XERROR changed to CALLs to XERMSG.  (THJ)
C   900911  Added variable YP to real declaration.  (WRB)
C   920501  Reformatted the REFERENCES section.  (WRB)
C   920527  Corrected erroneous statements in DESCRIPTION.  (WRB)
C***END PROLOGUE  DPOLFT
      INTEGER I,IDEGF,IERR,J,JP1,JPAS,K1,K1PJ,K2,K2PJ,K3,K3PI,K4,
     * K4PI,K5,K5PI,KSIG,M,MAXDEG,MOP1,NDEG,NDER,NFAIL
      real TEMD1,TEMD2
      real A(*),DEGF,DEN,EPS,ETST,F,FCRIT,R(*),SIG,SIGJ,
     * SIGJM1,SIGPAS,TEMP,X(*),XM,Y(*),YP,W(*),W1,W11
      real CO(4,3)
      LOGICAL KERR
      SAVE CO
      DATA  CO(1,1), CO(2,1), CO(3,1), CO(4,1), CO(1,2), CO(2,2),
     1      CO(3,2), CO(4,2), CO(1,3), CO(2,3), CO(3,3),
     2  CO(4,3)/-13.086850D0,-2.4648165D0,-3.3846535D0,-1.2973162D0,
     3          -3.3381146D0,-1.7812271D0,-3.2578406D0,-1.6589279D0,
     4          -1.6282703D0,-1.3152745D0,-3.2640179D0,-1.9829776D0/
C***FIRST EXECUTABLE STATEMENT  DPOLFT
      KERR = .FALSE.
      M = ABS(N)
      IF (M .EQ. 0) GO TO 30
      IF (MAXDEG .LT. 0) GO TO 30
      A(1) = MAXDEG
      MOP1 = MAXDEG + 1
      IF (M .LT. MOP1) GO TO 30
      IF (EPS .LT. 0.0D0 .AND.  M .EQ. MOP1) GO TO 30
      XM = M
      ETST = EPS*EPS*XM
      IF (W(1) .LT. 0.0D0) GO TO 2
      DO 1 I = 1,M
        IF (W(I) .LE. 0.0D0) GO TO 30
 1      CONTINUE
      GO TO 4
 2    DO 3 I = 1,M
 3      W(I) = 1.0D0
 4    IF (EPS .GE. 0.0D0) GO TO 8
C
C DETERMINE SIGNIFICANCE LEVEL INDEX TO BE USED IN STATISTICAL TEST
C FOR CHOOSING DEGREE OF POLYNOMIAL FIT
C
      IF (EPS .GT. (-.55D0)) GO TO 5
      IDEGF = M - MAXDEG - 1
      KSIG = 1
      IF (IDEGF .LT. 10) KSIG = 2
      IF (IDEGF .LT. 5) KSIG = 3
      GO TO 8
 5    KSIG = 1
      IF (EPS .LT. (-.03D0)) KSIG = 2
      IF (EPS .LT. (-.07D0)) KSIG = 3
C
C INITIALIZE INDEXES AND COEFFICIENTS FOR FITTING
C
 8    K1 = MAXDEG + 1
      K2 = K1 + MAXDEG
      K3 = K2 + MAXDEG + 2
      K4 = K3 + M
      K5 = K4 + M
      DO 9 I = 2,K4
 9      A(I) = 0.0D0
      W11 = 0.0D0
      IF (N .LT. 0) GO TO 11
C
C UNCONSTRAINED CASE
C
      DO 10 I = 1,M
        K4PI = K4 + I
        A(K4PI) = 1.0D0
 10     W11 = W11 + W(I)
      GO TO 13
C
C CONSTRAINED CASE
C
 11   DO 12 I = 1,M
        K4PI = K4 + I
 12     W11 = W11 + W(I)*A(K4PI)**2
C
C COMPUTE FIT OF DEGREE ZERO
C
 13   TEMD1 = 0.0D0
      DO 14 I = 1,M
        K4PI = K4 + I
        TEMD1 = TEMD1 + W(I)*Y(I)*A(K4PI)
 14     CONTINUE
      TEMD1 = TEMD1/W11
      A(K2+1) = TEMD1
      SIGJ = 0.0D0
      DO 15 I = 1,M
        K4PI = K4 + I
        K5PI = K5 + I
        TEMD2 = TEMD1*A(K4PI)
        R(I) = TEMD2
        A(K5PI) = TEMD2 - R(I)
 15     SIGJ = SIGJ + W(I)*((Y(I)-R(I)) - A(K5PI))**2
      J = 0
C
C SEE IF POLYNOMIAL OF DEGREE 0 SATISFIES THE DEGREE SELECTION CRITERIA
C
      IF (EPS) 24,26,27
C
C INCREMENT DEGREE
C
 16   J = J + 1
      JP1 = J + 1
      K1PJ = K1 + J
      K2PJ = K2 + J
      SIGJM1 = SIGJ
C
C COMPUTE NEW B COEFFICIENT EXCEPT WHEN J = 1
C
      IF (J .GT. 1) A(K1PJ) = W11/W1
C
C COMPUTE NEW A COEFFICIENT
C
      TEMD1 = 0.0D0
      DO 18 I = 1,M
        K4PI = K4 + I
        TEMD2 = A(K4PI)
        TEMD1 = TEMD1 + X(I)*W(I)*TEMD2*TEMD2
 18   CONTINUE
      A(JP1) = TEMD1/W11
C
C EVALUATE ORTHOGONAL POLYNOMIAL AT DATA POINTS
C
      W1 = W11
      W11 = 0.0D0
      DO 19 I = 1,M
        K3PI = K3 + I
        K4PI = K4 + I
        TEMP = A(K3PI)
        A(K3PI) = A(K4PI)
        A(K4PI) = (X(I)-A(JP1))*A(K3PI) - A(K1PJ)*TEMP
 19     W11 = W11 + W(I)*A(K4PI)**2
C
C GET NEW ORTHOGONAL POLYNOMIAL COEFFICIENT USING PARTIAL DOUBLE
C PRECISION
C
      TEMD1 = 0.0D0
      DO 20 I = 1,M
        K4PI = K4 + I
        K5PI = K5 + I
        TEMD2 = W(I)*((Y(I)-R(I))-A(K5PI))*A(K4PI)
 20     TEMD1 = TEMD1 + TEMD2
      TEMD1 = TEMD1/W11
      A(K2PJ+1) = TEMD1
C
C UPDATE POLYNOMIAL EVALUATIONS AT EACH OF THE DATA POINTS, AND
C ACCUMULATE SUM OF SQUARES OF ERRORS.  THE POLYNOMIAL EVALUATIONS ARE
C COMPUTED AND STORED IN EXTENDED PRECISION.  FOR THE I-TH DATA POINT,
C THE MOST SIGNIFICANT BITS ARE STORED IN  R(I) , AND THE LEAST
C SIGNIFICANT BITS ARE IN  A(K5PI) .
C
      SIGJ = 0.0D0
      DO 21 I = 1,M
        K4PI = K4 + I
        K5PI = K5 + I
        TEMD2 = R(I) + A(K5PI) + TEMD1*A(K4PI)
        R(I) = TEMD2
        A(K5PI) = TEMD2 - R(I)
 21     SIGJ = SIGJ + W(I)*((Y(I)-R(I)) - A(K5PI))**2
C
C SEE IF DEGREE SELECTION CRITERION HAS BEEN SATISFIED OR IF DEGREE
C MAXDEG  HAS BEEN REACHED
C
      IF (EPS) 23,26,27
C
C COMPUTE F STATISTICS  (INPUT EPS .LT. 0.)
C
 23   IF (SIGJ .EQ. 0.0D0) GO TO 29
      DEGF = M - J - 1
      DEN = (CO(4,KSIG)*DEGF + 1.0D0)*DEGF
      FCRIT = (((CO(3,KSIG)*DEGF) + CO(2,KSIG))*DEGF + CO(1,KSIG))/DEN
      FCRIT = FCRIT*FCRIT
      F = (SIGJM1 - SIGJ)*DEGF/SIGJ
      IF (F .LT. FCRIT) GO TO 25
C
C POLYNOMIAL OF DEGREE J SATISFIES F TEST
C
 24   SIGPAS = SIGJ
      JPAS = J
      NFAIL = 0
      IF (MAXDEG .EQ. J) GO TO 32
      GO TO 16
C
C POLYNOMIAL OF DEGREE J FAILS F TEST.  IF THERE HAVE BEEN THREE
C SUCCESSIVE FAILURES, A STATISTICALLY BEST DEGREE HAS BEEN FOUND.
C
 25   NFAIL = NFAIL + 1
      IF (NFAIL .GE. 3) GO TO 29
      IF (MAXDEG .EQ. J) GO TO 32
      GO TO 16
C
C RAISE THE DEGREE IF DEGREE  MAXDEG  HAS NOT YET BEEN REACHED  (INPUT
C EPS = 0.)
C
 26   IF (MAXDEG .EQ. J) GO TO 28
      GO TO 16
C
C SEE IF RMS ERROR CRITERION IS SATISFIED  (INPUT EPS .GT. 0.)
C
 27   IF (SIGJ .LE. ETST) GO TO 28
      IF (MAXDEG .EQ. J) GO TO 31
      GO TO 16
C
C RETURNS
C
 28   IERR = 1
      NDEG = J
      SIG = SIGJ
      GO TO 33
 29   IERR = 1
      NDEG = JPAS
      SIG = SIGPAS
      GO TO 33
 30   IERR = 2
C##ERROR_CONDITION##
      CALL XERMSG ('SLATEC', 'DPOLFT', 'INVALID INPUT PARAMETER.', 2,
     +   1)
      GO TO 37
 31   IERR = 3
      NDEG = MAXDEG
      SIG = SIGJ
      GO TO 33
 32   IERR = 4
      NDEG = JPAS
      SIG = SIGPAS
C
 33   A(K3) = NDEG
C
C WHEN STATISTICAL TEST HAS BEEN USED, EVALUATE THE BEST POLYNOMIAL AT
C ALL THE DATA POINTS IF  R  DOES NOT ALREADY CONTAIN THESE VALUES
C
      IF(EPS .GE. 0.0  .OR.  NDEG .EQ. MAXDEG) GO TO 36
      NDER = 0
      DO 35 I = 1,M
        CALL DP1VLU (NDEG,NDER,X(I),R(I),YP,A, KERR)
        IF (KERR) THEN
           IERR = 5
           RETURN
        ENDIF
 35   CONTINUE
 36   EPS = SQRT(SIG/XM)
C
C     end of SUBROUTINE DPOLFT
 37   RETURN
      END
C*****END precision > double
C                                                                     C
C---------------------------------------------------------------------C
C                                                                     C
C*****precision > single
C      SUBROUTINE PVALUE (L, NDER, X, YFIT, YP, A, KERR)
CC***BEGIN PROLOGUE  PVALUE
CC***PURPOSE  Use the coefficients generated by POLFIT to evaluate the
CC            polynomial fit of degree L, along with the first NDER of
CC            its derivatives, at a specified point.
CC***LIBRARY   SLATEC
CC***CATEGORY  K6
CC***TYPE      SINGLE PRECISION (PVALUE-S, DP1VLU-D)
CC***KEYWORDS  CURVE FITTING, LEAST SQUARES, POLYNOMIAL APPROXIMATION
CC***AUTHOR  Shampine, L. F., (SNLA)
CC           Davenport, S. M., (SNLA)
CC***DESCRIPTION
CC
CC     Written by L. F. Shampine and S. M. Davenport.
CC
CC     Abstract
CC
CC     The subroutine  PVALUE  uses the coefficients generated by POLFIT
CC     to evaluate the polynomial fit of degree  L, along with the first
CC     NDER  of its derivatives, at a specified point.  Computationally
CC     stable recurrence relations are used to perform this task.
CC
CC     The parameters for  PVALUE  are
CC
CC     Input --
CC         L -      the degree of polynomial to be evaluated.  L may be
CC                  any non-negative integer which is less than or equal
CC                  to  NDEG , the highest degree polynomial provided
CC                  by  POLFIT .
CC         NDER -   the number of derivatives to be evaluated.  NDER
CC                  may be 0 or any positive value.  If NDER is less
CC                  than 0, it will be treated as 0.
CC         X -      the argument at which the polynomial and its
CC                  derivatives are to be evaluated.
CC         A -      work and output array containing values from last
CC                  call to  POLFIT .
CC         KERR -   logical flag set to true if fatal error occurred
CC
CC     Output --
CC         YFIT -   value of the fitting polynomial of degree  L  at  X
CC         YP -     array containing the first through  NDER derivatives
CC                  of the polynomial of degree  L .  YP  must be
CC                  dimensioned at least  NDER  in the calling program.
CC
CC***REFERENCES  L. F. Shampine, S. M. Davenport and R. E. Huddleston,
CC                 Curve fitting by polynomials in one variable, Report
CC                 SLA-74-0270, Sandia Laboratories, June 1974.
CC***ROUTINES CALLED  XERMSG
CC***REVISION HISTORY  (YYMMDD)
CC   740601  DATE WRITTEN
CC   890531  Changed all specific intrinsics to generic.  (WRB)
CC   890531  REVISION DATE from Version 3.2
CC   891214  Prologue converted to Version 4.0 format.  (BAB)
CC   900315  CALLs to XERROR changed to CALLs to XERMSG.  (THJ)
CC   900510  Convert XERRWV calls to XERMSG calls.  (RWC)
CC   920501  Reformatted the REFERENCES section.  (WRB)
CC***END PROLOGUE  PVALUE
C      DIMENSION YP(*),A(*)
C      CHARACTER*8 XERN1, XERN2
C      LOGICAL KERR
CC***FIRST EXECUTABLE STATEMENT  PVALUE
C      KERR = .FALSE.
C      IF (L .LT. 0) GO TO 12
C      NDO = MAX(NDER,0)
C      NDO = MIN(NDO,L)
C      MAXORD = A(1) + 0.5
C      K1 = MAXORD + 1
C      K2 = K1 + MAXORD
C      K3 = K2 + MAXORD + 2
C      NORD = A(K3) + 0.5
C      IF (L .GT. NORD) GO TO 11
C      K4 = K3 + L + 1
C      IF (NDER .LT. 1) GO TO 2
C      DO 1 I = 1,NDER
C 1      YP(I) = 0.0
C 2    IF (L .GE. 2) GO TO 4
C      IF (L .EQ. 1) GO TO 3
CC
CC L IS 0
CC
C      VAL = A(K2+1)
C      GO TO 10
CC
CC L IS 1
CC
C 3    CC = A(K2+2)
C      VAL = A(K2+1) + (X-A(2))*CC
C      IF (NDER .GE. 1) YP(1) = CC
C      GO TO 10
CC
CC L IS GREATER THAN 1
CC
C 4    NDP1 = NDO + 1
C      K3P1 = K3 + 1
C      K4P1 = K4 + 1
C      LP1 = L + 1
C      LM1 = L - 1
C      ILO = K3 + 3
C      IUP = K4 + NDP1
C      DO 5 I = ILO,IUP
C 5      A(I) = 0.0
C      DIF = X - A(LP1)
C      KC = K2 + LP1
C      A(K4P1) = A(KC)
C      A(K3P1) = A(KC-1) + DIF*A(K4P1)
C      A(K3+2) = A(K4P1)
CC
CC EVALUATE RECURRENCE RELATIONS FOR FUNCTION VALUE AND DERIVATIVES
CC
C      DO 9 I = 1,LM1
C        IN = L - I
C        INP1 = IN + 1
C        K1I = K1 + INP1
C        IC = K2 + IN
C        DIF = X - A(INP1)
C        VAL = A(IC) + DIF*A(K3P1) - A(K1I)*A(K4P1)
C        IF (NDO .LE. 0) GO TO 8
C        DO 6 N = 1,NDO
C          K3PN = K3P1 + N
C          K4PN = K4P1 + N
C 6        YP(N) = DIF*A(K3PN) + N*A(K3PN-1) - A(K1I)*A(K4PN)
CC
CC SAVE VALUES NEEDED FOR NEXT EVALUATION OF RECURRENCE RELATIONS
CC
C        DO 7 N = 1,NDO
C          K3PN = K3P1 + N
C          K4PN = K4P1 + N
C          A(K4PN) = A(K3PN)
C 7        A(K3PN) = YP(N)
C 8      A(K4P1) = A(K3P1)
C 9      A(K3P1) = VAL
CC
CC NORMAL RETURN OR ABORT DUE TO ERROR
CC
C 10   YFIT = VAL
C      RETURN
CC
C   11 WRITE (XERN1, '(I8)') L
C      WRITE (XERN2, '(I8)') NORD
CC##ERROR_CONDITION##
C      CALL XERMSG ('SLATEC', 'PVALUE',
C     *  'THE ORDER OF POLYNOMIAL EVALUATION, L = ' // XERN1 //
C     *  ' REQUESTED EXCEEDS THE HIGHEST ORDER FIT, NORD = ' // XERN2 //
C     *  ', COMPUTED BY POLFIT -- EXECUTION TERMINATED.', 8, 2)
C      KERR = .TRUE.
C      RETURN
CC
CC##ERROR_CONDITION##
C   12 CALL XERMSG ('SLATEC', 'PVALUE',
C     +  'INVALID INPUT PARAMETER.  ORDER OF POLYNOMIAL EVALUATION ' //
C     +  'REQUESTED IS NEGATIVE -- EXECUTION TERMINATED.', 2, 2)
CC
C      KERR = .TRUE.
CC     end of SUBROUTINE PVALUE
C      RETURN
C      END
C                                                                     C
CC---------------------------------------------------------------------C
C                                                                     C
C      SUBROUTINE PCOEF (L, C, TC, A, KERR)
CC***BEGIN PROLOGUE  PCOEF
CC***PURPOSE  Convert the POLFIT coefficients to Taylor series form.
CC***LIBRARY   SLATEC
CC***CATEGORY  K1A1A2
CC***TYPE      SINGLE PRECISION (PCOEF-S, DPCOEF-D)
CC***KEYWORDS CURVE FITTING, DATA FITTING, LEAST SQUARES, POLYNOMIAL FIT
CC***AUTHOR  Shampine, L. F., (SNLA)
CC           Davenport, S. M., (SNLA)
CC***DESCRIPTION
CC
CC     Written BY L. F. Shampine and S. M. Davenport.
CC
CC     Abstract
CC
CC     POLFIT computes the least squares polynomial fit of degree  L  as
CC     a sum of orthogonal polynomials.  PCOEF  changes this fit to its
CC     Taylor expansion about any point  C , i.e. writes the polynomial
CC     as a sum of powers of (X-C).  Taking  C=0.  gives the polynomial
CC     in powers of X, but a suitable non-zero  C  often leads to
CC    polynomials which are better scaled and more accurately evaluated.
CC
CC     The parameters for  PCOEF  are
CC
CC     INPUT --
CC         L -      Indicates the degree of polynomial to be changed to
CC                  its Taylor expansion.  To obtain the Taylor
CC                  coefficients in reverse order, input  L  as the
CC                  negative of the degree desired.  The absolute value
CC                  of L must be less than or equal to NDEG, the highest
CC                  degree polynomial fitted by  POLFIT .
CC         C -      The point about which the Taylor expansion is to be
CC                  made.
CC         A -      Work and output array containing values from last
CC                  call to  POLFIT .
CC         KERR -   logical flag set to true if fatal error occurred
CC
CC     OUTPUT --
CC         TC -    Vector containing the first LL+1 Taylor coefficients
CC                  where LL=ABS(L).  If  L.GT.0 , the coefficients are
CC                  in the usual Taylor series order, i.e.
CC                   P(X) = TC(1) + TC(2)*(X-C) + ... + TC(N+1)*(X-C)**N
CC                  If L .LT. 0, the coefficients are in reverse order,
CC                  i.e.
CC                   P(X) = TC(1)*(X-C)**N + ... + TC(N)*(X-C) + TC(N+1)
CC
CC***REFERENCES  L. F. Shampine, S. M. Davenport and R. E. Huddleston,
CC                 Curve fitting by polynomials in one variable, Report
CC                 SLA-74-0270, Sandia Laboratories, June 1974.
CC***ROUTINES CALLED  PVALUE
CC***REVISION HISTORY  (YYMMDD)
CC   740601  DATE WRITTEN
CC   890531  Changed all specific intrinsics to generic.  (WRB)
CC   890531  REVISION DATE from Version 3.2
CC   891214  Prologue converted to Version 4.0 format.  (BAB)
CC   920501  Reformatted the REFERENCES section.  (WRB)
CC***END PROLOGUE  PCOEF
CC
C      DIMENSION A(*), TC(*)
C      LOGICAL KERR
CC***FIRST EXECUTABLE STATEMENT  PCOEF
C      KERR = .FALSE
C      LL = ABS(L)
C      LLP1 = LL + 1
C      CALL PVALUE (LL,LL,C,TC(1),TC(2),A,KERR)
C      IF (LL .LT. 2) GO TO 2
C      FAC = 1.0
C      DO 1 I = 3,LLP1
C        FAC = FAC*(I-1)
C 1      TC(I) = TC(I)/FAC
C 2    IF (L .GE. 0) GO TO 4
C      NR = LLP1/2
C      LLP2 = LL + 2
C      DO 3 I = 1,NR
C        SAVE = TC(I)
C        NEW = LLP2 - I
C        TC(I) = TC(NEW)
C 3      TC(NEW) = SAVE
CC
CC     end of SUBROUTINE PCOEF
C 4    RETURN
C      END
C                                                                     C
CC---------------------------------------------------------------------C
C                                                                     C
C      SUBROUTINE POLFIT (N, X, Y, W, MAXDEG, NDEG, EPS, R, IERR, A)
CC***BEGIN PROLOGUE  POLFIT
CC***PURPOSE  Fit discrete data in a least squares sense by polynomials
CC            in one variable.
CC***LIBRARY   SLATEC
CC***CATEGORY  K1A1A2
CC***TYPE      SINGLE PRECISION (POLFIT-S, DPOLFT-D)
CC***KEYWORDS CURVE FITTING, DATA FITTING, LEAST SQUARES, POLYNOMIAL FIT
CC***AUTHOR  Shampine, L. F., (SNLA)
CC           Davenport, S. M., (SNLA)
CC           Huddleston, R. E., (SNLL)
CC***DESCRIPTION
CC
CC     Abstract
CC
CC     Given a collection of points X(I) and a set of values Y(I) which
CC     correspond to some function or measurement at each of the X(I),
CC     subroutine  POLFIT computes the weighted least-squares polynomial
CC    fits of all degrees up to some degree either specified by the user
CC     or determined by the routine.  The fits thus obtained are in
CC     orthogonal polynomial form.  Subroutine  PVALUE  may then be
CC     called to evaluate the fitted polynomials and any of their
CC     derivatives at any point.  The subroutine  PCOEF  may be used to
CC     express the polynomial fits as powers of (X-C) for any specified
CC     point C.
CC
CC     The parameters for  POLFIT  are
CC
CC     Input --
CC         N -      the number of data points.  The arrays X, Y and W
CC                  must be dimensioned at least  N  (N .GE. 1).
CC         X -      array of values of the independent variable.  These
CC                  values may appear in any order and need not all be
CC                  distinct.
CC         Y -      array of corresponding function values.
CC         W -      array of positive values to be used as weights.  If
CC                  W(1) is negative,  POLFIT  will set all the weights
CC                  to 1.0, which means unweighted least squares error
CC                  will be minimized.  To minimize relative error, the
CC                  user should set the weights to:  W(I) = 1.0/Y(I)**2,
CC                  I = 1,...,N .
CC         MAXDEG - maximum degree to be allowed for polynomial fit.
CC                  MAXDEG  may be any non-negative integer less than N.
CC                  Note -- MAXDEG  cannot be equal to  N-1  when a
CC                  statistical test is to be used for degree selection,
CC                  i.e., when input value of  EPS  is negative.
CC         EPS -    specifies the criterion to be used in determining
CC                  the degree of fit to be computed.
CC                  (1)  If  EPS  is input negative,  POLFIT chooses the
CC                       degree based on a statistical F test of
CC                       significance.  One of three possible
CC                       significance levels will be used:  .01, .05 or
CC                       .10.  If  EPS=-1.0 , the routine will
CC                       automatically select one of these levels based
CC                       on the number of data points and the maximum
CC                       degree to be considered.  If  EPS  is input as
CC                       -.01, -.05, or -.10, a significance level of
CC                       .01, .05, or .10, respectively, will be used.
CC                  (2)  If  EPS  is set to 0.,  POLFIT  computes the
CC                       polynomials of degrees 0 through  MAXDEG .
CC                  (3)  If  EPS  is input positive,  EPS  is the RMS
CC                       error tolerance which must be satisfied by the
CC                       fitted polynomial.  POLFIT  will increase the
CC                       degree of fit until this criterion is met or
CC                       until the maximum degree is reached.
CC
CC     Output --
CC         NDEG -   degree of the highest degree fit computed.
CC         EPS -    RMS error of the polynomial of degree  NDEG .
CC         R -      vector of dimension at least NDEG containing values
CC                  of the fit of degree  NDEG  at each of the  X(I) .
CC                  Except when the statistical test is used, these
CC                 values are more accurate than results from subroutine
CC                  PVALUE  normally are.
CC         IERR -   error flag with the following possible values.
CC             1 -- indicates normal execution, i.e., either
CC                  (1)  the input value of  EPS  was negative, and the
CC                       computed polynomial fit of degree  NDEG
CC                       satisfies the specified F test, or
CC                 (2)  the input value of  EPS  was 0., and the fits of
CC                       all degrees up to  MAXDEG  are complete, or
CC                  (3)  the input value of  EPS  was positive, and the
CC                       polynomial of degree  NDEG  satisfies the RMS
CC                       error requirement.
CC             2 -- invalid input parameter.  At least one of the input
CC                 parameters has an illegal value and must be corrected
CC                  before  POLFIT  can proceed.  Valid input results
CC                  when the following restrictions are observed
CC                       N .GE. 1
CC                       0 .LE. MAXDEG .LE. N-1  for  EPS .GE. 0.
CC                       0 .LE. MAXDEG .LE. N-2  for  EPS .LT. 0.
CC                       W(1)=-1.0  or  W(I) .GT. 0., I=1,...,N .
CC             3 -- cannot satisfy the RMS error requirement with a
CC                  polynomial of degree no greater than  MAXDEG.  Best
CC                  fit found is of degree  MAXDEG .
CC             4 -- cannot satisfy the test for significance using
CC                  current value of  MAXDEG .  Statistically, the
CC                  best fit found is of order  NORD .  (In this case,
CC                  NDEG will have one of the values:  MAXDEG-2,
CC                  MAXDEG-1, or MAXDEG).  Using a higher value of
CC                  MAXDEG  may result in passing the test.
CC             5 -- fatal error in DP1VLU
CC         A -      work and output array having at least 3N+3MAXDEG+3
CC                  locations
CC
CC     Note - POLFIT calculates all fits of degrees up to and including
CC            NDEG .  Any or all of these fits can be evaluated or
CC            expressed as powers of (X-C) using  PVALUE  and  PCOEF
CC            after just one call to  POLFIT .
CC
CC***REFERENCES  L. F. Shampine, S. M. Davenport and R. E. Huddleston,
CC                 Curve fitting by polynomials in one variable, Report
CC                 SLA-74-0270, Sandia Laboratories, June 1974.
CC***ROUTINES CALLED  PVALUE, XERMSG
CC***REVISION HISTORY  (YYMMDD)
CC   740601  DATE WRITTEN
CC   890531  Changed all specific intrinsics to generic.  (WRB)
CC   890531  REVISION DATE from Version 3.2
CC   891214  Prologue converted to Version 4.0 format.  (BAB)
CC   900315  CALLs to XERROR changed to CALLs to XERMSG.  (THJ)
CC   920501  Reformatted the REFERENCES section.  (WRB)
CC   920527  Corrected erroneous statements in DESCRIPTION.  (WRB)
CC***END PROLOGUE  POLFIT
C      real TEMD1,TEMD2
C      DIMENSION X(*), Y(*), W(*), R(*), A(*)
C      DIMENSION CO(4,3)
C      LOGICAL KERR
C      SAVE CO
C      DATA  CO(1,1), CO(2,1), CO(3,1), CO(4,1), CO(1,2), CO(2,2),
C     1      CO(3,2), CO(4,2), CO(1,3), CO(2,3), CO(3,3),
C     2  CO(4,3)/-13.086850,-2.4648165,-3.3846535,-1.2973162,
C     3          -3.3381146,-1.7812271,-3.2578406,-1.6589279,
C     4          -1.6282703,-1.3152745,-3.2640179,-1.9829776/
CC***FIRST EXECUTABLE STATEMENT  POLFIT
C      KERR = .FALSE.
C      M = ABS(N)
C      IF (M .EQ. 0) GO TO 30
C      IF (MAXDEG .LT. 0) GO TO 30
C      A(1) = MAXDEG
C      MOP1 = MAXDEG + 1
C      IF (M .LT. MOP1) GO TO 30
C      IF (EPS .LT. 0.0  .AND.  M .EQ. MOP1) GO TO 30
C      XM = M
C      ETST = EPS*EPS*XM
C      IF (W(1) .LT. 0.0) GO TO 2
C      DO 1 I = 1,M
C        IF (W(I) .LE. 0.0) GO TO 30
C 1    CONTINUE
C      GO TO 4
C 2    DO 3 I = 1,M
C 3      W(I) = 1.0
C 4    IF (EPS .GE. 0.0) GO TO 8
CC
CC DETERMINE SIGNIFICANCE LEVEL INDEX TO BE USED IN STATISTICAL TEST
CC FOR CHOOSING DEGREE OF POLYNOMIAL FIT
CC
C      IF (EPS .GT. (-.55)) GO TO 5
C      IDEGF = M - MAXDEG - 1
C      KSIG = 1
C      IF (IDEGF .LT. 10) KSIG = 2
C      IF (IDEGF .LT. 5) KSIG = 3
C      GO TO 8
C 5    KSIG = 1
C      IF (EPS .LT. (-.03)) KSIG = 2
C      IF (EPS .LT. (-.07)) KSIG = 3
CC
CC INITIALIZE INDEXES AND COEFFICIENTS FOR FITTING
CC
C 8    K1 = MAXDEG + 1
C      K2 = K1 + MAXDEG
C      K3 = K2 + MAXDEG + 2
C      K4 = K3 + M
C      K5 = K4 + M
C      DO 9 I = 2,K4
C 9      A(I) = 0.0
C      W11 = 0.0
C      IF (N .LT. 0) GO TO 11
CC
CC UNCONSTRAINED CASE
CC
C      DO 10 I = 1,M
C        K4PI = K4 + I
C        A(K4PI) = 1.0
C 10     W11 = W11 + W(I)
C      GO TO 13
CC
CC CONSTRAINED CASE
CC
C 11   DO 12 I = 1,M
C        K4PI = K4 + I
C 12     W11 = W11 + W(I)*A(K4PI)**2
CC
CC COMPUTE FIT OF DEGREE ZERO
CC
C 13   TEMD1 = 0.0D0
C      DO 14 I = 1,M
C        K4PI = K4 + I
C        TEMD1 = TEMD1 + DBLE(W(I))*DBLE(Y(I))*DBLE(A(K4PI))
C 14   CONTINUE
C      TEMD1 = TEMD1/DBLE(W11)
C      A(K2+1) = TEMD1
C      SIGJ = 0.0
C      DO 15 I = 1,M
C        K4PI = K4 + I
C        K5PI = K5 + I
C        TEMD2 = TEMD1*DBLE(A(K4PI))
C        R(I) = TEMD2
C        A(K5PI) = TEMD2 - DBLE(R(I))
C 15     SIGJ = SIGJ + W(I)*((Y(I)-R(I)) - A(K5PI))**2
C      J = 0
CC
CC SEE IF POLYNOMIAL OF DEGREE 0 SATISFIES DEGREE SELECTION CRITERION
CC
C      IF (EPS) 24,26,27
CC
CC INCREMENT DEGREE
CC
C 16   J = J + 1
C      JP1 = J + 1
C      K1PJ = K1 + J
C      K2PJ = K2 + J
C      SIGJM1 = SIGJ
CC
CC COMPUTE NEW B COEFFICIENT EXCEPT WHEN J = 1
CC
C      IF (J .GT. 1) A(K1PJ) = W11/W1
CC
CC COMPUTE NEW A COEFFICIENT
CC
C      TEMD1 = 0.0D0
C      DO 18 I = 1,M
C        K4PI = K4 + I
C        TEMD2 = A(K4PI)
C        TEMD1 = TEMD1 + DBLE(X(I))*DBLE(W(I))*TEMD2*TEMD2
C 18   CONTINUE
C      A(JP1) = TEMD1/DBLE(W11)
CC
CC EVALUATE ORTHOGONAL POLYNOMIAL AT DATA POINTS
CC
C      W1 = W11
C      W11 = 0.0
C      DO 19 I = 1,M
C        K3PI = K3 + I
C        K4PI = K4 + I
C        TEMP = A(K3PI)
C        A(K3PI) = A(K4PI)
C        A(K4PI) = (X(I)-A(JP1))*A(K3PI) - A(K1PJ)*TEMP
C 19     W11 = W11 + W(I)*A(K4PI)**2
CC
CC GET NEW ORTHOGONAL POLYNOMIAL COEFFICIENT USING PARTIAL DOUBLE
CC PRECISION
CC
C      TEMD1 = 0.0D0
C      DO 20 I = 1,M
C        K4PI = K4 + I
C        K5PI = K5 + I
C        TEMD2 = DBLE(W(I))*DBLE((Y(I)-R(I))-A(K5PI))*DBLE(A(K4PI))
C 20     TEMD1 = TEMD1 + TEMD2
C      TEMD1 = TEMD1/DBLE(W11)
C      A(K2PJ+1) = TEMD1
CC
CC UPDATE POLYNOMIAL EVALUATIONS AT EACH OF THE DATA POINTS, AND
CC ACCUMULATE SUM OF SQUARES OF ERRORS.  THE POLYNOMIAL EVALUATIONS ARE
CC COMPUTED AND STORED IN EXTENDED PRECISION.  FOR THE I-TH DATA POINT,
CC THE MOST SIGNIFICANT BITS ARE STORED IN  R(I) , AND THE LEAST
CC SIGNIFICANT BITS ARE IN  A(K5PI) .
CC
C      SIGJ = 0.0
C      DO 21 I = 1,M
C        K4PI = K4 + I
C        K5PI = K5 + I
C        TEMD2 = DBLE(R(I)) + DBLE(A(K5PI)) + TEMD1*DBLE(A(K4PI))
C        R(I) = TEMD2
C        A(K5PI) = TEMD2 - DBLE(R(I))
C 21     SIGJ = SIGJ + W(I)*((Y(I)-R(I)) - A(K5PI))**2
CC
CC SEE IF DEGREE SELECTION CRITERION HAS BEEN SATISFIED OR IF DEGREE
CC MAXDEG  HAS BEEN REACHED
CC
C      IF (EPS) 23,26,27
CC
CC COMPUTE F STATISTICS  (INPUT EPS .LT. 0.)
CC
C 23   IF (SIGJ .EQ. 0.0) GO TO 29
C      DEGF = M - J - 1
C      DEN = (CO(4,KSIG)*DEGF + 1.0)*DEGF
C      FCRIT = (((CO(3,KSIG)*DEGF) + CO(2,KSIG))*DEGF + CO(1,KSIG))/DEN
C      FCRIT = FCRIT*FCRIT
C      F = (SIGJM1 - SIGJ)*DEGF/SIGJ
C      IF (F .LT. FCRIT) GO TO 25
CC
CC POLYNOMIAL OF DEGREE J SATISFIES F TEST
CC
C 24   SIGPAS = SIGJ
C      JPAS = J
C      NFAIL = 0
C      IF (MAXDEG .EQ. J) GO TO 32
C      GO TO 16
CC
CC POLYNOMIAL OF DEGREE J FAILS F TEST.  IF THERE HAVE BEEN THREE
CC SUCCESSIVE FAILURES, A STATISTICALLY BEST DEGREE HAS BEEN FOUND.
CC
C 25   NFAIL = NFAIL + 1
C      IF (NFAIL .GE. 3) GO TO 29
C      IF (MAXDEG .EQ. J) GO TO 32
C      GO TO 16
CC
CC RAISE THE DEGREE IF DEGREE  MAXDEG  HAS NOT YET BEEN REACHED  (INPUT
CC EPS = 0.)
CC
C 26   IF (MAXDEG .EQ. J) GO TO 28
C      GO TO 16
CC
CC SEE IF RMS ERROR CRITERION IS SATISFIED  (INPUT EPS .GT. 0.)
CC
C 27   IF (SIGJ .LE. ETST) GO TO 28
C      IF (MAXDEG .EQ. J) GO TO 31
C      GO TO 16
CC
CC RETURNS
CC
C 28   IERR = 1
C      NDEG = J
C      SIG = SIGJ
C      GO TO 33
C 29   IERR = 1
C      NDEG = JPAS
C      SIG = SIGPAS
C      GO TO 33
C 30   IERR = 2
CC##ERROR_CONDITION##
C      CALL XERMSG ('SLATEC', 'POLFIT', 'INVALID INPUT PARAMETER.', 2,
C     +   1)
C      GO TO 37
C 31   IERR = 3
C      NDEG = MAXDEG
C      SIG = SIGJ
C      GO TO 33
C 32   IERR = 4
C      NDEG = JPAS
C      SIG = SIGPAS
CC
C 33   A(K3) = NDEG
CC
CC WHEN STATISTICAL TEST HAS BEEN USED, EVALUATE THE BEST POLYNOMIAL AT
CC ALL THE DATA POINTS IF  R  DOES NOT ALREADY CONTAIN THESE VALUES
CC
C      IF(EPS .GE. 0.0  .OR.  NDEG .EQ. MAXDEG) GO TO 36
C      NDER = 0
C      DO 35 I = 1,M
C        CALL PVALUE (NDEG,NDER,X(I),R(I),YP,A,KERR)
C         IF (KERR) THEN
C            IERR = 5
C            RETURN
C         ENDIF
C 35   CONTINUE
C 36   EPS = SQRT(SIG/XM)
CC
CC     end of SUBROUTINE POLFIT
C 37   RETURN
C      END
C*****END precision > single
