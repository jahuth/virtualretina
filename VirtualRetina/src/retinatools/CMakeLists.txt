#The tools are added to their own library

ADD_LIBRARY ( retinatools SHARED
cimg_tools.cc
misc_tools.cc
randomness.cc
signal_tools.cc
cimg_signal_drawer.cc)

#LINK_DIRECTORIES("${VirtualRetina_BINARY_DIR}/local/lib")
