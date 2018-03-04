#pragma once

#include <map>
#include <ostream>

#include "lexeme.h"

class Grammar;

class Rule
{
public:
	/**- \brief Condition of a rule option for a lexeme.
	 */
	class Condition
	{
	public:
		enum Type {
			/// A specific terminal such as ponctuation.
			TERMINAL,
			// A group of terminals such as number, identifier… as defined in the lexical analyzer.
			TERMINAL_TYPE,
			// A link to a grammar rule.
			NON_TERMINAL,
			// An optional empty rule, sometimes used to break a recursion.
			EMPTY,
			NUM_LEXEME_TYPE
		} m_type;

		static const char names[NUM_LEXEME_TYPE];

		std::string m_value;
		// Initialized if the condition is for terminal type.
		Lexeme::Type m_terminalType;

		Condition() = default;
		Condition(const std::string& token);

		bool Match(const Lexeme& lexeme) const;
	};

	using ConditionList = std::vector<Condition>;
	using ProposalList = std::vector<ConditionList>;

private:
    ProposalList m_proposals;
	std::string m_name;

public:
	Rule() = default;
    Rule(const std::string& name, const StringList& tokens);

	const std::string& GetName() const;
	const ProposalList& GetProposals() const;

	void Print() const;
};

inline std::ostream& operator<< (std::ostream& out, const Rule::Condition& cond)
{
	out << cond.m_value << " (" << Rule::Condition::names[cond.m_type] << ")";
	return out;
}

inline std::ostream& operator<< (std::ostream& out, const Rule::ConditionList& list)
{
	out << list.front();
	for (unsigned int i = 1, size = list.size(); i < size; ++i) {
		out << " " << list[i];
	}
	return out;
}
