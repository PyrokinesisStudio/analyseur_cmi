#pragma once

#include "common.h"

/** \brief Representation of an input lexeme, with its token and the type matching.
 */
class Lexeme
{
public:
	enum Type {
		PONCTUATION,
		INTEGER,
		FLOAT,
		KEYWORD, // Must be before identifier.
		IDENTIFIER,
		OPERATOR,
		NUM_LEXEME_TYPE
	};

private:
	std::string m_token;
	Type m_type;

public:
	Lexeme(const std::string& token, Type type);

	const std::string& GetToken() const;
	Type GetType() const;
};

using LexemeList = std::vector<Lexeme>;
