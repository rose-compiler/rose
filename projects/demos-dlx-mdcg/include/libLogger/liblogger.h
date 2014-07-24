
#ifndef __LIBLOGGER_H__
#define __LIBLOGGER_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef size_t log_id_t;

void logger_log_before(log_id_t log_id, ...);
void logger_log_after(log_id_t log_id, ...);

struct logger_param_t {
  size_t size;
};

struct logger_cond_t {
  size_t size;
  void * value;
};

struct logger_point_t {
  enum { e_before, e_after, e_both } where;
  char * message;
  size_t num_params;
  struct logger_param_t * params;
  size_t num_conds;
  struct logger_cond_t * conds;
};

struct logger_data_t {
  size_t num_points;
  struct logger_point_t * points;
};

extern struct logger_data_t logger_data;

#ifdef __cplusplus
}
#endif

#endif /* __LIBLOGGER_H__ */

