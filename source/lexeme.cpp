#include "lexeme.h"

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

