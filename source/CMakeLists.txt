cmake_minimum_required(VERSION 2.6)
project(Common)

set(CMAKE_CXX_FLAGS "-g -ggdb")

set(SRCS
	abstract_tree.cpp
	derivation_tree.cpp
	grammar.cpp
	lexeme.cpp
	rule.cpp
	stack.cpp
	tokenizer.cpp
)

set(INCLS
	abstract_tree.h
	common.h
	derivation_tree.h
	grammar.h
	lexeme.h
	rule.h
	stack.h
	tokenizer.h
)

include_directories("../lib")
add_library(Common ${SRCS} ${INCLS})
