#pragma once

#include "common.h"
#include "lexeme.h"

#include <array>
#include <regex>

/** \brief The goal of this class is to analyze tokens and
 * indentify simple lexemes as number, variable names, special chars or
 * language keywords.
 */
class LexicalAnalyzer
{
private:
	/** String of char used to separate the input string.
	 * Contrary to white char, these separator chars are kept.
	 */
	std::string m_separators;

	/// The regex to match all tokens type.
	std::map<std::string, std::regex> m_regexes;

public:
	LexicalAnalyzer(std::istream& stream);

	LexemeList Process(const std::string& content);
};
