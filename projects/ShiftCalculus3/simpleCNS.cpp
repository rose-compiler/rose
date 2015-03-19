// This is the begining of a real application CNS
// At present it only implements the stencils for 
// directionally defined first and second derivative
// discretizations for computing the solution of the
// compressible Navier-stokes equation
// There is a meaningless input, and the operators 
// are applied to it. In a real application there will 
// be boundary conditions, sensible initial conditions,
// ghost cell fill and meaningfull coefficients. Additionally
// there will be concept of multiple components in the data
// holders, where results are obtained for one component by
// applying stencil operators or pointwise functions on some
// sunset of the data holder's components.


#define BLOCKSIZE 32
#define DIM       3

#define CNUM  5

#include "simpleCNS.h"


//stencils

Stencil<double> duds[3];
Stencil<double> d2uds2[3];

///constants 
//these are vectors in the fortran but they all have the same value
//fortran code says problo = 1 in the comments then sets it to 0.1
const double s_problo  = -0.1;
const double s_probhi  =  0.1;
const int    s_numblockpower = 0;
const int    s_numblocks = pow(2, s_numblockpower);
const int    s_ncell   =s_numblocks*BLOCKSIZE;
const double s_dx      = (s_probhi-s_problo)/s_ncell;

const int    s_nstep   = 10;

const double s_cfl     = 0.5;

//diffusion coefficients
const double s_eta  = 1.0e-4;
const double s_alam = 1.5e2;
//ideal gas constant
const double s_gamma = 1.4;
//pi
double s_pi = 4.*atan(1.0);
//number of ghost
const int    s_nghost = 4;

//coefficient thermal expansion at constant volume
const double s_CV  = 8.3333333333e6;

////////////
void
parseCommandLine(int argc, char* argv[])
{
#if 0
  cout<< "dim and blocksize set in SPACE.H" << endl;
  cout << "usage: CNS  -l problo (default -0.1) " << endl
       << "            -h probhi (default  0.1) " << endl
       << "            -n numblockpower (default 0) " << endl
       << "            -t nsteps (default 10) " << endl
       << "            -c cfl    (default 0.5)" << endl
       << "            -e eta    (default 1e-4)" << endl
       << "            -a alam   (default 1.5e2)" << endl
       << "            -g gamma  (default 1.4) " << endl
       << "            -v CV     (default 8.33e6)" << endl;

  char* ptr = NULL; //for weird strtod interface 
  for(int iarg = 0; iarg < argc-1; iarg++)
    {
      if(strcmp(argv[iarg],"-l") == 0)
        {
          s_problo = strtod(argv[iarg+1], &ptr);
        }
      else if(strcmp(argv[iarg], "-h") == 0)
        {
          s_probhi = strtod(argv[iarg+1], &ptr);
        }
      else if(strcmp(argv[iarg],"-n") == 0)
        {
          s_numblockpower = atoi(argv[iarg+1]);
          s_numblocks = pow(2, s_numblockpower);
          s_ncell   =s_numblocks*BLOCKSIZE;
        }
      else  if(strcmp(argv[iarg],"-t") == 0)
        {
          s_nstep = atoi(argv[iarg+1]);
        }
      else if(strcmp(argv[iarg], "-c") == 0)
        {
          s_cfl   = strtod(argv[iarg+1], &ptr);
        }
      else if(strcmp(argv[iarg], "-e") == 0)
        {
          s_eta   = strtod(argv[iarg+1], &ptr);
        }
      else if(strcmp(argv[iarg], "-a") == 0)
        {
          s_alam   = strtod(argv[iarg+1], &ptr);
        }
      else if(strcmp(argv[iarg], "-g") == 0)
        {
          s_gamma   = strtod(argv[iarg+1], &ptr);
        }
      else if(strcmp(argv[iarg], "-v") == 0)
        {
          s_CV   = strtod(argv[iarg+1], &ptr);
        }
    }
  cout << "problo = " << s_problo  << endl
       << "probhi = " << s_probhi  << endl
       << "ncell  = " << s_ncell   << endl
       << "nsteps = " << s_nstep   << endl
       << "cfl    = " << s_cfl     << endl
       << "eta    = " << s_eta     << endl
       << "alam   = " << s_alam    << endl
       << "gamma  = " << s_gamma   << endl
       << "CV     = " << s_CV      << endl; 
#endif
}

