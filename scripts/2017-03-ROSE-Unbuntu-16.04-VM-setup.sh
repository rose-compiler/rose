#!/bin/bash
# Author: Jordan Riley riley39@llnl.gov

# A script to install ROSE within a virtual machine based on Ubuntu 16.05.
# The content is described in https://en.wikibooks.org/wiki/ROSE_Compiler_Framework/Virtual_Machine_Image#V3 

apt-get update 
apt-get -y upgrade
apt-get -y install git wget build-essential libtool automake flex bison python3-dev

#---------- change gcc to 4.9.3 ----------

apt-get -y install gcc-4.9 g++-4.9 gfortran-4.9
update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.9 100
update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.9 100
update-alternatives --install /usr/bin/gfortran gfortran /usr/bin/gfortran-4.9 100

#---------- install jdk ----------

# download jdk-7u51-linux-x64.tar.gz

cd ~/Downloads
wget --no-cookies \
--no-check-certificate \
--header "Cookie: oraclelicense=accept-securebackup-cookie" \
"http://download.oracle.com/otn-pub/java/jdk/7u51-b13/jdk-7u51-linux-x64.tar.gz" \
-O jdk-7u51-linux-x64.tar.gz
# then
#tar -xzvf jdk-7-linux-x64.tar.gz

mkdir -p ~/opt/jvm
cd ~/opt/jvm
tar -xzvf ~/Downloads/jdk-7u51-linux-x64.tar.gz 

PATH=/home/demo/opt/jvm/jdk1.7.0_51/bin:$PATH
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/demo/opt/jvm/jdk1.7.0_51/jre/lib/amd64/server:/home/demo/opt/jvm/jdk1.7.0_51/lib
export PATH LD_LIBRARY_PATH

# add to .bashrc
sed -i '$ a export PATH=/home/demo/opt/jvm/jdk1.7.0_51/bin:$PATH\nexport LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/demo/opt/jvm/jdk1.7.0_51/jre/lib/amd64/server:/home/demo/opt/jvm/jdk1.7.0_51/lib\n' $HOME/.bashrc

#open new terminal
#check that javac -version says javac 1.7.0_51

#---------- installing Boost ----------

# download boost from
# https://sourceforge.net/projects/boost/files/boost/1.61.0/

cd ~/Downloads
wget -O boost-1.61.0.tar.bz2 http://sourceforge.net/projects/boost/files/boost/1.61.0/boost_1_61_0.tar.bz2/download
tar xf boost-1.61.0.tar.bz2
cd boost_1_61_0
./bootstrap.sh --prefix=/home/demo/opt/boost/1.61.0/gcc-4.9.3-default --with-libraries=chrono,date_time,filesystem,iostreams,program_options,random,regex,serialization,signals,system,thread,wave
./b2 -sNO_BZIP2=1 install

export LD_LIBRARY_PATH=/home/demo/opt/boost/1.61.0/gcc-4.9.3-default/lib:$LD_LIBRARY_PATH
# add to .bashrc
sed -i '$ a export LD_LIBRARY_PATH=/home/demo/opt/boost/1.61.0/gcc-4.9.3-default/lib:$LD_LIBRARY_PATH\n' $HOME/.bashrc

#---------- install zgrviewer ----------

apt-get -y install graphviz

#Download and untar the ZGRViewer distribution.
cd ~/Downloads
wget -O zgrviewer-0.10.0.zip https://sourceforge.net/projects/zvtm/files/zgrviewer/0.10.0/zgrviewer-0.10.0.zip/download

cd ~/opt/
unzip ~/Downloads/zgrviewer-0.10.0.zip 
cd zgrviewer-0.10.0

# Edit it's "run.sh" script so that the ZGRV_HOME variable has the correct value. 
# The scripts/zgrviewerExampleScript has some additional java switches that are useful.
# ZGRV_HOME=/home/demo/opt/zgrviewer-0.10.0
sed -i "/ZGRV_HOME=\./c\ZGRV_HOME=/home/demo/opt/zgrviewer-0.10.0/" run.sh

# Edit ~/.bashrc and add an alias that allows you to run ZGRViewer by typing "zgrviewer":
# alias zgrviewer='/home/demo/opt/zgrviewer-0.10.0/run.sh'
sed -i '$ a alias zgrviewer='/home/demo/opt/zgrviewer-0.10.0/run.sh'\n' $HOME/.bashrc

#---------------------------

# doxygen
apt-get -y install doxygen 

#---------------------------

# latex
apt-get -y install texlive

#---------------------------

# Dlib.
# --with-dlib='/home/demo/opt/dlib/18.18'
# Download tarball from http://dlib.net/  
# or https://sourceforge.net/projects/dclib/files/dlib/v18.18/
# unpack into desired installation directory

