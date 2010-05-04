@echo running Windows

@echo BOOST_LIBRARIES : %BOOST_LIBRARIES%
@echo BOOST_ROOT : %BOOST_ROOT%
@echo Path : %Path%
@echo JAVA_HOME : %JAVA_HOME%

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

mkdir win-build
cd win-build

@echo cmake -DBOOST_ROOT=%BOOST_ROOT% -DCMAKE_BUILD_TYPE=Release -G"Visual Studio 9 2008"  ../ 
cmake -DBOOST_ROOT=%BOOST_ROOT% -DCMAKE_BUILD_TYPE=Release -G"Visual Studio 9 2008"  ../

@echo ----------------------------
@echo PHASE 2 : vcbuild
@echo ----------------------------

vcbuild.exe ZERO_CHECK.vcproj "Release|Win32"




