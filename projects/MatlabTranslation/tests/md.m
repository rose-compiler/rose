function [ f, pot, kin ] = compute ( np, nd, pos, vel, mass )
%*****************************************************************************80
%
%% COMPUTE computes the forces and energies.
%
%  Discussion:
%
%    The potential function V(X) is a harmonic well which smoothly
%    saturates to a maximum value at PI/2:
%
%      v(x) = ( sin ( min ( x, PI/2 ) ) )^2
%
%    The derivative of the potential is:
%
%      dv(x) = 2.0 * sin ( min ( x, PI/2 ) ) * cos ( min ( x, PI/2 ) )
%            = sin ( 2.0 * min ( x, PI/2 ) )
%
%  Licensing:
%
%    This code is distributed under the GNU LGPL license.
%
%  Modified:
%
%    30 July 2009
%
%  Author:
%
%    John Burkardt and Gene Cliff.
%
%  Parameters:
%
%    Input, integer NP, the number of particles.
%
%    Input, integer ND, the number of spatial dimensions.
%
%    Input, real POS(ND,NP), the positions.
%
%    Input, real VEL(ND,NP), the velocities.
%
%    Input, real MASS, the mass of each particle.
%
%    Output, real F(ND,NP), the forces.
%
%    Output, real POT, the total potential energy.
%
%    Output, real KIN, the total kinetic energy.
%
  f = zeros ( nd, np );

  pot = 0.0;

  for i = 1 : np
%
%  Compute the potential energy and forces.
%
    for j = 1 : np

      if ( i ~= j )

        for k = 1 : nd
          rij(k) = pos(k,i) - pos(k,j);
        end

        d = 0.0;
        for k = 1 : nd
          d = d + rij(k)^2;
        end
        d = sqrt ( d );
%
%  Truncate the distance.
%
        d2 = min ( d, pi / 2.0 );
%
%  Attribute half of the total potential energy to particle J.
%
        pot = pot + 0.5 * sin ( d2 ) * sin ( d2 );
%
%  Add particle J's contribution to the force on particle I.
%
        for k = 1 : nd
          f(k,i) = f(k,i) - rij(k) * sin ( 2.0 * d2 ) / d;
        end

      end

    end

  end
%
%  Compute the total kinetic energy.
%
  kin = 0.5 * mass * sum ( sum ( vel(1:nd,1:np).^2 ) );

  return
end

function [ pos, vel, acc ] = initialize ( np, nd )

%*****************************************************************************80
%
%% INITIALIZE initializes the positions, velocities, and accelerations.
%
%  Licensing:
%
%    This code is distributed under the GNU LGPL license.
%
%  Modified:
%
%    26 December 2014
%
%  Author:
%
%    John Burkardt.
%
%  Parameters:
%
%    Input, integer NP, the number of particles.
%
%    Input, integer ND, the number of spatial dimensions.
%
%    Output, real POS(ND,NP), the positions.
%
%    Output, real VEL(ND,NP), the velocities.
%
%    Output, real ACC(ND,NP), the accelerations.
%
  seed = 123456789;
%
%  Positions.
%
  [ pos, seed ] = r8mat_uniform_ab ( nd, np, 0.0, 10.0, seed );
%
%  Velocities.
%
  vel = zeros ( nd, np );
%
%  Accelerations.
%
  acc = zeros ( nd, np );

  return
end

function [ r, seed ] = r8mat_uniform_ab ( m, n, a, b, seed )

