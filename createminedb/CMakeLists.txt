set(CREATEMINEDB_SOURCES createminedb.cpp)

add_executable(createminedb
  ${CREATEMINEDB_SOURCES}
)

target_link_libraries(createminedb ${QT_QTCORE_LIBRARY} ${QT_QTSQL_LIBRARY})
