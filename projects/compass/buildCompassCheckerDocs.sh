#!/bin/sh

cat <<'END'
\newenvironment{NOINDENT}%
{\noindent\ignorespaces}%
{\par\noindent%
\ignorespacesafterend}
END

cut -f 1 -d \  | \
while read checkerName; do
  echo '\begin{NOINDENT}'
  echo '\newpage'
  echo "\\input{${checkerName}Docs.new.tex}"
  echo '\end{NOINDENT}'
done