%*****************************************************************************80
%
%% R8MAT_UNIFORM_AB returns a scaled pseudorandom R8MAT.
%
%  Discussion:
%
%    An R8MAT is an array of R8's.
%
%  Licensing:
%
%    This code is distributed under the GNU LGPL license.
%
%  Modified:
%
%    21 September 2006
%
%  Author:
%
%    John Burkardt
%
%  Reference:
%
%    Paul Bratley, Bennett Fox, Linus Schrage,
%    A Guide to Simulation,
%    Second Edition,
%    Springer, 1987,
%    ISBN: 0387964673,
%    LC: QA76.9.C65.B73.
%
%    Bennett Fox,
%    Algorithm 647:
%    Implementation and Relative Efficiency of Quasirandom
%    Sequence Generators,
%    ACM Transactions on Mathematical Software,
%    Volume 12, Number 4, December 1986, pages 362-376.
%
%    Pierre L'Ecuyer,
%    Random Number Generation,
%    in Handbook of Simulation,
%    edited by Jerry Banks,
%    Wiley, 1998,
%    ISBN: 0471134031,
%    LC: T57.62.H37.
%
%    Peter Lewis, Allen Goodman, James Miller,
%    A Pseudo-Random Number Generator for the System/360,
%    IBM Systems Journal,
%    Volume 8, Number 2, 1969, pages 136-143.
%
%  Parameters:
%
%    Input, integer M, N, the number of rows and columns in the array.
%
%    Input, real A, B, the range of the pseudorandom values.
%
%    Input, integer SEED, the integer "seed" used to generate
%    the output random number.
%
%    Output, real R(M,N), an array of random values between 0 and 1.
%
%    Output, integer SEED, the updated seed.  This would
%    normally be used as the input seed on the next call.
%
  r = zeros ( m, n );

  i4_huge = 2147483647;

  if ( seed == 0 )
    fprintf ( 1, '\n' );
    fprintf ( 1, 'R8MAT_UNIFORM_AB - Fatal error!\n' );
    fprintf ( 1, '  Input SEED = 0!\n' );
    error ( 'R8MAT_UNIFORM_AB - Fatal error!' );
  end

  for j = 1 : n
    for i = 1 : m

      seed = floor ( seed );

      seed = mod ( seed, i4_huge );

      if ( seed < 0 )
        seed = seed + i4_huge;
      end

      k = floor ( seed / 127773 );

      seed = 16807 * ( seed - k * 127773 ) - k * 2836;

      if ( seed < 0 )
        seed = seed + i4_huge;
      end

      r(i,j) = a + ( b - a ) * seed * 4.656612875E-10;

    end
  end

  return
end
function timestamp ( )

%*****************************************************************************80
%
%% TIMESTAMP prints the current YMDHMS date as a timestamp.
%
%  Licensing:
%
%    This code is distributed under the GNU LGPL license.
%
%  Modified:
%
%    14 February 2003
%
%  Author:
%
%    John Burkardt
%
  t = now;
  c = datevec ( t );
  s = datestr ( c, 0 );
  fprintf ( 1, '%s\n', s );

  return
end

function [ pos, vel, acc ] = update ( np, nd, pos, vel, f, acc, mass, dt )

%*****************************************************************************80
%
%% UPDATE updates positions, velocities and accelerations.
%
%  Discussion:
%
%    The time integration is fully parallel.
%
%    A velocity Verlet algorithm is used for the updating.
%
%    x(t+dt) = x(t) + v(t) * dt + 0.5 * a(t) * dt * dt
%    v(t+dt) = v(t) + 0.5 * ( a(t) + a(t+dt) ) * dt
%    a(t+dt) = f(t) / m
%
%  Licensing:
%
%    This code is distributed under the GNU LGPL license.
%
%  Modified:
%
%    15 July 2008
%
%  Author:
%
%    John Burkardt
%
%  Parameters:
%
%    Input, integer NP, the number of particles.
%
%    Input, integer ND, the number of spatial dimensions.
%
%    Input, real POS(ND,NP), the positions.
%
%    Input, real VEL(ND,NP), the velocities.
%
%    Input, real F(ND,NP), the forces.
%
%    Input, real ACC(ND,NP), the accelerations.
%
%    Input, real MASS, the mass of each particle.
%
%    Input, real DT, the time step.
%
%    Output, real POS(ND,NP), the updated positions.
%
%    Output, real VEL(ND,NP), the updated velocities.
%
%    Output, real ACC(ND,NP), the updated accelerations.
%
  rmass = 1.0 / mass;

  pos(1:nd,1:np) = pos(1:nd,1:np) + vel(1:nd,1:np) * dt ...
    + 0.5 * acc(1:nd,1:np) * dt * dt;

  vel(1:nd,1:np) = vel(1:nd,1:np) ...
    + 0.5 * dt * ( f(1:nd,1:np) * rmass + acc(1:nd,1:np) );

  acc(1:nd,1:np) = f(1:nd,1:np) * rmass;

  return
end

function md ( nd, np, step_num, dt )

