%naturalvar SgName;

%typemap(jni) SgName "jstring"
%typemap(jtype) SgName "String"
%typemap(jstype) SgName "String"
%typemap(javadirectorin) SgName "$jniinput"
%typemap(javadirectorout) SgName "$javacall"

%typemap(in) SgName
%{
  if(!$input) {
    SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "null SgName");
    return $null;
  }
  const char *$1_pstr = (const char *)jenv->GetStringUTFChars($input, 0);
  if (!$1_pstr) return $null;
  $1 =  SgName($1_pstr);
  jenv->ReleaseStringUTFChars($input, $1_pstr);
%}

%typemap(directorout) SgName
%{
  if(!$input) {
    SWIG_JavaThrowException(jenv, SWIG_JavaNullPointerException, "null SgName");
    return $null;
  }
  const char *$1_pstr = (const char *)jenv->GetStringUTFChars($input, 0);
  if (!$1_pstr) return $null;
  $result =  SgName($1_pstr);
  jenv->ReleaseStringUTFChars($input, $1_pstr);
%}

%typemap(directorin,descriptor="S") SgName
%{ $input = jenv->NewStringUTF($1.str()); %}

%typemap(out) SgName
%{ $result = jenv->NewStringUTF($1.str()); %}

%typemap(javain) SgName "$javainput"

%typemap(javaout) SgName {
  return $jnicall;
}
