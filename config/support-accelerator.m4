
AC_DEFUN([GENERATE_CUDA_SPECIFIC_HEADERS],
[
   mkdir -p "./include-staging/cuda_HEADERS"
   cp ${srcdir}/config/preinclude-cuda.h ./include-staging/cuda_HEADERS
])

AC_DEFUN([ROSE_CHECK_CUDA],
[

AC_ARG_WITH(cuda-inc,   [  --with-cuda-inc=DIR    For CUDA Headers (API)])
AC_ARG_WITH(cuda-lib,   [  --with-cuda-lib=DIR    For CUDA Library (Runtime) (need --with-cuda-inc=DIR)])

# CUDA header files
if test "$with_cuda_inc" != ""
then
        AC_SUBST(CUDA_INC_DIR, [$with_cuda_inc])
fi
AM_CONDITIONAL(ROSE_HAVE_CUDA_INC, [test "$with_cuda_inc" != ""])

# CUDA library
if test "$with_cuda_lib" != ""
then
        AC_SUBST(CUDA_LIB_DIR, [$with_cuda_lib])
        if test "$with_cuda_inc" == ""
        then
                 AC_ERROR([Need CUDA headers directory if the libraries directory are provided])
        fi
fi
AM_CONDITIONAL(ROSE_HAVE_CUDA_LIB, test "$with_cuda_lib" != "")

])

AC_DEFUN([GENERATE_OPENCL_SPECIFIC_HEADERS],
[
   mkdir -p "./include-staging/opencl_HEADERS"
   cp ${srcdir}/config/preinclude-opencl.h ./include-staging/opencl_HEADERS
])

AC_DEFUN([ROSE_CHECK_OPENCL],
[

AC_ARG_WITH(opencl-inc, [  --with-opencl-inc=DIR  For OpenCL Headers (API)])
AC_ARG_WITH(opencl-lib, [  --with-opencl-lib=DIR  For OpenCL Library (Runtime) (need --with-opencl-inc=DIR)])

# OpenCL header files
if test "$with_opencl_inc" != ""
then
        AC_SUBST(OPENCL_INC_DIR, [$with_opencl_inc])
fi
AM_CONDITIONAL(ROSE_HAVE_OPENCL_INC, [test "$with_opencl_inc" != ""])

# OpenCL library
if test "$with_opencl_lib" != ""
then
        AC_SUBST(OPENCL_LIB_DIR, [$with_opencl_lib])
        if test "$with_opencl_inc" == ""
        then
                 AC_ERROR([Need OpenCL headers directory if the libraries directory are provided])
        fi
fi
AM_CONDITIONAL(ROSE_HAVE_OPENCL_LIB, test "$with_opencl_lib" != "")

])

AC_DEFUN([ROSE_CHECK_OPENACC],
[

AC_ARG_WITH(openacc-inc, [  --with-openacc-inc=DIR  For OpenACC Headers (API)])

# OpenACC header files
if test "$with_openacc_inc" != ""
then
        AC_SUBST(OPENACC_INC_DIR, [$with_openacc_inc])
fi
AM_CONDITIONAL(ROSE_HAVE_OPENACC_INC, [test "$with_openacc_inc" != ""])

])
