
cmake_minimum_required(VERSION 2.6)

#------------------------------------------------------------
# This is the VirtualRetina main CMakeLists.txt file
#------------------------------------------------------------

# Feel free to modify it in order to perform custom installation.
# However, concerning 'external' libraries MvaSpike, CImg and 
# xmlParameters++, your desires might be fulfilled more simply
# by changing the links in ext-lib-links/:
# For example, if MvaSpike is already installed on your machine
# in /usr/local, just set  ext-lib-links/MvaSpike --> /usr/local

#------------------------------------------------------------
#    Reminders
#------------------------------------------------------------

# - xxx_SOURCE_DIR is the dir where CMakeLists.txt is present
# - xxx_BINARY_DIR is the dir from which you call cmake

#------------------------------------------------------------
# Project Name
#------------------------------------------------------------

PROJECT(VirtualRetina)
SET(VirtualRetina_VERSION 2.2)



#######################################################################
# External Libraries
#######################################################################

#------------------------------------------------------------
# MvaSpike library
#------------------------------------------------------------

FIND_PATH(MVASPIKE_INCLUDE_PATH mvaspike.h
            "ext-lib-links/MvaSpike/include/mvaspike")

INCLUDE_DIRECTORIES("${MVASPIKE_INCLUDE_PATH}")

FIND_LIBRARY(MVASPIKE_LIB mvaspike
            "ext-lib-links/MvaSpike/lib")

LINK_DIRECTORIES("ext-lib-links/MvaSpike/lib")


#------------------------------------------------------------
# CImg library
#------------------------------------------------------------

FIND_PATH(CIMG_PATH CImg.h
            "ext-lib-links/CImg")

INCLUDE_DIRECTORIES("${CIMG_PATH}")


#------------------------------------------------------------
# xmlParameters++ library
#------------------------------------------------------------

FIND_PATH(XMLPARAM_INCLUDE_PATH xmlparameters.h
            "ext-lib-links/xmlParameters++/include/xmlparameters++")

INCLUDE_DIRECTORIES("${XMLPARAM_INCLUDE_PATH}")

FIND_LIBRARY(XMLPARAM_LIB xmlparameters++
            "ext-lib-links/xmlParameters++/lib")

LINK_DIRECTORIES("ext-lib-links/xmlParameters++/lib")


#------------------------------------------------------------
# libxml2 library
#------------------------------------------------------------

# This is a standard GNU library, so I only look for it in the root directories.
# If it is not installed on your machine, you must install it on your own :p

FIND_PATH(LIBXML2_INCLUDE_PATH  libxml/parser.h
            "/usr/include/libxml2"
            "/usr/local/include/libxml2")

INCLUDE_DIRECTORIES("${LIBXML2_INCLUDE_PATH}")

FIND_LIBRARY(LIBXML2_LIB xml2 
            "/usr/lib"
            "/usr/lib/x86_64-linux-gnu"
            "/usr/local/lib")

LINK_DIRECTORIES(
            "/usr/lib"
            "/usr/local/lib")


#------------------------------------------------------------
# X11 library
#------------------------------------------------------------

# This is a standard GNU library, so I only look for it in the root directories.
# If it is not installed on your machine, you must install it on your own :p

FIND_PATH(X11_PATH include/X11/Xlib.h
            "/usr")

INCLUDE_DIRECTORIES("${X11_PATH}/include")
LINK_DIRECTORIES("${X11_PATH}/lib")

SET(X11_LIBRARIES
        m
        pthread
        X11)

 
#---------------------------------------------------------------------------
# HDF5 library... accessed through the 'wrapper' for MvaSpike, mvaspike_hdf5
#    see VirtualRetina tutorial for this one ;-) Don't use unless necessary.
#---------------------------------------------------------------------------

OPTION( USE_HDF5 "Use hdf5 library to save spike times" OFF )

FIND_PATH(MVASPIKEHDF5_PATH hdf5memspikerecorder.h
           "../External_Libraries/mvaspike_hdf5"
           "../mvaspike_hdf5"
)

INCLUDE_DIRECTORIES("${MVASPIKEHDF5_PATH}")
FIND_LIBRARY(MVASPIKEHDF5_LIB mvaspike_hdf5 "${MVASPIKEHDF5_PATH}")
LINK_DIRECTORIES("${MVASPIKEHDF5_PATH}")

