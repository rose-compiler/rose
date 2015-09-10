#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>
#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))
#define REAL double
//#define ntotal  16
#define nghost 4
#define ncube 4
#define nc 5
#define nxL0 (3) // 1 + 2
#define nyL0 (3) // 1 + 2
#define nxL1 (4) // 2 + 2
#define nyL1 (4) // 2 + 2
#define nxL2 (6) // 4 + 2
#define nyL2 (6) // 4 + 2
#define nxL3 (10) // 8 + 2
#define nyL3 (10) // 8 + 2
#define nxL4 (18) // 16 + 2
#define nyL4 (18) // 16 + 2
#define nxL5 (34) // 32 + 2
#define nyL5 (34) // 32 + 2
#define nxL6 (66) // 64 + 2
#define nyL6 (66) // 64 + 2
#define nxL7 (130) // 130 + 2
#define nyL7 (130) // 130 + 2
#define nxL8 (258) // 256 + 2
#define nyL8 (258) // 256 + 2
#define nxL9 (514) // 512 + 2
#define nyL9 (514) // 512 + 2
#define nxL10 (1026) // 1024 + 2
#define nyL10 (1026) // 1024 + 2


REAL OneThird      = 1.e0/3.e0;
REAL TwoThirds     = 2.e0/3.e0;
REAL OneQuarter    = 1.e0/4.e0;
REAL ThreeQuarters = 3.e0/4.e0;
struct L0struct{
#pragma 0
REAL (*dptr)[nc][nyL0][nxL0][ncube][ncube];
};

struct L1struct{
#pragma 1
REAL (*dptr)[nc][nyL1][nxL1][ncube][ncube];
};

struct L2struct{
#pragma 2
REAL (*dptr)[nc][nyL2][nxL2][ncube][ncube];
};

struct L3struct{
#pragma 3
REAL (*dptr)[nc][nyL3][nxL3][ncube][ncube];
};

struct L4struct{
#pragma 4
REAL (*dptr)[nc][nyL4][nxL4][ncube][ncube];
};

struct L5struct{
#pragma 5
REAL (*dptr)[nc][nyL5][nxL5][ncube][ncube];
};

struct L6struct{
#pragma 6
REAL (*dptr)[nc][nyL6][nxL6][ncube][ncube];
};

struct L7struct{
#pragma 7
REAL (*dptr)[nc][nyL7][nxL7][ncube][ncube];
};

struct L8struct{
#pragma 8
REAL (*dptr)[nc][nyL8][nxL8][ncube][ncube];
};

struct L9struct{
#pragma 9
REAL (*dptr)[nc][nyL9][nxL9][ncube][ncube];
};

struct L10struct{
#pragma 10
REAL (*dptr)[nc][nyL10][nxL10][ncube][ncube];
};



void ctoprim(int lo,int hi,int ng, double (*u)[nyL0][nxL0][ncube][ncube], double (*q)[nyL0][nxL0][ncube][ncube], double *dx,double *courno)
{
  int i, icube;
  int j, jcube;
  double c;
  double eint;
  double courx;
  double coury;
  double courz;
  double courmx;
  double courmy;
  double courmz;
  double rhoinv;
  double dx1inv;
  double dx2inv;
  double dx3inv;
  double CVinv;
  double GAMMA = 1.4e0;
  double CV = 8.3333333333e6;
  CVinv = 1.0e0 / CV;
    for (jcube = 0; jcube < nyL0; ++jcube)
    for (icube = 0; icube < nyL0; ++icube)
    for (j = 0; j < ncube; j = j + 1) {
      for (i = 0; i < ncube; i = i + 1) {
             rhoinv     = 1.0e0/u[0][jcube][icube][j][i];
             q[0][jcube][icube][j][i] = u[0][jcube][icube][j][i];
             q[1][jcube][icube][j][i] = u[1][jcube][icube][j][i]*rhoinv;
             q[2][jcube][icube][j][i] = u[2][jcube][icube][j][i]*rhoinv;
             q[3][jcube][icube][j][i] = u[3][jcube][icube][j][i]*rhoinv;

             eint = u[4][jcube][icube][j][i]*rhoinv - 0.5e0*(pow(q[1][jcube][icube][j][i],2) + pow(q[2][jcube][icube][j][i],2) + pow(q[3][jcube][icube][j][i],2));
             q[4][jcube][icube][j][i] = (GAMMA-1.e0)*eint*u[0][jcube][icube][j][i];
             q[5][jcube][icube][j][i] = eint * CVinv;
      }
    }
  if (*courno != 0.0) {
    courmx = -DBL_MAX;
    courmy = -DBL_MAX;
    courmz = -DBL_MAX;
    dx1inv = 1.0e0 / dx[0];
    dx2inv = 1.0e0 / dx[1];
    dx3inv = 1.0e0 / dx[2];
    for (j = 0; j < ncube; j = j + 1) {
      for (i = 0; i < ncube; i = i + 1) {
        c     = sqrt(GAMMA*q[4][1][1][j][i]/q[0][1][1][j][i]);
        courx = ( c+abs(q[1][1][1][j][i]) ) * dx1inv;
        coury = ( c+abs(q[2][1][1][j][i]) ) * dx2inv;
        courz = ( c+abs(q[3][1][1][j][i]) ) * dx3inv;
        courmx = (courmx > courx?courmx : courx);
        courmy = (courmy > coury?courmy : coury);
        courmz = (courmz > courz?courmz : courz);
      }
    }
  *courno = max( max( max(courmx, courmy), courmz), *courno);
  }
  else {
  }
}

