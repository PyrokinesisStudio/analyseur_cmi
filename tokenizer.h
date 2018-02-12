#pragma once

#include "common.h"

StringList SplitBlank(const std::string& content);
StringList Tokenize(const std::string& content, const StringSet& sep);