void initialize(RectMDArray<double, CNUM>& a_patch)
{
  //this could be made dimension-independent with more cleverness
  assert(DIM==3);

  Box D0 = a_patch.getBox();
  int k=1;
  //this is a vector in the fortran but all have the same value
  double scale = (s_probhi-s_problo)/(2.*s_pi);
  for (Point pt = D0.getLowCorner();D0.notDone(pt);D0.increment(pt))
    {
      //this is reproducing the bug in the fortran.  should be (pt+0.5)*dx
      double xloc = pt[0]*s_dx/scale;
      double yloc = pt[1]*s_dx/scale;
      double zloc = pt[2]*s_dx/scale;
      
      double uvel   = 1.1e4*sin(1.*xloc)*sin(2.*yloc)*sin(3.*zloc);
      double vvel   = 1.0e4*sin(2.*xloc)*sin(4.*yloc)*sin(1.*zloc);
      double wvel   = 1.2e4*sin(3.*xloc)*cos(2.*yloc)*sin(2.*zloc);
      double rholoc = 1.0e-3 + 1.0e-5*sin(1.*xloc)*cos(2.*yloc)*cos(3.*zloc);
      double eloc   = 2.5e9  + 1.0e-3*sin(2.*xloc)*cos(2.*yloc)*sin(2.*zloc);
      double ke = 0.5*(uvel*uvel + 
                       vvel*vvel + 
                       wvel*wvel);

      a_patch(pt, CRHO) = rholoc;
      a_patch(pt, CMOMX)= rholoc*uvel;
      a_patch(pt, CMOMY)= rholoc*vvel;
      a_patch(pt, CMOMZ)= rholoc*wvel;
      a_patch(pt, CENG) = rholoc*(eloc + ke);
    }
}
///
void initialize(LevelData<double, CNUM> & a_cons)
{
  const BoxLayout& layout = a_cons.getBoxLayout();
  for(BLIterator blit(layout); blit != blit.end(); ++blit)
    {
      initialize(a_cons[*blit]);
    }
}
///
void makeStencils()
{
#if 0
// DQ (2/12/2015): This is the older version that used non-constant declarations.
  array<Shift,DIM> S=getShiftVec();
  double D1[4], D2[5], ND1[4], ND2[5];

  // set coefficients, at the moment they are
  // not the real values to be applied to the 
  // stencil, because the operation is the same
  // and the outcome much easier to verify with
  // simpler coefficients
  D1[0] =        0.8/s_dx;
  D1[1] =       -0.2/s_dx;
  D1[2] =  4.0/105.0/s_dx;
  D1[3] = -1.0/280.0/s_dx;
  
  D2[0] = -205.0/72.0/s_dx/s_dx;
  D2[1] =     8.0/5.0/s_dx/s_dx;
  D2[2] =        -0.2/s_dx/s_dx;
  D2[3] =   8.0/315.0/s_dx/s_dx;
  D2[4] =  -1.0/560.0/s_dx/s_dx;

  for (int i=0; i<4; i++)
    {
      ND1[i]=-D1[i];
      ND2[i]= D2[i];
    }
  ND2[4]= D2[4];
#else
// DQ (2/12/2015): Made these constants to permit them to be more compile-time analizable.
  const array<Shift,DIM> S = getShiftVec();

  const double D1 [4] = { 0.8/s_dx, -0.2/s_dx, 4.0/105.0/s_dx, -1.0/280.0/s_dx };
  const double D2 [5] = { -205.0/72.0/s_dx/s_dx, 8.0/5.0/s_dx/s_dx, -0.2/s_dx/s_dx, 8.0/315.0/s_dx/s_dx, -1.0/560.0/s_dx/s_dx };
  const double ND1[4] = { -D1[0], -D1[1], -D1[2], -D1[3] };
  const double ND2[5] = {  D2[0],  D2[1],  D2[2], D2[3], D2[4] };
#endif

  // now build the stencils
  for (int dir = 0; dir < DIM; dir++)
    {
      const Point uv   = getUnitv(dir);
      const Point nuv  = uv*(-1);
      const Point zero = getZeros();

      duds[dir] = D1[0]*(S^uv)     + ND1[0]*(S^nuv)     + D1[1]*(S^(uv*2)) + ND1[1]*(S^(nuv*2)) +
                  D1[2]*(S^(uv*3)) + ND1[2]*(S^(nuv*3)) + D1[3]*(S^(uv*4)) + ND1[3]*(S^(nuv*4));

      d2uds2[dir] = D2[0]*(S^(zero)) + D2[1]*(S^uv)       + ND2[1]*(S^nuv)   + D2[2]*(S^(uv*2)) + ND2[2]*(S^(nuv*2))
                  + D2[3]*(S^(uv*3)) + ND2[3]*(S^(nuv*3)) + D2[4]*(S^(uv*4)) + ND2[4]*(S^(nuv*4));

   // this is what the stencils look like
   // cout << "dumping duds stencil "<< endl ;
      duds[dir].stencilDump();
   // cout << endl << "dumping d2uds2 stencil "<< endl ;
      d2uds2[dir].stencilDump();            
    }
}

