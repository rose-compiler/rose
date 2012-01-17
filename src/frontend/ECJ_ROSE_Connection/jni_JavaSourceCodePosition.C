#include <string>
#include <assert.h>
#include "jni_JavaSourceCodePosition.h"
#include "jni_utils.h"

JavaSourceCodePosition*
convert_Java_SourcePosition(JNIEnv *env, jobject sourcePosition)
   {
     jclass cls;
     jmethodID method_id;
     JavaSourceCodePosition *pos_info = NULL;

     int line_start = -1;
     int line_end   = -1;
     int col_start  = -1;
     int col_end    = -1;

     assert(env != NULL);

  // Need to get the object class so we can use it to retrieve the methods.
     cls = get_class(env, sourcePosition);

  // ************************
  // Get the line information
  // ************************
     method_id = get_method(env, cls, "getLineStart", "()I");
     line_start = (int) env->CallIntMethod(sourcePosition, method_id);

     method_id = get_method(env, cls, "getLineEnd", "()I");
     line_end = (int) env->CallIntMethod(sourcePosition, method_id);

  // **************************
  // Get the column information
  // **************************
     method_id = get_method(env, cls, "getColumnStart", "()I");
     col_start = (int) env->CallIntMethod(sourcePosition, method_id);

     method_id = get_method(env, cls, "getColumnEnd", "()I");
     col_end = (int) env->CallIntMethod(sourcePosition, method_id);

  // build a Token of our own.
     pos_info = new JavaSourceCodePosition(line_start,line_end,col_start,col_end);

     return pos_info;
   }

