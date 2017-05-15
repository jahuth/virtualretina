#The virtual_retina library is here

SET( VirtualRetina_INCLUDE_DIR  ${VirtualRetina_SOURCE_DIR}/src )

INCLUDE_DIRECTORIES( ${VirtualRetina_INCLUDE_DIR} )


ADD_LIBRARY ( virtualretina SHARED
basemapfilter.cc
baserecfilter.cc
basespikingchannel.cc
basespikingchannel_adap.cc
condcellmap.cc
fixationalmovement.cc
instantsynapse.cc
logpolarscheme.cc
pulse_coupled.cc
radialfilter.cc
radialscheme.cc
rd_sampling.cc
recfilter.cc
reconstruct.cc
retina.cc
retina_spikingchannels.cc
retinamapfilter.cc
spik_units.cc
sq_sampling.cc )

ADD_SUBDIRECTORY(retinatools)
ADD_SUBDIRECTORY(appli)


# My personal sources... not for the retina package. Yup, I'm lazy :-)

IF(PERSO)
ADD_SUBDIRECTORY( perso )
ENDIF(PERSO)
