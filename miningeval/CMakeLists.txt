set(MININGEVAL_SOURCES miningeval.cpp)

add_executable(miningeval
  ${MININGEVAL_SOURCES}
)

target_link_libraries(miningeval ${QT_QTCORE_LIBRARY} ${QT_QTSQL_LIBRARY})
target_link_libraries(miningeval mine)
