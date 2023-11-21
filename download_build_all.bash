#!/bin/bash

#FEEL FREE TO MODIFY THIS SHELL AT YOUR CONVENIENCE !!
# (FOR ROOT INSTALLATION, PARTIAL INSTALLATION, ETC... )

# NOTE: A number of file checks (labeled 'OPTIM_CHECK') are
# present, for optimization if you call this script several
# times in a row. 
# You may have to remove them if you modify this script
# towards a custom installation.

# This is the 2023 modification by Adrien Wohrer, including mvaspike directly in the archive
# (since the official mvaspike website has gone down)


# (0) Versions, variables, addresses
# ==================================

ROOTDIR=$PWD

VIRTUALRETINA=VirtualRetina
CIMG=CImg-git
MVASPIKE=mvaspike-1.0.17_cmake
XMLPARAM=xmlParameters++-1.1.1

# if necessary:
CMAKE_BINARY_DIST=cmake-2.6.4-Linux-i386
CMAKE_DOWNLOAD_ADDRESS=http://www.cmake.org/files/v2.6/cmake-2.6.4-Linux-i386.tar.gz


# (1) Find/install CMake (to configure xmlParameters++ and VirtualRetina)
# =======================================================================

echo
echo ----------------------------------------
echo 
echo
echo "Looking for CMAKE (standard compilation tool)"
echo
echo 
echo ----------------------------------------

set $(whereis -b cmake) # If present, sets $2=/path/to/cmake
CMAKE=$2

if [[ -n $CMAKE ]] # Found an installed CMake
  then
  echo Found CMAKE executable: $CMAKE

elif [[ -x $ROOTDIR/External_Libraries/$CMAKE_BINARY_DIST/bin/cmake ]] # OPTIM_CHECK
  then
  CMAKE=$ROOTDIR/External_Libraries/$CMAKE_BINARY_DIST/bin/cmake
  echo Found CMAKE executable: $CMAKE

else # Did not find CMake
  read -p "Could not find CMAKE in regular diretories. You can either:
(1) Install it LOCALLY in the retina archive (press '1' and enter).
(2) Leave this script, install CMAKE on your machine, and then come back (press anything else).
Enter your choice: " NUM
   
  if [[ $NUM != 1 ]]
    then
    echo "OK. See you soon!"
    exit
    
  else # Install CMake locally
    echo
    echo "OK. Installing CMAKE in External_Libraries/CMake"
    cd $ROOTDIR/External_Libraries
    wget $CMAKE_DOWNLOAD_ADDRESS
    tar -zxf $CMAKE_BINARY_DIST.tar.gz
    rm $CMAKE_BINARY_DIST.tar.gz*
    CMAKE=$ROOTDIR/External_Libraries/$CMAKE_BINARY_DIST/bin/cmake
  fi
fi


# (2) Download CImg
# =================

echo
echo ----------------------------------------
echo
echo
echo "Installing CIMG (image processing library) locally"
echo
echo
echo ----------------------------------------

cd $ROOTDIR/External_Libraries
if [[ ! -d $CIMG ]]  # OPTIM_CHECK
  then
  git clone https://github.com/dtschump/CImg.git $CIMG
else
  echo "Found CIMG rootdir: External_Libraries/"$CIMG
fi

# Create (or overwrite) the link 'CImg' in VirtualRetina/ext-lib-links
cd $ROOTDIR/$VIRTUALRETINA/ext-lib-links/
ln -Tsf ../../External_Libraries/$CIMG CImg


# (3) Install MvaSpike
# ====================

echo
echo ----------------------------------------
echo
echo
echo "Installing MVASPIKE (spiking network library) locally"
echo 
echo
echo ----------------------------------------

echo Entering External_Libraries/$MVASPIKE
cd $ROOTDIR/External_Libraries/$MVASPIKE

# Compile and install locally (in source directory):
if [[ -r CMakeCache.txt ]] # if a cache is present, remove it (to take possible changes in CMakeLists.txt into account)
  then rm CMakeCache.txt
fi
$CMAKE CMakeLists.txt
EXITSTATUS=$?
if [[ $EXITSTATUS == 0 ]] # Do not try to compile if CMake produced an error
  then
  make
  make install
fi

# Create (or overwrite) the link 'MvaSpike' in VirtualRetina/ext-lib-links:
cd $ROOTDIR/$VIRTUALRETINA/ext-lib-links/
ln -Tsf ../../External_Libraries/$MVASPIKE MvaSpike


# (4) Install xmlParameters++
# ===========================

echo
echo ----------------------------------------
echo
echo
echo "Installing xmlParameters++ locally (with CMake)"
echo 
echo
echo ----------------------------------------

echo Entering External_Libraries/$XMLPARAM
cd $ROOTDIR/External_Libraries/$XMLPARAM

# Compile and install locally (in source directory):
if [[ -r CMakeCache.txt ]] # if a cache is present, remove it (to take possible changes in CMakeLists.txt into account)
  then rm CMakeCache.txt
fi
$CMAKE -D CMAKE_INSTALL_PREFIX":"PATH=. CMakeLists.txt # modify the prefix to change installation dir (default /usr/local)
EXITSTATUS=$?
if [[ $EXITSTATUS == 0 ]] # Do not try to compile if CMake produced an error
  then
  make
  make install
fi

# Create (or overwrite) the link 'xmlParameters++' in VirtualRetina/ext-lib-links
cd $ROOTDIR/$VIRTUALRETINA/ext-lib-links/
ln -Tsf ../../External_Libraries/$XMLPARAM xmlParameters++


# (5) Finally, compile VirtualRetina
# ===================================

echo
echo ----------------------------------------
echo
echo
echo "Compiling and installing VirtualRetina (with CMake)"
echo
echo
echo ----------------------------------------

cd  $ROOTDIR/$VIRTUALRETINA

# Compile and install locally (in source directory):

if [[ -r CMakeCache.txt ]] # if a cache is present, remove it (to take possible changes in CMakeLists.txt into account)
  then rm CMakeCache.txt
fi
$CMAKE CMakeLists.txt
EXITSTATUS=$?
if [[ $EXITSTATUS == 0 ]] # Do not try to compile if CMake produced an error
  then
  make
fi

cd $ROOTDIR
echo

