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
	static const Lexeme empty;

	Lexeme(const std::string& token, const std::string& type);

	const std::string& GetToken() const;
	const std::string& GetType() const;

	inline bool operator==(const Lexeme& other) const
	{
		return (m_token == other.m_token && m_type == other.m_type);
	}
	inline bool operator!=(const Lexeme& other) const
	{
		return !(*this == other);
	}
};

using LexemeList = std::vector<Lexeme>;
