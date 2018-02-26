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
	enum TokenType {
		PONCTUATION,
		INTEGER,
		FLOAT,
		KEYWORD, // Must be before identifier.
		IDENTIFIER,
		OPERATOR,
		NUM_TOKEN
	};

	class Token
	{
	public:
		const std::string m_value;
		TokenType m_type;
	};

	using TokenList = std::vector<Token>;

private:
	/** String of char used to separate the input string.
	 * Contrary to white char, these separator chars are kept.
	 */
	std::string m_separators;

	/// The regex to match all tokens type.
	std::array<std::regex, NUM_TOKEN> m_regexes;

public:
	LexicalAnalyzer(std::istream& stream);

	TokenList Process(const std::string& content);
};
