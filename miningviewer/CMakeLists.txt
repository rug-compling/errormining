list(APPEND CMAKE_MODULE_PATH "${errormining_SOURCE_DIR}/cmake")

find_package(Sqlite3 REQUIRED)
if(SQLITE3_FOUND)
  include_directories(${SQLITE3_INCLUDE_DIR})
endif()

include_directories (${CMAKE_CURRENT_BINARY_DIR})

set(miningviewer_SOURCES
  FormTreeWidgetItem.cpp
  MinerMainWindow.cpp
  PreferencesDialog.cpp  
  miningviewer.cpp
  )

set(miningviewer_MOC_HDRS
  MinerMainWindow.hh
  PreferencesDialog.hh
)

set(miningviewer_UI
  MinerMainWindow.ui
  PreferencesDialog.ui
)

qt4_wrap_ui(miningviewer_UI_HDRS ${miningviewer_UI})
qt4_wrap_cpp(miningviewer_MOC_SRCS ${miningviewer_MOC_HDRS})


add_executable(miningviewer
  ${miningviewer_SOURCES}
  ${miningviewer_UI_HDRS}
  ${miningviewer_MOC_SRCS}
)

target_link_libraries(miningviewer
  ${QT_QTCORE_LIBRARY}
  ${QT_QTGUI_LIBRARY}
  ${QT_QTSQL_LIBRARY}
)

target_link_libraries(miningviewer mine)

target_link_libraries(miningviewer ${SQLITE3_LIBRARIES})
