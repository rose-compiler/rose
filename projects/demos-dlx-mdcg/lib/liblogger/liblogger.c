
#include "libLogger/liblogger.h"

#include <stdio.h>
#include <string.h>

#include <stdarg.h>

#include <assert.h>

void logger_log_before(log_id_t log_id, ...) {
  assert(log_id < logger_data.num_points);

  void * args = ((void *)&log_id) + sizeof(log_id_t);

  if (logger_data.points[log_id].where == e_before || logger_data.points[log_id].where == e_both) {
    size_t i;
    for (i = 0; i < logger_data.points[log_id].num_params; i++)
      args += logger_data.points[log_id].params[i].size;
    for (i = 0; i < logger_data.points[log_id].num_conds; i++) {
      if (memcmp(logger_data.points[log_id].conds[i].value, args, logger_data.points[log_id].conds[i].size) != 0) return;
      args += logger_data.points[log_id].conds[i].size;
    }
    va_list va_args;
    va_start(va_args, log_id);
    vprintf(logger_data.points[log_id].message, va_args);
    va_end(va_args);
  }
}


void logger_log_after(log_id_t log_id, ...) {
  assert(log_id < logger_data.num_points);

  void * args = ((void *)&log_id) + sizeof(log_id_t);

  if (logger_data.points[log_id].where == e_after || logger_data.points[log_id].where == e_both) {
    size_t i, param_size = 0;
    for (i = 0; i < logger_data.points[log_id].num_params; i++) {
      args += logger_data.points[log_id].params[i].size;
    }
    for (i = 0; i < logger_data.points[log_id].num_conds; i++) {
      if (memcmp(logger_data.points[log_id].conds[i].value, args, logger_data.points[log_id].conds[i].size) != 0) return;
      args += logger_data.points[log_id].conds[i].size;
    }
    va_list va_args;
    va_start(va_args, log_id);
    vprintf(logger_data.points[log_id].message, va_args);
    va_end(va_args);
  }
}

