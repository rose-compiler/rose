#include<string>

#include "jni.h"
#include "token.h"
#include <assert.h>

  
Token_t*
convert_Java_token(JNIEnv *env, jobject token)
   {
     jclass cls;
  // jint type;
     jstring java_string;
     const char *text = NULL;
     jmethodID method_id;
     Token_t *our_token = NULL;
     int line = -1;
     int col = -1;

     assert(env != NULL);

     printf ("Need to get the object class so we can use it to retrieve the methods. \n");  
  // Need to get the object class so we can use it to retrieve the methods.
     cls = env->GetObjectClass(token);

     if ( cls != (jclass)0 )
        {
          printf ("jclass cls is OK! \n");
        }
       else
        {
          printf ("Error: jclass cls == NULL! \n");
          assert(false);
        }

#if 1
     printf ("Get the method ID for the getText() method. \n");
  // Get the method ID for the getText() method.
     method_id = env->GetMethodID(cls, "getText", "()Ljava/lang/String;");

     if ( method_id != NULL )
        {
          printf ("jmethodID method_id is OK! \n");
        }
       else
        {
          printf ("Error: jmethodID method_id == NULL (method not found) \n");
          assert(false);
        }

     printf ("Call getText() to get the Java String object. \n");
  // Call getText() to get the Java String object.
     java_string = (jstring) env->CallObjectMethod(token, method_id);

     printf ("Get a C char string for the Java String if the Java String exists. \n");
  // Get a C char string for the Java String if the Java String exists.
     if (java_string != NULL)
        {
          text = env->GetStringUTFChars(java_string, NULL);
          if (text == NULL)
             {
               fprintf(stderr, "Unable to retrieve text string from String object\n");
               exit(1);
             }
        }
       else
        {
          text = NULL;
        }
#else
     text = "foobar";
#endif

     printf ("Get the method ID for the getLine() method. \n");
  // Get the method ID for the getLine() method.
     method_id = env->GetMethodID(cls, "getLine", "()I");

     if ( method_id != NULL )
        {
          printf ("jmethodID method_id is OK! \n");
        }
       else
        {
          printf ("Error: jmethodID method_id == NULL (method not found) \n");
          assert(false);
        }

#if 1
     printf ("Call the getLine() method to get the line number for this token. \n");
  // Call the getLine() method to get the line number for this token.
  // line = env->CallIntMethod(token, method_id);
     line = (int) env->CallIntMethod(token, method_id);
#else
     line = -1;
#endif

     printf ("Get the method ID for the getCharPositionInLine() method. \n");
  // Get the method ID for the getCharPositionInLine() method.
     method_id = env->GetMethodID(cls, "getColumn", "()I");

     if ( method_id != NULL )
        {
          printf ("jmethodID method_id is OK! \n");
        }
       else
        {
          printf ("Error: jmethodID method_id == NULL (method not found) \n");
          assert(false);
        }

#if 1
     printf ("Call getCharPositionInLine() to get the column position for the token. \n");
  // Call getCharPositionInLine() to get the column position for the token.
     col = (int) env->CallIntMethod(token, method_id);
#else
     col = -1;
#endif

     printf ("build a Token of our own. \n");
  // build a Token of our own.
     our_token = new Token_t(text, line, col);

#if 0
     printf ("Release the string now that we've made a new copy for ourselves. \n");
  // Release the string now that we've made a new copy for ourselves.
     if (java_string != NULL)
          env->ReleaseStringUTFChars(java_string, text);
#endif

     printf ("returning from convert_Java_token()\n");
     return our_token;
   }

