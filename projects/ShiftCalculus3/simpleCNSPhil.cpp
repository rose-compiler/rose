#define BLOCKSIZE 32
#define DIM       3

#define CNUM  5

#include "simpleCNSPhil.H"
#include <cstring>

///constants 
//these are vectors in the fortran but they all have the same value
//fortran code says problo = 1 in the comments then sets it to 0.1


////////////
void
CNS::parseCommandLine(int argc, char* argv[])
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
          m_problo = strtod(argv[iarg+1], &ptr);
        }
      else if(strcmp(argv[iarg], "-h") == 0)
        {
          m_probhi = strtod(argv[iarg+1], &ptr);
        }
      else if(strcmp(argv[iarg],"-n") == 0)
        {
          m_numblockpower = atoi(argv[iarg+1]);
          m_numblocks = pow(2, m_numblockpower);
          m_ncell   =m_numblocks*BLOCKSIZE;
        }
      else  if(strcmp(argv[iarg],"-t") == 0)
        {
          m_nstep = atoi(argv[iarg+1]);
        }
      else if(strcmp(argv[iarg], "-c") == 0)
        {
          m_cfl   = strtod(argv[iarg+1], &ptr);
        }
      else if(strcmp(argv[iarg], "-e") == 0)
        {
          m_eta   = strtod(argv[iarg+1], &ptr);
        }
      else if(strcmp(argv[iarg], "-a") == 0)
        {
          m_alam   = strtod(argv[iarg+1], &ptr);
        }
      else if(strcmp(argv[iarg], "-g") == 0)
        {
          m_gamma   = strtod(argv[iarg+1], &ptr);
        }
      else if(strcmp(argv[iarg], "-v") == 0)
        {
          m_CV   = strtod(argv[iarg+1], &ptr);
        }
    }
  cout << "problo = " << m_problo  << endl
       << "probhi = " << m_probhi  << endl
       << "ncell  = " << m_ncell   << endl
       << "nsteps = " << m_nstep   << endl
       << "cfl    = " << m_cfl     << endl
       << "eta    = " << m_eta     << endl
       << "alam   = " << m_alam    << endl
       << "gamma  = " << m_gamma   << endl
       << "CV     = " << m_CV      << endl; 
#endif
}

