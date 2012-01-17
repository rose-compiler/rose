#include<string>

#include "jni_token.h"
#include "jni_utils.h"
#include "jni_JavaSourceCodePosition.h"
#include <assert.h>
  
Token_t*
convert_Java_token(JNIEnv *env, jobject token)
   {
     jclass cls;
     jstring java_string;
     const char *text = NULL;
     jmethodID method_id;
     Token_t *our_token = NULL;
     JavaSourceCodePosition * pos_info = NULL;
     jobject jpos_info;

     assert(env != NULL);

  // Need to get the object class so we can use it to retrieve the methods.
  // printf ("Need to get the object class so we can use it to retrieve the methods. \n");  
     cls = get_class(env, token);

  // Get the method ID for the getText() method.
  // printf ("Get the method ID for the getText() method. \n");
     method_id = get_method(env, cls, "getText", "()Ljava/lang/String;");

  // Call getText() to get the Java String object (note that the cast is critical!).
  // printf ("Call getText() to get the Java String object. \n");
     java_string = (jstring) env->CallObjectMethod(token, method_id);

  // Get a C char string for the Java String if the Java String exists.
  // printf ("Get a C char string for the Java String if the Java String exists. \n");
     if (java_string != NULL)
        {
          text = env->GetStringUTFChars(java_string, NULL);
          if (text == NULL)
             {
               fprintf(stderr, "Unable to retrieve text string from String object\n");
               assert(false);
             }
        }
       else
        {
          text = NULL;
        }

  // Get the method ID for the getJavaSourcePositionInformation() method.
     method_id = get_method(env, cls, "getJavaSourcePositionInformation", "()LJavaSourcePositionInformation;");
     jpos_info = (jobject) env->CallObjectMethod(token, method_id);

     // Convert position to its C-based representation
     pos_info = convert_Java_SourcePosition(env, jpos_info);

     // Build a C-based representation of the JavaToken
     our_token = new Token_t(text, pos_info);

#if 0
  // Release the string now that we've made a new copy for ourselves.
  // printf ("Release the string now that we've made a new copy for ourselves. \n");
     if (java_string != NULL)
          env->ReleaseStringUTFChars(java_string, text);
#endif

  // printf ("returning from convert_Java_token()\n");
     return our_token;
   }

