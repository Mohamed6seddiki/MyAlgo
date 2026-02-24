cmake_minimum_required(VERSION 3.16)

project(MyAlgo VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(myalgo
	src/main.cpp
	src/lexer/lexer.cpp
	src/parser/parser.cpp
)

target_include_directories(myalgo PRIVATE
	src
	src/core
	src/AST
	src/lexer
	src/parser
	src/semantic
	src/codegen
)

if(MSVC)
	target_compile_options(myalgo PRIVATE /W4)
else()
	target_compile_options(myalgo PRIVATE -Wall -Wextra -Wpedantic)
endif()
