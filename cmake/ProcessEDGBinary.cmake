execute_process(COMMAND cat ${CMAKE_ARGV3} OUTPUT_VARIABLE signature OUTPUT_STRIP_TRAILING_WHITESPACE)
message("signature='${signature}'")
set(tarball_site "http://edg-binaries.rosecompiler.org")
# example url
# http://edg-binaries.rosecompiler.org/roseBinaryEDG-5-0-x86_64-pc-linux-gnu-gnu-8-5.0.11.80.1.tar.gz
set(workingDirectory "${CMAKE_BINARY_DIR}/src/frontend/CxxFrontend")
set(tarball_filename "roseBinaryEDG-${CMAKE_ARGV4}-${CMAKE_ARGV5}-${CMAKE_ARGV6}-${signature}")
set(tarball_fullname "roseBinaryEDG-${CMAKE_ARGV4}-${CMAKE_ARGV5}-${CMAKE_ARGV6}-${signature}.tar.gz")
message("fetching ${tarball_site}/${tarball_fullname}")
# use temporary file for download, so we dont try to unpack a partial download in "EDG - unpacking EDG binary tar file"
execute_process(COMMAND wget -O ${workingDirectory}/EDG.tar.gz.tmp ${tarball_site}/${tarball_fullname} WORKING_DIRECTORY ${workingDirectory} COMMAND_ECHO STDOUT COMMAND_ERROR_IS_FATAL ANY)
execute_process(COMMAND mv ${workingDirectory}/EDG.tar.gz.tmp ${workingDirectory}/EDG.tar.gz COMMAND_ECHO STDOUT COMMAND_ERROR_IS_FATAL ANY)
message("unpacking EDG.tar.gz")
execute_process(COMMAND tar zxvf ${workingDirectory}/EDG.tar.gz -C ${CMAKE_BINARY_DIR}/src/frontend/CxxFrontend/ WORKING_DIRECTORY ${workingDirectory} COMMAND_ECHO STDOUT COMMAND_ERROR_IS_FATAL ANY)
message("removing old EDG files")
execute_process(COMMAND rm -rf ${workingDirectory}/EDG WORKING_DIRECTORY ${workingDirectory} COMMAND_ECHO STDOUT COMMAND_ERROR_IS_FATAL ANY) 
message("moving EDG files")
execute_process(COMMAND mv ${workingDirectory}/${tarball_filename} ${workingDirectory}/EDG WORKING_DIRECTORY ${workingDirectory} COMMAND_ECHO STDOUT COMMAND_ERROR_IS_FATAL ANY)
execute_process(COMMAND touch ${workingDirectory}/EDG/libroseEDG.la WORKING_DIRECTORY ${workingDirectory} COMMAND_ECHO STDOUT COMMAND_ERROR_IS_FATAL ANY)
