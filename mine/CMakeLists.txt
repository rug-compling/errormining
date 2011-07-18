set(MINE_SOURCES ProgramOptions.cpp mine.cpp)
set(MINE_HEADERS ProgramOptions.hh)

add_executable(mineit
  ${MINE_HEADERS}
  ${MINE_SOURCES}
)

target_link_libraries(mineit mine)
