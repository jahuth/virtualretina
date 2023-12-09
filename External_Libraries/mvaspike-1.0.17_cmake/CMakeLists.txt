
cmake_minimum_required(VERSION 2.6)

#------------------------------------------------------------
# CMakeLists.txt file to compile Mvaspike with CMake
#------------------------------------------------------------

# Fixing the problem of including library Mvaspike within VirtualRetina :
# - Mvaspike is not accessible online anymore, so we have to include a full
#   copy of Mvaspike within the VirtualRetina archive.
# - For this occasion, I replace the original Autoconf/automake installation
#   process of Mvaspike with a simpler and more versatile CMake installation.
#
# Adrien Wohrer, 2023

#------------------------------------------------------------
#    Reminders
#------------------------------------------------------------

# - xxx_SOURCE_DIR is the dir where CMakeLists.txt is present
# - xxx_BINARY_DIR is the dir from which you call cmake

#------------------------------------------------------------
# Project Name
#------------------------------------------------------------

PROJECT(mvaspike)
SET(mvaspike_VERSION 1.0.17)

#------------------------------------------------------------
# Compile output lib
#------------------------------------------------------------

SET( mvaspike_INCLUDE_DIR  ${mvaspike_SOURCE_DIR}/src )

INCLUDE_DIRECTORIES( ${mvaspike_INCLUDE_DIR} )

FILE(GLOB files "${mvaspike_INCLUDE_DIR}/*.cc")

SET (LIBRARY_OUTPUT_PATH
        ${mvaspike_SOURCE_DIR}/lib CACHE PATH
        "Output directory for built libraries")

ADD_LIBRARY ( mvaspike SHARED ${files})

#------------------------------------------------------------
# Install (locally)
#------------------------------------------------------------

SET(CMAKE_INSTALL_PREFIX ".")

FILE(GLOB headers "${mvaspike_INCLUDE_DIR}/*.h")

INSTALL(FILES ${headers} DESTINATION include/mvaspike)

#INSTALL(TARGETS mvaspike )

#---------------------------------------------------------------
# CMaking subdirectories
#---------------------------------------------------------------

# ADD_SUBDIRECTORY(examples)
# maybe later :)


