
OPENACC_INC_DIR?=$(OPENACC_DIR)/include
OPENACC_LIB_DIR?=$(OPENACC_DIR)/lib
OPENACC_LIB_NAME?=openacc
OPENACC_LIB_FILE=$(OPENACC_LIB_DIR)/lib$(OPENACC_LIB_NAME).so

OPENCL_LIB_DIR?=
OPENCL_LIB_NAME?=OpenCL

INCLUDES+=-I$(OPENACC_INC_DIR)

LIBS+=-L$(OPENACC_LIB_DIR) -l$(OPENACC_LIB_NAME) -l$(OPENCL_LIB_NAME) -lrt
DEPS+=$(OPENACC_LIB_FILE)

CC=gcc
CFLAGS+=-g -fno-stack-protector

CXX=g++
CXXFLAGS+=

LD=g++
LDFLAGS+=

MAKE=make

HOST_ENV_MACRO= \
  -DACC_RUNTIME_ABS_DIR=\"$(OPENACC_DIR)\" \
  -DACC_KERNELS_ABS_DIR=\"$(TEST_SRCDIR)\"

ACC_INTERNAL_DEPS= \
  $(OPENACC_INC_DIR)/OpenACC/internal/compiler.h \
  $(OPENACC_INC_DIR)/OpenACC/internal/region.h \
  $(OPENACC_INC_DIR)/OpenACC/internal/kernel.h \
  $(OPENACC_INC_DIR)/OpenACC/internal/loop.h

ACC_PRIVATE_DEPS= \
  $(OPENACC_INC_DIR)/OpenACC/private/debug.h \
  $(OPENACC_INC_DIR)/OpenACC/private/region.h \
  $(OPENACC_INC_DIR)/OpenACC/private/kernel.h \
  $(OPENACC_INC_DIR)/OpenACC/private/loop.h \
  $(OPENACC_INC_DIR)/OpenACC/private/data-env.h

ACC_PUBLIC_DEPS= \
  $(OPENACC_INC_DIR)/OpenACC/openacc.h \
  $(OPENACC_INC_DIR)/OpenACC/public/def.h \
  $(OPENACC_INC_DIR)/OpenACC/public/arch.h \
  $(OPENACC_INC_DIR)/OpenACC/public/device.h \
  $(OPENACC_INC_DIR)/OpenACC/public/async.h \
  $(OPENACC_INC_DIR)/OpenACC/public/runtime.h \
  $(OPENACC_INC_DIR)/OpenACC/public/memory.h

