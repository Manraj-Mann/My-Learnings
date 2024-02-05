cmake_minimum_required(VERSION 2.8.12)

set(FILE_NAME server_async)

project(${FILE_NAME})
set(CMAKE_CXX_FLAGS "-std=c++0x")

SET( DEFAULT_LIB_PATH /build_dependencies/muTrade )

# boost version
SET( BOOST_VERSION 1.54 )

SET( Boost_INCLUDE_DIR ${DEFAULT_LIB_PATH}/include/boost-1.54 )
SET( Boost_LIBRARY_DIR ${DEFAULT_LIB_PATH}/lib )

set(CMAKE_THREAD_LIBS_INIT "-lpthread")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")


find_package( Boost ${BOOST_VERSION} EXACT REQUIRED COMPONENTS
  system
  thread
  date_time
)


add_executable(${FILE_NAME}
  
${FILE_NAME}.cpp
  
)

target_link_libraries( ${FILE_NAME}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_THREAD_LIBRARY}
  ${Boost_DATE_TIME_LIBRARY}
)
