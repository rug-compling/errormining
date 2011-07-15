cmake_minimum_required(VERSION 2.6)
project(errormining)

if (NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE Release)
endif (NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)

find_package(Qt4 COMPONENTS QtCore QtGui QtSql REQUIRED)
include(${QT_USE_FILE})

include_directories (${errormining_SOURCE_DIR}/libmine)

set(CMAKE_CXX_FLAGS "-DFLEXIBLE -DNUMBERS -DSTOPBIT -DNEXTBIT -DMORPH_INFIX -DPOOR_MORPH -DLOOSING_RPM -DMULTICOLUMN")

add_subdirectory(libmine)
add_subdirectory(mine)
