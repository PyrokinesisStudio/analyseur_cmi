#pragma once

#include <map>
#include <ostream>

#include "common.h"

class Grammar;

class Rule
{
public:
	class Lexeme
	{
	public:
		enum Type {
			TERMINAL,
			NON_TERMINAL,
			EMPTY
		} m_type;

		static const char names[3];

		std::string m_value;

		Lexeme() = default;
		Lexeme(const std::string& str);
	};

	using LexemeList = std::vector<Lexeme>;

private:
    std::vector<LexemeList> m_options;
	std::string m_name;

public:
	Rule() = default;
    Rule(const std::string& name, const StringList& content);

	const std::string& GetName() const;
    StringSet GetTerms() const;
	const std::vector<LexemeList>& GetOptions() const;

	void Print() const;

	bool Match(const Grammar& grammar, StringList::const_iterator& it, const StringList& tokens) const;
};

inline std::ostream& operator<< (std::ostream& out, const Rule::Lexeme& lexeme)
{
	out << lexeme.m_value << " (" << Rule::Lexeme::names[lexeme.m_type] << ")";
	return out;
}

inline std::ostream& operator<< (std::ostream& out, const Rule::LexemeList& list)
{
	out << list.front();
	for (unsigned int i = 1, size = list.size(); i < size; ++i) {
		out << " " << list[i];
	}
	return out;
}
