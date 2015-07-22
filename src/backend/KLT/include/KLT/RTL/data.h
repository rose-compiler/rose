
#ifndef __KLT_RTL_DATA_H__
#define __KLT_RTL_DATA_H__

struct klt_data_container_t {
  int num_param;
  int * sizeof_param;

  int num_scalar;
  int * sizeof_scalar;

  int num_data;
  int * sizeof_data;
  int * ndims_data;

  int num_priv;
  int * sizeof_priv;
  int * ndims_priv;
};


struct klt_data_section_t {
  int offset;
  int length;
//int stride;
};

struct klt_data_t {
  void * ptr;
  struct klt_data_section_t * sections;
};

#endif /* __KLT_RTL_DATA_H__ */

