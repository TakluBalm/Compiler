cmake_minimum_required(VERSION 3.22.1)

project(Language_Recognizer C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_subdirectory(src)

add_custom_command(
    OUTPUT report
    COMMAND leaks --atExit --list -- src/main ${PROJECT_SOURCE_DIR}/test.txt > ${PROJECT_BINARY_DIR}/report
    DEPENDS test.txt main
    COMMENT "Leak Check"
    VERBATIM
)

add_custom_target(
    LeakCheck ALL
    DEPENDS report
)

# add_subdirectory(lib)