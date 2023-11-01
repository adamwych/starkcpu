cmake_minimum_required(VERSION 3.17)
project(emulator C)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "/O3")

set(CMAKE_C_STANDARD 11)
add_compile_definitions(emulator QUICK_INT_READ)

add_executable(emulator main.c cpu.c cpu-executor.c cpu-ui.c utils.c map.c opcode-handlers-map.c)

if (WIN32)
    target_link_libraries(emulator "../PDCurses-3.8/wincon/pdcurses")
    target_include_directories(emulator PUBLIC "../PDCurses-3.8")
else ()
    find_package(Curses REQUIRED)
    target_link_libraries(emulator ${CURSES_LIBRARY})
endif()