void CNS::initialize(RectMDArray<double, CNUM>& a_patch) const
{
  //this could be made dimension-independent with more cleverness
  assert(DIM==3);

  Box D0 = a_patch.getBox();
  int k=1;
  //this is a vector in the fortran but all have the same value
  double scale = (m_probhi-m_problo)/(2.*m_pi);
  for (Point pt = D0.getLowCorner();D0.notDone(pt);D0.increment(pt))
    {
      //this is reproducing the bug in the fortran.  should be (pt+0.5)*dx
      double xloc = pt[0]*m_dx/scale;
      double yloc = pt[1]*m_dx/scale;
      double zloc = pt[2]*m_dx/scale;
      
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
void CNS::initialize(LevelData<double, CNUM> & a_cons) const
{
  const BoxLayout& layout = a_cons.getBoxLayout();
  for(BLIterator blit(layout); blit != blit.end(); ++blit)
    {
      initialize(a_cons[*blit]);
    }
}

void 
CNS::makeStencils()
{
  array<Shift,DIM> S=getShiftVec();
  double D1[4], D2[5], ND1[4], ND2[5];

  // set coefficients, at the moment they are
  // not the real values to be applied to the 
  // stencil, because the operation is the same
  // and the outcome much easier to verify with
  // simpler coefficients
  D1[0] =        0.8/m_dx;
  D1[1] =       -0.2/m_dx;
  D1[2] =  4.0/105.0/m_dx;
  D1[3] = -1.0/280.0/m_dx;
  
  D2[0] = -205.0/72.0/m_dx/m_dx;
  D2[1] =     8.0/5.0/m_dx/m_dx;
  D2[2] =        -0.2/m_dx/m_dx;
  D2[3] =   8.0/315.0/m_dx/m_dx;
  D2[4] =  -1.0/560.0/m_dx/m_dx;

  for (int i=0; i<4; i++)
    {
      ND1[i]=-D1[i];
      ND2[i]= D2[i];
    }
  ND2[4]= D2[4];

  // now build the stencils
  for (int dir=0;dir<DIM;dir++)
    {
      Point uv=getUnitv(dir);
      Point nuv=uv*(-1);
      Point zero = getZeros();
      m_duds[dir] = D1[0]*(S^uv) + ND1[0]*(S^nuv)
	+ D1[1]*(S^(uv*2)) + ND1[1]*(S^(nuv*2))
	+ D1[2]*(S^(uv*3)) + ND1[2]*(S^(nuv*3))
	+ D1[3]*(S^(uv*4)) + ND1[3]*(S^(nuv*4));      

      m_d2uds2[dir] = D2[0]*(S^(zero)) + D2[1]*(S^uv) + ND2[1]*(S^nuv)
	+ D2[2]*(S^(uv*2)) + ND2[2]*(S^(nuv*2))
	+ D2[3]*(S^(uv*3)) + ND2[3]*(S^(nuv*3))
	+ D2[4]*(S^(uv*4)) + ND2[4]*(S^(nuv*4));      

      //this is what the stencils look like
      // cout << "dumping duds stencil "<< endl ;
      // m_duds[dir].stencilDump();
      // cout << endl << "dumping d2uds2 stencil "<< endl ;
      // m_d2uds2[dir].stencilDump();            
    }
}

double CNS::consToPrimF(Tensor<double,QNUM>& a_Q, CTensor<double, CNUM>& a_U) const 
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
  double temp = eint/m_CV;
  double press = (m_gamma-1.)*eint*rho;
  double sound = sqrt(m_gamma*press/rho);
  
  double maxV = fabs(u)+fabs(v)+fabs(w) + DIM*sound; //apparently split schemes needs this max wave speed.
 
  a_Q(QRHO)  = rho;
  a_Q(QVELX) = u;
  a_Q(QVELY) = v;
  a_Q(QVELZ) = w;
  a_Q(QPRES) = press;
  a_Q(QTEMP) = temp;
  return maxV;
}
void CNS::hypFluxesF(int a_dir,Tensor<double,CNUM>& a_flux, CTensor<double, QNUM>& a_prim) const 
{
  a_flux(CRHO) = -a_prim(QRHO)*a_prim(QRHO + a_dir + 1);
  double ke = 0.;
  for (int dir = 1;dir < DIM+1;dir++)
    {
      ke += a_prim(dir)*a_prim(dir)/2;
      a_flux(dir) = a_flux(CRHO)*a_prim(dir);
    }
  a_flux(CENG) = a_flux(CRHO)*(m_gamma*a_prim(QPRES)/a_prim(QRHO)/(m_gamma - 1.0)
                               + ke);
}
double CNS::consToPrim(LevelData<double,QNUM>& a_Q, const LevelData<double, CNUM>& a_U)const 
{
  const BoxLayout& layout = a_U.getBoxLayout();
  double maxWave=0;

  // DQ (2/14/2015): This code was a problem for ROSE (now fixed).
  auto f =  [this](Tensor<double,QNUM>& a_Q, CTensor<double, CNUM>& a_U){return consToPrimF(a_Q,a_U);};

  for(BLIterator blit(layout); blit != blit.end(); ++blit)
    {
      const RectMDArray<double,CNUM>& cons=a_U[*blit];

   // DQ (2/24/2015): Commented out for EDG.
   // maxWave = std::max(maxWave,forall_max<double,QNUM,CNUM>(a_Q[*blit],cons,f,cons.getBox()));
    }

  return maxWave;
}


void CNS::viscousRhsF(Tensor<double,DIM>& a_divTau, CTensor<double,DIM,DIM,DIM>& a_D2u) const
{
   a_divTau(0) = a_D2u(0,0,0)*4/3 + a_D2u(0,1,1) + a_D2u(0,2,2) +
    a_D2u(1,1,0)/3 + a_D2u(2,2,0)/2;
  a_divTau(1) = a_D2u(1,0,0)*4/3 + a_D2u(1,1,1) + a_D2u(1,2,2) +
    a_D2u(0,1,0)/3 + a_D2u(2,2,1)/3;
  a_divTau(2) = a_D2u(2,0,0)*4/3 + a_D2u(2,1,1) + a_D2u(2,2,2) +
    a_D2u(0,2,0)/3 + a_D2u(1,2,1)/3;
}

void CNS::viscousHeatingF(double& a_edot, CTensor<double,DIM,DIM>& a_gradu) const
{
  a_edot =-( 
    (a_gradu(0,0) - a_gradu(1,1))*(a_gradu(0,0) - a_gradu(1,1))*2/3 + 
    (a_gradu(2,2) - a_gradu(0,0))*(a_gradu(2,2) - a_gradu(0,0))*2/3 +
    (a_gradu(2,2) - a_gradu(1,1))*(a_gradu(2,2) - a_gradu(1,1))*2/3 +
    (a_gradu(2,1) + a_gradu(1,2))*(a_gradu(2,1) + a_gradu(1,2))/3 +
    (a_gradu(2,0) + a_gradu(0,2))*(a_gradu(2,0) + a_gradu(0,2))/3 +
    (a_gradu(1,0) + a_gradu(0,1))*(a_gradu(1,0) + a_gradu(0,1))/3);
}

void CNS::rhs(LevelData<double,CNUM>    & a_fp,  const LevelData<double,QNUM>  & a_prim) const
{
  const BoxLayout& layout = a_prim.getBoxLayout();

  // Get gradients of primitive variables.
  for(BLIterator blit(layout); blit != blit.end(); ++blit)
    {      
      Box ghostBox = a_prim[*blit].getBox();
      Box validBox = layout[*blit];
      Box gradBox = ghostBox.grow(-1);
      // Compute fluxes on ghosted box using pointwise operation.

      const RectMDArray<double,QNUM>& prim = a_prim[*blit];
      RectMDArray<double,CNUM>& fp = a_fp[*blit];
      fp.setVal(0.);
      RectMDArray<double,CNUM> dirFluxes(ghostBox);
      RectMDArray<double,DIM> vel = 
         slice<double,QNUM,DIM>(prim,Interval(1,DIM));
      RectMDArray<double> temp = 
        slice<double,QNUM,1>(prim,Interval(DIM+1,DIM+1)); // temperature.
      RectMDArray<double> LaplacianTemp(validBox);
      LaplacianTemp.setVal(0.);
      RectMDArray<double,DIM,DIM> gradu(ghostBox);
      RectMDArray<double,DIM,DIM,DIM> D2vel(validBox);
      RectMDArray<double,DIM> divTau(validBox);
      RectMDArray<double> kedot; // energy dissipation. 
 
 
      // put the previous two lines inside the dir loop, and pass the direction in.
      for (int dir = 0; dir < DIM; dir++)
        {
          // Hyperbolic fluxes.
          // 

  // DQ (2/14/2015): This code was a problem for ROSE (now fixed).
	  auto hyp = [this,dir](Tensor<double,CNUM>& a_flux, CTensor<double, QNUM>& a_q)
	    {
	      this->hypFluxesF(dir,a_flux, a_q);
	    };
// DQ (2/24/2015): New feature added.
//	  forall<double,CNUM,QNUM>(dirFluxes,prim,hyp,ghostBox); 
	  
	  for (int comp = 0; comp < CNUM; comp++)
	    {
	      const RectMDArray<double> df = slice<double,CNUM,1>(dirFluxes,Interval(comp,comp));
	      RectMDArray<double> fpc = slice<double,CNUM,1>(fp,Interval(comp,comp));
//	      fpc += m_duds[dir](df,validBox);
	     }
	  
          // first and second velocity derivatives in the dir direction.
	  for (int veldir = 0; veldir < DIM; veldir++)
	    {
	      RectMDArray<double> velcomp = slice<double,DIM,1>(vel,Interval(veldir,veldir));
	      RectMDArray<double> gu = slice<double,DIM,1>(slice<double,DIM,DIM,1>(gradu,Interval(dir,dir)),Interval(veldir,veldir));

      // DQ (2/24/2015): New feature added.
      // gu  += m_duds[dir](velcomp,gradBox);
	      
	      RectMDArray<double,DIM,DIM> d2velcomp = 
		slice<double,DIM,DIM,DIM,1>(D2vel,Interval(veldir,veldir));
	      RectMDArray<double> d2u = slice<double,DIM,1>(slice<double,DIM,DIM,1>
							    (d2velcomp,Interval(dir,dir)),
							    Interval(dir,dir));

      // DQ (2/24/2015): New feature added.
	   // d2u += m_d2uds2[dir](velcomp,validBox);
	    
	      // mixed partials of velocity.
	      for (int dir2 = 0; dir2 < dir; dir2++)
		{

		  RectMDArray<double> d2u = slice<double,DIM,1>(slice<double,DIM,DIM,1>
								(d2velcomp,Interval(dir,dir)),
								Interval(dir2,dir2));
		  RectMDArray<double> d1u = slice<double,DIM,1>(slice<double,DIM,DIM,1>
								(gradu,Interval(dir,dir)),
								Interval(veldir,veldir));

      // DQ (2/24/2015): New feature added.
      // d2u  += m_duds[dir2](d1u,validBox);
		}
	    }
      // DQ (2/24/2015): New feature added.
      // LaplacianTemp |= m_d2uds2[dir](temp,validBox);
        }
      // Start assembling the right-hand side.
      // Viscous stresses.
 

  // DQ (2/14/2015): This code was a problem for ROSE (now fixed).
      auto divT = [this](Tensor<double,DIM>& a_divTau, CTensor<double,DIM,DIM,DIM>& a_D2u)
	{  this->viscousRhsF(a_divTau, a_D2u);};

   // DQ (2/24/2015): New feature added.
   // forall(divTau,D2vel,divT,validBox);

      RectMDArray<double> viscousHeating(validBox);
      //Thermal transport.
      viscousHeating.setVal(0.);

  // DQ (2/14/2015): This code was a problem for ROSE (now fixed).
      auto visc = [this](double& a_edot, CTensor<double,DIM,DIM>& a_gradu)
	{ this->viscousHeatingF(a_edot, a_gradu);};
      
   // DQ (2/24/2015): New feature added.
   // forall(viscousHeating,gradu,visc,validBox);

      slice<double,CNUM,1>(fp,Interval(DIM+1,DIM+1)).plus(viscousHeating);
      
      for (int dir = 0; dir < DIM;dir++)
        {
	  RectMDArray<double> f = slice<double,CNUM,1>(fp,Interval(DIM+1,DIM+1));
	  RectMDArray<double> dTau = slice<double,DIM,1>(divTau,Interval(dir,dir));
          // viscous stresses.
	  f.plus(dTau);

          // kinetic energy transport.
          
	  RectMDArray<double> v(validBox);
          slice<double,DIM,1>(vel,Interval(dir,dir)).copyTo(v);

	  //  we're at the end the function, fine to clobber divTau with in-place multiply
	  f.plus(dTau*=v);
        }
    }
}

int main(int argc, char** argv)
{
// DQ (2/24/2015): New feature added.
// reportMemory=true;
  //fill in global params if you want different than the default
  CNS cns;
  cns.parseCommandLine(argc, argv);
  
  //this makes one box of size BLOCKSIZE (defined in SPACE.H)
  BoxLayout layout(cns.m_numblockpower);
  LevelData<double, CNUM > U(layout), f_0(layout), f_1(layout), U_hat(layout);
  LevelData<double, QNUM>  Q(layout,cns.m_nghost);

    //initialization
  cns.initialize(U);
  double time=0;
  double dt;
  for(int istep=0;istep< cns.m_nstep;istep++)
    {
      double w0 = cns.consToPrim(Q,U);
      if(istep == 0) dt=cns.m_cfl*cns.m_dx/w0;
      Q.exchange();
      cns.rhs(f_0, Q);
      // U_hat = U + 1.0/3*dt*f_0
      auto inc1 = [dt](RectMDArray<double,CNUM>& uhat, const RectMDArray<double,CNUM>& u, const RectMDArray<double,CNUM>& f0)
	{
	  uhat.setVal(0.0);
	  uhat.axby(1.0, u, 1.0/3.0*dt,f0);
	};
  // DQ (2/24/2015): New feature added.
  // forall(U_hat, U, f_0, inc1);

      double w1=cns.consToPrim(Q,U_hat);
      Q.exchange();
      cns.rhs(f_1,Q);
      //U_hat = U_hat +1.0/48*dt*(45*f_1-25*f_0);
      auto inc2 = [dt](RectMDArray<double,CNUM>& uhat, const RectMDArray<double,CNUM>& f1, const RectMDArray<double,CNUM>& f0)
	{
	  uhat.axby(45.0/48.0, f1, -25.0/48, f0);
	};
  // DQ (2/24/2015): New feature added.
  // forall(U_hat, f_1, f_0, inc2);
 

      //f_0 = -153*f_1 + 85*f_0;
      auto inc3 = [](RectMDArray<double,CNUM>& f0, const RectMDArray<double,CNUM>& f1)
	{
	  f0*=85;
	  f0.plus(-153, f1);
	};
  // DQ (2/24/2015): New feature added.
  // forall(f_0, f_1, inc3);
      double w2 = cns.consToPrim(Q,U_hat);
      Q.exchange();
      cns.rhs(f_1,Q);
      // U = U_hat +1.0/240*dt*(128*f_1 + 15*f_0);

#if 0
   // DQ (2/24/2015): New feature added.
      forall(U,U_hat, f_1, f_0, [dt](RectMDArray<double, CNUM>& u, const RectMDArray<double, CNUM>& uhat, const RectMDArray<double,CNUM>& f1, const RectMDArray<double,CNUM>& f0)
	     {
	       uhat.copyTo(u);
	       u.axby(128.0/240,f1,15.0/240,f0);
	     });
#endif

      time += dt;
//    cout << "We just advanced to time = "<< time << " using dt =  " << dt << "." << endl;
      double maxwavespeed = std::max(std::max(w0,w1),w2);
      dt=cns.m_cfl*cns.m_dx/maxwavespeed;
    }
}
