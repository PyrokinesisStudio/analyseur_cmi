cmake_minimum_required(VERSION 2.6)
project(Common)

set(SRCS
	grammar.cpp
	rule.cpp
	stack.cpp
	tokenizer.cpp
)

set(INCLS
	common.h
	grammar.h
	rule.h
	stack.h
	tokenizer.h
)

include_directories("../lib")
add_library(Common ${SRCS} ${INCLS})
