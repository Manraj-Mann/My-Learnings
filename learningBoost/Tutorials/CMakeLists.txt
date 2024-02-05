cmake_minimum_required(VERSION 2.8.12)
project(main)
set(CMAKE_CXX_FLAGS "-std=c++0x")

SET( DEFAULT_LIB_PATH /build_dependencies/muTrade )

# boost version
SET( BOOST_VERSION 1.54 )

SET( Boost_INCLUDE_DIR ${DEFAULT_LIB_PATH}/include/boost-1.54 )
SET( Boost_LIBRARY_DIR ${DEFAULT_LIB_PATH}/lib )

find_package( Boost ${BOOST_VERSION} EXACT REQUIRED COMPONENTS
  system
  thread
  date_time
)


add_executable(main
  
synctimer.cpp
  
)

target_link_libraries( main
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_THREAD_LIBRARY}
  ${Boost_DATE_TIME_LIBRARY}
)
