
cmake_minimum_required(VERSION 2.6)

#------------------------------------------------------------
#    Reminders
#------------------------------------------------------------

# - xxx_SOURCE_DIR is the dir where CMakeLists.txt is present
# - xxx_BINARY_DIR is the dir from which you call cmake
# - to install locally :
#     cmake -D CMAKE_INSTALL_PREFIX:PATH=. CMakeLists.txt
#     and then the usual : make / make install / make clean.

#------------------------------------------------------------
# Project Name
#------------------------------------------------------------

PROJECT(xmlParameters++)
SET(xmlParameters++_VERSION 1.1.1)


#------------------------------------------------------------
# libxml2 library
#------------------------------------------------------------

# This is a standard GNU library, so I only look for it in the root directories.
# If it is not installed on your machine, you must install it on your own :p

FIND_PATH(LIBXML2_INCLUDE_PATH  libxml/parser.h
            "/usr/include/libxml2"
            "/usr/local/include/libxml2"
)

INCLUDE_DIRECTORIES(
            "${LIBXML2_INCLUDE_PATH}"
)

FIND_LIBRARY(LIBXML2_LIB xml2 
            "/usr/lib"
            "/usr/local/lib"
            "/usr/lib/x86_64-linux-gnu"
            )

LINK_DIRECTORIES( 
            "/usr/lib"
            "/usr/local/lib"
            )

#---------------------------------------------------------------
#     OUTPUT FILES
#---------------------------------------------------------------

SET (LIBRARY_OUTPUT_PATH
        ${xmlParameters++_SOURCE_DIR}/src CACHE INTERNAL
        "Output directory for the library")

SET (EXECUTABLE_OUTPUT_PATH
        ${xmlParameters++_SOURCE_DIR}/Example CACHE INTERNAL
        "Output path for the test example executable")

INCLUDE_DIRECTORIES( 
${xmlParameters++_SOURCE_DIR}/src
${xmlParameters++_SOURCE_DIR}/Example
)

ADD_LIBRARY(xmlparameters++_stat STATIC
${xmlParameters++_SOURCE_DIR}/src/xmlparameters.cc
${xmlParameters++_SOURCE_DIR}/src/xmlobject_base.cc
)

ADD_LIBRARY(xmlparameters++ SHARED
${xmlParameters++_SOURCE_DIR}/src/xmlparameters.cc
${xmlParameters++_SOURCE_DIR}/src/xmlobject_base.cc
)
SET_PROPERTY(TARGET xmlparameters++ 
             PROPERTY SOVERSION ${xmlParameters++_VERSION})

ADD_SUBDIRECTORY(${xmlParameters++_SOURCE_DIR}/Example)


#---------------------------------------------------------------
#     default installation with 'make install'
#---------------------------------------------------------------

        
INSTALL(DIRECTORY ${xmlParameters++_SOURCE_DIR}/src/ DESTINATION include/xmlparameters++
          FILES_MATCHING PATTERN "*.h")

INSTALL(FILES ${xmlParameters++_SOURCE_DIR}/src/libxmlparameters++_stat.a
          DESTINATION lib RENAME libxmlparameters++.a)

INSTALL(TARGETS xmlparameters++
          LIBRARY DESTINATION lib )



###############################################################
# ERROR MESSAGES
###############################################################

# libxml2  : "Please install on your machine"
#---------------------------------------------------

IF( ${LIBXML2_INCLUDE_PATH} MATCHES "LIBXML2_INCLUDE_PATH-NOTFOUND" )
MESSAGE(SEND_ERROR
"libxml/parser.h was not found. Please install on your machine the *DEVELOPMENT HEADERS* for library libxml2 (or precise its location in CMakeLists.txt). [Under Ubuntu, this package is called libxml2-dev.]")
ENDIF( ${LIBXML2_INCLUDE_PATH} MATCHES "LIBXML2_INCLUDE_PATH-NOTFOUND" )


