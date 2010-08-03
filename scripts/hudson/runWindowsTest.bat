@echo running Windows


@echo BOOST_LIBRARIES : %BOOST_LIBRARIES%
@echo BOOST_ROOT : %BOOST_ROOT%
@echo Path : %Path%
@echo JAVA_HOME : %JAVA_HOME%

REM @echo 0 > rose_win_test.rose
REM pscp rose_win_test.rose hudson-rose@hudson-rose.llnl.gov:/nfs/casc/overture/ROSE/git/rose_win_test.rose

@echo Testing cl.exe
cl.exe

@echo Testing Bison.exe
bison.exe --version

@echo Testing flex.exe
flex.exe --version

@echo Testing cmake.exe
cmake.exe --version

@echo Current Dir : %CD%

@echo -----------------------------
@echo PHASE 1 : cmake configuration
@echo -----------------------------

@echo "First we need to delete the win-build directory because it contains old files"
@echo "Then we create it again and start our tests" 
rm -rf win-build
mkdir win-build
cd win-build

@echo cmake -DBOOST_ROOT=%BOOST_ROOT% -DCMAKE_BUILD_TYPE=Release -G"Visual Studio 9 2008"  ../ 
cmake -DBOOST_ROOT=%BOOST_ROOT% -DCMAKE_BUILD_TYPE=Release -G"Visual Studio 9 2008"  ../

@echo ----------------------------
@echo PHASE 2 : vcbuild
@echo ----------------------------

@echo "It is important to specify that we want the release version"
@echo "The debug version will not work because of problems with the debugging database = flag Zi"
vcbuild.exe ROSE.sln "Release|Win32"

@echo 1 > rose_win_test.rose
pscp rose_win_test.rose hudson-rose@hudson-rose.llnl.gov:/nfs/casc/overture/ROSE/git/rose_win_test.rose

