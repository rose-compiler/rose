! Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
! other BLT Project Developers. See the top-level LICENSE file for details
!
! SPDX-License-Identifier: (BSD-3-Clause)

!------------------------------------------------------------------------------
!
! blt_fruit_smoke.f90
!
!------------------------------------------------------------------------------
module fruit_mpi_smoke
  use iso_c_binding
  use fruit
  use mpi
  use fruit_mpi
  implicit none

contains
!------------------------------------------------------------------------------

  subroutine simple_mpi_test(comm)
        integer, intent(in) :: comm
        integer :: comm_size, comm_rank
        integer :: mpi_err_stat
        integer, parameter :: first_rank = 0

        call mpi_comm_size(comm, comm_size, mpi_err_stat)
        call mpi_comm_rank(comm, comm_rank, mpi_err_stat)

        call assert_true(comm_rank .ge. first_rank, &
             "MPI comm rank is negative; something strange is happening")
        call assert_true(comm_rank .lt. comm_size, &
             "MPI comm rank is greater than or equal to comm size")
  end subroutine simple_mpi_test


!----------------------------------------------------------------------
end module fruit_mpi_smoke
!----------------------------------------------------------------------

program fortran_mpi_test
  use fruit
  use mpi
  use fruit_mpi
  use fruit_mpi_smoke
  implicit none
  logical :: ok_on_rank, ok_on_comm

  integer :: mpi_err_stat
  integer :: comm_size, comm_rank
  integer, parameter :: comm = MPI_COMM_WORLD

  call mpi_init(mpi_err_stat)
  call mpi_comm_size(comm, comm_size, mpi_err_stat)
  call mpi_comm_rank(comm, comm_rank, mpi_err_stat)

  call init_fruit
!----------
! Our tests
  call simple_mpi_test(comm)
!----------

  call fruit_summary_mpi(comm_size, comm_rank)
  call is_all_successful(ok_on_rank)
  call mpi_allreduce(ok_on_rank, ok_on_comm, 1, MPI_LOGICAL, MPI_LAND, comm, mpi_err_stat)

  call fruit_finalize_mpi(comm_size, comm_rank)
  call mpi_finalize(mpi_err_stat)
  
  if (.not. ok_on_comm) then
     call exit(1)
  endif
end program fortran_mpi_test
