project(Gomoku)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules")
set(CMAKE_CXX_FLAGS "-DSERVER -g -Wall -Wextra")
SET(SOURCES
main.cpp ResourceMgr.cpp Map.cpp Session.cpp MenuSession.cpp SelectModeSession.cpp OptionSession.cpp ManualSession.cpp Gomoku.cpp GameSession.cpp Player.cpp HumanPlayer.cpp  IAPlayer.cpp GameType.cpp bnet_server.cpp
)

SET(EXECUTABLE_NAME
Gomoku
)



add_executable (
${EXECUTABLE_NAME} ${SOURCES}
)
target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARY} -lsfml-graphics -lsfml-window -lsfml-system )