cd ~/Downloads
wget http://dlib.net/files/dlib-18.18.tar.bz2
mkdir -p ~/opt/dlib
cd ~/opt/dlib
tar -xf ~/Downloads/dlib-18.18.tar.bz2
mv dlib-18.18 18.18

#---------------------------

# For various analysis algorithms that use cryptographic functions
apt-get -y install libssl-dev libgcrypt11-dev 

#---------------------------

# For parsing XML files in certain tools such as roseHPCT and BinaryContextLookup.
apt-get -y install libxml2-dev 

#---------------------------

apt-get -y install libdwarf-dev

#---------- cleanup -----------

cd ~/
rm -r ~/Downloads/*

#---------- bashrc ----------

#.bashrc should have

# add jdk to PATH and LD_LIBRARY_PATH
#PATH=/home/demo/opt/jvm/jdk1.7.0_51/bin:$PATH
#LD_LIBRARY_PATH=/home/demo/opt/jvm/jdk1.7.0_51/jre/lib/amd64/server:/home/demo/opt/jvm/jdk1.7.0_51/lib:$LD_LIBRARY_PATH
# add boost to LD_LIBRARY_PATH
#LD_LIBRARY_PATH=/home/demo/opt/boost/1.61.0/gcc-4.9.3-default/lib:$LD_LIBRARY_PATH
# create alias for zgrviewer
#alias zgrviewer='/home/demo/opt/zgrviewer-0.10.0/run.sh'
#export PATH LD_LIBRARY_PATH

#sed -i '$ a # add jdk to PATH and LD_LIBRARY_PATH\nPATH=/home/demo/opt/jvm/jdk1.7.0_51/bin:$PATH\nLD_LIBRARY_PATH=/home/demo/opt/jvm/jdk1.7.0_51/jre/lib/amd64/server:/home/demo/opt/jvm/jdk1.7.0_51/lib:$LD_LIBRARY_PATH\n# add boost to LD_LIBRARY_PATH\nLD_LIBRARY_PATH=/home/demo/opt/boost/1.61.0/gcc-4.9.3-default/lib:$LD_LIBRARY_PATH\n# create alias for zgrviewer\nalias zgrviewer='/home/demo/opt/zgrviewer-0.10.0/run.sh'\nexport PATH LD_LIBRARY_PATH\n' $HOME/.bashrc


#---------- install rose ----------

cd ~/
git clone https://github.com/rose-compiler/rose-develop
cd rose-develop
./build
cd ..
mkdir build-rose
cd build-rose
CC=/usr/bin/gcc-4.9 CXX=g++-4.9 FC=/usr/bin/gfortran-4.9 CXXFLAGS='-g -rdynamic -Wall -Wno-unused-local-typedefs -Wno-attributes' /home/demo/rose-develop/configure --enable-assertion-behavior=abort --prefix=/home/demo/opt/rose_inst --with-CFLAGS=-fPIC --with-CXXFLAGS=-fPIC --with-C_OPTIMIZE=-O0 --with-CXX_OPTIMIZE=-O0 --with-C_DEBUG='-g -rdynamic' --with-CXX_DEBUG='-g -rdynamic' --with-C_WARNINGS='-Wall -Wno-unused-local-typedefs -Wno-attributes' --with-CXX_WARNINGS='-Wall -Wno-unused-local-typedefs -Wno-attributes' --with-ROSE_LONG_MAKE_CHECK_RULE=yes --with-boost=/home/demo/opt/boost/1.61.0/gcc-4.9.3-default --with-gfortran='/usr/bin/gfortran-4.9' --with-python='/usr/bin/python3' --with-java=/home/demo/opt/jvm/jdk1.7.0_51/bin/javac --enable-languages=all --enable-projects-directory --with-doxygen --without-sqlite3 --without-libreadline --without-magic --without-yaml --with-dlib='/home/demo/opt/dlib/18.18' --without-wt --without-yices --without-pch --enable-rosehpct --with-gomp_omp_runtime_library=/usr/lib/gcc/x86_64-linux-gnu/4.9/ --without-haskell --enable-edg_version=4.12
make core
make install-core

#---------- set.rose ----------

# create a file to set the rose environment

cd ~/
echo -e "ROSE_INS=/home/demo/opt/rose_inst\nPATH=\$PATH:\$ROSE_INS/bin\nLD_LIBRARY_PATH=\$LD_LIBRARY_PATH:$ROSE_INS/lib\nexport PATH LD_LIBRARY_PATH"  > set.rose

#------- using ROSE ----------

source set.rose
mkdir tests/
cd tests/
echo -e "void foo()\n{\n        int a = 0;\n        a += 1;\n        return a;\n}\n" > sample.c
identityTranslator -c sample.c
dotGenerator -c sample.c 

