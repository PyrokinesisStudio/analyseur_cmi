#pragma once

#include "common.h"

#include <array>
#include <regex>

/** The goal of this class is to analyze lexems and
 * indentify simple tokens as number, variable names, special chars or
 * language keywords.
 */
class LexicalAnalyzer
{
public:
	enum LexemeType {
		PONCTUATION,
		INTEGER,
		FLOAT,
		KEYWORD, // Must be before identifier.
		IDENTIFIER,
		OPERATOR,
		NUM_LEXEME_TYPE
	};

	class Lexeme
	{
	public:
		const std::string m_value;
		LexemeType m_type;
	};

	using LexemeList = std::vector<Lexeme>;

private:
	/** String of char used to separate the input string.
	 * Contrary to white char, these separator chars are kept.
	 */
	std::string m_separators;

	/// The regex to match all tokens type.
	std::array<std::regex, NUM_LEXEME_TYPE> m_regexes;

public:
	LexicalAnalyzer(std::istream& stream);

	LexemeList Process(const std::string& content);
};
