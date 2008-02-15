%typemap(jni) wchar_t "jshort"
%typemap(jtype) wchar_t "short"
%typemap(jstype) wchar_t "short"
%typemap(javadirectorin) wchar_t "$jniinput"
%typemap(javadirectorout) wchar_t "$javacall"

%typemap(in) wchar_t
%{
  $1 = (wchar_t) $input;
%}

%typemap(directorout) wchar_t
%{
  $1 = (wchar_t) $input;
%}

%typemap(directorin,descriptor="S") wchar_t
%{ $input = (jshort) $1; %}

%typemap(out) wchar_t
%{ $result = (jshort)$1; %}

%typemap(javain) wchar_t "$javainput"

%typemap(javaout) wchar_t {
    return $jnicall;
  }
