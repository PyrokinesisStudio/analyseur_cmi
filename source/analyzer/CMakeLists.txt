cmake_minimum_required(VERSION 2.6)
project(Analyzer)

set(SRCS
	lexical.cpp
	syntax.cpp
)

set(INCLS
	lexical.h
	syntax.h
)

include_directories("..")
include_directories("../../lib")
add_library(Analyzer ${SRCS} ${INCLS})
