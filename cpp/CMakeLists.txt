cmake_minimum_required(VERSION 2.8)
project( IntegralCoocMatrix )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package( OpenCV REQUIRED )
include_directories(${OpenCV_INCLUDE_DIRS})

set(Boost_USE_STATIC_LIBS       ON)
set(Boost_USE_MULTITHREADED     ON)
set(Boost_USE_STATIC_RUNTIME    ON)
find_package(Boost COMPONENTS program_options system regex REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

find_package(Log4Cxx REQUIRED)
include_directories(${LOG4CXX_INCLUDE_DIR})

add_executable( standard
	standard.cpp
	boost_program_options_types.cpp
	ParseUtils.cpp
	cli_parser.cpp
	GLCM.h
	GLCM.hxx
	)

add_executable( integral
	integral.cpp
	boost_program_options_types.cpp
	ParseUtils.cpp
	cli_parser.cpp
	)
target_link_libraries( standard ${OpenCV_LIBS} ${Boost_LIBRARIES} ${LOG4CXX_LIBRARIES} )
target_link_libraries( integral ${OpenCV_LIBS} ${Boost_LIBRARIES} ${LOG4CXX_LIBRARIES} )