%*****************************************************************************80
%
%% MD is the main program for the molecular dynamics simulation.
%
%  Discussion:
%
%    MD implements a simple molecular dynamics simulation.
%
%    The velocity Verlet time integration scheme is used.
%
%    The particles interact with a central pair potential.
%
%    Based on a FORTRAN90 program by Bill Magro.
%
%  Usage:
%
%    md ( nd, np, step_num, dt )
%
%    where
%
%    * nd is the spatial dimension (2 or 3);
%    * np is the number of particles (500, for instance);
%    * step_num is the number of time steps (500, for instance).
%    * dt is the time step (0.1 for instance )
%
%  Licensing:
%
%    This code is distributed under the GNU LGPL license.
%
%  Modified:
%
%    27 December 2014
%
%  Author:
%
%    John Burkardt
%
%  Parameters:
%
%    Input, integer ND, the spatial dimension.
%    A value of 2 or 3 is usual.
%    The default value is 3.
%
%    Input, integer NP, the number of particles.
%    A value of 1000 or 2000 is small but "reasonable".
%    The default value is 500.
%
%    Input, integer STEP_NUM, the number of time steps.
%    A value of 500 is a small but reasonable value.
%    The default value is 500.
%
%    Input, real DT, the time step.
%    A value of 0.1 is large; the system will begin to move quickly but the
%    results will be less accurate.
%    A value of 0.0001 is small, but the results will be more accurate.
%    The default value is 0.1.
%
  if ( nargin < 1 )
    nd = 3;
  elseif ( ischar ( nd ) )
    nd = str2num ( nd );
  end

  if ( nargin < 2 )
    np = 500;
  elseif ( ischar ( np ) )
    np = str2num ( np );
  end

  if ( nargin < 3 )
    step_num = 500;
  elseif ( ischar ( step_num ) )
    step_num = str2num ( step_num );
  end

  if ( nargin < 4 )
    dt = 0.1;
  elseif ( ischar ( dt ) )
    dt = str2num ( dt );
  end

  mass = 1.0;
%
%  Report to the user.
%
  timestamp ( );
  fprintf ( 1, '\n' );
  fprintf ( 1, 'MD\n' );
  fprintf ( 1, '  MATLAB version\n' );
  fprintf ( 1, '  A molecular dynamics program.\n' );
  fprintf ( 1, '\n' );
  fprintf ( 1, '  ND, the spatial dimension, is %d\n', nd );
  fprintf ( 1, '  NP, the number of particles in the simulation is %d.\n', np );
  fprintf ( 1, '  STEP_NUM, the number of time steps, is %d.\n', step_num );
  fprintf ( 1, '  DT, the time step size, is %g seconds.\n', dt );
%
%  This is the main time stepping loop:
%    Compute forces and energies,
%    Update positions, velocities, accelerations.
%
  fprintf ( 1, '\n' );
  fprintf ( 1, '  At each step, we report the potential and kinetic energies.\n' );
  fprintf ( 1, '  The sum of these energies should be a constant.\n' );
  fprintf ( 1, '  As an accuracy check, we also print the relative error\n' );
  fprintf ( 1, '  in the total energy.\n' );
  fprintf ( 1, '\n' );
  fprintf ( 1, '      Step      Potential       Kinetic        (P+K-E0)/E0\n' );
  fprintf ( 1, '                Energy P        Energy K       Relative Energy Error\n' );
  fprintf ( 1, '\n' );

  step_print_index = 0;
  step_print_num = 10;
  step_print = 0;

  tic;

  for step = 0 : step_num

    if ( step == 0 )
      [ pos, vel, acc ] = initialize ( np, nd );
    else
      [ pos, vel, acc ] = update ( np, nd, pos, vel, force, acc, mass, dt );
    end

    [ force, potential, kinetic ] = compute ( np, nd, pos, vel, mass );

    if ( step == 0 )
      e0 = potential + kinetic;
    end

    if ( step == step_print )
      rel = ( potential + kinetic - e0 ) / e0;
      fprintf ( 1, '  %8d  %14f  %14f  %14e\n', step, potential, kinetic, rel );
      step_print_index = step_print_index + 1;
      step_print = floor ( ( step_print_index * step_num ) / step_print_num );
    end

  end
%
%  Report timing.
%
  wtime = toc;
  fprintf ( 1, '\n' );
  fprintf ( 1, '  Wall clock time = %f seconds.\n', wtime );
%
%  Terminate.
%
  fprintf ( 1, '\n' );
  fprintf ( 1, 'MD\n' );
  fprintf ( 1, '  Normal end of execution.\n' );
  fprintf ( 1, '\n' );
  timestamp ( );

  return
end
