#The executables are here... get ready !

#------- A small library, because code is shared by different executables.

ADD_LIBRARY ( retina-exec SHARED
TestGanglionCell_main.cc 
Retina_main.cc
ReconstructRetina_main.cc
)

LINK_DIRECTORIES("${VirtualRetina_BINARY_DIR}/local/lib")


# for all executables, the order of inclusion of the 3 retina libraries is important !
# first retina_exec, then virtual_retina, then retina_tools, then external libs...

#------- Retina

ADD_EXECUTABLE(Retina
Retina.cc)

TARGET_LINK_LIBRARIES(Retina
retina-exec
virtualretina
retinatools
${MVASPIKE_LIB}
${XMLPARAM_LIB}
${LIBXML2_LIB}
${X11_LIBRARIES}
${HDF5_LIBRARIES}
)

#------- TestGanglionCell

ADD_EXECUTABLE(TestGanglionCell
TestGanglionCell.cc)

TARGET_LINK_LIBRARIES(TestGanglionCell
retina-exec
virtualretina
retinatools
${MVASPIKE_LIB}
${XMLPARAM_LIB}
${LIBXML2_LIB}
${X11_LIBRARIES}
${HDF5_LIBRARIES}
)

#------- shapleyVictor

ADD_EXECUTABLE(shapleyVictor
shapleyVictor.cc)

TARGET_LINK_LIBRARIES(shapleyVictor
retina-exec
virtualretina
retinatools
${MVASPIKE_LIB}
${XMLPARAM_LIB}
${LIBXML2_LIB}
${X11_LIBRARIES}
${HDF5_LIBRARIES}
)

#------- ReconstructRetina

ADD_EXECUTABLE(ReconstructRetina
ReconstructRetina.cc)

TARGET_LINK_LIBRARIES(ReconstructRetina
retina-exec
virtualretina
retinatools
${MVASPIKE_LIB}
${XMLPARAM_LIB}
${LIBXML2_LIB}
${X11_LIBRARIES}
)

#-------- viewVideo

ADD_EXECUTABLE(viewVideo
viewVideo.cc)

TARGET_LINK_LIBRARIES(viewVideo
retinatools
${X11_LIBRARIES}
)

#-------- Grating

ADD_EXECUTABLE(Grating
Grating.cc)

TARGET_LINK_LIBRARIES(Grating
retinatools
${X11_LIBRARIES}
)

#-------- lumImage

ADD_EXECUTABLE(lumImage
lumImage.cc)

TARGET_LINK_LIBRARIES(lumImage
retinatools
${X11_LIBRARIES}
)

