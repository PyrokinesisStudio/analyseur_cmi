#pragma once

#include <array>

#include "common.h"

/** \brief Representation of an input lexeme, with its token and the type matching.
 */
class Lexeme
{
private:
	/// The string value of this lexeme aka token.
	std::string m_token;
	/// Type name from lexical rule definition.
	std::string m_type;

public:
	Lexeme(const std::string& token, const std::string& type);

	const std::string& GetToken() const;
	const std::string& GetType() const;
};

using LexemeList = std::vector<Lexeme>;
