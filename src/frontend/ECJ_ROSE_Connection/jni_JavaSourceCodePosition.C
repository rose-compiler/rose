#include<string>

// #include "jni.h"
// #include "token.h"
#include "jni_JavaSourceCodePosition.h"
#include "JavaSourceCodePosition.h"
#include <assert.h>

// Token_t* convert_Java_token(JNIEnv *env, jobject token)
  
JavaSourceCodePosition* 
convert_Java_SourcePosition(JNIEnv *env, jobject sourcePosition)
   {
     jclass cls;
     jstring java_string;
     const char *text = NULL;
     jmethodID method_id;
     JavaSourceCodePosition *our_token = NULL;

     int line_start = -1;
     int line_end   = -1;
     int col_start  = -1;
     int col_end    = -1;

     assert(env != NULL);

  // Need to get the object class so we can use it to retrieve the methods.
  // printf ("Need to get the object class so we can use it to retrieve the methods. \n");  
     cls = env->GetObjectClass(sourcePosition);

     if ( cls != (jclass)0 )
        {
       // printf ("jclass cls is OK! \n");
        }
       else
        {
          printf ("Error: jclass cls == NULL! \n");
          assert(false);
        }

#if 0
  // ************************
  // Get the text information
  // ************************

  // Get the method ID for the getText() method.
  // printf ("Get the method ID for the getText() method. \n");
     method_id = env->GetMethodID(cls, "getText", "()Ljava/lang/String;");

     if ( method_id != NULL )
        {
       // printf ("jmethodID method_id is OK! \n");
        }
       else
        {
          printf ("Error: jmethodID method_id == NULL (method not found) \n");
          assert(false);
        }

  // Call getText() to get the Java String object (note that the cast is critical!).
  // printf ("Call getText() to get the Java String object. \n");
     java_string = (jstring) env->CallObjectMethod(sourcePosition, method_id);

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
#else
     text = "foobar";
#endif

  // ************************
  // Get the line information
  // ************************

  // Get the method ID for the getLine() method.
  // printf ("Get the method ID for the getLine() method. \n");
     method_id = env->GetMethodID(cls, "getLineStart", "()I");

     if ( method_id != NULL )
        {
       // printf ("jmethodID method_id is OK! \n");
        }
       else
        {
          printf ("Error: jmethodID method_id == NULL (method not found) \n");
          assert(false);
        }

#if 1
  // Call the getLine() method to get the line number for this token.
  // printf ("Call the getLine() method to get the line number for this token. \n");
     line_start = (int) env->CallIntMethod(sourcePosition, method_id);
#else
     line_start = -1;
#endif

  // Get the method ID for the getLine() method.
  // printf ("Get the method ID for the getLine() method. \n");
     method_id = env->GetMethodID(cls, "getLineEnd", "()I");

     if ( method_id != NULL )
        {
       // printf ("jmethodID method_id is OK! \n");
        }
       else
        {
          printf ("Error: jmethodID method_id == NULL (method not found) \n");
          assert(false);
        }

#if 1
  // Call the getLine() method to get the line number for this token.
  // printf ("Call the getLine() method to get the line number for this token. \n");
     line_end = (int) env->CallIntMethod(sourcePosition, method_id);
#else
     line_end = -1;
#endif


  // **************************
  // Get the column information
  // **************************

  // Get the method ID for the getCharPositionInLine() method.
  // printf ("Get the method ID for the getCharPositionInLine() method. \n");
     method_id = env->GetMethodID(cls, "getColumnStart", "()I");

     if ( method_id != NULL )
        {
       // printf ("jmethodID method_id is OK! \n");
        }
       else
        {
          printf ("Error: jmethodID method_id == NULL (method not found) \n");
          assert(false);
        }

#if 1
  // Call getCharPositionInLine() to get the column position for the token.
  // printf ("Call getCharPositionInLine() to get the column position for the token. \n");
     col_start = (int) env->CallIntMethod(sourcePosition, method_id);
#else
     col_start = -1;
#endif

  // Get the method ID for the getCharPositionInLine() method.
  // printf ("Get the method ID for the getCharPositionInLine() method. \n");
     method_id = env->GetMethodID(cls, "getColumnEnd", "()I");

     if ( method_id != NULL )
        {
       // printf ("jmethodID method_id is OK! \n");
        }
       else
        {
          printf ("Error: jmethodID method_id == NULL (method not found) \n");
          assert(false);
        }

#if 1
  // Call getCharPositionInLine() to get the column position for the token.
  // printf ("Call getCharPositionInLine() to get the column position for the token. \n");
     col_end = (int) env->CallIntMethod(sourcePosition, method_id);
#else
     col_end = -1;
#endif

  // build a Token of our own.
  // printf ("build a Token of our own. \n");
     our_token = new JavaSourceCodePosition(line_start,line_end,col_start,col_end);

#if 0
  // Release the string now that we've made a new copy for ourselves.
  // printf ("Release the string now that we've made a new copy for ourselves. \n");
     if (java_string != NULL)
          env->ReleaseStringUTFChars(java_string, text);
#endif

  // printf ("returning from convert_Java_token()\n");
     return our_token;
   }