void hypterm(int lo,int hi,int ng,double* dx,double (*cons)[nyL0][nxL0][ncube][ncube],double (*q)[nyL0][nxL0][ncube][ncube],double (*flux)[nyL0][nxL0][ncube][ncube])
{
  int irho = 1;
  int imx = 2;
  int imy = 3;
  int imz = 4;
  int iene = 5;
  int qu = 2;
  int qv = 3;
  int qw = 4;
  int qpres = 5;
  double ALP = 0.8e0;
  double BET = - 0.2e0;
  double GAM = 4.e0 / 105.e0;
  double DEL = -(1.e0 / 280.e0);
  int i, icube;
  int j, jcube;
  double unp1;
  double unp2;
  double unp3;
  double unp4;
  double unm1;
  double unm2;
  double unm3;
  double unm4;
  double dxinv[3];
  for (i = 1; i <= 3; i = i + 1) {
    dxinv[i - 1] = 1.0e0 / dx[i-1];
  }
    for (j = 0; j < ncube; j = j + 1) {
      for (i = 0; i < ncube; i = i + 1) {
        unp1 = (i+1 >= ncube) ? q[qu][1][2][j][i-ncube] : q[qu][1][1][j][i+1];
        unp2 = (i+2 >= ncube) ? q[qu][1][2][j][i-ncube] : q[qu][1][1][j][i+2];
        unp3 = (i+3 >= ncube) ? q[qu][1][2][j][i-ncube] : q[qu][1][1][j][i+3];
        unp4 = (i+4 >= ncube) ? q[qu][1][2][j][i-ncube] : q[qu][1][1][j][i+4];
        unm1 = (i-1 < 0) ? q[qu][1][0][j][i+ncube] : q[qu][1][1][j][i-1];
        unm2 = (i-2 < 0) ? q[qu][1][0][j][i+ncube] : q[qu][1][1][j][i-2];
        unm3 = (i-3 < 0) ? q[qu][1][0][j][i+ncube] : q[qu][1][1][j][i-3];
        unm4 = (i-4 < 0) ? q[qu][1][0][j][i+ncube] : q[qu][1][1][j][i-4];
        flux[irho][1][1][j][i]= - \
              (ALP*(((i+1 >= ncube) ? cons[imx][1][2][j][i-ncube] : cons[imx][1][1][j][i+1])-((i-1 < 0) ? cons[imy][1][0][j][i+ncube] : cons[imx][1][1][j][i-1])) \
             + BET*(((i+2 >= ncube) ? cons[imx][1][2][j][i-ncube] : cons[imx][1][1][j][i+2])-((i-2 < 0) ? cons[imy][1][0][j][i+ncube] : cons[imx][1][1][j][i-2])) \
             + GAM*(((i+3 >= ncube) ? cons[imx][1][2][j][i-ncube] : cons[imx][1][1][j][i+3])-((i-3 < 0) ? cons[imy][1][0][j][i+ncube] : cons[imx][1][1][j][i-3])) \
             + DEL*(((i+4 >= ncube) ? cons[imx][1][2][j][i-ncube] : cons[imx][1][1][j][i+4])-((i-4 < 0) ? cons[imy][1][0][j][i+ncube] : cons[imx][1][1][j][i-4])))*dxinv[0];

        flux[imx][1][1][j][i]= - \
              (ALP*(((i+1 >= ncube) ? cons[imx][1][2][j][i-ncube] : cons[imx][1][1][j][i+1])*unp1-((i-1 < 0) ? cons[imy][1][0][j][i+ncube] : cons[imx][1][1][j][i-1])*unm1 \
             +        (((i+1 >= ncube) ? q[qpres][1][2][j][i-ncube] : q[qpres][1][1][j][i+1]) -     ((i-1 < 0) ? q[qpres][1][0][j][i+ncube] : q[qpres][1][1][j][i-1])))               \
             + BET*(((i+2 >= ncube) ? cons[imx][1][2][j][i-ncube] : cons[imx][1][1][j][i+2])*unp2-((i-2 < 0) ? cons[imy][1][0][j][i+ncube] : cons[imx][1][1][j][i-2])*unm2 \
             +        (((i+2 >= ncube) ? q[qpres][1][2][j][i-ncube] : q[qpres][1][1][j][i+2])-      ((i-2 < 0) ? q[qpres][1][0][j][i+ncube] : q[qpres][1][1][j][i-2])))               \
             + GAM*(((i+3 >= ncube) ? cons[imx][1][2][j][i-ncube] : cons[imx][1][1][j][i+3])*unp3-((i-3 < 0) ? cons[imy][1][0][j][i+ncube] : cons[imx][1][1][j][i-3])*unm3 \
             +        (((i+3 >= ncube) ? q[qpres][1][2][j][i-ncube] : q[qpres][1][1][j][i+3])-      ((i-3 < 0) ? q[qpres][1][0][j][i+ncube] : q[qpres][1][1][j][i-3])))               \
             + DEL*(((i+4 >= ncube) ? cons[imx][1][2][j][i-ncube] : cons[imx][1][1][j][i+4])*unp4-((i-4 < 0) ? cons[imy][1][0][j][i+ncube] : cons[imx][1][1][j][i-4])*unm4 \
             +        (((i+4 >= ncube) ? q[qpres][1][2][j][i-ncube] : q[qpres][1][1][j][i+4])-      ((i-4 < 0) ? q[qpres][1][0][j][i+ncube] : q[qpres][1][1][j][i-4]))))*dxinv[0];

        flux[imy][1][1][j][i]= - \
              (ALP*(((i+1 >= ncube) ? cons[imy][1][2][j][i-ncube] : cons[imy][1][1][j][i+1])*unp1-((i-1 < 0) ? cons[imy][1][0][j][i+ncube] : cons[imy][1][1][j][i-1])*unm1) \
             + BET*(((i+2 >= ncube) ? cons[imy][1][2][j][i-ncube] : cons[imy][1][1][j][i+2])*unp2-((i-2 < 0) ? cons[imy][1][0][j][i+ncube] : cons[imy][1][1][j][i-2])*unm2) \
             + GAM*(((i+3 >= ncube) ? cons[imy][1][2][j][i-ncube] : cons[imy][1][1][j][i+3])*unp3-((i-3 < 0) ? cons[imy][1][0][j][i+ncube] : cons[imy][1][1][j][i-3])*unm3) \
             + DEL*(((i+4 >= ncube) ? cons[imy][1][2][j][i-ncube] : cons[imy][1][1][j][i+4])*unp4-((i-4 < 0) ? cons[imy][1][0][j][i+ncube] : cons[imy][1][1][j][i-4])*unm4))*dxinv[0];

        flux[imz][1][1][j][i]= - \
              (ALP*(((i+1 >= ncube) ? cons[imz][1][2][j][i-ncube] : cons[imy][1][1][j][i+1])*unp1-((i-1 < 0) ? cons[imz][1][0][j][i+ncube] : cons[imz][1][1][j][i-1])*unm1) \
             + BET*(((i+1 >= ncube) ? cons[imz][1][2][j][i-ncube] : cons[imy][1][1][j][i+1])*unp2-((i-1 < 0) ? cons[imz][1][0][j][i+ncube] : cons[imz][1][1][j][i-1])*unm2) \
             + GAM*(((i+1 >= ncube) ? cons[imz][1][2][j][i-ncube] : cons[imy][1][1][j][i+1])*unp3-((i-1 < 0) ? cons[imz][1][0][j][i+ncube] : cons[imz][1][1][j][i-1])*unm3) \
             + DEL*(((i+1 >= ncube) ? cons[imz][1][2][j][i-ncube] : cons[imy][1][1][j][i+1])*unp4-((i-1 < 0) ? cons[imz][1][0][j][i+ncube] : cons[imz][1][1][j][i-1])*unm4))*dxinv[0];

        flux[iene][1][1][j][i]= - \
              (ALP*(((i+1 >= ncube) ? cons[iene][1][2][j][i-ncube] : cons[iene][1][1][j][i+1])*unp1-((i-1 < 0) ? cons[imy][1][0][j][i+ncube] : cons[iene][1][1][j][i-1])*unm1 \
             +        (((i+1 >= ncube) ? q[qpres][1][2][j][i-ncube] : q[qpres][1][1][j][i+1]) -     ((i-1 < 0) ? q[qpres][1][0][j][i+ncube] : q[qpres][1][1][j][i-1])))               \
             + BET*(((i+2 >= ncube) ? cons[iene][1][2][j][i-ncube] : cons[iene][1][1][j][i+2])*unp2-((i-2 < 0) ? cons[imy][1][0][j][i+ncube] : cons[iene][1][1][j][i-2])*unm2 \
             +        (((i+2 >= ncube) ? q[qpres][1][2][j][i-ncube] : q[qpres][1][1][j][i+2])-      ((i-2 < 0) ? q[qpres][1][0][j][i+ncube] : q[qpres][1][1][j][i-2])))               \
             + GAM*(((i+3 >= ncube) ? cons[iene][1][2][j][i-ncube] : cons[iene][1][1][j][i+3])*unp3-((i-3 < 0) ? cons[imy][1][0][j][i+ncube] : cons[iene][1][1][j][i-3])*unm3 \
             +        (((i+3 >= ncube) ? q[qpres][1][2][j][i-ncube] : q[qpres][1][1][j][i+3])-      ((i-3 < 0) ? q[qpres][1][0][j][i+ncube] : q[qpres][1][1][j][i-3])))               \
             + DEL*(((i+4 >= ncube) ? cons[iene][1][2][j][i-ncube] : cons[iene][1][1][j][i+4])*unp4-((i-4 < 0) ? cons[imy][1][0][j][i+ncube] : cons[iene][1][1][j][i-4])*unm4 \
             +        (((i+4 >= ncube) ? q[qpres][1][2][j][i-ncube] : q[qpres][1][1][j][i+4])-      ((i-4 < 0) ? q[qpres][1][0][j][i+ncube] : q[qpres][1][1][j][i-4]))))*dxinv[0];
      }
    }
    for (j = 0; j < ncube; j = j + 1) {
      for (i = 0; i < ncube; i = i + 1) {
        unp1 = (j+1 >= ncube) ? q[qv][2][1][j][i-ncube] : q[qv][1][1][j+1][i];
        unp2 = (j+2 >= ncube) ? q[qv][2][1][j][i-ncube] : q[qv][1][1][j+2][i];
        unp3 = (j+3 >= ncube) ? q[qv][2][1][j][i-ncube] : q[qv][1][1][j+3][i];
        unp4 = (j+4 >= ncube) ? q[qv][2][1][j][i-ncube] : q[qv][1][1][j+4][i];
        unm1 = (j-1 < 0) ? q[qv][0][1][j][i+ncube] : q[qv][1][1][j-1][i];
        unm2 = (j-2 < 0) ? q[qv][0][1][j][i+ncube] : q[qv][1][1][j-2][i];
        unm3 = (j-3 < 0) ? q[qv][0][1][j][i+ncube] : q[qv][1][1][j-3][i];
        unm4 = (j-4 < 0) ? q[qv][0][1][j][i+ncube] : q[qv][1][1][j-4][i];
        flux[irho][1][1][j][i]= - \
              (ALP*(((j+1 >= ncube) ? cons[imx][2][1][j-ncube][i] : cons[imx][1][1][j+1][i])-((j-1 < 0) ? cons[imy][0][1][j+ncube][i] : cons[imx][1][1][j-1][i])) \
             + BET*(((j+2 >= ncube) ? cons[imx][2][1][j-ncube][i] : cons[imx][1][1][j+2][i])-((j-2 < 0) ? cons[imy][0][1][j+ncube][i] : cons[imx][1][1][j-2][i])) \
             + GAM*(((j+3 >= ncube) ? cons[imx][2][1][j-ncube][i] : cons[imx][1][1][j+3][i])-((j-3 < 0) ? cons[imy][0][1][j+ncube][i] : cons[imx][1][1][j-3][i])) \
             + DEL*(((j+4 >= ncube) ? cons[imx][2][1][j-ncube][i] : cons[imx][1][1][j+4][i])-((j-4 < 0) ? cons[imy][0][1][j+ncube][i] : cons[imx][1][1][j-4][i])))*dxinv[0];

        flux[imx][1][1][j][i]= - \
              (ALP*(((j+1 >= ncube) ? cons[imx][2][1][j-icube][i] : cons[imx][1][1][j+1][i])*unp1-((j-1 < 0) ? cons[imy][0][1][j+icube][i] : cons[imx][1][1][j-1][i])*unm1 \
             +        (((j+1 >= ncube) ? q[qpres][2][1][j-icube][i] : q[qpres][1][1][j+1][i]) -     ((j-1 < 0) ? q[qpres][0][1][j+icube][i] : q[qpres][1][1][j-1][i])))               \
             + BET*(((j+2 >= ncube) ? cons[imx][2][1][j-icube][i] : cons[imx][1][1][j+2][i])*unp2-((j-2 < 0) ? cons[imy][0][1][j+icube][i] : cons[imx][1][1][j-2][i])*unm2 \
             +        (((j+2 >= ncube) ? q[qpres][2][1][j-icube][i] : q[qpres][1][1][j+2][i])-      ((j-2 < 0) ? q[qpres][0][1][j+icube][i] : q[qpres][1][1][j-2][i])))               \
             + GAM*(((j+3 >= ncube) ? cons[imx][2][1][j-icube][i] : cons[imx][1][1][j+3][i])*unp3-((j-3 < 0) ? cons[imy][0][1][j+icube][i] : cons[imx][1][1][j-3][i])*unm3 \
             +        (((j+3 >= ncube) ? q[qpres][2][1][j-icube][i] : q[qpres][1][1][j+3][i])-      ((j-3 < 0) ? q[qpres][0][1][j+icube][i] : q[qpres][1][1][j-3][i])))               \
             + DEL*(((j+4 >= ncube) ? cons[imx][2][1][j-icube][i] : cons[imx][1][1][j+4][i])*unp4-((j-4 < 0) ? cons[imy][0][1][j+icube][i] : cons[imx][1][1][j-4][i])*unm4 \
             +        (((j+4 >= ncube) ? q[qpres][2][1][j-icube][i] : q[qpres][1][1][j+4][i])-      ((j-4 < 0) ? q[qpres][0][1][j+icube][i] : q[qpres][1][1][j-4][i]))))*dxinv[0];

        flux[imy][1][1][j][i]= - \
              (ALP*(((j+1 >= ncube) ? cons[imy][2][1][j-ncube][i] : cons[imy][1][1][j+1][i])-((j-1 < 0) ? cons[imy][0][1][j+ncube][i] : cons[imy][1][1][j-1][i])) \
             + BET*(((j+2 >= ncube) ? cons[imy][2][1][j-ncube][i] : cons[imy][1][1][j+2][i])-((j-2 < 0) ? cons[imy][0][1][j+ncube][i] : cons[imy][1][1][j-2][i])) \
             + GAM*(((j+3 >= ncube) ? cons[imy][2][1][j-ncube][i] : cons[imy][1][1][j+3][i])-((j-3 < 0) ? cons[imy][0][1][j+ncube][i] : cons[imy][1][1][j-3][i])) \
             + DEL*(((j+4 >= ncube) ? cons[imy][2][1][j-ncube][i] : cons[imy][1][1][j+4][i])-((j-4 < 0) ? cons[imy][0][1][j+ncube][i] : cons[imy][1][1][j-4][i])))*dxinv[0];

        flux[imz][1][1][j][i]= - \
              (ALP*(((j+1 >= ncube) ? cons[imz][2][1][j-ncube][i] : cons[imz][1][1][j+1][i])-((j-1 < 0) ? cons[imz][0][1][j+ncube][i] : cons[imz][1][1][j-1][i])) \
             + BET*(((j+2 >= ncube) ? cons[imz][2][1][j-ncube][i] : cons[imz][1][1][j+2][i])-((j-2 < 0) ? cons[imz][0][1][j+ncube][i] : cons[imz][1][1][j-2][i])) \
             + GAM*(((j+3 >= ncube) ? cons[imz][2][1][j-ncube][i] : cons[imz][1][1][j+3][i])-((j-3 < 0) ? cons[imz][0][1][j+ncube][i] : cons[imz][1][1][j-3][i])) \
             + DEL*(((j+4 >= ncube) ? cons[imz][2][1][j-ncube][i] : cons[imz][1][1][j+4][i])-((j-4 < 0) ? cons[imz][0][1][j+ncube][i] : cons[imz][1][1][j-4][i])))*dxinv[0];

        flux[iene][1][1][j][i]= - \
              (ALP*(((j+1 >= ncube) ? cons[iene][2][1][j-icube][i] : cons[iene][1][1][j+1][i])*unp1-((j-1 < 0) ? cons[imy][0][1][j+icube][i] : cons[iene][1][1][j-1][i])*unm1 \
             +        (((j+1 >= ncube) ? q[qpres][2][1][j-icube][i] : q[qpres][1][1][j+1][i]) -     ((j-1 < 0) ? q[qpres][0][1][j+icube][i] : q[qpres][1][1][j-1][i])))               \
             + BET*(((j+2 >= ncube) ? cons[iene][2][1][j-icube][i] : cons[iene][1][1][j+2][i])*unp2-((j-2 < 0) ? cons[imy][0][1][j+icube][i] : cons[iene][1][1][j-2][i])*unm2 \
             +        (((j+2 >= ncube) ? q[qpres][2][1][j-icube][i] : q[qpres][1][1][j+2][i])-      ((j-2 < 0) ? q[qpres][0][1][j+icube][i] : q[qpres][1][1][j-2][i])))               \
             + GAM*(((j+3 >= ncube) ? cons[iene][2][1][j-icube][i] : cons[iene][1][1][j+3][i])*unp3-((j-3 < 0) ? cons[imy][0][1][j+icube][i] : cons[iene][1][1][j-3][i])*unm3 \
             +        (((j+3 >= ncube) ? q[qpres][2][1][j-icube][i] : q[qpres][1][1][j+3][i])-      ((j-3 < 0) ? q[qpres][0][1][j+icube][i] : q[qpres][1][1][j-3][i])))               \
             + DEL*(((j+4 >= ncube) ? cons[iene][2][1][j-icube][i] : cons[iene][1][1][j+4][i])*unp4-((j-4 < 0) ? cons[imy][0][1][j+icube][i] : cons[iene][1][1][j-4][i])*unm4 \
             +        (((j+4 >= ncube) ? q[qpres][2][1][j-icube][i] : q[qpres][1][1][j+4][i])-      ((j-4 < 0) ? q[qpres][0][1][j+icube][i] : q[qpres][1][1][j-4][i]))))*dxinv[0];
      }
    }
}

