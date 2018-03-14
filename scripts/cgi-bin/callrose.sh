#!/bin/bash
# By Chunhua Liao, 12/16/2017

ROSE_INSTALL_PATH=/home/ubuntu/opt/rose_inst/bin

#------------------------------------------------------------------------------
# a function to display a file within a textarea
function display_code()
{
# automatically adjust line count
linecount=`wc -l $1 | cut -f 1 -d ' '`

#echo "Debug: in display_code(), linecount=$linecount for $1"

let "linecount+=2" # add a bit margin
#echo "<textarea rows=\"40\" cols=\"80\">"
echo "<textarea rows=\"$linecount\" cols=\"80\">"
# space sensitive here: '=' must immediately follow 'IFS' !
# otherwise it does not work!
# not sure why?
while IFS= read -r line
do
   echo "$line"
done < "$1"  

echo "</textarea><br>"
}

# expecting one parameter as the path to the source code
# Special characters like < or > in source code may not display properly in html. 
# <stdio.h> will be treated as a HTML tag and disappear!!
# the solution: #include <iostream>  -->  #include &lt;iostream&gt;
# too many special things to handle , switch to use textarea instead
function display_code_old_deprecated()
{
echo "<pre>"
# space sensitive here: '=' must immediately follow 'IFS' !
# otherwise it does not work!
# not sure why?
while IFS= read -r line
do
#convert < and > on the fly
   line2=`echo $line| sed "s/</\&lt;/g" | sed "s/>/\&gt;/g"`
   echo "$line2"
done < "$1"  
#done < "$file"  

echo "</pre>"
}


#------------------------------------------------------------------------------
#This code for getting code from post data is from http://oinkzwurgl.org/bash_cgi and
#was written by Phillippe Kehi <phkehi@gmx.net> and flipflip industries

# (internal) routine to store POST data
function cgi_get_POST_vars()
{
    # check content type
    # FIXME: not sure if we could handle uploads with this..
    [ "${CONTENT_TYPE}" != "application/x-www-form-urlencoded" ] && \
	echo "Warning: you should probably use MIME type "\
	     "application/x-www-form-urlencoded!" 1>&2
    # save POST variables (only first time this is called)
    [ -z "$QUERY_STRING_POST" \
      -a "$REQUEST_METHOD" = "POST" -a ! -z "$CONTENT_LENGTH" ] && \
	read -n $CONTENT_LENGTH QUERY_STRING_POST
    return
}

