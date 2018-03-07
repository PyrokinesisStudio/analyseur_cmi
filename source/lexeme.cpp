#include "lexeme.h"

Lexeme::Lexeme(const std::string& token, const std::string& type)
	:m_token(token),
	m_type(type)
{
}

const std::string &Lexeme::GetToken() const
{
	return m_token;
}

const std::string& Lexeme::GetType() const
{
	return m_type;
}