int compute(REAL* dx, REAL dt, REAL cfl,REAL eta,REAL alam)
{
  // double-buffer (ld, st)
  struct L10struct L10data;
  L10data.dptr = (REAL (*)[nc][nyL10][nxL10][ncube][ncube]) malloc(sizeof(REAL)*nxL10*nyL10*ncube*ncube*nc*2);

  int i_l10_load = 0;
  int i_l10_comp = 1;
  // initailize data with linearalized values
  //initData(nxL10, nyL10, &L10data.dptr[i_l10_load]);

/******   starting Level-10    ******/
  struct L9struct L9data;
  L9data.dptr = (REAL (*)[nc][nyL9][nxL9][ncube][ncube]) malloc(sizeof(REAL)*nxL9*nyL9*ncube*ncube*nc*2);
  int i_l9_load = 0;
  int i_l9_comp = 1;
  //prefetch first chunk
  int icomponent;
  int iL10copy;
  for(icomponent = 0; icomponent < nc; ++icomponent)
  for(iL10copy = 0; iL10copy < nyL9; ++iL10copy)
    memcpy(&L9data.dptr[i_l9_load][icomponent][iL10copy], &L10data.dptr[i_l10_load][icomponent][iL10copy], sizeof(REAL)*ncube*ncube*nyL9);
  int idx_l10, idtofetch_l10;
  int i10offset = (nyL10-2) / (ncube);
  for(idx_l10 = 0; idx_l10 < 4; idx_l10++)
  {
    idtofetch_l10 = idx_l10 + 1; 
    int idx_fetch_l10 = idtofetch_l10 / 2;
    int idy_fetch_l10 = idtofetch_l10 % 2;
    int idx_store_l10 = idx_l10 % 2;
    int idy_store_l10 = idx_l10 / 2;
    // flip double buffer index
    i_l9_load ^= 1;
    i_l9_comp ^= 1;
    // fetch for next chunk
    if(idx_l10 != 3)
    {
      //printf("copying L10 %d %d %d\n",idx_fetch_l10, idy_fetch_l10,i10offset);
      for(icomponent = 0; icomponent < nc; ++icomponent)
      for(iL10copy = 0; iL10copy < nyL9; ++iL10copy)
        memcpy(&L9data.dptr[i_l9_load][icomponent][iL10copy], &L10data.dptr[i_l10_load][icomponent][iL10copy+idx_fetch_l10*i10offset][idy_fetch_l10*i10offset], sizeof(REAL)*ncube*ncube*nyL9);
    }
    // working on the computation for next level

  {
/******   starting Level-9    ******/
  struct L8struct L8data;
  L8data.dptr = (REAL (*)[nc][nyL8][nxL8][ncube][ncube]) malloc(sizeof(REAL)*nxL8*nyL8*ncube*ncube*nc*2);
  int i_l8_load = 0;
  int i_l8_comp = 1;
  //prefetch first chunk
  int iL9copy;
  for(icomponent = 0; icomponent < nc; ++icomponent)
  for(iL9copy = 0; iL9copy < nyL8; ++iL9copy)
    memcpy(&L8data.dptr[i_l8_load][icomponent][iL9copy], &L9data.dptr[i_l9_load][icomponent][iL9copy], sizeof(REAL)*ncube*ncube*nyL8);
  int idx_l9, idtofetch_l9;
  int i9offset = (nyL9-2) / (ncube);
  for(idx_l9 = 0; idx_l9 < 4; idx_l9++)
  {
    idtofetch_l9 = idx_l9 + 1; 
    int idx_fetch_l9 = idtofetch_l9 / 2;
    int idy_fetch_l9 = idtofetch_l9 % 2;
    int idx_store_l9 = idx_l9 % 2;
    int idy_store_l9 = idx_l9 / 2;
    // flip double buffer index
    i_l8_load ^= 1;
    i_l8_comp ^= 1;
    // fetch for next chunk
    if(idx_l9 != 3)
    {
      //printf("copying L9 %d %d %d\n",idx_fetch_l9, idy_fetch_l9,i9offset);
      for(icomponent = 0; icomponent < nc; ++icomponent)
      for(iL9copy = 0; iL9copy < nyL8; ++iL9copy)
        memcpy(&L8data.dptr[i_l8_load][icomponent][iL9copy], &L9data.dptr[i_l9_load][icomponent][iL9copy+idx_fetch_l9*i9offset][idy_fetch_l9*i9offset], sizeof(REAL)*ncube*ncube*nyL8);
    }
    // working on the computation for next level
  {
/******   starting Level-8    ******/
  struct L7struct L7data;
  L7data.dptr = (REAL (*)[nc][nyL7][nxL7][ncube][ncube]) malloc(sizeof(REAL)*nxL7*nyL7*ncube*ncube*nc*2);
  int i_l7_load = 0;
  int i_l7_comp = 1;
  //prefetch first chunk
  int iL8copy;
  for(icomponent = 0; icomponent < nc; ++icomponent)
  for(iL8copy = 0; iL8copy < nyL7; ++iL8copy)
    memcpy(&L7data.dptr[i_l7_load][icomponent][iL8copy], &L8data.dptr[i_l8_load][icomponent][iL8copy], sizeof(REAL)*ncube*ncube*nyL7);
  int idx_l8, idtofetch_l8;
  int i8offset = (nyL8-2) / (ncube);
  for(idx_l8 = 0; idx_l8 < 4; idx_l8++)
  {
    idtofetch_l8 = idx_l8 + 1; 
    int idx_fetch_l8 = idtofetch_l8 / 2;
    int idy_fetch_l8 = idtofetch_l8 % 2;
    int idx_store_l8 = idx_l8 % 2;
    int idy_store_l8 = idx_l8 / 2;
    // flip double buffer index
    i_l7_load ^= 1;
    i_l7_comp ^= 1;
    // fetch for next chunk
    if(idx_l8 != 3)
    {
      //printf("copying L8 %d %d %d\n",idx_fetch_l8, idy_fetch_l8,i8offset);
      for(icomponent = 0; icomponent < nc; ++icomponent)
      for(iL8copy = 0; iL8copy < nyL7; ++iL8copy)
        memcpy(&L7data.dptr[i_l7_load][icomponent][iL8copy], &L8data.dptr[i_l8_load][icomponent][iL8copy+idx_fetch_l8*i8offset][idy_fetch_l8*i8offset], sizeof(REAL)*ncube*ncube*nyL7);
    }
    // working on the computation for next level
  {
/******   starting Level-7    ******/
  struct L6struct L6data;
  L6data.dptr = (REAL (*)[nc][nyL6][nxL6][ncube][ncube]) malloc(sizeof(REAL)*nxL6*nyL6*ncube*ncube*nc*2);
  int i_l6_load = 0;
  int i_l6_comp = 1;
  //prefetch first chunk
  int iL7copy;
  for(icomponent = 0; icomponent < nc; ++icomponent)
  for(iL7copy = 0; iL7copy < nyL6; ++iL7copy)
    memcpy(&L6data.dptr[i_l6_load][icomponent][iL7copy], &L7data.dptr[i_l7_load][icomponent][iL7copy], sizeof(REAL)*ncube*ncube*nyL6);
  int idx_l7, idtofetch_l7;
  int i7offset = (nyL7-2) / (ncube);
  for(idx_l7 = 0; idx_l7 < 4; idx_l7++)
  {
    idtofetch_l7 = idx_l7 + 1; 
    int idx_fetch_l7 = idtofetch_l7 / 2;
    int idy_fetch_l7 = idtofetch_l7 % 2;
    int idx_store_l7 = idx_l7 % 2;
    int idy_store_l7 = idx_l7 / 2;
    // flip double buffer index
    i_l6_load ^= 1;
    i_l6_comp ^= 1;
    // fetch for next chunk
    if(idx_l7 != 3)
    {
      //printf("copying L7 %d %d %d\n",idx_fetch_l7, idy_fetch_l7,i7offset);
      for(icomponent = 0; icomponent < nc; ++icomponent)
      for(iL7copy = 0; iL7copy < nyL6; ++iL7copy)
        memcpy(&L6data.dptr[i_l6_load][icomponent][iL7copy], &L7data.dptr[i_l7_load][icomponent][iL7copy+idx_fetch_l7*i7offset][idy_fetch_l7*i7offset], sizeof(REAL)*ncube*ncube*nyL6);
    }
    // working on the computation for next level
  {
/******   starting Level-6    ******/
  struct L5struct L5data;
  L5data.dptr = (REAL (*)[nc][nyL5][nxL5][ncube][ncube]) malloc(sizeof(REAL)*nxL5*nyL5*ncube*ncube*nc*2);
  int i_l5_load = 0;
  int i_l5_comp = 1;
  //prefetch first chunk
  int iL6copy;
  for(icomponent = 0; icomponent < nc; ++icomponent)
  for(iL6copy = 0; iL6copy < nyL5; ++iL6copy)
    memcpy(&L5data.dptr[i_l5_load][icomponent][iL6copy], &L6data.dptr[i_l6_load][icomponent][iL6copy], sizeof(REAL)*ncube*ncube*nyL5);
  int idx_l6, idtofetch_l6;
  int i6offset = (nyL6-2) / (ncube);
  for(idx_l6 = 0; idx_l6 < 4; idx_l6++)
  {
    idtofetch_l6 = idx_l6 + 1; 
    int idx_fetch_l6 = idtofetch_l6 / 2;
    int idy_fetch_l6 = idtofetch_l6 % 2;
    int idx_store_l6 = idx_l6 % 2;
    int idy_store_l6 = idx_l6 / 2;
    // flip double buffer index
    i_l5_load ^= 1;
    i_l5_comp ^= 1;
    // fetch for next chunk
    if(idx_l6 != 3)
    {
      //printf("copying L6 %d %d %d\n",idx_fetch_l6, idy_fetch_l6,i6offset);
      for(icomponent = 0; icomponent < nc; ++icomponent)
      for(iL6copy = 0; iL6copy < nyL5; ++iL6copy)
        memcpy(&L5data.dptr[i_l5_load][icomponent][iL6copy], &L6data.dptr[i_l6_load][icomponent][iL6copy+idx_fetch_l6*i6offset][idy_fetch_l6*i6offset], sizeof(REAL)*ncube*ncube*nyL5);
    }
    // working on the computation for next level
  {
/******   starting Level-5    ******/
  struct L4struct L4data;
  L4data.dptr = (REAL (*)[nc][nyL4][nxL4][ncube][ncube]) malloc(sizeof(REAL)*nxL4*nyL4*ncube*ncube*nc*2);
  int i_l4_load = 0;
  int i_l4_comp = 1;
  //prefetch first chunk
  int iL5copy;
  for(icomponent = 0; icomponent < nc; ++icomponent)
  for(iL5copy = 0; iL5copy < nyL4; ++iL5copy)
    memcpy(&L4data.dptr[i_l4_load][icomponent][iL5copy], &L5data.dptr[i_l5_load][icomponent][iL5copy], sizeof(REAL)*ncube*ncube*nyL4);
  int idx_l5, idtofetch_l5;
  int i5offset = (nyL5-2) / (ncube);
  for(idx_l5 = 0; idx_l5 < 4; idx_l5++)
  {
    idtofetch_l5 = idx_l5 + 1; 
    int idx_fetch_l5 = idtofetch_l5 / 2;
    int idy_fetch_l5 = idtofetch_l5 % 2;
    int idx_store_l5 = idx_l5 % 2;
    int idy_store_l5 = idx_l5 / 2;
    // flip double buffer index
    i_l4_load ^= 1;
    i_l4_comp ^= 1;
    // fetch for next chunk
    if(idx_l5 != 3)
    {
      //printf("copying L5 %d %d %d\n",idx_fetch_l5, idy_fetch_l5,i5offset);
      for(icomponent = 0; icomponent < nc; ++icomponent)
      for(iL5copy = 0; iL5copy < nyL4; ++iL5copy)
        memcpy(&L4data.dptr[i_l4_load][icomponent][iL5copy], &L5data.dptr[i_l5_load][icomponent][iL5copy+idx_fetch_l5*i5offset][idy_fetch_l5*i5offset], sizeof(REAL)*ncube*ncube*nyL4);
    }
    // working on the computation for next level
  {
/******   starting Level-4    ******/
  struct L3struct L3data;
  L3data.dptr = (REAL (*)[nc][nyL3][nxL3][ncube][ncube]) malloc(sizeof(REAL)*nxL3*nyL3*ncube*ncube*nc*2);
  int i_l3_load = 0;
  int i_l3_comp = 1;
  //prefetch first chunk
  int iL4copy;
  for(icomponent = 0; icomponent < nc; ++icomponent)
  for(iL4copy = 0; iL4copy < nyL3; ++iL4copy)
    memcpy(&L3data.dptr[i_l3_load][icomponent][iL4copy], &L4data.dptr[i_l4_load][icomponent][iL4copy], sizeof(REAL)*ncube*ncube*nyL3);
  int idx_l4, idtofetch_l4;
  int i4offset = (nyL4-2) / (ncube);
  for(idx_l4 = 0; idx_l4 < 4; idx_l4++)
  {
    idtofetch_l4 = idx_l4 + 1; 
    int idx_fetch_l4 = idtofetch_l4 / 2;
    int idy_fetch_l4 = idtofetch_l4 % 2;
    int idx_store_l4 = idx_l4 % 2;
    int idy_store_l4 = idx_l4 % 2;
    // flip double buffer index
    i_l3_load ^= 1;
    i_l3_comp ^= 1;
    // fetch for next chunk
    if(idx_l4 != 3)
    {
      //printf("copying L4 %d %d %d\n",idx_fetch_l4, idy_fetch_l4,i4offset);
      for(icomponent = 0; icomponent < nc; ++icomponent)
      for(iL4copy = 0; iL4copy < nyL3; ++iL4copy)
        memcpy(&L3data.dptr[i_l3_load][icomponent][iL4copy], &L4data.dptr[i_l4_load][icomponent][iL4copy+idx_fetch_l4*i4offset][idy_fetch_l4*i4offset], sizeof(REAL)*ncube*ncube*nyL3);
    }
    // working on the computation for next level
  {
/******   starting Level-3    ******/
  struct L2struct L2data;
  L2data.dptr = (REAL (*)[nc][nyL2][nxL2][ncube][ncube]) malloc(sizeof(REAL)*nxL2*nyL2*ncube*ncube*nc*2);
  int i_l2_load = 0;
  int i_l2_comp = 1;
  //prefetch first chunk
  int iL3copy;
  for(icomponent = 0; icomponent < nc; ++icomponent)
  for(iL3copy = 0; iL3copy < nyL2; ++iL3copy)
    memcpy(&L2data.dptr[i_l2_load][icomponent][iL3copy], &L3data.dptr[i_l3_load][icomponent][iL3copy], sizeof(REAL)*ncube*ncube*nyL2);
  int idx_l3, idtofetch_l3;
  int i3offset = (nyL3-2) / (ncube);
  for(idx_l3 = 0; idx_l3 < 4; idx_l3++)
  {
    idtofetch_l3 = idx_l3 + 1; 
    int idx_fetch_l3 = idtofetch_l3 / 2;
    int idy_fetch_l3 = idtofetch_l3 % 2;
    int idx_store_l3 = idx_l3 % 2;
    int idy_store_l3 = idx_l3 / 2;
    // flip double buffer index
    i_l2_load ^= 1;
    i_l2_comp ^= 1;
    // fetch for next chunk
    if(idx_l3 != 3)
    {
      //printf("copying L3 %d %d %d\n",idx_fetch_l3, idy_fetch_l3,i3offset);
      for(icomponent = 0; icomponent < nc; ++icomponent)
      for(iL3copy = 0; iL3copy < nyL2; ++iL3copy)
        memcpy(&L2data.dptr[i_l2_load][icomponent][iL3copy], &L3data.dptr[i_l3_load][icomponent][iL3copy+idx_fetch_l3*i3offset][idy_fetch_l3*i3offset], sizeof(REAL)*ncube*ncube*nyL2);
    }
    // working on the computation for next level
  {
/******   starting Level-2    ******/
  struct L1struct L1data;
  L1data.dptr = (REAL (*)[nc][nyL1][nxL1][ncube][ncube]) malloc(sizeof(REAL)*ncube*ncube*nxL1*nyL1*nc*2);
  int i_l1_load = 0;
  int i_l1_comp = 1;
  //prefetch first chunk
  int iL2copy;
  for(icomponent = 0; icomponent < nc; ++icomponent)
  for(iL2copy = 0; iL2copy < nyL1; ++iL2copy)
    memcpy(&L1data.dptr[i_l1_load][icomponent][iL2copy], &L2data.dptr[i_l2_load][icomponent][iL2copy], sizeof(REAL)*ncube*ncube*nyL1);
  int idx_l2, idtofetch_l2;
  int i2offset = (nyL2-2) / (ncube);
  for(idx_l2 = 0; idx_l2 < 4; idx_l2++)
  {
    idtofetch_l2 = idx_l2 + 1; 
    int idx_fetch_l2 = idtofetch_l2 / 2;
    int idy_fetch_l2 = idtofetch_l2 % 2;
    int idx_store_l2 = idx_l2 % 2;
    int idy_store_l2 = idx_l2 / 2;
    // flip double buffer index
    i_l1_load ^= 1;
    i_l1_comp ^= 1;
    // fetch for next chunk
    if(idx_l2 != 3)
    {
      //printf("copying L2 %d %d %d\n",idx_fetch_l2, idy_fetch_l2,i2offset);
      for(icomponent = 0; icomponent < nc; ++icomponent)
      for(iL2copy = 0; iL2copy < nyL1; ++iL2copy)
        memcpy(&L1data.dptr[i_l1_load][icomponent][iL2copy], &L2data.dptr[i_l2_load][icomponent][iL2copy+idx_fetch_l2*i2offset][idy_fetch_l2*i2offset], sizeof(REAL)*ncube*ncube*nyL1);
    }
    // working on the computation for next level
  {
/******   starting Level-1    ******/
  struct L0struct L0data;
  L0data.dptr = (REAL (*)[nc][nyL0][nxL0][ncube][ncube]) malloc(sizeof(REAL)*ncube*ncube*nxL0*nyL0*nc*3);
  int i_l0_load = 0;
  int i_l0_comp = 1;
  int i_l0_store = 2;
  //prefetch first chunk
  int iL1copy;
  for(icomponent = 0; icomponent < nc; ++icomponent)
  for(iL1copy = 0; iL1copy < nyL0; ++iL1copy)
    memcpy(&L0data.dptr[i_l0_load][icomponent][iL1copy], &L1data.dptr[i_l1_load][icomponent][iL1copy], sizeof(REAL)*ncube*ncube*nyL0);
  int idx_l1, idtofetch_l1;
  int i1offset = (nyL1-2) / (ncube);
  for(idx_l1 = 0; idx_l1 < 4; idx_l1++)
  {
    idtofetch_l1 = idx_l1 + 1; 
    int idx_fetch_l1 = idtofetch_l1 / 2;
    int idy_fetch_l1 = idtofetch_l1 % 2;
    int idx_store_l1 = idx_l1 % 2;
    int idy_store_l1 = idx_l1 / 2;
    // rotate triple buffer index only for L0
    i_l0_load = (i_l0_load == 0) ? 2:(i_l0_load-1);
    i_l0_comp = (i_l0_comp == 0) ? 2:(i_l0_comp-1);
    i_l0_store = (i_l0_store == 0) ? 2:(i_l0_store-1);
    // fetch for next chunk
    if(idx_l1 != 3)
    {
      //printf("copying L1 %d %d %d\n",idx_fetch_l1, idy_fetch_l1,i1offset);
      for(icomponent = 0; icomponent < nc; ++icomponent)
      for(iL1copy = 0; iL1copy < nyL0; ++iL1copy)
        memcpy(&L0data.dptr[i_l0_load][icomponent][iL1copy], &L1data.dptr[i_l1_load][icomponent][iL1copy+idx_fetch_l1*i1offset][idy_fetch_l1*i1offset], sizeof(REAL)*ncube*ncube*nyL0);
    }
    // working on the computation for final level
    //computeL0(L0data.dptr, i_l0_comp, i_l0_store);
    REAL courno_proc = 1.0e-50;
    REAL courno_zero = 0.0;
    int m, i, j;
#pragma 0
    REAL (*up)[nyL0][nxL0][ncube][ncube] = &L0data.dptr[i_l0_load][0];
#pragma 0
    REAL (*qp)[nyL0][nxL0][ncube][ncube] = (REAL (*)[nxL0][nyL0][ncube][ncube]) malloc(sizeof(REAL)*ncube*ncube*nxL0*nyL0*(nc+1));
#pragma 0
    REAL (*dp)[nyL0][nxL0][ncube][ncube] = (REAL (*)[nxL0][nyL0][ncube][ncube]) malloc(sizeof(REAL)*ncube*ncube*nxL0*nyL0*nc);
#pragma 0
    REAL (*fp)[nyL0][nxL0][ncube][ncube] = (REAL (*)[nxL0][nyL0][ncube][ncube]) malloc(sizeof(REAL)*ncube*ncube*nxL0*nyL0*nc);
#pragma 0
    REAL (*unp)[nyL0][nxL0][ncube][ncube] = (REAL (*)[nxL0][nyL0][ncube][ncube]) malloc(sizeof(REAL)*ncube*ncube*nxL0*nyL0*nc);
    ctoprim(0, ncube, nghost, up, qp, dx, &courno_proc);
    //diffterm(0, ncube, nghost, dx, qp, da, eta, alam);
    hypterm(0, ncube, nghost, dx, up, qp, fp);
    for(m=0; m < nc ; ++m)
    for (j = 0; j < ncube; j = j + 1) {
      for (i = 0; i < ncube; i = i + 1) {
        unp[m][1][1][j][i] = up[m][1][1][j][i] + dt * (dp[m][1][1][j][i] + fp[m][1][1][j][i]);
      }
    }
    ctoprim(0, ncube, nghost, up, qp, dx, &courno_zero);
    //diffterm(0, ncube, nghost, dx, qp, da, eta, alam);
    hypterm(0, ncube, nghost, dx, up, qp, fp);
    for(m=0; m < nc ; ++m)
    for (j = 0; j < ncube; j = j + 1) {
      for (i = 0; i < ncube; i = i + 1) {
        unp[m][1][1][j][i] = ThreeQuarters * up[m][1][1][j][i] + \
        OneQuarter * ( unp[m][1][1][j][i] + dt * (dp[m][1][1][j][i] + fp[m][1][1][j][i]));
      }
    }
    ctoprim(0, ncube, nghost, up, qp, dx, &courno_zero);
    //diffterm(0, ncube, nghost, dx, qp, da, eta, alam);
    hypterm(0, ncube, nghost, dx, up, qp, fp);
    for(m=0; m < nc ; ++m)
    for (j = 0; j < ncube; j = j + 1) {
      for (i = 0; i < ncube; i = i + 1) {
        L0data.dptr[i_l0_store][m][1][1][j][i] = OneThird * up[m][1][1][j][i] + \
        TwoThirds * ( unp[m][1][1][j][i] + dt * (dp[m][1][1][j][i] + fp[m][1][1][j][i]));
      }
    }
 
    // storing back the computed result
    //printf("storing back to L1 %d %d\n",idx_store_l1,idy_store_l1);
  for(icomponent = 0; icomponent < nc; ++icomponent)
    for(iL1copy = 0; iL1copy < nyL0-1; ++iL1copy)
      memcpy(&L1data.dptr[i_l1_comp][icomponent][iL1copy+idy_store_l1*i1offset+1][idx_store_l1*i1offset+1], &L0data.dptr[i_l0_store][icomponent][iL1copy+1][1], sizeof(REAL)*ncube*ncube*(nyL0-1));
  }
  free(L0data.dptr);
/******   end Level-1    ******/
  } 
 
    //printf("storing back to L2 %d %d\n",idx_store_l2,idy_store_l2);
  for(icomponent = 0; icomponent < nc; ++icomponent)
    for(iL2copy = 0; iL2copy < nyL1-2; ++iL2copy)
      memcpy(&L2data.dptr[i_l2_comp][icomponent][iL2copy+idy_store_l2*i2offset+1][idx_store_l2*i2offset+1], &L1data.dptr[i_l1_comp][icomponent][iL2copy+1][1], sizeof(REAL)*ncube*ncube*(nyL1-2));
  }
  free(L1data.dptr);
/******   end Level-2    ******/
  } 
 
    //printf("storing back to L3 %d %d\n",idx_store_l3,idy_store_l3);
  for(icomponent = 0; icomponent < nc; ++icomponent)
    for(iL3copy = 0; iL3copy < nyL2-2; ++iL3copy)
      memcpy(&L3data.dptr[i_l3_comp][icomponent][iL3copy+idy_store_l3*i3offset+1][idx_store_l3*i3offset+1], &L2data.dptr[i_l2_comp][icomponent][iL3copy+1][1], sizeof(REAL)*ncube*ncube*(nyL2-2));
  }
  free(L2data.dptr);
/******   end Level-3    ******/
  } 
 
    //printf("storing back to L4 %d %d\n",idx_store_l4,idy_store_l4);
  for(icomponent = 0; icomponent < nc; ++icomponent)
    for(iL4copy = 0; iL4copy < nyL3-2; ++iL4copy)
      memcpy(&L4data.dptr[i_l4_comp][icomponent][iL4copy+idy_store_l4*i4offset+1][idx_store_l4*i4offset+1], &L3data.dptr[i_l3_comp][icomponent][iL4copy+1][1], sizeof(REAL)*ncube*ncube*(nyL3-2));
  }
  free(L3data.dptr);
/******   end Level-4    ******/
  } 
 
    //printf("storing back to L5 %d %d\n",idx_store_l5,idy_store_l5);
  for(icomponent = 0; icomponent < nc; ++icomponent)
    for(iL5copy = 0; iL5copy < nyL4-2; ++iL5copy)
      memcpy(&L5data.dptr[i_l5_comp][icomponent][iL5copy+idy_store_l5*i5offset+1][idx_store_l5*i5offset+1], &L4data.dptr[i_l4_comp][icomponent][iL5copy+1][1], sizeof(REAL)*ncube*ncube*(nyL4-2));
  }
  free(L4data.dptr);
/******   end Level-5    ******/
  } 
 
    //printf("storing back to L6 %d %d\n",idx_store_l6,idy_store_l6);
  for(icomponent = 0; icomponent < nc; ++icomponent)
    for(iL6copy = 0; iL6copy < nyL5-2; ++iL6copy)
      memcpy(&L6data.dptr[i_l6_comp][icomponent][iL6copy+idy_store_l6*i6offset+1][idx_store_l6*i6offset+1], &L5data.dptr[i_l5_comp][icomponent][iL6copy+1][1], sizeof(REAL)*ncube*ncube*(nyL5-2));
  }
  free(L5data.dptr);
/******   end Level-6    ******/
  } 
 
    //printf("storing back to L7 %d %d\n",idx_store_l7,idy_store_l7);
  for(icomponent = 0; icomponent < nc; ++icomponent)
    for(iL7copy = 0; iL7copy < nyL6-2; ++iL7copy)
      memcpy(&L7data.dptr[i_l7_comp][icomponent][iL7copy+idy_store_l7*i7offset+1][idx_store_l7*i7offset+1], &L6data.dptr[i_l6_comp][icomponent][iL7copy+1][1], sizeof(REAL)*ncube*ncube*(nyL6-2));
  }
  free(L6data.dptr);
/******   end Level-7    ******/
  } 
 
    //printf("storing back to L8 %d %d\n",idx_store_l8,idy_store_l8);
  for(icomponent = 0; icomponent < nc; ++icomponent)
    for(iL8copy = 0; iL8copy < nyL7-2; ++iL8copy)
      memcpy(&L8data.dptr[i_l8_comp][icomponent][iL8copy+idy_store_l8*i8offset+1][idx_store_l8*i8offset+1], &L7data.dptr[i_l7_comp][icomponent][iL8copy+1][1], sizeof(REAL)*ncube*ncube*(nyL7-2));
  }
  free(L7data.dptr);
/******   end Level-8    ******/
  } 
    //printf("storing back to L9 %d %d\n",idx_store_l9,idy_store_l9);
  for(icomponent = 0; icomponent < nc; ++icomponent)
    for(iL9copy = 0; iL9copy < nyL8-2; ++iL9copy)
      memcpy(&L9data.dptr[i_l9_comp][icomponent][iL9copy+idy_store_l9*i9offset+1][idx_store_l9*i9offset+1], &L8data.dptr[i_l8_comp][icomponent][iL9copy+1][1], sizeof(REAL)*ncube*ncube*(nyL8-2));
  }
  free(L8data.dptr);
/******   end Level-9    ******/
  } 
    //printf("storing back to L10 %d %d\n",idx_store_l10,idy_store_l10);
  for(icomponent = 0; icomponent < nc; ++icomponent)
    for(iL10copy = 0; iL10copy < nyL9-2; ++iL10copy)
      memcpy(&L10data.dptr[i_l10_comp][icomponent][iL10copy+idy_store_l10*i10offset+1][idx_store_l10*i10offset+1], &L9data.dptr[i_l9_comp][icomponent][iL10copy+1][1], sizeof(REAL)*ncube*ncube*(nyL9-2));
  }
  free(L9data.dptr);
/******   end Level-10    ******/
  free(L10data.dptr);
  return 0;
}
