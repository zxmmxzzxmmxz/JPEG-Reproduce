cmake_minimum_required(VERSION 2.8)
project( jhier )

set (CMAKE_BUILD_TYPE Debug)
set (CMAKE_EXE_LINKER_FLAGS -ldl)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ldl")

find_package( OpenCV REQUIRED )

include_directories( ${OpenCV_INCLUDE_DIR} )
include_directories( ${CMAKE_SOURCE_DIR} )

set (jhier_SRCS src/main.cpp src/yuv_extractor.cpp src/block_extractor.cpp src/dct.cpp src/quantizer.cpp src/zigzag_scanner.cpp src/encoder_decoder.cpp src/huffman.cpp)

add_definitions("-lpthread -std=c++0x -ldl")
add_executable( jhier ${jhier_SRCS})

target_link_libraries( jhier ${CMAKE_DL_LIBS} ${OpenCV_LIBS})


# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/doxyfile @ONLY)
add_custom_target(doc
${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doxyfile &&
make -C ../doc/latex
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
COMMENT "Generating ${PROJECT_NAME} documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)
