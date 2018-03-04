#pragma once

#include <array>

#include "common.h"

/** \brief Representation of an input lexeme, with its token and the type matching.
 */
class Lexeme
{
public:
	enum Type {
		NONE = -1,
		PONCTUATION = 0,
		INTEGER,
		FLOAT,
		KEYWORD, // Must be before identifier.
		IDENTIFIER,
		OPERATOR,
		NUM_LEXEME_TYPE
	};

	/// The names corresponding to the types.
	static const std::array<std::string, NUM_LEXEME_TYPE> typeNameTable;

private:
	/// The string value of this lexeme aka token.
	std::string m_token;
	Type m_type;

public:
	Lexeme(const std::string& token, Type type);

	const std::string& GetToken() const;
	Type GetType() const;
};

using LexemeList = std::vector<Lexeme>;
