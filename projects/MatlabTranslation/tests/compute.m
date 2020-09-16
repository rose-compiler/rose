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