IF(USE_HDF5)
  ADD_DEFINITIONS (-DUSE_HDF5 )  
  SET(HDF5_LIBRARIES
       ${MVASPIKEHDF5_LIB})
ELSE(USE_HDF5)
  SET(HDF5_LIBRARIES)
ENDIF(USE_HDF5)



#######################################################################
# VirtualRetina
#######################################################################

#---------------------------------------------------------------
# EXECUTABLE OUTPUT PATH
#---------------------------------------------------------------

SET (EXECUTABLE_OUTPUT_PATH
        ${VirtualRetina_BINARY_DIR}/bin CACHE PATH
        "Output directory for built binaries")

SET (LIBRARY_OUTPUT_PATH
        ${VirtualRetina_BINARY_DIR}/lib CACHE PATH
        "Output directory for built libraries")

#---------------------------------------------------------------
# Precompile options
#---------------------------------------------------------------

#Indicate the absolute pathway for the VirtualRetina home directory
ADD_DEFINITIONS( -DROOTDIR="${VirtualRetina_BINARY_DIR}" )

#---------------------------------------------------------------
# ...And CMaking subdirectories
#---------------------------------------------------------------

# An option just for me...
OPTION( PERSO  "dirty option allowing me to work with the same CMakeLists.txt file. Forget it! " OFF)

ADD_SUBDIRECTORY(src)


###############################################################
# ERROR MESSAGES
###############################################################

# libxml2 and X11 : "Please install on your machine"
#---------------------------------------------------

IF( ${LIBXML2_INCLUDE_PATH} MATCHES "LIBXML2_INCLUDE_PATH-NOTFOUND" )
MESSAGE(SEND_ERROR
"libxml/parser.h was not found. Please install on your machine the *DEVELOPMENT HEADERS* for library libxml2 (or precise its location in CMakeLists.txt). [Under Ubuntu, this package is called libxml2-dev.]")
ENDIF( ${LIBXML2_INCLUDE_PATH} MATCHES "LIBXML2_INCLUDE_PATH-NOTFOUND" )

IF( ${X11_PATH} MATCHES "X11_PATH-NOTFOUND" )
MESSAGE(SEND_ERROR 
"X11/Xlib.h was not found. Please install on your machine the *DEVELOPMENT HEADERS* for the X11 client-side library (or precise their location in CMakeLists.txt). [Under Ubuntu, this package is called libx11-dev.]")
ENDIF( ${X11_PATH} MATCHES "X11_PATH-NOTFOUND" )


# xmlParameters, CImg, MvaSpike : "Check configuration"
#------------------------------------------------------

IF( ${XMLPARAM_INCLUDE_PATH} MATCHES "XMLPARAM_INCLUDE_PATH-NOTFOUND" OR ${XMLPARAM_LIB} MATCHES "XMLPARAM_LIB-NOTFOUND" )
MESSAGE(SEND_ERROR
"xmlParameters++ is not correctly installed. Make sure that: (1) It has been installed on your computer, (2) Link 'ext-lib-links/xmlParameters++' points to the installation directory of xmlParameters++.")
ENDIF( ${XMLPARAM_INCLUDE_PATH} MATCHES "XMLPARAM_INCLUDE_PATH-NOTFOUND" OR ${XMLPARAM_LIB} MATCHES "XMLPARAM_LIB-NOTFOUND" )

IF( ${MVASPIKE_INCLUDE_PATH} MATCHES "MVASPIKE_INCLUDE_PATH-NOTFOUND" OR ${MVASPIKE_LIB} MATCHES "MVASPIKE_LIB-NOTFOUND" )
MESSAGE(SEND_ERROR
"MvaSpike is not correctly installed. Make sure that: (1) It has been installed on your computer, (2) Link 'ext-lib-links/MvaSpike' points to the installation directory of MvaSpike.")
ENDIF( ${MVASPIKE_INCLUDE_PATH} MATCHES "MVASPIKE_INCLUDE_PATH-NOTFOUND" OR ${MVASPIKE_LIB} MATCHES "MVASPIKE_LIB-NOTFOUND" )

IF( ${CIMG_PATH} MATCHES "CIMG_PATH-NOTFOUND" )
MESSAGE(SEND_ERROR
"CImg.h was not found. Make sure that: (1) It is present on your computer, (2) Link "ext-lib-links/CImg" points to the CImg homedir.")
ENDIF( ${CIMG_PATH} MATCHES "CIMG_PATH-NOTFOUND" )


