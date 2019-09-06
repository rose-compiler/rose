#include <stdio.h>
#include <math.h>
//#include <string>
//#include <gsl/gsl_fft_complex.h>

#define REAL(z,i) ((z)[2*(i)])
#define IMAG(z,i) ((z)[2*(i)+1])
#define square(x) x*x

#define ABS(x) ( ((x) < 0.0) ? (-(x)) : (x) )
#define ANS 4.5825756949558389

#ifndef EPS
#define EPS 1e-1
#endif

#define SIZE 23

typedef enum
  {
    gsl_fft_forward = -1, gsl_fft_backward = +1
  }
gsl_fft_direction;

static int
fft_complex_bitreverse_order (double data[],
                                        const size_t stride,
                                        const size_t n,
                                        size_t logn)
{


  size_t i;
  size_t j = 0;

  logn = 0 ;

  for (i = 0; i < n - 1; i++)
    {
      size_t k = n / 2 ;

      if (i < j)
        {
          const double tmp_real = ((data)[2*(stride)*(i)]);
          const double tmp_imag = ((data)[2*(stride)*(i)+1]);
          ((data)[2*(stride)*(i)]) = ((data)[2*(stride)*(j)]);
          ((data)[2*(stride)*(i)+1]) = ((data)[2*(stride)*(j)+1]);
          ((data)[2*(stride)*(j)]) = tmp_real;
          ((data)[2*(stride)*(j)+1]) = tmp_imag;
        }

      while (k <= j)
        {
          j = j - k ;
          k = k / 2 ;
        }

      j += k ;
    }

  return 0;
}

static int
fft_binary_logn (const size_t n)
{
  size_t ntest ;
  size_t binary_logn = 0 ;
  size_t k = 1;

  while (k < n)
    {
      k *= 2;
      binary_logn++;
    }

  ntest = (1 << binary_logn) ;

  if (n != ntest )
    {
      return -1 ;
    }

  return binary_logn;
}

int
gsl_fft_complex_radix2_transform (double * data,
                                            const size_t stride,
                                            const size_t n,
                                            const gsl_fft_direction sign)
{
  int result ;
  size_t dual;
  size_t bit;
  size_t logn = 0;
  int status;

  if (n == 1)
    {
      return 0 ;
    }



  result = fft_binary_logn(n) ;

  if (result == -1)
    {
      do {printf("ERROR fft\n"); return 1; } while (0);
    }
  else
    {
      logn = result ;
    }



  status = fft_complex_bitreverse_order (data, stride, n, logn) ;



  dual = 1;

  for (bit = 0; bit < logn; bit++)
    {
      double w_real = 1.0;
      double w_imag = 0.0;

      const double theta = 2.0 * ((int) sign) * 3.14159265358979323846 / (2.0 * (double) dual);

      const double s = sin (theta);
      const double t = sin (theta / 2.0);
      const double s2 = 2.0 * t * t;

      size_t a, b;



      for (b = 0; b < n; b += 2 * dual)
        {
          const size_t i = b ;
          const size_t j = b + dual;

          const double z1_real = ((data)[2*(stride)*(j)]) ;
          const double z1_imag = ((data)[2*(stride)*(j)+1]) ;

          const double wd_real = z1_real ;
          const double wd_imag = z1_imag ;

          ((data)[2*(stride)*(j)]) = ((data)[2*(stride)*(i)]) - wd_real;
          ((data)[2*(stride)*(j)+1]) = ((data)[2*(stride)*(i)+1]) - wd_imag;
          ((data)[2*(stride)*(i)]) += wd_real;
          ((data)[2*(stride)*(i)+1]) += wd_imag;
        }


      for (a = 1; a < dual; a++)
        {



          {
            const double tmp_real = w_real - s * w_imag - s2 * w_real;
            const double tmp_imag = w_imag + s * w_real - s2 * w_imag;
            w_real = tmp_real;
            w_imag = tmp_imag;
          }

          for (b = 0; b < n; b += 2 * dual)
            {
              const size_t i = b + a;
              const size_t j = b + a + dual;

              const double z1_real = ((data)[2*(stride)*(j)]) ;
              const double z1_imag = ((data)[2*(stride)*(j)+1]) ;

              const double wd_real = w_real * z1_real - w_imag * z1_imag;
              const double wd_imag = w_real * z1_imag + w_imag * z1_real;

              ((data)[2*(stride)*(j)]) = ((data)[2*(stride)*(i)]) - wd_real;
              ((data)[2*(stride)*(j)+1]) = ((data)[2*(stride)*(i)+1]) - wd_imag;
              ((data)[2*(stride)*(i)]) += wd_real;
              ((data)[2*(stride)*(i)+1]) += wd_imag;
            }
        }
      dual *= 2;
    }

  return 0;

}

int
gsl_fft_complex_radix2_forward (double * data,
                                          const size_t stride, const size_t n)
{
  gsl_fft_direction sign = gsl_fft_forward;
  int status = gsl_fft_complex_radix2_transform (data, stride, n, sign);
  return status;
}

int
main (void)
{
//  printf("EPS: %e\n", EPS);

  size_t size = pow(2, SIZE);
  size_t i; double data[2*size];

  for (i = 0; i <  size; i++)
  {
       REAL(data,i) = 0.0; IMAG(data,i) = 0.0;
  }

  REAL(data,0) = 1.0;

  for (i = 1; i <= 10; i++)
  {
       REAL(data,i) = REAL(data,size - i) = 1.0;
  }

/*  for (i = 0; i < size; i++)
    {
      printf ("%d %e %e\n", i, 
              REAL(data,i), IMAG(data,i));
    }
  printf ("\n");
*/
# pragma adapt begin
  gsl_fft_complex_radix2_forward (data, 1, size);

  double norm = 0;
  for (i = 0; i < size; i++)
  {
//      printf ("%d %e %e\n", i, 
//              REAL(data,i)/sqrt(128), 
//              IMAG(data,i)/sqrt(128));
    norm += square(REAL(data, i)/sqrt(size)) + square(IMAG(data,i)/sqrt(size));
  }

  norm = sqrt(norm); 
# pragma adapt output norm EPS
# pragma adapt end

//  printf("norm: %.16f\n", norm);

  double diff = (ANS-norm);
  double error = ABS(diff);
  if ((double)error < (double)EPS) {
    printf("fft - SUCCESSFUL!\n");
  }
  else {
    printf("fft - FAILED!\n");
  }
  return 0;
}
