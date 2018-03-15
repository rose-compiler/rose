program call_device_kernel
  integer, parameter :: N = 4096
  real, device :: x_d(N), y_d(N)

  call device_kernel<<<16, 256>>>(x_d, y_d)

end program
