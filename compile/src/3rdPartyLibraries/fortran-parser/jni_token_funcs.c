/******************************************************************************
 * Copyright (c) 2005, 2006 Los Alamos National Security, LLC.  This
 * material was produced under U.S. Government contract
 * DE-AC52-06NA25396 for Los Alamos National Laboratory (LANL), which
 * is operated by the Los Alamos National Security, LLC (LANS) for the
 * U.S. Department of Energy. The U.S. Government has rights to use,
 * reproduce, and distribute this software. NEITHER THE GOVERNMENT NOR
 * LANS MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY
 * LIABILITY FOR THE USE OF THIS SOFTWARE. If software is modified to
 * produce derivative works, such modified software should be clearly
 * marked, so as not to confuse it with the version available from
 * LANL.
 *
 * Additionally, this program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>  
#include "jni.h"
#include "token.h"

  
Token_t *convert_Java_token(JNIEnv *env, jobject token)
{
  jclass cls;
  jint type;
  jstring java_string;
  const char *text = NULL;
  jmethodID method_id;
  Token_t *our_token = NULL;
  int line = -1;
  int col = -1;
  
  /* Need to get the object class so we can use it to retrieve the methods.  */
  cls = (*env)->GetObjectClass(env, token);
  /* Get the method ID for the getType() method.  */
  method_id = (*env)->GetMethodID(env, cls, "getType", "()I");
  /* Call the getType method to get the token type.  */
  type = (*env)->CallIntMethod(env, token, method_id);

  /* Get the method ID for the getText() method.  */
  method_id = (*env)->GetMethodID(env, cls, "getText", "()Ljava/lang/String;");
  /* Call getText() to get the Java String object.  */
  java_string = (*env)->CallObjectMethod(env, token, method_id);
  /* Get a C char string for the Java String if the Java String exists.  */
  if(java_string != NULL)
  {
    text = (*env)->GetStringUTFChars(env, java_string, NULL);
    if(text == NULL)
    {
      fprintf(stderr, "Unable to retrieve text string from String object\n");
      exit(1);
    }
  }
  else
  {
    text = NULL;
  }

  /* Get the method ID for the getLine() method.  */
  method_id = (*env)->GetMethodID(env, cls, "getLine", "()I");
  /* Call the getLine() method to get the line number for this token.  */
  line = (*env)->CallIntMethod(env, token, method_id);

  /* Get the method ID for the getCharPositionInLine() method.  */
  method_id = (*env)->GetMethodID(env, cls, "getCharPositionInLine", "()I");
  /* Call getCharPositionInLine() to get the column position for the token.  */
  col = (*env)->CallIntMethod(env, token, method_id);
  
  /* build a Token of our own.  */
  our_token = create_token(line, col, type, text);

  /* Release the string now that we've made a new copy for ourselves.  */
  if(java_string != NULL)
    (*env)->ReleaseStringUTFChars(env, java_string, text);

  return our_token;
}

#ifdef __cplusplus
} /* End extern C. */
#endif
  
