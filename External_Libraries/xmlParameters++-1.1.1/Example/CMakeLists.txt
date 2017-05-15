

ADD_EXECUTABLE(Test_Bus Test_Bus.cc)

TARGET_LINK_LIBRARIES(Test_Bus
${LIBXML2_LIB}
xmlparameters++
)


