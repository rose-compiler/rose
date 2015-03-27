
#define BLOCKSIZE 32

double pow (double x, double y);
double atan (double x);


class CNS
{
public:
  int    m_nstep   = 10;
  int    m_numblockpower = 0;
  //number of ghost
  const int    m_nghost = 4;

  double m_problo  = -0.1;
  double m_probhi  =  0.1;

  int    m_numblocks = pow(2, m_numblockpower);
  int    m_ncell     = m_numblocks*BLOCKSIZE;
  double m_dx        = (m_probhi-m_problo)/m_ncell;
  
 

  double m_cfl     = 0.5;

  //diffusion coefficients
  double m_eta  = 1.0e-4;
  double m_alam = 1.5e2;
  //ideal gas constant
  double m_gamma = 1.4;
  //pi
  double m_pi = 4.*atan(1.0);

  
  //coefficient thermal expansion at constant volume
  double m_CV  = 8.3333333333e6;
};