double consToPrimF(Tensor<double,QNUM>& a_Q, CTensor<double, CNUM>& a_U)
{
  double rho  = a_U(CRHO);
  double rhou = a_U(CMOMX);
  double rhov = a_U(CMOMY);
  double rhow = a_U(CMOMZ);
  double rhoe = a_U(CENG);

  double u = rhou/rho;
  double v = rhov/rho;
  double w = rhow/rho;
  double ke = 0.5*(u*u + v*v + w*w);
  double eint = rhoe/rho - ke;
  double temp = eint/s_CV;
  double press = (s_gamma-1.)*eint*rho;
  double sound = sqrt(s_gamma*press/rho);
  
  double maxV = fabs(u)+fabs(v)+fabs(w) + sound; //apparently split schemes needs this max wave speed.
 
  a_Q(QRHO)  = rho;
  a_Q(QVELX) = u;
  a_Q(QVELY) = v;
  a_Q(QVELZ) = w;
  a_Q(QPRES) = press;
  a_Q(QTEMP) = temp;
  return maxV;
}

/*******/
void constoprim(double                         & a_maxwave,
                RectMDArray<double,QNUM>       & a_prim, 
                const RectMDArray<double,CNUM> & a_cons, 
                const Box                      & a_src)
{
  Box D0 = a_src;
  double maxwave = 0;
  for (Point pt = D0.getLowCorner();D0.notDone(pt);D0.increment(pt))
    {
      double rho  = a_cons(pt, CRHO);
      double rhou = a_cons(pt, CMOMX);
      double rhov = a_cons(pt, CMOMY);
      double rhow = a_cons(pt, CMOMZ);
      double rhoe = a_cons(pt, CENG);

      double u = rhou/rho;
      double v = rhov/rho;
      double w = rhow/rho;
      double ke = 0.5*(u*u + v*v + w*w);
      double eint = rhoe/rho - ke;
      double temp = eint/s_CV;
      double press = (s_gamma-1.)*eint*rho;
      double sound = sqrt(s_gamma*press/rho);

      double upc = fabs(u + sound);
      double umc = fabs(u - sound);
      double vpc = fabs(v + sound);
      double vmc = fabs(v - sound);
      double wpc = fabs(w + sound);
      double wmc = fabs(w - sound);

      a_prim(pt, QRHO)  = rho;
      a_prim(pt, QVELX) = u;
      a_prim(pt, QVELY) = v;
      a_prim(pt, QVELZ) = w;
      a_prim(pt, QPRES) = press;
      a_prim(pt, QTEMP) = temp;

      maxwave = max(maxwave, u  );
      maxwave = max(maxwave, upc);
      maxwave = max(maxwave, umc);
      maxwave = max(maxwave, vpc);
      maxwave = max(maxwave, vmc);
      maxwave = max(maxwave, wpc);
      maxwave = max(maxwave, wmc);
    }
  ///NEED a gather HERE!!!!
  a_maxwave = maxwave;
}
///
void setHypFlux(const RectMDArray<double,QNUM> &a_prim, RectMDArray<double,CNUM> &a_flux, const int a_dir)
{
  Box D0 = a_prim.getBox();
  for (Point pt = D0.getLowCorner();D0.notDone(pt);D0.increment(pt))
    {
      a_flux(pt, CRHO) = a_prim(pt, QRHO)*a_prim(pt, QVELX+a_dir); 
      for(int jdir=0; jdir<DIM; jdir++)
	{
	  a_flux(pt, CMOMX+jdir) = a_prim(pt, QRHO)*a_prim(pt,QVELX+a_dir)*a_prim(pt,QVELX+jdir);
	  double kine=0.;
          if(a_dir==jdir)
            {
              a_flux(pt, CMOMX+jdir) += a_prim(pt, QPRES);
              kine += a_prim(pt, QVELX+jdir)*a_prim(pt, QVELX+jdir);
            }
	  kine *=0.5;
	  double pres=a_prim(pt, QPRES);
	  double rho=a_prim(pt, QRHO);
	  double u=a_prim(pt, QVELX+a_dir);
	  double ener = pres/rho/(s_gamma - 1.0);
	  a_flux(pt,CENG)=u*rho*(kine+ener) + u*pres;
	}
    }
}
///
void hypterm(RectMDArray<double,CNUM>        & a_fp, 
             const RectMDArray<double,QNUM>  & a_prim, 
             const Box                       & a_srcBox, 
             const Box                       & a_destBox)
{
  a_fp.setVal(0.0);
  for (int dir=0;dir<DIM;dir++)
    {
      RectMDArray<double,CNUM> flux(a_srcBox);
      setHypFlux(a_prim, flux, dir);

      RectMDArray<double,CNUM> dflux(a_destBox);
      //apply stencil
      // I need to be able to specify the components on which to apply
      for(int icomp =0; icomp < CNUM; icomp++)
        {
          RectMDArray<double, 1> flscal, dfscal;

          flscal = alias<double, CNUM, 1>(flux,  Interval(icomp, icomp)); 
          dfscal = alias<double, CNUM, 1>(dflux, Interval(icomp, icomp));
	       dfscal |= duds[dir](flscal,a_destBox);// can see what goes in , and what come out.
          //Stencil<double>::apply(duds[dir], flscal, dfscal, a_destBox);
        }
      Box bx = a_srcBox;
      forall<double,CNUM,CNUM>(a_fp,dflux,[](Tensor<double,CNUM>& fp, CTensor<double,CNUM>& flux){ for(int i=0; i<CNUM; i++) fp(i)-=flux(i);},bx);
    }
}
/////
void hypterm(LevelData<double,CNUM>        & a_fp, 
             const LevelData<double,QNUM>  & a_prim)
{
  const BoxLayout& layout = a_prim.getBoxLayout();
  for(BLIterator blit(layout); blit != blit.end(); ++blit)
    {
      Box validBox = layout[*blit];
      Box ghostBox = a_prim[*blit].getBox();
      hypterm(a_fp[*blit], a_prim[*blit], ghostBox, validBox);
    }
}
////
void diffterm(LevelData<double,CNUM>       & a_dp, 
              const LevelData<double,QNUM> & a_prim)
{
  const BoxLayout& layout = a_prim.getBoxLayout();

  //velocity first derivs have to be leveldatas because they get hit with a stencil
  //so they need an exchange
  LevelData<double,1> ux(layout, s_nghost);
  LevelData<double,1> uy(layout, s_nghost);
  LevelData<double,1> uz(layout, s_nghost);
  LevelData<double,1> vx(layout, s_nghost);
  LevelData<double,1> vy(layout, s_nghost);
  LevelData<double,1> vz(layout, s_nghost);
  LevelData<double,1> wx(layout, s_nghost);
  LevelData<double,1> wy(layout, s_nghost);
  LevelData<double,1> wz(layout, s_nghost);

  //get first derivs of velocity
  for(BLIterator blit(layout); blit != blit.end(); ++blit)
    {
      
      Box ghostBox = a_prim[*blit].getBox();
      Box validBox = layout[*blit];

      const RectMDArray<double, QNUM>& prim = a_prim[*blit];

      //velocity
      RectMDArray<double,1> u = alias<double, QNUM, 1>(prim, Interval(QVELX, QVELX));
      RectMDArray<double,1> v = alias<double, QNUM, 1>(prim, Interval(QVELY, QVELY));
      RectMDArray<double,1> w = alias<double, QNUM, 1>(prim, Interval(QVELZ, QVELZ));
      Stencil<double>::apply(duds[0],  u,  ux[*blit], validBox);
      Stencil<double>::apply(duds[1],  u,  uy[*blit], validBox);
      Stencil<double>::apply(duds[2],  u,  uz[*blit], validBox);
      Stencil<double>::apply(duds[0],  v,  vx[*blit], validBox);
      Stencil<double>::apply(duds[1],  v,  vy[*blit], validBox);
      Stencil<double>::apply(duds[2],  v,  vz[*blit], validBox);
      Stencil<double>::apply(duds[0],  w,  wx[*blit], validBox);
      Stencil<double>::apply(duds[1],  w,  wy[*blit], validBox);
      Stencil<double>::apply(duds[2],  w,  wz[*blit], validBox);
    }
  ///now exchange so ghost cells of the  velocity first derivs 
  ///so they can be used to make the (mixed) second derivs.
  ux.exchange();
  uy.exchange();
  uz.exchange();
  vx.exchange();
  vy.exchange();
  vz.exchange();
  wx.exchange();
  wy.exchange();
  wz.exchange();

  //now for the big loop
  for(BLIterator blit(layout); blit != blit.end(); ++blit)
    {
      
      Box ghostBox = a_prim[*blit].getBox();
      Box validBox = layout[*blit];

      const RectMDArray<double, QNUM>& prim = a_prim[*blit];

      //velocity
      RectMDArray<double,1> u = alias<double, QNUM, 1>(prim, Interval(QVELX, QVELX));
      RectMDArray<double,1> v = alias<double, QNUM, 1>(prim, Interval(QVELY, QVELY));
      RectMDArray<double,1> w = alias<double, QNUM, 1>(prim, Interval(QVELZ, QVELZ));
      //temperature
      RectMDArray<double,1> T = alias<double, QNUM, 1>(prim, Interval(QTEMP, QTEMP));

      //our vleocity second derivs in all all their warty glory
      RectMDArray<double,1> uxx(validBox);
      RectMDArray<double,1> uyy(validBox);
      RectMDArray<double,1> uzz(validBox);
      RectMDArray<double,1> uxy(validBox);
      RectMDArray<double,1> uxz(validBox);
      RectMDArray<double,1> uyz(validBox);
      RectMDArray<double,1> vxx(validBox);
      RectMDArray<double,1> vyy(validBox);
      RectMDArray<double,1> vzz(validBox);
      RectMDArray<double,1> vxy(validBox);
      RectMDArray<double,1> vxz(validBox);
      RectMDArray<double,1> vyz(validBox);
      RectMDArray<double,1> wxx(validBox);
      RectMDArray<double,1> wyy(validBox);
      RectMDArray<double,1> wzz(validBox);
      RectMDArray<double,1> wxy(validBox);
      RectMDArray<double,1> wxz(validBox);
      RectMDArray<double,1> wyz(validBox);
      
      /// xx, yy, zz are taken directly from u
      ///mixed derivs are taken from first derivs
      Stencil<double>::apply(d2uds2[0] ,u, uxx, validBox);
      Stencil<double>::apply(d2uds2[0] ,v, vxx, validBox);
      Stencil<double>::apply(d2uds2[0] ,w, wxx, validBox);

      Stencil<double>::apply(d2uds2[1] ,u, uyy, validBox);
      Stencil<double>::apply(d2uds2[1] ,v, vyy, validBox);
      Stencil<double>::apply(d2uds2[1] ,w, wyy, validBox);

      Stencil<double>::apply(d2uds2[2] ,u, uzz, validBox);
      Stencil<double>::apply(d2uds2[2] ,v, vzz, validBox);
      Stencil<double>::apply(d2uds2[2] ,w, wzz, validBox);

      Stencil<double>::apply(duds[1], ux[*blit] ,uxy , validBox);
      Stencil<double>::apply(duds[2], ux[*blit] ,uxz , validBox);
      Stencil<double>::apply(duds[2], uy[*blit] ,uyz , validBox);
      Stencil<double>::apply(duds[1], vx[*blit] ,vxy , validBox);
      Stencil<double>::apply(duds[2], vx[*blit] ,vxz , validBox);
      Stencil<double>::apply(duds[2], vy[*blit] ,vyz , validBox);
      Stencil<double>::apply(duds[1], wx[*blit] ,wxy , validBox);
      Stencil<double>::apply(duds[2], wx[*blit] ,wxz , validBox);
      Stencil<double>::apply(duds[2], wy[*blit] ,wyz , validBox);

      //temperature derivs
      RectMDArray<double,1> Txx(validBox);
      RectMDArray<double,1> Tyy(validBox);
      RectMDArray<double,1> Tzz(validBox);

      Stencil<double>::apply(d2uds2[0], T, Txx, validBox);
      Stencil<double>::apply(d2uds2[1], T, Tyy, validBox);
      Stencil<double>::apply(d2uds2[2], T, Tzz, validBox);

      Box bx = validBox;
      double fourthirds = 4./3.;
      double onethird = 1./3.;
      for (Point pt = bx.getLowCorner(); bx.notDone(pt);bx.increment(pt))
        {
          //just called divu in the fortran 
          double divu23 = (2./3.)*(ux[*blit][pt] + vy[*blit][pt] +wz[*blit][pt]);
          //not really taus here either because no eta yet.
          double tauxx = 2*ux[*blit][pt] - divu23;
          double tauyy = 2*uy[*blit][pt] - divu23;
          double tauzz = 2*uz[*blit][pt] - divu23;

          double tauxy = uy[*blit][pt] + vx[*blit][pt];
          double tauxz = uz[*blit][pt] + wx[*blit][pt];
          double tauyz = vz[*blit][pt] + wy[*blit][pt];

      
          a_dp[*blit](pt, CRHO) = 0.;  //only energy and momentum get non-zeros
          //this weird formulation brought to you by constant coefficients
          a_dp[*blit](pt, CMOMX) = s_eta*(fourthirds*uxx[pt] + uyy[pt] + uzz[pt] + onethird*(vxy[pt] + wxz[pt]));
          a_dp[*blit](pt, CMOMY) = s_eta*(fourthirds*vyy[pt] + vxx[pt] + vzz[pt] + onethird*(uxy[pt] + wyz[pt]));
          a_dp[*blit](pt, CMOMZ) = s_eta*(fourthirds*wzz[pt] + wxx[pt] + wzz[pt] + onethird*(uxz[pt] + vyz[pt]));

          double mechwork = 
            tauxx*ux[*blit][pt] + tauxy*tauxy +
            tauyy*uy[*blit][pt] + tauxz*tauxz +
            tauzz*uz[*blit][pt] + tauyz*tauyz;

          mechwork = s_eta*mechwork 
            + u[pt]*a_dp[*blit](pt, CMOMX)
            + v[pt]*a_dp[*blit](pt, CMOMY)
            + w[pt]*a_dp[*blit](pt, CMOMZ);

          //thermal diffusion
          double tempdiff = s_alam*(Txx[pt] + Tyy[pt] + Tzz[pt]);
          a_dp[*blit](pt, CENG) = s_eta*(fourthirds*wzz[pt] + wxx[pt] + wzz[pt] + onethird*(uxz[pt] + vyz[pt]));
        }
    }
}
///
void advance(LevelData<double,CNUM>   & a_cons,     
             double                   & a_dt)
{
  const BoxLayout& layout = a_cons.getBoxLayout();
  //set ghost cells (periodic bcs);
  a_cons.exchange();
  //this could be done with just a box by box primitive
  //but that would require computing the time step separately

  LevelData<double,QNUM> prim(layout, s_nghost);

  double  maxwave = 0;

  
  for(BLIterator blit(layout); blit != blit.end(); ++blit)
    {
      double maxwavebox;
      double maxwavebox2;
      Box validBox = layout[*blit];
      constoprim(maxwavebox, prim[*blit], a_cons[*blit], validBox);
      maxwavebox2 = forall_max(prim[*blit], a_cons[*blit], &consToPrimF, validBox);  // same operation
      assert(maxwavebox == maxwavebox2);
      maxwave = max(maxwave, maxwavebox);
    }
  /****!!!should gather max wave speeds here!!!****/
  a_dt = s_cfl*s_dx/maxwave;

  //again, with careful work, these could be done box by box
  LevelData<double,CNUM> fp(layout, 0);
  LevelData<double,CNUM> dp(layout, 0);

  hypterm( fp, prim); 
  diffterm(dp, prim);
  
  for(BLIterator blit(layout); blit != blit.end(); ++blit)
    {
      Box bx = layout[*blit];
      for (Point pt = bx.getLowCorner();bx.notDone(pt);bx.increment(pt))
        {
          for(int ivar = 0; ivar < CNUM; ivar++)
            {
              double incr  = a_dt*(fp[*blit](pt, ivar) + dp[*blit](pt, ivar));
              a_cons[*blit](pt, ivar) += incr;
            }
        }
    }
}
///
int main(int argc, char* argv[])
{
  //fill in global params if you want different than the default
  parseCommandLine(argc, argv);

  //this makes one box of size BLOCKSIZE (defined in SPACE.H)
  BoxLayout layout(s_numblockpower);
  LevelData<double, CNUM > cons(layout, s_nghost);
  
  makeStencils();

  //initialization
  initialize(cons);
  double time=0;
  for(int istep=0;istep< s_nstep;istep++)
    {
      double dt;
      advance(cons, dt);
      time += dt;
   // cout << "We just advanced to time = "<< time << " using dt =  " << dt << "." << endl;
    }
}
///
