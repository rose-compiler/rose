# Setting up Java:

setenv JAVA_PATH /usr/apps/java/latest
set path=($JAVA_PATH/bin $path)

# 64-bit
setenv LD_LIBRARY_PATH ${JAVA_PATH}/jre/lib/amd64/server:$LD_LIBRARY_PATH
# 32-bit
# setenv LD_LIBRARY_PATH ${JAVA_PATH}/jre/lib/i386/server:$LD_LIBRARY_PATH

# Setting up Boost:
setenv MY_BOOST_ROOT /home/dquinlan/local/boost_1_37_0.gxx-4.2.4

setenv LD_LIBRARY_PATH {$MY_BOOST_ROOT}/lib:$LD_LIBRARY_PATH
setenv LD_LIBRARY_PATH {$MY_BOOST_ROOT}/lib64:$LD_LIBRARY_PATH

