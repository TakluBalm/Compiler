include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(main main.cpp lexer.cpp parser.cpp stateGenerator.cpp)