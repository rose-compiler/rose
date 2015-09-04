      subroutine ctoprim (lo,hi,ng,u,q)
        integer         :: lo(3), hi(3), ng
        double precision::u(lo(1)-ng:hi(1)+ng,lo(2)-ng:hi(2)+ng,
     &                      lo(3)-ng:hi(3)+ng)
        double precision::q(lo(1)-ng:hi(1)+ng,lo(2)-ng:hi(2)+ng,
     &                      lo(3)-ng:hi(3)+ng)
      end subroutine
