
#ifndef __KLT_RTL_DATA_H__
#define __KLT_RTL_DATA_H__

struct klt_data_container_t {
  int num_param;
  int * sizeof_param;

  int num_data;
  int * sizeof_data;
  int * ndims_data;
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

struct klt_data_environment_t {
  struct klt_data_environment_t * parent;

  // TODO
};

#endif /* __KLT_RTL_DATA_H__ */

