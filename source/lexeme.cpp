#include "lexeme.h"

const std::array<std::string, Lexeme::NUM_LEXEME_TYPE> Lexeme::typeNameTable = {
	"ponctuation", // PONCTUATION
	"integer", // INTEGER
	"float", // FLOAT
	"keyword", // KEYWORD
	"identifier", // IDENTIFIER
	"operator", // OPERATOR
};

Lexeme::Lexeme(const std::string& token, Lexeme::Type type)
	:m_token(token),
	m_type(type)
{
}

const std::string &Lexeme::GetToken() const
{
	return m_token;
}

Lexeme::Type Lexeme::GetType() const
{
	return m_type;
}

