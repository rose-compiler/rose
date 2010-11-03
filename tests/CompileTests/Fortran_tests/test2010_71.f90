   implicit none
   integer nk, nq
   parameter (nq = 10, nk =90)
   double precision x, q

   common/storage/ x(2*nk), q(0:nq-1)
end