# (internal) routine to decode urlencoded strings
function cgi_decodevar()
{
    [ $# -ne 1 ] && return
    local v t h
    # replace all + with whitespace and append %%
    t="${1//+/ }%%"
    while [ ${#t} -gt 0 -a "${t}" != "%" ]; do
	v="${v}${t%%\%*}" # digest up to the first %
	t="${t#*%}"       # remove digested part
	# decode if there is anything to decode and if not at end of string
	if [ ${#t} -gt 0 -a "${t}" != "%" ]; then
	    h=${t:0:2} # save first two chars
	    t="${t:2}" # remove these
	    v="${v}"`echo -e \\\\x${h}` # convert hex to special char
	fi
    done
    # return decoded string
    echo "${v}"
    return
}

# routine to get variables from http requests
# usage: cgi_getvars method varname1 [.. varnameN]
# method is either GET or POST or BOTH
# the magic varible name ALL gets everything
function cgi_getvars()
{
    [ $# -lt 2 ] && return
    local q p k v s
    # get query
    case $1 in
	GET)
	    [ ! -z "${QUERY_STRING}" ] && q="${QUERY_STRING}&"
	    ;;
	POST)
	    cgi_get_POST_vars
	    [ ! -z "${QUERY_STRING_POST}" ] && q="${QUERY_STRING_POST}&"
	    ;;
	BOTH)
	    [ ! -z "${QUERY_STRING}" ] && q="${QUERY_STRING}&"
	    cgi_get_POST_vars
	    [ ! -z "${QUERY_STRING_POST}" ] && q="${q}${QUERY_STRING_POST}&"
	    ;;
    esac
    shift
    s=" $* "
    # parse the query data
    while [ ! -z "$q" ]; do
	p="${q%%&*}"  # get first part of query string
	k="${p%%=*}"  # get the key (variable name) from it
	v="${p#*=}"   # get the value from it
	q="${q#$p&*}" # strip first part from query string
	# decode and evaluate var if requested
	[ "$1" = "ALL" -o "${s/ $k /}" != "$s" ] && \
	    eval "$k=\"`cgi_decodevar \"$v\"`\""
    done
    return
}

#------------------------------------------------------------------------------
# register all GET and POST variables
cgi_getvars BOTH ALL

echo "Content-type: text/html"
echo ""


# this line is problematic somehow, don't know why.
#options=`echo "$QUERY_STRING" | sed -n 's/^.*options=\([^&]*\).*$/\1/p' | sed "s/%20/ /g"
# sed -e "s/^M/\n/g" test.c >test2.c
filecontent=`echo "$QUERY_STRING" | sed -n 's/^.*filecontent=\([^&]*\).*$/\1/p' | sed "s/%20/ /g" 
suffix=`echo "$QUERY_STRING" | sed -n 's/^.*suffix=\([^&]*\).*$/\1/p' | sed "s/%20/ /g"
#rosetool=`echo "$QUERY_STRING" | sed -n 's/^.*rosetool=\([^&]*\).*$/\1/p' | sed "s/%20/ /g"
roseTranslator="$ROSE_INSTALL_PATH/identityTranslator"

#------------------------------------------------------------------------------
echo "<html><head>"
echo "<script>"
echo "function goBack() {"
echo "      window.history.back();"
echo "} "
echo "</script>"
echo "<title>Execution results:</title>"
echo "</head>"

echo "<body>"
#echo "Debug: rosetool selected is: $roseTranslator<br />"
#echo "query string is: $QUERY_STRING<br />"
#using a process id to avoid conflicting filenames when this script is called concurrently.

mypid=$$
#echo "<p>"
#echo "options are $options<br />"
#echo "<p>"
echo "<p>"
echo "Selected file suffix is $suffix<br />"
tmp_input0="/tmp/test0$mypid.$suffix"
tmp_input="/tmp/test$mypid.$suffix"

echo "$filecontent" > "$tmp_input0"
# preprocess the file content
#  is inputed by typing ctrl-v followed by ctrl-m
# replace  with \n 
sed -e "s//\n/g" "$tmp_input0" > "$tmp_input"

#echo "Debug: Input file is: "$tmp_input" <br />"
echo "Input file is: <br />"
display_code "$tmp_input"

# change the current directory to /tmp, to avoid file permission problem
cd /tmp
rm -f /tmp/rose_test*.$suffix
#echo "Debug: final command line is: $roseTranslator -c /tmp/test$mypid.$suffix -rose:output /tmp/rose_test$mypid.$suffix &>/tmp/test$mypid.$suffix.output <br />"
#gcc -E /tmp/test.$suffix -o /tmp/rose_test.$suffix
# be very carefull about the file and dir permission here
# the translator will write rose_*.c and *.o file to two places respectively
# cgi-bin for the .o file and /tmp for the output source file

$roseTranslator -c /tmp/test$mypid.$suffix -rose:output /tmp/rose_test$mypid.$suffix &>/tmp/test$mypid.$suffix.output

if [ $? -eq 0 ]
then
  echo "Compilation is successful!"
  echo "<br />"
  PROCESSED=`cat /tmp/rose_test$mypid."$suffix"`

  #echo "Output file is: /tmp/rose_test$mypid.$suffix <br />"
  echo "Output file is:<br />"
  # reading and output a file using bash
  outputfile="/tmp/rose_test$mypid.$suffix"
  display_code $outputfile
else  
  echo "Compilation failed!"
fi 

echo "Compilation message (empty if no warnings or errors): <br />"
#  echo "Log file is: /tmp/test.$suffix.output <br />"
display_code "/tmp/test$mypid.$suffix.output"
#echo "return $? after running:  $roseTranslator -c /tmp/test.$suffix -rose:output /tmp/rose_test.$suffix &>/tmp/test.$suffix.output <br />" 

echo "<p>"
echo "Version of the translator used is:<br />"
$roseTranslator --version &>/tmp/rose$mypid.version
display_code "/tmp/rose$mypid.version"

echo "<p>"
echo "<button onclick=\"goBack()\">Go Back To Try Another One</button>"

echo "</body></html>"

